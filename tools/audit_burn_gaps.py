#!/usr/bin/env python3
"""Find straight-line burns that skip or repeat part of the ROM.

usage: tools/audit_burn_gaps.py

Inside straight-line code every instruction is burned once, in order: a burn must start where the
previous one ended. A CALL_C/CALL_L/CALL burns its call at `a` and continues at its return address.
A taken burn (CYCT), a burn whose first instruction is an unconditional jr/jp/ret/reti in the ROM,
and a burn followed by `push_effect(gb, b_+N)` (a call into a local routine) are jumps: the next
burn is at the target. A statement holding control flow (if/else/for/while/do/switch/goto/return/
TAIL/a label/a brace) ends the straight line, so branches are not judged here (audit_jumps does
those). Reported: two burns in a row whose ranges do not meet (zeldaApproachTrigger's `cp` at +73
was never burned; finalDungeonEnergy burned `rst $00` twice). Ranges are compared as written
(b_+N, b_+O(N), b_+S(N) with the same wrapper), so a mapped and an unmapped offset are not
compared. Files under src/game/seasons/ are read against the Seasons ROM, the rest against Ages.
"""
import glob, re

BURN = re.compile(r'\b(CYCT?)\(b_\+(?:(O|OE|S)\()?(\d+)\)?,\s*b_\+(?:(O|OE|S)\()?(\d+)\)?\)')
ROMCALL = re.compile(r'\bCALL_ROM\w*\(b_\+(?:(O|OE|S)\()?(\d+)\)?,')     # burns a 3-byte call, continues after it
CALL = re.compile(r'\bCALL(?:_C\w*|_L\w*)?\(b_\+(?:(O|OE|S)\()?(\d+)\)?,[^;]*,\s*b_\+(?:(O|OE|S)\()?(\d+)\)?\)\s*$')
PUSHRET = re.compile(r'push_effect\(gb, b_\+')
STACK = re.compile(r'\b(?:PUSH|POP)\(b_\+(?:(O|OE|S)\()?(\d+)\)?')     # burns its own push/pop
HELPER = re.compile(r'\b(?!mem_rd|mem_wr|hram_rd|hram_wr|alu_\w+|push_effect|pop_effect|burn_rom|bus_read|W8|H8)\w+\(gb\b')
FLOW = re.compile(r'^\s*(?:\}|\{)|\b(?:if|else|for|while|do|switch|goto|return|TAIL\w*|HANDOFF|hook_continue|RET\w*|ret_effect)\b|^\s*\w+:\s*$')
JUMPS = {0x18, 0xc3, 0xc9, 0xd9}

def game(prefix):
    rom = open(f'roms/Legend of Zelda, The - Oracle of {prefix} (USA, Australia).gbc', 'rb').read()
    ids = re.findall(r'^\s+S_(\w+),', open('src/game/syms.h').read(), re.M)
    table = open('src/game/syms.c').read().split('syms_' + prefix.lower())[1].split('};')[0]
    return rom, dict(zip(ids, (int(v, 16) for v in re.findall(r'0x([0-9a-f]{8})', table))))
GAMES = {'Ages': game('Ages'), 'Seasons': game('Seasons')}

def kind(w): return 'O' if w in ('O', 'OE') else (w or '')

bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or p.endswith(('syms.c', 'ofs.c')): continue
    seasons_file = '/seasons/' in p
    rom, sym = GAMES['Seasons' if seasons_file else 'Ages']
    prev, base = None, None
    def opcode(off):
        if base is None or base == 0xffffffff: return None
        b, a = base >> 16, (base & 0xffff) + off
        fo = a if a < 0x4000 else b * 0x4000 + a - 0x4000
        return rom[fo] if 0 <= fo < len(rom) else None
    for ln, line in enumerate(open(p, errors='replace'), 1):
        code = line.split('//')[0]
        if re.match(r'^(?:static )?\w[\w *]*\(GB \*gb', code): prev = base = None; continue
        mb = re.search(r'\bBASE\((\w+)\)', code)
        if mb: base = sym.get(mb.group(1))
        for stmt in code.split(';'):
            if FLOW.search(stmt): prev = None; continue
            ms = STACK.search(stmt)
            if ms:
                k1, a = kind(ms.group(1)), int(ms.group(2))
                if prev is not None and prev[0] == k1 and prev[1] != a:
                    bad += 1
                    print(f'{p}:{ln}: burn starts at +{a}, the previous one ended at +{prev[1]}')
                prev = (k1, a + 1)
                continue
            mr = ROMCALL.search(stmt)
            if mr:
                k1, a = kind(mr.group(1)), int(mr.group(2))
                if prev is not None and prev[0] == k1 and prev[1] != a:
                    bad += 1
                    print(f'{p}:{ln}: burn starts at +{a}, the previous one ended at +{prev[1]}')
                prev = (k1, a + 3)
                continue
            m = BURN.search(stmt)
            if not m and not CALL.search(stmt) and HELPER.search(stmt): prev = None; continue     # a helper burns inside
            if m:
                k1, a, k2, b = kind(m.group(2)), int(m.group(3)), kind(m.group(4)), int(m.group(5))
                own_offsets = k1 == '' or (k1 == 'O' and not seasons_file) or (k1 == 'S' and seasons_file)
                jump = m.group(1) == 'CYCT' or (own_offsets and opcode(a) in JUMPS)
            else:
                m = CALL.search(stmt)
                if not m:
                    if PUSHRET.search(stmt): prev = None
                    continue
                k1, a, k2, b, jump = kind(m.group(1)), int(m.group(2)), kind(m.group(3)), int(m.group(4)), False
            if prev is not None and prev[0] == k1 and prev[1] != a:
                bad += 1
                print(f'{p}:{ln}: burn starts at +{a}, the previous one ended at +{prev[1]}')
            prev = None if jump else (k2, b)
print(f'{bad} gaps or overlaps in straight-line burns')
