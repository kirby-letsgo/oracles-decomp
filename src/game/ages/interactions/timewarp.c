#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/ages/interactions/timewarp.s (INTERAC_TIMEWARP), bank $10.

static uint16_t timewarp_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void timewarp_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void timewarp_animate_hook(GB *gb) {
  BASE(timewarp_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  RET(b_+7); return;
}

void timewarp_common_state0_hook(GB *gb) {
  BASE(timewarp_common_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, interactionIncState_hook, SYM(interactionIncState), b_+6);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+8, b_+10); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+10, b_+12); alu_add(gb, 0x08);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+20);
  TAIL(objectSetVisible83);
}

void timewarp_spawnChild_hook(GB *gb) {
  BASE(timewarp_spawnChild);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xdd);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); E = L;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+14, b_+16); A = 0x40;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); E = alu_inc8(gb, E);
  CYC(b_+18, b_+19); A = H;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); SET_BC(0xf800);
  CYC(b_+23, b_+26);
  TAIL(objectCopyPositionWithOffset);
}

void timewarp_animateUntilFinished_hook(GB *gb) {
  BASE(timewarp_animateUntilFinished);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, timewarp_animate_hook, SYM(timewarp_animate), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7);
  TAIL(interactionDelete);
}

void timewarp_subid0_state1_hook(GB *gb) {
  BASE(timewarp_subid0_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, timewarp_animate_hook, SYM(timewarp_animate), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+6);
    TAIL(interactionIncState);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    goto spawnChild;
  }
  CYC(b_+7, b_+9);
  RET(b_+9); return;
spawnChild:
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); B = 0x03;
  TAIL(timewarp_spawnChild);
}

void timewarp_subid0_state2_hook(GB *gb) {
  BASE(timewarp_subid0_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto counterReached0;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x24);
  if (F & FC) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_and(gb, 0x38);
  CYC(b_+15, b_+16); alu_rrca(gb);
  CYC(b_+16, b_+19); SET_HL(b_+63);
  CYC(b_+19, b_+20);
  timewarp_addAToHl_from_rst(gb, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+25); E = mem_rd(gb, HL);
  CALL_C(b_+25, getFreePartSlot_hook, SYM(getFreePartSlot), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x2b);
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+33); mem_wr(gb, HL, E);
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_RELATED2 + 1;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x40);
  CYC(b_+41, b_+43); L = PART_BASE + OBJ_SPEED;
  CYC(b_+43, b_+44); mem_wr(gb, HL, B);
  CYC(b_+44, b_+46); B = 0x00;
  CYC(b_+46, b_+49);
  TAIL(objectCopyPositionWithOffset);
counterReached0:
  CYC(b_+49, b_+51); A = 0x01;
  CALL_C(b_+51, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+54);
  CYC(b_+54, b_+56); A = 0x04;
  CALL_C(b_+56, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+59);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+63);
  TAIL(interactionIncState);
}

void timewarp_subid1_state1_hook(GB *gb) {
  BASE(timewarp_subid1_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, timewarp_animate_hook, SYM(timewarp_animate), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto afterAnimate;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); B = 0x04;
  CYC(b_+11, b_+14);
  TAIL(timewarp_spawnChild);
afterAnimate:
  CYC(b_+14, b_+16); A = 0x04;
  CALL_C(b_+16, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+20, interactionIncState_hook, SYM(interactionIncState), b_+23);
  CYC(b_+23, b_+25); A = 0x01;
  CYC(b_+25, b_+28);
  TAIL(interactionSetAnimation);
}

void itemwarp_subid3Or4_state0_hook(GB *gb) {
  BASE(itemwarp_subid3Or4_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0xc0);
  CALL_C(b_+5, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+8);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, interactionIncState_hook, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+17);
  TAIL(objectSetVisible82);
}

void timewarp_subid3_state1_hook(GB *gb) {
  BASE(timewarp_subid3_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, timewarp_animate_hook, SYM(timewarp_animate), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x03;
  CALL_C(b_+6, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+9);
  CYC(b_+9, b_+12);
  TAIL(interactionIncState);
}

void timewarp_subid3Or4_state3_hook(GB *gb) {
  BASE(timewarp_subid3Or4_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionIncState_hook, SYM(interactionIncState), b_+3);
  CYC(b_+3, b_+5); A = 0x04;
  CYC(b_+5, b_+8);
  TAIL(interactionSetAnimation);
}

void timewarp_subid3Or4_state4_hook(GB *gb) {
  BASE(timewarp_subid3Or4_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, timewarp_animate_hook, SYM(timewarp_animate), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7);
  TAIL(interactionDelete);
}

void timewarp_subid0_hook(GB *gb) {
  BASE(timewarp_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timewarp_jump_table(gb));
    if (jt_ == SYM(timewarp_common_state0) && hook_enabled_at(gb, SYM(timewarp_common_state0))) { timewarp_common_state0_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid0_state1) && hook_enabled_at(gb, SYM(timewarp_subid0_state1))) { timewarp_subid0_state1_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid0_state2) && hook_enabled_at(gb, SYM(timewarp_subid0_state2))) { timewarp_subid0_state2_hook(gb); return; }
    else if (jt_ == SYM(timewarp_animateUntilFinished) && hook_enabled_at(gb, SYM(timewarp_animateUntilFinished))) { timewarp_animateUntilFinished_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void timewarp_subid1_hook(GB *gb) {
  BASE(timewarp_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timewarp_jump_table(gb));
    if (jt_ == SYM(timewarp_common_state0) && hook_enabled_at(gb, SYM(timewarp_common_state0))) { timewarp_common_state0_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid1_state1) && hook_enabled_at(gb, SYM(timewarp_subid1_state1))) { timewarp_subid1_state1_hook(gb); return; }
    else if (jt_ == SYM(timewarp_animateUntilFinished) && hook_enabled_at(gb, SYM(timewarp_animateUntilFinished))) { timewarp_animateUntilFinished_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void timewarp_subid2_hook(GB *gb) {
  BASE(timewarp_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timewarp_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+27) { goto state1; }
    else if (jt_ == b_+34) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13);
  CALL_C(b_+13, interactionIncState_hook, SYM(interactionIncState), b_+16);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_SPEED_TMP;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0xfc);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x06);
  CYC(b_+24, b_+27);
  TAIL(objectSetVisible81);
state1:
  CALL_C(b_+27, timewarp_animate_hook, SYM(timewarp_animate), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34);
  TAIL(interactionIncState);
state2:
  CALL_C(b_+34, objectApplyComponentSpeed_hook, SYM(objectApplyComponentSpeed), b_+37);
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(b_+42, b_+45);
    TAIL(interactionDelete);
  }
  CYC(b_+42, b_+45);
  CALL_C(b_+45, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+48);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x06);
  CYC(b_+51, b_+54); SET_BC(0x8401);
  CALL_C(b_+54, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+57);
  if (!(F & FZ)) { RET_TAKEN(b_+57); return; }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+60, b_+61); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+61); return;
}

void timewarp_subid3_hook(GB *gb) {
  BASE(timewarp_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timewarp_jump_table(gb));
    if (jt_ == SYM(itemwarp_subid3Or4_state0) && hook_enabled_at(gb, SYM(itemwarp_subid3Or4_state0))) { itemwarp_subid3Or4_state0_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid3_state1) && hook_enabled_at(gb, SYM(timewarp_subid3_state1))) { timewarp_subid3_state1_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid3Or4_state3) && hook_enabled_at(gb, SYM(timewarp_subid3Or4_state3))) { timewarp_subid3Or4_state3_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid3Or4_state4) && hook_enabled_at(gb, SYM(timewarp_subid3Or4_state4))) { timewarp_subid3Or4_state4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void timewarp_subid4_hook(GB *gb) {
  BASE(timewarp_subid4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timewarp_jump_table(gb));
    if (jt_ == SYM(itemwarp_subid3Or4_state0) && hook_enabled_at(gb, SYM(itemwarp_subid3Or4_state0))) { itemwarp_subid3Or4_state0_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid3Or4_state3) && hook_enabled_at(gb, SYM(timewarp_subid3Or4_state3))) { timewarp_subid3Or4_state3_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid3Or4_state4) && hook_enabled_at(gb, SYM(timewarp_subid3Or4_state4))) { timewarp_subid3Or4_state4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void interactionCodedd_hook(GB *gb) {
  BASE(interactionCodedd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timewarp_jump_table(gb));
    if (jt_ == SYM(timewarp_subid0) && hook_enabled_at(gb, SYM(timewarp_subid0))) { timewarp_subid0_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid1) && hook_enabled_at(gb, SYM(timewarp_subid1))) { timewarp_subid1_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid2) && hook_enabled_at(gb, SYM(timewarp_subid2))) { timewarp_subid2_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid3) && hook_enabled_at(gb, SYM(timewarp_subid3))) { timewarp_subid3_hook(gb); return; }
    else if (jt_ == SYM(timewarp_subid4) && hook_enabled_at(gb, SYM(timewarp_subid4))) { timewarp_subid4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
