#!/usr/bin/env python3
"""Check every jump-table chain in the C against the ROM table it dispatches: each ROM entry
must be matched by a `jt_ == ...`/`target == ...` case, or the native build (no interpreter)
dies at the missing state. Seasons files are checked against the Seasons ROM, shared and Ages
files against Ages. usage: tools/audit_jumptables.py"""
import glob, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from routine_equiv import Game
AG = Game('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'ref/oracles-disasm/ages.sym')
SG = Game('roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc', 'ref/oracles-disasm/seasons.sym')
def syms(game):
    h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
    ids = re.findall(r'^\s+S_(\w+),', h, re.M)
    vals = re.findall(r'0x([0-9a-f]{8})', re.search(rf'syms_{game}\[SYM_COUNT\] = \{{\n(.*?)\n\}};', c, re.S).group(1))
    return {i: int(v, 16) for i, v in zip(ids, vals) if v != 'ffffffff'}
SYMS = {'ages': syms('ages'), 'seasons': syms('seasons')}
FUNC = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int) \*?\w+\([^)]*\)\s*\{')
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or p.endswith(('syms.c', 'ofs.c')): continue
    game = 'seasons' if '/seasons/' in p else 'ages'
    G, sym = (SG if game == 'seasons' else AG), SYMS[game]
    L = open(p, errors='replace').read().split('\n')
    base = None
    for i, line in enumerate(L):
        m = re.search(r'\bBASE\((\w+)\)', line)
        if m: base = sym.get(m.group(1))
        if FUNC.match(line) and not m: pass
        code = line.split('//')[0]
        pm = re.search(r'push_effect\(gb, b_\+(\d+)\);', code)
        if not pm or base is None: continue
        # the dispatch must follow within a few lines
        window = ' '.join(x.split('//')[0] for x in L[i:i + 3])
        if 'jump_table' not in window: continue
        k = int(pm.group(1)); bank, addr = base >> 16, (base & 0xffff) + k - 1
        try: ins = [x for x in G.body(bank, base & 0xffff) if x[0] == addr]
        except Exception: continue
        if not ins or ins[0][2] != 'jumptable': continue
        targets = sorted(set(v for _, v in ins[0][4]))
        chain = ' '.join(x.split('//')[0] for x in L[i:i + 80])
        chain = chain[:chain.find('} while (0)') if '} while (0)' in chain else len(chain)]
        have = set()
        for n in re.findall(r'(?:jt_|target) == b_\+(\d+)', chain): have.add((base & 0xffff) + int(n))
        for n in re.findall(r'(?:jt_|target) == SYM\((\w+)\)', chain):
            if n in sym: have.add(sym[n] & 0xffff)
        miss = [t for t in targets if t not in have]
        if miss:
            bad += 1
            print(f'{p}:{i + 1}: jump table at {bank:02x}:{addr:04x} has entries {", ".join(f"{t:04x}" for t in miss)} with no case')
print(f'{bad} jump-table chains missing entries')
