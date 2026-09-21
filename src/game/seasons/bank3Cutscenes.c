#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank3Cutscenes.s, the Seasons-only pieces. The shared states are in
// src/game/bank3Cutscenes.c.

// Seasons splits Ages' introCinematic_inTemple_state1 in two: state 1 only waits for the
// palette fade, this state walks Link up to the triforce on simulated input.
void s_introCinematic_inTemple_state1_5_hook(GB *gb) {
  BASE(introCinematic_inTemple_state1_5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wUseSimulatedInput);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+7); TAIL(introCinematic_inTemple_updateCamera); }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(b_+11, introCinematic_inTemple_updateCamera_hook, SYM(introCinematic_inTemple_updateCamera), b_+14);
  CYC(b_+14, b_+17);
  TAIL(intro_incState);
}
