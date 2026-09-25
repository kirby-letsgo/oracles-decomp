#!/usr/bin/env python3
"""Check branch conditions in the C against the ROM's condition codes.

usage: tools/audit_cond.py

`if (COND) { CYCT(b_+N, ...)`, `if (COND) { RET_TAKEN(b_+N)` and `if (COND) { CALL_C_CC(b_+N, ...`
take the branch the ROM instruction at N takes, so COND must be that instruction's condition:
jr/jp/call/ret nz -> !(F & FZ), z -> (F & FZ), nc -> !(F & FC), c -> (F & FC). A swapped
polarity returns or jumps exactly when the ROM falls through (pincer_state1 returned on `z`
after `ret nz`, so the pincer spawner never spawned its parts). Offsets written b_+N or b_+O(N)
are read in the Ages ROM, and files under src/game/seasons/ (b_+N, b_+S(N)) in the Seasons ROM.
"""
import glob, re

def game(prefix):
    rom = open(f'roms/Legend of Zelda, The - Oracle of {prefix} (USA, Australia).gbc', 'rb').read()
    ids = re.findall(r'^\s+S_(\w+),', open('src/game/syms.h').read(), re.M)
    table = open('src/game/syms.c').read().split('syms_' + prefix.lower())[1].split('};')[0]
    return rom, dict(zip(ids, (int(v, 16) for v in re.findall(r'0x([0-9a-f]{8})', table))))
GAMES = {'Ages': game('Ages'), 'Seasons': game('Seasons')}

CC = {}
for base, ops in (('jr', (0x20, 0x28, 0x30, 0x38)), ('jp', (0xc2, 0xca, 0xd2, 0xda)),
                  ('call', (0xc4, 0xcc, 0xd4, 0xdc)), ('ret', (0xc0, 0xc8, 0xd0, 0xd8))):
    for op, cc in zip(ops, ('nz', 'z', 'nc', 'c')): CC[op] = (base, cc)
WANT = {'nz': '!(F & FZ)', 'z': '(F & FZ)', 'nc': '!(F & FC)', 'c': '(F & FC)'}
IMM8 = {0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e, 0xc6, 0xce, 0xd6, 0xde, 0xe6, 0xee, 0xf6, 0xfe,
        0x18, 0x20, 0x28, 0x30, 0x38, 0xe0, 0xf0, 0xe8, 0xf8, 0x10, 0xcb}
IMM16 = {0x01, 0x11, 0x21, 0x31, 0x08, 0xc2, 0xc3, 0xc4, 0xca, 0xcc, 0xcd, 0xd2, 0xd4, 0xda, 0xdc, 0xea, 0xfa}
def length(op): return 3 if op in IMM16 else 2 if op in IMM8 else 1
TAKEN = re.compile(r'\bCYCT\(b_\+(?:(O|OE|S)\()?(\d+)\)?,\s*b_\+(?:(O|OE|S)\()?(\d+)\)?\)')
BRANCH = re.compile(r'\bif \((!?\(F & F[ZC]\))\)\s*\{?\s*(?:CYCT|RET_TAKEN|CALL_C_CC|CALL_L_CC)\(b_\+(?:(O|OE|S)\()?(\d+)')

bad = checked = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or p.endswith(('syms.c', 'ofs.c')): continue
    seasons_file = '/seasons/' in p
    rom, sym = GAMES['Seasons' if seasons_file else 'Ages']
    base = None
    for ln, line in enumerate(open(p, errors='replace'), 1):
        mb = re.search(r'\bBASE\((\w+)\)', line)
        if mb: base = sym.get(mb.group(1))
        if base is None or base == 0xffffffff: continue
        for m in BRANCH.finditer(line.split('//')[0]):
            wrap = m.group(2)
            if (wrap == 'S') != seasons_file and wrap is not None: continue
            if seasons_file and wrap in ('O', 'OE'): continue
            b, a = base >> 16, (base & 0xffff) + int(m.group(3))
            fo = a if a < 0x4000 else b * 0x4000 + a - 0x4000
            if not 0 <= fo < len(rom) or rom[fo] not in CC: continue
            kind, cc = CC[rom[fo]]
            checked += 1
            if m.group(1) != WANT[cc]:
                bad += 1
                print(f'{p}:{ln}: if {m.group(1)} but the ROM has {kind} {cc} at +{m.group(3)}')
        # a taken burn charges its last instruction as taken: a conditional branch before it in
        # the same range is charged as not taken (roller's `jr nc` burned with the call after it)
        for m in TAKEN.finditer(line.split('//')[0]):
            w1, w2 = m.group(1), m.group(3)
            if w1 != w2 and not (w1 in ('O', 'OE') and w2 in ('O', 'OE')): continue
            if (w1 == 'S') != seasons_file and w1 is not None: continue
            if seasons_file and w1 in ('O', 'OE'): continue
            b, a0 = base >> 16, (base & 0xffff) + int(m.group(2))
            end = (base & 0xffff) + int(m.group(4))
            pos, inner = a0, []
            while pos < end:
                fo = pos if pos < 0x4000 else b * 0x4000 + pos - 0x4000
                if not 0 <= fo < len(rom): break
                inner.append((pos, rom[fo])); pos += length(rom[fo])
            for pos, op in inner[:-1]:
                if op in CC and CC[op][0] != 'call':
                    bad += 1
                    print(f'{p}:{ln}: CYCT(+{m.group(2)}, +{m.group(4)}) also charges the {CC[op][0]} {CC[op][1]} at +{pos - (base & 0xffff)} as not taken')
print(f'{bad} branch conditions differ from the ROM ({checked} checked)')
