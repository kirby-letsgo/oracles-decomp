#!/usr/bin/env python3
"""Per-game instruction offsets for routines that differ between Ages and Seasons.

usage: tools/ofsmap.py AGES.gbc ages.sym SEASONS.gbc seasons.sym [--apply] NAME...
       tools/ofsmap.py ... --report NAME       (the alignment, nothing written)

A DIFFERENT routine whose Seasons body is the Ages body with a few instructions added, removed
or moved keeps one C: every `b_+N` in its functions becomes `b_+O(N)`, and O() looks N up in a
per-routine table (src/game/ofs.c, generated here) when the running game is Seasons. The table
comes from aligning the two instruction streams (same template and symbolic operands; immediates
and in-routine offsets may differ): an aligned instruction maps its offset, and an aligned
`@+x` operand maps x too (tables and jump targets inside the routine). What the tool cannot do
is printed for hand work: Ages-only instructions (wrap their C in `if (!game_seasons)`),
Seasons-only instructions (add C under `if (game_seasons)` burning `b_+S(N)` offsets) and
differing constants (GV). src/hooks/ofs_routines.txt lists the mapped routines.
"""
import difflib, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from routine_equiv import Game
from symfiles import pair_instances, rom_labels

LIST = 'src/hooks/ofs_routines.txt'


def align(A, S, a, s):
    """[(ages_off, seasons_off)] for aligned instructions and operands, plus the hand-work lists."""
    ba, bs = A.body(*a), S.body(*s)
    na, ns = A.normalized(*a)[0], S.normalized(*s)[0]

    def key(g, insn, norm):
        addr, ln, kind, tmpl, ops = insn
        syms = []
        for k, v in ops:
            if k == 'imm': syms.append('imm')
            elif norm and re.match(r'^@[+-]\d+$', norm): syms.append('@')
            else: syms.append(norm)
        return tmpl, tuple(syms)
    # symbolic operand text per instruction, from the normalized form
    def op_texts(norm_line, tmpl):
        pat = re.escape(tmpl).replace('%s', '(.+?)') + '$'
        m = re.match(pat, norm_line)
        return list(m.groups()) if m else []
    ka = [key(A, i, None) for i in ba]; ks = [key(S, i, None) for i in bs]
    ta = [(i[3], tuple('@' if re.match(r'^@[+-]\d+$', t) else 'imm' if re.match(r'^\$[0-9a-f]{2}$', t) else t for t in op_texts(n[1], i[3]))) for i, n in zip(ba, na)]
    ts = [(i[3], tuple('@' if re.match(r'^@[+-]\d+$', t) else 'imm' if re.match(r'^\$[0-9a-f]{2}$', t) else t for t in op_texts(n[1], i[3]))) for i, n in zip(bs, ns)]
    sm = difflib.SequenceMatcher(None, ta, ts, autojunk=False)
    pairs, ages_only, seasons_only, consts = {}, [], [], []
    aligned = []        # (ages body index, seasons body index)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            for x, y in zip(range(i1, i2), range(j1, j2)):
                aligned.append((x, y))
                ia, is_ = ba[x], bs[y]
                pairs[ia[0] - a[1]] = is_[0] - s[1]
                pairs[ia[0] + ia[1] - a[1]] = is_[0] + is_[1] - s[1]
                oa, os_ = op_texts(na[x][1], ia[3]), op_texts(ns[y][1], is_[3])
                for u, v in zip(oa, os_):
                    mu, mv = re.match(r'^@([+-]\d+)$', u), re.match(r'^@([+-]\d+)$', v)
                    if mu and mv: pairs[int(mu.group(1))] = int(mv.group(1))
                    elif u == v and '@' in u:       # a named local of this routine (its tables): map its offset too
                        ml = re.match(r'^([\w@]+)(?:\+(\d+))?$', u)
                        if ml and ml.group(1) in A.labels and ml.group(1) in S.labels:
                            la, ls = A.labels[ml.group(1)], S.labels[ml.group(1)]
                            pairs[la[1] - a[1]] = ls[1] - s[1]
                    elif u != v: consts.append((ia[0] - a[1], ia[3], u, v))
        elif tag == 'replace' and i2 - i1 == j2 - j1 and all(ba[x][2] == 'jumptable' and bs[y][2] == 'jumptable' for x, y in zip(range(i1, i2), range(j1, j2))):
            # jump tables with different entries: the C dispatches whatever the ROM holds
            for x, y in zip(range(i1, i2), range(j1, j2)):
                aligned.append((x, y))
                ia, is_ = ba[x], bs[y]
                pairs[ia[0] - a[1]] = is_[0] - s[1]
                pairs[ia[0] + ia[1] - a[1]] = is_[0] + is_[1] - s[1]
        else:
            for x in range(i1, i2): ages_only.append((ba[x][0] - a[1], na[x][1]))
            for y in range(j1, j2): seasons_only.append((bs[y][0] - s[1], ns[y][1]))
    align.aligned = aligned
    return pairs, ages_only, seasons_only, consts, sm.ratio()


def main():
    args = sys.argv[1:]
    apply = '--apply' in args
    report = '--report' in args
    args = [x for x in args if not x.startswith('--')]
    A, S = Game(args[0], args[1]), Game(args[2], args[3])
    pairs_all = pair_instances(args[0], args[1], args[2], args[3])
    names = args[4:]
    listed = [l.split('#')[0].strip() for l in open(LIST)] if os.path.exists(LIST) else []
    listed = [n for n in listed if n]
    if apply: names = sorted(set(listed) | set(names))
    tables = {}
    for name in names:
        bare, mb = name, re.search(r'^(.*)_b([0-9a-f]{2})$', name)
        insts = A.instances.get(name) or ([A.labels[name]] if name in A.labels else [])
        if not insts and mb and mb.group(1) in A.instances:
            bare = mb.group(1)
            insts = [x for x in A.instances[bare] if x[0] == int(mb.group(2), 16)]
        if not insts: print(f'{name}: no such Ages label'); continue
        a = insts[0]
        s = pairs_all.get((bare,) + a)
        if not s: print(f'{name}: no Seasons pair'); continue
        pairs, ages_only, seasons_only, consts, ratio = align(A, S, a, s)
        size = max(pairs) + 1 if pairs else 0
        if report or not apply:
            print(f'== {name} ages {a[0]:02x}:{a[1]:04x} seasons {s[0]:02x}:{s[1]:04x} ratio {ratio:.2f}, {len(pairs)} offsets mapped')
            for off, text in ages_only: print(f'   ages only    +{off:<4} {text}')
            for off, text in seasons_only: print(f'   seasons only +{off:<4} {text}   (Seasons offset)')
            for off, tmpl, u, v in consts: print(f'   constant     +{off:<4} {tmpl % u} -> {v}')
        tables[name] = (size, pairs)
    if not apply: return
    with open('src/game/ofs.c', 'w') as f:
        f.write('// generated by tools/ofsmap.py; do not edit\n#include "game/syms.h"\n#include "game/ofs.h"\n')
        for name, (size, pairs) in sorted(tables.items()):
            sid = name.replace('@', '__')
            f.write(f'static const uint16_t ofs_{sid}[{size + 1}] = {{{size},')
            f.write(','.join(f'{pairs[i]}' if i in pairs and pairs[i] >= 0 else '0xffff' for i in range(size)))
            f.write('};\n')
        f.write('const uint16_t *ofs_for_sym(int sid) {\n  switch (sid) {\n')
        for name in sorted(tables):
            sid = name.replace('@', '__')
            f.write(f'  case S_{sid}: return ofs_{sid};\n')
        f.write('  default: return 0;\n  }\n}\n')
        f.write('#include <stdio.h>\n#include <stdlib.h>\nuint16_t ofs_missing(unsigned n) {\n  static int trap = -1; if (trap < 0) trap = getenv("OFS_TRAP") != NULL;\n  if (trap) { fprintf(stderr, "ofs: Ages offset +%u has no Seasons counterpart\\n", n); exit(6); }\n  return 0xffff;\n}\n')
    with open(LIST, 'w') as f:
        for name in sorted(tables): f.write(name + '\n')
    # rewrite b_+N to b_+O(N) in the functions based on a mapped label
    import glob
    changed, unmapped = 0, []
    for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
        if os.path.basename(path).startswith('gen_') or os.path.basename(path) in ('syms.c', 'ofs.c'): continue
        text = open(path, errors='replace').read()
        lines = text.split('\n')
        base, out, n, ages_only_depth = None, [], 0, 0
        ages_only_funcs = set()      # start line of every function containing AGES_ONLY()
        for i, line in enumerate(lines):
            if 'AGES_ONLY()' in line:
                j = i
                while j > 0 and not re.match(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) ', lines[j]): j -= 1
                ages_only_funcs.add(j)
        for i, line in enumerate(lines):
            m = re.search(r'\bBASE\((\w+)\)', line)
            if m: base = None if (i - 1) in ages_only_funcs or i in ages_only_funcs else m.group(1).replace('__', '@')
            if line == '}': base = None
            code = line.split('//')[0]
            # `if (!game_seasons) { ... }` blocks keep their Ages offsets as they are
            if ages_only_depth:
                ages_only_depth += code.count('{') - code.count('}')
                if ages_only_depth <= 0: ages_only_depth = 0
                out.append(line); continue
            if re.search(r'if \(!game_seasons\)\s*\{', code):
                ages_only_depth = code.count('{') - code.count('}')
                out.append(line); continue
            if base in tables:
                new = re.sub(r'\bb_\+(\d+)\b', r'b_+O(\1)', line)
                if new != line: n += 1
                line = new
                for m2 in re.finditer(r'\bO\((\d+)\)', line.split('//')[0]):
                    if int(m2.group(1)) not in tables[base][1]: unmapped.append(f'{path}:{len(out) + 1}: {base}+{m2.group(1)} has no Seasons offset (Ages-only code? wrap it in if (!game_seasons))')
            out.append(line)
        if n:
            open(path, 'w').write('\n'.join(out)); changed += n
    for u in unmapped: print(u)
    print(f'{len(tables)} routines mapped, {changed} lines rewritten, {len(unmapped)} unmapped offsets')


if __name__ == '__main__':
    main()
