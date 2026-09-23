#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DIN), bank 0x15.

void s_din_createExclamationMark_hook(GB *gb) {
  BASE(din_createExclamationMark);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf300);
  CYC(b_+3, b_+6);
  TAIL(objectCreateExclamationMark);
}

void s_din_animateAndLookAtLink_hook(GB *gb) {
  BASE(din_animateAndLookAtLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CALL_C(b_+3, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+6);
  CYC(b_+6, b_+9);
  TAIL(interactionSetAnimation);
}

// interactionCodeaa: an exclamation mark.
void s_seasonsFunc_15_62d9_hook(GB *gb) {
  BASE(seasonsFunc_15_62d9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xf8;
  CYC(b_+2, b_+4); C = 0xf0;
  CYC(b_+4, b_+6); A = 0x40;
  CYC(b_+6, b_+9);
  TAIL(objectCreateExclamationMark);
}
