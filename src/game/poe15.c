#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_POE), bank 0x15.

void poe_decCounterAndFlickerVisibility_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6131, 0x6132); H = D;
  CYC(0x6132, 0x6134); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6134, 0x6135); A = mem_rd(gb, HL);
  CYC(0x6135, 0x6136); alu_or(gb, A);
  CALL_C(0x6136, writeFlagsTocddb_hook, 0x5118, 0x6139);
  if (F & FZ) { CYCT(0x6139, 0x613b); goto set_visible; } // jr z
  CYC(0x6139, 0x613b);
  CYC(0x613b, 0x613c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x613c, 0x613f); A = mem_rd(gb, wFrameCounter);
  CYC(0x613f, 0x6140); alu_rrca(gb);
  CYC(0x6140, 0x6141); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x6141, 0x6144); objectSetInvisible_hook(gb); return; } // jp nc
  CYC(0x6141, 0x6144);

set_visible:
  CYC(0x6144, 0x6147); objectSetVisible_hook(gb); return; // jp
}
