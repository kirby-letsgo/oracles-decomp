#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void b2_updateMenus_hook(GB *gb);
void b2_updateMenus__updateMenu_hook(GB *gb);
void menuSpecificCode_hook(GB *gb);
void menuStateFadeIntoMenu_hook(GB *gb);
void menuStateFadeIntoMenu__openMenu_hook(GB *gb);
void saveGraphicsOnEnterMenu_body_hook(GB *gb);
void menuStateFadeIntoGame_hook(GB *gb);
void copyTextCharactersFromSecretTextTable_hook(GB *gb);
void copyTextCharactersFromHlUntilNull_hook(GB *gb);
void copyTextCharactersFromHl_hook(GB *gb);
void b2_fileSelectScreen_hook(GB *gb);
void fileSelectMode0_hook(GB *gb);
void fileSelectMode1_hook(GB *gb);
void fileSelectMode1__afterCall_hook(GB *gb);
void fileSelectMode1__subModes_hook(GB *gb);
void runBank2Function_hook(GB *gb);
void menuStateFadeOutOfMenu_hook(GB *gb);
void menuStateFadeOutOfMenu__afterCall_hook(GB *gb);
void reloadGraphicsOnExitMenu_body_hook(GB *gb);
void reloadGraphicsOnExitMenu_body__afterCall_hook(GB *gb);
void saveQuitMenu_state2_hook(GB *gb);

static void add_double_index_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) { burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); return; }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void loadRememberedCompanion_hook(GB *gb);
void checkAndSpawnMaple_hook(GB *gb);
void stub_02_77f4_hook(GB *gb);
void checkTileValidForEnemySpawn_hook(GB *gb);
void checkPositionValidForEnemySpawn_hook(GB *gb);
void getNextValueFromRandomBuffer_hook(GB *gb);
void getCandidatePositionForEnemy_hook(GB *gb);
void checkEnemyPlacedAtPosition_hook(GB *gb);
void calculateRoomStateModifier_hook(GB *gb);
void createSeaEffectsPartIfApplicable_hook(GB *gb);
void func_02_7a3a_hook(GB *gb);
void func_02_52f6_hook(GB *gb);
void func_02_52f6__func1_hook(GB *gb);
void func_02_52f6__func2_hook(GB *gb);
void loadItemIconGfx_hook(GB *gb);
void loadItemIconGfx__func_hook(GB *gb);
void loadItemIconGfx__clear_hook(GB *gb);
void loadStatusBarMap_hook(GB *gb);
void func_02_494a_hook(GB *gb);
void updateStatusBar_body_hook(GB *gb);
void updateStatusBar_body__updateRupeeDisplay_hook(GB *gb);
void updateStatusBar_body__updateHeartDisplay_hook(GB *gb);
void updateStatusBar_body__biggoronSword_hook(GB *gb);
void func_02_494a__end_hook(GB *gb);
void getRingTiles_hook(GB *gb);
void getRingTiles__drawTile_hook(GB *gb);
void func_02_4149_hook(GB *gb);
void setFileSelectCursorOffsetToFileSelectMode_hook(GB *gb);
void setFileSelectModeTo1_hook(GB *gb);
void setFileSelectMode_hook(GB *gb);
void loadGfxRegisterState5AndIncFileSelectMode2_hook(GB *gb);
void incFileSelectMode2_hook(GB *gb);
void decFileSelectMode2IfBPressed_hook(GB *gb);
void decFileSelectMode2_hook(GB *gb);
void createBlankSpritesForItemSubmenu_hook(GB *gb);
void dungeonMap_generateScrollableTilemap_hook(GB *gb);
void dungeonMap_generateScrollableTilemap__nextFloor67eb_hook(GB *gb);
void dungeonMap_generateScrollableTilemap__nextTile67fc_hook(GB *gb);
void dungeonMap_generateScrollableTilemap__doneThisFloor6815_hook(GB *gb);
void dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook(GB *gb);
void dungeonMap_checkCompassTile_hook(GB *gb);
void dungeonMap_checkCompassTile__treasure68e3_hook(GB *gb);
void dungeonMap_checkCompassTile__nothing68ea_hook(GB *gb);
void dungeonMap_checkCompassTile__ret68ec_hook(GB *gb);
void dungeonMap_getFloorAddress_hook(GB *gb);
void dungeonMap_drawItemSprites_hook(GB *gb);
void getNumSmallKeys_hook(GB *gb);
void checkLinkHasBossKey_hook(GB *gb);
void checkLinkHasCompass_hook(GB *gb);
void checkLinkHasMap_hook(GB *gb);
void dungeonMap_drawFloorCursor_hook(GB *gb);
void dungeonMap_drawBossSymbolForFloor_hook(GB *gb);
void dungeonMap_drawLinkIcons_hook(GB *gb);
void dungeonMap_updateCursorFlickerCounter_hook(GB *gb);
void dungeonMap_drawCursor_hook(GB *gb);
void dungeonMap_drawArrows_hook(GB *gb);
void mapGetRoomIndexWithoutUnusedColumns_hook(GB *gb);
void mapMenu_checkCursorRoomVisited_hook(GB *gb);
void mapMenu_checkRoomVisited_hook(GB *gb);
void mapMenu_drawArrow_hook(GB *gb);
void mapMenu_drawCursor_hook(GB *gb);
void mapMenu_drawSpriteAtRoomIndex_hook(GB *gb);
void mapMenu_drawWarpSites_hook(GB *gb);
void mapMenu_drawWarpSites__drawWarpDest_hook(GB *gb);
void mapMenu_drawWarpSites__nextTree_hook(GB *gb);
void getTreeWarpDataIndex_hook(GB *gb);
void getTreeWarpDataForRoom_hook(GB *gb);
void getWarpTreeData_hook(GB *gb);
void getWarpTreeData__ret_hook(GB *gb);
void mapMenu_drawTimePortal_hook(GB *gb);
void mapMenu_clearUnvisitedTiles_hook(GB *gb);
void mapMenu_clearUnvisitedTiles__rowLoop_hook(GB *gb);
void mapMenu_clearUnvisitedTiles__columnLoop_hook(GB *gb);
void mapMenu_clearUnvisitedTiles__nextTile_hook(GB *gb);
void checkMoblinsKeepDestroyed_hook(GB *gb);
void checkAdvanceShopVisited_hook(GB *gb);
void dungeonMap_getLinkIconPosition_hook(GB *gb);
void dungeonMap_drawFloorList_hook(GB *gb);
void dungeonMap_drawFloorList__loop_hook(GB *gb);
void dungeonMap_drawFloorList__nextFloor_hook(GB *gb);
void drawTileABtoDE_hook(GB *gb);
void dungeonMap_updateScroll_hook(GB *gb);
void dungeonMap_updateScroll__nextRow_hook(GB *gb);
void dungeonMap_updateScroll__nextColumn_hook(GB *gb);
void dungeonMap_getTileForRoom_hook(GB *gb);
void dungeonMap_getTileForRoom__hidden_hook(GB *gb);
void dungeonMap_getTileForRoom__visited_hook(GB *gb);
void dungeonMap_getTileForRoom__ret_hook(GB *gb);
void dungeonMap_checkCanViewFloor_hook(GB *gb);
void mapMenu_loadPopupData_hook(GB *gb);
void getMinimapPopupType_hook(GB *gb);
void minimapPopupType_normal_hook(GB *gb);
void minimapPopupType_advanceShop_hook(GB *gb);
void minimapPopupType_cave_hook(GB *gb);
void minimapPopupType_gashaSpot_hook(GB *gb);
void minimapPopupType_portalSpot_hook(GB *gb);
void minimapPopupType_seedTree_hook(GB *gb);
void minimapPopupType_moblinsKeep_hook(GB *gb);
void minimapNoPopup_hook(GB *gb);
void minimapPopupType_shop_hook(GB *gb);
void minimapPopupType_vasuOrSyrup_hook(GB *gb);
void minimapPopupType_blackTower_hook(GB *gb);
void minimapPopupType_makuTree_hook(GB *gb);
void maupMenu_drawPopup_hook(GB *gb);
void dungeonMap_checkDirectionButtons_hook(GB *gb);
void dungeonMap_scrollingState0_hook(GB *gb);
void dungeonMap_checkCanScrollDown_hook(GB *gb);
void dungeonMap_checkCanScrollUp_hook(GB *gb);
void dungeonMap_scrollingState1_hook(GB *gb);
void mapMenu_copyTilemapToVram_hook(GB *gb);
void mapMenu_drawSprites_hook(GB *gb);
void mapMenu_performTileSubstitutions_hook(GB *gb);
void runGaleSeedMenu_hook(GB *gb);
void runGaleSeedMenu__runState_hook(GB *gb);
void galeSeedMenu_state0_hook(GB *gb);
void galeSeedMenu_state1_hook(GB *gb);
void galeSeedMenu_state2_hook(GB *gb);
void galeSeedMenu_gotoState1_hook(GB *gb);
void galeSeedMenu_state3_hook(GB *gb);
void galeSeedMenu_addOffsetToWarpIndex_hook(GB *gb);
void runMapMenu_hook(GB *gb);
void mapMenu_state0_hook(GB *gb);
void loadMinimapDisplayRoom_hook(GB *gb);
void dungeonMap_drawSmallKeyCount_hook(GB *gb);
void dungeonMap_calculateVisitedFloorsAndLinkPosition_hook(GB *gb);
void mapMenu_state1_hook(GB *gb);
void mapMenu_state1__checkInput_hook(GB *gb);
void mapGetRoomTextOrReturn_hook(GB *gb);
void mapGetRoomText_hook(GB *gb);
void mapGetRoomText__specialCode0_hook(GB *gb);
void mapGetRoomText__specialCode1_hook(GB *gb);
void mapGetRoomText__specialCode2_hook(GB *gb);
void mapGetRoomText__specialCode3_hook(GB *gb);
void mapGetRoomText__specialCode4_hook(GB *gb);
void mapGetRoomText__checkDungeonEntered_hook(GB *gb);
void getFileDisplayVariableAddress_hook(GB *gb);
void getFileDisplayVariableAddress_paramE_hook(GB *gb);
void fileSelectUpdateInput_hook(GB *gb);
void fileSelectUpdateInput__upOrDown_hook(GB *gb);
void func_02_448d_hook(GB *gb);
void fileSelectSetCursor_hook(GB *gb);
void func_02_461c_hook(GB *gb);
void getNameBufferLength_hook(GB *gb);
void copyNameToW4NameBuffer_hook(GB *gb);
void func_02_465c_hook(GB *gb);
void label_02_038_hook(GB *gb);
void label_02_038__secretEntry_hook(GB *gb);
void label_02_038__end_hook(GB *gb);
void textInput_getCursorPosition_hook(GB *gb);
void drawNameInputCursors_hook(GB *gb);
void drawNameInputCursors__upperOptions_hook(GB *gb);
void drawNameInputCursors__lowerOptions_hook(GB *gb);
void drawSecretInputCursors_hook(GB *gb);
void drawSecretInputCursors__upperOptions_hook(GB *gb);
void drawSecretInputCursors__lowerOptions_hook(GB *gb);
void textInput_lowerOption_updateFileSelectCursorPos_hook(GB *gb);
void textInput_lowerOption_updateFileSelectCursorPos2_hook(GB *gb);
void textInput_mapUpperXToLowerX_hook(GB *gb);
void textInput_mapUpperXToLowerX__label_hook(GB *gb);
void textInput_loadCharacterGfx_hook(GB *gb);
void copyTextCharacters_hook(GB *gb);
void loadFileDisplayVariables_hook(GB *gb);
void loadFileDisplayVariables__nextFile_hook(GB *gb);
void textInput_updateEntryCursor_hook(GB *gb);
void textInput_getOutputAddress_hook(GB *gb);
void textInput_getOutputAddressOffset_hook(GB *gb);
void fileSelectDrawHeartsAndDeathCounter_hook(GB *gb);
void fileSelectDrawAcornCursor_hook(GB *gb);
void fileSelectDrawAcornCursor__func_hook(GB *gb);
void runGameLinkMenu_hook(GB *gb);
void fileSelectMode7_hook(GB *gb);
void fileSelectMode7__mode7States_hook(GB *gb);
void fileSelectMode7__state0_hook(GB *gb);
void fileSelectMode7__state1_hook(GB *gb);
void fileSelectMode7__state2_hook(GB *gb);
void fileSelectMode7__state3_hook(GB *gb);
void fileSelectMode7__moveCursorToQuit_hook(GB *gb);
void fileSelectMode7__selectedSomething_hook(GB *gb);
void fileSelectMode7__func_02_4c4b_hook(GB *gb);
void fileSelectMode7__func_02_4c55_hook(GB *gb);
void fileSelectMode7__state4_hook(GB *gb);
void fileSelectMode7__state5_hook(GB *gb);
void fileSelectMode7__cancelLink_hook(GB *gb);
void fileSelectMode7__state6_hook(GB *gb);
void fileSelect_redrawDecorationsAndSetWramBank4_hook(GB *gb);
void fileSelectDrawLinkInOtherGame_hook(GB *gb);
void fileSelectDrawLink_hook(GB *gb);
void fileSelectDrawLink__draw_hook(GB *gb);
void fileSelectMode1__state0_hook(GB *gb);
void fileSelectMode1__state1_hook(GB *gb);
void fileSelectMode1__getNextFileSelectMode_hook(GB *gb);
void fileSelectMode1__state2_hook(GB *gb);
void fileSelectMode1__textSpeedMenu_checkInput_hook(GB *gb);
void fileSelectMode1__back_hook(GB *gb);
void fileSelectMode1__leftOrRight_hook(GB *gb);
void fileSelectMode1__textSpeedMenu_addCursorToOam_hook(GB *gb);
void fileSelectMode5_hook(GB *gb);
void fileSelectMode5__mode5States_hook(GB *gb);
void fileSelectMode5__state0_hook(GB *gb);
void fileSelectMode5__state1_hook(GB *gb);
void fileSelectMode5__upOrDown_hook(GB *gb);
void fileSelectMode3_hook(GB *gb);
void fileSelectMode3__mode3Update_hook(GB *gb);
void fileSelectMode3__mode0_hook(GB *gb);
void fileSelectMode3__mode1_hook(GB *gb);
void fileSelectMode3__mode2_hook(GB *gb);
void fileSelectMode3__mode3_hook(GB *gb);
void fileSelectMode3__func_02_4397_hook(GB *gb);
void fileSelectMode3__label_02_015_hook(GB *gb);
void fileSelectMode2__func_hook(GB *gb);
void fileSelectMode2__mode0_hook(GB *gb);
void fileSelectMode2__mode2_hook(GB *gb);
void runKidNameEntryMenu__func_hook(GB *gb);
void runKidNameEntryMenu__mode0_hook(GB *gb);
void runKidNameEntryMenu__mode1_hook(GB *gb);
void runKidNameEntryMenu__mode2_hook(GB *gb);
void runSecretEntryMenu__func_hook(GB *gb);
void runSecretEntryMenu__mode0_hook(GB *gb);
void runSecretEntryMenu__mode1_hook(GB *gb);
void runSecretEntryMenu__mode2_hook(GB *gb);
void runSecretEntryMenu__invalidSecret_hook(GB *gb);
void runSecretEntryMenu__setTextInputResult_hook(GB *gb);
void runSecretEntryMenu__loadRingSecretData_hook(GB *gb);
void runTextInput__aButton_hook(GB *gb);
void runTextInput__gotCharacter_hook(GB *gb);
void runTextInput__selectionRight_hook(GB *gb);
void runTextInput__updateEntryCursor_hook(GB *gb);
void runTextInput__lowerOptions_hook(GB *gb);
void runTextInput__nameTable_hook(GB *gb);
void runTextInput__secretTable_hook(GB *gb);
void runTextInput__bButton_hook(GB *gb);
void runTextInput__selectionLeft_hook(GB *gb);
void runTextInput__selectButton_hook(GB *gb);
void runTextInput__back_hook(GB *gb);
void runTextInput__rightButton_hook(GB *gb);
void runTextInput__leftButton_hook(GB *gb);
void runTextInput__leftOrRight_hook(GB *gb);
void runTextInput__leftOrRight__lowerOptions_hook(GB *gb);
void runTextInput__upButton_hook(GB *gb);
void runTextInput__downButton_hook(GB *gb);
void runTextInput__upOrDown_hook(GB *gb);
void runTextInput__startButton_hook(GB *gb);
void fileSelect_printError_hook(GB *gb);
void textInput_waitForInput_hook(GB *gb);
void checkDisplayDmgModeScreen__vblankLoop_hook(GB *gb);
void updateTilesetFlagsForIndoorRoomInAltWorld_hook(GB *gb);
void fileSelectMode4_hook(GB *gb);
void fileSelectMode4__mode4Update_hook(GB *gb);
void fileSelectMode4__mode0_hook(GB *gb);
void fileSelectMode4__mode1_hook(GB *gb);
void fileSelectMode4__mode2_hook(GB *gb);
void fileSelectMode4__mode3_hook(GB *gb);
void inventoryMenuState2_hook(GB *gb);
void inventoryMenuState2__subStates_hook(GB *gb);
void inventoryMenuState2__subState0_hook(GB *gb);
void inventoryMenuState2__subState1_hook(GB *gb);
void inventoryMenuState2__subState2_hook(GB *gb);
void inventoryMenuState2__buttonPressed_hook(GB *gb);
void inventoryMenuState2__func_02_57f3_hook(GB *gb);
void inventoryMenuState3_hook(GB *gb);
void inventoryMenuState3__subState0_hook(GB *gb);
void inventoryMenuState3__subState1_hook(GB *gb);
void inventoryMenuState3__subState2_hook(GB *gb);
void getDirectionButtonOffsetFromHl_hook(GB *gb);
void inventorySubscreen0CheckDirectionButtons_hook(GB *gb);
void inventorySubmenu1CheckDirectionButtons_hook(GB *gb);
void inventorySubmenu1CheckDirectionButtons__leftOrRight_hook(GB *gb);
void inventorySubmenu1CheckDirectionButtons__upOrDown_hook(GB *gb);
void inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow_hook(GB *gb);
void inventorySubmenu1CheckDirectionButtons__updateCursorOnRingBoxRow_hook(GB *gb);
void inventorySubmenu2CheckDirectionButtons_hook(GB *gb);
void inventorySubmenu2CheckDirectionButtons__leftOrRight_hook(GB *gb);
void inventorySubmenu2CheckDirectionButtons__upOrDown_hook(GB *gb);
void inventorySubmenu2CheckDirectionButtons__upOrDown__rightSide_hook(GB *gb);
void inventorySubmenu2CheckDirectionButtons__upOrDown__leftSide_hook(GB *gb);
void func_02_5938_hook(GB *gb);
void inventorySubscreen0_drawCursor_hook(GB *gb);
void inventorySubmenu1_drawCursor_hook(GB *gb);
void inventorySubmenu2_drawCursor_hook(GB *gb);
void func_02_5a35_hook(GB *gb);
void func_02_5a35__next_hook(GB *gb);
void func_02_5a35__seedOnlyCodeDone_hook(GB *gb);
void func_02_5a35__dontHaveSubItem_hook(GB *gb);
void cpInventorySelectedItemToHarp_hook(GB *gb);
void func_02_5afc_hook(GB *gb);
void getSeedTypeInventoryIndex_hook(GB *gb);
void drawEquippedSpriteForActiveRing_hook(GB *gb);
void drawEquippedSpriteForActiveRing__foundRing_hook(GB *gb);
void inventorySubscreen0_drawStoredItems_hook(GB *gb);
void inventorySubscreen1_drawTreasures_hook(GB *gb);
void inventorySubscreen1_drawTreasures__drawTreasure_hook(GB *gb);
void inventorySubscreen1_drawTreasures__nextTreasure_hook(GB *gb);
void inventorySubscreen1_drawTreasures__undrawRingBox_hook(GB *gb);
void inventorySubscreen1_drawTreasures__drawRings_hook(GB *gb);
void inventorySubscreen1_drawTreasures__drawRing_hook(GB *gb);
void inventorySubscreen1_drawTreasures__nextRing_hook(GB *gb);
void inventorySubscreen1_drawTreasures__getAddressToDrawTreasureAt_hook(GB *gb);
void inventorySubscreen2_drawTreasures_hook(GB *gb);
void inventorySubscreen2_drawTreasures__drawEssence_hook(GB *gb);
void inventorySubscreen2_drawTreasures__nextEssence_hook(GB *gb);
void inventorySubscreen2_drawTreasures__nextQuarterHeart_hook(GB *gb);
void inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece_hook(GB *gb);
void getRingBoxCapacity_hook(GB *gb);
void fillRectangleInTileMapWithMenuBlock_hook(GB *gb);
void fillRectangleInTilemap_hook(GB *gb);
void drawTreasureDisplayDataToBg_hook(GB *gb);
void drawTreasureDisplayDataToBg__writeTile_hook(GB *gb);
void drawTreasureDisplayDataToBg__clearTile_hook(GB *gb);
void drawTreasureDisplayDataToBg__writeTileHlpr_hook(GB *gb);
void inventoryMenuDrawSprites_hook(GB *gb);
void inventoryMenuDrawSprites__drawIfOnSubscreen1_hook(GB *gb);
void inventoryMenuDrawSprites__menuScrolling_hook(GB *gb);
void inventoryMenuDrawSprites__drawSpriteWithXOffset_hook(GB *gb);
void inventoryMenuDrawSprites__drawSprite_hook(GB *gb);
void inventoryMenuDrawHarpSprites_hook(GB *gb);
void inventoryMenuDrawHarpSprites__drawSpriteWithXOffset_hook(GB *gb);
void inventoryMenuDrawHarpSprites__drawSprite_hook(GB *gb);
void runInventoryMenu_hook(GB *gb);
void runInventoryMenu__inventoryMenuStates_hook(GB *gb);
void showItemText1_hook(GB *gb);
void showItemText2_hook(GB *gb);
void inventoryMenuState0_hook(GB *gb);
void func_02_55a8_hook(GB *gb);
void func_02_55b2_hook(GB *gb);
void func_02_55b2__subScreen0_hook(GB *gb);
void func_02_55b2__subScreen1_hook(GB *gb);
void func_02_55b2__subScreen2_hook(GB *gb);
void inventoryMenuState1_hook(GB *gb);
void inventoryMenuState1__func_02_5606_hook(GB *gb);
void inventoryMenuState1__subscreen0_hook(GB *gb);
void inventoryMenuState1__aOrB_hook(GB *gb);
void inventoryMenuState1__hasSubmenu_hook(GB *gb);
void inventoryMenuState1__finalizeEquip_hook(GB *gb);
void inventoryMenuState1__equipItem_hook(GB *gb);
void inventoryMenuState1__equipItem__unequipBiggoron_hook(GB *gb);
void inventoryMenuState1__equipItem__swapItems_hook(GB *gb);
void inventoryMenuState1__equipItem__equipBiggoron_hook(GB *gb);
void inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook(GB *gb);
void inventoryMenuState1__subscreen1_hook(GB *gb);
void inventoryMenuState1__checkEquipRing_hook(GB *gb);
void inventoryMenuState1__subscreen2_hook(GB *gb);
void runRingMenu_hook(GB *gb);
void runRingMenu__runStateCode_hook(GB *gb);
void ringMenu_state0_hook(GB *gb);
void ringMenu_copyTilemapToVram_hook(GB *gb);
void ringMenu_redrawRingListOrUnappraisedRings_hook(GB *gb);
void ringMenu_drawRingBox_hook(GB *gb);
void ringMenu_state1_hook(GB *gb);
void ringMenu_state1_unappraisedRings_hook(GB *gb);
void ringMenu_unappraisedRings_state0_hook(GB *gb);
void ringMenu_unappraisedRings_state0__bPressed_hook(GB *gb);
void ringMenu_unappraisedRings_state0__aPressed_hook(GB *gb);
void ringMenu_unappraisedRings_state1_hook(GB *gb);
void ringMenu_state1_restart_hook(GB *gb);
void ringMenu_unappraisedRings_state2_hook(GB *gb);
void ringMenu_unappraisedRings_state3_hook(GB *gb);
void ringMenu_unappraisedRings_state3__refund_hook(GB *gb);
void ringMenu_unappraisedRings_state4_hook(GB *gb);
void ringMenu_unappraisedRings_state4__not100th_hook(GB *gb);
void ringMenu_unappraisedRings_gotoState5_hook(GB *gb);
void ringMenu_showExitableText_hook(GB *gb);
void ringMenu_unappraisedRings_state5_hook(GB *gb);
void ringMenu_checkObtainedRingBox_hook(GB *gb);
void ringMenu_getUnappraisedRingIndex_hook(GB *gb);
void ringMenu_retIfCounterNotFinished_hook(GB *gb);
void runSecretListMenu__runState_hook(GB *gb);
void secretListMenu_state0__clearVramBank_hook(GB *gb);
void secretListMenu_state1_hook(GB *gb);
void secretListMenu_state1__upOrDown_hook(GB *gb);
void secretListMenu_state1__scrollDown_hook(GB *gb);
void secretListMenu_state1__scrollUp_hook(GB *gb);
void secretListMenu_state1__playSound_hook(GB *gb);
void secretListMenu_state1__end_hook(GB *gb);
void secretListMenu_state2_hook(GB *gb);
void secretListMenu_drawCursorSprite_hook(GB *gb);
void secretListMenu_printSecret_hook(GB *gb);
void secretListMenu_printSecret__end_hook(GB *gb);
void secretListMenu_printSecret__getSecretText_hook(GB *gb);
void secretListMenu_printSecret__val0_hook(GB *gb);
void secretListMenu_printSecret__val2_hook(GB *gb);
void secretListMenu_printSecret__val3_hook(GB *gb);
void secretListMenu_loadAllSecretNames_hook(GB *gb);
void secretListMenu_loadAllSecretNames__nextSecret_hook(GB *gb);
void secretListMenu_loadAllSecretNames__end_hook(GB *gb);
void secretListMenu_getSecretData_hook(GB *gb);
void ringMenu_state1_ringList_hook(GB *gb);
void ringMenu_ringList_substate0_hook(GB *gb);
void ringMenu_ringList_substate0__checkInput_hook(GB *gb);
void ringMenu_ringList_substate0__aPressed_hook(GB *gb);
void ringMenu_ringList_substate0__bPressed_hook(GB *gb);
void ringMenu_ringList_substate1_hook(GB *gb);
void ringMenu_updateRingText_hook(GB *gb);
void ringMenu_updateRingText__printDescription_hook(GB *gb);
void ringMenu_selectedRingFromList_hook(GB *gb);
void ringMenu_moveCursorToRingBox_hook(GB *gb);
void ringMenu_checkRingIsInBox_hook(GB *gb);
void ringMenu_checkRingIsInBox__nextRing_hook(GB *gb);
void ringMenu_checkRingIsInBox__foundRing_hook(GB *gb);
void ringMenu_initiateScrollRight_hook(GB *gb);
void ringMenu_initiateScroll_hook(GB *gb);
void ringMenu_setState_hook(GB *gb);
void ringMenu_state2_hook(GB *gb);
void ringMenu_state2__substate0_hook(GB *gb);
void ringMenu_state2__substate1_hook(GB *gb);
void ringMenu_state2__scrollLeft_hook(GB *gb);
void ringMenu_state2__scrollRight_hook(GB *gb);
void ringMenu_state2__doneScrolling_hook(GB *gb);
void ringMenu_checkRingListCursorMoved_hook(GB *gb);
void ringMenu_checkRingListCursorMoved__playSound_hook(GB *gb);
void ringMenu_checkRingBoxCursorMoved_hook(GB *gb);
void ringMenu_drawSprites_hook(GB *gb);
void ringMenu_drawEquippedRingSprite_hook(GB *gb);
void ringMenu_getSpriteOffsetForRingBoxPosition_hook(GB *gb);
void ringMenu_drawRingBoxCursor_hook(GB *gb);
void ringMenu_drawSpritesForRingsInBox_hook(GB *gb);
void ringMenu_drawSpritesForRingsInBox__loop_hook(GB *gb);
void ringMenu_drawSpritesForRingsInBox__nextRing_hook(GB *gb);
void ringMenu_calculateNumPagesForUnappraisedRings_hook(GB *gb);
void ringMenu_updateSelectedRingFromList_hook(GB *gb);
void ringMenu_clearRingSelectionArea_hook(GB *gb);
void ringMenu_drawUnappraisedRings_hook(GB *gb);
void ringMenu_drawUnappraisedRings__nextRing_hook(GB *gb);
void ringMenu_drawRingList_hook(GB *gb);
void ringMenu_drawRingList__nextRing_hook(GB *gb);
void ringMenu_drawPageCounter_hook(GB *gb);
void ringMenu_drawRingBoxContents_hook(GB *gb);
void ringMenu_drawRingBoxContents__nextRing_hook(GB *gb);
void ringMenu_drawRingBoxContents__drawRing_hook(GB *gb);
void ringMenu_drawRing_hook(GB *gb);
void ringMenu_updateDisplayedRingNumber_hook(GB *gb);
void ringMenu_updateDisplayedRingNumberWithGivenComparator_hook(GB *gb);
void ringMenu_updateDisplayedRingNumberWithGivenComparator__noRing_hook(GB *gb);
void ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber_hook(GB *gb);
void ringMenu_setDisplayedText_hook(GB *gb);
void ringMenu_retIfTextIsPrinting_hook(GB *gb);
void saveQuitMenu_checkIsGameOver_hook(GB *gb);
void runSaveAndQuitMenu__runState_hook(GB *gb);
void saveQuitMenu_state0__gameOver_hook(GB *gb);
void saveQuitMenu_state0__notGameOver_hook(GB *gb);
void saveQuitMenu_state1_hook(GB *gb);
void saveQuitMenu_state1__upOrDown_hook(GB *gb);
void saveQuitMenu_state1__bPressed_hook(GB *gb);
void saveQuitMenu_drawSprites_hook(GB *gb);
void fileSelectMode6__updateMode6_hook(GB *gb);
void fileSelectMode6__mode0_hook(GB *gb);
void fileSelectMode6__mode2_hook(GB *gb);

static uint16_t function_caller_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void swap_de_hl_memory(GB *gb) {
  BASE(generateRandomBuffer_b02);
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); C = A;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); mem_wr(gb, HL, C);
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+57); ret_effect(gb);
}

void functionCaller_b02_hook(GB *gb) {
  BASE(functionCaller_b02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(1)); C = L;
  CYC(b_+O(1), b_+OE(2)); A = H;
  CYC(b_+O(2), b_+OE(3)); push_effect(gb, b_+OE(3));
  HANDOFF(function_caller_jump_table(gb));
}

void addRoomToEnemiesKilledList_b02_hook(GB *gb) {
  BASE(addRoomToEnemiesKilledList_b02);
  CYC(b_+0, b_+3); SET_HL(wEnemiesKilledList);
  CYC(b_+3, b_+6); A = mem_rd(gb, wActiveRoom);
  CYC(b_+6, b_+8); B = 0x08;
  for (;;) {
    CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+9, b_+11); break; }
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); L = alu_inc8(gb, L);
    CYC(b_+12, b_+13); L = alu_inc8(gb, L);
    CYC(b_+13, b_+14); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+14, b_+16); continue; }
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+19); A = mem_rd(gb, wEnemiesKilledListTail);
    CYC(b_+19, b_+20); B = A;
    CYC(b_+20, b_+21); A = alu_inc8(gb, A);
    CYC(b_+21, b_+22); A = alu_inc8(gb, A);
    CYC(b_+22, b_+24); alu_and(gb, 0x0f);
    CYC(b_+24, b_+27); mem_wr(gb, wEnemiesKilledListTail, A);
    CYC(b_+27, b_+28); A = B;
    CYC(b_+28, b_+30); alu_and(gb, 0x0f);
    CYC(b_+30, b_+32); alu_add(gb, GV(0xc0, 0x1f));
    CYC(b_+32, b_+33); L = A;
    CYC(b_+33, b_+36); A = mem_rd(gb, wActiveRoom);
    CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+37, b_+38); alu_xor(gb, A);
    CYC(b_+38, b_+39); mem_wr(gb, HL, A);
    CYC(b_+39, b_+42); mem_wr(gb, wEnemyPlacement_killedEnemiesBitset, A);
    CYC(b_+42, b_+43); ret_effect(gb);
    return;
  }
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+48); mem_wr(gb, wEnemyPlacement_killedEnemiesBitset, A);
  CYC(b_+48, b_+49); ret_effect(gb);
}

void markEnemyAsKilledInRoom_b02_hook(GB *gb) {
  BASE(markEnemyAsKilledInRoom_b02);
  CYC(b_+0, b_+3); SET_HL(wEnemiesKilledList);
  CYC(b_+3, b_+5); B = 0x08;
  CYC(b_+5, b_+8); A = mem_rd(gb, wActiveRoom);
  for (;;) {
    CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+9, b_+11); break; }
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); L = alu_inc8(gb, L);
    CYC(b_+12, b_+13); L = alu_inc8(gb, L);
    CYC(b_+13, b_+14); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+14, b_+16); continue; }
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); ret_effect(gb);
    return;
  }
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); E = 0x80;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_and(gb, 0x70);
  CYC(b_+23, b_+25); A = alu_swap(gb, A);
  CYC(b_+25, b_+28); SET_BC(0x00f8);
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+31); A = mem_rd(gb, BC);
  CYC(b_+31, b_+32); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+34); ret_effect(gb);
}

void clearEnemiesKilledList_b02_hook(GB *gb) {
  BASE(clearEnemiesKilledList_b02);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wEnemiesKilledListTail, A);
  CYC(b_+4, b_+7); SET_HL(wEnemiesKilledList);
  CYC(b_+7, b_+9); B = 0x10;
  CYC(b_+9, b_+12); TAIL(clearMemory);
}

void generateRandomBuffer_b02_hook(GB *gb) {
  BASE(generateRandomBuffer_b02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); A = 0x04;
  CYC(b_+3, b_+5); mem_wr(gb, 0xff70, A);
  CYC(b_+5, b_+8); SET_HL(w1Link);
  CYC(b_+8, b_+10); B = 0;
  do {
    CYC(b_+10, b_+11); A = B;
    CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+12, b_+13); B = alu_inc8(gb, B);
    if (!(F & FZ)) CYCT(b_+13, b_+15); else CYC(b_+13, b_+15);
  } while (!(F & FZ));
  CYC(b_+15, b_+18); SET_HL((w7d0f3 + 12));
  CYC(b_+18, b_+19); D = H;
  CALL_C(b_+19, getRandomNumber_hook, SYM(getRandomNumber), b_+22);
  CYC(b_+22, b_+23); E = A;
  CYC(b_+23, b_+26); push_effect(gb, b_+26); swap_de_hl_memory(gb);
  CYC(b_+26, b_+28); B = 0xff;
  do {
    CALL_C(b_+28, getRandomNumber_hook, SYM(getRandomNumber), b_+31);
    CYC(b_+31, b_+32); C = L;
    CALL_C(b_+32, multiplyAByC_hook, SYM(multiplyAByC), b_+35);
    CYC(b_+35, b_+36); E = H;
    CYC(b_+36, b_+37); L = C;
    CYC(b_+37, b_+39); H = 0xd0;
    CYC(b_+39, b_+42); push_effect(gb, b_+42); swap_de_hl_memory(gb);
    CYC(b_+42, b_+43); L = alu_dec8(gb, L);
    if (!(F & FZ)) CYCT(b_+43, b_+45); else CYC(b_+43, b_+45);
  } while (!(F & FZ));
  CYC(b_+45, b_+47); A = 0x01;
  CYC(b_+47, b_+49); mem_wr(gb, 0xff70, A);
  CYC(b_+49, b_+50); SET_DE(pop_effect(gb));
  CYC(b_+50, b_+51); ret_effect(gb);
}

void getRandomPositionForEnemy_b02_hook(GB *gb) {
  BASE(getRandomPositionForEnemy_b02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x40;
  CYC(b_+2, b_+5); mem_wr(gb, wEnemyPlacement_randomPlacementAttemptCounter, A);
  for (;;) {
    CYC(b_+5, b_+8); SET_HL(wEnemyPlacement_randomPlacementAttemptCounter);
    CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) { CYCT(b_+9, b_+11); CYC(b_+36, b_+37); alu_scf(gb); CYC(b_+37, b_+38); ret_effect(gb); return; }
    CYC(b_+9, b_+11);
    CALL_C(b_+11, getCandidatePositionForEnemy_hook, SYM(getCandidatePositionForEnemy), b_+14);
    CYC(b_+14, b_+17); mem_wr(gb, wEnemyPlacement_enemyPos, A);
    CYC(b_+17, b_+18); C = A;
    CALL_C(b_+18, checkPositionValidForEnemySpawn_hook, SYM(checkPositionValidForEnemySpawn), b_+21);
    if (F & FC) { CYCT(b_+21, b_+23); continue; }
    CYC(b_+21, b_+23);
    CYC(b_+23, b_+25); A = mem_rd(gb, hFF8B);
    CYC(b_+25, b_+27); alu_and(gb, 0x04);
    if (F & FZ) {
      CYC(b_+27, b_+29);
      CALL_C(b_+29, checkTileValidForEnemySpawn_hook, SYM(checkTileValidForEnemySpawn), b_+32);
      if (F & FC) { CYCT(b_+32, b_+34); continue; }
      CYC(b_+32, b_+34);
    } else CYCT(b_+27, b_+29);
    CYC(b_+34, b_+35); alu_xor(gb, A);
    CYC(b_+35, b_+36); ret_effect(gb);
    return;
  }
}

void checkSpawnTimeportalInteraction_b02_hook(GB *gb) {
  BASE(checkSpawnTimeportalInteraction_b02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wcddd, A);
  CYC(b_+4, b_+7); SET_HL(wPortalGroup);
  CYC(b_+7, b_+10); A = mem_rd(gb, wActiveGroup);
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+16); A = mem_rd(gb, wActiveRoom);
  CYC(b_+16, b_+17); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+20); C = mem_rd(gb, HL);
  CALL_C(b_+20, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xde);
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+31); mem_wr(gb, wcddd, A);
  CYC(b_+31, b_+33); L = 0x4b;
  CYC(b_+33, b_+36); TAIL(setShortPosition_paramC);
}

void loadRememberedCompanion_hook(GB *gb) {
  BASE(loadRememberedCompanion);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wRememberedCompanionId);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); A = W8(wActiveGroup);
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+16); A = W8(wActiveRoom);
  CYC(b_+16, b_+17); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+21); A = W8(w1Companion_enabled);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); A = C;
  CYC(b_+24, b_+26); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(b_+26, b_+28); goto raft; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+31); W8(w1Companion_id) = A;
  CYC(b_+31, b_+33); A = 1;
  CYC(b_+33, b_+36); W8(w1Companion_enabled) = A;
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+41); W8(w1Companion_yh) = A;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+45); W8(w1Companion_xh) = A;
  CYC(b_+45, b_+46); ret_effect(gb);
  return;
raft:
  CYC(b_+46, b_+49); A = W8(wTilesetFlags);
  CYC(b_+49, b_+51); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+51, b_+52); ret_effect(gb); return; }
  CYC(b_+51, b_+52);
  CALL_C(b_+52, getFreeInteractionSlot_hook, ROM_getFreeInteractionSlot, b_+55);
  if (!(F & FZ)) { CYCT(b_+55, b_+56); ret_effect(gb); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0xe6);
  CYC(b_+58, b_+59); L = alu_inc8(gb, L);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 2);
  CYC(b_+61, b_+64); A = W8(wRememberedCompanionY);
  CYC(b_+64, b_+66); L = 0x4b;
  CYC(b_+66, b_+67); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+67, b_+68); L = alu_inc8(gb, L);
  CYC(b_+68, b_+71); A = W8(wRememberedCompanionX);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+72, b_+73); ret_effect(gb);
}

void checkAndSpawnMaple_hook(GB *gb) {
  BASE(checkAndSpawnMaple);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wIsMaplePresent) = A;
  CYC(b_+4, b_+7); A = W8(wcc85);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); A = W8(wActiveGroup);
  CYC(b_+12, b_+15); SET_HL(SYM(maplePastLocations));
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+16, b_+18); goto start_check; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+20); ret_effect(gb); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); A = W8(w1Companion_enabled);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+24, b_+25); ret_effect(gb); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); A = W8(wAnimalCompanion);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+29, b_+31); goto maple_table; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); alu_sub(gb, 0x0b);
maple_table:
  CYC(b_+33, b_+36); SET_HL(SYM(maplePresentLocationsTable));
  CYC(b_+36, b_+37); add_double_index_to_hl(gb, b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+39); H = mem_rd(gb, HL);
  CYC(b_+39, b_+40); L = A;
start_check:
  CYC(b_+40, b_+43); A = W8(wActiveRoom);
  CALL_C(b_+43, checkFlag_hook, SYM(checkFlag), b_+46);
  if (!(F & FZ)) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); A = 0x0f;
  CALL_C(b_+49, cpActiveRing_hook, SYM(cpActiveRing), b_+52);
  CYC(b_+52, b_+54); E = 0x1e;
  if (!(F & FZ)) CYCT(b_+54, b_+56);
  else { CYC(b_+54, b_+56); CYC(b_+56, b_+58); E = alu_srl(gb, E); }
  CYC(b_+58, b_+61); SET_HL(wMapleKillCounter);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+63); alu_cp(gb, E);
  if (F & FC) { CYCT(b_+63, b_+64); ret_effect(gb); return; }
  CYC(b_+63, b_+64); CYC(b_+64, b_+66); mem_wr(gb, HL, 0);
  CYC(b_+66, b_+69); SET_HL(w1Companion);
  CYC(b_+69, b_+71); A = 1;
  CYC(b_+71, b_+74); W8(wcc85) = A;
  CYC(b_+74, b_+77); W8(wIsMaplePresent) = A;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x0e);
  CYC(b_+80, b_+82); L = 0x0b;
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x18);
  CYC(b_+84, b_+86); L = 0x0d;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0xb8);
  CYC(b_+88, b_+89); ret_effect(gb);
}

void stub_02_77f4_hook(GB *gb) {
  BASE(stub_02_77f4);
  CYC(b_+0, b_+1); ret_effect(gb);
}

static void get_absolute_value(GB *gb) {
  BASE(checkPositionValidForEnemySpawn);
  CYC(b_+133, b_+135); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+135, b_+136); ret_effect(gb); return; }
  CYC(b_+135, b_+136); CYC(b_+136, b_+137); alu_cpl(gb);
  CYC(b_+137, b_+138); A = alu_inc8(gb, A);
  CYC(b_+138, b_+139); ret_effect(gb);
}

void checkTileValidForEnemySpawn_hook(GB *gb) {
  BASE(checkTileValidForEnemySpawn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xce;
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto invalid; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); B = 0xcf;
  CYC(b_+8, b_+9); A = mem_rd(gb, BC);
  CYC(b_+9, b_+12); SET_HL(SYM(enemyUnspawnableTilesTable));
  CALL_C(b_+12, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+15);
  if (!(F & FC)) { CYCT(b_+15, b_+16); ret_effect(gb); return; }
  CYC(b_+15, b_+16);
invalid:
  CYC(b_+16, b_+17); alu_scf(gb);
  CYC(b_+17, b_+18); ret_effect(gb);
}

static void check_enemy_position_bounds(GB *gb) {
  BASE(checkPositionValidForEnemySpawn);
  CYC(b_+7, b_+10); A = W8(wActiveGroup);
  CYC(b_+10, b_+12); alu_and(gb, 4);
  CYC(b_+12, b_+15); SET_HL(b_+52);
  if (F & FZ) CYCT(b_+15, b_+17);
  else { CYC(b_+15, b_+17); CYC(b_+17, b_+20); SET_HL(b_+68); }
  CYC(b_+20, b_+23); A = W8(wScreenTransitionDirection);
  CYC(b_+23, b_+24); alu_add(gb, A);
  CYC(b_+24, b_+25); add_double_index_to_hl(gb, b_+25);
  CYC(b_+25, b_+26); A = C;
  CYC(b_+26, b_+28); alu_and(gb, 0xf0);
  CYC(b_+28, b_+30); A = alu_swap(gb, A);
  CYC(b_+30, b_+31); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+31, b_+33); goto invalid; }
  CYC(b_+31, b_+33); CYC(b_+33, b_+34); SET_HL(HL + 1);
  CYC(b_+34, b_+35); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+35, b_+37); goto invalid; }
  CYC(b_+35, b_+37); A = C;
  CYC(b_+37, b_+38); alu_and(gb, 0x0f);
  CYC(b_+38, b_+40); SET_HL(HL + 1);
  CYC(b_+40, b_+42); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+42, b_+44); goto invalid; }
  CYC(b_+42, b_+44); CYC(b_+44, b_+45); SET_HL(HL + 1);
  CYC(b_+45, b_+46); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto invalid; }
  CYC(b_+46, b_+48); CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+50); ret_effect(gb);
  return;
invalid:
  CYC(b_+50, b_+51); alu_scf(gb);
  CYC(b_+51, b_+52); ret_effect(gb);
}

void checkPositionValidForEnemySpawn_hook(GB *gb) {
  BASE(checkPositionValidForEnemySpawn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wScreenVariables);
  CYC(b_+3, b_+5); alu_and(gb, 8);
  if (!(F & FZ)) { CYC(b_+5, b_+7); check_enemy_position_bounds(gb); return; }
  CYCT(b_+5, b_+7); CYC(b_+84, b_+85); alu_xor(gb, A);
  CYC(b_+85, b_+88); W8(wScreenTransitionDirection) = A;
  CYC(b_+88, b_+91); A = W8(wWarpDestPos);
  CYC(b_+91, b_+92); B = A;
  CYC(b_+92, b_+94); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+94, b_+96); check_enemy_position_bounds(gb); return; }
  CYC(b_+94, b_+96); CYC(b_+96, b_+98); alu_and(gb, 0xf0);
  CYC(b_+98, b_+100); A = alu_swap(gb, A);
  CYC(b_+100, b_+101); H = A;
  CYC(b_+101, b_+102); A = C;
  CYC(b_+102, b_+104); alu_and(gb, 0xf0);
  CYC(b_+104, b_+106); A = alu_swap(gb, A);
  CYC(b_+106, b_+107); alu_sub(gb, H);
  CYC(b_+107, b_+110); push_effect(gb, b_+110); get_absolute_value(gb);
  CYC(b_+110, b_+112); alu_cp(gb, 3);
  if (F & FC) { CYCT(b_+112, b_+114); goto check_x; }
  CYC(b_+112, b_+114); goto valid;
check_x:
  CYC(b_+116, b_+117); A = B;
  CYC(b_+117, b_+119); alu_and(gb, 0x0f);
  CYC(b_+119, b_+120); H = A;
  CYC(b_+120, b_+121); A = C;
  CYC(b_+121, b_+123); alu_and(gb, 0x0f);
  CYC(b_+123, b_+124); alu_sub(gb, H);
  CYC(b_+124, b_+127); push_effect(gb, b_+127); get_absolute_value(gb);
  CYC(b_+127, b_+129); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(b_+129, b_+131); goto valid; }
  CYC(b_+129, b_+131); CYC(b_+131, b_+132); alu_scf(gb);
  CYC(b_+132, b_+133); ret_effect(gb);
  return;
valid:
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CYC(b_+115, b_+116); ret_effect(gb);
}

void getNextValueFromRandomBuffer_hook(GB *gb) {
  BASE(getNextValueFromRandomBuffer);
  CYC(b_+0, b_+3); SET_HL(wEnemyPlacement_randomBufferIndex);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); A = 4;
  CYC(b_+6, b_+8); mem_wr(gb, IO_SVBK, A);
  CYC(b_+8, b_+9); L = mem_rd(gb, HL);
  CYC(b_+9, b_+11); H = 0xd0;
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+14); A = 1;
  CYC(b_+14, b_+16); mem_wr(gb, IO_SVBK, A);
  CYC(b_+16, b_+17); A = H;
  CYC(b_+17, b_+18); ret_effect(gb);
}

void checkEnemyPlacedAtPosition_hook(GB *gb) {
  BASE(checkEnemyPlacedAtPosition);
  CYC(b_+0, b_+3); A = W8(wEnemyPlacement_numEnemies);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5); CYC(b_+5, b_+6); push_effect(gb, BC);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); SET_HL(wEnemyPlacement_placedEnemyPositions);
  for (;;) {
    CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+11, b_+12); alu_cp(gb, B);
    if (F & FZ) { CYCT(b_+12, b_+14); break; }
    CYC(b_+12, b_+14); CYC(b_+14, b_+15); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+15, b_+17); continue; }
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+18); SET_BC(pop_effect(gb));
    CYC(b_+18, b_+19); alu_xor(gb, A);
    CYC(b_+19, b_+20); ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21); SET_BC(pop_effect(gb));
  CYC(b_+21, b_+22); alu_scf(gb);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void getCandidatePositionForEnemy_hook(GB *gb) {
  BASE(getCandidatePositionForEnemy);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+5); alu_and(gb, 4);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto dungeon; }
  CYC(b_+5, b_+7);
overworld:
  CALL_C(b_+7, getNextValueFromRandomBuffer_hook, SYM(getNextValueFromRandomBuffer), b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto overworld; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+17); alu_and(gb, 0x0f);
  CYC(b_+17, b_+19); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto overworld; }
  CYC(b_+19, b_+21);
  CALL_C(b_+21, checkEnemyPlacedAtPosition_hook, SYM(checkEnemyPlacedAtPosition), b_+24);
  if (F & FC) { CYCT(b_+24, b_+26); goto overworld; }
  CYC(b_+24, b_+26); CYC(b_+26, b_+27); A = B;
  CYC(b_+27, b_+28); ret_effect(gb);
  return;
dungeon:
  CALL_C(b_+28, getNextValueFromRandomBuffer_hook, SYM(getNextValueFromRandomBuffer), b_+31);
  CYC(b_+31, b_+33); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(b_+33, b_+35); goto dungeon; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); B = A;
  CYC(b_+36, b_+38); alu_and(gb, 0xf0);
  if (F & FZ) { CYCT(b_+38, b_+40); goto dungeon; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); alu_cp(gb, 0xa0);
  if (F & FZ) { CYCT(b_+42, b_+44); goto dungeon; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); A = B;
  CYC(b_+45, b_+47); alu_and(gb, 0x0f);
  if (F & FZ) { CYCT(b_+47, b_+49); goto dungeon; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(b_+51, b_+53); goto dungeon; }
  CYC(b_+51, b_+53);
  CALL_C(b_+53, checkEnemyPlacedAtPosition_hook, SYM(checkEnemyPlacedAtPosition), b_+56);
  if (F & FC) { CYCT(b_+56, b_+58); goto dungeon; }
  CYC(b_+56, b_+58); CYC(b_+58, b_+59); A = B;
  CYC(b_+59, b_+60); ret_effect(gb);
}

void calculateRoomStateModifier_hook(GB *gb) {
  BASE(calculateRoomStateModifier);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto standard; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wRoomPack);
  CYC(b_+9, b_+11); alu_cp(gb, 0x7f);
  if (F & FZ) { CYCT(b_+11, b_+13); goto companion_region; }
  CYC(b_+11, b_+13);
standard:
  CYC(b_+13, b_+16); A = W8(wTilesetFlags);
  CYC(b_+16, b_+18); alu_and(gb, 0x40);
  CYC(b_+18, b_+20); B = 0;
  if (F & FZ) CYCT(b_+20, b_+22);
  else { CYC(b_+20, b_+22); CYC(b_+22, b_+23); B = alu_inc8(gb, B); }
  CALL_C(b_+23, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 1);
  if (F & FZ) CYCT(b_+28, b_+30);
  else { CYC(b_+28, b_+30); CYC(b_+30, b_+31); B = alu_inc8(gb, B); }
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+35); W8(wRoomStateModifier) = A;
  CYC(b_+35, b_+36); ret_effect(gb);
  return;
companion_region:
  CYC(b_+36, b_+39); A = W8(wAnimalCompanion);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+40, b_+42); goto standard; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); alu_sub(gb, 0x0b);
  CYC(b_+44, b_+47); W8(wRoomStateModifier) = A;
  CYC(b_+47, b_+48); ret_effect(gb);
}

void createSeaEffectsPartIfApplicable_hook(GB *gb) {
  BASE(createSeaEffectsPartIfApplicable);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveCollisions);
  CYC(b_+3, b_+6); SET_HL(SYM(seaEffectTileTable));
  CYC(b_+6, b_+7); push_effect(gb, b_+7); add_a_to_hl(gb);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); push_effect(gb, b_+9); add_a_to_hl(gb);
  for (;;) {
    CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+10, b_+11); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
    CYC(b_+11, b_+12); CYC(b_+12, b_+13); push_effect(gb, HL);
    CALL_C(b_+13, findTileInRoom_hook, SYM(findTileInRoom), b_+16);
    CYC(b_+16, b_+17); SET_HL(pop_effect(gb));
    if (!(F & FZ)) { CYCT(b_+17, b_+19); continue; }
    CYC(b_+17, b_+19);
    CALL_C(b_+19, getFreePartSlot_hook, ROM_getFreePartSlot, b_+22);
    if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; }
    CYC(b_+22, b_+23); CYC(b_+23, b_+25); mem_wr(gb, HL, 0x2e);
    CYC(b_+25, b_+26); ret_effect(gb);
    return;
  }
}

void func_02_7a3a_hook(GB *gb) {
  BASE(func_02_7a3a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wcddd);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto spawn; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); W8(wcddd) = A;
  CYC(b_+11, b_+12); ret_effect(gb);
  return;
spawn:
  CALL_C(b_+12, getFreeInteractionSlot_hook, ROM_getFreeInteractionSlot, b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+16); ret_effect(gb); return; }
  CYC(b_+15, b_+16); CYC(b_+16, b_+18); mem_wr(gb, HL, 0xde);
  CYC(b_+18, b_+21); A = W8(wPortalPos);
  CYC(b_+21, b_+23); L = 0x4b;
  CYC(b_+23, b_+26); TAIL(setShortPosition);
}

void dungeonMap_drawItemSprites_hook(GB *gb) {
  BASE(dungeonMap_drawItemSprites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getNumSmallKeys_hook, SYM(getNumSmallKeys), b_+3);
  CYC(b_+3, b_+6); SET_HL(b_+64);
  if (!(F & FZ)) CALL_C_CC(b_+6, addSpritesToOam_hook, SYM(addSpritesToOam), b_+9);
  else CYC(b_+6, b_+9);
  CALL_C(b_+9, checkLinkHasBossKey_hook, SYM(checkLinkHasBossKey), b_+12);
  CYC(b_+12, b_+15); SET_HL(b_+55);
  if (!(F & FZ)) CALL_C_CC(b_+15, addSpritesToOam_hook, SYM(addSpritesToOam), b_+18);
  else CYC(b_+15, b_+18);
  CALL_C(b_+18, checkLinkHasCompass_hook, SYM(checkLinkHasCompass), b_+21);
  CYC(b_+21, b_+24); SET_HL(b_+46);
  if (!(F & FZ)) CALL_C_CC(b_+24, addSpritesToOam_hook, SYM(addSpritesToOam), b_+27);
  else CYC(b_+24, b_+27);
  CALL_C(b_+27, checkLinkHasMap_hook, SYM(checkLinkHasMap), b_+30);
  CYC(b_+30, b_+33); SET_HL(b_+37);
  if (!(F & FZ)) CALL_C_CC(b_+33, addSpritesToOam_hook, SYM(addSpritesToOam), b_+36);
  else CYC(b_+33, b_+36);
  CYC(b_+36, b_+37); ret_effect(gb);
}

void getNumSmallKeys_hook(GB *gb) {
  BASE(getNumSmallKeys);
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+6); SET_HL(wDungeonSmallKeys);
  CYC(b_+6, b_+7); push_effect(gb, b_+7); add_a_to_hl(gb);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void checkLinkHasBossKey_hook(GB *gb) {
  BASE(checkLinkHasBossKey);
  CYC(b_+0, b_+3); SET_HL(wDungeonBossKeys);
  CYC(b_+3, b_+6); A = W8(wDungeonIndex);
  CYC(b_+6, b_+9); TAIL(checkFlag);
}

void checkLinkHasCompass_hook(GB *gb) {
  BASE(checkLinkHasCompass);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); SET_HL(wDungeonCompasses);
  CYC(b_+4, b_+7); A = W8(wDungeonIndex);
  CALL_C(b_+7, checkFlag_hook, SYM(checkFlag), b_+10);
  CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
  CYC(b_+11, b_+12); ret_effect(gb);
}

void checkLinkHasMap_hook(GB *gb) {
  BASE(checkLinkHasMap);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); SET_HL(wDungeonMaps);
  CYC(b_+4, b_+7); A = W8(wDungeonIndex);
  CALL_C(b_+7, checkFlag_hook, SYM(checkFlag), b_+10);
  CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
  CYC(b_+11, b_+12); ret_effect(gb);
}

void dungeonMap_drawFloorCursor_hook(GB *gb) {
  BASE(dungeonMap_drawFloorCursor);
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+6); SET_HL(SYM(dungeonMapSymbolPositions));
  CYC(b_+6, b_+7); add_double_index_to_hl(gb, b_+7);
  CYC(b_+7, b_+10); A = W8(wMapMenu_floorIndex);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+13); alu_rrca(gb);
  CYC(b_+13, b_+14); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+17); C = 0;
  CYC(b_+17, b_+20); SET_HL(b_+23);
  CYC(b_+20, b_+23); TAIL(addSpritesToOam_withOffset);
}

void dungeonMap_drawBossSymbolForFloor_hook(GB *gb) {
  BASE(dungeonMap_drawBossSymbolForFloor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkLinkHasCompass_hook, SYM(checkLinkHasCompass), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = W8(wDungeonIndex);
  CYC(b_+7, b_+10); SET_HL((SYM(dungeonMapSymbolPositions) + 1));
  CYC(b_+10, b_+11); add_double_index_to_hl(gb, b_+11);
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+14); C = 0;
  CYC(b_+14, b_+17); SET_HL(b_+20);
  CYC(b_+17, b_+20); TAIL(addSpritesToOam_withOffset);
}

void dungeonMap_drawLinkIcons_hook(GB *gb) {
  BASE(dungeonMap_drawLinkIcons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wMapMenu_dungeonCursorFlicker);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto floor_list; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, dungeonMap_getLinkIconPosition_hook, SYM(dungeonMap_getLinkIconPosition), b_+9);
  CYC(b_+9, b_+12); SET_HL(wMapMenu_dungeonScrollY);
  CYC(b_+12, b_+13); A = B;
  CYC(b_+13, b_+14); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+16); alu_cp(gb, 0x12);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto floor_list; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, b_+22); alu_rrca(gb);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+25); C = alu_swap(gb, C);
  CYC(b_+25, b_+27); C = alu_rrc(gb, C);
  CYC(b_+27, b_+30); SET_HL(b_+62);
  CALL_C(b_+30, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+33);
floor_list:
  CYC(b_+33, b_+36); A = W8(wDungeonIndex);
  CYC(b_+36, b_+39); SET_HL(SYM(dungeonMapSymbolPositions));
  CYC(b_+39, b_+40); add_double_index_to_hl(gb, b_+40);
  CYC(b_+40, b_+43); A = W8(wMapMenu_linkFloor);
  CYC(b_+43, b_+44); C = A;
  CYC(b_+44, b_+47); A = W8(wDungeonNumFloors);
  CYC(b_+47, b_+48); A = alu_dec8(gb, A);
  CYC(b_+48, b_+49); alu_sub(gb, C);
  CYC(b_+49, b_+51); A = alu_swap(gb, A);
  CYC(b_+51, b_+52); alu_rrca(gb);
  CYC(b_+52, b_+53); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+53, b_+54); B = A;
  CYC(b_+54, b_+56); C = 0;
  CYC(b_+56, b_+59); SET_HL(b_+67);
  CYC(b_+59, b_+62); TAIL(addSpritesToOam_withOffset);
}

void dungeonMap_updateCursorFlickerCounter_hook(GB *gb) {
  BASE(dungeonMap_updateCursorFlickerCounter);
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wMapMenu_dungeonCursorFlicker);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_xor(gb, 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void dungeonMap_drawCursor_hook(GB *gb) {
  BASE(dungeonMap_drawCursor);
  CYC(b_+0, b_+3); A = W8(wSubmenuState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wMapMenu_dungeonCursorFlicker);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(b_+13, b_+15); alu_and(gb, 0xf8);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+19); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(b_+19, b_+21); alu_and(gb, 7);
  CYC(b_+21, b_+22); alu_add(gb, A);
  CYC(b_+22, b_+23); alu_add(gb, A);
  CYC(b_+23, b_+24); alu_add(gb, A);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+28); SET_HL(b_+31);
  CYC(b_+28, b_+31); TAIL(addSpritesToOam_withOffset);
}

void dungeonMap_drawArrows_hook(GB *gb) {
  BASE(dungeonMap_drawArrows);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wSubmenuState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, dungeonMap_checkCanScrollUp_hook, SYM(dungeonMap_checkCanScrollUp), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto down_arrow; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); SET_HL(b_+26);
  CALL_C(b_+13, addSpritesToOam_hook, SYM(addSpritesToOam), b_+16);
down_arrow:
  CALL_C(b_+16, dungeonMap_checkCanScrollDown_hook, SYM(dungeonMap_checkCanScrollDown), b_+19);
  if (F & FZ) { CYCT(b_+19, b_+20); ret_effect(gb); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+23); SET_HL(b_+31);
  CYC(b_+23, b_+26); TAIL(addSpritesToOam);
}

void mapGetRoomIndexWithoutUnusedColumns_hook(GB *gb) {
  BASE(mapGetRoomIndexWithoutUnusedColumns);
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+4); A = W8(wMapMenu_cursorIndex);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); alu_and(gb, 0xf0);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_add(gb, A);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, b_+13); alu_sub(gb, C);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+17); A = W8(wTilesetFlags);
  CYC(b_+17, b_+18); alu_rlca(gb);
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+20); SET_BC(pop_effect(gb));
  CYC(b_+20, b_+21); ret_effect(gb);
}

void mapMenu_checkCursorRoomVisited_hook(GB *gb) {
  BASE(mapMenu_checkCursorRoomVisited);
  CYC(b_+0, b_+3); A = W8(wMapMenu_cursorIndex);
  TAIL(mapMenu_checkRoomVisited);
}

void mapMenu_checkRoomVisited_hook(GB *gb) {
  BASE(mapMenu_checkRoomVisited);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); H = A;
  CYC(b_+2, b_+5); A = W8(wMapMenu_mode);
  CYC(b_+5, b_+6); alu_rrca(gb);
  CYC(b_+6, b_+7); A = H;
  CYC(b_+7, b_+10); SET_HL(wGroup1RoomFlags);
  if (F & FC) CYCT(b_+10, b_+12);
  else { CYC(b_+10, b_+12); CYC(b_+12, b_+15); SET_HL(wGroup0RoomFlags); }
  CYC(b_+15, b_+16); push_effect(gb, b_+16); add_a_to_hl(gb);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_bit(gb, 4, A);
  CYC(b_+19, b_+20); SET_HL(pop_effect(gb));
  CYC(b_+20, b_+21); ret_effect(gb);
}

void mapMenu_drawArrow_hook(GB *gb) {
  BASE(mapMenu_drawArrow);
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+14);
  CYC(b_+9, b_+12); A = W8(wMapMenu_currentRoom);
  CYC(b_+12, b_+14); TAIL(mapMenu_drawSpriteAtRoomIndex);
}

void mapMenu_drawCursor_hook(GB *gb) {
  BASE(mapMenu_drawCursor);
  CYC(b_+0, b_+3); SET_HL(b_+8);
  CYC(b_+3, b_+6); A = W8(wMapMenu_cursorIndex);
  CYC(b_+6, b_+8); TAIL(mapMenu_drawSpriteAtRoomIndex);
}

void mapMenu_drawSpriteAtRoomIndex_hook(GB *gb) {
  BASE(mapMenu_drawSpriteAtRoomIndex);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); SET_DE(0x1018);
  CYC(b_+4, b_+5); A = C;
  CYC(b_+5, b_+7); alu_and(gb, 0xf0);
  CYC(b_+7, b_+9); A = alu_srl(gb, A);
  CYC(b_+9, b_+10); alu_add(gb, D);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); A = C;
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+18); alu_add(gb, E);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+22); TAIL(addSpritesToOam_withOffset);
}

void mapMenu_drawWarpSites_hook(GB *gb) {
  BASE(mapMenu_drawWarpSites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(b_+49);
  CYC(b_+3, b_+6); SET_HL(wTmpcec0);
  CYC(b_+6, b_+8); B = 5;
  CALL_C(b_+8, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+11);
  CYC(b_+11, b_+14); A = W8(wFrameCounter);
  CYC(b_+14, b_+16); alu_and(gb, 0x18);
  CYC(b_+16, b_+17); alu_rrca(gb);
  CYC(b_+17, b_+18); alu_rrca(gb);
  CYC(b_+18, b_+20); L = 0xc3;
  CYC(b_+20, b_+21); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); C = 0;
  TAIL(mapMenu_drawWarpSites__drawWarpDest);
}

void mapMenu_drawWarpSites__drawWarpDest_hook(GB *gb) {
  BASE(mapMenu_drawWarpSites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+24, b_+25); A = C;
  CALL_C(b_+25, getTreeWarpDataIndex_hook, SYM(getTreeWarpDataIndex), b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+31); ret_effect(gb); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); push_effect(gb, BC);
  CYC(b_+32, b_+33); C = A;
  CALL_C(b_+33, mapMenu_checkRoomVisited_hook, SYM(mapMenu_checkRoomVisited), b_+36);
  if (F & FZ) { CYCT(b_+36, b_+38); mapMenu_drawWarpSites__nextTree_hook(gb); return; }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); A = C;
  CYC(b_+39, b_+42); SET_HL(wTmpcec0);
  CALL_C(b_+42, mapMenu_drawSpriteAtRoomIndex_hook, SYM(mapMenu_drawSpriteAtRoomIndex), b_+45);
  TAIL(mapMenu_drawWarpSites__nextTree);
}

void mapMenu_drawWarpSites__nextTree_hook(GB *gb) {
  BASE(mapMenu_drawWarpSites);
  CYC(b_+45, b_+46); SET_BC(pop_effect(gb));
  CYC(b_+46, b_+47); C = alu_inc8(gb, C);
  CYC(b_+47, b_+49); TAIL(mapMenu_drawWarpSites__drawWarpDest);
}

void getTreeWarpDataIndex_hook(GB *gb) {
  BASE(getTreeWarpDataIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CALL_C(b_+1, getWarpTreeData_hook, SYM(getWarpTreeData), b_+4);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); alu_add(gb, C);
  CYC(b_+6, b_+7); push_effect(gb, b_+7); add_a_to_hl(gb);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void getTreeWarpDataForRoom_hook(GB *gb) {
  BASE(getTreeWarpDataForRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CALL_C(b_+1, getWarpTreeData_hook, SYM(getWarpTreeData), b_+4);
  for (;;) {
    CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+5, b_+6); alu_or(gb, A);
    CYC(b_+6, b_+7); alu_scf(gb);
    if (F & FZ) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
    CYC(b_+7, b_+8);
    CYC(b_+8, b_+9); alu_cp(gb, C);
    if (F & FZ) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
    CYC(b_+9, b_+10);
    CYC(b_+10, b_+11); SET_HL(HL + 1);
    CYC(b_+11, b_+12); SET_HL(HL + 1);
    CYC(b_+12, b_+14);
  }
}

void getWarpTreeData_hook(GB *gb) {
  BASE(getWarpTreeData);
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+4); SET_HL(SYM(pastTreeWarps));
  CYC(b_+4, b_+7); A = W8(wTilesetFlags);
  CYC(b_+7, b_+8); alu_rlca(gb);
  if (F & FC) { CYCT(b_+8, b_+10); getWarpTreeData__ret_hook(gb); return; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); SET_HL(SYM(presentTreeWarps));
  CYC(b_+13, b_+16); A = mem_rd(gb, (wGroup0RoomFlags + 172));
  CYC(b_+16, b_+17); alu_rlca(gb);
  if (F & FC) { CYCT(b_+17, b_+19); getWarpTreeData__ret_hook(gb); return; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 3;
  CYC(b_+21, b_+22); push_effect(gb, b_+22); add_a_to_hl(gb);
  TAIL(getWarpTreeData__ret);
}

void getWarpTreeData__ret_hook(GB *gb) {
  BASE(getWarpTreeData);
  CYC(b_+22, b_+23); SET_AF(pop_effect(gb));
  CYC(b_+23, b_+24); ret_effect(gb);
}

void mapMenu_drawTimePortal_hook(GB *gb) {
  BASE(mapMenu_drawTimePortal);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(b_+43);
  CYC(b_+3, b_+6); SET_HL(wTmpcec0);
  CYC(b_+6, b_+8); B = 5;
  CALL_C(b_+8, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+11);
  CYC(b_+11, b_+13); L = 0xc3;
  CYC(b_+13, b_+16); A = W8(wFrameCounter);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+21); alu_and(gb, 3);
  CYC(b_+21, b_+22); alu_add(gb, A);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+27); SET_HL(wPortalGroup);
  CYC(b_+27, b_+30); A = W8(wTilesetFlags);
  CYC(b_+30, b_+31); alu_rlca(gb);
  CYC(b_+31, b_+33); alu_and(gb, 1);
  CYC(b_+33, b_+34); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+34, b_+35); ret_effect(gb); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+40); SET_HL(wTmpcec0);
  CYC(b_+40, b_+43); TAIL(mapMenu_drawSpriteAtRoomIndex);
}

void mapMenu_clearUnvisitedTiles_hook(GB *gb) {
  BASE(mapMenu_clearUnvisitedTiles);
  CYC(b_+0, b_+2); A = 4;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+7); SET_DE(0x0e0e);
  CYC(b_+7, b_+10); SET_HL(w1ReservedInteraction0_var03);
  CYC(b_+10, b_+12); B = 0;
  TAIL(mapMenu_clearUnvisitedTiles__rowLoop);
}

void mapMenu_clearUnvisitedTiles__rowLoop_hook(GB *gb) {
  BASE(mapMenu_clearUnvisitedTiles);
  CYC(b_+12, b_+14); C = 0;
  CYC(b_+14, b_+15); push_effect(gb, DE);
  TAIL(mapMenu_clearUnvisitedTiles__columnLoop);
}

void mapMenu_clearUnvisitedTiles__columnLoop_hook(GB *gb) {
  BASE(mapMenu_clearUnvisitedTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+18); A = alu_swap(gb, A);
  CYC(b_+18, b_+19); alu_add(gb, C);
  CALL_C(b_+19, mapMenu_checkRoomVisited_hook, SYM(mapMenu_checkRoomVisited), b_+22);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); mapMenu_clearUnvisitedTiles__nextTile_hook(gb); return; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 4);
  CYC(b_+26, b_+28); H = (uint8_t)(H | (1 << 2));
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0a);
  CYC(b_+30, b_+32); H = (uint8_t)(H & ~(1 << 2));
  TAIL(mapMenu_clearUnvisitedTiles__nextTile);
}

void mapMenu_clearUnvisitedTiles__nextTile_hook(GB *gb) {
  BASE(mapMenu_clearUnvisitedTiles);
  CYC(b_+32, b_+33); SET_HL(HL + 1);
  CYC(b_+33, b_+34); C = alu_inc8(gb, C);
  CYC(b_+34, b_+35); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); mapMenu_clearUnvisitedTiles__columnLoop_hook(gb); return; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); SET_DE(pop_effect(gb));
  CYC(b_+38, b_+40); A = 0x20;
  CYC(b_+40, b_+41); alu_sub(gb, E);
  CYC(b_+41, b_+42); push_effect(gb, b_+42); add_a_to_hl(gb);
  CYC(b_+42, b_+43); B = alu_inc8(gb, B);
  CYC(b_+43, b_+44); D = alu_dec8(gb, D);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); mapMenu_clearUnvisitedTiles__rowLoop_hook(gb); return; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); ret_effect(gb);
}

void checkMoblinsKeepDestroyed_hook(GB *gb) {
  BASE(checkMoblinsKeepDestroyed);
  CYC(b_+0, b_+2); A = GV(0x1a, 0x16);
  CYC(b_+2, b_+5); TAIL(checkGlobalFlag);
}

void checkAdvanceShopVisited_hook(GB *gb) {
  BASE(checkAdvanceShopVisited);
  CYC(b_+0, b_+3); A = mem_rd(gb, GV((wGroup1RoomFlags + 254), 0xc8af));
  CYC(b_+3, b_+5); alu_and(gb, 0x10);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void dungeonMap_getLinkIconPosition_hook(GB *gb) {
  BASE(dungeonMap_getLinkIconPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wMapMenu_linkFloor);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(wDungeonNumFloors);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  CYC(b_+8, b_+9); alu_sub(gb, B);
  CYC(b_+9, b_+10); H = A;
  CALL_C(b_+10, multiplyABy8_hook, SYM(multiplyABy8), b_+13);
  CYC(b_+13, b_+14); A = H;
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+16); alu_add(gb, C);
  CYC(b_+16, b_+18); alu_add(gb, 5);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+22); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(b_+22, b_+24); alu_and(gb, 0xf8);
  CYC(b_+24, b_+26); A = alu_swap(gb, A);
  CYC(b_+26, b_+27); alu_rlca(gb);
  CYC(b_+27, b_+28); C = A;
  CYC(b_+28, b_+29); A = B;
  CYC(b_+29, b_+30); alu_add(gb, C);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+34); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(b_+34, b_+36); alu_and(gb, 7);
  CYC(b_+36, b_+37); C = A;
  CYC(b_+37, b_+38); ret_effect(gb);
}

void dungeonMap_drawFloorList_hook(GB *gb) {
  BASE(dungeonMap_drawFloorList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+7); A = W8(wDungeonIndex);
  CYC(b_+7, b_+10); SET_HL(SYM(dungeonMapFloorListStartPositions));
  CYC(b_+10, b_+11); push_effect(gb, b_+11); add_a_to_hl(gb);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+15); SET_DE((w1ReservedInteraction0_var3f + 33));
  CALL_C(b_+15, addAToDe_hook, 0x0068, b_+18);
  CYC(b_+18, b_+21); A = W8(wDungeonNumFloors);
  CYC(b_+21, b_+22); A = alu_dec8(gb, A);
  CYC(b_+22, b_+23); C = A;
  TAIL(dungeonMap_drawFloorList__loop);
}

void dungeonMap_drawFloorList__loop_hook(GB *gb) {
  BASE(dungeonMap_drawFloorList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+23, checkLinkHasMap_hook, SYM(checkLinkHasMap), b_+26);
  if (!(F & FZ)) {
    CYCT(b_+26, b_+28);
  } else {
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+29); A = C;
    CYC(b_+29, b_+32); SET_HL(0x00f8);
    CYC(b_+32, b_+33); alu_add(gb, L);
    CYC(b_+33, b_+34); L = A;
    CYC(b_+34, b_+37); A = W8(wMapMenu_visitedFloors);
    CYC(b_+37, b_+38); alu_and(gb, mem_rd(gb, HL));
    CYC(b_+38, b_+40); A = 0x20;
    if (F & FZ) {
      CYCT(b_+40, b_+42);
      TAIL(dungeonMap_drawFloorList__nextFloor);
    }
    CYC(b_+40, b_+42);
  }
  CYC(b_+42, b_+45); A = W8(wDungeonMapBaseFloor);
  CYC(b_+45, b_+46); alu_add(gb, C);
  CYC(b_+46, b_+49); SET_HL(SYM(dungeonMapFloorNameTiles));
  CYC(b_+49, b_+50); add_double_index_to_hl(gb, b_+50);
  CYC(b_+50, b_+52); B = 0x02;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+53, drawTileABtoDE_hook, SYM(drawTileABtoDE), b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+57, drawTileABtoDE_hook, SYM(drawTileABtoDE), b_+60);
  CYC(b_+60, b_+62); A = 0x9c;
  CALL_C(b_+62, drawTileABtoDE_hook, SYM(drawTileABtoDE), b_+65);
  CYC(b_+65, b_+66); E = alu_inc8(gb, E);
  CYC(b_+66, b_+68); B = 0x04;
  CYC(b_+68, b_+70); A = 0xaa;
  CALL_C(b_+70, drawTileABtoDE_hook, SYM(drawTileABtoDE), b_+73);
  CYC(b_+73, b_+75); A = 0xab;
  CALL_C(b_+75, drawTileABtoDE_hook, SYM(drawTileABtoDE), b_+78);
  CYC(b_+78, b_+80); A = 0x1a;
  TAIL(dungeonMap_drawFloorList__nextFloor);
}

void dungeonMap_drawFloorList__nextFloor_hook(GB *gb) {
  BASE(dungeonMap_drawFloorList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+80, addAToDe_hook, 0x0068, b_+83);
  CYC(b_+83, b_+84); A = C;
  CYC(b_+84, b_+85); C = alu_dec8(gb, C);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+86, b_+88);
    TAIL(dungeonMap_drawFloorList__loop);
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); ret_effect(gb);
}

void drawTileABtoDE_hook(GB *gb) {
  BASE(drawTileABtoDE);
  CYC(b_+0, b_+1); mem_wr(gb, DE, A);
  CYC(b_+1, b_+3); D |= 0x04;
  CYC(b_+3, b_+4); A = B;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); D &= (uint8_t)~0x04;
  CYC(b_+7, b_+8); SET_DE(DE + 1);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void dungeonMap_updateScroll_hook(GB *gb) {
  BASE(dungeonMap_updateScroll);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0x70);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+6); A = W8(wMapMenu_dungeonScrollY);
  CALL_C(b_+6, multiplyABy8_hook, SYM(multiplyABy8), b_+9);
  CYC(b_+9, b_+12); SET_HL(w1ReservedItemC);
  CYC(b_+12, b_+13); alu_add_hl(gb, BC);
  CYC(b_+13, b_+16); SET_DE(w1Link_y);
  CYC(b_+16, b_+18); A = 0x12;
  CYC(b_+18, b_+20); mem_wr(gb, hFF8D, A);
  TAIL(dungeonMap_updateScroll__nextRow);
}

void dungeonMap_updateScroll__nextRow_hook(GB *gb) {
  BASE(dungeonMap_updateScroll);
  CYC(b_+20, b_+22); C = 0x08;
  TAIL(dungeonMap_updateScroll__nextColumn);
}

void dungeonMap_updateScroll__nextColumn_hook(GB *gb) {
  BASE(dungeonMap_updateScroll);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+22, b_+24); A = 0x04;
    CYC(b_+24, b_+26); hram_wr(gb, 0x70, A);
    CYC(b_+26, b_+27); A = mem_rd(gb, HL);
    CYC(b_+27, b_+29); B = 0x00;
    CYC(b_+29, b_+31); alu_cp(gb, 0x83);
    if (F & FZ) {
      CYCT(b_+31, b_+33);
    } else {
      CYC(b_+31, b_+33);
      CYC(b_+33, b_+35); alu_cp(gb, 0xad);
      if (F & FZ) {
        CYCT(b_+35, b_+37);
      } else {
        CYC(b_+35, b_+37);
        CYC(b_+37, b_+39); B = 0x02;
        CYC(b_+39, b_+41); alu_cp(gb, 0xae);
        if (F & FZ) {
          CYCT(b_+41, b_+43);
        } else {
          CYC(b_+41, b_+43);
          CYC(b_+43, b_+45); B = 0x04;
          CYC(b_+45, b_+47); alu_cp(gb, 0xaf);
          if (F & FZ) {
            CYCT(b_+47, b_+49);
          } else {
            CYC(b_+47, b_+49);
            CYC(b_+49, b_+51); B = 0x05;
          }
        }
      }
    }
    CALL_C(b_+51, drawTileABtoDE_hook, SYM(drawTileABtoDE), b_+54);
    CYC(b_+54, b_+55); SET_HL(HL + 1);
    CYC(b_+55, b_+56); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+56, b_+58);
      continue;
    }
    CYC(b_+56, b_+58);
    break;
  }
  CYC(b_+58, b_+60); A = 0x18;
  CALL_C(b_+60, addAToDe_hook, 0x0068, b_+63);
  CYC(b_+63, b_+65); A = mem_rd(gb, hFF8D);
  CYC(b_+65, b_+66); A = alu_dec8(gb, A);
  CYC(b_+66, b_+68); mem_wr(gb, hFF8D, A);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+70);
    TAIL(dungeonMap_updateScroll__nextRow);
  }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); SET_AF(pop_effect(gb));
  CYC(b_+71, b_+73); hram_wr(gb, 0x70, A);
  TAIL(dungeonMap_getTileForRoom);
}

void dungeonMap_getTileForRoom_hook(GB *gb) {
  BASE(dungeonMap_getTileForRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); B = A;
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    TAIL(dungeonMap_getTileForRoom__hidden);
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+8); L = B;
  CYC(b_+8, b_+11); A = W8(wDungeonFlagsAddressH);
  CYC(b_+11, b_+12); H = A;
  CYC(b_+12, b_+13); D = mem_rd(gb, HL);
  CALL_C(b_+13, getRoomDungeonProperties_hook, SYM(getRoomDungeonProperties), b_+16);
  CYC(b_+16, b_+17); E = B;
  CYC(b_+17, b_+18); SET_HL(pop_effect(gb));
  CYC(b_+18, b_+19); A = E;
  CYC(b_+19, b_+21); alu_cp(gb, 0x60);
  if (F & FZ) {
    CYCT(b_+21, b_+23);
    TAIL(dungeonMap_getTileForRoom__hidden);
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); alu_cp(gb, 0x70);
  if (F & FZ) {
    CYCT(b_+25, b_+27);
    TAIL(dungeonMap_getTileForRoom__hidden);
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); alu_bit(gb, 4, D);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+31);
    TAIL(dungeonMap_getTileForRoom__visited);
  }
  CYC(b_+29, b_+31);
  CALL_C(b_+31, dungeonMap_checkCompassTile_hook, SYM(dungeonMap_checkCompassTile), b_+34);
  if (!(F & FZ)) {
    CYCT(b_+34, b_+36);
    TAIL(dungeonMap_getTileForRoom__ret);
  }
  CYC(b_+34, b_+36);
  CALL_C(b_+36, checkLinkHasMap_hook, SYM(checkLinkHasMap), b_+39);
  CYC(b_+39, b_+41); A = 0xaf;
  if (!(F & FZ)) {
    CYCT(b_+41, b_+43);
    TAIL(dungeonMap_getTileForRoom__ret);
  }
  CYC(b_+41, b_+43);
  TAIL(dungeonMap_getTileForRoom__hidden);
}

void dungeonMap_getTileForRoom__hidden_hook(GB *gb) {
  BASE(dungeonMap_getTileForRoom);
  CYC(b_+43, b_+45); A = 0xac;
  CYC(b_+45, b_+47); TAIL(dungeonMap_getTileForRoom__ret);
}

void dungeonMap_getTileForRoom__visited_hook(GB *gb) {
  BASE(dungeonMap_getTileForRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+56, dungeonMap_checkCompassTile_hook, SYM(dungeonMap_checkCompassTile), b_+59);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+61);
    TAIL(dungeonMap_getTileForRoom__ret);
  }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); A = D;
  CYC(b_+62, b_+63); alu_or(gb, E);
  CYC(b_+63, b_+65); alu_and(gb, 0x0f);
  CYC(b_+65, b_+67); alu_add(gb, 0xb0);
  TAIL(dungeonMap_getTileForRoom__ret);
}

void dungeonMap_getTileForRoom__ret_hook(GB *gb) {
  BASE(dungeonMap_getTileForRoom);
  CYC(b_+67, b_+68); SET_DE(pop_effect(gb));
  CYC(b_+68, b_+69); SET_BC(pop_effect(gb));
  CYC(b_+69, b_+70); ret_effect(gb);
}

void dungeonMap_checkCanViewFloor_hook(GB *gb) {
  BASE(dungeonMap_checkCanViewFloor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkLinkHasMap_hook, SYM(checkLinkHasMap), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  CYC(b_+5, b_+7); A = mem_rd(gb, hFF8D);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  CYC(b_+8, b_+11); SET_HL(0x00f8);
  CYC(b_+11, b_+12); alu_add(gb, L);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+16); A = W8(wMapMenu_visitedFloors);
  CYC(b_+16, b_+17); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); SET_HL(pop_effect(gb));
  CYC(b_+18, b_+19); ret_effect(gb);
}

void mapMenu_loadPopupData_hook(GB *gb) {
  BASE(mapMenu_loadPopupData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, mapMenu_checkCursorRoomVisited_hook, SYM(mapMenu_checkCursorRoomVisited), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto no_icon;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); SET_HL(SYM(presentMinimapPopups));
  CYC(b_+8, b_+11); A = mem_rd(gb, wFakeResetMenu);
  CYC(b_+11, b_+12); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
  } else {
    CYC(b_+12, b_+14);
    CYC(b_+14, b_+17); SET_HL(SYM(pastMinimapPopups));
  }
  CYC(b_+17, b_+20); A = mem_rd(gb, wFileSelect_cbb6);
  CYC(b_+20, b_+21); C = A;
  for (;;) {
    CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+22, b_+24); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(b_+24, b_+26);
      goto no_icon;
    }
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+27); alu_cp(gb, C);
    CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!(F & FZ)) {
      CYCT(b_+28, b_+30);
      continue;
    }
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+32);
    break;
  }
  goto got_icon;

no_icon:
  CYC(b_+32, b_+33); alu_xor(gb, A);

got_icon:
  CYC(b_+33, b_+34); D = A;
  CYC(b_+34, b_+36); A = alu_swap(gb, A);
  CALL_C(b_+36, getMinimapPopupType_hook, SYM(getMinimapPopupType), b_+39);
  CYC(b_+39, b_+42); mem_wr(gb, wFileSelect_linkTimer, A);
  CYC(b_+42, b_+43); A = D;
  CALL_C(b_+43, getMinimapPopupType_hook, SYM(getMinimapPopupType), b_+46);
  CYC(b_+46, b_+49); SET_HL(wFileSelect_textInputCursorPos);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+51, b_+53);
  } else {
    CYC(b_+51, b_+53);
    CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  }
  CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+57, b_+59);
  } else {
    CYC(b_+57, b_+59);
    CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+60, b_+61); mem_wr(gb, HL, A); SET_HL(HL - 1);
  }
  CYC(b_+61, b_+64); SET_DE(0x8008);
  CYC(b_+64, b_+67); SET_BC(0x2080);
  CYC(b_+67, b_+70); A = mem_rd(gb, wFileSelect_cbb6);
  CYC(b_+70, b_+71); alu_cp(gb, D);
  if (F & FC) {
    CYCT(b_+71, b_+73);
  } else {
    CYC(b_+71, b_+73);
    CYC(b_+73, b_+75); B = 0x70;
  }
  CYC(b_+75, b_+77); alu_and(gb, 0x0f);
  CYC(b_+77, b_+78); alu_cp(gb, E);
  if (F & FC) {
    CYCT(b_+78, b_+80);
  } else {
    CYC(b_+78, b_+80);
    CYC(b_+80, b_+82); C = 0x20;
  }
  CYC(b_+82, b_+85); SET_HL(wFileSelect_cursorOffset);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+87); mem_wr(gb, HL, B);
  CYC(b_+87, b_+88); L = alu_inc8(gb, L);
  CYC(b_+88, b_+89); alu_sub(gb, B);
  CYC(b_+89, b_+90); B = A;
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+92); mem_wr(gb, HL, C);
  CYC(b_+92, b_+93); alu_sub(gb, C);
  CYC(b_+93, b_+94); alu_or(gb, B);
  if (F & FZ) {
    CYCT(b_+94, b_+95); ret_effect(gb);
    return;
  }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+97); L = 0xb9;
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x00);
  CYC(b_+99, b_+100); ret_effect(gb);
}

void getMinimapPopupType_hook(GB *gb) {
  BASE(getMinimapPopupType);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_and(gb, 0x0f);
  CYC(b_+2, b_+3); E = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(minimapPopupType_normal) && hook_enabled_at(gb, SYM(minimapPopupType_normal))) { minimapPopupType_normal_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_advanceShop) && hook_enabled_at(gb, SYM(minimapPopupType_advanceShop))) { minimapPopupType_advanceShop_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_cave) && hook_enabled_at(gb, SYM(minimapPopupType_cave))) { minimapPopupType_cave_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_gashaSpot) && hook_enabled_at(gb, SYM(minimapPopupType_gashaSpot))) { minimapPopupType_gashaSpot_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_portalSpot) && hook_enabled_at(gb, SYM(minimapPopupType_portalSpot))) { minimapPopupType_portalSpot_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_seedTree) && hook_enabled_at(gb, SYM(minimapPopupType_seedTree))) { minimapPopupType_seedTree_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_moblinsKeep) && hook_enabled_at(gb, SYM(minimapPopupType_moblinsKeep))) { minimapPopupType_moblinsKeep_hook(gb); return; }
    else if (jt_ == SYM(minimapNoPopup) && hook_enabled_at(gb, SYM(minimapNoPopup))) { minimapNoPopup_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_shop) && hook_enabled_at(gb, SYM(minimapPopupType_shop))) { minimapPopupType_shop_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_vasuOrSyrup) && hook_enabled_at(gb, SYM(minimapPopupType_vasuOrSyrup))) { minimapPopupType_vasuOrSyrup_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_blackTower) && hook_enabled_at(gb, SYM(minimapPopupType_blackTower))) { minimapPopupType_blackTower_hook(gb); return; }
    else if (jt_ == SYM(minimapPopupType_makuTree) && hook_enabled_at(gb, SYM(minimapPopupType_makuTree))) { minimapPopupType_makuTree_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void minimapPopupType_normal_hook(GB *gb) {
  BASE(minimapPopupType_normal);
  CYC(b_+0, b_+1); A = E;
  CYC(b_+1, b_+2); ret_effect(gb);
}

void minimapPopupType_advanceShop_hook(GB *gb) {
  BASE(minimapPopupType_advanceShop);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkAdvanceShopVisited_hook, SYM(checkAdvanceShopVisited), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x0e;
  CYC(b_+6, b_+7); ret_effect(gb);
}

void minimapPopupType_cave_hook(GB *gb) {
  BASE(minimapPopupType_cave);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cbb6);
  CALL_C(b_+3, mapGetRoomText_hook, SYM(mapGetRoomText), b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+9); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
    TAIL(minimapNoPopup);
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = E;
  CYC(b_+12, b_+13); ret_effect(gb);
}

void minimapPopupType_gashaSpot_hook(GB *gb) {
  BASE(minimapPopupType_gashaSpot);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cbb6);
  CALL_C(b_+3, getIndexOfGashaSpotInRoom_hook, SYM(getIndexOfGashaSpotInRoom), b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 7, C);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    TAIL(minimapNoPopup);
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = E;
  CYC(b_+11, b_+12); ret_effect(gb);
}

void minimapPopupType_portalSpot_hook(GB *gb) {
  BASE(minimapPopupType_portalSpot);
  CYC(b_+0, b_+3); SET_HL(wGroup0RoomFlags);
  CYC(b_+3, b_+6); A = mem_rd(gb, wFakeResetMenu);
  CYC(b_+6, b_+7); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+7, b_+9);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+12); SET_HL(wGroup1RoomFlags);
  }
  CYC(b_+12, b_+15); A = mem_rd(gb, wFileSelect_cbb6);
  CYC(b_+15, b_+16); push_effect(gb, b_+16); add_a_to_hl(gb);
  CYC(b_+16, b_+18); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+18, b_+20);
    TAIL(minimapNoPopup);
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); A = E;
  CYC(b_+21, b_+22); ret_effect(gb);
}

void minimapPopupType_seedTree_hook(GB *gb) {
  BASE(minimapPopupType_seedTree);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cbb6);
  CALL_C(b_+3, getTreeWarpDataForRoom_hook, SYM(getTreeWarpDataForRoom), b_+6);
  if (F & FC) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); SET_HL(HL + 1);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void minimapPopupType_moblinsKeep_hook(GB *gb) {
  BASE(minimapPopupType_moblinsKeep);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkMoblinsKeepDestroyed_hook, SYM(checkMoblinsKeepDestroyed), b_+3);
  CYC(b_+3, b_+5); A = 0x0f;
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void minimapNoPopup_hook(GB *gb) {
  BASE(minimapNoPopup);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+2); ret_effect(gb);
}

void minimapPopupType_shop_hook(GB *gb) {
  BASE(minimapPopupType_shop);
  CYC(b_+0, b_+2); A = 0x0e;
  CYC(b_+2, b_+3); ret_effect(gb);
}

void minimapPopupType_vasuOrSyrup_hook(GB *gb) {
  BASE(minimapPopupType_vasuOrSyrup);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cbb6);
  CYC(b_+3, b_+5); alu_cp(gb, 0x5d);
  CYC(b_+5, b_+7); A = 0x0c;
  if (F & FZ) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void minimapPopupType_blackTower_hook(GB *gb) {
  BASE(minimapPopupType_blackTower);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getBlackTowerProgress_hook, SYM(getBlackTowerProgress), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x11);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void minimapPopupType_makuTree_hook(GB *gb) {
  BASE(minimapPopupType_makuTree);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+3, b_+4); alu_rlca(gb);
  CYC(b_+4, b_+6); A = 0x0b;
  if (F & FC) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); SET_HL((wGroup0RoomFlags + 56));
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+12, b_+14); A = 0x04;
  if (F & FZ) {
    CYCT(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x07;
  CYC(b_+17, b_+18); ret_effect(gb);
}

// maupMenu_drawPopup@checkPopupExists: z set when neither popup slot holds a value.
static void maup_popup_check_exists(GB *gb) {
  BASE(maupMenu_drawPopup__updatePopupVariables);
  H = D;
  L = wMapMenu_popup1 & 0xff;
  CYC(b_+110, b_+113);
  CYC(b_+113, b_+114); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+114, b_+115); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+115, b_+116);
}

// maupMenu_drawPopup@updatePopupVariables and its four states; the caller pushed the return
// address the state's ret pops.
static void maup_update_popup_variables(GB *gb) {
  BASE(maupMenu_drawPopup__updatePopupVariables);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  SET_DE(wMapMenu_popupState);
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+13) { goto state0; }
    else if (jt_ == b_+42) { goto state1; }
    else if (jt_ == b_+68) { goto state2; }
    else if (jt_ == b_+95) { goto state3; }
    else { HANDOFF(jt_); }
  } while (0);
state0:
  CYC(b_+13, b_+16);
  maup_popup_check_exists(gb);
  if (F & FZ) { CYCT(b_+16, b_+18); goto resetPopup; }
  CYC(b_+16, b_+18);
  A = 0x01;
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  E = wMapMenu_popupSize & 0xff;
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  E = wTmpcbba & 0xff;
  A = alu_inc8(gb, A);
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); ret_effect(gb);
  return;
resetPopup:
  alu_xor(gb, A);
  SET_HL(wMapMenu_popupState);
  CYC(b_+29, b_+33);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = wMapMenu_popupSize & 0xff;
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  L = wTmpcbc0 & 0xff;
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+42); ret_effect(gb);
  return;
state1:
  CYC(b_+42, b_+45);
  maup_popup_check_exists(gb);
  if (F & FZ) { CYCT(b_+45, b_+47); goto gotoState3; }
  CYC(b_+45, b_+47);
  L = wTmpcbba & 0xff;
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x02);
  L = wMapMenu_popupSize & 0xff;
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  alu_cp(gb, 0x04);
  CYC(b_+57, b_+59);
  if (F & FC) { CYCT(b_+59, b_+60); ret_effect(gb); return; }
  CYC(b_+59, b_+60);
  L = wTmpcbba & 0xff;
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x18);
  L = wMapMenu_popupState & 0xff;
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x02);
state2:
  CYC(b_+68, b_+71);
  maup_popup_check_exists(gb);
  if (F & FZ) { CYCT(b_+71, b_+73); goto gotoState3; }
  CYC(b_+71, b_+73);
  L = wTmpcbba & 0xff;
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+76, b_+77); ret_effect(gb); return; }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x18);
  L = wTmpcbc0 & 0xff;
  CYC(b_+79, b_+81);
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  alu_xor(gb, 0x01);
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+85); mem_wr(gb, HL, A);
  CYC(b_+85, b_+86); ret_effect(gb);
  return;
gotoState3:
  H = D;
  L = wMapMenu_popupState & 0xff;
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x03);
  L = wTmpcbba & 0xff;
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x01);
state3:
  H = D;
  L = wTmpcbba & 0xff;
  CYC(b_+95, b_+98);
  CYC(b_+98, b_+99); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+99, b_+100); ret_effect(gb); return; }
  CYC(b_+99, b_+100);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x02);
  L = wMapMenu_popupSize & 0xff;
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  A = alu_dec8(gb, A);
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+107, b_+108); ret_effect(gb); return; }
  CYC(b_+107, b_+108);
  CYCT(b_+108, b_+110);
  goto resetPopup;
}

// runInventoryMenu@inventoryMenuStates, runRingMenu@runStateCode and
// secretListMenu_printSecret@getSecretText: jump tables the caller pushed a return address for.
static void run_inventory_menu_states(GB *gb) {
  BASE(runInventoryMenu__inventoryMenuStates);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMenuActiveState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(inventoryMenuState0) && hook_enabled_at(gb, SYM(inventoryMenuState0))) { inventoryMenuState0_hook(gb); return; }
    else if (jt_ == SYM(inventoryMenuState1) && hook_enabled_at(gb, SYM(inventoryMenuState1))) { inventoryMenuState1_hook(gb); return; }
    else if (jt_ == SYM(inventoryMenuState2) && hook_enabled_at(gb, SYM(inventoryMenuState2))) { inventoryMenuState2_hook(gb); return; }
    else if (jt_ == SYM(inventoryMenuState3) && hook_enabled_at(gb, SYM(inventoryMenuState3))) { inventoryMenuState3_hook(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

static void run_ring_menu_state_code(GB *gb) {
  BASE(runRingMenu__runStateCode);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMenuActiveState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(ringMenu_state0) && hook_enabled_at(gb, SYM(ringMenu_state0))) { ringMenu_state0_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_state1) && hook_enabled_at(gb, SYM(ringMenu_state1))) { ringMenu_state1_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_state2) && hook_enabled_at(gb, SYM(ringMenu_state2))) { ringMenu_state2_hook(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

static void secret_list_get_secret_text(GB *gb) {
  BASE(secretListMenu_printSecret__getSecretText);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  A = B;
  CYC(b_+0, b_+1);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+10) {
      SET_HL(SYM(generateGameTransferSecret));
      E = SYMBANK(generateGameTransferSecret);
      CYC(b_+10, b_+15);
      CYC(b_+15, b_+18);
      interBankCall_hook(gb);
      return;
    }
    else if (jt_ == b_+18) {
      SET_BC(0x0002);
      CYC(b_+18, b_+21);
      CYC(b_+21, b_+24);
      TAIL(secretFunctionCaller);
    }
    else if (jt_ == b_+24) {
      A = C;
      CYC(b_+24, b_+25);
      CYC(b_+25, b_+28); mem_wr(gb, wShortSecretIndex, A);
      C = B;
      B = 0x00;
      CYC(b_+28, b_+31);
      CYC(b_+31, b_+34);
      TAIL(secretFunctionCaller);
    }
    else { HANDOFF(jt_); }
  } while (0);
}

void maupMenu_drawPopup_hook(GB *gb) {
  BASE(maupMenu_drawPopup);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  maup_update_popup_variables(gb);
  CYC(b_+3, b_+6); SET_HL(wFileSelect_cursorOffset);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); C = mem_rd(gb, HL);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+12); A = mem_rd(gb, wFileSelect_cursorPos2);
  CYC(b_+12, b_+14); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
  } else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); push_effect(gb, BC);
    CYC(b_+17, b_+20); A = mem_rd(gb, wFileSelect_cbc0);
    CYC(b_+20, b_+22); alu_and(gb, 0x01);
    CYC(b_+22, b_+25); SET_HL(wFileSelect_textInputCursorPos);
    CYC(b_+25, b_+26); push_effect(gb, b_+26); add_a_to_hl(gb);
    CYC(b_+26, b_+27); A = mem_rd(gb, HL);
    CYC(b_+27, b_+30); SET_HL(SYM(mapIconOamTable));
    CYC(b_+30, b_+31); push_effect(gb, b_+31); add_a_to_hl(gb);
    CYC(b_+31, b_+32); A = mem_rd(gb, HL);
    CYC(b_+32, b_+33); push_effect(gb, b_+33); add_a_to_hl(gb);
    CALL_C(b_+33, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+36);
    CYC(b_+36, b_+37); SET_BC(pop_effect(gb));
  }
  CYC(b_+37, b_+40); A = mem_rd(gb, wFileSelect_cursorPos2);
  CYC(b_+40, b_+43); SET_HL(SYM(mapIconBorderOamTable));
  CYC(b_+43, b_+44); push_effect(gb, b_+44); add_a_to_hl(gb);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); push_effect(gb, b_+46); add_a_to_hl(gb);
  CYC(b_+46, b_+49); TAIL(addSpritesToOam_withOffset);
}

void dungeonMap_checkDirectionButtons_hook(GB *gb) {
  BASE(dungeonMap_checkDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSubmenuState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(dungeonMap_scrollingState0) && hook_enabled_at(gb, SYM(dungeonMap_scrollingState0))) { dungeonMap_scrollingState0_hook(gb); return; }
    else if (jt_ == SYM(dungeonMap_scrollingState1) && hook_enabled_at(gb, SYM(dungeonMap_scrollingState1))) { dungeonMap_scrollingState1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void dungeonMap_scrollingState0_hook(GB *gb) {
  BASE(dungeonMap_scrollingState0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getInputWithAutofire_hook, SYM(getInputWithAutofire), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto check_up;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, dungeonMap_checkCanScrollDown_hook, SYM(dungeonMap_checkCanScrollDown), b_+10);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
    goto move;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); ret_effect(gb);
  return;

check_up:
  CYC(b_+13, b_+15); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, dungeonMap_checkCanScrollUp_hook, SYM(dungeonMap_checkCanScrollUp), b_+19);
  if (F & FZ) {
    CYCT(b_+19, b_+20); ret_effect(gb);
    return;
  }
  CYC(b_+19, b_+20);

move:
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+22); A = B;
  CYC(b_+22, b_+25); mem_wr(gb, wFileSelect_cbb5, A);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+26, b_+28);
    CYC(b_+37, b_+40); A = mem_rd(gb, wFileSelect_textInputMode);
    CYC(b_+40, b_+41); alu_sub(gb, C);
    CYC(b_+41, b_+44); mem_wr(gb, wFileSelect_textInputMode, A);
  } else {
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+31); A = mem_rd(gb, wFileSelect_textInputMode);
    CYC(b_+31, b_+32); alu_add(gb, C);
    CYC(b_+32, b_+35); mem_wr(gb, wFileSelect_textInputMode, A);
    CYC(b_+35, b_+37);
  }
  CYC(b_+44, b_+45); A = C;
  CYC(b_+45, b_+46); D = A;
  CALL_C(b_+46, multiplyABy8_hook, SYM(multiplyABy8), b_+49);
  CYC(b_+49, b_+50); A = D;
  CYC(b_+50, b_+51); alu_add(gb, A);
  CYC(b_+51, b_+52); alu_add(gb, C);
  CYC(b_+52, b_+53); A = alu_inc8(gb, A);
  CYC(b_+53, b_+56); mem_wr(gb, wFakeResetMenu_delayCounter, A);
  CYC(b_+56, b_+59); SET_HL(wSubmenuState);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+62); A = 0x84;
  CYC(b_+62, b_+65); TAIL(playSound_b00);
}

void dungeonMap_checkCanScrollDown_hook(GB *gb) {
  BASE(dungeonMap_checkCanScrollDown);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+4); A = mem_rd(gb, wDungeonNumFloors);
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+9); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+9, b_+10); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto failure;
  }
  CYC(b_+10, b_+12);
  CALL_C(b_+12, checkLinkHasMap_hook, SYM(checkLinkHasMap), b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19);
    goto done;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+26); A = mem_rd(gb, wDungeonNumFloors);
  CYC(b_+26, b_+27); A = alu_dec8(gb, A);
  CYC(b_+27, b_+28); alu_sub(gb, C);
  CYC(b_+28, b_+29); C = A;
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+32); D = 0;
  for (;;) {
    CYC(b_+32, b_+33); D = alu_inc8(gb, D);
    CYC(b_+33, b_+34); A = E;
    CYC(b_+34, b_+35); alu_sub(gb, D);
    CYC(b_+35, b_+38); SET_HL(0x00f8);
    CYC(b_+38, b_+39); alu_add(gb, L);
    CYC(b_+39, b_+40); L = A;
    CYC(b_+40, b_+41); B = mem_rd(gb, HL);
    CYC(b_+41, b_+44); A = mem_rd(gb, wFileSelect_fontXor);
    CYC(b_+44, b_+45); alu_and(gb, B);
    CYC(b_+45, b_+46); A = D;
    if (!(F & FZ)) {
      CYCT(b_+46, b_+48);
      goto done;
    }
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+49); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+49, b_+51);
      continue;
    }
    CYC(b_+49, b_+51);
    break;
  }

failure:
  CYC(b_+51, b_+52); alu_xor(gb, A);

done:
  CYC(b_+52, b_+54); B = 0x01;
  CYC(b_+54, b_+55); alu_or(gb, A);
  CYC(b_+55, b_+56); SET_DE(pop_effect(gb));
  CYC(b_+56, b_+57); ret_effect(gb);
}

void dungeonMap_checkCanScrollUp_hook(GB *gb) {
  BASE(dungeonMap_checkCanScrollUp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+4); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto failure;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, checkLinkHasMap_hook, SYM(checkLinkHasMap), b_+10);
  CYC(b_+10, b_+12); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14);
    goto done;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+17, b_+18); E = A;
  CYC(b_+18, b_+21); A = mem_rd(gb, wDungeonNumFloors);
  CYC(b_+21, b_+22); A = alu_dec8(gb, A);
  CYC(b_+22, b_+23); alu_sub(gb, E);
  CYC(b_+23, b_+24); C = E;
  CYC(b_+24, b_+25); E = A;
  CYC(b_+25, b_+27); D = 0;
  for (;;) {
    CYC(b_+27, b_+28); D = alu_inc8(gb, D);
    CYC(b_+28, b_+29); A = E;
    CYC(b_+29, b_+30); alu_add(gb, D);
    CYC(b_+30, b_+33); SET_HL(0x00f8);
    CYC(b_+33, b_+34); alu_add(gb, L);
    CYC(b_+34, b_+35); L = A;
    CYC(b_+35, b_+36); B = mem_rd(gb, HL);
    CYC(b_+36, b_+39); A = mem_rd(gb, wFileSelect_fontXor);
    CYC(b_+39, b_+40); alu_and(gb, B);
    CYC(b_+40, b_+41); A = D;
    if (!(F & FZ)) {
      CYCT(b_+41, b_+43);
      goto done;
    }
    CYC(b_+41, b_+43);
    CYC(b_+43, b_+44); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+44, b_+46);
      continue;
    }
    CYC(b_+44, b_+46);
    break;
  }

failure:
  CYC(b_+46, b_+47); alu_xor(gb, A);

done:
  CYC(b_+47, b_+49); B = 0x00;
  CYC(b_+49, b_+50); alu_or(gb, A);
  CYC(b_+50, b_+51); SET_DE(pop_effect(gb));
  CYC(b_+51, b_+52); ret_effect(gb);
}

void dungeonMap_scrollingState1_hook(GB *gb) {
  BASE(dungeonMap_scrollingState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wFakeResetMenu_delayCounter);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+7); alu_xor(gb, A);
    CYC(b_+7, b_+10); mem_wr(gb, wSubmenuState, A);
    CYC(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYCT(b_+4, b_+6);
  CYC(b_+11, b_+14); A = mem_rd(gb, wFileSelect_cbb5);
  CYC(b_+14, b_+15); alu_or(gb, A);
  CYC(b_+15, b_+17); A = 0xff;
  if (F & FZ) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+21); A = 0x01;
  }
  CYC(b_+21, b_+24); SET_HL(wFileSelect_textInputMaxCursorPos);
  CYC(b_+24, b_+25); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CALL_C(b_+26, dungeonMap_updateScroll_hook, SYM(dungeonMap_updateScroll), SYM(mapMenu_copyTilemapToVram));
  TAIL(mapMenu_copyTilemapToVram);
}

void mapMenu_copyTilemapToVram_hook(GB *gb) {
  BASE(mapMenu_copyTilemapToVram);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+4, b_+6); A = 0x0a;
  CYC(b_+6, b_+9); TAIL(loadUncompressedGfxHeader);
}

void mapMenu_drawSprites_hook(GB *gb) {
  BASE(mapMenu_drawSprites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFakeResetMenu);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto overworld;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, dungeonMap_drawItemSprites_hook, SYM(dungeonMap_drawItemSprites), b_+10);
  CALL_C(b_+10, dungeonMap_drawLinkIcons_hook, SYM(dungeonMap_drawLinkIcons), b_+13);
  CALL_C(b_+13, dungeonMap_drawCursor_hook, SYM(dungeonMap_drawCursor), b_+16);
  CALL_C(b_+16, dungeonMap_drawArrows_hook, SYM(dungeonMap_drawArrows), b_+19);
  CALL_C(b_+19, dungeonMap_drawBossSymbolForFloor_hook, SYM(dungeonMap_drawBossSymbolForFloor), b_+22);
  CYC(b_+22, b_+25); dungeonMap_drawFloorCursor_hook(gb);
  return;

overworld:
  CALL_C(b_+25, maupMenu_drawPopup_hook, SYM(maupMenu_drawPopup), b_+28);
  CALL_C(b_+28, mapMenu_drawArrow_hook, SYM(mapMenu_drawArrow), b_+31);
  CALL_C(b_+31, mapMenu_drawCursor_hook, SYM(mapMenu_drawCursor), b_+34);
  CYC(b_+34, b_+37); A = mem_rd(gb, wGenericCutscene_cbc1);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+41);
    TAIL(mapMenu_drawWarpSites);
  }
  CYC(b_+38, b_+41);
  CYC(b_+41, b_+44); TAIL_GV(mapMenu_drawTimePortal, mapMenu_drawJewelLocations);
}

void mapMenu_performTileSubstitutions_hook(GB *gb) {
  BASE(mapMenu_performTileSubstitutions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(mapMenu_tileSubstitutionTable));
  CYC(b_+3, b_+4); push_effect(gb, b_+4); add_a_to_hl(gb);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+6); push_effect(gb, b_+6); add_a_to_hl(gb);
  for (;;) {
    CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+7, b_+8); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
    CYC(b_+8, b_+9);
    CYC(b_+9, b_+10); B = A;
    CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+11, b_+12); E = A;
    CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+13, b_+14); D = A;
    CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+15, b_+16); C = A;
    CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+17, b_+18); push_effect(gb, HL);
    CYC(b_+18, b_+19); H = A;
    CYC(b_+19, b_+20); L = C;
    CYC(b_+20, b_+21); A = B;
    CYC(b_+21, b_+23); alu_and(gb, 0x0f);
    CYC(b_+23, b_+24); C = A;
    CYC(b_+24, b_+25); A = B;
    CYC(b_+25, b_+27); alu_and(gb, 0xf0);
    CYC(b_+27, b_+29); A = alu_swap(gb, A);
    CYC(b_+29, b_+30); B = A;
    for (;;) {
      CYC(b_+30, b_+31); push_effect(gb, BC);
      for (;;) {
        CYC(b_+31, b_+32); A = mem_rd(gb, HL);
        CYC(b_+32, b_+33); mem_wr(gb, DE, A);
        CYC(b_+33, b_+35); H |= 0x04;
        CYC(b_+35, b_+37); D |= 0x04;
        CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+38, b_+39); mem_wr(gb, DE, A);
        CYC(b_+39, b_+40); SET_DE(DE + 1);
        CYC(b_+40, b_+42); H &= (uint8_t)~0x04;
        CYC(b_+42, b_+44); D &= (uint8_t)~0x04;
        CYC(b_+44, b_+45); C = alu_dec8(gb, C);
        if (C) { CYCT(b_+45, b_+47); continue; }
        CYC(b_+45, b_+47);
        break;
      }
      CYC(b_+47, b_+48); SET_BC(pop_effect(gb));
      CYC(b_+48, b_+50); A = 0x20;
      CYC(b_+50, b_+51); alu_sub(gb, C);
      CYC(b_+51, b_+53); H8(hFF8B) = A;
      CYC(b_+53, b_+54); push_effect(gb, b_+54); add_a_to_hl(gb);
      CYC(b_+54, b_+56); A = H8(hFF8B);
      CALL_C(b_+56, addAToDe_hook, 0x0068, b_+59);
      CYC(b_+59, b_+60); B = alu_dec8(gb, B);
      if (B) { CYCT(b_+60, b_+62); continue; }
      CYC(b_+60, b_+62);
      break;
    }
    CYC(b_+62, b_+63); SET_HL(pop_effect(gb));
    CYC(b_+63, b_+65);
  }
}

void runGaleSeedMenu__runState_hook(GB *gb) {
  BASE(runGaleSeedMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); A = W8(wMenuActiveState);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(galeSeedMenu_state0) && hook_enabled_at(gb, SYM(galeSeedMenu_state0))) { galeSeedMenu_state0_hook(gb); return; }
    else if (jt_ == SYM(galeSeedMenu_state1) && hook_enabled_at(gb, SYM(galeSeedMenu_state1))) { galeSeedMenu_state1_hook(gb); return; }
    else if (jt_ == SYM(galeSeedMenu_state2) && hook_enabled_at(gb, SYM(galeSeedMenu_state2))) { galeSeedMenu_state2_hook(gb); return; }
    else if (jt_ == SYM(galeSeedMenu_state3) && hook_enabled_at(gb, SYM(galeSeedMenu_state3))) { galeSeedMenu_state3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void runGaleSeedMenu_hook(GB *gb) {
  BASE(runGaleSeedMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CALL_C(b_+3, runGaleSeedMenu__runState_hook, b_+9, b_+6);
  CYC(b_+6, b_+9);
  TAIL(mapMenu_drawSprites);
}

void galeSeedMenu_state0_hook(GB *gb) {
  BASE(galeSeedMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, mapMenu_state0_hook, SYM(mapMenu_state0), b_+3);
  CYC(b_+3, b_+5); A = 0xff;
  CYC(b_+5, b_+8); W8(wMapMenu_warpIndex) = A;
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+13); W8(wMapMenu_drawWarpDestinations) = A;
  CYC(b_+13, b_+16);
  TAIL(galeSeedMenu_addOffsetToWarpIndex);
}

void galeSeedMenu_state1_hook(GB *gb) {
  BASE(galeSeedMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) CYC(b_+4, b_+6);
  else { CYCT(b_+4, b_+6); goto end; }
  CYC(b_+6, b_+9); A = W8(wKeysJustPressed);
  CYC(b_+9, b_+11); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto b_pressed; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x09);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto a_pressed; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); SET_HL(b_+64);
  CALL_C(b_+20, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+23);
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto end; }
  CYC(b_+23, b_+25);
  CALL_C(b_+25, galeSeedMenu_addOffsetToWarpIndex_hook, SYM(galeSeedMenu_addOffsetToWarpIndex), b_+28);
  CYC(b_+28, b_+30); A = 0x84;
  if (!(F & FZ)) CALL_C_CC(b_+30, playSound_b00_hook, SYM(playSound_b00), b_+33);
  else CYC(b_+30, b_+33);
end:
  CYC(b_+33, b_+36);
  TAIL(mapMenu_loadPopupData);
b_pressed:
  CALL_C(b_+36, mapGetRoomTextOrReturn_hook, SYM(mapGetRoomTextOrReturn), b_+39);
  CYC(b_+39, b_+41); A = 0x03;
  CYC(b_+41, b_+43); C = 0x01;
  CYC(b_+43, b_+45);
  goto set_state;
a_pressed:
  CALL_C(b_+45, mapGetRoomTextOrReturn_hook, SYM(mapGetRoomTextOrReturn), b_+48);
  CYC(b_+48, b_+49); A = C;
  CYC(b_+49, b_+52); mem_wr(gb, wTextSubstitutions + 2, A);
  CYC(b_+52, b_+54); C = 0x00;
  CYC(b_+54, b_+56); A = 0x02;
set_state:
  CYC(b_+56, b_+59); W8(wMenuActiveState) = A;
  CYC(b_+59, b_+61); B = 0x03;
  CYC(b_+61, b_+64);
  TAIL(showText);
}

void galeSeedMenu_state2_hook(GB *gb) {
  BASE(galeSeedMenu_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); A = W8(wSelectedTextOption);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    TAIL(galeSeedMenu_gotoState1);
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); W8(wOpenedMenuType) = A;
  CYC(b_+12, b_+15); A = W8(wActiveGroup);
  CYC(b_+15, b_+17); alu_or(gb, 0x80);
  CYC(b_+17, b_+20); W8(wWarpDestGroup) = A;
  CYC(b_+20, b_+23); A = W8(wMapMenu_warpIndex);
  CALL_C(b_+23, getTreeWarpDataIndex_hook, SYM(getTreeWarpDataIndex), b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+30); W8(wWarpDestRoom) = A;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+34); W8(wWarpDestPos) = A;
  CYC(b_+34, b_+36); A = 0x05;
  CYC(b_+36, b_+39); W8(wWarpTransition) = A;
  CYC(b_+39, b_+41); A = 0x03;
  CYC(b_+41, b_+44); W8(wWarpTransition2) = A;
  CYC(b_+44, b_+46); A = 0x03;
  CALL_C(b_+46, setMusicVolume_hook, SYM(setMusicVolume), b_+49);
  CYC(b_+49, b_+52);
  TAIL(fadeoutToWhite);
}

void galeSeedMenu_gotoState1_hook(GB *gb) {
  BASE(galeSeedMenu_gotoState1);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); W8(wMenuActiveState) = A;
  CYC(b_+5, b_+6); ret_effect(gb);
}

void galeSeedMenu_state3_hook(GB *gb) {
  BASE(galeSeedMenu_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); A = W8(wSelectedTextOption);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+7, b_+9);
    TAIL(galeSeedMenu_gotoState1);
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0xff;
  CYC(b_+11, b_+14); W8(wWarpTransition2) = A;
  CYC(b_+14, b_+17);
  TAIL(closeMenu);
}

void galeSeedMenu_addOffsetToWarpIndex_hook(GB *gb) {
  BASE(galeSeedMenu_addOffsetToWarpIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = A;
  CYC(b_+1, b_+4); A = W8(wMapMenu_warpIndex);
  CYC(b_+4, b_+5); D = A;
  for (;;) {
    CYC(b_+5, b_+6); A = D;
    CYC(b_+6, b_+7); alu_add(gb, E);
    CYC(b_+7, b_+9); alu_and(gb, 0x07);
    CYC(b_+9, b_+10); D = A;
    CALL_C(b_+10, getTreeWarpDataIndex_hook, SYM(getTreeWarpDataIndex), b_+13);
    CYC(b_+13, b_+14); A = mem_rd(gb, HL);
    CYC(b_+14, b_+15); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+15, b_+17); continue; }
    CYC(b_+15, b_+17);
    CALL_C(b_+17, mapMenu_checkRoomVisited_hook, SYM(mapMenu_checkRoomVisited), b_+20);
    if (F & FZ) { CYCT(b_+20, b_+22); continue; }
    CYC(b_+20, b_+22);
    break;
  }
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+26); W8(wMapMenu_cursorIndex) = A;
  CYC(b_+26, b_+29); SET_HL(wMapMenu_warpIndex);
  CYC(b_+29, b_+30); A = D;
  CYC(b_+30, b_+31); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+33); ret_effect(gb);
}

void runMapMenu_hook(GB *gb) {
  BASE(runMapMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CYC(b_+3, b_+6); A = W8(wMenuActiveState);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(mapMenu_state0) && hook_enabled_at(gb, SYM(mapMenu_state0))) { mapMenu_state0_hook(gb); return; }
    else if (jt_ == SYM(mapMenu_state1) && hook_enabled_at(gb, SYM(mapMenu_state1))) { mapMenu_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void mapMenu_state0_hook(GB *gb) {
  BASE(mapMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+4, loadMinimapDisplayRoom_hook, SYM(loadMinimapDisplayRoom), b_+7);
  CYC(b_+7, b_+10); A = W8(wMapMenu_mode);
  CYC(b_+10, b_+12); alu_add(gb, 0x0d);
  CALL_C(b_+12, loadGfxHeader_hook, SYM(loadGfxHeader), b_+15);
  CYC(b_+15, b_+18); A = W8(wMapMenu_mode);
  CYC(b_+18, b_+20); alu_add(gb, 0x07);
  CALL_C(b_+20, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+23);
  CYC(b_+23, b_+26); A = W8(wMapMenu_mode);
  CYC(b_+26, b_+28); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+28, b_+30); goto dungeon; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto past; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); A = W8(wAnimalCompanion);
  CYC(b_+36, b_+38); alu_sub(gb, 0x0c);
  if (!(F & FC)) CALL_C_CC(b_+38, mapMenu_performTileSubstitutions_hook, SYM(mapMenu_performTileSubstitutions), b_+41);
  else CYC(b_+38, b_+41);
  CYC(b_+41, b_+44); A = mem_rd(gb, wGroup0RoomFlags + 0x13);
  CYC(b_+44, b_+45); alu_rrca(gb);
  CYC(b_+45, b_+47); A = 0x05;
  if (F & FC) CALL_C_CC(b_+47, mapMenu_performTileSubstitutions_hook, SYM(mapMenu_performTileSubstitutions), b_+50);
  else CYC(b_+47, b_+50);
past:
  CYC(b_+50, b_+53); A = mem_rd(gb, wGroup1RoomFlags + 0x41);
  CYC(b_+53, b_+54); alu_rrca(gb);
  CYC(b_+54, b_+56); A = 0x06;
  if (F & FC) CALL_C_CC(b_+56, mapMenu_performTileSubstitutions_hook, SYM(mapMenu_performTileSubstitutions), b_+59);
  else CYC(b_+56, b_+59);
  CALL_C(b_+59, mapMenu_clearUnvisitedTiles_hook, SYM(mapMenu_clearUnvisitedTiles), b_+62);
  CYC(b_+62, b_+65); A = W8(wMapMenu_currentRoom);
  CYC(b_+65, b_+68); W8(wMapMenu_cursorIndex) = A;
  CALL_C(b_+68, mapMenu_loadPopupData_hook, SYM(mapMenu_loadPopupData), b_+71);
  CYC(b_+71, b_+73);
  goto common;
dungeon:
  CYC(b_+73, b_+76); A = W8(wTilesetFlags);
  CYC(b_+76, b_+78); alu_and(gb, 0x20);
  CYC(b_+78, b_+81); A = W8(wMinimapDungeonFloor);
  if (!(F & FZ)) CYCT(b_+81, b_+83);
  else {
    CYC(b_+81, b_+83);
    CYC(b_+83, b_+86); A = W8(wDungeonFloor);
  }
  CYC(b_+86, b_+87); B = A;
  CYC(b_+87, b_+90); A = W8(wDungeonNumFloors);
  CYC(b_+90, b_+91); A = alu_dec8(gb, A);
  CYC(b_+91, b_+92); alu_sub(gb, B);
  CYC(b_+92, b_+95); W8(wMapMenu_floorIndex) = A;
  CALL_C(b_+95, multiplyABy8_hook, SYM(multiplyABy8), b_+98);
  CYC(b_+98, b_+101); A = W8(wMapMenu_floorIndex);
  CYC(b_+101, b_+102); alu_add(gb, A);
  CYC(b_+102, b_+103); alu_add(gb, C);
  CYC(b_+103, b_+106); W8(wMapMenu_dungeonScrollY) = A;
  CALL_C(b_+106, dungeonMap_calculateVisitedFloorsAndLinkPosition_hook, SYM(dungeonMap_calculateVisitedFloorsAndLinkPosition), b_+109);
  CYC(b_+109, b_+112); A = W8(wDungeonIndex);
  CYC(b_+112, b_+114); alu_add(gb, 0x10);
  CALL_C(b_+114, loadGfxHeader_hook, SYM(loadGfxHeader), b_+117);
  CALL_C(b_+117, dungeonMap_drawSmallKeyCount_hook, SYM(dungeonMap_drawSmallKeyCount), b_+120);
  CALL_C(b_+120, dungeonMap_generateScrollableTilemap_hook, SYM(dungeonMap_generateScrollableTilemap), b_+123);
  CALL_C(b_+123, dungeonMap_drawFloorList_hook, SYM(dungeonMap_drawFloorList), b_+126);
  CALL_C(b_+126, dungeonMap_updateScroll_hook, SYM(dungeonMap_updateScroll), b_+129);
common:
  CYC(b_+129, b_+130); alu_xor(gb, A);
  CYC(b_+130, b_+132); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+132, mapMenu_drawSprites_hook, SYM(mapMenu_drawSprites), b_+135);
  CYC(b_+135, b_+136); alu_xor(gb, A);
  CYC(b_+136, b_+138); H8(hCameraX) = A;
  CYC(b_+138, b_+140); H8(hCameraY) = A;
  CYC(b_+140, b_+143); W8(wScreenOffsetX) = A;
  CYC(b_+143, b_+146); W8(wScreenOffsetY) = A;
  CYC(b_+146, b_+149); SET_HL(wMenuActiveState);
  CYC(b_+149, b_+150); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+150, mapMenu_copyTilemapToVram_hook, SYM(mapMenu_copyTilemapToVram), b_+153);
  CALL_C(b_+153, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+156);
  CYC(b_+156, b_+158); A = 0x07;
  CYC(b_+158, b_+161);
  TAIL(loadGfxRegisterStateIndex);
}

void loadMinimapDisplayRoom_hook(GB *gb) {
  BASE(loadMinimapDisplayRoom);
  CYC(b_+0, b_+3); SET_HL(wMinimapGroup);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); B = 0x02;
  CYC(b_+8, b_+11); A = W8(wTilesetFlags);
  CYC(b_+11, b_+13); alu_bit(gb, 4, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto overworld; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto set_room; }
  CYC(b_+17, b_+19);
overworld:
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+21); alu_rlca(gb);
  CYC(b_+21, b_+23); alu_and(gb, 0x01);
  CYC(b_+23, b_+25); alu_bit(gb, 1, B);
  CYC(b_+25, b_+26); B = A;
  if (F & FZ) { CYCT(b_+26, b_+28); goto set_room; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); C = 0x38;
set_room:
  CYC(b_+30, b_+31); A = C;
  CYC(b_+31, b_+34); W8(wMapMenu_currentRoom) = A;
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+38); W8(wMapMenu_mode) = A;
  CYC(b_+38, b_+39); ret_effect(gb);
}

void dungeonMap_drawSmallKeyCount_hook(GB *gb) {
  BASE(dungeonMap_drawSmallKeyCount);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getNumSmallKeys_hook, SYM(getNumSmallKeys), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(w4TileMap + 0x226);
  CYC(b_+7, b_+9); alu_add(gb, 0x90);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+10, b_+12); A = 0x9a;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void dungeonMap_calculateVisitedFloorsAndLinkPosition_hook(GB *gb) {
  BASE(dungeonMap_calculateVisitedFloorsAndLinkPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+6); SET_HL(wDungeonVisitedFloors);
  CYC(b_+6, b_+7); push_effect(gb, b_+7); add_a_to_hl(gb);
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CALL_C(b_+8, checkLinkHasCompass_hook, SYM(checkLinkHasCompass), b_+11);
  CYC(b_+11, b_+12); A = B;
  if (F & FZ) CYCT(b_+12, b_+14);
  else {
    CYC(b_+12, b_+14);
    CYC(b_+14, b_+17); A = mem_rd(gb, wMapFloorsUnlockedWithCompass);
    CYC(b_+17, b_+18); alu_or(gb, B);
  }
  CYC(b_+18, b_+21); W8(wMapMenu_visitedFloors) = A;
  CYC(b_+21, b_+24); A = W8(wMinimapDungeonMapPosition);
  CYC(b_+24, b_+27); W8(wMapMenu_dungeonCursorIndex) = A;
  CYC(b_+27, b_+30); A = W8(wMinimapDungeonFloor);
  CYC(b_+30, b_+33); W8(wMapMenu_linkFloor) = A;
  CYC(b_+33, b_+36); A = W8(wActiveGroup);
  CYC(b_+36, b_+38); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+38, b_+39); ret_effect(gb); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+42); A = W8(wActiveRoom);
  CYC(b_+42, b_+44); alu_cp(gb, GV(0xf5, 0x9e));
  if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); A = 0x13;
  CYC(b_+47, b_+50); W8(wMapMenu_dungeonCursorIndex) = A;
  CYC(b_+50, b_+51); ret_effect(gb);
}

void mapMenu_state1_hook(GB *gb) {
  BASE(mapMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+4, mapMenu_state1__checkInput_hook, b_+10, b_+7);
  else CYC(b_+4, b_+7);
  CYC(b_+7, b_+10);
  TAIL(mapMenu_drawSprites);
}

void mapMenu_state1__checkInput_hook(GB *gb) {
  BASE(mapMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+10, b_+13); A = W8(wMapMenu_mode);
  CYC(b_+13, b_+15); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto overworld; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); A = W8(wKeysJustPressed);
  CYC(b_+20, b_+22); alu_and(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+25);
    TAIL(closeMenu);
  }
  CYC(b_+22, b_+25);
  CALL_C(b_+25, dungeonMap_updateCursorFlickerCounter_hook, SYM(dungeonMap_updateCursorFlickerCounter), b_+28);
  CYC(b_+28, b_+31);
  TAIL(dungeonMap_checkDirectionButtons);
overworld:
  CYC(b_+31, b_+34); A = W8(wMapMenu_varcbb4);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (F & FZ) CYCT(b_+35, b_+37);
  else {
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+38); A = alu_dec8(gb, A);
    CYC(b_+38, b_+41); W8(wMapMenu_varcbb4) = A;
  }
  CALL_C(b_+41, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+44);
  CYC(b_+44, b_+47); SET_HL(b_+124);
  CALL_C(b_+47, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+50);
  if (!(F & FC)) { CYCT(b_+50, b_+52); goto no_direction; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); C = A;
  CYC(b_+53, b_+56); D = 0xe0; E = 0x0e;
  CYC(b_+56, b_+59); A = W8(wMapMenu_cursorIndex);
  CYC(b_+59, b_+60); L = A;
  CYC(b_+60, b_+62); alu_and(gb, 0xf0);
  CYC(b_+62, b_+63); H = A;
  CYC(b_+63, b_+64); A = L;
  CYC(b_+64, b_+65); alu_xor(gb, H);
  CYC(b_+65, b_+66); L = A;
  CYC(b_+66, b_+68); C = alu_sra(gb, C);
  if (F & FC) { CYCT(b_+68, b_+70); goto vertical; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); A = L;
  for (;;) {
    CYC(b_+71, b_+72); alu_add(gb, C);
    CYC(b_+72, b_+74); alu_and(gb, 0x0f);
    CYC(b_+74, b_+75); alu_cp(gb, E);
    if (!(F & FC)) { CYCT(b_+75, b_+77); continue; }
    CYC(b_+75, b_+77);
    break;
  }
  CYC(b_+77, b_+78); L = A;
  CYC(b_+78, b_+80);
  goto set_cursor;
vertical:
  CYC(b_+80, b_+81); A = H;
  for (;;) {
    CYC(b_+81, b_+82); alu_add(gb, C);
    CYC(b_+82, b_+84); alu_and(gb, 0xf0);
    CYC(b_+84, b_+85); alu_cp(gb, D);
    if (!(F & FC)) { CYCT(b_+85, b_+87); continue; }
    CYC(b_+85, b_+87);
    break;
  }
  CYC(b_+87, b_+88); H = A;
set_cursor:
  CYC(b_+88, b_+89); A = H;
  CYC(b_+89, b_+90); alu_or(gb, L);
  CYC(b_+90, b_+93); W8(wMapMenu_cursorIndex) = A;
  CYC(b_+93, b_+95); A = 0x84;
  CALL_C(b_+95, playSound_b00_hook, SYM(playSound_b00), b_+98);
  CYC(b_+98, b_+101);
  TAIL(mapMenu_loadPopupData);
no_direction:
  CYC(b_+101, b_+104); A = W8(wKeysJustPressed);
  CYC(b_+104, b_+106); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto show_room_text; }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); alu_and(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+110, b_+113);
    TAIL(closeMenu);
  }
  CYC(b_+110, b_+113);
  CYC(b_+113, b_+114); ret_effect(gb);
  return;
show_room_text:
  CALL_C(b_+114, mapGetRoomTextOrReturn_hook, SYM(mapGetRoomTextOrReturn), b_+117);
  CYC(b_+117, b_+120); SET_HL(wSubmenuState);
  CYC(b_+120, b_+121); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+121, b_+124);
  TAIL(showText);
}

void mapGetRoomTextOrReturn_hook(GB *gb) {
  BASE(mapGetRoomTextOrReturn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, mapMenu_checkCursorRoomVisited_hook, SYM(mapMenu_checkCursorRoomVisited), b_+3);
  if (!(F & FZ)) CYCT(b_+3, b_+5);
  else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+6); SET_AF(pop_effect(gb));
    CYC(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+9); C = 0x80;
  CYC(b_+9, b_+12); A = W8(wMapMenu_cursorIndex);
  CYC(b_+12, b_+13); alu_cp(gb, C);
  CYC(b_+13, b_+15); A = 0x03;
  if (F & FC) CYCT(b_+15, b_+17);
  else {
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+18); alu_xor(gb, A);
  }
  CYC(b_+18, b_+21); W8(wTextboxPosition) = A;
  CYC(b_+21, b_+23); A = 0x09;
  CYC(b_+23, b_+26); W8(wTextboxFlags) = A;
  TAIL(mapGetRoomText);
}

void mapGetRoomText_hook(GB *gb) {
  BASE(mapGetRoomText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, mapGetRoomIndexWithoutUnusedColumns_hook, SYM(mapGetRoomIndexWithoutUnusedColumns), b_+3);
  CYC(b_+3, b_+6); SET_HL(SYM(presentMapTextIndices));
  if (!(F & FC)) CYCT(b_+6, b_+8);
  else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+11); SET_HL(SYM(pastMapTextIndices));
  }
  CYC(b_+11, b_+13); B = 0x03;
  CYC(b_+13, b_+14); push_effect(gb, b_+14); add_a_to_hl(gb);
  CYC(b_+14, b_+15); C = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_bit(gb, 7, C);
  if (F & FZ) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); A = C;
  CYC(b_+19, b_+21); alu_and(gb, 0x07);
  CYC(b_+21, b_+22); push_effect(gb, b_+22);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+32) { mapGetRoomText__specialCode0_hook(gb); return; }
    else if (jt_ == b_+61) { mapGetRoomText__specialCode1_hook(gb); return; }
    else if (jt_ == b_+81) { mapGetRoomText__specialCode2_hook(gb); return; }
    else if (jt_ == b_+89) { mapGetRoomText__specialCode3_hook(gb); return; }
    else if (jt_ == b_+98) { mapGetRoomText__specialCode4_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void mapGetRoomText__specialCode0_hook(GB *gb) {
  BASE(mapGetRoomText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+32, b_+33); push_effect(gb, DE);
  CYC(b_+33, b_+36); A = W8(wTilesetFlags);
  CYC(b_+36, b_+37); alu_rlca(gb);
  CYC(b_+37, b_+40); SET_DE(wMakuMapTextPresent);
  CYC(b_+40, b_+42); C = 0x23;
  CYC(b_+42, b_+44); A = 0x3e;
  if (!(F & FC)) CYCT(b_+44, b_+46);
  else {
    CYC(b_+44, b_+46);
    CYC(b_+46, b_+47); E = alu_inc8(gb, E);
    CYC(b_+47, b_+48); C = alu_inc8(gb, C);
    CYC(b_+48, b_+49); A = alu_inc8(gb, A);
  }
  CALL_C(b_+49, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+52);
  CYC(b_+52, b_+53); L = E;
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+55); SET_DE(pop_effect(gb));
  if (F & FZ) { CYCT(b_+55, b_+56); ret_effect(gb); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+58); C = A;
  CYC(b_+58, b_+60); B = 0x05;
  CYC(b_+60, b_+61); ret_effect(gb);
}

void mapGetRoomText__specialCode1_hook(GB *gb) {
  BASE(mapGetRoomText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+61, b_+62); A = C;
  CYC(b_+62, b_+63); alu_add(gb, A);
  CYC(b_+63, b_+65); A = alu_swap(gb, A);
  CYC(b_+65, b_+67); alu_and(gb, 0x0f);
  CYC(b_+67, b_+68); C = A;
  CALL_C(b_+68, mapGetRoomText__checkDungeonEntered_hook, b_+106, b_+71);
  if (!(F & FZ)) {
    CYCT(b_+71, b_+73);
    CYC(b_+78, b_+80); B = 0x02;
    CYC(b_+80, b_+81); ret_effect(gb);
    return;
  }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+76); alu_and(gb, 0x7f);
  CYC(b_+76, b_+77); C = A;
  CYC(b_+77, b_+78); ret_effect(gb);
}

void mapGetRoomText__specialCode2_hook(GB *gb) {
  BASE(mapGetRoomText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+81, checkMoblinsKeepDestroyed_hook, SYM(checkMoblinsKeepDestroyed), b_+84);
  CYC(b_+84, b_+86); C = GV(0x17, 0x0b);
  if (!(F & FZ)) { CYCT(b_+86, b_+87); ret_effect(gb); return; }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+88); C = alu_inc8(gb, C);
  CYC(b_+88, b_+89); ret_effect(gb);
}

void mapGetRoomText__specialCode3_hook(GB *gb) {
  BASE(mapGetRoomText);
  CYC(b_+89, b_+92); A = W8(wAnimalCompanion);
  CYC(b_+92, b_+94); alu_sub(gb, 0x0b);
  CYC(b_+94, b_+96); alu_add(gb, GV(0x2d, 0x08));
  CYC(b_+96, b_+97); C = A;
  CYC(b_+97, b_+98); ret_effect(gb);
}

void mapGetRoomText__specialCode4_hook(GB *gb) {
  BASE(mapGetRoomText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+98, checkAdvanceShopVisited_hook, SYM(checkAdvanceShopVisited), b_+101);
  CYC(b_+101, b_+103); C = 0x26;
  if (F & FZ) { CYCT(b_+103, b_+104); ret_effect(gb); return; }
  CYC(b_+103, b_+104);
  CYC(b_+104, b_+105); C = alu_dec8(gb, C);
  CYC(b_+105, b_+106); ret_effect(gb);
}

void mapGetRoomText__checkDungeonEntered_hook(GB *gb) {
  BASE(mapGetRoomText);
  CYC(b_+106, b_+107); push_effect(gb, DE);
  CYC(b_+107, b_+110); SET_HL(SYM(mapMenu_dungeonEntranceText));
  CYC(b_+110, b_+111); add_double_index_to_hl(gb, b_+111);
  CYC(b_+111, b_+112); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+112, b_+113); E = A;
  CYC(b_+113, b_+115); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+115, b_+117); D = 0xc9;
  if (!(F & FZ)) CYCT(b_+117, b_+119);
  else {
    CYC(b_+117, b_+119);
    CYC(b_+119, b_+120); D = alu_inc8(gb, D);
  }
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+123); alu_bit(gb, 4, A);
  CYC(b_+123, b_+124); SET_DE(pop_effect(gb));
  CYC(b_+124, b_+125); ret_effect(gb);
}

void getFileDisplayVariableAddress_hook(GB *gb) {
  BASE(getFileDisplayVariableAddress);
  CYC(b_+0, b_+1); E = A;
  TAIL(getFileDisplayVariableAddress_paramE);
}

void getFileDisplayVariableAddress_paramE_hook(GB *gb) {
  BASE(getFileDisplayVariableAddress_paramE);
  CYC(b_+0, b_+1); A = E;
  CYC(b_+1, b_+3); A = alu_swap(gb, A);
  CYC(b_+3, b_+4); alu_rrca(gb);
  CYC(b_+4, b_+5); alu_add(gb, D);
  CYC(b_+5, b_+8); SET_HL(w4FileDisplayVariables);
  CYC(b_+8, b_+9); push_effect(gb, b_+9); add_a_to_hl(gb);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void fileSelectUpdateInput_hook(GB *gb) {
  BASE(fileSelectUpdateInput);
  CYC(b_+0, b_+3); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); SET_HL(wFileSelect_cursorPos);
  CYC(b_+7, b_+9); A = 0xff;
  CYC(b_+9, b_+11); alu_bit(gb, 6, C);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); fileSelectUpdateInput__upOrDown_hook(gb); return; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+17); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); fileSelectUpdateInput__upOrDown_hook(gb); return; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); A = C;
  CYC(b_+20, b_+22); alu_and(gb, 0x09);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+24); ret_effect(gb);
}

void fileSelectUpdateInput__upOrDown_hook(GB *gb) {
  BASE(fileSelectUpdateInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+26); push_effect(gb, BC);
  CYC(b_+26, b_+27); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+27, b_+29); alu_and(gb, 0x03);
  CALL_C(b_+29, fileSelectSetCursor_hook, SYM(fileSelectSetCursor), b_+32);
  CALL_C(b_+32, fileSelectDrawHeartsAndDeathCounter_hook, SYM(fileSelectDrawHeartsAndDeathCounter), b_+35);
  CYC(b_+35, b_+36); SET_BC(pop_effect(gb));
  CYC(b_+36, b_+37); alu_xor(gb, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void func_02_448d_hook(GB *gb) {
  BASE(func_02_448d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); SET_HL(wFileSelect_cursorPos2);
  CYC(b_+7, b_+9); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+12); alu_bit(gb, 5, C);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto moved; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);
  CYC(b_+15, b_+17); alu_bit(gb, 4, C);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto moved; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); A = C;
  CYC(b_+20, b_+22); alu_and(gb, 0x09);
  CYC(b_+22, b_+23); ret_effect(gb);
  return;
moved:
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) CALL_C_CC(b_+24, fileSelectSetCursor_hook, SYM(fileSelectSetCursor), b_+27);
  else CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void fileSelectSetCursor_hook(GB *gb) {
  BASE(fileSelectSetCursor);
  CYC(b_+0, b_+1); mem_wr(gb, HL, A);
  CYC(b_+1, b_+3); A = 0x84;
  CYC(b_+3, b_+6); TAIL(playSound_b00);
}

void fileSelectMode2_hook(GB *gb) {
  BASE(fileSelectMode2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode2__func_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); TAIL(drawNameInputCursors);
}

void fileSelectMode2__func_hook(GB *gb) {
  BASE(fileSelectMode2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); A = W8(wFileSelect_mode2);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+16) { fileSelectMode2__mode0_hook(gb); return; }
    else if (jt_ == b_+26) { fileSelectMode2__mode2_hook(gb); return; }
    else if (jt_ == SYM(runTextInput)) { HANDOFF(HL); }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode2__mode0_hook(GB *gb) {
  BASE(fileSelectMode2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+16, eraseFile_b00_hook, SYM(eraseFile_b00), b_+19);
  CALL_C(b_+19, loadFile_b00_hook, SYM(loadFile_b00), b_+22);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+26); TAIL(copyNameToW4NameBuffer);
}

void fileSelectMode2__mode2_hook(GB *gb) {
  BASE(fileSelectMode2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+26, getNameBufferLength_hook, SYM(getNameBufferLength), b_+29);
  if (F & FZ) CYCT(b_+29, b_+31);
  else {
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+34); SET_HL(w4NameBuffer);
    CYC(b_+34, b_+37); SET_DE(wLinkName);
    CYC(b_+37, b_+39); B = 0x06;
    CALL_C(b_+39, copyMemory_hook, SYM(copyMemory), b_+42);
    CALL_C(b_+42, initializeFile_b00_hook, SYM(initializeFile_b00), b_+45);
  }
  CYC(b_+45, b_+48); TAIL(setFileSelectModeTo1);
}

void runKidNameEntryMenu_hook(GB *gb) {
  BASE(runKidNameEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelect_redrawDecorationsAndSetWramBank4_hook, SYM(fileSelect_redrawDecorationsAndSetWramBank4), b_+3);
  CALL_C(b_+3, runKidNameEntryMenu__func_hook, b_+9, b_+6);
  CYC(b_+6, b_+9); TAIL(drawNameInputCursors);
}

void runKidNameEntryMenu__func_hook(GB *gb) {
  BASE(runKidNameEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); A = W8(wFileSelect_mode2);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+19) { runKidNameEntryMenu__mode0_hook(gb); return; }
    else if (jt_ == b_+32) { runKidNameEntryMenu__mode1_hook(gb); return; }
    else if (jt_ == b_+40) { runKidNameEntryMenu__mode2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void runKidNameEntryMenu__mode0_hook(GB *gb) {
  BASE(runKidNameEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+19, b_+21); A = 0xa0;
  CALL_C(b_+21, loadGfxHeader_hook, SYM(loadGfxHeader), b_+24);
  CYC(b_+24, b_+26); A = 0x01;
  CALL_C(b_+26, copyNameToW4NameBuffer_hook, SYM(copyNameToW4NameBuffer), b_+29);
  CYC(b_+29, b_+32); TAIL(fadeinFromWhite);
}

void runKidNameEntryMenu__mode1_hook(GB *gb) {
  BASE(runKidNameEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+32, b_+35); A = W8(wPaletteThread_mode);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+40); HANDOFF(SYM(runTextInput));
}

void runKidNameEntryMenu__mode2_hook(GB *gb) {
  BASE(runKidNameEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+40, getNameBufferLength_hook, SYM(getNameBufferLength), b_+43);
  CYC(b_+43, b_+45); A = 0x01;
  if (F & FZ) CYCT(b_+45, b_+47);
  else {
    CYC(b_+45, b_+47);
    CYC(b_+47, b_+50); SET_HL(w4NameBuffer);
    CYC(b_+50, b_+53); SET_DE(wKidName);
    CYC(b_+53, b_+55); B = 0x06;
    CALL_C(b_+55, copyMemory_hook, SYM(copyMemory), b_+58);
    CYC(b_+58, b_+60); A = 0x56;
    CALL_C(b_+60, playSound_b00_hook, SYM(playSound_b00), b_+63);
    CYC(b_+63, b_+64); alu_xor(gb, A);
  }
  CYC(b_+64, b_+67); W8(wTextInputResult) = A;
  CYC(b_+67, b_+70); TAIL(closeMenu);
}

void runSecretEntryMenu_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelect_redrawDecorationsAndSetWramBank4_hook, SYM(fileSelect_redrawDecorationsAndSetWramBank4), b_+3);
  CALL_C(b_+3, runSecretEntryMenu__func_hook, b_+9, b_+6);
  CYC(b_+6, b_+9); TAIL(drawSecretInputCursors);
}

void runSecretEntryMenu__func_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); A = W8(wFileSelect_mode2);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+23) { runSecretEntryMenu__mode0_hook(gb); return; }
    else if (jt_ == b_+34) { runSecretEntryMenu__mode1_hook(gb); return; }
    else if (jt_ == b_+42) { runSecretEntryMenu__mode2_hook(gb); return; }
    else if (jt_ == SYM(closeMenu) && hook_enabled_at(gb, SYM(closeMenu))) { closeMenu_hook(gb); return; }
    else if (jt_ == SYM(textInput_waitForInput)) { HANDOFF(HL); }
    else { HANDOFF(HL); }
  } while (0);
}

void runSecretEntryMenu__mode0_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+23, b_+25); A = 0xa0;
  CALL_C(b_+25, loadGfxHeader_hook, SYM(loadGfxHeader), b_+28);
  CALL_C(b_+28, func_02_465c_hook, SYM(func_02_465c), b_+31);
  CYC(b_+31, b_+34); TAIL(fadeinFromWhite);
}

void runSecretEntryMenu__mode1_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+34, b_+37); A = W8(wPaletteThread_mode);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+39); ret_effect(gb); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+42); HANDOFF(SYM(runTextInput));
}

void runSecretEntryMenu__mode2_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+42, b_+45); SET_HL(w4SecretBuffer);
  CYC(b_+45, b_+48); SET_DE(wTmpcec0);
  CYC(b_+48, b_+50); B = 0x20;
  CALL_C(b_+50, copyMemory_hook, SYM(copyMemory), b_+53);
  CYC(b_+53, b_+56); SET_BC(0x0103);
  CYC(b_+56, b_+59); A = W8(wSecretInputType);
  CYC(b_+59, b_+60); alu_rlca(gb);
  if (F & FC) { CYCT(b_+60, b_+62); goto unpack_secret; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); C = 0x02;
unpack_secret:
  CALL_C(b_+64, secretFunctionCaller_hook, SYM(secretFunctionCaller), b_+67);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); runSecretEntryMenu__invalidSecret_hook(gb); return; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); B = 0x02;
  CALL_C(b_+71, secretFunctionCaller_hook, SYM(secretFunctionCaller), b_+74);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); runSecretEntryMenu__invalidSecret_hook(gb); return; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+79); A = mem_rd(gb, wTmpcec0 + 4);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+83); A = W8(wSecretInputType);
  CYC(b_+83, b_+85); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto check_short_secret; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); alu_xor(gb, A);
  CYC(b_+88, b_+91); W8(wSecretInputType) = A;
  CYC(b_+91, b_+92); A = B;
  CYC(b_+92, b_+94); runSecretEntryMenu__setTextInputResult_hook(gb);
  return;
check_short_secret:
  CYC(b_+94, b_+96); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+96, b_+98); runSecretEntryMenu__loadRingSecretData_hook(gb); return; }
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+100); alu_and(gb, 0x3f);
  CYC(b_+100, b_+101); alu_sub(gb, B);
  if (F & FZ) { CYCT(b_+101, b_+103); runSecretEntryMenu__setTextInputResult_hook(gb); return; }
  CYC(b_+101, b_+103);
  TAIL(runSecretEntryMenu__invalidSecret);
}

void runSecretEntryMenu__invalidSecret_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  CYC(b_+103, b_+105); A = 0x01;
  TAIL(runSecretEntryMenu__setTextInputResult);
}

void runSecretEntryMenu__setTextInputResult_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+105, b_+108); W8(wTextInputResult) = A;
  if (!(F & FZ)) { CYCT(b_+108, b_+110); fileSelect_printError_hook(gb); return; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); A = 0x4d;
  CALL_C(b_+112, playSound_b00_hook, SYM(playSound_b00), b_+115);
  CYC(b_+115, b_+118); TAIL(closeMenu);
}

void runSecretEntryMenu__loadRingSecretData_hook(GB *gb) {
  BASE(runSecretEntryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+118, b_+121); SET_BC(0x0402);
  CALL_C(b_+121, secretFunctionCaller_hook, SYM(secretFunctionCaller), b_+124);
  CYC(b_+124, b_+125); alu_xor(gb, A);
  CYC(b_+125, b_+127); TAIL(runSecretEntryMenu__setTextInputResult);
}

void fileSelectMode4_hook(GB *gb) {
  BASE(fileSelectMode4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode4__mode4Update_hook, b_+9, b_+3);
  CALL_C(b_+3, fileSelectDrawAcornCursor_hook, SYM(fileSelectDrawAcornCursor), b_+6);
  CYC(b_+6, b_+9); TAIL(fileSelectDrawLink);
}

void fileSelectMode4__mode4Update_hook(GB *gb) {
  BASE(fileSelectMode4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); A = W8(wFileSelect_mode2);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+21) { fileSelectMode4__mode0_hook(gb); return; }
    else if (jt_ == b_+54) { fileSelectMode4__mode1_hook(gb); return; }
    else if (jt_ == b_+76) { fileSelectMode4__mode2_hook(gb); return; }
    else if (jt_ == b_+106) { fileSelectMode4__mode3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode4__mode0_hook(GB *gb) {
  BASE(fileSelectMode4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+21, setFileSelectCursorOffsetToFileSelectMode_hook, SYM(setFileSelectCursorOffsetToFileSelectMode), b_+24);
  CYC(b_+24, b_+26); A = 0x03;
  CALL_C(b_+26, func_02_4149_hook, SYM(func_02_4149), b_+29);
  CALL_C(b_+29, disableLcd_hook, SYM(disableLcd), b_+32);
  CYC(b_+32, b_+34); A = 0xa4;
  CALL_C(b_+34, loadGfxHeader_hook, SYM(loadGfxHeader), b_+37);
  CYC(b_+37, b_+39); A = 0x06;
  CALL_C(b_+39, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+42);
  CALL_C(b_+42, loadFileDisplayVariables_hook, SYM(loadFileDisplayVariables), b_+45);
  CALL_C(b_+45, textInput_updateEntryCursor_hook, SYM(textInput_updateEntryCursor), b_+48);
  CALL_C(b_+48, fileSelectDrawHeartsAndDeathCounter_hook, SYM(fileSelectDrawHeartsAndDeathCounter), b_+51);
  CYC(b_+51, b_+54); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode4__mode1_hook(GB *gb) {
  BASE(fileSelectMode4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+54, fileSelectUpdateInput_hook, SYM(fileSelectUpdateInput), b_+57);
  if (F & FZ) { CYCT(b_+57, b_+58); ret_effect(gb); return; }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); A = 0x56;
  CALL_C(b_+60, playSound_b00_hook, SYM(playSound_b00), b_+63);
  CYC(b_+63, b_+66); A = W8(wFileSelect_cursorPos);
  CYC(b_+66, b_+68); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+68, b_+71); setFileSelectModeTo1_hook(gb); return; }
  CYC(b_+68, b_+71);
  CYC(b_+71, b_+73); H8(hActiveFileSlot) = A;
  CYC(b_+73, b_+76); TAIL(incFileSelectMode2);
}

void fileSelectMode4__mode2_hook(GB *gb) {
  BASE(fileSelectMode4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+76, decFileSelectMode2IfBPressed_hook, SYM(decFileSelectMode2IfBPressed), b_+79);
  if (!(F & FZ)) { CYCT(b_+79, b_+81); goto selected; }
  CYC(b_+79, b_+81);
  CALL_C(b_+81, func_02_448d_hook, SYM(func_02_448d), b_+84);
  if (F & FZ) { CYCT(b_+84, b_+85); ret_effect(gb); return; }
  CYC(b_+84, b_+85);
  CYC(b_+85, b_+88); A = W8(wFileSelect_cursorPos2);
  CYC(b_+88, b_+89); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+89, b_+92); setFileSelectModeTo1_hook(gb); return; }
  CYC(b_+89, b_+92);
  CYC(b_+92, b_+95); incFileSelectMode2_hook(gb);
  return;

selected:
  CYC(b_+95, b_+97); A = 0x50;
  CALL_C(b_+97, playSound_b00_hook, SYM(playSound_b00), b_+100);
  CYC(b_+100, b_+103); A = W8(wFileSelect_cursorPos);
  CYC(b_+103, b_+106); TAIL(func_02_4149);
}

void fileSelectMode4__mode3_hook(GB *gb) {
  BASE(fileSelectMode4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+106, b_+109); SET_HL(wFileSelect_linkTimer);
  CYC(b_+109, b_+110); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+110, b_+112); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+112, b_+113); ret_effect(gb); return; }
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+115); A = H8(hActiveFileSlot);
  CYC(b_+115, b_+117); D = 0x02;
  CALL_C(b_+117, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+120);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL);
  CYC(b_+121, b_+122); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+122, b_+124); goto erase; }
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+125); A = alu_dec8(gb, A);
  CYC(b_+125, b_+126); mem_wr(gb, HL, A);
  CYC(b_+126, b_+128); alu_and(gb, 0x03);
  CYC(b_+128, b_+130); A = 0x57;
  if (F & FZ) CALL_C_CC(b_+130, playSound_b00_hook, SYM(playSound_b00), b_+133);
  else CYC(b_+130, b_+133);
  CYC(b_+133, b_+136); fileSelectDrawHeartsAndDeathCounter_hook(gb);
  return;

erase:
  CALL_C(b_+136, eraseFile_b00_hook, SYM(eraseFile_b00), b_+139);
  CYC(b_+139, b_+142); TAIL(setFileSelectModeTo1);
}

void func_02_461c_hook(GB *gb) {
  BASE(func_02_461c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xac;
  CALL_C(b_+2, loadGfxHeader_hook, SYM(loadGfxHeader), b_+5);
  CYC(b_+5, b_+7); A = 0x08;
  CYC(b_+7, b_+10); TAIL(loadUncompressedGfxHeader);
}

void getNameBufferLength_hook(GB *gb) {
  BASE(getNameBufferLength);
  CYC(b_+0, b_+3); SET_HL(w4NameBuffer);
  CYC(b_+3, b_+5); B = 0x05;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  do {
    CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FZ)) CYCT(b_+7, b_+9);
    else { CYC(b_+7, b_+9); CYC(b_+9, b_+11); mem_wr(gb, HL, 0x20); }
    CYC(b_+11, b_+12); L = alu_inc8(gb, L);
    CYC(b_+12, b_+13); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+13, b_+15); else CYC(b_+13, b_+15);
  } while (!(F & FZ));
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(b_+16, b_+18); B = 0x05;
  for (;;) {
    CYC(b_+18, b_+19); A = mem_rd(gb, HL);
    CYC(b_+19, b_+21); alu_sub(gb, 0x20);
    if (!(F & FZ)) { CYCT(b_+21, b_+22); ret_effect(gb); return; }
    CYC(b_+21, b_+22);
    CYC(b_+22, b_+23); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
    CYC(b_+23, b_+24); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+24, b_+26); continue; }
    CYC(b_+24, b_+26); RET(b_+26); return;
  }
}

void copyNameToW4NameBuffer_hook(GB *gb) {
  BASE(copyNameToW4NameBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); mem_wr(gb, wFileSelect_textInputMode, A);
  CYC(b_+3, b_+6); SET_DE(wLinkName);
  CYC(b_+6, b_+8); alu_cp(gb, 0x01);
  if (!(F & FZ)) CYCT(b_+8, b_+10);
  else { CYC(b_+8, b_+10); CYC(b_+10, b_+12); E = 0x09; }
  CYC(b_+12, b_+15); SET_HL(w4NameBuffer);
  CYC(b_+15, b_+17); B = 0x06;
  CALL_C(b_+17, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+20);
  CYC(b_+20, b_+22); A = 0x04;
  CYC(b_+22, b_+25); mem_wr(gb, wFileSelect_textInputMaxCursorPos, A);
  CYC(b_+25, b_+27); TAIL(label_02_038);
}

void func_02_465c_hook(GB *gb) {
  BASE(func_02_465c);
  CYC(b_+0, b_+3); A = W8(wSecretInputType);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto secret_input; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x0e81);
  CYC(b_+10, b_+12); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+12, b_+14); goto set_input_mode; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); SET_BC(0x1382);
  CYC(b_+17, b_+19);
  goto set_input_mode;
secret_input:
  CYC(b_+19, b_+21); A = 0xff;
  CYC(b_+21, b_+24); W8(wLastSecretInputLength) = A;
  CYC(b_+24, b_+27); SET_BC(0x0480);
set_input_mode:
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+31); W8(wFileSelect_textInputMaxCursorPos) = A;
  CYC(b_+31, b_+32); A = C;
  CYC(b_+32, b_+35); W8(wFileSelect_textInputMode) = A;
  TAIL(label_02_038);
}

void label_02_038_hook(GB *gb) {
  BASE(label_02_038);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb9);
  CYC(b_+3, b_+5); B = 0x0a;
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CALL_C(b_+8, textInput_loadCharacterGfx_hook, SYM(textInput_loadCharacterGfx), b_+11);
  CALL_C(b_+11, disableLcd_hook, SYM(disableLcd), b_+14);
  CYC(b_+14, b_+16); A = 0x0b;
  CALL_C(b_+16, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+19);
  CYC(b_+19, b_+21); A = 0x05;
  CALL_C(b_+21, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+24);
  CYC(b_+24, b_+27); A = W8(wFileSelect_textInputMode);
  CYC(b_+27, b_+28); alu_rlca(gb);
  if (F & FC) { CYCT(b_+28, b_+30); label_02_038__secretEntry_hook(gb); return; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0xa5;
  CALL_C(b_+32, loadGfxHeader_hook, SYM(loadGfxHeader), b_+35);
  CYC(b_+35, b_+38); A = W8(wFileSelect_textInputMode);
  CYC(b_+38, b_+39); alu_rrca(gb);
  if (F & FC) CYCT(b_+39, b_+41);
  else {
    CYC(b_+39, b_+41);
    CYC(b_+41, b_+43); A = H8(hActiveFileSlot);
    CYC(b_+43, b_+45); alu_add(gb, 0x20);
    CYC(b_+45, b_+48); mem_wr(gb, w4TileMap + 0x49, A);
  }
  CYC(b_+48, b_+50); A = 0x08;
  CALL_C(b_+50, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+53);
  CYC(b_+53, b_+55);
  TAIL(label_02_038__end);
}

void label_02_038__secretEntry_hook(GB *gb) {
  BASE(label_02_038);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+55, b_+58); A = W8(wFileSelect_textInputMaxCursorPos);
  CYC(b_+58, b_+61); SET_HL(wLastSecretInputLength);
  CYC(b_+61, b_+62); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+66); SET_HL(w4SecretBuffer);
  CYC(b_+66, b_+68); B = 0x20;
  CYC(b_+68, b_+70); A = 0x20;
  if (!(F & FZ)) CALL_C_CC(b_+70, fillMemory_hook, SYM(fillMemory), b_+73);
  else CYC(b_+70, b_+73);
  CYC(b_+73, b_+75); A = 0xaa;
  CALL_C(b_+75, loadGfxHeader_hook, SYM(loadGfxHeader), b_+78);
  CALL_C(b_+78, func_02_461c_hook, SYM(func_02_461c), b_+81);
  TAIL(label_02_038__end);
}

void label_02_038__end_hook(GB *gb) {
  BASE(label_02_038);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+81, textInput_updateEntryCursor_hook, SYM(textInput_updateEntryCursor), b_+84);
  CYC(b_+84, b_+87); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void runTextInput_hook(GB *gb) {
  BASE(runTextInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); W8(wTextInputResult) = A;
  CALL_C(b_+5, getInputWithAutofire_hook, SYM(getInputWithAutofire), b_+8);
  CYC(b_+8, b_+9); B = A;
  CALL_C(b_+9, getHighestSetBit_hook, SYM(getHighestSetBit), b_+12);
  if (!(F & FC)) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+17); SET_HL(b_+40);
  CYC(b_+17, b_+18); push_effect(gb, b_+18); add_a_to_hl(gb);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CALL_C(b_+19, playSound_b00_hook, SYM(playSound_b00), b_+22);
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+48) { runTextInput__aButton_hook(gb); return; }
    else if (jt_ == b_+142) { runTextInput__bButton_hook(gb); return; }
    else if (jt_ == b_+159) { runTextInput__selectButton_hook(gb); return; }
    else if (jt_ == b_+271) { runTextInput__startButton_hook(gb); return; }
    else if (jt_ == b_+186) { runTextInput__rightButton_hook(gb); return; }
    else if (jt_ == b_+190) { runTextInput__leftButton_hook(gb); return; }
    else if (jt_ == b_+240) { runTextInput__upButton_hook(gb); return; }
    else if (jt_ == b_+244) { runTextInput__downButton_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void runTextInput__aButton_hook(GB *gb) {
  BASE(runTextInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+48, b_+51); SET_HL(wFileSelect_cursorPos);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+52, b_+54); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+54, b_+56); runTextInput__lowerOptions_hook(gb); return; }
  CYC(b_+54, b_+56);
  CALL_C(b_+56, textInput_getCursorPosition_hook, SYM(textInput_getCursorPosition), b_+59);
  CYC(b_+59, b_+61); alu_and(gb, 0x0f);
  CYC(b_+61, b_+64); SET_HL(w4TileMap + 0xa3);
  CYC(b_+64, b_+65); push_effect(gb, b_+65); add_a_to_hl(gb);
  CYC(b_+65, b_+66); A = B;
  CYC(b_+66, b_+68); A = alu_swap(gb, A);
  CYC(b_+68, b_+69); alu_add(gb, A);
  CYC(b_+69, b_+70); alu_add(gb, A);
  CALL_C(b_+70, multiplyABy16_hook, SYM(multiplyABy16), b_+73);
  CYC(b_+73, b_+74); alu_add_hl(gb, BC);
  CYC(b_+74, b_+76); C = 0x20;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+79, b_+81); runTextInput__gotCharacter_hook(gb); return; }
  CYC(b_+79, b_+81);
  CYC(b_+81, b_+82); alu_rrca(gb);
  CYC(b_+82, b_+84); alu_and(gb, 0x3f);
  CYC(b_+84, b_+86); alu_add(gb, 0x40);
  CYC(b_+86, b_+87); C = A;
  CYC(b_+87, b_+90); A = W8(wFileSelect_textInputMode);
  CYC(b_+90, b_+91); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+91, b_+93); runTextInput__gotCharacter_hook(gb); return; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+94); A = C;
  CYC(b_+94, b_+97); SET_HL(0x0061);
  CYC(b_+97, b_+98); push_effect(gb, b_+98); add_a_to_hl(gb);
  CYC(b_+98, b_+99); C = mem_rd(gb, HL);
  TAIL(runTextInput__gotCharacter);
}

void runTextInput__gotCharacter_hook(GB *gb) {
  BASE(runTextInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+99, textInput_getOutputAddress_hook, SYM(textInput_getOutputAddress), b_+102);
  CYC(b_+102, b_+103); mem_wr(gb, HL, C);
  TAIL(runTextInput__selectionRight);
}

void runTextInput__selectionRight_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+103, b_+106); SET_HL(wFileSelect_textInputCursorPos);
  CYC(b_+106, b_+107); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+107, b_+110); A = W8(wFileSelect_textInputMaxCursorPos);
  CYC(b_+110, b_+111); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+111, b_+113); runTextInput__updateEntryCursor_hook(gb); return; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); mem_wr(gb, HL, A);
  TAIL(runTextInput__updateEntryCursor);
}

void runTextInput__updateEntryCursor_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+114, b_+117); TAIL(textInput_updateEntryCursor);
}

void runTextInput__lowerOptions_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+117, b_+120); A = W8(wFileSelect_textInputMode);
  CYC(b_+120, b_+121); alu_rlca(gb);
  CYC(b_+121, b_+124); A = W8(wFileSelect_cursorPos2);
  if (F & FC) { CYCT(b_+124, b_+126); runTextInput__secretTable_hook(gb); return; }
  CYC(b_+124, b_+126);
  TAIL(runTextInput__nameTable);
}

void runTextInput__nameTable_hook(GB *gb) {
  BASE(runTextInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+126, b_+127); push_effect(gb, b_+127);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+147) { runTextInput__selectionLeft_hook(gb); return; }
    else if (jt_ == b_+103) { runTextInput__selectionRight_hook(gb); return; }
    else if (jt_ == b_+271) { runTextInput__startButton_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void runTextInput__secretTable_hook(GB *gb) {
  BASE(runTextInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+133, b_+134); push_effect(gb, b_+134);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+147) { runTextInput__selectionLeft_hook(gb); return; }
    else if (jt_ == b_+103) { runTextInput__selectionRight_hook(gb); return; }
    else if (jt_ == b_+160) { runTextInput__back_hook(gb); return; }
    else if (jt_ == b_+271) { runTextInput__startButton_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void runTextInput__bButton_hook(GB *gb) {
  BASE(runTextInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+142, textInput_getOutputAddress_hook, SYM(textInput_getOutputAddress), b_+145);
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x20);
  TAIL(runTextInput__selectionLeft);
}

void runTextInput__selectionLeft_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+147, b_+150); SET_HL(wFileSelect_textInputCursorPos);
  CYC(b_+150, b_+151); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+151, b_+153); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+153, b_+155); runTextInput__updateEntryCursor_hook(gb); return; }
  CYC(b_+153, b_+155);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x00);
  CYC(b_+157, b_+159); TAIL(runTextInput__updateEntryCursor);
}

void runTextInput__selectButton_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+159, b_+160); ret_effect(gb);
}

void runTextInput__back_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+160, b_+163); A = W8(wFileSelect_textInputMode);
  CYC(b_+163, b_+164); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+164, b_+165); ret_effect(gb); return; }
  CYC(b_+164, b_+165);
  CYC(b_+165, b_+166); alu_xor(gb, A);
  CYC(b_+166, b_+169); W8(wTmpcbb9) = A;
  CYC(b_+169, b_+172); SET_HL(wFileSelect_cursorPos);
  CYC(b_+172, b_+174); A = 0x57;
  CYC(b_+174, b_+175); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+175, b_+177); A = 0x02;
  CYC(b_+177, b_+178); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+178, b_+179); mem_wr(gb, HL, A); SET_HL(HL - 1);
  if (!(F & FZ)) { CYCT(b_+179, b_+180); ret_effect(gb); return; }
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+182); A = 0x03;
  CYC(b_+182, b_+185); W8(wFileSelect_mode2) = A;
  CYC(b_+185, b_+186); ret_effect(gb);
}

void runTextInput__rightButton_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+186, b_+188); C = 0x01;
  CYC(b_+188, b_+190); TAIL(runTextInput__leftOrRight);
}

void runTextInput__leftButton_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+190, b_+192); C = 0xff;
  TAIL(runTextInput__leftOrRight);
}

void runTextInput__leftOrRight_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+192, b_+195); SET_DE(0x040d);
  CYC(b_+195, b_+198); A = W8(wFileSelect_textInputMode);
  CYC(b_+198, b_+199); alu_rlca(gb);
  if (F & FC) { CYCT(b_+199, b_+201); goto check_upper_options; }
  CYC(b_+199, b_+201);
  CYC(b_+201, b_+204); SET_DE(0x030c);
check_upper_options:
  CYC(b_+204, b_+207); SET_HL(wFileSelect_cursorPos);
  CYC(b_+207, b_+208); A = mem_rd(gb, HL);
  CYC(b_+208, b_+210); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+210, b_+212); runTextInput__leftOrRight__lowerOptions_hook(gb); return; }
  CYC(b_+210, b_+212);
  for (;;) {
    CYC(b_+212, b_+213); alu_add(gb, C);
    CYC(b_+213, b_+215); alu_and(gb, 0x0f);
    CYC(b_+215, b_+216); alu_cp(gb, E);
    if (!(F & FC)) { CYCT(b_+216, b_+218); continue; }
    CYC(b_+216, b_+218);
    break;
  }
  CYC(b_+218, b_+219); C = A;
  CYC(b_+219, b_+220); A = mem_rd(gb, HL);
  CYC(b_+220, b_+222); alu_and(gb, 0xf0);
  CYC(b_+222, b_+223); alu_add(gb, C);
  CYC(b_+223, b_+224); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+224, b_+226); mem_wr(gb, HL, 0x80);
  CYC(b_+226, b_+227); ret_effect(gb);
}

void runTextInput__leftOrRight__lowerOptions_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+227, b_+228); L = alu_inc8(gb, L);
  CYC(b_+228, b_+229); B = D;
  CYC(b_+229, b_+230); A = mem_rd(gb, HL);
  for (;;) {
    CYC(b_+230, b_+231); alu_add(gb, C);
    CYC(b_+231, b_+233); alu_and(gb, 0x0f);
    CYC(b_+233, b_+234); alu_cp(gb, B);
    if (!(F & FC)) { CYCT(b_+234, b_+236); continue; }
    CYC(b_+234, b_+236);
    break;
  }
  CYC(b_+236, b_+237); mem_wr(gb, HL, A);
  CYC(b_+237, b_+240); TAIL(textInput_lowerOption_updateFileSelectCursorPos);
}

void runTextInput__upButton_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+240, b_+242); C = 0xf0;
  CYC(b_+242, b_+244); TAIL(runTextInput__upOrDown);
}

void runTextInput__downButton_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+244, b_+246); C = 0x10;
  TAIL(runTextInput__upOrDown);
}

void runTextInput__upOrDown_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+246, b_+249); SET_HL(wFileSelect_cursorPos);
  CYC(b_+249, b_+250); A = mem_rd(gb, HL);
  for (;;) {
    CYC(b_+250, b_+251); alu_add(gb, C);
    CYC(b_+251, b_+253); alu_and(gb, 0x70);
    CYC(b_+253, b_+255); alu_cp(gb, 0x60);
    if (!(F & FC)) { CYCT(b_+255, b_+257); continue; }
    CYC(b_+255, b_+257);
    break;
  }
  CYC(b_+257, b_+258); C = A;
  CYC(b_+258, b_+259); A = mem_rd(gb, HL);
  CYC(b_+259, b_+261); alu_and(gb, 0x0f);
  CYC(b_+261, b_+262); alu_add(gb, C);
  CYC(b_+262, b_+263); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+263, b_+265); mem_wr(gb, HL, 0x80);
  CYC(b_+265, b_+267); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(b_+267, b_+268); ret_effect(gb); return; }
  CYC(b_+267, b_+268);
  CYC(b_+268, b_+271); TAIL(textInput_lowerOption_updateFileSelectCursorPos2);
}

void runTextInput__startButton_hook(GB *gb) {
  BASE(runTextInput);
  CYC(b_+271, b_+274); SET_HL(wFileSelect_cursorPos);
  CYC(b_+274, b_+276); A = 0x5a;
  CYC(b_+276, b_+277); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+277, b_+280); A = W8(wFileSelect_textInputMode);
  CYC(b_+280, b_+281); alu_rlca(gb);
  CYC(b_+281, b_+283); A = 0x02;
  if (!(F & FC)) CYCT(b_+283, b_+285);
  else { CYC(b_+283, b_+285); CYC(b_+285, b_+287); A = 0x03; }
  CYC(b_+287, b_+288); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+288, b_+289); mem_wr(gb, HL, A); SET_HL(HL - 1);
  if (!(F & FZ)) { CYCT(b_+289, b_+290); ret_effect(gb); return; }
  CYC(b_+289, b_+290);
  CYC(b_+290, b_+293); TAIL(incFileSelectMode2);
}

void textInput_getCursorPosition_hook(GB *gb) {
  BASE(textInput_getCursorPosition);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cursorPos);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); alu_and(gb, 0xf0);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); A = C;
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); push_effect(gb, DE);
  CYC(b_+12, b_+15); SET_DE(0x0801);
  CYC(b_+15, b_+18); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+18, b_+19); alu_rlca(gb);
  if (F & FC) CYCT(b_+19, b_+21);
  else { CYC(b_+19, b_+21); CYC(b_+21, b_+24); SET_DE(0x0602); }
  CYC(b_+24, b_+25); A = C;
  CYC(b_+25, b_+26); alu_cp(gb, D);
  CYC(b_+26, b_+27); C = E;
  CYC(b_+27, b_+28); SET_DE(pop_effect(gb));
  if (!(F & FC)) CYCT(b_+28, b_+30);
  else { CYC(b_+28, b_+30); CYC(b_+30, b_+32); C = 0; }
  CYC(b_+32, b_+33); alu_add(gb, C);
  CYC(b_+33, b_+34); alu_add(gb, B);
  CYC(b_+34, b_+35); ret_effect(gb);
}

static void draw_name_input_cursors_tail(GB *gb) {
  BASE(drawNameInputCursors);
  CYC(b_+51, b_+54); A = W8(wFileSelect_textInputCursorPos);
  CYC(b_+54, b_+55); alu_add(gb, A);
  CYC(b_+55, b_+56); alu_add(gb, A);
  CYC(b_+56, b_+57); alu_add(gb, A);
  CYC(b_+57, b_+58); C = A;
  CYC(b_+58, b_+60); B = 0;
  CYC(b_+60, b_+63); SET_HL(b_+91);
  CYC(b_+63, b_+66); addSpritesToOam_withOffset_hook(gb);
}

void drawNameInputCursors_hook(GB *gb) {
  BASE(drawNameInputCursors);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, textInput_getCursorPosition_hook, SYM(textInput_getCursorPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+5, b_+7); drawNameInputCursors__lowerOptions_hook(gb); return; }
  CYC(b_+5, b_+7); TAIL(drawNameInputCursors__upperOptions);
}

void drawNameInputCursors__upperOptions_hook(GB *gb) {
  BASE(drawNameInputCursors);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); alu_add(gb, A);
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+17); alu_and(gb, 0xf0);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+21); SET_HL(b_+66);
  CALL_C(b_+21, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+24);
  CYC(b_+24, b_+26); draw_name_input_cursors_tail(gb);
}

void drawNameInputCursors__lowerOptions_hook(GB *gb) {
  BASE(drawNameInputCursors);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+26, b_+29); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+29, b_+30); alu_rlca(gb);
  CYC(b_+30, b_+33); SET_HL(b_+78);
  if (F & FC) CYCT(b_+33, b_+35);
  else { CYC(b_+33, b_+35); CYC(b_+35, b_+38); SET_HL(b_+75); }
  CYC(b_+38, b_+41); A = mem_rd(gb, wFileSelect_cursorPos2);
  CYC(b_+41, b_+42); push_effect(gb, b_+42); add_a_to_hl(gb);
  CYC(b_+42, b_+43); C = mem_rd(gb, HL);
  CYC(b_+43, b_+45); B = 0;
  CYC(b_+45, b_+48); SET_HL(b_+82);
  CALL_C(b_+48, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+51);
  draw_name_input_cursors_tail(gb);
}

static void draw_secret_input_cursors_tail(GB *gb) {
  BASE(drawSecretInputCursors);
  CYC(b_+42, b_+44); C = 0x0a;
  CYC(b_+44, b_+47); A = W8(wFileSelect_textInputCursorPos);
  CYC(b_+47, b_+48); alu_cp(gb, C);
  CYC(b_+48, b_+50); B = 0;
  if (F & FC) CYCT(b_+50, b_+52);
  else {
    CYC(b_+50, b_+52);
    CYC(b_+52, b_+54); B = 0x10;
    CYC(b_+54, b_+55); alu_sub(gb, C);
  }
  CYC(b_+55, b_+57); alu_cp(gb, 0x05);
  if (F & FC) CYCT(b_+57, b_+59);
  else { CYC(b_+57, b_+59); CYC(b_+59, b_+60); A = alu_inc8(gb, A); }
  CYC(b_+60, b_+61); alu_add(gb, A);
  CYC(b_+61, b_+62); alu_add(gb, A);
  CYC(b_+62, b_+63); alu_add(gb, A);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+67); SET_HL(b_+92);
  CYC(b_+67, b_+70); addSpritesToOam_withOffset_hook(gb);
}

void drawSecretInputCursors_hook(GB *gb) {
  BASE(drawSecretInputCursors);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, textInput_getCursorPosition_hook, SYM(textInput_getCursorPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+5, b_+7); drawSecretInputCursors__lowerOptions_hook(gb); return; }
  CYC(b_+5, b_+7); TAIL(drawSecretInputCursors__upperOptions);
}

void drawSecretInputCursors__upperOptions_hook(GB *gb) {
  BASE(drawSecretInputCursors);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); alu_add(gb, A);
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+17); alu_and(gb, 0xf0);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+21); SET_HL(b_+70);
  CALL_C(b_+21, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+24);
  CYC(b_+24, b_+26); draw_secret_input_cursors_tail(gb);
}

void drawSecretInputCursors__lowerOptions_hook(GB *gb) {
  BASE(drawSecretInputCursors);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+26, b_+29); A = mem_rd(gb, wFileSelect_cursorPos2);
  CYC(b_+29, b_+32); SET_HL(b_+79);
  CYC(b_+32, b_+33); push_effect(gb, b_+33); add_a_to_hl(gb);
  CYC(b_+33, b_+34); C = mem_rd(gb, HL);
  CYC(b_+34, b_+36); B = 0;
  CYC(b_+36, b_+39); SET_HL(b_+83);
  CALL_C(b_+39, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+42);
  draw_secret_input_cursors_tail(gb);
}

void textInput_lowerOption_updateFileSelectCursorPos_hook(GB *gb) {
  BASE(textInput_lowerOption_updateFileSelectCursorPos);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cursorPos2);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+6); D = 0xff;
  CALL_C(b_+6, textInput_mapUpperXToLowerX_hook, SYM(textInput_mapUpperXToLowerX), b_+9);
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13); mem_wr(gb, wFileSelect_cursorPos, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void textInput_lowerOption_updateFileSelectCursorPos2_hook(GB *gb) {
  BASE(textInput_lowerOption_updateFileSelectCursorPos2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cursorPos);
  CYC(b_+3, b_+4); D = A;
  CYC(b_+4, b_+6); E = 0xff;
  CALL_C(b_+6, textInput_mapUpperXToLowerX_hook, SYM(textInput_mapUpperXToLowerX), b_+9);
  CYC(b_+9, b_+10); A = C;
  CYC(b_+10, b_+13); mem_wr(gb, wFileSelect_cursorPos2, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void textInput_mapUpperXToLowerX_hook(GB *gb) {
  BASE(textInput_mapUpperXToLowerX);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+3, b_+4); alu_rlca(gb);
  CYC(b_+4, b_+7); SET_HL(b_+23);
  if (!(F & FC)) { CYCT(b_+7, b_+9); textInput_mapUpperXToLowerX__label_hook(gb); return; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(b_+51);
  TAIL(textInput_mapUpperXToLowerX__label);
}

void textInput_mapUpperXToLowerX__label_hook(GB *gb) {
  BASE(textInput_mapUpperXToLowerX);
  for (;;) {
    CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+13, b_+14); B = A;
    CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+15, b_+16); C = A;
    CYC(b_+16, b_+17); alu_cp(gb, E);
    if (F & FZ) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
    CYC(b_+17, b_+18);
    CYC(b_+18, b_+19); A = B;
    CYC(b_+19, b_+20); alu_cp(gb, D);
    if (!(F & FZ)) { CYCT(b_+20, b_+22); continue; }
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+23); ret_effect(gb);
    return;
  }
}

void textInput_loadCharacterGfx_hook(GB *gb) {
  BASE(textInput_loadCharacterGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0x70);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x05;
  CYC(b_+5, b_+7); hram_wr(gb, 0x70, A);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wFileSelect_fontXor, A);
  CYC(b_+11, b_+14); SET_DE(w1Link);
  CYC(b_+14, b_+17); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+17, b_+18); alu_rlca(gb);
  if (F & FC) { CYCT(b_+18, b_+20); goto secret; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); SET_BC(0x3b40);
  CALL_C(b_+23, copyTextCharacters_hook, SYM(copyTextCharacters), b_+26);
  CYC(b_+26, b_+28);
  goto done;
secret:
  CYC(b_+28, b_+31); SET_HL(0x00a1);
  CYC(b_+31, b_+33); B = 0x40;
  CALL_C(b_+33, copyTextCharactersFromHlUntilNull_hook, SYM(copyTextCharactersFromHlUntilNull), b_+36);
done:
  CYC(b_+36, b_+37); SET_AF(pop_effect(gb));
  CYC(b_+37, b_+39); hram_wr(gb, 0x70, A);
  CYC(b_+39, b_+40); ret_effect(gb);
}

void copyTextCharacters_hook(GB *gb) {
  BASE(copyTextCharacters);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  do {
    CYC(b_+0, b_+1); push_effect(gb, BC);
    CYC(b_+1, b_+2); A = C;
    CYC(b_+2, b_+4); C = 0;
    CALL_C(b_+4, copyTextCharacterGfx_hook, SYM(copyTextCharacterGfx), b_+7);
    CYC(b_+7, b_+8); SET_BC(pop_effect(gb));
    CYC(b_+8, b_+9); C = alu_inc8(gb, C);
    CYC(b_+9, b_+10); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+10, b_+12); else CYC(b_+10, b_+12);
  } while (!(F & FZ));
  CYC(b_+12, b_+13); ret_effect(gb);
}

void loadFileDisplayVariables_hook(GB *gb) {
  BASE(loadFileDisplayVariables);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, hActiveFileSlot, A);
  TAIL(loadFileDisplayVariables__nextFile);
}

void loadFileDisplayVariables__nextFile_hook(GB *gb) {
  BASE(loadFileDisplayVariables);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(b_+4, loadFile_b00_hook, SYM(loadFile_b00), b_+7);
    CYC(b_+7, b_+9); A = mem_rd(gb, hActiveFileSlot);
    CYC(b_+9, b_+11); D = 0;
    CALL_C(b_+11, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+14);
    CYC(b_+14, b_+15); A = C;
    CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+17, b_+20); A = mem_rd(gb, wLinkMaxHealth);
    CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+22, b_+25); A = mem_rd(gb, wDeathCounter);
    CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+26, b_+29); A = mem_rd(gb, (wDeathCounter + 1));
    CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+30, b_+33); A = mem_rd(gb, wFileIsLinkedGame);
    CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+34, b_+37); A = mem_rd(gb, wFileIsHeroGame);
    CYC(b_+37, b_+38); alu_add(gb, A);
    CYC(b_+38, b_+39); E = A;
    CYC(b_+39, b_+42); A = mem_rd(gb, wFileIsCompleted);
    CYC(b_+42, b_+43); alu_or(gb, E);
    CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+44, b_+46); A = mem_rd(gb, hActiveFileSlot);
    CYC(b_+46, b_+47); alu_add(gb, A);
    CYC(b_+47, b_+48); E = A;
    CYC(b_+48, b_+49); alu_add(gb, E);
    CYC(b_+49, b_+50); alu_add(gb, E);
    CYC(b_+50, b_+53); SET_HL(w4NameBuffer);
    CYC(b_+53, b_+54); push_effect(gb, b_+54); add_a_to_hl(gb);
    CYC(b_+54, b_+57); SET_DE(wLinkName);
    CYC(b_+57, b_+59); B = 0x06;
    CALL_C(b_+59, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+62);
    CYC(b_+62, b_+65); SET_HL(hActiveFileSlot);
    CYC(b_+65, b_+66); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+66, b_+68); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+68, b_+70); continue; }
    CYC(b_+68, b_+70);
    CYC(b_+70, b_+71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+71, b_+72); ret_effect(gb);
    return;
  }
}

void textInput_updateEntryCursor_hook(GB *gb) {
  BASE(textInput_updateEntryCursor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, textInput_getOutputAddressOffset_hook, SYM(textInput_getOutputAddressOffset), b_+4);
  CYC(b_+4, b_+7); SET_DE(w1ReservedItemC);
  CYC(b_+7, b_+9); B = 0x18;
  CALL_C(b_+9, copyTextCharactersFromHl_hook, SYM(copyTextCharactersFromHl), b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, wFileSelect_fontXor, A);
  CYC(b_+16, b_+18); A = 0x07;
  CYC(b_+18, b_+21); TAIL(loadUncompressedGfxHeader);
}

void textInput_getOutputAddress_hook(GB *gb) {
  BASE(textInput_getOutputAddress);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_textInputCursorPos);
  TAIL(textInput_getOutputAddressOffset);
}

void textInput_getOutputAddressOffset_hook(GB *gb) {
  BASE(textInput_getOutputAddressOffset);
  CYC(b_+0, b_+1); L = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wFileSelect_textInputMode);
  CYC(b_+4, b_+5); alu_rlca(gb);
  CYC(b_+5, b_+6); A = L;
  CYC(b_+6, b_+9); SET_HL(w4NameBuffer);
  if (!(F & FC)) CYCT(b_+9, b_+11);
  else { CYC(b_+9, b_+11); CYC(b_+11, b_+14); SET_HL(w4SecretBuffer); }
  CYC(b_+14, b_+15); push_effect(gb, b_+15); add_a_to_hl(gb);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void fileSelectDrawHeartsAndDeathCounter_hook(GB *gb) {
  BASE(fileSelectDrawHeartsAndDeathCounter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFakeResetMenu);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0xa2;
  CALL_C(b_+8, loadGfxHeader_hook, SYM(loadGfxHeader), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wFileSelect_cursorPos);
  CYC(b_+14, b_+16); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto done; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); D = 0;
  CALL_C(b_+20, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+23);
  CYC(b_+23, b_+25); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto done; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); D = 0x04;
  CALL_C(b_+29, getFileDisplayVariableAddress_paramE_hook, SYM(getFileDisplayVariableAddress_paramE), b_+32);
  CYC(b_+32, b_+33); E = L;
  CYC(b_+33, b_+34); D = H;
  CYC(b_+34, b_+37); SET_HL(w1Companion_animMode);
  CYC(b_+37, b_+39); B = 0x10;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_and(gb, 0x0f);
  CYC(b_+42, b_+43); alu_add(gb, B);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); alu_and(gb, 0xf0);
  CYC(b_+47, b_+49); A = alu_swap(gb, A);
  CYC(b_+49, b_+50); alu_add(gb, B);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(b_+51, b_+52); E = alu_inc8(gb, E);
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_add(gb, B);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(b_+55, b_+58); A = mem_rd(gb, wFileSelect_cursorPos);
  CYC(b_+58, b_+60); D = 0x02;
  CALL_C(b_+60, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+64, b_+65); B = mem_rd(gb, HL);
  CYC(b_+65, b_+66); C = A;
  CYC(b_+66, b_+69); SET_HL(w1ReservedInteraction1_y);
  CALL_C(b_+69, fileSelectDrawHeartDisplay_hook, SYM(fileSelectDrawHeartDisplay), b_+72);
done:
  CYC(b_+72, b_+74); A = 0x08;
  CYC(b_+74, b_+77); TAIL(loadUncompressedGfxHeader);
}

void fileSelectDrawAcornCursor_hook(GB *gb) {
  BASE(fileSelectDrawAcornCursor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFileSelect_cursorOffset);
  CYC(b_+3, b_+6); SET_HL(b_+55);
  CYC(b_+6, b_+7); add_double_index_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); D = A;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+17); B = mem_rd(gb, HL);
  CYC(b_+17, b_+18); push_effect(gb, BC);
  CYC(b_+18, b_+21); SET_HL(b_+50);
  CYC(b_+21, b_+24); A = mem_rd(gb, wFileSelect_cursorPos);
  CYC(b_+24, b_+26); alu_bit(gb, 7, A);
  if (F & FZ) CALL_C_CC(b_+26, fileSelectDrawAcornCursor__func_hook, b_+39, b_+29);
  else CYC(b_+26, b_+29);
  CYC(b_+29, b_+30); SET_DE(pop_effect(gb));
  CYC(b_+30, b_+33); SET_HL(b_+50);
  CYC(b_+33, b_+36); A = mem_rd(gb, wFileSelect_cursorPos2);
  CYC(b_+36, b_+38); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+39); ret_effect(gb); return; }
  CYC(b_+38, b_+39); TAIL(fileSelectDrawAcornCursor__func);
}

void fileSelectDrawAcornCursor__func_hook(GB *gb) {
  BASE(fileSelectDrawAcornCursor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+39, addDoubleIndexToDe_hook, 0x0072, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+45); E = alu_inc8(gb, E);
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); C = A;
  CYC(b_+47, b_+50); TAIL(addSpritesToOam_withOffset);
}

void hideStatusBar_body_hook(GB *gb) {
  BASE(hideStatusBar_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+7); SET_HL(wDontUpdateStatusBar);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xff);
  CYC(b_+10, b_+12); alu_cp(gb, 0x77);
  CYC(b_+12, b_+14); A = 0x80;
  if (!(F & FZ)) CYCT(b_+14, b_+16);
  else { CYC(b_+14, b_+16); CYC(b_+16, b_+17); alu_xor(gb, A); }
  CYC(b_+17, b_+20); SET_HL(w4StatusBarAttributeMap);
  CYC(b_+20, b_+22); B = 0x40;
  CALL_C(b_+22, fillMemory_hook, SYM(fillMemory), b_+25);
  CYC(b_+25, b_+28); SET_HL(w4StatusBarTileMap);
  CYC(b_+28, b_+30); B = 0x40;
  CALL_C(b_+30, clearMemory_hook, SYM(clearMemory), b_+33);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37); W8(wStatusBarNeedsRefresh) = A;
  CYC(b_+37, b_+39); A = 0x03;
  CALL_C(b_+39, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+42);
  CYC(b_+42, b_+44); B = 0x10;
  CYC(b_+44, b_+46); A = mem_rd(gb, hOamTail);
  CYC(b_+46, b_+47); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0xe0;
  CYC(b_+50, b_+53); SET_HL(wOam);
  CYC(b_+53, b_+56); TAIL(fillMemory);
}

void showStatusBar_body_hook(GB *gb) {
  BASE(showStatusBar_body);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wDontUpdateStatusBar) = A;
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  CYC(b_+5, b_+8); W8(wStatusBarNeedsRefresh) = A;
  CYC(b_+8, b_+9); ret_effect(gb);
}

void openMenu_body_hook(GB *gb) {
  BASE(openMenu_body);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+4); SET_HL(wOpenedMenuType);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+12); W8(wTextIsActive) = A;
  CYC(b_+12, b_+15); TAIL(fastFadeoutToWhite);
}

void copyW2TilesetBgPalettesToW4PaletteData_body_hook(GB *gb) {
  BASE(copyW2TilesetBgPalettesToW4PaletteData_body);
  CYC(b_+0, b_+3); SET_HL(w2TilesetBgPalettes);
  CYC(b_+3, b_+6); SET_DE(w4PaletteData);
  CYC(b_+6, b_+8); B = 0x80;
  do {
    CYC(b_+8, b_+10); A = 0x02;
    CYC(b_+10, b_+12); hram_wr(gb, 0x70, A);
    CYC(b_+12, b_+13); C = mem_rd(gb, HL);
    CYC(b_+13, b_+14); L = alu_inc8(gb, L);
    CYC(b_+14, b_+16); A = 0x04;
    CYC(b_+16, b_+18); hram_wr(gb, 0x70, A);
    CYC(b_+18, b_+19); A = C;
    CYC(b_+19, b_+20); mem_wr(gb, DE, A);
    CYC(b_+20, b_+21); SET_DE(DE + 1);
    CYC(b_+21, b_+22); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+22, b_+24); else CYC(b_+22, b_+24);
  } while (!(F & FZ));
  CYC(b_+24, b_+26); A = 0xff;
  CYC(b_+26, b_+28); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+28, b_+30); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void copyW4PaletteDataToW2TilesetBgPalettes_body_hook(GB *gb) {
  BASE(copyW4PaletteDataToW2TilesetBgPalettes_body);
  CYC(b_+0, b_+3); SET_HL(w4PaletteData);
  CYC(b_+3, b_+6); SET_DE(w2TilesetBgPalettes);
  CYC(b_+6, b_+8); B = 0x80;
  do {
    CYC(b_+8, b_+10); A = 0x04;
    CYC(b_+10, b_+12); hram_wr(gb, 0x70, A);
    CYC(b_+12, b_+13); C = mem_rd(gb, HL);
    CYC(b_+13, b_+14); L = alu_inc8(gb, L);
    CYC(b_+14, b_+16); A = 0x02;
    CYC(b_+16, b_+18); hram_wr(gb, 0x70, A);
    CYC(b_+18, b_+19); A = C;
    CYC(b_+19, b_+20); mem_wr(gb, DE, A);
    CYC(b_+20, b_+21); SET_DE(DE + 1);
    CYC(b_+21, b_+22); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+22, b_+24); else CYC(b_+22, b_+24);
  } while (!(F & FZ));
  CYC(b_+24, b_+26); A = 0xff;
  CYC(b_+26, b_+28); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+28, b_+30); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void closeMenu_hook(GB *gb) {
  BASE(closeMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wMenuLoadState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); A = W8(wOpenedMenuType);
  CYC(b_+7, b_+9); alu_cp(gb, 0x03);
  CYC(b_+9, b_+11); A = 0x55;
  if (!(F & FZ)) CALL_C_CC(b_+11, playSound_b00_hook, SYM(playSound_b00), b_+14);
  else CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); W8(wTextIsActive) = A;
  CYC(b_+18, b_+21); TAIL(fastFadeoutToWhite);
}

void func_02_494a_hook(GB *gb) {
  BASE(func_02_494a);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); SET_HL(b_+80);
  CYC(b_+4, b_+6); alu_bit(gb, 0, E);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto have_table; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+39);
have_table:
  CYC(b_+11, b_+13); alu_cp(gb, 0x60);
  CYC(b_+13, b_+14); alu_ccf(gb);
  if (!(F & FC)) { CYCT(b_+14, b_+16); func_02_494a__end_hook(gb); return; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); B = 0x00;
  CYC(b_+18, b_+20); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+20, b_+22); goto set_comparator; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); B = 0x50;
  CYC(b_+24, b_+25); alu_sub(gb, B);
set_comparator:
  CYC(b_+25, b_+26); C = A;
next_table_entry:
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); func_02_494a__end_hook(gb); return; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); alu_cp(gb, C);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto next_table_entry; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); alu_add(gb, B);
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+37); alu_scf(gb);
  TAIL(func_02_494a__end);
}

void func_02_494a__end_hook(GB *gb) {
  BASE(func_02_494a);
  CYC(b_+37, b_+38); SET_HL(pop_effect(gb));
  CYC(b_+38, b_+39); ret_effect(gb);
}

void playHeartBeepAtInterval_hook(GB *gb) {
  BASE(playHeartBeepAtInterval);
  CYC(b_+0, b_+3); A = W8(w1Link_id);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wFrameCounter);
  CYC(b_+8, b_+10); alu_and(gb, 0x3f);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_HL(wLinkHealth);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, A);
  if (F & FC) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); alu_add(gb, A);
  CYC(b_+19, b_+20); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+20, b_+21); ret_effect(gb); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); A = 0x60;
  CYC(b_+23, b_+26); TAIL(playSound_b00);
}

void loadCommonGraphics_body_hook(GB *gb) {
  BASE(loadCommonGraphics_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CYC(b_+3, b_+5); A = 0x20;
  CALL_C(b_+5, loadGfxHeader_hook, SYM(loadGfxHeader), b_+8);
  CYC(b_+8, b_+10); A = 0x83;
  CALL_C(b_+10, loadGfxHeader_hook, SYM(loadGfxHeader), b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+17); W8(wcbe8) = A;
  CALL_C(b_+17, updateStatusBar_body_hook, SYM(updateStatusBar_body), b_+20);
  CYC(b_+20, b_+23); A = W8(wActiveGroup);
  CYC(b_+23, b_+25); alu_sub(gb, 0x02);
  CYC(b_+25, b_+27); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto done; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); A = W8(wTilesetFlags);
  CYC(b_+32, b_+34); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+34, b_+36); goto done; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); A = 0x44;
  CALL_C(b_+38, loadGfxHeader_hook, SYM(loadGfxHeader), b_+41);
  CYC(b_+41, b_+43); A = 0x34;
  CALL_C(b_+43, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+46);
done:
  CYC(b_+46, b_+49); TAIL(checkReloadStatusBarGraphics);
}

void updateStatusBar_body_hook(GB *gb) {
  BASE(updateStatusBar_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wDontUpdateStatusBar);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x04;
  CYC(b_+7, b_+9); hram_wr(gb, R_SVBK, A);
  CALL_C(b_+9, loadStatusBarMap_hook, SYM(loadStatusBarMap), b_+12);
  CYC(b_+12, b_+15); A = W8(wStatusBarNeedsRefresh);
  CYC(b_+15, b_+17); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+17, b_+19); goto item_count_refresh; }
  CYC(b_+17, b_+19);
  CALL_C(b_+19, loadEquippedItemGfx_hook, SYM(loadEquippedItemGfx), b_+22);
  CALL_C(b_+22, drawItemTilesOnStatusBar_hook, SYM(drawItemTilesOnStatusBar), b_+25);
  CYC(b_+25, b_+27);
  goto update_rupees;
item_count_refresh:
  CYC(b_+27, b_+29); alu_bit(gb, 1, A);
  if (!(F & FZ)) CALL_C_CC(b_+29, drawItemTilesOnStatusBar_hook, SYM(drawItemTilesOnStatusBar), b_+32);
  else CYC(b_+29, b_+32);
update_rupees:
  CYC(b_+32, b_+35); SET_HL(wNumRupees);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); B = mem_rd(gb, HL);
  CYC(b_+37, b_+38); C = A;
  CYC(b_+38, b_+41); SET_HL(wDisplayedRupees);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); H = mem_rd(gb, HL);
  CYC(b_+43, b_+44); L = A;
  CALL_C(b_+44, compareHlToBc_hook, SYM(compareHlToBc), b_+47);
  if (F & FZ) { CYCT(b_+47, b_+49); updateStatusBar_body__updateRupeeDisplay_hook(gb); return; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+52); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+52, b_+54); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(b_+54, b_+57); SET_BC(0x0001);
  CYC(b_+57, b_+59); L = (uint8_t)wDisplayedRupees;
  CYC(b_+59, b_+60); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+60, b_+62); goto subtract_rupee; }
  CYC(b_+60, b_+62);
  CALL_C(b_+62, addDecimalToHlRef_hook, SYM(addDecimalToHlRef), b_+65);
  CYC(b_+65, b_+67);
  goto play_rupee_sound;
subtract_rupee:
  CALL_C(b_+67, subDecimalFromHlRef_hook, SYM(subDecimalFromHlRef), b_+70);
play_rupee_sound:
  CYC(b_+70, b_+72); A = 0x61;
  CALL_C(b_+72, playSound_b00_hook, SYM(playSound_b00), b_+75);
  TAIL(updateStatusBar_body__updateRupeeDisplay);
}

void updateStatusBar_body__updateRupeeDisplay_hook(GB *gb) {
  BASE(updateStatusBar_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+75, b_+78); A = W8(wStatusBarNeedsRefresh);
  CYC(b_+78, b_+80); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+80, b_+82); goto update_hearts; }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+85); SET_HL(w4StatusBarTileMap + 0x2c);
  CALL_C(b_+85, correctAddressForExtraHeart_hook, SYM(correctAddressForExtraHeart), b_+88);
  CYC(b_+88, b_+90); C = 0x10;
  CYC(b_+90, b_+93); A = W8(wDisplayedRupees);
  CYC(b_+93, b_+94); B = A;
  CYC(b_+94, b_+96); alu_and(gb, 0x0f);
  CYC(b_+96, b_+97); alu_add(gb, C);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+98, b_+99); A = B;
  CYC(b_+99, b_+101); A = alu_swap(gb, A);
  CYC(b_+101, b_+103); alu_and(gb, 0x0f);
  CYC(b_+103, b_+104); alu_add(gb, C);
  CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+105, b_+108); A = mem_rd(gb, wDisplayedRupees + 1);
  CYC(b_+108, b_+110); alu_and(gb, 0x0f);
  CYC(b_+110, b_+111); alu_add(gb, C);
  CYC(b_+111, b_+112); mem_wr(gb, HL, A); SET_HL(HL - 1);
update_hearts:
  CYC(b_+112, b_+115); SET_HL(wDisplayedHearts);
  CYC(b_+115, b_+118); A = W8(wLinkHealth);
  CYC(b_+118, b_+119); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+119, b_+121); updateStatusBar_body__updateHeartDisplay_hook(gb); return; }
  CYC(b_+119, b_+121);
  if (F & FC) { CYCT(b_+121, b_+123); goto decrement_hearts; }
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+126); A = W8(wFrameCounter);
  CYC(b_+126, b_+128); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); updateStatusBar_body__updateHeartDisplay_hook(gb); return; }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+131); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+134); alu_and(gb, 0x03);
  CYC(b_+134, b_+136); A = 0x57;
  if (F & FZ) CALL_C_CC(b_+136, playSound_b00_hook, SYM(playSound_b00), b_+139);
  else CYC(b_+136, b_+139);
  CYC(b_+139, b_+141);
  goto mark_heart_refresh;
decrement_hearts:
  CYC(b_+141, b_+142); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
mark_heart_refresh:
  CYC(b_+142, b_+145); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+145, b_+147); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  TAIL(updateStatusBar_body__updateHeartDisplay);
}

void updateStatusBar_body__updateHeartDisplay_hook(GB *gb) {
  BASE(updateStatusBar_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+147, b_+150); A = W8(wStatusBarNeedsRefresh);
  CYC(b_+150, b_+152); alu_bit(gb, 2, A);
  if (!(F & FZ)) CALL_C_CC(b_+152, inGameDrawHeartDisplay_hook, SYM(inGameDrawHeartDisplay), b_+155);
  else CYC(b_+152, b_+155);
  CYC(b_+155, b_+158); SET_HL(w4StatusBarTileMap + 0x0a);
  CALL_C(b_+158, correctAddressForExtraHeart_hook, SYM(correctAddressForExtraHeart), b_+161);
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0x09);
  CYC(b_+163, b_+166); A = W8(wTilesetFlags);
  CYC(b_+166, b_+168); alu_bit(gb, 4, A);
  if (!(F & FZ)) { CYCT(b_+168, b_+170); goto sprites; }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+172); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+172, b_+174); goto sprites; }
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+175); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+175, b_+176); L = alu_inc8(gb, L);
  CYC(b_+176, b_+178); mem_wr(gb, HL, 0x1b);
  CYC(b_+178, b_+181); A = W8(wStatusBarNeedsRefresh);
  CYC(b_+181, b_+183); alu_bit(gb, 4, A);
  if (F & FZ) { CYCT(b_+183, b_+185); goto sprites; }
  CYC(b_+183, b_+185);
  CYC(b_+185, b_+186); L = alu_inc8(gb, L);
  CYC(b_+186, b_+189); A = W8(wDungeonIndex);
  CYC(b_+189, b_+192); SET_BC(wDungeonSmallKeys);
  CALL_C(b_+192, addAToBc_hook, 0x006d, b_+195);
  CYC(b_+195, b_+196); A = mem_rd(gb, BC);
  CYC(b_+196, b_+198); alu_add(gb, 0x10);
  CYC(b_+198, b_+199); mem_wr(gb, HL, A);
sprites:
  CYC(b_+199, b_+200); alu_xor(gb, A);
  CYC(b_+200, b_+202); hram_wr(gb, R_SVBK, A);
  CYC(b_+202, b_+205); A = W8(wcbe8);
  CYC(b_+205, b_+207); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+207, b_+209); updateStatusBar_body__biggoronSword_hook(gb); return; }
  CYC(b_+207, b_+209);
  CYC(b_+209, b_+211); E = 0x10;
  CYC(b_+211, b_+214); SET_BC(0x1038);
  CYC(b_+214, b_+215); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+215, b_+217); goto positions; }
  CYC(b_+215, b_+217);
  CYC(b_+217, b_+219); C = 0x30;
positions:
  CYC(b_+219, b_+222); SET_HL(wInventoryB);
  CYC(b_+222, b_+224); A = 0x11;
  CYC(b_+224, b_+225); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+225, b_+227); goto second_item; }
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+229); E |= 0x08;
second_item:
  CYC(b_+229, b_+230); L = alu_inc8(gb, L);
  CYC(b_+230, b_+231); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+231, b_+233); goto write_sprites; }
  CYC(b_+231, b_+233);
  CYC(b_+233, b_+234); A = C;
  CYC(b_+234, b_+236); alu_add(gb, 0x08);
  CYC(b_+236, b_+237); C = A;
write_sprites:
  CYC(b_+237, b_+240); SET_HL(wOam);
  CYC(b_+240, b_+241); A = B;
  CYC(b_+241, b_+242); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+242, b_+243); A = E;
  CYC(b_+243, b_+244); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+244, b_+246); A = 0x78;
  CYC(b_+246, b_+247); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+247, b_+250); A = W8(wBItemSpriteAttribute1);
  CYC(b_+250, b_+251); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+251, b_+252); A = B;
  CYC(b_+252, b_+253); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+253, b_+256); A = W8(wBItemSpriteXOffset);
  CYC(b_+256, b_+257); alu_add(gb, E);
  CYC(b_+257, b_+258); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+258, b_+260); A = 0x7a;
  CYC(b_+260, b_+261); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+261, b_+264); A = W8(wBItemSpriteAttribute2);
  CYC(b_+264, b_+265); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+265, b_+266); A = B;
  CYC(b_+266, b_+267); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+267, b_+268); A = C;
  CYC(b_+268, b_+269); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+269, b_+271); A = 0x7c;
  CYC(b_+271, b_+272); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+272, b_+275); A = W8(wAItemSpriteAttribute1);
  CYC(b_+275, b_+276); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+276, b_+277); A = B;
  CYC(b_+277, b_+278); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+278, b_+281); A = W8(wAItemSpriteXOffset);
  CYC(b_+281, b_+282); alu_add(gb, C);
  CYC(b_+282, b_+283); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+283, b_+285); A = 0x7e;
  CYC(b_+285, b_+286); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+286, b_+289); A = W8(wAItemSpriteAttribute2);
  CYC(b_+289, b_+290); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+290, b_+291); ret_effect(gb);
}

void updateStatusBar_body__biggoronSword_hook(GB *gb) {
  BASE(updateStatusBar_body);
  CYC(b_+291, b_+294); SET_HL(wOam);
  CYC(b_+294, b_+297); SET_DE(GV(b_+302, 0x528d));
  CYC(b_+297, b_+299); B = 0x10;
  CYC(b_+299, b_+302); TAIL(copyMemoryReverse);
}

void correctAddressForExtraHeart_hook(GB *gb) {
  BASE(correctAddressForExtraHeart);
  CYC(b_+0, b_+3); A = W8(wcbe8);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); L = alu_dec8(gb, L);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void loadEquippedItemGfx_hook(GB *gb) {
  BASE(loadEquippedItemGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, loadStatusBarMap_hook, SYM(loadStatusBarMap), b_+3);
  CYC(b_+3, b_+6); A = W8(wcbe8);
  CYC(b_+6, b_+7); alu_rlca(gb);
  if (F & FC) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = W8(wInventoryB);
  CYC(b_+11, b_+14); SET_DE(wBItemTreasure);
  CALL_C(b_+14, loadEquippedItemSpriteData_hook, SYM(loadEquippedItemSpriteData), b_+17);
  CYC(b_+17, b_+19); E = (uint8_t)w4ItemIconGfx;
  if (F & FC) CALL_C_CC(b_+19, loadItemIconGfx_hook, SYM(loadItemIconGfx), b_+22);
  else CYC(b_+19, b_+22);
  CYC(b_+22, b_+25); A = W8(wInventoryA);
  CYC(b_+25, b_+28); SET_DE(wAItemTreasure);
  CALL_C(b_+28, loadEquippedItemSpriteData_hook, SYM(loadEquippedItemSpriteData), b_+31);
  CYC(b_+31, b_+33); E = (uint8_t)(w4ItemIconGfx + 0x40);
  if (F & FC) CALL_C_CC(b_+33, loadItemIconGfx_hook, SYM(loadItemIconGfx), SYM(func_02_52f6));
  else CYC(b_+33, b_+36);
  TAIL(func_02_52f6);
}

void func_02_52f6_hook(GB *gb) {
  BASE(func_02_52f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0020);
  CYC(b_+3, b_+6); SET_HL(w4StatusBarAttributeMap + 0x02);
  CYC(b_+6, b_+9); A = W8(wBItemSpriteXOffset);
  CYC(b_+9, b_+11); alu_bit(gb, 7, A);
  if (F & FZ) CALL_C_CC(b_+11, func_02_52f6__func1_hook, b_+29, b_+14);
  else CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); L = (uint8_t)(w4StatusBarAttributeMap + 0x07);
  CYC(b_+16, b_+19); A = W8(wcbe8);
  CYC(b_+19, b_+20); alu_rrca(gb);
  if (!(F & FC)) CYCT(b_+20, b_+22);
  else { CYC(b_+20, b_+22); CYC(b_+22, b_+23); L = alu_dec8(gb, L); }
  CYC(b_+23, b_+26); A = W8(wAItemSpriteXOffset);
  CYC(b_+26, b_+28); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+28, b_+29); ret_effect(gb); return; }
  CYC(b_+28, b_+29);
  TAIL(func_02_52f6__func1);
}

void func_02_52f6__func1_hook(GB *gb) {
  BASE(func_02_52f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+30, func_02_52f6__func2_hook, b_+34, b_+33);
  else CYC(b_+30, b_+33);
  CYC(b_+33, b_+34); L = alu_dec8(gb, L);
  TAIL(func_02_52f6__func2);
}

void func_02_52f6__func2_hook(GB *gb) {
  BASE(func_02_52f6);
  CYC(b_+34, b_+35); D = L;
  CYC(b_+35, b_+36); mem_wr(gb, HL, B);
  CYC(b_+36, b_+37); alu_add_hl(gb, BC);
  CYC(b_+37, b_+38); mem_wr(gb, HL, B);
  CYC(b_+38, b_+39); L = D;
  CYC(b_+39, b_+40); ret_effect(gb);
}

void loadEquippedItemSpriteData_hook(GB *gb) {
  BASE(loadEquippedItemSpriteData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, loadTreasureDisplayData_b00_hook, SYM(loadTreasureDisplayData_b00), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); loadEquippedItemSpriteData__clearItem_hook(gb); return; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); alu_cp(gb, GV(0x84, 0x86));
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto attr1; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_sub(gb, 0x03);
  CYC(b_+18, b_+20); alu_or(gb, 0x01);
attr1:
  CYC(b_+20, b_+22); A |= 0x08;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); alu_or(gb, A);
  CYC(b_+26, b_+27); C = A;
  if (F & FZ) { CYCT(b_+27, b_+29); goto attr2; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); alu_scf(gb);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
attr2:
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+34); A |= 0x08;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);
  CYC(b_+36, b_+38); A = 0x08;
  if (F & FC) CYCT(b_+38, b_+40);
  else { CYC(b_+38, b_+40); CYC(b_+40, b_+41); alu_xor(gb, A); }
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); E = alu_inc8(gb, E);
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); alu_scf(gb);
  CYC(b_+46, b_+47); ret_effect(gb);
}

void loadEquippedItemSpriteData__clearItem_hook(GB *gb) {
  BASE(loadEquippedItemSpriteData);
  CYC(b_+47, b_+48); L = E;
  CYC(b_+48, b_+49); H = D;
  CYC(b_+49, b_+51); B = 0x05;
  CYC(b_+51, b_+53); A = 0xff;
  do {
    CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+54, b_+55); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+55, b_+57); else CYC(b_+55, b_+57);
  } while (!(F & FZ));
  CYC(b_+57, b_+58); ret_effect(gb);
}

void drawItemTilesOnStatusBar_hook(GB *gb) {
  BASE(drawItemTilesOnStatusBar);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wcbe8);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x04;
  CYC(b_+8, b_+10); hram_wr(gb, 0x70, A);
  CYC(b_+10, b_+13); A = W8(wInventoryB);
  CYC(b_+13, b_+16); SET_DE(wBItemTreasure);
  CALL_C(b_+16, loadEquippedItemSpriteData_hook, SYM(loadEquippedItemSpriteData), b_+19);
  CYC(b_+19, b_+22); A = W8(wInventoryA);
  CYC(b_+22, b_+25); SET_DE(wAItemTreasure);
  CALL_C(b_+25, loadEquippedItemSpriteData_hook, SYM(loadEquippedItemSpriteData), b_+28);
  CALL_C(b_+28, func_02_52f6_hook, SYM(func_02_52f6), b_+31);
  CYC(b_+31, b_+34); A = W8(wcbe8);
  CYC(b_+34, b_+35); alu_rrca(gb);
  CYC(b_+35, b_+38); SET_DE(w4StatusBarTileMap + 0x27);
  if (!(F & FC)) CYCT(b_+38, b_+40);
  else { CYC(b_+38, b_+40); CYC(b_+40, b_+41); E = alu_dec8(gb, E); }
  CYC(b_+41, b_+44); A = W8(wAItemTreasure);
  CYC(b_+44, b_+45); B = A;
  CYC(b_+45, b_+48); A = W8(wAItemDisplayMode);
  CALL_C(b_+48, drawItemTilesOnStatusBar__drawItem_hook, b_+61, b_+51);
  CYC(b_+51, b_+54); SET_DE(w4StatusBarTileMap + 0x22);
  CYC(b_+54, b_+57); A = W8(wBItemTreasure);
  CYC(b_+57, b_+58); B = A;
  CYC(b_+58, b_+61); A = W8(wBItemDisplayMode);
  TAIL(drawItemTilesOnStatusBar__drawItem);
}

void drawItemTilesOnStatusBar__drawItem_hook(GB *gb) {
  BASE(drawItemTilesOnStatusBar);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+61, b_+62); C = A;
  CYC(b_+62, b_+63); alu_rlca(gb);
  if (F & FC) { CYCT(b_+63, b_+64); ret_effect(gb); return; }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+65); A = B;
  CALL_C(b_+65, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+68);
  CYC(b_+68, b_+69); B = A;
  CYC(b_+69, b_+70); A = C;
  CYC(b_+70, b_+72); C = 0x80;
  TAIL(drawTreasureExtraTiles);
}

void drawTreasureExtraTiles_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+0, b_+2); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); drawTreasureExtraTiles__val01_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); drawTreasureExtraTiles__val02_hook(gb); return; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); drawTreasureExtraTiles__val03_hook(gb); return; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+13, b_+15); drawTreasureExtraTiles__val04_hook(gb); return; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); TAIL(drawTreasureExtraTiles__val00);
}

void drawTreasureExtraTiles__val04_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+17, b_+18); E = alu_inc8(gb, E);
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+21); alu_and(gb, 0x0f);
  CYC(b_+21, b_+23); alu_add(gb, 0x10);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); D |= 0x04;
  CYC(b_+26, b_+27); A = C;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_dec8(gb, E);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); D &= ~0x04;
  CYC(b_+32, b_+34); A = 0x1b;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); ret_effect(gb);
}

void drawTreasureExtraTiles__val01_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+36, b_+37); E = alu_inc8(gb, E);
  CYC(b_+37, b_+38); A = B;
  CYC(b_+38, b_+40); alu_and(gb, 0x0f);
  CYC(b_+40, b_+42); alu_add(gb, 0x10);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+45); D |= 0x04;
  CYC(b_+45, b_+46); A = C;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+48); E = alu_dec8(gb, E);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+51); D &= ~0x04;
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+54); A = alu_swap(gb, A);
  CYC(b_+54, b_+56); alu_and(gb, 0x0f);
  CYC(b_+56, b_+58); alu_add(gb, 0x10);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+60); ret_effect(gb);
}

void drawTreasureExtraTiles__val00_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+60, b_+61); E = alu_inc8(gb, E);
  CYC(b_+61, b_+62); A = B;
  CYC(b_+62, b_+64); alu_and(gb, 0x0f);
  CYC(b_+64, b_+66); alu_add(gb, 0x10);
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+69); D |= 0x04;
  CYC(b_+69, b_+70); A = C;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+72); E = alu_dec8(gb, E);
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+75); D &= ~0x04;
  CYC(b_+75, b_+77); A = 0x1a;
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  CYC(b_+78, b_+79); ret_effect(gb);
}

void drawTreasureExtraTiles__val03_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+79, b_+80); ret_effect(gb);
}

void drawTreasureExtraTiles__val02_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+82); L = E;
  CYC(b_+82, b_+83); A = C;
  CYC(b_+83, b_+85); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+85, b_+87); drawTreasureExtraTiles__val02__drawOnInventory_hook(gb); return; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); A = 0x1f;
  CYC(b_+89, b_+90); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x1d);
  CYC(b_+92, b_+94); H |= 0x04;
  CYC(b_+94, b_+96); A = 0x80;
  CYC(b_+96, b_+97); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x00);
  CYC(b_+100, b_+103); SET_BC((hSoundChannelBanks + 6));
  CYC(b_+103, b_+104); alu_add_hl(gb, BC);
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x00);
  CYC(b_+106, b_+107); L = alu_dec8(gb, L);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+108, b_+109); mem_wr(gb, HL, A);
  CYC(b_+109, b_+111); H &= ~0x04;
  CYC(b_+111, b_+113); A = 0x1c;
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x1e);
  CYC(b_+116, b_+117); ret_effect(gb);
}

void drawTreasureExtraTiles__val02__drawOnInventory_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+117, b_+119); A = 0x1f;
  CYC(b_+119, b_+120); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x1d);
  CYC(b_+122, b_+124); H |= 0x04;
  CYC(b_+124, b_+126); A = 0x84;
  CYC(b_+126, b_+127); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+127, b_+128); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+128, b_+131); SET_BC((hSoundChannelBanks + 6));
  CYC(b_+131, b_+132); alu_add_hl(gb, BC);
  CYC(b_+132, b_+133); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+133, b_+134); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+134, b_+136); H &= ~0x04;
  CYC(b_+136, b_+138); A = 0x1c;
  CYC(b_+138, b_+139); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x1e);
  CYC(b_+141, b_+142); ret_effect(gb);
}

void drawTreasureExtraTiles__drawTile_hook(GB *gb) {
  BASE(drawTreasureExtraTiles);
  CYC(b_+142, b_+143); mem_wr(gb, HL, B);
  CYC(b_+143, b_+145); H |= 0x04;
  CYC(b_+145, b_+146); mem_wr(gb, HL, C);
  CYC(b_+146, b_+148); H &= ~0x04;
  CYC(b_+148, b_+149); ret_effect(gb);
}

void fileSelectDrawHeartDisplay_hook(GB *gb) {
  BASE(fileSelectDrawHeartDisplay);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); mem_wr(gb, hFF8B, A);
  CYC(b_+4, b_+5); A = B;
  CYC(b_+5, b_+7); TAIL(drawHeartDisplay);
}

void inGameDrawHeartDisplay_hook(GB *gb) {
  BASE(inGameDrawHeartDisplay);
  CYC(b_+0, b_+3); SET_HL(w4StatusBarTileMap + 0x0d);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8B, A);
  CYC(b_+6, b_+9); A = W8(wDisplayedHearts);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); A = W8(wLinkMaxHealth);
  TAIL(drawHeartDisplay);
}

void drawHeartDisplay_hook(GB *gb) {
  BASE(drawHeartDisplay);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x07;
  CYC(b_+2, b_+4); alu_cp(gb, 0x39);
  if (F & FC) CYCT(b_+4, b_+6);
  else { CYC(b_+4, b_+6); CYC(b_+6, b_+7); E = alu_inc8(gb, E); }
  CYC(b_+7, b_+9); A = alu_srl(gb, A);
  CYC(b_+9, b_+11); A = alu_srl(gb, A);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); A = C;
  CYC(b_+13, b_+15); alu_and(gb, 0x03);
  CYC(b_+15, b_+16); D = A;
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+19); A = alu_srl(gb, A);
  CYC(b_+19, b_+21); A = alu_srl(gb, A);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); push_effect(gb, BC);
  CYC(b_+23, b_+24); alu_cp(gb, E);
  if (F & FC) CYCT(b_+24, b_+26);
  else { CYC(b_+24, b_+26); CYC(b_+26, b_+27); C = E; }
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+29); alu_cp(gb, E);
  if (F & FC) CYCT(b_+29, b_+31);
  else { CYC(b_+29, b_+31); CYC(b_+31, b_+32); A = E; }
  CYC(b_+32, b_+33); alu_sub(gb, C);
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+36); A = mem_rd(gb, hFF8B);
  CYC(b_+36, b_+37); alu_or(gb, E);
  CYC(b_+37, b_+38); alu_rrca(gb);
  if (F & FC) CYCT(b_+38, b_+40);
  else { CYC(b_+38, b_+40); CYC(b_+40, b_+41); L = alu_dec8(gb, L); }
  CYC(b_+41, b_+42); push_effect(gb, HL);
  CALL_C(b_+42, drawHeartDisplay__drawHeartDisplayRow_hook, b_+65, b_+45);
  CYC(b_+45, b_+46); SET_HL(pop_effect(gb));
  CYC(b_+46, b_+48); A = 0x20;
  CYC(b_+48, b_+49); push_effect(gb, b_+49); add_a_to_hl(gb);
  CYC(b_+49, b_+50); SET_BC(pop_effect(gb));
  CYC(b_+50, b_+51); A = C;
  CYC(b_+51, b_+52); alu_sub(gb, E);
  if (!(F & FC)) CYCT(b_+52, b_+54);
  else { CYC(b_+52, b_+54); CYC(b_+54, b_+55); alu_xor(gb, A); }
  CYC(b_+55, b_+56); C = A;
  CYC(b_+56, b_+57); A = B;
  CYC(b_+57, b_+58); alu_sub(gb, E);
  CYC(b_+58, b_+59); alu_sub(gb, C);
  CYC(b_+59, b_+61); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(b_+61, b_+63);
  else { CYC(b_+61, b_+63); CYC(b_+63, b_+64); alu_xor(gb, A); }
  CYC(b_+64, b_+65); B = A;
  TAIL(drawHeartDisplay__drawHeartDisplayRow);
}

void drawHeartDisplay__drawHeartDisplayRow_hook(GB *gb) {
  BASE(drawHeartDisplay);
  CYC(b_+65, b_+66); A = C;
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+67, b_+69); drawHeartDisplay__partiallyFilledHeart_hook(gb); return; }
  CYC(b_+67, b_+69);
  TAIL(drawHeartDisplay__filledHearts);
}

void drawHeartDisplay__filledHearts_hook(GB *gb) {
  BASE(drawHeartDisplay);
  CYC(b_+69, b_+71); A = 0x0f;
  do {
    CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+72, b_+73); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(b_+73, b_+75); else CYC(b_+73, b_+75);
  } while (!(F & FZ));
  TAIL(drawHeartDisplay__partiallyFilledHeart);
}

void drawHeartDisplay__partiallyFilledHeart_hook(GB *gb) {
  BASE(drawHeartDisplay);
  CYC(b_+75, b_+76); A = B;
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+77, b_+79); drawHeartDisplay__fillBlankSpace_hook(gb); return; }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); A = D;
  CYC(b_+80, b_+81); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+81, b_+83); drawHeartDisplay__unfilledHearts_hook(gb); return; }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); alu_add(gb, 0x0b);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+86, b_+88); D = 0;
  CYC(b_+88, b_+89); B = alu_dec8(gb, B);
  TAIL(drawHeartDisplay__unfilledHearts);
}

void drawHeartDisplay__unfilledHearts_hook(GB *gb) {
  BASE(drawHeartDisplay);
  CYC(b_+89, b_+90); A = B;
  CYC(b_+90, b_+91); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+91, b_+93); drawHeartDisplay__fillBlankSpace_hook(gb); return; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); A = 0x0b;
  do {
    CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+96, b_+97); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+97, b_+99); else CYC(b_+97, b_+99);
  } while (!(F & FZ));
  TAIL(drawHeartDisplay__fillBlankSpace);
}

void drawHeartDisplay__fillBlankSpace_hook(GB *gb) {
  BASE(drawHeartDisplay);
  CYC(b_+99, b_+101); A = mem_rd(gb, hFF8B);
  CYC(b_+101, b_+102); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; }
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+105); C = 0x08;
  do {
    CYC(b_+105, b_+106); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+106, b_+107); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(b_+107, b_+109); else CYC(b_+107, b_+109);
  } while (!(F & FZ));
  CYC(b_+109, b_+110); ret_effect(gb);
}

void loadItemIconGfx_hook(GB *gb) {
  BASE(loadItemIconGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); D = (uint8_t)(w4ItemIconGfx >> 8);
  CYC(b_+2, b_+3); push_effect(gb, BC);
  CYC(b_+3, b_+4); A = B;
  CALL_C(b_+4, loadItemIconGfx__func_hook, b_+9, b_+7);
  CYC(b_+7, b_+8); SET_BC(pop_effect(gb));
  CYC(b_+8, b_+9); A = C;
  TAIL(loadItemIconGfx__func);
}

void loadItemIconGfx__func_hook(GB *gb) {
  BASE(loadItemIconGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); loadItemIconGfx__clear_hook(gb); return; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_cp(gb, 0xa3);
  if (F & FC) CYCT(b_+14, b_+16);
  else { CYC(b_+14, b_+16); CYC(b_+16, b_+18); alu_add(gb, 0x02); }
  CYC(b_+18, b_+19); alu_add(gb, A);
  CALL_C(b_+19, multiplyABy16_hook, SYM(multiplyABy16), b_+22);
  CYC(b_+22, b_+25); SET_HL(SYM(checkDisplayDmgModeScreen));
  CYC(b_+25, b_+26); alu_add_hl(gb, BC);
  CYC(b_+26, b_+28); B = 0x19;
  CYC(b_+28, b_+31); TAIL(copy20BytesFromBank);
}

void loadItemIconGfx__clear_hook(GB *gb) {
  BASE(loadItemIconGfx);
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+33); L = E;
  CYC(b_+33, b_+35); B = 0x20;
  CYC(b_+35, b_+37); A = 0xff;
  CYC(b_+37, b_+40); TAIL(fillMemory);
}

void loadStatusBarMap_hook(GB *gb) {
  BASE(loadStatusBarMap);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CYC(b_+2, b_+5); A = W8(wLinkMaxHealth);
  CYC(b_+5, b_+7); alu_cp(gb, 0x39);
  if (F & FC) CYCT(b_+7, b_+9);
  else { CYC(b_+7, b_+9); CYC(b_+9, b_+10); C = alu_inc8(gb, C); }
  CYC(b_+10, b_+13); A = W8(wInventoryB);
  CYC(b_+13, b_+15); alu_cp(gb, 0x0c);
  if (!(F & FZ)) CYCT(b_+15, b_+17);
  else { CYC(b_+15, b_+17); CYC(b_+17, b_+19); C |= 0x80; }
  CYC(b_+19, b_+22); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+23, b_+24); alu_rrca(gb);
  CYC(b_+24, b_+25); A = C;
  if (F & FC) CYCT(b_+25, b_+27);
  else {
    CYC(b_+25, b_+27);
    CYC(b_+27, b_+28); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+28, b_+29); ret_effect(gb); return; }
    CYC(b_+28, b_+29);
  }
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xff);
  CYC(b_+32, b_+35); SET_HL(wBItemTreasure);
  CYC(b_+35, b_+37); B = 0x0a;
  CALL_C(b_+37, clearMemory_hook, SYM(clearMemory), b_+40);
  CYC(b_+40, b_+42); alu_bit(gb, 7, C);
  CYC(b_+42, b_+44); A = 0x23;
  if (!(F & FZ)) CYCT(b_+44, b_+46);
  else {
    CYC(b_+44, b_+46);
    CYC(b_+46, b_+47); A = C;
    CYC(b_+47, b_+49); alu_and(gb, 0x01);
    CYC(b_+49, b_+51); alu_add(gb, 0x21);
  }
  CYC(b_+51, b_+54); TAIL(loadGfxHeader);
}

void runInventoryMenu_hook(GB *gb) {
  BASE(runInventoryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CYC(b_+3, b_+5); A = 0x10;
  CYC(b_+5, b_+7); H8(hOamTail) = A;
  CYC(b_+7, b_+9); A = 0x04;
  CYC(b_+9, b_+11); hram_wr(gb, R_SVBK, A);
  CYC(b_+11, b_+14); push_effect(gb, b_+14);
  run_inventory_menu_states(gb);
  CALL_C(b_+14, inventoryMenuDrawSprites_hook, SYM(inventoryMenuDrawSprites), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20); hram_wr(gb, R_SVBK, A);
  CYC(b_+20, b_+23); TAIL(updateStatusBar);
}

void runInventoryMenu__inventoryMenuStates_hook(GB *gb) {
  BASE(runInventoryMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+23, b_+26); A = W8(wMenuActiveState);
  CYC(b_+26, b_+27); push_effect(gb, b_+27);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(inventoryMenuState0) && hook_enabled_at(gb, SYM(inventoryMenuState0))) { inventoryMenuState0_hook(gb); return; }
    else if (jt_ == SYM(inventoryMenuState1) && hook_enabled_at(gb, SYM(inventoryMenuState1))) { inventoryMenuState1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void showItemText1_hook(GB *gb) {
  BASE(showItemText1);
  CYC(b_+0, b_+3); SET_HL(w4SubscreenTextIndices);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); add_a_to_hl(gb);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  TAIL(showItemText2);
}

void showItemText2_hook(GB *gb) {
  BASE(showItemText2);
  CYC(b_+0, b_+3); SET_HL(wInventory_activeText);
  CYC(b_+3, b_+4); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); B = 0x09;
  CYC(b_+9, b_+11); alu_bit(gb, 7, C);
  if (F & FZ) { CYCT(b_+11, b_+13); goto show_text; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); B = 0x30;
  CYC(b_+15, b_+17); C = 0xc0;
  CYC(b_+17, b_+19); alu_and(gb, 0x3f);
  CYC(b_+19, b_+20); L = A;
  CYC(b_+20, b_+22); alu_add(gb, 0x40);
  CYC(b_+22, b_+24); alu_bit(gb, 6, C);
  CYC(b_+24, b_+25); C = A;
  if (F & FZ) { CYCT(b_+25, b_+27); goto show_text; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+30); mem_wr(gb, wTextSubstitutions + 2, A);
  CYC(b_+30, b_+31); A = L;
  CYC(b_+31, b_+33); alu_add(gb, 0x80);
  CYC(b_+33, b_+36); mem_wr(gb, wTextSubstitutions + 3, A);
  CYC(b_+36, b_+38); C = 0xc1;
show_text:
  CYC(b_+38, b_+41); TAIL(showTextOnInventoryMenu);
}

void inventoryMenuState0_hook(GB *gb) {
  BASE(inventoryMenuState0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wInventorySubmenu2CursorPos);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x08);
  if (!(F & FC)) CYCT(b_+6, b_+8);
  else { CYC(b_+6, b_+8); CYC(b_+8, b_+10); mem_wr(gb, HL, 0); }
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+14); W8(wInventorySubmenu) = A;
  CYC(b_+14, b_+17); W8(wInventory_cbba) = A;
  CYC(b_+17, b_+20); W8(wInventory_submenu2CursorPos2) = A;
  CYC(b_+20, b_+21); A = alu_dec8(gb, A);
  CYC(b_+21, b_+24); W8(wInventory_activeText) = A;
  CALL_C(b_+24, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+27);
  CYC(b_+27, b_+29); A = 0x08;
  CALL_C(b_+29, loadGfxHeader_hook, SYM(loadGfxHeader), b_+32);
  CYC(b_+32, b_+34); A = 0x06;
  CALL_C(b_+34, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+37);
  CYC(b_+37, b_+39); A = 0x0a;
  CALL_C(b_+39, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+42);
  CYC(b_+42, b_+45); SET_HL((SYM(label_02_038) + 24));
  CYC(b_+45, b_+47); E = 0x3f;
  CALL_C(b_+47, interBankCall_hook, 0x008a, b_+50);
  CALL_C(b_+50, func_02_55b2_hook, SYM(func_02_55b2), b_+53);
  CYC(b_+53, b_+55); A = 0x01;
  CYC(b_+55, b_+58); W8(wMenuActiveState) = A;
  CALL_C(b_+58, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+61);
  CYC(b_+61, b_+63); A = 0x03;
  CYC(b_+63, b_+66); TAIL(loadGfxRegisterStateIndex);
}

void func_02_55a8_hook(GB *gb) {
  BASE(func_02_55a8);
  CYC(b_+0, b_+3); A = W8(wInventory_cbba);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CYC(b_+5, b_+7); alu_add(gb, 0x04);
  CYC(b_+7, b_+10); TAIL(loadUncompressedGfxHeader);
}

void func_02_55b2_hook(GB *gb) {
  BASE(func_02_55b2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w4SubscreenTextIndices);
  CYC(b_+3, b_+5); B = 0x20;
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CALL_C(b_+9, showItemText2_hook, SYM(showItemText2), b_+12);
  CYC(b_+12, b_+15); SET_HL(SYM(func_02_55a8));
  CYC(b_+15, b_+16); push_effect(gb, HL);
  CYC(b_+16, b_+19); A = W8(wInventorySubmenu);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+26) { func_02_55b2__subScreen0_hook(gb); func_02_55a8_hook(gb); return; }
    else if (jt_ == b_+39) { func_02_55b2__subScreen1_hook(gb); func_02_55a8_hook(gb); return; }
    else if (jt_ == b_+47) { func_02_55b2__subScreen2_hook(gb); func_02_55a8_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void func_02_55b2__subScreen0_hook(GB *gb) {
  BASE(func_02_55b2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+26, b_+28); A = 0xff;
  CYC(b_+28, b_+31); W8(wStatusBarNeedsRefresh) = A;
  CYC(b_+31, b_+33); A = 0x09;
  CALL_C(b_+33, loadGfxHeader_hook, SYM(loadGfxHeader), b_+36);
  CYC(b_+36, b_+39); TAIL(inventorySubscreen0_drawStoredItems);
}

void func_02_55b2__subScreen1_hook(GB *gb) {
  BASE(func_02_55b2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+39, b_+41); A = 0x0a;
  CALL_C(b_+41, loadGfxHeader_hook, SYM(loadGfxHeader), b_+44);
  CYC(b_+44, b_+47); TAIL(inventorySubscreen1_drawTreasures);
}

void func_02_55b2__subScreen2_hook(GB *gb) {
  BASE(func_02_55b2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+47, b_+49); A = 0x0b;
  CALL_C(b_+49, loadGfxHeader_hook, SYM(loadGfxHeader), b_+52);
  CYC(b_+52, b_+55); TAIL(inventorySubscreen2_drawTreasures);
}

void inventoryMenuState1_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wKeysJustPressed);
  CYC(b_+8, b_+10); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); closeMenu_hook(gb); return; }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); alu_bit(gb, 2, A);
  CYC(b_+15, b_+17); A = 0x03;
  if (!(F & FZ)) { CYCT(b_+17, b_+19); inventoryMenuState1__func_02_5606_hook(gb); return; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = W8(wInventorySubmenu);
  CYC(b_+22, b_+23); push_effect(gb, b_+23);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+36) { inventoryMenuState1__subscreen0_hook(gb); return; }
    else if (jt_ == b_+217) { inventoryMenuState1__subscreen1_hook(gb); return; }
    else if (jt_ == b_+274) { inventoryMenuState1__subscreen2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void inventoryMenuState1__func_02_5606_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+29, b_+32); SET_HL(wMenuActiveState);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0);
  CYC(b_+35, b_+36); ret_effect(gb);
}

void inventoryMenuState1__subscreen0_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+36, b_+39); A = W8(wKeysJustPressed);
  CYC(b_+39, b_+40); C = A;
  CYC(b_+40, b_+42); A = (uint8_t)wInventoryB;
  CYC(b_+42, b_+44); alu_bit(gb, 1, C);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); inventoryMenuState1__aOrB_hook(gb); return; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  CYC(b_+47, b_+49); alu_bit(gb, 0, C);
  if (!(F & FZ)) { CYCT(b_+49, b_+51); inventoryMenuState1__aOrB_hook(gb); return; }
  CYC(b_+49, b_+51);
  CALL_C(b_+51, inventorySubscreen0CheckDirectionButtons_hook, SYM(inventorySubscreen0CheckDirectionButtons), b_+54);
  CYC(b_+54, b_+57); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+57, b_+60); SET_HL(wInventoryStorage);
  CYC(b_+60, b_+61); push_effect(gb, b_+61); add_a_to_hl(gb);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CALL_C(b_+62, loadTreasureDisplayData_b00_hook, SYM(loadTreasureDisplayData_b00), b_+65);
  CYC(b_+65, b_+67); A = 0x06;
  CYC(b_+67, b_+68); push_effect(gb, b_+68); add_a_to_hl(gb);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CALL_C(b_+69, showItemText2_hook, SYM(showItemText2), b_+72);
  CYC(b_+72, b_+75); TAIL(inventorySubscreen0_drawCursor);
}

void inventoryMenuState1__aOrB_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+75, b_+78); W8(wInventory_cbb6) = A;
  CYC(b_+78, b_+81); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+81, b_+84); SET_HL(wInventoryStorage);
  CYC(b_+84, b_+85); push_effect(gb, b_+85); add_a_to_hl(gb);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+89); W8(wInventory_selectedItem) = A;
  CYC(b_+89, b_+91); C = 0x1f;
  CYC(b_+91, b_+93); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(b_+93, b_+95); inventoryMenuState1__hasSubmenu_hook(gb); return; }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(b_+97, b_+99); inventoryMenuState1__hasSubmenu_hook(gb); return; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); alu_cp(gb, 0x11);
  if (!(F & FZ)) { CYCT(b_+101, b_+103); inventoryMenuState1__finalizeEquip_hook(gb); return; }
  CYC(b_+101, b_+103);
  CYC(b_+103, b_+105); C = 0xe0;
  TAIL(inventoryMenuState1__hasSubmenu);
}

void inventoryMenuState1__hasSubmenu_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+105, b_+108); A = mem_rd(gb, wObtainedTreasureFlags + 4);
  CYC(b_+108, b_+109); alu_and(gb, C);
  CALL_C(b_+109, getNumSetBits_hook, SYM(getNumSetBits), b_+112);
  CYC(b_+112, b_+115); W8(wInventory_cbb8) = A;
  CYC(b_+115, b_+117); alu_cp(gb, 0x02);
  CYC(b_+117, b_+119); A = 0x02;
  if (!(F & FC)) { CYCT(b_+119, b_+122); inventoryMenuState1__func_02_5606_hook(gb); return; }
  CYC(b_+119, b_+122);
  TAIL(inventoryMenuState1__finalizeEquip);
}

void inventoryMenuState1__finalizeEquip_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+122, inventoryMenuState1__equipItem_hook, b_+144, b_+125);
  CALL_C(b_+125, inventorySubscreen0_drawStoredItems_hook, SYM(inventorySubscreen0_drawStoredItems), b_+128);
  CALL_C(b_+128, inventorySubscreen0_drawCursor_hook, SYM(inventorySubscreen0_drawCursor), b_+131);
  CYC(b_+131, b_+133); A = 0x56;
  CALL_C(b_+133, playSound_b00_hook, SYM(playSound_b00), b_+136);
  CYC(b_+136, b_+138); A = 0x01;
  CALL_C(b_+138, inventoryMenuState1__func_02_5606_hook, b_+29, b_+141);
  CYC(b_+141, b_+144); TAIL(func_02_55b2);
}

void inventoryMenuState1__equipItem_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+144, b_+146); D = (uint8_t)(wInventoryStorage >> 8);
  CYC(b_+146, b_+147); H = D;
  CYC(b_+147, b_+150); A = W8(wInventory_cbb6);
  CYC(b_+150, b_+151); E = A;
  CYC(b_+151, b_+154); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+154, b_+156); alu_add(gb, (uint8_t)wInventoryStorage);
  CYC(b_+156, b_+157); L = A;
  CYC(b_+157, b_+159); B = 0x0c;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL);
  CYC(b_+160, b_+161); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+161, b_+163); inventoryMenuState1__equipItem__equipBiggoron_hook(gb); return; }
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+165); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+165, b_+167); inventoryMenuState1__equipItem__swapItems_hook(gb); return; }
  CYC(b_+165, b_+167);
  TAIL(inventoryMenuState1__equipItem__unequipBiggoron);
}

void inventoryMenuState1__equipItem__unequipBiggoron_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+167, b_+168); C = L;
  CYC(b_+168, b_+170); L = (uint8_t)wInventoryB;
  CYC(b_+170, b_+171); alu_xor(gb, A);
  CYC(b_+171, b_+172); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+172, b_+173); mem_wr(gb, HL, A);
  CYC(b_+173, b_+174); L = C;
  CYC(b_+174, b_+175); A = B;
  CYC(b_+175, b_+176); mem_wr(gb, DE, A);
  TAIL(inventoryMenuState1__equipItem__swapItems);
}

void inventoryMenuState1__equipItem__swapItems_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); C = A;
  CYC(b_+178, b_+179); A = mem_rd(gb, HL);
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);
  CYC(b_+180, b_+181); mem_wr(gb, HL, C);
  CYC(b_+181, b_+182); ret_effect(gb);
}

void inventoryMenuState1__equipItem__equipBiggoron_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0);
  CALL_C(b_+184, inventoryMenuState1__equipItem__swapItems_hook, b_+176, b_+187);
  CYC(b_+187, b_+190); A = W8(wInventoryB);
  CALL_C(b_+190, inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook, b_+205, b_+193);
  CYC(b_+193, b_+196); A = W8(wInventoryA);
  CALL_C(b_+196, inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook, b_+205, b_+199);
  CYC(b_+199, b_+201); L = (uint8_t)wInventoryB;
  CYC(b_+201, b_+202); mem_wr(gb, HL, B);
  CYC(b_+202, b_+203); L = alu_inc8(gb, L);
  CYC(b_+203, b_+204); mem_wr(gb, HL, B);
  CYC(b_+204, b_+205); ret_effect(gb);
}

void inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+205, b_+206); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+206, b_+207); ret_effect(gb); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+208); C = A;
  CYC(b_+208, b_+210); L = (uint8_t)wInventoryStorage;
  do {
    CYC(b_+210, b_+211); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+211, b_+212); alu_or(gb, A);
    if (!(F & FZ)) CYCT(b_+212, b_+214); else CYC(b_+212, b_+214);
  } while (!(F & FZ));
  CYC(b_+214, b_+215); L = alu_dec8(gb, L);
  CYC(b_+215, b_+216); mem_wr(gb, HL, C);
  CYC(b_+216, b_+217); ret_effect(gb);
}

void inventoryMenuState1__subscreen1_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+217, b_+220); A = W8(wKeysJustPressed);
  CYC(b_+220, b_+222); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+222, b_+224);
    CALL_C(b_+229, inventoryMenuState1__checkEquipRing_hook, b_+244, b_+232);
  } else {
    CYC(b_+222, b_+224);
    CALL_C(b_+224, inventorySubmenu1CheckDirectionButtons_hook, SYM(inventorySubmenu1CheckDirectionButtons), b_+227);
    CYC(b_+227, b_+229);
  }
  CALL_C(b_+232, inventorySubmenu1_drawCursor_hook, SYM(inventorySubmenu1_drawCursor), b_+235);
  CYC(b_+235, b_+238); A = W8(wInventorySubmenu1CursorPos);
  CALL_C(b_+238, showItemText1_hook, SYM(showItemText1), b_+241);
  CYC(b_+241, b_+244); TAIL(drawEquippedSpriteForActiveRing);
}

void inventoryMenuState1__checkEquipRing_hook(GB *gb) {
  BASE(inventoryMenuState1);
  CYC(b_+244, b_+247); A = W8(wInventorySubmenu1CursorPos);
  CYC(b_+247, b_+249); alu_sub(gb, 0x10);
  if (F & FC) { CYCT(b_+249, b_+250); ret_effect(gb); return; }
  CYC(b_+249, b_+250);
  CYC(b_+250, b_+253); SET_HL(wActiveRing);
  CYC(b_+253, b_+254); C = mem_rd(gb, HL);
  CYC(b_+254, b_+256); L = (uint8_t)wRingBoxContents;
  CYC(b_+256, b_+257); push_effect(gb, b_+257); add_a_to_hl(gb);
  CYC(b_+257, b_+258); A = mem_rd(gb, HL);
  CYC(b_+258, b_+259); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+259, b_+261); goto set_ring; }
  CYC(b_+259, b_+261);
  CYC(b_+261, b_+263); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+263, b_+264); ret_effect(gb); return; }
  CYC(b_+263, b_+264);
  CYC(b_+264, b_+266); A = 0xff;
set_ring:
  CYC(b_+266, b_+269); W8(wActiveRing) = A;
  CYC(b_+269, b_+271); A = 0x56;
  CYC(b_+271, b_+274); TAIL(playSound_b00);
}

void inventoryMenuState1__subscreen2_hook(GB *gb) {
  BASE(inventoryMenuState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+274, b_+277); A = W8(wKeysJustPressed);
  CYC(b_+277, b_+279); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+279, b_+281); goto check_direction; }
  CYC(b_+279, b_+281);
  CYC(b_+281, b_+284); A = W8(wInventorySubmenu2CursorPos);
  CYC(b_+284, b_+285); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+285, b_+287); goto check_direction; }
  CYC(b_+285, b_+287);
  CYC(b_+287, b_+290); A = W8(wInventory_submenu2CursorPos2);
  CYC(b_+290, b_+292); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+292, b_+294); goto check_direction; }
  CYC(b_+292, b_+294);
  CYC(b_+294, b_+295); A = alu_inc8(gb, A);
  CYC(b_+295, b_+298); W8(wOpenedMenuType) = A;
  CYC(b_+298, b_+300); A = 0x56;
  CALL_C(b_+300, playSound_b00_hook, SYM(playSound_b00), b_+303);
  CYC(b_+303, b_+306); SET_HL(wInventory);
  CYC(b_+306, b_+308); B = 0x10;
  CYC(b_+308, b_+311); clearMemory_hook(gb);
  return;
check_direction:
  CALL_C(b_+311, inventorySubmenu2CheckDirectionButtons_hook, SYM(inventorySubmenu2CheckDirectionButtons), b_+314);
  CYC(b_+314, b_+317); A = W8(wInventorySubmenu2CursorPos);
  CYC(b_+317, b_+319); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(b_+319, b_+321);
  else {
    CYC(b_+319, b_+321);
    CYC(b_+321, b_+324); A = W8(wInventory_submenu2CursorPos2);
    CYC(b_+324, b_+326); alu_add(gb, 0x08);
  }
  CALL_C(b_+326, showItemText1_hook, SYM(showItemText1), b_+329);
  CYC(b_+329, b_+332); TAIL(inventorySubmenu2_drawCursor);
}

void runRingMenu_hook(GB *gb) {
  BASE(runRingMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CYC(b_+3, b_+5); A = 0x10;
  CYC(b_+5, b_+7); H8(hOamTail) = A;
  CYC(b_+7, b_+10); SET_HL(wTextboxFlags);
  CYC(b_+10, b_+12); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+12, b_+14); A = 0x04;
  CYC(b_+14, b_+16); hram_wr(gb, R_SVBK, A);
  CYC(b_+16, b_+19); push_effect(gb, b_+19);
  run_ring_menu_state_code(gb);
  CYC(b_+19, b_+22); A = W8(wRingMenu_mode);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+27); TAIL(updateStatusBar);
}

void runRingMenu__runStateCode_hook(GB *gb) {
  BASE(runRingMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+27, b_+30); A = W8(wMenuActiveState);
  CYC(b_+30, b_+31); push_effect(gb, b_+31);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(ringMenu_state0) && hook_enabled_at(gb, SYM(ringMenu_state0))) { ringMenu_state0_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_state1) && hook_enabled_at(gb, SYM(ringMenu_state1))) { ringMenu_state1_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_state2) && hook_enabled_at(gb, SYM(ringMenu_state2))) { ringMenu_state2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ringMenu_state0_hook(GB *gb) {
  BASE(ringMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+3);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+7); W8(wRingMenu_tileMapIndex) = A;
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  CYC(b_+8, b_+11); W8(wRingMenu_ringNameTextIndex) = A;
  CYC(b_+11, b_+13); A = 0x80;
  CYC(b_+13, b_+16); W8(wRingMenu_boxCursorFlickerCounter) = A;
  CYC(b_+16, b_+19); A = W8(wRingMenu_mode);
  CYC(b_+19, b_+21); alu_add(gb, 0x3a);
  CALL_C(b_+21, loadGfxHeader_hook, SYM(loadGfxHeader), b_+24);
  CYC(b_+24, b_+26); A = 0x0a;
  CALL_C(b_+26, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+29);
  CYC(b_+29, b_+32); SET_HL(GV((SYM(func_02_465c) + 19), 0x4675));
  CYC(b_+32, b_+34); E = 0x3f;
  CALL_C(b_+34, interBankCall_hook, 0x008a, b_+37);
  CALL_C(b_+37, ringMenu_calculateNumPagesForUnappraisedRings_hook, SYM(ringMenu_calculateNumPagesForUnappraisedRings), b_+40);
  CALL_C(b_+40, ringMenu_redrawRingListOrUnappraisedRings_hook, SYM(ringMenu_redrawRingListOrUnappraisedRings), b_+43);
  CYC(b_+43, b_+46); SET_HL(wMenuActiveState);
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+47, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+50);
  CYC(b_+50, b_+52); A = 0x05;
  CYC(b_+52, b_+54); H8(hNextLcdInterruptBehaviour) = A;
  CYC(b_+54, b_+57); A = W8(wRingMenu_mode);
  CYC(b_+57, b_+59); alu_add(gb, 0x0f);
  CYC(b_+59, b_+62); TAIL(loadGfxRegisterStateIndex);
}

void ringMenu_copyTilemapToVram_hook(GB *gb) {
  BASE(ringMenu_copyTilemapToVram);
  CYC(b_+0, b_+3); SET_HL(wRingMenu_mode);
  CYC(b_+3, b_+6); A = W8(wRingMenu_tileMapIndex);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  CYC(b_+8, b_+9); alu_add(gb, A);
  CYC(b_+9, b_+10); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+12); alu_add(gb, 0x12);
  CYC(b_+12, b_+15); TAIL(loadUncompressedGfxHeader);
}

void ringMenu_redrawRingListOrUnappraisedRings_hook(GB *gb) {
  BASE(ringMenu_redrawRingListOrUnappraisedRings);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, showItemText2_hook, SYM(showItemText2), b_+4);
  CYC(b_+4, b_+7); SET_HL(SYM(ringMenu_copyTilemapToVram));
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CYC(b_+8, b_+11); A = W8(wRingMenu_mode);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(ringMenu_drawRingBox)) { ringMenu_drawRingBox_hook(gb); ringMenu_copyTilemapToVram_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ringMenu_drawRingBox_hook(GB *gb) {
  BASE(ringMenu_drawRingBox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wMenuActiveState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+9); A = W8(wRingBoxLevel);
    CYC(b_+9, b_+10); A = alu_inc8(gb, A);
    CALL_C(b_+10, mapMenu_performTileSubstitutions_hook, SYM(mapMenu_performTileSubstitutions), b_+13);
    CYC(b_+13, b_+16); SET_DE(w4TileMap + 0x201);
    CYC(b_+16, b_+18); A = 0xfe;
    CALL_C(b_+18, getRingTiles_hook, SYM(getRingTiles), b_+21);
  } else {
    CYCT(b_+4, b_+6);
  }
  CALL_C(b_+21, ringMenu_drawRingBoxContents_hook, SYM(ringMenu_drawRingBoxContents), b_+24);
  CYC(b_+24, b_+26); A = 0x04;
  CYC(b_+26, b_+29); W8(wRingMenu_numPages) = A;
  CYC(b_+29, b_+31); A = 0xfe;
  CYC(b_+31, b_+34); W8(wRingMenu_displayedRingNumberComparator) = A;
  CYC(b_+34, b_+37); TAIL(ringMenu_drawRingList);
}

void ringMenu_state1_hook(GB *gb) {
  BASE(ringMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wRingMenu_mode);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(ringMenu_state1_unappraisedRings) && hook_enabled_at(gb, SYM(ringMenu_state1_unappraisedRings))) { ringMenu_state1_unappraisedRings_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_state1_ringList) && hook_enabled_at(gb, SYM(ringMenu_state1_ringList))) { ringMenu_state1_ringList_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ringMenu_state1_unappraisedRings_hook(GB *gb) {
  BASE(ringMenu_state1_unappraisedRings);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_drawSprites_hook, SYM(ringMenu_drawSprites), b_+3);
  CYC(b_+3, b_+6); A = W8(wSubmenuState);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(ringMenu_unappraisedRings_state0) && hook_enabled_at(gb, SYM(ringMenu_unappraisedRings_state0))) { ringMenu_unappraisedRings_state0_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_unappraisedRings_state1) && hook_enabled_at(gb, SYM(ringMenu_unappraisedRings_state1))) { ringMenu_unappraisedRings_state1_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_unappraisedRings_state2) && hook_enabled_at(gb, SYM(ringMenu_unappraisedRings_state2))) { ringMenu_unappraisedRings_state2_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_unappraisedRings_state3) && hook_enabled_at(gb, SYM(ringMenu_unappraisedRings_state3))) { ringMenu_unappraisedRings_state3_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_unappraisedRings_state4) && hook_enabled_at(gb, SYM(ringMenu_unappraisedRings_state4))) { ringMenu_unappraisedRings_state4_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_unappraisedRings_state5) && hook_enabled_at(gb, SYM(ringMenu_unappraisedRings_state5))) { ringMenu_unappraisedRings_state5_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ringMenu_unappraisedRings_state0_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); A = 0x04;
  if (F & FZ) CALL_C_CC(b_+6, ringMenu_setDisplayedText_hook, SYM(ringMenu_setDisplayedText), b_+9);
  else CYC(b_+6, b_+9);
  CYC(b_+9, b_+12); A = W8(wKeysJustPressed);
  CYC(b_+12, b_+14); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); ringMenu_unappraisedRings_state0__bPressed_hook(gb); return; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); ringMenu_unappraisedRings_state0__aPressed_hook(gb); return; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+25); ringMenu_initiateScrollRight_hook(gb); return; }
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+28); TAIL(ringMenu_checkRingListCursorMoved);
}

void ringMenu_unappraisedRings_state0__bPressed_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+28, ringMenu_checkObtainedRingBox_hook, SYM(ringMenu_checkObtainedRingBox), b_+31);
  CYC(b_+31, b_+33); A = 0x12;
  if (F & FZ) { CYCT(b_+33, b_+36); ringMenu_setDisplayedText_hook(gb); return; }
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+39); TAIL(closeMenu);
}

void ringMenu_unappraisedRings_state0__aPressed_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+39, ringMenu_updateSelectedRingFromList_hook, SYM(ringMenu_updateSelectedRingFromList), b_+42);
  CALL_C(b_+42, ringMenu_getUnappraisedRingIndex_hook, SYM(ringMenu_getUnappraisedRingIndex), b_+45);
  CYC(b_+45, b_+46); alu_rlca(gb);
  if (F & FC) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+52); W8(wSubmenuState) = A;
  CALL_C(b_+52, ringMenu_checkObtainedRingBox_hook, SYM(ringMenu_checkObtainedRingBox), b_+55);
  CYC(b_+55, b_+57); A = 0x11;
  if (F & FZ) CYCT(b_+57, b_+59);
  else { CYC(b_+57, b_+59); CYC(b_+59, b_+61); A = 0x05; }
  CYC(b_+61, b_+64); TAIL(ringMenu_setDisplayedText);
}

void ringMenu_unappraisedRings_state1_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_retIfTextIsPrinting_hook, SYM(ringMenu_retIfTextIsPrinting), b_+3);
  CYC(b_+3, b_+6); A = W8(wSelectedTextOption);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); ringMenu_state1_restart_hook(gb); return; }
  CYC(b_+7, b_+9);
  CALL_C(b_+9, ringMenu_checkObtainedRingBox_hook, SYM(ringMenu_checkObtainedRingBox), b_+12);
  if (F & FZ) CYCT(b_+12, b_+14);
  else {
    CYC(b_+12, b_+14);
    CYC(b_+14, b_+16); A = 0x05;
    CALL_C(b_+16, cpRupeeValue_hook, SYM(cpRupeeValue), b_+19);
    CYC(b_+19, b_+21); B = 0x06;
    if (!(F & FZ)) { CYCT(b_+21, b_+24); ringMenu_unappraisedRings_gotoState5_hook(gb); return; }
    CYC(b_+21, b_+24);
    CYC(b_+24, b_+26); A = 0x05;
    CALL_C(b_+26, removeRupeeValue_hook, SYM(removeRupeeValue), b_+29);
  }
  CYC(b_+29, b_+32); SET_HL(wNumRingsAppraised);
  CALL_C(b_+32, incHlRefWithCap_hook, SYM(incHlRefWithCap), b_+35);
  CALL_C(b_+35, ringMenu_getUnappraisedRingIndex_hook, SYM(ringMenu_getUnappraisedRingIndex), b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x40);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+44); W8(wRingMenu_textDelayCounter2) = A;
  CYC(b_+44, b_+46); alu_add(gb, 0x40);
  CYC(b_+46, b_+49); mem_wr(gb, wTextSubstitutions + 2, A);
  CYC(b_+49, b_+52); SET_BC(0x301c);
  CALL_C(b_+52, ringMenu_showExitableText_hook, SYM(ringMenu_showExitableText), b_+55);
  CYC(b_+55, b_+57); A = 0x02;
  CYC(b_+57, b_+60); W8(wSubmenuState) = A;
  CALL_C(b_+60, ringMenu_drawUnappraisedRings_hook, SYM(ringMenu_drawUnappraisedRings), b_+63);
  CYC(b_+63, b_+66); TAIL(ringMenu_copyTilemapToVram);
}

void ringMenu_state1_restart_hook(GB *gb) {
  BASE(ringMenu_state1_restart);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wSubmenuState) = A;
  CYC(b_+4, b_+7); W8(wTextIsActive) = A;
  CYC(b_+7, b_+8); ret_effect(gb);
}

void ringMenu_unappraisedRings_state2_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_retIfTextIsPrinting_hook, SYM(ringMenu_retIfTextIsPrinting), b_+3);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+8); W8(wSubmenuState) = A;
  CALL_C(b_+8, ringMenu_getUnappraisedRingIndex_hook, SYM(ringMenu_getUnappraisedRingIndex), b_+11);
  CYC(b_+11, b_+13); alu_add(gb, 0x80);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+16); B = 0x30;
  CYC(b_+16, b_+18); TAIL(ringMenu_showExitableText);
}

void ringMenu_unappraisedRings_state3_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_retIfTextIsPrinting_hook, SYM(ringMenu_retIfTextIsPrinting), b_+3);
  CALL_C(b_+3, ringMenu_getUnappraisedRingIndex_hook, SYM(ringMenu_getUnappraisedRingIndex), b_+6);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xff);
  CYC(b_+9, b_+12); SET_HL(wRingsObtained);
  CALL_C(b_+12, checkFlag_hook, SYM(checkFlag), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); ringMenu_unappraisedRings_state3__refund_hook(gb); return; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = C;
  CALL_C(b_+18, setFlag_hook, SYM(setFlag), b_+21);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+24); B = 0x17;
  CYC(b_+24, b_+26);
  goto finish;
finish:
  CYC(b_+30, b_+33); W8(wRingMenu_rupeeRefundValue) = A;
  CALL_C(b_+33, ringMenu_checkObtainedRingBox_hook, SYM(ringMenu_checkObtainedRingBox), b_+36);
  if (F & FZ) { CYCT(b_+36, b_+39); closeMenu_hook(gb); return; }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+41); A = 0x28;
  CYC(b_+41, b_+44); W8(wRingMenu_textDelayCounter2) = A;
  CYC(b_+44, b_+46); A = 0x04;
  CYC(b_+46, b_+49); W8(wSubmenuState) = A;
  CYC(b_+49, b_+50); A = B;
  CYC(b_+50, b_+53); TAIL(ringMenu_setDisplayedText);
}

void ringMenu_unappraisedRings_state3__refund_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+26, b_+28); A = 0x07;
  CYC(b_+28, b_+30); B = 0x07;
  CYC(b_+30, b_+33); W8(wRingMenu_rupeeRefundValue) = A;
  CALL_C(b_+33, ringMenu_checkObtainedRingBox_hook, SYM(ringMenu_checkObtainedRingBox), b_+36);
  if (F & FZ) { CYCT(b_+36, b_+39); closeMenu_hook(gb); return; }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+41); A = 0x28;
  CYC(b_+41, b_+44); W8(wRingMenu_textDelayCounter2) = A;
  CYC(b_+44, b_+46); A = 0x04;
  CYC(b_+46, b_+49); W8(wSubmenuState) = A;
  CYC(b_+49, b_+50); A = B;
  CYC(b_+50, b_+53); TAIL(ringMenu_setDisplayedText);
}

void ringMenu_unappraisedRings_state4_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_retIfTextIsPrinting_hook, SYM(ringMenu_retIfTextIsPrinting), b_+3);
  CALL_C(b_+3, ringMenu_retIfCounterNotFinished_hook, SYM(ringMenu_retIfCounterNotFinished), b_+6);
  CYC(b_+6, b_+9); A = W8(wRingMenu_rupeeRefundValue);
  CYC(b_+9, b_+10); alu_or(gb, A);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+13); A = 0x28;
  if (!(F & FZ)) CALL_C_CC(b_+13, giveTreasure_hook, SYM(giveTreasure), b_+16);
  else CYC(b_+13, b_+16);
  CYC(b_+16, b_+19); SET_HL(GV((SYM(label_02_038) + 24), 0x469d));
  CYC(b_+19, b_+21); E = 0x3f;
  CALL_C(b_+21, interBankCall_hook, 0x008a, b_+24);
  CALL_C(b_+24, ringMenu_drawUnappraisedRings_hook, SYM(ringMenu_drawUnappraisedRings), b_+27);
  CALL_C(b_+27, ringMenu_copyTilemapToVram_hook, SYM(ringMenu_copyTilemapToVram), b_+30);
  CYC(b_+30, b_+33); A = W8(wNumRingsAppraised);
  CYC(b_+33, b_+35); alu_cp(gb, 100);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); ringMenu_unappraisedRings_state4__not100th_hook(gb); return; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); A = 0x09;
  CALL_C(b_+39, setGlobalFlag_hook, SYM(setGlobalFlag), b_+42);
  CYC(b_+42, b_+44); B = 0x3c;
  CYC(b_+44, b_+46); TAIL(ringMenu_unappraisedRings_gotoState5);
}

void ringMenu_unappraisedRings_state4__not100th_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state4);
  CYC(b_+46, b_+49); A = W8(wNumUnappraisedRingsBcd);
  CYC(b_+49, b_+50); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+50, b_+53); ringMenu_state1_restart_hook(gb); return; }
  CYC(b_+50, b_+53);
  CYC(b_+53, b_+55); B = 0x02;
  TAIL(ringMenu_unappraisedRings_gotoState5);
}

void ringMenu_unappraisedRings_gotoState5_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_gotoState5);
  CYC(b_+0, b_+2); A = 0x05;
  CYC(b_+2, b_+5); W8(wSubmenuState) = A;
  CYC(b_+5, b_+7); A = 0x3c;
  CYC(b_+7, b_+10); W8(wRingMenu_textDelayCounter2) = A;
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+14); TAIL(ringMenu_setDisplayedText);
}

void ringMenu_showExitableText_hook(GB *gb) {
  BASE(ringMenu_showExitableText);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); W8(wTextboxPosition) = A;
  CYC(b_+5, b_+7); A = 0x09;
  CYC(b_+7, b_+10); W8(wTextboxFlags) = A;
  CYC(b_+10, b_+13); TAIL(showText);
}

void ringMenu_unappraisedRings_state5_hook(GB *gb) {
  BASE(ringMenu_unappraisedRings_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_retIfTextIsPrinting_hook, SYM(ringMenu_retIfTextIsPrinting), b_+3);
  CALL_C(b_+3, ringMenu_retIfCounterNotFinished_hook, SYM(ringMenu_retIfCounterNotFinished), b_+6);
  CYC(b_+6, b_+9); TAIL(closeMenu);
}

void ringMenu_checkObtainedRingBox_hook(GB *gb) {
  BASE(ringMenu_checkObtainedRingBox);
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+5); TAIL(checkGlobalFlag);
}

void ringMenu_getUnappraisedRingIndex_hook(GB *gb) {
  BASE(ringMenu_getUnappraisedRingIndex);
  CYC(b_+0, b_+3); A = W8(wRingMenu_selectedRing);
  CYC(b_+3, b_+6); SET_HL(wUnappraisedRings);
  CYC(b_+6, b_+7); push_effect(gb, b_+7); add_a_to_hl(gb);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void ringMenu_retIfCounterNotFinished_hook(GB *gb) {
  BASE(ringMenu_retIfCounterNotFinished);
  CYC(b_+0, b_+3); SET_HL(wRingMenu_textDelayCounter2);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); SET_AF(pop_effect(gb));
  CYC(b_+8, b_+9); ret_effect(gb);
}

void runSecretListMenu_hook(GB *gb) {
  BASE(runSecretListMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CYC(b_+3, b_+5); A = 0x07;
  CYC(b_+5, b_+7); hram_wr(gb, R_SVBK, A);
  CALL_C(b_+7, runSecretListMenu__runState_hook, b_+13, b_+10);
  CYC(b_+10, b_+13); TAIL(secretListMenu_drawCursorSprite);
}

void runSecretListMenu__runState_hook(GB *gb) {
  BASE(runSecretListMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+13, b_+16); A = W8(wSecretListMenu_state);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(secretListMenu_state0) && hook_enabled_at(gb, SYM(secretListMenu_state0))) { secretListMenu_state0_hook(gb); return; }
    else if (jt_ == SYM(secretListMenu_state1) && hook_enabled_at(gb, SYM(secretListMenu_state1))) { secretListMenu_state1_hook(gb); return; }
    else if (jt_ == SYM(secretListMenu_state2) && hook_enabled_at(gb, SYM(secretListMenu_state2))) { secretListMenu_state2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void secretListMenu_state0_hook(GB *gb) {
  BASE(secretListMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CALL_C(b_+3, stopTextThread_hook, SYM(stopTextThread), b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+11); W8(wSecretListMenu_state) = A;
  CALL_C(b_+11, secretListMenu_state0__clearVramBank_hook, b_+44, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CALL_C(b_+15, secretListMenu_state0__clearVramBank_hook, b_+44, b_+18);
  CYC(b_+18, b_+20); A = 0x05;
  CALL_C(b_+20, loadGfxHeader_hook, SYM(loadGfxHeader), b_+23);
  CYC(b_+23, b_+25); A = GV(0xa8, 0x3f);
  CALL_C(b_+25, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+28);
  CALL_C(b_+28, secretListMenu_loadAllSecretNames_hook, SYM(secretListMenu_loadAllSecretNames), b_+31);
  CYC(b_+31, b_+33); A = 0xff;
  CALL_C(b_+33, secretListMenu_printSecret_hook, SYM(secretListMenu_printSecret), b_+36);
  CALL_C(b_+36, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+39);
  CYC(b_+39, b_+41); A = 0x16;
  CYC(b_+41, b_+44); TAIL(loadGfxRegisterStateIndex);
}

void secretListMenu_state0__clearVramBank_hook(GB *gb) {
  BASE(secretListMenu_state0);
  CYC(b_+44, b_+46); hram_wr(gb, R_VBK, A);
  CYC(b_+46, b_+49); SET_HL(0x8000);
  CYC(b_+49, b_+52); SET_BC(0x1000);
  CYC(b_+52, b_+54); A = 0xff;
  CYC(b_+54, b_+57); TAIL(fillMemoryBc);
}

void secretListMenu_state1_hook(GB *gb) {
  BASE(secretListMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wKeysJustPressed);
  CYC(b_+8, b_+10); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); closeMenu_hook(gb); return; }
  CYC(b_+10, b_+13);
  CALL_C(b_+13, getInputWithAutofire_hook, SYM(getInputWithAutofire), b_+16);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+20); SET_HL(wSecretListMenu_numEntries);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+24); A = 0xff;
  CYC(b_+24, b_+26); alu_bit(gb, 6, C);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); secretListMenu_state1__upOrDown_hook(gb); return; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); alu_bit(gb, 7, C);
  if (F & FZ) { CYCT(b_+30, b_+32); secretListMenu_state1__end_hook(gb); return; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x01;
  TAIL(secretListMenu_state1__upOrDown);
}

void secretListMenu_state1__upOrDown_hook(GB *gb) {
  BASE(secretListMenu_state1);
  CYC(b_+34, b_+35); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+36, b_+38); secretListMenu_state1__end_hook(gb); return; }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+39, b_+40); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+40, b_+42); alu_cp(gb, 0x01);
  if (F & FC) { CYCT(b_+42, b_+44); secretListMenu_state1__scrollUp_hook(gb); return; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+46, b_+48); secretListMenu_state1__playSound_hook(gb); return; }
  CYC(b_+46, b_+48);
  TAIL(secretListMenu_state1__scrollDown);
}

void secretListMenu_state1__scrollDown_hook(GB *gb) {
  BASE(secretListMenu_state1);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+50); alu_sub(gb, B);
  CYC(b_+50, b_+52); alu_cp(gb, 0xfc);
  if (!(F & FC)) { CYCT(b_+52, b_+54); secretListMenu_state1__playSound_hook(gb); return; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); A = 0x02;
  CYC(b_+56, b_+58);
  CYC(b_+64, b_+67); W8(wSecretListMenu_scrollSpeed) = A;
  CYC(b_+67, b_+69); L = (uint8_t)wSecretListMenu_state;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(secretListMenu_state1__playSound);
}

void secretListMenu_state1__scrollUp_hook(GB *gb) {
  BASE(secretListMenu_state1);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+60, b_+62); secretListMenu_state1__playSound_hook(gb); return; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); A = 0xfe;
  CYC(b_+64, b_+67); W8(wSecretListMenu_scrollSpeed) = A;
  CYC(b_+67, b_+69); L = (uint8_t)wSecretListMenu_state;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(secretListMenu_state1__playSound);
}

void secretListMenu_state1__playSound_hook(GB *gb) {
  BASE(secretListMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+70, b_+72); A = 0x84;
  CALL_C(b_+72, playSound_b00_hook, SYM(playSound_b00), b_+75);
  TAIL(secretListMenu_state1__end);
}

void secretListMenu_state1__end_hook(GB *gb) {
  BASE(secretListMenu_state1);
  CYC(b_+75, b_+78); A = W8(wSecretListMenu_cursorIndex);
  CYC(b_+78, b_+80); TAIL(secretListMenu_printSecret);
}

void secretListMenu_state2_hook(GB *gb) {
  BASE(secretListMenu_state2);
  CYC(b_+0, b_+3); SET_HL(wSecretListMenu_scrollSpeed);
  CYC(b_+3, b_+6); A = W8(wGfxRegs2_SCY);
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+10); W8(wGfxRegs2_SCY) = A;
  CYC(b_+10, b_+12); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); A = alu_sra(gb, A);
  CYC(b_+16, b_+18); L = (uint8_t)wSecretListMenu_scroll;
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = (uint8_t)wSecretListMenu_state;
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+23, b_+24); ret_effect(gb);
}

void secretListMenu_drawCursorSprite_hook(GB *gb) {
  BASE(secretListMenu_drawCursorSprite);
  CYC(b_+0, b_+3); A = W8(wGfxRegs2_SCY);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(wSecretListMenu_cursorIndex);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_sub(gb, B);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); C = 0;
  CYC(b_+13, b_+16); SET_HL(b_+19);
  CYC(b_+16, b_+19); TAIL(addSpritesToOam_withOffset);
}

void secretListMenu_printSecret_hook(GB *gb) {
  BASE(secretListMenu_printSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
restart:
  CYC(b_+0, b_+3); SET_HL(wTmpcbb9);
  CYC(b_+3, b_+4); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+7); push_effect(gb, AF);
  CYC(b_+7, b_+10); SET_HL(w7d800);
  CYC(b_+10, b_+13); SET_BC(0x0300);
  CALL_C(b_+13, clearMemoryBc_hook, SYM(clearMemoryBc), b_+16);
  CYC(b_+16, b_+19); SET_HL(w7SecretText1);
  CYC(b_+19, b_+21); B = 0x18;
  CALL_C(b_+21, clearMemory_hook, SYM(clearMemory), b_+24);
  CYC(b_+24, b_+25); SET_AF(pop_effect(gb));
  CYC(b_+25, b_+27); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+27, b_+29); secretListMenu_printSecret__end_hook(gb); return; }
  CYC(b_+27, b_+29);
  CALL_C(b_+29, secretListMenu_getSecretData_hook, SYM(secretListMenu_getSecretData), b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+33, b_+34); alu_rlca(gb);
  CYC(b_+34, b_+35); alu_rlca(gb);
  CYC(b_+35, b_+37); alu_and(gb, 0x03);
  CYC(b_+37, b_+38); B = A;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+39, b_+40); C = mem_rd(gb, HL);
  CALL_C(b_+40, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+43);
  CYC(b_+43, b_+45); A = 0xff;
  CYC(b_+45, b_+48); W8(wFileSelect_fontXor) = A;
  if (F & FZ) { CYCT(b_+48, b_+50); goto restart; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+53); push_effect(gb, b_+53);
  secret_list_get_secret_text(gb);
  CYC(b_+53, b_+56); SET_HL(w7SecretText1);
  CYC(b_+56, b_+59); SET_DE(w7d800);
  CYC(b_+59, b_+61); B = 0x18;
  CALL_C(b_+61, copyTextCharactersFromHl_hook, SYM(copyTextCharactersFromHl), b_+64);
  TAIL(secretListMenu_printSecret__end);
}

void secretListMenu_printSecret__end_hook(GB *gb) {
  BASE(secretListMenu_printSecret);
  CYC(b_+64, b_+66); A = 0x35;
  CYC(b_+66, b_+69); TAIL(loadUncompressedGfxHeader);
}

void secretListMenu_printSecret__getSecretText_hook(GB *gb) {
  BASE(secretListMenu_printSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+69, b_+70); A = B;
  CYC(b_+70, b_+71); push_effect(gb, b_+71);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+79) { secretListMenu_printSecret__val0_hook(gb); return; }
    else if (jt_ == b_+87) { secretListMenu_printSecret__val2_hook(gb); return; }
    else if (jt_ == b_+93) { secretListMenu_printSecret__val3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void secretListMenu_printSecret__val0_hook(GB *gb) {
  BASE(secretListMenu_printSecret);
  CYC(b_+79, b_+82); SET_HL(0x481b);
  CYC(b_+82, b_+84); E = 0x03;
  CYC(b_+84, b_+87); interBankCall_hook(gb);
}

void secretListMenu_printSecret__val2_hook(GB *gb) {
  BASE(secretListMenu_printSecret);
  CYC(b_+87, b_+90); SET_BC(0x0002);
  CYC(b_+90, b_+93); TAIL(secretFunctionCaller);
}

void secretListMenu_printSecret__val3_hook(GB *gb) {
  BASE(secretListMenu_printSecret);
  CYC(b_+93, b_+94); A = C;
  CYC(b_+94, b_+97); W8(wShortSecretIndex) = A;
  CYC(b_+97, b_+98); C = B;
  CYC(b_+98, b_+100); B = 0;
  CYC(b_+100, b_+103); TAIL(secretFunctionCaller);
}

void secretListMenu_loadAllSecretNames_hook(GB *gb) {
  BASE(secretListMenu_loadAllSecretNames);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); hram_wr(gb, R_VBK, A);
  CYC(b_+3, b_+6); SET_DE(0x8a00);
  CYC(b_+6, b_+8); B = 0;
  TAIL(secretListMenu_loadAllSecretNames__nextSecret);
}

void secretListMenu_loadAllSecretNames__nextSecret_hook(GB *gb) {
  BASE(secretListMenu_loadAllSecretNames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+8, b_+9); A = B;
    CALL_C(b_+9, secretListMenu_getSecretData_hook, SYM(secretListMenu_getSecretData), b_+12);
    CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+13, b_+14); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+14, b_+16); secretListMenu_loadAllSecretNames__end_hook(gb); return; }
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); push_effect(gb, BC);
    CYC(b_+17, b_+18); C = A;
    CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(b_+19, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+22);
    CYC(b_+22, b_+24); A = 0x01;
    if (F & FZ) {
      CYCT(b_+24, b_+26);
    } else {
      CYC(b_+24, b_+26);
      CYC(b_+26, b_+27); A = C;
      CYC(b_+27, b_+29); alu_and(gb, 0x3f);
      CALL_C(b_+29, copyTextCharactersFromSecretTextTable_hook, SYM(copyTextCharactersFromSecretTextTable), b_+32);
      CYC(b_+32, b_+34); A = 0x02;
    }
    CALL_C(b_+34, copyTextCharactersFromSecretTextTable_hook, SYM(copyTextCharactersFromSecretTextTable), b_+37);
    CYC(b_+37, b_+38); SET_BC(pop_effect(gb));
    CYC(b_+38, b_+39); SET_DE(DE - 1);
    CYC(b_+39, b_+41); E = 0;
    CYC(b_+41, b_+42); A = D;
    CYC(b_+42, b_+44); alu_and(gb, 0xfe);
    CYC(b_+44, b_+46); alu_add(gb, 0x02);
    CYC(b_+46, b_+48); alu_cp(gb, 0x90);
    if (F & FC) {
      CYCT(b_+48, b_+50);
    } else {
      CYC(b_+48, b_+50);
      CYC(b_+50, b_+52); A = 0x01;
      CYC(b_+52, b_+54); hram_wr(gb, R_VBK, A);
      CYC(b_+54, b_+56); A = 0x80;
    }
    CYC(b_+56, b_+57); D = A;
    CYC(b_+57, b_+58); B = alu_inc8(gb, B);
    CYC(b_+58, b_+60);
  }
}

void secretListMenu_loadAllSecretNames__end_hook(GB *gb) {
  BASE(secretListMenu_loadAllSecretNames);
  CYC(b_+60, b_+61); A = B;
  CYC(b_+61, b_+64); W8(wSecretListMenu_numEntries) = A;
  CYC(b_+64, b_+65); ret_effect(gb);
}

void secretListMenu_getSecretData_hook(GB *gb) {
  BASE(secretListMenu_getSecretData);
  CYC(b_+0, b_+3); SET_HL(wFileIsLinkedGame);
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+5, b_+8); SET_HL(b_+20);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+13); SET_HL(b_+51);
  }
  CYC(b_+13, b_+14); push_effect(gb, BC);
  CYC(b_+14, b_+15); C = A;
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, C);
  CYC(b_+17, b_+18); push_effect(gb, b_+18); add_a_to_hl(gb);
  CYC(b_+18, b_+19); SET_BC(pop_effect(gb));
  CYC(b_+19, b_+20); ret_effect(gb);
}

void ringMenu_state1_ringList_hook(GB *gb) {
  BASE(ringMenu_state1_ringList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_drawRingBoxCursor_hook, SYM(ringMenu_drawRingBoxCursor), b_+3);
  CALL_C(b_+3, ringMenu_drawEquippedRingSprite_hook, SYM(ringMenu_drawEquippedRingSprite), b_+6);
  CALL_C(b_+6, ringMenu_drawSpritesForRingsInBox_hook, SYM(ringMenu_drawSpritesForRingsInBox), b_+9);
  CYC(b_+9, b_+12); A = W8(wSubmenuState);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(ringMenu_ringList_substate0) && hook_enabled_at(gb, SYM(ringMenu_ringList_substate0))) { ringMenu_ringList_substate0_hook(gb); return; }
    else if (jt_ == SYM(ringMenu_ringList_substate1) && hook_enabled_at(gb, SYM(ringMenu_ringList_substate1))) { ringMenu_ringList_substate1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ringMenu_ringList_substate0_hook(GB *gb) {
  BASE(ringMenu_ringList_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wRingMenu_boxCursorFlickerCounter);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); ringMenu_ringList_substate0__aPressed_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRingMenu_textDelayCounter);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto display_ring_text; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); ringMenu_ringList_substate0__checkInput_hook(gb);
  return;
display_ring_text:
  CYC(b_+16, b_+19); A = W8(wRingMenu_ringBoxCursorIndex);
  CYC(b_+19, b_+22); SET_HL(wRingBoxContents);
  CYC(b_+22, b_+23); push_effect(gb, b_+23); add_a_to_hl(gb);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+27); W8(wRingMenu_selectedRing) = A;
  CALL_C(b_+27, ringMenu_updateDisplayedRingNumberWithGivenComparator_hook, SYM(ringMenu_updateDisplayedRingNumberWithGivenComparator), b_+30);
  CALL_C(b_+30, ringMenu_updateRingText_hook, SYM(ringMenu_updateRingText), b_+33);
  TAIL(ringMenu_ringList_substate0__checkInput);
}

void ringMenu_ringList_substate0__checkInput_hook(GB *gb) {
  BASE(ringMenu_ringList_substate0);
  CYC(b_+33, b_+36); A = W8(wKeysJustPressed);
  CYC(b_+36, b_+38); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); ringMenu_ringList_substate0__bPressed_hook(gb); return; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+42, b_+45); ringMenu_checkRingBoxCursorMoved_hook(gb); return; }
  CYC(b_+42, b_+45);
  TAIL(ringMenu_ringList_substate0__aPressed);
}

void ringMenu_ringList_substate0__aPressed_hook(GB *gb) {
  BASE(ringMenu_ringList_substate0);
  CYC(b_+45, b_+46); alu_xor(gb, A);
  CYC(b_+46, b_+49); W8(wRingMenu_boxCursorFlickerCounter) = A;
  CYC(b_+49, b_+50); A = alu_inc8(gb, A);
  CYC(b_+50, b_+53); W8(wSubmenuState) = A;
  CYC(b_+53, b_+55); A = 0x80;
  CYC(b_+55, b_+58); W8(wRingMenu_displayedRingNumberComparator) = A;
  CYC(b_+58, b_+60); A = 0xff;
  CYC(b_+60, b_+63); W8(wRingMenu_descriptionTextIndex) = A;
  CYC(b_+63, b_+64); ret_effect(gb);
}

void ringMenu_ringList_substate0__bPressed_hook(GB *gb) {
  BASE(ringMenu_ringList_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+64, b_+67); A = W8(wActiveRing);
  CALL_C(b_+67, ringMenu_checkRingIsInBox_hook, SYM(ringMenu_checkRingIsInBox), b_+70);
  if (!(F & FC)) { CYCT(b_+70, b_+72); goto close_menu; }
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); A = 0xff;
  CYC(b_+74, b_+77); W8(wActiveRing) = A;
close_menu:
  CYC(b_+77, b_+78); alu_xor(gb, A);
  CYC(b_+78, b_+81); W8(wTextIsActive) = A;
  CYC(b_+81, b_+84); W8(wTextboxFlags) = A;
  CYC(b_+84, b_+87); TAIL(closeMenu);
}

void ringMenu_ringList_substate1_hook(GB *gb) {
  BASE(ringMenu_ringList_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wKeysJustPressed);
  CYC(b_+3, b_+5); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); ringMenu_selectedRingFromList_hook(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ringMenu_moveCursorToRingBox_hook(gb); return; }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); ringMenu_initiateScrollRight_hook(gb); return; }
  CYC(b_+14, b_+17);
  CALL_C(b_+17, ringMenu_checkRingListCursorMoved_hook, SYM(ringMenu_checkRingListCursorMoved), b_+20);
  CALL_C(b_+20, ringMenu_updateSelectedRingFromList_hook, SYM(ringMenu_updateSelectedRingFromList), b_+23);
  CALL_C(b_+23, ringMenu_updateDisplayedRingNumber_hook, SYM(ringMenu_updateDisplayedRingNumber), b_+26);
  CALL_C(b_+26, ringMenu_drawSprites_hook, SYM(ringMenu_drawSprites), b_+29);
  CALL_C(b_+29, ringMenu_retIfCounterNotFinished_hook, SYM(ringMenu_retIfCounterNotFinished), SYM(ringMenu_updateRingText));
  TAIL(ringMenu_updateRingText);
}

void ringMenu_updateRingText_hook(GB *gb) {
  BASE(ringMenu_updateRingText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wRingMenu_selectedRing);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); SET_HL(wRingsObtained);
  CALL_C(b_+7, checkFlag_hook, SYM(checkFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+12); goto name_done; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); A = C;
  CYC(b_+13, b_+15); alu_or(gb, 0x80);
name_done:
  CYC(b_+15, b_+18); SET_HL(wRingMenu_ringNameTextIndex);
  CYC(b_+18, b_+19); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+19, b_+21); goto description; }
  CYC(b_+19, b_+21);
  CALL_C(b_+21, showItemText2_hook, SYM(showItemText2), b_+24);
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+29); W8(wRingMenu_textDelayCounter) = A;
  CYC(b_+29, b_+30); ret_effect(gb);
  return;
description:
  CYC(b_+30, b_+33); A = W8(wRingMenu_selectedRing);
  CYC(b_+33, b_+34); C = A;
  CYC(b_+34, b_+36); alu_cp(gb, 0xff);
  CYC(b_+36, b_+38); A = 0xc0;
  if (F & FZ) { CYCT(b_+38, b_+40); ringMenu_updateRingText__printDescription_hook(gb); return; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); A = C;
  CYC(b_+41, b_+44); SET_HL(wRingsObtained);
  CALL_C(b_+44, checkFlag_hook, SYM(checkFlag), b_+47);
  CYC(b_+47, b_+49); A = 0xc0;
  if (F & FZ) { CYCT(b_+49, b_+51); ringMenu_updateRingText__printDescription_hook(gb); return; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); A = C;
  CYC(b_+52, b_+54); alu_add(gb, 0x80);
  TAIL(ringMenu_updateRingText__printDescription);
}

void ringMenu_updateRingText__printDescription_hook(GB *gb) {
  BASE(ringMenu_updateRingText);
  CYC(b_+54, b_+57); SET_HL(wRingMenu_descriptionTextIndex);
  CYC(b_+57, b_+58); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+58, b_+59); ret_effect(gb); return; }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+60); mem_wr(gb, HL, A);
  CYC(b_+60, b_+61); C = A;
  CYC(b_+61, b_+63); B = 0x30;
  CYC(b_+63, b_+65); A = 0x04;
  CYC(b_+65, b_+68); W8(wTextboxPosition) = A;
  CYC(b_+68, b_+70); A = 0x09;
  CYC(b_+70, b_+73); W8(wTextboxFlags) = A;
  CYC(b_+73, b_+76); TAIL(showTextNonExitable);
}

void ringMenu_selectedRingFromList_hook(GB *gb) {
  BASE(ringMenu_selectedRingFromList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x56;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CALL_C(b_+5, ringMenu_updateSelectedRingFromList_hook, SYM(ringMenu_updateSelectedRingFromList), b_+8);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+12); SET_HL(wRingsObtained);
  CALL_C(b_+12, checkFlag_hook, SYM(checkFlag), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto put_ring_in_box; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); C = 0xff;
put_ring_in_box:
  CYC(b_+19, b_+22); A = W8(wRingMenu_ringBoxCursorIndex);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+24); A = C;
  CALL_C(b_+24, ringMenu_checkRingIsInBox_hook, SYM(ringMenu_checkRingIsInBox), b_+27);
  if (F & FC) { CYCT(b_+27, b_+29); goto write_ring; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0xff);
  CYC(b_+31, b_+32); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+32, b_+34); ringMenu_moveCursorToRingBox_hook(gb); return; }
  CYC(b_+32, b_+34);
write_ring:
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+38); SET_HL(wRingBoxContents);
  CYC(b_+38, b_+39); push_effect(gb, b_+39); add_a_to_hl(gb);
  CYC(b_+39, b_+40); mem_wr(gb, HL, C);
  TAIL(ringMenu_moveCursorToRingBox);
}

void ringMenu_moveCursorToRingBox_hook(GB *gb) {
  BASE(ringMenu_moveCursorToRingBox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wSubmenuState) = A;
  CYC(b_+4, b_+6); A = 0x80;
  CYC(b_+6, b_+9); W8(wRingMenu_boxCursorFlickerCounter) = A;
  CYC(b_+9, b_+11); A = 0xff;
  CYC(b_+11, b_+14); W8(wTextIsActive) = A;
  CYC(b_+14, b_+17); W8(wRingMenu_ringNameTextIndex) = A;
  CYC(b_+17, b_+20); W8(wRingMenu_descriptionTextIndex) = A;
  CALL_C(b_+20, ringMenu_drawRingBoxContents_hook, SYM(ringMenu_drawRingBoxContents), b_+23);
  CYC(b_+23, b_+26); TAIL(ringMenu_copyTilemapToVram);
}

void ringMenu_checkRingIsInBox_hook(GB *gb) {
  BASE(ringMenu_checkRingIsInBox);
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+4); SET_HL(wRingBoxContents + 4);
  CYC(b_+4, b_+6); B = 0x05;
  TAIL(ringMenu_checkRingIsInBox__nextRing);
}

void ringMenu_checkRingIsInBox__nextRing_hook(GB *gb) {
  BASE(ringMenu_checkRingIsInBox);
  for (;;) {
    CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+7, b_+9); ringMenu_checkRingIsInBox__foundRing_hook(gb); return; }
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+10); L = alu_dec8(gb, L);
    CYC(b_+10, b_+11); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+11, b_+13); continue; }
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); SET_BC(pop_effect(gb));
    CYC(b_+14, b_+15); alu_scf(gb);
    CYC(b_+15, b_+16); ret_effect(gb);
    return;
  }
}

void ringMenu_checkRingIsInBox__foundRing_hook(GB *gb) {
  BASE(ringMenu_checkRingIsInBox);
  CYC(b_+16, b_+17); B = alu_dec8(gb, B);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+19); SET_BC(pop_effect(gb));
  CYC(b_+19, b_+20); ret_effect(gb);
}

void ringMenu_initiateScrollRight_hook(GB *gb) {
  BASE(ringMenu_initiateScrollRight);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); W8(wRingMenu_scrollDirection) = A;
  CYC(b_+5, b_+8); W8(wRingMenu_displayedRingNumberComparator) = A;
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12); W8(wRingMenu_ringListCursorIndex) = A;
  CYC(b_+12, b_+15); A = W8(wRingMenu_page);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  TAIL(ringMenu_initiateScroll);
}

void ringMenu_initiateScroll_hook(GB *gb) {
  BASE(ringMenu_initiateScroll);
  CYC(b_+0, b_+3); SET_HL(wRingMenu_numPages);
  CYC(b_+3, b_+4); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+4, b_+6); goto set_page; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
set_page:
  CYC(b_+11, b_+14); W8(wRingMenu_page) = A;
  CYC(b_+14, b_+16); A = 0x02;
  TAIL(ringMenu_setState);
}

void ringMenu_setState_hook(GB *gb) {
  BASE(ringMenu_setState);
  CYC(b_+0, b_+3); SET_HL(wMenuActiveState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+9); W8(wTextIsActive) = A;
  CYC(b_+9, b_+11); A = 0xff;
  CYC(b_+11, b_+14); W8(wRingMenu_descriptionTextIndex) = A;
  CYC(b_+14, b_+15); ret_effect(gb);
}

void ringMenu_state2_hook(GB *gb) {
  BASE(ringMenu_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wRingMenu_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto dispatch; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, ringMenu_drawRingBoxCursor_hook, SYM(ringMenu_drawRingBoxCursor), b_+9);
  CALL_C(b_+9, ringMenu_drawEquippedRingSprite_hook, SYM(ringMenu_drawEquippedRingSprite), b_+12);
dispatch:
  CYC(b_+12, b_+15); A = W8(wSubmenuState);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+20) { ringMenu_state2__substate0_hook(gb); return; }
    else if (jt_ == b_+65) { ringMenu_state2__substate1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ringMenu_state2__substate0_hook(GB *gb) {
  BASE(ringMenu_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+20, b_+23); SET_HL(wRingMenu_tileMapIndex);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_xor(gb, 0x01);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CALL_C(b_+27, ringMenu_redrawRingListOrUnappraisedRings_hook, SYM(ringMenu_redrawRingListOrUnappraisedRings), b_+30);
  CYC(b_+30, b_+33); A = W8(wRingMenu_scrollDirection);
  CYC(b_+33, b_+35); alu_bit(gb, 7, A);
  CYC(b_+35, b_+37); A = 0x9f;
  if (F & FZ) { CYCT(b_+37, b_+39); goto set_window_x; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); SET_HL(wGfxRegs2_LCDC);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_xor(gb, 0x48);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+48); A = 0x98;
  CYC(b_+48, b_+51); W8(wGfxRegs2_SCX) = A;
  CYC(b_+51, b_+53); A = 0x07;
set_window_x:
  CYC(b_+53, b_+56); W8(wGfxRegs2_WINX) = A;
  CYC(b_+56, b_+59); SET_HL(wSubmenuState);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+62); A = 0x54;
  CYC(b_+62, b_+65); TAIL(playSound_b00);
}

void ringMenu_state2__substate1_hook(GB *gb) {
  BASE(ringMenu_state2);
  CYC(b_+65, b_+68); SET_BC(0x089f);
  CYC(b_+68, b_+71); SET_HL(wGfxRegs2_WINX);
  CYC(b_+71, b_+74); SET_DE(wGfxRegs2_SCX);
  CYC(b_+74, b_+77); A = W8(wRingMenu_scrollDirection);
  CYC(b_+77, b_+79); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+79, b_+81); ringMenu_state2__scrollRight_hook(gb); return; }
  CYC(b_+79, b_+81);
  TAIL(ringMenu_state2__scrollLeft);
}

void ringMenu_state2__scrollLeft_hook(GB *gb) {
  BASE(ringMenu_state2);
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+83); alu_add(gb, B);
  CYC(b_+83, b_+84); alu_cp(gb, C);
  if (F & FC) { CYCT(b_+84, b_+86); goto store_left; }
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+87); A = C;
store_left:
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); alu_sub(gb, B);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+93); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+93, b_+94); ret_effect(gb); return; }
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); TAIL(ringMenu_state2__doneScrolling);
}

void ringMenu_state2__scrollRight_hook(GB *gb) {
  BASE(ringMenu_state2);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+98); alu_sub(gb, B);
  CYC(b_+98, b_+100); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+100, b_+102); goto store_right; }
  CYC(b_+100, b_+102);
  CYC(b_+102, b_+104); A = 0x07;
store_right:
  CYC(b_+104, b_+105); mem_wr(gb, HL, A);
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+107); alu_add(gb, B);
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+110); alu_cp(gb, 0x98);
  if (F & FC) { CYCT(b_+110, b_+111); ret_effect(gb); return; }
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+114); A = W8(wGfxRegs2_LCDC);
  CYC(b_+114, b_+116); alu_xor(gb, 0x48);
  CYC(b_+116, b_+119); W8(wGfxRegs2_LCDC) = A;
  TAIL(ringMenu_state2__doneScrolling);
}

void ringMenu_state2__doneScrolling_hook(GB *gb) {
  BASE(ringMenu_state2);
  CYC(b_+119, b_+121); A = 0xc7;
  CYC(b_+121, b_+124); W8(wGfxRegs2_WINX) = A;
  CYC(b_+124, b_+125); alu_xor(gb, A);
  CYC(b_+125, b_+128); W8(wGfxRegs2_SCX) = A;
  CYC(b_+128, b_+130); A = 0x01;
  CYC(b_+130, b_+133); TAIL(ringMenu_setState);
}

void ringMenu_checkRingListCursorMoved_hook(GB *gb) {
  BASE(ringMenu_checkRingListCursorMoved);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+55);
  CALL_C(b_+3, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); SET_HL(wRingMenu_ringListCursorIndex);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); alu_bit(gb, 0, C);
  if (F & FZ) { CYCT(b_+19, b_+21); ringMenu_checkRingListCursorMoved__playSound_hook(gb); return; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 4, B);
  if (F & FZ) { CYCT(b_+23, b_+25); ringMenu_checkRingListCursorMoved__playSound_hook(gb); return; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = E;
  CYC(b_+26, b_+29); W8(wRingMenu_scrollDirection) = A;
  CYC(b_+29, b_+32); A = W8(wRingMenu_page);
  CYC(b_+32, b_+33); alu_add(gb, E);
  CYC(b_+33, b_+35); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto initiate_scroll; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+40); A = W8(wRingMenu_numPages);
  CYC(b_+40, b_+42); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+42, b_+44); ringMenu_checkRingListCursorMoved__playSound_hook(gb); return; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); A = alu_dec8(gb, A);
initiate_scroll:
  CALL_C(b_+45, ringMenu_initiateScroll_hook, SYM(ringMenu_initiateScroll), b_+48);
  TAIL(ringMenu_checkRingListCursorMoved__playSound);
}

void ringMenu_checkRingListCursorMoved__playSound_hook(GB *gb) {
  BASE(ringMenu_checkRingListCursorMoved);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+48, b_+50); A = 0x84;
  CALL_C(b_+50, playSound_b00_hook, SYM(playSound_b00), b_+53);
  CYC(b_+53, b_+54); alu_scf(gb);
  CYC(b_+54, b_+55); ret_effect(gb);
}

void ringMenu_checkRingBoxCursorMoved_hook(GB *gb) {
  BASE(ringMenu_checkRingBoxCursorMoved);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRingBoxCapacity_hook, SYM(getRingBoxCapacity), b_+3);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+7); SET_HL(b_+24);
  CALL_C(b_+7, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+10);
  if (!(F & FC)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_HL(wRingMenu_ringBoxCursorIndex);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); alu_cp(gb, E);
  if (!(F & FC)) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); A = 0x84;
  CYC(b_+21, b_+24); TAIL(playSound_b00);
}

void ringMenu_drawSprites_hook(GB *gb) {
  BASE(ringMenu_drawSprites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wRingMenu_numPages);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+7); SET_HL(b_+46);
  if (!(F & FZ)) CALL_C_CC(b_+7, addSpritesToOam_hook, SYM(addSpritesToOam), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+13); SET_HL(wRingMenu_listCursorFlickerCounter);
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+16, b_+17); ret_effect(gb); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); SET_BC(0x3e20);
  CYC(b_+20, b_+23); A = W8(wRingMenu_ringListCursorIndex);
  CYC(b_+23, b_+25); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+25, b_+27); goto position_cursor; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); B = 0x56;
position_cursor:
  CYC(b_+29, b_+31); alu_and(gb, 0x07);
  CYC(b_+31, b_+33); A = alu_swap(gb, A);
  CYC(b_+33, b_+34); alu_add(gb, C);
  CYC(b_+34, b_+35); C = A;
  CYC(b_+35, b_+38); SET_HL(b_+41);
  CYC(b_+38, b_+41); TAIL(addSpritesToOam_withOffset);
}

void ringMenu_drawEquippedRingSprite_hook(GB *gb) {
  BASE(ringMenu_drawEquippedRingSprite);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveRing);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ringMenu_checkRingIsInBox_hook, SYM(ringMenu_checkRingIsInBox), b_+9);
  if (F & FC) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, ringMenu_getSpriteOffsetForRingBoxPosition_hook, SYM(ringMenu_getSpriteOffsetForRingBoxPosition), b_+13);
  CYC(b_+13, b_+16); SET_HL(b_+19);
  CYC(b_+16, b_+19); TAIL(addSpritesToOam_withOffset);
}

void ringMenu_getSpriteOffsetForRingBoxPosition_hook(GB *gb) {
  BASE(ringMenu_getSpriteOffsetForRingBoxPosition);
  CYC(b_+0, b_+3); SET_HL(b_+8);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); add_a_to_hl(gb);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+7); B = 0;
  CYC(b_+7, b_+8); ret_effect(gb);
}

void ringMenu_drawRingBoxCursor_hook(GB *gb) {
  BASE(ringMenu_drawRingBoxCursor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wRingMenu_boxCursorFlickerCounter);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto draw_cursor; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x10);
  CYC(b_+10, b_+12); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
draw_cursor:
  CYC(b_+13, b_+16); A = W8(wRingMenu_ringBoxCursorIndex);
  CALL_C(b_+16, ringMenu_getSpriteOffsetForRingBoxPosition_hook, SYM(ringMenu_getSpriteOffsetForRingBoxPosition), b_+19);
  CYC(b_+19, b_+22); SET_HL(b_+25);
  CYC(b_+22, b_+25); TAIL(addSpritesToOam_withOffset);
}

void ringMenu_drawSpritesForRingsInBox_hook(GB *gb) {
  BASE(ringMenu_drawSpritesForRingsInBox);
  CYC(b_+0, b_+2); A = 0x05;
  TAIL(ringMenu_drawSpritesForRingsInBox__loop);
}

void ringMenu_drawSpritesForRingsInBox__loop_hook(GB *gb) {
  BASE(ringMenu_drawSpritesForRingsInBox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+2, b_+3); push_effect(gb, AF);
    CYC(b_+3, b_+6); SET_HL(wRingBoxContents - 1);
    CYC(b_+6, b_+7); push_effect(gb, b_+7); add_a_to_hl(gb);
    CYC(b_+7, b_+10); A = W8(wRingMenu_page);
    CYC(b_+10, b_+12); A = alu_swap(gb, A);
    CYC(b_+12, b_+13); C = A;
    CYC(b_+13, b_+14); A = mem_rd(gb, HL);
    CYC(b_+14, b_+16); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(b_+16, b_+18); ringMenu_drawSpritesForRingsInBox__nextRing_hook(gb); return; }
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+19); alu_sub(gb, C);
    CYC(b_+19, b_+21); alu_cp(gb, 0x10);
    if (!(F & FC)) { CYCT(b_+21, b_+23); ringMenu_drawSpritesForRingsInBox__nextRing_hook(gb); return; }
    CYC(b_+21, b_+23);
    CYC(b_+23, b_+25); B = 0x30;
    CYC(b_+25, b_+27); alu_bit(gb, 3, A);
    if (F & FZ) { CYCT(b_+27, b_+29); goto draw_sprite; }
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+31); B = 0x48;
draw_sprite:
    CYC(b_+31, b_+33); alu_and(gb, 0x07);
    CYC(b_+33, b_+35); A = alu_swap(gb, A);
    CYC(b_+35, b_+36); C = A;
    CYC(b_+36, b_+39); SET_HL(b_+47);
    CALL_C(b_+39, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+42);
    CYC(b_+42, b_+43); SET_AF(pop_effect(gb));
    CYC(b_+43, b_+44); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+44, b_+46); continue; }
    CYC(b_+44, b_+46);
    CYC(b_+46, b_+47); ret_effect(gb);
    return;
  }
}

void ringMenu_drawSpritesForRingsInBox__nextRing_hook(GB *gb) {
  BASE(ringMenu_drawSpritesForRingsInBox);
  CYC(b_+42, b_+43); SET_AF(pop_effect(gb));
  CYC(b_+43, b_+44); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); ringMenu_drawSpritesForRingsInBox__loop_hook(gb); return; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); ret_effect(gb);
}

void ringMenu_calculateNumPagesForUnappraisedRings_hook(GB *gb) {
  BASE(ringMenu_calculateNumPagesForUnappraisedRings);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(GV((SYM(label_02_038) + 24), 0x469d));
  CYC(b_+3, b_+5); E = 0x3f;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CYC(b_+8, b_+11); A = W8(wNumUnappraisedRingsBcd);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); A = B;
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);
  CYC(b_+20, b_+23); W8(wRingMenu_numPages) = A;
  CYC(b_+23, b_+24); ret_effect(gb);
}

void ringMenu_updateSelectedRingFromList_hook(GB *gb) {
  BASE(ringMenu_updateSelectedRingFromList);
  CYC(b_+0, b_+3); A = W8(wRingMenu_page);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+9); A = W8(wRingMenu_ringListCursorIndex);
  CYC(b_+9, b_+10); alu_add(gb, C);
  CYC(b_+10, b_+13); W8(wRingMenu_selectedRing) = A;
  CYC(b_+13, b_+14); ret_effect(gb);
}

void ringMenu_clearRingSelectionArea_hook(GB *gb) {
  BASE(ringMenu_clearRingSelectionArea);
  CYC(b_+0, b_+3); SET_HL(w4TileMap + 0x040);
  CYC(b_+3, b_+6); SET_BC(0x0514);
  CYC(b_+6, b_+9); SET_DE(0x0007);
  CYC(b_+9, b_+12); TAIL(fillRectangleInTilemap);
}

void ringMenu_drawUnappraisedRings_hook(GB *gb) {
  BASE(ringMenu_drawUnappraisedRings);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_clearRingSelectionArea_hook, SYM(ringMenu_clearRingSelectionArea), b_+3);
  CYC(b_+3, b_+5); B = 0x10;
  CYC(b_+5, b_+8); A = W8(wRingMenu_page);
  CYC(b_+8, b_+10); A = alu_swap(gb, A);
  CYC(b_+10, b_+13); SET_HL(wUnappraisedRings);
  CYC(b_+13, b_+14); push_effect(gb, b_+14); add_a_to_hl(gb);
  TAIL(ringMenu_drawUnappraisedRings__nextRing);
}

void ringMenu_drawUnappraisedRings__nextRing_hook(GB *gb) {
  BASE(ringMenu_drawUnappraisedRings);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+15, b_+16); C = A;
    CALL_C(b_+16, ringMenu_drawRing_hook, SYM(ringMenu_drawRing), b_+19);
    CYC(b_+19, b_+20); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+20, b_+22); continue; }
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+24); ringMenu_drawPageCounter_hook(gb);
    return;
  }
}

void ringMenu_drawRingList_hook(GB *gb) {
  BASE(ringMenu_drawRingList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ringMenu_clearRingSelectionArea_hook, SYM(ringMenu_clearRingSelectionArea), b_+3);
  CYC(b_+3, b_+5); B = 0x10;
  CYC(b_+5, b_+8); A = W8(wRingMenu_page);
  CYC(b_+8, b_+10); A = alu_swap(gb, A);
  CYC(b_+10, b_+11); C = A;
  TAIL(ringMenu_drawRingList__nextRing);
}

void ringMenu_drawRingList__nextRing_hook(GB *gb) {
  BASE(ringMenu_drawRingList);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+11, b_+12); A = C;
    CYC(b_+12, b_+15); SET_HL(wRingsObtained);
    CALL_C(b_+15, checkFlag_hook, SYM(checkFlag), b_+18);
    if (!(F & FZ)) CALL_C_CC(b_+18, ringMenu_drawRing_hook, SYM(ringMenu_drawRing), b_+21);
    else CYC(b_+18, b_+21);
    CYC(b_+21, b_+22); C = alu_inc8(gb, C);
    CYC(b_+22, b_+23); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+23, b_+25); continue; }
    CYC(b_+23, b_+25);
    TAIL(ringMenu_drawPageCounter);
  }
}

void ringMenu_drawPageCounter_hook(GB *gb) {
  BASE(ringMenu_drawPageCounter);
  CYC(b_+0, b_+3); SET_HL(w4TileMap + 0x10f);
  CYC(b_+3, b_+6); A = W8(wRingMenu_page);
  CYC(b_+6, b_+8); alu_add(gb, 0x11);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+13); A = W8(wRingMenu_numPages);
  CYC(b_+13, b_+15); alu_add(gb, 0x10);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void ringMenu_drawRingBoxContents_hook(GB *gb) {
  BASE(ringMenu_drawRingBoxContents);
  CYC(b_+0, b_+3); SET_HL(wRingBoxContents);
  CYC(b_+3, b_+5); B = 0x11;
  TAIL(ringMenu_drawRingBoxContents__nextRing);
}

void ringMenu_drawRingBoxContents__nextRing_hook(GB *gb) {
  BASE(ringMenu_drawRingBoxContents);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+6, b_+8); alu_cp(gb, 0xff);
    if (!(F & FZ)) { CYCT(b_+8, b_+10); ringMenu_drawRingBoxContents__drawRing_hook(gb); return; }
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+11); push_effect(gb, HL);
    CYC(b_+11, b_+12); push_effect(gb, BC);
    CYC(b_+12, b_+13); A = B;
    CYC(b_+13, b_+16); SET_HL((SYM(ringMenu_drawRing) + 15));
    CYC(b_+16, b_+17); add_double_index_to_hl(gb, b_+17);
    CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+18, b_+19); H = mem_rd(gb, HL);
    CYC(b_+19, b_+20); L = A;
    CYC(b_+20, b_+23); SET_BC(0x0202);
    CYC(b_+23, b_+26); SET_DE(0x0007);
    CALL_C(b_+26, fillRectangleInTilemap_hook, SYM(fillRectangleInTilemap), b_+29);
    CYC(b_+29, b_+30); SET_BC(pop_effect(gb));
    CYC(b_+30, b_+31); SET_HL(pop_effect(gb));
    CYC(b_+31, b_+33);
    CYC(b_+37, b_+38); B = alu_inc8(gb, B);
    CYC(b_+38, b_+39); A = L;
    CYC(b_+39, b_+41); alu_cp(gb, (uint8_t)(wRingBoxContents + 5));
    if (F & FC) { CYCT(b_+41, b_+43); continue; }
    CYC(b_+41, b_+43);
    CYC(b_+43, b_+44); ret_effect(gb);
    return;
  }
}

void ringMenu_drawRingBoxContents__drawRing_hook(GB *gb) {
  BASE(ringMenu_drawRingBoxContents);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+33, b_+34); C = A;
  CALL_C(b_+34, ringMenu_drawRing_hook, SYM(ringMenu_drawRing), b_+37);
  CYC(b_+37, b_+38); B = alu_inc8(gb, B);
  CYC(b_+38, b_+39); A = L;
  CYC(b_+39, b_+41); alu_cp(gb, (uint8_t)(wRingBoxContents + 5));
  if (F & FC) { CYCT(b_+41, b_+43); ringMenu_drawRingBoxContents__nextRing_hook(gb); return; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); ret_effect(gb);
}

void ringMenu_drawRing_hook(GB *gb) {
  BASE(ringMenu_drawRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+3); A = B;
  CYC(b_+3, b_+6); SET_HL(b_+15);
  CYC(b_+6, b_+7); add_double_index_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); D = mem_rd(gb, HL);
  CYC(b_+9, b_+10); E = A;
  CYC(b_+10, b_+11); A = C;
  CALL_C(b_+11, getRingTiles_hook, SYM(getRingTiles), b_+14);
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, b_+16); SET_BC(pop_effect(gb));
  CYC(b_+16, b_+17); ret_effect(gb);
}

void getRingTiles_hook(GB *gb) {
  BASE(getRingTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, A);
  if (F & FZ) CYCT(b_+5, b_+7);
  else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+9); alu_cp(gb, 0xfe);
    CYC(b_+9, b_+11); A = 0x40;
    if (!(F & FZ)) CYCT(b_+11, b_+13);
    else {
      CYC(b_+11, b_+13);
      CYC(b_+13, b_+16); A = W8(wRingBoxLevel);
      CYC(b_+16, b_+18); alu_add(gb, 0x40);
      CYC(b_+18, b_+20);
    }
  }
  CALL_C(b_+20, multiplyABy8_hook, SYM(multiplyABy8), b_+23);
  CYC(b_+23, b_+26); SET_HL(0x57a0);
  CYC(b_+26, b_+27); alu_add_hl(gb, BC);
  CYC(b_+27, b_+28); push_effect(gb, DE);
  CALL_C(b_+28, copy8BytesFromRingMapToCec0_hook, SYM(copy8BytesFromRingMapToCec0), b_+31);
  CYC(b_+31, b_+32); SET_HL(pop_effect(gb));
  CYC(b_+32, b_+35); SET_DE(wTmpcec0);
  CALL_C(b_+35, getRingTiles__drawTile_hook, b_+49, b_+38);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CALL_C(b_+39, getRingTiles__drawTile_hook, b_+49, b_+42);
  CYC(b_+42, b_+44); A = 0x1f;
  CYC(b_+44, b_+45); push_effect(gb, b_+45); add_a_to_hl(gb);
  CALL_C(b_+45, getRingTiles__drawTile_hook, b_+49, b_+48);
  CYC(b_+48, b_+49); L = alu_inc8(gb, L);
  TAIL(getRingTiles__drawTile);
}

void getRingTiles__drawTile_hook(GB *gb) {
  BASE(getRingTiles);
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  CYC(b_+51, b_+52); E = alu_inc8(gb, E);
  CYC(b_+52, b_+54); H |= 0x04;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  CYC(b_+56, b_+57); E = alu_inc8(gb, E);
  CYC(b_+57, b_+59); H &= ~0x04;
  CYC(b_+59, b_+60); ret_effect(gb);
}

void ringMenu_updateDisplayedRingNumber_hook(GB *gb) {
  BASE(ringMenu_updateDisplayedRingNumber);
  CYC(b_+0, b_+3); A = W8(wRingMenu_ringListCursorIndex);
  TAIL(ringMenu_updateDisplayedRingNumberWithGivenComparator);
}

void ringMenu_updateDisplayedRingNumberWithGivenComparator_hook(GB *gb) {
  BASE(ringMenu_updateDisplayedRingNumberWithGivenComparator);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wRingMenu_displayedRingNumberComparator);
  CYC(b_+3, b_+4); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+9); A = W8(wRingMenu_selectedRing);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); ringMenu_updateDisplayedRingNumberWithGivenComparator__noRing_hook(gb); return; }
  CYC(b_+10, b_+12);
  CALL_C(b_+12, hexToDec_hook, SYM(hexToDec), b_+15);
  CYC(b_+15, b_+17); A |= 0x10;
  CYC(b_+17, b_+19); C |= 0x10;
  CYC(b_+19, b_+21);
  TAIL(ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber);
}

void ringMenu_updateDisplayedRingNumberWithGivenComparator__noRing_hook(GB *gb) {
  BASE(ringMenu_updateDisplayedRingNumberWithGivenComparator);
  CYC(b_+21, b_+23); A = 0xe8;
  CYC(b_+23, b_+24); C = A;
  TAIL(ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber);
}

void ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber_hook(GB *gb) {
  BASE(ringMenu_updateDisplayedRingNumberWithGivenComparator);
  CYC(b_+24, b_+27); SET_HL(w4TileMap + 0x105);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+28, b_+29); mem_wr(gb, HL, C);
  CYC(b_+29, b_+32); TAIL(ringMenu_copyTilemapToVram);
}

void ringMenu_setDisplayedText_hook(GB *gb) {
  BASE(ringMenu_setDisplayedText);
  CYC(b_+0, b_+3); SET_HL(wRingMenu_descriptionTextIndex);
  CYC(b_+3, b_+4); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); B = 0x30;
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+14); W8(wTextboxPosition) = A;
  CYC(b_+14, b_+16); A = 0x09;
  CYC(b_+16, b_+19); W8(wTextboxFlags) = A;
  CYC(b_+19, b_+22); TAIL(showTextNonExitable);
}

void ringMenu_retIfTextIsPrinting_hook(GB *gb) {
  BASE(ringMenu_retIfTextIsPrinting);
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+5); alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_AF(pop_effect(gb));
  CYC(b_+7, b_+8); ret_effect(gb);
}

void saveQuitMenu_checkIsGameOver_hook(GB *gb) {
  BASE(saveQuitMenu_checkIsGameOver);
  CYC(b_+0, b_+3); A = W8(wSaveQuitMenu_gameOver);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+5); ret_effect(gb);
}

void runSaveAndQuitMenu_hook(GB *gb) {
  BASE(runSaveAndQuitMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+4); hram_wr(gb, R_SVBK, A);
  CALL_C(b_+4, runSaveAndQuitMenu__runState_hook, b_+10, b_+7);
  CYC(b_+7, b_+10); TAIL(saveQuitMenu_drawSprites);
}

void runSaveAndQuitMenu__runState_hook(GB *gb) {
  BASE(runSaveAndQuitMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+10, b_+13); A = W8(wSaveQuitMenu_state);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(saveQuitMenu_state0) && hook_enabled_at(gb, SYM(saveQuitMenu_state0))) { saveQuitMenu_state0_hook(gb); return; }
    else if (jt_ == SYM(saveQuitMenu_state1) && hook_enabled_at(gb, SYM(saveQuitMenu_state1))) { saveQuitMenu_state1_hook(gb); return; }
    else if (jt_ == SYM(saveQuitMenu_state2) && hook_enabled_at(gb, SYM(saveQuitMenu_state2))) { saveQuitMenu_state2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void saveQuitMenu_state0_hook(GB *gb) {
  BASE(saveQuitMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CALL_C(b_+3, stopTextThread_hook, SYM(stopTextThread), b_+6);
  CYC(b_+6, b_+8); A = 0xa0;
  CALL_C(b_+8, loadGfxHeader_hook, SYM(loadGfxHeader), b_+11);
  CYC(b_+11, b_+13); A = 0xa6;
  CALL_C(b_+13, loadGfxHeader_hook, SYM(loadGfxHeader), b_+16);
  CYC(b_+16, b_+18); A = 0xa8;
  CALL_C(b_+18, loadGfxHeader_hook, SYM(loadGfxHeader), b_+21);
  CALL_C(b_+21, saveQuitMenu_checkIsGameOver_hook, SYM(saveQuitMenu_checkIsGameOver), b_+24);
  if (F & FZ) { CYCT(b_+24, b_+26); saveQuitMenu_state0__notGameOver_hook(gb); return; }
  CYC(b_+24, b_+26);
  TAIL(saveQuitMenu_state0__gameOver);
}

void saveQuitMenu_state0__gameOver_hook(GB *gb) {
  BASE(saveQuitMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+26, restartSound_hook, SYM(restartSound), b_+29);
  CYC(b_+29, b_+31); A = 0xe8;
  CALL_C(b_+31, threadStop_hook, SYM(threadStop), b_+34);
  CYC(b_+34, b_+37); SET_HL(wDeathCounter);
  CYC(b_+37, b_+40); SET_BC(0x0001);
  CALL_C(b_+40, addDecimalToHlRef_hook, SYM(addDecimalToHlRef), b_+43);
  CYC(b_+43, b_+45); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(b_+45, b_+47); goto load_game_over_gfx; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x99);
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x09);
load_game_over_gfx:
  CYC(b_+52, b_+54); A = 0xa9;
  CALL_C(b_+54, loadGfxHeader_hook, SYM(loadGfxHeader), b_+57);
  CYC(b_+57, b_+59); A = 0x09;
  CALL_C(b_+59, playSound_b00_hook, SYM(playSound_b00), b_+62);
  CYC(b_+62, b_+64); A = 0x06;
  CYC(b_+64, b_+66);
  CALL_C(b_+72, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+75);
  CYC(b_+75, b_+77); A = 0x08;
  CALL_C(b_+77, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+80);
  CALL_C(b_+80, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+83);
  CYC(b_+83, b_+85); A = 0x01;
  CYC(b_+85, b_+88); W8(wSaveQuitMenu_state) = A;
  CYC(b_+88, b_+90); A = 0x05;
  CYC(b_+90, b_+93); TAIL(loadGfxRegisterStateIndex);
}

void saveQuitMenu_state0__notGameOver_hook(GB *gb) {
  BASE(saveQuitMenu_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+66, b_+67); alu_xor(gb, A);
  CALL_C(b_+67, setMusicVolume_hook, SYM(setMusicVolume), b_+70);
  CYC(b_+70, b_+72); A = 0x05;
  CALL_C(b_+72, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+75);
  CYC(b_+75, b_+77); A = 0x08;
  CALL_C(b_+77, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+80);
  CALL_C(b_+80, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+83);
  CYC(b_+83, b_+85); A = 0x01;
  CYC(b_+85, b_+88); W8(wSaveQuitMenu_state) = A;
  CYC(b_+88, b_+90); A = 0x05;
  CYC(b_+90, b_+93); TAIL(loadGfxRegisterStateIndex);
}

void saveQuitMenu_state1_hook(GB *gb) {
  BASE(saveQuitMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wKeysJustPressed);
  CYC(b_+8, b_+10); C = 0xff;
  CYC(b_+10, b_+12); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); saveQuitMenu_state1__upOrDown_hook(gb); return; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); C = 1;
  CYC(b_+16, b_+18); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); saveQuitMenu_state1__upOrDown_hook(gb); return; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); saveQuitMenu_state1__bPressed_hook(gb); return; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x09);
  if (F & FZ) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+30); A = W8(wSaveQuitMenu_cursorIndex);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+31, saveFile_b00_hook, SYM(saveFile_b00), b_+34); else CYC(b_+31, b_+34);
  CYC(b_+34, b_+36); A = 2;
  CYC(b_+36, b_+39); W8(wSaveQuitMenu_state) = A;
  CYC(b_+39, b_+41); A = 0x1e;
  CYC(b_+41, b_+44); W8(wSaveQuitMenu_delayCounter) = A;
  CYC(b_+44, b_+46); A = 0x56;
  CYC(b_+46, b_+49); TAIL(playSound_b00);
}

void saveQuitMenu_state1__upOrDown_hook(GB *gb) {
  BASE(saveQuitMenu_state1);
  CYC(b_+49, b_+52); SET_HL(wSaveQuitMenu_cursorIndex);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+54); alu_add(gb, C);
  CYC(b_+54, b_+56); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(b_+56, b_+57); ret_effect(gb); return; }
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); A = 0x84;
  CYC(b_+60, b_+63); TAIL(playSound_b00);
}

void saveQuitMenu_state1__bPressed_hook(GB *gb) {
  BASE(saveQuitMenu_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+63, saveQuitMenu_checkIsGameOver_hook, SYM(saveQuitMenu_checkIsGameOver), b_+66);
  if (!(F & FZ)) { CYCT(b_+66, b_+67); ret_effect(gb); return; }
  CYC(b_+66, b_+67); CYC(b_+67, b_+70); closeMenu_hook(gb);
}

void saveQuitMenu_drawSprites_hook(GB *gb) {
  BASE(saveQuitMenu_drawSprites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelect_redrawDecorationsAndSetWramBank4_hook, SYM(fileSelect_redrawDecorationsAndSetWramBank4), b_+3);
  CYC(b_+3, b_+6); A = W8(wSaveQuitMenu_delayCounter);
  CYC(b_+6, b_+8); alu_and(gb, 4);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); A = W8(wSaveQuitMenu_cursorIndex);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+18); A = alu_swap(gb, A);
  CYC(b_+18, b_+19); alu_rrca(gb);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+23); SET_HL(b_+26);
  CYC(b_+23, b_+26); TAIL(addSpritesToOam_withOffset);
}

void fileSelect_redrawDecorationsAndSetWramBank4_hook(GB *gb) {
  BASE(fileSelect_redrawDecorationsAndSetWramBank4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearOam_hook, SYM(clearOam), b_+3);
  CYC(b_+3, b_+5); A = 4;
  CYC(b_+5, b_+7); hram_wr(gb, R_SVBK, A);
  CYC(b_+7, b_+10); SET_HL(b_+13);
  CYC(b_+10, b_+13); TAIL(addSpritesToOam);
}

void fileSelectDrawLink_body_hook(GB *gb) {
  BASE(fileSelectDrawLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+2, b_+5); A = W8(wFileSelect_cursorPos);
  CYC(b_+5, b_+7); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); D = 0;
  CALL_C(b_+10, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+13);
  CYC(b_+13, b_+15); C = 0;
  CYC(b_+15, b_+17); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto draw; }
  CYC(b_+17, b_+19); CYC(b_+19, b_+20); push_effect(gb, BC);
  CYC(b_+20, b_+21); push_effect(gb, DE); CYC(b_+21, b_+23); D = 7;
  CALL_C(b_+23, getFileDisplayVariableAddress_paramE_hook, SYM(getFileDisplayVariableAddress_paramE), b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A); CYC(b_+27, b_+29); B = 0x10;
  CYC(b_+29, b_+31); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) CALL_C_CC(b_+31, fileSelectDrawLink__draw_hook, b_+62, b_+34); else CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); SET_DE(pop_effect(gb)); CYC(b_+35, b_+36); SET_BC(pop_effect(gb));
  CYC(b_+36, b_+38); D = 6; CALL_C(b_+38, getFileDisplayVariableAddress_paramE_hook, SYM(getFileDisplayVariableAddress_paramE), b_+41);
  CYC(b_+41, b_+42); C = alu_inc8(gb, C); CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); alu_rrca(gb); if (F & FC) { CYCT(b_+44,b_+46); goto draw; }
  CYC(b_+44,b_+46); CYC(b_+46,b_+47); C = alu_inc8(gb,C); CYC(b_+47,b_+49); alu_bit(gb,0,mem_rd(gb,HL));
  if (!(F & FZ)) { CYCT(b_+49,b_+51); goto draw; } CYC(b_+49,b_+51); CYC(b_+51,b_+52); C=alu_inc8(gb,C);
draw:
  CYC(b_+52,b_+55); A=W8(wFileSelect_cbb6); CYC(b_+55,b_+57); alu_and(gb,0x10); CYC(b_+57,b_+58); A=C;
  if (F & FZ) { CYCT(b_+58,b_+60); fileSelectDrawLink__draw_hook(gb); return; }
  CYC(b_+58,b_+60); CYC(b_+60,b_+62); alu_add(gb,8); fileSelectDrawLink__draw_hook(gb);
}
void fileSelectDrawLinkInOtherGame_hook(GB *gb) {
  BASE(fileSelectDrawLinkInOtherGame);
  CYC(b_+0, b_+2); B = GV(0x00, 0x04);
  CYC(b_+2, b_+4);
  fileSelectDrawLink_body_hook(gb);
}

void fileSelectDrawLink_hook(GB *gb) {
  BASE(fileSelectDrawLink);
  CYC(b_+0, b_+2); B = GV(0x04, 0x00);
  fileSelectDrawLink_body_hook(gb);
}
void fileSelectDrawLink__draw_hook(GB *gb) {
  BASE(fileSelectDrawLink);
  CYC(b_+62,b_+63); alu_add(gb,B); CYC(b_+63,b_+66); SET_HL(b_+72);
  CYC(b_+66,b_+67); push_effect(gb,b_+67); add_a_to_hl(gb);
  CYC(b_+67,b_+68); A=mem_rd(gb,HL);
  CYC(b_+68,b_+69); push_effect(gb,b_+69); add_a_to_hl(gb);
  CYC(b_+69,b_+72); TAIL(addSpritesToOam);
}

void runGameLinkMenu_hook(GB *gb) {
  BASE(runGameLinkMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wFileSelect_cbb6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+4, fileSelect_redrawDecorationsAndSetWramBank4_hook, SYM(fileSelect_redrawDecorationsAndSetWramBank4), SYM(fileSelectMode7));
  TAIL(fileSelectMode7);
}

void fileSelectMode7_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode7__mode7States_hook, b_+18, b_+3);
  CYC(b_+3, b_+6); A = W8(wFileSelect_mode2);
  CYC(b_+6, b_+8); alu_cp(gb, 6);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); alu_cp(gb, 3);
  if (F & FC) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, fileSelectDrawAcornCursor_hook, SYM(fileSelectDrawAcornCursor), b_+15);
  CYC(b_+15, b_+18); TAIL(fileSelectDrawLinkInOtherGame);
}

void fileSelectMode7__mode7States_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+18, b_+21); A = W8(wFileSelect_mode2);
  CYC(b_+21, b_+22); push_effect(gb, b_+22);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+36) { fileSelectMode7__state0_hook(gb); return; }
    else if (jt_ == b_+95) { fileSelectMode7__state1_hook(gb); return; }
    else if (jt_ == b_+185) { fileSelectMode7__state2_hook(gb); return; }
    else if (jt_ == b_+219) { fileSelectMode7__state3_hook(gb); return; }
    else if (jt_ == b_+331) { fileSelectMode7__state4_hook(gb); return; }
    else if (jt_ == b_+385) { fileSelectMode7__state5_hook(gb); return; }
    else if (jt_ == b_+405) { fileSelectMode7__state6_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode7__state0_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+36, disableLcd_hook, SYM(disableLcd), b_+39);
  CYC(b_+39, b_+41); A = 0xa0;
  CALL_C(b_+41, loadGfxHeader_hook, SYM(loadGfxHeader), b_+44);
  CYC(b_+44, b_+46); A = 0xae;
  CALL_C(b_+46, loadGfxHeader_hook, SYM(loadGfxHeader), b_+49);
  CYC(b_+49, b_+51); A = 5;
  CALL_C(b_+51, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+54);
  CYC(b_+54, b_+56); A = 8;
  CALL_C(b_+56, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+59);
  CYC(b_+59, b_+62); SET_HL(w4NameBuffer);
  CYC(b_+62, b_+64); B = 0x20;
  CALL_C(b_+64, clearMemory_hook, SYM(clearMemory), b_+67);
  CALL_C(b_+67, textInput_updateEntryCursor_hook, SYM(textInput_updateEntryCursor), b_+70);
  CALL_C(b_+70, serialFunc_0c85_hook, SYM(serialFunc_0c85), b_+73);
  CYC(b_+73, b_+75); A = 4;
  CYC(b_+75, b_+77); H8(hFFBE) = A;
  CYC(b_+77, b_+78); alu_xor(gb, A);
  CYC(b_+78, b_+80); H8(hSerialLinkState) = A;
  CYC(b_+80, b_+83); mem_wr(gb, wFileSelect_cbc0 + 2, A);
  CYC(b_+83, b_+86); SET_HL(wFileSelect_linkTimer);
  CYC(b_+86, b_+88); A = 0xf0;
  CYC(b_+88, b_+89); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+89, b_+91); A = 0x1e;
  CYC(b_+91, b_+92); mem_wr(gb, HL, A);
  CYC(b_+92, b_+95); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode7__state1_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+95, b_+97); A = H8(hSerialInterruptBehaviour);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto serial_interrupt; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); A = H8(hFFBD);
  CYC(b_+102, b_+103); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+103, b_+106); fileSelectMode7__func_02_4c55_hook(gb); return; }
  CYC(b_+103, b_+106);
  CYC(b_+106, b_+109); SET_HL(wFileSelect_linkTimer);
  CYC(b_+109, b_+110); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+110, b_+112); CYC(b_+119, b_+122); serialFunc_0c73_hook(gb); return; }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); A = 0x80;
  CYC(b_+114, b_+116); H8(hFFBD) = A;
  CYC(b_+116, b_+119); fileSelectMode7__func_02_4c55_hook(gb);
  return;
serial_interrupt:
  CYC(b_+122, b_+125); A = W8(wFileSelect_cbc0);
  CYC(b_+125, b_+126); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+126, b_+128); goto transfer; }
  CYC(b_+126, b_+128);
  CYC(b_+128, b_+129); A = alu_dec8(gb, A);
  CYC(b_+129, b_+132); W8(wFileSelect_cbc0) = A;
  CYC(b_+132, b_+133); ret_effect(gb);
  return;
transfer:
  CALL_C(b_+133, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+136);
  CYC(b_+136, b_+138); A = H8(hFFBD);
  CYC(b_+138, b_+139); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+139, b_+141); goto check_link; }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(b_+143, b_+145); goto check_link; }
  CYC(b_+143, b_+145);
  if (!(F & FZ)) { CYCT(b_+145, b_+148); fileSelectMode7__func_02_4c55_hook(gb); return; }
  CYC(b_+145, b_+148);
check_link:
  CYC(b_+148, b_+150); A = H8(hSerialLinkState);
  CYC(b_+150, b_+152); alu_cp(gb, 7);
  if (!(F & FZ)) { CYCT(b_+152, b_+153); ret_effect(gb); return; }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+155); E = 3;
find_file:
  CYC(b_+155, b_+156); E = alu_dec8(gb, E);
  CYC(b_+156, b_+158); D = 0;
  CALL_C(b_+158, getFileDisplayVariableAddress_paramE_hook, SYM(getFileDisplayVariableAddress_paramE), b_+161);
  CYC(b_+161, b_+163); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+163, b_+165); CYC(b_+182, b_+185); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb); return; }
  CYC(b_+163, b_+165);
  CYC(b_+165, b_+166); A = E;
  CYC(b_+166, b_+167); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+167, b_+169); goto find_file; }
  CYC(b_+167, b_+169);
  CYC(b_+169, b_+171); A = 0x85;
  CYC(b_+171, b_+174); mem_wr(gb, wFileSelect_cbc0 + 2, A);
  CYC(b_+174, b_+176); A = 0xff;
  CYC(b_+176, b_+179); W8(wFileSelect_cursorPos) = A;
  CYC(b_+179, b_+182); TAIL(fileSelectMode7__func_02_4c4b);
}

void fileSelectMode7__state2_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+185, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+188);
  CYC(b_+188, b_+190); A = 6;
  CYC(b_+190, b_+193); W8(wFileSelect_cursorOffset) = A;
  CYC(b_+193, b_+194); alu_xor(gb, A);
  CALL_C(b_+194, func_02_4149_hook, SYM(func_02_4149), b_+197);
  CALL_C(b_+197, disableLcd_hook, SYM(disableLcd), b_+200);
  CYC(b_+200, b_+202); A = 0xa1;
  CALL_C(b_+202, loadGfxHeader_hook, SYM(loadGfxHeader), b_+205);
  CYC(b_+205, b_+207); A = 0xaf;
  CALL_C(b_+207, loadGfxHeader_hook, SYM(loadGfxHeader), b_+210);
  CALL_C(b_+210, textInput_updateEntryCursor_hook, SYM(textInput_updateEntryCursor), b_+213);
  CALL_C(b_+213, fileSelectDrawHeartsAndDeathCounter_hook, SYM(fileSelectDrawHeartsAndDeathCounter), b_+216);
  CYC(b_+216, b_+219); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode7__state3_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+219, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+222);
  CALL_C(b_+222, fileSelectUpdateInput_hook, SYM(fileSelectUpdateInput), b_+225);
  if (!(F & FZ)) { CYCT(b_+225, b_+227); fileSelectMode7__selectedSomething_hook(gb); return; }
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+230); A = W8(wKeysJustPressed);
  CYC(b_+230, b_+232); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+232, b_+233); ret_effect(gb); return; }
  CYC(b_+232, b_+233);
  TAIL(fileSelectMode7__moveCursorToQuit);
}

void fileSelectMode7__moveCursorToQuit_hook(GB *gb) {
  BASE(fileSelectMode7);
  CYC(b_+233, b_+235); A = 3;
  CYC(b_+235, b_+238); W8(wFileSelect_cursorPos) = A;
  CYC(b_+238, b_+240); A = 0x8f;
  CYC(b_+240, b_+243); mem_wr(gb, wFileSelect_cbc0 + 2, A);
  CYC(b_+243, b_+245); TAIL(fileSelectMode7__func_02_4c4b);
}

void fileSelectMode7__selectedSomething_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+245, b_+248); A = W8(wFileSelect_cursorPos);
  CYC(b_+248, b_+250); alu_cp(gb, 3);
  if (F & FZ) { CYCT(b_+250, b_+252); fileSelectMode7__moveCursorToQuit_hook(gb); return; }
  CYC(b_+250, b_+252);
  CYC(b_+252, b_+254); D = 0;
  CALL_C(b_+254, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+257);
  CYC(b_+257, b_+259); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+259, b_+261); goto valid_file; }
  CYC(b_+259, b_+261);
  CYC(b_+261, b_+263); A = 0x5a;
  CYC(b_+263, b_+266); playSound_b00_hook(gb);
  return;
valid_file:
  CYC(b_+266, b_+269); A = W8(wOpenedMenuType);
  CYC(b_+269, b_+271); alu_cp(gb, 8);
  if (!(F & FZ)) { CYCT(b_+271, b_+273); goto file_select; }
  CYC(b_+271, b_+273);
  CYC(b_+273, b_+275); A = 0x0c;
  CYC(b_+275, b_+277); H8(hSerialLinkState) = A;
  CYC(b_+277, b_+279); A = 5;
  CYC(b_+279, b_+282); W8(wFileSelect_mode2) = A;
  CYC(b_+282, b_+283); ret_effect(gb);
  return;
file_select:
  CYC(b_+283, b_+285); A = 8;
  CYC(b_+285, b_+287); H8(hSerialLinkState) = A;
  CYC(b_+287, b_+290); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode7__func_02_4c4b_hook(GB *gb) {
  BASE(fileSelectMode7);
  CYC(b_+290, b_+292); A = 8;
  CYC(b_+292, b_+294); H8(hSerialLinkState) = A;
  CYC(b_+294, b_+296); A = 5;
  CYC(b_+296, b_+299); W8(wFileSelect_mode2) = A;
  CYC(b_+299, b_+300); ret_effect(gb);
}

void fileSelectMode7__func_02_4c55_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+300, disableLcd_hook, SYM(disableLcd), b_+303);
  CYC(b_+303, b_+305); A = 7;
  CALL_C(b_+305, loadGfxHeader_hook, SYM(loadGfxHeader), b_+308);
  CALL_C(b_+308, loadGfxRegisterState5AndIncFileSelectMode2_hook, SYM(loadGfxRegisterState5AndIncFileSelectMode2), b_+311);
  CYC(b_+311, b_+313); A = 8;
  CYC(b_+313, b_+315); H8(hSerialLinkState) = A;
  CYC(b_+315, b_+317); A = 6;
  CYC(b_+317, b_+320); W8(wFileSelect_mode2) = A;
  CYC(b_+320, b_+322); A = 0xb4;
  CYC(b_+322, b_+325); W8(wFileSelect_linkTimer) = A;
  CYC(b_+325, b_+327); A = H8(hFFBD);
  CYC(b_+327, b_+330); W8(wFileSelect_cbc0) = A;
  CYC(b_+330, b_+331); ret_effect(gb);
}

void fileSelectMode7__state4_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+331, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+334);
  CYC(b_+334, b_+336); A = H8(hSerialInterruptBehaviour);
  CYC(b_+336, b_+337); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+337, b_+338); ret_effect(gb); return; }
  CYC(b_+337, b_+338);
  CALL_C(b_+338, loadFile_b00_hook, SYM(loadFile_b00), b_+341);
  CYC(b_+341, b_+344); A = W8(wFileSelect_cursorPos);
  CYC(b_+344, b_+345); A = alu_inc8(gb, A);
  CYC(b_+345, b_+348); SET_HL(w4RingFortuneStuff);
  CYC(b_+348, b_+351); SET_BC(0x16);
offset_loop:
  CYC(b_+351, b_+352); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+352, b_+354); goto copy_file; }
  CYC(b_+352, b_+354);
  CYC(b_+354, b_+355); alu_add_hl(gb, BC);
  CYC(b_+355, b_+357);
  goto offset_loop;
copy_file:
  CYC(b_+357, b_+359); B = 0x16;
  CYC(b_+359, b_+362); SET_DE(wc600Block);
  CALL_C(b_+362, copyMemory_hook, SYM(copyMemory), b_+365);
  CYC(b_+365, b_+368); SET_HL(wFileIsLinkedGame);
  CYC(b_+368, b_+370); mem_wr(gb, HL, mem_rd(gb, HL) | 1);
  CYC(b_+370, b_+372); L = (uint8_t)wFileIsCompleted;
  CYC(b_+372, b_+374); mem_wr(gb, HL, 0);
  CALL_C(b_+374, initializeFile_b00_hook, SYM(initializeFile_b00), b_+377);
  CYC(b_+377, b_+379); A = 0x56;
  CALL_C(b_+379, playSound_b00_hook, SYM(playSound_b00), b_+382);
  CYC(b_+382, b_+385); TAIL(setFileSelectModeTo1);
}

void fileSelectMode7__state5_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+385, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+388);
  CYC(b_+388, b_+390); A = H8(hSerialInterruptBehaviour);
  CYC(b_+390, b_+391); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+391, b_+392); ret_effect(gb); return; }
  CYC(b_+391, b_+392);
  TAIL(fileSelectMode7__cancelLink);
}

void fileSelectMode7__cancelLink_hook(GB *gb) {
  BASE(fileSelectMode7);
  CYC(b_+392, b_+395); A = W8(wOpenedMenuType);
  CYC(b_+395, b_+397); alu_cp(gb, 8);
  if (F & FZ) { CYCT(b_+397, b_+400); closeMenu_hook(gb); return; }
  CYC(b_+397, b_+400);
  CYC(b_+400, b_+402); A = 0;
  CYC(b_+402, b_+405); TAIL(setFileSelectMode);
}

void fileSelectMode7__state6_hook(GB *gb) {
  BASE(fileSelectMode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+405, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+408);
  CYC(b_+408, b_+410); A = H8(hSerialInterruptBehaviour);
  CYC(b_+410, b_+411); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+411, b_+412); ret_effect(gb); return; }
  CYC(b_+411, b_+412);
  CYC(b_+412, b_+415); A = W8(wFileSelect_cbc0);
  CYC(b_+415, b_+417); H8(hFFBD) = A;
  CYC(b_+417, b_+420); A = W8(wKeysJustPressed);
  CYC(b_+420, b_+421); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+421, b_+423); fileSelectMode7__cancelLink_hook(gb); return; }
  CYC(b_+421, b_+423);
  CYC(b_+423, b_+426); SET_HL(wFileSelect_linkTimer);
  CYC(b_+426, b_+427); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+427, b_+428); ret_effect(gb); return; }
  CYC(b_+427, b_+428);
  CYC(b_+428, b_+430);
  TAIL(fileSelectMode7__cancelLink);
}

void runFakeReset_hook(GB *gb) {
  BASE(runFakeReset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wFakeResetMenu_state);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+8) { runFakeReset__state0_hook(gb); return; }
    else if (jt_ == b_+51) { runFakeReset__state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void runFakeReset__state0_hook(GB *gb) {
  BASE(runFakeReset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+8, disableLcd_hook, SYM(disableLcd), b_+11);
  CALL_C(b_+11, clearOam_hook, SYM(clearOam), b_+14);
  CALL_C(b_+14, clearVram_hook, SYM(clearVram), b_+17);
  CALL_C(b_+17, initializeVramMaps_hook, SYM(initializeVramMaps), b_+20);
  CYC(b_+20, b_+22); A = 0xf5;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+27); A = 1;
  CALL_C(b_+27, loadGfxHeader_hook, SYM(loadGfxHeader), b_+30);
  CYC(b_+30, b_+32); A = 1;
  CALL_C(b_+32, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+35);
  CYC(b_+35, b_+37); A = 0x78;
  CYC(b_+37, b_+40); W8(wFakeResetMenu_delayCounter) = A;
  CYC(b_+40, b_+43); SET_HL(wFakeResetMenu_state);
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+44, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+47);
  CYC(b_+47, b_+48); alu_xor(gb, A);
  CYC(b_+48, b_+51); TAIL(loadGfxRegisterStateIndex);
}

void runFakeReset__state1_hook(GB *gb) {
  BASE(runFakeReset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+51, b_+54); A = W8(wPaletteThread_mode);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+55, b_+56); ret_effect(gb); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+59); SET_HL(wFakeResetMenu_delayCounter);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); A = 0xf6;
  CALL_C(b_+63, playSound_b00_hook, SYM(playSound_b00), b_+66);
  CYC(b_+66, b_+69); SET_HL(wMenuLoadState);
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+70, b_+73); TAIL(fadeoutToWhite);
}

void fileSelectMode1__state0_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+21, setFileSelectCursorOffsetToFileSelectMode_hook, SYM(setFileSelectCursorOffsetToFileSelectMode), b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CALL_C(b_+25, func_02_4149_hook, SYM(func_02_4149), b_+28);
  CALL_C(b_+28, disableLcd_hook, SYM(disableLcd), b_+31);
  CYC(b_+31, b_+33); A = 0xba;
  CALL_C(b_+33, loadGfxHeader_hook, SYM(loadGfxHeader), b_+36);
  CYC(b_+36, b_+38); A = 5;
  CALL_C(b_+38, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+41);
  CALL_C(b_+41, loadFileDisplayVariables_hook, SYM(loadFileDisplayVariables), b_+44);
  CALL_C(b_+44, textInput_updateEntryCursor_hook, SYM(textInput_updateEntryCursor), b_+47);
  CALL_C(b_+47, fileSelectDrawHeartsAndDeathCounter_hook, SYM(fileSelectDrawHeartsAndDeathCounter), b_+50);
  CYC(b_+50, b_+53); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode1__state1_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+53, fileSelectUpdateInput_hook, SYM(fileSelectUpdateInput), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto selected; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+61); SET_HL(wFileSelect_cursorPos);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+62, b_+64); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+64, b_+66); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(b_+66, b_+67); ret_effect(gb); return; }
  CYC(b_+66, b_+67);
  CALL_C(b_+67, func_02_448d_hook, SYM(func_02_448d), b_+70);
  if (F & FZ) { CYCT(b_+70, b_+71); ret_effect(gb); return; }
  CYC(b_+70, b_+71);
selected:
  CYC(b_+71, b_+73); A = 0x56;
  CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
  CALL_C(b_+76, fileSelectMode1__getNextFileSelectMode_hook, b_+93, b_+79);
  if (!(F & FZ)) { CYCT(b_+79, b_+82); setFileSelectMode_hook(gb); return; }
  CYC(b_+79, b_+82);
  CALL_C(b_+82, incFileSelectMode2_hook, SYM(incFileSelectMode2), b_+85);
  CALL_C(b_+85, loadFile_b00_hook, SYM(loadFile_b00), b_+88);
  CYC(b_+88, b_+90); A = 0x16;
  CYC(b_+90, b_+93); TAIL(loadUncompressedGfxHeader);
}

void fileSelectMode1__getNextFileSelectMode_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+93, b_+96); A = W8(wFileSelect_cursorPos);
  CYC(b_+96, b_+98); alu_cp(gb, 3);
  if (F & FZ) { CYCT(b_+98, b_+100); goto non_file; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); H8(hActiveFileSlot) = A;
  CYC(b_+102, b_+104); D = 0;
  CALL_C(b_+104, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+107);
  CYC(b_+107, b_+109); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+109, b_+111); A = 5;
  if (!(F & FZ)) { CYCT(b_+111, b_+112); ret_effect(gb); return; }
  CYC(b_+111, b_+112);
  CYC(b_+112, b_+113); alu_xor(gb, A);
  CYC(b_+113, b_+114); ret_effect(gb);
  return;
non_file:
  CYC(b_+114, b_+117); A = W8(wFileSelect_cursorPos2);
  CYC(b_+117, b_+119); alu_add(gb, 3);
  CYC(b_+119, b_+120); ret_effect(gb);
}

void fileSelectMode1__state2_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+120, fileSelectMode1__textSpeedMenu_checkInput_hook, b_+125, b_+123);
  CYC(b_+123, b_+125);
  TAIL(fileSelectMode1__textSpeedMenu_addCursorToOam);
}

void fileSelectMode1__textSpeedMenu_checkInput_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+125, b_+128); A = W8(wKeysJustPressed);
  CYC(b_+128, b_+129); B = A;
  CYC(b_+129, b_+131); alu_and(gb, 6);
  if (!(F & FZ)) { CYCT(b_+131, b_+133); fileSelectMode1__back_hook(gb); return; }
  CYC(b_+131, b_+133); CYC(b_+133, b_+135); C = 1;
  CYC(b_+135, b_+137); alu_bit(gb, 4, B);
  if (!(F & FZ)) { CYCT(b_+137, b_+139); fileSelectMode1__leftOrRight_hook(gb); return; }
  CYC(b_+137, b_+139); CYC(b_+139, b_+141); C = 0xff;
  CYC(b_+141, b_+143); alu_bit(gb, 5, B);
  if (!(F & FZ)) { CYCT(b_+143, b_+145); fileSelectMode1__leftOrRight_hook(gb); return; }
  CYC(b_+143, b_+145); CYC(b_+145, b_+146); A = B;
  CYC(b_+146, b_+148); alu_and(gb, 9);
  if (F & FZ) { CYCT(b_+148, b_+149); ret_effect(gb); return; }
  CYC(b_+148, b_+149); CYC(b_+149, b_+151); A = 0x56;
  CALL_C(b_+151, playSound_b00_hook, SYM(playSound_b00), b_+154);
  CALL_C(b_+154, incFileSelectMode2_hook, SYM(incFileSelectMode2), b_+157);
  CALL_C(b_+157, saveFile_b00_hook, SYM(saveFile_b00), b_+160);
  CYC(b_+160, b_+163); TAIL(fadeoutToWhite);
}

void fileSelectMode1__back_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+163, b_+165); A = 8;
  CALL_C(b_+165, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+168);
  CYC(b_+168, b_+171); TAIL(decFileSelectMode2);
}

void fileSelectMode1__leftOrRight_hook(GB *gb) {
  BASE(fileSelectMode1);
  CYC(b_+171, b_+174); SET_HL(wTextSpeed);
  CYC(b_+174, b_+175); A = mem_rd(gb, HL);
  CYC(b_+175, b_+176); alu_add(gb, C);
  CYC(b_+176, b_+178); alu_cp(gb, 5);
  if (!(F & FC)) { CYCT(b_+178, b_+179); ret_effect(gb); return; }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+180); mem_wr(gb, HL, A);
  CYC(b_+180, b_+182); A = 0x84;
  CYC(b_+182, b_+185); TAIL(playSound_b00);
}

void fileSelectMode1__textSpeedMenu_addCursorToOam_hook(GB *gb) {
  BASE(fileSelectMode1);
  CYC(b_+185, b_+188); A = W8(wTextSpeed);
  CYC(b_+188, b_+190); A = alu_swap(gb, A);
  CYC(b_+190, b_+191); C = A;
  CYC(b_+191, b_+193); B = 0;
  CYC(b_+193, b_+196); SET_HL(b_+199);
  CYC(b_+196, b_+199); TAIL(addSpritesToOam_withOffset);
}

void fileSelectMode5_hook(GB *gb) {
  BASE(fileSelectMode5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode5__mode5States_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); TAIL(fileSelectDrawAcornCursor);
}

void fileSelectMode5__mode5States_hook(GB *gb) {
  BASE(fileSelectMode5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); A = W8(wFileSelect_mode2);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+14) { fileSelectMode5__state0_hook(gb); return; }
    else if (jt_ == b_+42) { fileSelectMode5__state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode5__state0_hook(GB *gb) {
  BASE(fileSelectMode5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+14, disableLcd_hook, SYM(disableLcd), b_+17);
  CYC(b_+17, b_+19); A = 0xa7;
  CALL_C(b_+19, loadGfxHeader_hook, SYM(loadGfxHeader), b_+22);
  CYC(b_+22, b_+24); A = 0xa6;
  CALL_C(b_+24, loadGfxHeader_hook, SYM(loadGfxHeader), b_+27);
  CYC(b_+27, b_+29); A = 8;
  CALL_C(b_+29, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+32);
  CALL_C(b_+32, setFileSelectCursorOffsetToFileSelectMode_hook, SYM(setFileSelectCursorOffsetToFileSelectMode), b_+35);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CALL_C(b_+36, func_02_4149_hook, SYM(func_02_4149), b_+39);
  CYC(b_+39, b_+42); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode5__state1_hook(GB *gb) {
  BASE(fileSelectMode5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+42, b_+45); A = W8(wKeysJustPressed);
  CYC(b_+45, b_+47); C = 1;
  CYC(b_+47, b_+49); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+49, b_+51); fileSelectMode5__upOrDown_hook(gb); return; }
  CYC(b_+49, b_+51); CYC(b_+51, b_+53); C = 0xff;
  CYC(b_+53, b_+55); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); fileSelectMode5__upOrDown_hook(gb); return; }
  CYC(b_+55, b_+57); CYC(b_+57, b_+58); C = A;
  CYC(b_+58, b_+60); alu_and(gb, 6);
  if (!(F & FZ)) { CYCT(b_+60, b_+63); setFileSelectModeTo1_hook(gb); return; }
  CYC(b_+60, b_+63); CYC(b_+63, b_+64); A = C;
  CYC(b_+64, b_+66); alu_and(gb, 9);
  if (F & FZ) { CYCT(b_+66, b_+67); ret_effect(gb); return; }
  CYC(b_+66, b_+67); CYC(b_+67, b_+70); A = W8(wFileSelect_cursorPos);
  CYC(b_+70, b_+73); SET_HL(b_+83);
  CYC(b_+73, b_+74); push_effect(gb, b_+74); add_a_to_hl(gb);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CALL_C(b_+75, setFileSelectMode_hook, SYM(setFileSelectMode), b_+78);
  CYC(b_+78, b_+80); A = 0x56;
  CYC(b_+80, b_+83); TAIL(playSound_b00);
}

void fileSelectMode5__upOrDown_hook(GB *gb) {
  BASE(fileSelectMode5);
  CYC(b_+86, b_+89); SET_HL(wFileSelect_cursorPos);
  CYC(b_+89, b_+90); A = mem_rd(gb, HL);
  do {
    CYC(b_+90, b_+91); alu_add(gb, C);
    CYC(b_+91, b_+93); alu_and(gb, 3);
    CYC(b_+93, b_+95); alu_cp(gb, 3);
    if (!(F & FC)) { CYCT(b_+95, b_+97); continue; }
    CYC(b_+95, b_+97);
    break;
  } while (true);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A);
  CYC(b_+98, b_+100); A = 0x84;
  CYC(b_+100, b_+103); TAIL(playSound_b00);
}

void fileSelectMode3_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode3__mode3Update_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); TAIL(fileSelectDrawAcornCursor);
}

void fileSelectMode3__mode3Update_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); A = W8(wFileSelect_mode2);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+18) { fileSelectMode3__mode0_hook(gb); return; }
    else if (jt_ == b_+48) { fileSelectMode3__mode1_hook(gb); return; }
    else if (jt_ == b_+110) { fileSelectMode3__mode2_hook(gb); return; }
    else if (jt_ == b_+141) { fileSelectMode3__mode3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode3__mode0_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+18, setFileSelectCursorOffsetToFileSelectMode_hook, SYM(setFileSelectCursorOffsetToFileSelectMode), b_+21);
  CYC(b_+21, b_+23); A = 3;
  CALL_C(b_+23, func_02_4149_hook, SYM(func_02_4149), b_+26);
  CALL_C(b_+26, disableLcd_hook, SYM(disableLcd), b_+29);
  CYC(b_+29, b_+31); A = 0xa3;
  CALL_C(b_+31, loadGfxHeader_hook, SYM(loadGfxHeader), b_+34);
  CALL_C(b_+34, loadFileDisplayVariables_hook, SYM(loadFileDisplayVariables), b_+37);
  CALL_C(b_+37, textInput_updateEntryCursor_hook, SYM(textInput_updateEntryCursor), b_+40);
  CYC(b_+40, b_+42); A = 8;
  CALL_C(b_+42, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+45);
  CYC(b_+45, b_+48); TAIL(loadGfxRegisterState5AndIncFileSelectMode2);
}

void fileSelectMode3__mode1_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+48, fileSelectUpdateInput_hook, SYM(fileSelectUpdateInput), b_+51);
  if (F & FZ) { CYCT(b_+51, b_+52); ret_effect(gb); return; }
  CYC(b_+51, b_+52); CYC(b_+52, b_+54); A = 0x56;
  CALL_C(b_+54, playSound_b00_hook, SYM(playSound_b00), b_+57);
  CYC(b_+57, b_+60); A = W8(wFileSelect_cursorPos);
  CYC(b_+60, b_+62); alu_cp(gb, 3);
  if (F & FZ) { CYCT(b_+62, b_+65); setFileSelectModeTo1_hook(gb); return; }
  CYC(b_+62, b_+65); CYC(b_+65, b_+67); H8(hActiveFileSlot) = A;
  CYC(b_+67, b_+69); D = 0;
  CALL_C(b_+69, getFileDisplayVariableAddress_hook, SYM(getFileDisplayVariableAddress), b_+72);
  CYC(b_+72, b_+74); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+74, b_+76); goto copy_selected; }
  CYC(b_+74, b_+76); CYC(b_+76, b_+78); A = 0x5a;
  CYC(b_+78, b_+81); playSound_b00_hook(gb);
  return;
copy_selected:
  CYC(b_+81, b_+82); alu_xor(gb, A);
  CYC(b_+82, b_+85); W8(wFileSelect_cursorOffset) = A;
  CALL_C(b_+85, func_02_4149_hook, SYM(func_02_4149), b_+88);
  CALL_C(b_+88, incFileSelectMode2_hook, SYM(incFileSelectMode2), b_+91);
  CYC(b_+91, b_+93); B = 1;
  CYC(b_+93, b_+94); A = B;
  CYC(b_+94, b_+95); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+95, b_+96); ret_effect(gb); return; }
  CYC(b_+95, b_+96);
collision_check:
  CYC(b_+96, b_+99); SET_HL(wFileSelect_cursorPos);
  CYC(b_+99, b_+101); A = H8(hActiveFileSlot);
  CYC(b_+101, b_+102); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; }
  CYC(b_+102, b_+103); CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+105); alu_add(gb, B);
  CYC(b_+105, b_+107); alu_and(gb, 3);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A);
  CYC(b_+108, b_+110);
  goto collision_check;
}

void fileSelectMode3__mode2_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+110, fileSelectMode3__func_02_4397_hook, b_+179, b_+113);
  CALL_C(b_+113, decFileSelectMode2IfBPressed_hook, SYM(decFileSelectMode2IfBPressed), b_+116);
  if (!(F & FZ)) { CYCT(b_+116, b_+118); fileSelectMode3__label_02_015_hook(gb); return; }
  CYC(b_+116, b_+118);
  CALL_C(b_+118, fileSelectUpdateInput_hook, SYM(fileSelectUpdateInput), b_+121);
  if (F & FZ) { CYCT(b_+121, b_+123); goto next_cursor; }
  CYC(b_+121, b_+123); CYC(b_+123, b_+125); A = 0x56;
  CALL_C(b_+125, playSound_b00_hook, SYM(playSound_b00), b_+128);
  CYC(b_+128, b_+131); A = W8(wFileSelect_cursorPos);
  CYC(b_+131, b_+133); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(b_+133, b_+136); incFileSelectMode2_hook(gb); return; }
  CYC(b_+133, b_+136);
  CALL_C(b_+136, decFileSelectMode2_hook, SYM(decFileSelectMode2), b_+139);
  CYC(b_+139, b_+141);
  TAIL(fileSelectMode3__label_02_015);
next_cursor:
  CYC(b_+93, b_+94); A = B;
  CYC(b_+94, b_+95); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+95, b_+96); ret_effect(gb); return; }
  CYC(b_+95, b_+96);
collision_check:
  CYC(b_+96, b_+99); SET_HL(wFileSelect_cursorPos);
  CYC(b_+99, b_+101); A = H8(hActiveFileSlot);
  CYC(b_+101, b_+102); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; }
  CYC(b_+102, b_+103); CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+105); alu_add(gb, B); CYC(b_+105, b_+107); alu_and(gb, 3);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A); CYC(b_+108, b_+110);
  goto collision_check;
}

void fileSelectMode3__mode3_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+141, fileSelectMode3__func_02_4397_hook, b_+179, b_+144);
  CALL_C(b_+144, decFileSelectMode2IfBPressed_hook, SYM(decFileSelectMode2IfBPressed), b_+147);
  if (!(F & FZ)) { CYCT(b_+147, b_+149); fileSelectMode3__label_02_015_hook(gb); return; }
  CYC(b_+147, b_+149);
  CALL_C(b_+149, func_02_448d_hook, SYM(func_02_448d), b_+152);
  if (F & FZ) { CYCT(b_+152, b_+153); ret_effect(gb); return; }
  CYC(b_+152, b_+153); CYC(b_+153, b_+155); A = 0x56;
  CALL_C(b_+155, playSound_b00_hook, SYM(playSound_b00), b_+158);
  CYC(b_+158, b_+161); A = W8(wFileSelect_cursorPos2);
  CYC(b_+161, b_+162); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+162, b_+165); setFileSelectModeTo1_hook(gb); return; }
  CYC(b_+162, b_+165);
  CALL_C(b_+165, loadFile_b00_hook, SYM(loadFile_b00), b_+168);
  CYC(b_+168, b_+171); A = W8(wFileSelect_cursorPos);
  CYC(b_+171, b_+173); H8(hActiveFileSlot) = A;
  CALL_C(b_+173, saveFile_b00_hook, SYM(saveFile_b00), b_+176);
  CYC(b_+176, b_+179); TAIL(setFileSelectModeTo1);
}

void fileSelectMode3__func_02_4397_hook(GB *gb) {
  BASE(fileSelectMode3);
  CYC(b_+179, b_+181); A = H8(hActiveFileSlot);
  CYC(b_+181, b_+184); SET_HL(b_+194);
  CYC(b_+184, b_+185); push_effect(gb, b_+185); add_a_to_hl(gb);
  CYC(b_+185, b_+186); B = mem_rd(gb, HL);
  CYC(b_+186, b_+188); C = 0;
  CYC(b_+188, b_+191); SET_HL(b_+197);
  CYC(b_+191, b_+194); TAIL(addSpritesToOam_withOffset);
}

void fileSelectMode3__label_02_015_hook(GB *gb) {
  BASE(fileSelectMode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+222, b_+224); A = 0x50;
  CALL_C(b_+224, playSound_b00_hook, SYM(playSound_b00), b_+227);
  CYC(b_+227, b_+230); A = W8(wFileSelect_mode2);
  CYC(b_+230, b_+232); alu_cp(gb, 1);
  CYC(b_+232, b_+235); A = W8(wFileSelect_cursorPos);
  if (!(F & FZ)) { CYCT(b_+235, b_+237); goto update_cursor; }
  CYC(b_+235, b_+237);
  CALL_C(b_+237, setFileSelectCursorOffsetToFileSelectMode_hook, SYM(setFileSelectCursorOffsetToFileSelectMode), b_+240);
  CYC(b_+240, b_+242); A = H8(hActiveFileSlot);
update_cursor:
  CYC(b_+242, b_+245); TAIL(func_02_4149);
}

void inventoryMenuState2_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, inventoryMenuState2__subStates_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); TAIL(createBlankSpritesForItemSubmenu);
}

void inventoryMenuState2__subStates_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); A = W8(wSubmenuState);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+16) { inventoryMenuState2__subState0_hook(gb); return; }
    else if (jt_ == b_+85) { inventoryMenuState2__subState1_hook(gb); return; }
    else if (jt_ == b_+107) { inventoryMenuState2__subState2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void inventoryMenuState2__subState0_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+16, b_+19); SET_HL(wSelectedHarpSong);
  CYC(b_+19, b_+20); D = mem_rd(gb, HL);
  CYC(b_+20, b_+21); D = alu_dec8(gb, D);
  CYC(b_+21, b_+23); L = (uint8_t)wSatchelSelectedSeeds;
  CALL_C(b_+23, cpInventorySelectedItemToHarp_hook, SYM(cpInventorySelectedItemToHarp), b_+26);
  if (F & FZ) { CYCT(b_+26, b_+28); goto found; }
  CYC(b_+26, b_+28); CYC(b_+28, b_+30); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(b_+30, b_+32); goto selected; }
  CYC(b_+30, b_+32); CYC(b_+32, b_+33); L = alu_inc8(gb, L);
selected:
  CYC(b_+33, b_+34); E = mem_rd(gb, HL);
  CYC(b_+34, b_+36); D = 0;
  do {
    CYC(b_+36, b_+37); A = D;
    CALL_C(b_+37, getSeedTypeInventoryIndex_hook, SYM(getSeedTypeInventoryIndex), b_+40);
    CYC(b_+40, b_+41); alu_cp(gb, E);
    if (F & FZ) { CYCT(b_+41, b_+43); break; }
    CYC(b_+41, b_+43); CYC(b_+43, b_+44); D = alu_inc8(gb, D);
    CYC(b_+44, b_+46);
  } while (true);
found:
  CYC(b_+46, b_+47); A = D;
  CYC(b_+47, b_+50); W8(wInventory_itemSubmenuIndex) = A;
  CYC(b_+50, b_+53); A = W8(wInventory_cbb8);
  CYC(b_+53, b_+56); SET_HL(b_+236);
  CYC(b_+56, b_+57); push_effect(gb, b_+57); add_a_to_hl(gb);
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+61); SET_HL(wInventory_itemSubmenuMaxWidth);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+62, b_+63); alu_xor(gb, A);
  CYC(b_+63, b_+64); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+64, b_+65); A = alu_inc8(gb, A);
  CYC(b_+65, b_+66); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+66, b_+69); W8(wInventory_itemSubmenuCounter) = A;
  CYC(b_+69, b_+72); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+72, b_+74); alu_cp(gb, 8);
  CYC(b_+74, b_+76); A = 0x0a;
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto store_width; }
  CYC(b_+76, b_+78); CYC(b_+78, b_+80); alu_add(gb, 0xa0);
store_width:
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+81, b_+84); SET_HL(wSubmenuState);
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(inventoryMenuState2__subState1);
}

void inventoryMenuState2__subState1_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+85, b_+88); SET_HL(wInventory_itemSubmenuCounter);
  CYC(b_+88, b_+89); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+89, b_+90); ret_effect(gb); return; }
  CYC(b_+89, b_+90); CYC(b_+90, b_+92); mem_wr(gb, HL, 2);
  CALL_C(b_+92, inventoryMenuState2__func_02_57f3_hook, b_+190, b_+95);
  if (F & FC) { CYCT(b_+95, b_+97); goto done; }
  CYC(b_+95, b_+97); CALL_C(b_+97, func_02_5a35_hook, SYM(func_02_5a35), b_+100);
  CYC(b_+100, b_+103); SET_HL(wSubmenuState);
  CYC(b_+103, b_+104); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
done:
  CYC(b_+104, b_+107); TAIL(func_02_55a8);
}

void inventoryMenuState2__subState2_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+107, b_+110); A = W8(wKeysJustPressed);
  CYC(b_+110, b_+112); alu_and(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+112, b_+114); inventoryMenuState2__buttonPressed_hook(gb); return; }
  CYC(b_+112, b_+114); CALL_C(b_+114, func_02_5938_hook, SYM(func_02_5938), b_+117);
  CALL_C(b_+117, cpInventorySelectedItemToHarp_hook, SYM(cpInventorySelectedItemToHarp), b_+120);
  CYC(b_+120, b_+123); A = W8(wInventory_itemSubmenuIndex);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); goto seed; }
  CYC(b_+123, b_+125); CYC(b_+125, b_+127); alu_add(gb, 0x25); CYC(b_+127, b_+129); goto display;
seed:
  CALL_C(b_+129, getSeedTypeInventoryIndex_hook, SYM(getSeedTypeInventoryIndex), b_+132);
  CYC(b_+132, b_+134); alu_add(gb, 0x20);
display:
  CALL_C(b_+134, loadTreasureDisplayData_b00_hook, SYM(loadTreasureDisplayData_b00), b_+137);
  CYC(b_+137, b_+139); A = 6;
  CYC(b_+139, b_+140); push_effect(gb, b_+140); add_a_to_hl(gb);
  CYC(b_+140, b_+143); A = W8(wInventory_selectedItem);
  CYC(b_+143, b_+145); alu_cp(gb, 0x0f);
  CYC(b_+145, b_+147); A = 0;
  if (!(F & FZ)) { CYCT(b_+147, b_+149); goto text; }
  CYC(b_+147, b_+149); CYC(b_+149, b_+151); A = 5;
text:
  CYC(b_+151, b_+152); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+152, showItemText2_hook, SYM(showItemText2), b_+155);
  CYC(b_+155, b_+158); TAIL(func_02_5a35);
}

void inventoryMenuState2__buttonPressed_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+158, cpInventorySelectedItemToHarp_hook, SYM(cpInventorySelectedItemToHarp), b_+161);
  if (!(F & FZ)) { CYCT(b_+161, b_+163); goto seed; }
  CYC(b_+161, b_+163); CYC(b_+163, b_+165); E = (uint8_t)wSelectedHarpSong;
  CYC(b_+165, b_+168); A = W8(wInventory_itemSubmenuIndex);
  CYC(b_+168, b_+169); A = alu_inc8(gb, A); CYC(b_+169, b_+171); goto set;
seed:
  CYC(b_+171, b_+173); E = (uint8_t)wSatchelSelectedSeeds;
  CYC(b_+173, b_+175); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(b_+175, b_+177); goto index; }
  CYC(b_+175, b_+177); CYC(b_+177, b_+178); E = alu_inc8(gb, E);
index:
  CYC(b_+178, b_+181); A = W8(wInventory_itemSubmenuIndex);
  CALL_C(b_+181, getSeedTypeInventoryIndex_hook, SYM(getSeedTypeInventoryIndex), b_+184);
set:
  CYC(b_+184, b_+186); D = (uint8_t)(wSatchelSelectedSeeds >> 8);
  CYC(b_+186, b_+187); mem_wr(gb, DE, A);
  CYC(b_+187, b_+190); TAIL(inventoryMenuState1__finalizeEquip);
}

void inventoryMenuState2__func_02_57f3_hook(GB *gb) {
  BASE(inventoryMenuState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+190, b_+193); SET_HL(wInventory_itemSubmenuMaxWidth);
  CYC(b_+193, b_+194); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+194, b_+195); C = A;
  CYC(b_+195, b_+196); A = mem_rd(gb, HL);
  CYC(b_+196, b_+197); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+197, b_+199); goto max; }
  CYC(b_+197, b_+199); CYC(b_+199, b_+201); alu_add(gb, 2);
  CYC(b_+201, b_+202); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+202, b_+203); SET_HL(HL + 1);
  CYC(b_+203, b_+204); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+204, b_+206); goto fill;
max:
  CYC(b_+206, b_+207); SET_HL(HL + 1);
  CYC(b_+207, b_+208); A = mem_rd(gb, HL);
  CYC(b_+208, b_+210); alu_cp(gb, 4);
  if (!(F & FC)) { CYCT(b_+210, b_+211); ret_effect(gb); return; }
  CYC(b_+210, b_+211); CYC(b_+211, b_+212); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
fill:
  CYC(b_+212, b_+214); L = (uint8_t)wInventory_itemSubmenuWidth;
  CYC(b_+214, b_+215); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+215, b_+216); C = A;
  CYC(b_+216, b_+217); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+217, b_+218); B = A;
  CYC(b_+218, b_+219); A = mem_rd(gb, HL);
  CYC(b_+219, b_+222); SET_HL(w4TileMap + 0x80);
  CYC(b_+222, b_+223); push_effect(gb, b_+223); add_a_to_hl(gb);
  CYC(b_+223, b_+226); SET_DE(0x0101);
  CYC(b_+226, b_+227); A = B;
  CYC(b_+227, b_+229); alu_cp(gb, 4);
  if (F & FZ) { CYCT(b_+229, b_+231); goto rectangle; }
  CYC(b_+229, b_+231); CYC(b_+231, b_+233); E |= 0x80;
rectangle:
  CALL_C(b_+233, fillRectangleInTilemap_hook, SYM(fillRectangleInTilemap), b_+236);
  CYC(b_+236, b_+237); alu_scf(gb);
  CYC(b_+237, b_+238); ret_effect(gb);
}

void inventoryMenuState3_hook(GB *gb) {
  BASE(inventoryMenuState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wSubmenuState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+10) { inventoryMenuState3__subState0_hook(gb); return; }
    else if (jt_ == b_+46) { inventoryMenuState3__subState1_hook(gb); return; }
    else if (jt_ == b_+70) { inventoryMenuState3__subState2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void inventoryMenuState3__subState0_hook(GB *gb) {
  BASE(inventoryMenuState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+10, b_+13); SET_HL(wInventorySubmenu);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);
  CYC(b_+15, b_+17); alu_cp(gb, 3);
  if (F & FC) { CYCT(b_+17, b_+19); goto set_submenu; }
  CYC(b_+17, b_+19); CYC(b_+19, b_+20); alu_xor(gb, A);
set_submenu:
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+24); A = W8(wInventory_cbba);
  CYC(b_+24, b_+26); alu_xor(gb, 1);
  CYC(b_+26, b_+29); W8(wInventory_cbba) = A;
  CALL_C(b_+29, func_02_55b2_hook, SYM(func_02_55b2), b_+32);
  CYC(b_+32, b_+34); A = 0x9f;
  CYC(b_+34, b_+37); W8(wGfxRegs2_WINX) = A;
  CYC(b_+37, b_+40); SET_HL(wSubmenuState);
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+43); A = 0x54;
  CALL_C(b_+43, playSound_b00_hook, SYM(playSound_b00), b_+46);
  TAIL(inventoryMenuState3__subState1);
}

void inventoryMenuState3__subState1_hook(GB *gb) {
  BASE(inventoryMenuState3);
  CYC(b_+46, b_+49); SET_BC(0x070c);
  CYC(b_+49, b_+52); A = W8(wGfxRegs2_WINX);
  CYC(b_+52, b_+53); alu_sub(gb, C);
  CYC(b_+53, b_+54); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+54, b_+56); goto store; }
  CYC(b_+54, b_+56); CYC(b_+56, b_+57); A = B;
store:
  CYC(b_+57, b_+60); W8(wGfxRegs2_WINX) = A;
  CYC(b_+60, b_+63); A = W8(wGfxRegs2_SCX);
  CYC(b_+63, b_+64); alu_add(gb, C);
  CYC(b_+64, b_+67); W8(wGfxRegs2_SCX) = A;
  CYC(b_+67, b_+69); alu_cp(gb, 0x98);
  if (F & FC) { CYCT(b_+69, b_+70); ret_effect(gb); return; }
  CYC(b_+69, b_+70);
  TAIL(inventoryMenuState3__subState2);
}

void inventoryMenuState3__subState2_hook(GB *gb) {
  BASE(inventoryMenuState3);
  CYC(b_+70, b_+72); A = 0xc7;
  CYC(b_+72, b_+75); W8(wGfxRegs2_WINX) = A;
  CYC(b_+75, b_+76); alu_xor(gb, A);
  CYC(b_+76, b_+79); W8(wGfxRegs2_SCX) = A;
  CYC(b_+79, b_+82); A = W8(wGfxRegs2_LCDC);
  CYC(b_+82, b_+84); alu_xor(gb, 0x48);
  CYC(b_+84, b_+87); W8(wGfxRegs2_LCDC) = A;
  CYC(b_+87, b_+89); A = 1;
  CYC(b_+89, b_+92); TAIL(inventoryMenuState1__func_02_5606);
}

void getDirectionButtonOffsetFromHl_hook(GB *gb) {
  BASE(getDirectionButtonOffsetFromHl);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getInputWithAutofire_hook, SYM(getInputWithAutofire), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0xf0);
  CYC(b_+5, b_+7); A = alu_swap(gb, A);
  CALL_C(b_+7, getLowestSetBit_hook, SYM(getLowestSetBit), b_+10);
  if (!(F & FC)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); push_effect(gb, b_+12); add_a_to_hl(gb);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_or(gb, A);
  CYC(b_+14, b_+15); alu_scf(gb);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void inventorySubscreen0CheckDirectionButtons_hook(GB *gb) {
  BASE(inventorySubscreen0CheckDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+19);
  CALL_C(b_+3, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7); CYC(b_+7, b_+10); SET_HL(wInventorySubmenu0CursorPos);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); A = 0x84;
  CYC(b_+16, b_+19); TAIL(playSound_b00);
}

void inventorySubmenu1CheckDirectionButtons_hook(GB *gb) {
  BASE(inventorySubmenu1CheckDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+90);
  CALL_C(b_+3, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CALL_C(b_+10, getRingBoxCapacity_hook, SYM(getRingBoxCapacity), b_+13);
  CYC(b_+13, b_+15); E = 0x0f;
  if (F & FZ) { CYCT(b_+15, b_+17); goto capacity; }
  CYC(b_+15, b_+17); CYC(b_+17, b_+18); A = alu_inc8(gb, A);
capacity:
  CYC(b_+18, b_+19); alu_add(gb, E);
  CYC(b_+19, b_+20); D = A;
  CYC(b_+20, b_+23); SET_HL(wInventorySubmenu1CursorPos);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_bit(gb, 2, B);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); inventorySubmenu1CheckDirectionButtons__upOrDown_hook(gb); return; }
  CYC(b_+26, b_+28); TAIL(inventorySubmenu1CheckDirectionButtons__leftOrRight);
}

void inventorySubmenu1CheckDirectionButtons__leftOrRight_hook(GB *gb) {
  BASE(inventorySubmenu1CheckDirectionButtons);
  do {
    CYC(b_+28, b_+29); alu_add(gb, C);
    CYC(b_+29, b_+30); alu_cp(gb, D);
    if (!(F & FC)) { CYCT(b_+30, b_+32); continue; }
    CYC(b_+30, b_+32); break;
  } while (true);
  CYC(b_+32, b_+34);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+52); A = 0x84;
  CYC(b_+52, b_+55); TAIL(playSound_b00);
}

void inventorySubmenu1CheckDirectionButtons__upOrDown_hook(GB *gb) {
  BASE(inventorySubmenu1CheckDirectionButtons);
  CYC(b_+34, b_+35); alu_cp(gb, E);
  if (!(F & FC)) { CYCT(b_+35, b_+37); inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow_hook(gb); return; }
  CYC(b_+35, b_+37); CYC(b_+37, b_+38); alu_add(gb, C);
  CYC(b_+38, b_+39); alu_cp(gb, E);
  if (F & FC) { CYCT(b_+39, b_+41); goto store; }
  CYC(b_+39, b_+41); TAIL(inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow);
store:
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+52); A = 0x84;
  CYC(b_+52, b_+55); TAIL(playSound_b00);
}

void inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow_hook(GB *gb) {
  BASE(inventorySubmenu1CheckDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  do {
    CYC(b_+42, b_+43); C = A;
    CALL_C(b_+43, inventorySubmenu1CheckDirectionButtons__updateCursorOnRingBoxRow_hook, b_+55, b_+46);
    CYC(b_+46, b_+47); alu_cp(gb, D);
    if (!(F & FC)) { CYCT(b_+47, b_+49); continue; }
    CYC(b_+47, b_+49); break;
  } while (true);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+52); A = 0x84;
  CYC(b_+52, b_+55); TAIL(playSound_b00);
}

void inventorySubmenu1CheckDirectionButtons__updateCursorOnRingBoxRow_hook(GB *gb) {
  BASE(inventorySubmenu1CheckDirectionButtons);
  CYC(b_+55, b_+56); push_effect(gb, HL);
  CYC(b_+56, b_+59); SET_HL(b_+72);
  do {
    CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+60, b_+61); alu_cp(gb, C);
    if (!(F & FZ)) { CYCT(b_+61, b_+63); continue; }
    CYC(b_+61, b_+63); break;
  } while (true);
  CYC(b_+63, b_+65); alu_bit(gb, 3, B);
  if (F & FZ) { CYCT(b_+65, b_+67); goto result; }
  CYC(b_+65, b_+67); CYC(b_+67, b_+68); SET_HL(HL - 1);
  CYC(b_+68, b_+69); SET_HL(HL - 1);
result:
  CYC(b_+69, b_+70); A = mem_rd(gb, HL);
  CYC(b_+70, b_+71); SET_HL(pop_effect(gb));
  CYC(b_+71, b_+72); ret_effect(gb);
}

void inventorySubmenu2CheckDirectionButtons_hook(GB *gb) {
  BASE(inventorySubmenu2CheckDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+44);
  CALL_C(b_+3, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7); CYC(b_+7, b_+10); SET_HL(wInventorySubmenu2CursorPos);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+13); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); inventorySubmenu2CheckDirectionButtons__upOrDown_hook(gb); return; }
  CYC(b_+13, b_+15); TAIL(inventorySubmenu2CheckDirectionButtons__leftOrRight);
}

void inventorySubmenu2CheckDirectionButtons__leftOrRight_hook(GB *gb) {
  BASE(inventorySubmenu2CheckDirectionButtons);
  CYC(b_+15, b_+16); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+18);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); A = 0x84;
  CYC(b_+41, b_+44); TAIL(playSound_b00);
}

void inventorySubmenu2CheckDirectionButtons__upOrDown_hook(GB *gb) {
  BASE(inventorySubmenu2CheckDirectionButtons);
  CYC(b_+18, b_+20); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+20, b_+22); inventorySubmenu2CheckDirectionButtons__upOrDown__leftSide_hook(gb); return; }
  CYC(b_+20, b_+22); TAIL(inventorySubmenu2CheckDirectionButtons__upOrDown__rightSide);
}

void inventorySubmenu2CheckDirectionButtons__upOrDown__rightSide_hook(GB *gb) {
  BASE(inventorySubmenu2CheckDirectionButtons);
  CYC(b_+22, b_+25); SET_HL(wInventory_submenu2CursorPos2);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  do {
    CYC(b_+26, b_+27); alu_add(gb, C);
    CYC(b_+27, b_+29); alu_and(gb, 3);
    CYC(b_+29, b_+31); alu_cp(gb, 3);
    if (!(F & FC)) { CYCT(b_+31, b_+33); continue; }
    CYC(b_+31, b_+33); break;
  } while (true);
  CYC(b_+33, b_+35);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); A = 0x84;
  CYC(b_+41, b_+44); TAIL(playSound_b00);
}

void inventorySubmenu2CheckDirectionButtons__upOrDown__leftSide_hook(GB *gb) {
  BASE(inventorySubmenu2CheckDirectionButtons);
  CYC(b_+35, b_+36); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+36, b_+38); alu_and(gb, 7);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); A = 0x84;
  CYC(b_+41, b_+44); TAIL(playSound_b00);
}

void func_02_5938_hook(GB *gb) {
  BASE(func_02_5938);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wInventory_cbb8);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); SET_HL(b_+34);
  CALL_C(b_+7, getDirectionButtonOffsetFromHl_hook, SYM(getDirectionButtonOffsetFromHl), b_+10);
  if (!(F & FC)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12); CYC(b_+12, b_+15); SET_HL(wTmpcbb5);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+18); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto decrement; }
  CYC(b_+18, b_+20); CYC(b_+20, b_+21); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+21, b_+23); goto store; }
  CYC(b_+21, b_+23); CYC(b_+23, b_+24); alu_xor(gb, A); CYC(b_+24, b_+26); goto store;
decrement:
  CYC(b_+26, b_+27); A = B;
  CYC(b_+27, b_+28); A = alu_dec8(gb, A);
store:
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+31); A = 0x84;
  CYC(b_+31, b_+34); TAIL(playSound_b00);
}

void inventorySubscreen0_drawCursor_hook(GB *gb) {
  BASE(inventorySubscreen0_drawCursor);
  CYC(b_+0, b_+3); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); alu_and(gb, 0x0c);
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+10); A = alu_swap(gb, A);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_and(gb, 3);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+24); SET_HL(b_+27);
  CYC(b_+24, b_+27); TAIL(addSpritesToOam_withOffset);
}

void inventorySubmenu1_drawCursor_hook(GB *gb) {
  BASE(inventorySubmenu1_drawCursor);
  CYC(b_+0, b_+3); A = W8(wInventorySubmenu1CursorPos);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+7); SET_HL(b_+51);
  CYC(b_+7, b_+8); push_effect(gb, b_+8); add_a_to_hl(gb);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0xf0);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
  CYC(b_+16, b_+18); A = alu_swap(gb, A);
  CYC(b_+18, b_+19); alu_rrca(gb);
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+22); D = 2;
  CYC(b_+22, b_+23); A = E;
  CYC(b_+23, b_+25); alu_cp(gb, 4);
  if (F & FZ) { CYCT(b_+25, b_+27); goto sprites; }
  CYC(b_+25, b_+27); CYC(b_+27, b_+29); alu_cp(gb, 9);
  if (F & FZ) { CYCT(b_+29, b_+31); goto sprites; }
  CYC(b_+29, b_+31); CYC(b_+31, b_+33); alu_sub(gb, 0x0e);
  if (F & FZ) { CYCT(b_+33, b_+35); goto sprites; }
  CYC(b_+33, b_+35); CYC(b_+35, b_+36); D = alu_dec8(gb, D);
  CYC(b_+36, b_+37); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+37, b_+39); goto sprites; }
  CYC(b_+37, b_+39); CYC(b_+39, b_+40); D = alu_dec8(gb, D);
sprites:
  CYC(b_+40, b_+41); A = D;
  CYC(b_+41, b_+44); SET_HL(b_+72);
  CYC(b_+44, b_+45); add_double_index_to_hl(gb, b_+45);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+47); H = mem_rd(gb, HL);
  CYC(b_+47, b_+48); L = A;
  CYC(b_+48, b_+51); TAIL(addSpritesToOam_withOffset);
}

void inventorySubmenu2_drawCursor_hook(GB *gb) {
  BASE(inventorySubmenu2_drawCursor);
  CYC(b_+0, b_+3); A = W8(wInventorySubmenu2CursorPos);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto position; }
  CYC(b_+5, b_+7); CYC(b_+7, b_+10); A = W8(wInventory_submenu2CursorPos2);
  CYC(b_+10, b_+12); alu_add(gb, 8);
position:
  CYC(b_+12, b_+13); E = A;
  CYC(b_+13, b_+16); SET_HL(b_+34);
  CYC(b_+16, b_+17); add_double_index_to_hl(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); C = mem_rd(gb, HL);
  CYC(b_+20, b_+21); A = E;
  CYC(b_+21, b_+23); alu_cp(gb, 8);
  CYC(b_+23, b_+26); SET_HL(b_+56);
  if (F & FC) { CYCT(b_+26, b_+28); goto draw; }
  CYC(b_+26, b_+28); CYC(b_+28, b_+31); SET_HL(b_+65);
draw:
  CYC(b_+31, b_+34); TAIL(addSpritesToOam_withOffset);
}

void func_02_5a35_hook(GB *gb) {
  BASE(func_02_5a35);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(0x0500);
  CALL_C(b_+3, cpInventorySelectedItemToHarp_hook, SYM(cpInventorySelectedItemToHarp), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto selected; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_DE(0x0305);
selected:
  CYC(b_+11, b_+12); B = D;
  CYC(b_+12, b_+14); D = 0;
  TAIL(func_02_5a35__next);
}

void func_02_5a35__next_hook(GB *gb) {
  BASE(func_02_5a35);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+14, b_+15); push_effect(gb, BC);
  CYC(b_+15, b_+16); A = E;
  CYC(b_+16, b_+19); SET_HL(wObtainedTreasureFlags + 4);
  CALL_C(b_+19, checkFlag_hook, SYM(checkFlag), b_+22);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
    TAIL(func_02_5a35__dontHaveSubItem);
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); push_effect(gb, DE);
  CYC(b_+25, b_+26); A = D;
  CALL_C(b_+26, func_02_5afc_hook, SYM(func_02_5afc), b_+29);
  CYC(b_+29, b_+30); A = E;
  CYC(b_+30, b_+33); SET_HL(SYM(seedAndHarpSpriteTable));
  CYC(b_+33, b_+34); push_effect(gb, b_+34); add_a_to_hl(gb);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); push_effect(gb, b_+36); add_a_to_hl(gb);
  CALL_C(b_+36, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+39);
  CYC(b_+39, b_+40); SET_DE(pop_effect(gb));
  CYC(b_+40, b_+41); A = E;
  CYC(b_+41, b_+43); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(b_+43, b_+45);
    TAIL(func_02_5a35__seedOnlyCodeDone);
  }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+46); A = E;
  CYC(b_+46, b_+49); SET_HL(wNumEmberSeeds);
  CYC(b_+49, b_+50); push_effect(gb, b_+50); add_a_to_hl(gb);
  CYC(b_+50, b_+51); B = mem_rd(gb, HL);
  CYC(b_+51, b_+54); A = W8(wInventory_cbb8);
  CYC(b_+54, b_+57); SET_HL((SYM(seedAndHarpSpriteTable__sprite7) + 5));
  CYC(b_+57, b_+58); add_double_index_to_hl(gb, b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+60); H = mem_rd(gb, HL);
  CYC(b_+60, b_+61); L = A;
  CYC(b_+61, b_+62); A = D;
  CYC(b_+62, b_+63); push_effect(gb, b_+63); add_a_to_hl(gb);
  CYC(b_+63, b_+64); C = mem_rd(gb, HL);
  CYC(b_+64, b_+67); SET_HL(w4TileMap + 0xc0);
  CYC(b_+67, b_+70); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+70, b_+72); alu_cp(gb, 0x08);
  if (!(F & FC)) CYCT(b_+72, b_+74);
  else {
    CYC(b_+72, b_+74);
    CYC(b_+74, b_+77); SET_HL(w4TileMap + 0x160);
  }
  CYC(b_+77, b_+78); A = C;
  CYC(b_+78, b_+79); push_effect(gb, b_+79); add_a_to_hl(gb);
  CYC(b_+79, b_+80); A = B;
  CYC(b_+80, b_+82); alu_and(gb, 0xf0);
  CYC(b_+82, b_+84); A = alu_swap(gb, A);
  CYC(b_+84, b_+86); alu_add(gb, 0x20);
  CYC(b_+86, b_+87); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+87, b_+88); A = B;
  CYC(b_+88, b_+90); alu_and(gb, 0x0f);
  CYC(b_+90, b_+92); alu_add(gb, 0x20);
  CYC(b_+92, b_+93); mem_wr(gb, HL, A); SET_HL(HL - 1);
  TAIL(func_02_5a35__seedOnlyCodeDone);
}

void func_02_5a35__seedOnlyCodeDone_hook(GB *gb) {
  BASE(func_02_5a35);
  CYC(b_+93, b_+94); D = alu_inc8(gb, D);
  TAIL(func_02_5a35__dontHaveSubItem);
}

void func_02_5a35__dontHaveSubItem_hook(GB *gb) {
  BASE(func_02_5a35);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+94, b_+95); E = alu_inc8(gb, E);
  CYC(b_+95, b_+96); SET_BC(pop_effect(gb));
  CYC(b_+96, b_+97); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+97, b_+99);
    TAIL(func_02_5a35__next);
  }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+102); A = W8(wTmpcbb5);
  CALL_C(b_+102, func_02_5afc_hook, SYM(func_02_5afc), b_+105);
  CYC(b_+105, b_+108); SET_HL(b_+111);
  CYC(b_+108, b_+111); TAIL(addSpritesToOam_withOffset);
}

void cpInventorySelectedItemToHarp_hook(GB *gb) {
  BASE(cpInventorySelectedItemToHarp);
  CYC(b_+0, b_+3); A = W8(wInventory_selectedItem);
  CYC(b_+3, b_+5); alu_cp(gb, 0x11);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void func_02_5afc_hook(GB *gb) {
  BASE(func_02_5afc);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); A = W8(wInventory_cbb8);
  CYC(b_+4, b_+7); SET_HL(GV((SYM(seedAndHarpSpriteTable__sprite7) + 5), 0x5a70));
  CYC(b_+7, b_+8); add_double_index_to_hl(gb, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); H = mem_rd(gb, HL);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+12); A = C;
  CYC(b_+12, b_+13); push_effect(gb, b_+13); add_a_to_hl(gb);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); A = alu_swap(gb, A);
  CYC(b_+16, b_+17); alu_rrca(gb);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); B = 0x20;
  CYC(b_+20, b_+23); A = W8(wInventorySubmenu0CursorPos);
  CYC(b_+23, b_+25); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+25, b_+26); ret_effect(gb); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); B = 0x48;
  CYC(b_+28, b_+29); ret_effect(gb);
}

void getSeedTypeInventoryIndex_hook(GB *gb) {
  BASE(getSeedTypeInventoryIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+2); C = alu_inc8(gb, C);
  CYC(b_+2, b_+5); SET_HL(wObtainedTreasureFlags + 4);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  for (;;) {
    CYC(b_+6, b_+7); B = A;
    CALL_C(b_+7, checkFlag_hook, SYM(checkFlag), b_+10);
    if (F & FZ) CYCT(b_+10, b_+12);
    else {
      CYC(b_+10, b_+12);
      CYC(b_+12, b_+13); C = alu_dec8(gb, C);
      if (F & FZ) {
        CYCT(b_+13, b_+15);
        CYC(b_+19, b_+20); A = B;
        CYC(b_+20, b_+21); ret_effect(gb);
        return;
      }
      CYC(b_+13, b_+15);
    }
    CYC(b_+15, b_+16); A = B;
    CYC(b_+16, b_+17); A = alu_inc8(gb, A);
    CYC(b_+17, b_+19);
  }
}

void drawEquippedSpriteForActiveRing_hook(GB *gb) {
  BASE(drawEquippedSpriteForActiveRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRingBoxCapacity_hook, SYM(getRingBoxCapacity), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+8); A = W8(wActiveRing);
  CYC(b_+8, b_+10); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_HL(wRingBoxContents);
  CYC(b_+14, b_+16); C = 0;
  do {
    CYC(b_+16, b_+17); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+17, b_+19);
      TAIL(drawEquippedSpriteForActiveRing__foundRing);
    }
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+20); SET_HL(HL + 1);
    CYC(b_+20, b_+21); C = alu_inc8(gb, C);
    CYC(b_+21, b_+22); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+22, b_+24); else CYC(b_+22, b_+24);
  } while (!(F & FZ));
  CYC(b_+24, b_+25); ret_effect(gb);
}

void drawEquippedSpriteForActiveRing__foundRing_hook(GB *gb) {
  BASE(drawEquippedSpriteForActiveRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+25, b_+27); A = 0x18;
  CALL_C(b_+27, multiplyAByC_hook, SYM(multiplyAByC), b_+30);
  CYC(b_+30, b_+31); C = L;
  CYC(b_+31, b_+33); B = 0;
  CYC(b_+33, b_+36); SET_HL(b_+39);
  CYC(b_+36, b_+39); TAIL(addSpritesToOam_withOffset);
}

void inventorySubscreen0_drawStoredItems_hook(GB *gb) {
  BASE(inventorySubscreen0_drawStoredItems);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x10;
  do {
    CYC(b_+2, b_+4); H8(hFF8D) = A;
    CYC(b_+4, b_+7); SET_HL(wInventoryStorage - 1);
    CYC(b_+7, b_+8); push_effect(gb, b_+8); add_a_to_hl(gb);
    CYC(b_+8, b_+9); A = mem_rd(gb, HL);
    CALL_C(b_+9, loadTreasureDisplayData_b00_hook, SYM(loadTreasureDisplayData_b00), b_+12);
    CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(b_+13, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+16);
    CYC(b_+16, b_+18); H8(hFF8B) = A;
    CYC(b_+18, b_+20); A = H8(hFF8D);
    CYC(b_+20, b_+23); SET_BC(b_+38);
    CALL_C(b_+23, addDoubleIndexToBc_hook, 0x007e, b_+26);
    CYC(b_+26, b_+27); A = mem_rd(gb, BC);
    CYC(b_+27, b_+28); E = A;
    CYC(b_+28, b_+29); SET_BC(BC + 1);
    CYC(b_+29, b_+30); A = mem_rd(gb, BC);
    CYC(b_+30, b_+31); D = A;
    CALL_C(b_+31, drawTreasureDisplayDataToBg_hook, SYM(drawTreasureDisplayDataToBg), b_+34);
    CYC(b_+34, b_+36); A = H8(hFF8D);
    CYC(b_+36, b_+37); A = alu_dec8(gb, A);
    if (!(F & FZ)) CYCT(b_+37, b_+39); else CYC(b_+37, b_+39);
  } while (!(F & FZ));
  CYC(b_+39, b_+40); ret_effect(gb);
}

void inventorySubscreen1_drawTreasures_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  CYC(b_+0, b_+3); SET_HL(SYM(subscreen1TreasureData));
  TAIL(inventorySubscreen1_drawTreasures__drawTreasure);
}

void inventorySubscreen1_drawTreasures__drawTreasure_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    TAIL(inventorySubscreen1_drawTreasures__undrawRingBox);
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); H8(hFF8C) = A;
  CALL_C(b_+9, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+12);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
    TAIL(inventorySubscreen1_drawTreasures__nextTreasure);
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); H8(hFF8B) = A;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+17, inventorySubscreen1_drawTreasures__getAddressToDrawTreasureAt_hook, b_+124, b_+20);
  CYC(b_+20, b_+21); push_effect(gb, HL);
  CYC(b_+21, b_+23); A = H8(hFF8C);
  CALL_C(b_+23, loadTreasureDisplayData_b00_hook, SYM(loadTreasureDisplayData_b00), b_+26);
  CYC(b_+26, b_+27); SET_HL(HL + 1);
  CALL_C(b_+27, drawTreasureDisplayDataToBg_hook, SYM(drawTreasureDisplayDataToBg), b_+30);
  CYC(b_+30, b_+31); C = mem_rd(gb, HL);
  CYC(b_+31, b_+32); SET_HL(pop_effect(gb));
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+33, b_+36); SET_DE(w4SubscreenTextIndices);
  CALL_C(b_+36, addAToDe_hook, 0x0068, b_+39);
  CYC(b_+39, b_+40); A = C;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  TAIL(inventorySubscreen1_drawTreasures__nextTreasure);
}

void inventorySubscreen1_drawTreasures__nextTreasure_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  CYC(b_+41, b_+42); SET_HL(HL + 1);
  CYC(b_+42, b_+43); SET_HL(HL + 1);
  CYC(b_+43, b_+45);
  TAIL(inventorySubscreen1_drawTreasures__drawTreasure);
}

void inventorySubscreen1_drawTreasures__undrawRingBox_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+45, b_+48); A = W8(wRingBoxLevel);
  CYC(b_+48, b_+50); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+50, b_+52);
    TAIL(inventorySubscreen1_drawTreasures__drawRings);
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); SET_HL(b_+149);
  CYC(b_+55, b_+56); add_double_index_to_hl(gb, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+57, b_+58); C = mem_rd(gb, HL);
  CYC(b_+58, b_+60); B = 0x03;
  CYC(b_+60, b_+61); L = A;
  CYC(b_+61, b_+63); H = (uint8_t)((w4TileMap + 0x100) >> 8);
  CALL_C(b_+63, fillRectangleInTileMapWithMenuBlock_hook, SYM(fillRectangleInTileMapWithMenuBlock), b_+66);
  TAIL(inventorySubscreen1_drawTreasures__drawRings);
}

void inventorySubscreen1_drawTreasures__drawRings_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+66, getRingBoxCapacity_hook, SYM(getRingBoxCapacity), b_+69);
  if (F & FZ) { CYCT(b_+69, b_+70); ret_effect(gb); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+71); B = A;
  TAIL(inventorySubscreen1_drawTreasures__drawRing);
}

void inventorySubscreen1_drawTreasures__drawRing_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+71, b_+72); A = B;
  CYC(b_+72, b_+75); SET_HL(b_+143);
  CYC(b_+75, b_+76); push_effect(gb, b_+76); add_a_to_hl(gb);
  CYC(b_+76, b_+77); E = mem_rd(gb, HL);
  CYC(b_+77, b_+79); D = (uint8_t)((w4TileMap + 0x100) >> 8);
  CYC(b_+79, b_+80); A = B;
  CYC(b_+80, b_+83); SET_HL(wRingBoxContents - 1);
  CYC(b_+83, b_+84); push_effect(gb, b_+84); add_a_to_hl(gb);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+87, b_+89);
    TAIL(inventorySubscreen1_drawTreasures__nextRing);
  }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+90); push_effect(gb, BC);
  CYC(b_+90, b_+91); C = A;
  CYC(b_+91, b_+92); A = B;
  CYC(b_+92, b_+95); SET_HL(w4SubscreenTextIndices + 0x0f);
  CYC(b_+95, b_+96); push_effect(gb, b_+96); add_a_to_hl(gb);
  CYC(b_+96, b_+97); A = C;
  CYC(b_+97, b_+99); alu_or(gb, 0xc0);
  CYC(b_+99, b_+100); mem_wr(gb, HL, A);
  CYC(b_+100, b_+101); A = C;
  CALL_C(b_+101, getRingTiles_hook, SYM(getRingTiles), b_+104);
  CYC(b_+104, b_+105); SET_BC(pop_effect(gb));
  TAIL(inventorySubscreen1_drawTreasures__nextRing);
}

void inventorySubscreen1_drawTreasures__nextRing_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  CYC(b_+105, b_+106); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+108);
    TAIL(inventorySubscreen1_drawTreasures__drawRing);
  }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+111); A = W8(wRingBoxLevel);
  CYC(b_+111, b_+113); alu_add(gb, 0x1c);
  CYC(b_+113, b_+116); mem_wr(gb, w4SubscreenTextIndices + 0x0f, A);
  CYC(b_+116, b_+119); SET_DE(w4TileMap + 0x182);
  CYC(b_+119, b_+121); A = 0xfe;
  CYC(b_+121, b_+124); TAIL(getRingTiles);
}

void inventorySubscreen1_drawTreasures__getAddressToDrawTreasureAt_hook(GB *gb) {
  BASE(inventorySubscreen1_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+124, b_+125); D = A;
  CYC(b_+125, b_+127); alu_and(gb, 0xf0);
  CYC(b_+127, b_+129); A = alu_swap(gb, A);
  CYC(b_+129, b_+130); alu_add(gb, A);
  CALL_C(b_+130, multiplyABy16_hook, SYM(multiplyABy16), b_+133);
  CYC(b_+133, b_+134); A = D;
  CYC(b_+134, b_+136); alu_and(gb, 0x0f);
  CYC(b_+136, b_+137); alu_add(gb, C);
  CYC(b_+137, b_+140); SET_DE(w4TileMap + 0x62);
  CALL_C(b_+140, addAToDe_hook, 0x0068, b_+143);
  CYC(b_+143, b_+144); ret_effect(gb);
}

void inventorySubscreen2_drawTreasures_hook(GB *gb) {
  BASE(inventorySubscreen2_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(itemSubmenu2TextIndices));
  CYC(b_+3, b_+6); SET_DE(w4SubscreenTextIndices);
  CYC(b_+6, b_+8); B = 0x0b;
  CALL_C(b_+8, copyMemory_hook, SYM(copyMemory), b_+11);
  CYC(b_+11, b_+13); B = 0x08;
  TAIL(inventorySubscreen2_drawTreasures__drawEssence);
}

void inventorySubscreen2_drawTreasures__drawEssence_hook(GB *gb) {
  BASE(inventorySubscreen2_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+13, b_+14); A = B;
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+18); SET_HL(wEssencesObtained);
  CALL_C(b_+18, checkFlag_hook, SYM(checkFlag), b_+21);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
    TAIL(inventorySubscreen2_drawTreasures__nextEssence);
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); push_effect(gb, BC);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+28); SET_HL(b_+117);
  CYC(b_+28, b_+29); add_double_index_to_hl(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); H = mem_rd(gb, HL);
  CYC(b_+31, b_+32); L = A;
  CYC(b_+32, b_+35); SET_BC(0x0202);
  CYC(b_+35, b_+38); SET_DE(0x0007);
  CALL_C(b_+38, fillRectangleInTilemap_hook, SYM(fillRectangleInTilemap), b_+41);
  CYC(b_+41, b_+42); SET_BC(pop_effect(gb));
  CYC(b_+42, b_+43); A = B;
  CYC(b_+43, b_+46); SET_HL(w4SubscreenTextIndices - 1);
  CYC(b_+46, b_+47); push_effect(gb, b_+47); add_a_to_hl(gb);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0);
  TAIL(inventorySubscreen2_drawTreasures__nextEssence);
}

void inventorySubscreen2_drawTreasures__nextEssence_hook(GB *gb) {
  BASE(inventorySubscreen2_drawTreasures);
  CYC(b_+49, b_+50); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+50, b_+52);
    TAIL(inventorySubscreen2_drawTreasures__drawEssence);
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+55); A = W8(wNumHeartPieces);
  CYC(b_+55, b_+56); C = A;
  CYC(b_+56, b_+59); SET_HL(w4SubscreenTextIndices + 9);
  CYC(b_+59, b_+60); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+60, b_+61); mem_wr(gb, HL, A);
  CYC(b_+61, b_+62); A = C;
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+63, b_+65);
    TAIL(inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece);
  }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); alu_add(gb, 0x10);
  CYC(b_+67, b_+70); mem_wr(gb, w4TileMap + 0x14f, A);
  CYC(b_+70, b_+73); SET_HL(SYM(itemSubmenu2HeartPieceDisplayData));
  TAIL(inventorySubscreen2_drawTreasures__nextQuarterHeart);
}

void inventorySubscreen2_drawTreasures__nextQuarterHeart_hook(GB *gb) {
  BASE(inventorySubscreen2_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  do {
    CYC(b_+73, b_+74); push_effect(gb, BC);
    CYC(b_+74, b_+75); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+75, b_+78); SET_DE(w4TileMap + 0xce);
    CALL_C(b_+78, addAToDe_hook, 0x0068, b_+81);
    CALL_C(b_+81, drawTreasureDisplayDataToBg_hook, SYM(drawTreasureDisplayDataToBg), b_+84);
    CYC(b_+84, b_+85); SET_BC(pop_effect(gb));
    CYC(b_+85, b_+86); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(b_+86, b_+88); else CYC(b_+86, b_+88);
  } while (!(F & FZ));
  TAIL(inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece);
}

void inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece_hook(GB *gb) {
  BASE(inventorySubscreen2_drawTreasures);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+88, b_+91); A = W8(wTilesetFlags);
  CYC(b_+91, b_+93); alu_and(gb, 0x80);
  CYC(b_+93, b_+94); alu_rlca(gb);
  CYC(b_+94, b_+95); C = A;
  CYC(b_+95, b_+98); SET_HL(w4SubscreenTextIndices + 8);
  CYC(b_+98, b_+99); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+99, b_+100); mem_wr(gb, HL, A);
  CYC(b_+100, b_+101); A = C;
  CYC(b_+101, b_+102); alu_add(gb, A);
  CYC(b_+102, b_+103); alu_add(gb, A);
  CYC(b_+103, b_+104); alu_add(gb, C);
  CYC(b_+104, b_+107); SET_HL(SYM(itemSubmenu2BlurbDisplayData));
  CYC(b_+107, b_+108); add_double_index_to_hl(gb, b_+108);
  CYC(b_+108, b_+111); SET_DE(w4TileMap + 0x6e);
  CALL_C(b_+111, drawTreasureDisplayDataToBg_hook, SYM(drawTreasureDisplayDataToBg), b_+114);
  CYC(b_+114, b_+116); E = 0x70;
  CYC(b_+116, b_+119); TAIL(drawTreasureDisplayDataToBg);
}

void getRingBoxCapacity_hook(GB *gb) {
  BASE(getRingBoxCapacity);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); A = W8(wRingBoxLevel);
  CYC(b_+4, b_+7); SET_HL(b_+12);
  CYC(b_+7, b_+8); push_effect(gb, b_+8); add_a_to_hl(gb);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
  CYC(b_+11, b_+12); ret_effect(gb);
}

void fillRectangleInTileMapWithMenuBlock_hook(GB *gb) {
  BASE(fillRectangleInTileMapWithMenuBlock);
  CYC(b_+0, b_+3); SET_DE(0xe701);
  TAIL(fillRectangleInTilemap);
}

void fillRectangleInTilemap_hook(GB *gb) {
  BASE(fillRectangleInTilemap);
  do {
    CYC(b_+0, b_+1); push_effect(gb, HL);
    CYC(b_+1, b_+2); A = C;
    do {
      CYC(b_+2, b_+3); mem_wr(gb, HL, D);
      CYC(b_+3, b_+5); H |= 0x04;
      CYC(b_+5, b_+6); mem_wr(gb, HL, E);
      CYC(b_+6, b_+8); H &= (uint8_t)~0x04;
      CYC(b_+8, b_+9); SET_HL(HL + 1);
      CYC(b_+9, b_+10); A = alu_dec8(gb, A);
      if (!(F & FZ)) CYCT(b_+10, b_+12); else CYC(b_+10, b_+12);
    } while (!(F & FZ));
    CYC(b_+12, b_+13); SET_HL(pop_effect(gb));
    CYC(b_+13, b_+15); A = 0x20;
    CYC(b_+15, b_+16); push_effect(gb, b_+16); add_a_to_hl(gb);
    CYC(b_+16, b_+17); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+17, b_+19); else CYC(b_+17, b_+19);
  } while (!(F & FZ));
  CYC(b_+19, b_+20); ret_effect(gb);
}

void drawTreasureDisplayDataToBg_hook(GB *gb) {
  BASE(drawTreasureDisplayDataToBg);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); C = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, drawTreasureDisplayDataToBg__writeTile_hook, b_+29, b_+7);
  CYC(b_+7, b_+8); E = alu_inc8(gb, E);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); B = A;
  CALL_C(b_+12, drawTreasureDisplayDataToBg__writeTile_hook, b_+29, b_+15);
  CYC(b_+15, b_+17); A = 0x20;
  CALL_C(b_+17, addAToDe_hook, 0x0068, b_+20);
  CYC(b_+20, b_+22); A = H8(hFF8B);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+25); C = 0x07;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+29); TAIL(drawTreasureExtraTiles);
}

void drawTreasureDisplayDataToBg__writeTile_hook(GB *gb) {
  BASE(drawTreasureDisplayDataToBg);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+29, b_+30); push_effect(gb, DE);
  CYC(b_+30, b_+31); A = C;
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+32, b_+34);
    TAIL(drawTreasureDisplayDataToBg__clearTile);
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); B = alu_inc8(gb, B);
  CYC(b_+35, b_+36); B = alu_inc8(gb, B);
  CYC(b_+36, b_+37); alu_add(gb, A);
  if (!(F & FC)) CYCT(b_+37, b_+39);
  else {
    CYC(b_+37, b_+39);
    CYC(b_+39, b_+41); B |= 0x08;
  }
  CYC(b_+41, b_+42); C = A;
  CALL_C(b_+42, drawTreasureDisplayDataToBg__writeTileHlpr_hook, b_+68, b_+45);
  CYC(b_+45, b_+46); SET_DE(pop_effect(gb));
  CYC(b_+46, b_+47); ret_effect(gb);
}

void drawTreasureDisplayDataToBg__clearTile_hook(GB *gb) {
  BASE(drawTreasureDisplayDataToBg);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+47, b_+49); A = 0x02;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+52); D |= 0x04;
  CYC(b_+52, b_+53); A = alu_dec8(gb, A);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+56); A = 0x20;
  CALL_C(b_+56, addAToDe_hook, 0x0068, b_+59);
  CYC(b_+59, b_+61); A = 0x01;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+64); D &= (uint8_t)~0x04;
  CYC(b_+64, b_+65); A = alu_inc8(gb, A);
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+67); SET_DE(pop_effect(gb));
  CYC(b_+67, b_+68); ret_effect(gb);
}

void drawTreasureDisplayDataToBg__writeTileHlpr_hook(GB *gb) {
  BASE(drawTreasureDisplayDataToBg);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+68, b_+69); A = C;
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  CYC(b_+70, b_+72); D |= 0x04;
  CYC(b_+72, b_+73); A = B;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+76); A = 0x20;
  CALL_C(b_+76, addAToDe_hook, 0x0068, b_+79);
  CYC(b_+79, b_+80); A = B;
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CYC(b_+81, b_+83); D &= (uint8_t)~0x04;
  CYC(b_+83, b_+84); A = C;
  CYC(b_+84, b_+85); A = alu_inc8(gb, A);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); ret_effect(gb);
}

void inventoryMenuDrawSprites_hook(GB *gb) {
  BASE(inventoryMenuDrawSprites);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, inventoryMenuDrawHarpSprites_hook, SYM(inventoryMenuDrawHarpSprites), b_+3);
  CYC(b_+3, b_+5); A = 0x36;
  CALL_C(b_+5, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_BC(0x2068);
  CYC(b_+12, b_+15); A = W8(wMenuActiveState);
  CYC(b_+15, b_+17); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
    TAIL(inventoryMenuDrawSprites__menuScrolling);
  }
  CYC(b_+17, b_+19);
  TAIL(inventoryMenuDrawSprites__drawIfOnSubscreen1);
}

void inventoryMenuDrawSprites__drawIfOnSubscreen1_hook(GB *gb) {
  BASE(inventoryMenuDrawSprites);
  CYC(b_+19, b_+22); A = W8(wInventorySubmenu);
  CYC(b_+22, b_+23); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26);
  TAIL(inventoryMenuDrawSprites__drawSprite);
}

void inventoryMenuDrawSprites__menuScrolling_hook(GB *gb) {
  BASE(inventoryMenuDrawSprites);
  CYC(b_+26, b_+29); A = W8(wSubmenuState);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+30, b_+32);
    TAIL(inventoryMenuDrawSprites__drawIfOnSubscreen1);
  }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+35); A = W8(wInventorySubmenu);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+40);
    CYC(b_+47, b_+50); A = W8(wGfxRegs2_SCX);
    CYC(b_+50, b_+51); alu_cpl(gb);
    CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  } else {
    CYC(b_+38, b_+40);
    CYC(b_+40, b_+43); A = W8(wGfxRegs2_WINX);
    CYC(b_+43, b_+45); alu_sub(gb, 0x07);
    CYC(b_+45, b_+47);
  }
  TAIL(inventoryMenuDrawSprites__drawSpriteWithXOffset);
}

void inventoryMenuDrawSprites__drawSpriteWithXOffset_hook(GB *gb) {
  BASE(inventoryMenuDrawSprites);
  CYC(b_+52, b_+53); alu_add(gb, C);
  CYC(b_+53, b_+54); C = A;
  TAIL(inventoryMenuDrawSprites__drawSprite);
}

void inventoryMenuDrawSprites__drawSprite_hook(GB *gb) {
  BASE(inventoryMenuDrawSprites);
  CYC(b_+54, b_+57); SET_HL(GV(b_+60, 0x5d5f));
  CYC(b_+57, b_+60); TAIL(addSpritesToOam_withOffset);
}

void inventoryMenuDrawHarpSprites_hook(GB *gb) {
  BASE(inventoryMenuDrawHarpSprites);
  CYC(b_+0, b_+3); SET_HL(wInventoryStorage);
  CYC(b_+3, b_+6); SET_BC(0x1000);
  for (;;) {
    CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+7, b_+9); alu_cp(gb, 0x11);
    if (F & FZ) { CYCT(b_+9, b_+11); break; }
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); C = alu_inc8(gb, C);
    CYC(b_+12, b_+13); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+13, b_+15);
    else {
      CYC(b_+13, b_+15);
      CYC(b_+15, b_+16); ret_effect(gb);
      return;
    }
  }
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+19); alu_and(gb, 0xfc);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+21); alu_add(gb, A);
  CYC(b_+21, b_+22); alu_add(gb, B);
  CYC(b_+22, b_+23); alu_add(gb, A);
  CYC(b_+23, b_+25); alu_add(gb, 0x14);
  CYC(b_+25, b_+26); B = A;
  CYC(b_+26, b_+27); A = C;
  CYC(b_+27, b_+29); alu_and(gb, 0x03);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+32); alu_add(gb, A);
  CYC(b_+32, b_+34); alu_add(gb, 0x22);
  CYC(b_+34, b_+35); C = A;
  CYC(b_+35, b_+38); A = W8(wMenuActiveState);
  CYC(b_+38, b_+40); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+40, b_+42);
    goto scrolling;
  }
  CYC(b_+40, b_+42);
subscreen_zero:
  CYC(b_+42, b_+45); A = W8(wInventorySubmenu);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49);
  TAIL(inventoryMenuDrawHarpSprites__drawSprite);
scrolling:
  CYC(b_+49, b_+52); A = W8(wSubmenuState);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+53, b_+55); goto subscreen_zero; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+58); A = W8(wInventorySubmenu);
  CYC(b_+58, b_+60); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+62); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+62, b_+64);
    CYC(b_+71, b_+74); A = W8(wGfxRegs2_SCX);
    CYC(b_+74, b_+75); alu_cpl(gb);
    CYC(b_+75, b_+76); A = alu_inc8(gb, A);
  } else {
    CYC(b_+62, b_+64);
    CYC(b_+64, b_+67); A = W8(wGfxRegs2_WINX);
    CYC(b_+67, b_+69); alu_sub(gb, 0x07);
    CYC(b_+69, b_+71);
  }
  TAIL(inventoryMenuDrawHarpSprites__drawSpriteWithXOffset);
}

void inventoryMenuDrawHarpSprites__drawSpriteWithXOffset_hook(GB *gb) {
  BASE(inventoryMenuDrawHarpSprites);
  CYC(b_+76, b_+77); alu_add(gb, C);
  CYC(b_+77, b_+78); C = A;
  TAIL(inventoryMenuDrawHarpSprites__drawSprite);
}

void inventoryMenuDrawHarpSprites__drawSprite_hook(GB *gb) {
  BASE(inventoryMenuDrawHarpSprites);
  CYC(b_+78, b_+81); A = W8(wSelectedHarpSong);
  CYC(b_+81, b_+84); SET_HL((SYM(seedAndHarpSpriteTable) + 4));
  CYC(b_+84, b_+85); push_effect(gb, b_+85); add_a_to_hl(gb);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+87); push_effect(gb, b_+87); add_a_to_hl(gb);
  CYC(b_+87, b_+90); TAIL(addSpritesToOam_withOffset);
}

void func_02_4149_hook(GB *gb) {
  BASE(func_02_4149);
  CYC(b_+0, b_+3); SET_HL(wFileSelect_cursorPos);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+6); A = 0x80;
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void setFileSelectCursorOffsetToFileSelectMode_hook(GB *gb) {
  BASE(setFileSelectCursorOffsetToFileSelectMode);
  CYC(b_+0, b_+3); A = W8(wFileSelect_mode);
  CYC(b_+3, b_+6); W8(wFileSelect_cursorOffset) = A;
  CYC(b_+6, b_+7); ret_effect(gb);
}

void setFileSelectMode_hook(GB *gb) {
  BASE(setFileSelectMode);
  CYC(b_+0, b_+3); SET_HL(wFileSelect_mode);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+9); W8(wFileSelect_textInputMode) = A;
  CYC(b_+9, b_+10); ret_effect(gb);
}

void setFileSelectModeTo1_hook(GB *gb) {
  BASE(setFileSelectModeTo1);
  CYC(b_+0, b_+2); A = 0x01;
  TAIL(setFileSelectMode);
}

void loadGfxRegisterState5AndIncFileSelectMode2_hook(GB *gb) {
  BASE(loadGfxRegisterState5AndIncFileSelectMode2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;
  CALL_C(b_+2, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), SYM(incFileSelectMode2));
  TAIL(incFileSelectMode2);
}

void incFileSelectMode2_hook(GB *gb) {
  BASE(incFileSelectMode2);
  CYC(b_+0, b_+3); SET_HL(wFileSelect_mode2);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void decFileSelectMode2_hook(GB *gb) {
  BASE(decFileSelectMode2);
  CYC(b_+0, b_+3); SET_HL(wFileSelect_mode2);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void decFileSelectMode2IfBPressed_hook(GB *gb) {
  BASE(decFileSelectMode2IfBPressed);
  CYC(b_+0, b_+3); A = W8(wKeysJustPressed);
  CYC(b_+3, b_+5); alu_and(gb, 0x02);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  TAIL(decFileSelectMode2);
}

void createBlankSpritesForItemSubmenu_hook(GB *gb) {
  BASE(createBlankSpritesForItemSubmenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wInventory_cbc1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x2800);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(b_+13, b_+15); goto select_sprites; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); B = 0x50;
select_sprites:
  CYC(b_+17, b_+19); E = 0x03;
  CYC(b_+19, b_+22); A = W8(wInventory_cbb8);
  CYC(b_+22, b_+24); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto start_loop; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); E = alu_dec8(gb, E);
  CYC(b_+27, b_+29); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto start_loop; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); E = alu_dec8(gb, E);
start_loop:
  CYC(b_+32, b_+33); A = E;
  for (;;) {
    CYC(b_+33, b_+34); A = alu_dec8(gb, A);
    CYC(b_+34, b_+35); push_effect(gb, AF);
    CYC(b_+35, b_+36); push_effect(gb, BC);
    CYC(b_+36, b_+39); SET_HL(b_+50);
    CYC(b_+39, b_+40); push_effect(gb, b_+40); add_a_to_hl(gb);
    CYC(b_+40, b_+41); A = mem_rd(gb, HL);
    CYC(b_+41, b_+42); push_effect(gb, b_+42); add_a_to_hl(gb);
    CALL_C(b_+42, addSpritesToOam_withOffset_hook, SYM(addSpritesToOam_withOffset), b_+45);
    CYC(b_+45, b_+46); SET_BC(pop_effect(gb));
    CYC(b_+46, b_+47); SET_AF(pop_effect(gb));
    if (!(F & FZ)) { CYCT(b_+47, b_+49); continue; }
    CYC(b_+47, b_+49);
    CYC(b_+49, b_+50); ret_effect(gb);
    return;
  }
}

static void dungeon_map_generate_scrollable_tilemap(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(dungeonMap_generateScrollableTilemap);
  if (entry == b_+0) {
    CYC(b_+0, b_+3); SET_DE(w4GfxBuf1);
    CYC(b_+3, b_+5); A = 0x28;
    CALL_C(b_+5, dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook, b_+67, b_+8);
    CYC(b_+8, b_+11); A = W8(wDungeonNumFloors);
    CYC(b_+11, b_+13); H8(hFF8D) = A;
  } else if (entry == b_+30) {
    goto next_tile;
  } else if (entry == b_+55) {
    goto done_this_floor;
  } else if (entry == b_+67) {
    goto fill_tilemap_with_blank;
  }
next_floor:
  CYC(b_+13, b_+15); A = H8(hFF8D);
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  CALL_C(b_+16, dungeonMap_getFloorAddress_hook, SYM(dungeonMap_getFloorAddress), b_+19);
  CALL_C(b_+19, dungeonMap_checkCanViewFloor_hook, SYM(dungeonMap_checkCanViewFloor), b_+22);
  CYC(b_+22, b_+24); A = 0x50;
  if (F & FZ) { CYCT(b_+24, b_+26); goto done_this_floor; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x40;
  CYC(b_+28, b_+30); H8(hFF8C) = A;
next_tile:
  CYC(b_+30, b_+32); A = 0x02;
  CYC(b_+32, b_+34); mem_wr(gb, IO_SVBK, A);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+38); A = 0x04;
  CYC(b_+38, b_+40); mem_wr(gb, IO_SVBK, A);
  CYC(b_+40, b_+41); A = C;
  CALL_C(b_+41, dungeonMap_getTileForRoom_hook, SYM(dungeonMap_getTileForRoom), b_+44);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); SET_DE(DE + 1);
  CYC(b_+46, b_+48); A = H8(hFF8C);
  CYC(b_+48, b_+49); A = alu_dec8(gb, A);
  CYC(b_+49, b_+51); H8(hFF8C) = A;
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto next_tile; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); A = 0x10;
done_this_floor:
  CALL_C(b_+55, dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook, b_+67, b_+58);
  CYC(b_+58, b_+60); A = H8(hFF8D);
  CYC(b_+60, b_+61); A = alu_dec8(gb, A);
  CYC(b_+61, b_+63); H8(hFF8D) = A;
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto next_floor; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); A = 0x18;
fill_tilemap_with_blank:
  CYC(b_+67, b_+68); push_effect(gb, BC);
  CYC(b_+68, b_+69); C = A;
  CYC(b_+69, b_+71); A = 0xad;
  for (;;) {
    CYC(b_+71, b_+72); mem_wr(gb, DE, A);
    CYC(b_+72, b_+73); SET_DE(DE + 1);
    CYC(b_+73, b_+74); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+74, b_+76); continue; }
    CYC(b_+74, b_+76);
    CYC(b_+76, b_+77); SET_BC(pop_effect(gb));
    CYC(b_+77, b_+78); ret_effect(gb);
    return;
  }
}

void dungeonMap_generateScrollableTilemap_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, SYM(dungeonMap_generateScrollableTilemap), sp0_); }
void dungeonMap_generateScrollableTilemap__nextFloor67eb_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, SYM(dungeonMap_generateScrollableTilemap__nextFloor), sp0_); }
void dungeonMap_generateScrollableTilemap__nextTile67fc_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, SYM(dungeonMap_generateScrollableTilemap__nextTile), sp0_); }
void dungeonMap_generateScrollableTilemap__doneThisFloor6815_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, SYM(dungeonMap_generateScrollableTilemap__doneThisFloor), sp0_); }
void dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, SYM(dungeonMap_generateScrollableTilemap__fillTileMapWithBlank), sp0_); }

static void dungeon_map_check_compass_tile(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(dungeonMap_checkCompassTile);
  if (entry == b_+0) {
    CALL_C(b_+0, checkLinkHasCompass_hook, SYM(checkLinkHasCompass), b_+3);
    if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
    CYC(b_+3, b_+4);
    CYC(b_+4, b_+5); A = E;
    CYC(b_+5, b_+7); C = 0x83;
    CYC(b_+7, b_+9); alu_and(gb, 0x70);
    CYC(b_+9, b_+11); alu_cp(gb, 0x40);
    if (F & FZ) { CYCT(b_+11, b_+13); goto ret; }
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+15); alu_cp(gb, 0x20);
    if (F & FZ) { CYCT(b_+15, b_+17); goto treasure; }
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+19); alu_cp(gb, 0x30);
    if (!(F & FZ)) { CYCT(b_+19, b_+21); goto nothing; }
    CYC(b_+19, b_+21);
  } else if (entry == b_+28) {
    goto nothing;
  } else if (entry == b_+30) {
    goto ret;
  }
treasure:
  CYC(b_+21, b_+23); C = 0xae;
  CYC(b_+23, b_+24); A = D;
  CYC(b_+24, b_+26); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+26, b_+28); goto ret; }
  CYC(b_+26, b_+28);
nothing:
  CYC(b_+28, b_+30); C = 0;
ret:
  CYC(b_+30, b_+31); A = C;
  CYC(b_+31, b_+32); alu_or(gb, A);
  CYC(b_+32, b_+33); ret_effect(gb);
}

void dungeonMap_checkCompassTile_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, SYM(dungeonMap_checkCompassTile), sp0_); }
void dungeonMap_checkCompassTile__treasure68e3_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, SYM(dungeonMap_checkCompassTile____treasure), sp0_); }
void dungeonMap_checkCompassTile__nothing68ea_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, SYM(dungeonMap_checkCompassTile____nothing), sp0_); }
void dungeonMap_checkCompassTile__ret68ec_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, SYM(dungeonMap_checkCompassTile____ret), sp0_); }

void dungeonMap_getFloorAddress_hook(GB *gb) {
  BASE(dungeonMap_getFloorAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, multiplyABy16_hook, SYM(multiplyABy16), b_+3);
  CYC(b_+3, b_+6); SET_HL(w2DungeonLayout);
  CYC(b_+6, b_+7); alu_add_hl(gb, BC);
  CYC(b_+7, b_+8); alu_add_hl(gb, BC);
  CYC(b_+8, b_+9); alu_add_hl(gb, BC);
  CYC(b_+9, b_+10); alu_add_hl(gb, BC);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void fileSelectMode6_hook(GB *gb) {
  BASE(fileSelectMode6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode6__updateMode6_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); TAIL(drawSecretInputCursors);
}

void fileSelectMode6__updateMode6_hook(GB *gb) {
  BASE(fileSelectMode6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); A = W8(wFileSelect_mode2);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+20) { fileSelectMode6__mode0_hook(gb); return; }
    else if (jt_ == b_+27) { fileSelectMode6__mode2_hook(gb); return; }
    else if (jt_ == SYM(textInput_waitForInput) && hook_enabled_at(gb, SYM(textInput_waitForInput))) { textInput_waitForInput_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode6__mode0_hook(GB *gb) {
  BASE(fileSelectMode6);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+24); W8(wSecretInputType) = A;
  CYC(b_+24, b_+27); TAIL(func_02_465c);
}

void fileSelectMode6__mode2_hook(GB *gb) {
  BASE(fileSelectMode6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+27, b_+30); SET_HL(w4SecretBuffer);
  CYC(b_+30, b_+33); SET_DE(wTmpcec0);
  CYC(b_+33, b_+35); B = 0x20;
  CALL_C(b_+35, copyMemory_hook, SYM(copyMemory), b_+38);
  CYC(b_+38, b_+41); SET_BC(0x0100);
  CALL_C(b_+41, secretFunctionCaller_hook, SYM(secretFunctionCaller), b_+44);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+47); fileSelect_printError_hook(gb);
    return;
  }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+50); A = mem_rd(gb, wTmpcec0 + 0x12);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+51, b_+53);
  } else {
    CYC(b_+51, b_+53);
    CYC(b_+53, b_+56); A = mem_rd(gb, wTmpcec0 + 5);
    CYC(b_+56, b_+57); A = alu_dec8(gb, A);
    if (F & FZ) {
      CYCT(b_+57, b_+60); fileSelect_printError_hook(gb);
      return;
    }
    CYC(b_+57, b_+60);
  }
  CALL_C(b_+60, loadFile_b00_hook, SYM(loadFile_b00), b_+63);
  CYC(b_+63, b_+66); SET_BC(0x0400);
  CALL_C(b_+66, secretFunctionCaller_hook, SYM(secretFunctionCaller), b_+69);
  CALL_C(b_+69, initializeFile_b00_hook, SYM(initializeFile_b00), b_+72);
  CYC(b_+72, b_+75); HANDOFF(SYM(setFileSelectModeTo1));
}

void b2_updateMenus_hook(GB *gb) {
  BASE(b2_updateMenus);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wOpenedMenuType);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); b2_updateMenus__updateMenu_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wScrollMode);
  CYC(b_+9, b_+11); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  if (W8(wTextIsActive)) {
    CALL_C(b_+12, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+15);
    return;
  }
  CALL_C(b_+12, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+15);
  CYC(b_+15, b_+18); A = W8(wLinkDeathTrigger);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+22); A = W8(wLinkPlayingInstrument);
  CYC(b_+22, b_+23); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+27); A = W8(wKeysJustPressed);
  CYC(b_+27, b_+29); alu_and(gb, 0x0c);
  if (F & FZ) { CYCT(b_+29, b_+31); goto no_button; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wGlobalFlags + 1);
  CYC(b_+34, b_+36); alu_bit(gb, 2, A);
  CYC(b_+36, b_+38); A = 0x5a;
  if (F & FZ) { CYCT(b_+38, b_+41); playSound_b00_hook(gb); return; }
  CYC(b_+38, b_+41);
no_button:
  CYC(b_+41, b_+44); A = W8(wMenuDisabled);
  CYC(b_+44, b_+45); B = A;
  CYC(b_+45, b_+48); A = W8(wDisableLinkCollisionsAndMenu);
  CYC(b_+48, b_+49); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+49, b_+50); ret_effect(gb); return; }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, playHeartBeepAtInterval_hook, SYM(playHeartBeepAtInterval), b_+53);
  CYC(b_+53, b_+56); A = W8(wKeysJustPressed);
  CYC(b_+56, b_+58); alu_and(gb, 0x0c);
  if (F & FZ) { CYCT(b_+58, b_+59); ret_effect(gb); return; }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+61); C = 3;
  CYC(b_+61, b_+63); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+63, b_+65); goto open_menu; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); C = alu_dec8(gb, C);
  CYC(b_+66, b_+68); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto open_menu; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); C = alu_dec8(gb, C);
open_menu:
  CYC(b_+71, b_+74); TAIL(openMenu_body);
}

void b2_updateMenus__updateMenu_hook(GB *gb) {
  BASE(b2_updateMenus);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+74, b_+76); A = 0xff;
  CYC(b_+76, b_+79); mem_wr(gb, wc4b6, A);
  CYC(b_+79, b_+82); A = W8(wMenuLoadState);
  CYC(b_+82, b_+83); push_effect(gb, b_+83);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(menuStateFadeIntoMenu) && hook_enabled_at(gb, SYM(menuStateFadeIntoMenu))) { menuStateFadeIntoMenu_hook(gb); return; }
    else if (jt_ == SYM(menuSpecificCode) && hook_enabled_at(gb, SYM(menuSpecificCode))) { menuSpecificCode_hook(gb); return; }
    else if (jt_ == SYM(menuStateFadeOutOfMenu) && hook_enabled_at(gb, SYM(menuStateFadeOutOfMenu))) { menuStateFadeOutOfMenu_hook(gb); return; }
    else if (jt_ == SYM(menuStateFadeIntoGame) && hook_enabled_at(gb, SYM(menuStateFadeIntoGame))) { menuStateFadeIntoGame_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void menuSpecificCode_hook(GB *gb) {
  BASE(menuSpecificCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wOpenedMenuType);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(runSaveAndQuitMenu) || jt_ == SYM(runSecretListMenu)) { HANDOFF(HL); }
    else if (jt_ == SYM(runInventoryMenu) && hook_enabled_at(gb, SYM(runInventoryMenu))) { runInventoryMenu_hook(gb); return; }
    else if (jt_ == SYM(runMapMenu) && hook_enabled_at(gb, SYM(runMapMenu))) { runMapMenu_hook(gb); return; }
    else if (jt_ == SYM(runRingMenu) && hook_enabled_at(gb, SYM(runRingMenu))) { runRingMenu_hook(gb); return; }
    else if (jt_ == SYM(runGaleSeedMenu) && hook_enabled_at(gb, SYM(runGaleSeedMenu))) { runGaleSeedMenu_hook(gb); return; }
    else if (jt_ == SYM(runSecretEntryMenu) && hook_enabled_at(gb, SYM(runSecretEntryMenu))) { runSecretEntryMenu_hook(gb); return; }
    else if (jt_ == SYM(runKidNameEntryMenu) && hook_enabled_at(gb, SYM(runKidNameEntryMenu))) { runKidNameEntryMenu_hook(gb); return; }
    else if (jt_ == SYM(runGameLinkMenu) && hook_enabled_at(gb, SYM(runGameLinkMenu))) { runGameLinkMenu_hook(gb); return; }
    else if (jt_ == SYM(runFakeReset) && hook_enabled_at(gb, SYM(runFakeReset))) { runFakeReset_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void menuStateFadeIntoMenu_hook(GB *gb) {
  BASE(menuStateFadeIntoMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wOpenedMenuType);
  CYC(b_+3, b_+5); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto fade_done; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wKeysPressed);
  CYC(b_+10, b_+12); alu_and(gb, 0x0c);
  CYC(b_+12, b_+14); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto fade_done; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = 3;
  CYC(b_+18, b_+21); W8(wOpenedMenuType) = A;
fade_done:
  CYC(b_+21, b_+24); A = W8(wPaletteThread_mode);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+25, b_+26); ret_effect(gb); return; }
  CYC(b_+25, b_+26);
  CALL_C(b_+26, menuStateFadeIntoMenu__openMenu_hook, b_+36, b_+29);
  CYC(b_+29, b_+32); SET_HL(wMenuLoadState);
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+33, b_+36); TAIL(menuSpecificCode);
}

void menuStateFadeIntoMenu__openMenu_hook(GB *gb) {
  BASE(menuStateFadeIntoMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+36, b_+39); A = W8(wOpenedMenuType);
  CYC(b_+39, b_+41); alu_cp(gb, 3);
  CYC(b_+41, b_+43); A = 0x54;
  if (!(F & FZ)) CALL_C_CC(b_+43, playSound_b00_hook, SYM(playSound_b00), b_+46);
  else CYC(b_+43, b_+46);
  CYC(b_+46, b_+48); A = 2;
  CALL_C(b_+48, setMusicVolume_hook, SYM(setMusicVolume), SYM(saveGraphicsOnEnterMenu_body));
  TAIL(saveGraphicsOnEnterMenu_body);
}

void saveGraphicsOnEnterMenu_body_hook(GB *gb) {
  BASE(saveGraphicsOnEnterMenu_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = H8(hCameraY);
  CYC(b_+2, b_+5); SET_HL(wcbe1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+8); A = H8(hCameraX);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10); push_effect(gb, DE);
  CYC(b_+10, b_+13); SET_HL(wGfxRegs1);
  CYC(b_+13, b_+16); SET_DE(wGfxRegs4);
  CYC(b_+16, b_+18); B = 0x0c;
  CALL_C(b_+18, copyMemory_hook, SYM(copyMemory), b_+21);
  CALL_C(b_+21, disableLcd_hook, SYM(disableLcd), b_+24);
  CALL_C(b_+24, copyW2TilesetBgPalettesToW4PaletteData_body_hook, SYM(copyW2TilesetBgPalettesToW4PaletteData_body), b_+27);
  CYC(b_+27, b_+29); A = 4;
  CYC(b_+29, b_+31); mem_wr(gb, IO_SVBK, A);
  CYC(b_+31, b_+34); SET_HL(wOam);
  CYC(b_+34, b_+37); SET_DE(w4SavedOam);
  CYC(b_+37, b_+39); B = 0xa0;
  CALL_C(b_+39, copyMemory_hook, SYM(copyMemory), b_+42);
  CYC(b_+42, b_+44); A = 1;
  CYC(b_+44, b_+46); mem_wr(gb, IO_VBK, A);
  CYC(b_+46, b_+49); SET_HL(0x8600);
  CYC(b_+49, b_+52); SET_BC(0x0180);
  CYC(b_+52, b_+55); SET_DE(w4SavedVramTiles);
  CALL_C(b_+55, copyMemoryBc_hook, SYM(copyMemoryBc), b_+58);
  CYC(b_+58, b_+61); SET_HL(wMenuUnionStart);
  CYC(b_+61, b_+63); B = 0x10;
  CALL_C(b_+63, clearMemory_hook, SYM(clearMemory), b_+66);
  CYC(b_+66, b_+68); A = 0xff;
  CYC(b_+68, b_+71); mem_wr(gb, wc4b6, A);
  CYC(b_+71, b_+72); SET_DE(pop_effect(gb));
  CYC(b_+72, b_+75); TAIL(clearOam);
}

void menuStateFadeIntoGame_hook(GB *gb) {
  BASE(menuStateFadeIntoGame);
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wc4b6, A);
  CYC(b_+9, b_+12); W8(wOpenedMenuType) = A;
  CYC(b_+12, b_+14); A = 3;
  CYC(b_+14, b_+17); TAIL(setMusicVolume);
}

void copyTextCharactersFromSecretTextTable_hook(GB *gb) {
  BASE(copyTextCharactersFromSecretTextTable);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(secretTextTable));
  CYC(b_+3, b_+4); add_double_index_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  TAIL(copyTextCharactersFromHlUntilNull);
}

void copyTextCharactersFromHlUntilNull_hook(GB *gb) {
  BASE(copyTextCharactersFromHlUntilNull);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B |= 0x80;
  TAIL(copyTextCharactersFromHl);
}

void copyTextCharactersFromHl_hook(GB *gb) {
  BASE(copyTextCharactersFromHl);
  uint16_t sp0_ = gb->sp; (void)sp0_;
copy_next:
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+3); alu_bit(gb, 7, B);
  if (F & FZ) { CYCT(b_+3, b_+5); goto glyph; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 1);
  if (F & FZ) { CYCT(b_+9, b_+11); goto copy_next; }
  CYC(b_+9, b_+11);
glyph:
  CYC(b_+11, b_+13); C = 0;
  CYC(b_+13, b_+15); alu_cp(gb, 6);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto draw; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); C = alu_inc8(gb, C);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
draw:
  CALL_C(b_+19, copyTextCharacterGfx_hook, SYM(copyTextCharacterGfx), b_+22);
  CYC(b_+22, b_+24); alu_bit(gb, 7, B);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto copy_next; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto copy_next; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); ret_effect(gb);
}

void b2_fileSelectScreen_hook(GB *gb) {
  BASE(b2_fileSelectScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wFileSelect_cbb6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+4, fileSelect_redrawDecorationsAndSetWramBank4_hook, SYM(fileSelect_redrawDecorationsAndSetWramBank4), b_+7);
  CYC(b_+7, b_+10); A = W8(wFileSelect_mode);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(fileSelectMode0) && hook_enabled_at(gb, SYM(fileSelectMode0))) { fileSelectMode0_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode1) && hook_enabled_at(gb, SYM(fileSelectMode1))) { fileSelectMode1_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode2) && hook_enabled_at(gb, SYM(fileSelectMode2))) { fileSelectMode2_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode3) && hook_enabled_at(gb, SYM(fileSelectMode3))) { fileSelectMode3_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode4) && hook_enabled_at(gb, SYM(fileSelectMode4))) { fileSelectMode4_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode5) && hook_enabled_at(gb, SYM(fileSelectMode5))) { fileSelectMode5_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode6) && hook_enabled_at(gb, SYM(fileSelectMode6))) { fileSelectMode6_hook(gb); return; }
    else if (jt_ == SYM(fileSelectMode7) && hook_enabled_at(gb, SYM(fileSelectMode7))) { fileSelectMode7_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void fileSelectMode0_hook(GB *gb) {
  BASE(fileSelectMode0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wFileSelect_mode);
  CYC(b_+3, b_+5); B = 0x10;
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CALL_C(b_+8, disableLcd_hook, SYM(disableLcd), b_+11);
  CYC(b_+11, b_+13); A = 0xa0;
  CALL_C(b_+13, loadGfxHeader_hook, SYM(loadGfxHeader), b_+16);
  CYC(b_+16, b_+18); A = 0x11;
  CALL_C(b_+18, playSound_b00_hook, SYM(playSound_b00), b_+21);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+25); W8(wLastSecretInputLength) = A;
  CALL_C(b_+25, setFileSelectModeTo1_hook, SYM(setFileSelectModeTo1), SYM(fileSelectMode1));
  TAIL(fileSelectMode1);
}

void fileSelectMode1_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fileSelectMode1__subModes_hook, b_+9, b_+3);
  fileSelectMode1__afterCall_hook(gb);
}

void fileSelectMode1__afterCall_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+3, fileSelectDrawAcornCursor_hook, SYM(fileSelectDrawAcornCursor), b_+6);
  CYC(b_+6, b_+9); TAIL(fileSelectDrawLink);
}

void fileSelectMode1__subModes_hook(GB *gb) {
  BASE(fileSelectMode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); A = W8(wFileSelect_mode2);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == b_+21) { fileSelectMode1__state0_hook(gb); return; }
    else if (jt_ == b_+53) { fileSelectMode1__state1_hook(gb); return; }
    else if (jt_ == b_+120) { fileSelectMode1__state2_hook(gb); return; }
    else if (jt_ == b_+204) {
      CYC(b_+204, b_+207); A = W8(wPaletteThread_mode);
      CYC(b_+207, b_+208); alu_or(gb, A);
      if (!(F & FZ)) { CYCT(b_+208, b_+210); fileSelectMode1__textSpeedMenu_addCursorToOam_hook(gb); return; }
      CYC(b_+208, b_+210);
      CYC(b_+210, b_+211); alu_xor(gb, A);
      CYC(b_+211, b_+214); W8(wLastSecretInputLength) = A;
      CYC(b_+214, b_+217); SET_BC(SYM(mainThreadStart));
      CYC(b_+217, b_+220); HANDOFF(SYM(restartThisThread));
    }
    else { HANDOFF(HL); }
  } while (0);
}

void runBank2Function_hook(GB *gb) {
  BASE(runBank2Function);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = L;
  CYC(b_+1, b_+2); A = H;
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (function_caller_jump_table(gb));
    if (jt_ == SYM(loadCommonGraphics_body) && hook_enabled_at(gb, SYM(loadCommonGraphics_body))) { loadCommonGraphics_body_hook(gb); return; }
    else if (jt_ == SYM(updateStatusBar_body) && hook_enabled_at(gb, SYM(updateStatusBar_body))) { updateStatusBar_body_hook(gb); return; }
    else if (jt_ == SYM(hideStatusBar_body) && hook_enabled_at(gb, SYM(hideStatusBar_body))) { hideStatusBar_body_hook(gb); return; }
    else if (jt_ == SYM(showStatusBar_body) && hook_enabled_at(gb, SYM(showStatusBar_body))) { showStatusBar_body_hook(gb); return; }
    else if (jt_ == SYM(saveGraphicsOnEnterMenu_body) && hook_enabled_at(gb, SYM(saveGraphicsOnEnterMenu_body))) { saveGraphicsOnEnterMenu_body_hook(gb); return; }
    else if (jt_ == SYM(reloadGraphicsOnExitMenu_body) && hook_enabled_at(gb, SYM(reloadGraphicsOnExitMenu_body))) { reloadGraphicsOnExitMenu_body_hook(gb); return; }
    else if (jt_ == SYM(openMenu_body) && hook_enabled_at(gb, SYM(openMenu_body))) { openMenu_body_hook(gb); return; }
    else if (jt_ == SYM(copyW2TilesetBgPalettesToW4PaletteData_body) && hook_enabled_at(gb, SYM(copyW2TilesetBgPalettesToW4PaletteData_body))) { copyW2TilesetBgPalettesToW4PaletteData_body_hook(gb); return; }
    else if (jt_ == SYM(copyW4PaletteDataToW2TilesetBgPalettes_body) && hook_enabled_at(gb, SYM(copyW4PaletteDataToW2TilesetBgPalettes_body))) { copyW4PaletteDataToW2TilesetBgPalettes_body_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void menuStateFadeOutOfMenu_hook(GB *gb) {
  BASE(menuStateFadeOutOfMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, reloadGraphicsOnExitMenu_body_hook, SYM(reloadGraphicsOnExitMenu_body), b_+8);
  menuStateFadeOutOfMenu__afterCall_hook(gb);
}

void menuStateFadeOutOfMenu__afterCall_hook(GB *gb) {
  BASE(menuStateFadeOutOfMenu);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+8, b_+11); SET_HL(wMenuLoadState);
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+15); TAIL(updateParentItemButtonAssignment);
}

void reloadGraphicsOnExitMenu_body_hook(GB *gb) {
  BASE(reloadGraphicsOnExitMenu_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wcbe1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); H8(hCameraY) = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); H8(hCameraX) = A;
  CYC(b_+9, b_+10); push_effect(gb, DE);
  CALL_C(b_+10, disableLcd_hook, SYM(disableLcd), b_+13);
  CYC(b_+13, b_+15); A = 0x04;
  CYC(b_+15, b_+17); mem_wr(gb, IO_SVBK, A);
  CYC(b_+17, b_+20); SET_DE(0x8601);
  CYC(b_+20, b_+23); SET_BC(0x1704);
  CYC(b_+23, b_+26); SET_HL(w4SavedVramTiles);
  CALL_C(b_+26, queueDmaTransfer_hook, SYM(queueDmaTransfer), b_+29);
  CYC(b_+29, b_+32); SET_HL(w4SavedOam);
  CYC(b_+32, b_+35); SET_DE(wOam);
  CYC(b_+35, b_+37); B = 0xa0;
  CALL_C(b_+37, copyMemory_hook, SYM(copyMemory), b_+40);
  CALL_C(b_+40, copyW4PaletteDataToW2TilesetBgPalettes_body_hook, SYM(copyW4PaletteDataToW2TilesetBgPalettes_body), b_+43);
  CYC(b_+43, b_+46); SET_HL(wGfxRegs4);
  CYC(b_+46, b_+49); SET_DE(wGfxRegs1);
  CYC(b_+49, b_+51); B = 0x0c;
  CALL_C(b_+51, copyMemory_hook, SYM(copyMemory), b_+54);
  CALL_C(b_+54, loadCommonGraphics_body_hook, SYM(loadCommonGraphics_body), b_+57);
  CALL_C(b_+57, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+60);
  reloadGraphicsOnExitMenu_body__afterCall_hook(gb);
}

void reloadGraphicsOnExitMenu_body__afterCall_hook(GB *gb) {
  BASE(reloadGraphicsOnExitMenu_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+60, loadTilesetData_hook, SYM(loadTilesetData), b_+63);
  CALL_C(b_+63, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+66);
  CALL_C(b_+66, reloadTileMap_hook, SYM(reloadTileMap), b_+69);
  CALL_C(b_+69, fastFadeinFromWhiteToRoom_hook, SYM(fastFadeinFromWhiteToRoom), b_+72);
  CYC(b_+72, b_+75); A = W8(wExtraBgPaletteHeader);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+76, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+79);
  else CYC(b_+76, b_+79);
  CYC(b_+79, b_+82); A = W8(wGfxRegs1_LCDC);
  CYC(b_+82, b_+85); W8(wGfxRegsFinal_LCDC) = A;
  CYC(b_+85, b_+87); mem_wr(gb, IO_LCDC, A);
  CYC(b_+87, b_+88); SET_DE(pop_effect(gb));
  CYC(b_+88, b_+91); SET_HL((SYM(mapMenu_loadPopupData__gotIcon) + 25));
  CYC(b_+91, b_+93); E = 0x01;
  CYC(b_+93, b_+96); interBankCall_hook(gb);
}

void saveQuitMenu_state2_hook(GB *gb) {
  BASE(saveQuitMenu_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wSaveQuitMenu_delayCounter);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wSaveQuitMenu_cursorIndex);
  CYC(b_+8, b_+10); alu_cp(gb, 2);
  if (F & FZ) { CYCT(b_+10, b_+13); HANDOFF(SYM(resetGame)); }
  CYC(b_+10, b_+13);
  CALL_C(b_+13, saveQuitMenu_checkIsGameOver_hook, SYM(saveQuitMenu_checkIsGameOver), b_+16);
  if (F & FZ) { CYCT(b_+16, b_+19); closeMenu_hook(gb); return; }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); A = 0xe8;
  CYC(b_+21, b_+24); SET_BC(SYM(mainThreadStart));
  CALL_C(b_+24, threadRestart_hook, SYM(threadRestart), b_+27);
  CYC(b_+27, b_+30); HANDOFF(SYM(stubThreadStart));
}

void fileSelect_printError_hook(GB *gb) {
  BASE(fileSelect_printError);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x5a;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+7); A = 0x10;
  CYC(b_+7, b_+10); W8(wFileSelect_linkTimer) = A;
  CYC(b_+10, b_+12); A = 0x04;
  CYC(b_+12, b_+15); W8(wFileSelect_mode2) = A;
  CYC(b_+15, b_+17); A = 0xad;
  CALL_C(b_+17, loadGfxHeader_hook, SYM(loadGfxHeader), b_+20);
  CYC(b_+20, b_+22); A = 0x08;
  CYC(b_+22, b_+25); TAIL(loadUncompressedGfxHeader);
}

void textInput_waitForInput_hook(GB *gb) {
  BASE(textInput_waitForInput);
  CYC(b_+0, b_+3); SET_HL(wFileSelect_linkTimer);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+12); A = W8(wKeysPressed);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+13, b_+14); ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+19); W8(wFileSelect_mode2) = A;
  TAIL(func_02_461c);
}

void checkDisplayDmgModeScreen_hook(GB *gb) {
  BASE(checkDisplayDmgModeScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = H8(hGameboyType);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, disableLcd_hook, SYM(disableLcd), b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CALL_C(b_+8, loadGfxHeader_hook, SYM(loadGfxHeader), b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CALL_C(b_+12, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+15);
  CYC(b_+15, b_+18); SET_HL(wGfxRegs1);
  CYC(b_+18, b_+21); SET_DE(wGfxRegsFinal);
  CYC(b_+21, b_+23); B = 0x06;
  CALL_C(b_+23, copyMemory_hook, SYM(copyMemory), b_+26);
  TAIL(checkDisplayDmgModeScreen__vblankLoop);
}

void checkDisplayDmgModeScreen__vblankLoop_hook(GB *gb) {
  BASE(checkDisplayDmgModeScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+26, b_+28); A = 0xff;
    CYC(b_+28, b_+31); W8(wVBlankChecker) = A;
    int halt_r;
    do {
      CYC(b_+31, b_+32); halt_r = hook_halt(gb, b_+32);
    } while (halt_r == 1);
    if (halt_r < 0) {
      hook_handoff(gb, b_+32);
      return;
    }
    CYC(b_+32, b_+33);
    CYC(b_+33, b_+36); A = W8(wVBlankChecker);
    CYC(b_+36, b_+38); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(b_+38, b_+40);
      continue;
    }
    CYC(b_+38, b_+40);
    CYC(b_+40, b_+42); A = H8(hSerialInterruptBehaviour);
    CYC(b_+42, b_+43); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+43, b_+45);
      CALL_C(b_+50, serialFunc_0c85_hook, SYM(serialFunc_0c85), b_+53);
      CYC(b_+53, b_+55); A = 0x03;
      CYC(b_+55, b_+57); H8(hFFBE) = A;
      CYC(b_+57, b_+58); alu_xor(gb, A);
      CYC(b_+58, b_+60); H8(hSerialLinkState) = A;
      CYC(b_+60, b_+62);
      continue;
    }
    CYC(b_+43, b_+45);
    CALL_C(b_+45, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+48);
    CYC(b_+48, b_+50);
  }
}

void updateTilesetFlagsForIndoorRoomInAltWorld_hook(GB *gb) {
  BASE(updateTilesetFlagsForIndoorRoomInAltWorld);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(SYM(roomsInAltWorldTable));
  CYC(b_+8, b_+9); add_double_index_to_hl(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); H = mem_rd(gb, HL);
  CYC(b_+11, b_+12); L = A;
  CYC(b_+12, b_+15); A = W8(wActiveRoom);
  CALL_C(b_+15, checkFlag_hook, SYM(checkFlag), b_+18);
  if (F & FZ) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+22); SET_HL(wTilesetFlags);
  CYC(b_+22, b_+24); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+24, b_+25); ret_effect(gb);
}
