#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_POE), bank 0x15.

void poe_decCounterAndFlickerVisibility_hook(GB *gb) {
  BASE(poe_decCounterAndFlickerVisibility);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  CALL_C(b_+5, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto set_visible; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+14); A = mem_rd(gb, wFrameCounter);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+16); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+16, b_+19); objectSetInvisible_hook(gb); return; } // jp nc
  CYC(b_+16, b_+19);

set_visible:
  CYC(b_+19, b_+22); TAIL(objectSetVisible); // jp
}
