#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_VERAN_GHOST), bank 0x15.

void ghostVeranApplySpeedUntilVar38Zero_hook(GB *gb) {
  BASE(ghostVeranApplySpeedUntilVar38Zero);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, objectApplySpeed_hook, SYM(objectApplySpeed), b_+8);
  CYC(b_+8, b_+11); TAIL(objectApplySpeed); // jp
}
