#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void zelda_warpOutOfVireMinigame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7739, 0x773b); A = 0xf0;
  CALL_C(0x773b, playSound_b00_hook, 0x0c98, 0x773e);
  CYC(0x773e, 0x773f); alu_xor(gb, A);
  CYC(0x773f, 0x7742); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7742, 0x7745); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7745, 0x7747); A = 0x3c;
  CALL_C(0x7747, setGlobalFlag_hook, 0x31f9, 0x774a);
  CYC(0x774a, 0x774d); SET_HL(0x7750);
  CYC(0x774d, 0x7750); setWarpDestVariables_hook(gb);
}

void zelda_giveBlueJoyRing_hook(GB *gb) {
  CYC(0x7755, 0x7758); SET_BC(0x2500);
  CYC(0x7758, 0x775b); giveRingToLink_hook(gb);
}
