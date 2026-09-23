#!/usr/bin/env python3
"""Check address operands in the C against the ROM: for `CYC(b_+X, b_+Y); A = mem_rd(gb, E);`,
`mem_wr(gb, E, A)` and `SET_BC/DE/HL(E)` where BASE+X is one `ld a,(nn)` / `ldh a,(n)` /
`ld (nn),a` / `ldh (n),a` / `ld rr,nn`, the expression E (ram.h names, RAMSYM names, IO_ names,
SYM(label), b_, hex offsets) must evaluate to the ROM's operand. A name that points at the wrong
byte shows up here even in code the playthrough never runs. usage: tools/audit_addr.py
[--game=seasons] [FILE...] (Ages ROM and the Ages-side files by default)."""
import glob, re, sys
SEASONS = '--game=seasons' in sys.argv
files = [a for a in sys.argv[1:] if not a.startswith('--')]
ROM = open('roms/Legend of Zelda, The - Oracle of ' + ('Seasons' if SEASONS else 'Ages') + ' (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_' + ('seasons' if SEASONS else 'ages') + r'\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}
rids = re.findall(r'^\s+R_(\w+),', h, re.M)
rvals = re.findall(r'0x([0-9a-f]{8})', re.search(r'ram_' + ('seasons' if SEASONS else 'ages') + r'\[RAMSYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
ramsym = {i: int(v, 16) for i, v in zip(rids, rvals)}
names = {}
for path in ('src/game/ram.h', 'src/game/game.h'):
    for m in re.finditer(r'^#define (\w+) (0x[0-9a-fA-F]+|RAMSYM\(R_(\w+)\))\s*$', open(path).read(), re.M):
        names[m.group(1)] = ramsym.get(m.group(3)) if m.group(3) else int(m.group(2), 16)

def ev(e, base):
    e = re.sub(r'\bSYM\((\w+)\)', lambda m: str(sym[m.group(1)] & 0xffff) if m.group(1) in sym else 'None', e)
    e = re.sub(r'\bb_\b', str(base & 0xffff), e)
    e = re.sub(r'\b([A-Za-z_]\w*)\b', lambda m: str(names[m.group(1)]) if names.get(m.group(1)) is not None else m.group(1), e)
    if re.search(r'[A-Za-z_]', e.replace('0x', '')): return None
    try: return eval(e) & 0xffff
    except Exception: return None

def rd(base, off):
    bank, addr = base >> 16, (base & 0xffff) + off
    return ROM[addr if addr < 0x4000 else bank * 0x4000 + addr - 0x4000]

LOAD = re.compile(r'\bCYCT?\(b_\+(\d+), b_\+(\d+)\);\s*A = mem_rd\(gb, ([^;]+?)\);')
STORE = re.compile(r'\bCYCT?\(b_\+(\d+), b_\+(\d+)\);\s*mem_wr\(gb, ([^;]+?), A\);')
PAIR = re.compile(r'\bCYCT?\(b_\+(\d+), b_\+(\d+)\);\s*SET_(BC|DE|HL)\(([^;]+?)\);')
bad = checked = 0
paths = files or sorted(glob.glob('src/game/**/*.c', recursive=True))
for p in paths:
    if p.endswith(('syms.c', 'ofs.c')) or '/gen_' in p: continue
    if ('/seasons/' in p) != SEASONS: continue
    base = None
    for n, line in enumerate(open(p, errors='replace'), 1):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = sym.get(m.group(1))
        if re.search(r'\buint16_t b_\s*[,)]', line) or line.startswith('}'): base = None
        if base is None or 'O(' in line or 'S(' in line: continue
        code = line.split('//')[0]
        checks = []
        for m in LOAD.finditer(code): checks.append((int(m.group(1)), int(m.group(2)), m.group(3), {0xfa: 3, 0xf0: 2}))
        for m in STORE.finditer(code): checks.append((int(m.group(1)), int(m.group(2)), m.group(3), {0xea: 3, 0xe0: 2}))
        for m in PAIR.finditer(code): checks.append((int(m.group(1)), int(m.group(2)), m.group(4), {{'BC': 0x01, 'DE': 0x11, 'HL': 0x21}[m.group(3)]: 3}))
        for x, y, expr, ops in checks:
            op = rd(base, x)
            if op not in ops or y - x != ops[op]: continue
            want = rd(base, x + 1) | rd(base, x + 2) << 8 if ops[op] == 3 else 0xff00 | rd(base, x + 1)
            got = ev(expr, base); checked += got is not None
            if got is not None and got != want:
                bad += 1; print(f'{p}:{n}: {expr.strip()} is ${got:04x}, ROM operand is ${want:04x}')
print(f"{bad} address operands differ from the ROM ({checked} checked)")
