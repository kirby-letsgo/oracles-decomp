#!/usr/bin/env python3
"""Lint hand-written files in src/game/: no emulated registers outside _hook shims, no raw RAM
addresses, no rewritten routine still generated, and no TAIL that re-enters its own hook.

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
    if 's_' + n.replace('@', '__').replace('.', '_') in gen_s: err('src/hooks/generated_seasons_gen.txt', 0, f'{n} is in rewritten_seasons.txt but still generated')
    if 's_' + n.replace('@', '__').replace('.', '_') + '_hook' not in gen_s: err('src/hooks/rewritten_seasons.txt', 0, f'{n} has no s_{n}_hook entry')
# A TAIL whose callee lives at an address the table owns under the *enclosing* hook's name never
# reaches that callee: hook_is() fails, hook_continue() resumes at the same address, and the table
# dispatches this hook again with nothing changed, so it recurses until the thread's stack hits its
# guard page. Ages 01:7b6e was that shape -- cutscene13 behind the tilesetLayoutGroup33 data label,
# which crashed the app on entering the fairies' hide-and-seek cutscene. A TAIL back into the hook
# under its own name is the ROM's own jump to the routine's start and is fine.
sym_order = re.findall(r'^\s+S_(\w+),', open('src/game/syms.h').read(), re.M)
syms_c = open('src/game/syms.c').read()
def sym_addrs(game):
    m = re.search(r'syms_' + game + r'\[SYM_COUNT\] = \{(.*?)\n\};', syms_c, re.S)
    v = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{8})', m.group(1))]
    return {n: (v[i] >> 16, v[i] & 0xffff) for i, n in enumerate(sym_order)}
def hook_owners(path):
    out = {}
    for l in open(path):
        p = l.split('#')[0].split()
        if len(p) >= 2 and ':' in p[0]:
            b, a = p[0].split(':')
            out[(int(b, 16), int(a, 16))] = p[1]
    return out
tables = [(sym_addrs(g), hook_owners(f)) for g, f in (('ages', 'src/hooks/generated.txt'),
                                                      ('seasons', 'src/hooks/generated_seasons.txt'))
          if os.path.exists(f)]
TAIL = re.compile(r'\bTAIL(_S|_SG)?\((\w+)\)')
for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    fn = None
    for ln, line in enumerate(open(path), 1):
        m = re.match(r'(?:static )?\w+ (\w+)\(GB \*gb\)', line)
        if m: fn = m.group(1)
        for kind, target in TAIL.findall(line):
            callee = {'': target + '_hook', '_S': 's_' + target + '_hook', '_SG': 's_' + target}[kind]
            if callee == fn: continue
            for addrs, owner in tables:
                at = addrs.get(target)
                if at and owner.get(at) == fn:
                    err(path, ln, f'TAIL({target}) re-enters {fn}: the table owns '
                                  f'{at[0]:02x}:{at[1]:04x} as {fn}, not {callee}; call {callee} directly')
print('lint: %d problems' % bad)
sys.exit(1 if bad else 0)
