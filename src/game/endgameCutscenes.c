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
