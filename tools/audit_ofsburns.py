#!/usr/bin/env python3
"""Review list: a Seasons offset burned both by a shared line (`b_+O(a), b_+OE(b)` through the
routine's src/game/ofs.c table) and by a Seasons-only line (`b_+S(u), b_+S(v)`) in the same
function. That is how a wrong ofsmap pairing shows (the shared line lands on an instruction the
Seasons block also burns); cycles often still match, so shadow verify only notices when a value
differs. Lines the Seasons run never reaches (after a Seasons branch that always returns) also
show up. usage: tools/audit_ofsburns.py"""
import glob, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from symfiles import seasons_code
tabs = {}
for m in re.finditer(r'ofs_(\w+)\[\d+\] = \{([^}]*)\}', open('src/game/ofs.c').read()):
    v = [int(x, 0) for x in m.group(2).split(',')]; n = v[0]; tabs[m.group(1)] = (v[1:n + 1], v[n + 1:])
FUNC = re.compile(r'^(?:static )?\w+ \*?(\w+)\([^)]*\)\s*\{')
hits = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/seasons/' in p or '/gen_' in p or p.endswith(('ofs.c', 'syms.c')): continue
    src = open(p, errors='replace').read().split('\n')
    live = seasons_code(src)
    fn, base, O, S_ = None, None, [], []
    def flush():
        global hits
        for x, y, a, b, ln in O:
            for u, v, ln2 in S_:
                if x < v and u < y:
                    hits += 1; print(f'{p}:{ln}: {fn}: O({a})..OE({b}) is Seasons {x}..{y}, also burned by S({u})..S({v}) at line {ln2}'); break
    for i, (line, sc) in enumerate(zip(src, live), 1):
        m = FUNC.match(line)
        if m or line.startswith('}'):
            if fn: flush()
            fn, base, O, S_ = (m.group(1) if m else None), None, [], []
            continue
        bm = re.search(r'\bBASE\((\w+)\)', line)
        if bm: base = bm.group(1)
        if base not in tabs: continue
        st, en = tabs[base]
        for a, b in re.findall(r'CYCT?\(b_\+O\((\d+)\), b_\+OE\((\d+)\)\)', sc):
            a, b = int(a), int(b)
            if a < len(st) and b < len(en) and st[a] != 0xffff and en[b] != 0xffff: O.append((st[a], en[b], a, b, i))
        for u, v in re.findall(r'CYCT?\(b_\+S\((\d+)\), b_\+S\((\d+)\)\)', sc): S_.append((int(u), int(v), i))
print(f'{hits} shared burns overlapping a Seasons block')
