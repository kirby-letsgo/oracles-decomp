#!/usr/bin/env python3
"""Check branch targets in the C against the ROM: after `CYCT(b_+N, b_+M); goto LABEL;` where
the instruction at BASE+N is a jump, the ROM's target must be where LABEL's code starts (its
first burn). A `goto` to a local that itself only jumps onward burns that extra jump.
usage: tools/audit_jumps.py (Ages ROM and symbols)"""
import glob, re
ROM = open('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_ages\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}
JP = {0xc3, 0xc2, 0xca, 0xd2, 0xda}; JR = {0x18, 0x20, 0x28, 0x30, 0x38}
FUNC = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int) \*?\w+\([^)]*\)\s*\{')
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/seasons/' in p or p.endswith(('syms.c', 'ofs.c')): continue
    lines = open(p, errors='replace').read().split('\n')
    i = 0
    while i < len(lines):
        if not FUNC.match(lines[i]): i += 1; continue
        j = i + 1
        while j < len(lines) and not lines[j].startswith('}'): j += 1
        body = lines[i:j + 1]; i = j + 1
        bases = set(re.findall(r'\bBASE\((\w+)\)', '\n'.join(body)))
        if len(bases) != 1: continue
        base = sym.get(bases.pop())
        if base is None: continue
        bank = base >> 16; base &= 0xffff
        labels = {}
        for k, l in enumerate(body):
            m = re.match(r'^\s*(\w+):\s*(.*)$', l.split('//')[0])
            if m and m.group(1) not in ('default', 'case'):
                rest = m.group(2) + ' ' + ' '.join(x.split('//')[0] for x in body[k + 1:k + 4])
                b = re.search(r'\b(?:CYCT?|CALL_C|CALL_C_CC|CALL_ROM|CALL_L|RET|RET_TAKEN|POP|PUSH|bank_push|switch_bank|push_effect|I)\((?:gb, )?b_\+(\d+)', rest)
                if b: labels[m.group(1)] = int(b.group(1))
        for k, l in enumerate(body):
            code = l.split('//')[0]
            # a taken conditional branch must burn the taken cost
            for m in re.finditer(r'\bif \([^{}]*\)\s*\{\s*CYC\(b_\+(\d+), b_\+(\d+)\);\s*(?:goto|TAIL|return|\w+_hook\(gb\))', code):
                n = int(m.group(1)); a = base + n
                fo = a if a < 0x4000 else bank * 0x4000 + a - 0x4000
                ln = 2 if ROM[fo] in (0x20, 0x28, 0x30, 0x38) else 1 if ROM[fo] in (0xc0, 0xc8, 0xd0, 0xd8) else 3
                if int(m.group(2)) - n != ln: continue      # the range runs on: this is the not-taken path
                if ROM[fo] in (0xc2, 0xca, 0xd2, 0xda, 0x20, 0x28, 0x30, 0x38, 0xc4, 0xcc, 0xd4, 0xdc, 0xc0, 0xc8, 0xd0, 0xd8):
                    bad += 1; print(f'{p}:{i - len(body) + k}: branch taken at b_+{n} burned with CYC (the not-taken cost)')
            for m in re.finditer(r'\b(CYCT?)\(b_\+(\d+), b_\+(\d+)\);\s*goto (\w+);', code):
                taken, n, lab = m.group(1) == 'CYCT', int(m.group(2)), m.group(4)
                if lab not in labels: continue
                a = base + n
                fo = a if a < 0x4000 else bank * 0x4000 + a - 0x4000
                op = ROM[fo]
                if op in (0xc3, 0x18) or taken:
                    if op in JP: t = ROM[fo + 1] | ROM[fo + 2] << 8
                    elif op in JR: t = (a + 2 + ((ROM[fo + 1] ^ 0x80) - 0x80)) & 0xffff
                    else: continue
                elif op in JP: t = a + 3            # not taken: falls through
                elif op in JR: t = a + 2
                else: continue
                if t != base + labels[lab]:
                    bad += 1; print(f'{p}:{i - len(body) + k}: goto {lab} (b_+{labels[lab]}) but the jump at b_+{n} targets {t:04x} (b_+{t - base})')
print(f'{bad} gotos land somewhere else than the ROM jump')
