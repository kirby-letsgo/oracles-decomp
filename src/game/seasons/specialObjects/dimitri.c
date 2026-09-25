#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/specialObjects/dimitri.s.
// var38: nonzero if Dimitri is in water?

// State A: cutscene-related stuff
void s_dimitriStateA_hook(GB *gb) {
  BASE(dimitriStateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, s_companionPreventLinkFromPassing_noExtraChecks, SYM(companionPreventLinkFromPassing_noExtraChecks), b_+6);
  CALL_C(b_+6, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+9);
  CYC(b_+9, b_+11); E = 0x1a;
  CYC(b_+11, b_+13); A = 0xc7;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+17); A = mem_rd(gb, wDimitriState);
  CYC(b_+17, b_+19); alu_and(gb, 0x80);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = 0x1a;
  CYC(b_+22, b_+24); A = 0xc1;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); A = 0xff;
  CYC(b_+27, b_+30); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+30, b_+32); C = 0x1c;
  CALL_C(b_+32, s_companionSetAnimation, SYM(companionSetAnimation), b_+35);
  CYC(b_+35, b_+38);
  TAIL(companionForceMount);
}

