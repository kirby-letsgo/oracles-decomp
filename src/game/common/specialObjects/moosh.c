#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void specialObjectCode_moosh__runState_hook(GB *gb);
void mooshState0_hook(GB *gb);
void mooshState1_hook(GB *gb);
void mooshCheckHazards_hook(GB *gb);
void mooshState2_hook(GB *gb);
void mooshState3_hook(GB *gb);
void mooshState4_hook(GB *gb);
void mooshTryToBreakTileFromMovingAndCheckHazards_hook(GB *gb);
void mooshSetVar37ForHazard_hook(GB *gb);
void mooshState5_hook(GB *gb);
void mooshLandOnGroundAndGotoState5_hook(GB *gb);
void mooshPressedAButton_hook(GB *gb);
void mooshState8_hook(GB *gb);
void mooshState8Substate0_hook(GB *gb);
void mooshState8Substate1_hook(GB *gb);
void mooshState8Substate2_hook(GB *gb);
void mooshState8Substate3_hook(GB *gb);
void mooshState8Substate4_hook(GB *gb);
void mooshState8Substate5_hook(GB *gb);
void mooshState6_hook(GB *gb);
void mooshState7_hook(GB *gb);
void mooshStateC_hook(GB *gb);
void mooshStateA_hook(GB *gb);
void mooshStateASubstate1_hook(GB *gb);
void mooshStateASubstate3_hook(GB *gb);
void mooshStateASubstate4_hook(GB *gb);
void mooshStateASubstate5_hook(GB *gb);
void mooshStateASubstate6_hook(GB *gb);
void mooshUpdateAsNpc_hook(GB *gb);
void mooshIncVar03_hook(GB *gb);

static uint16_t moosh_jump_table(GB *gb) {
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

void specialObjectCode_moosh_hook(GB *gb) {
  BASE(specialObjectCode_moosh);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionRetIfInactive_hook, SYM(companionRetIfInactive), b_+3);
  CALL_C(b_+3, companionFunc_47d8_hook, SYM(companionFunc_47d8), b_+6);
  CALL_C(b_+6, specialObjectCode_moosh__runState_hook, b_+12, b_+9);
  CYC(b_+9, b_+12); companionCheckEnableTerrainEffects_hook(gb);
}

void specialObjectCode_moosh__runState_hook(GB *gb) {
  BASE(specialObjectCode_moosh);
  uint16_t sp0_ = gb->sp;
  CYC(b_+12, b_+14); E = 0x04;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (moosh_jump_table(gb));
    if (jt_ == SYM(mooshState0) && hook_enabled_at(gb, SYM(mooshState0))) { mooshState0_hook(gb); return; }
    else if (jt_ == SYM(mooshState1) && hook_enabled_at(gb, SYM(mooshState1))) { mooshState1_hook(gb); return; }
    else if (jt_ == SYM(mooshState2) && hook_enabled_at(gb, SYM(mooshState2))) { mooshState2_hook(gb); return; }
    else if (jt_ == SYM(mooshState3) && hook_enabled_at(gb, SYM(mooshState3))) { mooshState3_hook(gb); return; }
    else if (jt_ == SYM(mooshState4) && hook_enabled_at(gb, SYM(mooshState4))) { mooshState4_hook(gb); return; }
    else if (jt_ == SYM(mooshState5) && hook_enabled_at(gb, SYM(mooshState5))) { mooshState5_hook(gb); return; }
    else if (jt_ == SYM(mooshState6) && hook_enabled_at(gb, SYM(mooshState6))) { mooshState6_hook(gb); return; }
    else if (jt_ == SYM(mooshState7) && hook_enabled_at(gb, SYM(mooshState7))) { mooshState7_hook(gb); return; }
    else if (jt_ == SYM(mooshState8) && hook_enabled_at(gb, SYM(mooshState8))) { mooshState8_hook(gb); return; }
    else if (jt_ == SYM(mooshStateA) && hook_enabled_at(gb, SYM(mooshStateA))) { mooshStateA_hook(gb); return; }
    else if (jt_ == SYM(mooshStateC) && hook_enabled_at(gb, SYM(mooshStateC))) { mooshStateC_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void mooshState0_hook(GB *gb) {
  BASE(mooshState0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckCanSpawn_hook, SYM(companionCheckCanSpawn), b_+3);
  CYC(b_+3, b_+5); A = 0x06;
  CALL_C(b_+5, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+8);
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+12); L = 0x08;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+17); SET_HL(wMooshState);
  CYC(b_+17, b_+19); A = 0x80;
  CYC(b_+19, b_+20); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto set_animation;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); A = W8(wActiveRoom);
  CYC(b_+25, b_+27); alu_cp(gb, 0x54);
  if (F & FZ) {
    CYCT(b_+27, b_+29);
    goto cutscene_state;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x20;
  CYC(b_+31, b_+32); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+32, b_+34);
    goto cutscene_state;
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = 0x40;
  CYC(b_+36, b_+37); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+37, b_+39);
    goto cutscene_state;
  }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0x52;
  CALL_C(b_+41, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+44);
  if (!(F & FC)) {
    CYCT(b_+44, b_+46);
    goto set_animation;
  }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+49); A = W8(wActiveRoom);
  CYC(b_+49, b_+51); alu_cp(gb, 0x6b);
  if (!(F & FZ)) {
    CYCT(b_+51, b_+53);
    goto set_animation;
  }
  CYC(b_+51, b_+53);

cutscene_state:
  CYC(b_+53, b_+55); E = 0x04;
  CYC(b_+55, b_+57); A = 0x0a;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+61); mooshStateA_hook(gb);
  return;

set_animation:
  CYC(b_+61, b_+63); C = 0x01;
  CALL_C(b_+63, companionSetAnimation_hook, SYM(companionSetAnimation), b_+66);
  CYC(b_+66, b_+69); objectSetVisiblec1_hook(gb);
}

void mooshState1_hook(GB *gb) {
  BASE(mooshState1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionSetPriorityRelativeToLink_hook, SYM(companionSetPriorityRelativeToLink), b_+3);
  CALL_C(b_+3, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+6);
  CYC(b_+6, b_+8); C = 0x09;
  CALL_C(b_+8, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+11);
  if (F & FC) {
    CYCT(b_+11, b_+14);
    TAIL(companionTryToMount);
  }
  CYC(b_+11, b_+14);
  TAIL(mooshCheckHazards);
}

void mooshCheckHazards_hook(GB *gb) {
  BASE(mooshCheckHazards);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckHazards_hook, SYM(companionCheckHazards), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mooshSetVar37ForHazard_hook(gb);
}

void mooshState3_hook(GB *gb) {
  BASE(mooshState3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckMountingComplete_hook, SYM(companionCheckMountingComplete), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, companionFinalizeMounting_hook, SYM(companionFinalizeMounting), b_+7);
  CYC(b_+7, b_+9); C = 0x13;
  CYC(b_+9, b_+12); companionSetAnimation_hook(gb);
}

void mooshState4_hook(GB *gb) {
  BASE(mooshState4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x24;
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+5, b_+7); L = 0x37;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto update_counter;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x0e;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CALL_C(b_+15, companionDragToCenterOfHole_hook, SYM(companionDragToCenterOfHole), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);

update_counter:
  CALL_C(b_+19, companionDecCounter1_hook, SYM(companionDecCounter1), b_+22);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+24);
    goto animate;
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+27); L = 0x37;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CALL_C(b_+28, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+31);
  CYC(b_+31, b_+33); E = 0x37;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+36); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(b_+36, b_+38);
    goto animate;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); A = 0x65;
  CYC(b_+40, b_+43); playSound_b00_hook(gb);
  return;

animate:
  CALL_C(b_+43, companionAnimateDrowningOrFallingThenRespawn_hook, SYM(companionAnimateDrowningOrFallingThenRespawn), b_+46);
  if (!(F & FC)) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); C = 0x13;
  CYC(b_+49, b_+52); A = W8(wLinkObjectIndex);
  CYC(b_+52, b_+53); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+53, b_+55);
  } else {
    CYC(b_+53, b_+55);
    CYC(b_+55, b_+57); C = 0x01;
  }
  CYC(b_+57, b_+60); companionUpdateDirectionAndSetAnimation_hook(gb);
}

void mooshTryToBreakTileFromMovingAndCheckHazards_hook(GB *gb) {
  BASE(mooshTryToBreakTileFromMovingAndCheckHazards);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionTryToBreakTileFromMoving_hook, SYM(companionTryToBreakTileFromMoving), b_+3);
  CALL_C(b_+3, companionCheckHazards_hook, SYM(companionCheckHazards), b_+6);
  CYC(b_+6, b_+8); C = 0x13;
  if (!(F & FC)) {
    CYCT(b_+8, b_+11);
    TAIL(companionUpdateDirectionAndAnimate);
  }
  CYC(b_+8, b_+11);
  TAIL(mooshSetVar37ForHazard);
}

void mooshSetVar37ForHazard_hook(GB *gb) {
  BASE(mooshSetVar37ForHazard);
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  CYC(b_+1, b_+3); C = 0x0d;
  if (F & FZ) {
    CYCT(b_+3, b_+5);
  } else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+7); C = 0x0e;
  }
  CYC(b_+7, b_+9); E = 0x37;
  CYC(b_+9, b_+10); A = C;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = 0x06;
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void mooshState5_hook(GB *gb) {
  BASE(mooshState5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, companionCheckHazards_hook, SYM(companionCheckHazards), b_+9);
  if (F & FC) {
    CYCT(b_+9, b_+11);
    TAIL(mooshSetVar37ForHazard);
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); A = W8(wForceCompanionDismount);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto check_dismount;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); A = W8(wGameKeysJustPressed);
  CYC(b_+20, b_+22); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+24);
    TAIL(mooshPressedAButton);
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_bit(gb, 1, A);

check_dismount:
  if (!(F & FZ)) {
    CYCT(b_+26, b_+29);
    TAIL(companionGotoDismountState);
  }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+32); A = W8(wLinkAngle);
  CYC(b_+32, b_+34); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+34, b_+35); ret_effect(gb); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+38); SET_HL(w1Companion_angle);
  CYC(b_+38, b_+39); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); C = 0x13;
  if (!(F & FZ)) {
    CYCT(b_+42, b_+45);
    TAIL(companionUpdateDirectionAndAnimate);
  }
  CYC(b_+42, b_+45);
  CALL_C(b_+45, companionCheckHopDownCliff_hook, SYM(companionCheckHopDownCliff), b_+48);
  if (F & FZ) { CYCT(b_+48, b_+49); ret_effect(gb); return; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); E = 0x10;
  CYC(b_+51, b_+53); A = 0x28;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CALL_C(b_+54, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+57);
  CYC(b_+57, b_+59); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
}

void mooshLandOnGroundAndGotoState5_hook(GB *gb) {
  BASE(mooshLandOnGroundAndGotoState5);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wLinkInAir) = A;
  CYC(b_+4, b_+6); C = 0x13;
  CYC(b_+6, b_+9); companionSetAnimationAndGotoState5_hook(gb);
}

void mooshPressedAButton_hook(GB *gb) {
  BASE(mooshPressedAButton);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+4); E = 0x04;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); A = 0x53;
  CALL_C(b_+10, playSound_b00_hook, SYM(playSound_b00), SYM(mooshState2));
  TAIL(mooshState2);
}

void mooshState2_hook(GB *gb) {
  BASE(mooshState2);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void mooshState8_hook(GB *gb) {
  BASE(mooshState8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moosh_jump_table(gb));
    if (jt_ == SYM(mooshState8Substate0) && hook_enabled_at(gb, SYM(mooshState8Substate0))) { mooshState8Substate0_hook(gb); return; }
    else if (jt_ == SYM(mooshState8Substate1) && hook_enabled_at(gb, SYM(mooshState8Substate1))) { mooshState8Substate1_hook(gb); return; }
    else if (jt_ == SYM(mooshState8Substate2) && hook_enabled_at(gb, SYM(mooshState8Substate2))) { mooshState8Substate2_hook(gb); return; }
    else if (jt_ == SYM(mooshState8Substate3) && hook_enabled_at(gb, SYM(mooshState8Substate3))) { mooshState8Substate3_hook(gb); return; }
    else if (jt_ == SYM(mooshState8Substate4) && hook_enabled_at(gb, SYM(mooshState8Substate4))) { mooshState8Substate4_hook(gb); return; }
    else if (jt_ == SYM(mooshState8Substate5) && hook_enabled_at(gb, SYM(mooshState8Substate5))) { mooshState8Substate5_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void mooshState8Substate0_hook(GB *gb) {
  BASE(mooshState8Substate0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); SET_BC(0xfec0);
  CALL_C(b_+6, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+9);
  CYC(b_+9, b_+11); L = 0x10;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x28);
  CYC(b_+13, b_+15); L = 0x39;
  CYC(b_+15, b_+17); A = 0x04;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+23); C = 0x09;
  CYC(b_+23, b_+26); companionSetAnimation_hook(gb);
}

void mooshState8Substate1_hook(GB *gb) {
  BASE(mooshState8Substate1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCheckIsOverHazard_hook, SYM(objectCheckIsOverHazard), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto not_over_water;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x0000);
  CALL_C(b_+10, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+13);
  CYC(b_+13, b_+15); L = 0x05;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x05);
  CYC(b_+17, b_+19); B = 0x9f;
  CALL_C(b_+19, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+22);
  CYC(b_+22, b_+23); L = alu_dec8(gb, L);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_sub(gb, 0x20);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = 0x46;
  CYC(b_+29, b_+31); E = 0x06;
  CYC(b_+31, b_+33); A = 0x3c;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); ret_effect(gb);
  return;

not_over_water:
  CYC(b_+36, b_+39); A = W8(wLinkAngle);
  CYC(b_+39, b_+41); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+41, b_+43);
    goto check_vertical_speed;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); SET_HL(w1Companion_angle);
  CYC(b_+46, b_+47); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CALL_C(b_+48, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+51);

check_vertical_speed:
  CYC(b_+51, b_+53); E = 0x15;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+55, b_+57);
    goto moving_up;
  }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); E = 0x3b;
  CYC(b_+59, b_+62); A = W8(wGameKeysPressed);
  CYC(b_+62, b_+64); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+64, b_+66);
    goto store_hold_counter;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); A = alu_inc8(gb, A);

store_hold_counter:
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+71); alu_cp(gb, 0x0a);
  if (!(F & FC)) {
    CYCT(b_+71, b_+73);
    goto goto_substate2;
  }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+76); A = W8(wGameKeysJustPressed);
  CYC(b_+76, b_+78); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(b_+78, b_+80);
    goto flutter_done;
  }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); E = 0x3a;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+85); alu_cp(gb, 0x10);
  if (F & FZ) {
    CYCT(b_+85, b_+87);
    goto flutter_done;
  }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); A = alu_inc8(gb, A);
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+90); E = alu_dec8(gb, E);
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+93); alu_add(gb, 0x08);
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+96); E = 0x20;
  CYC(b_+96, b_+98); A = 0x01;
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CALL_C(b_+99, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+102);
  CYC(b_+102, b_+104); A = 0x53;
  CALL_C(b_+104, playSound_b00_hook, SYM(playSound_b00), b_+107);

flutter_done:
  CYC(b_+107, b_+109); E = 0x39;
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+111); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+111, b_+113);
    goto update_movement;
  }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); A = alu_dec8(gb, A);
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);
  CYC(b_+115, b_+117); E = 0x20;
  CYC(b_+117, b_+119); A = 0x0f;
  CYC(b_+119, b_+120); mem_wr(gb, DE, A);
  CYC(b_+120, b_+122); C = 0x09;
  CYC(b_+122, b_+125); companionUpdateDirectionAndAnimate_hook(gb);
  return;

moving_up:
  CYC(b_+125, b_+127); C = 0x09;
  CALL_C(b_+127, companionUpdateDirectionAndAnimate_hook, SYM(companionUpdateDirectionAndAnimate), b_+130);

update_movement:
  CYC(b_+130, b_+132); C = 0x10;
  CALL_C(b_+132, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+135);
  if (!(F & FZ)) { CYCT(b_+135, b_+136); ret_effect(gb); return; }
  CYC(b_+135, b_+136);
  CALL_C(b_+136, companionTryToBreakTileFromMoving_hook, SYM(companionTryToBreakTileFromMoving), b_+139);
  CALL_C(b_+139, mooshLandOnGroundAndGotoState5_hook, SYM(mooshLandOnGroundAndGotoState5), b_+142);
  CYC(b_+142, b_+145); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
  return;

goto_substate2:
  CYC(b_+145, b_+148); itemIncSubstate_hook(gb);
}

void mooshState8Substate2_hook(GB *gb) {
  BASE(mooshState8Substate2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+6); A = W8(wGameKeysPressed);
  CYC(b_+6, b_+8); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    goto next_substate;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = 0x3b;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_cp(gb, 0x28);
  if (F & FC) {
    CYCT(b_+15, b_+17);
    goto increment_charge;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); C = 0x02;
  CALL_C(b_+19, companionFlashFromChargingAnimation_hook, SYM(companionFlashFromChargingAnimation), b_+22);

increment_charge:
  CYC(b_+22, b_+24); E = 0x3b;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); alu_cp(gb, 0x28);
  if (F & FC) { CYCT(b_+29, b_+30); ret_effect(gb); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); A = 0x4f;
  if (F & FZ) {
    CYCT(b_+32, b_+35);
    TAIL(playSound_b00);
  }
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+38); SET_HL(w1Link_collisionType);
  CYC(b_+38, b_+40); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CYC(b_+40, b_+41); H = alu_inc8(gb, H);
  CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CYC(b_+43, b_+45); E = 0x3b;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+48); alu_cp(gb, 0x78);
  if (!(F & FZ)) { CYCT(b_+48, b_+49); ret_effect(gb); return; }
  CYC(b_+48, b_+49);

next_substate:
  CYC(b_+49, b_+52); SET_HL(w1Link_oamFlagsBackup);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  CALL_C(b_+54, itemIncSubstate_hook, SYM(itemIncSubstate), b_+57);
  CYC(b_+57, b_+59); C = 0x17;
  CYC(b_+59, b_+61); E = 0x3b;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+64); alu_cp(gb, 0x28);
  if (F & FC) { CYCT(b_+64, b_+65); ret_effect(gb); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+68); companionSetAnimation_hook(gb);
}

void mooshState8Substate3_hook(GB *gb) {
  BASE(mooshState8Substate3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x80;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x3b;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0x28);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
    goto charged;
  }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, mooshLandOnGroundAndGotoState5_hook, SYM(mooshLandOnGroundAndGotoState5), b_+16);
  CYC(b_+16, b_+19); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
  return;

charged:
  CALL_C(b_+19, companionCheckHazards_hook, SYM(companionCheckHazards), b_+22);
  if (F & FC) {
    CYCT(b_+22, b_+25);
    TAIL(mooshSetVar37ForHazard);
  }
  CYC(b_+22, b_+25);
  CALL_C(b_+25, itemIncSubstate_hook, SYM(itemIncSubstate), b_+28);
  CYC(b_+28, b_+30); A = 0x0f;
  CYC(b_+30, b_+33); W8(wScreenShakeCounterY) = A;
  CYC(b_+33, b_+35); A = 0xf1;
  CALL_C(b_+35, playSound_b00_hook, SYM(playSound_b00), b_+38);
  CYC(b_+38, b_+40); A = 0x85;
  CALL_C(b_+40, playSound_b00_hook, SYM(playSound_b00), b_+43);
  CYC(b_+43, b_+45); A = 0x05;
  CYC(b_+45, b_+48); SET_HL(wCompanionTutorialTextShown);
  CALL_C(b_+48, setFlag_hook, SYM(setFlag), b_+51);
  CYC(b_+51, b_+54); SET_BC(0x2800);
  CYC(b_+54, b_+57); companionCreateWeaponItem_hook(gb);
}

void mooshState8Substate4_hook(GB *gb) {
  BASE(mooshState8Substate4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x21;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(w1Link_collisionType);
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+13, b_+14); H = alu_inc8(gb, H);
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+16, b_+19); mooshLandOnGroundAndGotoState5_hook(gb);
}

void mooshState8Substate5_hook(GB *gb) {
  BASE(mooshState8Substate5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDecCounter1IfNonzero_hook, SYM(companionDecCounter1IfNonzero), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto descend;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); specialObjectAnimate_hook(gb);
  return;

descend:
  CYC(b_+8, b_+10); C = 0x10;
  CALL_C(b_+10, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CALL_C(b_+14, mooshLandOnGroundAndGotoState5_hook, SYM(mooshLandOnGroundAndGotoState5), b_+17);
  CYC(b_+17, b_+20); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
}

void mooshState6_hook(GB *gb) {
  BASE(mooshState6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moosh_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+21) { goto substate1; }
    else if (jt_ == b_+29) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, companionDismountAndSavePosition_hook, SYM(companionDismountAndSavePosition), b_+16);
  CYC(b_+16, b_+18); C = 0x01;
  CYC(b_+18, b_+21); companionSetAnimation_hook(gb);
  return;

substate1:
  CYC(b_+21, b_+24); A = W8(wLinkInAir);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+25, b_+26); ret_effect(gb); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); itemIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+29, b_+31); C = 0x09;
  CALL_C(b_+31, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+34);
  if (F & FC) {
    CYCT(b_+34, b_+37);
    TAIL(mooshCheckHazards);
  }
  CYC(b_+34, b_+37);
  CYC(b_+37, b_+39); E = 0x05;
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); E = alu_dec8(gb, E);
  CYC(b_+42, b_+44); A = 0x01;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); ret_effect(gb);
}

void mooshState7_hook(GB *gb) {
  BASE(mooshState7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDecCounter1ToJumpDownCliff_hook, SYM(companionDecCounter1ToJumpDownCliff), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+5);
    goto check_walls;
  }
  CYC(b_+3, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); C = 0x09;
  CYC(b_+8, b_+11); companionSetAnimation_hook(gb);
  return;

check_walls:
  CALL_C(b_+11, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+14);
  CALL_C(b_+14, specialObjectCheckMovingAwayFromWall_hook, SYM(specialObjectCheckMovingAwayFromWall), b_+17);
  CYC(b_+17, b_+19); E = 0x07;
  if (F & FZ) {
    CYCT(b_+19, b_+21);
    goto stopped;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); ret_effect(gb);
  return;

stopped:
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+25, b_+26); ret_effect(gb); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); mooshLandOnGroundAndGotoState5_hook(gb);
}

void mooshStateC_hook(GB *gb) {
  BASE(mooshStateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moosh_jump_table(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+23) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+8, companionInitializeOnEnteringScreen_hook, SYM(companionInitializeOnEnteringScreen), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x3c);
  CYC(b_+13, b_+15); A = 0xc5;
  CALL_C(b_+15, playSound_b00_hook, SYM(playSound_b00), b_+18);
  CYC(b_+18, b_+20); C = 0x0f;
  CYC(b_+20, b_+23); companionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(b_+23, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+26);
  CYC(b_+26, b_+28); E = 0x10;
  CYC(b_+28, b_+30); A = 0x1e;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+34);
  CYC(b_+34, b_+37); SET_HL(b_+47);
  CALL_C(b_+37, companionRetIfNotFinishedWalkingIn_hook, SYM(companionRetIfNotFinishedWalkingIn), b_+40);
  CYC(b_+40, b_+42); E = 0x03;
  CYC(b_+42, b_+43); alu_xor(gb, A);
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+47); mooshState0_hook(gb);
}

void mooshStateA_hook(GB *gb) {
  BASE(mooshStateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moosh_jump_table(gb));
    if (jt_ == b_+18) { goto substate0; }
    else if (jt_ == SYM(mooshStateASubstate1) && hook_enabled_at(gb, SYM(mooshStateASubstate1))) { mooshStateASubstate1_hook(gb); return; }
    else if (jt_ == b_+34) { goto substate2; }
    else if (jt_ == SYM(mooshStateASubstate3) && hook_enabled_at(gb, SYM(mooshStateASubstate3))) { mooshStateASubstate3_hook(gb); return; }
    else if (jt_ == SYM(mooshStateASubstate4) && hook_enabled_at(gb, SYM(mooshStateASubstate4))) { mooshStateASubstate4_hook(gb); return; }
    else if (jt_ == SYM(mooshStateASubstate5) && hook_enabled_at(gb, SYM(mooshStateASubstate5))) { mooshStateASubstate5_hook(gb); return; }
    else if (jt_ == SYM(mooshStateASubstate6) && hook_enabled_at(gb, SYM(mooshStateASubstate6))) { mooshStateASubstate6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+24); SET_HL(wMooshState);
  CYC(b_+24, b_+26); A = 0x20;
  CYC(b_+26, b_+27); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+27, b_+29);
    goto select_animation;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x40;
  CYC(b_+31, b_+32); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+32, b_+34);
    goto disable_controls;
  }
  CYC(b_+32, b_+34);

substate2:
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+39); E = 0x3d;
  CALL_C(b_+39, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+42);

select_animation:
  CYC(b_+42, b_+44); A = 0x24;
  CALL_C(b_+44, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+47);
  CYC(b_+47, b_+49); A = 0x00;
  if (F & FZ) {
    CYCT(b_+49, b_+51);
    goto store_animation;
  }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); A = 0x03;

store_animation:
  CYC(b_+53, b_+55); E = 0x3f;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CALL_C(b_+56, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+59);
  CYC(b_+59, b_+62); objectSetVisiblec3_hook(gb);
  return;

disable_controls:
  CYC(b_+62, b_+64); A = 0x01;
  CYC(b_+64, b_+67); W8(wMenuDisabled) = A;
  CYC(b_+67, b_+70); W8(wDisabledObjects) = A;
  CYC(b_+70, b_+72); A = 0x04;
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+75); A = 0x01;
  CALL_C(b_+75, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+78);
  CYC(b_+78, b_+81); objectSetVisiblec3_hook(gb);
}

void mooshStateASubstate1_hook(GB *gb) {
  BASE(mooshStateASubstate1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3d;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto update;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+11); W8(wDisabledObjects) = A;
  CYC(b_+11, b_+14); W8(wMenuDisabled) = A;

update:
  CALL_C(b_+14, companionSetAnimationToVar3f_hook, SYM(companionSetAnimationToVar3f), b_+17);
  CALL_C(b_+17, mooshUpdateAsNpc_hook, SYM(mooshUpdateAsNpc), b_+20);
  CYC(b_+20, b_+23); A = W8(wMooshState);
  CYC(b_+23, b_+25); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+25, b_+26); ret_effect(gb); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28);

  CYC((SYM(mooshStateASubstate3) + 17), (SYM(mooshStateASubstate3) + 19)); E = 0x3d;
  CYC((SYM(mooshStateASubstate3) + 19), (SYM(mooshStateASubstate3) + 20)); alu_xor(gb, A);
  CYC((SYM(mooshStateASubstate3) + 20), (SYM(mooshStateASubstate3) + 21)); mem_wr(gb, DE, A);
  CALL_C((SYM(mooshStateASubstate3) + 21), objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), (SYM(mooshStateASubstate3) + 24));
  CYC((SYM(mooshStateASubstate3) + 24), (SYM(mooshStateASubstate3) + 26)); C = 0x01;
  CALL_C((SYM(mooshStateASubstate3) + 26), companionSetAnimation_hook, SYM(companionSetAnimation), (SYM(mooshStateASubstate3) + 29));
  CYC((SYM(mooshStateASubstate3) + 29), (SYM(mooshStateASubstate3) + 32)); companionForceMount_hook(gb);
}

void mooshStateASubstate3_hook(GB *gb) {
  BASE(mooshStateASubstate3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionSetAnimationToVar3f_hook, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, mooshUpdateAsNpc_hook, SYM(mooshUpdateAsNpc), b_+6);
  CYC(b_+6, b_+9); A = W8(wMooshState);
  CYC(b_+9, b_+11); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0xff;
  CYC(b_+14, b_+17); W8(wStatusBarNeedsRefresh) = A;
  CYC(b_+17, b_+19); E = 0x3d;
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CALL_C(b_+21, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+24);
  CYC(b_+24, b_+26); C = 0x01;
  CALL_C(b_+26, companionSetAnimation_hook, SYM(companionSetAnimation), b_+29);
  CYC(b_+29, b_+32); companionForceMount_hook(gb);
}

void mooshStateASubstate4_hook(GB *gb) {
  BASE(mooshStateASubstate4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, mooshIncVar03_hook, SYM(mooshIncVar03), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x2208);
  CYC(b_+6, b_+9); showText_hook(gb);
}

void mooshStateASubstate5_hook(GB *gb) {
  BASE(mooshStateASubstate5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); SET_BC(0xfec0);
  CALL_C(b_+6, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+9);
  CYC(b_+9, b_+11); L = 0x09;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10);
  CYC(b_+13, b_+15); L = 0x10;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x28);
  CYC(b_+17, b_+19); A = 0x0b;
  CALL_C(b_+19, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+22);
  CYC(b_+22, b_+25); mooshIncVar03_hook(gb);
}

void mooshStateASubstate6_hook(GB *gb) {
  BASE(mooshStateASubstate6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x15;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  CYC(b_+7, b_+9); C = 0x10;
  if (!(F & FZ)) {
    CYCT(b_+9, b_+12);
    TAIL(objectUpdateSpeedZ_paramC);
  }
  CYC(b_+9, b_+12);
  CALL_C(b_+12, objectApplySpeed_hook, SYM(objectApplySpeed), b_+15);
  CYC(b_+15, b_+17); E = 0x0b;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(b_+20, b_+21); ret_effect(gb); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+25); W8(wDisabledObjects) = A;
  CYC(b_+25, b_+28); W8(wMenuDisabled) = A;
  CYC(b_+28, b_+31); W8(wRememberedCompanionId) = A;
  CYC(b_+31, b_+34); SET_HL(wMooshState);
  CYC(b_+34, b_+36); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(b_+36, b_+39); itemDelete_hook(gb);
}

void mooshUpdateAsNpc_hook(GB *gb) {
  BASE(mooshUpdateAsNpc);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionPreventLinkFromPassing_noExtraChecks_hook, SYM(companionPreventLinkFromPassing_noExtraChecks), b_+3);
  CALL_C(b_+3, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+6);
  CYC(b_+6, b_+9); companionSetPriorityRelativeToLink_hook(gb);
}

void mooshIncVar03_hook(GB *gb) {
  BASE(mooshIncVar03);
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}
