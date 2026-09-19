#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_VERAN_GHOST), bank 0x15.

void ghostVeranApplySpeedUntilVar38Zero_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x593b, 0x593c); H = D;
  CYC(0x593c, 0x593e); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x593e, 0x593f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { RET_TAKEN(0x593f); return; }
  CYC(0x593f, 0x5940);
  CALL_C(0x5940, objectApplySpeed_hook, 0x201d, 0x5943);
  CYC(0x5943, 0x5946); objectApplySpeed_hook(gb); return; // jp
}
