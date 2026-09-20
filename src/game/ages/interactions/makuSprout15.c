#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(makuSprout_setAnimation), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(makuSprout_setAnimation), (from), (to), true)

void makuSprout_setAnimation_hook(GB *gb) {
  BASE(makuSprout_setAnimation);
  CYC(b_+0, b_+2); E = 0x7b;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, SYM(makuSprout_subid00Script_body)); interactionSetAnimation_hook(gb);
}
