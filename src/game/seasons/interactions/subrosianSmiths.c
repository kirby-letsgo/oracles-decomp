#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosianSmiths.s.
// INTERAC_SUBROSIAN_SMITHS

// INTERAC_SUBROSIAN_SMITHS
void s_interactionCode34_hook(GB *gb) {
  BASE(interactionCode34);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_6384; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
L_6384:
  CALL_C(b_+11, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_cp(gb, 0xff);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); A = 0x50;
  CYC(b_+22, b_+25);
  TAIL(playSound_b00);
}

