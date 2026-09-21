#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_PIRATIAN), bank 0x15.

// Applies the jump with gravity $30; sets var3d once the piratian has landed.
void s_piratian_waitUntilJumpDone_hook(GB *gb) {
  BASE(piratian_waitUntilJumpDone);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x7d; // Interaction.var3d
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x01);
  RET(b_+11); return;
}
