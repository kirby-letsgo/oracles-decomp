#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SUNKEN_CITY_BULLIES), bank 0x15.

// Turns toward Link: angle, direction and the matching animation.
void s_sunkenCityBullies_lookToLink_hook(GB *gb) {
  BASE(sunkenCityBullies_lookToLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+5); E = 0x49; // Interaction.angle
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CALL_C(b_+6, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+9);
  CYC(b_+9, b_+11); E = 0x48; // Interaction.direction
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15);
  TAIL(interactionSetAnimation);
}
