#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactiond7_jump_table(GB *gb) {
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

static void interactiond7_addAToHl(GB *gb, uint16_t return_address) {
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

// 0b:7c0a, interactiond7_essence@playCirclingSound. Reached by a genuine `call z` from
// @state3, which still has more work to do afterward, so the resume is verified like CALL_C
// (this local itself is a bare tail-call into playSound_b00_hook with no push of its own).
static void interactiond7_essence_playCirclingSound(GB *gb) {
  CYC(0x7c0a, 0x7c0c); A = 0xc9; // SND_CIRCLING
  CYC(0x7c0c, 0x7c0f); playSound_b00_hook(gb); return; // jp
}

// 0b:7c0f, called from interactiond7_makuSeed@state2 and @state3.
void interactiond7_updateSmallSparkles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c0f, 0x7c12); A = mem_rd(gb, wFrameCounter);
  CYC(0x7c12, 0x7c14); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7c14, 0x7c15); ret_effect(gb); return; } // ret nz
  CYC(0x7c14, 0x7c15);
  CYC(0x7c15, 0x7c18); SET_BC(0x8403); // INTERAC_SPARKLE, 3
  CALL_C(0x7c18, objectCreateInteraction_hook, 0x24c5, 0x7c1b);
  if (!(F & FZ)) { CYCT(0x7c1b, 0x7c1c); ret_effect(gb); return; } // ret nz
  CYC(0x7c1b, 0x7c1c);
  CYC(0x7c1c, 0x7c1f); A = mem_rd(gb, wFrameCounter);
  CYC(0x7c1f, 0x7c21); alu_and(gb, 0x38);
  CYC(0x7c21, 0x7c23); alu_swap_a(gb);
  CYC(0x7c23, 0x7c24); alu_rlca(gb);
  CYC(0x7c24, 0x7c27); SET_BC(0x7c36); // @sparklePositionOffsets
  CALL_C(0x7c27, addDoubleIndexToBc_hook, 0x007e, 0x7c2a);
  CYC(0x7c2a, 0x7c2c); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7c2c, 0x7c2d); A = mem_rd(gb, BC);
  CYC(0x7c2d, 0x7c2e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7c2e, 0x7c2f); mem_wr(gb, HL, A);
  CYC(0x7c2f, 0x7c30); SET_BC(BC + 1); // inc bc
  CYC(0x7c30, 0x7c32); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7c32, 0x7c33); A = mem_rd(gb, BC);
  CYC(0x7c33, 0x7c34); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7c34, 0x7c35); mem_wr(gb, HL, A);
  RET(0x7c35); return;
}

// 0b:7c46, called from interactiond7_makuSeed@state3.
// Updates Z-position based on frame counter.
void interactiond7_updateFloating_hook(GB *gb) {
  CYC(0x7c46, 0x7c49); A = mem_rd(gb, wFrameCounter);
  CYC(0x7c49, 0x7c4b); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7c4b, 0x7c4c); ret_effect(gb); return; } // ret nz
  CYC(0x7c4b, 0x7c4c);
  CYC(0x7c4c, 0x7c4f); A = mem_rd(gb, wFrameCounter);
  CYC(0x7c4f, 0x7c51); alu_and(gb, 0x38);
  CYC(0x7c51, 0x7c53); alu_swap_a(gb);
  CYC(0x7c53, 0x7c54); alu_rlca(gb);
  CYC(0x7c54, 0x7c57); SET_HL(0x7c5d); // @zPositions
  CYC(0x7c57, 0x7c58); interactiond7_addAToHl(gb, 0x7c58);
  CYC(0x7c58, 0x7c5a); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7c5a, 0x7c5b); A = mem_rd(gb, HL);
  CYC(0x7c5b, 0x7c5c); mem_wr(gb, DE, A);
  RET(0x7c5c); return;
}

// 0b:7bac, called from interactionCoded7 (subid 1-8).
void interactiond7_essence_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7bac, 0x7bae); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7bae, 0x7baf); A = mem_rd(gb, DE);
  CYC(0x7baf, 0x7bb0); push_effect(gb, 0x7bb0);
  switch (interactiond7_jump_table(gb)) {
    case 0x7bb8: goto state0;
    case 0x7bd7: goto state1;
    case 0x7be1: goto state2;
    case 0x7be9: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x7bb8, 0x7bba); A = 0x01;
  CYC(0x7bba, 0x7bbb); mem_wr(gb, DE, A); // [state]
  CYC(0x7bbb, 0x7bbc); H = D;
  CYC(0x7bbc, 0x7bbe); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7bbe, 0x7bc0); mem_wr(gb, HL, 0x10);
  CYC(0x7bc0, 0x7bc2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7bc2, 0x7bc4); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x7bc4, 0x7bc6); A = 0x98; // SND_POOF
  CALL_C(0x7bc6, playSound_b00_hook, 0x0c98, 0x7bc9);
  CALL_C(0x7bc9, objectCenterOnTile_hook, 0x20db, 0x7bcc);
  CYC(0x7bcc, 0x7bce); L = INTERACTION_BASE + OBJ_Z;
  CYC(0x7bce, 0x7bcf); alu_xor(gb, A);
  CYC(0x7bcf, 0x7bd0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7bd0, 0x7bd1); mem_wr(gb, HL, A);
  CALL_C(0x7bd1, interactionInitGraphics_hook, 0x15fb, 0x7bd4);
  CYC(0x7bd4, 0x7bd7); objectSetVisible80_hook(gb); return; // jp

state1:
  CALL_C(0x7bd7, objectApplySpeed_hook, 0x201d, 0x7bda);
  CALL_C(0x7bda, interactionDecCounter1_hook, 0x23cc, 0x7bdd);
  if (!(F & FZ)) { CYCT(0x7bdd, 0x7bde); ret_effect(gb); return; } // ret nz
  CYC(0x7bdd, 0x7bde);
  CYC(0x7bde, 0x7be1); interactionIncState_hook(gb); return; // jp

state2:
  CYC(0x7be1, 0x7be4); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x7be4, 0x7be5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7be5, 0x7be6); ret_effect(gb); return; } // ret z
  CYC(0x7be5, 0x7be6);
  CYC(0x7be6, 0x7be9); interactionIncState_hook(gb); return; // jp

state3:
  CALL_C(0x7be9, objectCheckWithinScreenBoundary_hook, 0x2184, 0x7bec);
  if (!(F & FC)) { CYCT(0x7bec, 0x7bef); interactionDelete_hook(gb); return; } // jp nc
  CYC(0x7bec, 0x7bef);
  CYC(0x7bef, 0x7bf2); A = mem_rd(gb, wFrameCounter);
  CYC(0x7bf2, 0x7bf3); alu_rrca(gb);
  if (F & FC) { CYCT(0x7bf3, 0x7bf4); ret_effect(gb); return; } // ret c
  CYC(0x7bf3, 0x7bf4);
  CYC(0x7bf4, 0x7bf5); H = D;
  CYC(0x7bf5, 0x7bf7); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7bf7, 0x7bf8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x7bf8, 0x7bf9); A = mem_rd(gb, HL);
  CYC(0x7bf9, 0x7bfb); alu_and(gb, 0x1f);
  CYC(0x7bfb, 0x7bfc); mem_wr(gb, HL, A);
  CYC(0x7bfc, 0x7bfd); E = L;
  CYC(0x7bfd, 0x7bfe); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7bfe, 0x7c01); push_effect(gb, 0x7c01); interactiond7_essence_playCirclingSound(gb);
    if (!(gb->pc == 0x7c01 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  } else {
    CYC(0x7bfe, 0x7c01);
  }
  CYC(0x7c01, 0x7c04); SET_BC(0x5878);
  CYC(0x7c04, 0x7c07); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfc1);
  CYC(0x7c07, 0x7c0a); objectSetPositionInCircleArc_hook(gb); return; // jp
}

// 0b:79d8, called from interactionCoded7 (subid 0).
void interactiond7_makuSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79d8, 0x79da); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x79da, 0x79db); A = mem_rd(gb, DE);
  CYC(0x79db, 0x79dc); push_effect(gb, 0x79dc);
  switch (interactiond7_jump_table(gb)) {
    case 0x79e6: goto state0;
    case 0x7a18: goto state1;
    case 0x7a2a: goto state2;
    case 0x7a50: goto state3;
    case 0x7b10: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x79e6, 0x79e8); A = 0x01;
  CYC(0x79e8, 0x79e9); mem_wr(gb, DE, A); // [state]
  CALL_C(0x79e9, interactionInitGraphics_hook, 0x15fb, 0x79ec);
  CYC(0x79ec, 0x79ef); A = mem_rd(gb, w1Link_yh);
  CYC(0x79ef, 0x79f1); alu_sub(gb, 0x0e);
  CYC(0x79f1, 0x79f3); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x79f3, 0x79f4); mem_wr(gb, DE, A);
  CYC(0x79f4, 0x79f7); A = mem_rd(gb, w1Link_xh);
  CYC(0x79f7, 0x79f9); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x79f9, 0x79fa); mem_wr(gb, DE, A);
  CALL_C(0x79fa, setLinkForceStateToState08_hook, 0x2aad, 0x79fd);
  CYC(0x79fd, 0x79ff); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(0x79ff, playSound_b00_hook, 0x0c98, 0x7a02);
  CYC(0x7a02, 0x7a04); A = 0x77; // SND_DROPESSENCE
  CALL_C(0x7a04, playSound_b00_hook, 0x0c98, 0x7a07);
  CYC(0x7a07, 0x7a0a); SET_BC(0x8404); // INTERAC_SPARKLE, 4
  CALL_C(0x7a0a, objectCreateInteraction_hook, 0x24c5, 0x7a0d);
  if (!(F & FZ)) { CYCT(0x7a0d, 0x7a0e); ret_effect(gb); return; } // ret nz
  CYC(0x7a0d, 0x7a0e);
  CYC(0x7a0e, 0x7a10); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7a10, 0x7a11); E = L;
  CYC(0x7a11, 0x7a13); A = 0x78; // 120
  CYC(0x7a13, 0x7a14); mem_wr(gb, HL, A);
  CYC(0x7a14, 0x7a15); mem_wr(gb, DE, A);
  CYC(0x7a15, 0x7a18); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(0x7a18, 0x7a1a); A = 0x0f; // LINK_ANIM_MODE_GETITEM2HAND
  CYC(0x7a1a, 0x7a1d); mem_wr(gb, wcc50, A);
  CALL_C(0x7a1d, interactionDecCounter1_hook, 0x23cc, 0x7a20);
  if (!(F & FZ)) { CYCT(0x7a20, 0x7a21); ret_effect(gb); return; } // ret nz
  CYC(0x7a20, 0x7a21);
  CYC(0x7a21, 0x7a23); mem_wr(gb, HL, 0x40); // [counter1]
  CYC(0x7a23, 0x7a25); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7a25, 0x7a27); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x7a27, 0x7a2a); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(0x7a2a, objectApplySpeed_hook, 0x201d, 0x7a2d);
  CALL_C(0x7a2d, interactiond7_updateSmallSparkles_hook, 0x7c0f, 0x7a30);
  CALL_C(0x7a30, interactionDecCounter1_hook, 0x23cc, 0x7a33);
  if (!(F & FZ)) { CYCT(0x7a33, 0x7a34); ret_effect(gb); return; } // ret nz
  CYC(0x7a33, 0x7a34);
  CYC(0x7a34, 0x7a36); mem_wr(gb, HL, 0x78); // [counter1]
  CYC(0x7a36, 0x7a38); A = 0x10; // LINK_ANIM_MODE_WALK
  CYC(0x7a38, 0x7a3b); mem_wr(gb, wcc50, A);
  CYC(0x7a3b, 0x7a3d); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7a3d, 0x7a3f); mem_wr(gb, HL, 0x58);
  CYC(0x7a3f, 0x7a41); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7a41, 0x7a43); mem_wr(gb, HL, 0x78);
  CYC(0x7a43, 0x7a45); A = 0x8a; // SND_PIECE_OF_POWER
  CALL_C(0x7a45, playSound_b00_hook, 0x0c98, 0x7a48);
  CYC(0x7a48, 0x7a4a); A = 0x03;
  CALL_C(0x7a4a, fadeinFromWhiteWithDelay_hook, 0x3284, 0x7a4d);
  CYC(0x7a4d, 0x7a50); interactionIncState_hook(gb); return; // jp

state3:
  CALL_C(0x7a50, interactiond7_updateSmallSparkles_hook, 0x7c0f, 0x7a53);
  CALL_C(0x7a53, interactiond7_updateFloating_hook, 0x7c46, 0x7a56);
  CYC(0x7a56, 0x7a58); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7a58, 0x7a59); A = mem_rd(gb, DE);
  CYC(0x7a59, 0x7a5a); push_effect(gb, 0x7a5a);
  switch (interactiond7_jump_table(gb)) {
    case 0x7a6e: goto state3Substate0;
    case 0x7a7a: goto state3Substate1;
    case 0x7ab9: goto state3Substate2;
    case 0x7acc: goto state3Substate3;
    case 0x7ad9: goto state3Substate4;
    case 0x7aed: goto state3Substate8;
    case 0x7afe: goto state3Substate9;
    default: hook_continue(gb, HL, sp0_); return;
  }

state3Substate0:
  CALL_C(0x7a6e, interactionDecCounter1_hook, 0x23cc, 0x7a71);
  if (!(F & FZ)) { CYCT(0x7a71, 0x7a72); ret_effect(gb); return; } // ret nz
  CYC(0x7a71, 0x7a72);
  CYC(0x7a72, 0x7a74); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(0x7a74, 0x7a75); L = alu_inc8(gb, L);
  CYC(0x7a75, 0x7a77); mem_wr(gb, HL, 0x08); // [counter2]
  CYC(0x7a77, 0x7a7a); interactionIncSubstate_hook(gb); return; // jp

state3Substate1:
  CALL_C(0x7a7a, interactionDecCounter1_hook, 0x23cc, 0x7a7d);
  if (!(F & FZ)) { CYCT(0x7a7d, 0x7a7e); ret_effect(gb); return; } // ret nz
  CYC(0x7a7d, 0x7a7e);
  CYC(0x7a7e, 0x7a80); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(0x7a80, 0x7a81); L = alu_inc8(gb, L); // [counter2]
  CYC(0x7a81, 0x7a82); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x7a82, 0x7a83); B = mem_rd(gb, HL);
  CALL_C(0x7a83, getFreeInteractionSlot_hook, 0x3aef, 0x7a86);
  if (!(F & FZ)) { CYCT(0x7a86, 0x7a87); ret_effect(gb); return; } // ret nz
  CYC(0x7a86, 0x7a87);
  CYC(0x7a87, 0x7a89); mem_wr(gb, HL, 0xd7); // INTERAC_MAKU_SEED_AND_ESSENCES
  CALL_C(0x7a89, objectCopyPosition_hook, 0x2242, 0x7a8c);
  CYC(0x7a8c, 0x7a8d); A = B;
  CYC(0x7a8d, 0x7a90); SET_BC(0x7aa9); // @essenceSpawnerData
  CALL_C(0x7a90, addDoubleIndexToBc_hook, 0x007e, 0x7a93);
  CYC(0x7a93, 0x7a94); A = mem_rd(gb, BC);
  CYC(0x7a94, 0x7a96); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7a96, 0x7a97); mem_wr(gb, HL, A);
  CYC(0x7a97, 0x7a99); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7a99, 0x7a9a); SET_BC(BC + 1); // inc bc
  CYC(0x7a9a, 0x7a9b); A = mem_rd(gb, BC);
  CYC(0x7a9b, 0x7a9c); mem_wr(gb, HL, A);
  CYC(0x7a9c, 0x7a9e); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7a9e, 0x7a9f); A = mem_rd(gb, DE);
  CYC(0x7a9f, 0x7aa0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7aa0, 0x7aa1); ret_effect(gb); return; } // ret nz
  CYC(0x7aa0, 0x7aa1);
  CALL_C(0x7aa1, interactionIncSubstate_hook, 0x23e5, 0x7aa4);
  CYC(0x7aa4, 0x7aa6); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7aa6, 0x7aa8); mem_wr(gb, HL, 0x78); // 120
  RET(0x7aa8); return;

state3Substate2:
  CALL_C(0x7ab9, interactionDecCounter1_hook, 0x23cc, 0x7abc);
  if (!(F & FZ)) { CYCT(0x7abc, 0x7abd); ret_effect(gb); return; } // ret nz
  CYC(0x7abc, 0x7abd);
  CYC(0x7abd, 0x7abf); mem_wr(gb, HL, 0x3c); // [counter1], 60
  CYC(0x7abf, 0x7ac1); A = 0x01;
  CYC(0x7ac1, 0x7ac4); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x7ac4, 0x7ac6); A = 0x20;
  CYC(0x7ac6, 0x7ac9); mem_wr(gb, wTmpcfc0_genericCutscene_cfc1, A);
  CYC(0x7ac9, 0x7acc); interactionIncSubstate_hook(gb); return; // jp

state3Substate3: // also @state3Substate5, @state3Substate7
  CYC(0x7acc, 0x7acf); A = mem_rd(gb, wFrameCounter);
  CYC(0x7acf, 0x7ad1); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x7ad1, 0x7ad3); goto essenceRotationCommon; } // jr nz
  CYC(0x7ad1, 0x7ad3);
  CYC(0x7ad3, 0x7ad6); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(0x7ad6, 0x7ad7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYCT(0x7ad7, 0x7ad9); goto essenceRotationCommon; // jr

state3Substate4: // also @state3Substate6
  CYC(0x7ad9, 0x7adc); A = mem_rd(gb, wFrameCounter);
  CYC(0x7adc, 0x7ade); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x7ade, 0x7ae0); goto essenceRotationCommon; } // jr nz
  CYC(0x7ade, 0x7ae0);
  CYC(0x7ae0, 0x7ae3); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(0x7ae3, 0x7ae4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  // falls through into @essenceRotationCommon

essenceRotationCommon:
  CALL_C(0x7ae4, interactionDecCounter1_hook, 0x23cc, 0x7ae7);
  if (!(F & FZ)) { CYCT(0x7ae7, 0x7ae8); ret_effect(gb); return; } // ret nz
  CYC(0x7ae7, 0x7ae8);
  CYC(0x7ae8, 0x7aea); mem_wr(gb, HL, 0x3c); // [counter1], 60
  CYC(0x7aea, 0x7aed); interactionIncSubstate_hook(gb); return; // jp

state3Substate8:
  CYC(0x7aed, 0x7af0); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(0x7af0, 0x7af1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x7af1, 0x7af3); A = 0xb4; // SND_FADEOUT
  CALL_C(0x7af3, playSound_b00_hook, 0x0c98, 0x7af6);
  CYC(0x7af6, 0x7af8); A = 0x04;
  CALL_C(0x7af8, fadeoutToWhiteWithDelay_hook, 0x3257, 0x7afb);
  CYC(0x7afb, 0x7afe); interactionIncSubstate_hook(gb); return; // jp

state3Substate9:
  CYC(0x7afe, 0x7b01); SET_HL(wTmpcfc0_genericCutscene_cfc1);
  CYC(0x7b01, 0x7b02); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x7b02, 0x7b05); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7b05, 0x7b06); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7b06, 0x7b07); ret_effect(gb); return; } // ret nz
  CYC(0x7b06, 0x7b07);
  CALL_C(0x7b07, interactionIncState_hook, 0x23e0, 0x7b0a);
  CYC(0x7b0a, 0x7b0b); L = alu_inc8(gb, L);
  CYC(0x7b0b, 0x7b0d); mem_wr(gb, HL, 0x00);
  CYC(0x7b0d, 0x7b10); objectSetInvisible_hook(gb); return; // jp

state4:
  CYC(0x7b10, 0x7b12); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7b12, 0x7b13); A = mem_rd(gb, DE);
  CYC(0x7b13, 0x7b14); push_effect(gb, 0x7b14);
  switch (interactiond7_jump_table(gb)) {
    case 0x7b1e: goto state4Substate0;
    case 0x7b5b: goto state4Substate1;
    case 0x7b69: goto state4Substate2;
    case 0x7b76: goto state4Substate3;
    case 0x7b9f: goto state4Substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state4Substate0:
  CYC(0x7b1e, 0x7b21); SET_HL(0x7b36); // @tileReplacements

l_7b21:
  CYC(0x7b21, 0x7b22); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7b22, 0x7b23); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7b23, 0x7b25); goto l_7b2e; } // jr z
  CYC(0x7b23, 0x7b25);
  CYC(0x7b25, 0x7b26); C = mem_rd(gb, HL);
  CYC(0x7b26, 0x7b27); SET_HL(HL + 1); // inc hl
  CYC(0x7b27, 0x7b28); push_effect(gb, HL); // push hl
  CALL_C(0x7b28, setTile_hook, 0x3a9c, 0x7b2b);
  CYC(0x7b2b, 0x7b2c); SET_HL(pop_effect(gb)); // pop hl
  CYCT(0x7b2c, 0x7b2e); goto l_7b21; // jr

l_7b2e:
  CYC(0x7b2e, 0x7b30); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7b30, 0x7b32); A = 0x1e; // 30
  CYC(0x7b32, 0x7b33); mem_wr(gb, DE, A);
  CYC(0x7b33, 0x7b36); interactionIncSubstate_hook(gb); return; // jp

state4Substate1:
  CALL_C(0x7b5b, interactionDecCounter1_hook, 0x23cc, 0x7b5e);
  if (!(F & FZ)) { CYCT(0x7b5e, 0x7b5f); ret_effect(gb); return; } // ret nz
  CYC(0x7b5e, 0x7b5f);
  CYC(0x7b5f, 0x7b61); mem_wr(gb, HL, 0x78); // [counter1], 120
  CYC(0x7b61, 0x7b63); A = 0x08;
  CALL_C(0x7b63, fadeinFromWhiteWithDelay_hook, 0x3284, 0x7b66);
  CYC(0x7b66, 0x7b69); interactionIncSubstate_hook(gb); return; // jp

state4Substate2:
  CYC(0x7b69, 0x7b6c); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7b6c, 0x7b6d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7b6d, 0x7b6e); ret_effect(gb); return; } // ret nz
  CYC(0x7b6d, 0x7b6e);
  CYC(0x7b6e, 0x7b70); A = 0x5b; // SND_SOLVEPUZZLE_2
  CALL_C(0x7b70, playSound_b00_hook, 0x0c98, 0x7b73);
  CYC(0x7b73, 0x7b76); interactionIncSubstate_hook(gb); return; // jp

state4Substate3:
  CALL_C(0x7b76, interactionDecCounter1_hook, 0x23cc, 0x7b79);
  if (!(F & FZ)) { CYCT(0x7b79, 0x7b7a); ret_effect(gb); return; } // ret nz
  CYC(0x7b79, 0x7b7a);
  CALL_C(0x7b7a, getThisRoomFlags_hook, 0x197d, 0x7b7d);
  CYC(0x7b7d, 0x7b7f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6))); // set 6,(hl)
  CYC(0x7b7f, 0x7b82); SET_HL(wRoomLayout + 0x47);
  CYC(0x7b82, 0x7b84); mem_wr(gb, HL, 0x44);
  CALL_C(0x7b84, checkIsLinkedGame_hook, 0x1992, 0x7b87);
  if (F & FZ) { CYCT(0x7b87, 0x7b89); goto unlinkedGame; } // jr z
  CYC(0x7b87, 0x7b89);
  CALL_C(0x7b89, fadeoutToBlack_hook, 0x32c0, 0x7b8c);
  CYC(0x7b8c, 0x7b8f); interactionIncSubstate_hook(gb); return; // jp

unlinkedGame:
  CYC(0x7b8f, 0x7b90); alu_xor(gb, A);
  CYC(0x7b90, 0x7b93); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7b93, 0x7b96); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7b96, 0x7b99); A = mem_rd(gb, wActiveMusic);
  CALL_C(0x7b99, playSound_b00_hook, 0x0c98, 0x7b9c);
  CYC(0x7b9c, 0x7b9f); interactionDelete_hook(gb); return; // jp

state4Substate4:
  CYC(0x7b9f, 0x7ba2); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7ba2, 0x7ba3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7ba3, 0x7ba4); ret_effect(gb); return; } // ret nz
  CYC(0x7ba3, 0x7ba4);
  CYC(0x7ba4, 0x7ba6); A = 0x11; // CUTSCENE_FLAME_OF_SORROW
  CYC(0x7ba6, 0x7ba9); mem_wr(gb, wCutsceneTrigger, A);
  CYC(0x7ba9, 0x7bac); interactionDelete_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_MAKU_SEED_AND_ESSENCES
//
// Variables:
//   counter1: Essence index (for the maku seed / spawner object)
// ==================================================================================================
void interactionCoded7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79c2, 0x79c4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x79c4, 0x79c5); A = mem_rd(gb, DE);
  CYC(0x79c5, 0x79c6); push_effect(gb, 0x79c6);
  switch (interactiond7_jump_table(gb)) {
    case 0x79d8: interactiond7_makuSeed_hook(gb); return;
    case 0x7bac: interactiond7_essence_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
