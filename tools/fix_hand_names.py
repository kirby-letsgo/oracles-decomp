#!/usr/bin/env python3
"""Point Seasons hand files at routines that became hand C since they were drafted.

usage: tools/fix_hand_names.py [--check]

A hand file drafted while routine x was still generated names it `s_x` and tails into it with
`TAIL_SG(x)`. Once x is listed in src/hooks/rewritten_seasons.txt it is the hand hook
`s_x_hook`, reached with `TAIL_S(x)`, and the old spelling no longer compiles. This rewrites
those spellings in src/game/seasons/ (generated gen_*.c files excepted); --check only lists them.
"""
import glob, re, sys

check = '--check' in sys.argv
hand = sorted(set(l.split('#')[0].strip().replace('@', '__') for l in open('src/hooks/rewritten_seasons.txt')
                  if l.split('#')[0].strip()), key=len, reverse=True)
if not hand: sys.exit()
alt = '|'.join(map(re.escape, hand))
name_ref = re.compile(r'\bs_(' + alt + r')\b(?!_hook)')
tail_ref = re.compile(r'\bTAIL_SG\((' + alt + r')\)')
total = 0
for p in sorted(glob.glob('src/game/seasons/**/*.c', recursive=True)):
    if '/gen_' in p: continue
    s = open(p).read()
    t, n1 = name_ref.subn(r's_\1_hook', s)
    t, n2 = tail_ref.subn(r'TAIL_S(\1)', t)
    if n1 + n2:
        total += n1 + n2
        print(f'{p}: {n1} s_x -> s_x_hook, {n2} TAIL_SG -> TAIL_S')
        if not check: open(p, 'w').write(t)
print(f'{total} spellings {"to fix" if check else "fixed"}')
