#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void vire_activateMusic_hook(GB *gb) {
  CYC(0x77e6, 0x77e7); alu_xor(gb, A);
  CYC(0x77e7, 0x77ea); mem_wr(gb, wActiveMusic, A);
  CYC(0x77ea, 0x77ec); A = 0x2d;
  CYC(0x77ec, 0x77ef); playSound_b00_hook(gb);
}
