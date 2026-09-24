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
WB_OVERRIDE = {}
args = []
for a in sys.argv[1:]:
    if a.startswith('--prefix='): PREFIX = a.split('=', 1)[1]
    elif a.startswith('--wbank='):
        WB_OVERRIDE = {k: int(v) for k, v in (x.split(':') for x in a.split('=', 1)[1].split(','))}
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
for (b, a) in name_at: top_addrs.setdefault(b, set()).add(a)
def noncanonical(b, a):
    n = name_at.get((b, a))
    return n is not None and tops[n][1] != a
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

WBANK = [1]
WINDOW = [0x100]
def bank_name(n, b):
    pre = f'w{b}'
    return n.startswith(pre) and not n[len(pre):len(pre) + 1] in '0123456789'

def banked(v, b):
    ex = [n for n in ram_names.get(v, []) if bank_name(n, b)]
    if ex: return min(ex, key=rank)
    for d in range(1, WINDOW[0]):
        ex = [n for n in ram_names.get(v - d, []) if bank_name(n, b)]
        if ex: return f'{min(ex, key=rank)} + 0x{d:02x}'
    return None

def pick_wbank(values):
    ds = [v for v in values if 0xd000 <= v < 0xe000]
    if not ds: return 1
    def score(b):
        sc = 0
        for v in ds:
            n = banked(v, b)
            if n: sc += 2 if '+' not in n else 1
        return sc
    return max([1, 3, 2, 4, 5, 6, 7], key=score)

def ram(v):
    if 0xcfc0 < v < 0xd000: return f'wTmpcfc0 + 0x{v - 0xcfc0:02x}'
    if 0xd000 <= v < 0xe000:
        n = banked(v, WBANK[0])
        if n: return n
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

genh = set(re.findall(r'\bs_\w+', open('src/game/seasons/gen.h').read()))
def hooked(n):
    return n in hand or f's_{n}' in genh or f's_{n}_hook' in genh

def fn_of(n):
    if n in hand: return f's_{n}_hook'
    return f's_{n}'

def tail_of(n):
    if not hooked(n): return f'HANDOFF(SYM({symref(n)})); /* no hook: interpreted */'
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
    if n and noncanonical(bank if v >= 0x4000 else 0, v) and base >= 0:
        d = v - base
        return (f'(b_ + {d})' if d >= 0 else f'(b_ - {-d})') + f' /* {n} */'
    if n: return f'SYM({symref(n)})'
    loc = locs.get((bank if v >= 0x4000 else 0, v))
    if not guess: return f'0x{v:04x}'
    if loc:
        tb = bank if v >= 0x4000 else 0
        parent = loc.split('@')[0]
        if parent in tops and tops[parent][0] == tb and tops[parent][1] <= v:
            return f'(SYM({symref(parent)}) + {v - tops[parent][1]}) /* @{loc.split("@", 1)[1]} */'
        return f'0x{v:04x} /* TODO {loc} */'
    other = [nm for (b, a), nm in name_at.items() if a == v and v >= 0x4000]
    scripts = [nm for nm in other if 'script' in nm.lower()]
    if len(other) == 1 or len(scripts) == 1: return f'SYM({symref((scripts or other)[0])})'
    if other: return f'0x{v:04x} /* TODO SYM of one of: {" ".join(sorted(other)[:6])} */'
    return f'0x{v:04x}'

C_KEYWORDS = {'auto', 'break', 'case', 'char', 'const', 'continue', 'default', 'do', 'double', 'else', 'enum',
              'extern', 'float', 'for', 'goto', 'if', 'int', 'long', 'register', 'return', 'short', 'signed',
              'sizeof', 'static', 'struct', 'switch', 'typedef', 'union', 'unsigned', 'void', 'volatile', 'while'}
helper_names = {}
def helper_name(owner, label, t):
    n = f'{PREFIX}_{label}'
    if helper_names.get(n, t) != t: n = f'{PREFIX}_{owner}_{label}'
    helper_names[n] = t
    return n

def owner_of(tb, t):
    cands = [a2 for a2 in top_addrs.get(tb, ()) if a2 <= t]
    return max(cands) if cands else None

def canonical_base(tb, a):
    cands = [a2 for a2 in top_addrs.get(tb, ()) if a2 <= a and not noncanonical(tb, a2)]
    return max(cands) if cands else None

def need_helper(pending, tb, t):
    o = owner_of(tb, t)
    if o is None: return None
    on = name_at[(tb, o)]
    l = locs.get((tb, t))
    label = (l.split('@', 1)[1].replace('@', '_') if l else (on if t == o else f'{on}_{t - o}'))
    if label in C_KEYWORDS: label = 'l_' + label
    key = (tb, t)
    if pending is not None and key in pending: return pending[key][2]
    fname = helper_name(on, label, key)
    if pending is not None: pending[key] = (on, (tb, o), fname)
    return fname

def draft(name, start=None, helper=None, pending=None, owner=None, hookname=None):
    bank, lo = owner if owner else tops[name]
    base_label = name
    if noncanonical(bank, lo):
        cb = canonical_base(bank, lo)
        base_label = name_at[(bank, cb)]
    base = tops[base_label][1]
    later = sorted(a for a in top_addrs.get(bank if lo >= 0x4000 else 0, set()) if a > lo)
    hi = later[0] if later else lo + 0x400
    entry = lo if start is None else start
    seen, work, tables = {}, [entry], {}
    labels = set()
    while work:
        a = work.pop()
        while lo <= a < hi and a not in seen:
            op = rd(bank, a); n = length(op); seen[a] = n
            nxt = a + n
            if op in (0x18, 0x20, 0x28, 0x30, 0x38):
                t = (nxt + ((rd(bank, a + 1) ^ 0x80) - 0x80)) & 0xffff
                if lo <= t < hi: labels.add(t); work.append(t)
                if op == 0x18: break
            elif op in (0xc3, 0xc2, 0xca, 0xd2, 0xda):
                t = rd(bank, a + 1) | rd(bank, a + 2) << 8
                if lo <= t < hi: labels.add(t); work.append(t)
                if op == 0xc3: break
            elif op in (0xc9, 0xd9, 0xe9): break
            elif op == 0xc7:
                ents, p = [], nxt
                first = None
                while p + 1 < hi and (first is None or p < first):
                    if ents and ((bank, p) in locs or (bank, p) in name_at): break
                    t = rd(bank, p) | rd(bank, p + 1) << 8
                    tb = bank if t >= 0x4000 else 0
                    if not (lo <= t < hi) and ((tb, t) not in name_at and (tb, t) not in locs): break
                    ents.append(t)
                    if lo <= t < hi:
                        first = t if first is None else min(first, t)
                        labels.add(t); work.append(t)
                    p += 2
                    if len(ents) > 64: break
                tables[a] = ents
                break
            a = nxt
    vals, svbk = [], None
    for a in sorted(seen):
        op = rd(bank, a)
        if op in (0x01, 0x11, 0x21, 0xea, 0xfa): vals.append(rd(bank, a + 1) | rd(bank, a + 2) << 8)
        if op == 0xe0 and rd(bank, a + 1) == 0x70 and rd(bank, a - 2) == 0x3e and rd(bank, a - 1) and svbk is None:
            svbk = rd(bank, a - 1)
    WINDOW[0] = 0x100
    WBANK[0] = svbk if svbk else pick_wbank(vals)
    if name in WB_OVERRIDE: WBANK[0], WINDOW[0] = WB_OVERRIDE[name], 0x800
    if helper:
        out = [f'// {locs.get((bank, entry), name + ("+" + str(entry - lo) if entry != lo else ""))}', f'static void {helper}(GB *gb) {{', f'  BASE({symref(base_label)});', '  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;']
    else:
        out = [f'void s_{hookname or name}_hook(GB *gb) {{', f'  BASE({symref(base_label)});', '  uint16_t sp0_ = gb->sp; (void)sp0_;']
    def lab(t):
        l = locs.get((bank, t))
        r = (l.split('@', 1)[1] if l else f'L_{t:04x}').replace('@', '_')
        return 'l_' + r if r in C_KEYWORDS else r
    rombank = [None]
    last_a = [None]
    KEEPS_A = {0x00, 0xe0, 0xea, 0x02, 0x12, 0x22, 0x32, 0x77, 0xc5, 0xd5, 0xe5, 0xf5, 0x47, 0x4f, 0x57, 0x5f, 0x67, 0x6f,
               0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x01, 0x11, 0x21, 0x03, 0x13, 0x23, 0x0b, 0x1b, 0x2b}
    def tbank(t):
        if t < 0x4000: return 0
        if bank == 0: return rombank[0]
        return bank
    def hname(tb, t):
        n = name_at.get((tb, t))
        if n is None: return None
        if not noncanonical(tb, t) and hooked(n): return n
        sfx = f'{n}_b{tb:02x}'
        return sfx if sfx in hand else None
    def is_c(tb, t):
        return hname(tb, t) is not None
    def addr_expr(tb, t):
        n = name_at[(tb, t)]
        if not noncanonical(tb, t): return f'SYM({symref(n)})'
        if tb == bank:
            d = t - base
            return f'(b_ + {d})' if d >= 0 else f'(b_ - {-d})'
        cb = canonical_base(tb, t)
        return f'(SYM({symref(name_at[(tb, cb)])}) + {t - cb})'
    def c_tail(tb, t):
        hn = hname(tb, t)
        if hn != name_at[(tb, t)]: return f'{fn_of(hn)}(gb); return;'
        return tail_of(hn)
    if entry != min(seen):
        labels.add(entry)
        out.append(f'  goto {lab(entry)};')
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
        def jump(t, cond=None):
            tb = tbank(t)
            if lo <= t < hi and t in labels:
                go = f'goto {lab(t)};'
            elif tb is None:
                go = f'/* TODO jump to {hex(t)} in an unknown bank */ HANDOFF(0x{t:04x});'
            elif is_c(tb, t):
                go = c_tail(tb, t)
            else:
                f = need_helper(pending, tb, t)
                go = f'{f}(gb); return;' if f else f'/* TODO jump to {hex(t)} */ HANDOFF(0x{t:04x});'
            if cond is None: return f'  {cy}\n  {go}'
            return f'  if ({cond}) {{ {cyt} {go} }}\n  {cy}'
        if op == 0x00: s = f'  {cy}'
        elif hi3 == 1 and op != 0x76:
            s = f'  {cy} ' + r8set(mid, r8get(lo3))
        elif hi3 == 0 and lo3 == 6: s = f'  {cy} ' + r8set(mid, field(n1) if OBJ and mid in (3, 5) and n1 >= 0x40 else f'0x{n1:02x}')
        elif hi3 == 0 and lo3 == 1 and mid % 2 == 0:
            rp = RP[mid // 2]
            xb = bank
            if rp == 'HL' and rd(bank, e) == 0x1e and rd(bank, e + 2) in (0xcd, 0xc3) and \
                    name_at.get((0, rd(bank, e + 3) | rd(bank, e + 4) << 8)) == 'interBankCall':
                xb = rd(bank, e + 1)
            v = imm16_expr(xb, n16, base if xb == bank else -1, lo if xb == bank else -1, hi if xb == bank else -1, guess=(rp == 'HL' and xb == bank))
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
        elif op == 0xea:
            s = f'  {cy} mem_wr(gb, {imm16_expr(bank, n16, base, lo, hi)}, A);'
            if 0x2000 <= n16 < 0x4000: rombank[0] = last_a[0]
        elif op == 0xfa: s = f'  {cy} A = mem_rd(gb, {imm16_expr(bank, n16, base, lo, hi)});'
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
            t = n16; tb = tbank(t)
            if tb is not None and is_c(tb, t):
                hn = hname(tb, t)
                c1 = f'CALL_C(b_+{x}, {fn_of(hn)}, {addr_expr(tb, t)}, b_+{y});'
                c2 = f'CALL_C_CC(b_+{x}, {fn_of(hn)}, {addr_expr(tb, t)}, b_+{y});'
            else:
                f = need_helper(pending, tb, t) if tb is not None else None
                if f:
                    c1 = f'CALL_L(b_+{x}, {f}, b_+{y});'
                    c2 = f'CALL_L_CC(b_+{x}, {f}, b_+{y});'
                else:
                    c1 = f'CALL_ROM(b_+{x}, 0x{t:04x}); /* TODO unknown bank */'
                    c2 = f'CALL_ROM_CC(b_+{x}, 0x{t:04x}); /* TODO unknown bank */'
            s = f'  {c1}' if op == 0xcd else f'  if ({CC[(op >> 3) & 3]}) {c2}\n  else {cy}'
        elif op == 0xc9: s = f'  RET(b_+{x}); return;'
        elif op in (0xc0, 0xc8, 0xd0, 0xd8): s = f'  if ({CC[(op >> 3) & 3]}) {{ RET_TAKEN(b_+{x}); return; }}\n  CYC(b_+{x}, b_+{y});'
        elif op == 0xe9: s = f'  {cy}\n  HANDOFF(HL);'
        elif op == 0xd7: s = f'  {cy} {PREFIX}_add_a_to_hl(gb, b_+{y});'
        elif op == 0xdf: s = f'  {cy} {PREFIX}_add_double_index(gb, b_+{y});'
        elif op == 0xc7:
            lines = [f'  {cy} push_effect(gb, b_+{y});', f'  do {{ uint16_t jt_ = ({PREFIX}_jump_table(gb));']
            done_t = set()
            for t in tables.get(a, []):
                if t in done_t: continue
                done_t.add(t)
                tb = bank if t >= 0x4000 else 0
                if lo <= t < hi: lines.append(f'    if (jt_ == b_+{t - base}) goto {lab(t)};')
                elif is_c(tb, t):
                    f = fn_of(hname(tb, t)); ae = addr_expr(tb, t)
                    lines.append(f'    if (jt_ == {ae} && hook_is(gb, {ae}, {f})) {{ {f}(gb); return; }}')
                else:
                    f = need_helper(pending, tb, t)
                    d = t - base
                    lines.append(f'    if (jt_ == {"(b_ + " + str(d) + ")" if d >= 0 else "(b_ - " + str(-d) + ")"}) {{ {f}(gb); return; }}')
            lines.append('    HANDOFF(HL);')
            lines.append('  } while (0);')
            s = '\n'.join(lines)
            prev_end = None
        else: s = f'  {cy} /* TODO opcode ${op:02x} */'
        out.append(s)
        if op == 0x3e: last_a[0] = n1
        elif op not in KEEPS_A: last_a[0] = None
        if e == hi and op not in (0x18, 0xc3, 0xc9, 0xd9, 0xe9, 0xc7) and (bank, hi) in name_at:
            if is_c(bank, hi):
                out.append(f'  {fn_of(hname(bank, hi))}(gb); return; // falls through')
            else:
                out.append(f'  {need_helper(pending, bank, hi)}(gb); return; // falls through')
    out.append('}')
    return '\n'.join(out)

pending, done = {}, set()
for n in args:
    helpers = []
    if ':' in n:
        n, bk = n.split(':'); bk = int(bk, 16)
        addr = next(a for (b, a), nm in name_at.items() if b == bk and nm == n)
        main = draft(n, pending=pending, owner=(bk, addr), hookname=f'{n}_b{bk:02x}')
    else:
        main = draft(n, pending=pending)
    while set(pending) - done:
        key = min(set(pending) - done); done.add(key)
        on, okey, fname = pending[key]
        helpers.append(draft(on, start=key[1], helper=fname, pending=pending, owner=okey))
    for h in reversed(helpers): print(h); print()
    print(main); print()
missing = sorted(x for x in needed if not re.search(rf'^{re.escape(x)} ', open('src/hooks/syms_used.txt').read(), re.M))
if missing: print('// syms_used.txt needs: ' + ' '.join(missing))
