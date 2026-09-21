#!/usr/bin/env python3
"""Make SAME_SHAPE routines run under Seasons: per-game constants in the shared C.

usage: tools/gameconst.py AGES.gbc ages.sym SEASONS.gbc seasons.sym [--apply]

For every routine that tools/routine_equiv.py rates SAME_SHAPE (same instructions, some operand
differs), each differing operand is located in the C (the CYC/CALL_C line burning that
instruction, found through the function's BASE label and the instruction's offset) and the C
expression is evaluated for both games from src/game/syms.c and ram.h. If it already yields the
right value for Seasons (a SYM() the name-based comparison could not see through) nothing
changes; if not, the Ages expression is wrapped as GV(ages_expr, 0xSEASONS). A routine whose
every difference is resolved this way is written to src/hooks/seasons_ok.txt, which
tools/seasons_hooks.py treats as IDENTICAL. Differences that are not constants (a different
callee, a different variable) are reported and the routine is left alone.
"""
import glob, os, re, sys
from collections import defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from routine_equiv import Game
from symfiles import ram_map

FUNC_START = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?\w+\([^)]*\)\s*\{')


def load_syms():
    h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
    ids = re.findall(r'^\s+S_(\w+),', h, re.M)
    a = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_ages\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
    s = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_seasons\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
    return {i: (int(x, 16), int(y, 16)) for i, x, y in zip(ids, a, s)}


class Tool:
    def __init__(self, ages, seasons):
        self.A, self.S = ages, seasons
        self.syms = load_syms()
        self.ram_a = {k: v[1] for k, v in ram_map('ref/oracles-disasm/ages.sym').items()}
        self.ram_s = {k: v[1] for k, v in ram_map('ref/oracles-disasm/seasons.sym').items()}
        self.ram_s_by_addr = {}
        for k, v in sorted(self.ram_s.items(), key=lambda kv: (len(kv[0]), kv[0])):
            self.ram_s_by_addr.setdefault(v, k)
        self.macros = {}
        for m in re.finditer(r'^#define (\w+) ((?:0x[0-9a-f]+|SYM\(\w+\)|\d+)(?: [+-] (?:0x[0-9a-f]+|\d+))?)\s*$', open('src/game/game.h').read(), re.M):
            self.macros[m.group(1)] = m.group(2)
        self.files = {}
        self.file_macros = {}
        for f in sorted(glob.glob('src/game/**/*.c', recursive=True)):
            if '/gen_' in f or f.endswith('syms.c'): continue
            text = open(f, errors='replace').read()
            self.files[f] = text.split('\n')
            self.file_macros[f] = {m.group(1): m.group(2) for m in re.finditer(r'^#define (\w+) ((?:0x[0-9a-f]+|SYM\(\w+\)|\d+)(?: [+-] (?:0x[0-9a-f]+|\d+))?)\s*$', text, re.M)}
        # BASE label -> list of (file, start_line, end_line)
        self.funcs = defaultdict(list)
        for f, lines in self.files.items():
            start = None
            for i, l in enumerate(lines):
                if FUNC_START.match(l): start = i
                m = re.search(r'\bBASE\((\w+)\)', l)
                if m and start is not None:
                    j = i
                    while j < len(lines) and lines[j] != '}': j += 1
                    self.funcs[m.group(1)].append((f, start, j))
        self.changed = defaultdict(int)
        self.report = []
        self.cur_file = None
        self.literal_fixes = 0
        self.auditing = False
        self.ofs_routines, self.ofs_anchors = set(), {}
        self.cur_ofs, self.cur_ofs_end = {}, {}
        if os.path.exists('src/hooks/ofs_routines.txt'):
            for l in open('src/hooks/ofs_routines.txt'):
                p = l.split('#')[0].split()
                if not p: continue
                self.ofs_routines.add(p[0])
                self.ofs_anchors[p[0]] = [tuple(int(x) for x in t.split('=')) for t in p[1:]]

    def eval_expr(self, expr, game, base=None):
        """Evaluate a C address/constant expression for a game (0 ages, 1 seasons); None if unknown."""
        e = expr.strip()
        macros = dict(self.macros); macros.update(self.file_macros.get(self.cur_file, {}))
        for _ in range(3):
            e = re.sub(r'(?<!SYM\()\b([A-Za-z_]\w*)\b', lambda m: f'({macros[m.group(1)]})' if m.group(1) in macros else m.group(0), e)
        if base is not None: e = re.sub(r'\bb_\b', str(base), e)
        e = re.sub(r'\bS\((\d+)\)', r'\1', e)
        e = re.sub(r'\bOE\((\d+)\)', lambda m: str(self.cur_ofs_end.get(int(m.group(1)), -0x10000) if game else int(m.group(1))), e)
        e = re.sub(r'\bO\((\d+)\)', lambda m: str(self.cur_ofs.get(int(m.group(1)), -0x10000) if game else int(m.group(1))), e)
        if '-65536' in e: return None
        while True:
            m = re.search(r'\bGV[WH]?\(', e)
            if not m: break
            depth, j, args, start = 1, m.end(), [], m.end()
            while j < len(e) and depth:
                if e[j] == '(': depth += 1
                elif e[j] == ')':
                    depth -= 1
                    if depth == 0: args.append(e[start:j]); break
                elif e[j] == ',' and depth == 1: args.append(e[start:j]); start = j + 1
                j += 1
            if len(args) != 2: return None
            e = e[:m.start()] + '(' + args[1 if game else 0].strip() + ')' + e[j + 1:]
        e = re.sub(r'\bSYM\((\w+)\)', lambda m: str(self.syms[m.group(1)][game] & 0xffff) if m.group(1) in self.syms else 'None', e)
        ram = self.ram_s if game else self.ram_a
        e = re.sub(r'\b([wh][A-Za-z0-9_]+)\b', lambda m: str(ram[m.group(1)]) if m.group(1) in ram else m.group(0), e)
        e = re.sub(r'\bINTERACTION_BASE\b', '0x40', e); e = re.sub(r'\bENEMY_BASE\b', '0x80', e)
        if re.search(r'[A-Za-z_]', re.sub(r'0x[0-9a-f]+|None', '', e)): return None
        if 'None' in e: return None
        try: return eval(e) & 0xffff
        except Exception: return None

    def sid_for(self, name, bank):
        base = name.split('@')[0]
        sid = name.replace('@', '__')
        if len(self.A.instances.get(base, [])) > 1: sid += f'_b{bank:02x}'
        return sid

    def process(self, name, ab, aa, sb, sa, apply):
        parent = name.split('@')[0]
        pa = [x for x in self.A.instances.get(parent, []) if x[0] == ab] or [self.A.labels.get(parent)]
        pbase = pa[0][1] if pa[0] else aa
        base_sid = self.sid_for(parent, ab)
        self.base_sid = base_sid
        an = self.A.body(ab, aa); sn = self.S.body(sb, sa)
        if name in self.ofs_routines:       # per-game offsets: audit the aligned instructions only (call-only @locals too)
            from ofsmap import align_all
            self.cur_ofs, self.cur_ofs_end, segments = align_all(self.A, self.S, name, (ab, aa), (sb, sa), self.ofs_anchors.get(name, ()))
            pairs_ = [(ba_[x], bs_[y]) for ba_, bs_, al_, da, ds, *_ in segments for x, y in al_]
        elif len(an) != len(sn): self.report.append(f'{name}: shape differs'); return False
        else:
            pairs_ = list(zip(an, sn)); self.cur_ofs, self.cur_ofs_end = {}, {}
            if parent != name and parent in self.ofs_routines:      # an entry point of a mapped routine: its C uses O()
                from ofsmap import align_all
                pa = [x for x in self.A.instances.get(parent, []) if x[0] == ab] or [self.A.labels[parent]]
                ps = [x for x in self.S.instances.get(parent, []) if x[0] == sb] or [self.S.labels[parent]]
                self.cur_ofs, self.cur_ofs_end, _ = align_all(self.A, self.S, parent, pa[0], ps[0], self.ofs_anchors.get(parent, ()))
        funcs = self.funcs.get(base_sid)
        if not funcs: self.report.append(f'{name}: no C function with BASE({base_sid})'); return False
        ok = True
        for (a1, l1, k1, t1, o1), (a2, l2, k2, t2, o2) in pairs_:
            if k1 == 'jumptable': continue
            if t1 != t2 or len(o1) != len(o2): self.report.append(f'{name}: template differs at +{a1 - aa}'); ok = False; continue
            off = a1 - pbase
            for (k, v1), (_, v2) in zip(o1, o2):
                if self.cur_ofs and k in ('rom', 'any') and self.cur_ofs.get(v1 - pbase) == v2 - (sa - (aa - pbase)): continue   # in-routine target, mapped
                ra, rs = self.A.rom_sym(ab, v1), self.S.rom_sym(sb, v2)
                ra, rs = self.A.peer_sym(ra, rs), self.S.peer_sym(rs, ra)
                if k == 'rom' and ra == rs and not ra.startswith('$'): continue
                if k == 'ram' and self.A.ram_sym(v1) == self.S.ram_sym(v2) and not self.A.ram_sym(v1).startswith('$'): continue
                if k == 'imm' and v1 == v2: continue
                if k == 'ram' and v1 == v2 and (v1 >= 0xff00 or v1 < 0x8000): continue      # IO/HRAM or MBC register, same in both
                if not self.fix_site(name, funcs, off, k, v1, v2, t1, apply, quiet=self.auditing): ok = False
        return ok

    def fix_site(self, name, funcs, off, kind, v1, v2, tmpl, apply, quiet=False, helpers=False):
        for f, s, e in funcs:
            self.cur_file = f
            lines = self.files[f]
            def joined(i):
                code = lines[i].split('//')[0]
                j = i
                while code.count('(') > code.count(')') and j + 1 <= e:
                    j += 1; code += ' ' + lines[j].split('//')[0].strip()
                return code
            anchor = [i for i in range(s, e + 1) if re.search(rf'\b(CYCT?|CALL_C|CALL_C_CC|CALL_ROM|CALL_ROM_CC)\(b_\+(?:OE?\()?{off}\b', lines[i].split('//')[0])]
            if not anchor:
                for i in range(s, e + 1):
                    for m in re.finditer(r'\bCYCT?\(b_\+(?:OE?\()?(\d+)\)?, b_\+(?:OE?\()?(\d+)\)?\)', lines[i].split('//')[0]):
                        if int(m.group(1)) < off < int(m.group(2)): anchor.append(i)
            if not anchor and helpers:
                # a helper that burns a run of instructions from its address argument (bank_push, obj helpers)
                for i in range(s, e + 1):
                    for m in re.finditer(r'\b\w+\(gb, b_\+(\d+)\b', lines[i].split('//')[0]):
                        if int(m.group(1)) <= off <= int(m.group(1)) + 24: anchor.append(i)
            if not anchor: continue
            order = []
            for i0 in anchor:
                for i in (i0, i0 - 1, i0 + 1, i0 - 2):
                    if s <= i <= e and i not in order: order.append(i)
            found_ok = False
            for i in order:
                code = joined(i)
                # candidate expressions on this line
                if kind == 'imm':
                    lit = f'0x{v1:02x}'
                    def in_gv(pos):
                        pre = code[:pos]; return len(re.findall(r'\bGV[WH]?\(', pre)) > pre.count(')') - (pre.count('(') - len(re.findall(r'\bGV[WH]?\(', pre))) and re.search(r'\bGV[WH]?\([^)]*$', pre) is not None
                    cands = [m for m in re.finditer(r'(?<![\w.])0x[0-9a-f]{2}(?![0-9a-f])', code) if int(m.group(0), 16) == v1 and not in_gv(m.start())]
                    cands = [m for m in cands if not re.search(r'burn_rom\(gb, $', code[:m.start()])]
                    if not cands and any(int(m.group(0), 16) == v1 and in_gv(m.start()) for m in re.finditer(r'(?<![\w.])0x[0-9a-f]{2}(?![0-9a-f])', code)): return True
                    if not cands:
                        cands = [m for m in re.finditer(r'(?<=[=, (])\d{1,3}(?=[;,) ])', code) if int(m.group(0)) == v1 and not re.search(r'\bb_\+$|CYCT?\(b_\+\d+, b_\+$|alu_bit\(gb, $', code[:m.start()])]
                    if not cands:
                        cands = [m for m in re.finditer(r'(?:INTERACTION_BASE|ENEMY_BASE|PART_BASE|ITEM_BASE)? ?\+? ?OBJ_\w+(?: \+ \d+)?', code) if self.eval_expr(m.group(0), 0) == v1]
                    if not cands:
                        cands = [m for m in re.finditer(r'\(uint8_t\)(?:\([^()]*\)|\w+)', code) if self.eval_expr(m.group(0).replace('(uint8_t)', ''), 0) is not None and (self.eval_expr(m.group(0).replace('(uint8_t)', ''), 0) & 0xff) == v1]
                        if len(cands) == 1 and (self.eval_expr(cands[0].group(0).replace('(uint8_t)', ''), 1) & 0xff) == v2: return True
                    if len(cands) != 1:
                        if i != order[-1]: continue
                        self.report.append(f'{name}: +{off} imm {lit} -> 0x{v2:02x}: {len(cands)} candidates near line {anchor[0] + 1} of {f}'); return False
                    m = cands[0]
                    if m.start() >= len(lines[i].split('//')[0]): self.report.append(f'{name}: +{off} imm on a continuation line at {f}:{i + 1}'); return False
                    code = lines[i].split('//')[0]
                    new = code[:m.start()] + f'GV(0x{v1:02x}, 0x{v2:02x})' + code[m.end():]
                    if apply: lines[i] = new + lines[i][len(code):]
                    self.changed[f] += 1
                    # the readable style repeats the constant in helper arguments and pointer bumps: fix those too
                    if v1 != v2:
                        for k in range(i + 1, min(i + 7, e + 1)):
                            c2 = lines[k].split('//')[0]
                            if re.search(r'\bCYCT?\(', c2) and not re.search(r'\bb_\+', c2): pass
                            c3 = re.sub(rf'((?:\w+\(gb, [^;]*?, |HL \+ |DE \+ |BC \+ )){lit}\b', rf'\1GV(0x{v1:02x}, 0x{v2:02x})', c2)
                            if c3 != c2:
                                if apply: lines[k] = c3 + lines[k][len(c2):]
                                self.changed[f] += 1
                    return True
                # 16-bit: every expression argument whose Ages evaluation equals v1 must also be right for Seasons
                def args_of(pos):
                    depth, j, start, out = 0, pos, pos, []
                    while j < len(code):
                        ch = code[j]
                        if ch == '(': depth += 1
                        elif ch == ')':
                            depth -= 1
                            if depth < 0: out.append((start, j)); break
                        elif ch == ',' and depth == 0: out.append((start, j)); start = j + 1
                        j += 1
                    return out
                exprs = []
                for m in re.finditer(r'\b([A-Za-z_]\w*)\(', code):
                    if re.match(r'CYCT?$|if$|while$|for$|GV[WH]?$', m.group(1)): continue
                    for (st, en) in args_of(m.end()):
                        exprs.append((st, en, code[st:en]))
                bases = [self.syms.get(self.base_sid, (None, None))]
                b0, b1 = (bases[0][0] & 0xffff) if bases[0][0] is not None else None, (bases[0][1] & 0xffff) if bases[0][1] is not None else None
                exprs = [(st, en, ex) for st, en, ex in exprs if not re.search(r'\bGV[WH]?\($', code[:st]) and not re.search(r'\bGV[WH]?\([^()]*,$', code[:st].rstrip())]
                for m in re.finditer(r'\bGV[WH]?\((?:[^()]|\([^()]*\))*\)', code):
                    if not any(st <= m.start() < en for st, en, _ in exprs): exprs.append((m.start(), m.end(), m.group(0)))
                hits = [(st, en, ex) for st, en, ex in exprs if ex.strip() and not re.match(r'\s*gb\s*$', ex) and self.eval_expr(ex, 0, b0) == v1]
                if not hits:
                    if i != order[-1]: continue
                    if found_ok or v1 == v2: return True     # same value in both games and not spelled on these lines: burned inside a helper
                    self.report.append(f'{name}: +{off} {kind} 0x{v1:04x} -> 0x{v2:04x}: 0 candidates near line {anchor[0] + 1} of {f}: {lines[anchor[0]].strip()[:90]}')
                    return False
                wrong = [(st, en, ex) for st, en, ex in hits if self.eval_expr(ex, 1, b1) != v2]
                if not wrong:
                    found_ok = True
                    continue        # this line is fine; other neighbouring lines may still hold a wrong copy
                if len(wrong) > 1:
                    self.report.append(f'{name}: +{off} {kind}: {len(wrong)} wrong candidates on line {i + 1} of {f}'); return False
                st, en, ex = wrong[0]
                if st >= len(lines[i].split('//')[0]):
                    self.report.append(f'{name}: +{off} {kind} on a continuation line at {f}:{i + 1}'); return False
                lead = ex[:len(ex) - len(ex.lstrip())]
                if v1 == v2 and not re.search(r'\bW8\($|\bH8\($', code[:st]):
                    new = code[:st] + f'{lead}0x{v1:04x}' + code[en:]
                    if apply: lines[i] = new + lines[i][len(code):]
                    self.changed[f] += 1
                    self.literal_fixes += 1
                    return True
                sname = self.ram_s_by_addr.get(v2) if v2 >= 0x8000 else None
                sval = sname or f'0x{v2:04x}'
                wm = re.search(r'\b(W8|H8)\($', code[:st])
                if wm and sname:
                    macro = 'GVW' if wm.group(1) == 'W8' else 'GVH'
                    new = code[:wm.start()] + f'{macro}({ex.strip()}, {sname})' + code[en + 1:]
                    if apply: lines[i] = new + lines[i][len(code):]
                    self.changed[f] += 1
                    return True
                if wm: self.report.append(f'{name}: +{off} RAM 0x{v2:04x} has no Seasons name at {f}:{i + 1}'); return False
                shared = None
                if v1 < 0x8000 and v2 < 0x8000:     # a ROM label both games have at these addresses: name it
                    na = {n for (b, a), n in self.A.by_addr.items() if a == v1 and '@' not in n}
                    ns = {n for (b, a), n in self.S.by_addr.items() if a == v2 and '@' not in n}
                    both = sorted(na & ns)
                    if len(both) == 1 and both[0] in self.syms: shared = both[0]
                new = code[:st] + (f'{lead}SYM({shared})' if shared else f'{lead}GV({ex.strip()}, {sval})') + code[en:]
                if os.environ.get('GCDEBUG'): print(f'WRAP {name} +{off} {f}:{i + 1} {ex.strip()!r} -> ages {self.eval_expr(ex, 0, b0)} seasons {self.eval_expr(ex, 1, b1)} want {v2} cur_ofs={len(self.cur_ofs)}')
                if apply: lines[i] = new + lines[i][len(code):]
                self.changed[f] += 1
                return True
            if found_ok: return True
        # constants handed to a helper: the nearest preceding b_+M reference on a line with the value
        if kind == 'imm':
            best = None
            for f, s, e in funcs:
                lines = self.files[f]
                for i in range(s, e + 1):
                    code = lines[i].split('//')[0]
                    ms = [int(x) for x in re.findall(r'\bb_\+(\d+)', code)]
                    if ms and max(ms) <= off and (best is None or max(ms) > best[0]):
                        allc = [m for m in re.finditer(r'(?<![\w.])0x[0-9a-f]{2}(?![0-9a-f])', code) if int(m.group(0), 16) == v1]
                        if any(re.search(r'\bGV[WH]?\([^)]*$', code[:m.start()]) for m in allc): return True
                        cands = [m for m in allc if not re.search(r'\bGV[WH]?\([^)]*$', code[:m.start()])]
                        if len(cands) == 1: best = (max(ms), f, i, cands[0])
            if best:
                _, f, i, m = best
                code = self.files[f][i].split('//')[0]
                new = code[:m.start()] + f'GV(0x{v1:02x}, 0x{v2:02x})' + code[m.end():]
                if apply: self.files[f][i] = new + self.files[f][i][len(code):]
                self.changed[f] += 1
                return True
        if not helpers: return self.fix_site(name, funcs, off, kind, v1, v2, tmpl, apply, quiet, helpers=True)
        if v1 == v2: return True     # the same value in both games, burned by a helper: nothing to change
        if not quiet: self.report.append(f'{name}: +{off} no C line burning that instruction')
        return False


def main():
    args = sys.argv[1:]
    apply = '--apply' in args
    if apply: args.remove('--apply')
    import subprocess
    subprocess.run([sys.executable, 'tools/routine_equiv.py', args[0], args[1], args[2], args[3], '--tsv', '/tmp/routine_equiv.tsv'], check=True, stdout=subprocess.DEVNULL)
    A, S = Game(args[0], args[1]), Game(args[2], args[3])
    tool = Tool(A, S)
    ok_names = []
    n_same = 0
    for line in open('/tmp/routine_equiv.tsv'):
        n, v, where, d = line.rstrip('\n').split('\t')
        if v not in ('SAME_SHAPE', 'IDENTICAL', 'JT_ONLY') and n.replace('__', '@') not in tool.ofs_routines: continue
        if v == 'SAME_SHAPE': n_same += 1
        bare = re.sub(r'_b[0-9a-f]{2}$', '', n).replace('__', '@')
        parent = bare.split('@')[0]
        if bare != parent: ainst = [A.labels[bare]] if bare in A.labels else []
        else:
            ainst = A.instances.get(parent) or ([A.labels[parent]] if parent in A.labels else [])
            m = re.search(r'_b([0-9a-f]{2})$', n)
            if m and len(ainst) > 1: ainst = [x for x in ainst if x[0] == int(m.group(1), 16)]
        if not ainst: continue
        ab, aa = ainst[0]; sb, sa = int(where[:2], 16), int(where[3:], 16)
        tool.auditing = v in ('IDENTICAL', 'JT_ONLY') and bare not in tool.ofs_routines
        if tool.process(bare, ab, aa, sb, sa, apply) and v == 'SAME_SHAPE': ok_names.append(n)
    if apply:
        for f, lines in tool.files.items():
            if tool.changed[f]: open(f, 'w').write('\n'.join(lines))
        # routines checked by hand (a shared helper already carries the GV, a TAIL_GV, ...)
        if os.path.exists('src/hooks/seasons_ok_manual.txt'):
            for l in open('src/hooks/seasons_ok_manual.txt'):
                n = l.split('#')[0].strip()
                if n and n not in ok_names: ok_names.append(n)
        with open('src/hooks/seasons_ok.txt', 'w') as f:
            for n in sorted(ok_names): f.write(n + '\n')
    for r in tool.report: print(r)
    print(f'{n_same} SAME_SHAPE routines: {len(ok_names)} resolved; {sum(tool.changed.values())} sites changed in {sum(1 for v in tool.changed.values() if v)} files ({tool.literal_fixes} literals restored); {len(tool.report)} notes', file=sys.stderr)


if __name__ == '__main__':
    main()
