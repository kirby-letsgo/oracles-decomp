#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/scripting.s.

void s_scriptCmd_jump_hook(GB *gb) {
  BASE(scriptCmd_jump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); L = mem_rd(gb, HL);
  CYC(b_+2, b_+3); H = A;
  CYC(b_+3, b_+4); alu_scf(gb);
  RET(b_+4); return;
}

