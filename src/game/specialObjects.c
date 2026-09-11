#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

void specialObjectCode_minecart_b05_hook(GB *gb) {
  CYC(0x6364, 0x6367); SET_HL(0x563e);
  CYC(0x6367, 0x6369); E = 0x06;
  CYC(0x6369, 0x636c);
  interBankCall_hook(gb);
}

void specialObjectCode_raft_b05_hook(GB *gb) {
  CYC(0x7c66, 0x7c69); SET_HL(0x57ef);
  CYC(0x7c69, 0x7c6b); E = 0x06;
  CYC(0x7c6b, 0x7c6e);
  interBankCall_hook(gb);
}
