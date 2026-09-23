#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SHIP_PIRATIAN,
// INTERAC_SHIP_PIRATIAN_CAPTAIN), bank 0x15.

void s_shipPiratian_incCbb3_hook(GB *gb) {
  BASE(shipPiratian_incCbb3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void s_shipPiratian_setRandomAnimation_hook(GB *gb) {
  BASE(shipPiratian_setRandomAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8);
  TAIL(interactionSetAnimation);
}

// Link faces right, visible, in the forced state.
void s_shipPiratian_linkBoarding_hook(GB *gb) {
  BASE(shipPiratian_linkBoarding);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link_direction);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x01); // DIR_RIGHT
  CYC(b_+8, b_+10); L = 0x1a; // w1Link.visible
  CYC(b_+10, b_+12); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+12); return;
}

// Animation from the closeness check's carry.
void s_shipPiratian_setAnimationIfLinkNear_hook(GB *gb) {
  BASE(shipPiratian_setAnimationIfLinkNear);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  CYC(b_+5, b_+6); alu_rrca(gb);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+11);
  TAIL(interactionSetAnimation);
}
