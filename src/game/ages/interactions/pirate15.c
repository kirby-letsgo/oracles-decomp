#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void pirate_openEyeballCave_hook(GB *gb) {
  BASE(pirate_openEyeballCave);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x54;
  CYC(b_+2, b_+4); A = 0xa2;
  CALL_C(b_+4, setTile_hook, SYM(setTile), b_+7);
  CYC(b_+7, b_+8); C = alu_inc8(gb, C);
  CYC(b_+8, b_+10); A = 0xef;
  CALL_C(b_+10, setTile_hook, SYM(setTile), b_+13);
  CYC(b_+13, b_+14); C = alu_inc8(gb, C);
  CYC(b_+14, b_+16); A = 0xa4;
  CALL_C(b_+16, setTile_hook, SYM(setTile), b_+19);
  CYC(b_+19, b_+21); A = 0x70;
  CALL_C(b_+21, playSound_b00_hook, SYM(playSound_b00), b_+24);
  CYC(b_+24, b_+27); SET_BC(0x0500);
  CYC(b_+27, b_+30); TAIL(objectCreateInteraction);
}
