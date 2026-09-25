#!/usr/bin/env python3
"""Generate cycle-exact C transliterations of routines from the ROM and symbol file.

usage: tools/transliterate.py ROM SYM PORTED_LIST NAME... > out.c
Every instruction becomes `I(addr, cycles); effect;`. Jumps inside the routine become gotos,
calls to ported routines become CALL(...), everything else falls back to the interpreter.
"""
import glob, re, sys
RESUME_TAILS = '--resume-tails' in sys.argv
SEASONS = '--game=seasons' in sys.argv       # Seasons-only routines: `s_` names, Seasons extra labels, the shared hooks as callees
sys.argv = [a for a in sys.argv if a not in ('--resume-tails', '--game=seasons')]
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
relocs = []
externs = set()
extra_names = set()
alias_labels = set()
import os as _os
for extra_path in (['src/hooks/extra_seasons.sym', 'src/hooks/alias_seasons.sym'] if SEASONS else ['src/hooks/extra.sym']):
    if not _os.path.exists(extra_path): continue
    for line in open(extra_path):
        line = line.split('#')[0].strip()
        m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)(?: = ([0-9a-f]{2}):([0-9a-f]{4})| (extern))?$', line)
        if not m: continue
        b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
        labels.setdefault(n, (b, a)); by_addr.setdefault((b, a), n); instances.setdefault(n, [(b, a)]); extra_names.add(n)
        if extra_path.endswith('alias_seasons.sym'): alias_labels.add(n)
        if m.group(4): relocs.append((a, a + 0x80, int(m.group(4), 16), int(m.group(5), 16)))
        if m.group(6): externs.add((b, a))
def in_reloc(a): return any(lo <= a < hi for lo, hi, sb, sa in relocs)
def cname_at(bank, addr):
    n = by_addr.get((bank, addr))
    parent = n if n is not None else local_by_addr[(bank, addr)].split('@')[0]
    base = cname(n if n is not None else local_by_addr[(bank, addr)])
    inst = instances.get(parent, [])
    if len(inst) <= 1: return base
    if sum(1 for b, a in inst if b == bank) > 1: return f'{base}_b{bank:02x}_{addr:04x}' if n is not None else f'{base}_b{bank:02x}_{[a for b, a in inst if b == bank and a <= addr][-1]:04x}'
    return f'{base}_b{bank:02x}'
entries = {}
_body_ok = {}
def body_ok(bank, addr):
    if (bank, addr) in externs: return True
    if (bank, addr) not in _body_ok:
        _body_ok[(bank, addr)] = not any(kind == 'unsupported' for a, m, ln, cy, kind, info in routine_body(bank, addr))
    return _body_ok[(bank, addr)]
ported = set(l.split('#')[0].strip() for l in open(sys.argv[3]) if l.split('#')[0].strip()) if sys.argv[3] != '-' else set()
rewritten = set(l.split('#')[0].strip() for l in open('src/hooks/rewritten.txt') if l.split('#')[0].strip()) if _os.path.exists('src/hooks/rewritten.txt') and not SEASONS else set()
# Seasons-only routines rewritten by hand (src/game/seasons/*.c, hook s_<name>_hook): label names
rewritten_seasons = set(l.split('#')[0].strip() for l in open('src/hooks/rewritten_seasons.txt') if l.split('#')[0].strip()) if _os.path.exists('src/hooks/rewritten_seasons.txt') and SEASONS else set()
# Seasons: the shared C hooks that run under Seasons (tools/seasons_hooks.py) play the part of
# the rewritten routines: they are called by name and not generated
shared_hooks = {}
if SEASONS:
    for l in open('src/hooks/generated_seasons.txt'):
        pp = l.split()
        if len(pp) >= 2: shared_hooks[(int(pp[0][:2], 16), int(pp[0][3:], 16))] = pp[1]
entries.update(shared_hooks)
def is_rewritten(n, b, a):
    if SEASONS: return (b, a) in shared_hooks or n in rewritten_seasons or cname_at(b, a)[2:] in rewritten_seasons
    return n in rewritten or cname_at(b, a) in rewritten
rewritten_noverify = set(l.split('#')[0].strip() for l in open('src/hooks/rewritten_noverify.txt') if l.split('#')[0].strip()) if _os.path.exists('src/hooks/rewritten_noverify.txt') else set()
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
    for lo, hi, sb, sa in relocs:
        if lo <= a < hi: return rom[sb * 0x4000 + (sa - 0x4000) + (a - lo)]
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
    n1 = rd(bank, addr, 1) if addr + 1 < 0x8000 or in_reloc(addr) else 0
    n16 = n1 | (rd(bank, addr, 2) << 8) if addr + 2 < 0x8000 or in_reloc(addr) else 0
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
        if op == 0x76: return 'halt', 1, 1, 'halt', ()
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
        return [('ret', 1, 4, 'ret', ()), ('reti', 1, 4, 'reti', ()), ('jp hl', 1, 1, 'jphl', ()), ('ld sp,hl', 1, 2, 'spload', ('gb->sp = HL',))][p]
    if z == 2:
        if y < 4: return f'jp {CC[y]},${n16:04x}', 3, (4, 3), 'jpcc', (n16, CCEXPR[y])
        return [('ld ($ff00+c),a', 1, 2, 'op', ('mem_wr(gb, 0xff00 | C, A)',)), (f'ld (${n16:04x}),a', 3, 4, 'op', (f'mem_wr(gb, 0x{n16:04x}, A)',)),
                ('ld a,($ff00+c)', 1, 2, 'op', ('A = mem_rd(gb, 0xff00 | C)',)), (f'ld a,(${n16:04x})', 3, 4, 'op', (f'A = mem_rd(gb, 0x{n16:04x})',))][y-4]
    if z == 3:
        if y == 0: return f'jp ${n16:04x}', 3, 4, 'jp', (n16, None)
        if y == 6: return 'di', 1, 1, 'op', ('gb->ime = false; gb->ime_delay = false; gb->ime_writes++',)
        if y == 7: return 'ei', 1, 1, 'op', ('gb->ime_delay = true; gb->ime_writes++',)
    if z == 4: return f'call {CC[y]},${n16:04x}', 3, (6, 3), 'callcc', (n16, CCEXPR[y])
    if z == 5:
        if q == 0: return f'push {R16P[p].lower()}', 1, 4, 'push', (R16P[p],)
        return f'call ${n16:04x}', 3, 6, 'call', (n16, None)
    if z == 6: return f'{ALUF[y][4:]} ${n1:02x}', 2, 2, 'op', (f'{ALUF[y]}(gb, 0x{n1:02x})',)
    return f'rst ${y*8:02x}', 1, 4, 'rst', (y * 8,)

def target_bank(bank, t):
    if t < 0x4000 or t >= 0x8000: return 0
    return bank if bank != 0 else None

_bodies = {}
def routine_body(bank, start):
    """Decode every instruction reachable from start without crossing into another labeled routine or bank."""
    if (bank, start) in _bodies: return _bodies[(bank, start)]
    seen = {}
    work = [start]
    while work:
        a = work.pop()
        if a in seen or (a >= 0x8000 and not in_reloc(a)): continue
        if a != start and by_addr.get((bank, a)): continue
        m, ln, cy, kind, info = decode(bank, a)
        succ = []
        if kind == 'rst' and info[0] == 0:
            table = []
            pos = a + ln
            while pos < 0x8000:
                w = rd(bank, pos) | (rd(bank, pos + 1) << 8)
                if not (0x0150 <= w < 0x8000): break
                if table and ((bank, pos) in by_addr or (bank, pos) in local_by_addr): break
                if any(pos <= t < pos + 2 for t in table): break
                table.append(w)
                pos += 2
            if not table: table = [rd(bank, a + ln) | (rd(bank, a + ln + 1) << 8)]
            kind, info = 'jumptable', (table, a + ln)
            succ = list(table)
        elif kind in ('jp', 'jpcc'):
            succ.append(info[0])
            if kind != 'jp': succ.append(a + ln)
        elif kind in ('ret', 'reti', 'jphl', 'unsupported'):
            pass
        else:
            succ.append(a + ln)
        seen[a] = (a, m, ln, cy, kind, info)
        work.extend(t for t in succ if (t < 0x4000) == (start < 0x4000) and (t >= 0x8000) == (start >= 0x8000))
    _bodies[(bank, start)] = [seen[a] for a in sorted(seen)]
    return _bodies[(bank, start)]

def target_name(bank, t, insns_addrs):
    if t in insns_addrs: return None
    return by_addr.get((bank, t))

def cname(n): return ('s_' if SEASONS else '') + n.replace('@', '__').replace('.', '_')

def infer_banks(bank, insns):
    """For bank 0 code, map each jp/call to a switchable-bank address to the bank selected by the
    preceding `ld a,n` / `ld ($2222),a` pair, when that is what precedes it."""
    out = {}
    if bank != 0: return out
    last_a, sel = None, None
    for a, m, ln, cy, kind, info in insns:
        mm = re.match(r'ld a,\$([0-9a-f]{2})$', m)
        if mm: last_a = int(mm.group(1), 16); continue
        if m == 'ld ($2222),a': sel = last_a; continue
        if kind in ('jp', 'jpcc', 'call', 'callcc') and info[0] >= 0x4000 and sel is not None: out[a] = sel
        if kind in ('call', 'callcc'): sel = None
        if m.startswith('ld a,') or m.startswith('ldh a,') or m.startswith('pop af'): last_a = None
    return out

_switches = {}
def switches_threads(tb, t, seen=()):
    if (tb, t) in _switches: return _switches[(tb, t)]
    if (tb, t) in externs or (tb, t) in seen or t >= 0x8000: return False
    body = routine_body(tb, t)
    r = any(k in ('spload', 'jphl', 'jumptable') or (k == 'jp' and info[0] == 0x00c4) or (k == 'call' and info[0] in (0x00c4, 0xc4b7)) for _, _, _, _, k, info in body)
    if not r:
        addrs = set(a for a, *_ in body)
        for a, m, ln, cy, k, info in body:
            nxt = []
            if k in ('jp', 'jpcc', 'call', 'callcc') and info[0] not in addrs: nxt.append(info[0])
            if k not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported', 'spload') and a + ln not in addrs: nxt.append(a + ln)
            banks_here = infer_banks(tb, body)
            for x in nxt:
                nb = target_bank(tb, x)
                if nb is None: nb = banks_here.get(a)
                if nb is not None and switches_threads(nb, x, seen + ((tb, t),)): r = True; break
            if r: break
    _switches[(tb, t)] = r
    return r

def gen(name, bank=None, start=None):
    if bank is None: bank, start = labels[name]
    insns = routine_body(bank, start)
    banks_at = infer_banks(bank, insns)
    bad = [m for a, m, ln, cy, kind, info in insns if kind == 'unsupported']
    if bad:
        print(f'warning: {name} skipped ({bad[0]})', file=sys.stderr)
        return None
    addrs = set(a for a, *_ in insns)
    jump_targets = set()
    for a, m, ln, cy, kind, info in insns:
        if kind in ('jp', 'jpcc', 'callcc') and info[0] in addrs: jump_targets.add(info[0])
        if kind == 'jumptable': jump_targets.update(t for t in info[0] if t in addrs)
    gen.targets = set()
    out = Out()
    out.lines.append(f'// {bank:02x}:{start:04x}')
    out.lines.append(f'void {cname_at(bank, start)}(GB *gb) {{')
    out.emit('uint16_t sp0_ = gb->sp; (void)sp0_;')
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
        elif kind == 'reti':
            out.emit(f'RETI(0x{a:04x}); return;{com}')
        elif kind == 'halt':
            out.emit(f'HALT(0x{a:04x});{com}')
        elif kind == 'retcc':
            out.emit(f'if ({info[0]}) {{ RET_TAKEN(0x{a:04x}); return; }} I(0x{a:04x}, 2);{com}')
        elif kind in ('jp', 'jpcc'):
            t, cond = info
            taken, not_taken = (cy if isinstance(cy, tuple) else (cy, None))
            if t in addrs:
                jump = f'goto L_{t:04x};'
            else:
                tb = target_bank(bank, t)
                if tb is None: tb = banks_at.get(a)
                tn = by_addr.get((tb, t)) or local_by_addr.get((tb, t))
                gen.targets.add((tb, t))
                if (tb, t) in entries and entries[(tb, t)].endswith('_hook'): jump = f'if (hook_is(gb, 0x{t:04x}, {entries[(tb, t)]})) {{ {entries[(tb, t)]}(gb); return; }} HANDOFF(0x{t:04x});'
                elif (tb, t) in entries: jump = f'{entries[(tb, t)]}(gb); return;'
                else: jump = f'HANDOFF(0x{t:04x}); /* {tn or "unported"} */'
            if cond is None:
                out.emit(f'I(0x{a:04x}, {taken}); {jump}{com}')
            else:
                out.emit(f'if ({cond}) {{ I(0x{a:04x}, {taken}); {jump} }} I(0x{a:04x}, {not_taken});{com}')
        elif kind in ('call', 'callcc'):
            t, cond = info
            tb = target_bank(bank, t)
            if tb is None: tb = banks_at.get(a)
            tn = by_addr.get((tb, t)) or local_by_addr.get((tb, t))
            ra = a + ln
            gen.targets.add((tb, t))
            if (tb, t) in entries: c = f'CALL(0x{a:04x}, {entries[(tb, t)]}, 0x{t:04x}, 0x{ra:04x});'
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
            gen.targets.update((bank, t) for t in table if t not in addrs)
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
            out.emit(f'I(0x{a:04x}, {cy}); {info[0]}; gb->sp_loads++; HANDOFF_UP(0x{a + ln:04x});{com}')
        else:
            out.emit(f'#error unsupported instruction {m} at {a:04x}')
        falls = kind not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported', 'spload')
        nxt_a = a + ln
        if falls and nxt_a not in addrs:
            nn = by_addr.get((bank, nxt_a)) or local_by_addr.get((bank, nxt_a))
            gen.targets.add((bank, nxt_a))
            if (bank, nxt_a) in entries and entries[(bank, nxt_a)].endswith('_hook'): out.emit(f'if (hook_is(gb, 0x{nxt_a:04x}, {entries[(bank, nxt_a)]})) {{ {entries[(bank, nxt_a)]}(gb); return; }} HANDOFF(0x{nxt_a:04x});  // fallthrough')
            elif (bank, nxt_a) in entries: out.emit(f'{entries[(bank, nxt_a)]}(gb); return;  // fallthrough')
            else: out.emit(f'HANDOFF(0x{nxt_a:04x});  // fallthrough to {nn or "unlabeled"}')
    out.lines.append('}')
    gen.flags = ('H' if any(k == 'halt' for _, _, _, _, k, _ in insns) else '') + ('L' if '@' in name else '') or '-'
    return '\n'.join(out.lines)

if names and names[0] == '--out':
    outdir = names[1]
    if sys.argv[3] != '-': names = [l.split('#')[0].strip() for l in open(sys.argv[3])]
    else:
        # every routine the disassembly sources spell as code (tools/label_kinds.py), plus the extra labels
        code_labels = set(l.strip() for l in open('src/hooks/seasons_code_labels.txt') if l.strip() and not l.startswith('#')) if _os.path.exists('src/hooks/seasons_code_labels.txt') else set(instances)
        names = sorted(n for n in instances if '@' not in n and (n in code_labels or any(inst in shared_hooks or inst in externs for inst in instances[n]) or n in extra_names))
    names = [n for n in names if n]
    import collections, os
    by_bank = collections.OrderedDict()
    missing = [n for n in names if n not in labels]
    for n in missing: print(f'warning: unknown routine {n} (skipped)', file=sys.stderr)
    names = list(dict.fromkeys(n for n in names if n in labels))
    ported = set(n for n in ported if n in labels and not has_unsupported(n))
    resume_points = set()
    def discover_resume(n, b, a):
        body = routine_body(b, a)
        banks_at = infer_banks(b, body)
        found = []
        for ia, m, ln, cy, kind, info in body:
            if kind == 'spload' and (b, ia + ln) not in by_addr: local_by_addr.setdefault((b, ia + ln), f'{n}@afterSp{ia + ln:04x}'); resume_points.add((b, ia + ln)); found.append((b, ia + ln))
            if kind in ('call', 'callcc'):
                tb = target_bank(b, info[0])
                if tb is None: tb = banks_at.get(ia)
                if tb is not None and switches_threads(tb, info[0]) and (b, ia + ln) not in by_addr: local_by_addr.setdefault((b, ia + ln), f'{n}@afterCall{ia + ln:04x}'); resume_points.add((b, ia + ln)); found.append((b, ia + ln))
        return found
    for n in names:
        for (b, a) in instances.get(n, []):
            if (b, a) in externs or (not RESUME_TAILS and is_rewritten(n, b, a)): continue
            discover_resume(n, b, a)
    owner = {}
    for n in names:
        for (b, a) in instances.get(n, []):
            if (b, a) in externs: continue
            for ia, *_ in routine_body(b, a): owner.setdefault((b, ia), (n, a))
    for n in names:
        for (b, a) in instances.get(n, []):
            if (b, a) in externs: continue
            body = routine_body(b, a)
            addrs = set(ia for ia, *_ in body)
            for ia, m, ln, cy, kind, info in body:
                if kind not in ('jp', 'jpcc') or info[0] in addrs: continue
                tb = target_bank(b, info[0])
                if tb is None or (tb, info[0]) in by_addr or (tb, info[0]) in local_by_addr: continue
                o = owner.get((tb, info[0]))
                if o and o[1] != info[0]:
                    local_by_addr[(tb, info[0])] = f'{o[0]}@jump{info[0]:04x}'
                    if is_rewritten(o[0], tb, o[1]): print(f'warning: {n} jumps into rewritten {o[0]} at {info[0]:04x} (interpreted)', file=sys.stderr)
    # Seasons: a @local that no jump, call, jump table or fallthrough reaches is data (a table
    # inside the routine), not code to generate; the code map would otherwise zero it
    code_targets = set()
    if SEASONS:
        def targets_of(b, a):
            body = routine_body(b, a)
            banks_at = infer_banks(b, body)
            for ia, m, ln, cy, kind, info in body:
                if kind in ('jp', 'jpcc', 'call', 'callcc'):
                    tb = target_bank(b, info[0])
                    if tb is None: tb = banks_at.get(ia)
                    if tb is not None: code_targets.add((tb, info[0]))
                elif kind == 'jumptable': code_targets.update((b, t) for t in info[0])
                if kind not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported', 'spload'): code_targets.add((b, ia + ln))
        for n in names:
            for (b, a) in instances.get(n, []):
                if (b, a) not in externs: targets_of(b, a)
        done = set()
        while True:     # locals reached by code are code: their bodies reach more
            new_locals = [(lb, la) for (lb, la) in local_by_addr if (lb, la) in code_targets and (lb, la) not in by_addr and (lb, la) not in done]
            if not new_locals: break
            for lb, la in new_locals: done.add((lb, la)); targets_of(lb, la)
    # Seasons: an entry into the middle of a shared routine (one of its @locals, or an alias copy of
    # it in another bank) is only needed when something outside that routine's own C lands there.
    # Generated code is walked in the ROM (its calls, jumps, fall-throughs and jump tables become
    # dispatches); hand-written and shared C reach other code only through names (SYM, TAIL*, s_x,
    # x_hook) or a constant fallback address (HANDOFF/hook_continue/asm_call on b_+N or SYM(x) + N),
    # which are read from the C. Resume points and extra/alias labels are always kept.
    class WhySet(set):     # PRUNE_WHY=1 prints what keeps each generated local
        ctx = ''
        def add(self, x):
            if x not in self: self.why[x] = self.ctx
            set.add(self, x)
        def update(self, xs):
            for x in xs: self.add(x)
    reach = WhySet(); reach.why = {}
    c_refs = WhySet(); c_refs.why = {}
    hook_named = set()
    PRUNE = SEASONS and _os.environ.get('SEASONS_PRUNE', '2') != '0'
    # SEASONS_PRUNE=1 keeps the looser rules (every routine's ROM code walked, every jump-table
    # entry kept, every SYM mention counted); 0 turns pruning off
    STRICT = PRUNE and _os.environ.get('SEASONS_PRUNE', '2') == '2'
    if PRUNE:
        for n2 in names:
            for (b2, a2) in instances.get(n2, []):
                if (b2, a2) in externs or (STRICT and (is_rewritten(n2, b2, a2) or n2 in alias_labels)): continue
                reach.ctx = 'generated ' + n2
                body = routine_body(b2, a2)
                banks_at = infer_banks(b2, body)
                addrs2 = set(x[0] for x in body)
                for ia, m, ln, cy, kind, info in body:
                    if kind not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported') and ia + ln not in addrs2:
                        reach.add((b2, ia + ln))     # falls through into the next label
                    if kind == 'jumptable':
                        for t in info[0]:
                            o = owner.get((b2, t))
                            if not (STRICT and (b2, a2) in shared_hooks and o is not None and o[1] == a2): reach.add((b2, t))
                        continue
                    if kind not in ('jp', 'jpcc', 'call', 'callcc'): continue
                    tb = target_bank(b2, info[0])
                    if tb is None: tb = banks_at.get(ia)
                    if tb is None: continue
                    o = owner.get((tb, info[0]))
                    if o is None or tb != b2 or o[1] != a2 or kind in ('call', 'callcc'): reach.add((tb, info[0]))
        ident_label = {}
        for l in open('src/hooks/syms_used.txt'):
            w = l.split()
            if len(w) >= 2: ident_label.setdefault(w[0], w[1])
        seasons_sym = {}     # syms.c holds each ident's Seasons address, the right copy of a banked label
        if _os.path.exists('src/game/syms.c'):
            idents = re.findall(r'^  S_(\w+),$', open('src/game/syms.h').read().split('SYM_COUNT')[0], re.M)
            vals = re.findall(r'0x([0-9a-f]{8})', open('src/game/syms.c').read().split('syms_seasons')[1].split('};')[0])
            if len(idents) == len(vals):
                seasons_sym = {i: (int(v, 16) >> 16, int(v, 16) & 0xffff) for i, v in zip(idents, vals) if v != 'ffffffff'}
        def sym_addr(ident):
            if ident in seasons_sym: return seasons_sym[ident]
            lab = ident_label.get(ident, ident.replace('__', '@'))
            return labels.get(lab)
        def cyc_spans(line):     # a burn names the instructions it charges, not a place control goes
            spans = []
            for m in re.finditer(r'\bCYC\w*\(', line):
                depth, i = 1, m.end()
                while i < len(line) and depth:
                    depth += {'(': 1, ')': -1}.get(line[i], 0); i += 1
                spans.append((m.end(), i))
            return spans
        FALLBACK = re.compile(r'\b(?:HANDOFF|hook_continue|asm_call|CALL_ASM|CALL_C\w*\([^,]+, \w+,|CALL\([^,]+, \w+,|CALL_ROM\w*\([^,]+,)\s*\(?(?:gb, )?\(?(?:b_ ?\+ ?(\d+)|SYM\((\w+)\)(?: ?\+ ?(\d+))?)')
        for p in glob.glob('src/game/**/*.c', recursive=True):
            if _os.path.basename(p).startswith('gen_'): continue
            s = open(p, errors='replace').read()
            c_refs.ctx = reach.ctx = p
            # strict: a SYM(x) loaded into a register, inside GV() or compared with == is an address
            # (a table, a pointer, a jump-table case), not a place the C hands control to
            c_refs.update(re.findall(r'\bTAIL\w*\((\w+)\)', s))
            for line in s.split('\n'):
                burns = cyc_spans(line) if STRICT else []
                for msym in re.finditer(r'\bSYM\((\w+)\)', line):
                    if STRICT and re.search(r'(SET_(?:HL|BC|DE)\(|GV\(|== )\(*$', line[:msym.start()]): continue
                    if any(a <= msym.start() < b for a, b in burns): continue
                    if STRICT and re.match(r'\)? ?\+ ?\d', line[msym.end():]): continue     # SYM(x) + N is another address; FALLBACK reads it
                    if STRICT and re.search(r'\bCALL\w*\(\(*$', line[:msym.start()]): continue     # the call site, not the callee
                    c_refs.add(msym.group(1))
            c_refs.update(m[:-5] if m.endswith('_hook') else m for m in re.findall(r'\bs_(\w+)', s))
            # a hook passed to CALL_C falls back to its address; a hook defined or called directly only
            # matters where a jump-table chain may HANDOFF instead of calling it
            for mh in re.finditer(r'\b(\w+)_hook\b(\s*[(;]?)', s):
                if not STRICT or not mh.group(2).strip(): c_refs.add(mh.group(1))
                else: hook_named.add(mh.group(1))
            base = None
            for line in s.split('\n'):
                mb = re.search(r'\bBASE\((\w+)\)', line)
                if mb: base = sym_addr(mb.group(1))
                for off, ident, off2 in FALLBACK.findall(line):
                    at = (base[0], base[1] + int(off)) if off and base else None
                    if ident:
                        la = sym_addr(ident)
                        if la: at = (la[0], la[1] + int(off2 or 0))
                    if at: reach.add((at[0] if at[1] >= 0x4000 else 0, at[1]))
        if STRICT:
            jt_targets = set()
            starts = [i for n2 in names for i in instances.get(n2, []) if i not in externs]
            starts += [i for i in local_by_addr if i in code_targets]
            for (b2, a2) in starts:
                for ia, m, ln, cy, kind, info in routine_body(b2, a2):
                    if kind == 'jumptable': jt_targets.update((b2, t) for t in info[0])
            for ident in hook_named - c_refs:
                la = sym_addr(ident)
                if la and (la[0] if la[1] >= 0x4000 else 0, la[1]) in jt_targets:
                    reach.ctx = 'jump-table entry with C hook ' + ident
                    reach.add((la[0] if la[1] >= 0x4000 else 0, la[1]))
        for ident in list(c_refs):     # C spells labels the Ages way; reach the Seasons address too
            reach.ctx = 'name %s in %s' % (ident, c_refs.why.get(ident))
            la = sym_addr(ident)
            if la: reach.add((la[0] if la[1] >= 0x4000 else 0, la[1]))
    def reachable(b, a, n):
        return ((b, a) in reach or (b, a) in resume_points or (n in extra_names and not (STRICT and n in alias_labels))
                or n.replace('@', '__') in c_refs or cname_at(b, a)[2:] in c_refs)
    if PRUNE:     # a kept local is generated code of its own: whatever it reaches needs an entry too
        walked = set()
        while True:
            todo = [(lb, la) for (lb, la), ln in local_by_addr.items() if (lb, la) not in walked and (lb, la) not in by_addr
                    and (lb, la) in code_targets and (lb, la) not in shared_hooks and reachable(lb, la, ln)]
            if STRICT: todo += [labels[n2] for n2 in alias_labels if labels[n2] not in walked and labels[n2] not in shared_hooks and reachable(*labels[n2], n2)]
            if not todo: break
            for lb, la in todo:
                walked.add((lb, la))
                reach.ctx = 'kept local ' + (local_by_addr.get((lb, la)) or by_addr.get((lb, la)))
                body = routine_body(lb, la)
                banks_at = infer_banks(lb, body)
                addrs2 = set(x[0] for x in body)
                for ia, m, ln, cy, kind, info in body:
                    if kind not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported') and ia + ln not in addrs2: reach.add((lb, ia + ln))
                    if kind == 'jumptable': reach.update((lb, t) for t in info[0]); continue
                    # a jump inside the body becomes a goto; a call to a labelled address needs its entry
                    if kind in ('call', 'callcc') or (kind in ('jp', 'jpcc') and info[0] not in addrs2):
                        tb = target_bank(lb, info[0])
                        if tb is None: tb = banks_at.get(ia)
                        if tb is not None: reach.add((tb, info[0]))
    locals_of = collections.defaultdict(list)
    for (lb, la), ln in local_by_addr.items():
        if (lb, la) not in by_addr and (not SEASONS or (lb, la) in code_targets): locals_of[(lb, ln.split('@')[0])].append((la, ln))
    alias_names = collections.defaultdict(list)      # several labels can name one routine; its @locals may hang off any of them
    for n2, insts in instances.items():
        if '@' not in n2:
            for inst in insts: alias_names[inst].append(n2)
    def locals_for(b, a, n):
        out = set(locals_of.get((b, n), []))
        for n2 in alias_names.get((b, a), []): out.update(locals_of.get((b, n2), []))
        return sorted(out)
    items_by_bank = collections.OrderedDict()
    seen_addrs = set()
    pruned = []
    for n in names:
        for (b, a) in instances.get(n, []):
            if (b, a) in seen_addrs: continue
            seen_addrs.add((b, a))
            if is_rewritten(n, b, a):
                entries[(b, a)] = shared_hooks[(b, a)] if (b, a) in shared_hooks else cname_at(b, a) + '_hook'
                items_by_bank.setdefault(b, []).append((n, b, a))
                if SEASONS and (b, a) not in shared_hooks:     # @locals the hand file hooks itself (listed as parent@local)
                    for la, ln in locals_for(b, a, n):
                        if ln in rewritten_seasons and (b, la) not in entries:
                            entries[(b, la)] = cname_at(b, la) + '_hook'
                            items_by_bank[b].append((ln, b, la))
                if (b, a) in shared_hooks:     # a shared routine's @locals that no shared hook covers are generated
                    for la, ln in locals_for(b, a, n):
                        if (b, la) in shared_hooks or (b, la) in entries or not body_ok(b, la): continue
                        if PRUNE and not reachable(b, la, ln): pruned.append(ln); continue
                        if PRUNE and _os.environ.get('PRUNE_WHY'):
                            print('kept', ln, '<-', reach.why.get((b, la)) or c_refs.why.get(ln.replace('@', '__')) or c_refs.why.get(cname_at(b, la)[2:]) or 'resume/extra', file=sys.stderr)
                        entries[(b, la)] = cname_at(b, la)
                        items_by_bank[b].append((ln, b, la))
                # A rewritten routine still gets generated resume tails at its post-yield return
                # addresses: the yield discards its C frames, so nothing else can run them.
                # Those tails call the routine's own @-locals, which the hand-written C reaches
                # only as static helpers, so every local that is a call target is generated too.
                if not RESUME_TAILS: continue
                body = routine_body(b, a)
                called = set(info[0] for ia, m, ln, cy, kind, info in body if kind in ('call', 'callcc'))
                tails = [(la, ln) for la, ln in locals_for(b, a, n) if (b, la) in resume_points]
                tails += [(la, ln) for (lb, la), ln in local_by_addr.items() if lb == b and la in called and ln.split('@')[0] in alias_names.get((b, a), [n]) and (b, la) not in by_addr]
                for la, ln in sorted(set(tails)):
                    if is_rewritten(ln, b, la): continue
                    if not body_ok(b, la): print(f'warning: {ln} skipped (unsupported)', file=sys.stderr); continue
                    if (b, la) in entries: continue
                    entries[(b, la)] = cname_at(b, la)
                    items_by_bank[b].append((ln, b, la))
                continue
            if (PRUNE and (b, a) not in externs and (any(i in shared_hooks for i in instances.get(n, [])) or (STRICT and n in alias_labels))
                    and not reachable(b, a, n)):     # an alias copy of a shared routine, or an Ages spelling, that nothing reaches
                pruned.append(cname_at(b, a)); continue
            if body_ok(b, a): entries[(b, a)] = cname_at(b, a)[2:] if SEASONS and (b, a) in externs else cname_at(b, a)
            items_by_bank.setdefault(b, []).append((n, b, a))
            for la, ln in locals_for(b, a, n):
                if SEASONS and (b, la) in shared_hooks: entries[(b, la)] = shared_hooks[(b, la)]; continue
                if not body_ok(b, la): print(f'warning: {ln} skipped (unsupported)', file=sys.stderr); continue
                entries[(b, la)] = cname_at(b, la)
                items_by_bank[b].append((ln, b, la))
    for n in names:
        by_bank.setdefault(labels[n][0], []).append(n)
    with open(os.path.join(outdir, 'gen.h'), 'w') as h:
        h.write('// generated by tools/transliterate.py; do not edit\n#pragma once\n#include "core/gb.h"\n')
        for n in names: h.write(f'void {cname(n)}(GB *gb);\n')
    generated = []
    by_bank = items_by_bank
    bank_outputs = {}
    bank_bodies = collections.defaultdict(list)
    pending = []
    def is_rewritten_parent(ln):
        parent = ln.split('@')[0]
        if parent not in labels: return False
        pb, pa = labels[parent]
        return is_rewritten(parent, pb, pa)
    for bank, items in by_bank.items():
        for n, b, a in items:
            if is_rewritten(n, b, a):
                if not SEASONS or (b, a) not in shared_hooks: generated.append((b, a, cname_at(b, a) + '_hook', 'H' if (n in rewritten_noverify or cname_at(b, a) in rewritten_noverify) else '-'))
                continue
            if (b, a) in externs:
                # hand-written (ram_code.c), game-neutral: the same function serves both games
                generated.append((b, a, cname_at(b, a)[2:] if SEASONS else cname_at(b, a), '-'))
                continue
            if RESUME_TAILS and '@' in n and is_rewritten_parent(n): pending.extend(discover_resume(n.split('@')[0], b, a))
            code = gen(n, b, a)
            if code: bank_bodies[bank].append(code); generated.append((b, a, cname_at(b, a), gen.flags)); pending.extend(gen.targets)
    if SEASONS and not RESUME_TAILS:
        # A local of a hand-rewritten routine that generated code calls or jumps to has no entry
        # unless the hand file hooks it (a parent@local line in rewritten_seasons.txt); the native
        # build has no interpreter to fall back on there.
        for tb, t in sorted(set(pending), key=lambda x: (x[0] if x[0] is not None else -1, x[1])):
            if (tb, t) in entries or (tb, t) in by_addr or (tb, t) not in local_by_addr or t >= 0x8000: continue
            ln = local_by_addr[(tb, t)]
            if is_rewritten_parent(ln): print(f'warning: {ln} is reached from generated code but has no hook: add it to rewritten_seasons.txt and hook it by hand', file=sys.stderr)
    # Generated code that lands on an @-local of a rewritten routine (a jump-table case, a
    # fallthrough, a call) has no C for it: the hand-written file keeps those as static helpers.
    # Generate every such local, to a fixed point, so the dispatcher always finds an entry.
    while pending and RESUME_TAILS:
        tb, t = pending.pop()
        if (tb, t) in entries or (tb, t) in by_addr or (tb, t) not in local_by_addr or t >= 0x8000: continue
        ln = local_by_addr[(tb, t)]
        if not is_rewritten_parent(ln) or is_rewritten(ln, tb, t): continue
        if not body_ok(tb, t): print(f'warning: {ln} skipped (unsupported)', file=sys.stderr); continue
        entries[(tb, t)] = cname_at(tb, t)
        pending.extend(discover_resume(ln.split('@')[0], tb, t))
        code = gen(ln, tb, t)
        if code: bank_bodies[tb].append(code); generated.append((tb, t, cname_at(tb, t), gen.flags)); pending.extend(gen.targets)
    for bank, bodies in bank_bodies.items():
        bank_outputs[os.path.join(outdir, f'gen_bank{bank:02x}.c')] = bodies
    for path in glob.glob(os.path.join(outdir, 'gen_bank[0-9a-f][0-9a-f].c')):
        if path not in bank_outputs: os.remove(path)
    for path, bodies in bank_outputs.items():
        with open(path, 'w') as f:
            f.write('// generated by tools/transliterate.py; do not edit\n#include "game/asm.h"\n#include "' + ('game/seasons/gen.h' if SEASONS else 'game/gen.h') + '"\n\n')
            f.write('\n\n'.join(bodies) + '\n\n')
    with open(os.path.join(outdir, 'gen.h'), 'w') as h:
        h.write('// generated by tools/transliterate.py; do not edit\n#pragma once\n#include "core/gb.h"\n' + ('#include "game/gen.h"\n' if SEASONS else ''))
        for b, a, cn, fl in generated: h.write(f'void {cn}(GB *gb);\n')
        if SEASONS:     # a routine that became a shared hook keeps its s_ name for the hand-written Seasons files
            gen_names = set(cn for b, a, cn, fl in generated)
            for (b, a), hook in sorted(shared_hooks.items()):
                for n in alias_names.get((b, a), []):
                    if cname(n) not in gen_names: h.write(f'#define {cname(n)} {hook}\n')
    with open('src/hooks/generated_seasons_gen.txt' if SEASONS else 'src/hooks/generated.txt', 'w') as g:
        for b, a, cn, fl in generated: g.write(f'{b:02x}:{a:04x} {cn} {fl}\n')
    print(f'{len(generated)} routines in {len(bank_outputs)} bank files' + (f'; {len(pruned)} unreachable entries into shared routines not generated' if pruned else ''))
elif names and names[0] == '--report':
    import glob
    src_files = sorted(glob.glob('ref/oracles-disasm/code/**/*.s', recursive=True) + glob.glob('ref/oracles-disasm/object_code/**/*.s', recursive=True))
    def source_comment(n):
        for path in src_files:
            lines = open(path, errors='replace').read().split('\n')
            for i, l in enumerate(lines):
                if l.rstrip() == n + ':':
                    j = i
                    while j > 0 and lines[j - 1].startswith(';'): j -= 1
                    return path, i + 1, lines[j:i]
        return None, None, []
    pnames = [l.split('#')[0].strip() for l in open(sys.argv[3])] if sys.argv[3] != '-' else []
    pnames = [x for x in pnames if x and x in labels]
    def report(n):
        if n not in labels: print(f'unknown routine {n}'); return
        path, ln, com = source_comment(n)
        print(f'== {n}  ({path}:{ln})' if path else f'== {n}')
        for l in com: print(l)
        for (b, a) in instances.get(n, [labels[n]]):
            body = routine_body(b, a)
            addrs = set(ia for ia, *_ in body)
            banks_at = infer_banks(b, body)
            targets = set()
            for ia, m, l_, cy, kind, info in body:
                if kind in ('jp', 'jpcc', 'callcc') and info[0] in addrs: targets.add(info[0])
                if kind == 'jumptable': targets.update(t for t in info[0] if t in addrs)
            print(f'-- {b:02x}:{a:04x}  {len(body)} instructions')
            block, bsum, first = None, 0, True
            def flush():
                if block is not None: print(f'   [{block:04x}] block {bsum} cycles')
            for ia, m, l_, cy, kind, info in body:
                if ia == a or ia in targets or (b, ia) in local_by_addr:
                    flush(); block, bsum = ia, 0
                    lab = local_by_addr.get((b, ia)) or ('' if ia == a else f'L_{ia:04x}')
                    print(f'{lab}:' if lab else '')
                c = cy if not isinstance(cy, tuple) else cy[1]
                bsum += c
                cs = f'{cy[0]}/{cy[1]}' if isinstance(cy, tuple) else f'{cy}'
                note = ''
                if kind in ('call', 'callcc', 'jp', 'jpcc') and info[0] not in addrs:
                    tb = target_bank(b, info[0])
                    if tb is None: tb = banks_at.get(ia)
                    tn = by_addr.get((tb, info[0])) or local_by_addr.get((tb, info[0])) or '?'
                    fl = []
                    if tb is not None:
                        if tn in rewritten: fl.append('rewritten')
                        elif tn in ported: fl.append('hooked')
                        else: fl.append('NOT HOOKED')
                        if switches_threads(tb, info[0]): fl.append('SWITCHES THREADS')
                        tbody = routine_body(tb, info[0])
                        if any(k == 'halt' for _, _, _, _, k, _ in tbody): fl.append('halt')
                        if any(k == 'unsupported' for _, _, _, _, k, _ in tbody): fl.append('unsupported')
                    note = f'  -> {tn} ({", ".join(fl)})'
                if kind in ('halt', 'spload', 'unsupported', 'jphl', 'jumptable', 'rst'): note += f'  !! {kind}'
                print(f'  {ia:04x}  {cs:>4}  {m}{note}')
            flush()
            last = body[-1]
            if last[4] not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported', 'spload'):
                nxt = last[0] + last[2]
                print(f'   falls through to {by_addr.get((b, nxt)) or local_by_addr.get((b, nxt)) or f"{nxt:04x}"}')
            callers, jumpers, fallers = [], [], []
            for pn in pnames:
                for (pb, pa) in instances.get(pn, []):
                    if (pb, pa) == (b, a) or (pb, pa) in externs: continue
                    pbody = routine_body(pb, pa)
                    pbanks = infer_banks(pb, pbody)
                    paddrs = set(ia for ia, *_ in pbody)
                    for ia, m, l_, cy, kind, info in pbody:
                        if kind in ('jp', 'jpcc', 'call', 'callcc') and info[0] not in paddrs:
                            tb = target_bank(pb, info[0])
                            if tb is None: tb = pbanks.get(ia)
                            if tb != b or info[0] not in addrs: continue
                            if info[0] == a: callers.append(pn)
                            else: jumpers.append(f'{pn} -> {info[0]:04x}')
                    pl = pbody[-1]
                    if pl[4] not in ('ret', 'reti', 'jp', 'jphl', 'jumptable', 'unsupported', 'spload') and pb == b and pl[0] + pl[2] == a: fallers.append(pn)
            print(f'   callers: {len(callers)}' + (f' ({", ".join(sorted(set(callers))[:12])}{"..." if len(set(callers)) > 12 else ""})' if callers else ''))
            if jumpers: print(f'   INCOMING JUMPS: {", ".join(sorted(set(jumpers)))}')
            if fallers: print(f'   FALLEN INTO FROM: {", ".join(sorted(set(fallers)))}')
        print()
    for n in names[1:]: report(n)
else:
    print('#include "game/asm.h"\n#include "game/gen.h"\n')
    for n in names:
        print(gen(n)); print()
