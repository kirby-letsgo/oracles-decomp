#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)
#define CYC15(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT15(from, to) burn_rom(gb, 0x15, (from), (to), true)
#define CALL_C15(a, fn, target, ra) do { CYC15((a), (a) + 3); CALL_C_((a), fn, (target), (ra)); } while (0)

void goron_setLinkPositionAndDirection_hook(GB *gb);
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
void goronSubid01_hook(GB *gb);
void goronSubid01__afterCall7778_hook(GB *gb);
void goronSubid02_hook(GB *gb);
void goronSubid00_hook(GB *gb);
void goronSubid00__afterCall7585_hook(GB *gb);
void goronDance_clearVariables_hook(GB *gb);
void goronDance_restartGame_hook(GB *gb);
void goronDance_initLinkPosition_hook(GB *gb);
void goronDance_checkNumFailedRounds_hook(GB *gb);
void goronDance_giveRandomRingPrize_hook(GB *gb);
void goronElder_lookingUpAnimation_hook(GB *gb);
void goronElder_normalAnimation_hook(GB *gb);
void goron_beginWalkingLeft_hook(GB *gb);
void goron_checkEnoughTimePassed_hook(GB *gb);
void goron_clearRefillBit_hook(GB *gb);
void goron_checkInPast_hook(GB *gb);
void goron_checkInPresent_hook(GB *gb);
void goron_checkLinkInAir_hook(GB *gb);

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
    case 0x7574: goronSubid00_hook(gb); return;
	case 0x776b: goronSubid01_hook(gb); return;
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

static void goron_subid01_face_down(GB *gb, uint16_t sp0_) {
  CYC(0x777b, 0x777d); A = 2;
  CALL_C(0x777d, interactionSetAnimation_hook, 0x262e, 0x7780);
}

void goronSubid01__afterCall7778_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7778, goron_loadScript_hook, 0x7d88, 0x777b);
  goron_subid01_face_down(gb, sp0_);
}

void goronSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x776b, 0x776d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x776d, 0x776e); A = mem_rd(gb, DE);
  CYC(0x776e, 0x776f); push_effect(gb, 0x776f);
  switch (goron_jump_table(gb)) {
    case 0x7775: goto state0;
    case 0x7780: goto state1;
    case 0x7794: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CALL_C(0x7775, interactionInitGraphics_hook, 0x15fb, 0x7778);
  goronSubid01__afterCall7778_hook(gb);
  return;
state1:
  CYC(0x7780, 0x7783); A = W8(wTmpcfc0_goronDance_linkStartedDance);
  CYC(0x7783, 0x7784); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7784, 0x7786); goto goto_state2; }
  CYC(0x7784, 0x7786);
  CALL_C(0x7786, interactionRunScript_hook, 0x2552, 0x7789);
  if (F & FC) { CYCT(0x7789, 0x778c); interactionDelete_hook(gb); return; }
  CYC(0x7789, 0x778c); CYC(0x778c, 0x778f); npcFaceLinkAndAnimate_hook(gb); return;
goto_state2:
  CALL_C(0x778f, interactionIncState_hook, 0x23e0, 0x7792);
  CYC(0x7792, 0x7794); goto update_animation;
state2:
  CYC(0x7794, 0x7797); A = W8(wTmpcfc0_goronDance_linkStartedDance);
  CYC(0x7797, 0x7798); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7798, 0x779a); goto goto_state1; }
  CYC(0x7798, 0x779a);
update_animation:
  CYC(0x779a, 0x779d); SET_HL(w1Link_yh + 3);
  CYC(0x779d, 0x779f); E = INTERACTION_BASE + OBJ_Z;
  CYC(0x779f, 0x77a0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x77a0, 0x77a1); mem_wr(gb, DE, A);
  CYC(0x77a1, 0x77a2); E = alu_inc8(gb, E);
  CYC(0x77a2, 0x77a3); A = mem_rd(gb, HL);
  CYC(0x77a3, 0x77a4); mem_wr(gb, DE, A);
  CYC(0x77a4, 0x77a7); A = W8(wTmpcfc0_goronDance_danceAnimation);
  CALL_C(0x77a7, interactionSetAnimation_hook, 0x262e, 0x77aa);
  CYC(0x77aa, 0x77ad); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return;
goto_state1:
  CYC(0x77ad, 0x77ae); H = D;
  CYC(0x77ae, 0x77b0); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x77b0, 0x77b2); mem_wr(gb, HL, 1);
  CYC(0x77b2, 0x77b5); goron_subid01_face_down(gb, sp0_);
}

static void goron_subid00_push_link_away(GB *gb) {
  CYC(0x762a, 0x762d); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

static void goron_subid00_select_script(GB *gb, uint16_t address, uint16_t return_address, uint16_t sp0_) {
  CYC(address, address + 3); SET_HL(0x7de8);
  CYC(address + 3, address + 4); goron_add_double_index(gb, address + 4);
  CYC(address + 4, address + 5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(address + 5, address + 6); H = mem_rd(gb, HL);
  CYC(address + 6, address + 7); L = A;
  CALL_C(address + 7, interactionSetScript_hook, 0x2544, return_address);
}

static void goron_subid00_reset_dance(GB *gb) {
  CYC(0x773b, 0x773c); alu_xor(gb, A);
  CYC(0x773c, 0x773f); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(0x773f, 0x7741); A = 2;
  CYC(0x7741, 0x7744); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(0x7744, 0x7747); goronDance_turnLinkToDirection_hook(gb);
}

static void goron_subid00_next_round(GB *gb) {
  CYC(0x7721, 0x7722); H = D;
  CYC(0x7722, 0x7724); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7724, 0x7726); mem_wr(gb, HL, 2);
  CYC(0x7726, 0x7727); L = alu_inc8(gb, L);
  CYC(0x7727, 0x7729); mem_wr(gb, HL, 0);
  CYC(0x7729, 0x772b); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x772b, 0x772d); mem_wr(gb, HL, 30);
  CYC(0x772d, 0x772f); goron_subid00_reset_dance(gb);
}

static void goron_subid00_end_dance(GB *gb) {
  CYC(0x772f, 0x7730); H = D;
  CYC(0x7730, 0x7732); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7732, 0x7734); mem_wr(gb, HL, 4);
  CYC(0x7734, 0x7735); L = alu_inc8(gb, L);
  CYC(0x7735, 0x7737); mem_wr(gb, HL, 0);
  CYC(0x7737, 0x7739); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7739, 0x773b); mem_wr(gb, HL, 60);
  goron_subid00_reset_dance(gb);
}

static void goron_subid00_run(GB *gb, uint16_t entry, uint16_t sp0_) {
  switch (entry) {
    case 0x7585: goto after_graphics;
    case 0x75c0: goto state1;
    case 0x75df: goto state2;
    case 0x768a: goto state3;
    case 0x7747: goto state4;
    default: hook_continue(gb, entry, sp0_); return;
  }
after_graphics:
  CYC(0x7585, 0x7588); A = W8(wTilesetFlags);
  CYC(0x7588, 0x758a); alu_and(gb, 0x80);
  CYC(0x758a, 0x758c); A = 1;
  if (F & FZ) { CYCT(0x758c, 0x758e); goto set_palette; }
  CYC(0x758c, 0x758e); CYC(0x758e, 0x7590); A = 2;
set_palette:
  CYC(0x7590, 0x7592); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7592, 0x7593); mem_wr(gb, DE, A);
  CYC(0x7593, 0x7596); SET_HL(0x7818);
  CALL_C(0x7596, checkIsLinkedGame_hook, 0x1992, 0x7599);
  if (F & FZ) { CYCT(0x7599, 0x759b); goto load_dancers; }
  CYC(0x7599, 0x759b);
  CYC(0x759b, 0x759e); A = W8(wTilesetFlags);
  CYC(0x759e, 0x75a0); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x75a0, 0x75a2); goto load_dancers; }
  CYC(0x75a0, 0x75a2); CYC(0x75a2, 0x75a5); SET_HL(0x7844);
load_dancers:
  CALL_C(0x75a5, parseGivenObjectData_b00_hook, 0x3171, 0x75a8);
  CYC(0x75a8, 0x75aa); B = 0x20;
  CYC(0x75aa, 0x75ad); SET_HL(wTmpcfc0_goronDance);
  CALL_C(0x75ad, clearMemory_hook, 0x046f, 0x75b0);
  CYC(0x75b0, 0x75b2); A = 2;
  CYC(0x75b2, 0x75b5); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(0x75b5, 0x75b6); alu_xor(gb, A);
  goron_subid00_select_script(gb, 0x75b6, 0x75c0, sp0_);
state1:
  CALL_C(0x75c0, interactionRunScript_hook, 0x2552, 0x75c3);
  if (F & FC) { CYCT(0x75c3, 0x75c6); goto script_done; }
  CYC(0x75c3, 0x75c6); CYC(0x75c6, 0x75c9); npcFaceLinkAndAnimate_hook(gb); return;
script_done:
  CYC(0x75c9, 0x75cb); B = 10;
  CYC(0x75cb, 0x75ce); SET_HL(0x5786);
  CYC(0x75ce, 0x75d0); E = 8;
  CALL_C(0x75d0, interBankCall_hook, 0x008a, 0x75d3);
  CYC(0x75d3, 0x75d5); A = 2;
  CYC(0x75d5, 0x75d8); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(0x75d8, interactionIncState_hook, 0x23e0, 0x75db);
  CYC(0x75db, 0x75dd); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x75dd, 0x75df); mem_wr(gb, HL, 30);
state2:
  CYC(0x75df, 0x75e1); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x75e1, 0x75e2); A = mem_rd(gb, DE);
  CYC(0x75e2, 0x75e3); push_effect(gb, 0x75e3);
  switch (goron_jump_table(gb)) {
    case 0x75ed: goto state2_wait;
    case 0x7602: goto state2_start;
    case 0x760d: goto state2_beat;
    case 0x7659: goto state2_jump;
    case 0x767c: goto state2_finished;
    default: hook_continue(gb, HL, sp0_); return;
  }
state2_wait:
  CALL_C(0x75ed, interactionDecCounter1_hook, 0x23cc, 0x75f0);
  if (!(F & FZ)) { CYCT(0x75f0, 0x75f3); goron_subid00_push_link_away(gb); return; }
  CYC(0x75f0, 0x75f3);
  CALL_C(0x75f3, interactionIncSubstate_hook, 0x23e5, 0x75f6);
  CYC(0x75f6, 0x75f8); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x75f8, 0x75fa); mem_wr(gb, HL, 90);
  CYC(0x75fa, 0x75fc); A = 0xcc;
  CALL_C(0x75fc, playSound_b00_hook, 0x0c98, 0x75ff);
  CALL_C(0x75ff, goronDance_initNextRound_hook, 0x78db, 0x7602);
state2_start:
  CALL_C(0x7602, interactionDecCounter1_hook, 0x23cc, 0x7605);
  if (!(F & FZ)) { CYCT(0x7605, 0x7608); goron_subid00_push_link_away(gb); return; }
  CYC(0x7605, 0x7608);
  CALL_C(0x7608, interactionIncSubstate_hook, 0x23e5, 0x760b);
  CYC(0x760b, 0x760d); goto next_move;
state2_beat:
  CALL_C(0x760d, interactionDecCounter1_hook, 0x23cc, 0x7610);
  if (!(F & FZ)) { CYCT(0x7610, 0x7612); goron_subid00_push_link_away(gb); return; }
  CYC(0x7610, 0x7612);
  CALL_C(0x7612, goronDance_incBeat_hook, 0x7a1b, 0x7615);
next_move:
  CALL_C(0x7615, goronDance_getNextMove_hook, 0x7a20, 0x7618);
  if (!(F & FZ)) { CYCT(0x7618, 0x761a); goto demonstration_finished; }
  CYC(0x7618, 0x761a);
  CALL_C(0x761a, goronDance_updateConsecutiveBPressCounter_hook, 0x7a3c, 0x761d);
  CALL_C(0x761d, goronDance_updateGracefulGoronAnimation_hook, 0x7acf, 0x7620);
  if (F & FZ) { CYCT(0x7620, 0x7622); goto jump; }
  CYC(0x7620, 0x7622);
  CALL_C(0x7622, goronDance_playMoveSound_hook, 0x7a04, 0x7625);
  CYC(0x7625, 0x7626); H = D;
  CYC(0x7626, 0x7628); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7628, 0x762a); mem_wr(gb, HL, 20);
  goron_subid00_push_link_away(gb); return;
jump:
  CYC(0x762d, 0x762e); H = D;
  CYC(0x762e, 0x7630); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7630, 0x7632); mem_wr(gb, HL, 3);
  CYC(0x7632, 0x7634); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7634, 0x7636); mem_wr(gb, HL, 0x28);
  CYC(0x7636, 0x7638); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x7638, 0x763a); mem_wr(gb, HL, 0);
  CYC(0x763a, 0x763b); SET_HL(HL + 1);
  CYC(0x763b, 0x763d); mem_wr(gb, HL, 0xfe);
  CYC(0x763d, 0x763f); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x763f, 0x7641); mem_wr(gb, HL, 20);
  CYC(0x7641, 0x7643); A = 0xcd;
  CALL_C(0x7643, playSound_b00_hook, 0x0c98, 0x7646);
  CYC(0x7646, 0x7649); goron_subid00_push_link_away(gb); return;
demonstration_finished:
  CYC(0x7649, 0x764a); H = D;
  CYC(0x764a, 0x764c); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x764c, 0x764e); mem_wr(gb, HL, 4);
  CYC(0x764e, 0x7650); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7650, 0x7652); mem_wr(gb, HL, 60);
  CYC(0x7652, 0x7654); A = 2;
  CALL_C(0x7654, interactionSetAnimation_hook, 0x262e, 0x7657);
  CYC(0x7657, 0x7659); goron_subid00_push_link_away(gb); return;
state2_jump:
  CALL_C(0x7659, interactionDecCounter1_hook, 0x23cc, 0x765c);
  CYC(0x765c, 0x765e); C = 0x40;
  CALL_C(0x765e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7661);
  if (F & FZ) { CYCT(0x7661, 0x7663); goto landed; }
  CYC(0x7661, 0x7663);
  CYC(0x7663, 0x7664); H = D;
  CYC(0x7664, 0x7666); L = INTERACTION_BASE + OBJ_SPEED_Z + 1;
  CYC(0x7666, 0x7667); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x7667, 0x7668); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7668, 0x766a); goron_subid00_push_link_away(gb); return; }
  CYC(0x7668, 0x766a);
  CYC(0x766a, 0x766c); A = 2;
  CYC(0x766c, 0x766f); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(0x766f, interactionSetAnimation_hook, 0x262e, 0x7672);
  CYC(0x7672, 0x7674); goron_subid00_push_link_away(gb); return;
landed:
  CYC(0x7674, 0x7675); H = D;
  CYC(0x7675, 0x7677); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7677, 0x7679); mem_wr(gb, HL, 2);
  CYC(0x7679, 0x767c); goto state2_beat;
state2_finished:
  CALL_C(0x767c, interactionDecCounter1_hook, 0x23cc, 0x767f);
  if (!(F & FZ)) { CYCT(0x767f, 0x7681); goron_subid00_push_link_away(gb); return; }
  CYC(0x767f, 0x7681);
  CALL_C(0x7681, interactionIncState_hook, 0x23e0, 0x7684);
  CYC(0x7684, 0x7686); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7686, 0x7688); mem_wr(gb, HL, 0);
  CYC(0x7688, 0x768a); goron_subid00_push_link_away(gb); return;
state3:
  CYC(0x768a, 0x768c); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x768c, 0x768d); A = mem_rd(gb, DE);
  CYC(0x768d, 0x768e); push_effect(gb, 0x768e);
  switch (goron_jump_table(gb)) {
    case 0x7698: goto state3_start;
    case 0x76ae: goto state3_input;
    case 0x76b7: goto state3_landing;
    case 0x76c8: goto state3_round_end;
    case 0x76e1: goto state3_failed;
    default: hook_continue(gb, HL, sp0_); return;
  }
state3_start:
  CALL_C(0x7698, interactionIncSubstate_hook, 0x23e5, 0x769b);
  CALL_C(0x769b, goronDance_clearDanceVariables_hook, 0x78e9, 0x769e);
  CYC(0x769e, 0x76a0); A = 0xcc;
  CALL_C(0x76a0, playSound_b00_hook, 0x0c98, 0x76a3);
  CYC(0x76a3, 0x76a5); A = 2;
  CYC(0x76a5, 0x76a8); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(0x76a8, goronDance_turnLinkToDirection_hook, 0x7a72, 0x76ab);
  CYC(0x76ab, 0x76ae); goron_subid00_push_link_away(gb); return;
state3_input:
  CALL_C(0x76ae, goronDance_updateFrameCounter_hook, 0x78d0, 0x76b1);
  CALL_C(0x76b1, goronDance_checkLinkInput_hook, 0x7903, 0x76b4);
  CYC(0x76b4, 0x76b7); goron_subid00_push_link_away(gb); return;
state3_landing:
  CALL_C(0x76b7, goronDance_updateFrameCounter_hook, 0x78d0, 0x76ba);
  CYC(0x76ba, 0x76bd); A = W8(wTmpcfc0_goronDance_linkJumping);
  CYC(0x76bd, 0x76be); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76be, 0x76c1); goron_subid00_push_link_away(gb); return; }
  CYC(0x76be, 0x76c1);
  CYC(0x76c1, 0x76c2); H = D;
  CYC(0x76c2, 0x76c4); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x76c4, 0x76c5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x76c5, 0x76c8); goron_subid00_push_link_away(gb); return;
state3_round_end:
  CALL_C(0x76c8, interactionDecCounter1_hook, 0x23cc, 0x76cb);
  if (!(F & FZ)) { CYCT(0x76cb, 0x76ce); goron_subid00_push_link_away(gb); return; }
  CYC(0x76cb, 0x76ce);
  CYC(0x76ce, 0x76d1); A = W8(wTmpcfc0_goronDance_roundIndex);
  CYC(0x76d1, 0x76d3); alu_cp(gb, 8);
  if (F & FZ) { CYCT(0x76d3, 0x76d5); goto end_dance; }
  CYC(0x76d3, 0x76d5);
next_round:
  CYC(0x76d5, 0x76d8); push_effect(gb, 0x76d8); goron_subid00_next_round(gb); return;
end_dance:
  CYC(0x76db, 0x76de); push_effect(gb, 0x76de); goron_subid00_end_dance(gb); return;
state3_failed:
  CYC(0x76e1, 0x76e3); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x76e3, 0x76e4); A = mem_rd(gb, DE);
  CYC(0x76e4, 0x76e5); push_effect(gb, 0x76e5);
  switch (goron_jump_table(gb)) {
    case 0x76e9: goto initialize_failed_script;
    case 0x7718: goto run_failed_script;
    default: hook_continue(gb, HL, sp0_); return;
  }
initialize_failed_script:
  CALL_C(0x76e9, interactionDecCounter1_hook, 0x23cc, 0x76ec);
  if (!(F & FZ)) { CYCT(0x76ec, 0x76ef); goron_subid00_push_link_away(gb); return; }
  CYC(0x76ec, 0x76ef);
  CYC(0x76ef, 0x76f1); A = 1;
  CYC(0x76f1, 0x76f4); W8(wTmpcfc0_goronDance_cfd9) = A;
  CYC(0x76f4, 0x76f7); SET_HL(wTmpcfc0_goronDance_roundIndex);
  CYC(0x76f7, 0x76f8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x76f8, 0x76fb); SET_HL(wTmpcfc0_goronDance_numFailedRounds);
  CYC(0x76fb, 0x76fc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x76fc, 0x76fd); A = mem_rd(gb, HL);
  CYC(0x76fd, 0x76ff); alu_cp(gb, 3);
  if (F & FZ) { CYCT(0x76ff, 0x7701); goto set_failed_script; }
  CYC(0x76ff, 0x7701);
  CYC(0x7701, 0x7704); A = W8(wTmpcfc0_goronDance_roundIndex);
  CYC(0x7704, 0x7706); alu_cp(gb, 8);
  if (F & FZ) { CYCT(0x7706, 0x7708); goto end_dance; }
  CYC(0x7706, 0x7708);
set_failed_script:
  CYC(0x7708, 0x7709); H = D;
  CYC(0x7709, 0x770b); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x770b, 0x770c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x770c, 0x770e); A = 1;
  goron_subid00_select_script(gb, 0x770e, 0x7718, sp0_);
run_failed_script:
  CALL_C(0x7718, interactionRunScript_hook, 0x2552, 0x771b);
  if (!(F & FC)) { CYCT(0x771b, 0x771e); goron_subid00_push_link_away(gb); return; }
  CYC(0x771b, 0x771e); CYC(0x771e, 0x7721); goto next_round;
state4:
  CYC(0x7747, 0x7749); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7749, 0x774a); A = mem_rd(gb, DE);
  CYC(0x774a, 0x774b); push_effect(gb, 0x774b);
  switch (goron_jump_table(gb)) {
    case 0x774f: goto state4_start;
    case 0x7762: goto state4_script;
    default: hook_continue(gb, HL, sp0_); return;
  }
state4_start:
  CALL_C(0x774f, interactionIncSubstate_hook, 0x23e5, 0x7752);
  CYC(0x7752, 0x7753); alu_xor(gb, A);
  CYC(0x7753, 0x7756); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(0x7756, 0x7758); A = 2;
  goron_subid00_select_script(gb, 0x7758, 0x7762, sp0_);
state4_script:
  CALL_C(0x7762, interactionRunScript_hook, 0x2552, 0x7765);
  if (!(F & FC)) { CYCT(0x7765, 0x7768); goron_subid00_push_link_away(gb); return; }
  CYC(0x7765, 0x7768); CYC(0x7768, 0x776b); goron_subid00_push_link_away(gb);
}

void goronSubid00__afterCall7585_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goron_subid00_run(gb, 0x7585, sp0_);
}

void goronSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7574, 0x7576); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7576, 0x7577); A = mem_rd(gb, DE);
  CYC(0x7577, 0x7578); push_effect(gb, 0x7578);
  switch (goron_jump_table(gb)) {
    case 0x7582:
      CALL_C(0x7582, goron_initGraphicsAndIncState_hook, 0x7d72, 0x7585);
      goronSubid00__afterCall7585_hook(gb);
      return;
    case 0x75c0: goron_subid00_run(gb, 0x75c0, sp0_); return;
    case 0x75df: goron_subid00_run(gb, 0x75df, sp0_); return;
    case 0x768a: goron_subid00_run(gb, 0x768a, sp0_); return;
    case 0x7747: goron_subid00_run(gb, 0x7747, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void goronDance_clearVariables_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC15(0x62ef, 0x62f1); B = 0x20;
  CYC15(0x62f1, 0x62f4); SET_HL(wTmpcfc0_goronDance);
  CALL_C15(0x62f4, clearMemory_hook, 0x046f, 0x62f7);
  CYC15(0x62f7, 0x62f9); A = 2;
  CYC15(0x62f9, 0x62fc); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC15(0x62fc, 0x62ff); SET_HL(w1Link_yh - 3);
  CYC15(0x62ff, 0x6301); mem_wr(gb, HL, 2);
  CYC15(0x6301, 0x6302); H = D;
  CYC15(0x6302, 0x6304); L = INTERACTION_BASE + OBJ_STATE;
  CYC15(0x6304, 0x6306); mem_wr(gb, HL, 1);
  CYC15(0x6306, 0x6307); L = alu_inc8(gb, L);
  CYC15(0x6307, 0x6309); mem_wr(gb, HL, 0);
  CYC15(0x6309, 0x630a); ret_effect(gb);
}

void goronDance_restartGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x630a, 0x630b); alu_xor(gb, A);
  CYC15(0x630b, 0x630e); W8(wTmpcfc0_goronDance_roundIndex) = A;
  CYC15(0x630e, 0x6311); W8(wTmpcfc0_goronDance_numFailedRounds) = A;
  CYC15(0x6311, 0x6314); SET_HL(w1Link_yh - 3);
  CYC15(0x6314, 0x6316); mem_wr(gb, HL, 2);
  CYC15(0x6316, 0x6318); B = 10;
  CYC15(0x6318, 0x631b); SET_HL(0x5786);
  CYC15(0x631b, 0x631d); E = 8;
  CYC15(0x631d, 0x6320); interBankCall_hook(gb);
}

void goronDance_initLinkPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x6331, 0x6333); A = 2;
  CYC15(0x6333, 0x6336); SET_BC(0x5c50);
  CYC15(0x6336, 0x6338); goron_setLinkPositionAndDirection_hook(gb);
}

void goronDance_checkNumFailedRounds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x635b, 0x635e); A = W8(wTmpcfc0_goronDance_numFailedRounds);
  CYC15(0x635e, 0x635f); B = A;
  CYC15(0x635f, 0x6361); A = 8;
  CYC15(0x6361, 0x6362); alu_sub(gb, B);
  CYC15(0x6362, 0x6365); SET_HL(wTextNumberSubstitution);
  CYC15(0x6365, 0x6366); mem_wr(gb, HL, A);
  CYC15(0x6366, 0x6367); SET_HL(HL + 1);
  CYC15(0x6367, 0x6369); mem_wr(gb, HL, 0);
  CYC15(0x6369, 0x636c); A = W8(wTmpcfc0_goronDance_numFailedRounds);
  CYC15(0x636c, 0x636d); alu_or(gb, A);
  CYC15(0x636d, 0x6370); writeFlagsTocddb_hook(gb);
}

void goronDance_giveRandomRingPrize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC15(0x6370, 0x6373); A = W8(wTilesetFlags);
  CYC15(0x6373, 0x6375); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT15(0x6375, 0x6377); goto past; }
  CYC15(0x6375, 0x6377);
  CYC15(0x6377, 0x6379); B = 2;
  CYC15(0x6379, 0x637b); goto give_ring;
past:
  CYC15(0x637b, 0x637d); B = 0;
  CYC15(0x637d, 0x6380); A = W8(wTmpcfc0_goronDance_danceLevel);
  CYC15(0x6380, 0x6382); alu_cp(gb, 0);
  if (F & FZ) { CYCT15(0x6382, 0x6384); goto give_ring; }
  CYC15(0x6382, 0x6384);
  CYC15(0x6384, 0x6386); B = 2;
give_ring:
  CALL_C15(0x6386, getRandomNumber_hook, 0x043e, 0x6389);
  CYC15(0x6389, 0x638b); alu_and(gb, 1);
  CYC15(0x638b, 0x638c); alu_add(gb, B);
  CYC15(0x638c, 0x638f); SET_HL(0x6394);
  CYC15(0x638f, 0x6390); goron_add_a_to_hl(gb, 0x6390);
  CYC15(0x6390, 0x6391); A = mem_rd(gb, HL);
  CYC15(0x6391, 0x6394); giveRingAToLink_hook(gb);
}

void goronElder_lookingUpAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x7341, 0x7342); H = D;
  CYC15(0x7342, 0x7344); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC15(0x7344, 0x7346); mem_wr(gb, HL, 1);
  CYC15(0x7346, 0x7348); A = 4;
  CYC15(0x7348, 0x734b); interactionSetAnimation_hook(gb);
}

void goronElder_normalAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x734b, 0x734c); H = D;
  CYC15(0x734c, 0x734e); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC15(0x734e, 0x7350); mem_wr(gb, HL, 0);
  CYC15(0x7350, 0x7352); A = 2;
  CYC15(0x7352, 0x7355); interactionSetAnimation_hook(gb);
}

void goron_beginWalkingLeft_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x6523, 0x6524); H = D;
  CYC15(0x6524, 0x6526); L = INTERACTION_BASE + OBJ_SPEED;
  CYC15(0x6526, 0x6528); mem_wr(gb, HL, 0x14);
  CYC15(0x6528, 0x652a); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC15(0x652a, 0x652c); mem_wr(gb, HL, 0x18);
  CYC15(0x652c, 0x652e); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC15(0x652e, 0x6530); mem_wr(gb, HL, 0x40);
  CYC15(0x6530, 0x6531); L = alu_inc8(gb, L);
  CYC15(0x6531, 0x6533); mem_wr(gb, HL, 0);
  CYC15(0x6533, 0x6535); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC15(0x6535, 0x6537); mem_wr(gb, HL, 1);
  CYC15(0x6537, 0x6539); A = 3;
  CYC15(0x6539, 0x653b); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC15(0x653b, 0x653c); mem_wr(gb, DE, A);
  CYC15(0x653c, 0x653f); interactionSetAnimation_hook(gb);
}

void goron_checkEnoughTimePassed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC15(0x6689, 0x668c); A = W8(wSeedTreeRefilledBitset);
  CYC15(0x668c, 0x668d); A = (uint8_t)~A;
  CYC15(0x668d, 0x668f); alu_bit(gb, 0, A);
  CALL_C15(0x668f, writeFlagsTocddb_hook, 0x5118, 0x6692);
  goron_clearRefillBit_hook(gb);
}

void goron_clearRefillBit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x6692, 0x6695); SET_HL(wSeedTreeRefilledBitset);
  CYC15(0x6695, 0x6697); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC15(0x6697, 0x6698); ret_effect(gb);
}

void goron_checkInPast_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x6328, 0x632b); A = W8(wTilesetFlags);
  CYC15(0x632b, 0x632c); A = (uint8_t)~A;
  CYC15(0x632c, 0x632e); alu_and(gb, 0x80);
  CYC15(0x632e, 0x6331); writeFlagsTocddb_hook(gb);
}

void goron_checkInPresent_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x6320, 0x6323); A = W8(wTilesetFlags);
  CYC15(0x6323, 0x6325); alu_and(gb, 0x80);
  CYC15(0x6325, 0x6328); writeFlagsTocddb_hook(gb);
}

void goron_checkLinkInAir_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(0x67c5, 0x67c8); A = W8(wLinkInAir);
  CYC15(0x67c8, 0x67c9); alu_or(gb, A);
  CYC15(0x67c9, 0x67cc); writeFlagsTocddb_hook(gb);
}
