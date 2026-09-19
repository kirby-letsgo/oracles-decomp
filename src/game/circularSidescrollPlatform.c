#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void sidescrollingPlatformCommon_hook(GB *gb);
void sidescrollPlatform_checkLinkSquished_hook(GB *gb);
void sidescrollPlatform_checkLinkIsClose_hook(GB *gb);
void sidescrollPlatform_getTileCollisionBehindLink_hook(GB *gb);
void sidescrollPlatformFunc_5b51_hook(GB *gb);
void sidescrollPlatform_checkLinkOnPlatform_hook(GB *gb);
void sidescrollPlatform_updateLinkKnockbackForConveyor_hook(GB *gb);
void sidescrollPlatform_decCounter1_hook(GB *gb);
void sidescrollPlatform_pushLinkAwayVertical_hook(GB *gb);
void sidescrollPlatform_pushLinkAwayHorizontal_hook(GB *gb);
void sidescrollPlatformFunc_5bfc_hook(GB *gb);
void sidescrollPlatform_updateLinkSubpixels_hook(GB *gb);

static uint16_t interactionCodea4_jump_table(GB *gb) {
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

static void interactionCodea4_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

// 0b:5a67. Reached both by a genuine `call` from @state0 and by plain fallthrough from the end of
// @state1 (no jump instruction at all) -- a plain function works for both.
void interactionCodea4_func_5a67_hook(GB *gb) {
  CYC(0x5a67, 0x5a68); H = D;
  CYC(0x5a68, 0x5a6a); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(0x5a6a, 0x5a6c); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5a6c, 0x5a6d); A = mem_rd(gb, DE);
  CYC(0x5a6d, 0x5a6e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5a6e, 0x5a70); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5a70, 0x5a71); A = mem_rd(gb, DE);
  CYC(0x5a71, 0x5a72); mem_wr(gb, HL, A);
  RET(0x5a72); return;
}

// 0b:59f7, called once from interactionCodea4_hook.
void interactionCodea4_updateState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59f7, 0x59f9); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x59f9, 0x59fa); A = mem_rd(gb, DE);
  CYC(0x59fa, 0x59fb); push_effect(gb, 0x59fb);
  switch (interactionCodea4_jump_table(gb)) {
    case 0x59ff: goto state0;
    case 0x5a38: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x59ff, interactionInitGraphics_hook, 0x15fb, 0x5a02);
  CYC(0x5a02, 0x5a03); H = D;
  CYC(0x5a03, 0x5a05); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5a05, 0x5a06); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5a06, 0x5a08); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5a08, 0x5a0a); A = 0x08;
  CYC(0x5a0a, 0x5a0b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5a0b, 0x5a0c); mem_wr(gb, HL, A);
  CYC(0x5a0c, 0x5a0e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5a0e, 0x5a10); mem_wr(gb, HL, 0xc0); // SPEED_c0
  CYC(0x5a10, 0x5a12); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5a12, 0x5a14); mem_wr(gb, HL, 0x07);
  CYC(0x5a14, 0x5a16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5a16, 0x5a17); A = mem_rd(gb, DE);
  CYC(0x5a17, 0x5a1a); SET_HL(0x5a35); // @angles
  CYC(0x5a1a, 0x5a1b); interactionCodea4_add_a_to_hl(gb); // rst $10
  CYC(0x5a1b, 0x5a1d); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5a1d, 0x5a1e); A = mem_rd(gb, HL);
  CYC(0x5a1e, 0x5a1f); mem_wr(gb, DE, A);
  CYC(0x5a1f, 0x5a22); SET_BC(0x5678);
  CYC(0x5a22, 0x5a24); A = 0x35;
  CALL_C(0x5a24, objectSetPositionInCircleArc_hook, 0x210e, 0x5a27);
  CYC(0x5a27, 0x5a29); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5a29, 0x5a2a); A = mem_rd(gb, DE);
  CYC(0x5a2a, 0x5a2c); alu_add(gb, 0x08);
  CYC(0x5a2c, 0x5a2e); alu_and(gb, 0x1f);
  CYC(0x5a2e, 0x5a2f); mem_wr(gb, DE, A);
  CALL_C(0x5a2f, interactionCodea4_func_5a67_hook, 0x5a67, 0x5a32);
  CYC(0x5a32, 0x5a35); objectSetVisible82_hook(gb); return; // jp

  // @angles (0b:5a35): pure ROM data (ANGLE_UP, ANGLE_RIGHT, ANGLE_DOWN), referenced only via
  // SET_HL(0x5a35) above.

state1:
  CALL_C(0x5a38, interactionDecCounter1_hook, 0x23cc, 0x5a3b);
  if (!(F & FZ)) { CYCT(0x5a3b, 0x5a3d); goto l_5a46; } // jr nz
  CYC(0x5a3b, 0x5a3d);
  CYC(0x5a3d, 0x5a3f); mem_wr(gb, HL, 0x0e);
  CYC(0x5a3f, 0x5a41); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5a41, 0x5a42); A = mem_rd(gb, HL);
  CYC(0x5a42, 0x5a43); A = alu_inc8(gb, A);
  CYC(0x5a43, 0x5a45); alu_and(gb, 0x1f);
  CYC(0x5a45, 0x5a46); mem_wr(gb, HL, A);

l_5a46:
  CALL_C(0x5a46, objectApplySpeed_hook, 0x201d, 0x5a49);
  CYC(0x5a49, 0x5a4b); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x5a4b, 0x5a4c); A = mem_rd(gb, DE);
  CYC(0x5a4c, 0x5a4d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5a4d, 0x5a4f); interactionCodea4_func_5a67_hook(gb); return; } // jr z
  CYC(0x5a4d, 0x5a4f);
  CYC(0x5a4f, 0x5a50); H = D;
  CYC(0x5a50, 0x5a52); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(0x5a52, 0x5a54); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5a54, 0x5a55); A = mem_rd(gb, DE);
  CYC(0x5a55, 0x5a56); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5a56, 0x5a57); B = A;
  CYC(0x5a57, 0x5a58); L = alu_inc8(gb, L);
  CYC(0x5a58, 0x5a5a); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5a5a, 0x5a5b); A = mem_rd(gb, DE);
  CYC(0x5a5b, 0x5a5c); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5a5c, 0x5a5d); C = A;
  CYC(0x5a5d, 0x5a60); SET_HL(w1Link_yh);
  CYC(0x5a60, 0x5a61); A = mem_rd(gb, HL);
  CYC(0x5a61, 0x5a62); alu_add(gb, B);
  CYC(0x5a62, 0x5a63); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5a63, 0x5a64); L = alu_inc8(gb, L);
  CYC(0x5a64, 0x5a65); A = mem_rd(gb, HL);
  CYC(0x5a65, 0x5a66); alu_add(gb, C);
  CYC(0x5a66, 0x5a67); mem_wr(gb, HL, A);
  interactionCodea4_func_5a67_hook(gb); // falls through (no jump at all)
}

// ==================================================================================================
// INTERAC_CIRCULAR_SIDESCROLL_PLATFORM
// ==================================================================================================
void interactionCodea4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59ee, sidescrollPlatform_checkLinkOnPlatform_hook, 0x5b7f, 0x59f1);
  CALL_C(0x59f1, interactionCodea4_updateState_hook, 0x59f7, 0x59f4);
  CYC(0x59f4, 0x59f7); sidescrollingPlatformCommon_hook(gb); return; // jp
}

//;;
// Used by:
// * INTERAC_MOVING_SIDESCROLL_PLATFORM
// * INTERAC_MOVING_SIDESCROLL_CONVEYOR
// * INTERAC_DISAPPEARING_SIDESCROLL_PLATFORM
// * INTERAC_CIRCULAR_SIDESCROLL_PLATFORM
void sidescrollingPlatformCommon_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5a73, 0x5a76); A = mem_rd(gb, w1Link_state);
  CYC(0x5a76, 0x5a78); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) { CYCT(0x5a78, 0x5a79); ret_effect(gb); return; } // ret nz
  CYC(0x5a78, 0x5a79);
  CALL_C(0x5a79, objectCheckCollidedWithLink_hook, 0x1c41, 0x5a7c);
  if (!(F & FC)) { CYCT(0x5a7c, 0x5a7d); ret_effect(gb); return; } // ret nc
  CYC(0x5a7c, 0x5a7d);
  CALL_C(0x5a7d, sidescrollPlatform_checkLinkIsClose_hook, 0x5b04, 0x5a80);
  if (F & FC) { CYCT(0x5a80, 0x5a82); goto label_0b_183; } // jr c
  CYC(0x5a80, 0x5a82);
  CALL_C(0x5a82, sidescrollPlatform_getTileCollisionBehindLink_hook, 0x5b32, 0x5a85);
  if (F & FZ) { CYCT(0x5a85, 0x5a88); sidescrollPlatform_pushLinkAwayHorizontal_hook(gb); return; } // jp z
  CYC(0x5a85, 0x5a88);
  CALL_C(0x5a88, sidescrollPlatform_checkLinkSquished_hook, 0x5acf, 0x5a8b);
  if (F & FC) { CYCT(0x5a8b, 0x5a8c); ret_effect(gb); return; } // ret c
  CYC(0x5a8b, 0x5a8c);
  CYC(0x5a8c, 0x5a8e); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5a8e, 0x5a8f); A = mem_rd(gb, DE);
  CYC(0x5a8f, 0x5a90); B = A;
  CYC(0x5a90, 0x5a93); A = mem_rd(gb, w1Link_yh);
  CYC(0x5a93, 0x5a94); alu_cp(gb, B);
  CYC(0x5a94, 0x5a96); C = 0x00; // ANGLE_UP
  if (!(F & FC)) { CYCT(0x5a96, 0x5a98); goto moveLinkAtAngle; } // jr nc
  CYC(0x5a96, 0x5a98);
  CYC(0x5a98, 0x5a9a); C = 0x10; // ANGLE_DOWN
  CYC(0x5a9a, 0x5a9c); goto moveLinkAtAngle; // jr (unconditional)

label_0b_183:
  CALL_C(0x5a9c, sidescrollPlatformFunc_5b51_hook, 0x5b51, 0x5a9f);
  CYC(0x5a9f, 0x5aa0); A = mem_rd(gb, HL);
  CYC(0x5aa0, 0x5aa1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5aa1, 0x5aa4); sidescrollPlatform_pushLinkAwayVertical_hook(gb); return; } // jp z
  CYC(0x5aa1, 0x5aa4);
  CALL_C(0x5aa4, sidescrollPlatform_checkLinkSquished_hook, 0x5acf, 0x5aa7);
  if (F & FC) { CYCT(0x5aa7, 0x5aa8); ret_effect(gb); return; } // ret c
  CYC(0x5aa7, 0x5aa8);
  CYC(0x5aa8, 0x5aab); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x5aab, 0x5aac); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(0x5aac, 0x5aae); goto label_0b_184; } // jr nz
  CYC(0x5aac, 0x5aae);
  CYC(0x5aae, 0x5ab0); A = hram_rd(gb, 0x8b);
  CYC(0x5ab0, 0x5ab2); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x5ab2, 0x5ab4); goto label_0b_184; } // jr z
  CYC(0x5ab2, 0x5ab4);
  CYC(0x5ab4, 0x5ab5); push_effect(gb, AF);
  CALL_C(0x5ab5, sidescrollPlatform_pushLinkAwayVertical_hook, 0x5bd6, 0x5ab8);
  CYC(0x5ab8, 0x5ab9); SET_AF(pop_effect(gb));
  CYC(0x5ab9, 0x5aba); alu_rrca(gb);
  CYC(0x5aba, 0x5abc); goto l_5ac4; // jr (unconditional)

label_0b_184:
  CYC(0x5abc, 0x5abe); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5abe, 0x5abf); A = mem_rd(gb, DE);
  CYC(0x5abf, 0x5ac0); B = A;
  CYC(0x5ac0, 0x5ac3); A = mem_rd(gb, w1Link_xh);
  CYC(0x5ac3, 0x5ac4); alu_cp(gb, B);

l_5ac4:
  CYC(0x5ac4, 0x5ac6); C = 0x08; // ANGLE_RIGHT
  if (!(F & FC)) { CYCT(0x5ac6, 0x5ac8); goto moveLinkAtAngle; } // jr nc
  CYC(0x5ac6, 0x5ac8);
  CYC(0x5ac8, 0x5aca); C = 0x18; // ANGLE_LEFT

moveLinkAtAngle:
  CYC(0x5aca, 0x5acc); B = 0x14; // SPEED_80
  CYC(0x5acc, 0x5acf); updateLinkPositionGivenVelocity_hook(gb); return; // jp
}

// @param[out] cflag c if Link got squished
void sidescrollPlatform_checkLinkSquished_hook(GB *gb) {
  CYC(0x5acf, 0x5ad0); H = D;
  CYC(0x5ad0, 0x5ad2); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5ad2, 0x5ad3); A = mem_rd(gb, HL);
  CYC(0x5ad3, 0x5ad4); B = A;
  CYC(0x5ad4, 0x5ad5); alu_add(gb, A);
  CYC(0x5ad5, 0x5ad6); A = alu_inc8(gb, A);
  CYC(0x5ad6, 0x5ad7); C = A;
  CYC(0x5ad7, 0x5ad9); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5ad9, 0x5adc); A = mem_rd(gb, w1Link_yh);
  CYC(0x5adc, 0x5add); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5add, 0x5ade); alu_add(gb, B);
  CYC(0x5ade, 0x5adf); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x5adf, 0x5ae0); ret_effect(gb); return; } // ret nc
  CYC(0x5adf, 0x5ae0);
  CYC(0x5ae0, 0x5ae2); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x5ae2, 0x5ae3); A = mem_rd(gb, HL);
  CYC(0x5ae3, 0x5ae5); alu_add(gb, 0x02);
  CYC(0x5ae5, 0x5ae6); B = A;
  CYC(0x5ae6, 0x5ae7); alu_add(gb, A);
  CYC(0x5ae7, 0x5ae8); A = alu_inc8(gb, A);
  CYC(0x5ae8, 0x5ae9); C = A;
  CYC(0x5ae9, 0x5aeb); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5aeb, 0x5aee); A = mem_rd(gb, w1Link_xh);
  CYC(0x5aee, 0x5aef); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5aef, 0x5af0); alu_add(gb, B);
  CYC(0x5af0, 0x5af1); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x5af1, 0x5af2); ret_effect(gb); return; } // ret nc
  CYC(0x5af1, 0x5af2);
  CYC(0x5af2, 0x5af3); alu_xor(gb, A);
  CYC(0x5af3, 0x5af5); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5af5, 0x5af7); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5af7, 0x5af9); goto l_5afa; } // jr nz
  CYC(0x5af7, 0x5af9);
  CYC(0x5af9, 0x5afa); A = alu_inc8(gb, A);

l_5afa:
  CYC(0x5afa, 0x5afd); mem_wr(gb, wcc50, A);
  CYC(0x5afd, 0x5aff); A = 0x11; // LINK_STATE_SQUISHED
  CYC(0x5aff, 0x5b02); mem_wr(gb, wLinkForceState, A);
  CYC(0x5b02, 0x5b03); alu_scf(gb);
  RET(0x5b03); return;
}

// @param[out] cflag c if Link's close enough to the platform?
void sidescrollPlatform_checkLinkIsClose_hook(GB *gb) {
  CYC(0x5b04, 0x5b07); A = mem_rd(gb, wLinkInAir);
  CYC(0x5b07, 0x5b08); alu_or(gb, A);
  CYC(0x5b08, 0x5b0a); B = 0x05;
  if (F & FZ) { CYCT(0x5b0a, 0x5b0c); goto l_5b0d; } // jr z
  CYC(0x5b0a, 0x5b0c);
  CYC(0x5b0c, 0x5b0d); B = alu_dec8(gb, B);

l_5b0d:
  CYC(0x5b0d, 0x5b0e); H = D;
  CYC(0x5b0e, 0x5b10); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x5b10, 0x5b11); A = mem_rd(gb, HL);
  CYC(0x5b11, 0x5b12); alu_add(gb, B);
  CYC(0x5b12, 0x5b13); B = A;
  CYC(0x5b13, 0x5b14); alu_add(gb, A);
  CYC(0x5b14, 0x5b15); A = alu_inc8(gb, A);
  CYC(0x5b15, 0x5b16); C = A;
  CYC(0x5b16, 0x5b18); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5b18, 0x5b1b); A = mem_rd(gb, w1Link_xh);
  CYC(0x5b1b, 0x5b1c); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b1c, 0x5b1d); alu_add(gb, B);
  CYC(0x5b1d, 0x5b1e); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x5b1e, 0x5b1f); ret_effect(gb); return; } // ret nc
  CYC(0x5b1e, 0x5b1f);
  CYC(0x5b1f, 0x5b21); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5b21, 0x5b22); A = mem_rd(gb, HL);
  CYC(0x5b22, 0x5b24); alu_sub(gb, 0x02);
  CYC(0x5b24, 0x5b25); B = A;
  CYC(0x5b25, 0x5b26); alu_add(gb, A);
  CYC(0x5b26, 0x5b27); A = alu_inc8(gb, A);
  CYC(0x5b27, 0x5b28); C = A;
  CYC(0x5b28, 0x5b2a); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5b2a, 0x5b2d); A = mem_rd(gb, w1Link_yh);
  CYC(0x5b2d, 0x5b2e); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b2e, 0x5b2f); alu_add(gb, B);
  CYC(0x5b2f, 0x5b30); alu_cp(gb, C);
  CYC(0x5b30, 0x5b31); alu_ccf(gb);
  RET(0x5b31); return;
}

// @param[out] a Collision value
// @param[out] zflag nz if a valid collision value is returned
void sidescrollPlatform_getTileCollisionBehindLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b32, 0x5b34); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5b34, 0x5b37); A = mem_rd(gb, w1Link_xh);
  CYC(0x5b37, 0x5b38); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x5b38, 0x5b3a); B = 0xfb; // -5
  if (F & FC) { CYCT(0x5b3a, 0x5b3c); goto l_5b3e; } // jr c
  CYC(0x5b3a, 0x5b3c);
  CYC(0x5b3c, 0x5b3e); B = 0x04;

l_5b3e:
  CYC(0x5b3e, 0x5b3f); alu_add(gb, B);
  CYC(0x5b3f, 0x5b40); C = A;
  CYC(0x5b40, 0x5b43); A = mem_rd(gb, w1Link_yh);
  CYC(0x5b43, 0x5b45); alu_sub(gb, 0x04);
  CYC(0x5b45, 0x5b46); B = A;
  CALL_C(0x5b46, getTileCollisionsAtPosition_hook, 0x14b7, 0x5b49);
  if (!(F & FZ)) { CYCT(0x5b49, 0x5b4a); ret_effect(gb); return; } // ret nz
  CYC(0x5b49, 0x5b4a);
  CYC(0x5b4a, 0x5b4b); A = B;
  CYC(0x5b4b, 0x5b4d); alu_add(gb, 0x08);
  CYC(0x5b4d, 0x5b4e); B = A;
  CYC(0x5b4e, 0x5b51); getTileCollisionsAtPosition_hook(gb); return; // jp
}

// @param[out] hl
void sidescrollPlatformFunc_5b51_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b51, 0x5b52); H = D;
  CYC(0x5b52, 0x5b54); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5b54, 0x5b57); A = mem_rd(gb, w1Link_yh);
  CYC(0x5b57, 0x5b58); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x5b58, 0x5b5a); B = 0xfa; // -6
  if (F & FC) { CYCT(0x5b5a, 0x5b5c); goto l_5b5e; } // jr c
  CYC(0x5b5a, 0x5b5c);
  CYC(0x5b5c, 0x5b5e); B = 0x09;

l_5b5e:
  CYC(0x5b5e, 0x5b5f); alu_add(gb, B);
  CYC(0x5b5f, 0x5b60); B = A;
  CYC(0x5b60, 0x5b63); A = mem_rd(gb, w1Link_xh);
  CYC(0x5b63, 0x5b65); alu_sub(gb, 0x03);
  CYC(0x5b65, 0x5b66); C = A;
  CALL_C(0x5b66, getTileCollisionsAtPosition_hook, 0x14b7, 0x5b69);
  CYC(0x5b69, 0x5b6c); SET_HL(hFF8B);
  CYC(0x5b6c, 0x5b6e); mem_wr(gb, HL, 0x00);
  if (F & FZ) { CYCT(0x5b6e, 0x5b70); goto l_5b72; } // jr z
  CYC(0x5b6e, 0x5b70);
  CYC(0x5b70, 0x5b72); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));

l_5b72:
  CYC(0x5b72, 0x5b73); A = C;
  CYC(0x5b73, 0x5b75); alu_add(gb, 0x05);
  CYC(0x5b75, 0x5b76); C = A;
  CALL_C(0x5b76, getTileCollisionsAtPosition_hook, 0x14b7, 0x5b79);
  CYC(0x5b79, 0x5b7c); SET_HL(hFF8B);
  if (F & FZ) { CYCT(0x5b7c, 0x5b7d); ret_effect(gb); return; } // ret z
  CYC(0x5b7c, 0x5b7d);
  CYC(0x5b7d, 0x5b7e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x5b7e); return;
}

// Checks if Link's on the platform, updates wLinkRidingObject if so.
// @param[out] zflag nz if Link is standing on the platform
void sidescrollPlatform_checkLinkOnPlatform_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b7f, objectCheckCollidedWithLink_hook, 0x1c41, 0x5b82);
  if (!(F & FC)) { CYCT(0x5b82, 0x5b84); goto notOnPlatform; } // jr nc
  CYC(0x5b82, 0x5b84);
  CYC(0x5b84, 0x5b85); H = D;
  CYC(0x5b85, 0x5b87); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5b87, 0x5b88); A = mem_rd(gb, HL);
  CYC(0x5b88, 0x5b8a); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5b8a, 0x5b8b); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b8b, 0x5b8d); alu_sub(gb, 0x02);
  CYC(0x5b8d, 0x5b8e); B = A;
  CYC(0x5b8e, 0x5b91); A = mem_rd(gb, w1Link_yh);
  CYC(0x5b91, 0x5b92); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x5b92, 0x5b94); goto notOnPlatform; } // jr nc
  CYC(0x5b92, 0x5b94);
  CALL_C(0x5b94, sidescrollPlatform_checkLinkIsClose_hook, 0x5b04, 0x5b97);
  if (!(F & FC)) { CYCT(0x5b97, 0x5b99); goto notOnPlatform; } // jr nc
  CYC(0x5b97, 0x5b99);
  CYC(0x5b99, 0x5b9b); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x5b9b, 0x5b9c); A = mem_rd(gb, DE);
  CYC(0x5b9c, 0x5b9d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5b9d, 0x5b9f); goto onPlatform; } // jr nz
  CYC(0x5b9d, 0x5b9f);
  CYC(0x5b9f, 0x5ba1); A = 0x01;
  CYC(0x5ba1, 0x5ba2); mem_wr(gb, DE, A);
  CALL_C(0x5ba2, sidescrollPlatform_updateLinkSubpixels_hook, 0x5c04, 0x5ba5);

onPlatform:
  CYC(0x5ba5, 0x5ba6); A = D;
  CYC(0x5ba6, 0x5ba9); mem_wr(gb, wLinkRidingObject, A);
  CYC(0x5ba9, 0x5baa); alu_xor(gb, A);
  RET(0x5baa); return;

notOnPlatform:
  CYC(0x5bab, 0x5bad); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x5bad, 0x5bae); A = mem_rd(gb, DE);
  CYC(0x5bae, 0x5baf); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5baf, 0x5bb0); ret_effect(gb); return; } // ret z
  CYC(0x5baf, 0x5bb0);
  CYC(0x5bb0, 0x5bb2); A = 0x00;
  CYC(0x5bb2, 0x5bb3); mem_wr(gb, DE, A);
  RET(0x5bb3); return;
}

void sidescrollPlatform_updateLinkKnockbackForConveyor_hook(GB *gb) {
  CYC(0x5bb4, 0x5bb6); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5bb6, 0x5bb7); A = mem_rd(gb, DE);
  CYC(0x5bb7, 0x5bb9); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(0x5bb9, 0x5bba); ret_effect(gb); return; } // ret z
  CYC(0x5bb9, 0x5bba);
  CYC(0x5bba, 0x5bbd); SET_HL(w1Link_knockbackAngle);
  CYC(0x5bbd, 0x5bbf); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x5bbf, 0x5bc0); A = mem_rd(gb, DE);
  CYC(0x5bc0, 0x5bc2); A = alu_swap(gb, A);
  CYC(0x5bc2, 0x5bc4); alu_add(gb, 0x08);
  CYC(0x5bc4, 0x5bc5); mem_wr(gb, HL, A);
  CYC(0x5bc5, 0x5bc7); L = (uint8_t)w1Link_invincibilityCounter;
  CYC(0x5bc7, 0x5bc9); mem_wr(gb, HL, 0xfc);
  CYC(0x5bc9, 0x5bcb); L = (uint8_t)w1Link_knockbackCounter;
  CYC(0x5bcb, 0x5bcd); mem_wr(gb, HL, 0x0c);
  RET(0x5bcd); return;
}

// @param[out] hl counter1
void sidescrollPlatform_decCounter1_hook(GB *gb) {
  CYC(0x5bce, 0x5bcf); H = D;
  CYC(0x5bcf, 0x5bd1); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5bd1, 0x5bd2); A = mem_rd(gb, HL);
  CYC(0x5bd2, 0x5bd3); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5bd3, 0x5bd4); ret_effect(gb); return; } // ret z
  CYC(0x5bd3, 0x5bd4);
  CYC(0x5bd4, 0x5bd5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x5bd5); return;
}

// 0b:5bf0, the shared tail of pushLinkAwayVertical/pushLinkAwayHorizontal below. Entered either by
// straight fallthrough from pushLinkAwayHorizontal's own setup, or by a `jr` (no stack effect) from
// pushLinkAwayVertical after its own setup.
static void interactionCode9e_pushLinkAway_tail(GB *gb) {
  CYC(0x5bf0, 0x5bf1); A = mem_rd(gb, DE);
  CYC(0x5bf1, 0x5bf2); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x5bf2, 0x5bf4); goto tail2; } // jr c
  CYC(0x5bf2, 0x5bf4);
  CYC(0x5bf4, 0x5bf5); A = B;
  CYC(0x5bf5, 0x5bf6); alu_cpl(gb);
  CYC(0x5bf6, 0x5bf7); A = alu_inc8(gb, A);
  CYC(0x5bf7, 0x5bf8); B = A;

tail2:
  CYC(0x5bf8, 0x5bf9); A = mem_rd(gb, DE);
  CYC(0x5bf9, 0x5bfa); alu_add(gb, B);
  CYC(0x5bfa, 0x5bfb); mem_wr(gb, HL, A);
  RET(0x5bfb); return;
}

void sidescrollPlatform_pushLinkAwayVertical_hook(GB *gb) {
  CYC(0x5bd6, 0x5bd9); SET_HL(w1Link_collisionRadiusY);
  CYC(0x5bd9, 0x5bdb); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5bdb, 0x5bdc); A = mem_rd(gb, DE);
  CYC(0x5bdc, 0x5bdd); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5bdd, 0x5bde); B = A;
  CYC(0x5bde, 0x5be0); L = (uint8_t)w1Link_yh;
  CYC(0x5be0, 0x5be2); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5be2, 0x5be4); interactionCode9e_pushLinkAway_tail(gb); // jr
}

void sidescrollPlatform_pushLinkAwayHorizontal_hook(GB *gb) {
  CYC(0x5be4, 0x5be7); SET_HL(w1Link_collisionRadiusX);
  CYC(0x5be7, 0x5be9); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x5be9, 0x5bea); A = mem_rd(gb, DE);
  CYC(0x5bea, 0x5beb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5beb, 0x5bec); B = A;
  CYC(0x5bec, 0x5bee); L = (uint8_t)w1Link_xh;
  CYC(0x5bee, 0x5bf0); E = INTERACTION_BASE + OBJ_XH;
  interactionCode9e_pushLinkAway_tail(gb); // falls through
}

// 0b:5bfc, called from movingPlatform_stateC (not part of this file's cluster). Its "ret nz" not
// taken path falls straight into sidescrollPlatform_updateLinkSubpixels below (no jump at all).
void sidescrollPlatformFunc_5bfc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5bfc, objectRunMovementScript_hook, 0x3049, 0x5bff);
  CYC(0x5bff, 0x5c02); A = mem_rd(gb, wLinkRidingObject);
  CYC(0x5c02, 0x5c03); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(0x5c03, 0x5c04); ret_effect(gb); return; } // ret nz
  CYC(0x5c03, 0x5c04);
  sidescrollPlatform_updateLinkSubpixels_hook(gb); // falls through
}

void sidescrollPlatform_updateLinkSubpixels_hook(GB *gb) {
  CYC(0x5c04, 0x5c06); E = INTERACTION_BASE + OBJ_Y;
  CYC(0x5c06, 0x5c07); A = mem_rd(gb, DE);
  CYC(0x5c07, 0x5c0a); mem_wr(gb, w1Link_y, A);
  CYC(0x5c0a, 0x5c0c); E = INTERACTION_BASE + OBJ_X;
  CYC(0x5c0c, 0x5c0d); A = mem_rd(gb, DE);
  CYC(0x5c0d, 0x5c10); mem_wr(gb, w1Link_x, A);
  RET(0x5c10); return;
}
