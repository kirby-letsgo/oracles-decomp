#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(troy_chooseRandomAnimalText), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(troy_chooseRandomAnimalText), (from), (to), true)

void troy_chooseRandomAnimalText_hook(GB *gb) {
  BASE(troy_chooseRandomAnimalText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_add(gb, 0x13);
  CYC(b_+7, b_+10); mem_wr(gb, wTextSubstitutions, A);
  CYC(b_+10, SYM(troySubid0Script_b15)); ret_effect(gb);
}
