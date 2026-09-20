#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define interactionCode39_body_bank3f 0x72e6

// INTERAC_MONKEY: `jpab bank3f.interactionCode39_body`, which this ROM assembles as
// `ld hl,addr; ld e,bank; jp interBankCall`. The body lives in monkeyMain.c.
void interactionCode39_hook(GB *gb) {
  CYC(0x7441, 0x7444); SET_HL(interactionCode39_body_bank3f);
  CYC(0x7444, 0x7446); E = 0x3f;
  CYC(0x7446, 0x7449); interBankCall_hook(gb);
}
