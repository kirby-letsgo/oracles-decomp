#!/usr/bin/env python3
"""Find an ALU instruction written as a plain C expression, which leaves the flags unchanged:

    CYC(b_+92, b_+94); A = (uint8_t)(A ^ 0x80);     // ROM: xor $80 (sets Z, clears N/H/C)

Fine for `set`/`res` (no flags) and `ld`; wrong for and/or/xor/add/adc/sub/sbc/cp/inc/dec, whose
flags the next instruction may test. Reported: a single-instruction burn (checked against the Ages
ROM) of a flag-setting opcode followed on the same line by an assignment to a register that does
not go through an alu_* helper.
usage: tools/audit_flags.py"""
import glob, re
ROM = open('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_ages\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}

def flag_op(op, nxt):
    """(mnemonic, length) for an instruction that writes flags, else None."""
    alu_n = {0xc6: 'add', 0xce: 'adc', 0xd6: 'sub', 0xde: 'sbc', 0xe6: 'and', 0xee: 'xor', 0xf6: 'or', 0xfe: 'cp'}
    if op in alu_n: return alu_n[op], 2
    if 0x80 <= op < 0xc0: return ('add', 'adc', 'sub', 'sbc', 'and', 'xor', 'or', 'cp')[(op >> 3) & 7], 1
    if op & 0xc7 in (0x04, 0x05) and op != 0x34 and op != 0x35: return ('inc' if op & 1 == 0 else 'dec'), 1
    if op in (0x07, 0x0f, 0x17, 0x1f, 0x27, 0x2f, 0x37, 0x3f): return 'rot/misc', 1
    if op == 0xcb and nxt < 0x40: return 'cb-shift', 2
    if op == 0xcb and nxt < 0x80: return 'bit', 2
    return None

ASSIGN = re.compile(r'\b([ABCDEHL])\s*(?:[-+&|^]?=)\s*(?!alu_)([^;]*)')
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or '/seasons/' in p or p.endswith(('syms.c', 'ofs.c')): continue
    base = None
    for i, l in enumerate(open(p, errors='replace').read().split('\n')):
        bm = re.search(r'\bBASE\((\w+)\)', l)
        if bm: base = sym.get(bm.group(1))
        if l.startswith('}'): base = None
        if base is None: continue
        m = re.match(r'\s*CYCT?\(b_\+(\d+), b_\+(\d+)\);\s*(.*)$', l.split('//')[0])
        if not m: continue
        x, y, rest = int(m.group(1)), int(m.group(2)), m.group(3)
        b, a = base >> 16, (base & 0xffff) + x
        fo = a if a < 0x4000 else b * 0x4000 + a - 0x4000
        fl = flag_op(ROM[fo], ROM[fo + 1])
        if not fl or fl[1] != y - x or not rest.strip(): continue
        if re.search(r'\balu_\w+\(|\bF\s*=|\bF\s*[|&]=|SET_F|set_flags', rest): continue
        if ASSIGN.search(rest):
            bad += 1
            print(f'{p}:{i + 1}: `{rest.strip()}` burns {fl[0]} (flags not set)')
print(f'{bad} flag-setting instructions written without their flags')
