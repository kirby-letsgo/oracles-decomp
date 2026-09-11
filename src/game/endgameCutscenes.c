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
