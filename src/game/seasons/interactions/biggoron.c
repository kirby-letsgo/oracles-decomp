#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/biggoron.s.
// INTERAC_BIGGORON

static uint16_t biggoron_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// INTERAC_BIGGORON
void s_interactionCode52_hook(GB *gb) {
  BASE(interactionCode52);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (biggoron_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+31) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+17);
  CALL_C(b_+17, s_objectSetVisible82, SYM(objectSetVisible82), b_+20);
  CYC(b_+20, b_+22); A = 0x0b;
  CALL_C(b_+22, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+25);
  CYC(b_+25, b_+28); SET_HL(SYM(biggoronScript));
  CYC(b_+28, b_+31);
  TAIL(interactionSetScript);
state1:
  CALL_C(b_+31, s_interactionAnimate, SYM(interactionAnimate), b_+34);
  CYC(b_+34, b_+37);
  TAIL(interactionRunScript);
}
