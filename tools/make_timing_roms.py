"""Generates tiny CGB test ROMs that record timing measurements into WRAM at C000.
Each ROM: boots in double speed, sets TAC=5 (TIMA ticks every 4 M-cycles), then runs a test.
Results: C000.. bytes. Read back by tests/timing/read_results (ours) and dump_timing.lua (BizHawk)."""
import sys

def rom_with(code, vblank=b'\xd9', stat=b'\xd9', timer=b'\xd9'):
    rom = bytearray(0x8000)
    rom[0x40:0x43] = b'\xc3\x00\x02'
    rom[0x48:0x4b] = b'\xc3\x80\x02'
    rom[0x50:0x53] = b'\xc3\x00\x03'
    rom[0x200:0x200 + len(vblank)] = vblank
    rom[0x280:0x280 + len(stat)] = stat
    rom[0x300:0x300 + len(timer)] = timer
    rom[0x100:0x104] = b'\x00\xc3\x50\x01'
    src = open('roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc', 'rb').read()
    rom[0x104:0x134] = src[0x104:0x134]
    rom[0x134:0x144] = b'TIMING'.ljust(15, b'\0') + b'\xc0'
    rom[0x147] = 0; rom[0x148] = 0; rom[0x149] = 0
    rom[0x150:0x150 + len(code)] = code
    chk = 0
    for i in range(0x134, 0x14d): chk = (chk - rom[i] - 1) & 0xff
    rom[0x14d] = chk
    return bytes(rom)

PRELUDE = (b'\xf3'                 # di
           b'\x31\xfe\xff'         # ld sp,fffe
           b'\x3e\x01\xe0\x4d'     # KEY1=1
           b'\xaf\xe0\xff'         # IE=0
           b'\x3e\x30\xe0\x00'     # P1=30
           b'\x10\x00'             # stop
           b'\x3e\x05\xe0\x07'     # TAC=5 (262144 Hz)
           b'\xaf\xe0\x06'         # TMA=0
           b'\xe0\x05'             # TIMA=0
           )

def wait_ly(n):  # busy-wait until LY == n
    return b'\xf0\x44\xfe' + bytes([n]) + b'\x20\xfa'

# Test A: DIV right after STOP, plus TIMA; then LCD off, LCD on, record TIMA/DIV at first VBlank via interrupt.
# Handler at 0x40: ld a,(TIMA) -> (C010); ld a,(DIV) -> (C011); ld a,(LY) -> (C012); inc (C013); reti
vb_a = (b'\xf5'                 # push af
        b'\xfa\x13\xc0\xb7\x20\x11'  # if (C013) != 0 skip the stores (jr nz +17)
        b'\xf0\x05\xea\x10\xc0'  # C010 = TIMA
        b'\xf0\x04\xea\x11\xc0'  # C011 = DIV
        b'\xf0\x44\xea\x12\xc0'  # C012 = LY
        b'\x21\x13\xc0\x34'      # inc (C013)
        b'\xf1\xd9')             # pop af; reti
code_a = (PRELUDE +
          b'\xf0\x04\xea\x00\xc0'   # C000 = DIV right after stop sequence
          b'\xf0\x05\xea\x01\xc0'   # C001 = TIMA
          + wait_ly(0x90) +          # wait VBlank
          b'\xaf\xe0\x40'            # LCDC=0
          b'\x3e\x00\xea\x13\xc0'    # C013=0
          b'\x3e\x01\xe0\xff'        # IE=VBlank
          b'\xaf\xe0\x0f'            # IF=0
          b'\xaf\xe0\x05'            # TIMA=0
          b'\xe0\x04'                # DIV=0 (reset)
          b'\x3e\x91\xe0\x40'        # LCDC=91 (on)
          b'\xfb'                    # ei
          + (b'\x00' * 8) + b'\x18\xf6'  # nop sled loop (jr -10)
          )
open('tests/timing/a_lcd_on.gbc', 'wb').write(rom_with(code_a, vblank=vb_a))

# Test B: interrupt latency: variant 1 halted, variant 2 busy loop. Records TIMA at handler entry for 4 frames each.
# Uses LYC=100 STAT interrupt (STAT=40) for a mid-frame event; handler: C020+idx = TIMA, idx++
st_b = (b'\xf5\xc5'                     # push af, push bc
        b'\xfa\x2f\xc0\x4f\x06\xc0'     # a=(C02F) idx; c=a; b=C0
        b'\xf0\x05\x02'                 # a=TIMA; (bc)=a
        b'\x79\x3c\xea\x2f\xc0'         # a=c; inc a; (C02F)=a
        b'\xc1\xf1\xd9')                # pop bc, pop af, reti
code_b = (PRELUDE + wait_ly(0x90) +
          b'\x3e\x64\xe0\x45'            # LYC=100
          b'\x3e\x40\xe0\x41'            # STAT=40 (LYC irq)
          b'\x3e\x02\xe0\xff'            # IE=STAT
          b'\x3e\x20\xea\x2f\xc0'        # idx=0x20
          b'\xaf\xe0\x0f'                # IF=0
          b'\xfb'                        # ei
          # phase 1: 4 halts
          b'\x76\x00\x76\x00\x76\x00\x76\x00'
          # phase 2: busy loop until idx >= 0x28
          b'\xfa\x2f\xc0\xfe\x28\x38\xf9'  # loop: a=(C02F); cp 28; jr c,loop
          b'\x18\xfe')                   # jr $ (done)
open('tests/timing/b_irq_latency.gbc', 'wb').write(rom_with(code_b, stat=st_b))

# Test C: STAT mode sequence right after LCD enable: read STAT repeatedly into C040.. (64 samples, 3 M-cycles apart)
code_c = (PRELUDE + wait_ly(0x90) +
          b'\xaf\xe0\x40'                # LCDC=0
          b'\x21\x40\xc0'                # hl=C040
          b'\x3e\x91\xe0\x40'            # LCDC=91
          + (b'\xf0\x41\x22') * 120 +     # 64x: a=STAT; ldi (hl),a   (3+2 = 5 M-cycles each)
          b'\x18\xfe')
open('tests/timing/c_lcd_modes.gbc', 'wb').write(rom_with(code_c))
print('wrote 3 roms')

# Test C2: same as C but sampling offset by one M-cycle (nop before the loop)
code_c2 = (PRELUDE + wait_ly(0x90) +
           b'\xaf\xe0\x40' + b'\x21\x40\xc0' + b'\x3e\x91\xe0\x40' + b'\x00' +
           (b'\xf0\x41\x22') * 120 + b'\x18\xfe')
open('tests/timing/c2_lcd_modes_nop.gbc', 'wb').write(rom_with(code_c2))

# Test D: after LCD enable, record TIMA at the first 6 VBlank interrupts (C010..) and first 6 LYC=50 STAT interrupts (C020..)
vb_d = (b'\xf5\xc5'                     # push af, push bc
        b'\xfa\x1f\xc0\x4f\x06\xc0'     # a=(C01F) idx; c=a; b=C0
        b'\xf0\x05\x02'                 # a=TIMA; (bc)=a
        b'\x79\x3c\xea\x1f\xc0'         # idx++
        b'\xc1\xf1\xd9')
st_d = (b'\xf5\xc5'
        b'\xfa\x2f\xc0\x4f\x06\xc0'
        b'\xf0\x05\x02'
        b'\x79\x3c\xea\x2f\xc0'
        b'\xc1\xf1\xd9')
code_d = (PRELUDE + wait_ly(0x90) +
          b'\xaf\xe0\x40'                # LCDC=0
          b'\x3e\x10\xea\x1f\xc0'        # vb idx=10
          b'\x3e\x20\xea\x2f\xc0'        # st idx=20
          b'\x3e\x32\xe0\x45'            # LYC=50
          b'\x3e\x40\xe0\x41'            # STAT=40
          b'\x3e\x03\xe0\xff'            # IE=VBlank|STAT
          b'\xaf\xe0\x0f'                # IF=0
          b'\xaf\xe0\x05'                # TIMA=0
          b'\xe0\x04'                    # DIV=0
          b'\x3e\x91\xe0\x40'            # LCDC=91
          b'\xfb'                        # ei
          + (b'\x00' * 8) + b'\x18\xf6')
open('tests/timing/d_lcd_irqs.gbc', 'wb').write(rom_with(code_d, vblank=vb_d, stat=st_d))
print('wrote C2 and D')

code_e = (PRELUDE + b'\x3e\x04\xe0\xff' + b'\xaf\xe0\x0f' + b'\xfb' + (b'\x00' * 8) + b'\x18\xf6')
open('tests/timing/e_timer_after_stop.gbc', 'wb').write(rom_with(code_e))
print('wrote E')

# Test E2: like E but TAC=4 (bit 9, 256 M-cycle ticks in double speed) and TMA=0xff so every tick overflows: timer irq every 256 M-cycles
code_e2 = (b'\xf3' b'\x31\xfe\xff' b'\x3e\x01\xe0\x4d' b'\xaf\xe0\xff' b'\x3e\x30\xe0\x00' b'\x10\x00'
           b'\x3e\x04\xe0\x07' b'\x3e\xff\xe0\x06' b'\xe0\x05'
           b'\x3e\x04\xe0\xff' + b'\xaf\xe0\x0f' + b'\xfb' + (b'\x00' * 8) + b'\x18\xf6')
open('tests/timing/e2_timer_tac4.gbc', 'wb').write(rom_with(code_e2))
print('wrote E2')

# Test C3: like C but sampling LY (FF44) instead of STAT
code_c3 = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x21\x40\xc0' + b'\x3e\x91\xe0\x40' + (b'\xf0\x44\x22') * 120 + b'\x18\xfe')
open('tests/timing/c3_ly_samples.gbc', 'wb').write(rom_with(code_c3))
# Test C4: LY samples with a 1 M-cycle phase offset
code_c4 = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x21\x40\xc0' + b'\x3e\x91\xe0\x40' + b'\x00' + (b'\xf0\x44\x22') * 120 + b'\x18\xfe')
open('tests/timing/c4_ly_samples_nop.gbc', 'wb').write(rom_with(code_c4))
print('wrote C3 C4')

for n in (2, 3, 4):
    code = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x21\x40\xc0' + b'\x3e\x91\xe0\x40' + (b'\x00' * n) + (b'\xf0\x41\x22') * 120 + b'\x18\xfe')
    open('tests/timing/c_stat_nop%d.gbc' % n, 'wb').write(rom_with(code))
    code = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x21\x40\xc0' + b'\x3e\x91\xe0\x40' + (b'\x00' * n) + (b'\xf0\x44\x22') * 120 + b'\x18\xfe')
    open('tests/timing/c_ly_nop%d.gbc' % n, 'wb').write(rom_with(code))
print('wrote phase variants')

# Test G: LCD off/on then a long nop sled (for internal per-dot observation)
code_g = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x3e\x91\xe0\x40' + (b'\x00' * 2000) + b'\x18\xfe')
open('tests/timing/g_nop_sled.gbc', 'wb').write(rom_with(code_g))
print('wrote G')

# Test S: CPU stall after STOP: reference = stop opcode (0x15f); target = 0x161 (first instruction after stop)
code_s = (PRELUDE + (b'\x00' * 16) + b'\x18\xfe')
open('tests/timing/s_stop_stall.gbc', 'wb').write(rom_with(code_s))
# Test H: GDMA of 128 blocks (2048 bytes) from 4000 to 8000 during VBlank, LCD on. HDMA5 write at 0x180, nop at 0x182.
pad = 0x180 - (0x150 + len(PRELUDE) + len(wait_ly(0x90)) + 10)
code_h = (PRELUDE + wait_ly(0x90) +
          b'\x3e\x40\xe0\x51' + b'\xaf\xe0\x52' + b'\x3e\x80\xe0\x53' + b'\xaf\xe0\x54' +   # HDMA1..4 (16 bytes)
          b'\x00' * (0x180 - 0x150 - len(PRELUDE) - len(wait_ly(0x90)) - 16 - 2) +
          b'\x3e\x7f' + b'\xe0\x55' + b'\x00' * 16 + b'\x18\xfe')
assert code_h[0x180 - 0x150] == 0xe0 and code_h[0x181 - 0x150] == 0x55, hex(code_h[0x180-0x150])
open('tests/timing/h_gdma.gbc', 'wb').write(rom_with(code_h))
# Test H2: same but HBlank-mode HDMA (0xff) while LCD on, measure time to reach 0x182 and to complete (poll HDMA5 bit7)
code_h2 = (PRELUDE + wait_ly(0x90) +
          b'\x3e\x40\xe0\x51' + b'\xaf\xe0\x52' + b'\x3e\x80\xe0\x53' + b'\xaf\xe0\x54' +
          b'\x00' * (0x180 - 0x150 - len(PRELUDE) - len(wait_ly(0x90)) - 16 - 2) +
          b'\x3e\x8f' + b'\xe0\x55' + b'\xf0\x55\xcb\x7f\x28\xfa' + b'\x00' * 8 + b'\x18\xfe')   # 16 blocks hblank; loop until bit7 set
open('tests/timing/h2_hdma.gbc', 'wb').write(rom_with(code_h2))
print('wrote S, H, H2')

# Test H3: GDMA 128 blocks with the LCD OFF. HDMA5 write at 0x180, nop at 0x182.
pre = PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x3e\x40\xe0\x51' + b'\xaf\xe0\x52' + b'\x3e\x80\xe0\x53' + b'\xaf\xe0\x54'
code_h3 = pre + b'\x00' * (0x1a0 - 0x150 - len(pre) - 2) + b'\x3e\x7f' + b'\xe0\x55' + b'\x00' * 16 + b'\x18\xfe'
assert code_h3[0x50] == 0xe0 and code_h3[0x51] == 0x55
open('tests/timing/h3_gdma_lcdoff.gbc', 'wb').write(rom_with(code_h3))
# Test H4: HBlank-mode HDMA request with LCD OFF (16 blocks), then poll HDMA5 until done
code_h4 = pre + b'\x00' * (0x1a0 - 0x150 - len(pre) - 2) + b'\x3e\x8f' + b'\xe0\x55' + b'\xf0\x55\xcb\x7f\x28\xfa' + b'\x00' * 8 + b'\x18\xfe'
open('tests/timing/h4_hdma_lcdoff.gbc', 'wb').write(rom_with(code_h4))
print('wrote H3 H4')

# Test H5: GDMA 128 blocks (LCD off) then 256 WRAM reads; H6: same with VRAM reads; H7: WRAM writes
def dma_then(loop_body_setup):
    pre = PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\x3e\x40\xe0\x51' + b'\xaf\xe0\x52' + b'\x3e\x80\xe0\x53' + b'\xaf\xe0\x54'
    code = pre + b'\x00' * (0x1a0 - 0x150 - len(pre) - 2) + b'\x3e\x7f' + b'\xe0\x55'
    code += loop_body_setup + b'\x06\x00' + b'\x7e\x23\x05\x20\xfb' + b'\x00' * 8 + b'\x18\xfe'
    return code
open('tests/timing/h5_gdma_wram_reads.gbc', 'wb').write(rom_with(dma_then(b'\x21\x00\xc0')))
open('tests/timing/h6_gdma_vram_reads.gbc', 'wb').write(rom_with(dma_then(b'\x21\x00\x90')))
open('tests/timing/h7_gdma_rom_reads.gbc', 'wb').write(rom_with(dma_then(b'\x21\x00\x40')))
print('wrote H5 H6 H7')

# Test O: OAM DMA then immediate WRAM reads (no wait loop). DMA write at 0x1a0; loop of 64 WRAM reads; anchor 0x1ae after loop.
pre_o = PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40'
code_o = pre_o + b'\x00' * (0x1a0 - 0x150 - len(pre_o) - 2) + b'\x3e\xc0' + b'\xe0\x46' + b'\x21\x00\xc0\x06\x40' + b'\x7e\x23\x05\x20\xfb' + b'\x00' * 8 + b'\x18\xfe'
assert code_o[0x50] == 0xe0 and code_o[0x51] == 0x46
open('tests/timing/o_oamdma_wram.gbc', 'wb').write(rom_with(code_o))
# Test O2: OAM DMA then immediate HRAM-only loop (should be unaffected), anchor after loop
code_o2 = pre_o + b'\x00' * (0x1a0 - 0x150 - len(pre_o) - 2) + b'\x3e\xc0' + b'\xe0\x46' + b'\x21\x80\xff\x06\x40' + b'\x7e\x05\x20\xfc' + b'\x00' * 8 + b'\x18\xfe'
open('tests/timing/o2_oamdma_hram.gbc', 'wb').write(rom_with(code_o2))
print('wrote O O2')

# Test P: mode 3 length with objects. LCD off, OAM written, LCD on, then 170 STAT samples 6 dots apart (double speed).
def p_rom(sprites, lcdc=0x97, scx=0, wx=0, wy=0, nops=0):
    oam = b'\x21\x00\xfe'
    entries = list(sprites) + [(0, 0)] * (40 - len(sprites))
    for (y, x) in entries:
        oam += bytes([0x36, y, 0x2c, 0x36, x, 0x2c, 0x36, 0x00, 0x2c, 0x36, 0x00, 0x2c])
    regs = bytes([0x3e, scx, 0xe0, 0x43, 0x3e, wx, 0xe0, 0x4a, 0x3e, wy, 0xe0, 0x4b])
    code = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + oam + regs + b'\x21\x00\xc0'
            + bytes([0x3e, lcdc, 0xe0, 0x40]) + b'\x00' * nops + (b'\xf0\x41\x22') * 170 + b'\x18\xfe')
    return rom_with(code)

P_CONFIGS = {
    'p1_game_line': dict(sprites=[(16, 16), (16, 16), (16, 56), (16, 56)]),
    'p2_x170': dict(sprites=[(16, 170)]),
    'p3_x20_x22': dict(sprites=[(16, 20), (16, 22)]),
    'p4_x0': dict(sprites=[(16, 0)]),
    'p5_window_x40': dict(sprites=[(16, 40)], lcdc=0xb7, wx=7, wy=0),
    'p6_ten': dict(sprites=[(16, 8 * i + 8) for i in range(10)]),
    'p7_scx3': dict(sprites=[(16, 16), (16, 56)], scx=3),
    'p8_x168': dict(sprites=[(16, 168)]),
    'p9_x100_101_102': dict(sprites=[(16, 100), (16, 101), (16, 102)]),
    'p0_none': dict(sprites=[]),
}
for name, cfg in P_CONFIGS.items():
    for n in range(3):
        open('tests/timing/%s_nop%d.gbc' % (name, n), 'wb').write(p_rom(nops=n, **cfg))
print('wrote P')

# Test L: LYC STAT interrupt timing in double speed. LCD off, LYC=5, STAT=0x40, IE=2, ei, LCD on, then a sled.
def l_rom(sled_unit, count):
    setup = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40'
             + b'\x3e\x05\xe0\x45' + b'\x3e\x40\xe0\x41' + b'\xaf\xe0\x0f' + b'\x3e\x02\xe0\xff' + b'\xfb')
    lcd_on_pc = 0x150 + len(setup)
    code = setup + b'\x3e\x91\xe0\x40' + sled_unit * count + b'\x18\xfe'
    return rom_with(code), lcd_on_pc
rom, pc = l_rom(b'\x00', 1400)
open('tests/timing/l_lyc_nops.gbc', 'wb').write(rom)
rom2, pc2 = l_rom(b'\xfa\x00\xc0', 350)
open('tests/timing/l2_lyc_ld4.gbc', 'wb').write(rom2)
rom3, pc3 = l_rom(b'\x21\x00\xc0', 470)   # ld hl,nn = 3 cycles
open('tests/timing/l3_lyc_ld3.gbc', 'wb').write(rom3)
print('wrote L; lcd-on pc %04x %04x %04x' % (pc, pc2, pc3))

# Test V: VBlank interrupt timing in double speed. LCD off, IE=1, ei, LCD on, then a sled (handler at 0x40 = reti).
def v_rom(sled_unit, count):
    setup = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\xaf\xe0\x0f' + b'\x3e\x01\xe0\xff' + b'\xfb')
    code = setup + b'\x3e\x91\xe0\x40' + wait_ly(0x8b) + sled_unit * count + b'\x18\xfe'
    assert len(code) < 0x7e00
    return rom_with(code)
open('tests/timing/v_vblank_nops.gbc', 'wb').write(v_rom(b'\x00', 2400))
open('tests/timing/v2_vblank_ld4.gbc', 'wb').write(v_rom(b'\xfa\x00\xc0', 600))
open('tests/timing/v3_vblank_ld3.gbc', 'wb').write(v_rom(b'\x21\x00\xc0', 800))
print('wrote V')

# Test W: VBlank interrupt timing on the second frame after LCD on (steady state).
def w_rom(sled_unit, count):
    setup = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40' + b'\xaf\xe0\x0f' + b'\x3e\x01\xe0\xff' + b'\xfb')
    head = setup + b'\x3e\x91\xe0\x40' + wait_ly(0x90) + wait_ly(0x8b) + b'\xc3\x10\x03'
    code = head + b'\x00' * (0x310 - 0x150 - len(head)) + sled_unit * count + b'\x18\xfe'
    rom = bytearray(rom_with(code))
    rom[0x200] = rom[0x280] = rom[0x300] = 0xd9
    return bytes(rom)
open('tests/timing/w_vblank2_nops.gbc', 'wb').write(w_rom(b'\x00', 2400))
open('tests/timing/w2_vblank2_ld4.gbc', 'wb').write(w_rom(b'\xfa\x00\xc0', 600))
open('tests/timing/w3_vblank2_ld3.gbc', 'wb').write(w_rom(b'\x21\x00\xc0', 800))
print('wrote W')

# Test X: halt/nop/bit/jr race. LYC=143 STAT interrupt wakes the halt loop just before VBlank; handler padded with k nops.
def x_rom(k):
    setup = (PRELUDE + wait_ly(0x90) + b'\xaf\xe0\x40'
             + b'\x3e\x8f\xe0\x45' + b'\x3e\x40\xe0\x41' + b'\xaf\xe0\x0f' + b'\x3e\x03\xe0\xff'
             + b'\x21\x00\xc0\x36\xff' + b'\xfb')
    code = setup + b'\x3e\x91\xe0\x40' + b'\x76\x00\xcb\x7e\x20\xfa'
    rom = bytearray(rom_with(code))
    handler = b'\x3e\x32' + b'\x3d\x20\xfd' + b'\x00' * k + b'\xd9'   # ld a,50; dec a; jr nz; nops; reti
    rom[0x280:0x280 + len(handler)] = handler
    return bytes(rom)
for k in range(17):
    open('tests/timing/x_race%d.gbc' % k, 'wb').write(x_rom(k))
rom = open('tests/timing/x_race0.gbc', 'rb').read()
print('wrote X; halt pc %04x' % rom.find(b'\x76\x00\xcb\x7e\x20\xfa'))
