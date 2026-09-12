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
void func_03_7849_hook(GB *gb);
void zeldaKidnappedState1_hook(GB *gb);
void zeldaKidnappedState1__runStates_hook(GB *gb);
void zeldaKidnappedState1__substate0_hook(GB *gb);
void zeldaKidnappedState1__substate1_hook(GB *gb);
void zeldaKidnappedState1__substate2_hook(GB *gb);
void zeldaKidnappedState1__substate3_hook(GB *gb);
void zeldaKidnappedState1__substate4_hook(GB *gb);
void zeldaKidnappedState1__substate5_hook(GB *gb);
void zeldaKidnappedState1__substate6_hook(GB *gb);
void zeldaKidnappedState1__substate7_hook(GB *gb);
void zeldaKidnappedState1__substate8_hook(GB *gb);
void zeldaKidnappedState1__substate9_hook(GB *gb);
void zeldaKidnappedState1__func_7a63_hook(GB *gb);
void zeldaKidnappedState1__substateA_hook(GB *gb);
void zeldaKidnappedState1__substateB_hook(GB *gb);
void zeldaKidnappedState1__substateC_hook(GB *gb);
void zeldaKidnappedState1__substateD_hook(GB *gb);
void zeldaKidnappedState1__substateE_hook(GB *gb);
void zeldaKidnappedState1__substateF_hook(GB *gb);
void zeldaKidnappedState1__substate11_hook(GB *gb);
void zeldaKidnappedState1__substate12_hook(GB *gb);
void zeldaKidnappedState1__substate13_hook(GB *gb);
void zeldaKidnappedState1__substate14_hook(GB *gb);
void zeldaKidnappedState1__substate15_hook(GB *gb);
void zeldaKidnappedState1__substate16_hook(GB *gb);

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

void func_03_7849_hook(GB *gb) {
  CYC(0x7849, 0x784c); A = mem_rd(gb, wCutsceneState);
  CYC(0x784c, 0x784d); push_effect(gb, 0x784d);
  switch (linked_cutscene_jump_table(gb)) {
    case 0x7851: func_03_7851_hook(gb); return;
    case 0x797d: zeldaKidnappedState1_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void zeldaKidnappedState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x797d, zeldaKidnappedState1__runStates_hook, 0x7983, 0x7980);
  CYC(0x7980, 0x7983);
  updateStatusBar_hook(gb);
}

void zeldaKidnappedState1__runStates_hook(GB *gb) {
  CYC(0x7983, 0x7986); A = mem_rd(gb, wTmpcbb3);
  CYC(0x7986, 0x7987); push_effect(gb, 0x7987);
  switch (linked_cutscene_jump_table(gb)) {
    case 0x79b5: zeldaKidnappedState1__substate0_hook(gb); return;
    case 0x79d8: zeldaKidnappedState1__substate1_hook(gb); return;
    case 0x79e6: zeldaKidnappedState1__substate2_hook(gb); return;
    case 0x79f9: zeldaKidnappedState1__substate3_hook(gb); return;
    case 0x7a0b: zeldaKidnappedState1__substate4_hook(gb); return;
    case 0x7a19: zeldaKidnappedState1__substate5_hook(gb); return;
    case 0x7a2c: zeldaKidnappedState1__substate6_hook(gb); return;
    case 0x7a37: zeldaKidnappedState1__substate7_hook(gb); return;
    case 0x7a45: zeldaKidnappedState1__substate8_hook(gb); return;
    case 0x7a54: zeldaKidnappedState1__substate9_hook(gb); return;
    case 0x7a6d: zeldaKidnappedState1__substateA_hook(gb); return;
    case 0x7a8b: zeldaKidnappedState1__substateB_hook(gb); return;
    case 0x7a9a: zeldaKidnappedState1__substateC_hook(gb); return;
    case 0x7aa3: zeldaKidnappedState1__substateD_hook(gb); return;
    case 0x7aac: zeldaKidnappedState1__substateE_hook(gb); return;
    case 0x7ab8: zeldaKidnappedState1__substateF_hook(gb); return;
    case 0x7aca: hook_handoff(gb, 0x7aca); return;
    case 0x7adf: zeldaKidnappedState1__substate11_hook(gb); return;
    case 0x7af6: zeldaKidnappedState1__substate12_hook(gb); return;
    case 0x7aff: zeldaKidnappedState1__substate13_hook(gb); return;
    case 0x7b08: zeldaKidnappedState1__substate14_hook(gb); return;
    case 0x7b1b: zeldaKidnappedState1__substate15_hook(gb); return;
    case 0x7b30: zeldaKidnappedState1__substate16_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void zeldaKidnappedState1__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x79b5, 0x79b8); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x79b8, 0x79b9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x79b9, 0x79ba); ret_effect(gb); return; }
  CYC(0x79b9, 0x79ba);
  CYC(0x79ba, 0x79bc); A = 0x01;
  CYC(0x79bc, 0x79bf); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(0x79bf, 0x79c2); SET_BC(0x0149);
  CALL_C(0x79c2, disableLcdAndLoadRoom_hook, 0x30b0, 0x79c5);
  CYC(0x79c5, 0x79c7); A = 0x02;
  CALL_C(0x79c7, loadGfxRegisterStateIndex_hook, 0x02ea, 0x79ca);
  CALL_C(0x79ca, restartSound_hook, 0x0cb2, 0x79cd);
  CALL_C(0x79cd, func_7c2a_hook, 0x7c2a, 0x79d0);
  CALL_C(0x79d0, fadeinFromWhite_hook, 0x3299, 0x79d3);
  CYC(0x79d3, 0x79d5); A = 0x3c;
  CYC(0x79d5, 0x79d8);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x79d8, func_7ba1_hook, 0x7ba1, 0x79db);
  if (!(F & FZ)) { CYCT(0x79db, 0x79dc); ret_effect(gb); return; }
  CYC(0x79db, 0x79dc);
  CYC(0x79dc, 0x79df); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x79df, 0x79e1); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x79e1, 0x79e3); A = 0x01;
  CYC(0x79e3, 0x79e6);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x79e6, 0x79e9); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x79e9, 0x79eb); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x79eb, 0x79ec); ret_effect(gb); return; }
  CYC(0x79eb, 0x79ec);
  CALL_C(0x79ec, func_03_7b95_hook, 0x7b95, 0x79ef);
  if (!(F & FZ)) { CYCT(0x79ef, 0x79f0); ret_effect(gb); return; }
  CYC(0x79ef, 0x79f0);
  CYC(0x79f0, 0x79f1); alu_xor(gb, A);
  CALL_C(0x79f1, func_7c68_hook, 0x7c68, 0x79f4);
  CYC(0x79f4, 0x79f6); A = 0x1e;
  CYC(0x79f6, 0x79f9);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x79f9, func_03_7b95_hook, 0x7b95, 0x79fc);
  if (!(F & FZ)) { CYCT(0x79fc, 0x79fd); ret_effect(gb); return; }
  CYC(0x79fc, 0x79fd);
  CYC(0x79fd, 0x79fe); alu_xor(gb, A);
  CALL_C(0x79fe, func_7c83_hook, 0x7c83, 0x7a01);
  CYC(0x7a01, 0x7a04); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7a04, 0x7a06); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  CYC(0x7a06, 0x7a08); A = 0x1e;
  CYC(0x7a08, 0x7a0b);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a0b, func_03_7b95_hook, 0x7b95, 0x7a0e);
  if (!(F & FZ)) { CYCT(0x7a0e, 0x7a0f); ret_effect(gb); return; }
  CYC(0x7a0e, 0x7a0f);
  CYC(0x7a0f, 0x7a11); A = 0x01;
  CALL_C(0x7a11, func_7c68_hook, 0x7c68, 0x7a14);
  CYC(0x7a14, 0x7a16); A = 0x1e;
  CYC(0x7a16, 0x7a19);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a19, func_03_7b95_hook, 0x7b95, 0x7a1c);
  if (!(F & FZ)) { CYCT(0x7a1c, 0x7a1d); ret_effect(gb); return; }
  CYC(0x7a1c, 0x7a1d);
  CYC(0x7a1d, 0x7a1f); A = 0x01;
  CALL_C(0x7a1f, func_7c83_hook, 0x7c83, 0x7a22);
  CYC(0x7a22, 0x7a25); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7a25, 0x7a27); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(0x7a27, 0x7a29); A = 0x1e;
  CYC(0x7a29, 0x7a2c);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate6_hook(GB *gb) {
  CYC(0x7a2c, 0x7a2f); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7a2f, 0x7a31); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7a31, 0x7a32); ret_effect(gb); return; }
  CYC(0x7a31, 0x7a32);
  CYC(0x7a32, 0x7a34); A = 0x1e;
  CYC(0x7a34, 0x7a37);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a37, func_03_7b95_hook, 0x7b95, 0x7a3a);
  if (!(F & FZ)) { CYCT(0x7a3a, 0x7a3b); ret_effect(gb); return; }
  CYC(0x7a3a, 0x7a3b);
  CYC(0x7a3b, 0x7a3e); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7a3e, 0x7a40); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(0x7a40, 0x7a42); A = 0x28;
  CYC(0x7a42, 0x7a45);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a45, func_03_7b95_hook, 0x7b95, 0x7a48);
  if (!(F & FZ)) { CYCT(0x7a48, 0x7a49); ret_effect(gb); return; }
  CYC(0x7a48, 0x7a49);
  CYC(0x7a49, 0x7a4b); C = 0x1f;
  CALL_C(0x7a4b, func_03_7b81_hook, 0x7b81, 0x7a4e);
  CYC(0x7a4e, 0x7a50); A = 0x5a;
  CYC(0x7a50, 0x7a53); mem_wr(gb, wTmpcbb5, A);
  CYC(0x7a53, 0x7a54); ret_effect(gb);
}

void zeldaKidnappedState1__substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a54, func_7b9a_hook, 0x7b9a, 0x7a57);
  if (F & FZ) {
    CYCT(0x7a57, 0x7a59);
    zeldaKidnappedState1__func_7a63_hook(gb);
    return;
  }
  CYC(0x7a57, 0x7a59);
  CYC(0x7a59, 0x7a5b); A = 0x3c;
  CYC(0x7a5b, 0x7a5c); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7a5c, 0x7a5d); ret_effect(gb); return; }
  CYC(0x7a5c, 0x7a5d);
  CYC(0x7a5d, 0x7a60); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7a60, 0x7a62); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(0x7a62, 0x7a63); ret_effect(gb);
}

void zeldaKidnappedState1__func_7a63_hook(GB *gb) {
  CYC(0x7a63, 0x7a66); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7a66, 0x7a68); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x7a68, 0x7a6a); A = 0x3c;
  CYC(0x7a6a, 0x7a6d);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a6d, func_03_7b95_hook, 0x7b95, 0x7a70);
  if (!(F & FZ)) { CYCT(0x7a70, 0x7a71); ret_effect(gb); return; }
  CYC(0x7a70, 0x7a71);
  CALL_C(0x7a71, func_7c1f_hook, 0x7c1f, 0x7a74);
  CALL_C(0x7a74, func_7beb_hook, 0x7beb, 0x7a77);
  CYC(0x7a77, 0x7a79); A = 0x21;
  CYC(0x7a79, 0x7a7c); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x7a7c, playSound_b00_hook, 0x0c98, 0x7a7f);
  CYC(0x7a7f, 0x7a80); alu_xor(gb, A);
  CYC(0x7a80, 0x7a83); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x7a83, 0x7a86); mem_wr(gb, wTmpcfc0_genericCutscene_cfc6, A);
  CYC(0x7a86, 0x7a88); A = 0x1e;
  CYC(0x7a88, 0x7a8b);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a8b, 0x7a8e); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x7a8e, 0x7a90); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x7a90, 0x7a91); ret_effect(gb); return; }
  CYC(0x7a90, 0x7a91);
  CALL_C(0x7a91, func_03_7b95_hook, 0x7b95, 0x7a94);
  if (!(F & FZ)) { CYCT(0x7a94, 0x7a95); ret_effect(gb); return; }
  CYC(0x7a94, 0x7a95);
  CYC(0x7a95, 0x7a97); C = 0x20;
  CYC(0x7a97, 0x7a9a);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a9a, func_7b9a_hook, 0x7b9a, 0x7a9d);
  if (!(F & FZ)) { CYCT(0x7a9d, 0x7a9e); ret_effect(gb); return; }
  CYC(0x7a9d, 0x7a9e);
  CYC(0x7a9e, 0x7aa0); C = 0x21;
  CYC(0x7aa0, 0x7aa3);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7aa3, func_7b9a_hook, 0x7b9a, 0x7aa6);
  if (!(F & FZ)) { CYCT(0x7aa6, 0x7aa7); ret_effect(gb); return; }
  CYC(0x7aa6, 0x7aa7);
  CYC(0x7aa7, 0x7aa9); C = 0x22;
  CYC(0x7aa9, 0x7aac);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7aac, func_7b9a_hook, 0x7b9a, 0x7aaf);
  if (!(F & FZ)) { CYCT(0x7aaf, 0x7ab0); ret_effect(gb); return; }
  CYC(0x7aaf, 0x7ab0);
  CYC(0x7ab0, 0x7ab3); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7ab3, 0x7ab5); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x01);
  CYC(0x7ab5, 0x7ab8);
  linkedCutscene_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7ab8, 0x7abb); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x7abb, 0x7abd); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x7abd, 0x7abe); ret_effect(gb); return; }
  CYC(0x7abd, 0x7abe);
  CYC(0x7abe, 0x7ac0); A = 0xd2;
  CALL_C(0x7ac0, playSound_b00_hook, 0x0c98, 0x7ac3);
  CYC(0x7ac3, 0x7ac4); alu_xor(gb, A);
  CYC(0x7ac4, 0x7ac7); mem_wr(gb, wTmpcbb4, A);
  CALL_C(0x7ac7, linkedCutscene_incSubstate_hook, 0x7b8b, 0x7aca);
  hook_handoff(gb, 0x7aca);
}

void zeldaKidnappedState1__substate11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7adf, func_03_7b95_hook, 0x7b95, 0x7ae2);
  if (!(F & FZ)) { CYCT(0x7ae2, 0x7ae3); ret_effect(gb); return; }
  CYC(0x7ae2, 0x7ae3);
  CALL_C(0x7ae3, func_7c1f_hook, 0x7c1f, 0x7ae6);
  CALL_C(0x7ae6, func_7bf6_hook, 0x7bf6, 0x7ae9);
  CALL_C(0x7ae9, func_7c2f_hook, 0x7c2f, 0x7aec);
  CYC(0x7aec, 0x7aee); A = 0x04;
  CALL_C(0x7aee, fadeinFromWhiteWithDelay_hook, 0x3284, 0x7af1);
  CYC(0x7af1, 0x7af3); A = 0x1e;
  CYC(0x7af3, 0x7af6);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7af6, func_7ba1_hook, 0x7ba1, 0x7af9);
  if (!(F & FZ)) { CYCT(0x7af9, 0x7afa); ret_effect(gb); return; }
  CYC(0x7af9, 0x7afa);
  CYC(0x7afa, 0x7afc); C = 0x23;
  CYC(0x7afc, 0x7aff);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substate13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7aff, func_7b9a_hook, 0x7b9a, 0x7b02);
  if (!(F & FZ)) { CYCT(0x7b02, 0x7b03); ret_effect(gb); return; }
  CYC(0x7b02, 0x7b03);
  CYC(0x7b03, 0x7b05); C = 0x24;
  CYC(0x7b05, 0x7b08);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substate14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b08, func_7b9a_hook, 0x7b9a, 0x7b0b);
  if (!(F & FZ)) { CYCT(0x7b0b, 0x7b0c); ret_effect(gb); return; }
  CYC(0x7b0b, 0x7b0c);
  CYC(0x7b0c, 0x7b0e); A = 0xbb;
  CALL_C(0x7b0e, playSound_b00_hook, 0x0c98, 0x7b11);
  CYC(0x7b11, 0x7b14); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7b14, 0x7b16); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7b16, 0x7b18); A = 0x5a;
  CYC(0x7b18, 0x7b1b);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b1b, func_03_7b95_hook, 0x7b95, 0x7b1e);
  if (!(F & FZ)) { CYCT(0x7b1e, 0x7b1f); ret_effect(gb); return; }
  CYC(0x7b1e, 0x7b1f);
  CYC(0x7b1f, 0x7b20); A = alu_dec8(gb, A);
  CYC(0x7b20, 0x7b23); mem_wr(gb, wTmpcbba, A);
  CYC(0x7b23, 0x7b25); A = 0xd2;
  CALL_C(0x7b25, playSound_b00_hook, 0x0c98, 0x7b28);
  CYC(0x7b28, 0x7b2a); A = 0xf0;
  CALL_C(0x7b2a, playSound_b00_hook, 0x0c98, 0x7b2d);
  CYC(0x7b2d, 0x7b30);
  linkedCutscene_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b30, 0x7b33); SET_HL(wTmpcbb5);
  CYC(0x7b33, 0x7b35); B = 0x02;
  CALL_C(0x7b35, flashScreen_hook, 0x2d73, 0x7b38);
  if (F & FZ) { CYCT(0x7b38, 0x7b39); ret_effect(gb); return; }
  CYC(0x7b38, 0x7b39);
  CYC(0x7b39, 0x7b3b); A = 0x3a;
  CALL_C(0x7b3b, setGlobalFlag_hook, 0x31f9, 0x7b3e);
  CYC(0x7b3e, 0x7b3f); alu_xor(gb, A);
  CYC(0x7b3f, 0x7b42); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7b42, 0x7b44); A = 0x20;
  CYC(0x7b44, 0x7b47); mem_wr(gb, wCutsceneTrigger, A);
  CYC(0x7b47, 0x7b48); ret_effect(gb);
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
