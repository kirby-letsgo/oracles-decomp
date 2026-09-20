#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(zelda_warpOutOfVireMinigame), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(zelda_warpOutOfVireMinigame), (from), (to), true)

void zelda_warpOutOfVireMinigame_hook(GB *gb) {
  BASE(zelda_warpOutOfVireMinigame);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0xf0;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+9, b_+12); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+12, b_+14); A = 0x3c;
  CALL_C(b_+14, setGlobalFlag_hook, SYM(setGlobalFlag), b_+17);
  CYC(b_+17, b_+20); SET_HL(b_+23);
  CYC(b_+20, b_+23); setWarpDestVariables_hook(gb);
}

void zelda_giveBlueJoyRing_hook(GB *gb) {
  BASE(zelda_giveBlueJoyRing);
  CYC(b_+0, b_+3); SET_BC((SYM(objectMarkSolidPosition) + 16));
  CYC(b_+3, SYM(zeldaSubid01Script_body)); giveRingToLink_hook(gb);
}
