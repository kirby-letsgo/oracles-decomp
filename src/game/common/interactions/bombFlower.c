#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// INTERAC_BOMB_FLOWER: ROM_AGES build compiles out the Seasons state machine,
// leaving a single unconditional delete.
void interactionCode6f_hook(GB *gb) {
  BASE(interactionCode6f);
  CYC(b_+0, b_+3); interactionDelete_hook(gb); // jp
}
