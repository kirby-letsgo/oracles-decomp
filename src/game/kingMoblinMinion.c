#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode56_hook(GB *gb);

// ==================================================================================================
// ENEMY_KING_MOBLIN_MINION
// ==================================================================================================
void enemyCode56_hook(GB *gb) {
  CYC(0x76f2, 0x76f5); SET_HL(0x6c8e); // bank10.enemyCode56_body
  CYC(0x76f5, 0x76f7); E = 0x10;
  CYC(0x76f7, 0x76fa); interBankCall_hook(gb); return; // jp
}
