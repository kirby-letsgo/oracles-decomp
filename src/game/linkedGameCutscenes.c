#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(func_03_7841), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(func_03_7841), (from), (to), true)

void linkedCutscene_aIntoCBB5_incSubstate_hook(GB *gb);
void linkedCutscene_incSubstate_hook(GB *gb);
void func_7b48_hook(GB *gb);
void func_7b48__cbb4_00_hook(GB *gb);
void func_7b48__cbb4_01_hook(GB *gb);
void func_7b48__cbb4_03_hook(GB *gb);
void func_7b48__cbb4_04_hook(GB *gb);
void func_7b48__cbb4_05_hook(GB *gb);
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
  BASE(func_03_7841);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_cutscene_jump_table(gb));
    if (jt_ == SYM(func_03_7851)) { func_03_7851_hook(gb); return; }
    else if (jt_ == SYM(flameOfSorrowState1)) { flameOfSorrowState1_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7851_hook(GB *gb) {
  BASE(func_03_7851);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x10;
  CYC(b_+2, b_+5); SET_HL(wTmpcbb3);
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CALL_C(b_+8, clearWramBank1_hook, SYM(clearWramBank1), b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+15, b_+17); A = 0x80;
  CYC(b_+17, b_+20); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); mem_wr(gb, wCutsceneState, A);
  CYC(b_+25, SYM(flameOfSorrowState1)); ret_effect(gb);
}

void flameOfSorrowState1_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_cutscene_jump_table(gb));
    if (jt_ == b_+28) { flameOfSorrowState1__substate0_hook(gb); return; }
    else if (jt_ == b_+36) { flameOfSorrowState1__substate1_hook(gb); return; }
    else if (jt_ == b_+77) { flameOfSorrowState1__substate2_hook(gb); return; }
    else if (jt_ == b_+92) { flameOfSorrowState1__substate3_hook(gb); return; }
    else if (jt_ == b_+118) { flameOfSorrowState1__substate4_hook(gb); return; }
    else if (jt_ == b_+132) { flameOfSorrowState1__substate5_hook(gb); return; }
    else if (jt_ == b_+168) { flameOfSorrowState1__substate6_hook(gb); return; }
    else if (jt_ == b_+203) { flameOfSorrowState1__substate7_hook(gb); return; }
    else if (jt_ == b_+212) { flameOfSorrowState1__substate8_hook(gb); return; }
    else if (jt_ == b_+221) { flameOfSorrowState1__substate9_hook(gb); return; }
    else if (jt_ == b_+230) { flameOfSorrowState1__substateA_hook(gb); return; }
    else if (jt_ == b_+245) { flameOfSorrowState1__substateB_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void flameOfSorrowState1__substate0_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  CYC(b_+28, b_+30); A = 0x28;
  CYC(b_+30, b_+33); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+33, b_+36);
  linkedCutscene_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate1_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+36, func_03_7b95_hook, SYM(func_03_7b95), b_+39);
  if (!(F & FZ)) { CYCT(b_+39, b_+40); ret_effect(gb); return; }
  CYC(b_+39, b_+40);
  CALL_C(b_+40, func_7bab_hook, SYM(func_7bab), b_+43);
  CALL_C(b_+43, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+46);
  if (!(F & FZ)) CYCT(b_+46, b_+48);
  else {
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+50); mem_wr(gb, HL, 0xa9);
    CYC(b_+50, b_+51); L = alu_inc8(gb, L);
    CYC(b_+51, b_+53); mem_wr(gb, HL, 0x01);
  }
  CYC(b_+53, b_+55); A = 0x13;
  CALL_C(b_+55, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+58);
  CYC(b_+58, b_+60); A = 0xd2;
  CALL_C(b_+60, playSound_b00_hook, SYM(playSound_b00), b_+63);
  CYC(b_+63, b_+64); alu_xor(gb, A);
  CYC(b_+64, b_+67); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+67, b_+70); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+70, b_+71); A = alu_dec8(gb, A);
  CYC(b_+71, b_+74); mem_wr(gb, wTmpcbba, A);
  CALL_C(b_+74, linkedCutscene_incSubstate_hook, SYM(linkedCutscene_incSubstate), b_+77);
  flameOfSorrowState1__substate2_hook(gb);
}

void flameOfSorrowState1__substate2_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+77, b_+80); SET_HL(wTmpcbb5);
  CYC(b_+80, b_+82); B = 0x05;
  CALL_C(b_+82, flashScreen_hook, SYM(flashScreen), b_+85);
  if (F & FZ) { CYCT(b_+85, b_+86); ret_effect(gb); return; }
  CYC(b_+85, b_+86);
  CALL_C(b_+86, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+89);
  CYC(b_+89, b_+92);
  linkedCutscene_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate3_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+92, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+95);
  if (!(F & FZ)) CYCT(b_+95, b_+97);
  else {
    CYC(b_+95, b_+97);
    CYC(b_+97, b_+99); mem_wr(gb, HL, 0xa9);
  }
  CYC(b_+99, b_+101); A = 0xf0;
  CALL_C(b_+101, playSound_b00_hook, SYM(playSound_b00), b_+104);
  CALL_C(b_+104, clearFadingPalettes2_hook, SYM(clearFadingPalettes2), b_+107);
  CYC(b_+107, b_+109); A = 0xbf;
  CYC(b_+109, b_+111); hram_wr(gb, 0xa9, A);
  CYC(b_+111, b_+113); hram_wr(gb, 0xa7, A);
  CYC(b_+113, b_+115); A = 0x04;
  CYC(b_+115, b_+118);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate4_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+118, func_03_7b95_hook, SYM(func_03_7b95), b_+121);
  if (!(F & FZ)) { CYCT(b_+121, b_+122); ret_effect(gb); return; }
  CYC(b_+121, b_+122);
  CYC(b_+122, b_+124); A = 0x04;
  CYC(b_+124, b_+127); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+127, b_+129); C = 0x1b;
  CYC(b_+129, b_+132);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substate5_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+132, func_7b9a_hook, SYM(func_7b9a), b_+135);
  if (!(F & FZ)) { CYCT(b_+135, b_+136); ret_effect(gb); return; }
  CYC(b_+135, b_+136);
  CYC(b_+136, b_+138); B = 0x10;
  CALL_C(b_+138, flameOfSorrowState1__func_78fd_hook, b_+146, b_+141);
  CYC(b_+141, b_+143); A = 0x1e;
  CYC(b_+143, b_+146);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void flameOfSorrowState1__func_78fd_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+146, fastFadeinFromBlack_hook, SYM(fastFadeinFromBlack), b_+149);
  CYC(b_+149, b_+150); A = B;
  CYC(b_+150, b_+153); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+153, b_+156); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+156, b_+157); alu_xor(gb, A);
  CYC(b_+157, b_+160); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+160, b_+163); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+163, b_+165); A = 0x72;
  CYC(b_+165, b_+168);
  playSound_b00_hook(gb);
}

void flameOfSorrowState1__substate6_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+168, func_7ba1_hook, SYM(func_7ba1), b_+171);
  if (!(F & FZ)) { CYCT(b_+171, b_+172); ret_effect(gb); return; }
  CYC(b_+171, b_+172);
  CALL_C(b_+172, fadeinFromBlack_hook, SYM(fadeinFromBlack), b_+175);
  CYC(b_+175, b_+177); A = 0xaf;
  CYC(b_+177, b_+180); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+180, b_+183); mem_wr(gb, wFadeSprPaletteSources, A);
  CALL_C(b_+183, func_7bd0_hook, SYM(func_7bd0), b_+186);
  CYC(b_+186, b_+188); A = 0x21;
  CYC(b_+188, b_+191); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+191, playSound_b00_hook, SYM(playSound_b00), b_+194);
  CYC(b_+194, b_+195); alu_xor(gb, A);
  CYC(b_+195, b_+198); mem_wr(gb, wTmpcfc0_genericCutscene_cfc6, A);
  CYC(b_+198, b_+200); A = 0x1e;
  CYC(b_+200, b_+203);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void flameOfSorrowState1__substate7_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+203, func_7ba1_hook, SYM(func_7ba1), b_+206);
  if (!(F & FZ)) { CYCT(b_+206, b_+207); ret_effect(gb); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+209); C = 0x29;
  CYC(b_+209, b_+212);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substate8_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+212, func_7b9a_hook, SYM(func_7b9a), b_+215);
  if (!(F & FZ)) { CYCT(b_+215, b_+216); ret_effect(gb); return; }
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+218); C = 0x1c;
  CYC(b_+218, b_+221);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substate9_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+221, func_7b9a_hook, SYM(func_7b9a), b_+224);
  if (!(F & FZ)) { CYCT(b_+224, b_+225); ret_effect(gb); return; }
  CYC(b_+224, b_+225);
  CYC(b_+225, b_+227); C = 0x1d;
  CYC(b_+227, b_+230);
  func_03_7b81_hook(gb);
}

void flameOfSorrowState1__substateA_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+230, func_7b9a_hook, SYM(func_7b9a), b_+233);
  if (!(F & FZ)) { CYCT(b_+233, b_+234); ret_effect(gb); return; }
  CYC(b_+233, b_+234);
  CYC(b_+234, b_+236); C = 0x1e;
  CALL_C(b_+236, func_03_7b81_hook, SYM(func_03_7b81), b_+239);
  CYC(b_+239, b_+241); A = 0x3c;
  CYC(b_+241, b_+244); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+244, b_+245); ret_effect(gb);
}

void flameOfSorrowState1__substateB_hook(GB *gb) {
  BASE(flameOfSorrowState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+245, func_7b9a_hook, SYM(func_7b9a), b_+248);
  if (!(F & FZ)) { CYCT(b_+248, b_+249); ret_effect(gb); return; }
  CYC(b_+248, b_+249);
  CYC(b_+249, b_+250); alu_xor(gb, A);
  CYC(b_+250, b_+253); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+253, b_+256); SET_HL(b_+269);
  CALL_C(b_+256, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+259);
  CYC(b_+259, b_+261); A = 0x00;
  CYC(b_+261, b_+264); mem_wr(gb, wcc50, A);
  CYC(b_+264, b_+266); A = 0x0f;
  CYC(b_+266, b_+269);
  loadPaletteHeader_hook(gb);
}

void func_03_7849_hook(GB *gb) {
  BASE(func_03_7849);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_cutscene_jump_table(gb));
    if (jt_ == SYM(func_03_7851)) { func_03_7851_hook(gb); return; }
    else if (jt_ == SYM(zeldaKidnappedState1)) { zeldaKidnappedState1_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void zeldaKidnappedState1_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, zeldaKidnappedState1__runStates_hook, b_+6, b_+3);
  CYC(b_+3, b_+6);
  updateStatusBar_hook(gb);
}

void zeldaKidnappedState1__runStates_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (linked_cutscene_jump_table(gb));
    if (jt_ == b_+56) { zeldaKidnappedState1__substate0_hook(gb); return; }
    else if (jt_ == b_+91) { zeldaKidnappedState1__substate1_hook(gb); return; }
    else if (jt_ == b_+105) { zeldaKidnappedState1__substate2_hook(gb); return; }
    else if (jt_ == b_+124) { zeldaKidnappedState1__substate3_hook(gb); return; }
    else if (jt_ == b_+142) { zeldaKidnappedState1__substate4_hook(gb); return; }
    else if (jt_ == b_+156) { zeldaKidnappedState1__substate5_hook(gb); return; }
    else if (jt_ == b_+175) { zeldaKidnappedState1__substate6_hook(gb); return; }
    else if (jt_ == b_+186) { zeldaKidnappedState1__substate7_hook(gb); return; }
    else if (jt_ == b_+200) { zeldaKidnappedState1__substate8_hook(gb); return; }
    else if (jt_ == b_+215) { zeldaKidnappedState1__substate9_hook(gb); return; }
    else if (jt_ == b_+240) { zeldaKidnappedState1__substateA_hook(gb); return; }
    else if (jt_ == b_+270) { zeldaKidnappedState1__substateB_hook(gb); return; }
    else if (jt_ == b_+285) { zeldaKidnappedState1__substateC_hook(gb); return; }
    else if (jt_ == b_+294) { zeldaKidnappedState1__substateD_hook(gb); return; }
    else if (jt_ == b_+303) { zeldaKidnappedState1__substateE_hook(gb); return; }
    else if (jt_ == b_+315) { zeldaKidnappedState1__substateF_hook(gb); return; }
    else if (jt_ == b_+333) { hook_handoff(gb, b_+333); return; }
    else if (jt_ == b_+354) { zeldaKidnappedState1__substate11_hook(gb); return; }
    else if (jt_ == b_+377) { zeldaKidnappedState1__substate12_hook(gb); return; }
    else if (jt_ == b_+386) { zeldaKidnappedState1__substate13_hook(gb); return; }
    else if (jt_ == b_+395) { zeldaKidnappedState1__substate14_hook(gb); return; }
    else if (jt_ == b_+414) { zeldaKidnappedState1__substate15_hook(gb); return; }
    else if (jt_ == b_+435) { zeldaKidnappedState1__substate16_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void zeldaKidnappedState1__substate0_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+56, b_+59); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); A = 0x01;
  CYC(b_+63, b_+66); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+66, b_+69); SET_BC(0x0149);
  CALL_C(b_+69, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+72);
  CYC(b_+72, b_+74); A = 0x02;
  CALL_C(b_+74, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+77);
  CALL_C(b_+77, restartSound_hook, SYM(restartSound), b_+80);
  CALL_C(b_+80, func_7c2a_hook, SYM(func_7c2a), b_+83);
  CALL_C(b_+83, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+86);
  CYC(b_+86, b_+88); A = 0x3c;
  CYC(b_+88, b_+91);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate1_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+91, func_7ba1_hook, SYM(func_7ba1), b_+94);
  if (!(F & FZ)) { CYCT(b_+94, b_+95); ret_effect(gb); return; }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+98); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+98, b_+100); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+100, b_+102); A = 0x01;
  CYC(b_+102, b_+105);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate2_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+105, b_+108); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+108, b_+110); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+110, b_+111); ret_effect(gb); return; }
  CYC(b_+110, b_+111);
  CALL_C(b_+111, func_03_7b95_hook, SYM(func_03_7b95), b_+114);
  if (!(F & FZ)) { CYCT(b_+114, b_+115); ret_effect(gb); return; }
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+116); alu_xor(gb, A);
  CALL_C(b_+116, func_7c68_hook, SYM(func_7c68), b_+119);
  CYC(b_+119, b_+121); A = 0x1e;
  CYC(b_+121, b_+124);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate3_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+124, func_03_7b95_hook, SYM(func_03_7b95), b_+127);
  if (!(F & FZ)) { CYCT(b_+127, b_+128); ret_effect(gb); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+129); alu_xor(gb, A);
  CALL_C(b_+129, func_7c83_hook, SYM(func_7c83), b_+132);
  CYC(b_+132, b_+135); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+135, b_+137); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  CYC(b_+137, b_+139); A = 0x1e;
  CYC(b_+139, b_+142);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate4_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+142, func_03_7b95_hook, SYM(func_03_7b95), b_+145);
  if (!(F & FZ)) { CYCT(b_+145, b_+146); ret_effect(gb); return; }
  CYC(b_+145, b_+146);
  CYC(b_+146, b_+148); A = 0x01;
  CALL_C(b_+148, func_7c68_hook, SYM(func_7c68), b_+151);
  CYC(b_+151, b_+153); A = 0x1e;
  CYC(b_+153, b_+156);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate5_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+156, func_03_7b95_hook, SYM(func_03_7b95), b_+159);
  if (!(F & FZ)) { CYCT(b_+159, b_+160); ret_effect(gb); return; }
  CYC(b_+159, b_+160);
  CYC(b_+160, b_+162); A = 0x01;
  CALL_C(b_+162, func_7c83_hook, SYM(func_7c83), b_+165);
  CYC(b_+165, b_+168); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+168, b_+170); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(b_+170, b_+172); A = 0x1e;
  CYC(b_+172, b_+175);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate6_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  CYC(b_+175, b_+178); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+178, b_+180); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+180, b_+181); ret_effect(gb); return; }
  CYC(b_+180, b_+181);
  CYC(b_+181, b_+183); A = 0x1e;
  CYC(b_+183, b_+186);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate7_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+186, func_03_7b95_hook, SYM(func_03_7b95), b_+189);
  if (!(F & FZ)) { CYCT(b_+189, b_+190); ret_effect(gb); return; }
  CYC(b_+189, b_+190);
  CYC(b_+190, b_+193); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+193, b_+195); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+195, b_+197); A = 0x28;
  CYC(b_+197, b_+200);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate8_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+200, func_03_7b95_hook, SYM(func_03_7b95), b_+203);
  if (!(F & FZ)) { CYCT(b_+203, b_+204); ret_effect(gb); return; }
  CYC(b_+203, b_+204);
  CYC(b_+204, b_+206); C = 0x1f;
  CALL_C(b_+206, func_03_7b81_hook, SYM(func_03_7b81), b_+209);
  CYC(b_+209, b_+211); A = 0x5a;
  CYC(b_+211, b_+214); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+214, b_+215); ret_effect(gb);
}

void zeldaKidnappedState1__substate9_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+215, func_7b9a_hook, SYM(func_7b9a), b_+218);
  if (F & FZ) {
    CYCT(b_+218, b_+220);
    zeldaKidnappedState1__func_7a63_hook(gb);
    return;
  }
  CYC(b_+218, b_+220);
  CYC(b_+220, b_+222); A = 0x3c;
  CYC(b_+222, b_+223); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+223, b_+224); ret_effect(gb); return; }
  CYC(b_+223, b_+224);
  CYC(b_+224, b_+227); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+227, b_+229); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+229, b_+230); ret_effect(gb);
}

void zeldaKidnappedState1__func_7a63_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  CYC(b_+230, b_+233); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+233, b_+235); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+235, b_+237); A = 0x3c;
  CYC(b_+237, b_+240);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substateA_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+240, func_03_7b95_hook, SYM(func_03_7b95), b_+243);
  if (!(F & FZ)) { CYCT(b_+243, b_+244); ret_effect(gb); return; }
  CYC(b_+243, b_+244);
  CALL_C(b_+244, func_7c1f_hook, SYM(func_7c1f), b_+247);
  CALL_C(b_+247, func_7beb_hook, SYM(func_7beb), b_+250);
  CYC(b_+250, b_+252); A = 0x21;
  CYC(b_+252, b_+255); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+255, playSound_b00_hook, SYM(playSound_b00), b_+258);
  CYC(b_+258, b_+259); alu_xor(gb, A);
  CYC(b_+259, b_+262); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+262, b_+265); mem_wr(gb, wTmpcfc0_genericCutscene_cfc6, A);
  CYC(b_+265, b_+267); A = 0x1e;
  CYC(b_+267, b_+270);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substateB_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+270, b_+273); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+273, b_+275); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+275, b_+276); ret_effect(gb); return; }
  CYC(b_+275, b_+276);
  CALL_C(b_+276, func_03_7b95_hook, SYM(func_03_7b95), b_+279);
  if (!(F & FZ)) { CYCT(b_+279, b_+280); ret_effect(gb); return; }
  CYC(b_+279, b_+280);
  CYC(b_+280, b_+282); C = 0x20;
  CYC(b_+282, b_+285);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substateC_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+285, func_7b9a_hook, SYM(func_7b9a), b_+288);
  if (!(F & FZ)) { CYCT(b_+288, b_+289); ret_effect(gb); return; }
  CYC(b_+288, b_+289);
  CYC(b_+289, b_+291); C = 0x21;
  CYC(b_+291, b_+294);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substateD_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+294, func_7b9a_hook, SYM(func_7b9a), b_+297);
  if (!(F & FZ)) { CYCT(b_+297, b_+298); ret_effect(gb); return; }
  CYC(b_+297, b_+298);
  CYC(b_+298, b_+300); C = 0x22;
  CYC(b_+300, b_+303);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substateE_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+303, func_7b9a_hook, SYM(func_7b9a), b_+306);
  if (!(F & FZ)) { CYCT(b_+306, b_+307); ret_effect(gb); return; }
  CYC(b_+306, b_+307);
  CYC(b_+307, b_+310); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+310, b_+312); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x01);
  CYC(b_+312, b_+315);
  linkedCutscene_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substateF_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+315, b_+318); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+318, b_+320); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+320, b_+321); ret_effect(gb); return; }
  CYC(b_+320, b_+321);
  CYC(b_+321, b_+323); A = 0xd2;
  CALL_C(b_+323, playSound_b00_hook, SYM(playSound_b00), b_+326);
  CYC(b_+326, b_+327); alu_xor(gb, A);
  CYC(b_+327, b_+330); mem_wr(gb, wTmpcbb4, A);
  CALL_C(b_+330, linkedCutscene_incSubstate_hook, SYM(linkedCutscene_incSubstate), b_+333);
  hook_handoff(gb, b_+333);
}

void zeldaKidnappedState1__substate11_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+354, func_03_7b95_hook, SYM(func_03_7b95), b_+357);
  if (!(F & FZ)) { CYCT(b_+357, b_+358); ret_effect(gb); return; }
  CYC(b_+357, b_+358);
  CALL_C(b_+358, func_7c1f_hook, SYM(func_7c1f), b_+361);
  CALL_C(b_+361, func_7bf6_hook, SYM(func_7bf6), b_+364);
  CALL_C(b_+364, func_7c2f_hook, SYM(func_7c2f), b_+367);
  CYC(b_+367, b_+369); A = 0x04;
  CALL_C(b_+369, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+372);
  CYC(b_+372, b_+374); A = 0x1e;
  CYC(b_+374, b_+377);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate12_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+377, func_7ba1_hook, SYM(func_7ba1), b_+380);
  if (!(F & FZ)) { CYCT(b_+380, b_+381); ret_effect(gb); return; }
  CYC(b_+380, b_+381);
  CYC(b_+381, b_+383); C = 0x23;
  CYC(b_+383, b_+386);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substate13_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+386, func_7b9a_hook, SYM(func_7b9a), b_+389);
  if (!(F & FZ)) { CYCT(b_+389, b_+390); ret_effect(gb); return; }
  CYC(b_+389, b_+390);
  CYC(b_+390, b_+392); C = 0x24;
  CYC(b_+392, b_+395);
  func_03_7b81_hook(gb);
}

void zeldaKidnappedState1__substate14_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+395, func_7b9a_hook, SYM(func_7b9a), b_+398);
  if (!(F & FZ)) { CYCT(b_+398, b_+399); ret_effect(gb); return; }
  CYC(b_+398, b_+399);
  CYC(b_+399, b_+401); A = 0xbb;
  CALL_C(b_+401, playSound_b00_hook, SYM(playSound_b00), b_+404);
  CYC(b_+404, b_+407); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+407, b_+409); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+409, b_+411); A = 0x5a;
  CYC(b_+411, b_+414);
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate15_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+414, func_03_7b95_hook, SYM(func_03_7b95), b_+417);
  if (!(F & FZ)) { CYCT(b_+417, b_+418); ret_effect(gb); return; }
  CYC(b_+417, b_+418);
  CYC(b_+418, b_+419); A = alu_dec8(gb, A);
  CYC(b_+419, b_+422); mem_wr(gb, wTmpcbba, A);
  CYC(b_+422, b_+424); A = 0xd2;
  CALL_C(b_+424, playSound_b00_hook, SYM(playSound_b00), b_+427);
  CYC(b_+427, b_+429); A = 0xf0;
  CALL_C(b_+429, playSound_b00_hook, SYM(playSound_b00), b_+432);
  CYC(b_+432, b_+435);
  linkedCutscene_incSubstate_hook(gb);
}

void zeldaKidnappedState1__substate16_hook(GB *gb) {
  BASE(zeldaKidnappedState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+435, b_+438); SET_HL(wTmpcbb5);
  CYC(b_+438, b_+440); B = 0x02;
  CALL_C(b_+440, flashScreen_hook, SYM(flashScreen), b_+443);
  if (F & FZ) { CYCT(b_+443, b_+444); ret_effect(gb); return; }
  CYC(b_+443, b_+444);
  CYC(b_+444, b_+446); A = 0x3a;
  CALL_C(b_+446, setGlobalFlag_hook, SYM(setGlobalFlag), b_+449);
  CYC(b_+449, b_+450); alu_xor(gb, A);
  CYC(b_+450, b_+453); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+453, b_+455); A = 0x20;
  CYC(b_+455, b_+458); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+458, SYM(func_7b48)); ret_effect(gb);
}

static void linked_cutscene_clear_and_advance(GB *gb, uint16_t sp0_) {
  BASE(func_7b48);
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcbb5, A);
  CALL_C(b_+21, clearFadingPalettes_hook, SYM(clearFadingPalettes), b_+24);
  CYC(b_+24, b_+27);
  func_03_7b90_hook(gb);
}

static void linked_cutscene_white_and_advance(GB *gb, uint16_t sp0_) {
  BASE(func_7b48);
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbb5, A);
  CALL_C(b_+36, fastFadeoutToWhite_hook, SYM(fastFadeoutToWhite), b_+39);
  CYC(b_+39, b_+42);
  func_03_7b90_hook(gb);
}

void func_7b48_hook(GB *gb) {
  BASE(func_7b48);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linked_cutscene_jump_table(gb));
    if (jt_ == b_+16) { func_7b48__cbb4_00_hook(gb); return; }
    else if (jt_ == b_+27) { func_7b48__cbb4_01_hook(gb); return; }
    else if (jt_ == b_+42) { func_7b48__cbb4_03_hook(gb); return; }
    else if (jt_ == b_+46) { func_7b48__cbb4_04_hook(gb); return; }
    else if (jt_ == b_+54) { func_7b48__cbb4_05_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_7b48__cbb4_00_hook(GB *gb) {
  BASE(func_7b48);
  uint16_t sp0_ = gb->sp;
  CYC(b_+16, b_+18); A = 0x0a;
  linked_cutscene_clear_and_advance(gb, sp0_);
}

void func_7b48__cbb4_01_hook(GB *gb) {
  BASE(func_7b48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+27, func_03_7b95_hook, SYM(func_03_7b95), b_+30);
  if (!(F & FZ)) { CYCT(b_+30, b_+31); ret_effect(gb); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); A = 0x0a;
  linked_cutscene_white_and_advance(gb, sp0_);
}

void func_7b48__cbb4_03_hook(GB *gb) {
  BASE(func_7b48);
  uint16_t sp0_ = gb->sp;
  CYC(b_+42, b_+44); A = 0x14;
  CYC(b_+44, b_+46);
  linked_cutscene_clear_and_advance(gb, sp0_);
}

void func_7b48__cbb4_04_hook(GB *gb) {
  BASE(func_7b48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+46, func_03_7b95_hook, SYM(func_03_7b95), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+50); ret_effect(gb); return; }
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+52); A = 0x1e;
  CYC(b_+52, b_+54);
  linked_cutscene_white_and_advance(gb, sp0_);
}

void func_7b48__cbb4_05_hook(GB *gb) {
  BASE(func_7b48);
  CYC(b_+54, SYM(func_03_7b81));
  func_7ba1_hook(gb);
}

void func_03_7b81_hook(GB *gb) {
  BASE(func_03_7b81);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x28;
  CALL_C(b_+2, showText_hook, SYM(showText), b_+5);
  CYC(b_+5, SYM(linkedCutscene_aIntoCBB5_incSubstate)); A = 0x1e;
  linkedCutscene_aIntoCBB5_incSubstate_hook(gb);
}

void linkedCutscene_aIntoCBB5_incSubstate_hook(GB *gb) {
  BASE(linkedCutscene_aIntoCBB5_incSubstate);
  CYC(b_+0, SYM(linkedCutscene_incSubstate)); W8(wTmpcbb5) = A;
  linkedCutscene_incSubstate_hook(gb);
}

void linkedCutscene_incSubstate_hook(GB *gb) {
  BASE(linkedCutscene_incSubstate);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, SYM(func_03_7b90)); ret_effect(gb);
}

void func_03_7b90_hook(GB *gb) {
  BASE(func_03_7b90);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, SYM(func_03_7b95)); ret_effect(gb);
}

void func_03_7b95_hook(GB *gb) {
  BASE(func_03_7b95);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb5);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, SYM(func_7b9a)); ret_effect(gb);
}

void func_7b9a_hook(GB *gb) {
  BASE(func_7b9a);
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, SYM(func_7ba1));
  linkedCutscene_decrementTimer(gb);
}

void func_7ba1_hook(GB *gb) {
  BASE(func_7ba1);
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  linkedCutscene_decrementTimer(gb);
}

static void linkedCutscene_decrementTimer(GB *gb) {
  BASE(func_7ba1);
  CYC(b_+5, b_+8); SET_HL(wTmpcbb5);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+9, SYM(func_7bab)); ret_effect(gb);
}

void func_7bab_hook(GB *gb) {
  BASE(func_7bab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); SET_BC((SYM(loadUncompressedGfxHeader) + 23));
  CALL_C(b_+4, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+7);
  CYC(b_+7, b_+9); A = 0xac;
  CALL_C(b_+9, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+12);
  CYC(b_+12, b_+14); A = 0x28;
  CYC(b_+14, b_+17); W8(wGfxRegs1_SCX) = A;
  CYC(b_+17, b_+20); W8(wGfxRegs2_SCX) = A;
  CYC(b_+20, b_+22); hram_wr(gb, 0xac, A);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+25); hram_wr(gb, 0xaa, A);
  CYC(b_+25, b_+27); A = 0;
  CYC(b_+27, b_+30); W8(wScrollMode) = A;
  CYC(b_+30, b_+32); A = 0x10;
  CYC(b_+32, b_+34); hram_wr(gb, 0x9f, A);
  CYC(b_+34, SYM(func_7bd0)); clearWramBank1_hook(gb);
}

void func_7bd0_hook(GB *gb) {
  BASE(func_7bd0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_7be5));
  CALL_C(b_+3, func_7bd9_hook, SYM(func_7bd9), b_+6);
  CYC(b_+6, SYM(func_7bd9)); SET_BC(SYM(table_7be8));
  func_7bd9_hook(gb);
}

void func_7bd9_hook(GB *gb) {
  BASE(func_7bd9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xb0);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); SET_BC(BC + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, SYM(table_7be5)); func_7c09_hook(gb);
}

void func_7beb_hook(GB *gb) {
  BASE(func_7beb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_7c13));
  CALL_C(b_+3, func_7bff_hook, SYM(func_7bff), b_+6);
  CYC(b_+6, b_+9); SET_BC(SYM(table_7c16));
  CYC(b_+9, SYM(func_7bf6)); func_7bff_hook(gb);
}

void func_7bf6_hook(GB *gb) {
  BASE(func_7bf6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_7c19));
  CALL_C(b_+3, func_7bff_hook, SYM(func_7bff), b_+6);
  CYC(b_+6, SYM(func_7bff)); SET_BC(SYM(table_7c1c));
  func_7bff_hook(gb);
}

void func_7bff_hook(GB *gb) {
  BASE(func_7bff);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xbc);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); SET_BC(BC + 1);
  CYC(b_+9, SYM(func_7c09)); mem_wr(gb, HL, A);
  func_7c09_hook(gb);
}

void func_7c09_hook(GB *gb) {
  BASE(func_7c09);
  CYC(b_+0, b_+2); L = 0x4b;
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+4); SET_BC(BC + 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+7); L = 0x4d;
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, SYM(table_7c13)); ret_effect(gb);
}

void func_7c1f_hook(GB *gb) {
  BASE(func_7c1f);
  CYC(b_+0, b_+2); A = 1;
  CYC(b_+2, b_+5); W8(wLoadedTreeGfxIndex) = A;
  CYC(b_+5, b_+7); A = 0xbc;
  CYC(b_+7, b_+10); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(b_+10, SYM(func_7c2a)); ret_effect(gb);
}

void func_7c2a_hook(GB *gb) {
  BASE(func_7c2a);
  CYC(b_+0, b_+3); SET_BC(SYM(table_7c4e));
  CYC(b_+3, SYM(func_7c2f)); spawnZeldaKidnappedNPCs_hook(gb);
}

void func_7c2f_hook(GB *gb) {
  BASE(func_7c2f);
  CYC(b_+0, SYM(spawnZeldaKidnappedNPCs)); SET_BC(SYM(table_7c5d));
  spawnZeldaKidnappedNPCs_hook(gb);
}

void spawnZeldaKidnappedNPCs_hook(GB *gb) {
  BASE(spawnZeldaKidnappedNPCs);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, BC);
    CYC(b_+1, b_+2); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
    CYC(b_+2, b_+3);
    CALL_C(b_+3, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+6);
    if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; }
    CYC(b_+6, b_+7);
    CYC(b_+7, b_+8); A = mem_rd(gb, BC);
    CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+9, b_+10); SET_BC(BC + 1);
    CYC(b_+10, b_+11); A = mem_rd(gb, BC);
    CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+12, b_+13); SET_BC(BC + 1);
    CYC(b_+13, b_+14); A = mem_rd(gb, BC);
    CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+15, b_+16); SET_BC(BC + 1);
    CYC(b_+16, b_+18); L = 0x4b;
    CYC(b_+18, b_+19); A = mem_rd(gb, BC);
    CYC(b_+19, b_+20); mem_wr(gb, HL, A);
    CYC(b_+20, b_+21); SET_BC(BC + 1);
    CYC(b_+21, b_+23); L = 0x4d;
    CYC(b_+23, b_+24); A = mem_rd(gb, BC);
    CYC(b_+24, b_+25); mem_wr(gb, HL, A);
    CYC(b_+25, b_+26); SET_BC(BC + 1);
    CYCT(b_+26, SYM(table_7c4e));
  }
}

void func_7c68_hook(GB *gb) {
  BASE(func_7c68);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_7c7f));
  CALL_C(b_+3, addDoubleIndexToBc_hook, 0x007e, b_+6);
  CALL_C(b_+6, getFreePartSlot_hook, SYM(getFreePartSlot), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10); CYC(b_+10, b_+12); mem_wr(gb, HL, 0x27);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); L = 0xcb;
  CYC(b_+16, b_+17); A = mem_rd(gb, BC);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); SET_BC(BC + 1);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, SYM(table_7c7f)); ret_effect(gb);
}

void func_7c83_hook(GB *gb) {
  BASE(func_7c83);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_7c7f));
  CALL_C(b_+3, addDoubleIndexToBc_hook, 0x007e, b_+6);
  CALL_C(b_+6, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10); CYC(b_+10, b_+12); mem_wr(gb, HL, 0x6b);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x16);
  CYC(b_+15, b_+17); L = 0x46;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x78);
  CYC(b_+19, SYM(blackTowerEscapeAttempt_incState)); func_7c09_hook(gb);
}
