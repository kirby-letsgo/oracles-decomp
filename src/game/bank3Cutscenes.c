#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void introCinematic_inTemple_updateCamera_hook(GB *gb);
void introCinematic_moveBlackBarsIn_hook(GB *gb);
void introCinematic_moveBlackBarsOut_hook(GB *gb);
void intro_gotoTitlescreen_hook(GB *gb);
void intro_gotoTitlescreen__afterCall4d09_hook(GB *gb);
void intro_restart_hook(GB *gb);
void intro_japaneseOnlyScreen_hook(GB *gb);
void intro_capcomScreen_hook(GB *gb);
void intro_titlescreen_hook(GB *gb);
void intro_titlescreen_state0_hook(GB *gb);
void intro_titlescreen_state1_hook(GB *gb);
void intro_titlescreen_state2_hook(GB *gb);
void intro_titlescreen_state3_hook(GB *gb);
void endgameCutsceneHandler_0f_hook(GB *gb);
void runIntroCinematic_hook(GB *gb);
void introCinematic_ridingHorse_hook(GB *gb);
void introCinematic_inTemple_hook(GB *gb);
void introCinematic_preTitlescreen_hook(GB *gb);
void introCinematic_preTitlescreen_state0_hook(GB *gb);
void introCinematic_preTitlescreen_state0__afterCall52da_hook(GB *gb);
void introCinematic_preTitlescreen_state3_hook(GB *gb);

static uint16_t intro_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void twinrovaCutsceneCaller_hook(GB *gb) {
  BASE(twinrovaCutsceneCaller);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  HANDOFF(HL);
}

void incCutsceneState_hook(GB *gb) {
  BASE(incCutsceneState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void unused_incTmpcbb3_hook(GB *gb) {
  BASE(unused_incTmpcbb3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void decTmpcbb4_hook(GB *gb) {
  BASE(decTmpcbb4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void setScreenShakeCounterTo255_hook(GB *gb) {
  BASE(setScreenShakeCounterTo255);
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5);
  TAIL(setScreenShakeCounter);
}

void twinrovaCutscene_createLightningStrike_hook(GB *gb) {
  BASE(twinrovaCutscene_createLightningStrike);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x27);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = 0xcb;
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); mem_wr(gb, HL, C);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void incIntroCinematicState_hook(GB *gb) {
  BASE(incIntroCinematicState);
  CYC(b_+0, b_+3); SET_HL(wIntro_cinematicState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, wThreadStateBuffer + 7, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void createInteraction_hook(GB *gb) {
  BASE(createInteraction);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); mem_wr(gb, HL, C);
  CYC(b_+7, b_+8); ret_effect(gb);
}

static void twinrovaCutscene_deleteAllInteractionsExceptFlames_delete(GB *gb) {
  BASE(twinrovaCutscene_deleteAllInteractionsExceptFlames);
  CYC(b_+22, b_+23); L = alu_dec8(gb, L);
  CYC(b_+23, b_+25); B = 0x40;
  CYC(b_+25, b_+28); clearMemory_hook(gb);
}

void twinrovaCutscene_deleteAllInteractionsExceptFlames_hook(GB *gb);
void twinrovaCutscene_loadAngryFlames_hook(GB *gb);

void twinrovaCutscene_state0_hook(GB *gb) {
  BASE(twinrovaCutscene_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+5);
  CYC(b_+5, b_+8); SET_HL(wTmpcbb3);
  CYC(b_+8, b_+10); B = 0x10;
  CALL_C(b_+10, clearMemory_hook, SYM(clearMemory), b_+13);
  CYC(b_+13, b_+15); TAIL(incCutsceneState);
}

void twinrovaCutscene_state1_hook(GB *gb) {
  BASE(twinrovaCutscene_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, incCutsceneState_hook, SYM(incCutsceneState), b_+8);
  CYC(b_+8, b_+10); A = GV(0xf1, 0x9a);
  CYC(b_+10, b_+13); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+13, twinrovaCutscene_fadeinToRoom_hook, SYM(twinrovaCutscene_fadeinToRoom), b_+16);
  CALL_C(b_+16, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+19);
  CYC(b_+19, b_+22); SET_HL(w1Link_yh);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x38);
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x78);
  CALL_C(b_+28, resetCamera_hook, SYM(resetCamera), b_+31);
  CYC(b_+31, b_+34); SET_HL((SYM(init) + 34));
  CALL_C(b_+34, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+37);
  CYC(b_+37, b_+39); A = 0xac;
  CALL_C(b_+39, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+42);
  CYC(b_+42, b_+44); A = 0x01;
  CYC(b_+44, b_+47); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+47, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+50);
  CYC(b_+50, b_+52); A = 0x04;
  CALL_C(b_+52, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+55);
  CYC(b_+55, b_+57); A = 0x02;
  CYC(b_+57, b_+60);
  TAIL(loadGfxRegisterStateIndex);
}

void twinrovaCutscene_fadeinToRoom_hook(GB *gb) {
  BASE(twinrovaCutscene_fadeinToRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CALL_C(b_+3, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+6);
  CALL_C(b_+6, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+9);
  CALL_C(b_+9, loadTilesetData_hook, SYM(loadTilesetData), b_+12);
  CALL_C(b_+12, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+15);
  CYC(b_+15, b_+18); TAIL(func_131f);
}

void cutscene18_body_hook(GB *gb) {
  BASE(cutscene18_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  HANDOFF(HL);
}

void twinrovaCutscene_state2_hook(GB *gb) {
  BASE(twinrovaCutscene_state2);
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+10, b_+13); TAIL(incCutsceneState);
}

void twinrovaCutscene_state3_hook(GB *gb) {
  BASE(twinrovaCutscene_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decTmpcbb4_hook, SYM(decTmpcbb4), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 180);
  CALL_C(b_+6, twinrovaCutscene_deleteAllInteractionsExceptFlames_hook, SYM(twinrovaCutscene_deleteAllInteractionsExceptFlames), b_+9);
  CALL_C(b_+9, twinrovaCutscene_loadAngryFlames_hook, SYM(twinrovaCutscene_loadAngryFlames), b_+12);
  CYC(b_+12, b_+14); A = 0xb0;
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+20); TAIL(incCutsceneState);
}

void cutscene18_state4_hook(GB *gb) {
  BASE(cutscene18_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setScreenShakeCounterTo255_hook, SYM(setScreenShakeCounterTo255), b_+3);
  CYC(b_+3, b_+6); A = W8(wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x3f);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); A = 0xb0;
    CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  }
  CALL_C(b_+15, decTmpcbb4_hook, SYM(decTmpcbb4), b_+18);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); A = 0x04;
  CALL_C(b_+21, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+24);
  CYC(b_+24, b_+27); TAIL(incCutsceneState);
}

void cutscene18_state5_hook(GB *gb) {
  BASE(cutscene18_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setScreenShakeCounterTo255_hook, SYM(setScreenShakeCounterTo255), b_+3);
  CYC(b_+3, b_+6); A = W8(wPaletteThread_mode);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = GV(0xf5, 0x9e);
  CYC(b_+10, b_+13); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+13, twinrovaCutscene_fadeinToRoom_hook, SYM(twinrovaCutscene_fadeinToRoom), b_+16);
  CALL_C(b_+16, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x03);
  CYC(b_+21, b_+23); L = 0x83;
  CYC(b_+23, b_+25); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+25, b_+28); SET_HL(w1Link_enabled);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x03);
  CYC(b_+30, b_+32); L = w1Link_yh & 0xff;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x78);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x78);
  CALL_C(b_+38, resetCamera_hook, SYM(resetCamera), b_+41);
  CYC(b_+41, b_+43); A = 0x01;
  CYC(b_+43, b_+46); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+46, b_+48); A = 0x01;
  CYC(b_+48, b_+51); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+51, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+54);
  CYC(b_+54, b_+56); A = 0x02;
  CALL_C(b_+56, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+59);
  CYC(b_+59, b_+61); A = 0x02;
  CYC(b_+61, b_+64); TAIL(loadGfxRegisterStateIndex);
}

void twinrovaCutscene_deleteAllInteractionsExceptFlames_hook(GB *gb) {
  BASE(twinrovaCutscene_deleteAllInteractionsExceptFlames);
  CYC(b_+0, b_+3); SET_HL(w4StatusBarTileMap);
  for (;;) {
    CYC(b_+3, b_+5); L = 0x40;
    CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+6, b_+7); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+7, b_+9);
    } else {
      CYC(b_+7, b_+9);
      CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+10, b_+12); alu_cp(gb, GV(0xa9, 0xb0));
      if (F & FZ) {
        CYCT(b_+12, b_+15); push_effect(gb, b_+15);
        twinrovaCutscene_deleteAllInteractionsExceptFlames_delete(gb);
      } else {
        CYC(b_+12, b_+15);
      }
    }
    CYC(b_+15, b_+16); H = alu_inc8(gb, H);
    CYC(b_+16, b_+17); A = H;
    CYC(b_+17, b_+19); alu_cp(gb, 0xe0);
    if (F & FC) {
      CYCT(b_+19, b_+21);
      continue;
    }
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+22); ret_effect(gb);
    return;
  }
}

void cutscene19_body_hook(GB *gb) {
  BASE(cutscene19_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  HANDOFF(HL);
}

void twinrovaCutscene_loadAngryFlames_hook(GB *gb) {
  BASE(twinrovaCutscene_loadAngryFlames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xaf;
  CALL_C(b_+2, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+5);
  CYC(b_+5, b_+8); SET_HL((SYM(init) + 47));
  CYC(b_+8, b_+11); TAIL(parseGivenObjectData_b00);
}

void cutscene19_strikeFlameWithLightning_hook(GB *gb) {
  BASE(cutscene19_strikeFlameWithLightning);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, twinrovaCutscene_createLightningStrike_hook, SYM(twinrovaCutscene_createLightningStrike), b_+3);
  CYC(b_+3, b_+6); TAIL(incCutsceneState);
}

void cutscene19_state4_hook(GB *gb) {
  BASE(cutscene19_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decTmpcbb4_hook, SYM(decTmpcbb4), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x14);
  CYC(b_+6, b_+9); SET_BC(0x1878);
  TAIL(cutscene19_strikeFlameWithLightning);
}

void cutscene19_state5_hook(GB *gb) {
  BASE(cutscene19_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decTmpcbb4_hook, SYM(decTmpcbb4), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x14);
  CYC(b_+6, b_+9); SET_BC(SYM(encodeSecretData));
  CYC(b_+9, b_+11); TAIL(cutscene19_strikeFlameWithLightning);
}

void cutscene19_state6_hook(GB *gb) {
  BASE(cutscene19_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decTmpcbb4_hook, SYM(decTmpcbb4), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x28);
  CYC(b_+6, b_+9); SET_BC((SYM(secretFunctionCaller_body__jumpTable) + 2));
  CYC(b_+9, b_+11); TAIL(cutscene19_strikeFlameWithLightning);
}

void cutscene19_state7_hook(GB *gb) {
  BASE(cutscene19_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decTmpcbb4_hook, SYM(decTmpcbb4), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x78);
  CYC(b_+6, b_+8); A = 0x67;
  CALL_C(b_+8, playSound_b00_hook, SYM(playSound_b00), b_+11);
  CYC(b_+11, b_+14); TAIL(incCutsceneState);
}

void cutscene19_state8_hook(GB *gb) {
  BASE(cutscene19_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setScreenShakeCounterTo255_hook, SYM(setScreenShakeCounterTo255), b_+3);
  CYC(b_+3, b_+6); A = W8(wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (F & FZ) {
    CALL_C_CC(b_+8, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+11);
  } else {
    CYC(b_+8, b_+11);
  }
  CALL_C(b_+11, decTmpcbb4_hook, SYM(decTmpcbb4), b_+14);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x04;
  CALL_C(b_+17, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+20);
  CYC(b_+20, b_+22); A = 0xb4;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+28); TAIL(incCutsceneState);
}

void cutscene19_state9_hook(GB *gb) {
  BASE(cutscene19_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setScreenShakeCounterTo255_hook, SYM(setScreenShakeCounterTo255), b_+3);
  CYC(b_+3, b_+6); A = W8(wPaletteThread_mode);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CALL_C(b_+8, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+11);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+16); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+21); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+21, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x04);
  CYC(b_+26, b_+28); A = 0xf0;
  CYC(b_+28, b_+31); TAIL(playSound_b00);
}

void intro_runStage_hook(GB *gb) {
  BASE(intro_runStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wThreadStateBuffer + 6);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(intro_japaneseOnlyScreen)) {
      TAIL(intro_japaneseOnlyScreen);
    }
    else if (jt_ == SYM(intro_capcomScreen)) {
      TAIL(intro_capcomScreen);
    }
    else if (jt_ == SYM(intro_cinematic)) {
      TAIL(intro_cinematic);
    }
    else if (jt_ == SYM(intro_titlescreen)) {
      TAIL(intro_titlescreen);
    }
    else if (jt_ == SYM(intro_restart)) {
      TAIL(intro_restart);
    }
    else {
      HANDOFF(HL);
      return;
    }
  } while (0);
}

static void runIntro_nextStage(GB *gb) {
  BASE(runIntro);
  CYC(b_+32, b_+34); A = mem_rd(gb, hIntroInputsEnabled);
  CYC(b_+34, b_+35); alu_add(gb, A);
  if (F & FZ) {
    CYCT(b_+35, b_+37);
    TAIL(intro_runStage);
  }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+40); A = mem_rd(gb, wThreadStateBuffer + 6);
  CYC(b_+40, b_+42); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+44);
    TAIL(intro_gotoTitlescreen);
  }
  CYC(b_+42, b_+44);
  intro_runStage_hook(gb);
}

void runIntro_hook(GB *gb) {
  BASE(runIntro);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hSerialInterruptBehaviour);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    CALL_C(b_+15, serialFunc_0c85_hook, SYM(serialFunc_0c85), b_+18);
    CYC(b_+18, b_+20); A = 0x03;
    CYC(b_+20, b_+22); mem_wr(gb, hFFBE, A);
    CYC(b_+22, b_+23); alu_xor(gb, A);
    CYC(b_+23, b_+25); mem_wr(gb, hSerialLinkState, A);
    CYC(b_+25, b_+28); A = mem_rd(gb, wKeysJustPressed);
    CYC(b_+28, b_+30); alu_and(gb, 0x08);
    if (F & FZ) {
      CYCT(b_+30, b_+32);
      TAIL(intro_runStage);
    }
    CYC(b_+30, b_+32);
  } else {
    CYC(b_+3, b_+5);
    CALL_C(b_+5, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+8);
    CYC(b_+8, b_+10); A = 0x09;
    CYC(b_+10, b_+13); mem_wr(gb, wTmpcbb4, A);
    CYC(b_+13, b_+15);
  }
  runIntro_nextStage(gb);
}

static void intro_goto_titlescreen_after_call(GB *gb) {
  BASE(intro_gotoTitlescreen);
  CYC(b_+6, b_+9); SET_HL(wThreadStateBuffer + 7);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+11, b_+13); mem_wr(gb, hCameraY, A);
  CYC(b_+13, b_+16); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x03);
  CYC(b_+18, b_+19); A = alu_dec8(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+22, b_+24);
  intro_runStage_hook(gb);
}

void intro_gotoTitlescreen_hook(GB *gb) {
  BASE(intro_gotoTitlescreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearPaletteFadeVariables_hook, SYM(clearPaletteFadeVariables), b_+3);
  CALL_C(b_+3, cutscene_clearObjects_hook, SYM(cutscene_clearObjects), b_+6);
  intro_goto_titlescreen_after_call(gb);
}

void intro_gotoTitlescreen__afterCall4d09_hook(GB *gb) {
  intro_goto_titlescreen_after_call(gb);
}

void intro_restart_hook(GB *gb) {
  BASE(intro_restart);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, (wThreadStateBuffer + 6), A);
  CYC(b_+4, b_+7); mem_wr(gb, (wThreadStateBuffer + 7), A);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void intro_gotoNextStage_hook(GB *gb) {
  BASE(intro_gotoNextStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enableIntroInputs_hook, SYM(enableIntroInputs), b_+3);
  CALL_C(b_+3, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+6);
  CYC(b_+6, b_+9); SET_HL((wThreadStateBuffer + 6));
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, b_+16); TAIL(clearPaletteFadeVariables);
}

void intro_incState_hook(GB *gb) {
  BASE(intro_incState);
  CYC(b_+0, b_+3); SET_HL((wThreadStateBuffer + 7));
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void intro_capcomScreen__state0_hook(GB *gb) {
  BASE(intro_capcomScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+10, restartSound_hook, SYM(restartSound), b_+13);
  CALL_C(b_+13, clearVram_hook, SYM(clearVram), b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  CALL_C(b_+18, loadGfxHeader_hook, SYM(loadGfxHeader), b_+21);
  CYC(b_+21, b_+23); A = 0x01;
  CALL_C(b_+23, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+26);
  CYC(b_+26, b_+29); SET_HL(wTmpcbb3);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0xd0);
  CYC(b_+31, b_+32); SET_HL(HL + 1);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x00);
  CALL_C(b_+34, intro_incState_hook, SYM(intro_incState), b_+37);
  CALL_C(b_+37, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+40);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+44); TAIL(loadGfxRegisterStateIndex);
}

void intro_capcomScreen__state1_hook(GB *gb) {
  BASE(intro_capcomScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+44, b_+47); SET_HL(wTmpcbb3);
  CALL_C(b_+47, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+50);
  if (!(F & FZ)) {
    CYCT(b_+50, b_+51); ret_effect(gb);
    return;
  }
  CYC(b_+50, b_+51);
  CALL_C(b_+51, intro_incState_hook, SYM(intro_incState), b_+54);
  CYC(b_+54, b_+57); TAIL(fadeoutToWhite);
}

void intro_capcomScreen__state2_hook(GB *gb) {
  BASE(intro_capcomScreen);
  CYC(b_+57, b_+60); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+61, b_+62); ret_effect(gb);
    return;
  }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+63); alu_xor(gb, A);
  CYC(b_+63, b_+66); SET_HL(wThreadStateBuffer + 6);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x02);
  CYC(b_+68, b_+69); L = alu_inc8(gb, L);
  CYC(b_+69, b_+70); mem_wr(gb, HL, A);
  CYC(b_+70, b_+73); mem_wr(gb, wIntro_cinematicState, A);
  CYC(b_+73, b_+76); TAIL(enableIntroInputs);
}

void intro_capcomScreen_hook(GB *gb) {
  BASE(intro_capcomScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wThreadStateBuffer + 7);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == b_+10) {
      TAIL(intro_capcomScreen__state0);
    }
    else if (jt_ == b_+44) {
      TAIL(intro_capcomScreen__state1);
    }
    else if (jt_ == b_+57) {
      TAIL(intro_capcomScreen__state2);
    }
    else {
      HANDOFF(HL);
      return;
    }
  } while (0);
}

void intro_japaneseOnlyScreen_hook(GB *gb) {
  BASE(intro_japaneseOnlyScreen);
  CYC(b_+0, b_+3); SET_HL(wThreadStateBuffer + 6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(intro_capcomScreen);
}

void intro_titlescreen__runState_hook(GB *gb) {
  BASE(intro_titlescreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+31, b_+34); A = mem_rd(gb, wThreadStateBuffer + 7);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(intro_titlescreen_state0)) {
      TAIL(intro_titlescreen_state0);
    }
    else if (jt_ == SYM(intro_titlescreen_state1)) {
      TAIL(intro_titlescreen_state1);
    }
    else if (jt_ == SYM(intro_titlescreen_state2)) {
      TAIL(intro_titlescreen_state2);
    }
    else if (jt_ == SYM(intro_titlescreen_state3)) {
      TAIL(intro_titlescreen_state3);
    }
    else {
      HANDOFF(HL);
      return;
    }
  } while (0);
}

void intro_titlescreen_hook(GB *gb) {
  BASE(intro_titlescreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CALL_C(b_+3, intro_titlescreen__runState_hook, b_+31, b_+6);
  CALL_C(b_+6, clearOam_hook, SYM(clearOam), b_+9);
  CYC(b_+9, b_+12); SET_HL((SYM(endgameCutsceneHandler_20__func_5953) + 10));
  CYC(b_+12, b_+14); E = 0x3f;
  CALL_C(b_+14, addSpritesFromBankToOam_hook, SYM(addSpritesFromBankToOam), b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+20, b_+22); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); SET_HL((SYM(endgameCutsceneHandler_20__func_5995) + 21));
  CYC(b_+26, b_+28); E = 0x3f;
  CYC(b_+28, b_+31); TAIL(addSpritesFromBankToOam);
}

void intro_titlescreen_state0_hook(GB *gb) {
  BASE(intro_titlescreen_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, restartSound_hook, SYM(restartSound), b_+3);
  CYC(b_+3, b_+5); A = 0xe8;
  CALL_C(b_+5, threadStop_hook, SYM(threadStop), b_+8);
  CALL_C(b_+8, stopTextThread_hook, SYM(stopTextThread), b_+11);
  CALL_C(b_+11, disableLcd_hook, SYM(disableLcd), b_+14);
  CYC(b_+14, b_+16); A = 0x02;
  CALL_C(b_+16, loadGfxHeader_hook, SYM(loadGfxHeader), b_+19);
  CYC(b_+19, b_+21); A = 0x03;
  CALL_C(b_+21, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+24);
  CYC(b_+24, b_+27); SET_HL(wTmpcbb3);
  CYC(b_+27, b_+29); A = 0x60;
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+32); A = 0x09;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(b_+33, intro_incState_hook, SYM(intro_incState), b_+36);
  CYC(b_+36, b_+38); A = 0x01;
  CALL_C(b_+38, playSound_b00_hook, SYM(playSound_b00), b_+41);
  CYC(b_+41, b_+43); A = 0x04;
  CYC(b_+43, b_+46); TAIL(loadGfxRegisterStateIndex);
}

void intro_titlescreen_state1__gotoState_hook(GB *gb) {
  BASE(intro_titlescreen_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+28, b_+31); mem_wr(gb, wThreadStateBuffer + 7, A);
  CYC(b_+31, b_+33); A = 0xfa;
  CALL_C(b_+33, playSound_b00_hook, SYM(playSound_b00), b_+36);
  CYC(b_+36, b_+39); TAIL(fadeoutToWhite);
}

void intro_titlescreen_state1__pressedStart_hook(GB *gb) {
  BASE(intro_titlescreen_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+18, b_+20); A = 0x56;
  CALL_C(b_+20, playSound_b00_hook, SYM(playSound_b00), b_+23);
  CALL_C(b_+23, disableSerialPort_hook, SYM(disableSerialPort), b_+26);
  CYC(b_+26, b_+28); A = 0x03;
  TAIL(intro_titlescreen_state1__gotoState);
}

void intro_titlescreen_state1_hook(GB *gb) {
  BASE(intro_titlescreen_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+3, b_+5); alu_and(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    TAIL(intro_titlescreen_state1__pressedStart);
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wTmpcbb3);
  CALL_C(b_+10, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+13);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+14); ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); A = 0x02;
  CYC(b_+16, b_+18);
  TAIL(intro_titlescreen_state1__gotoState);
}

void intro_titlescreen_state2_hook(GB *gb) {
  BASE(intro_titlescreen_state2);
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); TAIL(intro_gotoNextStage);
}

void intro_titlescreen_state3_hook(GB *gb) {
  BASE(intro_titlescreen_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0xe8;
  CYC(b_+7, b_+10); SET_BC(SYM(fileSelectThreadStart));
  CALL_C(b_+10, threadRestart_hook, SYM(threadRestart), b_+13);
  CYC(b_+13, b_+16); TAIL(stubThreadStart);
}

void runIntroCinematic_hook(GB *gb) {
  BASE(runIntroCinematic);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wIntro_cinematicState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(introCinematic_ridingHorse)) {
      TAIL(introCinematic_ridingHorse);
    }
    else if (jt_ == SYM(introCinematic_inTemple)) {
      TAIL(introCinematic_inTemple);
    }
    else if (jt_ == SYM(introCinematic_preTitlescreen)) {
      TAIL(introCinematic_preTitlescreen);
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}

void introCinematic_ridingHorse_hook(GB *gb) {
  BASE(introCinematic_ridingHorse);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+O(3)); A = mem_rd(gb, wThreadStateBuffer + 7);
  CYC(b_+O(3), b_+O(4)); push_effect(gb, b_+O(4));
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(introCinematic_ridingHorse_state0) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state0))) { introCinematic_ridingHorse_state0_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state1) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state1))) { introCinematic_ridingHorse_state1_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state2) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state2))) { introCinematic_ridingHorse_state2_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state3) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state3))) { introCinematic_ridingHorse_state3_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state4) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state4))) { introCinematic_ridingHorse_state4_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state5) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state5))) { introCinematic_ridingHorse_state5_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state6) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state6))) { introCinematic_ridingHorse_state6_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state7) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state7))) { introCinematic_ridingHorse_state7_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state8) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state8))) { introCinematic_ridingHorse_state8_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state9) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state9))) { introCinematic_ridingHorse_state9_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_ridingHorse_state10) && hook_enabled_at(gb, SYM(introCinematic_ridingHorse_state10))) { introCinematic_ridingHorse_state10_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void introCinematic_inTemple_hook(GB *gb) {
  BASE(introCinematic_inTemple);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+O(3)); A = mem_rd(gb, wThreadStateBuffer + 7);
  CYC(b_+O(3), b_+O(4)); push_effect(gb, b_+O(4));
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(introCinematic_inTemple_state0) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state0))) { introCinematic_inTemple_state0_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state1) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state1))) { introCinematic_inTemple_state1_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state2) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state2))) { introCinematic_inTemple_state2_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state3) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state3))) { introCinematic_inTemple_state3_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state4) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state4))) { introCinematic_inTemple_state4_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state5) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state5))) { introCinematic_inTemple_state5_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state6) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state6))) { introCinematic_inTemple_state6_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state7) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state7))) { introCinematic_inTemple_state7_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state8) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state8))) { introCinematic_inTemple_state8_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state9) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state9))) { introCinematic_inTemple_state9_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_inTemple_state10) && hook_enabled_at(gb, SYM(introCinematic_inTemple_state10))) { introCinematic_inTemple_state10_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void introCinematic_preTitlescreen_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wThreadStateBuffer + 7);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(introCinematic_preTitlescreen_state0) && hook_enabled_at(gb, SYM(introCinematic_preTitlescreen_state0))) { introCinematic_preTitlescreen_state0_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_preTitlescreen_state1) && hook_enabled_at(gb, SYM(introCinematic_preTitlescreen_state1))) { introCinematic_preTitlescreen_state1_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_preTitlescreen_state2) && hook_enabled_at(gb, SYM(introCinematic_preTitlescreen_state2))) { introCinematic_preTitlescreen_state2_hook(gb); return; }
    else if (jt_ == SYM(introCinematic_preTitlescreen_state3) && hook_enabled_at(gb, SYM(introCinematic_preTitlescreen_state3))) { introCinematic_preTitlescreen_state3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void introCinematic_ridingHorse_state0_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CYC(b_+3, b_+6); SET_HL(wOamEnd);
  CYC(b_+6, b_+9); SET_BC(0x0460);
  CALL_C(b_+9, clearMemoryBc_hook, SYM(clearMemoryBc), b_+12);
  CYC(b_+12, b_+14); A = 0x04;
  CYC(b_+14, b_+16); hram_wr(gb, 0x70, A);
  CYC(b_+16, b_+19); SET_HL(w4TileMap);
  CYC(b_+19, b_+22); SET_BC(0x0120);
  CALL_C(b_+22, clearMemoryBc_hook, SYM(clearMemoryBc), b_+25);
  CYC(b_+25, b_+28); SET_HL(w4AttributeMap);
  CYC(b_+28, b_+31); SET_BC(0x0120);
  CALL_C(b_+31, clearMemoryBc_hook, SYM(clearMemoryBc), b_+34);
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+38); hram_wr(gb, 0x70, A);
  CALL_C(b_+38, clearOam_hook, SYM(clearOam), b_+41);
  CYC(b_+41, b_+43); A = 0x10;
  CYC(b_+43, b_+45); mem_wr(gb, hOamTail, A);
  CYC(b_+45, b_+47); A = 0x9b;
  CALL_C(b_+47, loadGfxHeader_hook, SYM(loadGfxHeader), b_+50);
  CYC(b_+50, b_+52); A = 0x90;
  CALL_C(b_+52, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+55);
  CYC(b_+55, b_+58); SET_HL(wTmpcbb3);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x5e);
  CYC(b_+60, b_+61); SET_HL(HL + 1);
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x01);
  CYC(b_+63, b_+65); A = 0x20;
  CYC(b_+65, b_+68); mem_wr(gb, wTmpcbb8, A);
  CYC(b_+68, b_+70); A = 0x10;
  CYC(b_+70, b_+73); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+73, b_+75); A = 0x22;
  CYC(b_+75, b_+78); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+78, b_+79); alu_xor(gb, A);
  CYC(b_+79, b_+82); mem_wr(gb, wTmpcbba, A);
  CYC(b_+82, b_+84); A = 0x3f;
  CALL_C(b_+84, playSound_b00_hook, SYM(playSound_b00), b_+87);
  CYC(b_+87, b_+89); A = 0x0b;
  CALL_C(b_+89, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+92);
  CYC(b_+92, b_+95); SET_HL(wLockBG7Color3ToBlack);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x01);
  CYC(b_+97, b_+100); SET_HL((SYM(init) + 55));
  CALL_C(b_+100, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+103);
  CYC(b_+103, b_+105); A = 0x17;
  CALL_C(b_+105, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+108);
  CYC(b_+108, b_+111); A = mem_rd(gb, wGfxRegs2_LCDC);
  CYC(b_+111, b_+114); mem_wr(gb, wGfxRegs6_LCDC, A);
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CYC(b_+115, b_+117); mem_wr(gb, hCameraX, A);
  CYC(b_+117, b_+120); TAIL(intro_incState);
}

void introCinematic_ridingHorse_state1_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_moveBlackBarsIn_hook, SYM(introCinematic_moveBlackBarsIn), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb3);
  CALL_C(b_+6, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x06);
  CALL_C(b_+12, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+15);
  CYC(b_+15, b_+17); A = 0x06;
  CYC(b_+17, b_+19); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+19, b_+22); TAIL(intro_incState);
}

void introCinematic_ridingHorse_updateScrollingGround_hook(GB *gb);

void introCinematic_ridingHorse_state2_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_ridingHorse_updateScrollingGround_hook, SYM(introCinematic_ridingHorse_updateScrollingGround), b_+3);
  CALL_C(b_+3, decCbb3_hook, SYM(decCbb3), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x06);
  CYC(b_+9, b_+12); SET_HL(wGfxRegs2_SCY);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); mem_wr(gb, hCameraY, A);
  CYC(b_+16, b_+18); alu_cp(gb, 0x48);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); A = 0x7e;
  CYC(b_+21, b_+24); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+24, b_+27); TAIL(intro_incState);
}

void introCinematic_ridingHorse_updateScrollingGround_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_updateScrollingGround);
  CYC(b_+0, b_+2); A = 0xa8;
  CYC(b_+2, b_+5); SET_HL(wGfxRegs2_SCY);
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); A = 0xc7;
  }
  CYC(b_+12, b_+15); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+19); SET_HL(wGfxRegs6_SCY);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+23); A = mem_rd(gb, wIntro_frameCounter);
  CYC(b_+23, b_+25); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+26); ret_effect(gb);
    return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+28); ret_effect(gb);
}

void introCinematic_ridingHorse_state3_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_ridingHorse_updateScrollingGround_hook, SYM(introCinematic_ridingHorse_updateScrollingGround), b_+3);
  CALL_C(b_+3, decCbb3_hook, SYM(decCbb3), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x20);
  CYC(b_+9, b_+10); SET_HL(HL + 1);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CYC(b_+12, b_+14); A = 0x96;
  CALL_C(b_+14, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+17);
  CYC(b_+17, b_+19); A = 0x38;
  CALL_C(b_+19, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+22);
  CYC(b_+22, b_+24); A = 0x18;
  CYC(b_+24, b_+27); mem_wr(gb, wTmpcbba, A);
  CALL_C(b_+27, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+30);
  CYC(b_+30, b_+31); alu_xor(gb, A);
  CYC(b_+31, b_+33); mem_wr(gb, hCameraY, A);
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbbc, A);
  CYC(b_+36, b_+39); SET_BC((SYM(func_03_7493__cbb3_00) + 29));
  CALL_C(b_+39, createInteraction_hook, SYM(createInteraction), b_+42);
  CYC(b_+42, b_+44); A = 0x0d;
  CYC(b_+44, b_+47); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+47, b_+49); A = 0x3c;
  CYC(b_+49, b_+52); mem_wr(gb, wTmpcbbb, A);
  CYC(b_+52, b_+54); A = 0x03;
  CYC(b_+54, b_+56); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+56, b_+59); TAIL(intro_incState);
}

void introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen_hook(GB *gb);
void introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(GB *gb);
void introCinematic_ridingHorse_drawTempleSprites_hook(GB *gb);

void introCinematic_ridingHorse_state4_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_L(b_+0, introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen_hook, b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb3);
  CALL_C(b_+6, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x36;
  CALL_C(b_+12, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x5a);
  CYC(b_+17, b_+19); A = 0x9b;
  CALL_C(b_+19, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+22);
  CALL_C(b_+22, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+25);
  CALL_C(b_+25, clearOam_hook, SYM(clearOam), b_+28);
  CYC(b_+28, b_+30); A = 0x19;
  CALL_C(b_+30, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+33);
  CYC(b_+33, b_+35); A = 0x48;
  CYC(b_+35, b_+38); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(b_+38, b_+41); mem_wr(gb, wGfxRegs2_WINY, A);
  CYC(b_+41, b_+44); TAIL(intro_incState);
}

void introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+44, b_+47); SET_HL((SYM(endgameCutsceneHandler_20__stateD) + 48));
  CYC(b_+47, b_+49); E = 0x3f;
  CALL_C(b_+49, addSpritesFromBankToOam_hook, SYM(addSpritesFromBankToOam), b_+52);
  CYC(b_+52, b_+55); A = mem_rd(gb, wIntro_frameCounter);
  CYC(b_+55, b_+57); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(b_+57, b_+59);
  } else {
    CYC(b_+57, b_+59);
    CYC(b_+59, b_+62); SET_HL(wGfxRegs1_SCX);
    CYC(b_+62, b_+63); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+63, b_+66); SET_HL(wTmpcbb6);
  CYC(b_+66, b_+67); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+67, b_+69);
  } else {
    CYC(b_+67, b_+69);
    CYC(b_+69, b_+71); mem_wr(gb, HL, 0x0d);
    CYC(b_+71, b_+74); SET_HL(wGfxRegs2_SCX);
    CYC(b_+74, b_+75); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+75, b_+78); SET_HL(wTmpcbbb);
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+79, b_+80); ret_effect(gb);
    return;
  }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x3c);
  CYC(b_+82, b_+83); SET_HL(HL + 1);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+86); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+86, b_+87); ret_effect(gb);
    return;
  }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+88, b_+91); SET_HL(b_+96);
  CYC(b_+91, b_+92); push_effect(gb, b_+92);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+96); TAIL(loadPaletteHeader);
}

void introCinematic_ridingHorse_state5_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_moveBlackBarsOut_hook, SYM(introCinematic_moveBlackBarsOut), b_+3);
  CYC(b_+3, b_+6); SET_HL(wGfxRegs2_SCX);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_add(gb, 0x08);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(b_+12, b_+13); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x60);
  CALL_C(b_+15, intro_incState_hook, SYM(intro_incState), b_+18);
  CYC(b_+18, b_+21); SET_HL(wTmpcbb3);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x18);
  CYC(b_+23, b_+26); SET_BC((SYM(func_03_7493__cbb3_00) + 30));
  CYC(b_+26, b_+29); TAIL(createInteraction);
}

void introCinematic_ridingHorse_state6_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CALL_C(b_+3, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, disableLcd_hook, SYM(disableLcd), b_+10);
  CYC(b_+10, b_+12); A = 0x92;
  CALL_C(b_+12, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+15);
  CYC(b_+15, b_+17); A = 0x9c;
  CALL_C(b_+17, loadGfxHeader_hook, SYM(loadGfxHeader), b_+20);
  CALL_C(b_+20, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+23);
  CYC(b_+23, b_+25); A = 0x0a;
  CALL_C(b_+25, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+28);
  CYC(b_+28, b_+31); TAIL(intro_incState);
}

void introCinematic_ridingHorse_state7_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wGfxRegs1_SCY);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    TAIL(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2);
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0xcc;
  CYC(b_+8, b_+11); mem_wr(gb, wTmpcbb6, A);
  CALL_C(b_+11, intro_incState_hook, SYM(intro_incState), SYM(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2));
  TAIL(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2);
}

void introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2);
  CYC(b_+0, b_+3); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hOamTail, A);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); SET_HL((SYM(endgameCutsceneHandler_20__state9) + 18));
  CYC(b_+13, b_+15); E = 0x3f;
  CYC(b_+15, b_+18); TAIL(addSpritesFromBankToOam_withOffset);
}

void introCinematic_ridingHorse_state8_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    TAIL(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2);
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x93;
  CALL_C(b_+8, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+11);
  CALL_C(b_+11, disableLcd_hook, SYM(disableLcd), b_+14);
  CALL_C(b_+14, clearOam_hook, SYM(clearOam), b_+17);
  CYC(b_+17, b_+19); A = 0x10;
  CYC(b_+19, b_+21); mem_wr(gb, hOamTail, A);
  CYC(b_+21, b_+23); A = 0x9d;
  CALL_C(b_+23, loadGfxHeader_hook, SYM(loadGfxHeader), b_+26);
  CYC(b_+26, b_+28); A = 0x05;
  CYC(b_+28, b_+31); mem_wr(gb, wTmpcbbb, A);
  CYC(b_+31, b_+34); SET_HL(wTmpcbb3);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x90);
  CYC(b_+36, b_+37); SET_HL(HL + 1);
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x01);
  CYC(b_+39, b_+41); A = 0xb4;
  CYC(b_+41, b_+44); mem_wr(gb, wTmpcbb6, A);
  CALL_C(b_+44, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+47);
  CYC(b_+47, b_+49); A = 0x0b;
  CALL_C(b_+49, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+52);
  CALL_C(b_+52, introCinematic_ridingHorse_drawTempleSprites_hook, SYM(introCinematic_ridingHorse_drawTempleSprites), b_+55);
  CYC(b_+55, b_+57); B = 0x02;
  for (;;) {
    CALL_C(b_+57, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+60);
    if (!(F & FZ)) {
      CYCT(b_+60, b_+62);
      break;
    }
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+64); mem_wr(gb, HL, 0x75);
    CYC(b_+64, b_+65); L = alu_inc8(gb, L);
    CYC(b_+65, b_+66); mem_wr(gb, HL, B);
    CYC(b_+66, b_+67); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+67, b_+69);
      continue;
    }
    CYC(b_+67, b_+69);
    break;
  }
  CYC(b_+69, b_+72); TAIL(intro_incState);
}

void introCinematic_ridingHorse_state9_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CALL_C(b_+3, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CALL_C(b_+8, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+11);
    CALL_C(b_+11, intro_incState_hook, SYM(intro_incState), b_+14);
    CYC(b_+14, b_+16);
    TAIL(introCinematic_ridingHorse_drawTempleSprites);
  }
  CYC(b_+16, b_+19); SET_HL(wTmpcbb6);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+21, b_+23);
    TAIL(introCinematic_ridingHorse_drawTempleSprites);
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+27); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+28, b_+30);
    TAIL(introCinematic_ridingHorse_drawTempleSprites);
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+33); SET_HL(wTmpcbbb);
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+34, b_+36);
    TAIL(introCinematic_ridingHorse_drawTempleSprites);
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x05);
  CYC(b_+38, b_+41); SET_HL(wGfxRegs1_SCX);
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  TAIL(introCinematic_ridingHorse_drawTempleSprites);
}

void introCinematic_ridingHorse_drawTempleSprites_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_drawTempleSprites);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hOamTail, A);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(b_+7, b_+8); alu_cpl(gb);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); SET_HL((SYM(endgameCutsceneHandler_20__stateD) + 27));
  CYC(b_+13, b_+15); E = 0x3f;
  CYC(b_+15, b_+18); TAIL(addSpritesFromBankToOam_withOffset);
}

void introCinematic_ridingHorse_state10_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_state10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    TAIL(introCinematic_ridingHorse_drawTempleSprites);
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+9);
  CYC(b_+9, b_+11); TAIL(incIntroCinematicState);
}

void introCinematic_preTitlescreen_updateScrollingTree_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen_updateScrollingTree);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wTmpcbba);
  CYC(b_+8, b_+11); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+11, b_+14); SET_HL(wGfxRegs1_SCY);
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_cp(gb, 0x88);
  if (F & FZ) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
    CYC(b_+44, b_+46); alu_cp(gb, 0xb0);
    if (!(F & FZ)) {
      CYCT(b_+46, b_+48);
    } else {
      CYC(b_+46, b_+48);
      CYC(b_+48, b_+50); A = 0x2a;
      CALL_C(b_+50, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+53);
    }
    CYC(b_+53, b_+55); alu_or(gb, 0x01);
    CYC(b_+55, b_+56); ret_effect(gb);
    return;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x0d;
  CALL_C(b_+25, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+28);
  CYC(b_+28, b_+30); B = 0x04;
  for (;;) {
    CALL_C(b_+30, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+33);
    if (!(F & FZ)) {
      CYCT(b_+33, b_+35);
      break;
    }
    CYC(b_+33, b_+35);
    CYC(b_+35, b_+37); mem_wr(gb, HL, 0xd2);
    CYC(b_+37, b_+38); L = alu_inc8(gb, L);
    CYC(b_+38, b_+39); B = alu_dec8(gb, B);
    CYC(b_+39, b_+40); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(b_+40, b_+42);
      continue;
    }
    CYC(b_+40, b_+42);
    CYC(b_+42, b_+44);
    break;
  }
  CYC(b_+53, b_+55); alu_or(gb, 0x01);
  CYC(b_+55, b_+56); ret_effect(gb);
}

void introCinematic_inTemple_state0__nextTriforce_hook(GB *gb);
void introCinematic_inTemple_state0__doneSpawningTriforce_hook(GB *gb);
void introCinematic_inTemple_updateWave_hook(GB *gb);

void introCinematic_inTemple_state0_hook(GB *gb) {
  BASE(introCinematic_inTemple_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CALL_C(b_+3, clearOam_hook, SYM(clearOam), b_+6);
  CYC(b_+6, b_+8); A = 0x10;
  CYC(b_+8, b_+10); mem_wr(gb, hOamTail, A);
  CYC(b_+10, b_+12); A = 0x9e;
  CALL_C(b_+12, loadGfxHeader_hook, SYM(loadGfxHeader), b_+15);
  CYC(b_+15, b_+17); A = 0x91;
  CALL_C(b_+17, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+20);
  CYC(b_+20, b_+22); A = 0x09;
  CALL_C(b_+22, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+28, b_+30); mem_wr(gb, hCameraY, A);
  CYC(b_+30, b_+32); A = GV(0x10, 0x18);
  CYC(b_+32, b_+35); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(b_+35, loadAnimationData_hook, SYM(loadAnimationData), b_+38);
  CYC(b_+38, b_+40); A = 0x01;
  CYC(b_+40, b_+43); mem_wr(gb, wScrollMode, A);
  CYC(b_+43, b_+45); A = 0x08;
  CALL_C(b_+45, setLinkID_hook, SYM(setLinkID), b_+48);
  CYC(b_+48, b_+50); L = 0x00;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x01);
  CYC(b_+52, b_+54); L = 0x0b;
  CYC(b_+54, b_+57); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+57, b_+59); alu_add(gb, 0x60);
  CYC(b_+59, b_+60); mem_wr(gb, HL, A);
  CYC(b_+60, b_+62); L = 0x0d;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x50);
  CYC(b_+64, b_+67); SET_HL(GV((SYM(func_7098) + 2), 0x54a8));
  CYC(b_+67, b_+69); A = GV(0x10, 0x03);
  CALL_C(b_+69, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+72);
  CYC(b_+72, b_+74); B = 0x03;
  CYC(b_+74, b_+76); C = 0x30;
  TAIL(introCinematic_inTemple_state0__nextTriforce);
}

void introCinematic_inTemple_state0__nextTriforce_hook(GB *gb) {
  BASE(introCinematic_inTemple_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(b_+76, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+79);
    if (!(F & FZ)) {
      CYCT(b_+79, b_+81);
      TAIL(introCinematic_inTemple_state0__doneSpawningTriforce);
    }
    CYC(b_+79, b_+81);
    CYC(b_+81, b_+83); mem_wr(gb, HL, 0x4a);
    CYC(b_+83, b_+84); L = alu_inc8(gb, L);
    CYC(b_+84, b_+85); A = B;
    CYC(b_+85, b_+86); A = alu_dec8(gb, A);
    CYC(b_+86, b_+87); mem_wr(gb, HL, A);
    CYC(b_+87, b_+89); L = 0x4b;
    CYC(b_+89, b_+91); mem_wr(gb, HL, 0x19);
    CYC(b_+91, b_+92); A = C;
    CYC(b_+92, b_+94); L = 0x4d;
    CYC(b_+94, b_+95); mem_wr(gb, HL, A);
    CYC(b_+95, b_+97); alu_add(gb, 0x20);
    CYC(b_+97, b_+98); C = A;
    CYC(b_+98, b_+99); A = C;
    CYC(b_+99, b_+100); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+100, b_+102);
      continue;
    }
    CYC(b_+100, b_+102);
    TAIL(introCinematic_inTemple_state0__doneSpawningTriforce);
  }
}

void introCinematic_inTemple_state0__doneSpawningTriforce_hook(GB *gb) {
  BASE(introCinematic_inTemple_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+102, b_+105); SET_HL(wMenuDisabled);
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x01);
  CALL_C(b_+107, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+110);
  CYC(b_+110, b_+111); alu_xor(gb, A);
  CYC(b_+111, b_+114); mem_wr(gb, wIntro_triforceState, A);
  CYC(b_+114, b_+117); TAIL(intro_incState);
}

void introCinematic_inTemple_state1_hook(GB *gb) {
  BASE(introCinematic_inTemple_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wUseSimulatedInput);
  CYC(b_+8, b_+9); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+9, b_+12);
    TAIL(introCinematic_inTemple_updateCamera);
  }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(b_+16, introCinematic_inTemple_updateCamera_hook, SYM(introCinematic_inTemple_updateCamera), b_+19);
  CYC(b_+19, b_+22); TAIL(intro_incState);
}

void introCinematic_inTemple_state2_hook(GB *gb) {
  BASE(introCinematic_inTemple_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wIntro_triforceState);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+9);
  CYC(b_+9, b_+12); TAIL(intro_incState);
}

void introCinematic_inTemple_state3_hook(GB *gb) {
  BASE(introCinematic_inTemple_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+14); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+14, b_+16); A = 0x00;
  CYC(b_+16, b_+18); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+18, b_+20); A = 0x20;
  CALL_C(b_+20, initWaveScrollValues_hook, SYM(initWaveScrollValues), b_+23);
  CALL_C(b_+23, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+26);
  CALL_C(b_+26, intro_incState_hook, SYM(intro_incState), SYM(introCinematic_inTemple_updateWave));
  TAIL(introCinematic_inTemple_updateWave);
}

void introCinematic_inTemple_updateWave_hook(GB *gb) {
  BASE(introCinematic_inTemple_updateWave);
  CYC(b_+0, b_+3); SET_HL(wFrameCounter);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); A = 0x02;
  CYC(b_+6, b_+9); TAIL(loadBigBufferScrollValues);
}

void introCinematic_inTemple_state4_hook(GB *gb) {
  BASE(introCinematic_inTemple_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(wTmpcbb6);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x78);
  CYC(b_+13, b_+16); TAIL(intro_incState);
}

void introCinematic_inTemple_state5_hook(GB *gb) {
  BASE(introCinematic_inTemple_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wTmpcbba, A);
  CALL_C(b_+15, intro_incState_hook, SYM(intro_incState), SYM(introCinematic_inTemple_state6));
  TAIL(introCinematic_inTemple_state6);
}

void flashScreen_body_hook(GB *gb);
void clearFadingPalettes_body_hook(GB *gb);
void introCinematic_preTitlescreen_updateScrollForTitle_hook(GB *gb);

void introCinematic_inTemple_state6_hook(GB *gb) {
  BASE(introCinematic_inTemple_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb6);
  CYC(b_+6, b_+8); B = 0x00;
  CALL_C(b_+8, flashScreen_body_hook, SYM(flashScreen_body), b_+11);
  if (F & FZ) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+15);
  CYC(b_+15, b_+17); A = 0x06;
  CYC(b_+17, b_+20); mem_wr(gb, wIntro_triforceState, A);
  CYC(b_+20, b_+22); A = 0x91;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+28); TAIL(intro_incState);
}

void introCinematic_inTemple_state7_hook(GB *gb) {
  BASE(introCinematic_inTemple_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wIntro_triforceState);
  CYC(b_+6, b_+8); alu_cp(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, clearLinkObject_hook, SYM(clearLinkObject), b_+12);
  CYC(b_+12, b_+14); B = 0x08;
  CALL_C(b_+14, func_2d48_hook, SYM(func_2d48), b_+17);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+21, b_+24); TAIL(intro_incState);
}

void introCinematic_inTemple_state8_hook(GB *gb) {
  BASE(introCinematic_inTemple_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x3c);
  CYC(b_+10, b_+13); TAIL(intro_incState);
}

void introCinematic_inTemple_state9_hook(GB *gb) {
  BASE(introCinematic_inTemple_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcbb6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = 0xb4;
  CALL_C(b_+10, playSound_b00_hook, SYM(playSound_b00), b_+13);
  CALL_C(b_+13, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+16);
  CYC(b_+16, b_+19); TAIL(intro_incState);
}

void introCinematic_inTemple_state10_hook(GB *gb) {
  BASE(introCinematic_inTemple_state10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_inTemple_updateWave_hook, SYM(introCinematic_inTemple_updateWave), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CALL_C(b_+8, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+11);
  CYC(b_+11, b_+14); TAIL(incIntroCinematicState);
}

void flashScreen_body_hook(GB *gb) {
  BASE(flashScreen_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+2, b_+3); B = mem_rd(gb, HL);
  CYC(b_+3, b_+6); SET_HL(SYM(screenFlashingData));
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); C = 0;
  for (;;) {
    CYC(b_+12, b_+13); A = mem_rd(gb, HL);
    CYC(b_+13, b_+15); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(b_+15, b_+16); ret_effect(gb);
      return;
    }
    CYC(b_+15, b_+16);
    CYC(b_+16, b_+17); alu_cp(gb, B);
    if (!(F & FC)) {
      CYCT(b_+17, b_+19);
      break;
    }
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+20); SET_HL(HL + 1);
    CYC(b_+20, b_+21); C = alu_inc8(gb, C);
    CYC(b_+21, b_+23);
  }
  CYC(b_+23, b_+24); A = C;
  CYC(b_+24, b_+26); alu_and(gb, 0x01);
  CYC(b_+26, b_+27); C = A;
  CYC(b_+27, b_+30); A = mem_rd(gb, wTmpcbba);
  CYC(b_+30, b_+31); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+31, b_+32); ret_effect(gb);
    return;
  }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+33); A = C;
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbba, A);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+37, b_+39);
    TAIL(clearFadingPalettes_body);
  }
  CYC(b_+37, b_+39);
  CALL_C(b_+39, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+42);
  CYC(b_+42, b_+43); alu_xor(gb, A);
  CYC(b_+43, b_+44); ret_effect(gb);
}

void clearFadingPalettes_body_hook(GB *gb) {
  BASE(clearFadingPalettes_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+6); B = 0x80;
  CYC(b_+6, b_+9); SET_HL(w2FadingBgPalettes);
  CYC(b_+9, b_+11); A = 0xff;
  CALL_C(b_+11, fillMemory_hook, SYM(fillMemory), b_+14);
  CYC(b_+14, b_+16); A = 0xff;
  CYC(b_+16, b_+18); mem_wr(gb, hSprPaletteSources, A);
  CYC(b_+18, b_+20); mem_wr(gb, hBgPaletteSources, A);
  CYC(b_+20, b_+22); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+22, b_+24); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+27); hram_wr(gb, 0x70, A);
  CYC(b_+27, b_+28); ret_effect(gb);
}

static void intro_pretitle_state0_after_call(GB *gb, uint16_t sp0_) {
  BASE(introCinematic_preTitlescreen_state0);
  CYC(b_+21, b_+23); A = 0x0a;
  CALL_C(b_+23, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+26);
  CALL_C(b_+26, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+29);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+31);
  } else {
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+33); mem_wr(gb, HL, 0x4a);
    CYC(b_+33, b_+34); L = alu_inc8(gb, L);
    CYC(b_+34, b_+36); mem_wr(gb, HL, 0x08);
    CYC(b_+36, b_+38); L = 0x4a;
    CYC(b_+38, b_+40); A = 0x60;
    CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+42, b_+44); A = 0x3d;
    CYC(b_+44, b_+45); L = alu_inc8(gb, L);
    CYC(b_+45, b_+46); mem_wr(gb, HL, A); SET_HL(HL + 1);
  }
  CYC(b_+46, b_+48); B = 0x08;
  for (;;) {
    CALL_C(b_+48, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+51);
    if (!(F & FZ)) {
      CYCT(b_+51, b_+53);
      break;
    }
    CYC(b_+51, b_+53);
    CYC(b_+53, b_+55); mem_wr(gb, HL, 0xd3);
    CYC(b_+55, b_+56); L = alu_inc8(gb, L);
    CYC(b_+56, b_+57); B = alu_dec8(gb, B);
    CYC(b_+57, b_+58); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(b_+58, b_+60);
    } else {
      CYC(b_+58, b_+60);
      break;
    }
  }
  CYC(b_+60, b_+62); A = 0x03;
  CYC(b_+62, b_+65); mem_wr(gb, wTmpcbba, A);
  CYC(b_+65, b_+68); mem_wr(gb, wTmpcbb6, A);
  CALL_C(b_+68, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+71);
  CYC(b_+71, b_+72); alu_xor(gb, A);
  CYC(b_+72, b_+74); mem_wr(gb, hCameraY, A);
  CYC(b_+74, b_+76); A = 0x40;
  CALL_C(b_+76, playSound_b00_hook, SYM(playSound_b00), b_+79);
  CYC(b_+79, b_+82);
  intro_incState_hook(gb);
}

void introCinematic_preTitlescreen_state0_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CYC(b_+3, b_+5); A = 0xff;
  CYC(b_+5, b_+8); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+8, b_+10); A = 0x9f;
  CALL_C(b_+10, loadGfxHeader_hook, SYM(loadGfxHeader), b_+13);
  CYC(b_+13, b_+15); A = 0x94;
  CALL_C(b_+15, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+18);
  CALL_C(b_+18, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+21);
  intro_pretitle_state0_after_call(gb, sp0_);
}

void introCinematic_preTitlescreen_state0__afterCall52da_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  intro_pretitle_state0_after_call(gb, sp0_);
}

void introCinematic_preTitlescreen_state1_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, introCinematic_preTitlescreen_updateScrollingTree_hook, SYM(introCinematic_preTitlescreen_updateScrollingTree), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, intro_incState_hook, SYM(intro_incState), b_+7);
  CYC(b_+7, b_+10); SET_HL(wTmpcbb3);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x02);
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+18); SET_HL(wTmpcbb6);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x10);
  CYC(b_+20, b_+21); A = alu_inc8(gb, A);
  CYC(b_+21, b_+24); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+28); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+32); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+32, b_+35); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+35, b_+37); B = 0x90;
  CYC(b_+37, b_+40); SET_HL(wBigBuffer);
  do {
    CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+41, b_+42); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+42, b_+44);
    } else {
      CYC(b_+42, b_+44);
      break;
    }
  } while (true);
  CYC(b_+44, b_+46); A = 0x01;
  TAIL(introCinematic_preTitlescreen_updateScrollForTitle);
}

void introCinematic_preTitlescreen_updateScrollForTitle_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen_updateScrollForTitle);
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+2); alu_xor(gb, A);
  CYC(b_+2, b_+3); C = A;
  for (;;) {
    CYC(b_+3, b_+4); C = alu_inc8(gb, C);
    CYC(b_+4, b_+5); alu_add(gb, B);
    CYC(b_+5, b_+7); alu_cp(gb, 0x18);
    if (F & FZ) {
      CYCT(b_+7, b_+9);
      break;
    }
    CYC(b_+7, b_+9);
    if (!(F & FC)) {
      CYCT(b_+9, b_+10); ret_effect(gb);
      return;
    }
    CYC(b_+9, b_+10);
    CYC(b_+10, b_+12);
  }
  CYC(b_+12, b_+13); push_effect(gb, BC);
  CYC(b_+13, b_+15); A = 0x38;
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CYC(b_+16, b_+18); H = 0xc3;
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+20); alu_xor(gb, A);
  do {
    CYC(b_+20, b_+21); push_effect(gb, AF);
    CYC(b_+21, b_+22); alu_sub(gb, L);
    CYC(b_+22, b_+24); alu_add(gb, 0x58);
    CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+25, b_+26); SET_AF(pop_effect(gb));
    CYC(b_+26, b_+27); alu_add(gb, C);
    CYC(b_+27, b_+28); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+28, b_+30);
    } else {
      CYC(b_+28, b_+30);
      break;
    }
  } while (true);
  CYC(b_+30, b_+31); SET_BC(pop_effect(gb));
  CYC(b_+31, b_+33); A = 0x37;
  CYC(b_+33, b_+34); alu_add(gb, B);
  CYC(b_+34, b_+35); L = A;
  CYC(b_+35, b_+37); A = 0x2f;
  do {
    CYC(b_+37, b_+38); push_effect(gb, AF);
    CYC(b_+38, b_+39); alu_sub(gb, L);
    CYC(b_+39, b_+41); alu_add(gb, 0x58);
    CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(b_+42, b_+43); SET_AF(pop_effect(gb));
    CYC(b_+43, b_+44); alu_sub(gb, C);
    CYC(b_+44, b_+45); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+45, b_+47);
    } else {
      CYC(b_+45, b_+47);
      break;
    }
  } while (true);
  CYC(b_+47, b_+48); ret_effect(gb);
}

void introCinematic_preTitlescreen_state2_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb6);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); A = alu_dec8(gb, A);
    CYC(b_+8, b_+9); mem_wr(gb, HL, A);
    CYC(b_+9, b_+11); A = 0xab;
    if (F & FZ) CALL_C_CC(b_+11, playSound_b00_hook, SYM(playSound_b00), b_+14);
    else CYC(b_+11, b_+14);
  }
  CYC(b_+14, b_+17); A = mem_rd(gb, wIntro_frameCounter);
  CYC(b_+17, b_+19); alu_and(gb, 0x01);
  CYC(b_+19, b_+22); SET_HL(wTmpcbb4);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(b_+26, b_+28);
    CYC(b_+38, b_+39); alu_xor(gb, A);
    CYC(b_+39, b_+42); mem_wr(gb, wTmpcbb6, A);
    CYC(b_+42, b_+43); A = alu_dec8(gb, A);
    CYC(b_+43, b_+46); mem_wr(gb, wTmpcbba, A);
    CYC(b_+46, b_+49); intro_incState_hook(gb);
    return;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+33); SET_HL(SYM(introCinematic_preTitlescreen_titleSizeData));
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+38); TAIL(introCinematic_preTitlescreen_updateScrollForTitle);
}

void introCinematic_preTitlescreen_state3_hook(GB *gb) {
  BASE(introCinematic_preTitlescreen_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb6);
  CYC(b_+3, b_+5); B = 0x01;
  CALL_C(b_+5, flashScreen_body_hook, SYM(flashScreen_body), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12);
  TAIL(intro_gotoTitlescreen);
}

void introCinematic_inTemple_updateCamera_hook(GB *gb) {
  BASE(introCinematic_inTemple_updateCamera);
  CYC(b_+0, b_+3); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); SET_DE(w1Link_yh);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_sub(gb, B);
  CYC(b_+9, b_+11); alu_sub(gb, 0x40);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+15); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+18); alu_cp(gb, 0x70);
  if (!(F & FC)) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+22); mem_wr(gb, wGfxRegs1_SCY, A);
  CYC(b_+22, b_+24); mem_wr(gb, hCameraY, A);
  CYC(b_+24, b_+25); ret_effect(gb);
}

void introCinematic_moveBlackBarsIn_hook(GB *gb) {
  BASE(introCinematic_moveBlackBarsIn);
  CYC(b_+0, b_+3); SET_HL(wGfxRegs1_LYC);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x17);
  if (F & FC) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); mem_wr(gb, HL, 0x17);
  }
  CYC(b_+12, b_+15); SET_HL(wGfxRegs2_WINY);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_cp(gb, 0x78);
  if (!(F & FC)) {
    CYCT(b_+20, b_+21); ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x78);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void introCinematic_moveBlackBarsOut_hook(GB *gb) {
  BASE(introCinematic_moveBlackBarsOut);
  CYC(b_+0, b_+3); SET_HL(wGfxRegs1_LYC);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x2f);
  if (!(F & FC)) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); mem_wr(gb, HL, 0x2f);
  }
  CYC(b_+12, b_+15); SET_HL(wGfxRegs2_WINY);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(b_+20, b_+21); ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x60);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void clearFadingPalettes2_hook(GB *gb) {
  BASE(clearFadingPalettes2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); hram_wr(gb, 0x70, A);
  CYC(b_+4, b_+7); SET_HL(w2FadingBgPalettes);
  CYC(b_+7, b_+9); B = 0x80;
  CALL_C(b_+9, clearMemory_hook, SYM(clearMemory), b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); hram_wr(gb, 0x70, A);
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  CYC(b_+16, b_+18); mem_wr(gb, hSprPaletteSources, A);
  CYC(b_+18, b_+20); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+20, b_+22); A = 0xfd;
  CYC(b_+22, b_+24); mem_wr(gb, hBgPaletteSources, A);
  CYC(b_+24, b_+26); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void cutscene_clearObjects_hook(GB *gb) {
  BASE(cutscene_clearObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+3);
  CALL_C(b_+3, clearLinkObject_hook, SYM(clearLinkObject), b_+6);
  CYC(b_+6, b_+9); TAIL(refreshObjectGfx);
}

void endgameCutsceneHandler_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+O(3)); SET_HL(wCutsceneState);
  CYC(b_+O(3), b_+O(5)); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+O(5), b_+O(7));
  } else {
    CYC(b_+O(5), b_+O(7));
    CYC(b_+O(7), b_+O(8)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+O(8), b_+O(11)); SET_HL(wTmpcbb3);
    CYC(b_+O(11), b_+O(13)); B = 0x10;
    CALL_C(b_+O(13), clearMemory_hook, SYM(clearMemory), b_+O(16));
  }
  CYC(b_+O(16), b_+O(17)); A = E;
  CYC(b_+O(17), b_+O(18)); push_effect(gb, b_+O(18));
  do { uint16_t jt_ = (intro_jumpTable(gb));
    if (jt_ == SYM(endgameCutsceneHandler_09)) {
      TAIL(endgameCutsceneHandler_09);
    }
    else if (jt_ == SYM(endgameCutsceneHandler_0a)) {
      TAIL(endgameCutsceneHandler_0a);
    }
    else if (jt_ == SYM(endgameCutsceneHandler_0f)) {
      TAIL(endgameCutsceneHandler_0f);
    }
    else if (jt_ == SYM(endgameCutsceneHandler_20)) {
      TAIL(endgameCutsceneHandler_20);
    }
    else {
      HANDOFF(HL);
      return;
    }
  } while (0);
}
