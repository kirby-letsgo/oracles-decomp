#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b60, interactionAnimate_hook, 0x261b, 0x7b63);
  CYC(0x7b63, 0x7b65); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7b65, 0x7b66); A = mem_rd(gb, DE);
  CYC(0x7b66, 0x7b67); A = alu_inc8(gb, A);
  RET(0x7b67); return;
}

void timewarp_common_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a14, interactionInitGraphics_hook, 0x15fb, 0x7a17);
  CALL_C(0x7a17, interactionIncState_hook, 0x23e0, 0x7a1a);
  CYC(0x7a1a, 0x7a1c); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7a1c, 0x7a1e); A = mem_rd(gb, 0xffb0);
  CYC(0x7a1e, 0x7a20); alu_add(gb, 0x08);
  CYC(0x7a20, 0x7a21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7a21, 0x7a22); L = alu_inc8(gb, L);
  CYC(0x7a22, 0x7a24); A = mem_rd(gb, 0xffb1);
  CYC(0x7a24, 0x7a25); mem_wr(gb, HL, A);
  CYC(0x7a25, 0x7a28);
  objectSetVisible83_hook(gb);
}

void timewarp_spawnChild_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a36, getFreeInteractionSlot_hook, 0x3aef, 0x7a39);
  if (!(F & FZ)) { RET_TAKEN(0x7a39); return; }
  CYC(0x7a39, 0x7a3a);
  CYC(0x7a3a, 0x7a3c); mem_wr(gb, HL, 0xdd);
  CYC(0x7a3c, 0x7a3d); L = alu_inc8(gb, L);
  CYC(0x7a3d, 0x7a3e); mem_wr(gb, HL, B);
  CYC(0x7a3e, 0x7a3f); L = alu_inc8(gb, L);
  CYC(0x7a3f, 0x7a40); E = L;
  CYC(0x7a40, 0x7a41); A = mem_rd(gb, DE);
  CYC(0x7a41, 0x7a42); mem_wr(gb, HL, A);
  CYC(0x7a42, 0x7a44); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x7a44, 0x7a46); A = 0x40;
  CYC(0x7a46, 0x7a47); mem_wr(gb, DE, A);
  CYC(0x7a47, 0x7a48); E = alu_inc8(gb, E);
  CYC(0x7a48, 0x7a49); A = H;
  CYC(0x7a49, 0x7a4a); mem_wr(gb, DE, A);
  CYC(0x7a4a, 0x7a4d); SET_BC(0xf800);
  CYC(0x7a4d, 0x7a50);
  objectCopyPositionWithOffset_hook(gb);
}

void timewarp_animateUntilFinished_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7aaf, timewarp_animate_hook, 0x7b60, 0x7ab2);
  if (!(F & FZ)) { RET_TAKEN(0x7ab2); return; }
  CYC(0x7ab2, 0x7ab3);
  CYC(0x7ab3, 0x7ab6);
  interactionDelete_hook(gb);
}

void timewarp_subid0_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a28, timewarp_animate_hook, 0x7b60, 0x7a2b);
  if (F & FZ) {
    CYCT(0x7a2b, 0x7a2e);
    interactionIncState_hook(gb);
    return;
  }
  CYC(0x7a2b, 0x7a2e);
  CYC(0x7a2e, 0x7a2f); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7a2f, 0x7a32);
    goto spawnChild;
  }
  CYC(0x7a2f, 0x7a32);
  RET(0x7a31); return;
spawnChild:
  CYC(0x7a32, 0x7a33); alu_xor(gb, A);
  CYC(0x7a33, 0x7a34); mem_wr(gb, DE, A);
  CYC(0x7a34, 0x7a36); B = 0x03;
  timewarp_spawnChild_hook(gb);
}

void timewarp_subid0_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a50, interactionDecCounter1_hook, 0x23cc, 0x7a53);
  if (F & FZ) {
    CYCT(0x7a53, 0x7a55);
    goto counterReached0;
  }
  CYC(0x7a53, 0x7a55);
  CYC(0x7a55, 0x7a56); A = mem_rd(gb, HL);
  CYC(0x7a56, 0x7a58); alu_cp(gb, 0x24);
  if (F & FC) { RET_TAKEN(0x7a58); return; }
  CYC(0x7a58, 0x7a59);
  CYC(0x7a59, 0x7a5b); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x7a5b); return; }
  CYC(0x7a5b, 0x7a5c);
  CYC(0x7a5c, 0x7a5d); A = mem_rd(gb, HL);
  CYC(0x7a5d, 0x7a5f); alu_and(gb, 0x38);
  CYC(0x7a5f, 0x7a60); alu_rrca(gb);
  CYC(0x7a60, 0x7a63); SET_HL(0x7a8f);
  CYC(0x7a63, 0x7a64);
  timewarp_addAToHl_from_rst(gb, 0x7a64);
  CYC(0x7a64, 0x7a65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7a65, 0x7a66); B = A;
  CYC(0x7a66, 0x7a67); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7a67, 0x7a68); C = A;
  CYC(0x7a68, 0x7a69); E = mem_rd(gb, HL);
  CALL_C(0x7a69, getFreePartSlot_hook, 0x3e8e, 0x7a6c);
  if (!(F & FZ)) { RET_TAKEN(0x7a6c); return; }
  CYC(0x7a6c, 0x7a6d);
  CYC(0x7a6d, 0x7a6f); mem_wr(gb, HL, 0x2b);
  CYC(0x7a6f, 0x7a70); L = alu_inc8(gb, L);
  CYC(0x7a70, 0x7a71); mem_wr(gb, HL, E);
  CYC(0x7a71, 0x7a73); E = INTERACTION_BASE + OBJ_RELATED2 + 1;
  CYC(0x7a73, 0x7a74); A = mem_rd(gb, DE);
  CYC(0x7a74, 0x7a76); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(0x7a76, 0x7a77); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x7a77, 0x7a79); mem_wr(gb, HL, 0x40);
  CYC(0x7a79, 0x7a7b); L = PART_BASE + OBJ_SPEED;
  CYC(0x7a7b, 0x7a7c); mem_wr(gb, HL, B);
  CYC(0x7a7c, 0x7a7e); B = 0x00;
  CYC(0x7a7e, 0x7a81);
  objectCopyPositionWithOffset_hook(gb);
  return;
counterReached0:
  CYC(0x7a81, 0x7a83); A = 0x01;
  CALL_C(0x7a83, interactionSetAnimation_hook, 0x262e, 0x7a86);
  CYC(0x7a86, 0x7a88); A = 0x04;
  CALL_C(0x7a88, objectGetRelatedObject2Var_hook, 0x2164, 0x7a8b);
  CYC(0x7a8b, 0x7a8c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7a8c, 0x7a8f);
  interactionIncState_hook(gb);
}

void timewarp_subid1_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7ac0, timewarp_animate_hook, 0x7b60, 0x7ac3);
  if (F & FZ) {
    CYCT(0x7ac3, 0x7ac5);
    goto afterAnimate;
  }
  CYC(0x7ac3, 0x7ac5);
  CYC(0x7ac5, 0x7ac6); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x7ac6); return; }
  CYC(0x7ac6, 0x7ac7);
  CYC(0x7ac7, 0x7ac8); alu_xor(gb, A);
  CYC(0x7ac8, 0x7ac9); mem_wr(gb, DE, A);
  CYC(0x7ac9, 0x7acb); B = 0x04;
  CYC(0x7acb, 0x7ace);
  timewarp_spawnChild_hook(gb);
  return;
afterAnimate:
  CYC(0x7ace, 0x7ad0); A = 0x04;
  CALL_C(0x7ad0, objectGetRelatedObject2Var_hook, 0x2164, 0x7ad3);
  CYC(0x7ad3, 0x7ad4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x7ad4, interactionIncState_hook, 0x23e0, 0x7ad7);
  CYC(0x7ad7, 0x7ad9); A = 0x01;
  CYC(0x7ad9, 0x7adc);
  interactionSetAnimation_hook(gb);
}

void itemwarp_subid3Or4_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b28, 0x7b2a); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7b2a, 0x7b2b); A = mem_rd(gb, DE);
  CYC(0x7b2b, 0x7b2d); alu_add(gb, 0xc0);
  CALL_C(0x7b2d, loadPaletteHeader_hook, 0x050b, 0x7b30);
  CALL_C(0x7b30, interactionInitGraphics_hook, 0x15fb, 0x7b33);
  CALL_C(0x7b33, interactionIncState_hook, 0x23e0, 0x7b36);
  CYC(0x7b36, 0x7b39);
  objectSetVisible82_hook(gb);
}

void timewarp_subid3_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b39, timewarp_animate_hook, 0x7b60, 0x7b3c);
  if (!(F & FZ)) { RET_TAKEN(0x7b3c); return; }
  CYC(0x7b3c, 0x7b3d);
  CYC(0x7b3d, 0x7b3f); A = 0x03;
  CALL_C(0x7b3f, interactionSetAnimation_hook, 0x262e, 0x7b42);
  CYC(0x7b42, 0x7b45);
  interactionIncState_hook(gb);
}

void timewarp_subid3Or4_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b45, interactionIncState_hook, 0x23e0, 0x7b48);
  CYC(0x7b48, 0x7b4a); A = 0x04;
  CYC(0x7b4a, 0x7b4d);
  interactionSetAnimation_hook(gb);
}

void timewarp_subid3Or4_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b4d, timewarp_animate_hook, 0x7b60, 0x7b50);
  if (!(F & FZ)) { RET_TAKEN(0x7b50); return; }
  CYC(0x7b50, 0x7b51);
  CYC(0x7b51, 0x7b54);
  interactionDelete_hook(gb);
}

void timewarp_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a08, 0x7a0a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7a0a, 0x7a0b); A = mem_rd(gb, DE);
  CYC(0x7a0b, 0x7a0c); push_effect(gb, 0x7a0c);
  switch (timewarp_jump_table(gb)) {
    case 0x7a14: timewarp_common_state0_hook(gb); return;
    case 0x7a28: timewarp_subid0_state1_hook(gb); return;
    case 0x7a50: timewarp_subid0_state2_hook(gb); return;
    case 0x7aaf: timewarp_animateUntilFinished_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void timewarp_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7ab6, 0x7ab8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7ab8, 0x7ab9); A = mem_rd(gb, DE);
  CYC(0x7ab9, 0x7aba); push_effect(gb, 0x7aba);
  switch (timewarp_jump_table(gb)) {
    case 0x7a14: timewarp_common_state0_hook(gb); return;
    case 0x7ac0: timewarp_subid1_state1_hook(gb); return;
    case 0x7aaf: timewarp_animateUntilFinished_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void timewarp_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7adc, 0x7ade); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7ade, 0x7adf); A = mem_rd(gb, DE);
  CYC(0x7adf, 0x7ae0); push_effect(gb, 0x7ae0);
  switch (timewarp_jump_table(gb)) {
    case 0x7ae6: goto state0;
    case 0x7af7: goto state1;
    case 0x7afe: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CALL_C(0x7ae6, interactionInitGraphics_hook, 0x15fb, 0x7ae9);
  CALL_C(0x7ae9, interactionIncState_hook, 0x23e0, 0x7aec);
  CYC(0x7aec, 0x7aee); L = INTERACTION_BASE + OBJ_SPEED_TMP;
  CYC(0x7aee, 0x7af0); mem_wr(gb, HL, 0xfc);
  CYC(0x7af0, 0x7af2); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7af2, 0x7af4); mem_wr(gb, HL, 0x06);
  CYC(0x7af4, 0x7af7);
  objectSetVisible81_hook(gb);
  return;
state1:
  CALL_C(0x7af7, timewarp_animate_hook, 0x7b60, 0x7afa);
  if (!(F & FZ)) { RET_TAKEN(0x7afa); return; }
  CYC(0x7afa, 0x7afb);
  CYC(0x7afb, 0x7afe);
  interactionIncState_hook(gb);
  return;
state2:
  CALL_C(0x7afe, objectApplyComponentSpeed_hook, 0x2008, 0x7b01);
  CYC(0x7b01, 0x7b03); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x7b03, 0x7b04); A = mem_rd(gb, DE);
  CYC(0x7b04, 0x7b06); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(0x7b06, 0x7b09);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7b06, 0x7b09);
  CALL_C(0x7b09, interactionDecCounter1_hook, 0x23cc, 0x7b0c);
  if (!(F & FZ)) { RET_TAKEN(0x7b0c); return; }
  CYC(0x7b0c, 0x7b0d);
  CYC(0x7b0d, 0x7b0f); mem_wr(gb, HL, 0x06);
  CYC(0x7b0f, 0x7b12); SET_BC(0x8401);
  CALL_C(0x7b12, objectCreateInteraction_hook, 0x24c5, 0x7b15);
  if (!(F & FZ)) { RET_TAKEN(0x7b15); return; }
  CYC(0x7b15, 0x7b16);
  CYC(0x7b16, 0x7b18); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7b18, 0x7b19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x7b19); return;
}

void timewarp_subid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b1a, 0x7b1c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7b1c, 0x7b1d); A = mem_rd(gb, DE);
  CYC(0x7b1d, 0x7b1e); push_effect(gb, 0x7b1e);
  switch (timewarp_jump_table(gb)) {
    case 0x7b28: itemwarp_subid3Or4_state0_hook(gb); return;
    case 0x7b39: timewarp_subid3_state1_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x7b45: timewarp_subid3Or4_state3_hook(gb); return;
    case 0x7b4d: timewarp_subid3Or4_state4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void timewarp_subid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b54, 0x7b56); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7b56, 0x7b57); A = mem_rd(gb, DE);
  CYC(0x7b57, 0x7b58); push_effect(gb, 0x7b58);
  switch (timewarp_jump_table(gb)) {
    case 0x7b28: itemwarp_subid3Or4_state0_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x7b45: timewarp_subid3Or4_state3_hook(gb); return;
    case 0x7b4d: timewarp_subid3Or4_state4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void interactionCodedd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x79fa, 0x79fc); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x79fc, 0x79fd); A = mem_rd(gb, DE);
  CYC(0x79fd, 0x79fe); push_effect(gb, 0x79fe);
  switch (timewarp_jump_table(gb)) {
    case 0x7a08: timewarp_subid0_hook(gb); return;
    case 0x7ab6: timewarp_subid1_hook(gb); return;
    case 0x7adc: timewarp_subid2_hook(gb); return;
    case 0x7b1a: timewarp_subid3_hook(gb); return;
    case 0x7b54: timewarp_subid4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
