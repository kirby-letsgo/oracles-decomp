#!/usr/bin/env python3
"""Check `ld r,n` immediates in the C against the ROM: for `CYC(b_+N, ...); R = <constant>;`
where the instruction at BASE+N is `ld R,n`, the constant must equal n (Ages ROM and symbols;
GV(...) takes the Ages value). usage: tools/audit_imm.py [--fix]"""
import glob, re, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
ROM = open('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'rb').read()
defs = {}
for h in ['src/game/game.h'] + glob.glob('src/game/**/*.h', recursive=True):
    if 'syms.h' in h or 'ram.h' in h or '/seasons/' in h: continue
    for m in re.finditer(r'^#define ([A-Z][A-Z0-9_]*) (\(?[0-9a-fA-Fx +\-()A-Z_]+\)?)\s*$', open(h, errors='replace').read(), re.M):
        defs.setdefault(m.group(1), m.group(2))
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_ages\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}
def ev(e, depth=0):
    e = re.sub(r'GV\(([^,()]+(?:\([^()]*\))?),[^()]+\)', r'\1', e)
    for _ in range(6):
        e2 = re.sub(r'\b([A-Z][A-Z0-9_]*)\b', lambda m: '(' + defs[m.group(1)] + ')' if m.group(1) in defs else m.group(1), e)
        if e2 == e: break
        e = e2
    if re.search(r'[A-Za-z_]{2,}', e.replace('0x', '')): return None
    try: return eval(e) & 0xff
    except Exception: return None
FIELDS = {}
for k, v in defs.items():
    if k.startswith('OBJ_'):
        try: FIELDS.setdefault(int(v, 0), []).append(k)
        except ValueError: pass
BASES = {0x40: 'INTERACTION_BASE', 0x80: 'ENEMY_BASE', 0xc0: 'PART_BASE'}
def spelled(expr, v):
    if 'OBJ_' not in expr and '_BASE' not in expr: return f'0x{v:02x}'
    base, field = (v & 0xc0), v & 0x3f
    want = re.findall(r'\bOBJ_\w+', expr)
    names = FIELDS.get(field, [])
    name = next((n for n in want if n in names), names[0] if names else None)
    if name is None: return f'0x{v:02x}'
    return f'{BASES[base]} + {name}' if base else name
fixes = {}
REG = {0x06: 'B', 0x0e: 'C', 0x16: 'D', 0x1e: 'E', 0x26: 'H', 0x2e: 'L', 0x3e: 'A'}
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/seasons/' in p or p.endswith(('syms.c', 'ofs.c')): continue
    base = None
    for n, line in enumerate(open(p, errors='replace'), 1):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = sym.get(m.group(1))
        if line.startswith('}'): base = None
        if base is None: continue
        code = line.split('//')[0]
        for m in re.finditer(r'\bCYCT?\(b_\+(\d+), b_\+(\d+)\);\s*([ABCDEHL]) = ([^;]+);', code):
            off, end, r, expr = int(m.group(1)), int(m.group(2)), m.group(3), m.group(4).strip()
            if end - off != 2: continue      # a range of several instructions: the assignment may be a later one's
            bank, addr = base >> 16, (base & 0xffff) + off
            fo = addr if addr < 0x4000 else bank * 0x4000 + addr - 0x4000
            op = ROM[fo]
            if REG.get(op) != r: continue
            v = ev(expr)
            if v is not None and v != ROM[fo + 1]:
                bad += 1; print(f'{p}:{n}: {r} = {expr} is 0x{v:02x}, ROM has ld {r.lower()},${ROM[fo+1]:02x}')
                fixes.setdefault(p, []).append((n, m.group(0), m.group(0).replace(f'{r} = {expr};', f'{r} = {spelled(expr, ROM[fo + 1])};')))
if '--fix' in sys.argv:
    for p, fs in fixes.items():
        L = open(p).read().split('\n')
        for n, old, new in fs: L[n - 1] = L[n - 1].replace(old, new, 1)
        open(p, 'w').write('\n'.join(L))
print(f'{bad} immediates differ from the ROM' + (' (fixed)' if '--fix' in sys.argv else ''))
