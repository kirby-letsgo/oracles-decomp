#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DUNGEON_WISE_OLD_MAN), bank 0x15.

void s_dungeonWiseOldMan_setLinksInvincibilityCounterTo0_hook(GB *gb) {
  BASE(dungeonWiseOldMan_setLinksInvincibilityCounterTo0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, w1Link_invincibilityCounter, A);
  RET(b_+4); return;
}
