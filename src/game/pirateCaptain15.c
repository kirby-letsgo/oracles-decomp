#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void pirateCaptain_warpOut_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7930, 0x7933); SET_HL(0x793e);
  CALL_C(0x7933, checkIsLinkedGame_hook, 0x1992, 0x7936);
  if (F & FZ) { CYCT(0x7936, 0x7938); goto doWarp; }
  CYC(0x7936, 0x7938);
  CYC(0x7938, 0x793b); SET_HL(0x7943);
doWarp:
  CYC(0x793b, 0x793e); setWarpDestVariables_hook(gb);
}
