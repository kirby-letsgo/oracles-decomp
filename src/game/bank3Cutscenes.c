#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

void introCinematic_inTemple_updateCamera_hook(GB *gb);
void introCinematic_moveBlackBarsIn_hook(GB *gb);
void introCinematic_moveBlackBarsOut_hook(GB *gb);

void twinrovaCutsceneCaller_hook(GB *gb) {
  CYC(0x4b0a, 0x4b0b); A = C;
  CYC(0x4b0b, 0x4b0c); push_effect(gb, 0x4b0c);
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
  hook_handoff(gb, HL);
}

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

void twinrovaCutscene_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b33, 0x4b36); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x4b36, 0x4b37); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4b37, 0x4b38); ret_effect(gb);
    return;
  }
  CYC(0x4b37, 0x4b38);
  CALL_C(0x4b38, incCutsceneState_hook, 0x4b10, 0x4b3b);
  CYC(0x4b3b, 0x4b3d); A = 0xf1;
  CYC(0x4b3d, 0x4b40); mem_wr(gb, wActiveRoom, A);
  CALL_C(0x4b40, twinrovaCutscene_fadeinToRoom_hook, 0x4b6f, 0x4b43);
  CALL_C(0x4b43, refreshObjectGfx_hook, 0x1618, 0x4b46);
  CYC(0x4b46, 0x4b49); SET_HL(w1Link_yh);
  CYC(0x4b49, 0x4b4b); mem_wr(gb, HL, 0x38);
  CYC(0x4b4b, 0x4b4c); L = alu_inc8(gb, L);
  CYC(0x4b4c, 0x4b4d); L = alu_inc8(gb, L);
  CYC(0x4b4d, 0x4b4f); mem_wr(gb, HL, 0x78);
  CALL_C(0x4b4f, resetCamera_hook, 0x12ce, 0x4b52);
  CYC(0x4b52, 0x4b55); SET_HL(0x4022);
  CALL_C(0x4b55, parseGivenObjectData_b00_hook, 0x3171, 0x4b58);
  CYC(0x4b58, 0x4b5a); A = 0xac;
  CALL_C(0x4b5a, loadPaletteHeader_hook, 0x050b, 0x4b5d);
  CYC(0x4b5d, 0x4b5f); A = 0x01;
  CYC(0x4b5f, 0x4b62); mem_wr(gb, wScrollMode, A);
  CALL_C(0x4b62, loadCommonGraphics_hook, 0x1a98, 0x4b65);
  CYC(0x4b65, 0x4b67); A = 0x04;
  CALL_C(0x4b67, fadeinFromWhiteWithDelay_hook, 0x3284, 0x4b6a);
  CYC(0x4b6a, 0x4b6c); A = 0x02;
  CYC(0x4b6c, 0x4b6f);
  loadGfxRegisterStateIndex_hook(gb);
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

void cutscene18_body_hook(GB *gb) {
  CYC(0x4b81, 0x4b84); A = mem_rd(gb, wCutsceneState);
  CYC(0x4b84, 0x4b85); push_effect(gb, 0x4b85);
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
  hook_handoff(gb, HL);
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

void cutscene19_body_hook(GB *gb) {
  CYC(0x4c34, 0x4c37); A = mem_rd(gb, wCutsceneState);
  CYC(0x4c37, 0x4c38); push_effect(gb, 0x4c38);
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
  hook_handoff(gb, HL);
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
  CALL_C(0x4ebc, introCinematic_moveBlackBarsIn_hook, 0x53d3, 0x4ebf);
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

void introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen_hook(GB *gb);
void introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(GB *gb);
void introCinematic_ridingHorse_drawTempleSprites_hook(GB *gb);

void introCinematic_ridingHorse_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4f44, introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen_hook, 0x4f70, 0x4f47);
  CYC(0x4f47, 0x4f4a); SET_HL(wTmpcbb3);
  CALL_C(0x4f4a, decHlRef16WithCap_hook, 0x0237, 0x4f4d);
  if (!(F & FZ)) {
    CYCT(0x4f4d, 0x4f4e); ret_effect(gb);
    return;
  }
  CYC(0x4f4d, 0x4f4e);
  CYC(0x4f4e, 0x4f50); A = 0x36;
  CALL_C(0x4f50, loadUncompressedGfxHeader_hook, 0x05da, 0x4f53);
  CYC(0x4f53, 0x4f55); mem_wr(gb, HL, 0x5a);
  CYC(0x4f55, 0x4f57); A = 0x9b;
  CALL_C(0x4f57, loadPaletteHeader_hook, 0x050b, 0x4f5a);
  CALL_C(0x4f5a, clearDynamicInteractions_hook, 0x35d2, 0x4f5d);
  CALL_C(0x4f5d, clearOam_hook, 0x049f, 0x4f60);
  CYC(0x4f60, 0x4f62); A = 0x19;
  CALL_C(0x4f62, loadGfxRegisterStateIndex_hook, 0x02ea, 0x4f65);
  CYC(0x4f65, 0x4f67); A = 0x48;
  CYC(0x4f67, 0x4f6a); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(0x4f6a, 0x4f6d); mem_wr(gb, wGfxRegs2_WINY, A);
  CYC(0x4f6d, 0x4f70); intro_incState_hook(gb);
}

void introCinematic_ridingHorse_state4__drawLinkOnHorseAndScrollScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f70, 0x4f73); SET_HL(0x5a81);
  CYC(0x4f73, 0x4f75); E = 0x3f;
  CALL_C(0x4f75, addSpritesFromBankToOam_hook, 0x30eb, 0x4f78);
  CYC(0x4f78, 0x4f7b); A = mem_rd(gb, wIntro_frameCounter);
  CYC(0x4f7b, 0x4f7d); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(0x4f7d, 0x4f7f);
  } else {
    CYC(0x4f7d, 0x4f7f);
    CYC(0x4f7f, 0x4f82); SET_HL(wGfxRegs1_SCX);
    CYC(0x4f82, 0x4f83); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(0x4f83, 0x4f86); SET_HL(wTmpcbb6);
  CYC(0x4f86, 0x4f87); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x4f87, 0x4f89);
  } else {
    CYC(0x4f87, 0x4f89);
    CYC(0x4f89, 0x4f8b); mem_wr(gb, HL, 0x0d);
    CYC(0x4f8b, 0x4f8e); SET_HL(wGfxRegs2_SCX);
    CYC(0x4f8e, 0x4f8f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(0x4f8f, 0x4f92); SET_HL(wTmpcbbb);
  CYC(0x4f92, 0x4f93); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x4f93, 0x4f94); ret_effect(gb);
    return;
  }
  CYC(0x4f93, 0x4f94);
  CYC(0x4f94, 0x4f96); mem_wr(gb, HL, 0x3c);
  CYC(0x4f96, 0x4f97); SET_HL(HL + 1);
  CYC(0x4f97, 0x4f98); A = mem_rd(gb, HL);
  CYC(0x4f98, 0x4f9a); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x4f9a, 0x4f9b); ret_effect(gb);
    return;
  }
  CYC(0x4f9a, 0x4f9b);
  CYC(0x4f9b, 0x4f9c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f9c, 0x4f9f); SET_HL(0x4fa4);
  CYC(0x4f9f, 0x4fa0); push_effect(gb, 0x4fa0);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(0x4fa0, 0x4fa1); A = mem_rd(gb, HL);
  CYC(0x4fa1, 0x4fa4); loadPaletteHeader_hook(gb);
}

void introCinematic_ridingHorse_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4fa7, introCinematic_moveBlackBarsOut_hook, 0x53eb, 0x4faa);
  CYC(0x4faa, 0x4fad); SET_HL(wGfxRegs2_SCX);
  CYC(0x4fad, 0x4fae); A = mem_rd(gb, HL);
  CYC(0x4fae, 0x4fb0); alu_add(gb, 0x08);
  CYC(0x4fb0, 0x4fb1); mem_wr(gb, HL, A);
  CYC(0x4fb1, 0x4fb3); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(0x4fb3, 0x4fb4); ret_effect(gb);
    return;
  }
  CYC(0x4fb3, 0x4fb4);
  CYC(0x4fb4, 0x4fb6); mem_wr(gb, HL, 0x60);
  CALL_C(0x4fb6, intro_incState_hook, 0x4d33, 0x4fb9);
  CYC(0x4fb9, 0x4fbc); SET_HL(wTmpcbb3);
  CYC(0x4fbc, 0x4fbe); mem_wr(gb, HL, 0x18);
  CYC(0x4fbe, 0x4fc1); SET_BC(0x7504);
  CYC(0x4fc1, 0x4fc4); createInteraction_hook(gb);
}

void introCinematic_ridingHorse_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4fc4, 0x4fc7); SET_HL(wTmpcbb3);
  CALL_C(0x4fc7, decHlRef16WithCap_hook, 0x0237, 0x4fca);
  if (!(F & FZ)) {
    CYCT(0x4fca, 0x4fcb); ret_effect(gb);
    return;
  }
  CYC(0x4fca, 0x4fcb);
  CALL_C(0x4fcb, disableLcd_hook, 0x02c1, 0x4fce);
  CYC(0x4fce, 0x4fd0); A = 0x92;
  CALL_C(0x4fd0, loadPaletteHeader_hook, 0x050b, 0x4fd3);
  CYC(0x4fd3, 0x4fd5); A = 0x9c;
  CALL_C(0x4fd5, loadGfxHeader_hook, 0x0626, 0x4fd8);
  CALL_C(0x4fd8, clearDynamicInteractions_hook, 0x35d2, 0x4fdb);
  CYC(0x4fdb, 0x4fdd); A = 0x0a;
  CALL_C(0x4fdd, loadGfxRegisterStateIndex_hook, 0x02ea, 0x4fe0);
  CYC(0x4fe0, 0x4fe3); intro_incState_hook(gb);
}

void introCinematic_ridingHorse_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4fe3, 0x4fe6); SET_HL(wGfxRegs1_SCY);
  CYC(0x4fe6, 0x4fe7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x4fe7, 0x4fe9);
    introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(gb);
    return;
  }
  CYC(0x4fe7, 0x4fe9);
  CYC(0x4fe9, 0x4feb); A = 0xcc;
  CYC(0x4feb, 0x4fee); mem_wr(gb, wTmpcbb6, A);
  CALL_C(0x4fee, intro_incState_hook, 0x4d33, 0x4ff1);
  introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(gb);
}

void introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(GB *gb) {
  CYC(0x4ff1, 0x4ff4); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x4ff4, 0x4ff5); alu_cpl(gb);
  CYC(0x4ff5, 0x4ff6); A = alu_inc8(gb, A);
  CYC(0x4ff6, 0x4ff7); B = A;
  CYC(0x4ff7, 0x4ff8); alu_xor(gb, A);
  CYC(0x4ff8, 0x4ffa); hram_wr(gb, 0x9f, A);
  CYC(0x4ffa, 0x4ffb); C = A;
  CYC(0x4ffb, 0x4ffe); SET_HL(0x59d3);
  CYC(0x4ffe, 0x5000); E = 0x3f;
  CYC(0x5000, 0x5003); addSpritesFromBankToOam_withOffset_hook(gb);
}

void introCinematic_ridingHorse_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5003, 0x5006); SET_HL(wTmpcbb6);
  CYC(0x5006, 0x5007); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5007, 0x5009);
    introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_2_hook(gb);
    return;
  }
  CYC(0x5007, 0x5009);
  CYC(0x5009, 0x500b); A = 0x93;
  CALL_C(0x500b, loadPaletteHeader_hook, 0x050b, 0x500e);
  CALL_C(0x500e, disableLcd_hook, 0x02c1, 0x5011);
  CALL_C(0x5011, clearOam_hook, 0x049f, 0x5014);
  CYC(0x5014, 0x5016); A = 0x10;
  CYC(0x5016, 0x5018); hram_wr(gb, 0x9f, A);
  CYC(0x5018, 0x501a); A = 0x9d;
  CALL_C(0x501a, loadGfxHeader_hook, 0x0626, 0x501d);
  CYC(0x501d, 0x501f); A = 0x05;
  CYC(0x501f, 0x5022); mem_wr(gb, wTmpcbbb, A);
  CYC(0x5022, 0x5025); SET_HL(wTmpcbb3);
  CYC(0x5025, 0x5027); mem_wr(gb, HL, 0x90);
  CYC(0x5027, 0x5028); SET_HL(HL + 1);
  CYC(0x5028, 0x502a); mem_wr(gb, HL, 0x01);
  CYC(0x502a, 0x502c); A = 0xb4;
  CYC(0x502c, 0x502f); mem_wr(gb, wTmpcbb6, A);
  CALL_C(0x502f, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x5032);
  CYC(0x5032, 0x5034); A = 0x0b;
  CALL_C(0x5034, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5037);
  CALL_C(0x5037, introCinematic_ridingHorse_drawTempleSprites_hook, 0x5075, 0x503a);
  CYC(0x503a, 0x503c); B = 0x02;
  for (;;) {
    CALL_C(0x503c, getFreeInteractionSlot_hook, 0x3aef, 0x503f);
    if (!(F & FZ)) {
      CYCT(0x503f, 0x5041);
      break;
    }
    CYC(0x503f, 0x5041);
    CYC(0x5041, 0x5043); mem_wr(gb, HL, 0x75);
    CYC(0x5043, 0x5044); L = alu_inc8(gb, L);
    CYC(0x5044, 0x5045); mem_wr(gb, HL, B);
    CYC(0x5045, 0x5046); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x5046, 0x5048);
      continue;
    }
    CYC(0x5046, 0x5048);
    break;
  }
  CYC(0x5048, 0x504b); intro_incState_hook(gb);
}

void introCinematic_ridingHorse_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x504b, 0x504e); SET_HL(wTmpcbb3);
  CALL_C(0x504e, decHlRef16WithCap_hook, 0x0237, 0x5051);
  if (!(F & FZ)) {
    CYCT(0x5051, 0x5053);
  } else {
    CYC(0x5051, 0x5053);
    CALL_C(0x5053, fadeoutToWhite_hook, 0x326c, 0x5056);
    CALL_C(0x5056, intro_incState_hook, 0x4d33, 0x5059);
    CYC(0x5059, 0x505b);
    introCinematic_ridingHorse_drawTempleSprites_hook(gb);
    return;
  }
  CYC(0x505b, 0x505e); SET_HL(wTmpcbb6);
  CYC(0x505e, 0x505f); A = mem_rd(gb, HL);
  CYC(0x505f, 0x5060); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5060, 0x5062);
    introCinematic_ridingHorse_drawTempleSprites_hook(gb);
    return;
  }
  CYC(0x5060, 0x5062);
  CYC(0x5062, 0x5063); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5063, 0x5066); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(0x5066, 0x5067); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5067, 0x5069);
    introCinematic_ridingHorse_drawTempleSprites_hook(gb);
    return;
  }
  CYC(0x5067, 0x5069);
  CYC(0x5069, 0x506c); SET_HL(wTmpcbbb);
  CYC(0x506c, 0x506d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x506d, 0x506f);
    introCinematic_ridingHorse_drawTempleSprites_hook(gb);
    return;
  }
  CYC(0x506d, 0x506f);
  CYC(0x506f, 0x5071); mem_wr(gb, HL, 0x05);
  CYC(0x5071, 0x5074); SET_HL(wGfxRegs1_SCX);
  CYC(0x5074, 0x5075); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  introCinematic_ridingHorse_drawTempleSprites_hook(gb);
}

void introCinematic_ridingHorse_drawTempleSprites_hook(GB *gb) {
  CYC(0x5075, 0x5076); alu_xor(gb, A);
  CYC(0x5076, 0x5078); hram_wr(gb, 0x9f, A);
  CYC(0x5078, 0x5079); B = A;
  CYC(0x5079, 0x507c); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(0x507c, 0x507d); alu_cpl(gb);
  CYC(0x507d, 0x507e); A = alu_inc8(gb, A);
  CYC(0x507e, 0x507f); C = A;
  CYC(0x507f, 0x5082); SET_HL(0x5a6c);
  CYC(0x5082, 0x5084); E = 0x3f;
  CYC(0x5084, 0x5087); addSpritesFromBankToOam_withOffset_hook(gb);
}

void introCinematic_ridingHorse_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5087, 0x508a); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x508a, 0x508b); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x508b, 0x508d);
    introCinematic_ridingHorse_drawTempleSprites_hook(gb);
    return;
  }
  CYC(0x508b, 0x508d);
  CALL_C(0x508d, clearDynamicInteractions_hook, 0x35d2, 0x5090);
  CYC(0x5090, 0x5092); incIntroCinematicState_hook(gb);
}

void introCinematic_preTitlescreen_updateScrollingTree_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5092, 0x5095); SET_HL(wTmpcbb6);
  CYC(0x5095, 0x5096); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5096, 0x5097); ret_effect(gb);
    return;
  }
  CYC(0x5096, 0x5097);
  CYC(0x5097, 0x509a); A = mem_rd(gb, wTmpcbba);
  CYC(0x509a, 0x509d); mem_wr(gb, wTmpcbb6, A);
  CYC(0x509d, 0x50a0); SET_HL(wGfxRegs1_SCY);
  CYC(0x50a0, 0x50a1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x50a1, 0x50a2); A = mem_rd(gb, HL);
  CYC(0x50a2, 0x50a4); alu_cp(gb, 0x88);
  if (F & FZ) {
    CYCT(0x50a4, 0x50a5); ret_effect(gb);
    return;
  }
  CYC(0x50a4, 0x50a5);
  CYC(0x50a5, 0x50a7); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x50a7, 0x50a9);
    CYC(0x50be, 0x50c0); alu_cp(gb, 0xb0);
    if (!(F & FZ)) {
      CYCT(0x50c0, 0x50c2);
    } else {
      CYC(0x50c0, 0x50c2);
      CYC(0x50c2, 0x50c4); A = 0x2a;
      CALL_C(0x50c4, loadUncompressedGfxHeader_hook, 0x05da, 0x50c7);
    }
    CYC(0x50c7, 0x50c9); alu_or(gb, 0x01);
    CYC(0x50c9, 0x50ca); ret_effect(gb);
    return;
  }
  CYC(0x50a7, 0x50a9);
  CYC(0x50a9, 0x50ab); A = 0x0d;
  CALL_C(0x50ab, loadUncompressedGfxHeader_hook, 0x05da, 0x50ae);
  CYC(0x50ae, 0x50b0); B = 0x04;
  for (;;) {
    CALL_C(0x50b0, getFreeInteractionSlot_hook, 0x3aef, 0x50b3);
    if (!(F & FZ)) {
      CYCT(0x50b3, 0x50b5);
      break;
    }
    CYC(0x50b3, 0x50b5);
    CYC(0x50b5, 0x50b7); mem_wr(gb, HL, 0xd2);
    CYC(0x50b7, 0x50b8); L = alu_inc8(gb, L);
    CYC(0x50b8, 0x50b9); B = alu_dec8(gb, B);
    CYC(0x50b9, 0x50ba); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(0x50ba, 0x50bc);
      continue;
    }
    CYC(0x50ba, 0x50bc);
    CYC(0x50bc, 0x50be);
    break;
  }
  CYC(0x50c7, 0x50c9); alu_or(gb, 0x01);
  CYC(0x50c9, 0x50ca); ret_effect(gb);
}

void introCinematic_inTemple_state0__nextTriforce_hook(GB *gb);
void introCinematic_inTemple_state0__doneSpawningTriforce_hook(GB *gb);
void introCinematic_inTemple_updateWave_hook(GB *gb);

void introCinematic_inTemple_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x50ed, disableLcd_hook, 0x02c1, 0x50f0);
  CALL_C(0x50f0, clearOam_hook, 0x049f, 0x50f3);
  CYC(0x50f3, 0x50f5); A = 0x10;
  CYC(0x50f5, 0x50f7); hram_wr(gb, 0x9f, A);
  CYC(0x50f7, 0x50f9); A = 0x9e;
  CALL_C(0x50f9, loadGfxHeader_hook, 0x0626, 0x50fc);
  CYC(0x50fc, 0x50fe); A = 0x91;
  CALL_C(0x50fe, loadPaletteHeader_hook, 0x050b, 0x5101);
  CYC(0x5101, 0x5103); A = 0x09;
  CALL_C(0x5103, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5106);
  CYC(0x5106, 0x5109); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x5109, 0x510b); hram_wr(gb, 0xaa, A);
  CYC(0x510b, 0x510d); A = 0x10;
  CYC(0x510d, 0x5110); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(0x5110, loadAnimationData_hook, 0x3659, 0x5113);
  CYC(0x5113, 0x5115); A = 0x01;
  CYC(0x5115, 0x5118); mem_wr(gb, wScrollMode, A);
  CYC(0x5118, 0x511a); A = 0x08;
  CALL_C(0x511a, setLinkID_hook, 0x2ad9, 0x511d);
  CYC(0x511d, 0x511f); L = 0x00;
  CYC(0x511f, 0x5121); mem_wr(gb, HL, 0x01);
  CYC(0x5121, 0x5123); L = 0x0b;
  CYC(0x5123, 0x5126); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x5126, 0x5128); alu_add(gb, 0x60);
  CYC(0x5128, 0x5129); mem_wr(gb, HL, A);
  CYC(0x5129, 0x512b); L = 0x0d;
  CYC(0x512b, 0x512d); mem_wr(gb, HL, 0x50);
  CYC(0x512d, 0x5130); SET_HL(0x709a);
  CYC(0x5130, 0x5132); A = 0x10;
  CALL_C(0x5132, setSimulatedInputAddress_hook, 0x2a1d, 0x5135);
  CYC(0x5135, 0x5137); B = 0x03;
  CYC(0x5137, 0x5139); C = 0x30;
  introCinematic_inTemple_state0__nextTriforce_hook(gb);
}

void introCinematic_inTemple_state0__nextTriforce_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CALL_C(0x5139, getFreeInteractionSlot_hook, 0x3aef, 0x513c);
    if (!(F & FZ)) {
      CYCT(0x513c, 0x513e);
      introCinematic_inTemple_state0__doneSpawningTriforce_hook(gb);
      return;
    }
    CYC(0x513c, 0x513e);
    CYC(0x513e, 0x5140); mem_wr(gb, HL, 0x4a);
    CYC(0x5140, 0x5141); L = alu_inc8(gb, L);
    CYC(0x5141, 0x5142); A = B;
    CYC(0x5142, 0x5143); A = alu_dec8(gb, A);
    CYC(0x5143, 0x5144); mem_wr(gb, HL, A);
    CYC(0x5144, 0x5146); L = 0x4b;
    CYC(0x5146, 0x5148); mem_wr(gb, HL, 0x19);
    CYC(0x5148, 0x5149); A = C;
    CYC(0x5149, 0x514b); L = 0x4d;
    CYC(0x514b, 0x514c); mem_wr(gb, HL, A);
    CYC(0x514c, 0x514e); alu_add(gb, 0x20);
    CYC(0x514e, 0x514f); C = A;
    CYC(0x514f, 0x5150); A = C;
    CYC(0x5150, 0x5151); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x5151, 0x5153);
      continue;
    }
    CYC(0x5151, 0x5153);
    introCinematic_inTemple_state0__doneSpawningTriforce_hook(gb);
    return;
  }
}

void introCinematic_inTemple_state0__doneSpawningTriforce_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5153, 0x5156); SET_HL(wMenuDisabled);
  CYC(0x5156, 0x5158); mem_wr(gb, HL, 0x01);
  CALL_C(0x5158, fadeinFromWhite_hook, 0x3299, 0x515b);
  CYC(0x515b, 0x515c); alu_xor(gb, A);
  CYC(0x515c, 0x515f); mem_wr(gb, wIntro_triforceState, A);
  CYC(0x515f, 0x5162); intro_incState_hook(gb);
}

void introCinematic_inTemple_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5162, 0x5165); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x5165, 0x5166); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5166, 0x5167); ret_effect(gb);
    return;
  }
  CYC(0x5166, 0x5167);
  CYC(0x5167, 0x516a); A = mem_rd(gb, wUseSimulatedInput);
  CYC(0x516a, 0x516b); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x516b, 0x516e);
    introCinematic_inTemple_updateCamera_hook(gb);
    return;
  }
  CYC(0x516b, 0x516e);
  CYC(0x516e, 0x516f); alu_xor(gb, A);
  CYC(0x516f, 0x5172); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(0x5172, introCinematic_inTemple_updateCamera_hook, 0x53ba, 0x5175);
  CYC(0x5175, 0x5178); intro_incState_hook(gb);
}

void introCinematic_inTemple_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5178, 0x517b); A = mem_rd(gb, wIntro_triforceState);
  CYC(0x517b, 0x517d); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x517d, 0x517e); ret_effect(gb);
    return;
  }
  CYC(0x517d, 0x517e);
  CALL_C(0x517e, fadeoutToWhite_hook, 0x326c, 0x5181);
  CYC(0x5181, 0x5184); intro_incState_hook(gb);
}

void introCinematic_inTemple_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5184, 0x5187); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x5187, 0x5188); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5188, 0x5189); ret_effect(gb);
    return;
  }
  CYC(0x5188, 0x5189);
  CYC(0x5189, 0x518b); A = 0x01;
  CYC(0x518b, 0x518e); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(0x518e, 0x518f); A = alu_inc8(gb, A);
  CYC(0x518f, 0x5192); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(0x5192, 0x5194); A = 0x00;
  CYC(0x5194, 0x5196); hram_wr(gb, 0x9d, A);
  CYC(0x5196, 0x5198); A = 0x20;
  CALL_C(0x5198, initWaveScrollValues_hook, 0x1384, 0x519b);
  CALL_C(0x519b, fadeinFromWhite_hook, 0x3299, 0x519e);
  CALL_C(0x519e, intro_incState_hook, 0x4d33, 0x51a1);
  introCinematic_inTemple_updateWave_hook(gb);
}

void introCinematic_inTemple_updateWave_hook(GB *gb) {
  CYC(0x51a1, 0x51a4); SET_HL(wFrameCounter);
  CYC(0x51a4, 0x51a5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x51a5, 0x51a7); A = 0x02;
  CYC(0x51a7, 0x51aa); loadBigBufferScrollValues_hook(gb);
}

void introCinematic_inTemple_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51aa, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x51ad);
  CYC(0x51ad, 0x51b0); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x51b0, 0x51b1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x51b1, 0x51b2); ret_effect(gb);
    return;
  }
  CYC(0x51b1, 0x51b2);
  CYC(0x51b2, 0x51b5); SET_HL(wTmpcbb6);
  CYC(0x51b5, 0x51b7); mem_wr(gb, HL, 0x78);
  CYC(0x51b7, 0x51ba); intro_incState_hook(gb);
}

void introCinematic_inTemple_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51ba, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x51bd);
  CYC(0x51bd, 0x51c0); SET_HL(wTmpcbb6);
  CYC(0x51c0, 0x51c1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x51c1, 0x51c2); ret_effect(gb);
    return;
  }
  CYC(0x51c1, 0x51c2);
  CYC(0x51c2, 0x51c5); mem_wr(gb, wTmpcbb6, A);
  CYC(0x51c5, 0x51c6); A = alu_dec8(gb, A);
  CYC(0x51c6, 0x51c9); mem_wr(gb, wTmpcbba, A);
  CALL_C(0x51c9, intro_incState_hook, 0x4d33, 0x51cc);
  introCinematic_inTemple_state6_hook(gb);
}

void flashScreen_body_hook(GB *gb);
void clearFadingPalettes_body_hook(GB *gb);
void introCinematic_preTitlescreen_updateScrollForTitle_hook(GB *gb);

void introCinematic_inTemple_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51cc, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x51cf);
  CYC(0x51cf, 0x51d2); SET_HL(wTmpcbb6);
  CYC(0x51d2, 0x51d4); B = 0x00;
  CALL_C(0x51d4, flashScreen_body_hook, 0x522e, 0x51d7);
  if (F & FZ) {
    CYCT(0x51d7, 0x51d8); ret_effect(gb);
    return;
  }
  CYC(0x51d7, 0x51d8);
  CALL_C(0x51d8, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x51db);
  CYC(0x51db, 0x51dd); A = 0x06;
  CYC(0x51dd, 0x51e0); mem_wr(gb, wIntro_triforceState, A);
  CYC(0x51e0, 0x51e2); A = 0x91;
  CALL_C(0x51e2, playSound_b00_hook, 0x0c98, 0x51e5);
  CYC(0x51e5, 0x51e8); intro_incState_hook(gb);
}

void introCinematic_inTemple_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51e8, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x51eb);
  CYC(0x51eb, 0x51ee); A = mem_rd(gb, wIntro_triforceState);
  CYC(0x51ee, 0x51f0); alu_cp(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x51f0, 0x51f1); ret_effect(gb);
    return;
  }
  CYC(0x51f0, 0x51f1);
  CALL_C(0x51f1, clearLinkObject_hook, 0x35ba, 0x51f4);
  CYC(0x51f4, 0x51f6); B = 0x08;
  CALL_C(0x51f6, func_2d48_hook, 0x2d48, 0x51f9);
  CYC(0x51f9, 0x51fa); A = B;
  CYC(0x51fa, 0x51fd); mem_wr(gb, wTmpcbb6, A);
  CYC(0x51fd, 0x5200); intro_incState_hook(gb);
}

void introCinematic_inTemple_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5200, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x5203);
  CYC(0x5203, 0x5206); SET_HL(wTmpcbb6);
  CYC(0x5206, 0x5207); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5207, 0x5208); ret_effect(gb);
    return;
  }
  CYC(0x5207, 0x5208);
  CYC(0x5208, 0x520a); mem_wr(gb, HL, 0x3c);
  CYC(0x520a, 0x520d); intro_incState_hook(gb);
}

void introCinematic_inTemple_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x520d, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x5210);
  CYC(0x5210, 0x5213); SET_HL(wTmpcbb6);
  CYC(0x5213, 0x5214); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5214, 0x5215); ret_effect(gb);
    return;
  }
  CYC(0x5214, 0x5215);
  CYC(0x5215, 0x5217); A = 0xb4;
  CALL_C(0x5217, playSound_b00_hook, 0x0c98, 0x521a);
  CALL_C(0x521a, fadeoutToWhite_hook, 0x326c, 0x521d);
  CYC(0x521d, 0x5220); intro_incState_hook(gb);
}

void introCinematic_inTemple_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5220, introCinematic_inTemple_updateWave_hook, 0x51a1, 0x5223);
  CYC(0x5223, 0x5226); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x5226, 0x5227); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5227, 0x5228); ret_effect(gb);
    return;
  }
  CYC(0x5227, 0x5228);
  CALL_C(0x5228, clearDynamicInteractions_hook, 0x35d2, 0x522b);
  CYC(0x522b, 0x522e); incIntroCinematicState_hook(gb);
}

void flashScreen_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x522e, 0x522f); A = B;
  CYC(0x522f, 0x5230); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5230, 0x5231); B = mem_rd(gb, HL);
  CYC(0x5231, 0x5234); SET_HL(0x5276);
  CYC(0x5234, 0x5235); push_effect(gb, 0x5235);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
  CYC(0x5235, 0x5236); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5236, 0x5237); H = mem_rd(gb, HL);
  CYC(0x5237, 0x5238); L = A;
  CYC(0x5238, 0x523a); C = 0;
  for (;;) {
    CYC(0x523a, 0x523b); A = mem_rd(gb, HL);
    CYC(0x523b, 0x523d); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(0x523d, 0x523e); ret_effect(gb);
      return;
    }
    CYC(0x523d, 0x523e);
    CYC(0x523e, 0x523f); alu_cp(gb, B);
    if (!(F & FC)) {
      CYCT(0x523f, 0x5241);
      break;
    }
    CYC(0x523f, 0x5241);
    CYC(0x5241, 0x5242); SET_HL(HL + 1);
    CYC(0x5242, 0x5243); C = alu_inc8(gb, C);
    CYC(0x5243, 0x5245);
  }
  CYC(0x5245, 0x5246); A = C;
  CYC(0x5246, 0x5248); alu_and(gb, 0x01);
  CYC(0x5248, 0x5249); C = A;
  CYC(0x5249, 0x524c); A = mem_rd(gb, wTmpcbba);
  CYC(0x524c, 0x524d); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x524d, 0x524e); ret_effect(gb);
    return;
  }
  CYC(0x524d, 0x524e);
  CYC(0x524e, 0x524f); A = C;
  CYC(0x524f, 0x5252); mem_wr(gb, wTmpcbba, A);
  CYC(0x5252, 0x5253); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5253, 0x5255);
    clearFadingPalettes_body_hook(gb);
    return;
  }
  CYC(0x5253, 0x5255);
  CALL_C(0x5255, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x5258);
  CYC(0x5258, 0x5259); alu_xor(gb, A);
  CYC(0x5259, 0x525a); ret_effect(gb);
}

void clearFadingPalettes_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x525a, 0x525c); A = 0x02;
  CYC(0x525c, 0x525e); hram_wr(gb, 0x70, A);
  CYC(0x525e, 0x5260); B = 0x80;
  CYC(0x5260, 0x5263); SET_HL(0xdf80);
  CYC(0x5263, 0x5265); A = 0xff;
  CALL_C(0x5265, fillMemory_hook, 0x0470, 0x5268);
  CYC(0x5268, 0x526a); A = 0xff;
  CYC(0x526a, 0x526c); hram_wr(gb, 0xa9, A);
  CYC(0x526c, 0x526e); hram_wr(gb, 0xa8, A);
  CYC(0x526e, 0x5270); hram_wr(gb, 0xa7, A);
  CYC(0x5270, 0x5272); hram_wr(gb, 0xa6, A);
  CYC(0x5272, 0x5273); alu_xor(gb, A);
  CYC(0x5273, 0x5275); hram_wr(gb, 0x70, A);
  CYC(0x5275, 0x5276); ret_effect(gb);
}

void introCinematic_preTitlescreen_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5317, introCinematic_preTitlescreen_updateScrollingTree_hook, 0x5092, 0x531a);
  if (!(F & FZ)) {
    CYCT(0x531a, 0x531b); ret_effect(gb);
    return;
  }
  CYC(0x531a, 0x531b);
  CALL_C(0x531b, intro_incState_hook, 0x4d33, 0x531e);
  CYC(0x531e, 0x5321); SET_HL(wTmpcbb3);
  CYC(0x5321, 0x5323); mem_wr(gb, HL, 0x02);
  CYC(0x5323, 0x5324); SET_HL(HL + 1);
  CYC(0x5324, 0x5325); alu_xor(gb, A);
  CYC(0x5325, 0x5326); mem_wr(gb, HL, A);
  CYC(0x5326, 0x5329); SET_HL(wTmpcbb6);
  CYC(0x5329, 0x532b); mem_wr(gb, HL, 0x10);
  CYC(0x532b, 0x532c); A = alu_inc8(gb, A);
  CYC(0x532c, 0x532f); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(0x532f, 0x5330); A = alu_inc8(gb, A);
  CYC(0x5330, 0x5333); mem_wr(gb, wGfxRegs2_LYC, A);
  CYC(0x5333, 0x5335); A = 0x01;
  CYC(0x5335, 0x5337); hram_wr(gb, 0x9d, A);
  CYC(0x5337, 0x533a); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x533a, 0x533c); B = 0x90;
  CYC(0x533c, 0x533f); SET_HL(0xc300);
  do {
    CYC(0x533f, 0x5340); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x5340, 0x5341); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x5341, 0x5343);
    } else {
      CYC(0x5341, 0x5343);
      break;
    }
  } while (true);
  CYC(0x5343, 0x5345); A = 0x01;
  introCinematic_preTitlescreen_updateScrollForTitle_hook(gb);
}

void introCinematic_preTitlescreen_updateScrollForTitle_hook(GB *gb) {
  CYC(0x5345, 0x5346); B = A;
  CYC(0x5346, 0x5347); alu_xor(gb, A);
  CYC(0x5347, 0x5348); C = A;
  for (;;) {
    CYC(0x5348, 0x5349); C = alu_inc8(gb, C);
    CYC(0x5349, 0x534a); alu_add(gb, B);
    CYC(0x534a, 0x534c); alu_cp(gb, 0x18);
    if (F & FZ) {
      CYCT(0x534c, 0x534e);
      break;
    }
    CYC(0x534c, 0x534e);
    if (!(F & FC)) {
      CYCT(0x534e, 0x534f); ret_effect(gb);
      return;
    }
    CYC(0x534e, 0x534f);
    CYC(0x534f, 0x5351);
  }
  CYC(0x5351, 0x5352); push_effect(gb, BC);
  CYC(0x5352, 0x5354); A = 0x38;
  CYC(0x5354, 0x5355); alu_sub(gb, B);
  CYC(0x5355, 0x5357); H = 0xc3;
  CYC(0x5357, 0x5358); L = A;
  CYC(0x5358, 0x5359); alu_xor(gb, A);
  do {
    CYC(0x5359, 0x535a); push_effect(gb, AF);
    CYC(0x535a, 0x535b); alu_sub(gb, L);
    CYC(0x535b, 0x535d); alu_add(gb, 0x58);
    CYC(0x535d, 0x535e); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x535e, 0x535f); SET_AF(pop_effect(gb));
    CYC(0x535f, 0x5360); alu_add(gb, C);
    CYC(0x5360, 0x5361); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x5361, 0x5363);
    } else {
      CYC(0x5361, 0x5363);
      break;
    }
  } while (true);
  CYC(0x5363, 0x5364); SET_BC(pop_effect(gb));
  CYC(0x5364, 0x5366); A = 0x37;
  CYC(0x5366, 0x5367); alu_add(gb, B);
  CYC(0x5367, 0x5368); L = A;
  CYC(0x5368, 0x536a); A = 0x2f;
  do {
    CYC(0x536a, 0x536b); push_effect(gb, AF);
    CYC(0x536b, 0x536c); alu_sub(gb, L);
    CYC(0x536c, 0x536e); alu_add(gb, 0x58);
    CYC(0x536e, 0x536f); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(0x536f, 0x5370); SET_AF(pop_effect(gb));
    CYC(0x5370, 0x5371); alu_sub(gb, C);
    CYC(0x5371, 0x5372); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x5372, 0x5374);
    } else {
      CYC(0x5372, 0x5374);
      break;
    }
  } while (true);
  CYC(0x5374, 0x5375); ret_effect(gb);
}

void introCinematic_preTitlescreen_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5375, 0x5378); SET_HL(wTmpcbb6);
  CYC(0x5378, 0x5379); A = mem_rd(gb, HL);
  CYC(0x5379, 0x537a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x537a, 0x537c);
  } else {
    CYC(0x537a, 0x537c);
    CYC(0x537c, 0x537d); A = alu_dec8(gb, A);
    CYC(0x537d, 0x537e); mem_wr(gb, HL, A);
    CYC(0x537e, 0x5380); A = 0xab;
    if (F & FZ) CALL_C_CC(0x5380, playSound_b00_hook, 0x0c98, 0x5383);
    else CYC(0x5380, 0x5383);
  }
  CYC(0x5383, 0x5386); A = mem_rd(gb, wIntro_frameCounter);
  CYC(0x5386, 0x5388); alu_and(gb, 0x01);
  CYC(0x5388, 0x538b); SET_HL(wTmpcbb4);
  if (!(F & FZ)) {
    CYCT(0x538b, 0x538c); ret_effect(gb);
    return;
  }
  CYC(0x538b, 0x538c);
  CYC(0x538c, 0x538d); A = mem_rd(gb, HL);
  CYC(0x538d, 0x538f); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(0x538f, 0x5391);
    CYC(0x539b, 0x539c); alu_xor(gb, A);
    CYC(0x539c, 0x539f); mem_wr(gb, wTmpcbb6, A);
    CYC(0x539f, 0x53a0); A = alu_dec8(gb, A);
    CYC(0x53a0, 0x53a3); mem_wr(gb, wTmpcbba, A);
    CYC(0x53a3, 0x53a6); intro_incState_hook(gb);
    return;
  }
  CYC(0x538f, 0x5391);
  CYC(0x5391, 0x5392); A = alu_inc8(gb, A);
  CYC(0x5392, 0x5393); mem_wr(gb, HL, A);
  CYC(0x5393, 0x5396); SET_HL(0x53b2);
  CYC(0x5396, 0x5397); push_effect(gb, 0x5397);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(0x5397, 0x5398); A = mem_rd(gb, HL);
  CYC(0x5398, 0x539b); introCinematic_preTitlescreen_updateScrollForTitle_hook(gb);
}

void introCinematic_inTemple_updateCamera_hook(GB *gb) {
  CYC(0x53ba, 0x53bd); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x53bd, 0x53be); B = A;
  CYC(0x53be, 0x53c1); SET_DE(0xd00b);
  CYC(0x53c1, 0x53c2); A = mem_rd(gb, DE);
  CYC(0x53c2, 0x53c3); alu_sub(gb, B);
  CYC(0x53c3, 0x53c5); alu_sub(gb, 0x40);
  CYC(0x53c5, 0x53c6); B = A;
  CYC(0x53c6, 0x53c9); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x53c9, 0x53ca); alu_add(gb, B);
  CYC(0x53ca, 0x53cc); alu_cp(gb, 0x70);
  if (!(F & FC)) {
    CYCT(0x53cc, 0x53cd); ret_effect(gb);
    return;
  }
  CYC(0x53cc, 0x53cd);
  CYC(0x53cd, 0x53d0); mem_wr(gb, wGfxRegs1_SCY, A);
  CYC(0x53d0, 0x53d2); hram_wr(gb, 0xaa, A);
  CYC(0x53d2, 0x53d3); ret_effect(gb);
}

void introCinematic_moveBlackBarsIn_hook(GB *gb) {
  CYC(0x53d3, 0x53d6); SET_HL(wGfxRegs1_LYC);
  CYC(0x53d6, 0x53d7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53d7, 0x53d8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53d8, 0x53d9); A = mem_rd(gb, HL);
  CYC(0x53d9, 0x53db); alu_cp(gb, 0x17);
  if (F & FC) {
    CYCT(0x53db, 0x53df);
  } else {
    CYC(0x53db, 0x53dd);
    CYC(0x53dd, 0x53df); mem_wr(gb, HL, 0x17);
  }
  CYC(0x53df, 0x53e2); SET_HL(wGfxRegs2_WINY);
  CYC(0x53e2, 0x53e3); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x53e3, 0x53e4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x53e4, 0x53e5); A = mem_rd(gb, HL);
  CYC(0x53e5, 0x53e7); alu_cp(gb, 0x78);
  if (!(F & FC)) {
    CYCT(0x53e7, 0x53e8); ret_effect(gb);
    return;
  }
  CYC(0x53e7, 0x53e8);
  CYC(0x53e8, 0x53ea); mem_wr(gb, HL, 0x78);
  CYC(0x53ea, 0x53eb); ret_effect(gb);
}

void introCinematic_moveBlackBarsOut_hook(GB *gb) {
  CYC(0x53eb, 0x53ee); SET_HL(wGfxRegs1_LYC);
  CYC(0x53ee, 0x53ef); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x53ef, 0x53f0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x53f0, 0x53f1); A = mem_rd(gb, HL);
  CYC(0x53f1, 0x53f3); alu_cp(gb, 0x2f);
  if (!(F & FC)) {
    CYCT(0x53f3, 0x53f7);
  } else {
    CYC(0x53f3, 0x53f5);
    CYC(0x53f5, 0x53f7); mem_wr(gb, HL, 0x2f);
  }
  CYC(0x53f7, 0x53fa); SET_HL(wGfxRegs2_WINY);
  CYC(0x53fa, 0x53fb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53fb, 0x53fc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53fc, 0x53fd); A = mem_rd(gb, HL);
  CYC(0x53fd, 0x53ff); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(0x53ff, 0x5400); ret_effect(gb);
    return;
  }
  CYC(0x53ff, 0x5400);
  CYC(0x5400, 0x5402); mem_wr(gb, HL, 0x60);
  CYC(0x5402, 0x5403); ret_effect(gb);
}

void clearFadingPalettes2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x542e, 0x5430); A = 0x02;
  CYC(0x5430, 0x5432); hram_wr(gb, 0x70, A);
  CYC(0x5432, 0x5435); SET_HL(0xdf80);
  CYC(0x5435, 0x5437); B = 0x80;
  CALL_C(0x5437, clearMemory_hook, 0x046f, 0x543a);
  CYC(0x543a, 0x543b); alu_xor(gb, A);
  CYC(0x543b, 0x543d); hram_wr(gb, 0x70, A);
  CYC(0x543d, 0x543e); A = alu_dec8(gb, A);
  CYC(0x543e, 0x5440); hram_wr(gb, 0xa9, A);
  CYC(0x5440, 0x5442); hram_wr(gb, 0xa7, A);
  CYC(0x5442, 0x5444); A = 0xfd;
  CYC(0x5444, 0x5446); hram_wr(gb, 0xa8, A);
  CYC(0x5446, 0x5448); hram_wr(gb, 0xa6, A);
  CYC(0x5448, 0x5449); ret_effect(gb);
}
