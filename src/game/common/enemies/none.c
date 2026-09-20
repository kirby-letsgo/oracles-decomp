#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode00), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode00), (from), (to), true)

// object_code/common/enemies/none.s (ENEMY_00), bank $10.

void enemyCode00_hook(GB *gb) {
  BASE(enemyCode00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;  // ret
}
