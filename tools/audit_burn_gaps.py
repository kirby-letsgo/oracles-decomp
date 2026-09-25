#!/usr/bin/env python3
"""Find straight-line burns that skip or repeat part of the ROM.

usage: tools/audit_burn_gaps.py

Inside straight-line code every instruction is burned once, in order: a burn must start where the
previous one ended. A CALL_C/CALL_L/CALL burns its call at `a` and continues at its return address;
CALL_ROM and PUSH/POP burn their own instruction. The last instruction of a burn decides what may
follow it (read from the ROM): after a `call` the next burn may also start at the call's target (a
local subroutine the C runs inline); after a taken burn (CYCT), a jr/jp, ret, reti or jp hl the
straight line ends. So does a statement holding control flow (if/else/for/while/do/switch/goto/
return/TAIL/a label/a brace), a call to a helper that burns inside, or push_effect of a return
address. Branches are not judged here (audit_jumps and audit_cond do those). Reported: two burns in
a row whose ranges do not meet (zeldaApproachTrigger's `cp` at +73 was never burned;
finalDungeonEnergy burned `rst $00` twice). Ranges are compared as written (b_+N, b_+O(N), b_+S(N)
with the same wrapper); the ROM is read only for offsets in the file's own game (plain and O() in
shared files against Ages, plain and S() under src/game/seasons/ against Seasons).
"""
import glob, re

OFS = r'b_\+(?:(O|OE|S)\()?(\d+)\)?(?:\s*\+\s*(\d+))?'
BURN = re.compile(r'\b(CYCT?)\(' + OFS + r',\s*' + OFS + r'\)')
ROMCALL = re.compile(r'\bCALL_ROM\w*\(' + OFS + ',')
CALL = re.compile(r'\bCALL(?:_C\w*|_L\w*)?\(' + OFS + r',[^;]*,\s*' + OFS + r'\)\s*$')
PUSHRET = re.compile(r'push_effect\(gb, b_\+')
STACK = re.compile(r'\b(?:PUSH|POP)\(' + OFS)
HELPER = re.compile(r'\b(?!mem_rd|mem_wr|hram_rd|hram_wr|alu_\w+|push_effect|pop_effect|burn_rom|bus_read|W8|H8)\w+\(gb\b')
FLOW = re.compile(r'^\s*(?:\}|\{)|\b(?:if|else|for|while|do|switch|goto|return|TAIL\w*|HANDOFF|hook_continue|RET\w*|ret_effect)\b|^\s*\w+:\s*$')
ENDS = {0x18, 0xc3, 0xc9, 0xd9, 0xe9}
CALLS = {0xcd, 0xc4, 0xcc, 0xd4, 0xdc}
IMM8 = {0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e, 0xc6, 0xce, 0xd6, 0xde, 0xe6, 0xee, 0xf6, 0xfe,
        0x18, 0x20, 0x28, 0x30, 0x38, 0xe0, 0xf0, 0xe8, 0xf8, 0x10, 0xcb}
IMM16 = {0x01, 0x11, 0x21, 0x31, 0x08, 0xc2, 0xc3, 0xc4, 0xca, 0xcc, 0xcd, 0xd2, 0xd4, 0xda, 0xdc, 0xea, 0xfa}

def game(prefix):
    rom = open(f'roms/Legend of Zelda, The - Oracle of {prefix} (USA, Australia).gbc', 'rb').read()
    ids = re.findall(r'^\s+S_(\w+),', open('src/game/syms.h').read(), re.M)
    table = open('src/game/syms.c').read().split('syms_' + prefix.lower())[1].split('};')[0]
    return rom, dict(zip(ids, (int(v, 16) for v in re.findall(r'0x([0-9a-f]{8})', table))))
GAMES = {'Ages': game('Ages'), 'Seasons': game('Seasons')}

def kind(w): return 'O' if w in ('O', 'OE') else (w or '')
def off(m, i): return int(m.group(i + 1)) + int(m.group(i + 2) or 0)

def statements(path):
    """(line number, statement) pairs, with a statement split over lines joined first."""
    pending, start = '', 0
    for ln, line in enumerate(open(path, errors='replace'), 1):
        code = line.split('//')[0].rstrip()
        if not pending: start = ln
        pending = pending + ' ' + code.strip() if pending else code
        if pending.count('(') > pending.count(')'): continue
        for stmt in pending.split(';'): yield start, stmt
        pending = ''

bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or p.endswith(('syms.c', 'ofs.c')): continue
    seasons_file = '/seasons/' in p
    rom, sym = GAMES['Seasons' if seasons_file else 'Ages']
    prev, base = None, None     # prev: (offset kind, offsets the next burn may start at)

    def last_op(k, a, b):
        """The opcode and call target of the last instruction in [a, b), when the ROM can be read."""
        own = k == '' or (k == 'O' and not seasons_file) or (k == 'S' and seasons_file)
        if not own or base is None or base == 0xffffffff: return None, None
        bank, start = base >> 16, base & 0xffff
        def rd(o):
            x = start + o
            fo = x if x < 0x4000 else bank * 0x4000 + x - 0x4000
            return rom[fo] if 0 <= fo < len(rom) else 0
        pos, last = a, None
        for _ in range(64):
            if pos >= b: break
            last = pos
            op = rd(pos)
            pos += 3 if op in IMM16 else 2 if op in IMM8 else 1
        if last is None: return None, None
        op = rd(last)
        target = (rd(last + 1) | rd(last + 2) << 8) - start if op in CALLS else None
        return op, target

    def check(ln, k1, a):
        global bad
        if prev is not None and prev[0] == k1 and a not in prev[1]:
            bad += 1
            print(f'{p}:{ln}: burn starts at +{a}, the previous one ended at +{min(prev[1])}')

    for ln, stmt in statements(p):
        if re.match(r'^(?:static )?\w[\w *]*\(GB \*gb', stmt.strip()): prev = base = None; continue
        mb = re.search(r'\bBASE\((\w+)\)', stmt)
        if mb: base = sym.get(mb.group(1))
        if FLOW.search(stmt): prev = None; continue
        ms = STACK.search(stmt)
        if ms:
            k1, a = kind(ms.group(1)), off(ms, 1)
            check(ln, k1, a); prev = (k1, {a + 1}); continue
        mr = ROMCALL.search(stmt)
        if mr:
            k1, a = kind(mr.group(1)), off(mr, 1)
            check(ln, k1, a); prev = (k1, {a + 3}); continue
        m = BURN.search(stmt)
        if not m and not CALL.search(stmt) and HELPER.search(stmt): prev = None; continue     # a helper burns inside
        if m:
            k1, a, k2, b = kind(m.group(2)), off(m, 2), kind(m.group(5)), off(m, 5)
            check(ln, k1, a)
            op, target = last_op(k1, a, b)
            if m.group(1) == 'CYCT' or op in ENDS: prev = None
            else: prev = (k2, {b} | ({target} if target is not None else set()))
            if PUSHRET.search(stmt): prev = None
            continue
        m = CALL.search(stmt)
        if not m:
            if PUSHRET.search(stmt): prev = None
            continue
        k1, a = kind(m.group(1)), off(m, 1)
        check(ln, k1, a); prev = (kind(m.group(4)), {off(m, 4)})
print(f'{bad} gaps or overlaps in straight-line burns')
