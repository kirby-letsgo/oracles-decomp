#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SUBROSIAN_AT_VOLCANO), bank 0x15.

// Refreshes the object gfx (a thread switch), then spawns the volcano items object.
void s_subrosianAtD8_spawnitem_hook(GB *gb) {
  BASE(subrosianAtD8_spawnitem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+3);
  CYC(b_+3, b_+5); A = mem_rd(gb, hActiveObject);
  CYC(b_+5, b_+6); D = A;
  CYC(b_+6, b_+8); B = 0x54; // INTERAC_SUBROSIAN_AT_VOLCANO_ITEMS
  CYC(b_+8, b_+11);
  TAIL(objectCreateInteractionWithSubid00);
}
