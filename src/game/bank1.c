#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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

static void bank1_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
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
void loadDungeonLayout_b01_hook(GB *gb);
void loadDungeonLayout_b01__nextFloor_hook(GB *gb);
void loadDungeonLayout_b01__nextByte_hook(GB *gb);
void loadDungeonLayout_b01__end_hook(GB *gb);
void clearDungeonLayout_hook(GB *gb);
void findActiveRoomInDungeonLayoutWithPointlessBankSwitch_hook(GB *gb);
void findActiveRoomInDungeonLayout_hook(GB *gb);
void getFirstDungeonLayoutAddress_hook(GB *gb);
void checkUpdateDungeonMinimap_hook(GB *gb);
void checkUpdateDungeonMinimap__setMinimapRoom_hook(GB *gb);
void checkBrightenRoom_hook(GB *gb);
void checkPlayRoomMusic_hook(GB *gb);
void checkPlayRoomMusic__setMusic_hook(GB *gb);
void func_593a_hook(GB *gb);
void func_5cfe_hook(GB *gb);
void func_5cfe__clearCompanion_hook(GB *gb);
void func_5cfe__end_hook(GB *gb);
void setEnteredWarpPosition_hook(GB *gb);
void paletteFadeHandler_hook(GB *gb);
void updateFadingPalettes_hook(GB *gb);
void checkLockBG7Color3ToBlack_hook(GB *gb);
void checkLockBG7Color3ToBlack__thing1_hook(GB *gb);
void checkLockBG7Color3ToBlack__thing0_hook(GB *gb);
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
void incrementScreenScrollRowVars_hook(GB *gb);
void addFunctionsToVBlankQueue_hook(GB *gb);
void addFunctionsToVBlankQueue__locFunc_hook(GB *gb);
void func_46ca_hook(GB *gb);
void func_46ff_hook(GB *gb);
void copyTileRowToVramBuffer_hook(GB *gb);
void copyTileRowToVramBuffer__copyFunc_hook(GB *gb);
void updateTilesetPalette_hook(GB *gb);
void cpLinkState0e_hook(GB *gb);
void initWaveScrollValues_body_hook(GB *gb);
void loadBigBufferScrollValues_body_hook(GB *gb);
void func_47fc_hook(GB *gb);
void checkAndApplyPaletteFadeTransition_hook(GB *gb);
void getPaletteFadeTransitionData_hook(GB *gb);
void applyPaletteFadeTransitionData_hook(GB *gb);
void checkSymmetryCityPaletteTransition_hook(GB *gb);
void checkSymmetryCityPaletteTransition__ok_hook(GB *gb);
void checkSymmetryCityPaletteTransition__notOk_hook(GB *gb);
void makeActiveObjectFollowLink_b01_hook(GB *gb);
void resetFollowingLinkPath_hook(GB *gb);
void checkUpdateFollowingLinkObject_hook(GB *gb);
void checkUpdateFollowingLinkObject__update_hook(GB *gb);
void clearMemoryOnScreenReload_hook(GB *gb);
void func_49c9_hook(GB *gb);
void setObjectsEnabledTo2_hook(GB *gb);
void setItemsEnabledTo2_hook(GB *gb);
void setInteractionsEnabledTo2_hook(GB *gb);
void setEnemiesEnabledTo2_hook(GB *gb);
void setPartsEnabledTo2_hook(GB *gb);
void setObjectsEnabledTo2_hlpr_hook(GB *gb);
void func_400b_hook(GB *gb);
void screenTransitionState0_hook(GB *gb);
void initializeRoomBoundaryAndLoadAnimations_hook(GB *gb);
void screenTransitionState1_hook(GB *gb);
void screenTransitionState1__substate0_hook(GB *gb);
void screenTransitionState1__substate1_hook(GB *gb);
void screenTransitionState1__substate2_hook(GB *gb);
void setScreenTransitionState02_hook(GB *gb);
void screenTransitionState2_hook(GB *gb);
void screenTransitionState2__transitionUp_hook(GB *gb);
void screenTransitionState2__transitionDown_hook(GB *gb);
void screenTransitionState2__transitionLeft_hook(GB *gb);
void screenTransitionState2__transitionRight_hook(GB *gb);
void screenTransitionState2__transition_hook(GB *gb);
void screenTransitionState2__doneBoundaryChecks_hook(GB *gb);
void screenTransitionState2__startTransition_hook(GB *gb);
void screenTransitionState2__checkCanTransitionOverWater_hook(GB *gb);
void screenTransitionState2__fail_hook(GB *gb);
void updateCameraPosition_hook(GB *gb);
void updateCameraPosition__smallRoom_hook(GB *gb);
void updateCameraPosition__largeRoom_hook(GB *gb);
void updateCameraPosition__updateComponent_hook(GB *gb);
void updateCameraPosition__smBit7_hook(GB *gb);
void calculateCameraPosition_hook(GB *gb);
void updateScreenShake_hook(GB *gb);
void updateScreenShake__getShakeAmount_hook(GB *gb);
void updateGfxRegs2Scroll_hook(GB *gb);
void screenTransitionState3_hook(GB *gb);
void checkDarkenRoomAndClearPaletteFadeState_hook(GB *gb);
void checkDarkenRoom_hook(GB *gb);
void screenTransitionState4_hook(GB *gb);
void screenTransitionState5_hook(GB *gb);
void screenTransitionState5Substate0_hook(GB *gb);
void screenTransitionState5Substate0__horizontal_hook(GB *gb);
void screenTransitionState5Substate0__vertical_hook(GB *gb);
void transitionUpdateScrollAndLinkPosition_hook(GB *gb);
void finishScrollingTransition_hook(GB *gb);
void label_01_037_hook(GB *gb);
void func_4493_hook(GB *gb);
void resetFollowingLinkObjectPosition_hook(GB *gb);
void screenTransitionState5Substate2_hook(GB *gb);
void screenTransitionState5Substate2__state0_hook(GB *gb);
void screenTransitionState5Substate2__state1_hook(GB *gb);
void screenTransitionState5Substate2__state2_hook(GB *gb);
void screenTransitionState5Substate2__state3_hook(GB *gb);
void screenTransitionState5Substate2__state4_hook(GB *gb);
void screenTransitionState5Substate2__state5_hook(GB *gb);
void screenTransitionState5Substate2__drawNextRow_hook(GB *gb);
void screenTransitionState5Substate1_hook(GB *gb);
void screenTransitionState5Substate1__state0_hook(GB *gb);
void screenTransitionState5Substate1__state1_hook(GB *gb);
void screenTransitionState5Substate1__state2_hook(GB *gb);
void screenTransitionState5Substate1__state3_hook(GB *gb);
void screenTransitionState5Substate1__state4_hook(GB *gb);
void screenTransitionState5Substate1__state5_hook(GB *gb);
void screenTransitionState5Substate1__drawNextRow_hook(GB *gb);
void screenTransitionState5Substate1__queueRowDmaTransfer_hook(GB *gb);
void clearObjectsWithEnabled2_hook(GB *gb);
void clearItemsWithEnabled2_hook(GB *gb);
void clearInteractionsWithEnabled2_hook(GB *gb);
void clearEnemiesWithEnabled2_hook(GB *gb);
void clearPartsWithEnabled2_hook(GB *gb);
void clearObjectsWithEnabled2_hlpr_hook(GB *gb);
void playCompassSoundIfKeyInRoom_hook(GB *gb);
void playCompassSoundIfKeyInRoom__playSound_hook(GB *gb);
void updateLinkBeingShocked_hook(GB *gb);
void updateLinkBeingShocked__val00_hook(GB *gb);
void updateLinkBeingShocked__val01_hook(GB *gb);
void updateLinkBeingShocked__val02_hook(GB *gb);
void initiateFallDownHoleWarp_hook(GB *gb);
void cutscene17_hook(GB *gb);
void cutscene17__state0_hook(GB *gb);
void cutscene17__state1_hook(GB *gb);
void cutscene17__state2_hook(GB *gb);
void cutscene17__state3_hook(GB *gb);
void cutscene17__state4_hook(GB *gb);
void initWaveScrollValuesForEverySecondLine_hook(GB *gb);
void cutscene15__func_4c03_hook(GB *gb);
void cutscene15__incTmpcbb3_hook(GB *gb);
void cutscene15__state0_hook(GB *gb);
void cutscene15__update_hook(GB *gb);
void cutscene15__state1_hook(GB *gb);
void cutscene15__state1__substate0_hook(GB *gb);
void cutscene15__state1__initWaveScrollValuesInverted_hook(GB *gb);
void cutscene15__state1__substate1_hook(GB *gb);
void cutscene15__state2_hook(GB *gb);
void cutscene15__state2__substate1_hook(GB *gb);

static void screenTransitionEyePuzzle_up(GB *gb);
static void screenTransitionEyePuzzle_rightOrLeft(GB *gb);
static void checkLinkCloseEnoughToWarpTileCenter_func(GB *gb);
static void checkLinkCloseEnoughToWarpTileCenter_tileSolid(GB *gb);

void func_5d41_hook(GB *gb) {
  BASE(func_5d41);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wWarpTransition2);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+10);
    TAIL(applyWarpTransition2);
  }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+13);
  TAIL(updateAllObjects);
}

void cutscene16_hook(GB *gb) {
  BASE(cutscene16);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateMenus_hook, SYM(updateMenus), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(wWarpTransition2);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+13); A = 0x03;
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
  } else {
    CYC(b_+13, b_+15);
    CALL_C(b_+15, updateAllObjects_hook, SYM(updateAllObjects), b_+18);
    CYC(b_+18, b_+20); A = 0x01;
  }
  CYC(b_+20, b_+23); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+27); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+27, b_+30); mem_wr(gb, wLinkCanPassNpcs, A);
  CYC(b_+30, b_+33); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+33, b_+34); ret_effect(gb);
}

void triggerFadeoutTransition_hook(GB *gb) {
  BASE(triggerFadeoutTransition);
  CYC(b_+0, b_+2); A = 0x05;
  CYC(b_+2, b_+5); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+5, b_+8);
  TAIL(fadeoutToWhite);
}

void applyWarpTransition2_hook(GB *gb) {
  BASE(applyWarpTransition2);
  CYC(b_+0, b_+3); SET_HL(wWarpTransition2);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x00);
  CYC(b_+7, b_+9); alu_and(gb, 0x0f);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+15); A = 0x01;
    CYC(b_+15, b_+18); mem_wr(gb, (wThreadStateBuffer + 14), A);
    CYC(b_+18, b_+19); alu_xor(gb, A);
    CYC(b_+19, b_+22); mem_wr(gb, (wThreadStateBuffer + 15), A);
    CYC(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+23, b_+26); A = W8(wLinkObjectIndex);
  CYC(b_+26, b_+28); alu_cp(gb, 0xd1);
  if (!(F & FZ)) {
    CYCT(b_+28, b_+30);
  } else {
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+31); B = alu_inc8(gb, B);
  }
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+34); alu_and(gb, 0x0f);
  CYC(b_+34, b_+37); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+37, b_+39); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(b_+39, b_+42);
    TAIL(fadeoutToWhite);
  }
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); A = 0x04;
  CYC(b_+44, b_+47);
  TAIL(fadeoutToWhiteWithDelay);
}

void setCutsceneIndexIfCutsceneTriggerSet_hook(GB *gb) {
  BASE(setCutsceneIndexIfCutsceneTriggerSet);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneTrigger);
  CYC(b_+3, b_+5); alu_and(gb, 0x7f);
  CYC(b_+5, b_+8); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+12, b_+15); mem_wr(gb, wCutsceneState, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void checkDisplayEraOrSeasonInfo_hook(GB *gb) {
  BASE(checkDisplayEraOrSeasonInfo);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x16;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+9); A = 0x16;
    CYC(b_+9, b_+12);
    TAIL(unsetGlobalFlag);
  }
  CYC(b_+12, b_+15); A = mem_rd(gb, wSentBackByStrangeForce);
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+16, b_+17); ret_effect(gb);
    return;
  }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+20, b_+22); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(b_+25, b_+26); ret_effect(gb);
    return;
  }
  CYC(b_+25, b_+26);
  CALL_C(b_+26, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+29);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+30); ret_effect(gb);
    return;
  }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xe0);
  CYC(b_+32, b_+33); ret_effect(gb);
}

void updateGrassAnimationModifier_hook(GB *gb) {
  BASE(updateGrassAnimationModifier);
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); mem_wr(gb, wGrassAnimationModifier, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void loadDeathRespawnBufferPreset_hook(GB *gb) {
  BASE(loadDeathRespawnBufferPreset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+2); A = C;
  CALL_C(b_+2, multiplyABy8_hook, SYM(multiplyABy8), b_+5);
  CYC(b_+5, b_+8); SET_HL(b_+25);
  CYC(b_+8, b_+9); alu_add_hl(gb, BC);
  CYC(b_+9, b_+12); SET_DE(wDeathRespawnBuffer - 1);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); B = A;
loadDeathRespawnBufferPreset_loop:
  CYC(b_+14, b_+15); SET_DE(DE + 1);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+18); B = alu_sla(gb, B);
  if (!(F & FC)) {
    CYCT(b_+18, b_+20);
  } else {
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  }
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
    goto loadDeathRespawnBufferPreset_loop;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); SET_DE(pop_effect(gb));
  CYC(b_+24, b_+25); ret_effect(gb);
}

void checkRoomPack_hook(GB *gb) {
  BASE(checkRoomPack);
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (F & FC) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); alu_xor(gb, A);
    CYC(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+12); A = W8(wRoomPack);
  CYC(b_+12, b_+14); alu_and(gb, 0x7f);
  CYC(b_+14, b_+15); C = A;
  CYC(b_+15, b_+18); A = W8(wLoadingRoomPack);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+21); alu_and(gb, 0x7f);
  CYC(b_+21, b_+22); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); A = W8(wRoomPack);
  CYC(b_+26, b_+27); C = A;
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+31); W8(wRoomPack) = A;
  CYC(b_+31, b_+32); alu_or(gb, C);
  CYC(b_+32, b_+34); alu_bit(gb, 7, A);
  CYC(b_+34, b_+35); ret_effect(gb);
}

void calculateRoomEdge_hook(GB *gb) {
  BASE(calculateRoomEdge);
  CYC(b_+0, b_+3); SET_BC(0x80a0);
  CYC(b_+3, b_+6); A = W8(wRoomIsLarge);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+7, b_+9);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+12); SET_BC((wMainStack + 64));
  }
  CYC(b_+12, b_+15); SET_HL(wRoomEdgeY);
  CYC(b_+15, b_+16); mem_wr(gb, HL, B);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); mem_wr(gb, HL, C);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void updateActiveRoom_hook(GB *gb) {
  BASE(updateActiveRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    CYC(b_+19, b_+22); A = W8(wScreenTransitionDirection);
    CYC(b_+22, b_+25); SET_HL(b_+46);
    CYC(b_+25, b_+26); bank1_add_a_to_hl_from_rst(gb, b_+26);
    CYC(b_+26, b_+29); A = W8(wDungeonMapPosition);
    CYC(b_+29, b_+30); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+30, b_+33); W8(wDungeonMapPosition) = A;
    CALL_C(b_+33, getActiveRoomFromDungeonMapPosition_hook, SYM(getActiveRoomFromDungeonMapPosition), b_+36);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+9); A = W8(wScreenTransitionDirection);
    CYC(b_+9, b_+12); SET_HL(b_+42);
    CYC(b_+12, b_+13); bank1_add_a_to_hl_from_rst(gb, b_+13);
    CYC(b_+13, b_+16); A = W8(wActiveRoom);
    CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+17, b_+19);
  }
  CYC(b_+36, b_+39); W8(wActiveRoom) = A;
  CYC(b_+39, b_+42); TAIL(setVisitedRoomFlag);
}

void getNextActiveRoom_hook(GB *gb) {
  BASE(getNextActiveRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wActiveRoom);
  CYC(b_+9, b_+12); SET_HL(SYM(mapTransitionGroupTable));
  CALL_C(b_+12, findRoomSpecificData_hook, SYM(findRoomSpecificData), b_+15);
  if (!(F & FC)) {
    CYCT(b_+15, b_+17);
    TAIL(screenTransitionStandard);
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); bank1_jump_table_from_rst(gb, b_+18);
  HANDOFF(HL);
}

void screenTransitionStandard_hook(GB *gb) {
  BASE(screenTransitionStandard);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearEyePuzzleVars_hook, SYM(clearEyePuzzleVars), b_+3);
  CALL_C(b_+3, updateActiveRoom_hook, SYM(updateActiveRoom), b_+6);
  CYC(b_+6, b_+7); alu_scf(gb);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void clearEyePuzzleVars_hook(GB *gb) {
  BASE(clearEyePuzzleVars);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wLostWoodsTransitionCounter1) = A;
  CYC(b_+4, b_+7); W8(wLostWoodsTransitionCounter2) = A;
  CYC(b_+7, b_+8); ret_effect(gb);
}

void screenTransitionForestScrambler_hook(GB *gb) {
  BASE(screenTransitionForestScrambler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x2b;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    TAIL(screenTransitionStandard);
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = W8(wActiveRoom);
  CYC(b_+11, b_+13); alu_sub(gb, 0x70);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); alu_and(gb, 0xf0);
  CYC(b_+16, b_+18); alu_swap_a(gb);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+25); alu_and(gb, 0x0f);
  CYC(b_+25, b_+26); alu_add(gb, C);
  CYC(b_+26, b_+27); alu_add(gb, A);
  CYC(b_+27, b_+28); alu_add(gb, A);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+32); A = W8(wScreenTransitionDirection);
  CYC(b_+32, b_+34); alu_and(gb, 0x03);
  CYC(b_+34, b_+35); alu_add(gb, B);
  CYC(b_+35, b_+38); SET_HL(b_+49);
  CYC(b_+38, b_+39); bank1_add_a_to_hl_from_rst(gb, b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+41, b_+44);
    TAIL(screenTransitionStandard);
  }
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+47); W8(wActiveRoom) = A;
  CYC(b_+47, b_+48); alu_scf(gb);
  CYC(b_+48, b_+49); ret_effect(gb);
}

void screenTransitionEyePuzzle_hook(GB *gb) {
  BASE(screenTransitionEyePuzzle);
  uint16_t target;
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wScreenTransitionDirection);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+9); A = W8(wEyePuzzleCorrectDirection);
  CYC(b_+9, b_+10); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    CYC(b_+17, b_+20); SET_HL(wEyePuzzleTransitionCounter);
    CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  } else {
    CYC(b_+10, b_+12);
    CALL_C(b_+12, clearEyePuzzleVars_hook, SYM(clearEyePuzzleVars), b_+15);
    CYC(b_+15, b_+17);
  }
  CYC(b_+21, b_+22); A = B;
  CYC(b_+22, b_+23); bank1_jump_table_from_rst(gb, b_+23);
  target = HL;
  do { uint16_t jt_ = (target);
    if (jt_ == SYM(screenTransitionStandard)) {
      TAIL(screenTransitionStandard);
    }
    else if (jt_ == b_+31) {
      screenTransitionEyePuzzle_up(gb);
      return;
    }
    else if (jt_ == b_+41) {
      screenTransitionEyePuzzle_rightOrLeft(gb);
      return;
    }
    else {
      HANDOFF(target);
      return;
    }
  } while (0);
}

static void screenTransitionEyePuzzle_up(GB *gb) {
  BASE(screenTransitionEyePuzzle);
  CYC(b_+31, b_+34); A = W8(wEyePuzzleTransitionCounter);
  CYC(b_+34, b_+36); alu_cp(gb, 0x06);
  if (F & FC) {
    CYCT(b_+36, b_+38);
    screenTransitionEyePuzzle_rightOrLeft(gb);
    return;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+41);
  screenTransitionStandard_hook(gb);
}

static void screenTransitionEyePuzzle_rightOrLeft(GB *gb) {
  BASE(screenTransitionEyePuzzle);
  CYC(b_+41, b_+42); alu_scf(gb);
  CYC(b_+42, b_+43); ret_effect(gb);
}

void func_60cd_hook(GB *gb) {
  BASE(func_60cd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wScrollMode);
  CYC(b_+8, b_+10); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); A = W8(w1Link_state);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+16, b_+17); ret_effect(gb);
    return;
  }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); A = W8(wTextIsActive);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+22); ret_effect(gb);
    return;
  }
  CYC(b_+21, b_+22);
  CALL_C(b_+22, checkScreenEdgeWarps_hook, SYM(checkScreenEdgeWarps), b_+25);
  if (!(F & FC)) {
    CYCT(b_+25, b_+26); ret_effect(gb);
    return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28);
  TAIL(initiateScreenEdgeWarp);
}

void checkWarpsTopDown_hook(GB *gb) {
  BASE(checkWarpsTopDown);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkTileWarps_hook, SYM(checkTileWarps), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, checkScreenEdgeWarps_hook, SYM(checkScreenEdgeWarps), b_+7);
  if (!(F & FC)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10);
  TAIL(initiateScreenEdgeWarp);
}

void checkWarpsSidescrolling_hook(GB *gb) {
  BASE(checkWarpsSidescrolling);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkScreenEdgeWarps_hook, SYM(checkScreenEdgeWarps), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = W8(wWarpTransition);
  CYC(b_+7, b_+9); alu_or(gb, 0x30);
  CYC(b_+9, b_+12); W8(wWarpTransition) = A;
  CYC(b_+12, b_+14);
  TAIL(initiateWarp);
}

void initiateScreenEdgeWarp_hook(GB *gb) {
  BASE(initiateScreenEdgeWarp);
  CYC(b_+0, b_+3); A = W8(wWarpTransition);
  CYC(b_+3, b_+5); alu_or(gb, 0x10);
  CYC(b_+5, b_+8); W8(wWarpTransition) = A;
  TAIL(initiateWarp);
}

void initiateWarp_hook(GB *gb) {
  BASE(initiateWarp);
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); W8(wScrollMode) = A;
  CYC(b_+5, b_+7); A = 0x1e;
  CYC(b_+7, b_+10); W8(wDisabledObjects) = A;
  CYC(b_+10, b_+12); A = 0x0a;
  CYC(b_+12, b_+15); W8(wLinkForceState) = A;
  CYC(b_+15, b_+17);
  TAIL(warpInitiated);
}

static void checkLinkCloseEnoughToWarpTileCenter_func(GB *gb) {
  BASE(checkLinkCloseEnoughToWarpTileCenter);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_add(gb, B);
  CYC(b_+29, b_+31); alu_and(gb, 0x0f);
  CYC(b_+31, b_+33); alu_sub(gb, 0x04);
  CYC(b_+33, b_+35); alu_cp(gb, 0x0a);
  CYC(b_+35, b_+36); ret_effect(gb);
}

static void checkLinkCloseEnoughToWarpTileCenter_tileSolid(GB *gb) {
  BASE(checkLinkCloseEnoughToWarpTileCenter);
  CYC(b_+25, b_+27); B = 0x02;
  checkLinkCloseEnoughToWarpTileCenter_func(gb);
}

void checkLinkCloseEnoughToWarpTileCenter_hook(GB *gb) {
  BASE(checkLinkCloseEnoughToWarpTileCenter);
  CYC(b_+0, b_+2); H = 0xd0;
  CYC(b_+2, b_+4); A = mem_rd(gb, hFF8D);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+7); B = 0xce;
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); alu_or(gb, A);
  CYC(b_+9, b_+11); L = 0x0b;
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    checkLinkCloseEnoughToWarpTileCenter_tileSolid(gb);
    return;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); B = 0x04;
  CYC(b_+15, b_+18); push_effect(gb, b_+18);
  checkLinkCloseEnoughToWarpTileCenter_func(gb);
  if (!(F & FC)) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); B = 0x00;
  CYC(b_+21, b_+23); L = 0x0d;
  CYC(b_+23, b_+25);
  checkLinkCloseEnoughToWarpTileCenter_func(gb);
}

void warpInitiated_hook(GB *gb) {
  BASE(warpInitiated);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+5, b_+6); alu_scf(gb);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void noWarpInitiated_hook(GB *gb) {
  BASE(noWarpInitiated);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+2); ret_effect(gb);
}

static void check_tile_warps_initiate(GB *gb, uint16_t sp0_) {
  BASE(checkTileWarps);
  CYC(b_+O(38), b_+OE(41)); SET_HL(SYM(findWarpSourceAndDest));
  CYC(b_+O(41), b_+OE(43)); E = 0x04;
  CALL_C(b_+O(43), interBankCall_hook, 0x008a, b_+OE(46));
  CYC(b_+O(46), b_+OE(49)); initiateWarp_hook(gb);
}

static void check_tile_warps_multi_tile_door(GB *gb, uint16_t sp0_) {
  BASE(checkTileWarps);
  CYC(b_+O(65), b_+OE(67)); A = mem_rd(gb, hFF8D);
  CYC(b_+O(67), b_+OE(68)); C = A;
  CYC(b_+O(68), b_+OE(70)); B = 0xce;
  CYC(b_+O(70), b_+OE(71)); A = mem_rd(gb, BC);
  if (game_seasons) {       // the door is two tiles wide unless it is a $0c-type tile
    CYC(b_+S(91), b_+S(93)); alu_cp(gb, 0x0c);
    CYC(b_+S(93), b_+S(95)); B = 0x02;
    if (F & FZ) {
      CYCT(b_+S(95), b_+S(97));
    } else {
      CYC(b_+S(95), b_+S(97));
      CYC(b_+S(97), b_+S(99)); B = 0x04;
    }
  } else {
    CYC(b_+O(71), b_+OE(72)); alu_or(gb, A);
    CYC(b_+O(72), b_+OE(74)); B = 0x02;
    if (!(F & FZ)) {
      CYCT(b_+O(74), b_+OE(76));
    } else {
      CYC(b_+O(74), b_+OE(76));
      CYC(b_+O(76), b_+OE(78)); B = 0x04;
    }
  }
  CYC(b_+O(78), b_+OE(81)); SET_HL(w1Link_yh);
  CYC(b_+O(81), b_+OE(82)); A = mem_rd(gb, HL);
  CYC(b_+O(82), b_+OE(83)); alu_add(gb, B);
  CYC(b_+O(83), b_+OE(85)); alu_and(gb, 0x0f);
  CYC(b_+O(85), b_+OE(87)); alu_sub(gb, 0x04);
  CYC(b_+O(87), b_+OE(89)); alu_cp(gb, 0x0a);
  if (!(F & FC)) {
    CYCT(b_+O(89), b_+OE(90)); ret_effect(gb);
    return;
  }
  CYC(b_+O(89), b_+OE(90));
  CYC(b_+O(90), b_+OE(92));
  check_tile_warps_initiate(gb, sp0_);
}

// checkTileWarps@checkAdjacentTileIsWarpTile: carry set when the tile left or right of hFF8D is
// a warp tile. @@checkIsWarpTile (at +10) ends in checkTileIsWarpTile, whose return pops the
// caller's address.
static void check_tile_warps_is_warp_tile(GB *gb) {
  BASE(checkTileWarps__checkAdjacentTileIsWarpTile);
  C = A;
  B = wRoomLayout >> 8;
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, BC);
  CYCT(b_+14, b_+16);
  checkTileIsWarpTile_hook(gb);
}

static void check_tile_warps_adjacent(GB *gb) {
  BASE(checkTileWarps__checkAdjacentTileIsWarpTile);
  CYC(b_+0, b_+2); A = H8(hFF8D);
  A = alu_inc8(gb, A);
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+6); push_effect(gb, b_+6);
  check_tile_warps_is_warp_tile(gb);
  if (F & FC) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = H8(hFF8D);
  A = alu_dec8(gb, A);
  CYC(b_+9, b_+10);
  check_tile_warps_is_warp_tile(gb);
}

void checkTileWarps_hook(GB *gb) {
  BASE(checkTileWarps);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(3)); A = W8(wLinkObjectIndex);
  CYC(b_+O(3), b_+OE(4)); H = A;
  CYC(b_+O(4), b_+OE(6)); L = 0x0f;
  CYC(b_+O(6), b_+OE(7)); A = mem_rd(gb, HL);
  CYC(b_+O(7), b_+OE(8)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(8), b_+OE(9)); ret_effect(gb);
    return;
  }
  CYC(b_+O(8), b_+OE(9));
  CYC(b_+O(9), b_+OE(12)); A = W8(wMenuDisabled);
  CYC(b_+O(12), b_+OE(13)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(13), b_+OE(15));
    TAIL(noWarpInitiated);
  }
  CYC(b_+O(13), b_+OE(15));
  CYC(b_+O(15), b_+OE(17)); A = mem_rd(gb, hFF8C);
  CALL_C(b_+O(17), checkTileIsWarpTile_hook, SYM(checkTileIsWarpTile), b_+OE(20));
  if (!(F & FC)) {
    CYCT(b_+O(20), b_+OE(22));
    TAIL(noWarpInitiated);
  }
  CYC(b_+O(20), b_+OE(22));
  if (game_seasons) {       // warp tile type 1 (a Subrosia portal): warp at once, dropping everything held
    CYC(b_+S(22), b_+S(23)); A = alu_dec8(gb, A);
    if (F & FZ) {
      CYCT(b_+S(23), b_+S(25));
      CYC(b_+S(52), b_+S(55)); SET_HL(w1Link_zh);
      CYC(b_+S(55), b_+S(56)); A = mem_rd(gb, HL);
      CYC(b_+S(56), b_+S(57)); alu_or(gb, A);
      if (!(F & FZ)) { CYCT(b_+S(57), b_+S(58)); ret_effect(gb); return; }
      CYC(b_+S(57), b_+S(58));
      CALL_C(b_+S(58), clearAllParentItems_hook, SYM(clearAllParentItems), b_+S(61));
      CALL_C(b_+S(61), dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+S(64));
      CALL_C(b_+S(64), resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+S(67));
      CYCT(b_+S(67), b_+S(69));
      check_tile_warps_initiate(gb, sp0_);
      return;
    }
    CYC(b_+S(23), b_+S(25));
  }
  CYC(b_+O(22), b_+OE(25)); A = W8(wLinkGrabState);
  CYC(b_+O(25), b_+OE(26)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(26), b_+OE(28));
    TAIL(noWarpInitiated);
  }
  CYC(b_+O(26), b_+OE(28));
  CYC(b_+O(28), b_+OE(31)); push_effect(gb, b_+OE(31));
  check_tile_warps_adjacent(gb);
  if (F & FC) {
    CYCT(b_+O(31), b_+OE(33));
    check_tile_warps_multi_tile_door(gb, sp0_);
    return;
  }
  CYC(b_+O(31), b_+OE(33));
  CALL_C(b_+O(33), checkLinkCloseEnoughToWarpTileCenter_hook, SYM(checkLinkCloseEnoughToWarpTileCenter), b_+OE(36));
  if (!(F & FC)) {
    CYCT(b_+O(36), b_+OE(38));
    TAIL(noWarpInitiated);
  }
  CYC(b_+O(36), b_+OE(38));
  check_tile_warps_initiate(gb, sp0_);
}

void checkStandingOnDeactivatedWarpTile_hook(GB *gb) {
  BASE(checkStandingOnDeactivatedWarpTile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_scf(gb);
  CYC(b_+1, b_+4); A = W8(wEnteredWarpPosition);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wEnteredWarpPosition);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); A = mem_rd(gb, hFF8D);
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+13, b_+14); ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); B = alu_dec8(gb, B);
  CYC(b_+15, b_+16); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+19); alu_scf(gb);
    CYC(b_+19, b_+20); ret_effect(gb);
    return;
  }
  CYCT(b_+16, b_+18);
  CYC(b_+20, b_+22); A = mem_rd(gb, hFF8C);
  CALL_C(b_+22, checkTileIsWarpTile_hook, SYM(checkTileIsWarpTile), b_+25);
  if (!(F & FC)) {
    CYCT(b_+25, b_+27);
    CYC(b_+18, b_+19); alu_scf(gb);
    CYC(b_+19, b_+20); ret_effect(gb);
    return;
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void checkScreenEdgeWarps_hook(GB *gb) {
  BASE(checkScreenEdgeWarps);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); W8(wTmpcec0) = A;
  CYC(b_+5, b_+8); SET_HL(GV((SYM(screenTransitionState5Substate1__queueRowDmaTransfer) + 32), 0x46a8));
  CYC(b_+8, b_+10); E = 0x04;
  CALL_C(b_+10, interBankCall_hook, 0x008a, b_+13);
  CYC(b_+13, b_+16); A = W8(wTmpcec0);
  CYC(b_+16, b_+18); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+18, b_+21);
    TAIL(noWarpInitiated);
  }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); TAIL(warpInitiated);
}

void checkTileIsWarpTile_hook(GB *gb) {
  BASE(checkTileIsWarpTile);
  CYC(b_+0, b_+3); SET_HL(SYM(warpTileTable));
  CYC(b_+3, b_+6); TAIL(lookupCollisionTable);
}

void cutscene13_hook(GB *gb) {
  BASE(cutscene13);     // shares its address with the tilesetLayoutGroup33 data label in Ages
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(func_03_6103));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_C(b_+8, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+11);
  CYC(b_+11, b_+14); TAIL(updateAllObjects);
}

void tilesetLayoutGroup33_hook(GB *gb) {
  TAIL(cutscene13);
}

void cutscene14_hook(GB *gb) {
  BASE(cutscene14);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(checkInitUnderwaterWaves) + 7));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_C(b_+8, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+11);
  CALL_C(b_+11, updateAllObjects_hook, SYM(updateAllObjects), b_+14);
  CYC(b_+14, b_+17); TAIL(updateStatusBar);
}

void linkSummonedCutscene_hook(GB *gb) {
  BASE(linkSummonedCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_7b93_hook, SYM(func_7b93), b_+3);
  CYC(b_+3, b_+6); TAIL(updateAllObjects);
}

void func_7b93_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, (wThreadStateBuffer + 15));
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+10) { func_7b93__state0_hook(gb); return; }
    else if (jt_ == b_+81) { func_7b93__state1_hook(gb); return; }
    else if (jt_ == b_+110) { func_7b93__state2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void func_7b93__state0_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+10, b_+13); SET_HL((wThreadStateBuffer + 15));
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+14, disableLcd_hook, SYM(disableLcd), b_+17);
  CALL_C(b_+17, clearOam_hook, SYM(clearOam), b_+20);
    CALL_C(b_+20, clearMemoryOnScreenReload_hook, SYM(clearMemoryOnScreenReload), b_+23);
  CALL_C(b_+23, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+26);
  CALL_C(b_+26, loadTilesetData_hook, SYM(loadTilesetData), b_+29);
  CALL_C(b_+29, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+32);
  CALL_C(b_+32, func_131f_hook, SYM(func_131f), b_+35);
  CALL_C(b_+35, loadDungeonLayout_hook, SYM(loadDungeonLayout), b_+38);
  CYC(b_+38, b_+40); A = 0x01;
  CYC(b_+40, b_+43); W8(wScrollMode) = A;
  CALL_C(b_+43, calculateRoomEdge_hook, SYM(calculateRoomEdge), b_+46);
  CALL_C(b_+46, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+49);
  CALL_C(b_+49, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+52);
  CYC(b_+52, b_+54); A = 0x02;
  CALL_C(b_+54, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+57);
  CYC(b_+57, b_+59); A = 0x02;
  CALL_C(b_+59, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+62);
  CYC(b_+62, b_+64); A = 0x10;
  CYC(b_+64, b_+67); W8(wGfxRegs2_LYC) = A;
  CYC(b_+67, b_+69); A = 0x02;
  CYC(b_+69, b_+71); H8(hNextLcdInterruptBehaviour) = A;
  CYC(b_+71, b_+73); A = 0x95;
  CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
  CYC(b_+76, b_+78); A = 0xff;
  CYC(b_+78, b_+81); TAIL(initWaveScrollValues);
}

void func_7b93__state1_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+81, b_+83); A = 0x01;
  CALL_C(b_+83, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+86);
  CYC(b_+86, b_+89); A = W8(wPaletteThread_mode);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+90, b_+91); ret_effect(gb);
    return;
  }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+94); SET_HL((wThreadStateBuffer + 15));
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+95, b_+97); A = 0x81;
  CYC(b_+97, b_+100); W8(wDisabledObjects) = A;
  CYC(b_+100, b_+102); A = 0xff;
  CYC(b_+102, b_+105); W8(wGenericCutscene_cbb4) = A;
  CYC(b_+105, b_+106); alu_xor(gb, A);
  CYC(b_+106, b_+109); W8(wGenericCutscene_cbb3) = A;
  CYC(b_+109, b_+110); ret_effect(gb);
}

void func_7b93__substate0_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+120, b_+122); A = 0x01;
  CALL_C(b_+122, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+125);
  CYC(b_+125, b_+128); SET_HL(wGenericCutscene_cbb4);
  CYC(b_+128, b_+129); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+129, b_+130); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+130, b_+131); A = mem_rd(gb, HL);
  CALL_C(b_+131, initWaveScrollValues_hook, SYM(initWaveScrollValues), b_+134);
  CYC(b_+134, b_+137); A = W8(wGenericCutscene_cbb4);
  CYC(b_+137, b_+139); alu_cp(gb, 0x80);
  if (!(F & FC)) {
    CYCT(b_+139, b_+140); ret_effect(gb);
    return;
  }
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+143); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+143, b_+144); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+144, b_+146); A = 0x03;
  CYC(b_+146, b_+149); mem_wr(gb, w1Link, A);
  CYC(b_+149, b_+151); A = 0x0a;
  CYC(b_+151, b_+154); W8(wLinkForceState) = A;
  CYC(b_+154, b_+156); A = 0x0b;
  CYC(b_+156, b_+159); W8(wWarpTransition) = A;
  CYC(b_+159, b_+160); ret_effect(gb);
}

void func_7b93__substate1_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+160, b_+162); A = 0x01;
  CALL_C(b_+162, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+165);
  CYC(b_+165, b_+168); SET_HL(wGenericCutscene_cbb4);
  CYC(b_+168, b_+169); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+169, b_+171);
  } else {
    CYC(b_+169, b_+171);
    CYC(b_+171, b_+172); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+172, b_+173); A = mem_rd(gb, HL);
  CALL_C(b_+173, initWaveScrollValues_hook, SYM(initWaveScrollValues), b_+176);
  CYC(b_+176, b_+179); A = W8(wGenericCutscene_cbb4);
  CYC(b_+179, b_+180); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+180, b_+181); ret_effect(gb);
    return;
  }
  CYC(b_+180, b_+181);
  CYC(b_+181, b_+184); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+184, b_+185); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+185, b_+187); A = 0x03;
  CYC(b_+187, b_+189); H8(hNextLcdInterruptBehaviour) = A;
  CYC(b_+189, b_+190); ret_effect(gb);
}

void func_7b93__substate2_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+190, b_+192); A = 0x02;
  CYC(b_+192, b_+195); mem_wr(gb, (wThreadStateBuffer + 14), A);
  CYC(b_+195, b_+196); alu_xor(gb, A);
  CYC(b_+196, b_+199); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+199, b_+202); W8(wDisabledObjects) = A;
  CYC(b_+202, b_+204); A = 0x21;
  CALL_C(b_+204, setGlobalFlag_hook, SYM(setGlobalFlag), b_+207);
  CYC(b_+207, b_+210); TAIL(initializeRoom);
}

void func_7b93__state2_hook(GB *gb) {
  BASE(func_7b93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+110, b_+113); A = W8(wGenericCutscene_cbb3);
  CYC(b_+113, b_+114); bank1_jump_table_from_rst(gb, b_+114);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+120) { func_7b93__substate0_hook(gb); return; }
    else if (jt_ == b_+160) { func_7b93__substate1_hook(gb); return; }
    else if (jt_ == b_+190) { func_7b93__substate2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void updateSeedTreeRefillData_hook(GB *gb) {
  BASE(updateSeedTreeRefillData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+10); hram_wr(gb, 0x70, A);
  CYC(b_+10, b_+13); SET_HL(SYM(seedTreeRefillLocations));
  CYC(b_+13, b_+15); B = 0x10;
  for (;;) {
    CYC(b_+15, b_+16); push_effect(gb, BC);
    CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+17, b_+18); C = A;
    CYC(b_+18, b_+19); A = mem_rd(gb, HL);
    CYC(b_+19, b_+20); E = A;
    CALL_C(b_+20, checkSeedTreeRefillIndex_hook, SYM(checkSeedTreeRefillIndex), b_+23);
    CYC(b_+23, b_+24); SET_HL(HL + 1);
    CYC(b_+24, b_+25); SET_BC(pop_effect(gb));
    CYC(b_+25, b_+26); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+26, b_+28); continue; }
    CYC(b_+26, b_+28);
    break;
  }
  CYC(b_+28, b_+29); alu_xor(gb, A);
  CYC(b_+29, b_+31); hram_wr(gb, 0x70, A);
  CYC(b_+31, b_+32); ret_effect(gb);
}

void checkSeedTreeRefillIndex__addRoom_hook(GB *gb) {
  BASE(checkSeedTreeRefillIndex);
  CYC(b_+55, b_+56); A = B;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+58); ret_effect(gb);
}

static void checkSeedTreeRefillIndex_finish(GB *gb, uint16_t sp0_) {
  BASE(checkSeedTreeRefillIndex);
  CYC(b_+85, b_+86); SET_DE(pop_effect(gb));
  CYC(b_+86, b_+87); L = E;
  CYC(b_+87, b_+88); H = D;
  CYC(b_+88, b_+90); B = 0x08;
  CALL_C(b_+90, clearMemory_hook, SYM(clearMemory), b_+93);
  CYC(b_+93, b_+94); SET_HL(pop_effect(gb));
  CYC(b_+94, b_+95); ret_effect(gb);
}

void checkSeedTreeRefillIndex__treeScreen_hook(GB *gb) {
  BASE(checkSeedTreeRefillIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+58, b_+59); push_effect(gb, HL);
  CYC(b_+59, b_+60); push_effect(gb, DE);
  CYC(b_+60, b_+62); C = 0x08;
  for (;;) {
    CYC(b_+62, b_+63); A = mem_rd(gb, DE);
    CYC(b_+63, b_+64); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+64, b_+66);
      break;
    }
    CYC(b_+64, b_+66);
    CYC(b_+66, b_+67); E = alu_inc8(gb, E);
    CYC(b_+67, b_+68); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+68, b_+70); continue; }
    CYC(b_+68, b_+70);
    CYC(b_+70, b_+71); alu_or(gb, D);
    break;
  }
  if (F & FZ) {
    CYCT(b_+71, b_+73);
  } else {
    CYC(b_+71, b_+73);
    CYC(b_+73, b_+75); A = mem_rd(gb, hFF8D);
    CYC(b_+75, b_+76); B = A;
    CYC(b_+76, b_+78); A = 0x10;
    CYC(b_+78, b_+79); alu_sub(gb, B);
    CYC(b_+79, b_+82); SET_HL(wSeedTreeRefilledBitset);
    CALL_C(b_+82, setFlag_hook, SYM(setFlag), b_+85);
  }
  checkSeedTreeRefillIndex_finish(gb, sp0_);
}

void checkSeedTreeRefillIndex_hook(GB *gb) {
  BASE(checkSeedTreeRefillIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); mem_wr(gb, hFF8D, A);
  CYC(b_+3, b_+4); A = E;
  CYC(b_+4, b_+6); E &= (uint8_t)~1;
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+12); A = mem_rd(gb, wActiveGroup);
  CYC(b_+12, b_+13); alu_cp(gb, B);
  CYC(b_+13, b_+15); D = 0xd9;
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
  } else {
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+20); A = mem_rd(gb, wActiveRoom);
    CYC(b_+20, b_+21); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+21, b_+23);
      TAIL(checkSeedTreeRefillIndex__treeScreen);
    }
    CYC(b_+21, b_+23);
  }
  CYC(b_+23, b_+25); A = mem_rd(gb, hFF8D);
  CYC(b_+25, b_+26); B = A;
  CYC(b_+26, b_+28); A = 0x10;
  CYC(b_+28, b_+29); alu_sub(gb, B);
  CYC(b_+29, b_+30); push_effect(gb, HL);
  CYC(b_+30, b_+33); SET_HL(wSeedTreeRefilledBitset);
  CALL_C(b_+33, checkFlag_hook, SYM(checkFlag), b_+36);
  CYC(b_+36, b_+37); SET_HL(pop_effect(gb));
  if (!(F & FZ)) {
    CYCT(b_+37, b_+38); ret_effect(gb);
    return;
  }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); A = mem_rd(gb, wActiveRoom);
  CYC(b_+41, b_+42); B = A;
  CYC(b_+42, b_+44); C = 0x08;
  for (;;) {
    CYC(b_+44, b_+45); A = mem_rd(gb, DE);
    CYC(b_+45, b_+46); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+46, b_+48);
      TAIL(checkSeedTreeRefillIndex__addRoom);
    }
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+49); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(b_+49, b_+50); ret_effect(gb);
      return;
    }
    CYC(b_+49, b_+50);
    CYC(b_+50, b_+51); E = alu_inc8(gb, E);
    CYC(b_+51, b_+52); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+52, b_+54); continue; }
    CYC(b_+52, b_+54);
    CYC(b_+54, b_+55); ret_effect(gb);
    return;
  }
}

void initializeSeedTreeRefillData_hook(GB *gb) {
  BASE(initializeSeedTreeRefillData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wSeedTreeRefilledBitset);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0xf0);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xff);
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+12); hram_wr(gb, 0x70, A);
  CYC(b_+12, b_+15); SET_HL(wxSeedTreeRefillData);
  CYC(b_+15, b_+17); B = 0x80;
  CALL_C(b_+17, clearMemory_hook, SYM(clearMemory), b_+20);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+23); hram_wr(gb, 0x70, A);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void func_60e9_hook(GB *gb) {
  BASE(func_60e9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, func_60cd_hook, SYM(func_60cd), b_+8);
  if (F & FC) {
    CYCT(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wLinkInAir);
  CYC(b_+12, b_+14); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, wWarpsDisabled);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+20); ret_effect(gb);
    return;
  }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_state);
  CYC(b_+23, b_+25); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+25, b_+26); ret_effect(gb);
    return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); A = mem_rd(gb, wTextIsActive);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+31); ret_effect(gb);
    return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wDisableWarpTiles);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+35, b_+36); ret_effect(gb);
    return;
  }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+39); SET_HL(w1Link_yh);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+40, b_+42); alu_add(gb, 0x04);
  CYC(b_+42, b_+43); B = A;
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); C = mem_rd(gb, HL);
  CALL_C(b_+45, getTileAtPosition_hook, SYM(getTileAtPosition), b_+48);
  CYC(b_+48, b_+50); mem_wr(gb, hFF8C, A);
  CYC(b_+50, b_+51); B = A;
  CYC(b_+51, b_+52); A = L;
  CYC(b_+52, b_+54); mem_wr(gb, hFF8D, A);
  CYC(b_+54, b_+57); A = mem_rd(gb, wScrollMode);
  CYC(b_+57, b_+59); alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+61);
  } else {
    CYC(b_+59, b_+61);
    CALL_C(b_+61, checkStandingOnDeactivatedWarpTile_hook, SYM(checkStandingOnDeactivatedWarpTile), b_+64);
    if (!(F & FC)) {
      CYCT(b_+64, b_+65); ret_effect(gb);
      return;
    }
    CYC(b_+64, b_+65);
  }
  CYC(b_+65, b_+67); A = 0xff;
  CYC(b_+67, b_+70); mem_wr(gb, wEnteredWarpPosition, A);
  CYC(b_+70, b_+73); A = mem_rd(gb, wActiveGroup);
  CYC(b_+73, b_+74); bank1_jump_table_from_rst(gb, b_+74);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(checkWarpsTopDown) && hook_is(gb, SYM(checkWarpsTopDown), checkWarpsTopDown_hook)) { checkWarpsTopDown_hook(gb); return; }
    else if (jt_ == SYM(checkWarpsSidescrolling) && hook_is(gb, SYM(checkWarpsSidescrolling), checkWarpsSidescrolling_hook)) { checkWarpsSidescrolling_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void updateLastToggleBlocksState_hook(GB *gb) {
  BASE(updateLastToggleBlocksState);
  CYC(b_+0, b_+3); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+3, b_+6); mem_wr(gb, wLastToggleBlocksState, A);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void checkUpdateToggleBlocks_hook(GB *gb) {
  BASE(checkUpdateToggleBlocks);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkDungeonUsesToggleBlocks_hook, SYM(checkDungeonUsesToggleBlocks), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+11); A = mem_rd(gb, wLastToggleBlocksState);
  CYC(b_+11, b_+12); alu_xor(gb, B);
  CYC(b_+12, b_+13); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+13, b_+14); ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); A = 0x02;
  CYC(b_+16, b_+19); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+19, b_+20); ret_effect(gb);
}

static void cutscene1f_after_call_7f1d_body(GB *gb, uint16_t sp0_) {
  BASE(cutscene1f);
  CALL_C(b_+8, updateStatusBar_hook, SYM(updateStatusBar), b_+11);
  CYC(b_+11, b_+14);
  updateAllObjects_hook(gb);
}

void cutscene1f_hook(GB *gb) {
  BASE(cutscene1f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(cutscene02__state0) + 26));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  cutscene1f_after_call_7f1d_body(gb, sp0_);
}

void cutscene1f__afterCall7f1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  cutscene1f_after_call_7f1d_body(gb, sp0_);
}

void paletteFadeHandler09_hook(GB *gb) {
  BASE(paletteFadeHandler09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_decCounter_hook, SYM(paletteThread_decCounter), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(paletteFadeHandler01);
}

void paletteFadeHandler01_hook(GB *gb) {
  BASE(paletteFadeHandler01);
  CYC(b_+0, b_+2); A = 0x1f;
  CYC(b_+2, b_+4); mem_wr(gb, hFF8B, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wPaletteThread_speed);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); A = mem_rd(gb, (wThreadStateBuffer + 31));
  CYC(b_+11, b_+12); alu_add(gb, C);
  CYC(b_+12, b_+14); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(b_+14, b_+17);
    TAIL(paletteThread_stop);
  }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+20); mem_wr(gb, (wThreadStateBuffer + 31), A);
  CYC(b_+20, b_+21); C = A;
  TAIL(updateFadingPalettes);
}

void paletteFadeHandler00_hook(GB *gb) {
  BASE(paletteFadeHandler00);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void paletteFadeHandler0a_hook(GB *gb) {
  BASE(paletteFadeHandler0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_decCounter_hook, SYM(paletteThread_decCounter), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(paletteFadeHandler02);
}

void paletteFadeHandler02_hook(GB *gb) {
  BASE(paletteFadeHandler02);
  CYC(b_+0, b_+2); A = 0x1f;
  CYC(b_+2, b_+4); mem_wr(gb, hFF8B, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wPaletteThread_speed);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); A = mem_rd(gb, (wThreadStateBuffer + 31));
  CYC(b_+11, b_+12); alu_sub(gb, C);
  if (F & FC) {
    CYCT(b_+12, b_+14);
    TAIL(paletteThread_stop);
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); mem_wr(gb, (wThreadStateBuffer + 31), A);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20);
  TAIL(updateFadingPalettes);
}

void paletteFadeHandler0b_hook(GB *gb) {
  BASE(paletteFadeHandler0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_decCounter_hook, SYM(paletteThread_decCounter), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(paletteFadeHandler03);
}

void paletteFadeHandler03_hook(GB *gb) {
  BASE(paletteFadeHandler03);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hFF8B, A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wPaletteThread_speed);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, (wThreadStateBuffer + 31));
  CYC(b_+10, b_+11); alu_sub(gb, C);
  CYC(b_+11, b_+13); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(b_+13, b_+15);
    TAIL(paletteThread_stop);
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); mem_wr(gb, (wThreadStateBuffer + 31), A);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21);
  TAIL(updateFadingPalettes);
}

void paletteFadeHandler0c_hook(GB *gb) {
  BASE(paletteFadeHandler0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_decCounter_hook, SYM(paletteThread_decCounter), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(paletteFadeHandler04);
}

void paletteFadeHandler04_hook(GB *gb) {
  BASE(paletteFadeHandler04);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hFF8B, A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wPaletteThread_speed);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, (wThreadStateBuffer + 31));
  CYC(b_+10, b_+11); alu_add(gb, C);
  if (F & FC) {
    CYCT(b_+11, b_+13);
    TAIL(paletteThread_stop);
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); mem_wr(gb, (wThreadStateBuffer + 31), A);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+20);
  TAIL(updateFadingPalettes);
}

void paletteThread_setFadeOffsetAndStop_hook(GB *gb) {
  BASE(paletteThread_setFadeOffsetAndStop);
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); alu_sub(gb, 0x1f);
  CYC(b_+3, b_+6); mem_wr(gb, (wThreadStateBuffer + 31), A);
  TAIL(paletteThread_stop);
}

void paletteThread_stop_hook(GB *gb) {
  BASE(paletteThread_stop);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wPaletteThread_updateRate, A);
  CYC(b_+4, b_+7); mem_wr(gb, wPaletteThread_mode, A);
  CYC(b_+7, b_+10); TAIL(clearPaletteFadeVariables);
}

void paletteThread_refreshPalettesAndStop_hook(GB *gb) {
  BASE(paletteThread_refreshPalettesAndStop);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wPaletteThread_updateRate, A);
  CYC(b_+4, b_+7); mem_wr(gb, wPaletteThread_mode, A);
  CYC(b_+7, b_+10); TAIL(clearPaletteFadeVariablesAndRefreshPalettes);
}

void paletteFadeHandler0d_hook(GB *gb) {
  BASE(paletteFadeHandler0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_decCounter_hook, SYM(paletteThread_decCounter), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(paletteFadeHandler05);
}

void paletteFadeHandler05_hook(GB *gb) {
  BASE(paletteFadeHandler05);
  CYC(b_+O(0), b_+OE(1)); alu_xor(gb, A);
  CYC(b_+O(1), b_+OE(3)); mem_wr(gb, hFF8B, A);
  if (game_seasons) {      // Seasons steps the fade offset by one instead of the thread speed
    CYC(b_+S(3), b_+S(6)); A = mem_rd(gb, wPaletteThread_parameter);
    CYC(b_+S(6), b_+S(7)); A = alu_dec8(gb, A);
    CYC(b_+S(7), b_+S(8)); B = A;
    CYC(b_+S(8), b_+S(11)); A = mem_rd(gb, (wThreadStateBuffer + 31));
    CYC(b_+S(11), b_+S(12)); A = alu_dec8(gb, A);
    CYC(b_+S(12), b_+S(13)); alu_cp(gb, B);
    if (F & FZ) { CYCT(b_+S(13), b_+S(15)); TAIL(paletteThread_stop); }
    CYC(b_+S(13), b_+S(15));
  } else {
    CYC(b_+3, b_+6); A = mem_rd(gb, wPaletteThread_speed);
    CYC(b_+6, b_+7); C = A;
    CYC(b_+7, b_+10); A = mem_rd(gb, wPaletteThread_parameter);
    CYC(b_+10, b_+11); A = alu_dec8(gb, A);
    CYC(b_+11, b_+12); B = A;
    CYC(b_+12, b_+15); A = mem_rd(gb, (wThreadStateBuffer + 31));
    CYC(b_+15, b_+16); alu_sub(gb, C);
    CYC(b_+16, b_+17); alu_cp(gb, B);
    if (F & FZ) { CYCT(b_+17, b_+19); TAIL(paletteThread_stop); }
    CYC(b_+17, b_+19);
    if (F & FC) { CYCT(b_+19, b_+21); TAIL(paletteThread_stop); }
    CYC(b_+19, b_+21);
  }
  CYC(b_+O(21), b_+OE(24)); mem_wr(gb, (wThreadStateBuffer + 31), A);
  CYC(b_+O(24), b_+OE(25)); C = A;
  CYC(b_+O(25), b_+OE(28)); TAIL(updateFadingPalettes);
}

void paletteFadeHandler0e_hook(GB *gb) {
  BASE(paletteFadeHandler0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_decCounter_hook, SYM(paletteThread_decCounter), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(paletteFadeHandler06);
}

void paletteFadeHandler06_hook(GB *gb) {
  BASE(paletteFadeHandler06);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hFF8B, A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wPaletteThread_speed);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, wPaletteThread_parameter);
  CYC(b_+10, b_+12); alu_add(gb, 0x1f);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+16); A = mem_rd(gb, (wThreadStateBuffer + 31));
  CYC(b_+16, b_+18); alu_add(gb, 0x1f);
  CYC(b_+18, b_+19); alu_add(gb, C);
  CYC(b_+19, b_+20); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+20, b_+22);
    TAIL(paletteThread_stop);
  }
  CYC(b_+20, b_+22);
  if (!(F & FC)) {
    CYCT(b_+22, b_+25);
    TAIL(paletteThread_setFadeOffsetAndStop);
  }
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+27); alu_sub(gb, 0x1f);
  CYC(b_+27, b_+30); mem_wr(gb, (wThreadStateBuffer + 31), A);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+34); TAIL(updateFadingPalettes);
}

void paletteFadeHandler07_hook(GB *gb) {
  BASE(paletteFadeHandler07);
  CYC(b_+0, b_+2); A = 0x1f;
  CYC(b_+2, b_+4); mem_wr(gb, hFF8B, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wPaletteThread_speed);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); A = mem_rd(gb, (wThreadStateBuffer + 31));
  CYC(b_+11, b_+12); alu_sub(gb, C);
  if (!(F & FC)) {
    CYC(b_+12, b_+14);
    CYC(b_+14, b_+17); mem_wr(gb, (wThreadStateBuffer + 31), A);
    CYC(b_+17, b_+18); C = A;
    CYC(b_+18, b_+21); updateFadingPalettes_hook(gb);
    return;
  }
  CYCT(b_+12, b_+14);
  CYC(b_+21, b_+23); A = 0xff;
  CYC(b_+23, b_+25); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+25, b_+27); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+27, b_+30); A = mem_rd(gb, wPaletteThread_parameter);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
    TAIL(paletteThread_refreshPalettesAndStop);
  }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+38); mem_wr(gb, wPaletteThread_parameter, A);
  CYC(b_+38, b_+39); A = B;
  CYC(b_+39, b_+41); alu_cp(gb, 0xf0);
  if (F & FZ) {
    CYCT(b_+41, b_+44);
    TAIL(darkenRoom);
  }
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+47); TAIL(darkenRoomLightly);
}

void palette_fade_handler08_body_hook(GB *gb) {
  BASE(paletteFadeHandler08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+15); mem_wr(gb, hFF91, A);
  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+18); alu_sub(gb, B);
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+22); mem_wr(gb, hFF90, A);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+24, b_+27);
    TAIL(paletteThread_mixBG234Palettes);
  }
  CYC(b_+24, b_+27);
  CALL_C(b_+27, paletteThread_mixBG567Palettes_hook, SYM(paletteThread_mixBG567Palettes), b_+30);
  CYC(b_+30, b_+32); A = mem_rd(gb, hDirtyBgPalettes);
  CYC(b_+32, b_+34); alu_or(gb, 0xfc);
  CYC(b_+34, b_+36); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+36, b_+38); A = 0xfc;
  CYC(b_+38, b_+40); mem_wr(gb, hBgPaletteSources, A);
  CYC(b_+40, b_+41); ret_effect(gb);
}

void paletteFadeHandler08_hook(GB *gb) {
  BASE(paletteFadeHandler08);
  CYC(b_+0, b_+3); SET_HL((wThreadStateBuffer + 31));
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    TAIL(paletteFadeHandler08__stop);
  }
  CYC(b_+4, b_+6);
  palette_fade_handler08_body_hook(gb);
}

void paletteFadeHandler08__seasonsFunc_01_5816_hook(GB *gb) {
  palette_fade_handler08_body_hook(gb);
}

void paletteFadeHandler08__stop_hook(GB *gb) {
  BASE(paletteFadeHandler08);
  CYC(b_+41, b_+44); TAIL(paletteThread_stop);
}

static void palette_thread_calculate_next_color(GB *gb) {
  BASE(paletteThread_calculateFadingPalettes);
  for (;;) {
    CYC(b_+5, b_+6); E = mem_rd(gb, HL);
    CYC(b_+6, b_+7); L = alu_inc8(gb, L);
    CYC(b_+7, b_+8); A = mem_rd(gb, HL);
    CYC(b_+8, b_+10); E = alu_sla(gb, E);
    CYC(b_+10, b_+11); alu_rla(gb);
    CYC(b_+11, b_+13); E = alu_rl(gb, E);
    CYC(b_+13, b_+14); alu_rla(gb);
    CYC(b_+14, b_+16); E = alu_rl(gb, E);
    CYC(b_+16, b_+17); alu_rla(gb);
    CYC(b_+17, b_+19); alu_and(gb, 0x1f);
    CYC(b_+19, b_+20); alu_add(gb, C);
    CYC(b_+20, b_+22); alu_bit(gb, 5, A);
    if (F & FZ) CYCT(b_+22, b_+24);
    else { CYC(b_+22, b_+24); CYC(b_+24, b_+26); A = mem_rd(gb, hFF8B); }
    CYC(b_+26, b_+28); E = 0;
    CYC(b_+28, b_+30); A = alu_srl(gb, A);
    CYC(b_+30, b_+32); E = alu_rr(gb, E);
    CYC(b_+32, b_+33); alu_rra(gb);
    CYC(b_+33, b_+35); E = alu_rr(gb, E);
    CYC(b_+35, b_+36); alu_rra(gb);
    CYC(b_+36, b_+38); E = alu_rr(gb, E);
    CYC(b_+38, b_+39); D = A;
    CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+40, b_+41); alu_rra(gb);
    CYC(b_+41, b_+42); alu_rra(gb);
    CYC(b_+42, b_+44); alu_and(gb, 0x1f);
    CYC(b_+44, b_+45); alu_add(gb, C);
    CYC(b_+45, b_+47); alu_bit(gb, 5, A);
    if (F & FZ) CYCT(b_+47, b_+49);
    else { CYC(b_+47, b_+49); CYC(b_+49, b_+51); A = mem_rd(gb, hFF8B); }
    CYC(b_+51, b_+52); alu_rlca(gb);
    CYC(b_+52, b_+53); alu_rlca(gb);
    CYC(b_+53, b_+54); alu_or(gb, D);
    CYC(b_+54, b_+55); D = A;
    CYC(b_+55, b_+56); A = mem_rd(gb, HL);
    CYC(b_+56, b_+58); alu_and(gb, 0x1f);
    CYC(b_+58, b_+59); alu_add(gb, C);
    CYC(b_+59, b_+61); alu_bit(gb, 5, A);
    if (F & FZ) CYCT(b_+61, b_+63);
    else { CYC(b_+61, b_+63); CYC(b_+63, b_+65); A = mem_rd(gb, hFF8B); }
    CYC(b_+65, b_+66); alu_or(gb, E);
    CYC(b_+66, b_+67); H = alu_inc8(gb, H);
    CYC(b_+67, b_+68); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+68, b_+69); mem_wr(gb, HL, D);
    CYC(b_+69, b_+70); L = alu_inc8(gb, L);
    CYC(b_+70, b_+71); H = alu_dec8(gb, H);
    CYC(b_+71, b_+72); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+72, b_+74); continue; }
    CYC(b_+72, b_+74);
    CYC(b_+74, b_+75); ret_effect(gb);
    return;
  }
}

void paletteThread_calculateFadingPalettes_hook(GB *gb) {
  BASE(paletteThread_calculateFadingPalettes);
  CYC(b_+0, b_+3); SET_HL(w2TilesetBgPalettes);
  CYC(b_+3, b_+5); B = 0x40;
  palette_thread_calculate_next_color(gb);
}

void paletteThread_calculateFadingPalettes__nextColor_hook(GB *gb) {
  palette_thread_calculate_next_color(gb);
}

void paletteThread_mixBG567Palettes__mixColors_hook(GB *gb) {
  BASE(paletteThread_mixBG567Palettes);
  for (;;) {
    CYC(b_+73, b_+75); A = mem_rd(gb, hFF91);
    CYC(b_+75, b_+76); H = A;
    CYC(b_+76, b_+78); D = 0xda;
    CYC(b_+78, b_+79); A = mem_rd(gb, DE);
    CYC(b_+79, b_+80); C = A;
    CYC(b_+80, b_+82); B = 0;
    CYC(b_+82, b_+83); L = B;
    CYC(b_+83, b_+85); A = 4;
    for (;;) {
      CYC(b_+85, b_+86); alu_add_hl(gb, HL);
      if (!(F & FC)) CYCT(b_+86, b_+88);
      else { CYC(b_+86, b_+88); CYC(b_+88, b_+89); alu_add_hl(gb, BC); }
      CYC(b_+89, b_+90); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+90, b_+92); continue; }
      CYC(b_+90, b_+92);
      break;
    }
    CYC(b_+92, b_+93); push_effect(gb, HL);
    CYC(b_+93, b_+95); A = mem_rd(gb, hFF90);
    CYC(b_+95, b_+96); H = A;
    CYC(b_+96, b_+98); D = 0xdb;
    CYC(b_+98, b_+99); A = mem_rd(gb, DE);
    CYC(b_+99, b_+100); C = A;
    CYC(b_+100, b_+102); B = 0;
    CYC(b_+102, b_+103); L = B;
    CYC(b_+103, b_+105); A = 4;
    for (;;) {
      CYC(b_+105, b_+106); alu_add_hl(gb, HL);
      if (!(F & FC)) CYCT(b_+106, b_+108);
      else { CYC(b_+106, b_+108); CYC(b_+108, b_+109); alu_add_hl(gb, BC); }
      CYC(b_+109, b_+110); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+110, b_+112); continue; }
      CYC(b_+110, b_+112);
      break;
    }
    CYC(b_+112, b_+113); SET_BC(pop_effect(gb));
    CYC(b_+113, b_+114); alu_add_hl(gb, BC);
    CYC(b_+114, b_+115); ret_effect(gb);
    return;
  }
}

void paletteThread_mixBG567Palettes__writeToFadingBgPalettes_hook(GB *gb) {
  BASE(paletteThread_mixBG567Palettes);
  CYC(b_+115, b_+116); H = alu_inc8(gb, H);
  CYC(b_+116, b_+118); A = mem_rd(gb, hFF8B);
  CYC(b_+118, b_+120); C = 0;
  CYC(b_+120, b_+122); A = alu_srl(gb, A);
  CYC(b_+122, b_+124); C = alu_rr(gb, C);
  CYC(b_+124, b_+125); alu_rra(gb);
  CYC(b_+125, b_+127); C = alu_rr(gb, C);
  CYC(b_+127, b_+128); alu_rra(gb);
  CYC(b_+128, b_+130); C = alu_rr(gb, C);
  CYC(b_+130, b_+131); B = A;
  CYC(b_+131, b_+133); A = mem_rd(gb, hFF8C);
  CYC(b_+133, b_+134); alu_or(gb, C);
  CYC(b_+134, b_+135); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+135, b_+137); A = mem_rd(gb, hFF8D);
  CYC(b_+137, b_+138); alu_rlca(gb);
  CYC(b_+138, b_+139); alu_rlca(gb);
  CYC(b_+139, b_+140); alu_or(gb, B);
  CYC(b_+140, b_+141); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+141, b_+142); H = alu_dec8(gb, H);
  CYC(b_+142, b_+143); ret_effect(gb);
}

void palette_thread_mix_next_color_hook(GB *gb) {
  BASE(paletteThread_mixBG567Palettes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+11, b_+12); push_effect(gb, BC);
    CYC(b_+12, b_+13); push_effect(gb, HL);
    CALL_C(b_+13, paletteThread_mixBG567Palettes__mixColors_hook, b_+73, b_+16);
    CYC(b_+16, b_+17); E = alu_inc8(gb, E);
    CYC(b_+17, b_+19); L = alu_swap(gb, L);
    CYC(b_+19, b_+20); A = L;
    CYC(b_+20, b_+22); alu_and(gb, 0x0f);
    CYC(b_+22, b_+23); L = A;
    CYC(b_+23, b_+24); A = H;
    CYC(b_+24, b_+26); A = alu_swap(gb, A);
    CYC(b_+26, b_+27); alu_or(gb, L);
    CYC(b_+27, b_+29); mem_wr(gb, hFF8B, A);
    CALL_C(b_+29, paletteThread_mixBG567Palettes__mixColors_hook, b_+73, b_+32);
    CYC(b_+32, b_+33); E = alu_inc8(gb, E);
    CYC(b_+33, b_+35); L = alu_swap(gb, L);
    CYC(b_+35, b_+36); A = L;
    CYC(b_+36, b_+38); alu_and(gb, 0x0f);
    CYC(b_+38, b_+39); L = A;
    CYC(b_+39, b_+40); A = H;
    CYC(b_+40, b_+42); A = alu_swap(gb, A);
    CYC(b_+42, b_+43); alu_or(gb, L);
    CYC(b_+43, b_+45); mem_wr(gb, hFF8D, A);
    CALL_C(b_+45, paletteThread_mixBG567Palettes__mixColors_hook, b_+73, b_+48);
    CYC(b_+48, b_+49); E = alu_inc8(gb, E);
    CYC(b_+49, b_+51); L = alu_swap(gb, L);
    CYC(b_+51, b_+52); A = L;
    CYC(b_+52, b_+54); alu_and(gb, 0x0f);
    CYC(b_+54, b_+55); L = A;
    CYC(b_+55, b_+56); A = H;
    CYC(b_+56, b_+58); A = alu_swap(gb, A);
    CYC(b_+58, b_+59); alu_or(gb, L);
    CYC(b_+59, b_+61); mem_wr(gb, hFF8C, A);
    CYC(b_+61, b_+62); SET_HL(pop_effect(gb));
    CALL_C(b_+62, paletteThread_mixBG567Palettes__writeToFadingBgPalettes_hook, b_+115, b_+65);
    CYC(b_+65, b_+66); SET_BC(pop_effect(gb));
    CYC(b_+66, b_+67); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+67, b_+69); continue; }
    CYC(b_+67, b_+69);
    CYC(b_+69, b_+70); alu_xor(gb, A);
    CYC(b_+70, b_+72); hram_wr(gb, 0x70, A);
    CYC(b_+72, b_+73); ret_effect(gb);
    return;
  }
}

static void palette_thread_mix_body(GB *gb) {
  BASE(paletteThread_mixBG567Palettes);
  CYC(b_+7, b_+9); A = 0x02;
  CYC(b_+9, b_+11); hram_wr(gb, 0x70, A);
  palette_thread_mix_next_color_hook(gb);
}

void paletteThread_mixBG234Palettes_hook(GB *gb) {
  BASE(paletteThread_mixBG234Palettes);
  CYC(b_+0, b_+3); SET_HL((w2TilesetBgPalettes + 16));
  CYC(b_+3, b_+5); E = 0;
  CYC(b_+5, b_+7); B = 0x0c;
  CYC(b_+7, b_+9);
  palette_thread_mix_body(gb);
}

void paletteThread_mixBG567Palettes_hook(GB *gb) {
  BASE(paletteThread_mixBG567Palettes);
  CYC(b_+0, b_+3); SET_HL((w2TilesetBgPalettes + 40));
  CYC(b_+3, b_+5); E = 0x24;
  CYC(b_+5, b_+7); B = 0x0c;
  palette_thread_mix_body(gb);
}

void paletteThread_mixBG567Palettes__nextColor_hook(GB *gb) {
  palette_thread_mix_next_color_hook(gb);
}

void paletteThread_decCounter_hook(GB *gb) {
  BASE(paletteThread_decCounter);
  CYC(b_+0, b_+3); SET_HL(wPaletteThread_counter);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wPaletteThread_counterRefill);
  CYC(b_+8, b_+11); mem_wr(gb, wPaletteThread_counter, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void incrementScreenScrollRowVars_hook(GB *gb) {
  BASE(incrementScreenScrollRowVars);
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenScrollRow);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wScreenScrollDirection);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); alu_add(gb, B);
  CYC(b_+9, b_+11); alu_and(gb, 0x1f);
  CYC(b_+11, b_+14); mem_wr(gb, wScreenScrollRow, A);
  CYC(b_+14, b_+17); A = mem_rd(gb, wScreenScrollVramRow);
  CYC(b_+17, b_+18); alu_add(gb, C);
  CYC(b_+18, b_+20); alu_and(gb, 0x1f);
  CYC(b_+20, b_+23); mem_wr(gb, wScreenScrollVramRow, A);
  CYC(b_+23, b_+26); A = mem_rd(gb, wScreenScrollCounter);
  CYC(b_+26, b_+27); A = alu_dec8(gb, A);
  CYC(b_+27, b_+30); mem_wr(gb, wScreenScrollCounter, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void addFunctionsToVBlankQueue_hook(GB *gb) {
  BASE(addFunctionsToVBlankQueue);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); C = 0x01;
  CYC(b_+3, b_+5); E = 0x60;
  CALL_C(b_+5, addFunctionsToVBlankQueue__locFunc_hook, b_+12, b_+8);
  CYC(b_+8, b_+10); C = 0x00;
  CYC(b_+10, b_+12); E = 0x40;
  TAIL(addFunctionsToVBlankQueue__locFunc);
}

void addFunctionsToVBlankQueue__locFunc_hook(GB *gb) {
  BASE(addFunctionsToVBlankQueue);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+12, b_+14); A = mem_rd(gb, hVBlankFunctionQueueTail);
  CYC(b_+14, b_+15); L = A;
  CYC(b_+15, b_+17); H = 0xc4;
  CYC(b_+17, b_+20); A = mem_rd(gb, SYM(vblankRunBank4FunctionOffset));
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); A = C;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); A = E;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); A = B;
  CYC(b_+26, b_+29); SET_DE(SYM(data_0bfd));
  CALL_C(b_+29, addDoubleIndexToDe_hook, 0x0072, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+34, b_+35); SET_DE(DE + 1);
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+37, b_+38); A = L;
  CYC(b_+38, b_+40); mem_wr(gb, hVBlankFunctionQueueTail, A);
  CYC(b_+40, b_+41); ret_effect(gb);
}

void func_46ca_hook(GB *gb) {
  BASE(func_46ca);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenOffsetY);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); alu_rrca(gb);
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+11); SET_HL(SYM(vramBgMapTable));
  CYC(b_+11, b_+12); bank1_add_a_to_hl_from_rst(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); alu_add(gb, E);
  CYC(b_+14, b_+15); E = A;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_add(gb, 0x40);
  CYC(b_+18, b_+19); D = A;
  CYC(b_+19, b_+21); A = hram_rd(gb, 0x70);
  CYC(b_+21, b_+22); push_effect(gb, AF);
  CYC(b_+22, b_+24); A = 0x03;
  CYC(b_+24, b_+26); hram_wr(gb, 0x70, A);
  CYC(b_+26, b_+27); push_effect(gb, DE);
  CYC(b_+27, b_+30); SET_HL(wTmpVramBuffer);
  CYC(b_+30, b_+32); B = 0x20;
  CYC(b_+32, b_+34); C = 0xdc;
  CALL_C(b_+34, func_46ff_hook, SYM(func_46ff), b_+37);
  CYC(b_+37, b_+38); SET_DE(pop_effect(gb));
  CYC(b_+38, b_+40); A = 0x04;
  CYC(b_+40, b_+41); alu_add(gb, D);
  CYC(b_+41, b_+42); D = A;
  CYC(b_+42, b_+44); B = 0x20;
  CYC(b_+44, b_+46); C = 0xe0;
  CALL_C(b_+46, func_46ff_hook, SYM(func_46ff), b_+49);
  CYC(b_+49, b_+50); SET_AF(pop_effect(gb));
  CYC(b_+50, b_+52); hram_wr(gb, 0x70, A);
  CYC(b_+52, b_+53); ret_effect(gb);
}

void func_46ff_hook(GB *gb) {
  BASE(func_46ff);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, DE);
    CYC(b_+1, b_+2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+2, b_+4); A = 0x20;
    CYC(b_+4, b_+5); alu_add(gb, E);
    CYC(b_+5, b_+6); E = A;
    CYC(b_+6, b_+7); A = D;
    CYC(b_+7, b_+9); alu_adc(gb, 0x00);
    CYC(b_+9, b_+10); alu_cp(gb, C);
    if (!(F & FZ)) CYCT(b_+10, b_+12);
    else {
      CYC(b_+10, b_+12);
      CYC(b_+12, b_+14); alu_sub(gb, 0x04);
    }
    CYC(b_+14, b_+15); D = A;
    CYC(b_+15, b_+16); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+16, b_+18); continue; }
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+19); ret_effect(gb);
    return;
  }
}

void copyTileRowToVramBuffer_hook(GB *gb) {
  BASE(copyTileRowToVramBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenOffsetX);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+7); A = alu_swap(gb, A);
  CYC(b_+7, b_+8); alu_rlca(gb);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+12); A = mem_rd(gb, wScreenScrollRow);
  CYC(b_+12, b_+13); alu_rlca(gb);
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); alu_and(gb, 0x0f);
  CYC(b_+18, b_+19); D = A;
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+22); alu_and(gb, 0xf0);
  CYC(b_+22, b_+23); E = A;
  CYC(b_+23, b_+26); SET_HL(w2WaveScrollValues);
  CYC(b_+26, b_+27); alu_add_hl(gb, DE);
  CYC(b_+27, b_+29); B = 0x00;
  CYC(b_+29, b_+30); alu_add_hl(gb, BC);
  CYC(b_+30, b_+32); A = hram_rd(gb, 0x70);
  CYC(b_+32, b_+33); push_effect(gb, AF);
  CYC(b_+33, b_+35); A = 0x03;
  CYC(b_+35, b_+37); hram_wr(gb, 0x70, A);
  CYC(b_+37, b_+38); push_effect(gb, HL);
  CYC(b_+38, b_+40); B = 0x20;
  CYC(b_+40, b_+43); SET_DE(wTmpVramBuffer);
  CALL_C(b_+43, copyTileRowToVramBuffer__copyFunc_hook, b_+63, b_+46);
  CYC(b_+46, b_+47); SET_HL(pop_effect(gb));
  CYC(b_+47, b_+49); B = 0x20;
  CYC(b_+49, b_+50); A = H;
  CYC(b_+50, b_+52); alu_add(gb, 0x04);
  CYC(b_+52, b_+53); H = A;
  CYC(b_+53, b_+56); SET_DE(wTmpVramBuffer + 0x20);
  CALL_C(b_+56, copyTileRowToVramBuffer__copyFunc_hook, b_+63, b_+59);
  CYC(b_+59, b_+60); SET_AF(pop_effect(gb));
  CYC(b_+60, b_+62); hram_wr(gb, 0x70, A);
  CYC(b_+62, b_+63); ret_effect(gb);
}

void copyTileRowToVramBuffer__copyFunc_hook(GB *gb) {
  BASE(copyTileRowToVramBuffer);
  for (;;) {
    CYC(b_+63, b_+64); A = mem_rd(gb, HL);
    CYC(b_+64, b_+65); mem_wr(gb, DE, A);
    CYC(b_+65, b_+66); SET_DE(DE + 1);
    CYC(b_+66, b_+67); L = alu_inc8(gb, L);
    CYC(b_+67, b_+68); A = L;
    CYC(b_+68, b_+70); alu_and(gb, 0x1f);
    if (!(F & FZ)) CYCT(b_+70, b_+72);
    else {
      CYC(b_+70, b_+72);
      CYC(b_+72, b_+73); A = L;
      CYC(b_+73, b_+75); alu_sub(gb, 0x20);
      CYC(b_+75, b_+76); L = A;
    }
    CYC(b_+76, b_+77); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+77, b_+79); continue; }
    CYC(b_+77, b_+79);
    CYC(b_+79, b_+80); ret_effect(gb);
    return;
  }
}

void updateTilesetPalette_hook(GB *gb) {
  BASE(updateTilesetPalette);
  CYC(b_+0, b_+3); A = mem_rd(gb, wLoadedTilesetPalette);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wTilesetPalette);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); mem_wr(gb, wLoadedTilesetPalette, A);
  CYC(b_+12, b_+15);
  TAIL(loadPaletteHeader);
}

void cpLinkState0e_hook(GB *gb) {
  BASE(cpLinkState0e);
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0xd0);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(w1Link_state);
  CYC(b_+9, b_+11); A = 0x0e;
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); ret_effect(gb);
}

void initWaveScrollValues_body_hook(GB *gb) {
  BASE(initWaveScrollValues_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+7); SET_DE(b_+60);
  CYC(b_+7, b_+10); SET_HL(w2WaveScrollValues);
  for (;;) {
    CYC(b_+10, b_+11); push_effect(gb, HL);
    CYC(b_+11, b_+12); push_effect(gb, DE);
    CYC(b_+12, b_+13); A = mem_rd(gb, DE);
    CALL_C(b_+13, multiplyAByC_hook, SYM(multiplyAByC), b_+16);
    CYC(b_+16, b_+17); A = H;
    CYC(b_+17, b_+18); SET_DE(pop_effect(gb));
    CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
    CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+20, b_+21); SET_DE(DE + 1);
    CYC(b_+21, b_+22); A = L;
    CYC(b_+22, b_+24); alu_cp(gb, 0x20);
    if (F & FC) { CYCT(b_+24, b_+26); continue; }
    CYC(b_+24, b_+26);
    break;
  }
  CYC(b_+26, b_+29); SET_HL(w2WaveScrollValues + 0x1f);
  CYC(b_+29, b_+32); SET_DE(w2WaveScrollValues + 0x20);
  CYC(b_+32, b_+34); B = 0x20;
  for (;;) {
    CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+35, b_+36); mem_wr(gb, DE, A);
    CYC(b_+36, b_+37); E = alu_inc8(gb, E);
    CYC(b_+37, b_+38); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+38, b_+40); continue; }
    CYC(b_+38, b_+40);
    break;
  }
  CYC(b_+40, b_+43); SET_HL(w2WaveScrollValues + 0x3f);
  CYC(b_+43, b_+46); SET_DE(w2WaveScrollValues + 0x40);
  CYC(b_+46, b_+48); B = 0x40;
  for (;;) {
    CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+49, b_+50); alu_cpl(gb);
    CYC(b_+50, b_+51); A = alu_inc8(gb, A);
    CYC(b_+51, b_+52); mem_wr(gb, DE, A);
    CYC(b_+52, b_+53); E = alu_inc8(gb, E);
    CYC(b_+53, b_+54); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+54, b_+56); continue; }
    CYC(b_+54, b_+56);
    break;
  }
  CYC(b_+56, b_+57); alu_xor(gb, A);
  CYC(b_+57, b_+59); hram_wr(gb, 0x70, A);
  CYC(b_+59, b_+60); ret_effect(gb);
}

void loadBigBufferScrollValues_body_hook(GB *gb) {
  BASE(loadBigBufferScrollValues_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wFrameCounter);
  CYC(b_+7, b_+9); alu_and(gb, 0x7f);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); SET_DE(w2WaveScrollValues);
  CALL_C(b_+13, addAToDe_hook, 0x0068, b_+16);
  CYC(b_+16, b_+19); SET_HL(wBigBuffer);
  for (;;) {
    CYC(b_+19, b_+20); A = mem_rd(gb, DE);
    CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+21, b_+22); A = E;
    CYC(b_+22, b_+23); alu_add(gb, B);
    CYC(b_+23, b_+25); alu_and(gb, 0x7f);
    CYC(b_+25, b_+26); E = A;
    CYC(b_+26, b_+27); A = L;
    CYC(b_+27, b_+28); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+28, b_+30); continue; }
    CYC(b_+28, b_+30);
    break;
  }
  CYC(b_+30, b_+31); alu_xor(gb, A);
  CYC(b_+31, b_+33); hram_wr(gb, 0x70, A);
  CYC(b_+33, b_+34); ret_effect(gb);
}

void func_47fc_hook(GB *gb) {
  BASE(func_47fc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getPaletteFadeTransitionData_hook, SYM(getPaletteFadeTransitionData), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+5);
    CYC(b_+7, b_+8); alu_scf(gb);
    CYC(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void checkAndApplyPaletteFadeTransition_hook(GB *gb) {
  BASE(checkAndApplyPaletteFadeTransition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getPaletteFadeTransitionData_hook, SYM(getPaletteFadeTransitionData), b_+3);
  if (F & FC) CALL_C_CC(b_+3, applyPaletteFadeTransitionData_hook, SYM(applyPaletteFadeTransitionData), b_+6);
  else CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void getPaletteFadeTransitionData_hook(GB *gb) {
  BASE(getPaletteFadeTransitionData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkSymmetryCityPaletteTransition_hook, SYM(checkSymmetryCityPaletteTransition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wActiveGroup);
  CYC(b_+7, b_+10); SET_HL(SYM(paletteTransitionData));
  CYC(b_+10, b_+11); bank1_add_a_to_hl_from_rst(gb, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); bank1_add_a_to_hl_from_rst(gb, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wActiveRoom);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+20); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+20, b_+21); C = A;
  for (;;) {
    CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+22, b_+24); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(b_+24, b_+25); ret_effect(gb); return; }
    CYC(b_+24, b_+25);
    CYC(b_+25, b_+26); alu_cp(gb, C);
    if (!(F & FZ)) CYCT(b_+26, b_+28);
    else {
      CYC(b_+26, b_+28);
      CYC(b_+28, b_+29); A = mem_rd(gb, HL);
      CYC(b_+29, b_+30); alu_cp(gb, B);
      if (F & FZ) {
        CYCT(b_+30, b_+32);
        CYC(b_+37, b_+38); SET_HL(HL + 1);
        CYC(b_+38, b_+39); alu_scf(gb);
        CYC(b_+39, b_+40); ret_effect(gb);
        return;
      }
      CYC(b_+30, b_+32);
    }
    CYC(b_+32, b_+34); A = 0x05;
    CYC(b_+34, b_+35); bank1_add_a_to_hl_from_rst(gb, b_+35);
    CYC(b_+35, b_+37);
  }
}

void applyPaletteFadeTransitionData_hook(GB *gb) {
  BASE(applyPaletteFadeTransitionData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLoadedTilesetPalette);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wTilesetPalette);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+13); hram_wr(gb, 0x70, A);
  CYC(b_+13, b_+14); push_effect(gb, HL);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+20); SET_DE(w2ColorComponentBuffer1);
  CALL_C(b_+20, extractColorComponents_hook, SYM(extractColorComponents), b_+23);
  CYC(b_+23, b_+24); SET_HL(pop_effect(gb));
  CYC(b_+24, b_+25); SET_HL(HL + 1);
  CYC(b_+25, b_+26); SET_HL(HL + 1);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); H = mem_rd(gb, HL);
  CYC(b_+28, b_+29); L = A;
  CYC(b_+29, b_+32); SET_DE(w2ColorComponentBuffer2);
  CALL_C(b_+32, extractColorComponents_hook, SYM(extractColorComponents), b_+35);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CYC(b_+36, b_+38); hram_wr(gb, 0x70, A);
  CYC(b_+38, b_+40); A = 0xff;
  CYC(b_+40, b_+43); mem_wr(gb, wLoadedTilesetPalette, A);
  CYC(b_+43, b_+46);
  TAIL(startFadeBetweenTwoPalettes);
}

void checkSymmetryCityPaletteTransition_hook(GB *gb) {
  BASE(checkSymmetryCityPaletteTransition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    TAIL(checkSymmetryCityPaletteTransition__ok);
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x29;
  CALL_C(b_+8, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+11);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    TAIL(checkSymmetryCityPaletteTransition__ok);
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wActiveRoom);
  CYC(b_+16, b_+18); alu_cp(gb, 0x12);
  if (F & FZ) { CYCT(b_+18, b_+20); checkSymmetryCityPaletteTransition__notOk_hook(gb); return; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_cp(gb, 0x22);
  if (F & FZ) { CYCT(b_+22, b_+24); checkSymmetryCityPaletteTransition__notOk_hook(gb); return; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_cp(gb, 0x14);
  if (F & FZ) { CYCT(b_+26, b_+28); checkSymmetryCityPaletteTransition__notOk_hook(gb); return; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 0x24);
  if (F & FZ) { CYCT(b_+30, b_+32); checkSymmetryCityPaletteTransition__notOk_hook(gb); return; }
  CYC(b_+30, b_+32);
  TAIL(checkSymmetryCityPaletteTransition__ok);
}

void checkSymmetryCityPaletteTransition__ok_hook(GB *gb) {
  BASE(checkSymmetryCityPaletteTransition);
  CYC(b_+32, b_+33); alu_scf(gb);
  CYC(b_+33, b_+34); ret_effect(gb);
}

void checkSymmetryCityPaletteTransition__notOk_hook(GB *gb) {
  BASE(checkSymmetryCityPaletteTransition);
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+36); ret_effect(gb);
}

void makeActiveObjectFollowLink_b01_hook(GB *gb) {
  BASE(makeActiveObjectFollowLink_b01);
  CYC(b_+0, b_+3); SET_HL(wFollowingLinkObjectType);
  CYC(b_+3, b_+5); A = mem_rd(gb, hActiveObjectType);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+8); A = mem_rd(gb, hActiveObject);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  TAIL(resetFollowingLinkPath);
}

void resetFollowingLinkPath_hook(GB *gb) {
  BASE(resetFollowingLinkPath);
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+4); A = mem_rd(gb, w1Link_direction);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+8); A = mem_rd(gb, w1Link_yh);
  CYC(b_+8, b_+9); D = A;
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link_xh);
  CYC(b_+12, b_+13); E = A;
  CYC(b_+13, b_+15); A = 0x02;
  CYC(b_+15, b_+17); hram_wr(gb, 0x70, A);
  CYC(b_+17, b_+20); SET_HL(w2LinkWalkPath);
  CYC(b_+20, b_+22); A = 0x10;
  for (;;) {
    CYC(b_+22, b_+23); mem_wr(gb, HL, C);
    CYC(b_+23, b_+24); L = alu_inc8(gb, L);
    CYC(b_+24, b_+25); mem_wr(gb, HL, D);
    CYC(b_+25, b_+26); L = alu_inc8(gb, L);
    CYC(b_+26, b_+27); mem_wr(gb, HL, E);
    CYC(b_+27, b_+28); L = alu_inc8(gb, L);
    CYC(b_+28, b_+29); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+29, b_+31); continue; }
    CYC(b_+29, b_+31);
    break;
  }
  CYC(b_+31, b_+34); mem_wr(gb, wLinkPathIndex, A);
  CYC(b_+34, b_+36); hram_wr(gb, 0x70, A);
  CYC(b_+36, b_+39); A = mem_rd(gb, wFollowingLinkObjectType);
  CYC(b_+39, b_+41); alu_add(gb, 0x0b);
  CYC(b_+41, b_+42); L = A;
  CYC(b_+42, b_+45); A = mem_rd(gb, wFollowingLinkObject);
  CYC(b_+45, b_+46); H = A;
  CYC(b_+46, b_+47); mem_wr(gb, HL, D);
  CYC(b_+47, b_+48); L = alu_inc8(gb, L);
  CYC(b_+48, b_+49); L = alu_inc8(gb, L);
  CYC(b_+49, b_+50); mem_wr(gb, HL, E);
  CYC(b_+50, b_+51); SET_DE(pop_effect(gb));
  CYC(b_+51, b_+52); ret_effect(gb);
}

void checkUpdateFollowingLinkObject_hook(GB *gb) {
  BASE(checkUpdateFollowingLinkObject);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFollowingLinkObject);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, checkUpdateFollowingLinkObject__update_hook, b_+12, b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+11); hram_wr(gb, 0x70, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void checkUpdateFollowingLinkObject__update_hook(GB *gb) {
  BASE(checkUpdateFollowingLinkObject);
  CYC(b_+12, b_+15); A = mem_rd(gb, wLinkPathIndex);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+17); alu_add(gb, A);
  if (F & FC) { CYCT(b_+17, b_+19); resetFollowingLinkPath_hook(gb); return; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); alu_add(gb, B);
  CYC(b_+20, b_+23); SET_HL(w2LinkWalkPath);
  CYC(b_+23, b_+24); bank1_add_a_to_hl_from_rst(gb, b_+24);
  CYC(b_+24, b_+27); A = mem_rd(gb, w1Link_direction);
  CYC(b_+27, b_+28); C = A;
  CYC(b_+28, b_+31); A = mem_rd(gb, w1Link_yh);
  CYC(b_+31, b_+32); D = A;
  CYC(b_+32, b_+35); A = mem_rd(gb, w1Link_xh);
  CYC(b_+35, b_+36); E = A;
  CYC(b_+36, b_+38); A = 0x02;
  CYC(b_+38, b_+40); hram_wr(gb, 0x70, A);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); alu_cp(gb, C);
  if (!(F & FZ)) CYCT(b_+42, b_+44);
  else {
    CYC(b_+42, b_+44);
    CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+45, b_+46); alu_cp(gb, D);
    if (!(F & FZ)) CYCT(b_+46, b_+48);
    else {
      CYC(b_+46, b_+48);
      CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+49, b_+50); alu_cp(gb, E);
      if (F & FZ) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
      CYC(b_+50, b_+51);
    }
  }
  CYC(b_+51, b_+54); A = mem_rd(gb, wLinkPathIndex);
  CYC(b_+54, b_+55); A = alu_inc8(gb, A);
  CYC(b_+55, b_+57); alu_and(gb, 0x0f);
  CYC(b_+57, b_+60); mem_wr(gb, wLinkPathIndex, A);
  CYC(b_+60, b_+61); B = A;
  CYC(b_+61, b_+62); alu_add(gb, A);
  CYC(b_+62, b_+63); alu_add(gb, B);
  CYC(b_+63, b_+66); SET_HL(w2LinkWalkPath);
  CYC(b_+66, b_+67); bank1_add_a_to_hl_from_rst(gb, b_+67);
  CYC(b_+67, b_+68); A = C;
  CYC(b_+68, b_+69); C = mem_rd(gb, HL);
  CYC(b_+69, b_+70); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+70, b_+71); A = D;
  CYC(b_+71, b_+72); D = mem_rd(gb, HL);
  CYC(b_+72, b_+73); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+73, b_+74); A = E;
  CYC(b_+74, b_+75); E = mem_rd(gb, HL);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+76, b_+77); alu_xor(gb, A);
  CYC(b_+77, b_+79); hram_wr(gb, 0x70, A);
  CYC(b_+79, b_+82); A = mem_rd(gb, wFollowingLinkObject);
  CYC(b_+82, b_+83); H = A;
  CYC(b_+83, b_+86); A = mem_rd(gb, wFollowingLinkObjectType);
  CYC(b_+86, b_+88); alu_add(gb, 0x08);
  CYC(b_+88, b_+89); L = A;
  CYC(b_+89, b_+90); mem_wr(gb, HL, C);
  CYC(b_+90, b_+91); L = alu_inc8(gb, L);
  CYC(b_+91, b_+92); L = alu_inc8(gb, L);
  CYC(b_+92, b_+93); L = alu_inc8(gb, L);
  CYC(b_+93, b_+94); mem_wr(gb, HL, D);
  CYC(b_+94, b_+95); L = alu_inc8(gb, L);
  CYC(b_+95, b_+96); L = alu_inc8(gb, L);
  CYC(b_+96, b_+97); mem_wr(gb, HL, E);
  CYC(b_+97, b_+98); ret_effect(gb);
}

static void finish_clear_memory_on_screen_reload(GB *gb) {
  BASE(func_49c9);
  CYC(b_+8, b_+10); A = 0xff;
  CYC(b_+10, b_+13); mem_wr(gb, wccaa, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void clearMemoryOnScreenReload_hook(GB *gb) {
  BASE(clearMemoryOnScreenReload);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wLinkInAir);
  CYC(b_+3, b_+5); B = GV(0x8d, 0x88);
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CYC(b_+8, b_+11); SET_HL(wLinkObjectIndex);
  CYC(b_+11, b_+13); A = 0xd1;
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+14, b_+16);
  else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); A = alu_dec8(gb, A);
    CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  }
  CALL_C(b_+18, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+21);
  CALL_C(b_+21, clearItems_hook, SYM(clearItems), b_+24);
  CYC(b_+24, b_+26);
  finish_clear_memory_on_screen_reload(gb);
}

void func_49c9_hook(GB *gb) {
  BASE(func_49c9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wDisabledObjects);
  CYC(b_+3, b_+5); B = GV(0x57, 0x53);
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  finish_clear_memory_on_screen_reload(gb);
}

void setObjectsEnabledTo2_hook(GB *gb) {
  BASE(setObjectsEnabledTo2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setInteractionsEnabledTo2_hook, SYM(setInteractionsEnabledTo2), b_+3);
  CALL_C(b_+3, setEnemiesEnabledTo2_hook, SYM(setEnemiesEnabledTo2), b_+6);
  CALL_C(b_+6, setPartsEnabledTo2_hook, SYM(setPartsEnabledTo2), b_+9);
  CALL_C(b_+9, setItemsEnabledTo2_hook, SYM(setItemsEnabledTo2), b_+12);
  CYC(b_+12, b_+15); SET_HL(w1Link);
  CYC(b_+15, b_+17); C = 0xd2;
  CYC(b_+17, b_+19);
  TAIL(setObjectsEnabledTo2_hlpr);
}

void setItemsEnabledTo2_hook(GB *gb) {
  BASE(setItemsEnabledTo2);
  CYC(b_+0, b_+3); SET_HL(w1WeaponItem);
  CYC(b_+3, b_+5); C = 0xe0;
  CYC(b_+5, b_+7);
  TAIL(setObjectsEnabledTo2_hlpr);
}

void setInteractionsEnabledTo2_hook(GB *gb) {
  BASE(setInteractionsEnabledTo2);
  CYC(b_+0, b_+3); SET_HL(w1ReservedInteraction0);
  CYC(b_+3, b_+5); C = 0xe0;
  CYC(b_+5, b_+7);
  TAIL(setObjectsEnabledTo2_hlpr);
}

void setEnemiesEnabledTo2_hook(GB *gb) {
  BASE(setEnemiesEnabledTo2);
  CYC(b_+0, b_+3); SET_HL((w1ReservedInteraction0_var3f + 1));
  CYC(b_+3, b_+5); C = 0xe0;
  CYC(b_+5, b_+7);
  TAIL(setObjectsEnabledTo2_hlpr);
}

void setPartsEnabledTo2_hook(GB *gb) {
  BASE(setPartsEnabledTo2);
  CYC(b_+0, b_+3); SET_HL(w7TextDisplayState);
  CYC(b_+3, b_+5); C = 0xe0;
  TAIL(setObjectsEnabledTo2_hlpr);
}

void setObjectsEnabledTo2_hlpr_hook(GB *gb) {
  BASE(setObjectsEnabledTo2_hlpr);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL);
    CYC(b_+1, b_+3); alu_and(gb, 0x03);
    CYC(b_+3, b_+5); alu_cp(gb, 0x01);
    if (!(F & FZ)) CYCT(b_+5, b_+7);
    else {
      CYC(b_+5, b_+7);
      CYC(b_+7, b_+8); A = mem_rd(gb, HL);
      CYC(b_+8, b_+10); alu_and(gb, 0xfc);
      CYC(b_+10, b_+12); alu_or(gb, 0x02);
      CYC(b_+12, b_+13); mem_wr(gb, HL, A);
    }
    CYC(b_+13, b_+14); H = alu_inc8(gb, H);
    CYC(b_+14, b_+15); A = H;
    CYC(b_+15, b_+16); alu_cp(gb, C);
    if (F & FC) { CYCT(b_+16, b_+18); continue; }
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+19); ret_effect(gb);
    return;
  }
}

void func_400b_hook(GB *gb) {
  BASE(func_400b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenTransitionState);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(screenTransitionState0) && hook_is(gb, SYM(screenTransitionState0), screenTransitionState0_hook)) { screenTransitionState0_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState1) && hook_is(gb, SYM(screenTransitionState1), screenTransitionState1_hook)) { screenTransitionState1_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState2) && hook_is(gb, SYM(screenTransitionState2), screenTransitionState2_hook)) { screenTransitionState2_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState3) && hook_is(gb, SYM(screenTransitionState3), screenTransitionState3_hook)) { screenTransitionState3_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState4) && hook_is(gb, SYM(screenTransitionState4), screenTransitionState4_hook)) { screenTransitionState4_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState5) && hook_is(gb, SYM(screenTransitionState5), screenTransitionState5_hook)) { screenTransitionState5_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void screenTransitionState0_hook(GB *gb) {
  BASE(screenTransitionState0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wPaletteThread_parameter, A);
  CALL_C(b_+4, checkDarkenRoom_hook, SYM(checkDarkenRoom), b_+7);
  CYC(b_+7, b_+9); A = 0x01;
  CYC(b_+9, b_+12); mem_wr(gb, wScreenTransitionState, A);
  TAIL(initializeRoomBoundaryAndLoadAnimations);
}

void initializeRoomBoundaryAndLoadAnimations_hook(GB *gb) {
  BASE(initializeRoomBoundaryAndLoadAnimations);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+3);
  CYC(b_+3, b_+5); B = 0x01;
  CYC(b_+5, b_+8); A = mem_rd(gb, wActiveGroup);
  CYC(b_+8, b_+10); alu_and(gb, 0x04);
  if (F & FZ) CYCT(b_+10, b_+12);
  else { CYC(b_+10, b_+12); CYC(b_+12, b_+14); B = 0x00; }
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+18); mem_wr(gb, wcd01, A);
  CYC(b_+18, b_+20); alu_xor(gb, 0x01);
  CYC(b_+20, b_+23); mem_wr(gb, wRoomIsLarge, A);
  CYC(b_+23, b_+26); A = mem_rd(gb, wcd01);
  CYC(b_+26, b_+27); alu_add(gb, A);
  CYC(b_+27, b_+30); SET_HL(b_+70);
  CYC(b_+30, b_+31); bank1_add_double_index_to_hl_from_rst(gb, b_+31);
  CYC(b_+31, b_+34); SET_DE(wRoomWidth);
  CYC(b_+34, b_+36); B = 0x04;
  do {
    CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+37, b_+38); mem_wr(gb, DE, A);
    CYC(b_+38, b_+39); SET_DE(DE + 1);
    CYC(b_+39, b_+40); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+40, b_+42); else CYC(b_+40, b_+42);
  } while (!(F & FZ));
  CYC(b_+42, b_+45); A = mem_rd(gb, wRoomWidth);
  CYC(b_+45, b_+47); alu_sub(gb, 0x14);
  CYC(b_+47, b_+48); alu_add(gb, A);
  CYC(b_+48, b_+49); alu_add(gb, A);
  CYC(b_+49, b_+50); alu_add(gb, A);
  CYC(b_+50, b_+53); mem_wr(gb, wMaxCameraY, A);
  CYC(b_+53, b_+56); A = mem_rd(gb, wRoomHeight);
  CYC(b_+56, b_+58); alu_sub(gb, 0x10);
  CYC(b_+58, b_+59); alu_add(gb, A);
  CYC(b_+59, b_+60); alu_add(gb, A);
  CYC(b_+60, b_+61); alu_add(gb, A);
  CYC(b_+61, b_+64); mem_wr(gb, wMaxCameraX, A);
  CALL_C(b_+64, calculateRoomEdge_hook, SYM(calculateRoomEdge), b_+67);
  CYC(b_+67, b_+70); TAIL(loadTilesetAnimation);
}

void screen_transition_state1_body_hook(GB *gb) {
  BASE(screenTransitionState1);
  uint16_t entry = gb->hook_pc;
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (entry == b_+0) {
    CYC(b_+0, b_+3); A = mem_rd(gb, wcd03);
    CYC(b_+3, b_+4); A = alu_inc8(gb, A);
    CYC(b_+4, b_+7); mem_wr(gb, wcd03, A);
    CYC(b_+7, b_+10); A = mem_rd(gb, wScreenTransitionState2);
    CYC(b_+10, b_+11); bank1_jump_table_from_rst(gb, b_+11);
    do { uint16_t jt_ = (HL);
      if (jt_ == b_+19) { entry = b_+19; break; }
      else if (jt_ == b_+44) { entry = b_+44; break; }
      else if (jt_ == b_+76) { entry = b_+76; break; }
      else if (jt_ == SYM(setScreenTransitionState02) && hook_is(gb, SYM(setScreenTransitionState02), setScreenTransitionState02_hook)) { setScreenTransitionState02_hook(gb); return; }
      else { HANDOFF(HL); }
    } while (0);
  }
  if (entry == b_+19) {
    CYC(b_+19, b_+22); A = mem_rd(gb, wPaletteThread_mode);
    CYC(b_+22, b_+23); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
    CYC(b_+23, b_+24);
    CYC(b_+24, b_+26); A = 0x02;
    CYC(b_+26, b_+29); mem_wr(gb, wScrollMode, A);
    CYC(b_+29, b_+31); A = 0x01;
    CYC(b_+31, b_+34); mem_wr(gb, wScreenTransitionState2, A);
    CYC(b_+34, b_+35); alu_xor(gb, A);
    CYC(b_+35, b_+38); mem_wr(gb, wScreenTransitionState3, A);
    CYC(b_+38, b_+41); mem_wr(gb, wcd03, A);
    CYC(b_+41, b_+44); TAIL(resetCamera);
  }
  if (entry == b_+44) {
    CYC(b_+44, b_+47); A = mem_rd(gb, wScreenOffsetX);
    CYC(b_+47, b_+48); B = A;
    CYC(b_+48, b_+50); A = hram_rd(gb, hCameraX & 0xff);
    CYC(b_+50, b_+51); alu_add(gb, B);
    CYC(b_+51, b_+53); alu_add(gb, 0x50);
    CYC(b_+53, b_+54); alu_rrca(gb);
    CYC(b_+54, b_+55); alu_rrca(gb);
    CYC(b_+55, b_+56); alu_rrca(gb);
    CYC(b_+56, b_+57); A = alu_dec8(gb, A);
    CYC(b_+57, b_+59); alu_and(gb, 0x1f);
    CYC(b_+59, b_+62); mem_wr(gb, wScreenScrollRow, A);
    CYC(b_+62, b_+63); A = alu_inc8(gb, A);
    CYC(b_+63, b_+66); mem_wr(gb, wScreenScrollDirection, A);
    CYC(b_+66, b_+67); alu_xor(gb, A);
    CYC(b_+67, b_+70); mem_wr(gb, wScreenScrollCounter, A);
    CYC(b_+70, b_+72); A = 0x02;
    CYC(b_+72, b_+75); mem_wr(gb, wScreenTransitionState2, A);
    CYC(b_+75, b_+76); ret_effect(gb); return;
  }
  CYC(b_+76, b_+79); A = mem_rd(gb, wScreenScrollCounter);
  CYC(b_+79, b_+81); alu_cp(gb, 0x20);
  if (F & FZ) { CYCT(b_+81, b_+83); setScreenTransitionState02_hook(gb); return; }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); A = alu_inc8(gb, A);
  CYC(b_+84, b_+87); mem_wr(gb, wScreenScrollCounter, A);
  CYC(b_+87, b_+90); A = mem_rd(gb, wcd03);
  CYC(b_+90, b_+91); alu_rrca(gb);
  if (F & FC) { CYCT(b_+91, b_+93); goto increment_direction; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+96); A = mem_rd(gb, wScreenScrollRow);
  CYC(b_+96, b_+98); hram_wr(gb, hFF8B & 0xff, A);
  CYC(b_+98, b_+99); B = A;
  CYC(b_+99, b_+100); A = alu_dec8(gb, A);
  CYC(b_+100, b_+102); alu_and(gb, 0x1f);
  CYC(b_+102, b_+105); mem_wr(gb, wScreenScrollRow, A);
  CYC(b_+105, b_+107); goto queue_row;
increment_direction:
  CYC(b_+107, b_+110); A = mem_rd(gb, wScreenScrollDirection);
  CYC(b_+110, b_+112); hram_wr(gb, hFF8B & 0xff, A);
  CYC(b_+112, b_+113); B = A;
  CYC(b_+113, b_+114); A = alu_inc8(gb, A);
  CYC(b_+114, b_+116); alu_and(gb, 0x1f);
  CYC(b_+116, b_+119); mem_wr(gb, wScreenScrollDirection, A);
queue_row:
  CYC(b_+119, b_+120); E = B;
  CALL_C(b_+120, func_46ca_hook, SYM(func_46ca), b_+123);
  CYC(b_+123, b_+125); A = hram_rd(gb, hFF8B & 0xff);
  CYC(b_+125, b_+128); TAIL(addFunctionsToVBlankQueue);
}

void screenTransitionState1_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState1); screen_transition_state1_body_hook(gb); }
void screenTransitionState1__substate0_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState1__substate0); screen_transition_state1_body_hook(gb); }
void screenTransitionState1__substate1_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState1__substate1); screen_transition_state1_body_hook(gb); }
void screenTransitionState1__substate2_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState1__substate2); screen_transition_state1_body_hook(gb); }

void setScreenTransitionState02_hook(GB *gb) {
  BASE(setScreenTransitionState02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setInstrumentsDisabledCounterAndScrollMode_hook, SYM(setInstrumentsDisabledCounterAndScrollMode), b_+3);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+8); mem_wr(gb, wScreenTransitionState, A);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12); mem_wr(gb, wScreenTransitionState2, A);
  CYC(b_+12, b_+15); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void screen_transition_state2_body_hook(GB *gb) {
  BASE(screenTransitionState2);
  uint16_t entry = gb->hook_pc;
  uint16_t sp0_ = gb->sp; (void)sp0_;
  do { uint16_t jt_ = (entry);
    if (jt_ == b_+O(57)) { goto transition_up; }
    else if (jt_ == b_+O(68)) { goto transition_down; }
    else if (jt_ == b_+O(78)) { goto transition_left; }
    else if (jt_ == b_+O(86)) { goto transition_right; }
    else if (jt_ == b_+O(91)) { goto transition; }
    else if (!game_seasons && jt_ == b_+185) { goto done_boundary_checks; }
    else if (jt_ == b_+O(209)) { goto start_transition; }
  } while (0);
  if (entry == b_+O(0)) {
    CYC(b_+O(0), b_+OE(3)); A = mem_rd(gb, wLinkInAir);
    CYC(b_+O(3), b_+OE(4)); alu_add(gb, A);
    if (F & FC) { CYCT(b_+O(4), b_+OE(6)); goto check_direction; }
    CYC(b_+O(4), b_+OE(6));
    if (F & FZ) { CYCT(b_+O(6), b_+OE(8)); goto check_direction; }
    CYC(b_+O(6), b_+OE(8));
    CYC(b_+O(8), b_+OE(10)); A = 0x04;
    CYC(b_+O(10), b_+OE(13)); mem_wr(gb, wScreenTransitionDelay, A);
  }
check_direction:
  if (entry == b_+O(0)) {
    CYC(b_+O(13), b_+OE(16)); A = mem_rd(gb, wScreenTransitionDirection);
    CYC(b_+O(16), b_+OE(18)); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYC(b_+O(18), b_+OE(20));
      CYC(b_+O(20), b_+OE(22)); alu_and(gb, 0x7f);
      CYC(b_+O(22), b_+OE(23)); C = A;
      CYC(b_+O(23), b_+OE(26)); goto start_transition;
    }
    CYCT(b_+O(18), b_+OE(20));
    CYC(b_+O(26), b_+OE(29)); A = mem_rd(gb, wLinkObjectIndex);
    CYC(b_+O(29), b_+OE(30)); H = A;
    CYC(b_+O(30), b_+OE(32)); L = 0x0b;
    CYC(b_+O(32), b_+OE(34)); A = 0x05;
    CYC(b_+O(34), b_+OE(35)); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FC)) { CYCT(b_+O(35), b_+OE(37)); entry = b_+O(57); goto transition_up; }
    CYC(b_+O(35), b_+OE(37));
    CYC(b_+O(37), b_+OE(40)); A = mem_rd(gb, wScreenTransitionBoundaryY);
    CYC(b_+O(40), b_+OE(41)); alu_cp(gb, mem_rd(gb, HL));
    if (F & FC) { CYCT(b_+O(41), b_+OE(43)); entry = b_+O(68); goto transition_down; }
    CYC(b_+O(41), b_+OE(43));
  }
check_horizontal:
  CYC(b_+O(43), b_+OE(45)); L = 0x0d;
  CYC(b_+O(45), b_+OE(47)); A = 0x05;
  CYC(b_+O(47), b_+OE(48)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+O(48), b_+OE(50)); entry = b_+O(78); goto transition_left; }
  CYC(b_+O(48), b_+OE(50));
  CYC(b_+O(50), b_+OE(53)); A = mem_rd(gb, wScreenTransitionBoundaryX);
  CYC(b_+O(53), b_+OE(54)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+O(54), b_+OE(56)); entry = b_+O(86); goto transition_right; }
  CYC(b_+O(54), b_+OE(56));
  CYC(b_+O(56), b_+OE(57)); ret_effect(gb); return;
transition_up:
  CYC(b_+O(57), b_+OE(58)); A = alu_inc8(gb, A);
  CYC(b_+O(58), b_+OE(59)); mem_wr(gb, HL, A);
  CYC(b_+O(59), b_+OE(61)); B = 0x40;
  CYC(b_+O(61), b_+OE(63)); C = 0x00;
  CALL_C(b_+O(63), screenTransitionState2__transition_hook, b_+O(91), b_+OE(66));
  CYC(b_+O(66), b_+OE(68)); goto check_horizontal;
transition_down:
  CYC(b_+O(68), b_+OE(69)); mem_wr(gb, HL, A);
  CYC(b_+O(69), b_+OE(71)); B = 0x80;
  CYC(b_+O(71), b_+OE(73)); C = 0x02;
  CALL_C(b_+O(73), screenTransitionState2__transition_hook, b_+O(91), b_+OE(76));
  CYC(b_+O(76), b_+OE(78)); goto check_horizontal;
transition_left:
  CYC(b_+O(78), b_+OE(79)); A = alu_inc8(gb, A);
  CYC(b_+O(79), b_+OE(80)); mem_wr(gb, HL, A);
  CYC(b_+O(80), b_+OE(82)); B = 0x20;
  CYC(b_+O(82), b_+OE(84)); C = 0x03;
  CYC(b_+O(84), b_+OE(86)); goto transition;
transition_right:
  CYC(b_+O(86), b_+OE(87)); mem_wr(gb, HL, A);
  CYC(b_+O(87), b_+OE(89)); B = 0x10;
  CYC(b_+O(89), b_+OE(91)); C = 0x01;
transition:
  CYC(b_+O(91), b_+OE(94)); A = mem_rd(gb, w1Link_enabled);
  CYC(b_+O(94), b_+OE(95)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(95), b_+OE(96)); ret_effect(gb); return; }
  CYC(b_+O(95), b_+OE(96));
  CYC(b_+O(96), b_+OE(99)); A = mem_rd(gb, wDisableScreenTransitions);
  CYC(b_+O(99), b_+OE(100)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(100), b_+OE(101)); ret_effect(gb); return; }
  CYC(b_+O(100), b_+OE(101));
  CYC(b_+O(101), b_+OE(104)); A = mem_rd(gb, wScreenTransitionDelay);
  CYC(b_+O(104), b_+OE(105)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(105), b_+OE(107)); goto ready_to_transition; }
  CYC(b_+O(105), b_+OE(107));
  CYC(b_+O(107), b_+OE(108)); A = alu_dec8(gb, A);
  CYC(b_+O(108), b_+OE(111)); mem_wr(gb, wScreenTransitionDelay, A);
  CYC(b_+O(111), b_+OE(112)); ret_effect(gb); return;
ready_to_transition:
  CYC(b_+O(112), b_+OE(115)); A = mem_rd(gb, w1Companion_id);
  CYC(b_+O(115), b_+OE(117)); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+O(117), b_+OE(119)); goto start_transition; }
  CYC(b_+O(117), b_+OE(119));
  CYC(b_+O(119), b_+OE(122)); A = mem_rd(gb, wcc92);
  CYC(b_+O(122), b_+OE(123)); alu_add(gb, A);
  if (F & FC) { CYCT(b_+O(123), b_+OE(124)); ret_effect(gb); return; }
  CYC(b_+O(123), b_+OE(124));
  CYC(b_+O(124), b_+OE(127)); A = mem_rd(gb, wLinkInAir);
  CYC(b_+O(127), b_+OE(128)); alu_add(gb, A);
  if (F & FC) { CYCT(b_+O(128), b_+OE(130)); goto start_transition; }
  CYC(b_+O(128), b_+OE(130));
  CYC(b_+O(130), b_+OE(133)); A = mem_rd(gb, w1Link_knockbackCounter);
  CYC(b_+O(133), b_+OE(134)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(134), b_+OE(135)); ret_effect(gb); return; }
  CYC(b_+O(134), b_+OE(135));
  CYC(b_+O(135), b_+OE(138)); A = mem_rd(gb, wcc92);
  CYC(b_+O(138), b_+OE(139)); alu_add(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(139), b_+OE(141)); goto boundary_checks; }
  CYC(b_+O(139), b_+OE(141));
  CALL_C(b_+O(141), convertLinkAngleToDirectionButtons_hook, SYM(convertLinkAngleToDirectionButtons), b_+OE(144));
  CYC(b_+O(144), b_+OE(145)); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+O(145), b_+OE(146)); ret_effect(gb); return; }
  CYC(b_+O(145), b_+OE(146));
boundary_checks:
  if (!game_seasons) {      // room-edge and toggle-block boundary rules: Ages only
    CYC(b_+146, b_+149); A = mem_rd(gb, wTilesetFlags);
    CYC(b_+149, b_+151); alu_and(gb, 0x01);
    if (F & FZ) { CYCT(b_+151, b_+153); goto done_boundary_checks; }
    CYC(b_+151, b_+153);
    CYC(b_+153, b_+156); A = mem_rd(gb, wActiveRoom);
    CYC(b_+156, b_+157); E = A;
    CYC(b_+157, b_+159); alu_and(gb, 0x0f);
    CYC(b_+159, b_+161); alu_cp(gb, 0x0d);
    if (!(F & FZ)) { CYCT(b_+161, b_+163); goto right_boundary; }
    CYC(b_+161, b_+163);
    CYC(b_+163, b_+164); A = C;
    CYC(b_+164, b_+166); alu_cp(gb, 0x01);
    if (F & FZ) { CYCT(b_+166, b_+167); ret_effect(gb); return; }
    CYC(b_+166, b_+167);
right_boundary:
    CYC(b_+167, b_+168); A = E;
    CYC(b_+168, b_+170); alu_cp(gb, 0xd0);
    if (F & FC) { CYCT(b_+170, b_+172); goto left_boundary; }
    CYC(b_+170, b_+172);
    CYC(b_+172, b_+173); A = C;
    CYC(b_+173, b_+175); alu_cp(gb, 0x02);
    if (F & FZ) { CYCT(b_+175, b_+176); ret_effect(gb); return; }
    CYC(b_+175, b_+176);
left_boundary:
    CYC(b_+176, b_+177); A = E;
    CYC(b_+177, b_+179); alu_and(gb, 0x0f);
    if (!(F & FZ)) { CYCT(b_+179, b_+181); goto done_boundary_checks; }
    CYC(b_+179, b_+181);
    CYC(b_+181, b_+182); A = C;
    CYC(b_+182, b_+184); alu_cp(gb, 0x03);
    if (F & FZ) { CYCT(b_+184, b_+185); ret_effect(gb); return; }
    CYC(b_+184, b_+185);
done_boundary_checks:
    CYC(b_+185, b_+188); A = mem_rd(gb, wTilesetFlags);
    CYC(b_+188, b_+190); alu_and(gb, 0x40);
    if (!(F & FZ)) { CYCT(b_+190, b_+192); goto start_transition; }
    CYC(b_+190, b_+192);
    CYC(b_+192, b_+195); A = mem_rd(gb, wcc92);
    CYC(b_+195, b_+197); alu_and(gb, 0x08);
    if (!(F & FZ)) { CYCT(b_+197, b_+199); goto start_transition; }
    CYC(b_+197, b_+199);
  }
  CALL_C(b_+O(199), checkLinkIsOverHazard_hook, SYM(checkLinkIsOverHazard), b_+OE(202));
  CYC(b_+O(202), b_+OE(203)); alu_rrca(gb);
  if (F & FC) CALL_C_CC(b_+O(203), screenTransitionState2__checkCanTransitionOverWater_hook, SYM(screenTransitionState2__checkCanTransitionOverWater), b_+OE(206));
  else CYC(b_+O(203), b_+OE(206));
  CYC(b_+O(206), b_+OE(208)); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+O(208), b_+OE(209)); ret_effect(gb); return; }
  CYC(b_+O(208), b_+OE(209));
start_transition:
  CYC(b_+O(209), b_+OE(211)); A = 0x04;
  CYC(b_+O(211), b_+OE(214)); mem_wr(gb, wScrollMode, A);
  CYC(b_+O(214), b_+OE(216)); A = 0x03;
  CYC(b_+O(216), b_+OE(219)); mem_wr(gb, wScreenTransitionState, A);
  CYC(b_+O(219), b_+OE(220)); A = C;
  CYC(b_+O(220), b_+OE(223)); mem_wr(gb, wScreenTransitionDirection, A);
  CYC(b_+O(223), b_+OE(224)); ret_effect(gb); return;
}

void screenTransitionState2_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__transitionUp_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__transitionUp); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__transitionDown_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__transitionDown); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__transitionLeft_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__transitionLeft); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__transitionRight_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__transitionRight); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__transition_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__transition); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__doneBoundaryChecks_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__doneBoundaryChecks); screen_transition_state2_body_hook(gb); }
void screenTransitionState2__startTransition_hook(GB *gb) { gb->hook_pc = SYM(screenTransitionState2__startTransition); screen_transition_state2_body_hook(gb); }

void screenTransitionState2__checkCanTransitionOverWater_hook(GB *gb) {
  BASE(screenTransitionState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(224), b_+OE(227)); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+O(227), b_+OE(228)); alu_rrca(gb);
  if (F & FC) { CYCT(b_+O(228), b_+OE(230)); screenTransitionState2__fail_hook(gb); return; }
  CYC(b_+O(228), b_+OE(230));
  CYC(b_+O(230), b_+OE(232)); A = GV(0x4a, 0x2e);    // TREASURE_MERMAID_SUIT / TREASURE_FLIPPERS
  CALL_C(b_+O(232), checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+OE(235));
  if (F & FC) { CYCT(b_+O(235), b_+OE(236)); ret_effect(gb); return; }
  CYC(b_+O(235), b_+OE(236));
  if (!game_seasons) {      // flippers as a second chance: Ages only
    CYC(b_+236, b_+239); A = mem_rd(gb, wObjectTileIndex);
    CYC(b_+239, b_+241); alu_cp(gb, 0xfc);
    if (F & FZ) { CYCT(b_+241, b_+243); screenTransitionState2__fail_hook(gb); return; }
    CYC(b_+241, b_+243);
    CYC(b_+243, b_+245); A = 0x2e;
    CALL_C(b_+245, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+248);
    if (F & FC) { CYCT(b_+248, b_+249); ret_effect(gb); return; }
    CYC(b_+248, b_+249);
  }
  TAIL(screenTransitionState2__fail);
}

void screenTransitionState2__fail_hook(GB *gb) {
  BASE(screenTransitionState2);
  CYC(b_+O(249), b_+OE(251)); A = 0xff;
  CYC(b_+O(251), b_+OE(252)); ret_effect(gb);
}

void update_camera_position_body_hook(GB *gb) {
  BASE(updateCameraPosition);
  uint16_t entry = gb->hook_pc;
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (entry == b_+0) {
    CYC(b_+0, b_+3); SET_HL(wScrollMode);
    CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
    CYC(b_+5, b_+8); A = mem_rd(gb, wActiveGroup);
    CYC(b_+8, b_+10); alu_cp(gb, 0x04);
    if (!(F & FC)) { CYCT(b_+10, b_+12); entry = b_+18; } else { CYC(b_+10, b_+12); entry = b_+12; }
  }
  if (entry == b_+12) {
    CYC(b_+12, b_+13); alu_xor(gb, A);
    CYC(b_+13, b_+15); hram_wr(gb, hCameraY & 0xff, A);
    CYC(b_+15, b_+17); hram_wr(gb, hCameraX & 0xff, A);
    CYC(b_+17, b_+18); ret_effect(gb); return;
  }
  CYC(b_+18, b_+21); A = mem_rd(gb, wCameraFocusedObject);
  CYC(b_+21, b_+22); D = A;
  CYC(b_+22, b_+25); A = mem_rd(gb, wCameraFocusedObjectType);
  CYC(b_+25, b_+27); alu_add(gb, 0x0b);
  CYC(b_+27, b_+28); E = A;
  CYC(b_+28, b_+31); SET_HL(hCameraY);
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_sub(gb, 0x40);
  if (!(F & FC)) CYCT(b_+34, b_+36); else { CYC(b_+34, b_+36); CYC(b_+36, b_+37); alu_xor(gb, A); }
  CYC(b_+37, b_+39); alu_cp(gb, 0x30);
  if (F & FC) CYCT(b_+39, b_+41); else { CYC(b_+39, b_+41); CYC(b_+41, b_+43); A = 0x30; }
  CALL_C(b_+43, updateCameraPosition__updateComponent_hook, b_+63, b_+46);
  CYC(b_+46, b_+49); SET_HL(hCameraX);
  CYC(b_+49, b_+50); SET_DE(DE + 1);
  CYC(b_+50, b_+51); SET_DE(DE + 1);
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+54); alu_sub(gb, 0x50);
  if (!(F & FC)) CYCT(b_+54, b_+56); else { CYC(b_+54, b_+56); CYC(b_+56, b_+57); alu_xor(gb, A); }
  CYC(b_+57, b_+59); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(b_+59, b_+61); updateCameraPosition__updateComponent_hook(gb); return; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); A = 0x50;
  TAIL(updateCameraPosition__updateComponent);
}

void updateCameraPosition_hook(GB *gb) { gb->hook_pc = SYM(updateCameraPosition); update_camera_position_body_hook(gb); }
void updateCameraPosition__smallRoom_hook(GB *gb) { gb->hook_pc = SYM(updateCameraPosition__smallRoom); update_camera_position_body_hook(gb); }
void updateCameraPosition__largeRoom_hook(GB *gb) { gb->hook_pc = SYM(updateCameraPosition__largeRoom); update_camera_position_body_hook(gb); }

void updateCameraPosition__updateComponent_hook(GB *gb) {
  BASE(updateCameraPosition);
  CYC(b_+63, b_+64); B = A;
  CYC(b_+64, b_+67); A = mem_rd(gb, wTextIsActive);
  CYC(b_+67, b_+68); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); updateCameraPosition__smBit7_hook(gb); return; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+72, b_+73); ret_effect(gb); return; }
  CYC(b_+72, b_+73);
  if (F & FC) { CYCT(b_+73, b_+75); CYC(b_+78, b_+79); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); updateCameraPosition__smBit7_hook(gb); return; }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+76, b_+78); TAIL(updateCameraPosition__smBit7);
}

void updateCameraPosition__smBit7_hook(GB *gb) {
  BASE(updateCameraPosition);
  CYC(b_+79, b_+82); SET_HL(wScrollMode);
  CYC(b_+82, b_+84); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+84, b_+85); ret_effect(gb);
}

void calculateCameraPosition_hook(GB *gb) {
  BASE(calculateCameraPosition);
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+3, b_+4); D = A;
  CYC(b_+4, b_+6); E = 0x0b;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_sub(gb, 0x40);
  if (!(F & FC)) CYCT(b_+9, b_+11); else { CYC(b_+9, b_+11); CYC(b_+11, b_+12); alu_xor(gb, A); }
  CYC(b_+12, b_+15); SET_HL(wMaxCameraX);
  CYC(b_+15, b_+16); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) CYCT(b_+16, b_+18); else { CYC(b_+16, b_+18); CYC(b_+18, b_+19); A = mem_rd(gb, HL); }
  CYC(b_+19, b_+21); hram_wr(gb, hCameraY & 0xff, A);
  CYC(b_+21, b_+23); E = 0x0d;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_sub(gb, 0x50);
  if (!(F & FC)) CYCT(b_+26, b_+28); else { CYC(b_+26, b_+28); CYC(b_+28, b_+29); alu_xor(gb, A); }
  CYC(b_+29, b_+32); SET_HL(wMaxCameraY);
  CYC(b_+32, b_+33); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) CYCT(b_+33, b_+35); else { CYC(b_+33, b_+35); CYC(b_+35, b_+36); A = mem_rd(gb, HL); }
  CYC(b_+36, b_+38); hram_wr(gb, hCameraX & 0xff, A);
  CYC(b_+38, b_+39); ret_effect(gb);
}

void updateScreenShake_hook(GB *gb) {
  BASE(updateScreenShake);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) CYCT(b_+4, b_+6); else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+9); A = mem_rd(gb, wLinkPlayingInstrument);
    CYC(b_+9, b_+10); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
    CYC(b_+10, b_+11);
  }
  CYC(b_+11, b_+14); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) CYCT(b_+15, b_+17); else {
    CYC(b_+15, b_+17);
    CALL_C(b_+17, updateScreenShake__getShakeAmount_hook, b_+51, b_+20);
    CYC(b_+20, b_+23); A = mem_rd(gb, wGfxRegs2_SCY);
    CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+24, b_+27); mem_wr(gb, wGfxRegs2_SCY, A);
    CYC(b_+27, b_+30); SET_HL(wScreenShakeCounterY);
    CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+31, b_+34); A = mem_rd(gb, wScreenShakeCounterX);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+35, b_+36); ret_effect(gb); return; }
  CYC(b_+35, b_+36);
  CALL_C(b_+36, updateScreenShake__getShakeAmount_hook, b_+51, b_+39);
  CYC(b_+39, b_+42); A = mem_rd(gb, wGfxRegs2_SCX);
  CYC(b_+42, b_+43); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+43, b_+46); mem_wr(gb, wGfxRegs2_SCX, A);
  CYC(b_+46, b_+49); SET_HL(wScreenShakeCounterX);
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+50, b_+51); ret_effect(gb);
}

void updateScreenShake__getShakeAmount_hook(GB *gb) {
  BASE(updateScreenShake);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+51, b_+54); A = mem_rd(gb, wScreenShakeMagnitude);
  CYC(b_+54, b_+55); alu_add(gb, A);
  CYC(b_+55, b_+58); SET_HL(b_+66);
  CYC(b_+58, b_+59); bank1_add_double_index_to_hl_from_rst(gb, b_+59);
  CALL_C(b_+59, getRandomNumber_hook, SYM(getRandomNumber), b_+62);
  CYC(b_+62, b_+64); alu_and(gb, 0x03);
  CYC(b_+64, b_+65); bank1_add_a_to_hl_from_rst(gb, b_+65);
  CYC(b_+65, b_+66); ret_effect(gb);
}

void updateGfxRegs2Scroll_hook(GB *gb) {
  BASE(updateGfxRegs2Scroll);
  CYC(b_+0, b_+2); A = hram_rd(gb, hCameraY & 0xff);
  CYC(b_+2, b_+3); B = A;
  CYC(b_+3, b_+6); A = mem_rd(gb, wScreenOffsetY);
  CYC(b_+6, b_+7); alu_add(gb, B);
  CYC(b_+7, b_+9); alu_sub(gb, 0x10);
  CYC(b_+9, b_+12); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(b_+12, b_+14); A = hram_rd(gb, hCameraX & 0xff);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = mem_rd(gb, wScreenOffsetX);
  CYC(b_+18, b_+19); alu_add(gb, B);
  CYC(b_+19, b_+22); mem_wr(gb, wGfxRegs2_SCX, A);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void screenTransitionState3_hook(GB *gb) {
  BASE(screenTransitionState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, loadTilesetAnimation_hook, SYM(loadTilesetAnimation), b_+12);
  CALL_C(b_+12, checkDarkenRoom_hook, SYM(checkDarkenRoom), b_+15);
  CYC(b_+15, b_+17); B = 0x05;
  CYC(b_+17, b_+20); A = mem_rd(gb, wTilesetUniqueGfx);
  CYC(b_+20, b_+22); alu_bit(gb, 7, A);
  if (!(F & FZ)) CYCT(b_+22, b_+24); else {
    CYC(b_+22, b_+24);
    CYC(b_+24, b_+25); alu_or(gb, A);
    if (F & FZ) CYCT(b_+25, b_+27); else {
      CYC(b_+25, b_+27);
      CALL_C(b_+27, loadUniqueGfxHeader_hook, SYM(loadUniqueGfxHeader), b_+30);
      CYC(b_+30, b_+32); B = 0x04;
    }
  }
  CYC(b_+32, b_+35); SET_HL(wScreenTransitionState);
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+42); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+42, b_+43); ret_effect(gb);
}

void checkDarkenRoomAndClearPaletteFadeState_hook(GB *gb) {
  BASE(checkDarkenRoomAndClearPaletteFadeState);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wPaletteThread_parameter, A);
  TAIL(checkDarkenRoom);
}

void checkDarkenRoom_hook(GB *gb) {
  BASE(checkDarkenRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, getThisRoomDungeonProperties_hook, SYM(getThisRoomDungeonProperties), b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wDungeonRoomProperties);
  CYC(b_+12, b_+14); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+18); TAIL(darkenRoom);
}

void screenTransitionState4_hook(GB *gb) {
  BASE(screenTransitionState4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateTilesetUniqueGfx_hook, SYM(updateTilesetUniqueGfx), b_+3);
  if (F & FC) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wTilesetUniqueGfx);
  CYC(b_+7, b_+10); mem_wr(gb, wLoadedTilesetUniqueGfx, A);
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+14); mem_wr(gb, wTilesetUniqueGfx, A);
  CALL_C(b_+14, func_47fc_hook, SYM(func_47fc), b_+17);
  if (!(F & FC)) CALL_C_CC(b_+17, updateTilesetPalette_hook, SYM(updateTilesetPalette), b_+20);
  else CYC(b_+17, b_+20);
  CYC(b_+20, b_+23); SET_HL(wScreenTransitionState);
  CYC(b_+23, b_+25); A = 0x05;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+31); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+31, b_+32); ret_effect(gb);
}

void screenTransitionState5_hook(GB *gb) {
  BASE(screenTransitionState5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenTransitionState2);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(screenTransitionState5Substate0) && hook_is(gb, SYM(screenTransitionState5Substate0), screenTransitionState5Substate0_hook)) { screenTransitionState5Substate0_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState5Substate1) && hook_is(gb, SYM(screenTransitionState5Substate1), screenTransitionState5Substate1_hook)) { screenTransitionState5Substate1_hook(gb); return; }
    else if (jt_ == SYM(screenTransitionState5Substate2) && hook_is(gb, SYM(screenTransitionState5Substate2), screenTransitionState5Substate2_hook)) { screenTransitionState5Substate2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void screenTransitionState5Substate0_hook(GB *gb) {
  BASE(screenTransitionState5Substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+8, b_+10); alu_and(gb, 0x01);
  if (!(F & FZ)) CALL_C_CC(b_+10, checkAndApplyPaletteFadeTransition_hook, SYM(checkAndApplyPaletteFadeTransition), b_+13);
  else CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wcd01);
  CYC(b_+16, b_+18); A = alu_swap(gb, A);
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+22); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+22, b_+23); alu_add(gb, A);
  CYC(b_+23, b_+24); alu_add(gb, A);
  CYC(b_+24, b_+25); alu_add(gb, L);
  CYC(b_+25, b_+28); SET_HL(b_+84);
  CYC(b_+28, b_+29); bank1_add_a_to_hl_from_rst(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+33); mem_wr(gb, wScreenScrollRow, A);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+37); mem_wr(gb, wScreenScrollVramRow, A);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+41); mem_wr(gb, wScreenScrollDirection, A);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+45); mem_wr(gb, wcd14, A);
  CALL_C(b_+45, resetCamera_hook, SYM(resetCamera), b_+48);
  CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+52); mem_wr(gb, wScreenTransitionState3, A);
  CALL_C(b_+52, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+55);
  CYC(b_+55, b_+58); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+58, b_+60); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+60, b_+62); screenTransitionState5Substate0__vertical_hook(gb); return; }
  CYC(b_+60, b_+62); TAIL(screenTransitionState5Substate0__horizontal);
}

void screenTransitionState5Substate0__horizontal_hook(GB *gb) {
  BASE(screenTransitionState5Substate0);
  CYC(b_+62, b_+64); A = 0x14;
  CYC(b_+64, b_+67); mem_wr(gb, wScreenScrollCounter, A);
  CYC(b_+67, b_+69); A = 0x02;
  CYC(b_+69, b_+72); mem_wr(gb, wScreenTransitionState2, A);
  CYC(b_+72, b_+73); ret_effect(gb);
}

void screenTransitionState5Substate0__vertical_hook(GB *gb) {
  BASE(screenTransitionState5Substate0);
  CYC(b_+73, b_+75); A = 0x10;
  CYC(b_+75, b_+78); mem_wr(gb, wScreenScrollCounter, A);
  CYC(b_+78, b_+80); A = 0x01;
  CYC(b_+80, b_+83); mem_wr(gb, wScreenTransitionState2, A);
  CYC(b_+83, b_+84); ret_effect(gb);
}

void transitionUpdateScrollAndLinkPosition_hook(GB *gb) {
  BASE(transitionUpdateScrollAndLinkPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wGfxRegs2_SCY);
  CYC(b_+3, b_+6); SET_HL(hCameraY);
  if (!(F & FC)) CYCT(b_+6, b_+8);
  else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); E = alu_inc8(gb, E);
    CYC(b_+9, b_+10); L = alu_inc8(gb, L);
    CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  }
  CYC(b_+11, b_+13); B = 0x00;
  CYC(b_+13, b_+16); A = mem_rd(gb, wcd14);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+18); alu_rlca(gb);
  if (!(F & FC)) CYCT(b_+18, b_+20);
  else { CYC(b_+18, b_+20); CYC(b_+20, b_+21); B = alu_dec8(gb, B); }
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_add(gb, C);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_add(gb, C);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_adc(gb, B);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CALL_C(b_+30, cpLinkState0e_hook, SYM(cpLinkState0e), b_+33);
  if (F & FZ) { CYCT(b_+33, b_+34); ret_effect(gb); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+37); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+37, b_+38); alu_add(gb, A);
  CYC(b_+38, b_+41); SET_DE(b_+66);
  CALL_C(b_+41, addDoubleIndexToDe_hook, 0x0072, b_+44);
  CYC(b_+44, b_+47); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+47, b_+48); H = A;
  CYC(b_+48, b_+50); L = 0x0a;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+54); SET_DE(DE + 1);
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+56, b_+57); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+57, b_+58); SET_DE(DE + 1);
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+60, b_+61); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+61, b_+62); SET_DE(DE + 1);
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+64); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+64, b_+65); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+65, b_+66); ret_effect(gb);
}

void finishScrollingTransition_hook(GB *gb) {
  BASE(finishScrollingTransition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, cpLinkState0e_hook, SYM(cpLinkState0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wcd01);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rrca(gb);
  CYC(b_+10, b_+11); E = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+16); alu_add(gb, E);
  CYC(b_+16, b_+19); SET_DE(GV(SYM(label_01_037__positionOffsets), 0x4454));
  CALL_C(b_+19, addAToDe_hook, 0x0068, SYM(label_01_037));
  TAIL(label_01_037);
}

void label_01_037_hook(GB *gb) {
  BASE(label_01_037);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = 0x0b;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+12); mem_wr(gb, wLinkLocalRespawnY, A);
  CYC(b_+12, b_+13); SET_DE(DE + 1);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+20); mem_wr(gb, wLinkLocalRespawnX, A);
  CYC(b_+20, b_+22); L = 0x08;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+26); mem_wr(gb, wLinkLocalRespawnDir, A);
  CYC(b_+26, b_+28); H = alu_srl(gb, H);
  if (!(F & FC)) CYCT(b_+28, b_+30);
  else {
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+33); SET_HL(wLastAnimalMountPointY);
    CYC(b_+33, b_+36); A = mem_rd(gb, wLinkLocalRespawnY);
    CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+37, b_+40); A = mem_rd(gb, wLinkLocalRespawnX);
    CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  }
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CYC(b_+42, b_+44); hram_wr(gb, hCameraY & 0xff, A);
  CYC(b_+44, b_+46); hram_wr(gb, hCameraX & 0xff, A);
  CYC(b_+46, b_+48); hram_wr(gb, (hCameraY + 1) & 0xff, A);
  CYC(b_+48, b_+50); hram_wr(gb, (hCameraX + 1) & 0xff, A);
  CALL_C(b_+50, resetFollowingLinkObjectPosition_hook, SYM(resetFollowingLinkObjectPosition), b_+53);
  CALL_C(b_+53, clearObjectsWithEnabled2_hook, SYM(clearObjectsWithEnabled2), b_+56);
  CYC(b_+56, b_+58); A = 0x01;
  CYC(b_+58, b_+61); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CALL_C(b_+61, calculateCameraPosition_hook, SYM(calculateCameraPosition), b_+64);
  CYC(b_+64, b_+67); TAIL(updateGfxRegs2Scroll);
}

void func_4493_hook(GB *gb) {
  BASE(func_4493);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+3, b_+6); SET_DE(b_+11);
  CALL_C(b_+6, addDoubleIndexToDe_hook, 0x0072, b_+9);
  CYC(b_+9, b_+11); TAIL(label_01_037);
}

void resetFollowingLinkObjectPosition_hook(GB *gb) {
  BASE(resetFollowingLinkObjectPosition);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFollowingLinkObject);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, w1Link_yh);
  CYC(b_+8, b_+9); D = A;
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link_xh);
  CYC(b_+12, b_+13); E = A;
  CYC(b_+13, b_+16); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+16, b_+18); alu_and(gb, 0x03);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+22); SET_HL(b_+76);
  CYC(b_+22, b_+23); bank1_add_double_index_to_hl_from_rst(gb, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+26); hram_wr(gb, hFF8D & 0xff, A);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); hram_wr(gb, hFF8C & 0xff, A);
  CYC(b_+29, b_+31); A = 0x02;
  CYC(b_+31, b_+33); mem_wr(gb, IO_SVBK, A);
  CYC(b_+33, b_+36); SET_HL(w2LinkWalkPath + 0x2f);
  CYC(b_+36, b_+38); B = 0x10;
  do {
    CYC(b_+38, b_+40); A = hram_rd(gb, hFF8C & 0xff);
    CYC(b_+40, b_+41); alu_add(gb, E);
    CYC(b_+41, b_+42); E = A;
    CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(b_+43, b_+45); A = hram_rd(gb, hFF8D & 0xff);
    CYC(b_+45, b_+46); alu_add(gb, D);
    CYC(b_+46, b_+47); D = A;
    CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(b_+48, b_+49); A = C;
    CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(b_+50, b_+51); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+51, b_+53); else CYC(b_+51, b_+53);
  } while (!(F & FZ));
  CYC(b_+53, b_+54); alu_xor(gb, A);
  CYC(b_+54, b_+56); mem_wr(gb, IO_SVBK, A);
  CYC(b_+56, b_+59); A = mem_rd(gb, wFollowingLinkObjectType);
  CYC(b_+59, b_+61); alu_add(gb, 0x0b);
  CYC(b_+61, b_+62); L = A;
  CYC(b_+62, b_+65); A = mem_rd(gb, wFollowingLinkObject);
  CYC(b_+65, b_+66); H = A;
  CYC(b_+66, b_+67); mem_wr(gb, HL, D);
  CYC(b_+67, b_+68); L = alu_inc8(gb, L);
  CYC(b_+68, b_+69); L = alu_inc8(gb, L);
  CYC(b_+69, b_+70); mem_wr(gb, HL, E);
  CYC(b_+70, b_+72); A = 0x0f;
  CYC(b_+72, b_+75); mem_wr(gb, wLinkPathIndex, A);
  CYC(b_+75, b_+76); ret_effect(gb);
}

void screenTransitionState5Substate2_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenTransitionState3);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+16) { screenTransitionState5Substate2__state0_hook(gb); return; }
    else if (jt_ == b_+38) { screenTransitionState5Substate2__state1_hook(gb); return; }
    else if (jt_ == b_+46) { screenTransitionState5Substate2__state2_hook(gb); return; }
    else if (jt_ == b_+76) { screenTransitionState5Substate2__state3_hook(gb); return; }
    else if (jt_ == b_+95) { screenTransitionState5Substate2__state4_hook(gb); return; }
    else if (jt_ == b_+109) { screenTransitionState5Substate2__state5_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void screenTransitionState5Substate2__state0_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  CYC(b_+16, b_+19); A = mem_rd(gb, wScreenOffsetX);
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, b_+22); alu_rlca(gb);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+26); A = mem_rd(gb, wScreenScrollVramRow);
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+29); alu_and(gb, 0x1f);
  CYC(b_+29, b_+32); mem_wr(gb, wScreenScrollVramRow, A);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+37); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void screenTransitionState5Substate2__state1_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  CYC(b_+38, b_+40); A = 0x02;
  CYC(b_+40, b_+43); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+43, b_+46); TAIL(screenTransitionState5Substate2__drawNextRow);
}

void screenTransitionState5Substate2__state2_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+46, b_+47); alu_scf(gb);
  CALL_C(b_+47, transitionUpdateScrollAndLinkPosition_hook, SYM(transitionUpdateScrollAndLinkPosition), b_+50);
  CYC(b_+50, b_+53); A = mem_rd(gb, wGfxRegs2_SCX);
  CYC(b_+53, b_+55); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+55, b_+56); ret_effect(gb); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+59); A = mem_rd(gb, wScreenScrollCounter);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); screenTransitionState5Substate2__drawNextRow_hook(gb); return; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+65); SET_HL(wScreenTransitionState3);
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+66, b_+69); A = mem_rd(gb, wMaxCameraY);
  CYC(b_+69, b_+71); A = alu_swap(gb, A);
  CYC(b_+71, b_+72); alu_rlca(gb);
  CYC(b_+72, b_+75); mem_wr(gb, wScreenScrollCounter, A);
  CYC(b_+75, b_+76); ret_effect(gb);
}

void screenTransitionState5Substate2__state3_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  CYC(b_+76, b_+79); A = mem_rd(gb, wScreenScrollCounter);
  CYC(b_+79, b_+80); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+80, b_+82); screenTransitionState5Substate2__drawNextRow_hook(gb); return; }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+85); SET_HL(wScreenTransitionState3);
  CYC(b_+85, b_+86); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+86, b_+89); A = mem_rd(gb, wTilesetUniqueGfx);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+90, b_+93); loadUniqueGfxHeader_hook(gb); return; }
  CYC(b_+90, b_+93);
  CYC(b_+93, b_+94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+94, b_+95); ret_effect(gb);
}

void screenTransitionState5Substate2__state4_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+95, updateTilesetUniqueGfx_hook, SYM(updateTilesetUniqueGfx), b_+98);
  if (F & FC) { CYCT(b_+98, b_+99); ret_effect(gb); return; }
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+102); A = mem_rd(gb, wTilesetUniqueGfx);
  CYC(b_+102, b_+105); mem_wr(gb, wLoadedTilesetUniqueGfx, A);
  CYC(b_+105, b_+106); alu_xor(gb, A);
  CYC(b_+106, b_+109); mem_wr(gb, wTilesetUniqueGfx, A);
  TAIL(screenTransitionState5Substate2__state5);
}

void screenTransitionState5Substate2__state5_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+109, checkBrightenRoom_hook, SYM(checkBrightenRoom), b_+112);
  CALL_C(b_+112, updateTilesetPalette_hook, SYM(updateTilesetPalette), b_+115);
  CALL_C(b_+115, setInstrumentsDisabledCounterAndScrollMode_hook, SYM(setInstrumentsDisabledCounterAndScrollMode), b_+118);
  CYC(b_+118, b_+119); alu_xor(gb, A);
  CYC(b_+119, b_+122); mem_wr(gb, wScreenTransitionState2, A);
  CYC(b_+122, b_+125); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+125, b_+127); A = 0x02;
  CYC(b_+127, b_+130); mem_wr(gb, wScreenTransitionState, A);
  CYC(b_+130, b_+133); A = mem_rd(gb, wRoomWidth);
  CYC(b_+133, b_+134); alu_add(gb, A);
  CYC(b_+134, b_+135); alu_add(gb, A);
  CYC(b_+135, b_+136); alu_add(gb, A);
  CYC(b_+136, b_+137); B = A;
  CYC(b_+137, b_+140); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+140, b_+142); alu_and(gb, 0x02);
  if (F & FZ) CYCT(b_+142, b_+144);
  else {
    CYC(b_+142, b_+144);
    CYC(b_+144, b_+145); A = B;
    CYC(b_+145, b_+146); alu_cpl(gb);
    CYC(b_+146, b_+147); A = alu_inc8(gb, A);
    CYC(b_+147, b_+148); B = A;
  }
  CYC(b_+148, b_+151); A = mem_rd(gb, wScreenOffsetX);
  CYC(b_+151, b_+152); alu_add(gb, B);
  CYC(b_+152, b_+155); mem_wr(gb, wScreenOffsetX, A);
  CYC(b_+155, b_+158); TAIL(finishScrollingTransition);
}

void screenTransitionState5Substate2__drawNextRow_hook(GB *gb) {
  BASE(screenTransitionState5Substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+158, b_+161); A = mem_rd(gb, wScreenScrollRow);
  CYC(b_+161, b_+162); E = A;
  CALL_C(b_+162, func_46ca_hook, SYM(func_46ca), b_+165);
  CYC(b_+165, b_+168); A = mem_rd(gb, wScreenScrollVramRow);
  CALL_C(b_+168, addFunctionsToVBlankQueue_hook, SYM(addFunctionsToVBlankQueue), SYM(incrementScreenScrollRowVars));
  TAIL(incrementScreenScrollRowVars);
}

void screenTransitionState5Substate1_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenTransitionState3);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+16) { screenTransitionState5Substate1__state0_hook(gb); return; }
    else if (jt_ == b_+38) { screenTransitionState5Substate1__state1_hook(gb); return; }
    else if (jt_ == b_+46) { screenTransitionState5Substate1__state2_hook(gb); return; }
    else if (jt_ == b_+76) { screenTransitionState5Substate1__state3_hook(gb); return; }
    else if (jt_ == b_+95) { screenTransitionState5Substate1__state4_hook(gb); return; }
    else if (jt_ == b_+109) { screenTransitionState5Substate1__state5_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void screenTransitionState5Substate1__state0_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  CYC(b_+16, b_+19); A = mem_rd(gb, wScreenOffsetY);
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, b_+22); alu_rlca(gb);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+26); A = mem_rd(gb, wScreenScrollVramRow);
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+29); alu_and(gb, 0x1f);
  CYC(b_+29, b_+32); mem_wr(gb, wScreenScrollVramRow, A);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+37); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void screenTransitionState5Substate1__state1_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  CYC(b_+38, b_+40); A = 0x02;
  CYC(b_+40, b_+43); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+43, b_+46); TAIL(screenTransitionState5Substate1__drawNextRow);
}

void screenTransitionState5Substate1__state2_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CALL_C(b_+47, transitionUpdateScrollAndLinkPosition_hook, SYM(transitionUpdateScrollAndLinkPosition), b_+50);
  CYC(b_+50, b_+53); A = mem_rd(gb, wGfxRegs2_SCY);
  CYC(b_+53, b_+55); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+55, b_+56); ret_effect(gb); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+59); A = mem_rd(gb, wScreenScrollCounter);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); screenTransitionState5Substate1__drawNextRow_hook(gb); return; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+65); SET_HL(wScreenTransitionState3);
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+66, b_+69); A = mem_rd(gb, wMaxCameraX);
  CYC(b_+69, b_+71); A = alu_swap(gb, A);
  CYC(b_+71, b_+72); alu_rlca(gb);
  CYC(b_+72, b_+75); mem_wr(gb, wScreenScrollCounter, A);
  CYC(b_+75, b_+76); ret_effect(gb);
}

void screenTransitionState5Substate1__state3_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  CYC(b_+76, b_+79); A = mem_rd(gb, wScreenScrollCounter);
  CYC(b_+79, b_+80); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+80, b_+82); screenTransitionState5Substate1__drawNextRow_hook(gb); return; }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+85); SET_HL(wScreenTransitionState3);
  CYC(b_+85, b_+86); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+86, b_+89); A = mem_rd(gb, wTilesetUniqueGfx);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+90, b_+93); loadUniqueGfxHeader_hook(gb); return; }
  CYC(b_+90, b_+93);
  CYC(b_+93, b_+94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+94, b_+95); ret_effect(gb);
}

void screenTransitionState5Substate1__state4_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+95, updateTilesetUniqueGfx_hook, SYM(updateTilesetUniqueGfx), b_+98);
  if (F & FC) { CYCT(b_+98, b_+99); ret_effect(gb); return; }
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+102); A = mem_rd(gb, wTilesetUniqueGfx);
  CYC(b_+102, b_+105); mem_wr(gb, wLoadedTilesetUniqueGfx, A);
  CYC(b_+105, b_+106); alu_xor(gb, A);
  CYC(b_+106, b_+109); mem_wr(gb, wTilesetUniqueGfx, A);
  TAIL(screenTransitionState5Substate1__state5);
}

void screenTransitionState5Substate1__state5_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+109, checkBrightenRoom_hook, SYM(checkBrightenRoom), b_+112);
  CALL_C(b_+112, updateTilesetPalette_hook, SYM(updateTilesetPalette), b_+115);
  CALL_C(b_+115, setInstrumentsDisabledCounterAndScrollMode_hook, SYM(setInstrumentsDisabledCounterAndScrollMode), b_+118);
  CYC(b_+118, b_+119); alu_xor(gb, A);
  CYC(b_+119, b_+122); mem_wr(gb, wScreenTransitionState2, A);
  CYC(b_+122, b_+125); mem_wr(gb, wScreenTransitionState3, A);
  CYC(b_+125, b_+127); A = 0x02;
  CYC(b_+127, b_+130); mem_wr(gb, wScreenTransitionState, A);
  CYC(b_+130, b_+133); A = mem_rd(gb, wcd01);
  CYC(b_+133, b_+134); alu_add(gb, A);
  CYC(b_+134, b_+135); alu_add(gb, A);
  CYC(b_+135, b_+136); L = A;
  CYC(b_+136, b_+139); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+139, b_+140); alu_add(gb, L);
  CYC(b_+140, b_+143); SET_HL(b_+159);
  CYC(b_+143, b_+144); bank1_add_a_to_hl_from_rst(gb, b_+144);
  CYC(b_+144, b_+145); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+145, b_+147); hram_wr(gb, hCameraY & 0xff, A);
  CYC(b_+147, b_+148); A = mem_rd(gb, HL);
  CYC(b_+148, b_+149); B = A;
  CYC(b_+149, b_+152); A = mem_rd(gb, wScreenOffsetY);
  CYC(b_+152, b_+153); alu_add(gb, B);
  CYC(b_+153, b_+156); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+156, b_+159); TAIL(finishScrollingTransition);
}

void screenTransitionState5Substate1__drawNextRow_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+167, b_+170); A = mem_rd(gb, wScreenScrollRow);
  CYC(b_+170, b_+171); E = A;
  CALL_C(b_+171, copyTileRowToVramBuffer_hook, SYM(copyTileRowToVramBuffer), b_+174);
  CYC(b_+174, b_+176); C = 0x01;
  CALL_C(b_+176, screenTransitionState5Substate1__queueRowDmaTransfer_hook, b_+187, b_+179);
  CYC(b_+179, b_+181); C = 0x00;
  CALL_C(b_+181, screenTransitionState5Substate1__queueRowDmaTransfer_hook, b_+187, b_+184);
  CYC(b_+184, b_+187); TAIL(incrementScreenScrollRowVars);
}

void screenTransitionState5Substate1__queueRowDmaTransfer_hook(GB *gb) {
  BASE(screenTransitionState5Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+187, b_+190); A = mem_rd(gb, wScreenScrollVramRow);
  CYC(b_+190, b_+191); B = A;
  CYC(b_+191, b_+193); alu_and(gb, 0x18);
  CYC(b_+193, b_+194); alu_rlca(gb);
  CYC(b_+194, b_+196); A = alu_swap(gb, A);
  CYC(b_+196, b_+198); alu_add(gb, 0x98);
  CYC(b_+198, b_+199); D = A;
  CYC(b_+199, b_+200); A = B;
  CYC(b_+200, b_+202); alu_and(gb, 0x07);
  CYC(b_+202, b_+204); A = alu_swap(gb, A);
  CYC(b_+204, b_+205); alu_rlca(gb);
  CYC(b_+205, b_+206); alu_or(gb, C);
  CYC(b_+206, b_+207); E = A;
  CYC(b_+207, b_+210); SET_HL(wTmpVramBuffer);
  CYC(b_+210, b_+212); C = alu_srl(gb, C);
  if (!(F & FC)) CYCT(b_+212, b_+214);
  else { CYC(b_+212, b_+214); CYC(b_+214, b_+216); L = 0x60; }
  CYC(b_+216, b_+218); B = 0x01;
  CYC(b_+218, b_+221); TAIL(queueDmaTransfer);
}

void clearObjectsWithEnabled2_hook(GB *gb) {
  BASE(clearObjectsWithEnabled2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearInteractionsWithEnabled2_hook, SYM(clearInteractionsWithEnabled2), b_+3);
  CALL_C(b_+3, clearEnemiesWithEnabled2_hook, SYM(clearEnemiesWithEnabled2), b_+6);
  CALL_C(b_+6, clearPartsWithEnabled2_hook, SYM(clearPartsWithEnabled2), b_+9);
  CALL_C(b_+9, clearItemsWithEnabled2_hook, SYM(clearItemsWithEnabled2), b_+12);
  CYC(b_+12, b_+15); SET_HL(w1Link);
  CYC(b_+15, b_+17); C = 0xd2;
  CYC(b_+17, b_+19); TAIL(clearObjectsWithEnabled2_hlpr);
}

void clearItemsWithEnabled2_hook(GB *gb) {
  BASE(clearItemsWithEnabled2);
  CYC(b_+0, b_+3); SET_HL(w1WeaponItem);
  CYC(b_+3, b_+5); C = 0xe0;
  CYC(b_+5, b_+7); TAIL(clearObjectsWithEnabled2_hlpr);
}

void clearInteractionsWithEnabled2_hook(GB *gb) {
  BASE(clearInteractionsWithEnabled2);
  CYC(b_+0, b_+3); SET_HL(w1ReservedInteraction0);
  CYC(b_+3, b_+5); C = 0xe0;
  CYC(b_+5, b_+7); TAIL(clearObjectsWithEnabled2_hlpr);
}

void clearEnemiesWithEnabled2_hook(GB *gb) {
  BASE(clearEnemiesWithEnabled2);
  CYC(b_+0, b_+3); SET_HL(ENEMY_SLOTS);
  CYC(b_+3, b_+5); C = 0xe0;
  CYC(b_+5, b_+7); TAIL(clearObjectsWithEnabled2_hlpr);
}

void clearPartsWithEnabled2_hook(GB *gb) {
  BASE(clearPartsWithEnabled2);
  CYC(b_+0, b_+3); SET_HL(PART_SLOTS);
  CYC(b_+3, b_+5); C = 0xe0;
  TAIL(clearObjectsWithEnabled2_hlpr);
}

void clearObjectsWithEnabled2_hlpr_hook(GB *gb) {
  BASE(clearObjectsWithEnabled2_hlpr);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL);
    CYC(b_+1, b_+3); alu_and(gb, 0x03);
    CYC(b_+3, b_+5); alu_cp(gb, 0x02);
    if (!(F & FZ)) CYCT(b_+5, b_+7);
    else {
      CYC(b_+5, b_+7);
      CYC(b_+7, b_+8); push_effect(gb, HL);
      CYC(b_+8, b_+10); B = 0x40;
      CALL_C(b_+10, clearMemory_hook, SYM(clearMemory), b_+13);
      CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
    }
    CYC(b_+14, b_+15); H = alu_inc8(gb, H);
    CYC(b_+15, b_+16); A = H;
    CYC(b_+16, b_+17); alu_cp(gb, C);
    if (F & FC) { CYCT(b_+17, b_+19); continue; }
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+20); ret_effect(gb);
    return;
  }
}

void playCompassSoundIfKeyInRoom_hook(GB *gb) {
  BASE(playCompassSoundIfKeyInRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(3)); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+O(3), b_+OE(4)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(4), b_+OE(5)); ret_effect(gb); return; }
  CYC(b_+O(4), b_+OE(5));
  CYC(b_+O(5), b_+OE(8)); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+O(8), b_+OE(10)); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+O(10), b_+OE(11)); ret_effect(gb); return; }
  CYC(b_+O(10), b_+OE(11));
  CYC(b_+O(11), b_+OE(14)); SET_HL(wDungeonCompasses);
  CALL_C(b_+O(14), checkFlag_hook, SYM(checkFlag), b_+OE(17));
  if (F & FZ) { CYCT(b_+O(17), b_+OE(18)); ret_effect(gb); return; }
  CYC(b_+O(17), b_+OE(18));
  CALL_C(b_+O(18), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(21));
  CYC(b_+O(21), b_+OE(23)); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+O(23), b_+OE(24)); ret_effect(gb); return; }
  CYC(b_+O(23), b_+OE(24));
  if (game_seasons) {      // the Seasons compass also chimes in dungeon room 06:8b
    CYC(b_+S(24), b_+S(27)); A = W8(wActiveGroup);
    CYC(b_+S(27), b_+S(29)); alu_cp(gb, 0x06);
    if (!(F & FZ)) CYCT(b_+S(29), b_+S(31));
    else {
      CYC(b_+S(29), b_+S(31));
      CYC(b_+S(31), b_+S(34)); A = W8(wActiveRoom);
      CYC(b_+S(34), b_+S(36)); alu_cp(gb, 0x8b);
      if (F & FZ) { CYCT(b_+S(36), b_+S(38)); playCompassSoundIfKeyInRoom__playSound_hook(gb); return; }
      CYC(b_+S(36), b_+S(38));
    }
  }
  CYC(b_+O(24), b_+OE(27)); A = mem_rd(gb, wDungeonRoomProperties);
  CYC(b_+O(27), b_+OE(29)); alu_and(gb, 0x70);
  CYC(b_+O(29), b_+OE(31)); alu_cp(gb, 0x30);
  if (F & FZ) { CYCT(b_+O(31), b_+OE(33)); playCompassSoundIfKeyInRoom__playSound_hook(gb); return; }
  CYC(b_+O(31), b_+OE(33));
  CYC(b_+O(33), b_+OE(35)); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+O(35), b_+OE(36)); ret_effect(gb); return; }
  CYC(b_+O(35), b_+OE(36)); TAIL(playCompassSoundIfKeyInRoom__playSound);
}

void playCompassSoundIfKeyInRoom__playSound_hook(GB *gb) {
  BASE(playCompassSoundIfKeyInRoom);
  CYC(b_+O(36), b_+OE(38)); A = 0xa2;
  CYC(b_+O(38), b_+OE(41)); TAIL(playSound_b00);
}

void updateLinkBeingShocked_hook(GB *gb) {
  BASE(updateLinkBeingShocked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wIsLinkBeingShocked);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); bank1_jump_table_from_rst(gb, b_+5);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+11) { updateLinkBeingShocked__val00_hook(gb); return; }
    else if (jt_ == b_+12) { updateLinkBeingShocked__val01_hook(gb); return; }
    else if (jt_ == b_+38) { updateLinkBeingShocked__val02_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void updateLinkBeingShocked__val00_hook(GB *gb) {
  BASE(updateLinkBeingShocked);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void updateLinkBeingShocked__val01_hook(GB *gb) {
  BASE(updateLinkBeingShocked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x2d);
  CYC(b_+18, b_+20); A = 0xac;
  CALL_C(b_+20, playSound_b00_hook, SYM(playSound_b00), b_+23);
  CYC(b_+23, b_+26); SET_HL(wDisabledObjects);
  CYC(b_+26, b_+28); A = 0x21;
  CYC(b_+28, b_+29); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+33); SET_HL(wDisableLinkCollisionsAndMenu);
  CYC(b_+33, b_+35); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x01));
  CYC(b_+35, b_+38); TAIL(copyW2TilesetBgPalettesToW4PaletteData);
}

void updateLinkBeingShocked__val02_hook(GB *gb) {
  BASE(updateLinkBeingShocked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+40); L = E;
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    CYC(b_+63, b_+64); alu_xor(gb, A);
    CYC(b_+64, b_+65); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(b_+65, b_+66); mem_wr(gb, HL, A);
    CYC(b_+66, b_+69); SET_HL(wDisabledObjects);
    CYC(b_+69, b_+71); A = 0xde;
    CYC(b_+71, b_+72); alu_and(gb, mem_rd(gb, HL));
    CYC(b_+72, b_+73); mem_wr(gb, HL, A);
    CYC(b_+73, b_+76); SET_HL(wDisableLinkCollisionsAndMenu);
    CYC(b_+76, b_+78); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x01));
    CYC(b_+78, b_+81); copyW4PaletteDataToW2TilesetBgPalettes_hook(gb);
    return;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+47); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+50, b_+53); copyW4PaletteDataToW2TilesetBgPalettes_hook(gb); return; }
  CYC(b_+50, b_+53);
  CYC(b_+53, b_+55); A = 0x08;
  CALL_C(b_+55, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+58);
  CYC(b_+58, b_+60); A = 0x0c;
  CYC(b_+60, b_+63); TAIL(loadPaletteHeader);
}

void initiateFallDownHoleWarp_hook(GB *gb) {
  BASE(initiateFallDownHoleWarp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonFloor);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+7); mem_wr(gb, wDungeonFloor, A);
  CALL_C(b_+7, getActiveRoomFromDungeonMapPosition_hook, SYM(getActiveRoomFromDungeonMapPosition), b_+10);
  CYC(b_+10, b_+13); mem_wr(gb, wWarpDestRoom, A);
  CALL_C(b_+13, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+16);
  CYC(b_+16, b_+19); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+19, b_+22); A = mem_rd(gb, wActiveGroup);
  CYC(b_+22, b_+24); alu_or(gb, 0x80);
  CYC(b_+24, b_+27); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+27, b_+29); A = 0x05;
  CYC(b_+29, b_+32); mem_wr(gb, wWarpTransition, A);
  CYC(b_+32, b_+34); A = 0x03;
  CYC(b_+34, b_+37); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void cutscene17_hook(GB *gb) {
  BASE(cutscene17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+14) { cutscene17__state0_hook(gb); return; }
    else if (jt_ == b_+67) { cutscene17__state1_hook(gb); return; }
    else if (jt_ == b_+91) { cutscene17__state2_hook(gb); return; }
    else if (jt_ == b_+144) { cutscene17__state3_hook(gb); return; }
    else if (jt_ == b_+173) { cutscene17__state4_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void cutscene17__state0_hook(GB *gb) {
  BASE(cutscene17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+14, reloadTileMap_hook, SYM(reloadTileMap), b_+17);
  CYC(b_+17, b_+19); A = 0x01;
  CYC(b_+19, b_+22); mem_wr(gb, wCutsceneState, A);
  CYC(b_+22, b_+25); SET_HL(wInteractionSlots);
  for (;;) {
    CYC(b_+25, b_+27); L = 0x40;
    CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+28, b_+29); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+29, b_+31);
    } else {
      CYC(b_+29, b_+31);
      CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+32, b_+34); alu_cp(gb, GV(0xad, 0x44));
      if (F & FZ) {
        CYCT(b_+34, b_+36);
        break;
      }
      CYC(b_+34, b_+36);
    }
    CYC(b_+36, b_+37); H = alu_inc8(gb, H);
    CYC(b_+37, b_+38); A = H;
    CYC(b_+38, b_+40); alu_cp(gb, 0xe0);
    if (F & FC) {
      CYCT(b_+40, b_+42);
      continue;
    }
    CYC(b_+40, b_+42);
    break;
  }
  CYC(b_+42, b_+43); A = H;
  CYC(b_+43, b_+46); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+46, b_+48); A = 0x10;
  CYC(b_+48, b_+51); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+51, b_+53); A = 0x02;
  CYC(b_+53, b_+55); hram_wr(gb, hNextLcdInterruptBehaviour & 0xff, A);
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, b_+59); mem_wr(gb, wGenericCutscene_cbb7, A);
  CALL_C(b_+59, initWaveScrollValuesForEverySecondLine_hook, SYM(initWaveScrollValuesForEverySecondLine), b_+62);
  CYC(b_+62, b_+64); A = 0xb9;
  CYC(b_+64, b_+67); TAIL(playSound_b00);
}

void cutscene17__state1_hook(GB *gb) {
  BASE(cutscene17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+67, b_+69); A = 0x02;
  CALL_C(b_+69, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+72);
  CYC(b_+72, b_+75); SET_HL(wGenericCutscene_cbb7);
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(b_+77, b_+80);
    TAIL(initWaveScrollValuesForEverySecondLine);
  }
  CYC(b_+77, b_+80);
  CYC(b_+80, b_+82); A = 0x02;
  CYC(b_+82, b_+85); mem_wr(gb, wCutsceneState, A);
  CYC(b_+85, b_+87); A = 0x1e;
  CYC(b_+87, b_+90); mem_wr(gb, wGenericCutscene_cbb3, A);
  CYC(b_+90, b_+91); ret_effect(gb);
}

void cutscene17__state2_hook(GB *gb) {
  BASE(cutscene17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+91, updateInteractionsAndDrawAllSprites_hook, SYM(updateInteractionsAndDrawAllSprites), b_+94);
  CYC(b_+94, b_+96); A = 0x02;
  CALL_C(b_+96, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+99);
  CYC(b_+99, b_+102); A = mem_rd(gb, wGenericCutscene_cbb4);
  CYC(b_+102, b_+103); A = alu_inc8(gb, A);
  CYC(b_+103, b_+105); alu_and(gb, 0x03);
  CYC(b_+105, b_+108); mem_wr(gb, wGenericCutscene_cbb4, A);
  if (!(F & FZ)) { CYCT(b_+108, b_+109); ret_effect(gb); return; }
  CYC(b_+108, b_+109);
  CYC(b_+109, b_+112); A = mem_rd(gb, wGenericCutscene_cbb5);
  CYC(b_+112, b_+113); H = A;
  CYC(b_+113, b_+115); L = 0x5a;
  CYC(b_+115, b_+116); A = mem_rd(gb, HL);
  CYC(b_+116, b_+118); alu_xor(gb, 0x80);
  CYC(b_+118, b_+119); mem_wr(gb, HL, A);
  CYC(b_+119, b_+122); A = mem_rd(gb, wGenericCutscene_cbb3);
  CYC(b_+122, b_+123); A = alu_dec8(gb, A);
  CYC(b_+123, b_+126); mem_wr(gb, wGenericCutscene_cbb3, A);
  if (!(F & FZ)) { CYCT(b_+126, b_+127); ret_effect(gb); return; }
  CYC(b_+126, b_+127);
  CYC(b_+127, b_+129); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CYC(b_+129, b_+131); A = 0x14;
  CYC(b_+131, b_+134); mem_wr(gb, wGenericCutscene_cbb4, A);
  CYC(b_+134, b_+136); A = 0x05;
  CYC(b_+136, b_+139); mem_wr(gb, wGenericCutscene_cbb3, A);
  CYC(b_+139, b_+141); A = 0x03;
  CYC(b_+141, b_+144); mem_wr(gb, wCutsceneState, A);
  TAIL(cutscene17__state3);
}

void cutscene17__state3_hook(GB *gb) {
  BASE(cutscene17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+144, updateInteractionsAndDrawAllSprites_hook, SYM(updateInteractionsAndDrawAllSprites), b_+147);
  CYC(b_+147, b_+149); A = 0x02;
  CALL_C(b_+149, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+152);
  CYC(b_+152, b_+155); SET_HL(wGenericCutscene_cbb4);
  CYC(b_+155, b_+156); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+156, b_+157); ret_effect(gb); return; }
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0x14);
  CALL_C(b_+159, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+162);
  CYC(b_+162, b_+165); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+165, b_+166); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+166, b_+167); ret_effect(gb); return; }
  CYC(b_+166, b_+167);
  CYC(b_+167, b_+169); A = 0x04;
  CYC(b_+169, b_+172); mem_wr(gb, wCutsceneState, A);
  CYC(b_+172, b_+173); ret_effect(gb);
}

void cutscene17__state4_hook(GB *gb) {
  BASE(cutscene17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+173, b_+175); A = 0x02;
  CALL_C(b_+175, loadBigBufferScrollValues_hook, SYM(loadBigBufferScrollValues), b_+178);
  CYC(b_+178, b_+181); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+181, b_+182); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+182, b_+183); ret_effect(gb); return; }
  CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); SET_HL(b_+220);
  CALL_C(b_+186, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+189);
  CYC(b_+189, b_+190); alu_xor(gb, A);
  CYC(b_+190, b_+193); mem_wr(gb, wcc50, A);
  CYC(b_+193, b_+196); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+196, b_+198); A = 0x03;
  CYC(b_+198, b_+201); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+201, b_+203); A = 0x03;
  CYC(b_+203, b_+205); hram_wr(gb, hNextLcdInterruptBehaviour & 0xff, A);
  CYC(b_+205, b_+207); A = 0x01;
  CYC(b_+207, b_+210); mem_wr(gb, wScrollMode, A);
  CYC(b_+210, b_+212); A = 0xf1;
  CALL_C(b_+212, playSound_b00_hook, SYM(playSound_b00), b_+215);
  CYC(b_+215, b_+217); A = 0xb4;
  CYC(b_+217, b_+220); TAIL(playSound_b00);
}

void initWaveScrollValuesForEverySecondLine_hook(GB *gb) {
  BASE(initWaveScrollValuesForEverySecondLine);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, initWaveScrollValues_hook, SYM(initWaveScrollValues), b_+3);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+7); hram_wr(gb, IO_SVBK & 0xff, A);
  CYC(b_+7, b_+10); SET_HL(w2WaveScrollValues);
  CYC(b_+10, b_+12); B = 0x80;
  do {
    CYC(b_+12, b_+14); A = hram_rd(gb, hCameraX & 0xff);
    CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+15, b_+16); SET_HL(HL + 1);
    CYC(b_+16, b_+17); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+17, b_+19);
    else CYC(b_+17, b_+19);
  } while (!(F & FZ));
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+22); hram_wr(gb, IO_SVBK & 0xff, A);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void cutscene15__update_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); A = mem_rd(gb, wCutsceneState);
  CYC(b_+12, b_+13); bank1_jump_table_from_rst(gb, b_+13);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+32) { cutscene15__state0_hook(gb); return; }
    else if (jt_ == b_+54) { cutscene15__state1_hook(gb); return; }
    else if (jt_ == b_+257) { cutscene15__state2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void cutscene15__func_4c03_hook(GB *gb) {
  BASE(cutscene15);
  CYC(b_+19, b_+22); SET_HL(wGenericCutscene_cbb4);
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x1e);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void cutscene15__incTmpcbb3_hook(GB *gb) {
  BASE(cutscene15);
  CYC(b_+27, b_+30); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+32); ret_effect(gb);
}

void cutscene15__state0_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+32, reloadTileMap_hook, SYM(reloadTileMap), b_+35);
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+40); mem_wr(gb, wCutsceneState, A);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+44); mem_wr(gb, wGenericCutscene_cbb3, A);
  CYC(b_+44, b_+47); mem_wr(gb, wGenericCutscene_cbb4, A);
  CYC(b_+47, b_+50); mem_wr(gb, wGenericCutscene_cbb5, A);
  CYC(b_+50, b_+53); mem_wr(gb, wGenericCutscene_cbb6, A);
  CYC(b_+53, b_+54); ret_effect(gb);
}

void cutscene15__state1_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+54, b_+57); A = mem_rd(gb, wGenericCutscene_cbb3);
  CYC(b_+57, b_+58); bank1_jump_table_from_rst(gb, b_+58);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+64) { cutscene15__state1__substate0_hook(gb); return; }
    else if (jt_ == b_+112) { cutscene15__state1__substate1_hook(gb); return; }
    else if (jt_ == b_+132) { HANDOFF(HL); }
    else { HANDOFF(HL); }
  } while (0);
}

void cutscene15__state1__substate0_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+64, b_+66); A = 0x04;
  CYC(b_+66, b_+69); mem_wr(gb, wGenericCutscene_cbbb, A);
  CYC(b_+69, b_+70); alu_xor(gb, A);
  CYC(b_+70, b_+73); mem_wr(gb, wGenericCutscene_cbbc, A);
  CALL_C(b_+73, cutscene15__state1__initWaveScrollValuesInverted_hook, b_+88, b_+76);
  CYC(b_+76, b_+78); A = 0x10;
  CYC(b_+78, b_+81); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+81, b_+83); A = 0x02;
  CYC(b_+83, b_+85); hram_wr(gb, hNextLcdInterruptBehaviour & 0xff, A);
  CYC(b_+85, b_+88); TAIL(cutscene15__incTmpcbb3);
}

void cutscene15__state1__initWaveScrollValuesInverted_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+88, initWaveScrollValues_hook, SYM(initWaveScrollValues), b_+91);
  CYC(b_+91, b_+93); A = 0x02;
  CYC(b_+93, b_+95); hram_wr(gb, IO_SVBK & 0xff, A);
  CYC(b_+95, b_+98); SET_HL(w2WaveScrollValues);
  CYC(b_+98, b_+100); B = 0x80;
  do {
    CYC(b_+100, b_+101); A = mem_rd(gb, HL);
    CYC(b_+101, b_+102); alu_cpl(gb);
    CYC(b_+102, b_+103); A = alu_inc8(gb, A);
    CYC(b_+103, b_+104); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+104, b_+105); SET_HL(HL + 1);
    CYC(b_+105, b_+106); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+106, b_+108);
    else CYC(b_+106, b_+108);
  } while (!(F & FZ));
  CYC(b_+108, b_+109); alu_xor(gb, A);
  CYC(b_+109, b_+111); hram_wr(gb, IO_SVBK & 0xff, A);
  CYC(b_+111, b_+112); ret_effect(gb);
}

void cutscene15__state1__substate1_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+112, b_+115); A = mem_rd(gb, wGenericCutscene_cbbd);
  CYC(b_+115, b_+116); B = A;
  CYC(b_+116, b_+119); A = mem_rd(gb, wGenericCutscene_cbbc);
  CYC(b_+119, b_+120); alu_cp(gb, B);
  CYC(b_+120, b_+123); mem_wr(gb, wGenericCutscene_cbbd, A);
  if (!(F & FZ)) CALL_C_CC(b_+123, cutscene15__state1__initWaveScrollValuesInverted_hook, b_+88, b_+126);
  else CYC(b_+123, b_+126);
  CYC(b_+126, b_+129); A = mem_rd(gb, wGenericCutscene_cbbb);
  CYC(b_+129, b_+132); TAIL(loadBigBufferScrollValues);
}

void cutscene15__state2_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+257, b_+260); A = mem_rd(gb, wGenericCutscene_cbb3);
  CYC(b_+260, b_+261); bank1_jump_table_from_rst(gb, b_+261);
  if (HL == b_+112) {
    TAIL(cutscene15__state1__substate1);
  }
  if (HL == b_+265) {
    TAIL(cutscene15__state2__substate1);
  }
  HANDOFF(HL);
}

void cutscene15__state2__substate1_hook(GB *gb) {
  BASE(cutscene15);
  CYC(b_+265, b_+267); A = 0x03;
  CYC(b_+267, b_+269); hram_wr(gb, hNextLcdInterruptBehaviour & 0xff, A);
  CYC(b_+269, b_+271); A = 0xc7;
  CYC(b_+271, b_+274); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+274, b_+275); alu_xor(gb, A);
  CYC(b_+275, b_+278); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+278, b_+281); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+281, b_+284); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+284, b_+286); A = 0x01;
  CYC(b_+286, b_+289); mem_wr(gb, wScrollMode, A);
  CYC(b_+289, b_+291); A = 0xf1;
  CYC(b_+291, b_+294); TAIL(playSound_b00);
}

static void load_dungeon_layout_b01(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(loadDungeonLayout_b01);
  if (entry == b_+42) goto next_floor;
  if (entry == b_+44) goto next_byte;
  if (entry == b_+79) goto end;

  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+4, clearDungeonLayout_hook, SYM(clearDungeonLayout), b_+7);
  CYC(b_+7, b_+10); A = W8(wDungeonIndex);
  CYC(b_+10, b_+13); SET_HL(SYM(dungeonDataTable));
  CYC(b_+13, b_+14); bank1_add_double_index_to_hl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+19); B = 0x08;
  CYC(b_+19, b_+22); SET_DE(wDungeonMapData);
  do {
    CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+23, b_+24); mem_wr(gb, DE, A);
    CYC(b_+24, b_+25); SET_DE(DE + 1);
    CYC(b_+25, b_+26); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+26, b_+28); else CYC(b_+26, b_+28);
  } while (!(F & FZ));
  CALL_C(b_+28, findActiveRoomInDungeonLayout_hook, SYM(findActiveRoomInDungeonLayout), b_+31);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CALL_C(b_+32, getFirstDungeonLayoutAddress_hook, SYM(getFirstDungeonLayoutAddress), b_+35);
  CYC(b_+35, b_+38); SET_DE(w2DungeonLayout);
  CYC(b_+38, b_+41); A = W8(wDungeonNumFloors);
  CYC(b_+41, b_+42); C = A;

next_floor:
  CYC(b_+42, b_+44); B = 0x40;
next_byte:
  do {
    CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+45, b_+46); mem_wr(gb, DE, A);
    CYC(b_+46, b_+47); SET_DE(DE + 1);
    CYC(b_+47, b_+48); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+48, b_+50); else CYC(b_+48, b_+50);
  } while (!(F & FZ));
  CYC(b_+50, b_+51); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto next_floor; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+56); A = W8(wTilesetFlags);
  CYC(b_+56, b_+58); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+58, b_+60); goto end; }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+63); A = W8(wDungeonFloor);
  CYC(b_+63, b_+66); SET_HL(0x00f8);
  CYC(b_+66, b_+67); alu_add(gb, L);
  CYC(b_+67, b_+68); L = A;
  CYC(b_+68, b_+69); B = mem_rd(gb, HL);
  CYC(b_+69, b_+72); A = W8(wDungeonIndex);
  CYC(b_+72, b_+75); SET_HL(wDungeonVisitedFloors);
  CYC(b_+75, b_+76); bank1_add_a_to_hl_from_rst(gb, b_+76);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+78); alu_or(gb, B);
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
end:
  CYC(b_+79, b_+80); alu_xor(gb, A);
  CYC(b_+80, b_+82); mem_wr(gb, IO_SVBK, A);
  CYC(b_+82, b_+85); setVisitedRoomFlag_hook(gb);
}

void loadDungeonLayout_b01_hook(GB *gb) { uint16_t sp0_ = gb->sp; load_dungeon_layout_b01(gb, SYM(loadDungeonLayout_b01), sp0_); }
void loadDungeonLayout_b01__nextFloor_hook(GB *gb) { uint16_t sp0_ = gb->sp; load_dungeon_layout_b01(gb, SYM(loadDungeonLayout_b01__nextFloor), sp0_); }
void loadDungeonLayout_b01__nextByte_hook(GB *gb) { uint16_t sp0_ = gb->sp; load_dungeon_layout_b01(gb, SYM(loadDungeonLayout_b01__nextByte), sp0_); }
void loadDungeonLayout_b01__end_hook(GB *gb) { uint16_t sp0_ = gb->sp; load_dungeon_layout_b01(gb, SYM(loadDungeonLayout_b01__end), sp0_); }

void clearDungeonLayout_hook(GB *gb) {
  BASE(clearDungeonLayout);
  CYC(b_+0, b_+3); SET_HL(w2DungeonLayout);
  CYC(b_+3, b_+6); SET_BC(0x0200);
  CYC(b_+6, b_+9); TAIL(clearMemoryBc);
}

static void find_active_room_in_dungeon_layout(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(findActiveRoomInDungeonLayout);
  if (entry == SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch)) {
    CYC(SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch), (SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch) + 2)); A = 0x01;
    CYC((SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch) + 2), (SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch) + 4)); H8(hRomBank) = A;
    CYC((SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch) + 4), (SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch) + 7)); mem_wr(gb, MBC_ROM_BANK, A);
  }
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, getFirstDungeonLayoutAddress_hook, SYM(getFirstDungeonLayoutAddress), b_+4);
  CYC(b_+4, b_+7); A = W8(wActiveRoom);
  CYC(b_+7, b_+9); C = 0x00;
next_floor:
  CYC(b_+9, b_+11); B = 0x40;
next_room:
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+12, b_+14); goto found; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); SET_HL(HL + 1);
  CYC(b_+15, b_+16); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto next_room; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); C = alu_inc8(gb, C);
  CYC(b_+19, b_+21); goto next_floor;
found:
  CYC(b_+21, b_+22); A = C;
  CYC(b_+22, b_+25); W8(wDungeonFloor) = A;
  CYC(b_+25, b_+27); A = 0x40;
  CYC(b_+27, b_+28); alu_sub(gb, B);
  CYC(b_+28, b_+31); W8(wDungeonMapPosition) = A;
  CYC(b_+31, b_+32); ret_effect(gb);
}

void findActiveRoomInDungeonLayoutWithPointlessBankSwitch_hook(GB *gb) { uint16_t sp0_ = gb->sp; find_active_room_in_dungeon_layout(gb, SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch), sp0_); }
void findActiveRoomInDungeonLayout_hook(GB *gb) { uint16_t sp0_ = gb->sp; find_active_room_in_dungeon_layout(gb, SYM(findActiveRoomInDungeonLayout), sp0_); }

void getFirstDungeonLayoutAddress_hook(GB *gb) {
  BASE(getFirstDungeonLayoutAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); A = W8(wDungeonFirstLayout);
  CYC(b_+4, b_+5); alu_add(gb, C);
  CALL_C(b_+5, multiplyABy16_hook, SYM(multiplyABy16), b_+8);
  CYC(b_+8, b_+11); SET_HL(SYM(dungeonLayoutDataStart));
  CYC(b_+11, b_+12); alu_add_hl(gb, BC);
  CYC(b_+12, b_+13); alu_add_hl(gb, BC);
  CYC(b_+13, b_+14); alu_add_hl(gb, BC);
  CYC(b_+14, b_+15); alu_add_hl(gb, BC);
  CYC(b_+15, b_+16); ret_effect(gb);
}

static void check_update_dungeon_minimap(GB *gb, uint16_t entry) {
  BASE(checkUpdateDungeonMinimap);
  if (entry == b_+0) {
    CYC(b_+0, b_+3); A = W8(wTilesetFlags);
    CYC(b_+3, b_+5); alu_bit(gb, 4, A);
    if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
    CYC(b_+5, b_+6);
    CYC(b_+6, b_+8); alu_bit(gb, 5, A);
    if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
    CYC(b_+8, b_+9);
    CYC(b_+9, b_+11); alu_bit(gb, 0, A);
    if (!(F & FZ)) { CYCT(b_+11, b_+13); goto set_minimap_room; }
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+15); alu_bit(gb, 3, A);
    if (F & FZ) { CYCT(b_+15, b_+16); ret_effect(gb); return; }
    CYC(b_+15, b_+16);
  }
set_minimap_room:
  CYC(b_+16, b_+19); SET_HL(wMinimapDungeonFloor);
  CYC(b_+19, b_+22); A = W8(wDungeonFloor);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+23, b_+26); A = W8(wDungeonMapPosition);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+27, b_+30); A = W8(wActiveRoom);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+31, b_+34); A = W8(wActiveGroup);
  CYC(b_+34, b_+35); C = mem_rd(gb, HL);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+37); ret_effect(gb);
}

void checkUpdateDungeonMinimap_hook(GB *gb) { check_update_dungeon_minimap(gb, SYM(checkUpdateDungeonMinimap)); }
void checkUpdateDungeonMinimap__setMinimapRoom_hook(GB *gb) { check_update_dungeon_minimap(gb, SYM(checkUpdateDungeonMinimap__setMinimapRoom)); }

void checkBrightenRoom_hook(GB *gb) {
  BASE(checkBrightenRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, getThisRoomDungeonProperties_hook, SYM(getThisRoomDungeonProperties), b_+9);
  CYC(b_+9, b_+12); A = W8(wDungeonRoomProperties);
  CYC(b_+12, b_+14); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+18); A = W8(wPaletteThread_parameter);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+19, b_+20); ret_effect(gb); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); TAIL(brightenRoom);
}

static void check_play_room_music(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(checkPlayRoomMusic);
  if (entry == b_+0) {
    CYC(b_+0, b_+2); A = 0x0a;
    CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
    if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
    CYC(b_+5, b_+6);
    CYC(b_+6, b_+9); A = W8(wActiveMusic);
    CYC(b_+9, b_+10); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
    CYC(b_+10, b_+11);
    CYC(b_+11, b_+14); A = W8(wActiveMusic2);
    CYC(b_+14, b_+16); alu_cp(gb, 0x24);
    if (!(F & FZ)) { CYCT(b_+16, b_+18); goto load_music; }
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+21); A = W8(wActiveGroup);
    CYC(b_+21, b_+22); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+22, b_+24); goto load_music; }
    CYC(b_+22, b_+24);
    CYC(b_+24, b_+27); A = mem_rd(gb, wGroup0RoomFlags + 3);
    CYC(b_+27, b_+29); alu_bit(gb, 0, A);
    if (!(F & FZ)) { CYCT(b_+29, b_+31); goto load_music; }
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+33); A = 0x1f;
    CYC(b_+33, b_+36); W8(wActiveMusic2) = A;
load_music:
    CYC(b_+36, b_+39); A = W8(wActiveMusic2);
  }
  CYC(b_+39, b_+42); SET_HL(wActiveMusic);
  CYC(b_+42, b_+43); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+43, b_+44); ret_effect(gb); return; }
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+48); playSound_b00_hook(gb);
}

void checkPlayRoomMusic_hook(GB *gb) { uint16_t sp0_ = gb->sp; check_play_room_music(gb, SYM(checkPlayRoomMusic), sp0_); }
void checkPlayRoomMusic__setMusic_hook(GB *gb) { uint16_t sp0_ = gb->sp; check_play_room_music(gb, SYM(checkPlayRoomMusic__setMusic), sp0_); }

void func_593a_hook(GB *gb) {
  BASE(func_593a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+3);
  CALL_C(b_+3, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+11); TAIL(loadGfxRegisterStateIndex);
}

static void func_5cfe(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(func_5cfe);
  if (entry == b_+42) goto clear_companion;
  if (entry == b_+46) goto end;
  if (entry == b_+0) {
    CYC(b_+0, b_+3); A = W8(wcc4c);
    CYC(b_+3, b_+4); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+4, b_+6); goto body; }
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+9); A = mem_rd(gb, w1Companion_enabled);
    CYC(b_+9, b_+10); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+10, b_+12); goto clear_companion; }
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+15); A = mem_rd(gb, w1Companion_id);
    CYC(b_+15, b_+17); alu_cp(gb, 0x0a);
    if (F & FZ) { CYCT(b_+17, b_+19); goto clear_companion; }
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+21); alu_cp(gb, 0x0e);
    if (F & FZ) { CYCT(b_+21, b_+23); goto clear_companion; }
    CYC(b_+21, b_+23);
  }
body:
  CALL_C(b_+23, func_4493_hook, SYM(func_4493), b_+26);
  CYC(b_+26, b_+29); A = W8(wLinkGrabState2);
  CYC(b_+29, b_+31); alu_and(gb, 0xf0);
  CYC(b_+31, b_+33); alu_cp(gb, 0x40);
  if (F & FZ) { CYCT(b_+33, b_+35); goto end; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); A = W8(wLinkObjectIndex);
  CYC(b_+38, b_+40); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto end; }
  CYC(b_+40, b_+42);
clear_companion:
  CYC(b_+42, b_+43); alu_xor(gb, A);
  CYC(b_+43, b_+46); W8(wRememberedCompanionId) = A;
end:
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CYC(b_+47, b_+50); W8(wcc4c) = A;
  CYC(b_+50, b_+51); ret_effect(gb);
}

void func_5cfe_hook(GB *gb) { uint16_t sp0_ = gb->sp; func_5cfe(gb, SYM(func_5cfe), sp0_); }
void func_5cfe__clearCompanion_hook(GB *gb) { uint16_t sp0_ = gb->sp; func_5cfe(gb, SYM(func_5cfe__clearCompanion), sp0_); }
void func_5cfe__end_hook(GB *gb) { uint16_t sp0_ = gb->sp; func_5cfe(gb, SYM(func_5cfe__end), sp0_); }

void setEnteredWarpPosition_hook(GB *gb) {
  BASE(setEnteredWarpPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(w1Link_yh);
  CALL_C(b_+3, getShortPositionFromDE_hook, SYM(getShortPositionFromDE), b_+6);
  CYC(b_+6, b_+9); W8(wEnteredWarpPosition) = A;
  CYC(b_+9, b_+10); ret_effect(gb);
}

void paletteFadeHandler_hook(GB *gb) {
  BASE(paletteFadeHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(3)); A = W8(wPaletteThread_mode);
  CYC(b_+O(3), b_+OE(4)); bank1_jump_table_from_rst(gb, b_+OE(4));
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(paletteFadeHandler09) && hook_is(gb, SYM(paletteFadeHandler09), paletteFadeHandler09_hook)) { paletteFadeHandler09_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler01) && hook_is(gb, SYM(paletteFadeHandler01), paletteFadeHandler01_hook)) { paletteFadeHandler01_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler00) && hook_is(gb, SYM(paletteFadeHandler00), paletteFadeHandler00_hook)) { paletteFadeHandler00_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler0a) && hook_is(gb, SYM(paletteFadeHandler0a), paletteFadeHandler0a_hook)) { paletteFadeHandler0a_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler02) && hook_is(gb, SYM(paletteFadeHandler02), paletteFadeHandler02_hook)) { paletteFadeHandler02_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler0b) && hook_is(gb, SYM(paletteFadeHandler0b), paletteFadeHandler0b_hook)) { paletteFadeHandler0b_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler03) && hook_is(gb, SYM(paletteFadeHandler03), paletteFadeHandler03_hook)) { paletteFadeHandler03_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler0c) && hook_is(gb, SYM(paletteFadeHandler0c), paletteFadeHandler0c_hook)) { paletteFadeHandler0c_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler04) && hook_is(gb, SYM(paletteFadeHandler04), paletteFadeHandler04_hook)) { paletteFadeHandler04_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler0d) && hook_is(gb, SYM(paletteFadeHandler0d), paletteFadeHandler0d_hook)) { paletteFadeHandler0d_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler05) && hook_is(gb, SYM(paletteFadeHandler05), paletteFadeHandler05_hook)) { paletteFadeHandler05_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler0e) && hook_is(gb, SYM(paletteFadeHandler0e), paletteFadeHandler0e_hook)) { paletteFadeHandler0e_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler06) && hook_is(gb, SYM(paletteFadeHandler06), paletteFadeHandler06_hook)) { paletteFadeHandler06_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler07) && hook_is(gb, SYM(paletteFadeHandler07), paletteFadeHandler07_hook)) { paletteFadeHandler07_hook(gb); return; }
    else if (jt_ == SYM(paletteFadeHandler08) && hook_is(gb, SYM(paletteFadeHandler08), paletteFadeHandler08_hook)) { paletteFadeHandler08_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void updateFadingPalettes_hook(GB *gb) {
  BASE(updateFadingPalettes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, paletteThread_calculateFadingPalettes_hook, SYM(paletteThread_calculateFadingPalettes), b_+3);
  CYC(b_+3, b_+6); SET_HL(wDirtyFadeBgPalettes);
  CYC(b_+6, b_+8); A = H8(hDirtyBgPalettes);
  CYC(b_+8, b_+9); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+11); H8(hDirtyBgPalettes) = A;
  CYC(b_+11, b_+12); SET_HL(HL + 1);
  CYC(b_+12, b_+14); A = H8(hDirtySprPalettes);
  CYC(b_+14, b_+15); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+17); H8(hDirtySprPalettes) = A;
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+21); H8(hBgPaletteSources) = A;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); H8(hSprPaletteSources) = A;
  TAIL(paletteFadeHandler00);
}

void checkLockBG7Color3ToBlack_hook(GB *gb) {
  BASE(checkLockBG7Color3ToBlack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wLockBG7Color3ToBlack);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+8) { checkLockBG7Color3ToBlack__thing1_hook(gb); return; }
    else if (jt_ == b_+15) { checkLockBG7Color3ToBlack__thing0_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void checkLockBG7Color3ToBlack__thing1_hook(GB *gb) {
  BASE(checkLockBG7Color3ToBlack);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12); mem_wr(gb, w2FadingBgPalettes + 0x3e, A);
  CYC(b_+12, b_+15); mem_wr(gb, w2FadingBgPalettes + 0x3f, A);
  TAIL(checkLockBG7Color3ToBlack__thing0);
}

void checkLockBG7Color3ToBlack__thing0_hook(GB *gb) {
  BASE(checkLockBG7Color3ToBlack);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void func_4000_b01_hook(GB *gb) {
  BASE(func_4000_b01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+4, func_400b_hook, SYM(func_400b), b_+7);
  else CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+10); mem_wr(gb, IO_SVBK, A);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void runGameLogic_hook(GB *gb) {
  BASE(runGameLogic);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wThreadStateBuffer + 0x0e);
  CYC(b_+3, b_+4); bank1_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(initializeGame) && hook_is(gb, SYM(initializeGame), initializeGame_hook)) { initializeGame_hook(gb); return; }
    else if (jt_ == SYM(loadingRoom) && hook_is(gb, SYM(loadingRoom), loadingRoom_hook)) { loadingRoom_hook(gb); return; }
    else if (jt_ == SYM(standardGameState) && hook_is(gb, SYM(standardGameState), standardGameState_hook)) { standardGameState_hook(gb); return; }
    else if (jt_ == SYM(linkSummonedCutscene) && hook_is(gb, SYM(linkSummonedCutscene), linkSummonedCutscene_hook)) { linkSummonedCutscene_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void initialize_game(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(initializeGame);
  if (entry == b_+25) goto not_gba_mode;
  if (entry == b_+54) goto fix_respawn;
  if (entry == b_+138) goto reset_health;
  if (entry == b_+207) goto summon_link;

  CYC(b_+0, b_+3); SET_HL(wOamEnd);
  CYC(b_+3, b_+6); SET_BC(0x0460);
  CALL_C(b_+6, clearMemoryBc_hook, SYM(clearMemoryBc), b_+9);
  CALL_C(b_+9, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+12);
  CALL_C(b_+12, initializeSeedTreeRefillData_hook, SYM(initializeSeedTreeRefillData), b_+15);
  CYC(b_+15, b_+17); A = 0x0f;
  CALL_C(b_+17, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+20);
  CYC(b_+20, b_+22); A = H8(hGameboyType);
  CYC(b_+22, b_+23); alu_rlca(gb);
  if (F & FC) { CYCT(b_+23, b_+25); goto load_respawn; }
  CYC(b_+23, b_+25);

not_gba_mode:
  CYC(b_+25, b_+28); SET_HL(wDeathRespawnBuffer);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); L = mem_rd(gb, HL);
  CYC(b_+30, b_+31); H = A;
  CYC(b_+31, b_+34); SET_BC(0x03fe);
  CALL_C(b_+34, compareHlToBc_hook, SYM(compareHlToBc), b_+37);
  if (F & FZ) { CYCT(b_+37, b_+39); goto fix_respawn; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); SET_BC(0x0158);
  CALL_C(b_+42, compareHlToBc_hook, SYM(compareHlToBc), b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto load_respawn; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+50); A = W8(wDeathRespawnBuffer_x);
  CYC(b_+50, b_+52); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+52, b_+54); goto load_respawn; }
  CYC(b_+52, b_+54);

fix_respawn:
  CYC(b_+54, b_+56); C = 0x03;
  CALL_C(b_+56, loadDeathRespawnBufferPreset_hook, SYM(loadDeathRespawnBufferPreset), b_+59);

load_respawn:
  CYC(b_+59, b_+62); A = W8(wFileIsLinkedGame);
  CYC(b_+62, b_+65); W8(wIsLinkedGame) = A;
  CYC(b_+65, b_+68); SET_HL(wDeathRespawnBuffer);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+69, b_+72); W8(wActiveGroup) = A;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+73, b_+76); W8(wActiveRoom) = A;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+77, b_+80); W8(wRoomStateModifier) = A;
  CYC(b_+80, b_+82); A = 0x03;
  CYC(b_+82, b_+85); mem_wr(gb, w1Link_enabled, A);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+86, b_+89); mem_wr(gb, w1Link_direction, A);
  CYC(b_+89, b_+92); W8(wLinkLocalRespawnDir) = A;
  CYC(b_+92, b_+93); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+93, b_+96); mem_wr(gb, w1Link_yh, A);
  CYC(b_+96, b_+99); W8(wLinkLocalRespawnY) = A;
  CYC(b_+99, b_+100); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+100, b_+103); mem_wr(gb, w1Link_xh, A);
  CYC(b_+103, b_+106); W8(wLinkLocalRespawnX) = A;
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+110); W8(wRememberedCompanionId) = A;
  CYC(b_+110, b_+111); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+111, b_+114); W8(wRememberedCompanionGroup) = A;
  CYC(b_+114, b_+115); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+115, b_+118); W8(wRememberedCompanionRoom) = A;
  CYC(b_+118, b_+119); L = alu_inc8(gb, L);
  CYC(b_+119, b_+120); L = alu_inc8(gb, L);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+121, b_+124); W8(wRememberedCompanionY) = A;
  CYC(b_+124, b_+125); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+125, b_+128); W8(wRememberedCompanionX) = A;
  CYC(b_+128, b_+130); L = 0xaa;
  CYC(b_+130, b_+131); A = mem_rd(gb, HL);
  CYC(b_+131, b_+132); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+132, b_+134); goto reset_health; }
  CYC(b_+132, b_+134);
  CYC(b_+134, b_+136); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+136, b_+138); goto health_ready; }
  CYC(b_+136, b_+138);

reset_health:
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+140, b_+142); A = alu_srl(gb, A);
  CYC(b_+142, b_+144); alu_and(gb, 0xfc);
  CYC(b_+144, b_+146); alu_cp(gb, 0x0c);
  if (!(F & FC)) { CYCT(b_+146, b_+148); goto health_ready; }
  CYC(b_+146, b_+148);
  CYC(b_+148, b_+150); A = 0x0c;

health_ready:
  CYC(b_+150, b_+151); mem_wr(gb, HL, A);
  CYC(b_+151, b_+154); W8(wDisplayedHearts) = A;
  CYC(b_+154, b_+156); A = 0x88;
  CYC(b_+156, b_+159); mem_wr(gb, w1Link_invincibilityCounter, A);
  CYC(b_+159, b_+161); L = 0xad;
  CYC(b_+161, b_+162); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+162, b_+165); W8(wDisplayedRupees) = A;
  CYC(b_+165, b_+166); A = mem_rd(gb, HL);
  CYC(b_+166, b_+169); mem_wr(gb, wDisplayedRupees + 1, A);
  CALL_C(b_+169, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+172);
  CYC(b_+172, b_+174); A = 0xff;
  CYC(b_+174, b_+177); W8(wActiveMusic) = A;
  CYC(b_+177, b_+180); W8(wcc05) = A;
  CYC(b_+180, b_+182); A = 0x21;
  CALL_C(b_+182, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+185);
  if (!(F & FZ)) {
    CYCT(b_+185, b_+187);
    TAIL(func_5a60);
  }
  CYC(b_+185, b_+187);
  CYC(b_+187, b_+189); A = 0x3d;
  CALL_C(b_+189, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+192);
  if (!(F & FZ)) { CYCT(b_+192, b_+194); goto summon_link; }
  CYC(b_+192, b_+194);
  CYC(b_+194, b_+196); A = 0x02;
  CYC(b_+196, b_+199); mem_wr(gb, wThreadStateBuffer + 0x0e, A);
  CYC(b_+199, b_+201); A = 0x0d;
  CYC(b_+201, b_+204); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CYC(b_+204, b_+207); TAIL(cutscene0d);

summon_link:
  CYC(b_+207, b_+209); A = 0x03;
  CYC(b_+209, b_+212); mem_wr(gb, wThreadStateBuffer + 0x0e, A);
  CYC(b_+212, b_+213); alu_xor(gb, A);
  CYC(b_+213, b_+216); mem_wr(gb, w1Link_enabled, A);
  CYC(b_+216, b_+217); ret_effect(gb);
}

void initializeGame_hook(GB *gb) { initialize_game(gb, SYM(initializeGame), gb->sp); }
void initializeGame__notGbaMode_hook(GB *gb) { initialize_game(gb, SYM(initializeGame__notGbaMode), gb->sp); }
void initializeGame__fixRespawnForGbc_hook(GB *gb) { initialize_game(gb, SYM(initializeGame__fixRespawnForGbc), gb->sp); }
void initializeGame__resetHealth_hook(GB *gb) { initialize_game(gb, SYM(initializeGame__resetHealth), gb->sp); }
void initializeGame__summonLinkCutscene_hook(GB *gb) { initialize_game(gb, SYM(initializeGame__summonLinkCutscene), gb->sp); }

static void func_5a60_after_initialize_room(GB *gb, uint16_t sp0_) {
  BASE(func_5a60);
  CALL_C(b_+O(77), checkDisplayEraOrSeasonInfo_hook, SYM(checkDisplayEraOrSeasonInfo), b_+OE(80));
  CALL_C(b_+O(80), updateGrassAnimationModifier_hook, SYM(updateGrassAnimationModifier), b_+OE(83));
  CALL_C(b_+O(83), checkPlayRoomMusic_hook, SYM(checkPlayRoomMusic), b_+OE(86));
  CALL_C(b_+O(86), checkUpdateDungeonMinimap_hook, SYM(checkUpdateDungeonMinimap), b_+OE(89));
  CYC(b_+O(89), b_+OE(92)); func_593a_hook(gb);
}

void func_5a60_hook(GB *gb) {
  BASE(func_5a60);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), clearOam_hook, SYM(clearOam), b_+OE(3));
  CALL_C(b_+O(3), initializeVramMaps_hook, SYM(initializeVramMaps), b_+OE(6));
  CALL_C(b_+O(6), clearMemoryOnScreenReload_hook, SYM(clearMemoryOnScreenReload), b_+OE(9));
  CALL_C(b_+O(9), clearScreenVariables_hook, SYM(clearScreenVariables), b_+OE(12));
  CALL_C(b_+O(12), clearEnemiesKilledList_b00_hook, SYM(clearEnemiesKilledList_b00), b_+OE(15));
  CALL_C(b_+O(15), clearAllParentItems_hook, SYM(clearAllParentItems), b_+OE(18));
  CALL_C(b_+O(18), dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+OE(21));
  CALL_C(b_+O(21), loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+OE(24));
  CALL_C(b_+O(24), loadTilesetData_hook, SYM(loadTilesetData), b_+OE(27));
  CALL_C(b_+O(27), loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+OE(30));
  if (!game_seasons) {
    CYC(b_+30, b_+33); A = W8(wLoadingRoomPack);
    CYC(b_+33, b_+36); W8(wRoomPack) = A;
  }
  CALL_C(b_+O(36), loadDungeonLayout_hook, SYM(loadDungeonLayout), b_+OE(39));
  CYC(b_+O(39), b_+OE(41)); A = 0x02;
  CYC(b_+O(41), b_+OE(44)); mem_wr(gb, wThreadStateBuffer + 0x0e, A);
  CYC(b_+O(44), b_+OE(45)); alu_xor(gb, A);
  CYC(b_+O(45), b_+OE(48)); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CYC(b_+O(48), b_+OE(51)); W8(wWarpTransition2) = A;
  CYC(b_+O(51), b_+OE(54)); W8(wSwitchState) = A;
  CYC(b_+O(54), b_+OE(57)); W8(wToggleBlocksState) = A;
  CYC(b_+O(57), b_+OE(59)); A = 0x02;
  CYC(b_+O(59), b_+OE(62)); W8(wScrollMode) = A;
  CALL_C(b_+O(62), loadTilesetAndRoomLayout_hook, SYM(loadTilesetAndRoomLayout), b_+OE(65));
  CALL_C(b_+O(65), loadRoomCollisions_hook, SYM(loadRoomCollisions), b_+OE(68));
  CALL_C(b_+O(68), generateVramTilesWithRoomChanges_hook, SYM(generateVramTilesWithRoomChanges), b_+OE(71));
  CALL_C(b_+O(71), setEnteredWarpPosition_hook, SYM(setEnteredWarpPosition), b_+OE(74));
  CALL_C(b_+O(74), initializeRoom_hook, SYM(initializeRoom), b_+OE(77));
  func_5a60_after_initialize_room(gb, sp0_);
}

void func_5a60__afterCall5aad_hook(GB *gb) {
  func_5a60_after_initialize_room(gb, gb->sp);
}

void loadingRoom_hook(GB *gb) {
  BASE(loadingRoom);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+3);
  CALL_C(b_+3, clearStaticObjects_hook, SYM(clearStaticObjects), b_+6);
  CALL_C(b_+6, stopTextThread_hook, SYM(stopTextThread), b_+9);
  CYC(b_+9, b_+11); A = 0xff;
  CYC(b_+11, b_+14); W8(wActiveMusic) = A;
  CALL_C(b_+14, applyWarpDest_hook, SYM(applyWarpDest), SYM(func_5a60));
  TAIL(func_5a60);
}

void standardGameState_hook(GB *gb) {
  BASE(standardGameState);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(3)); A = W8(wLinkDeathTrigger);
  CYC(b_+O(3), b_+OE(5)); alu_cp(gb, 0xff);
  if (!(F & FZ)) CYCT(b_+O(5), b_+OE(7));
  else {
    CYC(b_+O(5), b_+OE(7));
    CYC(b_+O(7), b_+OE(9)); A = 0xfc;
    CALL_C(b_+O(9), playSound_b00_hook, SYM(playSound_b00), b_+OE(12));
    CYC(b_+O(12), b_+OE(14)); A = 0xe7;
    CYC(b_+O(14), b_+OE(17)); W8(wLinkDeathTrigger) = A;
  }
  CYC(b_+O(17), b_+OE(20)); A = W8(wGameOverScreenTrigger);
  CYC(b_+O(20), b_+OE(21)); alu_or(gb, A);
  if (F & FZ) CYCT(b_+O(21), b_+OE(23));
  else {
    CYC(b_+O(21), b_+OE(23));
    CYC(b_+O(23), b_+OE(25)); A = 0xe0;
    CYC(b_+O(25), b_+OE(28)); SET_BC(SYM(thread_1b10));
    CALL_C(b_+O(28), threadRestart_hook, SYM(threadRestart), b_+OE(31));
    CYC(b_+O(31), b_+OE(34)); stubThreadStart_hook(gb);
    return;
  }
  CYC(b_+O(34), b_+OE(37)); A = mem_rd(gb, wThreadStateBuffer + 0x0f);
  CYC(b_+O(37), b_+OE(38)); bank1_jump_table_from_rst(gb, b_+OE(38));
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(cutscene00) && hook_is(gb, SYM(cutscene00), cutscene00_hook)) { cutscene00_hook(gb); return; }
    else if (jt_ == SYM(cutscene01) && hook_is(gb, SYM(cutscene01), cutscene01_hook)) { cutscene01_hook(gb); return; }
    else if (jt_ == SYM(cutscene02) && hook_is(gb, SYM(cutscene02), cutscene02_hook)) { cutscene02_hook(gb); return; }
    else if (jt_ == SYM(cutscene03) && hook_is(gb, SYM(cutscene03), cutscene03_hook)) { cutscene03_hook(gb); return; }
    else if (jt_ == SYM(cutscene04) && hook_is(gb, SYM(cutscene04), cutscene04_hook)) { cutscene04_hook(gb); return; }
    else if (jt_ == SYM(cutscene05) && hook_is(gb, SYM(cutscene05), cutscene05_hook)) { cutscene05_hook(gb); return; }
    else if (jt_ == SYM(cutscene06) && hook_is(gb, SYM(cutscene06), cutscene06_hook)) { cutscene06_hook(gb); return; }
    else if (jt_ == SYM(cutscene07) && hook_is(gb, SYM(cutscene07), cutscene07_hook)) { cutscene07_hook(gb); return; }
    else if (jt_ == SYM(cutscene08) && hook_is(gb, SYM(cutscene08), cutscene08_hook)) { cutscene08_hook(gb); return; }
    else if (jt_ == SYM(cutscene09) && hook_is(gb, SYM(cutscene09), cutscene09_hook)) { cutscene09_hook(gb); return; }
    else if (jt_ == SYM(cutscene0a) && hook_is(gb, SYM(cutscene0a), cutscene0a_hook)) { cutscene0a_hook(gb); return; }
    else if (jt_ == SYM(cutscene0b) && hook_is(gb, SYM(cutscene0b), cutscene0b_hook)) { cutscene0b_hook(gb); return; }
    else if (jt_ == SYM(cutscene0c) && hook_is(gb, SYM(cutscene0c), cutscene0c_hook)) { cutscene0c_hook(gb); return; }
    else if (jt_ == SYM(cutscene0d) && hook_is(gb, SYM(cutscene0d), cutscene0d_hook)) { cutscene0d_hook(gb); return; }
    else if (jt_ == SYM(cutscene0e) && hook_is(gb, SYM(cutscene0e), cutscene0e_hook)) { cutscene0e_hook(gb); return; }
    else if (jt_ == SYM(cutscene0f) && hook_is(gb, SYM(cutscene0f), cutscene0f_hook)) { cutscene0f_hook(gb); return; }
    else if (jt_ == SYM(cutscene10) && hook_is(gb, SYM(cutscene10), cutscene10_hook)) { cutscene10_hook(gb); return; }
    else if (jt_ == SYM(cutscene11) && hook_is(gb, SYM(cutscene11), cutscene11_hook)) { cutscene11_hook(gb); return; }
    else if (jt_ == SYM(cutscene12) && hook_is(gb, SYM(cutscene12), cutscene12_hook)) { cutscene12_hook(gb); return; }
    else if (jt_ == SYM(cutscene13) && hook_is(gb, SYM(cutscene13), cutscene13_hook)) { cutscene13_hook(gb); return; }
    else if (jt_ == SYM(cutscene14) && hook_is(gb, SYM(cutscene14), cutscene14_hook)) { cutscene14_hook(gb); return; }
    else if (jt_ == SYM(cutscene15) && hook_is(gb, SYM(cutscene15), cutscene15_hook)) { cutscene15_hook(gb); return; }
    else if (jt_ == SYM(cutscene16) && hook_is(gb, SYM(cutscene16), cutscene16_hook)) { cutscene16_hook(gb); return; }
    else if (jt_ == SYM(cutscene17) && hook_is(gb, SYM(cutscene17), cutscene17_hook)) { cutscene17_hook(gb); return; }
    else if (jt_ == SYM(cutscene18) && hook_is(gb, SYM(cutscene18), cutscene18_hook)) { cutscene18_hook(gb); return; }
    else if (jt_ == SYM(cutscene19) && hook_is(gb, SYM(cutscene19), cutscene19_hook)) { cutscene19_hook(gb); return; }
    else if (jt_ == SYM(cutscene1a) && hook_is(gb, SYM(cutscene1a), cutscene1a_hook)) { cutscene1a_hook(gb); return; }
    else if (jt_ == SYM(cutscene1b) && hook_is(gb, SYM(cutscene1b), cutscene1b_hook)) { cutscene1b_hook(gb); return; }
    else if (jt_ == SYM(cutscene1c) && hook_is(gb, SYM(cutscene1c), cutscene1c_hook)) { cutscene1c_hook(gb); return; }
    else if (jt_ == SYM(cutscene1d) && hook_is(gb, SYM(cutscene1d), cutscene1d_hook)) { cutscene1d_hook(gb); return; }
    else if (jt_ == SYM(cutscene1e) && hook_is(gb, SYM(cutscene1e), cutscene1e_hook)) { cutscene1e_hook(gb); return; }
    else if (jt_ == SYM(cutscene1f) && hook_is(gb, SYM(cutscene1f), cutscene1f_hook)) { cutscene1f_hook(gb); return; }
    else if (jt_ == SYM(cutscene20) && hook_is(gb, SYM(cutscene20), cutscene20_hook)) { cutscene20_hook(gb); return; }
    else if (jt_ == SYM(cutscene21) && hook_is(gb, SYM(cutscene21), cutscene21_hook)) { cutscene21_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void cutscene00_after_object_gfx(GB *gb, uint16_t sp0_) {
  BASE(cutscene00);
  CALL_C(b_+40, setVisitedRoomFlag_hook, SYM(setVisitedRoomFlag), b_+43);
  CALL_C(b_+43, checkUpdateDungeonMinimap_hook, SYM(checkUpdateDungeonMinimap), b_+46);
  CYC(b_+46, b_+48); A = 0x01;
  CYC(b_+48, b_+51); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CALL_C(b_+51, playCompassSoundIfKeyInRoom_hook, SYM(playCompassSoundIfKeyInRoom), b_+54);
  CALL_C(b_+54, updateLastToggleBlocksState_hook, SYM(updateLastToggleBlocksState), b_+57);
  CALL_C(b_+57, checkInitUnderwaterWaves_hook, SYM(checkInitUnderwaterWaves), b_+60);
  CYC(b_+60, b_+63); updateGrassAnimationModifier_hook(gb);
}

static void cutscene00_after_tree_gfx(GB *gb, uint16_t sp0_) {
  BASE(cutscene00);
  CYC(b_+9, b_+12); A = W8(wScrollMode);
  CYC(b_+12, b_+14); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, setInstrumentsDisabledCounterAndScrollMode_hook, SYM(setInstrumentsDisabledCounterAndScrollMode), b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+22, b_+25); A = W8(wcc05);
  CYC(b_+25, b_+27); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(b_+27, b_+29);
  else {
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+31); A = 0xff;
    CYC(b_+31, b_+34); W8(wcc05) = A;
  }
  CALL_C(b_+34, clearObjectsWithEnabled2_hook, SYM(clearObjectsWithEnabled2), b_+37);
  CALL_C(b_+37, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+40);
  cutscene00_after_object_gfx(gb, sp0_);
}

void cutscene00_hook(GB *gb) {
  BASE(cutscene00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, updateStatusBar_hook, SYM(updateStatusBar), b_+3);
  CALL_C(b_+3, updateAllObjects_hook, SYM(updateAllObjects), b_+6);
  CALL_C(b_+6, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+9);
  cutscene00_after_tree_gfx(gb, sp0_);
}

void cutscene00__afterCall5b2f_hook(GB *gb) { cutscene00_after_tree_gfx(gb, gb->sp); }
void cutscene00__afterCall5b4e_hook(GB *gb) { cutscene00_after_object_gfx(gb, gb->sp); }

static void cutscene01_after_initialize_room(GB *gb, uint16_t sp0_) {
  BASE(cutscene01);
  AGES_ONLY();
  (void)sp0_;
  CYC(b_+112, b_+115); checkPlayRoomMusic_hook(gb);
}

static void cutscene01_after_tree_gfx(GB *gb, uint16_t sp0_) {
  BASE(cutscene01);
  CALL_C(b_+O(3), updateLinkBeingShocked_hook, SYM(updateLinkBeingShocked), b_+OE(6));
  CALL_C(b_+O(6), updateMenus_hook, SYM(updateMenus), b_+OE(9));
  if (!(F & FZ)) { CYCT(b_+O(9), b_+OE(10)); ret_effect(gb); return; }
  CYC(b_+O(9), b_+OE(10));
  if (!game_seasons) CALL_C(b_+10, updatePirateShip_hook, SYM(updatePirateShip), b_+13);
  CALL_C(b_+O(13), updateAllObjects_hook, SYM(updateAllObjects), b_+OE(16));
  if (!game_seasons) {      // underwater waves and the bank 2 tile animation: Ages only
    CALL_C(b_+16, checkUpdateUnderwaterWaves_hook, SYM(checkUpdateUnderwaterWaves), b_+19);
    CYC(b_+19, b_+22); SET_HL(SYM(func_02_7a3a));
    CYC(b_+22, b_+24); E = 0x02;
    CALL_C(b_+24, interBankCall_hook, 0x008a, b_+27);
  }
  CALL_C(b_+O(27), updateStatusBar_hook, SYM(updateStatusBar), b_+OE(30));
  if (!game_seasons) CALL_C(b_+30, checkUpdateToggleBlocks_hook, SYM(checkUpdateToggleBlocks), b_+33);
  CYC(b_+O(33), b_+OE(36)); A = W8(wCutsceneTrigger);
  CYC(b_+O(36), b_+OE(37)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(37), b_+OE(40));
    TAIL(setCutsceneIndexIfCutsceneTriggerSet);
  }
  CYC(b_+O(37), b_+OE(40));
  CALL_C(b_+O(40), func_60e9_hook, SYM(func_60e9), b_+OE(43));
  CYC(b_+O(43), b_+OE(46)); A = W8(wWarpTransition2);
  CYC(b_+O(46), b_+OE(47)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(47), b_+OE(50));
    TAIL(applyWarpTransition2);
  }
  CYC(b_+O(47), b_+OE(50));
  if (game_seasons) {       // a pending warp (set while the season changed) fades out first
    CYC(b_+S(33), b_+S(36)); A = W8(wWarpDestVariablesEnd);
    CYC(b_+S(36), b_+S(37)); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+S(37), b_+S(40)); TAIL(triggerFadeoutTransition); }
    CYC(b_+S(37), b_+S(40));
  }
  CALL_C(b_+O(50), getNextActiveRoom_hook, SYM(getNextActiveRoom), b_+OE(53));
  if (!(F & FC)) {
    CYCT(b_+O(53), b_+OE(56));
    TAIL(checkEnemyAndPartCollisionsIfTextInactive);
  }
  CYC(b_+O(53), b_+OE(56));
  if (!game_seasons) CALL_C(b_+56, checkDisableUnderwaterWaves_hook, SYM(checkDisableUnderwaterWaves), b_+59);
  CALL_C(b_+O(59), updateSeedTreeRefillData_hook, SYM(updateSeedTreeRefillData), b_+OE(62));
  CYC(b_+O(62), b_+OE(64)); A = 0x05;
  CALL_C(b_+O(64), addToGashaMaturity_hook, SYM(addToGashaMaturity), b_+OE(67));
  CALL_C(b_+O(67), func_49c9_hook, SYM(func_49c9), b_+OE(70));
  CALL_C(b_+O(70), setObjectsEnabledTo2_hook, SYM(setObjectsEnabledTo2), b_+OE(73));
  CALL_C(b_+O(73), loadScreenMusic_hook, SYM(loadScreenMusic), b_+OE(76));
  CALL_C(b_+O(76), loadTilesetData_hook, SYM(loadTilesetData), b_+OE(79));
  CALL_C(b_+O(79), checkRoomPack_hook, SYM(checkRoomPack), b_+OE(82));
  if (!(F & FZ)) {
    CYCT(b_+O(82), b_+OE(85));
    TAIL(triggerFadeoutTransition);
  }
  CYC(b_+O(82), b_+OE(85));
  if (game_seasons) CALL_C(b_+S(72), checkPlayRoomMusic_hook, SYM(checkPlayRoomMusic), b_+S(75));
  CYC(b_+O(85), b_+OE(88)); A = W8(wActiveRoom);
  CYC(b_+O(88), b_+OE(91)); W8(wLoadingRoom) = A;
  CYC(b_+O(91), b_+OE(93)); A = 0x08;
  CYC(b_+O(93), b_+OE(96)); W8(wScrollMode) = A;
  CYC(b_+O(96), b_+OE(97)); alu_xor(gb, A);
  CYC(b_+O(97), b_+OE(100)); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CALL_C(b_+O(100), loadTilesetAndRoomLayout_hook, SYM(loadTilesetAndRoomLayout), b_+OE(103));
  CALL_C(b_+O(103), loadRoomCollisions_hook, SYM(loadRoomCollisions), b_+OE(106));
  CALL_C(b_+O(106), generateVramTilesWithRoomChanges_hook, SYM(generateVramTilesWithRoomChanges), b_+OE(109));
  if (game_seasons) { CYC(b_+S(99), b_+S(102)); TAIL(initializeRoom); }
  if (!game_seasons) {
    CALL_C(b_+109, initializeRoom_hook, SYM(initializeRoom), b_+112);
    cutscene01_after_initialize_room(gb, sp0_);
  }
}

void cutscene01_hook(GB *gb) {
  BASE(cutscene01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+OE(3));
  cutscene01_after_tree_gfx(gb, sp0_);
}

void cutscene01__afterCall5b68_hook(GB *gb) { cutscene01_after_tree_gfx(gb, gb->sp); }
void cutscene01__afterCall5bd5_hook(GB *gb) { cutscene01_after_initialize_room(gb, gb->sp); }

static void func_5c18_after_initialize_room(GB *gb, uint16_t sp0_) {
  BASE(func_5c18);
  CALL_C(b_+O(58), checkDisplayEraOrSeasonInfo_hook, SYM(checkDisplayEraOrSeasonInfo), b_+OE(61));
  CALL_C(b_+O(61), checkDarkenRoomAndClearPaletteFadeState_hook, SYM(checkDarkenRoomAndClearPaletteFadeState), b_+OE(64));
  CALL_C(b_+O(64), fadeinFromWhiteToRoom_hook, SYM(fadeinFromWhiteToRoom), b_+OE(67));
  CALL_C(b_+O(67), checkPlayRoomMusic_hook, SYM(checkPlayRoomMusic), b_+OE(70));
  CYC(b_+O(70), b_+OE(71)); alu_xor(gb, A);
  CYC(b_+O(71), b_+OE(74)); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  if (!game_seasons) { CYC(b_+74, b_+77); W8(wDontUpdateStatusBar) = A; }
  CALL_C(b_+O(77), func_593a_hook, SYM(func_593a), b_+OE(80));
  CYC(b_+O(80), b_+OE(83)); resetCamera_hook(gb);
}

void func_5c18_hook(GB *gb) {
  BASE(func_5c18);
  uint16_t sp0_ = gb->sp;
  if (!game_seasons) {
    CALL_C(b_+0, checkUpdateDungeonMinimap_hook, SYM(checkUpdateDungeonMinimap), b_+3);
    CYC(b_+3, b_+6); SET_HL(w1Companion_id);
    CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+7, b_+9); alu_cp(gb, 0x13);
    if (!(F & FZ)) CYCT(b_+9, b_+11);
    else {
      CYC(b_+9, b_+11);
      CYC(b_+11, b_+13); alu_bit(gb, 1, mem_rd(gb, HL));
      if (!(F & FZ)) CYCT(b_+13, b_+15);
      else {
        CYC(b_+13, b_+15);
        CYC(b_+15, b_+17); B = 0x40;
        CALL_C(b_+17, clearMemory_hook, SYM(clearMemory), b_+20);
        CYC(b_+20, b_+22); A = 0xd0;
        CYC(b_+22, b_+25); W8(wLinkObjectIndex) = A;
      }
    }
  }
  CYC(b_+O(25), b_+OE(28)); A = W8(wLinkGrabState2);
  CYC(b_+O(28), b_+OE(30)); alu_and(gb, 0xf0);
  CYC(b_+O(30), b_+OE(32)); alu_cp(gb, 0x40);
  if (F & FZ) CYCT(b_+O(32), b_+OE(34));
  else {
    CYC(b_+O(32), b_+OE(34));
    CALL_C(b_+O(34), dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+OE(37));
    CALL_C(b_+O(37), clearAllParentItems_hook, SYM(clearAllParentItems), b_+OE(40));
  }
  if (!game_seasons) {
    CYC(b_+40, b_+43); A = W8(wLoadingRoomPack);
    CYC(b_+43, b_+46); W8(wRoomPack) = A;
  }
  CALL_C(b_+O(46), setInstrumentsDisabledCounterAndScrollMode_hook, SYM(setInstrumentsDisabledCounterAndScrollMode), b_+OE(49));
  CALL_C(b_+O(49), setEnteredWarpPosition_hook, SYM(setEnteredWarpPosition), b_+OE(52));
  CALL_C(b_+O(52), calculateRoomEdge_hook, SYM(calculateRoomEdge), b_+OE(55));
  CALL_C(b_+O(55), initializeRoom_hook, SYM(initializeRoom), b_+OE(58));
  func_5c18_after_initialize_room(gb, sp0_);
}

void func_5c18__afterCall5c52_hook(GB *gb) { func_5c18_after_initialize_room(gb, gb->sp); }

static void cutscene03_after_object_gfx(GB *gb, uint16_t sp0_) {
  BASE(cutscene03);
  CYC(b_+43, b_+45); A = 0x0a;
  CYC(b_+45, b_+48); W8(wLinkForceState) = A;
  CYC(b_+48, b_+51); A = mem_rd(gb, wWarpTransition);
  CYC(b_+51, b_+53); alu_or(gb, 0x80);
  CYC(b_+53, b_+56); mem_wr(gb, wWarpTransition, A);
  CYC(b_+56, b_+59); A = W8(wDungeonIndex);
  CYC(b_+59, b_+61); alu_cp(gb, 0xff);
  if (F & FZ) CALL_C_CC(b_+61, clearEnemiesKilledList_b00_hook, SYM(clearEnemiesKilledList_b00), SYM(func_5c18));
  else CYC(b_+61, b_+64);
  func_5c18_hook(gb);
}

void cutscene03_hook(GB *gb) {
  BASE(cutscene03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, disableLcd_hook, SYM(disableLcd), b_+8);
  CALL_C(b_+8, clearOam_hook, SYM(clearOam), b_+11);
  CALL_C(b_+11, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+14);
  CALL_C(b_+14, clearMemoryOnScreenReload_hook, SYM(clearMemoryOnScreenReload), b_+17);
  CALL_C(b_+17, stopTextThread_hook, SYM(stopTextThread), b_+20);
  CYC(b_+20, b_+22); A = 0x0f;
  CALL_C(b_+22, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+25);
  CALL_C(b_+25, applyWarpDest_hook, SYM(applyWarpDest), b_+28);
  CALL_C(b_+28, loadTilesetData_hook, SYM(loadTilesetData), b_+31);
  CALL_C(b_+31, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+34);
  CALL_C(b_+34, loadDungeonLayout_hook, SYM(loadDungeonLayout), b_+37);
  CALL_C(b_+37, func_131f_hook, SYM(func_131f), b_+40);
  CALL_C(b_+40, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+43);
  cutscene03_after_object_gfx(gb, sp0_);
}

void cutscene03__afterCall5c03_hook(GB *gb) { cutscene03_after_object_gfx(gb, gb->sp); }

static void func_5c6b_after_initialize_room(GB *gb, uint16_t sp0_) {
  BASE(func_5c6b);
  CALL_C(b_+9, checkDisplayEraOrSeasonInfo_hook, SYM(checkDisplayEraOrSeasonInfo), b_+12);
  CALL_C(b_+12, checkDarkenRoomAndClearPaletteFadeState_hook, SYM(checkDarkenRoomAndClearPaletteFadeState), b_+15);
  CYC(b_+15, b_+17); A = 0x02;
  CALL_C(b_+17, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+20);
  CYC(b_+20, b_+23); resetCamera_hook(gb);
}

void func_5c6b_hook(GB *gb) {
  BASE(func_5c6b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, setEnteredWarpPosition_hook, SYM(setEnteredWarpPosition), b_+3);
  CALL_C(b_+3, calculateRoomEdge_hook, SYM(calculateRoomEdge), b_+6);
  CALL_C(b_+6, initializeRoom_hook, SYM(initializeRoom), b_+9);
  func_5c6b_after_initialize_room(gb, sp0_);
}

void func_5c6b__afterCall5c74_hook(GB *gb) { func_5c6b_after_initialize_room(gb, gb->sp); }

static void cutscene04_05_common(GB *gb, uint16_t sp0_) {
  BASE(cutscene05);
  CALL_C(b_+O(14), setInteractionsEnabledTo2_hook, SYM(setInteractionsEnabledTo2), b_+OE(17));
  CALL_C(b_+O(17), clearObjectsWithEnabled2_hook, SYM(clearObjectsWithEnabled2), b_+OE(20));
  CALL_C(b_+O(20), clearItems_hook, SYM(clearItems), b_+OE(23));
  CALL_C(b_+O(23), clearEnemies_hook, SYM(clearEnemies), b_+OE(26));
  CALL_C(b_+O(26), clearParts_hook, SYM(clearParts), b_+OE(29));
  CALL_C(b_+O(29), clearReservedInteraction0_hook, SYM(clearReservedInteraction0), b_+OE(32));
  if (!game_seasons) {
    CYC(b_+32, b_+35); A = W8(wScreenTransitionDirection);
    CYC(b_+35, b_+37); H8(hFF92) = A;
  }
  CALL_C(b_+O(37), clearScreenVariables_hook, SYM(clearScreenVariables), b_+OE(40));
  if (!game_seasons) {
    CYC(b_+40, b_+42); A = H8(hFF92);
    CYC(b_+42, b_+45); W8(wScreenTransitionDirection) = A;
  }
  CALL_C(b_+O(45), clearMemoryOnScreenReload_hook, SYM(clearMemoryOnScreenReload), b_+OE(48));
  CALL_C(b_+O(48), loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+OE(51));
  CALL_C(b_+O(51), loadTilesetData_hook, SYM(loadTilesetData), b_+OE(54));
  CALL_C(b_+O(54), loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+OE(57));
  CALL_C(b_+O(57), func_131f_hook, SYM(func_131f), b_+OE(60));
  CYC(b_+O(60), b_+OE(63)); SET_DE(w1Link_yh);
  CALL_C(b_+O(63), getShortPositionFromDE_hook, SYM(getShortPositionFromDE), b_+OE(66));
  CYC(b_+O(66), b_+OE(69)); W8(wWarpDestPos) = A;
  CYC(b_+O(69), b_+OE(72)); func_5c18_hook(gb);
}

void cutscene04_hook(GB *gb) {
  BASE(cutscene04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, disableLcd_hook, SYM(disableLcd), b_+8);
  CYC(b_+8, b_+11); A = W8(wWarpDestGroup);
  CYC(b_+11, b_+13); alu_and(gb, 0x07);
  CYC(b_+13, b_+16); W8(wActiveGroup) = A;
  CYC(b_+16, b_+19); A = W8(wWarpDestRoom);
  CYC(b_+19, b_+22); W8(wActiveRoom) = A;
  CYC(b_+22, b_+25); A = W8(wLinkObjectIndex);
  CYC(b_+25, b_+26); H = A;
  CYC(b_+26, b_+28); L = 0x0b;
  CYC(b_+28, b_+31); A = W8(wWarpDestPos);
  CALL_C(b_+31, setShortPosition_hook, SYM(setShortPosition), b_+34);
  CALL_C(b_+34, disableLcd_hook, SYM(disableLcd), b_+37);
  CALL_C(b_+37, clearOam_hook, SYM(clearOam), b_+40);
  CYC(b_+40, b_+42);
  cutscene04_05_common(gb, sp0_);
}

void cutscene05_hook(GB *gb) {
  BASE(cutscene05);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(3)); A = W8(wPaletteThread_mode);
  CYC(b_+O(3), b_+OE(4)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(4), b_+OE(5)); ret_effect(gb); return; }
  CYC(b_+O(4), b_+OE(5));
  CALL_C(b_+O(5), disableLcd_hook, SYM(disableLcd), b_+OE(8));
  CALL_C(b_+O(8), clearOam_hook, SYM(clearOam), b_+OE(11));
  CALL_C(b_+O(11), func_5cfe_hook, SYM(func_5cfe), b_+OE(14));
  cutscene04_05_common(gb, sp0_);
}

void cutscene15_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, cutscene15__update_hook, b_+9, b_+3);
  CALL_C(b_+3, updateStatusBar_hook, SYM(updateStatusBar), b_+6);
  CYC(b_+6, b_+9); TAIL(updateSpecialObjectsAndInteractions);
}

static void cutscene15_substate2_after_room(GB *gb, uint16_t sp0_) {
  BASE(cutscene15);
  CYC(b_+180, b_+183); A = W8(wActiveGroup);
  CYC(b_+183, b_+185); alu_cp(gb, 0x03);
  if (!(F & FZ)) CYCT(b_+185, b_+187);
  else {
    CYC(b_+185, b_+187);
    CYC(b_+187, b_+188); alu_xor(gb, A);
    CYC(b_+188, b_+191); W8(wMinimapGroup) = A;
    CYC(b_+191, b_+194); A = W8(wActiveRoom);
    CYC(b_+194, b_+196); alu_cp(gb, 0xab);
    CYC(b_+196, b_+198); A = 0xf7;
    if (F & FZ) CYCT(b_+198, b_+200);
    else {
      CYC(b_+198, b_+200);
      CYC(b_+200, b_+202); A = 0x04;
    }
    CYC(b_+202, b_+205); W8(wMinimapRoom) = A;
  }
  CALL_C(b_+205, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+208);
  CYC(b_+208, b_+211); SET_HL(SYM(updateInteractions));
  CYC(b_+211, b_+213); E = 0x00;
  CALL_C(b_+213, interBankCall_hook, 0x008a, b_+216);
  CYC(b_+216, b_+218); A = 0x02;
  CALL_C(b_+218, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+221);
  CYC(b_+221, b_+223); A = 0x10;
  CYC(b_+223, b_+226); W8(wGfxRegs2_LYC) = A;
  CYC(b_+226, b_+228); A = 0xf0;
  CYC(b_+228, b_+231); W8(wGfxRegs2_SCY) = A;
  CYC(b_+231, b_+233); A = 0x02;
  CYC(b_+233, b_+235); H8(hNextLcdInterruptBehaviour) = A;
  CYC(b_+235, b_+238); SET_HL(wCutsceneState);
  CYC(b_+238, b_+239); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+239, b_+240); alu_xor(gb, A);
  CYC(b_+240, b_+243); W8(wGenericCutscene_cbb3) = A;
  CYC(b_+243, b_+246); W8(wLinkForceState) = A;
  CYC(b_+246, b_+248); A = 0x08;
  CYC(b_+248, b_+251); mem_wr(gb, wWarpTransition, A);
  CYC(b_+251, b_+253); A = 0x81;
  CYC(b_+253, b_+256); W8(wDisabledObjects) = A;
  CYC(b_+256, b_+257); ret_effect(gb);
}

void cutscene15__state1__substate2_hook(GB *gb) {
  BASE(cutscene15);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+132, disableLcd_hook, SYM(disableLcd), b_+135);
  CALL_C(b_+135, clearOam_hook, SYM(clearOam), b_+138);
  CYC(b_+138, b_+139); alu_xor(gb, A);
  CYC(b_+139, b_+141); mem_wr(gb, IO_SVBK, A);
  CYC(b_+141, b_+144); SET_HL(w1ReservedInteraction0);
  CYC(b_+144, b_+147); SET_BC(0x0fc0);
  CALL_C(b_+147, clearMemoryBc_hook, SYM(clearMemoryBc), b_+150);
  CALL_C(b_+150, clearScreenVariables_hook, SYM(clearScreenVariables), b_+153);
  CALL_C(b_+153, clearMemoryOnScreenReload_hook, SYM(clearMemoryOnScreenReload), b_+156);
  CALL_C(b_+156, stopTextThread_hook, SYM(stopTextThread), b_+159);
  CALL_C(b_+159, applyWarpDest_hook, SYM(applyWarpDest), b_+162);
  CALL_C(b_+162, loadTilesetData_hook, SYM(loadTilesetData), b_+165);
  CALL_C(b_+165, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+168);
  CALL_C(b_+168, loadDungeonLayout_hook, SYM(loadDungeonLayout), b_+171);
  CALL_C(b_+171, func_131f_hook, SYM(func_131f), b_+174);
  CALL_C(b_+174, clearEnemiesKilledList_b00_hook, SYM(clearEnemiesKilledList_b00), b_+177);
  CALL_C(b_+177, func_5c6b_hook, SYM(func_5c6b), b_+180);
  cutscene15_substate2_after_room(gb, sp0_);
}

void cutscene15__state1__substate2__afterCall4ca4_hook(GB *gb) {
  cutscene15_substate2_after_room(gb, gb->sp);
}

static void cutscene18_19(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(cutscene19);
  if (entry == SYM(cutscene18)) {
    CYC(SYM(cutscene18), (SYM(cutscene18) + 2)); C = 0x00;
    CYC((SYM(cutscene18) + 2), (SYM(cutscene18) + 4));
  } else {
    CYC(b_+0, b_+2); C = 0x01;
  }
  CYC(b_+2, b_+5); SET_HL(0x4b0a);
  CYC(b_+5, b_+7); E = 0x03;
  CALL_C(b_+7, interBankCall_hook, 0x008a, b_+10);
  CALL_C(b_+10, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+13);
  CYC(b_+13, b_+16); updateAllObjects_hook(gb);
}

void cutscene18_hook(GB *gb) { cutscene18_19(gb, SYM(cutscene18), gb->sp); }
void cutscene19_hook(GB *gb) { cutscene18_19(gb, SYM(cutscene19), gb->sp); }

void cutscene18__afterCall4d27_hook(GB *gb) {
  BASE(cutscene19);
  CYC(b_+13, b_+16); TAIL(updateAllObjects);
}

static void func_5d31_after_tree_gfx(GB *gb, uint16_t sp0_) {
  BASE(func_5d31);
  CYC(b_+3, b_+6); A = W8(wWarpTransition2);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+10);
    TAIL(applyWarpTransition2);
  }
  CYC(b_+7, b_+10);
  CALL_C(b_+10, updateStatusBar_hook, SYM(updateStatusBar), b_+13);
  CYC(b_+13, b_+16); updateAllObjects_hook(gb);
}

void func_5d31_hook(GB *gb) {
  BASE(func_5d31);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  func_5d31_after_tree_gfx(gb, sp0_);
}

void func_5d31__afterCall5d34_hook(GB *gb) {
  func_5d31_after_tree_gfx(gb, gb->sp);
}

void func_7f55_hook(GB *gb) {
  BASE(func_7f55);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPirateShipRoom);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+7); SET_HL(b_+38);
  CALL_ROM(b_+7, (SYM(objectSetVisiblec1) + 7));
  if (F & FC) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_HL(b_+30);
  CALL_ROM(b_+14, (SYM(clearAllItemsAndPutLinkOnGround__nextItem) + 5));
  if (F & FZ) CYCT(b_+17, b_+19);
  else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+21); A = 0x04;
    CYC(b_+21, b_+22); bank1_add_a_to_hl_from_rst(gb, b_+22);
  }
  CYC(b_+22, b_+24); B = 0x04;
  CYC(b_+24, b_+27); SET_DE(wPirateShipRoom);
  CYC(b_+27, b_+30); hook_continue(gb, (SYM(copyMemory) + 5), sp0_);
}

void func_7f90_hook(GB *gb) {
  BASE(func_7f90);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL((SYM(cutscene02__func_7ced) + 51));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_ROM(b_+8, (SYM(copyW4PaletteDataToW2TilesetBgPalettes) + 18));
  CYC(b_+11, b_+14); updateAllObjects__jump34ad_hook(gb);
}

void func_7fb5_hook(GB *gb) {
  BASE(func_7fb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL((SYM(cutscene02__func_7ced) + 51));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_ROM(b_+8, (SYM(getRoomDungeonProperties) + 13));
  CYC(b_+11, b_+14); hook_continue(gb, (SYM(updateAllObjects) + 108), sp0_);
}
