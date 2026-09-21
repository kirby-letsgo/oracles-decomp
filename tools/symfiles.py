"""Shared parsing of the disassembly's .sym files for the per-game address tools."""
import re


def ram_map(sym, generated='src/hooks/generated.txt'):
    """C name -> (wram bank, address) for every WRAM/HRAM label, the way ram.h names them."""
    code = set()
    for l in open(generated):
        p = l.split()
        if len(p) >= 2: code.add(p[1].split('__')[0])
    ram = {}
    section = None
    for line in open(sym):
        line = line.strip()
        if line.startswith('['): section = line; continue
        if section not in ('[labels]', '[definitions]') or not line: continue
        if section == '[labels]':
            m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line)
            if not m: continue
            bank, addr, name = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
        else:
            m = re.match(r'([0-9a-f]{8}) (\S+)$', line)
            if not m: continue
            addr, name = int(m.group(1), 16), m.group(2)
            mb = re.match(r'w([1-7])[A-Z]', name)
            bank = int(mb.group(1)) if mb else (0 if addr < 0xd000 or addr >= 0xff80 else None)
            if bank is None: continue
        if '@' in name or not re.match(r'[wh][A-Za-z0-9]', name): continue
        if not (0xc000 <= addr < 0xe000 or 0xff80 <= addr < 0xffff): continue
        cn = name.replace('.', '_')
        if cn.split('_')[0] in code: continue
        ram.setdefault(cn, (bank if addr >= 0xd000 else 0, addr))
    return ram


def rom_labels(sym):
    """label name -> [(bank, addr)] sorted by bank, for every ROM label including @locals."""
    labels = {}
    for line in open(sym):
        m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
        if not m: continue
        b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
        if a >= 0x8000: continue
        lst = labels.setdefault(n, [])
        if (b, a) not in lst: lst.append((b, a))
    for n in labels: labels[n].sort()
    return labels


def pair_instances(ages_rom, ages_sym, seasons_rom, seasons_sym):
    """(label name, ages bank, addr) -> (seasons bank, addr) for every ROM label, pairing copies of a
    label by content (normalized instruction stream), then by same bank, then by bank order. Keyed
    by name as well as address because several labels can share an Ages address."""
    import os, sys
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from routine_equiv import Game
    A, S = Game(ages_rom, ages_sym), Game(seasons_rom, seasons_sym)
    al, sl = rom_labels(ages_sym), rom_labels(seasons_sym)
    pairs = {}
    parent_pairs = {}      # (parent name, ages bank) -> seasons bank
    for name, ainst in al.items():
        if '@' in name: continue
        sinst = sl.get(name)
        if not sinst: continue
        if len(ainst) == 1 and len(sinst) == 1:
            pairs[(name,) + ainst[0]] = sinst[0]; parent_pairs[(name, ainst[0][0])] = sinst[0][0]; continue
        def norm(g, b, a):
            try: return g.normalized(b, a)[0]
            except Exception: return None
        sn = {s: norm(S, *s) for s in sinst}
        taken = set()
        for a in ainst:
            an = norm(A, *a)
            cands = [s for s in sinst if s not in taken]
            best = [s for s in cands if an is not None and sn[s] == an] or \
                   [s for s in cands if an is not None and sn[s] is not None and [x[1].split(' ')[0] for x in sn[s]] == [x[1].split(' ')[0] for x in an]] or \
                   [s for s in cands if s[0] == a[0]] or cands
            if not best: continue
            s = best[0]; taken.add(s)
            pairs[(name,) + a] = s; parent_pairs[(name, a[0])] = s[0]
    for name, ainst in al.items():
        if '@' not in name: continue
        parent = name.split('@')[0]
        sinst = sl.get(name)
        if not sinst: continue
        for a in ainst:
            sb = parent_pairs.get((parent, a[0]))
            cand = [s for s in sinst if s[0] == sb] if sb is not None else []
            if not cand and len(sinst) == len(ainst): cand = [sinst[ainst.index(a)]]
            if cand: pairs[(name,) + a] = cand[0]
    return pairs
