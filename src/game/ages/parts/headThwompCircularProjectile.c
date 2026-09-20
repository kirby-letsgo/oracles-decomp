#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode3c), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode3c), (from), (to), true)

// PART_HEAD_THWOMP_CIRCULAR_PROJECTILE
void partCode3c_hook(GB *gb) {
  BASE(partCode3c);
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = 0xc4; // Part.state
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto state0; } // jr z
  CYC(b_+7, b_+9);
  CALL_C(b_+9, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+12);
  if (F & FZ) { CYCT(b_+12, b_+15); partDelete_hook(gb); return; } // jp z
  CYC(b_+12, b_+15);
  CALL_C(b_+15, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto counter1NonZero; } // jr nz
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); E = 0xf0; // Part.var30
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+27); alu_and(gb, 0x01);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = 0xc9; // Part.angle
  CYC(b_+33, b_+35); E = 0xc2; // Part.subid
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+37, b_+39); alu_and(gb, 0x1f);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);

counter1NonZero:
  CALL_C(b_+40, objectApplySpeed_hook, SYM(objectApplySpeed), b_+43);
  CYC(b_+43, b_+46); partAnimate_hook(gb); return; // jp

state0:
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+48); L = E;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+51); L = 0xc6; // Part.counter1
  CYC(b_+51, b_+53); A = 0x02;
  CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+54, b_+55); mem_wr(gb, HL, A);
  CYC(b_+55, b_+57); L = 0xd0; // Part.speed
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x64); // SPEED_280
  CALL_C(b_+59, objectSetVisible82_hook, SYM(objectSetVisible82), b_+62);
  CYC(b_+62, b_+64); A = 0xa4; // SND_BEAM
  CYC(b_+64, SYM(partCode3d)); playSound_b00_hook(gb); return; // jp
}
