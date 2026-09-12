#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

static void bank1_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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
  pop_effect(gb);
}

static void bank1_jump_table_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
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
}

void initiateScreenEdgeWarp_hook(GB *gb);
void initiateWarp_hook(GB *gb);
void warpInitiated_hook(GB *gb);
void noWarpInitiated_hook(GB *gb);
void checkTileWarps_hook(GB *gb);
void checkScreenEdgeWarps_hook(GB *gb);
void checkTileIsWarpTile_hook(GB *gb);

static void screenTransitionEyePuzzle_up(GB *gb);
static void screenTransitionEyePuzzle_rightOrLeft(GB *gb);
static void checkLinkCloseEnoughToWarpTileCenter_func(GB *gb);
static void checkLinkCloseEnoughToWarpTileCenter_tileSolid(GB *gb);

void func_5d41_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d41, refreshLoadedTreeGfx_hook, 0x1613, 0x5d44);
  CYC(0x5d44, 0x5d47); A = mem_rd(gb, wWarpTransition2);
  CYC(0x5d47, 0x5d48); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5d48, 0x5d4b);
    applyWarpTransition2_hook(gb);
    return;
  }
  CYC(0x5d48, 0x5d4b);
  CYC(0x5d4b, 0x5d4e);
  updateAllObjects_hook(gb);
}

void cutscene16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5de4, updateMenus_hook, 0x1a51, 0x5de7);
  if (!(F & FZ)) {
    CYCT(0x5de7, 0x5de8); ret_effect(gb);
    return;
  }
  CYC(0x5de7, 0x5de8);
  CYC(0x5de8, 0x5deb); SET_HL(wWarpTransition2);
  CYC(0x5deb, 0x5dec); A = mem_rd(gb, HL);
  CYC(0x5dec, 0x5dee); mem_wr(gb, HL, 0x00);
  CYC(0x5dee, 0x5def); A = alu_inc8(gb, A);
  CYC(0x5def, 0x5df1); A = 0x03;
  if (!(F & FZ)) {
    CYCT(0x5df1, 0x5df3);
  } else {
    CYC(0x5df1, 0x5df3);
    CALL_C(0x5df3, updateAllObjects_hook, 0x345b, 0x5df6);
    CYC(0x5df6, 0x5df8); A = 0x01;
  }
  CYC(0x5df8, 0x5dfb); mem_wr(gb, 0xc2ef, A);
  CYC(0x5dfb, 0x5dfc); alu_xor(gb, A);
  CYC(0x5dfc, 0x5dff); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5dff, 0x5e02); mem_wr(gb, wLinkCanPassNpcs, A);
  CYC(0x5e02, 0x5e05); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(0x5e05, 0x5e06); ret_effect(gb);
}

void triggerFadeoutTransition_hook(GB *gb) {
  CYC(0x5e06, 0x5e08); A = 0x05;
  CYC(0x5e08, 0x5e0b); mem_wr(gb, 0xc2ef, A);
  CYC(0x5e0b, 0x5e0e);
  fadeoutToWhite_hook(gb);
}

void applyWarpTransition2_hook(GB *gb) {
  CYC(0x5e0e, 0x5e11); SET_HL(wWarpTransition2);
  CYC(0x5e11, 0x5e12); A = mem_rd(gb, HL);
  CYC(0x5e12, 0x5e13); B = A;
  CYC(0x5e13, 0x5e15); mem_wr(gb, HL, 0x00);
  CYC(0x5e15, 0x5e17); alu_and(gb, 0x0f);
  CYC(0x5e17, 0x5e19); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(0x5e19, 0x5e1b);
  } else {
    CYC(0x5e19, 0x5e1b);
    CYC(0x5e1b, 0x5e1d); A = 0x01;
    CYC(0x5e1d, 0x5e20); mem_wr(gb, 0xc2ee, A);
    CYC(0x5e20, 0x5e21); alu_xor(gb, A);
    CYC(0x5e21, 0x5e24); mem_wr(gb, 0xc2ef, A);
    CYC(0x5e24, 0x5e25); ret_effect(gb);
    return;
  }
  CYC(0x5e25, 0x5e28); A = mem_rd(gb, wActiveRoom);
  CYC(0x5e28, 0x5e2a); alu_cp(gb, 0xd1);
  if (!(F & FZ)) {
    CYCT(0x5e2a, 0x5e2c);
  } else {
    CYC(0x5e2a, 0x5e2c);
    CYC(0x5e2c, 0x5e2d); B = alu_inc8(gb, B);
  }
  CYC(0x5e2d, 0x5e2e); A = B;
  CYC(0x5e2e, 0x5e30); alu_and(gb, 0x0f);
  CYC(0x5e30, 0x5e33); mem_wr(gb, 0xc2ef, A);
  CYC(0x5e33, 0x5e35); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(0x5e35, 0x5e38);
    fadeoutToWhite_hook(gb);
    return;
  }
  CYC(0x5e35, 0x5e38);
  CYC(0x5e38, 0x5e3a); A = 0x04;
  CYC(0x5e3a, 0x5e3d);
  fadeoutToWhiteWithDelay_hook(gb);
}

void setCutsceneIndexIfCutsceneTriggerSet_hook(GB *gb) {
  CYC(0x5e3d, 0x5e40); A = mem_rd(gb, wCutsceneTrigger);
  CYC(0x5e40, 0x5e42); alu_and(gb, 0x7f);
  CYC(0x5e42, 0x5e45); mem_wr(gb, 0xc2ef, A);
  CYC(0x5e45, 0x5e46); alu_xor(gb, A);
  CYC(0x5e46, 0x5e49); mem_wr(gb, wCutsceneTrigger, A);
  CYC(0x5e49, 0x5e4c); mem_wr(gb, wCutsceneState, A);
  CYC(0x5e4c, 0x5e4d); ret_effect(gb);
}

void checkDisplayEraOrSeasonInfo_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e7d, 0x5e7f); A = 0x16;
  CALL_C(0x5e7f, checkGlobalFlag_hook, 0x31f3, 0x5e82);
  if (F & FZ) {
    CYCT(0x5e82, 0x5e84);
  } else {
    CYC(0x5e82, 0x5e84);
    CYC(0x5e84, 0x5e86); A = 0x16;
    CYC(0x5e86, 0x5e89);
    unsetGlobalFlag_hook(gb);
    return;
  }
  CYC(0x5e89, 0x5e8c); A = mem_rd(gb, wSentBackByStrangeForce);
  CYC(0x5e8c, 0x5e8d); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x5e8d, 0x5e8e); ret_effect(gb);
    return;
  }
  CYC(0x5e8d, 0x5e8e);
  CYC(0x5e8e, 0x5e91); A = mem_rd(gb, wTilesetFlags);
  CYC(0x5e91, 0x5e93); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(0x5e93, 0x5e94); ret_effect(gb);
    return;
  }
  CYC(0x5e93, 0x5e94);
  CYC(0x5e94, 0x5e96); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(0x5e96, 0x5e97); ret_effect(gb);
    return;
  }
  CYC(0x5e96, 0x5e97);
  CALL_C(0x5e97, getFreeInteractionSlot_hook, 0x3aef, 0x5e9a);
  if (!(F & FZ)) {
    CYCT(0x5e9a, 0x5e9b); ret_effect(gb);
    return;
  }
  CYC(0x5e9a, 0x5e9b);
  CYC(0x5e9b, 0x5e9d); mem_wr(gb, HL, 0xe0);
  CYC(0x5e9d, 0x5e9e); ret_effect(gb);
}

void updateGrassAnimationModifier_hook(GB *gb) {
  CYC(0x5e9e, 0x5ea0); A = 0x00;
  CYC(0x5ea0, 0x5ea3); mem_wr(gb, wGrassAnimationModifier, A);
  CYC(0x5ea3, 0x5ea4); ret_effect(gb);
}

void loadDeathRespawnBufferPreset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ea4, 0x5ea5); push_effect(gb, DE);
  CYC(0x5ea5, 0x5ea6); A = C;
  CALL_C(0x5ea6, multiplyABy8_hook, 0x01b7, 0x5ea9);
  CYC(0x5ea9, 0x5eac); SET_HL(0x5ebd);
  CYC(0x5eac, 0x5ead); alu_add_hl(gb, BC);
  CYC(0x5ead, 0x5eb0); SET_DE(wDeathRespawnBuffer - 1);
  CYC(0x5eb0, 0x5eb1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5eb1, 0x5eb2); B = A;
loadDeathRespawnBufferPreset_loop:
  CYC(0x5eb2, 0x5eb3); SET_DE(DE + 1);
  CYC(0x5eb3, 0x5eb4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5eb4, 0x5eb6); B = alu_sla(gb, B);
  if (!(F & FC)) {
    CYCT(0x5eb6, 0x5eb8);
  } else {
    CYC(0x5eb6, 0x5eb8);
    CYC(0x5eb8, 0x5eb9); mem_wr(gb, DE, A);
  }
  if (!(F & FZ)) {
    CYCT(0x5eb9, 0x5ebb);
    goto loadDeathRespawnBufferPreset_loop;
  }
  CYC(0x5eb9, 0x5ebb);
  CYC(0x5ebb, 0x5ebc); SET_DE(pop_effect(gb));
  CYC(0x5ebc, 0x5ebd); ret_effect(gb);
}

void checkRoomPack_hook(GB *gb) {
  CYC(0x5edd, 0x5ee0); A = W8(wActiveGroup);
  CYC(0x5ee0, 0x5ee2); alu_cp(gb, 0x02);
  if (F & FC) {
    CYCT(0x5ee2, 0x5ee4);
  } else {
    CYC(0x5ee2, 0x5ee4);
    CYC(0x5ee4, 0x5ee5); alu_xor(gb, A);
    CYC(0x5ee5, 0x5ee6); ret_effect(gb);
    return;
  }
  CYC(0x5ee6, 0x5ee9); A = W8(wRoomPack);
  CYC(0x5ee9, 0x5eeb); alu_and(gb, 0x7f);
  CYC(0x5eeb, 0x5eec); C = A;
  CYC(0x5eec, 0x5eef); A = W8(wLoadingRoomPack);
  CYC(0x5eef, 0x5ef0); B = A;
  CYC(0x5ef0, 0x5ef2); alu_and(gb, 0x7f);
  CYC(0x5ef2, 0x5ef3); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x5ef3, 0x5ef4); ret_effect(gb);
    return;
  }
  CYC(0x5ef3, 0x5ef4);
  CYC(0x5ef4, 0x5ef7); A = W8(wRoomPack);
  CYC(0x5ef7, 0x5ef8); C = A;
  CYC(0x5ef8, 0x5ef9); A = B;
  CYC(0x5ef9, 0x5efc); W8(wRoomPack) = A;
  CYC(0x5efc, 0x5efd); alu_or(gb, C);
  CYC(0x5efd, 0x5eff); alu_bit(gb, 7, A);
  CYC(0x5eff, 0x5f00); ret_effect(gb);
}

void calculateRoomEdge_hook(GB *gb) {
  CYC(0x5f00, 0x5f03); SET_BC(0x80a0);
  CYC(0x5f03, 0x5f06); A = W8(wRoomIsLarge);
  CYC(0x5f06, 0x5f07); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5f07, 0x5f09);
  } else {
    CYC(0x5f07, 0x5f09);
    CYC(0x5f09, 0x5f0c); SET_BC(0xc0f0);
  }
  CYC(0x5f0c, 0x5f0f); SET_HL(wRoomEdgeY);
  CYC(0x5f0f, 0x5f10); mem_wr(gb, HL, B);
  CYC(0x5f10, 0x5f11); L = alu_inc8(gb, L);
  CYC(0x5f11, 0x5f12); mem_wr(gb, HL, C);
  CYC(0x5f12, 0x5f13); ret_effect(gb);
}

void updateActiveRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f13, 0x5f16); A = W8(wDungeonIndex);
  CYC(0x5f16, 0x5f17); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5f17, 0x5f19);
    CYC(0x5f26, 0x5f29); A = W8(wScreenTransitionDirection);
    CYC(0x5f29, 0x5f2c); SET_HL(0x5f41);
    CYC(0x5f2c, 0x5f2d); bank1_add_a_to_hl_from_rst(gb, 0x5f2d);
    CYC(0x5f2d, 0x5f30); A = W8(wDungeonMapPosition);
    CYC(0x5f30, 0x5f31); alu_add(gb, mem_rd(gb, HL));
    CYC(0x5f31, 0x5f34); W8(wDungeonMapPosition) = A;
    CALL_C(0x5f34, getActiveRoomFromDungeonMapPosition_hook, 0x2e12, 0x5f37);
  } else {
    CYC(0x5f17, 0x5f19);
    CYC(0x5f19, 0x5f1c); A = W8(wScreenTransitionDirection);
    CYC(0x5f1c, 0x5f1f); SET_HL(0x5f3d);
    CYC(0x5f1f, 0x5f20); bank1_add_a_to_hl_from_rst(gb, 0x5f20);
    CYC(0x5f20, 0x5f23); A = W8(wActiveRoom);
    CYC(0x5f23, 0x5f24); alu_add(gb, mem_rd(gb, HL));
    CYC(0x5f24, 0x5f26);
  }
  CYC(0x5f37, 0x5f3a); W8(wActiveRoom) = A;
  CYC(0x5f3a, 0x5f3d); setVisitedRoomFlag_hook(gb);
}

void getNextActiveRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f45, 0x5f48); A = W8(wScrollMode);
  CYC(0x5f48, 0x5f4a); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(0x5f4a, 0x5f4b); ret_effect(gb);
    return;
  }
  CYC(0x5f4a, 0x5f4b);
  CYC(0x5f4b, 0x5f4e); A = W8(wActiveRoom);
  CYC(0x5f4e, 0x5f51); SET_HL(0x5f6f);
  CALL_C(0x5f51, findRoomSpecificData_hook, 0x1dfe, 0x5f54);
  if (!(F & FC)) {
    CYCT(0x5f54, 0x5f56);
    screenTransitionStandard_hook(gb);
    return;
  }
  CYC(0x5f54, 0x5f56);
  CYC(0x5f56, 0x5f57); bank1_jump_table_from_rst(gb, 0x5f57);
  hook_handoff(gb, HL);
}

void screenTransitionStandard_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f5f, clearEyePuzzleVars_hook, 0x5f67, 0x5f62);
  CALL_C(0x5f62, updateActiveRoom_hook, 0x5f13, 0x5f65);
  CYC(0x5f65, 0x5f66); alu_scf(gb);
  CYC(0x5f66, 0x5f67); ret_effect(gb);
}

void clearEyePuzzleVars_hook(GB *gb) {
  CYC(0x5f67, 0x5f68); alu_xor(gb, A);
  CYC(0x5f68, 0x5f6b); W8(wLostWoodsTransitionCounter1) = A;
  CYC(0x5f6b, 0x5f6e); W8(wLostWoodsTransitionCounter2) = A;
  CYC(0x5f6e, 0x5f6f); ret_effect(gb);
}

void screenTransitionForestScrambler_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f96, 0x5f98); A = 0x2b;
  CALL_C(0x5f98, checkGlobalFlag_hook, 0x31f3, 0x5f9b);
  if (!(F & FZ)) {
    CYCT(0x5f9b, 0x5f9e);
    screenTransitionStandard_hook(gb);
    return;
  }
  CYC(0x5f9b, 0x5f9e);
  CYC(0x5f9e, 0x5fa1); A = W8(wActiveRoom);
  CYC(0x5fa1, 0x5fa3); alu_sub(gb, 0x70);
  CYC(0x5fa3, 0x5fa4); B = A;
  CYC(0x5fa4, 0x5fa6); alu_and(gb, 0xf0);
  CYC(0x5fa6, 0x5fa8); alu_swap_a(gb);
  CYC(0x5fa8, 0x5fa9); C = A;
  CYC(0x5fa9, 0x5faa); alu_add(gb, A);
  CYC(0x5faa, 0x5fab); alu_add(gb, C);
  CYC(0x5fab, 0x5fac); C = A;
  CYC(0x5fac, 0x5fad); A = B;
  CYC(0x5fad, 0x5faf); alu_and(gb, 0x0f);
  CYC(0x5faf, 0x5fb0); alu_add(gb, C);
  CYC(0x5fb0, 0x5fb1); alu_add(gb, A);
  CYC(0x5fb1, 0x5fb2); alu_add(gb, A);
  CYC(0x5fb2, 0x5fb3); B = A;
  CYC(0x5fb3, 0x5fb6); A = W8(wScreenTransitionDirection);
  CYC(0x5fb6, 0x5fb8); alu_and(gb, 0x03);
  CYC(0x5fb8, 0x5fb9); alu_add(gb, B);
  CYC(0x5fb9, 0x5fbc); SET_HL(0x5fc7);
  CYC(0x5fbc, 0x5fbd); bank1_add_a_to_hl_from_rst(gb, 0x5fbd);
  CYC(0x5fbd, 0x5fbe); A = mem_rd(gb, HL);
  CYC(0x5fbe, 0x5fbf); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5fbf, 0x5fc2);
    screenTransitionStandard_hook(gb);
    return;
  }
  CYC(0x5fbf, 0x5fc2);
  CYC(0x5fc2, 0x5fc5); W8(wActiveRoom) = A;
  CYC(0x5fc5, 0x5fc6); alu_scf(gb);
  CYC(0x5fc6, 0x5fc7); ret_effect(gb);
}

void screenTransitionEyePuzzle_hook(GB *gb) {
  uint16_t target;
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5feb, 0x5fee); A = W8(wScreenTransitionDirection);
  CYC(0x5fee, 0x5ff0); alu_and(gb, 0x03);
  CYC(0x5ff0, 0x5ff1); B = A;
  CYC(0x5ff1, 0x5ff4); A = W8(wEyePuzzleCorrectDirection);
  CYC(0x5ff4, 0x5ff5); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x5ff5, 0x5ff7);
    CYC(0x5ffc, 0x5fff); SET_HL(wEyePuzzleTransitionCounter);
    CYC(0x5fff, 0x6000); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  } else {
    CYC(0x5ff5, 0x5ff7);
    CALL_C(0x5ff7, clearEyePuzzleVars_hook, 0x5f67, 0x5ffa);
    CYC(0x5ffa, 0x5ffc);
  }
  CYC(0x6000, 0x6001); A = B;
  CYC(0x6001, 0x6002); bank1_jump_table_from_rst(gb, 0x6002);
  target = HL;
  switch (target) {
    case 0x5f5f:
      screenTransitionStandard_hook(gb);
      return;
    case 0x600a:
      screenTransitionEyePuzzle_up(gb);
      return;
    case 0x6014:
      screenTransitionEyePuzzle_rightOrLeft(gb);
      return;
    default:
      hook_handoff(gb, target);
      return;
  }
}

static void screenTransitionEyePuzzle_up(GB *gb) {
  CYC(0x600a, 0x600d); A = W8(wEyePuzzleTransitionCounter);
  CYC(0x600d, 0x600f); alu_cp(gb, 0x06);
  if (F & FC) {
    CYCT(0x600f, 0x6011);
    screenTransitionEyePuzzle_rightOrLeft(gb);
    return;
  }
  CYC(0x600f, 0x6011);
  CYC(0x6011, 0x6014);
  screenTransitionStandard_hook(gb);
}

static void screenTransitionEyePuzzle_rightOrLeft(GB *gb) {
  CYC(0x6014, 0x6015); alu_scf(gb);
  CYC(0x6015, 0x6016); ret_effect(gb);
}

void func_60cd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60cd, 0x60d0); A = W8(wLinkObjectIndex);
  CYC(0x60d0, 0x60d1); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x60d1, 0x60d2); ret_effect(gb);
    return;
  }
  CYC(0x60d1, 0x60d2);
  CYC(0x60d2, 0x60d5); A = W8(wScrollMode);
  CYC(0x60d5, 0x60d7); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(0x60d7, 0x60d8); ret_effect(gb);
    return;
  }
  CYC(0x60d7, 0x60d8);
  CYC(0x60d8, 0x60db); A = W8(w1Link_state);
  CYC(0x60db, 0x60dd); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x60dd, 0x60de); ret_effect(gb);
    return;
  }
  CYC(0x60dd, 0x60de);
  CYC(0x60de, 0x60e1); A = W8(wTextIsActive);
  CYC(0x60e1, 0x60e2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x60e2, 0x60e3); ret_effect(gb);
    return;
  }
  CYC(0x60e2, 0x60e3);
  CALL_C(0x60e3, checkScreenEdgeWarps_hook, 0x621a, 0x60e6);
  if (!(F & FC)) {
    CYCT(0x60e6, 0x60e7); ret_effect(gb);
    return;
  }
  CYC(0x60e6, 0x60e7);
  CYC(0x60e7, 0x60e9);
  initiateScreenEdgeWarp_hook(gb);
}

void checkWarpsTopDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6143, checkTileWarps_hook, 0x61a1, 0x6146);
  if (F & FC) {
    CYCT(0x6146, 0x6147); ret_effect(gb);
    return;
  }
  CYC(0x6146, 0x6147);
  CALL_C(0x6147, checkScreenEdgeWarps_hook, 0x621a, 0x614a);
  if (!(F & FC)) {
    CYCT(0x614a, 0x614b); ret_effect(gb);
    return;
  }
  CYC(0x614a, 0x614b);
  CYC(0x614b, 0x614d);
  initiateScreenEdgeWarp_hook(gb);
}

void checkWarpsSidescrolling_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x614d, checkScreenEdgeWarps_hook, 0x621a, 0x6150);
  if (!(F & FC)) {
    CYCT(0x6150, 0x6151); ret_effect(gb);
    return;
  }
  CYC(0x6150, 0x6151);
  CYC(0x6151, 0x6154); A = W8(wWarpTransition);
  CYC(0x6154, 0x6156); alu_or(gb, 0x30);
  CYC(0x6156, 0x6159); W8(wWarpTransition) = A;
  CYC(0x6159, 0x615b);
  initiateWarp_hook(gb);
}

void initiateScreenEdgeWarp_hook(GB *gb) {
  CYC(0x615b, 0x615e); A = W8(wWarpTransition);
  CYC(0x615e, 0x6160); alu_or(gb, 0x10);
  CYC(0x6160, 0x6163); W8(wWarpTransition) = A;
  initiateWarp_hook(gb);
}

void initiateWarp_hook(GB *gb) {
  CYC(0x6163, 0x6165); A = 0x00;
  CYC(0x6165, 0x6168); W8(wScrollMode) = A;
  CYC(0x6168, 0x616a); A = 0x1e;
  CYC(0x616a, 0x616d); W8(wDisabledObjects) = A;
  CYC(0x616d, 0x616f); A = 0x0a;
  CYC(0x616f, 0x6172); W8(wLinkForceState) = A;
  CYC(0x6172, 0x6174);
  warpInitiated_hook(gb);
}

static void checkLinkCloseEnoughToWarpTileCenter_func(GB *gb) {
  CYC(0x618f, 0x6190); A = mem_rd(gb, HL);
  CYC(0x6190, 0x6191); alu_add(gb, B);
  CYC(0x6191, 0x6193); alu_and(gb, 0x0f);
  CYC(0x6193, 0x6195); alu_sub(gb, 0x04);
  CYC(0x6195, 0x6197); alu_cp(gb, 0x0a);
  CYC(0x6197, 0x6198); ret_effect(gb);
}

static void checkLinkCloseEnoughToWarpTileCenter_tileSolid(GB *gb) {
  CYC(0x618d, 0x618f); B = 0x02;
  checkLinkCloseEnoughToWarpTileCenter_func(gb);
}

void checkLinkCloseEnoughToWarpTileCenter_hook(GB *gb) {
  CYC(0x6174, 0x6176); H = 0xd0;
  CYC(0x6176, 0x6178); A = hram_rd(gb, 0x8d);
  CYC(0x6178, 0x6179); C = A;
  CYC(0x6179, 0x617b); B = 0xce;
  CYC(0x617b, 0x617c); A = mem_rd(gb, BC);
  CYC(0x617c, 0x617d); alu_or(gb, A);
  CYC(0x617d, 0x617f); L = 0x0b;
  if (!(F & FZ)) {
    CYCT(0x617f, 0x6181);
    checkLinkCloseEnoughToWarpTileCenter_tileSolid(gb);
    return;
  }
  CYC(0x617f, 0x6181);
  CYC(0x6181, 0x6183); B = 0x04;
  CYC(0x6183, 0x6186); push_effect(gb, 0x6186);
  checkLinkCloseEnoughToWarpTileCenter_func(gb);
  if (!(F & FC)) {
    CYCT(0x6186, 0x6187); ret_effect(gb);
    return;
  }
  CYC(0x6186, 0x6187);
  CYC(0x6187, 0x6189); B = 0x00;
  CYC(0x6189, 0x618b); L = 0x0d;
  CYC(0x618b, 0x618d);
  checkLinkCloseEnoughToWarpTileCenter_func(gb);
}

void warpInitiated_hook(GB *gb) {
  CYC(0x6198, 0x619a); A = 0x01;
  CYC(0x619a, 0x619d); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x619d, 0x619e); alu_scf(gb);
  CYC(0x619e, 0x619f); ret_effect(gb);
}

void noWarpInitiated_hook(GB *gb) {
  CYC(0x619f, 0x61a0); alu_xor(gb, A);
  CYC(0x61a0, 0x61a1); ret_effect(gb);
}

static void check_tile_warps_initiate(GB *gb, uint16_t sp0_) {
  CYC(0x61c7, 0x61ca); SET_HL(0x4629);
  CYC(0x61ca, 0x61cc); E = 0x04;
  CALL_C(0x61cc, interBankCall_hook, 0x008a, 0x61cf);
  CYC(0x61cf, 0x61d2); initiateWarp_hook(gb);
}

static void check_tile_warps_multi_tile_door(GB *gb, uint16_t sp0_) {
  CYC(0x61e2, 0x61e4); A = hram_rd(gb, 0x8d);
  CYC(0x61e4, 0x61e5); C = A;
  CYC(0x61e5, 0x61e7); B = 0xce;
  CYC(0x61e7, 0x61e8); A = mem_rd(gb, BC);
  CYC(0x61e8, 0x61e9); alu_or(gb, A);
  CYC(0x61e9, 0x61eb); B = 0x02;
  if (!(F & FZ)) {
    CYCT(0x61eb, 0x61ed);
  } else {
    CYC(0x61eb, 0x61ed);
    CYC(0x61ed, 0x61ef); B = 0x04;
  }
  CYC(0x61ef, 0x61f2); SET_HL(w1Link_yh);
  CYC(0x61f2, 0x61f3); A = mem_rd(gb, HL);
  CYC(0x61f3, 0x61f4); alu_add(gb, B);
  CYC(0x61f4, 0x61f6); alu_and(gb, 0x0f);
  CYC(0x61f6, 0x61f8); alu_sub(gb, 0x04);
  CYC(0x61f8, 0x61fa); alu_cp(gb, 0x0a);
  if (!(F & FC)) {
    CYCT(0x61fa, 0x61fb); ret_effect(gb);
    return;
  }
  CYC(0x61fa, 0x61fb);
  CYC(0x61fb, 0x61fd);
  check_tile_warps_initiate(gb, sp0_);
}

void checkTileWarps_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61a1, 0x61a4); A = W8(wLinkObjectIndex);
  CYC(0x61a4, 0x61a5); H = A;
  CYC(0x61a5, 0x61a7); L = 0x0f;
  CYC(0x61a7, 0x61a8); A = mem_rd(gb, HL);
  CYC(0x61a8, 0x61a9); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61a9, 0x61aa); ret_effect(gb);
    return;
  }
  CYC(0x61a9, 0x61aa);
  CYC(0x61aa, 0x61ad); A = W8(wMenuDisabled);
  CYC(0x61ad, 0x61ae); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61ae, 0x61b0);
    noWarpInitiated_hook(gb);
    return;
  }
  CYC(0x61ae, 0x61b0);
  CYC(0x61b0, 0x61b2); A = hram_rd(gb, 0x8c);
  CALL_C(0x61b2, checkTileIsWarpTile_hook, 0x6232, 0x61b5);
  if (!(F & FC)) {
    CYCT(0x61b5, 0x61b7);
    noWarpInitiated_hook(gb);
    return;
  }
  CYC(0x61b5, 0x61b7);
  CYC(0x61b7, 0x61ba); A = W8(wLinkGrabState);
  CYC(0x61ba, 0x61bb); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61bb, 0x61bd);
    noWarpInitiated_hook(gb);
    return;
  }
  CYC(0x61bb, 0x61bd);
  CALL_ROM(0x61bd, 0x61d2);
  if (F & FC) {
    CYCT(0x61c0, 0x61c2);
    check_tile_warps_multi_tile_door(gb, sp0_);
    return;
  }
  CYC(0x61c0, 0x61c2);
  CALL_C(0x61c2, checkLinkCloseEnoughToWarpTileCenter_hook, 0x6174, 0x61c5);
  if (!(F & FC)) {
    CYCT(0x61c5, 0x61c7);
    noWarpInitiated_hook(gb);
    return;
  }
  CYC(0x61c5, 0x61c7);
  check_tile_warps_initiate(gb, sp0_);
}

void checkStandingOnDeactivatedWarpTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61fd, 0x61fe); alu_scf(gb);
  CYC(0x61fe, 0x6201); A = W8(wEnteredWarpPosition);
  CYC(0x6201, 0x6202); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x6202, 0x6203); ret_effect(gb);
    return;
  }
  CYC(0x6202, 0x6203);
  CYC(0x6203, 0x6206); A = W8(wEnteredWarpPosition);
  CYC(0x6206, 0x6207); B = A;
  CYC(0x6207, 0x6209); A = hram_rd(gb, 0x8d);
  CYC(0x6209, 0x620a); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x620a, 0x620b); ret_effect(gb);
    return;
  }
  CYC(0x620a, 0x620b);
  CYC(0x620b, 0x620c); B = alu_dec8(gb, B);
  CYC(0x620c, 0x620d); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYC(0x620d, 0x620f);
    CYC(0x620f, 0x6210); alu_scf(gb);
    CYC(0x6210, 0x6211); ret_effect(gb);
    return;
  }
  CYCT(0x620d, 0x620f);
  CYC(0x6211, 0x6213); A = hram_rd(gb, 0x8c);
  CALL_C(0x6213, checkTileIsWarpTile_hook, 0x6232, 0x6216);
  if (!(F & FC)) {
    CYCT(0x6216, 0x6218);
    CYC(0x620f, 0x6210); alu_scf(gb);
    CYC(0x6210, 0x6211); ret_effect(gb);
    return;
  }
  CYC(0x6216, 0x6218);
  CYC(0x6218, 0x6219); alu_xor(gb, A);
  CYC(0x6219, 0x621a); ret_effect(gb);
}

void checkScreenEdgeWarps_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x621a, 0x621c); A = 0xff;
  CYC(0x621c, 0x621f); W8(wTmpcec0) = A;
  CYC(0x621f, 0x6222); SET_HL(0x46c8);
  CYC(0x6222, 0x6224); E = 0x04;
  CALL_C(0x6224, interBankCall_hook, 0x008a, 0x6227);
  CYC(0x6227, 0x622a); A = W8(wTmpcec0);
  CYC(0x622a, 0x622c); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x622c, 0x622f);
    noWarpInitiated_hook(gb);
    return;
  }
  CYC(0x622c, 0x622f);
  CYC(0x622f, 0x6232); warpInitiated_hook(gb);
}

void checkTileIsWarpTile_hook(GB *gb) {
  CYC(0x6232, 0x6235); SET_HL(0x6238);
  CYC(0x6235, 0x6238); lookupCollisionTable_hook(gb);
}
