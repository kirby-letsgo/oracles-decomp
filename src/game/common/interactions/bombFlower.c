#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

// INTERAC_BOMB_FLOWER: ROM_AGES build compiles out the Seasons state machine,
// leaving a single unconditional delete.
void interactionCode6f_hook(GB *gb) {
  CYC(0x4000, 0x4003); interactionDelete_hook(gb); // jp
}
