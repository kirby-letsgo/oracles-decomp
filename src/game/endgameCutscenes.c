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
