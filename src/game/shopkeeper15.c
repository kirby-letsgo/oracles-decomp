#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void shopkeeper_take10Rupees_hook(GB *gb);

void shopkeeper_take10Rupees_hook(GB *gb) {
  CYC(0x411c, 0x411e); A = 0x04;
  CYC(0x411e, 0x4121); removeRupeeValue_hook(gb);
}
