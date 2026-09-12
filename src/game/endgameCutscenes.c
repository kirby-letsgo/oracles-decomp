#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x605c, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x605f);
  if (!(F & FZ)) {
    CYCT(0x605f, 0x6060); ret_effect(gb);
    return;
  }
  CYC(0x605f, 0x6060);
  CALL_C(0x6060, incCbc2_hook, 0x3067, 0x6063);
  CYC(0x6063, 0x6066); SET_HL(wTmpcbb3);
  CYC(0x6066, 0x6067); mem_wr(gb, HL, E);
  CYC(0x6067, 0x6068); ret_effect(gb);
}

void cutscene_decCBB3IfTextNotActive_hook(GB *gb) {
  CYC(0x6068, 0x606b); A = W8(wTextIsActive);
  CYC(0x606b, 0x606c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x606c, 0x606d); ret_effect(gb);
    return;
  }
  CYC(0x606c, 0x606d);
  CYC(0x606d, 0x6070);
  decCbb3_hook(gb);
}

void cutscene_decCBB3IfNotFadingOut_hook(GB *gb) {
  CYC(0x6070, 0x6073); A = W8(wPaletteThread_mode);
  CYC(0x6073, 0x6074); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6074, 0x6075); ret_effect(gb);
    return;
  }
  CYC(0x6074, 0x6075);
  CYC(0x6075, 0x6078);
  decCbb3_hook(gb);
}

void cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6078, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x607b);
  if (!(F & FZ)) {
    CYCT(0x607b, 0x607c); ret_effect(gb);
    return;
  }
  CYC(0x607b, 0x607c);
  CALL_C(0x607c, incCbc1_hook, 0x3062, 0x607f);
  CYC(0x607f, 0x6080); A = E;
  CYC(0x6080, 0x6083); W8(wTmpcbb3) = A;
  CYC(0x6083, 0x6086);
  showText_hook(gb);
}

void cutscene_clearCFC0ToCFDF_hook(GB *gb) {
  CYC(0x608e, 0x6090); B = 0x20;
  CYC(0x6090, 0x6093); SET_HL(0xcfc0);
  CYC(0x6093, 0x6096);
  clearMemory_hook(gb);
}

void cutscene_setScreenShakeCounterTo4RumbleAt0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6096, 0x6098); A = 0x04;
  CALL_C(0x6098, setScreenShakeCounter_hook, 0x24bb, 0x609b);
  cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook(gb);
}

void cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook(GB *gb) {
  CYC(0x609b, 0x609e); A = W8(wFrameCounter);
  CYC(0x609e, 0x60a0); alu_and(gb, 0x0f);
  CYC(0x60a0, 0x60a2); A = 0xb8;
  if (F & FZ) {
    CYCT(0x60a2, 0x60a5);
    playSound_b00_hook(gb);
    return;
  }
  CYC(0x60a2, 0x60a5);
  CYC(0x60a5, 0x60a6); ret_effect(gb);
}

void cutscene_resetOamWithSomething1_hook(GB *gb) {
  CYC(0x60a6, 0x60a9); SET_HL(0x4f73);
  CYC(0x60a9, 0x60ab); E = 0x16;
  CYC(0x60ab, 0x60ae); SET_BC(0x3038);
  CYC(0x60ae, 0x60b0);
  cutscene_resetOamWithData_hook(gb);
}

void cutscene_resetOamWithSomething2_hook(GB *gb) {
  CYC(0x60b0, 0x60b3); SET_HL(0x4e37);
  CYC(0x60b3, 0x60b5); E = 0x16;
  CYC(0x60b5, 0x60b8); SET_BC(0x3038);
  cutscene_resetOamWithData_hook(gb);
}

void cutscene_resetOamWithData_hook(GB *gb) {
  CYC(0x60b8, 0x60b9); alu_xor(gb, A);
  CYC(0x60b9, 0x60bb); hram_wr(gb, 0x9f, A);
  CYC(0x60bb, 0x60be);
  addSpritesFromBankToOam_withOffset_hook(gb);
}

void cutscene_replaceListOfTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60be, 0x60bf); B = mem_rd(gb, HL);
  CYC(0x60bf, 0x60c0); SET_HL(HL + 1);
  for (;;) {
    CYC(0x60c0, 0x60c1); C = mem_rd(gb, HL);
    CYC(0x60c1, 0x60c2); SET_HL(HL + 1);
    CYC(0x60c2, 0x60c3); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x60c3, 0x60c4); push_effect(gb, BC);
    CYC(0x60c4, 0x60c5); push_effect(gb, HL);
    CALL_C(0x60c5, setTile_hook, 0x3a9c, 0x60c8);
    CYC(0x60c8, 0x60c9); SET_HL(pop_effect(gb));
    CYC(0x60c9, 0x60ca); SET_BC(pop_effect(gb));
    CYC(0x60ca, 0x60cb); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x60cb, 0x60cd);
      continue;
    }
    CYC(0x60cb, 0x60cd);
    CYC(0x60cd, 0x60ce); ret_effect(gb);
    return;
  }
}

void func_60e0_hook(GB *gb) {
  CYC(0x60e0, 0x60e3); SET_HL(wLinkHealth);
  CYC(0x60e3, 0x60e5); mem_wr(gb, HL, 0x04);
  CYC(0x60e5, 0x60e7); L = 0x88;
  CYC(0x60e7, 0x60e8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60e8, 0x60e9); B = mem_rd(gb, HL);
  CYC(0x60e9, 0x60ec); SET_HL(0xcde3);
  CYC(0x60ec, 0x60ed); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x60ed, 0x60ee); mem_wr(gb, HL, B);
  CYC(0x60ee, 0x60f1);
  disableActiveRing_hook(gb);
}

void func_60f1_hook(GB *gb) {
  CYC(0x60f1, 0x60f4); SET_HL(wLinkMaxHealth);
  CYC(0x60f4, 0x60f5); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x60f5, 0x60f6); mem_wr(gb, HL, A);
  CYC(0x60f6, 0x60f9); SET_HL(0xcde3);
  CYC(0x60f9, 0x60fa); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60fa, 0x60fb); B = mem_rd(gb, HL);
  CYC(0x60fb, 0x60fe); SET_HL(wInventoryB);
  CYC(0x60fe, 0x60ff); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x60ff, 0x6100); mem_wr(gb, HL, B);
  CYC(0x6100, 0x6104);
  enableActiveRing_hook(gb);
}

void cutscene_parseObjectData_andLoadObjectGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x601a, getEntryFromObjectTable1_hook, 0x3080, 0x601d);
  CALL_C(0x601d, parseGivenObjectData_b00_hook, 0x3171, 0x6020);
  CALL_C(0x6020, refreshObjectGfx_hook, 0x1618, 0x6023);
  CYC(0x6023, 0x6026); cutsceneFunc_6026_hook(gb);
}

void cutsceneFunc_6026_hook(GB *gb) {
  CYC(0x6026, 0x6029); A = mem_rd(gb, 0xcfde);
  CYC(0x6029, 0x602b); alu_cp(gb, 0x00);
  if (F & FZ) {
    CYCT(0x602b, 0x602d);
    cutscene_load_04_ObjectGfx2Times_andReload_hook(gb);
    return;
  }
  CYC(0x602b, 0x602d);
  CYC(0x602d, 0x602f); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x602f, 0x6031);
    cutscene_load_26_ObjectGfx2Times_andReload_hook(gb);
    return;
  }
  CYC(0x602f, 0x6031);
  CYC(0x6031, 0x6033); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x6033, 0x6035);
    cutscene_load_24_ObjectGfx2Times_andReload_hook(gb);
    return;
  }
  CYC(0x6033, 0x6035);
  CYC(0x6035, 0x6037); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(0x6037, 0x6039);
    cutscene_load_26_ObjectGfx2Times_andReload_hook(gb);
    return;
  }
  CYC(0x6037, 0x6039);
  CYC(0x6039, 0x603a); ret_effect(gb);
}

void cutscene_loadAObjectGfxBTimes_hook(GB *gb) {
  CYC(0x603a, 0x603d); SET_HL(wLoadedObjectGfx);
  cutscene_loadAintoHL_BTimes_hook(gb);
}

void cutscene_loadAintoHL_BTimes_hook(GB *gb) {
  for (;;) {
    CYC(0x603d, 0x603e); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x603e, 0x603f); A = alu_inc8(gb, A);
    CYC(0x603f, 0x6041); mem_wr(gb, HL, 0x01);
    CYC(0x6041, 0x6042); L = alu_inc8(gb, L);
    CYC(0x6042, 0x6043); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x6043, 0x6045);
      continue;
    }
    CYC(0x6043, 0x6045);
    CYC(0x6045, 0x6046); ret_effect(gb);
    return;
  }
}

void cutscene_load_24_ObjectGfx2Times_andReload_hook(GB *gb) {
  CYC(0x6046, 0x6048); A = 0x24;
  CYC(0x6048, 0x604a); B = 0x02;
  CYC(0x604a, 0x604c); cutscene_loadAObjectGfxBTimes_andReload_hook(gb);
}

void cutscene_load_26_ObjectGfx2Times_andReload_hook(GB *gb) {
  CYC(0x604c, 0x604e); A = 0x26;
  CYC(0x604e, 0x6050); B = 0x02;
  CYC(0x6050, 0x6052); cutscene_loadAObjectGfxBTimes_andReload_hook(gb);
}

void cutscene_load_04_ObjectGfx2Times_andReload_hook(GB *gb) {
  CYC(0x6052, 0x6054); A = 0x04;
  CYC(0x6054, 0x6056); B = 0x02;
  cutscene_loadAObjectGfxBTimes_andReload_hook(gb);
}

void cutscene_loadAObjectGfxBTimes_andReload_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6056, cutscene_loadAObjectGfxBTimes_hook, 0x603a, 0x6059);
  CYC(0x6059, 0x605c); reloadObjectGfx_b00_hook(gb);
}

void cutscene_clearTmpCBB3_hook(GB *gb) {
  CYC(0x6086, 0x6089); SET_HL(wTmpcbb3);
  CYC(0x6089, 0x608b); B = 0x10;
  CYC(0x608b, 0x608e); clearMemory_hook(gb);
}

void endgameCutsceneHandler_0a__state0__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e2e, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5e31);
  if (!(F & FZ)) {
    CYCT(0x5e31, 0x5e32); ret_effect(gb);
    return;
  }
  CYC(0x5e31, 0x5e32);
  CALL_C(0x5e32, func_60e0_hook, 0x60e0, 0x5e35);
  CALL_C(0x5e35, incCbc2_hook, 0x3067, 0x5e38);
  CALL_C(0x5e38, clearOam_hook, 0x049f, 0x5e3b);
  CYC(0x5e3b, 0x5e3e); SET_HL(wTmpcbb3);
  CYC(0x5e3e, 0x5e40); mem_wr(gb, HL, 0xb4);
  CYC(0x5e40, 0x5e41); SET_HL(HL + 1);
  CYC(0x5e41, 0x5e43); mem_wr(gb, HL, 0x00);
  CYC(0x5e43, 0x5e46); SET_HL(wGfxRegs1_LCDC);
  CYC(0x5e46, 0x5e48); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(0x5e48, 0x5e4a); A = 0x2a;
  CYC(0x5e4a, 0x5e4d); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_0a__state0__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e4d, 0x5e50); SET_HL(wTmpcbb3);
  CALL_C(0x5e50, decHlRef16WithCap_hook, 0x0237, 0x5e53);
  if (!(F & FZ)) {
    CYCT(0x5e53, 0x5e54); ret_effect(gb);
    return;
  }
  CYC(0x5e53, 0x5e54);
  CALL_C(0x5e54, incCbc2_hook, 0x3067, 0x5e57);
  CYC(0x5e57, 0x5e5a); SET_HL(wTmpcbb3);
  CYC(0x5e5a, 0x5e5c); mem_wr(gb, HL, 0x48);
  CYC(0x5e5c, 0x5e5d); SET_HL(HL + 1);
  CYC(0x5e5d, 0x5e5f); mem_wr(gb, HL, 0x03);
  CYC(0x5e5f, 0x5e61); A = 0x04;
  CALL_C(0x5e61, loadPaletteHeader_hook, 0x050b, 0x5e64);
  CYC(0x5e64, 0x5e66); A = 0x06;
  CYC(0x5e66, 0x5e69); fadeinFromBlackWithDelay_hook(gb);
}

void endgameCutsceneHandler_0a__state0__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e69, 0x5e6c); SET_HL(wTmpcbb3);
  CALL_C(0x5e6c, decHlRef16WithCap_hook, 0x0237, 0x5e6f);
  if (!(F & FZ)) {
    CYCT(0x5e6f, 0x5e70); ret_effect(gb);
    return;
  }
  CYC(0x5e6f, 0x5e70);
  CALL_C(0x5e70, incCbc1_hook, 0x3062, 0x5e73);
  CYC(0x5e73, 0x5e74); L = alu_inc8(gb, L);
  CYC(0x5e74, 0x5e75); mem_wr(gb, HL, A);
  CYC(0x5e75, 0x5e77); B = 0x00;
  CALL_C(0x5e77, checkIsLinkedGame_hook, 0x1992, 0x5e7a);
  if (F & FZ) {
    CYCT(0x5e7a, 0x5e7c);
  } else {
    CYC(0x5e7a, 0x5e7c);
    CYC(0x5e7c, 0x5e7e); B = 0x04;
  }
  CYC(0x5e7e, 0x5e81); SET_HL(0xcfde);
  CYC(0x5e81, 0x5e82); mem_wr(gb, HL, B);
  CYC(0x5e82, 0x5e83); L = alu_inc8(gb, L);
  CYC(0x5e83, 0x5e85); mem_wr(gb, HL, 0x00);
  CYC(0x5e85, 0x5e88); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f30, 0x5f33); A = W8(wPaletteThread_mode);
  CYC(0x5f33, 0x5f34); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5f34, 0x5f35); ret_effect(gb);
    return;
  }
  CYC(0x5f34, 0x5f35);
  CYC(0x5f35, 0x5f38); A = mem_rd(gb, 0xcfdf);
  CYC(0x5f38, 0x5f39); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5f39, 0x5f3a); ret_effect(gb);
    return;
  }
  CYC(0x5f39, 0x5f3a);
  CALL_C(0x5f3a, incCbc2_hook, 0x3067, 0x5f3d);
  CYC(0x5f3d, 0x5f3f); A = 0xff;
  CYC(0x5f3f, 0x5f42); mem_wr(gb, wTilesetAnimation, A);
  CYC(0x5f42, 0x5f45); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f45, 0x5f48); A = W8(wPaletteThread_mode);
  CYC(0x5f48, 0x5f49); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5f49, 0x5f4a); ret_effect(gb);
    return;
  }
  CYC(0x5f49, 0x5f4a);
  CALL_C(0x5f4a, incCbc2_hook, 0x3067, 0x5f4d);
  CALL_C(0x5f4d, disableLcd_hook, 0x02c1, 0x5f50);
  CALL_C(0x5f50, clearWramBank1_hook, 0x3597, 0x5f53);
  CYC(0x5f53, 0x5f56); A = mem_rd(gb, 0xcfde);
  CYC(0x5f56, 0x5f57); alu_add(gb, A);
  CYC(0x5f57, 0x5f59); alu_add(gb, 0x86);
  CALL_C(0x5f59, loadGfxHeader_hook, 0x0626, 0x5f5c);
  CYC(0x5f5c, 0x5f5f); SET_HL(wTmpcbb3);
  CYC(0x5f5f, 0x5f61); mem_wr(gb, HL, 0x5a);
  CYC(0x5f61, 0x5f63); A = 0xa1;
  CALL_C(0x5f63, loadPaletteHeader_hook, 0x050b, 0x5f66);
  CYC(0x5f66, 0x5f68); A = 0x04;
  CALL_C(0x5f68, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5f6b);
  CYC(0x5f6b, 0x5f6e); A = mem_rd(gb, 0xcfde);
  CYC(0x5f6e, 0x5f71); SET_HL(0x5f81);
  CYC(0x5f71, 0x5f72); add_a_to_hl_from_rst(gb, 0x5f72);
  CYC(0x5f72, 0x5f73); A = mem_rd(gb, HL);
  CYC(0x5f73, 0x5f76); mem_wr(gb, wGfxRegs1_SCX, A);
  CYC(0x5f76, 0x5f78); A = 0x10;
  CYC(0x5f78, 0x5f7a); hram_wr(gb, 0xac, A);
  CYC(0x5f7a, 0x5f7b); alu_xor(gb, A);
  CYC(0x5f7b, 0x5f7e); mem_wr(gb, 0xcfdf, A);
  CYC(0x5f7e, 0x5f81); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f89, 0x5f8c); A = W8(wPaletteThread_mode);
  CYC(0x5f8c, 0x5f8d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5f8d, 0x5f8e); ret_effect(gb);
    return;
  }
  CYC(0x5f8d, 0x5f8e);
  CALL_C(0x5f8e, decCbb3_hook, 0x305d, 0x5f91);
  if (!(F & FZ)) {
    CYCT(0x5f91, 0x5f92); ret_effect(gb);
    return;
  }
  CYC(0x5f91, 0x5f92);
  CALL_C(0x5f92, incCbc2_hook, 0x3067, 0x5f95);
  CALL_C(0x5f95, getFreeInteractionSlot_hook, 0x3aef, 0x5f98);
  if (!(F & FZ)) {
    CYCT(0x5f98, 0x5f99); ret_effect(gb);
    return;
  }
  CYC(0x5f98, 0x5f99);
  CYC(0x5f99, 0x5f9b); mem_wr(gb, HL, 0xae);
  CYC(0x5f9b, 0x5f9c); L = alu_inc8(gb, L);
  CYC(0x5f9c, 0x5f9f); A = mem_rd(gb, 0xcfde);
  CYC(0x5f9f, 0x5fa0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5fa0, 0x5fa2); mem_wr(gb, HL, 0x00);
  CYC(0x5fa2, 0x5fa3); ret_effect(gb);
}

void endgameCutsceneHandler_0a__state1__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5fa3, 0x5fa6); A = W8(wPaletteThread_mode);
  CYC(0x5fa6, 0x5fa7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5fa7, 0x5fa8); ret_effect(gb);
    return;
  }
  CYC(0x5fa7, 0x5fa8);
  CYC(0x5fa8, 0x5fa9); alu_xor(gb, A);
  CYC(0x5fa9, 0x5fab); hram_wr(gb, 0x9f, A);
  CYC(0x5fab, 0x5fae); A = mem_rd(gb, 0xcfdf);
  CYC(0x5fae, 0x5faf); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5faf, 0x5fb0); ret_effect(gb);
    return;
  }
  CYC(0x5faf, 0x5fb0);
  CYC(0x5fb0, 0x5fb2); B = 0x03;
  CALL_C(0x5fb2, checkIsLinkedGame_hook, 0x1992, 0x5fb5);
  if (F & FZ) {
    CYCT(0x5fb5, 0x5fb7);
  } else {
    CYC(0x5fb5, 0x5fb7);
    CYC(0x5fb7, 0x5fb9); B = 0x07;
  }
  CYC(0x5fb9, 0x5fbc); SET_HL(0xcfde);
  CYC(0x5fbc, 0x5fbd); A = mem_rd(gb, HL);
  CYC(0x5fbd, 0x5fbe); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(0x5fbe, 0x5fc0);
    endgameCutsceneHandler_0a__state1__func_5fc7_hook(gb);
    return;
  }
  CYC(0x5fbe, 0x5fc0);
  CYC(0x5fc0, 0x5fc1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5fc1, 0x5fc2); alu_xor(gb, A);
  CYC(0x5fc2, 0x5fc5); mem_wr(gb, 0xcbc2, A);
  CYC(0x5fc5, 0x5fc7);
  CYC(0x5fd2, 0x5fd5); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state1__func_5fc7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5fc7, cutscene_clearTmpCBB3_hook, 0x6086, 0x5fca);
  CALL_C(0x5fca, cutscene_clearCFC0ToCFDF_hook, 0x608e, 0x5fcd);
  CYC(0x5fcd, 0x5fcf); A = 0x02;
  CYC(0x5fcf, 0x5fd2); mem_wr(gb, 0xcbc1, A);
  CYC(0x5fd2, 0x5fd5); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5bc0, decCbb3_hook, 0x305d, 0x5bc3);
  if (!(F & FZ)) {
    CYCT(0x5bc3, 0x5bc4); ret_effect(gb);
    return;
  }
  CYC(0x5bc3, 0x5bc4);
  CYC(0x5bc4, 0x5bc7); SET_HL(wTmpcbb3);
  CYC(0x5bc7, 0x5bc9); mem_wr(gb, HL, 0x1e);
  CYC(0x5bc9, 0x5bcb); A = 0xf0;
  CALL_C(0x5bcb, playSound_b00_hook, 0x0c98, 0x5bce);
  CYC(0x5bce, 0x5bd1); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5bd1, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5bd4);
  CALL_C(0x5bd4, decCbb3_hook, 0x305d, 0x5bd7);
  if (!(F & FZ)) {
    CYCT(0x5bd7, 0x5bd8); ret_effect(gb);
    return;
  }
  CYC(0x5bd7, 0x5bd8);
  CALL_C(0x5bd8, incCbc2_hook, 0x3067, 0x5bdb);
  CYC(0x5bdb, 0x5bde); SET_HL(wTmpcbb3);
  CYC(0x5bde, 0x5be0); mem_wr(gb, HL, 0x96);
  CYC(0x5be0, 0x5be3); endgameCutsceneHandler_0f__state0__func_5cb0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5be3, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5be6);
  CALL_C(0x5be6, decCbb3_hook, 0x305d, 0x5be9);
  if (!(F & FZ)) {
    CYCT(0x5be9, 0x5bea); ret_effect(gb);
    return;
  }
  CYC(0x5be9, 0x5bea);
  CALL_C(0x5bea, incCbc2_hook, 0x3067, 0x5bed);
  CYC(0x5bed, 0x5bef); A = 0xf1;
  CALL_C(0x5bef, playSound_b00_hook, 0x0c98, 0x5bf2);
  CYC(0x5bf2, 0x5bf5); SET_HL(wTmpcbb3);
  CYC(0x5bf5, 0x5bf7); mem_wr(gb, HL, 0x3c);
  CYC(0x5bf7, 0x5bfa); SET_BC(0x3d0e);
  CYC(0x5bfa, 0x5bfd); showText_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5bfd, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5c00);
  if (!(F & FZ)) {
    CYCT(0x5c00, 0x5c01); ret_effect(gb);
    return;
  }
  CYC(0x5c00, 0x5c01);
  CALL_C(0x5c01, incCbc2_hook, 0x3067, 0x5c04);
  CYC(0x5c04, 0x5c06); A = 0x21;
  CALL_C(0x5c06, playSound_b00_hook, 0x0c98, 0x5c09);
  CYC(0x5c09, 0x5c0c); SET_HL(wTmpcbb3);
  CYC(0x5c0c, 0x5c0e); mem_wr(gb, HL, 0x3c);
  CYC(0x5c0e, 0x5c11); endgameCutsceneHandler_0f__state0__func_5cb0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c11, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5c14);
  CALL_C(0x5c14, decCbb3_hook, 0x305d, 0x5c17);
  if (!(F & FZ)) {
    CYCT(0x5c17, 0x5c18); ret_effect(gb);
    return;
  }
  CYC(0x5c17, 0x5c18);
  CYC(0x5c18, 0x5c1b); SET_HL(wTmpcbb3);
  CYC(0x5c1b, 0x5c1d); mem_wr(gb, HL, 0x5a);
  CYC(0x5c1d, 0x5c20); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c20, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5c23);
  CALL_C(0x5c23, decCbb3_hook, 0x305d, 0x5c26);
  if (!(F & FZ)) {
    CYCT(0x5c26, 0x5c27); ret_effect(gb);
    return;
  }
  CYC(0x5c26, 0x5c27);
  CALL_C(0x5c27, incCbc2_hook, 0x3067, 0x5c2a);
  CYC(0x5c2a, 0x5c2d); SET_HL(wTmpcbb3);
  CYC(0x5c2d, 0x5c2f); mem_wr(gb, HL, 0x3c);
  CYC(0x5c2f, 0x5c31); A = 0xf1;
  CYC(0x5c31, 0x5c34); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c34, decCbb3_hook, 0x305d, 0x5c37);
  if (!(F & FZ)) {
    CYCT(0x5c37, 0x5c38); ret_effect(gb);
    return;
  }
  CYC(0x5c37, 0x5c38);
  CALL_C(0x5c38, incCbc2_hook, 0x3067, 0x5c3b);
  CYC(0x5c3b, 0x5c3e); SET_HL(wTmpcbb3);
  CYC(0x5c3e, 0x5c40); mem_wr(gb, HL, 0x3c);
  CYC(0x5c40, 0x5c43); SET_BC(0x3d0f);
  CYC(0x5c43, 0x5c46); showText_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c46, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5c49);
  if (!(F & FZ)) {
    CYCT(0x5c49, 0x5c4a); ret_effect(gb);
    return;
  }
  CYC(0x5c49, 0x5c4a);
  CALL_C(0x5c4a, incCbc2_hook, 0x3067, 0x5c4d);
  CYC(0x5c4d, 0x5c50); SET_HL(wTmpcbb3);
  CYC(0x5c50, 0x5c52); mem_wr(gb, HL, 0x68);
  CYC(0x5c52, 0x5c53); SET_HL(HL + 1);
  CYC(0x5c53, 0x5c55); mem_wr(gb, HL, 0x01);
  CYC(0x5c55, 0x5c58); endgameCutsceneHandler_0f__state0__func_5cb7_hook(gb);
}

void endgameCutsceneHandler_0f__state0__func_5cb0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5cb0, getFreePartSlot_hook, 0x3e8e, 0x5cb3);
  if (!(F & FZ)) {
    CYCT(0x5cb3, 0x5cb4); ret_effect(gb);
    return;
  }
  CYC(0x5cb3, 0x5cb4);
  CYC(0x5cb4, 0x5cb6); mem_wr(gb, HL, 0x54);
  CYC(0x5cb6, 0x5cb7); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state0__func_5cb7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5cb7, getFreeInteractionSlot_hook, 0x3aef, 0x5cba);
  if (!(F & FZ)) {
    CYCT(0x5cba, 0x5cbb); ret_effect(gb);
    return;
  }
  CYC(0x5cba, 0x5cbb);
  CYC(0x5cbb, 0x5cbd); mem_wr(gb, HL, 0x62);
  CYC(0x5cbd, 0x5cbe); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state0__substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c58, 0x5c5b); SET_HL(wTmpcbb3);
  CALL_C(0x5c5b, decHlRef16WithCap_hook, 0x0237, 0x5c5e);
  if (!(F & FZ)) {
    CYCT(0x5c5e, 0x5c5f); ret_effect(gb);
    return;
  }
  CYC(0x5c5e, 0x5c5f);
  CALL_C(0x5c5f, incCbc2_hook, 0x3067, 0x5c62);
  CYC(0x5c62, 0x5c65); SET_HL(wTmpcbb3);
  CYC(0x5c65, 0x5c67); mem_wr(gb, HL, 0x3c);
  CYC(0x5c67, 0x5c6a); SET_BC(0x0563);
  CYC(0x5c6a, 0x5c6d); showText_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateA_hook(GB *gb) {
  CYC(0x5c6d, 0x5c6f); E = 0x1e;
  CYC(0x5c6f, 0x5c72); cutscene_incCBC2setCBB3whenCBB3is0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c72, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5c75);
  CALL_C(0x5c75, decCbb3_hook, 0x305d, 0x5c78);
  if (!(F & FZ)) {
    CYCT(0x5c78, 0x5c79); ret_effect(gb);
    return;
  }
  CYC(0x5c78, 0x5c79);
  CALL_C(0x5c79, incCbc2_hook, 0x3067, 0x5c7c);
  CALL_C(0x5c7c, endgameCutsceneHandler_0f__state0__func_5cb0_hook, 0x5cb0, 0x5c7f);
  CYC(0x5c7f, 0x5c81); A = 0x8c;
  CYC(0x5c81, 0x5c84); mem_wr(gb, wTmpcbb3, A);
  CYC(0x5c84, 0x5c86); A = 0xff;
  CYC(0x5c86, 0x5c89); SET_BC(0x4478);
  CYC(0x5c89, 0x5c8c); createEnergySwirlGoingOut_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c8c, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5c8f);
  CALL_C(0x5c8f, decCbb3_hook, 0x305d, 0x5c92);
  if (!(F & FZ)) {
    CYCT(0x5c92, 0x5c93); ret_effect(gb);
    return;
  }
  CYC(0x5c92, 0x5c93);
  CALL_C(0x5c93, incCbc2_hook, 0x3067, 0x5c96);
  CYC(0x5c96, 0x5c99); SET_HL(wTmpcbb3);
  CYC(0x5c99, 0x5c9b); mem_wr(gb, HL, 0x3c);
  CYC(0x5c9b, 0x5c9e); endgameCutsceneHandler_0f__state0__func_5cb0_hook(gb);
}

void endgameCutsceneHandler_0f__state0__substateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c9e, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5ca1);
  CALL_C(0x5ca1, decCbb3_hook, 0x305d, 0x5ca4);
  if (!(F & FZ)) {
    CYCT(0x5ca4, 0x5ca5); ret_effect(gb);
    return;
  }
  CYC(0x5ca4, 0x5ca5);
  CALL_C(0x5ca5, incCbc1_hook, 0x3062, 0x5ca8);
  CYC(0x5ca8, 0x5ca9); L = alu_inc8(gb, L);
  CYC(0x5ca9, 0x5caa); alu_xor(gb, A);
  CYC(0x5caa, 0x5cac); mem_wr(gb, HL, A);
  CYC(0x5cac, 0x5cad); A = 0x03;
  CYC(0x5cad, 0x5cb0); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d10, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5d13);
  if (!(F & FZ)) {
    CYCT(0x5d13, 0x5d14); ret_effect(gb);
    return;
  }
  CYC(0x5d13, 0x5d14);
  CALL_C(0x5d14, incCbc2_hook, 0x3067, 0x5d17);
  CYC(0x5d17, 0x5d19); A = 0x3c;
  CYC(0x5d19, 0x5d1c); mem_wr(gb, wTmpcbb3, A);
  CYC(0x5d1c, 0x5d1e); A = 0x64;
  CYC(0x5d1e, 0x5d21); SET_BC(0x4850);
  CYC(0x5d21, 0x5d24); createEnergySwirlGoingIn_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d24, decCbb3_hook, 0x305d, 0x5d27);
  if (!(F & FZ)) {
    CYCT(0x5d27, 0x5d28); ret_effect(gb);
    return;
  }
  CYC(0x5d27, 0x5d28);
  CYC(0x5d28, 0x5d29); alu_xor(gb, A);
  CYC(0x5d29, 0x5d2c); mem_wr(gb, wTmpcbb3, A);
  CYC(0x5d2c, 0x5d2d); A = alu_dec8(gb, A);
  CYC(0x5d2d, 0x5d30); mem_wr(gb, wTmpcbba, A);
  CYC(0x5d30, 0x5d33); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d33, 0x5d36); SET_HL(wTmpcbb3);
  CYC(0x5d36, 0x5d38); B = 0x01;
  CALL_C(0x5d38, flashScreen_hook, 0x2d73, 0x5d3b);
  if (F & FZ) {
    CYCT(0x5d3b, 0x5d3c); ret_effect(gb);
    return;
  }
  CYC(0x5d3b, 0x5d3c);
  CALL_C(0x5d3c, incCbc2_hook, 0x3067, 0x5d3f);
  CYC(0x5d3f, 0x5d42); SET_HL(wTmpcbb3);
  CYC(0x5d42, 0x5d44); mem_wr(gb, HL, 0x3c);
  CYC(0x5d44, 0x5d46); A = 0x01;
  CYC(0x5d46, 0x5d49); mem_wr(gb, 0xcfc0, A);
  CYC(0x5d49, 0x5d4b); A = 0x03;
  CYC(0x5d4b, 0x5d4e); fadeinFromWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d4e, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5d51);
  if (!(F & FZ)) {
    CYCT(0x5d51, 0x5d52); ret_effect(gb);
    return;
  }
  CYC(0x5d51, 0x5d52);
  CALL_C(0x5d52, refreshObjectGfx_hook, 0x1618, 0x5d55);
  CYC(0x5d55, 0x5d57); A = 0x04;
  CYC(0x5d57, 0x5d59); B = 0x02;
  CALL_C(0x5d59, cutscene_loadAObjectGfxBTimes_hook, 0x603a, 0x5d5c);
  CYC(0x5d5c, 0x5d5e); A = 0x29;
  CALL_C(0x5d5e, playSound_b00_hook, 0x0c98, 0x5d61);
  CYC(0x5d61, 0x5d64); SET_HL(wTmpcbb3);
  CYC(0x5d64, 0x5d66); mem_wr(gb, HL, 0x3c);
  CYC(0x5d66, 0x5d69); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d69, decCbb3_hook, 0x305d, 0x5d6c);
  if (!(F & FZ)) {
    CYCT(0x5d6c, 0x5d6d); ret_effect(gb);
    return;
  }
  CYC(0x5d6c, 0x5d6d);
  CALL_C(0x5d6d, incCbc2_hook, 0x3067, 0x5d70);
  CYC(0x5d70, 0x5d73); SET_HL(wTmpcbb3);
  CYC(0x5d73, 0x5d75); mem_wr(gb, HL, 0x1e);
  CYC(0x5d75, 0x5d76); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state1__substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d76, decCbb3_hook, 0x305d, 0x5d79);
  if (!(F & FZ)) {
    CYCT(0x5d79, 0x5d7a); ret_effect(gb);
    return;
  }
  CYC(0x5d79, 0x5d7a);
  CALL_C(0x5d7a, refreshObjectGfx_hook, 0x1618, 0x5d7d);
  CYC(0x5d7d, 0x5d7f); A = 0x04;
  CYC(0x5d7f, 0x5d81); B = 0x02;
  CALL_C(0x5d81, cutscene_loadAObjectGfxBTimes_hook, 0x603a, 0x5d84);
  CYC(0x5d84, 0x5d87); SET_HL(wTmpcbb3);
  CYC(0x5d87, 0x5d89); mem_wr(gb, HL, 0x3c);
  CYC(0x5d89, 0x5d8c); SET_HL(0xcfc0);
  CYC(0x5d8c, 0x5d8e); mem_wr(gb, HL, 0x02);
  CYC(0x5d8e, 0x5d91); incCbc2_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d91, 0x5d94); A = mem_rd(gb, 0xcfc0);
  CYC(0x5d94, 0x5d96); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(0x5d96, 0x5d97); ret_effect(gb);
    return;
  }
  CYC(0x5d96, 0x5d97);
  CALL_C(0x5d97, incCbc2_hook, 0x3067, 0x5d9a);
  CYC(0x5d9a, 0x5d9c); A = 0x03;
  CYC(0x5d9c, 0x5d9f); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d9f, 0x5da2); A = W8(wPaletteThread_mode);
  CYC(0x5da2, 0x5da3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5da3, 0x5da4); ret_effect(gb);
    return;
  }
  CYC(0x5da3, 0x5da4);
  CALL_C(0x5da4, incCbc2_hook, 0x3067, 0x5da7);
  CALL_C(0x5da7, disableLcd_hook, 0x02c1, 0x5daa);
  CALL_C(0x5daa, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x5dad);
  CALL_C(0x5dad, hideStatusBar_hook, 0x1aa0, 0x5db0);
  CYC(0x5db0, 0x5db2); A = 0x3c;
  CALL_C(0x5db2, loadGfxHeader_hook, 0x0626, 0x5db5);
  CYC(0x5db5, 0x5db7); A = 0xc9;
  CALL_C(0x5db7, loadPaletteHeader_hook, 0x050b, 0x5dba);
  CYC(0x5dba, 0x5dbd); SET_HL(wTmpcbb3);
  CYC(0x5dbd, 0x5dbf); mem_wr(gb, HL, 0xf0);
  CYC(0x5dbf, 0x5dc1); A = 0x04;
  CALL_C(0x5dc1, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5dc4);
  CALL_C(0x5dc4, cutscene_resetOamWithSomething1_hook, 0x60a6, 0x5dc7);
  CYC(0x5dc7, 0x5dc9); A = 0x03;
  CYC(0x5dc9, 0x5dcc); fadeinFromWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5dcc, cutscene_resetOamWithSomething1_hook, 0x60a6, 0x5dcf);
  CALL_C(0x5dcf, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5dd2);
  if (!(F & FZ)) {
    CYCT(0x5dd2, 0x5dd3); ret_effect(gb);
    return;
  }
  CYC(0x5dd2, 0x5dd3);
  CALL_C(0x5dd3, incCbc2_hook, 0x3067, 0x5dd6);
  CYC(0x5dd6, 0x5dd9); SET_HL(wTmpcbb3);
  CYC(0x5dd9, 0x5ddb); mem_wr(gb, HL, 0x10);
  CYC(0x5ddb, 0x5ddd); A = 0x03;
  CYC(0x5ddd, 0x5de0); fadeoutToBlackWithDelay_hook(gb);
}

void endgameCutsceneHandler_0f__state1__substateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5de0, cutscene_resetOamWithSomething1_hook, 0x60a6, 0x5de3);
  CALL_C(0x5de3, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5de6);
  if (!(F & FZ)) {
    CYCT(0x5de6, 0x5de7); ret_effect(gb);
    return;
  }
  CYC(0x5de6, 0x5de7);
  CYC(0x5de7, 0x5de9); A = 0x0a;
  CYC(0x5de9, 0x5dec); mem_wr(gb, 0xc2ef, A);
  CALL_C(0x5dec, cutscene_clearTmpCBB3_hook, 0x6086, 0x5def);
  CYC(0x5def, 0x5df2); SET_HL(wRoomLayout);
  CYC(0x5df2, 0x5df5); SET_BC(0x00c0);
  CALL_C(0x5df5, clearMemoryBc_hook, 0x0475, 0x5df8);
  CYC(0x5df8, 0x5dfb); SET_HL(wRoomCollisions);
  CYC(0x5dfb, 0x5dfe); SET_BC(0x00c0);
  CALL_C(0x5dfe, clearMemoryBc_hook, 0x0475, 0x5e01);
  CYC(0x5e01, 0x5e02); alu_xor(gb, A);
  CYC(0x5e02, 0x5e04); hram_wr(gb, 0xaa, A);
  CYC(0x5e04, 0x5e06); hram_wr(gb, 0xac, A);
  CYC(0x5e06, 0x5e09); SET_HL(wTmpcbb3);
  CYC(0x5e09, 0x5e0b); mem_wr(gb, HL, 0x3c);
  CYC(0x5e0b, 0x5e0d); A = 0xfb;
  CYC(0x5e0d, 0x5e10); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e10, endgameCutsceneHandler_0a__runStates_hook, 0x5e16, 0x5e13);
  CYC(0x5e13, 0x5e16); func_3539_hook(gb);
}

void endgameCutsceneHandler_0a__runStates_hook(GB *gb) {
  CYC(0x5e16, 0x5e19); SET_DE(0xcbc1);
  CYC(0x5e19, 0x5e1a); A = mem_rd(gb, DE);
  CYC(0x5e1a, 0x5e1b); push_effect(gb, 0x5e1b);
  switch (endgame_jump_table(gb)) {
    case 0x5e23: endgameCutsceneHandler_0a__state0_hook(gb); return;
    case 0x5e88: endgameCutsceneHandler_0a__state1_hook(gb); return;
    case 0x5fd5: endgameCutsceneHandler_0a__state2_hook(gb); return;
    case 0x5fdd: endgameCutsceneHandler_0a__state3_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_0a__state0_hook(GB *gb) {
  CYC(0x5e23, 0x5e26); SET_DE(0xcbc2);
  CYC(0x5e26, 0x5e27); A = mem_rd(gb, DE);
  CYC(0x5e27, 0x5e28); push_effect(gb, 0x5e28);
  switch (endgame_jump_table(gb)) {
    case 0x5e2e: endgameCutsceneHandler_0a__state0__substate0_hook(gb); return;
    case 0x5e4d: endgameCutsceneHandler_0a__state0__substate1_hook(gb); return;
    case 0x5e69: endgameCutsceneHandler_0a__state0__substate2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_0a__state1_hook(GB *gb) {
  CYC(0x5e88, 0x5e8b); SET_DE(0xcbc2);
  CYC(0x5e8b, 0x5e8c); A = mem_rd(gb, DE);
  CYC(0x5e8c, 0x5e8d); push_effect(gb, 0x5e8d);
  switch (endgame_jump_table(gb)) {
    case 0x5e97: endgameCutsceneHandler_0a__state1__substate0_hook(gb); return;
    case 0x5f30: endgameCutsceneHandler_0a__state1__substate1_hook(gb); return;
    case 0x5f45: endgameCutsceneHandler_0a__state1__substate2_hook(gb); return;
    case 0x5f89: endgameCutsceneHandler_0a__state1__substate3_hook(gb); return;
    case 0x5fa3: endgameCutsceneHandler_0a__state1__substate4_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_0a__state1__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e97, 0x5e98); alu_xor(gb, A);
  CYC(0x5e98, 0x5e9a); hram_wr(gb, 0x9f, A);
  CYC(0x5e9a, 0x5e9d); A = W8(wPaletteThread_mode);
  CYC(0x5e9d, 0x5e9e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5e9e, 0x5e9f); ret_effect(gb);
    return;
  }
  CYC(0x5e9e, 0x5e9f);
  CALL_C(0x5e9f, disableLcd_hook, 0x02c1, 0x5ea2);
  CALL_C(0x5ea2, incCbc2_hook, 0x3067, 0x5ea5);
  CALL_C(0x5ea5, clearDynamicInteractions_hook, 0x35d2, 0x5ea8);
  CALL_C(0x5ea8, clearOam_hook, 0x049f, 0x5eab);
  CYC(0x5eab, 0x5ead); A = 0x10;
  CYC(0x5ead, 0x5eaf); hram_wr(gb, 0x9f, A);
  CYC(0x5eaf, 0x5eb2); A = mem_rd(gb, 0xcfde);
  CYC(0x5eb2, 0x5eb3); C = A;
  CALL_C(0x5eb3, cutscene_clearCFC0ToCFDF_hook, 0x608e, 0x5eb6);
  CYC(0x5eb6, 0x5eb7); A = C;
  CYC(0x5eb7, 0x5eba); mem_wr(gb, 0xcfde, A);
  CYC(0x5eba, 0x5ebc); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(0x5ebc, 0x5ebe);
    goto load_headers;
  }
  CYC(0x5ebc, 0x5ebe);
  CYC(0x5ebe, 0x5ec1); SET_HL(0x5f1c);
  CYC(0x5ec1, 0x5ec2); add_double_index_to_hl_from_rst(gb, 0x5ec2);
  CYC(0x5ec2, 0x5ec3); B = mem_rd(gb, HL);
  CYC(0x5ec3, 0x5ec4); SET_HL(HL + 1);
  CYC(0x5ec4, 0x5ec5); C = mem_rd(gb, HL);
  CYC(0x5ec5, 0x5ec7); A = 0x00;
  CALL_C(0x5ec7, forceLoadRoom_hook, 0x36f6, 0x5eca);
  CYC(0x5eca, 0x5ecd); A = mem_rd(gb, 0xcfde);
  CYC(0x5ecd, 0x5ed0); SET_HL(0x5f24);
  CYC(0x5ed0, 0x5ed1); add_a_to_hl_from_rst(gb, 0x5ed1);
  CYC(0x5ed1, 0x5ed2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x5ed2, loadUncompressedGfxHeader_hook, 0x05da, 0x5ed5);
load_headers:
  CYC(0x5ed5, 0x5ed8); A = mem_rd(gb, 0xcfde);
  CYC(0x5ed8, 0x5ed9); alu_add(gb, A);
  CYC(0x5ed9, 0x5edb); alu_add(gb, 0x85);
  CALL_C(0x5edb, loadGfxHeader_hook, 0x0626, 0x5ede);
  CYC(0x5ede, 0x5ee0); A = 0x0f;
  CALL_C(0x5ee0, loadPaletteHeader_hook, 0x050b, 0x5ee3);
  CYC(0x5ee3, 0x5ee6); A = mem_rd(gb, 0xcfde);
  CYC(0x5ee6, 0x5ee8); B = 0xff;
  CYC(0x5ee8, 0x5ee9); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5ee9, 0x5eeb);
    goto load_animation;
  }
  CYC(0x5ee9, 0x5eeb);
  CYC(0x5eeb, 0x5eed); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(0x5eed, 0x5eef);
    goto load_animation;
  }
  CYC(0x5eed, 0x5eef);
  CYC(0x5eef, 0x5ef1); B = 0x01;
load_animation:
  CYC(0x5ef1, 0x5ef2); C = A;
  CYC(0x5ef2, 0x5ef3); A = B;
  CYC(0x5ef3, 0x5ef6); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(0x5ef6, loadAnimationData_hook, 0x3659, 0x5ef9);
  CYC(0x5ef9, 0x5efa); A = C;
  CYC(0x5efa, 0x5efd); SET_HL(0x5f28);
  CYC(0x5efd, 0x5efe); add_a_to_hl_from_rst(gb, 0x5efe);
  CYC(0x5efe, 0x5eff); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x5eff, loadPaletteHeader_hook, 0x050b, 0x5f02);
  CALL_C(0x5f02, reloadObjectGfx_b00_hook, 0x1630, 0x5f05);
  CYC(0x5f05, 0x5f07); A = 0x01;
  CYC(0x5f07, 0x5f0a); mem_wr(gb, wScrollMode, A);
  CYC(0x5f0a, 0x5f0b); alu_xor(gb, A);
  CYC(0x5f0b, 0x5f0d); hram_wr(gb, 0xac, A);
  CYC(0x5f0d, 0x5f10); SET_HL(0xcfde);
  CYC(0x5f10, 0x5f11); B = mem_rd(gb, HL);
  CALL_C(0x5f11, cutscene_parseObjectData_andLoadObjectGfx_hook, 0x601a, 0x5f14);
  CYC(0x5f14, 0x5f16); A = 0x04;
  CALL_C(0x5f16, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5f19);
  CYC(0x5f19, 0x5f1c); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_0a__state2_hook(GB *gb) {
  CYC(0x5fd5, 0x5fd8); SET_HL(0x70f6);
  CYC(0x5fd8, 0x5fda); E = 0x10;
  CYC(0x5fda, 0x5fdd); interBankCall_hook(gb);
}

void endgameCutsceneHandler_0a__state3_hook(GB *gb) {
  CYC(0x5fdd, 0x5fe0); SET_HL(0x7298);
  CYC(0x5fe0, 0x5fe2); E = 0x10;
  CYC(0x5fe2, 0x5fe5); interBankCall_hook(gb);
}

void endgameCutsceneHandler_09_stage0__updateExplosionSoundsAndScreenShake_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x54de, 0x54e1); SET_HL(wTmpcbb6);
  CYC(0x54e1, 0x54e2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x54e2, 0x54e3); ret_effect(gb);
    return;
  }
  CYC(0x54e2, 0x54e3);
  CYC(0x54e3, 0x54e5); mem_wr(gb, HL, 0x10);
  CYC(0x54e5, 0x54e7); A = 0x6f;
  CALL_C(0x54e7, playSound_b00_hook, 0x0c98, 0x54ea);
  CYC(0x54ea, 0x54ec); A = 0x08;
  CALL_C(0x54ec, setScreenShakeCounter_hook, 0x24bb, 0x54ef);
  CYC(0x54ef, 0x54f0); alu_xor(gb, A);
  CYC(0x54f0, 0x54f1); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__explosions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5536, endgameCutsceneHandler_09_stage0__updateExplosionSoundsAndScreenShake_hook, 0x54de, 0x5539);
  if (!(F & FZ)) {
    CYCT(0x5539, 0x553a); ret_effect(gb);
    return;
  }
  CYC(0x5539, 0x553a);
  CALL_C(0x553a, getFreeInteractionSlot_hook, 0x3aef, 0x553d);
  if (!(F & FZ)) {
    CYCT(0x553d, 0x553e); ret_effect(gb);
    return;
  }
  CYC(0x553d, 0x553e);
  CYC(0x553e, 0x5540); mem_wr(gb, HL, 0x99);
  CYC(0x5540, 0x5541); L = alu_inc8(gb, L);
  CYC(0x5541, 0x5542); L = alu_inc8(gb, L);
  CYC(0x5542, 0x5543); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5543, 0x5545); A = 0x01;
  CYC(0x5545, 0x5548); mem_wr(gb, 0xcfd0, A);
  CYC(0x5548, 0x5549); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x548c, 0x548f); A = W8(wPaletteThread_mode);
  CYC(0x548f, 0x5490); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5490, 0x5491); ret_effect(gb);
    return;
  }
  CYC(0x5490, 0x5491);
  CALL_C(0x5491, cutscene_clearCFC0ToCFDF_hook, 0x608e, 0x5494);
  CALL_C(0x5494, incCbc2_hook, 0x3067, 0x5497);
  CYC(0x5497, 0x549a); SET_BC(0x0176);
  CALL_C(0x549a, disableLcdAndLoadRoom_hook, 0x30b0, 0x549d);
  CALL_C(0x549d, resetCamera_hook, 0x12ce, 0x54a0);
  CYC(0x54a0, 0x54a2); A = 0xfa;
  CALL_C(0x54a2, playSound_b00_hook, 0x0c98, 0x54a5);
  CALL_C(0x54a5, clearAllParentItems_hook, 0x2c10, 0x54a8);
  CALL_C(0x54a8, dropLinkHeldItem_hook, 0x2c43, 0x54ab);
  CYC(0x54ab, 0x54ae); SET_HL(0x540c);
  CALL_C(0x54ae, parseGivenObjectData_b00_hook, 0x3171, 0x54b1);
  CYC(0x54b1, 0x54b4); SET_HL(wTmpcbb3);
  CYC(0x54b4, 0x54b6); mem_wr(gb, HL, 0x3c);
  CYC(0x54b6, 0x54b9); SET_HL(0x60ce);
  CALL_C(0x54b9, cutscene_replaceListOfTiles_hook, 0x60be, 0x54bc);
  CALL_C(0x54bc, refreshObjectGfx_hook, 0x1618, 0x54bf);
  CYC(0x54bf, 0x54c1); A = 0x02;
  CALL_C(0x54c1, loadGfxRegisterStateIndex_hook, 0x02ea, 0x54c4);
  CYC(0x54c4, 0x54c7); fadeinFromWhiteToRoom_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54c7, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x54ca);
  if (!(F & FZ)) {
    CYCT(0x54ca, 0x54cb); ret_effect(gb);
    return;
  }
  CYC(0x54ca, 0x54cb);
  CYC(0x54cb, 0x54cd); mem_wr(gb, HL, 0x78);
  CYC(0x54cd, 0x54cf); L = 0xb6;
  CYC(0x54cf, 0x54d1); mem_wr(gb, HL, 0x10);
  CYC(0x54d1, 0x54d4); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54d4, decCbb3_hook, 0x305d, 0x54d7);
  if (!(F & FZ)) {
    CYCT(0x54d7, 0x54d9);
    endgameCutsceneHandler_09_stage0__updateExplosionSoundsAndScreenShake_hook(gb);
    return;
  }
  CYC(0x54d7, 0x54d9);
  CYC(0x54d9, 0x54db); mem_wr(gb, HL, 0x3c);
  CYC(0x54db, 0x54de); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54f1, decCbb3_hook, 0x305d, 0x54f4);
  if (!(F & FZ)) {
    CYCT(0x54f4, 0x54f5); ret_effect(gb);
    return;
  }
  CYC(0x54f4, 0x54f5);
  CYC(0x54f5, 0x54f7); mem_wr(gb, HL, 0x1e);
  CYC(0x54f7, 0x54fa); SET_BC(0x1d0a);
  CALL_C(0x54fa, showText_hook, 0x1872, 0x54fd);
  CYC(0x54fd, 0x5500); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5500, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5503);
  if (!(F & FZ)) {
    CYCT(0x5503, 0x5504); ret_effect(gb);
    return;
  }
  CYC(0x5503, 0x5504);
  CYC(0x5504, 0x5506); mem_wr(gb, HL, 0x78);
  CYC(0x5506, 0x5508); L = 0xb6;
  CYC(0x5508, 0x550a); mem_wr(gb, HL, 0x10);
  CYC(0x550a, 0x550d); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x550d, decCbb3_hook, 0x305d, 0x5510);
  if (!(F & FZ)) {
    CYCT(0x5510, 0x5512);
    endgameCutsceneHandler_09_stage0__explosions_hook(gb);
    return;
  }
  CYC(0x5510, 0x5512);
  CYC(0x5512, 0x5514); mem_wr(gb, HL, 0x28);
  CALL_C(0x5514, incCbc2_hook, 0x3067, 0x5517);
  CYC(0x5517, 0x551a); SET_HL(w1Link);
  CYC(0x551a, 0x551c); mem_wr(gb, HL, 0x03);
  CYC(0x551c, 0x551e); L = 0x0b;
  CYC(0x551e, 0x5520); mem_wr(gb, HL, 0x48);
  CYC(0x5520, 0x5522); L = 0x0d;
  CYC(0x5522, 0x5524); mem_wr(gb, HL, 0x50);
  CYC(0x5524, 0x5526); L = 0x08;
  CYC(0x5526, 0x5528); mem_wr(gb, HL, 0x02);
  CYC(0x5528, 0x552b); SET_HL(0x70d2);
  CYC(0x552b, 0x552d); A = 0x10;
  CALL_C(0x552d, setSimulatedInputAddress_hook, 0x2a1d, 0x5530);
  CYC(0x5530, 0x5533); SET_HL(0x60d7);
  CYC(0x5533, 0x5536); cutscene_replaceListOfTiles_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5549, decCbb3_hook, 0x305d, 0x554c);
  if (!(F & FZ)) {
    CYCT(0x554c, 0x554e);
    endgameCutsceneHandler_09_stage0__explosions_hook(gb);
    return;
  }
  CYC(0x554c, 0x554e); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5551, 0x5554); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5554, 0x5556); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x5556, 0x5557); ret_effect(gb);
    return;
  }
  CYC(0x5556, 0x5557);
  CALL_C(0x5557, incCbc2_hook, 0x3067, 0x555a);
  CYC(0x555a, 0x555b); alu_xor(gb, A);
  CYC(0x555b, 0x555e); mem_wr(gb, wDisabledObjects, A);
  CYC(0x555e, 0x5561); mem_wr(gb, wScrollMode, A);
  CYC(0x5561, 0x5564); SET_HL(0x70d5);
  CYC(0x5564, 0x5566); A = 0x10;
  CYC(0x5566, 0x5569); setSimulatedInputAddress_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5569, 0x556c); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x556c, 0x556e); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(0x556e, 0x556f); ret_effect(gb);
    return;
  }
  CYC(0x556e, 0x556f);
  CALL_C(0x556f, incCbc2_hook, 0x3067, 0x5572);
  CYC(0x5572, 0x5575); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5575, 0x5578); A = W8(wPaletteThread_mode);
  CYC(0x5578, 0x5579); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5579, 0x557a); ret_effect(gb);
    return;
  }
  CYC(0x5579, 0x557a);
  CALL_C(0x557a, incCbc2_hook, 0x3067, 0x557d);
  CYC(0x557d, 0x5580); SET_BC(0x0165);
  CALL_C(0x5580, disableLcdAndLoadRoom_hook, 0x30b0, 0x5583);
  CALL_C(0x5583, resetCamera_hook, 0x12ce, 0x5586);
  CYC(0x5586, 0x5588); A = 0x21;
  CALL_C(0x5588, playSound_b00_hook, 0x0c98, 0x558b);
  CYC(0x558b, 0x558d); A = 0x02;
  CALL_C(0x558d, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5590);
  CYC(0x5590, 0x5593); SET_HL(0x5416);
  CALL_C(0x5593, parseGivenObjectData_b00_hook, 0x3171, 0x5596);
  CYC(0x5596, 0x5599); SET_HL(wTmpcbb3);
  CYC(0x5599, 0x559b); mem_wr(gb, HL, 0x1e);
  CYC(0x559b, 0x559e); fadeinFromWhiteToRoom_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x559e, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x55a1);
  if (!(F & FZ)) {
    CYCT(0x55a1, 0x55a2); ret_effect(gb);
    return;
  }
  CYC(0x55a1, 0x55a2);
  CALL_C(0x55a2, incCbc2_hook, 0x3067, 0x55a5);
  CYC(0x55a5, 0x55a8); SET_HL(w1Link);
  CYC(0x55a8, 0x55aa); mem_wr(gb, HL, 0x03);
  CYC(0x55aa, 0x55ac); L = 0x0b;
  CYC(0x55ac, 0x55ae); mem_wr(gb, HL, 0x88);
  CYC(0x55ae, 0x55b0); L = 0x0d;
  CYC(0x55b0, 0x55b2); mem_wr(gb, HL, 0x50);
  CYC(0x55b2, 0x55b4); L = 0x08;
  CYC(0x55b4, 0x55b6); mem_wr(gb, HL, 0x00);
  CYC(0x55b6, 0x55b9); SET_HL(0x70dd);
  CYC(0x55b9, 0x55bb); A = 0x10;
  CALL_C(0x55bb, setSimulatedInputAddress_hook, 0x2a1d, 0x55be);
  CYC(0x55be, 0x55bf); alu_xor(gb, A);
  CYC(0x55bf, 0x55c2); mem_wr(gb, wScrollMode, A);
  CYC(0x55c2, 0x55c3); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55c3, 0x55c6); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x55c6, 0x55c8); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x55c8, 0x55c9); ret_effect(gb);
    return;
  }
  CYC(0x55c8, 0x55c9);
  CALL_C(0x55c9, incCbc2_hook, 0x3067, 0x55cc);
  CYC(0x55cc, 0x55cf); SET_HL(0x70ee);
  CYC(0x55cf, 0x55d1); A = 0x10;
  CYC(0x55d1, 0x55d4); setSimulatedInputAddress_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55d4, 0x55d7); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x55d7, 0x55d9); alu_cp(gb, 0x0a);
  if (!(F & FZ)) {
    CYCT(0x55d9, 0x55da); ret_effect(gb);
    return;
  }
  CYC(0x55d9, 0x55da);
  CALL_C(0x55da, incCbc2_hook, 0x3067, 0x55dd);
  CYC(0x55dd, 0x55e0); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(0x55e0, 0x55e2); mem_wr(gb, HL, 0x08);
  CYC(0x55e2, 0x55e3); L = alu_inc8(gb, L);
  CYC(0x55e3, 0x55e5); mem_wr(gb, HL, 0x00);
  CYC(0x55e5, 0x55e8); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55e8, 0x55eb); A = W8(wPaletteThread_mode);
  CYC(0x55eb, 0x55ec); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55ec, 0x55ed); ret_effect(gb);
    return;
  }
  CYC(0x55ec, 0x55ed);
  CALL_C(0x55ed, incCbc2_hook, 0x3067, 0x55f0);
  CALL_C(0x55f0, cutscene_loadRoomObjectSetAndFadein, 0x64c5, 0x55f3);
  CYC(0x55f3, 0x55f4); alu_xor(gb, A);
  CYC(0x55f4, 0x55f7); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(0x55f7, 0x55fa); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(0x55fa, 0x55fc); A = 0x02;
  CYC(0x55fc, 0x55ff); loadGfxRegisterStateIndex_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55ff, 0x5602); A = W8(wPaletteThread_mode);
  CYC(0x5602, 0x5603); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5603, 0x5604); ret_effect(gb);
    return;
  }
  CYC(0x5603, 0x5604);
  CYC(0x5604, 0x5607); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x5607, 0x5608); A = mem_rd(gb, HL);
  CYC(0x5608, 0x560a); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x560a, 0x560b); ret_effect(gb);
    return;
  }
  CYC(0x560a, 0x560b);
  CYC(0x560b, 0x560c); alu_xor(gb, A);
  CYC(0x560c, 0x560d); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x560d, 0x560e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x560e, 0x560f); A = mem_rd(gb, HL);
  CYC(0x560f, 0x5611); alu_cp(gb, 0x0a);
  CYC(0x5611, 0x5613); A = 0x0d;
  if (!(F & FZ)) {
    CYCT(0x5613, 0x5615);
  } else {
    CYC(0x5613, 0x5615);
    CYC(0x5615, 0x5617); A = 0x0f;
  }
  CYC(0x5617, 0x561a); SET_HL(wGenericCutscene_cbc2);
  CYC(0x561a, 0x561b); mem_wr(gb, HL, A);
  CYC(0x561b, 0x561e); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x561e, 0x5621); A = W8(wPaletteThread_mode);
  CYC(0x5621, 0x5622); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5622, 0x5623); ret_effect(gb);
    return;
  }
  CYC(0x5622, 0x5623);
  CALL_C(0x5623, incCbc2_hook, 0x3067, 0x5626);
  CALL_C(0x5626, cutscene_loadRoomObjectSetAndFadein, 0x64c5, 0x5629);
  CYC(0x5629, 0x562c); SET_HL(w1Link);
  CYC(0x562c, 0x562e); mem_wr(gb, HL, 0x03);
  CYC(0x562e, 0x5630); L = 0x0b;
  CYC(0x5630, 0x5632); mem_wr(gb, HL, 0x48);
  CYC(0x5632, 0x5634); L = 0x0d;
  CYC(0x5634, 0x5636); mem_wr(gb, HL, 0x60);
  CYC(0x5636, 0x5638); L = 0x08;
  CYC(0x5638, 0x563a); mem_wr(gb, HL, 0x00);
  CYC(0x563a, 0x563c); A = 0x0b;
  CYC(0x563c, 0x563f); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(0x563f, 0x5641); A = 0x02;
  CYC(0x5641, 0x5644); loadGfxRegisterStateIndex_hook(gb);
}

static void endgameCutsceneHandler_09_stage0__state10_linked(GB *gb) {
  CYC(0x5655, 0x5658); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5658, 0x565a); alu_cp(gb, 0x12);
  if (!(F & FZ)) {
    CYCT(0x565a, 0x565b); ret_effect(gb);
    return;
  }
  CYC(0x565a, 0x565b);
  CYC(0x565b, 0x565e); SET_HL(wGenericCutscene_cbc2);
  CYC(0x565e, 0x5660); mem_wr(gb, HL, 0x14);
  CYC(0x5660, 0x5661); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage0__state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5644, checkIsLinkedGame_hook, 0x1992, 0x5647);
  if (!(F & FZ)) {
    CYCT(0x5647, 0x5649);
    endgameCutsceneHandler_09_stage0__state10_linked(gb);
    return;
  }
  CYC(0x5647, 0x5649);
  CYC(0x5649, 0x564c); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x564c, 0x564e); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x564e, 0x564f); ret_effect(gb);
    return;
  }
  CYC(0x564e, 0x564f);
  CALL_C(0x564f, incCbc2_hook, 0x3067, 0x5652);
  CYC(0x5652, 0x5655); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5661, 0x5664); A = W8(wPaletteThread_mode);
  CYC(0x5664, 0x5665); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5665, 0x5666); ret_effect(gb);
    return;
  }
  CYC(0x5665, 0x5666);
  CALL_C(0x5666, incCbc2_hook, 0x3067, 0x5669);
  CYC(0x5669, 0x566c); SET_HL(wTmpcbb3);
  CYC(0x566c, 0x566e); mem_wr(gb, HL, 0x3c);
  CYC(0x566e, 0x5670); A = 0xff;
  CYC(0x5670, 0x5673); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(0x5673, disableLcd_hook, 0x02c1, 0x5676);
  CYC(0x5676, 0x5678); A = 0x2b;
  CALL_C(0x5678, loadGfxHeader_hook, 0x0626, 0x567b);
  CYC(0x567b, 0x567d); A = 0x9d;
  CALL_C(0x567d, loadPaletteHeader_hook, 0x050b, 0x5680);
  CALL_C(0x5680, cutscene_clearObjects_hook, 0x5403, 0x5683);
  CALL_C(0x5683, cutscene_resetOamWithSomething2_hook, 0x60b0, 0x5686);
  CYC(0x5686, 0x5688); A = 0x04;
  CALL_C(0x5688, loadGfxRegisterStateIndex_hook, 0x02ea, 0x568b);
  CYC(0x568b, 0x568e); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene_hook(GB *gb) {
  CYC(0x56a5, 0x56a7); A = 0x01;
  CYC(0x56a7, 0x56aa); mem_wr(gb, wTextboxFlags, A);
  CYC(0x56aa, 0x56ad); showText_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x568e, cutscene_resetOamWithSomething2_hook, 0x60b0, 0x5691);
  CALL_C(0x5691, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5694);
  if (!(F & FZ)) {
    CYCT(0x5694, 0x5695); ret_effect(gb);
    return;
  }
  CYC(0x5694, 0x5695);
  CALL_C(0x5695, incCbc2_hook, 0x3067, 0x5698);
  CYC(0x5698, 0x569b); SET_HL(wMenuDisabled);
  CYC(0x569b, 0x569d); mem_wr(gb, HL, 0x01);
  CYC(0x569d, 0x56a0); SET_HL(wTmpcbb3);
  CYC(0x56a0, 0x56a2); mem_wr(gb, HL, 0x3c);
  CYC(0x56a2, 0x56a5); SET_BC(0x1312);
  endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56ad, cutscene_resetOamWithSomething2_hook, 0x60b0, 0x56b0);
  CALL_C(0x56b0, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x56b3);
  if (!(F & FZ)) {
    CYCT(0x56b3, 0x56b4); ret_effect(gb);
    return;
  }
  CYC(0x56b3, 0x56b4);
  CALL_C(0x56b4, cutscene_clearTmpCBB3_hook, 0x6086, 0x56b7);
  CYC(0x56b7, 0x56b9); A = 0x01;
  CYC(0x56b9, 0x56bc); mem_wr(gb, wGenericCutscene_cbc1, A);
  CYC(0x56bc, 0x56bf); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56bf, 0x56c2); A = mem_rd(gb, wTextIsActive);
  CYC(0x56c2, 0x56c3); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x56c3, 0x56c4); ret_effect(gb);
    return;
  }
  CYC(0x56c3, 0x56c4);
  CYC(0x56c4, 0x56c7); A = mem_rd(gb, wKeysJustPressed);
  CYC(0x56c7, 0x56c8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x56c8, 0x56c9); ret_effect(gb);
    return;
  }
  CYC(0x56c8, 0x56c9);
  CALL_C(0x56c9, incCbc2_hook, 0x3067, 0x56cc);
  CYC(0x56cc, 0x56ce); A = 0x04;
  CYC(0x56ce, 0x56d1); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_09_stage0__state15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56d1, 0x56d4); A = W8(wPaletteThread_mode);
  CYC(0x56d4, 0x56d5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x56d5, 0x56d6); ret_effect(gb);
    return;
  }
  CYC(0x56d5, 0x56d6);
  CYC(0x56d6, 0x56d7); alu_xor(gb, A);
  CYC(0x56d7, 0x56da); mem_wr(gb, wTextIsActive, A);
  CYC(0x56da, 0x56dc); A = 0x12;
  CYC(0x56dc, 0x56df); mem_wr(gb, wCutsceneTrigger, A);
  CYC(0x56df, 0x56e0); ret_effect(gb);
}

void endgameCutsceneHandler_09_stage1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56e0, endgameCutsceneHandler_09_stage1__runStates_hook, 0x56e6, 0x56e3);
  CYC(0x56e3, 0x56e6); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56ff, cutscene_resetOamWithSomething2_hook, 0x60b0, 0x5702);
  CYC(0x5702, 0x5705); A = W8(wPaletteThread_mode);
  CYC(0x5705, 0x5706); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5706, 0x5707); ret_effect(gb);
    return;
  }
  CYC(0x5706, 0x5707);
  CALL_C(0x5707, incCbc2_hook, 0x3067, 0x570a);
  CYC(0x570a, 0x570d); SET_HL(wTmpcbb3);
  CYC(0x570d, 0x570f); mem_wr(gb, HL, 0x3c);
  CALL_C(0x570f, disableLcd_hook, 0x02c1, 0x5712);
  CALL_C(0x5712, clearOam_hook, 0x049f, 0x5715);
  CYC(0x5715, 0x5717); A = 0x2c;
  CALL_C(0x5717, loadGfxHeader_hook, 0x0626, 0x571a);
  CYC(0x571a, 0x571c); A = 0x9e;
  CALL_C(0x571c, loadPaletteHeader_hook, 0x050b, 0x571f);
  CYC(0x571f, 0x5721); A = 0x04;
  CALL_C(0x5721, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5724);
  CYC(0x5724, 0x5726); A = 0x21;
  CALL_C(0x5726, playSound_b00_hook, 0x0c98, 0x5729);
  CYC(0x5729, 0x572c); fadeinFromWhite_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x572c, 0x572e); A = 0x01;
  CYC(0x572e, 0x5731); mem_wr(gb, wTextboxFlags, A);
  CYC(0x5731, 0x5733); A = 0x3c;
  CYC(0x5733, 0x5736); SET_BC(0x280b);
  CALL_C(0x5736, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5739);
  if (!(F & FZ)) {
    CYCT(0x5739, 0x573a); ret_effect(gb);
    return;
  }
  CYC(0x5739, 0x573a);
  CALL_C(0x573a, incCbc2_hook, 0x3067, 0x573d);
  CYC(0x573d, 0x573e); A = E;
  CYC(0x573e, 0x5741); mem_wr(gb, wTmpcbb3, A);
  CYC(0x5741, 0x5744); showText_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5744, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5747);
  if (!(F & FZ)) {
    CYCT(0x5747, 0x5748); ret_effect(gb);
    return;
  }
  CYC(0x5747, 0x5748);
  CALL_C(0x5748, incCbc2_hook, 0x3067, 0x574b);
  CYC(0x574b, 0x574e); SET_HL(wTmpcbb5);
  CYC(0x574e, 0x5750); mem_wr(gb, HL, 0xd0);
  endgameCutsceneHandler_09_stage1__loadCertainOamData1_hook(gb);
}

void endgameCutsceneHandler_09_stage1__loadOamData_hook(GB *gb) {
  CYC(0x5755, 0x5757); B = 0x30;
  CYC(0x5757, 0x5758); push_effect(gb, DE);
  CYC(0x5758, 0x575b); SET_DE(wTmpcbb5);
  CYC(0x575b, 0x575c); A = mem_rd(gb, DE);
  CYC(0x575c, 0x575d); SET_DE(pop_effect(gb));
  CYC(0x575d, 0x575e); C = A;
  CYC(0x575e, 0x5761); cutscene_resetOamWithData_hook(gb);
}

void endgameCutsceneHandler_09_stage1__loadCertainOamData1_hook(GB *gb) {
  CYC(0x5750, 0x5753); SET_HL(0x4d05);
  CYC(0x5753, 0x5755); E = 0x16;
  endgameCutsceneHandler_09_stage1__loadOamData_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5761, 0x5764); SET_HL(wTmpcbb5);
  CYC(0x5764, 0x5765); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5765, 0x5767);
    endgameCutsceneHandler_09_stage1__loadCertainOamData1_hook(gb);
    return;
  }
  CYC(0x5765, 0x5767);
  CALL_C(0x5767, clearOam_hook, 0x049f, 0x576a);
  CYC(0x576a, 0x576c); A = 0x0a;
  CALL_C(0x576c, loadUncompressedGfxHeader_hook, 0x05da, 0x576f);
  CYC(0x576f, 0x5772); SET_HL(wTmpcbb3);
  CYC(0x5772, 0x5774); mem_wr(gb, HL, 0x1e);
  CYC(0x5774, 0x5777); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5777, decCbb3_hook, 0x305d, 0x577a);
  if (!(F & FZ)) {
    CYCT(0x577a, 0x577b); ret_effect(gb);
    return;
  }
  CYC(0x577a, 0x577b);
  CALL_C(0x577b, incCbc2_hook, 0x3067, 0x577e);
  CYC(0x577e, 0x5781); SET_HL(wTmpcbb5);
  CYC(0x5781, 0x5783); mem_wr(gb, HL, 0xd0);
  endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(GB *gb) {
  CYC(0x5783, 0x5786); SET_HL(0x4d9e);
  CYC(0x5786, 0x5788); E = 0x16;
  CYC(0x5788, 0x578a);
  endgameCutsceneHandler_09_stage1__loadOamData_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state5_hook(GB *gb) {
  CYC(0x578a, 0x578d); push_effect(gb, 0x578d);
  endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook(gb);
  CYC(0x578d, 0x5790); SET_HL(wTmpcbb5);
  CYC(0x5790, 0x5791); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5791, 0x5792); A = mem_rd(gb, HL);
  CYC(0x5792, 0x5794); alu_sub(gb, 0xa0);
  if (!(F & FZ)) {
    CYCT(0x5794, 0x5795); ret_effect(gb);
    return;
  }
  CYC(0x5794, 0x5795);
  CYC(0x5795, 0x5798); mem_wr(gb, wScreenOffsetY, A);
  CYC(0x5798, 0x579b); mem_wr(gb, wScreenOffsetX, A);
  CYC(0x579b, 0x579d); A = 0x1e;
  CYC(0x579d, 0x57a0); mem_wr(gb, wTmpcbb3, A);
  CYC(0x57a0, 0x57a3); mem_wr(gb, wOpenedMenuType, A);
  CYC(0x57a3, 0x57a6); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57a6, endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook, 0x5783, 0x57a9);
  CALL_C(0x57a9, decCbb3_hook, 0x305d, 0x57ac);
  if (!(F & FZ)) {
    CYCT(0x57ac, 0x57ad); ret_effect(gb);
    return;
  }
  CYC(0x57ac, 0x57ad);
  CYC(0x57ad, 0x57b0); SET_HL(wTmpcbb3);
  CYC(0x57b0, 0x57b2); mem_wr(gb, HL, 0x14);
  CYC(0x57b2, 0x57b5); SET_BC(0x280c);
  CALL_C(0x57b5, endgameCutsceneHandler_09_stage0__showTextDuringTwinrovaCutscene_hook, 0x56a5, 0x57b8);
  CYC(0x57b8, 0x57bb); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57bb, endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook, 0x5783, 0x57be);
  CALL_C(0x57be, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x57c1);
  if (!(F & FZ)) {
    CYCT(0x57c1, 0x57c2); ret_effect(gb);
    return;
  }
  CYC(0x57c1, 0x57c2);
  CYC(0x57c2, 0x57c3); alu_xor(gb, A);
  CYC(0x57c3, 0x57c6); mem_wr(gb, wOpenedMenuType, A);
  CYC(0x57c6, 0x57c7); A = alu_dec8(gb, A);
  CYC(0x57c7, 0x57ca); mem_wr(gb, wTmpcbba, A);
  CYC(0x57ca, 0x57cc); A = 0xd2;
  CALL_C(0x57cc, playSound_b00_hook, 0x0c98, 0x57cf);
  CYC(0x57cf, 0x57d2); incCbc2_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57d2, endgameCutsceneHandler_09_stage1__loadCertainOamData2_hook, 0x5783, 0x57d5);
  CYC(0x57d5, 0x57d8); SET_HL(wTmpcbb3);
  CYC(0x57d8, 0x57da); B = 0x02;
  CALL_C(0x57da, flashScreen_hook, 0x2d73, 0x57dd);
  if (F & FZ) {
    CYCT(0x57dd, 0x57de); ret_effect(gb);
    return;
  }
  CYC(0x57dd, 0x57de);
  CALL_C(0x57de, incCbc2_hook, 0x3067, 0x57e1);
  CYC(0x57e1, 0x57e4); SET_HL(wTmpcbb3);
  CYC(0x57e4, 0x57e6); mem_wr(gb, HL, 0x1e);
  CALL_C(0x57e6, disableLcd_hook, 0x02c1, 0x57e9);
  CALL_C(0x57e9, clearOam_hook, 0x049f, 0x57ec);
  CYC(0x57ec, 0x57ed); alu_xor(gb, A);
  CYC(0x57ed, 0x57ef); hram_wr(gb, 0x4f, A);
  CYC(0x57ef, 0x57f2); SET_HL(0x8000);
  CYC(0x57f2, 0x57f5); SET_BC(0x2000);
  CALL_C(0x57f5, clearMemoryBc_hook, 0x0475, 0x57f8);
  CYC(0x57f8, 0x57f9); alu_xor(gb, A);
  CYC(0x57f9, 0x57fb); hram_wr(gb, 0x4f, A);
  CYC(0x57fb, 0x57fe); SET_HL(0x9c00);
  CYC(0x57fe, 0x5801); SET_BC(0x0400);
  CALL_C(0x5801, clearMemoryBc_hook, 0x0475, 0x5804);
  CYC(0x5804, 0x5806); A = 0x01;
  CYC(0x5806, 0x5808); hram_wr(gb, 0x4f, A);
  CYC(0x5808, 0x580b); SET_HL(0x9c00);
  CYC(0x580b, 0x580e); SET_BC(0x0400);
  CALL_C(0x580e, clearMemoryBc_hook, 0x0475, 0x5811);
  CYC(0x5811, 0x5813); A = 0x2d;
  CALL_C(0x5813, loadGfxHeader_hook, 0x0626, 0x5816);
  CYC(0x5816, 0x5818); A = 0x9c;
  CALL_C(0x5818, loadPaletteHeader_hook, 0x050b, 0x581b);
  CYC(0x581b, 0x581d); A = 0x04;
  CALL_C(0x581d, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5820);
  CYC(0x5820, 0x5822); A = 0xd2;
  CALL_C(0x5822, playSound_b00_hook, 0x0c98, 0x5825);
  CYC(0x5825, 0x5828); clearPaletteFadeVariablesAndRefreshPalettes_hook(gb);
}

void endgameCutsceneHandler_09_stage1__state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5828, decCbb3_hook, 0x305d, 0x582b);
  if (!(F & FZ)) {
    CYCT(0x582b, 0x582c); ret_effect(gb);
    return;
  }
  CYC(0x582b, 0x582c);
  CYC(0x582c, 0x582e); A = 0x0a;
  CYC(0x582e, 0x5831); mem_wr(gb, 0xc2ef, A);
  CALL_C(0x5831, cutscene_clearTmpCBB3_hook, 0x6086, 0x5834);
  CYC(0x5834, 0x5837); SET_HL(wRoomLayout);
  CYC(0x5837, 0x583a); SET_BC(0x00c0);
  CALL_C(0x583a, clearMemoryBc_hook, 0x0475, 0x583d);
  CYC(0x583d, 0x5840); SET_HL(wRoomCollisions);
  CYC(0x5840, 0x5843); SET_BC(0x00c0);
  CALL_C(0x5843, clearMemoryBc_hook, 0x0475, 0x5846);
  CYC(0x5846, 0x5848); hram_wr(gb, 0xaa, A);
  CYC(0x5848, 0x584a); hram_wr(gb, 0xac, A);
  CYC(0x584a, 0x584d); SET_HL(wTmpcbb3);
  CYC(0x584d, 0x584f); mem_wr(gb, HL, 0x3c);
  CYC(0x584f, 0x5851); A = 0x03;
  CYC(0x5851, 0x5854); fadeoutToBlackWithDelay_hook(gb);
}

void endgameCutsceneHandler_20__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x58bf, decCbb3_hook, 0x305d, 0x58c2);
  if (!(F & FZ)) {
    CYCT(0x58c2, 0x58c3); ret_effect(gb);
    return;
  }
  CYC(0x58c2, 0x58c3);
  CALL_C(0x58c3, incCbc1_hook, 0x3062, 0x58c6);
  CYC(0x58c6, 0x58c9); SET_HL(wTmpcbb3);
  CYC(0x58c9, 0x58cb); mem_wr(gb, HL, 0x28);
  CYC(0x58cb, 0x58cd); A = 0x04;
  CYC(0x58cd, 0x58d0); mem_wr(gb, wTextboxFlags, A);
  CYC(0x58d0, 0x58d3); SET_BC(0x2825);
  CYC(0x58d3, 0x58d6); showText_hook(gb);
}

void endgameCutsceneHandler_20__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x58d6, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x58d9);
  if (!(F & FZ)) {
    CYCT(0x58d9, 0x58da); ret_effect(gb);
    return;
  }
  CYC(0x58d9, 0x58da);
  CALL_C(0x58da, incCbc1_hook, 0x3062, 0x58dd);
  CYC(0x58dd, 0x58df); A = 0x20;
  CYC(0x58df, 0x58e2); SET_HL(wTmpcbb3);
  CYC(0x58e2, 0x58e3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x58e3, 0x58e4); alu_xor(gb, A);
  CYC(0x58e4, 0x58e5); mem_wr(gb, HL, A);
  CYC(0x58e5, 0x58e6); ret_effect(gb);
}

void endgameCutsceneHandler_20__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x58e6, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x58e9);
  if (!(F & FZ)) {
    CYCT(0x58e9, 0x58ea); ret_effect(gb);
    return;
  }
  CYC(0x58e9, 0x58ea);
  CYC(0x58ea, 0x58ed); SET_HL(wTmpcbb3);
  CYC(0x58ed, 0x58ef); mem_wr(gb, HL, 0x20);
  CYC(0x58ef, 0x58f0); SET_HL(HL + 1);
  CYC(0x58f0, 0x58f1); A = mem_rd(gb, HL);
  CYC(0x58f1, 0x58f3); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x58f3, 0x58f5);
    goto continue_state3;
  }
  CYC(0x58f3, 0x58f5);
  CYC(0x58f5, 0x58f6); B = A;
  CYC(0x58f6, 0x58f7); push_effect(gb, HL);
  CYC(0x58f7, 0x58f9); A = 0x72;
  CALL_C(0x58f9, playSound_b00_hook, 0x0c98, 0x58fc);
  CYC(0x58fc, 0x58fd); SET_HL(pop_effect(gb));
  CYC(0x58fd, 0x58fe); A = B;

continue_state3:
  CYC(0x58fe, 0x58ff); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x58ff, 0x5902); SET_HL(0x5932);
  CYC(0x5902, 0x5903); add_a_to_hl_from_rst(gb, 0x5903);
  CYC(0x5903, 0x5904); A = mem_rd(gb, HL);
  CYC(0x5904, 0x5905); alu_or(gb, A);
  CYC(0x5905, 0x5906); B = A;
  if (!(F & FZ)) {
    CYCT(0x5906, 0x5908);
    endgameCutsceneHandler_20__func_5920_hook(gb);
    return;
  }
  CYC(0x5906, 0x5908);
  CALL_C(0x5908, fadeinFromBlack_hook, 0x32e6, 0x590b);
  CYC(0x590b, 0x590d); A = 0x01;
  CYC(0x590d, 0x5910); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x5910, 0x5913); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x5913, 0x5916); SET_HL(wTmpcbb3);
  CYC(0x5916, 0x5918); mem_wr(gb, HL, 0x3c);
  CYC(0x5918, 0x591a); A = 0x1d;
  CALL_C(0x591a, playSound_b00_hook, 0x0c98, 0x591d);
  CYC(0x591d, 0x5920); incCbc1_hook(gb);
}

void endgameCutsceneHandler_20__func_5920_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5920, fastFadeinFromBlack_hook, 0x32dd, 0x5923);
  CYC(0x5923, 0x5924); A = B;
  CYC(0x5924, 0x5927); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x5927, 0x592a); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x592a, 0x592b); alu_xor(gb, A);
  CYC(0x592b, 0x592e); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x592e, 0x5931); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x5931, 0x5932); ret_effect(gb);
}

void endgameCutsceneHandler_20__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5938, 0x593a); E = 0x28;
  CYC(0x593a, 0x593d); SET_BC(0x2826);
  CALL_C(0x593d, endgameCutsceneHandler_20__func_5943_hook, 0x5943, 0x5940);
  CYC(0x5940, 0x5943); cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(gb);
}

void endgameCutsceneHandler_20__func_5943_hook(GB *gb) {
  CYC(0x5943, 0x5945); A = 0x08;
  CYC(0x5945, 0x5948); mem_wr(gb, wTextboxFlags, A);
  CYC(0x5948, 0x594a); A = 0x03;
  CYC(0x594a, 0x594d); mem_wr(gb, wTextboxPosition, A);
  CYC(0x594d, 0x594e); ret_effect(gb);
}

void endgameCutsceneHandler_20__state5_hook(GB *gb) {
  CYC(0x594e, 0x5950); E = 0x28;
  CYC(0x5950, 0x5953); SET_BC(0x2827);
  endgameCutsceneHandler_20__func_5953_hook(gb);
}

void endgameCutsceneHandler_20__func_5953_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5953, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5956);
  if (!(F & FZ)) {
    CYCT(0x5956, 0x5957); ret_effect(gb);
    return;
  }
  CYC(0x5956, 0x5957);
  CALL_C(0x5957, incCbc1_hook, 0x3062, 0x595a);
  CYC(0x595a, 0x595d); SET_HL(wTmpcbb3);
  CYC(0x595d, 0x595e); mem_wr(gb, HL, E);
  CALL_C(0x595e, endgameCutsceneHandler_20__func_5943_hook, 0x5943, 0x5961);
  CYC(0x5961, 0x5964); showText_hook(gb);
}

void endgameCutsceneHandler_20__state6_hook(GB *gb) {
  CYC(0x5964, 0x5966); E = 0x3c;
  CYC(0x5966, 0x5969); SET_BC(0x2828);
  CYC(0x5969, 0x596b); endgameCutsceneHandler_20__func_5953_hook(gb);
}

void endgameCutsceneHandler_20__state7_hook(GB *gb) {
  CYC(0x596b, 0x596d); E = 0xb4;
  endgameCutsceneHandler_20__func_596d_hook(gb);
}

void endgameCutsceneHandler_20__func_596d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x596d, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5970);
  if (!(F & FZ)) {
    CYCT(0x5970, 0x5971); ret_effect(gb);
    return;
  }
  CYC(0x5970, 0x5971);
  CALL_C(0x5971, incCbc1_hook, 0x3062, 0x5974);
  CYC(0x5974, 0x5977); SET_HL(wTmpcbb3);
  CYC(0x5977, 0x5978); mem_wr(gb, HL, E);
  CYC(0x5978, 0x5979); ret_effect(gb);
}

void endgameCutsceneHandler_20__state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5979, endgameCutsceneHandler_20__func_5995_hook, 0x5995, 0x597c);
  CALL_C(0x597c, cutscene_rumbleSoundWhenFrameCounterLowerNibbleIs0_hook, 0x609b, 0x597f);
  CALL_C(0x597f, decCbb3_hook, 0x305d, 0x5982);
  if (!(F & FZ)) {
    CYCT(0x5982, 0x5983); ret_effect(gb);
    return;
  }
  CYC(0x5982, 0x5983);
  CYC(0x5983, 0x5985); A = 0xf1;
  CALL_C(0x5985, playSound_b00_hook, 0x0c98, 0x5988);
  CYC(0x5988, 0x598a); A = 0xfb;
  CALL_C(0x598a, playSound_b00_hook, 0x0c98, 0x598d);
  CALL_C(0x598d, incCbc1_hook, 0x3062, 0x5990);
  CYC(0x5990, 0x5992); A = 0x04;
  CYC(0x5992, 0x5995); fadeoutToWhiteWithDelay_hook(gb);
}

void endgameCutsceneHandler_20__func_5995_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5995, 0x5998); SET_HL(wGfxRegs1_SCY);
  CYC(0x5998, 0x599a); A = hram_rd(gb, 0xaa);
  CYC(0x599a, 0x599b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x599b, 0x599d); A = hram_rd(gb, 0xac);
  CYC(0x599d, 0x599e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x599e, 0x59a1); SET_HL(0x59ab);
  CYC(0x59a1, 0x59a4); SET_DE(wGfxRegs1_SCY);
  CALL_C(0x59a4, endgameCutsceneHandler_20__func_59b3_hook, 0x59b3, 0x59a7);
  CYC(0x59a7, 0x59a8); SET_DE(DE + 1);
  CYC(0x59a8, 0x59ab); endgameCutsceneHandler_20__func_59b3_hook(gb);
}

void endgameCutsceneHandler_20__func_59b3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x59b3, 0x59b4); push_effect(gb, HL);
  CALL_C(0x59b4, getRandomNumber_hook, 0x043e, 0x59b7);
  CYC(0x59b7, 0x59b9); alu_and(gb, 0x07);
  CYC(0x59b9, 0x59ba); add_a_to_hl_from_rst(gb, 0x59ba);
  CYC(0x59ba, 0x59bb); A = mem_rd(gb, HL);
  CYC(0x59bb, 0x59bc); B = A;
  CYC(0x59bc, 0x59bd); A = mem_rd(gb, DE);
  CYC(0x59bd, 0x59be); alu_add(gb, B);
  CYC(0x59be, 0x59bf); mem_wr(gb, DE, A);
  CYC(0x59bf, 0x59c0); SET_HL(pop_effect(gb));
  CYC(0x59c0, 0x59c1); ret_effect(gb);
}

void endgameCutsceneHandler_20__stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a02, updateStatusBar_hook, 0x1a9c, 0x5a05);
  CYC(0x5a05, 0x5a08); A = mem_rd(gb, 0xcfd0);
  CYC(0x5a08, 0x5a0a); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x5a0a, 0x5a0b); ret_effect(gb);
    return;
  }
  CYC(0x5a0a, 0x5a0b);
  CALL_C(0x5a0b, incCbc1_hook, 0x3062, 0x5a0e);
  CYC(0x5a0e, 0x5a10); C = 0x40;
  CYC(0x5a10, 0x5a12); A = 0x29;
  CALL_C(0x5a12, giveTreasure_hook, 0x171c, 0x5a15);
  CYC(0x5a15, 0x5a17); A = 0x08;
  CALL_C(0x5a17, setLinkIDOverride_hook, 0x2acf, 0x5a1a);
  CYC(0x5a1a, 0x5a1c); L = 0x02;
  CYC(0x5a1c, 0x5a1e); mem_wr(gb, HL, 0x0c);
  CYC(0x5a1e, 0x5a21); SET_HL(wTmpcbb3);
  CYC(0x5a21, 0x5a23); mem_wr(gb, HL, 0x5a);
  CYC(0x5a23, 0x5a25); A = 0x4a;
  CYC(0x5a25, 0x5a28); playSound_b00_hook(gb);
}

void endgameCutsceneHandler_20__stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a28, updateStatusBar_hook, 0x1a9c, 0x5a2b);
  CALL_C(0x5a2b, decCbb3_hook, 0x305d, 0x5a2e);
  if (!(F & FZ)) {
    CYCT(0x5a2e, 0x5a2f); ret_effect(gb);
    return;
  }
  CYC(0x5a2e, 0x5a2f);
  CALL_C(0x5a2f, incCbc1_hook, 0x3062, 0x5a32);
  CYC(0x5a32, 0x5a35); SET_HL(wTmpcbb3);
  CYC(0x5a35, 0x5a37); mem_wr(gb, HL, 0xb4);
  CYC(0x5a37, 0x5a3a); SET_BC(0x4860);
  CYC(0x5a3a, 0x5a3c); A = 0xff;
  CYC(0x5a3c, 0x5a3f); createEnergySwirlGoingOut_hook(gb);
}

void endgameCutsceneHandler_20__stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a3f, updateStatusBar_hook, 0x1a9c, 0x5a42);
  CALL_C(0x5a42, decCbb3_hook, 0x305d, 0x5a45);
  if (!(F & FZ)) {
    CYCT(0x5a45, 0x5a46); ret_effect(gb);
    return;
  }
  CYC(0x5a45, 0x5a46);
  CALL_C(0x5a46, incCbc1_hook, 0x3062, 0x5a49);
  CYC(0x5a49, 0x5a4c); SET_HL(wTmpcbb3);
  CYC(0x5a4c, 0x5a4e); mem_wr(gb, HL, 0x3c);
  CYC(0x5a4e, 0x5a51); fadeoutToWhite_hook(gb);
}

void endgameCutsceneHandler_20__stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ab9, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5abc);
  if (!(F & FZ)) {
    CYCT(0x5abc, 0x5abd); ret_effect(gb);
    return;
  }
  CYC(0x5abc, 0x5abd);
  CALL_C(0x5abd, incCbc1_hook, 0x3062, 0x5ac0);
  CYC(0x5ac0, 0x5ac2); B = 0x04;
  endgameCutsceneHandler_20__func_5ac2_hook(gb);
}

void endgameCutsceneHandler_20__func_5ac2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ac2, fadeinFromWhite_hook, 0x3299, 0x5ac5);
  CYC(0x5ac5, 0x5ac6); A = B;
  CYC(0x5ac6, 0x5ac9); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x5ac9, 0x5acc); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x5acc, 0x5acd); alu_xor(gb, A);
  CYC(0x5acd, 0x5ad0); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x5ad0, 0x5ad3); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x5ad3, 0x5ad6); SET_HL(wTmpcbb3);
  CYC(0x5ad6, 0x5ad8); mem_wr(gb, HL, 0x3c);
  CYC(0x5ad8, 0x5ad9); ret_effect(gb);
}

void endgameCutsceneHandler_20__stateF_hook(GB *gb) {
  CYC(0x5ad9, 0x5adb); E = 0x1e;
  CYC(0x5adb, 0x5ade); SET_BC(0x1d1b);
  CYC(0x5ade, 0x5ae1); cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(gb);
}

void endgameCutsceneHandler_20__state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ae1, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x5ae4);
  if (!(F & FZ)) {
    CYCT(0x5ae4, 0x5ae5); ret_effect(gb);
    return;
  }
  CYC(0x5ae4, 0x5ae5);
  CALL_C(0x5ae5, incCbc1_hook, 0x3062, 0x5ae8);
  CYC(0x5ae8, 0x5aea); B = 0x12;
  CYC(0x5aea, 0x5aed); endgameCutsceneHandler_20__func_5ac2_hook(gb);
}

void endgameCutsceneHandler_20__state11_hook(GB *gb) {
  CYC(0x5aed, 0x5aef); E = 0x1e;
  CYC(0x5aef, 0x5af2); SET_BC(0x1d1c);
  CYC(0x5af2, 0x5af5); cutscene_decCBB3IfNotFadingOut_incState_setCBB3_showText_hook(gb);
}

void endgameCutsceneHandler_20__state12_hook(GB *gb) {
  CYC(0x5af5, 0x5af7); E = 0x3c;
  CYC(0x5af7, 0x5afa); endgameCutsceneHandler_20__func_596d_hook(gb);
}

void endgameCutsceneHandler_20__state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5afa, decCbb3_hook, 0x305d, 0x5afd);
  if (!(F & FZ)) {
    CYCT(0x5afd, 0x5afe); ret_effect(gb);
    return;
  }
  CYC(0x5afd, 0x5afe);
  CALL_C(0x5afe, incCbc1_hook, 0x3062, 0x5b01);
  CYC(0x5b01, 0x5b04); SET_HL(wTmpcbb3);
  CYC(0x5b04, 0x5b06); mem_wr(gb, HL, 0xf0);
  CYC(0x5b06, 0x5b08); A = 0xff;
  CYC(0x5b08, 0x5b0b); SET_BC(0x4850);
  CYC(0x5b0b, 0x5b0e); createEnergySwirlGoingOut_hook(gb);
}

void endgameCutsceneHandler_20__state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b0e, decCbb3_hook, 0x305d, 0x5b11);
  if (!(F & FZ)) {
    CYCT(0x5b11, 0x5b12); ret_effect(gb);
    return;
  }
  CYC(0x5b11, 0x5b12);
  CYC(0x5b12, 0x5b15); SET_HL(wTmpcbb3);
  CYC(0x5b15, 0x5b17); mem_wr(gb, HL, 0x5a);
  CALL_C(0x5b17, fadeoutToWhite_hook, 0x326c, 0x5b1a);
  CYC(0x5b1a, 0x5b1c); A = 0xfc;
  CYC(0x5b1c, 0x5b1f); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x5b1f, 0x5b22); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x5b22, 0x5b25); incCbc1_hook(gb);
}

void endgameCutsceneHandler_20__state15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b25, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x5b28);
  if (!(F & FZ)) {
    CYCT(0x5b28, 0x5b29); ret_effect(gb);
    return;
  }
  CYC(0x5b28, 0x5b29);
  CALL_C(0x5b29, incCbc1_hook, 0x3062, 0x5b2c);
  CALL_C(0x5b2c, clearDynamicInteractions_hook, 0x35d2, 0x5b2f);
  CALL_C(0x5b2f, clearParts_hook, 0x3605, 0x5b32);
  CALL_C(0x5b32, clearOam_hook, 0x049f, 0x5b35);
  CYC(0x5b35, 0x5b38); SET_HL(wTmpcbb3);
  CYC(0x5b38, 0x5b3a); mem_wr(gb, HL, 0x3c);
  CYC(0x5b3a, 0x5b3d); SET_BC(0x1d1d);
  CYC(0x5b3d, 0x5b40); showTextNonExitable_hook(gb);
}

void endgameCutsceneHandler_20__state16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b40, 0x5b43); A = W8(wTextIsActive);
  CYC(0x5b43, 0x5b44); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x5b44, 0x5b45); ret_effect(gb);
    return;
  }
  CYC(0x5b44, 0x5b45);
  CALL_C(0x5b45, decCbb3_hook, 0x305d, 0x5b48);
  if (!(F & FZ)) {
    CYCT(0x5b48, 0x5b49); ret_effect(gb);
    return;
  }
  CYC(0x5b48, 0x5b49);
  CALL_C(0x5b49, showStatusBar_hook, 0x1aa4, 0x5b4c);
  CYC(0x5b4c, 0x5b4d); alu_xor(gb, A);
  CYC(0x5b4d, 0x5b50); W8(wOpenedMenuType) = A;
  CYC(0x5b50, 0x5b51); A = alu_dec8(gb, A);
  CYC(0x5b51, 0x5b54); W8(wActiveMusic) = A;
  CYC(0x5b54, 0x5b56); A = 0xfa;
  CALL_C(0x5b56, playSound_b00_hook, 0x0c98, 0x5b59);
  CYC(0x5b59, 0x5b5c); SET_HL(0x5b5f);
  CYC(0x5b5c, 0x5b5f); setWarpDestVariables_hook(gb);
}

void endgameCutsceneHandler_0f_hook(GB *gb) {
  CYC(0x5b64, 0x5b67); SET_DE(0xcbc1);
  CYC(0x5b67, 0x5b68); A = mem_rd(gb, DE);
  CYC(0x5b68, 0x5b69); push_effect(gb, 0x5b69);
  switch (endgame_jump_table(gb)) {
    case 0x5b6d: endgameCutsceneHandler_0f__state0_hook(gb); return;
    case 0x5cbe: endgameCutsceneHandler_0f__state1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_0f__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b6d, updateStatusBar_hook, 0x1a9c, 0x5b70);
  CALL_C(0x5b70, endgameCutsceneHandler_0f__state0__runSubstates_hook, 0x5b76, 0x5b73);
  CYC(0x5b73, 0x5b76); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_0f__state0__runSubstates_hook(GB *gb) {
  CYC(0x5b76, 0x5b79); SET_DE(0xcbc2);
  CYC(0x5b79, 0x5b7a); A = mem_rd(gb, DE);
  CYC(0x5b7a, 0x5b7b); push_effect(gb, 0x5b7b);
  switch (endgame_jump_table(gb)) {
    case 0x5b97: endgameCutsceneHandler_0f__state0__substate0_hook(gb); return;
    case 0x5bc0: endgameCutsceneHandler_0f__state0__substate1_hook(gb); return;
    case 0x5bd1: endgameCutsceneHandler_0f__state0__substate2_hook(gb); return;
    case 0x5be3: endgameCutsceneHandler_0f__state0__substate3_hook(gb); return;
    case 0x5bfd: endgameCutsceneHandler_0f__state0__substate4_hook(gb); return;
    case 0x5c11: endgameCutsceneHandler_0f__state0__substate5_hook(gb); return;
    case 0x5c20: endgameCutsceneHandler_0f__state0__substate6_hook(gb); return;
    case 0x5c34: endgameCutsceneHandler_0f__state0__substate7_hook(gb); return;
    case 0x5c46: endgameCutsceneHandler_0f__state0__substate8_hook(gb); return;
    case 0x5c58: endgameCutsceneHandler_0f__state0__substate9_hook(gb); return;
    case 0x5c6d: endgameCutsceneHandler_0f__state0__substateA_hook(gb); return;
    case 0x5c72: endgameCutsceneHandler_0f__state0__substateB_hook(gb); return;
    case 0x5c8c: endgameCutsceneHandler_0f__state0__substateC_hook(gb); return;
    case 0x5c9e: endgameCutsceneHandler_0f__state0__substateD_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_0f__state0__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b97, 0x5b99); A = 0x01;
  CYC(0x5b99, 0x5b9a); mem_wr(gb, DE, A);
  CYC(0x5b9a, 0x5b9d); SET_HL(wActiveRing);
  CYC(0x5b9d, 0x5b9f); mem_wr(gb, HL, 0xff);
  CYC(0x5b9f, 0x5ba0); alu_xor(gb, A);
  CYC(0x5ba0, 0x5ba2); H8(hActiveObjectType) = A;
  CYC(0x5ba2, 0x5ba5); SET_DE(0xd000);
  CYC(0x5ba5, 0x5ba8); SET_BC(0xf8f0);
  CYC(0x5ba8, 0x5baa); A = 0x28;
  CALL_C(0x5baa, objectCreateExclamationMark_hook, 0x27e0, 0x5bad);
  CYC(0x5bad, 0x5baf); A = 0x28;
  CALL_C(0x5baf, objectCreateExclamationMark_hook, 0x27e0, 0x5bb2);
  CYC(0x5bb2, 0x5bb4); L = 0x4b;
  CYC(0x5bb4, 0x5bb6); mem_wr(gb, HL, 0x30);
  CYC(0x5bb6, 0x5bb7); L = alu_inc8(gb, L);
  CYC(0x5bb7, 0x5bb8); L = alu_inc8(gb, L);
  CYC(0x5bb8, 0x5bba); mem_wr(gb, HL, 0x78);
  CYC(0x5bba, 0x5bbd); SET_HL(wTmpcbb3);
  CYC(0x5bbd, 0x5bbf); mem_wr(gb, HL, 0x0a);
  CYC(0x5bbf, 0x5bc0); ret_effect(gb);
}

void endgameCutsceneHandler_0f__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5cbe, updateStatusBar_hook, 0x1a9c, 0x5cc1);
  CALL_C(0x5cc1, endgameCutsceneHandler_0f__state1__runSubstates_hook, 0x5cc7, 0x5cc4);
  CYC(0x5cc4, 0x5cc7); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_0f__state1__runSubstates_hook(GB *gb) {
  CYC(0x5cc7, 0x5cca); SET_DE(0xcbc2);
  CYC(0x5cca, 0x5ccb); A = mem_rd(gb, DE);
  CYC(0x5ccb, 0x5ccc); push_effect(gb, 0x5ccc);
  switch (endgame_jump_table(gb)) {
    case 0x5ce2: endgameCutsceneHandler_0f__state1__substate0_hook(gb); return;
    case 0x5d10: endgameCutsceneHandler_0f__state1__substate1_hook(gb); return;
    case 0x5d24: endgameCutsceneHandler_0f__state1__substate2_hook(gb); return;
    case 0x5d33: endgameCutsceneHandler_0f__state1__substate3_hook(gb); return;
    case 0x5d4e: endgameCutsceneHandler_0f__state1__substate4_hook(gb); return;
    case 0x5d69: endgameCutsceneHandler_0f__state1__substate5_hook(gb); return;
    case 0x5d76: endgameCutsceneHandler_0f__state1__substate6_hook(gb); return;
    case 0x5d91: endgameCutsceneHandler_0f__state1__substate7_hook(gb); return;
    case 0x5d9f: endgameCutsceneHandler_0f__state1__substate8_hook(gb); return;
    case 0x5dcc: endgameCutsceneHandler_0f__state1__substate9_hook(gb); return;
    case 0x5de0: endgameCutsceneHandler_0f__state1__substateA_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_0f__state1__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ce2, cutscene_setScreenShakeCounterTo4RumbleAt0_hook, 0x6096, 0x5ce5);
  CYC(0x5ce5, 0x5ce8); A = W8(wPaletteThread_mode);
  CYC(0x5ce8, 0x5ce9); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5ce9, 0x5cea); ret_effect(gb);
    return;
  }
  CYC(0x5ce9, 0x5cea);
  CALL_C(0x5cea, incCbc2_hook, 0x3067, 0x5ced);
  CYC(0x5ced, 0x5cef); A = 0x11;
  CYC(0x5cef, 0x5cf2); mem_wr(gb, 0xcfde, A);
  CALL_C(0x5cf2, cutscene_loadRoomObjectSetAndFadein, 0x64c5, 0x5cf5);
  CYC(0x5cf5, 0x5cf7); A = 0x04;
  CYC(0x5cf7, 0x5cf9); B = 0x02;
  CALL_C(0x5cf9, cutscene_loadAObjectGfxBTimes_andReload_hook, 0x6056, 0x5cfc);
  CYC(0x5cfc, 0x5cfe); A = 0xf1;
  CALL_C(0x5cfe, playSound_b00_hook, 0x0c98, 0x5d01);
  CYC(0x5d01, 0x5d03); A = 0xfa;
  CALL_C(0x5d03, playSound_b00_hook, 0x0c98, 0x5d06);
  CYC(0x5d06, 0x5d09); SET_HL(wTmpcbb3);
  CYC(0x5d09, 0x5d0b); mem_wr(gb, HL, 0x3c);
  CYC(0x5d0b, 0x5d0d); A = 0x02;
  CYC(0x5d0d, 0x5d10); loadGfxRegisterStateIndex_hook(gb);
}

void endgameCutsceneHandler_09_hook(GB *gb) {
  CYC(0x5449, 0x544c); SET_DE(0xcbc1);
  CYC(0x544c, 0x544d); A = mem_rd(gb, DE);
  CYC(0x544d, 0x544e); push_effect(gb, 0x544e);
  switch (endgame_jump_table(gb)) {
    case 0x5452: endgameCutsceneHandler_09_stage0_hook(gb); return;
    case 0x56e0: endgameCutsceneHandler_09_stage1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_09_stage0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5452, updateStatusBar_hook, 0x1a9c, 0x5455);
  CALL_C(0x5455, endgameCutsceneHandler_09_stage0__runStates_hook, 0x545b, 0x5458);
  CYC(0x5458, 0x545b); updateAllObjects_hook(gb);
}

void endgameCutsceneHandler_09_stage0__runStates_hook(GB *gb) {
  CYC(0x545b, 0x545e); SET_DE(0xcbc2);
  CYC(0x545e, 0x545f); A = mem_rd(gb, DE);
  CYC(0x545f, 0x5460); push_effect(gb, 0x5460);
  switch (endgame_jump_table(gb)) {
    case 0x548c: endgameCutsceneHandler_09_stage0__state0_hook(gb); return;
    case 0x54c7: endgameCutsceneHandler_09_stage0__state1_hook(gb); return;
    case 0x54d4: endgameCutsceneHandler_09_stage0__state2_hook(gb); return;
    case 0x54f1: endgameCutsceneHandler_09_stage0__state3_hook(gb); return;
    case 0x5500: endgameCutsceneHandler_09_stage0__state4_hook(gb); return;
    case 0x550d: endgameCutsceneHandler_09_stage0__state5_hook(gb); return;
    case 0x5549: endgameCutsceneHandler_09_stage0__state6_hook(gb); return;
    case 0x5551: endgameCutsceneHandler_09_stage0__state7_hook(gb); return;
    case 0x5569: endgameCutsceneHandler_09_stage0__state8_hook(gb); return;
    case 0x5575: endgameCutsceneHandler_09_stage0__state9_hook(gb); return;
    case 0x559e: endgameCutsceneHandler_09_stage0__stateA_hook(gb); return;
    case 0x55c3: endgameCutsceneHandler_09_stage0__stateB_hook(gb); return;
    case 0x55d4: endgameCutsceneHandler_09_stage0__stateC_hook(gb); return;
    case 0x55e8: endgameCutsceneHandler_09_stage0__stateD_hook(gb); return;
    case 0x55ff: endgameCutsceneHandler_09_stage0__stateE_hook(gb); return;
    case 0x561e: endgameCutsceneHandler_09_stage0__stateF_hook(gb); return;
    case 0x5644: endgameCutsceneHandler_09_stage0__state10_hook(gb); return;
    case 0x5661: endgameCutsceneHandler_09_stage0__state11_hook(gb); return;
    case 0x568e: endgameCutsceneHandler_09_stage0__state12_hook(gb); return;
    case 0x56ad: endgameCutsceneHandler_09_stage0__state13_hook(gb); return;
    case 0x56bf: endgameCutsceneHandler_09_stage0__state14_hook(gb); return;
    case 0x56d1: endgameCutsceneHandler_09_stage0__state15_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void endgameCutsceneHandler_09_stage1__runStates_hook(GB *gb) {
  CYC(0x56e6, 0x56e9); SET_DE(0xcbc2);
  CYC(0x56e9, 0x56ea); A = mem_rd(gb, DE);
  CYC(0x56ea, 0x56eb); push_effect(gb, 0x56eb);
  switch (endgame_jump_table(gb)) {
    case 0x56ff: endgameCutsceneHandler_09_stage1__state0_hook(gb); return;
    case 0x572c: endgameCutsceneHandler_09_stage1__state1_hook(gb); return;
    case 0x5744: endgameCutsceneHandler_09_stage1__state2_hook(gb); return;
    case 0x5761: endgameCutsceneHandler_09_stage1__state3_hook(gb); return;
    case 0x5777: endgameCutsceneHandler_09_stage1__state4_hook(gb); return;
    case 0x578a: endgameCutsceneHandler_09_stage1__state5_hook(gb); return;
    case 0x57a6: endgameCutsceneHandler_09_stage1__state6_hook(gb); return;
    case 0x57bb: endgameCutsceneHandler_09_stage1__state7_hook(gb); return;
    case 0x57d2: endgameCutsceneHandler_09_stage1__state8_hook(gb); return;
    case 0x5828: endgameCutsceneHandler_09_stage1__state9_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}
