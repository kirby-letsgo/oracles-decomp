#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s, bank 0x15: the linked-game secret NPCs.

// var3f = 1 once the secret for this NPC (global flag $64 + var3e) has begun.
void s_linkedNpc_checkSecretBegun_hook(GB *gb) {
  BASE(linkedNpc_checkSecretBegun);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x64; // GLOBALFLAG_FIRST_SEASONS_BEGAN_SECRET
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0x7e; // Interaction.var3e
  CYC(b_+5, b_+6); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+6, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+9);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = 0x7f; // Interaction.var3f
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x01);
  RET(b_+15); return;
}
