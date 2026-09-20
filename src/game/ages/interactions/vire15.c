#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void vire_activateMusic_hook(GB *gb) {
  BASE(vire_activateMusic);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wActiveMusic, A);
  CYC(b_+4, b_+6); A = 0x2d;
  CYC(b_+6, b_+9); playSound_b00_hook(gb);
}
