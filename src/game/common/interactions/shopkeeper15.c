#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(shopkeeper_take10Rupees), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(shopkeeper_take10Rupees), (from), (to), true)

void shopkeeper_take10Rupees_hook(GB *gb);

void shopkeeper_take10Rupees_hook(GB *gb) {
  BASE(shopkeeper_take10Rupees);
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, SYM(movingPlatform_loadScript)); removeRupeeValue_hook(gb);
}
