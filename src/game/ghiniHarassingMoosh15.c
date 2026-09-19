#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void ghiniHarassingMoosh_beginCircularMovement_hook(GB *gb) {
  CYC(0x6f32, 0x6f34); E = 0x50;
  CYC(0x6f34, 0x6f36); A = 0x32;
  CYC(0x6f36, 0x6f37); mem_wr(gb, DE, A);
  CYC(0x6f37, 0x6f39); E = 0x49;
  CYC(0x6f39, 0x6f3b); A = 0x18;
  CYC(0x6f3b, 0x6f3c); mem_wr(gb, DE, A);
  CYC(0x6f3c, 0x6f3d); ret_effect(gb);
}
