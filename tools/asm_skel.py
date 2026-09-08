#!/usr/bin/env python3
"""Print a transliteration skeleton for a routine: address, bytes, mnemonic, M-cycles.

usage: tools/asm_skel.py ROM SYM routine [routine...]
Cycles for conditional jumps/calls/rets are given as taken/not-taken.
"""
import re, sys
rom = open(sys.argv[1], 'rb').read()
labels, by_addr = {}, {}
for line in open(sys.argv[2]):
    m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
    if m:
        b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
        labels.setdefault(n, (b, a))
        if a < 0x8000 and not n.startswith('@'): by_addr.setdefault((b, a), n)
R8 = ['b', 'c', 'd', 'e', 'h', 'l', '(hl)', 'a']
R16 = ['bc', 'de', 'hl', 'sp']
R16P = ['bc', 'de', 'hl', 'af']
CC = ['nz', 'z', 'nc', 'c']
ALU = ['add a,', 'adc a,', 'sub', 'sbc a,', 'and', 'xor', 'or', 'cp']
ROT = ['rlc', 'rrc', 'rl', 'rr', 'sla', 'sra', 'swap', 'srl']
def rd(bank, addr, off=0):
    a = addr + off
    return rom[(bank * 0x4000 + (a - 0x4000)) if a >= 0x4000 else a]
def decode(bank, addr):
    op = rd(bank, addr)
    n1 = lambda: rd(bank, addr, 1)
    n16 = lambda: rd(bank, addr, 1) | (rd(bank, addr, 2) << 8)
    x, y, z, p, q = op >> 6, (op >> 3) & 7, op & 7, (op >> 4) & 3, (op >> 3) & 1
    if op == 0xcb:
        o = n1(); cy, cz = (o >> 3) & 7, o & 7; r = R8[cz]; mem = cz == 6
        if o < 0x40: return f'{ROT[cy]} {r}', 2, 4 if mem else 2
        kind = ['bit', 'res', 'set'][(o >> 6) - 1]
        return f'{kind} {cy},{r}', 2, (3 if kind == 'bit' else 4) if mem else 2
    if x == 0:
        if z == 0:
            if y == 0: return 'nop', 1, 1
            if y == 1: return f'ld (${n16():04x}),sp', 3, 5
            if y == 2: return 'stop', 2, 1
            if y == 3: return f'jr ${(addr + 2 + ((n1() ^ 0x80) - 0x80)) & 0xffff:04x}', 2, 3
            return f'jr {CC[y-4]},${(addr + 2 + ((n1() ^ 0x80) - 0x80)) & 0xffff:04x}', 2, '3/2'
        if z == 1: return (f'ld {R16[p]},${n16():04x}', 3, 3) if q == 0 else (f'add hl,{R16[p]}', 1, 2)
        if z == 2:
            t = ['(bc)', '(de)', '(hl+)', '(hl-)'][p]
            return (f'ld {t},a', 1, 2) if q == 0 else (f'ld a,{t}', 1, 2)
        if z == 3: return (f'inc {R16[p]}', 1, 2) if q == 0 else (f'dec {R16[p]}', 1, 2)
        if z == 4: return f'inc {R8[y]}', 1, 3 if y == 6 else 1
        if z == 5: return f'dec {R8[y]}', 1, 3 if y == 6 else 1
        if z == 6: return f'ld {R8[y]},${n1():02x}', 2, 3 if y == 6 else 2
        return ['rlca', 'rrca', 'rla', 'rra', 'daa', 'cpl', 'scf', 'ccf'][y], 1, 1
    if x == 1:
        if op == 0x76: return 'halt', 1, 1
        return f'ld {R8[y]},{R8[z]}', 1, 2 if (y == 6 or z == 6) else 1
    if x == 2: return f'{ALU[y]} {R8[z]}', 1, 2 if z == 6 else 1
    if z == 0:
        if y < 4: return f'ret {CC[y]}', 1, '5/2'
        if y == 4: return f'ldh (${0xff00 | n1():04x}),a', 2, 3
        if y == 5: return f'add sp,${n1():02x}', 2, 4
        if y == 6: return f'ldh a,(${0xff00 | n1():04x})', 2, 3
        return f'ld hl,sp+${n1():02x}', 2, 3
    if z == 1:
        if q == 0: return f'pop {R16P[p]}', 1, 3
        return [('ret', 1, 4), ('reti', 1, 4), ('jp hl', 1, 1), ('ld sp,hl', 1, 2)][p]
    if z == 2:
        if y < 4: return f'jp {CC[y]},${n16():04x}', 3, '4/3'
        return [f'ld ($ff00+c),a', f'ld (${n16():04x}),a', 'ld a,($ff00+c)', f'ld a,(${n16():04x})'][y-4], (1 if y in (4, 6) else 3), (2 if y in (4, 6) else 4)
    if z == 3:
        if y == 0: return f'jp ${n16():04x}', 3, 4
        if y == 6: return 'di', 1, 1
        if y == 7: return 'ei', 1, 1
    if z == 4: return f'call {CC[y]},${n16():04x}', 3, '6/3'
    if z == 5: return (f'push {R16P[p]}', 1, 4) if q == 0 else (f'call ${n16():04x}', 3, 6)
    if z == 6: return f'{ALU[y]} ${n1():02x}', 2, 2
    return f'rst ${y*8:02x}', 1, 4
for name in sys.argv[3:]:
    bank, addr = labels[name]
    print(f'// {bank:02x}:{addr:04x} {name}')
    a = addr
    while True:
        lab = by_addr.get((bank, a))
        if a != addr and lab and not lab.startswith('_') and not lab.startswith('@'):
            break
        if lab and a != addr: print(f'  // {lab}:')
        m, ln, cy = decode(bank, a)
        bs = ' '.join(f'{rd(bank, a, i):02x}' for i in range(ln))
        tgt = ''
        mm = re.search(r'\$([0-9a-f]{4})$', m)
        if mm and (m.startswith(('jr', 'jp', 'call'))):
            t = int(mm.group(1), 16)
            if (bank, t) in by_addr: tgt = f'  ; {by_addr[(bank, t)]}'
        print(f'  I(0x{a:04x}, {cy});  // {bs:9s} {m}{tgt}')
        a += ln
        if m in ('ret', 'reti', 'jp hl') or m.startswith('jp $') or m.startswith('jr $'):
            nxt = by_addr.get((bank, a))
            if nxt and not nxt.startswith('@'): break
            if m in ('ret', 'reti', 'jp hl') or m.startswith('jp $'):
                if by_addr.get((bank, a)) is None:
                    continue
    print()
