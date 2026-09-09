#!/usr/bin/env python3
"""Summarise a PCHIST dump (interpreted instructions per bank:pc) by routine.

usage: tools/pc_hist.py HIST_FILE [ref/oracles-disasm/ages.sym] [src/hooks/ported.txt] [TOP]
Prints the unported routines that the interpreter spent the most instructions in.
"""
import re, struct, sys, bisect
hist = open(sys.argv[1], 'rb').read()
sym = sys.argv[2] if len(sys.argv) > 2 else 'ref/oracles-disasm/ages.sym'
ported_path = sys.argv[3] if len(sys.argv) > 3 else 'src/hooks/ported.txt'
top = int(sys.argv[4]) if len(sys.argv) > 4 else 60
ported = {l.split('#')[0].strip() for l in open(ported_path)}
banks = {}
for line in open(sym):
    m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', line.strip())
    if not m: continue
    b, a, n = int(m.group(1), 16), int(m.group(2), 16), m.group(3)
    if a >= 0x8000 or '@' in n or '.' in n: continue
    banks.setdefault(b, []).append((a, n))
for b in banks: banks[b].sort()
counts = {}
total = 0
vals = struct.unpack(f'<{len(hist) // 4}I', hist)
for i, c in enumerate(vals):
    if not c: continue
    total += c
    b, a = i >> 15, i & 0x7fff
    lst = banks.get(b, [])
    j = bisect.bisect_right(lst, (a, '\xff')) - 1
    n = lst[j][1] if j >= 0 else f'?{b:02x}:{a:04x}'
    counts[(b, n)] = counts.get((b, n), 0) + c
rows = sorted(counts.items(), key=lambda kv: -kv[1])
print(f'interpreted instructions: {total}')
acc = 0
for (b, n), c in rows[:top]:
    acc += c
    print(f'{c:12d} {100 * c / total:6.2f}% {100 * acc / total:6.2f}%  {b:02x} {n}{"  (ported)" if n in ported else ""}')
