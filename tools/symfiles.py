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


def extra_sym_for(sym):
    """The hand-written extra labels that go with a symbol file (vectors, code run from RAM)."""
    return 'src/hooks/extra_seasons.sym' if 'seasons' in sym else 'src/hooks/extra.sym'


def extra_labels(sym):
    """[(bank, addr, name, source (bank, addr) or None, extern)] from the extra file."""
    import os
    out = []
    paths = [extra_sym_for(sym)] + (['src/hooks/alias_seasons.sym'] if 'seasons' in sym else [])
    for path in paths:
        if not os.path.exists(path): continue
        for line in open(path):
            line = line.split('#')[0].strip()
            m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)(?: = ([0-9a-f]{2}):([0-9a-f]{4})| (extern))?$', line)
            if not m: continue
            out.append((int(m.group(1), 16), int(m.group(2), 16), m.group(3), (int(m.group(4), 16), int(m.group(5), 16)) if m.group(4) else None, bool(m.group(6))))
    return out


def rom_labels(sym, extras=True):
    """label name -> [(bank, addr)] sorted by bank, for every ROM label including @locals, plus
    the extra labels (RAM code keeps its RAM address)."""
    labels = {}
    for line in open(sym):
        m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
        if not m: continue
        b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
        if a >= 0x8000: continue
        lst = labels.setdefault(n, [])
        if (b, a) not in lst: lst.append((b, a))
    for b, a, n, src, ext in (extra_labels(sym) if extras else []):
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
    # unnamed code (`_label_XX_N`, `label_XX_N`: numbered per game, so never by name): paired
    # through the references of paired routines whose bodies have the same shape
    votes = {}
    for (name, ab, aa), (sb, sa) in list(pairs.items()):
        if '@' in name or UNNAMED.match(name): continue
        try: ba, bs = A.body(ab, aa), S.body(sb, sa)
        except Exception: continue
        if len(ba) != len(bs): continue
        for (xa, la, ka, ta, oa), (xs, ls, ks, ts, os_) in zip(ba, bs):
            if ta != ts or len(oa) != len(os_): continue
            for (k1, v1), (k2, v2) in zip(oa, os_):
                if k1 != 'rom' or v1 >= 0x8000 or v2 >= 0x8000: continue
                tba, tbs = (0 if v1 < 0x4000 else ab), (0 if v2 < 0x4000 else sb)
                na, ns = A.by_addr.get((tba, v1)), S.by_addr.get((tbs, v2))
                if na and ns and UNNAMED.match(na) and UNNAMED.match(ns):
                    votes.setdefault((na, tba, v1), {}).setdefault((tbs, v2), 0)
                    votes[(na, tba, v1)][(tbs, v2)] += 1
    for key, cands in votes.items():
        if len(cands) == 1 and key not in pairs: pairs[key] = next(iter(cands))
    return pairs


UNNAMED = re.compile(r'^(_label_[0-9a-f]{2}_\d+|label_[0-9a-f]{2}_\d+)$')


def seasons_code(lines):
    """For each C source line, the part that runs under Seasons: text inside
    `if (!game_seasons) { ... }` blocks, the else branch of `if (game_seasons) { ... } else {`,
    one-line `if (!game_seasons) ...;` / `!game_seasons && ...` conditions and the rest of a
    function that opens with `AGES_ONLY();` is blanked."""
    out, ages_depth, seasons_depth, ages_fn = [], 0, 0, False
    for line in lines:
        code = line.split('//')[0]
        if 'AGES_ONLY();' in code: ages_fn = True
        if ages_fn:
            if line.startswith('}'): ages_fn = False
            out.append(''); continue
        keep = ''
        if ages_depth:
            j = 0
            while j < len(code) and ages_depth:
                if code[j] == '{': ages_depth += 1
                elif code[j] == '}': ages_depth -= 1
                j += 1
            if ages_depth: out.append(''); continue
            code = code[j:]
        if re.search(r'!game_seasons\s*&&', code) or re.search(r'if \(!game_seasons\)\s*[^{\s]', code):
            out.append(''); continue
        if seasons_depth:
            j = 0
            while j < len(code) and seasons_depth:
                if code[j] == '{': seasons_depth += 1
                elif code[j] == '}': seasons_depth -= 1
                j += 1
            if not seasons_depth and re.match(r'\s*else\s*\{', code[j:]):
                ages_depth = 1; out.append(code[:j]); continue
        m3 = re.search(r'if \(game_seasons\)\s*\{', code)
        if m3:
            j = m3.end(); seasons_depth = 1
            while j < len(code) and seasons_depth:
                if code[j] == '{': seasons_depth += 1
                elif code[j] == '}': seasons_depth -= 1
                j += 1
            if not seasons_depth and re.match(r'\s*else\s*\{', code[j:]):
                ages_depth = 1; out.append(code[:j]); continue
        m2 = re.search(r'if \(!game_seasons\)\s*\{', code)
        if m2:
            j = m2.end(); ages_depth = 1
            while j < len(code) and ages_depth:
                if code[j] == '{': ages_depth += 1
                elif code[j] == '}': ages_depth -= 1
                j += 1
            out.append(code[:m2.start()] + (code[j:] if not ages_depth else '')); continue
        out.append(code)
    return out
