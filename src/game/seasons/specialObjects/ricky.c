#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/specialObjects/ricky.s.

void s_rickyStateASubstate9_hook(GB *gb) {
  BASE(rickyStateASubstate9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x80;
  CYC(b_+2, b_+5); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+9); E = 0x08;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, s_rickyIncVar03, SYM(rickyIncVar03), b_+13);
  CYC(b_+13, b_+15); C = 0x20;
  CALL_C(b_+15, s_companionSetAnimation, SYM(companionSetAnimation), b_+18);
  CYC(b_+18, b_+21); SET_BC(0x4070);
  CALL_C(b_+21, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x1c);
  CYC(b_+26, b_+28); E = 0x09;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return;
}

// rickyStateASubstate9+18
static void ricky_rickyStateASubstate9_18(GB *gb) {
  BASE(rickyStateASubstate9);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+18, b_+21); SET_BC(0x4070);
  CALL_C(b_+21, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x1c);
  CYC(b_+26, b_+28); E = 0x09;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return;
}

void s_rickyStateASubstateA_hook(GB *gb) {
  BASE(rickyStateASubstateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+3);
  CALL_C(b_+3, s_companionUpdateMovement, SYM(companionUpdateMovement), b_+6);
  CYC(b_+6, b_+8); E = 0x0d;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0x38);
  if (F & FC) { CYCT(b_+11, b_+13); ricky_rickyStateASubstate9_18(gb); return; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x2004);
  CALL_C(b_+16, s_showText, SYM(showText), b_+19);
  s_rickyIncVar03(gb); return; // falls through
}

