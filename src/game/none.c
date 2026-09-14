#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/common/enemies/none.s (ENEMY_00), bank $10.

void enemyCode00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(0x5826); return;  // ret
}
