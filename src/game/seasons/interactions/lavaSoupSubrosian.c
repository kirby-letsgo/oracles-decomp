#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/lavaSoupSubrosian.s.
// INTERAC_LAVA_SOUP_SUBROSIAN

static uint16_t lavaSoupSubrosian_jump_table(GB *gb) {
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

// INTERAC_LAVA_SOUP_SUBROSIAN
void s_interactionCode5c_hook(GB *gb) {
  BASE(interactionCode5c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (lavaSoupSubrosian_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+29) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, s_interactionIncState, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x04);
  CYC(b_+18, b_+20); A = 0x0b;
  CALL_C(b_+20, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+23);
  CYC(b_+23, b_+26); SET_HL(SYM(lavaSoupSubrosianScript));
  CALL_C(b_+26, s_interactionSetScript, SYM(interactionSetScript), b_+29);
state1:
  CALL_C(b_+29, s_interactionRunScript, SYM(interactionRunScript), b_+32);
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+36, b_+39); TAIL(npcFaceLinkAndAnimate); }
  CYC(b_+36, b_+39);
  CALL_C(b_+39, s_interactionAnimate, SYM(interactionAnimate), b_+42);
  CYC(b_+42, b_+45);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}
