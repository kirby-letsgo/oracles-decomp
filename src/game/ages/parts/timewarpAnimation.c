#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode2b_hook(GB *gb) {
  BASE(partCode2b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto state1; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = 0xc0;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

state1:
  CALL_C(b_+13, objectApplySpeed_hook, SYM(objectApplySpeed), b_+16);
  CYC(b_+16, b_+18); E = 0xcb; // Part.yh
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_add(gb, 0x04);
  CYC(b_+21, b_+23); alu_cp(gb, 0xf4);
  if (!(F & FC)) { CYCT(b_+23, b_+26); partDelete_hook(gb); return; } // jp nc
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+28); A = 0x04;
  CALL_C(b_+28, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_cp(gb, 0x03);
  CYC(b_+34, b_+36); E = 0xc2; // Part.subid
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  if (F & FC) { CYCT(b_+37, b_+39); goto relatedObj1_stateLessThan3; } // jr c
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+44); partDelete_hook(gb); return; } // jp nz

relatedObj1_stateLessThan3:
  CYC(b_+44, b_+46); L = 0x61;
  CYC(b_+46, b_+47); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+47, b_+48); alu_rrca(gb);
  if (F & FC) { CYCT(b_+48, b_+51); objectSetInvisible_hook(gb); return; } // jp c
  CYC(b_+48, b_+51);
  CYC(b_+51, b_+54); objectSetVisible83_hook(gb); return; // jp
}
