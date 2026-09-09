#!/usr/bin/env python3
"""Generate cycle-exact C transliterations of routines from the ROM and symbol file.

usage: tools/transliterate.py ROM SYM PORTED_LIST NAME... > out.c
Every instruction becomes `I(addr, cycles); effect;`. Jumps inside the routine become gotos,
calls to ported routines become CALL(...), everything else falls back to the interpreter.
"""
import re, sys
rom = open(sys.argv[1], 'rb').read()
labels, by_addr, local_by_addr, instances = {}, {}, {}, {}
for line in open(sys.argv[2]):
    m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
    if not m: continue
    b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
    labels.setdefault(n, (b, a))
    if a >= 0x8000: continue
    if '@' in n: local_by_addr.setdefault((b, a), n)
    else:
        by_addr.setdefault((b, a), n)
        instances.setdefault(n, [])
        if (b, a) not in instances[n]: instances[n].append((b, a))
def cname_at(bank, addr):
    n = by_addr[(bank, addr)]
    n = n.replace('@', '_')
    return n if len(instances.get(by_addr[(bank, addr)], [])) <= 1 else f'{n}_b{bank:02x}'
ported = set(l.split('#')[0].strip() for l in open(sys.argv[3]) if l.split('#')[0].strip())
def has_unsupported(n):
    if n not in labels: return True
    return all(any(kind == 'unsupported' for a, m, ln, cy, kind, info in routine_body(b, s0)) for (b, s0) in instances.get(n, [labels[n]]))
names = sys.argv[4:]
R8 = ['B', 'C', 'D', 'E', 'H', 'L', None, 'A']
R8N = ['b', 'c', 'd', 'e', 'h', 'l', '(hl)', 'a']
R16 = ['BC', 'DE', 'HL', 'SP']
R16P = ['BC', 'DE', 'HL', 'AF']
CC = ['nz', 'z', 'nc', 'c']
CCEXPR = ['!(F & FZ)', '(F & FZ)', '!(F & FC)', '(F & FC)']
ALUF = ['alu_add', 'alu_adc', 'alu_sub', 'alu_sbc', 'alu_and', 'alu_xor', 'alu_or', 'alu_cp']
ROTF = ['alu_rlc', 'alu_rrc', 'alu_rl', 'alu_rr', 'alu_sla', 'alu_sra', 'alu_swap', 'alu_srl']

def rd(bank, addr, off=0):
    a = addr + off
    return rom[(bank * 0x4000 + (a - 0x4000)) if a >= 0x4000 else a]

def get8(r):
    return 'mem_rd(gb, HL)' if r == 6 else R8[r]
def set8(r, v):
    return f'mem_wr(gb, HL, {v})' if r == 6 else f'{R8[r]} = {v}'
def set16(rr, v):
    return f'gb->sp = {v}' if rr == 'SP' else f'SET_{rr}({v})'
def get16(rr):
    return 'gb->sp' if rr == 'SP' else rr

class Out:
    def __init__(self): self.lines = []
    def emit(self, s): self.lines.append('  ' + s)

ILLEGAL = {0xd3, 0xdb, 0xdd, 0xe3, 0xe4, 0xeb, 0xec, 0xed, 0xf4, 0xfc, 0xfd}
def decode(bank, addr):
    """Return (mnemonic, length, cycles, kind, info)."""
    op = rd(bank, addr)
    if op in ILLEGAL: return f'illegal ${op:02x}', 1, 1, 'unsupported', ()
    n1 = rd(bank, addr, 1) if addr + 1 < 0x8000 else 0
    n16 = n1 | (rd(bank, addr, 2) << 8) if addr + 2 < 0x8000 else 0
    rel = (addr + 2 + ((n1 ^ 0x80) - 0x80)) & 0xffff
    x, y, z, p, q = op >> 6, (op >> 3) & 7, op & 7, (op >> 4) & 3, (op >> 3) & 1
    if op == 0xcb:
        o = n1; cy, cz = (o >> 3) & 7, o & 7; mem = cz == 6
        if o < 0x40:
            return f'{ROTF[cy][4:]} {R8N[cz]}', 2, 4 if mem else 2, 'op', (set8(cz, f'{ROTF[cy]}(gb, {get8(cz)})'),)
        kind = ['bit', 'res', 'set'][(o >> 6) - 1]
        if kind == 'bit': return f'bit {cy},{R8N[cz]}', 2, 3 if mem else 2, 'op', (f'alu_bit(gb, {cy}, {get8(cz)})',)
        expr = f'({get8(cz)} & ~(1 << {cy}))' if kind == 'res' else f'({get8(cz)} | (1 << {cy}))'
        return f'{kind} {cy},{R8N[cz]}', 2, 4 if mem else 2, 'op', (set8(cz, f'(uint8_t){expr}'),)
    if x == 0:
        if z == 0:
            if y == 0: return 'nop', 1, 1, 'op', ('',)
            if y == 1: return f'ld (${n16:04x}),sp', 3, 4, 'op', (f'mem_wr(gb, 0x{n16:04x}, (uint8_t)gb->sp); TN(1); mem_wr(gb, 0x{n16+1:04x}, gb->sp >> 8)',)
            if y == 2: return 'stop', 2, 1, 'unsupported', ()
            if y == 3: return f'jr ${rel:04x}', 2, 3, 'jp', (rel, None)
            return f'jr {CC[y-4]},${rel:04x}', 2, (3, 2), 'jpcc', (rel, CCEXPR[y-4])
        if z == 1:
            if q == 0: return f'ld {R16[p].lower()},${n16:04x}', 3, 3, 'spload' if p == 3 else 'op', (set16(R16[p], f'0x{n16:04x}'),)
            return f'add hl,{R16[p].lower()}', 1, 2, 'op', (f'alu_add_hl(gb, {get16(R16[p])})',)
        if z == 2:
            t = ['(bc)', '(de)', '(hl+)', '(hl-)'][p]
            ptr = ['BC', 'DE', 'HL', 'HL'][p]
            post = ['', '', '; SET_HL(HL + 1)', '; SET_HL(HL - 1)'][p]
            if q == 0: return f'ld {t},a', 1, 2, 'op', (f'mem_wr(gb, {ptr}, A){post}',)
            return f'ld a,{t}', 1, 2, 'op', (f'A = mem_rd(gb, {ptr}){post}',)
        if z == 3:
            if q == 0: return f'inc {R16[p].lower()}', 1, 2, 'op', (set16(R16[p], f'{get16(R16[p])} + 1'),)
            return f'dec {R16[p].lower()}', 1, 2, 'op', (set16(R16[p], f'{get16(R16[p])} - 1'),)
        if z == 4: return f'inc {R8N[y]}', 1, 3 if y == 6 else 1, 'op', (set8(y, f'alu_inc8(gb, {get8(y)})'),)
        if z == 5: return f'dec {R8N[y]}', 1, 3 if y == 6 else 1, 'op', (set8(y, f'alu_dec8(gb, {get8(y)})'),)
        if z == 6: return f'ld {R8N[y]},${n1:02x}', 2, 3 if y == 6 else 2, 'op', (set8(y, f'0x{n1:02x}'),)
        f = ['alu_rlca', 'alu_rrca', 'alu_rla', 'alu_rra', 'alu_daa', 'alu_cpl', 'alu_scf', 'alu_ccf'][y]
        return f[4:], 1, 1, 'op', (f'{f}(gb)',)
    if x == 1:
        if op == 0x76: return 'halt', 1, 1, 'unsupported', ()
        return f'ld {R8N[y]},{R8N[z]}', 1, 2 if (y == 6 or z == 6) else 1, 'op', (set8(y, get8(z)),)
    if x == 2: return f'{ALUF[y][4:]} {R8N[z]}', 1, 2 if z == 6 else 1, 'op', (f'{ALUF[y]}(gb, {get8(z)})',)
    if z == 0:
        if y < 4: return f'ret {CC[y]}', 1, (5, 2), 'retcc', (CCEXPR[y],)
        if y == 4: return f'ldh (${0xff00|n1:04x}),a', 2, 3, 'op', (f'mem_wr(gb, 0x{0xff00|n1:04x}, A)',)
        if y == 5: return f'add sp,${n1:02x}', 2, 4, 'op', (f'alu_add_sp(gb, 0x{n1:02x})',)
        if y == 6: return f'ldh a,(${0xff00|n1:04x})', 2, 3, 'op', (f'A = mem_rd(gb, 0x{0xff00|n1:04x})',)
        return f'ld hl,sp+${n1:02x}', 2, 3, 'op', (f'alu_ld_hl_sp(gb, 0x{n1:02x})',)
    if z == 1:
        if q == 0: return f'pop {R16P[p].lower()}', 1, 3, 'pop', (R16P[p],)
        return [('ret', 1, 4, 'ret', ()), ('reti', 1, 4, 'unsupported', ()), ('jp hl', 1, 1, 'jphl', ()), ('ld sp,hl', 1, 2, 'spload', ('gb->sp = HL',))][p]
    if z == 2:
        if y < 4: return f'jp {CC[y]},${n16:04x}', 3, (4, 3), 'jpcc', (n16, CCEXPR[y])
        return [('ld ($ff00+c),a', 1, 2, 'op', ('mem_wr(gb, 0xff00 | C, A)',)), (f'ld (${n16:04x}),a', 3, 4, 'op', (f'mem_wr(gb, 0x{n16:04x}, A)',)),
                ('ld a,($ff00+c)', 1, 2, 'op', ('A = mem_rd(gb, 0xff00 | C)',)), (f'ld a,(${n16:04x})', 3, 4, 'op', (f'A = mem_rd(gb, 0x{n16:04x})',))][y-4]
    if z == 3:
        if y == 0: return f'jp ${n16:04x}', 3, 4, 'jp', (n16, None)
        if y == 6: return 'di', 1, 1, 'op', ('gb->ime = false; gb->ime_delay = false',)
        if y == 7: return 'ei', 1, 1, 'unsupported', ()
    if z == 4: return f'call {CC[y]},${n16:04x}', 3, (6, 3), 'callcc', (n16, CCEXPR[y])
    if z == 5:
        if q == 0: return f'push {R16P[p].lower()}', 1, 4, 'push', (R16P[p],)
        return f'call ${n16:04x}', 3, 6, 'call', (n16, None)
    if z == 6: return f'{ALUF[y][4:]} ${n1:02x}', 2, 2, 'op', (f'{ALUF[y]}(gb, 0x{n1:02x})',)
    return f'rst ${y*8:02x}', 1, 4, 'rst', (y * 8,)

def routine_body(bank, start):
    """Decode every instruction reachable from start without crossing into another labeled routine."""
    seen = {}
    work = [start]
    while work:
        a = work.pop()
        if a in seen or a >= 0x8000: continue
        if a != start and by_addr.get((bank, a)): continue
        m, ln, cy, kind, info = decode(bank, a)
        succ = []
        if kind == 'rst' and info[0] == 0:
            table = []
            pos = a + ln
            while True:
                w = rd(bank, pos) | (rd(bank, pos + 1) << 8)
                table.append(w)
                pos += 2
                if pos >= min(table) or pos >= 0x8000: break
            kind, info = 'jumptable', (table, a + ln)
            succ = list(table)
        elif kind in ('jp', 'jpcc'):
            succ.append(info[0])
            if kind != 'jp': succ.append(a + ln)
        elif kind in ('ret', 'jphl', 'unsupported'):
            pass
        else:
            succ.append(a + ln)
        seen[a] = (a, m, ln, cy, kind, info)
        work.extend(succ)
    return [seen[a] for a in sorted(seen)]

def target_name(bank, t, insns_addrs):
    if t in insns_addrs: return None
    return by_addr.get((bank, t))

def cname(n): return n.replace('@', '_')

def gen(name, bank=None, start=None):
    if bank is None: bank, start = labels[name]
    insns = routine_body(bank, start)
    bad = [m for a, m, ln, cy, kind, info in insns if kind == 'unsupported']
    if bad:
        print(f'warning: {name} skipped ({bad[0]})', file=sys.stderr)
        return None
    addrs = set(a for a, *_ in insns)
    jump_targets = set()
    for a, m, ln, cy, kind, info in insns:
        if kind in ('jp', 'jpcc', 'callcc') and info[0] in addrs: jump_targets.add(info[0])
        if kind == 'jumptable': jump_targets.update(t for t in info[0] if t in addrs)
    out = Out()
    out.lines.append(f'// {bank:02x}:{start:04x}')
    out.lines.append(f'void {cname_at(bank, start)}(GB *gb) {{')
    if insns[0][0] != start:
        jump_targets.add(start)
        out.emit(f'goto L_{start:04x};')
    end = insns[-1][0] + insns[-1][2]
    for a, m, ln, cy, kind, info in insns:
        if a in jump_targets or (bank, a) in local_by_addr:
            out.lines.append(f'L_{a:04x}:')
        com = f'  // {m}'
        if kind == 'op':
            eff = info[0]
            out.emit(f'I(0x{a:04x}, {cy});{" " + eff + ";" if eff else ""}{com}')
        elif kind == 'push':
            out.emit(f'PUSH(0x{a:04x}, {info[0]});{com}')
        elif kind == 'pop':
            r = info[0]
            out.emit((f'SET_AF(POP(0x{a:04x}));' if r == 'AF' else f'SET_{r}(POP(0x{a:04x}));') + com)
        elif kind == 'ret':
            out.emit(f'RET(0x{a:04x}); return;{com}')
        elif kind == 'retcc':
            out.emit(f'if ({info[0]}) {{ RET_TAKEN(0x{a:04x}); return; }} I(0x{a:04x}, 2);{com}')
        elif kind in ('jp', 'jpcc'):
            t, cond = info
            taken, not_taken = (cy if isinstance(cy, tuple) else (cy, None))
            if t in addrs:
                jump = f'goto L_{t:04x};'
            else:
                tn = by_addr.get((bank, t)) or (local_by_addr.get((bank, t)) or '').split('@')[0]
                if tn in ported and (bank, t) in by_addr: jump = f'{cname_at(bank, t)}(gb); return;'
                else: jump = f'HANDOFF(0x{t:04x}); /* {tn or "unported"} */'
            if cond is None:
                out.emit(f'I(0x{a:04x}, {taken}); {jump}{com}')
            else:
                out.emit(f'if ({cond}) {{ I(0x{a:04x}, {taken}); {jump} }} I(0x{a:04x}, {not_taken});{com}')
        elif kind in ('call', 'callcc'):
            t, cond = info
            tn = by_addr.get((bank, t))
            ra = a + ln
            if tn in ported: c = f'CALL(0x{a:04x}, {cname_at(bank, t)}, 0x{t:04x}, 0x{ra:04x});'
            else: c = f'CALL_ASM(0x{a:04x}, 0x{t:04x}, 0x{ra:04x}); /* {tn or "unported"} */'
            if cond is None: out.emit(c + com)
            else: out.emit(f'if ({cond}) {{ {c} }} else I(0x{a:04x}, 3);{com}')
        elif kind == 'jumptable':
            table, ra = info
            out.emit(f'RST_PUSH(0x{a:04x}, 0x{ra:04x});  // rst $00 (jump table)')
            out.emit('I(0x0000, 1); alu_add(gb, A); I(0x0001, 3); SET_HL(pop_effect(gb)); I(0x0002, 1); alu_add(gb, L); I(0x0003, 1); L = A;')
            out.emit('if (!(F & FC)) I(0x0004, 3); else { I(0x0004, 2); I(0x0006, 1); H = alu_inc8(gb, H); }')
            out.emit('I(0x0007, 2); A = mem_rd(gb, HL); SET_HL(HL + 1); I(0x0008, 2); H = mem_rd(gb, HL); I(0x0009, 1); L = A; I(0x000a, 1);')
            cases = ' '.join(f'case 0x{t:04x}: goto L_{t:04x};' for t in sorted(set(t for t in table if t in addrs)))
            out.emit(f'switch (HL) {{ {cases} default: HANDOFF(HL); }}')
        elif kind == 'rst':
            ra = a + ln
            if info[0] == 0x10:
                out.emit(f'RST_PUSH(0x{a:04x}, 0x{ra:04x});{com} (addAToHl)')
                out.emit('I(0x0010, 1); alu_add(gb, L); I(0x0011, 1); L = A;')
                out.emit('if (!(F & FC)) { I(0x0012, 5); pop_effect(gb); } else { I(0x0012, 2); I(0x0013, 1); H = alu_inc8(gb, H); I(0x0014, 4); pop_effect(gb); }')
            elif info[0] == 0x18:
                out.emit(f'RST_PUSH(0x{a:04x}, 0x{ra:04x});{com} (addDoubleIndexToHl)')
                out.emit('PUSH(0x0018, BC); I(0x0019, 1); C = A; I(0x001a, 2); B = 0x00; I(0x001c, 2); alu_add_hl(gb, BC); I(0x001d, 2); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); I(0x001f, 4); pop_effect(gb);')
            else:
                out.emit(f'CALL_ASM_RST(0x{a:04x}, 0x{info[0]:04x}, 0x{ra:04x});{com}')
        elif kind == 'jphl':
            out.emit(f'I(0x{a:04x}, 1); HANDOFF(HL);{com}')
        elif kind == 'spload':
            out.emit(f'I(0x{a:04x}, {cy}); {info[0]}; HANDOFF(0x{a + ln:04x});{com}')
        else:
            out.emit(f'#error unsupported instruction {m} at {a:04x}')
        falls = kind not in ('ret', 'jp', 'jphl', 'jumptable', 'unsupported', 'spload')
        nxt_a = a + ln
        if falls and nxt_a not in addrs:
            nn = by_addr.get((bank, nxt_a))
            if nn in ported: out.emit(f'{cname_at(bank, nxt_a)}(gb); return;  // fallthrough')
            else: out.emit(f'HANDOFF(0x{nxt_a:04x});  // fallthrough to {nn or "unlabeled"}')
    out.lines.append('}')
    return '\n'.join(out.lines)

if names and names[0] == '--out':
    outdir = names[1]
    names = [l.split('#')[0].strip() for l in open(sys.argv[3])]
    names = [n for n in names if n]
    import collections, os
    by_bank = collections.OrderedDict()
    missing = [n for n in names if n not in labels]
    for n in missing: print(f'warning: unknown routine {n} (skipped)', file=sys.stderr)
    names = list(dict.fromkeys(n for n in names if n in labels))
    ported = set(n for n in ported if n in labels and not has_unsupported(n))
    for n in names:
        by_bank.setdefault(labels[n][0], []).append(n)
    with open(os.path.join(outdir, 'gen.h'), 'w') as h:
        h.write('// generated by tools/transliterate.py; do not edit\n#pragma once\n#include "core/gb.h"\n')
        for n in names: h.write(f'void {cname(n)}(GB *gb);\n')
    generated = []
    by_bank = collections.OrderedDict()
    for n in names:
        for (b, a) in instances.get(n, []): by_bank.setdefault(b, []).append((n, b, a))
    for bank, items in by_bank.items():
        with open(os.path.join(outdir, f'gen_bank{bank:02x}.c'), 'w') as f:
            f.write('// generated by tools/transliterate.py; do not edit\n#include "game/asm.h"\n#include "game/gen.h"\n\n')
            for n, b, a in items:
                code = gen(n, b, a)
                if code: f.write(code + '\n\n'); generated.append((b, a, cname_at(b, a)))
    with open(os.path.join(outdir, 'gen.h'), 'w') as h:
        h.write('// generated by tools/transliterate.py; do not edit\n#pragma once\n#include "core/gb.h"\n')
        for b, a, cn in generated: h.write(f'void {cn}(GB *gb);\n')
    with open('src/hooks/generated.txt', 'w') as g:
        for b, a, cn in generated: g.write(f'{b:02x}:{a:04x} {cn}\n')
    print(f'{len(generated)} routines in {len(by_bank)} bank files')
else:
    print('#include "game/asm.h"\n#include "game/gen.h"\n')
    for n in names:
        print(gen(n)); print()
