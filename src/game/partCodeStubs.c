#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCodeNil_hook(GB *gb) {
  RET(0x5f55); return; // ret
}

void partCode00_hook(GB *gb) {
  CYC(0x5f56, 0x5f59); partDelete_hook(gb); return; // jp
}
