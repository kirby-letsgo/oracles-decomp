#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(itemCode13), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(itemCode13), (from), (to), true)

void itemCode13_hook(GB *gb) {
  BASE(itemCode13);
  CYC(b_+0, SYM(itemCode1e)); ret_effect(gb);
}
