#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/loneZora.s.
// INTERAC_LONE_ZORA

static uint16_t loneZora_jump_table(GB *gb) {
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

// INTERAC_LONE_ZORA
void s_interactionCodee7_hook(GB *gb) {
  BASE(interactionCodee7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (loneZora_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+35) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); A = 0x36;
  CALL_C(b_+16, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+21, b_+23); A = 0x00;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+27); SET_HL(SYM(linkedGameNpcScript));
  CALL_C(b_+27, s_interactionSetScript, SYM(interactionSetScript), b_+30);
  CYC(b_+30, b_+32); A = 0x02;
  CALL_C(b_+32, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+35);
state1:
  CALL_C(b_+35, s_interactionRunScript, SYM(interactionRunScript), b_+38);
  CYC(b_+38, b_+41);
  TAIL(npcFaceLinkAndAnimate);
}

