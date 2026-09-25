#!/usr/bin/env python3
"""A CYC/CYCT range end must be spelled from the same label as its start: `CYC(b_+5, (SYM(x) + 1))`
only holds in Ages, where label x happens to follow the range. Rewrites such an end as
start-label + length (the length is the same in both games for an identical routine).

usage: tools/audit_cyc.py [--fix]
"""
import os, re, sys, glob
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gameconst
from symfiles import ram_map

fix = '--fix' in sys.argv
from routine_equiv import Game
AGES = Game("roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", 'ref/oracles-disasm/ages.sym')
SEASONS = Game("roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", 'ref/oracles-disasm/seasons.sym')

def far_end_ok(G, bank, fr, to):
    """A range ending at another label is fine only when the burn from fr reaches to exactly on an
    instruction boundary without running past an unconditional jump (burn_rom burns while the address
    is below the end: a backward end burns nothing, and an end past a jr/jp/ret aborts the run).
    Returns None when it is fine, else what the burn does instead."""
    if to <= fr: return 'ends before it starts (burns nothing)'
    a = fr
    for _ in range(64):
        ln, kind, tmpl, ops = G.decode(bank, a)
        a += ln
        if a == to: return None
        if a > to: return f'passes its end (+{to - fr}) inside an instruction'
        if kind in ('jp', 'ret', 'jphl') and tmpl.split()[0] in ('jp', 'jr', 'ret', 'reti'):
            return f'runs past the {tmpl.split()[0]} ending +{a - fr} toward an end at +{to - fr}'
    return 'does not reach its end'

def burn_length(bank, fr, to):
    """How far burn_rom would get from fr before to, or before it would refuse to run past a jump."""
    a = fr
    while a < to:
        ln, kind, tmpl, ops = AGES.decode(bank, a)
        a += ln
        if kind in ('jp', 'ret', 'jphl') and tmpl.split()[0] in ('jp', 'jr', 'ret', 'reti'): break
    return a - fr
tool = gameconst.Tool.__new__(gameconst.Tool)
tool.syms = gameconst.load_syms()
tool.ram_a = {k: v[1] for k, v in ram_map('ref/oracles-disasm/ages.sym').items()}
tool.ram_s = {k: v[1] for k, v in ram_map('ref/oracles-disasm/seasons.sym').items()}
tool.macros = {m.group(1): m.group(2) for m in re.finditer(r'^#define (\w+) ((?:0x[0-9a-f]+|SYM\(\w+\)|\d+)(?: [+-] (?:0x[0-9a-f]+|\d+))?)\s*$', open('src/game/game.h').read(), re.M)}
tool.file_macros = {}
LABEL_OF = re.compile(r'^\(?(?:SYM\((\w+)\)|(b_))(?: ?\+ ?(\d+))?\)?$')
bad = 0
for f in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in f or f.endswith('syms.c'): continue
    text = open(f, errors='replace').read()
    tool.cur_file = f
    tool.file_macros[f] = {m.group(1): m.group(2) for m in re.finditer(r'^#define (\w+) ((?:0x[0-9a-f]+|SYM\(\w+\)|\d+)(?: [+-] (?:0x[0-9a-f]+|\d+))?)\s*$', text, re.M)}
    lines = text.split('\n')
    base = None
    for i, line in enumerate(lines):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = m.group(1)
        if base is None or base not in tool.syms: continue
        ba, bank = tool.syms[base][0] & 0xffff, tool.syms[base][0] >> 16
        code = line.split('//')[0]
        new = code
        for m in re.finditer(r'\bCYCT?\(', code):
            depth, j, start, args = 1, m.end(), m.end(), []
            while j < len(code) and depth:
                if code[j] == '(': depth += 1
                elif code[j] == ')':
                    depth -= 1
                    if depth == 0: args.append(code[start:j]); break
                elif code[j] == ',' and depth == 1: args.append(code[start:j]); start = j + 1
                j += 1
            if len(args) != 2: continue
            fr, to = args[0].strip(), args[1].strip()
            mf, mt = LABEL_OF.match(fr), LABEL_OF.match(to)
            if not mf or not mt: continue
            lf, lt = mf.group(1) or base, mt.group(1) or base
            if lf == lt: continue
            games = [(AGES, 0, ba, bank)]
            if '/ages/' not in f and tool.syms[base][1] != 0xffffffff and 'O(' not in fr + to:
                games.append((SEASONS, 1, tool.syms[base][1] & 0xffff, tool.syms[base][1] >> 16))
            for G, gi, gba, gbank in games:
                ga, gb_ = tool.eval_expr(fr, gi, gba), tool.eval_expr(to, gi, gba)
                if ga is None or gb_ is None: continue
                why = far_end_ok(G, gbank, ga, gb_)
                if why:
                    bad += 1
                    print(f'{f}:{i + 1}: CYC({fr}, {to}) in {"Seasons" if gi else "Ages"}: the burn {why}')
            va, vb = tool.eval_expr(fr, 0, ba), tool.eval_expr(to, 0, ba)
            if va is None or vb is None: continue
            length = (vb - va) & 0xffff
            if length > 0x40: continue      # not a range end that merely coincides with another label
            length = burn_length(bank, va, vb)
            bad += 1
            off = int(mf.group(3) or 0) + length
            rep = f'b_+{off}' if mf.group(2) else f'(SYM({lf}) + {off})'
            print(f'{f}:{i + 1}: CYC({fr}, {to}) -> CYC({fr}, {rep})')
            new = new.replace(args[1], rep if args[1] == to else args[1].replace(to, rep), 1)
        if fix and new != code: lines[i] = new + line[len(code):]
    if fix: open(f, 'w').write('\n'.join(lines))
print(f'{bad} range ends spelled from another label')
