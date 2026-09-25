#!/usr/bin/env python3
"""Pinpoint where one hook's C diverges from the original code under --verify-shadow.

usage: [SHADOW_TAS=FILE] [SHADOW_INIT_RAM=] tools/shadow_trace.py ages|seasons HOOK FRAMES
Runs the movie with SHADOW_ONLY=HOOK to find the first mismatching call, reruns with burn and
PC traces from that call's start, and prints the first place where the C pass's clock (at each
burned instruction) and the original code's clock disagree, with the instructions around it."""
import os, re, subprocess, sys
game, hook, frames = sys.argv[1], sys.argv[2], sys.argv[3]
ROM = {'ages': 'roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc',
       'seasons': 'roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc'}[game]
# SHADOW_TAS picks another movie; SHADOW_INIT_RAM= (empty) boots with RAM at 0 (the console-verified Seasons resync)
TAS = os.environ.get('SHADOW_TAS') or {'ages': 'tas/ages-consoleverified.inputs', 'seasons': 'tas/seasons-play.inputs'}[game]
INIT = os.environ.get('SHADOW_INIT_RAM', 'tas/gbhawk-wram0.txt')
base = ['./build-quirk/oracles-run', '--rom', ROM, '--boot', 'roms/cgb_boot.bin'] + (['--init-ram', INIT] if INIT else []) + [
        '--tas', TAS, '--frames', frames, '--verify-shadow']
def run(env):
    return subprocess.run(base, env=dict(os.environ, SHADOW_ONLY=hook, VERIFYLOG='1', **env), capture_output=True, text=True).stderr.split('\n')
L = run({})
starts = {}
for i, l in enumerate(L):
    if l.startswith(f'VERIFY> {hook} '): cur = l
    if l.startswith(f'HOOK MISMATCH {hook} '):
        mc = int(re.search(r' mc (\d+)', cur).group(1)); print(l); break
else: sys.exit('no mismatch')
L = run({'BURNLOG': str(mc), 'PCTRACE': f'{mc},20000000'})
s = next(i for i, l in enumerate(L) if l.startswith(f'VERIFY> {hook} ') and f' mc {mc} ' in l)
e = next(i for i in range(s, len(L)) if L[i].startswith(f'VERIFY= {hook} '))
f = next(i for i in range(e, len(L)) if L[i].startswith(f'VERIFY {hook} '))
C = [(l.split()[1], int(l.split()[-1]), l) for l in L[s + 1:e] if l.startswith('BURN')]
A = [(l.split()[1], int(l.split()[7]), l) for l in L[e + 1:f] if l.startswith('PCT')]
j = 0
for ci, (addr, mc_c, line) in enumerate(C):
    a = addr.split(':')[1]
    k = j
    while k < len(A) and A[k][0] != a: k += 1
    if k >= len(A): print(f'C burns {addr}, which the original code never runs after asm step {j}'); k = None
    if k is None or mc_c != A[k][1]:
        print(f'clocks disagree at {addr}: C {mc_c}, original {A[k][1] if k is not None else "-"}')
        print('C pass before:'); print('\n'.join(x[2] for x in C[max(0, ci - 8):ci + 1]))
        print('original before:'); print('\n'.join(x[2] for x in A[max(0, (k or j) - 10):(k or j) + 1]))
        break
    j = k + 1
else:
    print('clocks agree at every burn; the difference is after the last burn:')
    print('\n'.join(x[2] for x in C[-4:])); print('---'); print('\n'.join(x[2] for x in A[-6:]))
