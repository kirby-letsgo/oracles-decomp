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
void func_6f0b_hook(GB *gb);
void func_6f44__cbb8_00_hook(GB *gb);
void func_6f44__cbb8_01_hook(GB *gb);
void func_6f44__cbb8_02_hook(GB *gb);
void func_712f_hook(GB *gb);
void func_03_7619__state1_hook(GB *gb);
void func_03_7619__state2_hook(GB *gb);
void func_03_7619__state3_hook(GB *gb);
void func_03_7619__state4_hook(GB *gb);
void func_03_7619__state5_hook(GB *gb);
void func_03_7619__state5__cbb3_03_hook(GB *gb);
void func_03_7619__state6_hook(GB *gb);
void fairyCutscene_cfd1is07_hook(GB *gb);
void func_03_6103__state0_hook(GB *gb);
void func_03_6103__state2_hook(GB *gb);
void func_03_6103__spawnForestFairy_hook(GB *gb);
void func_03_6103__state3_hook(GB *gb);
void func_03_6103__state5_hook(GB *gb);
void func_03_6103__state8_hook(GB *gb);
void func_03_6103__stateB_hook(GB *gb);
void fairyCutscene_cfd1is07__state0_hook(GB *gb);
void fairyCutscene_cfd1is07__state1_hook(GB *gb);
void fairyCutscene_cfd1is07__state2_hook(GB *gb);
void fairyCutscene_cfd1is07__state3_hook(GB *gb);
void fairyCutscene_cfd1is07__state4_hook(GB *gb);
void fairyCutscene_cfd1is07__state5_hook(GB *gb);
void nayruSingingCutsceneHandler__state0_hook(GB *gb);
void nayruSingingCutsceneHandler__state1_hook(GB *gb);
void nayruSingingCutsceneHandler__state2_hook(GB *gb);
void nayruSingingCutsceneHandler__state3_hook(GB *gb);
void nayruSingingCutsceneHandler__state4_hook(GB *gb);
void nayruSingingCutsceneHandler__state5_hook(GB *gb);
void nayruSingingCutsceneHandler__state6_hook(GB *gb);
void nayruSingingCutsceneHandler__state7_hook(GB *gb);
void nayruSingingCutsceneHandler__state8_hook(GB *gb);
void nayruSingingCutsceneHandler__state9_hook(GB *gb);
void nayruSingingCutsceneHandler__stateA_hook(GB *gb);
void nayruSingingCutsceneHandler__stateB_hook(GB *gb);
void nayruSingingCutsceneHandler__stateC_hook(GB *gb);
void nayruSingingStateE_hook(GB *gb);
void nayruSingingState10_hook(GB *gb);
void nayruSingingState11_hook(GB *gb);
void pregameIntroCutsceneHandler_hook(GB *gb);
void pregameIntroCutsceneHandler__runStates_hook(GB *gb);
void pregameIntroCutsceneHandler__state0_hook(GB *gb);
void pregameIntroCutsceneHandler__state1_hook(GB *gb);
void pregameIntroCutsceneHandler__state2_hook(GB *gb);
void pregameIntroCutsceneHandler__state3_hook(GB *gb);
void pregameIntroCutsceneHandler__state4_hook(GB *gb);
void pregameIntroCutsceneHandler__state5_hook(GB *gb);
void pregameIntroCutsceneHandler__state6_hook(GB *gb);
void pregameIntroCutsceneHandler__state7_hook(GB *gb);
void pregameIntroCutsceneHandler__state8_hook(GB *gb);
void pregameIntroCutsceneHandler__state9_hook(GB *gb);
void pregameIntroCutsceneHandler__stateA_hook(GB *gb);
void pregameIntroCutsceneHandler__stateB_hook(GB *gb);
void pregameIntroCutsceneHandler__stateC_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_00__state0_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_00__state1_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_00__state2_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01__state5_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01__state7_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_02__state1_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_02__state2_hook(GB *gb);
void func_6733_hook(GB *gb);
void func_6733__cbb8_00_hook(GB *gb);
void func_6733__cbb8_02_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state0_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state1_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state2_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state3_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state5_hook(GB *gb);
void makuTreeDisappearingCutsceneHandler_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state1_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state2_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state3_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state4_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state5_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state7_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state9_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__stateA_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__stateB_hook(GB *gb);
void turnToStoneCutsceneHandler__state0_hook(GB *gb);
void turnToStoneCutsceneHandler__state2_hook(GB *gb);
void turnToStoneCutsceneHandler__state4_hook(GB *gb);
void turnToStoneCutsceneHandler__state6_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state0_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state1_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state2_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state3_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state5_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state7_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state8_hook(GB *gb);

static void maku_tree_disappearing_body(GB *gb, uint16_t sp0_);

static void add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

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

static uint16_t misc_cutscene_jump_table(GB *gb) {
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

void func_6e9a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e9a, 0x6e9c); hram_wr(gb, 0x8b, A);
  CYC(0x6e9c, 0x6e9e); A = 0x01;
  CYC(0x6e9e, 0x6ea0); hram_wr(gb, 0x4f, A);
  CYC(0x6ea0, 0x6ea3); SET_HL(0x9800);
  CYC(0x6ea3, 0x6ea6); SET_BC(0x0400);
  CYC(0x6ea6, 0x6ea8); A = hram_rd(gb, 0x8b);
  CALL_C(0x6ea8, fillMemoryBc_hook, 0x0476, 0x6eab);
  CYC(0x6eab, 0x6eac); alu_xor(gb, A);
  CYC(0x6eac, 0x6eae); hram_wr(gb, 0x4f, A);
  CYC(0x6eae, 0x6eb1); SET_HL(0x9800);
  CYC(0x6eb1, 0x6eb4); SET_BC(0x0400);
  CYC(0x6eb4, 0x6eb7); clearMemoryBc_hook(gb);
}

void func_6eb7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6eb7, 0x6eb9); A = hram_rd(gb, 0x70);
  CYC(0x6eb9, 0x6eba); push_effect(gb, AF);
  CYC(0x6eba, 0x6ebc); A = 0x03;
  CYC(0x6ebc, 0x6ebe); hram_wr(gb, 0x70, A);
  CYC(0x6ebe, 0x6ec1); SET_HL(w3VramTiles);
  CYC(0x6ec1, 0x6ec4); SET_BC(0x0240);
  CALL_C(0x6ec4, clearMemoryBc_hook, 0x0475, 0x6ec7);
  CYC(0x6ec7, 0x6eca); SET_HL(w3VramAttributes);
  CYC(0x6eca, 0x6ecd); SET_BC(0x0240);
  CYC(0x6ecd, 0x6ecf); A = 0x02;
  CALL_C(0x6ecf, fillMemoryBc_hook, 0x0476, 0x6ed2);
  CYC(0x6ed2, 0x6ed3); SET_AF(pop_effect(gb));
  CYC(0x6ed3, 0x6ed5); hram_wr(gb, 0x70, A);
  CYC(0x6ed5, 0x6ed6); ret_effect(gb);
}

void func_6ed6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ed6, 0x6ed8); hram_wr(gb, 0x8b, A);
  CYC(0x6ed8, 0x6eda); A = hram_rd(gb, 0x70);
  CYC(0x6eda, 0x6edb); push_effect(gb, AF);
  CYC(0x6edb, 0x6edd); A = 0x04;
  CYC(0x6edd, 0x6edf); hram_wr(gb, 0x70, A);
  CYC(0x6edf, 0x6ee2); SET_HL(0xd000);
  CYC(0x6ee2, 0x6ee5); SET_BC(0x0240);
  CALL_C(0x6ee5, clearMemoryBc_hook, 0x0475, 0x6ee8);
  CYC(0x6ee8, 0x6eeb); SET_HL(0xd400);
  CYC(0x6eeb, 0x6eee); SET_BC(0x0240);
  CYC(0x6eee, 0x6ef0); A = hram_rd(gb, 0x8b);
  CALL_C(0x6ef0, fillMemoryBc_hook, 0x0476, 0x6ef3);
  CYC(0x6ef3, 0x6ef4); SET_AF(pop_effect(gb));
  CYC(0x6ef4, 0x6ef6); hram_wr(gb, 0x70, A);
  CYC(0x6ef6, 0x6ef7); ret_effect(gb);
}

void func_6ef7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ef7, 0x6efa); A = mem_rd(gb, wTmpcbb9);
  CYC(0x6efa, 0x6efb); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6efb, 0x6efd);
    func_6f0b_hook(gb);
    return;
  }
  CYC(0x6efb, 0x6efd);
  CYC(0x6efd, 0x6f00); SET_HL(wTmpcbb7);
  CYC(0x6f00, 0x6f02); B = 0x01;
  CALL_C(0x6f02, flashScreen_hook, 0x2d73, 0x6f05);
  if (F & FZ) {
    CYCT(0x6f05, 0x6f06); ret_effect(gb);
    return;
  }
  CYC(0x6f05, 0x6f06);
  CYC(0x6f06, 0x6f07); alu_xor(gb, A);
  CYC(0x6f07, 0x6f0a); mem_wr(gb, wTmpcbb9, A);
  CYC(0x6f0a, 0x6f0b); ret_effect(gb);
}

void func_6f0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f0b, 0x6f0e); A = mem_rd(gb, wFrameCounter);
  CYC(0x6f0e, 0x6f10); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(0x6f10, 0x6f11); ret_effect(gb);
    return;
  }
  CYC(0x6f10, 0x6f11);
  CALL_C(0x6f11, getRandomNumber_hook, 0x043e, 0x6f14);
  CYC(0x6f14, 0x6f16); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x6f16, 0x6f17); ret_effect(gb);
    return;
  }
  CYC(0x6f16, 0x6f17);
  CYC(0x6f17, 0x6f1a); mem_wr(gb, wTmpcbb7, A);
  CYC(0x6f1a, 0x6f1b); A = alu_dec8(gb, A);
  CYC(0x6f1b, 0x6f1e); mem_wr(gb, wTmpcbb9, A);
  CYC(0x6f1e, 0x6f21); mem_wr(gb, wTmpcbba, A);
  CYC(0x6f21, 0x6f23); A = 0xd2;
  CYC(0x6f23, 0x6f26); playSound_b00_hook(gb);
}

void func_6f26_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f26, 0x6f29); SET_HL(wTmpcbb6);
  CYC(0x6f29, 0x6f2a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x6f2a, 0x6f2b); ret_effect(gb);
    return;
  }
  CYC(0x6f2a, 0x6f2b);
  CALL_C(0x6f2b, decCbb3_hook, 0x305d, 0x6f2e);
  if (F & FZ) {
    CYCT(0x6f2e, 0x6f2f); ret_effect(gb);
    return;
  }
  CYC(0x6f2e, 0x6f2f);
  CYC(0x6f2f, 0x6f32); A = mem_rd(gb, wTmpcbbb);
  CYC(0x6f32, 0x6f35); mem_wr(gb, wTmpcbb6, A);
  CYC(0x6f35, 0x6f38); SET_HL(wGfxRegs1_SCY);
  CYC(0x6f38, 0x6f39); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6f39, 0x6f3a); A = mem_rd(gb, HL);
  CYC(0x6f3a, 0x6f3b); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6f3b, 0x6f3c); ret_effect(gb);
    return;
  }
  CYC(0x6f3b, 0x6f3c);
  CYC(0x6f3c, 0x6f3e); A = 0x34;
  CALL_C(0x6f3e, loadUncompressedGfxHeader_hook, 0x05da, 0x6f41);
  CYC(0x6f41, 0x6f43); alu_or(gb, 0x01);
  CYC(0x6f43, 0x6f44); ret_effect(gb);
}

void func_6f44_hook(GB *gb) {
  CYC(0x6f44, 0x6f47); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x6f47, 0x6f48); alu_cpl(gb);
  CYC(0x6f48, 0x6f49); A = alu_inc8(gb, A);
  CYC(0x6f49, 0x6f4a); B = A;
  CYC(0x6f4a, 0x6f4b); alu_xor(gb, A);
  CYC(0x6f4b, 0x6f4d); hram_wr(gb, 0x9f, A);
  CYC(0x6f4d, 0x6f4e); C = A;
  CYC(0x6f4e, 0x6f51); A = mem_rd(gb, wTmpcbb8);
  CYC(0x6f51, 0x6f52); push_effect(gb, 0x6f52);
  switch (misc_cutscene_jump_table(gb)) {
    case 0x6f58: func_6f44__cbb8_00_hook(gb); return;
    case 0x6f60: func_6f44__cbb8_01_hook(gb); return;
    case 0x6f70: func_6f44__cbb8_02_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_6f44__cbb8_00_hook(GB *gb) {
  CYC(0x6f58, 0x6f5b); SET_HL(0x714c);
  CYC(0x6f5b, 0x6f5d); E = 0x3f;
  CYC(0x6f5d, 0x6f60); addSpritesFromBankToOam_withOffset_hook(gb);
}

void func_6f44__cbb8_01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f60, 0x6f63); SET_HL(0x718d);
  CYC(0x6f63, 0x6f65); E = 0x3f;
  CALL_C(0x6f65, addSpritesFromBankToOam_withOffset_hook, 0x30d8, 0x6f68);
  CYC(0x6f68, 0x6f6b); SET_HL(0x71ce);
  CYC(0x6f6b, 0x6f6d); E = 0x3f;
  CYC(0x6f6d, 0x6f70); addSpritesFromBankToOam_withOffset_hook(gb);
}

void func_6f44__cbb8_02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f70, 0x6f73); SET_HL(0x71f7);
  CYC(0x6f73, 0x6f75); E = 0x3f;
  CALL_C(0x6f75, addSpritesFromBankToOam_withOffset_hook, 0x30d8, 0x6f78);
  CYC(0x6f78, 0x6f7b); SET_HL(0x718d);
  CYC(0x6f7b, 0x6f7d); E = 0x3f;
  CYC(0x6f7d, 0x6f80); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x6f80, 0x6f82); alu_cp(gb, 0x71);
  if (F & FC) {
    CYCT(0x6f82, 0x6f84);
  } else {
    CYC(0x6f82, 0x6f84);
    CYC(0x6f84, 0x6f87); SET_HL(0x7220);
    CYC(0x6f87, 0x6f89); E = 0x3f;
  }
  CYC(0x6f89, 0x6f8c); addSpritesFromBankToOam_withOffset_hook(gb);
}

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

void func_03_6103_hook(GB *gb) {
  CYC(0x6103, 0x6106); A = mem_rd(gb, wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CYC(0x6106, 0x6108); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(0x6108, 0x610b);
    fairyCutscene_cfd1is07_hook(gb);
    return;
  }
  CYC(0x6108, 0x610b);
  CYC(0x610b, 0x610e); A = mem_rd(gb, wCutsceneState);
  CYC(0x610e, 0x610f); push_effect(gb, 0x610f);
  switch (misc_cutscene_jump_table(gb)) {
    case 0x6127: func_03_6103__state0_hook(gb); return;
    case 0x6175: func_03_6103__state2_hook(gb); return;
    case 0x6188: func_03_6103__state3_hook(gb); return;
    case 0x619f: func_03_6103__state5_hook(gb); return;
    case 0x61b1: func_03_6103__state8_hook(gb); return;
    case 0x61dc: func_03_6103__stateB_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_6103__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6127, 0x612a); SET_HL(wTmpcbb3);
  CYC(0x612a, 0x612d); A = mem_rd(gb, w1Link_yh);
  CYC(0x612d, 0x612e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x612e, 0x6131); A = mem_rd(gb, w1Link_xh);
  CYC(0x6131, 0x6132); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6132, 0x6135); A = mem_rd(gb, w1Link_direction);
  CYC(0x6135, 0x6136); mem_wr(gb, HL, A);
  CALL_C(0x6136, fadeoutToWhite_hook, 0x326c, 0x6139);
  CYC(0x6139, 0x613c); fairyCutscene_incState_hook(gb);
}

void func_03_6103__state2_hook(GB *gb) {
  CYC(0x6175, 0x6178); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6178, 0x6179); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6179, 0x617a); ret_effect(gb);
    return;
  }
  CYC(0x6179, 0x617a);
  CYC(0x617a, 0x617c); B = 0x0c;
  func_03_6103__spawnForestFairy_hook(gb);
}

void func_03_6103__spawnForestFairy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x617c, getFreeInteractionSlot_hook, 0x3aef, 0x617f);
  if (!(F & FZ)) {
    CYCT(0x617f, 0x6180); ret_effect(gb);
    return;
  }
  CYC(0x617f, 0x6180);
  CYC(0x6180, 0x6182); mem_wr(gb, HL, 0x49);
  CYC(0x6182, 0x6184); L = 0x43;
  CYC(0x6184, 0x6185); mem_wr(gb, HL, B);
  CYC(0x6185, 0x6188); fairyCutscene_incState_hook(gb);
}

void func_03_6103__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6188, 0x618b); SET_HL(wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(0x618b, 0x618c); A = mem_rd(gb, HL);
  CYC(0x618c, 0x618d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x618d, 0x618e); ret_effect(gb);
    return;
  }
  CYC(0x618d, 0x618e);
  CYC(0x618e, 0x6190); mem_wr(gb, HL, 0x00);
  CALL_C(0x6190, fairyCutscene_incState_hook, 0x6270, 0x6193);
  CYC(0x6193, 0x6196); fadeoutToWhite_hook(gb);
}

void func_03_6103__state5_hook(GB *gb) {
  CYC(0x619f, 0x61a2); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x61a2, 0x61a3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61a3, 0x61a4); ret_effect(gb);
    return;
  }
  CYC(0x61a3, 0x61a4);
  CYC(0x61a4, 0x61a6); B = 0x0d;
  CYC(0x61a6, 0x61a8); func_03_6103__spawnForestFairy_hook(gb);
}

void func_03_6103__state8_hook(GB *gb) {
  CYC(0x61b1, 0x61b4); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x61b4, 0x61b5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61b5, 0x61b6); ret_effect(gb);
    return;
  }
  CYC(0x61b5, 0x61b6);
  CYC(0x61b6, 0x61b8); B = 0x0e;
  CYC(0x61b8, 0x61bb); func_03_6103__spawnForestFairy_hook(gb);
}

void func_03_6103__stateB_hook(GB *gb) {
  CYC(0x61dc, 0x61df); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x61df, 0x61e0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61e0, 0x61e1); ret_effect(gb);
    return;
  }
  CYC(0x61e0, 0x61e1);
  CYC(0x61e1, 0x61e2); alu_xor(gb, A);
  CYC(0x61e2, 0x61e5); mem_wr(gb, wDisabledObjects, A);
  CYC(0x61e5, 0x61e8); mem_wr(gb, wMenuDisabled, A);
  CYC(0x61e8, 0x61e9); A = alu_inc8(gb, A);
  CYC(0x61e9, 0x61ec); mem_wr(gb, 0xc2ef, A);
  CYC(0x61ec, 0x61ef); SET_BC(0x1104);
  CYC(0x61ef, 0x61f2); showText_hook(gb);
}

void fairyCutscene_cfd1is07_hook(GB *gb) {
  CYC(0x61f2, 0x61f5); A = mem_rd(gb, wCutsceneState);
  CYC(0x61f5, 0x61f6); push_effect(gb, 0x61f6);
  switch (misc_cutscene_jump_table(gb)) {
    case 0x6202: fairyCutscene_cfd1is07__state0_hook(gb); return;
    case 0x6210: fairyCutscene_cfd1is07__state1_hook(gb); return;
    case 0x6225: fairyCutscene_cfd1is07__state2_hook(gb); return;
    case 0x6231: fairyCutscene_cfd1is07__state3_hook(gb); return;
    case 0x624c: fairyCutscene_cfd1is07__state4_hook(gb); return;
    case 0x625a: fairyCutscene_cfd1is07__state5_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void fairyCutscene_cfd1is07__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6202, 0x6205); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6205, 0x6206); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6206, 0x6207); ret_effect(gb);
    return;
  }
  CYC(0x6206, 0x6207);
  CYC(0x6207, 0x620a); SET_BC(0x110a);
  CALL_C(0x620a, showText_hook, 0x1872, 0x620d);
  CYC(0x620d, 0x6210); fairyCutscene_incState_hook(gb);
}

void fairyCutscene_cfd1is07__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6210, 0x6213); A = mem_rd(gb, wTextIsActive);
  CYC(0x6213, 0x6214); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6214, 0x6215); ret_effect(gb);
    return;
  }
  CYC(0x6214, 0x6215);
  CALL_C(0x6215, fairyCutscene_incState_hook, 0x6270, 0x6218);
  CYC(0x6218, 0x621a); A = 0x0c;
  CYC(0x621a, 0x621d); mem_wr(gb, wTmpcbb6, A);
  CYC(0x621d, 0x621f); A = 0x7b;
  CALL_C(0x621f, playSound_b00_hook, 0x0c98, 0x6222);
  CYC(0x6222, 0x6225); fastFadeinFromWhite_hook(gb);
}

void fairyCutscene_cfd1is07__state2_hook(GB *gb) {
  CYC(0x6225, 0x6228); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6228, 0x6229); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6229, 0x622a); ret_effect(gb);
    return;
  }
  CYC(0x6229, 0x622a);
  CYC(0x622a, 0x622d); SET_HL(wTmpcbb6);
  CYC(0x622d, 0x622e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x622e, 0x622f); ret_effect(gb);
    return;
  }
  CYC(0x622e, 0x622f);
  CYC(0x622f, 0x6231); fairyCutscene_cfd1is07__state1_hook(gb);
}

void fairyCutscene_cfd1is07__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6231, 0x6234); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6234, 0x6235); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6235, 0x6236); ret_effect(gb);
    return;
  }
  CYC(0x6235, 0x6236);
  CYC(0x6236, 0x6239); SET_HL(wTmpcbb6);
  CYC(0x6239, 0x623a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x623a, 0x623b); ret_effect(gb);
    return;
  }
  CYC(0x623a, 0x623b);
  CALL_C(0x623b, fairyCutscene_incState_hook, 0x6270, 0x623e);
  CYC(0x623e, 0x623f); alu_xor(gb, A);
  CYC(0x623f, 0x6242); mem_wr(gb, wTmpcfc0_fairyHideAndSeek_active, A);
  CYC(0x6242, 0x6244); A = 0x7b;
  CALL_C(0x6244, playSound_b00_hook, 0x0c98, 0x6247);
  CYC(0x6247, 0x6249); A = 0x08;
  CYC(0x6249, 0x624c); fadeinFromWhiteWithDelay_hook(gb);
}

void fairyCutscene_cfd1is07__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x624c, 0x624f); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x624f, 0x6250); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6250, 0x6251); ret_effect(gb);
    return;
  }
  CYC(0x6250, 0x6251);
  CALL_C(0x6251, fairyCutscene_incState_hook, 0x6270, 0x6254);
  CYC(0x6254, 0x6257); SET_BC(0x110b);
  CYC(0x6257, 0x625a); showText_hook(gb);
}

void fairyCutscene_cfd1is07__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x625a, 0x625c); A = 0x0e;
  CALL_C(0x625c, setGlobalFlag_hook, 0x31f9, 0x625f);
  CYC(0x625f, 0x6261); A = 0x2b;
  CALL_C(0x6261, setGlobalFlag_hook, 0x31f9, 0x6264);
  CYC(0x6264, 0x6265); alu_xor(gb, A);
  CYC(0x6265, 0x6268); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6268, 0x626b); mem_wr(gb, wDisabledObjects, A);
  CYC(0x626b, 0x626c); A = alu_inc8(gb, A);
  CYC(0x626c, 0x626f); mem_wr(gb, 0xc2ef, A);
  CYC(0x626f, 0x6270); ret_effect(gb);
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

void makuTreeDisappearingCutsceneHandler_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6564, 0x6567); push_effect(gb, 0x6567);
  maku_tree_disappearing_body(gb, sp0_);
  CYC(0x6567, 0x656a);
  updateAllObjects_hook(gb);
}

static void maku_tree_disappearing_body(GB *gb, uint16_t sp0_) {
  CYC(0x656a, 0x656d); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x656d, 0x656e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x656e, 0x6570);
    makuTreeDisappearingCutsceneHandler__label_03_119_hook(gb);
    return;
  }
  CYC(0x656e, 0x6570);
  CYC(0x6570, 0x6572); A = 0xb4;
  CALL_C(0x6572, playSound_b00_hook, 0x0c98, 0x6575);
  CYC(0x6575, 0x6576); alu_xor(gb, A);
  CYC(0x6576, 0x6579); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x6579, 0x657c); mem_wr(gb, wMenuDisabled, A);
  CYC(0x657c, 0x657e); A = 0x0c;
  CALL_C(0x657e, setGlobalFlag_hook, 0x31f9, 0x6581);
  CALL_C(0x6581, getThisRoomFlags_hook, 0x197d, 0x6584);
  CYC(0x6584, 0x6586); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x6586, 0x6587); alu_xor(gb, A);
  CYC(0x6587, 0x658a); mem_wr(gb, wUseSimulatedInput, A);
  CYC(0x658a, 0x658b); A = alu_inc8(gb, A);
  CYC(0x658b, 0x658e); mem_wr(gb, wDisabledObjects, A);
  CYC(0x658e, 0x6591); SET_HL(0x65aa);
  CYC(0x6591, 0x6594);
  setWarpDestVariables_hook(gb);
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

void blackTowerExplanationCutsceneHandler__cbb8_00__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65d0, 0x65d3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x65d3, 0x65d4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x65d4, 0x65d5); ret_effect(gb); return; }
  CYC(0x65d4, 0x65d5);
  CALL_C(0x65d5, disableLcd_hook, 0x02c1, 0x65d8);
  CALL_C(0x65d8, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x65db);
  CALL_C(0x65db, clearOam_hook, 0x049f, 0x65de);
  CYC(0x65de, 0x65e1); A = mem_rd(gb, wTmpcbb8);
  CYC(0x65e1, 0x65e4); SET_HL(0x6625);
  CYC(0x65e4, 0x65e5); add_double_index_to_hl(gb, 0x65e5);
  CYC(0x65e5, 0x65e6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65e6, 0x65e7); push_effect(gb, HL);
  CALL_C(0x65e7, loadGfxHeader_hook, 0x0626, 0x65ea);
  CYC(0x65ea, 0x65eb); SET_HL(pop_effect(gb));
  CYC(0x65eb, 0x65ec); A = mem_rd(gb, HL);
  CALL_C(0x65ec, loadGfxHeader_hook, 0x0626, 0x65ef);
  CYC(0x65ef, 0x65f1); A = 0xc3;
  CALL_C(0x65f1, loadPaletteHeader_hook, 0x050b, 0x65f4);
  CYC(0x65f4, 0x65f6); B = 0x78;
  CYC(0x65f6, 0x65f9); A = mem_rd(gb, wTmpcbb8);
  CYC(0x65f9, 0x65fb); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x65fb, 0x65fd);
  } else {
    CYC(0x65fb, 0x65fd);
    CYC(0x65fd, 0x65ff); B = 0x3c;
  }
  CYC(0x65ff, 0x6602); SET_HL(wTmpcbb3);
  CYC(0x6602, 0x6603); mem_wr(gb, HL, B);
  CYC(0x6603, 0x6604); alu_or(gb, A);
  CYC(0x6604, 0x6606); A = 0x21;
  if (F & FZ) CALL_C_CC(0x6606, playSound_b00_hook, 0x0c98, 0x6609);
  else CYC(0x6606, 0x6609);
  CALL_C(0x6609, cutscene_incCutsceneState_hook, 0x6f8c, 0x660c);
  CYC(0x660c, 0x660d); alu_xor(gb, A);
  CYC(0x660d, 0x6610); mem_wr(gb, wTmpcbb9, A);
  CALL_C(0x6610, fadeinFromWhite_hook, 0x3299, 0x6613);
  CYC(0x6613, 0x6615); A = 0x70;
  CYC(0x6615, 0x6618); mem_wr(gb, wScreenOffsetY, A);
  CYC(0x6618, 0x661b); SET_HL(0xcc10);
  CYC(0x661b, 0x661d); B = 0x08;
  CALL_C(0x661d, clearMemory_hook, 0x046f, 0x6620);
  CYC(0x6620, 0x6622); A = 0x09;
  CYC(0x6622, 0x6625);
  loadGfxRegisterStateIndex_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_00__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x662b, func_6ef7_hook, 0x6ef7, 0x662e);
  CALL_C(0x662e, func_6f44_hook, 0x6f44, 0x6631);
  CYC(0x6631, 0x6634); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6634, 0x6635); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6635, 0x6636); ret_effect(gb); return; }
  CYC(0x6635, 0x6636);
  CALL_C(0x6636, decCbb3_hook, 0x305d, 0x6639);
  if (!(F & FZ)) { CYCT(0x6639, 0x663a); ret_effect(gb); return; }
  CYC(0x6639, 0x663a);
  CALL_C(0x663a, cutscene_incCutsceneState_hook, 0x6f8c, 0x663d);
  CYC(0x663d, 0x6640); SET_BC(0x1005);
  CYC(0x6640, 0x6643); A = mem_rd(gb, wTmpcbb8);
  CYC(0x6643, 0x6644); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6644, 0x6646);
  } else {
    CYC(0x6644, 0x6646);
    CYC(0x6646, 0x6649); SET_BC(0x1317);
  }
  CYC(0x6649, 0x664b); A = 0x01;
  CYC(0x664b, 0x664e); mem_wr(gb, wTextboxFlags, A);
  CYC(0x664e, 0x6651);
  showText_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_00__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6651, func_6ef7_hook, 0x6ef7, 0x6654);
  CALL_C(0x6654, func_6f44_hook, 0x6f44, 0x6657);
  CYC(0x6657, 0x665a); A = mem_rd(gb, wTextIsActive);
  CYC(0x665a, 0x665b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x665b, 0x665c); ret_effect(gb); return; }
  CYC(0x665b, 0x665c);
  CYC(0x665c, 0x665f); SET_HL(wTmpcbb3);
  CYC(0x665f, 0x6661); mem_wr(gb, HL, 0x3c);
  CYC(0x6661, 0x6664);
  cutscene_incCutsceneState_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_01__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6679, func_6ef7_hook, 0x6ef7, 0x667c);
  CALL_C(0x667c, func_6f44_hook, 0x6f44, 0x667f);
  CALL_C(0x667f, decCbb3_hook, 0x305d, 0x6682);
  if (!(F & FZ)) { CYCT(0x6682, 0x6683); ret_effect(gb); return; }
  CYC(0x6682, 0x6683);
  CALL_C(0x6683, cutscene_incCutsceneState_hook, 0x6f8c, 0x6686);
  CYC(0x6686, 0x6689);
  fadeoutToWhite_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_01__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66dc, updateStatusBar_hook, 0x1a9c, 0x66df);
  CYC(0x66df, 0x66e2); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x66e2, 0x66e3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x66e3, 0x66e4); ret_effect(gb); return; }
  CYC(0x66e3, 0x66e4);
  CYC(0x66e4, 0x66e6); A = 0x01;
  CYC(0x66e6, 0x66e9); mem_wr(gb, wMenuDisabled, A);
  CYC(0x66e9, 0x66ec); mem_wr(gb, wDisabledObjects, A);
  CYC(0x66ec, 0x66ef); mem_wr(gb, 0xc2ef, A);
  CYC(0x66ef, 0x66f0); ret_effect(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_02__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66fd, func_6ef7_hook, 0x6ef7, 0x6700);
  CALL_C(0x6700, func_6f44_hook, 0x6f44, 0x6703);
  CYC(0x6703, 0x6706); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6706, 0x6707); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6707, 0x6708); ret_effect(gb); return; }
  CYC(0x6707, 0x6708);
  CALL_C(0x6708, decCbb3_hook, 0x305d, 0x670b);
  if (!(F & FZ)) { CYCT(0x670b, 0x670c); ret_effect(gb); return; }
  CYC(0x670b, 0x670c);
  CYC(0x670c, 0x670e); A = 0x04;
  CYC(0x670e, 0x6711); mem_wr(gb, wTmpcbbb, A);
  CYC(0x6711, 0x6714); mem_wr(gb, wTmpcbb6, A);
  CYC(0x6714, 0x6717); A = mem_rd(gb, wTmpcbb8);
  CYC(0x6717, 0x671a); SET_HL(0x6722);
  CYC(0x671a, 0x671b); add_a_to_hl(gb, 0x671b);
  CYC(0x671b, 0x671c); A = mem_rd(gb, HL);
  CYC(0x671c, 0x671f); mem_wr(gb, wTmpcbb3, A);
  CYC(0x671f, 0x6722);
  cutscene_incCutsceneState_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_02__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6725, func_6ef7_hook, 0x6ef7, 0x6728);
  CALL_C(0x6728, func_6f26_hook, 0x6f26, 0x672b);
  if (!(F & FZ)) {
    CYCT(0x672b, 0x672e);
    func_6f44_hook(gb);
    return;
  }
  CYC(0x672b, 0x672e);
  CYC(0x672e, 0x6730); mem_wr(gb, HL, 0x78);
  CALL_C(0x6730, cutscene_incCutsceneState_hook, 0x6f8c, 0x6733);
  func_6733_hook(gb);
}

void func_6733_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6733, func_6ef7_hook, 0x6ef7, 0x6736);
  CALL_C(0x6736, func_6f44_hook, 0x6f44, 0x6739);
  CALL_C(0x6739, decCbb3_hook, 0x305d, 0x673c);
  if (!(F & FZ)) { CYCT(0x673c, 0x673d); ret_effect(gb); return; }
  CYC(0x673c, 0x673d);
  CYC(0x673d, 0x6740); A = mem_rd(gb, wTmpcbb8);
  CYC(0x6740, 0x6741); push_effect(gb, 0x6741);
  switch (misc_cutscene_jump_table(gb)) {
    case 0x6747: func_6733__cbb8_00_hook(gb); return;
    case 0x6768: func_6733__cbb8_02_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_6733__cbb8_00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6747, 0x674a); SET_HL(0x6772);
  CALL_C(0x674a, setWarpDestVariables_hook, 0x1997, 0x674d);
  CYC(0x674d, 0x6750); A = mem_rd(gb, 0xcfd3);
  CYC(0x6750, 0x6753); mem_wr(gb, wWarpDestPos, A);
  CYC(0x6753, 0x6756); A = mem_rd(gb, 0xcfd4);
  CYC(0x6756, 0x6759); mem_wr(gb, 0xcc50, A);
  CYC(0x6759, 0x675b); A = 0xff;
  CYC(0x675b, 0x675e); mem_wr(gb, wActiveMusic, A);
  CYC(0x675e, 0x6760); A = 0x01;
  CYC(0x6760, 0x6763); mem_wr(gb, 0xcfc0, A);
  CYC(0x6763, 0x6765); A = 0xfb;
  CYC(0x6765, 0x6768);
  playSound_b00_hook(gb);
}

void func_6733__cbb8_02_hook(GB *gb) {
  CYC(0x6768, 0x6769); alu_xor(gb, A);
  CYC(0x6769, 0x676c); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x676c, 0x676f); SET_HL(0x6777);
  CYC(0x676f, 0x6772);
  setWarpDestVariables_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x67fc, cutscene_decCBB3whenFadeDone_hook, 0x6f96, 0x67ff);
  if (!(F & FZ)) { CYCT(0x67ff, 0x6800); ret_effect(gb); return; }
  CYC(0x67ff, 0x6800);
  CYC(0x6800, 0x6802); mem_wr(gb, HL, 0x3c);
  CALL_C(0x6802, cutscene_incCutsceneState_hook, 0x6f8c, 0x6805);
  CYC(0x6805, 0x6807); A = 0x68;
  CYC(0x6807, 0x680a); SET_BC(0x5050);
  CYC(0x680a, 0x680d);
  createEnergySwirlGoingIn_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x680d, decCbb3_hook, 0x305d, 0x6810);
  if (!(F & FZ)) { CYCT(0x6810, 0x6811); ret_effect(gb); return; }
  CYC(0x6810, 0x6811);
  CYC(0x6811, 0x6812); alu_xor(gb, A);
  CYC(0x6812, 0x6813); mem_wr(gb, HL, A);
  CYC(0x6813, 0x6814); A = alu_dec8(gb, A);
  CYC(0x6814, 0x6817); mem_wr(gb, wTmpcbba, A);
  CYC(0x6817, 0x681a);
  cutscene_incCutsceneState_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x681a, 0x681d); SET_HL(wTmpcbb3);
  CYC(0x681d, 0x681f); B = 0x02;
  CALL_C(0x681f, flashScreen_hook, 0x2d73, 0x6822);
  if (F & FZ) { CYCT(0x6822, 0x6823); ret_effect(gb); return; }
  CYC(0x6822, 0x6823);
  CALL_C(0x6823, cutscene_incCutsceneState_hook, 0x6f8c, 0x6826);
  CYC(0x6826, 0x6829); SET_HL(wTmpcbb3);
  CYC(0x6829, 0x682b); mem_wr(gb, HL, 0x3c);
  CYC(0x682b, 0x682d); A = 0x01;
  CYC(0x682d, 0x6830); mem_wr(gb, 0xcfd0, A);
  CALL_C(0x6830, nayruWarpToMakuTreeCutsceneHandler__func_6838_hook, 0x6838, 0x6833);
  CYC(0x6833, 0x6835); A = 0x03;
  CYC(0x6835, 0x6838);
  fadeinFromWhiteWithDelay_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x684e, cutscene_decCBB3whenFadeDone_hook, 0x6f96, 0x6851);
  if (!(F & FZ)) { CYCT(0x6851, 0x6852); ret_effect(gb); return; }
  CYC(0x6851, 0x6852);
  CYC(0x6852, 0x6855);
  cutscene_incCutsceneState_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6855, 0x6858); A = mem_rd(gb, 0xcfd0);
  CYC(0x6858, 0x685a); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x685a, 0x685b); ret_effect(gb); return; }
  CYC(0x685a, 0x685b);
  CALL_C(0x685b, cutscene_incCutsceneState_hook, 0x6f8c, 0x685e);
  CYC(0x685e, 0x6860); A = 0xfa;
  CALL_C(0x6860, playSound_b00_hook, 0x0c98, 0x6863);
  CYC(0x6863, 0x6864); alu_xor(gb, A);
  CYC(0x6864, 0x6867); SET_HL(0xcfde);
  CYC(0x6867, 0x6869); mem_wr(gb, HL, 0x05);
  CYC(0x6869, 0x686a); L = alu_inc8(gb, L);
  CYC(0x686a, 0x686c); mem_wr(gb, HL, 0x00);
  CYC(0x686c, 0x686f);
  fadeoutToWhite_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x687f, 0x6882); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6882, 0x6883); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6883, 0x6884); ret_effect(gb); return; }
  CYC(0x6883, 0x6884);
  CYC(0x6884, 0x6887); SET_HL(0xcfdf);
  CYC(0x6887, 0x6888); A = mem_rd(gb, HL);
  CYC(0x6888, 0x688a); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x688a, 0x688b); ret_effect(gb); return; }
  CYC(0x688a, 0x688b);
  CYC(0x688b, 0x688c); alu_xor(gb, A);
  CYC(0x688c, 0x688d); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x688d, 0x688e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x688e, 0x688f); A = mem_rd(gb, HL);
  CYC(0x688f, 0x6891); alu_cp(gb, 0x07);
  CYC(0x6891, 0x6893); A = 0x06;
  if (!(F & FZ)) {
    CYCT(0x6893, 0x6895);
  } else {
    CYC(0x6893, 0x6895);
    CYC(0x6895, 0x6897); A = 0x08;
    CYC(0x6897, 0x689a); SET_HL(0xcfd0);
    CYC(0x689a, 0x689c); mem_wr(gb, HL, 0x03);
  }
  CYC(0x689c, 0x689f); SET_HL(wCutsceneState);
  CYC(0x689f, 0x68a0); mem_wr(gb, HL, A);
  CYC(0x68a0, 0x68a3);
  fadeoutToWhite_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68e8, 0x68eb); A = mem_rd(gb, 0xcfd0);
  CYC(0x68eb, 0x68ed); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x68ed, 0x68ee); ret_effect(gb); return; }
  CYC(0x68ed, 0x68ee);
  CALL_C(0x68ee, cutscene_incCutsceneState_hook, 0x6f8c, 0x68f1);
  CYC(0x68f1, 0x68f3); A = 0xf1;
  CALL_C(0x68f3, playSound_b00_hook, 0x0c98, 0x68f6);
  CYC(0x68f6, 0x68f9); SET_BC(0x2800);
  CALL_C(0x68f9, checkIsLinkedGame_hook, 0x1992, 0x68fc);
  if (F & FZ) {
    CYCT(0x68fc, 0x68fe);
  } else {
    CYC(0x68fc, 0x68fe);
    CYC(0x68fe, 0x6900); C = 0x02;
  }
  CYC(0x6900, 0x6903);
  showText_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6903, 0x6906); A = mem_rd(gb, wTextIsActive);
  CYC(0x6906, 0x6907); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6907, 0x6908); ret_effect(gb); return; }
  CYC(0x6907, 0x6908);
  CALL_C(0x6908, cutscene_incCutsceneState_hook, 0x6f8c, 0x690b);
  CYC(0x690b, 0x690e); SET_HL(wTmpcbb3);
  CYC(0x690e, 0x6910); mem_wr(gb, HL, 0xb4);
  CYC(0x6910, 0x6912); A = 0x01;
  CYC(0x6912, 0x6915); mem_wr(gb, 0xd008, A);
  CYC(0x6915, 0x6918); mem_wr(gb, wTmpcbb7, A);
  CYC(0x6918, 0x691a);
  nayruWarpToMakuTreeCutsceneHandler__func_6955_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x691a, decCbb3_hook, 0x305d, 0x691d);
  if (!(F & FZ)) {
    CYCT(0x691d, 0x691f);
    nayruWarpToMakuTreeCutsceneHandler__func_6948_hook(gb);
    return;
  }
  CYC(0x691d, 0x691f);
  CALL_C(0x691f, checkIsLinkedGame_hook, 0x1992, 0x6922);
  if (F & FZ) {
    CYCT(0x6922, 0x6924);
    nayruWarpToMakuTreeCutsceneHandler__func_692b_hook(gb);
    return;
  }
  CYC(0x6922, 0x6924);
  CYC(0x6924, 0x6926); A = 0x08;
  CYC(0x6926, 0x6929); mem_wr(gb, 0xcfd0, A);
  CYC(0x6929, 0x692b);
  cutscene_incCutsceneState_hook(gb);
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

void blackTowerCompleteCutsceneHandler__state0_hook(GB *gb) {
  CYC(0x69cf, 0x69d1); A = 0x3c;
  CYC(0x69d1, 0x69d4); mem_wr(gb, wTmpcbb3, A);
  CYC(0x69d4, 0x69d7);
  cutscene_incCutsceneState_hook(gb);
}

void blackTowerCompleteCutsceneHandler__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x69d7, decCbb3_hook, 0x305d, 0x69da);
  if (!(F & FZ)) { CYCT(0x69da, 0x69db); ret_effect(gb); return; }
  CYC(0x69da, 0x69db);
  CALL_C(0x69db, cutscene_incCutsceneState_hook, 0x6f8c, 0x69de);
  CYC(0x69de, 0x69e0); A = 0xfa;
  CALL_C(0x69e0, playSound_b00_hook, 0x0c98, 0x69e3);
  CYC(0x69e3, 0x69e6);
  fastFadeoutToBlack_hook(gb);
}

void blackTowerCompleteCutsceneHandler__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69e6, 0x69e9); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x69e9, 0x69ea); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x69ea, 0x69eb); ret_effect(gb); return; }
  CYC(0x69ea, 0x69eb);
  CALL_C(0x69eb, hideStatusBar_hook, 0x1aa0, 0x69ee);
  CYC(0x69ee, 0x69f0); A = hram_rd(gb, 0x70);
  CYC(0x69f0, 0x69f1); push_effect(gb, AF);
  CYC(0x69f1, 0x69f3); A = 0x02;
  CYC(0x69f3, 0x69f5); hram_wr(gb, 0x70, A);
  CYC(0x69f5, 0x69f8); SET_HL(0xde90);
  CYC(0x69f8, 0x69fa); B = 0x30;
  CALL_C(0x69fa, clearMemory_hook, 0x046f, 0x69fd);
  CYC(0x69fd, 0x69fe); SET_AF(pop_effect(gb));
  CYC(0x69fe, 0x6a00); hram_wr(gb, 0x70, A);
  CYC(0x6a00, 0x6a03); SET_HL(0x62b4);
  CYC(0x6a03, 0x6a05); E = 0x01;
  CALL_C(0x6a05, interBankCall_hook, 0x008a, 0x6a08);
  CYC(0x6a08, 0x6a09); alu_xor(gb, A);
  CYC(0x6a09, 0x6a0c); mem_wr(gb, wScrollMode, A);
  CYC(0x6a0c, 0x6a0f); mem_wr(gb, wTilesetFlags, A);
  CYC(0x6a0f, 0x6a12); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(0x6a12, 0x6a15); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(0x6a15, 0x6a18); mem_wr(gb, 0xd01a, A);
  CYC(0x6a18, 0x6a1a); A = 0x10;
  CYC(0x6a1a, 0x6a1d); mem_wr(gb, wScreenOffsetY, A);
  CALL_C(0x6a1d, checkIsLinkedGame_hook, 0x1992, 0x6a20);
  if (F & FZ) {
    CYCT(0x6a20, 0x6a22);
    blackTowerCompleteCutsceneHandler__func_6a2b_hook(gb);
    return;
  }
  CYC(0x6a20, 0x6a22);
  CALL_C(0x6a22, cutscene_incCutsceneState_hook, 0x6f8c, 0x6a25);
  CYC(0x6a25, 0x6a28); SET_HL(wTmpcbb3);
  CYC(0x6a28, 0x6a2a); mem_wr(gb, HL, 0x1e);
  CYC(0x6a2a, 0x6a2b); ret_effect(gb);
}

void blackTowerCompleteCutsceneHandler__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6a3c, decCbb3_hook, 0x305d, 0x6a3f);
  if (!(F & FZ)) { CYCT(0x6a3f, 0x6a40); ret_effect(gb); return; }
  CYC(0x6a3f, 0x6a40);
  CYC(0x6a40, 0x6a42); A = 0xd2;
  CALL_C(0x6a42, playSound_b00_hook, 0x0c98, 0x6a45);
  CYC(0x6a45, 0x6a46); alu_xor(gb, A);
  CYC(0x6a46, 0x6a49); SET_HL(wTmpcbb3);
  CYC(0x6a49, 0x6a4a); mem_wr(gb, HL, A);
  CYC(0x6a4a, 0x6a4b); A = alu_dec8(gb, A);
  CYC(0x6a4b, 0x6a4e); SET_HL(wTmpcbba);
  CYC(0x6a4e, 0x6a4f); mem_wr(gb, HL, A);
  CYC(0x6a4f, 0x6a52);
  cutscene_incCutsceneState_hook(gb);
}

void blackTowerCompleteCutsceneHandler__state5_hook(GB *gb) {
  CYC(0x6a8d, 0x6a8e); ret_effect(gb);
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

void turnToStoneCutsceneHandler__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6aaa, cutscene_incCutsceneState_hook, 0x6f8c, 0x6aad);
  CYC(0x6aad, 0x6aaf); B = 0x20;
  CYC(0x6aaf, 0x6ab2); SET_HL(wTmpcfc0_genericCutscene_state);
  CALL_C(0x6ab2, clearMemory_hook, 0x046f, 0x6ab5);
  CYC(0x6ab5, 0x6ab7); A = 0x0d;
  CYC(0x6ab7, 0x6aba); SET_HL(0xcfde);
  CYC(0x6aba, 0x6abb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6abb, 0x6abd); mem_wr(gb, HL, 0x00);
  CALL_C(0x6abd, showStatusBar_hook, 0x1aa4, 0x6ac0);
  CYC(0x6ac0, 0x6ac3);
  fadeoutToWhite_hook(gb);
}

void turnToStoneCutsceneHandler__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ad3, 0x6ad6); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6ad6, 0x6ad7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6ad7, 0x6ad8); ret_effect(gb); return; }
  CYC(0x6ad7, 0x6ad8);
  CYC(0x6ad8, 0x6adb); SET_HL(0xcfdf);
  CYC(0x6adb, 0x6adc); A = mem_rd(gb, HL);
  CYC(0x6adc, 0x6ade); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x6ade, 0x6adf); ret_effect(gb); return; }
  CYC(0x6ade, 0x6adf);
  CYC(0x6adf, 0x6ae0); alu_xor(gb, A);
  CYC(0x6ae0, 0x6ae1); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6ae1, 0x6ae2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ae2, 0x6ae3); A = mem_rd(gb, HL);
  CYC(0x6ae3, 0x6ae5); alu_cp(gb, 0x0f);
  CYC(0x6ae5, 0x6ae7); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x6ae7, 0x6ae9);
  } else {
    CYC(0x6ae7, 0x6ae9);
    CYC(0x6ae9, 0x6aeb); A = 0x03;
  }
  CYC(0x6aeb, 0x6aee); SET_HL(wCutsceneState);
  CYC(0x6aee, 0x6aef); mem_wr(gb, HL, A);
  CYC(0x6aef, 0x6af2);
  fadeoutToWhite_hook(gb);
}

void turnToStoneCutsceneHandler__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b16, 0x6b19); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x6b19, 0x6b1b); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6b1b, 0x6b1c); ret_effect(gb); return; }
  CYC(0x6b1b, 0x6b1c);
  CALL_C(0x6b1c, cutscene_incCutsceneState_hook, 0x6f8c, 0x6b1f);
  CYC(0x6b1f, 0x6b22);
  fadeoutToWhite_hook(gb);
}

void turnToStoneCutsceneHandler__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b66, 0x6b69); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6b69, 0x6b6a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6b6a, 0x6b6b); ret_effect(gb); return; }
  CYC(0x6b6a, 0x6b6b);
  CYC(0x6b6b, 0x6b6d); A = 0x01;
  CYC(0x6b6d, 0x6b70); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6b70, 0x6b73); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6b73, 0x6b76); mem_wr(gb, 0xc2ef, A);
  CYC(0x6b76, 0x6b77); ret_effect(gb);
}

void twinrovaRevealCutsceneHandler__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b99, cutscene_incCutsceneState_hook, 0x6f8c, 0x6b9c);
  CYC(0x6b9c, 0x6b9e); B = 0x20;
  CYC(0x6b9e, 0x6ba1); SET_HL(wTmpcfc0_genericCutscene_state);
  CALL_C(0x6ba1, clearMemory_hook, 0x046f, 0x6ba4);
  CYC(0x6ba4, 0x6ba6); A = 0xf0;
  CALL_C(0x6ba6, playSound_b00_hook, 0x0c98, 0x6ba9);
  CYC(0x6ba9, 0x6bac); SET_HL(wTmpcbb3);
  CYC(0x6bac, 0x6bae); mem_wr(gb, HL, 0x3c);
  CYC(0x6bae, 0x6bb1); SET_BC(0x2810);
  CALL_C(0x6bb1, checkIsLinkedGame_hook, 0x1992, 0x6bb4);
  if (F & FZ) {
    CYCT(0x6bb4, 0x6bb6);
  } else {
    CYC(0x6bb4, 0x6bb6);
    CYC(0x6bb6, 0x6bb8); C = 0x15;
  }
  CYC(0x6bb8, 0x6bba); A = 0x02;
  CYC(0x6bba, 0x6bbd); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x6bbd, 0x6bc0);
  showText_hook(gb);
}

void twinrovaRevealCutsceneHandler__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6bc0, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x6bc3);
  if (!(F & FZ)) { CYCT(0x6bc3, 0x6bc4); ret_effect(gb); return; }
  CYC(0x6bc3, 0x6bc4);
  CYC(0x6bc4, 0x6bc6); A = 0xf0;
  CALL_C(0x6bc6, playSound_b00_hook, 0x0c98, 0x6bc9);
  twinrovaRevealCutsceneHandler__func_6bc9_hook(gb);
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

void twinrovaRevealCutsceneHandler__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bd5, 0x6bd8); SET_HL(wTmpcbb3);
  CYC(0x6bd8, 0x6bda); B = 0x01;
  CALL_C(0x6bda, flashScreen_hook, 0x2d73, 0x6bdd);
  if (F & FZ) { CYCT(0x6bdd, 0x6bde); ret_effect(gb); return; }
  CYC(0x6bdd, 0x6bde);
  CALL_C(0x6bde, checkIsLinkedGame_hook, 0x1992, 0x6be1);
  if (!(F & FZ)) {
    CYCT(0x6be1, 0x6be3);
    twinrovaRevealCutsceneHandler__func_6be8_hook(gb);
    return;
  }
  CYC(0x6be1, 0x6be3);
  CALL_C(0x6be3, func_6fb0_hook, 0x6fb0, 0x6be6);
  CYC(0x6be6, 0x6be8);
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

void twinrovaRevealCutsceneHandler__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c09, 0x6c0c); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x6c0c, 0x6c0e); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6c0e, 0x6c0f); ret_effect(gb); return; }
  CYC(0x6c0e, 0x6c0f);
  CYC(0x6c0f, 0x6c11); A = 0xf0;
  CALL_C(0x6c11, playSound_b00_hook, 0x0c98, 0x6c14);
  CYC(0x6c14, 0x6c16); A = 0xd2;
  CALL_C(0x6c16, playSound_b00_hook, 0x0c98, 0x6c19);
  CYC(0x6c19, 0x6c1c);
  twinrovaRevealCutsceneHandler__func_6bc9_hook(gb);
}

void twinrovaRevealCutsceneHandler__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c40, 0x6c43); SET_HL(0xcfdf);
  CYC(0x6c43, 0x6c44); A = mem_rd(gb, HL);
  CYC(0x6c44, 0x6c46); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x6c46, 0x6c47); ret_effect(gb); return; }
  CYC(0x6c46, 0x6c47);
  CYC(0x6c47, 0x6c49); A = 0xd2;
  CALL_C(0x6c49, playSound_b00_hook, 0x0c98, 0x6c4c);
  CYC(0x6c4c, 0x6c4e); A = 0xf0;
  CALL_C(0x6c4e, playSound_b00_hook, 0x0c98, 0x6c51);
  CYC(0x6c51, 0x6c54);
  twinrovaRevealCutsceneHandler__func_6bc9_hook(gb);
}

void twinrovaRevealCutsceneHandler__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c9d, 0x6ca0); A = mem_rd(gb, 0xcfd0);
  CYC(0x6ca0, 0x6ca2); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(0x6ca2, 0x6ca3); ret_effect(gb); return; }
  CYC(0x6ca2, 0x6ca3);
  CYC(0x6ca3, 0x6ca5); A = 0xfa;
  CALL_C(0x6ca5, playSound_b00_hook, 0x0c98, 0x6ca8);
  CALL_C(0x6ca8, fadeoutToBlack_hook, 0x32c0, 0x6cab);
  CYC(0x6cab, 0x6cad); A = 0xff;
  CYC(0x6cad, 0x6cb0); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x6cb0, 0x6cb3); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x6cb3, 0x6cb5); A = 0x03;
  CYC(0x6cb5, 0x6cb8); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x6cb8, 0x6cbb); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x6cbb, 0x6cbe);
  cutscene_incCutsceneState_hook(gb);
}

void twinrovaRevealCutsceneHandler__state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6cbe, 0x6cc1); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6cc1, 0x6cc2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6cc2, 0x6cc3); ret_effect(gb); return; }
  CYC(0x6cc2, 0x6cc3);
  CALL_C(0x6cc3, cutscene_incCutsceneState_hook, 0x6f8c, 0x6cc6);
  CALL_C(0x6cc6, showStatusBar_hook, 0x1aa4, 0x6cc9);
  CYC(0x6cc9, 0x6ccb); A = 0x35;
  CALL_C(0x6ccb, setGlobalFlag_hook, 0x31f9, 0x6cce);
  CYC(0x6cce, 0x6cd0); A = 0x01;
  CYC(0x6cd0, 0x6cd3); mem_wr(gb, wScrollMode, A);
  CYC(0x6cd3, 0x6cd4); alu_xor(gb, A);
  CYC(0x6cd4, 0x6cd7); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(0x6cd7, 0x6cda); mem_wr(gb, wScreenShakeCounterX, A);
  CYC(0x6cda, 0x6cdc); A = 0x0f;
  CYC(0x6cdc, 0x6cdf); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(0x6cdf, 0x6ce1); A = 0xf0;
  CYC(0x6ce1, 0x6ce4); mem_wr(gb, wGfxRegs2_SCY, A);
  CALL_C(0x6ce4, fadeinFromBlack_hook, 0x32e6, 0x6ce7);
  CYC(0x6ce7, 0x6cea); SET_BC(0x8706);
  CALL_C(0x6cea, createInteraction_hook, 0x540c, 0x6ced);
  CYC(0x6ced, 0x6cf0); SET_BC(0x4050);
  CALL_C(0x6cf0, interactionHSetPosition_hook, 0x2774, 0x6cf3);
  CYC(0x6cf3, 0x6cf6); A = mem_rd(gb, wActiveMusic2);
  CYC(0x6cf6, 0x6cf9); mem_wr(gb, wActiveMusic, A);
  CYC(0x6cf9, 0x6cfc);
  playSound_b00_hook(gb);
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

void pregameIntroCutsceneHandler_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d0b, pregameIntroCutsceneHandler__runStates_hook, 0x6d11, 0x6d0e);
  CYC(0x6d0e, 0x6d11);
  updateAllObjects_hook(gb);
}

void pregameIntroCutsceneHandler__runStates_hook(GB *gb) {
  CYC(0x6d11, 0x6d14); SET_DE(wCutsceneState);
  CYC(0x6d14, 0x6d15); A = mem_rd(gb, DE);
  CYC(0x6d15, 0x6d16); push_effect(gb, 0x6d16);
  switch (misc_cutscene_jump_table(gb)) {
    case 0x6d30: pregameIntroCutsceneHandler__state0_hook(gb); return;
    case 0x6d76: pregameIntroCutsceneHandler__state1_hook(gb); return;
    case 0x6d84: pregameIntroCutsceneHandler__state2_hook(gb); return;
    case 0x6d88: pregameIntroCutsceneHandler__state3_hook(gb); return;
    case 0x6da7: pregameIntroCutsceneHandler__state4_hook(gb); return;
    case 0x6dc8: pregameIntroCutsceneHandler__state5_hook(gb); return;
    case 0x6dd8: pregameIntroCutsceneHandler__state6_hook(gb); return;
    case 0x6def: pregameIntroCutsceneHandler__state7_hook(gb); return;
    case 0x6e04: pregameIntroCutsceneHandler__state8_hook(gb); return;
    case 0x6e1a: pregameIntroCutsceneHandler__state9_hook(gb); return;
    case 0x6e26: pregameIntroCutsceneHandler__stateA_hook(gb); return;
    case 0x6e75: pregameIntroCutsceneHandler__stateB_hook(gb); return;
    case 0x6e86: pregameIntroCutsceneHandler__stateC_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void pregameIntroCutsceneHandler__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d30, 0x6d33); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6d33, 0x6d34); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d34, 0x6d35); ret_effect(gb); return; }
  CYC(0x6d34, 0x6d35);
  CALL_C(0x6d35, checkIsLinkedGame_hook, 0x1992, 0x6d38);
  if (!(F & FZ)) {
    CYCT(0x6d38, 0x6d3a);
    pregameIntroCutsceneHandler__func_6d40_hook(gb);
    return;
  }
  CYC(0x6d38, 0x6d3a);
  CYC(0x6d3a, 0x6d3c); A = 0x0a;
  CYC(0x6d3c, 0x6d3d); mem_wr(gb, DE, A);
  CYC(0x6d3d, 0x6d40);
  pregameIntroCutsceneHandler__stateA_hook(gb);
}

void pregameIntroCutsceneHandler__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d76, 0x6d78); E = 0x96;
  CALL_C(0x6d78, decCbb3_hook, 0x305d, 0x6d7b);
  if (!(F & FZ)) { CYCT(0x6d7b, 0x6d7c); ret_effect(gb); return; }
  CYC(0x6d7b, 0x6d7c);
  CALL_C(0x6d7c, cutscene_incCutsceneState_hook, 0x6f8c, 0x6d7f);
  CYC(0x6d7f, 0x6d82); SET_HL(wTmpcbb3);
  CYC(0x6d82, 0x6d83); mem_wr(gb, HL, E);
  CYC(0x6d83, 0x6d84); ret_effect(gb);
}

void pregameIntroCutsceneHandler__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d84, 0x6d86); E = 0x3c;
  CYC(0x6d86, 0x6d88);
  CALL_C(0x6d78, decCbb3_hook, 0x305d, 0x6d7b);
  if (!(F & FZ)) { CYCT(0x6d7b, 0x6d7c); ret_effect(gb); return; }
  CYC(0x6d7b, 0x6d7c);
  CALL_C(0x6d7c, cutscene_incCutsceneState_hook, 0x6f8c, 0x6d7f);
  CYC(0x6d7f, 0x6d82); SET_HL(wTmpcbb3);
  CYC(0x6d82, 0x6d83); mem_wr(gb, HL, E);
  CYC(0x6d83, 0x6d84); ret_effect(gb);
}

void pregameIntroCutsceneHandler__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d88, decCbb3_hook, 0x305d, 0x6d8b);
  if (!(F & FZ)) { CYCT(0x6d8b, 0x6d8c); ret_effect(gb); return; }
  CYC(0x6d8b, 0x6d8c);
  CALL_C(0x6d8c, cutscene_incCutsceneState_hook, 0x6f8c, 0x6d8f);
  CALL_C(0x6d8f, fastFadeinFromBlack_hook, 0x32dd, 0x6d92);
  CYC(0x6d92, 0x6d94); A = 0x40;
  CYC(0x6d94, 0x6d97); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x6d97, 0x6d9a); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x6d9a, 0x6d9c); A = 0x03;
  CYC(0x6d9c, 0x6d9f); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x6d9f, 0x6da2); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x6da2, 0x6da4); A = 0x72;
  CYC(0x6da4, 0x6da7);
  playSound_b00_hook(gb);
}

void pregameIntroCutsceneHandler__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6da7, 0x6daa); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6daa, 0x6dab); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6dab, 0x6dac); ret_effect(gb); return; }
  CYC(0x6dab, 0x6dac);
  CALL_C(0x6dac, cutscene_incCutsceneState_hook, 0x6f8c, 0x6daf);
  CYC(0x6daf, 0x6db1); A = 0x0e;
  CYC(0x6db1, 0x6db4); mem_wr(gb, wTmpcbb3, A);
  CALL_C(0x6db4, fadeinFromBlack_hook, 0x32e6, 0x6db7);
  CYC(0x6db7, 0x6db9); A = 0xbf;
  CYC(0x6db9, 0x6dbc); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x6dbc, 0x6dbf); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x6dbf, 0x6dc1); A = 0xfc;
  CYC(0x6dc1, 0x6dc4); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x6dc4, 0x6dc7); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x6dc7, 0x6dc8); ret_effect(gb);
}

void pregameIntroCutsceneHandler__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6dc8, decCbb3_hook, 0x305d, 0x6dcb);
  if (!(F & FZ)) { CYCT(0x6dcb, 0x6dcc); ret_effect(gb); return; }
  CYC(0x6dcb, 0x6dcc);
  CYC(0x6dcc, 0x6dcd); alu_xor(gb, A);
  CYC(0x6dcd, 0x6dd0); mem_wr(gb, wPaletteThread_mode, A);
  CYC(0x6dd0, 0x6dd2); A = 0x78;
  CYC(0x6dd2, 0x6dd5); mem_wr(gb, wTmpcbb3, A);
  CYC(0x6dd5, 0x6dd8);
  cutscene_incCutsceneState_hook(gb);
}

void pregameIntroCutsceneHandler__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6dd8, decCbb3_hook, 0x305d, 0x6ddb);
  if (!(F & FZ)) { CYCT(0x6ddb, 0x6ddc); ret_effect(gb); return; }
  CYC(0x6ddb, 0x6ddc);
  CALL_C(0x6ddc, cutscene_incCutsceneState_hook, 0x6f8c, 0x6ddf);
  CYC(0x6ddf, 0x6de1); A = 0x08;
  CYC(0x6de1, 0x6de4); mem_wr(gb, wTextboxFlags, A);
  CYC(0x6de4, 0x6de6); A = 0x03;
  CYC(0x6de6, 0x6de9); mem_wr(gb, wTextboxPosition, A);
  CYC(0x6de9, 0x6dec); SET_BC(0x281a);
  CYC(0x6dec, 0x6def);
  showText_hook(gb);
}

void pregameIntroCutsceneHandler__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6def, retIfTextIsActive_hook, 0x1859, 0x6df2);
  CALL_C(0x6df2, cutscene_incCutsceneState_hook, 0x6f8c, 0x6df5);
  CYC(0x6df5, 0x6df8); mem_wr(gb, wTmpcbb3, A);
  CYC(0x6df8, 0x6df9); A = alu_dec8(gb, A);
  CYC(0x6df9, 0x6dfc); mem_wr(gb, wTmpcbba, A);
  CALL_C(0x6dfc, restartSound_hook, 0x0cb2, 0x6dff);
  CYC(0x6dff, 0x6e01); A = 0xbc;
  CYC(0x6e01, 0x6e04);
  playSound_b00_hook(gb);
}

void pregameIntroCutsceneHandler__state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e04, 0x6e07); SET_HL(wTmpcbb3);
  CYC(0x6e07, 0x6e09); B = 0x03;
  CALL_C(0x6e09, flashScreen_hook, 0x2d73, 0x6e0c);
  if (F & FZ) { CYCT(0x6e0c, 0x6e0d); ret_effect(gb); return; }
  CYC(0x6e0c, 0x6e0d);
  CALL_C(0x6e0d, cutscene_incCutsceneState_hook, 0x6f8c, 0x6e10);
  CYC(0x6e10, 0x6e12); A = 0x3c;
  CYC(0x6e12, 0x6e15); mem_wr(gb, wTmpcbb3, A);
  CYC(0x6e15, 0x6e17); A = 0x02;
  CYC(0x6e17, 0x6e1a);
  fadeoutToWhiteWithDelay_hook(gb);
}

void pregameIntroCutsceneHandler__state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e1a, 0x6e1d); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6e1d, 0x6e1e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6e1e, 0x6e1f); ret_effect(gb); return; }
  CYC(0x6e1e, 0x6e1f);
  CALL_C(0x6e1f, decCbb3_hook, 0x305d, 0x6e22);
  if (!(F & FZ)) { CYCT(0x6e22, 0x6e23); ret_effect(gb); return; }
  CYC(0x6e22, 0x6e23);
  CYC(0x6e23, 0x6e26);
  cutscene_incCutsceneState_hook(gb);
}

void pregameIntroCutsceneHandler__stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e26, disableLcd_hook, 0x02c1, 0x6e29);
  CYC(0x6e29, 0x6e2b); A = hram_rd(gb, 0x70);
  CYC(0x6e2b, 0x6e2c); push_effect(gb, AF);
  CYC(0x6e2c, 0x6e2e); A = 0x02;
  CYC(0x6e2e, 0x6e30); hram_wr(gb, 0x70, A);
  CYC(0x6e30, 0x6e33); SET_HL(0xde80);
  CYC(0x6e33, 0x6e35); B = 0x40;
  CALL_C(0x6e35, clearMemory_hook, 0x046f, 0x6e38);
  CYC(0x6e38, 0x6e39); SET_AF(pop_effect(gb));
  CYC(0x6e39, 0x6e3b); hram_wr(gb, 0x70, A);
  CALL_C(0x6e3b, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x6e3e);
  CALL_C(0x6e3e, clearOam_hook, 0x049f, 0x6e41);
  CYC(0x6e41, 0x6e43); A = 0x0f;
  CALL_C(0x6e43, loadPaletteHeader_hook, 0x050b, 0x6e46);
  CYC(0x6e46, 0x6e48); A = 0x02;
  CALL_C(0x6e48, func_6e9a_hook, 0x6e9a, 0x6e4b);
  CALL_C(0x6e4b, func_6eb7_hook, 0x6eb7, 0x6e4e);
  CYC(0x6e4e, 0x6e50); A = 0x0d;
  CALL_C(0x6e50, playSound_b00_hook, 0x0c98, 0x6e53);
  CYC(0x6e53, 0x6e55); A = 0x08;
  CALL_C(0x6e55, setLinkID_hook, 0x2ad9, 0x6e58);
  CYC(0x6e58, 0x6e5a); L = 0x00;
  CYC(0x6e5a, 0x6e5c); mem_wr(gb, HL, 0x01);
  CYC(0x6e5c, 0x6e5e); L = 0x02;
  CYC(0x6e5e, 0x6e60); mem_wr(gb, HL, 0x0b);
  CYC(0x6e60, 0x6e62); A = 0x00;
  CYC(0x6e62, 0x6e65); mem_wr(gb, wScrollMode, A);
  CALL_C(0x6e65, cutscene_incCutsceneState_hook, 0x6f8c, 0x6e68);
  CALL_C(0x6e68, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x6e6b);
  CYC(0x6e6b, 0x6e6c); alu_xor(gb, A);
  CYC(0x6e6c, 0x6e6e); hram_wr(gb, 0xaa, A);
  CYC(0x6e6e, 0x6e70); hram_wr(gb, 0xac, A);
  CYC(0x6e70, 0x6e72); A = 0x15;
  CYC(0x6e72, 0x6e75);
  loadGfxRegisterStateIndex_hook(gb);
}

void pregameIntroCutsceneHandler__stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e75, 0x6e78); A = mem_rd(gb, wTmpcbb9);
  CYC(0x6e78, 0x6e7a); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6e7a, 0x6e7b); ret_effect(gb); return; }
  CYC(0x6e7a, 0x6e7b);
  CALL_C(0x6e7b, clearLinkObject_hook, 0x35ba, 0x6e7e);
  CYC(0x6e7e, 0x6e81); SET_HL(wTmpcbb3);
  CYC(0x6e81, 0x6e83); mem_wr(gb, HL, 0x3c);
  CYC(0x6e83, 0x6e86);
  cutscene_incCutsceneState_hook(gb);
}

void pregameIntroCutsceneHandler__stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e86, decCbb3_hook, 0x305d, 0x6e89);
  if (!(F & FZ)) { CYCT(0x6e89, 0x6e8a); ret_effect(gb); return; }
  CYC(0x6e89, 0x6e8a);
  CYC(0x6e8a, 0x6e8d); SET_HL(0xc2ee);
  CYC(0x6e8d, 0x6e8e); alu_xor(gb, A);
  CYC(0x6e8e, 0x6e8f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6e8f, 0x6e90); mem_wr(gb, HL, A);
  CYC(0x6e90, 0x6e92); A = 0xf0;
  CALL_C(0x6e92, playSound_b00_hook, 0x0c98, 0x6e95);
  CYC(0x6e95, 0x6e97); A = 0x3d;
  CYC(0x6e97, 0x6e9a);
  setGlobalFlag_hook(gb);
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

void func_03_7493__cbb3_00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x74e6, 0x74e9); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x74e9, 0x74ea); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x74ea, 0x74eb); ret_effect(gb);
    return;
  }
  CYC(0x74ea, 0x74eb);
  CALL_C(0x74eb, ambiPassageOpen_decCBB4_hook, 0x7483, 0x74ee);
  if (!(F & FZ)) {
    CYCT(0x74ee, 0x74ef); ret_effect(gb);
    return;
  }
  CYC(0x74ee, 0x74ef);
  CYC(0x74ef, 0x74f1); mem_wr(gb, HL, 0x3e);
  CYC(0x74f1, 0x74f4); A = mem_rd(gb, wTmpcbbd);
  CYC(0x74f4, 0x74f7); SET_HL(0x7513);
  CYC(0x74f7, 0x74f8); add_double_index_to_hl(gb, 0x74f8);
  CYC(0x74f8, 0x74f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74f9, 0x74fa); B = mem_rd(gb, HL);
  CYC(0x74fa, 0x74fb); C = A;
  CALL_C(0x74fb, getFreeInteractionSlot_hook, 0x3aef, 0x74fe);
  if (!(F & FZ)) {
    CYCT(0x74fe, 0x74ff); ret_effect(gb);
    return;
  }
  CYC(0x74fe, 0x74ff);
  CYC(0x74ff, 0x7501); mem_wr(gb, HL, 0x14);
  CYC(0x7501, 0x7503); L = 0x49;
  CYC(0x7503, 0x7504); mem_wr(gb, HL, B);
  CYC(0x7504, 0x7506); L = 0x4b;
  CALL_C(0x7506, setShortPosition_paramC_hook, 0x20b9, 0x7509);
  CYC(0x7509, 0x750b); L = 0x4b;
  CYC(0x750b, 0x750c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x750c, 0x750d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x750d, 0x750f); L = 0x70;
  CYC(0x750f, 0x7510); mem_wr(gb, HL, C);
  CYC(0x7510, 0x7513); ambiPassageOpen_incCBB3_hook(gb);
}

void func_03_7493__cbb3_01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x751b, ambiPassageOpen_decCBB4_hook, 0x7483, 0x751e);
  if (!(F & FZ)) {
    CYCT(0x751e, 0x751f); ret_effect(gb);
    return;
  }
  CYC(0x751e, 0x751f);
  CYC(0x751f, 0x7521); mem_wr(gb, HL, 0x1e);
  CYC(0x7521, 0x7523); A = 0x4d;
  CALL_C(0x7523, playSound_b00_hook, 0x0c98, 0x7526);
  CYC(0x7526, 0x7529); ambiPassageOpen_incState_hook(gb);
}

void func_03_7493__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7529, ambiPassageOpen_decCBB4_hook, 0x7483, 0x752c);
  if (!(F & FZ)) {
    CYCT(0x752c, 0x752d); ret_effect(gb);
    return;
  }
  CYC(0x752c, 0x752d);
  CALL_C(0x752d, getThisRoomFlags_hook, 0x197d, 0x7530);
  CYC(0x7530, 0x7533); A = mem_rd(gb, wTmpcbbb);
  CYC(0x7533, 0x7536); mem_wr(gb, wWarpDestRoom, A);
  CYC(0x7536, 0x7537); L = A;
  CYC(0x7537, 0x7539); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x7539, 0x753b); A = 0x81;
  CYC(0x753b, 0x753e); mem_wr(gb, wWarpDestGroup, A);
  CYC(0x753e, 0x7541); A = mem_rd(gb, wTmpcbbc);
  CYC(0x7541, 0x7544); mem_wr(gb, wWarpDestPos, A);
  CYC(0x7544, 0x7546); A = 0x00;
  CYC(0x7546, 0x7549); mem_wr(gb, wWarpTransition, A);
  CYC(0x7549, 0x754b); A = 0x03;
  CYC(0x754b, 0x754e); mem_wr(gb, 0xc2ef, A);
  CYC(0x754e, 0x754f); alu_xor(gb, A);
  CYC(0x754f, 0x7552); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7552, 0x7555); fadeoutToWhite_hook(gb);
}

void jabuOpen_loadGfxAndPlaySound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75cc, loadGfxHeader_hook, 0x0626, 0x75cf);
  CALL_C(0x75cf, reloadTileMap_hook, 0x12fc, 0x75d2);
  CYC(0x75d2, 0x75d4); A = 0x70;
  CYC(0x75d4, 0x75d7); playSound_b00_hook(gb);
}

void func_03_7565__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x756f, 0x7571); B = 0x10;
  CYC(0x7571, 0x7574); SET_HL(wTmpcbb3);
  CALL_C(0x7574, clearMemory_hook, 0x046f, 0x7577);
  CYC(0x7577, 0x757a); SET_HL(0x62b4);
  CYC(0x757a, 0x757c); E = 0x01;
  CALL_C(0x757c, interBankCall_hook, 0x008a, 0x757f);
  CALL_C(0x757f, getThisRoomFlags_hook, 0x197d, 0x7582);
  CYC(0x7582, 0x7584); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CYC(0x7584, 0x7586); A = 0x04;
  CYC(0x7586, 0x7589); mem_wr(gb, wTmpcbb4, A);
  CYC(0x7589, 0x758a); alu_xor(gb, A);
  CYC(0x758a, 0x758d); mem_wr(gb, wScrollMode, A);
  CYC(0x758d, 0x758f); jabuOpen_incState_hook(gb);
}

void func_03_7565__cbb3_00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x759b, jabuOpen_decCBB4_hook, 0x7555, 0x759e);
  if (!(F & FZ)) {
    CYCT(0x759e, 0x759f); ret_effect(gb);
    return;
  }
  CYC(0x759e, 0x759f);
  CYC(0x759f, 0x75a1); mem_wr(gb, HL, 0x3c);
  CALL_C(0x75a1, reloadTileMap_hook, 0x12fc, 0x75a4);
  CYC(0x75a4, 0x75a7); SET_HL(0x626e);
  CYC(0x75a7, 0x75a9); E = 0x01;
  CALL_C(0x75a9, interBankCall_hook, 0x008a, 0x75ac);
  CYC(0x75ac, 0x75ae); jabuOpen_incCBB3_hook(gb);
}

void func_03_7565__cbb3_01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75ae, jabuOpen_decCBB4_hook, 0x7555, 0x75b1);
  if (!(F & FZ)) {
    CYCT(0x75b1, 0x75b2); ret_effect(gb);
    return;
  }
  CYC(0x75b1, 0x75b2);
  CYC(0x75b2, 0x75b4); mem_wr(gb, HL, 0x3c);
  CYC(0x75b4, 0x75b6); jabuOpen_incCBB3_hook(gb);
}

void func_03_7565__cbb3_02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x75b6, 0x75b8); A = 0x3c;
  CALL_C(0x75b8, setScreenShakeCounter_hook, 0x24bb, 0x75bb);
  CALL_C(0x75bb, jabuOpen_decCBB4_hook, 0x7555, 0x75be);
  if (!(F & FZ)) {
    CYCT(0x75be, 0x75bf); ret_effect(gb);
    return;
  }
  CYC(0x75be, 0x75bf);
  CYC(0x75bf, 0x75c1); mem_wr(gb, HL, 0x3c);
  CALL_C(0x75c1, jabuOpen_incCBB3_hook, 0x7560, 0x75c4);
  CYC(0x75c4, 0x75c7); SET_BC(0x9701);
  CALL_C(0x75c7, objectCreateInteraction_hook, 0x24c5, 0x75ca);
  CYC(0x75ca, 0x75cc); A = 0x74;
  jabuOpen_loadGfxAndPlaySound_hook(gb);
}

void func_03_7565__cbb3_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x75d7, 0x75d9); A = 0x3c;
  CALL_C(0x75d9, setScreenShakeCounter_hook, 0x24bb, 0x75dc);
  CALL_C(0x75dc, jabuOpen_decCBB4_hook, 0x7555, 0x75df);
  if (!(F & FZ)) {
    CYCT(0x75df, 0x75e0); ret_effect(gb);
    return;
  }
  CYC(0x75df, 0x75e0);
  CYC(0x75e0, 0x75e2); mem_wr(gb, HL, 0x3c);
  CALL_C(0x75e2, jabuOpen_incState_hook, 0x755b, 0x75e5);
  CYC(0x75e5, 0x75e7); A = 0x75;
  CYC(0x75e7, 0x75e9); jabuOpen_loadGfxAndPlaySound_hook(gb);
}

void func_03_7565__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75e9, jabuOpen_decCBB4_hook, 0x7555, 0x75ec);
  if (!(F & FZ)) {
    CYCT(0x75ec, 0x75ed); ret_effect(gb);
    return;
  }
  CYC(0x75ec, 0x75ed);
  CYC(0x75ed, 0x75ef); A = 0x4d;
  CALL_C(0x75ef, playSound_b00_hook, 0x0c98, 0x75f2);
  CYC(0x75f2, 0x75f4); A = 0x01;
  CYC(0x75f4, 0x75f7); mem_wr(gb, 0xc2ef, A);
  CYC(0x75f7, 0x75f9); A = 0x01;
  CYC(0x75f9, 0x75fc); mem_wr(gb, wScrollMode, A);
  CYC(0x75fc, 0x75fd); alu_xor(gb, A);
  CYC(0x75fd, 0x7600); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7600, 0x7603); mem_wr(gb, wMenuDisabled, A);
  CALL_C(0x7603, loadTilesetAndRoomLayout_hook, 0x38a5, 0x7606);
  CYC(0x7606, 0x7609); loadRoomCollisions_hook(gb);
}

void func_03_7619__func_764a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x764a, 0x764b); push_effect(gb, BC);
  CALL_C(0x764b, disableLcd_hook, 0x02c1, 0x764e);
  CYC(0x764e, 0x7650); A = 0x0f;
  CALL_C(0x7650, loadPaletteHeader_hook, 0x050b, 0x7653);
  CALL_C(0x7653, clearOam_hook, 0x049f, 0x7656);
  CALL_C(0x7656, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x7659);
  CYC(0x7659, 0x765c); SET_HL(0x49af);
  CYC(0x765c, 0x765e); E = 0x01;
  CALL_C(0x765e, interBankCall_hook, 0x008a, 0x7661);
  CALL_C(0x7661, stopTextThread_hook, 0x184d, 0x7664);
  CYC(0x7664, 0x7666); A = 0x01;
  CYC(0x7666, 0x7669); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7669, 0x766c); mem_wr(gb, wMenuDisabled, A);
  CYC(0x766c, 0x766d); alu_xor(gb, A);
  CYC(0x766d, 0x766e); SET_BC(pop_effect(gb));
  CALL_C(0x766e, forceLoadRoom_hook, 0x36f6, 0x7671);
  CALL_C(0x7671, func_131f_hook, 0x131f, 0x7674);
  CALL_C(0x7674, loadCommonGraphics_hook, 0x1a98, 0x7677);
  CYC(0x7677, 0x7679); A = 0x02;
  CYC(0x7679, 0x767c); loadGfxRegisterStateIndex_hook(gb);
}

void func_03_7619__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x762b, 0x762e); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x762e, 0x762f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x762f, 0x7630); ret_effect(gb);
    return;
  }
  CYC(0x762f, 0x7630);
  CYC(0x7630, 0x7632); B = 0x10;
  CYC(0x7632, 0x7635); SET_HL(wTmpcbb3);
  CALL_C(0x7635, clearMemory_hook, 0x046f, 0x7638);
  CALL_C(0x7638, clearScreenVariablesAndWramBank1_hook, 0x35a3, 0x763b);
  CALL_C(0x763b, refreshObjectGfx_hook, 0x1618, 0x763e);
  CYC(0x763e, 0x7640); A = 0x0f;
  CALL_C(0x7640, playSound_b00_hook, 0x0c98, 0x7643);
  CALL_C(0x7643, cleanSeas_incState_hook, 0x760f, 0x7646);
  CYC(0x7646, 0x7647); alu_xor(gb, A);
  CYC(0x7647, 0x764a); SET_BC(0x01a5);
  func_03_7619__func_764a_hook(gb);
}

void func_03_7619__state1__cbb3_00_hook(GB *gb) {
  CYC(0x7688, 0x768b); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x768b, 0x768c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x768c, 0x768d); ret_effect(gb);
    return;
  }
  CYC(0x768c, 0x768d);
  CYC(0x768d, 0x768f); A = 0xf0;
  CYC(0x768f, 0x7692); mem_wr(gb, wTmpcbb4, A);
  CYC(0x7692, 0x7695); cleanSeas_incCBB3_hook(gb);
}

void func_03_7619__state1__cbb3_01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7695, 0x7698); A = mem_rd(gb, wFrameCounter);
  CYC(0x7698, 0x769a); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x769a, 0x769c);
    goto after_sparkle;
  }
  CYC(0x769a, 0x769c);
  CALL_C(0x769c, getFreePartSlot_hook, 0x3e8e, 0x769f);
  if (!(F & FZ)) {
    CYCT(0x769f, 0x76a1);
    goto after_sparkle;
  }
  CYC(0x769f, 0x76a1);
  CYC(0x76a1, 0x76a3); mem_wr(gb, HL, 0x26);
  CALL_C(0x76a3, getRandomNumber_hook, 0x043e, 0x76a6);
  CYC(0x76a6, 0x76a8); alu_and(gb, 0x7f);
  CYC(0x76a8, 0x76a9); C = A;
  CYC(0x76a9, 0x76ab); L = 0xcb;
  CALL_C(0x76ab, setShortPosition_paramC_hook, 0x20b9, 0x76ae);
after_sparkle:
  CYC(0x76ae, 0x76b1); A = mem_rd(gb, wFrameCounter);
  CYC(0x76b1, 0x76b3); alu_and(gb, 0x1f);
  CYC(0x76b3, 0x76b5); A = 0x83;
  if (F & FZ) CALL_C_CC(0x76b5, playSound_b00_hook, 0x0c98, 0x76b8);
  else CYC(0x76b5, 0x76b8);
  CALL_C(0x76b8, cleanSeas_decCBB4_hook, 0x7609, 0x76bb);
  if (!(F & FZ)) {
    CYCT(0x76bb, 0x76bc); ret_effect(gb);
    return;
  }
  CYC(0x76bb, 0x76bc);
  CYC(0x76bc, 0x76be); mem_wr(gb, HL, 0x78);
  CYC(0x76be, 0x76c0); A = 0x04;
  CALL_C(0x76c0, fadeoutToWhiteWithDelay_hook, 0x3257, 0x76c3);
  CYC(0x76c3, 0x76c5); A = 0xb4;
  CALL_C(0x76c5, playSound_b00_hook, 0x0c98, 0x76c8);
  CYC(0x76c8, 0x76cb); cleanSeas_incCBB3_hook(gb);
}

void func_03_7619__state1__cbb3_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x76ea, 0x76ed); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x76ed, 0x76ee); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x76ee, 0x76ef); ret_effect(gb);
    return;
  }
  CYC(0x76ee, 0x76ef);
  CALL_C(0x76ef, cleanSeas_decCBB4_hook, 0x7609, 0x76f2);
  if (!(F & FZ)) {
    CYCT(0x76f2, 0x76f3); ret_effect(gb);
    return;
  }
  CYC(0x76f2, 0x76f3);
  CYC(0x76f3, 0x76f5); mem_wr(gb, HL, 0x3c);
  CALL_C(0x76f5, cleanSeas_incState_hook, 0x760f, 0x76f8);
  CYC(0x76f8, 0x76f9); alu_xor(gb, A);
  CYC(0x76f9, 0x76fc); mem_wr(gb, wTmpcbb3, A);
  CYC(0x76fc, 0x76ff); SET_BC(0x01d2);
  CYC(0x76ff, 0x7702); func_03_7619__func_764a_hook(gb);
}

void func_03_7619__state2__cbb3_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x770e, 0x7711); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7711, 0x7712); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7712, 0x7713); ret_effect(gb);
    return;
  }
  CYC(0x7712, 0x7713);
  CALL_C(0x7713, cleanSeas_decCBB4_hook, 0x7609, 0x7716);
  if (!(F & FZ)) {
    CYCT(0x7716, 0x7717); ret_effect(gb);
    return;
  }
  CYC(0x7716, 0x7717);
  CYC(0x7717, 0x7719); mem_wr(gb, HL, 0x3c);
  CALL_C(0x7719, cleanSeas_incState_hook, 0x760f, 0x771c);
  CYC(0x771c, 0x771d); alu_xor(gb, A);
  CYC(0x771d, 0x7720); mem_wr(gb, wTmpcbb3, A);
  CYC(0x7720, 0x7723); SET_BC(0x03b1);
  CALL_C(0x7723, func_03_7619__func_764a_hook, 0x764a, 0x7726);
  CYC(0x7726, 0x7729); SET_HL(0x7e71);
  CYC(0x7729, 0x772c); parseGivenObjectData_b00_hook(gb);
}

void func_03_7619__state3__cbb3_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7738, 0x773b); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x773b, 0x773c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x773c, 0x773d); ret_effect(gb);
    return;
  }
  CYC(0x773c, 0x773d);
  CYC(0x773d, 0x7740); SET_HL(0xcfc0);
  CYC(0x7740, 0x7742); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7742, 0x7743); ret_effect(gb);
    return;
  }
  CYC(0x7742, 0x7743);
  CYC(0x7743, 0x7745); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CALL_C(0x7745, cleanSeas_incState_hook, 0x760f, 0x7748);
  CYC(0x7748, 0x7749); alu_xor(gb, A);
  CYC(0x7749, 0x774c); mem_wr(gb, wTmpcbb3, A);
  CYC(0x774c, 0x774e); A = 0x3c;
  CYC(0x774e, 0x7751); mem_wr(gb, wTmpcbb4, A);
  CYC(0x7751, 0x7754); SET_BC(0x03b0);
  CALL_C(0x7754, func_03_7619__func_764a_hook, 0x764a, 0x7757);
  CYC(0x7757, 0x775a); SET_HL(0x7e7b);
  CYC(0x775a, 0x775d); parseGivenObjectData_b00_hook(gb);
}

void func_03_7619__state4__cbb3_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7769, 0x776c); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x776c, 0x776d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x776d, 0x776e); ret_effect(gb);
    return;
  }
  CYC(0x776d, 0x776e);
  CYC(0x776e, 0x7771); SET_HL(0xcfc0);
  CYC(0x7771, 0x7773); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7773, 0x7774); ret_effect(gb);
    return;
  }
  CYC(0x7773, 0x7774);
  CYC(0x7774, 0x7776); A = 0x3c;
  CYC(0x7776, 0x7779); mem_wr(gb, wTmpcbb4, A);
  CALL_C(0x7779, cleanSeas_incState_hook, 0x760f, 0x777c);
  CYC(0x777c, 0x777d); alu_xor(gb, A);
  CYC(0x777d, 0x7780); mem_wr(gb, wTmpcbb3, A);
  CYC(0x7780, 0x7783); SET_BC(0x01a3);
  CALL_C(0x7783, func_03_7619__func_764a_hook, 0x764a, 0x7786);
  CYC(0x7786, 0x7789); SET_HL(w1Link);
  CYC(0x7789, 0x778b); mem_wr(gb, HL, 0x03);
  CYC(0x778b, 0x778d); L = 0x0b;
  CYC(0x778d, 0x778f); mem_wr(gb, HL, 0x38);
  CYC(0x778f, 0x7791); L = 0x0d;
  CYC(0x7791, 0x7793); mem_wr(gb, HL, 0x68);
  CYC(0x7793, 0x7795); L = 0x08;
  CYC(0x7795, 0x7797); mem_wr(gb, HL, 0x02);
  CYC(0x7797, 0x779a); setLinkForceStateToState08_hook(gb);
}

static uint16_t clean_seas_jump_table(GB *gb) {
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

void func_03_7619_hook(GB *gb) {
  CYC(0x7619, 0x761c); A = mem_rd(gb, wCutsceneState);
  CYC(0x761c, 0x761d); push_effect(gb, 0x761d);
  switch (clean_seas_jump_table(gb)) {
    case 0x762b: func_03_7619__state0_hook(gb); return;
    case 0x767c: func_03_7619__state1_hook(gb); return;
    case 0x7702: func_03_7619__state2_hook(gb); return;
    case 0x772c: func_03_7619__state3_hook(gb); return;
    case 0x775d: func_03_7619__state4_hook(gb); return;
    case 0x779a: func_03_7619__state5_hook(gb); return;
    case 0x77d6: func_03_7619__state6_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7619__state1_hook(GB *gb) {
  CYC(0x767c, 0x767f); A = mem_rd(gb, wTmpcbb3);
  CYC(0x767f, 0x7680); push_effect(gb, 0x7680);
  switch (clean_seas_jump_table(gb)) {
    case 0x7688: func_03_7619__state1__cbb3_00_hook(gb); return;
    case 0x7695: func_03_7619__state1__cbb3_01_hook(gb); return;
    case 0x76ea: func_03_7619__state1__cbb3_03_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7619__state2_hook(GB *gb) {
  CYC(0x7702, 0x7705); A = mem_rd(gb, wTmpcbb3);
  CYC(0x7705, 0x7706); push_effect(gb, 0x7706);
  switch (clean_seas_jump_table(gb)) {
    case 0x7688: func_03_7619__state1__cbb3_00_hook(gb); return;
    case 0x7695: func_03_7619__state1__cbb3_01_hook(gb); return;
    case 0x770e: func_03_7619__state2__cbb3_03_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7619__state3_hook(GB *gb) {
  CYC(0x772c, 0x772f); A = mem_rd(gb, wTmpcbb3);
  CYC(0x772f, 0x7730); push_effect(gb, 0x7730);
  switch (clean_seas_jump_table(gb)) {
    case 0x7688: func_03_7619__state1__cbb3_00_hook(gb); return;
    case 0x7695: func_03_7619__state1__cbb3_01_hook(gb); return;
    case 0x7738: func_03_7619__state3__cbb3_03_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7619__state4_hook(GB *gb) {
  CYC(0x775d, 0x7760); A = mem_rd(gb, wTmpcbb3);
  CYC(0x7760, 0x7761); push_effect(gb, 0x7761);
  switch (clean_seas_jump_table(gb)) {
    case 0x7688: func_03_7619__state1__cbb3_00_hook(gb); return;
    case 0x7695: func_03_7619__state1__cbb3_01_hook(gb); return;
    case 0x7769: func_03_7619__state4__cbb3_03_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7619__state5_hook(GB *gb) {
  CYC(0x779a, 0x779d); A = mem_rd(gb, wTmpcbb3);
  CYC(0x779d, 0x779e); E = A;
  CYC(0x779e, 0x779f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x779f, 0x77a1);
    goto dispatch;
  }
  CYC(0x779f, 0x77a1);
  CYC(0x77a1, 0x77a4); A = mem_rd(gb, wFrameCounter);
  CYC(0x77a4, 0x77a6); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(0x77a6, 0x77a8);
    goto dispatch;
  }
  CYC(0x77a6, 0x77a8);
  CYC(0x77a8, 0x77ab); A = mem_rd(gb, w1Link_direction);
  CYC(0x77ab, 0x77ad); alu_and(gb, 0x02);
  CYC(0x77ad, 0x77af); alu_xor(gb, 0x02);
  CYC(0x77af, 0x77b1); alu_or(gb, 0x01);
  CYC(0x77b1, 0x77b4); mem_wr(gb, w1Link_direction, A);
dispatch:
  CYC(0x77b4, 0x77b5); A = E;
  CYC(0x77b5, 0x77b6); push_effect(gb, 0x77b6);
  switch (clean_seas_jump_table(gb)) {
    case 0x7688: func_03_7619__state1__cbb3_00_hook(gb); return;
    case 0x7695: func_03_7619__state1__cbb3_01_hook(gb); return;
    case 0x77be: func_03_7619__state5__cbb3_03_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7619__state5__cbb3_03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x77be, 0x77c1); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x77c1, 0x77c2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x77c2, 0x77c3); ret_effect(gb);
    return;
  }
  CYC(0x77c2, 0x77c3);
  CYC(0x77c3, 0x77c6); SET_HL(0xcfc0);
  CYC(0x77c6, 0x77c8); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x77c8, 0x77c9); ret_effect(gb);
    return;
  }
  CYC(0x77c8, 0x77c9);
  CYC(0x77c9, 0x77cb); A = 0x3c;
  CYC(0x77cb, 0x77ce); mem_wr(gb, wTmpcbb4, A);
  CYC(0x77ce, 0x77d0); A = 0x5b;
  CALL_C(0x77d0, playSound_b00_hook, 0x0c98, 0x77d3);
  CYC(0x77d3, 0x77d6); cleanSeas_incState_hook(gb);
}

void func_03_7619__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x77d6, cleanSeas_decCBB4_hook, 0x7609, 0x77d9);
  if (!(F & FZ)) {
    CYCT(0x77d9, 0x77da); ret_effect(gb);
    return;
  }
  CYC(0x77d9, 0x77da);
  CYC(0x77da, 0x77dc); A = 0x01;
  CYC(0x77dc, 0x77df); mem_wr(gb, wScrollMode, A);
  CYC(0x77df, 0x77e2); SET_HL(0x5f00);
  CYC(0x77e2, 0x77e4); E = 0x01;
  CALL_C(0x77e4, interBankCall_hook, 0x008a, 0x77e7);
  CYC(0x77e7, 0x77ea); SET_HL(0x49c9);
  CYC(0x77ea, 0x77ec); E = 0x01;
  CALL_C(0x77ec, interBankCall_hook, 0x008a, 0x77ef);
  CYC(0x77ef, 0x77f2); SET_HL(0x49d7);
  CYC(0x77f2, 0x77f4); E = 0x01;
  CALL_C(0x77f4, interBankCall_hook, 0x008a, 0x77f7);
  CYC(0x77f7, 0x77f8); alu_xor(gb, A);
  CYC(0x77f8, 0x77fb); mem_wr(gb, wMenuDisabled, A);
  CYC(0x77fb, 0x77fe); mem_wr(gb, wDisabledObjects, A);
  CYC(0x77fe, 0x7801); A = mem_rd(gb, wLoadingRoomPack);
  CYC(0x7801, 0x7804); mem_wr(gb, wRoomPack, A);
  CYC(0x7804, 0x7807); A = mem_rd(gb, wActiveRoom);
  CYC(0x7807, 0x780a); mem_wr(gb, wLoadingRoom, A);
  CYC(0x780a, 0x780c); A = 0x36;
  CYC(0x780c, 0x780f); mem_wr(gb, wEnteredWarpPosition, A);
  CYC(0x780f, 0x7812); A = mem_rd(gb, wActiveMusic2);
  CYC(0x7812, 0x7815); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x7815, playSound_b00_hook, 0x0c98, 0x7818);
  CYC(0x7818, 0x781a); A = 0x00;
  CYC(0x781a, 0x781d); mem_wr(gb, 0xc2ef, A);
  CYC(0x781d, 0x781f); A = 0x02;
  CYC(0x781f, 0x7822); mem_wr(gb, w1Link_direction, A);
  CYC(0x7822, 0x7824); A = 0x30;
  CALL_C(0x7824, setGlobalFlag_hook, 0x31f9, 0x7827);
  CYC(0x7827, 0x782a); setDeathRespawnPoint_hook(gb);
}

void nayruSingingCutsceneHandler__state0_hook(GB *gb) {
  CYC(0x6351, 0x6353); A = 0x01;
  CYC(0x6353, 0x6354); mem_wr(gb, DE, A);
  CYC(0x6354, 0x6356); A = 0x55;
  CYC(0x6356, 0x6359); playSound_b00_hook(gb);
}

void nayruSingingCutsceneHandler__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6359, 0x635b); A = 0xff;
  CYC(0x635b, 0x635e); mem_wr(gb, 0xcd25, A);
  CYC(0x635e, 0x6360); A = 0x08;
  CYC(0x6360, 0x6363); mem_wr(gb, 0xcfd0, A);
  CYC(0x6363, 0x6366); SET_HL(wMenuDisabled);
  CYC(0x6366, 0x6368); mem_wr(gb, HL, 0x01);
  CYC(0x6368, 0x636b); SET_HL(0xd01a);
  CYC(0x636b, 0x636d); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x636d, saveGraphicsOnEnterMenu_hook, 0x1aa8, 0x6370);
  CYC(0x6370, 0x6372); A = 0x0c;
  CALL_C(0x6372, loadGfxHeader_hook, 0x0626, 0x6375);
  CYC(0x6375, 0x6377); A = 0x95;
  CALL_C(0x6377, loadPaletteHeader_hook, 0x050b, 0x637a);
  CYC(0x637a, 0x637c); A = 0x04;
  CALL_C(0x637c, loadGfxRegisterStateIndex_hook, 0x02ea, 0x637f);
  CYC(0x637f, 0x6382); SET_HL(wTmpcbb3);
  CYC(0x6382, 0x6384); mem_wr(gb, HL, 0x58);
  CYC(0x6384, 0x6385); SET_HL(HL + 1);
  CYC(0x6385, 0x6387); mem_wr(gb, HL, 0x02);
  CYC(0x6387, 0x638a); SET_HL(wTmpcbb6);
  CYC(0x638a, 0x638c); mem_wr(gb, HL, 0x28);
  CALL_C(0x638c, fastFadeinFromWhite_hook, 0x3290, 0x638f);
  CALL_C(0x638f, cutscene_incCutsceneState_hook, 0x6f8c, 0x6392);
  CYC(0x6392, 0x6395); SET_HL(wTmpcbb5);
  CYC(0x6395, 0x6397); mem_wr(gb, HL, 0x02);
  nayruSingingCutsceneHandler__func_6397_hook(gb);
}

void nayruSingingCutsceneHandler__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63aa, 0x63ad); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x63ad, 0x63ae); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x63ae, 0x63b1);
    nayruSingingCutsceneHandler__func_6397_hook(gb);
    return;
  }
  CYC(0x63ae, 0x63b1);
  if (!(F & FZ)) {
    CYCT(0x63b1, 0x63b2); ret_effect(gb);
    return;
  }
  CYC(0x63b1, 0x63b2);
  CALL_C(0x63b2, nayruSingingCutsceneHandler__func_63db_hook, 0x63db, 0x63b5);
  CALL_C(0x63b5, nayruSingingCutsceneHandler__func_6397_hook, 0x6397, 0x63b8);
  CYC(0x63b8, 0x63bb); SET_HL(wTmpcbb3);
  CALL_C(0x63bb, decHlRef16WithCap_hook, 0x0237, 0x63be);
  if (F & FZ) {
    CYCT(0x63be, 0x63c0);
    goto finish;
  }
  CYC(0x63be, 0x63c0);
  CYC(0x63c0, 0x63c1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63c1, 0x63c2); H = mem_rd(gb, HL);
  CYC(0x63c2, 0x63c3); L = A;
  CYC(0x63c3, 0x63c6); SET_BC(0x00f0);
  CALL_C(0x63c6, compareHlToBc_hook, 0x01d6, 0x63c9);
  if (!(F & FC)) {
    CYCT(0x63c9, 0x63ca); ret_effect(gb);
    return;
  }
  CYC(0x63c9, 0x63ca);
  CYC(0x63ca, 0x63cd); A = mem_rd(gb, 0xc482);
  CYC(0x63cd, 0x63cf); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x63cf, 0x63d0); ret_effect(gb);
    return;
  }
  CYC(0x63cf, 0x63d0);
finish:
  CYC(0x63d0, 0x63d2); A = 0x55;
  CALL_C(0x63d2, playSound_b00_hook, 0x0c98, 0x63d5);
  CALL_C(0x63d5, cutscene_incCutsceneState_hook, 0x6f8c, 0x63d8);
  CYC(0x63d8, 0x63db); fastFadeoutToWhite_hook(gb);
}

void nayruSingingCutsceneHandler__state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63ed, 0x63f0); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x63f0, 0x63f1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x63f1, 0x63f4);
    nayruSingingCutsceneHandler__func_6397_hook(gb);
    return;
  }
  CYC(0x63f1, 0x63f4);
  if (!(F & FZ)) {
    CYCT(0x63f4, 0x63f5); ret_effect(gb);
    return;
  }
  CYC(0x63f4, 0x63f5);
  CYC(0x63f5, 0x63f6); alu_xor(gb, A);
  CYC(0x63f6, 0x63f9); mem_wr(gb, 0xcd25, A);
  CYC(0x63f9, 0x63fc); SET_HL(0xd01a);
  CYC(0x63fc, 0x63fe); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x63fe, 0x6400); A = 0x09;
  CYC(0x6400, 0x6403); mem_wr(gb, 0xcfd0, A);
  CALL_C(0x6403, cutscene_incCutsceneState_hook, 0x6f8c, 0x6406);
  CYC(0x6406, 0x6409); SET_HL(wTmpcbb3);
  CYC(0x6409, 0x640b); mem_wr(gb, HL, 0xaa);
  CYC(0x640b, 0x640e); reloadGraphicsOnExitMenu_hook(gb);
}

void nayruSingingCutsceneHandler__state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x640e, 0x6411); A = mem_rd(gb, 0xcfd0);
  CYC(0x6411, 0x6413); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x6413, 0x6414); ret_effect(gb);
    return;
  }
  CYC(0x6413, 0x6414);
  CALL_C(0x6414, cutscene_incCutsceneState_hook, 0x6f8c, 0x6417);
  CYC(0x6417, 0x641a); SET_HL(0xde90);
  CYC(0x641a, 0x641d); SET_BC(0x44a8);
  CYC(0x641d, 0x6420); func_13c6_hook(gb);
}

void nayruSingingCutsceneHandler__state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6420, 0x6423); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6423, 0x6424); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6424, 0x6425); ret_effect(gb);
    return;
  }
  CYC(0x6424, 0x6425);
  CYC(0x6425, 0x6427); A = 0x99;
  CALL_C(0x6427, loadPaletteHeader_hook, 0x050b, 0x642a);
  CYC(0x642a, 0x642c); A = 0x10;
  CYC(0x642c, 0x642f); mem_wr(gb, 0xcfd0, A);
  CYC(0x642f, 0x6432); cutscene_incCutsceneState_hook(gb);
}

void nayruSingingCutsceneHandler__state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6432, 0x6435); A = mem_rd(gb, 0xcfd0);
  CYC(0x6435, 0x6437); alu_cp(gb, 0x14);
  if (!(F & FZ)) {
    CYCT(0x6437, 0x6438); ret_effect(gb);
    return;
  }
  CYC(0x6437, 0x6438);
  CALL_C(0x6438, cutscene_incCutsceneState_hook, 0x6f8c, 0x643b);
  CYC(0x643b, 0x643e); SET_HL(wTmpcbb3);
  CYC(0x643e, 0x6440); mem_wr(gb, HL, 0x3c);
  CYC(0x6440, 0x6443); fadeoutToWhite_hook(gb);
}

void nayruSingingCutsceneHandler__state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6443, cutscene_decCBB3IfNotFadingOut_hook, 0x6070, 0x6446);
  if (!(F & FZ)) {
    CYCT(0x6446, 0x6447); ret_effect(gb);
    return;
  }
  CYC(0x6446, 0x6447);
  CALL_C(0x6447, cutscene_incCutsceneState_hook, 0x6f8c, 0x644a);
  CYC(0x644a, 0x644c); A = 0x15;
  CYC(0x644c, 0x644f); mem_wr(gb, 0xcfd0, A);
  CYC(0x644f, 0x6451); A = 0x03;
  CYC(0x6451, 0x6454); fadeinFromWhiteWithDelay_hook(gb);
}

void nayruSingingCutsceneHandler__state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6454, 0x6457); A = mem_rd(gb, 0xcfd0);
  CYC(0x6457, 0x6459); alu_cp(gb, 0x18);
  if (!(F & FZ)) {
    CYCT(0x6459, 0x645a); ret_effect(gb);
    return;
  }
  CYC(0x6459, 0x645a);
  CALL_C(0x645a, cutscene_incCutsceneState_hook, 0x6f8c, 0x645d);
  CYC(0x645d, 0x645e); alu_xor(gb, A);
  CYC(0x645e, 0x6461); mem_wr(gb, 0xcfd2, A);
  CALL_C(0x6461, getFreePartSlot_hook, 0x3e8e, 0x6464);
  if (!(F & FZ)) {
    CYCT(0x6464, 0x6465); ret_effect(gb);
    return;
  }
  CYC(0x6464, 0x6465);
  CYC(0x6465, 0x6467); mem_wr(gb, HL, 0x27);
  CYC(0x6467, 0x6468); L = alu_inc8(gb, L);
  CYC(0x6468, 0x6469); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6469, 0x646a); L = alu_inc8(gb, L);
  CYC(0x646a, 0x646b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x646b, 0x646d); L = 0xcb;
  CYC(0x646d, 0x646f); mem_wr(gb, HL, 0x24);
  CYC(0x646f, 0x6471); L = 0xcd;
  CYC(0x6471, 0x6473); mem_wr(gb, HL, 0x28);
  CYC(0x6473, 0x6474); ret_effect(gb);
}

void nayruSingingCutsceneHandler__state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6474, 0x6477); A = mem_rd(gb, 0xcfd2);
  CYC(0x6477, 0x6478); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6478, 0x6479); ret_effect(gb);
    return;
  }
  CYC(0x6478, 0x6479);
  CALL_C(0x6479, cutscene_incCutsceneState_hook, 0x6f8c, 0x647c);
  CALL_C(0x647c, getThisRoomFlags_hook, 0x197d, 0x647f);
  CYC(0x647f, 0x6481); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(0x6481, 0x6483); C = 0x22;
  CYC(0x6483, 0x6485); A = 0xd7;
  CYC(0x6485, 0x6488); setTile_hook(gb);
}

void nayruSingingCutsceneHandler__stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6488, 0x648b); A = mem_rd(gb, 0xcfd0);
  CYC(0x648b, 0x648d); alu_cp(gb, 0x1d);
  if (!(F & FZ)) {
    CYCT(0x648d, 0x648e); ret_effect(gb);
    return;
  }
  CYC(0x648d, 0x648e);
  CYC(0x648e, 0x6491); SET_HL(wTmpcbb3);
  CYC(0x6491, 0x6493); mem_wr(gb, HL, 0x78);
  CYC(0x6493, 0x6496); cutscene_incCutsceneState_hook(gb);
}

void nayruSingingCutsceneHandler__stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6496, decCbb3_hook, 0x305d, 0x6499);
  if (!(F & FZ)) {
    CYCT(0x6499, 0x649a); ret_effect(gb);
    return;
  }
  CYC(0x6499, 0x649a);
  CYC(0x649a, 0x649c); mem_wr(gb, HL, 0x5a);
  CALL_C(0x649c, cutscene_incCutsceneState_hook, 0x6f8c, 0x649f);
  CYC(0x649f, 0x64a2); SET_BC(0x5607);
  CYC(0x64a2, 0x64a5); showText_hook(gb);
}

void nayruSingingCutsceneHandler__stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x64a5, cutscene_decCBB3IfTextNotActive_hook, 0x6068, 0x64a8);
  if (!(F & FZ)) {
    CYCT(0x64a8, 0x64a9); ret_effect(gb);
    return;
  }
  CYC(0x64a8, 0x64a9);
  CALL_C(0x64a9, cutscene_incCutsceneState_hook, 0x6f8c, 0x64ac);
  CYC(0x64ac, 0x64ad); alu_xor(gb, A);
  CYC(0x64ad, 0x64b0); SET_HL(0xcfde);
  CYC(0x64b0, 0x64b1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x64b1, 0x64b2); mem_wr(gb, HL, A);
  CYC(0x64b2, 0x64b5); fadeoutToWhite_hook(gb);
}

void nayruSingingStateE_hook(GB *gb) {
  CYC(0x64df, 0x64e2); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x64e2, 0x64e3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x64e3, 0x64e4); ret_effect(gb);
    return;
  }
  CYC(0x64e3, 0x64e4);
  CYC(0x64e4, 0x64e7); SET_HL(0xcfdf);
  CYC(0x64e7, 0x64e8); A = mem_rd(gb, HL);
  CYC(0x64e8, 0x64ea); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x64ea, 0x64eb); ret_effect(gb);
    return;
  }
  CYC(0x64ea, 0x64eb);
  CYC(0x64eb, 0x64ec); alu_xor(gb, A);
  CYC(0x64ec, 0x64ed); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x64ed, 0x64ee); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x64ee, 0x64ef); A = mem_rd(gb, HL);
  CYC(0x64ef, 0x64f1); alu_cp(gb, 0x03);
  CYC(0x64f1, 0x64f3); A = 0x0d;
  if (!(F & FZ)) {
    CYCT(0x64f3, 0x64f5);
  } else {
    CYC(0x64f3, 0x64f5);
    CYC(0x64f5, 0x64f7); A = 0x0f;
  }
  CYC(0x64f7, 0x64fa); SET_HL(wCutsceneState);
  CYC(0x64fa, 0x64fb); mem_wr(gb, HL, A);
  CYC(0x64fb, 0x64fe); fadeoutToWhite_hook(gb);
}

void nayruSingingState10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6538, 0x653b); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x653b, 0x653c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x653c, 0x653d); ret_effect(gb);
    return;
  }
  CYC(0x653c, 0x653d);
  CYC(0x653d, 0x6540); A = mem_rd(gb, 0xcfd0);
  CYC(0x6540, 0x6542); alu_cp(gb, 0x1e);
  if (!(F & FZ)) {
    CYCT(0x6542, 0x6543); ret_effect(gb);
    return;
  }
  CYC(0x6542, 0x6543);
  CALL_C(0x6543, cutscene_incCutsceneState_hook, 0x6f8c, 0x6546);
  CYC(0x6546, 0x6549); SET_HL(0xde90);
  CYC(0x6549, 0x654c); SET_BC(0x4a30);
  CYC(0x654c, 0x654f); func_13c6_hook(gb);
}

void nayruSingingState11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x654f, 0x6552); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6552, 0x6553); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6553, 0x6554); ret_effect(gb);
    return;
  }
  CYC(0x6553, 0x6554);
  CYC(0x6554, 0x6556); A = 0x10;
  CALL_C(0x6556, loadPaletteHeader_hook, 0x050b, 0x6559);
  CYC(0x6559, 0x655b); A = 0x1f;
  CYC(0x655b, 0x655e); mem_wr(gb, 0xcfd0, A);
  CYC(0x655e, 0x6560); A = 0x01;
  CYC(0x6560, 0x6563); mem_wr(gb, 0xc2ef, A);
  CYC(0x6563, 0x6564); ret_effect(gb);
}
