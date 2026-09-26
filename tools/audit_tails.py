#!/usr/bin/env python3
"""Find control transfers the hosted build hides and the native build dies on (or burns wrong).
In functions with BASE(label):
  trampoline  a burned jp/jr whose ROM target is itself `jp x`, followed by TAIL(x) or x_hook(gb)
              with no burn of that second jp: the C skips 4 M-cycles (ghini, frame 320349).
  nopop       a hook (x_hook) that calls a function that burns a ret and never pops (a body such
              as flyingTile_saveTileDataAddress) and then returns to the dispatcher (a
              `return` or the end of the function) without RET/RET_TAKEN/ret_effect: the hook
              leaves a stale gb->pc (flyingTile, frame 262394). A goto, TAIL or HANDOFF after the
              call ends the check.
  nopush      such a call in a hook right after a burned `call` instruction with no push_effect:
              the ROM writes the return address below SP and the C does not.
usage: tools/audit_tails.py [--game=seasons] (Ages ROM and symbols for the shared and Ages files;
with --game=seasons the hand-written Seasons files against the Seasons ROM)"""
import glob, re, sys
SEASONS = '--game=seasons' in sys.argv
ROM = open('roms/Legend of Zelda, The - Oracle of ' + ('Seasons' if SEASONS else 'Ages') + ' (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_' + ('seasons' if SEASONS else 'ages') + r'\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}

def length(op):
    if op == 0xcb: return 2
    if op in (0x01, 0x11, 0x21, 0x31, 0x08, 0xc2, 0xc3, 0xc4, 0xca, 0xcc, 0xcd, 0xd2, 0xd4, 0xda, 0xdc, 0xea, 0xfa): return 3
    if op in (0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e, 0x18, 0x20, 0x28, 0x30, 0x38, 0xc6, 0xce, 0xd6, 0xde,
              0xe6, 0xee, 0xf6, 0xfe, 0xe0, 0xf0, 0xe8, 0xf8, 0x10): return 2
    return 1

def rd_abs(bank, addr): return ROM[addr if addr < 0x4000 else bank * 0x4000 + addr - 0x4000]
def rd(base, off): return rd_abs(base >> 16, (base & 0xffff) + off)

def last_insn(base, x, y):
    a, last = x, None
    for _ in range(64):
        if a == y: return last
        if a > y: return None
        last = a
        a += length(rd(base, a))
    return None

def jump_target(base, off):
    op = rd(base, off)
    if op in (0xc3, 0xc2, 0xca, 0xd2, 0xda): return rd(base, off + 1) | rd(base, off + 2) << 8
    if op in (0x18, 0x20, 0x28, 0x30, 0x38):
        d = rd(base, off + 1)
        return ((base & 0xffff) + off + 2 + (d - 256 if d > 127 else d)) & 0xffff
    return None

FUNC = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int) \*?(\w+)\([^)]*\)\s*\{')
BURN = re.compile(r'\bCYCT?\(b_\+(\d+), b_\+(\d+)\)')
ANYBURN = re.compile(r'\bCYCT?\(')
POPS = re.compile(r'\b(?:RET|RET_TAKEN|RETI)\(|\breti?_effect\(')
TAILED = re.compile(r'\bTAIL\((\w+)\)|\b(\w+)_hook\(gb\);\s*(?:return|goto)')
LEAVE = re.compile(r'\breturn\b|\bgoto\b|\bTAIL\(|\bHANDOFF\(')

funcs = []      # (path, first line, name, base, body)
paths = [p for p in sorted(glob.glob('src/game/**/*.c', recursive=True))
         if ('/seasons/' in p) == SEASONS and '/gen_' not in p and not p.endswith(('syms.c', 'ofs.c'))]
for p in paths:
    lines = open(p, errors='replace').read().split('\n')
    i = 0
    while i < len(lines):
        m = FUNC.match(lines[i])
        if not m: i += 1; continue
        j = i + 1
        while j < len(lines) and not lines[j].startswith('}'): j += 1
        body = lines[i:j + 1]
        bases = set(re.findall(r'\bBASE\((\w+)\)', '\n'.join(body)))
        base = sym.get(bases.pop()) if len(bases) == 1 else None
        funcs.append((p, i, m.group(1), base, body))
        i = j + 1

retbodies = set()
for p, start, fn, base, body in funcs:
    text = '\n'.join(l.split('//')[0] for l in body)
    if base is None or POPS.search(text) or re.search(r'gb->pc\s*=', text): continue
    for bm in BURN.finditer(text):
        off = last_insn(base, int(bm.group(1)), int(bm.group(2)))
        if off is not None and rd(base, off) in (0xc9, 0xd9): retbodies.add(fn); break

exits = {fn for p, start, fn, base, body in funcs
         if POPS.search('\n'.join(l.split('//')[0] for l in body)) or re.search(r'\b(?:HANDOFF|TAIL)\(', '\n'.join(body))}
EXIT_CALL = re.compile(r'\b(\w+)\(gb\)')

def exits_here(r, upto):
    return any(m.group(1) in exits or m.group(1).endswith('_hook') for m in EXIT_CALL.finditer(r[:upto]))

found = {'trampoline': [], 'nopop': [], 'nopush': []}
for p, start, fn, base, body in funcs:
    for k, l in enumerate(body):
        code = l.split('//')[0]
        nxt = body[k + 1].split('//')[0] if k + 1 < len(body) else ''
        where = f'{p}:{start + k + 1} {fn}'
        if base is not None:
            for bm in BURN.finditer(code):
                off = last_insn(base, int(bm.group(1)), int(bm.group(2)))
                if off is None: continue
                t = jump_target(base, off)
                if t is None: continue
                tail = code[bm.end():]
                tm = TAILED.search(tail) or (TAILED.search(nxt) if not ANYBURN.search(nxt) else None)
                if not tm or ANYBURN.search(tail[:tm.start()]): continue
                name = tm.group(1) or tm.group(2)
                dest = sym.get(name)
                if dest is None or rd_abs(base >> 16, t) != 0xc3: continue
                second = rd_abs(base >> 16, t + 1) | rd_abs(base >> 16, t + 2) << 8
                if second == dest & 0xffff and t != dest & 0xffff:
                    found['trampoline'].append(f'{where}: b_+{off} jumps to ${t:04x}, a `jp {name}` the C never burns')
        for callee in retbodies if fn.endswith('_hook') else ():
            cm = re.search(r'(?<![\w.])' + callee + r'\(gb\)', code)
            if not cm: continue
            if base is not None:
                prior = list(BURN.finditer(code[:cm.start()]))
                if prior:
                    off = last_insn(base, int(prior[-1].group(1)), int(prior[-1].group(2)))
                    if off is not None and rd(base, off) in (0xcd, 0xc4, 0xcc, 0xd4, 0xdc) and 'push_effect' not in code[prior[-1].end():cm.start()]:
                        found['nopush'].append(f'{where}: calls {callee} after the call at b_+{off} without push_effect')
            if re.search(r'\b(?:TAIL\(|\w+_hook\(gb\);\s*return)', code[cm.end():]): continue
            rest = [code[cm.end():]] + [x.split('//')[0] for x in body[k + 1:]]
            popped = False
            for r in rest:
                pm, lm = POPS.search(r), LEAVE.search(r)
                if pm and (not lm or pm.start() < lm.start()): popped = True; break
                if lm:
                    popped = not re.search(r'\breturn\b', r[lm.start():lm.start() + 8]) or exits_here(r, lm.start())
                    break
                if exits_here(r, len(r)): popped = True; break
            if not popped:
                found['nopop'].append(f'{where}: calls {callee}, whose ret is burned without a pop, and leaves without popping')
for kind, items in found.items():
    for s in sorted(set(items)): print(f'{kind}: {s}')
print(f"{len(set(found['trampoline']))} tails past a trampoline, {len(set(found['nopop']))} calls to a ret body without a pop, "
      f"{len(set(found['nopush']))} without a push ({len(retbodies)} functions burn a ret without popping)")
