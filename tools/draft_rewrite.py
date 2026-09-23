#!/usr/bin/env python3
"""Draft a hand-written Seasons hook in the house idiom, to be edited into readable form.

usage: tools/draft_rewrite.py ROUTINE [ROUTINE...] > draft.c

For each routine (a top-level label of ref/oracles-disasm/seasons.sym) it walks the code reachable
from the entry up to the next top-level label and prints `void s_<name>_hook(GB *gb)` with one
`CYC(b_+X, b_+Y)` per instruction and its effect: RAM operands by their ram.h name (checked the way
tools/audit_addr.py checks them), calls as `CALL_C(.., s_x, SYM(x), ..)`, jumps into other routines
as TAIL/TAIL_S/TAIL_SG, conditional branches with CYCT on the taken side, rst $00 jump tables as an
if-chain of gotos, rst $10/$18 through `<prefix>_add_a_to_hl`/`<prefix>_add_double_index` helpers
the file must define. Locals of the routine become C labels. Anything it cannot place is marked
`TODO`. Symbols the draft needs in src/hooks/syms_used.txt are listed at the end as a comment.
The draft is not the deliverable: name things, add comments, factor shared tails, then run the
audits (audit_bounds, audit_imm, audit_addr, audit_calls, audit_retcc) before gating."""
import re, sys
ROM = open('roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc', 'rb').read()
PREFIX = 'draft'
args = []
for a in sys.argv[1:]:
    if a.startswith('--prefix='): PREFIX = a.split('=', 1)[1]
    elif a.startswith('--'): pass
    else: args.append(a)

tops, locs, name_at = {}, {}, {}
ram_sym = {}
for line in open('ref/oracles-disasm/seasons.sym'):
    m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
    if not m: continue
    b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
    if a >= 0x8000:
        ram_sym.setdefault(a, []).append(n.replace('.', '_'))
        continue
    if '@' in n: locs.setdefault((b, a), n)
    else:
        tops.setdefault(n, (b, a))
        name_at.setdefault((b, a), n)
top_addrs = {}
for n, (b, a) in tops.items(): top_addrs.setdefault(b, set()).add(a)
if 0 not in top_addrs: top_addrs[0] = set()

h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
rids = re.findall(r'^\s+R_(\w+),', h, re.M)
rvals = re.findall(r'0x([0-9a-f]{8})', re.search(r'ram_seasons\[RAMSYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
ramsym = {i: int(v, 16) for i, v in zip(rids, rvals)}
ram_names = {}
for path in ('src/game/ram.h', 'src/game/game.h'):
    for m in re.finditer(r'^#define (\w+) (0x[0-9a-fA-F]+|RAMSYM\(R_(\w+)\))\s*$', open(path).read(), re.M):
        if m.group(1).endswith('_BANK'): continue
        v = ramsym.get(m.group(3)) if m.group(3) else int(m.group(2), 16)
        if v is not None and v >= 0x8000: ram_names.setdefault(v, []).append(m.group(1))

MENU = ('wFileSelect', 'wMenu', 'wInventory', 'wRingMenu', 'wMapMenu', 'wSaveQuit', 'wSecretList', 'wIntro')
def rank(n):
    r = 0
    if n.startswith('wTmp'): r -= 3
    if n.startswith('wGenericCutscene'): r -= 2
    if n.startswith(MENU): r += 4
    if re.search(r'Block|Start|End|Filler|Union|_cb[a-f0-9]{2}$', n): r += 3
    if '_' in n: r += 1
    return (r, len(n))

def ram(v):
    if 0xcfc0 < v < 0xd000: return f'wTmpcfc0 + 0x{v - 0xcfc0:02x}'
    names = ram_names.get(v, [])
    cands = [n for n in ram_sym.get(v, []) if n in names] or names
    if cands: return min(cands, key=rank)
    for d in range(1, 0x40):
        for n in ram_sym.get(v - d, []):
            if n in ram_names.get(v - d, []): return f'{n} + 0x{d:02x}'
        if ram_names.get(v - d): return f'{ram_names[v - d][0]} + 0x{d:02x}'
    return f'0x{v:04x} /* TODO name */'

ages_spelling = {}
for line in open('src/hooks/syms_used.txt'):
    p = line.split()
    if len(p) >= 2: ages_spelling.setdefault(p[1], p[0])
hand = set(l.split('#')[0].strip() for l in open('src/hooks/rewritten_seasons.txt'))
gen = set(l.split()[1] for l in open('src/hooks/generated_seasons_gen.txt') if len(l.split()) >= 2)
shared = set()
for l in open('src/hooks/generated_seasons.txt'):
    p = l.split()
    if len(p) >= 2: shared.add(p[1])
needed = set()

def symref(n):
    s = ages_spelling.get(n, n)
    if n not in ages_spelling: needed.add(n)
    return s

def fn_of(n):
    if n in hand: return f's_{n}_hook'
    return f's_{n}'

def tail_of(n):
    if n in hand: return f'TAIL_S({symref(n)});'
    s = ages_spelling.get(n, n)
    if f'{s}_hook' in shared: return f'TAIL({s});'
    return f'TAIL_SG({symref(n)});'

def rd(bank, a):
    return ROM[a if a < 0x4000 else bank * 0x4000 + a - 0x4000]

R8 = ['B', 'C', 'D', 'E', 'H', 'L', '(hl)', 'A']
OBJ = '--obj' in sys.argv
fields = {}
for m in re.finditer(r'^#define (OBJ_\w+) 0x([0-9a-f]{2})$', open('src/game/game.h').read(), re.M):
    fields.setdefault(int(m.group(2), 16), []).append(m.group(1))
def field(n):
    base = {0x40: 'INTERACTION_BASE', 0x80: 'ENEMY_BASE', 0xc0: 'PART_BASE'}.get(n & 0xc0)
    names = fields.get(n & 0x3f)
    if not base or not names: return f'0x{n:02x}'
    var_first = base != 'INTERACTION_BASE' and (n & 0x3f) >= 0x2a
    best = min(names, key=lambda s: ((0 if 'VAR' in s else 3) if var_first else 2 * ('VAR' in s) + ('SCRIPT' in s or 'TEXT' in s or 'PRESSED' in s or 'COLLIDE' in s), names.index(s)))
    return f'{base} + {best}'
def r8get(i): return 'mem_rd(gb, HL)' if i == 6 else R8[i]
def r8set(i, v): return f'mem_wr(gb, HL, {v});' if i == 6 else f'{R8[i]} = {v};'
RP = ['BC', 'DE', 'HL', 'SP']
CC = ['!(F & FZ)', 'F & FZ', '!(F & FC)', 'F & FC']
ALU = ['alu_add', 'alu_adc', 'alu_sub', 'alu_sbc', 'alu_and', 'alu_xor', 'alu_or', 'alu_cp']
ROT = ['alu_rlc', 'alu_rrc', 'alu_rl', 'alu_rr', 'alu_sla', 'alu_sra', 'alu_swap', 'alu_srl']

def length(op):
    if op == 0xcb: return 2
    if op in (0x01, 0x11, 0x21, 0x31, 0x08, 0xc2, 0xc3, 0xc4, 0xca, 0xcc, 0xcd, 0xd2, 0xd4, 0xda, 0xdc, 0xea, 0xfa): return 3
    if op in (0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e, 0x18, 0x20, 0x28, 0x30, 0x38, 0xc6, 0xce, 0xd6, 0xde,
              0xe6, 0xee, 0xf6, 0xfe, 0xe0, 0xf0, 0xe8, 0xf8, 0x10): return 2
    return 1

def imm16_expr(bank, v, base, lo, hi, guess=True):
    if v >= 0x8000 and v < 0xa000: return f'0x{v:04x}'
    if v >= 0xa000: return ram(v) if v in ram_names or (v >= 0xc000 and v < 0xe000) or v >= 0xff80 else f'0x{v:04x}'
    if lo <= v < hi: return f'b_+{v - base}'
    n = name_at.get((bank if v >= 0x4000 else 0, v))
    if n: return f'SYM({symref(n)})'
    loc = locs.get((bank if v >= 0x4000 else 0, v))
    if not guess: return f'0x{v:04x}'
    if loc: return f'0x{v:04x} /* TODO {loc} */'
    other = [nm for (b, a), nm in name_at.items() if a == v and v >= 0x4000]
    if other: return f'0x{v:04x} /* TODO SYM of one of: {" ".join(sorted(other)[:6])} */'
    return f'0x{v:04x}'

def draft(name, start=None, helper=None, pending=None):
    bank, base = tops[name]
    later = sorted(a for a in top_addrs.get(bank if base >= 0x4000 else 0, set()) if a > base)
    hi = later[0] if later else base + 0x400
    entry = base if start is None else start
    seen, work, tables = {}, [entry], {}
    labels = set()
    while work:
        a = work.pop()
        while base <= a < hi and a not in seen:
            op = rd(bank, a); n = length(op); seen[a] = n
            nxt = a + n
            if op in (0x18, 0x20, 0x28, 0x30, 0x38):
                t = (nxt + ((rd(bank, a + 1) ^ 0x80) - 0x80)) & 0xffff
                if base <= t < hi: labels.add(t); work.append(t)
                if op == 0x18: break
            elif op in (0xc3, 0xc2, 0xca, 0xd2, 0xda):
                t = rd(bank, a + 1) | rd(bank, a + 2) << 8
                if base <= t < hi and t not in name_at: labels.add(t); work.append(t)
                if op == 0xc3: break
            elif op in (0xc4, 0xcc, 0xd4, 0xdc, 0xcd):
                pass
            elif op in (0xc9, 0xd9, 0xe9): break
            elif op == 0xc7:
                ents, p = [], nxt
                first = None
                while p + 1 < hi and (first is None or p < first):
                    t = rd(bank, p) | rd(bank, p + 1) << 8
                    if not (base <= t < hi) and (t < 0x4000 or ((bank, t) not in name_at and (bank, t) not in locs)): break
                    ents.append(t)
                    if base <= t < hi:
                        first = t if first is None else min(first, t)
                        labels.add(t); work.append(t)
                    p += 2
                    if len(ents) > 64: break
                tables[a] = ents
                break
            a = nxt
    if helper:
        out = [f'// {locs.get((bank, entry), hex(entry))}', f'static void {helper}(GB *gb) {{', f'  BASE({symref(name)});', '  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;']
    else:
        out = [f'void s_{name}_hook(GB *gb) {{', f'  BASE({symref(name)});', '  uint16_t sp0_ = gb->sp; (void)sp0_;']
    def lab(t):
        l = locs.get((bank, t))
        return (l.split('@', 1)[1] if l else f'L_{t:04x}').replace('@', '_')
    prev_end = None
    for a in sorted(seen):
        op = rd(bank, a); n = seen[a]; e = a + n; x, y = a - base, e - base
        if prev_end is not None and a != prev_end: out.append('')
        if a in labels: out.append(f'{lab(a)}:')
        prev_end = e
        cy = f'CYC(b_+{x}, b_+{y});'
        cyt = f'CYCT(b_+{x}, b_+{y});'
        n1 = rd(bank, a + 1) if n > 1 else 0
        n16 = n1 | (rd(bank, a + 2) << 8) if n > 2 else 0
        s = None
        hi3, mid, lo3 = op >> 6, (op >> 3) & 7, op & 7
        def jump(t, cond=None, kind='jp'):
            if base <= t < hi and t in labels:
                go = f'goto {lab(t)};'
            elif (bank if t >= 0x4000 else 0, t) in name_at:
                go = tail_of(name_at[(bank if t >= 0x4000 else 0, t)])
            else:
                l = locs.get((bank if t >= 0x4000 else 0, t))
                go = f'/* TODO jump to {l or hex(t)} */ HANDOFF(0x{t:04x});'
            if cond is None: return f'  {cy}\n  {go}'
            return f'  if ({cond}) {{ {cyt} {go} }}\n  {cy}'
        if op == 0x00: s = f'  {cy}'
        elif hi3 == 1 and op != 0x76:
            src = r8get(lo3)
            s = f'  {cy} ' + r8set(mid, src)
        elif hi3 == 0 and lo3 == 6: s = f'  {cy} ' + r8set(mid, field(n1) if OBJ and mid in (3, 5) and n1 >= 0x40 else f'0x{n1:02x}')
        elif hi3 == 0 and lo3 == 1 and mid % 2 == 0:
            rp = RP[mid // 2]
            v = imm16_expr(bank, n16, base, base, hi, guess=(rp == 'HL'))
            if rp in ('BC', 'DE') and v.startswith('b_'):
                loc = locs.get((bank, n16))
                v += f' /* @{loc.split("@", 1)[1]} */' if loc else f' /* TODO pointer or the constant 0x{n16:04x}? */'
            elif rp == 'HL' and v.startswith('b_') and locs.get((bank, n16)):
                v += f' /* @{locs[(bank, n16)].split("@", 1)[1]} */'
            s = f'  {cy} SET_{rp}({v});' if rp != 'SP' else f'  {cy} /* TODO ld sp */'
        elif hi3 == 0 and lo3 == 3:
            rp = RP[mid // 2]
            s = f'  {cy} SET_{rp}({rp} {"+" if mid % 2 == 0 else "-"} 1);'
        elif hi3 == 0 and lo3 == 4: s = f'  {cy} ' + (f'mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));' if mid == 6 else f'{R8[mid]} = alu_inc8(gb, {R8[mid]});')
        elif hi3 == 0 and lo3 == 5: s = f'  {cy} ' + (f'mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));' if mid == 6 else f'{R8[mid]} = alu_dec8(gb, {R8[mid]});')
        elif hi3 == 0 and lo3 == 1: s = f'  {cy} alu_add_hl(gb, {RP[mid // 2]});'
        elif op == 0x02: s = f'  {cy} mem_wr(gb, BC, A);'
        elif op == 0x12: s = f'  {cy} mem_wr(gb, DE, A);'
        elif op == 0x0a: s = f'  {cy} A = mem_rd(gb, BC);'
        elif op == 0x1a: s = f'  {cy} A = mem_rd(gb, DE);'
        elif op == 0x22: s = f'  {cy} mem_wr(gb, HL, A); SET_HL(HL + 1);'
        elif op == 0x32: s = f'  {cy} mem_wr(gb, HL, A); SET_HL(HL - 1);'
        elif op == 0x2a: s = f'  {cy} A = mem_rd(gb, HL); SET_HL(HL + 1);'
        elif op == 0x3a: s = f'  {cy} A = mem_rd(gb, HL); SET_HL(HL - 1);'
        elif op in (0x07, 0x0f, 0x17, 0x1f, 0x27, 0x2f, 0x37, 0x3f):
            s = f'  {cy} ' + {0x07: 'alu_rlca(gb);', 0x0f: 'alu_rrca(gb);', 0x17: 'alu_rla(gb);', 0x1f: 'alu_rra(gb);',
                              0x27: 'alu_daa(gb);', 0x2f: 'alu_cpl(gb);', 0x37: 'alu_scf(gb);', 0x3f: 'alu_ccf(gb);'}[op]
        elif hi3 == 2: s = f'  {cy} {ALU[mid]}(gb, {r8get(lo3)});'
        elif hi3 == 3 and lo3 == 6: s = f'  {cy} {ALU[mid]}(gb, 0x{n1:02x});'
        elif op == 0xea: s = f'  {cy} mem_wr(gb, {imm16_expr(bank, n16, base, base, hi)}, A);'
        elif op == 0xfa: s = f'  {cy} A = mem_rd(gb, {imm16_expr(bank, n16, base, base, hi)});'
        elif op == 0xe0: s = f'  {cy} mem_wr(gb, {ram(0xff00 | n1)}, A);'
        elif op == 0xf0: s = f'  {cy} A = mem_rd(gb, {ram(0xff00 | n1)});'
        elif op == 0xe2: s = f'  {cy} mem_wr(gb, 0xff00 | C, A);'
        elif op == 0xf2: s = f'  {cy} A = mem_rd(gb, 0xff00 | C);'
        elif op in (0xc5, 0xd5, 0xe5, 0xf5): s = f'  {cy} push_effect(gb, {["BC", "DE", "HL", "AF"][(op >> 4) - 0xc]});'
        elif op in (0xc1, 0xd1, 0xe1, 0xf1): s = f'  {cy} SET_{["BC", "DE", "HL", "AF"][(op >> 4) - 0xc]}(pop_effect(gb));'
        elif op == 0xcb:
            o = n1; k, b3, r = o >> 6, (o >> 3) & 7, o & 7
            if k == 0: s = f'  {cy} ' + r8set(r, f'{ROT[b3]}(gb, {r8get(r)})')
            elif k == 1: s = f'  {cy} alu_bit(gb, {b3}, {r8get(r)});'
            elif k == 2: s = f'  {cy} ' + r8set(r, f'{r8get(r)} & 0x{(~(1 << b3)) & 0xff:02x}')
            else: s = f'  {cy} ' + r8set(r, f'{r8get(r)} | 0x{1 << b3:02x}')
        elif op == 0x18: s = jump((e + ((n1 ^ 0x80) - 0x80)) & 0xffff)
        elif op in (0x20, 0x28, 0x30, 0x38): s = jump((e + ((n1 ^ 0x80) - 0x80)) & 0xffff, CC[(op >> 3) & 3])
        elif op == 0xc3: s = jump(n16)
        elif op in (0xc2, 0xca, 0xd2, 0xda): s = jump(n16, CC[(op >> 3) & 3])
        elif op in (0xcd, 0xc4, 0xcc, 0xd4, 0xdc):
            t = n16; tb = bank if t >= 0x4000 else 0
            if base <= t < hi:
                callee, target, local = f'{PREFIX}_{lab(t)}', None, True
                if pending is not None: pending.setdefault(t, callee)
            elif (tb, t) in name_at: nm = name_at[(tb, t)]; callee, target, local = fn_of(nm), f'SYM({symref(nm)})', False
            else: callee, target, local = f'TODO_{locs.get((tb, t), hex(t))}', f'0x{t:04x}', False
            if op == 0xcd:
                s = f'  CALL_L(b_+{x}, {callee}, b_+{y});' if local else f'  CALL_C(b_+{x}, {callee}, {target}, b_+{y});'
            else:
                c1 = f'CALL_L_CC(b_+{x}, {callee}, b_+{y});' if local else f'CALL_C_CC(b_+{x}, {callee}, {target}, b_+{y});'
                s = f'  if ({CC[(op >> 3) & 3]}) {c1}\n  else {cy}'
        elif op == 0xc9: s = f'  RET(b_+{x}); return;'
        elif op in (0xc0, 0xc8, 0xd0, 0xd8): s = f'  if ({CC[(op >> 3) & 3]}) {{ RET_TAKEN(b_+{x}); return; }}\n  CYC(b_+{x}, b_+{y});'
        elif op == 0xe9: s = f'  {cy}\n  HANDOFF(HL);'
        elif op == 0xd7: s = f'  {cy} {PREFIX}_add_a_to_hl(gb, b_+{y});'
        elif op == 0xdf: s = f'  {cy} {PREFIX}_add_double_index(gb, b_+{y});'
        elif op == 0xc7:
            lines = [f'  {cy} push_effect(gb, b_+{y});', f'  do {{ uint16_t jt_ = ({PREFIX}_jump_table(gb));']
            done = set()
            for t in tables.get(a, []):
                if t in done: continue
                done.add(t)
                if base <= t < hi: lines.append(f'    if (jt_ == b_+{t - base}) goto {lab(t)};')
                elif (bank, t) in name_at:
                    nm = name_at[(bank, t)]; f = fn_of(nm)
                    lines.append(f'    if (jt_ == SYM({symref(nm)}) && hook_is(gb, SYM({symref(nm)}), {f})) {{ {f}(gb); return; }}')
                elif (bank, t) in locs:
                    lines.append(f'    if (jt_ == 0x{t:04x}) {{ /* TODO {locs[(bank, t)]} */ HANDOFF(HL); }}')
            lines.append('    HANDOFF(HL);')
            lines.append('  } while (0);')
            s = '\n'.join(lines)
            prev_end = None
        else: s = f'  {cy} /* TODO opcode ${op:02x} */'
        out.append(s)
        if e == hi and op not in (0x18, 0xc3, 0xc9, 0xd9, 0xe9, 0xc7) and (bank, hi) in name_at:
            nm = name_at[(bank, hi)]
            out.append(f'  {fn_of(nm)}(gb); return; // falls through')
    out.append('}')
    return '\n'.join(out)

for n in args:
    pending, done, helpers = {}, set(), []
    main = draft(n, pending=pending)
    while set(pending) - done:
        t = min(set(pending) - done); done.add(t)
        helpers.append(draft(n, start=t, helper=pending[t], pending=pending))
    for h in reversed(helpers): print(h); print()
    print(main); print()
missing = sorted(x for x in needed if not re.search(rf'^{re.escape(x)} ', open('src/hooks/syms_used.txt').read(), re.M))
if missing: print('// syms_used.txt needs: ' + ' '.join(missing))
