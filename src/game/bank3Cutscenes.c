#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

void incCutsceneState_hook(GB *gb) {
  CYC(0x4b10, 0x4b13); SET_HL(wCutsceneState);
  CYC(0x4b13, 0x4b14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b14, 0x4b15); ret_effect(gb);
}

void unused_incTmpcbb3_hook(GB *gb) {
  CYC(0x4b15, 0x4b18); SET_HL(wTmpcbb3);
  CYC(0x4b18, 0x4b19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b19, 0x4b1a); ret_effect(gb);
}

void decTmpcbb4_hook(GB *gb) {
  CYC(0x4b1a, 0x4b1d); SET_HL(wTmpcbb4);
  CYC(0x4b1d, 0x4b1e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4b1e, 0x4b1f); ret_effect(gb);
}

void setScreenShakeCounterTo255_hook(GB *gb) {
  CYC(0x4b1f, 0x4b21); A = 0xff;
  CYC(0x4b21, 0x4b24);
  setScreenShakeCounter_hook(gb);
}

void twinrovaCutscene_createLightningStrike_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4cba, getFreePartSlot_hook, 0x3e8e, 0x4cbd);
  if (!(F & FZ)) {
    CYCT(0x4cbd, 0x4cbe); ret_effect(gb);
    return;
  }
  CYC(0x4cbd, 0x4cbe);
  CYC(0x4cbe, 0x4cc0); mem_wr(gb, HL, 0x27);
  CYC(0x4cc0, 0x4cc1); L = alu_inc8(gb, L);
  CYC(0x4cc1, 0x4cc2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4cc2, 0x4cc4); L = 0xcb;
  CYC(0x4cc4, 0x4cc5); mem_wr(gb, HL, B);
  CYC(0x4cc5, 0x4cc6); L = alu_inc8(gb, L);
  CYC(0x4cc6, 0x4cc7); L = alu_inc8(gb, L);
  CYC(0x4cc7, 0x4cc8); mem_wr(gb, HL, C);
  CYC(0x4cc8, 0x4cc9); ret_effect(gb);
}

void incIntroCinematicState_hook(GB *gb) {
  CYC(0x50ca, 0x50cd); SET_HL(wIntro_cinematicState);
  CYC(0x50cd, 0x50ce); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x50ce, 0x50cf); alu_xor(gb, A);
  CYC(0x50cf, 0x50d2); mem_wr(gb, wThreadStateBuffer + 7, A);
  CYC(0x50d2, 0x50d3); ret_effect(gb);
}

void createInteraction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x540c, getFreeInteractionSlot_hook, 0x3aef, 0x540f);
  if (!(F & FZ)) {
    CYCT(0x540f, 0x5410); ret_effect(gb);
    return;
  }
  CYC(0x540f, 0x5410);
  CYC(0x5410, 0x5411); mem_wr(gb, HL, B);
  CYC(0x5411, 0x5412); L = alu_inc8(gb, L);
  CYC(0x5412, 0x5413); mem_wr(gb, HL, C);
  CYC(0x5413, 0x5414); ret_effect(gb);
}

static void twinrovaCutscene_deleteAllInteractionsExceptFlames_delete(GB *gb) {
  CYC(0x4c23, 0x4c24); L = alu_dec8(gb, L);
  CYC(0x4c24, 0x4c26); B = 0x40;
  CYC(0x4c26, 0x4c29); clearMemory_hook(gb);
}

void twinrovaCutscene_deleteAllInteractionsExceptFlames_hook(GB *gb);
void twinrovaCutscene_loadAngryFlames_hook(GB *gb);

void twinrovaCutscene_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b24, 0x4b26); A = 0x04;
  CALL_C(0x4b26, fadeoutToWhiteWithDelay_hook, 0x3257, 0x4b29);
  CYC(0x4b29, 0x4b2c); SET_HL(wTmpcbb3);
  CYC(0x4b2c, 0x4b2e); B = 0x10;
  CALL_C(0x4b2e, clearMemory_hook, 0x046f, 0x4b31);
  CYC(0x4b31, 0x4b33); incCutsceneState_hook(gb);
}

void twinrovaCutscene_fadeinToRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4b6f, disableLcd_hook, 0x02c1, 0x4b72);
  CALL_C(0x4b72, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x4b75);
  CALL_C(0x4b75, loadScreenMusicAndSetRoomPack_hook, 0x341a, 0x4b78);
  CALL_C(0x4b78, loadTilesetData_hook, 0x3889, 0x4b7b);
  CALL_C(0x4b7b, loadTilesetGraphics_hook, 0x3796, 0x4b7e);
  CYC(0x4b7e, 0x4b81); func_131f_hook(gb);
}

void twinrovaCutscene_state2_hook(GB *gb) {
  CYC(0x4b91, 0x4b94); A = W8(wPaletteThread_mode);
  CYC(0x4b94, 0x4b95); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4b95, 0x4b96); ret_effect(gb);
    return;
  }
  CYC(0x4b95, 0x4b96);
  CYC(0x4b96, 0x4b98); A = 0x01;
  CYC(0x4b98, 0x4b9b); mem_wr(gb, wTmpcbb4, A);
  CYC(0x4b9b, 0x4b9e); incCutsceneState_hook(gb);
}

void twinrovaCutscene_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4b9e, decTmpcbb4_hook, 0x4b1a, 0x4ba1);
  if (!(F & FZ)) {
    CYCT(0x4ba1, 0x4ba2); ret_effect(gb);
    return;
  }
  CYC(0x4ba1, 0x4ba2);
  CYC(0x4ba2, 0x4ba4); mem_wr(gb, HL, 180);
  CALL_C(0x4ba4, twinrovaCutscene_deleteAllInteractionsExceptFlames_hook, 0x4c0d, 0x4ba7);
  CALL_C(0x4ba7, twinrovaCutscene_loadAngryFlames_hook, 0x4c29, 0x4baa);
  CYC(0x4baa, 0x4bac); A = 0xb0;
  CALL_C(0x4bac, playSound_b00_hook, 0x0c98, 0x4baf);
  CYC(0x4baf, 0x4bb2); incCutsceneState_hook(gb);
}

void cutscene18_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4bb2, setScreenShakeCounterTo255_hook, 0x4b1f, 0x4bb5);
  CYC(0x4bb5, 0x4bb8); A = W8(wFrameCounter);
  CYC(0x4bb8, 0x4bba); alu_and(gb, 0x3f);
  if (!(F & FZ)) {
    CYCT(0x4bba, 0x4bbc);
  } else {
    CYC(0x4bba, 0x4bbc);
    CYC(0x4bbc, 0x4bbe); A = 0xb0;
    CALL_C(0x4bbe, playSound_b00_hook, 0x0c98, 0x4bc1);
  }
  CALL_C(0x4bc1, decTmpcbb4_hook, 0x4b1a, 0x4bc4);
  if (!(F & FZ)) {
    CYCT(0x4bc4, 0x4bc5); ret_effect(gb);
    return;
  }
  CYC(0x4bc4, 0x4bc5);
  CYC(0x4bc5, 0x4bc7); A = 0x04;
  CALL_C(0x4bc7, fadeoutToWhiteWithDelay_hook, 0x3257, 0x4bca);
  CYC(0x4bca, 0x4bcd); incCutsceneState_hook(gb);
}

void cutscene18_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4bcd, setScreenShakeCounterTo255_hook, 0x4b1f, 0x4bd0);
  CYC(0x4bd0, 0x4bd3); A = W8(wPaletteThread_mode);
  CYC(0x4bd3, 0x4bd4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4bd4, 0x4bd5); ret_effect(gb);
    return;
  }
  CYC(0x4bd4, 0x4bd5);
  CYC(0x4bd5, 0x4bd7); A = 0xf5;
  CYC(0x4bd7, 0x4bda); mem_wr(gb, wActiveRoom, A);
  CALL_C(0x4bda, twinrovaCutscene_fadeinToRoom_hook, 0x4b6f, 0x4bdd);
  CALL_C(0x4bdd, getFreeEnemySlot_hook, 0x2e27, 0x4be0);
  CYC(0x4be0, 0x4be2); mem_wr(gb, HL, 0x03);
  CYC(0x4be2, 0x4be4); L = 0x83;
  CYC(0x4be4, 0x4be6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x4be6, 0x4be9); SET_HL(w1Link_enabled);
  CYC(0x4be9, 0x4beb); mem_wr(gb, HL, 0x03);
  CYC(0x4beb, 0x4bed); L = w1Link_yh & 0xff;
  CYC(0x4bed, 0x4bef); mem_wr(gb, HL, 0x78);
  CYC(0x4bef, 0x4bf0); L = alu_inc8(gb, L);
  CYC(0x4bf0, 0x4bf1); L = alu_inc8(gb, L);
  CYC(0x4bf1, 0x4bf3); mem_wr(gb, HL, 0x78);
  CALL_C(0x4bf3, resetCamera_hook, 0x12ce, 0x4bf6);
  CYC(0x4bf6, 0x4bf8); A = 0x01;
  CYC(0x4bf8, 0x4bfb); mem_wr(gb, 0xc2ef, A);
  CYC(0x4bfb, 0x4bfd); A = 0x01;
  CYC(0x4bfd, 0x4c00); mem_wr(gb, wScrollMode, A);
  CALL_C(0x4c00, loadCommonGraphics_hook, 0x1a98, 0x4c03);
  CYC(0x4c03, 0x4c05); A = 0x02;
  CALL_C(0x4c05, fadeinFromWhiteWithDelay_hook, 0x3284, 0x4c08);
  CYC(0x4c08, 0x4c0a); A = 0x02;
  CYC(0x4c0a, 0x4c0d); loadGfxRegisterStateIndex_hook(gb);
}

void twinrovaCutscene_deleteAllInteractionsExceptFlames_hook(GB *gb) {
  CYC(0x4c0d, 0x4c10); SET_HL(0xd240);
  for (;;) {
    CYC(0x4c10, 0x4c12); L = 0x40;
    CYC(0x4c12, 0x4c13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4c13, 0x4c14); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x4c14, 0x4c16);
    } else {
      CYC(0x4c14, 0x4c16);
      CYC(0x4c16, 0x4c17); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x4c17, 0x4c19); alu_cp(gb, 0xa9);
      if (F & FZ) {
        CYCT(0x4c19, 0x4c1c); push_effect(gb, 0x4c1c);
        twinrovaCutscene_deleteAllInteractionsExceptFlames_delete(gb);
      } else {
        CYC(0x4c19, 0x4c1c);
      }
    }
    CYC(0x4c1c, 0x4c1d); H = alu_inc8(gb, H);
    CYC(0x4c1d, 0x4c1e); A = H;
    CYC(0x4c1e, 0x4c20); alu_cp(gb, 0xe0);
    if (F & FC) {
      CYCT(0x4c20, 0x4c22);
      continue;
    }
    CYC(0x4c20, 0x4c22);
    CYC(0x4c22, 0x4c23); ret_effect(gb);
    return;
  }
}

void twinrovaCutscene_loadAngryFlames_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c29, 0x4c2b); A = 0xaf;
  CALL_C(0x4c2b, loadPaletteHeader_hook, 0x050b, 0x4c2e);
  CYC(0x4c2e, 0x4c31); SET_HL(0x402f);
  CYC(0x4c31, 0x4c34); parseGivenObjectData_b00_hook(gb);
}

void cutscene19_strikeFlameWithLightning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c55, twinrovaCutscene_createLightningStrike_hook, 0x4cba, 0x4c58);
  CYC(0x4c58, 0x4c5b); incCutsceneState_hook(gb);
}

void cutscene19_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c4c, decTmpcbb4_hook, 0x4b1a, 0x4c4f);
  if (!(F & FZ)) {
    CYCT(0x4c4f, 0x4c50); ret_effect(gb);
    return;
  }
  CYC(0x4c4f, 0x4c50);
  CYC(0x4c50, 0x4c52); mem_wr(gb, HL, 0x14);
  CYC(0x4c52, 0x4c55); SET_BC(0x1878);
  cutscene19_strikeFlameWithLightning_hook(gb);
}

void cutscene19_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c5b, decTmpcbb4_hook, 0x4b1a, 0x4c5e);
  if (!(F & FZ)) {
    CYCT(0x4c5e, 0x4c5f); ret_effect(gb);
    return;
  }
  CYC(0x4c5e, 0x4c5f);
  CYC(0x4c5f, 0x4c61); mem_wr(gb, HL, 0x14);
  CYC(0x4c61, 0x4c64); SET_BC(0x48a8);
  CYC(0x4c64, 0x4c66); cutscene19_strikeFlameWithLightning_hook(gb);
}

void cutscene19_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c66, decTmpcbb4_hook, 0x4b1a, 0x4c69);
  if (!(F & FZ)) {
    CYCT(0x4c69, 0x4c6a); ret_effect(gb);
    return;
  }
  CYC(0x4c69, 0x4c6a);
  CYC(0x4c6a, 0x4c6c); mem_wr(gb, HL, 0x28);
  CYC(0x4c6c, 0x4c6f); SET_BC(0x4848);
  CYC(0x4c6f, 0x4c71); cutscene19_strikeFlameWithLightning_hook(gb);
}

void cutscene19_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c71, decTmpcbb4_hook, 0x4b1a, 0x4c74);
  if (!(F & FZ)) {
    CYCT(0x4c74, 0x4c75); ret_effect(gb);
    return;
  }
  CYC(0x4c74, 0x4c75);
  CYC(0x4c75, 0x4c77); mem_wr(gb, HL, 0x78);
  CYC(0x4c77, 0x4c79); A = 0x67;
  CALL_C(0x4c79, playSound_b00_hook, 0x0c98, 0x4c7c);
  CYC(0x4c7c, 0x4c7f); incCutsceneState_hook(gb);
}

void cutscene19_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c7f, setScreenShakeCounterTo255_hook, 0x4b1f, 0x4c82);
  CYC(0x4c82, 0x4c85); A = W8(wFrameCounter);
  CYC(0x4c85, 0x4c87); alu_and(gb, 0x07);
  if (F & FZ) {
    CALL_C_CC(0x4c87, fastFadeinFromWhite_hook, 0x3290, 0x4c8a);
  } else {
    CYC(0x4c87, 0x4c8a);
  }
  CALL_C(0x4c8a, decTmpcbb4_hook, 0x4b1a, 0x4c8d);
  if (!(F & FZ)) {
    CYCT(0x4c8d, 0x4c8e); ret_effect(gb);
    return;
  }
  CYC(0x4c8d, 0x4c8e);
  CYC(0x4c8e, 0x4c90); A = 0x04;
  CALL_C(0x4c90, fadeoutToWhiteWithDelay_hook, 0x3257, 0x4c93);
  CYC(0x4c93, 0x4c95); A = 0xb4;
  CALL_C(0x4c95, playSound_b00_hook, 0x0c98, 0x4c98);
  CYC(0x4c98, 0x4c9b); incCutsceneState_hook(gb);
}

void cutscene19_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c9b, setScreenShakeCounterTo255_hook, 0x4b1f, 0x4c9e);
  CYC(0x4c9e, 0x4ca1); A = W8(wPaletteThread_mode);
  CYC(0x4ca1, 0x4ca2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4ca2, 0x4ca3); ret_effect(gb);
    return;
  }
  CYC(0x4ca2, 0x4ca3);
  CALL_C(0x4ca3, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x4ca6);
  CYC(0x4ca6, 0x4ca8); A = 0x01;
  CYC(0x4ca8, 0x4cab); mem_wr(gb, 0xc2ef, A);
  CYC(0x4cab, 0x4cad); A = 0x01;
  CYC(0x4cad, 0x4cb0); mem_wr(gb, wScrollMode, A);
  CALL_C(0x4cb0, getFreeEnemySlot_hook, 0x2e27, 0x4cb3);
  CYC(0x4cb3, 0x4cb5); mem_wr(gb, HL, 0x04);
  CYC(0x4cb5, 0x4cb7); A = 0xf0;
  CYC(0x4cb7, 0x4cba); playSound_b00_hook(gb);
}

void intro_restart_hook(GB *gb) {
  CYC(0x4d1b, 0x4d1c); alu_xor(gb, A);
  CYC(0x4d1c, 0x4d1f); mem_wr(gb, 0xc2e6, A);
  CYC(0x4d1f, 0x4d22); mem_wr(gb, 0xc2e7, A);
  CYC(0x4d22, 0x4d23); ret_effect(gb);
}

void intro_gotoNextStage_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4d23, enableIntroInputs_hook, 0x0881, 0x4d26);
  CALL_C(0x4d26, clearDynamicInteractions_hook, 0x35d2, 0x4d29);
  CYC(0x4d29, 0x4d2c); SET_HL(0xc2e6);
  CYC(0x4d2c, 0x4d2d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d2d, 0x4d2e); L = alu_inc8(gb, L);
  CYC(0x4d2e, 0x4d30); mem_wr(gb, HL, 0x00);
  CYC(0x4d30, 0x4d33); clearPaletteFadeVariables_hook(gb);
}

void intro_incState_hook(GB *gb) {
  CYC(0x4d33, 0x4d36); SET_HL(0xc2e7);
  CYC(0x4d36, 0x4d37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d37, 0x4d38); ret_effect(gb);
}

void intro_titlescreen_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4db3, restartSound_hook, 0x0cb2, 0x4db6);
  CYC(0x4db6, 0x4db8); A = 0xe8;
  CALL_C(0x4db8, threadStop_hook, 0x089b, 0x4dbb);
  CALL_C(0x4dbb, stopTextThread_hook, 0x184d, 0x4dbe);
  CALL_C(0x4dbe, disableLcd_hook, 0x02c1, 0x4dc1);
  CYC(0x4dc1, 0x4dc3); A = 0x02;
  CALL_C(0x4dc3, loadGfxHeader_hook, 0x0626, 0x4dc6);
  CYC(0x4dc6, 0x4dc8); A = 0x03;
  CALL_C(0x4dc8, loadPaletteHeader_hook, 0x050b, 0x4dcb);
  CYC(0x4dcb, 0x4dce); SET_HL(wTmpcbb3);
  CYC(0x4dce, 0x4dd0); A = 0x60;
  CYC(0x4dd0, 0x4dd1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4dd1, 0x4dd3); A = 0x09;
  CYC(0x4dd3, 0x4dd4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(0x4dd4, intro_incState_hook, 0x4d33, 0x4dd7);
  CYC(0x4dd7, 0x4dd9); A = 0x01;
  CALL_C(0x4dd9, playSound_b00_hook, 0x0c98, 0x4ddc);
  CYC(0x4ddc, 0x4dde); A = 0x04;
  CYC(0x4dde, 0x4de1); loadGfxRegisterStateIndex_hook(gb);
}

void intro_titlescreen_state1__gotoState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4dfd, 0x4e00); mem_wr(gb, wThreadStateBuffer + 7, A);
  CYC(0x4e00, 0x4e02); A = 0xfa;
  CALL_C(0x4e02, playSound_b00_hook, 0x0c98, 0x4e05);
  CYC(0x4e05, 0x4e08); fadeoutToWhite_hook(gb);
}

void intro_titlescreen_state1__pressedStart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4df3, 0x4df5); A = 0x56;
  CALL_C(0x4df5, playSound_b00_hook, 0x0c98, 0x4df8);
  CALL_C(0x4df8, disableSerialPort_hook, 0x0c7e, 0x4dfb);
  CYC(0x4dfb, 0x4dfd); A = 0x03;
  intro_titlescreen_state1__gotoState_hook(gb);
}

void intro_titlescreen_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4de1, 0x4de4); A = mem_rd(gb, wKeysJustPressed);
  CYC(0x4de4, 0x4de6); alu_and(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x4de6, 0x4de8);
    intro_titlescreen_state1__pressedStart_hook(gb);
    return;
  }
  CYC(0x4de6, 0x4de8);
  CYC(0x4de8, 0x4deb); SET_HL(wTmpcbb3);
  CALL_C(0x4deb, decHlRef16WithCap_hook, 0x0237, 0x4dee);
  if (!(F & FZ)) {
    CYCT(0x4dee, 0x4def); ret_effect(gb);
    return;
  }
  CYC(0x4dee, 0x4def);
  CYC(0x4def, 0x4df1); A = 0x02;
  CYC(0x4df1, 0x4df3);
  intro_titlescreen_state1__gotoState_hook(gb);
}

void intro_titlescreen_state2_hook(GB *gb) {
  CYC(0x4e08, 0x4e0b); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x4e0b, 0x4e0c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4e0c, 0x4e0d); ret_effect(gb);
    return;
  }
  CYC(0x4e0c, 0x4e0d);
  CYC(0x4e0d, 0x4e10); intro_gotoNextStage_hook(gb);
}

void introCinematic_ridingHorse_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e44, disableLcd_hook, 0x02c1, 0x4e47);
  CYC(0x4e47, 0x4e4a); SET_HL(wOamEnd);
  CYC(0x4e4a, 0x4e4d); SET_BC(0x0460);
  CALL_C(0x4e4d, clearMemoryBc_hook, 0x0475, 0x4e50);
  CYC(0x4e50, 0x4e52); A = 0x04;
  CYC(0x4e52, 0x4e54); hram_wr(gb, 0x70, A);
  CYC(0x4e54, 0x4e57); SET_HL(w4TileMap);
  CYC(0x4e57, 0x4e5a); SET_BC(0x0120);
  CALL_C(0x4e5a, clearMemoryBc_hook, 0x0475, 0x4e5d);
  CYC(0x4e5d, 0x4e60); SET_HL(w4AttributeMap);
  CYC(0x4e60, 0x4e63); SET_BC(0x0120);
  CALL_C(0x4e63, clearMemoryBc_hook, 0x0475, 0x4e66);
  CYC(0x4e66, 0x4e68); A = 0x01;
  CYC(0x4e68, 0x4e6a); hram_wr(gb, 0x70, A);
  CALL_C(0x4e6a, clearOam_hook, 0x049f, 0x4e6d);
  CYC(0x4e6d, 0x4e6f); A = 0x10;
  CYC(0x4e6f, 0x4e71); hram_wr(gb, 0x9f, A);
  CYC(0x4e71, 0x4e73); A = 0x9b;
  CALL_C(0x4e73, loadGfxHeader_hook, 0x0626, 0x4e76);
  CYC(0x4e76, 0x4e78); A = 0x90;
  CALL_C(0x4e78, loadPaletteHeader_hook, 0x050b, 0x4e7b);
  CYC(0x4e7b, 0x4e7e); SET_HL(wTmpcbb3);
  CYC(0x4e7e, 0x4e80); mem_wr(gb, HL, 0x5e);
  CYC(0x4e80, 0x4e81); SET_HL(HL + 1);
  CYC(0x4e81, 0x4e83); mem_wr(gb, HL, 0x01);
  CYC(0x4e83, 0x4e85); A = 0x20;
  CYC(0x4e85, 0x4e88); mem_wr(gb, wTmpcbb8, A);
  CYC(0x4e88, 0x4e8a); A = 0x10;
  CYC(0x4e8a, 0x4e8d); mem_wr(gb, wTmpcbb9, A);
  CYC(0x4e8d, 0x4e8f); A = 0x22;
  CYC(0x4e8f, 0x4e92); mem_wr(gb, wTmpcbb6, A);
  CYC(0x4e92, 0x4e93); alu_xor(gb, A);
  CYC(0x4e93, 0x4e96); mem_wr(gb, wTmpcbba, A);
  CYC(0x4e96, 0x4e98); A = 0x3f;
  CALL_C(0x4e98, playSound_b00_hook, 0x0c98, 0x4e9b);
  CYC(0x4e9b, 0x4e9d); A = 0x0b;
  CALL_C(0x4e9d, fadeinFromWhiteWithDelay_hook, 0x3284, 0x4ea0);
  CYC(0x4ea0, 0x4ea3); SET_HL(wLockBG7Color3ToBlack);
  CYC(0x4ea3, 0x4ea5); mem_wr(gb, HL, 0x01);
  CYC(0x4ea5, 0x4ea8); SET_HL(0x4037);
  CALL_C(0x4ea8, parseGivenObjectData_b00_hook, 0x3171, 0x4eab);
  CYC(0x4eab, 0x4ead); A = 0x17;
  CALL_C(0x4ead, loadGfxRegisterStateIndex_hook, 0x02ea, 0x4eb0);
  CYC(0x4eb0, 0x4eb3); A = mem_rd(gb, wGfxRegs2_LCDC);
  CYC(0x4eb3, 0x4eb6); mem_wr(gb, wGfxRegs6_LCDC, A);
  CYC(0x4eb6, 0x4eb7); alu_xor(gb, A);
  CYC(0x4eb7, 0x4eb9); hram_wr(gb, 0xac, A);
  CYC(0x4eb9, 0x4ebc); intro_incState_hook(gb);
}

void introCinematic_ridingHorse_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4ebc, introCinematic_moveBlackBarsIn, 0x53d3, 0x4ebf);
  CYC(0x4ebf, 0x4ec2); SET_HL(wTmpcbb3);
  CALL_C(0x4ec2, decHlRef16WithCap_hook, 0x0237, 0x4ec5);
  if (!(F & FZ)) {
    CYCT(0x4ec5, 0x4ec6); ret_effect(gb);
    return;
  }
  CYC(0x4ec5, 0x4ec6);
  CYC(0x4ec6, 0x4ec8); mem_wr(gb, HL, 0x06);
  CALL_C(0x4ec8, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x4ecb);
  CYC(0x4ecb, 0x4ecd); A = 0x06;
  CYC(0x4ecd, 0x4ecf); hram_wr(gb, 0x9d, A);
  CYC(0x4ecf, 0x4ed2); intro_incState_hook(gb);
}

void introCinematic_ridingHorse_updateScrollingGround_hook(GB *gb);

void introCinematic_ridingHorse_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4ed2, introCinematic_ridingHorse_updateScrollingGround_hook, 0x4eed, 0x4ed5);
  CALL_C(0x4ed5, decCbb3_hook, 0x305d, 0x4ed8);
  if (!(F & FZ)) {
    CYCT(0x4ed8, 0x4ed9); ret_effect(gb);
    return;
  }
  CYC(0x4ed8, 0x4ed9);
  CYC(0x4ed9, 0x4edb); mem_wr(gb, HL, 0x06);
  CYC(0x4edb, 0x4ede); SET_HL(wGfxRegs2_SCY);
  CYC(0x4ede, 0x4edf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4edf, 0x4ee0); A = mem_rd(gb, HL);
  CYC(0x4ee0, 0x4ee2); hram_wr(gb, 0xaa, A);
  CYC(0x4ee2, 0x4ee4); alu_cp(gb, 0x48);
  if (!(F & FZ)) {
    CYCT(0x4ee4, 0x4ee5); ret_effect(gb);
    return;
  }
  CYC(0x4ee4, 0x4ee5);
  CYC(0x4ee5, 0x4ee7); A = 0x7e;
  CYC(0x4ee7, 0x4eea); mem_wr(gb, wTmpcbb3, A);
  CYC(0x4eea, 0x4eed); intro_incState_hook(gb);
}

void introCinematic_ridingHorse_updateScrollingGround_hook(GB *gb) {
  CYC(0x4eed, 0x4eef); A = 0xa8;
  CYC(0x4eef, 0x4ef2); SET_HL(wGfxRegs2_SCY);
  CYC(0x4ef2, 0x4ef3); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4ef3, 0x4ef5); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(0x4ef5, 0x4ef7);
  } else {
    CYC(0x4ef5, 0x4ef7);
    CYC(0x4ef7, 0x4ef9); A = 0xc7;
  }
  CYC(0x4ef9, 0x4efc); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(0x4efc, 0x4efd); A = mem_rd(gb, HL);
  CYC(0x4efd, 0x4f00); SET_HL(wGfxRegs6_SCY);
  CYC(0x4f00, 0x4f01); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4f01, 0x4f04); A = mem_rd(gb, wIntro_frameCounter);
  CYC(0x4f04, 0x4f06); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x4f06, 0x4f07); ret_effect(gb);
    return;
  }
  CYC(0x4f06, 0x4f07);
  CYC(0x4f07, 0x4f08); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4f08, 0x4f09); ret_effect(gb);
}

void introCinematic_ridingHorse_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4f09, introCinematic_ridingHorse_updateScrollingGround_hook, 0x4eed, 0x4f0c);
  CALL_C(0x4f0c, decCbb3_hook, 0x305d, 0x4f0f);
  if (!(F & FZ)) {
    CYCT(0x4f0f, 0x4f10); ret_effect(gb);
    return;
  }
  CYC(0x4f0f, 0x4f10);
  CYC(0x4f10, 0x4f12); mem_wr(gb, HL, 0x20);
  CYC(0x4f12, 0x4f13); SET_HL(HL + 1);
  CYC(0x4f13, 0x4f15); mem_wr(gb, HL, 0x01);
  CYC(0x4f15, 0x4f17); A = 0x96;
  CALL_C(0x4f17, loadPaletteHeader_hook, 0x050b, 0x4f1a);
  CYC(0x4f1a, 0x4f1c); A = 0x38;
  CALL_C(0x4f1c, loadUncompressedGfxHeader_hook, 0x05da, 0x4f1f);
  CYC(0x4f1f, 0x4f21); A = 0x18;
  CYC(0x4f21, 0x4f24); mem_wr(gb, wTmpcbba, A);
  CALL_C(0x4f24, loadGfxRegisterStateIndex_hook, 0x02ea, 0x4f27);
  CYC(0x4f27, 0x4f28); alu_xor(gb, A);
  CYC(0x4f28, 0x4f2a); hram_wr(gb, 0xaa, A);
  CYC(0x4f2a, 0x4f2d); mem_wr(gb, wTmpcbbc, A);
  CYC(0x4f2d, 0x4f30); SET_BC(0x7503);
  CALL_C(0x4f30, createInteraction_hook, 0x540c, 0x4f33);
  CYC(0x4f33, 0x4f35); A = 0x0d;
  CYC(0x4f35, 0x4f38); mem_wr(gb, wTmpcbb6, A);
  CYC(0x4f38, 0x4f3a); A = 0x3c;
  CYC(0x4f3a, 0x4f3d); mem_wr(gb, wTmpcbbb, A);
  CYC(0x4f3d, 0x4f3f); A = 0x03;
  CYC(0x4f3f, 0x4f41); hram_wr(gb, 0x9d, A);
  CYC(0x4f41, 0x4f44); intro_incState_hook(gb);
}
