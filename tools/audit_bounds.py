#!/usr/bin/env python3
"""Check that every burn range lands on instruction boundaries: for `CYC(b_+X, b_+Y)` /
`CYCT(...)` / `CALL_C(b_+X, ...)` / `RET(b_+X)` in a function with `BASE(label)`, decoding the ROM
from BASE+X must reach BASE+Y exactly (a CALL_C is one 3-byte call, a RET one 1-byte ret). A
range that overshoots or undershoots means an instruction length was misread, which shifts every
later burn of the routine. usage: tools/audit_bounds.py [--game=seasons] [FILE...]
Without --game it checks the Ages-side files against the Ages ROM; with it, the hand-written
Seasons files (src/game/seasons/, not gen_*) against the Seasons ROM."""
import glob, re, sys
SEASONS = '--game=seasons' in sys.argv
files = [a for a in sys.argv[1:] if not a.startswith('--')]
ROM = open('roms/Legend of Zelda, The - Oracle of ' + ('Seasons' if SEASONS else 'Ages') + ' (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_' + ('seasons' if SEASONS else 'ages') + r'\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}

def length(op, nxt):
    if op == 0xcb: return 2
    if op in (0x01, 0x11, 0x21, 0x31, 0x08, 0xc2, 0xc3, 0xc4, 0xca, 0xcc, 0xcd, 0xd2, 0xd4, 0xda, 0xdc, 0xea, 0xfa): return 3
    if op in (0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e, 0x18, 0x20, 0x28, 0x30, 0x38, 0xc6, 0xce, 0xd6, 0xde,
              0xe6, 0xee, 0xf6, 0xfe, 0xe0, 0xf0, 0xe8, 0xf8, 0x10): return 2
    return 1

def rd(base, off):
    bank, addr = base >> 16, (base & 0xffff) + off
    return ROM[addr if addr < 0x4000 else bank * 0x4000 + addr - 0x4000]

def reaches(base, x, y):
    a = x
    for _ in range(64):
        if a == y: return True
        if a > y: return False
        a += length(rd(base, a), rd(base, a + 1))
    return False

BURN = re.compile(r'\b(CYCT?)\(b_\+(\d+), b_\+(\d+)\)|\bCALL_[CL](?:_CC)?\(b_\+(\d+)|\bRET(?:_TAKEN)?\(b_\+(\d+)\)')
bad = 0
paths = files or sorted(glob.glob('src/game/**/*.c', recursive=True))
for p in paths:
    if p.endswith(('syms.c', 'ofs.c')) or '/seasons/gen_' in p or '/gen_' in p: continue
    if ('/seasons/' in p) != SEASONS: continue
    base = None
    for n, line in enumerate(open(p, errors='replace'), 1):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = sym.get(m.group(1))
        m = re.search(r'\buint16_t b_\s*[,)]', line)
        if m: base = None
        if line.startswith('}'): base = None
        if base is None or 'O(' in line or 'S(' in line: continue
        for m in BURN.finditer(line.split('//')[0]):
            if m.group(1):
                x, y = int(m.group(2)), int(m.group(3))
                if x == y: continue
                if not reaches(base, x, y):
                    bad += 1; print(f'{p}:{n}: {m.group(1)}(b_+{x}, b_+{y}) does not end on an instruction boundary')
            elif m.group(4):
                x = int(m.group(4))
                if rd(base, x) not in (0xcd, 0xc4, 0xcc, 0xd4, 0xdc):
                    bad += 1; print(f'{p}:{n}: call at b_+{x} is ${rd(base, x):02x} in the ROM, not a call')
            else:
                x = int(m.group(5))
                if rd(base, x) not in (0xc9, 0xc0, 0xc8, 0xd0, 0xd8, 0xd9):
                    bad += 1; print(f'{p}:{n}: ret at b_+{x} is ${rd(base, x):02x} in the ROM, not a ret')
print(f'{bad} burns off instruction boundaries')
