#include "game/game.h"
#include "game/gen.h"

#undef CYC
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)

void interactionCode4b_hook(GB *gb) {
  BASE(interactionCode4b);
  CYC(b_+0, b_+3); SET_HL((SYM(goronSubid04__state1) + 5));
  CYC(b_+3, b_+5); E = 0x3f;
  CYC(b_+5, b_+8); TAIL(interBankCall);
}
