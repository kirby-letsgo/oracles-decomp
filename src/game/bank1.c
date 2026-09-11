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
