#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

void b2_updateMenus_hook(GB *gb);
void b2_updateMenus__updateMenu_hook(GB *gb);
void menuSpecificCode_hook(GB *gb);
void menuStateFadeIntoMenu_hook(GB *gb);
void menuStateFadeIntoMenu__openMenu_hook(GB *gb);
void saveGraphicsOnEnterMenu_body_hook(GB *gb);
void menuStateFadeIntoGame_hook(GB *gb);

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
  CYC(0x7856, 0x7857); A = mem_rd(gb, DE);
  CYC(0x7857, 0x7858); C = A;
  CYC(0x7858, 0x7859); A = mem_rd(gb, HL);
  CYC(0x7859, 0x785a); mem_wr(gb, HL, C);
  CYC(0x785a, 0x785b); mem_wr(gb, DE, A);
  CYC(0x785b, 0x785c); ret_effect(gb);
}

void functionCaller_b02_hook(GB *gb) {
  CYC(0x77b2, 0x77b3); C = L;
  CYC(0x77b3, 0x77b4); A = H;
  CYC(0x77b4, 0x77b5); push_effect(gb, 0x77b5);
  hook_handoff(gb, function_caller_jump_table(gb));
}

void addRoomToEnemiesKilledList_b02_hook(GB *gb) {
  CYC(0x77c3, 0x77c6); SET_HL(0xcdc0);
  CYC(0x77c6, 0x77c9); A = mem_rd(gb, 0xcc30);
  CYC(0x77c9, 0x77cb); B = 0x08;
  for (;;) {
    CYC(0x77cb, 0x77cc); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x77cc, 0x77ce); break; }
    CYC(0x77cc, 0x77ce);
    CYC(0x77ce, 0x77cf); L = alu_inc8(gb, L);
    CYC(0x77cf, 0x77d0); L = alu_inc8(gb, L);
    CYC(0x77d0, 0x77d1); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x77d1, 0x77d3); continue; }
    CYC(0x77d1, 0x77d3);
    CYC(0x77d3, 0x77d6); A = mem_rd(gb, 0xcdd0);
    CYC(0x77d6, 0x77d7); B = A;
    CYC(0x77d7, 0x77d8); A = alu_inc8(gb, A);
    CYC(0x77d8, 0x77d9); A = alu_inc8(gb, A);
    CYC(0x77d9, 0x77db); alu_and(gb, 0x0f);
    CYC(0x77db, 0x77de); mem_wr(gb, 0xcdd0, A);
    CYC(0x77de, 0x77df); A = B;
    CYC(0x77df, 0x77e1); alu_and(gb, 0x0f);
    CYC(0x77e1, 0x77e3); alu_add(gb, 0xc0);
    CYC(0x77e3, 0x77e4); L = A;
    CYC(0x77e4, 0x77e7); A = mem_rd(gb, 0xcc30);
    CYC(0x77e7, 0x77e8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x77e8, 0x77e9); alu_xor(gb, A);
    CYC(0x77e9, 0x77ea); mem_wr(gb, HL, A);
    CYC(0x77ea, 0x77ed); mem_wr(gb, 0xcec9, A);
    CYC(0x77ed, 0x77ee); ret_effect(gb);
    return;
  }
  CYC(0x77ee, 0x77ef); L = alu_inc8(gb, L);
  CYC(0x77ef, 0x77f0); A = mem_rd(gb, HL);
  CYC(0x77f0, 0x77f3); mem_wr(gb, 0xcec9, A);
  CYC(0x77f3, 0x77f4); ret_effect(gb);
}

void markEnemyAsKilledInRoom_b02_hook(GB *gb) {
  CYC(0x77f5, 0x77f8); SET_HL(0xcdc0);
  CYC(0x77f8, 0x77fa); B = 0x08;
  CYC(0x77fa, 0x77fd); A = mem_rd(gb, 0xcc30);
  for (;;) {
    CYC(0x77fd, 0x77fe); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x77fe, 0x7800); break; }
    CYC(0x77fe, 0x7800);
    CYC(0x7800, 0x7801); L = alu_inc8(gb, L);
    CYC(0x7801, 0x7802); L = alu_inc8(gb, L);
    CYC(0x7802, 0x7803); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x7803, 0x7805); continue; }
    CYC(0x7803, 0x7805);
    CYC(0x7805, 0x7806); ret_effect(gb);
    return;
  }
  CYC(0x7806, 0x7807); L = alu_inc8(gb, L);
  CYC(0x7807, 0x7809); E = 0x80;
  CYC(0x7809, 0x780a); A = mem_rd(gb, DE);
  CYC(0x780a, 0x780c); alu_and(gb, 0x70);
  CYC(0x780c, 0x780e); A = alu_swap(gb, A);
  CYC(0x780e, 0x7811); SET_BC(0x00f8);
  CYC(0x7811, 0x7812); alu_add(gb, C);
  CYC(0x7812, 0x7813); C = A;
  CYC(0x7813, 0x7814); A = mem_rd(gb, BC);
  CYC(0x7814, 0x7815); alu_or(gb, mem_rd(gb, HL));
  CYC(0x7815, 0x7816); mem_wr(gb, HL, A);
  CYC(0x7816, 0x7817); ret_effect(gb);
}

void clearEnemiesKilledList_b02_hook(GB *gb) {
  CYC(0x7817, 0x7818); alu_xor(gb, A);
  CYC(0x7818, 0x781b); mem_wr(gb, 0xcdd0, A);
  CYC(0x781b, 0x781e); SET_HL(0xcdc0);
  CYC(0x781e, 0x7820); B = 0x10;
  CYC(0x7820, 0x7823); clearMemory_hook(gb);
}

void generateRandomBuffer_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7823, 0x7824); push_effect(gb, DE);
  CYC(0x7824, 0x7826); A = 0x04;
  CYC(0x7826, 0x7828); mem_wr(gb, 0xff70, A);
  CYC(0x7828, 0x782b); SET_HL(0xd000);
  CYC(0x782b, 0x782d); B = 0;
  do {
    CYC(0x782d, 0x782e); A = B;
    CYC(0x782e, 0x782f); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x782f, 0x7830); B = alu_inc8(gb, B);
    if (!(F & FZ)) CYCT(0x7830, 0x7832); else CYC(0x7830, 0x7832);
  } while (!(F & FZ));
  CYC(0x7832, 0x7835); SET_HL(0xd0ff);
  CYC(0x7835, 0x7836); D = H;
  CALL_C(0x7836, getRandomNumber_hook, 0x043e, 0x7839);
  CYC(0x7839, 0x783a); E = A;
  CYC(0x783a, 0x783d); push_effect(gb, 0x783d); swap_de_hl_memory(gb);
  CYC(0x783d, 0x783f); B = 0xff;
  do {
    CALL_C(0x783f, getRandomNumber_hook, 0x043e, 0x7842);
    CYC(0x7842, 0x7843); C = L;
    CALL_C(0x7843, multiplyAByC_hook, 0x019d, 0x7846);
    CYC(0x7846, 0x7847); E = H;
    CYC(0x7847, 0x7848); L = C;
    CYC(0x7848, 0x784a); H = 0xd0;
    CYC(0x784a, 0x784d); push_effect(gb, 0x784d); swap_de_hl_memory(gb);
    CYC(0x784d, 0x784e); L = alu_dec8(gb, L);
    if (!(F & FZ)) CYCT(0x784e, 0x7850); else CYC(0x784e, 0x7850);
  } while (!(F & FZ));
  CYC(0x7850, 0x7852); A = 0x01;
  CYC(0x7852, 0x7854); mem_wr(gb, 0xff70, A);
  CYC(0x7854, 0x7855); SET_DE(pop_effect(gb));
  CYC(0x7855, 0x7856); ret_effect(gb);
}

void getRandomPositionForEnemy_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x785c, 0x785e); A = 0x40;
  CYC(0x785e, 0x7861); mem_wr(gb, 0xcecf, A);
  for (;;) {
    CYC(0x7861, 0x7864); SET_HL(0xcecf);
    CYC(0x7864, 0x7865); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) { CYCT(0x7865, 0x7867); CYC(0x7880, 0x7881); alu_scf(gb); CYC(0x7881, 0x7882); ret_effect(gb); return; }
    CYC(0x7865, 0x7867);
    CALL_C(0x7867, getCandidatePositionForEnemy_hook, 0x796b, 0x786a);
    CYC(0x786a, 0x786d); mem_wr(gb, 0xcec2, A);
    CYC(0x786d, 0x786e); C = A;
    CALL_C(0x786e, checkPositionValidForEnemySpawn_hook, 0x7894, 0x7871);
    if (F & FC) { CYCT(0x7871, 0x7873); continue; }
    CYC(0x7871, 0x7873);
    CYC(0x7873, 0x7875); A = mem_rd(gb, 0xff8b);
    CYC(0x7875, 0x7877); alu_and(gb, 0x04);
    if (F & FZ) {
      CYC(0x7877, 0x7879);
      CALL_C(0x7879, checkTileValidForEnemySpawn_hook, 0x7882, 0x787c);
      if (F & FC) { CYCT(0x787c, 0x787e); continue; }
      CYC(0x787c, 0x787e);
    } else CYCT(0x7877, 0x7879);
    CYC(0x787e, 0x787f); alu_xor(gb, A);
    CYC(0x787f, 0x7880); ret_effect(gb);
    return;
  }
}

void checkSpawnTimeportalInteraction_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79be, 0x79bf); alu_xor(gb, A);
  CYC(0x79bf, 0x79c2); mem_wr(gb, 0xcddd, A);
  CYC(0x79c2, 0x79c5); SET_HL(0xc63e);
  CYC(0x79c5, 0x79c8); A = mem_rd(gb, 0xcc2d);
  CYC(0x79c8, 0x79c9); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79c9, 0x79ca); ret_effect(gb); return; }
  CYC(0x79c9, 0x79ca);
  CYC(0x79ca, 0x79cb); L = alu_inc8(gb, L);
  CYC(0x79cb, 0x79ce); A = mem_rd(gb, 0xcc30);
  CYC(0x79ce, 0x79cf); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79cf, 0x79d0); ret_effect(gb); return; }
  CYC(0x79cf, 0x79d0);
  CYC(0x79d0, 0x79d1); L = alu_inc8(gb, L);
  CYC(0x79d1, 0x79d2); C = mem_rd(gb, HL);
  CALL_C(0x79d2, getFreeInteractionSlot_hook, 0x3aef, 0x79d5);
  if (!(F & FZ)) { CYCT(0x79d5, 0x79d6); ret_effect(gb); return; }
  CYC(0x79d5, 0x79d6);
  CYC(0x79d6, 0x79d8); mem_wr(gb, HL, 0xde);
  CYC(0x79d8, 0x79da); A = 0x01;
  CYC(0x79da, 0x79dd); mem_wr(gb, 0xcddd, A);
  CYC(0x79dd, 0x79df); L = 0x4b;
  CYC(0x79df, 0x79e2); setShortPosition_paramC_hook(gb);
}

void loadRememberedCompanion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x768a, 0x768d); SET_HL(wRememberedCompanionId);
  CYC(0x768d, 0x768e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x768e, 0x768f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x768f, 0x7690); ret_effect(gb); return; }
  CYC(0x768f, 0x7690);
  CYC(0x7690, 0x7691); C = A;
  CYC(0x7691, 0x7694); A = W8(wActiveGroup);
  CYC(0x7694, 0x7695); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7695, 0x7696); ret_effect(gb); return; }
  CYC(0x7695, 0x7696);
  CYC(0x7696, 0x7697); L = alu_inc8(gb, L);
  CYC(0x7697, 0x769a); A = W8(wActiveRoom);
  CYC(0x769a, 0x769b); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x769b, 0x769c); ret_effect(gb); return; }
  CYC(0x769b, 0x769c);
  CYC(0x769c, 0x769f); A = W8(w1Companion_enabled);
  CYC(0x769f, 0x76a0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76a0, 0x76a1); ret_effect(gb); return; }
  CYC(0x76a0, 0x76a1);
  CYC(0x76a1, 0x76a2); A = C;
  CYC(0x76a2, 0x76a4); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(0x76a4, 0x76a6); goto raft; }
  CYC(0x76a4, 0x76a6);
  CYC(0x76a6, 0x76a9); W8(w1Companion_id) = A;
  CYC(0x76a9, 0x76ab); A = 1;
  CYC(0x76ab, 0x76ae); W8(w1Companion_enabled) = A;
  CYC(0x76ae, 0x76af); L = alu_inc8(gb, L);
  CYC(0x76af, 0x76b0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76b0, 0x76b3); W8(w1Companion_yh) = A;
  CYC(0x76b3, 0x76b4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76b4, 0x76b7); W8(w1Companion_xh) = A;
  CYC(0x76b7, 0x76b8); ret_effect(gb);
  return;
raft:
  CYC(0x76b8, 0x76bb); A = W8(wTilesetFlags);
  CYC(0x76bb, 0x76bd); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x76bd, 0x76be); ret_effect(gb); return; }
  CYC(0x76bd, 0x76be);
  CALL_C(0x76be, getFreeInteractionSlot_hook, ROM_getFreeInteractionSlot, 0x76c1);
  if (!(F & FZ)) { CYCT(0x76c1, 0x76c2); ret_effect(gb); return; }
  CYC(0x76c1, 0x76c2);
  CYC(0x76c2, 0x76c4); mem_wr(gb, HL, 0xe6);
  CYC(0x76c4, 0x76c5); L = alu_inc8(gb, L);
  CYC(0x76c5, 0x76c7); mem_wr(gb, HL, 2);
  CYC(0x76c7, 0x76ca); A = W8(wRememberedCompanionY);
  CYC(0x76ca, 0x76cc); L = 0x4b;
  CYC(0x76cc, 0x76cd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x76cd, 0x76ce); L = alu_inc8(gb, L);
  CYC(0x76ce, 0x76d1); A = W8(wRememberedCompanionX);
  CYC(0x76d1, 0x76d2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x76d2, 0x76d3); ret_effect(gb);
}

void checkAndSpawnMaple_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x76d3, 0x76d4); alu_xor(gb, A);
  CYC(0x76d4, 0x76d7); W8(wIsMaplePresent) = A;
  CYC(0x76d7, 0x76da); A = W8(wcc85);
  CYC(0x76da, 0x76db); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76db, 0x76dc); ret_effect(gb); return; }
  CYC(0x76db, 0x76dc);
  CYC(0x76dc, 0x76df); A = W8(wActiveGroup);
  CYC(0x76df, 0x76e2); SET_HL(0x7792);
  CYC(0x76e2, 0x76e3); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x76e3, 0x76e5); goto start_check; }
  CYC(0x76e3, 0x76e5);
  CYC(0x76e5, 0x76e6); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x76e6, 0x76e7); ret_effect(gb); return; }
  CYC(0x76e6, 0x76e7);
  CYC(0x76e7, 0x76ea); A = W8(w1Companion_enabled);
  CYC(0x76ea, 0x76eb); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76eb, 0x76ec); ret_effect(gb); return; }
  CYC(0x76eb, 0x76ec);
  CYC(0x76ec, 0x76ef); A = W8(wAnimalCompanion);
  CYC(0x76ef, 0x76f0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x76f0, 0x76f2); goto maple_table; }
  CYC(0x76f0, 0x76f2);
  CYC(0x76f2, 0x76f4); alu_sub(gb, 0x0b);
maple_table:
  CYC(0x76f4, 0x76f7); SET_HL(0x772c);
  CYC(0x76f7, 0x76f8); add_double_index_to_hl(gb, 0x76f8);
  CYC(0x76f8, 0x76f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76f9, 0x76fa); H = mem_rd(gb, HL);
  CYC(0x76fa, 0x76fb); L = A;
start_check:
  CYC(0x76fb, 0x76fe); A = W8(wActiveRoom);
  CALL_C(0x76fe, checkFlag_hook, 0x0205, 0x7701);
  if (!(F & FZ)) { CYCT(0x7701, 0x7702); ret_effect(gb); return; }
  CYC(0x7701, 0x7702);
  CYC(0x7702, 0x7704); A = 0x0f;
  CALL_C(0x7704, cpActiveRing_hook, 0x23b0, 0x7707);
  CYC(0x7707, 0x7709); E = 0x1e;
  if (!(F & FZ)) CYCT(0x7709, 0x770b);
  else { CYC(0x7709, 0x770b); CYC(0x770b, 0x770d); E = alu_srl(gb, E); }
  CYC(0x770d, 0x7710); SET_HL(wMapleKillCounter);
  CYC(0x7710, 0x7711); A = mem_rd(gb, HL);
  CYC(0x7711, 0x7712); alu_cp(gb, E);
  if (F & FC) { CYCT(0x7712, 0x7713); ret_effect(gb); return; }
  CYC(0x7712, 0x7713); CYC(0x7713, 0x7715); mem_wr(gb, HL, 0);
  CYC(0x7715, 0x7718); SET_HL(w1Companion);
  CYC(0x7718, 0x771a); A = 1;
  CYC(0x771a, 0x771d); W8(wcc85) = A;
  CYC(0x771d, 0x7720); W8(wIsMaplePresent) = A;
  CYC(0x7720, 0x7721); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7721, 0x7723); mem_wr(gb, HL, 0x0e);
  CYC(0x7723, 0x7725); L = 0x0b;
  CYC(0x7725, 0x7727); mem_wr(gb, HL, 0x18);
  CYC(0x7727, 0x7729); L = 0x0d;
  CYC(0x7729, 0x772b); mem_wr(gb, HL, 0xb8);
  CYC(0x772b, 0x772c); ret_effect(gb);
}

void stub_02_77f4_hook(GB *gb) {
  CYC(0x77f4, 0x77f5); ret_effect(gb);
}

static void get_absolute_value(GB *gb) {
  CYC(0x7919, 0x791b); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x791b, 0x791c); ret_effect(gb); return; }
  CYC(0x791b, 0x791c); CYC(0x791c, 0x791d); alu_cpl(gb);
  CYC(0x791d, 0x791e); A = alu_inc8(gb, A);
  CYC(0x791e, 0x791f); ret_effect(gb);
}

void checkTileValidForEnemySpawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7882, 0x7884); B = 0xce;
  CYC(0x7884, 0x7885); A = mem_rd(gb, BC);
  CYC(0x7885, 0x7886); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7886, 0x7888); goto invalid; }
  CYC(0x7886, 0x7888);
  CYC(0x7888, 0x788a); B = 0xcf;
  CYC(0x788a, 0x788b); A = mem_rd(gb, BC);
  CYC(0x788b, 0x788e); SET_HL(0x791f);
  CALL_C(0x788e, lookupCollisionTable_hook, 0x1e1f, 0x7891);
  if (!(F & FC)) { CYCT(0x7891, 0x7892); ret_effect(gb); return; }
  CYC(0x7891, 0x7892);
invalid:
  CYC(0x7892, 0x7893); alu_scf(gb);
  CYC(0x7893, 0x7894); ret_effect(gb);
}

static void check_enemy_position_bounds(GB *gb) {
  CYC(0x789b, 0x789e); A = W8(wActiveGroup);
  CYC(0x789e, 0x78a0); alu_and(gb, 4);
  CYC(0x78a0, 0x78a3); SET_HL(0x78c8);
  if (F & FZ) CYCT(0x78a3, 0x78a5);
  else { CYC(0x78a3, 0x78a5); CYC(0x78a5, 0x78a8); SET_HL(0x78d8); }
  CYC(0x78a8, 0x78ab); A = W8(wScreenTransitionDirection);
  CYC(0x78ab, 0x78ac); alu_add(gb, A);
  CYC(0x78ac, 0x78ad); add_double_index_to_hl(gb, 0x78ad);
  CYC(0x78ad, 0x78ae); A = C;
  CYC(0x78ae, 0x78b0); alu_and(gb, 0xf0);
  CYC(0x78b0, 0x78b2); A = alu_swap(gb, A);
  CYC(0x78b2, 0x78b3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x78b3, 0x78b5); goto invalid; }
  CYC(0x78b3, 0x78b5); CYC(0x78b5, 0x78b6); SET_HL(HL + 1);
  CYC(0x78b6, 0x78b7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x78b7, 0x78b9); goto invalid; }
  CYC(0x78b7, 0x78b9); A = C;
  CYC(0x78b9, 0x78ba); alu_and(gb, 0x0f);
  CYC(0x78ba, 0x78bc); SET_HL(HL + 1);
  CYC(0x78bc, 0x78be); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x78be, 0x78c0); goto invalid; }
  CYC(0x78be, 0x78c0); CYC(0x78c0, 0x78c1); SET_HL(HL + 1);
  CYC(0x78c1, 0x78c2); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x78c2, 0x78c4); goto invalid; }
  CYC(0x78c2, 0x78c4); CYC(0x78c4, 0x78c5); alu_xor(gb, A);
  CYC(0x78c5, 0x78c6); ret_effect(gb);
  return;
invalid:
  CYC(0x78c6, 0x78c7); alu_scf(gb);
  CYC(0x78c7, 0x78c8); ret_effect(gb);
}

void checkPositionValidForEnemySpawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7894, 0x7897); A = W8(wScreenVariables);
  CYC(0x7897, 0x7899); alu_and(gb, 8);
  if (!(F & FZ)) { CYC(0x7899, 0x789b); check_enemy_position_bounds(gb); return; }
  CYCT(0x7899, 0x789b); CYC(0x78e8, 0x78e9); alu_xor(gb, A);
  CYC(0x78e9, 0x78ec); W8(wScreenTransitionDirection) = A;
  CYC(0x78ec, 0x78ef); A = W8(wWarpDestPos);
  CYC(0x78ef, 0x78f0); B = A;
  CYC(0x78f0, 0x78f2); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(0x78f2, 0x78f4); check_enemy_position_bounds(gb); return; }
  CYC(0x78f2, 0x78f4); CYC(0x78f4, 0x78f6); alu_and(gb, 0xf0);
  CYC(0x78f6, 0x78f8); A = alu_swap(gb, A);
  CYC(0x78f8, 0x78f9); H = A;
  CYC(0x78f9, 0x78fa); A = C;
  CYC(0x78fa, 0x78fc); alu_and(gb, 0xf0);
  CYC(0x78fc, 0x78fe); A = alu_swap(gb, A);
  CYC(0x78fe, 0x78ff); alu_sub(gb, H);
  CYC(0x78ff, 0x7902); push_effect(gb, 0x7902); get_absolute_value(gb);
  CYC(0x7902, 0x7904); alu_cp(gb, 3);
  if (F & FC) { CYCT(0x7904, 0x7906); goto check_x; }
  CYC(0x7904, 0x7906); goto valid;
check_x:
  CYC(0x7908, 0x7909); A = B;
  CYC(0x7909, 0x790b); alu_and(gb, 0x0f);
  CYC(0x790b, 0x790c); H = A;
  CYC(0x790c, 0x790d); A = C;
  CYC(0x790d, 0x790f); alu_and(gb, 0x0f);
  CYC(0x790f, 0x7910); alu_sub(gb, H);
  CYC(0x7910, 0x7913); push_effect(gb, 0x7913); get_absolute_value(gb);
  CYC(0x7913, 0x7915); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(0x7915, 0x7917); goto valid; }
  CYC(0x7915, 0x7917); CYC(0x7917, 0x7918); alu_scf(gb);
  CYC(0x7918, 0x7919); ret_effect(gb);
  return;
valid:
  CYC(0x7906, 0x7907); alu_xor(gb, A);
  CYC(0x7907, 0x7908); ret_effect(gb);
}

void getNextValueFromRandomBuffer_hook(GB *gb) {
  CYC(0x7959, 0x795c); SET_HL(wEnemyPlacement_randomBufferIndex);
  CYC(0x795c, 0x795d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x795d, 0x795f); A = 4;
  CYC(0x795f, 0x7961); mem_wr(gb, IO_SVBK, A);
  CYC(0x7961, 0x7962); L = mem_rd(gb, HL);
  CYC(0x7962, 0x7964); H = 0xd0;
  CYC(0x7964, 0x7965); H = mem_rd(gb, HL);
  CYC(0x7965, 0x7967); A = 1;
  CYC(0x7967, 0x7969); mem_wr(gb, IO_SVBK, A);
  CYC(0x7969, 0x796a); A = H;
  CYC(0x796a, 0x796b); ret_effect(gb);
}

void checkEnemyPlacedAtPosition_hook(GB *gb) {
  CYC(0x79a7, 0x79aa); A = W8(wEnemyPlacement_numEnemies);
  CYC(0x79aa, 0x79ab); alu_or(gb, A);
  if (F & FZ) { CYCT(0x79ab, 0x79ac); ret_effect(gb); return; }
  CYC(0x79ab, 0x79ac); CYC(0x79ac, 0x79ad); push_effect(gb, BC);
  CYC(0x79ad, 0x79ae); C = A;
  CYC(0x79ae, 0x79b1); SET_HL(wEnemyPlacement_placedEnemyPositions);
  for (;;) {
    CYC(0x79b1, 0x79b2); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x79b2, 0x79b3); alu_cp(gb, B);
    if (F & FZ) { CYCT(0x79b3, 0x79b5); break; }
    CYC(0x79b3, 0x79b5); CYC(0x79b5, 0x79b6); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x79b6, 0x79b8); continue; }
    CYC(0x79b6, 0x79b8);
    CYC(0x79b8, 0x79b9); SET_BC(pop_effect(gb));
    CYC(0x79b9, 0x79ba); alu_xor(gb, A);
    CYC(0x79ba, 0x79bb); ret_effect(gb);
    return;
  }
  CYC(0x79bb, 0x79bc); SET_BC(pop_effect(gb));
  CYC(0x79bc, 0x79bd); alu_scf(gb);
  CYC(0x79bd, 0x79be); ret_effect(gb);
}

void getCandidatePositionForEnemy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x796b, 0x796e); A = W8(wActiveGroup);
  CYC(0x796e, 0x7970); alu_and(gb, 4);
  if (!(F & FZ)) { CYCT(0x7970, 0x7972); goto dungeon; }
  CYC(0x7970, 0x7972);
overworld:
  CALL_C(0x7972, getNextValueFromRandomBuffer_hook, 0x7959, 0x7975);
  CYC(0x7975, 0x7977); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(0x7977, 0x7979); goto overworld; }
  CYC(0x7977, 0x7979);
  CYC(0x7979, 0x797a); B = A;
  CYC(0x797a, 0x797c); alu_and(gb, 0x0f);
  CYC(0x797c, 0x797e); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x797e, 0x7980); goto overworld; }
  CYC(0x797e, 0x7980);
  CALL_C(0x7980, checkEnemyPlacedAtPosition_hook, 0x79a7, 0x7983);
  if (F & FC) { CYCT(0x7983, 0x7985); goto overworld; }
  CYC(0x7983, 0x7985); CYC(0x7985, 0x7986); A = B;
  CYC(0x7986, 0x7987); ret_effect(gb);
  return;
dungeon:
  CALL_C(0x7987, getNextValueFromRandomBuffer_hook, 0x7959, 0x798a);
  CYC(0x798a, 0x798c); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(0x798c, 0x798e); goto dungeon; }
  CYC(0x798c, 0x798e);
  CYC(0x798e, 0x798f); B = A;
  CYC(0x798f, 0x7991); alu_and(gb, 0xf0);
  if (F & FZ) { CYCT(0x7991, 0x7993); goto dungeon; }
  CYC(0x7991, 0x7993);
  CYC(0x7993, 0x7995); alu_cp(gb, 0xa0);
  if (F & FZ) { CYCT(0x7995, 0x7997); goto dungeon; }
  CYC(0x7995, 0x7997);
  CYC(0x7997, 0x7998); A = B;
  CYC(0x7998, 0x799a); alu_and(gb, 0x0f);
  if (F & FZ) { CYCT(0x799a, 0x799c); goto dungeon; }
  CYC(0x799a, 0x799c);
  CYC(0x799c, 0x799e); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(0x799e, 0x79a0); goto dungeon; }
  CYC(0x799e, 0x79a0);
  CALL_C(0x79a0, checkEnemyPlacedAtPosition_hook, 0x79a7, 0x79a3);
  if (F & FC) { CYCT(0x79a3, 0x79a5); goto dungeon; }
  CYC(0x79a3, 0x79a5); CYC(0x79a5, 0x79a6); A = B;
  CYC(0x79a6, 0x79a7); ret_effect(gb);
}

void calculateRoomStateModifier_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x79e2, 0x79e5); A = W8(wActiveGroup);
  CYC(0x79e5, 0x79e6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x79e6, 0x79e8); goto standard; }
  CYC(0x79e6, 0x79e8);
  CYC(0x79e8, 0x79eb); A = W8(wRoomPack);
  CYC(0x79eb, 0x79ed); alu_cp(gb, 0x7f);
  if (F & FZ) { CYCT(0x79ed, 0x79ef); goto companion_region; }
  CYC(0x79ed, 0x79ef);
standard:
  CYC(0x79ef, 0x79f2); A = W8(wTilesetFlags);
  CYC(0x79f2, 0x79f4); alu_and(gb, 0x40);
  CYC(0x79f4, 0x79f6); B = 0;
  if (F & FZ) CYCT(0x79f6, 0x79f8);
  else { CYC(0x79f6, 0x79f8); CYC(0x79f8, 0x79f9); B = alu_inc8(gb, B); }
  CALL_C(0x79f9, getThisRoomFlags_hook, 0x197d, 0x79fc);
  CYC(0x79fc, 0x79fe); alu_and(gb, 1);
  if (F & FZ) CYCT(0x79fe, 0x7a00);
  else { CYC(0x79fe, 0x7a00); CYC(0x7a00, 0x7a01); B = alu_inc8(gb, B); }
  CYC(0x7a01, 0x7a02); A = B;
  CYC(0x7a02, 0x7a05); W8(wRoomStateModifier) = A;
  CYC(0x7a05, 0x7a06); ret_effect(gb);
  return;
companion_region:
  CYC(0x7a06, 0x7a09); A = W8(wAnimalCompanion);
  CYC(0x7a09, 0x7a0a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7a0a, 0x7a0c); goto standard; }
  CYC(0x7a0a, 0x7a0c);
  CYC(0x7a0c, 0x7a0e); alu_sub(gb, 0x0b);
  CYC(0x7a0e, 0x7a11); W8(wRoomStateModifier) = A;
  CYC(0x7a11, 0x7a12); ret_effect(gb);
}

void createSeaEffectsPartIfApplicable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a12, 0x7a15); A = W8(wActiveCollisions);
  CYC(0x7a15, 0x7a18); SET_HL(0x7a2c);
  CYC(0x7a18, 0x7a19); push_effect(gb, 0x7a19); add_a_to_hl(gb);
  CYC(0x7a19, 0x7a1a); A = mem_rd(gb, HL);
  CYC(0x7a1a, 0x7a1b); push_effect(gb, 0x7a1b); add_a_to_hl(gb);
  for (;;) {
    CYC(0x7a1b, 0x7a1c); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x7a1c, 0x7a1d); alu_or(gb, A);
    if (F & FZ) { CYCT(0x7a1d, 0x7a1e); ret_effect(gb); return; }
    CYC(0x7a1d, 0x7a1e); CYC(0x7a1e, 0x7a1f); push_effect(gb, HL);
    CALL_C(0x7a1f, findTileInRoom_hook, 0x15cc, 0x7a22);
    CYC(0x7a22, 0x7a23); SET_HL(pop_effect(gb));
    if (!(F & FZ)) { CYCT(0x7a23, 0x7a25); continue; }
    CYC(0x7a23, 0x7a25);
    CALL_C(0x7a25, getFreePartSlot_hook, ROM_getFreePartSlot, 0x7a28);
    if (!(F & FZ)) { CYCT(0x7a28, 0x7a29); ret_effect(gb); return; }
    CYC(0x7a28, 0x7a29); CYC(0x7a29, 0x7a2b); mem_wr(gb, HL, 0x2e);
    CYC(0x7a2b, 0x7a2c); ret_effect(gb);
    return;
  }
}

void func_02_7a3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a3a, 0x7a3d); A = W8(wcddd);
  CYC(0x7a3d, 0x7a3e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7a3e, 0x7a3f); ret_effect(gb); return; }
  CYC(0x7a3e, 0x7a3f);
  CYC(0x7a3f, 0x7a40); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7a40, 0x7a42); goto spawn; }
  CYC(0x7a40, 0x7a42);
  CYC(0x7a42, 0x7a45); W8(wcddd) = A;
  CYC(0x7a45, 0x7a46); ret_effect(gb);
  return;
spawn:
  CALL_C(0x7a46, getFreeInteractionSlot_hook, ROM_getFreeInteractionSlot, 0x7a49);
  if (!(F & FZ)) { CYCT(0x7a49, 0x7a4a); ret_effect(gb); return; }
  CYC(0x7a49, 0x7a4a); CYC(0x7a4a, 0x7a4c); mem_wr(gb, HL, 0xde);
  CYC(0x7a4c, 0x7a4f); A = W8(wPortalPos);
  CYC(0x7a4f, 0x7a51); L = 0x4b;
  CYC(0x7a51, 0x7a54); setShortPosition_hook(gb);
}

void dungeonMap_drawItemSprites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x64da, getNumSmallKeys_hook, 0x651f, 0x64dd);
  CYC(0x64dd, 0x64e0); SET_HL(0x651a);
  if (!(F & FZ)) CALL_C_CC(0x64e0, addSpritesToOam_hook, 0x0d5e, 0x64e3);
  else CYC(0x64e0, 0x64e3);
  CALL_C(0x64e3, checkLinkHasBossKey_hook, 0x6529, 0x64e6);
  CYC(0x64e6, 0x64e9); SET_HL(0x6511);
  if (!(F & FZ)) CALL_C_CC(0x64e9, addSpritesToOam_hook, 0x0d5e, 0x64ec);
  else CYC(0x64e9, 0x64ec);
  CALL_C(0x64ec, checkLinkHasCompass_hook, 0x6532, 0x64ef);
  CYC(0x64ef, 0x64f2); SET_HL(0x6508);
  if (!(F & FZ)) CALL_C_CC(0x64f2, addSpritesToOam_hook, 0x0d5e, 0x64f5);
  else CYC(0x64f2, 0x64f5);
  CALL_C(0x64f5, checkLinkHasMap_hook, 0x653e, 0x64f8);
  CYC(0x64f8, 0x64fb); SET_HL(0x64ff);
  if (!(F & FZ)) CALL_C_CC(0x64fb, addSpritesToOam_hook, 0x0d5e, 0x64fe);
  else CYC(0x64fb, 0x64fe);
  CYC(0x64fe, 0x64ff); ret_effect(gb);
}

void getNumSmallKeys_hook(GB *gb) {
  CYC(0x651f, 0x6522); A = W8(wDungeonIndex);
  CYC(0x6522, 0x6525); SET_HL(wDungeonSmallKeys);
  CYC(0x6525, 0x6526); push_effect(gb, 0x6526); add_a_to_hl(gb);
  CYC(0x6526, 0x6527); A = mem_rd(gb, HL);
  CYC(0x6527, 0x6528); alu_or(gb, A);
  CYC(0x6528, 0x6529); ret_effect(gb);
}

void checkLinkHasBossKey_hook(GB *gb) {
  CYC(0x6529, 0x652c); SET_HL(wDungeonBossKeys);
  CYC(0x652c, 0x652f); A = W8(wDungeonIndex);
  CYC(0x652f, 0x6532); checkFlag_hook(gb);
}

void checkLinkHasCompass_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6532, 0x6533); push_effect(gb, HL);
  CYC(0x6533, 0x6536); SET_HL(wDungeonCompasses);
  CYC(0x6536, 0x6539); A = W8(wDungeonIndex);
  CALL_C(0x6539, checkFlag_hook, 0x0205, 0x653c);
  CYC(0x653c, 0x653d); SET_HL(pop_effect(gb));
  CYC(0x653d, 0x653e); ret_effect(gb);
}

void checkLinkHasMap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x653e, 0x653f); push_effect(gb, HL);
  CYC(0x653f, 0x6542); SET_HL(wDungeonMaps);
  CYC(0x6542, 0x6545); A = W8(wDungeonIndex);
  CALL_C(0x6545, checkFlag_hook, 0x0205, 0x6548);
  CYC(0x6548, 0x6549); SET_HL(pop_effect(gb));
  CYC(0x6549, 0x654a); ret_effect(gb);
}

void dungeonMap_drawFloorCursor_hook(GB *gb) {
  CYC(0x654a, 0x654d); A = W8(wDungeonIndex);
  CYC(0x654d, 0x6550); SET_HL(0x691e);
  CYC(0x6550, 0x6551); add_double_index_to_hl(gb, 0x6551);
  CYC(0x6551, 0x6554); A = W8(wMapMenu_floorIndex);
  CYC(0x6554, 0x6556); A = alu_swap(gb, A);
  CYC(0x6556, 0x6557); alu_rrca(gb);
  CYC(0x6557, 0x6558); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6558, 0x6559); B = A;
  CYC(0x6559, 0x655b); C = 0;
  CYC(0x655b, 0x655e); SET_HL(0x6561);
  CYC(0x655e, 0x6561); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_drawBossSymbolForFloor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6566, checkLinkHasCompass_hook, 0x6532, 0x6569);
  if (F & FZ) { CYCT(0x6569, 0x656a); ret_effect(gb); return; }
  CYC(0x6569, 0x656a);
  CYC(0x656a, 0x656d); A = W8(wDungeonIndex);
  CYC(0x656d, 0x6570); SET_HL(0x691f);
  CYC(0x6570, 0x6571); add_double_index_to_hl(gb, 0x6571);
  CYC(0x6571, 0x6572); B = mem_rd(gb, HL);
  CYC(0x6572, 0x6574); C = 0;
  CYC(0x6574, 0x6577); SET_HL(0x657a);
  CYC(0x6577, 0x657a); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_drawLinkIcons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x657f, 0x6582); A = W8(wMapMenu_dungeonCursorFlicker);
  CYC(0x6582, 0x6583); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6583, 0x6585); goto floor_list; }
  CYC(0x6583, 0x6585);
  CALL_C(0x6585, dungeonMap_getLinkIconPosition_hook, 0x6756, 0x6588);
  CYC(0x6588, 0x658b); SET_HL(wMapMenu_dungeonScrollY);
  CYC(0x658b, 0x658c); A = B;
  CYC(0x658c, 0x658d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x658d, 0x658f); alu_cp(gb, 0x12);
  if (!(F & FC)) { CYCT(0x658f, 0x6591); goto floor_list; }
  CYC(0x658f, 0x6591);
  CYC(0x6591, 0x6592); A = alu_inc8(gb, A);
  CYC(0x6592, 0x6594); A = alu_swap(gb, A);
  CYC(0x6594, 0x6595); alu_rrca(gb);
  CYC(0x6595, 0x6596); B = A;
  CYC(0x6596, 0x6598); C = alu_swap(gb, C);
  CYC(0x6598, 0x659a); C = alu_rrc(gb, C);
  CYC(0x659a, 0x659d); SET_HL(0x65bd);
  CALL_C(0x659d, addSpritesToOam_withOffset_hook, 0x0d61, 0x65a0);
floor_list:
  CYC(0x65a0, 0x65a3); A = W8(wDungeonIndex);
  CYC(0x65a3, 0x65a6); SET_HL(0x691e);
  CYC(0x65a6, 0x65a7); add_double_index_to_hl(gb, 0x65a7);
  CYC(0x65a7, 0x65aa); A = W8(wMapMenu_linkFloor);
  CYC(0x65aa, 0x65ab); C = A;
  CYC(0x65ab, 0x65ae); A = W8(wDungeonNumFloors);
  CYC(0x65ae, 0x65af); A = alu_dec8(gb, A);
  CYC(0x65af, 0x65b0); alu_sub(gb, C);
  CYC(0x65b0, 0x65b2); A = alu_swap(gb, A);
  CYC(0x65b2, 0x65b3); alu_rrca(gb);
  CYC(0x65b3, 0x65b4); alu_add(gb, mem_rd(gb, HL));
  CYC(0x65b4, 0x65b5); B = A;
  CYC(0x65b5, 0x65b7); C = 0;
  CYC(0x65b7, 0x65ba); SET_HL(0x65c2);
  CYC(0x65ba, 0x65bd); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_updateCursorFlickerCounter_hook(GB *gb) {
  CYC(0x65c7, 0x65ca); A = W8(wFrameCounter);
  CYC(0x65ca, 0x65cc); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(0x65cc, 0x65cd); ret_effect(gb); return; }
  CYC(0x65cc, 0x65cd);
  CYC(0x65cd, 0x65d0); SET_HL(wMapMenu_dungeonCursorFlicker);
  CYC(0x65d0, 0x65d1); A = mem_rd(gb, HL);
  CYC(0x65d1, 0x65d3); alu_xor(gb, 1);
  CYC(0x65d3, 0x65d4); mem_wr(gb, HL, A);
  CYC(0x65d4, 0x65d5); ret_effect(gb);
}

void dungeonMap_drawCursor_hook(GB *gb) {
  CYC(0x65d5, 0x65d8); A = W8(wSubmenuState);
  CYC(0x65d8, 0x65d9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x65d9, 0x65da); ret_effect(gb); return; }
  CYC(0x65d9, 0x65da);
  CYC(0x65da, 0x65dd); A = W8(wMapMenu_dungeonCursorFlicker);
  CYC(0x65dd, 0x65de); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x65de, 0x65df); ret_effect(gb); return; }
  CYC(0x65de, 0x65df);
  CYC(0x65df, 0x65e2); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(0x65e2, 0x65e4); alu_and(gb, 0xf8);
  CYC(0x65e4, 0x65e5); B = A;
  CYC(0x65e5, 0x65e8); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(0x65e8, 0x65ea); alu_and(gb, 7);
  CYC(0x65ea, 0x65eb); alu_add(gb, A);
  CYC(0x65eb, 0x65ec); alu_add(gb, A);
  CYC(0x65ec, 0x65ed); alu_add(gb, A);
  CYC(0x65ed, 0x65ee); C = A;
  CYC(0x65ee, 0x65f1); SET_HL(0x65f4);
  CYC(0x65f1, 0x65f4); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_drawArrows_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65fd, 0x6600); A = W8(wSubmenuState);
  CYC(0x6600, 0x6601); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6601, 0x6602); ret_effect(gb); return; }
  CYC(0x6601, 0x6602);
  CALL_C(0x6602, dungeonMap_checkCanScrollUp_hook, 0x6454, 0x6605);
  if (F & FZ) { CYCT(0x6605, 0x6607); goto down_arrow; }
  CYC(0x6605, 0x6607);
  CYC(0x6607, 0x660a); SET_HL(0x6617);
  CALL_C(0x660a, addSpritesToOam_hook, 0x0d5e, 0x660d);
down_arrow:
  CALL_C(0x660d, dungeonMap_checkCanScrollDown_hook, 0x641b, 0x6610);
  if (F & FZ) { CYCT(0x6610, 0x6611); ret_effect(gb); return; }
  CYC(0x6610, 0x6611);
  CYC(0x6611, 0x6614); SET_HL(0x661c);
  CYC(0x6614, 0x6617); addSpritesToOam_hook(gb);
}

void mapGetRoomIndexWithoutUnusedColumns_hook(GB *gb) {
  CYC(0x6621, 0x6622); push_effect(gb, BC);
  CYC(0x6622, 0x6625); A = W8(wMapMenu_cursorIndex);
  CYC(0x6625, 0x6626); B = A;
  CYC(0x6626, 0x6628); alu_and(gb, 0xf0);
  CYC(0x6628, 0x662a); A = alu_swap(gb, A);
  CYC(0x662a, 0x662b); alu_add(gb, A);
  CYC(0x662b, 0x662c); C = A;
  CYC(0x662c, 0x662d); A = B;
  CYC(0x662d, 0x662e); alu_sub(gb, C);
  CYC(0x662e, 0x662f); B = A;
  CYC(0x662f, 0x6632); A = W8(wTilesetFlags);
  CYC(0x6632, 0x6633); alu_rlca(gb);
  CYC(0x6633, 0x6634); A = B;
  CYC(0x6634, 0x6635); SET_BC(pop_effect(gb));
  CYC(0x6635, 0x6636); ret_effect(gb);
}

void mapMenu_checkCursorRoomVisited_hook(GB *gb) {
  CYC(0x6636, 0x6639); A = W8(wMapMenu_cursorIndex);
  mapMenu_checkRoomVisited_hook(gb);
}

void mapMenu_checkRoomVisited_hook(GB *gb) {
  CYC(0x6639, 0x663a); push_effect(gb, HL);
  CYC(0x663a, 0x663b); H = A;
  CYC(0x663b, 0x663e); A = W8(wMapMenu_mode);
  CYC(0x663e, 0x663f); alu_rrca(gb);
  CYC(0x663f, 0x6640); A = H;
  CYC(0x6640, 0x6643); SET_HL(0xc800);
  if (F & FC) CYCT(0x6643, 0x6645);
  else { CYC(0x6643, 0x6645); CYC(0x6645, 0x6648); SET_HL(0xc700); }
  CYC(0x6648, 0x6649); push_effect(gb, 0x6649); add_a_to_hl(gb);
  CYC(0x6649, 0x664a); A = mem_rd(gb, HL);
  CYC(0x664a, 0x664c); alu_bit(gb, 4, A);
  CYC(0x664c, 0x664d); SET_HL(pop_effect(gb));
  CYC(0x664d, 0x664e); ret_effect(gb);
}

void mapMenu_drawArrow_hook(GB *gb) {
  CYC(0x664e, 0x6651); A = W8(wFrameCounter);
  CYC(0x6651, 0x6653); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x6653, 0x6654); ret_effect(gb); return; }
  CYC(0x6653, 0x6654);
  CYC(0x6654, 0x6657); SET_HL(0x665c);
  CYC(0x6657, 0x665a); A = W8(wMapMenu_currentRoom);
  CYC(0x665a, 0x665c); mapMenu_drawSpriteAtRoomIndex_hook(gb);
}

void mapMenu_drawCursor_hook(GB *gb) {
  CYC(0x6661, 0x6664); SET_HL(0x6669);
  CYC(0x6664, 0x6667); A = W8(wMapMenu_cursorIndex);
  CYC(0x6667, 0x6669); mapMenu_drawSpriteAtRoomIndex_hook(gb);
}

void mapMenu_drawSpriteAtRoomIndex_hook(GB *gb) {
  CYC(0x6672, 0x6673); C = A;
  CYC(0x6673, 0x6676); SET_DE(0x1018);
  CYC(0x6676, 0x6677); A = C;
  CYC(0x6677, 0x6679); alu_and(gb, 0xf0);
  CYC(0x6679, 0x667b); A = alu_srl(gb, A);
  CYC(0x667b, 0x667c); alu_add(gb, D);
  CYC(0x667c, 0x667d); B = A;
  CYC(0x667d, 0x667e); A = C;
  CYC(0x667e, 0x6680); alu_and(gb, 0x0f);
  CYC(0x6680, 0x6681); alu_add(gb, A);
  CYC(0x6681, 0x6682); alu_add(gb, A);
  CYC(0x6682, 0x6683); alu_add(gb, A);
  CYC(0x6683, 0x6684); alu_add(gb, E);
  CYC(0x6684, 0x6685); C = A;
  CYC(0x6685, 0x6688); addSpritesToOam_withOffset_hook(gb);
}

void mapMenu_drawWarpSites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6688, 0x668b); SET_DE(0x66b9);
  CYC(0x668b, 0x668e); SET_HL(wTmpcec0);
  CYC(0x668e, 0x6690); B = 5;
  CALL_C(0x6690, copyMemoryReverse_hook, 0x047f, 0x6693);
  CYC(0x6693, 0x6696); A = W8(wFrameCounter);
  CYC(0x6696, 0x6698); alu_and(gb, 0x18);
  CYC(0x6698, 0x6699); alu_rrca(gb);
  CYC(0x6699, 0x669a); alu_rrca(gb);
  CYC(0x669a, 0x669c); L = 0xc3;
  CYC(0x669c, 0x669d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x669d, 0x669e); mem_wr(gb, HL, A);
  CYC(0x669e, 0x66a0); C = 0;
  mapMenu_drawWarpSites__drawWarpDest_hook(gb);
}

void mapMenu_drawWarpSites__drawWarpDest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66a0, 0x66a1); A = C;
  CALL_C(0x66a1, getTreeWarpDataIndex_hook, 0x66be, 0x66a4);
  CYC(0x66a4, 0x66a5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x66a5, 0x66a6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x66a6, 0x66a7); ret_effect(gb); return; }
  CYC(0x66a6, 0x66a7);
  CYC(0x66a7, 0x66a8); push_effect(gb, BC);
  CYC(0x66a8, 0x66a9); C = A;
  CALL_C(0x66a9, mapMenu_checkRoomVisited_hook, 0x6639, 0x66ac);
  if (F & FZ) { CYCT(0x66ac, 0x66ae); mapMenu_drawWarpSites__nextTree_hook(gb); return; }
  CYC(0x66ac, 0x66ae);
  CYC(0x66ae, 0x66af); A = C;
  CYC(0x66af, 0x66b2); SET_HL(wTmpcec0);
  CALL_C(0x66b2, mapMenu_drawSpriteAtRoomIndex_hook, 0x6672, 0x66b5);
  mapMenu_drawWarpSites__nextTree_hook(gb);
}

void mapMenu_drawWarpSites__nextTree_hook(GB *gb) {
  CYC(0x66b5, 0x66b6); SET_BC(pop_effect(gb));
  CYC(0x66b6, 0x66b7); C = alu_inc8(gb, C);
  CYC(0x66b7, 0x66b9); mapMenu_drawWarpSites__drawWarpDest_hook(gb);
}

void getTreeWarpDataIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66be, 0x66bf); C = A;
  CALL_C(0x66bf, getWarpTreeData_hook, 0x66d4, 0x66c2);
  CYC(0x66c2, 0x66c3); alu_add(gb, A);
  CYC(0x66c3, 0x66c4); alu_add(gb, C);
  CYC(0x66c4, 0x66c5); push_effect(gb, 0x66c5); add_a_to_hl(gb);
  CYC(0x66c5, 0x66c6); ret_effect(gb);
}

void getTreeWarpDataForRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66c6, 0x66c7); C = A;
  CALL_C(0x66c7, getWarpTreeData_hook, 0x66d4, 0x66ca);
  for (;;) {
    CYC(0x66ca, 0x66cb); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x66cb, 0x66cc); alu_or(gb, A);
    CYC(0x66cc, 0x66cd); alu_scf(gb);
    if (F & FZ) { CYCT(0x66cd, 0x66ce); ret_effect(gb); return; }
    CYC(0x66cd, 0x66ce);
    CYC(0x66ce, 0x66cf); alu_cp(gb, C);
    if (F & FZ) { CYCT(0x66cf, 0x66d0); ret_effect(gb); return; }
    CYC(0x66cf, 0x66d0);
    CYC(0x66d0, 0x66d1); SET_HL(HL + 1);
    CYC(0x66d1, 0x66d2); SET_HL(HL + 1);
    CYC(0x66d2, 0x66d4);
  }
}

void getWarpTreeData_hook(GB *gb) {
  CYC(0x66d4, 0x66d5); push_effect(gb, AF);
  CYC(0x66d5, 0x66d8); SET_HL(0x6d1e);
  CYC(0x66d8, 0x66db); A = W8(wTilesetFlags);
  CYC(0x66db, 0x66dc); alu_rlca(gb);
  if (F & FC) { CYCT(0x66dc, 0x66de); getWarpTreeData__ret_hook(gb); return; }
  CYC(0x66dc, 0x66de);
  CYC(0x66de, 0x66e1); SET_HL(0x6d03);
  CYC(0x66e1, 0x66e4); A = mem_rd(gb, 0xc7ac);
  CYC(0x66e4, 0x66e5); alu_rlca(gb);
  if (F & FC) { CYCT(0x66e5, 0x66e7); getWarpTreeData__ret_hook(gb); return; }
  CYC(0x66e5, 0x66e7);
  CYC(0x66e7, 0x66e9); A = 3;
  CYC(0x66e9, 0x66ea); push_effect(gb, 0x66ea); add_a_to_hl(gb);
  getWarpTreeData__ret_hook(gb);
}

void getWarpTreeData__ret_hook(GB *gb) {
  CYC(0x66ea, 0x66eb); SET_AF(pop_effect(gb));
  CYC(0x66eb, 0x66ec); ret_effect(gb);
}

void mapMenu_drawTimePortal_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66ec, 0x66ef); SET_DE(0x6717);
  CYC(0x66ef, 0x66f2); SET_HL(wTmpcec0);
  CYC(0x66f2, 0x66f4); B = 5;
  CALL_C(0x66f4, copyMemoryReverse_hook, 0x047f, 0x66f7);
  CYC(0x66f7, 0x66f9); L = 0xc3;
  CYC(0x66f9, 0x66fc); A = W8(wFrameCounter);
  CYC(0x66fc, 0x66fd); alu_add(gb, A);
  CYC(0x66fd, 0x66ff); A = alu_swap(gb, A);
  CYC(0x66ff, 0x6701); alu_and(gb, 3);
  CYC(0x6701, 0x6702); alu_add(gb, A);
  CYC(0x6702, 0x6703); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6703, 0x6704); mem_wr(gb, HL, A);
  CYC(0x6704, 0x6707); SET_HL(wPortalGroup);
  CYC(0x6707, 0x670a); A = W8(wTilesetFlags);
  CYC(0x670a, 0x670b); alu_rlca(gb);
  CYC(0x670b, 0x670d); alu_and(gb, 1);
  CYC(0x670d, 0x670e); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x670e, 0x670f); ret_effect(gb); return; }
  CYC(0x670e, 0x670f);
  CYC(0x670f, 0x6710); L = alu_inc8(gb, L);
  CYC(0x6710, 0x6711); A = mem_rd(gb, HL);
  CYC(0x6711, 0x6714); SET_HL(wTmpcec0);
  CYC(0x6714, 0x6717); mapMenu_drawSpriteAtRoomIndex_hook(gb);
}

void mapMenu_clearUnvisitedTiles_hook(GB *gb) {
  CYC(0x671c, 0x671e); A = 4;
  CYC(0x671e, 0x6720); hram_wr(gb, 0x70, A);
  CYC(0x6720, 0x6723); SET_DE(0x0e0e);
  CYC(0x6723, 0x6726); SET_HL(0xd043);
  CYC(0x6726, 0x6728); B = 0;
  mapMenu_clearUnvisitedTiles__rowLoop_hook(gb);
}

void mapMenu_clearUnvisitedTiles__rowLoop_hook(GB *gb) {
  CYC(0x6728, 0x672a); C = 0;
  CYC(0x672a, 0x672b); push_effect(gb, DE);
  mapMenu_clearUnvisitedTiles__columnLoop_hook(gb);
}

void mapMenu_clearUnvisitedTiles__columnLoop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x672b, 0x672c); A = B;
  CYC(0x672c, 0x672e); A = alu_swap(gb, A);
  CYC(0x672e, 0x672f); alu_add(gb, C);
  CALL_C(0x672f, mapMenu_checkRoomVisited_hook, 0x6639, 0x6732);
  if (!(F & FZ)) { CYCT(0x6732, 0x6734); mapMenu_clearUnvisitedTiles__nextTile_hook(gb); return; }
  CYC(0x6732, 0x6734);
  CYC(0x6734, 0x6736); mem_wr(gb, HL, 4);
  CYC(0x6736, 0x6738); H = (uint8_t)(H | (1 << 2));
  CYC(0x6738, 0x673a); mem_wr(gb, HL, 0x0a);
  CYC(0x673a, 0x673c); H = (uint8_t)(H & ~(1 << 2));
  mapMenu_clearUnvisitedTiles__nextTile_hook(gb);
}

void mapMenu_clearUnvisitedTiles__nextTile_hook(GB *gb) {
  CYC(0x673c, 0x673d); SET_HL(HL + 1);
  CYC(0x673d, 0x673e); C = alu_inc8(gb, C);
  CYC(0x673e, 0x673f); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(0x673f, 0x6741); mapMenu_clearUnvisitedTiles__columnLoop_hook(gb); return; }
  CYC(0x673f, 0x6741);
  CYC(0x6741, 0x6742); SET_DE(pop_effect(gb));
  CYC(0x6742, 0x6744); A = 0x20;
  CYC(0x6744, 0x6745); alu_sub(gb, E);
  CYC(0x6745, 0x6746); push_effect(gb, 0x6746); add_a_to_hl(gb);
  CYC(0x6746, 0x6747); B = alu_inc8(gb, B);
  CYC(0x6747, 0x6748); D = alu_dec8(gb, D);
  if (!(F & FZ)) { CYCT(0x6748, 0x674a); mapMenu_clearUnvisitedTiles__rowLoop_hook(gb); return; }
  CYC(0x6748, 0x674a);
  CYC(0x674a, 0x674b); ret_effect(gb);
}

void checkMoblinsKeepDestroyed_hook(GB *gb) {
  CYC(0x674b, 0x674d); A = 0x1a;
  CYC(0x674d, 0x6750); checkGlobalFlag_hook(gb);
}

void checkAdvanceShopVisited_hook(GB *gb) {
  CYC(0x6750, 0x6753); A = mem_rd(gb, 0xc8fe);
  CYC(0x6753, 0x6755); alu_and(gb, 0x10);
  CYC(0x6755, 0x6756); ret_effect(gb);
}

void dungeonMap_getLinkIconPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6756, 0x6759); A = W8(wMapMenu_linkFloor);
  CYC(0x6759, 0x675a); B = A;
  CYC(0x675a, 0x675d); A = W8(wDungeonNumFloors);
  CYC(0x675d, 0x675e); A = alu_dec8(gb, A);
  CYC(0x675e, 0x675f); alu_sub(gb, B);
  CYC(0x675f, 0x6760); H = A;
  CALL_C(0x6760, multiplyABy8_hook, 0x01b7, 0x6763);
  CYC(0x6763, 0x6764); A = H;
  CYC(0x6764, 0x6765); alu_add(gb, A);
  CYC(0x6765, 0x6766); alu_add(gb, C);
  CYC(0x6766, 0x6768); alu_add(gb, 5);
  CYC(0x6768, 0x6769); B = A;
  CYC(0x6769, 0x676c); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(0x676c, 0x676e); alu_and(gb, 0xf8);
  CYC(0x676e, 0x6770); A = alu_swap(gb, A);
  CYC(0x6770, 0x6771); alu_rlca(gb);
  CYC(0x6771, 0x6772); C = A;
  CYC(0x6772, 0x6773); A = B;
  CYC(0x6773, 0x6774); alu_add(gb, C);
  CYC(0x6774, 0x6775); B = A;
  CYC(0x6775, 0x6778); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(0x6778, 0x677a); alu_and(gb, 7);
  CYC(0x677a, 0x677b); C = A;
  CYC(0x677b, 0x677c); ret_effect(gb);
}

void dungeonMap_drawFloorList_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x677c, 0x677e); A = 0x04;
  CYC(0x677e, 0x6780); hram_wr(gb, 0x70, A);
  CYC(0x6780, 0x6783); A = W8(wDungeonIndex);
  CYC(0x6783, 0x6786); SET_HL(0x6910);
  CYC(0x6786, 0x6787); push_effect(gb, 0x6787); add_a_to_hl(gb);
  CYC(0x6787, 0x6788); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6788, 0x678b); SET_DE(0xd0a0);
  CALL_C(0x678b, addAToDe_hook, 0x0068, 0x678e);
  CYC(0x678e, 0x6791); A = W8(wDungeonNumFloors);
  CYC(0x6791, 0x6792); A = alu_dec8(gb, A);
  CYC(0x6792, 0x6793); C = A;
  dungeonMap_drawFloorList__loop_hook(gb);
}

void dungeonMap_drawFloorList__loop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6793, checkLinkHasMap_hook, 0x653e, 0x6796);
  if (!(F & FZ)) {
    CYCT(0x6796, 0x6798);
  } else {
    CYC(0x6796, 0x6798);
    CYC(0x6798, 0x6799); A = C;
    CYC(0x6799, 0x679c); SET_HL(0x00f8);
    CYC(0x679c, 0x679d); alu_add(gb, L);
    CYC(0x679d, 0x679e); L = A;
    CYC(0x679e, 0x67a1); A = W8(wMapMenu_visitedFloors);
    CYC(0x67a1, 0x67a2); alu_and(gb, mem_rd(gb, HL));
    CYC(0x67a2, 0x67a4); A = 0x20;
    if (F & FZ) {
      CYCT(0x67a4, 0x67a6);
      dungeonMap_drawFloorList__nextFloor_hook(gb);
      return;
    }
    CYC(0x67a4, 0x67a6);
  }
  CYC(0x67a6, 0x67a9); A = W8(wDungeonMapBaseFloor);
  CYC(0x67a9, 0x67aa); alu_add(gb, C);
  CYC(0x67aa, 0x67ad); SET_HL(0x68fa);
  CYC(0x67ad, 0x67ae); add_double_index_to_hl(gb, 0x67ae);
  CYC(0x67ae, 0x67b0); B = 0x02;
  CYC(0x67b0, 0x67b1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x67b1, drawTileABtoDE_hook, 0x67d5, 0x67b4);
  CYC(0x67b4, 0x67b5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x67b5, drawTileABtoDE_hook, 0x67d5, 0x67b8);
  CYC(0x67b8, 0x67ba); A = 0x9c;
  CALL_C(0x67ba, drawTileABtoDE_hook, 0x67d5, 0x67bd);
  CYC(0x67bd, 0x67be); E = alu_inc8(gb, E);
  CYC(0x67be, 0x67c0); B = 0x04;
  CYC(0x67c0, 0x67c2); A = 0xaa;
  CALL_C(0x67c2, drawTileABtoDE_hook, 0x67d5, 0x67c5);
  CYC(0x67c5, 0x67c7); A = 0xab;
  CALL_C(0x67c7, drawTileABtoDE_hook, 0x67d5, 0x67ca);
  CYC(0x67ca, 0x67cc); A = 0x1a;
  dungeonMap_drawFloorList__nextFloor_hook(gb);
}

void dungeonMap_drawFloorList__nextFloor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x67cc, addAToDe_hook, 0x0068, 0x67cf);
  CYC(0x67cf, 0x67d0); A = C;
  CYC(0x67d0, 0x67d1); C = alu_dec8(gb, C);
  CYC(0x67d1, 0x67d2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x67d2, 0x67d4);
    dungeonMap_drawFloorList__loop_hook(gb);
    return;
  }
  CYC(0x67d2, 0x67d4);
  CYC(0x67d4, 0x67d5); ret_effect(gb);
}

void drawTileABtoDE_hook(GB *gb) {
  CYC(0x67d5, 0x67d6); mem_wr(gb, DE, A);
  CYC(0x67d6, 0x67d8); D |= 0x04;
  CYC(0x67d8, 0x67d9); A = B;
  CYC(0x67d9, 0x67da); mem_wr(gb, DE, A);
  CYC(0x67da, 0x67dc); D &= (uint8_t)~0x04;
  CYC(0x67dc, 0x67dd); SET_DE(DE + 1);
  CYC(0x67dd, 0x67de); ret_effect(gb);
}

void dungeonMap_updateScroll_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x682c, 0x682e); A = hram_rd(gb, 0x70);
  CYC(0x682e, 0x682f); push_effect(gb, AF);
  CYC(0x682f, 0x6832); A = W8(wMapMenu_dungeonScrollY);
  CALL_C(0x6832, multiplyABy8_hook, 0x01b7, 0x6835);
  CYC(0x6835, 0x6838); SET_HL(0xdc00);
  CYC(0x6838, 0x6839); alu_add_hl(gb, BC);
  CYC(0x6839, 0x683c); SET_DE(0xd00a);
  CYC(0x683c, 0x683e); A = 0x12;
  CYC(0x683e, 0x6840); hram_wr(gb, 0x8d, A);
  dungeonMap_updateScroll__nextRow_hook(gb);
}

void dungeonMap_updateScroll__nextRow_hook(GB *gb) {
  CYC(0x6840, 0x6842); C = 0x08;
  dungeonMap_updateScroll__nextColumn_hook(gb);
}

void dungeonMap_updateScroll__nextColumn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x6842, 0x6844); A = 0x04;
    CYC(0x6844, 0x6846); hram_wr(gb, 0x70, A);
    CYC(0x6846, 0x6847); A = mem_rd(gb, HL);
    CYC(0x6847, 0x6849); B = 0x00;
    CYC(0x6849, 0x684b); alu_cp(gb, 0x83);
    if (F & FZ) {
      CYCT(0x684b, 0x684d);
    } else {
      CYC(0x684b, 0x684d);
      CYC(0x684d, 0x684f); alu_cp(gb, 0xad);
      if (F & FZ) {
        CYCT(0x684f, 0x6851);
      } else {
        CYC(0x684f, 0x6851);
        CYC(0x6851, 0x6853); B = 0x02;
        CYC(0x6853, 0x6855); alu_cp(gb, 0xae);
        if (F & FZ) {
          CYCT(0x6855, 0x6857);
        } else {
          CYC(0x6855, 0x6857);
          CYC(0x6857, 0x6859); B = 0x04;
          CYC(0x6859, 0x685b); alu_cp(gb, 0xaf);
          if (F & FZ) {
            CYCT(0x685b, 0x685d);
          } else {
            CYC(0x685b, 0x685d);
            CYC(0x685d, 0x685f); B = 0x05;
          }
        }
      }
    }
    CALL_C(0x685f, drawTileABtoDE_hook, 0x67d5, 0x6862);
    CYC(0x6862, 0x6863); SET_HL(HL + 1);
    CYC(0x6863, 0x6864); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(0x6864, 0x6866);
      continue;
    }
    CYC(0x6864, 0x6866);
    break;
  }
  CYC(0x6866, 0x6868); A = 0x18;
  CALL_C(0x6868, addAToDe_hook, 0x0068, 0x686b);
  CYC(0x686b, 0x686d); A = hram_rd(gb, 0x8d);
  CYC(0x686d, 0x686e); A = alu_dec8(gb, A);
  CYC(0x686e, 0x6870); hram_wr(gb, 0x8d, A);
  if (!(F & FZ)) {
    CYCT(0x6870, 0x6872);
    dungeonMap_updateScroll__nextRow_hook(gb);
    return;
  }
  CYC(0x6870, 0x6872);
  CYC(0x6872, 0x6873); SET_AF(pop_effect(gb));
  CYC(0x6873, 0x6875); hram_wr(gb, 0x70, A);
  dungeonMap_getTileForRoom_hook(gb);
}

void dungeonMap_getTileForRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6875, 0x6876); push_effect(gb, BC);
  CYC(0x6876, 0x6877); push_effect(gb, DE);
  CYC(0x6877, 0x6878); B = A;
  CYC(0x6878, 0x6879); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6879, 0x687b);
    dungeonMap_getTileForRoom__hidden_hook(gb);
    return;
  }
  CYC(0x6879, 0x687b);
  CYC(0x687b, 0x687c); push_effect(gb, HL);
  CYC(0x687c, 0x687d); L = B;
  CYC(0x687d, 0x6880); A = W8(wDungeonFlagsAddressH);
  CYC(0x6880, 0x6881); H = A;
  CYC(0x6881, 0x6882); D = mem_rd(gb, HL);
  CALL_C(0x6882, getRoomDungeonProperties_hook, 0x1ad7, 0x6885);
  CYC(0x6885, 0x6886); E = B;
  CYC(0x6886, 0x6887); SET_HL(pop_effect(gb));
  CYC(0x6887, 0x6888); A = E;
  CYC(0x6888, 0x688a); alu_cp(gb, 0x60);
  if (F & FZ) {
    CYCT(0x688a, 0x688c);
    dungeonMap_getTileForRoom__hidden_hook(gb);
    return;
  }
  CYC(0x688a, 0x688c);
  CYC(0x688c, 0x688e); alu_cp(gb, 0x70);
  if (F & FZ) {
    CYCT(0x688e, 0x6890);
    dungeonMap_getTileForRoom__hidden_hook(gb);
    return;
  }
  CYC(0x688e, 0x6890);
  CYC(0x6890, 0x6892); alu_bit(gb, 4, D);
  if (!(F & FZ)) {
    CYCT(0x6892, 0x6894);
    dungeonMap_getTileForRoom__visited_hook(gb);
    return;
  }
  CYC(0x6892, 0x6894);
  CALL_C(0x6894, dungeonMap_checkCompassTile_hook, 0x68ce, 0x6897);
  if (!(F & FZ)) {
    CYCT(0x6897, 0x6899);
    dungeonMap_getTileForRoom__ret_hook(gb);
    return;
  }
  CYC(0x6897, 0x6899);
  CALL_C(0x6899, checkLinkHasMap_hook, 0x653e, 0x689c);
  CYC(0x689c, 0x689e); A = 0xaf;
  if (!(F & FZ)) {
    CYCT(0x689e, 0x68a0);
    dungeonMap_getTileForRoom__ret_hook(gb);
    return;
  }
  CYC(0x689e, 0x68a0);
  dungeonMap_getTileForRoom__hidden_hook(gb);
}

void dungeonMap_getTileForRoom__hidden_hook(GB *gb) {
  CYC(0x68a0, 0x68a2); A = 0xac;
  CYC(0x68a2, 0x68a4); dungeonMap_getTileForRoom__ret_hook(gb);
}

void dungeonMap_getTileForRoom__visited_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x68ad, dungeonMap_checkCompassTile_hook, 0x68ce, 0x68b0);
  if (!(F & FZ)) {
    CYCT(0x68b0, 0x68b2);
    dungeonMap_getTileForRoom__ret_hook(gb);
    return;
  }
  CYC(0x68b0, 0x68b2);
  CYC(0x68b2, 0x68b3); A = D;
  CYC(0x68b3, 0x68b4); alu_or(gb, E);
  CYC(0x68b4, 0x68b6); alu_and(gb, 0x0f);
  CYC(0x68b6, 0x68b8); alu_add(gb, 0xb0);
  dungeonMap_getTileForRoom__ret_hook(gb);
}

void dungeonMap_getTileForRoom__ret_hook(GB *gb) {
  CYC(0x68b8, 0x68b9); SET_DE(pop_effect(gb));
  CYC(0x68b9, 0x68ba); SET_BC(pop_effect(gb));
  CYC(0x68ba, 0x68bb); ret_effect(gb);
}

void dungeonMap_checkCanViewFloor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x68bb, checkLinkHasMap_hook, 0x653e, 0x68be);
  if (!(F & FZ)) {
    CYCT(0x68be, 0x68bf); ret_effect(gb);
    return;
  }
  CYC(0x68be, 0x68bf);
  CYC(0x68bf, 0x68c0); push_effect(gb, HL);
  CYC(0x68c0, 0x68c2); A = hram_rd(gb, 0x8d);
  CYC(0x68c2, 0x68c3); A = alu_dec8(gb, A);
  CYC(0x68c3, 0x68c6); SET_HL(0x00f8);
  CYC(0x68c6, 0x68c7); alu_add(gb, L);
  CYC(0x68c7, 0x68c8); L = A;
  CYC(0x68c8, 0x68cb); A = W8(wMapMenu_visitedFloors);
  CYC(0x68cb, 0x68cc); alu_and(gb, mem_rd(gb, HL));
  CYC(0x68cc, 0x68cd); SET_HL(pop_effect(gb));
  CYC(0x68cd, 0x68ce); ret_effect(gb);
}

void mapMenu_loadPopupData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6234, mapMenu_checkCursorRoomVisited_hook, 0x6636, 0x6237);
  if (F & FZ) {
    CYCT(0x6237, 0x6239);
    goto no_icon;
  }
  CYC(0x6237, 0x6239);
  CYC(0x6239, 0x623c); SET_HL(0x6c37);
  CYC(0x623c, 0x623f); A = mem_rd(gb, 0xcbb3);
  CYC(0x623f, 0x6240); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x6240, 0x6242);
  } else {
    CYC(0x6240, 0x6242);
    CYC(0x6242, 0x6245); SET_HL(0x6c94);
  }
  CYC(0x6245, 0x6248); A = mem_rd(gb, 0xcbb6);
  CYC(0x6248, 0x6249); C = A;
  for (;;) {
    CYC(0x6249, 0x624a); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x624a, 0x624c); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(0x624c, 0x624e);
      goto no_icon;
    }
    CYC(0x624c, 0x624e);
    CYC(0x624e, 0x624f); alu_cp(gb, C);
    CYC(0x624f, 0x6250); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!(F & FZ)) {
      CYCT(0x6250, 0x6252);
      continue;
    }
    CYC(0x6250, 0x6252);
    CYC(0x6252, 0x6254);
    break;
  }
  goto got_icon;

no_icon:
  CYC(0x6254, 0x6255); alu_xor(gb, A);

got_icon:
  CYC(0x6255, 0x6256); D = A;
  CYC(0x6256, 0x6258); A = alu_swap(gb, A);
  CALL_C(0x6258, getMinimapPopupType_hook, 0x6298, 0x625b);
  CYC(0x625b, 0x625e); mem_wr(gb, 0xcbbf, A);
  CYC(0x625e, 0x625f); A = D;
  CALL_C(0x625f, getMinimapPopupType_hook, 0x6298, 0x6262);
  CYC(0x6262, 0x6265); SET_HL(0xcbbe);
  CYC(0x6265, 0x6266); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6266, 0x6267); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6267, 0x6269);
  } else {
    CYC(0x6267, 0x6269);
    CYC(0x6269, 0x626a); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(0x626a, 0x626b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  }
  CYC(0x626b, 0x626c); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x626c, 0x626d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x626d, 0x626f);
  } else {
    CYC(0x626d, 0x626f);
    CYC(0x626f, 0x6270); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6270, 0x6271); mem_wr(gb, HL, A); SET_HL(HL - 1);
  }
  CYC(0x6271, 0x6274); SET_DE(0x8008);
  CYC(0x6274, 0x6277); SET_BC(0x2080);
  CYC(0x6277, 0x627a); A = mem_rd(gb, 0xcbb6);
  CYC(0x627a, 0x627b); alu_cp(gb, D);
  if (F & FC) {
    CYCT(0x627b, 0x627d);
  } else {
    CYC(0x627b, 0x627d);
    CYC(0x627d, 0x627f); B = 0x70;
  }
  CYC(0x627f, 0x6281); alu_and(gb, 0x0f);
  CYC(0x6281, 0x6282); alu_cp(gb, E);
  if (F & FC) {
    CYCT(0x6282, 0x6284);
  } else {
    CYC(0x6282, 0x6284);
    CYC(0x6284, 0x6286); C = 0x20;
  }
  CYC(0x6286, 0x6289); SET_HL(0xcbbb);
  CYC(0x6289, 0x628a); A = mem_rd(gb, HL);
  CYC(0x628a, 0x628b); mem_wr(gb, HL, B);
  CYC(0x628b, 0x628c); L = alu_inc8(gb, L);
  CYC(0x628c, 0x628d); alu_sub(gb, B);
  CYC(0x628d, 0x628e); B = A;
  CYC(0x628e, 0x628f); A = mem_rd(gb, HL);
  CYC(0x628f, 0x6290); mem_wr(gb, HL, C);
  CYC(0x6290, 0x6291); alu_sub(gb, C);
  CYC(0x6291, 0x6292); alu_or(gb, B);
  if (F & FZ) {
    CYCT(0x6292, 0x6293); ret_effect(gb);
    return;
  }
  CYC(0x6292, 0x6293);
  CYC(0x6293, 0x6295); L = 0xb9;
  CYC(0x6295, 0x6297); mem_wr(gb, HL, 0x00);
  CYC(0x6297, 0x6298); ret_effect(gb);
}

void getMinimapPopupType_hook(GB *gb) {
  CYC(0x6298, 0x629a); alu_and(gb, 0x0f);
  CYC(0x629a, 0x629b); E = A;
  CYC(0x629b, 0x629c); push_effect(gb, 0x629c);
  switch (function_caller_jump_table(gb)) {
    case 0x62bc: minimapPopupType_normal_hook(gb); return;
    case 0x62be: minimapPopupType_advanceShop_hook(gb); return;
    case 0x62c5: minimapPopupType_cave_hook(gb); return;
    case 0x62d2: minimapPopupType_gashaSpot_hook(gb); return;
    case 0x62de: minimapPopupType_portalSpot_hook(gb); return;
    case 0x62f4: minimapPopupType_seedTree_hook(gb); return;
    case 0x62fe: minimapPopupType_moblinsKeep_hook(gb); return;
    case 0x6306: minimapNoPopup_hook(gb); return;
    case 0x6308: minimapPopupType_shop_hook(gb); return;
    case 0x630b: minimapPopupType_vasuOrSyrup_hook(gb); return;
    case 0x6315: minimapPopupType_blackTower_hook(gb); return;
    case 0x631b: minimapPopupType_makuTree_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void minimapPopupType_normal_hook(GB *gb) {
  CYC(0x62bc, 0x62bd); A = E;
  CYC(0x62bd, 0x62be); ret_effect(gb);
}

void minimapPopupType_advanceShop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x62be, checkAdvanceShopVisited_hook, 0x6750, 0x62c1);
  if (F & FZ) {
    CYCT(0x62c1, 0x62c2); ret_effect(gb);
    return;
  }
  CYC(0x62c1, 0x62c2);
  CYC(0x62c2, 0x62c4); A = 0x0e;
  CYC(0x62c4, 0x62c5); ret_effect(gb);
}

void minimapPopupType_cave_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62c5, 0x62c8); A = mem_rd(gb, 0xcbb6);
  CALL_C(0x62c8, mapGetRoomText_hook, 0x61b7, 0x62cb);
  CYC(0x62cb, 0x62cd); A = 0x02;
  CYC(0x62cd, 0x62ce); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x62ce, 0x62d0);
    minimapNoPopup_hook(gb);
    return;
  }
  CYC(0x62ce, 0x62d0);
  CYC(0x62d0, 0x62d1); A = E;
  CYC(0x62d1, 0x62d2); ret_effect(gb);
}

void minimapPopupType_gashaSpot_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62d2, 0x62d5); A = mem_rd(gb, 0xcbb6);
  CALL_C(0x62d5, getIndexOfGashaSpotInRoom_hook, 0x36a8, 0x62d8);
  CYC(0x62d8, 0x62da); alu_bit(gb, 7, C);
  if (!(F & FZ)) {
    CYCT(0x62da, 0x62dc);
    minimapNoPopup_hook(gb);
    return;
  }
  CYC(0x62da, 0x62dc);
  CYC(0x62dc, 0x62dd); A = E;
  CYC(0x62dd, 0x62de); ret_effect(gb);
}

void minimapPopupType_portalSpot_hook(GB *gb) {
  CYC(0x62de, 0x62e1); SET_HL(0xc700);
  CYC(0x62e1, 0x62e4); A = mem_rd(gb, 0xcbb3);
  CYC(0x62e4, 0x62e5); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x62e5, 0x62e7);
  } else {
    CYC(0x62e5, 0x62e7);
    CYC(0x62e7, 0x62ea); SET_HL(0xc800);
  }
  CYC(0x62ea, 0x62ed); A = mem_rd(gb, 0xcbb6);
  CYC(0x62ed, 0x62ee); push_effect(gb, 0x62ee); add_a_to_hl(gb);
  CYC(0x62ee, 0x62f0); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x62f0, 0x62f2);
    minimapNoPopup_hook(gb);
    return;
  }
  CYC(0x62f0, 0x62f2);
  CYC(0x62f2, 0x62f3); A = E;
  CYC(0x62f3, 0x62f4); ret_effect(gb);
}

void minimapPopupType_seedTree_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62f4, 0x62f7); A = mem_rd(gb, 0xcbb6);
  CALL_C(0x62f7, getTreeWarpDataForRoom_hook, 0x66c6, 0x62fa);
  if (F & FC) {
    CYCT(0x62fa, 0x62fb); ret_effect(gb);
    return;
  }
  CYC(0x62fa, 0x62fb);
  CYC(0x62fb, 0x62fc); SET_HL(HL + 1);
  CYC(0x62fc, 0x62fd); A = mem_rd(gb, HL);
  CYC(0x62fd, 0x62fe); ret_effect(gb);
}

void minimapPopupType_moblinsKeep_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x62fe, checkMoblinsKeepDestroyed_hook, 0x674b, 0x6301);
  CYC(0x6301, 0x6303); A = 0x0f;
  if (F & FZ) {
    CYCT(0x6303, 0x6304); ret_effect(gb);
    return;
  }
  CYC(0x6303, 0x6304);
  CYC(0x6304, 0x6305); A = alu_inc8(gb, A);
  CYC(0x6305, 0x6306); ret_effect(gb);
}

void minimapNoPopup_hook(GB *gb) {
  CYC(0x6306, 0x6307); alu_xor(gb, A);
  CYC(0x6307, 0x6308); ret_effect(gb);
}

void minimapPopupType_shop_hook(GB *gb) {
  CYC(0x6308, 0x630a); A = 0x0e;
  CYC(0x630a, 0x630b); ret_effect(gb);
}

void minimapPopupType_vasuOrSyrup_hook(GB *gb) {
  CYC(0x630b, 0x630e); A = mem_rd(gb, 0xcbb6);
  CYC(0x630e, 0x6310); alu_cp(gb, 0x5d);
  CYC(0x6310, 0x6312); A = 0x0c;
  if (F & FZ) {
    CYCT(0x6312, 0x6313); ret_effect(gb);
    return;
  }
  CYC(0x6312, 0x6313);
  CYC(0x6313, 0x6314); A = alu_inc8(gb, A);
  CYC(0x6314, 0x6315); ret_effect(gb);
}

void minimapPopupType_blackTower_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6315, getBlackTowerProgress_hook, 0x36c0, 0x6318);
  CYC(0x6318, 0x631a); alu_add(gb, 0x11);
  CYC(0x631a, 0x631b); ret_effect(gb);
}

void minimapPopupType_makuTree_hook(GB *gb) {
  CYC(0x631b, 0x631e); A = mem_rd(gb, 0xcc34);
  CYC(0x631e, 0x631f); alu_rlca(gb);
  CYC(0x631f, 0x6321); A = 0x0b;
  if (F & FC) {
    CYCT(0x6321, 0x6322); ret_effect(gb);
    return;
  }
  CYC(0x6321, 0x6322);
  CYC(0x6322, 0x6325); SET_HL(0xc738);
  CYC(0x6325, 0x6327); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x6327, 0x6329); A = 0x04;
  if (F & FZ) {
    CYCT(0x6329, 0x632a); ret_effect(gb);
    return;
  }
  CYC(0x6329, 0x632a);
  CYC(0x632a, 0x632c); A = 0x07;
  CYC(0x632c, 0x632d); ret_effect(gb);
}

void maupMenu_drawPopup_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_ROM(0x632d, 0x635e);
  CYC(0x6330, 0x6333); SET_HL(0xcbbb);
  CYC(0x6333, 0x6334); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6334, 0x6335); C = mem_rd(gb, HL);
  CYC(0x6335, 0x6336); B = A;
  CYC(0x6336, 0x6339); A = mem_rd(gb, 0xcbbd);
  CYC(0x6339, 0x633b); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x633b, 0x633d);
  } else {
    CYC(0x633b, 0x633d);
    CYC(0x633d, 0x633e); push_effect(gb, BC);
    CYC(0x633e, 0x6341); A = mem_rd(gb, 0xcbc0);
    CYC(0x6341, 0x6343); alu_and(gb, 0x01);
    CYC(0x6343, 0x6346); SET_HL(0xcbbe);
    CYC(0x6346, 0x6347); push_effect(gb, 0x6347); add_a_to_hl(gb);
    CYC(0x6347, 0x6348); A = mem_rd(gb, HL);
    CYC(0x6348, 0x634b); SET_HL(0x6990);
    CYC(0x634b, 0x634c); push_effect(gb, 0x634c); add_a_to_hl(gb);
    CYC(0x634c, 0x634d); A = mem_rd(gb, HL);
    CYC(0x634d, 0x634e); push_effect(gb, 0x634e); add_a_to_hl(gb);
    CALL_C(0x634e, addSpritesToOam_withOffset_hook, 0x0d61, 0x6351);
    CYC(0x6351, 0x6352); SET_BC(pop_effect(gb));
  }
  CYC(0x6352, 0x6355); A = mem_rd(gb, 0xcbbd);
  CYC(0x6355, 0x6358); SET_HL(0x693a);
  CYC(0x6358, 0x6359); push_effect(gb, 0x6359); add_a_to_hl(gb);
  CYC(0x6359, 0x635a); A = mem_rd(gb, HL);
  CYC(0x635a, 0x635b); push_effect(gb, 0x635b); add_a_to_hl(gb);
  CYC(0x635b, 0x635e); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_checkDirectionButtons_hook(GB *gb) {
  CYC(0x63d2, 0x63d5); A = mem_rd(gb, 0xcbce);
  CYC(0x63d5, 0x63d6); push_effect(gb, 0x63d6);
  switch (function_caller_jump_table(gb)) {
    case 0x63da: dungeonMap_scrollingState0_hook(gb); return;
    case 0x6488: dungeonMap_scrollingState1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void dungeonMap_scrollingState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63da, getInputWithAutofire_hook, 0x0294, 0x63dd);
  CYC(0x63dd, 0x63df); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x63df, 0x63e1);
    goto check_up;
  }
  CYC(0x63df, 0x63e1);
  CALL_C(0x63e1, dungeonMap_checkCanScrollDown_hook, 0x641b, 0x63e4);
  if (!(F & FZ)) {
    CYCT(0x63e4, 0x63e6);
    goto move;
  }
  CYC(0x63e4, 0x63e6);
  CYC(0x63e6, 0x63e7); ret_effect(gb);
  return;

check_up:
  CYC(0x63e7, 0x63e9); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(0x63e9, 0x63ea); ret_effect(gb);
    return;
  }
  CYC(0x63e9, 0x63ea);
  CALL_C(0x63ea, dungeonMap_checkCanScrollUp_hook, 0x6454, 0x63ed);
  if (F & FZ) {
    CYCT(0x63ed, 0x63ee); ret_effect(gb);
    return;
  }
  CYC(0x63ed, 0x63ee);

move:
  CYC(0x63ee, 0x63ef); C = A;
  CYC(0x63ef, 0x63f0); A = B;
  CYC(0x63f0, 0x63f3); mem_wr(gb, 0xcbb5, A);
  CYC(0x63f3, 0x63f4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x63f4, 0x63f6);
    CYC(0x63ff, 0x6402); A = mem_rd(gb, 0xcbb7);
    CYC(0x6402, 0x6403); alu_sub(gb, C);
    CYC(0x6403, 0x6406); mem_wr(gb, 0xcbb7, A);
  } else {
    CYC(0x63f4, 0x63f6);
    CYC(0x63f6, 0x63f9); A = mem_rd(gb, 0xcbb7);
    CYC(0x63f9, 0x63fa); alu_add(gb, C);
    CYC(0x63fa, 0x63fd); mem_wr(gb, 0xcbb7, A);
    CYC(0x63fd, 0x63ff);
  }
  CYC(0x6406, 0x6407); A = C;
  CYC(0x6407, 0x6408); D = A;
  CALL_C(0x6408, multiplyABy8_hook, 0x01b7, 0x640b);
  CYC(0x640b, 0x640c); A = D;
  CYC(0x640c, 0x640d); alu_add(gb, A);
  CYC(0x640d, 0x640e); alu_add(gb, C);
  CYC(0x640e, 0x640f); A = alu_inc8(gb, A);
  CYC(0x640f, 0x6412); mem_wr(gb, 0xcbb4, A);
  CYC(0x6412, 0x6415); SET_HL(0xcbce);
  CYC(0x6415, 0x6416); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6416, 0x6418); A = 0x84;
  CYC(0x6418, 0x641b); playSound_b00_hook(gb);
}

void dungeonMap_checkCanScrollDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x641b, 0x641c); push_effect(gb, DE);
  CYC(0x641c, 0x641f); A = mem_rd(gb, 0xcc40);
  CYC(0x641f, 0x6420); A = alu_dec8(gb, A);
  CYC(0x6420, 0x6421); B = A;
  CYC(0x6421, 0x6424); A = mem_rd(gb, 0xcbb7);
  CYC(0x6424, 0x6425); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x6425, 0x6427);
    goto failure;
  }
  CYC(0x6425, 0x6427);
  CALL_C(0x6427, checkLinkHasMap_hook, 0x653e, 0x642a);
  CYC(0x642a, 0x642c); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x642c, 0x642e);
    goto done;
  }
  CYC(0x642c, 0x642e);
  CYC(0x642e, 0x6431); A = mem_rd(gb, 0xcbb7);
  CYC(0x6431, 0x6432); C = A;
  CYC(0x6432, 0x6435); A = mem_rd(gb, 0xcc40);
  CYC(0x6435, 0x6436); A = alu_dec8(gb, A);
  CYC(0x6436, 0x6437); alu_sub(gb, C);
  CYC(0x6437, 0x6438); C = A;
  CYC(0x6438, 0x6439); E = A;
  CYC(0x6439, 0x643b); D = 0;
  for (;;) {
    CYC(0x643b, 0x643c); D = alu_inc8(gb, D);
    CYC(0x643c, 0x643d); A = E;
    CYC(0x643d, 0x643e); alu_sub(gb, D);
    CYC(0x643e, 0x6441); SET_HL(0x00f8);
    CYC(0x6441, 0x6442); alu_add(gb, L);
    CYC(0x6442, 0x6443); L = A;
    CYC(0x6443, 0x6444); B = mem_rd(gb, HL);
    CYC(0x6444, 0x6447); A = mem_rd(gb, 0xcbba);
    CYC(0x6447, 0x6448); alu_and(gb, B);
    CYC(0x6448, 0x6449); A = D;
    if (!(F & FZ)) {
      CYCT(0x6449, 0x644b);
      goto done;
    }
    CYC(0x6449, 0x644b);
    CYC(0x644b, 0x644c); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(0x644c, 0x644e);
      continue;
    }
    CYC(0x644c, 0x644e);
    break;
  }

failure:
  CYC(0x644e, 0x644f); alu_xor(gb, A);

done:
  CYC(0x644f, 0x6451); B = 0x01;
  CYC(0x6451, 0x6452); alu_or(gb, A);
  CYC(0x6452, 0x6453); SET_DE(pop_effect(gb));
  CYC(0x6453, 0x6454); ret_effect(gb);
}

void dungeonMap_checkCanScrollUp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6454, 0x6455); push_effect(gb, DE);
  CYC(0x6455, 0x6458); A = mem_rd(gb, 0xcbb7);
  CYC(0x6458, 0x6459); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6459, 0x645b);
    goto failure;
  }
  CYC(0x6459, 0x645b);
  CALL_C(0x645b, checkLinkHasMap_hook, 0x653e, 0x645e);
  CYC(0x645e, 0x6460); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x6460, 0x6462);
    goto done;
  }
  CYC(0x6460, 0x6462);
  CYC(0x6462, 0x6465); A = mem_rd(gb, 0xcbb7);
  CYC(0x6465, 0x6466); E = A;
  CYC(0x6466, 0x6469); A = mem_rd(gb, 0xcc40);
  CYC(0x6469, 0x646a); A = alu_dec8(gb, A);
  CYC(0x646a, 0x646b); alu_sub(gb, E);
  CYC(0x646b, 0x646c); C = E;
  CYC(0x646c, 0x646d); E = A;
  CYC(0x646d, 0x646f); D = 0;
  for (;;) {
    CYC(0x646f, 0x6470); D = alu_inc8(gb, D);
    CYC(0x6470, 0x6471); A = E;
    CYC(0x6471, 0x6472); alu_add(gb, D);
    CYC(0x6472, 0x6475); SET_HL(0x00f8);
    CYC(0x6475, 0x6476); alu_add(gb, L);
    CYC(0x6476, 0x6477); L = A;
    CYC(0x6477, 0x6478); B = mem_rd(gb, HL);
    CYC(0x6478, 0x647b); A = mem_rd(gb, 0xcbba);
    CYC(0x647b, 0x647c); alu_and(gb, B);
    CYC(0x647c, 0x647d); A = D;
    if (!(F & FZ)) {
      CYCT(0x647d, 0x647f);
      goto done;
    }
    CYC(0x647d, 0x647f);
    CYC(0x647f, 0x6480); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(0x6480, 0x6482);
      continue;
    }
    CYC(0x6480, 0x6482);
    break;
  }

failure:
  CYC(0x6482, 0x6483); alu_xor(gb, A);

done:
  CYC(0x6483, 0x6485); B = 0x00;
  CYC(0x6485, 0x6486); alu_or(gb, A);
  CYC(0x6486, 0x6487); SET_DE(pop_effect(gb));
  CYC(0x6487, 0x6488); ret_effect(gb);
}

void dungeonMap_scrollingState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6488, 0x648b); SET_HL(0xcbb4);
  CYC(0x648b, 0x648c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYC(0x648c, 0x648e);
    CYC(0x648e, 0x648f); alu_xor(gb, A);
    CYC(0x648f, 0x6492); mem_wr(gb, 0xcbce, A);
    CYC(0x6492, 0x6493); ret_effect(gb);
    return;
  }
  CYCT(0x648c, 0x648e);
  CYC(0x6493, 0x6496); A = mem_rd(gb, 0xcbb5);
  CYC(0x6496, 0x6497); alu_or(gb, A);
  CYC(0x6497, 0x6499); A = 0xff;
  if (F & FZ) {
    CYCT(0x6499, 0x649b);
  } else {
    CYC(0x6499, 0x649b);
    CYC(0x649b, 0x649d); A = 0x01;
  }
  CYC(0x649d, 0x64a0); SET_HL(0xcbb8);
  CYC(0x64a0, 0x64a1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x64a1, 0x64a2); mem_wr(gb, HL, A);
  CALL_C(0x64a2, dungeonMap_updateScroll_hook, 0x682c, 0x64a5);
  mapMenu_copyTilemapToVram_hook(gb);
}

void mapMenu_copyTilemapToVram_hook(GB *gb) {
  CYC(0x64a5, 0x64a6); alu_xor(gb, A);
  CYC(0x64a6, 0x64a9); mem_wr(gb, 0xcbe9, A);
  CYC(0x64a9, 0x64ab); A = 0x0a;
  CYC(0x64ab, 0x64ae); loadUncompressedGfxHeader_hook(gb);
}

void mapMenu_drawSprites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x64ae, 0x64b1); A = mem_rd(gb, 0xcbb3);
  CYC(0x64b1, 0x64b3); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x64b3, 0x64b5);
    goto overworld;
  }
  CYC(0x64b3, 0x64b5);
  CALL_C(0x64b5, dungeonMap_drawItemSprites_hook, 0x64da, 0x64b8);
  CALL_C(0x64b8, dungeonMap_drawLinkIcons_hook, 0x657f, 0x64bb);
  CALL_C(0x64bb, dungeonMap_drawCursor_hook, 0x65d5, 0x64be);
  CALL_C(0x64be, dungeonMap_drawArrows_hook, 0x65fd, 0x64c1);
  CALL_C(0x64c1, dungeonMap_drawBossSymbolForFloor_hook, 0x6566, 0x64c4);
  CYC(0x64c4, 0x64c7); dungeonMap_drawFloorCursor_hook(gb);
  return;

overworld:
  CALL_C(0x64c7, maupMenu_drawPopup_hook, 0x632d, 0x64ca);
  CALL_C(0x64ca, mapMenu_drawArrow_hook, 0x664e, 0x64cd);
  CALL_C(0x64cd, mapMenu_drawCursor_hook, 0x6661, 0x64d0);
  CYC(0x64d0, 0x64d3); A = mem_rd(gb, 0xcbc1);
  CYC(0x64d3, 0x64d4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x64d4, 0x64d7);
    mapMenu_drawWarpSites_hook(gb);
    return;
  }
  CYC(0x64d4, 0x64d7);
  CYC(0x64d7, 0x64da); mapMenu_drawTimePortal_hook(gb);
}

void mapMenu_performTileSubstitutions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ef3, 0x5ef6); SET_HL(0x6a84);
  CYC(0x5ef6, 0x5ef7); push_effect(gb, 0x5ef7); add_a_to_hl(gb);
  CYC(0x5ef7, 0x5ef8); A = mem_rd(gb, HL);
  CYC(0x5ef8, 0x5ef9); push_effect(gb, 0x5ef9); add_a_to_hl(gb);
  for (;;) {
    CYC(0x5ef9, 0x5efa); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5efa, 0x5efb); alu_or(gb, A);
    if (F & FZ) { CYCT(0x5efb, 0x5efc); ret_effect(gb); return; }
    CYC(0x5efb, 0x5efc);
    CYC(0x5efc, 0x5efd); B = A;
    CYC(0x5efd, 0x5efe); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5efe, 0x5eff); E = A;
    CYC(0x5eff, 0x5f00); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5f00, 0x5f01); D = A;
    CYC(0x5f01, 0x5f02); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5f02, 0x5f03); C = A;
    CYC(0x5f03, 0x5f04); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5f04, 0x5f05); push_effect(gb, HL);
    CYC(0x5f05, 0x5f06); H = A;
    CYC(0x5f06, 0x5f07); L = C;
    CYC(0x5f07, 0x5f08); A = B;
    CYC(0x5f08, 0x5f0a); alu_and(gb, 0x0f);
    CYC(0x5f0a, 0x5f0b); C = A;
    CYC(0x5f0b, 0x5f0c); A = B;
    CYC(0x5f0c, 0x5f0e); alu_and(gb, 0xf0);
    CYC(0x5f0e, 0x5f10); A = alu_swap(gb, A);
    CYC(0x5f10, 0x5f11); B = A;
    for (;;) {
      CYC(0x5f11, 0x5f12); push_effect(gb, BC);
      for (;;) {
        CYC(0x5f12, 0x5f13); A = mem_rd(gb, HL);
        CYC(0x5f13, 0x5f14); mem_wr(gb, DE, A);
        CYC(0x5f14, 0x5f16); H |= 0x04;
        CYC(0x5f16, 0x5f18); D |= 0x04;
        CYC(0x5f18, 0x5f19); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x5f19, 0x5f1a); mem_wr(gb, DE, A);
        CYC(0x5f1a, 0x5f1b); SET_DE(DE + 1);
        CYC(0x5f1b, 0x5f1d); H &= (uint8_t)~0x04;
        CYC(0x5f1d, 0x5f1f); D &= (uint8_t)~0x04;
        CYC(0x5f1f, 0x5f20); C = alu_dec8(gb, C);
        if (C) { CYCT(0x5f20, 0x5f22); continue; }
        CYC(0x5f20, 0x5f22);
        break;
      }
      CYC(0x5f22, 0x5f23); SET_BC(pop_effect(gb));
      CYC(0x5f23, 0x5f25); A = 0x20;
      CYC(0x5f25, 0x5f26); alu_sub(gb, C);
      CYC(0x5f26, 0x5f28); H8(hFF8B) = A;
      CYC(0x5f28, 0x5f29); push_effect(gb, 0x5f29); add_a_to_hl(gb);
      CYC(0x5f29, 0x5f2b); A = H8(hFF8B);
      CALL_C(0x5f2b, addAToDe_hook, 0x0068, 0x5f2e);
      CYC(0x5f2e, 0x5f2f); B = alu_dec8(gb, B);
      if (B) { CYCT(0x5f2f, 0x5f31); continue; }
      CYC(0x5f2f, 0x5f31);
      break;
    }
    CYC(0x5f31, 0x5f32); SET_HL(pop_effect(gb));
    CYC(0x5f32, 0x5f34);
  }
}

void runGaleSeedMenu__runState_hook(GB *gb) {
  CYC(0x5f3d, 0x5f40); A = W8(wMenuActiveState);
  CYC(0x5f40, 0x5f41); push_effect(gb, 0x5f41);
  switch (function_caller_jump_table(gb)) {
    case 0x5f49: galeSeedMenu_state0_hook(gb); return;
    case 0x5f59: galeSeedMenu_state1_hook(gb); return;
    case 0x5f9d: galeSeedMenu_state2_hook(gb); return;
    case 0x5fd7: galeSeedMenu_state3_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runGaleSeedMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f34, clearOam_hook, 0x049f, 0x5f37);
  CALL_C(0x5f37, runGaleSeedMenu__runState_hook, 0x5f3d, 0x5f3a);
  CYC(0x5f3a, 0x5f3d);
  mapMenu_drawSprites_hook(gb);
}

void galeSeedMenu_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f49, mapMenu_state0_hook, 0x6014, 0x5f4c);
  CYC(0x5f4c, 0x5f4e); A = 0xff;
  CYC(0x5f4e, 0x5f51); W8(wMapMenu_warpIndex) = A;
  CYC(0x5f51, 0x5f53); A = 0x01;
  CYC(0x5f53, 0x5f56); W8(wMapMenu_drawWarpDestinations) = A;
  CYC(0x5f56, 0x5f59);
  galeSeedMenu_addOffsetToWarpIndex_hook(gb);
}

void galeSeedMenu_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f59, 0x5f5c); A = W8(wPaletteThread_mode);
  CYC(0x5f5c, 0x5f5d); alu_or(gb, A);
  if (F & FZ) CYC(0x5f5d, 0x5f5f);
  else { CYCT(0x5f5d, 0x5f5f); goto end; }
  CYC(0x5f5f, 0x5f62); A = W8(wKeysJustPressed);
  CYC(0x5f62, 0x5f64); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x5f64, 0x5f66); goto b_pressed; }
  CYC(0x5f64, 0x5f66);
  CYC(0x5f66, 0x5f68); alu_and(gb, 0x09);
  if (!(F & FZ)) { CYCT(0x5f68, 0x5f6a); goto a_pressed; }
  CYC(0x5f68, 0x5f6a);
  CYC(0x5f6a, 0x5f6d); SET_HL(0x5f99);
  CALL_C(0x5f6d, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x5f70);
  if (!(F & FC)) { CYCT(0x5f70, 0x5f72); goto end; }
  CYC(0x5f70, 0x5f72);
  CALL_C(0x5f72, galeSeedMenu_addOffsetToWarpIndex_hook, 0x5fe8, 0x5f75);
  CYC(0x5f75, 0x5f77); A = 0x84;
  if (!(F & FZ)) CALL_C_CC(0x5f77, playSound_b00_hook, 0x0c98, 0x5f7a);
  else CYC(0x5f77, 0x5f7a);
end:
  CYC(0x5f7a, 0x5f7d);
  mapMenu_loadPopupData_hook(gb);
  return;
b_pressed:
  CALL_C(0x5f7d, mapGetRoomTextOrReturn_hook, 0x619d, 0x5f80);
  CYC(0x5f80, 0x5f82); A = 0x03;
  CYC(0x5f82, 0x5f84); C = 0x01;
  CYC(0x5f84, 0x5f86);
  goto set_state;
a_pressed:
  CALL_C(0x5f86, mapGetRoomTextOrReturn_hook, 0x619d, 0x5f89);
  CYC(0x5f89, 0x5f8a); A = C;
  CYC(0x5f8a, 0x5f8d); mem_wr(gb, wTextSubstitutions + 2, A);
  CYC(0x5f8d, 0x5f8f); C = 0x00;
  CYC(0x5f8f, 0x5f91); A = 0x02;
set_state:
  CYC(0x5f91, 0x5f94); W8(wMenuActiveState) = A;
  CYC(0x5f94, 0x5f96); B = 0x03;
  CYC(0x5f96, 0x5f99);
  showText_hook(gb);
}

void galeSeedMenu_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f9d, retIfTextIsActive_hook, 0x1859, 0x5fa0);
  CYC(0x5fa0, 0x5fa3); A = W8(wSelectedTextOption);
  CYC(0x5fa3, 0x5fa4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5fa4, 0x5fa6);
    galeSeedMenu_gotoState1_hook(gb);
    return;
  }
  CYC(0x5fa4, 0x5fa6);
  CYC(0x5fa6, 0x5fa9); W8(wOpenedMenuType) = A;
  CYC(0x5fa9, 0x5fac); A = W8(wActiveGroup);
  CYC(0x5fac, 0x5fae); alu_or(gb, 0x80);
  CYC(0x5fae, 0x5fb1); W8(wWarpDestGroup) = A;
  CYC(0x5fb1, 0x5fb4); A = W8(wMapMenu_warpIndex);
  CALL_C(0x5fb4, getTreeWarpDataIndex_hook, 0x66be, 0x5fb7);
  CYC(0x5fb7, 0x5fb8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fb8, 0x5fbb); W8(wWarpDestRoom) = A;
  CYC(0x5fbb, 0x5fbc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fbc, 0x5fbf); W8(wWarpDestPos) = A;
  CYC(0x5fbf, 0x5fc1); A = 0x05;
  CYC(0x5fc1, 0x5fc4); W8(wWarpTransition) = A;
  CYC(0x5fc4, 0x5fc6); A = 0x03;
  CYC(0x5fc6, 0x5fc9); W8(wWarpTransition2) = A;
  CYC(0x5fc9, 0x5fcb); A = 0x03;
  CALL_C(0x5fcb, setMusicVolume_hook, 0x0cad, 0x5fce);
  CYC(0x5fce, 0x5fd1);
  fadeoutToWhite_hook(gb);
}

void galeSeedMenu_gotoState1_hook(GB *gb) {
  CYC(0x5fd1, 0x5fd3); A = 0x01;
  CYC(0x5fd3, 0x5fd6); W8(wMenuActiveState) = A;
  CYC(0x5fd6, 0x5fd7); ret_effect(gb);
}

void galeSeedMenu_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5fd7, retIfTextIsActive_hook, 0x1859, 0x5fda);
  CYC(0x5fda, 0x5fdd); A = W8(wSelectedTextOption);
  CYC(0x5fdd, 0x5fde); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5fde, 0x5fe0);
    galeSeedMenu_gotoState1_hook(gb);
    return;
  }
  CYC(0x5fde, 0x5fe0);
  CYC(0x5fe0, 0x5fe2); A = 0xff;
  CYC(0x5fe2, 0x5fe5); W8(wWarpTransition2) = A;
  CYC(0x5fe5, 0x5fe8);
  closeMenu_hook(gb);
}

void galeSeedMenu_addOffsetToWarpIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5fe8, 0x5fe9); E = A;
  CYC(0x5fe9, 0x5fec); A = W8(wMapMenu_warpIndex);
  CYC(0x5fec, 0x5fed); D = A;
  for (;;) {
    CYC(0x5fed, 0x5fee); A = D;
    CYC(0x5fee, 0x5fef); alu_add(gb, E);
    CYC(0x5fef, 0x5ff1); alu_and(gb, 0x07);
    CYC(0x5ff1, 0x5ff2); D = A;
    CALL_C(0x5ff2, getTreeWarpDataIndex_hook, 0x66be, 0x5ff5);
    CYC(0x5ff5, 0x5ff6); A = mem_rd(gb, HL);
    CYC(0x5ff6, 0x5ff7); alu_or(gb, A);
    if (F & FZ) { CYCT(0x5ff7, 0x5ff9); continue; }
    CYC(0x5ff7, 0x5ff9);
    CALL_C(0x5ff9, mapMenu_checkRoomVisited_hook, 0x6639, 0x5ffc);
    if (F & FZ) { CYCT(0x5ffc, 0x5ffe); continue; }
    CYC(0x5ffc, 0x5ffe);
    break;
  }
  CYC(0x5ffe, 0x5fff); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fff, 0x6002); W8(wMapMenu_cursorIndex) = A;
  CYC(0x6002, 0x6005); SET_HL(wMapMenu_warpIndex);
  CYC(0x6005, 0x6006); A = D;
  CYC(0x6006, 0x6007); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x6007, 0x6008); mem_wr(gb, HL, A);
  CYC(0x6008, 0x6009); ret_effect(gb);
}

void runMapMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6009, clearOam_hook, 0x049f, 0x600c);
  CYC(0x600c, 0x600f); A = W8(wMenuActiveState);
  CYC(0x600f, 0x6010); push_effect(gb, 0x6010);
  switch (function_caller_jump_table(gb)) {
    case 0x6014: mapMenu_state0_hook(gb); return;
    case 0x611d: mapMenu_state1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void mapMenu_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6014, 0x6016); A = 0x04;
  CYC(0x6016, 0x6018); mem_wr(gb, IO_SVBK, A);
  CALL_C(0x6018, loadMinimapDisplayRoom_hook, 0x60b5, 0x601b);
  CYC(0x601b, 0x601e); A = W8(wMapMenu_mode);
  CYC(0x601e, 0x6020); alu_add(gb, 0x0d);
  CALL_C(0x6020, loadGfxHeader_hook, 0x0626, 0x6023);
  CYC(0x6023, 0x6026); A = W8(wMapMenu_mode);
  CYC(0x6026, 0x6028); alu_add(gb, 0x07);
  CALL_C(0x6028, loadPaletteHeader_hook, 0x050b, 0x602b);
  CYC(0x602b, 0x602e); A = W8(wMapMenu_mode);
  CYC(0x602e, 0x6030); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x6030, 0x6032); goto dungeon; }
  CYC(0x6030, 0x6032);
  CYC(0x6032, 0x6033); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6033, 0x6035); goto past; }
  CYC(0x6033, 0x6035);
  CYC(0x6035, 0x6038); A = W8(wAnimalCompanion);
  CYC(0x6038, 0x603a); alu_sub(gb, 0x0c);
  if (!(F & FC)) CALL_C_CC(0x603a, mapMenu_performTileSubstitutions_hook, 0x5ef3, 0x603d);
  else CYC(0x603a, 0x603d);
  CYC(0x603d, 0x6040); A = mem_rd(gb, wGroup0RoomFlags + 0x13);
  CYC(0x6040, 0x6041); alu_rrca(gb);
  CYC(0x6041, 0x6043); A = 0x05;
  if (F & FC) CALL_C_CC(0x6043, mapMenu_performTileSubstitutions_hook, 0x5ef3, 0x6046);
  else CYC(0x6043, 0x6046);
past:
  CYC(0x6046, 0x6049); A = mem_rd(gb, wGroup1RoomFlags + 0x41);
  CYC(0x6049, 0x604a); alu_rrca(gb);
  CYC(0x604a, 0x604c); A = 0x06;
  if (F & FC) CALL_C_CC(0x604c, mapMenu_performTileSubstitutions_hook, 0x5ef3, 0x604f);
  else CYC(0x604c, 0x604f);
  CALL_C(0x604f, mapMenu_clearUnvisitedTiles_hook, 0x671c, 0x6052);
  CYC(0x6052, 0x6055); A = W8(wMapMenu_currentRoom);
  CYC(0x6055, 0x6058); W8(wMapMenu_cursorIndex) = A;
  CALL_C(0x6058, mapMenu_loadPopupData_hook, 0x6234, 0x605b);
  CYC(0x605b, 0x605d);
  goto common;
dungeon:
  CYC(0x605d, 0x6060); A = W8(wTilesetFlags);
  CYC(0x6060, 0x6062); alu_and(gb, 0x20);
  CYC(0x6062, 0x6065); A = W8(wMinimapDungeonFloor);
  if (!(F & FZ)) CYCT(0x6065, 0x6067);
  else {
    CYC(0x6065, 0x6067);
    CYC(0x6067, 0x606a); A = W8(wDungeonFloor);
  }
  CYC(0x606a, 0x606b); B = A;
  CYC(0x606b, 0x606e); A = W8(wDungeonNumFloors);
  CYC(0x606e, 0x606f); A = alu_dec8(gb, A);
  CYC(0x606f, 0x6070); alu_sub(gb, B);
  CYC(0x6070, 0x6073); W8(wMapMenu_floorIndex) = A;
  CALL_C(0x6073, multiplyABy8_hook, 0x01b7, 0x6076);
  CYC(0x6076, 0x6079); A = W8(wMapMenu_floorIndex);
  CYC(0x6079, 0x607a); alu_add(gb, A);
  CYC(0x607a, 0x607b); alu_add(gb, C);
  CYC(0x607b, 0x607e); W8(wMapMenu_dungeonScrollY) = A;
  CALL_C(0x607e, dungeonMap_calculateVisitedFloorsAndLinkPosition_hook, 0x60ea, 0x6081);
  CYC(0x6081, 0x6084); A = W8(wDungeonIndex);
  CYC(0x6084, 0x6086); alu_add(gb, 0x10);
  CALL_C(0x6086, loadGfxHeader_hook, 0x0626, 0x6089);
  CALL_C(0x6089, dungeonMap_drawSmallKeyCount_hook, 0x60dc, 0x608c);
  CALL_C(0x608c, dungeonMap_generateScrollableTilemap_hook, 0x67de, 0x608f);
  CALL_C(0x608f, dungeonMap_drawFloorList_hook, 0x677c, 0x6092);
  CALL_C(0x6092, dungeonMap_updateScroll_hook, 0x682c, 0x6095);
common:
  CYC(0x6095, 0x6096); alu_xor(gb, A);
  CYC(0x6096, 0x6098); mem_wr(gb, IO_SVBK, A);
  CALL_C(0x6098, mapMenu_drawSprites_hook, 0x64ae, 0x609b);
  CYC(0x609b, 0x609c); alu_xor(gb, A);
  CYC(0x609c, 0x609e); H8(hCameraX) = A;
  CYC(0x609e, 0x60a0); H8(hCameraY) = A;
  CYC(0x60a0, 0x60a3); W8(wScreenOffsetX) = A;
  CYC(0x60a3, 0x60a6); W8(wScreenOffsetY) = A;
  CYC(0x60a6, 0x60a9); SET_HL(wMenuActiveState);
  CYC(0x60a9, 0x60aa); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x60aa, mapMenu_copyTilemapToVram_hook, 0x64a5, 0x60ad);
  CALL_C(0x60ad, fastFadeinFromWhite_hook, 0x3290, 0x60b0);
  CYC(0x60b0, 0x60b2); A = 0x07;
  CYC(0x60b2, 0x60b5);
  loadGfxRegisterStateIndex_hook(gb);
}

void loadMinimapDisplayRoom_hook(GB *gb) {
  CYC(0x60b5, 0x60b8); SET_HL(wMinimapGroup);
  CYC(0x60b8, 0x60b9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60b9, 0x60ba); C = mem_rd(gb, HL);
  CYC(0x60ba, 0x60bb); B = A;
  CYC(0x60bb, 0x60bd); B = 0x02;
  CYC(0x60bd, 0x60c0); A = W8(wTilesetFlags);
  CYC(0x60c0, 0x60c2); alu_bit(gb, 4, A);
  if (!(F & FZ)) { CYCT(0x60c2, 0x60c4); goto overworld; }
  CYC(0x60c2, 0x60c4);
  CYC(0x60c4, 0x60c6); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(0x60c6, 0x60c8); goto set_room; }
  CYC(0x60c6, 0x60c8);
overworld:
  CYC(0x60c8, 0x60c9); B = A;
  CYC(0x60c9, 0x60ca); alu_rlca(gb);
  CYC(0x60ca, 0x60cc); alu_and(gb, 0x01);
  CYC(0x60cc, 0x60ce); alu_bit(gb, 1, B);
  CYC(0x60ce, 0x60cf); B = A;
  if (F & FZ) { CYCT(0x60cf, 0x60d1); goto set_room; }
  CYC(0x60cf, 0x60d1);
  CYC(0x60d1, 0x60d3); C = 0x38;
set_room:
  CYC(0x60d3, 0x60d4); A = C;
  CYC(0x60d4, 0x60d7); W8(wMapMenu_currentRoom) = A;
  CYC(0x60d7, 0x60d8); A = B;
  CYC(0x60d8, 0x60db); W8(wMapMenu_mode) = A;
  CYC(0x60db, 0x60dc); ret_effect(gb);
}

void dungeonMap_drawSmallKeyCount_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x60dc, getNumSmallKeys_hook, 0x651f, 0x60df);
  if (F & FZ) { CYCT(0x60df, 0x60e0); ret_effect(gb); return; }
  CYC(0x60df, 0x60e0);
  CYC(0x60e0, 0x60e3); SET_HL(w4TileMap + 0x226);
  CYC(0x60e3, 0x60e5); alu_add(gb, 0x90);
  CYC(0x60e5, 0x60e6); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x60e6, 0x60e8); A = 0x9a;
  CYC(0x60e8, 0x60e9); mem_wr(gb, HL, A);
  CYC(0x60e9, 0x60ea); ret_effect(gb);
}

void dungeonMap_calculateVisitedFloorsAndLinkPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60ea, 0x60ed); A = W8(wDungeonIndex);
  CYC(0x60ed, 0x60f0); SET_HL(wDungeonVisitedFloors);
  CYC(0x60f0, 0x60f1); push_effect(gb, 0x60f1); add_a_to_hl(gb);
  CYC(0x60f1, 0x60f2); B = mem_rd(gb, HL);
  CALL_C(0x60f2, checkLinkHasCompass_hook, 0x6532, 0x60f5);
  CYC(0x60f5, 0x60f6); A = B;
  if (F & FZ) CYCT(0x60f6, 0x60f8);
  else {
    CYC(0x60f6, 0x60f8);
    CYC(0x60f8, 0x60fb); A = mem_rd(gb, wMapFloorsUnlockedWithCompass);
    CYC(0x60fb, 0x60fc); alu_or(gb, B);
  }
  CYC(0x60fc, 0x60ff); W8(wMapMenu_visitedFloors) = A;
  CYC(0x60ff, 0x6102); A = W8(wMinimapDungeonMapPosition);
  CYC(0x6102, 0x6105); W8(wMapMenu_dungeonCursorIndex) = A;
  CYC(0x6105, 0x6108); A = W8(wMinimapDungeonFloor);
  CYC(0x6108, 0x610b); W8(wMapMenu_linkFloor) = A;
  CYC(0x610b, 0x610e); A = W8(wActiveGroup);
  CYC(0x610e, 0x6110); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(0x6110, 0x6111); ret_effect(gb); return; }
  CYC(0x6110, 0x6111);
  CYC(0x6111, 0x6114); A = W8(wActiveRoom);
  CYC(0x6114, 0x6116); alu_cp(gb, 0xf5);
  if (!(F & FZ)) { CYCT(0x6116, 0x6117); ret_effect(gb); return; }
  CYC(0x6116, 0x6117);
  CYC(0x6117, 0x6119); A = 0x13;
  CYC(0x6119, 0x611c); W8(wMapMenu_dungeonCursorIndex) = A;
  CYC(0x611c, 0x611d); ret_effect(gb);
}

void mapMenu_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x611d, 0x6120); A = W8(wPaletteThread_mode);
  CYC(0x6120, 0x6121); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(0x6121, mapMenu_state1__checkInput_hook, 0x6127, 0x6124);
  else CYC(0x6121, 0x6124);
  CYC(0x6124, 0x6127);
  mapMenu_drawSprites_hook(gb);
}

void mapMenu_state1__checkInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6127, 0x612a); A = W8(wMapMenu_mode);
  CYC(0x612a, 0x612c); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x612c, 0x612e); goto overworld; }
  CYC(0x612c, 0x612e);
  CYC(0x612e, 0x6131); A = W8(wKeysJustPressed);
  CYC(0x6131, 0x6133); alu_and(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x6133, 0x6136);
    closeMenu_hook(gb);
    return;
  }
  CYC(0x6133, 0x6136);
  CALL_C(0x6136, dungeonMap_updateCursorFlickerCounter_hook, 0x65c7, 0x6139);
  CYC(0x6139, 0x613c);
  dungeonMap_checkDirectionButtons_hook(gb);
  return;
overworld:
  CYC(0x613c, 0x613f); A = W8(wMapMenu_varcbb4);
  CYC(0x613f, 0x6140); alu_or(gb, A);
  if (F & FZ) CYCT(0x6140, 0x6142);
  else {
    CYC(0x6140, 0x6142);
    CYC(0x6142, 0x6143); A = alu_dec8(gb, A);
    CYC(0x6143, 0x6146); W8(wMapMenu_varcbb4) = A;
  }
  CALL_C(0x6146, retIfTextIsActive_hook, 0x1859, 0x6149);
  CYC(0x6149, 0x614c); SET_HL(0x6199);
  CALL_C(0x614c, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x614f);
  if (!(F & FC)) { CYCT(0x614f, 0x6151); goto no_direction; }
  CYC(0x614f, 0x6151);
  CYC(0x6151, 0x6152); C = A;
  CYC(0x6152, 0x6155); D = 0xe0; E = 0x0e;
  CYC(0x6155, 0x6158); A = W8(wMapMenu_cursorIndex);
  CYC(0x6158, 0x6159); L = A;
  CYC(0x6159, 0x615b); alu_and(gb, 0xf0);
  CYC(0x615b, 0x615c); H = A;
  CYC(0x615c, 0x615d); A = L;
  CYC(0x615d, 0x615e); alu_xor(gb, H);
  CYC(0x615e, 0x615f); L = A;
  CYC(0x615f, 0x6161); C = alu_sra(gb, C);
  if (F & FC) { CYCT(0x6161, 0x6163); goto vertical; }
  CYC(0x6161, 0x6163);
  CYC(0x6163, 0x6164); A = L;
  for (;;) {
    CYC(0x6164, 0x6165); alu_add(gb, C);
    CYC(0x6165, 0x6167); alu_and(gb, 0x0f);
    CYC(0x6167, 0x6168); alu_cp(gb, E);
    if (!(F & FC)) { CYCT(0x6168, 0x616a); continue; }
    CYC(0x6168, 0x616a);
    break;
  }
  CYC(0x616a, 0x616b); L = A;
  CYC(0x616b, 0x616d);
  goto set_cursor;
vertical:
  CYC(0x616d, 0x616e); A = H;
  for (;;) {
    CYC(0x616e, 0x616f); alu_add(gb, C);
    CYC(0x616f, 0x6171); alu_and(gb, 0xf0);
    CYC(0x6171, 0x6172); alu_cp(gb, D);
    if (!(F & FC)) { CYCT(0x6172, 0x6174); continue; }
    CYC(0x6172, 0x6174);
    break;
  }
  CYC(0x6174, 0x6175); H = A;
set_cursor:
  CYC(0x6175, 0x6176); A = H;
  CYC(0x6176, 0x6177); alu_or(gb, L);
  CYC(0x6177, 0x617a); W8(wMapMenu_cursorIndex) = A;
  CYC(0x617a, 0x617c); A = 0x84;
  CALL_C(0x617c, playSound_b00_hook, 0x0c98, 0x617f);
  CYC(0x617f, 0x6182);
  mapMenu_loadPopupData_hook(gb);
  return;
no_direction:
  CYC(0x6182, 0x6185); A = W8(wKeysJustPressed);
  CYC(0x6185, 0x6187); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6187, 0x6189); goto show_room_text; }
  CYC(0x6187, 0x6189);
  CYC(0x6189, 0x618b); alu_and(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x618b, 0x618e);
    closeMenu_hook(gb);
    return;
  }
  CYC(0x618b, 0x618e);
  CYC(0x618e, 0x618f); ret_effect(gb);
  return;
show_room_text:
  CALL_C(0x618f, mapGetRoomTextOrReturn_hook, 0x619d, 0x6192);
  CYC(0x6192, 0x6195); SET_HL(wSubmenuState);
  CYC(0x6195, 0x6196); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6196, 0x6199);
  showText_hook(gb);
}

void mapGetRoomTextOrReturn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x619d, mapMenu_checkCursorRoomVisited_hook, 0x6636, 0x61a0);
  if (!(F & FZ)) CYCT(0x61a0, 0x61a2);
  else {
    CYC(0x61a0, 0x61a2);
    CYC(0x61a2, 0x61a3); SET_AF(pop_effect(gb));
    CYC(0x61a3, 0x61a4); ret_effect(gb);
    return;
  }
  CYC(0x61a4, 0x61a6); C = 0x80;
  CYC(0x61a6, 0x61a9); A = W8(wMapMenu_cursorIndex);
  CYC(0x61a9, 0x61aa); alu_cp(gb, C);
  CYC(0x61aa, 0x61ac); A = 0x03;
  if (F & FC) CYCT(0x61ac, 0x61ae);
  else {
    CYC(0x61ac, 0x61ae);
    CYC(0x61ae, 0x61af); alu_xor(gb, A);
  }
  CYC(0x61af, 0x61b2); W8(wTextboxPosition) = A;
  CYC(0x61b2, 0x61b4); A = 0x09;
  CYC(0x61b4, 0x61b7); W8(wTextboxFlags) = A;
  mapGetRoomText_hook(gb);
}

void mapGetRoomText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x61b7, mapGetRoomIndexWithoutUnusedColumns_hook, 0x6621, 0x61ba);
  CYC(0x61ba, 0x61bd); SET_HL(0x6aaf);
  if (!(F & FC)) CYCT(0x61bd, 0x61bf);
  else {
    CYC(0x61bd, 0x61bf);
    CYC(0x61bf, 0x61c2); SET_HL(0x6b73);
  }
  CYC(0x61c2, 0x61c4); B = 0x03;
  CYC(0x61c4, 0x61c5); push_effect(gb, 0x61c5); add_a_to_hl(gb);
  CYC(0x61c5, 0x61c6); C = mem_rd(gb, HL);
  CYC(0x61c6, 0x61c8); alu_bit(gb, 7, C);
  if (F & FZ) { CYCT(0x61c8, 0x61c9); ret_effect(gb); return; }
  CYC(0x61c8, 0x61c9);
  CYC(0x61c9, 0x61ca); A = C;
  CYC(0x61ca, 0x61cc); alu_and(gb, 0x07);
  CYC(0x61cc, 0x61cd); push_effect(gb, 0x61cd);
  switch (function_caller_jump_table(gb)) {
    case 0x61d7: mapGetRoomText__specialCode0_hook(gb); return;
    case 0x61f4: mapGetRoomText__specialCode1_hook(gb); return;
    case 0x6208: mapGetRoomText__specialCode2_hook(gb); return;
    case 0x6210: mapGetRoomText__specialCode3_hook(gb); return;
    case 0x6219: mapGetRoomText__specialCode4_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void mapGetRoomText__specialCode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61d7, 0x61d8); push_effect(gb, DE);
  CYC(0x61d8, 0x61db); A = W8(wTilesetFlags);
  CYC(0x61db, 0x61dc); alu_rlca(gb);
  CYC(0x61dc, 0x61df); SET_DE(wMakuMapTextPresent);
  CYC(0x61df, 0x61e1); C = 0x23;
  CYC(0x61e1, 0x61e3); A = 0x3e;
  if (!(F & FC)) CYCT(0x61e3, 0x61e5);
  else {
    CYC(0x61e3, 0x61e5);
    CYC(0x61e5, 0x61e6); E = alu_inc8(gb, E);
    CYC(0x61e6, 0x61e7); C = alu_inc8(gb, C);
    CYC(0x61e7, 0x61e8); A = alu_inc8(gb, A);
  }
  CALL_C(0x61e8, checkGlobalFlag_hook, 0x31f3, 0x61eb);
  CYC(0x61eb, 0x61ec); L = E;
  CYC(0x61ec, 0x61ed); H = D;
  CYC(0x61ed, 0x61ee); SET_DE(pop_effect(gb));
  if (F & FZ) { CYCT(0x61ee, 0x61ef); ret_effect(gb); return; }
  CYC(0x61ee, 0x61ef);
  CYC(0x61ef, 0x61f0); A = mem_rd(gb, HL);
  CYC(0x61f0, 0x61f1); C = A;
  CYC(0x61f1, 0x61f3); B = 0x05;
  CYC(0x61f3, 0x61f4); ret_effect(gb);
}

void mapGetRoomText__specialCode1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61f4, 0x61f5); A = C;
  CYC(0x61f5, 0x61f6); alu_add(gb, A);
  CYC(0x61f6, 0x61f8); A = alu_swap(gb, A);
  CYC(0x61f8, 0x61fa); alu_and(gb, 0x0f);
  CYC(0x61fa, 0x61fb); C = A;
  CALL_C(0x61fb, mapGetRoomText__checkDungeonEntered_hook, 0x6221, 0x61fe);
  if (!(F & FZ)) {
    CYCT(0x61fe, 0x6200);
    CYC(0x6205, 0x6207); B = 0x02;
    CYC(0x6207, 0x6208); ret_effect(gb);
    return;
  }
  CYC(0x61fe, 0x6200);
  CYC(0x6200, 0x6201); A = mem_rd(gb, HL);
  CYC(0x6201, 0x6203); alu_and(gb, 0x7f);
  CYC(0x6203, 0x6204); C = A;
  CYC(0x6204, 0x6205); ret_effect(gb);
}

void mapGetRoomText__specialCode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6208, checkMoblinsKeepDestroyed_hook, 0x674b, 0x620b);
  CYC(0x620b, 0x620d); C = 0x17;
  if (!(F & FZ)) { CYCT(0x620d, 0x620e); ret_effect(gb); return; }
  CYC(0x620d, 0x620e);
  CYC(0x620e, 0x620f); C = alu_inc8(gb, C);
  CYC(0x620f, 0x6210); ret_effect(gb);
}

void mapGetRoomText__specialCode3_hook(GB *gb) {
  CYC(0x6210, 0x6213); A = W8(wAnimalCompanion);
  CYC(0x6213, 0x6215); alu_sub(gb, 0x0b);
  CYC(0x6215, 0x6217); alu_add(gb, 0x2d);
  CYC(0x6217, 0x6218); C = A;
  CYC(0x6218, 0x6219); ret_effect(gb);
}

void mapGetRoomText__specialCode4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6219, checkAdvanceShopVisited_hook, 0x6750, 0x621c);
  CYC(0x621c, 0x621e); C = 0x26;
  if (F & FZ) { CYCT(0x621e, 0x621f); ret_effect(gb); return; }
  CYC(0x621e, 0x621f);
  CYC(0x621f, 0x6220); C = alu_dec8(gb, C);
  CYC(0x6220, 0x6221); ret_effect(gb);
}

void mapGetRoomText__checkDungeonEntered_hook(GB *gb) {
  CYC(0x6221, 0x6222); push_effect(gb, DE);
  CYC(0x6222, 0x6225); SET_HL(0x6ce3);
  CYC(0x6225, 0x6226); add_double_index_to_hl(gb, 0x6226);
  CYC(0x6226, 0x6227); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6227, 0x6228); E = A;
  CYC(0x6228, 0x622a); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x622a, 0x622c); D = 0xc9;
  if (!(F & FZ)) CYCT(0x622c, 0x622e);
  else {
    CYC(0x622c, 0x622e);
    CYC(0x622e, 0x622f); D = alu_inc8(gb, D);
  }
  CYC(0x622f, 0x6230); A = mem_rd(gb, DE);
  CYC(0x6230, 0x6232); alu_bit(gb, 4, A);
  CYC(0x6232, 0x6233); SET_DE(pop_effect(gb));
  CYC(0x6233, 0x6234); ret_effect(gb);
}

void getFileDisplayVariableAddress_hook(GB *gb) {
  CYC(0x417a, 0x417b); E = A;
  getFileDisplayVariableAddress_paramE_hook(gb);
}

void getFileDisplayVariableAddress_paramE_hook(GB *gb) {
  CYC(0x417b, 0x417c); A = E;
  CYC(0x417c, 0x417e); A = alu_swap(gb, A);
  CYC(0x417e, 0x417f); alu_rrca(gb);
  CYC(0x417f, 0x4180); alu_add(gb, D);
  CYC(0x4180, 0x4183); SET_HL(0xd780);
  CYC(0x4183, 0x4184); push_effect(gb, 0x4184); add_a_to_hl(gb);
  CYC(0x4184, 0x4185); ret_effect(gb);
}

void fileSelectUpdateInput_hook(GB *gb) {
  CYC(0x4467, 0x446a); A = mem_rd(gb, 0xc482);
  CYC(0x446a, 0x446b); C = A;
  CYC(0x446b, 0x446e); SET_HL(0xcbbc);
  CYC(0x446e, 0x4470); A = 0xff;
  CYC(0x4470, 0x4472); alu_bit(gb, 6, C);
  if (!(F & FZ)) { CYCT(0x4472, 0x4474); fileSelectUpdateInput__upOrDown_hook(gb); return; }
  CYC(0x4472, 0x4474);
  CYC(0x4474, 0x4476); A = 0x01;
  CYC(0x4476, 0x4478); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(0x4478, 0x447a); fileSelectUpdateInput__upOrDown_hook(gb); return; }
  CYC(0x4478, 0x447a);
  CYC(0x447a, 0x447b); A = C;
  CYC(0x447b, 0x447d); alu_and(gb, 0x09);
  CYC(0x447d, 0x447e); B = A;
  CYC(0x447e, 0x447f); ret_effect(gb);
}

void fileSelectUpdateInput__upOrDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x447f, 0x4480); B = A;
  CYC(0x4480, 0x4481); push_effect(gb, BC);
  CYC(0x4481, 0x4482); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4482, 0x4484); alu_and(gb, 0x03);
  CALL_C(0x4484, fileSelectSetCursor_hook, 0x44aa, 0x4487);
  CALL_C(0x4487, fileSelectDrawHeartsAndDeathCounter_hook, 0x4a4a, 0x448a);
  CYC(0x448a, 0x448b); SET_BC(pop_effect(gb));
  CYC(0x448b, 0x448c); alu_xor(gb, A);
  CYC(0x448c, 0x448d); ret_effect(gb);
}

void func_02_448d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x448d, 0x4490); A = mem_rd(gb, 0xc482);
  CYC(0x4490, 0x4491); C = A;
  CYC(0x4491, 0x4494); SET_HL(0xcbbd);
  CYC(0x4494, 0x4496); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4496, 0x4497); alu_xor(gb, A);
  CYC(0x4497, 0x4499); alu_bit(gb, 5, C);
  if (!(F & FZ)) { CYCT(0x4499, 0x449b); goto moved; }
  CYC(0x4499, 0x449b);
  CYC(0x449b, 0x449c); A = alu_inc8(gb, A);
  CYC(0x449c, 0x449e); alu_bit(gb, 4, C);
  if (!(F & FZ)) { CYCT(0x449e, 0x44a0); goto moved; }
  CYC(0x449e, 0x44a0);
  CYC(0x44a0, 0x44a1); A = C;
  CYC(0x44a1, 0x44a3); alu_and(gb, 0x09);
  CYC(0x44a3, 0x44a4); ret_effect(gb);
  return;
moved:
  CYC(0x44a4, 0x44a5); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) CALL_C_CC(0x44a5, fileSelectSetCursor_hook, 0x44aa, 0x44a8);
  else CYC(0x44a5, 0x44a8);
  CYC(0x44a8, 0x44a9); alu_xor(gb, A);
  CYC(0x44a9, 0x44aa); ret_effect(gb);
}

void fileSelectSetCursor_hook(GB *gb) {
  CYC(0x44aa, 0x44ab); mem_wr(gb, HL, A);
  CYC(0x44ab, 0x44ad); A = 0x84;
  CYC(0x44ad, 0x44b0); playSound_b00_hook(gb);
}

void fileSelectMode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44b0, fileSelectMode2__func_hook, 0x44b6, 0x44b3);
  CYC(0x44b3, 0x44b6); drawNameInputCursors_hook(gb);
}

void fileSelectMode2__func_hook(GB *gb) {
  CYC(0x44b6, 0x44b9); A = W8(wFileSelect_mode2);
  CYC(0x44b9, 0x44ba); push_effect(gb, 0x44ba);
  switch (function_caller_jump_table(gb)) {
    case 0x44c0: fileSelectMode2__mode0_hook(gb); return;
    case 0x44ca: fileSelectMode2__mode2_hook(gb); return;
    case 0x46d6: hook_handoff(gb, HL); return;
    default: hook_handoff(gb, HL); return;
  }
}

void fileSelectMode2__mode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44c0, eraseFile_b00_hook, 0x09e0, 0x44c3);
  CALL_C(0x44c3, loadFile_b00_hook, 0x09dc, 0x44c6);
  CYC(0x44c6, 0x44c7); alu_xor(gb, A);
  CYC(0x44c7, 0x44ca); copyNameToW4NameBuffer_hook(gb);
}

void fileSelectMode2__mode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44ca, getNameBufferLength_hook, 0x4626, 0x44cd);
  if (F & FZ) CYCT(0x44cd, 0x44cf);
  else {
    CYC(0x44cd, 0x44cf);
    CYC(0x44cf, 0x44d2); SET_HL(w4NameBuffer);
    CYC(0x44d2, 0x44d5); SET_DE(wLinkName);
    CYC(0x44d5, 0x44d7); B = 0x06;
    CALL_C(0x44d7, copyMemory_hook, 0x0486, 0x44da);
    CALL_C(0x44da, initializeFile_b00_hook, 0x09d4, 0x44dd);
  }
  CYC(0x44dd, 0x44e0); setFileSelectModeTo1_hook(gb);
}

void runKidNameEntryMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44e0, fileSelect_redrawDecorationsAndSetWramBank4_hook, 0x4cd7, 0x44e3);
  CALL_C(0x44e3, runKidNameEntryMenu__func_hook, 0x44e9, 0x44e6);
  CYC(0x44e6, 0x44e9); drawNameInputCursors_hook(gb);
}

void runKidNameEntryMenu__func_hook(GB *gb) {
  CYC(0x44e9, 0x44ec); A = W8(wFileSelect_mode2);
  CYC(0x44ec, 0x44ed); push_effect(gb, 0x44ed);
  switch (function_caller_jump_table(gb)) {
    case 0x44f3: runKidNameEntryMenu__mode0_hook(gb); return;
    case 0x4500: runKidNameEntryMenu__mode1_hook(gb); return;
    case 0x4508: runKidNameEntryMenu__mode2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runKidNameEntryMenu__mode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44f3, 0x44f5); A = 0xa0;
  CALL_C(0x44f5, loadGfxHeader_hook, 0x0626, 0x44f8);
  CYC(0x44f8, 0x44fa); A = 0x01;
  CALL_C(0x44fa, copyNameToW4NameBuffer_hook, 0x4641, 0x44fd);
  CYC(0x44fd, 0x4500); fadeinFromWhite_hook(gb);
}

void runKidNameEntryMenu__mode1_hook(GB *gb) {
  CYC(0x4500, 0x4503); A = W8(wPaletteThread_mode);
  CYC(0x4503, 0x4504); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4504, 0x4505); ret_effect(gb); return; }
  CYC(0x4504, 0x4505);
  CYC(0x4505, 0x4508); hook_handoff(gb, 0x46d6);
}

void runKidNameEntryMenu__mode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4508, getNameBufferLength_hook, 0x4626, 0x450b);
  CYC(0x450b, 0x450d); A = 0x01;
  if (F & FZ) CYCT(0x450d, 0x450f);
  else {
    CYC(0x450d, 0x450f);
    CYC(0x450f, 0x4512); SET_HL(w4NameBuffer);
    CYC(0x4512, 0x4515); SET_DE(wKidName);
    CYC(0x4515, 0x4517); B = 0x06;
    CALL_C(0x4517, copyMemory_hook, 0x0486, 0x451a);
    CYC(0x451a, 0x451c); A = 0x56;
    CALL_C(0x451c, playSound_b00_hook, 0x0c98, 0x451f);
    CYC(0x451f, 0x4520); alu_xor(gb, A);
  }
  CYC(0x4520, 0x4523); W8(wTextInputResult) = A;
  CYC(0x4523, 0x4526); closeMenu_hook(gb);
}

void runSecretEntryMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4571, fileSelect_redrawDecorationsAndSetWramBank4_hook, 0x4cd7, 0x4574);
  CALL_C(0x4574, runSecretEntryMenu__func_hook, 0x457a, 0x4577);
  CYC(0x4577, 0x457a); drawSecretInputCursors_hook(gb);
}

void runSecretEntryMenu__func_hook(GB *gb) {
  CYC(0x457a, 0x457d); A = W8(wFileSelect_mode2);
  CYC(0x457d, 0x457e); push_effect(gb, 0x457e);
  switch (function_caller_jump_table(gb)) {
    case 0x4588: runSecretEntryMenu__mode0_hook(gb); return;
    case 0x4593: runSecretEntryMenu__mode1_hook(gb); return;
    case 0x459b: runSecretEntryMenu__mode2_hook(gb); return;
    case 0x4fba: closeMenu_hook(gb); return;
    case 0x4609: hook_handoff(gb, HL); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runSecretEntryMenu__mode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4588, 0x458a); A = 0xa0;
  CALL_C(0x458a, loadGfxHeader_hook, 0x0626, 0x458d);
  CALL_C(0x458d, func_02_465c_hook, 0x465c, 0x4590);
  CYC(0x4590, 0x4593); fadeinFromWhite_hook(gb);
}

void runSecretEntryMenu__mode1_hook(GB *gb) {
  CYC(0x4593, 0x4596); A = W8(wPaletteThread_mode);
  CYC(0x4596, 0x4597); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4597, 0x4598); ret_effect(gb); return; }
  CYC(0x4597, 0x4598);
  CYC(0x4598, 0x459b); hook_handoff(gb, 0x46d6);
}

void runSecretEntryMenu__mode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x459b, 0x459e); SET_HL(w4SecretBuffer);
  CYC(0x459e, 0x45a1); SET_DE(wTmpcec0);
  CYC(0x45a1, 0x45a3); B = 0x20;
  CALL_C(0x45a3, copyMemory_hook, 0x0486, 0x45a6);
  CYC(0x45a6, 0x45a9); SET_BC(0x0103);
  CYC(0x45a9, 0x45ac); A = W8(wSecretInputType);
  CYC(0x45ac, 0x45ad); alu_rlca(gb);
  if (F & FC) { CYCT(0x45ad, 0x45af); goto unpack_secret; }
  CYC(0x45ad, 0x45af);
  CYC(0x45af, 0x45b1); C = 0x02;
unpack_secret:
  CALL_C(0x45b1, secretFunctionCaller_hook, 0x1a2e, 0x45b4);
  if (!(F & FZ)) { CYCT(0x45b4, 0x45b6); runSecretEntryMenu__invalidSecret_hook(gb); return; }
  CYC(0x45b4, 0x45b6);
  CYC(0x45b6, 0x45b8); B = 0x02;
  CALL_C(0x45b8, secretFunctionCaller_hook, 0x1a2e, 0x45bb);
  if (!(F & FZ)) { CYCT(0x45bb, 0x45bd); runSecretEntryMenu__invalidSecret_hook(gb); return; }
  CYC(0x45bb, 0x45bd);
  CYC(0x45bd, 0x45c0); A = mem_rd(gb, wTmpcec0 + 4);
  CYC(0x45c0, 0x45c1); B = A;
  CYC(0x45c1, 0x45c4); A = W8(wSecretInputType);
  CYC(0x45c4, 0x45c6); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x45c6, 0x45c8); goto check_short_secret; }
  CYC(0x45c6, 0x45c8);
  CYC(0x45c8, 0x45c9); alu_xor(gb, A);
  CYC(0x45c9, 0x45cc); W8(wSecretInputType) = A;
  CYC(0x45cc, 0x45cd); A = B;
  CYC(0x45cd, 0x45cf); runSecretEntryMenu__setTextInputResult_hook(gb);
  return;
check_short_secret:
  CYC(0x45cf, 0x45d1); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x45d1, 0x45d3); runSecretEntryMenu__loadRingSecretData_hook(gb); return; }
  CYC(0x45d1, 0x45d3);
  CYC(0x45d3, 0x45d5); alu_and(gb, 0x3f);
  CYC(0x45d5, 0x45d6); alu_sub(gb, B);
  if (F & FZ) { CYCT(0x45d6, 0x45d8); runSecretEntryMenu__setTextInputResult_hook(gb); return; }
  CYC(0x45d6, 0x45d8);
  runSecretEntryMenu__invalidSecret_hook(gb);
}

void runSecretEntryMenu__invalidSecret_hook(GB *gb) {
  CYC(0x45d8, 0x45da); A = 0x01;
  runSecretEntryMenu__setTextInputResult_hook(gb);
}

void runSecretEntryMenu__setTextInputResult_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x45da, 0x45dd); W8(wTextInputResult) = A;
  if (!(F & FZ)) { CYCT(0x45dd, 0x45df); fileSelect_printError_hook(gb); return; }
  CYC(0x45dd, 0x45df);
  CYC(0x45df, 0x45e1); A = 0x4d;
  CALL_C(0x45e1, playSound_b00_hook, 0x0c98, 0x45e4);
  CYC(0x45e4, 0x45e7); closeMenu_hook(gb);
}

void runSecretEntryMenu__loadRingSecretData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x45e7, 0x45ea); SET_BC(0x0402);
  CALL_C(0x45ea, secretFunctionCaller_hook, 0x1a2e, 0x45ed);
  CYC(0x45ed, 0x45ee); alu_xor(gb, A);
  CYC(0x45ee, 0x45f0); runSecretEntryMenu__setTextInputResult_hook(gb);
}

void fileSelectMode4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x43d9, fileSelectMode4__mode4Update_hook, 0x43e2, 0x43dc);
  CALL_C(0x43dc, fileSelectDrawAcornCursor_hook, 0x4a97, 0x43df);
  CYC(0x43df, 0x43e2); fileSelectDrawLink_hook(gb);
}

void fileSelectMode4__mode4Update_hook(GB *gb) {
  CYC(0x43e2, 0x43e5); A = W8(wFileSelect_mode2);
  CYC(0x43e5, 0x43e6); push_effect(gb, 0x43e6);
  switch (function_caller_jump_table(gb)) {
    case 0x43ee: fileSelectMode4__mode0_hook(gb); return;
    case 0x440f: fileSelectMode4__mode1_hook(gb); return;
    case 0x4425: fileSelectMode4__mode2_hook(gb); return;
    case 0x4443: fileSelectMode4__mode3_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void fileSelectMode4__mode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x43ee, setFileSelectCursorOffsetToFileSelectMode_hook, 0x4152, 0x43f1);
  CYC(0x43f1, 0x43f3); A = 0x03;
  CALL_C(0x43f3, func_02_4149_hook, 0x4149, 0x43f6);
  CALL_C(0x43f6, disableLcd_hook, 0x02c1, 0x43f9);
  CYC(0x43f9, 0x43fb); A = 0xa4;
  CALL_C(0x43fb, loadGfxHeader_hook, 0x0626, 0x43fe);
  CYC(0x43fe, 0x4400); A = 0x06;
  CALL_C(0x4400, loadPaletteHeader_hook, 0x050b, 0x4403);
  CALL_C(0x4403, loadFileDisplayVariables_hook, 0x49da, 0x4406);
  CALL_C(0x4406, textInput_updateEntryCursor_hook, 0x4a22, 0x4409);
  CALL_C(0x4409, fileSelectDrawHeartsAndDeathCounter_hook, 0x4a4a, 0x440c);
  CYC(0x440c, 0x440f); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode4__mode1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x440f, fileSelectUpdateInput_hook, 0x4467, 0x4412);
  if (F & FZ) { CYCT(0x4412, 0x4413); ret_effect(gb); return; }
  CYC(0x4412, 0x4413);
  CYC(0x4413, 0x4415); A = 0x56;
  CALL_C(0x4415, playSound_b00_hook, 0x0c98, 0x4418);
  CYC(0x4418, 0x441b); A = W8(wFileSelect_cursorPos);
  CYC(0x441b, 0x441d); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x441d, 0x4420); setFileSelectModeTo1_hook(gb); return; }
  CYC(0x441d, 0x4420);
  CYC(0x4420, 0x4422); H8(hActiveFileSlot) = A;
  CYC(0x4422, 0x4425); incFileSelectMode2_hook(gb);
}

void fileSelectMode4__mode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4425, decFileSelectMode2IfBPressed_hook, 0x416f, 0x4428);
  if (!(F & FZ)) { CYCT(0x4428, 0x442a); goto selected; }
  CYC(0x4428, 0x442a);
  CALL_C(0x442a, func_02_448d_hook, 0x448d, 0x442d);
  if (F & FZ) { CYCT(0x442d, 0x442e); ret_effect(gb); return; }
  CYC(0x442d, 0x442e);
  CYC(0x442e, 0x4431); A = W8(wFileSelect_cursorPos2);
  CYC(0x4431, 0x4432); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4432, 0x4435); setFileSelectModeTo1_hook(gb); return; }
  CYC(0x4432, 0x4435);
  CYC(0x4435, 0x4438); incFileSelectMode2_hook(gb);
  return;

selected:
  CYC(0x4438, 0x443a); A = 0x50;
  CALL_C(0x443a, playSound_b00_hook, 0x0c98, 0x443d);
  CYC(0x443d, 0x4440); A = W8(wFileSelect_cursorPos);
  CYC(0x4440, 0x4443); func_02_4149_hook(gb);
}

void fileSelectMode4__mode3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4443, 0x4446); SET_HL(wFileSelect_linkTimer);
  CYC(0x4446, 0x4447); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4447, 0x4449); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4449, 0x444a); ret_effect(gb); return; }
  CYC(0x4449, 0x444a);
  CYC(0x444a, 0x444c); A = H8(hActiveFileSlot);
  CYC(0x444c, 0x444e); D = 0x02;
  CALL_C(0x444e, getFileDisplayVariableAddress_hook, 0x417a, 0x4451);
  CYC(0x4451, 0x4452); A = mem_rd(gb, HL);
  CYC(0x4452, 0x4453); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4453, 0x4455); goto erase; }
  CYC(0x4453, 0x4455);
  CYC(0x4455, 0x4456); A = alu_dec8(gb, A);
  CYC(0x4456, 0x4457); mem_wr(gb, HL, A);
  CYC(0x4457, 0x4459); alu_and(gb, 0x03);
  CYC(0x4459, 0x445b); A = 0x57;
  if (F & FZ) CALL_C_CC(0x445b, playSound_b00_hook, 0x0c98, 0x445e);
  else CYC(0x445b, 0x445e);
  CYC(0x445e, 0x4461); fileSelectDrawHeartsAndDeathCounter_hook(gb);
  return;

erase:
  CALL_C(0x4461, eraseFile_b00_hook, 0x09e0, 0x4464);
  CYC(0x4464, 0x4467); setFileSelectModeTo1_hook(gb);
}

void func_02_461c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x461c, 0x461e); A = 0xac;
  CALL_C(0x461e, loadGfxHeader_hook, 0x0626, 0x4621);
  CYC(0x4621, 0x4623); A = 0x08;
  CYC(0x4623, 0x4626); loadUncompressedGfxHeader_hook(gb);
}

void getNameBufferLength_hook(GB *gb) {
  CYC(0x4626, 0x4629); SET_HL(0xd7a0);
  CYC(0x4629, 0x462b); B = 0x05;
  CYC(0x462b, 0x462c); alu_xor(gb, A);
  do {
    CYC(0x462c, 0x462d); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FZ)) CYCT(0x462d, 0x462f);
    else { CYC(0x462d, 0x462f); CYC(0x462f, 0x4631); mem_wr(gb, HL, 0x20); }
    CYC(0x4631, 0x4632); L = alu_inc8(gb, L);
    CYC(0x4632, 0x4633); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x4633, 0x4635); else CYC(0x4633, 0x4635);
  } while (!(F & FZ));
  CYC(0x4635, 0x4636); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(0x4636, 0x4638); B = 0x05;
  for (;;) {
    CYC(0x4638, 0x4639); A = mem_rd(gb, HL);
    CYC(0x4639, 0x463b); alu_sub(gb, 0x20);
    if (!(F & FZ)) { CYCT(0x463b, 0x463c); ret_effect(gb); return; }
    CYC(0x463b, 0x463c);
    CYC(0x463c, 0x463d); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
    CYC(0x463d, 0x463e); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x463e, 0x4640); continue; }
    CYC(0x463e, 0x4640); ret_effect(gb); return;
  }
}

void copyNameToW4NameBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4641, 0x4644); mem_wr(gb, 0xcbb7, A);
  CYC(0x4644, 0x4647); SET_DE(0xc602);
  CYC(0x4647, 0x4649); alu_cp(gb, 0x01);
  if (!(F & FZ)) CYCT(0x4649, 0x464b);
  else { CYC(0x4649, 0x464b); CYC(0x464b, 0x464d); E = 0x09; }
  CYC(0x464d, 0x4650); SET_HL(0xd7a0);
  CYC(0x4650, 0x4652); B = 0x06;
  CALL_C(0x4652, copyMemoryReverse_hook, 0x047f, 0x4655);
  CYC(0x4655, 0x4657); A = 0x04;
  CYC(0x4657, 0x465a); mem_wr(gb, 0xcbb8, A);
  CYC(0x465a, 0x465c); label_02_038_hook(gb);
}

void func_02_465c_hook(GB *gb) {
  CYC(0x465c, 0x465f); A = W8(wSecretInputType);
  CYC(0x465f, 0x4661); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x4661, 0x4663); goto secret_input; }
  CYC(0x4661, 0x4663);
  CYC(0x4663, 0x4666); SET_BC(0x0e81);
  CYC(0x4666, 0x4668); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4668, 0x466a); goto set_input_mode; }
  CYC(0x4668, 0x466a);
  CYC(0x466a, 0x466d); SET_BC(0x1382);
  CYC(0x466d, 0x466f);
  goto set_input_mode;
secret_input:
  CYC(0x466f, 0x4671); A = 0xff;
  CYC(0x4671, 0x4674); W8(wLastSecretInputLength) = A;
  CYC(0x4674, 0x4677); SET_BC(0x0480);
set_input_mode:
  CYC(0x4677, 0x4678); A = B;
  CYC(0x4678, 0x467b); W8(wFileSelect_textInputMaxCursorPos) = A;
  CYC(0x467b, 0x467c); A = C;
  CYC(0x467c, 0x467f); W8(wFileSelect_textInputMode) = A;
  label_02_038_hook(gb);
}

void label_02_038_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x467f, 0x4682); SET_HL(wTmpcbb9);
  CYC(0x4682, 0x4684); B = 0x0a;
  CALL_C(0x4684, clearMemory_hook, 0x046f, 0x4687);
  CALL_C(0x4687, textInput_loadCharacterGfx_hook, 0x49a5, 0x468a);
  CALL_C(0x468a, disableLcd_hook, 0x02c1, 0x468d);
  CYC(0x468d, 0x468f); A = 0x0b;
  CALL_C(0x468f, loadUncompressedGfxHeader_hook, 0x05da, 0x4692);
  CYC(0x4692, 0x4694); A = 0x05;
  CALL_C(0x4694, loadPaletteHeader_hook, 0x050b, 0x4697);
  CYC(0x4697, 0x469a); A = W8(wFileSelect_textInputMode);
  CYC(0x469a, 0x469b); alu_rlca(gb);
  if (F & FC) { CYCT(0x469b, 0x469d); label_02_038__secretEntry_hook(gb); return; }
  CYC(0x469b, 0x469d);
  CYC(0x469d, 0x469f); A = 0xa5;
  CALL_C(0x469f, loadGfxHeader_hook, 0x0626, 0x46a2);
  CYC(0x46a2, 0x46a5); A = W8(wFileSelect_textInputMode);
  CYC(0x46a5, 0x46a6); alu_rrca(gb);
  if (F & FC) CYCT(0x46a6, 0x46a8);
  else {
    CYC(0x46a6, 0x46a8);
    CYC(0x46a8, 0x46aa); A = H8(hActiveFileSlot);
    CYC(0x46aa, 0x46ac); alu_add(gb, 0x20);
    CYC(0x46ac, 0x46af); mem_wr(gb, w4TileMap + 0x49, A);
  }
  CYC(0x46af, 0x46b1); A = 0x08;
  CALL_C(0x46b1, loadUncompressedGfxHeader_hook, 0x05da, 0x46b4);
  CYC(0x46b4, 0x46b6);
  label_02_038__end_hook(gb);
}

void label_02_038__secretEntry_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46b6, 0x46b9); A = W8(wFileSelect_textInputMaxCursorPos);
  CYC(0x46b9, 0x46bc); SET_HL(wLastSecretInputLength);
  CYC(0x46bc, 0x46bd); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x46bd, 0x46be); mem_wr(gb, HL, A);
  CYC(0x46be, 0x46c1); SET_HL(w4SecretBuffer);
  CYC(0x46c1, 0x46c3); B = 0x20;
  CYC(0x46c3, 0x46c5); A = 0x20;
  if (!(F & FZ)) CALL_C_CC(0x46c5, fillMemory_hook, 0x0470, 0x46c8);
  else CYC(0x46c5, 0x46c8);
  CYC(0x46c8, 0x46ca); A = 0xaa;
  CALL_C(0x46ca, loadGfxHeader_hook, 0x0626, 0x46cd);
  CALL_C(0x46cd, func_02_461c_hook, 0x461c, 0x46d0);
  label_02_038__end_hook(gb);
}

void label_02_038__end_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x46d0, textInput_updateEntryCursor_hook, 0x4a22, 0x46d3);
  CYC(0x46d3, 0x46d6); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void runTextInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46d6, 0x46d8); A = 0x01;
  CYC(0x46d8, 0x46db); W8(wTextInputResult) = A;
  CALL_C(0x46db, getInputWithAutofire_hook, 0x0294, 0x46de);
  CYC(0x46de, 0x46df); B = A;
  CALL_C(0x46df, getHighestSetBit_hook, 0x01ea, 0x46e2);
  if (!(F & FC)) { CYCT(0x46e2, 0x46e3); ret_effect(gb); return; }
  CYC(0x46e2, 0x46e3);
  CYC(0x46e3, 0x46e4); B = A;
  CYC(0x46e4, 0x46e7); SET_HL(0x46fe);
  CYC(0x46e7, 0x46e8); push_effect(gb, 0x46e8); add_a_to_hl(gb);
  CYC(0x46e8, 0x46e9); A = mem_rd(gb, HL);
  CALL_C(0x46e9, playSound_b00_hook, 0x0c98, 0x46ec);
  CYC(0x46ec, 0x46ed); A = B;
  CYC(0x46ed, 0x46ee); push_effect(gb, 0x46ee);
  switch (function_caller_jump_table(gb)) {
    case 0x4706: runTextInput__aButton_hook(gb); return;
    case 0x4764: runTextInput__bButton_hook(gb); return;
    case 0x4775: runTextInput__selectButton_hook(gb); return;
    case 0x47e5: runTextInput__startButton_hook(gb); return;
    case 0x4790: runTextInput__rightButton_hook(gb); return;
    case 0x4794: runTextInput__leftButton_hook(gb); return;
    case 0x47c6: runTextInput__upButton_hook(gb); return;
    case 0x47ca: runTextInput__downButton_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runTextInput__aButton_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4706, 0x4709); SET_HL(wFileSelect_cursorPos);
  CYC(0x4709, 0x470a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x470a, 0x470c); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x470c, 0x470e); runTextInput__lowerOptions_hook(gb); return; }
  CYC(0x470c, 0x470e);
  CALL_C(0x470e, textInput_getCursorPosition_hook, 0x47fb, 0x4711);
  CYC(0x4711, 0x4713); alu_and(gb, 0x0f);
  CYC(0x4713, 0x4716); SET_HL(w4TileMap + 0xa3);
  CYC(0x4716, 0x4717); push_effect(gb, 0x4717); add_a_to_hl(gb);
  CYC(0x4717, 0x4718); A = B;
  CYC(0x4718, 0x471a); A = alu_swap(gb, A);
  CYC(0x471a, 0x471b); alu_add(gb, A);
  CYC(0x471b, 0x471c); alu_add(gb, A);
  CALL_C(0x471c, multiplyABy16_hook, 0x01ac, 0x471f);
  CYC(0x471f, 0x4720); alu_add_hl(gb, BC);
  CYC(0x4720, 0x4722); C = 0x20;
  CYC(0x4722, 0x4723); A = mem_rd(gb, HL);
  CYC(0x4723, 0x4725); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4725, 0x4727); runTextInput__gotCharacter_hook(gb); return; }
  CYC(0x4725, 0x4727);
  CYC(0x4727, 0x4728); alu_rrca(gb);
  CYC(0x4728, 0x472a); alu_and(gb, 0x3f);
  CYC(0x472a, 0x472c); alu_add(gb, 0x40);
  CYC(0x472c, 0x472d); C = A;
  CYC(0x472d, 0x4730); A = W8(wFileSelect_textInputMode);
  CYC(0x4730, 0x4731); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x4731, 0x4733); runTextInput__gotCharacter_hook(gb); return; }
  CYC(0x4731, 0x4733);
  CYC(0x4733, 0x4734); A = C;
  CYC(0x4734, 0x4737); SET_HL(0x0061);
  CYC(0x4737, 0x4738); push_effect(gb, 0x4738); add_a_to_hl(gb);
  CYC(0x4738, 0x4739); C = mem_rd(gb, HL);
  runTextInput__gotCharacter_hook(gb);
}

void runTextInput__gotCharacter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4739, textInput_getOutputAddress_hook, 0x4a37, 0x473c);
  CYC(0x473c, 0x473d); mem_wr(gb, HL, C);
  runTextInput__selectionRight_hook(gb);
}

void runTextInput__selectionRight_hook(GB *gb) {
  CYC(0x473d, 0x4740); SET_HL(wFileSelect_textInputCursorPos);
  CYC(0x4740, 0x4741); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4741, 0x4744); A = W8(wFileSelect_textInputMaxCursorPos);
  CYC(0x4744, 0x4745); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x4745, 0x4747); runTextInput__updateEntryCursor_hook(gb); return; }
  CYC(0x4745, 0x4747);
  CYC(0x4747, 0x4748); mem_wr(gb, HL, A);
  runTextInput__updateEntryCursor_hook(gb);
}

void runTextInput__updateEntryCursor_hook(GB *gb) {
  CYC(0x4748, 0x474b); textInput_updateEntryCursor_hook(gb);
}

void runTextInput__lowerOptions_hook(GB *gb) {
  CYC(0x474b, 0x474e); A = W8(wFileSelect_textInputMode);
  CYC(0x474e, 0x474f); alu_rlca(gb);
  CYC(0x474f, 0x4752); A = W8(wFileSelect_cursorPos2);
  if (F & FC) { CYCT(0x4752, 0x4754); runTextInput__secretTable_hook(gb); return; }
  CYC(0x4752, 0x4754);
  runTextInput__nameTable_hook(gb);
}

void runTextInput__nameTable_hook(GB *gb) {
  CYC(0x4754, 0x4755); push_effect(gb, 0x4755);
  switch (function_caller_jump_table(gb)) {
    case 0x4769: runTextInput__selectionLeft_hook(gb); return;
    case 0x473d: runTextInput__selectionRight_hook(gb); return;
    case 0x47e5: runTextInput__startButton_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runTextInput__secretTable_hook(GB *gb) {
  CYC(0x475b, 0x475c); push_effect(gb, 0x475c);
  switch (function_caller_jump_table(gb)) {
    case 0x4769: runTextInput__selectionLeft_hook(gb); return;
    case 0x473d: runTextInput__selectionRight_hook(gb); return;
    case 0x4776: runTextInput__back_hook(gb); return;
    case 0x47e5: runTextInput__startButton_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runTextInput__bButton_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4764, textInput_getOutputAddress_hook, 0x4a37, 0x4767);
  CYC(0x4767, 0x4769); mem_wr(gb, HL, 0x20);
  runTextInput__selectionLeft_hook(gb);
}

void runTextInput__selectionLeft_hook(GB *gb) {
  CYC(0x4769, 0x476c); SET_HL(wFileSelect_textInputCursorPos);
  CYC(0x476c, 0x476d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x476d, 0x476f); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x476f, 0x4771); runTextInput__updateEntryCursor_hook(gb); return; }
  CYC(0x476f, 0x4771);
  CYC(0x4771, 0x4773); mem_wr(gb, HL, 0x00);
  CYC(0x4773, 0x4775); runTextInput__updateEntryCursor_hook(gb);
}

void runTextInput__selectButton_hook(GB *gb) {
  CYC(0x4775, 0x4776); ret_effect(gb);
}

void runTextInput__back_hook(GB *gb) {
  CYC(0x4776, 0x4779); A = W8(wFileSelect_textInputMode);
  CYC(0x4779, 0x477a); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x477a, 0x477b); ret_effect(gb); return; }
  CYC(0x477a, 0x477b);
  CYC(0x477b, 0x477c); alu_xor(gb, A);
  CYC(0x477c, 0x477f); W8(wTmpcbb9) = A;
  CYC(0x477f, 0x4782); SET_HL(wFileSelect_cursorPos);
  CYC(0x4782, 0x4784); A = 0x57;
  CYC(0x4784, 0x4785); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4785, 0x4787); A = 0x02;
  CYC(0x4787, 0x4788); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x4788, 0x4789); mem_wr(gb, HL, A); SET_HL(HL - 1);
  if (!(F & FZ)) { CYCT(0x4789, 0x478a); ret_effect(gb); return; }
  CYC(0x4789, 0x478a);
  CYC(0x478a, 0x478c); A = 0x03;
  CYC(0x478c, 0x478f); W8(wFileSelect_mode2) = A;
  CYC(0x478f, 0x4790); ret_effect(gb);
}

void runTextInput__rightButton_hook(GB *gb) {
  CYC(0x4790, 0x4792); C = 0x01;
  CYC(0x4792, 0x4794); runTextInput__leftOrRight_hook(gb);
}

void runTextInput__leftButton_hook(GB *gb) {
  CYC(0x4794, 0x4796); C = 0xff;
  runTextInput__leftOrRight_hook(gb);
}

void runTextInput__leftOrRight_hook(GB *gb) {
  CYC(0x4796, 0x4799); SET_DE(0x040d);
  CYC(0x4799, 0x479c); A = W8(wFileSelect_textInputMode);
  CYC(0x479c, 0x479d); alu_rlca(gb);
  if (F & FC) { CYCT(0x479d, 0x479f); goto check_upper_options; }
  CYC(0x479d, 0x479f);
  CYC(0x479f, 0x47a2); SET_DE(0x030c);
check_upper_options:
  CYC(0x47a2, 0x47a5); SET_HL(wFileSelect_cursorPos);
  CYC(0x47a5, 0x47a6); A = mem_rd(gb, HL);
  CYC(0x47a6, 0x47a8); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x47a8, 0x47aa); runTextInput__leftOrRight__lowerOptions_hook(gb); return; }
  CYC(0x47a8, 0x47aa);
  for (;;) {
    CYC(0x47aa, 0x47ab); alu_add(gb, C);
    CYC(0x47ab, 0x47ad); alu_and(gb, 0x0f);
    CYC(0x47ad, 0x47ae); alu_cp(gb, E);
    if (!(F & FC)) { CYCT(0x47ae, 0x47b0); continue; }
    CYC(0x47ae, 0x47b0);
    break;
  }
  CYC(0x47b0, 0x47b1); C = A;
  CYC(0x47b1, 0x47b2); A = mem_rd(gb, HL);
  CYC(0x47b2, 0x47b4); alu_and(gb, 0xf0);
  CYC(0x47b4, 0x47b5); alu_add(gb, C);
  CYC(0x47b5, 0x47b6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x47b6, 0x47b8); mem_wr(gb, HL, 0x80);
  CYC(0x47b8, 0x47b9); ret_effect(gb);
}

void runTextInput__leftOrRight__lowerOptions_hook(GB *gb) {
  CYC(0x47b9, 0x47ba); L = alu_inc8(gb, L);
  CYC(0x47ba, 0x47bb); B = D;
  CYC(0x47bb, 0x47bc); A = mem_rd(gb, HL);
  for (;;) {
    CYC(0x47bc, 0x47bd); alu_add(gb, C);
    CYC(0x47bd, 0x47bf); alu_and(gb, 0x0f);
    CYC(0x47bf, 0x47c0); alu_cp(gb, B);
    if (!(F & FC)) { CYCT(0x47c0, 0x47c2); continue; }
    CYC(0x47c0, 0x47c2);
    break;
  }
  CYC(0x47c2, 0x47c3); mem_wr(gb, HL, A);
  CYC(0x47c3, 0x47c6); textInput_lowerOption_updateFileSelectCursorPos_hook(gb);
}

void runTextInput__upButton_hook(GB *gb) {
  CYC(0x47c6, 0x47c8); C = 0xf0;
  CYC(0x47c8, 0x47ca); runTextInput__upOrDown_hook(gb);
}

void runTextInput__downButton_hook(GB *gb) {
  CYC(0x47ca, 0x47cc); C = 0x10;
  runTextInput__upOrDown_hook(gb);
}

void runTextInput__upOrDown_hook(GB *gb) {
  CYC(0x47cc, 0x47cf); SET_HL(wFileSelect_cursorPos);
  CYC(0x47cf, 0x47d0); A = mem_rd(gb, HL);
  for (;;) {
    CYC(0x47d0, 0x47d1); alu_add(gb, C);
    CYC(0x47d1, 0x47d3); alu_and(gb, 0x70);
    CYC(0x47d3, 0x47d5); alu_cp(gb, 0x60);
    if (!(F & FC)) { CYCT(0x47d5, 0x47d7); continue; }
    CYC(0x47d5, 0x47d7);
    break;
  }
  CYC(0x47d7, 0x47d8); C = A;
  CYC(0x47d8, 0x47d9); A = mem_rd(gb, HL);
  CYC(0x47d9, 0x47db); alu_and(gb, 0x0f);
  CYC(0x47db, 0x47dc); alu_add(gb, C);
  CYC(0x47dc, 0x47dd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x47dd, 0x47df); mem_wr(gb, HL, 0x80);
  CYC(0x47df, 0x47e1); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(0x47e1, 0x47e2); ret_effect(gb); return; }
  CYC(0x47e1, 0x47e2);
  CYC(0x47e2, 0x47e5); textInput_lowerOption_updateFileSelectCursorPos2_hook(gb);
}

void runTextInput__startButton_hook(GB *gb) {
  CYC(0x47e5, 0x47e8); SET_HL(wFileSelect_cursorPos);
  CYC(0x47e8, 0x47ea); A = 0x5a;
  CYC(0x47ea, 0x47eb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x47eb, 0x47ee); A = W8(wFileSelect_textInputMode);
  CYC(0x47ee, 0x47ef); alu_rlca(gb);
  CYC(0x47ef, 0x47f1); A = 0x02;
  if (!(F & FC)) CYCT(0x47f1, 0x47f3);
  else { CYC(0x47f1, 0x47f3); CYC(0x47f3, 0x47f5); A = 0x03; }
  CYC(0x47f5, 0x47f6); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x47f6, 0x47f7); mem_wr(gb, HL, A); SET_HL(HL - 1);
  if (!(F & FZ)) { CYCT(0x47f7, 0x47f8); ret_effect(gb); return; }
  CYC(0x47f7, 0x47f8);
  CYC(0x47f8, 0x47fb); incFileSelectMode2_hook(gb);
}

void textInput_getCursorPosition_hook(GB *gb) {
  CYC(0x47fb, 0x47fe); A = mem_rd(gb, 0xcbbc);
  CYC(0x47fe, 0x47ff); C = A;
  CYC(0x47ff, 0x4801); alu_and(gb, 0xf0);
  CYC(0x4801, 0x4802); B = A;
  CYC(0x4802, 0x4803); A = C;
  CYC(0x4803, 0x4805); alu_and(gb, 0x0f);
  CYC(0x4805, 0x4806); C = A;
  CYC(0x4806, 0x4807); push_effect(gb, DE);
  CYC(0x4807, 0x480a); SET_DE(0x0801);
  CYC(0x480a, 0x480d); A = mem_rd(gb, 0xcbb7);
  CYC(0x480d, 0x480e); alu_rlca(gb);
  if (F & FC) CYCT(0x480e, 0x4810);
  else { CYC(0x480e, 0x4810); CYC(0x4810, 0x4813); SET_DE(0x0602); }
  CYC(0x4813, 0x4814); A = C;
  CYC(0x4814, 0x4815); alu_cp(gb, D);
  CYC(0x4815, 0x4816); C = E;
  CYC(0x4816, 0x4817); SET_DE(pop_effect(gb));
  if (!(F & FC)) CYCT(0x4817, 0x4819);
  else { CYC(0x4817, 0x4819); CYC(0x4819, 0x481b); C = 0; }
  CYC(0x481b, 0x481c); alu_add(gb, C);
  CYC(0x481c, 0x481d); alu_add(gb, B);
  CYC(0x481d, 0x481e); ret_effect(gb);
}

static void draw_name_input_cursors_tail(GB *gb) {
  CYC(0x4851, 0x4854); A = W8(wFileSelect_textInputCursorPos);
  CYC(0x4854, 0x4855); alu_add(gb, A);
  CYC(0x4855, 0x4856); alu_add(gb, A);
  CYC(0x4856, 0x4857); alu_add(gb, A);
  CYC(0x4857, 0x4858); C = A;
  CYC(0x4858, 0x485a); B = 0;
  CYC(0x485a, 0x485d); SET_HL(0x4879);
  CYC(0x485d, 0x4860); addSpritesToOam_withOffset_hook(gb);
}

void drawNameInputCursors_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x481e, textInput_getCursorPosition_hook, 0x47fb, 0x4821);
  CYC(0x4821, 0x4823); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x4823, 0x4825); drawNameInputCursors__lowerOptions_hook(gb); return; }
  CYC(0x4823, 0x4825); drawNameInputCursors__upperOptions_hook(gb);
}

void drawNameInputCursors__upperOptions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4825, 0x4826); B = A;
  CYC(0x4826, 0x4828); alu_and(gb, 0x0f);
  CYC(0x4828, 0x4829); alu_add(gb, A);
  CYC(0x4829, 0x482a); alu_add(gb, A);
  CYC(0x482a, 0x482b); alu_add(gb, A);
  CYC(0x482b, 0x482c); C = A;
  CYC(0x482c, 0x482d); A = B;
  CYC(0x482d, 0x482f); alu_and(gb, 0xf0);
  CYC(0x482f, 0x4830); B = A;
  CYC(0x4830, 0x4833); SET_HL(0x4860);
  CALL_C(0x4833, addSpritesToOam_withOffset_hook, 0x0d61, 0x4836);
  CYC(0x4836, 0x4838); draw_name_input_cursors_tail(gb);
}

void drawNameInputCursors__lowerOptions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4838, 0x483b); A = mem_rd(gb, 0xcbb7);
  CYC(0x483b, 0x483c); alu_rlca(gb);
  CYC(0x483c, 0x483f); SET_HL(0x486c);
  if (F & FC) CYCT(0x483f, 0x4841);
  else { CYC(0x483f, 0x4841); CYC(0x4841, 0x4844); SET_HL(0x4869); }
  CYC(0x4844, 0x4847); A = mem_rd(gb, 0xcbbd);
  CYC(0x4847, 0x4848); push_effect(gb, 0x4848); add_a_to_hl(gb);
  CYC(0x4848, 0x4849); C = mem_rd(gb, HL);
  CYC(0x4849, 0x484b); B = 0;
  CYC(0x484b, 0x484e); SET_HL(0x4870);
  CALL_C(0x484e, addSpritesToOam_withOffset_hook, 0x0d61, 0x4851);
  draw_name_input_cursors_tail(gb);
}

static void draw_secret_input_cursors_tail(GB *gb) {
  CYC(0x48a8, 0x48aa); C = 0x0a;
  CYC(0x48aa, 0x48ad); A = W8(wFileSelect_textInputCursorPos);
  CYC(0x48ad, 0x48ae); alu_cp(gb, C);
  CYC(0x48ae, 0x48b0); B = 0;
  if (F & FC) CYCT(0x48b0, 0x48b2);
  else {
    CYC(0x48b0, 0x48b2);
    CYC(0x48b2, 0x48b4); B = 0x10;
    CYC(0x48b4, 0x48b5); alu_sub(gb, C);
  }
  CYC(0x48b5, 0x48b7); alu_cp(gb, 0x05);
  if (F & FC) CYCT(0x48b7, 0x48b9);
  else { CYC(0x48b7, 0x48b9); CYC(0x48b9, 0x48ba); A = alu_inc8(gb, A); }
  CYC(0x48ba, 0x48bb); alu_add(gb, A);
  CYC(0x48bb, 0x48bc); alu_add(gb, A);
  CYC(0x48bc, 0x48bd); alu_add(gb, A);
  CYC(0x48bd, 0x48be); C = A;
  CYC(0x48be, 0x48c1); SET_HL(0x48da);
  CYC(0x48c1, 0x48c4); addSpritesToOam_withOffset_hook(gb);
}

void drawSecretInputCursors_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x487e, textInput_getCursorPosition_hook, 0x47fb, 0x4881);
  CYC(0x4881, 0x4883); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x4883, 0x4885); drawSecretInputCursors__lowerOptions_hook(gb); return; }
  CYC(0x4883, 0x4885); drawSecretInputCursors__upperOptions_hook(gb);
}

void drawSecretInputCursors__upperOptions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4885, 0x4886); B = A;
  CYC(0x4886, 0x4888); alu_and(gb, 0x0f);
  CYC(0x4888, 0x4889); alu_add(gb, A);
  CYC(0x4889, 0x488a); alu_add(gb, A);
  CYC(0x488a, 0x488b); alu_add(gb, A);
  CYC(0x488b, 0x488c); C = A;
  CYC(0x488c, 0x488d); A = B;
  CYC(0x488d, 0x488f); alu_and(gb, 0xf0);
  CYC(0x488f, 0x4890); B = A;
  CYC(0x4890, 0x4893); SET_HL(0x48c4);
  CALL_C(0x4893, addSpritesToOam_withOffset_hook, 0x0d61, 0x4896);
  CYC(0x4896, 0x4898); draw_secret_input_cursors_tail(gb);
}

void drawSecretInputCursors__lowerOptions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4898, 0x489b); A = mem_rd(gb, 0xcbbd);
  CYC(0x489b, 0x489e); SET_HL(0x48cd);
  CYC(0x489e, 0x489f); push_effect(gb, 0x489f); add_a_to_hl(gb);
  CYC(0x489f, 0x48a0); C = mem_rd(gb, HL);
  CYC(0x48a0, 0x48a2); B = 0;
  CYC(0x48a2, 0x48a5); SET_HL(0x48d1);
  CALL_C(0x48a5, addSpritesToOam_withOffset_hook, 0x0d61, 0x48a8);
  draw_secret_input_cursors_tail(gb);
}

void textInput_lowerOption_updateFileSelectCursorPos_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x48df, 0x48e2); A = mem_rd(gb, 0xcbbd);
  CYC(0x48e2, 0x48e3); E = A;
  CYC(0x48e3, 0x48e5); D = 0xff;
  CALL_C(0x48e5, textInput_mapUpperXToLowerX_hook, 0x48fb, 0x48e8);
  CYC(0x48e8, 0x48e9); A = B;
  CYC(0x48e9, 0x48ec); mem_wr(gb, 0xcbbc, A);
  CYC(0x48ec, 0x48ed); ret_effect(gb);
}

void textInput_lowerOption_updateFileSelectCursorPos2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x48ed, 0x48f0); A = mem_rd(gb, 0xcbbc);
  CYC(0x48f0, 0x48f1); D = A;
  CYC(0x48f1, 0x48f3); E = 0xff;
  CALL_C(0x48f3, textInput_mapUpperXToLowerX_hook, 0x48fb, 0x48f6);
  CYC(0x48f6, 0x48f7); A = C;
  CYC(0x48f7, 0x48fa); mem_wr(gb, 0xcbbd, A);
  CYC(0x48fa, 0x48fb); ret_effect(gb);
}

void textInput_mapUpperXToLowerX_hook(GB *gb) {
  CYC(0x48fb, 0x48fe); A = mem_rd(gb, 0xcbb7);
  CYC(0x48fe, 0x48ff); alu_rlca(gb);
  CYC(0x48ff, 0x4902); SET_HL(0x4912);
  if (!(F & FC)) { CYCT(0x4902, 0x4904); textInput_mapUpperXToLowerX__label_hook(gb); return; }
  CYC(0x4902, 0x4904);
  CYC(0x4904, 0x4907); SET_HL(0x492e);
  textInput_mapUpperXToLowerX__label_hook(gb);
}

void textInput_mapUpperXToLowerX__label_hook(GB *gb) {
  for (;;) {
    CYC(0x4907, 0x4908); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4908, 0x4909); B = A;
    CYC(0x4909, 0x490a); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x490a, 0x490b); C = A;
    CYC(0x490b, 0x490c); alu_cp(gb, E);
    if (F & FZ) { CYCT(0x490c, 0x490d); ret_effect(gb); return; }
    CYC(0x490c, 0x490d);
    CYC(0x490d, 0x490e); A = B;
    CYC(0x490e, 0x490f); alu_cp(gb, D);
    if (!(F & FZ)) { CYCT(0x490f, 0x4911); continue; }
    CYC(0x490f, 0x4911);
    CYC(0x4911, 0x4912); ret_effect(gb);
    return;
  }
}

void textInput_loadCharacterGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x49a5, 0x49a7); A = hram_rd(gb, 0x70);
  CYC(0x49a7, 0x49a8); push_effect(gb, AF);
  CYC(0x49a8, 0x49aa); A = 0x05;
  CYC(0x49aa, 0x49ac); hram_wr(gb, 0x70, A);
  CYC(0x49ac, 0x49ad); alu_xor(gb, A);
  CYC(0x49ad, 0x49b0); mem_wr(gb, 0xcbba, A);
  CYC(0x49b0, 0x49b3); SET_DE(0xd000);
  CYC(0x49b3, 0x49b6); A = mem_rd(gb, 0xcbb7);
  CYC(0x49b6, 0x49b7); alu_rlca(gb);
  if (F & FC) { CYCT(0x49b7, 0x49b9); goto secret; }
  CYC(0x49b7, 0x49b9);
  CYC(0x49b9, 0x49bc); SET_BC(0x3b40);
  CALL_C(0x49bc, copyTextCharacters_hook, 0x49cd, 0x49bf);
  CYC(0x49bf, 0x49c1);
  goto done;
secret:
  CYC(0x49c1, 0x49c4); SET_HL(0x00a1);
  CYC(0x49c4, 0x49c6); B = 0x40;
  CALL_C(0x49c6, copyTextCharactersFromHlUntilNull, 0x410e, 0x49c9);
done:
  CYC(0x49c9, 0x49ca); SET_AF(pop_effect(gb));
  CYC(0x49ca, 0x49cc); hram_wr(gb, 0x70, A);
  CYC(0x49cc, 0x49cd); ret_effect(gb);
}

void copyTextCharacters_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  do {
    CYC(0x49cd, 0x49ce); push_effect(gb, BC);
    CYC(0x49ce, 0x49cf); A = C;
    CYC(0x49cf, 0x49d1); C = 0;
    CALL_C(0x49d1, copyTextCharacterGfx_hook, 0x19df, 0x49d4);
    CYC(0x49d4, 0x49d5); SET_BC(pop_effect(gb));
    CYC(0x49d5, 0x49d6); C = alu_inc8(gb, C);
    CYC(0x49d6, 0x49d7); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x49d7, 0x49d9); else CYC(0x49d7, 0x49d9);
  } while (!(F & FZ));
  CYC(0x49d9, 0x49da); ret_effect(gb);
}

void loadFileDisplayVariables_hook(GB *gb) {
  CYC(0x49da, 0x49dc); A = 0x02;
  CYC(0x49dc, 0x49de); hram_wr(gb, 0x9a, A);
  loadFileDisplayVariables__nextFile_hook(gb);
}

void loadFileDisplayVariables__nextFile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(0x49de, loadFile_b00_hook, 0x09dc, 0x49e1);
    CYC(0x49e1, 0x49e3); A = hram_rd(gb, 0x9a);
    CYC(0x49e3, 0x49e5); D = 0;
    CALL_C(0x49e5, getFileDisplayVariableAddress_hook, 0x417a, 0x49e8);
    CYC(0x49e8, 0x49e9); A = C;
    CYC(0x49e9, 0x49ea); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49ea, 0x49eb); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49eb, 0x49ee); A = mem_rd(gb, 0xc6ab);
    CYC(0x49ee, 0x49ef); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49ef, 0x49f0); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49f0, 0x49f3); A = mem_rd(gb, 0xc61e);
    CYC(0x49f3, 0x49f4); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49f4, 0x49f7); A = mem_rd(gb, 0xc61f);
    CYC(0x49f7, 0x49f8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49f8, 0x49fb); A = mem_rd(gb, 0xc612);
    CYC(0x49fb, 0x49fc); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49fc, 0x49ff); A = mem_rd(gb, 0xc613);
    CYC(0x49ff, 0x4a00); alu_add(gb, A);
    CYC(0x4a00, 0x4a01); E = A;
    CYC(0x4a01, 0x4a04); A = mem_rd(gb, 0xc614);
    CYC(0x4a04, 0x4a05); alu_or(gb, E);
    CYC(0x4a05, 0x4a06); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x4a06, 0x4a08); A = hram_rd(gb, 0x9a);
    CYC(0x4a08, 0x4a09); alu_add(gb, A);
    CYC(0x4a09, 0x4a0a); E = A;
    CYC(0x4a0a, 0x4a0b); alu_add(gb, E);
    CYC(0x4a0b, 0x4a0c); alu_add(gb, E);
    CYC(0x4a0c, 0x4a0f); SET_HL(0xd7a0);
    CYC(0x4a0f, 0x4a10); push_effect(gb, 0x4a10); add_a_to_hl(gb);
    CYC(0x4a10, 0x4a13); SET_DE(0xc602);
    CYC(0x4a13, 0x4a15); B = 0x06;
    CALL_C(0x4a15, copyMemoryReverse_hook, 0x047f, 0x4a18);
    CYC(0x4a18, 0x4a1b); SET_HL(0xff9a);
    CYC(0x4a1b, 0x4a1c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x4a1c, 0x4a1e); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x4a1e, 0x4a20); continue; }
    CYC(0x4a1e, 0x4a20);
    CYC(0x4a20, 0x4a21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x4a21, 0x4a22); ret_effect(gb);
    return;
  }
}

void textInput_updateEntryCursor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4a22, 0x4a23); alu_xor(gb, A);
  CALL_C(0x4a23, textInput_getOutputAddressOffset_hook, 0x4a3a, 0x4a26);
  CYC(0x4a26, 0x4a29); SET_DE(0xdc00);
  CYC(0x4a29, 0x4a2b); B = 0x18;
  CALL_C(0x4a2b, copyTextCharactersFromHl, 0x4110, 0x4a2e);
  CYC(0x4a2e, 0x4a2f); alu_xor(gb, A);
  CYC(0x4a2f, 0x4a32); mem_wr(gb, 0xcbba, A);
  CYC(0x4a32, 0x4a34); A = 0x07;
  CYC(0x4a34, 0x4a37); loadUncompressedGfxHeader_hook(gb);
}

void textInput_getOutputAddress_hook(GB *gb) {
  CYC(0x4a37, 0x4a3a); A = mem_rd(gb, 0xcbbe);
  textInput_getOutputAddressOffset_hook(gb);
}

void textInput_getOutputAddressOffset_hook(GB *gb) {
  CYC(0x4a3a, 0x4a3b); L = A;
  CYC(0x4a3b, 0x4a3e); A = mem_rd(gb, 0xcbb7);
  CYC(0x4a3e, 0x4a3f); alu_rlca(gb);
  CYC(0x4a3f, 0x4a40); A = L;
  CYC(0x4a40, 0x4a43); SET_HL(0xd7a0);
  if (!(F & FC)) CYCT(0x4a43, 0x4a45);
  else { CYC(0x4a43, 0x4a45); CYC(0x4a45, 0x4a48); SET_HL(0xd7c0); }
  CYC(0x4a48, 0x4a49); push_effect(gb, 0x4a49); add_a_to_hl(gb);
  CYC(0x4a49, 0x4a4a); ret_effect(gb);
}

void fileSelectDrawHeartsAndDeathCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4a4a, 0x4a4d); A = mem_rd(gb, 0xcbb3);
  CYC(0x4a4d, 0x4a4f); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x4a4f, 0x4a50); ret_effect(gb); return; }
  CYC(0x4a4f, 0x4a50);
  CYC(0x4a50, 0x4a52); A = 0xa2;
  CALL_C(0x4a52, loadGfxHeader_hook, 0x0626, 0x4a55);
  CYC(0x4a55, 0x4a58); A = mem_rd(gb, 0xcbbc);
  CYC(0x4a58, 0x4a5a); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x4a5a, 0x4a5c); goto done; }
  CYC(0x4a5a, 0x4a5c);
  CYC(0x4a5c, 0x4a5e); D = 0;
  CALL_C(0x4a5e, getFileDisplayVariableAddress_hook, 0x417a, 0x4a61);
  CYC(0x4a61, 0x4a63); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4a63, 0x4a65); goto done; }
  CYC(0x4a63, 0x4a65);
  CYC(0x4a65, 0x4a67); D = 0x04;
  CALL_C(0x4a67, getFileDisplayVariableAddress_paramE_hook, 0x417b, 0x4a6a);
  CYC(0x4a6a, 0x4a6b); E = L;
  CYC(0x4a6b, 0x4a6c); D = H;
  CYC(0x4a6c, 0x4a6f); SET_HL(0xd130);
  CYC(0x4a6f, 0x4a71); B = 0x10;
  CYC(0x4a71, 0x4a72); A = mem_rd(gb, DE);
  CYC(0x4a72, 0x4a74); alu_and(gb, 0x0f);
  CYC(0x4a74, 0x4a75); alu_add(gb, B);
  CYC(0x4a75, 0x4a76); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(0x4a76, 0x4a77); A = mem_rd(gb, DE);
  CYC(0x4a77, 0x4a79); alu_and(gb, 0xf0);
  CYC(0x4a79, 0x4a7b); A = alu_swap(gb, A);
  CYC(0x4a7b, 0x4a7c); alu_add(gb, B);
  CYC(0x4a7c, 0x4a7d); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(0x4a7d, 0x4a7e); E = alu_inc8(gb, E);
  CYC(0x4a7e, 0x4a7f); A = mem_rd(gb, DE);
  CYC(0x4a7f, 0x4a80); alu_add(gb, B);
  CYC(0x4a80, 0x4a81); mem_wr(gb, HL, A); L = alu_dec8(gb, L);
  CYC(0x4a81, 0x4a84); A = mem_rd(gb, 0xcbbc);
  CYC(0x4a84, 0x4a86); D = 0x02;
  CALL_C(0x4a86, getFileDisplayVariableAddress_hook, 0x417a, 0x4a89);
  CYC(0x4a89, 0x4a8a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a8a, 0x4a8b); B = mem_rd(gb, HL);
  CYC(0x4a8b, 0x4a8c); C = A;
  CYC(0x4a8c, 0x4a8f); SET_HL(0xd14a);
  CALL_C(0x4a8f, fileSelectDrawHeartDisplay_hook, 0x5435, 0x4a92);
done:
  CYC(0x4a92, 0x4a94); A = 0x08;
  CYC(0x4a94, 0x4a97); loadUncompressedGfxHeader_hook(gb);
}

void fileSelectDrawAcornCursor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4a97, 0x4a9a); A = mem_rd(gb, 0xcbbb);
  CYC(0x4a9a, 0x4a9d); SET_HL(0x4ace);
  CYC(0x4a9d, 0x4a9e); add_double_index_to_hl(gb, 0x4a9e);
  CYC(0x4a9e, 0x4a9f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a9f, 0x4aa0); H = mem_rd(gb, HL);
  CYC(0x4aa0, 0x4aa1); L = A;
  CYC(0x4aa1, 0x4aa2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4aa2, 0x4aa3); E = A;
  CYC(0x4aa3, 0x4aa4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4aa4, 0x4aa5); D = A;
  CYC(0x4aa5, 0x4aa6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4aa6, 0x4aa7); C = A;
  CYC(0x4aa7, 0x4aa8); B = mem_rd(gb, HL);
  CYC(0x4aa8, 0x4aa9); push_effect(gb, BC);
  CYC(0x4aa9, 0x4aac); SET_HL(0x4ac9);
  CYC(0x4aac, 0x4aaf); A = mem_rd(gb, 0xcbbc);
  CYC(0x4aaf, 0x4ab1); alu_bit(gb, 7, A);
  if (F & FZ) CALL_C_CC(0x4ab1, fileSelectDrawAcornCursor__func_hook, 0x4abe, 0x4ab4);
  else CYC(0x4ab1, 0x4ab4);
  CYC(0x4ab4, 0x4ab5); SET_DE(pop_effect(gb));
  CYC(0x4ab5, 0x4ab8); SET_HL(0x4ac9);
  CYC(0x4ab8, 0x4abb); A = mem_rd(gb, 0xcbbd);
  CYC(0x4abb, 0x4abd); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x4abd, 0x4abe); ret_effect(gb); return; }
  CYC(0x4abd, 0x4abe); fileSelectDrawAcornCursor__func_hook(gb);
}

void fileSelectDrawAcornCursor__func_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4abe, addDoubleIndexToDe_hook, 0x0072, 0x4ac1);
  CYC(0x4ac1, 0x4ac2); A = mem_rd(gb, DE);
  CYC(0x4ac2, 0x4ac3); B = A;
  CYC(0x4ac3, 0x4ac4); E = alu_inc8(gb, E);
  CYC(0x4ac4, 0x4ac5); A = mem_rd(gb, DE);
  CYC(0x4ac5, 0x4ac6); C = A;
  CYC(0x4ac6, 0x4ac9); addSpritesToOam_withOffset_hook(gb);
}

void hideStatusBar_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f2c, 0x4f2e); A = 0x04;
  CYC(0x4f2e, 0x4f30); hram_wr(gb, 0x70, A);
  CYC(0x4f30, 0x4f33); SET_HL(wDontUpdateStatusBar);
  CYC(0x4f33, 0x4f34); A = mem_rd(gb, HL);
  CYC(0x4f34, 0x4f36); mem_wr(gb, HL, 0xff);
  CYC(0x4f36, 0x4f38); alu_cp(gb, 0x77);
  CYC(0x4f38, 0x4f3a); A = 0x80;
  if (!(F & FZ)) CYCT(0x4f3a, 0x4f3c);
  else { CYC(0x4f3a, 0x4f3c); CYC(0x4f3c, 0x4f3d); alu_xor(gb, A); }
  CYC(0x4f3d, 0x4f40); SET_HL(w4StatusBarAttributeMap);
  CYC(0x4f40, 0x4f42); B = 0x40;
  CALL_C(0x4f42, fillMemory_hook, 0x0470, 0x4f45);
  CYC(0x4f45, 0x4f48); SET_HL(w4StatusBarTileMap);
  CYC(0x4f48, 0x4f4a); B = 0x40;
  CALL_C(0x4f4a, clearMemory_hook, 0x046f, 0x4f4d);
  CYC(0x4f4d, 0x4f4e); alu_xor(gb, A);
  CYC(0x4f4e, 0x4f51); W8(wStatusBarNeedsRefresh) = A;
  CYC(0x4f51, 0x4f53); A = 0x03;
  CALL_C(0x4f53, loadUncompressedGfxHeader_hook, 0x05da, 0x4f56);
  CYC(0x4f56, 0x4f58); B = 0x10;
  CYC(0x4f58, 0x4f5a); A = hram_rd(gb, 0x9f);
  CYC(0x4f5a, 0x4f5b); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x4f5b, 0x4f5c); ret_effect(gb); return; }
  CYC(0x4f5b, 0x4f5c);
  CYC(0x4f5c, 0x4f5e); A = 0xe0;
  CYC(0x4f5e, 0x4f61); SET_HL(wOam);
  CYC(0x4f61, 0x4f64); fillMemory_hook(gb);
}

void showStatusBar_body_hook(GB *gb) {
  CYC(0x4f64, 0x4f65); alu_xor(gb, A);
  CYC(0x4f65, 0x4f68); W8(wDontUpdateStatusBar) = A;
  CYC(0x4f68, 0x4f69); A = alu_dec8(gb, A);
  CYC(0x4f69, 0x4f6c); W8(wStatusBarNeedsRefresh) = A;
  CYC(0x4f6c, 0x4f6d); ret_effect(gb);
}

void openMenu_body_hook(GB *gb) {
  CYC(0x4f6d, 0x4f6e); A = C;
  CYC(0x4f6e, 0x4f71); SET_HL(wOpenedMenuType);
  CYC(0x4f71, 0x4f72); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4f72, 0x4f73); alu_xor(gb, A);
  CYC(0x4f73, 0x4f74); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4f74, 0x4f75); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4f75, 0x4f76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4f76, 0x4f79); W8(wTextIsActive) = A;
  CYC(0x4f79, 0x4f7c); fastFadeoutToWhite_hook(gb);
}

void copyW2TilesetBgPalettesToW4PaletteData_body_hook(GB *gb) {
  CYC(0x4f7c, 0x4f7f); SET_HL(w2TilesetBgPalettes);
  CYC(0x4f7f, 0x4f82); SET_DE(w4PaletteData);
  CYC(0x4f82, 0x4f84); B = 0x80;
  do {
    CYC(0x4f84, 0x4f86); A = 0x02;
    CYC(0x4f86, 0x4f88); hram_wr(gb, 0x70, A);
    CYC(0x4f88, 0x4f89); C = mem_rd(gb, HL);
    CYC(0x4f89, 0x4f8a); L = alu_inc8(gb, L);
    CYC(0x4f8a, 0x4f8c); A = 0x04;
    CYC(0x4f8c, 0x4f8e); hram_wr(gb, 0x70, A);
    CYC(0x4f8e, 0x4f8f); A = C;
    CYC(0x4f8f, 0x4f90); mem_wr(gb, DE, A);
    CYC(0x4f90, 0x4f91); SET_DE(DE + 1);
    CYC(0x4f91, 0x4f92); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x4f92, 0x4f94); else CYC(0x4f92, 0x4f94);
  } while (!(F & FZ));
  CYC(0x4f94, 0x4f96); A = 0xff;
  CYC(0x4f96, 0x4f98); hram_wr(gb, 0xa6, A);
  CYC(0x4f98, 0x4f9a); hram_wr(gb, 0xa7, A);
  CYC(0x4f9a, 0x4f9b); ret_effect(gb);
}

void copyW4PaletteDataToW2TilesetBgPalettes_body_hook(GB *gb) {
  CYC(0x4f9b, 0x4f9e); SET_HL(w4PaletteData);
  CYC(0x4f9e, 0x4fa1); SET_DE(w2TilesetBgPalettes);
  CYC(0x4fa1, 0x4fa3); B = 0x80;
  do {
    CYC(0x4fa3, 0x4fa5); A = 0x04;
    CYC(0x4fa5, 0x4fa7); hram_wr(gb, 0x70, A);
    CYC(0x4fa7, 0x4fa8); C = mem_rd(gb, HL);
    CYC(0x4fa8, 0x4fa9); L = alu_inc8(gb, L);
    CYC(0x4fa9, 0x4fab); A = 0x02;
    CYC(0x4fab, 0x4fad); hram_wr(gb, 0x70, A);
    CYC(0x4fad, 0x4fae); A = C;
    CYC(0x4fae, 0x4faf); mem_wr(gb, DE, A);
    CYC(0x4faf, 0x4fb0); SET_DE(DE + 1);
    CYC(0x4fb0, 0x4fb1); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x4fb1, 0x4fb3); else CYC(0x4fb1, 0x4fb3);
  } while (!(F & FZ));
  CYC(0x4fb3, 0x4fb5); A = 0xff;
  CYC(0x4fb5, 0x4fb7); hram_wr(gb, 0xa6, A);
  CYC(0x4fb7, 0x4fb9); hram_wr(gb, 0xa7, A);
  CYC(0x4fb9, 0x4fba); ret_effect(gb);
}

void closeMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4fba, 0x4fbd); SET_HL(wMenuLoadState);
  CYC(0x4fbd, 0x4fbe); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4fbe, 0x4fc1); A = W8(wOpenedMenuType);
  CYC(0x4fc1, 0x4fc3); alu_cp(gb, 0x03);
  CYC(0x4fc3, 0x4fc5); A = 0x55;
  if (!(F & FZ)) CALL_C_CC(0x4fc5, playSound_b00_hook, 0x0c98, 0x4fc8);
  else CYC(0x4fc5, 0x4fc8);
  CYC(0x4fc8, 0x4fc9); alu_xor(gb, A);
  CYC(0x4fc9, 0x4fcc); W8(wTextIsActive) = A;
  CYC(0x4fcc, 0x4fcf); fastFadeoutToWhite_hook(gb);
}

void playHeartBeepAtInterval_hook(GB *gb) {
  CYC(0x5142, 0x5145); A = W8(w1Link_id);
  CYC(0x5145, 0x5146); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x5146, 0x5147); ret_effect(gb); return; }
  CYC(0x5146, 0x5147);
  CYC(0x5147, 0x514a); A = W8(wFrameCounter);
  CYC(0x514a, 0x514c); alu_and(gb, 0x3f);
  if (!(F & FZ)) { CYCT(0x514c, 0x514d); ret_effect(gb); return; }
  CYC(0x514c, 0x514d);
  CYC(0x514d, 0x5150); SET_HL(wLinkHealth);
  CYC(0x5150, 0x5151); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5151, 0x5152); A = alu_dec8(gb, A);
  CYC(0x5152, 0x5153); alu_add(gb, A);
  if (F & FC) { CYCT(0x5153, 0x5154); ret_effect(gb); return; }
  CYC(0x5153, 0x5154);
  CYC(0x5154, 0x5155); alu_add(gb, A);
  CYC(0x5155, 0x5156); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x5156, 0x5157); ret_effect(gb); return; }
  CYC(0x5156, 0x5157);
  CYC(0x5157, 0x5159); A = 0x60;
  CYC(0x5159, 0x515c); playSound_b00_hook(gb);
}

void loadCommonGraphics_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x515c, disableLcd_hook, 0x02c1, 0x515f);
  CYC(0x515f, 0x5161); A = 0x20;
  CALL_C(0x5161, loadGfxHeader_hook, 0x0626, 0x5164);
  CYC(0x5164, 0x5166); A = 0x83;
  CALL_C(0x5166, loadGfxHeader_hook, 0x0626, 0x5169);
  CYC(0x5169, 0x516a); alu_xor(gb, A);
  CYC(0x516a, 0x516d); W8(wcbe8) = A;
  CALL_C(0x516d, updateStatusBar_body, 0x518d, 0x5170);
  CYC(0x5170, 0x5173); A = W8(wActiveGroup);
  CYC(0x5173, 0x5175); alu_sub(gb, 0x02);
  CYC(0x5175, 0x5177); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x5177, 0x5179); goto done; }
  CYC(0x5177, 0x5179);
  CYC(0x5179, 0x517c); A = W8(wTilesetFlags);
  CYC(0x517c, 0x517e); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(0x517e, 0x5180); goto done; }
  CYC(0x517e, 0x5180);
  CYC(0x5180, 0x5182); A = 0x44;
  CALL_C(0x5182, loadGfxHeader_hook, 0x0626, 0x5185);
  CYC(0x5185, 0x5187); A = 0x34;
  CALL_C(0x5187, loadPaletteHeader_hook, 0x050b, 0x518a);
done:
  CYC(0x518a, 0x518d); checkReloadStatusBarGraphics_hook(gb);
}

void correctAddressForExtraHeart_hook(GB *gb) {
  CYC(0x52cb, 0x52ce); A = W8(wcbe8);
  CYC(0x52ce, 0x52cf); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x52cf, 0x52d0); ret_effect(gb); return; }
  CYC(0x52cf, 0x52d0);
  CYC(0x52d0, 0x52d1); L = alu_dec8(gb, L);
  CYC(0x52d1, 0x52d2); ret_effect(gb);
}

void loadEquippedItemGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52d2, loadStatusBarMap_hook, 0x54df, 0x52d5);
  CYC(0x52d5, 0x52d8); A = W8(wcbe8);
  CYC(0x52d8, 0x52d9); alu_rlca(gb);
  if (F & FC) { CYCT(0x52d9, 0x52da); ret_effect(gb); return; }
  CYC(0x52d9, 0x52da);
  CYC(0x52da, 0x52dd); A = W8(wInventoryB);
  CYC(0x52dd, 0x52e0); SET_DE(wBItemTreasure);
  CALL_C(0x52e0, loadEquippedItemSpriteData_hook, 0x531e, 0x52e3);
  CYC(0x52e3, 0x52e5); E = (uint8_t)w4ItemIconGfx;
  if (F & FC) CALL_C_CC(0x52e5, loadItemIconGfx_hook, 0x54b7, 0x52e8);
  else CYC(0x52e5, 0x52e8);
  CYC(0x52e8, 0x52eb); A = W8(wInventoryA);
  CYC(0x52eb, 0x52ee); SET_DE(wAItemTreasure);
  CALL_C(0x52ee, loadEquippedItemSpriteData_hook, 0x531e, 0x52f1);
  CYC(0x52f1, 0x52f3); E = (uint8_t)(w4ItemIconGfx + 0x40);
  if (F & FC) CALL_C_CC(0x52f3, loadItemIconGfx_hook, 0x54b7, 0x52f6);
  else CYC(0x52f3, 0x52f6);
  func_02_52f6_hook(gb);
}

void func_02_52f6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x52f6, 0x52f9); SET_BC(0x0020);
  CYC(0x52f9, 0x52fc); SET_HL(w4StatusBarAttributeMap + 0x02);
  CYC(0x52fc, 0x52ff); A = W8(wBItemSpriteXOffset);
  CYC(0x52ff, 0x5301); alu_bit(gb, 7, A);
  if (F & FZ) CALL_C_CC(0x5301, func_02_52f6__func1_hook, 0x5313, 0x5304);
  else CYC(0x5301, 0x5304);
  CYC(0x5304, 0x5306); L = (uint8_t)(w4StatusBarAttributeMap + 0x07);
  CYC(0x5306, 0x5309); A = W8(wcbe8);
  CYC(0x5309, 0x530a); alu_rrca(gb);
  if (!(F & FC)) CYCT(0x530a, 0x530c);
  else { CYC(0x530a, 0x530c); CYC(0x530c, 0x530d); L = alu_dec8(gb, L); }
  CYC(0x530d, 0x5310); A = W8(wAItemSpriteXOffset);
  CYC(0x5310, 0x5312); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x5312, 0x5313); ret_effect(gb); return; }
  CYC(0x5312, 0x5313);
  func_02_52f6__func1_hook(gb);
}

void func_02_52f6__func1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5313, 0x5314); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x5314, func_02_52f6__func2_hook, 0x5318, 0x5317);
  else CYC(0x5314, 0x5317);
  CYC(0x5317, 0x5318); L = alu_dec8(gb, L);
  func_02_52f6__func2_hook(gb);
}

void func_02_52f6__func2_hook(GB *gb) {
  CYC(0x5318, 0x5319); D = L;
  CYC(0x5319, 0x531a); mem_wr(gb, HL, B);
  CYC(0x531a, 0x531b); alu_add_hl(gb, BC);
  CYC(0x531b, 0x531c); mem_wr(gb, HL, B);
  CYC(0x531c, 0x531d); L = D;
  CYC(0x531d, 0x531e); ret_effect(gb);
}

void loadEquippedItemSpriteData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x531e, loadTreasureDisplayData_b00_hook, 0x16d6, 0x5321);
  CYC(0x5321, 0x5322); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5322, 0x5323); mem_wr(gb, DE, A);
  CYC(0x5323, 0x5324); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5324, 0x5325); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5325, 0x5327); loadEquippedItemSpriteData__clearItem_hook(gb); return; }
  CYC(0x5325, 0x5327);
  CYC(0x5327, 0x5328); E = alu_inc8(gb, E);
  CYC(0x5328, 0x5329); B = A;
  CYC(0x5329, 0x532b); alu_cp(gb, 0x84);
  CYC(0x532b, 0x532c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FC)) { CYCT(0x532c, 0x532e); goto attr1; }
  CYC(0x532c, 0x532e);
  CYC(0x532e, 0x5330); alu_sub(gb, 0x03);
  CYC(0x5330, 0x5332); alu_or(gb, 0x01);
attr1:
  CYC(0x5332, 0x5334); A |= 0x08;
  CYC(0x5334, 0x5335); mem_wr(gb, DE, A);
  CYC(0x5335, 0x5336); E = alu_inc8(gb, E);
  CYC(0x5336, 0x5337); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5337, 0x5338); alu_or(gb, A);
  CYC(0x5338, 0x5339); C = A;
  if (F & FZ) { CYCT(0x5339, 0x533b); goto attr2; }
  CYC(0x5339, 0x533b);
  CYC(0x533b, 0x533c); alu_scf(gb);
  CYC(0x533c, 0x533d); A = mem_rd(gb, HL);
attr2:
  CYC(0x533d, 0x533e); L = alu_inc8(gb, L);
  CYC(0x533e, 0x5340); A |= 0x08;
  CYC(0x5340, 0x5341); mem_wr(gb, DE, A);
  CYC(0x5341, 0x5342); E = alu_inc8(gb, E);
  CYC(0x5342, 0x5344); A = 0x08;
  if (F & FC) CYCT(0x5344, 0x5346);
  else { CYC(0x5344, 0x5346); CYC(0x5346, 0x5347); alu_xor(gb, A); }
  CYC(0x5347, 0x5348); mem_wr(gb, DE, A);
  CYC(0x5348, 0x5349); E = alu_inc8(gb, E);
  CYC(0x5349, 0x534a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x534a, 0x534b); mem_wr(gb, DE, A);
  CYC(0x534b, 0x534c); alu_scf(gb);
  CYC(0x534c, 0x534d); ret_effect(gb);
}

void loadEquippedItemSpriteData__clearItem_hook(GB *gb) {
  CYC(0x534d, 0x534e); L = E;
  CYC(0x534e, 0x534f); H = D;
  CYC(0x534f, 0x5351); B = 0x05;
  CYC(0x5351, 0x5353); A = 0xff;
  do {
    CYC(0x5353, 0x5354); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x5354, 0x5355); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x5355, 0x5357); else CYC(0x5355, 0x5357);
  } while (!(F & FZ));
  CYC(0x5357, 0x5358); ret_effect(gb);
}

void drawItemTilesOnStatusBar_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5358, 0x535b); A = W8(wcbe8);
  CYC(0x535b, 0x535d); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x535d, 0x535e); ret_effect(gb); return; }
  CYC(0x535d, 0x535e);
  CYC(0x535e, 0x5360); A = 0x04;
  CYC(0x5360, 0x5362); hram_wr(gb, 0x70, A);
  CYC(0x5362, 0x5365); A = W8(wInventoryB);
  CYC(0x5365, 0x5368); SET_DE(wBItemTreasure);
  CALL_C(0x5368, loadEquippedItemSpriteData_hook, 0x531e, 0x536b);
  CYC(0x536b, 0x536e); A = W8(wInventoryA);
  CYC(0x536e, 0x5371); SET_DE(wAItemTreasure);
  CALL_C(0x5371, loadEquippedItemSpriteData_hook, 0x531e, 0x5374);
  CALL_C(0x5374, func_02_52f6_hook, 0x52f6, 0x5377);
  CYC(0x5377, 0x537a); A = W8(wcbe8);
  CYC(0x537a, 0x537b); alu_rrca(gb);
  CYC(0x537b, 0x537e); SET_DE(w4StatusBarTileMap + 0x27);
  if (!(F & FC)) CYCT(0x537e, 0x5380);
  else { CYC(0x537e, 0x5380); CYC(0x5380, 0x5381); E = alu_dec8(gb, E); }
  CYC(0x5381, 0x5384); A = W8(wAItemTreasure);
  CYC(0x5384, 0x5385); B = A;
  CYC(0x5385, 0x5388); A = W8(wAItemDisplayMode);
  CALL_C(0x5388, drawItemTilesOnStatusBar__drawItem_hook, 0x5395, 0x538b);
  CYC(0x538b, 0x538e); SET_DE(w4StatusBarTileMap + 0x22);
  CYC(0x538e, 0x5391); A = W8(wBItemTreasure);
  CYC(0x5391, 0x5392); B = A;
  CYC(0x5392, 0x5395); A = W8(wBItemDisplayMode);
  drawItemTilesOnStatusBar__drawItem_hook(gb);
}

void drawItemTilesOnStatusBar__drawItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5395, 0x5396); C = A;
  CYC(0x5396, 0x5397); alu_rlca(gb);
  if (F & FC) { CYCT(0x5397, 0x5398); ret_effect(gb); return; }
  CYC(0x5397, 0x5398);
  CYC(0x5398, 0x5399); A = B;
  CALL_C(0x5399, checkTreasureObtained_hook, 0x1748, 0x539c);
  CYC(0x539c, 0x539d); B = A;
  CYC(0x539d, 0x539e); A = C;
  CYC(0x539e, 0x53a0); C = 0x80;
  drawTreasureExtraTiles_hook(gb);
}

void drawTreasureExtraTiles_hook(GB *gb) {
  CYC(0x53a0, 0x53a2); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x53a2, 0x53a3); ret_effect(gb); return; }
  CYC(0x53a2, 0x53a3);
  CYC(0x53a3, 0x53a4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x53a4, 0x53a6); drawTreasureExtraTiles__val01_hook(gb); return; }
  CYC(0x53a4, 0x53a6);
  CYC(0x53a6, 0x53a7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x53a7, 0x53a9); drawTreasureExtraTiles__val02_hook(gb); return; }
  CYC(0x53a7, 0x53a9);
  CYC(0x53a9, 0x53aa); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x53aa, 0x53ac); drawTreasureExtraTiles__val03_hook(gb); return; }
  CYC(0x53aa, 0x53ac);
  CYC(0x53ac, 0x53ad); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x53ad, 0x53af); drawTreasureExtraTiles__val04_hook(gb); return; }
  CYC(0x53ad, 0x53af);
  CYC(0x53af, 0x53b1); drawTreasureExtraTiles__val00_hook(gb);
}

void drawTreasureExtraTiles__val04_hook(GB *gb) {
  CYC(0x53b1, 0x53b2); E = alu_inc8(gb, E);
  CYC(0x53b2, 0x53b3); A = B;
  CYC(0x53b3, 0x53b5); alu_and(gb, 0x0f);
  CYC(0x53b5, 0x53b7); alu_add(gb, 0x10);
  CYC(0x53b7, 0x53b8); mem_wr(gb, DE, A);
  CYC(0x53b8, 0x53ba); D |= 0x04;
  CYC(0x53ba, 0x53bb); A = C;
  CYC(0x53bb, 0x53bc); mem_wr(gb, DE, A);
  CYC(0x53bc, 0x53bd); E = alu_dec8(gb, E);
  CYC(0x53bd, 0x53be); mem_wr(gb, DE, A);
  CYC(0x53be, 0x53c0); D &= ~0x04;
  CYC(0x53c0, 0x53c2); A = 0x1b;
  CYC(0x53c2, 0x53c3); mem_wr(gb, DE, A);
  CYC(0x53c3, 0x53c4); ret_effect(gb);
}

void drawTreasureExtraTiles__val01_hook(GB *gb) {
  CYC(0x53c4, 0x53c5); E = alu_inc8(gb, E);
  CYC(0x53c5, 0x53c6); A = B;
  CYC(0x53c6, 0x53c8); alu_and(gb, 0x0f);
  CYC(0x53c8, 0x53ca); alu_add(gb, 0x10);
  CYC(0x53ca, 0x53cb); mem_wr(gb, DE, A);
  CYC(0x53cb, 0x53cd); D |= 0x04;
  CYC(0x53cd, 0x53ce); A = C;
  CYC(0x53ce, 0x53cf); mem_wr(gb, DE, A);
  CYC(0x53cf, 0x53d0); E = alu_dec8(gb, E);
  CYC(0x53d0, 0x53d1); mem_wr(gb, DE, A);
  CYC(0x53d1, 0x53d3); D &= ~0x04;
  CYC(0x53d3, 0x53d4); A = B;
  CYC(0x53d4, 0x53d6); A = alu_swap(gb, A);
  CYC(0x53d6, 0x53d8); alu_and(gb, 0x0f);
  CYC(0x53d8, 0x53da); alu_add(gb, 0x10);
  CYC(0x53da, 0x53db); mem_wr(gb, DE, A);
  CYC(0x53db, 0x53dc); ret_effect(gb);
}

void drawTreasureExtraTiles__val00_hook(GB *gb) {
  CYC(0x53dc, 0x53dd); E = alu_inc8(gb, E);
  CYC(0x53dd, 0x53de); A = B;
  CYC(0x53de, 0x53e0); alu_and(gb, 0x0f);
  CYC(0x53e0, 0x53e2); alu_add(gb, 0x10);
  CYC(0x53e2, 0x53e3); mem_wr(gb, DE, A);
  CYC(0x53e3, 0x53e5); D |= 0x04;
  CYC(0x53e5, 0x53e6); A = C;
  CYC(0x53e6, 0x53e7); mem_wr(gb, DE, A);
  CYC(0x53e7, 0x53e8); E = alu_dec8(gb, E);
  CYC(0x53e8, 0x53e9); mem_wr(gb, DE, A);
  CYC(0x53e9, 0x53eb); D &= ~0x04;
  CYC(0x53eb, 0x53ed); A = 0x1a;
  CYC(0x53ed, 0x53ee); mem_wr(gb, DE, A);
  CYC(0x53ee, 0x53ef); ret_effect(gb);
}

void drawTreasureExtraTiles__val03_hook(GB *gb) {
  CYC(0x53ef, 0x53f0); ret_effect(gb);
}

void drawTreasureExtraTiles__val02_hook(GB *gb) {
  CYC(0x53f0, 0x53f1); H = D;
  CYC(0x53f1, 0x53f2); L = E;
  CYC(0x53f2, 0x53f3); A = C;
  CYC(0x53f3, 0x53f5); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x53f5, 0x53f7); drawTreasureExtraTiles__val02__drawOnInventory_hook(gb); return; }
  CYC(0x53f5, 0x53f7);
  CYC(0x53f7, 0x53f9); A = 0x1f;
  CYC(0x53f9, 0x53fa); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x53fa, 0x53fc); mem_wr(gb, HL, 0x1d);
  CYC(0x53fc, 0x53fe); H |= 0x04;
  CYC(0x53fe, 0x5400); A = 0x80;
  CYC(0x5400, 0x5401); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5401, 0x5402); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5402, 0x5404); mem_wr(gb, HL, 0x00);
  CYC(0x5404, 0x5407); SET_BC(0xffe0);
  CYC(0x5407, 0x5408); alu_add_hl(gb, BC);
  CYC(0x5408, 0x540a); mem_wr(gb, HL, 0x00);
  CYC(0x540a, 0x540b); L = alu_dec8(gb, L);
  CYC(0x540b, 0x540c); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x540c, 0x540d); mem_wr(gb, HL, A);
  CYC(0x540d, 0x540f); H &= ~0x04;
  CYC(0x540f, 0x5411); A = 0x1c;
  CYC(0x5411, 0x5412); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5412, 0x5414); mem_wr(gb, HL, 0x1e);
  CYC(0x5414, 0x5415); ret_effect(gb);
}

void drawTreasureExtraTiles__val02__drawOnInventory_hook(GB *gb) {
  CYC(0x5415, 0x5417); A = 0x1f;
  CYC(0x5417, 0x5418); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5418, 0x541a); mem_wr(gb, HL, 0x1d);
  CYC(0x541a, 0x541c); H |= 0x04;
  CYC(0x541c, 0x541e); A = 0x84;
  CYC(0x541e, 0x541f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x541f, 0x5420); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5420, 0x5423); SET_BC(0xffe0);
  CYC(0x5423, 0x5424); alu_add_hl(gb, BC);
  CYC(0x5424, 0x5425); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5425, 0x5426); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5426, 0x5428); H &= ~0x04;
  CYC(0x5428, 0x542a); A = 0x1c;
  CYC(0x542a, 0x542b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x542b, 0x542d); mem_wr(gb, HL, 0x1e);
  CYC(0x542d, 0x542e); ret_effect(gb);
}

void drawTreasureExtraTiles__drawTile_hook(GB *gb) {
  CYC(0x542e, 0x542f); mem_wr(gb, HL, B);
  CYC(0x542f, 0x5431); H |= 0x04;
  CYC(0x5431, 0x5432); mem_wr(gb, HL, C);
  CYC(0x5432, 0x5434); H &= ~0x04;
  CYC(0x5434, 0x5435); ret_effect(gb);
}

void fileSelectDrawHeartDisplay_hook(GB *gb) {
  CYC(0x5435, 0x5437); A = 0x01;
  CYC(0x5437, 0x5439); hram_wr(gb, 0x8b, A);
  CYC(0x5439, 0x543a); A = B;
  CYC(0x543a, 0x543c); drawHeartDisplay_hook(gb);
}

void inGameDrawHeartDisplay_hook(GB *gb) {
  CYC(0x543c, 0x543f); SET_HL(w4StatusBarTileMap + 0x0d);
  CYC(0x543f, 0x5440); alu_xor(gb, A);
  CYC(0x5440, 0x5442); hram_wr(gb, 0x8b, A);
  CYC(0x5442, 0x5445); A = W8(wDisplayedHearts);
  CYC(0x5445, 0x5446); C = A;
  CYC(0x5446, 0x5449); A = W8(wLinkMaxHealth);
  drawHeartDisplay_hook(gb);
}

void drawHeartDisplay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5449, 0x544b); E = 0x07;
  CYC(0x544b, 0x544d); alu_cp(gb, 0x39);
  if (F & FC) CYCT(0x544d, 0x544f);
  else { CYC(0x544d, 0x544f); CYC(0x544f, 0x5450); E = alu_inc8(gb, E); }
  CYC(0x5450, 0x5452); A = alu_srl(gb, A);
  CYC(0x5452, 0x5454); A = alu_srl(gb, A);
  CYC(0x5454, 0x5455); B = A;
  CYC(0x5455, 0x5456); A = C;
  CYC(0x5456, 0x5458); alu_and(gb, 0x03);
  CYC(0x5458, 0x5459); D = A;
  CYC(0x5459, 0x545a); A = C;
  CYC(0x545a, 0x545c); A = alu_srl(gb, A);
  CYC(0x545c, 0x545e); A = alu_srl(gb, A);
  CYC(0x545e, 0x545f); C = A;
  CYC(0x545f, 0x5460); push_effect(gb, BC);
  CYC(0x5460, 0x5461); alu_cp(gb, E);
  if (F & FC) CYCT(0x5461, 0x5463);
  else { CYC(0x5461, 0x5463); CYC(0x5463, 0x5464); C = E; }
  CYC(0x5464, 0x5465); A = B;
  CYC(0x5465, 0x5466); alu_cp(gb, E);
  if (F & FC) CYCT(0x5466, 0x5468);
  else { CYC(0x5466, 0x5468); CYC(0x5468, 0x5469); A = E; }
  CYC(0x5469, 0x546a); alu_sub(gb, C);
  CYC(0x546a, 0x546b); B = A;
  CYC(0x546b, 0x546d); A = hram_rd(gb, 0x8b);
  CYC(0x546d, 0x546e); alu_or(gb, E);
  CYC(0x546e, 0x546f); alu_rrca(gb);
  if (F & FC) CYCT(0x546f, 0x5471);
  else { CYC(0x546f, 0x5471); CYC(0x5471, 0x5472); L = alu_dec8(gb, L); }
  CYC(0x5472, 0x5473); push_effect(gb, HL);
  CALL_C(0x5473, drawHeartDisplay__drawHeartDisplayRow_hook, 0x548a, 0x5476);
  CYC(0x5476, 0x5477); SET_HL(pop_effect(gb));
  CYC(0x5477, 0x5479); A = 0x20;
  CYC(0x5479, 0x547a); push_effect(gb, 0x547a); add_a_to_hl(gb);
  CYC(0x547a, 0x547b); SET_BC(pop_effect(gb));
  CYC(0x547b, 0x547c); A = C;
  CYC(0x547c, 0x547d); alu_sub(gb, E);
  if (!(F & FC)) CYCT(0x547d, 0x547f);
  else { CYC(0x547d, 0x547f); CYC(0x547f, 0x5480); alu_xor(gb, A); }
  CYC(0x5480, 0x5481); C = A;
  CYC(0x5481, 0x5482); A = B;
  CYC(0x5482, 0x5483); alu_sub(gb, E);
  CYC(0x5483, 0x5484); alu_sub(gb, C);
  CYC(0x5484, 0x5486); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(0x5486, 0x5488);
  else { CYC(0x5486, 0x5488); CYC(0x5488, 0x5489); alu_xor(gb, A); }
  CYC(0x5489, 0x548a); B = A;
  drawHeartDisplay__drawHeartDisplayRow_hook(gb);
}

void drawHeartDisplay__drawHeartDisplayRow_hook(GB *gb) {
  CYC(0x548a, 0x548b); A = C;
  CYC(0x548b, 0x548c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x548c, 0x548e); drawHeartDisplay__partiallyFilledHeart_hook(gb); return; }
  CYC(0x548c, 0x548e);
  drawHeartDisplay__filledHearts_hook(gb);
}

void drawHeartDisplay__filledHearts_hook(GB *gb) {
  CYC(0x548e, 0x5490); A = 0x0f;
  do {
    CYC(0x5490, 0x5491); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x5491, 0x5492); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(0x5492, 0x5494); else CYC(0x5492, 0x5494);
  } while (!(F & FZ));
  drawHeartDisplay__partiallyFilledHeart_hook(gb);
}

void drawHeartDisplay__partiallyFilledHeart_hook(GB *gb) {
  CYC(0x5494, 0x5495); A = B;
  CYC(0x5495, 0x5496); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5496, 0x5498); drawHeartDisplay__fillBlankSpace_hook(gb); return; }
  CYC(0x5496, 0x5498);
  CYC(0x5498, 0x5499); A = D;
  CYC(0x5499, 0x549a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x549a, 0x549c); drawHeartDisplay__unfilledHearts_hook(gb); return; }
  CYC(0x549a, 0x549c);
  CYC(0x549c, 0x549e); alu_add(gb, 0x0b);
  CYC(0x549e, 0x549f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x549f, 0x54a1); D = 0;
  CYC(0x54a1, 0x54a2); B = alu_dec8(gb, B);
  drawHeartDisplay__unfilledHearts_hook(gb);
}

void drawHeartDisplay__unfilledHearts_hook(GB *gb) {
  CYC(0x54a2, 0x54a3); A = B;
  CYC(0x54a3, 0x54a4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x54a4, 0x54a6); drawHeartDisplay__fillBlankSpace_hook(gb); return; }
  CYC(0x54a4, 0x54a6);
  CYC(0x54a6, 0x54a8); A = 0x0b;
  do {
    CYC(0x54a8, 0x54a9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x54a9, 0x54aa); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x54aa, 0x54ac); else CYC(0x54aa, 0x54ac);
  } while (!(F & FZ));
  drawHeartDisplay__fillBlankSpace_hook(gb);
}

void drawHeartDisplay__fillBlankSpace_hook(GB *gb) {
  CYC(0x54ac, 0x54ae); A = hram_rd(gb, 0x8b);
  CYC(0x54ae, 0x54af); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x54af, 0x54b0); ret_effect(gb); return; }
  CYC(0x54af, 0x54b0);
  CYC(0x54b0, 0x54b2); C = 0x08;
  do {
    CYC(0x54b2, 0x54b3); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x54b3, 0x54b4); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(0x54b4, 0x54b6); else CYC(0x54b4, 0x54b6);
  } while (!(F & FZ));
  CYC(0x54b6, 0x54b7); ret_effect(gb);
}

void loadItemIconGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x54b7, 0x54b9); D = (uint8_t)(w4ItemIconGfx >> 8);
  CYC(0x54b9, 0x54ba); push_effect(gb, BC);
  CYC(0x54ba, 0x54bb); A = B;
  CALL_C(0x54bb, loadItemIconGfx__func_hook, 0x54c0, 0x54be);
  CYC(0x54be, 0x54bf); SET_BC(pop_effect(gb));
  CYC(0x54bf, 0x54c0); A = C;
  loadItemIconGfx__func_hook(gb);
}

void loadItemIconGfx__func_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x54c0, 0x54c1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x54c1, 0x54c3); loadItemIconGfx__clear_hook(gb); return; }
  CYC(0x54c1, 0x54c3);
  CYC(0x54c3, 0x54c5); alu_cp(gb, 0xa3);
  if (F & FC) CYCT(0x54c5, 0x54c7);
  else { CYC(0x54c5, 0x54c7); CYC(0x54c7, 0x54c9); alu_add(gb, 0x02); }
  CYC(0x54c9, 0x54ca); alu_add(gb, A);
  CALL_C(0x54ca, multiplyABy16_hook, 0x01ac, 0x54cd);
  CYC(0x54cd, 0x54d0); SET_HL(0x4000);
  CYC(0x54d0, 0x54d1); alu_add_hl(gb, BC);
  CYC(0x54d1, 0x54d3); B = 0x19;
  CYC(0x54d3, 0x54d6); copy20BytesFromBank_hook(gb);
}

void loadItemIconGfx__clear_hook(GB *gb) {
  CYC(0x54d6, 0x54d7); H = D;
  CYC(0x54d7, 0x54d8); L = E;
  CYC(0x54d8, 0x54da); B = 0x20;
  CYC(0x54da, 0x54dc); A = 0xff;
  CYC(0x54dc, 0x54df); fillMemory_hook(gb);
}

void loadStatusBarMap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x54df, 0x54e1); C = 0x10;
  CYC(0x54e1, 0x54e4); A = W8(wLinkMaxHealth);
  CYC(0x54e4, 0x54e6); alu_cp(gb, 0x39);
  if (F & FC) CYCT(0x54e6, 0x54e8);
  else { CYC(0x54e6, 0x54e8); CYC(0x54e8, 0x54e9); C = alu_inc8(gb, C); }
  CYC(0x54e9, 0x54ec); A = W8(wInventoryB);
  CYC(0x54ec, 0x54ee); alu_cp(gb, 0x0c);
  if (!(F & FZ)) CYCT(0x54ee, 0x54f0);
  else { CYC(0x54ee, 0x54f0); CYC(0x54f0, 0x54f2); C |= 0x80; }
  CYC(0x54f2, 0x54f5); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x54f5, 0x54f6); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x54f6, 0x54f7); alu_rrca(gb);
  CYC(0x54f7, 0x54f8); A = C;
  if (F & FC) CYCT(0x54f8, 0x54fa);
  else {
    CYC(0x54f8, 0x54fa);
    CYC(0x54fa, 0x54fb); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x54fb, 0x54fc); ret_effect(gb); return; }
    CYC(0x54fb, 0x54fc);
  }
  CYC(0x54fc, 0x54fd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x54fd, 0x54ff); mem_wr(gb, HL, 0xff);
  CYC(0x54ff, 0x5502); SET_HL(wBItemTreasure);
  CYC(0x5502, 0x5504); B = 0x0a;
  CALL_C(0x5504, clearMemory_hook, 0x046f, 0x5507);
  CYC(0x5507, 0x5509); alu_bit(gb, 7, C);
  CYC(0x5509, 0x550b); A = 0x23;
  if (!(F & FZ)) CYCT(0x550b, 0x550d);
  else {
    CYC(0x550b, 0x550d);
    CYC(0x550d, 0x550e); A = C;
    CYC(0x550e, 0x5510); alu_and(gb, 0x01);
    CYC(0x5510, 0x5512); alu_add(gb, 0x21);
  }
  CYC(0x5512, 0x5515); loadGfxHeader_hook(gb);
}

void runInventoryMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5515, clearOam_hook, 0x049f, 0x5518);
  CYC(0x5518, 0x551a); A = 0x10;
  CYC(0x551a, 0x551c); H8(hOamTail) = A;
  CYC(0x551c, 0x551e); A = 0x04;
  CYC(0x551e, 0x5520); hram_wr(gb, R_SVBK, A);
  CALL_ROM(0x5520, 0x552c);
  CALL_C(0x5523, inventoryMenuDrawSprites_hook, 0x5d73, 0x5526);
  CYC(0x5526, 0x5527); alu_xor(gb, A);
  CYC(0x5527, 0x5529); hram_wr(gb, R_SVBK, A);
  CYC(0x5529, 0x552c); updateStatusBar_hook(gb);
}

void runInventoryMenu__inventoryMenuStates_hook(GB *gb) {
  CYC(0x552c, 0x552f); A = W8(wMenuActiveState);
  CYC(0x552f, 0x5530); push_effect(gb, 0x5530);
  switch (function_caller_jump_table(gb)) {
    case 0x5566: inventoryMenuState0_hook(gb); return;
    case 0x55e9: inventoryMenuState1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void showItemText1_hook(GB *gb) {
  CYC(0x5538, 0x553b); SET_HL(w4SubscreenTextIndices);
  CYC(0x553b, 0x553c); push_effect(gb, 0x553c); add_a_to_hl(gb);
  CYC(0x553c, 0x553d); A = mem_rd(gb, HL);
  showItemText2_hook(gb);
}

void showItemText2_hook(GB *gb) {
  CYC(0x553d, 0x5540); SET_HL(wInventory_activeText);
  CYC(0x5540, 0x5541); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5541, 0x5542); ret_effect(gb); return; }
  CYC(0x5541, 0x5542);
  CYC(0x5542, 0x5543); mem_wr(gb, HL, A);
  CYC(0x5543, 0x5544); C = A;
  CYC(0x5544, 0x5546); B = 0x09;
  CYC(0x5546, 0x5548); alu_bit(gb, 7, C);
  if (F & FZ) { CYCT(0x5548, 0x554a); goto show_text; }
  CYC(0x5548, 0x554a);
  CYC(0x554a, 0x554c); B = 0x30;
  CYC(0x554c, 0x554e); C = 0xc0;
  CYC(0x554e, 0x5550); alu_and(gb, 0x3f);
  CYC(0x5550, 0x5551); L = A;
  CYC(0x5551, 0x5553); alu_add(gb, 0x40);
  CYC(0x5553, 0x5555); alu_bit(gb, 6, C);
  CYC(0x5555, 0x5556); C = A;
  if (F & FZ) { CYCT(0x5556, 0x5558); goto show_text; }
  CYC(0x5556, 0x5558);
  CYC(0x5558, 0x555b); mem_wr(gb, wTextSubstitutions + 2, A);
  CYC(0x555b, 0x555c); A = L;
  CYC(0x555c, 0x555e); alu_add(gb, 0x80);
  CYC(0x555e, 0x5561); mem_wr(gb, wTextSubstitutions + 3, A);
  CYC(0x5561, 0x5563); C = 0xc1;
show_text:
  CYC(0x5563, 0x5566); showTextOnInventoryMenu_hook(gb);
}

void inventoryMenuState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5566, 0x5569); SET_HL(wInventorySubmenu2CursorPos);
  CYC(0x5569, 0x556a); A = mem_rd(gb, HL);
  CYC(0x556a, 0x556c); alu_cp(gb, 0x08);
  if (!(F & FC)) CYCT(0x556c, 0x556e);
  else { CYC(0x556c, 0x556e); CYC(0x556e, 0x5570); mem_wr(gb, HL, 0); }
  CYC(0x5570, 0x5571); alu_xor(gb, A);
  CYC(0x5571, 0x5574); W8(wInventorySubmenu) = A;
  CYC(0x5574, 0x5577); W8(wInventory_cbba) = A;
  CYC(0x5577, 0x557a); W8(wInventory_submenu2CursorPos2) = A;
  CYC(0x557a, 0x557b); A = alu_dec8(gb, A);
  CYC(0x557b, 0x557e); W8(wInventory_activeText) = A;
  CALL_C(0x557e, loadCommonGraphics_hook, 0x1a98, 0x5581);
  CYC(0x5581, 0x5583); A = 0x08;
  CALL_C(0x5583, loadGfxHeader_hook, 0x0626, 0x5586);
  CYC(0x5586, 0x5588); A = 0x06;
  CALL_C(0x5588, loadUncompressedGfxHeader_hook, 0x05da, 0x558b);
  CYC(0x558b, 0x558d); A = 0x0a;
  CALL_C(0x558d, loadPaletteHeader_hook, 0x050b, 0x5590);
  CYC(0x5590, 0x5593); SET_HL(0x4697);
  CYC(0x5593, 0x5595); E = 0x3f;
  CALL_C(0x5595, interBankCall_hook, 0x008a, 0x5598);
  CALL_C(0x5598, func_02_55b2_hook, 0x55b2, 0x559b);
  CYC(0x559b, 0x559d); A = 0x01;
  CYC(0x559d, 0x55a0); W8(wMenuActiveState) = A;
  CALL_C(0x55a0, fastFadeinFromWhite_hook, 0x3290, 0x55a3);
  CYC(0x55a3, 0x55a5); A = 0x03;
  CYC(0x55a5, 0x55a8); loadGfxRegisterStateIndex_hook(gb);
}

void func_02_55a8_hook(GB *gb) {
  CYC(0x55a8, 0x55ab); A = W8(wInventory_cbba);
  CYC(0x55ab, 0x55ad); alu_and(gb, 0x01);
  CYC(0x55ad, 0x55af); alu_add(gb, 0x04);
  CYC(0x55af, 0x55b2); loadUncompressedGfxHeader_hook(gb);
}

void func_02_55b2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55b2, 0x55b5); SET_HL(w4SubscreenTextIndices);
  CYC(0x55b5, 0x55b7); B = 0x20;
  CALL_C(0x55b7, clearMemory_hook, 0x046f, 0x55ba);
  CYC(0x55ba, 0x55bb); alu_xor(gb, A);
  CALL_C(0x55bb, showItemText2_hook, 0x553d, 0x55be);
  CYC(0x55be, 0x55c1); SET_HL(0x55a8);
  CYC(0x55c1, 0x55c2); push_effect(gb, HL);
  CYC(0x55c2, 0x55c5); A = W8(wInventorySubmenu);
  CYC(0x55c5, 0x55c6); push_effect(gb, 0x55c6);
  switch (function_caller_jump_table(gb)) {
    case 0x55cc: func_02_55b2__subScreen0_hook(gb); func_02_55a8_hook(gb); return;
    case 0x55d9: func_02_55b2__subScreen1_hook(gb); func_02_55a8_hook(gb); return;
    case 0x55e1: func_02_55b2__subScreen2_hook(gb); func_02_55a8_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_02_55b2__subScreen0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55cc, 0x55ce); A = 0xff;
  CYC(0x55ce, 0x55d1); W8(wStatusBarNeedsRefresh) = A;
  CYC(0x55d1, 0x55d3); A = 0x09;
  CALL_C(0x55d3, loadGfxHeader_hook, 0x0626, 0x55d6);
  CYC(0x55d6, 0x55d9); inventorySubscreen0_drawStoredItems_hook(gb);
}

void func_02_55b2__subScreen1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55d9, 0x55db); A = 0x0a;
  CALL_C(0x55db, loadGfxHeader_hook, 0x0626, 0x55de);
  CYC(0x55de, 0x55e1); inventorySubscreen1_drawTreasures_hook(gb);
}

void func_02_55b2__subScreen2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55e1, 0x55e3); A = 0x0b;
  CALL_C(0x55e3, loadGfxHeader_hook, 0x0626, 0x55e6);
  CYC(0x55e6, 0x55e9); inventorySubscreen2_drawTreasures_hook(gb);
}

void inventoryMenuState1_hook(GB *gb) {
  CYC(0x55e9, 0x55ec); A = W8(wPaletteThread_mode);
  CYC(0x55ec, 0x55ed); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x55ed, 0x55ee); ret_effect(gb); return; }
  CYC(0x55ed, 0x55ee);
  CYC(0x55ee, 0x55f1); A = W8(wKeysJustPressed);
  CYC(0x55f1, 0x55f3); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(0x55f3, 0x55f6); closeMenu_hook(gb); return; }
  CYC(0x55f3, 0x55f6);
  CYC(0x55f6, 0x55f8); alu_bit(gb, 2, A);
  CYC(0x55f8, 0x55fa); A = 0x03;
  if (!(F & FZ)) { CYCT(0x55fa, 0x55fc); inventoryMenuState1__func_02_5606_hook(gb); return; }
  CYC(0x55fa, 0x55fc);
  CYC(0x55fc, 0x55ff); A = W8(wInventorySubmenu);
  CYC(0x55ff, 0x5600); push_effect(gb, 0x5600);
  switch (function_caller_jump_table(gb)) {
    case 0x560d: inventoryMenuState1__subscreen0_hook(gb); return;
    case 0x56c2: inventoryMenuState1__subscreen1_hook(gb); return;
    case 0x56fb: inventoryMenuState1__subscreen2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void inventoryMenuState1__func_02_5606_hook(GB *gb) {
  CYC(0x5606, 0x5609); SET_HL(wMenuActiveState);
  CYC(0x5609, 0x560a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x560a, 0x560c); mem_wr(gb, HL, 0);
  CYC(0x560c, 0x560d); ret_effect(gb);
}

void inventoryMenuState1__subscreen0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x560d, 0x5610); A = W8(wKeysJustPressed);
  CYC(0x5610, 0x5611); C = A;
  CYC(0x5611, 0x5613); A = (uint8_t)wInventoryB;
  CYC(0x5613, 0x5615); alu_bit(gb, 1, C);
  if (!(F & FZ)) { CYCT(0x5615, 0x5617); inventoryMenuState1__aOrB_hook(gb); return; }
  CYC(0x5615, 0x5617);
  CYC(0x5617, 0x5618); A = alu_inc8(gb, A);
  CYC(0x5618, 0x561a); alu_bit(gb, 0, C);
  if (!(F & FZ)) { CYCT(0x561a, 0x561c); inventoryMenuState1__aOrB_hook(gb); return; }
  CYC(0x561a, 0x561c);
  CALL_C(0x561c, inventorySubscreen0CheckDirectionButtons_hook, 0x5893, 0x561f);
  CYC(0x561f, 0x5622); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x5622, 0x5625); SET_HL(wInventoryStorage);
  CYC(0x5625, 0x5626); push_effect(gb, 0x5626); add_a_to_hl(gb);
  CYC(0x5626, 0x5627); A = mem_rd(gb, HL);
  CALL_C(0x5627, loadTreasureDisplayData_b00_hook, 0x16d6, 0x562a);
  CYC(0x562a, 0x562c); A = 0x06;
  CYC(0x562c, 0x562d); push_effect(gb, 0x562d); add_a_to_hl(gb);
  CYC(0x562d, 0x562e); A = mem_rd(gb, HL);
  CALL_C(0x562e, showItemText2_hook, 0x553d, 0x5631);
  CYC(0x5631, 0x5634); inventorySubscreen0_drawCursor_hook(gb);
}

void inventoryMenuState1__aOrB_hook(GB *gb) {
  CYC(0x5634, 0x5637); W8(wInventory_cbb6) = A;
  CYC(0x5637, 0x563a); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x563a, 0x563d); SET_HL(wInventoryStorage);
  CYC(0x563d, 0x563e); push_effect(gb, 0x563e); add_a_to_hl(gb);
  CYC(0x563e, 0x563f); A = mem_rd(gb, HL);
  CYC(0x563f, 0x5642); W8(wInventory_selectedItem) = A;
  CYC(0x5642, 0x5644); C = 0x1f;
  CYC(0x5644, 0x5646); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(0x5646, 0x5648); inventoryMenuState1__hasSubmenu_hook(gb); return; }
  CYC(0x5646, 0x5648);
  CYC(0x5648, 0x564a); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(0x564a, 0x564c); inventoryMenuState1__hasSubmenu_hook(gb); return; }
  CYC(0x564a, 0x564c);
  CYC(0x564c, 0x564e); alu_cp(gb, 0x11);
  if (!(F & FZ)) { CYCT(0x564e, 0x5650); inventoryMenuState1__finalizeEquip_hook(gb); return; }
  CYC(0x564e, 0x5650);
  CYC(0x5650, 0x5652); C = 0xe0;
  inventoryMenuState1__hasSubmenu_hook(gb);
}

void inventoryMenuState1__hasSubmenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5652, 0x5655); A = mem_rd(gb, wObtainedTreasureFlags + 4);
  CYC(0x5655, 0x5656); alu_and(gb, C);
  CALL_C(0x5656, getNumSetBits_hook, 0x0176, 0x5659);
  CYC(0x5659, 0x565c); W8(wInventory_cbb8) = A;
  CYC(0x565c, 0x565e); alu_cp(gb, 0x02);
  CYC(0x565e, 0x5660); A = 0x02;
  if (!(F & FC)) { CYCT(0x5660, 0x5663); inventoryMenuState1__func_02_5606_hook(gb); return; }
  CYC(0x5660, 0x5663);
  inventoryMenuState1__finalizeEquip_hook(gb);
}

void inventoryMenuState1__finalizeEquip_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5663, inventoryMenuState1__equipItem_hook, 0x5679, 0x5666);
  CALL_C(0x5666, inventorySubscreen0_drawStoredItems_hook, 0x5b5a, 0x5669);
  CALL_C(0x5669, inventorySubscreen0_drawCursor_hook, 0x595e, 0x566c);
  CYC(0x566c, 0x566e); A = 0x56;
  CALL_C(0x566e, playSound_b00_hook, 0x0c98, 0x5671);
  CYC(0x5671, 0x5673); A = 0x01;
  CALL_C(0x5673, inventoryMenuState1__func_02_5606_hook, 0x5606, 0x5676);
  CYC(0x5676, 0x5679); func_02_55b2_hook(gb);
}

void inventoryMenuState1__equipItem_hook(GB *gb) {
  CYC(0x5679, 0x567b); D = (uint8_t)(wInventoryStorage >> 8);
  CYC(0x567b, 0x567c); H = D;
  CYC(0x567c, 0x567f); A = W8(wInventory_cbb6);
  CYC(0x567f, 0x5680); E = A;
  CYC(0x5680, 0x5683); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x5683, 0x5685); alu_add(gb, (uint8_t)wInventoryStorage);
  CYC(0x5685, 0x5686); L = A;
  CYC(0x5686, 0x5688); B = 0x0c;
  CYC(0x5688, 0x5689); A = mem_rd(gb, HL);
  CYC(0x5689, 0x568a); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x568a, 0x568c); inventoryMenuState1__equipItem__equipBiggoron_hook(gb); return; }
  CYC(0x568a, 0x568c);
  CYC(0x568c, 0x568d); A = mem_rd(gb, DE);
  CYC(0x568d, 0x568e); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x568e, 0x5690); inventoryMenuState1__equipItem__swapItems_hook(gb); return; }
  CYC(0x568e, 0x5690);
  inventoryMenuState1__equipItem__unequipBiggoron_hook(gb);
}

void inventoryMenuState1__equipItem__unequipBiggoron_hook(GB *gb) {
  CYC(0x5690, 0x5691); C = L;
  CYC(0x5691, 0x5693); L = (uint8_t)wInventoryB;
  CYC(0x5693, 0x5694); alu_xor(gb, A);
  CYC(0x5694, 0x5695); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5695, 0x5696); mem_wr(gb, HL, A);
  CYC(0x5696, 0x5697); L = C;
  CYC(0x5697, 0x5698); A = B;
  CYC(0x5698, 0x5699); mem_wr(gb, DE, A);
  inventoryMenuState1__equipItem__swapItems_hook(gb);
}

void inventoryMenuState1__equipItem__swapItems_hook(GB *gb) {
  CYC(0x5699, 0x569a); A = mem_rd(gb, DE);
  CYC(0x569a, 0x569b); C = A;
  CYC(0x569b, 0x569c); A = mem_rd(gb, HL);
  CYC(0x569c, 0x569d); mem_wr(gb, DE, A);
  CYC(0x569d, 0x569e); mem_wr(gb, HL, C);
  CYC(0x569e, 0x569f); ret_effect(gb);
}

void inventoryMenuState1__equipItem__equipBiggoron_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x569f, 0x56a1); mem_wr(gb, HL, 0);
  CALL_C(0x56a1, inventoryMenuState1__equipItem__swapItems_hook, 0x5699, 0x56a4);
  CYC(0x56a4, 0x56a7); A = W8(wInventoryB);
  CALL_C(0x56a7, inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook, 0x56b6, 0x56aa);
  CYC(0x56aa, 0x56ad); A = W8(wInventoryA);
  CALL_C(0x56ad, inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook, 0x56b6, 0x56b0);
  CYC(0x56b0, 0x56b2); L = (uint8_t)wInventoryB;
  CYC(0x56b2, 0x56b3); mem_wr(gb, HL, B);
  CYC(0x56b3, 0x56b4); L = alu_inc8(gb, L);
  CYC(0x56b4, 0x56b5); mem_wr(gb, HL, B);
  CYC(0x56b5, 0x56b6); ret_effect(gb);
}

void inventoryMenuState1__equipItem__putItemInFirstBlankSlot_hook(GB *gb) {
  CYC(0x56b6, 0x56b7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x56b7, 0x56b8); ret_effect(gb); return; }
  CYC(0x56b7, 0x56b8);
  CYC(0x56b8, 0x56b9); C = A;
  CYC(0x56b9, 0x56bb); L = (uint8_t)wInventoryStorage;
  do {
    CYC(0x56bb, 0x56bc); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x56bc, 0x56bd); alu_or(gb, A);
    if (!(F & FZ)) CYCT(0x56bd, 0x56bf); else CYC(0x56bd, 0x56bf);
  } while (!(F & FZ));
  CYC(0x56bf, 0x56c0); L = alu_dec8(gb, L);
  CYC(0x56c0, 0x56c1); mem_wr(gb, HL, C);
  CYC(0x56c1, 0x56c2); ret_effect(gb);
}

void inventoryMenuState1__subscreen1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56c2, 0x56c5); A = W8(wKeysJustPressed);
  CYC(0x56c5, 0x56c7); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x56c7, 0x56c9);
    CALL_C(0x56ce, inventoryMenuState1__checkEquipRing_hook, 0x56dd, 0x56d1);
  } else {
    CYC(0x56c7, 0x56c9);
    CALL_C(0x56c9, inventorySubmenu1CheckDirectionButtons_hook, 0x58aa, 0x56cc);
    CYC(0x56cc, 0x56ce);
  }
  CALL_C(0x56d1, inventorySubmenu1_drawCursor_hook, 0x5982, 0x56d4);
  CYC(0x56d4, 0x56d7); A = W8(wInventorySubmenu1CursorPos);
  CALL_C(0x56d7, showItemText1_hook, 0x5538, 0x56da);
  CYC(0x56da, 0x56dd); drawEquippedSpriteForActiveRing_hook(gb);
}

void inventoryMenuState1__checkEquipRing_hook(GB *gb) {
  CYC(0x56dd, 0x56e0); A = W8(wInventorySubmenu1CursorPos);
  CYC(0x56e0, 0x56e2); alu_sub(gb, 0x10);
  if (F & FC) { CYCT(0x56e2, 0x56e3); ret_effect(gb); return; }
  CYC(0x56e2, 0x56e3);
  CYC(0x56e3, 0x56e6); SET_HL(wActiveRing);
  CYC(0x56e6, 0x56e7); C = mem_rd(gb, HL);
  CYC(0x56e7, 0x56e9); L = (uint8_t)wRingBoxContents;
  CYC(0x56e9, 0x56ea); push_effect(gb, 0x56ea); add_a_to_hl(gb);
  CYC(0x56ea, 0x56eb); A = mem_rd(gb, HL);
  CYC(0x56eb, 0x56ec); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x56ec, 0x56ee); goto set_ring; }
  CYC(0x56ec, 0x56ee);
  CYC(0x56ee, 0x56f0); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x56f0, 0x56f1); ret_effect(gb); return; }
  CYC(0x56f0, 0x56f1);
  CYC(0x56f1, 0x56f3); A = 0xff;
set_ring:
  CYC(0x56f3, 0x56f6); W8(wActiveRing) = A;
  CYC(0x56f6, 0x56f8); A = 0x56;
  CYC(0x56f8, 0x56fb); playSound_b00_hook(gb);
}

void inventoryMenuState1__subscreen2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56fb, 0x56fe); A = W8(wKeysJustPressed);
  CYC(0x56fe, 0x5700); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x5700, 0x5702); goto check_direction; }
  CYC(0x5700, 0x5702);
  CYC(0x5702, 0x5705); A = W8(wInventorySubmenu2CursorPos);
  CYC(0x5705, 0x5706); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x5706, 0x5708); goto check_direction; }
  CYC(0x5706, 0x5708);
  CYC(0x5708, 0x570b); A = W8(wInventory_submenu2CursorPos2);
  CYC(0x570b, 0x570d); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x570d, 0x570f); goto check_direction; }
  CYC(0x570d, 0x570f);
  CYC(0x570f, 0x5710); A = alu_inc8(gb, A);
  CYC(0x5710, 0x5713); W8(wOpenedMenuType) = A;
  CYC(0x5713, 0x5715); A = 0x56;
  CALL_C(0x5715, playSound_b00_hook, 0x0c98, 0x5718);
  CYC(0x5718, 0x571b); SET_HL(wInventory);
  CYC(0x571b, 0x571d); B = 0x10;
  CYC(0x571d, 0x5720); clearMemory_hook(gb);
  return;
check_direction:
  CALL_C(0x5720, inventorySubmenu2CheckDirectionButtons_hook, 0x5908, 0x5723);
  CYC(0x5723, 0x5726); A = W8(wInventorySubmenu2CursorPos);
  CYC(0x5726, 0x5728); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(0x5728, 0x572a);
  else {
    CYC(0x5728, 0x572a);
    CYC(0x572a, 0x572d); A = W8(wInventory_submenu2CursorPos2);
    CYC(0x572d, 0x572f); alu_add(gb, 0x08);
  }
  CALL_C(0x572f, showItemText1_hook, 0x5538, 0x5732);
  CYC(0x5732, 0x5735); inventorySubmenu2_drawCursor_hook(gb);
}

void runRingMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d36, clearOam_hook, 0x049f, 0x6d39);
  CYC(0x6d39, 0x6d3b); A = 0x10;
  CYC(0x6d3b, 0x6d3d); H8(hOamTail) = A;
  CYC(0x6d3d, 0x6d40); SET_HL(wTextboxFlags);
  CYC(0x6d40, 0x6d42); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x6d42, 0x6d44); A = 0x04;
  CYC(0x6d44, 0x6d46); hram_wr(gb, R_SVBK, A);
  CALL_ROM(0x6d46, 0x6d51);
  CYC(0x6d49, 0x6d4c); A = W8(wRingMenu_mode);
  CYC(0x6d4c, 0x6d4d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d4d, 0x6d4e); ret_effect(gb); return; }
  CYC(0x6d4d, 0x6d4e);
  CYC(0x6d4e, 0x6d51); updateStatusBar_hook(gb);
}

void runRingMenu__runStateCode_hook(GB *gb) {
  CYC(0x6d51, 0x6d54); A = W8(wMenuActiveState);
  CYC(0x6d54, 0x6d55); push_effect(gb, 0x6d55);
  switch (function_caller_jump_table(gb)) {
    case 0x6d5b: ringMenu_state0_hook(gb); return;
    case 0x6ddd: ringMenu_state1_hook(gb); return;
    case 0x7099: ringMenu_state2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void ringMenu_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d5b, loadCommonGraphics_hook, 0x1a98, 0x6d5e);
  CYC(0x6d5e, 0x6d5f); alu_xor(gb, A);
  CYC(0x6d5f, 0x6d62); W8(wRingMenu_tileMapIndex) = A;
  CYC(0x6d62, 0x6d63); A = alu_dec8(gb, A);
  CYC(0x6d63, 0x6d66); W8(wRingMenu_ringNameTextIndex) = A;
  CYC(0x6d66, 0x6d68); A = 0x80;
  CYC(0x6d68, 0x6d6b); W8(wRingMenu_boxCursorFlickerCounter) = A;
  CYC(0x6d6b, 0x6d6e); A = W8(wRingMenu_mode);
  CYC(0x6d6e, 0x6d70); alu_add(gb, 0x3a);
  CALL_C(0x6d70, loadGfxHeader_hook, 0x0626, 0x6d73);
  CYC(0x6d73, 0x6d75); A = 0x0a;
  CALL_C(0x6d75, loadPaletteHeader_hook, 0x050b, 0x6d78);
  CYC(0x6d78, 0x6d7b); SET_HL(0x466f);
  CYC(0x6d7b, 0x6d7d); E = 0x3f;
  CALL_C(0x6d7d, interBankCall_hook, 0x008a, 0x6d80);
  CALL_C(0x6d80, ringMenu_calculateNumPagesForUnappraisedRings_hook, 0x7223, 0x6d83);
  CALL_C(0x6d83, ringMenu_redrawRingListOrUnappraisedRings_hook, 0x6da8, 0x6d86);
  CYC(0x6d86, 0x6d89); SET_HL(wMenuActiveState);
  CYC(0x6d89, 0x6d8a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x6d8a, fastFadeinFromWhite_hook, 0x3290, 0x6d8d);
  CYC(0x6d8d, 0x6d8f); A = 0x05;
  CYC(0x6d8f, 0x6d91); H8(hNextLcdInterruptBehaviour) = A;
  CYC(0x6d91, 0x6d94); A = W8(wRingMenu_mode);
  CYC(0x6d94, 0x6d96); alu_add(gb, 0x0f);
  CYC(0x6d96, 0x6d99); loadGfxRegisterStateIndex_hook(gb);
}

void ringMenu_copyTilemapToVram_hook(GB *gb) {
  CYC(0x6d99, 0x6d9c); SET_HL(wRingMenu_mode);
  CYC(0x6d9c, 0x6d9f); A = W8(wRingMenu_tileMapIndex);
  CYC(0x6d9f, 0x6da1); alu_and(gb, 0x01);
  CYC(0x6da1, 0x6da2); alu_add(gb, A);
  CYC(0x6da2, 0x6da3); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6da3, 0x6da5); alu_add(gb, 0x12);
  CYC(0x6da5, 0x6da8); loadUncompressedGfxHeader_hook(gb);
}

void ringMenu_redrawRingListOrUnappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6da8, 0x6da9); alu_xor(gb, A);
  CALL_C(0x6da9, showItemText2_hook, 0x553d, 0x6dac);
  CYC(0x6dac, 0x6daf); SET_HL(0x6d99);
  CYC(0x6daf, 0x6db0); push_effect(gb, HL);
  CYC(0x6db0, 0x6db3); A = W8(wRingMenu_mode);
  CYC(0x6db3, 0x6db4); push_effect(gb, 0x6db4);
  switch (function_caller_jump_table(gb)) {
    case 0x6db8: ringMenu_drawRingBox_hook(gb); ringMenu_copyTilemapToVram_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void ringMenu_drawRingBox_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6db8, 0x6dbb); A = W8(wMenuActiveState);
  CYC(0x6dbb, 0x6dbc); alu_or(gb, A);
  if (F & FZ) {
    CYC(0x6dbc, 0x6dbe);
    CYC(0x6dbe, 0x6dc1); A = W8(wRingBoxLevel);
    CYC(0x6dc1, 0x6dc2); A = alu_inc8(gb, A);
    CALL_C(0x6dc2, mapMenu_performTileSubstitutions_hook, 0x5ef3, 0x6dc5);
    CYC(0x6dc5, 0x6dc8); SET_DE(w4TileMap + 0x201);
    CYC(0x6dc8, 0x6dca); A = 0xfe;
    CALL_C(0x6dca, getRingTiles_hook, 0x72fe, 0x6dcd);
  } else {
    CYCT(0x6dbc, 0x6dbe);
  }
  CALL_C(0x6dcd, ringMenu_drawRingBoxContents_hook, 0x7297, 0x6dd0);
  CYC(0x6dd0, 0x6dd2); A = 0x04;
  CYC(0x6dd2, 0x6dd5); W8(wRingMenu_numPages) = A;
  CYC(0x6dd5, 0x6dd7); A = 0xfe;
  CYC(0x6dd7, 0x6dda); W8(wRingMenu_displayedRingNumberComparator) = A;
  CYC(0x6dda, 0x6ddd); ringMenu_drawRingList_hook(gb);
}

void ringMenu_state1_hook(GB *gb) {
  CYC(0x6ddd, 0x6de0); A = W8(wPaletteThread_mode);
  CYC(0x6de0, 0x6de1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6de1, 0x6de2); ret_effect(gb); return; }
  CYC(0x6de1, 0x6de2);
  CYC(0x6de2, 0x6de5); A = W8(wRingMenu_mode);
  CYC(0x6de5, 0x6de6); push_effect(gb, 0x6de6);
  switch (function_caller_jump_table(gb)) {
    case 0x6dea: ringMenu_state1_unappraisedRings_hook(gb); return;
    case 0x6f40: ringMenu_state1_ringList_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void ringMenu_state1_unappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6dea, ringMenu_drawSprites_hook, 0x7175, 0x6ded);
  CYC(0x6ded, 0x6df0); A = W8(wSubmenuState);
  CYC(0x6df0, 0x6df1); push_effect(gb, 0x6df1);
  switch (function_caller_jump_table(gb)) {
    case 0x6dfd: ringMenu_unappraisedRings_state0_hook(gb); return;
    case 0x6e3d: ringMenu_unappraisedRings_state1_hook(gb); return;
    case 0x6e87: ringMenu_unappraisedRings_state2_hook(gb); return;
    case 0x6e99: ringMenu_unappraisedRings_state3_hook(gb); return;
    case 0x6ece: ringMenu_unappraisedRings_state4_hook(gb); return;
    case 0x6f20: ringMenu_unappraisedRings_state5_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void ringMenu_unappraisedRings_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6dfd, 0x6e00); A = W8(wTextIsActive);
  CYC(0x6e00, 0x6e01); alu_or(gb, A);
  CYC(0x6e01, 0x6e03); A = 0x04;
  if (F & FZ) CALL_C_CC(0x6e03, ringMenu_setDisplayedText_hook, 0x735d, 0x6e06);
  else CYC(0x6e03, 0x6e06);
  CYC(0x6e06, 0x6e09); A = W8(wKeysJustPressed);
  CYC(0x6e09, 0x6e0b); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x6e0b, 0x6e0d); ringMenu_unappraisedRings_state0__bPressed_hook(gb); return; }
  CYC(0x6e0b, 0x6e0d);
  CYC(0x6e0d, 0x6e0f); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6e0f, 0x6e11); ringMenu_unappraisedRings_state0__aPressed_hook(gb); return; }
  CYC(0x6e0f, 0x6e11);
  CYC(0x6e11, 0x6e13); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x6e13, 0x6e16); ringMenu_initiateScrollRight_hook(gb); return; }
  CYC(0x6e13, 0x6e16);
  CYC(0x6e16, 0x6e19); ringMenu_checkRingListCursorMoved_hook(gb);
}

void ringMenu_unappraisedRings_state0__bPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e19, ringMenu_checkObtainedRingBox_hook, 0x6f29, 0x6e1c);
  CYC(0x6e1c, 0x6e1e); A = 0x12;
  if (F & FZ) { CYCT(0x6e1e, 0x6e21); ringMenu_setDisplayedText_hook(gb); return; }
  CYC(0x6e1e, 0x6e21);
  CYC(0x6e21, 0x6e24); closeMenu_hook(gb);
}

void ringMenu_unappraisedRings_state0__aPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e24, ringMenu_updateSelectedRingFromList_hook, 0x723b, 0x6e27);
  CALL_C(0x6e27, ringMenu_getUnappraisedRingIndex_hook, 0x6f2e, 0x6e2a);
  CYC(0x6e2a, 0x6e2b); alu_rlca(gb);
  if (F & FC) { CYCT(0x6e2b, 0x6e2c); ret_effect(gb); return; }
  CYC(0x6e2b, 0x6e2c);
  CYC(0x6e2c, 0x6e2e); A = 0x01;
  CYC(0x6e2e, 0x6e31); W8(wSubmenuState) = A;
  CALL_C(0x6e31, ringMenu_checkObtainedRingBox_hook, 0x6f29, 0x6e34);
  CYC(0x6e34, 0x6e36); A = 0x11;
  if (F & FZ) CYCT(0x6e36, 0x6e38);
  else { CYC(0x6e36, 0x6e38); CYC(0x6e38, 0x6e3a); A = 0x05; }
  CYC(0x6e3a, 0x6e3d); ringMenu_setDisplayedText_hook(gb);
}

void ringMenu_unappraisedRings_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e3d, ringMenu_retIfTextIsPrinting_hook, 0x7373, 0x6e40);
  CYC(0x6e40, 0x6e43); A = W8(wSelectedTextOption);
  CYC(0x6e43, 0x6e44); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6e44, 0x6e46); ringMenu_state1_restart_hook(gb); return; }
  CYC(0x6e44, 0x6e46);
  CALL_C(0x6e46, ringMenu_checkObtainedRingBox_hook, 0x6f29, 0x6e49);
  if (F & FZ) CYCT(0x6e49, 0x6e4b);
  else {
    CYC(0x6e49, 0x6e4b);
    CYC(0x6e4b, 0x6e4d); A = 0x05;
    CALL_C(0x6e4d, cpRupeeValue_hook, 0x1765, 0x6e50);
    CYC(0x6e50, 0x6e52); B = 0x06;
    if (!(F & FZ)) { CYCT(0x6e52, 0x6e55); ringMenu_unappraisedRings_gotoState5_hook(gb); return; }
    CYC(0x6e52, 0x6e55);
    CYC(0x6e55, 0x6e57); A = 0x05;
    CALL_C(0x6e57, removeRupeeValue_hook, 0x1778, 0x6e5a);
  }
  CYC(0x6e5a, 0x6e5d); SET_HL(wNumRingsAppraised);
  CALL_C(0x6e5d, incHlRefWithCap_hook, 0x0245, 0x6e60);
  CALL_C(0x6e60, ringMenu_getUnappraisedRingIndex_hook, 0x6f2e, 0x6e63);
  CYC(0x6e63, 0x6e65); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x40);
  CYC(0x6e65, 0x6e66); A = mem_rd(gb, HL);
  CYC(0x6e66, 0x6e69); W8(wRingMenu_textDelayCounter2) = A;
  CYC(0x6e69, 0x6e6b); alu_add(gb, 0x40);
  CYC(0x6e6b, 0x6e6e); mem_wr(gb, wTextSubstitutions + 2, A);
  CYC(0x6e6e, 0x6e71); SET_BC(0x301c);
  CALL_C(0x6e71, ringMenu_showExitableText_hook, 0x6f13, 0x6e74);
  CYC(0x6e74, 0x6e76); A = 0x02;
  CYC(0x6e76, 0x6e79); W8(wSubmenuState) = A;
  CALL_C(0x6e79, ringMenu_drawUnappraisedRings_hook, 0x7255, 0x6e7c);
  CYC(0x6e7c, 0x6e7f); ringMenu_copyTilemapToVram_hook(gb);
}

void ringMenu_state1_restart_hook(GB *gb) {
  CYC(0x6e7f, 0x6e80); alu_xor(gb, A);
  CYC(0x6e80, 0x6e83); W8(wSubmenuState) = A;
  CYC(0x6e83, 0x6e86); W8(wTextIsActive) = A;
  CYC(0x6e86, 0x6e87); ret_effect(gb);
}

void ringMenu_unappraisedRings_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e87, ringMenu_retIfTextIsPrinting_hook, 0x7373, 0x6e8a);
  CYC(0x6e8a, 0x6e8c); A = 0x03;
  CYC(0x6e8c, 0x6e8f); W8(wSubmenuState) = A;
  CALL_C(0x6e8f, ringMenu_getUnappraisedRingIndex_hook, 0x6f2e, 0x6e92);
  CYC(0x6e92, 0x6e94); alu_add(gb, 0x80);
  CYC(0x6e94, 0x6e95); C = A;
  CYC(0x6e95, 0x6e97); B = 0x30;
  CYC(0x6e97, 0x6e99); ringMenu_showExitableText_hook(gb);
}

void ringMenu_unappraisedRings_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e99, ringMenu_retIfTextIsPrinting_hook, 0x7373, 0x6e9c);
  CALL_C(0x6e9c, ringMenu_getUnappraisedRingIndex_hook, 0x6f2e, 0x6e9f);
  CYC(0x6e9f, 0x6ea0); C = A;
  CYC(0x6ea0, 0x6ea2); mem_wr(gb, HL, 0xff);
  CYC(0x6ea2, 0x6ea5); SET_HL(wRingsObtained);
  CALL_C(0x6ea5, checkFlag_hook, 0x0205, 0x6ea8);
  if (!(F & FZ)) { CYCT(0x6ea8, 0x6eaa); ringMenu_unappraisedRings_state3__refund_hook(gb); return; }
  CYC(0x6ea8, 0x6eaa);
  CYC(0x6eaa, 0x6eab); A = C;
  CALL_C(0x6eab, setFlag_hook, 0x020e, 0x6eae);
  CYC(0x6eae, 0x6eaf); alu_xor(gb, A);
  CYC(0x6eaf, 0x6eb1); B = 0x17;
  CYC(0x6eb1, 0x6eb3);
  goto finish;
finish:
  CYC(0x6eb7, 0x6eba); W8(wRingMenu_rupeeRefundValue) = A;
  CALL_C(0x6eba, ringMenu_checkObtainedRingBox_hook, 0x6f29, 0x6ebd);
  if (F & FZ) { CYCT(0x6ebd, 0x6ec0); closeMenu_hook(gb); return; }
  CYC(0x6ebd, 0x6ec0);
  CYC(0x6ec0, 0x6ec2); A = 0x28;
  CYC(0x6ec2, 0x6ec5); W8(wRingMenu_textDelayCounter2) = A;
  CYC(0x6ec5, 0x6ec7); A = 0x04;
  CYC(0x6ec7, 0x6eca); W8(wSubmenuState) = A;
  CYC(0x6eca, 0x6ecb); A = B;
  CYC(0x6ecb, 0x6ece); ringMenu_setDisplayedText_hook(gb);
}

void ringMenu_unappraisedRings_state3__refund_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6eb3, 0x6eb5); A = 0x07;
  CYC(0x6eb5, 0x6eb7); B = 0x07;
  CYC(0x6eb7, 0x6eba); W8(wRingMenu_rupeeRefundValue) = A;
  CALL_C(0x6eba, ringMenu_checkObtainedRingBox_hook, 0x6f29, 0x6ebd);
  if (F & FZ) { CYCT(0x6ebd, 0x6ec0); closeMenu_hook(gb); return; }
  CYC(0x6ebd, 0x6ec0);
  CYC(0x6ec0, 0x6ec2); A = 0x28;
  CYC(0x6ec2, 0x6ec5); W8(wRingMenu_textDelayCounter2) = A;
  CYC(0x6ec5, 0x6ec7); A = 0x04;
  CYC(0x6ec7, 0x6eca); W8(wSubmenuState) = A;
  CYC(0x6eca, 0x6ecb); A = B;
  CYC(0x6ecb, 0x6ece); ringMenu_setDisplayedText_hook(gb);
}

void ringMenu_unappraisedRings_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6ece, ringMenu_retIfTextIsPrinting_hook, 0x7373, 0x6ed1);
  CALL_C(0x6ed1, ringMenu_retIfCounterNotFinished_hook, 0x6f37, 0x6ed4);
  CYC(0x6ed4, 0x6ed7); A = W8(wRingMenu_rupeeRefundValue);
  CYC(0x6ed7, 0x6ed8); alu_or(gb, A);
  CYC(0x6ed8, 0x6ed9); C = A;
  CYC(0x6ed9, 0x6edb); A = 0x28;
  if (!(F & FZ)) CALL_C_CC(0x6edb, giveTreasure_hook, 0x171c, 0x6ede);
  else CYC(0x6edb, 0x6ede);
  CYC(0x6ede, 0x6ee1); SET_HL(0x4697);
  CYC(0x6ee1, 0x6ee3); E = 0x3f;
  CALL_C(0x6ee3, interBankCall_hook, 0x008a, 0x6ee6);
  CALL_C(0x6ee6, ringMenu_drawUnappraisedRings_hook, 0x7255, 0x6ee9);
  CALL_C(0x6ee9, ringMenu_copyTilemapToVram_hook, 0x6d99, 0x6eec);
  CYC(0x6eec, 0x6eef); A = W8(wNumRingsAppraised);
  CYC(0x6eef, 0x6ef1); alu_cp(gb, 100);
  if (!(F & FZ)) { CYCT(0x6ef1, 0x6ef3); ringMenu_unappraisedRings_state4__not100th_hook(gb); return; }
  CYC(0x6ef1, 0x6ef3);
  CYC(0x6ef3, 0x6ef5); A = 0x09;
  CALL_C(0x6ef5, setGlobalFlag_hook, 0x31f9, 0x6ef8);
  CYC(0x6ef8, 0x6efa); B = 0x3c;
  CYC(0x6efa, 0x6efc); ringMenu_unappraisedRings_gotoState5_hook(gb);
}

void ringMenu_unappraisedRings_state4__not100th_hook(GB *gb) {
  CYC(0x6efc, 0x6eff); A = W8(wNumUnappraisedRingsBcd);
  CYC(0x6eff, 0x6f00); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6f00, 0x6f03); ringMenu_state1_restart_hook(gb); return; }
  CYC(0x6f00, 0x6f03);
  CYC(0x6f03, 0x6f05); B = 0x02;
  ringMenu_unappraisedRings_gotoState5_hook(gb);
}

void ringMenu_unappraisedRings_gotoState5_hook(GB *gb) {
  CYC(0x6f05, 0x6f07); A = 0x05;
  CYC(0x6f07, 0x6f0a); W8(wSubmenuState) = A;
  CYC(0x6f0a, 0x6f0c); A = 0x3c;
  CYC(0x6f0c, 0x6f0f); W8(wRingMenu_textDelayCounter2) = A;
  CYC(0x6f0f, 0x6f10); A = B;
  CYC(0x6f10, 0x6f13); ringMenu_setDisplayedText_hook(gb);
}

void ringMenu_showExitableText_hook(GB *gb) {
  CYC(0x6f13, 0x6f15); A = 0x02;
  CYC(0x6f15, 0x6f18); W8(wTextboxPosition) = A;
  CYC(0x6f18, 0x6f1a); A = 0x09;
  CYC(0x6f1a, 0x6f1d); W8(wTextboxFlags) = A;
  CYC(0x6f1d, 0x6f20); showText_hook(gb);
}

void ringMenu_unappraisedRings_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6f20, ringMenu_retIfTextIsPrinting_hook, 0x7373, 0x6f23);
  CALL_C(0x6f23, ringMenu_retIfCounterNotFinished_hook, 0x6f37, 0x6f26);
  CYC(0x6f26, 0x6f29); closeMenu_hook(gb);
}

void ringMenu_checkObtainedRingBox_hook(GB *gb) {
  CYC(0x6f29, 0x6f2b); A = 0x08;
  CYC(0x6f2b, 0x6f2e); checkGlobalFlag_hook(gb);
}

void ringMenu_getUnappraisedRingIndex_hook(GB *gb) {
  CYC(0x6f2e, 0x6f31); A = W8(wRingMenu_selectedRing);
  CYC(0x6f31, 0x6f34); SET_HL(wUnappraisedRings);
  CYC(0x6f34, 0x6f35); push_effect(gb, 0x6f35); add_a_to_hl(gb);
  CYC(0x6f35, 0x6f36); A = mem_rd(gb, HL);
  CYC(0x6f36, 0x6f37); ret_effect(gb);
}

void ringMenu_retIfCounterNotFinished_hook(GB *gb) {
  CYC(0x6f37, 0x6f3a); SET_HL(wRingMenu_textDelayCounter2);
  CYC(0x6f3a, 0x6f3b); A = mem_rd(gb, HL);
  CYC(0x6f3b, 0x6f3c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6f3c, 0x6f3d); ret_effect(gb); return; }
  CYC(0x6f3c, 0x6f3d);
  CYC(0x6f3d, 0x6f3e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6f3e, 0x6f3f); SET_AF(pop_effect(gb));
  CYC(0x6f3f, 0x6f40); ret_effect(gb);
}

void runSecretListMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7474, clearOam_hook, 0x049f, 0x7477);
  CYC(0x7477, 0x7479); A = 0x07;
  CYC(0x7479, 0x747b); hram_wr(gb, R_SVBK, A);
  CALL_C(0x747b, runSecretListMenu__runState_hook, 0x7481, 0x747e);
  CYC(0x747e, 0x7481); secretListMenu_drawCursorSprite_hook(gb);
}

void runSecretListMenu__runState_hook(GB *gb) {
  CYC(0x7481, 0x7484); A = W8(wSecretListMenu_state);
  CYC(0x7484, 0x7485); push_effect(gb, 0x7485);
  switch (function_caller_jump_table(gb)) {
    case 0x748b: secretListMenu_state0_hook(gb); return;
    case 0x74c4: secretListMenu_state1_hook(gb); return;
    case 0x7514: secretListMenu_state2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void secretListMenu_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x748b, disableLcd_hook, 0x02c1, 0x748e);
  CALL_C(0x748e, stopTextThread_hook, 0x184d, 0x7491);
  CYC(0x7491, 0x7493); A = 0x01;
  CYC(0x7493, 0x7496); W8(wSecretListMenu_state) = A;
  CALL_C(0x7496, secretListMenu_state0__clearVramBank_hook, 0x74b7, 0x7499);
  CYC(0x7499, 0x749a); alu_xor(gb, A);
  CALL_C(0x749a, secretListMenu_state0__clearVramBank_hook, 0x74b7, 0x749d);
  CYC(0x749d, 0x749f); A = 0x05;
  CALL_C(0x749f, loadGfxHeader_hook, 0x0626, 0x74a2);
  CYC(0x74a2, 0x74a4); A = 0xa8;
  CALL_C(0x74a4, loadPaletteHeader_hook, 0x050b, 0x74a7);
  CALL_C(0x74a7, secretListMenu_loadAllSecretNames_hook, 0x75ab, 0x74aa);
  CYC(0x74aa, 0x74ac); A = 0xff;
  CALL_C(0x74ac, secretListMenu_printSecret_hook, 0x7544, 0x74af);
  CALL_C(0x74af, fastFadeinFromWhite_hook, 0x3290, 0x74b2);
  CYC(0x74b2, 0x74b4); A = 0x16;
  CYC(0x74b4, 0x74b7); loadGfxRegisterStateIndex_hook(gb);
}

void secretListMenu_state0__clearVramBank_hook(GB *gb) {
  CYC(0x74b7, 0x74b9); hram_wr(gb, R_VBK, A);
  CYC(0x74b9, 0x74bc); SET_HL(0x8000);
  CYC(0x74bc, 0x74bf); SET_BC(0x1000);
  CYC(0x74bf, 0x74c1); A = 0xff;
  CYC(0x74c1, 0x74c4); fillMemoryBc_hook(gb);
}

void secretListMenu_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x74c4, 0x74c7); A = W8(wPaletteThread_mode);
  CYC(0x74c7, 0x74c8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x74c8, 0x74c9); ret_effect(gb); return; }
  CYC(0x74c8, 0x74c9);
  CYC(0x74c9, 0x74cc); A = W8(wKeysJustPressed);
  CYC(0x74cc, 0x74ce); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x74ce, 0x74d1); closeMenu_hook(gb); return; }
  CYC(0x74ce, 0x74d1);
  CALL_C(0x74d1, getInputWithAutofire_hook, 0x0294, 0x74d4);
  CYC(0x74d4, 0x74d5); C = A;
  CYC(0x74d5, 0x74d8); SET_HL(wSecretListMenu_numEntries);
  CYC(0x74d8, 0x74d9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74d9, 0x74da); B = A;
  CYC(0x74da, 0x74dc); A = 0xff;
  CYC(0x74dc, 0x74de); alu_bit(gb, 6, C);
  if (!(F & FZ)) { CYCT(0x74de, 0x74e0); secretListMenu_state1__upOrDown_hook(gb); return; }
  CYC(0x74de, 0x74e0);
  CYC(0x74e0, 0x74e2); alu_bit(gb, 7, C);
  if (F & FZ) { CYCT(0x74e2, 0x74e4); secretListMenu_state1__end_hook(gb); return; }
  CYC(0x74e2, 0x74e4);
  CYC(0x74e4, 0x74e6); A = 0x01;
  secretListMenu_state1__upOrDown_hook(gb);
}

void secretListMenu_state1__upOrDown_hook(GB *gb) {
  CYC(0x74e6, 0x74e7); alu_add(gb, mem_rd(gb, HL));
  CYC(0x74e7, 0x74e8); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x74e8, 0x74ea); secretListMenu_state1__end_hook(gb); return; }
  CYC(0x74e8, 0x74ea);
  CYC(0x74ea, 0x74eb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x74eb, 0x74ec); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x74ec, 0x74ee); alu_cp(gb, 0x01);
  if (F & FC) { CYCT(0x74ee, 0x74f0); secretListMenu_state1__scrollUp_hook(gb); return; }
  CYC(0x74ee, 0x74f0);
  CYC(0x74f0, 0x74f2); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x74f2, 0x74f4); secretListMenu_state1__playSound_hook(gb); return; }
  CYC(0x74f2, 0x74f4);
  secretListMenu_state1__scrollDown_hook(gb);
}

void secretListMenu_state1__scrollDown_hook(GB *gb) {
  CYC(0x74f4, 0x74f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74f5, 0x74f6); alu_sub(gb, B);
  CYC(0x74f6, 0x74f8); alu_cp(gb, 0xfc);
  if (!(F & FC)) { CYCT(0x74f8, 0x74fa); secretListMenu_state1__playSound_hook(gb); return; }
  CYC(0x74f8, 0x74fa);
  CYC(0x74fa, 0x74fc); A = 0x02;
  CYC(0x74fc, 0x74fe);
  CYC(0x7504, 0x7507); W8(wSecretListMenu_scrollSpeed) = A;
  CYC(0x7507, 0x7509); L = (uint8_t)wSecretListMenu_state;
  CYC(0x7509, 0x750a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  secretListMenu_state1__playSound_hook(gb);
}

void secretListMenu_state1__scrollUp_hook(GB *gb) {
  CYC(0x74fe, 0x74ff); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74ff, 0x7500); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7500, 0x7502); secretListMenu_state1__playSound_hook(gb); return; }
  CYC(0x7500, 0x7502);
  CYC(0x7502, 0x7504); A = 0xfe;
  CYC(0x7504, 0x7507); W8(wSecretListMenu_scrollSpeed) = A;
  CYC(0x7507, 0x7509); L = (uint8_t)wSecretListMenu_state;
  CYC(0x7509, 0x750a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  secretListMenu_state1__playSound_hook(gb);
}

void secretListMenu_state1__playSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x750a, 0x750c); A = 0x84;
  CALL_C(0x750c, playSound_b00_hook, 0x0c98, 0x750f);
  secretListMenu_state1__end_hook(gb);
}

void secretListMenu_state1__end_hook(GB *gb) {
  CYC(0x750f, 0x7512); A = W8(wSecretListMenu_cursorIndex);
  CYC(0x7512, 0x7514); secretListMenu_printSecret_hook(gb);
}

void secretListMenu_state2_hook(GB *gb) {
  CYC(0x7514, 0x7517); SET_HL(wSecretListMenu_scrollSpeed);
  CYC(0x7517, 0x751a); A = W8(wGfxRegs2_SCY);
  CYC(0x751a, 0x751b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x751b, 0x751e); W8(wGfxRegs2_SCY) = A;
  CYC(0x751e, 0x7520); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x7520, 0x7521); ret_effect(gb); return; }
  CYC(0x7520, 0x7521);
  CYC(0x7521, 0x7522); A = mem_rd(gb, HL);
  CYC(0x7522, 0x7524); A = alu_sra(gb, A);
  CYC(0x7524, 0x7526); L = (uint8_t)wSecretListMenu_scroll;
  CYC(0x7526, 0x7527); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7527, 0x7528); mem_wr(gb, HL, A);
  CYC(0x7528, 0x752a); L = (uint8_t)wSecretListMenu_state;
  CYC(0x752a, 0x752b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x752b, 0x752c); ret_effect(gb);
}

void secretListMenu_drawCursorSprite_hook(GB *gb) {
  CYC(0x752c, 0x752f); A = W8(wGfxRegs2_SCY);
  CYC(0x752f, 0x7530); B = A;
  CYC(0x7530, 0x7533); A = W8(wSecretListMenu_cursorIndex);
  CYC(0x7533, 0x7535); A = alu_swap(gb, A);
  CYC(0x7535, 0x7536); alu_sub(gb, B);
  CYC(0x7536, 0x7537); B = A;
  CYC(0x7537, 0x7539); C = 0;
  CYC(0x7539, 0x753c); SET_HL(0x753f);
  CYC(0x753c, 0x753f); addSpritesToOam_withOffset_hook(gb);
}

void secretListMenu_printSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
restart:
  CYC(0x7544, 0x7547); SET_HL(wTmpcbb9);
  CYC(0x7547, 0x7548); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7548, 0x7549); ret_effect(gb); return; }
  CYC(0x7548, 0x7549);
  CYC(0x7549, 0x754a); mem_wr(gb, HL, A);
  CYC(0x754a, 0x754b); push_effect(gb, AF);
  CYC(0x754b, 0x754e); SET_HL(w7d800);
  CYC(0x754e, 0x7551); SET_BC(0x0300);
  CALL_C(0x7551, clearMemoryBc_hook, 0x0475, 0x7554);
  CYC(0x7554, 0x7557); SET_HL(w7SecretText1);
  CYC(0x7557, 0x7559); B = 0x18;
  CALL_C(0x7559, clearMemory_hook, 0x046f, 0x755c);
  CYC(0x755c, 0x755d); SET_AF(pop_effect(gb));
  CYC(0x755d, 0x755f); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x755f, 0x7561); secretListMenu_printSecret__end_hook(gb); return; }
  CYC(0x755f, 0x7561);
  CALL_C(0x7561, secretListMenu_getSecretData_hook, 0x75ec, 0x7564);
  CYC(0x7564, 0x7565); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7565, 0x7566); alu_rlca(gb);
  CYC(0x7566, 0x7567); alu_rlca(gb);
  CYC(0x7567, 0x7569); alu_and(gb, 0x03);
  CYC(0x7569, 0x756a); B = A;
  CYC(0x756a, 0x756b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x756b, 0x756c); C = mem_rd(gb, HL);
  CALL_C(0x756c, checkGlobalFlag_hook, 0x31f3, 0x756f);
  CYC(0x756f, 0x7571); A = 0xff;
  CYC(0x7571, 0x7574); W8(wFileSelect_fontXor) = A;
  if (F & FZ) { CYCT(0x7574, 0x7576); goto restart; }
  CYC(0x7574, 0x7576);
  CALL_ROM(0x7576, 0x7589);
  CYC(0x7579, 0x757c); SET_HL(w7SecretText1);
  CYC(0x757c, 0x757f); SET_DE(w7d800);
  CYC(0x757f, 0x7581); B = 0x18;
  CALL_C(0x7581, copyTextCharactersFromHl, 0x4110, 0x7584);
  secretListMenu_printSecret__end_hook(gb);
}

void secretListMenu_printSecret__end_hook(GB *gb) {
  CYC(0x7584, 0x7586); A = 0x35;
  CYC(0x7586, 0x7589); loadUncompressedGfxHeader_hook(gb);
}

void secretListMenu_printSecret__getSecretText_hook(GB *gb) {
  CYC(0x7589, 0x758a); A = B;
  CYC(0x758a, 0x758b); push_effect(gb, 0x758b);
  switch (function_caller_jump_table(gb)) {
    case 0x7593: secretListMenu_printSecret__val0_hook(gb); return;
    case 0x759b: secretListMenu_printSecret__val2_hook(gb); return;
    case 0x75a1: secretListMenu_printSecret__val3_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void secretListMenu_printSecret__val0_hook(GB *gb) {
  CYC(0x7593, 0x7596); SET_HL(0x481b);
  CYC(0x7596, 0x7598); E = 0x03;
  CYC(0x7598, 0x759b); interBankCall_hook(gb);
}

void secretListMenu_printSecret__val2_hook(GB *gb) {
  CYC(0x759b, 0x759e); SET_BC(0x0002);
  CYC(0x759e, 0x75a1); secretFunctionCaller_hook(gb);
}

void secretListMenu_printSecret__val3_hook(GB *gb) {
  CYC(0x75a1, 0x75a2); A = C;
  CYC(0x75a2, 0x75a5); W8(wShortSecretIndex) = A;
  CYC(0x75a5, 0x75a6); C = B;
  CYC(0x75a6, 0x75a8); B = 0;
  CYC(0x75a8, 0x75ab); secretFunctionCaller_hook(gb);
}

void secretListMenu_loadAllSecretNames_hook(GB *gb) {
  CYC(0x75ab, 0x75ac); alu_xor(gb, A);
  CYC(0x75ac, 0x75ae); hram_wr(gb, R_VBK, A);
  CYC(0x75ae, 0x75b1); SET_DE(0x8a00);
  CYC(0x75b1, 0x75b3); B = 0;
  secretListMenu_loadAllSecretNames__nextSecret_hook(gb);
}

void secretListMenu_loadAllSecretNames__nextSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x75b3, 0x75b4); A = B;
    CALL_C(0x75b4, secretListMenu_getSecretData_hook, 0x75ec, 0x75b7);
    CYC(0x75b7, 0x75b8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x75b8, 0x75b9); alu_or(gb, A);
    if (F & FZ) { CYCT(0x75b9, 0x75bb); secretListMenu_loadAllSecretNames__end_hook(gb); return; }
    CYC(0x75b9, 0x75bb);
    CYC(0x75bb, 0x75bc); push_effect(gb, BC);
    CYC(0x75bc, 0x75bd); C = A;
    CYC(0x75bd, 0x75be); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(0x75be, checkGlobalFlag_hook, 0x31f3, 0x75c1);
    CYC(0x75c1, 0x75c3); A = 0x01;
    if (F & FZ) {
      CYCT(0x75c3, 0x75c5);
    } else {
      CYC(0x75c3, 0x75c5);
      CYC(0x75c5, 0x75c6); A = C;
      CYC(0x75c6, 0x75c8); alu_and(gb, 0x3f);
      CALL_C(0x75c8, copyTextCharactersFromSecretTextTable, 0x4107, 0x75cb);
      CYC(0x75cb, 0x75cd); A = 0x02;
    }
    CALL_C(0x75cd, copyTextCharactersFromSecretTextTable, 0x4107, 0x75d0);
    CYC(0x75d0, 0x75d1); SET_BC(pop_effect(gb));
    CYC(0x75d1, 0x75d2); SET_DE(DE - 1);
    CYC(0x75d2, 0x75d4); E = 0;
    CYC(0x75d4, 0x75d5); A = D;
    CYC(0x75d5, 0x75d7); alu_and(gb, 0xfe);
    CYC(0x75d7, 0x75d9); alu_add(gb, 0x02);
    CYC(0x75d9, 0x75db); alu_cp(gb, 0x90);
    if (F & FC) {
      CYCT(0x75db, 0x75dd);
    } else {
      CYC(0x75db, 0x75dd);
      CYC(0x75dd, 0x75df); A = 0x01;
      CYC(0x75df, 0x75e1); hram_wr(gb, R_VBK, A);
      CYC(0x75e1, 0x75e3); A = 0x80;
    }
    CYC(0x75e3, 0x75e4); D = A;
    CYC(0x75e4, 0x75e5); B = alu_inc8(gb, B);
    CYC(0x75e5, 0x75e7);
  }
}

void secretListMenu_loadAllSecretNames__end_hook(GB *gb) {
  CYC(0x75e7, 0x75e8); A = B;
  CYC(0x75e8, 0x75eb); W8(wSecretListMenu_numEntries) = A;
  CYC(0x75eb, 0x75ec); ret_effect(gb);
}

void secretListMenu_getSecretData_hook(GB *gb) {
  CYC(0x75ec, 0x75ef); SET_HL(wFileIsLinkedGame);
  CYC(0x75ef, 0x75f1); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x75f1, 0x75f4); SET_HL(0x7600);
  if (F & FZ) {
    CYCT(0x75f4, 0x75f6);
  } else {
    CYC(0x75f4, 0x75f6);
    CYC(0x75f6, 0x75f9); SET_HL(0x761f);
  }
  CYC(0x75f9, 0x75fa); push_effect(gb, BC);
  CYC(0x75fa, 0x75fb); C = A;
  CYC(0x75fb, 0x75fc); alu_add(gb, A);
  CYC(0x75fc, 0x75fd); alu_add(gb, C);
  CYC(0x75fd, 0x75fe); push_effect(gb, 0x75fe); add_a_to_hl(gb);
  CYC(0x75fe, 0x75ff); SET_BC(pop_effect(gb));
  CYC(0x75ff, 0x7600); ret_effect(gb);
}

void ringMenu_state1_ringList_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6f40, ringMenu_drawRingBoxCursor_hook, 0x71d1, 0x6f43);
  CALL_C(0x6f43, ringMenu_drawEquippedRingSprite_hook, 0x71ac, 0x6f46);
  CALL_C(0x6f46, ringMenu_drawSpritesForRingsInBox_hook, 0x71ef, 0x6f49);
  CYC(0x6f49, 0x6f4c); A = W8(wSubmenuState);
  CYC(0x6f4c, 0x6f4d); push_effect(gb, 0x6f4d);
  switch (function_caller_jump_table(gb)) {
    case 0x6f51: ringMenu_ringList_substate0_hook(gb); return;
    case 0x6fa8: ringMenu_ringList_substate1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void ringMenu_ringList_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f51, 0x6f54); A = W8(wRingMenu_boxCursorFlickerCounter);
  CYC(0x6f54, 0x6f55); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6f55, 0x6f57); ringMenu_ringList_substate0__aPressed_hook(gb); return; }
  CYC(0x6f55, 0x6f57);
  CYC(0x6f57, 0x6f5a); SET_HL(wRingMenu_textDelayCounter);
  CYC(0x6f5a, 0x6f5b); A = mem_rd(gb, HL);
  CYC(0x6f5b, 0x6f5c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6f5c, 0x6f5e); goto display_ring_text; }
  CYC(0x6f5c, 0x6f5e);
  CYC(0x6f5e, 0x6f5f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6f5f, 0x6f61); ringMenu_ringList_substate0__checkInput_hook(gb);
  return;
display_ring_text:
  CYC(0x6f61, 0x6f64); A = W8(wRingMenu_ringBoxCursorIndex);
  CYC(0x6f64, 0x6f67); SET_HL(wRingBoxContents);
  CYC(0x6f67, 0x6f68); push_effect(gb, 0x6f68); add_a_to_hl(gb);
  CYC(0x6f68, 0x6f69); A = mem_rd(gb, HL);
  CYC(0x6f69, 0x6f6c); W8(wRingMenu_selectedRing) = A;
  CALL_C(0x6f6c, ringMenu_updateDisplayedRingNumberWithGivenComparator_hook, 0x733d, 0x6f6f);
  CALL_C(0x6f6f, ringMenu_updateRingText_hook, 0x6fc8, 0x6f72);
  ringMenu_ringList_substate0__checkInput_hook(gb);
}

void ringMenu_ringList_substate0__checkInput_hook(GB *gb) {
  CYC(0x6f72, 0x6f75); A = W8(wKeysJustPressed);
  CYC(0x6f75, 0x6f77); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x6f77, 0x6f79); ringMenu_ringList_substate0__bPressed_hook(gb); return; }
  CYC(0x6f77, 0x6f79);
  CYC(0x6f79, 0x6f7b); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x6f7b, 0x6f7e); ringMenu_checkRingBoxCursorMoved_hook(gb); return; }
  CYC(0x6f7b, 0x6f7e);
  ringMenu_ringList_substate0__aPressed_hook(gb);
}

void ringMenu_ringList_substate0__aPressed_hook(GB *gb) {
  CYC(0x6f7e, 0x6f7f); alu_xor(gb, A);
  CYC(0x6f7f, 0x6f82); W8(wRingMenu_boxCursorFlickerCounter) = A;
  CYC(0x6f82, 0x6f83); A = alu_inc8(gb, A);
  CYC(0x6f83, 0x6f86); W8(wSubmenuState) = A;
  CYC(0x6f86, 0x6f88); A = 0x80;
  CYC(0x6f88, 0x6f8b); W8(wRingMenu_displayedRingNumberComparator) = A;
  CYC(0x6f8b, 0x6f8d); A = 0xff;
  CYC(0x6f8d, 0x6f90); W8(wRingMenu_descriptionTextIndex) = A;
  CYC(0x6f90, 0x6f91); ret_effect(gb);
}

void ringMenu_ringList_substate0__bPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f91, 0x6f94); A = W8(wActiveRing);
  CALL_C(0x6f94, ringMenu_checkRingIsInBox_hook, 0x7056, 0x6f97);
  if (!(F & FC)) { CYCT(0x6f97, 0x6f99); goto close_menu; }
  CYC(0x6f97, 0x6f99);
  CYC(0x6f99, 0x6f9b); A = 0xff;
  CYC(0x6f9b, 0x6f9e); W8(wActiveRing) = A;
close_menu:
  CYC(0x6f9e, 0x6f9f); alu_xor(gb, A);
  CYC(0x6f9f, 0x6fa2); W8(wTextIsActive) = A;
  CYC(0x6fa2, 0x6fa5); W8(wTextboxFlags) = A;
  CYC(0x6fa5, 0x6fa8); closeMenu_hook(gb);
}

void ringMenu_ringList_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6fa8, 0x6fab); A = W8(wKeysJustPressed);
  CYC(0x6fab, 0x6fad); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6fad, 0x6faf); ringMenu_selectedRingFromList_hook(gb); return; }
  CYC(0x6fad, 0x6faf);
  CYC(0x6faf, 0x6fb1); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x6fb1, 0x6fb4); ringMenu_moveCursorToRingBox_hook(gb); return; }
  CYC(0x6fb1, 0x6fb4);
  CYC(0x6fb4, 0x6fb6); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x6fb6, 0x6fb9); ringMenu_initiateScrollRight_hook(gb); return; }
  CYC(0x6fb6, 0x6fb9);
  CALL_C(0x6fb9, ringMenu_checkRingListCursorMoved_hook, 0x711e, 0x6fbc);
  CALL_C(0x6fbc, ringMenu_updateSelectedRingFromList_hook, 0x723b, 0x6fbf);
  CALL_C(0x6fbf, ringMenu_updateDisplayedRingNumber_hook, 0x733a, 0x6fc2);
  CALL_C(0x6fc2, ringMenu_drawSprites_hook, 0x7175, 0x6fc5);
  CALL_C(0x6fc5, ringMenu_retIfCounterNotFinished_hook, 0x6f37, 0x6fc8);
  ringMenu_updateRingText_hook(gb);
}

void ringMenu_updateRingText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6fc8, 0x6fcb); A = W8(wRingMenu_selectedRing);
  CYC(0x6fcb, 0x6fcc); C = A;
  CYC(0x6fcc, 0x6fcf); SET_HL(wRingsObtained);
  CALL_C(0x6fcf, checkFlag_hook, 0x0205, 0x6fd2);
  if (F & FZ) { CYCT(0x6fd2, 0x6fd4); goto name_done; }
  CYC(0x6fd2, 0x6fd4);
  CYC(0x6fd4, 0x6fd5); A = C;
  CYC(0x6fd5, 0x6fd7); alu_or(gb, 0x80);
name_done:
  CYC(0x6fd7, 0x6fda); SET_HL(wRingMenu_ringNameTextIndex);
  CYC(0x6fda, 0x6fdb); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6fdb, 0x6fdd); goto description; }
  CYC(0x6fdb, 0x6fdd);
  CALL_C(0x6fdd, showItemText2_hook, 0x553d, 0x6fe0);
  CYC(0x6fe0, 0x6fe2); A = 0x01;
  CYC(0x6fe2, 0x6fe5); W8(wRingMenu_textDelayCounter) = A;
  CYC(0x6fe5, 0x6fe6); ret_effect(gb);
  return;
description:
  CYC(0x6fe6, 0x6fe9); A = W8(wRingMenu_selectedRing);
  CYC(0x6fe9, 0x6fea); C = A;
  CYC(0x6fea, 0x6fec); alu_cp(gb, 0xff);
  CYC(0x6fec, 0x6fee); A = 0xc0;
  if (F & FZ) { CYCT(0x6fee, 0x6ff0); ringMenu_updateRingText__printDescription_hook(gb); return; }
  CYC(0x6fee, 0x6ff0);
  CYC(0x6ff0, 0x6ff1); A = C;
  CYC(0x6ff1, 0x6ff4); SET_HL(wRingsObtained);
  CALL_C(0x6ff4, checkFlag_hook, 0x0205, 0x6ff7);
  CYC(0x6ff7, 0x6ff9); A = 0xc0;
  if (F & FZ) { CYCT(0x6ff9, 0x6ffb); ringMenu_updateRingText__printDescription_hook(gb); return; }
  CYC(0x6ff9, 0x6ffb);
  CYC(0x6ffb, 0x6ffc); A = C;
  CYC(0x6ffc, 0x6ffe); alu_add(gb, 0x80);
  ringMenu_updateRingText__printDescription_hook(gb);
}

void ringMenu_updateRingText__printDescription_hook(GB *gb) {
  CYC(0x6ffe, 0x7001); SET_HL(wRingMenu_descriptionTextIndex);
  CYC(0x7001, 0x7002); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7002, 0x7003); ret_effect(gb); return; }
  CYC(0x7002, 0x7003);
  CYC(0x7003, 0x7004); mem_wr(gb, HL, A);
  CYC(0x7004, 0x7005); C = A;
  CYC(0x7005, 0x7007); B = 0x30;
  CYC(0x7007, 0x7009); A = 0x04;
  CYC(0x7009, 0x700c); W8(wTextboxPosition) = A;
  CYC(0x700c, 0x700e); A = 0x09;
  CYC(0x700e, 0x7011); W8(wTextboxFlags) = A;
  CYC(0x7011, 0x7014); showTextNonExitable_hook(gb);
}

void ringMenu_selectedRingFromList_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7014, 0x7016); A = 0x56;
  CALL_C(0x7016, playSound_b00_hook, 0x0c98, 0x7019);
  CALL_C(0x7019, ringMenu_updateSelectedRingFromList_hook, 0x723b, 0x701c);
  CYC(0x701c, 0x701d); C = A;
  CYC(0x701d, 0x7020); SET_HL(wRingsObtained);
  CALL_C(0x7020, checkFlag_hook, 0x0205, 0x7023);
  if (!(F & FZ)) { CYCT(0x7023, 0x7025); goto put_ring_in_box; }
  CYC(0x7023, 0x7025);
  CYC(0x7025, 0x7027); C = 0xff;
put_ring_in_box:
  CYC(0x7027, 0x702a); A = W8(wRingMenu_ringBoxCursorIndex);
  CYC(0x702a, 0x702b); B = A;
  CYC(0x702b, 0x702c); A = C;
  CALL_C(0x702c, ringMenu_checkRingIsInBox_hook, 0x7056, 0x702f);
  if (F & FC) { CYCT(0x702f, 0x7031); goto write_ring; }
  CYC(0x702f, 0x7031);
  CYC(0x7031, 0x7033); mem_wr(gb, HL, 0xff);
  CYC(0x7033, 0x7034); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x7034, 0x7036); ringMenu_moveCursorToRingBox_hook(gb); return; }
  CYC(0x7034, 0x7036);
write_ring:
  CYC(0x7036, 0x7037); A = B;
  CYC(0x7037, 0x703a); SET_HL(wRingBoxContents);
  CYC(0x703a, 0x703b); push_effect(gb, 0x703b); add_a_to_hl(gb);
  CYC(0x703b, 0x703c); mem_wr(gb, HL, C);
  ringMenu_moveCursorToRingBox_hook(gb);
}

void ringMenu_moveCursorToRingBox_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x703c, 0x703d); alu_xor(gb, A);
  CYC(0x703d, 0x7040); W8(wSubmenuState) = A;
  CYC(0x7040, 0x7042); A = 0x80;
  CYC(0x7042, 0x7045); W8(wRingMenu_boxCursorFlickerCounter) = A;
  CYC(0x7045, 0x7047); A = 0xff;
  CYC(0x7047, 0x704a); W8(wTextIsActive) = A;
  CYC(0x704a, 0x704d); W8(wRingMenu_ringNameTextIndex) = A;
  CYC(0x704d, 0x7050); W8(wRingMenu_descriptionTextIndex) = A;
  CALL_C(0x7050, ringMenu_drawRingBoxContents_hook, 0x7297, 0x7053);
  CYC(0x7053, 0x7056); ringMenu_copyTilemapToVram_hook(gb);
}

void ringMenu_checkRingIsInBox_hook(GB *gb) {
  CYC(0x7056, 0x7057); push_effect(gb, BC);
  CYC(0x7057, 0x705a); SET_HL(wRingBoxContents + 4);
  CYC(0x705a, 0x705c); B = 0x05;
  ringMenu_checkRingIsInBox__nextRing_hook(gb);
}

void ringMenu_checkRingIsInBox__nextRing_hook(GB *gb) {
  for (;;) {
    CYC(0x705c, 0x705d); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x705d, 0x705f); ringMenu_checkRingIsInBox__foundRing_hook(gb); return; }
    CYC(0x705d, 0x705f);
    CYC(0x705f, 0x7060); L = alu_dec8(gb, L);
    CYC(0x7060, 0x7061); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x7061, 0x7063); continue; }
    CYC(0x7061, 0x7063);
    CYC(0x7063, 0x7064); SET_BC(pop_effect(gb));
    CYC(0x7064, 0x7065); alu_scf(gb);
    CYC(0x7065, 0x7066); ret_effect(gb);
    return;
  }
}

void ringMenu_checkRingIsInBox__foundRing_hook(GB *gb) {
  CYC(0x7066, 0x7067); B = alu_dec8(gb, B);
  CYC(0x7067, 0x7068); A = B;
  CYC(0x7068, 0x7069); SET_BC(pop_effect(gb));
  CYC(0x7069, 0x706a); ret_effect(gb);
}

void ringMenu_initiateScrollRight_hook(GB *gb) {
  CYC(0x706a, 0x706c); A = 0x01;
  CYC(0x706c, 0x706f); W8(wRingMenu_scrollDirection) = A;
  CYC(0x706f, 0x7072); W8(wRingMenu_displayedRingNumberComparator) = A;
  CYC(0x7072, 0x7073); alu_xor(gb, A);
  CYC(0x7073, 0x7076); W8(wRingMenu_ringListCursorIndex) = A;
  CYC(0x7076, 0x7079); A = W8(wRingMenu_page);
  CYC(0x7079, 0x707a); A = alu_inc8(gb, A);
  ringMenu_initiateScroll_hook(gb);
}

void ringMenu_initiateScroll_hook(GB *gb) {
  CYC(0x707a, 0x707d); SET_HL(wRingMenu_numPages);
  CYC(0x707d, 0x707e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x707e, 0x7080); goto set_page; }
  CYC(0x707e, 0x7080);
  CYC(0x7080, 0x7082); A = 0x01;
  CYC(0x7082, 0x7083); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7083, 0x7084); ret_effect(gb); return; }
  CYC(0x7083, 0x7084);
  CYC(0x7084, 0x7085); A = alu_dec8(gb, A);
set_page:
  CYC(0x7085, 0x7088); W8(wRingMenu_page) = A;
  CYC(0x7088, 0x708a); A = 0x02;
  ringMenu_setState_hook(gb);
}

void ringMenu_setState_hook(GB *gb) {
  CYC(0x708a, 0x708d); SET_HL(wMenuActiveState);
  CYC(0x708d, 0x708e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x708e, 0x708f); alu_xor(gb, A);
  CYC(0x708f, 0x7090); mem_wr(gb, HL, A);
  CYC(0x7090, 0x7093); W8(wTextIsActive) = A;
  CYC(0x7093, 0x7095); A = 0xff;
  CYC(0x7095, 0x7098); W8(wRingMenu_descriptionTextIndex) = A;
  CYC(0x7098, 0x7099); ret_effect(gb);
}

void ringMenu_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7099, 0x709c); A = W8(wRingMenu_mode);
  CYC(0x709c, 0x709d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x709d, 0x709f); goto dispatch; }
  CYC(0x709d, 0x709f);
  CALL_C(0x709f, ringMenu_drawRingBoxCursor_hook, 0x71d1, 0x70a2);
  CALL_C(0x70a2, ringMenu_drawEquippedRingSprite_hook, 0x71ac, 0x70a5);
dispatch:
  CYC(0x70a5, 0x70a8); A = W8(wSubmenuState);
  CYC(0x70a8, 0x70a9); push_effect(gb, 0x70a9);
  switch (function_caller_jump_table(gb)) {
    case 0x70ad: ringMenu_state2__substate0_hook(gb); return;
    case 0x70da: ringMenu_state2__substate1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void ringMenu_state2__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x70ad, 0x70b0); SET_HL(wRingMenu_tileMapIndex);
  CYC(0x70b0, 0x70b1); A = mem_rd(gb, HL);
  CYC(0x70b1, 0x70b3); alu_xor(gb, 0x01);
  CYC(0x70b3, 0x70b4); mem_wr(gb, HL, A);
  CALL_C(0x70b4, ringMenu_redrawRingListOrUnappraisedRings_hook, 0x6da8, 0x70b7);
  CYC(0x70b7, 0x70ba); A = W8(wRingMenu_scrollDirection);
  CYC(0x70ba, 0x70bc); alu_bit(gb, 7, A);
  CYC(0x70bc, 0x70be); A = 0x9f;
  if (F & FZ) { CYCT(0x70be, 0x70c0); goto set_window_x; }
  CYC(0x70be, 0x70c0);
  CYC(0x70c0, 0x70c3); SET_HL(wGfxRegs2_LCDC);
  CYC(0x70c3, 0x70c4); A = mem_rd(gb, HL);
  CYC(0x70c4, 0x70c6); alu_xor(gb, 0x48);
  CYC(0x70c6, 0x70c7); mem_wr(gb, HL, A);
  CYC(0x70c7, 0x70c9); A = 0x98;
  CYC(0x70c9, 0x70cc); W8(wGfxRegs2_SCX) = A;
  CYC(0x70cc, 0x70ce); A = 0x07;
set_window_x:
  CYC(0x70ce, 0x70d1); W8(wGfxRegs2_WINX) = A;
  CYC(0x70d1, 0x70d4); SET_HL(wSubmenuState);
  CYC(0x70d4, 0x70d5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x70d5, 0x70d7); A = 0x54;
  CYC(0x70d7, 0x70da); playSound_b00_hook(gb);
}

void ringMenu_state2__substate1_hook(GB *gb) {
  CYC(0x70da, 0x70dd); SET_BC(0x089f);
  CYC(0x70dd, 0x70e0); SET_HL(wGfxRegs2_WINX);
  CYC(0x70e0, 0x70e3); SET_DE(wGfxRegs2_SCX);
  CYC(0x70e3, 0x70e6); A = W8(wRingMenu_scrollDirection);
  CYC(0x70e6, 0x70e8); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x70e8, 0x70ea); ringMenu_state2__scrollRight_hook(gb); return; }
  CYC(0x70e8, 0x70ea);
  ringMenu_state2__scrollLeft_hook(gb);
}

void ringMenu_state2__scrollLeft_hook(GB *gb) {
  CYC(0x70ea, 0x70eb); A = mem_rd(gb, HL);
  CYC(0x70eb, 0x70ec); alu_add(gb, B);
  CYC(0x70ec, 0x70ed); alu_cp(gb, C);
  if (F & FC) { CYCT(0x70ed, 0x70ef); goto store_left; }
  CYC(0x70ed, 0x70ef);
  CYC(0x70ef, 0x70f0); A = C;
store_left:
  CYC(0x70f0, 0x70f1); mem_wr(gb, HL, A);
  CYC(0x70f1, 0x70f2); A = mem_rd(gb, DE);
  CYC(0x70f2, 0x70f3); alu_sub(gb, B);
  CYC(0x70f3, 0x70f4); mem_wr(gb, DE, A);
  CYC(0x70f4, 0x70f6); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(0x70f6, 0x70f7); ret_effect(gb); return; }
  CYC(0x70f6, 0x70f7);
  CYC(0x70f7, 0x70f9); ringMenu_state2__doneScrolling_hook(gb);
}

void ringMenu_state2__scrollRight_hook(GB *gb) {
  CYC(0x70f9, 0x70fa); A = mem_rd(gb, HL);
  CYC(0x70fa, 0x70fb); alu_sub(gb, B);
  CYC(0x70fb, 0x70fd); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(0x70fd, 0x70ff); goto store_right; }
  CYC(0x70fd, 0x70ff);
  CYC(0x70ff, 0x7101); A = 0x07;
store_right:
  CYC(0x7101, 0x7102); mem_wr(gb, HL, A);
  CYC(0x7102, 0x7103); A = mem_rd(gb, DE);
  CYC(0x7103, 0x7104); alu_add(gb, B);
  CYC(0x7104, 0x7105); mem_wr(gb, DE, A);
  CYC(0x7105, 0x7107); alu_cp(gb, 0x98);
  if (F & FC) { CYCT(0x7107, 0x7108); ret_effect(gb); return; }
  CYC(0x7107, 0x7108);
  CYC(0x7108, 0x710b); A = W8(wGfxRegs2_LCDC);
  CYC(0x710b, 0x710d); alu_xor(gb, 0x48);
  CYC(0x710d, 0x7110); W8(wGfxRegs2_LCDC) = A;
  ringMenu_state2__doneScrolling_hook(gb);
}

void ringMenu_state2__doneScrolling_hook(GB *gb) {
  CYC(0x7110, 0x7112); A = 0xc7;
  CYC(0x7112, 0x7115); W8(wGfxRegs2_WINX) = A;
  CYC(0x7115, 0x7116); alu_xor(gb, A);
  CYC(0x7116, 0x7119); W8(wGfxRegs2_SCX) = A;
  CYC(0x7119, 0x711b); A = 0x01;
  CYC(0x711b, 0x711e); ringMenu_setState_hook(gb);
}

void ringMenu_checkRingListCursorMoved_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x711e, 0x7121); SET_HL(0x7155);
  CALL_C(0x7121, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x7124);
  if (!(F & FC)) { CYCT(0x7124, 0x7125); ret_effect(gb); return; }
  CYC(0x7124, 0x7125);
  CYC(0x7125, 0x7126); C = A;
  CYC(0x7126, 0x7129); SET_HL(wRingMenu_ringListCursorIndex);
  CYC(0x7129, 0x712a); E = A;
  CYC(0x712a, 0x712b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x712b, 0x712c); B = A;
  CYC(0x712c, 0x712e); alu_and(gb, 0x0f);
  CYC(0x712e, 0x712f); mem_wr(gb, HL, A);
  CYC(0x712f, 0x7131); alu_bit(gb, 0, C);
  if (F & FZ) { CYCT(0x7131, 0x7133); ringMenu_checkRingListCursorMoved__playSound_hook(gb); return; }
  CYC(0x7131, 0x7133);
  CYC(0x7133, 0x7135); alu_bit(gb, 4, B);
  if (F & FZ) { CYCT(0x7135, 0x7137); ringMenu_checkRingListCursorMoved__playSound_hook(gb); return; }
  CYC(0x7135, 0x7137);
  CYC(0x7137, 0x7138); A = E;
  CYC(0x7138, 0x713b); W8(wRingMenu_scrollDirection) = A;
  CYC(0x713b, 0x713e); A = W8(wRingMenu_page);
  CYC(0x713e, 0x713f); alu_add(gb, E);
  CYC(0x713f, 0x7141); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x7141, 0x7143); goto initiate_scroll; }
  CYC(0x7141, 0x7143);
  CYC(0x7143, 0x7146); A = W8(wRingMenu_numPages);
  CYC(0x7146, 0x7148); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x7148, 0x714a); ringMenu_checkRingListCursorMoved__playSound_hook(gb); return; }
  CYC(0x7148, 0x714a);
  CYC(0x714a, 0x714b); A = alu_dec8(gb, A);
initiate_scroll:
  CALL_C(0x714b, ringMenu_initiateScroll_hook, 0x707a, 0x714e);
  ringMenu_checkRingListCursorMoved__playSound_hook(gb);
}

void ringMenu_checkRingListCursorMoved__playSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x714e, 0x7150); A = 0x84;
  CALL_C(0x7150, playSound_b00_hook, 0x0c98, 0x7153);
  CYC(0x7153, 0x7154); alu_scf(gb);
  CYC(0x7154, 0x7155); ret_effect(gb);
}

void ringMenu_checkRingBoxCursorMoved_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7159, getRingBoxCapacity_hook, 0x5cf5, 0x715c);
  CYC(0x715c, 0x715d); E = A;
  CYC(0x715d, 0x7160); SET_HL(0x7171);
  CALL_C(0x7160, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x7163);
  if (!(F & FC)) { CYCT(0x7163, 0x7164); ret_effect(gb); return; }
  CYC(0x7163, 0x7164);
  if (F & FZ) { CYCT(0x7164, 0x7165); ret_effect(gb); return; }
  CYC(0x7164, 0x7165);
  CYC(0x7165, 0x7168); SET_HL(wRingMenu_ringBoxCursorIndex);
  CYC(0x7168, 0x7169); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7169, 0x716a); alu_cp(gb, E);
  if (!(F & FC)) { CYCT(0x716a, 0x716b); ret_effect(gb); return; }
  CYC(0x716a, 0x716b);
  CYC(0x716b, 0x716c); mem_wr(gb, HL, A);
  CYC(0x716c, 0x716e); A = 0x84;
  CYC(0x716e, 0x7171); playSound_b00_hook(gb);
}

void ringMenu_drawSprites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7175, 0x7178); A = W8(wRingMenu_numPages);
  CYC(0x7178, 0x7179); A = alu_dec8(gb, A);
  CYC(0x7179, 0x717c); SET_HL(0x71a3);
  if (!(F & FZ)) CALL_C_CC(0x717c, addSpritesToOam_hook, 0x0d5e, 0x717f);
  else CYC(0x717c, 0x717f);
  CYC(0x717f, 0x7182); SET_HL(wRingMenu_listCursorFlickerCounter);
  CYC(0x7182, 0x7183); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7183, 0x7185); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7185, 0x7186); ret_effect(gb); return; }
  CYC(0x7185, 0x7186);
  CYC(0x7186, 0x7189); SET_BC(0x3e20);
  CYC(0x7189, 0x718c); A = W8(wRingMenu_ringListCursorIndex);
  CYC(0x718c, 0x718e); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(0x718e, 0x7190); goto position_cursor; }
  CYC(0x718e, 0x7190);
  CYC(0x7190, 0x7192); B = 0x56;
position_cursor:
  CYC(0x7192, 0x7194); alu_and(gb, 0x07);
  CYC(0x7194, 0x7196); A = alu_swap(gb, A);
  CYC(0x7196, 0x7197); alu_add(gb, C);
  CYC(0x7197, 0x7198); C = A;
  CYC(0x7198, 0x719b); SET_HL(0x719e);
  CYC(0x719b, 0x719e); addSpritesToOam_withOffset_hook(gb);
}

void ringMenu_drawEquippedRingSprite_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x71ac, 0x71af); A = W8(wActiveRing);
  CYC(0x71af, 0x71b1); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x71b1, 0x71b2); ret_effect(gb); return; }
  CYC(0x71b1, 0x71b2);
  CALL_C(0x71b2, ringMenu_checkRingIsInBox_hook, 0x7056, 0x71b5);
  if (F & FC) { CYCT(0x71b5, 0x71b6); ret_effect(gb); return; }
  CYC(0x71b5, 0x71b6);
  CALL_C(0x71b6, ringMenu_getSpriteOffsetForRingBoxPosition_hook, 0x71c4, 0x71b9);
  CYC(0x71b9, 0x71bc); SET_HL(0x71bf);
  CYC(0x71bc, 0x71bf); addSpritesToOam_withOffset_hook(gb);
}

void ringMenu_getSpriteOffsetForRingBoxPosition_hook(GB *gb) {
  CYC(0x71c4, 0x71c7); SET_HL(0x71cc);
  CYC(0x71c7, 0x71c8); push_effect(gb, 0x71c8); add_a_to_hl(gb);
  CYC(0x71c8, 0x71c9); C = mem_rd(gb, HL);
  CYC(0x71c9, 0x71cb); B = 0;
  CYC(0x71cb, 0x71cc); ret_effect(gb);
}

void ringMenu_drawRingBoxCursor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x71d1, 0x71d4); SET_HL(wRingMenu_boxCursorFlickerCounter);
  CYC(0x71d4, 0x71d6); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x71d6, 0x71d8); goto draw_cursor; }
  CYC(0x71d6, 0x71d8);
  CYC(0x71d8, 0x71d9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x71d9, 0x71db); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x10);
  CYC(0x71db, 0x71dd); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x71dd, 0x71de); ret_effect(gb); return; }
  CYC(0x71dd, 0x71de);
draw_cursor:
  CYC(0x71de, 0x71e1); A = W8(wRingMenu_ringBoxCursorIndex);
  CALL_C(0x71e1, ringMenu_getSpriteOffsetForRingBoxPosition_hook, 0x71c4, 0x71e4);
  CYC(0x71e4, 0x71e7); SET_HL(0x71ea);
  CYC(0x71e7, 0x71ea); addSpritesToOam_withOffset_hook(gb);
}

void ringMenu_drawSpritesForRingsInBox_hook(GB *gb) {
  CYC(0x71ef, 0x71f1); A = 0x05;
  ringMenu_drawSpritesForRingsInBox__loop_hook(gb);
}

void ringMenu_drawSpritesForRingsInBox__loop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x71f1, 0x71f2); push_effect(gb, AF);
    CYC(0x71f2, 0x71f5); SET_HL(wRingBoxContents - 1);
    CYC(0x71f5, 0x71f6); push_effect(gb, 0x71f6); add_a_to_hl(gb);
    CYC(0x71f6, 0x71f9); A = W8(wRingMenu_page);
    CYC(0x71f9, 0x71fb); A = alu_swap(gb, A);
    CYC(0x71fb, 0x71fc); C = A;
    CYC(0x71fc, 0x71fd); A = mem_rd(gb, HL);
    CYC(0x71fd, 0x71ff); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(0x71ff, 0x7201); ringMenu_drawSpritesForRingsInBox__nextRing_hook(gb); return; }
    CYC(0x71ff, 0x7201);
    CYC(0x7201, 0x7202); alu_sub(gb, C);
    CYC(0x7202, 0x7204); alu_cp(gb, 0x10);
    if (!(F & FC)) { CYCT(0x7204, 0x7206); ringMenu_drawSpritesForRingsInBox__nextRing_hook(gb); return; }
    CYC(0x7204, 0x7206);
    CYC(0x7206, 0x7208); B = 0x30;
    CYC(0x7208, 0x720a); alu_bit(gb, 3, A);
    if (F & FZ) { CYCT(0x720a, 0x720c); goto draw_sprite; }
    CYC(0x720a, 0x720c);
    CYC(0x720c, 0x720e); B = 0x48;
draw_sprite:
    CYC(0x720e, 0x7210); alu_and(gb, 0x07);
    CYC(0x7210, 0x7212); A = alu_swap(gb, A);
    CYC(0x7212, 0x7213); C = A;
    CYC(0x7213, 0x7216); SET_HL(0x721e);
    CALL_C(0x7216, addSpritesToOam_withOffset_hook, 0x0d61, 0x7219);
    CYC(0x7219, 0x721a); SET_AF(pop_effect(gb));
    CYC(0x721a, 0x721b); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(0x721b, 0x721d); continue; }
    CYC(0x721b, 0x721d);
    CYC(0x721d, 0x721e); ret_effect(gb);
    return;
  }
}

void ringMenu_drawSpritesForRingsInBox__nextRing_hook(GB *gb) {
  CYC(0x7219, 0x721a); SET_AF(pop_effect(gb));
  CYC(0x721a, 0x721b); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x721b, 0x721d); ringMenu_drawSpritesForRingsInBox__loop_hook(gb); return; }
  CYC(0x721b, 0x721d);
  CYC(0x721d, 0x721e); ret_effect(gb);
}

void ringMenu_calculateNumPagesForUnappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7223, 0x7226); SET_HL(0x4697);
  CYC(0x7226, 0x7228); E = 0x3f;
  CALL_C(0x7228, interBankCall_hook, 0x008a, 0x722b);
  CYC(0x722b, 0x722e); A = W8(wNumUnappraisedRingsBcd);
  CYC(0x722e, 0x722f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x722f, 0x7230); ret_effect(gb); return; }
  CYC(0x722f, 0x7230);
  CYC(0x7230, 0x7231); A = B;
  CYC(0x7231, 0x7232); A = alu_dec8(gb, A);
  CYC(0x7232, 0x7234); A = alu_swap(gb, A);
  CYC(0x7234, 0x7236); alu_and(gb, 0x0f);
  CYC(0x7236, 0x7237); A = alu_inc8(gb, A);
  CYC(0x7237, 0x723a); W8(wRingMenu_numPages) = A;
  CYC(0x723a, 0x723b); ret_effect(gb);
}

void ringMenu_updateSelectedRingFromList_hook(GB *gb) {
  CYC(0x723b, 0x723e); A = W8(wRingMenu_page);
  CYC(0x723e, 0x7240); A = alu_swap(gb, A);
  CYC(0x7240, 0x7241); C = A;
  CYC(0x7241, 0x7244); A = W8(wRingMenu_ringListCursorIndex);
  CYC(0x7244, 0x7245); alu_add(gb, C);
  CYC(0x7245, 0x7248); W8(wRingMenu_selectedRing) = A;
  CYC(0x7248, 0x7249); ret_effect(gb);
}

void ringMenu_clearRingSelectionArea_hook(GB *gb) {
  CYC(0x7249, 0x724c); SET_HL(w4TileMap + 0x040);
  CYC(0x724c, 0x724f); SET_BC(0x0514);
  CYC(0x724f, 0x7252); SET_DE(0x0007);
  CYC(0x7252, 0x7255); fillRectangleInTilemap_hook(gb);
}

void ringMenu_drawUnappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7255, ringMenu_clearRingSelectionArea_hook, 0x7249, 0x7258);
  CYC(0x7258, 0x725a); B = 0x10;
  CYC(0x725a, 0x725d); A = W8(wRingMenu_page);
  CYC(0x725d, 0x725f); A = alu_swap(gb, A);
  CYC(0x725f, 0x7262); SET_HL(wUnappraisedRings);
  CYC(0x7262, 0x7263); push_effect(gb, 0x7263); add_a_to_hl(gb);
  ringMenu_drawUnappraisedRings__nextRing_hook(gb);
}

void ringMenu_drawUnappraisedRings__nextRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x7263, 0x7264); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x7264, 0x7265); C = A;
    CALL_C(0x7265, ringMenu_drawRing_hook, 0x72c3, 0x7268);
    CYC(0x7268, 0x7269); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x7269, 0x726b); continue; }
    CYC(0x7269, 0x726b);
    CYC(0x726b, 0x726d); ringMenu_drawPageCounter_hook(gb);
    return;
  }
}

void ringMenu_drawRingList_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x726d, ringMenu_clearRingSelectionArea_hook, 0x7249, 0x7270);
  CYC(0x7270, 0x7272); B = 0x10;
  CYC(0x7272, 0x7275); A = W8(wRingMenu_page);
  CYC(0x7275, 0x7277); A = alu_swap(gb, A);
  CYC(0x7277, 0x7278); C = A;
  ringMenu_drawRingList__nextRing_hook(gb);
}

void ringMenu_drawRingList__nextRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x7278, 0x7279); A = C;
    CYC(0x7279, 0x727c); SET_HL(wRingsObtained);
    CALL_C(0x727c, checkFlag_hook, 0x0205, 0x727f);
    if (!(F & FZ)) CALL_C_CC(0x727f, ringMenu_drawRing_hook, 0x72c3, 0x7282);
    else CYC(0x727f, 0x7282);
    CYC(0x7282, 0x7283); C = alu_inc8(gb, C);
    CYC(0x7283, 0x7284); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x7284, 0x7286); continue; }
    CYC(0x7284, 0x7286);
    ringMenu_drawPageCounter_hook(gb);
    return;
  }
}

void ringMenu_drawPageCounter_hook(GB *gb) {
  CYC(0x7286, 0x7289); SET_HL(w4TileMap + 0x10f);
  CYC(0x7289, 0x728c); A = W8(wRingMenu_page);
  CYC(0x728c, 0x728e); alu_add(gb, 0x11);
  CYC(0x728e, 0x728f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x728f, 0x7290); L = alu_inc8(gb, L);
  CYC(0x7290, 0x7293); A = W8(wRingMenu_numPages);
  CYC(0x7293, 0x7295); alu_add(gb, 0x10);
  CYC(0x7295, 0x7296); mem_wr(gb, HL, A);
  CYC(0x7296, 0x7297); ret_effect(gb);
}

void ringMenu_drawRingBoxContents_hook(GB *gb) {
  CYC(0x7297, 0x729a); SET_HL(wRingBoxContents);
  CYC(0x729a, 0x729c); B = 0x11;
  ringMenu_drawRingBoxContents__nextRing_hook(gb);
}

void ringMenu_drawRingBoxContents__nextRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x729c, 0x729d); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x729d, 0x729f); alu_cp(gb, 0xff);
    if (!(F & FZ)) { CYCT(0x729f, 0x72a1); ringMenu_drawRingBoxContents__drawRing_hook(gb); return; }
    CYC(0x729f, 0x72a1);
    CYC(0x72a1, 0x72a2); push_effect(gb, HL);
    CYC(0x72a2, 0x72a3); push_effect(gb, BC);
    CYC(0x72a3, 0x72a4); A = B;
    CYC(0x72a4, 0x72a7); SET_HL(0x72d2);
    CYC(0x72a7, 0x72a8); add_double_index_to_hl(gb, 0x72a8);
    CYC(0x72a8, 0x72a9); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x72a9, 0x72aa); H = mem_rd(gb, HL);
    CYC(0x72aa, 0x72ab); L = A;
    CYC(0x72ab, 0x72ae); SET_BC(0x0202);
    CYC(0x72ae, 0x72b1); SET_DE(0x0007);
    CALL_C(0x72b1, fillRectangleInTilemap_hook, 0x5d08, 0x72b4);
    CYC(0x72b4, 0x72b5); SET_BC(pop_effect(gb));
    CYC(0x72b5, 0x72b6); SET_HL(pop_effect(gb));
    CYC(0x72b6, 0x72b8);
    CYC(0x72bc, 0x72bd); B = alu_inc8(gb, B);
    CYC(0x72bd, 0x72be); A = L;
    CYC(0x72be, 0x72c0); alu_cp(gb, (uint8_t)(wRingBoxContents + 5));
    if (F & FC) { CYCT(0x72c0, 0x72c2); continue; }
    CYC(0x72c0, 0x72c2);
    CYC(0x72c2, 0x72c3); ret_effect(gb);
    return;
  }
}

void ringMenu_drawRingBoxContents__drawRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x72b8, 0x72b9); C = A;
  CALL_C(0x72b9, ringMenu_drawRing_hook, 0x72c3, 0x72bc);
  CYC(0x72bc, 0x72bd); B = alu_inc8(gb, B);
  CYC(0x72bd, 0x72be); A = L;
  CYC(0x72be, 0x72c0); alu_cp(gb, (uint8_t)(wRingBoxContents + 5));
  if (F & FC) { CYCT(0x72c0, 0x72c2); ringMenu_drawRingBoxContents__nextRing_hook(gb); return; }
  CYC(0x72c0, 0x72c2);
  CYC(0x72c2, 0x72c3); ret_effect(gb);
}

void ringMenu_drawRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x72c3, 0x72c4); push_effect(gb, BC);
  CYC(0x72c4, 0x72c5); push_effect(gb, HL);
  CYC(0x72c5, 0x72c6); A = B;
  CYC(0x72c6, 0x72c9); SET_HL(0x72d2);
  CYC(0x72c9, 0x72ca); add_double_index_to_hl(gb, 0x72ca);
  CYC(0x72ca, 0x72cb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x72cb, 0x72cc); D = mem_rd(gb, HL);
  CYC(0x72cc, 0x72cd); E = A;
  CYC(0x72cd, 0x72ce); A = C;
  CALL_C(0x72ce, getRingTiles_hook, 0x72fe, 0x72d1);
  CYC(0x72d1, 0x72d2); SET_HL(pop_effect(gb));
  CYC(0x72d2, 0x72d3); SET_BC(pop_effect(gb));
  CYC(0x72d3, 0x72d4); ret_effect(gb);
}

void getRingTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x72fe, 0x7300); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x7300, 0x7301); ret_effect(gb); return; }
  CYC(0x7300, 0x7301);
  CYC(0x7301, 0x7303); alu_bit(gb, 6, A);
  if (F & FZ) CYCT(0x7303, 0x7305);
  else {
    CYC(0x7303, 0x7305);
    CYC(0x7305, 0x7307); alu_cp(gb, 0xfe);
    CYC(0x7307, 0x7309); A = 0x40;
    if (!(F & FZ)) CYCT(0x7309, 0x730b);
    else {
      CYC(0x7309, 0x730b);
      CYC(0x730b, 0x730e); A = W8(wRingBoxLevel);
      CYC(0x730e, 0x7310); alu_add(gb, 0x40);
      CYC(0x7310, 0x7312);
    }
  }
  CALL_C(0x7312, multiplyABy8_hook, 0x01b7, 0x7315);
  CYC(0x7315, 0x7318); SET_HL(0x57a0);
  CYC(0x7318, 0x7319); alu_add_hl(gb, BC);
  CYC(0x7319, 0x731a); push_effect(gb, DE);
  CALL_C(0x731a, copy8BytesFromRingMapToCec0_hook, 0x1af7, 0x731d);
  CYC(0x731d, 0x731e); SET_HL(pop_effect(gb));
  CYC(0x731e, 0x7321); SET_DE(wTmpcec0);
  CALL_C(0x7321, getRingTiles__drawTile_hook, 0x732f, 0x7324);
  CYC(0x7324, 0x7325); L = alu_inc8(gb, L);
  CALL_C(0x7325, getRingTiles__drawTile_hook, 0x732f, 0x7328);
  CYC(0x7328, 0x732a); A = 0x1f;
  CYC(0x732a, 0x732b); push_effect(gb, 0x732b); add_a_to_hl(gb);
  CALL_C(0x732b, getRingTiles__drawTile_hook, 0x732f, 0x732e);
  CYC(0x732e, 0x732f); L = alu_inc8(gb, L);
  getRingTiles__drawTile_hook(gb);
}

void getRingTiles__drawTile_hook(GB *gb) {
  CYC(0x732f, 0x7330); A = mem_rd(gb, DE);
  CYC(0x7330, 0x7331); mem_wr(gb, HL, A);
  CYC(0x7331, 0x7332); E = alu_inc8(gb, E);
  CYC(0x7332, 0x7334); H |= 0x04;
  CYC(0x7334, 0x7335); A = mem_rd(gb, DE);
  CYC(0x7335, 0x7336); mem_wr(gb, HL, A);
  CYC(0x7336, 0x7337); E = alu_inc8(gb, E);
  CYC(0x7337, 0x7339); H &= ~0x04;
  CYC(0x7339, 0x733a); ret_effect(gb);
}

void ringMenu_updateDisplayedRingNumber_hook(GB *gb) {
  CYC(0x733a, 0x733d); A = W8(wRingMenu_ringListCursorIndex);
  ringMenu_updateDisplayedRingNumberWithGivenComparator_hook(gb);
}

void ringMenu_updateDisplayedRingNumberWithGivenComparator_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x733d, 0x7340); SET_HL(wRingMenu_displayedRingNumberComparator);
  CYC(0x7340, 0x7341); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7341, 0x7342); ret_effect(gb); return; }
  CYC(0x7341, 0x7342);
  CYC(0x7342, 0x7343); mem_wr(gb, HL, A);
  CYC(0x7343, 0x7346); A = W8(wRingMenu_selectedRing);
  CYC(0x7346, 0x7347); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x7347, 0x7349); ringMenu_updateDisplayedRingNumberWithGivenComparator__noRing_hook(gb); return; }
  CYC(0x7347, 0x7349);
  CALL_C(0x7349, hexToDec_hook, 0x0259, 0x734c);
  CYC(0x734c, 0x734e); A |= 0x10;
  CYC(0x734e, 0x7350); C |= 0x10;
  CYC(0x7350, 0x7352);
  ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber_hook(gb);
}

void ringMenu_updateDisplayedRingNumberWithGivenComparator__noRing_hook(GB *gb) {
  CYC(0x7352, 0x7354); A = 0xe8;
  CYC(0x7354, 0x7355); C = A;
  ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber_hook(gb);
}

void ringMenu_updateDisplayedRingNumberWithGivenComparator__drawNumber_hook(GB *gb) {
  CYC(0x7355, 0x7358); SET_HL(w4TileMap + 0x105);
  CYC(0x7358, 0x7359); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x7359, 0x735a); mem_wr(gb, HL, C);
  CYC(0x735a, 0x735d); ringMenu_copyTilemapToVram_hook(gb);
}

void ringMenu_setDisplayedText_hook(GB *gb) {
  CYC(0x735d, 0x7360); SET_HL(wRingMenu_descriptionTextIndex);
  CYC(0x7360, 0x7361); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7361, 0x7362); ret_effect(gb); return; }
  CYC(0x7361, 0x7362);
  CYC(0x7362, 0x7363); mem_wr(gb, HL, A);
  CYC(0x7363, 0x7364); C = A;
  CYC(0x7364, 0x7366); B = 0x30;
  CYC(0x7366, 0x7368); A = 0x02;
  CYC(0x7368, 0x736b); W8(wTextboxPosition) = A;
  CYC(0x736b, 0x736d); A = 0x09;
  CYC(0x736d, 0x7370); W8(wTextboxFlags) = A;
  CYC(0x7370, 0x7373); showTextNonExitable_hook(gb);
}

void ringMenu_retIfTextIsPrinting_hook(GB *gb) {
  CYC(0x7373, 0x7376); A = W8(wTextIsActive);
  CYC(0x7376, 0x7378); alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(0x7378, 0x7379); ret_effect(gb); return; }
  CYC(0x7378, 0x7379);
  CYC(0x7379, 0x737a); SET_AF(pop_effect(gb));
  CYC(0x737a, 0x737b); ret_effect(gb);
}

void saveQuitMenu_checkIsGameOver_hook(GB *gb) {
  CYC(0x737b, 0x737e); A = W8(wSaveQuitMenu_gameOver);
  CYC(0x737e, 0x737f); alu_or(gb, A);
  CYC(0x737f, 0x7380); ret_effect(gb);
}

void runSaveAndQuitMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7380, 0x7382); A = 0x00;
  CYC(0x7382, 0x7384); hram_wr(gb, R_SVBK, A);
  CALL_C(0x7384, runSaveAndQuitMenu__runState_hook, 0x738a, 0x7387);
  CYC(0x7387, 0x738a); saveQuitMenu_drawSprites_hook(gb);
}

void runSaveAndQuitMenu__runState_hook(GB *gb) {
  CYC(0x738a, 0x738d); A = W8(wSaveQuitMenu_state);
  CYC(0x738d, 0x738e); push_effect(gb, 0x738e);
  switch (function_caller_jump_table(gb)) {
    case 0x7394: saveQuitMenu_state0_hook(gb); return;
    case 0x73f1: saveQuitMenu_state1_hook(gb); return;
    case 0x7437: hook_handoff(gb, HL); return;
    default: hook_handoff(gb, HL); return;
  }
}

void saveQuitMenu_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7394, disableLcd_hook, 0x02c1, 0x7397);
  CALL_C(0x7397, stopTextThread_hook, 0x184d, 0x739a);
  CYC(0x739a, 0x739c); A = 0xa0;
  CALL_C(0x739c, loadGfxHeader_hook, 0x0626, 0x739f);
  CYC(0x739f, 0x73a1); A = 0xa6;
  CALL_C(0x73a1, loadGfxHeader_hook, 0x0626, 0x73a4);
  CYC(0x73a4, 0x73a6); A = 0xa8;
  CALL_C(0x73a6, loadGfxHeader_hook, 0x0626, 0x73a9);
  CALL_C(0x73a9, saveQuitMenu_checkIsGameOver_hook, 0x737b, 0x73ac);
  if (F & FZ) { CYCT(0x73ac, 0x73ae); saveQuitMenu_state0__notGameOver_hook(gb); return; }
  CYC(0x73ac, 0x73ae);
  saveQuitMenu_state0__gameOver_hook(gb);
}

void saveQuitMenu_state0__gameOver_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x73ae, restartSound_hook, 0x0cb2, 0x73b1);
  CYC(0x73b1, 0x73b3); A = 0xe8;
  CALL_C(0x73b3, threadStop_hook, 0x089b, 0x73b6);
  CYC(0x73b6, 0x73b9); SET_HL(wDeathCounter);
  CYC(0x73b9, 0x73bc); SET_BC(0x0001);
  CALL_C(0x73bc, addDecimalToHlRef_hook, 0x0181, 0x73bf);
  CYC(0x73bf, 0x73c1); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(0x73c1, 0x73c3); goto load_game_over_gfx; }
  CYC(0x73c1, 0x73c3);
  CYC(0x73c3, 0x73c5); mem_wr(gb, HL, 0x99);
  CYC(0x73c5, 0x73c6); L = alu_inc8(gb, L);
  CYC(0x73c6, 0x73c8); mem_wr(gb, HL, 0x09);
load_game_over_gfx:
  CYC(0x73c8, 0x73ca); A = 0xa9;
  CALL_C(0x73ca, loadGfxHeader_hook, 0x0626, 0x73cd);
  CYC(0x73cd, 0x73cf); A = 0x09;
  CALL_C(0x73cf, playSound_b00_hook, 0x0c98, 0x73d2);
  CYC(0x73d2, 0x73d4); A = 0x06;
  CYC(0x73d4, 0x73d6);
  CALL_C(0x73dc, loadPaletteHeader_hook, 0x050b, 0x73df);
  CYC(0x73df, 0x73e1); A = 0x08;
  CALL_C(0x73e1, loadUncompressedGfxHeader_hook, 0x05da, 0x73e4);
  CALL_C(0x73e4, fastFadeinFromWhite_hook, 0x3290, 0x73e7);
  CYC(0x73e7, 0x73e9); A = 0x01;
  CYC(0x73e9, 0x73ec); W8(wSaveQuitMenu_state) = A;
  CYC(0x73ec, 0x73ee); A = 0x05;
  CYC(0x73ee, 0x73f1); loadGfxRegisterStateIndex_hook(gb);
}

void saveQuitMenu_state0__notGameOver_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x73d6, 0x73d7); alu_xor(gb, A);
  CALL_C(0x73d7, setMusicVolume_hook, 0x0cad, 0x73da);
  CYC(0x73da, 0x73dc); A = 0x05;
  CALL_C(0x73dc, loadPaletteHeader_hook, 0x050b, 0x73df);
  CYC(0x73df, 0x73e1); A = 0x08;
  CALL_C(0x73e1, loadUncompressedGfxHeader_hook, 0x05da, 0x73e4);
  CALL_C(0x73e4, fastFadeinFromWhite_hook, 0x3290, 0x73e7);
  CYC(0x73e7, 0x73e9); A = 0x01;
  CYC(0x73e9, 0x73ec); W8(wSaveQuitMenu_state) = A;
  CYC(0x73ec, 0x73ee); A = 0x05;
  CYC(0x73ee, 0x73f1); loadGfxRegisterStateIndex_hook(gb);
}

void saveQuitMenu_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x73f1, 0x73f4); A = W8(wPaletteThread_mode);
  CYC(0x73f4, 0x73f5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x73f5, 0x73f6); ret_effect(gb); return; }
  CYC(0x73f5, 0x73f6);
  CYC(0x73f6, 0x73f9); A = W8(wKeysJustPressed);
  CYC(0x73f9, 0x73fb); C = 0xff;
  CYC(0x73fb, 0x73fd); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x73fd, 0x73ff); saveQuitMenu_state1__upOrDown_hook(gb); return; }
  CYC(0x73fd, 0x73ff);
  CYC(0x73ff, 0x7401); C = 1;
  CYC(0x7401, 0x7403); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x7403, 0x7405); saveQuitMenu_state1__upOrDown_hook(gb); return; }
  CYC(0x7403, 0x7405);
  CYC(0x7405, 0x7407); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x7407, 0x7409); saveQuitMenu_state1__bPressed_hook(gb); return; }
  CYC(0x7407, 0x7409);
  CYC(0x7409, 0x740b); alu_and(gb, 0x09);
  if (F & FZ) { CYCT(0x740b, 0x740c); ret_effect(gb); return; }
  CYC(0x740b, 0x740c);
  CYC(0x740c, 0x740f); A = W8(wSaveQuitMenu_cursorIndex);
  CYC(0x740f, 0x7410); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x7410, saveFile_b00_hook, 0x09d8, 0x7413); else CYC(0x7410, 0x7413);
  CYC(0x7413, 0x7415); A = 2;
  CYC(0x7415, 0x7418); W8(wSaveQuitMenu_state) = A;
  CYC(0x7418, 0x741a); A = 0x1e;
  CYC(0x741a, 0x741d); W8(wSaveQuitMenu_delayCounter) = A;
  CYC(0x741d, 0x741f); A = 0x56;
  CYC(0x741f, 0x7422); playSound_b00_hook(gb);
}

void saveQuitMenu_state1__upOrDown_hook(GB *gb) {
  CYC(0x7422, 0x7425); SET_HL(wSaveQuitMenu_cursorIndex);
  CYC(0x7425, 0x7426); A = mem_rd(gb, HL);
  CYC(0x7426, 0x7427); alu_add(gb, C);
  CYC(0x7427, 0x7429); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(0x7429, 0x742a); ret_effect(gb); return; }
  CYC(0x7429, 0x742a);
  CYC(0x742a, 0x742b); mem_wr(gb, HL, A);
  CYC(0x742b, 0x742d); A = 0x84;
  CYC(0x742d, 0x7430); playSound_b00_hook(gb);
}

void saveQuitMenu_state1__bPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7430, saveQuitMenu_checkIsGameOver_hook, 0x737b, 0x7433);
  if (!(F & FZ)) { CYCT(0x7433, 0x7434); ret_effect(gb); return; }
  CYC(0x7433, 0x7434); CYC(0x7434, 0x7437); closeMenu_hook(gb);
}

void saveQuitMenu_drawSprites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7455, fileSelect_redrawDecorationsAndSetWramBank4_hook, 0x4cd7, 0x7458);
  CYC(0x7458, 0x745b); A = W8(wSaveQuitMenu_delayCounter);
  CYC(0x745b, 0x745d); alu_and(gb, 4);
  if (!(F & FZ)) { CYCT(0x745d, 0x745e); ret_effect(gb); return; }
  CYC(0x745d, 0x745e);
  CYC(0x745e, 0x745f); C = A;
  CYC(0x745f, 0x7462); A = W8(wSaveQuitMenu_cursorIndex);
  CYC(0x7462, 0x7463); B = A;
  CYC(0x7463, 0x7464); alu_add(gb, A);
  CYC(0x7464, 0x7465); alu_add(gb, B);
  CYC(0x7465, 0x7467); A = alu_swap(gb, A);
  CYC(0x7467, 0x7468); alu_rrca(gb);
  CYC(0x7468, 0x7469); B = A;
  CYC(0x7469, 0x746c); SET_HL(0x746f);
  CYC(0x746c, 0x746f); addSpritesToOam_withOffset_hook(gb);
}

void fileSelect_redrawDecorationsAndSetWramBank4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4cd7, clearOam_hook, 0x049f, 0x4cda);
  CYC(0x4cda, 0x4cdc); A = 4;
  CYC(0x4cdc, 0x4cde); hram_wr(gb, R_SVBK, A);
  CYC(0x4cde, 0x4ce1); SET_HL(0x4ce4);
  CYC(0x4ce1, 0x4ce4); addSpritesToOam_hook(gb);
}

void fileSelectDrawLink_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4d2b, 0x4d2e); A = W8(wFileSelect_cursorPos);
  CYC(0x4d2e, 0x4d30); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(0x4d30, 0x4d31); ret_effect(gb); return; }
  CYC(0x4d30, 0x4d31);
  CYC(0x4d31, 0x4d33); D = 0;
  CALL_C(0x4d33, getFileDisplayVariableAddress_hook, 0x417a, 0x4d36);
  CYC(0x4d36, 0x4d38); C = 0;
  CYC(0x4d38, 0x4d3a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4d3a, 0x4d3c); goto draw; }
  CYC(0x4d3a, 0x4d3c); CYC(0x4d3c, 0x4d3d); push_effect(gb, BC);
  CYC(0x4d3d, 0x4d3e); push_effect(gb, DE); CYC(0x4d3e, 0x4d40); D = 7;
  CALL_C(0x4d40, getFileDisplayVariableAddress_paramE_hook, 0x417b, 0x4d43);
  CYC(0x4d43, 0x4d44); alu_xor(gb, A); CYC(0x4d44, 0x4d46); B = 0x10;
  CYC(0x4d46, 0x4d48); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) CALL_C_CC(0x4d48, fileSelectDrawLink__draw_hook, 0x4d67, 0x4d4b); else CYC(0x4d48, 0x4d4b);
  CYC(0x4d4b, 0x4d4c); SET_DE(pop_effect(gb)); CYC(0x4d4c, 0x4d4d); SET_BC(pop_effect(gb));
  CYC(0x4d4d, 0x4d4f); D = 6; CALL_C(0x4d4f, getFileDisplayVariableAddress_paramE_hook, 0x417b, 0x4d52);
  CYC(0x4d52, 0x4d53); C = alu_inc8(gb, C); CYC(0x4d53, 0x4d54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4d54, 0x4d55); alu_rrca(gb); if (F & FC) { CYCT(0x4d55,0x4d57); goto draw; }
  CYC(0x4d55,0x4d57); CYC(0x4d57,0x4d58); C = alu_inc8(gb,C); CYC(0x4d58,0x4d5a); alu_bit(gb,0,mem_rd(gb,HL));
  if (!(F & FZ)) { CYCT(0x4d5a,0x4d5c); goto draw; } CYC(0x4d5a,0x4d5c); CYC(0x4d5c,0x4d5d); C=alu_inc8(gb,C);
draw:
  CYC(0x4d5d,0x4d60); A=W8(wFileSelect_cbb6); CYC(0x4d60,0x4d62); alu_and(gb,0x10); CYC(0x4d62,0x4d63); A=C;
  if (F & FZ) { CYCT(0x4d63,0x4d65); fileSelectDrawLink__draw_hook(gb); return; }
  CYC(0x4d63,0x4d65); CYC(0x4d65,0x4d67); alu_add(gb,8); fileSelectDrawLink__draw_hook(gb);
}
void fileSelectDrawLinkInOtherGame_hook(GB *gb) {
  CYC(0x4d25, 0x4d27); B = 0;
  CYC(0x4d27, 0x4d29);
  fileSelectDrawLink_body_hook(gb);
}

void fileSelectDrawLink_hook(GB *gb) {
  CYC(0x4d29, 0x4d2b); B = 4;
  fileSelectDrawLink_body_hook(gb);
}
void fileSelectDrawLink__draw_hook(GB *gb) {
  CYC(0x4d67,0x4d68); alu_add(gb,B); CYC(0x4d68,0x4d6b); SET_HL(0x4d71);
  CYC(0x4d6b,0x4d6c); push_effect(gb,0x4d6c); add_a_to_hl(gb);
  CYC(0x4d6c,0x4d6d); A=mem_rd(gb,HL);
  CYC(0x4d6d,0x4d6e); push_effect(gb,0x4d6e); add_a_to_hl(gb);
  CYC(0x4d6e,0x4d71); addSpritesToOam_hook(gb);
}

void runGameLinkMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b22, 0x4b25); SET_HL(wFileSelect_cbb6);
  CYC(0x4b25, 0x4b26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x4b26, fileSelect_redrawDecorationsAndSetWramBank4_hook, 0x4cd7, 0x4b29);
  fileSelectMode7_hook(gb);
}

void fileSelectMode7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4b29, fileSelectMode7__mode7States_hook, 0x4b3b, 0x4b2c);
  CYC(0x4b2c, 0x4b2f); A = W8(wFileSelect_mode2);
  CYC(0x4b2f, 0x4b31); alu_cp(gb, 6);
  if (F & FZ) { CYCT(0x4b31, 0x4b32); ret_effect(gb); return; }
  CYC(0x4b31, 0x4b32);
  CYC(0x4b32, 0x4b34); alu_cp(gb, 3);
  if (F & FC) { CYCT(0x4b34, 0x4b35); ret_effect(gb); return; }
  CYC(0x4b34, 0x4b35);
  CALL_C(0x4b35, fileSelectDrawAcornCursor_hook, 0x4a97, 0x4b38);
  CYC(0x4b38, 0x4b3b); fileSelectDrawLinkInOtherGame_hook(gb);
}

void fileSelectMode7__mode7States_hook(GB *gb) {
  CYC(0x4b3b, 0x4b3e); A = W8(wFileSelect_mode2);
  CYC(0x4b3e, 0x4b3f); push_effect(gb, 0x4b3f);
  switch (function_caller_jump_table(gb)) {
    case 0x4b4d: fileSelectMode7__state0_hook(gb); return;
    case 0x4b88: fileSelectMode7__state1_hook(gb); return;
    case 0x4be2: fileSelectMode7__state2_hook(gb); return;
    case 0x4c04: fileSelectMode7__state3_hook(gb); return;
    case 0x4c74: fileSelectMode7__state4_hook(gb); return;
    case 0x4caa: fileSelectMode7__state5_hook(gb); return;
    case 0x4cbe: fileSelectMode7__state6_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void fileSelectMode7__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4b4d, disableLcd_hook, 0x02c1, 0x4b50);
  CYC(0x4b50, 0x4b52); A = 0xa0;
  CALL_C(0x4b52, loadGfxHeader_hook, 0x0626, 0x4b55);
  CYC(0x4b55, 0x4b57); A = 0xae;
  CALL_C(0x4b57, loadGfxHeader_hook, 0x0626, 0x4b5a);
  CYC(0x4b5a, 0x4b5c); A = 5;
  CALL_C(0x4b5c, loadPaletteHeader_hook, 0x050b, 0x4b5f);
  CYC(0x4b5f, 0x4b61); A = 8;
  CALL_C(0x4b61, loadUncompressedGfxHeader_hook, 0x05da, 0x4b64);
  CYC(0x4b64, 0x4b67); SET_HL(w4NameBuffer);
  CYC(0x4b67, 0x4b69); B = 0x20;
  CALL_C(0x4b69, clearMemory_hook, 0x046f, 0x4b6c);
  CALL_C(0x4b6c, textInput_updateEntryCursor_hook, 0x4a22, 0x4b6f);
  CALL_C(0x4b6f, serialFunc_0c85_hook, 0x0c85, 0x4b72);
  CYC(0x4b72, 0x4b74); A = 4;
  CYC(0x4b74, 0x4b76); H8(hFFBE) = A;
  CYC(0x4b76, 0x4b77); alu_xor(gb, A);
  CYC(0x4b77, 0x4b79); H8(hSerialLinkState) = A;
  CYC(0x4b79, 0x4b7c); mem_wr(gb, wFileSelect_cbc0 + 2, A);
  CYC(0x4b7c, 0x4b7f); SET_HL(wFileSelect_linkTimer);
  CYC(0x4b7f, 0x4b81); A = 0xf0;
  CYC(0x4b81, 0x4b82); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b82, 0x4b84); A = 0x1e;
  CYC(0x4b84, 0x4b85); mem_wr(gb, HL, A);
  CYC(0x4b85, 0x4b88); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode7__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b88, 0x4b8a); A = H8(hSerialInterruptBehaviour);
  CYC(0x4b8a, 0x4b8b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4b8b, 0x4b8d); goto serial_interrupt; }
  CYC(0x4b8b, 0x4b8d);
  CYC(0x4b8d, 0x4b8f); A = H8(hFFBD);
  CYC(0x4b8f, 0x4b90); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4b90, 0x4b93); fileSelectMode7__func_02_4c55_hook(gb); return; }
  CYC(0x4b90, 0x4b93);
  CYC(0x4b93, 0x4b96); SET_HL(wFileSelect_linkTimer);
  CYC(0x4b96, 0x4b97); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x4b97, 0x4b99); CYC(0x4ba0, 0x4ba3); serialFunc_0c73_hook(gb); return; }
  CYC(0x4b97, 0x4b99);
  CYC(0x4b99, 0x4b9b); A = 0x80;
  CYC(0x4b9b, 0x4b9d); H8(hFFBD) = A;
  CYC(0x4b9d, 0x4ba0); fileSelectMode7__func_02_4c55_hook(gb);
  return;
serial_interrupt:
  CYC(0x4ba3, 0x4ba6); A = W8(wFileSelect_cbc0);
  CYC(0x4ba6, 0x4ba7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4ba7, 0x4ba9); goto transfer; }
  CYC(0x4ba7, 0x4ba9);
  CYC(0x4ba9, 0x4baa); A = alu_dec8(gb, A);
  CYC(0x4baa, 0x4bad); W8(wFileSelect_cbc0) = A;
  CYC(0x4bad, 0x4bae); ret_effect(gb);
  return;
transfer:
  CALL_C(0x4bae, serialFunc_0c8d_hook, 0x0c8d, 0x4bb1);
  CYC(0x4bb1, 0x4bb3); A = H8(hFFBD);
  CYC(0x4bb3, 0x4bb4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4bb4, 0x4bb6); goto check_link; }
  CYC(0x4bb4, 0x4bb6);
  CYC(0x4bb6, 0x4bb8); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(0x4bb8, 0x4bba); goto check_link; }
  CYC(0x4bb8, 0x4bba);
  if (!(F & FZ)) { CYCT(0x4bba, 0x4bbd); fileSelectMode7__func_02_4c55_hook(gb); return; }
  CYC(0x4bba, 0x4bbd);
check_link:
  CYC(0x4bbd, 0x4bbf); A = H8(hSerialLinkState);
  CYC(0x4bbf, 0x4bc1); alu_cp(gb, 7);
  if (!(F & FZ)) { CYCT(0x4bc1, 0x4bc2); ret_effect(gb); return; }
  CYC(0x4bc1, 0x4bc2);
  CYC(0x4bc2, 0x4bc4); E = 3;
find_file:
  CYC(0x4bc4, 0x4bc5); E = alu_dec8(gb, E);
  CYC(0x4bc5, 0x4bc7); D = 0;
  CALL_C(0x4bc7, getFileDisplayVariableAddress_paramE_hook, 0x417b, 0x4bca);
  CYC(0x4bca, 0x4bcc); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4bcc, 0x4bce); CYC(0x4bdf, 0x4be2); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb); return; }
  CYC(0x4bcc, 0x4bce);
  CYC(0x4bce, 0x4bcf); A = E;
  CYC(0x4bcf, 0x4bd0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4bd0, 0x4bd2); goto find_file; }
  CYC(0x4bd0, 0x4bd2);
  CYC(0x4bd2, 0x4bd4); A = 0x85;
  CYC(0x4bd4, 0x4bd7); mem_wr(gb, wFileSelect_cbc0 + 2, A);
  CYC(0x4bd7, 0x4bd9); A = 0xff;
  CYC(0x4bd9, 0x4bdc); W8(wFileSelect_cursorPos) = A;
  CYC(0x4bdc, 0x4bdf); fileSelectMode7__func_02_4c4b_hook(gb);
}

void fileSelectMode7__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4be2, serialFunc_0c8d_hook, 0x0c8d, 0x4be5);
  CYC(0x4be5, 0x4be7); A = 6;
  CYC(0x4be7, 0x4bea); W8(wFileSelect_cursorOffset) = A;
  CYC(0x4bea, 0x4beb); alu_xor(gb, A);
  CALL_C(0x4beb, func_02_4149_hook, 0x4149, 0x4bee);
  CALL_C(0x4bee, disableLcd_hook, 0x02c1, 0x4bf1);
  CYC(0x4bf1, 0x4bf3); A = 0xa1;
  CALL_C(0x4bf3, loadGfxHeader_hook, 0x0626, 0x4bf6);
  CYC(0x4bf6, 0x4bf8); A = 0xaf;
  CALL_C(0x4bf8, loadGfxHeader_hook, 0x0626, 0x4bfb);
  CALL_C(0x4bfb, textInput_updateEntryCursor_hook, 0x4a22, 0x4bfe);
  CALL_C(0x4bfe, fileSelectDrawHeartsAndDeathCounter_hook, 0x4a4a, 0x4c01);
  CYC(0x4c01, 0x4c04); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode7__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c04, serialFunc_0c8d_hook, 0x0c8d, 0x4c07);
  CALL_C(0x4c07, fileSelectUpdateInput_hook, 0x4467, 0x4c0a);
  if (!(F & FZ)) { CYCT(0x4c0a, 0x4c0c); fileSelectMode7__selectedSomething_hook(gb); return; }
  CYC(0x4c0a, 0x4c0c);
  CYC(0x4c0c, 0x4c0f); A = W8(wKeysJustPressed);
  CYC(0x4c0f, 0x4c11); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x4c11, 0x4c12); ret_effect(gb); return; }
  CYC(0x4c11, 0x4c12);
  fileSelectMode7__moveCursorToQuit_hook(gb);
}

void fileSelectMode7__moveCursorToQuit_hook(GB *gb) {
  CYC(0x4c12, 0x4c14); A = 3;
  CYC(0x4c14, 0x4c17); W8(wFileSelect_cursorPos) = A;
  CYC(0x4c17, 0x4c19); A = 0x8f;
  CYC(0x4c19, 0x4c1c); mem_wr(gb, wFileSelect_cbc0 + 2, A);
  CYC(0x4c1c, 0x4c1e); fileSelectMode7__func_02_4c4b_hook(gb);
}

void fileSelectMode7__selectedSomething_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c1e, 0x4c21); A = W8(wFileSelect_cursorPos);
  CYC(0x4c21, 0x4c23); alu_cp(gb, 3);
  if (F & FZ) { CYCT(0x4c23, 0x4c25); fileSelectMode7__moveCursorToQuit_hook(gb); return; }
  CYC(0x4c23, 0x4c25);
  CYC(0x4c25, 0x4c27); D = 0;
  CALL_C(0x4c27, getFileDisplayVariableAddress_hook, 0x417a, 0x4c2a);
  CYC(0x4c2a, 0x4c2c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4c2c, 0x4c2e); goto valid_file; }
  CYC(0x4c2c, 0x4c2e);
  CYC(0x4c2e, 0x4c30); A = 0x5a;
  CYC(0x4c30, 0x4c33); playSound_b00_hook(gb);
  return;
valid_file:
  CYC(0x4c33, 0x4c36); A = W8(wOpenedMenuType);
  CYC(0x4c36, 0x4c38); alu_cp(gb, 8);
  if (!(F & FZ)) { CYCT(0x4c38, 0x4c3a); goto file_select; }
  CYC(0x4c38, 0x4c3a);
  CYC(0x4c3a, 0x4c3c); A = 0x0c;
  CYC(0x4c3c, 0x4c3e); H8(hSerialLinkState) = A;
  CYC(0x4c3e, 0x4c40); A = 5;
  CYC(0x4c40, 0x4c43); W8(wFileSelect_mode2) = A;
  CYC(0x4c43, 0x4c44); ret_effect(gb);
  return;
file_select:
  CYC(0x4c44, 0x4c46); A = 8;
  CYC(0x4c46, 0x4c48); H8(hSerialLinkState) = A;
  CYC(0x4c48, 0x4c4b); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode7__func_02_4c4b_hook(GB *gb) {
  CYC(0x4c4b, 0x4c4d); A = 8;
  CYC(0x4c4d, 0x4c4f); H8(hSerialLinkState) = A;
  CYC(0x4c4f, 0x4c51); A = 5;
  CYC(0x4c51, 0x4c54); W8(wFileSelect_mode2) = A;
  CYC(0x4c54, 0x4c55); ret_effect(gb);
}

void fileSelectMode7__func_02_4c55_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c55, disableLcd_hook, 0x02c1, 0x4c58);
  CYC(0x4c58, 0x4c5a); A = 7;
  CALL_C(0x4c5a, loadGfxHeader_hook, 0x0626, 0x4c5d);
  CALL_C(0x4c5d, loadGfxRegisterState5AndIncFileSelectMode2_hook, 0x4165, 0x4c60);
  CYC(0x4c60, 0x4c62); A = 8;
  CYC(0x4c62, 0x4c64); H8(hSerialLinkState) = A;
  CYC(0x4c64, 0x4c66); A = 6;
  CYC(0x4c66, 0x4c69); W8(wFileSelect_mode2) = A;
  CYC(0x4c69, 0x4c6b); A = 0xb4;
  CYC(0x4c6b, 0x4c6e); W8(wFileSelect_linkTimer) = A;
  CYC(0x4c6e, 0x4c70); A = H8(hFFBD);
  CYC(0x4c70, 0x4c73); W8(wFileSelect_cbc0) = A;
  CYC(0x4c73, 0x4c74); ret_effect(gb);
}

void fileSelectMode7__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c74, serialFunc_0c8d_hook, 0x0c8d, 0x4c77);
  CYC(0x4c77, 0x4c79); A = H8(hSerialInterruptBehaviour);
  CYC(0x4c79, 0x4c7a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4c7a, 0x4c7b); ret_effect(gb); return; }
  CYC(0x4c7a, 0x4c7b);
  CALL_C(0x4c7b, loadFile_b00_hook, 0x09dc, 0x4c7e);
  CYC(0x4c7e, 0x4c81); A = W8(wFileSelect_cursorPos);
  CYC(0x4c81, 0x4c82); A = alu_inc8(gb, A);
  CYC(0x4c82, 0x4c85); SET_HL(w4RingFortuneStuff);
  CYC(0x4c85, 0x4c88); SET_BC(0x16);
offset_loop:
  CYC(0x4c88, 0x4c89); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4c89, 0x4c8b); goto copy_file; }
  CYC(0x4c89, 0x4c8b);
  CYC(0x4c8b, 0x4c8c); alu_add_hl(gb, BC);
  CYC(0x4c8c, 0x4c8e);
  goto offset_loop;
copy_file:
  CYC(0x4c8e, 0x4c90); B = 0x16;
  CYC(0x4c90, 0x4c93); SET_DE(wc600Block);
  CALL_C(0x4c93, copyMemory_hook, 0x0486, 0x4c96);
  CYC(0x4c96, 0x4c99); SET_HL(wFileIsLinkedGame);
  CYC(0x4c99, 0x4c9b); mem_wr(gb, HL, mem_rd(gb, HL) | 1);
  CYC(0x4c9b, 0x4c9d); L = (uint8_t)wFileIsCompleted;
  CYC(0x4c9d, 0x4c9f); mem_wr(gb, HL, 0);
  CALL_C(0x4c9f, initializeFile_b00_hook, 0x09d4, 0x4ca2);
  CYC(0x4ca2, 0x4ca4); A = 0x56;
  CALL_C(0x4ca4, playSound_b00_hook, 0x0c98, 0x4ca7);
  CYC(0x4ca7, 0x4caa); setFileSelectModeTo1_hook(gb);
}

void fileSelectMode7__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4caa, serialFunc_0c8d_hook, 0x0c8d, 0x4cad);
  CYC(0x4cad, 0x4caf); A = H8(hSerialInterruptBehaviour);
  CYC(0x4caf, 0x4cb0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4cb0, 0x4cb1); ret_effect(gb); return; }
  CYC(0x4cb0, 0x4cb1);
  fileSelectMode7__cancelLink_hook(gb);
}

void fileSelectMode7__cancelLink_hook(GB *gb) {
  CYC(0x4cb1, 0x4cb4); A = W8(wOpenedMenuType);
  CYC(0x4cb4, 0x4cb6); alu_cp(gb, 8);
  if (F & FZ) { CYCT(0x4cb6, 0x4cb9); closeMenu_hook(gb); return; }
  CYC(0x4cb6, 0x4cb9);
  CYC(0x4cb9, 0x4cbb); A = 0;
  CYC(0x4cbb, 0x4cbe); setFileSelectMode_hook(gb);
}

void fileSelectMode7__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4cbe, serialFunc_0c8d_hook, 0x0c8d, 0x4cc1);
  CYC(0x4cc1, 0x4cc3); A = H8(hSerialInterruptBehaviour);
  CYC(0x4cc3, 0x4cc4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4cc4, 0x4cc5); ret_effect(gb); return; }
  CYC(0x4cc4, 0x4cc5);
  CYC(0x4cc5, 0x4cc8); A = W8(wFileSelect_cbc0);
  CYC(0x4cc8, 0x4cca); H8(hFFBD) = A;
  CYC(0x4cca, 0x4ccd); A = W8(wKeysJustPressed);
  CYC(0x4ccd, 0x4cce); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4cce, 0x4cd0); fileSelectMode7__cancelLink_hook(gb); return; }
  CYC(0x4cce, 0x4cd0);
  CYC(0x4cd0, 0x4cd3); SET_HL(wFileSelect_linkTimer);
  CYC(0x4cd3, 0x4cd4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x4cd4, 0x4cd5); ret_effect(gb); return; }
  CYC(0x4cd4, 0x4cd5);
  CYC(0x4cd5, 0x4cd7);
  fileSelectMode7__cancelLink_hook(gb);
}

void runFakeReset_hook(GB *gb) {
  CYC(0x7641, 0x7644); A = W8(wFakeResetMenu_state);
  CYC(0x7644, 0x7645); push_effect(gb, 0x7645);
  switch (function_caller_jump_table(gb)) {
    case 0x7649: runFakeReset__state0_hook(gb); return;
    case 0x7674: runFakeReset__state1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void runFakeReset__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7649, disableLcd_hook, 0x02c1, 0x764c);
  CALL_C(0x764c, clearOam_hook, 0x049f, 0x764f);
  CALL_C(0x764f, clearVram_hook, 0x04af, 0x7652);
  CALL_C(0x7652, initializeVramMaps_hook, 0x04cd, 0x7655);
  CYC(0x7655, 0x7657); A = 0xf5;
  CALL_C(0x7657, playSound_b00_hook, 0x0c98, 0x765a);
  CYC(0x765a, 0x765c); A = 1;
  CALL_C(0x765c, loadGfxHeader_hook, 0x0626, 0x765f);
  CYC(0x765f, 0x7661); A = 1;
  CALL_C(0x7661, loadPaletteHeader_hook, 0x050b, 0x7664);
  CYC(0x7664, 0x7666); A = 0x78;
  CYC(0x7666, 0x7669); W8(wFakeResetMenu_delayCounter) = A;
  CYC(0x7669, 0x766c); SET_HL(wFakeResetMenu_state);
  CYC(0x766c, 0x766d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x766d, fadeinFromWhite_hook, 0x3299, 0x7670);
  CYC(0x7670, 0x7671); alu_xor(gb, A);
  CYC(0x7671, 0x7674); loadGfxRegisterStateIndex_hook(gb);
}

void runFakeReset__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7674, 0x7677); A = W8(wPaletteThread_mode);
  CYC(0x7677, 0x7678); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7678, 0x7679); ret_effect(gb); return; }
  CYC(0x7678, 0x7679);
  CYC(0x7679, 0x767c); SET_HL(wFakeResetMenu_delayCounter);
  CYC(0x767c, 0x767d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x767d, 0x767e); ret_effect(gb); return; }
  CYC(0x767d, 0x767e);
  CYC(0x767e, 0x7680); A = 0xf6;
  CALL_C(0x7680, playSound_b00_hook, 0x0c98, 0x7683);
  CYC(0x7683, 0x7686); SET_HL(wMenuLoadState);
  CYC(0x7686, 0x7687); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7687, 0x768a); fadeoutToWhite_hook(gb);
}

void fileSelectMode1__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x41b6, setFileSelectCursorOffsetToFileSelectMode_hook, 0x4152, 0x41b9);
  CYC(0x41b9, 0x41ba); alu_xor(gb, A);
  CALL_C(0x41ba, func_02_4149_hook, 0x4149, 0x41bd);
  CALL_C(0x41bd, disableLcd_hook, 0x02c1, 0x41c0);
  CYC(0x41c0, 0x41c2); A = 0xba;
  CALL_C(0x41c2, loadGfxHeader_hook, 0x0626, 0x41c5);
  CYC(0x41c5, 0x41c7); A = 5;
  CALL_C(0x41c7, loadPaletteHeader_hook, 0x050b, 0x41ca);
  CALL_C(0x41ca, loadFileDisplayVariables_hook, 0x49da, 0x41cd);
  CALL_C(0x41cd, textInput_updateEntryCursor_hook, 0x4a22, 0x41d0);
  CALL_C(0x41d0, fileSelectDrawHeartsAndDeathCounter_hook, 0x4a4a, 0x41d3);
  CYC(0x41d3, 0x41d6); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode1__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x41d6, fileSelectUpdateInput_hook, 0x4467, 0x41d9);
  if (!(F & FZ)) { CYCT(0x41d9, 0x41db); goto selected; }
  CYC(0x41d9, 0x41db);
  CYC(0x41db, 0x41de); SET_HL(wFileSelect_cursorPos);
  CYC(0x41de, 0x41df); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41df, 0x41e1); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x41e1, 0x41e3); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(0x41e3, 0x41e4); ret_effect(gb); return; }
  CYC(0x41e3, 0x41e4);
  CALL_C(0x41e4, func_02_448d_hook, 0x448d, 0x41e7);
  if (F & FZ) { CYCT(0x41e7, 0x41e8); ret_effect(gb); return; }
  CYC(0x41e7, 0x41e8);
selected:
  CYC(0x41e8, 0x41ea); A = 0x56;
  CALL_C(0x41ea, playSound_b00_hook, 0x0c98, 0x41ed);
  CALL_C(0x41ed, fileSelectMode1__getNextFileSelectMode_hook, 0x41fe, 0x41f0);
  if (!(F & FZ)) { CYCT(0x41f0, 0x41f3); setFileSelectMode_hook(gb); return; }
  CYC(0x41f0, 0x41f3);
  CALL_C(0x41f3, incFileSelectMode2_hook, 0x416a, 0x41f6);
  CALL_C(0x41f6, loadFile_b00_hook, 0x09dc, 0x41f9);
  CYC(0x41f9, 0x41fb); A = 0x16;
  CYC(0x41fb, 0x41fe); loadUncompressedGfxHeader_hook(gb);
}

void fileSelectMode1__getNextFileSelectMode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x41fe, 0x4201); A = W8(wFileSelect_cursorPos);
  CYC(0x4201, 0x4203); alu_cp(gb, 3);
  if (F & FZ) { CYCT(0x4203, 0x4205); goto non_file; }
  CYC(0x4203, 0x4205);
  CYC(0x4205, 0x4207); H8(hActiveFileSlot) = A;
  CYC(0x4207, 0x4209); D = 0;
  CALL_C(0x4209, getFileDisplayVariableAddress_hook, 0x417a, 0x420c);
  CYC(0x420c, 0x420e); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x420e, 0x4210); A = 5;
  if (!(F & FZ)) { CYCT(0x4210, 0x4211); ret_effect(gb); return; }
  CYC(0x4210, 0x4211);
  CYC(0x4211, 0x4212); alu_xor(gb, A);
  CYC(0x4212, 0x4213); ret_effect(gb);
  return;
non_file:
  CYC(0x4213, 0x4216); A = W8(wFileSelect_cursorPos2);
  CYC(0x4216, 0x4218); alu_add(gb, 3);
  CYC(0x4218, 0x4219); ret_effect(gb);
}

void fileSelectMode1__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4219, fileSelectMode1__textSpeedMenu_checkInput_hook, 0x421e, 0x421c);
  CYC(0x421c, 0x421e);
  fileSelectMode1__textSpeedMenu_addCursorToOam_hook(gb);
}

void fileSelectMode1__textSpeedMenu_checkInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x421e, 0x4221); A = W8(wKeysJustPressed);
  CYC(0x4221, 0x4222); B = A;
  CYC(0x4222, 0x4224); alu_and(gb, 6);
  if (!(F & FZ)) { CYCT(0x4224, 0x4226); fileSelectMode1__back_hook(gb); return; }
  CYC(0x4224, 0x4226); CYC(0x4226, 0x4228); C = 1;
  CYC(0x4228, 0x422a); alu_bit(gb, 4, B);
  if (!(F & FZ)) { CYCT(0x422a, 0x422c); fileSelectMode1__leftOrRight_hook(gb); return; }
  CYC(0x422a, 0x422c); CYC(0x422c, 0x422e); C = 0xff;
  CYC(0x422e, 0x4230); alu_bit(gb, 5, B);
  if (!(F & FZ)) { CYCT(0x4230, 0x4232); fileSelectMode1__leftOrRight_hook(gb); return; }
  CYC(0x4230, 0x4232); CYC(0x4232, 0x4233); A = B;
  CYC(0x4233, 0x4235); alu_and(gb, 9);
  if (F & FZ) { CYCT(0x4235, 0x4236); ret_effect(gb); return; }
  CYC(0x4235, 0x4236); CYC(0x4236, 0x4238); A = 0x56;
  CALL_C(0x4238, playSound_b00_hook, 0x0c98, 0x423b);
  CALL_C(0x423b, incFileSelectMode2_hook, 0x416a, 0x423e);
  CALL_C(0x423e, saveFile_b00_hook, 0x09d8, 0x4241);
  CYC(0x4241, 0x4244); fadeoutToWhite_hook(gb);
}

void fileSelectMode1__back_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4244, 0x4246); A = 8;
  CALL_C(0x4246, loadUncompressedGfxHeader_hook, 0x05da, 0x4249);
  CYC(0x4249, 0x424c); decFileSelectMode2_hook(gb);
}

void fileSelectMode1__leftOrRight_hook(GB *gb) {
  CYC(0x424c, 0x424f); SET_HL(wTextSpeed);
  CYC(0x424f, 0x4250); A = mem_rd(gb, HL);
  CYC(0x4250, 0x4251); alu_add(gb, C);
  CYC(0x4251, 0x4253); alu_cp(gb, 5);
  if (!(F & FC)) { CYCT(0x4253, 0x4254); ret_effect(gb); return; }
  CYC(0x4253, 0x4254);
  CYC(0x4254, 0x4255); mem_wr(gb, HL, A);
  CYC(0x4255, 0x4257); A = 0x84;
  CYC(0x4257, 0x425a); playSound_b00_hook(gb);
}

void fileSelectMode1__textSpeedMenu_addCursorToOam_hook(GB *gb) {
  CYC(0x425a, 0x425d); A = W8(wTextSpeed);
  CYC(0x425d, 0x425f); A = alu_swap(gb, A);
  CYC(0x425f, 0x4260); C = A;
  CYC(0x4260, 0x4262); B = 0;
  CYC(0x4262, 0x4265); SET_HL(0x4268);
  CYC(0x4265, 0x4268); addSpritesToOam_withOffset_hook(gb);
}

void fileSelectMode5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x427d, fileSelectMode5__mode5States_hook, 0x4283, 0x4280);
  CYC(0x4280, 0x4283); fileSelectDrawAcornCursor_hook(gb);
}

void fileSelectMode5__mode5States_hook(GB *gb) {
  CYC(0x4283, 0x4286); A = W8(wFileSelect_mode2);
  CYC(0x4286, 0x4287); push_effect(gb, 0x4287);
  switch (function_caller_jump_table(gb)) {
    case 0x428b: fileSelectMode5__state0_hook(gb); return;
    case 0x42a7: fileSelectMode5__state1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void fileSelectMode5__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x428b, disableLcd_hook, 0x02c1, 0x428e);
  CYC(0x428e, 0x4290); A = 0xa7;
  CALL_C(0x4290, loadGfxHeader_hook, 0x0626, 0x4293);
  CYC(0x4293, 0x4295); A = 0xa6;
  CALL_C(0x4295, loadGfxHeader_hook, 0x0626, 0x4298);
  CYC(0x4298, 0x429a); A = 8;
  CALL_C(0x429a, loadUncompressedGfxHeader_hook, 0x05da, 0x429d);
  CALL_C(0x429d, setFileSelectCursorOffsetToFileSelectMode_hook, 0x4152, 0x42a0);
  CYC(0x42a0, 0x42a1); alu_xor(gb, A);
  CALL_C(0x42a1, func_02_4149_hook, 0x4149, 0x42a4);
  CYC(0x42a4, 0x42a7); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode5__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42a7, 0x42aa); A = W8(wKeysJustPressed);
  CYC(0x42aa, 0x42ac); C = 1;
  CYC(0x42ac, 0x42ae); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x42ae, 0x42b0); fileSelectMode5__upOrDown_hook(gb); return; }
  CYC(0x42ae, 0x42b0); CYC(0x42b0, 0x42b2); C = 0xff;
  CYC(0x42b2, 0x42b4); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x42b4, 0x42b6); fileSelectMode5__upOrDown_hook(gb); return; }
  CYC(0x42b4, 0x42b6); CYC(0x42b6, 0x42b7); C = A;
  CYC(0x42b7, 0x42b9); alu_and(gb, 6);
  if (!(F & FZ)) { CYCT(0x42b9, 0x42bc); setFileSelectModeTo1_hook(gb); return; }
  CYC(0x42b9, 0x42bc); CYC(0x42bc, 0x42bd); A = C;
  CYC(0x42bd, 0x42bf); alu_and(gb, 9);
  if (F & FZ) { CYCT(0x42bf, 0x42c0); ret_effect(gb); return; }
  CYC(0x42bf, 0x42c0); CYC(0x42c0, 0x42c3); A = W8(wFileSelect_cursorPos);
  CYC(0x42c3, 0x42c6); SET_HL(0x42d0);
  CYC(0x42c6, 0x42c7); push_effect(gb, 0x42c7); add_a_to_hl(gb);
  CYC(0x42c7, 0x42c8); A = mem_rd(gb, HL);
  CALL_C(0x42c8, setFileSelectMode_hook, 0x415b, 0x42cb);
  CYC(0x42cb, 0x42cd); A = 0x56;
  CYC(0x42cd, 0x42d0); playSound_b00_hook(gb);
}

void fileSelectMode5__upOrDown_hook(GB *gb) {
  CYC(0x42d3, 0x42d6); SET_HL(wFileSelect_cursorPos);
  CYC(0x42d6, 0x42d7); A = mem_rd(gb, HL);
  do {
    CYC(0x42d7, 0x42d8); alu_add(gb, C);
    CYC(0x42d8, 0x42da); alu_and(gb, 3);
    CYC(0x42da, 0x42dc); alu_cp(gb, 3);
    if (!(F & FC)) { CYCT(0x42dc, 0x42de); continue; }
    CYC(0x42dc, 0x42de);
    break;
  } while (true);
  CYC(0x42de, 0x42df); mem_wr(gb, HL, A);
  CYC(0x42df, 0x42e1); A = 0x84;
  CYC(0x42e1, 0x42e4); playSound_b00_hook(gb);
}

void fileSelectMode3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x42e4, fileSelectMode3__mode3Update_hook, 0x42ea, 0x42e7);
  CYC(0x42e7, 0x42ea); fileSelectDrawAcornCursor_hook(gb);
}

void fileSelectMode3__mode3Update_hook(GB *gb) {
  CYC(0x42ea, 0x42ed); A = W8(wFileSelect_mode2);
  CYC(0x42ed, 0x42ee); push_effect(gb, 0x42ee);
  switch (function_caller_jump_table(gb)) {
    case 0x42f6: fileSelectMode3__mode0_hook(gb); return;
    case 0x4314: fileSelectMode3__mode1_hook(gb); return;
    case 0x4352: fileSelectMode3__mode2_hook(gb); return;
    case 0x4371: fileSelectMode3__mode3_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void fileSelectMode3__mode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x42f6, setFileSelectCursorOffsetToFileSelectMode_hook, 0x4152, 0x42f9);
  CYC(0x42f9, 0x42fb); A = 3;
  CALL_C(0x42fb, func_02_4149_hook, 0x4149, 0x42fe);
  CALL_C(0x42fe, disableLcd_hook, 0x02c1, 0x4301);
  CYC(0x4301, 0x4303); A = 0xa3;
  CALL_C(0x4303, loadGfxHeader_hook, 0x0626, 0x4306);
  CALL_C(0x4306, loadFileDisplayVariables_hook, 0x49da, 0x4309);
  CALL_C(0x4309, textInput_updateEntryCursor_hook, 0x4a22, 0x430c);
  CYC(0x430c, 0x430e); A = 8;
  CALL_C(0x430e, loadUncompressedGfxHeader_hook, 0x05da, 0x4311);
  CYC(0x4311, 0x4314); loadGfxRegisterState5AndIncFileSelectMode2_hook(gb);
}

void fileSelectMode3__mode1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4314, fileSelectUpdateInput_hook, 0x4467, 0x4317);
  if (F & FZ) { CYCT(0x4317, 0x4318); ret_effect(gb); return; }
  CYC(0x4317, 0x4318); CYC(0x4318, 0x431a); A = 0x56;
  CALL_C(0x431a, playSound_b00_hook, 0x0c98, 0x431d);
  CYC(0x431d, 0x4320); A = W8(wFileSelect_cursorPos);
  CYC(0x4320, 0x4322); alu_cp(gb, 3);
  if (F & FZ) { CYCT(0x4322, 0x4325); setFileSelectModeTo1_hook(gb); return; }
  CYC(0x4322, 0x4325); CYC(0x4325, 0x4327); H8(hActiveFileSlot) = A;
  CYC(0x4327, 0x4329); D = 0;
  CALL_C(0x4329, getFileDisplayVariableAddress_hook, 0x417a, 0x432c);
  CYC(0x432c, 0x432e); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x432e, 0x4330); goto copy_selected; }
  CYC(0x432e, 0x4330); CYC(0x4330, 0x4332); A = 0x5a;
  CYC(0x4332, 0x4335); playSound_b00_hook(gb);
  return;
copy_selected:
  CYC(0x4335, 0x4336); alu_xor(gb, A);
  CYC(0x4336, 0x4339); W8(wFileSelect_cursorOffset) = A;
  CALL_C(0x4339, func_02_4149_hook, 0x4149, 0x433c);
  CALL_C(0x433c, incFileSelectMode2_hook, 0x416a, 0x433f);
  CYC(0x433f, 0x4341); B = 1;
  CYC(0x4341, 0x4342); A = B;
  CYC(0x4342, 0x4343); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4343, 0x4344); ret_effect(gb); return; }
  CYC(0x4343, 0x4344);
collision_check:
  CYC(0x4344, 0x4347); SET_HL(wFileSelect_cursorPos);
  CYC(0x4347, 0x4349); A = H8(hActiveFileSlot);
  CYC(0x4349, 0x434a); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x434a, 0x434b); ret_effect(gb); return; }
  CYC(0x434a, 0x434b); CYC(0x434b, 0x434c); A = mem_rd(gb, HL);
  CYC(0x434c, 0x434d); alu_add(gb, B);
  CYC(0x434d, 0x434f); alu_and(gb, 3);
  CYC(0x434f, 0x4350); mem_wr(gb, HL, A);
  CYC(0x4350, 0x4352);
  goto collision_check;
}

void fileSelectMode3__mode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4352, fileSelectMode3__func_02_4397_hook, 0x4397, 0x4355);
  CALL_C(0x4355, decFileSelectMode2IfBPressed_hook, 0x416f, 0x4358);
  if (!(F & FZ)) { CYCT(0x4358, 0x435a); fileSelectMode3__label_02_015_hook(gb); return; }
  CYC(0x4358, 0x435a);
  CALL_C(0x435a, fileSelectUpdateInput_hook, 0x4467, 0x435d);
  if (F & FZ) { CYCT(0x435d, 0x435f); goto next_cursor; }
  CYC(0x435d, 0x435f); CYC(0x435f, 0x4361); A = 0x56;
  CALL_C(0x4361, playSound_b00_hook, 0x0c98, 0x4364);
  CYC(0x4364, 0x4367); A = W8(wFileSelect_cursorPos);
  CYC(0x4367, 0x4369); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(0x4369, 0x436c); incFileSelectMode2_hook(gb); return; }
  CYC(0x4369, 0x436c);
  CALL_C(0x436c, decFileSelectMode2_hook, 0x4175, 0x436f);
  CYC(0x436f, 0x4371);
  fileSelectMode3__label_02_015_hook(gb);
  return;
next_cursor:
  CYC(0x4341, 0x4342); A = B;
  CYC(0x4342, 0x4343); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4343, 0x4344); ret_effect(gb); return; }
  CYC(0x4343, 0x4344);
collision_check:
  CYC(0x4344, 0x4347); SET_HL(wFileSelect_cursorPos);
  CYC(0x4347, 0x4349); A = H8(hActiveFileSlot);
  CYC(0x4349, 0x434a); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x434a, 0x434b); ret_effect(gb); return; }
  CYC(0x434a, 0x434b); CYC(0x434b, 0x434c); A = mem_rd(gb, HL);
  CYC(0x434c, 0x434d); alu_add(gb, B); CYC(0x434d, 0x434f); alu_and(gb, 3);
  CYC(0x434f, 0x4350); mem_wr(gb, HL, A); CYC(0x4350, 0x4352);
  goto collision_check;
}

void fileSelectMode3__mode3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4371, fileSelectMode3__func_02_4397_hook, 0x4397, 0x4374);
  CALL_C(0x4374, decFileSelectMode2IfBPressed_hook, 0x416f, 0x4377);
  if (!(F & FZ)) { CYCT(0x4377, 0x4379); fileSelectMode3__label_02_015_hook(gb); return; }
  CYC(0x4377, 0x4379);
  CALL_C(0x4379, func_02_448d_hook, 0x448d, 0x437c);
  if (F & FZ) { CYCT(0x437c, 0x437d); ret_effect(gb); return; }
  CYC(0x437c, 0x437d); CYC(0x437d, 0x437f); A = 0x56;
  CALL_C(0x437f, playSound_b00_hook, 0x0c98, 0x4382);
  CYC(0x4382, 0x4385); A = W8(wFileSelect_cursorPos2);
  CYC(0x4385, 0x4386); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4386, 0x4389); setFileSelectModeTo1_hook(gb); return; }
  CYC(0x4386, 0x4389);
  CALL_C(0x4389, loadFile_b00_hook, 0x09dc, 0x438c);
  CYC(0x438c, 0x438f); A = W8(wFileSelect_cursorPos);
  CYC(0x438f, 0x4391); H8(hActiveFileSlot) = A;
  CALL_C(0x4391, saveFile_b00_hook, 0x09d8, 0x4394);
  CYC(0x4394, 0x4397); setFileSelectModeTo1_hook(gb);
}

void fileSelectMode3__func_02_4397_hook(GB *gb) {
  CYC(0x4397, 0x4399); A = H8(hActiveFileSlot);
  CYC(0x4399, 0x439c); SET_HL(0x43a6);
  CYC(0x439c, 0x439d); push_effect(gb, 0x439d); add_a_to_hl(gb);
  CYC(0x439d, 0x439e); B = mem_rd(gb, HL);
  CYC(0x439e, 0x43a0); C = 0;
  CYC(0x43a0, 0x43a3); SET_HL(0x43a9);
  CYC(0x43a3, 0x43a6); addSpritesToOam_withOffset_hook(gb);
}

void fileSelectMode3__label_02_015_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43c2, 0x43c4); A = 0x50;
  CALL_C(0x43c4, playSound_b00_hook, 0x0c98, 0x43c7);
  CYC(0x43c7, 0x43ca); A = W8(wFileSelect_mode2);
  CYC(0x43ca, 0x43cc); alu_cp(gb, 1);
  CYC(0x43cc, 0x43cf); A = W8(wFileSelect_cursorPos);
  if (!(F & FZ)) { CYCT(0x43cf, 0x43d1); goto update_cursor; }
  CYC(0x43cf, 0x43d1);
  CALL_C(0x43d1, setFileSelectCursorOffsetToFileSelectMode_hook, 0x4152, 0x43d4);
  CYC(0x43d4, 0x43d6); A = H8(hActiveFileSlot);
update_cursor:
  CYC(0x43d6, 0x43d9); func_02_4149_hook(gb);
}

void inventoryMenuState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5735, inventoryMenuState2__subStates_hook, 0x573b, 0x5738);
  CYC(0x5738, 0x573b); createBlankSpritesForItemSubmenu_hook(gb);
}

void inventoryMenuState2__subStates_hook(GB *gb) {
  CYC(0x573b, 0x573e); A = W8(wSubmenuState);
  CYC(0x573e, 0x573f); push_effect(gb, 0x573f);
  switch (function_caller_jump_table(gb)) {
    case 0x5745: inventoryMenuState2__subState0_hook(gb); return;
    case 0x578a: inventoryMenuState2__subState1_hook(gb); return;
    case 0x57a0: inventoryMenuState2__subState2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void inventoryMenuState2__subState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5745, 0x5748); SET_HL(wSelectedHarpSong);
  CYC(0x5748, 0x5749); D = mem_rd(gb, HL);
  CYC(0x5749, 0x574a); D = alu_dec8(gb, D);
  CYC(0x574a, 0x574c); L = (uint8_t)wSatchelSelectedSeeds;
  CALL_C(0x574c, cpInventorySelectedItemToHarp_hook, 0x5af6, 0x574f);
  if (F & FZ) { CYCT(0x574f, 0x5751); goto found; }
  CYC(0x574f, 0x5751); CYC(0x5751, 0x5753); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(0x5753, 0x5755); goto selected; }
  CYC(0x5753, 0x5755); CYC(0x5755, 0x5756); L = alu_inc8(gb, L);
selected:
  CYC(0x5756, 0x5757); E = mem_rd(gb, HL);
  CYC(0x5757, 0x5759); D = 0;
  do {
    CYC(0x5759, 0x575a); A = D;
    CALL_C(0x575a, getSeedTypeInventoryIndex_hook, 0x5b19, 0x575d);
    CYC(0x575d, 0x575e); alu_cp(gb, E);
    if (F & FZ) { CYCT(0x575e, 0x5760); break; }
    CYC(0x575e, 0x5760); CYC(0x5760, 0x5761); D = alu_inc8(gb, D);
    CYC(0x5761, 0x5763);
  } while (true);
found:
  CYC(0x5763, 0x5764); A = D;
  CYC(0x5764, 0x5767); W8(wInventory_itemSubmenuIndex) = A;
  CYC(0x5767, 0x576a); A = W8(wInventory_cbb8);
  CYC(0x576a, 0x576d); SET_HL(0x5821);
  CYC(0x576d, 0x576e); push_effect(gb, 0x576e); add_a_to_hl(gb);
  CYC(0x576e, 0x576f); A = mem_rd(gb, HL);
  CYC(0x576f, 0x5772); SET_HL(wInventory_itemSubmenuMaxWidth);
  CYC(0x5772, 0x5773); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5773, 0x5774); alu_xor(gb, A);
  CYC(0x5774, 0x5775); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5775, 0x5776); A = alu_inc8(gb, A);
  CYC(0x5776, 0x5777); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5777, 0x577a); W8(wInventory_itemSubmenuCounter) = A;
  CYC(0x577a, 0x577d); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x577d, 0x577f); alu_cp(gb, 8);
  CYC(0x577f, 0x5781); A = 0x0a;
  if (!(F & FC)) { CYCT(0x5781, 0x5783); goto store_width; }
  CYC(0x5781, 0x5783); CYC(0x5783, 0x5785); alu_add(gb, 0xa0);
store_width:
  CYC(0x5785, 0x5786); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5786, 0x5789); SET_HL(wSubmenuState);
  CYC(0x5789, 0x578a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  inventoryMenuState2__subState1_hook(gb);
}

void inventoryMenuState2__subState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x578a, 0x578d); SET_HL(wInventory_itemSubmenuCounter);
  CYC(0x578d, 0x578e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x578e, 0x578f); ret_effect(gb); return; }
  CYC(0x578e, 0x578f); CYC(0x578f, 0x5791); mem_wr(gb, HL, 2);
  CALL_C(0x5791, inventoryMenuState2__func_02_57f3_hook, 0x57f3, 0x5794);
  if (F & FC) { CYCT(0x5794, 0x5796); goto done; }
  CYC(0x5794, 0x5796); CALL_C(0x5796, func_02_5a35_hook, 0x5a35, 0x5799);
  CYC(0x5799, 0x579c); SET_HL(wSubmenuState);
  CYC(0x579c, 0x579d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
done:
  CYC(0x579d, 0x57a0); func_02_55a8_hook(gb);
}

void inventoryMenuState2__subState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x57a0, 0x57a3); A = W8(wKeysJustPressed);
  CYC(0x57a3, 0x57a5); alu_and(gb, 0x0b);
  if (!(F & FZ)) { CYCT(0x57a5, 0x57a7); inventoryMenuState2__buttonPressed_hook(gb); return; }
  CYC(0x57a5, 0x57a7); CALL_C(0x57a7, func_02_5938_hook, 0x5938, 0x57aa);
  CALL_C(0x57aa, cpInventorySelectedItemToHarp_hook, 0x5af6, 0x57ad);
  CYC(0x57ad, 0x57b0); A = W8(wInventory_itemSubmenuIndex);
  if (!(F & FZ)) { CYCT(0x57b0, 0x57b2); goto seed; }
  CYC(0x57b0, 0x57b2); CYC(0x57b2, 0x57b4); alu_add(gb, 0x25); CYC(0x57b4, 0x57b6); goto display;
seed:
  CALL_C(0x57b6, getSeedTypeInventoryIndex_hook, 0x5b19, 0x57b9);
  CYC(0x57b9, 0x57bb); alu_add(gb, 0x20);
display:
  CALL_C(0x57bb, loadTreasureDisplayData_b00_hook, 0x16d6, 0x57be);
  CYC(0x57be, 0x57c0); A = 6;
  CYC(0x57c0, 0x57c1); push_effect(gb, 0x57c1); add_a_to_hl(gb);
  CYC(0x57c1, 0x57c4); A = W8(wInventory_selectedItem);
  CYC(0x57c4, 0x57c6); alu_cp(gb, 0x0f);
  CYC(0x57c6, 0x57c8); A = 0;
  if (!(F & FZ)) { CYCT(0x57c8, 0x57ca); goto text; }
  CYC(0x57c8, 0x57ca); CYC(0x57ca, 0x57cc); A = 5;
text:
  CYC(0x57cc, 0x57cd); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x57cd, showItemText2_hook, 0x553d, 0x57d0);
  CYC(0x57d0, 0x57d3); func_02_5a35_hook(gb);
}

void inventoryMenuState2__buttonPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57d3, cpInventorySelectedItemToHarp_hook, 0x5af6, 0x57d6);
  if (!(F & FZ)) { CYCT(0x57d6, 0x57d8); goto seed; }
  CYC(0x57d6, 0x57d8); CYC(0x57d8, 0x57da); E = (uint8_t)wSelectedHarpSong;
  CYC(0x57da, 0x57dd); A = W8(wInventory_itemSubmenuIndex);
  CYC(0x57dd, 0x57de); A = alu_inc8(gb, A); CYC(0x57de, 0x57e0); goto set;
seed:
  CYC(0x57e0, 0x57e2); E = (uint8_t)wSatchelSelectedSeeds;
  CYC(0x57e2, 0x57e4); alu_cp(gb, 0x19);
  if (F & FZ) { CYCT(0x57e4, 0x57e6); goto index; }
  CYC(0x57e4, 0x57e6); CYC(0x57e6, 0x57e7); E = alu_inc8(gb, E);
index:
  CYC(0x57e7, 0x57ea); A = W8(wInventory_itemSubmenuIndex);
  CALL_C(0x57ea, getSeedTypeInventoryIndex_hook, 0x5b19, 0x57ed);
set:
  CYC(0x57ed, 0x57ef); D = (uint8_t)(wSatchelSelectedSeeds >> 8);
  CYC(0x57ef, 0x57f0); mem_wr(gb, DE, A);
  CYC(0x57f0, 0x57f3); inventoryMenuState1__finalizeEquip_hook(gb);
}

void inventoryMenuState2__func_02_57f3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x57f3, 0x57f6); SET_HL(wInventory_itemSubmenuMaxWidth);
  CYC(0x57f6, 0x57f7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x57f7, 0x57f8); C = A;
  CYC(0x57f8, 0x57f9); A = mem_rd(gb, HL);
  CYC(0x57f9, 0x57fa); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x57fa, 0x57fc); goto max; }
  CYC(0x57fa, 0x57fc); CYC(0x57fc, 0x57fe); alu_add(gb, 2);
  CYC(0x57fe, 0x57ff); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x57ff, 0x5800); SET_HL(HL + 1);
  CYC(0x5800, 0x5801); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5801, 0x5803); goto fill;
max:
  CYC(0x5803, 0x5804); SET_HL(HL + 1);
  CYC(0x5804, 0x5805); A = mem_rd(gb, HL);
  CYC(0x5805, 0x5807); alu_cp(gb, 4);
  if (!(F & FC)) { CYCT(0x5807, 0x5808); ret_effect(gb); return; }
  CYC(0x5807, 0x5808); CYC(0x5808, 0x5809); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
fill:
  CYC(0x5809, 0x580b); L = (uint8_t)wInventory_itemSubmenuWidth;
  CYC(0x580b, 0x580c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x580c, 0x580d); C = A;
  CYC(0x580d, 0x580e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x580e, 0x580f); B = A;
  CYC(0x580f, 0x5810); A = mem_rd(gb, HL);
  CYC(0x5810, 0x5813); SET_HL(w4TileMap + 0x80);
  CYC(0x5813, 0x5814); push_effect(gb, 0x5814); add_a_to_hl(gb);
  CYC(0x5814, 0x5817); SET_DE(0x0101);
  CYC(0x5817, 0x5818); A = B;
  CYC(0x5818, 0x581a); alu_cp(gb, 4);
  if (F & FZ) { CYCT(0x581a, 0x581c); goto rectangle; }
  CYC(0x581a, 0x581c); CYC(0x581c, 0x581e); E |= 0x80;
rectangle:
  CALL_C(0x581e, fillRectangleInTilemap_hook, 0x5d08, 0x5821);
  CYC(0x5821, 0x5822); alu_scf(gb);
  CYC(0x5822, 0x5823); ret_effect(gb);
}

void inventoryMenuState3_hook(GB *gb) {
  CYC(0x5827, 0x582a); A = W8(wSubmenuState);
  CYC(0x582a, 0x582b); push_effect(gb, 0x582b);
  switch (function_caller_jump_table(gb)) {
    case 0x5831: inventoryMenuState3__subState0_hook(gb); return;
    case 0x5855: inventoryMenuState3__subState1_hook(gb); return;
    case 0x586d: inventoryMenuState3__subState2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void inventoryMenuState3__subState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5831, 0x5834); SET_HL(wInventorySubmenu);
  CYC(0x5834, 0x5835); A = mem_rd(gb, HL);
  CYC(0x5835, 0x5836); A = alu_inc8(gb, A);
  CYC(0x5836, 0x5838); alu_cp(gb, 3);
  if (F & FC) { CYCT(0x5838, 0x583a); goto set_submenu; }
  CYC(0x5838, 0x583a); CYC(0x583a, 0x583b); alu_xor(gb, A);
set_submenu:
  CYC(0x583b, 0x583c); mem_wr(gb, HL, A);
  CYC(0x583c, 0x583f); A = W8(wInventory_cbba);
  CYC(0x583f, 0x5841); alu_xor(gb, 1);
  CYC(0x5841, 0x5844); W8(wInventory_cbba) = A;
  CALL_C(0x5844, func_02_55b2_hook, 0x55b2, 0x5847);
  CYC(0x5847, 0x5849); A = 0x9f;
  CYC(0x5849, 0x584c); W8(wGfxRegs2_WINX) = A;
  CYC(0x584c, 0x584f); SET_HL(wSubmenuState);
  CYC(0x584f, 0x5850); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5850, 0x5852); A = 0x54;
  CALL_C(0x5852, playSound_b00_hook, 0x0c98, 0x5855);
  inventoryMenuState3__subState1_hook(gb);
}

void inventoryMenuState3__subState1_hook(GB *gb) {
  CYC(0x5855, 0x5858); SET_BC(0x070c);
  CYC(0x5858, 0x585b); A = W8(wGfxRegs2_WINX);
  CYC(0x585b, 0x585c); alu_sub(gb, C);
  CYC(0x585c, 0x585d); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x585d, 0x585f); goto store; }
  CYC(0x585d, 0x585f); CYC(0x585f, 0x5860); A = B;
store:
  CYC(0x5860, 0x5863); W8(wGfxRegs2_WINX) = A;
  CYC(0x5863, 0x5866); A = W8(wGfxRegs2_SCX);
  CYC(0x5866, 0x5867); alu_add(gb, C);
  CYC(0x5867, 0x586a); W8(wGfxRegs2_SCX) = A;
  CYC(0x586a, 0x586c); alu_cp(gb, 0x98);
  if (F & FC) { CYCT(0x586c, 0x586d); ret_effect(gb); return; }
  CYC(0x586c, 0x586d);
  inventoryMenuState3__subState2_hook(gb);
}

void inventoryMenuState3__subState2_hook(GB *gb) {
  CYC(0x586d, 0x586f); A = 0xc7;
  CYC(0x586f, 0x5872); W8(wGfxRegs2_WINX) = A;
  CYC(0x5872, 0x5873); alu_xor(gb, A);
  CYC(0x5873, 0x5876); W8(wGfxRegs2_SCX) = A;
  CYC(0x5876, 0x5879); A = W8(wGfxRegs2_LCDC);
  CYC(0x5879, 0x587b); alu_xor(gb, 0x48);
  CYC(0x587b, 0x587e); W8(wGfxRegs2_LCDC) = A;
  CYC(0x587e, 0x5880); A = 1;
  CYC(0x5880, 0x5883); inventoryMenuState1__func_02_5606_hook(gb);
}

void getDirectionButtonOffsetFromHl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5883, getInputWithAutofire_hook, 0x0294, 0x5886);
  CYC(0x5886, 0x5888); alu_and(gb, 0xf0);
  CYC(0x5888, 0x588a); A = alu_swap(gb, A);
  CALL_C(0x588a, getLowestSetBit_hook, 0x01f8, 0x588d);
  if (!(F & FC)) { CYCT(0x588d, 0x588e); ret_effect(gb); return; }
  CYC(0x588d, 0x588e);
  CYC(0x588e, 0x588f); push_effect(gb, 0x588f); add_a_to_hl(gb);
  CYC(0x588f, 0x5890); A = mem_rd(gb, HL);
  CYC(0x5890, 0x5891); alu_or(gb, A);
  CYC(0x5891, 0x5892); alu_scf(gb);
  CYC(0x5892, 0x5893); ret_effect(gb);
}

void inventorySubscreen0CheckDirectionButtons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5893, 0x5896); SET_HL(0x58a6);
  CALL_C(0x5896, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x5899);
  if (!(F & FC)) { CYCT(0x5899, 0x589a); ret_effect(gb); return; }
  CYC(0x5899, 0x589a); CYC(0x589a, 0x589d); SET_HL(wInventorySubmenu0CursorPos);
  CYC(0x589d, 0x589e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x589e, 0x58a0); alu_and(gb, 0x0f);
  CYC(0x58a0, 0x58a1); mem_wr(gb, HL, A);
  CYC(0x58a1, 0x58a3); A = 0x84;
  CYC(0x58a3, 0x58a6); playSound_b00_hook(gb);
}

void inventorySubmenu1CheckDirectionButtons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58aa, 0x58ad); SET_HL(0x5904);
  CALL_C(0x58ad, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x58b0);
  if (!(F & FC)) { CYCT(0x58b0, 0x58b1); ret_effect(gb); return; }
  CYC(0x58b0, 0x58b1);
  CYC(0x58b1, 0x58b2); C = A;
  CYC(0x58b2, 0x58b3); B = A;
  CYC(0x58b3, 0x58b4); B = alu_inc8(gb, B);
  CALL_C(0x58b4, getRingBoxCapacity_hook, 0x5cf5, 0x58b7);
  CYC(0x58b7, 0x58b9); E = 0x0f;
  if (F & FZ) { CYCT(0x58b9, 0x58bb); goto capacity; }
  CYC(0x58b9, 0x58bb); CYC(0x58bb, 0x58bc); A = alu_inc8(gb, A);
capacity:
  CYC(0x58bc, 0x58bd); alu_add(gb, E);
  CYC(0x58bd, 0x58be); D = A;
  CYC(0x58be, 0x58c1); SET_HL(wInventorySubmenu1CursorPos);
  CYC(0x58c1, 0x58c2); A = mem_rd(gb, HL);
  CYC(0x58c2, 0x58c4); alu_bit(gb, 2, B);
  if (!(F & FZ)) { CYCT(0x58c4, 0x58c6); inventorySubmenu1CheckDirectionButtons__upOrDown_hook(gb); return; }
  CYC(0x58c4, 0x58c6); inventorySubmenu1CheckDirectionButtons__leftOrRight_hook(gb);
}

void inventorySubmenu1CheckDirectionButtons__leftOrRight_hook(GB *gb) {
  do {
    CYC(0x58c6, 0x58c7); alu_add(gb, C);
    CYC(0x58c7, 0x58c8); alu_cp(gb, D);
    if (!(F & FC)) { CYCT(0x58c8, 0x58ca); continue; }
    CYC(0x58c8, 0x58ca); break;
  } while (true);
  CYC(0x58ca, 0x58cc);
  CYC(0x58db, 0x58dc); mem_wr(gb, HL, A);
  CYC(0x58dc, 0x58de); A = 0x84;
  CYC(0x58de, 0x58e1); playSound_b00_hook(gb);
}

void inventorySubmenu1CheckDirectionButtons__upOrDown_hook(GB *gb) {
  CYC(0x58cc, 0x58cd); alu_cp(gb, E);
  if (!(F & FC)) { CYCT(0x58cd, 0x58cf); inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow_hook(gb); return; }
  CYC(0x58cd, 0x58cf); CYC(0x58cf, 0x58d0); alu_add(gb, C);
  CYC(0x58d0, 0x58d1); alu_cp(gb, E);
  if (F & FC) { CYCT(0x58d1, 0x58d3); goto store; }
  CYC(0x58d1, 0x58d3); inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow_hook(gb); return;
store:
  CYC(0x58db, 0x58dc); mem_wr(gb, HL, A);
  CYC(0x58dc, 0x58de); A = 0x84;
  CYC(0x58de, 0x58e1); playSound_b00_hook(gb);
}

void inventorySubmenu1CheckDirectionButtons__upOrDown__ringBoxRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58d3, 0x58d4); A = mem_rd(gb, HL);
  do {
    CYC(0x58d4, 0x58d5); C = A;
    CALL_C(0x58d5, inventorySubmenu1CheckDirectionButtons__updateCursorOnRingBoxRow_hook, 0x58e1, 0x58d8);
    CYC(0x58d8, 0x58d9); alu_cp(gb, D);
    if (!(F & FC)) { CYCT(0x58d9, 0x58db); continue; }
    CYC(0x58d9, 0x58db); break;
  } while (true);
  CYC(0x58db, 0x58dc); mem_wr(gb, HL, A);
  CYC(0x58dc, 0x58de); A = 0x84;
  CYC(0x58de, 0x58e1); playSound_b00_hook(gb);
}

void inventorySubmenu1CheckDirectionButtons__updateCursorOnRingBoxRow_hook(GB *gb) {
  CYC(0x58e1, 0x58e2); push_effect(gb, HL);
  CYC(0x58e2, 0x58e5); SET_HL(0x58f2);
  do {
    CYC(0x58e5, 0x58e6); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x58e6, 0x58e7); alu_cp(gb, C);
    if (!(F & FZ)) { CYCT(0x58e7, 0x58e9); continue; }
    CYC(0x58e7, 0x58e9); break;
  } while (true);
  CYC(0x58e9, 0x58eb); alu_bit(gb, 3, B);
  if (F & FZ) { CYCT(0x58eb, 0x58ed); goto result; }
  CYC(0x58eb, 0x58ed); CYC(0x58ed, 0x58ee); SET_HL(HL - 1);
  CYC(0x58ee, 0x58ef); SET_HL(HL - 1);
result:
  CYC(0x58ef, 0x58f0); A = mem_rd(gb, HL);
  CYC(0x58f0, 0x58f1); SET_HL(pop_effect(gb));
  CYC(0x58f1, 0x58f2); ret_effect(gb);
}

void inventorySubmenu2CheckDirectionButtons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5908, 0x590b); SET_HL(0x5934);
  CALL_C(0x590b, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x590e);
  if (!(F & FC)) { CYCT(0x590e, 0x590f); ret_effect(gb); return; }
  CYC(0x590e, 0x590f); CYC(0x590f, 0x5912); SET_HL(wInventorySubmenu2CursorPos);
  CYC(0x5912, 0x5913); C = A;
  CYC(0x5913, 0x5915); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x5915, 0x5917); inventorySubmenu2CheckDirectionButtons__upOrDown_hook(gb); return; }
  CYC(0x5915, 0x5917); inventorySubmenu2CheckDirectionButtons__leftOrRight_hook(gb);
}

void inventorySubmenu2CheckDirectionButtons__leftOrRight_hook(GB *gb) {
  CYC(0x5917, 0x5918); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x5918, 0x591a);
  CYC(0x592e, 0x592f); mem_wr(gb, HL, A);
  CYC(0x592f, 0x5931); A = 0x84;
  CYC(0x5931, 0x5934); playSound_b00_hook(gb);
}

void inventorySubmenu2CheckDirectionButtons__upOrDown_hook(GB *gb) {
  CYC(0x591a, 0x591c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x591c, 0x591e); inventorySubmenu2CheckDirectionButtons__upOrDown__leftSide_hook(gb); return; }
  CYC(0x591c, 0x591e); inventorySubmenu2CheckDirectionButtons__upOrDown__rightSide_hook(gb);
}

void inventorySubmenu2CheckDirectionButtons__upOrDown__rightSide_hook(GB *gb) {
  CYC(0x591e, 0x5921); SET_HL(wInventory_submenu2CursorPos2);
  CYC(0x5921, 0x5922); A = mem_rd(gb, HL);
  do {
    CYC(0x5922, 0x5923); alu_add(gb, C);
    CYC(0x5923, 0x5925); alu_and(gb, 3);
    CYC(0x5925, 0x5927); alu_cp(gb, 3);
    if (!(F & FC)) { CYCT(0x5927, 0x5929); continue; }
    CYC(0x5927, 0x5929); break;
  } while (true);
  CYC(0x5929, 0x592b);
  CYC(0x592e, 0x592f); mem_wr(gb, HL, A);
  CYC(0x592f, 0x5931); A = 0x84;
  CYC(0x5931, 0x5934); playSound_b00_hook(gb);
}

void inventorySubmenu2CheckDirectionButtons__upOrDown__leftSide_hook(GB *gb) {
  CYC(0x592b, 0x592c); alu_add(gb, mem_rd(gb, HL));
  CYC(0x592c, 0x592e); alu_and(gb, 7);
  CYC(0x592e, 0x592f); mem_wr(gb, HL, A);
  CYC(0x592f, 0x5931); A = 0x84;
  CYC(0x5931, 0x5934); playSound_b00_hook(gb);
}

void func_02_5938_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5938, 0x593b); A = W8(wInventory_cbb8);
  CYC(0x593b, 0x593c); B = A;
  CYC(0x593c, 0x593f); SET_HL(0x595a);
  CALL_C(0x593f, getDirectionButtonOffsetFromHl_hook, 0x5883, 0x5942);
  if (!(F & FC)) { CYCT(0x5942, 0x5943); ret_effect(gb); return; }
  CYC(0x5942, 0x5943);
  if (F & FZ) { CYCT(0x5943, 0x5944); ret_effect(gb); return; }
  CYC(0x5943, 0x5944); CYC(0x5944, 0x5947); SET_HL(wTmpcbb5);
  CYC(0x5947, 0x5948); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5948, 0x594a); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x594a, 0x594c); goto decrement; }
  CYC(0x594a, 0x594c); CYC(0x594c, 0x594d); alu_cp(gb, B);
  if (F & FC) { CYCT(0x594d, 0x594f); goto store; }
  CYC(0x594d, 0x594f); CYC(0x594f, 0x5950); alu_xor(gb, A); CYC(0x5950, 0x5952); goto store;
decrement:
  CYC(0x5952, 0x5953); A = B;
  CYC(0x5953, 0x5954); A = alu_dec8(gb, A);
store:
  CYC(0x5954, 0x5955); mem_wr(gb, HL, A);
  CYC(0x5955, 0x5957); A = 0x84;
  CYC(0x5957, 0x595a); playSound_b00_hook(gb);
}

void inventorySubscreen0_drawCursor_hook(GB *gb) {
  CYC(0x595e, 0x5961); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x5961, 0x5962); C = A;
  CYC(0x5962, 0x5964); alu_and(gb, 0x0c);
  CYC(0x5964, 0x5965); alu_rrca(gb);
  CYC(0x5965, 0x5966); alu_rrca(gb);
  CYC(0x5966, 0x5968); A = alu_swap(gb, A);
  CYC(0x5968, 0x5969); B = A;
  CYC(0x5969, 0x596a); alu_rrca(gb);
  CYC(0x596a, 0x596b); alu_add(gb, B);
  CYC(0x596b, 0x596c); B = A;
  CYC(0x596c, 0x596d); A = C;
  CYC(0x596d, 0x596f); alu_and(gb, 3);
  CYC(0x596f, 0x5971); A = alu_swap(gb, A);
  CYC(0x5971, 0x5972); alu_add(gb, A);
  CYC(0x5972, 0x5973); C = A;
  CYC(0x5973, 0x5976); SET_HL(0x5979);
  CYC(0x5976, 0x5979); addSpritesToOam_withOffset_hook(gb);
}

void inventorySubmenu1_drawCursor_hook(GB *gb) {
  CYC(0x5982, 0x5985); A = W8(wInventorySubmenu1CursorPos);
  CYC(0x5985, 0x5986); E = A;
  CYC(0x5986, 0x5989); SET_HL(0x59b5);
  CYC(0x5989, 0x598a); push_effect(gb, 0x598a); add_a_to_hl(gb);
  CYC(0x598a, 0x598b); A = mem_rd(gb, HL);
  CYC(0x598b, 0x598d); alu_and(gb, 0xf0);
  CYC(0x598d, 0x598e); alu_rrca(gb);
  CYC(0x598e, 0x598f); B = A;
  CYC(0x598f, 0x5990); A = mem_rd(gb, HL);
  CYC(0x5990, 0x5992); alu_and(gb, 0x0f);
  CYC(0x5992, 0x5994); A = alu_swap(gb, A);
  CYC(0x5994, 0x5995); alu_rrca(gb);
  CYC(0x5995, 0x5996); C = A;
  CYC(0x5996, 0x5998); D = 2;
  CYC(0x5998, 0x5999); A = E;
  CYC(0x5999, 0x599b); alu_cp(gb, 4);
  if (F & FZ) { CYCT(0x599b, 0x599d); goto sprites; }
  CYC(0x599b, 0x599d); CYC(0x599d, 0x599f); alu_cp(gb, 9);
  if (F & FZ) { CYCT(0x599f, 0x59a1); goto sprites; }
  CYC(0x599f, 0x59a1); CYC(0x59a1, 0x59a3); alu_sub(gb, 0x0e);
  if (F & FZ) { CYCT(0x59a3, 0x59a5); goto sprites; }
  CYC(0x59a3, 0x59a5); CYC(0x59a5, 0x59a6); D = alu_dec8(gb, D);
  CYC(0x59a6, 0x59a7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x59a7, 0x59a9); goto sprites; }
  CYC(0x59a7, 0x59a9); CYC(0x59a9, 0x59aa); D = alu_dec8(gb, D);
sprites:
  CYC(0x59aa, 0x59ab); A = D;
  CYC(0x59ab, 0x59ae); SET_HL(0x59ca);
  CYC(0x59ae, 0x59af); add_double_index_to_hl(gb, 0x59af);
  CYC(0x59af, 0x59b0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59b0, 0x59b1); H = mem_rd(gb, HL);
  CYC(0x59b1, 0x59b2); L = A;
  CYC(0x59b2, 0x59b5); addSpritesToOam_withOffset_hook(gb);
}

void inventorySubmenu2_drawCursor_hook(GB *gb) {
  CYC(0x59eb, 0x59ee); A = W8(wInventorySubmenu2CursorPos);
  CYC(0x59ee, 0x59f0); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x59f0, 0x59f2); goto position; }
  CYC(0x59f0, 0x59f2); CYC(0x59f2, 0x59f5); A = W8(wInventory_submenu2CursorPos2);
  CYC(0x59f5, 0x59f7); alu_add(gb, 8);
position:
  CYC(0x59f7, 0x59f8); E = A;
  CYC(0x59f8, 0x59fb); SET_HL(0x5a0d);
  CYC(0x59fb, 0x59fc); add_double_index_to_hl(gb, 0x59fc);
  CYC(0x59fc, 0x59fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59fd, 0x59fe); B = A;
  CYC(0x59fe, 0x59ff); C = mem_rd(gb, HL);
  CYC(0x59ff, 0x5a00); A = E;
  CYC(0x5a00, 0x5a02); alu_cp(gb, 8);
  CYC(0x5a02, 0x5a05); SET_HL(0x5a23);
  if (F & FC) { CYCT(0x5a05, 0x5a07); goto draw; }
  CYC(0x5a05, 0x5a07); CYC(0x5a07, 0x5a0a); SET_HL(0x5a2c);
draw:
  CYC(0x5a0a, 0x5a0d); addSpritesToOam_withOffset_hook(gb);
}

void func_02_5a35_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a35, 0x5a38); SET_DE(0x0500);
  CALL_C(0x5a38, cpInventorySelectedItemToHarp_hook, 0x5af6, 0x5a3b);
  if (!(F & FZ)) { CYCT(0x5a3b, 0x5a3d); goto selected; }
  CYC(0x5a3b, 0x5a3d);
  CYC(0x5a3d, 0x5a40); SET_DE(0x0305);
selected:
  CYC(0x5a40, 0x5a41); B = D;
  CYC(0x5a41, 0x5a43); D = 0;
  func_02_5a35__next_hook(gb);
}

void func_02_5a35__next_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a43, 0x5a44); push_effect(gb, BC);
  CYC(0x5a44, 0x5a45); A = E;
  CYC(0x5a45, 0x5a48); SET_HL(wObtainedTreasureFlags + 4);
  CALL_C(0x5a48, checkFlag_hook, 0x0205, 0x5a4b);
  if (F & FZ) {
    CYCT(0x5a4b, 0x5a4d);
    func_02_5a35__dontHaveSubItem_hook(gb);
    return;
  }
  CYC(0x5a4b, 0x5a4d);
  CYC(0x5a4d, 0x5a4e); push_effect(gb, DE);
  CYC(0x5a4e, 0x5a4f); A = D;
  CALL_C(0x5a4f, func_02_5afc_hook, 0x5afc, 0x5a52);
  CYC(0x5a52, 0x5a53); A = E;
  CYC(0x5a53, 0x5a56); SET_HL(0x5aa9);
  CYC(0x5a56, 0x5a57); push_effect(gb, 0x5a57); add_a_to_hl(gb);
  CYC(0x5a57, 0x5a58); A = mem_rd(gb, HL);
  CYC(0x5a58, 0x5a59); push_effect(gb, 0x5a59); add_a_to_hl(gb);
  CALL_C(0x5a59, addSpritesToOam_withOffset_hook, 0x0d61, 0x5a5c);
  CYC(0x5a5c, 0x5a5d); SET_DE(pop_effect(gb));
  CYC(0x5a5d, 0x5a5e); A = E;
  CYC(0x5a5e, 0x5a60); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(0x5a60, 0x5a62);
    func_02_5a35__seedOnlyCodeDone_hook(gb);
    return;
  }
  CYC(0x5a60, 0x5a62);
  CYC(0x5a62, 0x5a63); A = E;
  CYC(0x5a63, 0x5a66); SET_HL(wNumEmberSeeds);
  CYC(0x5a66, 0x5a67); push_effect(gb, 0x5a67); add_a_to_hl(gb);
  CYC(0x5a67, 0x5a68); B = mem_rd(gb, HL);
  CYC(0x5a68, 0x5a6b); A = W8(wInventory_cbb8);
  CYC(0x5a6b, 0x5a6e); SET_HL(0x5ae1);
  CYC(0x5a6e, 0x5a6f); add_double_index_to_hl(gb, 0x5a6f);
  CYC(0x5a6f, 0x5a70); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5a70, 0x5a71); H = mem_rd(gb, HL);
  CYC(0x5a71, 0x5a72); L = A;
  CYC(0x5a72, 0x5a73); A = D;
  CYC(0x5a73, 0x5a74); push_effect(gb, 0x5a74); add_a_to_hl(gb);
  CYC(0x5a74, 0x5a75); C = mem_rd(gb, HL);
  CYC(0x5a75, 0x5a78); SET_HL(w4TileMap + 0xc0);
  CYC(0x5a78, 0x5a7b); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x5a7b, 0x5a7d); alu_cp(gb, 0x08);
  if (!(F & FC)) CYCT(0x5a7d, 0x5a7f);
  else {
    CYC(0x5a7d, 0x5a7f);
    CYC(0x5a7f, 0x5a82); SET_HL(w4TileMap + 0x160);
  }
  CYC(0x5a82, 0x5a83); A = C;
  CYC(0x5a83, 0x5a84); push_effect(gb, 0x5a84); add_a_to_hl(gb);
  CYC(0x5a84, 0x5a85); A = B;
  CYC(0x5a85, 0x5a87); alu_and(gb, 0xf0);
  CYC(0x5a87, 0x5a89); A = alu_swap(gb, A);
  CYC(0x5a89, 0x5a8b); alu_add(gb, 0x20);
  CYC(0x5a8b, 0x5a8c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5a8c, 0x5a8d); A = B;
  CYC(0x5a8d, 0x5a8f); alu_and(gb, 0x0f);
  CYC(0x5a8f, 0x5a91); alu_add(gb, 0x20);
  CYC(0x5a91, 0x5a92); mem_wr(gb, HL, A); SET_HL(HL - 1);
  func_02_5a35__seedOnlyCodeDone_hook(gb);
}

void func_02_5a35__seedOnlyCodeDone_hook(GB *gb) {
  CYC(0x5a92, 0x5a93); D = alu_inc8(gb, D);
  func_02_5a35__dontHaveSubItem_hook(gb);
}

void func_02_5a35__dontHaveSubItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a93, 0x5a94); E = alu_inc8(gb, E);
  CYC(0x5a94, 0x5a95); SET_BC(pop_effect(gb));
  CYC(0x5a95, 0x5a96); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5a96, 0x5a98);
    func_02_5a35__next_hook(gb);
    return;
  }
  CYC(0x5a96, 0x5a98);
  CYC(0x5a98, 0x5a9b); A = W8(wTmpcbb5);
  CALL_C(0x5a9b, func_02_5afc_hook, 0x5afc, 0x5a9e);
  CYC(0x5a9e, 0x5aa1); SET_HL(0x5aa4);
  CYC(0x5aa1, 0x5aa4); addSpritesToOam_withOffset_hook(gb);
}

void cpInventorySelectedItemToHarp_hook(GB *gb) {
  CYC(0x5af6, 0x5af9); A = W8(wInventory_selectedItem);
  CYC(0x5af9, 0x5afb); alu_cp(gb, 0x11);
  CYC(0x5afb, 0x5afc); ret_effect(gb);
}

void func_02_5afc_hook(GB *gb) {
  CYC(0x5afc, 0x5afd); C = A;
  CYC(0x5afd, 0x5b00); A = W8(wInventory_cbb8);
  CYC(0x5b00, 0x5b03); SET_HL(0x5ae1);
  CYC(0x5b03, 0x5b04); add_double_index_to_hl(gb, 0x5b04);
  CYC(0x5b04, 0x5b05); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5b05, 0x5b06); H = mem_rd(gb, HL);
  CYC(0x5b06, 0x5b07); L = A;
  CYC(0x5b07, 0x5b08); A = C;
  CYC(0x5b08, 0x5b09); push_effect(gb, 0x5b09); add_a_to_hl(gb);
  CYC(0x5b09, 0x5b0a); A = mem_rd(gb, HL);
  CYC(0x5b0a, 0x5b0c); A = alu_swap(gb, A);
  CYC(0x5b0c, 0x5b0d); alu_rrca(gb);
  CYC(0x5b0d, 0x5b0e); C = A;
  CYC(0x5b0e, 0x5b10); B = 0x20;
  CYC(0x5b10, 0x5b13); A = W8(wInventorySubmenu0CursorPos);
  CYC(0x5b13, 0x5b15); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(0x5b15, 0x5b16); ret_effect(gb); return; }
  CYC(0x5b15, 0x5b16);
  CYC(0x5b16, 0x5b18); B = 0x48;
  CYC(0x5b18, 0x5b19); ret_effect(gb);
}

void getSeedTypeInventoryIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b19, 0x5b1a); C = A;
  CYC(0x5b1a, 0x5b1b); C = alu_inc8(gb, C);
  CYC(0x5b1b, 0x5b1e); SET_HL(wObtainedTreasureFlags + 4);
  CYC(0x5b1e, 0x5b1f); alu_xor(gb, A);
  for (;;) {
    CYC(0x5b1f, 0x5b20); B = A;
    CALL_C(0x5b20, checkFlag_hook, 0x0205, 0x5b23);
    if (F & FZ) CYCT(0x5b23, 0x5b25);
    else {
      CYC(0x5b23, 0x5b25);
      CYC(0x5b25, 0x5b26); C = alu_dec8(gb, C);
      if (F & FZ) {
        CYCT(0x5b26, 0x5b28);
        CYC(0x5b2c, 0x5b2d); A = B;
        CYC(0x5b2d, 0x5b2e); ret_effect(gb);
        return;
      }
      CYC(0x5b26, 0x5b28);
    }
    CYC(0x5b28, 0x5b29); A = B;
    CYC(0x5b29, 0x5b2a); A = alu_inc8(gb, A);
    CYC(0x5b2a, 0x5b2c);
  }
}

void drawEquippedSpriteForActiveRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b2e, getRingBoxCapacity_hook, 0x5cf5, 0x5b31);
  if (F & FZ) { CYCT(0x5b31, 0x5b32); ret_effect(gb); return; }
  CYC(0x5b31, 0x5b32);
  CYC(0x5b32, 0x5b33); B = A;
  CYC(0x5b33, 0x5b36); A = W8(wActiveRing);
  CYC(0x5b36, 0x5b38); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x5b38, 0x5b39); ret_effect(gb); return; }
  CYC(0x5b38, 0x5b39);
  CYC(0x5b39, 0x5b3c); SET_HL(wRingBoxContents);
  CYC(0x5b3c, 0x5b3e); C = 0;
  do {
    CYC(0x5b3e, 0x5b3f); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x5b3f, 0x5b41);
      drawEquippedSpriteForActiveRing__foundRing_hook(gb);
      return;
    }
    CYC(0x5b3f, 0x5b41);
    CYC(0x5b41, 0x5b42); SET_HL(HL + 1);
    CYC(0x5b42, 0x5b43); C = alu_inc8(gb, C);
    CYC(0x5b43, 0x5b44); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x5b44, 0x5b46); else CYC(0x5b44, 0x5b46);
  } while (!(F & FZ));
  CYC(0x5b46, 0x5b47); ret_effect(gb);
}

void drawEquippedSpriteForActiveRing__foundRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b47, 0x5b49); A = 0x18;
  CALL_C(0x5b49, multiplyAByC_hook, 0x019d, 0x5b4c);
  CYC(0x5b4c, 0x5b4d); C = L;
  CYC(0x5b4d, 0x5b4f); B = 0;
  CYC(0x5b4f, 0x5b52); SET_HL(0x5b55);
  CYC(0x5b52, 0x5b55); addSpritesToOam_withOffset_hook(gb);
}

void inventorySubscreen0_drawStoredItems_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b5a, 0x5b5c); A = 0x10;
  do {
    CYC(0x5b5c, 0x5b5e); H8(hFF8D) = A;
    CYC(0x5b5e, 0x5b61); SET_HL(wInventoryStorage - 1);
    CYC(0x5b61, 0x5b62); push_effect(gb, 0x5b62); add_a_to_hl(gb);
    CYC(0x5b62, 0x5b63); A = mem_rd(gb, HL);
    CALL_C(0x5b63, loadTreasureDisplayData_b00_hook, 0x16d6, 0x5b66);
    CYC(0x5b66, 0x5b67); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(0x5b67, checkTreasureObtained_hook, 0x1748, 0x5b6a);
    CYC(0x5b6a, 0x5b6c); H8(hFF8B) = A;
    CYC(0x5b6c, 0x5b6e); A = H8(hFF8D);
    CYC(0x5b6e, 0x5b71); SET_BC(0x5b80);
    CALL_C(0x5b71, addDoubleIndexToBc_hook, 0x007e, 0x5b74);
    CYC(0x5b74, 0x5b75); A = mem_rd(gb, BC);
    CYC(0x5b75, 0x5b76); E = A;
    CYC(0x5b76, 0x5b77); SET_BC(BC + 1);
    CYC(0x5b77, 0x5b78); A = mem_rd(gb, BC);
    CYC(0x5b78, 0x5b79); D = A;
    CALL_C(0x5b79, drawTreasureDisplayDataToBg_hook, 0x5d1c, 0x5b7c);
    CYC(0x5b7c, 0x5b7e); A = H8(hFF8D);
    CYC(0x5b7e, 0x5b7f); A = alu_dec8(gb, A);
    if (!(F & FZ)) CYCT(0x5b7f, 0x5b81); else CYC(0x5b7f, 0x5b81);
  } while (!(F & FZ));
  CYC(0x5b81, 0x5b82); ret_effect(gb);
}

void inventorySubscreen1_drawTreasures_hook(GB *gb) {
  CYC(0x5ba2, 0x5ba5); SET_HL(0x5e92);
  inventorySubscreen1_drawTreasures__drawTreasure_hook(gb);
}

void inventorySubscreen1_drawTreasures__drawTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ba5, 0x5ba6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5ba6, 0x5ba7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5ba7, 0x5ba9);
    inventorySubscreen1_drawTreasures__undrawRingBox_hook(gb);
    return;
  }
  CYC(0x5ba7, 0x5ba9);
  CYC(0x5ba9, 0x5bab); H8(hFF8C) = A;
  CALL_C(0x5bab, checkTreasureObtained_hook, 0x1748, 0x5bae);
  if (!(F & FC)) {
    CYCT(0x5bae, 0x5bb0);
    inventorySubscreen1_drawTreasures__nextTreasure_hook(gb);
    return;
  }
  CYC(0x5bae, 0x5bb0);
  CYC(0x5bb0, 0x5bb2); H8(hFF8B) = A;
  CYC(0x5bb2, 0x5bb3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x5bb3, inventorySubscreen1_drawTreasures__getAddressToDrawTreasureAt_hook, 0x5c1e, 0x5bb6);
  CYC(0x5bb6, 0x5bb7); push_effect(gb, HL);
  CYC(0x5bb7, 0x5bb9); A = H8(hFF8C);
  CALL_C(0x5bb9, loadTreasureDisplayData_b00_hook, 0x16d6, 0x5bbc);
  CYC(0x5bbc, 0x5bbd); SET_HL(HL + 1);
  CALL_C(0x5bbd, drawTreasureDisplayDataToBg_hook, 0x5d1c, 0x5bc0);
  CYC(0x5bc0, 0x5bc1); C = mem_rd(gb, HL);
  CYC(0x5bc1, 0x5bc2); SET_HL(pop_effect(gb));
  CYC(0x5bc2, 0x5bc3); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5bc3, 0x5bc6); SET_DE(w4SubscreenTextIndices);
  CALL_C(0x5bc6, addAToDe_hook, 0x0068, 0x5bc9);
  CYC(0x5bc9, 0x5bca); A = C;
  CYC(0x5bca, 0x5bcb); mem_wr(gb, DE, A);
  inventorySubscreen1_drawTreasures__nextTreasure_hook(gb);
}

void inventorySubscreen1_drawTreasures__nextTreasure_hook(GB *gb) {
  CYC(0x5bcb, 0x5bcc); SET_HL(HL + 1);
  CYC(0x5bcc, 0x5bcd); SET_HL(HL + 1);
  CYC(0x5bcd, 0x5bcf);
  inventorySubscreen1_drawTreasures__drawTreasure_hook(gb);
}

void inventorySubscreen1_drawTreasures__undrawRingBox_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5bcf, 0x5bd2); A = W8(wRingBoxLevel);
  CYC(0x5bd2, 0x5bd4); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x5bd4, 0x5bd6);
    inventorySubscreen1_drawTreasures__drawRings_hook(gb);
    return;
  }
  CYC(0x5bd4, 0x5bd6);
  CYC(0x5bd6, 0x5bd9); SET_HL(0x5c37);
  CYC(0x5bd9, 0x5bda); add_double_index_to_hl(gb, 0x5bda);
  CYC(0x5bda, 0x5bdb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5bdb, 0x5bdc); C = mem_rd(gb, HL);
  CYC(0x5bdc, 0x5bde); B = 0x03;
  CYC(0x5bde, 0x5bdf); L = A;
  CYC(0x5bdf, 0x5be1); H = (uint8_t)((w4TileMap + 0x100) >> 8);
  CALL_C(0x5be1, fillRectangleInTileMapWithMenuBlock_hook, 0x5d05, 0x5be4);
  inventorySubscreen1_drawTreasures__drawRings_hook(gb);
}

void inventorySubscreen1_drawTreasures__drawRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5be4, getRingBoxCapacity_hook, 0x5cf5, 0x5be7);
  if (F & FZ) { CYCT(0x5be7, 0x5be8); ret_effect(gb); return; }
  CYC(0x5be7, 0x5be8);
  CYC(0x5be8, 0x5be9); B = A;
  inventorySubscreen1_drawTreasures__drawRing_hook(gb);
}

void inventorySubscreen1_drawTreasures__drawRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5be9, 0x5bea); A = B;
  CYC(0x5bea, 0x5bed); SET_HL(0x5c31);
  CYC(0x5bed, 0x5bee); push_effect(gb, 0x5bee); add_a_to_hl(gb);
  CYC(0x5bee, 0x5bef); E = mem_rd(gb, HL);
  CYC(0x5bef, 0x5bf1); D = (uint8_t)((w4TileMap + 0x100) >> 8);
  CYC(0x5bf1, 0x5bf2); A = B;
  CYC(0x5bf2, 0x5bf5); SET_HL(wRingBoxContents - 1);
  CYC(0x5bf5, 0x5bf6); push_effect(gb, 0x5bf6); add_a_to_hl(gb);
  CYC(0x5bf6, 0x5bf7); A = mem_rd(gb, HL);
  CYC(0x5bf7, 0x5bf9); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x5bf9, 0x5bfb);
    inventorySubscreen1_drawTreasures__nextRing_hook(gb);
    return;
  }
  CYC(0x5bf9, 0x5bfb);
  CYC(0x5bfb, 0x5bfc); push_effect(gb, BC);
  CYC(0x5bfc, 0x5bfd); C = A;
  CYC(0x5bfd, 0x5bfe); A = B;
  CYC(0x5bfe, 0x5c01); SET_HL(w4SubscreenTextIndices + 0x0f);
  CYC(0x5c01, 0x5c02); push_effect(gb, 0x5c02); add_a_to_hl(gb);
  CYC(0x5c02, 0x5c03); A = C;
  CYC(0x5c03, 0x5c05); alu_or(gb, 0xc0);
  CYC(0x5c05, 0x5c06); mem_wr(gb, HL, A);
  CYC(0x5c06, 0x5c07); A = C;
  CALL_C(0x5c07, getRingTiles_hook, 0x72fe, 0x5c0a);
  CYC(0x5c0a, 0x5c0b); SET_BC(pop_effect(gb));
  inventorySubscreen1_drawTreasures__nextRing_hook(gb);
}

void inventorySubscreen1_drawTreasures__nextRing_hook(GB *gb) {
  CYC(0x5c0b, 0x5c0c); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5c0c, 0x5c0e);
    inventorySubscreen1_drawTreasures__drawRing_hook(gb);
    return;
  }
  CYC(0x5c0c, 0x5c0e);
  CYC(0x5c0e, 0x5c11); A = W8(wRingBoxLevel);
  CYC(0x5c11, 0x5c13); alu_add(gb, 0x1c);
  CYC(0x5c13, 0x5c16); mem_wr(gb, w4SubscreenTextIndices + 0x0f, A);
  CYC(0x5c16, 0x5c19); SET_DE(w4TileMap + 0x182);
  CYC(0x5c19, 0x5c1b); A = 0xfe;
  CYC(0x5c1b, 0x5c1e); getRingTiles_hook(gb);
}

void inventorySubscreen1_drawTreasures__getAddressToDrawTreasureAt_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c1e, 0x5c1f); D = A;
  CYC(0x5c1f, 0x5c21); alu_and(gb, 0xf0);
  CYC(0x5c21, 0x5c23); A = alu_swap(gb, A);
  CYC(0x5c23, 0x5c24); alu_add(gb, A);
  CALL_C(0x5c24, multiplyABy16_hook, 0x01ac, 0x5c27);
  CYC(0x5c27, 0x5c28); A = D;
  CYC(0x5c28, 0x5c2a); alu_and(gb, 0x0f);
  CYC(0x5c2a, 0x5c2b); alu_add(gb, C);
  CYC(0x5c2b, 0x5c2e); SET_DE(w4TileMap + 0x62);
  CALL_C(0x5c2e, addAToDe_hook, 0x0068, 0x5c31);
  CYC(0x5c31, 0x5c32); ret_effect(gb);
}

void inventorySubscreen2_drawTreasures_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c3d, 0x5c40); SET_HL(0x5cea);
  CYC(0x5c40, 0x5c43); SET_DE(w4SubscreenTextIndices);
  CYC(0x5c43, 0x5c45); B = 0x0b;
  CALL_C(0x5c45, copyMemory_hook, 0x0486, 0x5c48);
  CYC(0x5c48, 0x5c4a); B = 0x08;
  inventorySubscreen2_drawTreasures__drawEssence_hook(gb);
}

void inventorySubscreen2_drawTreasures__drawEssence_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c4a, 0x5c4b); A = B;
  CYC(0x5c4b, 0x5c4c); A = alu_dec8(gb, A);
  CYC(0x5c4c, 0x5c4f); SET_HL(wEssencesObtained);
  CALL_C(0x5c4f, checkFlag_hook, 0x0205, 0x5c52);
  if (!(F & FZ)) {
    CYCT(0x5c52, 0x5c54);
    inventorySubscreen2_drawTreasures__nextEssence_hook(gb);
    return;
  }
  CYC(0x5c52, 0x5c54);
  CYC(0x5c54, 0x5c55); push_effect(gb, BC);
  CYC(0x5c55, 0x5c56); A = B;
  CYC(0x5c56, 0x5c59); SET_HL(0x5cb2);
  CYC(0x5c59, 0x5c5a); add_double_index_to_hl(gb, 0x5c5a);
  CYC(0x5c5a, 0x5c5b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5c5b, 0x5c5c); H = mem_rd(gb, HL);
  CYC(0x5c5c, 0x5c5d); L = A;
  CYC(0x5c5d, 0x5c60); SET_BC(0x0202);
  CYC(0x5c60, 0x5c63); SET_DE(0x0007);
  CALL_C(0x5c63, fillRectangleInTilemap_hook, 0x5d08, 0x5c66);
  CYC(0x5c66, 0x5c67); SET_BC(pop_effect(gb));
  CYC(0x5c67, 0x5c68); A = B;
  CYC(0x5c68, 0x5c6b); SET_HL(w4SubscreenTextIndices - 1);
  CYC(0x5c6b, 0x5c6c); push_effect(gb, 0x5c6c); add_a_to_hl(gb);
  CYC(0x5c6c, 0x5c6e); mem_wr(gb, HL, 0);
  inventorySubscreen2_drawTreasures__nextEssence_hook(gb);
}

void inventorySubscreen2_drawTreasures__nextEssence_hook(GB *gb) {
  CYC(0x5c6e, 0x5c6f); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5c6f, 0x5c71);
    inventorySubscreen2_drawTreasures__drawEssence_hook(gb);
    return;
  }
  CYC(0x5c6f, 0x5c71);
  CYC(0x5c71, 0x5c74); A = W8(wNumHeartPieces);
  CYC(0x5c74, 0x5c75); C = A;
  CYC(0x5c75, 0x5c78); SET_HL(w4SubscreenTextIndices + 9);
  CYC(0x5c78, 0x5c79); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5c79, 0x5c7a); mem_wr(gb, HL, A);
  CYC(0x5c7a, 0x5c7b); A = C;
  CYC(0x5c7b, 0x5c7c); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5c7c, 0x5c7e);
    inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece_hook(gb);
    return;
  }
  CYC(0x5c7c, 0x5c7e);
  CYC(0x5c7e, 0x5c80); alu_add(gb, 0x10);
  CYC(0x5c80, 0x5c83); mem_wr(gb, w4TileMap + 0x14f, A);
  CYC(0x5c83, 0x5c86); SET_HL(0x5cd8);
  inventorySubscreen2_drawTreasures__nextQuarterHeart_hook(gb);
}

void inventorySubscreen2_drawTreasures__nextQuarterHeart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  do {
    CYC(0x5c86, 0x5c87); push_effect(gb, BC);
    CYC(0x5c87, 0x5c88); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5c88, 0x5c8b); SET_DE(w4TileMap + 0xce);
    CALL_C(0x5c8b, addAToDe_hook, 0x0068, 0x5c8e);
    CALL_C(0x5c8e, drawTreasureDisplayDataToBg_hook, 0x5d1c, 0x5c91);
    CYC(0x5c91, 0x5c92); SET_BC(pop_effect(gb));
    CYC(0x5c92, 0x5c93); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(0x5c93, 0x5c95); else CYC(0x5c93, 0x5c95);
  } while (!(F & FZ));
  inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece_hook(gb);
}

void inventorySubscreen2_drawTreasures__doneUpdatingHeartPiece_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c95, 0x5c98); A = W8(wTilesetFlags);
  CYC(0x5c98, 0x5c9a); alu_and(gb, 0x80);
  CYC(0x5c9a, 0x5c9b); alu_rlca(gb);
  CYC(0x5c9b, 0x5c9c); C = A;
  CYC(0x5c9c, 0x5c9f); SET_HL(w4SubscreenTextIndices + 8);
  CYC(0x5c9f, 0x5ca0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5ca0, 0x5ca1); mem_wr(gb, HL, A);
  CYC(0x5ca1, 0x5ca2); A = C;
  CYC(0x5ca2, 0x5ca3); alu_add(gb, A);
  CYC(0x5ca3, 0x5ca4); alu_add(gb, A);
  CYC(0x5ca4, 0x5ca5); alu_add(gb, C);
  CYC(0x5ca5, 0x5ca8); SET_HL(0x5cc4);
  CYC(0x5ca8, 0x5ca9); add_double_index_to_hl(gb, 0x5ca9);
  CYC(0x5ca9, 0x5cac); SET_DE(w4TileMap + 0x6e);
  CALL_C(0x5cac, drawTreasureDisplayDataToBg_hook, 0x5d1c, 0x5caf);
  CYC(0x5caf, 0x5cb1); E = 0x70;
  CYC(0x5cb1, 0x5cb4); drawTreasureDisplayDataToBg_hook(gb);
}

void getRingBoxCapacity_hook(GB *gb) {
  CYC(0x5cf5, 0x5cf6); push_effect(gb, HL);
  CYC(0x5cf6, 0x5cf9); A = W8(wRingBoxLevel);
  CYC(0x5cf9, 0x5cfc); SET_HL(0x5d01);
  CYC(0x5cfc, 0x5cfd); push_effect(gb, 0x5cfd); add_a_to_hl(gb);
  CYC(0x5cfd, 0x5cfe); A = mem_rd(gb, HL);
  CYC(0x5cfe, 0x5cff); alu_or(gb, A);
  CYC(0x5cff, 0x5d00); SET_HL(pop_effect(gb));
  CYC(0x5d00, 0x5d01); ret_effect(gb);
}

void fillRectangleInTileMapWithMenuBlock_hook(GB *gb) {
  CYC(0x5d05, 0x5d08); SET_DE(0xe701);
  fillRectangleInTilemap_hook(gb);
}

void fillRectangleInTilemap_hook(GB *gb) {
  do {
    CYC(0x5d08, 0x5d09); push_effect(gb, HL);
    CYC(0x5d09, 0x5d0a); A = C;
    do {
      CYC(0x5d0a, 0x5d0b); mem_wr(gb, HL, D);
      CYC(0x5d0b, 0x5d0d); H |= 0x04;
      CYC(0x5d0d, 0x5d0e); mem_wr(gb, HL, E);
      CYC(0x5d0e, 0x5d10); H &= (uint8_t)~0x04;
      CYC(0x5d10, 0x5d11); SET_HL(HL + 1);
      CYC(0x5d11, 0x5d12); A = alu_dec8(gb, A);
      if (!(F & FZ)) CYCT(0x5d12, 0x5d14); else CYC(0x5d12, 0x5d14);
    } while (!(F & FZ));
    CYC(0x5d14, 0x5d15); SET_HL(pop_effect(gb));
    CYC(0x5d15, 0x5d17); A = 0x20;
    CYC(0x5d17, 0x5d18); push_effect(gb, 0x5d18); add_a_to_hl(gb);
    CYC(0x5d18, 0x5d19); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x5d19, 0x5d1b); else CYC(0x5d19, 0x5d1b);
  } while (!(F & FZ));
  CYC(0x5d1b, 0x5d1c); ret_effect(gb);
}

void drawTreasureDisplayDataToBg_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d1c, 0x5d1d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d1d, 0x5d1e); C = A;
  CYC(0x5d1e, 0x5d1f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d1f, 0x5d20); B = A;
  CALL_C(0x5d20, drawTreasureDisplayDataToBg__writeTile_hook, 0x5d39, 0x5d23);
  CYC(0x5d23, 0x5d24); E = alu_inc8(gb, E);
  CYC(0x5d24, 0x5d25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d25, 0x5d26); C = A;
  CYC(0x5d26, 0x5d27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d27, 0x5d28); B = A;
  CALL_C(0x5d28, drawTreasureDisplayDataToBg__writeTile_hook, 0x5d39, 0x5d2b);
  CYC(0x5d2b, 0x5d2d); A = 0x20;
  CALL_C(0x5d2d, addAToDe_hook, 0x0068, 0x5d30);
  CYC(0x5d30, 0x5d32); A = H8(hFF8B);
  CYC(0x5d32, 0x5d33); B = A;
  CYC(0x5d33, 0x5d35); C = 0x07;
  CYC(0x5d35, 0x5d36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d36, 0x5d39); drawTreasureExtraTiles_hook(gb);
}

void drawTreasureDisplayDataToBg__writeTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d39, 0x5d3a); push_effect(gb, DE);
  CYC(0x5d3a, 0x5d3b); A = C;
  CYC(0x5d3b, 0x5d3c); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5d3c, 0x5d3e);
    drawTreasureDisplayDataToBg__clearTile_hook(gb);
    return;
  }
  CYC(0x5d3c, 0x5d3e);
  CYC(0x5d3e, 0x5d3f); B = alu_inc8(gb, B);
  CYC(0x5d3f, 0x5d40); B = alu_inc8(gb, B);
  CYC(0x5d40, 0x5d41); alu_add(gb, A);
  if (!(F & FC)) CYCT(0x5d41, 0x5d43);
  else {
    CYC(0x5d41, 0x5d43);
    CYC(0x5d43, 0x5d45); B |= 0x08;
  }
  CYC(0x5d45, 0x5d46); C = A;
  CALL_C(0x5d46, drawTreasureDisplayDataToBg__writeTileHlpr_hook, 0x5d60, 0x5d49);
  CYC(0x5d49, 0x5d4a); SET_DE(pop_effect(gb));
  CYC(0x5d4a, 0x5d4b); ret_effect(gb);
}

void drawTreasureDisplayDataToBg__clearTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d4b, 0x5d4d); A = 0x02;
  CYC(0x5d4d, 0x5d4e); mem_wr(gb, DE, A);
  CYC(0x5d4e, 0x5d50); D |= 0x04;
  CYC(0x5d50, 0x5d51); A = alu_dec8(gb, A);
  CYC(0x5d51, 0x5d52); mem_wr(gb, DE, A);
  CYC(0x5d52, 0x5d54); A = 0x20;
  CALL_C(0x5d54, addAToDe_hook, 0x0068, 0x5d57);
  CYC(0x5d57, 0x5d59); A = 0x01;
  CYC(0x5d59, 0x5d5a); mem_wr(gb, DE, A);
  CYC(0x5d5a, 0x5d5c); D &= (uint8_t)~0x04;
  CYC(0x5d5c, 0x5d5d); A = alu_inc8(gb, A);
  CYC(0x5d5d, 0x5d5e); mem_wr(gb, DE, A);
  CYC(0x5d5e, 0x5d5f); SET_DE(pop_effect(gb));
  CYC(0x5d5f, 0x5d60); ret_effect(gb);
}

void drawTreasureDisplayDataToBg__writeTileHlpr_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d60, 0x5d61); A = C;
  CYC(0x5d61, 0x5d62); mem_wr(gb, DE, A);
  CYC(0x5d62, 0x5d64); D |= 0x04;
  CYC(0x5d64, 0x5d65); A = B;
  CYC(0x5d65, 0x5d66); mem_wr(gb, DE, A);
  CYC(0x5d66, 0x5d68); A = 0x20;
  CALL_C(0x5d68, addAToDe_hook, 0x0068, 0x5d6b);
  CYC(0x5d6b, 0x5d6c); A = B;
  CYC(0x5d6c, 0x5d6d); mem_wr(gb, DE, A);
  CYC(0x5d6d, 0x5d6f); D &= (uint8_t)~0x04;
  CYC(0x5d6f, 0x5d70); A = C;
  CYC(0x5d70, 0x5d71); A = alu_inc8(gb, A);
  CYC(0x5d71, 0x5d72); mem_wr(gb, DE, A);
  CYC(0x5d72, 0x5d73); ret_effect(gb);
}

void inventoryMenuDrawSprites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d73, inventoryMenuDrawHarpSprites_hook, 0x5dc0, 0x5d76);
  CYC(0x5d76, 0x5d78); A = 0x36;
  CALL_C(0x5d78, checkTreasureObtained_hook, 0x1748, 0x5d7b);
  if (!(F & FC)) { CYCT(0x5d7b, 0x5d7c); ret_effect(gb); return; }
  CYC(0x5d7b, 0x5d7c);
  CYC(0x5d7c, 0x5d7f); SET_BC(0x2068);
  CYC(0x5d7f, 0x5d82); A = W8(wMenuActiveState);
  CYC(0x5d82, 0x5d84); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x5d84, 0x5d86);
    inventoryMenuDrawSprites__menuScrolling_hook(gb);
    return;
  }
  CYC(0x5d84, 0x5d86);
  inventoryMenuDrawSprites__drawIfOnSubscreen1_hook(gb);
}

void inventoryMenuDrawSprites__drawIfOnSubscreen1_hook(GB *gb) {
  CYC(0x5d86, 0x5d89); A = W8(wInventorySubmenu);
  CYC(0x5d89, 0x5d8a); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5d8a, 0x5d8b); ret_effect(gb); return; }
  CYC(0x5d8a, 0x5d8b);
  CYC(0x5d8b, 0x5d8d);
  inventoryMenuDrawSprites__drawSprite_hook(gb);
}

void inventoryMenuDrawSprites__menuScrolling_hook(GB *gb) {
  CYC(0x5d8d, 0x5d90); A = W8(wSubmenuState);
  CYC(0x5d90, 0x5d91); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5d91, 0x5d93);
    inventoryMenuDrawSprites__drawIfOnSubscreen1_hook(gb);
    return;
  }
  CYC(0x5d91, 0x5d93);
  CYC(0x5d93, 0x5d96); A = W8(wInventorySubmenu);
  CYC(0x5d96, 0x5d97); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5d97, 0x5d98); ret_effect(gb); return; }
  CYC(0x5d97, 0x5d98);
  CYC(0x5d98, 0x5d99); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5d99, 0x5d9b);
    CYC(0x5da2, 0x5da5); A = W8(wGfxRegs2_SCX);
    CYC(0x5da5, 0x5da6); alu_cpl(gb);
    CYC(0x5da6, 0x5da7); A = alu_inc8(gb, A);
  } else {
    CYC(0x5d99, 0x5d9b);
    CYC(0x5d9b, 0x5d9e); A = W8(wGfxRegs2_WINX);
    CYC(0x5d9e, 0x5da0); alu_sub(gb, 0x07);
    CYC(0x5da0, 0x5da2);
  }
  inventoryMenuDrawSprites__drawSpriteWithXOffset_hook(gb);
}

void inventoryMenuDrawSprites__drawSpriteWithXOffset_hook(GB *gb) {
  CYC(0x5da7, 0x5da8); alu_add(gb, C);
  CYC(0x5da8, 0x5da9); C = A;
  inventoryMenuDrawSprites__drawSprite_hook(gb);
}

void inventoryMenuDrawSprites__drawSprite_hook(GB *gb) {
  CYC(0x5da9, 0x5dac); SET_HL(0x5daf);
  CYC(0x5dac, 0x5daf); addSpritesToOam_withOffset_hook(gb);
}

void inventoryMenuDrawHarpSprites_hook(GB *gb) {
  CYC(0x5dc0, 0x5dc3); SET_HL(wInventoryStorage);
  CYC(0x5dc3, 0x5dc6); SET_BC(0x1000);
  for (;;) {
    CYC(0x5dc6, 0x5dc7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5dc7, 0x5dc9); alu_cp(gb, 0x11);
    if (F & FZ) { CYCT(0x5dc9, 0x5dcb); break; }
    CYC(0x5dc9, 0x5dcb);
    CYC(0x5dcb, 0x5dcc); C = alu_inc8(gb, C);
    CYC(0x5dcc, 0x5dcd); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x5dcd, 0x5dcf);
    else {
      CYC(0x5dcd, 0x5dcf);
      CYC(0x5dcf, 0x5dd0); ret_effect(gb);
      return;
    }
  }
  CYC(0x5dd0, 0x5dd1); A = C;
  CYC(0x5dd1, 0x5dd3); alu_and(gb, 0xfc);
  CYC(0x5dd3, 0x5dd4); B = A;
  CYC(0x5dd4, 0x5dd5); alu_add(gb, A);
  CYC(0x5dd5, 0x5dd6); alu_add(gb, B);
  CYC(0x5dd6, 0x5dd7); alu_add(gb, A);
  CYC(0x5dd7, 0x5dd9); alu_add(gb, 0x14);
  CYC(0x5dd9, 0x5dda); B = A;
  CYC(0x5dda, 0x5ddb); A = C;
  CYC(0x5ddb, 0x5ddd); alu_and(gb, 0x03);
  CYC(0x5ddd, 0x5ddf); A = alu_swap(gb, A);
  CYC(0x5ddf, 0x5de0); alu_add(gb, A);
  CYC(0x5de0, 0x5de2); alu_add(gb, 0x22);
  CYC(0x5de2, 0x5de3); C = A;
  CYC(0x5de3, 0x5de6); A = W8(wMenuActiveState);
  CYC(0x5de6, 0x5de8); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x5de8, 0x5dea);
    goto scrolling;
  }
  CYC(0x5de8, 0x5dea);
subscreen_zero:
  CYC(0x5dea, 0x5ded); A = W8(wInventorySubmenu);
  CYC(0x5ded, 0x5dee); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5dee, 0x5def); ret_effect(gb); return; }
  CYC(0x5dee, 0x5def);
  CYC(0x5def, 0x5df1);
  inventoryMenuDrawHarpSprites__drawSprite_hook(gb);
  return;
scrolling:
  CYC(0x5df1, 0x5df4); A = W8(wSubmenuState);
  CYC(0x5df4, 0x5df5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5df5, 0x5df7); goto subscreen_zero; }
  CYC(0x5df5, 0x5df7);
  CYC(0x5df7, 0x5dfa); A = W8(wInventorySubmenu);
  CYC(0x5dfa, 0x5dfc); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x5dfc, 0x5dfd); ret_effect(gb); return; }
  CYC(0x5dfc, 0x5dfd);
  CYC(0x5dfd, 0x5dfe); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5dfe, 0x5e00);
    CYC(0x5e07, 0x5e0a); A = W8(wGfxRegs2_SCX);
    CYC(0x5e0a, 0x5e0b); alu_cpl(gb);
    CYC(0x5e0b, 0x5e0c); A = alu_inc8(gb, A);
  } else {
    CYC(0x5dfe, 0x5e00);
    CYC(0x5e00, 0x5e03); A = W8(wGfxRegs2_WINX);
    CYC(0x5e03, 0x5e05); alu_sub(gb, 0x07);
    CYC(0x5e05, 0x5e07);
  }
  inventoryMenuDrawHarpSprites__drawSpriteWithXOffset_hook(gb);
}

void inventoryMenuDrawHarpSprites__drawSpriteWithXOffset_hook(GB *gb) {
  CYC(0x5e0c, 0x5e0d); alu_add(gb, C);
  CYC(0x5e0d, 0x5e0e); C = A;
  inventoryMenuDrawHarpSprites__drawSprite_hook(gb);
}

void inventoryMenuDrawHarpSprites__drawSprite_hook(GB *gb) {
  CYC(0x5e0e, 0x5e11); A = W8(wSelectedHarpSong);
  CYC(0x5e11, 0x5e14); SET_HL(0x5aad);
  CYC(0x5e14, 0x5e15); push_effect(gb, 0x5e15); add_a_to_hl(gb);
  CYC(0x5e15, 0x5e16); A = mem_rd(gb, HL);
  CYC(0x5e16, 0x5e17); push_effect(gb, 0x5e17); add_a_to_hl(gb);
  CYC(0x5e17, 0x5e1a); addSpritesToOam_withOffset_hook(gb);
}

void func_02_4149_hook(GB *gb) {
  CYC(0x4149, 0x414c); SET_HL(wFileSelect_cursorPos);
  CYC(0x414c, 0x414d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x414d, 0x414f); A = 0x80;
  CYC(0x414f, 0x4150); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4150, 0x4151); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4151, 0x4152); ret_effect(gb);
}

void setFileSelectCursorOffsetToFileSelectMode_hook(GB *gb) {
  CYC(0x4152, 0x4155); A = W8(wFileSelect_mode);
  CYC(0x4155, 0x4158); W8(wFileSelect_cursorOffset) = A;
  CYC(0x4158, 0x4159); ret_effect(gb);
}

void setFileSelectMode_hook(GB *gb) {
  CYC(0x415b, 0x415e); SET_HL(wFileSelect_mode);
  CYC(0x415e, 0x415f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x415f, 0x4160); alu_xor(gb, A);
  CYC(0x4160, 0x4161); mem_wr(gb, HL, A);
  CYC(0x4161, 0x4164); W8(wFileSelect_textInputMode) = A;
  CYC(0x4164, 0x4165); ret_effect(gb);
}

void setFileSelectModeTo1_hook(GB *gb) {
  CYC(0x4159, 0x415b); A = 0x01;
  setFileSelectMode_hook(gb);
}

void loadGfxRegisterState5AndIncFileSelectMode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4165, 0x4167); A = 0x05;
  CALL_C(0x4167, loadGfxRegisterStateIndex_hook, 0x02ea, 0x416a);
  incFileSelectMode2_hook(gb);
}

void incFileSelectMode2_hook(GB *gb) {
  CYC(0x416a, 0x416d); SET_HL(wFileSelect_mode2);
  CYC(0x416d, 0x416e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x416e, 0x416f); ret_effect(gb);
}

void decFileSelectMode2_hook(GB *gb) {
  CYC(0x4175, 0x4178); SET_HL(wFileSelect_mode2);
  CYC(0x4178, 0x4179); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4179, 0x417a); ret_effect(gb);
}

void decFileSelectMode2IfBPressed_hook(GB *gb) {
  CYC(0x416f, 0x4172); A = W8(wKeysJustPressed);
  CYC(0x4172, 0x4174); alu_and(gb, 0x02);
  if (F & FZ) { CYCT(0x4174, 0x4175); ret_effect(gb); return; }
  CYC(0x4174, 0x4175);
  decFileSelectMode2_hook(gb);
}

void createBlankSpritesForItemSubmenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e1a, 0x5e1d); SET_HL(wInventory_cbc1);
  CYC(0x5e1d, 0x5e1e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5e1e, 0x5e20); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x5e20, 0x5e21); ret_effect(gb); return; }
  CYC(0x5e20, 0x5e21);
  CYC(0x5e21, 0x5e24); SET_BC(0x2800);
  CYC(0x5e24, 0x5e25); A = mem_rd(gb, HL);
  CYC(0x5e25, 0x5e27); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(0x5e27, 0x5e29); goto select_sprites; }
  CYC(0x5e27, 0x5e29);
  CYC(0x5e29, 0x5e2b); B = 0x50;
select_sprites:
  CYC(0x5e2b, 0x5e2d); E = 0x03;
  CYC(0x5e2d, 0x5e30); A = W8(wInventory_cbb8);
  CYC(0x5e30, 0x5e32); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(0x5e32, 0x5e34); goto start_loop; }
  CYC(0x5e32, 0x5e34);
  CYC(0x5e34, 0x5e35); E = alu_dec8(gb, E);
  CYC(0x5e35, 0x5e37); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x5e37, 0x5e39); goto start_loop; }
  CYC(0x5e37, 0x5e39);
  CYC(0x5e39, 0x5e3a); E = alu_dec8(gb, E);
start_loop:
  CYC(0x5e3a, 0x5e3b); A = E;
  for (;;) {
    CYC(0x5e3b, 0x5e3c); A = alu_dec8(gb, A);
    CYC(0x5e3c, 0x5e3d); push_effect(gb, AF);
    CYC(0x5e3d, 0x5e3e); push_effect(gb, BC);
    CYC(0x5e3e, 0x5e41); SET_HL(0x5e4c);
    CYC(0x5e41, 0x5e42); push_effect(gb, 0x5e42); add_a_to_hl(gb);
    CYC(0x5e42, 0x5e43); A = mem_rd(gb, HL);
    CYC(0x5e43, 0x5e44); push_effect(gb, 0x5e44); add_a_to_hl(gb);
    CALL_C(0x5e44, addSpritesToOam_withOffset_hook, 0x0d61, 0x5e47);
    CYC(0x5e47, 0x5e48); SET_BC(pop_effect(gb));
    CYC(0x5e48, 0x5e49); SET_AF(pop_effect(gb));
    if (!(F & FZ)) { CYCT(0x5e49, 0x5e4b); continue; }
    CYC(0x5e49, 0x5e4b);
    CYC(0x5e4b, 0x5e4c); ret_effect(gb);
    return;
  }
}

static void dungeon_map_generate_scrollable_tilemap(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x67de) {
    CYC(0x67de, 0x67e1); SET_DE(w4GfxBuf1);
    CYC(0x67e1, 0x67e3); A = 0x28;
    CALL_C(0x67e3, dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook, 0x6821, 0x67e6);
    CYC(0x67e6, 0x67e9); A = W8(wDungeonNumFloors);
    CYC(0x67e9, 0x67eb); H8(hFF8D) = A;
  } else if (entry == 0x67fc) {
    goto next_tile;
  } else if (entry == 0x6815) {
    goto done_this_floor;
  } else if (entry == 0x6821) {
    goto fill_tilemap_with_blank;
  }
next_floor:
  CYC(0x67eb, 0x67ed); A = H8(hFF8D);
  CYC(0x67ed, 0x67ee); A = alu_dec8(gb, A);
  CALL_C(0x67ee, dungeonMap_getFloorAddress_hook, 0x68ef, 0x67f1);
  CALL_C(0x67f1, dungeonMap_checkCanViewFloor_hook, 0x68bb, 0x67f4);
  CYC(0x67f4, 0x67f6); A = 0x50;
  if (F & FZ) { CYCT(0x67f6, 0x67f8); goto done_this_floor; }
  CYC(0x67f6, 0x67f8);
  CYC(0x67f8, 0x67fa); A = 0x40;
  CYC(0x67fa, 0x67fc); H8(hFF8C) = A;
next_tile:
  CYC(0x67fc, 0x67fe); A = 0x02;
  CYC(0x67fe, 0x6800); mem_wr(gb, IO_SVBK, A);
  CYC(0x6800, 0x6801); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6801, 0x6802); C = A;
  CYC(0x6802, 0x6804); A = 0x04;
  CYC(0x6804, 0x6806); mem_wr(gb, IO_SVBK, A);
  CYC(0x6806, 0x6807); A = C;
  CALL_C(0x6807, dungeonMap_getTileForRoom_hook, 0x6875, 0x680a);
  CYC(0x680a, 0x680b); mem_wr(gb, DE, A);
  CYC(0x680b, 0x680c); SET_DE(DE + 1);
  CYC(0x680c, 0x680e); A = H8(hFF8C);
  CYC(0x680e, 0x680f); A = alu_dec8(gb, A);
  CYC(0x680f, 0x6811); H8(hFF8C) = A;
  if (!(F & FZ)) { CYCT(0x6811, 0x6813); goto next_tile; }
  CYC(0x6811, 0x6813);
  CYC(0x6813, 0x6815); A = 0x10;
done_this_floor:
  CALL_C(0x6815, dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook, 0x6821, 0x6818);
  CYC(0x6818, 0x681a); A = H8(hFF8D);
  CYC(0x681a, 0x681b); A = alu_dec8(gb, A);
  CYC(0x681b, 0x681d); H8(hFF8D) = A;
  if (!(F & FZ)) { CYCT(0x681d, 0x681f); goto next_floor; }
  CYC(0x681d, 0x681f);
  CYC(0x681f, 0x6821); A = 0x18;
fill_tilemap_with_blank:
  CYC(0x6821, 0x6822); push_effect(gb, BC);
  CYC(0x6822, 0x6823); C = A;
  CYC(0x6823, 0x6825); A = 0xad;
  for (;;) {
    CYC(0x6825, 0x6826); mem_wr(gb, DE, A);
    CYC(0x6826, 0x6827); SET_DE(DE + 1);
    CYC(0x6827, 0x6828); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x6828, 0x682a); continue; }
    CYC(0x6828, 0x682a);
    CYC(0x682a, 0x682b); SET_BC(pop_effect(gb));
    CYC(0x682b, 0x682c); ret_effect(gb);
    return;
  }
}

void dungeonMap_generateScrollableTilemap_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, 0x67de, sp0_); }
void dungeonMap_generateScrollableTilemap__nextFloor67eb_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, 0x67eb, sp0_); }
void dungeonMap_generateScrollableTilemap__nextTile67fc_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, 0x67fc, sp0_); }
void dungeonMap_generateScrollableTilemap__doneThisFloor6815_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, 0x6815, sp0_); }
void dungeonMap_generateScrollableTilemap__fillTileMapWithBlank6821_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_generate_scrollable_tilemap(gb, 0x6821, sp0_); }

static void dungeon_map_check_compass_tile(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x68ce) {
    CALL_C(0x68ce, checkLinkHasCompass_hook, 0x6532, 0x68d1);
    if (F & FZ) { CYCT(0x68d1, 0x68d2); ret_effect(gb); return; }
    CYC(0x68d1, 0x68d2);
    CYC(0x68d2, 0x68d3); A = E;
    CYC(0x68d3, 0x68d5); C = 0x83;
    CYC(0x68d5, 0x68d7); alu_and(gb, 0x70);
    CYC(0x68d7, 0x68d9); alu_cp(gb, 0x40);
    if (F & FZ) { CYCT(0x68d9, 0x68db); goto ret; }
    CYC(0x68d9, 0x68db);
    CYC(0x68db, 0x68dd); alu_cp(gb, 0x20);
    if (F & FZ) { CYCT(0x68dd, 0x68df); goto treasure; }
    CYC(0x68dd, 0x68df);
    CYC(0x68df, 0x68e1); alu_cp(gb, 0x30);
    if (!(F & FZ)) { CYCT(0x68e1, 0x68e3); goto nothing; }
    CYC(0x68e1, 0x68e3);
  } else if (entry == 0x68ea) {
    goto nothing;
  } else if (entry == 0x68ec) {
    goto ret;
  }
treasure:
  CYC(0x68e3, 0x68e5); C = 0xae;
  CYC(0x68e5, 0x68e6); A = D;
  CYC(0x68e6, 0x68e8); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(0x68e8, 0x68ea); goto ret; }
  CYC(0x68e8, 0x68ea);
nothing:
  CYC(0x68ea, 0x68ec); C = 0;
ret:
  CYC(0x68ec, 0x68ed); A = C;
  CYC(0x68ed, 0x68ee); alu_or(gb, A);
  CYC(0x68ee, 0x68ef); ret_effect(gb);
}

void dungeonMap_checkCompassTile_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, 0x68ce, sp0_); }
void dungeonMap_checkCompassTile__treasure68e3_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, 0x68e3, sp0_); }
void dungeonMap_checkCompassTile__nothing68ea_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, 0x68ea, sp0_); }
void dungeonMap_checkCompassTile__ret68ec_hook(GB *gb) { uint16_t sp0_ = gb->sp; dungeon_map_check_compass_tile(gb, 0x68ec, sp0_); }

void dungeonMap_getFloorAddress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x68ef, multiplyABy16_hook, 0x01ac, 0x68f2);
  CYC(0x68f2, 0x68f5); SET_HL(w2DungeonLayout);
  CYC(0x68f5, 0x68f6); alu_add_hl(gb, BC);
  CYC(0x68f6, 0x68f7); alu_add_hl(gb, BC);
  CYC(0x68f7, 0x68f8); alu_add_hl(gb, BC);
  CYC(0x68f8, 0x68f9); alu_add_hl(gb, BC);
  CYC(0x68f9, 0x68fa); ret_effect(gb);
}

void fileSelectMode6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4526, fileSelectMode6__updateMode6_hook, 0x452c, 0x4529);
  CYC(0x4529, 0x452c); drawSecretInputCursors_hook(gb);
}

void fileSelectMode6__updateMode6_hook(GB *gb) {
  CYC(0x452c, 0x452f); A = W8(wFileSelect_mode2);
  CYC(0x452f, 0x4530); push_effect(gb, 0x4530);
  switch (function_caller_jump_table(gb)) {
    case 0x453a: fileSelectMode6__mode0_hook(gb); return;
    case 0x4541: fileSelectMode6__mode2_hook(gb); return;
    case 0x4609: textInput_waitForInput_hook(gb); return;
    case 0x46d6:
    case 0x4159:
    default: hook_handoff(gb, HL); return;
  }
}

void fileSelectMode6__mode0_hook(GB *gb) {
  CYC(0x453a, 0x453b); alu_xor(gb, A);
  CYC(0x453b, 0x453e); W8(wSecretInputType) = A;
  CYC(0x453e, 0x4541); func_02_465c_hook(gb);
}

void fileSelectMode6__mode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4541, 0x4544); SET_HL(w4SecretBuffer);
  CYC(0x4544, 0x4547); SET_DE(wTmpcec0);
  CYC(0x4547, 0x4549); B = 0x20;
  CALL_C(0x4549, copyMemory_hook, 0x0486, 0x454c);
  CYC(0x454c, 0x454f); SET_BC(0x0100);
  CALL_C(0x454f, secretFunctionCaller_hook, 0x1a2e, 0x4552);
  if (!(F & FZ)) {
    CYCT(0x4552, 0x4555); fileSelect_printError_hook(gb);
    return;
  }
  CYC(0x4552, 0x4555);
  CYC(0x4555, 0x4558); A = mem_rd(gb, wTmpcec0 + 0x12);
  CYC(0x4558, 0x4559); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4559, 0x455b);
  } else {
    CYC(0x4559, 0x455b);
    CYC(0x455b, 0x455e); A = mem_rd(gb, wTmpcec0 + 5);
    CYC(0x455e, 0x455f); A = alu_dec8(gb, A);
    if (F & FZ) {
      CYCT(0x455f, 0x4562); fileSelect_printError_hook(gb);
      return;
    }
    CYC(0x455f, 0x4562);
  }
  CALL_C(0x4562, loadFile_b00_hook, 0x09dc, 0x4565);
  CYC(0x4565, 0x4568); SET_BC(0x0400);
  CALL_C(0x4568, secretFunctionCaller_hook, 0x1a2e, 0x456b);
  CALL_C(0x456b, initializeFile_b00_hook, 0x09d4, 0x456e);
  CYC(0x456e, 0x4571); hook_handoff(gb, 0x4159);
}

void b2_updateMenus_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4fcf, 0x4fd2); A = W8(wOpenedMenuType);
  CYC(0x4fd2, 0x4fd3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4fd3, 0x4fd5); b2_updateMenus__updateMenu_hook(gb); return; }
  CYC(0x4fd3, 0x4fd5);
  CYC(0x4fd5, 0x4fd8); A = W8(wScrollMode);
  CYC(0x4fd8, 0x4fda); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x4fda, 0x4fdb); ret_effect(gb); return; }
  CYC(0x4fda, 0x4fdb);
  if (W8(wTextIsActive)) {
    CALL_C(0x4fdb, retIfTextIsActive_hook, 0x1859, 0x4fde);
    return;
  }
  CALL_C(0x4fdb, retIfTextIsActive_hook, 0x1859, 0x4fde);
  CYC(0x4fde, 0x4fe1); A = W8(wLinkDeathTrigger);
  CYC(0x4fe1, 0x4fe2); B = A;
  CYC(0x4fe2, 0x4fe5); A = W8(wLinkPlayingInstrument);
  CYC(0x4fe5, 0x4fe6); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x4fe6, 0x4fe7); ret_effect(gb); return; }
  CYC(0x4fe6, 0x4fe7);
  CYC(0x4fe7, 0x4fea); A = W8(wKeysJustPressed);
  CYC(0x4fea, 0x4fec); alu_and(gb, 0x0c);
  if (F & FZ) { CYCT(0x4fec, 0x4fee); goto no_button; }
  CYC(0x4fec, 0x4fee);
  CYC(0x4fee, 0x4ff1); A = mem_rd(gb, wGlobalFlags + 1);
  CYC(0x4ff1, 0x4ff3); alu_bit(gb, 2, A);
  CYC(0x4ff3, 0x4ff5); A = 0x5a;
  if (F & FZ) { CYCT(0x4ff5, 0x4ff8); playSound_b00_hook(gb); return; }
  CYC(0x4ff5, 0x4ff8);
no_button:
  CYC(0x4ff8, 0x4ffb); A = W8(wMenuDisabled);
  CYC(0x4ffb, 0x4ffc); B = A;
  CYC(0x4ffc, 0x4fff); A = W8(wDisableLinkCollisionsAndMenu);
  CYC(0x4fff, 0x5000); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x5000, 0x5001); ret_effect(gb); return; }
  CYC(0x5000, 0x5001);
  CALL_C(0x5001, playHeartBeepAtInterval_hook, 0x5142, 0x5004);
  CYC(0x5004, 0x5007); A = W8(wKeysJustPressed);
  CYC(0x5007, 0x5009); alu_and(gb, 0x0c);
  if (F & FZ) { CYCT(0x5009, 0x500a); ret_effect(gb); return; }
  CYC(0x5009, 0x500a);
  CYC(0x500a, 0x500c); C = 3;
  CYC(0x500c, 0x500e); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(0x500e, 0x5010); goto open_menu; }
  CYC(0x500e, 0x5010);
  CYC(0x5010, 0x5011); C = alu_dec8(gb, C);
  CYC(0x5011, 0x5013); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x5013, 0x5015); goto open_menu; }
  CYC(0x5013, 0x5015);
  CYC(0x5015, 0x5016); C = alu_dec8(gb, C);
open_menu:
  CYC(0x5016, 0x5019); openMenu_body_hook(gb);
}

void b2_updateMenus__updateMenu_hook(GB *gb) {
  CYC(0x5019, 0x501b); A = 0xff;
  CYC(0x501b, 0x501e); mem_wr(gb, wc4b6, A);
  CYC(0x501e, 0x5021); A = W8(wMenuLoadState);
  CYC(0x5021, 0x5022); push_effect(gb, 0x5022);
  switch (function_caller_jump_table(gb)) {
    case 0x5044: menuStateFadeIntoMenu_hook(gb); return;
    case 0x502a: menuSpecificCode_hook(gb); return;
    case 0x50c2: hook_handoff(gb, HL); return;
    case 0x5131: menuStateFadeIntoGame_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void menuSpecificCode_hook(GB *gb) {
  CYC(0x502a, 0x502d); A = W8(wOpenedMenuType);
  CYC(0x502d, 0x502e); push_effect(gb, 0x502e);
  switch (function_caller_jump_table(gb)) {
    case 0x7380:
    case 0x7474: hook_handoff(gb, HL); return;
    case 0x5515: runInventoryMenu_hook(gb); return;
    case 0x6009: runMapMenu_hook(gb); return;
    case 0x6d36: runRingMenu_hook(gb); return;
    case 0x5f34: runGaleSeedMenu_hook(gb); return;
    case 0x4571: runSecretEntryMenu_hook(gb); return;
    case 0x44e0: runKidNameEntryMenu_hook(gb); return;
    case 0x4b22: runGameLinkMenu_hook(gb); return;
    case 0x7641: runFakeReset_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void menuStateFadeIntoMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5044, 0x5047); A = W8(wOpenedMenuType);
  CYC(0x5047, 0x5049); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(0x5049, 0x504b); goto fade_done; }
  CYC(0x5049, 0x504b);
  CYC(0x504b, 0x504e); A = W8(wKeysPressed);
  CYC(0x504e, 0x5050); alu_and(gb, 0x0c);
  CYC(0x5050, 0x5052); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(0x5052, 0x5054); goto fade_done; }
  CYC(0x5052, 0x5054);
  CYC(0x5054, 0x5056); A = 3;
  CYC(0x5056, 0x5059); W8(wOpenedMenuType) = A;
fade_done:
  CYC(0x5059, 0x505c); A = W8(wPaletteThread_mode);
  CYC(0x505c, 0x505d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x505d, 0x505e); ret_effect(gb); return; }
  CYC(0x505d, 0x505e);
  CALL_C(0x505e, menuStateFadeIntoMenu__openMenu_hook, 0x5068, 0x5061);
  CYC(0x5061, 0x5064); SET_HL(wMenuLoadState);
  CYC(0x5064, 0x5065); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5065, 0x5068); menuSpecificCode_hook(gb);
}

void menuStateFadeIntoMenu__openMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5068, 0x506b); A = W8(wOpenedMenuType);
  CYC(0x506b, 0x506d); alu_cp(gb, 3);
  CYC(0x506d, 0x506f); A = 0x54;
  if (!(F & FZ)) CALL_C_CC(0x506f, playSound_b00_hook, 0x0c98, 0x5072);
  else CYC(0x506f, 0x5072);
  CYC(0x5072, 0x5074); A = 2;
  CALL_C(0x5074, setMusicVolume_hook, 0x0cad, 0x5077);
  saveGraphicsOnEnterMenu_body_hook(gb);
}

void saveGraphicsOnEnterMenu_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5077, 0x5079); A = H8(hCameraY);
  CYC(0x5079, 0x507c); SET_HL(wcbe1);
  CYC(0x507c, 0x507d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x507d, 0x507f); A = H8(hCameraX);
  CYC(0x507f, 0x5080); mem_wr(gb, HL, A);
  CYC(0x5080, 0x5081); push_effect(gb, DE);
  CYC(0x5081, 0x5084); SET_HL(wGfxRegs1);
  CYC(0x5084, 0x5087); SET_DE(wGfxRegs4);
  CYC(0x5087, 0x5089); B = 0x0c;
  CALL_C(0x5089, copyMemory_hook, 0x0486, 0x508c);
  CALL_C(0x508c, disableLcd_hook, 0x02c1, 0x508f);
  CALL_C(0x508f, copyW2TilesetBgPalettesToW4PaletteData_body_hook, 0x4f7c, 0x5092);
  CYC(0x5092, 0x5094); A = 4;
  CYC(0x5094, 0x5096); mem_wr(gb, IO_SVBK, A);
  CYC(0x5096, 0x5099); SET_HL(wOam);
  CYC(0x5099, 0x509c); SET_DE(w4SavedOam);
  CYC(0x509c, 0x509e); B = 0xa0;
  CALL_C(0x509e, copyMemory_hook, 0x0486, 0x50a1);
  CYC(0x50a1, 0x50a3); A = 1;
  CYC(0x50a3, 0x50a5); mem_wr(gb, IO_VBK, A);
  CYC(0x50a5, 0x50a8); SET_HL(0x8600);
  CYC(0x50a8, 0x50ab); SET_BC(0x0180);
  CYC(0x50ab, 0x50ae); SET_DE(w4SavedVramTiles);
  CALL_C(0x50ae, copyMemoryBc_hook, 0x0496, 0x50b1);
  CYC(0x50b1, 0x50b4); SET_HL(wMenuUnionStart);
  CYC(0x50b4, 0x50b6); B = 0x10;
  CALL_C(0x50b6, clearMemory_hook, 0x046f, 0x50b9);
  CYC(0x50b9, 0x50bb); A = 0xff;
  CYC(0x50bb, 0x50be); mem_wr(gb, wc4b6, A);
  CYC(0x50be, 0x50bf); SET_DE(pop_effect(gb));
  CYC(0x50bf, 0x50c2); clearOam_hook(gb);
}

void menuStateFadeIntoGame_hook(GB *gb) {
  CYC(0x5131, 0x5134); A = W8(wPaletteThread_mode);
  CYC(0x5134, 0x5135); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5135, 0x5136); ret_effect(gb); return; }
  CYC(0x5135, 0x5136);
  CYC(0x5136, 0x5137); alu_xor(gb, A);
  CYC(0x5137, 0x513a); mem_wr(gb, wc4b6, A);
  CYC(0x513a, 0x513d); W8(wOpenedMenuType) = A;
  CYC(0x513d, 0x513f); A = 3;
  CYC(0x513f, 0x5142); setMusicVolume_hook(gb);
}

void fileSelect_printError_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x45f0, 0x45f2); A = 0x5a;
  CALL_C(0x45f2, playSound_b00_hook, 0x0c98, 0x45f5);
  CYC(0x45f5, 0x45f7); A = 0x10;
  CYC(0x45f7, 0x45fa); W8(wFileSelect_linkTimer) = A;
  CYC(0x45fa, 0x45fc); A = 0x04;
  CYC(0x45fc, 0x45ff); W8(wFileSelect_mode2) = A;
  CYC(0x45ff, 0x4601); A = 0xad;
  CALL_C(0x4601, loadGfxHeader_hook, 0x0626, 0x4604);
  CYC(0x4604, 0x4606); A = 0x08;
  CYC(0x4606, 0x4609); loadUncompressedGfxHeader_hook(gb);
}

void textInput_waitForInput_hook(GB *gb) {
  CYC(0x4609, 0x460c); SET_HL(wFileSelect_linkTimer);
  CYC(0x460c, 0x460d); A = mem_rd(gb, HL);
  CYC(0x460d, 0x460e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x460e, 0x4610);
  } else {
    CYC(0x460e, 0x4610);
    CYC(0x4610, 0x4611); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x4611, 0x4612); ret_effect(gb);
    return;
  }
  CYC(0x4612, 0x4615); A = W8(wKeysPressed);
  CYC(0x4615, 0x4616); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4616, 0x4617); ret_effect(gb);
    return;
  }
  CYC(0x4616, 0x4617);
  CYC(0x4617, 0x4619); A = 0x01;
  CYC(0x4619, 0x461c); W8(wFileSelect_mode2) = A;
  func_02_461c_hook(gb);
}
