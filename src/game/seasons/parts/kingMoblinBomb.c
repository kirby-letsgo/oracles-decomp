#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/kingMoblinBomb.s.
// PART_KING_MOBLIN_BOMB

static uint16_t kingMoblinBomb_jump_table(GB *gb) {
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

// PART_KING_MOBLIN_BOMB
void s_partCode3f_hook(GB *gb) {
  BASE(partCode3f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_SUBID;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto normalStatus; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x95);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto normalStatus; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); H = D;
  CALL_C(b_+16, s_kingMoblinBomb_explode, SYM(kingMoblinBomb_explode), b_+19);
normalStatus:
  CYC(b_+19, b_+21); E = PART_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); E = PART_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); push_effect(gb, b_+25);
  do { uint16_t jt_ = (kingMoblinBomb_jump_table(gb));
    if (jt_ == SYM(kingMoblinBomb_subid0) && hook_is(gb, SYM(kingMoblinBomb_subid0), s_kingMoblinBomb_subid0_hook)) { s_kingMoblinBomb_subid0_hook(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_subid1) && hook_is(gb, SYM(kingMoblinBomb_subid1), s_kingMoblinBomb_subid1_hook)) { s_kingMoblinBomb_subid1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_kingMoblinBomb_subid0_hook(GB *gb) {
  BASE(kingMoblinBomb_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (kingMoblinBomb_jump_table(gb));
    if (jt_ == SYM(kingMoblinBomb_state0) && hook_is(gb, SYM(kingMoblinBomb_state0), s_kingMoblinBomb_state0_hook)) { s_kingMoblinBomb_state0_hook(gb); return; }
    if (jt_ == SYM(seasons_kingMoblinBomb_state1) && hook_is(gb, SYM(seasons_kingMoblinBomb_state1), s_seasons_kingMoblinBomb_state1_hook)) { s_seasons_kingMoblinBomb_state1_hook(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state2) && hook_is(gb, SYM(kingMoblinBomb_state2), s_kingMoblinBomb_state2_hook)) { s_kingMoblinBomb_state2_hook(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state3) && hook_is(gb, SYM(kingMoblinBomb_state3), s_kingMoblinBomb_state3_hook)) { s_kingMoblinBomb_state3_hook(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state4) && hook_is(gb, SYM(kingMoblinBomb_state4), s_kingMoblinBomb_state4)) { s_kingMoblinBomb_state4(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state5) && hook_is(gb, SYM(kingMoblinBomb_state5), s_kingMoblinBomb_state5)) { s_kingMoblinBomb_state5(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state6) && hook_is(gb, SYM(kingMoblinBomb_state6), s_kingMoblinBomb_state6)) { s_kingMoblinBomb_state6(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state7) && hook_is(gb, SYM(kingMoblinBomb_state7), s_kingMoblinBomb_state7)) { s_kingMoblinBomb_state7(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_state8) && hook_is(gb, SYM(kingMoblinBomb_state8), s_kingMoblinBomb_state8)) { s_kingMoblinBomb_state8(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_kingMoblinBomb_state0_hook(GB *gb) {
  BASE(kingMoblinBomb_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10);
  CYC(b_+7, b_+9); L = PART_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x32);
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xfe);
  CYC(b_+18, b_+21);
  TAIL(objectSetVisiblec2);
}

void s_seasons_kingMoblinBomb_state1_hook(GB *gb) {
  BASE(seasons_kingMoblinBomb_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

// Being held by Link
void s_kingMoblinBomb_state2_hook(GB *gb) {
  BASE(kingMoblinBomb_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (kingMoblinBomb_jump_table(gb));
    if (jt_ == b_+11) goto justGrabbed;
    if (jt_ == b_+21) goto beingHeld;
    if (jt_ == b_+28) goto released;
    if (jt_ == b_+59) goto atRest;
    HANDOFF(HL);
  } while (0);
justGrabbed:
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+18, b_+21);
  TAIL(objectSetVisiblec1);
beingHeld:
  CALL_C(b_+21, s_common_kingMoblinBomb_state1_hook, SYM(common_kingMoblinBomb_state1), b_+24);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28);
  TAIL(dropLinkHeldItem);
released:
  CYC(b_+28, b_+30); E = PART_BASE + OBJ_YH;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_cp(gb, 0x30);
  if (!(F & FC)) { CYCT(b_+33, b_+35); goto beingHeld; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+38); L = PART_BASE + OBJ_ZH;
  CYC(b_+38, b_+40); E = PART_BASE + OBJ_SUBID;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto beingHeld; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+47); SET_HL(w1ReservedItemC_speedZ + 0x01);
  CYC(b_+47, b_+49); mem_wr(gb, HL, alu_sra(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+50); L = alu_dec8(gb, L);
  CYC(b_+50, b_+52); mem_wr(gb, HL, alu_rr(gb, mem_rd(gb, HL)));
  CYC(b_+52, b_+54); L = 0x10;
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x0a);
  CYC(b_+56, b_+59);
  TAIL_S(common_kingMoblinBomb_state1);
atRest:
  CYC(b_+59, b_+61); E = PART_BASE + OBJ_STATE;
  CYC(b_+61, b_+63); A = 0x04;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CALL_C(b_+64, s_objectSetVisiblec2, SYM(objectSetVisiblec2), b_+67);
  CYC(b_+67, b_+69);
  TAIL(kingMoblinBomb_state4);
}

// Being thrown. (King moblin sets the state to this.)
void s_kingMoblinBomb_state3_hook(GB *gb) {
  BASE(kingMoblinBomb_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto doneBouncing; }
  CYC(b_+5, b_+7);
  if (F & FZ) CALL_C_CC(b_+7, s_kingMoblinBomb_playSound_hook, SYM(kingMoblinBomb_playSound), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+13);
  TAIL(objectApplySpeed);
doneBouncing:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = PART_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+17, s_kingMoblinBomb_playSound_hook, SYM(kingMoblinBomb_playSound), b_+20);
  s_kingMoblinBomb_state4(gb); return; // falls through
}

void s_kingMoblinBomb_subid1_hook(GB *gb) {
  BASE(kingMoblinBomb_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (kingMoblinBomb_jump_table(gb));
    if (jt_ == SYM(kingMoblinBomb_subid1_state0) && hook_is(gb, SYM(kingMoblinBomb_subid1_state0), s_kingMoblinBomb_subid1_state0_hook)) { s_kingMoblinBomb_subid1_state0_hook(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_subid1_state1) && hook_is(gb, SYM(kingMoblinBomb_subid1_state1), s_kingMoblinBomb_subid1_state1_hook)) { s_kingMoblinBomb_subid1_state1_hook(gb); return; }
    if (jt_ == SYM(kingMoblinBomb_subid1_state2) && hook_is(gb, SYM(kingMoblinBomb_subid1_state2), s_kingMoblinBomb_subid1_state2_hook)) { s_kingMoblinBomb_subid1_state2_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_kingMoblinBomb_subid1_state0_hook(GB *gb) {
  BASE(kingMoblinBomb_subid1_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = PART_BASE + OBJ_SPEED;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x28);
  CYC(b_+7, b_+9); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x20);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0xfe);
  CYC(b_+14, b_+17);
  TAIL(objectSetVisiblec2);
}

void s_kingMoblinBomb_subid1_state1_hook(GB *gb) {
  BASE(kingMoblinBomb_subid1_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto doneBouncing; }
  CYC(b_+5, b_+7);
  if (F & FZ) CALL_C_CC(b_+7, s_kingMoblinBomb_playSound_hook, SYM(kingMoblinBomb_playSound), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+13);
  TAIL(objectApplySpeed);
doneBouncing:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = PART_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  s_kingMoblinBomb_playSound_hook(gb); return; // falls through
}

void s_kingMoblinBomb_playSound_hook(GB *gb) {
  BASE(kingMoblinBomb_playSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x52;
  CYC(b_+2, b_+5);
  TAIL(playSound_b00);
}

void s_kingMoblinBomb_subid1_state2_hook(GB *gb) {
  BASE(kingMoblinBomb_subid1_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(partAnimate); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);
  CYC(b_+10, b_+12); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+16, b_+19); TAIL(partAnimate); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+24);
  TAIL(kingMoblinBomb_explode);
}

void s_common_kingMoblinBomb_state1_hook(GB *gb) {
  BASE(common_kingMoblinBomb_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto animate; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+15, b_+17); TAIL(kingMoblinBomb_explode); }
  CYC(b_+15, b_+17);
animate:
  CYC(b_+17, b_+20);
  TAIL(partAnimate);
}

