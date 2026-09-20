#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(ghiniHarassingMoosh_beginCircularMovement), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(ghiniHarassingMoosh_beginCircularMovement), (from), (to), true)

void ghiniHarassingMoosh_beginCircularMovement_hook(GB *gb) {
  BASE(ghiniHarassingMoosh_beginCircularMovement);
  CYC(b_+0, b_+2); E = 0x50;
  CYC(b_+2, b_+4); A = 0x32;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = 0x49;
  CYC(b_+7, b_+9); A = 0x18;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, SYM(tokayShopItem_giveFeatherAndLoseShovel)); ret_effect(gb);
}
