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
void func_712f_hook(GB *gb);

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

static void func_7460_body(GB *gb);

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

void cutscene_decCBB3whenFadeDone_hook(GB *gb) {
  CYC(0x6f96, 0x6f99); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6f99, 0x6f9a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6f9a, 0x6f9b); ret_effect(gb);
    return;
  }
  CYC(0x6f9a, 0x6f9b);
  CYC(0x6f9b, 0x6f9e); decCbb3_hook(gb);
}

void func_7098_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7098, 0x709a); A = 0x4d;
  CALL_C(0x709a, playSound_b00_hook, 0x0c98, 0x709d);
  CYC(0x709d, 0x709f); A = 0x01;
  CYC(0x709f, 0x70a2); mem_wr(gb, 0xc2ef, A);
  CYC(0x70a2, 0x70a4); A = 0x01;
  CYC(0x70a4, 0x70a7); mem_wr(gb, wScrollMode, A);
  CYC(0x70a7, 0x70a8); alu_xor(gb, A);
  CYC(0x70a8, 0x70ab); mem_wr(gb, wDisabledObjects, A);
  CYC(0x70ab, 0x70ae); mem_wr(gb, wMenuDisabled, A);
  CALL_C(0x70ae, loadTilesetAndRoomLayout_hook, 0x38a5, 0x70b1);
  CYC(0x70b1, 0x70b4); loadRoomCollisions_hook(gb);
}

void func_70f7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x70f7, 0x70f8); A = C;
  CYC(0x70f8, 0x70fa); hram_wr(gb, 0x8c, A);
  CYC(0x70fa, 0x70fb); A = B;
  CYC(0x70fb, 0x70fd); hram_wr(gb, 0x8d, A);
  CYC(0x70fd, 0x70ff); A = alu_swap(gb, A);
  CYC(0x70ff, 0x7101); alu_and(gb, 0x0f);
  CYC(0x7101, 0x7102); alu_add(gb, A);
  CYC(0x7102, 0x7103); E = A;
  CYC(0x7103, 0x7106); A = mem_rd(gb, wTmpcbb7);
  CYC(0x7106, 0x7107); alu_add(gb, E);
  CYC(0x7107, 0x7109); hram_wr(gb, 0x93, A);
  CYC(0x7109, 0x710b); C = 0x20;
  CALL_C(0x710b, multiplyAByC_hook, 0x019d, 0x710e);
  CYC(0x710e, 0x7111); SET_BC(w3VramTiles);
  CYC(0x7111, 0x7113); A = hram_rd(gb, 0x8d);
  CYC(0x7113, 0x7115); alu_and(gb, 0x0f);
  CALL_C(0x7115, addDoubleIndexToBc_hook, 0x007e, 0x7118);
  CYC(0x7118, 0x7119); alu_add_hl(gb, BC);
  CYC(0x7119, 0x711b); A = hram_rd(gb, 0x8c);
  CYC(0x711b, 0x711c); B = A;
  CYC(0x711c, 0x711e); A = 0x20;
  CYC(0x711e, 0x711f); alu_sub(gb, B);
  CYC(0x711f, 0x7121); hram_wr(gb, 0x8e, A);
  CYC(0x7121, 0x7122); push_effect(gb, HL);
  CYC(0x7122, 0x7123); C = D;
  CYC(0x7123, 0x7126); SET_DE(0xd000);
  CALL_C(0x7126, func_712f_hook, 0x712f, 0x7129);
  CYC(0x7129, 0x712a); SET_HL(pop_effect(gb));
  CYC(0x712a, 0x712c); H = (uint8_t)(H | 0x04);
  CYC(0x712c, 0x712f); SET_DE(0xd400);
  func_712f_hook(gb);
}

void func_712f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x712f, 0x7131); A = hram_rd(gb, 0x93);
  CYC(0x7131, 0x7132); C = A;
  CYC(0x7132, 0x7134); A = 0x14;
  CYC(0x7134, 0x7135); alu_sub(gb, C);
  if (F & FC) {
    CYCT(0x7135, 0x7136); ret_effect(gb);
    return;
  }
  CYC(0x7135, 0x7136);
  CYC(0x7136, 0x7137); C = A;
  do {
    CYC(0x7137, 0x7139); A = hram_rd(gb, 0x8c);
    CYC(0x7139, 0x713a); B = A;
    do {
      CYC(0x713a, 0x713c); A = 0x02;
      CYC(0x713c, 0x713e); mem_wr(gb, IO_SVBK, A);
      CYC(0x713e, 0x713f); A = mem_rd(gb, DE);
      CYC(0x713f, 0x7140); SET_DE(DE + 1);
      CYC(0x7140, 0x7142); hram_wr(gb, 0x8b, A);
      CYC(0x7142, 0x7144); A = 0x03;
      CYC(0x7144, 0x7146); mem_wr(gb, IO_SVBK, A);
      CYC(0x7146, 0x7148); A = hram_rd(gb, 0x8b);
      CYC(0x7148, 0x7149); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x7149, 0x714a); B = alu_dec8(gb, B);
      if (!(F & FZ)) CYCT(0x714a, 0x714c);
      else CYC(0x714a, 0x714c);
    } while (!(F & FZ));
    CYC(0x714c, 0x714e); A = hram_rd(gb, 0x8e);
    CALL_C(0x714e, addAToDe_hook, 0x0068, 0x7151);
    CYC(0x7151, 0x7153); A = hram_rd(gb, 0x8e);
    CYC(0x7153, 0x7154); push_effect(gb, 0x7154);
    burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L); L = A;
    burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
    if (!(F & FC)) {
      burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    } else {
      burn_rom(gb, 0x00, 0x0012, 0x0013, false);
      burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
      burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
    }
    CYC(0x7154, 0x7155); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(0x7155, 0x7157);
    else CYC(0x7155, 0x7157);
  } while (!(F & FZ));
  CYC(0x7157, 0x7158); ret_effect(gb);
}

void timewarpCutscene_incState_hook(GB *gb) {
  CYC(0x723a, 0x723d); SET_HL(wCutsceneState);
  CYC(0x723d, 0x723e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x723e, 0x723f); ret_effect(gb);
}

void func_7431_hook(GB *gb) {
  CYC(0x7431, 0x7432); push_effect(gb, HL);
  CYC(0x7432, 0x7435); A = mem_rd(gb, wTmpcbb8);
  CYC(0x7435, 0x7437); alu_and(gb, 0x07);
  CYC(0x7437, 0x743a); SET_HL(0x7440);
  CYC(0x743a, 0x743b); add_double_index_to_hl(gb, 0x743b);
  CYC(0x743b, 0x743c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x743c, 0x743d); E = mem_rd(gb, HL);
  CYC(0x743d, 0x743e); D = A;
  CYC(0x743e, 0x743f); SET_HL(pop_effect(gb));
  CYC(0x743f, 0x7440); ret_effect(gb);
}

void func_7450_hook(GB *gb) {
  CYC(0x7450, 0x7452); B = 0x2f;
  CYC(0x7452, 0x7454); C = 0x06;
  CYC(0x7454, 0x7456); func_7460_body(gb);
}

void func_7456_hook(GB *gb) {
  CYC(0x7456, 0x7458); B = 0x3f;
  CYC(0x7458, 0x745a); C = 0x06;
  CYC(0x745a, 0x745c); func_7460_body(gb);
}

void func_745c_hook(GB *gb) {
  CYC(0x745c, 0x745e); B = 0x3f;
  CYC(0x745e, 0x7460); C = 0x05;
  func_7460_body(gb);
}

static void func_7460_body(GB *gb) {
  CYC(0x7460, 0x7461); push_effect(gb, BC);
  CYC(0x7461, 0x7462); push_effect(gb, HL);
  CYC(0x7462, 0x7463); A = C;
  CYC(0x7463, 0x7465); mem_wr(gb, IO_SVBK, A);
  CYC(0x7465, 0x7467); B = 0;
  do {
    CYC(0x7467, 0x7468); A = mem_rd(gb, HL);
    CYC(0x7468, 0x7469); alu_and(gb, D);
    CYC(0x7469, 0x746a); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x746a, 0x746b); A = mem_rd(gb, HL);
    CYC(0x746b, 0x746c); alu_and(gb, D);
    CYC(0x746c, 0x746d); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x746d, 0x746e); A = mem_rd(gb, HL);
    CYC(0x746e, 0x746f); alu_and(gb, E);
    CYC(0x746f, 0x7470); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7470, 0x7471); A = mem_rd(gb, HL);
    CYC(0x7471, 0x7472); alu_and(gb, E);
    CYC(0x7472, 0x7473); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7473, 0x7474); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x7474, 0x7476);
    else CYC(0x7474, 0x7476);
  } while (!(F & FZ));
  CYC(0x7476, 0x7477); SET_HL(pop_effect(gb));
  CYC(0x7477, 0x7478); SET_BC(pop_effect(gb));
  CYC(0x7478, 0x7479); A = C;
  CYC(0x7479, 0x747b); alu_sub(gb, 0x05);
  CYC(0x747b, 0x747c); E = A;
  CYC(0x747c, 0x747d); A = H;
  CYC(0x747d, 0x747f); alu_and(gb, 0x8f);
  CYC(0x747f, 0x7480); D = A;
  CYC(0x7480, 0x7483); queueDmaTransfer_hook(gb);
}

void ambiPassageOpen_decCBB4_hook(GB *gb) {
  CYC(0x7483, 0x7486); SET_HL(wTmpcbb4);
  CYC(0x7486, 0x7487); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7487, 0x7488); ret_effect(gb);
    return;
  }
  CYC(0x7487, 0x7488);
  CYC(0x7488, 0x7489); ret_effect(gb);
}

void ambiPassageOpen_incState_hook(GB *gb) {
  CYC(0x7489, 0x748c); SET_HL(wCutsceneState);
  CYC(0x748c, 0x748d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x748d, 0x748e); ret_effect(gb);
}

void ambiPassageOpen_incCBB3_hook(GB *gb) {
  CYC(0x748e, 0x7491); SET_HL(wTmpcbb3);
  CYC(0x7491, 0x7492); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7492, 0x7493); ret_effect(gb);
}

void jabuOpen_decCBB4_hook(GB *gb) {
  CYC(0x7555, 0x7558); SET_HL(wTmpcbb4);
  CYC(0x7558, 0x7559); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7559, 0x755a); ret_effect(gb);
    return;
  }
  CYC(0x7559, 0x755a);
  CYC(0x755a, 0x755b); ret_effect(gb);
}

void jabuOpen_incState_hook(GB *gb) {
  CYC(0x755b, 0x755e); SET_HL(wCutsceneState);
  CYC(0x755e, 0x755f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x755f, 0x7560); ret_effect(gb);
}

void jabuOpen_incCBB3_hook(GB *gb) {
  CYC(0x7560, 0x7563); SET_HL(wTmpcbb3);
  CYC(0x7563, 0x7564); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7564, 0x7565); ret_effect(gb);
}

void cleanSeas_decCBB4_hook(GB *gb) {
  CYC(0x7609, 0x760c); SET_HL(wTmpcbb4);
  CYC(0x760c, 0x760d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x760d, 0x760e); ret_effect(gb);
    return;
  }
  CYC(0x760d, 0x760e);
  CYC(0x760e, 0x760f); ret_effect(gb);
}

void cleanSeas_incState_hook(GB *gb) {
  CYC(0x760f, 0x7612); SET_HL(wCutsceneState);
  CYC(0x7612, 0x7613); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7613, 0x7614); ret_effect(gb);
}

void cleanSeas_incCBB3_hook(GB *gb) {
  CYC(0x7614, 0x7617); SET_HL(wTmpcbb3);
  CYC(0x7617, 0x7618); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7618, 0x7619); ret_effect(gb);
}
