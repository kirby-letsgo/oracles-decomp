#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/boomerangSubrosian.s.
// INTERAC_BOOMERANG_SUBROSIAN

static uint16_t boomerangSubrosian_jump_table(GB *gb) {
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

// interactionCodec8@func_78cc
static void boomerangSubrosian_func_78cc(GB *gb) {
  BASE(interactionCodec8);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x01);
  CYC(b_+76, b_+79);
  TAIL(interactionAnimate);
}

// INTERAC_BOOMERANG_SUBROSIAN
void s_interactionCodec8_hook(GB *gb) {
  BASE(interactionCodec8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boomerangSubrosian_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+33) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x28;
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+19);
  CALL_C(b_+19, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x00);
  CYC(b_+24, b_+27); SET_HL(SYM(boomerangSubrosianScript));
  CALL_C(b_+27, s_interactionSetScript, SYM(interactionSetScript), b_+30);
  CALL_L(b_+30, boomerangSubrosian_func_78cc, b_+33);
state1:
  CALL_C(b_+33, s_interactionRunScript, SYM(interactionRunScript), b_+36);
  CALL_C(b_+36, s_interactionPushLinkAwayAndUpdateDrawPriority, SYM(interactionPushLinkAwayAndUpdateDrawPriority), b_+39);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); push_effect(gb, b_+43);
  do { uint16_t jt_ = (boomerangSubrosian_jump_table(gb));
    if (jt_ == b_+47) goto substate0;
    if (jt_ == b_+79) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CALL_C(b_+52, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+55);
  CYC(b_+55, b_+57); B = 0xc9;
  CALL_C(b_+57, s_objectCreateInteractionWithSubid00, SYM(objectCreateInteractionWithSubid00), b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto L_78c7; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+64, b_+65); mem_wr(gb, HL, E);
  CYC(b_+65, b_+66); L = alu_inc8(gb, L);
  CYC(b_+66, b_+67); mem_wr(gb, HL, D);
L_78c7:
  CYC(b_+67, b_+68); H = D;
  CYC(b_+68, b_+70); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x01);
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x01);
  CYC(b_+76, b_+79);
  TAIL(interactionAnimate);
substate1:
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+83); return; }
  CYC(b_+83, b_+84);
  CYC(b_+84, b_+85); H = D;
  CYC(b_+85, b_+87); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_L(b_+88, boomerangSubrosian_func_78cc, b_+91);
  CALL_C(b_+91, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+94);
  CYC(b_+94, b_+96); alu_and(gb, 0x3f);
  CYC(b_+96, b_+98); alu_add(gb, 0x3c);
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  RET(b_+101); return;
}
