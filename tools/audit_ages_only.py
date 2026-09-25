#!/usr/bin/env python3
"""Find Seasons table hooks whose C is Ages-only.

usage: tools/audit_ages_only.py

AGES_ONLY() returns at once under Seasons, without the routine's `ret`, so a function that opens
with it must never be a Seasons hook: a shared chain that reaches it (a TAIL whose hook_is holds)
then returns with the stack one entry deep and the caller's CALL_C continues at a ROM address
(drawTreasureExtraTiles@val02/@val03 were listed, and the native build stopped in the inventory).
Lists every hook in src/hooks/generated_seasons.txt whose function starts with AGES_ONLY().
"""
import glob, re

ages_only = set()
for p in glob.glob('src/game/**/*.c', recursive=True):
    if '/gen_' in p: continue
    lines = open(p, errors='replace').read().split('\n')
    for i, l in enumerate(lines):
        m = re.match(r'^(?:static )?void (\w+)\(GB \*gb\) \{', l)
        if not m: continue
        for nxt in lines[i + 1:i + 4]:
            if 'AGES_ONLY()' in nxt: ages_only.add(m.group(1)); break
            if not re.match(r'\s*(BASE\(|uint16_t sp0_)', nxt): break
bad = [l.split() for l in open('src/hooks/generated_seasons.txt') if len(l.split()) >= 2 and l.split()[1] in ages_only]
for t in bad: print(f'{t[0]} {t[1]}: AGES_ONLY() but hooked under Seasons')
print(f'{len(bad)} Ages-only hooks in the Seasons table')
