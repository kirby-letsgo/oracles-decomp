#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/jewel.s.
// INTERAC_JEWEL

static void jewel_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// INTERAC_JEWEL
void s_interactionCode92_hook(GB *gb) {
  BASE(interactionCode92);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(b_+28 /* @xPositions */);
  CYC(b_+12, b_+13); jewel_add_a_to_hl(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x2c);
  CALL_C(b_+22, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+25);
  CYC(b_+25, b_+28);
  TAIL(objectSetVisible83);
}

