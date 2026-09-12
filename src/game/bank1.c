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
void func_7b93_hook(GB *gb);
void func_7b93__state0_hook(GB *gb);
void func_7b93__state1_hook(GB *gb);
void func_7b93__state2_hook(GB *gb);
void checkSeedTreeRefillIndex_hook(GB *gb);
void checkSeedTreeRefillIndex__addRoom_hook(GB *gb);
void checkSeedTreeRefillIndex__treeScreen_hook(GB *gb);
void paletteFadeHandler09_hook(GB *gb);
void paletteFadeHandler01_hook(GB *gb);
void paletteFadeHandler00_hook(GB *gb);
void paletteFadeHandler0a_hook(GB *gb);
void paletteFadeHandler02_hook(GB *gb);
void paletteFadeHandler0b_hook(GB *gb);
void paletteFadeHandler03_hook(GB *gb);
void paletteFadeHandler0c_hook(GB *gb);
void paletteFadeHandler04_hook(GB *gb);
void paletteThread_setFadeOffsetAndStop_hook(GB *gb);
void paletteThread_stop_hook(GB *gb);
void paletteThread_refreshPalettesAndStop_hook(GB *gb);
void paletteFadeHandler0d_hook(GB *gb);
void paletteFadeHandler05_hook(GB *gb);
void paletteFadeHandler0e_hook(GB *gb);
void paletteFadeHandler06_hook(GB *gb);
void paletteFadeHandler07_hook(GB *gb);
void paletteFadeHandler08_hook(GB *gb);
void paletteFadeHandler08__seasonsFunc_01_5816_hook(GB *gb);
void paletteFadeHandler08__stop_hook(GB *gb);
void paletteThread_calculateFadingPalettes_hook(GB *gb);
void paletteThread_calculateFadingPalettes__nextColor_hook(GB *gb);
void paletteThread_mixBG234Palettes_hook(GB *gb);
void paletteThread_mixBG567Palettes_hook(GB *gb);
void paletteThread_mixBG567Palettes__nextColor_hook(GB *gb);
void paletteThread_mixBG567Palettes__mixColors_hook(GB *gb);
void paletteThread_mixBG567Palettes__writeToFadingBgPalettes_hook(GB *gb);
void paletteThread_decCounter_hook(GB *gb);

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

void cutscene13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b6e, 0x7b71); SET_HL(0x6103);
  CYC(0x7b71, 0x7b73); E = 0x03;
  CALL_C(0x7b73, interBankCall_hook, 0x008a, 0x7b76);
  CALL_C(0x7b76, refreshLoadedTreeGfx_hook, 0x1613, 0x7b79);
  CYC(0x7b79, 0x7b7c); updateAllObjects_hook(gb);
}

void tilesetLayoutGroup33_hook(GB *gb) {
  cutscene13_hook(gb);
}

void cutscene14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b7c, 0x7b7f); SET_HL(0x6275);
  CYC(0x7b7f, 0x7b81); E = 0x03;
  CALL_C(0x7b81, interBankCall_hook, 0x008a, 0x7b84);
  CALL_C(0x7b84, refreshLoadedTreeGfx_hook, 0x1613, 0x7b87);
  CALL_C(0x7b87, updateAllObjects_hook, 0x345b, 0x7b8a);
  CYC(0x7b8a, 0x7b8d); updateStatusBar_hook(gb);
}

void linkSummonedCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b8d, func_7b93_hook, 0x7b93, 0x7b90);
  CYC(0x7b90, 0x7b93); updateAllObjects_hook(gb);
}

void func_7b93_hook(GB *gb) {
  CYC(0x7b93, 0x7b96); A = mem_rd(gb, 0xc2ef);
  CYC(0x7b96, 0x7b97); bank1_jump_table_from_rst(gb, 0x7b97);
  switch (HL) {
    case 0x7b9d: func_7b93__state0_hook(gb); return;
    case 0x7be4: func_7b93__state1_hook(gb); return;
    case 0x7c01: func_7b93__state2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_7b93__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b9d, 0x7ba0); SET_HL(0xc2ef);
  CYC(0x7ba0, 0x7ba1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x7ba1, disableLcd_hook, 0x02c1, 0x7ba4);
  CALL_C(0x7ba4, clearOam_hook, 0x049f, 0x7ba7);
  CALL_C(0x7ba7, clearMemoryOnScreenReload, 0x49af, 0x7baa);
  CALL_C(0x7baa, loadScreenMusicAndSetRoomPack_hook, 0x341a, 0x7bad);
  CALL_C(0x7bad, loadTilesetData_hook, 0x3889, 0x7bb0);
  CALL_C(0x7bb0, loadTilesetGraphics_hook, 0x3796, 0x7bb3);
  CALL_C(0x7bb3, func_131f_hook, 0x131f, 0x7bb6);
  CALL_C(0x7bb6, loadDungeonLayout_hook, 0x2daa, 0x7bb9);
  CYC(0x7bb9, 0x7bbb); A = 0x01;
  CYC(0x7bbb, 0x7bbe); W8(wScrollMode) = A;
  CALL_C(0x7bbe, calculateRoomEdge_hook, 0x5f00, 0x7bc1);
  CALL_C(0x7bc1, updateLinkLocalRespawnPosition_hook, 0x113a, 0x7bc4);
  CALL_C(0x7bc4, loadCommonGraphics_hook, 0x1a98, 0x7bc7);
  CYC(0x7bc7, 0x7bc9); A = 0x02;
  CALL_C(0x7bc9, fadeinFromWhiteWithDelay_hook, 0x3284, 0x7bcc);
  CYC(0x7bcc, 0x7bce); A = 0x02;
  CALL_C(0x7bce, loadGfxRegisterStateIndex_hook, 0x02ea, 0x7bd1);
  CYC(0x7bd1, 0x7bd3); A = 0x10;
  CYC(0x7bd3, 0x7bd6); W8(wGfxRegs2_LYC) = A;
  CYC(0x7bd6, 0x7bd8); A = 0x02;
  CYC(0x7bd8, 0x7bda); H8(hNextLcdInterruptBehaviour) = A;
  CYC(0x7bda, 0x7bdc); A = 0x95;
  CALL_C(0x7bdc, playSound_b00_hook, 0x0c98, 0x7bdf);
  CYC(0x7bdf, 0x7be1); A = 0xff;
  CYC(0x7be1, 0x7be4); initWaveScrollValues_hook(gb);
}

void func_7b93__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7be4, 0x7be6); A = 0x01;
  CALL_C(0x7be6, loadBigBufferScrollValues_hook, 0x13a5, 0x7be9);
  CYC(0x7be9, 0x7bec); A = W8(wPaletteThread_mode);
  CYC(0x7bec, 0x7bed); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7bed, 0x7bee); ret_effect(gb);
    return;
  }
  CYC(0x7bed, 0x7bee);
  CYC(0x7bee, 0x7bf1); SET_HL(0xc2ef);
  CYC(0x7bf1, 0x7bf2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7bf2, 0x7bf4); A = 0x81;
  CYC(0x7bf4, 0x7bf7); W8(wDisabledObjects) = A;
  CYC(0x7bf7, 0x7bf9); A = 0xff;
  CYC(0x7bf9, 0x7bfc); W8(wGenericCutscene_cbb4) = A;
  CYC(0x7bfc, 0x7bfd); alu_xor(gb, A);
  CYC(0x7bfd, 0x7c00); W8(wGenericCutscene_cbb3) = A;
  CYC(0x7c00, 0x7c01); ret_effect(gb);
}

void func_7b93__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c0b, 0x7c0d); A = 0x01;
  CALL_C(0x7c0d, loadBigBufferScrollValues_hook, 0x13a5, 0x7c10);
  CYC(0x7c10, 0x7c13); SET_HL(wGenericCutscene_cbb4);
  CYC(0x7c13, 0x7c14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7c14, 0x7c15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7c15, 0x7c16); A = mem_rd(gb, HL);
  CALL_C(0x7c16, initWaveScrollValues_hook, 0x1384, 0x7c19);
  CYC(0x7c19, 0x7c1c); A = W8(wGenericCutscene_cbb4);
  CYC(0x7c1c, 0x7c1e); alu_cp(gb, 0x80);
  if (!(F & FC)) {
    CYCT(0x7c1e, 0x7c1f); ret_effect(gb);
    return;
  }
  CYC(0x7c1e, 0x7c1f);
  CYC(0x7c1f, 0x7c22); SET_HL(wGenericCutscene_cbb3);
  CYC(0x7c22, 0x7c23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c23, 0x7c25); A = 0x03;
  CYC(0x7c25, 0x7c28); mem_wr(gb, 0xd000, A);
  CYC(0x7c28, 0x7c2a); A = 0x0a;
  CYC(0x7c2a, 0x7c2d); W8(wLinkForceState) = A;
  CYC(0x7c2d, 0x7c2f); A = 0x0b;
  CYC(0x7c2f, 0x7c32); W8(wWarpTransition) = A;
  CYC(0x7c32, 0x7c33); ret_effect(gb);
}

void func_7b93__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c33, 0x7c35); A = 0x01;
  CALL_C(0x7c35, loadBigBufferScrollValues_hook, 0x13a5, 0x7c38);
  CYC(0x7c38, 0x7c3b); SET_HL(wGenericCutscene_cbb4);
  CYC(0x7c3b, 0x7c3c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x7c3c, 0x7c3e);
  } else {
    CYC(0x7c3c, 0x7c3e);
    CYC(0x7c3e, 0x7c3f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(0x7c3f, 0x7c40); A = mem_rd(gb, HL);
  CALL_C(0x7c40, initWaveScrollValues_hook, 0x1384, 0x7c43);
  CYC(0x7c43, 0x7c46); A = W8(wGenericCutscene_cbb4);
  CYC(0x7c46, 0x7c47); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7c47, 0x7c48); ret_effect(gb);
    return;
  }
  CYC(0x7c47, 0x7c48);
  CYC(0x7c48, 0x7c4b); SET_HL(wGenericCutscene_cbb3);
  CYC(0x7c4b, 0x7c4c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c4c, 0x7c4e); A = 0x03;
  CYC(0x7c4e, 0x7c50); H8(hNextLcdInterruptBehaviour) = A;
  CYC(0x7c50, 0x7c51); ret_effect(gb);
}

void func_7b93__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c51, 0x7c53); A = 0x02;
  CYC(0x7c53, 0x7c56); mem_wr(gb, 0xc2ee, A);
  CYC(0x7c56, 0x7c57); alu_xor(gb, A);
  CYC(0x7c57, 0x7c5a); mem_wr(gb, 0xc2ef, A);
  CYC(0x7c5a, 0x7c5d); W8(wDisabledObjects) = A;
  CYC(0x7c5d, 0x7c5f); A = 0x21;
  CALL_C(0x7c5f, setGlobalFlag_hook, 0x31f9, 0x7c62);
  CYC(0x7c62, 0x7c65); initializeRoom_hook(gb);
}

void func_7b93__state2_hook(GB *gb) {
  CYC(0x7c01, 0x7c04); A = W8(wGenericCutscene_cbb3);
  CYC(0x7c04, 0x7c05); bank1_jump_table_from_rst(gb, 0x7c05);
  switch (HL) {
    case 0x7c0b: func_7b93__substate0_hook(gb); return;
    case 0x7c33: func_7b93__substate1_hook(gb); return;
    case 0x7c51: func_7b93__substate2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void updateSeedTreeRefillData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6016, 0x6019); A = mem_rd(gb, wTilesetFlags);
  CYC(0x6019, 0x601b); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x601b, 0x601c); ret_effect(gb);
    return;
  }
  CYC(0x601b, 0x601c);
  CYC(0x601c, 0x601e); A = 0x02;
  CYC(0x601e, 0x6020); hram_wr(gb, 0x70, A);
  CYC(0x6020, 0x6023); SET_HL(0x6036);
  CYC(0x6023, 0x6025); B = 0x10;
  for (;;) {
    CYC(0x6025, 0x6026); push_effect(gb, BC);
    CYC(0x6026, 0x6027); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6027, 0x6028); C = A;
    CYC(0x6028, 0x6029); A = mem_rd(gb, HL);
    CYC(0x6029, 0x602a); E = A;
    CALL_C(0x602a, checkSeedTreeRefillIndex_hook, 0x6056, 0x602d);
    CYC(0x602d, 0x602e); SET_HL(HL + 1);
    CYC(0x602e, 0x602f); SET_BC(pop_effect(gb));
    CYC(0x602f, 0x6030); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x6030, 0x6032); continue; }
    CYC(0x6030, 0x6032);
    break;
  }
  CYC(0x6032, 0x6033); alu_xor(gb, A);
  CYC(0x6033, 0x6035); hram_wr(gb, 0x70, A);
  CYC(0x6035, 0x6036); ret_effect(gb);
}

void checkSeedTreeRefillIndex__addRoom_hook(GB *gb) {
  CYC(0x608d, 0x608e); A = B;
  CYC(0x608e, 0x608f); mem_wr(gb, DE, A);
  CYC(0x608f, 0x6090); ret_effect(gb);
}

static void checkSeedTreeRefillIndex_finish(GB *gb, uint16_t sp0_) {
  CYC(0x60ab, 0x60ac); SET_DE(pop_effect(gb));
  CYC(0x60ac, 0x60ad); L = E;
  CYC(0x60ad, 0x60ae); H = D;
  CYC(0x60ae, 0x60b0); B = 0x08;
  CALL_C(0x60b0, clearMemory_hook, 0x046f, 0x60b3);
  CYC(0x60b3, 0x60b4); SET_HL(pop_effect(gb));
  CYC(0x60b4, 0x60b5); ret_effect(gb);
}

void checkSeedTreeRefillIndex__treeScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6090, 0x6091); push_effect(gb, HL);
  CYC(0x6091, 0x6092); push_effect(gb, DE);
  CYC(0x6092, 0x6094); C = 0x08;
  for (;;) {
    CYC(0x6094, 0x6095); A = mem_rd(gb, DE);
    CYC(0x6095, 0x6096); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x6096, 0x6098);
      break;
    }
    CYC(0x6096, 0x6098);
    CYC(0x6098, 0x6099); E = alu_inc8(gb, E);
    CYC(0x6099, 0x609a); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x609a, 0x609c); continue; }
    CYC(0x609a, 0x609c);
    CYC(0x609c, 0x609d); alu_or(gb, D);
    break;
  }
  if (F & FZ) {
    CYCT(0x609d, 0x609f);
  } else {
    CYC(0x609d, 0x609f);
    CYC(0x609f, 0x60a1); A = hram_rd(gb, 0x8d);
    CYC(0x60a1, 0x60a2); B = A;
    CYC(0x60a2, 0x60a4); A = 0x10;
    CYC(0x60a4, 0x60a5); alu_sub(gb, B);
    CYC(0x60a5, 0x60a8); SET_HL(wSeedTreeRefilledBitset);
    CALL_C(0x60a8, setFlag_hook, 0x020e, 0x60ab);
  }
  checkSeedTreeRefillIndex_finish(gb, sp0_);
}

void checkSeedTreeRefillIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6056, 0x6057); A = B;
  CYC(0x6057, 0x6059); hram_wr(gb, 0x8d, A);
  CYC(0x6059, 0x605a); A = E;
  CYC(0x605a, 0x605c); E &= (uint8_t)~1;
  CYC(0x605c, 0x605e); alu_and(gb, 0x01);
  CYC(0x605e, 0x605f); B = A;
  CYC(0x605f, 0x6062); A = mem_rd(gb, wActiveGroup);
  CYC(0x6062, 0x6063); alu_cp(gb, B);
  CYC(0x6063, 0x6065); D = 0xd9;
  if (!(F & FZ)) {
    CYCT(0x6065, 0x6067);
  } else {
    CYC(0x6065, 0x6067);
    CYC(0x6067, 0x606a); A = mem_rd(gb, wActiveRoom);
    CYC(0x606a, 0x606b); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(0x606b, 0x606d);
      checkSeedTreeRefillIndex__treeScreen_hook(gb);
      return;
    }
    CYC(0x606b, 0x606d);
  }
  CYC(0x606d, 0x606f); A = hram_rd(gb, 0x8d);
  CYC(0x606f, 0x6070); B = A;
  CYC(0x6070, 0x6072); A = 0x10;
  CYC(0x6072, 0x6073); alu_sub(gb, B);
  CYC(0x6073, 0x6074); push_effect(gb, HL);
  CYC(0x6074, 0x6077); SET_HL(wSeedTreeRefilledBitset);
  CALL_C(0x6077, checkFlag_hook, 0x0205, 0x607a);
  CYC(0x607a, 0x607b); SET_HL(pop_effect(gb));
  if (!(F & FZ)) {
    CYCT(0x607b, 0x607c); ret_effect(gb);
    return;
  }
  CYC(0x607b, 0x607c);
  CYC(0x607c, 0x607f); A = mem_rd(gb, wActiveRoom);
  CYC(0x607f, 0x6080); B = A;
  CYC(0x6080, 0x6082); C = 0x08;
  for (;;) {
    CYC(0x6082, 0x6083); A = mem_rd(gb, DE);
    CYC(0x6083, 0x6084); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x6084, 0x6086);
      checkSeedTreeRefillIndex__addRoom_hook(gb);
      return;
    }
    CYC(0x6084, 0x6086);
    CYC(0x6086, 0x6087); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(0x6087, 0x6088); ret_effect(gb);
      return;
    }
    CYC(0x6087, 0x6088);
    CYC(0x6088, 0x6089); E = alu_inc8(gb, E);
    CYC(0x6089, 0x608a); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x608a, 0x608c); continue; }
    CYC(0x608a, 0x608c);
    CYC(0x608c, 0x608d); ret_effect(gb);
    return;
  }
}

void initializeSeedTreeRefillData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60b5, 0x60b8); SET_HL(wSeedTreeRefilledBitset);
  CYC(0x60b8, 0x60ba); mem_wr(gb, HL, 0xf0);
  CYC(0x60ba, 0x60bb); L = alu_inc8(gb, L);
  CYC(0x60bb, 0x60bd); mem_wr(gb, HL, 0xff);
  CYC(0x60bd, 0x60bf); A = 0x02;
  CYC(0x60bf, 0x60c1); hram_wr(gb, 0x70, A);
  CYC(0x60c1, 0x60c4); SET_HL(0xd900);
  CYC(0x60c4, 0x60c6); B = 0x80;
  CALL_C(0x60c6, clearMemory_hook, 0x046f, 0x60c9);
  CYC(0x60c9, 0x60ca); alu_xor(gb, A);
  CYC(0x60ca, 0x60cc); hram_wr(gb, 0x70, A);
  CYC(0x60cc, 0x60cd); ret_effect(gb);
}

void func_60e9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60e9, 0x60ec); A = mem_rd(gb, wScrollMode);
  CYC(0x60ec, 0x60ed); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x60ed, 0x60ee); ret_effect(gb);
    return;
  }
  CYC(0x60ed, 0x60ee);
  CALL_C(0x60ee, func_60cd_hook, 0x60cd, 0x60f1);
  if (F & FC) {
    CYCT(0x60f1, 0x60f2); ret_effect(gb);
    return;
  }
  CYC(0x60f1, 0x60f2);
  CYC(0x60f2, 0x60f5); A = mem_rd(gb, wLinkInAir);
  CYC(0x60f5, 0x60f7); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(0x60f7, 0x60f8); ret_effect(gb);
    return;
  }
  CYC(0x60f7, 0x60f8);
  CYC(0x60f8, 0x60fb); A = mem_rd(gb, wWarpsDisabled);
  CYC(0x60fb, 0x60fc); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x60fc, 0x60fd); ret_effect(gb);
    return;
  }
  CYC(0x60fc, 0x60fd);
  CYC(0x60fd, 0x6100); A = mem_rd(gb, w1Link_state);
  CYC(0x6100, 0x6102); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x6102, 0x6103); ret_effect(gb);
    return;
  }
  CYC(0x6102, 0x6103);
  CYC(0x6103, 0x6106); A = mem_rd(gb, wTextIsActive);
  CYC(0x6106, 0x6107); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6107, 0x6108); ret_effect(gb);
    return;
  }
  CYC(0x6107, 0x6108);
  CYC(0x6108, 0x610b); A = mem_rd(gb, wDisableWarpTiles);
  CYC(0x610b, 0x610c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x610c, 0x610d); ret_effect(gb);
    return;
  }
  CYC(0x610c, 0x610d);
  CYC(0x610d, 0x6110); SET_HL(w1Link_yh);
  CYC(0x6110, 0x6111); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6111, 0x6113); alu_add(gb, 0x04);
  CYC(0x6113, 0x6114); B = A;
  CYC(0x6114, 0x6115); L = alu_inc8(gb, L);
  CYC(0x6115, 0x6116); C = mem_rd(gb, HL);
  CALL_C(0x6116, getTileAtPosition_hook, 0x1447, 0x6119);
  CYC(0x6119, 0x611b); hram_wr(gb, 0x8c, A);
  CYC(0x611b, 0x611c); B = A;
  CYC(0x611c, 0x611d); A = L;
  CYC(0x611d, 0x611f); hram_wr(gb, 0x8d, A);
  CYC(0x611f, 0x6122); A = mem_rd(gb, wScrollMode);
  CYC(0x6122, 0x6124); alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x6124, 0x6126);
  } else {
    CYC(0x6124, 0x6126);
    CALL_C(0x6126, checkStandingOnDeactivatedWarpTile_hook, 0x61fd, 0x6129);
    if (!(F & FC)) {
      CYCT(0x6129, 0x612a); ret_effect(gb);
      return;
    }
    CYC(0x6129, 0x612a);
  }
  CYC(0x612a, 0x612c); A = 0xff;
  CYC(0x612c, 0x612f); mem_wr(gb, wEnteredWarpPosition, A);
  CYC(0x612f, 0x6132); A = mem_rd(gb, wActiveGroup);
  CYC(0x6132, 0x6133); bank1_jump_table_from_rst(gb, 0x6133);
  switch (HL) {
    case 0x6143: checkWarpsTopDown_hook(gb); return;
    case 0x614d: checkWarpsSidescrolling_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void updateLastToggleBlocksState_hook(GB *gb) {
  CYC(0x7c65, 0x7c68); A = mem_rd(gb, wToggleBlocksState);
  CYC(0x7c68, 0x7c6b); mem_wr(gb, wLastToggleBlocksState, A);
  CYC(0x7c6b, 0x7c6c); ret_effect(gb);
}

void checkUpdateToggleBlocks_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7c6c, checkDungeonUsesToggleBlocks_hook, 0x364b, 0x7c6f);
  if (F & FZ) {
    CYCT(0x7c6f, 0x7c70); ret_effect(gb);
    return;
  }
  CYC(0x7c6f, 0x7c70);
  CYC(0x7c70, 0x7c73); A = mem_rd(gb, wToggleBlocksState);
  CYC(0x7c73, 0x7c74); B = A;
  CYC(0x7c74, 0x7c77); A = mem_rd(gb, wLastToggleBlocksState);
  CYC(0x7c77, 0x7c78); alu_xor(gb, B);
  CYC(0x7c78, 0x7c79); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x7c79, 0x7c7a); ret_effect(gb);
    return;
  }
  CYC(0x7c79, 0x7c7a);
  CYC(0x7c7a, 0x7c7c); A = 0x02;
  CYC(0x7c7c, 0x7c7f); mem_wr(gb, wCutsceneTrigger, A);
  CYC(0x7c7f, 0x7c80); ret_effect(gb);
}

void cutscene1f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7f15, 0x7f18); SET_HL(0x7cb7);
  CYC(0x7f18, 0x7f1a); E = 0x03;
  CALL_C(0x7f1a, interBankCall_hook, 0x008a, 0x7f1d);
  CALL_C(0x7f1d, updateStatusBar_hook, 0x1a9c, 0x7f20);
  CYC(0x7f20, 0x7f23); updateAllObjects_hook(gb);
}

void paletteFadeHandler09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5705, paletteThread_decCounter_hook, 0x592e, 0x5708);
  if (!(F & FZ)) {
    CYCT(0x5708, 0x5709); ret_effect(gb);
    return;
  }
  CYC(0x5708, 0x5709);
  paletteFadeHandler01_hook(gb);
}

void paletteFadeHandler01_hook(GB *gb) {
  CYC(0x5709, 0x570b); A = 0x1f;
  CYC(0x570b, 0x570d); hram_wr(gb, 0x8b, A);
  CYC(0x570d, 0x5710); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x5710, 0x5711); C = A;
  CYC(0x5711, 0x5714); A = mem_rd(gb, 0xc2ff);
  CYC(0x5714, 0x5715); alu_add(gb, C);
  CYC(0x5715, 0x5717); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(0x5717, 0x571a);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x5717, 0x571a);
  CYC(0x571a, 0x571d); mem_wr(gb, 0xc2ff, A);
  CYC(0x571d, 0x571e); C = A;
  updateFadingPalettes(gb);
}

void paletteFadeHandler00_hook(GB *gb) {
  CYC(0x5736, 0x5737); ret_effect(gb);
}

void paletteFadeHandler0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5737, paletteThread_decCounter_hook, 0x592e, 0x573a);
  if (!(F & FZ)) {
    CYCT(0x573a, 0x573b); ret_effect(gb);
    return;
  }
  CYC(0x573a, 0x573b);
  paletteFadeHandler02_hook(gb);
}

void paletteFadeHandler02_hook(GB *gb) {
  CYC(0x573b, 0x573d); A = 0x1f;
  CYC(0x573d, 0x573f); hram_wr(gb, 0x8b, A);
  CYC(0x573f, 0x5742); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x5742, 0x5743); C = A;
  CYC(0x5743, 0x5746); A = mem_rd(gb, 0xc2ff);
  CYC(0x5746, 0x5747); alu_sub(gb, C);
  if (F & FC) {
    CYCT(0x5747, 0x5749);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x5747, 0x5749);
  CYC(0x5749, 0x574c); mem_wr(gb, 0xc2ff, A);
  CYC(0x574c, 0x574d); C = A;
  CYC(0x574d, 0x574f);
  updateFadingPalettes(gb);
}

void paletteFadeHandler0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x574f, paletteThread_decCounter_hook, 0x592e, 0x5752);
  if (!(F & FZ)) {
    CYCT(0x5752, 0x5753); ret_effect(gb);
    return;
  }
  CYC(0x5752, 0x5753);
  paletteFadeHandler03_hook(gb);
}

void paletteFadeHandler03_hook(GB *gb) {
  CYC(0x5753, 0x5754); alu_xor(gb, A);
  CYC(0x5754, 0x5756); hram_wr(gb, 0x8b, A);
  CYC(0x5756, 0x5759); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x5759, 0x575a); C = A;
  CYC(0x575a, 0x575d); A = mem_rd(gb, 0xc2ff);
  CYC(0x575d, 0x575e); alu_sub(gb, C);
  CYC(0x575e, 0x5760); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(0x5760, 0x5762);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x5760, 0x5762);
  CYC(0x5762, 0x5765); mem_wr(gb, 0xc2ff, A);
  CYC(0x5765, 0x5766); C = A;
  CYC(0x5766, 0x5768);
  updateFadingPalettes(gb);
}

void paletteFadeHandler0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5768, paletteThread_decCounter_hook, 0x592e, 0x576b);
  if (!(F & FZ)) {
    CYCT(0x576b, 0x576c); ret_effect(gb);
    return;
  }
  CYC(0x576b, 0x576c);
  paletteFadeHandler04_hook(gb);
}

void paletteFadeHandler04_hook(GB *gb) {
  CYC(0x576c, 0x576d); alu_xor(gb, A);
  CYC(0x576d, 0x576f); hram_wr(gb, 0x8b, A);
  CYC(0x576f, 0x5772); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x5772, 0x5773); C = A;
  CYC(0x5773, 0x5776); A = mem_rd(gb, 0xc2ff);
  CYC(0x5776, 0x5777); alu_add(gb, C);
  if (F & FC) {
    CYCT(0x5777, 0x5779);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x5777, 0x5779);
  CYC(0x5779, 0x577c); mem_wr(gb, 0xc2ff, A);
  CYC(0x577c, 0x577d); C = A;
  CYC(0x577d, 0x5780);
  updateFadingPalettes(gb);
}

void paletteThread_setFadeOffsetAndStop_hook(GB *gb) {
  CYC(0x5780, 0x5781); A = B;
  CYC(0x5781, 0x5783); alu_sub(gb, 0x1f);
  CYC(0x5783, 0x5786); mem_wr(gb, 0xc2ff, A);
  paletteThread_stop_hook(gb);
}

void paletteThread_stop_hook(GB *gb) {
  CYC(0x5786, 0x5787); alu_xor(gb, A);
  CYC(0x5787, 0x578a); mem_wr(gb, wPaletteThread_updateRate, A);
  CYC(0x578a, 0x578d); mem_wr(gb, wPaletteThread_mode, A);
  CYC(0x578d, 0x5790); clearPaletteFadeVariables_hook(gb);
}

void paletteThread_refreshPalettesAndStop_hook(GB *gb) {
  CYC(0x5790, 0x5791); alu_xor(gb, A);
  CYC(0x5791, 0x5794); mem_wr(gb, wPaletteThread_updateRate, A);
  CYC(0x5794, 0x5797); mem_wr(gb, wPaletteThread_mode, A);
  CYC(0x5797, 0x579a); clearPaletteFadeVariablesAndRefreshPalettes_hook(gb);
}

void paletteFadeHandler0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x579a, paletteThread_decCounter_hook, 0x592e, 0x579d);
  if (!(F & FZ)) {
    CYCT(0x579d, 0x579e); ret_effect(gb);
    return;
  }
  CYC(0x579d, 0x579e);
  paletteFadeHandler05_hook(gb);
}

void paletteFadeHandler05_hook(GB *gb) {
  CYC(0x579e, 0x579f); alu_xor(gb, A);
  CYC(0x579f, 0x57a1); hram_wr(gb, 0x8b, A);
  CYC(0x57a1, 0x57a4); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x57a4, 0x57a5); C = A;
  CYC(0x57a5, 0x57a8); A = mem_rd(gb, wPaletteThread_parameter);
  CYC(0x57a8, 0x57a9); A = alu_dec8(gb, A);
  CYC(0x57a9, 0x57aa); B = A;
  CYC(0x57aa, 0x57ad); A = mem_rd(gb, 0xc2ff);
  CYC(0x57ad, 0x57ae); alu_sub(gb, C);
  CYC(0x57ae, 0x57af); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x57af, 0x57b1);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x57af, 0x57b1);
  if (F & FC) {
    CYCT(0x57b1, 0x57b3);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x57b1, 0x57b3);
  CYC(0x57b3, 0x57b6); mem_wr(gb, 0xc2ff, A);
  CYC(0x57b6, 0x57b7); C = A;
  CYC(0x57b7, 0x57ba); updateFadingPalettes(gb);
}

void paletteFadeHandler0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57ba, paletteThread_decCounter_hook, 0x592e, 0x57bd);
  if (!(F & FZ)) {
    CYCT(0x57bd, 0x57be); ret_effect(gb);
    return;
  }
  CYC(0x57bd, 0x57be);
  paletteFadeHandler06_hook(gb);
}

void paletteFadeHandler06_hook(GB *gb) {
  CYC(0x57be, 0x57bf); alu_xor(gb, A);
  CYC(0x57bf, 0x57c1); hram_wr(gb, 0x8b, A);
  CYC(0x57c1, 0x57c4); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x57c4, 0x57c5); C = A;
  CYC(0x57c5, 0x57c8); A = mem_rd(gb, wPaletteThread_parameter);
  CYC(0x57c8, 0x57ca); alu_add(gb, 0x1f);
  CYC(0x57ca, 0x57cb); B = A;
  CYC(0x57cb, 0x57ce); A = mem_rd(gb, 0xc2ff);
  CYC(0x57ce, 0x57d0); alu_add(gb, 0x1f);
  CYC(0x57d0, 0x57d1); alu_add(gb, C);
  CYC(0x57d1, 0x57d2); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x57d2, 0x57d4);
    paletteThread_stop_hook(gb);
    return;
  }
  CYC(0x57d2, 0x57d4);
  if (!(F & FC)) {
    CYCT(0x57d4, 0x57d7);
    paletteThread_setFadeOffsetAndStop_hook(gb);
    return;
  }
  CYC(0x57d4, 0x57d7);
  CYC(0x57d7, 0x57d9); alu_sub(gb, 0x1f);
  CYC(0x57d9, 0x57dc); mem_wr(gb, 0xc2ff, A);
  CYC(0x57dc, 0x57dd); C = A;
  CYC(0x57dd, 0x57e0); updateFadingPalettes(gb);
}

void paletteFadeHandler07_hook(GB *gb) {
  CYC(0x57e0, 0x57e2); A = 0x1f;
  CYC(0x57e2, 0x57e4); hram_wr(gb, 0x8b, A);
  CYC(0x57e4, 0x57e7); A = mem_rd(gb, wPaletteThread_speed);
  CYC(0x57e7, 0x57e8); C = A;
  CYC(0x57e8, 0x57eb); A = mem_rd(gb, 0xc2ff);
  CYC(0x57eb, 0x57ec); alu_sub(gb, C);
  if (!(F & FC)) {
    CYC(0x57ec, 0x57ee);
    CYC(0x57ee, 0x57f1); mem_wr(gb, 0xc2ff, A);
    CYC(0x57f1, 0x57f2); C = A;
    CYC(0x57f2, 0x57f5); updateFadingPalettes(gb);
    return;
  }
  CYCT(0x57ec, 0x57ee);
  CYC(0x57f5, 0x57f7); A = 0xff;
  CYC(0x57f7, 0x57f9); hram_wr(gb, 0xa6, A);
  CYC(0x57f9, 0x57fb); hram_wr(gb, 0xa7, A);
  CYC(0x57fb, 0x57fe); A = mem_rd(gb, wPaletteThread_parameter);
  CYC(0x57fe, 0x57ff); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x57ff, 0x5801);
    paletteThread_refreshPalettesAndStop_hook(gb);
    return;
  }
  CYC(0x57ff, 0x5801);
  CYC(0x5801, 0x5802); B = A;
  CYC(0x5802, 0x5803); alu_xor(gb, A);
  CYC(0x5803, 0x5806); mem_wr(gb, wPaletteThread_parameter, A);
  CYC(0x5806, 0x5807); A = B;
  CYC(0x5807, 0x5809); alu_cp(gb, 0xf0);
  if (F & FZ) {
    CYCT(0x5809, 0x580c);
    darkenRoom_hook(gb);
    return;
  }
  CYC(0x5809, 0x580c);
  CYC(0x580c, 0x580f); darkenRoomLightly_hook(gb);
}

void palette_fade_handler08_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5815, 0x5816); A = mem_rd(gb, HL);
  CYC(0x5816, 0x5817); alu_rrca(gb);
  CYC(0x5817, 0x5819); alu_and(gb, 0x0f);
  CYC(0x5819, 0x581a); B = A;
  CYC(0x581a, 0x581c); A = alu_swap(gb, A);
  CYC(0x581c, 0x581e); hram_wr(gb, 0x91, A);
  CYC(0x581e, 0x5820); A = 0x10;
  CYC(0x5820, 0x5821); alu_sub(gb, B);
  CYC(0x5821, 0x5823); A = alu_swap(gb, A);
  CYC(0x5823, 0x5825); hram_wr(gb, 0x90, A);
  CYC(0x5825, 0x5826); A = mem_rd(gb, HL);
  CYC(0x5826, 0x5827); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x5827, 0x582a);
    paletteThread_mixBG234Palettes_hook(gb);
    return;
  }
  CYC(0x5827, 0x582a);
  CALL_C(0x582a, paletteThread_mixBG567Palettes_hook, 0x588f, 0x582d);
  CYC(0x582d, 0x582f); A = hram_rd(gb, 0xa6);
  CYC(0x582f, 0x5831); alu_or(gb, 0xfc);
  CYC(0x5831, 0x5833); hram_wr(gb, 0xa6, A);
  CYC(0x5833, 0x5835); A = 0xfc;
  CYC(0x5835, 0x5837); hram_wr(gb, 0xa8, A);
  CYC(0x5837, 0x5838); ret_effect(gb);
}

void paletteFadeHandler08_hook(GB *gb) {
  CYC(0x580f, 0x5812); SET_HL(0xc2ff);
  CYC(0x5812, 0x5813); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x5813, 0x5815);
    paletteFadeHandler08__stop_hook(gb);
    return;
  }
  CYC(0x5813, 0x5815);
  palette_fade_handler08_body_hook(gb);
}

void paletteFadeHandler08__seasonsFunc_01_5816_hook(GB *gb) {
  palette_fade_handler08_body_hook(gb);
}

void paletteFadeHandler08__stop_hook(GB *gb) {
  CYC(0x5838, 0x583b); paletteThread_stop_hook(gb);
}

static void palette_thread_calculate_next_color(GB *gb) {
  for (;;) {
    CYC(0x5840, 0x5841); E = mem_rd(gb, HL);
    CYC(0x5841, 0x5842); L = alu_inc8(gb, L);
    CYC(0x5842, 0x5843); A = mem_rd(gb, HL);
    CYC(0x5843, 0x5845); E = alu_sla(gb, E);
    CYC(0x5845, 0x5846); alu_rla(gb);
    CYC(0x5846, 0x5848); E = alu_rl(gb, E);
    CYC(0x5848, 0x5849); alu_rla(gb);
    CYC(0x5849, 0x584b); E = alu_rl(gb, E);
    CYC(0x584b, 0x584c); alu_rla(gb);
    CYC(0x584c, 0x584e); alu_and(gb, 0x1f);
    CYC(0x584e, 0x584f); alu_add(gb, C);
    CYC(0x584f, 0x5851); alu_bit(gb, 5, A);
    if (F & FZ) CYCT(0x5851, 0x5853);
    else { CYC(0x5851, 0x5853); CYC(0x5853, 0x5855); A = hram_rd(gb, 0x8b); }
    CYC(0x5855, 0x5857); E = 0;
    CYC(0x5857, 0x5859); A = alu_srl(gb, A);
    CYC(0x5859, 0x585b); E = alu_rr(gb, E);
    CYC(0x585b, 0x585c); alu_rra(gb);
    CYC(0x585c, 0x585e); E = alu_rr(gb, E);
    CYC(0x585e, 0x585f); alu_rra(gb);
    CYC(0x585f, 0x5861); E = alu_rr(gb, E);
    CYC(0x5861, 0x5862); D = A;
    CYC(0x5862, 0x5863); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(0x5863, 0x5864); alu_rra(gb);
    CYC(0x5864, 0x5865); alu_rra(gb);
    CYC(0x5865, 0x5867); alu_and(gb, 0x1f);
    CYC(0x5867, 0x5868); alu_add(gb, C);
    CYC(0x5868, 0x586a); alu_bit(gb, 5, A);
    if (F & FZ) CYCT(0x586a, 0x586c);
    else { CYC(0x586a, 0x586c); CYC(0x586c, 0x586e); A = hram_rd(gb, 0x8b); }
    CYC(0x586e, 0x586f); alu_rlca(gb);
    CYC(0x586f, 0x5870); alu_rlca(gb);
    CYC(0x5870, 0x5871); alu_or(gb, D);
    CYC(0x5871, 0x5872); D = A;
    CYC(0x5872, 0x5873); A = mem_rd(gb, HL);
    CYC(0x5873, 0x5875); alu_and(gb, 0x1f);
    CYC(0x5875, 0x5876); alu_add(gb, C);
    CYC(0x5876, 0x5878); alu_bit(gb, 5, A);
    if (F & FZ) CYCT(0x5878, 0x587a);
    else { CYC(0x5878, 0x587a); CYC(0x587a, 0x587c); A = hram_rd(gb, 0x8b); }
    CYC(0x587c, 0x587d); alu_or(gb, E);
    CYC(0x587d, 0x587e); H = alu_inc8(gb, H);
    CYC(0x587e, 0x587f); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x587f, 0x5880); mem_wr(gb, HL, D);
    CYC(0x5880, 0x5881); L = alu_inc8(gb, L);
    CYC(0x5881, 0x5882); H = alu_dec8(gb, H);
    CYC(0x5882, 0x5883); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x5883, 0x5885); continue; }
    CYC(0x5883, 0x5885);
    CYC(0x5885, 0x5886); ret_effect(gb);
    return;
  }
}

void paletteThread_calculateFadingPalettes_hook(GB *gb) {
  CYC(0x583b, 0x583e); SET_HL(w2TilesetBgPalettes);
  CYC(0x583e, 0x5840); B = 0x40;
  palette_thread_calculate_next_color(gb);
}

void paletteThread_calculateFadingPalettes__nextColor_hook(GB *gb) {
  palette_thread_calculate_next_color(gb);
}

void paletteThread_mixBG567Palettes__mixColors_hook(GB *gb) {
  for (;;) {
    CYC(0x58d8, 0x58da); A = hram_rd(gb, 0x91);
    CYC(0x58da, 0x58db); H = A;
    CYC(0x58db, 0x58dd); D = 0xda;
    CYC(0x58dd, 0x58de); A = mem_rd(gb, DE);
    CYC(0x58de, 0x58df); C = A;
    CYC(0x58df, 0x58e1); B = 0;
    CYC(0x58e1, 0x58e2); L = B;
    CYC(0x58e2, 0x58e4); A = 4;
    for (;;) {
      CYC(0x58e4, 0x58e5); alu_add_hl(gb, HL);
      if (!(F & FC)) CYCT(0x58e5, 0x58e7);
      else { CYC(0x58e5, 0x58e7); CYC(0x58e7, 0x58e8); alu_add_hl(gb, BC); }
      CYC(0x58e8, 0x58e9); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(0x58e9, 0x58eb); continue; }
      CYC(0x58e9, 0x58eb);
      break;
    }
    CYC(0x58eb, 0x58ec); push_effect(gb, HL);
    CYC(0x58ec, 0x58ee); A = hram_rd(gb, 0x90);
    CYC(0x58ee, 0x58ef); H = A;
    CYC(0x58ef, 0x58f1); D = 0xdb;
    CYC(0x58f1, 0x58f2); A = mem_rd(gb, DE);
    CYC(0x58f2, 0x58f3); C = A;
    CYC(0x58f3, 0x58f5); B = 0;
    CYC(0x58f5, 0x58f6); L = B;
    CYC(0x58f6, 0x58f8); A = 4;
    for (;;) {
      CYC(0x58f8, 0x58f9); alu_add_hl(gb, HL);
      if (!(F & FC)) CYCT(0x58f9, 0x58fb);
      else { CYC(0x58f9, 0x58fb); CYC(0x58fb, 0x58fc); alu_add_hl(gb, BC); }
      CYC(0x58fc, 0x58fd); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(0x58fd, 0x58ff); continue; }
      CYC(0x58fd, 0x58ff);
      break;
    }
    CYC(0x58ff, 0x5900); SET_BC(pop_effect(gb));
    CYC(0x5900, 0x5901); alu_add_hl(gb, BC);
    CYC(0x5901, 0x5902); ret_effect(gb);
    return;
  }
}

void paletteThread_mixBG567Palettes__writeToFadingBgPalettes_hook(GB *gb) {
  CYC(0x5902, 0x5903); H = alu_inc8(gb, H);
  CYC(0x5903, 0x5905); A = hram_rd(gb, 0x8b);
  CYC(0x5905, 0x5907); C = 0;
  CYC(0x5907, 0x5909); A = alu_srl(gb, A);
  CYC(0x5909, 0x590b); C = alu_rr(gb, C);
  CYC(0x590b, 0x590c); alu_rra(gb);
  CYC(0x590c, 0x590e); C = alu_rr(gb, C);
  CYC(0x590e, 0x590f); alu_rra(gb);
  CYC(0x590f, 0x5911); C = alu_rr(gb, C);
  CYC(0x5911, 0x5912); B = A;
  CYC(0x5912, 0x5914); A = hram_rd(gb, 0x8c);
  CYC(0x5914, 0x5915); alu_or(gb, C);
  CYC(0x5915, 0x5916); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5916, 0x5918); A = hram_rd(gb, 0x8d);
  CYC(0x5918, 0x5919); alu_rlca(gb);
  CYC(0x5919, 0x591a); alu_rlca(gb);
  CYC(0x591a, 0x591b); alu_or(gb, B);
  CYC(0x591b, 0x591c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x591c, 0x591d); H = alu_dec8(gb, H);
  CYC(0x591d, 0x591e); ret_effect(gb);
}

void palette_thread_mix_next_color_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x589a, 0x589b); push_effect(gb, BC);
    CYC(0x589b, 0x589c); push_effect(gb, HL);
    CALL_C(0x589c, paletteThread_mixBG567Palettes__mixColors_hook, 0x58d8, 0x589f);
    CYC(0x589f, 0x58a0); E = alu_inc8(gb, E);
    CYC(0x58a0, 0x58a2); L = alu_swap(gb, L);
    CYC(0x58a2, 0x58a3); A = L;
    CYC(0x58a3, 0x58a5); alu_and(gb, 0x0f);
    CYC(0x58a5, 0x58a6); L = A;
    CYC(0x58a6, 0x58a7); A = H;
    CYC(0x58a7, 0x58a9); A = alu_swap(gb, A);
    CYC(0x58a9, 0x58aa); alu_or(gb, L);
    CYC(0x58aa, 0x58ac); hram_wr(gb, 0x8b, A);
    CALL_C(0x58ac, paletteThread_mixBG567Palettes__mixColors_hook, 0x58d8, 0x58af);
    CYC(0x58af, 0x58b0); E = alu_inc8(gb, E);
    CYC(0x58b0, 0x58b2); L = alu_swap(gb, L);
    CYC(0x58b2, 0x58b3); A = L;
    CYC(0x58b3, 0x58b5); alu_and(gb, 0x0f);
    CYC(0x58b5, 0x58b6); L = A;
    CYC(0x58b6, 0x58b7); A = H;
    CYC(0x58b7, 0x58b9); A = alu_swap(gb, A);
    CYC(0x58b9, 0x58ba); alu_or(gb, L);
    CYC(0x58ba, 0x58bc); hram_wr(gb, 0x8d, A);
    CALL_C(0x58bc, paletteThread_mixBG567Palettes__mixColors_hook, 0x58d8, 0x58bf);
    CYC(0x58bf, 0x58c0); E = alu_inc8(gb, E);
    CYC(0x58c0, 0x58c2); L = alu_swap(gb, L);
    CYC(0x58c2, 0x58c3); A = L;
    CYC(0x58c3, 0x58c5); alu_and(gb, 0x0f);
    CYC(0x58c5, 0x58c6); L = A;
    CYC(0x58c6, 0x58c7); A = H;
    CYC(0x58c7, 0x58c9); A = alu_swap(gb, A);
    CYC(0x58c9, 0x58ca); alu_or(gb, L);
    CYC(0x58ca, 0x58cc); hram_wr(gb, 0x8c, A);
    CYC(0x58cc, 0x58cd); SET_HL(pop_effect(gb));
    CALL_C(0x58cd, paletteThread_mixBG567Palettes__writeToFadingBgPalettes_hook, 0x5902, 0x58d0);
    CYC(0x58d0, 0x58d1); SET_BC(pop_effect(gb));
    CYC(0x58d1, 0x58d2); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x58d2, 0x58d4); continue; }
    CYC(0x58d2, 0x58d4);
    CYC(0x58d4, 0x58d5); alu_xor(gb, A);
    CYC(0x58d5, 0x58d7); hram_wr(gb, 0x70, A);
    CYC(0x58d7, 0x58d8); ret_effect(gb);
    return;
  }
}

static void palette_thread_mix_body(GB *gb) {
  CYC(0x5896, 0x5898); A = 0x02;
  CYC(0x5898, 0x589a); hram_wr(gb, 0x70, A);
  palette_thread_mix_next_color_hook(gb);
}

void paletteThread_mixBG234Palettes_hook(GB *gb) {
  CYC(0x5886, 0x5889); SET_HL(0xde90);
  CYC(0x5889, 0x588b); E = 0;
  CYC(0x588b, 0x588d); B = 0x0c;
  CYC(0x588d, 0x588f);
  palette_thread_mix_body(gb);
}

void paletteThread_mixBG567Palettes_hook(GB *gb) {
  CYC(0x588f, 0x5892); SET_HL(0xdea8);
  CYC(0x5892, 0x5894); E = 0x24;
  CYC(0x5894, 0x5896); B = 0x0c;
  palette_thread_mix_body(gb);
}

void paletteThread_mixBG567Palettes__nextColor_hook(GB *gb) {
  palette_thread_mix_next_color_hook(gb);
}

void paletteThread_decCounter_hook(GB *gb) {
  CYC(0x592e, 0x5931); SET_HL(wPaletteThread_counter);
  CYC(0x5931, 0x5932); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5932, 0x5933); ret_effect(gb);
    return;
  }
  CYC(0x5932, 0x5933);
  CYC(0x5933, 0x5936); A = mem_rd(gb, wPaletteThread_counterRefill);
  CYC(0x5936, 0x5939); mem_wr(gb, wPaletteThread_counter, A);
  CYC(0x5939, 0x593a); ret_effect(gb);
}
