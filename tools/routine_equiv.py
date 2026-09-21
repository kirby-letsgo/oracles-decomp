#!/usr/bin/env python3
"""Compare every rewritten routine between the Ages and Seasons ROMs.

usage: tools/routine_equiv.py AGES.gbc ages.sym SEASONS.gbc seasons.sym [--tsv OUT] [--by-file]

A routine is IDENTICAL when its instruction stream matches after normalizing operands: ROM
targets become label+offset, jumps inside the routine become routine-relative, RAM/IO addresses
become symbol names. SAME_SHAPE means the opcodes and lengths match but some symbolic operand
differs (a different callee or variable). DIFFERENT means the instruction stream itself differs.
AGES_ONLY means the label does not exist in seasons.sym.
"""
import bisect, glob, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

R8N = ['b', 'c', 'd', 'e', 'h', 'l', '(hl)', 'a']
R16 = ['bc', 'de', 'hl', 'sp']
R16P = ['bc', 'de', 'hl', 'af']
CC = ['nz', 'z', 'nc', 'c']
ALU = ['add', 'adc', 'sub', 'sbc', 'and', 'xor', 'or', 'cp']
ROT = ['rlc', 'rrc', 'rl', 'rr', 'sla', 'sra', 'swap', 'srl']
ILLEGAL = {0xd3, 0xdb, 0xdd, 0xe3, 0xe4, 0xeb, 0xec, 0xed, 0xf4, 0xfc, 0xfd}


class Game:
    def __init__(self, rom_path, sym_path):
        self.rom = open(rom_path, 'rb').read()
        self.labels, self.by_addr, self.instances = {}, {}, {}
        self.ram = {}
        for line in open(sym_path):
            m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
            if not m: continue
            b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
            self.labels.setdefault(n, (b, a))
            if a >= 0x8000:
                self.ram.setdefault(a, n)
                continue
            self.by_addr.setdefault((b, a), n)
            if '@' not in n:
                self.instances.setdefault(n, [])
                if (b, a) not in self.instances[n]: self.instances[n].append((b, a))
        # extra labels: vectors and code copied to RAM (read through its ROM source)
        self.relocs = []
        self.extra_names = set()        # never end a body: most are resume points inside a routine
        from symfiles import extra_labels
        for b, a, n, src, ext in extra_labels(sym_path):
            self.labels.setdefault(n, (b, a))
            if '__' in n: self.by_addr.setdefault((b, a), n); self.extra_names.add(n)    # a C spelling of an @local or a resume point
            else: self.by_addr[(b, a)] = n            # a routine of its own (vectors, RAM code, an alias for a plain Ages label)
            if '@' not in n and '__' not in n:      # `parent__local` names are C spellings of @locals
                self.instances.setdefault(n, [])
                if (b, a) not in self.instances[n]: self.instances[n].append((b, a))
            if src: self.relocs.append((a, a + 0x80, src))
        self.sorted_by_bank = {}
        for (b, a) in self.by_addr:
            self.sorted_by_bank.setdefault(b, []).append(a)
        for b in self.sorted_by_bank: self.sorted_by_bank[b].sort()
        self.ram_sorted = sorted(self.ram)
        self._bodies = {}

    def rd(self, bank, addr, off=0):
        a = addr + off
        if a >= 0x8000:
            for lo, hi, (sb, sa) in self.relocs:
                if lo <= a < hi: return self.rd(sb, sa + (a - lo))
            return 0
        return self.rom[(bank * 0x4000 + (a - 0x4000)) if a >= 0x4000 else a]

    def rom_sym(self, bank, t, hint=None):
        """label+offset for a ROM address, resolved in the bank the code would land in (for bank 0
        code, the bank the routine last switched to when known, else the one bank with a label)."""
        tb = 0 if t < 0x4000 else bank
        if t >= 0x4000 and hint is not None and hint != tb and (hint, t) in self.by_addr: return self.by_addr[(hint, t)]
        if tb == 0 and t >= 0x4000:
            hits = [b for b in self.sorted_by_bank if b != 0 and (b, t) in self.by_addr]
            return self.by_addr[(hits[0], t)] if len(hits) == 1 else f'${t:04x}'
        addrs = self.sorted_by_bank.get(tb, [])
        i = bisect.bisect_right(addrs, t) - 1
        if i < 0: return f'${t:04x}'
        base = addrs[i]
        n = self.by_addr[(tb, base)]
        return n if base == t else f'{n}+{t - base}'

    def peer_sym(self, sym, peer):
        """An unresolved `$xxxx` (a bank-0 reference several banks could satisfy) counts as the
        other game's label when this game has that label at that address in some bank."""
        m = re.match(r'^\$([0-9a-f]{4})$', sym)
        if not m or not re.match(r'^[A-Za-z_]\w*$', peer): return sym
        t = int(m.group(1), 16)
        return peer if any((b, t) in self.by_addr and self.by_addr[(b, t)] == peer for b in self.sorted_by_bank if b != 0) else sym

    def ram_sym(self, t):
        i = bisect.bisect_right(self.ram_sorted, t) - 1
        if i < 0: return f'${t:04x}'
        base = self.ram_sorted[i]
        if t - base > 0x100 or (t >= 0xff00) != (base >= 0xff00): return f'${t:04x}'
        n = self.ram[base]
        return n if base == t else f'{n}+{t - base}'

    def decode(self, bank, addr):
        """Return (length, kind, template, operands) where operands are ('rom', t) / ('ram', t) / ('imm', v)."""
        op = self.rd(bank, addr)
        if op in ILLEGAL: return 1, 'bad', f'illegal ${op:02x}', ()
        n1 = self.rd(bank, addr, 1)
        n16 = n1 | (self.rd(bank, addr, 2) << 8)
        rel = (addr + 2 + ((n1 ^ 0x80) - 0x80)) & 0xffff
        x, y, z, p, q = op >> 6, (op >> 3) & 7, op & 7, (op >> 4) & 3, (op >> 3) & 1
        if op == 0xcb:
            o = n1; cy, cz = (o >> 3) & 7, o & 7
            if o < 0x40: return 2, 'op', f'{ROT[cy]} {R8N[cz]}', ()
            return 2, 'op', f'{["bit", "res", "set"][(o >> 6) - 1]} {cy},{R8N[cz]}', ()
        if x == 0:
            if z == 0:
                if y == 0: return 1, 'op', 'nop', ()
                if y == 1: return 3, 'op', 'ld (%s),sp', (('ram', n16),)
                if y == 2: return 2, 'bad', 'stop', ()
                if y == 3: return 2, 'jp', 'jr %s', (('rom', rel),)
                return 2, 'jpcc', f'jr {CC[y-4]},%s', (('rom', rel),)
            if z == 1:
                if q == 0: return 3, 'op', f'ld {R16[p]},%s', (('any', n16),)
                return 1, 'op', f'add hl,{R16[p]}', ()
            if z == 2:
                t = ['(bc)', '(de)', '(hl+)', '(hl-)'][p]
                return 1, 'op', f'ld {t},a' if q == 0 else f'ld a,{t}', ()
            if z == 3: return 1, 'op', f'{"inc" if q == 0 else "dec"} {R16[p]}', ()
            if z == 4: return 1, 'op', f'inc {R8N[y]}', ()
            if z == 5: return 1, 'op', f'dec {R8N[y]}', ()
            if z == 6: return 2, 'op', f'ld {R8N[y]},%s', (('imm', n1),)
            return 1, 'op', ['rlca', 'rrca', 'rla', 'rra', 'daa', 'cpl', 'scf', 'ccf'][y], ()
        if x == 1:
            if op == 0x76: return 1, 'halt', 'halt', ()
            return 1, 'op', f'ld {R8N[y]},{R8N[z]}', ()
        if x == 2: return 1, 'op', f'{ALU[y]} {R8N[z]}', ()
        if z == 0:
            if y < 4: return 1, 'retcc', f'ret {CC[y]}', ()
            if y == 4: return 2, 'op', 'ldh (%s),a', (('ram', 0xff00 | n1),)
            if y == 5: return 2, 'op', 'add sp,%s', (('imm', n1),)
            if y == 6: return 2, 'op', 'ldh a,(%s)', (('ram', 0xff00 | n1),)
            return 2, 'op', 'ld hl,sp+%s', (('imm', n1),)
        if z == 1:
            if q == 0: return 1, 'op', f'pop {R16P[p]}', ()
            return [(1, 'ret', 'ret', ()), (1, 'ret', 'reti', ()), (1, 'jphl', 'jp hl', ()), (1, 'op', 'ld sp,hl', ())][p]
        if z == 2:
            if y < 4: return 3, 'jpcc', f'jp {CC[y]},%s', (('rom', n16),)
            return [(1, 'op', 'ld ($ff00+c),a', ()), (3, 'op', 'ld (%s),a', (('ram', n16),)),
                    (1, 'op', 'ld a,($ff00+c)', ()), (3, 'op', 'ld a,(%s)', (('ram', n16),))][y-4]
        if z == 3:
            if y == 0: return 3, 'jp', 'jp %s', (('rom', n16),)
            if y == 6: return 1, 'op', 'di', ()
            if y == 7: return 1, 'op', 'ei', ()
            return 1, 'bad', f'illegal ${op:02x}', ()
        if z == 4: return 3, 'callcc', f'call {CC[y]},%s', (('rom', n16),)
        if z == 5:
            if q == 0: return 1, 'op', f'push {R16P[p]}', ()
            return 3, 'call', 'call %s', (('rom', n16),)
        if z == 6: return 2, 'op', f'{ALU[y]} %s', (('imm', n1),)
        return 1, 'rst', f'rst ${y*8:02x}', ()

    def body(self, bank, start):
        """Every instruction reachable from start without crossing another label, as [(addr, len, kind, template, operands)]."""
        key = (bank, start)
        if key in self._bodies: return self._bodies[key]
        seen, work = {}, [start]
        while work:
            a = work.pop()
            if a in seen or (a >= 0x8000 and not any(lo <= a < hi for lo, hi, _ in self.relocs)): continue
            if a != start and (bank, a) in self.by_addr and '@' not in self.by_addr[(bank, a)] and self.by_addr[(bank, a)] not in self.extra_names: continue
            ln, kind, tmpl, ops = self.decode(bank, a)
            succ = []
            if kind == 'rst' and tmpl == 'rst $00':
                table, pos = [], a + ln
                while pos < 0x8000:
                    w = self.rd(bank, pos) | (self.rd(bank, pos + 1) << 8)
                    if not (0x0150 <= w < 0x8000): break
                    if table and (bank, pos) in self.by_addr: break
                    if any(pos <= t < pos + 2 for t in table): break
                    table.append(w); pos += 2
                if not table: table = [self.rd(bank, a + ln) | (self.rd(bank, a + ln + 1) << 8)]
                kind, tmpl, ops = 'jumptable', 'jumptable ' + ','.join(['%s'] * len(table)), tuple(('rom', t) for t in table)
                succ = list(table)
            elif kind in ('jp', 'jpcc'):
                succ.append(ops[0][1])
                if kind != 'jp': succ.append(a + ln)
            elif kind in ('ret', 'jphl', 'bad'):
                pass
            else:
                succ.append(a + ln)
            seen[a] = (a, ln, kind, tmpl, ops)
            work.extend(t for t in succ if (t < 0x4000) == (start < 0x4000) or (start >= 0x8000 and t >= 0x8000))
        self._bodies[key] = [seen[a] for a in sorted(seen)]
        return self._bodies[key]

    def normalized(self, bank, start):
        """[(offset, template, symbolic operands)] plus the shape [(offset, template)]."""
        insns = self.body(bank, start)
        addrs = set(a for a, *_ in insns)
        lo = min(addrs)
        out, shape = [], []
        hint, last_imm = None, None
        ibc = self.labels.get('interBankCall', (0, -1))[1]
        for i, (a, ln, kind, tmpl, ops) in enumerate(insns):
            if tmpl == 'ld a,%s' and ops and ops[0][0] == 'imm': last_imm = ops[0][1]
            elif tmpl in ('ld (%s),a', 'ldh (%s),a') and ops and ops[0][1] in (0x2222, 0xff97): hint = last_imm
            h = hint if start < 0x4000 else None
            for j in range(i, min(i + 4, len(insns))):     # ld hl,x / ld e,bank / call interBankCall, in any order
                xj = insns[j]
                if xj[3] == 'ld e,%s' and xj[4] and xj[4][0][0] == 'imm' and any(x[2] == 'call' and x[4] and x[4][0][1] == ibc for x in insns[j + 1:j + 4]): h = xj[4][0][1]; break
            syms = []
            for k, v in ops:
                if k == 'imm': syms.append(f'${v:02x}')
                elif v >= 0x8000: syms.append(self.ram_sym(v))
                elif k == 'ram' and 0x0000 <= v < 0x8000 and not (0x0150 <= v < 0x4000 and tmpl.startswith('ld a,(')): syms.append(f'${v:04x}')   # MBC register writes
                elif v in addrs or (k != 'any' and lo <= v < lo + 0x200 and any(x <= v < x + l for x, l, *_ in insns)):
                    syms.append(f'@{v - start:+d}')
                else: syms.append(self.rom_sym(bank, v, h))
            out.append((a - start, tmpl % tuple(syms) if syms else tmpl))
            shape.append((a - start, tmpl))
        return out, shape


def mask_jumptables(norm):
    """The body with jump-table entries blanked: a table that only differs in its entries is
    dispatched by the C as an if-chain over the targets with an interpreter fallback (JT_ONLY)."""
    return [(o, 'jumptable' if t.startswith('jumptable') else t) for o, t in norm]


def reconcile(an, sn, ages, seasons, unnamed_pairs=None):
    """Resolve `$xxxx` operands on either side through the other side's label (Game.peer_sym), and
    unnamed labels (`_label_XX_N`) through the pairs symfiles inferred from references."""
    unnamed_pairs = unnamed_pairs or {}
    out_a, out_s = [], []
    for (oa, ta), (os_, ts) in zip(an, sn):
        if ta != ts:
            wa, ws = ta.split(' '), ts.split(' ')
            if len(wa) == len(ws):
                for i in range(len(wa)):
                    if wa[i] == ws[i]: continue
                    xa, xs = wa[i].split(','), ws[i].split(',')
                    if len(xa) == len(xs):
                        xa = [ages.peer_sym(x, y) for x, y in zip(xa, xs)]
                        xs = [seasons.peer_sym(y, x) for x, y in zip(xa, xs)]
                        xs = [x if unnamed_pairs.get(x.split('+')[0]) == y.split('+')[0] and x.split('+')[1:] == y.split('+')[1:] else y for x, y in zip(xa, xs)]
                        wa[i], ws[i] = ','.join(xa), ','.join(xs)
                ta, ts = ' '.join(wa), ' '.join(ws)
        out_a.append((oa, ta)); out_s.append((os_, ts))
    return out_a, out_s


def main():
    args = sys.argv[1:]
    tsv = None
    by_file = False
    if '--tsv' in args:
        i = args.index('--tsv'); tsv = args[i + 1]; del args[i:i + 2]
    if '--by-file' in args:
        by_file = True; args.remove('--by-file')
    ages, seasons = Game(args[0], args[1]), Game(args[2], args[3])
    from symfiles import pair_instances, UNNAMED
    pairs = pair_instances(args[0], args[1], args[2], args[3])
    unnamed_pairs = {}      # ages unnamed label -> seasons unnamed label, for reconcile()
    for (n, b, a), (sb, sa) in pairs.items():
        if UNNAMED.match(n) and (sb, sa) in seasons.by_addr: unnamed_pairs[n] = seasons.by_addr[(sb, sa)]
    if '--diff' in args:
        n = args[args.index('--diff') + 1].replace('__', '@')
        for g, label in ((ages, 'ages'), (seasons, 'seasons')):
            for (b, a) in g.instances.get(n) or [g.labels[n]]:
                print(f'== {label} {b:02x}:{a:04x} {n}')
                for off, s in g.normalized(b, a)[0]: print(f'  {off:+5d}  {s}')
        return
    names = []
    for l in open('src/hooks/rewritten.txt'):
        n = l.split('#')[0].strip()
        if n: names.append(n)
    results = {}
    for n in names:
        bare, insts = n, ages.instances.get(n)
        m = re.search(r'^(.*)_b([0-9a-f]{2})$', n)
        if insts is None and m and m.group(1) in ages.instances:
            bare = m.group(1)
            insts = [(b, a) for b, a in ages.instances[bare] if b == int(m.group(2), 16)] or ages.instances[bare]
        if insts is None and '__' in n:
            bare = n.replace('__', '@')
            if bare in ages.labels: insts = [ages.labels[bare]]
        if not insts: continue
        s_insts = seasons.instances.get(bare) or ([seasons.labels[bare]] if bare in seasons.labels else None)
        if re.match(r'^(_label_[0-9a-f]{2}_\d+|label_[0-9a-f]{2}_\d+)', bare.split('@')[0]):
            s_insts = [pairs[(bare,) + a] for a in insts if (bare,) + a in pairs] or None
        elif s_insts and len(s_insts) > 1:      # copies in several banks: the paired copy only
            paired = [pairs[(bare,) + a] for a in insts if (bare,) + a in pairs]
            if paired: s_insts = paired
        if not s_insts:
            results[n] = ('AGES_ONLY', '', 0)
            continue
        verdicts = []
        for (b, a) in insts:
            an, ash = ages.normalized(b, a)
            best = None
            for (sb, sa) in s_insts:
                sn, ssh = seasons.normalized(sb, sa)
                if an != sn and ash == ssh: an, sn = reconcile(an, sn, ages, seasons, unnamed_pairs)
                if an == sn: best = ('IDENTICAL', f'{sb:02x}:{sa:04x}'); break
                if ash == ssh and mask_jumptables(an) == mask_jumptables(sn): best = ('JT_ONLY', f'{sb:02x}:{sa:04x}'); break
                if ash == ssh:
                    diffs = sum(1 for x, y in zip(an, sn) if x != y)
                    if best is None or best[0] != 'SAME_SHAPE' or diffs < best[2]: best = ('SAME_SHAPE', f'{sb:02x}:{sa:04x}', diffs)
                elif best is None: best = ('DIFFERENT', f'{sb:02x}:{sa:04x}', abs(len(an) - len(sn)) or 1)
            verdicts.append(best)
        rank = {'IDENTICAL': 0, 'JT_ONLY': 1, 'SAME_SHAPE': 2, 'DIFFERENT': 3}
        worst = max(verdicts, key=lambda v: rank[v[0]])
        results[n] = (worst[0], worst[1], worst[2] if len(worst) > 2 else 0)

    counts = {}
    for v, *_ in results.values(): counts[v] = counts.get(v, 0) + 1
    total = len(results)
    print(f'{total} rewritten routines compared')
    for k in ('IDENTICAL', 'JT_ONLY', 'SAME_SHAPE', 'DIFFERENT', 'AGES_ONLY'):
        c = counts.get(k, 0)
        print(f'  {k:11s} {c:5d}  ({100.0 * c / total:5.1f}%)')
    if tsv:
        with open(tsv, 'w') as f:
            for n, (v, where, d) in sorted(results.items(), key=lambda kv: (kv[1][0], kv[0])):
                f.write(f'{n}\t{v}\t{where}\t{d}\n')
        print('wrote', tsv)
    if by_file:
        hook_re = re.compile(r'\bvoid\s+([A-Za-z_][A-Za-z0-9_]*)_hook\(GB \*gb\)\s*\{')
        per_file = {}
        for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
            rel = os.path.relpath(path, 'src/game')
            if rel.startswith('gen_'): continue
            defs = hook_re.findall(open(path, errors='replace').read())
            vs = [results[h][0] for h in defs if h in results]
            if vs: per_file[rel] = vs
        print()
        print('per file (routines: identical / same-shape / different / ages-only):')
        agg = {}
        for rel, vs in sorted(per_file.items()):
            c = {k: vs.count(k) for k in ('IDENTICAL', 'SAME_SHAPE', 'DIFFERENT', 'AGES_ONLY')}
            top = os.path.dirname(rel) or '.'
            agg.setdefault(top, [0, 0, 0, 0, 0])
            for i, k in enumerate(('IDENTICAL', 'SAME_SHAPE', 'DIFFERENT', 'AGES_ONLY')): agg[top][i] += c[k]
            agg[top][4] += 1
            if c['IDENTICAL'] == len(vs): tag = 'ALL-IDENTICAL'
            elif c['AGES_ONLY'] == len(vs): tag = 'ALL-AGES-ONLY'
            else: tag = 'MIXED'
            print(f'  {rel:60s} {c["IDENTICAL"]:4d} {c["SAME_SHAPE"]:4d} {c["DIFFERENT"]:4d} {c["AGES_ONLY"]:4d}  {tag}')
        print()
        print('per directory (files, identical / same-shape / different / ages-only):')
        for top, (i, s, d, o, nf) in sorted(agg.items()):
            print(f'  {top:32s} {nf:4d} files  {i:5d} {s:5d} {d:5d} {o:5d}')


if __name__ == '__main__':
    main()
