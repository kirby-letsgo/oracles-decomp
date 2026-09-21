#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode56_hook(GB *gb);

// ==================================================================================================
// ENEMY_KING_MOBLIN_MINION
// ==================================================================================================
void enemyCode56_hook(GB *gb) {
  BASE(enemyCode56);
  CYC(b_+0, b_+3); SET_HL((SYM(bari_subid0_state8__dontShockYet) + 5)); // bank10.enemyCode56_body
  CYC(b_+3, b_+5); E = 0x10;
  CYC(b_+5, b_+8); TAIL(interBankCall); // jp
}
