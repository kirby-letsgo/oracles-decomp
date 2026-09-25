#!/usr/bin/env python3
"""Check that every call of an `rst` helper (`xxx_add_a_to_hl(gb, b_+N)`, `..._add_double_index(gb,
b_+N)`, `..._from_rst(gb, b_+N)`, whose argument is the return address) has the rst instruction
itself burned: a CYC/CYCT range ending at b_+N on that line or the line before, or a helper whose
body burns b_+N-1 through the caller's bank. Only calls where the ROM byte at N-1 is an rst opcode
are checked. usage: tools/audit_rst.py [--game=seasons]"""
import glob, re, sys, os
SEASONS = '--game=seasons' in sys.argv
ROM = open('roms/Legend of Zelda, The - Oracle of ' + ('Seasons' if SEASONS else 'Ages') + ' (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_' + ('seasons' if SEASONS else 'ages') + r'\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}
CALL = re.compile(r'\b(\w*(?:add_a_to_hl|addAToHl|add_double_index|AddDoubleIndex|from_rst)\w*)\(gb, b_\+(\d+)\)')
# helpers that burn the rst themselves (their body starts with CYC(b_+N-1, b_+N))
self_burning = set()
for p in glob.glob('src/game/**/*.c', recursive=True):
    for m in re.finditer(r'static void (\w+)\(GB \*gb, uint16_t \w+\) \{\n(?:  [^\n]*\n){0,2}?  CYC\(b_\+\d+, b_\+\d+\); push_effect', open(p, errors='replace').read()):
        self_burning.add(m.group(1))
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if p.endswith(('syms.c', 'ofs.c')) or '/gen_' in p: continue
    if ('/seasons/' in p) != SEASONS: continue
    L = open(p, errors='replace').read().split('\n')
    base = None
    for i, line in enumerate(L):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = sym.get(m.group(1))
        if line.startswith('}'): base = None
        if base is None: continue
        code = line.split('//')[0]
        for m in CALL.finditer(code):
            n = int(m.group(2)); bank, addr = base >> 16, (base & 0xffff) + n - 1
            fo = addr if addr < 0x4000 else bank * 0x4000 + addr - 0x4000
            if (ROM[fo] & 0xc7) != 0xc7: continue
            here = code[:m.start()] + ' ' + (L[i - 1].split('//')[0] if i else '')
            if re.search(rf'CYCT?\(b_\+\d+, b_\+{n}\)', here): continue
            if m.group(1) in self_burning: continue
            bad += 1; print(f'{p}:{i + 1}: {m.group(1)}(gb, b_+{n}) with the rst at b_+{n - 1} not burned')
print(f'{bad} rst helper calls missing the rst burn')
