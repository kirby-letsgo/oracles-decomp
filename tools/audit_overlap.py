#!/usr/bin/env python3
"""Find a burn that starts before the previous straight-line burn ends (an instruction burned
twice), across lines: consecutive CYC ranges in the same block, not separated by a label, an
if/else, a return or a goto. usage: tools/audit_overlap.py"""
import glob, re
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p: continue
    prev = None
    for i, l in enumerate(open(p, errors='replace'), 1):
        code = l.split('//')[0]
        if re.search(r'\b(if|else|return|goto|case|for|while|do)\b|^\s*\w+:|[{}]', code): prev = None; continue
        for a, b in re.findall(r'\bCYCT?\(b_\+(\d+), b_\+(\d+)\)', code):
            a, b = int(a), int(b)
            if prev and a < prev[1] and a >= prev[0]:
                bad += 1; print(f'{p}:{i}: CYC(b_+{a}, ...) starts inside the previous burn ending at b_+{prev[1]}')
            prev = (a, b)
print(f'{bad} overlapping burns')
