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
    case 0x757d: dimitriState3(gb); return;
    case 0x7589: dimitriState4(gb); return;
    case 0x75a6: dimitriState5(gb); return;
    case 0x764b: dimitriState6(gb); return;
    case 0x767d: dimitriState7(gb); return;
    case 0x7693: dimitriState8(gb); return;
    case 0x761e: dimitriState9(gb); return;
    case 0x774d: dimitriStateA(gb); return;
    case 0x76e8: dimitriStateB(gb); return;
    case 0x76fe: dimitriStateC(gb); return;
    case 0x772a: dimitriStateD(gb); return;
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
  CALL_C(0x7407, dimitriAddWaterfallResistance, 0x783d, 0x740a);
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
    dimitriGotoState1IfLinkFarAway(gb);
    return;
  }
  CYC(0x7427, 0x742a);
  CYC(0x742a, 0x742c); C = 0x09;
  CALL_C(0x742c, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x742f);
  if (!(F & FC)) {
    CYCT(0x742f, 0x7432);
    dimitriCheckAddToGrabbableObjectBuffer(gb);
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
  CALL_C(0x7488, dimitriCheckCanBeHeldInDirection, 0x7800, 0x748b);
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
