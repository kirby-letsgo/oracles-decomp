#!/usr/bin/env python3
"""Locate a HOOK MISMATCH cycle difference: compare the C pass's burned instructions with the
asm pass's interpreted ones for one verified hook call.

usage: BURNLOG=1 VERIFY_ALL=1 VERIFYLOG=1 PCTRACE=0,999999999 oracles-run ... --verify-hooks-continue 2>&1 | tools/verify_trace.py HOOK_NAME [FRAME]

Prints the first instruction sequence difference and every address whose burned cycle count
differs between the two passes (nested verifications are folded to their asm pass).
"""
import sys, difflib

name = sys.argv[1]
frame = sys.argv[2] if len(sys.argv) > 2 else None
lines = sys.stdin.read().split('\n')
start = None
for i, l in enumerate(lines):
    if l.startswith(f'VERIFY> {name} ') and (frame is None or f' frame {frame} ' in l): start = i; break
if start is None: sys.exit(f'no VERIFY> {name}' + (f' at frame {frame}' if frame else ''))
end = next(i for i in range(start, len(lines)) if lines[i].startswith(f'VERIFY {name} '))
seg = lines[start + 1:end]
i_eq = next(i for i, l in enumerate(seg) if l.startswith(f'VERIFY= {name} '))
cpass, apass = seg[:i_eq], seg[i_eq + 1:]

seq, skip = [], 0
for l in cpass:
    if l.startswith('VERIFY> '): skip += 1; continue
    if l.startswith('VERIFY= '): skip -= 1; continue
    if l.startswith('VERIFY ') or skip > 0: continue
    p = l.split()
    if l.startswith('BURN'): seq.append((p[1].split(':')[1], int(p[5]), int(p[7])))
    elif l.startswith('PCT'): seq.append((p[1], None, int(p[7])))
cs = []
for k, (a, c, mc) in enumerate(seq):
    if c is None: c = (seq[k + 1][2] if k + 1 < len(seq) else mc) - mc
    cs.append((a, c))
ap = [l.split() for l in apass if l.startswith('PCT')]
asq = [(p[1], (int(ap[k + 1][7]) if k + 1 < len(ap) else int(p[7])) - int(p[7])) for k, p in enumerate(ap)]
print(f'C pass: {len(cs)} instructions, {sum(c for _, c in cs)} cycles; asm pass: {len(asq)} instructions, {sum(c for _, c in asq)} cycles')
sm = difflib.SequenceMatcher(None, [a for a, _ in cs], [a for a, _ in asq], autojunk=False)
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag != 'equal': print(tag, 'C:', cs[i1:i2][:10], 'asm:', asq[j1:j2][:10])
    else:
        for x in range(i2 - i1):
            if cs[i1 + x][1] != asq[j1 + x][1]: print('cycles differ at', cs[i1 + x][0], 'C', cs[i1 + x][1], 'asm', asq[j1 + x][1])
