#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7382, companionRetIfInactive_hook, 0x4883, 0x7385);
  CALL_C(0x7385, companionFunc_47d8_hook, 0x47d8, 0x7388);
  CALL_C(0x7388, specialObjectCode_dimitri__runState_hook, 0x7392, 0x738b);
  CYC(0x738b, 0x738c); alu_xor(gb, A);
  CYC(0x738c, 0x738f); W8(wDimitriHitNpc) = A;
  CYC(0x738f, 0x7392); companionCheckEnableTerrainEffects_hook(gb);
}

void specialObjectCode_dimitri__runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7392, 0x7394); E = 0x04;
  CYC(0x7394, 0x7395); A = mem_rd(gb, DE);
  CYC(0x7395, 0x7396); push_effect(gb, 0x7396);
  switch (dimitri_jump_table(gb)) {
    case 0x73b2: dimitriState0_hook(gb); return;
    case 0x73f6: dimitriState1_hook(gb); return;
    case 0x7435: dimitriState2_hook(gb); return;
    case 0x757d: dimitriState3_hook(gb); return;
    case 0x7589: dimitriState4_hook(gb); return;
    case 0x75a6: dimitriState5_hook(gb); return;
    case 0x764b: dimitriState6_hook(gb); return;
    case 0x767d: dimitriState7_hook(gb); return;
    case 0x7693: dimitriState8_hook(gb); return;
    case 0x761e: dimitriState9_hook(gb); return;
    case 0x774d: dimitriStateA_hook(gb); return;
    case 0x76e8: dimitriStateB_hook(gb); return;
    case 0x76fe: dimitriStateC_hook(gb); return;
    case 0x772a: dimitriStateD_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void dimitriState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x73b2, companionCheckCanSpawn_hook, 0x4822, 0x73b5);
  CYC(0x73b5, 0x73b7); A = 0x02;
  CYC(0x73b7, 0x73b9); L = 0x08;
  CYC(0x73b9, 0x73ba); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x73ba, 0x73bb); mem_wr(gb, HL, A);
  CYC(0x73bb, 0x73be); A = W8(wDimitriState);
  CYC(0x73be, 0x73c0); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x73c0, 0x73c2);
    goto set_animation;
  }
  CYC(0x73c0, 0x73c2);
  CYC(0x73c2, 0x73c4); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x73c4, 0x73c6);
    goto initialize_cutscene;
  }
  CYC(0x73c4, 0x73c6);
  CYC(0x73c6, 0x73c8); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x73c8, 0x73ca);
    goto set_animation;
  }
  CYC(0x73c8, 0x73ca);

initialize_cutscene:
  CYC(0x73ca, 0x73cc); A = 0x24;
  CALL_C(0x73cc, checkGlobalFlag_hook, 0x31f3, 0x73cf);
  CYC(0x73cf, 0x73d0); H = D;
  CYC(0x73d0, 0x73d2); C = 0x24;
  if (F & FZ) {
    CYCT(0x73d2, 0x73d4);
  } else {
    CYC(0x73d2, 0x73d4);
    CYC(0x73d4, 0x73d6); C = 0x1e;
  }
  CYC(0x73d6, 0x73d8); L = 0x04;
  CYC(0x73d8, 0x73da); mem_wr(gb, HL, 0x0a);
  CYC(0x73da, 0x73dc); E = 0x3d;
  CALL_C(0x73dc, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x73df);
  CYC(0x73df, 0x73e0); A = C;
  CYC(0x73e0, 0x73e2); E = 0x3f;
  CYC(0x73e2, 0x73e3); mem_wr(gb, DE, A);
  CALL_C(0x73e3, specialObjectSetAnimation_hook, 0x2b0a, 0x73e6);
  CYC(0x73e6, 0x73e9); SET_BC(0x0408);
  CALL_C(0x73e9, objectSetCollideRadii_hook, 0x24a9, 0x73ec);
  CYC(0x73ec, 0x73ee);
  goto set_visible;

set_animation:
  CYC(0x73ee, 0x73f0); C = 0x1c;
  CALL_C(0x73f0, companionSetAnimation_hook, 0x458e, 0x73f3);

set_visible:
  CYC(0x73f3, 0x73f6); objectSetVisible81_hook(gb);
}

void dimitriState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x73f6, companionSetPriorityRelativeToLink_hook, 0x4936, 0x73f9);
  CYC(0x73f9, 0x73fb); C = 0x40;
  CALL_C(0x73fb, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x73fe);
  if (!(F & FZ)) { CYCT(0x73fe, 0x73ff); ret_effect(gb); return; }
  CYC(0x73fe, 0x73ff);
  CALL_C(0x73ff, companionCheckHazards_hook, 0x45ec, 0x7402);
  if (!(F & FC)) {
    CYCT(0x7402, 0x7404);
    goto on_land;
  }
  CYC(0x7402, 0x7404);
  CYC(0x7404, 0x7406); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x7406, 0x7407); ret_effect(gb); return; }
  CYC(0x7406, 0x7407);
  CALL_C(0x7407, dimitriAddWaterfallResistance_hook, 0x783d, 0x740a);
  CYC(0x740a, 0x740c); A = 0x04;
  CALL_C(0x740c, dimitriFunc_756d_hook, 0x756d, 0x740f);
  CYC(0x740f, 0x7411);
  goto after_landing;

on_land:
  CYC(0x7411, 0x7413); E = 0x38;
  CYC(0x7413, 0x7414); A = mem_rd(gb, DE);
  CYC(0x7414, 0x7415); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7415, 0x7417);
    goto after_landing;
  }
  CYC(0x7415, 0x7417);
  CYC(0x7417, 0x7418); alu_xor(gb, A);
  CYC(0x7418, 0x7419); mem_wr(gb, DE, A);
  CYC(0x7419, 0x741b); C = 0x1c;
  CALL_C(0x741b, companionSetAnimation_hook, 0x458e, 0x741e);

after_landing:
  CYC(0x741e, 0x7420); A = 0x06;
  CALL_C(0x7420, objectSetCollideRadius_hook, 0x24a1, 0x7423);
  CYC(0x7423, 0x7425); E = 0x3b;
  CYC(0x7425, 0x7426); A = mem_rd(gb, DE);
  CYC(0x7426, 0x7427); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7427, 0x742a);
    dimitriGotoState1IfLinkFarAway_hook(gb);
    return;
  }
  CYC(0x7427, 0x742a);
  CYC(0x742a, 0x742c); C = 0x09;
  CALL_C(0x742c, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x742f);
  if (!(F & FC)) {
    CYCT(0x742f, 0x7432);
    dimitriCheckAddToGrabbableObjectBuffer_hook(gb);
    return;
  }
  CYC(0x742f, 0x7432);
  CYC(0x7432, 0x7435); companionTryToMount_hook(gb);
}

void dimitriState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7435, 0x7436); E = alu_inc8(gb, E);
  CYC(0x7436, 0x7437); A = mem_rd(gb, DE);
  CYC(0x7437, 0x7438); push_effect(gb, 0x7438);
  switch (dimitri_jump_table(gb)) {
    case 0x7440: dimitriState2Substate0_hook(gb); return;
    case 0x7463: dimitriState2Substate1_hook(gb); return;
    case 0x74a4: dimitriState2Substate2_hook(gb); return;
    case 0x7551: dimitriState2Substate3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void dimitriState2Substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7440, 0x7442); A = 0x40;
  CYC(0x7442, 0x7445); W8(wLinkGrabState2) = A;
  CALL_C(0x7445, itemIncSubstate_hook, 0x23ef, 0x7448);
  CYC(0x7448, 0x7449); alu_xor(gb, A);
  CYC(0x7449, 0x744c); W8(wDisableWarpTiles) = A;
  CYC(0x744c, 0x744e); L = 0x38;
  CYC(0x744e, 0x744f); mem_wr(gb, HL, A);
  CYC(0x744f, 0x7451); L = 0x3f;
  CYC(0x7451, 0x7453); mem_wr(gb, HL, 0xff);
  CALL_C(0x7453, objectSetVisiblec0_hook, 0x1e33, 0x7456);
  CYC(0x7456, 0x7458); A = 0x02;
  CYC(0x7458, 0x745b); SET_HL(wCompanionTutorialTextShown);
  CALL_C(0x745b, setFlag_hook, 0x020e, 0x745e);
  CYC(0x745e, 0x7460); C = 0x18;
  CYC(0x7460, 0x7463); companionSetAnimation_hook(gb);
}

void dimitriState2Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7463, 0x7464); alu_xor(gb, A);
  CYC(0x7464, 0x7467); W8(w1Link_knockbackCounter) = A;
  CYC(0x7467, 0x746a); A = W8(wActiveTileType);
  CYC(0x746a, 0x746c); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x746c, 0x746e);
    goto check_release;
  }
  CYC(0x746c, 0x746e);
  CYC(0x746e, 0x7470); A = 0x20;
  CYC(0x7470, 0x7473); W8(wStandingOnTileCounter) = A;

check_release:
  CYC(0x7473, 0x7476); A = W8(wLinkClimbingVine);
  CYC(0x7476, 0x7477); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7477, 0x7479);
    goto release_dimitri;
  }
  CYC(0x7477, 0x7479);
  CYC(0x7479, 0x747c); A = W8(w1Link_angle);
  CYC(0x747c, 0x747e); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x747e, 0x7480);
    goto update;
  }
  CYC(0x747e, 0x7480);
  CYC(0x7480, 0x7482); E = 0x09;
  CYC(0x7482, 0x7483); mem_wr(gb, DE, A);
  CYC(0x7483, 0x7486); A = W8(w1Link_direction);
  CYC(0x7486, 0x7487); E = alu_dec8(gb, E);
  CYC(0x7487, 0x7488); mem_wr(gb, DE, A);
  CALL_C(0x7488, dimitriCheckCanBeHeldInDirection_hook, 0x7800, 0x748b);
  if (!(F & FZ)) {
    CYCT(0x748b, 0x748d);
    goto update;
  }
  CYC(0x748b, 0x748d);

release_dimitri:
  CYC(0x748d, 0x748e); H = D;
  CYC(0x748e, 0x7490); L = 0x00;
  CYC(0x7490, 0x7492); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x7492, 0x7494); L = 0x3b;
  CYC(0x7494, 0x7496); mem_wr(gb, HL, 0x01);
  CYC(0x7496, 0x7499); dropLinkHeldItem_hook(gb);
  return;

update:
  CALL_C(0x7499, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x749c);
  CALL_C(0x749c, specialObjectCheckMovingTowardWall_hook, 0x451b, 0x749f);
  if (F & FZ) { CYCT(0x749f, 0x74a0); ret_effect(gb); return; }
  CYC(0x749f, 0x74a0);
  CYC(0x74a0, 0x74a3); W8(wcc67) = A;
  CYC(0x74a3, 0x74a4); ret_effect(gb);
}

void dimitriState2Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74a4, 0x74a5); H = D;
  CYC(0x74a5, 0x74a7); L = 0x00;
  CYC(0x74a7, 0x74a9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CALL_C(0x74a9, companionCheckHazards_hook, 0x45ec, 0x74ac);
  if (!(F & FC)) {
    CYCT(0x74ac, 0x74ae);
    goto no_hazard;
  }
  CYC(0x74ac, 0x74ae);
  CYC(0x74ae, 0x74b0); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x74b0, 0x74b1); ret_effect(gb); return; }
  CYC(0x74b0, 0x74b1);
  CYC(0x74b1, 0x74b3);
  goto on_hazard;

no_hazard:
  CYC(0x74b3, 0x74b4); H = D;
  CYC(0x74b4, 0x74b6); L = 0x3f;
  CYC(0x74b6, 0x74b7); A = mem_rd(gb, HL);
  CYC(0x74b7, 0x74b9); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x74b9, 0x74bb);
    goto check_movement;
  }
  CYC(0x74b9, 0x74bb);
  CYC(0x74bb, 0x74bc); alu_xor(gb, A);
  CYC(0x74bc, 0x74bd); mem_wr(gb, HL, A);
  CYC(0x74bd, 0x74bf); L = 0x39;
  CYC(0x74bf, 0x74c2); A = W8(w1Link_yh);
  CYC(0x74c2, 0x74c3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x74c3, 0x74c6); A = W8(w1Link_xh);
  CYC(0x74c6, 0x74c7); mem_wr(gb, HL, A);

check_movement:
  CYC(0x74c7, 0x74ca); A = W8(wDimitriHitNpc);
  CYC(0x74ca, 0x74cb); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x74cb, 0x74cd);
    goto stop_movement;
  }
  CYC(0x74cb, 0x74cd);
  CALL_C(0x74cd, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x74d0);
  CALL_C(0x74d0, specialObjectCheckMovingTowardWall_hook, 0x451b, 0x74d3);
  if (!(F & FZ)) {
    CYCT(0x74d3, 0x74d5);
    goto stop_movement;
  }
  CYC(0x74d3, 0x74d5);
  CYC(0x74d5, 0x74d7); C = 0x00;
  CYC(0x74d7, 0x74d8); H = D;
  CYC(0x74d8, 0x74da); L = 0x0b;
  CYC(0x74da, 0x74db); A = mem_rd(gb, HL);
  CYC(0x74db, 0x74dd); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(0x74dd, 0x74df);
    goto y_upper_bound;
  }
  CYC(0x74dd, 0x74df);
  CYC(0x74df, 0x74e1); mem_wr(gb, HL, 0x10);
  CYC(0x74e1, 0x74e2); C = alu_inc8(gb, C);
  CYC(0x74e2, 0x74e4);
  goto check_x;

y_upper_bound:
  CYC(0x74e4, 0x74e7); A = W8(wActiveGroup);
  CYC(0x74e7, 0x74e8); alu_or(gb, A);
  CYC(0x74e8, 0x74e9); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(0x74e9, 0x74eb);
    goto large_room_y;
  }
  CYC(0x74e9, 0x74eb);
  CYC(0x74eb, 0x74ed); alu_cp(gb, 0x7a);
  if (F & FC) {
    CYCT(0x74ed, 0x74ef);
    goto check_x;
  }
  CYC(0x74ed, 0x74ef);
  CYC(0x74ef, 0x74f1); mem_wr(gb, HL, 0x7a);
  CYC(0x74f1, 0x74f2); C = alu_inc8(gb, C);
  CYC(0x74f2, 0x74f4);
  goto check_x;

large_room_y:
  CYC(0x74f4, 0x74f6); alu_cp(gb, 0xa8);
  if (F & FC) {
    CYCT(0x74f6, 0x74f8);
    goto check_x;
  }
  CYC(0x74f6, 0x74f8);
  CYC(0x74f8, 0x74fa); mem_wr(gb, HL, 0xa8);
  CYC(0x74fa, 0x74fb); C = alu_inc8(gb, C);
  CYC(0x74fb, 0x74fd);

check_x:
  CYC(0x74fd, 0x74ff); L = 0x0d;
  CYC(0x74ff, 0x7500); A = mem_rd(gb, HL);
  CYC(0x7500, 0x7502); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(0x7502, 0x7504);
    goto x_upper_bound;
  }
  CYC(0x7502, 0x7504);
  CYC(0x7504, 0x7506); mem_wr(gb, HL, 0x04);
  CYC(0x7506, 0x7507); C = alu_inc8(gb, C);
  CYC(0x7507, 0x7509);
  goto bounds_done;

x_upper_bound:
  CYC(0x7509, 0x750c); A = W8(wActiveGroup);
  CYC(0x750c, 0x750d); alu_or(gb, A);
  CYC(0x750d, 0x750e); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(0x750e, 0x7510);
    goto large_room_x;
  }
  CYC(0x750e, 0x7510);
  CYC(0x7510, 0x7512); alu_cp(gb, 0x9b);
  if (F & FC) {
    CYCT(0x7512, 0x7514);
    goto bounds_done;
  }
  CYC(0x7512, 0x7514);
  CYC(0x7514, 0x7516); mem_wr(gb, HL, 0x9b);
  CYC(0x7516, 0x7517); C = alu_inc8(gb, C);
  CYC(0x7517, 0x7519);
  goto bounds_done;

large_room_x:
  CYC(0x7519, 0x751b); alu_cp(gb, 0xdf);
  if (F & FC) {
    CYCT(0x751b, 0x751d);
    goto bounds_done;
  }
  CYC(0x751b, 0x751d);
  CYC(0x751d, 0x751f); mem_wr(gb, HL, 0xdf);
  CYC(0x751f, 0x7520); C = alu_inc8(gb, C);

bounds_done:
  CYC(0x7520, 0x7521); A = C;
  CYC(0x7521, 0x7522); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7522, 0x7524);
    goto check_hazard;
  }
  CYC(0x7522, 0x7524);

stop_movement:
  CYC(0x7524, 0x7526); A = 0x00;
  CYC(0x7526, 0x7529); W8(w1ReservedItemC_speed) = A;

check_hazard:
  CALL_C(0x7529, objectCheckIsOnHazard_hook, 0x220d, 0x752c);
  CYC(0x752c, 0x752e); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x752e, 0x752f); ret_effect(gb); return; }
  CYC(0x752e, 0x752f);

on_hazard:
  CYC(0x752f, 0x7530); H = D;
  CYC(0x7530, 0x7532); L = 0x04;
  CYC(0x7532, 0x7534); mem_wr(gb, HL, 0x0b);
  CYC(0x7534, 0x7536); L = 0x38;
  CYC(0x7536, 0x7538); mem_wr(gb, HL, 0x04);
  CYC(0x7538, 0x753a); L = 0x39;
  CYC(0x753a, 0x753b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x753b, 0x753c); C = mem_rd(gb, HL);
  CYC(0x753c, 0x753d); B = A;
  CALL_C(0x753d, objectGetRelativeAngle_hook, 0x1ea4, 0x7540);
  CYC(0x7540, 0x7542); alu_and(gb, 0x18);
  CYC(0x7542, 0x7544); E = 0x09;
  CYC(0x7544, 0x7545); mem_wr(gb, DE, A);
  CYC(0x7545, 0x7546); alu_add(gb, A);
  CYC(0x7546, 0x7548); A = alu_swap(gb, A);
  CYC(0x7548, 0x754a); alu_and(gb, 0x03);
  CYC(0x754a, 0x754b); E = alu_dec8(gb, E);
  CYC(0x754b, 0x754c); mem_wr(gb, DE, A);
  CYC(0x754c, 0x754e); C = 0x00;
  CYC(0x754e, 0x7551); companionSetAnimation_hook(gb);
}

void dimitriState2Substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7551, 0x7552); H = D;
  CYC(0x7552, 0x7554); L = 0x00;
  CYC(0x7554, 0x7556); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x7556, 0x7558); C = 0x40;
  CALL_C(0x7558, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x755b);
  if (!(F & FZ)) { CYCT(0x755b, 0x755c); ret_effect(gb); return; }
  CYC(0x755b, 0x755c);
  CALL_C(0x755c, companionTryToBreakTileFromMoving_hook, 0x4477, 0x755f);
  CALL_C(0x755f, companionCheckHazards_hook, 0x45ec, 0x7562);
  if (!(F & FC)) {
    CYCT(0x7562, 0x7564);
    goto state1;
  }
  CYC(0x7562, 0x7564);
  CYC(0x7564, 0x7566); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x7566, 0x7567); ret_effect(gb); return; }
  CYC(0x7566, 0x7567);
  CYC(0x7567, 0x7569); A = 0x04;
  CYC(0x7569, 0x756c); dimitriFunc_756d_hook(gb);
  return;

state1:
  CYC(0x756c, 0x756d); alu_xor(gb, A);
  dimitriFunc_756d_hook(gb);
}

void dimitriFunc_756d_hook(GB *gb) {
  CYC(0x756d, 0x756e); H = D;
  CYC(0x756e, 0x7570); L = 0x38;
  CYC(0x7570, 0x7571); mem_wr(gb, HL, A);
  CYC(0x7571, 0x7573); L = 0x04;
  CYC(0x7573, 0x7575); A = 0x01;
  CYC(0x7575, 0x7576); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7576, 0x7578); mem_wr(gb, HL, 0x00);
  CYC(0x7578, 0x757a); C = 0x1c;
  CYC(0x757a, 0x757d); companionSetAnimation_hook(gb);
}

void dimitriState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x757d, companionCheckMountingComplete, 0x48c1, 0x7580);
  if (!(F & FZ)) { CYCT(0x7580, 0x7581); ret_effect(gb); return; }
  CYC(0x7580, 0x7581);
  CALL_C(0x7581, companionFinalizeMounting_hook, 0x479e, 0x7584);
  CYC(0x7584, 0x7586); C = 0x00;
  CYC(0x7586, 0x7589); companionSetAnimation_hook(gb);
}

void dimitriState4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7589, companionDragToCenterOfHole_hook, 0x46a0, 0x758c);
  if (!(F & FZ)) { CYCT(0x758c, 0x758d); ret_effect(gb); return; }
  CYC(0x758c, 0x758d);
  CALL_C(0x758d, companionDecCounter1_hook, 0x49a2, 0x7590);
  if (!(F & FZ)) {
    CYCT(0x7590, 0x7592);
    goto animate;
  }
  CYC(0x7590, 0x7592);
  CYC(0x7592, 0x7593); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7593, 0x7595); A = 0x65;
  CALL_C(0x7595, playSound_b00_hook, 0x0c98, 0x7598);
  CYC(0x7598, 0x759a); A = 0x25;
  CYC(0x759a, 0x759d); specialObjectSetAnimation_hook(gb);
  return;

animate:
  CALL_C(0x759d, companionAnimateDrowningOrFallingThenRespawn_hook, 0x4966, 0x75a0);
  if (!(F & FC)) { CYCT(0x75a0, 0x75a1); ret_effect(gb); return; }
  CYC(0x75a0, 0x75a1);
  CYC(0x75a1, 0x75a3); C = 0x00;
  CYC(0x75a3, 0x75a6); companionUpdateDirectionAndSetAnimation_hook(gb);
}

void dimitriState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75a6, 0x75a8); C = 0x40;
  CALL_C(0x75a8, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x75ab);
  if (!(F & FZ)) { CYCT(0x75ab, 0x75ac); ret_effect(gb); return; }
  CYC(0x75ab, 0x75ac);
  CYC(0x75ac, 0x75af); A = W8(wForceCompanionDismount);
  CYC(0x75af, 0x75b0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x75b0, 0x75b2);
    goto check_dismount;
  }
  CYC(0x75b0, 0x75b2);
  CYC(0x75b2, 0x75b5); A = W8(wGameKeysJustPressed);
  CYC(0x75b5, 0x75b7); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x75b7, 0x75b9);
    dimitriGotoEatingState_hook(gb);
    return;
  }
  CYC(0x75b7, 0x75b9);
  CYC(0x75b9, 0x75bb); alu_bit(gb, 1, A);

check_dismount:
  if (!(F & FZ)) {
    CYCT(0x75bb, 0x75be);
    companionGotoDismountState_hook(gb);
    return;
  }
  CYC(0x75bb, 0x75be);
  CYC(0x75be, 0x75c1); A = W8(wLinkAngle);
  CYC(0x75c1, 0x75c3); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x75c3, 0x75c5);
    goto check_hazards;
  }
  CYC(0x75c3, 0x75c5);
  CYC(0x75c5, 0x75c8); SET_HL(w1Companion_angle);
  CYC(0x75c8, 0x75c9); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x75c9, 0x75ca); mem_wr(gb, HL, A);
  CYC(0x75ca, 0x75cc); C = 0x00;
  if (!(F & FZ)) {
    CYCT(0x75cc, 0x75cf);
    companionUpdateDirectionAndAnimate_hook(gb);
    return;
  }
  CYC(0x75cc, 0x75cf);
  CALL_C(0x75cf, companionCheckHopDownCliff_hook, 0x473c, 0x75d2);
  if (F & FZ) { CYCT(0x75d2, 0x75d3); ret_effect(gb); return; }
  CYC(0x75d2, 0x75d3);
  dimitriUpdateMovement_hook(gb);
  return;

check_hazards:
  CALL_C(0x75f0, companionCheckHazards_hook, 0x45ec, 0x75f3);
  CYC(0x75f3, 0x75f4); H = D;
  if (!(F & FC)) {
    CYCT(0x75f4, 0x75f6);
    goto not_in_water;
  }
  CYC(0x75f4, 0x75f6);
  CYC(0x75f6, 0x75f8); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x75f8, 0x75f9); ret_effect(gb); return; }
  CYC(0x75f8, 0x75f9);
  CYC(0x75f9, 0x75fb); L = 0x04;
  CYC(0x75fb, 0x75fd); mem_wr(gb, HL, 0x05);
  CYC(0x75fd, 0x7600); A = W8(wLinkForceState);
  CYC(0x7600, 0x7602); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7602, 0x7604);
    goto waterfall;
  }
  CYC(0x7602, 0x7604);
  CYC(0x7604, 0x7605); alu_xor(gb, A);
  CYC(0x7605, 0x7608); W8(wLinkForceState) = A;
  CYC(0x7608, 0x760b); companionGotoHazardHandlingState_hook(gb);
  return;

waterfall:
  CALL_C(0x760b, dimitriAddWaterfallResistance_hook, 0x783d, 0x760e);
  CYC(0x760e, 0x7610); B = 0x04;
  CYC(0x7610, 0x7612);
  goto set_water_status;

not_in_water:
  CYC(0x7612, 0x7614); B = 0x00;

set_water_status:
  CYC(0x7614, 0x7616); L = 0x38;
  CYC(0x7616, 0x7617); A = mem_rd(gb, HL);
  CYC(0x7617, 0x7618); alu_cp(gb, B);
  CYC(0x7618, 0x7619); mem_wr(gb, HL, B);
  CYC(0x7619, 0x761b); C = 0x00;
  if (!(F & FZ)) {
    CYCT(0x761b, 0x761e);
    companionUpdateDirectionAndSetAnimation_hook(gb);
    return;
  }
  CYC(0x761b, 0x761e);
  dimitriState9_hook(gb);
}

void dimitriUpdateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75d3, 0x75d4); H = D;
  CYC(0x75d4, 0x75d6); L = 0x21;
  CYC(0x75d6, 0x75d7); A = mem_rd(gb, HL);
  CYC(0x75d7, 0x75d8); alu_rlca(gb);
  CYC(0x75d8, 0x75da); A = 0x88;
  if (F & FC) {
    CALL_C_CC(0x75da, playSound_b00_hook, 0x0c98, 0x75dd);
  } else {
    CYC(0x75da, 0x75dd);
  }
  CYC(0x75dd, 0x75df); L = 0x38;
  CYC(0x75df, 0x75e0); A = mem_rd(gb, HL);
  CYC(0x75e0, 0x75e1); alu_or(gb, A);
  CYC(0x75e1, 0x75e3); A = 0x1e;
  if (F & FZ) {
    CYCT(0x75e3, 0x75e5);
  } else {
    CYC(0x75e3, 0x75e5);
    CYC(0x75e5, 0x75e7); A = 0x28;
  }
  CYC(0x75e7, 0x75e9); L = 0x10;
  CYC(0x75e9, 0x75ea); mem_wr(gb, HL, A);
  CALL_C(0x75ea, companionUpdateMovement_hook, 0x446b, 0x75ed);
  CALL_C(0x75ed, specialObjectAnimate_hook, 0x2aef, 0x75f0);
  CALL_C(0x75f0, companionCheckHazards_hook, 0x45ec, 0x75f3);
  CYC(0x75f3, 0x75f4); H = D;
  if (!(F & FC)) {
    CYCT(0x75f4, 0x75f6);
    goto not_in_water;
  }
  CYC(0x75f4, 0x75f6);
  CYC(0x75f6, 0x75f8); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x75f8, 0x75f9); ret_effect(gb); return; }
  CYC(0x75f8, 0x75f9);
  CYC(0x75f9, 0x75fb); L = 0x04;
  CYC(0x75fb, 0x75fd); mem_wr(gb, HL, 0x05);
  CYC(0x75fd, 0x7600); A = W8(wLinkForceState);
  CYC(0x7600, 0x7602); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7602, 0x7604);
    goto waterfall;
  }
  CYC(0x7602, 0x7604);
  CYC(0x7604, 0x7605); alu_xor(gb, A);
  CYC(0x7605, 0x7608); W8(wLinkForceState) = A;
  CYC(0x7608, 0x760b); companionGotoHazardHandlingState_hook(gb);
  return;

waterfall:
  CALL_C(0x760b, dimitriAddWaterfallResistance_hook, 0x783d, 0x760e);
  CYC(0x760e, 0x7610); B = 0x04;
  CYC(0x7610, 0x7612);
  goto set_water_status;

not_in_water:
  CYC(0x7612, 0x7614); B = 0x00;

set_water_status:
  CYC(0x7614, 0x7616); L = 0x38;
  CYC(0x7616, 0x7617); A = mem_rd(gb, HL);
  CYC(0x7617, 0x7618); alu_cp(gb, B);
  CYC(0x7618, 0x7619); mem_wr(gb, HL, B);
  CYC(0x7619, 0x761b); C = 0x00;
  if (!(F & FZ)) {
    CYCT(0x761b, 0x761e);
    companionUpdateDirectionAndSetAnimation_hook(gb);
    return;
  }
  CYC(0x761b, 0x761e);
  dimitriState9_hook(gb);
}

void dimitriState9_hook(GB *gb) {
  CYC(0x761e, 0x761f); ret_effect(gb);
}

void dimitriGotoEatingState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x761f, 0x7620); H = D;
  CYC(0x7620, 0x7622); L = 0x04;
  CYC(0x7622, 0x7624); A = 0x08;
  CYC(0x7624, 0x7625); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7625, 0x7626); alu_xor(gb, A);
  CYC(0x7626, 0x7627); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7627, 0x7628); mem_wr(gb, HL, A);
  CYC(0x7628, 0x762a); L = 0x35;
  CYC(0x762a, 0x762b); mem_wr(gb, HL, A);
  CYC(0x762b, 0x762d); L = 0x08;
  CYC(0x762d, 0x762e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x762e, 0x7630); A = alu_swap(gb, A);
  CYC(0x7630, 0x7631); alu_rrca(gb);
  CYC(0x7631, 0x7632); mem_wr(gb, HL, A);
  CYC(0x7632, 0x7634); A = 0x01;
  CYC(0x7634, 0x7637); W8(wLinkInAir) = A;
  CYC(0x7637, 0x7639); L = 0x10;
  CYC(0x7639, 0x763b); mem_wr(gb, HL, 0x1e);
  CYC(0x763b, 0x763d); C = 0x08;
  CALL_C(0x763d, companionSetAnimation_hook, 0x458e, 0x7640);
  CYC(0x7640, 0x7643); SET_BC(0x2b00);
  CALL_C(0x7643, companionCreateWeaponItem_hook, 0x455b, 0x7646);
  CYC(0x7646, 0x7648); A = 0xc4;
  CYC(0x7648, 0x764b); playSound_b00_hook(gb);
}

void dimitriState6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x764b, 0x764d); E = 0x05;
  CYC(0x764d, 0x764e); A = mem_rd(gb, DE);
  CYC(0x764e, 0x764f); push_effect(gb, 0x764f);
  switch (dimitri_jump_table(gb)) {
    case 0x7655: goto substate0;
    case 0x7660: goto substate1;
    case 0x7668: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7655, 0x7657); A = 0x01;
  CYC(0x7657, 0x7658); mem_wr(gb, DE, A);
  CALL_C(0x7658, companionDismountAndSavePosition_hook, 0x460c, 0x765b);
  CYC(0x765b, 0x765d); C = 0x1c;
  CYC(0x765d, 0x7660); companionSetAnimation_hook(gb);
  return;

substate1:
  CYC(0x7660, 0x7663); A = W8(wLinkInAir);
  CYC(0x7663, 0x7664); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7664, 0x7665); ret_effect(gb); return; }
  CYC(0x7664, 0x7665);
  CYC(0x7665, 0x7668); itemIncSubstate_hook(gb);
  return;

substate2:
  CALL_C(0x7668, dimitriCheckAddToGrabbableObjectBuffer_hook, 0x77d8, 0x766b);
  dimitriGotoState1IfLinkFarAway_hook(gb);
}

void dimitriGotoState1IfLinkFarAway_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x766b, 0x766d); C = 0x09;
  CALL_C(0x766d, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x7670);
  if (F & FC) { CYCT(0x7670, 0x7671); ret_effect(gb); return; }
  CYC(0x7670, 0x7671);
  dimitriGotoState1_hook(gb);
}

void dimitriGotoState1_hook(GB *gb) {
  CYC(0x7671, 0x7673); E = 0x04;
  CYC(0x7673, 0x7675); A = 0x01;
  CYC(0x7675, 0x7676); mem_wr(gb, DE, A);
  CYC(0x7676, 0x7677); E = alu_inc8(gb, E);
  CYC(0x7677, 0x7678); alu_xor(gb, A);
  CYC(0x7678, 0x7679); mem_wr(gb, DE, A);
  CYC(0x7679, 0x767b); E = 0x3b;
  CYC(0x767b, 0x767c); mem_wr(gb, DE, A);
  CYC(0x767c, 0x767d); ret_effect(gb);
}

void dimitriState7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x767d, companionDecCounter1ToJumpDownCliff_hook, 0x493f, 0x7680);
  if (F & FC) { CYCT(0x7680, 0x7681); ret_effect(gb); return; }
  CYC(0x7680, 0x7681);
  CALL_C(0x7681, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x7684);
  CALL_C(0x7684, specialObjectCheckMovingAwayFromWall_hook, 0x4500, 0x7687);
  CYC(0x7687, 0x7689); L = 0x07;
  if (F & FZ) {
    CYCT(0x7689, 0x768b);
    goto stopped;
  }
  CYC(0x7689, 0x768b);
  CYC(0x768b, 0x768c); mem_wr(gb, HL, A);
  CYC(0x768c, 0x768d); ret_effect(gb);
  return;

stopped:
  CYC(0x768d, 0x768e); A = mem_rd(gb, HL);
  CYC(0x768e, 0x768f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x768f, 0x7690); ret_effect(gb); return; }
  CYC(0x768f, 0x7690);
  CYC(0x7690, 0x7693); dimitriLandOnGroundAndGotoState5_hook(gb);
}

void dimitriState8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7693, 0x7695); E = 0x05;
  CYC(0x7695, 0x7696); A = mem_rd(gb, DE);
  CYC(0x7696, 0x7697); push_effect(gb, 0x7697);
  switch (dimitri_jump_table(gb)) {
    case 0x769d: goto substate0;
    case 0x76bd: goto substate1;
    case 0x76df: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x769d, specialObjectAnimate_hook, 0x2aef, 0x76a0);
  CALL_C(0x76a0, objectApplySpeed_hook, 0x201d, 0x76a3);
  CYC(0x76a3, 0x76a5); E = 0x21;
  CYC(0x76a5, 0x76a6); A = mem_rd(gb, DE);
  CYC(0x76a6, 0x76a7); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x76a7, 0x76a8); ret_effect(gb); return; }
  CYC(0x76a7, 0x76a8);
  CALL_C(0x76a8, itemIncSubstate_hook, 0x23ef, 0x76ab);
  CYC(0x76ab, 0x76ad); L = 0x08;
  CYC(0x76ad, 0x76ae); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76ae, 0x76b0); alu_xor(gb, 0x02);
  CYC(0x76b0, 0x76b2); A = alu_swap(gb, A);
  CYC(0x76b2, 0x76b3); alu_rrca(gb);
  CYC(0x76b3, 0x76b4); mem_wr(gb, HL, A);
  CYC(0x76b4, 0x76b6); L = 0x06;
  CYC(0x76b6, 0x76b8); mem_wr(gb, HL, 0x0c);
  CYC(0x76b8, 0x76ba); C = 0x00;
  CYC(0x76ba, 0x76bd); companionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x76bd, specialObjectAnimate_hook, 0x2aef, 0x76c0);
  CALL_C(0x76c0, objectApplySpeed_hook, 0x201d, 0x76c3);
  CALL_C(0x76c3, companionDecCounter1IfNonzero_hook, 0x495e, 0x76c6);
  if (!(F & FZ)) { CYCT(0x76c6, 0x76c7); ret_effect(gb); return; }
  CYC(0x76c6, 0x76c7);
  CYC(0x76c7, 0x76c9); mem_wr(gb, HL, 0x14);
  CYC(0x76c9, 0x76cb); L = 0x08;
  CYC(0x76cb, 0x76cc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76cc, 0x76ce); A = alu_swap(gb, A);
  CYC(0x76ce, 0x76cf); alu_rrca(gb);
  CYC(0x76cf, 0x76d0); mem_wr(gb, HL, A);
  CYC(0x76d0, 0x76d2); L = 0x35;
  CYC(0x76d2, 0x76d3); A = mem_rd(gb, HL);
  CYC(0x76d3, 0x76d4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x76d4, 0x76d7);
    dimitriLandOnGroundAndGotoState5_hook(gb);
    return;
  }
  CYC(0x76d4, 0x76d7);
  CALL_C(0x76d7, itemIncSubstate_hook, 0x23ef, 0x76da);
  CYC(0x76da, 0x76dc); C = 0x10;
  CYC(0x76dc, 0x76df); companionSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(0x76df, specialObjectAnimate_hook, 0x2aef, 0x76e2);
  CALL_C(0x76e2, companionDecCounter1IfNonzero_hook, 0x495e, 0x76e5);
  if (!(F & FZ)) { CYCT(0x76e5, 0x76e6); ret_effect(gb); return; }
  CYC(0x76e5, 0x76e6);
  CYC(0x76e6, 0x76e8); dimitriLandOnGroundAndGotoState5_hook(gb);
}

void dimitriStateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76e8, 0x76ea); C = 0x40;
  CALL_C(0x76ea, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x76ed);
  if (!(F & FZ)) { CYCT(0x76ed, 0x76ee); ret_effect(gb); return; }
  CYC(0x76ed, 0x76ee);
  CALL_C(0x76ee, dimitriUpdateMovement_hook, 0x75d3, 0x76f1);
  CYC(0x76f1, 0x76f2); H = D;
  CYC(0x76f2, 0x76f4); L = 0x38;
  CYC(0x76f4, 0x76f5); A = mem_rd(gb, HL);
  CYC(0x76f5, 0x76f6); alu_or(gb, A);
  CYC(0x76f6, 0x76f8); L = 0x04;
  CYC(0x76f8, 0x76fa); mem_wr(gb, HL, 0x0b);
  if (!(F & FZ)) { CYCT(0x76fa, 0x76fb); ret_effect(gb); return; }
  CYC(0x76fa, 0x76fb);
  CYC(0x76fb, 0x76fd); mem_wr(gb, HL, 0x01);
  CYC(0x76fd, 0x76fe); ret_effect(gb);
}

void dimitriStateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76fe, 0x7700); E = 0x03;
  CYC(0x7700, 0x7701); A = mem_rd(gb, DE);
  CYC(0x7701, 0x7702); push_effect(gb, 0x7702);
  switch (dimitri_jump_table(gb)) {
    case 0x7706: goto parameter0;
    case 0x7715: goto parameter1;
    default: hook_continue(gb, HL, sp0_); return;
  }

parameter0:
  CALL_C(0x7706, companionInitializeOnEnteringScreen_hook, 0x4973, 0x7709);
  CYC(0x7709, 0x770b); mem_wr(gb, HL, 0x3c);
  CYC(0x770b, 0x770d); A = 0xc4;
  CALL_C(0x770d, playSound_b00_hook, 0x0c98, 0x7710);
  CYC(0x7710, 0x7712); C = 0x00;
  CYC(0x7712, 0x7715); companionSetAnimation_hook(gb);
  return;

parameter1:
  CALL_C(0x7715, dimitriUpdateMovement_hook, 0x75d3, 0x7718);
  CYC(0x7718, 0x771a); E = 0x04;
  CYC(0x771a, 0x771c); A = 0x0c;
  CYC(0x771c, 0x771d); mem_wr(gb, DE, A);
  CYC(0x771d, 0x7720); SET_HL(0x785d);
  CALL_C(0x7720, companionRetIfNotFinishedWalkingIn_hook, 0x4982, 0x7723);
  CYC(0x7723, 0x7725); E = 0x03;
  CYC(0x7725, 0x7726); alu_xor(gb, A);
  CYC(0x7726, 0x7727); mem_wr(gb, DE, A);
  CYC(0x7727, 0x772a); dimitriState0_hook(gb);
}

void dimitriStateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x772a, 0x772c); E = 0x3c;
  CYC(0x772c, 0x772d); A = mem_rd(gb, DE);
  CYC(0x772d, 0x772e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x772e, 0x7730);
    goto state5;
  }
  CYC(0x772e, 0x7730);
  CALL_C(0x7730, dimitriGotoState1_hook, 0x7671, 0x7733);
  CYC(0x7733, 0x7734); A = alu_inc8(gb, A);
  CYC(0x7734, 0x7735); mem_wr(gb, DE, A);
  CYC(0x7735, 0x7738); SET_HL(w1Companion_enabled);
  CYC(0x7738, 0x773a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x773a, 0x773c); C = 0x1c;
  CYC(0x773c, 0x773f); companionSetAnimation_hook(gb);
  return;

state5:
  CYC(0x773f, 0x7741); E = 0x04;
  CYC(0x7741, 0x7743); A = 0x05;
  CYC(0x7743, 0x7744); mem_wr(gb, DE, A);
  dimitriLandOnGroundAndGotoState5_hook(gb);
}

void dimitriLandOnGroundAndGotoState5_hook(GB *gb) {
  CYC(0x7744, 0x7745); alu_xor(gb, A);
  CYC(0x7745, 0x7748); W8(wLinkInAir) = A;
  CYC(0x7748, 0x774a); C = 0x00;
  CYC(0x774a, 0x774d); companionSetAnimationAndGotoState5_hook(gb);
}

void dimitriStateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x774d, 0x774f); E = 0x03;
  CYC(0x774f, 0x7750); A = mem_rd(gb, DE);
  CYC(0x7750, 0x7751); push_effect(gb, 0x7751);
  switch (dimitri_jump_table(gb)) {
    case 0x775b: dimitriStateASubstate0_hook(gb); return;
    case 0x778c: dimitriStateASubstate1_hook(gb); return;
    case 0x77ca: dimitriStateASubstate2_hook(gb); return;
    case 0x7799: dimitriStateASubstate3_hook(gb); return;
    case 0x77b1: dimitriStateASubstate4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void dimitriStateASubstate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x775b, 0x775d); E = 0x3d;
  CYC(0x775d, 0x775e); A = mem_rd(gb, DE);
  CYC(0x775e, 0x775f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x775f, 0x7761);
    goto update;
  }
  CYC(0x775f, 0x7761);
  CYC(0x7761, 0x7763); A = 0x81;
  CYC(0x7763, 0x7766); W8(wDisabledObjects) = A;

update:
  CALL_C(0x7766, companionSetAnimationToVar3f_hook, 0x48a3, 0x7769);
  CALL_C(0x7769, companionPreventLinkFromPassing_noExtraChecks_hook, 0x4465, 0x776c);
  CALL_C(0x776c, specialObjectAnimate_hook, 0x2aef, 0x776f);
  CYC(0x776f, 0x7771); E = 0x1a;
  CYC(0x7771, 0x7773); A = 0xc7;
  CYC(0x7773, 0x7774); mem_wr(gb, DE, A);
  CYC(0x7774, 0x7777); A = W8(wDimitriState);
  CYC(0x7777, 0x7779); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x7779, 0x777a); ret_effect(gb); return; }
  CYC(0x7779, 0x777a);
  CYC(0x777a, 0x777c); E = 0x1a;
  CYC(0x777c, 0x777e); A = 0xc1;
  CYC(0x777e, 0x777f); mem_wr(gb, DE, A);
  CYC(0x777f, 0x7781); A = 0xff;
  CYC(0x7781, 0x7784); W8(wStatusBarNeedsRefresh) = A;
  CYC(0x7784, 0x7786); C = 0x1c;
  CALL_C(0x7786, companionSetAnimation_hook, 0x458e, 0x7789);
  CYC(0x7789, 0x778c); companionForceMount_hook(gb);
}

void dimitriStateASubstate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x778c, 0x778e); E = 0x3d;
  CALL_C(0x778e, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x7791);
  CYC(0x7791, 0x7793); C = 0x1c;
  CALL_C(0x7793, companionSetAnimation_hook, 0x458e, 0x7796);
  CYC(0x7796, 0x7799); companionForceMount_hook(gb);
}

void dimitriStateASubstate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7799, 0x779b); E = 0x08;
  CYC(0x779b, 0x779d); A = 0x01;
  CYC(0x779d, 0x779e); mem_wr(gb, DE, A);
  CYC(0x779e, 0x779f); E = alu_inc8(gb, E);
  CYC(0x779f, 0x77a1); A = 0x08;
  CYC(0x77a1, 0x77a2); mem_wr(gb, DE, A);
  CYC(0x77a2, 0x77a4); C = 0x00;
  CALL_C(0x77a4, companionSetAnimation_hook, 0x458e, 0x77a7);
  CYC(0x77a7, 0x77a9); E = 0x03;
  CYC(0x77a9, 0x77ab); A = 0x04;
  CYC(0x77ab, 0x77ac); mem_wr(gb, DE, A);
  CYC(0x77ac, 0x77ae); A = 0xc4;
  CYC(0x77ae, 0x77b1); playSound_b00_hook(gb);
}

void dimitriStateASubstate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77b1, dimitriUpdateMovement_hook, 0x75d3, 0x77b4);
  CYC(0x77b4, 0x77b6); E = 0x04;
  CYC(0x77b6, 0x77b8); A = 0x0a;
  CYC(0x77b8, 0x77b9); mem_wr(gb, DE, A);
  CALL_C(0x77b9, objectCheckWithinScreenBoundary_hook, 0x2184, 0x77bc);
  if (F & FC) { CYCT(0x77bc, 0x77bd); ret_effect(gb); return; }
  CYC(0x77bc, 0x77bd);
  CYC(0x77bd, 0x77be); alu_xor(gb, A);
  CYC(0x77be, 0x77c1); W8(wDisabledObjects) = A;
  CYC(0x77c1, 0x77c4); W8(wMenuDisabled) = A;
  CYC(0x77c4, 0x77c7); W8(wUseSimulatedInput) = A;
  CYC(0x77c7, 0x77ca); itemDelete_hook(gb);
}

void dimitriStateASubstate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x77ca, 0x77cd); A = W8(wLinkObjectIndex);
  CYC(0x77cd, 0x77cf); alu_cp(gb, 0xd1);
  if (!(F & FZ)) { CYCT(0x77cf, 0x77d0); ret_effect(gb); return; }
  CYC(0x77cf, 0x77d0);
  CALL_C(0x77d0, companionDismountAndSavePosition_hook, 0x460c, 0x77d3);
  CYC(0x77d3, 0x77d4); alu_xor(gb, A);
  CYC(0x77d4, 0x77d7); W8(wRememberedCompanionId) = A;
  CYC(0x77d7, 0x77d8); ret_effect(gb);
}

void dimitriCheckAddToGrabbableObjectBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x77d8, 0x77db); A = W8(wLinkClimbingVine);
  CYC(0x77db, 0x77dc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x77dc, 0x77dd); ret_effect(gb); return; }
  CYC(0x77dc, 0x77dd);
  CYC(0x77dd, 0x77e0); A = W8(w1Link_direction);
  CALL_C(0x77e0, dimitriCheckCanBeHeldInDirection_hook, 0x7800, 0x77e3);
  if (F & FZ) { CYCT(0x77e3, 0x77e4); ret_effect(gb); return; }
  CYC(0x77e3, 0x77e4);
  CYC(0x77e4, 0x77e7); SET_HL(w1Link_yh);
  CYC(0x77e7, 0x77e8); B = mem_rd(gb, HL);
  CYC(0x77e8, 0x77ea); L = 0x0d;
  CYC(0x77ea, 0x77eb); C = mem_rd(gb, HL);
  CALL_C(0x77eb, getTileCollisionsAtPosition_hook, 0x14b7, 0x77ee);
  CYC(0x77ee, 0x77f0); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(0x77f0, 0x77f2); goto done; }
  CYC(0x77f0, 0x77f2);
  CYC(0x77f2, 0x77f4); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(0x77f4, 0x77f6); goto done; }
  CYC(0x77f4, 0x77f6);
  CYC(0x77f6, 0x77f8); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(0x77f8, 0x77fa); goto done; }
  CYC(0x77f8, 0x77fa);
  CYC(0x77fa, 0x77fc); alu_cp(gb, 0x19);
  if (!(F & FZ)) {
    CALL_C_CC(0x77fc, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x77ff);
  } else {
    CYC(0x77fc, 0x77ff);
  }

done:
  CYC(0x77ff, 0x7800); ret_effect(gb);
}

void dimitriCheckCanBeHeldInDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7800, 0x7803); push_effect(gb, 0x7803);
  goto check_tile;

after_initial_check:
  if (F & FZ) { CYCT(0x7803, 0x7804); ret_effect(gb); return; }
  CYC(0x7803, 0x7804);
  CYC(0x7804, 0x7807); SET_HL(w1Link_angle);
  CYC(0x7807, 0x7808); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x7808, 0x780a); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x780a, 0x780b); ret_effect(gb); return; }
  CYC(0x780a, 0x780b);
  CYC(0x780b, 0x780d); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(0x780d, 0x780f);
    goto diagonal;
  }
  CYC(0x780d, 0x780f);
  CYC(0x780f, 0x7810); alu_or(gb, D);
  CYC(0x7810, 0x7811); ret_effect(gb);
  return;

diagonal:
  CYC(0x7811, 0x7812); alu_add(gb, A);
  CYC(0x7812, 0x7813); B = A;
  CYC(0x7813, 0x7814); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7814, 0x7816); A = alu_swap(gb, A);
  CYC(0x7816, 0x7818); A = alu_srl(gb, A);
  CYC(0x7818, 0x7819); alu_xor(gb, B);
  CYC(0x7819, 0x781a); alu_add(gb, A);
  CYC(0x781a, 0x781c); A = alu_swap(gb, A);
  CYC(0x781c, 0x781e); alu_and(gb, 0x03);

check_tile:
  CYC(0x781e, 0x7821); SET_HL(0x785d);
  CYC(0x7821, 0x7822); dimitri_add_double_index(gb, 0x7822);
  CYC(0x7822, 0x7823); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7823, 0x7824); C = mem_rd(gb, HL);
  CYC(0x7824, 0x7825); B = A;
  CALL_C(0x7825, objectGetRelativeTile_hook, 0x1435, 0x7828);
  CYC(0x7828, 0x782a); alu_cp(gb, 0xd6);
  if (F & FZ) { CYCT(0x782a, 0x782b); goto return_from_check_tile; }
  CYC(0x782a, 0x782b);
  CYC(0x782b, 0x782d); alu_cp(gb, 0xd5);
  if (F & FZ) { CYCT(0x782d, 0x782e); goto return_from_check_tile; }
  CYC(0x782d, 0x782e);
  CYC(0x782e, 0x7830); alu_cp(gb, 0xd4);
  if (F & FZ) { CYCT(0x7830, 0x7831); goto return_from_check_tile; }
  CYC(0x7830, 0x7831);
  CYC(0x7831, 0x7833); H = 0xce;
  CYC(0x7833, 0x7834); A = mem_rd(gb, HL);
  CYC(0x7834, 0x7836); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(0x7836, 0x7837); goto return_from_check_tile; }
  CYC(0x7836, 0x7837);
  CYC(0x7837, 0x7839); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(0x7839, 0x783a); goto return_from_check_tile; }
  CYC(0x7839, 0x783a);
  CYC(0x783a, 0x783c); alu_cp(gb, 0x19);
  CYC(0x783c, 0x783d);

return_from_check_tile:
  ret_effect(gb);
  if (gb->pc == 0x7803 && gb->sp == sp0_) goto after_initial_check;
}

void dimitriAddWaterfallResistance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x783d, objectGetTileAtPosition_hook, 0x1444, 0x7840);
  CYC(0x7840, 0x7841); H = D;
  CYC(0x7841, 0x7843); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x7843, 0x7845);
    goto move_down;
  }
  CYC(0x7843, 0x7845);
  CYC(0x7845, 0x7847); alu_cp(gb, 0xfe);
  if (!(F & FZ)) { CYCT(0x7847, 0x7848); ret_effect(gb); return; }
  CYC(0x7847, 0x7848);

move_down:
  CYC(0x7848, 0x784a); L = 0x0a;
  CYC(0x784a, 0x784b); A = mem_rd(gb, HL);
  CYC(0x784b, 0x784d); alu_add(gb, 0xc0);
  CYC(0x784d, 0x784e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x784e, 0x784f); A = mem_rd(gb, HL);
  CYC(0x784f, 0x7851); alu_adc(gb, 0x00);
  CYC(0x7851, 0x7852); mem_wr(gb, HL, A);
  CYC(0x7852, 0x7855); A = W8(wScreenTransitionBoundaryY);
  CYC(0x7855, 0x7856); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x7856, 0x7857); ret_effect(gb); return; }
  CYC(0x7856, 0x7857);
  CYC(0x7857, 0x7859); A = 0x82;
  CYC(0x7859, 0x785c); W8(wScreenTransitionDirection) = A;
  CYC(0x785c, 0x785d); ret_effect(gb);
}
