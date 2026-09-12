#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

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
void getNameBufferLength_hook(GB *gb);
void copyNameToW4NameBuffer_hook(GB *gb);
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
  CALL_C(0x6894, dungeonMap_checkCompassTile, 0x68ce, 0x6897);
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
  CALL_C(0x68ad, dungeonMap_checkCompassTile, 0x68ce, 0x68b0);
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
  CALL_C(0x5f6d, getDirectionButtonOffsetFromHl, 0x5883, 0x5f70);
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
  CALL_C(0x608c, dungeonMap_generateScrollableTilemap, 0x67de, 0x608f);
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
  CALL_C(0x614c, getDirectionButtonOffsetFromHl, 0x5883, 0x614f);
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
  CYC(0x465a, 0x465c); label_02_038(gb);
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
  CALL_C(0x5374, func_02_52f6, 0x52f6, 0x5377);
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
