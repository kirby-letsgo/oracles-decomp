#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void pirateCaptain_warpOut_hook(GB *gb) {
  BASE(pirateCaptain_warpOut);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(b_+14);
  CALL_C(b_+3, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto doWarp; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+19);
doWarp:
  CYC(b_+11, b_+14); TAIL(setWarpDestVariables);
}
