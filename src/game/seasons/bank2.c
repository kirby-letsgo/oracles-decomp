#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank2.s, the Seasons-only pieces.

// NZ when the tileset has a season to show in the subscreen (wTilesetFlags bits 2..7).
void s_checkWhetherToDisplaySeasonInSubscreen_hook(GB *gb) {
  BASE(checkWhetherToDisplaySeasonInSubscreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0xfc);
  RET(b_+5); return;
}
