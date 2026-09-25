#!/usr/bin/env python3
"""Find ofsmap pairings that may have picked the wrong Seasons copy of an instruction.

usage: tools/audit_mispair.py

For every routine in src/hooks/ofs_routines.txt, each Ages instruction the aligner paired with a
Seasons instruction of the same template and length is checked: when the two differ byte for byte
while another Seasons instruction in the same routine is an exact copy of the Ages bytes, the
aligner may have paired it with the wrong one (O() then burns the wrong instruction; the cycles
still match when both are the same length, so only the constants go wrong). A hit is a review item:
a named RAM variable that moved between the games (the C names it, so it is right) shows up too.
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ofsmap
from routine_equiv import Game

AR = 'roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc'
SR = 'roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc'
A, S = Game(AR, 'ref/oracles-disasm/ages.sym'), Game(SR, 'ref/oracles-disasm/seasons.sym')


def ins_bytes(g, bank, addr, ln):
    return bytes(g.rd(bank if addr >= 0x4000 else 0, addr + i) for i in range(ln))


names, anchors = [], {}
for l in open('src/hooks/ofs_routines.txt'):
    w = l.split('#')[0].split()
    if not w: continue
    names.append(w[0])
    anchors[w[0]] = [tuple(int(x) for x in a.split('=')) for a in w[1:] if '=' in a]
found = 0
for n in names:
    a = A.labels.get(n.replace('__', '@')); s = S.labels.get(n.replace('__', '@'))
    if not a or not s: continue
    try: _, _, segs = ofsmap.align_all(A, S, n.split('@')[0], a, s, anchors.get(n, []))
    except Exception: continue
    sbody = S.body(*s)
    for seg in segs:
        ab, sb, aligned = seg[0], seg[1], seg[2]
        for ia, js in aligned:
            ai, si = ab[ia], sb[js]
            if ai[1] != si[1] or ai[3] != si[3]: continue
            ba, bs = ins_bytes(A, a[0], ai[0], ai[1]), ins_bytes(S, s[0], si[0], si[1])
            if ba == bs: continue
            copies = [x for x in sbody if x[0] != si[0] and x[1] == ai[1] and ins_bytes(S, s[0], x[0], x[1]) == ba]
            if not copies: continue
            found += 1
            print(f'{n}: ages +{ai[0] - a[1]} {ba.hex()} paired with seasons +{si[0] - s[1]} {bs.hex()}; '
                  f'exact copy at seasons ' + ', '.join(f'+{x[0] - s[1]}' for x in copies))
print(f'{found} pairings to review')
