#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/shipPiratian.s.
// INTERAC_SHIP_PIRATIAN

static uint16_t shipPiratian_jump_table(GB *gb) {
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

static void shipPiratian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SHIP_PIRATIAN
void s_interactionCodeb1_hook(GB *gb) {
  BASE(interactionCodeb1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (shipPiratian_jump_table(gb));
    if (jt_ == SYM(piratian_state0) && hook_is(gb, SYM(piratian_state0), s_piratian_state0_hook)) { s_piratian_state0_hook(gb); return; }
    if (jt_ == SYM(piratian_state1) && hook_is(gb, SYM(piratian_state1), s_piratian_state1_hook)) { s_piratian_state1_hook(gb); return; }
    if (jt_ == SYM(piratian_state2) && hook_is(gb, SYM(piratian_state2), s_piratian_state2_hook)) { s_piratian_state2_hook(gb); return; }
    if (jt_ == SYM(piratian_state3) && hook_is(gb, SYM(piratian_state3), s_piratian_state3_hook)) { s_piratian_state3_hook(gb); return; }
    if (jt_ == SYM(piratian_state4) && hook_is(gb, SYM(piratian_state4), s_piratian_state4_hook)) { s_piratian_state4_hook(gb); return; }
    if (jt_ == SYM(piratian_state5) && hook_is(gb, SYM(piratian_state5), s_piratian_state5_hook)) { s_piratian_state5_hook(gb); return; }
    if (jt_ == SYM(piratian_state6) && hook_is(gb, SYM(piratian_state6), s_piratian_state6_hook)) { s_piratian_state6_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// INTERAC_SHIP_PIRATIAN_CAPTAIN
void s_interactionCodeb2_hook(GB *gb) {
  BASE(interactionCodeb2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (shipPiratian_jump_table(gb));
    if (jt_ == SYM(piratianCaptain_state0) && hook_is(gb, SYM(piratianCaptain_state0), s_piratianCaptain_state0_hook)) { s_piratianCaptain_state0_hook(gb); return; }
    if (jt_ == SYM(piratian_state2) && hook_is(gb, SYM(piratian_state2), s_piratian_state2_hook)) { s_piratian_state2_hook(gb); return; }
    if (jt_ == SYM(piratian_state1) && hook_is(gb, SYM(piratian_state1), s_piratian_state1_hook)) { s_piratian_state1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_piratian_state0_hook(GB *gb) {
  BASE(piratian_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xff);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+21); alu_cp(gb, 0x18);
  CYC(b_+21, b_+23); A = 0x4e;
  if (F & FC) { CYCT(b_+23, b_+25); goto L_6ea3; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); A = 0x4d;
L_6ea3:
  CALL_C(b_+27, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+30);
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+34); SET_HL(SYM(table_6f4b));
  CYC(b_+34, b_+35); shipPiratian_add_double_index(gb, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); H = mem_rd(gb, HL);
  CYC(b_+37, b_+38); L = A;
  CALL_C(b_+38, s_interactionSetScript, SYM(interactionSetScript), b_+41);
  CALL_C(b_+41, s_objectSetVisiblec2, SYM(objectSetVisiblec2), b_+44);
  CALL_C(b_+44, s_interactionRunScript, SYM(interactionRunScript), b_+47);
  CALL_C(b_+47, s_interactionRunScript, SYM(interactionRunScript), b_+50);
  if (F & FC) { CYCT(b_+50, b_+53); TAIL(interactionDelete); }
  CYC(b_+50, b_+53);
  RET(b_+53); return;
}

void s_piratianCaptain_state0_hook(GB *gb) {
  BASE(piratianCaptain_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(SYM(table_6f81));
  CYC(b_+12, b_+13); shipPiratian_add_double_index(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CALL_C(b_+16, s_interactionSetScript, SYM(interactionSetScript), b_+19);
  CALL_C(b_+19, s_objectSetVisiblec2, SYM(objectSetVisiblec2), b_+22);
  CYC(b_+22, b_+24); A = 0x4e;
  CALL_C(b_+24, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+27);
  CALL_C(b_+27, s_interactionRunScript, SYM(interactionRunScript), b_+30);
  CYC(b_+30, b_+33);
  TAIL(interactionRunScript);
}

void s_piratian_state1_hook(GB *gb) {
  BASE(piratian_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, s_interactionRunScript, SYM(interactionRunScript), b_+8);
  if (F & FC) { CYCT(b_+8, b_+11); TAIL(interactionDelete); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL_SG(npcFaceLinkAndAnimate);
}

void s_piratian_state2_hook(GB *gb) {
  BASE(piratian_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, s_interactionRunScript, SYM(interactionRunScript), b_+8);
  if (F & FC) { CYCT(b_+8, b_+11); TAIL(interactionDelete); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL(interactionAnimate);
}

void s_piratian_state3_hook(GB *gb) {
  BASE(piratian_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x10;
  CALL_C(b_+2, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+5);
  CALL_C(b_+5, s_interactionRunScript, SYM(interactionRunScript), b_+8);
  if (F & FC) { CYCT(b_+8, b_+11); TAIL(interactionDelete); }
  CYC(b_+8, b_+11);
  RET(b_+11); return;
}

void s_piratian_state4_hook(GB *gb) {
  BASE(piratian_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, s_interactionAnimate, SYM(interactionAnimate), b_+8);
  CALL_C(b_+8, s_interactionRunScript, SYM(interactionRunScript), b_+11);
  if (F & FC) { CYCT(b_+11, b_+14); TAIL(interactionDelete); }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); A = mem_rd(gb, wFrameCounter);
  CYC(b_+17, b_+19); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); A = alu_inc8(gb, A);
  CYC(b_+24, b_+26); alu_and(gb, 0x03);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30);
  TAIL(interactionSetAnimation);
}

void s_piratian_state5_hook(GB *gb) {
  BASE(piratian_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+3);
  CALL_C(b_+3, s_interactionAnimate, SYM(interactionAnimate), b_+6);
  CALL_C(b_+6, s_interactionRunScript, SYM(interactionRunScript), b_+9);
  if (F & FC) { CYCT(b_+9, b_+12); TAIL(interactionDelete); }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); A = mem_rd(gb, wFrameCounter);
  CYC(b_+15, b_+16); alu_rrca(gb);
  if (F & FC) { CYCT(b_+16, b_+19); TAIL(objectSetInvisible); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+22);
  TAIL(objectSetVisible);
}

void s_piratian_state6_hook(GB *gb) {
  BASE(piratian_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); TAIL(interactionDelete); }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, s_interactionRunScript, SYM(interactionRunScript), b_+10);
  if (F & FC) { CYCT(b_+10, b_+13); TAIL(interactionDelete); }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16);
  TAIL(objectSetInvisible);
}

