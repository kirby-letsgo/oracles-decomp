#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)
#define CYC15(from, to) burn_rom(gb, SYMBANK(goronDance_clearVariables), (from), (to), false)
#define CYCT15(from, to) burn_rom(gb, SYMBANK(goronDance_giveRandomRingPrize), (from), (to), true)
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
  BASE(interactionCode66);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == SYM(goronSubid00)) { goronSubid00_hook(gb); return; }
    else if (jt_ == SYM(goronSubid01)) { goronSubid01_hook(gb); return; }
    else if (jt_ == SYM(goronSubid02)) { goronSubid02_hook(gb); return; }
    else if (jt_ == SYM(goronSubid03)) { goronSubid03_hook(gb); return; }
    else if (jt_ == SYM(goronSubid05)) { goronSubid05_hook(gb); return; }
    else if (jt_ == SYM(goronSubid06)) { goronSubid06_hook(gb); return; }
    else if (jt_ == SYM(goronSubid07)) { goronSubid07_hook(gb); return; }
    else if (jt_ == SYM(goronSubid09)) { goronSubid09_hook(gb); return; }
    else if (jt_ == SYM(goronSubid0b)) { goronSubid0b_hook(gb); return; }
    else if (jt_ == SYM(goronSubid0f)) { goronSubid0f_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void goronDance_updateFrameCounter_hook(GB *gb) {
  BASE(goronDance_updateFrameCounter);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_linkStartedDance);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(wTmpcfc0_goronDance_frameCounter);
  CYC(b_+8, b_+11); incHlRef16WithCap_hook(gb);
}

void goronDance_initNextRound_hook(GB *gb) {
  BASE(goronDance_initNextRound);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_remainingRounds);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goronDance_clearDanceVariables_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_HL((SYM(interactionCode48__initSubid06) + 6));
  CYC(b_+9, b_+11); E = 0x08;
  CALL_C(b_+11, interBankCall_hook, 0x008a, SYM(goronDance_clearDanceVariables));
  goronDance_clearDanceVariables_hook(gb);
}

void goronDance_clearDanceVariables_hook(GB *gb) {
  BASE(goronDance_clearDanceVariables);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wTmpcfc0_goronDance_linkJumping) = A;
  CYC(b_+4, b_+7); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(b_+7, b_+10); W8(wTmpcfc0_goronDance_frameCounter) = A;
  CYC(b_+10, b_+13); mem_wr(gb, wTmpcfc0_goronDance_frameCounter + 1, A);
  CYC(b_+13, b_+16); W8(wTmpcfc0_goronDance_currentMove) = A;
  CYC(b_+16, b_+19); W8(wTmpcfc0_goronDance_consecutiveBPressCounter) = A;
  CYC(b_+19, b_+22); W8(wTmpcfc0_goronDance_cfd9) = A;
  CYC(b_+22, b_+25); W8(wTmpcfc0_goronDance_beat) = A;
  CYC(b_+25, b_+26); ret_effect(gb);
}

void goronDance_getCurrentAndNeededFrameCounts_hook(GB *gb) {
  BASE(goronDance_getCurrentAndNeededFrameCounts);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_beat);
  CYC(b_+3, b_+4); push_effect(gb, AF);
  CALL_C(b_+4, multiplyABy4_hook, SYM(multiplyABy4), b_+7);
  CYC(b_+7, b_+8); L = C;
  CYC(b_+8, b_+9); H = B;
  CYC(b_+9, b_+10); SET_AF(pop_effect(gb));
  CALL_C(b_+10, multiplyABy16_hook, SYM(multiplyABy16), b_+13);
  CYC(b_+13, b_+14); alu_add_hl(gb, BC);
  CYC(b_+14, b_+17); A = W8(wTmpcfc0_goronDance_frameCounter);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+21); A = mem_rd(gb, wTmpcfc0_goronDance_frameCounter + 1);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); ret_effect(gb);
}

void goronDance_checkInputNotTooEarlyOrLate_hook(GB *gb) {
  BASE(goronDance_checkInputNotTooEarlyOrLate);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goronDance_getCurrentAndNeededFrameCounts_hook, SYM(goronDance_getCurrentAndNeededFrameCounts), b_+3);
  CYC(b_+3, b_+5); A = 8;
  CYC(b_+5, b_+6); goron_add_a_to_hl(gb, b_+6);
  CYC(b_+6, b_+8); A = 8;
  CALL_C(b_+8, addAToBc_hook, 0x006d, b_+11);
  CYC(b_+11, b_+12); push_effect(gb, BC);
  CYC(b_+12, b_+14); B = 0xff;
  CYC(b_+14, b_+16); C = 0xf8;
  CYC(b_+16, b_+17); alu_add_hl(gb, BC);
  CYC(b_+17, b_+18); SET_BC(pop_effect(gb));
  CALL_C(b_+18, compareHlToBc_hook, SYM(compareHlToBc), b_+21);
  CYC(b_+21, b_+23); alu_cp(gb, 1);
  if (F & FZ) { CYCT(b_+23, b_+25); goto too_early; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); A = 0x10;
  CYC(b_+27, b_+28); goron_add_a_to_hl(gb, b_+28);
  CALL_C(b_+28, compareHlToBc_hook, SYM(compareHlToBc), b_+31);
  CYC(b_+31, b_+33); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+33, b_+35); goto too_late; }
  CYC(b_+33, b_+35); ret_effect(gb); return;
too_early:
  CYC(b_+36, b_+38); A = 0;
  CYC(b_+38, b_+41); W8(wTmpcfc0_goronDance_failureType) = A;
  CYC(b_+41, b_+42); ret_effect(gb); return;
too_late:
  CYC(b_+42, b_+44); A = 1;
  CYC(b_+44, b_+47); W8(wTmpcfc0_goronDance_failureType) = A;
  CYC(b_+47, b_+48); ret_effect(gb);
}

void goronDance_checkTooLateToInput_hook(GB *gb) {
  BASE(goronDance_checkTooLateToInput);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goronDance_getCurrentAndNeededFrameCounts_hook, SYM(goronDance_getCurrentAndNeededFrameCounts), b_+3);
  CYC(b_+3, b_+5); A = 8;
  CYC(b_+5, b_+6); goron_add_a_to_hl(gb, b_+6);
  CYC(b_+6, b_+8);
  CALL_C((SYM(goronDance_checkExactInputTimePassed) + 3), compareHlToBc_hook, SYM(compareHlToBc), (SYM(goronDance_checkExactInputTimePassed) + 6));
  CYC((SYM(goronDance_checkExactInputTimePassed) + 6), (SYM(goronDance_checkExactInputTimePassed) + 8)); alu_cp(gb, 0xff);
  CYC((SYM(goronDance_checkExactInputTimePassed) + 8), (SYM(goronDance_checkExactInputTimePassed) + 9)); ret_effect(gb);
}

void goronDance_checkExactInputTimePassed_hook(GB *gb) {
  BASE(goronDance_checkExactInputTimePassed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goronDance_getCurrentAndNeededFrameCounts_hook, SYM(goronDance_getCurrentAndNeededFrameCounts), b_+3);
  CALL_C(b_+3, compareHlToBc_hook, SYM(compareHlToBc), b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0xff);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void goronDance_playMoveSound_hook(GB *gb) {
  BASE(goronDance_playMoveSound);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); alu_cp(gb, 2);
  if (F & FZ) { CYCT(b_+11, b_+13); A = 0xcd; CYC(b_+18, b_+23); playSound_b00_hook(gb); return; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0xc8;
  CYC(b_+15, b_+18); playSound_b00_hook(gb);
}

void goronDance_incBeat_hook(GB *gb) {
  BASE(goronDance_incBeat);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_goronDance_beat);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void goronDance_getNextMove_hook(GB *gb) {
  BASE(goronDance_getNextMove);
  CYC(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_danceLevel);
  CYC(b_+3, b_+6); SET_HL(SYM(goronDance_sequenceData));
  CYC(b_+6, b_+7); goron_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); A = W8(wTmpcfc0_goronDance_dancePattern);
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+19); A = W8(wTmpcfc0_goronDance_beat);
  CYC(b_+19, b_+20); alu_add(gb, B);
  CYC(b_+20, b_+21); goron_add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+25); W8(wTmpcfc0_goronDance_currentMove) = A;
  CYC(b_+25, b_+27); alu_bit(gb, 7, A);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void goronDance_updateConsecutiveBPressCounter_hook(GB *gb) {
  BASE(goronDance_updateConsecutiveBPressCounter);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(b_+3, b_+6); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+6, b_+8); alu_cp(gb, 2);
  if (F & FZ) { CYCT(b_+8, b_+10); CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void goronDance_turnLinkToDirection_hook(GB *gb) {
  BASE(goronDance_turnLinkToDirection);
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); A = 0x10;
  CYC(b_+6, b_+9); W8(wcc50) = A;
  CYC(b_+9, b_+10); alu_or(gb, D);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void goronDance_updateBackupDancerAnimation_hook(GB *gb) {
  BASE(goronDance_updateBackupDancerAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto gorons; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = W8(wTilesetFlags);
  CYC(b_+8, b_+10); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+10, b_+12); goto gorons; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+15, b_+17); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto subrosian_b; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 6;
  CYC(b_+21, b_+23); goto set_animation;
subrosian_b:
  CYC(b_+23, b_+26); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(b_+26, b_+29); SET_HL(SYM(goronDance_subrosianBAnimations));
  CYC(b_+29, b_+30); goron_add_a_to_hl(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+33, b_+34); ret_effect(gb); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+37); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(b_+37, b_+38); ret_effect(gb); return;
gorons:
  CYC(b_+38, b_+41); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+41, b_+43); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto goron_b; }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); A = 6;
  CYC(b_+47, b_+49); goto set_animation;
goron_b:
  CYC(b_+49, b_+52); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(b_+52, b_+55); SET_HL(SYM(goronDance_goronBAnimations));
  CYC(b_+55, b_+56); goron_add_a_to_hl(gb, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+59, b_+60); ret_effect(gb); return; }
  CYC(b_+59, b_+60);
set_animation:
  CYC(b_+60, b_+63); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(b_+63, b_+64); ret_effect(gb);
}

void goronDance_updateLinkAndBackupDancerAnimation_hook(GB *gb) {
  BASE(goronDance_updateLinkAndBackupDancerAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goronDance_updateBackupDancerAnimation_hook, SYM(goronDance_updateBackupDancerAnimation), b_+3);
  CYC(b_+3, b_+6); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+6, b_+8); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto b_button; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); A = 8;
  CYC(b_+12, b_+15); W8(wcc50) = A;
  CYC(b_+15, b_+16); alu_or(gb, D);
  CYC(b_+16, b_+17); ret_effect(gb); return;
b_button:
  CYC(b_+17, b_+20); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(b_+20, b_+23); SET_HL(SYM(goronDance_linkBButtonAnimations));
  CYC(b_+23, b_+24); goron_add_a_to_hl(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+27, b_+28); ret_effect(gb); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 4);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goronDance_turnLinkToDirection_hook(gb); return; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x0e;
  CYC(b_+34, b_+37); W8(wcc50) = A;
  CYC(b_+37, b_+38); alu_or(gb, D);
  CYC(b_+38, b_+39); ret_effect(gb);
}

void goronDance_updateGracefulGoronAnimation_hook(GB *gb) {
  BASE(goronDance_updateGracefulGoronAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+3, b_+5); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto b_button; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 6;
  CYC(b_+9, b_+11); goto set_animation;
b_button:
  CYC(b_+11, b_+14); A = W8(wTmpcfc0_goronDance_consecutiveBPressCounter);
  CYC(b_+14, b_+17); SET_HL(SYM(goronDance_goronBAnimations));
  CYC(b_+17, b_+18); goron_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+21, b_+22); ret_effect(gb); return; }
  CYC(b_+21, b_+22);
set_animation:
  CALL_C(b_+22, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+25);
  CYC(b_+25, b_+26); alu_or(gb, D);
  CYC(b_+26, b_+27); ret_effect(gb);
}

static void goron_dance_made_mistake(GB *gb, uint16_t sp0_) {
  BASE(goronDance_checkLinkInput);
  CYC(b_+61, b_+62); H = D;
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 4);
  CYC(b_+66, b_+68); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0);
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 30);
  CYC(b_+74, b_+76); A = 0x5a;
  CALL_C(b_+76, playSound_b00_hook, SYM(playSound_b00), b_+79);
  CYC(b_+79, b_+81); A = 2;
  CYC(b_+81, b_+84); W8(wcc50) = A;
  CALL_C(b_+84, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+87);
  if (F & FZ) { CYCT(b_+87, b_+89); goto gorons; }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+92); A = W8(wTilesetFlags);
  CYC(b_+92, b_+94); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+94, b_+96); goto gorons; }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); A = 2;
  CYC(b_+98, b_+101); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(b_+101, b_+102); ret_effect(gb); return;
gorons:
  CYC(b_+102, b_+104); A = 4;
  CYC(b_+104, b_+107); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(b_+107, b_+108); ret_effect(gb);
}

void goronDance_checkLinkInput_hook(GB *gb) {
  BASE(goronDance_checkLinkInput);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goronDance_getNextMove_hook, SYM(goronDance_getNextMove), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0);
  if (F & FZ) { CYCT(b_+5, b_+7); goto rest; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, goronDance_checkTooLateToInput_hook, SYM(goronDance_checkTooLateToInput), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+12); goto too_late; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = W8(wGameKeysJustPressed);
  CYC(b_+15, b_+17); alu_and(gb, 3);
  if (F & FZ) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+22); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(b_+22, b_+25); A = W8(wTmpcfc0_goronDance_currentMove);
  CYC(b_+25, b_+26); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto wrong_move; }
  CYC(b_+26, b_+28);
  CALL_C(b_+28, goronDance_checkInputNotTooEarlyOrLate_hook, SYM(goronDance_checkInputNotTooEarlyOrLate), b_+31);
  if (F & FZ) { CYCT(b_+31, b_+33); goron_dance_made_mistake(gb, sp0_); return; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); goto do_dance_move;
rest:
  CALL_C(b_+36, goronDance_checkExactInputTimePassed_hook, SYM(goronDance_checkExactInputTimePassed), b_+39);
  if (F & FZ) { CYCT(b_+39, b_+41); goto do_dance_move; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+44); A = W8(wGameKeysJustPressed);
  CYC(b_+44, b_+46); alu_and(gb, 3);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto wrong_move; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+49); ret_effect(gb); return;
too_late:
  CYC(b_+49, b_+51); A = 1;
  CYC(b_+51, b_+54); W8(wTmpcfc0_goronDance_failureType) = A;
  CYC(b_+54, b_+56); goto made_mistake;
wrong_move:
  CYC(b_+56, b_+58); A = 2;
  CYC(b_+58, b_+61); W8(wTmpcfc0_goronDance_failureType) = A;
made_mistake:
  goron_dance_made_mistake(gb, sp0_); return;
do_dance_move:
  CALL_C(b_+108, goronDance_updateConsecutiveBPressCounter_hook, SYM(goronDance_updateConsecutiveBPressCounter), b_+111);
  CALL_C(b_+111, goronDance_updateLinkAndBackupDancerAnimation_hook, SYM(goronDance_updateLinkAndBackupDancerAnimation), b_+114);
  if (F & FZ) { CYCT(b_+114, b_+116); goto jump; }
  CYC(b_+114, b_+116);
  CALL_C(b_+116, goronDance_playMoveSound_hook, SYM(goronDance_playMoveSound), b_+119);
  CALL_C(b_+119, goronDance_incBeat_hook, SYM(goronDance_incBeat), b_+122);
  CALL_C(b_+122, goronDance_getNextMove_hook, SYM(goronDance_getNextMove), b_+125);
  if (!(F & FZ)) { CYCT(b_+125, b_+127); goto round_finished; }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+128); ret_effect(gb); return;
jump:
  CALL_C(b_+128, goronDance_incBeat_hook, SYM(goronDance_incBeat), b_+131);
  CALL_C(b_+131, goronDance_getNextMove_hook, SYM(goronDance_getNextMove), b_+134);
  CALL_C(b_+134, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+137);
  if (!(F & FZ)) { CYCT(b_+137, b_+138); ret_effect(gb); return; }
  CYC(b_+137, b_+138);
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x66);
  CYC(b_+140, b_+141); L = alu_inc8(gb, L);
  CYC(b_+141, b_+143); mem_wr(gb, HL, 2);
  CYC(b_+143, b_+145); A = 1;
  CYC(b_+145, b_+148); W8(wTmpcfc0_goronDance_linkJumping) = A;
  CYC(b_+148, b_+151); interactionIncSubstate_hook(gb); return;
round_finished:
  CYC(b_+151, b_+152); alu_xor(gb, A);
  CYC(b_+152, b_+155); W8(wTmpcfc0_goronDance_cfd9) = A;
  CYC(b_+155, b_+158); SET_HL(wTmpcfc0_goronDance_roundIndex);
  CYC(b_+158, b_+159); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+159, b_+160); H = D;
  CYC(b_+160, b_+162); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 3);
  CYC(b_+164, b_+166); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+166, b_+168); mem_wr(gb, HL, 30);
  CYC(b_+168, b_+169); ret_effect(gb);
}

void goronSubid02_hook(GB *gb) {
  BASE(goronSubid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectSetInvisible_hook, SYM(objectSetInvisible), b_+8);
  CALL_C(b_+8, interactionIncState_hook, SYM(interactionIncState), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0);
  CYC(b_+19, b_+20); SET_HL(HL + 1);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xfe);
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 20);
  CYC(b_+26, b_+29); SET_HL(w1Link_yh);
  CALL_C(b_+29, objectTakePosition_hook, SYM(objectTakePosition), b_+32);
  CYC(b_+32, b_+34); A = 0;
  CYC(b_+34, b_+37); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(b_+37, goronDance_turnLinkToDirection_hook, SYM(goronDance_turnLinkToDirection), b_+40);
  CYC(b_+40, b_+42); A = 0xcd;
  CALL_C(b_+42, playSound_b00_hook, SYM(playSound_b00), b_+45);
state1:
  CYC(b_+45, b_+47); C = 0x40;
  CALL_C(b_+47, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+50);
  if (F & FZ) { CYCT(b_+50, b_+52); goto landed; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); SET_HL(w1Link_yh);
  CALL_C(b_+55, objectCopyPosition_hook, SYM(objectCopyPosition), b_+58);
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); L = INTERACTION_BASE + OBJ_SPEED_Z + 1;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+62, b_+63); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+63, b_+64); ret_effect(gb); return; }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+66); A = 2;
  CYC(b_+66, b_+69); goronDance_turnLinkToDirection_hook(gb);
  return;
landed:
  CYC(b_+69, b_+72); SET_HL(w1Link_yh);
  CALL_C(b_+72, objectCopyPosition_hook, SYM(objectCopyPosition), b_+75);
  CYC(b_+75, b_+76); alu_xor(gb, A);
  CYC(b_+76, b_+79); W8(wTmpcfc0_goronDance_linkJumping) = A;
  CYC(b_+79, b_+82); interactionDelete_hook(gb);
}

static void goron_run_script_and_delete(GB *gb, uint16_t sp0_) {
  BASE(goron_runScriptAndDeleteWhenFinished);
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+6); goron_faceLinkAndAnimateIfNotNapping_hook(gb);
}

void goronSubid04__afterCall780f_hook(GB *gb) {
  BASE(goronSubid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
  if (F & FC) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; }
  CYC(b_+14, b_+17); CYC(b_+17, b_+20); interactionAnimateAsNpc_hook(gb);
}

void goronSubid03_hook(GB *gb) {
  BASE(goronSubid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14); if (F & FC) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; } CYC(b_+14, b_+17); CYC(b_+17, b_+20); interactionAnimateAsNpc_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, goron_loadScriptAndInitGraphics_hook, SYM(goron_loadScriptAndInitGraphics), b_+8);
  goronSubid04__afterCall780f_hook(gb);
}

void goronSubid05_hook(GB *gb) {
  BASE(goronSubid05);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); CYC(b_+11, b_+13); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, goron_loadScriptFromTableAndInitGraphics_hook, SYM(goron_loadScriptFromTableAndInitGraphics), b_+8);
  goronSubid05__afterCall7823_hook(gb);
}

void goronSubid05__afterCall7823_hook(GB *gb) {
  BASE(goronSubid05);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
  CYC(b_+11, b_+13); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid06_hook(GB *gb) {
  BASE(goronSubid06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); CYC(b_+34, b_+36); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, goron_loadScriptFromTableAndInitGraphics_hook, SYM(goron_loadScriptFromTableAndInitGraphics), b_+8);
  goronSubid06__afterCall7830_hook(gb);
}

void goronSubid06__afterCall7830_hook(GB *gb) {
  BASE(goronSubid06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 10);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto clear; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); W8(wTmpcfc0_goronCutscenes_elderVar_cfdd) = A;
  CYC(b_+21, b_+23); goto run;
clear:
  CYC(b_+23, b_+25); B = 0x20;
  CYC(b_+25, b_+28); SET_HL(wTmpcfc0_goronCutscenes);
  CALL_C(b_+28, clearMemory_hook, SYM(clearMemory), b_+31);
run:
  CALL_C(b_+31, interactionRunScript_hook, SYM(interactionRunScript), b_+34);
  CYC(b_+34, b_+36); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid07_hook(GB *gb) {
  BASE(goronSubid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, goron_loadScriptAndInitGraphics_hook, SYM(goron_loadScriptAndInitGraphics), b_+8);
  goronSubid10__afterCall7854_hook(gb);
}

void goronSubid10__afterCall7854_hook(GB *gb) {
  BASE(goronSubid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), SYM(goron_runScriptAndDeleteWhenFinished));
  goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goron_runScriptAndDeleteWhenFinished_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goron_run_script_and_delete(gb, sp0_);
}

void goron_faceLinkAndAnimateIfNotNapping_hook(GB *gb) {
  BASE(goron_faceLinkAndAnimateIfNotNapping);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); npcFaceLinkAndAnimate_hook(gb); return; }
  CYC(b_+4, b_+7); CYC(b_+7, b_+10); interactionAnimateAsNpc_hook(gb);
}

void goronSubid09__afterCall7875_hook(GB *gb) {
  BASE(goronSubid09);
  uint16_t sp0_ = gb->sp;
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); W8(wTmpcfc0_targetCarts_cfdf) = A;
  CYC(b_+18, b_+21); W8(wTmpcfc0_targetCarts_beginGameTrigger) = A;
  CALL_C(b_+21, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+24);
  CYC(b_+24, b_+26); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+26, b_+28); goto run; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+31); SET_HL((SYM(dog_checkCloseToTargetPosition) + 16));
  CYC(b_+31, b_+33); E = 0x15;
  CALL_C(b_+33, interBankCall_hook, 0x008a, b_+36);
run:
  CALL_C(b_+36, interactionRunScript_hook, SYM(interactionRunScript), b_+39);
  CYC(b_+39, b_+41); CYC(b_+49, b_+51); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid09__afterCall7893_hook(GB *gb) {
  BASE(goronSubid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+44, interactionRunScript_hook, SYM(interactionRunScript), b_+47);
  CYC(b_+47, b_+49); CYC(b_+49, b_+51); goron_runScriptAndDeleteWhenFinished_hook(gb);
}

void goronSubid09_hook(GB *gb) {
  BASE(goronSubid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); CYC(b_+49, b_+51); goron_runScriptAndDeleteWhenFinished_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto right; }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, goron_loadScriptFromTableAndInitGraphics_hook, SYM(goron_loadScriptFromTableAndInitGraphics), b_+14);
  goronSubid09__afterCall7875_hook(gb); return;
right:
  CALL_C(b_+41, goron_loadScriptFromTableAndInitGraphics_hook, SYM(goron_loadScriptFromTableAndInitGraphics), b_+44);
  goronSubid09__afterCall7893_hook(gb);
}

void goronSubid0b__afterCall78a2_hook(GB *gb) {
  BASE(goronSubid0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
  if (F & FC) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+21, b_+22); ret_effect(gb); return; }
  CYC(b_+21, b_+22); CYC(b_+22, b_+24); goron_faceLinkAndAnimateIfNotNapping_hook(gb);
}

void goronSubid0b_hook(GB *gb) {
  BASE(goronSubid0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goronSubid0b__afterCall78a2_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, goron_loadScriptFromTableAndInitGraphics_hook, SYM(goron_loadScriptFromTableAndInitGraphics), b_+8);
  goronSubid0b__afterCall78a2_hook(gb);
}

void goronSubid0f__afterCall78ba_hook(GB *gb) {
  BASE(goronSubid0f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 8);
  CYC(b_+12, b_+15); SET_HL((SYM(goronDanceScriptTable) + 241));
  CALL_C(b_+15, interactionSetScript_hook, SYM(interactionSetScript), b_+18);
  CALL_C(b_+18, interactionRunScript_hook, SYM(interactionRunScript), b_+21);
  CALL_C(b_+21, interactionRunScript_hook, SYM(interactionRunScript), b_+24);
  if (F & FC) { CYCT(b_+24, b_+27); interactionDelete_hook(gb); return; }
  CYC(b_+24, b_+27); CYC(b_+27, b_+30); npcFaceLinkAndAnimate_hook(gb);
}

void goronSubid0f_hook(GB *gb) {
  BASE(goronSubid0f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); CALL_C(b_+21, interactionRunScript_hook, SYM(interactionRunScript), b_+24); if (F & FC) { CYCT(b_+24, b_+27); interactionDelete_hook(gb); return; } CYC(b_+24, b_+27); CYC(b_+27, b_+30); npcFaceLinkAndAnimate_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, goron_initGraphicsAndIncState_hook, SYM(goron_initGraphicsAndIncState), b_+8);
  goronSubid0f__afterCall78ba_hook(gb);
}

void goron_initGraphicsAndIncState_hook(GB *gb) {
  BASE(goron_initGraphicsAndIncState);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goron_initGraphics_hook, SYM(goron_initGraphics), b_+3);
  goron_initGraphicsAndIncState__afterCall7d75_hook(gb);
}

void goron_initGraphicsAndIncState__afterCall7d75_hook(GB *gb) {
  BASE(goron_initGraphicsAndIncState);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+3, b_+6); interactionIncState_hook(gb);
}

void goron_loadScriptAndInitGraphics_hook(GB *gb) {
  BASE(goron_loadScriptAndInitGraphics);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goron_initGraphics_hook, SYM(goron_initGraphics), b_+3);
  goron_loadScriptAndInitGraphics__afterCall7d7b_hook(gb);
}

void goron_loadScriptAndInitGraphics__afterCall7d7b_hook(GB *gb) {
  BASE(goron_loadScriptAndInitGraphics);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+3, b_+5); goron_loadScript_hook(gb);
}

void goron_loadScriptFromTableAndInitGraphics_hook(GB *gb) {
  BASE(goron_loadScriptFromTableAndInitGraphics);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, goron_initGraphics_hook, SYM(goron_initGraphics), b_+3);
  goron_loadScriptFromTableAndInitGraphics__afterCall7d80_hook(gb);
}

void goron_loadScriptFromTableAndInitGraphics__afterCall7d80_hook(GB *gb) {
  BASE(goron_loadScriptFromTableAndInitGraphics);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+3, b_+5); goron_loadScriptFromTable_hook(gb);
}

void goron_initGraphics_hook(GB *gb) {
  BASE(goron_initGraphics);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+3);
  CYC(b_+3, b_+6); interactionInitGraphics_hook(gb);
}

void goron_loadScript_hook(GB *gb) {
  BASE(goron_loadScript);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(goron_scriptTable));
  CYC(b_+6, b_+7); goron_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CALL_C(b_+10, interactionSetScript_hook, SYM(interactionSetScript), b_+13);
  CYC(b_+13, b_+16); interactionIncState_hook(gb);
}

void goron_loadScriptFromTable_hook(GB *gb) {
  BASE(goron_loadScriptFromTable);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(goron_scriptTable));
  CYC(b_+6, b_+7); goron_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+11); E = alu_inc8(gb, E);
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); goron_add_double_index(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CALL_C(b_+16, interactionSetScript_hook, SYM(interactionSetScript), b_+19);
  CYC(b_+19, b_+22); interactionIncState_hook(gb);
}

static void goron_subid01_face_down(GB *gb, uint16_t sp0_) {
  BASE(goronSubid01);
  CYC(b_+16, b_+18); A = 2;
  CALL_C(b_+18, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+21);
}

void goronSubid01__afterCall7778_hook(GB *gb) {
  BASE(goronSubid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+13, goron_loadScript_hook, SYM(goron_loadScript), b_+16);
  goron_subid01_face_down(gb, sp0_);
}

void goronSubid01_hook(GB *gb) {
  BASE(goronSubid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+21) { goto state1; }
    else if (jt_ == b_+41) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13);
  goronSubid01__afterCall7778_hook(gb);
  return;
state1:
  CYC(b_+21, b_+24); A = W8(wTmpcfc0_goronDance_linkStartedDance);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto goto_state2; }
  CYC(b_+25, b_+27);
  CALL_C(b_+27, interactionRunScript_hook, SYM(interactionRunScript), b_+30);
  if (F & FC) { CYCT(b_+30, b_+33); interactionDelete_hook(gb); return; }
  CYC(b_+30, b_+33); CYC(b_+33, b_+36); npcFaceLinkAndAnimate_hook(gb); return;
goto_state2:
  CALL_C(b_+36, interactionIncState_hook, SYM(interactionIncState), b_+39);
  CYC(b_+39, b_+41); goto update_animation;
state2:
  CYC(b_+41, b_+44); A = W8(wTmpcfc0_goronDance_linkStartedDance);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+45, b_+47); goto goto_state1; }
  CYC(b_+45, b_+47);
update_animation:
  CYC(b_+47, b_+50); SET_HL(w1Link_yh + 3);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_Z;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); E = alu_inc8(gb, E);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+60); A = W8(wTmpcfc0_goronDance_danceAnimation);
  CALL_C(b_+60, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+63);
  CYC(b_+63, b_+66); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return;
goto_state1:
  CYC(b_+66, b_+67); H = D;
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+69, b_+71); mem_wr(gb, HL, 1);
  CYC(b_+71, b_+74); goron_subid01_face_down(gb, sp0_);
}

static void goron_subid00_push_link_away(GB *gb) {
  BASE(goronSubid00);
  CYC(b_+182, b_+185); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

static void goron_subid00_select_script(GB *gb, uint16_t address, uint16_t return_address, uint16_t sp0_) {
  BANKOF(interactionCode66);
  CYC(address, address + 3); SET_HL(SYM(goronDanceScriptTable));
  CYC(address + 3, address + 4); goron_add_double_index(gb, address + 4);
  CYC(address + 4, address + 5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(address + 5, address + 6); H = mem_rd(gb, HL);
  CYC(address + 6, address + 7); L = A;
  CALL_C(address + 7, interactionSetScript_hook, SYM(interactionSetScript), return_address);
}

static void goron_subid00_reset_dance(GB *gb) {
  BASE(goronSubid00);
  CYC(b_+455, b_+456); alu_xor(gb, A);
  CYC(b_+456, b_+459); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(b_+459, b_+461); A = 2;
  CYC(b_+461, b_+464); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(b_+464, b_+467); goronDance_turnLinkToDirection_hook(gb);
}

static void goron_subid00_next_round(GB *gb) {
  BASE(goronSubid00);
  CYC(b_+429, b_+430); H = D;
  CYC(b_+430, b_+432); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+432, b_+434); mem_wr(gb, HL, 2);
  CYC(b_+434, b_+435); L = alu_inc8(gb, L);
  CYC(b_+435, b_+437); mem_wr(gb, HL, 0);
  CYC(b_+437, b_+439); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+439, b_+441); mem_wr(gb, HL, 30);
  CYC(b_+441, b_+443); goron_subid00_reset_dance(gb);
}

static void goron_subid00_end_dance(GB *gb) {
  BASE(goronSubid00);
  CYC(b_+443, b_+444); H = D;
  CYC(b_+444, b_+446); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+446, b_+448); mem_wr(gb, HL, 4);
  CYC(b_+448, b_+449); L = alu_inc8(gb, L);
  CYC(b_+449, b_+451); mem_wr(gb, HL, 0);
  CYC(b_+451, b_+453); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+453, b_+455); mem_wr(gb, HL, 60);
  goron_subid00_reset_dance(gb);
}

static void goron_subid00_run(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(goronSubid00);
  do { uint16_t jt_ = (entry);
    if (jt_ == b_+17) { goto after_graphics; }
    else if (jt_ == b_+76) { goto state1; }
    else if (jt_ == b_+107) { goto state2; }
    else if (jt_ == b_+278) { goto state3; }
    else if (jt_ == b_+467) { goto state4; }
    else { hook_continue(gb, entry, sp0_); return; }
  } while (0);
after_graphics:
  CYC(b_+17, b_+20); A = W8(wTilesetFlags);
  CYC(b_+20, b_+22); alu_and(gb, 0x80);
  CYC(b_+22, b_+24); A = 1;
  if (F & FZ) { CYCT(b_+24, b_+26); goto set_palette; }
  CYC(b_+24, b_+26); CYC(b_+26, b_+28); A = 2;
set_palette:
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+34); SET_HL((SYM(goronSubid04__state1) + 6));
  CALL_C(b_+34, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+37);
  if (F & FZ) { CYCT(b_+37, b_+39); goto load_dancers; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); A = W8(wTilesetFlags);
  CYC(b_+42, b_+44); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+44, b_+46); goto load_dancers; }
  CYC(b_+44, b_+46); CYC(b_+46, b_+49); SET_HL((SYM(goronSubid06__state0) + 23));
load_dancers:
  CALL_C(b_+49, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+52);
  CYC(b_+52, b_+54); B = 0x20;
  CYC(b_+54, b_+57); SET_HL(wTmpcfc0_goronDance);
  CALL_C(b_+57, clearMemory_hook, SYM(clearMemory), b_+60);
  CYC(b_+60, b_+62); A = 2;
  CYC(b_+62, b_+65); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC(b_+65, b_+66); alu_xor(gb, A);
  goron_subid00_select_script(gb, b_+66, b_+76, sp0_);
state1:
  CALL_C(b_+76, interactionRunScript_hook, SYM(interactionRunScript), b_+79);
  if (F & FC) { CYCT(b_+79, b_+82); goto script_done; }
  CYC(b_+79, b_+82); CYC(b_+82, b_+85); npcFaceLinkAndAnimate_hook(gb); return;
script_done:
  CYC(b_+85, b_+87); B = 10;
  CYC(b_+87, b_+90); SET_HL((SYM(interactionCode48__initSubid1d) + 42));
  CYC(b_+90, b_+92); E = 8;
  CALL_C(b_+92, interBankCall_hook, 0x008a, b_+95);
  CYC(b_+95, b_+97); A = 2;
  CYC(b_+97, b_+100); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(b_+100, interactionIncState_hook, SYM(interactionIncState), b_+103);
  CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 30);
state2:
  CYC(b_+107, b_+109); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+111); push_effect(gb, b_+111);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+121) { goto state2_wait; }
    else if (jt_ == b_+142) { goto state2_start; }
    else if (jt_ == b_+153) { goto state2_beat; }
    else if (jt_ == b_+229) { goto state2_jump; }
    else if (jt_ == b_+264) { goto state2_finished; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state2_wait:
  CALL_C(b_+121, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+124);
  if (!(F & FZ)) { CYCT(b_+124, b_+127); goron_subid00_push_link_away(gb); return; }
  CYC(b_+124, b_+127);
  CALL_C(b_+127, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+130);
  CYC(b_+130, b_+132); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+132, b_+134); mem_wr(gb, HL, 90);
  CYC(b_+134, b_+136); A = 0xcc;
  CALL_C(b_+136, playSound_b00_hook, SYM(playSound_b00), b_+139);
  CALL_C(b_+139, goronDance_initNextRound_hook, SYM(goronDance_initNextRound), b_+142);
state2_start:
  CALL_C(b_+142, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+145);
  if (!(F & FZ)) { CYCT(b_+145, b_+148); goron_subid00_push_link_away(gb); return; }
  CYC(b_+145, b_+148);
  CALL_C(b_+148, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+151);
  CYC(b_+151, b_+153); goto next_move;
state2_beat:
  CALL_C(b_+153, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+156);
  if (!(F & FZ)) { CYCT(b_+156, b_+158); goron_subid00_push_link_away(gb); return; }
  CYC(b_+156, b_+158);
  CALL_C(b_+158, goronDance_incBeat_hook, SYM(goronDance_incBeat), b_+161);
next_move:
  CALL_C(b_+161, goronDance_getNextMove_hook, SYM(goronDance_getNextMove), b_+164);
  if (!(F & FZ)) { CYCT(b_+164, b_+166); goto demonstration_finished; }
  CYC(b_+164, b_+166);
  CALL_C(b_+166, goronDance_updateConsecutiveBPressCounter_hook, SYM(goronDance_updateConsecutiveBPressCounter), b_+169);
  CALL_C(b_+169, goronDance_updateGracefulGoronAnimation_hook, SYM(goronDance_updateGracefulGoronAnimation), b_+172);
  if (F & FZ) { CYCT(b_+172, b_+174); goto jump; }
  CYC(b_+172, b_+174);
  CALL_C(b_+174, goronDance_playMoveSound_hook, SYM(goronDance_playMoveSound), b_+177);
  CYC(b_+177, b_+178); H = D;
  CYC(b_+178, b_+180); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 20);
  goron_subid00_push_link_away(gb); return;
jump:
  CYC(b_+185, b_+186); H = D;
  CYC(b_+186, b_+188); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+188, b_+190); mem_wr(gb, HL, 3);
  CYC(b_+190, b_+192); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+192, b_+194); mem_wr(gb, HL, 0x28);
  CYC(b_+194, b_+196); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0);
  CYC(b_+198, b_+199); SET_HL(HL + 1);
  CYC(b_+199, b_+201); mem_wr(gb, HL, 0xfe);
  CYC(b_+201, b_+203); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+203, b_+205); mem_wr(gb, HL, 20);
  CYC(b_+205, b_+207); A = 0xcd;
  CALL_C(b_+207, playSound_b00_hook, SYM(playSound_b00), b_+210);
  CYC(b_+210, b_+213); goron_subid00_push_link_away(gb); return;
demonstration_finished:
  CYC(b_+213, b_+214); H = D;
  CYC(b_+214, b_+216); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+216, b_+218); mem_wr(gb, HL, 4);
  CYC(b_+218, b_+220); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+220, b_+222); mem_wr(gb, HL, 60);
  CYC(b_+222, b_+224); A = 2;
  CALL_C(b_+224, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+227);
  CYC(b_+227, b_+229); goron_subid00_push_link_away(gb); return;
state2_jump:
  CALL_C(b_+229, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+232);
  CYC(b_+232, b_+234); C = 0x40;
  CALL_C(b_+234, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+237);
  if (F & FZ) { CYCT(b_+237, b_+239); goto landed; }
  CYC(b_+237, b_+239);
  CYC(b_+239, b_+240); H = D;
  CYC(b_+240, b_+242); L = INTERACTION_BASE + OBJ_SPEED_Z + 1;
  CYC(b_+242, b_+243); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+243, b_+244); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+244, b_+246); goron_subid00_push_link_away(gb); return; }
  CYC(b_+244, b_+246);
  CYC(b_+246, b_+248); A = 2;
  CYC(b_+248, b_+251); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(b_+251, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+254);
  CYC(b_+254, b_+256); goron_subid00_push_link_away(gb); return;
landed:
  CYC(b_+256, b_+257); H = D;
  CYC(b_+257, b_+259); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+259, b_+261); mem_wr(gb, HL, 2);
  CYC(b_+261, b_+264); goto state2_beat;
state2_finished:
  CALL_C(b_+264, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+267);
  if (!(F & FZ)) { CYCT(b_+267, b_+269); goron_subid00_push_link_away(gb); return; }
  CYC(b_+267, b_+269);
  CALL_C(b_+269, interactionIncState_hook, SYM(interactionIncState), b_+272);
  CYC(b_+272, b_+274); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+274, b_+276); mem_wr(gb, HL, 0);
  CYC(b_+276, b_+278); goron_subid00_push_link_away(gb); return;
state3:
  CYC(b_+278, b_+280); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+280, b_+281); A = mem_rd(gb, DE);
  CYC(b_+281, b_+282); push_effect(gb, b_+282);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+292) { goto state3_start; }
    else if (jt_ == b_+314) { goto state3_input; }
    else if (jt_ == b_+323) { goto state3_landing; }
    else if (jt_ == b_+340) { goto state3_round_end; }
    else if (jt_ == b_+365) { goto state3_failed; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state3_start:
  CALL_C(b_+292, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+295);
  CALL_C(b_+295, goronDance_clearDanceVariables_hook, SYM(goronDance_clearDanceVariables), b_+298);
  CYC(b_+298, b_+300); A = 0xcc;
  CALL_C(b_+300, playSound_b00_hook, SYM(playSound_b00), b_+303);
  CYC(b_+303, b_+305); A = 2;
  CYC(b_+305, b_+308); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CALL_C(b_+308, goronDance_turnLinkToDirection_hook, SYM(goronDance_turnLinkToDirection), b_+311);
  CYC(b_+311, b_+314); goron_subid00_push_link_away(gb); return;
state3_input:
  CALL_C(b_+314, goronDance_updateFrameCounter_hook, SYM(goronDance_updateFrameCounter), b_+317);
  CALL_C(b_+317, goronDance_checkLinkInput_hook, SYM(goronDance_checkLinkInput), b_+320);
  CYC(b_+320, b_+323); goron_subid00_push_link_away(gb); return;
state3_landing:
  CALL_C(b_+323, goronDance_updateFrameCounter_hook, SYM(goronDance_updateFrameCounter), b_+326);
  CYC(b_+326, b_+329); A = W8(wTmpcfc0_goronDance_linkJumping);
  CYC(b_+329, b_+330); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+330, b_+333); goron_subid00_push_link_away(gb); return; }
  CYC(b_+330, b_+333);
  CYC(b_+333, b_+334); H = D;
  CYC(b_+334, b_+336); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+336, b_+337); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+337, b_+340); goron_subid00_push_link_away(gb); return;
state3_round_end:
  CALL_C(b_+340, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+343);
  if (!(F & FZ)) { CYCT(b_+343, b_+346); goron_subid00_push_link_away(gb); return; }
  CYC(b_+343, b_+346);
  CYC(b_+346, b_+349); A = W8(wTmpcfc0_goronDance_roundIndex);
  CYC(b_+349, b_+351); alu_cp(gb, 8);
  if (F & FZ) { CYCT(b_+351, b_+353); goto end_dance; }
  CYC(b_+351, b_+353);
next_round:
  CYC(b_+353, b_+356); push_effect(gb, b_+356); goron_subid00_next_round(gb); return;
end_dance:
  CYC(b_+359, b_+362); push_effect(gb, b_+362); goron_subid00_end_dance(gb); return;
state3_failed:
  CYC(b_+365, b_+367); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+367, b_+368); A = mem_rd(gb, DE);
  CYC(b_+368, b_+369); push_effect(gb, b_+369);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+373) { goto initialize_failed_script; }
    else if (jt_ == b_+420) { goto run_failed_script; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
initialize_failed_script:
  CALL_C(b_+373, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+376);
  if (!(F & FZ)) { CYCT(b_+376, b_+379); goron_subid00_push_link_away(gb); return; }
  CYC(b_+376, b_+379);
  CYC(b_+379, b_+381); A = 1;
  CYC(b_+381, b_+384); W8(wTmpcfc0_goronDance_cfd9) = A;
  CYC(b_+384, b_+387); SET_HL(wTmpcfc0_goronDance_roundIndex);
  CYC(b_+387, b_+388); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+388, b_+391); SET_HL(wTmpcfc0_goronDance_numFailedRounds);
  CYC(b_+391, b_+392); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+392, b_+393); A = mem_rd(gb, HL);
  CYC(b_+393, b_+395); alu_cp(gb, 3);
  if (F & FZ) { CYCT(b_+395, b_+397); goto set_failed_script; }
  CYC(b_+395, b_+397);
  CYC(b_+397, b_+400); A = W8(wTmpcfc0_goronDance_roundIndex);
  CYC(b_+400, b_+402); alu_cp(gb, 8);
  if (F & FZ) { CYCT(b_+402, b_+404); goto end_dance; }
  CYC(b_+402, b_+404);
set_failed_script:
  CYC(b_+404, b_+405); H = D;
  CYC(b_+405, b_+407); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+407, b_+408); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+408, b_+410); A = 1;
  goron_subid00_select_script(gb, b_+410, b_+420, sp0_);
run_failed_script:
  CALL_C(b_+420, interactionRunScript_hook, SYM(interactionRunScript), b_+423);
  if (!(F & FC)) { CYCT(b_+423, b_+426); goron_subid00_push_link_away(gb); return; }
  CYC(b_+423, b_+426); CYC(b_+426, b_+429); goto next_round;
state4:
  CYC(b_+467, b_+469); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+469, b_+470); A = mem_rd(gb, DE);
  CYC(b_+470, b_+471); push_effect(gb, b_+471);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+475) { goto state4_start; }
    else if (jt_ == b_+494) { goto state4_script; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state4_start:
  CALL_C(b_+475, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+478);
  CYC(b_+478, b_+479); alu_xor(gb, A);
  CYC(b_+479, b_+482); W8(wTmpcfc0_goronDance_linkStartedDance) = A;
  CYC(b_+482, b_+484); A = 2;
  goron_subid00_select_script(gb, b_+484, b_+494, sp0_);
state4_script:
  CALL_C(b_+494, interactionRunScript_hook, SYM(interactionRunScript), b_+497);
  if (!(F & FC)) { CYCT(b_+497, b_+500); goron_subid00_push_link_away(gb); return; }
  CYC(b_+497, b_+500); CYC(b_+500, b_+503); goron_subid00_push_link_away(gb);
}

void goronSubid00__afterCall7585_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goron_subid00_run(gb, (SYM(goronSubid00__state0) + 3), sp0_);
}

void goronSubid00_hook(GB *gb) {
  BASE(goronSubid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+14) {
      CALL_C(b_+14, goron_initGraphicsAndIncState_hook, SYM(goron_initGraphicsAndIncState), b_+17);
      goronSubid00__afterCall7585_hook(gb);
      return;
    }
    else if (jt_ == b_+76) { goron_subid00_run(gb, b_+76, sp0_); return; }
    else if (jt_ == b_+107) { goron_subid00_run(gb, b_+107, sp0_); return; }
    else if (jt_ == b_+278) { goron_subid00_run(gb, b_+278, sp0_); return; }
    else if (jt_ == b_+467) { goron_subid00_run(gb, b_+467, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void goronDance_clearVariables_hook(GB *gb) {
  BASE(goronDance_clearVariables);
  uint16_t sp0_ = gb->sp;
  CYC15(b_+0, b_+2); B = 0x20;
  CYC15(b_+2, b_+5); SET_HL(wTmpcfc0_goronDance);
  CALL_C15(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CYC15(b_+8, b_+10); A = 2;
  CYC15(b_+10, b_+13); W8(wTmpcfc0_goronDance_danceAnimation) = A;
  CYC15(b_+13, b_+16); SET_HL(w1Link_yh - 3);
  CYC15(b_+16, b_+18); mem_wr(gb, HL, 2);
  CYC15(b_+18, b_+19); H = D;
  CYC15(b_+19, b_+21); L = INTERACTION_BASE + OBJ_STATE;
  CYC15(b_+21, b_+23); mem_wr(gb, HL, 1);
  CYC15(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC15(b_+24, b_+26); mem_wr(gb, HL, 0);
  CYC15(b_+26, b_+27); ret_effect(gb);
}

void goronDance_restartGame_hook(GB *gb) {
  BASE(goronDance_restartGame);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+1); alu_xor(gb, A);
  CYC15(b_+1, b_+4); W8(wTmpcfc0_goronDance_roundIndex) = A;
  CYC15(b_+4, b_+7); W8(wTmpcfc0_goronDance_numFailedRounds) = A;
  CYC15(b_+7, b_+10); SET_HL(w1Link_yh - 3);
  CYC15(b_+10, b_+12); mem_wr(gb, HL, 2);
  CYC15(b_+12, b_+14); B = 10;
  CYC15(b_+14, b_+17); SET_HL((SYM(interactionCode48__initSubid1d) + 42));
  CYC15(b_+17, b_+19); E = 8;
  CYC15(b_+19, b_+22); interBankCall_hook(gb);
}

void goronDance_initLinkPosition_hook(GB *gb) {
  BASE(goronDance_initLinkPosition);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+2); A = 2;
  CYC15(b_+2, b_+5); SET_BC((SYM(tokayScriptTable) + 49));
  CYC15(b_+5, b_+7); goron_setLinkPositionAndDirection_hook(gb);
}

void goronDance_checkNumFailedRounds_hook(GB *gb) {
  BASE(goronDance_checkNumFailedRounds);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+3); A = W8(wTmpcfc0_goronDance_numFailedRounds);
  CYC15(b_+3, b_+4); B = A;
  CYC15(b_+4, b_+6); A = 8;
  CYC15(b_+6, b_+7); alu_sub(gb, B);
  CYC15(b_+7, b_+10); SET_HL(wTextNumberSubstitution);
  CYC15(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC15(b_+11, b_+12); SET_HL(HL + 1);
  CYC15(b_+12, b_+14); mem_wr(gb, HL, 0);
  CYC15(b_+14, b_+17); A = W8(wTmpcfc0_goronDance_numFailedRounds);
  CYC15(b_+17, b_+18); alu_or(gb, A);
  CYC15(b_+18, b_+21); writeFlagsTocddb_hook(gb);
}

void goronDance_giveRandomRingPrize_hook(GB *gb) {
  BASE(goronDance_giveRandomRingPrize);
  uint16_t sp0_ = gb->sp;
  CYC15(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC15(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT15(b_+5, b_+7); goto past; }
  CYC15(b_+5, b_+7);
  CYC15(b_+7, b_+9); B = 2;
  CYC15(b_+9, b_+11); goto give_ring;
past:
  CYC15(b_+11, b_+13); B = 0;
  CYC15(b_+13, b_+16); A = W8(wTmpcfc0_goronDance_danceLevel);
  CYC15(b_+16, b_+18); alu_cp(gb, 0);
  if (F & FZ) { CYCT15(b_+18, b_+20); goto give_ring; }
  CYC15(b_+18, b_+20);
  CYC15(b_+20, b_+22); B = 2;
give_ring:
  CALL_C15(b_+22, getRandomNumber_hook, SYM(getRandomNumber), b_+25);
  CYC15(b_+25, b_+27); alu_and(gb, 1);
  CYC15(b_+27, b_+28); alu_add(gb, B);
  CYC15(b_+28, b_+31); SET_HL((SYM(subrosian_subid00__setScript) + 1));
  CYC15(b_+31, b_+32); goron_add_a_to_hl(gb, (SYM(subrosian_subid00__state0) + 41));
  CYC15(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC15(b_+33, b_+36); giveRingAToLink_hook(gb);
}

void goronElder_lookingUpAnimation_hook(GB *gb) {
  BASE(goronElder_lookingUpAnimation);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+1); H = D;
  CYC15(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC15(b_+3, b_+5); mem_wr(gb, HL, 1);
  CYC15(b_+5, b_+7); A = 4;
  CYC15(b_+7, b_+10); interactionSetAnimation_hook(gb);
}

void goronElder_normalAnimation_hook(GB *gb) {
  BASE(goronElder_normalAnimation);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+1); H = D;
  CYC15(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC15(b_+3, b_+5); mem_wr(gb, HL, 0);
  CYC15(b_+5, b_+7); A = 2;
  CYC15(b_+7, b_+10); interactionSetAnimation_hook(gb);
}

void goron_beginWalkingLeft_hook(GB *gb) {
  BASE(goron_beginWalkingLeft);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+1); H = D;
  CYC15(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SPEED;
  CYC15(b_+3, b_+5); mem_wr(gb, HL, 0x14);
  CYC15(b_+5, b_+7); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC15(b_+7, b_+9); mem_wr(gb, HL, 0x18);
  CYC15(b_+9, b_+11); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC15(b_+11, b_+13); mem_wr(gb, HL, 0x40);
  CYC15(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC15(b_+14, b_+16); mem_wr(gb, HL, 0);
  CYC15(b_+16, b_+18); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC15(b_+18, b_+20); mem_wr(gb, HL, 1);
  CYC15(b_+20, b_+22); A = 3;
  CYC15(b_+22, b_+24); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC15(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC15(b_+25, b_+28); interactionSetAnimation_hook(gb);
}

void goron_checkEnoughTimePassed_hook(GB *gb) {
  BASE(goron_checkEnoughTimePassed);
  uint16_t sp0_ = gb->sp;
  CYC15(b_+0, b_+3); A = W8(wSeedTreeRefilledBitset);
  CYC15(b_+3, b_+4); A = (uint8_t)~A;
  CYC15(b_+4, b_+6); alu_bit(gb, 0, A);
  CALL_C15(b_+6, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), SYM(goron_clearRefillBit));
  goron_clearRefillBit_hook(gb);
}

void goron_clearRefillBit_hook(GB *gb) {
  BASE(goron_clearRefillBit);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+3); SET_HL(wSeedTreeRefilledBitset);
  CYC15(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC15(b_+5, b_+6); ret_effect(gb);
}

void goron_checkInPast_hook(GB *gb) {
  BASE(goron_checkInPast);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC15(b_+3, b_+4); A = (uint8_t)~A;
  CYC15(b_+4, b_+6); alu_and(gb, 0x80);
  CYC15(b_+6, b_+9); writeFlagsTocddb_hook(gb);
}

void goron_checkInPresent_hook(GB *gb) {
  BASE(goron_checkInPresent);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC15(b_+3, b_+5); alu_and(gb, 0x80);
  CYC15(b_+5, b_+8); writeFlagsTocddb_hook(gb);
}

void goron_checkLinkInAir_hook(GB *gb) {
  BASE(goron_checkLinkInAir);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC15(b_+0, b_+3); A = W8(wLinkInAir);
  CYC15(b_+3, b_+4); alu_or(gb, A);
  CYC15(b_+4, b_+7); writeFlagsTocddb_hook(gb);
}
