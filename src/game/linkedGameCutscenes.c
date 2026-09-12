#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

void linkedCutscene_aIntoCBB5_incSubstate_hook(GB *gb);
void linkedCutscene_incSubstate_hook(GB *gb);
void func_7ba1_hook(GB *gb);
static void linkedCutscene_decrementTimer(GB *gb);
void func_7bd9_hook(GB *gb);
void func_7bff_hook(GB *gb);
void func_7c09_hook(GB *gb);
void spawnZeldaKidnappedNPCs_hook(GB *gb);
void func_03_7841_hook(GB *gb);
void func_03_7851_hook(GB *gb);
void flameOfSorrowState1_hook(GB *gb);
void flameOfSorrowState1__substate0_hook(GB *gb);
void flameOfSorrowState1__substate1_hook(GB *gb);
void flameOfSorrowState1__substate2_hook(GB *gb);
void flameOfSorrowState1__substate3_hook(GB *gb);
void flameOfSorrowState1__substate4_hook(GB *gb);
void flameOfSorrowState1__substate5_hook(GB *gb);
void flameOfSorrowState1__func_78fd_hook(GB *gb);
void flameOfSorrowState1__substate6_hook(GB *gb);
void flameOfSorrowState1__substate7_hook(GB *gb);
void flameOfSorrowState1__substate8_hook(GB *gb);
void flameOfSorrowState1__substate9_hook(GB *gb);
void flameOfSorrowState1__substateA_hook(GB *gb);
void flameOfSorrowState1__substateB_hook(GB *gb);

static uint16_t linked_cutscene_jump_table(GB *gb) {
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

void func_03_7841_hook(GB *gb) {
  CYC(0x7841, 0x7844); A = mem_rd(gb, wCutsceneState);
  CYC(0x7844, 0x7845); push_effect(gb, 0x7845);
  switch (linked_cutscene_jump_table(gb)) {
    case 0x7851: func_03_7851_hook(gb); return;
    case 0x786b: flameOfSorrowState1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void func_03_7851_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7851, 0x7853); B = 0x10;
  CYC(0x7853, 0x7856); SET_HL(wTmpcbb3);
  CALL_C(0x7856, clearMemory_hook, 0x046f, 0x7859);
  CALL_C(0x7859, clearWramBank1_hook, 0x3597, 0x785c);
  CYC(0x785c, 0x785d); alu_xor(gb, A);
  CYC(0x785d, 0x7860); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7860, 0x7862); A = 0x80;
  CYC(0x7862, 0x7865); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7865, 0x7867); A = 0x01;
  CYC(0x7867, 0x786a); mem_wr(gb, wCutsceneState, A);
  CYC(0x786a, 0x786b); ret_effect(gb);
}

void flameOfSorrowState1_hook(GB *gb) {
  CYC(0x786b, 0x786e); A = mem_rd(gb, wTmpcbb3);
  CYC(0x786e, 0x786f); push_effect(gb, 0x786f);
  switch (linked_cutscene_jump_table(gb)) {
    case 0x7887: flameOfSorrowState1__substate0_hook(gb); return;
    case 0x788f: flameOfSorrowState1__substate1_hook(gb); return;
    case 0x78b8: flameOfSorrowState1__substate2_hook(gb); return;
    case 0x78c7: flameOfSorrowState1__substate3_hook(gb); return;
    case 0x78e1: flameOfSorrowState1__substate4_hook(gb); return;
    case 0x78ef: flameOfSorrowState1__substate5_hook(gb); return;
    case 0x7913: flameOfSorrowState1__substate6_hook(gb); return;
    case 0x7936: flameOfSorrowState1__substate7_hook(gb); return;
    case 0x793f: flameOfSorrowState1__substate8_hook(gb); return;
    case 0x7948: flameOfSorrowState1__substate9_hook(gb); return;
    case 0x7951: flameOfSorrowState1__substateA_hook(gb); return;
    case 0x7960: flameOfSorrowState1__substateB_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void flameOfSorrowState1__substate0_hook(GB *gb) {
  CYC(0x7887, 0x7889); A = 0x28;
  CYC(0x7889, 0x788c); mem_wr(gb, wTmpcbb5, A);
  CYC(0x788c, 0x788f);
  linkedCutscene_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x788f, func_03_7b95_hook, 0x7b95, 0x7892);
  if (!(F & FZ)) { CYCT(0x7892, 0x7893); ret_effect(gb); return; }
  CYC(0x7892, 0x7893);
  CALL_C(0x7893, func_7bab_hook, 0x7bab, 0x7896);
  CALL_C(0x7896, getFreeInteractionSlot_hook, 0x3aef, 0x7899);
  if (!(F & FZ)) CYCT(0x7899, 0x789b);
  else {
    CYC(0x7899, 0x789b);
    CYC(0x789b, 0x789d); mem_wr(gb, HL, 0xa9);
    CYC(0x789d, 0x789e); L = alu_inc8(gb, L);
    CYC(0x789e, 0x78a0); mem_wr(gb, HL, 0x01);
  }
  CYC(0x78a0, 0x78a2); A = 0x13;
  CALL_C(0x78a2, loadGfxRegisterStateIndex_hook, 0x02ea, 0x78a5);
  CYC(0x78a5, 0x78a7); A = 0xd2;
  CALL_C(0x78a7, playSound_b00_hook, 0x0c98, 0x78aa);
  CYC(0x78aa, 0x78ab); alu_xor(gb, A);
  CYC(0x78ab, 0x78ae); mem_wr(gb, wTmpcbb5, A);
  CYC(0x78ae, 0x78b1); mem_wr(gb, wTmpcbb6, A);
  CYC(0x78b1, 0x78b2); A = alu_dec8(gb, A);
  CYC(0x78b2, 0x78b5); mem_wr(gb, wTmpcbba, A);
  CALL_C(0x78b5, linkedCutscene_incSubstate_hook, 0x7b8b, 0x78b8);
  flameOfSorrowState1__substate2_hook(gb);
}

void flameOfSorrowState1__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x78b8, 0x78bb); SET_HL(wTmpcbb5);
  CYC(0x78bb, 0x78bd); B = 0x05;
  CALL_C(0x78bd, flashScreen_hook, 0x2d73, 0x78c0);
  if (F & FZ) { CYCT(0x78c0, 0x78c1); ret_effect(gb); return; }
  CYC(0x78c0, 0x78c1);
  CALL_C(0x78c1, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x78c4);
  CYC(0x78c4, 0x78c7);
  linkedCutscene_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x78c7, getFreeInteractionSlot_hook, 0x3aef, 0x78ca);
  if (!(F & FZ)) CYCT(0x78ca, 0x78cc);
  else {
    CYC(0x78ca, 0x78cc);
    CYC(0x78cc, 0x78ce); mem_wr(gb, HL, 0xa9);
  }
  CYC(0x78ce, 0x78d0); A = 0xf0;
  CALL_C(0x78d0, playSound_b00_hook, 0x0c98, 0x78d3);
  CALL_C(0x78d3, clearFadingPalettes2_hook, 0x542e, 0x78d6);
  CYC(0x78d6, 0x78d8); A = 0xbf;
  CYC(0x78d8, 0x78da); hram_wr(gb, 0xa9, A);
  CYC(0x78da, 0x78dc); hram_wr(gb, 0xa7, A);
  CYC(0x78dc, 0x78de); A = 0x04;
  CYC(0x78de, 0x78e1);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x78e1, func_03_7b95_hook, 0x7b95, 0x78e4);
  if (!(F & FZ)) { CYCT(0x78e4, 0x78e5); ret_effect(gb); return; }
  CYC(0x78e4, 0x78e5);
  CYC(0x78e5, 0x78e7); A = 0x04;
  CYC(0x78e7, 0x78ea); mem_wr(gb, wTextboxFlags, A);
  CYC(0x78ea, 0x78ec); C = 0x1b;
  CYC(0x78ec, 0x78ef);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x78ef, func_7b9a_hook, 0x7b9a, 0x78f2);
  if (!(F & FZ)) { CYCT(0x78f2, 0x78f3); ret_effect(gb); return; }
  CYC(0x78f2, 0x78f3);
  CYC(0x78f3, 0x78f5); B = 0x10;
  CALL_C(0x78f5, flameOfSorrowState1__func_78fd_hook, 0x78fd, 0x78f8);
  CYC(0x78f8, 0x78fa); A = 0x1e;
  CYC(0x78fa, 0x78fd);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void flameOfSorrowState1__func_78fd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x78fd, fastFadeinFromBlack_hook, 0x32dd, 0x7900);
  CYC(0x7900, 0x7901); A = B;
  CYC(0x7901, 0x7904); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x7904, 0x7907); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x7907, 0x7908); alu_xor(gb, A);
  CYC(0x7908, 0x790b); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x790b, 0x790e); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x790e, 0x7910); A = 0x72;
  CYC(0x7910, 0x7913);
  playSound_b00_hook(gb);
}

void flameOfSorrowState1__substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7913, func_7ba1_hook, 0x7ba1, 0x7916);
  if (!(F & FZ)) { CYCT(0x7916, 0x7917); ret_effect(gb); return; }
  CYC(0x7916, 0x7917);
  CALL_C(0x7917, fadeinFromBlack_hook, 0x32e6, 0x791a);
  CYC(0x791a, 0x791c); A = 0xaf;
  CYC(0x791c, 0x791f); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x791f, 0x7922); mem_wr(gb, wFadeSprPaletteSources, A);
  CALL_C(0x7922, func_7bd0_hook, 0x7bd0, 0x7925);
  CYC(0x7925, 0x7927); A = 0x21;
  CYC(0x7927, 0x792a); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x792a, playSound_b00_hook, 0x0c98, 0x792d);
  CYC(0x792d, 0x792e); alu_xor(gb, A);
  CYC(0x792e, 0x7931); mem_wr(gb, 0xcfc6, A);
  CYC(0x7931, 0x7933); A = 0x1e;
  CYC(0x7933, 0x7936);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7936, func_7ba1_hook, 0x7ba1, 0x7939);
  if (!(F & FZ)) { CYCT(0x7939, 0x793a); ret_effect(gb); return; }
  CYC(0x7939, 0x793a);
  CYC(0x793a, 0x793c); C = 0x29;
  CYC(0x793c, 0x793f);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x793f, func_7b9a_hook, 0x7b9a, 0x7942);
  if (!(F & FZ)) { CYCT(0x7942, 0x7943); ret_effect(gb); return; }
  CYC(0x7942, 0x7943);
  CYC(0x7943, 0x7945); C = 0x1c;
  CYC(0x7945, 0x7948);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7948, func_7b9a_hook, 0x7b9a, 0x794b);
  if (!(F & FZ)) { CYCT(0x794b, 0x794c); ret_effect(gb); return; }
  CYC(0x794b, 0x794c);
  CYC(0x794c, 0x794e); C = 0x1d;
  CYC(0x794e, 0x7951);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7951, func_7b9a_hook, 0x7b9a, 0x7954);
  if (!(F & FZ)) { CYCT(0x7954, 0x7955); ret_effect(gb); return; }
  CYC(0x7954, 0x7955);
  CYC(0x7955, 0x7957); C = 0x1e;
  CALL_C(0x7957, func_03_7b81_hook, 0x7b81, 0x795a);
  CYC(0x795a, 0x795c); A = 0x3c;
  CYC(0x795c, 0x795f); mem_wr(gb, wTmpcbb5, A);
  CYC(0x795f, 0x7960); ret_effect(gb);
}

void flameOfSorrowState1__substateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7960, func_7b9a_hook, 0x7b9a, 0x7963);
  if (!(F & FZ)) { CYCT(0x7963, 0x7964); ret_effect(gb); return; }
  CYC(0x7963, 0x7964);
  CYC(0x7964, 0x7965); alu_xor(gb, A);
  CYC(0x7965, 0x7968); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7968, 0x796b); SET_HL(0x7978);
  CALL_C(0x796b, setWarpDestVariables_hook, 0x1997, 0x796e);
  CYC(0x796e, 0x7970); A = 0x00;
  CYC(0x7970, 0x7973); mem_wr(gb, wcc50, A);
  CYC(0x7973, 0x7975); A = 0x0f;
  CYC(0x7975, 0x7978);
  loadPaletteHeader_hook(gb);
}

void func_03_7b81_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b81, 0x7b83); B = 0x28;
  CALL_C(0x7b83, showText_hook, 0x1872, 0x7b86);
  CYC(0x7b86, 0x7b88); A = 0x1e;
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void linkedCutscene_aIntoCBB5_incSubstate_hook(GB *gb) {
  CYC(0x7b88, 0x7b8b); W8(wTmpcbb5) = A;
  linkedCutscene_incSubstate_hook(gb);
}

void linkedCutscene_incSubstate_hook(GB *gb) {
  CYC(0x7b8b, 0x7b8e); SET_HL(wTmpcbb3);
  CYC(0x7b8e, 0x7b8f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7b8f, 0x7b90); ret_effect(gb);
}

void func_03_7b90_hook(GB *gb) {
  CYC(0x7b90, 0x7b93); SET_HL(wTmpcbb4);
  CYC(0x7b93, 0x7b94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7b94, 0x7b95); ret_effect(gb);
}

void func_03_7b95_hook(GB *gb) {
  CYC(0x7b95, 0x7b98); SET_HL(wTmpcbb5);
  CYC(0x7b98, 0x7b99); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7b99, 0x7b9a); ret_effect(gb);
}

void func_7b9a_hook(GB *gb) {
  CYC(0x7b9a, 0x7b9d); A = W8(wTextIsActive);
  CYC(0x7b9d, 0x7b9e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7b9e, 0x7b9f); ret_effect(gb); return; }
  CYC(0x7b9e, 0x7b9f);
  CYC(0x7b9f, 0x7ba1);
  linkedCutscene_decrementTimer(gb);
}

void func_7ba1_hook(GB *gb) {
  CYC(0x7ba1, 0x7ba4); A = W8(wPaletteThread_mode);
  CYC(0x7ba4, 0x7ba5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7ba5, 0x7ba6); ret_effect(gb); return; }
  CYC(0x7ba5, 0x7ba6);
  linkedCutscene_decrementTimer(gb);
}

static void linkedCutscene_decrementTimer(GB *gb) {
  CYC(0x7ba6, 0x7ba9); SET_HL(wTmpcbb5);
  CYC(0x7ba9, 0x7baa); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7baa, 0x7bab); ret_effect(gb);
}

void func_7bab_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7bab, 0x7bac); alu_xor(gb, A);
  CYC(0x7bac, 0x7baf); SET_BC(0x05f1);
  CALL_C(0x7baf, disableLcdAndLoadRoom_hook, 0x30b0, 0x7bb2);
  CYC(0x7bb2, 0x7bb4); A = 0xac;
  CALL_C(0x7bb4, loadPaletteHeader_hook, 0x050b, 0x7bb7);
  CYC(0x7bb7, 0x7bb9); A = 0x28;
  CYC(0x7bb9, 0x7bbc); W8(wGfxRegs1_SCX) = A;
  CYC(0x7bbc, 0x7bbf); W8(wGfxRegs2_SCX) = A;
  CYC(0x7bbf, 0x7bc1); hram_wr(gb, 0xac, A);
  CYC(0x7bc1, 0x7bc2); alu_xor(gb, A);
  CYC(0x7bc2, 0x7bc4); hram_wr(gb, 0xaa, A);
  CYC(0x7bc4, 0x7bc6); A = 0;
  CYC(0x7bc6, 0x7bc9); W8(wScrollMode) = A;
  CYC(0x7bc9, 0x7bcb); A = 0x10;
  CYC(0x7bcb, 0x7bcd); hram_wr(gb, 0x9f, A);
  CYC(0x7bcd, 0x7bd0); clearWramBank1_hook(gb);
}

void func_7bd0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7bd0, 0x7bd3); SET_BC(0x7be5);
  CALL_C(0x7bd3, func_7bd9_hook, 0x7bd9, 0x7bd6);
  CYC(0x7bd6, 0x7bd9); SET_BC(0x7be8);
  func_7bd9_hook(gb);
}

void func_7bd9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7bd9, getFreeInteractionSlot_hook, 0x3aef, 0x7bdc);
  if (!(F & FZ)) { CYCT(0x7bdc, 0x7bdd); ret_effect(gb); return; }
  CYC(0x7bdc, 0x7bdd);
  CYC(0x7bdd, 0x7bdf); mem_wr(gb, HL, 0xb0);
  CYC(0x7bdf, 0x7be0); L = alu_inc8(gb, L);
  CYC(0x7be0, 0x7be1); A = mem_rd(gb, BC);
  CYC(0x7be1, 0x7be2); SET_BC(BC + 1);
  CYC(0x7be2, 0x7be3); mem_wr(gb, HL, A);
  CYC(0x7be3, 0x7be5); func_7c09_hook(gb);
}

void func_7beb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7beb, 0x7bee); SET_BC(0x7c13);
  CALL_C(0x7bee, func_7bff_hook, 0x7bff, 0x7bf1);
  CYC(0x7bf1, 0x7bf4); SET_BC(0x7c16);
  CYC(0x7bf4, 0x7bf6); func_7bff_hook(gb);
}

void func_7bf6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7bf6, 0x7bf9); SET_BC(0x7c19);
  CALL_C(0x7bf9, func_7bff_hook, 0x7bff, 0x7bfc);
  CYC(0x7bfc, 0x7bff); SET_BC(0x7c1c);
  func_7bff_hook(gb);
}

void func_7bff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7bff, getFreeInteractionSlot_hook, 0x3aef, 0x7c02);
  if (!(F & FZ)) { CYCT(0x7c02, 0x7c03); ret_effect(gb); return; }
  CYC(0x7c02, 0x7c03);
  CYC(0x7c03, 0x7c05); mem_wr(gb, HL, 0xbc);
  CYC(0x7c05, 0x7c06); L = alu_inc8(gb, L);
  CYC(0x7c06, 0x7c07); A = mem_rd(gb, BC);
  CYC(0x7c07, 0x7c08); SET_BC(BC + 1);
  CYC(0x7c08, 0x7c09); mem_wr(gb, HL, A);
  func_7c09_hook(gb);
}

void func_7c09_hook(GB *gb) {
  CYC(0x7c09, 0x7c0b); L = 0x4b;
  CYC(0x7c0b, 0x7c0c); A = mem_rd(gb, BC);
  CYC(0x7c0c, 0x7c0d); SET_BC(BC + 1);
  CYC(0x7c0d, 0x7c0e); mem_wr(gb, HL, A);
  CYC(0x7c0e, 0x7c10); L = 0x4d;
  CYC(0x7c10, 0x7c11); A = mem_rd(gb, BC);
  CYC(0x7c11, 0x7c12); mem_wr(gb, HL, A);
  CYC(0x7c12, 0x7c13); ret_effect(gb);
}

void func_7c1f_hook(GB *gb) {
  CYC(0x7c1f, 0x7c21); A = 1;
  CYC(0x7c21, 0x7c24); W8(wLoadedTreeGfxIndex) = A;
  CYC(0x7c24, 0x7c26); A = 0xbc;
  CYC(0x7c26, 0x7c29); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(0x7c29, 0x7c2a); ret_effect(gb);
}

void func_7c2a_hook(GB *gb) {
  CYC(0x7c2a, 0x7c2d); SET_BC(0x7c4e);
  CYC(0x7c2d, 0x7c2f); spawnZeldaKidnappedNPCs_hook(gb);
}

void func_7c2f_hook(GB *gb) {
  CYC(0x7c2f, 0x7c32); SET_BC(0x7c5d);
  spawnZeldaKidnappedNPCs_hook(gb);
}

void spawnZeldaKidnappedNPCs_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x7c32, 0x7c33); A = mem_rd(gb, BC);
    CYC(0x7c33, 0x7c34); alu_or(gb, A);
    if (F & FZ) { CYCT(0x7c34, 0x7c35); ret_effect(gb); return; }
    CYC(0x7c34, 0x7c35);
    CALL_C(0x7c35, getFreeInteractionSlot_hook, 0x3aef, 0x7c38);
    if (!(F & FZ)) { CYCT(0x7c38, 0x7c39); ret_effect(gb); return; }
    CYC(0x7c38, 0x7c39);
    CYC(0x7c39, 0x7c3a); A = mem_rd(gb, BC);
    CYC(0x7c3a, 0x7c3b); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7c3b, 0x7c3c); SET_BC(BC + 1);
    CYC(0x7c3c, 0x7c3d); A = mem_rd(gb, BC);
    CYC(0x7c3d, 0x7c3e); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7c3e, 0x7c3f); SET_BC(BC + 1);
    CYC(0x7c3f, 0x7c40); A = mem_rd(gb, BC);
    CYC(0x7c40, 0x7c41); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7c41, 0x7c42); SET_BC(BC + 1);
    CYC(0x7c42, 0x7c44); L = 0x4b;
    CYC(0x7c44, 0x7c45); A = mem_rd(gb, BC);
    CYC(0x7c45, 0x7c46); mem_wr(gb, HL, A);
    CYC(0x7c46, 0x7c47); SET_BC(BC + 1);
    CYC(0x7c47, 0x7c49); L = 0x4d;
    CYC(0x7c49, 0x7c4a); A = mem_rd(gb, BC);
    CYC(0x7c4a, 0x7c4b); mem_wr(gb, HL, A);
    CYC(0x7c4b, 0x7c4c); SET_BC(BC + 1);
    CYCT(0x7c4c, 0x7c4e);
  }
}

void func_7c68_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c68, 0x7c6b); SET_BC(0x7c7f);
  CALL_C(0x7c6b, addDoubleIndexToBc_hook, 0x007e, 0x7c6e);
  CALL_C(0x7c6e, getFreePartSlot_hook, 0x3e8e, 0x7c71);
  if (!(F & FZ)) { CYCT(0x7c71, 0x7c72); ret_effect(gb); return; }
  CYC(0x7c71, 0x7c72); CYC(0x7c72, 0x7c74); mem_wr(gb, HL, 0x27);
  CYC(0x7c74, 0x7c75); L = alu_inc8(gb, L);
  CYC(0x7c75, 0x7c76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c76, 0x7c78); L = 0xcb;
  CYC(0x7c78, 0x7c79); A = mem_rd(gb, BC);
  CYC(0x7c79, 0x7c7a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7c7a, 0x7c7b); SET_BC(BC + 1);
  CYC(0x7c7b, 0x7c7c); L = alu_inc8(gb, L);
  CYC(0x7c7c, 0x7c7d); A = mem_rd(gb, BC);
  CYC(0x7c7d, 0x7c7e); mem_wr(gb, HL, A);
  CYC(0x7c7e, 0x7c7f); ret_effect(gb);
}

void func_7c83_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c83, 0x7c86); SET_BC(0x7c7f);
  CALL_C(0x7c86, addDoubleIndexToBc_hook, 0x007e, 0x7c89);
  CALL_C(0x7c89, getFreeInteractionSlot_hook, 0x3aef, 0x7c8c);
  if (!(F & FZ)) { CYCT(0x7c8c, 0x7c8d); ret_effect(gb); return; }
  CYC(0x7c8c, 0x7c8d); CYC(0x7c8d, 0x7c8f); mem_wr(gb, HL, 0x6b);
  CYC(0x7c8f, 0x7c90); L = alu_inc8(gb, L);
  CYC(0x7c90, 0x7c92); mem_wr(gb, HL, 0x16);
  CYC(0x7c92, 0x7c94); L = 0x46;
  CYC(0x7c94, 0x7c96); mem_wr(gb, HL, 0x78);
  CYC(0x7c96, 0x7c99); func_7c09_hook(gb);
}
