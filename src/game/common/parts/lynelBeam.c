#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode1b_hook(GB *gb) {
  BASE(partCode1b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+7, b_+9); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+9, b_+12); partDelete_hook(gb); return; } // jp c
  CYC(b_+9, b_+12);

normalStatus:
  CYC(b_+12, b_+14); E = 0xc4; // Part.state
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+16, b_+18); goto L_5174; } // jr z
  CYC(b_+16, b_+18);
  CALL_C(b_+18, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+24); partDelete_hook(gb); return; } // jp nc
  CYC(b_+21, b_+24);
  CALL_C(b_+24, objectApplySpeed_hook, SYM(objectApplySpeed), b_+27);
  CYC(b_+27, b_+30); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+30, b_+32); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; } // ret nz
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); E = 0xdc; // Part.oamFlags
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_xor(gb, 0x07);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  RET(b_+39); return; // ret

L_5174:
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+42); L = E;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+43, b_+45); L = 0xd0; // Part.speed
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x78);
  CYC(b_+47, b_+49); L = 0xcb; // Part.yh
  CYC(b_+49, b_+50); B = mem_rd(gb, HL);
  CYC(b_+50, b_+52); L = 0xcd; // Part.xh
  CYC(b_+52, b_+53); C = mem_rd(gb, HL);
  CALL_C(b_+53, partCommon_setPositionOffsetAndRadiusFromAngle_hook, SYM(partCommon_setPositionOffsetAndRadiusFromAngle), b_+56);
  CYC(b_+56, b_+58); E = 0xc9; // Part.angle
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+61); A = alu_swap(gb, A);
  CYC(b_+61, b_+62); alu_rlca(gb);
  CALL_C(b_+62, partSetAnimation_hook, SYM(partSetAnimation), b_+65);
  CYC(b_+65, b_+68); TAIL(objectSetVisible81); // jp
}
