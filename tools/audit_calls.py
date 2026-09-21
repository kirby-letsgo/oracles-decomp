#!/usr/bin/env python3
"""Check every CALL_C / CALL_C_CC / CALL_L / CALL_L_CC site against the ROM: a conditional call opcode (c4/cc/d4/dc)
must use CALL_C_CC (taken: 6 cycles), an unconditional one (cd) CALL_C.

usage: tools/audit_calls.py ROM SYM [--fix]
"""
import glob, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from symfiles import rom_labels

rom = open(sys.argv[1], 'rb').read()
labels = rom_labels(sys.argv[2])
fix = '--fix' in sys.argv
def byte(bank, addr): return rom[(bank * 0x4000 + (addr & 0x3fff)) if addr >= 0x4000 else addr]
SITE = re.compile(r'\b(CALL_C|CALL_C_CC|CALL_L|CALL_L_CC)\(\s*(b_\+(\d+)|\(SYM\((\w+)\) \+ (\d+)\)|SYM\((\w+)\)|b_)\s*,')
bad = 0
for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if os.path.basename(path).startswith('gen_'): continue
    text = open(path, errors='replace').read()
    out = []
    base = None
    for line in text.split('\n'):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = m.group(1)
        new = line
        for s in SITE.finditer(line):
            kind = s.group(1)
            if s.group(3) is not None: lab, off = base, int(s.group(3))
            elif s.group(4): lab, off = s.group(4), int(s.group(5))
            elif s.group(6): lab, off = s.group(6), 0
            else: lab, off = base, 0
            if lab is None or lab not in labels: continue
            bank, addr = labels[lab][0]
            op = byte(bank, addr + off)
            fam = 'CALL_L' if kind.startswith('CALL_L') else 'CALL_C'
            want = fam + '_CC' if op in (0xc4, 0xcc, 0xd4, 0xdc) else fam if op == 0xcd else None
            if want is None or want == kind: continue
            bad += 1
            print(f'{path}: {lab}+{off} ({bank:02x}:{addr + off:04x}) op {op:02x} uses {kind}, want {want}')
            new = new.replace(s.group(0), s.group(0).replace(kind + '(', want + '(', 1), 1)
        out.append(new)
    if fix and '\n'.join(out) != text: open(path, 'w').write('\n'.join(out))
print(f'{bad} sites wrong')
