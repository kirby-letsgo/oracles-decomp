#!/usr/bin/env python3
"""Find a memory access that the C performs before the burn of its own instruction:

    alu_bit(gb, 7, mem_rd(gb, HL));      // effect
    CYC(b_+48, b_+50);                   // burn of `bit 7,(hl)`

burn_rom services interrupts at instruction boundaries, so an interrupt that lands on that
instruction runs before the ROM's access but after the C's, and the C sees (or leaves) the old
value. Reported: a CYC/CYCT over exactly one memory-accessing instruction (checked against the Ages
ROM) preceded, since the previous burn, by a statement that reads or writes memory.
usage: tools/audit_burn_order.py [--all]   (default: only accesses of RAM/IO an interrupt can touch)"""
import glob, re, sys
ROM = open('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'rb').read()
h = open('src/game/syms.h').read(); c = open('src/game/syms.c').read()
ids = re.findall(r'^\s+S_(\w+),', h, re.M)
vals = re.findall(r'0x([0-9a-f]{8})', re.search(r'syms_ages\[SYM_COUNT\] = \{\n(.*?)\n\};', c, re.S).group(1))
sym = {i: int(v, 16) for i, v in zip(ids, vals)}

def length(op, nxt):
    if op == 0xcb: return 2
    if op in (0x01, 0x11, 0x21, 0x31, 0xc2, 0xc3, 0xc4, 0xca, 0xcc, 0xcd, 0xd2, 0xd4, 0xda, 0xdc, 0xea, 0xfa, 0x08): return 3
    if op in (0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e, 0x18, 0x20, 0x28, 0x30, 0x38, 0xc6, 0xce, 0xd6, 0xde,
              0xe6, 0xee, 0xf6, 0xfe, 0xe0, 0xf0, 0xe8, 0xf8, 0x10): return 2
    return 1

def mem_insn(op, nxt):
    """(kind, address or None) when the instruction reads or writes memory."""
    if op == 0xcb: return ('(hl)', None) if nxt & 7 == 6 else None
    if op in (0xe0, 0xf0): return ('ldh', 0xff00 | nxt)
    if op in (0xe2, 0xf2): return ('(c)', None)
    if op in (0xea, 0xfa): return ('(nn)', None)
    if op in (0x02, 0x12, 0x0a, 0x1a, 0x22, 0x2a, 0x32, 0x3a, 0x34, 0x35, 0x36): return ('ind', None)
    if 0x40 <= op < 0x80 and op != 0x76 and (op & 7 == 6 or (op >> 3) & 7 == 6): return ('(hl)', None)
    if 0x80 <= op < 0xc0 and op & 7 == 6: return ('(hl)', None)
    return None

MEM = re.compile(r'\b(?:mem_rd|mem_wr|W8|H8|W16|hram_wr|hram_rd|rd16|wr16|IO_\w+)\b')
BURN = re.compile(r'\b(CYCT?)\(b_\+(\d+), b_\+(\d+)\)')
show_all = '--all' in sys.argv
bad = 0
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/gen_' in p or '/seasons/' in p or p.endswith(('syms.c', 'ofs.c')): continue
    in_irq = False
    base, pending, owed = None, None, 0
    for i, l in enumerate(open(p, errors='replace').read().split('\n')):
        bm = re.search(r'\bBASE\((\w+)\)', l)
        if bm:
            base, pending, owed = sym.get(bm.group(1)), None, 0
            in_irq = bool(re.match(r'(vblank|lcd|timer|serial)(Vector|Interrupt|Handler)|_lcdInterrupt|lcdInterrupt|vblankDma|_vblank', bm.group(1)))
        if l.startswith('}'): base = None; continue
        if base is None: continue
        code = l.split('//')[0]
        # statements in source order; each burned memory instruction owes one access, an access
        # with nothing owed belongs to a later instruction
        for st in re.split(r';', code):
            m = BURN.search(st)
            if not m and re.search(r'\b(?:CALL_C|CALL_C_CC|CALL_L|CALL_L_CC|CALL_ROM|CALL_ROM_CC|RET|RET_TAKEN)\(', st):
                pending, owed = None, 0
                continue
            if not m and re.search(r'\b(?:CYCT?|burn_rom|I)\(', st):
                pending, owed = None, 2
                continue
            if m:
                x, y = int(m.group(2)), int(m.group(3))
                b, a = base >> 16, (base & 0xffff) + x
                fo = a if a < 0x4000 else b * 0x4000 + a - 0x4000
                op, nxt = ROM[fo], ROM[fo + 1]
                mi = mem_insn(op, nxt)
                single = length(op, nxt) == y - x
                if pending and mi and single and not in_irq:
                    txt = pending[1]
                    risky = mi[0] in ('ldh', '(c)') or re.search(r'H8|hram|IO_|wVBlankChecker', txt)
                    if show_all or risky:
                        bad += 1
                        print(f'{p}:{pending[0] + 1}: `{txt.strip()}` runs before the burn of b_+{x} ({op:02x}) at line {i + 1}')
                    pending, owed = None, 0
                else:
                    pending = None
                    owed = 1 if (mi and single) else (2 if not single else 0)
                continue
            if MEM.search(st) and not re.search(r'\b(?:CALL_C|CALL_L|CALL_ROM|TAIL|RET|ret_effect|push_effect|pop_effect)\b', st):
                if owed == 1: owed = 0
                elif owed == 2: pass
                else: pending = (i, st)
print(f'{bad} memory accesses before their own burn')
