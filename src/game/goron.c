#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void goronDance_updateFrameCounter_hook(GB *gb);
void goronDance_initNextRound_hook(GB *gb);
void goronDance_clearDanceVariables_hook(GB *gb);
void goronDance_checkLinkInput_hook(GB *gb);
void goronDance_checkInputNotTooEarlyOrLate_hook(GB *gb);
void goronDance_checkTooLateToInput_hook(GB *gb);
void goronDance_checkExactInputTimePassed_hook(GB *gb);
void goronDance_getCurrentAndNeededFrameCounts_hook(GB *gb);
void goronDance_playMoveSound_hook(GB *gb);
void goronDance_incBeat_hook(GB *gb);
void goronDance_getNextMove_hook(GB *gb);
void goronDance_updateConsecutiveBPressCounter_hook(GB *gb);
void goronDance_updateLinkAndBackupDancerAnimation_hook(GB *gb);
void goronDance_turnLinkToDirection_hook(GB *gb);
void goronDance_updateBackupDancerAnimation_hook(GB *gb);
void goronDance_updateGracefulGoronAnimation_hook(GB *gb);
void goronSubid03_hook(GB *gb);
void goronSubid04__afterCall780f_hook(GB *gb);
void goronSubid05_hook(GB *gb);
void goronSubid05__afterCall7823_hook(GB *gb);
void goronSubid06_hook(GB *gb);
void goronSubid06__afterCall7830_hook(GB *gb);
void goronSubid07_hook(GB *gb);
void goronSubid10__afterCall7854_hook(GB *gb);
void goron_runScriptAndDeleteWhenFinished_hook(GB *gb);
void goron_faceLinkAndAnimateIfNotNapping_hook(GB *gb);
void goronSubid09_hook(GB *gb);
void goronSubid09__afterCall7875_hook(GB *gb);
void goronSubid09__afterCall7893_hook(GB *gb);
void goronSubid0b_hook(GB *gb);
void goronSubid0b__afterCall78a2_hook(GB *gb);
void goronSubid0f_hook(GB *gb);
void goronSubid0f__afterCall78ba_hook(GB *gb);
void goron_initGraphicsAndIncState_hook(GB *gb);
void goron_initGraphicsAndIncState__afterCall7d75_hook(GB *gb);
void goron_loadScriptAndInitGraphics_hook(GB *gb);
void goron_loadScriptAndInitGraphics__afterCall7d7b_hook(GB *gb);
void goron_loadScriptFromTableAndInitGraphics_hook(GB *gb);
void goron_loadScriptFromTableAndInitGraphics__afterCall7d80_hook(GB *gb);
void goron_initGraphics_hook(GB *gb);
void goron_loadScript_hook(GB *gb);
void goron_loadScriptFromTable_hook(GB *gb);

static uint16_t goron_jump_table(GB *gb) {
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

static void goron_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void goron_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void interactionCode66_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x754e, 0x7550); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7550, 0x7551); A = mem_rd(gb, DE);
  CYC(0x7551, 0x7552); push_effect(gb, 0x7552);
  switch (goron_jump_table(gb)) {
    case 0x7574: goronSubid00(gb); return;
    case 0x776b: goronSubid01(gb); return;
    case 0x77b5: goronSubid02_hook(gb); return;
    case 0x7807: goronSubid03_hook(gb); return;
    case 0x781b: goronSubid05_hook(gb); return;
    case 0x7828: goronSubid06_hook(gb); return;
    case 0x784c: goronSubid07_hook(gb); return;
    case 0x7867: goronSubid09_hook(gb); return;
    case 0x789a: goronSubid0b_hook(gb); return;
    case 0x78b2: goronSubid0f_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void goronDance_updateFrameCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x78d0, 0x78d3); A = W8(wTmpcfc0_goronDance_linkStartedDance);
  CYC(0x78d3, 0x78d4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x78d4, 0x78d5); ret_effect(gb); return; }
  CYC(0x78d4, 0x78d5);
  CYC(0x78d5, 0x78d8); SET_HL(wTmpcfc0_goronDance_frameCounter);
  CYC(0x78d8, 0x78db); incHlRef16WithCap_hook(gb);
}

void goronDance_initNextRound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78db, 0x78de); A = W8(wTmpcfc0_goronDance_remainingRounds);
  CYC(0x78de, 0x78df); alu_or(gb, A);
  if (F & FZ) { CYCT(0x78df, 0x78e1); goronDance_clearDanceVariables_hook(gb); return; }
  CYC(0x78df, 0x78e1);
  CYC(0x78e1, 0x78e4); SET_HL(0x5793);
  CYC(0x78e4, 0x78e6); E = 0x08;
  CALL_C(0x78e6, interBankCall_hook, 0x008a, 0x78e9);
  goronDance_clearDanceVariables_hook(gb);
}

void goronDance_clearDanceVariables_hook(GB *gb) {
  CYC(0x78e9, 0x78ea); alu_xor(gb, A);
  CYC(0x78ea, 0x78ed); W8(wTmpcfc0_goronDance_linkJumping) = A;
  CYC(0x78ed, 0x78f0); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(0x78f0, 0x78f3); W8(wTmpcfc0_goronDance_frameCounter) = A;
  CYC(0x78f3, 0x78f6); mem_wr(gb, wTmpcfc0_goronDance_frameCounter + 1, A);
  CYC(0x78f6, 0x78f9); W8(wTmpcfc0_goronDance_currentMove) = A;
  CYC(0x78f9, 0x78fc); W8(wTmpcfc0_goronDance_consecutiveBPressCounter) = A;
  CYC(0x78fc, 0x78ff); W8(wTmpcfc0_goronDance_cfd9) = A;
  CYC(0x78ff, 0x7902); W8(wTmpcfc0_goronDance_beat) = A;
  CYC(0x7902, 0x7903); ret_effect(gb);
}

void goronDance_getCurrentAndNeededFrameCounts_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79ed, 0x79f0); A = W8(wTmpcfc0_goronDance_beat);
  CYC(0x79f0, 0x79f1); push_effect(gb, AF);
  CALL_C(0x79f1, multiplyABy4_hook, 0x01c3, 0x79f4);
  CYC(0x79f4, 0x79f5); L = C;
  CYC(0x79f5, 0x79f6); H = B;
  CYC(0x79f6, 0x79f7); SET_AF(pop_effect(gb));
  CALL_C(0x79f7, multiplyABy16_hook, 0x01ac, 0x79fa);
  CYC(0x79fa, 0x79fb); alu_add_hl(gb, BC);
  CYC(0x79fb, 0x79fe); A = W8(wTmpcfc0_goronDance_frameCounter);
  CYC(0x79fe, 0x79ff); C = A;
  CYC(0x79ff, 0x7a02); A = mem_rd(gb, wTmpcfc0_goronDance_frameCounter + 1);
  CYC(0x7a02, 0x7a03); B = A;
  CYC(0x7a03, 0x7a04); ret_effect(gb);
}

void goronDance_checkInputNotTooEarlyOrLate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79ac, goronDance_getCurrentAndNeededFrameCounts_hook, 0x79ed, 0x79af);
  CYC(0x79af, 0x79b1); A = 8;
  CYC(0x79b1, 0x79b2); goron_add_a_to_hl(gb, 0x79b2);
  CYC(0x79b2, 0x79b4); A = 8;
  CALL_C(0x79b4, addAToBc_hook, 0x006d, 0x79b7);
  CYC(0x79b7, 0x79b8); push_effect(gb, BC);
  CYC(0x79b8, 0x79ba); B = 0xff;
  CYC(0x79ba, 0x79bc); C = 0xf8;
  CYC(0x79bc, 0x79bd); alu_add_hl(gb, BC);
  CYC(0x79bd, 0x79be); SET_BC(pop_effect(gb));
  CALL_C(0x79be, compareHlToBc_hook, 0x01d6, 0x79c1);
  CYC(0x79c1, 0x79c3); alu_cp(gb, 1);
  if (F & FZ) { CYCT(0x79c3, 0x79c5); goto too_early; }
  CYC(0x79c3, 0x79c5);
  CYC(0x79c5, 0x79c7); A = 0x10;
  CYC(0x79c7, 0x79c8); goron_add_a_to_hl(gb, 0x79c8);
  CALL_C(0x79c8, compareHlToBc_hook, 0x01d6, 0x79cb);
  CYC(0x79cb, 0x79cd); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x79cd, 0x79cf); goto too_late; }
  CYC(0x79cd, 0x79cf); ret_effect(gb); return;
too_early:
  CYC(0x79d0, 0x79d2); A = 0;
  CYC(0x79d2, 0x79d5); W8(wTmpcfc0_goronDance_failureType) = A;
  CYC(0x79d5, 0x79d6); ret_effect(gb); return;
too_late:
  CYC(0x79d6, 0x79d8); A = 1;
  CYC(0x79d8, 0x79db); W8(wTmpcfc0_goronDance_failureType) = A;
  CYC(0x79db, 0x79dc); ret_effect(gb);
}

void goronDance_checkTooLateToInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79dc, goronDance_getCurrentAndNeededFrameCounts_hook, 0x79ed, 0x79df);
  CYC(0x79df, 0x79e1); A = 8;
  CYC(0x79e1, 0x79e2); goron_add_a_to_hl(gb, 0x79e2);
  CYC(0x79e2, 0x79e4);
  CALL_C(0x79e7, compareHlToBc_hook, 0x01d6, 0x79ea);
  CYC(0x79ea, 0x79ec); alu_cp(gb, 0xff);
  CYC(0x79ec, 0x79ed); ret_effect(gb);
}

void goronDance_checkExactInputTimePassed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79e4, goronDance_getCurrentAndNeededFrameCounts_hook, 0x79ed, 0x79e7);
  CALL_C(0x79e7, compareHlToBc_hook, 0x01d6, 0x79ea);
  CYC(0x79ea, 0x79ec); alu_cp(gb, 0xff);
  CYC(0x79ec, 0x79ed); ret_effect(gb);
}

void goronDance_playMoveSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x7a04, 0x7a07); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x7a07, 0x7a09); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x7a09, 0x7a0a); ret_effect(gb); return; }
  CYC(0x7a09, 0x7a0a);
  CYC(0x7a0a, 0x7a0c); alu_cp(gb, 0);
  if (F & FZ) { CYCT(0x7a0c, 0x7a0d); ret_effect(gb); return; }
  CYC(0x7a0c, 0x7a0d);
  CYC(0x7a0d, 0x7a0f); alu_cp(gb, 2);
  if (F & FZ) { CYCT(0x7a0f, 0x7a11); A = 0xcd; CYC(0x7a16, 0x7a1b); playSound_b00_hook(gb); return; }
  CYC(0x7a0f, 0x7a11);
  CYC(0x7a11, 0x7a13); A = 0xc8;
  CYC(0x7a13, 0x7a16); playSound_b00_hook(gb);
}

void goronDance_incBeat_hook(GB *gb) {
  CYC(0x7a1b, 0x7a1e); SET_HL(wTmpcfc0_goronDance_beat);
  CYC(0x7a1e, 0x7a1f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7a1f, 0x7a20); ret_effect(gb);
}

void goronDance_getNextMove_hook(GB *gb) {
  CYC(0x7a20, 0x7a23); A = W8(wTmpcfc0_goronDance_danceLevel);
  CYC(0x7a23, 0x7a26); SET_HL(0x7aea);
  CYC(0x7a26, 0x7a27); goron_add_double_index(gb, 0x7a27);
  CYC(0x7a27, 0x7a28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7a28, 0x7a29); H = mem_rd(gb, HL);
  CYC(0x7a29, 0x7a2a); L = A;
  CYC(0x7a2a, 0x7a2d); A = W8(wTmpcfc0_goronDance_dancePattern);
  CYC(0x7a2d, 0x7a2f); A = alu_swap(gb, A);
  CYC(0x7a2f, 0x7a30); B = A;
  CYC(0x7a30, 0x7a33); A = W8(wTmpcfc0_goronDance_beat);
  CYC(0x7a33, 0x7a34); alu_add(gb, B);
  CYC(0x7a34, 0x7a35); goron_add_a_to_hl(gb, 0x7a35);
  CYC(0x7a35, 0x7a36); A = mem_rd(gb, HL);
  CYC(0x7a36, 0x7a39); W8(wTmpcfc0_goronDance_currentMove) = A;
  CYC(0x7a39, 0x7a3b); alu_bit(gb, 7, A);
  CYC(0x7a3b, 0x7a3c); ret_effect(gb);
}

void goronDance_updateConsecutiveBPressCounter_hook(GB *gb) {
  CYC(0x7a3c, 0x7a3f); SET_HL(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(0x7a3f, 0x7a42); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x7a42, 0x7a44); alu_cp(gb, 2);
  if (F & FZ) { CYCT(0x7a44, 0x7a46); CYC(0x7a49, 0x7a4a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(0x7a4a, 0x7a4b); ret_effect(gb); return; }
  CYC(0x7a44, 0x7a46);
  CYC(0x7a46, 0x7a48); mem_wr(gb, HL, 0);
  CYC(0x7a48, 0x7a49); ret_effect(gb);
}

void goronDance_turnLinkToDirection_hook(GB *gb) {
  CYC(0x7a72, 0x7a75); SET_HL(w1Link_direction);
  CYC(0x7a75, 0x7a76); mem_wr(gb, HL, A);
  CYC(0x7a76, 0x7a78); A = 0x10;
  CYC(0x7a78, 0x7a7b); W8(wcc50) = A;
  CYC(0x7a7b, 0x7a7c); alu_or(gb, D);
  CYC(0x7a7c, 0x7a7d); ret_effect(gb);
}

void goronDance_updateBackupDancerAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a83, checkIsLinkedGame_hook, 0x1992, 0x7a86);
  if (F & FZ) { CYCT(0x7a86, 0x7a88); goto gorons; }
  CYC(0x7a86, 0x7a88);
  CYC(0x7a88, 0x7a8b); A = W8(wTilesetFlags);
  CYC(0x7a8b, 0x7a8d); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x7a8d, 0x7a8f); goto gorons; }
  CYC(0x7a8d, 0x7a8f);
  CYC(0x7a8f, 0x7a92); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x7a92, 0x7a94); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x7a94, 0x7a96); goto subrosian_b; }
  CYC(0x7a94, 0x7a96);
  CYC(0x7a96, 0x7a98); A = 6;
  CYC(0x7a98, 0x7a9a); goto set_animation;
subrosian_b:
  CYC(0x7a9a, 0x7a9d); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(0x7a9d, 0x7aa0); SET_HL(0x7ac9);
  CYC(0x7aa0, 0x7aa1); goron_add_a_to_hl(gb, 0x7aa1);
  CYC(0x7aa1, 0x7aa2); A = mem_rd(gb, HL);
  CYC(0x7aa2, 0x7aa4); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(0x7aa4, 0x7aa5); ret_effect(gb); return; }
  CYC(0x7aa4, 0x7aa5);
  CYC(0x7aa5, 0x7aa8); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(0x7aa8, 0x7aa9); ret_effect(gb); return;
gorons:
  CYC(0x7aa9, 0x7aac); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x7aac, 0x7aae); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x7aae, 0x7ab0); goto goron_b; }
  CYC(0x7aae, 0x7ab0);
  CYC(0x7ab0, 0x7ab2); A = 6;
  CYC(0x7ab2, 0x7ab4); goto set_animation;
goron_b:
  CYC(0x7ab4, 0x7ab7); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(0x7ab7, 0x7aba); SET_HL(0x7ac3);
  CYC(0x7aba, 0x7abb); goron_add_a_to_hl(gb, 0x7abb);
  CYC(0x7abb, 0x7abc); A = mem_rd(gb, HL);
  CYC(0x7abc, 0x7abe); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(0x7abe, 0x7abf); ret_effect(gb); return; }
  CYC(0x7abe, 0x7abf);
set_animation:
  CYC(0x7abf, 0x7ac2); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(0x7ac2, 0x7ac3); ret_effect(gb);
}

void goronDance_updateLinkAndBackupDancerAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a4b, goronDance_updateBackupDancerAnimation_hook, 0x7a83, 0x7a4e);
  CYC(0x7a4e, 0x7a51); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x7a51, 0x7a53); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x7a53, 0x7a55); goto b_button; }
  CYC(0x7a53, 0x7a55);
  CYC(0x7a55, 0x7a57); A = 8;
  CYC(0x7a57, 0x7a5a); W8(wcc50) = A;
  CYC(0x7a5a, 0x7a5b); alu_or(gb, D);
  CYC(0x7a5b, 0x7a5c); ret_effect(gb); return;
b_button:
  CYC(0x7a5c, 0x7a5f); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(0x7a5f, 0x7a62); SET_HL(0x7a7d);
  CYC(0x7a62, 0x7a63); goron_add_a_to_hl(gb, 0x7a63);
  CYC(0x7a63, 0x7a64); A = mem_rd(gb, HL);
  CYC(0x7a64, 0x7a66); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(0x7a66, 0x7a67); ret_effect(gb); return; }
  CYC(0x7a66, 0x7a67);
  CYC(0x7a67, 0x7a69); alu_cp(gb, 4);
  if (!(F & FZ)) { CYCT(0x7a69, 0x7a6b); goronDance_turnLinkToDirection_hook(gb); return; }
  CYC(0x7a69, 0x7a6b);
  CYC(0x7a6b, 0x7a6d); A = 0x0e;
  CYC(0x7a6d, 0x7a70); W8(wcc50) = A;
  CYC(0x7a70, 0x7a71); alu_or(gb, D);
  CYC(0x7a71, 0x7a72); ret_effect(gb);
}

void goronDance_updateGracefulGoronAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7acf, 0x7ad2); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x7ad2, 0x7ad4); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x7ad4, 0x7ad6); goto b_button; }
  CYC(0x7ad4, 0x7ad6);
  CYC(0x7ad6, 0x7ad8); A = 6;
  CYC(0x7ad8, 0x7ada); goto set_animation;
b_button:
  CYC(0x7ada, 0x7add); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(0x7add, 0x7ae0); SET_HL(0x7ac3);
  CYC(0x7ae0, 0x7ae1); goron_add_a_to_hl(gb, 0x7ae1);
  CYC(0x7ae1, 0x7ae2); A = mem_rd(gb, HL);
  CYC(0x7ae2, 0x7ae4); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(0x7ae4, 0x7ae5); ret_effect(gb); return; }
  CYC(0x7ae4, 0x7ae5);
set_animation:
  CALL_C(0x7ae5, interactionSetAnimation_hook, 0x262e, 0x7ae8);
  CYC(0x7ae8, 0x7ae9); alu_or(gb, D);
  CYC(0x7ae9, 0x7aea); ret_effect(gb);
}

static void goron_dance_made_mistake(GB *gb, uint16_t sp0_) {
  CYC(0x7940, 0x7941); H = D;
  CYC(0x7941, 0x7943); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7943, 0x7945); mem_wr(gb, HL, 4);
  CYC(0x7945, 0x7947); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7947, 0x7949); mem_wr(gb, HL, 0);
  CYC(0x7949, 0x794b); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x794b, 0x794d); mem_wr(gb, HL, 30);
  CYC(0x794d, 0x794f); A = 0x5a;
  CALL_C(0x794f, playSound_b00_hook, 0x0c98, 0x7952);
  CYC(0x7952, 0x7954); A = 2;
  CYC(0x7954, 0x7957); W8(wcc50) = A;
  CALL_C(0x7957, checkIsLinkedGame_hook, 0x1992, 0x795a);
  if (F & FZ) { CYCT(0x795a, 0x795c); goto gorons; }
  CYC(0x795a, 0x795c);
  CYC(0x795c, 0x795f); A = W8(wTilesetFlags);
  CYC(0x795f, 0x7961); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x7961, 0x7963); goto gorons; }
  CYC(0x7961, 0x7963);
  CYC(0x7963, 0x7965); A = 2;
  CYC(0x7965, 0x7968); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(0x7968, 0x7969); ret_effect(gb); return;
gorons:
  CYC(0x7969, 0x796b); A = 4;
  CYC(0x796b, 0x796e); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(0x796e, 0x796f); ret_effect(gb);
}

void goronDance_checkLinkInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7903, goronDance_getNextMove_hook, 0x7a20, 0x7906);
  CYC(0x7906, 0x7908); alu_cp(gb, 0);
  if (F & FZ) { CYCT(0x7908, 0x790a); goto rest; }
  CYC(0x7908, 0x790a);
  CALL_C(0x790a, goronDance_checkTooLateToInput_hook, 0x79dc, 0x790d);
  if (F & FZ) { CYCT(0x790d, 0x790f); goto too_late; }
  CYC(0x790d, 0x790f);
  CYC(0x790f, 0x7912); A = W8(wGameKeysJustPressed);
  CYC(0x7912, 0x7914); alu_and(gb, 3);
  if (F & FZ) { CYCT(0x7914, 0x7915); ret_effect(gb); return; }
  CYC(0x7914, 0x7915);
  CYC(0x7915, 0x7916); B = A;
  CYC(0x7916, 0x7919); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(0x7919, 0x791c); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(0x791c, 0x791d); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x791d, 0x791f); goto wrong_move; }
  CYC(0x791d, 0x791f);
  CALL_C(0x791f, goronDance_checkInputNotTooEarlyOrLate_hook, 0x79ac, 0x7922);
  if (F & FZ) { CYCT(0x7922, 0x7924); goron_dance_made_mistake(gb, sp0_); return; }
  CYC(0x7922, 0x7924);
  CYC(0x7924, 0x7927); goto do_dance_move;
rest:
  CALL_C(0x7927, goronDance_checkExactInputTimePassed_hook, 0x79e4, 0x792a);
  if (F & FZ) { CYCT(0x792a, 0x792c); goto do_dance_move; }
  CYC(0x792a, 0x792c);
  CYC(0x792c, 0x792f); A = W8(wGameKeysJustPressed);
  CYC(0x792f, 0x7931); alu_and(gb, 3);
  if (!(F & FZ)) { CYCT(0x7931, 0x7933); goto wrong_move; }
  CYC(0x7931, 0x7933);
  CYC(0x7933, 0x7934); ret_effect(gb); return;
too_late:
  CYC(0x7934, 0x7936); A = 1;
  CYC(0x7936, 0x7939); W8(wTmpcfc0_goronDance_failureType) = A;
  CYC(0x7939, 0x793b); goto made_mistake;
wrong_move:
  CYC(0x793b, 0x793d); A = 2;
  CYC(0x793d, 0x7940); W8(wTmpcfc0_goronDance_failureType) = A;
made_mistake:
  goron_dance_made_mistake(gb, sp0_); return;
do_dance_move:
  CALL_C(0x796f, goronDance_updateConsecutiveBPressCounter_hook, 0x7a3c, 0x7972);
  CALL_C(0x7972, goronDance_updateLinkAndBackupDancerAnimation_hook, 0x7a4b, 0x7975);
  if (F & FZ) { CYCT(0x7975, 0x7977); goto jump; }
  CYC(0x7975, 0x7977);
  CALL_C(0x7977, goronDance_playMoveSound_hook, 0x7a04, 0x797a);
  CALL_C(0x797a, goronDance_incBeat_hook, 0x7a1b, 0x797d);
  CALL_C(0x797d, goronDance_getNextMove_hook, 0x7a20, 0x7980);
  if (!(F & FZ)) { CYCT(0x7980, 0x7982); goto round_finished; }
  CYC(0x7980, 0x7982);
  CYC(0x7982, 0x7983); ret_effect(gb); return;
jump:
  CALL_C(0x7983, goronDance_incBeat_hook, 0x7a1b, 0x7986);
  CALL_C(0x7986, goronDance_getNextMove_hook, 0x7a20, 0x7989);
  CALL_C(0x7989, getFreeInteractionSlot_hook, 0x3aef, 0x798c);
  if (!(F & FZ)) { CYCT(0x798c, 0x798d); ret_effect(gb); return; }
  CYC(0x798c, 0x798d);
  CYC(0x798d, 0x798f); mem_wr(gb, HL, 0x66);
  CYC(0x798f, 0x7990); L = alu_inc8(gb, L);
  CYC(0x7990, 0x7992); mem_wr(gb, HL, 2);
  CYC(0x7992, 0x7994); A = 1;
  CYC(0x7994, 0x7997); W8(wTmpcfc0_goronDance_linkJumping) = A;
  CYC(0x7997, 0x799a); interactionIncSubstate_hook(gb); return;
round_finished:
  CYC(0x799a, 0x799b); alu_xor(gb, A);
  CYC(0x799b, 0x799e); W8(wTmpcfc0_goronDance_cfd9) = A;
  CYC(0x799e, 0x79a1); SET_HL(wTmpcfc0_goronDance_roundIndex);
  CYC(0x79a1, 0x79a2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x79a2, 0x79a3); H = D;
  CYC(0x79a3, 0x79a5); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x79a5, 0x79a7); mem_wr(gb, HL, 3);
  CYC(0x79a7, 0x79a9); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x79a9, 0x79ab); mem_wr(gb, HL, 30);
  CYC(0x79ab, 0x79ac); ret_effect(gb);
}

void goronSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77b5, checkInteractionState_hook, 0x23fe, 0x77b8);
  if (!(F & FZ)) { CYCT(0x77b8, 0x77ba); goto state1; }
  CYC(0x77b8, 0x77ba);
  CALL_C(0x77ba, objectSetInvisible_hook, 0x1e7b, 0x77bd);
  CALL_C(0x77bd, interactionIncState_hook, 0x23e0, 0x77c0);
  CYC(0x77c0, 0x77c2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x77c2, 0x77c4); mem_wr(gb, HL, 0x28);
  CYC(0x77c4, 0x77c6); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x77c6, 0x77c8); mem_wr(gb, HL, 0);
  CYC(0x77c8, 0x77c9); SET_HL(HL + 1);
  CYC(0x77c9, 0x77cb); mem_wr(gb, HL, 0xfe);
  CYC(0x77cb, 0x77cd); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x77cd, 0x77cf); mem_wr(gb, HL, 20);
  CYC(0x77cf, 0x77d2); SET_HL(w1Link_yh);
  CALL_C(0x77d2, objectTakePosition_hook, 0x2274, 0x77d5);
  CYC(0x77d5, 0x77d7); A = 0;
  CYC(0x77d7, 0x77da); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(0x77da, goronDance_turnLinkToDirection_hook, 0x7a72, 0x77dd);
  CYC(0x77dd, 0x77df); A = 0xcd;
  CALL_C(0x77df, playSound_b00_hook, 0x0c98, 0x77e2);
state1:
  CYC(0x77e2, 0x77e4); C = 0x40;
  CALL_C(0x77e4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x77e7);
  if (F & FZ) { CYCT(0x77e7, 0x77e9); goto landed; }
  CYC(0x77e7, 0x77e9);
  CYC(0x77e9, 0x77ec); SET_HL(w1Link_yh);
  CALL_C(0x77ec, objectCopyPosition_hook, 0x2242, 0x77ef);
  CYC(0x77ef, 0x77f0); H = D;
  CYC(0x77f0, 0x77f2); L = INTERACTION_BASE + OBJ_SPEED_Z + 1;
  CYC(0x77f2, 0x77f3); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x77f3, 0x77f4); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x77f4, 0x77f5); ret_effect(gb); return; }
  CYC(0x77f4, 0x77f5);
  CYC(0x77f5, 0x77f7); A = 2;
  CYC(0x77f7, 0x77fa); goronDance_turnLinkToDirection_hook(gb);
  return;
landed:
  CYC(0x77fa, 0x77fd); SET_HL(w1Link_yh);
  CALL_C(0x77fd, objectCopyPosition_hook, 0x2242, 0x7800);
  CYC(0x7800, 0x7801); alu_xor(gb, A);
  CYC(0x7801, 0x7804); W8(wTmpcfc0_goronDance_linkJumping) = A;
  CYC(0x7804, 0x7807); interactionDelete_hook(gb);
}

static void goron_run_script_and_delete(GB *gb, uint16_t sp0_) {
  CALL_C(0x7857, interactionRunScript_hook, 0x2552, 0x785a);
  if (F & FC) { CYCT(0x785a, 0x785d); interactionDelete_hook(gb); return; }
  CYC(0x785a, 0x785d); goron_faceLinkAndAnimateIfNotNapping_hook(gb);
}

void goronSubid04__afterCall780f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x780f, interactionRunScript_hook, 0x2552, 0x7812);
  CALL_C(0x7812, interactionRunScript_hook, 0x2552, 0x7815);
  if (F & FC) { CYCT(0x7815, 0x7818); interactionDelete_hook(gb); return; }
  CYC(0x7815, 0x7818); CYC(0x7818, 0x781b); interactionAnimateAsNpc_hook(gb);
}

void goronSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7807, checkInteractionState_hook, 0x23fe, 0x780a);
  if (!(F & FZ)) { CYCT(0x780a, 0x780c); CALL_C(0x7812, interactionRunScript_hook, 0x2552, 0x7815); if (F & FC) { CYCT(0x7815, 0x7818); interactionDelete_hook(gb); return; } CYC(0x7815, 0x7818); CYC(0x7818, 0x781b); interactionAnimateAsNpc_hook(gb); return; }
  CYC(0x780a, 0x780c);
  CALL_C(0x780c, goron_loadScriptAndInitGraphics_hook, 0x7d78, 0x780f);
  goronSubid04__afterCall780f_hook(gb);
}

void goronSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x781b, checkInteractionState_hook, 0x23fe, 0x781e);
  if (!(F & FZ)) { CYCT(0x781e, 0x7820); CYC(0x7826, 0x7828); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(0x781e, 0x7820);
  CALL_C(0x7820, goron_loadScriptFromTableAndInitGraphics_hook, 0x7d7d, 0x7823);
  goronSubid05__afterCall7823_hook(gb);
}

void goronSubid05__afterCall7823_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7823, interactionRunScript_hook, 0x2552, 0x7826);
  CYC(0x7826, 0x7828); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7828, checkInteractionState_hook, 0x23fe, 0x782b);
  if (!(F & FZ)) { CYCT(0x782b, 0x782d); CYC(0x784a, 0x784c); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(0x782b, 0x782d);
  CALL_C(0x782d, goron_loadScriptFromTableAndInitGraphics_hook, 0x7d7d, 0x7830);
  goronSubid06__afterCall7830_hook(gb);
}

void goronSubid06__afterCall7830_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7830, 0x7832); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x7832, 0x7834); mem_wr(gb, HL, 10);
  CYC(0x7834, 0x7836); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7836, 0x7837); A = mem_rd(gb, DE);
  CYC(0x7837, 0x7838); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7838, 0x783a); goto clear; }
  CYC(0x7838, 0x783a);
  CYC(0x783a, 0x783d); W8(wTmpcfc0_goronCutscenes_elderVar_cfdd) = A;
  CYC(0x783d, 0x783f); goto run;
clear:
  CYC(0x783f, 0x7841); B = 0x20;
  CYC(0x7841, 0x7844); SET_HL(wTmpcfc0_goronCutscenes);
  CALL_C(0x7844, clearMemory_hook, 0x046f, 0x7847);
run:
  CALL_C(0x7847, interactionRunScript_hook, 0x2552, 0x784a);
  CYC(0x784a, 0x784c); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x784c, checkInteractionState_hook, 0x23fe, 0x784f);
  if (!(F & FZ)) { CYCT(0x784f, 0x7851); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(0x784f, 0x7851);
  CALL_C(0x7851, goron_loadScriptAndInitGraphics_hook, 0x7d78, 0x7854);
  goronSubid10__afterCall7854_hook(gb);
}

void goronSubid10__afterCall7854_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7854, interactionRunScript_hook, 0x2552, 0x7857);
  goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goron_runScriptAndDeleteWhenFinished_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goron_run_script_and_delete(gb, sp0_);
}

void goron_faceLinkAndAnimateIfNotNapping_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x785d, 0x785f); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x785f, 0x7860); A = mem_rd(gb, DE);
  CYC(0x7860, 0x7861); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7861, 0x7864); npcFaceLinkAndAnimate_hook(gb); return; }
  CYC(0x7861, 0x7864); CYC(0x7864, 0x7867); interactionAnimateAsNpc_hook(gb);
}

void goronSubid09__afterCall7875_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7875, 0x7876); alu_xor(gb, A);
  CYC(0x7876, 0x7879); W8(wTmpcfc0_targetCarts_cfdf) = A;
  CYC(0x7879, 0x787c); W8(wTmpcfc0_targetCarts_beginGameTrigger) = A;
  CALL_C(0x787c, getThisRoomFlags_hook, 0x197d, 0x787f);
  CYC(0x787f, 0x7881); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7881, 0x7883); goto run; }
  CYC(0x7881, 0x7883);
  CYC(0x7883, 0x7886); SET_HL(0x6851);
  CYC(0x7886, 0x7888); E = 0x15;
  CALL_C(0x7888, interBankCall_hook, 0x008a, 0x788b);
run:
  CALL_C(0x788b, interactionRunScript_hook, 0x2552, 0x788e);
  CYC(0x788e, 0x7890); CYC(0x7898, 0x789a); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid09__afterCall7893_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7893, interactionRunScript_hook, 0x2552, 0x7896);
  CYC(0x7896, 0x7898); CYC(0x7898, 0x789a); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7867, checkInteractionState_hook, 0x23fe, 0x786a);
  if (!(F & FZ)) { CYCT(0x786a, 0x786c); CYC(0x7898, 0x789a); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(0x786a, 0x786c);
  CYC(0x786c, 0x786e); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x786e, 0x786f); A = mem_rd(gb, DE);
  CYC(0x786f, 0x7870); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7870, 0x7872); goto right; }
  CYC(0x7870, 0x7872);
  CALL_C(0x7872, goron_loadScriptFromTableAndInitGraphics_hook, 0x7d7d, 0x7875);
  goronSubid09__afterCall7875_hook(gb); return;
right:
  CALL_C(0x7890, goron_loadScriptFromTableAndInitGraphics_hook, 0x7d7d, 0x7893);
  goronSubid09__afterCall7893_hook(gb);
}

void goronSubid0b__afterCall78a2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78a2, interactionRunScript_hook, 0x2552, 0x78a5);
  CALL_C(0x78a5, interactionRunScript_hook, 0x2552, 0x78a8);
  if (F & FC) { CYCT(0x78a8, 0x78ab); interactionDelete_hook(gb); return; }
  CYC(0x78a8, 0x78ab);
  CYC(0x78ab, 0x78ad); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x78ad, 0x78ae); A = mem_rd(gb, DE);
  CYC(0x78ae, 0x78af); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x78af, 0x78b0); ret_effect(gb); return; }
  CYC(0x78af, 0x78b0); CYC(0x78b0, 0x78b2); goron_faceLinkAndAnimateIfNotNapping_hook(gb);
}

void goronSubid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x789a, checkInteractionState_hook, 0x23fe, 0x789d);
  if (!(F & FZ)) { CYCT(0x789d, 0x789f); goronSubid0b__afterCall78a2_hook(gb); return; }
  CYC(0x789d, 0x789f);
  CALL_C(0x789f, goron_loadScriptFromTableAndInitGraphics_hook, 0x7d7d, 0x78a2);
  goronSubid0b__afterCall78a2_hook(gb);
}

void goronSubid0f__afterCall78ba_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78ba, 0x78bc); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x78bc, 0x78be); mem_wr(gb, HL, 8);
  CYC(0x78be, 0x78c1); SET_HL(0x7ed9);
  CALL_C(0x78c1, interactionSetScript_hook, 0x2544, 0x78c4);
  CALL_C(0x78c4, interactionRunScript_hook, 0x2552, 0x78c7);
  CALL_C(0x78c7, interactionRunScript_hook, 0x2552, 0x78ca);
  if (F & FC) { CYCT(0x78ca, 0x78cd); interactionDelete_hook(gb); return; }
  CYC(0x78ca, 0x78cd); CYC(0x78cd, 0x78d0); npcFaceLinkAndAnimate_hook(gb);
}

void goronSubid0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78b2, checkInteractionState_hook, 0x23fe, 0x78b5);
  if (!(F & FZ)) { CYCT(0x78b5, 0x78b7); CALL_C(0x78c7, interactionRunScript_hook, 0x2552, 0x78ca); if (F & FC) { CYCT(0x78ca, 0x78cd); interactionDelete_hook(gb); return; } CYC(0x78ca, 0x78cd); CYC(0x78cd, 0x78d0); npcFaceLinkAndAnimate_hook(gb); return; }
  CYC(0x78b5, 0x78b7);
  CALL_C(0x78b7, goron_initGraphicsAndIncState_hook, 0x7d72, 0x78ba);
  goronSubid0f__afterCall78ba_hook(gb);
}

void goron_initGraphicsAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d72, goron_initGraphics_hook, 0x7d82, 0x7d75);
  goron_initGraphicsAndIncState__afterCall7d75_hook(gb);
}

void goron_initGraphicsAndIncState__afterCall7d75_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x7d75, 0x7d78); interactionIncState_hook(gb);
}

void goron_loadScriptAndInitGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d78, goron_initGraphics_hook, 0x7d82, 0x7d7b);
  goron_loadScriptAndInitGraphics__afterCall7d7b_hook(gb);
}

void goron_loadScriptAndInitGraphics__afterCall7d7b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x7d7b, 0x7d7d); goron_loadScript_hook(gb);
}

void goron_loadScriptFromTableAndInitGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d7d, goron_initGraphics_hook, 0x7d82, 0x7d80);
  goron_loadScriptFromTableAndInitGraphics__afterCall7d80_hook(gb);
}

void goron_loadScriptFromTableAndInitGraphics__afterCall7d80_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x7d80, 0x7d82); goron_loadScriptFromTable_hook(gb);
}

void goron_initGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d82, interactionLoadExtraGraphics_hook, 0x2781, 0x7d85);
  CYC(0x7d85, 0x7d88); interactionInitGraphics_hook(gb);
}

void goron_loadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d88, 0x7d8a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d8a, 0x7d8b); A = mem_rd(gb, DE);
  CYC(0x7d8b, 0x7d8e); SET_HL(0x7dae);
  CYC(0x7d8e, 0x7d8f); goron_add_double_index(gb, 0x7d8f);
  CYC(0x7d8f, 0x7d90); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d90, 0x7d91); H = mem_rd(gb, HL);
  CYC(0x7d91, 0x7d92); L = A;
  CALL_C(0x7d92, interactionSetScript_hook, 0x2544, 0x7d95);
  CYC(0x7d95, 0x7d98); interactionIncState_hook(gb);
}

void goron_loadScriptFromTable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d98, 0x7d9a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d9a, 0x7d9b); A = mem_rd(gb, DE);
  CYC(0x7d9b, 0x7d9e); SET_HL(0x7dae);
  CYC(0x7d9e, 0x7d9f); goron_add_double_index(gb, 0x7d9f);
  CYC(0x7d9f, 0x7da0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7da0, 0x7da1); H = mem_rd(gb, HL);
  CYC(0x7da1, 0x7da2); L = A;
  CYC(0x7da2, 0x7da3); E = alu_inc8(gb, E);
  CYC(0x7da3, 0x7da4); A = mem_rd(gb, DE);
  CYC(0x7da4, 0x7da5); goron_add_double_index(gb, 0x7da5);
  CYC(0x7da5, 0x7da6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7da6, 0x7da7); H = mem_rd(gb, HL);
  CYC(0x7da7, 0x7da8); L = A;
  CALL_C(0x7da8, interactionSetScript_hook, 0x2544, 0x7dab);
  CYC(0x7dab, 0x7dae); interactionIncState_hook(gb);
}
