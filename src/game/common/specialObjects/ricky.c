#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define rickyCliffOffsets_bank05 SYM(rickyCheckHopUpCliff__cliffOffset_oneUp_right)
#define rickyHoleCheckOffsets_bank05 SYM(rickyHoleCheckOffsets)

void rickyState0_hook(GB *gb);
void rickyState1_hook(GB *gb);
void rickyCheckHazards_hook(GB *gb);
void rickyState9_hook(GB *gb);
void rickyState2_hook(GB *gb);
void rickyState3_hook(GB *gb);
void rickyState4_hook(GB *gb);
void rickyState5_hook(GB *gb);
void rickyState5Substate0_hook(GB *gb);
void rickyCheckForHoleInFront_hook(GB *gb);
void rickyState5Substate1_hook(GB *gb);
void rickyState5Substate2_hook(GB *gb);
void rickyState5Substate3_hook(GB *gb);
void rickyState8_hook(GB *gb);
void rickyStartPunch_hook(GB *gb);
void rickyStopUntilLandedOnGround_hook(GB *gb);
void rickyCheckHazards2_hook(GB *gb);
void rickyBreakTilesOnLanding_hook(GB *gb);
void rickyCheckAtScreenEdge_hook(GB *gb);
void rickyState6_hook(GB *gb);
void rickyState7_hook(GB *gb);
void rickyFunc_70cc_hook(GB *gb);
void rickyStateA_hook(GB *gb);
void rickyStateASubstate0_hook(GB *gb);
void rickyStateASubstate1_hook(GB *gb);
void rickyStateASubstate2_hook(GB *gb);
void rickySetJumpSpeedForCutsceneAndSetAngle_hook(GB *gb);
void rickySetJumpSpeedForCutscene_hook(GB *gb);
void rickyStateASubstate6_hook(GB *gb);
void rickyStateASubstate3_hook(GB *gb);
void rickyStateASubstate5_hook(GB *gb);
void rickyStateASubstate4_hook(GB *gb);
void rickyIncVar03_hook(GB *gb);
void rickyStateASubstateB_hook(GB *gb);
void rickyStateASubstateC_hook(GB *gb);
void rickyWaitUntilJumpDone_hook(GB *gb);
void rickyStateC_hook(GB *gb);
void rickyCheckHopUpCliff_hook(GB *gb);
void rickyBeginJumpOverHole_hook(GB *gb);
void rickySetJumpSpeed_andcc91_hook(GB *gb);
void rickySetJumpSpeed_hook(GB *gb);

static uint16_t ricky_jump_table(GB *gb) {
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

static void ricky_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void ricky_run_state(GB *gb, uint16_t sp0_) {
  BASE(specialObjectCode_ricky);
  CYC(b_+12, b_+14); E = 0x04;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (ricky_jump_table(gb));
    if (jt_ == SYM(rickyState0) && hook_enabled_at(gb, SYM(rickyState0))) { rickyState0_hook(gb); return; }
    else if (jt_ == SYM(rickyState1) && hook_enabled_at(gb, SYM(rickyState1))) { rickyState1_hook(gb); return; }
    else if (jt_ == SYM(rickyState2) && hook_enabled_at(gb, SYM(rickyState2))) { rickyState2_hook(gb); return; }
    else if (jt_ == SYM(rickyState3) && hook_enabled_at(gb, SYM(rickyState3))) { rickyState3_hook(gb); return; }
    else if (jt_ == SYM(rickyState4) && hook_enabled_at(gb, SYM(rickyState4))) { rickyState4_hook(gb); return; }
    else if (jt_ == SYM(rickyState5) && hook_enabled_at(gb, SYM(rickyState5))) { rickyState5_hook(gb); return; }
    else if (jt_ == SYM(rickyState6) && hook_enabled_at(gb, SYM(rickyState6))) { rickyState6_hook(gb); return; }
    else if (jt_ == SYM(rickyState7) && hook_enabled_at(gb, SYM(rickyState7))) { rickyState7_hook(gb); return; }
    else if (jt_ == SYM(rickyState8) && hook_enabled_at(gb, SYM(rickyState8))) { rickyState8_hook(gb); return; }
    else if (jt_ == SYM(rickyState9) && hook_enabled_at(gb, SYM(rickyState9))) { rickyState9_hook(gb); return; }
    else if (jt_ == SYM(rickyStateA) && hook_enabled_at(gb, SYM(rickyStateA))) { rickyStateA_hook(gb); return; }
    else if (jt_ == SYM(rickyStateC) && hook_enabled_at(gb, SYM(rickyStateC))) { rickyStateC_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void specialObjectCode_ricky_hook(GB *gb) {
  BASE(specialObjectCode_ricky);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionRetIfInactive_hook, SYM(companionRetIfInactive), b_+3);
  CALL_C(b_+3, companionFunc_47d8_hook, SYM(companionFunc_47d8), b_+6);
  CYC(b_+6, b_+9); push_effect(gb, b_+9); ricky_run_state(gb, gb->sp);
  CYC(b_+9, b_+12); TAIL(companionCheckEnableTerrainEffects);
}

void rickyState0_hook(GB *gb) {
  BASE(rickyState0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckCanSpawn_hook, SYM(companionCheckCanSpawn), b_+3);
  CYC(b_+3, b_+5); A = 0x06;
  CALL_C(b_+5, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+8);
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+12); L = 0x08;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = 0x39;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x10);
  CYC(b_+18, b_+21); A = W8(wRickyState);
  CYC(b_+21, b_+23); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25);
    goto setAnimation17;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); C = 0x17;
  CYC(b_+27, b_+29); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+31);
    goto canTalk;
  }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto setAnimation17;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); C = 0x00;
canTalk:
  CYC(b_+37, b_+39); L = 0x04;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0a);
  CYC(b_+41, b_+43); E = 0x3d;
  CALL_C(b_+43, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+46);
  CYC(b_+46, b_+47); A = C;
  CYC(b_+47, b_+49);
  goto setAnimation;
setAnimation17:
  CYC(b_+49, b_+51); A = 0x17;
setAnimation:
  CALL_C(b_+51, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+54);
  CYC(b_+54, b_+57); TAIL(objectSetVisiblec1);
}

void rickyState1_hook(GB *gb) {
  BASE(rickyState1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CALL_C(b_+3, companionSetPriorityRelativeToLink_hook, SYM(companionSetPriorityRelativeToLink), b_+6);
  CYC(b_+6, b_+8); C = 0x09;
  CALL_C(b_+8, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+11);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
    goto didntMount;
  }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, companionTryToMount_hook, SYM(companionTryToMount), b_+16);
  if (F & FZ) { CYCT(b_+16, b_+17); ret_effect(gb); return; }
  CYC(b_+16, b_+17);
didntMount:
  CYC(b_+17, b_+19); E = 0x21;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_and(gb, 0xc0);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
    TAIL(rickyCheckHazards);
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); alu_rlca(gb);
  CYC(b_+25, b_+27); C = 0x40;
  if (!(F & FC)) {
    CYCT(b_+27, b_+30);
    TAIL(objectUpdateSpeedZ_paramC);
  }
  CYC(b_+27, b_+30);
  CYC(b_+30, b_+33); SET_BC(0xff00);
  CALL_C(b_+33, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), SYM(rickyCheckHazards));
  TAIL(rickyCheckHazards);
}

void rickyCheckHazards_hook(GB *gb) {
  BASE(rickyCheckHazards);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckHazards_hook, SYM(companionCheckHazards), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6);
    TAIL(rickyFunc_70cc);
  }
  CYC(b_+3, b_+6);
  TAIL(rickyState9);
}

void rickyState9_hook(GB *gb) {
  BASE(rickyState9);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void rickyState2_hook(GB *gb) {
  BASE(rickyState2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDecCounter1_hook, SYM(companionDecCounter1), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto update;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = 0xc3;
  CALL_C(b_+9, playSound_b00_hook, SYM(playSound_b00), b_+12);
update:
  CYC(b_+12, b_+14); C = 0x40;
  CALL_C(b_+14, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+17);
  CALL_C(b_+17, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+20);
  CALL_C(b_+20, objectApplySpeed_hook, SYM(objectApplySpeed), b_+23);
  CALL_C(b_+23, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+26);
  CYC(b_+26, b_+28); E = 0x33;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+31); alu_and(gb, 0x0f);
  CYC(b_+31, b_+33); E = 0x07;
  if (F & FZ) {
    CYCT(b_+33, b_+35);
    goto noWalls;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+37); ret_effect(gb); return;
noWalls:
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+39, b_+40); ret_effect(gb); return; }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+43); TAIL(rickyStopUntilLandedOnGround);
}

void rickyState3_hook(GB *gb) {
  BASE(rickyState3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x40;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, companionCheckMountingComplete_hook, SYM(companionCheckMountingComplete), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, companionFinalizeMounting_hook, SYM(companionFinalizeMounting), b_+12);
  CYC(b_+12, b_+14); A = 0xc3;
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+19); C = 0x20;
  CYC(b_+19, b_+22); TAIL(companionSetAnimation);
}

void rickyState4_hook(GB *gb) {
  BASE(rickyState4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x37;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0e);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto countdown;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x0d;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, companionDragToCenterOfHole_hook, SYM(companionDragToCenterOfHole), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
countdown:
  CALL_C(b_+14, companionDecCounter1_hook, SYM(companionDecCounter1), b_+17);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19);
    goto animate;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); E = 0x37;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CALL_C(b_+23, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+26);
  CYC(b_+26, b_+28); E = 0x37;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+31); alu_cp(gb, 0x0e);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
    goto animate;
  }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); A = 0x65;
  CYC(b_+35, b_+38); TAIL(playSound_b00);
animate:
  CALL_C(b_+38, companionAnimateDrowningOrFallingThenRespawn_hook, SYM(companionAnimateDrowningOrFallingThenRespawn), b_+41);
  if (!(F & FC)) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); C = 0x01;
  CYC(b_+44, b_+47); A = W8(wLinkObjectIndex);
  CYC(b_+47, b_+48); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+48, b_+50);
  } else {
    CYC(b_+48, b_+50);
    CYC(b_+50, b_+52); C = 0x05;
  }
  CYC(b_+52, b_+55); TAIL(companionUpdateDirectionAndSetAnimation);
}

void rickyState5_hook(GB *gb) {
  BASE(rickyState5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ricky_jump_table(gb));
    if (jt_ == SYM(rickyState5Substate0) && hook_enabled_at(gb, SYM(rickyState5Substate0))) { rickyState5Substate0_hook(gb); return; }
    else if (jt_ == SYM(rickyState5Substate1) && hook_enabled_at(gb, SYM(rickyState5Substate1))) { rickyState5Substate1_hook(gb); return; }
    else if (jt_ == SYM(rickyState5Substate2) && hook_enabled_at(gb, SYM(rickyState5Substate2))) { rickyState5Substate2_hook(gb); return; }
    else if (jt_ == SYM(rickyState5Substate3) && hook_enabled_at(gb, SYM(rickyState5Substate3))) { rickyState5Substate3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void rickyState5Substate0_hook(GB *gb) {
  BASE(rickyState5Substate0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wForceCompanionDismount);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto checkDismount;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wGameKeysJustPressed);
  CYC(b_+9, b_+11); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+14);
    TAIL(rickyStartPunch);
  }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); alu_bit(gb, 1, A);
checkDismount:
  if (!(F & FZ)) {
    CYCT(b_+16, b_+19);
    TAIL(companionGotoDismountState);
  }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+23); A = W8(wLinkAngle);
  CYC(b_+23, b_+25); L = 0x09;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+27); alu_rlca(gb);
  CYC(b_+27, b_+29); L = 0x39;
  if (!(F & FC)) {
    CYCT(b_+29, b_+31);
    goto moving;
  }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); A = 0x10;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); C = 0x20;
  CALL_C(b_+36, companionSetAnimation_hook, SYM(companionSetAnimation), b_+39);
  CYC(b_+39, b_+42); TAIL(rickyCheckHazards);
moving:
  CYC(b_+42, b_+44); L = 0x39;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+46, b_+48);
    goto tryToJump;
  }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+51); L = 0x10;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x1e);
  CYC(b_+53, b_+55); C = 0x20;
  CALL_C(b_+55, companionUpdateDirectionAndAnimate_hook, SYM(companionUpdateDirectionAndAnimate), b_+58);
  CALL_C(b_+58, rickyCheckForHoleInFront_hook, SYM(rickyCheckForHoleInFront), b_+61);
  if (F & FZ) {
    CYCT(b_+61, b_+64);
    TAIL(rickyBeginJumpOverHole);
  }
  CYC(b_+61, b_+64);
  CALL_C(b_+64, companionCheckHopDownCliff_hook, SYM(companionCheckHopDownCliff), b_+67);
  if (!(F & FZ)) {
    CYCT(b_+67, b_+69);
    goto checkHopUp;
  }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+72); TAIL(rickySetJumpSpeed);
checkHopUp:
  CALL_C(b_+72, rickyCheckHopUpCliff_hook, SYM(rickyCheckHopUpCliff), b_+75);
  if (!(F & FZ)) {
    CYCT(b_+75, b_+77);
    goto updateMovement;
  }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+80); TAIL(rickySetJumpSpeed_andcc91);
updateMovement:
  CALL_C(b_+80, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+83);
  CYC(b_+83, b_+86); TAIL(rickyCheckHazards);
tryToJump:
  CYC(b_+86, b_+87); H = D;
  CYC(b_+87, b_+89); L = 0x09;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+90, b_+91); alu_add(gb, A);
  CYC(b_+91, b_+93); A = alu_swap(gb, A);
  CYC(b_+93, b_+95); alu_and(gb, 0x03);
  CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(b_+96, rickySetJumpSpeed_andcc91_hook, SYM(rickySetJumpSpeed_andcc91), b_+99);
  CYC(b_+99, b_+101); L = 0x09;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+104); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(b_+104, b_+106);
    goto jump;
  }
  CYC(b_+104, b_+106);
  CALL_C(b_+106, companionCheckHopDownCliff_hook, SYM(companionCheckHopDownCliff), b_+109);
  if (!(F & FZ)) {
    CYCT(b_+109, b_+111);
    goto checkHopUpForJump;
  }
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+114); W8(wDisableScreenTransitions) = A;
  CYC(b_+114, b_+116); C = 0x0f;
  CYC(b_+116, b_+119); TAIL(companionSetAnimation);
checkHopUpForJump:
  CALL_C(b_+119, rickyCheckHopUpCliff_hook, SYM(rickyCheckHopUpCliff), b_+122);
  CYC(b_+122, b_+124); C = 0x0f;
  if (F & FZ) {
    CYCT(b_+124, b_+127);
    TAIL(companionSetAnimation);
  }
  CYC(b_+124, b_+127);
jump:
  CYC(b_+127, b_+129); E = 0x05;
  CYC(b_+129, b_+131); A = 0x02;
  CYC(b_+131, b_+132); mem_wr(gb, DE, A);
  CALL_C(b_+132, rickyCheckForHoleInFront_hook, SYM(rickyCheckForHoleInFront), b_+135);
  if (F & FZ) {
    CYCT(b_+135, b_+138);
    TAIL(rickyBeginJumpOverHole);
  }
  CYC(b_+135, b_+138);
  CYC(b_+138, b_+141); SET_BC(0xfe80);
  CALL_C(b_+141, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+144);
  CYC(b_+144, b_+146); L = 0x05;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x01);
  CYC(b_+148, b_+150); L = 0x06;
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x08);
  CYC(b_+152, b_+154); L = 0x10;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x50);
  CYC(b_+156, b_+158); C = 0x19;
  CALL_C(b_+158, companionSetAnimation_hook, SYM(companionSetAnimation), b_+161);
  CALL_C(b_+161, getRandomNumber_hook, SYM(getRandomNumber), b_+164);
  CYC(b_+164, b_+166); alu_and(gb, 0x0f);
  CYC(b_+166, b_+168); A = 0x53;
  if (!(F & FZ)) {
    CYCT(b_+168, b_+170);
  } else {
    CYC(b_+168, b_+170);
    CYC(b_+170, b_+172); A = 0xc3;
  }
  CYC(b_+172, b_+175); TAIL(playSound_b00);
}

void rickyCheckForHoleInFront_hook(GB *gb) {
  BASE(rickyCheckForHoleInFront);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkAngle);
  CYC(b_+3, b_+5); alu_and(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x08;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(SYM(rickyHoleCheckOffsets));
  CYC(b_+12, b_+13); ricky_add_double_index(gb, b_+13);
  CYC(b_+13, b_+15); E = 0x0b;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); H8(hFF90) = A;
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+22); SET_HL(HL + 1);
  CYC(b_+22, b_+24); E = 0x0d;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+26, b_+28); H8(hFF91) = A;
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+30); C = A;
  CALL_C(b_+30, getTileAtPosition_hook, SYM(getTileAtPosition), b_+33);
  CYC(b_+33, b_+34); A = L;
  CYC(b_+34, b_+36); E = 0x36;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+39); A = H8(hFF90);
  CYC(b_+39, b_+40); B = A;
  CYC(b_+40, b_+42); A = H8(hFF91);
  CYC(b_+42, b_+43); C = A;
  CALL_C(b_+43, getTileAtPosition_hook, SYM(getTileAtPosition), b_+46);
  CYC(b_+46, b_+48); H = 0xcf;
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); alu_cp(gb, 0xf3);
  if (F & FZ) { CYCT(b_+51, b_+52); ret_effect(gb); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); alu_cp(gb, 0xfd);
  CYC(b_+54, b_+55); ret_effect(gb);
}

void rickyState5Substate1_hook(GB *gb) {
  BASE(rickyState5Substate1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_dec8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+4); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto doneInputParsing;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wGameKeysJustPressed);
  CYC(b_+9, b_+11); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+14);
    TAIL(rickyStartPunch);
  }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); A = W8(wLinkAngle);
  CYC(b_+17, b_+19); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto doneInputParsing;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); SET_HL(w1Companion_direction);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+26); alu_add(gb, A);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+30); alu_and(gb, 0x03);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+33); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); C = 0x19;
  if (!(F & FZ)) CALL_C_CC(b_+36, companionSetAnimation_hook, SYM(companionSetAnimation), b_+39);
  else CYC(b_+36, b_+39);
doneInputParsing:
  CYC(b_+39, b_+41); C = 0x40;
  CALL_C(b_+41, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+44);
  if (F & FZ) {
    CYCT(b_+44, b_+46);
    goto landed;
  }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+49); A = W8(wLinkObjectIndex);
  CYC(b_+49, b_+50); alu_rra(gb);
  if (!(F & FC)) {
    CYCT(b_+50, b_+52);
    goto checkHole;
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); A = W8(wLinkAngle);
  CYC(b_+55, b_+57); alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+57, b_+59);
    goto updateMovement;
  }
  CYC(b_+57, b_+59);
checkHole:
  CYC(b_+59, b_+62); SET_HL(SYM(rickyHoleCheckOffsets));
  CALL_C(b_+62, specialObjectGetRelativeTileWithDirectionTable_hook, SYM(specialObjectGetRelativeTileWithDirectionTable), b_+65);
  CYC(b_+65, b_+66); A = B;
  CYC(b_+66, b_+68); alu_cp(gb, 0xf3);
  if (F & FZ) { CYCT(b_+68, b_+69); ret_effect(gb); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); alu_cp(gb, 0xfd);
  if (F & FZ) { CYCT(b_+71, b_+72); ret_effect(gb); return; }
  CYC(b_+71, b_+72);
updateMovement:
  CYC(b_+72, b_+75); TAIL(companionUpdateMovement);
landed:
  CALL_C(b_+75, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+78);
  CALL_C(b_+78, companionDecCounter1IfNonzero_hook, SYM(companionDecCounter1IfNonzero), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+82); ret_effect(gb); return; }
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+85); TAIL(rickyStopUntilLandedOnGround);
}

void rickyState5Substate2_hook(GB *gb) {
  BASE(rickyState5Substate2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDecCounter1_hook, SYM(companionDecCounter1), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto update;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = 0xc3;
  CALL_C(b_+9, playSound_b00_hook, SYM(playSound_b00), b_+12);
update:
  CYC(b_+12, b_+14); C = 0x40;
  CALL_C(b_+14, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+17);
  if (F & FZ) {
    CYCT(b_+17, b_+20);
    TAIL(rickyStopUntilLandedOnGround);
  }
  CYC(b_+17, b_+20);
  CALL_C(b_+20, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+23);
  CALL_C(b_+23, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+26);
  CALL_C(b_+26, specialObjectCheckMovingTowardWall_hook, SYM(specialObjectCheckMovingTowardWall), b_+29);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+32);
    TAIL(rickyStopUntilLandedOnGround);
  }
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+33); ret_effect(gb);
}

void rickyState5Substate3_hook(GB *gb) {
  BASE(rickyState5Substate3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x40;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, rickyBreakTilesOnLanding_hook, SYM(rickyBreakTilesOnLanding), b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+12); E = 0x05;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); TAIL(rickyCheckHazards2);
}

void rickyState8_hook(GB *gb) {
  BASE(rickyState8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ricky_jump_table(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+71) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+8, b_+10); C = 0x40;
  CALL_C(b_+10, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+13);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    goto onGround;
  }
  CYC(b_+13, b_+15);
  CALL_C(b_+15, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+18);
  CYC(b_+18, b_+20);
  goto animate;
onGround:
  CALL_C(b_+20, companionTryToBreakTileFromMoving_hook, SYM(companionTryToBreakTileFromMoving), b_+23);
  CALL_C(b_+23, rickyCheckHazards_hook, SYM(rickyCheckHazards), b_+26);
animate:
  CALL_C(b_+26, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+29);
  CYC(b_+29, b_+31); E = 0x21;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_and(gb, 0xc0);
  if (F & FZ) { CYCT(b_+34, b_+35); ret_effect(gb); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+36, b_+38);
    goto startTornadoCharge;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); A = 0x75;
  CYC(b_+40, b_+43); TAIL(playSound_b00);
startTornadoCharge:
  CYC(b_+43, b_+45); E = 0x0f;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+51); A = W8(wGameKeysPressed);
  CYC(b_+51, b_+53); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+53, b_+56);
    TAIL(rickyStopUntilLandedOnGround);
  }
  CYC(b_+53, b_+56);
  CALL_C(b_+56, itemIncSubstate_hook, SYM(itemIncSubstate), b_+59);
  CYC(b_+59, b_+61); C = 0x13;
  CALL_C(b_+61, companionSetAnimation_hook, SYM(companionSetAnimation), b_+64);
  CALL_C(b_+64, companionCheckHazards_hook, SYM(companionCheckHazards), b_+67);
  if (!(F & FC)) { CYCT(b_+67, b_+68); ret_effect(gb); return; }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+71); TAIL(rickyFunc_70cc);
substate1:
  CYC(b_+71, b_+74); A = W8(wLinkAngle);
  CYC(b_+74, b_+76); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+78);
    goto animateCharge;
  }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+81); SET_HL(w1Companion_angle);
  CYC(b_+81, b_+82); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
  CYC(b_+83, b_+85); C = 0x13;
  if (!(F & FZ)) CALL_C_CC(b_+85, companionUpdateDirectionAndAnimate_hook, SYM(companionUpdateDirectionAndAnimate), b_+88);
  else CYC(b_+85, b_+88);
animateCharge:
  CALL_C(b_+88, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+91);
  CYC(b_+91, b_+94); A = W8(wGameKeysPressed);
  CYC(b_+94, b_+96); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+96, b_+98);
    goto releasedAButton;
  }
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+100); E = 0x35;
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+103); alu_cp(gb, 0x1e);
  if (!(F & FZ)) {
    CYCT(b_+103, b_+105);
    goto continueCharging;
  }
  CYC(b_+103, b_+105);
  CALL_C(b_+105, companionTryToBreakTileFromMoving_hook, SYM(companionTryToBreakTileFromMoving), b_+108);
  CALL_C(b_+108, rickyCheckHazards_hook, SYM(rickyCheckHazards), b_+111);
  CYC(b_+111, b_+113); C = 0x04;
  CYC(b_+113, b_+116); TAIL(companionFlashFromChargingAnimation);
continueCharging:
  CYC(b_+116, b_+117); A = alu_inc8(gb, A);
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
  CYC(b_+118, b_+120); alu_cp(gb, 0x1e);
  if (!(F & FZ)) { CYCT(b_+120, b_+121); ret_effect(gb); return; }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); A = 0x4f;
  CYC(b_+123, b_+126); TAIL(playSound_b00);
releasedAButton:
  CYC(b_+126, b_+129); SET_HL(w1Link_oamFlagsBackup);
  CYC(b_+129, b_+130); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+130, b_+131); mem_wr(gb, HL, A);
  CYC(b_+131, b_+133); E = 0x35;
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+136); alu_cp(gb, 0x1e);
  if (!(F & FZ)) {
    CYCT(b_+136, b_+138);
    goto notCharged;
  }
  CYC(b_+136, b_+138);
  CYC(b_+138, b_+141); SET_BC(0x2a00);
  CALL_C(b_+141, companionCreateItem_hook, SYM(companionCreateItem), b_+144);
  CYC(b_+144, b_+146); A = 0xf1;
  CALL_C(b_+146, playSound_b00_hook, SYM(playSound_b00), b_+149);
  CYC(b_+149, b_+151); A = 0x6b;
  CALL_C(b_+151, playSound_b00_hook, SYM(playSound_b00), b_+154);
  CYC(b_+154, b_+156);
  TAIL(rickyStartPunch);
notCharged:
  CYC(b_+156, b_+158); C = 0x05;
  CYC(b_+158, b_+161); TAIL(companionSetAnimationAndGotoState5);
}

void rickyStartPunch_hook(GB *gb) {
  BASE(rickyStartPunch);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x2800);
  CALL_C(b_+3, companionCreateWeaponItem_hook, SYM(companionCreateWeaponItem), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = 0x04;
  CYC(b_+10, b_+12); A = 0x08;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+18); L = 0x35;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); C = 0x09;
  CALL_C(b_+21, companionSetAnimation_hook, SYM(companionSetAnimation), b_+24);
  CYC(b_+24, b_+26); A = 0x74;
  CYC(b_+26, b_+29); TAIL(playSound_b00);
}

void rickyState6_hook(GB *gb) {
  BASE(rickyState6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ricky_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+27) { goto substate1; }
    else if (jt_ == b_+35) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+10, b_+12); C = 0x40;
  CALL_C(b_+12, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+16); ret_effect(gb); return; }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, itemIncSubstate_hook, SYM(itemIncSubstate), b_+19);
  CALL_C(b_+19, companionDismountAndSavePosition_hook, SYM(companionDismountAndSavePosition), b_+22);
  CYC(b_+22, b_+24); A = 0x17;
  CYC(b_+24, b_+27); TAIL(specialObjectSetAnimation);
substate1:
  CYC(b_+27, b_+30); A = W8(wLinkInAir);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+32); ret_effect(gb); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+35); TAIL(itemIncSubstate);
substate2:
  CALL_C(b_+35, companionSetPriorityRelativeToLink_hook, SYM(companionSetPriorityRelativeToLink), b_+38);
  CYC(b_+38, b_+40); C = 0x09;
  CALL_C(b_+40, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+43);
  if (F & FC) {
    CYCT(b_+43, b_+46);
    TAIL(rickyCheckHazards);
  }
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+48); E = 0x05;
  CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+51); E = alu_dec8(gb, E);
  CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+54); ret_effect(gb);
}

void rickyState7_hook(GB *gb) {
  BASE(rickyState7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDecCounter1ToJumpDownCliff_hook, SYM(companionDecCounter1ToJumpDownCliff), b_+3);
  if (F & FC) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+7);
  CALL_C(b_+7, specialObjectCheckMovingAwayFromWall_hook, SYM(specialObjectCheckMovingAwayFromWall), b_+10);
  CYC(b_+10, b_+12); E = 0x07;
  if (F & FZ) {
    CYCT(b_+12, b_+14);
    goto noWalls;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); ret_effect(gb); return;
noWalls:
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);
  TAIL(rickyStopUntilLandedOnGround);
}

void rickyStopUntilLandedOnGround_hook(GB *gb) {
  BASE(rickyStopUntilLandedOnGround);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+4, b_+6);
    goto setState;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+10); W8(wLinkInAir) = A;
  CYC(b_+10, b_+13); W8(wDisableScreenTransitions) = A;
setState:
  CYC(b_+13, b_+15); A = 0x05;
  CYC(b_+15, b_+17); E = 0x04;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+20); A = 0x03;
  CYC(b_+20, b_+22); E = 0x05;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, rickyCheckAtScreenEdge_hook, SYM(rickyCheckAtScreenEdge), b_+26);
  if (F & FZ) {
    CYCT(b_+26, b_+28);
    TAIL(rickyCheckHazards2);
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = 0x39;
  CYC(b_+30, b_+32); A = 0x10;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  TAIL(rickyCheckHazards2);
}

void rickyCheckHazards2_hook(GB *gb) {
  BASE(rickyCheckHazards2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckHazards_hook, SYM(companionCheckHazards), b_+3);
  CYC(b_+3, b_+5); C = 0x20;
  if (!(F & FC)) {
    CYCT(b_+5, b_+8);
    TAIL(companionSetAnimation);
  }
  CYC(b_+5, b_+8);
  TAIL(rickyFunc_70cc);
}

void rickyFunc_70cc_hook(GB *gb) {
  BASE(rickyFunc_70cc);
  CYC(b_+0, b_+2); C = 0x0e;
  CYC(b_+2, b_+4); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto setHazard;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); C = 0x0d;
setHazard:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = 0x37;
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+14); L = 0x06;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x00);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void rickyStateA_hook(GB *gb) {
  BASE(rickyStateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x03;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (ricky_jump_table(gb));
    if (jt_ == SYM(rickyStateASubstate0) && hook_enabled_at(gb, SYM(rickyStateASubstate0))) { rickyStateASubstate0_hook(gb); return; }
    else if (jt_ == SYM(rickyStateASubstate1) && hook_enabled_at(gb, SYM(rickyStateASubstate1))) { rickyStateASubstate1_hook(gb); return; }
    else if (jt_ == SYM(rickyStateASubstate2) && hook_enabled_at(gb, SYM(rickyStateASubstate2))) { rickyStateASubstate2_hook(gb); return; }
    else if (jt_ == SYM(rickyStateASubstate3) && hook_enabled_at(gb, SYM(rickyStateASubstate3))) { rickyStateASubstate3_hook(gb); return; }
    else if (jt_ == SYM(rickyStateASubstate4) && hook_enabled_at(gb, SYM(rickyStateASubstate4))) { rickyStateASubstate4_hook(gb); return; }
    else if (jt_ == SYM(rickyStateASubstate5) && hook_enabled_at(gb, SYM(rickyStateASubstate5))) { rickyStateASubstate5_hook(gb); return; }
    else if (jt_ == SYM(rickyStateASubstate6) && hook_enabled_at(gb, SYM(rickyStateASubstate6))) { rickyStateASubstate6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void rickyStateASubstate0_hook(GB *gb) {
  BASE(rickyStateASubstate0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionPreventLinkFromPassing_noExtraChecks_hook, SYM(companionPreventLinkFromPassing_noExtraChecks), b_+3);
  CALL_C(b_+3, companionSetPriorityRelativeToLink_hook, SYM(companionSetPriorityRelativeToLink), b_+6);
  CALL_C(b_+6, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+9);
  CYC(b_+9, b_+11); E = 0x21;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_rlca(gb);
  CYC(b_+13, b_+15); C = 0x40;
  if (!(F & FC)) {
    CYCT(b_+15, b_+18);
    TAIL(objectUpdateSpeedZ_paramC);
  }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+21); SET_BC(0xff00);
  CYC(b_+21, b_+24); TAIL(objectSetSpeedZ);
}

void rickyStateASubstate1_hook(GB *gb) {
  BASE(rickyStateASubstate1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3d;
  CALL_C(b_+2, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+5);
  CYC(b_+5, b_+8); TAIL(companionForceMount);
}

void rickyStateASubstate2_hook(GB *gb) {
  BASE(rickyStateASubstate2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x40;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_BC(0x2006);
  CALL_C(b_+9, showText_hook, SYM(showText), b_+12);
  CYC(b_+12, b_+15); SET_HL(w1Link_yh);
  CYC(b_+15, b_+17); E = 0x0b;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); A = 0x02;
  if (F & FC) {
    CYCT(b_+21, b_+23);
    goto setDirection;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x00;
setDirection:
  CYC(b_+25, b_+27); E = 0x08;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); A = 0x03;
  CYC(b_+30, b_+32); E = 0x3f;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CALL_C(b_+33, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+36);
  CALL_C(b_+36, rickyIncVar03_hook, SYM(rickyIncVar03), b_+39);
  CYC(b_+39, b_+41); TAIL(rickySetJumpSpeedForCutscene);
}

void rickySetJumpSpeedForCutsceneAndSetAngle_hook(GB *gb) {
  BASE(rickySetJumpSpeedForCutsceneAndSetAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x30;
  CYC(b_+2, b_+4); C = 0x58;
  CALL_C(b_+4, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x1c);
  CYC(b_+9, b_+11); E = 0x09;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  TAIL(rickySetJumpSpeedForCutscene);
}

void rickySetJumpSpeedForCutscene_hook(GB *gb) {
  BASE(rickySetJumpSpeedForCutscene);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xfe80);
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+8); L = 0x05;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x01);
  CYC(b_+10, b_+12); L = 0x10;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x50);
  CYC(b_+14, b_+16); L = 0x06;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x08);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void rickyStateASubstate6_hook(GB *gb) {
  BASE(rickyStateASubstate6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x21;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  CYC(b_+7, b_+9); A = 0xc3;
  if (F & FZ) {
    CYCT(b_+9, b_+12);
    TAIL(playSound_b00);
  }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, rickySetJumpSpeedForCutsceneAndSetAngle_hook, SYM(rickySetJumpSpeedForCutsceneAndSetAngle), b_+18);
  CYC(b_+18, b_+20); E = 0x09;
  CYC(b_+20, b_+22); A = 0x10;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); C = 0x05;
  CALL_C(b_+25, companionSetAnimation_hook, SYM(companionSetAnimation), b_+28);
  CYC(b_+28, b_+31); TAIL(rickyIncVar03);
}

void rickyStateASubstate3_hook(GB *gb) {
  BASE(rickyStateASubstate3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+5); A = 0x14;
  CYC(b_+5, b_+7); E = 0x09;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); E = alu_dec8(gb, E);
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); C = 0x05;
  CALL_C(b_+14, companionSetAnimation_hook, SYM(companionSetAnimation), b_+17);
  CYC(b_+17, b_+20); TAIL(rickyIncVar03);
}

void rickyStateASubstate5_hook(GB *gb) {
  BASE(rickyStateASubstate5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  CYC(b_+6, b_+8); C = 0x40;
  CALL_C(b_+8, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x18;
  CALL_C(b_+14, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+17);
  CYC(b_+17, b_+20); TAIL(rickyIncVar03);
}

void rickyStateASubstate4_hook(GB *gb) {
  BASE(rickyStateASubstate4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionSetAnimationToVar3f_hook, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, rickyWaitUntilJumpDone_hook, SYM(rickyWaitUntilJumpDone), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = 0x18;
  CYC(b_+9, b_+11); E = 0x09;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, specialObjectCheckMovingTowardWall_hook, SYM(specialObjectCheckMovingTowardWall), b_+15);
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    goto hop;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x10;
  CYC(b_+19, b_+21); E = 0x09;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, specialObjectCheckMovingTowardWall_hook, SYM(specialObjectCheckMovingTowardWall), b_+25);
  if (F & FZ) {
    CYCT(b_+25, b_+27);
    goto hop;
  }
  CYC(b_+25, b_+27);
  CALL_C(b_+27, rickySetJumpSpeed_hook, SYM(rickySetJumpSpeed), b_+30);
  CYC(b_+30, b_+32); A = 0x53;
  CALL_C(b_+32, playSound_b00_hook, SYM(playSound_b00), b_+35);
  CYC(b_+35, b_+38); rickyIncVar03_hook(gb);
  return;

hop:
  CALL_C(b_+38, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+41);
  if (!(F & FC)) {
    CYCT(b_+41, b_+43);
    goto left_screen;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); E = 0x03;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+48); alu_cp(gb, 0x07);
  CYC(b_+48, b_+50); A = 0x10;
  if (F & FZ) {
    CYCT(b_+50, b_+52);
    goto set_angle;
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); A = 0x14;

set_angle:
  CYC(b_+54, b_+56); E = 0x09;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+60); rickySetJumpSpeedForCutscene_hook(gb);
  return;

left_screen:
  CYC(b_+60, b_+61); alu_xor(gb, A);
  CYC(b_+61, b_+64); W8(wDisabledObjects) = A;
  CYC(b_+64, b_+67); W8(wMenuDisabled) = A;
  CYC(b_+67, b_+70); W8(wDeathRespawnBuffer_rememberedCompanionId) = A;
  CALL_C(b_+70, itemDelete_hook, SYM(itemDelete), b_+73);
  CYC(b_+73, b_+76); SET_HL(wRickyState);
  CYC(b_+76, b_+78); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(b_+78, b_+81); TAIL(saveLinkLocalRespawnAndCompanionPosition);
}

void rickyIncVar03_hook(GB *gb) {
  BASE(rickyIncVar03);
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void rickyStateASubstateB_hook(GB *gb) {
  BASE(rickyStateASubstateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CALL_C(b_+3, companionDismount_hook, SYM(companionDismount), b_+6);
  CYC(b_+6, b_+8); A = 0x18;
  CYC(b_+8, b_+11); W8(w1Link_angle) = A;
  CYC(b_+11, b_+14); W8(wLinkAngle) = A;
  CYC(b_+14, b_+16); A = 0x32;
  CYC(b_+16, b_+19); W8(w1Link_speed) = A;
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = 0x09;
  CYC(b_+22, b_+24); A = 0x18;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+25, b_+27); A = 0x03;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+28, b_+30); A = 0x1e;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); A = 0x24;
  CALL_C(b_+33, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+36);
  CYC(b_+36, b_+38); TAIL(rickyIncVar03);
}

void rickyStateASubstateC_hook(GB *gb) {
  BASE(rickyStateASubstateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkInAir);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+8);
  CYC(b_+8, b_+11); SET_HL(w1Link_xh);
  CYC(b_+11, b_+13); E = 0x0d;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto facing_right;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); A = 0x01;
  if (!(F & FC)) {
    CYCT(b_+21, b_+23);
    goto set_direction;
  }
  CYC(b_+21, b_+23);

facing_right:
  CYC(b_+23, b_+25); A = 0x03;

set_direction:
  CYC(b_+25, b_+27); L = 0x08;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); E = 0x07;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+32, b_+34);
    goto move_companion;
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); A = alu_dec8(gb, A);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+37); ret_effect(gb); return;

move_companion:
  CALL_C(b_+37, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+40);
  CALL_C(b_+40, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+43);
  CALL_C(b_+43, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+46);
  if (F & FC) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); alu_xor(gb, A);
  CYC(b_+48, b_+51); W8(wRememberedCompanionId) = A;
  CYC(b_+51, b_+54); W8(wDisabledObjects) = A;
  CYC(b_+54, b_+57); W8(wMenuDisabled) = A;
  CYC(b_+57, b_+60); TAIL(itemDelete);
}

void rickyWaitUntilJumpDone_hook(GB *gb) {
  BASE(rickyWaitUntilJumpDone);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x40;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto on_ground;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+10);
  CYC(b_+10, b_+11); alu_or(gb, D);
  CYC(b_+11, b_+12); ret_effect(gb); return;

on_ground:
  CYC(b_+12, b_+14); C = 0x05;
  CALL_C(b_+14, companionSetAnimation_hook, SYM(companionSetAnimation), b_+17);
  CYC(b_+17, b_+20); TAIL(companionDecCounter1IfNonzero);
}

void rickyStateC_hook(GB *gb) {
  BASE(rickyStateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ricky_jump_table(gb));
    if (jt_ == b_+8) { goto parameter0; }
    else if (jt_ == b_+26) { goto parameter1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

parameter0:
  CALL_C(b_+8, companionInitializeOnEnteringScreen_hook, SYM(companionInitializeOnEnteringScreen), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x02);
  CALL_C(b_+13, rickySetJumpSpeedForCutscene_hook, SYM(rickySetJumpSpeedForCutscene), b_+16);
  CYC(b_+16, b_+18); A = 0xc3;
  CALL_C(b_+18, playSound_b00_hook, SYM(playSound_b00), b_+21);
  CYC(b_+21, b_+23); C = 0x01;
  CYC(b_+23, b_+26); companionSetAnimation_hook(gb);
  return;

parameter1:
  CALL_C(b_+26, rickyState5_hook, SYM(rickyState5), b_+29);
  CYC(b_+29, b_+31); E = 0x04;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+34, b_+35); ret_effect(gb); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); A = 0x0c;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+39); E = alu_inc8(gb, E);
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+42, b_+43); ret_effect(gb); return; }
  CYC(b_+42, b_+43);
  CALL_C(b_+43, rickyBreakTilesOnLanding_hook, SYM(rickyBreakTilesOnLanding), b_+46);
  CYC(b_+46, b_+49); SET_HL(rickyHoleCheckOffsets_bank05);
  CALL_C(b_+49, specialObjectGetRelativeTileWithDirectionTable_hook, SYM(specialObjectGetRelativeTileWithDirectionTable), b_+52);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+53, b_+55);
    goto initialize_ricky;
  }
  CYC(b_+53, b_+55);
  CALL_C(b_+55, itemDecCounter2_hook, SYM(itemDecCounter2), b_+58);
  if (F & FZ) {
    CYCT(b_+58, b_+60);
    goto initialize_ricky;
  }
  CYC(b_+58, b_+60);
  CALL_C(b_+60, rickySetJumpSpeedForCutscene_hook, SYM(rickySetJumpSpeedForCutscene), b_+63);
  CYC(b_+63, b_+65); C = 0x01;
  CYC(b_+65, b_+68); companionSetAnimation_hook(gb);
  return;

initialize_ricky:
  CYC(b_+68, b_+70); E = 0x03;
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+75); TAIL(rickyState0);
}

void rickyCheckHopUpCliff_hook(GB *gb) {
  BASE(rickyCheckHopUpCliff);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0xc0);
  CYC(b_+5, b_+7); alu_cp(gb, 0xc0);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = W8(wLinkAngle);
  CYC(b_+11, b_+13); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); SET_HL(rickyCliffOffsets_bank05);
  CALL_C(b_+17, specialObjectGetRelativeTileFromHl_hook, SYM(specialObjectGetRelativeTileFromHl), b_+20);
  CYC(b_+20, b_+22); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
    goto check_one_up_left;
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+27); alu_cp(gb, GV(0xd4, 0xdd));
  if (!(F & FZ)) {
    CYCT(b_+27, b_+29);
    goto try_two_tiles_up;
  }
  CYC(b_+27, b_+29);

check_one_up_left:
  CYC(b_+29, b_+32); SET_HL(rickyCliffOffsets_bank05 + 2);
  CALL_C(b_+32, specialObjectGetRelativeTileFromHl_hook, SYM(specialObjectGetRelativeTileFromHl), b_+35);
  CYC(b_+35, b_+37); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+37, b_+39);
    goto can_jump;
  }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); alu_cp(gb, GV(0xd4, 0xdd));
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    goto can_jump;
  }
  CYC(b_+42, b_+44);

try_two_tiles_up:
  CYC(b_+44, b_+47); SET_HL(rickyCliffOffsets_bank05 + 4);
  CALL_C(b_+47, specialObjectGetRelativeTileFromHl_hook, SYM(specialObjectGetRelativeTileFromHl), b_+50);
  CYC(b_+50, b_+52); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+52, b_+54);
    goto check_two_up_left;
  }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); A = B;
  CYC(b_+55, b_+57); alu_cp(gb, GV(0xd4, 0xdd));
  if (!(F & FZ)) { CYCT(b_+57, b_+58); ret_effect(gb); return; }
  CYC(b_+57, b_+58);

check_two_up_left:
  CYC(b_+58, b_+61); SET_HL(rickyCliffOffsets_bank05 + 6);
  CALL_C(b_+61, specialObjectGetRelativeTileFromHl_hook, SYM(specialObjectGetRelativeTileFromHl), b_+64);
  CYC(b_+64, b_+66); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+66, b_+68);
    goto can_jump;
  }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+69); A = B;
  CYC(b_+69, b_+71); alu_cp(gb, GV(0xd4, 0xdd));
  if (!(F & FZ)) { CYCT(b_+71, b_+72); ret_effect(gb); return; }
  CYC(b_+71, b_+72);

can_jump:
  CYC(b_+72, b_+74); E = 0x04;
  CYC(b_+74, b_+76); A = 0x02;
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CYC(b_+77, b_+78); E = alu_inc8(gb, E);
  CYC(b_+78, b_+79); alu_xor(gb, A);
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+82); E = 0x07;
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CYC(b_+83, b_+84); ret_effect(gb);
}

void rickyBreakTilesOnLanding_hook(GB *gb) {
  BASE(rickyBreakTilesOnLanding);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(b_+28);
next:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+8); alu_or(gb, B);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CYC(b_+10, b_+13); A = W8(w1Companion_yh);
  CYC(b_+13, b_+14); alu_add(gb, B);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = W8(w1Companion_xh);
  CYC(b_+18, b_+19); alu_add(gb, C);
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+22); A = 0x10;
  CALL_C(b_+22, tryToBreakTile_hook, SYM(tryToBreakTile), b_+25);
  CYC(b_+25, b_+26); SET_HL(pop_effect(gb));
  CYC(b_+26, b_+28);
  goto next;
}

void rickyBeginJumpOverHole_hook(GB *gb) {
  BASE(rickyBeginJumpOverHole);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); W8(wLinkInAir) = A;
  TAIL(rickySetJumpSpeed_andcc91);
}

void rickySetJumpSpeed_andcc91_hook(GB *gb) {
  BASE(rickySetJumpSpeed_andcc91);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); W8(wDisableScreenTransitions) = A;
  TAIL(rickySetJumpSpeed);
}

void rickySetJumpSpeed_hook(GB *gb) {
  BASE(rickySetJumpSpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xfd00);
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+8); L = 0x06;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x08);
  CYC(b_+10, b_+12); L = 0x10;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x32);
  CYC(b_+14, b_+16); C = 0x0f;
  CALL_C(b_+16, companionSetAnimation_hook, SYM(companionSetAnimation), b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+21); ret_effect(gb);
}

void rickyCheckAtScreenEdge_hook(GB *gb) {
  BASE(rickyCheckAtScreenEdge);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0b;
  CYC(b_+3, b_+5); A = 0x06;
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
    goto outsideScreen;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); A = W8(wScreenTransitionBoundaryY);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+13, b_+15);
    goto outsideScreen;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); L = 0x0d;
  CYC(b_+17, b_+19); A = 0x06;
  CYC(b_+19, b_+20); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+20, b_+22);
    goto outsideScreen;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); A = W8(wScreenTransitionBoundaryX);
  CYC(b_+25, b_+26); A = alu_dec8(gb, A);
  CYC(b_+26, b_+27); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+27, b_+29);
    goto outsideScreen;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); alu_xor(gb, A);
  CYC(b_+30, b_+31); ret_effect(gb); return;
outsideScreen:
  CYC(b_+31, b_+32); alu_or(gb, D);
  CYC(b_+32, b_+33); ret_effect(gb);
}
