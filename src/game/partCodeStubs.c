#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCodeNil_hook(GB *gb) {
  BASE(partCodeNil);
  RET(b_+0); return; // ret
}

void partCode00_hook(GB *gb) {
  BASE(partCode00);
  CYC(b_+0, b_+3); partDelete_hook(gb); return; // jp
}
