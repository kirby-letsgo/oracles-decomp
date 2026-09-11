#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

void nayruWarpToMakuTreeCutsceneHandler__func_6955_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__func_6962_hook(GB *gb);
void func_6f9e_hook(GB *gb);
void func_6fb0_hook(GB *gb);

void cutscene_incCutsceneState_hook(GB *gb) {
  CYC(0x6f8c, 0x6f8f); SET_HL(wCutsceneState);
  CYC(0x6f8f, 0x6f90); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f90, 0x6f91); ret_effect(gb);
}

void cutscene_decCBB6_hook(GB *gb) {
  CYC(0x6f91, 0x6f94); SET_HL(wTmpcbb6);
  CYC(0x6f94, 0x6f95); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6f95, 0x6f96); ret_effect(gb);
}

void cutscene_tickDownCBB4ThenSetTo30_hook(GB *gb) {
  CYC(0x700b, 0x700e); SET_HL(wTmpcbb4);
  CYC(0x700e, 0x700f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x700f, 0x7010); ret_effect(gb);
    return;
  }
  CYC(0x700f, 0x7010);
  CYC(0x7010, 0x7012); mem_wr(gb, HL, 0x1e);
  CYC(0x7012, 0x7013); ret_effect(gb);
}

void cutscene_incState_hook(GB *gb) {
  CYC(0x7013, 0x7016); SET_HL(wCutsceneState);
  CYC(0x7016, 0x7017); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7017, 0x7018); ret_effect(gb);
}

void cutscene_incCBB3_hook(GB *gb) {
  CYC(0x7018, 0x701b); SET_HL(wTmpcbb3);
  CYC(0x701b, 0x701c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x701c, 0x701d); ret_effect(gb);
}

void d2Collapse_decCBB4_hook(GB *gb) {
  CYC(0x7158, 0x715b); SET_HL(wTmpcbb4);
  CYC(0x715b, 0x715c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x715c, 0x715d); ret_effect(gb);
    return;
  }
  CYC(0x715c, 0x715d);
  CYC(0x715d, 0x715e); ret_effect(gb);
}

void timewarpCutscene_decCBB4_hook(GB *gb) {
  CYC(0x7234, 0x7237); SET_HL(wTmpcbb4);
  CYC(0x7237, 0x7238); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7238, 0x7239); ret_effect(gb);
    return;
  }
  CYC(0x7238, 0x7239);
  CYC(0x7239, 0x723a); ret_effect(gb);
}

void fairyCutscene_incState_hook(GB *gb) {
  CYC(0x6270, 0x6273); SET_HL(wCutsceneState);
  CYC(0x6273, 0x6274); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6274, 0x6275); ret_effect(gb);
}

void func_03_6275__bootedFromPalace_incState_hook(GB *gb) {
  CYC(0x62c8, 0x62cb); SET_HL(wCutsceneState);
  CYC(0x62cb, 0x62cc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x62cc, 0x62cd); ret_effect(gb);
}

void nayruSingingCutsceneHandler__func_6397_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6397, clearOam_hook, 0x049f, 0x639a);
  CYC(0x639a, 0x639c); B = 0x00;
  CYC(0x639c, 0x639f); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(0x639f, 0x63a0); alu_cpl(gb);
  CYC(0x63a0, 0x63a1); A = alu_inc8(gb, A);
  CYC(0x63a1, 0x63a2); C = A;
  CYC(0x63a2, 0x63a5); SET_HL(0x7249);
  CYC(0x63a5, 0x63a7); E = 0x3f;
  CYC(0x63a7, 0x63aa);
  addSpritesFromBankToOam_withOffset_hook(gb);
}

void nayruSingingCutsceneHandler__func_63db_hook(GB *gb) {
  CYC(0x63db, 0x63de); A = mem_rd(gb, wFrameCounter);
  CYC(0x63de, 0x63e0); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x63e0, 0x63e1); ret_effect(gb);
    return;
  }
  CYC(0x63e0, 0x63e1);
  CYC(0x63e1, 0x63e4); SET_HL(wTmpcbb6);
  CYC(0x63e4, 0x63e5); A = mem_rd(gb, HL);
  CYC(0x63e5, 0x63e6); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x63e6, 0x63e7); ret_effect(gb);
    return;
  }
  CYC(0x63e6, 0x63e7);
  CYC(0x63e7, 0x63e8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x63e8, 0x63eb); SET_HL(wGfxRegs1_SCX);
  CYC(0x63eb, 0x63ec); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x63ec, 0x63ed); ret_effect(gb);
}

void makuTreeDisappearingCutsceneHandler__label_03_119_hook(GB *gb) {
  CYC(0x6594, 0x6597); A = mem_rd(gb, wFrameCounter);
  CYC(0x6597, 0x6599); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x6599, 0x659a); ret_effect(gb);
    return;
  }
  CYC(0x6599, 0x659a);
  CYC(0x659a, 0x659d); SET_HL(wTmpcbb7);
  CYC(0x659d, 0x659e); A = mem_rd(gb, HL);
  CYC(0x659e, 0x659f); A = alu_inc8(gb, A);
  CYC(0x659f, 0x65a1); alu_and(gb, 0x03);
  CYC(0x65a1, 0x65a2); mem_wr(gb, HL, A);
  CYC(0x65a2, 0x65a5); SET_HL(0x65af);
  CYC(0x65a5, 0x65a6); push_effect(gb, 0x65a6);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(0x65a6, 0x65a7); A = mem_rd(gb, HL);
  CYC(0x65a7, 0x65aa);
  loadPaletteHeader_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6838_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6838, 0x683a); A = 0x00;
  CALL_C(0x683a, setLinkIDOverride_hook, 0x2acf, 0x683d);
  CYC(0x683d, 0x683f); L = 0x00;
  CYC(0x683f, 0x6841); mem_wr(gb, HL, 0x03);
  CYC(0x6841, 0x6843); L = 0x0b;
  CYC(0x6843, 0x6845); mem_wr(gb, HL, 0x68);
  CYC(0x6845, 0x6847); L = 0x0d;
  CYC(0x6847, 0x6849); mem_wr(gb, HL, 0x50);
  CYC(0x6849, 0x684b); L = 0x08;
  CYC(0x684b, 0x684d); mem_wr(gb, HL, 0x00);
  CYC(0x684d, 0x684e); ret_effect(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_692b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x692b, getFreePartSlot_hook, 0x3e8e, 0x692e);
  if (!(F & FZ)) {
    CYCT(0x692e, 0x6930);
  } else {
    CYC(0x692e, 0x6930);
    CYC(0x6930, 0x6932); mem_wr(gb, HL, 0x27);
    CYC(0x6932, 0x6933); L = alu_inc8(gb, L);
    CYC(0x6933, 0x6934); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x6934, 0x6935); L = alu_inc8(gb, L);
    CYC(0x6935, 0x6936); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x6936, 0x6938); L = 0xcb;
    CYC(0x6938, 0x693a); mem_wr(gb, HL, 0x40);
    CYC(0x693a, 0x693c); L = 0xcd;
    CYC(0x693c, 0x693e); mem_wr(gb, HL, 0x88);
  }
  CALL_C(0x693e, getFreeInteractionSlot_hook, 0x3aef, 0x6941);
  if (!(F & FZ)) {
    CYCT(0x6941, 0x6943);
  } else {
    CYC(0x6941, 0x6943);
    CYC(0x6943, 0x6945); mem_wr(gb, HL, 0x8d);
  }
  CYC(0x6945, 0x6948);
  cutscene_incCutsceneState_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6948_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6948, cutscene_decCBB6_hook, 0x6f91, 0x694b);
  if (!(F & FZ)) {
    CYCT(0x694b, 0x694c); ret_effect(gb);
    return;
  }
  CYC(0x694b, 0x694c);
  CYC(0x694c, 0x694e); L = 0xb7;
  CYC(0x694e, 0x694f); A = mem_rd(gb, HL);
  CYC(0x694f, 0x6951); alu_xor(gb, 0x02);
  CYC(0x6951, 0x6952); mem_wr(gb, HL, A);
  CALL_C(0x6952, nayruWarpToMakuTreeCutsceneHandler__func_6962_hook, 0x6962, 0x6955);
  nayruWarpToMakuTreeCutsceneHandler__func_6955_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6955_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6955, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6958);
  CYC(0x6958, 0x695a); alu_and(gb, 0x03);
  CYC(0x695a, 0x695b); alu_add(gb, A);
  CYC(0x695b, 0x695c); alu_add(gb, A);
  CYC(0x695c, 0x695e); alu_add(gb, 0x10);
  CYC(0x695e, 0x6961); mem_wr(gb, wTmpcbb6, A);
  CYC(0x6961, 0x6962); ret_effect(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6962_hook(GB *gb) {
  CYC(0x6962, 0x6965); mem_wr(gb, 0xd008, A);
  CYC(0x6965, 0x6967); A = 0x08;
  CYC(0x6967, 0x696a); mem_wr(gb, wLinkForceState, A);
  CYC(0x696a, 0x696b); ret_effect(gb);
}

void blackTowerCompleteCutsceneHandler__func_6a2b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6a2b, clearWramBank1_hook, 0x3597, 0x6a2e);
  CALL_C(0x6a2e, clearOam_hook, 0x049f, 0x6a31);
  CYC(0x6a31, 0x6a33); A = 0x05;
  CYC(0x6a33, 0x6a36); mem_wr(gb, wCutsceneState, A);
  CYC(0x6a36, 0x6a39); SET_BC(0x8d01);
  CYC(0x6a39, 0x6a3c);
  createInteraction_hook(gb);
}

void twinrovaRevealCutsceneHandler__func_6bc9_hook(GB *gb) {
  CYC(0x6bc9, 0x6bcc); SET_HL(wTmpcbb3);
  CYC(0x6bcc, 0x6bcd); alu_xor(gb, A);
  CYC(0x6bcd, 0x6bce); mem_wr(gb, HL, A);
  CYC(0x6bce, 0x6bcf); A = alu_dec8(gb, A);
  CYC(0x6bcf, 0x6bd2); mem_wr(gb, wTmpcbba, A);
  CYC(0x6bd2, 0x6bd5);
  cutscene_incCutsceneState_hook(gb);
}

void twinrovaRevealCutsceneHandler__func_6be8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6be8, func_6f9e_hook, 0x6f9e, 0x6beb);
  CYC(0x6beb, 0x6bed); A = 0x01;
  CYC(0x6bed, 0x6bf0); mem_wr(gb, wDisabledObjects, A);
  CALL_C(0x6bf0, clearDynamicInteractions_hook, 0x35d2, 0x6bf3);
  CYC(0x6bf3, 0x6bf6); SET_HL(0x77b2);
  CALL_C(0x6bf6, checkIsLinkedGame_hook, 0x1992, 0x6bf9);
  if (!(F & FZ)) {
    CYCT(0x6bf9, 0x6bfb);
  } else {
    CYC(0x6bf9, 0x6bfb);
    CYC(0x6bfb, 0x6bfe); SET_HL(wCutsceneState);
    CYC(0x6bfe, 0x6c00); mem_wr(gb, HL, 0x06);
    CYC(0x6c00, 0x6c03); SET_HL(0x77a5);
  }
  CALL_C(0x6c03, parseGivenObjectData_b00_hook, 0x3171, 0x6c06);
  CYC(0x6c06, 0x6c09);
  cutscene_incCutsceneState_hook(gb);
}

void pregameIntroCutsceneHandler__func_6d40_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d40, 0x6d42); A = 0x01;
  CYC(0x6d42, 0x6d43); mem_wr(gb, DE, A);
  CYC(0x6d43, 0x6d46); SET_BC(0x05f1);
  CALL_C(0x6d46, disableLcdAndLoadRoom_hook, 0x30b0, 0x6d49);
  CALL_C(0x6d49, resetCamera_hook, 0x12ce, 0x6d4c);
  CYC(0x6d4c, 0x6d4e); A = 0xac;
  CALL_C(0x6d4e, loadPaletteHeader_hook, 0x050b, 0x6d51);
  CYC(0x6d51, 0x6d54); SET_HL(0x77b6);
  CALL_C(0x6d54, parseGivenObjectData_b00_hook, 0x3171, 0x6d57);
  CYC(0x6d57, 0x6d59); A = 0x1b;
  CALL_C(0x6d59, playSound_b00_hook, 0x0c98, 0x6d5c);
  CYC(0x6d5c, 0x6d5f); SET_HL(wTmpcbb3);
  CYC(0x6d5f, 0x6d61); mem_wr(gb, HL, 0x3c);
  CYC(0x6d61, 0x6d63); A = 0x13;
  CALL_C(0x6d63, loadGfxRegisterStateIndex_hook, 0x02ea, 0x6d66);
  CYC(0x6d66, 0x6d69); A = mem_rd(gb, wGfxRegs2_SCX);
  CYC(0x6d69, 0x6d6b); hram_wr(gb, 0xac, A);
  CYC(0x6d6b, 0x6d6c); alu_xor(gb, A);
  CYC(0x6d6c, 0x6d6e); hram_wr(gb, 0xaa, A);
  CYC(0x6d6e, 0x6d70); A = 0x00;
  CYC(0x6d70, 0x6d73); mem_wr(gb, wScrollMode, A);
  CYC(0x6d73, 0x6d76);
  clearFadingPalettes2_hook(gb);
}

void func_6f9e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f9e, 0x6fa0); A = hram_rd(gb, 0x70);
  CYC(0x6fa0, 0x6fa1); push_effect(gb, AF);
  CYC(0x6fa1, 0x6fa3); A = 0x02;
  CYC(0x6fa3, 0x6fa5); hram_wr(gb, 0x70, A);
  CYC(0x6fa5, 0x6fa8); SET_HL(0xde90);
  CYC(0x6fa8, 0x6faa); B = 0x30;
  CALL_C(0x6faa, clearMemory_hook, 0x046f, 0x6fad);
  CYC(0x6fad, 0x6fae); SET_AF(pop_effect(gb));
  CYC(0x6fae, 0x6fb0); hram_wr(gb, 0x70, A);
  func_6fb0_hook(gb);
}

void func_6fb0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6fb0, 0x6fb2); A = hram_rd(gb, 0x70);
  CYC(0x6fb2, 0x6fb3); push_effect(gb, AF);
  CYC(0x6fb3, 0x6fb5); A = 0x02;
  CYC(0x6fb5, 0x6fb7); hram_wr(gb, 0x70, A);
  CYC(0x6fb7, 0x6fba); SET_HL(0xdf80);
  CYC(0x6fba, 0x6fbc); B = 0x80;
  CALL_C(0x6fbc, clearMemory_hook, 0x046f, 0x6fbf);
  CYC(0x6fbf, 0x6fc0); SET_AF(pop_effect(gb));
  CYC(0x6fc0, 0x6fc2); hram_wr(gb, 0x70, A);
  CALL_C(0x6fc2, hideStatusBar_hook, 0x1aa0, 0x6fc5);
  CYC(0x6fc5, 0x6fc7); A = 0xfc;
  CYC(0x6fc7, 0x6fc9); hram_wr(gb, 0xa8, A);
  CYC(0x6fc9, 0x6fcb); hram_wr(gb, 0xa6, A);
  CYC(0x6fcb, 0x6fcc); alu_xor(gb, A);
  CYC(0x6fcc, 0x6fcf); mem_wr(gb, wScrollMode, A);
  CYC(0x6fcf, 0x6fd2); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(0x6fd2, 0x6fd5); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(0x6fd5, 0x6fd6); ret_effect(gb);
}
