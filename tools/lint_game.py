#!/usr/bin/env python3
"""Lint hand-written files in src/game/: no emulated registers outside _hook shims, no raw RAM
addresses, and no rewritten routine still generated.

usage: tools/lint_game.py
"""
import glob, os, re, sys
bad = 0
def err(path, ln, msg):
    global bad
    bad += 1
    print(f'{path}:{ln}: {msg}')
for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if os.path.basename(path).startswith('gen_') or os.path.basename(path) in ('ram_code.c', 'kernel.c', 'cyc.c', 'ofs.c', 'syms.c'): continue
    in_hook = False
    for ln, line in enumerate(open(path), 1):
        if re.match(r'void \w+_hook\(GB \*gb\)', line): in_hook = True
        elif re.match(r'\S.*\(', line) and not line.startswith('}'): in_hook = False
        if line.startswith('}'): in_hook = False
        if in_hook: continue
        if re.search(r'gb->(?:[abcdefhl]|sp|pc|f)\b', line): err(path, ln, 'emulated register outside a _hook shim')
        if re.search(r'0x[cd][0-9a-fA-F]{3}\b|0xff[89a-fA-F][0-9a-fA-F]\b', line): err(path, ln, 'raw RAM address, use ram.h')
rewritten = set(x for x in (l.split('#')[0].strip() for l in open('src/hooks/rewritten.txt')) if x) if os.path.exists('src/hooks/rewritten.txt') else set()
gen = {}
for l in open('src/hooks/generated.txt'):
    p = l.split()
    if len(p) >= 2: gen[p[1]] = l.strip()
for n in sorted(rewritten):
    if n in gen: err('src/hooks/generated.txt', 0, f'{n} is in rewritten.txt but still generated')
    if n + '_hook' not in gen: err('src/hooks/rewritten.txt', 0, f'{n} has no {n}_hook entry')
rewritten_s = set(x for x in (l.split('#')[0].strip() for l in open('src/hooks/rewritten_seasons.txt')) if x) if os.path.exists('src/hooks/rewritten_seasons.txt') else set()
gen_s = set(l.split()[1] for l in open('src/hooks/generated_seasons_gen.txt') if len(l.split()) >= 2) if os.path.exists('src/hooks/generated_seasons_gen.txt') else set()
for n in sorted(rewritten_s):
    if 's_' + n in gen_s: err('src/hooks/generated_seasons_gen.txt', 0, f'{n} is in rewritten_seasons.txt but still generated')
    if 's_' + n + '_hook' not in gen_s: err('src/hooks/rewritten_seasons.txt', 0, f'{n} has no s_{n}_hook entry')
print('lint: %d problems' % bad)
sys.exit(1 if bad else 0)
