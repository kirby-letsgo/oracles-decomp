#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

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
