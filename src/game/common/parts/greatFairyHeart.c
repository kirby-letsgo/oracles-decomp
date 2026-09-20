#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode30_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58f3, 0x58f5); E = 0xc4; // Part.state
  CYC(0x58f5, 0x58f6); A = mem_rd(gb, DE);
  CYC(0x58f6, 0x58f7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x58f7, 0x58f9); goto L_5903; } // jr nz
  CYC(0x58f7, 0x58f9);
  CYC(0x58f9, 0x58fa); H = D;
  CYC(0x58fa, 0x58fb); L = E;
  CYC(0x58fb, 0x58fc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x58fc, 0x58fe); L = 0xc6; // Part.counter1
  CYC(0x58fe, 0x5900); mem_wr(gb, HL, 0x03);
  CALL_C(0x5900, objectSetVisible81_hook, 0x1e60, 0x5903);

L_5903:
  CYC(0x5903, 0x5905); A = H8(hEnemyTargetY);
  CYC(0x5905, 0x5906); B = A;
  CYC(0x5906, 0x5908); A = H8(hEnemyTargetX);
  CYC(0x5908, 0x5909); C = A;
  CYC(0x5909, 0x590b); A = 0x20;
  CYC(0x590b, 0x590d); E = 0xc9; // Part.angle
  CALL_C(0x590d, objectSetPositionInCircleArc_hook, 0x210e, 0x5910);
  CALL_C(0x5910, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5913);
  if (!(F & FZ)) { RET_TAKEN(0x5913); return; } // ret nz
  CYC(0x5913, 0x5914);
  CYC(0x5914, 0x5916); mem_wr(gb, HL, 0x03);
  CYC(0x5916, 0x5918); L = 0xc9; // Part.angle
  CYC(0x5918, 0x5919); A = mem_rd(gb, HL);
  CYC(0x5919, 0x591a); A = alu_dec8(gb, A);
  CYC(0x591a, 0x591c); alu_and(gb, 0x1f);
  CYC(0x591c, 0x591d); mem_wr(gb, HL, A);
  if (!(F & FZ)) { RET_TAKEN(0x591d); return; } // ret nz
  CYC(0x591d, 0x591e);
  CYC(0x591e, 0x5921); SET_HL(wLinkMaxHealth);
  CYC(0x5921, 0x5924); A = mem_rd(gb, wDisplayedHearts);
  CYC(0x5924, 0x5925); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x5925); return; } // ret nz
  CYC(0x5925, 0x5926);
  CYC(0x5926, 0x5928); A = 0x31;
  CALL_C(0x5928, objectGetRelatedObject1Var_hook, 0x2160, 0x592b);
  CYC(0x592b, 0x592c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x592c, 0x592f); partDelete_hook(gb); return; // jp
}
