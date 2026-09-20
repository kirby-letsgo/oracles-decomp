#!/usr/bin/env python3
"""List every instruction the interpreter executed in a PCHIST dump, one line per address.

usage: tools/pc_sites.py HIST_FILE ROM [ages.sym] [src/hooks/table.h]
Columns: count, bank:addr, bytes, mnemonic, enclosing label+offset, whether the address is a hook
entry, and the distance to the next hook entry in the same bank (how far a tail runs before C
takes over again). Milestone 5 uses this to enumerate the trampolines and caller tails the native
dispatch macros must cost and cover.
"""
import re, struct, sys, bisect
hist = open(sys.argv[1], 'rb').read()
rom = open(sys.argv[2], 'rb').read()
sym = sys.argv[3] if len(sys.argv) > 3 else 'ref/oracles-disasm/ages.sym'
table = sys.argv[4] if len(sys.argv) > 4 else 'src/hooks/table.h'
banks = {}
for line in open(sym):
    m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
    if not m: continue
    b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
    if a >= 0x8000 or '.' in n: continue
    banks.setdefault(b, []).append((a, n))
for b in banks: banks[b].sort()
hooks = {}
for line in open(table):
    m = re.match(r'HOOK\(0x([0-9a-f]+), 0x([0-9a-f]+), (\w+)', line.strip())
    if m: hooks.setdefault(int(m.group(1), 16), set()).add(int(m.group(2), 16))
hooked = {b: sorted(s) for b, s in hooks.items()}
R8 = ['b', 'c', 'd', 'e', 'h', 'l', '(hl)', 'a']; R16 = ['bc', 'de', 'hl', 'sp']; R16P = ['bc', 'de', 'hl', 'af']
CC = ['nz', 'z', 'nc', 'c']; ALU = ['add', 'adc', 'sub', 'sbc', 'and', 'xor', 'or', 'cp']
def decode(bs):
    op = bs[0]; x, y, z, p, q = op >> 6, (op >> 3) & 7, op & 7, op >> 4 & 3, op >> 3 & 1
    n8 = bs[1] if len(bs) > 1 else 0; n16 = n8 | (bs[2] << 8 if len(bs) > 2 else 0)
    if op == 0xcb: return 2, f'cb {n8:02x}'
    if x == 0:
        if z == 0: return {0: (1, 'nop'), 1: (3, f'ld (${n16:04x}),sp'), 2: (2, 'stop'), 3: (2, f'jr ${n8:02x}')}.get(y, (2, f'jr {CC[y-4]},${n8:02x}'))
        if z == 1: return (1, f'add hl,{R16[p]}') if q else (3, f'ld {R16[p]},${n16:04x}')
        if z == 2: return 1, ['ld (bc),a', 'ld a,(bc)', 'ld (de),a', 'ld a,(de)', 'ldi (hl),a', 'ldi a,(hl)', 'ldd (hl),a', 'ldd a,(hl)'][p * 2 + q]
        if z == 3: return 1, f'{"dec" if q else "inc"} {R16[p]}'
        if z == 4: return 1, f'inc {R8[y]}'
        if z == 5: return 1, f'dec {R8[y]}'
        if z == 6: return 2, f'ld {R8[y]},${n8:02x}'
        return 1, ['rlca', 'rrca', 'rla', 'rra', 'daa', 'cpl', 'scf', 'ccf'][y]
    if x == 1: return 1, 'halt' if op == 0x76 else f'ld {R8[y]},{R8[z]}'
    if x == 2: return 1, f'{ALU[y]} {R8[z]}'
    if z == 0:
        if y < 4: return 1, f'ret {CC[y]}'
        return {4: (2, f'ldh (${n8:02x}),a'), 5: (2, f'add sp,${n8:02x}'), 6: (2, f'ldh a,(${n8:02x})'), 7: (2, f'ld hl,sp+${n8:02x}')}[y]
    if z == 1: return (1, ['ret', 'reti', 'jp hl', 'ld sp,hl'][p]) if q else (1, f'pop {R16P[p]}')
    if z == 2:
        if y < 4: return 3, f'jp {CC[y]},${n16:04x}'
        return {4: (1, 'ld (c),a'), 5: (3, f'ld (${n16:04x}),a'), 6: (1, 'ld a,(c)'), 7: (3, f'ld a,(${n16:04x})')}[y]
    if z == 3: return (3, f'jp ${n16:04x}') if y == 0 else (1, ['di', 'ei'][y - 6] if y >= 6 else f'ILLEGAL {op:02x}')
    if z == 4: return 3, f'call {CC[y]},${n16:04x}'
    if z == 5: return (3, f'call ${n16:04x}') if q else (1, f'push {R16P[p]}')
    if z == 6: return 2, f'{ALU[y]} ${n8:02x}'
    return 1, f'rst ${y * 8:02x}'
vals = struct.unpack(f'<{len(hist) // 4}I', hist)
rows = []
for i, c in enumerate(vals):
    if not c: continue
    b, addr = i >> 15, i & 0x7fff
    off = addr if b == 0 else b * 0x4000 + (addr - 0x4000)
    bs = rom[off:off + 3]
    ln, mn = decode(bs)
    lst = banks.get(b, [])
    j = bisect.bisect_right(lst, (addr, '\xff')) - 1
    lab = f'{lst[j][1]}+{addr - lst[j][0]}' if j >= 0 else '?'
    hk = hooked.get(b, [])
    k = bisect.bisect_left(hk, addr)
    is_hook = k < len(hk) and hk[k] == addr
    kn = k + (1 if is_hook else 0)
    nxt = hk[kn] - addr if kn < len(hk) else -1
    rows.append((c, b, addr, bs[:ln].hex(), mn, lab, is_hook, nxt))
rows.sort(key=lambda r: (r[1], r[2]))
print(f'{len(rows)} distinct addresses, {sum(r[0] for r in rows)} instructions')
for c, b, addr, hx, mn, lab, is_hook, nxt in rows:
    print(f'{c:10d}  {b:02x}:{addr:04x}  {hx:6s}  {mn:18s} {lab:48s} {"HOOK " if is_hook else "     "} next+{nxt}')
