#include "game/game.h"
#include "game/gen.h"

#undef CYC
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)

void interactionCode4b_hook(GB *gb) {
  CYC(0x5fe8, 0x5feb); SET_HL(0x7817);
  CYC(0x5feb, 0x5fed); E = 0x3f;
  CYC(0x5fed, 0x5ff0); interBankCall_hook(gb);
}
