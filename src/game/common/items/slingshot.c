#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemCode13_hook(GB *gb) {
  BASE(itemCode13);
  CYC(b_+0, b_+1); ret_effect(gb);
}
