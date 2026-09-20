#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void troy_chooseRandomAnimalText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79a7, getRandomNumber_hook, 0x043e, 0x79aa);
  CYC(0x79aa, 0x79ac); alu_and(gb, 0x0f);
  CYC(0x79ac, 0x79ae); alu_add(gb, 0x13);
  CYC(0x79ae, 0x79b1); mem_wr(gb, wTextSubstitutions, A);
  CYC(0x79b1, 0x79b2); ret_effect(gb);
}
