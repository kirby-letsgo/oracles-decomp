#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void pirate_openEyeballCave_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7972, 0x7974); C = 0x54;
  CYC(0x7974, 0x7976); A = 0xa2;
  CALL_C(0x7976, setTile_hook, 0x3a9c, 0x7979);
  CYC(0x7979, 0x797a); C = alu_inc8(gb, C);
  CYC(0x797a, 0x797c); A = 0xef;
  CALL_C(0x797c, setTile_hook, 0x3a9c, 0x797f);
  CYC(0x797f, 0x7980); C = alu_inc8(gb, C);
  CYC(0x7980, 0x7982); A = 0xa4;
  CALL_C(0x7982, setTile_hook, 0x3a9c, 0x7985);
  CYC(0x7985, 0x7987); A = 0x70;
  CALL_C(0x7987, playSound_b00_hook, 0x0c98, 0x798a);
  CYC(0x798a, 0x798d); SET_BC(0x0500);
  CYC(0x798d, 0x7990); objectCreateInteraction_hook(gb);
}
