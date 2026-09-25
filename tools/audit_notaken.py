#!/usr/bin/env python3
"""Find a conditional jr/jp/ret whose taken side is `if (...) { CYCT(b_+X, b_+Y); <leave> }` but
whose fall-through never burns b_+X: no CYC/CYCT/CALL_C starting at b_+X on that line's tail or in
the next few lines. Only branches whose CYCT range is exactly the one instruction (checked against
the Ages ROM) are reported; older multi-instruction ranges are left alone.
usage: tools/audit_notaken.py"""
import glob, re
ROM = open('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_ages\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}
CC = {0x20: 2, 0x28: 2, 0x30: 2, 0x38: 2, 0xc2: 3, 0xca: 3, 0xd2: 3, 0xda: 3, 0xc0: 1, 0xc8: 1, 0xd0: 1, 0xd8: 1}
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or '/seasons/' in p or p.endswith(('syms.c', 'ofs.c')): continue
    L = open(p, errors='replace').read().split('\n'); base = None
    for i, l in enumerate(L):
        bm = re.search(r'\bBASE\((\w+)\)', l)
        if bm: base = sym.get(bm.group(1))
        if l.startswith('}'): base = None
        c = l.split('//')[0]
        m = re.match(r'\s*if \(([^{}]*)\) \{ CYCT\(b_\+(\d+), b_\+(\d+)\);([^{}]*)\}\s*(.*)$', c)
        if not m or base is None: continue
        x, y = int(m.group(2)), int(m.group(3))
        if not re.search(r'return;|goto|continue|break|TAIL', m.group(4)): continue
        b, a = base >> 16, (base & 0xffff) + x
        fo = a if a < 0x4000 else b * 0x4000 + a - 0x4000
        if CC.get(ROM[fo]) != y - x: continue
        after = m.group(5) + ' ' + ' '.join(t.split('//')[0] for t in L[i + 1:i + 5])
        if re.search(rf'\b(?:CYCT?|CALL_C|CALL_C_CC|CALL_ROM)\(b_\+{x},', after) or re.match(r'\s*else', m.group(5)): continue
        bad += 1; print(f'{p}:{i + 1}: the fall-through of b_+{x} ({ROM[fo]:02x}) is never burned')
print(f'{bad} conditional branches with an unburned fall-through')
