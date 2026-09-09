#!/usr/bin/env python3
"""Bisect a replay failure down to the ported routine that causes it.

usage: tools/bisect_hooks.py ROM FRAMES [FIRST_INDEX]

Runs the headless replay in replace mode with HOOK_ONLY set to halves of the ported list
(starting at FIRST_INDEX, default 0) until a single routine is left. Filtered-out routines run
in the interpreter, so any subset is a valid run.
"""
import os, re, subprocess, sys

rom, frames = sys.argv[1], sys.argv[2]
first = int(sys.argv[3]) if len(sys.argv) > 3 else 0
names = [l.split('#')[0].strip() for l in open('src/hooks/ported.txt')]
generated = {re.sub(r'_b[0-9a-f]{2}$', '', l.split()[1]) for l in open('src/hooks/generated.txt') if l.strip()}
names = [n for n in names if n and n in generated]
cand = names[first:]

def ok(subset):
    env = dict(os.environ, HOOK_ONLY=' '.join(subset) if subset else 'none')
    cmd = ['./build-quirk/oracles-run', '--rom', rom, '--boot', 'roms/cgb_boot.bin', '--init-ram', 'tas/gbhawk-wram0.txt',
           '--tas', 'tas/ages-consoleverified.inputs', '--frames', frames, '--ref-check', 'tas/ages.ref',
           '--frame-hash-check', 'tas/ages.frames']
    try:
        r = subprocess.run(cmd, env=env, capture_output=True, text=True, timeout=300)
    except subprocess.TimeoutExpired:
        return False
    return r.returncode == 0 and 'mismatch' not in r.stdout + r.stderr

while len(cand) > 1:
    h = cand[:len(cand) // 2]
    if not ok(h): cand = h
    else:
        t = cand[len(cand) // 2:]
        if not ok(t): cand = t
        else: print('combination needed'); break
print('culprit:', cand)
