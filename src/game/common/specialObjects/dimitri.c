#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void specialObjectCode_dimitri__runState_hook(GB *gb);
void dimitriState0_hook(GB *gb);
void dimitriState1_hook(GB *gb);
void dimitriState2_hook(GB *gb);
void dimitriState2Substate0_hook(GB *gb);
void dimitriState2Substate1_hook(GB *gb);
void dimitriState2Substate2_hook(GB *gb);
void dimitriState2Substate3_hook(GB *gb);
void dimitriFunc_756d_hook(GB *gb);
void dimitriState3_hook(GB *gb);
void dimitriState4_hook(GB *gb);
void dimitriState5_hook(GB *gb);
void dimitriUpdateMovement_hook(GB *gb);
void dimitriState9_hook(GB *gb);
void dimitriGotoEatingState_hook(GB *gb);
void dimitriState6_hook(GB *gb);
void dimitriGotoState1IfLinkFarAway_hook(GB *gb);
void dimitriGotoState1_hook(GB *gb);
void dimitriState7_hook(GB *gb);
void dimitriState8_hook(GB *gb);
void dimitriStateB_hook(GB *gb);
void dimitriStateC_hook(GB *gb);
void dimitriStateD_hook(GB *gb);
void dimitriLandOnGroundAndGotoState5_hook(GB *gb);
void dimitriStateA_hook(GB *gb);
void dimitriStateASubstate0_hook(GB *gb);
void dimitriStateASubstate1_hook(GB *gb);
void dimitriStateASubstate3_hook(GB *gb);
void dimitriStateASubstate4_hook(GB *gb);
void dimitriStateASubstate2_hook(GB *gb);
void dimitriCheckAddToGrabbableObjectBuffer_hook(GB *gb);
void dimitriCheckCanBeHeldInDirection_hook(GB *gb);
void dimitriAddWaterfallResistance_hook(GB *gb);

static uint16_t dimitri_jump_table(GB *gb) {
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

static void dimitri_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void specialObjectCode_dimitri_hook(GB *gb) {
  BASE(specialObjectCode_dimitri);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionRetIfInactive_hook, SYM(companionRetIfInactive), b_+3);
  CALL_C(b_+3, companionFunc_47d8_hook, SYM(companionFunc_47d8), b_+6);
  CALL_C(b_+6, specialObjectCode_dimitri__runState_hook, b_+16, b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+13); W8(wDimitriHitNpc) = A;
  CYC(b_+13, b_+16); companionCheckEnableTerrainEffects_hook(gb);
}

void specialObjectCode_dimitri__runState_hook(GB *gb) {
  BASE(specialObjectCode_dimitri);
  uint16_t sp0_ = gb->sp;
  CYC(b_+16, b_+18); E = 0x04;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (dimitri_jump_table(gb));
    if (jt_ == SYM(dimitriState0) && hook_enabled_at(gb, SYM(dimitriState0))) { dimitriState0_hook(gb); return; }
    else if (jt_ == SYM(dimitriState1) && hook_enabled_at(gb, SYM(dimitriState1))) { dimitriState1_hook(gb); return; }
    else if (jt_ == SYM(dimitriState2) && hook_enabled_at(gb, SYM(dimitriState2))) { dimitriState2_hook(gb); return; }
    else if (jt_ == SYM(dimitriState3) && hook_enabled_at(gb, SYM(dimitriState3))) { dimitriState3_hook(gb); return; }
    else if (jt_ == SYM(dimitriState4) && hook_enabled_at(gb, SYM(dimitriState4))) { dimitriState4_hook(gb); return; }
    else if (jt_ == SYM(dimitriState5) && hook_enabled_at(gb, SYM(dimitriState5))) { dimitriState5_hook(gb); return; }
    else if (jt_ == SYM(dimitriState6) && hook_enabled_at(gb, SYM(dimitriState6))) { dimitriState6_hook(gb); return; }
    else if (jt_ == SYM(dimitriState7) && hook_enabled_at(gb, SYM(dimitriState7))) { dimitriState7_hook(gb); return; }
    else if (jt_ == SYM(dimitriState8) && hook_enabled_at(gb, SYM(dimitriState8))) { dimitriState8_hook(gb); return; }
    else if (jt_ == SYM(dimitriState9) && hook_enabled_at(gb, SYM(dimitriState9))) { dimitriState9_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateA) && hook_enabled_at(gb, SYM(dimitriStateA))) { dimitriStateA_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateB) && hook_enabled_at(gb, SYM(dimitriStateB))) { dimitriStateB_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateC) && hook_enabled_at(gb, SYM(dimitriStateC))) { dimitriStateC_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateD) && hook_enabled_at(gb, SYM(dimitriStateD))) { dimitriStateD_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void dimitriState0_hook(GB *gb) {
  BASE(dimitriState0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckCanSpawn_hook, SYM(companionCheckCanSpawn), b_+3);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+7); L = 0x08;
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+12); A = W8(wDimitriState);
  CYC(b_+12, b_+14); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
    goto set_animation;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20);
    goto initialize_cutscene;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+24);
    goto set_animation;
  }
  CYC(b_+22, b_+24);

initialize_cutscene:
  CYC(b_+24, b_+26); A = 0x24;
  CALL_C(b_+26, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+29);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); C = 0x24;
  if (F & FZ) {
    CYCT(b_+32, b_+34);
  } else {
    CYC(b_+32, b_+34);
    CYC(b_+34, b_+36); C = 0x1e;
  }
  CYC(b_+36, b_+38); L = 0x04;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x0a);
  CYC(b_+40, b_+42); E = 0x3d;
  CALL_C(b_+42, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+45);
  CYC(b_+45, b_+46); A = C;
  CYC(b_+46, b_+48); E = 0x3f;
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CALL_C(b_+49, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+52);
  CYC(b_+52, b_+55); SET_BC(0x0408);
  CALL_C(b_+55, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+58);
  CYC(b_+58, b_+60);
  goto set_visible;

set_animation:
  CYC(b_+60, b_+62); C = 0x1c;
  CALL_C(b_+62, companionSetAnimation_hook, SYM(companionSetAnimation), b_+65);

set_visible:
  CYC(b_+65, b_+68); objectSetVisible81_hook(gb);
}

void dimitriState1_hook(GB *gb) {
  BASE(dimitriState1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionSetPriorityRelativeToLink_hook, SYM(companionSetPriorityRelativeToLink), b_+3);
  CYC(b_+3, b_+5); C = 0x40;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, companionCheckHazards_hook, SYM(companionCheckHazards), b_+12);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
    goto on_land;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+16, b_+17); ret_effect(gb); return; }
  CYC(b_+16, b_+17);
  CALL_C(b_+17, dimitriAddWaterfallResistance_hook, SYM(dimitriAddWaterfallResistance), b_+20);
  CYC(b_+20, b_+22); A = 0x04;
  CALL_C(b_+22, dimitriFunc_756d_hook, SYM(dimitriFunc_756d), b_+25);
  CYC(b_+25, b_+27);
  goto after_landing;

on_land:
  CYC(b_+27, b_+29); E = 0x38;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
    goto after_landing;
  }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); C = 0x1c;
  CALL_C(b_+37, companionSetAnimation_hook, SYM(companionSetAnimation), b_+40);

after_landing:
  CYC(b_+40, b_+42); A = 0x06;
  CALL_C(b_+42, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+45);
  CYC(b_+45, b_+47); E = 0x3b;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+49, b_+52);
    TAIL(dimitriGotoState1IfLinkFarAway);
  }
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+54); C = 0x09;
  CALL_C(b_+54, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+57);
  if (!(F & FC)) {
    CYCT(b_+57, b_+60);
    TAIL(dimitriCheckAddToGrabbableObjectBuffer);
  }
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+63); companionTryToMount_hook(gb);
}

void dimitriState2_hook(GB *gb) {
  BASE(dimitriState2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (dimitri_jump_table(gb));
    if (jt_ == SYM(dimitriState2Substate0) && hook_enabled_at(gb, SYM(dimitriState2Substate0))) { dimitriState2Substate0_hook(gb); return; }
    else if (jt_ == SYM(dimitriState2Substate1) && hook_enabled_at(gb, SYM(dimitriState2Substate1))) { dimitriState2Substate1_hook(gb); return; }
    else if (jt_ == SYM(dimitriState2Substate2) && hook_enabled_at(gb, SYM(dimitriState2Substate2))) { dimitriState2Substate2_hook(gb); return; }
    else if (jt_ == SYM(dimitriState2Substate3) && hook_enabled_at(gb, SYM(dimitriState2Substate3))) { dimitriState2Substate3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void dimitriState2Substate0_hook(GB *gb) {
  BASE(dimitriState2Substate0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x40;
  CYC(b_+2, b_+5); W8(wLinkGrabState2) = A;
  CALL_C(b_+5, itemIncSubstate_hook, SYM(itemIncSubstate), b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12); W8(wDisableWarpTiles) = A;
  CYC(b_+12, b_+14); L = 0x38;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = 0x3f;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xff);
  CALL_C(b_+19, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+22);
  CYC(b_+22, b_+24); A = 0x02;
  CYC(b_+24, b_+27); SET_HL(wCompanionTutorialTextShown);
  CALL_C(b_+27, setFlag_hook, SYM(setFlag), b_+30);
  CYC(b_+30, b_+32); C = 0x18;
  CYC(b_+32, b_+35); companionSetAnimation_hook(gb);
}

void dimitriState2Substate1_hook(GB *gb) {
  BASE(dimitriState2Substate1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(w1Link_knockbackCounter) = A;
  CYC(b_+4, b_+7); A = W8(wActiveTileType);
  CYC(b_+7, b_+9); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
    goto check_release;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x20;
  CYC(b_+13, b_+16); W8(wStandingOnTileCounter) = A;

check_release:
  CYC(b_+16, b_+19); A = W8(wLinkClimbingVine);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto release_dimitri;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); A = W8(w1Link_angle);
  CYC(b_+25, b_+27); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+27, b_+29);
    goto update;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); E = 0x09;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+35); A = W8(w1Link_direction);
  CYC(b_+35, b_+36); E = alu_dec8(gb, E);
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CALL_C(b_+37, dimitriCheckCanBeHeldInDirection_hook, SYM(dimitriCheckCanBeHeldInDirection), b_+40);
  if (!(F & FZ)) {
    CYCT(b_+40, b_+42);
    goto update;
  }
  CYC(b_+40, b_+42);

release_dimitri:
  CYC(b_+42, b_+43); H = D;
  CYC(b_+43, b_+45); L = 0x00;
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+47, b_+49); L = 0x3b;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x01);
  CYC(b_+51, b_+54); dropLinkHeldItem_hook(gb);
  return;

update:
  CALL_C(b_+54, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+57);
  CALL_C(b_+57, specialObjectCheckMovingTowardWall_hook, SYM(specialObjectCheckMovingTowardWall), b_+60);
  if (F & FZ) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+64); W8(wcc67) = A;
  CYC(b_+64, b_+65); ret_effect(gb);
}

void dimitriState2Substate2_hook(GB *gb) {
  BASE(dimitriState2Substate2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x00;
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CALL_C(b_+5, companionCheckHazards_hook, SYM(companionCheckHazards), b_+8);
  if (!(F & FC)) {
    CYCT(b_+8, b_+10);
    goto no_hazard;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15);
  goto on_hazard;

no_hazard:
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = 0x3f;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
    goto check_movement;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); L = 0x39;
  CYC(b_+27, b_+30); A = W8(w1Link_yh);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+34); A = W8(w1Link_xh);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);

check_movement:
  CYC(b_+35, b_+38); A = W8(wDimitriHitNpc);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+39, b_+41);
    goto stop_movement;
  }
  CYC(b_+39, b_+41);
  CALL_C(b_+41, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+44);
  CALL_C(b_+44, specialObjectCheckMovingTowardWall_hook, SYM(specialObjectCheckMovingTowardWall), b_+47);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
    goto stop_movement;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); C = 0x00;
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = 0x0b;
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+57); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(b_+57, b_+59);
    goto y_upper_bound;
  }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x10);
  CYC(b_+61, b_+62); C = alu_inc8(gb, C);
  CYC(b_+62, b_+64);
  goto check_x;

y_upper_bound:
  CYC(b_+64, b_+67); A = W8(wActiveGroup);
  CYC(b_+67, b_+68); alu_or(gb, A);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(b_+69, b_+71);
    goto large_room_y;
  }
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+73); alu_cp(gb, 0x7a);
  if (F & FC) {
    CYCT(b_+73, b_+75);
    goto check_x;
  }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x7a);
  CYC(b_+77, b_+78); C = alu_inc8(gb, C);
  CYC(b_+78, b_+80);
  goto check_x;

large_room_y:
  CYC(b_+80, b_+82); alu_cp(gb, 0xa8);
  if (F & FC) {
    CYCT(b_+82, b_+84);
    goto check_x;
  }
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0xa8);
  CYC(b_+86, b_+87); C = alu_inc8(gb, C);
  CYC(b_+87, b_+89);

check_x:
  CYC(b_+89, b_+91); L = 0x0d;
  CYC(b_+91, b_+92); A = mem_rd(gb, HL);
  CYC(b_+92, b_+94); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(b_+94, b_+96);
    goto x_upper_bound;
  }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x04);
  CYC(b_+98, b_+99); C = alu_inc8(gb, C);
  CYC(b_+99, b_+101);
  goto bounds_done;

x_upper_bound:
  CYC(b_+101, b_+104); A = W8(wActiveGroup);
  CYC(b_+104, b_+105); alu_or(gb, A);
  CYC(b_+105, b_+106); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+108);
    goto large_room_x;
  }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); alu_cp(gb, 0x9b);
  if (F & FC) {
    CYCT(b_+110, b_+112);
    goto bounds_done;
  }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x9b);
  CYC(b_+114, b_+115); C = alu_inc8(gb, C);
  CYC(b_+115, b_+117);
  goto bounds_done;

large_room_x:
  CYC(b_+117, b_+119); alu_cp(gb, 0xdf);
  if (F & FC) {
    CYCT(b_+119, b_+121);
    goto bounds_done;
  }
  CYC(b_+119, b_+121);
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0xdf);
  CYC(b_+123, b_+124); C = alu_inc8(gb, C);

bounds_done:
  CYC(b_+124, b_+125); A = C;
  CYC(b_+125, b_+126); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+126, b_+128);
    goto check_hazard;
  }
  CYC(b_+126, b_+128);

stop_movement:
  CYC(b_+128, b_+130); A = 0x00;
  CYC(b_+130, b_+133); W8(w1ReservedItemC_speed) = A;

check_hazard:
  CALL_C(b_+133, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+136);
  CYC(b_+136, b_+138); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+138, b_+139); ret_effect(gb); return; }
  CYC(b_+138, b_+139);

on_hazard:
  CYC(b_+139, b_+140); H = D;
  CYC(b_+140, b_+142); L = 0x04;
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0x0b);
  CYC(b_+144, b_+146); L = 0x38;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x04);
  CYC(b_+148, b_+150); L = 0x39;
  CYC(b_+150, b_+151); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+151, b_+152); C = mem_rd(gb, HL);
  CYC(b_+152, b_+153); B = A;
  CALL_C(b_+153, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+156);
  CYC(b_+156, b_+158); alu_and(gb, 0x18);
  CYC(b_+158, b_+160); E = 0x09;
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  CYC(b_+161, b_+162); alu_add(gb, A);
  CYC(b_+162, b_+164); A = alu_swap(gb, A);
  CYC(b_+164, b_+166); alu_and(gb, 0x03);
  CYC(b_+166, b_+167); E = alu_dec8(gb, E);
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  CYC(b_+168, b_+170); C = 0x00;
  CYC(b_+170, b_+173); companionSetAnimation_hook(gb);
}

void dimitriState2Substate3_hook(GB *gb) {
  BASE(dimitriState2Substate3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x00;
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+5, b_+7); C = 0x40;
  CALL_C(b_+7, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, companionTryToBreakTileFromMoving_hook, SYM(companionTryToBreakTileFromMoving), b_+14);
  CALL_C(b_+14, companionCheckHazards_hook, SYM(companionCheckHazards), b_+17);
  if (!(F & FC)) {
    CYCT(b_+17, b_+19);
    goto state1;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+21, b_+22); ret_effect(gb); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); A = 0x04;
  CYC(b_+24, b_+27); dimitriFunc_756d_hook(gb);
  return;

state1:
  CYC(b_+27, b_+28); alu_xor(gb, A);
  TAIL(dimitriFunc_756d);
}

void dimitriFunc_756d_hook(GB *gb) {
  BASE(dimitriFunc_756d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x38;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = 0x04;
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  CYC(b_+11, b_+13); C = 0x1c;
  CYC(b_+13, b_+16); companionSetAnimation_hook(gb);
}

void dimitriState3_hook(GB *gb) {
  BASE(dimitriState3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckMountingComplete_hook, SYM(companionCheckMountingComplete), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, companionFinalizeMounting_hook, SYM(companionFinalizeMounting), b_+7);
  CYC(b_+7, b_+9); C = 0x00;
  CYC(b_+9, b_+12); companionSetAnimation_hook(gb);
}

void dimitriState4_hook(GB *gb) {
  BASE(dimitriState4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDragToCenterOfHole_hook, SYM(companionDragToCenterOfHole), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, companionDecCounter1_hook, SYM(companionDecCounter1), b_+7);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    goto animate;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+12); A = 0x65;
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+17); A = 0x25;
  CYC(b_+17, b_+20); specialObjectSetAnimation_hook(gb);
  return;

animate:
  CALL_C(b_+20, companionAnimateDrowningOrFallingThenRespawn_hook, SYM(companionAnimateDrowningOrFallingThenRespawn), b_+23);
  if (!(F & FC)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); C = 0x00;
  CYC(b_+26, b_+29); companionUpdateDirectionAndSetAnimation_hook(gb);
}

void dimitriState5_hook(GB *gb) {
  BASE(dimitriState5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x40;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wForceCompanionDismount);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
    goto check_dismount;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = W8(wGameKeysJustPressed);
  CYC(b_+15, b_+17); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19);
    TAIL(dimitriGotoEatingState);
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); alu_bit(gb, 1, A);

check_dismount:
  if (!(F & FZ)) {
    CYCT(b_+21, b_+24);
    TAIL(companionGotoDismountState);
  }
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+27); A = W8(wLinkAngle);
  CYC(b_+27, b_+29); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+31);
    goto check_hazards;
  }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); SET_HL(w1Companion_angle);
  CYC(b_+34, b_+35); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+38); C = 0x00;
  if (!(F & FZ)) {
    CYCT(b_+38, b_+41);
    TAIL(companionUpdateDirectionAndAnimate);
  }
  CYC(b_+38, b_+41);
  CALL_C(b_+41, companionCheckHopDownCliff_hook, SYM(companionCheckHopDownCliff), b_+44);
  if (F & FZ) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
  CYC(b_+44, b_+45);
  TAIL(dimitriUpdateMovement);

check_hazards:
  CALL_C(SYM(dimitriUpdateMovement__checkHazards), companionCheckHazards_hook, SYM(companionCheckHazards), (SYM(dimitriUpdateMovement__checkHazards) + 3));
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 3), (SYM(dimitriUpdateMovement__checkHazards) + 4)); H = D;
  if (!(F & FC)) {
    CYCT((SYM(dimitriUpdateMovement__checkHazards) + 4), (SYM(dimitriUpdateMovement__checkHazards) + 6));
    goto not_in_water;
  }
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 4), (SYM(dimitriUpdateMovement__checkHazards) + 6));
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 6), (SYM(dimitriUpdateMovement__checkHazards) + 8)); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT((SYM(dimitriUpdateMovement__checkHazards) + 8), (SYM(dimitriUpdateMovement__checkHazards) + 9)); ret_effect(gb); return; }
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 8), (SYM(dimitriUpdateMovement__checkHazards) + 9));
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 9), (SYM(dimitriUpdateMovement__checkHazards) + 11)); L = 0x04;
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 11), (SYM(dimitriUpdateMovement__checkHazards) + 13)); mem_wr(gb, HL, 0x05);
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 13), (SYM(dimitriUpdateMovement__checkHazards) + 16)); A = W8(wLinkForceState);
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 16), (SYM(dimitriUpdateMovement__checkHazards) + 18)); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT((SYM(dimitriUpdateMovement__checkHazards) + 18), (SYM(dimitriUpdateMovement__checkHazards) + 20));
    goto waterfall;
  }
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 18), (SYM(dimitriUpdateMovement__checkHazards) + 20));
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 20), (SYM(dimitriUpdateMovement__checkHazards) + 21)); alu_xor(gb, A);
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 21), (SYM(dimitriUpdateMovement__checkHazards) + 24)); W8(wLinkForceState) = A;
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 24), (SYM(dimitriUpdateMovement__checkHazards) + 27)); companionGotoHazardHandlingState_hook(gb);
  return;

waterfall:
  CALL_C((SYM(dimitriUpdateMovement__checkHazards) + 27), dimitriAddWaterfallResistance_hook, SYM(dimitriAddWaterfallResistance), (SYM(dimitriUpdateMovement__checkHazards) + 30));
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 30), (SYM(dimitriUpdateMovement__checkHazards) + 32)); B = 0x04;
  CYC((SYM(dimitriUpdateMovement__checkHazards) + 32), (SYM(dimitriUpdateMovement__checkHazards) + 34));
  goto set_water_status;

not_in_water:
  CYC(SYM(dimitriUpdateMovement__setNotInWater), (SYM(dimitriUpdateMovement__setNotInWater) + 2)); B = 0x00;

set_water_status:
  CYC(SYM(dimitriUpdateMovement__setWaterStatus), (SYM(dimitriUpdateMovement__setWaterStatus) + 2)); L = 0x38;
  CYC((SYM(dimitriUpdateMovement__setWaterStatus) + 2), (SYM(dimitriUpdateMovement__setWaterStatus) + 3)); A = mem_rd(gb, HL);
  CYC((SYM(dimitriUpdateMovement__setWaterStatus) + 3), (SYM(dimitriUpdateMovement__setWaterStatus) + 4)); alu_cp(gb, B);
  CYC((SYM(dimitriUpdateMovement__setWaterStatus) + 4), (SYM(dimitriUpdateMovement__setWaterStatus) + 5)); mem_wr(gb, HL, B);
  CYC((SYM(dimitriUpdateMovement__setWaterStatus) + 5), (SYM(dimitriUpdateMovement__setWaterStatus) + 7)); C = 0x00;
  if (!(F & FZ)) {
    CYCT((SYM(dimitriUpdateMovement__setWaterStatus) + 7), (SYM(dimitriUpdateMovement__setWaterStatus) + 10));
    TAIL(companionUpdateDirectionAndSetAnimation);
  }
  CYC((SYM(dimitriUpdateMovement__setWaterStatus) + 7), (SYM(dimitriUpdateMovement__setWaterStatus) + 10));
  TAIL(dimitriState9);
}

void dimitriUpdateMovement_hook(GB *gb) {
  BASE(dimitriUpdateMovement);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x21;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_rlca(gb);
  CYC(b_+5, b_+7); A = 0x88;
  if (F & FC) {
    CALL_C_CC(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  CYC(b_+10, b_+12); L = 0x38;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_or(gb, A);
  CYC(b_+14, b_+16); A = 0x1e;
  if (F & FZ) {
    CYCT(b_+16, b_+18);
  } else {
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+20); A = 0x28;
  }
  CYC(b_+20, b_+22); L = 0x10;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CALL_C(b_+23, companionUpdateMovement_hook, SYM(companionUpdateMovement), b_+26);
  CALL_C(b_+26, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+29);
  CALL_C(b_+29, companionCheckHazards_hook, SYM(companionCheckHazards), b_+32);
  CYC(b_+32, b_+33); H = D;
  if (!(F & FC)) {
    CYCT(b_+33, b_+35);
    goto not_in_water;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+37, b_+38); ret_effect(gb); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); L = 0x04;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x05);
  CYC(b_+42, b_+45); A = W8(wLinkForceState);
  CYC(b_+45, b_+47); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
    goto waterfall;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); alu_xor(gb, A);
  CYC(b_+50, b_+53); W8(wLinkForceState) = A;
  CYC(b_+53, b_+56); companionGotoHazardHandlingState_hook(gb);
  return;

waterfall:
  CALL_C(b_+56, dimitriAddWaterfallResistance_hook, SYM(dimitriAddWaterfallResistance), b_+59);
  CYC(b_+59, b_+61); B = 0x04;
  CYC(b_+61, b_+63);
  goto set_water_status;

not_in_water:
  CYC(b_+63, b_+65); B = 0x00;

set_water_status:
  CYC(b_+65, b_+67); L = 0x38;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); alu_cp(gb, B);
  CYC(b_+69, b_+70); mem_wr(gb, HL, B);
  CYC(b_+70, b_+72); C = 0x00;
  if (!(F & FZ)) {
    CYCT(b_+72, b_+75);
    TAIL(companionUpdateDirectionAndSetAnimation);
  }
  CYC(b_+72, b_+75);
  TAIL(dimitriState9);
}

void dimitriState9_hook(GB *gb) {
  BASE(dimitriState9);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void dimitriGotoEatingState_hook(GB *gb) {
  BASE(dimitriGotoEatingState);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x04;
  CYC(b_+3, b_+5); A = 0x08;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = 0x35;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = 0x08;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+18); alu_rrca(gb);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+24); W8(wLinkInAir) = A;
  CYC(b_+24, b_+26); L = 0x10;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x1e);
  CYC(b_+28, b_+30); C = 0x08;
  CALL_C(b_+30, companionSetAnimation_hook, SYM(companionSetAnimation), b_+33);
  CYC(b_+33, b_+36); SET_BC(0x2b00);
  CALL_C(b_+36, companionCreateWeaponItem_hook, SYM(companionCreateWeaponItem), b_+39);
  CYC(b_+39, b_+41); A = 0xc4;
  CYC(b_+41, b_+44); playSound_b00_hook(gb);
}

void dimitriState6_hook(GB *gb) {
  BASE(dimitriState6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dimitri_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+21) { goto substate1; }
    else if (jt_ == b_+29) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, companionDismountAndSavePosition_hook, SYM(companionDismountAndSavePosition), b_+16);
  CYC(b_+16, b_+18); C = 0x1c;
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
  CALL_C(b_+29, dimitriCheckAddToGrabbableObjectBuffer_hook, SYM(dimitriCheckAddToGrabbableObjectBuffer), SYM(dimitriGotoState1IfLinkFarAway));
  TAIL(dimitriGotoState1IfLinkFarAway);
}

void dimitriGotoState1IfLinkFarAway_hook(GB *gb) {
  BASE(dimitriGotoState1IfLinkFarAway);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x09;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (F & FC) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  TAIL(dimitriGotoState1);
}

void dimitriGotoState1_hook(GB *gb) {
  BASE(dimitriGotoState1);
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); E = 0x3b;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void dimitriState7_hook(GB *gb) {
  BASE(dimitriState7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDecCounter1ToJumpDownCliff_hook, SYM(companionDecCounter1ToJumpDownCliff), b_+3);
  if (F & FC) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+7);
  CALL_C(b_+7, specialObjectCheckMovingAwayFromWall_hook, SYM(specialObjectCheckMovingAwayFromWall), b_+10);
  CYC(b_+10, b_+12); L = 0x07;
  if (F & FZ) {
    CYCT(b_+12, b_+14);
    goto stopped;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+16); ret_effect(gb);
  return;

stopped:
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+22); dimitriLandOnGroundAndGotoState5_hook(gb);
}

void dimitriState8_hook(GB *gb) {
  BASE(dimitriState8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dimitri_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+42) { goto substate1; }
    else if (jt_ == b_+76) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+10, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+13);
  CALL_C(b_+13, objectApplySpeed_hook, SYM(objectApplySpeed), b_+16);
  CYC(b_+16, b_+18); E = 0x21;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+20, b_+21); ret_effect(gb); return; }
  CYC(b_+20, b_+21);
  CALL_C(b_+21, itemIncSubstate_hook, SYM(itemIncSubstate), b_+24);
  CYC(b_+24, b_+26); L = 0x08;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+29); alu_xor(gb, 0x02);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+32); alu_rrca(gb);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+35); L = 0x06;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x0c);
  CYC(b_+37, b_+39); C = 0x00;
  CYC(b_+39, b_+42); companionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(b_+42, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+45);
  CALL_C(b_+45, objectApplySpeed_hook, SYM(objectApplySpeed), b_+48);
  CALL_C(b_+48, companionDecCounter1IfNonzero_hook, SYM(companionDecCounter1IfNonzero), b_+51);
  if (!(F & FZ)) { CYCT(b_+51, b_+52); ret_effect(gb); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x14);
  CYC(b_+54, b_+56); L = 0x08;
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+57, b_+59); A = alu_swap(gb, A);
  CYC(b_+59, b_+60); alu_rrca(gb);
  CYC(b_+60, b_+61); mem_wr(gb, HL, A);
  CYC(b_+61, b_+63); L = 0x35;
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+65, b_+68);
    TAIL(dimitriLandOnGroundAndGotoState5);
  }
  CYC(b_+65, b_+68);
  CALL_C(b_+68, itemIncSubstate_hook, SYM(itemIncSubstate), b_+71);
  CYC(b_+71, b_+73); C = 0x10;
  CYC(b_+73, b_+76); companionSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(b_+76, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+79);
  CALL_C(b_+79, companionDecCounter1IfNonzero_hook, SYM(companionDecCounter1IfNonzero), b_+82);
  if (!(F & FZ)) { CYCT(b_+82, b_+83); ret_effect(gb); return; }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+85); dimitriLandOnGroundAndGotoState5_hook(gb);
}

void dimitriStateB_hook(GB *gb) {
  BASE(dimitriStateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x40;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, dimitriUpdateMovement_hook, SYM(dimitriUpdateMovement), b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0x38;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_or(gb, A);
  CYC(b_+14, b_+16); L = 0x04;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0b);
  if (!(F & FZ)) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x01);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void dimitriStateC_hook(GB *gb) {
  BASE(dimitriStateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dimitri_jump_table(gb));
    if (jt_ == b_+8) { goto parameter0; }
    else if (jt_ == b_+23) { goto parameter1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

parameter0:
  CALL_C(b_+8, companionInitializeOnEnteringScreen_hook, SYM(companionInitializeOnEnteringScreen), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x3c);
  CYC(b_+13, b_+15); A = 0xc4;
  CALL_C(b_+15, playSound_b00_hook, SYM(playSound_b00), b_+18);
  CYC(b_+18, b_+20); C = 0x00;
  CYC(b_+20, b_+23); companionSetAnimation_hook(gb);
  return;

parameter1:
  CALL_C(b_+23, dimitriUpdateMovement_hook, SYM(dimitriUpdateMovement), b_+26);
  CYC(b_+26, b_+28); E = 0x04;
  CYC(b_+28, b_+30); A = 0x0c;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+34); SET_HL(SYM(dimitriTileOffsets));
  CALL_C(b_+34, companionRetIfNotFinishedWalkingIn_hook, SYM(companionRetIfNotFinishedWalkingIn), b_+37);
  CYC(b_+37, b_+39); E = 0x03;
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+44); dimitriState0_hook(gb);
}

void dimitriStateD_hook(GB *gb) {
  BASE(dimitriStateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3c;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto state5;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, dimitriGotoState1_hook, SYM(dimitriGotoState1), b_+9);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); SET_HL(w1Companion_enabled);
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+16, b_+18); C = 0x1c;
  CYC(b_+18, b_+21); companionSetAnimation_hook(gb);
  return;

state5:
  CYC(b_+21, b_+23); E = 0x04;
  CYC(b_+23, b_+25); A = 0x05;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  TAIL(dimitriLandOnGroundAndGotoState5);
}

void dimitriLandOnGroundAndGotoState5_hook(GB *gb) {
  BASE(dimitriLandOnGroundAndGotoState5);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wLinkInAir) = A;
  CYC(b_+4, b_+6); C = 0x00;
  CYC(b_+6, b_+9); companionSetAnimationAndGotoState5_hook(gb);
}

void dimitriStateA_hook(GB *gb) {
  BASE(dimitriStateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dimitri_jump_table(gb));
    if (jt_ == SYM(dimitriStateASubstate0) && hook_enabled_at(gb, SYM(dimitriStateASubstate0))) { dimitriStateASubstate0_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateASubstate1) && hook_enabled_at(gb, SYM(dimitriStateASubstate1))) { dimitriStateASubstate1_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateASubstate2) && hook_enabled_at(gb, SYM(dimitriStateASubstate2))) { dimitriStateASubstate2_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateASubstate3) && hook_enabled_at(gb, SYM(dimitriStateASubstate3))) { dimitriStateASubstate3_hook(gb); return; }
    else if (jt_ == SYM(dimitriStateASubstate4) && hook_enabled_at(gb, SYM(dimitriStateASubstate4))) { dimitriStateASubstate4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void dimitriStateASubstate0_hook(GB *gb) {
  BASE(dimitriStateASubstate0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3d;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto update;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x81;
  CYC(b_+8, b_+11); W8(wDisabledObjects) = A;

update:
  CALL_C(b_+11, companionSetAnimationToVar3f_hook, SYM(companionSetAnimationToVar3f), b_+14);
  CALL_C(b_+14, companionPreventLinkFromPassing_noExtraChecks_hook, SYM(companionPreventLinkFromPassing_noExtraChecks), b_+17);
  CALL_C(b_+17, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+20);
  CYC(b_+20, b_+22); E = 0x1a;
  CYC(b_+22, b_+24); A = 0xc7;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); A = W8(wDimitriState);
  CYC(b_+28, b_+30); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+30, b_+31); ret_effect(gb); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); E = 0x1a;
  CYC(b_+33, b_+35); A = 0xc1;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); A = 0xff;
  CYC(b_+38, b_+41); W8(wStatusBarNeedsRefresh) = A;
  CYC(b_+41, b_+43); C = 0x1c;
  CALL_C(b_+43, companionSetAnimation_hook, SYM(companionSetAnimation), b_+46);
  CYC(b_+46, b_+49); companionForceMount_hook(gb);
}

void dimitriStateASubstate1_hook(GB *gb) {
  BASE(dimitriStateASubstate1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3d;
  CALL_C(b_+2, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+5);
  CYC(b_+5, b_+7); C = 0x1c;
  CALL_C(b_+7, companionSetAnimation_hook, SYM(companionSetAnimation), b_+10);
  CYC(b_+10, b_+13); companionForceMount_hook(gb);
}

void dimitriStateASubstate3_hook(GB *gb) {
  BASE(dimitriStateASubstate3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+8); A = 0x08;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); C = 0x00;
  CALL_C(b_+11, companionSetAnimation_hook, SYM(companionSetAnimation), b_+14);
  CYC(b_+14, b_+16); E = 0x03;
  CYC(b_+16, b_+18); A = 0x04;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); A = 0xc4;
  CYC(b_+21, b_+24); playSound_b00_hook(gb);
}

void dimitriStateASubstate4_hook(GB *gb) {
  BASE(dimitriStateASubstate4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dimitriUpdateMovement_hook, SYM(dimitriUpdateMovement), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+7); A = 0x0a;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+11);
  if (F & FC) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); W8(wDisabledObjects) = A;
  CYC(b_+16, b_+19); W8(wMenuDisabled) = A;
  CYC(b_+19, b_+22); W8(wUseSimulatedInput) = A;
  CYC(b_+22, b_+25); itemDelete_hook(gb);
}

void dimitriStateASubstate2_hook(GB *gb) {
  BASE(dimitriStateASubstate2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0xd1);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, companionDismountAndSavePosition_hook, SYM(companionDismountAndSavePosition), b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+13); W8(wRememberedCompanionId) = A;
  CYC(b_+13, b_+14); ret_effect(gb);
}

void dimitriCheckAddToGrabbableObjectBuffer_hook(GB *gb) {
  BASE(dimitriCheckAddToGrabbableObjectBuffer);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkClimbingVine);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(w1Link_direction);
  CALL_C(b_+8, dimitriCheckCanBeHeldInDirection_hook, SYM(dimitriCheckCanBeHeldInDirection), b_+11);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_HL(w1Link_yh);
  CYC(b_+15, b_+16); B = mem_rd(gb, HL);
  CYC(b_+16, b_+18); L = 0x0d;
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CALL_C(b_+19, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+22);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+24, b_+26); goto done; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(b_+28, b_+30); goto done; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(b_+32, b_+34); goto done; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); alu_cp(gb, 0x19);
  if (!(F & FZ)) {
    CALL_C_CC(b_+36, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+39);
  } else {
    CYC(b_+36, b_+39);
  }

done:
  CYC(b_+39, b_+40); ret_effect(gb);
}

void dimitriCheckCanBeHeldInDirection_hook(GB *gb) {
  BASE(dimitriCheckCanBeHeldInDirection);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  goto check_tile;

after_initial_check:
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(w1Link_angle);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+8, b_+10); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
    goto diagonal;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); alu_or(gb, D);
  CYC(b_+16, b_+17); ret_effect(gb);
  return;

diagonal:
  CYC(b_+17, b_+18); alu_add(gb, A);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+24); A = alu_srl(gb, A);
  CYC(b_+24, b_+25); alu_xor(gb, B);
  CYC(b_+25, b_+26); alu_add(gb, A);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+30); alu_and(gb, 0x03);

check_tile:
  CYC(b_+30, b_+33); SET_HL(SYM(dimitriTileOffsets));
  CYC(b_+33, b_+34); dimitri_add_double_index(gb, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+35, b_+36); C = mem_rd(gb, HL);
  CYC(b_+36, b_+37); B = A;
  CALL_C(b_+37, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+40);
  CYC(b_+40, b_+42); alu_cp(gb, GV(0xd6, 0xdf));
  if (F & FZ) { CYCT(b_+42, b_+43); goto return_from_check_tile; }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); alu_cp(gb, GV(0xd5, 0xde));
  if (F & FZ) { CYCT(b_+45, b_+46); goto return_from_check_tile; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); alu_cp(gb, GV(0xd4, 0xdd));
  if (F & FZ) { CYCT(b_+48, b_+49); goto return_from_check_tile; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); H = 0xce;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+54, b_+55); goto return_from_check_tile; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(b_+57, b_+58); goto return_from_check_tile; }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); alu_cp(gb, 0x19);
  CYC(b_+60, b_+61);

return_from_check_tile:
  ret_effect(gb);
  if (gb->pc == b_+3 && gb->sp == sp0_) goto after_initial_check;
}

void dimitriAddWaterfallResistance_hook(GB *gb) {
  BASE(dimitriAddWaterfallResistance);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    goto move_down;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); alu_cp(gb, 0xfe);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);

move_down:
  CYC(b_+11, b_+13); L = 0x0a;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_add(gb, 0xc0);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_adc(gb, 0x00);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+24); A = W8(wScreenTransitionBoundaryY);
  CYC(b_+24, b_+25); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+25, b_+26); ret_effect(gb); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); A = 0x82;
  CYC(b_+28, b_+31); W8(wScreenTransitionDirection) = A;
  CYC(b_+31, b_+32); ret_effect(gb);
}
