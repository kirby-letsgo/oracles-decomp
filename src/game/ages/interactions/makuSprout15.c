#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void makuSprout_setAnimation_hook(GB *gb) {
  CYC(0x72ca, 0x72cc); E = 0x7b;
  CYC(0x72cc, 0x72cd); mem_wr(gb, DE, A);
  CYC(0x72cd, 0x72d0); interactionSetAnimation_hook(gb);
}
