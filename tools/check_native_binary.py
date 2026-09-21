#!/usr/bin/env python3
"""The cut's gate: the native library links no interpreter and the native executables embed no
ROM bytes.

usage: tools/check_native_binary.py LIBRARY.a EXECUTABLE...

The library's member list must not contain cpu.c's object; each executable must not contain the
Nintendo logo (the 48 bytes every Game Boy ROM carries at $104, the one ROM content that is
the same in both games and cannot be game data the engine keeps on purpose).
"""
import subprocess, sys

LOGO = bytes.fromhex('CEED6666CC0D000B03730083000C000D0008111F8889000EDCCC6EE6DDDDD999BBBB67636E0EECCCDDDC999FBBB9333E')

lib, exes = sys.argv[1], sys.argv[2:]
bad = 0
members = subprocess.run(['ar', 't', lib], capture_output=True, text=True).stdout.split()
interp = [m for m in members if m.endswith('cpu.c.o') or m.endswith('cpu.o')]
if interp: print(f'{lib}: contains the interpreter: {interp}'); bad += 1
else: print(f'{lib}: no interpreter object ({len(members)} members)')
for exe in exes:
    data = open(exe, 'rb').read()
    if LOGO in data: print(f'{exe}: contains the Nintendo logo bytes (ROM content)'); bad += 1
    else: print(f'{exe}: no ROM header content ({len(data)} bytes)')
sys.exit(1 if bad else 0)
