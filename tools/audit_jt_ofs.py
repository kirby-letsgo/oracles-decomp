#!/usr/bin/env python3
"""Check jump-table chain targets written with per-game offsets against the Seasons labels.

usage: tools/audit_jt_ofs.py

A chain case `target == b_+O(n)` (or `jt_ == b_+O(n)`) must equal the address the Seasons jump
table holds. O(n) maps Ages offset n to the Seasons offset of the same instruction, which is not
the table entry when Seasons inserts instructions at the head of that case: the entry points at
the inserted ones (enemyCode37@state0 starts with a Seasons-only wRoomStateModifier check). The
chain then misses, hands the address to the dispatcher, and the native build has no code there.
For every such case whose Ages address is a label, this compares O(n) with the offset of the
same label in Seasons; a mismatch wants `b_+(game_seasons ? S(s) : n)`.
"""
import glob, re, collections

def read_syms(path):
    by_addr, by_name = collections.defaultdict(list), collections.defaultdict(list)
    for l in open(path):
        m = re.match(r'([0-9a-f]{2}):([0-9a-f]{4}) (\S+)$', l.strip())
        if m:
            b, a = int(m.group(1), 16), int(m.group(2), 16)
            by_addr[(b, a)].append(m.group(3)); by_name[m.group(3)].append((b, a))
    return by_addr, by_name

ages_addr, _ = read_syms('ref/oracles-disasm/ages.sym')
_, seasons_name = read_syms('ref/oracles-disasm/seasons.sym')
idents = re.findall(r'^  S_(\w+),$', open('src/game/syms.h').read().split('SYM_COUNT')[0], re.M)
src = open('src/game/syms.c').read()
def table(name):
    return [int(v, 16) for v in re.findall(r'0x([0-9a-f]{8})', src.split(name)[1].split('};')[0])]
sym_a = dict(zip(idents, table('syms_ages'))); sym_s = dict(zip(idents, table('syms_seasons')))
ofs = {}
for m in re.finditer(r'static const uint16_t ofs_(\w+)\[\d+\] = \{([^}]*)\};', open('src/game/ofs.c').read()):
    v = [int(x, 0) for x in m.group(2).split(',')]
    ofs[m.group(1)] = v[1:1 + v[0]]

bad = checked = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p: continue
    base = None
    for ln, line in enumerate(open(p, errors='replace'), 1):
        mb = re.search(r'\bBASE\((\w+)\)', line)
        if mb: base = mb.group(1)
        for mc in re.finditer(r'(?:target|jt_) == b_\+O\((\d+)\)', line):
            if not base or base not in ofs or base not in sym_a or sym_s.get(base, 0xffffffff) == 0xffffffff: continue
            n = int(mc.group(1))
            ab, aa = sym_a[base] >> 16, sym_a[base] & 0xffff
            sb, sa = sym_s[base] >> 16, sym_s[base] & 0xffff
            names = [x for x in ages_addr.get((ab, aa + n), []) if '@' in x]
            if not names: continue
            spots = sorted(a for b, a in seasons_name.get(names[0], []) if b == sb and a >= sa)
            if not spots: continue
            checked += 1
            want = spots[0] - sa
            got = ofs[base][n] if n < len(ofs[base]) else 0xffff
            if got != want:
                bad += 1
                print(f'{p}:{ln}: {names[0]}: O({n}) is {got}, the Seasons table entry is +{want}')
print(f'{bad} jump-table cases off their Seasons label ({checked} checked)')
