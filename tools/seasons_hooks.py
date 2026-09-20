#!/usr/bin/env python3
"""Write src/hooks/generated_seasons.txt: the hooks that are safe to run under Oracle of Seasons.

usage: tools/seasons_hooks.py AGES.gbc ages.sym SEASONS.gbc seasons.sym

A hook is eligible when its routine is instruction-identical in Seasons (tools/routine_equiv.py)
and every C function it calls directly (a `foo_hook(gb)` tail call or a static helper in the same
file, transitively) is eligible too; CALL_C targets are checked against the hook table at run
time, so they do not matter here. The Seasons (bank, address) of each eligible hook comes from
seasons.sym, pairing multi-copy labels by bank order. Then run
tools/gen_hooks.py seasons.sym src/hooks/generated_seasons.txt src/hooks/table_seasons.h.
"""
import glob, os, re, subprocess, sys
from collections import defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from symfiles import rom_labels

FUNC = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?(\w+)\([^)]*\)\s*\{')
CALL = re.compile(r'\b(\w+)\(gb[,)]')


def main():
    ages_rom, ages_sym, seasons_rom, seasons_sym = sys.argv[1:5]
    tsv = '/tmp/routine_equiv.tsv'
    subprocess.run([sys.executable, 'tools/routine_equiv.py', ages_rom, ages_sym, seasons_rom, seasons_sym, '--tsv', tsv], check=True, stdout=subprocess.DEVNULL)
    verdict = {}
    for l in open(tsv):
        n, v, *_ = l.rstrip('\n').split('\t')
        verdict[n] = v

    # C call graph: function name -> callees, per file (static helpers are file-local)
    callees, burns = {}, {}
    EXEC = re.compile(r'\b(?:CYCT?[0-9a-f]*|burn_rom|RET|RET_TAKEN|RETI|I|push_effect|BASE)\(([^;]*)')
    for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
        if os.path.basename(path).startswith('gen_') or os.path.basename(path) == 'syms.c': continue
        cur = None
        for line in open(path, errors='replace'):
            m = FUNC.match(line)
            if m:
                cur = (path, m.group(1)); callees[cur] = set(); burns[cur] = set()
                line = line[m.end():]
            elif line.startswith('}'): cur = None; continue
            if cur is None: continue
            code = line.split('//')[0]
            if m and code.count('}') > code.count('{'): oneliner = True
            else: oneliner = False
            for c in CALL.findall(code): callees[cur].add(c)
            for args in EXEC.findall(code):
                for lab in re.findall(r'\bSYM\((\w+)\)', args): burns[cur].add(lab)
                bm = re.match(r'\s*(\w+)\)', args)
                if bm and 'BASE(' in code: burns[cur].add(bm.group(1))
            if oneliner: cur = None
    by_name = defaultdict(list)
    for (path, fn) in callees: by_name[fn].append(path)

    def routine_of(fn):
        base = fn[:-5] if fn.endswith('_hook') else None
        if base is None: return None
        base = base.replace('__', '@').split('@')[0]
        return base

    def hook_verdict(fn):
        base = fn[:-5]
        for cand in (base, base.replace('__', '@').split('@')[0], re.sub(r'_b[0-9a-f]{2}$', '', base.replace('__', '@').split('@')[0])):
            if cand in verdict: return verdict[cand]
        return None

    seasons_names = set(rom_labels(seasons_sym))
    unpairable = re.compile(r'^(_label_[0-9a-f]{2}_\d+|label_[0-9a-f]{2}_\d+)')

    def label_verdict(sid):
        name = re.sub(r'_b[0-9a-f]{2}$', '', sid).replace('__', '@')
        if unpairable.match(name) or name not in seasons_names: return 'AGES_ONLY'
        return verdict.get(name.split('@')[0])

    eligible = {}
    for key, fn in ((k, k[1]) for k in callees):
        ok = hook_verdict(fn) == 'IDENTICAL' if fn.endswith('_hook') else True
        for lab in burns[key]:
            v = label_verdict(lab)
            if v is not None and v != 'IDENTICAL': ok = False
        eligible[key] = ok
    changed = True
    while changed:
        changed = False
        for key, cs in callees.items():
            if not eligible[key]: continue
            path, fn = key
            for c in cs:
                if (path, c) in callees: ok = eligible[(path, c)]
                elif c.endswith('_hook'):
                    ok = any(eligible.get((p, c), False) for p in by_name.get(c, [])) if by_name.get(c) else False
                else: ok = True   # engine/core helpers (mem_rd, alu_*, push_effect...) are game-neutral
                if not ok:
                    eligible[key] = False; changed = True; break

    ages, seasons = rom_labels(ages_sym), rom_labels(seasons_sym)
    rows, skipped = [], defaultdict(int)
    for l in open('src/hooks/generated.txt'):
        p = l.split()
        if len(p) < 2: continue
        bank, addr = int(p[0][:2], 16), int(p[0][3:], 16)
        fn, flags = p[1], (p[2] if len(p) > 2 else '-')
        if not fn.endswith('_hook'): skipped['not a hook'] += 1; continue
        if not any(eligible.get((path, fn), False) for path in by_name.get(fn, [])):
            skipped['not eligible'] += 1; continue
        name = [n for n, inst in ages.items() if (bank, addr) in inst]
        name = [n for n in name if n.replace('@', '__') in fn] or name
        if not name: skipped['no label'] += 1; continue
        name = name[0]
        parent = name.split('@')[0]
        a_inst = ages.get(parent, [])
        rank = [b for b, a in a_inst].index(bank) if bank in [b for b, a in a_inst] else 0
        s_inst = seasons.get(name, [])
        if re.match(r'^(_label_[0-9a-f]{2}_\d+|label_[0-9a-f]{2}_\d+)', parent): s_inst = []
        if len(s_inst) == len(a_inst) and s_inst: s = s_inst[rank]
        elif len(s_inst) == 1: s = s_inst[0]
        else: skipped['no seasons label'] += 1; continue
        rows.append((s[0], s[1], fn, flags))
    rows.sort()
    with open('src/hooks/generated_seasons.txt', 'w') as f:
        for b, a, fn, fl in rows: f.write(f'{b:02x}:{a:04x} {fn} {fl}\n')
    print(f'{len(rows)} hooks eligible for Seasons; skipped: {dict(skipped)}')


if __name__ == '__main__':
    main()
