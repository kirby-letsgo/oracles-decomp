#!/usr/bin/env python3
"""Find a RET/RET_TAKEN whose instruction the CYC just before it already burned (double burn).

usage: tools/audit_burns.py
"""
import glob, re, os
n=0
for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if os.path.basename(path).startswith('gen_'): continue
    lines=open(path, errors='replace').read().split('\n')
    for i,l in enumerate(lines):
        code=l.split('//')[0]
        for m in re.finditer(r'\b(RET_TAKEN|RET)\(b_\+(\d+)', code):
            k=int(m.group(2))
            ctx = code[:m.start()]
            if i>0: ctx = lines[i-1].split('//')[0] + ' ' + ctx
            for c in re.finditer(r'\bCYC\(b_\+(\d+), b_\+(\d+)\)', ctx):
                a,b=int(c.group(1)),int(c.group(2))
                if a<=k<b:
                    print(f'{path}:{i+1}: {m.group(1)}(b_+{k}) after CYC(b_+{a}, b_+{b})'); n+=1
print(f'{n} double burns')
