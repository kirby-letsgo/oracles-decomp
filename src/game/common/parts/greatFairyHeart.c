#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode30_hook(GB *gb) {
  BASE(partCode30);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_5903; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0xc6; // Part.counter1
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x03);
  CALL_C(b_+13, objectSetVisible81_hook, SYM(objectSetVisible81), b_+16);

L_5903:
  CYC(b_+16, b_+18); A = H8(hEnemyTargetY);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+21); A = H8(hEnemyTargetX);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+24); A = 0x20;
  CYC(b_+24, b_+26); E = 0xc9; // Part.angle
  CALL_C(b_+26, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+29);
  CALL_C(b_+29, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+32);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; } // ret nz
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x03);
  CYC(b_+35, b_+37); L = 0xc9; // Part.angle
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); A = alu_dec8(gb, A);
  CYC(b_+39, b_+41); alu_and(gb, 0x1f);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; } // ret nz
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+46); SET_HL(wLinkMaxHealth);
  CYC(b_+46, b_+49); A = mem_rd(gb, wDisplayedHearts);
  CYC(b_+49, b_+50); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+50); return; } // ret nz
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); A = 0x31;
  CALL_C(b_+53, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+56);
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+57, b_+60); partDelete_hook(gb); return; // jp
}
