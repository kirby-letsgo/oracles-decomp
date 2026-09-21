#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void cutscene_decCBB3IfTextNotActive_hook(GB *gb);
void cutscene_decCBB3IfNotFadingOut_hook(GB *gb);
void cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook(GB *gb);
void cutscene_resetOamWithData_hook(GB *gb);
void cutsceneFunc_6026_hook(GB *gb);
void cutscene_loadAintoHL_BTimes_hook(GB *gb);
void cutscene_load_24_ObjectGfx2Times_andReload_hook(GB *gb);
void cutscene_load_26_ObjectGfx2Times_andReload_hook(GB *gb);
void cutscene_load_04_ObjectGfx2Times_andReload_hook(GB *gb);
void cutscene_loadAObjectGfxBTimes_andReload_hook(GB *gb);
void endgameCutsceneHandler_0a__state1__func_5fc7_hook(GB *gb);
void endgameCutsceneHandler_0f__state0__func_5cb0_hook(GB *gb);
void endgameCutsceneHandler_0f__state0__func_5cb7_hook(GB *gb);
void endgameCutsceneHandler_0a__runStates_hook(GB *gb);
void endgameCutsceneHandler_0a__state0_hook(GB *gb);
void endgameCutsceneHandler_0a__state1_hook(GB *gb);
void endgameCutsceneHandler_0a__state1__substate0_hook(GB *gb);
void endgameCutsceneHandler_0a__state2_hook(GB *gb);
void endgameCutsceneHandler_0a__state3_hook(GB *gb);

void endgameCutsceneHandler_09_stage1__loadCertainOamData1_hook(GB *gb);
void endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(GB *gb);
void endgameCutsceneHandler_20__func_5920_hook(GB *gb);
void endgameCutsceneHandler_20__func_5943_hook(GB *gb);
void endgameCutsceneHandler_20__func_5953_hook(GB *gb);
void endgameCutsceneHandler_20__func_596d_hook(GB *gb);
void endgameCutsceneHandler_20__func_5995_hook(GB *gb);
void endgameCutsceneHandler_20__func_59b3_hook(GB *gb);
void endgameCutsceneHandler_20__func_5ac2_hook(GB *gb);
void endgameCutsceneHandler_20_hook(GB *gb);
void endgameCutsceneHandler_20__afterCall5857_hook(GB *gb);
void endgameCutsceneHandler_20__runStates_hook(GB *gb);
void endgameCutsceneHandler_20__state0_hook(GB *gb);
void endgameCutsceneHandler_20__state0__afterCall5895_hook(GB *gb);
void endgameCutsceneHandler_20__state9_hook(GB *gb);
void endgameCutsceneHandler_20__state9__afterCall59d4_hook(GB *gb);
void endgameCutsceneHandler_20__stateD_hook(GB *gb);
void endgameCutsceneHandler_20__stateD__afterCall5a64_hook(GB *gb);
void endgameCutsceneHandler_0f__state0_hook(GB *gb);
void endgameCutsceneHandler_0f__state0__runSubstates_hook(GB *gb);
void endgameCutsceneHandler_0f__state0__substate0_hook(GB *gb);
void endgameCutsceneHandler_0f__state1_hook(GB *gb);
void endgameCutsceneHandler_0f__state1__runSubstates_hook(GB *gb);
void endgameCutsceneHandler_0f__state1__substate0_hook(GB *gb);
void endgameCutsceneHandler_09_stage0_hook(GB *gb);
void endgameCutsceneHandler_09_stage0__runStates_hook(GB *gb);
void endgameCutsceneHandler_09_stage1__runStates_hook(GB *gb);

static void add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static void add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t endgame_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

void cutscene_incCBC2setCBB3whenCBB3is0_hook(GB *gb) {
  BASE(cutscene_incCBC2setCBB3whenCBB3is0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, incCbc2_hook, SYM(incCbc2), b_+7);
  CYC(b_+7, b_+10); SET_HL(wTmpcbb3);
  CYC(b_+10, b_+11); mem_wr(gb, HL, E);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void cutscene_decCBB3IfTextNotActive_hook(GB *gb) {
  BASE(cutscene_decCBB3IfTextNotActive);
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8);
  decCbb3_hook(gb);
}

void cutscene_decCBB3IfNotFadingOut_hook(GB *gb) {
  BASE(cutscene_decCBB3IfNotFadingOut);
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8);
  decCbb3_hook(gb);
}

void cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(GB *gb) {
  BASE(cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, incCbc1_hook, SYM(incCbc1), b_+7);
  CYC(b_+7, b_+8); A = E;
  CYC(b_+8, b_+11); W8(wTmpcbb3) = A;
  CYC(b_+11, b_+14);
  showText_hook(gb);
}

void cutscene_clearCFC0ToCFDF_hook(GB *gb) {
  BASE(cutscene_clearCFC0ToCFDF);
  CYC(b_+0, b_+2); B = 0x20;
  CYC(b_+2, b_+5); SET_HL(wRoomLayoutEnd);
  CYC(b_+5, b_+8);
  clearMemory_hook(gb);
}

void cutscene_setScreenShakeCounterTo4RumbleAt0_hook(GB *gb) {
  BASE(cutscene_setScreenShakeCounterTo4RumbleAt0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), SYM(cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0));
  cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook(gb);
}

void cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook(GB *gb) {
  BASE(cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0);
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); A = 0xb8;
  if (F & FZ) {
    CYCT(b_+7, b_+10);
    TAIL(playSound_b00);
  }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void cutscene_resetOamWithSomething1_hook(GB *gb) {
  BASE(cutscene_resetOamWithSomething1);
  CYC(b_+0, b_+3); SET_HL((SYM(introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen) + 3));
  CYC(b_+3, b_+5); E = 0x16;
  CYC(b_+5, b_+8); SET_BC(0x3038);
  CYC(b_+8, b_+10);
  cutscene_resetOamWithData_hook(gb);
}

void cutscene_resetOamWithSomething2_hook(GB *gb) {
  BASE(cutscene_resetOamWithSomething2);
  CYC(b_+0, b_+3); SET_HL((SYM(introCinematic_ridingHorse) + 13));
  CYC(b_+3, b_+5); E = 0x16;
  CYC(b_+5, b_+8); SET_BC(0x3038);
  cutscene_resetOamWithData_hook(gb);
}

void cutscene_resetOamWithData_hook(GB *gb) {
  BASE(cutscene_resetOamWithData);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hOamTail, A);
  CYC(b_+3, b_+6);
  addSpritesFromBankToOam_withOffset_hook(gb);
}

void cutscene_replaceListOfTiles_hook(GB *gb) {
  BASE(cutscene_replaceListOfTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = mem_rd(gb, HL);
  CYC(b_+1, b_+2); SET_HL(HL + 1);
  for (;;) {
    CYC(b_+2, b_+3); C = mem_rd(gb, HL);
    CYC(b_+3, b_+4); SET_HL(HL + 1);
    CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+5, b_+6); push_effect(gb, BC);
    CYC(b_+6, b_+7); push_effect(gb, HL);
    CALL_C(b_+7, setTile_hook, SYM(setTile), b_+10);
    CYC(b_+10, b_+11); SET_HL(pop_effect(gb));
    CYC(b_+11, b_+12); SET_BC(pop_effect(gb));
    CYC(b_+12, b_+13); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+13, b_+15);
      continue;
    }
    CYC(b_+13, b_+15);
    CYC(b_+15, b_+16); ret_effect(gb);
    return;
  }
}

void func_60e0_hook(GB *gb) {
  BASE(func_60e0);
  CYC(b_+0, b_+3); SET_HL(wLinkHealth);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x04);
  CYC(b_+5, b_+7); L = 0x88;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); B = mem_rd(gb, HL);
  CYC(b_+9, b_+12); SET_HL(wcde3);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);
  CYC(b_+14, b_+17);
  disableActiveRing_hook(gb);
}

void func_60f1_hook(GB *gb) {
  BASE(func_60f1);
  CYC(b_+0, b_+3); SET_HL(wLinkMaxHealth);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+8); SET_HL(wcde3);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  CYC(b_+10, b_+13); SET_HL(wInventoryB);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, B);
  CYC(b_+15, (SYM(func_03_6103) + 1));
  enableActiveRing_hook(gb);
}

void cutscene_parseObjectData_andLoadObjectGfx_hook(GB *gb) {
  BASE(cutscene_parseObjectData_andLoadObjectGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getEntryFromObjectTable1_hook, SYM(getEntryFromObjectTable1), b_+3);
  CALL_C(b_+3, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+6);
  CALL_C(b_+6, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+9);
  CYC(b_+9, b_+12); cutsceneFunc_6026_hook(gb);
}

void cutsceneFunc_6026_hook(GB *gb) {
  BASE(cutsceneFunc_6026);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+3, b_+5); alu_cp(gb, 0x00);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    TAIL(cutscene_load_04_ObjectGfx2Times_andReload);
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    TAIL(cutscene_load_26_ObjectGfx2Times_andReload);
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    TAIL(cutscene_load_24_ObjectGfx2Times_andReload);
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
    TAIL(cutscene_load_26_ObjectGfx2Times_andReload);
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void cutscene_loadAObjectGfxBTimes_hook(GB *gb) {
  BASE(cutscene_loadAObjectGfxBTimes);
  CYC(b_+0, b_+3); SET_HL(wLoadedObjectGfx);
  cutscene_loadAintoHL_BTimes_hook(gb);
}

void cutscene_loadAintoHL_BTimes_hook(GB *gb) {
  BASE(cutscene_loadAintoHL_BTimes);
  for (;;) {
    CYC(b_+0, b_+1); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+1, b_+2); A = alu_inc8(gb, A);
    CYC(b_+2, b_+4); mem_wr(gb, HL, 0x01);
    CYC(b_+4, b_+5); L = alu_inc8(gb, L);
    CYC(b_+5, b_+6); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+6, b_+8);
      continue;
    }
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); ret_effect(gb);
    return;
  }
}

void cutscene_load_24_ObjectGfx2Times_andReload_hook(GB *gb) {
  BASE(cutscene_load_24_ObjectGfx2Times_andReload);
  CYC(b_+0, b_+2); A = 0x24;
  CYC(b_+2, b_+4); B = 0x02;
  CYC(b_+4, b_+6); cutscene_loadAObjectGfxBTimes_andReload_hook(gb);
}

void cutscene_load_26_ObjectGfx2Times_andReload_hook(GB *gb) {
  BASE(cutscene_load_26_ObjectGfx2Times_andReload);
  CYC(b_+0, b_+2); A = 0x26;
  CYC(b_+2, b_+4); B = 0x02;
  CYC(b_+4, b_+6); cutscene_loadAObjectGfxBTimes_andReload_hook(gb);
}

void cutscene_load_04_ObjectGfx2Times_andReload_hook(GB *gb) {
  BASE(cutscene_load_04_ObjectGfx2Times_andReload);
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, b_+4); B = 0x02;
  cutscene_loadAObjectGfxBTimes_andReload_hook(gb);
}

void cutscene_loadAObjectGfxBTimes_andReload_hook(GB *gb) {
  BASE(cutscene_loadAObjectGfxBTimes_andReload);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+3);
  CYC(b_+3, b_+6); reloadObjectGfx_b00_hook(gb);
}

void cutscene_clearTmpCBB3_hook(GB *gb) {
  BASE(cutscene_clearTmpCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+5); B = 0x10;
  CYC(b_+5, b_+8); clearMemory_hook(gb);
}

void endgameCutsceneHandler_0a__state0__substate0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+30, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+33);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+34); ret_effect(gb);
    return;
  }
  CYC(b_+33, b_+34);
  CALL_C(b_+34, func_60e0_hook, SYM(func_60e0), b_+37);
  CALL_C(b_+37, incCbc2_hook, SYM(incCbc2), b_+40);
  CALL_C(b_+40, clearOam_hook, SYM(clearOam), b_+43);
  CYC(b_+43, b_+46); SET_HL(wTmpcbb3);
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0xb4);
  CYC(b_+48, b_+49); SET_HL(HL + 1);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x00);
  CYC(b_+51, b_+54); SET_HL(wGfxRegs1_LCDC);
  CYC(b_+54, b_+56); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(b_+56, b_+58); A = 0x2a;
  CYC(b_+58, b_+61); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_0a__state0__substate1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+61, b_+64); SET_HL(wTmpcbb3);
  CALL_C(b_+64, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+67);
  if (!(F & FZ)) {
    CYCT(b_+67, b_+68); ret_effect(gb);
    return;
  }
  CYC(b_+67, b_+68);
  CALL_C(b_+68, incCbc2_hook, SYM(incCbc2), b_+71);
  CYC(b_+71, b_+74); SET_HL(wTmpcbb3);
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x48);
  CYC(b_+76, b_+77); SET_HL(HL + 1);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x03);
  CYC(b_+79, b_+81); A = 0x04;
  CALL_C(b_+81, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+84);
  CYC(b_+84, b_+86); A = 0x06;
  CYC(b_+86, b_+89); fadeinFromBlackWithDelay_hook(gb);
}

void endgameCutsceneHandler_0a__state0__substate2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+89, b_+92); SET_HL(wTmpcbb3);
  CALL_C(b_+92, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+95);
  if (!(F & FZ)) {
    CYCT(b_+95, b_+96); ret_effect(gb);
    return;
  }
  CYC(b_+95, b_+96);
  CALL_C(b_+96, incCbc1_hook, SYM(incCbc1), b_+99);
  CYC(b_+99, b_+100); L = alu_inc8(gb, L);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  CYC(b_+101, b_+103); B = 0x00;
  CALL_C(b_+103, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+106);
  if (F & FZ) {
    CYCT(b_+106, b_+108);
  } else {
    CYC(b_+106, b_+108);
    CYC(b_+108, b_+110); B = 0x04;
  }
  CYC(b_+110, b_+113); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+113, b_+114); mem_wr(gb, HL, B);
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x00);
  CYC(b_+117, b_+120); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__substate1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+288, b_+291); A = W8(wPaletteThread_mode);
  CYC(b_+291, b_+292); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+292, b_+293); ret_effect(gb);
    return;
  }
  CYC(b_+292, b_+293);
  CYC(b_+293, b_+296); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+296, b_+297); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+297, b_+298); ret_effect(gb);
    return;
  }
  CYC(b_+297, b_+298);
  CALL_C(b_+298, incCbc2_hook, SYM(incCbc2), b_+301);
  CYC(b_+301, b_+303); A = 0xff;
  CYC(b_+303, b_+306); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+306, b_+309); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__substate2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+309, b_+312); A = W8(wPaletteThread_mode);
  CYC(b_+312, b_+313); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+313, b_+314); ret_effect(gb);
    return;
  }
  CYC(b_+313, b_+314);
  CALL_C(b_+314, incCbc2_hook, SYM(incCbc2), b_+317);
  CALL_C(b_+317, disableLcd_hook, SYM(disableLcd), b_+320);
  CALL_C(b_+320, clearWramBank1_hook, SYM(clearWramBank1), b_+323);
  CYC(b_+323, b_+326); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+326, b_+327); alu_add(gb, A);
  CYC(b_+327, b_+329); alu_add(gb, 0x86);
  CALL_C(b_+329, loadGfxHeader_hook, SYM(loadGfxHeader), b_+332);
  CYC(b_+332, b_+335); SET_HL(wTmpcbb3);
  CYC(b_+335, b_+337); mem_wr(gb, HL, 0x5a);
  CYC(b_+337, b_+339); A = 0xa1;
  CALL_C(b_+339, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+342);
  CYC(b_+342, b_+344); A = 0x04;
  CALL_C(b_+344, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+347);
  CYC(b_+347, b_+350); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+350, b_+353); SET_HL(b_+369);
  CYC(b_+353, b_+354); add_a_to_hl_from_rst(gb, b_+354);
  CYC(b_+354, b_+355); A = mem_rd(gb, HL);
  CYC(b_+355, b_+358); mem_wr(gb, wGfxRegs1_SCX, A);
  CYC(b_+358, b_+360); A = 0x10;
  CYC(b_+360, b_+362); mem_wr(gb, hCameraX, A);
  CYC(b_+362, b_+363); alu_xor(gb, A);
  CYC(b_+363, b_+366); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+366, b_+369); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__substate3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+377, b_+380); A = W8(wPaletteThread_mode);
  CYC(b_+380, b_+381); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+381, b_+382); ret_effect(gb);
    return;
  }
  CYC(b_+381, b_+382);
  CALL_C(b_+382, decCbb3_hook, SYM(decCbb3), b_+385);
  if (!(F & FZ)) {
    CYCT(b_+385, b_+386); ret_effect(gb);
    return;
  }
  CYC(b_+385, b_+386);
  CALL_C(b_+386, incCbc2_hook, SYM(incCbc2), b_+389);
  CALL_C(b_+389, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+392);
  if (!(F & FZ)) {
    CYCT(b_+392, b_+393); ret_effect(gb);
    return;
  }
  CYC(b_+392, b_+393);
  CYC(b_+393, b_+395); mem_wr(gb, HL, 0xae);
  CYC(b_+395, b_+396); L = alu_inc8(gb, L);
  CYC(b_+396, b_+399); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+399, b_+400); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+400, b_+402); mem_wr(gb, HL, 0x00);
  CYC(b_+402, b_+403); ret_effect(gb);
}

void endgameCutsceneHandler_0a__state1__substate4_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+403, b_+406); A = W8(wPaletteThread_mode);
  CYC(b_+406, b_+407); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+407, b_+408); ret_effect(gb);
    return;
  }
  CYC(b_+407, b_+408);
  CYC(b_+408, b_+409); alu_xor(gb, A);
  CYC(b_+409, b_+411); mem_wr(gb, hOamTail, A);
  CYC(b_+411, b_+414); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+414, b_+415); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+415, b_+416); ret_effect(gb);
    return;
  }
  CYC(b_+415, b_+416);
  CYC(b_+416, b_+418); B = 0x03;
  CALL_C(b_+418, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+421);
  if (F & FZ) {
    CYCT(b_+421, b_+423);
  } else {
    CYC(b_+421, b_+423);
    CYC(b_+423, b_+425); B = 0x07;
  }
  CYC(b_+425, b_+428); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+428, b_+429); A = mem_rd(gb, HL);
  CYC(b_+429, b_+430); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(b_+430, b_+432);
    TAIL(endgameCutsceneHandler_0a__state1__func_5fc7);
  }
  CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+433, b_+434); alu_xor(gb, A);
  CYC(b_+434, b_+437); mem_wr(gb, wGenericCutscene_cbc2, A);
  CYC(b_+437, b_+439);
  CYC(b_+450, b_+453); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__func_5fc7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+439, cutscene_clearTmpCBB3_hook, SYM(cutscene_clearTmpCBB3), b_+442);
  CALL_C(b_+442, cutscene_clearCFC0ToCFDF_hook, SYM(cutscene_clearCFC0ToCFDF), b_+445);
  CYC(b_+445, b_+447); A = 0x02;
  CYC(b_+447, b_+450); mem_wr(gb, wGenericCutscene_cbc1, A);
  CYC(b_+450, b_+453); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+92, decCbb3_hook, SYM(decCbb3), b_+95);
  if (!(F & FZ)) {
    CYCT(b_+95, b_+96); ret_effect(gb);
    return;
  }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+99); SET_HL(wTmpcbb3);
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x1e);
  CYC(b_+101, b_+103); A = 0xf0;
  CALL_C(b_+103, playSound_b00_hook, SYM(playSound_b00), b_+106);
  CYC(b_+106, b_+109); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+109, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+112);
  CALL_C(b_+112, decCbb3_hook, SYM(decCbb3), b_+115);
  if (!(F & FZ)) {
    CYCT(b_+115, b_+116); ret_effect(gb);
    return;
  }
  CYC(b_+115, b_+116);
  CALL_C(b_+116, incCbc2_hook, SYM(incCbc2), b_+119);
  CYC(b_+119, b_+122); SET_HL(wTmpcbb3);
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x96);
  CYC(b_+124, b_+127); endgameCutsceneHandler_0f__state0__func_5cb0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+127, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+130);
  CALL_C(b_+130, decCbb3_hook, SYM(decCbb3), b_+133);
  if (!(F & FZ)) {
    CYCT(b_+133, b_+134); ret_effect(gb);
    return;
  }
  CYC(b_+133, b_+134);
  CALL_C(b_+134, incCbc2_hook, SYM(incCbc2), b_+137);
  CYC(b_+137, b_+139); A = 0xf1;
  CALL_C(b_+139, playSound_b00_hook, SYM(playSound_b00), b_+142);
  CYC(b_+142, b_+145); SET_HL(wTmpcbb3);
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x3c);
  CYC(b_+147, b_+150); SET_BC(0x3d0e);
  CYC(b_+150, b_+153); showText_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate4_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+153, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+156);
  if (!(F & FZ)) {
    CYCT(b_+156, b_+157); ret_effect(gb);
    return;
  }
  CYC(b_+156, b_+157);
  CALL_C(b_+157, incCbc2_hook, SYM(incCbc2), b_+160);
  CYC(b_+160, b_+162); A = 0x21;
  CALL_C(b_+162, playSound_b00_hook, SYM(playSound_b00), b_+165);
  CYC(b_+165, b_+168); SET_HL(wTmpcbb3);
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x3c);
  CYC(b_+170, b_+173); endgameCutsceneHandler_0f__state0__func_5cb0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate5_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+173, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+176);
  CALL_C(b_+176, decCbb3_hook, SYM(decCbb3), b_+179);
  if (!(F & FZ)) {
    CYCT(b_+179, b_+180); ret_effect(gb);
    return;
  }
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+183); SET_HL(wTmpcbb3);
  CYC(b_+183, b_+185); mem_wr(gb, HL, 0x5a);
  CYC(b_+185, b_+188); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate6_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+188, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+191);
  CALL_C(b_+191, decCbb3_hook, SYM(decCbb3), b_+194);
  if (!(F & FZ)) {
    CYCT(b_+194, b_+195); ret_effect(gb);
    return;
  }
  CYC(b_+194, b_+195);
  CALL_C(b_+195, incCbc2_hook, SYM(incCbc2), b_+198);
  CYC(b_+198, b_+201); SET_HL(wTmpcbb3);
  CYC(b_+201, b_+203); mem_wr(gb, HL, 0x3c);
  CYC(b_+203, b_+205); A = 0xf1;
  CYC(b_+205, b_+208); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+208, decCbb3_hook, SYM(decCbb3), b_+211);
  if (!(F & FZ)) {
    CYCT(b_+211, b_+212); ret_effect(gb);
    return;
  }
  CYC(b_+211, b_+212);
  CALL_C(b_+212, incCbc2_hook, SYM(incCbc2), b_+215);
  CYC(b_+215, b_+218); SET_HL(wTmpcbb3);
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x3c);
  CYC(b_+220, b_+223); SET_BC(0x3d0f);
  CYC(b_+223, b_+226); showText_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate8_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+226, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+229);
  if (!(F & FZ)) {
    CYCT(b_+229, b_+230); ret_effect(gb);
    return;
  }
  CYC(b_+229, b_+230);
  CALL_C(b_+230, incCbc2_hook, SYM(incCbc2), b_+233);
  CYC(b_+233, b_+236); SET_HL(wTmpcbb3);
  CYC(b_+236, b_+238); mem_wr(gb, HL, 0x68);
  CYC(b_+238, b_+239); SET_HL(HL + 1);
  CYC(b_+239, b_+241); mem_wr(gb, HL, 0x01);
  CYC(b_+241, b_+244); endgameCutsceneHandler_0f__state0__func_5cb7_hook(gb);
}

void endgameCutsceneHandler_0f__state0__func_5cb0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+332, getFreePartSlot_hook, SYM(getFreePartSlot), b_+335);
  if (!(F & FZ)) {
    CYCT(b_+335, b_+336); ret_effect(gb);
    return;
  }
  CYC(b_+335, b_+336);
  CYC(b_+336, b_+338); mem_wr(gb, HL, 0x54);
  CYC(b_+338, b_+339); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state0__func_5cb7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+339, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+342);
  if (!(F & FZ)) {
    CYCT(b_+342, b_+343); ret_effect(gb);
    return;
  }
  CYC(b_+342, b_+343);
  CYC(b_+343, b_+345); mem_wr(gb, HL, 0x62);
  CYC(b_+345, b_+346); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state0__substate9_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+244, b_+247); SET_HL(wTmpcbb3);
  CALL_C(b_+247, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+250);
  if (!(F & FZ)) {
    CYCT(b_+250, b_+251); ret_effect(gb);
    return;
  }
  CYC(b_+250, b_+251);
  CALL_C(b_+251, incCbc2_hook, SYM(incCbc2), b_+254);
  CYC(b_+254, b_+257); SET_HL(wTmpcbb3);
  CYC(b_+257, b_+259); mem_wr(gb, HL, 0x3c);
  CYC(b_+259, b_+262); SET_BC(0x0563);
  CYC(b_+262, b_+265); showText_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateA_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  CYC(b_+265, b_+267); E = 0x1e;
  CYC(b_+267, b_+270); cutscene_incCBC2setCBB3whenCBB3is0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateB_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+270, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+273);
  CALL_C(b_+273, decCbb3_hook, SYM(decCbb3), b_+276);
  if (!(F & FZ)) {
    CYCT(b_+276, b_+277); ret_effect(gb);
    return;
  }
  CYC(b_+276, b_+277);
  CALL_C(b_+277, incCbc2_hook, SYM(incCbc2), b_+280);
  CALL_C(b_+280, endgameCutsceneHandler_0f__state0__func_5cb0_hook, b_+332, b_+283);
  CYC(b_+283, b_+285); A = 0x8c;
  CYC(b_+285, b_+288); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+288, b_+290); A = 0xff;
  CYC(b_+290, b_+293); SET_BC((SYM(oamDmaFunctionEnd) + 989));
  CYC(b_+293, b_+296); createEnergySwirlGoingOut_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateC_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+296, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+299);
  CALL_C(b_+299, decCbb3_hook, SYM(decCbb3), b_+302);
  if (!(F & FZ)) {
    CYCT(b_+302, b_+303); ret_effect(gb);
    return;
  }
  CYC(b_+302, b_+303);
  CALL_C(b_+303, incCbc2_hook, SYM(incCbc2), b_+306);
  CYC(b_+306, b_+309); SET_HL(wTmpcbb3);
  CYC(b_+309, b_+311); mem_wr(gb, HL, 0x3c);
  CYC(b_+311, b_+314); endgameCutsceneHandler_0f__state0__func_5cb0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateD_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+314, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+317);
  CALL_C(b_+317, decCbb3_hook, SYM(decCbb3), b_+320);
  if (!(F & FZ)) {
    CYCT(b_+320, b_+321); ret_effect(gb);
    return;
  }
  CYC(b_+320, b_+321);
  CALL_C(b_+321, incCbc1_hook, SYM(incCbc1), b_+324);
  CYC(b_+324, b_+325); L = alu_inc8(gb, L);
  CYC(b_+325, b_+326); alu_xor(gb, A);
  CYC(b_+326, b_+328); mem_wr(gb, HL, A);
  CYC(b_+328, b_+329); A = 0x03;
  CYC(b_+329, b_+332); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+428, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+431);
  if (!(F & FZ)) {
    CYCT(b_+431, b_+432); ret_effect(gb);
    return;
  }
  CYC(b_+431, b_+432);
  CALL_C(b_+432, incCbc2_hook, SYM(incCbc2), b_+435);
  CYC(b_+435, b_+437); A = 0x3c;
  CYC(b_+437, b_+440); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+440, b_+442); A = 0x64;
  CYC(b_+442, b_+445); SET_BC((SYM(secretFunctionCaller_body__jumpTable) + 10));
  CYC(b_+445, b_+448); createEnergySwirlGoingIn_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+448, decCbb3_hook, SYM(decCbb3), b_+451);
  if (!(F & FZ)) {
    CYCT(b_+451, b_+452); ret_effect(gb);
    return;
  }
  CYC(b_+451, b_+452);
  CYC(b_+452, b_+453); alu_xor(gb, A);
  CYC(b_+453, b_+456); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+456, b_+457); A = alu_dec8(gb, A);
  CYC(b_+457, b_+460); mem_wr(gb, wTmpcbba, A);
  CYC(b_+460, b_+463); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+463, b_+466); SET_HL(wTmpcbb3);
  CYC(b_+466, b_+468); B = 0x01;
  CALL_C(b_+468, flashScreen_hook, SYM(flashScreen), b_+471);
  if (F & FZ) {
    CYCT(b_+471, b_+472); ret_effect(gb);
    return;
  }
  CYC(b_+471, b_+472);
  CALL_C(b_+472, incCbc2_hook, SYM(incCbc2), b_+475);
  CYC(b_+475, b_+478); SET_HL(wTmpcbb3);
  CYC(b_+478, b_+480); mem_wr(gb, HL, 0x3c);
  CYC(b_+480, b_+482); A = 0x01;
  CYC(b_+482, b_+485); mem_wr(gb, wRoomLayoutEnd, A);
  CYC(b_+485, b_+487); A = 0x03;
  CYC(b_+487, b_+490); fadeinFromWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate4_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+490, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+493);
  if (!(F & FZ)) {
    CYCT(b_+493, b_+494); ret_effect(gb);
    return;
  }
  CYC(b_+493, b_+494);
  CALL_C(b_+494, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+497);
  CYC(b_+497, b_+499); A = 0x04;
  CYC(b_+499, b_+501); B = 0x02;
  CALL_C(b_+501, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+504);
  CYC(b_+504, b_+506); A = 0x29;
  CALL_C(b_+506, playSound_b00_hook, SYM(playSound_b00), b_+509);
  CYC(b_+509, b_+512); SET_HL(wTmpcbb3);
  CYC(b_+512, b_+514); mem_wr(gb, HL, 0x3c);
  CYC(b_+514, b_+517); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate5_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+517, decCbb3_hook, SYM(decCbb3), b_+520);
  if (!(F & FZ)) {
    CYCT(b_+520, b_+521); ret_effect(gb);
    return;
  }
  CYC(b_+520, b_+521);
  CALL_C(b_+521, incCbc2_hook, SYM(incCbc2), b_+524);
  CYC(b_+524, b_+527); SET_HL(wTmpcbb3);
  CYC(b_+527, b_+529); mem_wr(gb, HL, 0x1e);
  CYC(b_+529, b_+530); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state1__substate6_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+530, decCbb3_hook, SYM(decCbb3), b_+533);
  if (!(F & FZ)) {
    CYCT(b_+533, b_+534); ret_effect(gb);
    return;
  }
  CYC(b_+533, b_+534);
  CALL_C(b_+534, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+537);
  CYC(b_+537, b_+539); A = 0x04;
  CYC(b_+539, b_+541); B = 0x02;
  CALL_C(b_+541, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+544);
  CYC(b_+544, b_+547); SET_HL(wTmpcbb3);
  CYC(b_+547, b_+549); mem_wr(gb, HL, 0x3c);
  CYC(b_+549, b_+552); SET_HL(wRoomLayoutEnd);
  CYC(b_+552, b_+554); mem_wr(gb, HL, 0x02);
  CYC(b_+554, b_+557); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+557, b_+560); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+560, b_+562); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(b_+562, b_+563); ret_effect(gb);
    return;
  }
  CYC(b_+562, b_+563);
  CALL_C(b_+563, incCbc2_hook, SYM(incCbc2), b_+566);
  CYC(b_+566, b_+568); A = 0x03;
  CYC(b_+568, b_+571); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate8_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+571, b_+574); A = W8(wPaletteThread_mode);
  CYC(b_+574, b_+575); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+575, b_+576); ret_effect(gb);
    return;
  }
  CYC(b_+575, b_+576);
  CALL_C(b_+576, incCbc2_hook, SYM(incCbc2), b_+579);
  CALL_C(b_+579, disableLcd_hook, SYM(disableLcd), b_+582);
  CALL_C(b_+582, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+585);
  CALL_C(b_+585, hideStatusBar_hook, SYM(hideStatusBar), b_+588);
  CYC(b_+588, b_+590); A = 0x3c;
  CALL_C(b_+590, loadGfxHeader_hook, SYM(loadGfxHeader), b_+593);
  CYC(b_+593, b_+595); A = 0xc9;
  CALL_C(b_+595, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+598);
  CYC(b_+598, b_+601); SET_HL(wTmpcbb3);
  CYC(b_+601, b_+603); mem_wr(gb, HL, 0xf0);
  CYC(b_+603, b_+605); A = 0x04;
  CALL_C(b_+605, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+608);
  CALL_C(b_+608, cutscene_resetOamWithSomething1_hook, SYM(cutscene_resetOamWithSomething1), b_+611);
  CYC(b_+611, b_+613); A = 0x03;
  CYC(b_+613, b_+616); fadeinFromWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate9_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+616, cutscene_resetOamWithSomething1_hook, SYM(cutscene_resetOamWithSomething1), b_+619);
  CALL_C(b_+619, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+622);
  if (!(F & FZ)) {
    CYCT(b_+622, b_+623); ret_effect(gb);
    return;
  }
  CYC(b_+622, b_+623);
  CALL_C(b_+623, incCbc2_hook, SYM(incCbc2), b_+626);
  CYC(b_+626, b_+629); SET_HL(wTmpcbb3);
  CYC(b_+629, b_+631); mem_wr(gb, HL, 0x10);
  CYC(b_+631, b_+633); A = 0x03;
  CYC(b_+633, b_+636); fadeoutToBlackWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substateA_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+636, cutscene_resetOamWithSomething1_hook, SYM(cutscene_resetOamWithSomething1), b_+639);
  CALL_C(b_+639, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+642);
  if (!(F & FZ)) {
    CYCT(b_+642, b_+643); ret_effect(gb);
    return;
  }
  CYC(b_+642, b_+643);
  CYC(b_+643, b_+645); A = 0x0a;
  CYC(b_+645, b_+648); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CALL_C(b_+648, cutscene_clearTmpCBB3_hook, SYM(cutscene_clearTmpCBB3), b_+651);
  CYC(b_+651, b_+654); SET_HL(wRoomLayout);
  CYC(b_+654, b_+657); SET_BC(0x00c0);
  CALL_C(b_+657, clearMemoryBc_hook, SYM(clearMemoryBc), b_+660);
  CYC(b_+660, b_+663); SET_HL(wRoomCollisions);
  CYC(b_+663, b_+666); SET_BC(0x00c0);
  CALL_C(b_+666, clearMemoryBc_hook, SYM(clearMemoryBc), b_+669);
  CYC(b_+669, b_+670); alu_xor(gb, A);
  CYC(b_+670, b_+672); mem_wr(gb, hCameraY, A);
  CYC(b_+672, b_+674); mem_wr(gb, hCameraX, A);
  CYC(b_+674, b_+677); SET_HL(wTmpcbb3);
  CYC(b_+677, b_+679); mem_wr(gb, HL, 0x3c);
  CYC(b_+679, b_+681); A = 0xfb;
  CYC(b_+681, b_+684); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_0a_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, endgameCutsceneHandler_0a__runStates_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); func_3539_hook(gb);
}

void endgameCutsceneHandler_0a__runStates_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); SET_DE(wGenericCutscene_cbc1);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+19) { endgameCutsceneHandler_0a__state0_hook(gb); return; }
    else if (jt_ == b_+120) { endgameCutsceneHandler_0a__state1_hook(gb); return; }
    else if (jt_ == b_+453) { endgameCutsceneHandler_0a__state2_hook(gb); return; }
    else if (jt_ == b_+461) { endgameCutsceneHandler_0a__state3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_0a__state0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+19, b_+22); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+30) { endgameCutsceneHandler_0a__state0__substate0_hook(gb); return; }
    else if (jt_ == b_+61) { endgameCutsceneHandler_0a__state0__substate1_hook(gb); return; }
    else if (jt_ == b_+89) { endgameCutsceneHandler_0a__state0__substate2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_0a__state1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+120, b_+123); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); push_effect(gb, b_+125);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+135) { endgameCutsceneHandler_0a__state1__substate0_hook(gb); return; }
    else if (jt_ == b_+288) { endgameCutsceneHandler_0a__state1__substate1_hook(gb); return; }
    else if (jt_ == b_+309) { endgameCutsceneHandler_0a__state1__substate2_hook(gb); return; }
    else if (jt_ == b_+377) { endgameCutsceneHandler_0a__state1__substate3_hook(gb); return; }
    else if (jt_ == b_+403) { endgameCutsceneHandler_0a__state1__substate4_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_0a__state1__substate0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+135, b_+136); alu_xor(gb, A);
  CYC(b_+136, b_+138); mem_wr(gb, hOamTail, A);
  CYC(b_+138, b_+141); A = W8(wPaletteThread_mode);
  CYC(b_+141, b_+142); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+142, b_+143); ret_effect(gb);
    return;
  }
  CYC(b_+142, b_+143);
  CALL_C(b_+143, disableLcd_hook, SYM(disableLcd), b_+146);
  CALL_C(b_+146, incCbc2_hook, SYM(incCbc2), b_+149);
  CALL_C(b_+149, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+152);
  CALL_C(b_+152, clearOam_hook, SYM(clearOam), b_+155);
  CYC(b_+155, b_+157); A = 0x10;
  CYC(b_+157, b_+159); mem_wr(gb, hOamTail, A);
  CYC(b_+159, b_+162); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+162, b_+163); C = A;
  CALL_C(b_+163, cutscene_clearCFC0ToCFDF_hook, SYM(cutscene_clearCFC0ToCFDF), b_+166);
  CYC(b_+166, b_+167); A = C;
  CYC(b_+167, b_+170); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CYC(b_+170, b_+172); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(b_+172, b_+174);
    goto load_headers;
  }
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+177); SET_HL(b_+268);
  CYC(b_+177, b_+178); add_double_index_to_hl_from_rst(gb, b_+178);
  CYC(b_+178, b_+179); B = mem_rd(gb, HL);
  CYC(b_+179, b_+180); SET_HL(HL + 1);
  CYC(b_+180, b_+181); C = mem_rd(gb, HL);
  CYC(b_+181, b_+183); A = 0x00;
  CALL_C(b_+183, forceLoadRoom_hook, SYM(forceLoadRoom), b_+186);
  CYC(b_+186, b_+189); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+189, b_+192); SET_HL(b_+276);
  CYC(b_+192, b_+193); add_a_to_hl_from_rst(gb, b_+193);
  CYC(b_+193, b_+194); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+194, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+197);
load_headers:
  CYC(b_+197, b_+200); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+200, b_+201); alu_add(gb, A);
  CYC(b_+201, b_+203); alu_add(gb, 0x85);
  CALL_C(b_+203, loadGfxHeader_hook, SYM(loadGfxHeader), b_+206);
  CYC(b_+206, b_+208); A = 0x0f;
  CALL_C(b_+208, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+211);
  CYC(b_+211, b_+214); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+214, b_+216); B = 0xff;
  CYC(b_+216, b_+217); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+217, b_+219);
    goto load_animation;
  }
  CYC(b_+217, b_+219);
  CYC(b_+219, b_+221); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+221, b_+223);
    goto load_animation;
  }
  CYC(b_+221, b_+223);
  CYC(b_+223, b_+225); B = 0x01;
load_animation:
  CYC(b_+225, b_+226); C = A;
  CYC(b_+226, b_+227); A = B;
  CYC(b_+227, b_+230); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(b_+230, loadAnimationData_hook, SYM(loadAnimationData), b_+233);
  CYC(b_+233, b_+234); A = C;
  CYC(b_+234, b_+237); SET_HL(b_+280);
  CYC(b_+237, b_+238); add_a_to_hl_from_rst(gb, b_+238);
  CYC(b_+238, b_+239); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(b_+239, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+242);
  CALL_C(b_+242, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+245);
  CYC(b_+245, b_+247); A = 0x01;
  CYC(b_+247, b_+250); mem_wr(gb, wScrollMode, A);
  CYC(b_+250, b_+251); alu_xor(gb, A);
  CYC(b_+251, b_+253); mem_wr(gb, hCameraX, A);
  CYC(b_+253, b_+256); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+256, b_+257); B = mem_rd(gb, HL);
  CALL_C(b_+257, cutscene_parseObjectData_andLoadObjectGfx_hook, SYM(cutscene_parseObjectData_andLoadObjectGfx), b_+260);
  CYC(b_+260, b_+262); A = 0x04;
  CALL_C(b_+262, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+265);
  CYC(b_+265, b_+268); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  CYC(b_+453, b_+456); SET_HL((SYM(wallRetraction_dungeon8__cbb3_01) + 45));
  CYC(b_+456, b_+458); E = 0x10;
  CYC(b_+458, b_+461); interBankCall_hook(gb);
}

void endgameCutsceneHandler_0a__state3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a);
  CYC(b_+461, b_+464); SET_HL((SYM(func_03_7244__state0) + 72));
  CYC(b_+464, b_+466); E = 0x10;
  CYC(b_+466, b_+469); interBankCall_hook(gb);
}

void endgameCutsceneHandler_09_stage0__updateExplosionSoundsAndScreenShake_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+140, b_+143); SET_HL(wTmpcbb6);
  CYC(b_+143, b_+144); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+144, b_+145); ret_effect(gb);
    return;
  }
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x10);
  CYC(b_+147, b_+149); A = 0x6f;
  CALL_C(b_+149, playSound_b00_hook, SYM(playSound_b00), b_+152);
  CYC(b_+152, b_+154); A = 0x08;
  CALL_C(b_+154, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+157);
  CYC(b_+157, b_+158); alu_xor(gb, A);
  CYC(b_+158, b_+159); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__explosions_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+228, endgameCutsceneHandler_09_stage0__updateExplosionSoundsAndScreenShake_hook, b_+140, b_+231);
  if (!(F & FZ)) {
    CYCT(b_+231, b_+232); ret_effect(gb);
    return;
  }
  CYC(b_+231, b_+232);
  CALL_C(b_+232, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+235);
  if (!(F & FZ)) {
    CYCT(b_+235, b_+236); ret_effect(gb);
    return;
  }
  CYC(b_+235, b_+236);
  CYC(b_+236, b_+238); mem_wr(gb, HL, 0x99);
  CYC(b_+238, b_+239); L = alu_inc8(gb, L);
  CYC(b_+239, b_+240); L = alu_inc8(gb, L);
  CYC(b_+240, b_+241); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+241, b_+243); A = 0x01;
  CYC(b_+243, b_+246); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+246, b_+247); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__state0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+58, b_+61); A = W8(wPaletteThread_mode);
  CYC(b_+61, b_+62); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+62, b_+63); ret_effect(gb);
    return;
  }
  CYC(b_+62, b_+63);
  CALL_C(b_+63, cutscene_clearCFC0ToCFDF_hook, SYM(cutscene_clearCFC0ToCFDF), b_+66);
  CALL_C(b_+66, incCbc2_hook, SYM(incCbc2), b_+69);
  CYC(b_+69, b_+72); SET_BC(SYM(getNumSetBits));
  CALL_C(b_+72, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+75);
  CALL_C(b_+75, resetCamera_hook, SYM(resetCamera), b_+78);
  CYC(b_+78, b_+80); A = 0xfa;
  CALL_C(b_+80, playSound_b00_hook, SYM(playSound_b00), b_+83);
  CALL_C(b_+83, clearAllParentItems_hook, SYM(clearAllParentItems), b_+86);
  CALL_C(b_+86, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+89);
  CYC(b_+89, b_+92); SET_HL(SYM(createInteraction));
  CALL_C(b_+92, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+95);
  CYC(b_+95, b_+98); SET_HL(wTmpcbb3);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x3c);
  CYC(b_+100, b_+103); SET_HL(SYM(blackTowerEscapeCutscene_doorBlockReplacement));
  CALL_C(b_+103, cutscene_replaceListOfTiles_hook, SYM(cutscene_replaceListOfTiles), b_+106);
  CALL_C(b_+106, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+109);
  CYC(b_+109, b_+111); A = 0x02;
  CALL_C(b_+111, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+114);
  CYC(b_+114, b_+117); fadeinFromWhiteToRoom_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+117, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+120);
  if (!(F & FZ)) {
    CYCT(b_+120, b_+121); ret_effect(gb);
    return;
  }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0x78);
  CYC(b_+123, b_+125); L = 0xb6;
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x10);
  CYC(b_+127, b_+130); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+130, decCbb3_hook, SYM(decCbb3), b_+133);
  if (!(F & FZ)) {
    CYCT(b_+133, b_+135);
    TAIL(endgameCutsceneHandler_09_stage0__updateExplosionSoundsAndScreenShake);
  }
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x3c);
  CYC(b_+137, b_+140); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+159, decCbb3_hook, SYM(decCbb3), b_+162);
  if (!(F & FZ)) {
    CYCT(b_+162, b_+163); ret_effect(gb);
    return;
  }
  CYC(b_+162, b_+163);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x1e);
  CYC(b_+165, b_+168); SET_BC(0x1d0a);
  CALL_C(b_+168, showText_hook, SYM(showText), b_+171);
  CYC(b_+171, b_+174); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state4_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+174, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+177);
  if (!(F & FZ)) {
    CYCT(b_+177, b_+178); ret_effect(gb);
    return;
  }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+180); mem_wr(gb, HL, 0x78);
  CYC(b_+180, b_+182); L = 0xb6;
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0x10);
  CYC(b_+184, b_+187); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state5_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+187, decCbb3_hook, SYM(decCbb3), b_+190);
  if (!(F & FZ)) {
    CYCT(b_+190, b_+192);
    TAIL(endgameCutsceneHandler_09_stage0__explosions);
  }
  CYC(b_+190, b_+192);
  CYC(b_+192, b_+194); mem_wr(gb, HL, 0x28);
  CALL_C(b_+194, incCbc2_hook, SYM(incCbc2), b_+197);
  CYC(b_+197, b_+200); SET_HL(w1Link);
  CYC(b_+200, b_+202); mem_wr(gb, HL, 0x03);
  CYC(b_+202, b_+204); L = 0x0b;
  CYC(b_+204, b_+206); mem_wr(gb, HL, 0x48);
  CYC(b_+206, b_+208); L = 0x0d;
  CYC(b_+208, b_+210); mem_wr(gb, HL, 0x50);
  CYC(b_+210, b_+212); L = 0x08;
  CYC(b_+212, b_+214); mem_wr(gb, HL, 0x02);
  CYC(b_+214, b_+217); SET_HL((SYM(wallRetraction_dungeon8__cbb3_01) + 9));
  CYC(b_+217, b_+219); A = 0x10;
  CALL_C(b_+219, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+222);
  CYC(b_+222, b_+225); SET_HL(SYM(blackTowerEscapeCutscene_doorOpenReplacement));
  CYC(b_+225, b_+228); cutscene_replaceListOfTiles_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state6_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+247, decCbb3_hook, SYM(decCbb3), b_+250);
  if (!(F & FZ)) {
    CYCT(b_+250, b_+252);
    TAIL(endgameCutsceneHandler_09_stage0__explosions);
  }
  CYC(b_+250, b_+252); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+255, b_+258); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+258, b_+260); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+260, b_+261); ret_effect(gb);
    return;
  }
  CYC(b_+260, b_+261);
  CALL_C(b_+261, incCbc2_hook, SYM(incCbc2), b_+264);
  CYC(b_+264, b_+265); alu_xor(gb, A);
  CYC(b_+265, b_+268); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+268, b_+271); mem_wr(gb, wScrollMode, A);
  CYC(b_+271, b_+274); SET_HL((SYM(wallRetraction_dungeon8__cbb3_01) + 12));
  CYC(b_+274, b_+276); A = 0x10;
  CYC(b_+276, b_+279); setSimulatedInputAddress_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state8_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+279, b_+282); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+282, b_+284); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(b_+284, b_+285); ret_effect(gb);
    return;
  }
  CYC(b_+284, b_+285);
  CALL_C(b_+285, incCbc2_hook, SYM(incCbc2), b_+288);
  CYC(b_+288, b_+291); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state9_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+291, b_+294); A = W8(wPaletteThread_mode);
  CYC(b_+294, b_+295); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+295, b_+296); ret_effect(gb);
    return;
  }
  CYC(b_+295, b_+296);
  CALL_C(b_+296, incCbc2_hook, SYM(incCbc2), b_+299);
  CYC(b_+299, b_+302); SET_BC(0x0165);
  CALL_C(b_+302, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+305);
  CALL_C(b_+305, resetCamera_hook, SYM(resetCamera), b_+308);
  CYC(b_+308, b_+310); A = 0x21;
  CALL_C(b_+310, playSound_b00_hook, SYM(playSound_b00), b_+313);
  CYC(b_+313, b_+315); A = 0x02;
  CALL_C(b_+315, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+318);
  CYC(b_+318, b_+321); SET_HL((SYM(endgameCutsceneHandler_body) + 2));
  CALL_C(b_+321, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+324);
  CYC(b_+324, b_+327); SET_HL(wTmpcbb3);
  CYC(b_+327, b_+329); mem_wr(gb, HL, 0x1e);
  CYC(b_+329, b_+332); fadeinFromWhiteToRoom_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateA_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+332, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+335);
  if (!(F & FZ)) {
    CYCT(b_+335, b_+336); ret_effect(gb);
    return;
  }
  CYC(b_+335, b_+336);
  CALL_C(b_+336, incCbc2_hook, SYM(incCbc2), b_+339);
  CYC(b_+339, b_+342); SET_HL(w1Link);
  CYC(b_+342, b_+344); mem_wr(gb, HL, 0x03);
  CYC(b_+344, b_+346); L = 0x0b;
  CYC(b_+346, b_+348); mem_wr(gb, HL, 0x88);
  CYC(b_+348, b_+350); L = 0x0d;
  CYC(b_+350, b_+352); mem_wr(gb, HL, 0x50);
  CYC(b_+352, b_+354); L = 0x08;
  CYC(b_+354, b_+356); mem_wr(gb, HL, 0x00);
  CYC(b_+356, b_+359); SET_HL((SYM(wallRetraction_dungeon8__cbb3_01) + 20));
  CYC(b_+359, b_+361); A = 0x10;
  CALL_C(b_+361, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+364);
  CYC(b_+364, b_+365); alu_xor(gb, A);
  CYC(b_+365, b_+368); mem_wr(gb, wScrollMode, A);
  CYC(b_+368, b_+369); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__stateB_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+369, b_+372); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+372, b_+374); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+374, b_+375); ret_effect(gb);
    return;
  }
  CYC(b_+374, b_+375);
  CALL_C(b_+375, incCbc2_hook, SYM(incCbc2), b_+378);
  CYC(b_+378, b_+381); SET_HL((SYM(wallRetraction_dungeon8__cbb3_01) + 37));
  CYC(b_+381, b_+383); A = 0x10;
  CYC(b_+383, b_+386); setSimulatedInputAddress_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateC_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+386, b_+389); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+389, b_+391); alu_cp(gb, 0x0a);
  if (!(F & FZ)) {
    CYCT(b_+391, b_+392); ret_effect(gb);
    return;
  }
  CYC(b_+391, b_+392);
  CALL_C(b_+392, incCbc2_hook, SYM(incCbc2), b_+395);
  CYC(b_+395, b_+398); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+398, b_+400); mem_wr(gb, HL, 0x08);
  CYC(b_+400, b_+401); L = alu_inc8(gb, L);
  CYC(b_+401, b_+403); mem_wr(gb, HL, 0x00);
  CYC(b_+403, b_+406); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateD_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+406, b_+409); A = W8(wPaletteThread_mode);
  CYC(b_+409, b_+410); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+410, b_+411); ret_effect(gb);
    return;
  }
  CYC(b_+410, b_+411);
  CALL_C(b_+411, incCbc2_hook, SYM(incCbc2), b_+414);
  CALL_C(b_+414, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+417);
  CYC(b_+417, b_+418); alu_xor(gb, A);
  CYC(b_+418, b_+421); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(b_+421, b_+424); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+424, b_+426); A = 0x02;
  CYC(b_+426, b_+429); loadGfxRegisterStateIndex_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateE_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+429, b_+432); A = W8(wPaletteThread_mode);
  CYC(b_+432, b_+433); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+433, b_+434); ret_effect(gb);
    return;
  }
  CYC(b_+433, b_+434);
  CYC(b_+434, b_+437); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+437, b_+438); A = mem_rd(gb, HL);
  CYC(b_+438, b_+440); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+440, b_+441); ret_effect(gb);
    return;
  }
  CYC(b_+440, b_+441);
  CYC(b_+441, b_+442); alu_xor(gb, A);
  CYC(b_+442, b_+443); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+443, b_+444); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+444, b_+445); A = mem_rd(gb, HL);
  CYC(b_+445, b_+447); alu_cp(gb, 0x0a);
  CYC(b_+447, b_+449); A = 0x0d;
  if (!(F & FZ)) {
    CYCT(b_+449, b_+451);
  } else {
    CYC(b_+449, b_+451);
    CYC(b_+451, b_+453); A = 0x0f;
  }
  CYC(b_+453, b_+456); SET_HL(wGenericCutscene_cbc2);
  CYC(b_+456, b_+457); mem_wr(gb, HL, A);
  CYC(b_+457, b_+460); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateF_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+460, b_+463); A = W8(wPaletteThread_mode);
  CYC(b_+463, b_+464); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+464, b_+465); ret_effect(gb);
    return;
  }
  CYC(b_+464, b_+465);
  CALL_C(b_+465, incCbc2_hook, SYM(incCbc2), b_+468);
  CALL_C(b_+468, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+471);
  CYC(b_+471, b_+474); SET_HL(w1Link);
  CYC(b_+474, b_+476); mem_wr(gb, HL, 0x03);
  CYC(b_+476, b_+478); L = 0x0b;
  CYC(b_+478, b_+480); mem_wr(gb, HL, 0x48);
  CYC(b_+480, b_+482); L = 0x0d;
  CYC(b_+482, b_+484); mem_wr(gb, HL, 0x60);
  CYC(b_+484, b_+486); L = 0x08;
  CYC(b_+486, b_+488); mem_wr(gb, HL, 0x00);
  CYC(b_+488, b_+490); A = 0x0b;
  CYC(b_+490, b_+493); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(b_+493, b_+495); A = 0x02;
  CYC(b_+495, b_+498); loadGfxRegisterStateIndex_hook(gb);
}

static void endgameCutsceneHandler_09_stage0__state10_linked(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  CYC(b_+515, b_+518); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+518, b_+520); alu_cp(gb, 0x12);
  if (!(F & FZ)) {
    CYCT(b_+520, b_+521); ret_effect(gb);
    return;
  }
  CYC(b_+520, b_+521);
  CYC(b_+521, b_+524); SET_HL(wGenericCutscene_cbc2);
  CYC(b_+524, b_+526); mem_wr(gb, HL, 0x14);
  CYC(b_+526, b_+527); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__state10_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+498, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+501);
  if (!(F & FZ)) {
    CYCT(b_+501, b_+503);
    endgameCutsceneHandler_09_stage0__state10_linked(gb);
    return;
  }
  CYC(b_+501, b_+503);
  CYC(b_+503, b_+506); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+506, b_+508); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+508, b_+509); ret_effect(gb);
    return;
  }
  CYC(b_+508, b_+509);
  CALL_C(b_+509, incCbc2_hook, SYM(incCbc2), b_+512);
  CYC(b_+512, b_+515); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state11_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+527, b_+530); A = W8(wPaletteThread_mode);
  CYC(b_+530, b_+531); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+531, b_+532); ret_effect(gb);
    return;
  }
  CYC(b_+531, b_+532);
  CALL_C(b_+532, incCbc2_hook, SYM(incCbc2), b_+535);
  CYC(b_+535, b_+538); SET_HL(wTmpcbb3);
  CYC(b_+538, b_+540); mem_wr(gb, HL, 0x3c);
  CYC(b_+540, b_+542); A = 0xff;
  CYC(b_+542, b_+545); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(b_+545, disableLcd_hook, SYM(disableLcd), b_+548);
  CYC(b_+548, b_+550); A = 0x2b;
  CALL_C(b_+550, loadGfxHeader_hook, SYM(loadGfxHeader), b_+553);
  CYC(b_+553, b_+555); A = 0x9d;
  CALL_C(b_+555, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+558);
  CALL_C(b_+558, cutscene_clearObjects_hook, SYM(cutscene_clearObjects), b_+561);
  CALL_C(b_+561, cutscene_resetOamWithSomething2_hook, SYM(cutscene_resetOamWithSomething2), b_+564);
  CYC(b_+564, b_+566); A = 0x04;
  CALL_C(b_+566, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+569);
  CYC(b_+569, b_+572); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  CYC(b_+595, b_+597); A = 0x01;
  CYC(b_+597, b_+600); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+600, b_+603); showText_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state12_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+572, cutscene_resetOamWithSomething2_hook, SYM(cutscene_resetOamWithSomething2), b_+575);
  CALL_C(b_+575, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+578);
  if (!(F & FZ)) {
    CYCT(b_+578, b_+579); ret_effect(gb);
    return;
  }
  CYC(b_+578, b_+579);
  CALL_C(b_+579, incCbc2_hook, SYM(incCbc2), b_+582);
  CYC(b_+582, b_+585); SET_HL(wMenuDisabled);
  CYC(b_+585, b_+587); mem_wr(gb, HL, 0x01);
  CYC(b_+587, b_+590); SET_HL(wTmpcbb3);
  CYC(b_+590, b_+592); mem_wr(gb, HL, 0x3c);
  CYC(b_+592, b_+595); SET_BC(0x1312);
  endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state13_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+603, cutscene_resetOamWithSomething2_hook, SYM(cutscene_resetOamWithSomething2), b_+606);
  CALL_C(b_+606, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+609);
  if (!(F & FZ)) {
    CYCT(b_+609, b_+610); ret_effect(gb);
    return;
  }
  CYC(b_+609, b_+610);
  CALL_C(b_+610, cutscene_clearTmpCBB3_hook, SYM(cutscene_clearTmpCBB3), b_+613);
  CYC(b_+613, b_+615); A = 0x01;
  CYC(b_+615, b_+618); mem_wr(gb, wGenericCutscene_cbc1, A);
  CYC(b_+618, b_+621); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state14_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+621, b_+624); A = mem_rd(gb, wTextIsActive);
  CYC(b_+624, b_+625); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+625, b_+626); ret_effect(gb);
    return;
  }
  CYC(b_+625, b_+626);
  CYC(b_+626, b_+629); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+629, b_+630); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+630, b_+631); ret_effect(gb);
    return;
  }
  CYC(b_+630, b_+631);
  CALL_C(b_+631, incCbc2_hook, SYM(incCbc2), b_+634);
  CYC(b_+634, b_+636); A = 0x04;
  CYC(b_+636, b_+639); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state15_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+639, b_+642); A = W8(wPaletteThread_mode);
  CYC(b_+642, b_+643); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+643, b_+644); ret_effect(gb);
    return;
  }
  CYC(b_+643, b_+644);
  CYC(b_+644, b_+645); alu_xor(gb, A);
  CYC(b_+645, b_+648); mem_wr(gb, wTextIsActive, A);
  CYC(b_+648, b_+650); A = 0x12;
  CYC(b_+650, b_+653); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+653, b_+654); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, endgameCutsceneHandler_09_stage1__runStates_hook, b_+6, b_+3);
  CYC(b_+3, b_+6); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+31, cutscene_resetOamWithSomething2_hook, SYM(cutscene_resetOamWithSomething2), b_+34);
  CYC(b_+34, b_+37); A = W8(wPaletteThread_mode);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+39); ret_effect(gb);
    return;
  }
  CYC(b_+38, b_+39);
  CALL_C(b_+39, incCbc2_hook, SYM(incCbc2), b_+42);
  CYC(b_+42, b_+45); SET_HL(wTmpcbb3);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+47, disableLcd_hook, SYM(disableLcd), b_+50);
  CALL_C(b_+50, clearOam_hook, SYM(clearOam), b_+53);
  CYC(b_+53, b_+55); A = 0x2c;
  CALL_C(b_+55, loadGfxHeader_hook, SYM(loadGfxHeader), b_+58);
  CYC(b_+58, b_+60); A = 0x9e;
  CALL_C(b_+60, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+63);
  CYC(b_+63, b_+65); A = 0x04;
  CALL_C(b_+65, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+68);
  CYC(b_+68, b_+70); A = 0x21;
  CALL_C(b_+70, playSound_b00_hook, SYM(playSound_b00), b_+73);
  CYC(b_+73, b_+76); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+76, b_+78); A = 0x01;
  CYC(b_+78, b_+81); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+81, b_+83); A = 0x3c;
  CYC(b_+83, b_+86); SET_BC(0x280b);
  CALL_C(b_+86, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+89);
  if (!(F & FZ)) {
    CYCT(b_+89, b_+90); ret_effect(gb);
    return;
  }
  CYC(b_+89, b_+90);
  CALL_C(b_+90, incCbc2_hook, SYM(incCbc2), b_+93);
  CYC(b_+93, b_+94); A = E;
  CYC(b_+94, b_+97); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+97, b_+100); showText_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+100, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+103);
  if (!(F & FZ)) {
    CYCT(b_+103, b_+104); ret_effect(gb);
    return;
  }
  CYC(b_+103, b_+104);
  CALL_C(b_+104, incCbc2_hook, SYM(incCbc2), b_+107);
  CYC(b_+107, b_+110); SET_HL(wTmpcbb5);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0xd0);
  endgameCutsceneHandler_09_stage1__loadCertainOamData1_hook(gb);
}

void endgameCutsceneHandler_09_stage1__loadOamData_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  CYC(b_+117, b_+119); B = 0x30;
  CYC(b_+119, b_+120); push_effect(gb, DE);
  CYC(b_+120, b_+123); SET_DE(wTmpcbb5);
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); SET_DE(pop_effect(gb));
  CYC(b_+125, b_+126); C = A;
  CYC(b_+126, b_+129); cutscene_resetOamWithData_hook(gb);
}

void endgameCutsceneHandler_09_stage1__loadCertainOamData1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  CYC(b_+112, b_+115); SET_HL((SYM(intro_gotoTitlescreen) + 2));
  CYC(b_+115, b_+117); E = 0x16;
  endgameCutsceneHandler_09_stage1__loadOamData_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+129, b_+132); SET_HL(wTmpcbb5);
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+133, b_+135);
    TAIL(endgameCutsceneHandler_09_stage1__loadCertainOamData1);
  }
  CYC(b_+133, b_+135);
  CALL_C(b_+135, clearOam_hook, SYM(clearOam), b_+138);
  CYC(b_+138, b_+140); A = 0x0a;
  CALL_C(b_+140, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+143);
  CYC(b_+143, b_+146); SET_HL(wTmpcbb3);
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x1e);
  CYC(b_+148, b_+151); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state4_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+151, decCbb3_hook, SYM(decCbb3), b_+154);
  if (!(F & FZ)) {
    CYCT(b_+154, b_+155); ret_effect(gb);
    return;
  }
  CYC(b_+154, b_+155);
  CALL_C(b_+155, incCbc2_hook, SYM(incCbc2), b_+158);
  CYC(b_+158, b_+161); SET_HL(wTmpcbb5);
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0xd0);
  endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  CYC(b_+163, b_+166); SET_HL((SYM(intro_titlescreen) + 22));
  CYC(b_+166, b_+168); E = 0x16;
  CYC(b_+168, b_+170);
  endgameCutsceneHandler_09_stage1__loadOamData_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state5_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  CYC(b_+170, b_+173); push_effect(gb, b_+173);
  endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(gb);
  CYC(b_+173, b_+176); SET_HL(wTmpcbb5);
  CYC(b_+176, b_+177); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+177, b_+178); A = mem_rd(gb, HL);
  CYC(b_+178, b_+180); alu_sub(gb, 0xa0);
  if (!(F & FZ)) {
    CYCT(b_+180, b_+181); ret_effect(gb);
    return;
  }
  CYC(b_+180, b_+181);
  CYC(b_+181, b_+184); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+184, b_+187); mem_wr(gb, wScreenOffsetX, A);
  CYC(b_+187, b_+189); A = 0x1e;
  CYC(b_+189, b_+192); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+192, b_+195); mem_wr(gb, wOpenedMenuType, A);
  CYC(b_+195, b_+198); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state6_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+198, endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook, b_+163, b_+201);
  CALL_C(b_+201, decCbb3_hook, SYM(decCbb3), b_+204);
  if (!(F & FZ)) {
    CYCT(b_+204, b_+205); ret_effect(gb);
    return;
  }
  CYC(b_+204, b_+205);
  CYC(b_+205, b_+208); SET_HL(wTmpcbb3);
  CYC(b_+208, b_+210); mem_wr(gb, HL, 0x14);
  CYC(b_+210, b_+213); SET_BC(0x280c);
  CALL_C(b_+213, endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene_hook, SYM(endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene), b_+216);
  CYC(b_+216, b_+219); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+219, endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook, b_+163, b_+222);
  CALL_C(b_+222, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+225);
  if (!(F & FZ)) {
    CYCT(b_+225, b_+226); ret_effect(gb);
    return;
  }
  CYC(b_+225, b_+226);
  CYC(b_+226, b_+227); alu_xor(gb, A);
  CYC(b_+227, b_+230); mem_wr(gb, wOpenedMenuType, A);
  CYC(b_+230, b_+231); A = alu_dec8(gb, A);
  CYC(b_+231, b_+234); mem_wr(gb, wTmpcbba, A);
  CYC(b_+234, b_+236); A = 0xd2;
  CALL_C(b_+236, playSound_b00_hook, SYM(playSound_b00), b_+239);
  CYC(b_+239, b_+242); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state8_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+242, endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook, b_+163, b_+245);
  CYC(b_+245, b_+248); SET_HL(wTmpcbb3);
  CYC(b_+248, b_+250); B = 0x02;
  CALL_C(b_+250, flashScreen_hook, SYM(flashScreen), b_+253);
  if (F & FZ) {
    CYCT(b_+253, b_+254); ret_effect(gb);
    return;
  }
  CYC(b_+253, b_+254);
  CALL_C(b_+254, incCbc2_hook, SYM(incCbc2), b_+257);
  CYC(b_+257, b_+260); SET_HL(wTmpcbb3);
  CYC(b_+260, b_+262); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+262, disableLcd_hook, SYM(disableLcd), b_+265);
  CALL_C(b_+265, clearOam_hook, SYM(clearOam), b_+268);
  CYC(b_+268, b_+269); alu_xor(gb, A);
  CYC(b_+269, b_+271); hram_wr(gb, 0x4f, A);
  CYC(b_+271, b_+274); SET_HL(0x8000);
  CYC(b_+274, b_+277); SET_BC(0x2000);
  CALL_C(b_+277, clearMemoryBc_hook, SYM(clearMemoryBc), b_+280);
  CYC(b_+280, b_+281); alu_xor(gb, A);
  CYC(b_+281, b_+283); hram_wr(gb, 0x4f, A);
  CYC(b_+283, b_+286); SET_HL(0x9c00);
  CYC(b_+286, b_+289); SET_BC(0x0400);
  CALL_C(b_+289, clearMemoryBc_hook, SYM(clearMemoryBc), b_+292);
  CYC(b_+292, b_+294); A = 0x01;
  CYC(b_+294, b_+296); hram_wr(gb, 0x4f, A);
  CYC(b_+296, b_+299); SET_HL(0x9c00);
  CYC(b_+299, b_+302); SET_BC(0x0400);
  CALL_C(b_+302, clearMemoryBc_hook, SYM(clearMemoryBc), b_+305);
  CYC(b_+305, b_+307); A = 0x2d;
  CALL_C(b_+307, loadGfxHeader_hook, SYM(loadGfxHeader), b_+310);
  CYC(b_+310, b_+312); A = 0x9c;
  CALL_C(b_+312, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+315);
  CYC(b_+315, b_+317); A = 0x04;
  CALL_C(b_+317, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+320);
  CYC(b_+320, b_+322); A = 0xd2;
  CALL_C(b_+322, playSound_b00_hook, SYM(playSound_b00), b_+325);
  CYC(b_+325, b_+328); clearPaletteFadeVariablesAndRefreshPalettes_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state9_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+328, decCbb3_hook, SYM(decCbb3), b_+331);
  if (!(F & FZ)) {
    CYCT(b_+331, b_+332); ret_effect(gb);
    return;
  }
  CYC(b_+331, b_+332);
  CYC(b_+332, b_+334); A = 0x0a;
  CYC(b_+334, b_+337); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CALL_C(b_+337, cutscene_clearTmpCBB3_hook, SYM(cutscene_clearTmpCBB3), b_+340);
  CYC(b_+340, b_+343); SET_HL(wRoomLayout);
  CYC(b_+343, b_+346); SET_BC(0x00c0);
  CALL_C(b_+346, clearMemoryBc_hook, SYM(clearMemoryBc), b_+349);
  CYC(b_+349, b_+352); SET_HL(wRoomCollisions);
  CYC(b_+352, b_+355); SET_BC(0x00c0);
  CALL_C(b_+355, clearMemoryBc_hook, SYM(clearMemoryBc), b_+358);
  CYC(b_+358, b_+360); mem_wr(gb, hCameraY, A);
  CYC(b_+360, b_+362); mem_wr(gb, hCameraX, A);
  CYC(b_+362, b_+365); SET_HL(wTmpcbb3);
  CYC(b_+365, b_+367); mem_wr(gb, HL, 0x3c);
  CYC(b_+367, b_+369); A = 0x03;
  CYC(b_+369, b_+372); fadeoutToBlackWithDelay_hook(gb);
}

void disableLcdAndLoadRoom_body_hook(GB *gb) {
  BASE(disableLcdAndLoadRoom_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+4); mem_wr(gb, wActiveGroup, A);
  CYC(b_+4, b_+5); A = C;
  CYC(b_+5, b_+8); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+8, disableLcd_hook, SYM(disableLcd), b_+11);
  CALL_C(b_+11, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+14);
  CYC(b_+14, b_+17); SET_HL(wLinkInAir);
  CYC(b_+17, b_+19); B = 0x8d;
  CALL_C(b_+19, clearMemory_hook, SYM(clearMemory), b_+22);
  CALL_C(b_+22, initializeVramMaps_hook, SYM(initializeVramMaps), b_+25);
  CALL_C(b_+25, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+28);
  CALL_C(b_+28, loadTilesetData_hook, SYM(loadTilesetData), b_+31);
  CALL_C(b_+31, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+34);
  CALL_C(b_+34, func_131f_hook, SYM(func_131f), b_+37);
  CYC(b_+37, b_+39); A = 0x01;
  CYC(b_+39, b_+42); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+42, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+45);
  CALL_C(b_+45, clearOam_hook, SYM(clearOam), b_+48);
  CYC(b_+48, b_+50); A = 0x10;
  CYC(b_+50, b_+52); H8(hOamTail) = A;
  CYC(b_+52, b_+53); ret_effect(gb);
}

static void endgame_20_after_call_5857_body(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+3, b_+6);
  updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_20_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, endgameCutsceneHandler_20__runStates_hook, b_+6, b_+3);
  endgame_20_after_call_5857_body(gb);
}

void endgameCutsceneHandler_20__afterCall5857_hook(GB *gb) {
  endgame_20_after_call_5857_body(gb);
}

void endgameCutsceneHandler_20__runStates_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp;
  CYC(b_+6, b_+9); SET_DE(wGenericCutscene_cbc1);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+57) { endgameCutsceneHandler_20__state0_hook(gb); return; }
    else if (jt_ == b_+107) { endgameCutsceneHandler_20__state1_hook(gb); return; }
    else if (jt_ == b_+130) { endgameCutsceneHandler_20__state2_hook(gb); return; }
    else if (jt_ == b_+146) { endgameCutsceneHandler_20__state3_hook(gb); return; }
    else if (jt_ == b_+228) { endgameCutsceneHandler_20__state4_hook(gb); return; }
    else if (jt_ == b_+250) { endgameCutsceneHandler_20__state5_hook(gb); return; }
    else if (jt_ == b_+272) { endgameCutsceneHandler_20__state6_hook(gb); return; }
    else if (jt_ == b_+279) { endgameCutsceneHandler_20__state7_hook(gb); return; }
    else if (jt_ == b_+293) { endgameCutsceneHandler_20__state8_hook(gb); return; }
    else if (jt_ == b_+365) { endgameCutsceneHandler_20__state9_hook(gb); return; }
    else if (jt_ == b_+430) { endgameCutsceneHandler_20__stateA_hook(gb); return; }
    else if (jt_ == b_+468) { endgameCutsceneHandler_20__stateB_hook(gb); return; }
    else if (jt_ == b_+491) { endgameCutsceneHandler_20__stateC_hook(gb); return; }
    else if (jt_ == b_+509) { endgameCutsceneHandler_20__stateD_hook(gb); return; }
    else if (jt_ == b_+613) { endgameCutsceneHandler_20__stateE_hook(gb); return; }
    else if (jt_ == b_+645) { endgameCutsceneHandler_20__stateF_hook(gb); return; }
    else if (jt_ == b_+653) { endgameCutsceneHandler_20__state10_hook(gb); return; }
    else if (jt_ == b_+665) { endgameCutsceneHandler_20__state11_hook(gb); return; }
    else if (jt_ == b_+673) { endgameCutsceneHandler_20__state12_hook(gb); return; }
    else if (jt_ == b_+678) { endgameCutsceneHandler_20__state13_hook(gb); return; }
    else if (jt_ == b_+698) { endgameCutsceneHandler_20__state14_hook(gb); return; }
    else if (jt_ == b_+721) { endgameCutsceneHandler_20__state15_hook(gb); return; }
    else if (jt_ == b_+748) { endgameCutsceneHandler_20__state16_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void endgame_20_state0_after_call_5895_body(GB *gb, uint16_t sp0_) {
  BASE(endgameCutsceneHandler_20);
  CALL_C(b_+65, hideStatusBar_hook, SYM(hideStatusBar), b_+68);
  CYC(b_+68, b_+70); A = 0xac;
  CALL_C(b_+70, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+73);
  CYC(b_+73, b_+74); alu_xor(gb, A);
  CYC(b_+74, b_+77); mem_wr(gb, wPaletteThread_mode, A);
  CALL_C(b_+77, clearFadingPalettes2_hook, SYM(clearFadingPalettes2), b_+80);
  CYC(b_+80, b_+83); SET_HL(wTmpcbb3);
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x1e);
  CYC(b_+85, b_+87); A = 0x13;
  CALL_C(b_+87, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+90);
  CYC(b_+90, b_+93); SET_HL(wGfxRegs1_SCY);
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+94, b_+96); H8(hCameraY) = A;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+99); H8(hCameraX) = A;
  CYC(b_+99, b_+101); A = 0x00;
  CYC(b_+101, b_+104); mem_wr(gb, wScrollMode, A);
  CYC(b_+104, b_+107);
  incCbc1_hook(gb);
}

void endgameCutsceneHandler_20__state0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+57, b_+59); A = 0x0b;
  CYC(b_+59, b_+62); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CALL_C(b_+62, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+65);
  endgame_20_state0_after_call_5895_body(gb, sp0_);
}

void endgameCutsceneHandler_20__state0__afterCall5895_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  endgame_20_state0_after_call_5895_body(gb, sp0_);
}

void endgameCutsceneHandler_20__state1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+107, decCbb3_hook, SYM(decCbb3), b_+110);
  if (!(F & FZ)) {
    CYCT(b_+110, b_+111); ret_effect(gb);
    return;
  }
  CYC(b_+110, b_+111);
  CALL_C(b_+111, incCbc1_hook, SYM(incCbc1), b_+114);
  CYC(b_+114, b_+117); SET_HL(wTmpcbb3);
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x28);
  CYC(b_+119, b_+121); A = 0x04;
  CYC(b_+121, b_+124); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+124, b_+127); SET_BC(0x2825);
  CYC(b_+127, b_+130); showText_hook(gb);
}

void endgameCutsceneHandler_20__state2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+130, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+133);
  if (!(F & FZ)) {
    CYCT(b_+133, b_+134); ret_effect(gb);
    return;
  }
  CYC(b_+133, b_+134);
  CALL_C(b_+134, incCbc1_hook, SYM(incCbc1), b_+137);
  CYC(b_+137, b_+139); A = 0x20;
  CYC(b_+139, b_+142); SET_HL(wTmpcbb3);
  CYC(b_+142, b_+143); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+143, b_+144); alu_xor(gb, A);
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CYC(b_+145, b_+146); ret_effect(gb);
}

void endgameCutsceneHandler_20__state3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+146, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+149);
  if (!(F & FZ)) {
    CYCT(b_+149, b_+150); ret_effect(gb);
    return;
  }
  CYC(b_+149, b_+150);
  CYC(b_+150, b_+153); SET_HL(wTmpcbb3);
  CYC(b_+153, b_+155); mem_wr(gb, HL, 0x20);
  CYC(b_+155, b_+156); SET_HL(HL + 1);
  CYC(b_+156, b_+157); A = mem_rd(gb, HL);
  CYC(b_+157, b_+159); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+159, b_+161);
    goto continue_state3;
  }
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+162); B = A;
  CYC(b_+162, b_+163); push_effect(gb, HL);
  CYC(b_+163, b_+165); A = 0x72;
  CALL_C(b_+165, playSound_b00_hook, SYM(playSound_b00), b_+168);
  CYC(b_+168, b_+169); SET_HL(pop_effect(gb));
  CYC(b_+169, b_+170); A = B;

continue_state3:
  CYC(b_+170, b_+171); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+171, b_+174); SET_HL(b_+222);
  CYC(b_+174, b_+175); add_a_to_hl_from_rst(gb, b_+175);
  CYC(b_+175, b_+176); A = mem_rd(gb, HL);
  CYC(b_+176, b_+177); alu_or(gb, A);
  CYC(b_+177, b_+178); B = A;
  if (!(F & FZ)) {
    CYCT(b_+178, b_+180);
    TAIL(endgameCutsceneHandler_20__func_5920);
  }
  CYC(b_+178, b_+180);
  CALL_C(b_+180, fadeinFromBlack_hook, SYM(fadeinFromBlack), b_+183);
  CYC(b_+183, b_+185); A = 0x01;
  CYC(b_+185, b_+188); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+188, b_+191); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+191, b_+194); SET_HL(wTmpcbb3);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0x3c);
  CYC(b_+196, b_+198); A = 0x1d;
  CALL_C(b_+198, playSound_b00_hook, SYM(playSound_b00), b_+201);
  CYC(b_+201, b_+204); incCbc1_hook(gb);
}

void endgameCutsceneHandler_20__func_5920_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+204, fastFadeinFromBlack_hook, SYM(fastFadeinFromBlack), b_+207);
  CYC(b_+207, b_+208); A = B;
  CYC(b_+208, b_+211); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+211, b_+214); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+214, b_+215); alu_xor(gb, A);
  CYC(b_+215, b_+218); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+218, b_+221); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+221, b_+222); ret_effect(gb);
}

void endgameCutsceneHandler_20__state4_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+228, b_+230); E = 0x28;
  CYC(b_+230, b_+233); SET_BC(0x2826);
  CALL_C(b_+233, endgameCutsceneHandler_20__func_5943_hook, b_+239, b_+236);
  CYC(b_+236, b_+239); cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(gb);
}

void endgameCutsceneHandler_20__func_5943_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+239, b_+241); A = 0x08;
  CYC(b_+241, b_+244); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+244, b_+246); A = 0x03;
  CYC(b_+246, b_+249); mem_wr(gb, wTextboxPosition, A);
  CYC(b_+249, b_+250); ret_effect(gb);
}

void endgameCutsceneHandler_20__state5_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+250, b_+252); E = 0x28;
  CYC(b_+252, b_+255); SET_BC(0x2827);
  endgameCutsceneHandler_20__func_5953_hook(gb);
}

void endgameCutsceneHandler_20__func_5953_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+255, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+258);
  if (!(F & FZ)) {
    CYCT(b_+258, b_+259); ret_effect(gb);
    return;
  }
  CYC(b_+258, b_+259);
  CALL_C(b_+259, incCbc1_hook, SYM(incCbc1), b_+262);
  CYC(b_+262, b_+265); SET_HL(wTmpcbb3);
  CYC(b_+265, b_+266); mem_wr(gb, HL, E);
  CALL_C(b_+266, endgameCutsceneHandler_20__func_5943_hook, b_+239, b_+269);
  CYC(b_+269, b_+272); showText_hook(gb);
}

void endgameCutsceneHandler_20__state6_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+272, b_+274); E = 0x3c;
  CYC(b_+274, b_+277); SET_BC(0x2828);
  CYC(b_+277, b_+279); endgameCutsceneHandler_20__func_5953_hook(gb);
}

void endgameCutsceneHandler_20__state7_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+279, b_+281); E = 0xb4;
  endgameCutsceneHandler_20__func_596d_hook(gb);
}

void endgameCutsceneHandler_20__func_596d_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+281, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+284);
  if (!(F & FZ)) {
    CYCT(b_+284, b_+285); ret_effect(gb);
    return;
  }
  CYC(b_+284, b_+285);
  CALL_C(b_+285, incCbc1_hook, SYM(incCbc1), b_+288);
  CYC(b_+288, b_+291); SET_HL(wTmpcbb3);
  CYC(b_+291, b_+292); mem_wr(gb, HL, E);
  CYC(b_+292, b_+293); ret_effect(gb);
}

void endgameCutsceneHandler_20__state8_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+293, endgameCutsceneHandler_20__func_5995_hook, b_+321, b_+296);
  CALL_C(b_+296, cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook, SYM(cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0), b_+299);
  CALL_C(b_+299, decCbb3_hook, SYM(decCbb3), b_+302);
  if (!(F & FZ)) {
    CYCT(b_+302, b_+303); ret_effect(gb);
    return;
  }
  CYC(b_+302, b_+303);
  CYC(b_+303, b_+305); A = 0xf1;
  CALL_C(b_+305, playSound_b00_hook, SYM(playSound_b00), b_+308);
  CYC(b_+308, b_+310); A = 0xfb;
  CALL_C(b_+310, playSound_b00_hook, SYM(playSound_b00), b_+313);
  CALL_C(b_+313, incCbc1_hook, SYM(incCbc1), b_+316);
  CYC(b_+316, b_+318); A = 0x04;
  CYC(b_+318, b_+321); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_20__func_5995_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+321, b_+324); SET_HL(wGfxRegs1_SCY);
  CYC(b_+324, b_+326); A = mem_rd(gb, hCameraY);
  CYC(b_+326, b_+327); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+327, b_+329); A = mem_rd(gb, hCameraX);
  CYC(b_+329, b_+330); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+330, b_+333); SET_HL(b_+343);
  CYC(b_+333, b_+336); SET_DE(wGfxRegs1_SCY);
  CALL_C(b_+336, endgameCutsceneHandler_20__func_59b3_hook, b_+351, b_+339);
  CYC(b_+339, b_+340); SET_DE(DE + 1);
  CYC(b_+340, b_+343); endgameCutsceneHandler_20__func_59b3_hook(gb);
}

void endgameCutsceneHandler_20__func_59b3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+351, b_+352); push_effect(gb, HL);
  CALL_C(b_+352, getRandomNumber_hook, SYM(getRandomNumber), b_+355);
  CYC(b_+355, b_+357); alu_and(gb, 0x07);
  CYC(b_+357, b_+358); add_a_to_hl_from_rst(gb, b_+358);
  CYC(b_+358, b_+359); A = mem_rd(gb, HL);
  CYC(b_+359, b_+360); B = A;
  CYC(b_+360, b_+361); A = mem_rd(gb, DE);
  CYC(b_+361, b_+362); alu_add(gb, B);
  CYC(b_+362, b_+363); mem_wr(gb, DE, A);
  CYC(b_+363, b_+364); SET_HL(pop_effect(gb));
  CYC(b_+364, b_+365); ret_effect(gb);
}

static void endgame_20_state9_after_call_59d4_body(GB *gb, uint16_t sp0_) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+384, b_+387); SET_HL(w1Link_enabled);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x03);
  CYC(b_+389, b_+391); L = (uint8_t)w1Link_yh;
  CYC(b_+391, b_+393); mem_wr(gb, HL, 0x48);
  CYC(b_+393, b_+395); L = (uint8_t)w1Link_xh;
  CYC(b_+395, b_+397); mem_wr(gb, HL, 0x60);
  CYC(b_+397, b_+399); L = (uint8_t)w1Link_direction;
  CYC(b_+399, b_+401); mem_wr(gb, HL, 0x00);
  CYC(b_+401, b_+403); A = 0x81;
  CYC(b_+403, b_+406); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+406, b_+409); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+409, cutscene_clearCFC0ToCFDF_hook, SYM(cutscene_clearCFC0ToCFDF), b_+412);
  CALL_C(b_+412, showStatusBar_hook, SYM(showStatusBar), b_+415);
  CYC(b_+415, b_+417); A = 0xf1;
  CALL_C(b_+417, playSound_b00_hook, SYM(playSound_b00), b_+420);
  CYC(b_+420, b_+422); A = 0xf0;
  CALL_C(b_+422, playSound_b00_hook, SYM(playSound_b00), b_+425);
  CYC(b_+425, b_+427); A = 0x02;
  CYC(b_+427, b_+430);
  loadGfxRegisterStateIndex_hook(gb);
}

void endgameCutsceneHandler_20__state9_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+365, endgameCutsceneHandler_20__func_5995_hook, b_+321, b_+368);
  CYC(b_+368, b_+371); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+371, b_+372); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+372, b_+373); ret_effect(gb); return; }
  CYC(b_+372, b_+373);
  CALL_C(b_+373, incCbc1_hook, SYM(incCbc1), b_+376);
  CYC(b_+376, b_+378); A = 0x0c;
  CYC(b_+378, b_+381); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CALL_C(b_+381, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+384);
  endgame_20_state9_after_call_59d4_body(gb, sp0_);
}

void endgameCutsceneHandler_20__state9__afterCall59d4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  endgame_20_state9_after_call_59d4_body(gb, sp0_);
}

void endgameCutsceneHandler_20__stateA_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+430, updateStatusBar_hook, SYM(updateStatusBar), b_+433);
  CYC(b_+433, b_+436); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+436, b_+438); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+438, b_+439); ret_effect(gb);
    return;
  }
  CYC(b_+438, b_+439);
  CALL_C(b_+439, incCbc1_hook, SYM(incCbc1), b_+442);
  CYC(b_+442, b_+444); C = 0x40;
  CYC(b_+444, b_+446); A = 0x29;
  CALL_C(b_+446, giveTreasure_hook, SYM(giveTreasure), b_+449);
  CYC(b_+449, b_+451); A = 0x08;
  CALL_C(b_+451, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+454);
  CYC(b_+454, b_+456); L = 0x02;
  CYC(b_+456, b_+458); mem_wr(gb, HL, 0x0c);
  CYC(b_+458, b_+461); SET_HL(wTmpcbb3);
  CYC(b_+461, b_+463); mem_wr(gb, HL, 0x5a);
  CYC(b_+463, b_+465); A = 0x4a;
  CYC(b_+465, b_+468); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_20__stateB_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+468, updateStatusBar_hook, SYM(updateStatusBar), b_+471);
  CALL_C(b_+471, decCbb3_hook, SYM(decCbb3), b_+474);
  if (!(F & FZ)) {
    CYCT(b_+474, b_+475); ret_effect(gb);
    return;
  }
  CYC(b_+474, b_+475);
  CALL_C(b_+475, incCbc1_hook, SYM(incCbc1), b_+478);
  CYC(b_+478, b_+481); SET_HL(wTmpcbb3);
  CYC(b_+481, b_+483); mem_wr(gb, HL, 0xb4);
  CYC(b_+483, b_+486); SET_BC((SYM(generateSecret) + 14));
  CYC(b_+486, b_+488); A = 0xff;
  CYC(b_+488, b_+491); createEnergySwirlGoingOut_hook(gb);
}

void endgameCutsceneHandler_20__stateC_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+491, updateStatusBar_hook, SYM(updateStatusBar), b_+494);
  CALL_C(b_+494, decCbb3_hook, SYM(decCbb3), b_+497);
  if (!(F & FZ)) {
    CYCT(b_+497, b_+498); ret_effect(gb);
    return;
  }
  CYC(b_+497, b_+498);
  CALL_C(b_+498, incCbc1_hook, SYM(incCbc1), b_+501);
  CYC(b_+501, b_+504); SET_HL(wTmpcbb3);
  CYC(b_+504, b_+506); mem_wr(gb, HL, 0x3c);
  CYC(b_+506, b_+509); fadeoutToWhite_hook(gb);
}

static void endgame_20_state_d_after_call_5a64_body(GB *gb, uint16_t sp0_) {
  BASE(endgameCutsceneHandler_20);
  CALL_C(b_+528, hideStatusBar_hook, SYM(hideStatusBar), b_+531);
  CYC(b_+531, b_+533); A = 0x02;
  CYC(b_+533, b_+535); mem_wr(gb, IO_SVBK, A);
  CYC(b_+535, b_+538); SET_HL(w2TilesetBgPalettes + 0x10);
  CYC(b_+538, b_+540); B = 0x08;
  CYC(b_+540, b_+542); A = 0xff;
  CALL_C(b_+542, fillMemory_hook, SYM(fillMemory), b_+545);
  CYC(b_+545, b_+546); alu_xor(gb, A);
  CYC(b_+546, b_+548); mem_wr(gb, IO_SVBK, A);
  CYC(b_+548, b_+550); A = 0x07;
  CYC(b_+550, b_+552); H8(hDirtyBgPalettes) = A;
  CALL_C(b_+552, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+555);
  if (!(F & FZ)) {
    CYCT(b_+555, b_+557);
  } else {
    CYC(b_+555, b_+557);
    CYC(b_+557, b_+559); mem_wr(gb, HL, 0x36);
    CYC(b_+559, b_+560); L = alu_inc8(gb, L);
    CYC(b_+560, b_+562); mem_wr(gb, HL, 0x12);
    CALL_C(b_+562, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+565);
    if (!(F & FZ)) {
      CYCT(b_+565, b_+567);
    } else {
      CYC(b_+565, b_+567);
      CYC(b_+567, b_+569); mem_wr(gb, HL, 0xaa);
      CYC(b_+569, b_+570); L = alu_inc8(gb, L);
      CYC(b_+570, b_+572); mem_wr(gb, HL, 0x02);
    }
  }
  CYC(b_+572, b_+574); A = 0x02;
  CYC(b_+574, b_+577); mem_wr(gb, wOpenedMenuType, A);
  CALL_C(b_+577, func_6e9a_hook, SYM(func_6e9a), b_+580);
  CYC(b_+580, b_+582); A = 0x02;
  CALL_C(b_+582, func_6ed6_hook, SYM(func_6ed6), b_+585);
  CYC(b_+585, b_+588); SET_HL(wTmpcbb3);
  CYC(b_+588, b_+590); mem_wr(gb, HL, 0x1e);
  CYC(b_+590, b_+592); A = 0x04;
  CALL_C(b_+592, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+595);
  CYC(b_+595, b_+597); A = 0x10;
  CYC(b_+597, b_+599); H8(hCameraY) = A;
  CYC(b_+599, b_+600); alu_xor(gb, A);
  CYC(b_+600, b_+602); H8(hCameraX) = A;
  CYC(b_+602, b_+604); A = 0x00;
  CYC(b_+604, b_+607); mem_wr(gb, wScrollMode, A);
  CYC(b_+607, b_+610); SET_BC(0x1d1a);
  CYC(b_+610, b_+613);
  showText_hook(gb);
}

void endgameCutsceneHandler_20__stateD_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+509, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+512);
  if (!(F & FZ)) { CYCT(b_+512, b_+513); ret_effect(gb); return; }
  CYC(b_+512, b_+513);
  CALL_C(b_+513, incCbc1_hook, SYM(incCbc1), b_+516);
  CALL_C(b_+516, disableLcd_hook, SYM(disableLcd), b_+519);
  CALL_C(b_+519, clearOam_hook, SYM(clearOam), b_+522);
  CALL_C(b_+522, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+525);
  CALL_C(b_+525, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+528);
  endgame_20_state_d_after_call_5a64_body(gb, sp0_);
}

void endgameCutsceneHandler_20__stateD__afterCall5a64_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  endgame_20_state_d_after_call_5a64_body(gb, sp0_);
}

void endgameCutsceneHandler_20__stateE_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+613, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+616);
  if (!(F & FZ)) {
    CYCT(b_+616, b_+617); ret_effect(gb);
    return;
  }
  CYC(b_+616, b_+617);
  CALL_C(b_+617, incCbc1_hook, SYM(incCbc1), b_+620);
  CYC(b_+620, b_+622); B = 0x04;
  endgameCutsceneHandler_20__func_5ac2_hook(gb);
}

void endgameCutsceneHandler_20__func_5ac2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+622, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+625);
  CYC(b_+625, b_+626); A = B;
  CYC(b_+626, b_+629); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+629, b_+632); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+632, b_+633); alu_xor(gb, A);
  CYC(b_+633, b_+636); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+636, b_+639); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+639, b_+642); SET_HL(wTmpcbb3);
  CYC(b_+642, b_+644); mem_wr(gb, HL, 0x3c);
  CYC(b_+644, b_+645); ret_effect(gb);
}

void endgameCutsceneHandler_20__stateF_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+645, b_+647); E = 0x1e;
  CYC(b_+647, b_+650); SET_BC(0x1d1b);
  CYC(b_+650, b_+653); cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(gb);
}

void endgameCutsceneHandler_20__state10_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+653, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+656);
  if (!(F & FZ)) {
    CYCT(b_+656, b_+657); ret_effect(gb);
    return;
  }
  CYC(b_+656, b_+657);
  CALL_C(b_+657, incCbc1_hook, SYM(incCbc1), b_+660);
  CYC(b_+660, b_+662); B = 0x12;
  CYC(b_+662, b_+665); endgameCutsceneHandler_20__func_5ac2_hook(gb);
}

void endgameCutsceneHandler_20__state11_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+665, b_+667); E = 0x1e;
  CYC(b_+667, b_+670); SET_BC(0x1d1c);
  CYC(b_+670, b_+673); cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(gb);
}

void endgameCutsceneHandler_20__state12_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  CYC(b_+673, b_+675); E = 0x3c;
  CYC(b_+675, b_+678); endgameCutsceneHandler_20__func_596d_hook(gb);
}

void endgameCutsceneHandler_20__state13_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+678, decCbb3_hook, SYM(decCbb3), b_+681);
  if (!(F & FZ)) {
    CYCT(b_+681, b_+682); ret_effect(gb);
    return;
  }
  CYC(b_+681, b_+682);
  CALL_C(b_+682, incCbc1_hook, SYM(incCbc1), b_+685);
  CYC(b_+685, b_+688); SET_HL(wTmpcbb3);
  CYC(b_+688, b_+690); mem_wr(gb, HL, 0xf0);
  CYC(b_+690, b_+692); A = 0xff;
  CYC(b_+692, b_+695); SET_BC((SYM(secretFunctionCaller_body__jumpTable) + 10));
  CYC(b_+695, b_+698); createEnergySwirlGoingOut_hook(gb);
}

void endgameCutsceneHandler_20__state14_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+698, decCbb3_hook, SYM(decCbb3), b_+701);
  if (!(F & FZ)) {
    CYCT(b_+701, b_+702); ret_effect(gb);
    return;
  }
  CYC(b_+701, b_+702);
  CYC(b_+702, b_+705); SET_HL(wTmpcbb3);
  CYC(b_+705, b_+707); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+707, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+710);
  CYC(b_+710, b_+712); A = 0xfc;
  CYC(b_+712, b_+715); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+715, b_+718); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+718, b_+721); incCbc1_hook(gb);
}

void endgameCutsceneHandler_20__state15_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+721, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+724);
  if (!(F & FZ)) {
    CYCT(b_+724, b_+725); ret_effect(gb);
    return;
  }
  CYC(b_+724, b_+725);
  CALL_C(b_+725, incCbc1_hook, SYM(incCbc1), b_+728);
  CALL_C(b_+728, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+731);
  CALL_C(b_+731, clearParts_hook, SYM(clearParts), b_+734);
  CALL_C(b_+734, clearOam_hook, SYM(clearOam), b_+737);
  CYC(b_+737, b_+740); SET_HL(wTmpcbb3);
  CYC(b_+740, b_+742); mem_wr(gb, HL, 0x3c);
  CYC(b_+742, b_+745); SET_BC(0x1d1d);
  CYC(b_+745, b_+748); showTextNonExitable_hook(gb);
}

void endgameCutsceneHandler_20__state16_hook(GB *gb) {
  BASE(endgameCutsceneHandler_20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+748, b_+751); A = W8(wTextIsActive);
  CYC(b_+751, b_+752); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+752, b_+753); ret_effect(gb);
    return;
  }
  CYC(b_+752, b_+753);
  CALL_C(b_+753, decCbb3_hook, SYM(decCbb3), b_+756);
  if (!(F & FZ)) {
    CYCT(b_+756, b_+757); ret_effect(gb);
    return;
  }
  CYC(b_+756, b_+757);
  CALL_C(b_+757, showStatusBar_hook, SYM(showStatusBar), b_+760);
  CYC(b_+760, b_+761); alu_xor(gb, A);
  CYC(b_+761, b_+764); W8(wOpenedMenuType) = A;
  CYC(b_+764, b_+765); A = alu_dec8(gb, A);
  CYC(b_+765, b_+768); W8(wActiveMusic) = A;
  CYC(b_+768, b_+770); A = 0xfa;
  CALL_C(b_+770, playSound_b00_hook, SYM(playSound_b00), b_+773);
  CYC(b_+773, b_+776); SET_HL(b_+779);
  CYC(b_+776, b_+779); setWarpDestVariables_hook(gb);
}

void endgameCutsceneHandler_0f_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wGenericCutscene_cbc1);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+9) { endgameCutsceneHandler_0f__state0_hook(gb); return; }
    else if (jt_ == b_+346) { endgameCutsceneHandler_0f__state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_0f__state0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+9, updateStatusBar_hook, SYM(updateStatusBar), b_+12);
  CALL_C(b_+12, endgameCutsceneHandler_0f__state0__runSubstates_hook, b_+18, b_+15);
  CYC(b_+15, b_+18); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_0f__state0__runSubstates_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+18, b_+21); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); push_effect(gb, b_+23);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+51) { endgameCutsceneHandler_0f__state0__substate0_hook(gb); return; }
    else if (jt_ == b_+92) { endgameCutsceneHandler_0f__state0__substate1_hook(gb); return; }
    else if (jt_ == b_+109) { endgameCutsceneHandler_0f__state0__substate2_hook(gb); return; }
    else if (jt_ == b_+127) { endgameCutsceneHandler_0f__state0__substate3_hook(gb); return; }
    else if (jt_ == b_+153) { endgameCutsceneHandler_0f__state0__substate4_hook(gb); return; }
    else if (jt_ == b_+173) { endgameCutsceneHandler_0f__state0__substate5_hook(gb); return; }
    else if (jt_ == b_+188) { endgameCutsceneHandler_0f__state0__substate6_hook(gb); return; }
    else if (jt_ == b_+208) { endgameCutsceneHandler_0f__state0__substate7_hook(gb); return; }
    else if (jt_ == b_+226) { endgameCutsceneHandler_0f__state0__substate8_hook(gb); return; }
    else if (jt_ == b_+244) { endgameCutsceneHandler_0f__state0__substate9_hook(gb); return; }
    else if (jt_ == b_+265) { endgameCutsceneHandler_0f__state0__substateA_hook(gb); return; }
    else if (jt_ == b_+270) { endgameCutsceneHandler_0f__state0__substateB_hook(gb); return; }
    else if (jt_ == b_+296) { endgameCutsceneHandler_0f__state0__substateC_hook(gb); return; }
    else if (jt_ == b_+314) { endgameCutsceneHandler_0f__state0__substateD_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_0f__state0__substate0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+51, b_+53); A = 0x01;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+57); SET_HL(wActiveRing);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0xff);
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+62); H8(hActiveObjectType) = A;
  CYC(b_+62, b_+65); SET_DE(w1Link);
  CYC(b_+65, b_+68); SET_BC(0xf8f0);
  CYC(b_+68, b_+70); A = 0x28;
  CALL_C(b_+70, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+73);
  CYC(b_+73, b_+75); A = 0x28;
  CALL_C(b_+75, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+78);
  CYC(b_+78, b_+80); L = 0x4b;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x30);
  CYC(b_+82, b_+83); L = alu_inc8(gb, L);
  CYC(b_+83, b_+84); L = alu_inc8(gb, L);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x78);
  CYC(b_+86, b_+89); SET_HL(wTmpcbb3);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x0a);
  CYC(b_+91, b_+92); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+346, updateStatusBar_hook, SYM(updateStatusBar), b_+349);
  CALL_C(b_+349, endgameCutsceneHandler_0f__state1__runSubstates_hook, b_+355, b_+352);
  CYC(b_+352, b_+355); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_0f__state1__runSubstates_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+355, b_+358); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+358, b_+359); A = mem_rd(gb, DE);
  CYC(b_+359, b_+360); push_effect(gb, b_+360);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+382) { endgameCutsceneHandler_0f__state1__substate0_hook(gb); return; }
    else if (jt_ == b_+428) { endgameCutsceneHandler_0f__state1__substate1_hook(gb); return; }
    else if (jt_ == b_+448) { endgameCutsceneHandler_0f__state1__substate2_hook(gb); return; }
    else if (jt_ == b_+463) { endgameCutsceneHandler_0f__state1__substate3_hook(gb); return; }
    else if (jt_ == b_+490) { endgameCutsceneHandler_0f__state1__substate4_hook(gb); return; }
    else if (jt_ == b_+517) { endgameCutsceneHandler_0f__state1__substate5_hook(gb); return; }
    else if (jt_ == b_+530) { endgameCutsceneHandler_0f__state1__substate6_hook(gb); return; }
    else if (jt_ == b_+557) { endgameCutsceneHandler_0f__state1__substate7_hook(gb); return; }
    else if (jt_ == b_+571) { endgameCutsceneHandler_0f__state1__substate8_hook(gb); return; }
    else if (jt_ == b_+616) { endgameCutsceneHandler_0f__state1__substate9_hook(gb); return; }
    else if (jt_ == b_+636) { endgameCutsceneHandler_0f__state1__substateA_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_0f__state1__substate0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+382, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, SYM(cutscene_setScreenShakeCounterTo4RumbleAt0), b_+385);
  CYC(b_+385, b_+388); A = W8(wPaletteThread_mode);
  CYC(b_+388, b_+389); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+389, b_+390); ret_effect(gb);
    return;
  }
  CYC(b_+389, b_+390);
  CALL_C(b_+390, incCbc2_hook, SYM(incCbc2), b_+393);
  CYC(b_+393, b_+395); A = 0x11;
  CYC(b_+395, b_+398); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CALL_C(b_+398, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+401);
  CYC(b_+401, b_+403); A = 0x04;
  CYC(b_+403, b_+405); B = 0x02;
  CALL_C(b_+405, cutscene_loadAObjectGfxBTimes_andReload_hook, SYM(cutscene_loadAObjectGfxBTimes_andReload), b_+408);
  CYC(b_+408, b_+410); A = 0xf1;
  CALL_C(b_+410, playSound_b00_hook, SYM(playSound_b00), b_+413);
  CYC(b_+413, b_+415); A = 0xfa;
  CALL_C(b_+415, playSound_b00_hook, SYM(playSound_b00), b_+418);
  CYC(b_+418, b_+421); SET_HL(wTmpcbb3);
  CYC(b_+421, b_+423); mem_wr(gb, HL, 0x3c);
  CYC(b_+423, b_+425); A = 0x02;
  CYC(b_+425, b_+428); loadGfxRegisterStateIndex_hook(gb);
}

void endgameCutsceneHandler_09_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wGenericCutscene_cbc1);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == SYM(endgameCutsceneHandler_09_stage0) && hook_enabled_at(gb, SYM(endgameCutsceneHandler_09_stage0))) { endgameCutsceneHandler_09_stage0_hook(gb); return; }
    else if (jt_ == SYM(endgameCutsceneHandler_09_stage1) && hook_enabled_at(gb, SYM(endgameCutsceneHandler_09_stage1))) { endgameCutsceneHandler_09_stage1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_09_stage0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateStatusBar_hook, SYM(updateStatusBar), b_+3);
  CALL_C(b_+3, endgameCutsceneHandler_09_stage0__runStates_hook, b_+9, b_+6);
  CYC(b_+6, b_+9); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_09_stage0__runStates_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+9, b_+12); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+58) { endgameCutsceneHandler_09_stage0__state0_hook(gb); return; }
    else if (jt_ == b_+117) { endgameCutsceneHandler_09_stage0__state1_hook(gb); return; }
    else if (jt_ == b_+130) { endgameCutsceneHandler_09_stage0__state2_hook(gb); return; }
    else if (jt_ == b_+159) { endgameCutsceneHandler_09_stage0__state3_hook(gb); return; }
    else if (jt_ == b_+174) { endgameCutsceneHandler_09_stage0__state4_hook(gb); return; }
    else if (jt_ == b_+187) { endgameCutsceneHandler_09_stage0__state5_hook(gb); return; }
    else if (jt_ == b_+247) { endgameCutsceneHandler_09_stage0__state6_hook(gb); return; }
    else if (jt_ == b_+255) { endgameCutsceneHandler_09_stage0__state7_hook(gb); return; }
    else if (jt_ == b_+279) { endgameCutsceneHandler_09_stage0__state8_hook(gb); return; }
    else if (jt_ == b_+291) { endgameCutsceneHandler_09_stage0__state9_hook(gb); return; }
    else if (jt_ == b_+332) { endgameCutsceneHandler_09_stage0__stateA_hook(gb); return; }
    else if (jt_ == b_+369) { endgameCutsceneHandler_09_stage0__stateB_hook(gb); return; }
    else if (jt_ == b_+386) { endgameCutsceneHandler_09_stage0__stateC_hook(gb); return; }
    else if (jt_ == b_+406) { endgameCutsceneHandler_09_stage0__stateD_hook(gb); return; }
    else if (jt_ == b_+429) { endgameCutsceneHandler_09_stage0__stateE_hook(gb); return; }
    else if (jt_ == b_+460) { endgameCutsceneHandler_09_stage0__stateF_hook(gb); return; }
    else if (jt_ == b_+498) { endgameCutsceneHandler_09_stage0__state10_hook(gb); return; }
    else if (jt_ == b_+527) { endgameCutsceneHandler_09_stage0__state11_hook(gb); return; }
    else if (jt_ == b_+572) { endgameCutsceneHandler_09_stage0__state12_hook(gb); return; }
    else if (jt_ == b_+603) { endgameCutsceneHandler_09_stage0__state13_hook(gb); return; }
    else if (jt_ == b_+621) { endgameCutsceneHandler_09_stage0__state14_hook(gb); return; }
    else if (jt_ == b_+639) { endgameCutsceneHandler_09_stage0__state15_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void endgameCutsceneHandler_09_stage1__runStates_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+6, b_+9); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+31) { endgameCutsceneHandler_09_stage1__state0_hook(gb); return; }
    else if (jt_ == b_+76) { endgameCutsceneHandler_09_stage1__state1_hook(gb); return; }
    else if (jt_ == b_+100) { endgameCutsceneHandler_09_stage1__state2_hook(gb); return; }
    else if (jt_ == b_+129) { endgameCutsceneHandler_09_stage1__state3_hook(gb); return; }
    else if (jt_ == b_+151) { endgameCutsceneHandler_09_stage1__state4_hook(gb); return; }
    else if (jt_ == b_+170) { endgameCutsceneHandler_09_stage1__state5_hook(gb); return; }
    else if (jt_ == b_+198) { endgameCutsceneHandler_09_stage1__state6_hook(gb); return; }
    else if (jt_ == b_+219) { endgameCutsceneHandler_09_stage1__state7_hook(gb); return; }
    else if (jt_ == b_+242) { endgameCutsceneHandler_09_stage1__state8_hook(gb); return; }
    else if (jt_ == b_+328) { endgameCutsceneHandler_09_stage1__state9_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}
