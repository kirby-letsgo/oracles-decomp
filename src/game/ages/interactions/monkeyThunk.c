#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define interactionCode39_body_bank3f SYM(interactionCode39_body)

// INTERAC_MONKEY: `jpab bank3f.interactionCode39_body`, which this ROM assembles as
// `ld hl,addr; ld e,bank; jp interBankCall`. The body lives in monkeyMain.c.
void interactionCode39_hook(GB *gb) {
  BASE(interactionCode39);
  CYC(b_+0, b_+3); SET_HL(interactionCode39_body_bank3f);
  CYC(b_+3, b_+5); E = 0x3f;
  CYC(b_+5, b_+8); TAIL(interBankCall);
}
