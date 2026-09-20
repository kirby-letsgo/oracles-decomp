#!/usr/bin/env python3
"""Replace hard-coded ROM/RAM addresses in src/game/ with per-game symbolic lookups.

usage: tools/symbolize.py AGES.gbc ages.sym [--apply] [FILE...]

Each C function gets a BASE(label) line naming the ROM label most of its addresses hang off;
addresses under that label become b_+offset, addresses under any other label become
SYM(label) / (SYM(label) + offset), and the bank in each file's CYC macro becomes
SYMBANK(label). RAM literals become their ram.h name plus offset. For Ages every rewritten
expression evaluates to the original literal by construction. Without --apply the tool only
prints its notes. tools/gen_syms.py then regenerates src/game/syms.c from src/hooks/syms_used.txt.
"""
import bisect, glob, os, re, sys
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from routine_equiv import Game
from symfiles import ram_map

ADDR_CALLS = {'CYC', 'CYCT', 'burn_rom', 'CALL_C', 'CALL_C_CC', 'CALL_C_', 'CALL_ROM', 'CALL_ROM_CC', 'push_effect', 'RET',
              'RET_TAKEN', 'RETI', 'I', 'HANDOFF', 'HANDOFF_UP', 'hook_continue', 'asm_call', 'hook_handoff'}
FIXED_BELOW = 0x0150  # rst/interrupt vectors and the header: identical in both games, never symbolized
HEX = re.compile(r'0x([0-9a-f]{4})\b')
FUNC_HEAD = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?\w+\([^)]*$')
FUNC_START = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?\w+\([^)]*\)\s*\{')


def label_id(name):
    return name.replace('@', '__')


class Symbolizer:
    def __init__(self, ages, ages_sym):
        self.g = ages
        self.rom_sorted = {}
        for (b, a), n in self.g.by_addr.items():
            self.rom_sorted.setdefault(b, []).append(a)
        for b in self.rom_sorted: self.rom_sorted[b].sort()
        self.instance_count = {n: len(v) for n, v in self.g.instances.items()}
        self.ram_by_addr = {}
        for cn, (bank, addr) in sorted(ram_map(ages_sym).items()):
            self.ram_by_addr.setdefault(addr, cn)
        self.ram_sorted = sorted(self.ram_by_addr)
        self.used = {}      # id -> (name, bank, addr)
        self.report = []

    def sym_id(self, bank, addr):
        n = self.g.by_addr[(bank, addr)]
        base = n.split('@')[0]
        sid = f'{label_id(n)}_b{bank:02x}' if self.instance_count.get(base, 1) > 1 else label_id(n)
        self.used[sid] = (n, bank, addr)
        return sid

    def rom_ref(self, bank, v, nearby_only):
        """(global_id, global_off, nearest_id, nearest_off) for a ROM address, or None."""
        tb = 0 if v < 0x4000 else bank
        if tb == 0 and v >= 0x4000:
            hits = [b for b in self.rom_sorted if b != 0 and (b, v) in self.g.by_addr]
            if len(hits) != 1: return None
            sid = self.sym_id(hits[0], v)
            return sid, 0, sid, 0
        addrs = self.rom_sorted.get(tb, [])
        i = bisect.bisect_right(addrs, v) - 1
        if i < 0: return None
        base = addrs[i]
        if v - base > (0x200 if nearby_only else 0x600): return None
        j = i
        while j >= 0 and '@' in self.g.by_addr[(tb, addrs[j])]: j -= 1
        gbase = addrs[j] if j >= 0 and v - addrs[j] <= 0x800 else base
        return self.sym_id(tb, gbase), v - gbase, self.sym_id(tb, base), v - base

    def ram_ref(self, v):
        i = bisect.bisect_right(self.ram_sorted, v) - 1
        if i < 0: return None
        base = self.ram_sorted[i]
        if (v >= 0xff00) != (base >= 0xff00) or v - base > 0x100: return None
        n = self.ram_by_addr[base]
        return n if base == v else f'({n} + {v - base})'

    @staticmethod
    def file_banks(text):
        banks = {'CYC': 0, 'CYCT': 0}
        for m in re.finditer(r'#define (CYCT?[0-9a-f]*)\(\w+, ?\w+\) burn_rom\(gb, ?(?:0x([0-9a-f]{2})|\(\(\w\)<0x4000\?0:(\d+)\)),', text):
            banks[m.group(1)] = int(m.group(2), 16) if m.group(2) else int(m.group(3))
        return banks

    @staticmethod
    def context(line, pos):
        """Innermost enclosing call name and argument index for the literal at pos."""
        depth, i, arg = 0, pos - 1, 0
        while i >= 0:
            c = line[i]
            if c == ')': depth += 1
            elif c == '(':
                if depth == 0:
                    m = re.search(r'([A-Za-z_][A-Za-z0-9_]*)\s*$', line[:i])
                    return (m.group(1) if m else ''), arg
                depth -= 1
            elif c == ',' and depth == 0: arg += 1
            i -= 1
        return '', arg

    def classify_line(self, path, lineno, line, banks, default_bank):
        """[(start, end, kind, payload)] for each literal on the code part of the line."""
        code = line.split('//')[0]
        out = []
        explicit_bank = None
        m = re.search(r'\bburn_rom\(gb, 0x([0-9a-f]{2}),', code)
        if m: explicit_bank = int(m.group(1), 16)
        for h in HEX.finditer(code):
            v = int(h.group(1), 16)
            call, arg = self.context(code, h.start())
            if v >= 0x8000:
                r = self.ram_ref(v)
                if r is None and 0xc000 <= v < 0xe000: self.report.append(f'{path}:{lineno}: unresolved RAM 0x{v:04x}')
                if r: out.append((h.start(), h.end(), 'ram', r))
                continue
            if v < FIXED_BELOW or (call == 'burn_rom' and arg == 1): continue
            if call.startswith('CYC'): bank = banks.get(call, default_bank)
            elif call == 'burn_rom' and explicit_bank is not None: bank = explicit_bank
            else:
                sm = re.match(r'CALL_(?:C|C_CC|ROM|ROM_CC)([0-9a-f]{2})$', call)
                bank = banks.get('CYC' + sm.group(1), default_bank) if sm else default_bank
            addr_ctx = call in ADDR_CALLS or call.startswith('CYC') or re.search(r'[=!]= *$', code[:h.start()]) \
                or (call == '' and re.search(r'\breturn +$', code[:h.start()]))
            r = None
            if call in ('CALL_C', 'CALL_C_CC', 'CALL_C_') and arg == 2:
                fm = re.search(r'\(\s*[^,]+,\s*(\w+)_hook\s*,', code)
                if fm:
                    full = fm.group(1)
                    bm = re.match(r'(.*)_b([0-9a-f]{2})$', full)
                    cands = [(b, a) for (b, a) in self.g.instances.get(full, []) if a == v]
                    if not cands and bm:
                        cands = [(b, a) for (b, a) in self.g.instances.get(bm.group(1), []) if a == v and b == int(bm.group(2), 16)]
                    if len(cands) == 1:
                        sid = self.sym_id(*cands[0]); r = (sid, 0, sid, 0)
            span_end = (call.startswith('CYC') and arg == 1) or (call == 'burn_rom' and arg == 3)
            if r is None and span_end and v > 0x150:
                r = self.rom_ref(bank, v - 1, nearby_only=False)
                if r: r = (r[0], r[1] + 1, r[2], r[3] + 1)
            if r is None:
                r = self.rom_ref(bank, v, nearby_only=not addr_ctx and v < 0x4000)
                if r and not addr_ctx and v < 0x4000 and r[3] != 0 and (call.startswith('SET_') or call.startswith('alu_') or call in ('mem_rd', 'mem_wr', 'W8', 'wr16', 'rd16')): r = None
            if r is None:
                if addr_ctx: self.report.append(f'{path}:{lineno}: unresolved ROM 0x{v:04x} in {call or "expr"} (bank {bank:02x})')
                elif 0x0150 <= v < 0x8000 and call not in ('', 'alu_and', 'alu_or', 'alu_xor'):
                    self.report.append(f'{path}:{lineno}: kept value 0x{v:04x} in {call or "expr"}')
                continue
            is_target = call in ('CALL_C', 'CALL_C_CC', 'CALL_C_') and arg == 2 or call in ('CALL_ROM', 'CALL_ROM_CC') and arg == 1 or call in ('asm_call', 'hook_continue', 'hook_handoff', 'HANDOFF', 'HANDOFF_UP')
            out.append((h.start(), h.end(), 'rom', (r, bool(addr_ctx) and not is_target, call)))
        return out

    DEFINE = re.compile(r'#define (CYCT?[0-9a-f]*)\(\w+, ?\w+\) burn_rom\(gb, ?(?:0x([0-9a-f]{2})|\(\(\w\)<0x4000\?0:(\d+)\)),')

    def rewrite_file(self, path, apply):
        text = open(path, errors='replace').read()
        lines = text.split('\n')
        classified = [None] * len(lines)
        banks = {'CYC': 0, 'CYCT': 0}
        default_bank = 0
        define_lines = []      # (line index, macro, bank in effect after it)
        for i, line in enumerate(lines):
            m = self.DEFINE.search(line)
            if m:
                banks[m.group(1)] = int(m.group(2), 16) if m.group(2) else int(m.group(3))
                default_bank = banks['CYC']
                define_lines.append((i, m.group(1)))
                continue
            if not line.lstrip().startswith('#'):
                def hram(m):
                    r = self.ram_ref(0xff00 | int(m.group(2), 16))
                    return f'mem_{m.group(1)}(gb, {r}' if r else m.group(0)
                lines[i] = line = re.sub(r'\bhram_(wr|rd)\(gb, 0x([0-9a-f]{2})', hram, line)
            if line.lstrip().startswith('#'):
                dm = re.match(r'#define (\w+) (0x([0-9a-f]{4}))\b', line)
                if dm:
                    v = int(dm.group(3), 16)
                    name = dm.group(1)
                    bm = re.search(r'_bank([0-9a-f]{2})$', name)
                    pm = re.match(r'(?:ROM_)?b([0-9a-f]{2})_(.*)$', name)
                    r = None
                    if 0x150 <= v < 0x8000 and name != 'MBC_ROM_BANK':
                        for cand in (name, re.sub(r'^ROM_', '', name), re.sub(r'_bank[0-9a-f]{2}$', '', re.sub(r'^ROM_', '', name)), pm.group(2) if pm else None):
                            hits = [(b, a) for (b, a) in self.g.instances.get(cand, []) if a == v] if cand else []
                            if hits:
                                sid = self.sym_id(*hits[0]); r = (sid, 0, sid, 0); break
                        if r is None:
                            bank = int(bm.group(1), 16) if bm else int(pm.group(1), 16) if pm else default_bank
                            r = self.rom_ref(bank, v, nearby_only=v < 0x4000)
                    if r: classified[i] = [(dm.start(2), dm.end(2), 'rom', (r, True, 'define'))]
                    elif v >= 0x8000: classified[i] = [(dm.start(2), dm.end(2), 'ram', self.ram_ref(v))] if self.ram_ref(v) else None
                    elif 0x150 <= v: self.report.append(f'{path}:{i + 1}: kept define 0x{v:04x}')
                continue
            classified[i] = self.classify_line(path, i + 1, line, dict(banks), default_bank)

        # function extents: [start_line, end_line) with base label chosen by majority of anchored addresses
        funcs = []
        start, depth, pending = None, 0, None
        for i, line in enumerate(lines):
            code = line.split('//')[0]
            if start is None:
                if pending is not None:
                    if '{' in code: start, depth, pending = i, 0, None
                    elif i - pending > 4: pending = None
                    else: continue
                elif FUNC_START.match(code): start, depth = i, 0
                elif FUNC_HEAD.match(code): pending = i; continue
                else: continue
            depth += code.count('{') - code.count('}')
            if depth <= 0:
                funcs.append((start, i + 1)); start = None
        base_of_line = {}
        base_decl = {}
        for (s, e) in funcs:
            votes = Counter()
            for i in range(s, e):
                for (_, _, kind, payload) in classified[i] or []:
                    if kind == 'rom' and payload[1] and payload[2] != 'burn_rom': votes[payload[0][0]] += 1
            if not votes: continue
            base = votes.most_common(1)[0][0]
            for i in range(s, e): base_of_line[i] = base
            base_decl[s] = base

        singles = [b for b in base_decl.values() if not re.search(r'_b[0-9a-f]{2}$', b) and not re.match(r'^(_?label_[0-9a-f]{2}_|func_[0-9a-f]{2}_)', b)]
        fallback = singles[0] if singles else next(iter(base_decl.values()), None)
        needs_bankof = set()
        if define_lines and fallback:
            for (s, e) in funcs:
                if s in base_decl: continue
                if re.search(r'(?<![0-9a-f])CYCT?\(', '\n'.join(lines[s + 1:e])): needs_bankof.add(s)
        first_after = {}       # define line index -> anchor id
        out = []
        changed = 0
        for i, line in enumerate(lines):
            items = classified[i]
            if items:
                new, last = [], 0
                for (a, b, kind, payload) in items:
                    if kind == 'ram': repl = payload
                    else:
                        (gid, goff, sid, off), addr_ctx, call = payload
                        if base_of_line.get(i) == gid: repl = f'b_+{goff}'
                        elif off == 0: repl = f'SYM({sid})'
                        else: repl = f'(SYM({sid}) + {off})'
                        if call.startswith('CYC'):
                            prev = [d for d, mac in define_lines if d < i and (mac == call or mac.rstrip('T') == call.rstrip('T'))]
                            for d in prev[-2:]:
                                first_after.setdefault(d, gid)
                    new.append(line[last:a]); new.append(repl); last = b; changed += 1
                new.append(line[last:])
                line = ''.join(new)
            out.append(line)
            decl = f'BASE({base_decl[i]});' if i in base_decl else f'BANKOF({fallback});' if i in needs_bankof else None
            if decl:
                code_ = line.split('//')[0]
                if code_[code_.index('{') + 1:].strip() == '': out.append(f'  {decl}')
                else:
                    j = out[-1].index('{') + 1
                    out[-1] = out[-1][:j] + decl + out[-1][j:]
        for d, mac in define_lines:
            if mac in ('CYC', 'CYCT'):
                out[d] = re.sub(r'(burn_rom\(gb, ?)(0x[0-9a-f]{2}|\(\(\w\)<0x4000\?0:\d+\)),', r'\1bk_,', out[d])
                continue
            anchor = first_after.get(d) or fallback
            if anchor is None:
                self.report.append(f'{path}:{d + 1}: no anchor label for macro {mac}')
                continue
            out[d] = re.sub(r'(burn_rom\(gb, ?)(0x[0-9a-f]{2}|\(\(\w\)<0x4000\?0:\d+\)),', rf'\1SYMBANK({anchor}),', out[d])
        text2 = '\n'.join(out)
        if apply and text2 != text:
            open(path, 'w').write(text2)
        return changed


def main():
    args = sys.argv[1:]
    apply = '--apply' in args
    if apply: args.remove('--apply')
    ages = Game(args[0], args[1])
    s = Symbolizer(ages, args[1])
    SKIP = {'kernel.c', 'cyc.c', 'ram_code.c'}
    files = args[2:] or ['src/game/game.h'] + [f for f in sorted(glob.glob('src/game/**/*.c', recursive=True))
                         if not os.path.basename(f).startswith('gen_') and os.path.basename(f) not in SKIP]
    total = sum(s.rewrite_file(f, apply) for f in files)
    for line in s.report: print(line)
    print(f'{len(files)} files, {total} literals rewritten, {len(s.used)} labels referenced, {len(s.report)} notes', file=sys.stderr)
    if apply or not args[2:]:
        with open('src/hooks/syms_used.txt', 'w') as f:
            for sid, (n, b, a) in sorted(s.used.items()):
                f.write(f'{sid} {n} {b:02x}:{a:04x}\n')


if __name__ == '__main__':
    main()
