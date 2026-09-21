#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void nayruWarpToMakuTreeCutsceneHandler__func_6955_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__func_6962_hook(GB *gb);
void nayruSingingCutsceneHandler_hook(GB *gb);
void blackTowerExplanationCutsceneHandler_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state0_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state0__afterCall67c3_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state0__afterCall67cd_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state6_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state6__afterCall687a_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state8_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state8__afterCall68ae_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__state8__afterCall68de_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__stateC_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__func_699a_hook(GB *gb);
void nayruWarpToMakuTreeCutsceneHandler__func_699a__afterCall69af_hook(GB *gb);
void func_6f9e_hook(GB *gb);
void func_6fb0_hook(GB *gb);
void func_6f0b_hook(GB *gb);
void cutscene_disableLcdLoadRoomResetCamera_hook(GB *gb);
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
void nayruSingingCutsceneHandler__stateD_hook(GB *gb);
void nayruSingingStateF_hook(GB *gb);
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
void blackTowerExplanationCutsceneHandler__cbb8_00_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01__state5_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01__state6_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01__state6__afterCall66d4_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_01__state7_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_02_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_02__state1_hook(GB *gb);
void blackTowerExplanationCutsceneHandler__cbb8_02__state2_hook(GB *gb);
void func_6733_hook(GB *gb);
void func_6733__cbb8_00_hook(GB *gb);
void func_6733__cbb8_02_hook(GB *gb);
void blackTowerCompleteCutsceneHandler_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__afterCall69b8_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__runStates_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state4_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state4__afterCall6a6b_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state0_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state1_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state2_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state3_hook(GB *gb);
void blackTowerCompleteCutsceneHandler__state5_hook(GB *gb);
void turnToStoneCutsceneHandler_hook(GB *gb);
void turnToStoneCutsceneHandler__afterCall6a91_hook(GB *gb);
void turnToStoneCutsceneHandler__runStates_hook(GB *gb);
void turnToStoneCutsceneHandler__state1_hook(GB *gb);
void turnToStoneCutsceneHandler__state1__afterCall6ace_hook(GB *gb);
void turnToStoneCutsceneHandler__state3_hook(GB *gb);
void turnToStoneCutsceneHandler__state3__afterCall6afa_hook(GB *gb);
void turnToStoneCutsceneHandler__state5_hook(GB *gb);
void turnToStoneCutsceneHandler__state5__afterCall6b50_hook(GB *gb);
void twinrovaRevealCutsceneHandler_hook(GB *gb);
void twinrovaRevealCutsceneHandler__afterCall6b7a_hook(GB *gb);
void twinrovaRevealCutsceneHandler__runStates_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state4_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state4__afterCall6c2d_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state6_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state6__afterCall6c64_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state6__afterCall6c95_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state9_hook(GB *gb);
void twinrovaRevealCutsceneHandler__state9__afterCall6d05_hook(GB *gb);
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
void func_701d_hook(GB *gb);
void func_701d__state0_hook(GB *gb);
void func_701d__func_702f_hook(GB *gb);
void func_701d__state1_hook(GB *gb);
void func_701d__cbb3_00_hook(GB *gb);
void func_701d__cbb3_01_hook(GB *gb);
void wallRetraction_dungeon8_hook(GB *gb);
void wallRetraction_dungeon8__state0_hook(GB *gb);
void wallRetraction_dungeon8__state1_hook(GB *gb);
void wallRetraction_dungeon8__cbb3_01_hook(GB *gb);
void d2Collapse_incState_hook(GB *gb);
void d2Collapse_incCBB3_hook(GB *gb);
void func_7168_hook(GB *gb);
void func_7168__state0_hook(GB *gb);
void func_7168__state1_hook(GB *gb);
void func_7168__state2_hook(GB *gb);
void func_7168__cbb3_00_hook(GB *gb);
void func_7168__func_71d0_hook(GB *gb);
void func_7168__cbb3_01_hook(GB *gb);
void func_7168__cbb3_02_hook(GB *gb);
void func_7168__cbb3_03_hook(GB *gb);
void func_7168__state3_hook(GB *gb);
void func_7168__state4_hook(GB *gb);
void func_stub_hook(GB *gb);
void func_03_7244_hook(GB *gb);
void func_03_7244__state0_hook(GB *gb);
void func_03_7244__state0__afterCall727a_hook(GB *gb);
void func_03_7244__state1_hook(GB *gb);
void func_03_7244__state1__cbb3_00_hook(GB *gb);
void func_03_7244__state1__cbb3_01_hook(GB *gb);
void func_03_7244__state1__cbb3_02_hook(GB *gb);
void func_03_7244__state1__cbb3_03_hook(GB *gb);
void func_03_7244__state1__cbb3_04_hook(GB *gb);
void func_03_7244__state1__cbb3_05_hook(GB *gb);
void func_03_7244__state1__func_72ec_hook(GB *gb);
void func_03_7244__afterCall730c_hook(GB *gb);
void func_03_7244__state2_hook(GB *gb);
void func_03_7244__state2__cbb3_00_hook(GB *gb);
void func_03_7244__state2__cbb3_01_hook(GB *gb);
void func_03_7244__state2__cbb3_02_hook(GB *gb);
void func_03_7244__state2__cbb3_03_hook(GB *gb);
void func_03_7244__state2__cbb3_04_hook(GB *gb);
void func_03_7244__state3_hook(GB *gb);
void blackTowerEscapeAttempt_incState_hook(GB *gb);
void blackTowerEscapeAttempt_decCBB4_hook(GB *gb);
void blackTowerEscapeAttempt_loadNewRoom_hook(GB *gb);
void func_03_7493_hook(GB *gb);
void func_03_7493__state0_hook(GB *gb);
void func_03_7493__state1_hook(GB *gb);
void func_03_7565_hook(GB *gb);
void func_03_7565__state1_hook(GB *gb);
void func_03_7cb7_hook(GB *gb);
void func_03_7cb7__state0_hook(GB *gb);
void func_03_7cb7__afterCall7ce0_hook(GB *gb);
void func_03_7cb7__state1_hook(GB *gb);
void func_03_7cb7__func_7d33_hook(GB *gb);
void func_03_7cb7__state2_hook(GB *gb);
void func_03_7cb7__state3_hook(GB *gb);
void func_03_7cb7__state4_hook(GB *gb);
void func_03_7cb7__func_7db6_hook(GB *gb);
void func_03_7cb7__func_7dbc_hook(GB *gb);
void func_03_7cb7__state5_hook(GB *gb);
void func_03_7cb7__state6_hook(GB *gb);
void func_7e40_hook(GB *gb);

static void maku_tree_disappearing_body(GB *gb, uint16_t sp0_);
static void func_7168_cbb3_01_02_body(GB *gb, uint16_t sp0_);
static void timewarp_state1_cbb3_00_03_body(GB *gb, uint16_t sp0_);

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
  BASE(func_6e9a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+6); hram_wr(gb, 0x4f, A);
  CYC(b_+6, b_+9); SET_HL(0x9800);
  CYC(b_+9, b_+12); SET_BC(0x0400);
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8B);
  CALL_C(b_+14, fillMemoryBc_hook, SYM(fillMemoryBc), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+20); hram_wr(gb, 0x4f, A);
  CYC(b_+20, b_+23); SET_HL(0x9800);
  CYC(b_+23, b_+26); SET_BC(0x0400);
  CYC(b_+26, b_+29); clearMemoryBc_hook(gb);
}

void func_6eb7_hook(GB *gb) {
  BASE(func_6eb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0x70);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); hram_wr(gb, 0x70, A);
  CYC(b_+7, b_+10); SET_HL(w3VramTiles);
  CYC(b_+10, b_+13); SET_BC(0x0240);
  CALL_C(b_+13, clearMemoryBc_hook, SYM(clearMemoryBc), b_+16);
  CYC(b_+16, b_+19); SET_HL(w3VramAttributes);
  CYC(b_+19, b_+22); SET_BC(0x0240);
  CYC(b_+22, b_+24); A = 0x02;
  CALL_C(b_+24, fillMemoryBc_hook, SYM(fillMemoryBc), b_+27);
  CYC(b_+27, b_+28); SET_AF(pop_effect(gb));
  CYC(b_+28, b_+30); hram_wr(gb, 0x70, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void func_6ed6_hook(GB *gb) {
  BASE(func_6ed6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CYC(b_+2, b_+4); A = hram_rd(gb, 0x70);
  CYC(b_+4, b_+5); push_effect(gb, AF);
  CYC(b_+5, b_+7); A = 0x04;
  CYC(b_+7, b_+9); hram_wr(gb, 0x70, A);
  CYC(b_+9, b_+12); SET_HL(w1Link);
  CYC(b_+12, b_+15); SET_BC(0x0240);
  CALL_C(b_+15, clearMemoryBc_hook, SYM(clearMemoryBc), b_+18);
  CYC(b_+18, b_+21); SET_HL(w1ParentItem4);
  CYC(b_+21, b_+24); SET_BC(0x0240);
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF8B);
  CALL_C(b_+26, fillMemoryBc_hook, SYM(fillMemoryBc), b_+29);
  CYC(b_+29, b_+30); SET_AF(pop_effect(gb));
  CYC(b_+30, b_+32); hram_wr(gb, 0x70, A);
  CYC(b_+32, b_+33); ret_effect(gb);
}

void func_6ef7_hook(GB *gb) {
  BASE(func_6ef7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb9);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    TAIL(func_6f0b);
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_HL(wTmpcbb7);
  CYC(b_+9, b_+11); B = 0x01;
  CALL_C(b_+11, flashScreen_hook, SYM(flashScreen), b_+14);
  if (F & FZ) {
    CYCT(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void func_6f0b_hook(GB *gb) {
  BASE(func_6f0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, getRandomNumber_hook, SYM(getRandomNumber), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); mem_wr(gb, wTmpcbb7, A);
  CYC(b_+15, b_+16); A = alu_dec8(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+19, b_+22); mem_wr(gb, wTmpcbba, A);
  CYC(b_+22, b_+24); A = 0xd2;
  CYC(b_+24, b_+27); playSound_b00_hook(gb);
}

void func_6f26_hook(GB *gb) {
  BASE(func_6f26);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcbb6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, decCbb3_hook, SYM(decCbb3), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wTmpcbbb);
  CYC(b_+12, b_+15); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+15, b_+18); SET_HL(wGfxRegs1_SCY);
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+22); ret_effect(gb);
    return;
  }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); A = 0x34;
  CALL_C(b_+24, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+27);
  CYC(b_+27, b_+29); alu_or(gb, 0x01);
  CYC(b_+29, b_+30); ret_effect(gb);
}

void func_6f44_hook(GB *gb) {
  BASE(func_6f44);
  CYC(b_+0, b_+3); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hOamTail, A);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+13); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+20) { func_6f44__cbb8_00_hook(gb); return; }
    else if (jt_ == b_+28) { func_6f44__cbb8_01_hook(gb); return; }
    else if (jt_ == b_+44) { func_6f44__cbb8_02_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_6f44__cbb8_00_hook(GB *gb) {
  BASE(func_6f44);
  CYC(b_+20, b_+23); SET_HL((SYM(func_712f) + 29));
  CYC(b_+23, b_+25); E = 0x3f;
  CYC(b_+25, b_+28); addSpritesFromBankToOam_withOffset_hook(gb);
}

void func_6f44__cbb8_01_hook(GB *gb) {
  BASE(func_6f44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+28, b_+31); SET_HL((SYM(func_7168__state0) + 23));
  CYC(b_+31, b_+33); E = 0x3f;
  CALL_C(b_+33, addSpritesFromBankToOam_withOffset_hook, SYM(addSpritesFromBankToOam_withOffset), b_+36);
  CYC(b_+36, b_+39); SET_HL((SYM(func_7168__cbb3_00) + 9));
  CYC(b_+39, b_+41); E = 0x3f;
  CYC(b_+41, b_+44); addSpritesFromBankToOam_withOffset_hook(gb);
}

void func_6f44__cbb8_02_hook(GB *gb) {
  BASE(func_6f44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+44, b_+47); SET_HL((SYM(func_7168__cbb3_03) + 3));
  CYC(b_+47, b_+49); E = 0x3f;
  CALL_C(b_+49, addSpritesFromBankToOam_withOffset_hook, SYM(addSpritesFromBankToOam_withOffset), b_+52);
  CYC(b_+52, b_+55); SET_HL((SYM(func_7168__state0) + 23));
  CYC(b_+55, b_+57); E = 0x3f;
  CYC(b_+57, b_+60); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+60, b_+62); alu_cp(gb, 0x71);
  if (F & FC) {
    CYCT(b_+62, b_+64);
  } else {
    CYC(b_+62, b_+64);
    CYC(b_+64, b_+67); SET_HL((SYM(func_7168__state4) + 14));
    CYC(b_+67, b_+69); E = 0x3f;
  }
  CYC(b_+69, b_+72); addSpritesFromBankToOam_withOffset_hook(gb);
}

void cutscene_incCutsceneState_hook(GB *gb) {
  BASE(cutscene_incCutsceneState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void cutscene_decCBB6_hook(GB *gb) {
  BASE(cutscene_decCBB6);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb6);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void cutscene_tickDownCBB4ThenSetTo30_hook(GB *gb) {
  BASE(cutscene_tickDownCBB4ThenSetTo30);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x1e);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void cutscene_incState_hook(GB *gb) {
  BASE(cutscene_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void cutscene_incCBB3_hook(GB *gb) {
  BASE(cutscene_incCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void d2Collapse_decCBB4_hook(GB *gb) {
  BASE(d2Collapse_decCBB4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void timewarpCutscene_decCBB4_hook(GB *gb) {
  BASE(timewarpCutscene_decCBB4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void fairyCutscene_incState_hook(GB *gb) {
  BASE(fairyCutscene_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void func_03_6103_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CYC(b_+3, b_+5); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+5, b_+8);
    TAIL(fairyCutscene_cfd1is07);
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wCutsceneState);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+36) { func_03_6103__state0_hook(gb); return; }
    else if (jt_ == b_+57) { func_03_6103__state1_hook(gb); return; }
    else if (jt_ == b_+114) { func_03_6103__state2_hook(gb); return; }
    else if (jt_ == b_+133) { func_03_6103__state3_hook(gb); return; }
    else if (jt_ == b_+147) { func_03_6103__state4_hook(gb); return; }
    else if (jt_ == b_+156) { func_03_6103__state5_hook(gb); return; }
    else if (jt_ == b_+165) { func_03_6103__state7_hook(gb); return; }
    else if (jt_ == b_+174) { func_03_6103__state8_hook(gb); return; }
    else if (jt_ == b_+184) { func_03_6103__stateA_hook(gb); return; }
    else if (jt_ == b_+217) { func_03_6103__stateB_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_6103__state0_hook(GB *gb) {
  BASE(func_03_6103);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+36, b_+39); SET_HL(wTmpcbb3);
  CYC(b_+39, b_+42); A = mem_rd(gb, w1Link_yh);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+43, b_+46); A = mem_rd(gb, w1Link_xh);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+47, b_+50); A = mem_rd(gb, w1Link_direction);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  CALL_C(b_+51, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+54);
  CYC(b_+54, b_+57); fairyCutscene_incState_hook(gb);
}

void func_03_6103__state1_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+57, b_+60); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+61, b_+62); ret_effect(gb); return; }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); A = 0x81;
  TAIL(func_03_6103__loadNewFairyRoom);
}

void func_03_6103__loadNewFairyRoom_hook(GB *gb) {
  BASE(func_03_6103);
  uint16_t sp0_ = gb->sp;
  CYC(b_+64, b_+67); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+67, fairyCutscene_incState_hook, SYM(fairyCutscene_incState), b_+70);
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+74); mem_wr(gb, wTmpcfc0_fairyHideAndSeek_cfd2, A);
  CALL_C(b_+74, disableLcd_hook, SYM(disableLcd), b_+77);
  CALL_C(b_+77, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+80);
  CALL_C(b_+80, initializeVramMaps_hook, SYM(initializeVramMaps), b_+83);
  CALL_C(b_+83, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+86);
  CALL_C(b_+86, loadTilesetData_hook, SYM(loadTilesetData), b_+89);
  CALL_C(b_+89, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+92);
  CALL_C(b_+92, func_131f_hook, SYM(func_131f), b_+95);
  CYC(b_+95, b_+97); A = 0x01;
  CYC(b_+97, b_+100); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+100, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+103);
  func_03_6103__afterCall616a_hook(gb);
}

void func_03_6103__afterCall616a_hook(GB *gb) {
  BASE(func_03_6103);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+103, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+106);
  CYC(b_+106, b_+108); A = 0x02;
  CALL_C(b_+108, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+111);
  CYC(b_+111, b_+114); fadeinFromWhite_hook(gb);
}

void func_03_6103__state2_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+114, b_+117); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+117, b_+118); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+118, b_+119); ret_effect(gb);
    return;
  }
  CYC(b_+118, b_+119);
  CYC(b_+119, b_+121); B = 0x0c;
  TAIL(func_03_6103__spawnForestFairy);
}

void func_03_6103__spawnForestFairy_hook(GB *gb) {
  BASE(func_03_6103);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+121, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+124);
  if (!(F & FZ)) {
    CYCT(b_+124, b_+125); ret_effect(gb);
    return;
  }
  CYC(b_+124, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x49);
  CYC(b_+127, b_+129); L = 0x43;
  CYC(b_+129, b_+130); mem_wr(gb, HL, B);
  CYC(b_+130, b_+133); fairyCutscene_incState_hook(gb);
}

void func_03_6103__state3_hook(GB *gb) {
  BASE(func_03_6103);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+133, b_+136); SET_HL(wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+136, b_+137); A = mem_rd(gb, HL);
  CYC(b_+137, b_+138); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+138, b_+139); ret_effect(gb);
    return;
  }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x00);
  CALL_C(b_+141, fairyCutscene_incState_hook, SYM(fairyCutscene_incState), b_+144);
  CYC(b_+144, b_+147); fadeoutToWhite_hook(gb);
}

void func_03_6103__state4_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+147, b_+150); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+150, b_+151); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+151, b_+152); ret_effect(gb); return; }
  CYC(b_+151, b_+152);
  CYC(b_+152, b_+154); A = 0x80;
  CYC(b_+154, b_+156); func_03_6103__loadNewFairyRoom_hook(gb);
}

void func_03_6103__state5_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+156, b_+159); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+159, b_+160); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+160, b_+161); ret_effect(gb);
    return;
  }
  CYC(b_+160, b_+161);
  CYC(b_+161, b_+163); B = 0x0d;
  CYC(b_+163, b_+165); func_03_6103__spawnForestFairy_hook(gb);
}

void func_03_6103__state7_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+165, b_+168); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+168, b_+169); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+169, b_+170); ret_effect(gb); return; }
  CYC(b_+169, b_+170);
  CYC(b_+170, b_+172); A = 0x91;
  CYC(b_+172, b_+174); func_03_6103__loadNewFairyRoom_hook(gb);
}

void func_03_6103__state8_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+174, b_+177); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+177, b_+178); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+178, b_+179); ret_effect(gb);
    return;
  }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+181); B = 0x0e;
  CYC(b_+181, b_+184); func_03_6103__spawnForestFairy_hook(gb);
}

void func_03_6103__stateA_hook(GB *gb) {
  BASE(func_03_6103);
  uint16_t sp0_ = gb->sp;
  CYC(b_+184, b_+187); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+187, b_+188); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+188, b_+189); ret_effect(gb); return; }
  CYC(b_+188, b_+189);
  CYC(b_+189, b_+191); A = 0x82;
  CALL_C(b_+191, func_03_6103__loadNewFairyRoom_hook, b_+64, b_+194);
  func_03_6103__afterCall61c5_hook(gb);
}

void func_03_6103__afterCall61c5_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+194, b_+197); SET_HL(w1Link_enabled);
  CYC(b_+197, b_+199); mem_wr(gb, HL, 0x03);
  CYC(b_+199, b_+201); L = w1Link_yh & 0xff;
  CYC(b_+201, b_+204); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+204, b_+205); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+205, b_+206); L = alu_inc8(gb, L);
  CYC(b_+206, b_+209); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+209, b_+210); mem_wr(gb, HL, A);
  CYC(b_+210, b_+213); A = mem_rd(gb, wTmpcbb5);
  CYC(b_+213, b_+215); L = w1Link_direction & 0xff;
  CYC(b_+215, b_+216); mem_wr(gb, HL, A);
  CYC(b_+216, b_+217); ret_effect(gb);
}

void func_03_6103__stateB_hook(GB *gb) {
  BASE(func_03_6103);
  CYC(b_+217, b_+220); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+220, b_+221); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+221, b_+222); ret_effect(gb);
    return;
  }
  CYC(b_+221, b_+222);
  CYC(b_+222, b_+223); alu_xor(gb, A);
  CYC(b_+223, b_+226); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+226, b_+229); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+229, b_+230); A = alu_inc8(gb, A);
  CYC(b_+230, b_+233); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+233, b_+236); SET_BC(0x1104);
  CYC(b_+236, b_+239); showText_hook(gb);
}

void fairyCutscene_cfd1is07_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+16) { fairyCutscene_cfd1is07__state0_hook(gb); return; }
    else if (jt_ == b_+30) { fairyCutscene_cfd1is07__state1_hook(gb); return; }
    else if (jt_ == b_+51) { fairyCutscene_cfd1is07__state2_hook(gb); return; }
    else if (jt_ == b_+63) { fairyCutscene_cfd1is07__state3_hook(gb); return; }
    else if (jt_ == b_+90) { fairyCutscene_cfd1is07__state4_hook(gb); return; }
    else if (jt_ == b_+104) { fairyCutscene_cfd1is07__state5_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void fairyCutscene_cfd1is07__state0_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+16, b_+19); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+21); ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+24); SET_BC(0x110a);
  CALL_C(b_+24, showText_hook, SYM(showText), b_+27);
  CYC(b_+27, b_+30); fairyCutscene_incState_hook(gb);
}

void fairyCutscene_cfd1is07__state1_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+30, b_+33); A = mem_rd(gb, wTextIsActive);
  CYC(b_+33, b_+34); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+34, b_+35); ret_effect(gb);
    return;
  }
  CYC(b_+34, b_+35);
  CALL_C(b_+35, fairyCutscene_incState_hook, SYM(fairyCutscene_incState), b_+38);
  CYC(b_+38, b_+40); A = 0x0c;
  CYC(b_+40, b_+43); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+43, b_+45); A = 0x7b;
  CALL_C(b_+45, playSound_b00_hook, SYM(playSound_b00), b_+48);
  CYC(b_+48, b_+51); fastFadeinFromWhite_hook(gb);
}

void fairyCutscene_cfd1is07__state2_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  CYC(b_+51, b_+54); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+55, b_+56); ret_effect(gb);
    return;
  }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+59); SET_HL(wTmpcbb6);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+60, b_+61); ret_effect(gb);
    return;
  }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); fairyCutscene_cfd1is07__state1_hook(gb);
}

void fairyCutscene_cfd1is07__state3_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+63, b_+66); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+67, b_+68); ret_effect(gb);
    return;
  }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+71); SET_HL(wTmpcbb6);
  CYC(b_+71, b_+72); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+72, b_+73); ret_effect(gb);
    return;
  }
  CYC(b_+72, b_+73);
  CALL_C(b_+73, fairyCutscene_incState_hook, SYM(fairyCutscene_incState), b_+76);
  CYC(b_+76, b_+77); alu_xor(gb, A);
  CYC(b_+77, b_+80); mem_wr(gb, wTmpcfc0_fairyHideAndSeek_active, A);
  CYC(b_+80, b_+82); A = 0x7b;
  CALL_C(b_+82, playSound_b00_hook, SYM(playSound_b00), b_+85);
  CYC(b_+85, b_+87); A = 0x08;
  CYC(b_+87, b_+90); fadeinFromWhiteWithDelay_hook(gb);
}

void fairyCutscene_cfd1is07__state4_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+90, b_+93); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+93, b_+94); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+94, b_+95); ret_effect(gb);
    return;
  }
  CYC(b_+94, b_+95);
  CALL_C(b_+95, fairyCutscene_incState_hook, SYM(fairyCutscene_incState), b_+98);
  CYC(b_+98, b_+101); SET_BC(0x110b);
  CYC(b_+101, b_+104); showText_hook(gb);
}

void fairyCutscene_cfd1is07__state5_hook(GB *gb) {
  BASE(fairyCutscene_cfd1is07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+104, b_+106); A = 0x0e;
  CALL_C(b_+106, setGlobalFlag_hook, SYM(setGlobalFlag), b_+109);
  CYC(b_+109, b_+111); A = 0x2b;
  CALL_C(b_+111, setGlobalFlag_hook, SYM(setGlobalFlag), b_+114);
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CYC(b_+115, b_+118); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+118, b_+121); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+121, b_+122); A = alu_inc8(gb, A);
  CYC(b_+122, b_+125); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+125, b_+126); ret_effect(gb);
}

void func_03_6275__bootedFromPalace_incState_hook(GB *gb) {
  BASE(func_03_6275);
  CYC(b_+83, b_+86); SET_HL(wCutsceneState);
  CYC(b_+86, b_+87); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+87, b_+88); ret_effect(gb);
}

void func_03_6275_hook(GB *gb) {
  BASE(func_03_6275);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+14) { func_03_6275__state0_hook(gb); return; }
    else if (jt_ == b_+19) { func_03_6275__state1_hook(gb); return; }
    else if (jt_ == b_+88) { func_03_6275__state2_hook(gb); return; }
    else if (jt_ == b_+100) { func_03_6275__state3_hook(gb); return; }
    else if (jt_ == b_+119) { func_03_6275__state4_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_6275__state0_hook(GB *gb) {
  BASE(func_03_6275);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+14, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+17);
  CYC(b_+17, b_+19); func_03_6275__bootedFromPalace_incState_hook(gb);
}

void func_03_6275__state1_hook(GB *gb) {
  BASE(func_03_6275);
  uint16_t sp0_ = gb->sp;
  CYC(b_+19, b_+22); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; }
  CYC(b_+23, b_+24);
  CALL_C(b_+24, clearAllParentItems_hook, SYM(clearAllParentItems), b_+27);
  CALL_C(b_+27, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+30);
  CYC(b_+30, b_+32); A = 0x01;
  CYC(b_+32, b_+35); mem_wr(gb, wActiveGroup, A);
  CYC(b_+35, b_+37); A = 0x46;
  CYC(b_+37, b_+40); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+40, disableLcd_hook, SYM(disableLcd), b_+43);
  CALL_C(b_+43, clearOam_hook, SYM(clearOam), b_+46);
  CALL_C(b_+46, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+49);
  CALL_C(b_+49, initializeVramMaps_hook, SYM(initializeVramMaps), b_+52);
  CALL_C(b_+52, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+55);
  CALL_C(b_+55, loadTilesetData_hook, SYM(loadTilesetData), b_+58);
  CALL_C(b_+58, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+61);
  CALL_C(b_+61, func_131f_hook, SYM(func_131f), b_+64);
  CYC(b_+64, b_+66); A = 0x01;
  CYC(b_+66, b_+69); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+69, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+72);
  CALL_C(b_+72, initializeRoom_hook, SYM(initializeRoom), b_+75);
  func_03_6275__afterCall62c0_hook(gb);
}

void func_03_6275__afterCall62c0_hook(GB *gb) {
  BASE(func_03_6275);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+75, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+78);
  CYC(b_+78, b_+80); A = 0x02;
  CALL_C(b_+80, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+83);
  TAIL(func_03_6275__bootedFromPalace_incState);
}

void func_03_6275__state2_hook(GB *gb) {
  BASE(func_03_6275);
  CYC(b_+88, b_+90); A = 0x03;
  CYC(b_+90, b_+93); mem_wr(gb, w1Link_enabled, A);
  CYC(b_+93, b_+95); A = 0x0f;
  CYC(b_+95, b_+98); mem_wr(gb, wLinkForceState, A);
  CYC(b_+98, b_+100); func_03_6275__bootedFromPalace_incState_hook(gb);
}

void func_03_6275__state3_hook(GB *gb) {
  BASE(func_03_6275);
  uint16_t sp0_ = gb->sp;
  CYC(b_+100, b_+103); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+103, b_+104); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+104, b_+105); ret_effect(gb); return; }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+108); A = mem_rd(gb, w1Link_substate);
  CYC(b_+108, b_+110); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+110, b_+111); ret_effect(gb); return; }
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+114); SET_BC((SYM(endgameCutsceneHandler_20__state3) + 36));
  CALL_C(b_+114, showText_hook, SYM(showText), b_+117);
  CYC(b_+117, b_+119); func_03_6275__bootedFromPalace_incState_hook(gb);
}

void func_03_6275__state4_hook(GB *gb) {
  BASE(func_03_6275);
  CYC(b_+119, b_+122); A = mem_rd(gb, wTextIsActive);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+123, b_+124); ret_effect(gb); return; }
  CYC(b_+123, b_+124);
  CYC(b_+124, b_+127); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+127, b_+130); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+130, b_+133); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+133, b_+136); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+136, b_+139); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+139, b_+142); mem_wr(gb, wActiveMusic, A);
  CYC(b_+142, b_+145); playSound_b00_hook(gb);
}

void cutscene_loadRoomObjectSetAndFadein_hook(GB *gb) {
  BASE(cutscene_loadRoomObjectSetAndFadein);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); push_effect(gb, AF);
  CALL_C(b_+5, cutscene_disableLcdLoadRoomResetCamera_hook, SYM(cutscene_disableLcdLoadRoomResetCamera), b_+8);
  CYC(b_+8, b_+9); SET_AF(pop_effect(gb));
  CYC(b_+9, b_+10); B = A;
  CALL_C(b_+10, getEntryFromObjectTable2_hook, SYM(getEntryFromObjectTable2), b_+13);
  CALL_C(b_+13, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+16);
  CALL_C(b_+16, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+19);
  cutscene_loadRoomObjectSetAndFadein__afterCall64d8_hook(gb);
}

void cutscene_loadRoomObjectSetAndFadein__afterCall64d8_hook(GB *gb) {
  BASE(cutscene_loadRoomObjectSetAndFadein);
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+23); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(b_+23, b_+26); fadeinFromWhite_hook(gb);
}

void nayruSingingCutsceneHandler__func_6397_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+127, clearOam_hook, SYM(clearOam), b_+130);
  CYC(b_+130, b_+132); B = 0x00;
  CYC(b_+132, b_+135); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(b_+135, b_+136); alu_cpl(gb);
  CYC(b_+136, b_+137); A = alu_inc8(gb, A);
  CYC(b_+137, b_+138); C = A;
  CYC(b_+138, b_+141); SET_HL((SYM(func_03_7244) + 5));
  CYC(b_+141, b_+143); E = 0x3f;
  CYC(b_+143, b_+146);
  TAIL(addSpritesFromBankToOam_withOffset);
}

void nayruSingingCutsceneHandler__func_63db_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  CYC(b_+195, b_+198); A = mem_rd(gb, wFrameCounter);
  CYC(b_+198, b_+200); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+200, b_+201); ret_effect(gb);
    return;
  }
  CYC(b_+200, b_+201);
  CYC(b_+201, b_+204); SET_HL(wTmpcbb6);
  CYC(b_+204, b_+205); A = mem_rd(gb, HL);
  CYC(b_+205, b_+206); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+206, b_+207); ret_effect(gb);
    return;
  }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+208); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+208, b_+211); SET_HL(wGfxRegs1_SCX);
  CYC(b_+211, b_+212); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+212, b_+213); ret_effect(gb);
}

void makuTreeDisappearingCutsceneHandler_hook(GB *gb) {
  BASE(makuTreeDisappearingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  maku_tree_disappearing_body(gb, sp0_);
  CYC(b_+3, b_+6);
  TAIL(updateAllObjects);
}

static void maku_tree_disappearing_body(GB *gb, uint16_t sp0_) {
  BASE(makuTreeDisappearingCutsceneHandler);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    TAIL(makuTreeDisappearingCutsceneHandler__label_03_119);
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0xb4;
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+21); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+21, b_+24); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+24, b_+26); A = 0x0c;
  CALL_C(b_+26, setGlobalFlag_hook, SYM(setGlobalFlag), b_+29);
  CALL_C(b_+29, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+38); mem_wr(gb, wUseSimulatedInput, A);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+42); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+42, b_+45); SET_HL(b_+70);
  CYC(b_+45, b_+48);
  setWarpDestVariables_hook(gb);
}

void makuTreeDisappearingCutsceneHandler__label_03_119_hook(GB *gb) {
  BASE(makuTreeDisappearingCutsceneHandler);
  CYC(b_+48, b_+51); A = mem_rd(gb, wFrameCounter);
  CYC(b_+51, b_+53); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+53, b_+54); ret_effect(gb);
    return;
  }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+57); SET_HL(wTmpcbb7);
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+59); A = alu_inc8(gb, A);
  CYC(b_+59, b_+61); alu_and(gb, 0x03);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A);
  CYC(b_+62, b_+65); SET_HL(b_+75);
  CYC(b_+65, b_+66); push_effect(gb, b_+66);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(b_+66, b_+67); A = mem_rd(gb, HL);
  CYC(b_+67, b_+70);
  TAIL(loadPaletteHeader);
}

void blackTowerExplanationCutsceneHandler__afterCall65b6_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  CYC(b_+3, b_+6);
  TAIL(updateAllObjects);
}

void blackTowerExplanationCutsceneHandler__runStates_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+16) { blackTowerExplanationCutsceneHandler__cbb8_00_hook(gb); return; }
    else if (jt_ == b_+177) { blackTowerExplanationCutsceneHandler__cbb8_01_hook(gb); return; }
    else if (jt_ == b_+317) { blackTowerExplanationCutsceneHandler__cbb8_02_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void blackTowerExplanationCutsceneHandler_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, blackTowerExplanationCutsceneHandler__runStates_hook, b_+6, b_+3);
  blackTowerExplanationCutsceneHandler__afterCall65b6_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_00_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  CYC(b_+16, b_+19); SET_DE(wCutsceneState);
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+29) { blackTowerExplanationCutsceneHandler__cbb8_00__state0_hook(gb); return; }
    else if (jt_ == b_+120) { blackTowerExplanationCutsceneHandler__cbb8_00__state1_hook(gb); return; }
    else if (jt_ == b_+158) { blackTowerExplanationCutsceneHandler__cbb8_00__state2_hook(gb); return; }
    else if (jt_ == SYM(func_6733) && hook_enabled_at(gb, SYM(func_6733))) { func_6733_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void blackTowerExplanationCutsceneHandler__cbb8_01_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  CYC(b_+177, b_+180); SET_DE(wCutsceneState);
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  CYC(b_+181, b_+182); push_effect(gb, b_+182);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+29) { blackTowerExplanationCutsceneHandler__cbb8_00__state0_hook(gb); return; }
    else if (jt_ == b_+120) { blackTowerExplanationCutsceneHandler__cbb8_00__state1_hook(gb); return; }
    else if (jt_ == b_+158) { blackTowerExplanationCutsceneHandler__cbb8_00__state2_hook(gb); return; }
    else if (jt_ == b_+330) { blackTowerExplanationCutsceneHandler__cbb8_02__state1_hook(gb); return; }
    else if (jt_ == b_+370) { blackTowerExplanationCutsceneHandler__cbb8_02__state2_hook(gb); return; }
    else if (jt_ == b_+198) { blackTowerExplanationCutsceneHandler__cbb8_01__state5_hook(gb); return; }
    else if (jt_ == b_+214) { blackTowerExplanationCutsceneHandler__cbb8_01__state6_hook(gb); return; }
    else if (jt_ == b_+297) { blackTowerExplanationCutsceneHandler__cbb8_01__state7_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void blackTowerExplanationCutsceneHandler__cbb8_01__state6_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+214, func_6f44_hook, SYM(func_6f44), b_+217);
  CYC(b_+217, b_+220); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+220, b_+221); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+221, b_+222); ret_effect(gb); return; }
  CYC(b_+221, b_+222);
  CALL_C(b_+222, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+225);
  CALL_C(b_+225, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+228);
  CYC(b_+228, b_+231); SET_BC(0x00ba);
  CALL_C(b_+231, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+234);
  CALL_C(b_+234, resetCamera_hook, SYM(resetCamera), b_+237);
  CALL_C(b_+237, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+240);
  if (!(F & FZ)) {
    CYCT(b_+240, b_+242);
  } else {
    CYC(b_+240, b_+242);
    CYC(b_+242, b_+244); mem_wr(gb, HL, 0x8a);
    CYC(b_+244, b_+245); L = alu_inc8(gb, L);
    CYC(b_+245, b_+247); mem_wr(gb, HL, 0x00);
    CYC(b_+247, b_+248); L = alu_inc8(gb, L);
    CYC(b_+248, b_+250); mem_wr(gb, HL, 0x04);
  }
  CYC(b_+250, b_+253); SET_HL(w1Link_enabled);
  CYC(b_+253, b_+255); mem_wr(gb, HL, 0x03);
  CYC(b_+255, b_+257); L = (uint8_t)w1Link_yh;
  CYC(b_+257, b_+259); mem_wr(gb, HL, 0x65);
  CYC(b_+259, b_+261); L = (uint8_t)w1Link_xh;
  CYC(b_+261, b_+263); mem_wr(gb, HL, 0x58);
  CYC(b_+263, b_+265); L = (uint8_t)w1Link_direction;
  CYC(b_+265, b_+267); mem_wr(gb, HL, 0x02);
  CYC(b_+267, b_+270); A = mem_rd(gb, wLoadingRoomPack);
  CYC(b_+270, b_+273); mem_wr(gb, wRoomPack, A);
  CYC(b_+273, b_+275); A = 0xf0;
  CALL_C(b_+275, playSound_b00_hook, SYM(playSound_b00), b_+278);
  CYC(b_+278, b_+280); A = 0x0f;
  CALL_C(b_+280, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+283);
  CALL_C(b_+283, fadeinFromWhiteToRoom_hook, SYM(fadeinFromWhiteToRoom), b_+286);
  CALL_C(b_+286, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+289);
  blackTowerExplanationCutsceneHandler__cbb8_01__state6__afterCall66d4_hook(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_01__state6__afterCall66d4_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+289, showStatusBar_hook, SYM(showStatusBar), b_+292);
  CYC(b_+292, b_+294); A = 0x02;
  CYC(b_+294, b_+297);
  TAIL(loadGfxRegisterStateIndex);
}

void blackTowerExplanationCutsceneHandler__cbb8_02_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  CYC(b_+317, b_+320); SET_DE(wCutsceneState);
  CYC(b_+320, b_+321); A = mem_rd(gb, DE);
  CYC(b_+321, b_+322); push_effect(gb, b_+322);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+29) { blackTowerExplanationCutsceneHandler__cbb8_00__state0_hook(gb); return; }
    else if (jt_ == b_+330) { blackTowerExplanationCutsceneHandler__cbb8_02__state1_hook(gb); return; }
    else if (jt_ == b_+370) { blackTowerExplanationCutsceneHandler__cbb8_02__state2_hook(gb); return; }
    else if (jt_ == SYM(func_6733) && hook_enabled_at(gb, SYM(func_6733))) { func_6733_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void blackTowerExplanationCutsceneHandler__cbb8_00__state0_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+29, b_+32); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+33, b_+34); ret_effect(gb); return; }
  CYC(b_+33, b_+34);
  CALL_C(b_+34, disableLcd_hook, SYM(disableLcd), b_+37);
  CALL_C(b_+37, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+40);
  CALL_C(b_+40, clearOam_hook, SYM(clearOam), b_+43);
  CYC(b_+43, b_+46); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+46, b_+49); SET_HL(b_+114);
  CYC(b_+49, b_+50); add_double_index_to_hl(gb, b_+50);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+51, b_+52); push_effect(gb, HL);
  CALL_C(b_+52, loadGfxHeader_hook, SYM(loadGfxHeader), b_+55);
  CYC(b_+55, b_+56); SET_HL(pop_effect(gb));
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CALL_C(b_+57, loadGfxHeader_hook, SYM(loadGfxHeader), b_+60);
  CYC(b_+60, b_+62); A = 0xc3;
  CALL_C(b_+62, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+65);
  CYC(b_+65, b_+67); B = 0x78;
  CYC(b_+67, b_+70); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+70, b_+72); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+72, b_+74);
  } else {
    CYC(b_+72, b_+74);
    CYC(b_+74, b_+76); B = 0x3c;
  }
  CYC(b_+76, b_+79); SET_HL(wTmpcbb3);
  CYC(b_+79, b_+80); mem_wr(gb, HL, B);
  CYC(b_+80, b_+81); alu_or(gb, A);
  CYC(b_+81, b_+83); A = 0x21;
  if (F & FZ) CALL_C_CC(b_+83, playSound_b00_hook, SYM(playSound_b00), b_+86);
  else CYC(b_+83, b_+86);
  CALL_C(b_+86, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+89);
  CYC(b_+89, b_+90); alu_xor(gb, A);
  CYC(b_+90, b_+93); mem_wr(gb, wTmpcbb9, A);
  CALL_C(b_+93, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+96);
  CYC(b_+96, b_+98); A = 0x70;
  CYC(b_+98, b_+101); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+101, b_+104); SET_HL((wLoadedObjectGfx + 8));
  CYC(b_+104, b_+106); B = 0x08;
  CALL_C(b_+106, clearMemory_hook, SYM(clearMemory), b_+109);
  CYC(b_+109, b_+111); A = 0x09;
  CYC(b_+111, b_+114);
  TAIL(loadGfxRegisterStateIndex);
}

void blackTowerExplanationCutsceneHandler__cbb8_00__state1_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+120, func_6ef7_hook, SYM(func_6ef7), b_+123);
  CALL_C(b_+123, func_6f44_hook, SYM(func_6f44), b_+126);
  CYC(b_+126, b_+129); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+129, b_+130); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+130, b_+131); ret_effect(gb); return; }
  CYC(b_+130, b_+131);
  CALL_C(b_+131, decCbb3_hook, SYM(decCbb3), b_+134);
  if (!(F & FZ)) { CYCT(b_+134, b_+135); ret_effect(gb); return; }
  CYC(b_+134, b_+135);
  CALL_C(b_+135, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+138);
  CYC(b_+138, b_+141); SET_BC(0x1005);
  CYC(b_+141, b_+144); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+144, b_+145); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+145, b_+147);
  } else {
    CYC(b_+145, b_+147);
    CYC(b_+147, b_+150); SET_BC(0x1317);
  }
  CYC(b_+150, b_+152); A = 0x01;
  CYC(b_+152, b_+155); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+155, b_+158);
  TAIL(showText);
}

void blackTowerExplanationCutsceneHandler__cbb8_00__state2_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+158, func_6ef7_hook, SYM(func_6ef7), b_+161);
  CALL_C(b_+161, func_6f44_hook, SYM(func_6f44), b_+164);
  CYC(b_+164, b_+167); A = mem_rd(gb, wTextIsActive);
  CYC(b_+167, b_+168); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+168, b_+169); ret_effect(gb); return; }
  CYC(b_+168, b_+169);
  CYC(b_+169, b_+172); SET_HL(wTmpcbb3);
  CYC(b_+172, b_+174); mem_wr(gb, HL, 0x3c);
  CYC(b_+174, b_+177);
  TAIL(cutscene_incCutsceneState);
}

void blackTowerExplanationCutsceneHandler__cbb8_01__state5_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+198, func_6ef7_hook, SYM(func_6ef7), b_+201);
  CALL_C(b_+201, func_6f44_hook, SYM(func_6f44), b_+204);
  CALL_C(b_+204, decCbb3_hook, SYM(decCbb3), b_+207);
  if (!(F & FZ)) { CYCT(b_+207, b_+208); ret_effect(gb); return; }
  CYC(b_+207, b_+208);
  CALL_C(b_+208, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+211);
  CYC(b_+211, b_+214);
  TAIL(fadeoutToWhite);
}

void blackTowerExplanationCutsceneHandler__cbb8_01__state7_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+297, updateStatusBar_hook, SYM(updateStatusBar), b_+300);
  CYC(b_+300, b_+303); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+303, b_+304); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+304, b_+305); ret_effect(gb); return; }
  CYC(b_+304, b_+305);
  CYC(b_+305, b_+307); A = 0x01;
  CYC(b_+307, b_+310); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+310, b_+313); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+313, b_+316); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+316, b_+317); ret_effect(gb);
}

void blackTowerExplanationCutsceneHandler__cbb8_02__state1_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+330, func_6ef7_hook, SYM(func_6ef7), b_+333);
  CALL_C(b_+333, func_6f44_hook, SYM(func_6f44), b_+336);
  CYC(b_+336, b_+339); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+339, b_+340); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+340, b_+341); ret_effect(gb); return; }
  CYC(b_+340, b_+341);
  CALL_C(b_+341, decCbb3_hook, SYM(decCbb3), b_+344);
  if (!(F & FZ)) { CYCT(b_+344, b_+345); ret_effect(gb); return; }
  CYC(b_+344, b_+345);
  CYC(b_+345, b_+347); A = 0x04;
  CYC(b_+347, b_+350); mem_wr(gb, wTmpcbbb, A);
  CYC(b_+350, b_+353); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+353, b_+356); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+356, b_+359); SET_HL(b_+367);
  CYC(b_+359, b_+360); add_a_to_hl(gb, b_+360);
  CYC(b_+360, b_+361); A = mem_rd(gb, HL);
  CYC(b_+361, b_+364); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+364, b_+367);
  TAIL(cutscene_incCutsceneState);
}

void blackTowerExplanationCutsceneHandler__cbb8_02__state2_hook(GB *gb) {
  BASE(blackTowerExplanationCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+370, func_6ef7_hook, SYM(func_6ef7), b_+373);
  CALL_C(b_+373, func_6f26_hook, SYM(func_6f26), b_+376);
  if (!(F & FZ)) {
    CYCT(b_+376, b_+379);
    TAIL(func_6f44);
  }
  CYC(b_+376, b_+379);
  CYC(b_+379, b_+381); mem_wr(gb, HL, 0x78);
  CALL_C(b_+381, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), SYM(func_6733));
  TAIL(func_6733);
}

void func_6733_hook(GB *gb) {
  BASE(func_6733);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_6ef7_hook, SYM(func_6ef7), b_+3);
  CALL_C(b_+3, func_6f44_hook, SYM(func_6f44), b_+6);
  CALL_C(b_+6, decCbb3_hook, SYM(decCbb3), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+20) { func_6733__cbb8_00_hook(gb); return; }
    else if (jt_ == b_+53) { func_6733__cbb8_02_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_6733__cbb8_00_hook(GB *gb) {
  BASE(func_6733);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+20, b_+23); SET_HL(b_+63);
  CALL_C(b_+23, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+26);
  CYC(b_+26, b_+29); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd3);
  CYC(b_+29, b_+32); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+32, b_+35); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd4);
  CYC(b_+35, b_+38); mem_wr(gb, wcc50, A);
  CYC(b_+38, b_+40); A = 0xff;
  CYC(b_+40, b_+43); mem_wr(gb, wActiveMusic, A);
  CYC(b_+43, b_+45); A = 0x01;
  CYC(b_+45, b_+48); mem_wr(gb, wRoomLayoutEnd, A);
  CYC(b_+48, b_+50); A = 0xfb;
  CYC(b_+50, b_+53);
  TAIL(playSound_b00);
}

void func_6733__cbb8_02_hook(GB *gb) {
  BASE(func_6733);
  CYC(b_+53, b_+54); alu_xor(gb, A);
  CYC(b_+54, b_+57); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+57, b_+60); SET_HL(b_+68);
  CYC(b_+60, b_+63);
  TAIL(setWarpDestVariables);
}

void nayruWarpToMakuTreeCutsceneHandler__afterCall677f_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+3, updateStatusBar_hook, SYM(updateStatusBar), b_+6);
  CYC(b_+6, b_+9);
  TAIL(updateAllObjects);
}

void nayruWarpToMakuTreeCutsceneHandler__runStates_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  CYC(b_+9, b_+12); SET_DE(wCutsceneState);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+40) { nayruWarpToMakuTreeCutsceneHandler__state0_hook(gb); return; }
    else if (jt_ == b_+128) { nayruWarpToMakuTreeCutsceneHandler__state1_hook(gb); return; }
    else if (jt_ == b_+145) { nayruWarpToMakuTreeCutsceneHandler__state2_hook(gb); return; }
    else if (jt_ == b_+158) { nayruWarpToMakuTreeCutsceneHandler__state3_hook(gb); return; }
    else if (jt_ == b_+210) { nayruWarpToMakuTreeCutsceneHandler__state4_hook(gb); return; }
    else if (jt_ == b_+217) { nayruWarpToMakuTreeCutsceneHandler__state5_hook(gb); return; }
    else if (jt_ == b_+243) { nayruWarpToMakuTreeCutsceneHandler__state6_hook(gb); return; }
    else if (jt_ == b_+259) { nayruWarpToMakuTreeCutsceneHandler__state7_hook(gb); return; }
    else if (jt_ == b_+295) { nayruWarpToMakuTreeCutsceneHandler__state8_hook(gb); return; }
    else if (jt_ == b_+364) { nayruWarpToMakuTreeCutsceneHandler__state9_hook(gb); return; }
    else if (jt_ == b_+391) { nayruWarpToMakuTreeCutsceneHandler__stateA_hook(gb); return; }
    else if (jt_ == b_+414) { nayruWarpToMakuTreeCutsceneHandler__stateB_hook(gb); return; }
    else if (jt_ == b_+495) { nayruWarpToMakuTreeCutsceneHandler__stateC_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void nayruWarpToMakuTreeCutsceneHandler_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruWarpToMakuTreeCutsceneHandler__runStates_hook, b_+9, b_+3);
  nayruWarpToMakuTreeCutsceneHandler__afterCall677f_hook(gb);
}

void nayru_warp_state0_after_reload_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+81, b_+83); A = 0xfa;
  CALL_C(b_+83, playSound_b00_hook, SYM(playSound_b00), b_+86);
  CYC(b_+86, b_+88); A = 0x02;
  CALL_C(b_+88, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+91);
  CYC(b_+91, b_+94); SET_HL(wTmpcbb3);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x3c);
  CYC(b_+96, b_+98); B = 0x20;
  CYC(b_+98, b_+101); SET_HL(wTmpcfc0_genericCutscene_state);
  CALL_C(b_+101, clearMemory_hook, SYM(clearMemory), b_+104);
  CYC(b_+104, b_+106); A = 0x01;
  CYC(b_+106, b_+109); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+109, b_+112); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+112, b_+114); A = 0xf0;
  CALL_C(b_+114, playSound_b00_hook, SYM(playSound_b00), b_+117);
  CYC(b_+117, b_+119); A = 0x1e;
  CALL_C(b_+119, playSound_b00_hook, SYM(playSound_b00), b_+122);
  CALL_C(b_+122, incMakuTreeState_hook, SYM(incMakuTreeState), b_+125);
  CYC(b_+125, b_+128);
  TAIL(fadeinFromWhiteToRoom);
}

void nayru_warp_state0_after_refresh_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+71, b_+73); A = 0x04;
  CYC(b_+73, b_+75); B = 0x02;
  CALL_C(b_+75, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+78);
  CALL_C(b_+78, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+81);
  nayru_warp_state0_after_reload_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state0__afterCall67c3_hook(GB *gb) {
  nayru_warp_state0_after_refresh_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state0__afterCall67cd_hook(GB *gb) {
  nayru_warp_state0_after_reload_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state0_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+40, b_+43); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
  CYC(b_+44, b_+45);
  CALL_C(b_+45, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+48);
  CALL_C(b_+48, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+51);
  CYC(b_+51, b_+54); SET_BC(0x0038);
  CALL_C(b_+54, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+57);
  CALL_C(b_+57, resetCamera_hook, SYM(resetCamera), b_+60);
  CYC(b_+60, b_+62); B = 0x04;
  CALL_C(b_+62, getEntryFromObjectTable2_hook, SYM(getEntryFromObjectTable2), b_+65);
  CALL_C(b_+65, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+68);
  CALL_C(b_+68, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+71);
  nayru_warp_state0_after_refresh_hook(gb);
}

void nayru_warp_state6_after_room_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  CYC(b_+254, b_+256); A = 0x02;
  CYC(b_+256, b_+259);
  TAIL(loadGfxRegisterStateIndex);
}

void nayruWarpToMakuTreeCutsceneHandler__state6__afterCall687a_hook(GB *gb) {
  nayru_warp_state6_after_room_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state6_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+243, b_+246); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+246, b_+247); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+247, b_+248); ret_effect(gb); return; }
  CYC(b_+247, b_+248);
  CALL_C(b_+248, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+251);
  CALL_C(b_+251, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+254);
  nayru_warp_state6_after_room_hook(gb);
}

void nayru_warp_state8_after_reload_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+354, b_+356); A = 0x1e;
  CALL_C(b_+356, playSound_b00_hook, SYM(playSound_b00), b_+359);
  CYC(b_+359, b_+361); A = 0x02;
  CYC(b_+361, b_+364);
  TAIL(loadGfxRegisterStateIndex);
}

void nayru_warp_state8_after_room_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+306, nayruWarpToMakuTreeCutsceneHandler__func_6838_hook, b_+188, b_+309);
  CYC(b_+309, b_+311); A = 0x01;
  CYC(b_+311, b_+314); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+314, b_+317); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+317, b_+319); A = 0x04;
  CYC(b_+319, b_+321); B = 0x02;
  CALL_C(b_+321, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+324);
  CYC(b_+324, b_+326); A = 0x26;
  CYC(b_+326, b_+328); B = 0x02;
  CALL_C(b_+328, cutscene_loadAintoHL_BTimes_hook, SYM(cutscene_loadAintoHL_BTimes), b_+331);
  CYC(b_+331, b_+333); A = 0x24;
  CYC(b_+333, b_+335); B = 0x01;
  CALL_C(b_+335, cutscene_loadAintoHL_BTimes_hook, SYM(cutscene_loadAintoHL_BTimes), b_+338);
  CYC(b_+338, b_+339); B = L;
  CALL_C(b_+339, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+342);
  if (F & FZ) {
    CYCT(b_+342, b_+344);
  } else {
    CYC(b_+342, b_+344);
    CALL_C(b_+344, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+347);
    if (!(F & FZ)) {
      CYCT(b_+347, b_+349);
    } else {
      CYC(b_+347, b_+349);
      CYC(b_+349, b_+351); mem_wr(gb, HL, 0x93);
    }
  }
  CALL_C(b_+351, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+354);
  nayru_warp_state8_after_reload_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state8__afterCall68ae_hook(GB *gb) {
  nayru_warp_state8_after_room_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state8__afterCall68de_hook(GB *gb) {
  nayru_warp_state8_after_reload_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state8_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+295, b_+298); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+298, b_+299); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+299, b_+300); ret_effect(gb); return; }
  CYC(b_+299, b_+300);
  CALL_C(b_+300, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+303);
  CALL_C(b_+303, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+306);
  nayru_warp_state8_after_room_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__incCutsceneState_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  CYC(b_+457, b_+460);
  TAIL(cutscene_incCutsceneState);
}

void nayruWarpToMakuTreeCutsceneHandler__stateC_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+495, b_+498); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+498, b_+500); alu_cp(gb, 0x63);
  if (F & FZ) {
    CYCT(b_+500, b_+502);
    TAIL(nayruWarpToMakuTreeCutsceneHandler__func_699a);
  }
  CYC(b_+500, b_+502);
  CALL_C(b_+502, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+505);
  if (F & FZ) { CYCT(b_+505, b_+506); ret_effect(gb); return; }
  CYC(b_+505, b_+506);
  CYC(b_+506, b_+509); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+509, b_+511); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+511, b_+512); ret_effect(gb); return; }
  CYC(b_+511, b_+512);
  CYC(b_+512, b_+515); A = mem_rd(gb, wFrameCounter);
  CYC(b_+515, b_+517); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+517, b_+518); ret_effect(gb); return; }
  CYC(b_+517, b_+518);
  CYC(b_+518, b_+521); SET_HL((SYM(flameOfSorrowState1) + 12));
  CYC(b_+521, b_+523); E = 0x0a;
  CALL_C(b_+523, interBankCall_hook, 0x008a, b_+526);
  CYC(b_+526, b_+529); SET_DE(w1Link_yh);
  CALL_C(b_+529, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+532);
  CALL_C(b_+532, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+535);
  CYC(b_+535, b_+536); H = D;
  CYC(b_+536, b_+538); L = (uint8_t)w1Link_direction;
  CYC(b_+538, b_+539); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+539, b_+540); ret_effect(gb); return; }
  CYC(b_+539, b_+540);
  CYC(b_+540, b_+542);
  TAIL(nayruWarpToMakuTreeCutsceneHandler__func_6962);
}

void nayruWarpToMakuTreeCutsceneHandler__func_699a__afterCall69af_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  CYC(b_+563, b_+565); A = 0x01;
  CYC(b_+565, b_+568); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CYC(b_+568, b_+569); ret_effect(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_699a_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+542, b_+543); alu_xor(gb, A);
  CYC(b_+543, b_+546); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+546, b_+549); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+549, b_+552); A = mem_rd(gb, wLoadingRoomPack);
  CYC(b_+552, b_+555); mem_wr(gb, wRoomPack, A);
  CYC(b_+555, b_+557); A = 0x11;
  CALL_C(b_+557, setGlobalFlag_hook, SYM(setGlobalFlag), b_+560);
  CALL_C(b_+560, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+563);
  nayruWarpToMakuTreeCutsceneHandler__func_699a__afterCall69af_hook(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__state1_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+128, cutscene_decCBB3whenFadeDone_hook, SYM(cutscene_decCBB3whenFadeDone), b_+131);
  if (!(F & FZ)) { CYCT(b_+131, b_+132); ret_effect(gb); return; }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+134, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+137);
  CYC(b_+137, b_+139); A = 0x68;
  CYC(b_+139, b_+142); SET_BC((SYM(introCinematic_ridingHorse_state9) + 5));
  CYC(b_+142, b_+145);
  TAIL(createEnergySwirlGoingIn);
}

void nayruWarpToMakuTreeCutsceneHandler__state2_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+145, decCbb3_hook, SYM(decCbb3), b_+148);
  if (!(F & FZ)) { CYCT(b_+148, b_+149); ret_effect(gb); return; }
  CYC(b_+148, b_+149);
  CYC(b_+149, b_+150); alu_xor(gb, A);
  CYC(b_+150, b_+151); mem_wr(gb, HL, A);
  CYC(b_+151, b_+152); A = alu_dec8(gb, A);
  CYC(b_+152, b_+155); mem_wr(gb, wTmpcbba, A);
  CYC(b_+155, b_+158);
  TAIL(cutscene_incCutsceneState);
}

void nayruWarpToMakuTreeCutsceneHandler__state3_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+158, b_+161); SET_HL(wTmpcbb3);
  CYC(b_+161, b_+163); B = 0x02;
  CALL_C(b_+163, flashScreen_hook, SYM(flashScreen), b_+166);
  if (F & FZ) { CYCT(b_+166, b_+167); ret_effect(gb); return; }
  CYC(b_+166, b_+167);
  CALL_C(b_+167, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+170);
  CYC(b_+170, b_+173); SET_HL(wTmpcbb3);
  CYC(b_+173, b_+175); mem_wr(gb, HL, 0x3c);
  CYC(b_+175, b_+177); A = 0x01;
  CYC(b_+177, b_+180); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CALL_C(b_+180, nayruWarpToMakuTreeCutsceneHandler__func_6838_hook, b_+188, b_+183);
  CYC(b_+183, b_+185); A = 0x03;
  CYC(b_+185, b_+188);
  TAIL(fadeinFromWhiteWithDelay);
}

void nayruWarpToMakuTreeCutsceneHandler__state4_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+210, cutscene_decCBB3whenFadeDone_hook, SYM(cutscene_decCBB3whenFadeDone), b_+213);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; }
  CYC(b_+213, b_+214);
  CYC(b_+214, b_+217);
  TAIL(cutscene_incCutsceneState);
}

void nayruWarpToMakuTreeCutsceneHandler__state5_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+217, b_+220); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+220, b_+222); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+222, b_+223); ret_effect(gb); return; }
  CYC(b_+222, b_+223);
  CALL_C(b_+223, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+226);
  CYC(b_+226, b_+228); A = 0xfa;
  CALL_C(b_+228, playSound_b00_hook, SYM(playSound_b00), b_+231);
  CYC(b_+231, b_+232); alu_xor(gb, A);
  CYC(b_+232, b_+235); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+235, b_+237); mem_wr(gb, HL, 0x05);
  CYC(b_+237, b_+238); L = alu_inc8(gb, L);
  CYC(b_+238, b_+240); mem_wr(gb, HL, 0x00);
  CYC(b_+240, b_+243);
  TAIL(fadeoutToWhite);
}

void nayruWarpToMakuTreeCutsceneHandler__state7_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+259, b_+262); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+263, b_+264); ret_effect(gb); return; }
  CYC(b_+263, b_+264);
  CYC(b_+264, b_+267); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+267, b_+268); A = mem_rd(gb, HL);
  CYC(b_+268, b_+270); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+270, b_+271); ret_effect(gb); return; }
  CYC(b_+270, b_+271);
  CYC(b_+271, b_+272); alu_xor(gb, A);
  CYC(b_+272, b_+273); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+273, b_+274); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+274, b_+275); A = mem_rd(gb, HL);
  CYC(b_+275, b_+277); alu_cp(gb, 0x07);
  CYC(b_+277, b_+279); A = 0x06;
  if (!(F & FZ)) {
    CYCT(b_+279, b_+281);
  } else {
    CYC(b_+279, b_+281);
    CYC(b_+281, b_+283); A = 0x08;
    CYC(b_+283, b_+286); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
    CYC(b_+286, b_+288); mem_wr(gb, HL, 0x03);
  }
  CYC(b_+288, b_+291); SET_HL(wCutsceneState);
  CYC(b_+291, b_+292); mem_wr(gb, HL, A);
  CYC(b_+292, b_+295);
  TAIL(fadeoutToWhite);
}

void nayruWarpToMakuTreeCutsceneHandler__state9_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+364, b_+367); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+367, b_+369); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+369, b_+370); ret_effect(gb); return; }
  CYC(b_+369, b_+370);
  CALL_C(b_+370, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+373);
  CYC(b_+373, b_+375); A = 0xf1;
  CALL_C(b_+375, playSound_b00_hook, SYM(playSound_b00), b_+378);
  CYC(b_+378, b_+381); SET_BC(0x2800);
  CALL_C(b_+381, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+384);
  if (F & FZ) {
    CYCT(b_+384, b_+386);
  } else {
    CYC(b_+384, b_+386);
    CYC(b_+386, b_+388); C = 0x02;
  }
  CYC(b_+388, b_+391);
  TAIL(showText);
}

void nayruWarpToMakuTreeCutsceneHandler__stateA_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+391, b_+394); A = mem_rd(gb, wTextIsActive);
  CYC(b_+394, b_+395); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+395, b_+396); ret_effect(gb); return; }
  CYC(b_+395, b_+396);
  CALL_C(b_+396, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+399);
  CYC(b_+399, b_+402); SET_HL(wTmpcbb3);
  CYC(b_+402, b_+404); mem_wr(gb, HL, 0xb4);
  CYC(b_+404, b_+406); A = 0x01;
  CYC(b_+406, b_+409); mem_wr(gb, w1Link_direction, A);
  CYC(b_+409, b_+412); mem_wr(gb, wTmpcbb7, A);
  CYC(b_+412, b_+414);
  TAIL(nayruWarpToMakuTreeCutsceneHandler__func_6955);
}

void nayruWarpToMakuTreeCutsceneHandler__stateB_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+414, decCbb3_hook, SYM(decCbb3), b_+417);
  if (!(F & FZ)) {
    CYCT(b_+417, b_+419);
    TAIL(nayruWarpToMakuTreeCutsceneHandler__func_6948);
  }
  CYC(b_+417, b_+419);
  CALL_C(b_+419, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+422);
  if (F & FZ) {
    CYCT(b_+422, b_+424);
    TAIL(nayruWarpToMakuTreeCutsceneHandler__func_692b);
  }
  CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); A = 0x08;
  CYC(b_+426, b_+429); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+429, b_+431);
  TAIL(cutscene_incCutsceneState);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6838_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+188, b_+190); A = 0x00;
  CALL_C(b_+190, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+193);
  CYC(b_+193, b_+195); L = 0x00;
  CYC(b_+195, b_+197); mem_wr(gb, HL, 0x03);
  CYC(b_+197, b_+199); L = 0x0b;
  CYC(b_+199, b_+201); mem_wr(gb, HL, 0x68);
  CYC(b_+201, b_+203); L = 0x0d;
  CYC(b_+203, b_+205); mem_wr(gb, HL, 0x50);
  CYC(b_+205, b_+207); L = 0x08;
  CYC(b_+207, b_+209); mem_wr(gb, HL, 0x00);
  CYC(b_+209, b_+210); ret_effect(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_692b_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+431, getFreePartSlot_hook, SYM(getFreePartSlot), b_+434);
  if (!(F & FZ)) {
    CYCT(b_+434, b_+436);
  } else {
    CYC(b_+434, b_+436);
    CYC(b_+436, b_+438); mem_wr(gb, HL, 0x27);
    CYC(b_+438, b_+439); L = alu_inc8(gb, L);
    CYC(b_+439, b_+440); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+440, b_+441); L = alu_inc8(gb, L);
    CYC(b_+441, b_+442); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+442, b_+444); L = 0xcb;
    CYC(b_+444, b_+446); mem_wr(gb, HL, 0x40);
    CYC(b_+446, b_+448); L = 0xcd;
    CYC(b_+448, b_+450); mem_wr(gb, HL, 0x88);
  }
  CALL_C(b_+450, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+453);
  if (!(F & FZ)) {
    CYCT(b_+453, b_+455);
  } else {
    CYC(b_+453, b_+455);
    CYC(b_+455, b_+457); mem_wr(gb, HL, 0x8d);
  }
  CYC(b_+457, b_+460);
  TAIL(cutscene_incCutsceneState);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6948_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+460, cutscene_decCBB6_hook, SYM(cutscene_decCBB6), b_+463);
  if (!(F & FZ)) {
    CYCT(b_+463, b_+464); ret_effect(gb);
    return;
  }
  CYC(b_+463, b_+464);
  CYC(b_+464, b_+466); L = 0xb7;
  CYC(b_+466, b_+467); A = mem_rd(gb, HL);
  CYC(b_+467, b_+469); alu_xor(gb, 0x02);
  CYC(b_+469, b_+470); mem_wr(gb, HL, A);
  CALL_C(b_+470, nayruWarpToMakuTreeCutsceneHandler__func_6962_hook, b_+486, b_+473);
  TAIL(nayruWarpToMakuTreeCutsceneHandler__func_6955);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6955_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+473, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+476);
  CYC(b_+476, b_+478); alu_and(gb, 0x03);
  CYC(b_+478, b_+479); alu_add(gb, A);
  CYC(b_+479, b_+480); alu_add(gb, A);
  CYC(b_+480, b_+482); alu_add(gb, 0x10);
  CYC(b_+482, b_+485); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+485, b_+486); ret_effect(gb);
}

void nayruWarpToMakuTreeCutsceneHandler__func_6962_hook(GB *gb) {
  BASE(nayruWarpToMakuTreeCutsceneHandler);
  CYC(b_+486, b_+489); mem_wr(gb, w1Link_direction, A);
  CYC(b_+489, b_+491); A = 0x08;
  CYC(b_+491, b_+494); mem_wr(gb, wLinkForceState, A);
  CYC(b_+494, b_+495); ret_effect(gb);
}

void black_tower_complete_after_states_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+3, updateStatusBar_hook, SYM(updateStatusBar), b_+6);
  CYC(b_+6, b_+9);
  TAIL(updateAllObjects);
}

void blackTowerCompleteCutsceneHandler__afterCall69b8_hook(GB *gb) {
  black_tower_complete_after_states_hook(gb);
}

void blackTowerCompleteCutsceneHandler__runStates_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+9, b_+12); SET_DE(wCutsceneState);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+26) { blackTowerCompleteCutsceneHandler__state0_hook(gb); return; }
    else if (jt_ == b_+34) { blackTowerCompleteCutsceneHandler__state1_hook(gb); return; }
    else if (jt_ == b_+49) { blackTowerCompleteCutsceneHandler__state2_hook(gb); return; }
    else if (jt_ == b_+135) { blackTowerCompleteCutsceneHandler__state3_hook(gb); return; }
    else if (jt_ == b_+157) { blackTowerCompleteCutsceneHandler__state4_hook(gb); return; }
    else if (jt_ == b_+216) { blackTowerCompleteCutsceneHandler__state5_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void blackTowerCompleteCutsceneHandler_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, blackTowerCompleteCutsceneHandler__runStates_hook, b_+9, b_+3);
  black_tower_complete_after_states_hook(gb);
}

void black_tower_complete_state4_after_room_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+182, showStatusBar_hook, SYM(showStatusBar), b_+185);
  CYC(b_+185, b_+187); A = 0x21;
  CALL_C(b_+187, playSound_b00_hook, SYM(playSound_b00), b_+190);
  CYC(b_+190, b_+192); A = 0xac;
  CALL_C(b_+192, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+195);
  CYC(b_+195, b_+197); A = 0x02;
  CALL_C(b_+197, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+200);
  CYC(b_+200, b_+201); alu_xor(gb, A);
  CYC(b_+201, b_+204); mem_wr(gb, wScrollMode, A);
  CYC(b_+204, b_+206); A = 0x28;
  CYC(b_+206, b_+209); mem_wr(gb, wGfxRegs2_SCX, A);
  CYC(b_+209, b_+211); hram_wr(gb, (uint8_t)hCameraX, A);
  CYC(b_+211, b_+213); A = 0xf0;
  CYC(b_+213, b_+216); mem_wr(gb, wGfxRegs2_SCY, A);
  TAIL(blackTowerCompleteCutsceneHandler__state5);
}

void blackTowerCompleteCutsceneHandler__state4__afterCall6a6b_hook(GB *gb) {
  black_tower_complete_state4_after_room_hook(gb);
}

void blackTowerCompleteCutsceneHandler__state4_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+157, b_+160); SET_HL(wTmpcbb3);
  CYC(b_+160, b_+162); B = 0x02;
  CALL_C(b_+162, flashScreen_hook, SYM(flashScreen), b_+165);
  if (F & FZ) { CYCT(b_+165, b_+166); ret_effect(gb); return; }
  CYC(b_+165, b_+166);
  CALL_C(b_+166, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+169);
  CYC(b_+169, b_+171); A = 0x10;
  CYC(b_+171, b_+174); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CYC(b_+174, b_+177); SET_HL(SYM(cutscene_loadRoomObjectSetAndFadein));
  CYC(b_+177, b_+179); E = 0x03;
  CALL_C(b_+179, interBankCall_hook, 0x008a, b_+182);
  black_tower_complete_state4_after_room_hook(gb);
}

void blackTowerCompleteCutsceneHandler__state0_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  CYC(b_+26, b_+28); A = 0x3c;
  CYC(b_+28, b_+31); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+31, b_+34);
  TAIL(cutscene_incCutsceneState);
}

void blackTowerCompleteCutsceneHandler__state1_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+34, decCbb3_hook, SYM(decCbb3), b_+37);
  if (!(F & FZ)) { CYCT(b_+37, b_+38); ret_effect(gb); return; }
  CYC(b_+37, b_+38);
  CALL_C(b_+38, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+41);
  CYC(b_+41, b_+43); A = 0xfa;
  CALL_C(b_+43, playSound_b00_hook, SYM(playSound_b00), b_+46);
  CYC(b_+46, b_+49);
  TAIL(fastFadeoutToBlack);
}

void blackTowerCompleteCutsceneHandler__state2_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+49, b_+52); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, hideStatusBar_hook, SYM(hideStatusBar), b_+57);
  CYC(b_+57, b_+59); A = hram_rd(gb, 0x70);
  CYC(b_+59, b_+60); push_effect(gb, AF);
  CYC(b_+60, b_+62); A = 0x02;
  CYC(b_+62, b_+64); hram_wr(gb, 0x70, A);
  CYC(b_+64, b_+67); SET_HL((w2TilesetBgPalettes + 16));
  CYC(b_+67, b_+69); B = 0x30;
  CALL_C(b_+69, clearMemory_hook, SYM(clearMemory), b_+72);
  CYC(b_+72, b_+73); SET_AF(pop_effect(gb));
  CYC(b_+73, b_+75); hram_wr(gb, 0x70, A);
  CYC(b_+75, b_+78); SET_HL((SYM(func_03_6275__state1) + 44));
  CYC(b_+78, b_+80); E = 0x01;
  CALL_C(b_+80, interBankCall_hook, 0x008a, b_+83);
  CYC(b_+83, b_+84); alu_xor(gb, A);
  CYC(b_+84, b_+87); mem_wr(gb, wScrollMode, A);
  CYC(b_+87, b_+90); mem_wr(gb, wTilesetFlags, A);
  CYC(b_+90, b_+93); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(b_+93, b_+96); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(b_+96, b_+99); mem_wr(gb, w1Link_visible, A);
  CYC(b_+99, b_+101); A = 0x10;
  CYC(b_+101, b_+104); mem_wr(gb, wScreenOffsetY, A);
  CALL_C(b_+104, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+107);
  if (F & FZ) {
    CYCT(b_+107, b_+109);
    TAIL(blackTowerCompleteCutsceneHandler__func_6a2b);
  }
  CYC(b_+107, b_+109);
  CALL_C(b_+109, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+112);
  CYC(b_+112, b_+115); SET_HL(wTmpcbb3);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x1e);
  CYC(b_+117, b_+118); ret_effect(gb);
}

void blackTowerCompleteCutsceneHandler__state3_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+135, decCbb3_hook, SYM(decCbb3), b_+138);
  if (!(F & FZ)) { CYCT(b_+138, b_+139); ret_effect(gb); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); A = 0xd2;
  CALL_C(b_+141, playSound_b00_hook, SYM(playSound_b00), b_+144);
  CYC(b_+144, b_+145); alu_xor(gb, A);
  CYC(b_+145, b_+148); SET_HL(wTmpcbb3);
  CYC(b_+148, b_+149); mem_wr(gb, HL, A);
  CYC(b_+149, b_+150); A = alu_dec8(gb, A);
  CYC(b_+150, b_+153); SET_HL(wTmpcbba);
  CYC(b_+153, b_+154); mem_wr(gb, HL, A);
  CYC(b_+154, b_+157);
  TAIL(cutscene_incCutsceneState);
}

void blackTowerCompleteCutsceneHandler__state5_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  CYC(b_+216, b_+217); ret_effect(gb);
}

void blackTowerCompleteCutsceneHandler__func_6a2b_hook(GB *gb) {
  BASE(blackTowerCompleteCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+118, clearWramBank1_hook, SYM(clearWramBank1), b_+121);
  CALL_C(b_+121, clearOam_hook, SYM(clearOam), b_+124);
  CYC(b_+124, b_+126); A = 0x05;
  CYC(b_+126, b_+129); mem_wr(gb, wCutsceneState, A);
  CYC(b_+129, b_+132); SET_BC(0x8d01);
  CYC(b_+132, b_+135);
  TAIL(createInteraction);
}

void turn_to_stone_after_states_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+3, updateStatusBar_hook, SYM(updateStatusBar), b_+6);
  CYC(b_+6, b_+9);
  TAIL(updateAllObjects);
}

void turnToStoneCutsceneHandler__afterCall6a91_hook(GB *gb) {
  turn_to_stone_after_states_hook(gb);
}

void turnToStoneCutsceneHandler__runStates_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+9, b_+12); SET_DE(wCutsceneState);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+28) { turnToStoneCutsceneHandler__state0_hook(gb); return; }
    else if (jt_ == b_+53) { turnToStoneCutsceneHandler__state1_hook(gb); return; }
    else if (jt_ == b_+69) { turnToStoneCutsceneHandler__state2_hook(gb); return; }
    else if (jt_ == b_+100) { turnToStoneCutsceneHandler__state3_hook(gb); return; }
    else if (jt_ == b_+136) { turnToStoneCutsceneHandler__state4_hook(gb); return; }
    else if (jt_ == b_+148) { turnToStoneCutsceneHandler__state5_hook(gb); return; }
    else if (jt_ == b_+216) { turnToStoneCutsceneHandler__state6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void turnToStoneCutsceneHandler_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, turnToStoneCutsceneHandler__runStates_hook, b_+9, b_+3);
  turn_to_stone_after_states_hook(gb);
}

void turn_to_stone_state1_after_room_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  CYC(b_+64, b_+66); A = 0x02;
  CYC(b_+66, b_+69);
  TAIL(loadGfxRegisterStateIndex);
}

void turnToStoneCutsceneHandler__state1__afterCall6ace_hook(GB *gb) {
  turn_to_stone_state1_after_room_hook(gb);
}

void turnToStoneCutsceneHandler__state1_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+53, b_+56); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+57, b_+58); ret_effect(gb); return; }
  CYC(b_+57, b_+58);
  CALL_C(b_+58, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+61);
  CALL_C(b_+61, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+64);
  turn_to_stone_state1_after_room_hook(gb);
}

void turn_to_stone_state3_after_state1_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  CYC(b_+108, b_+110); A = 0x08;
  CYC(b_+110, b_+112); B = 0x28;
  CYC(b_+112, b_+115); SET_HL(hCameraY);
  CYC(b_+115, b_+116); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+116, b_+117); L = alu_inc8(gb, L);
  CYC(b_+117, b_+118); mem_wr(gb, HL, B);
  CYC(b_+118, b_+120); A = 0xf8;
  CYC(b_+120, b_+123); SET_HL(wGfxRegs2_SCY);
  CYC(b_+123, b_+124); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+124, b_+125); mem_wr(gb, HL, B);
  CYC(b_+125, b_+126); alu_xor(gb, A);
  CYC(b_+126, b_+129); mem_wr(gb, wScrollMode, A);
  CYC(b_+129, b_+132); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+132, b_+135); mem_wr(gb, wScreenOffsetX, A);
  CYC(b_+135, b_+136); ret_effect(gb);
}

void turnToStoneCutsceneHandler__state3__afterCall6afa_hook(GB *gb) {
  turn_to_stone_state3_after_state1_hook(gb);
}

void turnToStoneCutsceneHandler__state3_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+100, b_+103); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+103, b_+104); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+104, b_+105); ret_effect(gb); return; }
  CYC(b_+104, b_+105);
  CALL_C(b_+105, turnToStoneCutsceneHandler__state1_hook, b_+53, b_+108);
  turn_to_stone_state3_after_state1_hook(gb);
}

void turn_to_stone_state5_after_refresh_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+194, b_+196); A = 0xf0;
  CALL_C(b_+196, playSound_b00_hook, SYM(playSound_b00), b_+199);
  CALL_C(b_+199, showStatusBar_hook, SYM(showStatusBar), b_+202);
  CYC(b_+202, b_+204); A = 0x02;
  CALL_C(b_+204, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+207);
  CYC(b_+207, b_+210); A = mem_rd(gb, wLoadingRoomPack);
  CYC(b_+210, b_+213); mem_wr(gb, wRoomPack, A);
  CYC(b_+213, b_+216);
  TAIL(fadeinFromWhiteToRoom);
}

void turnToStoneCutsceneHandler__state5__afterCall6b50_hook(GB *gb) {
  turn_to_stone_state5_after_refresh_hook(gb);
}

void turnToStoneCutsceneHandler__state5_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+148, b_+151); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+151, b_+152); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+152, b_+153); ret_effect(gb); return; }
  CYC(b_+152, b_+153);
  CALL_C(b_+153, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+156);
  CALL_C(b_+156, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+159);
  CYC(b_+159, b_+162); SET_BC(0x0290);
  CALL_C(b_+162, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+165);
  CALL_C(b_+165, resetCamera_hook, SYM(resetCamera), b_+168);
  CYC(b_+168, b_+171); SET_HL((SYM(func_03_7619__state4__cbb3_03) + 47));
  CALL_C(b_+171, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+174);
  CYC(b_+174, b_+177); SET_HL(w1Link_enabled);
  CYC(b_+177, b_+179); mem_wr(gb, HL, 0x03);
  CYC(b_+179, b_+181); L = (uint8_t)w1Link_yh;
  CYC(b_+181, b_+183); mem_wr(gb, HL, 0x58);
  CYC(b_+183, b_+185); L = (uint8_t)w1Link_xh;
  CYC(b_+185, b_+187); mem_wr(gb, HL, 0x50);
  CYC(b_+187, b_+189); L = (uint8_t)w1Link_direction;
  CYC(b_+189, b_+191); mem_wr(gb, HL, 0x02);
  CALL_C(b_+191, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+194);
  turn_to_stone_state5_after_refresh_hook(gb);
}

void turnToStoneCutsceneHandler__state0_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+28, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+31);
  CYC(b_+31, b_+33); B = 0x20;
  CYC(b_+33, b_+36); SET_HL(wTmpcfc0_genericCutscene_state);
  CALL_C(b_+36, clearMemory_hook, SYM(clearMemory), b_+39);
  CYC(b_+39, b_+41); A = 0x0d;
  CYC(b_+41, b_+44); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x00);
  CALL_C(b_+47, showStatusBar_hook, SYM(showStatusBar), b_+50);
  CYC(b_+50, b_+53);
  TAIL(fadeoutToWhite);
}

void turnToStoneCutsceneHandler__state2_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+69, b_+72); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+73, b_+74); ret_effect(gb); return; }
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+77); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+77, b_+78); A = mem_rd(gb, HL);
  CYC(b_+78, b_+80); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+80, b_+81); ret_effect(gb); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+82); alu_xor(gb, A);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+83, b_+84); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); alu_cp(gb, 0x0f);
  CYC(b_+87, b_+89); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+89, b_+91);
  } else {
    CYC(b_+89, b_+91);
    CYC(b_+91, b_+93); A = 0x03;
  }
  CYC(b_+93, b_+96); SET_HL(wCutsceneState);
  CYC(b_+96, b_+97); mem_wr(gb, HL, A);
  CYC(b_+97, b_+100);
  TAIL(fadeoutToWhite);
}

void turnToStoneCutsceneHandler__state4_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+136, b_+139); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+139, b_+141); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+141, b_+142); ret_effect(gb); return; }
  CYC(b_+141, b_+142);
  CALL_C(b_+142, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+145);
  CYC(b_+145, b_+148);
  TAIL(fadeoutToWhite);
}

void turnToStoneCutsceneHandler__state6_hook(GB *gb) {
  BASE(turnToStoneCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+216, b_+219); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+219, b_+220); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+220, b_+221); ret_effect(gb); return; }
  CYC(b_+220, b_+221);
  CYC(b_+221, b_+223); A = 0x01;
  CYC(b_+223, b_+226); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+226, b_+229); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+229, b_+232); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+232, b_+233); ret_effect(gb);
}

void twinrova_reveal_after_states_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+3, updateStatusBar_hook, SYM(updateStatusBar), b_+6);
  CYC(b_+6, b_+9);
  TAIL(updateAllObjects);
}

void twinrovaRevealCutsceneHandler__afterCall6b7a_hook(GB *gb) {
  twinrova_reveal_after_states_hook(gb);
}

void twinrovaRevealCutsceneHandler__runStates_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+9, b_+12); SET_DE(wCutsceneState);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+34) { twinrovaRevealCutsceneHandler__state0_hook(gb); return; }
    else if (jt_ == b_+73) { twinrovaRevealCutsceneHandler__state1_hook(gb); return; }
    else if (jt_ == b_+94) { twinrovaRevealCutsceneHandler__state2_hook(gb); return; }
    else if (jt_ == b_+146) { twinrovaRevealCutsceneHandler__state3_hook(gb); return; }
    else if (jt_ == b_+165) { twinrovaRevealCutsceneHandler__state4_hook(gb); return; }
    else if (jt_ == b_+201) { twinrovaRevealCutsceneHandler__state5_hook(gb); return; }
    else if (jt_ == b_+221) { twinrovaRevealCutsceneHandler__state6_hook(gb); return; }
    else if (jt_ == b_+294) { twinrovaRevealCutsceneHandler__state7_hook(gb); return; }
    else if (jt_ == b_+327) { twinrovaRevealCutsceneHandler__state8_hook(gb); return; }
    else if (jt_ == b_+389) { twinrovaRevealCutsceneHandler__state9_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void twinrovaRevealCutsceneHandler_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, twinrovaRevealCutsceneHandler__runStates_hook, b_+9, b_+3);
  twinrova_reveal_after_states_hook(gb);
}

void twinrova_state4_after_room_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+182, showStatusBar_hook, SYM(showStatusBar), b_+185);
  CALL_C(b_+185, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+188);
  CYC(b_+188, b_+190); A = 0x38;
  CALL_C(b_+190, playSound_b00_hook, SYM(playSound_b00), b_+193);
  CYC(b_+193, b_+195); A = 0x02;
  CALL_C(b_+195, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+198);
  CYC(b_+198, b_+201);
  TAIL(resetCamera);
}

void twinrovaRevealCutsceneHandler__state4__afterCall6c2d_hook(GB *gb) {
  twinrova_state4_after_room_hook(gb);
}

void twinrovaRevealCutsceneHandler__state4_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+165, b_+168); SET_HL(wTmpcbb3);
  CYC(b_+168, b_+170); B = 0x04;
  CALL_C(b_+170, flashScreen_hook, SYM(flashScreen), b_+173);
  if (F & FZ) { CYCT(b_+173, b_+174); ret_effect(gb); return; }
  CYC(b_+173, b_+174);
  CYC(b_+174, b_+176); A = 0x12;
  CYC(b_+176, b_+179); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CALL_C(b_+179, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+182);
  twinrova_state4_after_room_hook(gb);
}

void twinrova_state6_after_reload_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+286, b_+288); A = 0x02;
  CALL_C(b_+288, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+291);
  CYC(b_+291, b_+294);
  TAIL(func_6fb0);
}

void twinrova_state6_after_room_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+237, b_+240); SET_HL(w1Link_enabled);
  CYC(b_+240, b_+242); mem_wr(gb, HL, 0x03);
  CYC(b_+242, b_+244); L = (uint8_t)w1Link_yh;
  CYC(b_+244, b_+246); mem_wr(gb, HL, 0x68);
  CYC(b_+246, b_+248); L = (uint8_t)w1Link_xh;
  CYC(b_+248, b_+250); mem_wr(gb, HL, 0x50);
  CYC(b_+250, b_+252); L = (uint8_t)w1Link_direction;
  CYC(b_+252, b_+254); mem_wr(gb, HL, 0x00);
  CYC(b_+254, b_+256); A = 0x2c;
  CYC(b_+256, b_+258); B = 0x03;
  CALL_C(b_+258, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+261);
  CALL_C(b_+261, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+264);
  CYC(b_+264, b_+265); alu_xor(gb, A);
  CYC(b_+265, b_+268); mem_wr(gb, wPaletteThread_mode, A);
  CYC(b_+268, b_+270); hram_wr(gb, (uint8_t)hVBlankFunctionQueueTail, A);
  CYC(b_+270, b_+271); A = alu_inc8(gb, A);
  CYC(b_+271, b_+274); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+274, b_+277); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+277, b_+280); A = mem_rd(gb, wLoadingRoomPack);
  CYC(b_+280, b_+283); mem_wr(gb, wRoomPack, A);
  CALL_C(b_+283, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+286);
  twinrova_state6_after_reload_hook(gb);
}

void twinrovaRevealCutsceneHandler__state6__afterCall6c64_hook(GB *gb) {
  twinrova_state6_after_room_hook(gb);
}

void twinrovaRevealCutsceneHandler__state6__afterCall6c95_hook(GB *gb) {
  twinrova_state6_after_reload_hook(gb);
}

void twinrovaRevealCutsceneHandler__state6_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+221, b_+224); SET_HL(wTmpcbb3);
  CYC(b_+224, b_+226); B = 0x01;
  CALL_C(b_+226, flashScreen_hook, SYM(flashScreen), b_+229);
  if (F & FZ) { CYCT(b_+229, b_+230); ret_effect(gb); return; }
  CYC(b_+229, b_+230);
  CYC(b_+230, b_+233); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+233, b_+234); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+234, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+237);
  twinrova_state6_after_room_hook(gb);
}

void twinrova_state9_after_refresh_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  CYC(b_+398, b_+400); A = 0x01;
  CYC(b_+400, b_+403); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CYC(b_+403, b_+404); ret_effect(gb);
}

void twinrovaRevealCutsceneHandler__state9__afterCall6d05_hook(GB *gb) {
  twinrova_state9_after_refresh_hook(gb);
}

void twinrovaRevealCutsceneHandler__state9_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+389, b_+392); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+392, b_+394); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+394, b_+395); ret_effect(gb); return; }
  CYC(b_+394, b_+395);
  CALL_C(b_+395, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+398);
  twinrova_state9_after_refresh_hook(gb);
}

void twinrovaRevealCutsceneHandler__state0_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+34, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+37);
  CYC(b_+37, b_+39); B = 0x20;
  CYC(b_+39, b_+42); SET_HL(wTmpcfc0_genericCutscene_state);
  CALL_C(b_+42, clearMemory_hook, SYM(clearMemory), b_+45);
  CYC(b_+45, b_+47); A = 0xf0;
  CALL_C(b_+47, playSound_b00_hook, SYM(playSound_b00), b_+50);
  CYC(b_+50, b_+53); SET_HL(wTmpcbb3);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x3c);
  CYC(b_+55, b_+58); SET_BC(0x2810);
  CALL_C(b_+58, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+61);
  if (F & FZ) {
    CYCT(b_+61, b_+63);
  } else {
    CYC(b_+61, b_+63);
    CYC(b_+63, b_+65); C = 0x15;
  }
  CYC(b_+65, b_+67); A = 0x02;
  CYC(b_+67, b_+70); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+70, b_+73);
  TAIL(showText);
}

void twinrovaRevealCutsceneHandler__state1_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+73, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+76);
  if (!(F & FZ)) { CYCT(b_+76, b_+77); ret_effect(gb); return; }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); A = 0xf0;
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);
  TAIL(twinrovaRevealCutsceneHandler__func_6bc9);
}

void twinrovaRevealCutsceneHandler__func_6bc9_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  CYC(b_+82, b_+85); SET_HL(wTmpcbb3);
  CYC(b_+85, b_+86); alu_xor(gb, A);
  CYC(b_+86, b_+87); mem_wr(gb, HL, A);
  CYC(b_+87, b_+88); A = alu_dec8(gb, A);
  CYC(b_+88, b_+91); mem_wr(gb, wTmpcbba, A);
  CYC(b_+91, b_+94);
  TAIL(cutscene_incCutsceneState);
}

void twinrovaRevealCutsceneHandler__state2_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+94, b_+97); SET_HL(wTmpcbb3);
  CYC(b_+97, b_+99); B = 0x01;
  CALL_C(b_+99, flashScreen_hook, SYM(flashScreen), b_+102);
  if (F & FZ) { CYCT(b_+102, b_+103); ret_effect(gb); return; }
  CYC(b_+102, b_+103);
  CALL_C(b_+103, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+106);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+108);
    TAIL(twinrovaRevealCutsceneHandler__func_6be8);
  }
  CYC(b_+106, b_+108);
  CALL_C(b_+108, func_6fb0_hook, SYM(func_6fb0), b_+111);
  CYC(b_+111, b_+113);
  CYC(b_+116, b_+118); A = 0x01;
  CYC(b_+118, b_+121); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+121, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+124);
  CYC(b_+124, b_+127); SET_HL((SYM(func_03_7619__state5) + 24));
  CALL_C(b_+127, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+130);
  if (!(F & FZ)) {
    CYCT(b_+130, b_+132);
  } else {
    CYC(b_+130, b_+132);
    CYC(b_+132, b_+135); SET_HL(wCutsceneState);
    CYC(b_+135, b_+137); mem_wr(gb, HL, 0x06);
    CYC(b_+137, b_+140); SET_HL((SYM(func_03_7619__state5) + 11));
  }
  CALL_C(b_+140, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+143);
  CYC(b_+143, b_+146);
  TAIL(cutscene_incCutsceneState);
}

void twinrovaRevealCutsceneHandler__func_6be8_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+113, func_6f9e_hook, SYM(func_6f9e), b_+116);
  CYC(b_+116, b_+118); A = 0x01;
  CYC(b_+118, b_+121); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+121, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+124);
  CYC(b_+124, b_+127); SET_HL((SYM(func_03_7619__state5) + 24));
  CALL_C(b_+127, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+130);
  if (!(F & FZ)) {
    CYCT(b_+130, b_+132);
  } else {
    CYC(b_+130, b_+132);
    CYC(b_+132, b_+135); SET_HL(wCutsceneState);
    CYC(b_+135, b_+137); mem_wr(gb, HL, 0x06);
    CYC(b_+137, b_+140); SET_HL((SYM(func_03_7619__state5) + 11));
  }
  CALL_C(b_+140, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+143);
  CYC(b_+143, b_+146);
  TAIL(cutscene_incCutsceneState);
}

void twinrovaRevealCutsceneHandler__state3_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+146, b_+149); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+149, b_+151); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+151, b_+152); ret_effect(gb); return; }
  CYC(b_+151, b_+152);
  CYC(b_+152, b_+154); A = 0xf0;
  CALL_C(b_+154, playSound_b00_hook, SYM(playSound_b00), b_+157);
  CYC(b_+157, b_+159); A = 0xd2;
  CALL_C(b_+159, playSound_b00_hook, SYM(playSound_b00), b_+162);
  CYC(b_+162, b_+165);
  TAIL(twinrovaRevealCutsceneHandler__func_6bc9);
}

void twinrovaRevealCutsceneHandler__state5_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+201, b_+204); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+204, b_+205); A = mem_rd(gb, HL);
  CYC(b_+205, b_+207); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+207, b_+208); ret_effect(gb); return; }
  CYC(b_+207, b_+208);
  CYC(b_+208, b_+210); A = 0xd2;
  CALL_C(b_+210, playSound_b00_hook, SYM(playSound_b00), b_+213);
  CYC(b_+213, b_+215); A = 0xf0;
  CALL_C(b_+215, playSound_b00_hook, SYM(playSound_b00), b_+218);
  CYC(b_+218, b_+221);
  TAIL(twinrovaRevealCutsceneHandler__func_6bc9);
}

void twinrovaRevealCutsceneHandler__state7_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+294, b_+297); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+297, b_+299); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(b_+299, b_+300); ret_effect(gb); return; }
  CYC(b_+299, b_+300);
  CYC(b_+300, b_+302); A = 0xfa;
  CALL_C(b_+302, playSound_b00_hook, SYM(playSound_b00), b_+305);
  CALL_C(b_+305, fadeoutToBlack_hook, SYM(fadeoutToBlack), b_+308);
  CYC(b_+308, b_+310); A = 0xff;
  CYC(b_+310, b_+313); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+313, b_+316); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+316, b_+318); A = 0x03;
  CYC(b_+318, b_+321); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+321, b_+324); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+324, b_+327);
  TAIL(cutscene_incCutsceneState);
}

void twinrovaRevealCutsceneHandler__state8_hook(GB *gb) {
  BASE(twinrovaRevealCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+327, b_+330); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+330, b_+331); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+331, b_+332); ret_effect(gb); return; }
  CYC(b_+331, b_+332);
  CALL_C(b_+332, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+335);
  CALL_C(b_+335, showStatusBar_hook, SYM(showStatusBar), b_+338);
  CYC(b_+338, b_+340); A = 0x35;
  CALL_C(b_+340, setGlobalFlag_hook, SYM(setGlobalFlag), b_+343);
  CYC(b_+343, b_+345); A = 0x01;
  CYC(b_+345, b_+348); mem_wr(gb, wScrollMode, A);
  CYC(b_+348, b_+349); alu_xor(gb, A);
  CYC(b_+349, b_+352); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+352, b_+355); mem_wr(gb, wScreenShakeCounterX, A);
  CYC(b_+355, b_+357); A = 0x0f;
  CYC(b_+357, b_+360); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(b_+360, b_+362); A = 0xf0;
  CYC(b_+362, b_+365); mem_wr(gb, wGfxRegs2_SCY, A);
  CALL_C(b_+365, fadeinFromBlack_hook, SYM(fadeinFromBlack), b_+368);
  CYC(b_+368, b_+371); SET_BC(0x8706);
  CALL_C(b_+371, createInteraction_hook, SYM(createInteraction), b_+374);
  CYC(b_+374, b_+377); SET_BC((SYM(init) + 80));
  CALL_C(b_+377, interactionHSetPosition_hook, SYM(interactionHSetPosition), b_+380);
  CYC(b_+380, b_+383); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+383, b_+386); mem_wr(gb, wActiveMusic, A);
  CYC(b_+386, b_+389);
  TAIL(playSound_b00);
}

void pregameIntroCutsceneHandler__func_6d40_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+53, b_+55); A = 0x01;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+59); SET_BC(0x05f1);
  CALL_C(b_+59, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+62);
  CALL_C(b_+62, resetCamera_hook, SYM(resetCamera), b_+65);
  CYC(b_+65, b_+67); A = 0xac;
  CALL_C(b_+67, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+70);
  CYC(b_+70, b_+73); SET_HL((SYM(func_03_7619__state5) + 28));
  CALL_C(b_+73, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+76);
  CYC(b_+76, b_+78); A = 0x1b;
  CALL_C(b_+78, playSound_b00_hook, SYM(playSound_b00), b_+81);
  CYC(b_+81, b_+84); SET_HL(wTmpcbb3);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x3c);
  CYC(b_+86, b_+88); A = 0x13;
  CALL_C(b_+88, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+91);
  CYC(b_+91, b_+94); A = mem_rd(gb, wGfxRegs2_SCX);
  CYC(b_+94, b_+96); mem_wr(gb, hCameraX, A);
  CYC(b_+96, b_+97); alu_xor(gb, A);
  CYC(b_+97, b_+99); mem_wr(gb, hCameraY, A);
  CYC(b_+99, b_+101); A = 0x00;
  CYC(b_+101, b_+104); mem_wr(gb, wScrollMode, A);
  CYC(b_+104, b_+107);
  TAIL(clearFadingPalettes2);
}

void pregameIntroCutsceneHandler_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, pregameIntroCutsceneHandler__runStates_hook, b_+6, b_+3);
  CYC(b_+3, b_+6);
  TAIL(updateAllObjects);
}

void pregameIntroCutsceneHandler__runStates_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  CYC(b_+6, b_+9); SET_DE(wCutsceneState);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+37) { pregameIntroCutsceneHandler__state0_hook(gb); return; }
    else if (jt_ == b_+107) { pregameIntroCutsceneHandler__state1_hook(gb); return; }
    else if (jt_ == b_+121) { pregameIntroCutsceneHandler__state2_hook(gb); return; }
    else if (jt_ == b_+125) { pregameIntroCutsceneHandler__state3_hook(gb); return; }
    else if (jt_ == b_+156) { pregameIntroCutsceneHandler__state4_hook(gb); return; }
    else if (jt_ == b_+189) { pregameIntroCutsceneHandler__state5_hook(gb); return; }
    else if (jt_ == b_+205) { pregameIntroCutsceneHandler__state6_hook(gb); return; }
    else if (jt_ == b_+228) { pregameIntroCutsceneHandler__state7_hook(gb); return; }
    else if (jt_ == b_+249) { pregameIntroCutsceneHandler__state8_hook(gb); return; }
    else if (jt_ == b_+271) { pregameIntroCutsceneHandler__state9_hook(gb); return; }
    else if (jt_ == b_+283) { pregameIntroCutsceneHandler__stateA_hook(gb); return; }
    else if (jt_ == b_+362) { pregameIntroCutsceneHandler__stateB_hook(gb); return; }
    else if (jt_ == b_+379) { pregameIntroCutsceneHandler__stateC_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void pregameIntroCutsceneHandler__state0_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+37, b_+40); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
  CYC(b_+41, b_+42);
  CALL_C(b_+42, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+45);
  if (!(F & FZ)) {
    CYCT(b_+45, b_+47);
    TAIL(pregameIntroCutsceneHandler__func_6d40);
  }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); A = 0x0a;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53);
  TAIL(pregameIntroCutsceneHandler__stateA);
}

void pregameIntroCutsceneHandler__state1_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+107, b_+109); E = 0x96;
  CALL_C(b_+109, decCbb3_hook, SYM(decCbb3), b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+113); ret_effect(gb); return; }
  CYC(b_+112, b_+113);
  CALL_C(b_+113, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+116);
  CYC(b_+116, b_+119); SET_HL(wTmpcbb3);
  CYC(b_+119, b_+120); mem_wr(gb, HL, E);
  CYC(b_+120, b_+121); ret_effect(gb);
}

void pregameIntroCutsceneHandler__state2_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+121, b_+123); E = 0x3c;
  CYC(b_+123, b_+125);
  CALL_C(b_+109, decCbb3_hook, SYM(decCbb3), b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+113); ret_effect(gb); return; }
  CYC(b_+112, b_+113);
  CALL_C(b_+113, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+116);
  CYC(b_+116, b_+119); SET_HL(wTmpcbb3);
  CYC(b_+119, b_+120); mem_wr(gb, HL, E);
  CYC(b_+120, b_+121); ret_effect(gb);
}

void pregameIntroCutsceneHandler__state3_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+125, decCbb3_hook, SYM(decCbb3), b_+128);
  if (!(F & FZ)) { CYCT(b_+128, b_+129); ret_effect(gb); return; }
  CYC(b_+128, b_+129);
  CALL_C(b_+129, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+132);
  CALL_C(b_+132, fastFadeinFromBlack_hook, SYM(fastFadeinFromBlack), b_+135);
  CYC(b_+135, b_+137); A = 0x40;
  CYC(b_+137, b_+140); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+140, b_+143); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+143, b_+145); A = 0x03;
  CYC(b_+145, b_+148); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+148, b_+151); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+151, b_+153); A = 0x72;
  CYC(b_+153, b_+156);
  TAIL(playSound_b00);
}

void pregameIntroCutsceneHandler__state4_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+156, b_+159); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+159, b_+160); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+160, b_+161); ret_effect(gb); return; }
  CYC(b_+160, b_+161);
  CALL_C(b_+161, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+164);
  CYC(b_+164, b_+166); A = 0x0e;
  CYC(b_+166, b_+169); mem_wr(gb, wTmpcbb3, A);
  CALL_C(b_+169, fadeinFromBlack_hook, SYM(fadeinFromBlack), b_+172);
  CYC(b_+172, b_+174); A = 0xbf;
  CYC(b_+174, b_+177); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+177, b_+180); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+180, b_+182); A = 0xfc;
  CYC(b_+182, b_+185); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+185, b_+188); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+188, b_+189); ret_effect(gb);
}

void pregameIntroCutsceneHandler__state5_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+189, decCbb3_hook, SYM(decCbb3), b_+192);
  if (!(F & FZ)) { CYCT(b_+192, b_+193); ret_effect(gb); return; }
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+194); alu_xor(gb, A);
  CYC(b_+194, b_+197); mem_wr(gb, wPaletteThread_mode, A);
  CYC(b_+197, b_+199); A = 0x78;
  CYC(b_+199, b_+202); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+202, b_+205);
  TAIL(cutscene_incCutsceneState);
}

void pregameIntroCutsceneHandler__state6_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+205, decCbb3_hook, SYM(decCbb3), b_+208);
  if (!(F & FZ)) { CYCT(b_+208, b_+209); ret_effect(gb); return; }
  CYC(b_+208, b_+209);
  CALL_C(b_+209, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+212);
  CYC(b_+212, b_+214); A = 0x08;
  CYC(b_+214, b_+217); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+217, b_+219); A = 0x03;
  CYC(b_+219, b_+222); mem_wr(gb, wTextboxPosition, A);
  CYC(b_+222, b_+225); SET_BC(0x281a);
  CYC(b_+225, b_+228);
  TAIL(showText);
}

void pregameIntroCutsceneHandler__state7_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+228, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+231);
  CALL_C(b_+231, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+234);
  CYC(b_+234, b_+237); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+237, b_+238); A = alu_dec8(gb, A);
  CYC(b_+238, b_+241); mem_wr(gb, wTmpcbba, A);
  CALL_C(b_+241, restartSound_hook, SYM(restartSound), b_+244);
  CYC(b_+244, b_+246); A = 0xbc;
  CYC(b_+246, b_+249);
  TAIL(playSound_b00);
}

void pregameIntroCutsceneHandler__state8_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+249, b_+252); SET_HL(wTmpcbb3);
  CYC(b_+252, b_+254); B = 0x03;
  CALL_C(b_+254, flashScreen_hook, SYM(flashScreen), b_+257);
  if (F & FZ) { CYCT(b_+257, b_+258); ret_effect(gb); return; }
  CYC(b_+257, b_+258);
  CALL_C(b_+258, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+261);
  CYC(b_+261, b_+263); A = 0x3c;
  CYC(b_+263, b_+266); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271);
  TAIL(fadeoutToWhiteWithDelay);
}

void pregameIntroCutsceneHandler__state9_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+271, b_+274); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+274, b_+275); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+275, b_+276); ret_effect(gb); return; }
  CYC(b_+275, b_+276);
  CALL_C(b_+276, decCbb3_hook, SYM(decCbb3), b_+279);
  if (!(F & FZ)) { CYCT(b_+279, b_+280); ret_effect(gb); return; }
  CYC(b_+279, b_+280);
  CYC(b_+280, b_+283);
  TAIL(cutscene_incCutsceneState);
}

void pregameIntroCutsceneHandler__stateA_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+283, disableLcd_hook, SYM(disableLcd), b_+286);
  CYC(b_+286, b_+288); A = hram_rd(gb, 0x70);
  CYC(b_+288, b_+289); push_effect(gb, AF);
  CYC(b_+289, b_+291); A = 0x02;
  CYC(b_+291, b_+293); hram_wr(gb, 0x70, A);
  CYC(b_+293, b_+296); SET_HL(w2TilesetBgPalettes);
  CYC(b_+296, b_+298); B = 0x40;
  CALL_C(b_+298, clearMemory_hook, SYM(clearMemory), b_+301);
  CYC(b_+301, b_+302); SET_AF(pop_effect(gb));
  CYC(b_+302, b_+304); hram_wr(gb, 0x70, A);
  CALL_C(b_+304, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+307);
  CALL_C(b_+307, clearOam_hook, SYM(clearOam), b_+310);
  CYC(b_+310, b_+312); A = 0x0f;
  CALL_C(b_+312, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+315);
  CYC(b_+315, b_+317); A = 0x02;
  CALL_C(b_+317, func_6e9a_hook, SYM(func_6e9a), b_+320);
  CALL_C(b_+320, func_6eb7_hook, SYM(func_6eb7), b_+323);
  CYC(b_+323, b_+325); A = 0x0d;
  CALL_C(b_+325, playSound_b00_hook, SYM(playSound_b00), b_+328);
  CYC(b_+328, b_+330); A = 0x08;
  CALL_C(b_+330, setLinkID_hook, SYM(setLinkID), b_+333);
  CYC(b_+333, b_+335); L = 0x00;
  CYC(b_+335, b_+337); mem_wr(gb, HL, 0x01);
  CYC(b_+337, b_+339); L = 0x02;
  CYC(b_+339, b_+341); mem_wr(gb, HL, 0x0b);
  CYC(b_+341, b_+343); A = 0x00;
  CYC(b_+343, b_+346); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+346, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+349);
  CALL_C(b_+349, clearPaletteFadeVariablesAndRefreshPalettes_hook, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+352);
  CYC(b_+352, b_+353); alu_xor(gb, A);
  CYC(b_+353, b_+355); mem_wr(gb, hCameraY, A);
  CYC(b_+355, b_+357); mem_wr(gb, hCameraX, A);
  CYC(b_+357, b_+359); A = 0x15;
  CYC(b_+359, b_+362);
  TAIL(loadGfxRegisterStateIndex);
}

void pregameIntroCutsceneHandler__stateB_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+362, b_+365); A = mem_rd(gb, wTmpcbb9);
  CYC(b_+365, b_+367); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+367, b_+368); ret_effect(gb); return; }
  CYC(b_+367, b_+368);
  CALL_C(b_+368, clearLinkObject_hook, SYM(clearLinkObject), b_+371);
  CYC(b_+371, b_+374); SET_HL(wTmpcbb3);
  CYC(b_+374, b_+376); mem_wr(gb, HL, 0x3c);
  CYC(b_+376, b_+379);
  TAIL(cutscene_incCutsceneState);
}

void pregameIntroCutsceneHandler__stateC_hook(GB *gb) {
  BASE(pregameIntroCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+379, decCbb3_hook, SYM(decCbb3), b_+382);
  if (!(F & FZ)) { CYCT(b_+382, b_+383); ret_effect(gb); return; }
  CYC(b_+382, b_+383);
  CYC(b_+383, b_+386); SET_HL((wThreadStateBuffer + 14));
  CYC(b_+386, b_+387); alu_xor(gb, A);
  CYC(b_+387, b_+388); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+388, b_+389); mem_wr(gb, HL, A);
  CYC(b_+389, b_+391); A = 0xf0;
  CALL_C(b_+391, playSound_b00_hook, SYM(playSound_b00), b_+394);
  CYC(b_+394, b_+396); A = 0x3d;
  CYC(b_+396, b_+399);
  TAIL(setGlobalFlag);
}

void cutscene_disableLcdLoadRoomResetCamera_hook(GB *gb) {
  BASE(cutscene_disableLcdLoadRoomResetCamera);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+13);
  CYC(b_+3, b_+4); add_double_index_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); B = mem_rd(gb, HL);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CALL_C(b_+7, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+10);
  CYC(b_+10, b_+13);
  TAIL(resetCamera);
}

void func_6f9e_hook(GB *gb) {
  BASE(func_6f9e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0x70);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+7); hram_wr(gb, 0x70, A);
  CYC(b_+7, b_+10); SET_HL((w2TilesetBgPalettes + 16));
  CYC(b_+10, b_+12); B = 0x30;
  CALL_C(b_+12, clearMemory_hook, SYM(clearMemory), b_+15);
  CYC(b_+15, b_+16); SET_AF(pop_effect(gb));
  CYC(b_+16, b_+18); hram_wr(gb, 0x70, A);
  TAIL(func_6fb0);
}

void func_6fb0_hook(GB *gb) {
  BASE(func_6fb0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0x70);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+7); hram_wr(gb, 0x70, A);
  CYC(b_+7, b_+10); SET_HL(w2FadingBgPalettes);
  CYC(b_+10, b_+12); B = 0x80;
  CALL_C(b_+12, clearMemory_hook, SYM(clearMemory), b_+15);
  CYC(b_+15, b_+16); SET_AF(pop_effect(gb));
  CYC(b_+16, b_+18); hram_wr(gb, 0x70, A);
  CALL_C(b_+18, hideStatusBar_hook, SYM(hideStatusBar), b_+21);
  CYC(b_+21, b_+23); A = 0xfc;
  CYC(b_+23, b_+25); mem_wr(gb, hBgPaletteSources, A);
  CYC(b_+25, b_+27); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+31); mem_wr(gb, wScrollMode, A);
  CYC(b_+31, b_+34); mem_wr(gb, wGfxRegs1_LYC, A);
  CYC(b_+34, b_+37); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void cutscene_decCBB3whenFadeDone_hook(GB *gb) {
  BASE(cutscene_decCBB3whenFadeDone);
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); decCbb3_hook(gb);
}

void func_701d_hook(GB *gb) {
  BASE(func_701d);
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+5, b_+8);
    TAIL(wallRetraction_dungeon8);
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wCutsceneState);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+16) { func_701d__state0_hook(gb); return; }
    else if (jt_ == b_+54) { func_701d__state1_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_701d__state0_hook(GB *gb) {
  BASE(func_701d);
  CYC(b_+16, b_+18); A = 0x72;
  TAIL(func_701d__func_702f);
}

void func_701d__func_702f_hook(GB *gb) {
  BASE(func_701d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+18, loadGfxHeader_hook, SYM(loadGfxHeader), b_+21);
  CYC(b_+21, b_+23); B = 0x10;
  CYC(b_+23, b_+26); SET_HL(wTmpcbb3);
  CALL_C(b_+26, clearMemory_hook, SYM(clearMemory), b_+29);
  CALL_C(b_+29, reloadTileMap_hook, SYM(reloadTileMap), b_+32);
  CALL_C(b_+32, resetCamera_hook, SYM(resetCamera), b_+35);
  CALL_C(b_+35, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CALL_C(b_+40, loadTilesetAndRoomLayout_hook, SYM(loadTilesetAndRoomLayout), b_+43);
  CYC(b_+43, b_+45); A = 0x3c;
  CYC(b_+45, b_+48); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+52); mem_wr(gb, wScrollMode, A);
  CYC(b_+52, b_+54);
  TAIL(cutscene_incState);
}

void func_701d__state1_hook(GB *gb) {
  BASE(func_701d);
  CYC(b_+54, b_+57); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+57, b_+58); push_effect(gb, b_+58);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+62) { func_701d__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+70) { func_701d__cbb3_01_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_701d__cbb3_00_hook(GB *gb) {
  BASE(func_701d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+62, cutscene_tickDownCBB4ThenSetTo30_hook, SYM(cutscene_tickDownCBB4ThenSetTo30), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+66); ret_effect(gb); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x3c);
  CYC(b_+68, b_+70);
  TAIL(cutscene_incCBB3);
}

void func_701d__cbb3_01_hook(GB *gb) {
  BASE(func_701d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+70, b_+72); A = 0x3c;
  CALL_C(b_+72, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+75);
  CALL_C(b_+75, cutscene_tickDownCBB4ThenSetTo30_hook, SYM(cutscene_tickDownCBB4ThenSetTo30), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+79); ret_effect(gb); return; }
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x19);
  CYC(b_+81, b_+84); SET_HL((SYM(twinrovaRevealCutsceneHandler__func_6be8) + 9));
  CYC(b_+84, b_+86); E = 0x04;
  CALL_C(b_+86, interBankCall_hook, 0x008a, b_+89);
  CYC(b_+89, b_+92); SET_BC(0x260c);
  CALL_C(b_+92, func_70f7_hook, SYM(func_70f7), b_+95);
  CYC(b_+95, b_+96); alu_xor(gb, A);
  CYC(b_+96, b_+98); hram_wr(gb, 0x70, A);
  CALL_C(b_+98, reloadTileMap_hook, SYM(reloadTileMap), b_+101);
  CYC(b_+101, b_+103); A = 0x70;
  CALL_C(b_+103, playSound_b00_hook, SYM(playSound_b00), b_+106);
  CYC(b_+106, b_+109); SET_HL(wTmpcbb7);
  CYC(b_+109, b_+110); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+113); alu_cp(gb, 0x0f);
  if (F & FC) { CYCT(b_+113, b_+114); ret_effect(gb); return; }
  CYC(b_+113, b_+114);
  CALL_C(b_+114, func_7098_hook, SYM(func_7098), b_+117);
  CYC(b_+117, b_+119); A = 0x0f;
  CYC(b_+119, b_+122); mem_wr(gb, (wRoomCollisions + 93), A);
  CYC(b_+122, b_+123); ret_effect(gb);
}

void wallRetraction_dungeon8_hook(GB *gb) {
  BASE(wallRetraction_dungeon8);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+8) { wallRetraction_dungeon8__state0_hook(gb); return; }
    else if (jt_ == b_+13) { wallRetraction_dungeon8__state1_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void wallRetraction_dungeon8__state0_hook(GB *gb) {
  BASE(wallRetraction_dungeon8);
  CYC(b_+8, b_+10); A = 0x73;
  CYC(b_+10, b_+13);
  TAIL(func_701d__func_702f);
}

void wallRetraction_dungeon8__state1_hook(GB *gb) {
  BASE(wallRetraction_dungeon8);
  CYC(b_+13, b_+16); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == SYM(func_701d__cbb3_00) && hook_enabled_at(gb, SYM(func_701d__cbb3_00))) { func_701d__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+21) { wallRetraction_dungeon8__cbb3_01_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void wallRetraction_dungeon8__cbb3_01_hook(GB *gb) {
  BASE(wallRetraction_dungeon8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+21, b_+23); A = 0x3c;
  CALL_C(b_+23, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+26);
  CALL_C(b_+26, cutscene_tickDownCBB4ThenSetTo30_hook, SYM(cutscene_tickDownCBB4ThenSetTo30), b_+29);
  if (!(F & FZ)) { CYCT(b_+29, b_+30); ret_effect(gb); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x19);
  CYC(b_+32, b_+35); SET_HL((SYM(twinrovaRevealCutsceneHandler__func_6be8) + 9));
  CYC(b_+35, b_+37); E = 0x04;
  CALL_C(b_+37, interBankCall_hook, 0x008a, b_+40);
  CYC(b_+40, b_+43); SET_BC((SYM(intro_gotoTitlescreen) + 1));
  CALL_C(b_+43, func_70f7_hook, SYM(func_70f7), b_+46);
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CYC(b_+47, b_+49); hram_wr(gb, 0x70, A);
  CALL_C(b_+49, reloadTileMap_hook, SYM(reloadTileMap), b_+52);
  CYC(b_+52, b_+54); A = 0x70;
  CALL_C(b_+54, playSound_b00_hook, SYM(playSound_b00), b_+57);
  CYC(b_+57, b_+60); SET_HL(wTmpcbb7);
  CYC(b_+60, b_+61); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(b_+64, b_+65); ret_effect(gb); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+67);
  TAIL(func_7098);
}

void d2Collapse_incState_hook(GB *gb) {
  BASE(d2Collapse_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void d2Collapse_incCBB3_hook(GB *gb) {
  BASE(d2Collapse_incCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void func_7168_hook(GB *gb) {
  BASE(func_7168);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+14) { func_7168__state0_hook(gb); return; }
    else if (jt_ == b_+46) { func_7168__state1_hook(gb); return; }
    else if (jt_ == b_+73) { func_7168__state2_hook(gb); return; }
    else if (jt_ == b_+163) { func_7168__state3_hook(gb); return; }
    else if (jt_ == b_+170) { func_7168__state4_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_7168__state0_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+14, b_+16); B = 0x10;
  CYC(b_+16, b_+19); SET_HL(wTmpcbb3);
  CALL_C(b_+19, clearMemory_hook, SYM(clearMemory), b_+22);
  CALL_C(b_+22, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+25);
  CYC(b_+25, b_+27); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+27, b_+29); L = 0x73;
  CYC(b_+29, b_+31); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+35); mem_wr(gb, wScrollMode, A);
  CYC(b_+35, b_+37); A = 0x3c;
  CYC(b_+37, b_+40); mem_wr(gb, wTmpcbb4, A);
  CALL_C(b_+40, d2Collapse_incState_hook, SYM(d2Collapse_incState), b_+43);
  CYC(b_+43, b_+46);
  TAIL(reloadTileMap);
}

void func_7168__state1_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+46, d2Collapse_decCBB4_hook, SYM(d2Collapse_decCBB4), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+50); ret_effect(gb); return; }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, d2Collapse_incState_hook, SYM(d2Collapse_incState), b_+53);
  CALL_C(b_+53, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+56);
  if (!(F & FZ)) {
    CYCT(b_+56, b_+58);
  } else {
    CYC(b_+56, b_+58);
    CYC(b_+58, b_+60); mem_wr(gb, HL, 0x97);
    CYC(b_+60, b_+62); L = 0x4b;
    CYC(b_+62, b_+64); mem_wr(gb, HL, 0x2c);
    CYC(b_+64, b_+66); L = 0x4d;
    CYC(b_+66, b_+68); mem_wr(gb, HL, 0x58);
  }
  CYC(b_+68, b_+70); A = 0x50;
  CYC(b_+70, b_+73);
  TAIL(loadGfxHeader);
}

void func_7168__state2_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+73, b_+75); A = 0x0f;
  CALL_C(b_+75, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+78);
  CALL_C(b_+78, func_stub_hook, SYM(func_stub), b_+81);
  CYC(b_+81, b_+84); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+84, b_+85); push_effect(gb, b_+85);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+93) { func_7168__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+122) { func_7168__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+136) { func_7168__cbb3_02_hook(gb); return; }
    else if (jt_ == b_+140) { func_7168__cbb3_03_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_7168__cbb3_00_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+93, b_+96); SET_BC((SYM(func_03_7cb7__state5) + 2));
  CYC(b_+96, b_+99); SET_HL((SYM(func_03_7cb7__state3) + 1));
  CYC(b_+99, b_+101); E = 0x02;
  CALL_C(b_+101, interBankCall_hook, 0x008a, b_+104);
  TAIL(func_7168__func_71d0);
}

void func_7168__func_71d0_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+104, b_+106); A = 0x3c;
  CALL_C(b_+106, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+109);
  CYC(b_+109, b_+111); A = 0x70;
  CALL_C(b_+111, playSound_b00_hook, SYM(playSound_b00), b_+114);
  CYC(b_+114, b_+116); A = 0x1e;
  CYC(b_+116, b_+119); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+119, b_+122);
  TAIL(d2Collapse_incCBB3);
}

static void func_7168_cbb3_01_02_body(GB *gb, uint16_t sp0_) {
  BASE(func_7168);
  CALL_C(b_+124, d2Collapse_decCBB4_hook, SYM(d2Collapse_decCBB4), b_+127);
  if (!(F & FZ)) { CYCT(b_+127, b_+128); ret_effect(gb); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x1e);
  CYC(b_+130, b_+131); A = B;
  CALL_C(b_+131, loadGfxHeader_hook, SYM(loadGfxHeader), b_+134);
  CYC(b_+134, b_+136);
  func_7168__cbb3_00_hook(gb);
}

void func_7168__cbb3_01_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+122, b_+124); B = 0x51;
  func_7168_cbb3_01_02_body(gb, sp0_);
}

void func_7168__cbb3_02_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+136, b_+138); B = 0x52;
  CYC(b_+138, b_+140);
  func_7168_cbb3_01_02_body(gb, sp0_);
}

void func_7168__cbb3_03_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+140, d2Collapse_decCBB4_hook, SYM(d2Collapse_decCBB4), b_+143);
  if (!(F & FZ)) { CYCT(b_+143, b_+144); ret_effect(gb); return; }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+147); SET_HL((SYM(func_7bff) + 3));
  CYC(b_+147, b_+149); E = 0x02;
  CALL_C(b_+149, interBankCall_hook, 0x008a, b_+152);
  CALL_C(b_+152, func_7168__func_71d0_hook, b_+104, b_+155);
  CYC(b_+155, b_+157); A = 0x3c;
  CYC(b_+157, b_+160); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+160, b_+163);
  TAIL(d2Collapse_incState);
}

void func_7168__state3_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+163, d2Collapse_decCBB4_hook, SYM(d2Collapse_decCBB4), b_+166);
  if (!(F & FZ)) { CYCT(b_+166, b_+167); ret_effect(gb); return; }
  CYC(b_+166, b_+167);
  CYC(b_+167, b_+170);
  TAIL(d2Collapse_incState);
}

void func_7168__state4_hook(GB *gb) {
  BASE(func_7168);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+170, b_+172); A = 0x01;
  CYC(b_+172, b_+175); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+175, b_+177); A = 0x01;
  CYC(b_+177, b_+180); mem_wr(gb, wScrollMode, A);
  CYC(b_+180, b_+183); SET_HL((SYM(func_7e54) + 21));
  CALL_C(b_+183, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+186);
  CYC(b_+186, b_+187); alu_xor(gb, A);
  CYC(b_+187, b_+190); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+190, b_+193); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+193, b_+196); A = mem_rd(gb, wActiveMusic);
  CYC(b_+196, b_+199);
  TAIL(playSound_b00);
}

void func_stub_hook(GB *gb) {
  BASE(func_stub);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void func_7098_hook(GB *gb) {
  BASE(func_7098);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x4d;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+15); mem_wr(gb, wScrollMode, A);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+19, b_+22); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+22, loadTilesetAndRoomLayout_hook, SYM(loadTilesetAndRoomLayout), b_+25);
  CYC(b_+25, b_+28); loadRoomCollisions_hook(gb);
}

void func_70f7_hook(GB *gb) {
  BASE(func_70f7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); mem_wr(gb, hFF8C, A);
  CYC(b_+3, b_+4); A = B;
  CYC(b_+4, b_+6); mem_wr(gb, hFF8D, A);
  CYC(b_+6, b_+8); A = alu_swap(gb, A);
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+15); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+15, b_+16); alu_add(gb, E);
  CYC(b_+16, b_+18); mem_wr(gb, hFF93, A);
  CYC(b_+18, b_+20); C = 0x20;
  CALL_C(b_+20, multiplyAByC_hook, SYM(multiplyAByC), b_+23);
  CYC(b_+23, b_+26); SET_BC(w3VramTiles);
  CYC(b_+26, b_+28); A = mem_rd(gb, hFF8D);
  CYC(b_+28, b_+30); alu_and(gb, 0x0f);
  CALL_C(b_+30, addDoubleIndexToBc_hook, 0x007e, b_+33);
  CYC(b_+33, b_+34); alu_add_hl(gb, BC);
  CYC(b_+34, b_+36); A = mem_rd(gb, hFF8C);
  CYC(b_+36, b_+37); B = A;
  CYC(b_+37, b_+39); A = 0x20;
  CYC(b_+39, b_+40); alu_sub(gb, B);
  CYC(b_+40, b_+42); mem_wr(gb, hFF8E, A);
  CYC(b_+42, b_+43); push_effect(gb, HL);
  CYC(b_+43, b_+44); C = D;
  CYC(b_+44, b_+47); SET_DE(w1Link);
  CALL_C(b_+47, func_712f_hook, SYM(func_712f), b_+50);
  CYC(b_+50, b_+51); SET_HL(pop_effect(gb));
  CYC(b_+51, b_+53); H = (uint8_t)(H | 0x04);
  CYC(b_+53, b_+56); SET_DE(w1ParentItem4);
  TAIL(func_712f);
}

void func_712f_hook(GB *gb) {
  BASE(func_712f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hFF93);
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+5); A = 0x14;
  CYC(b_+5, b_+6); alu_sub(gb, C);
  if (F & FC) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); C = A;
  do {
    CYC(b_+8, b_+10); A = mem_rd(gb, hFF8C);
    CYC(b_+10, b_+11); B = A;
    do {
      CYC(b_+11, b_+13); A = 0x02;
      CYC(b_+13, b_+15); mem_wr(gb, IO_SVBK, A);
      CYC(b_+15, b_+16); A = mem_rd(gb, DE);
      CYC(b_+16, b_+17); SET_DE(DE + 1);
      CYC(b_+17, b_+19); mem_wr(gb, hFF8B, A);
      CYC(b_+19, b_+21); A = 0x03;
      CYC(b_+21, b_+23); mem_wr(gb, IO_SVBK, A);
      CYC(b_+23, b_+25); A = mem_rd(gb, hFF8B);
      CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+26, b_+27); B = alu_dec8(gb, B);
      if (!(F & FZ)) CYCT(b_+27, b_+29);
      else CYC(b_+27, b_+29);
    } while (!(F & FZ));
    CYC(b_+29, b_+31); A = mem_rd(gb, hFF8E);
    CALL_C(b_+31, addAToDe_hook, 0x0068, b_+34);
    CYC(b_+34, b_+36); A = mem_rd(gb, hFF8E);
    CYC(b_+36, b_+37); push_effect(gb, b_+37);
    burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L); L = A;
    burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
    if (!(F & FC)) {
      burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    } else {
      burn_rom(gb, 0x00, 0x0012, 0x0013, false);
      burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
      burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
    }
    CYC(b_+37, b_+38); C = alu_dec8(gb, C);
    if (!(F & FZ)) CYCT(b_+38, b_+40);
    else CYC(b_+38, b_+40);
  } while (!(F & FZ));
  CYC(b_+40, b_+41); ret_effect(gb);
}

void timewarpCutscene_incState_hook(GB *gb) {
  BASE(timewarpCutscene_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void timewarpCutscene_incCBB3_hook(GB *gb) {
  BASE(timewarpCutscene_incCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void func_03_7244_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+12) { func_03_7244__state0_hook(gb); return; }
    else if (jt_ == b_+89) { func_03_7244__state1_hook(gb); return; }
    else if (jt_ == b_+212) { func_03_7244__state2_hook(gb); return; }
    else if (jt_ == b_+366) { func_03_7244__state3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void timewarp_state0_after_call_727a_body(GB *gb, uint16_t sp0_) {
  BASE(func_03_7244);
  CYC(b_+54, b_+57); SET_HL((SYM(zeldaKidnappedState1__substate1) + 4));
  CYC(b_+57, b_+59); E = 0x06;
  CALL_C(b_+59, interBankCall_hook, 0x008a, b_+62);
  CYC(b_+62, b_+64); A = 0x6f;
  CALL_C(b_+64, loadGfxHeader_hook, SYM(loadGfxHeader), b_+67);
  CALL_C(b_+67, fastFadeoutToBlack_hook, SYM(fastFadeoutToBlack), b_+70);
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+74); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+74, b_+75); A = alu_dec8(gb, A);
  CYC(b_+75, b_+78); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+78, b_+81); SET_HL(wLoadedObjectGfx);
  CYC(b_+81, b_+83); B = 0x10;
  CALL_C(b_+83, clearMemory_hook, SYM(clearMemory), b_+86);
  CYC(b_+86, b_+89);
  hideStatusBar_hook(gb);
}

void func_03_7244__state0_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+12, b_+14); B = 0x10;
  CYC(b_+14, b_+17); SET_HL(wTmpcbb3);
  CALL_C(b_+17, clearMemory_hook, SYM(clearMemory), b_+20);
  CALL_C(b_+20, timewarpCutscene_incState_hook, SYM(timewarpCutscene_incState), b_+23);
  CALL_C(b_+23, stopTextThread_hook, SYM(stopTextThread), b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+30); SET_HL(wLoadedTreeGfxActive);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+37); mem_wr(gb, (wcc1f + 1), A);
  CYC(b_+37, b_+38); A = alu_dec8(gb, A);
  CYC(b_+38, b_+41); mem_wr(gb, wScrollMode, A);
  CYC(b_+41, b_+43); A = 0x08;
  CYC(b_+43, b_+46); mem_wr(gb, wTmpcbb7, A);
  CYC(b_+46, b_+49); SET_HL((SYM(oamDmaFunctionEnd) + 152));
  CYC(b_+49, b_+51); E = 0x3f;
  CALL_C(b_+51, interBankCall_hook, 0x008a, b_+54);
  timewarp_state0_after_call_727a_body(gb, sp0_);
}

void func_03_7244__state0__afterCall727a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  timewarp_state0_after_call_727a_body(gb, sp0_);
}

void func_03_7244__state1_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp;
  CYC(b_+89, b_+92); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+92, b_+93); push_effect(gb, b_+93);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+105) { func_03_7244__state1__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+116) { func_03_7244__state1__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+121) { func_03_7244__state1__cbb3_02_hook(gb); return; }
    else if (jt_ == b_+126) { func_03_7244__state1__cbb3_03_hook(gb); return; }
    else if (jt_ == b_+131) { func_03_7244__state1__cbb3_04_hook(gb); return; }
    else if (jt_ == b_+143) { func_03_7244__state1__cbb3_05_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void timewarp_state1_cbb3_00_03_body(GB *gb, uint16_t sp0_) {
  BASE(func_03_7244);
  CALL_C(b_+108, func_7431_hook, SYM(func_7431), b_+111);
  CALL_C(b_+111, func_745c_hook, SYM(func_745c), b_+114);
  CYC(b_+114, b_+116);
  timewarpCutscene_incCBB3_hook(gb);
}

void func_03_7244__state1__cbb3_00_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+105, b_+108); SET_HL(w1Link);
  timewarp_state1_cbb3_00_03_body(gb, sp0_);
}

void func_03_7244__state1__cbb3_01_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+116, b_+119); SET_HL(w1ParentItem4);
  CYC(b_+119, b_+121);
  timewarp_state1_cbb3_00_03_body(gb, sp0_);
}

void func_03_7244__state1__cbb3_02_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+121, b_+124); SET_HL(w2WaveScrollValues);
  CYC(b_+124, b_+126);
  timewarp_state1_cbb3_00_03_body(gb, sp0_);
}

void func_03_7244__state1__cbb3_03_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+126, b_+129); SET_HL(w1ReservedItemC);
  CYC(b_+129, b_+131);
  timewarp_state1_cbb3_00_03_body(gb, sp0_);
}

void func_03_7244__state1__cbb3_04_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+131, b_+134); SET_HL(w1Link);
  CALL_C(b_+134, func_7431_hook, SYM(func_7431), b_+137);
  CALL_C(b_+137, func_7456_hook, SYM(func_7456), b_+140);
  CYC(b_+140, b_+143);
  TAIL(timewarpCutscene_incCBB3);
}

void func_03_7244__state1__cbb3_05_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+143, b_+146); SET_HL(w1ParentItem4);
  CALL_C(b_+146, func_7431_hook, SYM(func_7431), b_+149);
  CALL_C(b_+149, func_7450_hook, SYM(func_7450), b_+152);
  CYC(b_+152, b_+155); SET_HL(wTmpcbb7);
  CYC(b_+155, b_+156); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+156, b_+158);
    TAIL(func_03_7244__state1__func_72ec);
  }
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+161); SET_HL(wTmpcbb8);
  CYC(b_+161, b_+162); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+162, b_+165); SET_HL(wTmpcbb3);
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x00);
  CYC(b_+167, b_+168); ret_effect(gb);
}

static void timewarp_after_call_730c_body(GB *gb) {
  BASE(func_03_7244);
  CYC(b_+200, b_+201); alu_xor(gb, A);
  CYC(b_+201, b_+204); mem_wr(gb, wcc20, A);
  CYC(b_+204, b_+207); SET_HL(wTmpcbb3);
  CYC(b_+207, b_+209); mem_wr(gb, HL, 0x00);
  CYC(b_+209, b_+212);
  timewarpCutscene_incState_hook(gb);
}

void func_03_7244__state1__func_72ec_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+168, b_+169); alu_xor(gb, A);
  CYC(b_+169, b_+171); hram_wr(gb, 0x70, A);
  CALL_C(b_+171, clearItems_hook, SYM(clearItems), b_+174);
  CALL_C(b_+174, clearEnemies_hook, SYM(clearEnemies), b_+177);
  CALL_C(b_+177, clearParts_hook, SYM(clearParts), b_+180);
  CALL_C(b_+180, clearReservedInteraction0_hook, SYM(clearReservedInteraction0), b_+183);
  CALL_C(b_+183, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+186);
  CYC(b_+186, b_+189); SET_DE(w1Companion);
  CALL_C(b_+189, objectDelete_de_hook, SYM(objectDelete_de), b_+192);
  CYC(b_+192, b_+194); A = (uint8_t)(w1Link >> 8);
  CYC(b_+194, b_+197); mem_wr(gb, wLinkObjectIndex, A);
  CALL_C(b_+197, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+200);
  timewarp_after_call_730c_body(gb);
}

void func_03_7244__afterCall730c_hook(GB *gb) {
  timewarp_after_call_730c_body(gb);
}

void func_03_7244__state2_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp;
  CYC(b_+212, b_+215); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+215, b_+216); push_effect(gb, b_+216);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+226) { func_03_7244__state2__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+268) { func_03_7244__state2__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+305) { func_03_7244__state2__cbb3_02_hook(gb); return; }
    else if (jt_ == b_+345) { func_03_7244__state2__cbb3_03_hook(gb); return; }
    else if (jt_ == b_+355) { func_03_7244__state2__cbb3_04_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_03_7244__state2__cbb3_00_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+226, b_+229); A = mem_rd(gb, wcddf);
  CYC(b_+229, b_+230); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+230, b_+232);
  } else {
    CYC(b_+230, b_+232);
    CYC(b_+232, b_+235); SET_HL((SYM(func_6ef7) + 16));
    CYC(b_+235, b_+237); E = 0x04;
    CALL_C(b_+237, interBankCall_hook, 0x008a, b_+240);
  }
  CYC(b_+240, b_+242); A = 0x03;
  CYC(b_+242, b_+244); hram_wr(gb, 0x70, A);
  CYC(b_+244, b_+247); SET_BC(0x02c0);
  CYC(b_+247, b_+250); SET_HL(w2WaveScrollValues);
  CALL_C(b_+250, clearMemoryBc_hook, SYM(clearMemoryBc), b_+253);
  CYC(b_+253, b_+256); SET_BC(0x02c0);
  CYC(b_+256, b_+259); SET_HL(w1ReservedItemC);
  CALL_C(b_+259, clearMemoryBc_hook, SYM(clearMemoryBc), b_+262);
  CALL_C(b_+262, reloadTileMap_hook, SYM(reloadTileMap), b_+265);
  CYC(b_+265, b_+268);
  TAIL(timewarpCutscene_incCBB3);
}

void func_03_7244__state2__cbb3_01_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+268, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+271);
  CYC(b_+271, b_+273); mem_wr(gb, HL, 0xdd);
  CYC(b_+273, b_+275); L = 0x46;
  CYC(b_+275, b_+277); A = 0x78;
  CYC(b_+277, b_+278); mem_wr(gb, HL, A);
  CYC(b_+278, b_+281); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+281, b_+284); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+284, b_+286); alu_and(gb, 0x80);
  CYC(b_+286, b_+288); A = 0x02;
  if (!(F & FZ)) CYCT(b_+288, b_+290);
  else {
    CYC(b_+288, b_+290);
    CYC(b_+290, b_+291); A = alu_dec8(gb, A);
  }
  CYC(b_+291, b_+293); L = 0x43;
  CYC(b_+293, b_+294); mem_wr(gb, HL, A);
  CYC(b_+294, b_+297); mem_wr(gb, wcc50, A);
  CYC(b_+297, b_+299); A = 0xd1;
  CALL_C(b_+299, playSound_b00_hook, SYM(playSound_b00), b_+302);
  CYC(b_+302, b_+305);
  TAIL(timewarpCutscene_incCBB3);
}

void func_03_7244__state2__cbb3_02_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+305, timewarpCutscene_decCBB4_hook, SYM(timewarpCutscene_decCBB4), b_+308);
  if (!(F & FZ)) { CYCT(b_+308, b_+309); ret_effect(gb); return; }
  CYC(b_+308, b_+309);
  CYC(b_+309, b_+311); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+311, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+314);
  if (!(F & FZ)) {
    CYCT(b_+314, b_+316);
  } else {
    CYC(b_+314, b_+316);
    CYC(b_+316, b_+318); mem_wr(gb, HL, 0xdd);
    CYC(b_+318, b_+319); L = alu_inc8(gb, L);
    CYC(b_+319, b_+321); mem_wr(gb, HL, 0x02);
    CYC(b_+321, b_+324); SET_DE(w1Link_yh);
    CALL_C(b_+324, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+327);
  }
  CYC(b_+327, b_+330); SET_DE(w1Link_yh);
  CALL_C(b_+330, getShortPositionFromDE_hook, SYM(getShortPositionFromDE), b_+333);
  CYC(b_+333, b_+336); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+336, b_+339); SET_DE(w1Link);
  CALL_C(b_+339, objectDelete_de_hook, SYM(objectDelete_de), b_+342);
  CYC(b_+342, b_+345);
  TAIL(timewarpCutscene_incCBB3);
}

void func_03_7244__state2__cbb3_03_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+345, timewarpCutscene_decCBB4_hook, SYM(timewarpCutscene_decCBB4), b_+348);
  if (!(F & FZ)) { CYCT(b_+348, b_+349); ret_effect(gb); return; }
  CYC(b_+348, b_+349);
  CALL_C(b_+349, fastFadeinFromBlack_hook, SYM(fastFadeinFromBlack), b_+352);
  CYC(b_+352, b_+355);
  TAIL(timewarpCutscene_incCBB3);
}

void func_03_7244__state2__cbb3_04_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+355, b_+358); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+358, b_+359); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+359, b_+360); ret_effect(gb); return; }
  CYC(b_+359, b_+360);
  CALL_C(b_+360, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+363);
  CYC(b_+363, b_+366);
  TAIL(timewarpCutscene_incState);
}

void func_03_7244__state3_hook(GB *gb) {
  BASE(func_03_7244);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+366, b_+369); A = mem_rd(gb, wcddf);
  CYC(b_+369, b_+370); alu_or(gb, A);
  if (!(F & FZ)) CYCT(b_+370, b_+372);
  else {
    CYC(b_+370, b_+372);
    CYC(b_+372, b_+375); SET_HL((SYM(func_6e9a) + 1));
    CYC(b_+375, b_+377); E = 0x04;
    CALL_C(b_+377, interBankCall_hook, 0x008a, b_+380);
  }
  CYC(b_+380, b_+383); A = mem_rd(gb, wActiveRoom);
  CYC(b_+383, b_+384); B = A;
  CYC(b_+384, b_+387); A = mem_rd(gb, wActiveGroup);
  CYC(b_+387, b_+389); alu_xor(gb, 0x01);
  CALL_C(b_+389, getRoomFlags_hook, SYM(getRoomFlags), b_+392);
  CYC(b_+392, b_+395); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+395, b_+398); SET_HL(wWarpDestGroup);
  CYC(b_+398, b_+401); A = mem_rd(gb, wActiveGroup);
  CYC(b_+401, b_+403); alu_xor(gb, 0x01);
  CYC(b_+403, b_+405); alu_or(gb, 0x80);
  CYC(b_+405, b_+406); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+406, b_+409); A = mem_rd(gb, wActiveRoom);
  CYC(b_+409, b_+410); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+410, b_+412); A = 0x06;
  CYC(b_+412, b_+413); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+413, b_+416); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+416, b_+417); mem_wr(gb, HL, A);
  CYC(b_+417, b_+420); SET_HL((SYM(func_03_6275__state1) + 60));
  CYC(b_+420, b_+422); E = 0x01;
  CALL_C(b_+422, interBankCall_hook, 0x008a, b_+425);
  CYC(b_+425, b_+427); C = alu_srl(gb, C);
  if (!(F & FC)) CYCT(b_+427, b_+429);
  else {
    CYC(b_+427, b_+429);
    CYC(b_+429, b_+432); A = mem_rd(gb, wTmpcbb9);
    CYC(b_+432, b_+435); mem_wr(gb, wWarpDestPos, A);
  }
  CYC(b_+435, b_+437); A = 0x03;
  CYC(b_+437, b_+440); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+440, b_+442); A = 0xff;
  CYC(b_+442, b_+445); mem_wr(gb, wActiveMusic, A);
  CYC(b_+445, b_+448); A = mem_rd(gb, wActiveRoom);
  CYC(b_+448, b_+451); SET_HL(b_+461);
  CALL_C(b_+451, checkFlag_hook, SYM(checkFlag), b_+454);
  if (F & FZ) { CYCT(b_+454, b_+455); ret_effect(gb); return; }
  CYC(b_+454, b_+455);
  CYC(b_+455, b_+457); A = 0x01;
  CYC(b_+457, b_+460); mem_wr(gb, wSentBackByStrangeForce, A);
  CYC(b_+460, b_+461); ret_effect(gb);
}

void func_7431_hook(GB *gb) {
  BASE(func_7431);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); A = mem_rd(gb, wTmpcbb8);
  CYC(b_+4, b_+6); alu_and(gb, 0x07);
  CYC(b_+6, b_+9); SET_HL(SYM(table_7440));
  CYC(b_+9, b_+10); add_double_index_to_hl(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); E = mem_rd(gb, HL);
  CYC(b_+12, b_+13); D = A;
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+15); ret_effect(gb);
}

void func_7450_hook(GB *gb) {
  BASE(func_7450);
  CYC(b_+0, b_+2); B = 0x2f;
  CYC(b_+2, b_+4); C = 0x06;
  CYC(b_+4, b_+6); func_7460_body(gb);
}

void func_7456_hook(GB *gb) {
  BASE(func_7456);
  CYC(b_+0, b_+2); B = 0x3f;
  CYC(b_+2, b_+4); C = 0x06;
  CYC(b_+4, b_+6); func_7460_body(gb);
}

void func_745c_hook(GB *gb) {
  BASE(func_745c);
  CYC(b_+0, b_+2); B = 0x3f;
  CYC(b_+2, b_+4); C = 0x05;
  func_7460_body(gb);
}

static void func_7460_body(GB *gb) {
  BASE(func_745c);
  CYC(b_+4, b_+5); push_effect(gb, BC);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CYC(b_+6, b_+7); A = C;
  CYC(b_+7, b_+9); mem_wr(gb, IO_SVBK, A);
  CYC(b_+9, b_+11); B = 0;
  do {
    CYC(b_+11, b_+12); A = mem_rd(gb, HL);
    CYC(b_+12, b_+13); alu_and(gb, D);
    CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+14, b_+15); A = mem_rd(gb, HL);
    CYC(b_+15, b_+16); alu_and(gb, D);
    CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+17, b_+18); A = mem_rd(gb, HL);
    CYC(b_+18, b_+19); alu_and(gb, E);
    CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+20, b_+21); A = mem_rd(gb, HL);
    CYC(b_+21, b_+22); alu_and(gb, E);
    CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+23, b_+24); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+24, b_+26);
    else CYC(b_+24, b_+26);
  } while (!(F & FZ));
  CYC(b_+26, b_+27); SET_HL(pop_effect(gb));
  CYC(b_+27, b_+28); SET_BC(pop_effect(gb));
  CYC(b_+28, b_+29); A = C;
  CYC(b_+29, b_+31); alu_sub(gb, 0x05);
  CYC(b_+31, b_+32); E = A;
  CYC(b_+32, b_+33); A = H;
  CYC(b_+33, b_+35); alu_and(gb, 0x8f);
  CYC(b_+35, b_+36); D = A;
  CYC(b_+36, b_+39); queueDmaTransfer_hook(gb);
}

void ambiPassageOpen_decCBB4_hook(GB *gb) {
  BASE(ambiPassageOpen_decCBB4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void ambiPassageOpen_incState_hook(GB *gb) {
  BASE(ambiPassageOpen_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void ambiPassageOpen_incCBB3_hook(GB *gb) {
  BASE(ambiPassageOpen_incCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void jabuOpen_decCBB4_hook(GB *gb) {
  BASE(jabuOpen_decCBB4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void jabuOpen_incState_hook(GB *gb) {
  BASE(jabuOpen_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void jabuOpen_incCBB3_hook(GB *gb) {
  BASE(jabuOpen_incCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void cleanSeas_decCBB4_hook(GB *gb) {
  BASE(cleanSeas_decCBB4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void cleanSeas_incState_hook(GB *gb) {
  BASE(cleanSeas_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void cleanSeas_incCBB3_hook(GB *gb) {
  BASE(cleanSeas_incCBB3);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void func_03_7493_hook(GB *gb) {
  BASE(func_03_7493);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+10) { func_03_7493__state0_hook(gb); return; }
    else if (jt_ == b_+75) { func_03_7493__state1_hook(gb); return; }
    else if (jt_ == b_+150) { func_03_7493__state2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_03_7493__state0_hook(GB *gb) {
  BASE(func_03_7493);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+10, b_+13); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); B = 0x08;
  CYC(b_+17, b_+20); SET_HL(wTmpcbb3);
  CALL_C(b_+20, clearMemory_hook, SYM(clearMemory), b_+23);
  CYC(b_+23, b_+25); A = 0x3c;
  CYC(b_+25, b_+28); mem_wr(gb, wTmpcbb4, A);
  CALL_C(b_+28, ambiPassageOpen_incState_hook, SYM(ambiPassageOpen_incState), b_+31);
  CALL_C(b_+31, disableLcd_hook, SYM(disableLcd), b_+34);
  CALL_C(b_+34, clearOam_hook, SYM(clearOam), b_+37);
  CALL_C(b_+37, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+40);
  CYC(b_+40, b_+43); SET_HL((SYM(verifyUnpackedSecretGameID) + 11));
  CYC(b_+43, b_+45); E = 0x01;
  CALL_C(b_+45, interBankCall_hook, 0x008a, b_+48);
  CALL_C(b_+48, stopTextThread_hook, SYM(stopTextThread), b_+51);
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CYC(b_+52, b_+55); SET_BC(0x0127);
  CALL_C(b_+55, forceLoadRoom_hook, SYM(forceLoadRoom), b_+58);
  CALL_C(b_+58, loadRoomCollisions_hook, SYM(loadRoomCollisions), b_+61);
  CALL_C(b_+61, func_131f_hook, SYM(func_131f), b_+64);
  CALL_C(b_+64, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+67);
  CALL_C(b_+67, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+70);
  CYC(b_+70, b_+72); A = 0x02;
  CYC(b_+72, b_+75);
  TAIL(loadGfxRegisterStateIndex);
}

void func_03_7493__state1_hook(GB *gb) {
  BASE(func_03_7493);
  uint16_t sp0_ = gb->sp;
  CYC(b_+75, b_+78); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+78, b_+79); push_effect(gb, b_+79);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+83) { func_03_7493__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+136) { func_03_7493__cbb3_01_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_03_7493__cbb3_00_hook(GB *gb) {
  BASE(func_03_7493);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+83, b_+86); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+87, b_+88); ret_effect(gb);
    return;
  }
  CYC(b_+87, b_+88);
  CALL_C(b_+88, ambiPassageOpen_decCBB4_hook, SYM(ambiPassageOpen_decCBB4), b_+91);
  if (!(F & FZ)) {
    CYCT(b_+91, b_+92); ret_effect(gb);
    return;
  }
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x3e);
  CYC(b_+94, b_+97); A = mem_rd(gb, wTmpcbbd);
  CYC(b_+97, b_+100); SET_HL(b_+128);
  CYC(b_+100, b_+101); add_double_index_to_hl(gb, b_+101);
  CYC(b_+101, b_+102); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+102, b_+103); B = mem_rd(gb, HL);
  CYC(b_+103, b_+104); C = A;
  CALL_C(b_+104, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+107);
  if (!(F & FZ)) {
    CYCT(b_+107, b_+108); ret_effect(gb);
    return;
  }
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x14);
  CYC(b_+110, b_+112); L = 0x49;
  CYC(b_+112, b_+113); mem_wr(gb, HL, B);
  CYC(b_+113, b_+115); L = 0x4b;
  CALL_C(b_+115, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+118);
  CYC(b_+118, b_+120); L = 0x4b;
  CYC(b_+120, b_+121); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+122, b_+124); L = 0x70;
  CYC(b_+124, b_+125); mem_wr(gb, HL, C);
  CYC(b_+125, b_+128); ambiPassageOpen_incCBB3_hook(gb);
}

void func_03_7493__cbb3_01_hook(GB *gb) {
  BASE(func_03_7493);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+136, ambiPassageOpen_decCBB4_hook, SYM(ambiPassageOpen_decCBB4), b_+139);
  if (!(F & FZ)) {
    CYCT(b_+139, b_+140); ret_effect(gb);
    return;
  }
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0x1e);
  CYC(b_+142, b_+144); A = 0x4d;
  CALL_C(b_+144, playSound_b00_hook, SYM(playSound_b00), b_+147);
  CYC(b_+147, b_+150); ambiPassageOpen_incState_hook(gb);
}

void func_03_7493__state2_hook(GB *gb) {
  BASE(func_03_7493);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+150, ambiPassageOpen_decCBB4_hook, SYM(ambiPassageOpen_decCBB4), b_+153);
  if (!(F & FZ)) {
    CYCT(b_+153, b_+154); ret_effect(gb);
    return;
  }
  CYC(b_+153, b_+154);
  CALL_C(b_+154, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+157);
  CYC(b_+157, b_+160); A = mem_rd(gb, wTmpcbbb);
  CYC(b_+160, b_+163); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+163, b_+164); L = A;
  CYC(b_+164, b_+166); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+166, b_+168); A = 0x81;
  CYC(b_+168, b_+171); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+171, b_+174); A = mem_rd(gb, wTmpcbbc);
  CYC(b_+174, b_+177); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+177, b_+179); A = 0x00;
  CYC(b_+179, b_+182); mem_wr(gb, wWarpTransition, A);
  CYC(b_+182, b_+184); A = 0x03;
  CYC(b_+184, b_+187); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+187, b_+188); alu_xor(gb, A);
  CYC(b_+188, b_+191); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+191, b_+194); fadeoutToWhite_hook(gb);
}

void jabuOpen_loadGfxAndPlaySound_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+103, loadGfxHeader_hook, SYM(loadGfxHeader), b_+106);
  CALL_C(b_+106, reloadTileMap_hook, SYM(reloadTileMap), b_+109);
  CYC(b_+109, b_+111); A = 0x70;
  CYC(b_+111, b_+114); playSound_b00_hook(gb);
}

void func_03_7565_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+10) { func_03_7565__state0_hook(gb); return; }
    else if (jt_ == b_+42) { func_03_7565__state1_hook(gb); return; }
    else if (jt_ == b_+132) { func_03_7565__state2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_03_7565__state0_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+10, b_+12); B = 0x10;
  CYC(b_+12, b_+15); SET_HL(wTmpcbb3);
  CALL_C(b_+15, clearMemory_hook, SYM(clearMemory), b_+18);
  CYC(b_+18, b_+21); SET_HL((SYM(func_03_6275__state1) + 44));
  CYC(b_+21, b_+23); E = 0x01;
  CALL_C(b_+23, interBankCall_hook, 0x008a, b_+26);
  CALL_C(b_+26, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CYC(b_+31, b_+33); A = 0x04;
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+36, b_+37); alu_xor(gb, A);
  CYC(b_+37, b_+40); mem_wr(gb, wScrollMode, A);
  CYC(b_+40, b_+42); jabuOpen_incState_hook(gb);
}

void func_03_7565__state1_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp;
  CYC(b_+42, b_+45); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+45, b_+46); push_effect(gb, b_+46);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+54) { func_03_7565__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+73) { func_03_7565__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+81) { func_03_7565__cbb3_02_hook(gb); return; }
    else if (jt_ == b_+114) { func_03_7565__cbb3_03_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void func_03_7565__cbb3_00_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+54, jabuOpen_decCBB4_hook, SYM(jabuOpen_decCBB4), b_+57);
  if (!(F & FZ)) {
    CYCT(b_+57, b_+58); ret_effect(gb);
    return;
  }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+60, reloadTileMap_hook, SYM(reloadTileMap), b_+63);
  CYC(b_+63, b_+66); SET_HL((SYM(fairyCutscene_cfd1is07__state5) + 20));
  CYC(b_+66, b_+68); E = 0x01;
  CALL_C(b_+68, interBankCall_hook, 0x008a, b_+71);
  CYC(b_+71, b_+73); jabuOpen_incCBB3_hook(gb);
}

void func_03_7565__cbb3_01_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+73, jabuOpen_decCBB4_hook, SYM(jabuOpen_decCBB4), b_+76);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+77); ret_effect(gb);
    return;
  }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x3c);
  CYC(b_+79, b_+81); jabuOpen_incCBB3_hook(gb);
}

void func_03_7565__cbb3_02_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+81, b_+83); A = 0x3c;
  CALL_C(b_+83, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+86);
  CALL_C(b_+86, jabuOpen_decCBB4_hook, SYM(jabuOpen_decCBB4), b_+89);
  if (!(F & FZ)) {
    CYCT(b_+89, b_+90); ret_effect(gb);
    return;
  }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+92, jabuOpen_incCBB3_hook, SYM(jabuOpen_incCBB3), b_+95);
  CYC(b_+95, b_+98); SET_BC(0x9701);
  CALL_C(b_+98, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+101);
  CYC(b_+101, b_+103); A = 0x74;
  jabuOpen_loadGfxAndPlaySound_hook(gb);
}

void func_03_7565__cbb3_03_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+114, b_+116); A = 0x3c;
  CALL_C(b_+116, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+119);
  CALL_C(b_+119, jabuOpen_decCBB4_hook, SYM(jabuOpen_decCBB4), b_+122);
  if (!(F & FZ)) {
    CYCT(b_+122, b_+123); ret_effect(gb);
    return;
  }
  CYC(b_+122, b_+123);
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+125, jabuOpen_incState_hook, SYM(jabuOpen_incState), b_+128);
  CYC(b_+128, b_+130); A = 0x75;
  CYC(b_+130, b_+132); jabuOpen_loadGfxAndPlaySound_hook(gb);
}

void func_03_7565__state2_hook(GB *gb) {
  BASE(func_03_7565);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+132, jabuOpen_decCBB4_hook, SYM(jabuOpen_decCBB4), b_+135);
  if (!(F & FZ)) {
    CYCT(b_+135, b_+136); ret_effect(gb);
    return;
  }
  CYC(b_+135, b_+136);
  CYC(b_+136, b_+138); A = 0x4d;
  CALL_C(b_+138, playSound_b00_hook, SYM(playSound_b00), b_+141);
  CYC(b_+141, b_+143); A = 0x01;
  CYC(b_+143, b_+146); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+146, b_+148); A = 0x01;
  CYC(b_+148, b_+151); mem_wr(gb, wScrollMode, A);
  CYC(b_+151, b_+152); alu_xor(gb, A);
  CYC(b_+152, b_+155); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+155, b_+158); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+158, loadTilesetAndRoomLayout_hook, SYM(loadTilesetAndRoomLayout), b_+161);
  CYC(b_+161, b_+164); loadRoomCollisions_hook(gb);
}

void func_03_7619__func_764a_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+49, b_+50); push_effect(gb, BC);
  CALL_C(b_+50, disableLcd_hook, SYM(disableLcd), b_+53);
  CYC(b_+53, b_+55); A = 0x0f;
  CALL_C(b_+55, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+58);
  CALL_C(b_+58, clearOam_hook, SYM(clearOam), b_+61);
  CALL_C(b_+61, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+64);
  CYC(b_+64, b_+67); SET_HL((SYM(verifyUnpackedSecretGameID) + 11));
  CYC(b_+67, b_+69); E = 0x01;
  CALL_C(b_+69, interBankCall_hook, 0x008a, b_+72);
  CALL_C(b_+72, stopTextThread_hook, SYM(stopTextThread), b_+75);
  CYC(b_+75, b_+77); A = 0x01;
  CYC(b_+77, b_+80); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+80, b_+83); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+83, b_+84); alu_xor(gb, A);
  CYC(b_+84, b_+85); SET_BC(pop_effect(gb));
  CALL_C(b_+85, forceLoadRoom_hook, SYM(forceLoadRoom), b_+88);
  CALL_C(b_+88, func_131f_hook, SYM(func_131f), b_+91);
  CALL_C(b_+91, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+94);
  CYC(b_+94, b_+96); A = 0x02;
  CYC(b_+96, b_+99); loadGfxRegisterStateIndex_hook(gb);
}

void func_03_7619__state0_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+18, b_+21); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+23); ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); B = 0x10;
  CYC(b_+25, b_+28); SET_HL(wTmpcbb3);
  CALL_C(b_+28, clearMemory_hook, SYM(clearMemory), b_+31);
  CALL_C(b_+31, clearScreenVariablesAndWramBank1_hook, SYM(clearScreenVariablesAndWramBank1), b_+34);
  CALL_C(b_+34, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+37);
  CYC(b_+37, b_+39); A = 0x0f;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CALL_C(b_+42, cleanSeas_incState_hook, SYM(cleanSeas_incState), b_+45);
  CYC(b_+45, b_+46); alu_xor(gb, A);
  CYC(b_+46, b_+49); SET_BC(0x01a5);
  TAIL(func_03_7619__func_764a);
}

void func_03_7619__state1__cbb3_00_hook(GB *gb) {
  BASE(func_03_7619);
  CYC(b_+111, b_+114); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+114, b_+115); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+115, b_+116); ret_effect(gb);
    return;
  }
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+118); A = 0xf0;
  CYC(b_+118, b_+121); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+121, b_+124); cleanSeas_incCBB3_hook(gb);
}

void func_03_7619__state1__cbb3_01_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+124, b_+127); A = mem_rd(gb, wFrameCounter);
  CYC(b_+127, b_+129); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+129, b_+131);
    goto after_sparkle;
  }
  CYC(b_+129, b_+131);
  CALL_C(b_+131, getFreePartSlot_hook, SYM(getFreePartSlot), b_+134);
  if (!(F & FZ)) {
    CYCT(b_+134, b_+136);
    goto after_sparkle;
  }
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x26);
  CALL_C(b_+138, getRandomNumber_hook, SYM(getRandomNumber), b_+141);
  CYC(b_+141, b_+143); alu_and(gb, 0x7f);
  CYC(b_+143, b_+144); C = A;
  CYC(b_+144, b_+146); L = 0xcb;
  CALL_C(b_+146, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+149);
after_sparkle:
  CYC(b_+149, b_+152); A = mem_rd(gb, wFrameCounter);
  CYC(b_+152, b_+154); alu_and(gb, 0x1f);
  CYC(b_+154, b_+156); A = 0x83;
  if (F & FZ) CALL_C_CC(b_+156, playSound_b00_hook, SYM(playSound_b00), b_+159);
  else CYC(b_+156, b_+159);
  CALL_C(b_+159, cleanSeas_decCBB4_hook, SYM(cleanSeas_decCBB4), b_+162);
  if (!(F & FZ)) {
    CYCT(b_+162, b_+163); ret_effect(gb);
    return;
  }
  CYC(b_+162, b_+163);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x78);
  CYC(b_+165, b_+167); A = 0x04;
  CALL_C(b_+167, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+170);
  CYC(b_+170, b_+172); A = 0xb4;
  CALL_C(b_+172, playSound_b00_hook, SYM(playSound_b00), b_+175);
  CYC(b_+175, b_+178); cleanSeas_incCBB3_hook(gb);
}

void func_03_7619__state1__cbb3_03_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+213, b_+214); ret_effect(gb);
    return;
  }
  CYC(b_+213, b_+214);
  CALL_C(b_+214, cleanSeas_decCBB4_hook, SYM(cleanSeas_decCBB4), b_+217);
  if (!(F & FZ)) {
    CYCT(b_+217, b_+218); ret_effect(gb);
    return;
  }
  CYC(b_+217, b_+218);
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+220, cleanSeas_incState_hook, SYM(cleanSeas_incState), b_+223);
  CYC(b_+223, b_+224); alu_xor(gb, A);
  CYC(b_+224, b_+227); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+227, b_+230); SET_BC(0x01d2);
  CYC(b_+230, b_+233); func_03_7619__func_764a_hook(gb);
}

void func_03_7619__state2__cbb3_03_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+245, b_+248); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+248, b_+249); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+249, b_+250); ret_effect(gb);
    return;
  }
  CYC(b_+249, b_+250);
  CALL_C(b_+250, cleanSeas_decCBB4_hook, SYM(cleanSeas_decCBB4), b_+253);
  if (!(F & FZ)) {
    CYCT(b_+253, b_+254); ret_effect(gb);
    return;
  }
  CYC(b_+253, b_+254);
  CYC(b_+254, b_+256); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+256, cleanSeas_incState_hook, SYM(cleanSeas_incState), b_+259);
  CYC(b_+259, b_+260); alu_xor(gb, A);
  CYC(b_+260, b_+263); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+263, b_+266); SET_BC(0x03b1);
  CALL_C(b_+266, func_03_7619__func_764a_hook, b_+49, b_+269);
  CYC(b_+269, b_+272); SET_HL((SYM(func_7e54) + 29));
  CYC(b_+272, b_+275); parseGivenObjectData_b00_hook(gb);
}

void func_03_7619__state3__cbb3_03_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+287, b_+290); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+290, b_+291); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+291, b_+292); ret_effect(gb);
    return;
  }
  CYC(b_+291, b_+292);
  CYC(b_+292, b_+295); SET_HL(wRoomLayoutEnd);
  CYC(b_+295, b_+297); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+297, b_+298); ret_effect(gb);
    return;
  }
  CYC(b_+297, b_+298);
  CYC(b_+298, b_+300); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CALL_C(b_+300, cleanSeas_incState_hook, SYM(cleanSeas_incState), b_+303);
  CYC(b_+303, b_+304); alu_xor(gb, A);
  CYC(b_+304, b_+307); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+307, b_+309); A = 0x3c;
  CYC(b_+309, b_+312); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+312, b_+315); SET_BC(0x03b0);
  CALL_C(b_+315, func_03_7619__func_764a_hook, b_+49, b_+318);
  CYC(b_+318, b_+321); SET_HL((SYM(func_7e54) + 39));
  CYC(b_+321, b_+324); parseGivenObjectData_b00_hook(gb);
}

void func_03_7619__state4__cbb3_03_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+336, b_+339); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+339, b_+340); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+340, b_+341); ret_effect(gb);
    return;
  }
  CYC(b_+340, b_+341);
  CYC(b_+341, b_+344); SET_HL(wRoomLayoutEnd);
  CYC(b_+344, b_+346); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+346, b_+347); ret_effect(gb);
    return;
  }
  CYC(b_+346, b_+347);
  CYC(b_+347, b_+349); A = 0x3c;
  CYC(b_+349, b_+352); mem_wr(gb, wTmpcbb4, A);
  CALL_C(b_+352, cleanSeas_incState_hook, SYM(cleanSeas_incState), b_+355);
  CYC(b_+355, b_+356); alu_xor(gb, A);
  CYC(b_+356, b_+359); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+359, b_+362); SET_BC(0x01a3);
  CALL_C(b_+362, func_03_7619__func_764a_hook, b_+49, b_+365);
  CYC(b_+365, b_+368); SET_HL(w1Link);
  CYC(b_+368, b_+370); mem_wr(gb, HL, 0x03);
  CYC(b_+370, b_+372); L = 0x0b;
  CYC(b_+372, b_+374); mem_wr(gb, HL, 0x38);
  CYC(b_+374, b_+376); L = 0x0d;
  CYC(b_+376, b_+378); mem_wr(gb, HL, 0x68);
  CYC(b_+378, b_+380); L = 0x08;
  CYC(b_+380, b_+382); mem_wr(gb, HL, 0x02);
  CYC(b_+382, b_+385); setLinkForceStateToState08_hook(gb);
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
  BASE(func_03_7619);
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (clean_seas_jump_table(gb));
    if (jt_ == b_+18) { func_03_7619__state0_hook(gb); return; }
    else if (jt_ == b_+99) { func_03_7619__state1_hook(gb); return; }
    else if (jt_ == b_+233) { func_03_7619__state2_hook(gb); return; }
    else if (jt_ == b_+275) { func_03_7619__state3_hook(gb); return; }
    else if (jt_ == b_+324) { func_03_7619__state4_hook(gb); return; }
    else if (jt_ == b_+385) { func_03_7619__state5_hook(gb); return; }
    else if (jt_ == b_+445) { func_03_7619__state6_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7619__state1_hook(GB *gb) {
  BASE(func_03_7619);
  CYC(b_+99, b_+102); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+102, b_+103); push_effect(gb, b_+103);
  do { uint16_t jt_ = (clean_seas_jump_table(gb));
    if (jt_ == b_+111) { func_03_7619__state1__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+124) { func_03_7619__state1__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+209) { func_03_7619__state1__cbb3_03_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7619__state2_hook(GB *gb) {
  BASE(func_03_7619);
  CYC(b_+233, b_+236); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+236, b_+237); push_effect(gb, b_+237);
  do { uint16_t jt_ = (clean_seas_jump_table(gb));
    if (jt_ == b_+111) { func_03_7619__state1__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+124) { func_03_7619__state1__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+245) { func_03_7619__state2__cbb3_03_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7619__state3_hook(GB *gb) {
  BASE(func_03_7619);
  CYC(b_+275, b_+278); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+278, b_+279); push_effect(gb, b_+279);
  do { uint16_t jt_ = (clean_seas_jump_table(gb));
    if (jt_ == b_+111) { func_03_7619__state1__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+124) { func_03_7619__state1__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+287) { func_03_7619__state3__cbb3_03_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7619__state4_hook(GB *gb) {
  BASE(func_03_7619);
  CYC(b_+324, b_+327); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+327, b_+328); push_effect(gb, b_+328);
  do { uint16_t jt_ = (clean_seas_jump_table(gb));
    if (jt_ == b_+111) { func_03_7619__state1__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+124) { func_03_7619__state1__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+336) { func_03_7619__state4__cbb3_03_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7619__state5_hook(GB *gb) {
  BASE(func_03_7619);
  CYC(b_+385, b_+388); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+388, b_+389); E = A;
  CYC(b_+389, b_+390); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+390, b_+392);
    goto dispatch;
  }
  CYC(b_+390, b_+392);
  CYC(b_+392, b_+395); A = mem_rd(gb, wFrameCounter);
  CYC(b_+395, b_+397); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(b_+397, b_+399);
    goto dispatch;
  }
  CYC(b_+397, b_+399);
  CYC(b_+399, b_+402); A = mem_rd(gb, w1Link_direction);
  CYC(b_+402, b_+404); alu_and(gb, 0x02);
  CYC(b_+404, b_+406); alu_xor(gb, 0x02);
  CYC(b_+406, b_+408); alu_or(gb, 0x01);
  CYC(b_+408, b_+411); mem_wr(gb, w1Link_direction, A);
dispatch:
  CYC(b_+411, b_+412); A = E;
  CYC(b_+412, b_+413); push_effect(gb, b_+413);
  do { uint16_t jt_ = (clean_seas_jump_table(gb));
    if (jt_ == b_+111) { func_03_7619__state1__cbb3_00_hook(gb); return; }
    else if (jt_ == b_+124) { func_03_7619__state1__cbb3_01_hook(gb); return; }
    else if (jt_ == b_+421) { func_03_7619__state5__cbb3_03_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void func_03_7619__state5__cbb3_03_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+421, b_+424); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+424, b_+425); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+425, b_+426); ret_effect(gb);
    return;
  }
  CYC(b_+425, b_+426);
  CYC(b_+426, b_+429); SET_HL(wRoomLayoutEnd);
  CYC(b_+429, b_+431); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+431, b_+432); ret_effect(gb);
    return;
  }
  CYC(b_+431, b_+432);
  CYC(b_+432, b_+434); A = 0x3c;
  CYC(b_+434, b_+437); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+437, b_+439); A = 0x5b;
  CALL_C(b_+439, playSound_b00_hook, SYM(playSound_b00), b_+442);
  CYC(b_+442, b_+445); cleanSeas_incState_hook(gb);
}

void func_03_7619__state6_hook(GB *gb) {
  BASE(func_03_7619);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+445, cleanSeas_decCBB4_hook, SYM(cleanSeas_decCBB4), b_+448);
  if (!(F & FZ)) {
    CYCT(b_+448, b_+449); ret_effect(gb);
    return;
  }
  CYC(b_+448, b_+449);
  CYC(b_+449, b_+451); A = 0x01;
  CYC(b_+451, b_+454); mem_wr(gb, wScrollMode, A);
  CYC(b_+454, b_+457); SET_HL((SYM(endgameCutsceneHandler_0a__state1__substate0) + 105));
  CYC(b_+457, b_+459); E = 0x01;
  CALL_C(b_+459, interBankCall_hook, 0x008a, b_+462);
  CYC(b_+462, b_+465); SET_HL((SYM(generateGameIDIfNeeded) + 11));
  CYC(b_+465, b_+467); E = 0x01;
  CALL_C(b_+467, interBankCall_hook, 0x008a, b_+470);
  CYC(b_+470, b_+473); SET_HL((SYM(generateGameIDIfNeeded) + 25));
  CYC(b_+473, b_+475); E = 0x01;
  CALL_C(b_+475, interBankCall_hook, 0x008a, b_+478);
  CYC(b_+478, b_+479); alu_xor(gb, A);
  CYC(b_+479, b_+482); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+482, b_+485); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+485, b_+488); A = mem_rd(gb, wLoadingRoomPack);
  CYC(b_+488, b_+491); mem_wr(gb, wRoomPack, A);
  CYC(b_+491, b_+494); A = mem_rd(gb, wActiveRoom);
  CYC(b_+494, b_+497); mem_wr(gb, wLoadingRoom, A);
  CYC(b_+497, b_+499); A = 0x36;
  CYC(b_+499, b_+502); mem_wr(gb, wEnteredWarpPosition, A);
  CYC(b_+502, b_+505); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+505, b_+508); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+508, playSound_b00_hook, SYM(playSound_b00), b_+511);
  CYC(b_+511, b_+513); A = 0x00;
  CYC(b_+513, b_+516); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+516, b_+518); A = 0x02;
  CYC(b_+518, b_+521); mem_wr(gb, w1Link_direction, A);
  CYC(b_+521, b_+523); A = 0x30;
  CALL_C(b_+523, setGlobalFlag_hook, SYM(setGlobalFlag), b_+526);
  CYC(b_+526, b_+529); setDeathRespawnPoint_hook(gb);
}

void nayru_singing_after_states_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  CYC(b_+3, b_+6); SET_HL(wCutsceneState);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16);
  TAIL(updateAllObjects);
}

void nayruSingingCutsceneHandler__afterCall631b_hook(GB *gb) {
  nayru_singing_after_states_hook(gb);
}

void nayruSingingCutsceneHandler__runStates_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  CYC(b_+16, b_+19); SET_DE(wCutsceneState);
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+57) { nayruSingingCutsceneHandler__state0_hook(gb); return; }
    else if (jt_ == b_+65) { nayruSingingCutsceneHandler__state1_hook(gb); return; }
    else if (jt_ == b_+146) { nayruSingingCutsceneHandler__state2_hook(gb); return; }
    else if (jt_ == b_+213) { nayruSingingCutsceneHandler__state3_hook(gb); return; }
    else if (jt_ == b_+246) { nayruSingingCutsceneHandler__state4_hook(gb); return; }
    else if (jt_ == b_+264) { nayruSingingCutsceneHandler__state5_hook(gb); return; }
    else if (jt_ == b_+282) { nayruSingingCutsceneHandler__state6_hook(gb); return; }
    else if (jt_ == b_+299) { nayruSingingCutsceneHandler__state7_hook(gb); return; }
    else if (jt_ == b_+316) { nayruSingingCutsceneHandler__state8_hook(gb); return; }
    else if (jt_ == b_+348) { nayruSingingCutsceneHandler__state9_hook(gb); return; }
    else if (jt_ == b_+368) { nayruSingingCutsceneHandler__stateA_hook(gb); return; }
    else if (jt_ == b_+382) { nayruSingingCutsceneHandler__stateB_hook(gb); return; }
    else if (jt_ == b_+397) { nayruSingingCutsceneHandler__stateC_hook(gb); return; }
    else if (jt_ == b_+413) { nayruSingingCutsceneHandler__stateD_hook(gb); return; }
    else if (jt_ == SYM(nayruSingingStateE) && hook_enabled_at(gb, SYM(nayruSingingStateE))) { nayruSingingStateE_hook(gb); return; }
    else if (jt_ == SYM(nayruSingingStateF) && hook_enabled_at(gb, SYM(nayruSingingStateF))) { nayruSingingStateF_hook(gb); return; }
    else if (jt_ == SYM(nayruSingingState10) && hook_enabled_at(gb, SYM(nayruSingingState10))) { nayruSingingState10_hook(gb); return; }
    else if (jt_ == SYM(nayruSingingState11) && hook_enabled_at(gb, SYM(nayruSingingState11))) { nayruSingingState11_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void nayruSingingCutsceneHandler_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruSingingCutsceneHandler__runStates_hook, b_+16, b_+3);
  nayru_singing_after_states_hook(gb);
}

void nayru_singing_state_d_after_room_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  CYC(b_+424, b_+426); A = 0x02;
  CYC(b_+426, b_+429);
  TAIL(loadGfxRegisterStateIndex);
}

void nayruSingingCutsceneHandler__afterCall64c0_hook(GB *gb) {
  nayru_singing_state_d_after_room_hook(gb);
}

void nayruSingingCutsceneHandler__stateD_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp;
  CYC(b_+413, b_+416); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+416, b_+417); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+417, b_+418); ret_effect(gb); return; }
  CYC(b_+417, b_+418);
  CALL_C(b_+418, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+421);
  CALL_C(b_+421, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+424);
  nayru_singing_state_d_after_room_hook(gb);
}

void nayru_singing_state_f_tail_hook(GB *gb) {
  BASE(nayruSingingStateF);
  CYC(b_+53, b_+55); A = 0x02;
  CYC(b_+55, b_+58);
  TAIL(loadGfxRegisterStateIndex);
}

void nayru_singing_state_f_after_room_hook(GB *gb) {
  BASE(nayruSingingStateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+11, b_+13); A = 0x99;
  CALL_C(b_+13, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+16);
  CYC(b_+16, b_+18); A = 0x08;
  CALL_C(b_+18, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+21);
  CYC(b_+21, b_+23); L = (uint8_t)w1Link_enabled;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x03);
  CYC(b_+25, b_+27); L = (uint8_t)w1Link_subid;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x04);
  CYC(b_+29, b_+31); A = 0xfb;
  CALL_C(b_+31, playSound_b00_hook, SYM(playSound_b00), b_+34);
  CYC(b_+34, b_+36); A = 0x1f;
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+43); mem_wr(gb, wPaletteThread_parameter, A);
  CYC(b_+43, b_+45); A = 0x24;
  CYC(b_+45, b_+47); B = 0x02;
  CALL_C(b_+47, cutscene_loadAObjectGfxBTimes_hook, SYM(cutscene_loadAObjectGfxBTimes), b_+50);
  CALL_C(b_+50, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+53);
  nayru_singing_state_f_tail_hook(gb);
}

void nayruSingingStateF__afterCall6509_hook(GB *gb) {
  nayru_singing_state_f_after_room_hook(gb);
}

void nayruSingingStateF__afterCall6533_hook(GB *gb) {
  nayru_singing_state_f_tail_hook(gb);
}

void nayruSingingStateF_hook(GB *gb) {
  BASE(nayruSingingStateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+8);
  CALL_C(b_+8, cutscene_loadRoomObjectSetAndFadein_hook, SYM(cutscene_loadRoomObjectSetAndFadein), b_+11);
  nayru_singing_state_f_after_room_hook(gb);
}

void miscCutsceneHandler_hook(GB *gb) {
  BASE(miscCutsceneHandler);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == SYM(nayruSingingCutsceneHandler) && hook_enabled_at(gb, SYM(nayruSingingCutsceneHandler))) { nayruSingingCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(makuTreeDisappearingCutsceneHandler) && hook_enabled_at(gb, SYM(makuTreeDisappearingCutsceneHandler))) { makuTreeDisappearingCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(blackTowerExplanationCutsceneHandler) && hook_enabled_at(gb, SYM(blackTowerExplanationCutsceneHandler))) { blackTowerExplanationCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(nayruWarpToMakuTreeCutsceneHandler) && hook_enabled_at(gb, SYM(nayruWarpToMakuTreeCutsceneHandler))) { nayruWarpToMakuTreeCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(blackTowerCompleteCutsceneHandler) && hook_enabled_at(gb, SYM(blackTowerCompleteCutsceneHandler))) { blackTowerCompleteCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(turnToStoneCutsceneHandler) && hook_enabled_at(gb, SYM(turnToStoneCutsceneHandler))) { turnToStoneCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(twinrovaRevealCutsceneHandler) && hook_enabled_at(gb, SYM(twinrovaRevealCutsceneHandler))) { twinrovaRevealCutsceneHandler_hook(gb); return; }
    else if (jt_ == SYM(pregameIntroCutsceneHandler) && hook_enabled_at(gb, SYM(pregameIntroCutsceneHandler))) { pregameIntroCutsceneHandler_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void nayruSingingCutsceneHandler__state0_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  CYC(b_+57, b_+59); A = 0x01;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+62); A = 0x55;
  CYC(b_+62, b_+65); playSound_b00_hook(gb);
}

void nayruSingingCutsceneHandler__state1_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+65, b_+67); A = 0xff;
  CYC(b_+67, b_+70); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+70, b_+72); A = 0x08;
  CYC(b_+72, b_+75); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+75, b_+78); SET_HL(wMenuDisabled);
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x01);
  CYC(b_+80, b_+83); SET_HL(w1Link_visible);
  CYC(b_+83, b_+85); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+85, saveGraphicsOnEnterMenu_hook, SYM(saveGraphicsOnEnterMenu), b_+88);
  CYC(b_+88, b_+90); A = 0x0c;
  CALL_C(b_+90, loadGfxHeader_hook, SYM(loadGfxHeader), b_+93);
  CYC(b_+93, b_+95); A = 0x95;
  CALL_C(b_+95, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+98);
  CYC(b_+98, b_+100); A = 0x04;
  CALL_C(b_+100, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+103);
  CYC(b_+103, b_+106); SET_HL(wTmpcbb3);
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x58);
  CYC(b_+108, b_+109); SET_HL(HL + 1);
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x02);
  CYC(b_+111, b_+114); SET_HL(wTmpcbb6);
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x28);
  CALL_C(b_+116, fastFadeinFromWhite_hook, SYM(fastFadeinFromWhite), b_+119);
  CALL_C(b_+119, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+122);
  CYC(b_+122, b_+125); SET_HL(wTmpcbb5);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x02);
  TAIL(nayruSingingCutsceneHandler__func_6397);
}

void nayruSingingCutsceneHandler__state2_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+146, b_+149); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+149, b_+150); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+150, b_+153);
    TAIL(nayruSingingCutsceneHandler__func_6397);
  }
  CYC(b_+150, b_+153);
  if (!(F & FZ)) {
    CYCT(b_+153, b_+154); ret_effect(gb);
    return;
  }
  CYC(b_+153, b_+154);
  CALL_C(b_+154, nayruSingingCutsceneHandler__func_63db_hook, b_+195, b_+157);
  CALL_C(b_+157, nayruSingingCutsceneHandler__func_6397_hook, b_+127, b_+160);
  CYC(b_+160, b_+163); SET_HL(wTmpcbb3);
  CALL_C(b_+163, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+166);
  if (F & FZ) {
    CYCT(b_+166, b_+168);
    goto finish;
  }
  CYC(b_+166, b_+168);
  CYC(b_+168, b_+169); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+169, b_+170); H = mem_rd(gb, HL);
  CYC(b_+170, b_+171); L = A;
  CYC(b_+171, b_+174); SET_BC(0x00f0);
  CALL_C(b_+174, compareHlToBc_hook, SYM(compareHlToBc), b_+177);
  if (!(F & FC)) {
    CYCT(b_+177, b_+178); ret_effect(gb);
    return;
  }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+181); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+181, b_+183); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+183, b_+184); ret_effect(gb);
    return;
  }
  CYC(b_+183, b_+184);
finish:
  CYC(b_+184, b_+186); A = 0x55;
  CALL_C(b_+186, playSound_b00_hook, SYM(playSound_b00), b_+189);
  CALL_C(b_+189, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+192);
  CYC(b_+192, b_+195); fastFadeoutToWhite_hook(gb);
}

void nayruSingingCutsceneHandler__state3_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+213, b_+216); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+216, b_+217); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+217, b_+220);
    TAIL(nayruSingingCutsceneHandler__func_6397);
  }
  CYC(b_+217, b_+220);
  if (!(F & FZ)) {
    CYCT(b_+220, b_+221); ret_effect(gb);
    return;
  }
  CYC(b_+220, b_+221);
  CYC(b_+221, b_+222); alu_xor(gb, A);
  CYC(b_+222, b_+225); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+225, b_+228); SET_HL(w1Link_visible);
  CYC(b_+228, b_+230); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+230, b_+232); A = 0x09;
  CYC(b_+232, b_+235); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CALL_C(b_+235, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+238);
  CYC(b_+238, b_+241); SET_HL(wTmpcbb3);
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0xaa);
  CYC(b_+243, b_+246); reloadGraphicsOnExitMenu_hook(gb);
}

void nayruSingingCutsceneHandler__state4_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+246, b_+249); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+249, b_+251); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+251, b_+252); ret_effect(gb);
    return;
  }
  CYC(b_+251, b_+252);
  CALL_C(b_+252, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+255);
  CYC(b_+255, b_+258); SET_HL((w2TilesetBgPalettes + 16));
  CYC(b_+258, b_+261); SET_BC((SYM(oamDmaFunctionEnd) + 1037));
  CYC(b_+261, b_+264); func_13c6_hook(gb);
}

void nayruSingingCutsceneHandler__state5_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+264, b_+267); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+267, b_+268); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+268, b_+269); ret_effect(gb);
    return;
  }
  CYC(b_+268, b_+269);
  CYC(b_+269, b_+271); A = 0x99;
  CALL_C(b_+271, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+274);
  CYC(b_+274, b_+276); A = 0x10;
  CYC(b_+276, b_+279); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+279, b_+282); cutscene_incCutsceneState_hook(gb);
}

void nayruSingingCutsceneHandler__state6_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+282, b_+285); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+285, b_+287); alu_cp(gb, 0x14);
  if (!(F & FZ)) {
    CYCT(b_+287, b_+288); ret_effect(gb);
    return;
  }
  CYC(b_+287, b_+288);
  CALL_C(b_+288, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+291);
  CYC(b_+291, b_+294); SET_HL(wTmpcbb3);
  CYC(b_+294, b_+296); mem_wr(gb, HL, 0x3c);
  CYC(b_+296, b_+299); fadeoutToWhite_hook(gb);
}

void nayruSingingCutsceneHandler__state7_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+299, cutscene_decCBB3IfNotFadingOut_hook, SYM(cutscene_decCBB3IfNotFadingOut), b_+302);
  if (!(F & FZ)) {
    CYCT(b_+302, b_+303); ret_effect(gb);
    return;
  }
  CYC(b_+302, b_+303);
  CALL_C(b_+303, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+306);
  CYC(b_+306, b_+308); A = 0x15;
  CYC(b_+308, b_+311); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+311, b_+313); A = 0x03;
  CYC(b_+313, b_+316); fadeinFromWhiteWithDelay_hook(gb);
}

void nayruSingingCutsceneHandler__state8_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+316, b_+319); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+319, b_+321); alu_cp(gb, 0x18);
  if (!(F & FZ)) {
    CYCT(b_+321, b_+322); ret_effect(gb);
    return;
  }
  CYC(b_+321, b_+322);
  CALL_C(b_+322, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+325);
  CYC(b_+325, b_+326); alu_xor(gb, A);
  CYC(b_+326, b_+329); mem_wr(gb, wTmpcfc0_fairyHideAndSeek_cfd2, A);
  CALL_C(b_+329, getFreePartSlot_hook, SYM(getFreePartSlot), b_+332);
  if (!(F & FZ)) {
    CYCT(b_+332, b_+333); ret_effect(gb);
    return;
  }
  CYC(b_+332, b_+333);
  CYC(b_+333, b_+335); mem_wr(gb, HL, 0x27);
  CYC(b_+335, b_+336); L = alu_inc8(gb, L);
  CYC(b_+336, b_+337); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+337, b_+338); L = alu_inc8(gb, L);
  CYC(b_+338, b_+339); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+339, b_+341); L = 0xcb;
  CYC(b_+341, b_+343); mem_wr(gb, HL, 0x24);
  CYC(b_+343, b_+345); L = 0xcd;
  CYC(b_+345, b_+347); mem_wr(gb, HL, 0x28);
  CYC(b_+347, b_+348); ret_effect(gb);
}

void nayruSingingCutsceneHandler__state9_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+348, b_+351); A = mem_rd(gb, wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+351, b_+352); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+352, b_+353); ret_effect(gb);
    return;
  }
  CYC(b_+352, b_+353);
  CALL_C(b_+353, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+356);
  CALL_C(b_+356, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+359);
  CYC(b_+359, b_+361); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+361, b_+363); C = 0x22;
  CYC(b_+363, b_+365); A = 0xd7;
  CYC(b_+365, b_+368); setTile_hook(gb);
}

void nayruSingingCutsceneHandler__stateA_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+368, b_+371); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+371, b_+373); alu_cp(gb, 0x1d);
  if (!(F & FZ)) {
    CYCT(b_+373, b_+374); ret_effect(gb);
    return;
  }
  CYC(b_+373, b_+374);
  CYC(b_+374, b_+377); SET_HL(wTmpcbb3);
  CYC(b_+377, b_+379); mem_wr(gb, HL, 0x78);
  CYC(b_+379, b_+382); cutscene_incCutsceneState_hook(gb);
}

void nayruSingingCutsceneHandler__stateB_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+382, decCbb3_hook, SYM(decCbb3), b_+385);
  if (!(F & FZ)) {
    CYCT(b_+385, b_+386); ret_effect(gb);
    return;
  }
  CYC(b_+385, b_+386);
  CYC(b_+386, b_+388); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+388, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+391);
  CYC(b_+391, b_+394); SET_BC((SYM(endgameCutsceneHandler_09_stage0__stateE) + 8));
  CYC(b_+394, b_+397); showText_hook(gb);
}

void nayruSingingCutsceneHandler__stateC_hook(GB *gb) {
  BASE(nayruSingingCutsceneHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+397, cutscene_decCBB3IfTextNotActive_hook, SYM(cutscene_decCBB3IfTextNotActive), b_+400);
  if (!(F & FZ)) {
    CYCT(b_+400, b_+401); ret_effect(gb);
    return;
  }
  CYC(b_+400, b_+401);
  CALL_C(b_+401, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+404);
  CYC(b_+404, b_+405); alu_xor(gb, A);
  CYC(b_+405, b_+408); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+408, b_+409); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+409, b_+410); mem_wr(gb, HL, A);
  CYC(b_+410, b_+413); fadeoutToWhite_hook(gb);
}

void nayruSingingStateE_hook(GB *gb) {
  BASE(nayruSingingStateE);
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_cp(gb, 0x03);
  CYC(b_+18, b_+20); A = 0x0d;
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
  } else {
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+24); A = 0x0f;
  }
  CYC(b_+24, b_+27); SET_HL(wCutsceneState);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+31); fadeoutToWhite_hook(gb);
}

void nayruSingingState10_hook(GB *gb) {
  BASE(nayruSingingState10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+8, b_+10); alu_cp(gb, 0x1e);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, cutscene_incCutsceneState_hook, SYM(cutscene_incCutsceneState), b_+14);
  CYC(b_+14, b_+17); SET_HL((w2TilesetBgPalettes + 16));
  CYC(b_+17, b_+20); SET_BC((SYM(loadSecretBufferFromText__textCharacterToByte) + 7));
  CYC(b_+20, b_+23); func_13c6_hook(gb);
}

void nayruSingingState11_hook(GB *gb) {
  BASE(nayruSingingState11);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x10;
  CALL_C(b_+7, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+10);
  CYC(b_+10, b_+12); A = 0x1f;
  CYC(b_+12, b_+15); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+20, b_+21); ret_effect(gb);
}

void blackTowerEscapeAttempt_incState_hook(GB *gb) {
  BASE(blackTowerEscapeAttempt_incState);
  CYC(b_+0, b_+3); SET_HL(wCutsceneState);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void blackTowerEscapeAttempt_decCBB4_hook(GB *gb) {
  BASE(blackTowerEscapeAttempt_decCBB4);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb4);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void blackTowerEscapeAttempt_loadNewRoom_hook(GB *gb) {
  BASE(blackTowerEscapeAttempt_loadNewRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, disableLcd_hook, SYM(disableLcd), b_+3);
  CALL_C(b_+3, loadScreenMusicAndSetRoomPack_hook, SYM(loadScreenMusicAndSetRoomPack), b_+6);
  CALL_C(b_+6, loadTilesetData_hook, SYM(loadTilesetData), b_+9);
  CALL_C(b_+9, loadTilesetGraphics_hook, SYM(loadTilesetGraphics), b_+12);
  CYC(b_+12, b_+15);
  TAIL(func_131f);
}

void func_03_7cb7_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneState);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_cutscene_jump_table(gb));
    if (jt_ == b_+18) { func_03_7cb7__state0_hook(gb); return; }
    else if (jt_ == b_+93) { func_03_7cb7__state1_hook(gb); return; }
    else if (jt_ == b_+134) { func_03_7cb7__state2_hook(gb); return; }
    else if (jt_ == b_+180) { func_03_7cb7__state3_hook(gb); return; }
    else if (jt_ == b_+222) { func_03_7cb7__state4_hook(gb); return; }
    else if (jt_ == b_+296) { func_03_7cb7__state5_hook(gb); return; }
    else if (jt_ == b_+360) { func_03_7cb7__state6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void black_tower_escape_after_call_7ce0_body(GB *gb, uint16_t sp0_) {
  BASE(func_03_7cb7);
  CYC(b_+41, b_+43); A = 0x01;
  CYC(b_+43, b_+46); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+46, b_+49); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+49, b_+51); A = 0x3c;
  CYC(b_+51, b_+54); mem_wr(gb, wTmpcbb4, A);
  CALL_C(b_+54, blackTowerEscapeAttempt_incState_hook, SYM(blackTowerEscapeAttempt_incState), b_+57);
  CYC(b_+57, b_+60); SET_HL(w1Link_enabled);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x03);
  CYC(b_+62, b_+64); L = 0x0b;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x58);
  CYC(b_+66, b_+67); L = alu_inc8(gb, L);
  CYC(b_+67, b_+68); L = alu_inc8(gb, L);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x78);
  CYC(b_+70, b_+72); L = 0x08;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x02);
  CALL_C(b_+74, resetCamera_hook, SYM(resetCamera), b_+77);
  CYC(b_+77, b_+79); A = 0x00;
  CYC(b_+79, b_+82); mem_wr(gb, wScrollMode, A);
  CYC(b_+82, b_+85); SET_HL((SYM(func_7e54) + 49));
  CALL_C(b_+85, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+88);
  CYC(b_+88, b_+90); A = 0x04;
  CYC(b_+90, b_+93);
  fadeinFromWhiteWithDelay_hook(gb);
}

void func_03_7cb7__state0_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+18, b_+21); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+21, b_+24); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+24, playSound_b00_hook, SYM(playSound_b00), b_+27);
  CYC(b_+27, b_+30); SET_HL(wTmpcbb3);
  CYC(b_+30, b_+32); B = 0x10;
  CALL_C(b_+32, clearMemory_hook, SYM(clearMemory), b_+35);
  CALL_C(b_+35, clearWramBank1_hook, SYM(clearWramBank1), b_+38);
  CALL_C(b_+38, refreshObjectGfx_hook, SYM(refreshObjectGfx), b_+41);
  black_tower_escape_after_call_7ce0_body(gb, sp0_);
}

void func_03_7cb7__afterCall7ce0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  black_tower_escape_after_call_7ce0_body(gb, sp0_);
}

void func_03_7cb7__state1_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+93, b_+96); A = mem_rd(gb, wTmpcbb5);
  CYC(b_+96, b_+98); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+98, b_+99); ret_effect(gb); return; }
  CYC(b_+98, b_+99);
  CALL_C(b_+99, blackTowerEscapeAttempt_decCBB4_hook, SYM(blackTowerEscapeAttempt_decCBB4), b_+102);
  if (F & FZ) {
    CYCT(b_+102, b_+104);
    TAIL(func_03_7cb7__func_7d33);
  }
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+107); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+107, b_+108); ret_effect(gb); return; }
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+110); A = 0x0b;
  CYC(b_+110, b_+113); mem_wr(gb, wLinkForceState, A);
  CYC(b_+113, b_+115); A = 0x50;
  CYC(b_+115, b_+118); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+118, b_+120); A = 0x10;
  CYC(b_+120, b_+123); mem_wr(gb, w1Link_angle, A);
  CYC(b_+123, b_+124); ret_effect(gb);
}

void func_03_7cb7__func_7d33_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x10);
  CALL_C(b_+126, blackTowerEscapeAttempt_incState_hook, SYM(blackTowerEscapeAttempt_incState), b_+129);
  CYC(b_+129, b_+131); A = 0x04;
  CYC(b_+131, b_+134);
  TAIL(fadeoutToWhiteWithDelay);
}

void func_03_7cb7__state2_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+134, b_+137); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+137, b_+138); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+138, b_+139); ret_effect(gb); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); A = 0xf0;
  CALL_C(b_+141, playSound_b00_hook, SYM(playSound_b00), b_+144);
  CALL_C(b_+144, blackTowerEscapeAttempt_incState_hook, SYM(blackTowerEscapeAttempt_incState), b_+147);
  CYC(b_+147, b_+149); A = 0xf3;
  CYC(b_+149, b_+152); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+152, blackTowerEscapeAttempt_loadNewRoom_hook, SYM(blackTowerEscapeAttempt_loadNewRoom), b_+155);
  CYC(b_+155, b_+158); SET_HL(w1Link_yh);
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x78);
  CYC(b_+160, b_+161); L = alu_inc8(gb, L);
  CYC(b_+161, b_+162); L = alu_inc8(gb, L);
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x78);
  CALL_C(b_+164, resetCamera_hook, SYM(resetCamera), b_+167);
  CALL_C(b_+167, loadCommonGraphics_hook, SYM(loadCommonGraphics), b_+170);
  CYC(b_+170, b_+172); A = 0x04;
  CALL_C(b_+172, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+175);
  CYC(b_+175, b_+177); A = 0x02;
  CYC(b_+177, b_+180);
  TAIL(loadGfxRegisterStateIndex);
}

void func_03_7cb7__state3_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+180, b_+182); A = 0x00;
  CYC(b_+182, b_+185); mem_wr(gb, wScrollMode, A);
  CYC(b_+185, b_+187); A = 0xf8;
  CYC(b_+187, b_+190); mem_wr(gb, w1Link_yh, A);
  CYC(b_+190, b_+192); A = 0x05;
  CYC(b_+192, b_+195); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+195, b_+198); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+198, b_+199); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+199, b_+200); ret_effect(gb); return; }
  CYC(b_+199, b_+200);
  CALL_C(b_+200, blackTowerEscapeAttempt_decCBB4_hook, SYM(blackTowerEscapeAttempt_decCBB4), b_+203);
  if (!(F & FZ)) { CYCT(b_+203, b_+204); ret_effect(gb); return; }
  CYC(b_+203, b_+204);
  CYC(b_+204, b_+206); A = 0x0b;
  CYC(b_+206, b_+209); mem_wr(gb, wLinkForceState, A);
  CYC(b_+209, b_+211); A = 0x60;
  CYC(b_+211, b_+214); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+214, b_+216); A = 0x10;
  CYC(b_+216, b_+219); mem_wr(gb, w1Link_angle, A);
  CYC(b_+219, b_+222);
  TAIL(blackTowerEscapeAttempt_incState);
}

void func_03_7cb7__state4_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+222, b_+225); A = mem_rd(gb, wTmpcbb5);
  CYC(b_+225, b_+227); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+227, b_+228); ret_effect(gb); return; }
  CYC(b_+227, b_+228);
  CALL_C(b_+228, func_7e40_hook, SYM(func_7e40), b_+231);
  CYC(b_+231, b_+234); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+234, b_+235); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+235, b_+237);
    TAIL(func_03_7cb7__func_7dbc);
  }
  CYC(b_+235, b_+237);
  CYC(b_+237, b_+239); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+239, b_+240); ret_effect(gb); return; }
  CYC(b_+239, b_+240);
  CYC(b_+240, b_+243); A = mem_rd(gb, w1Link_direction);
  CYC(b_+243, b_+244); C = A;
  CYC(b_+244, b_+245); alu_rra(gb);
  CYC(b_+245, b_+246); alu_xor(gb, C);
  CYC(b_+246, b_+248); alu_bit(gb, 0, A);
  CYC(b_+248, b_+249); A = C;
  if (F & FZ) {
    CYCT(b_+249, b_+251);
    TAIL(func_03_7cb7__func_7db6);
  }
  CYC(b_+249, b_+251);
  CYC(b_+251, b_+253); alu_xor(gb, 0x01);
  CYC(b_+253, b_+255);
  CYC(b_+257, b_+260); mem_wr(gb, w1Link_direction, A);
  CYC(b_+260, b_+261); ret_effect(gb);
}

void func_03_7cb7__func_7db6_hook(GB *gb) {
  BASE(func_03_7cb7);
  CYC(b_+255, b_+257); alu_xor(gb, 0x02);
  CYC(b_+257, b_+260); mem_wr(gb, w1Link_direction, A);
  CYC(b_+260, b_+261); ret_effect(gb);
}

void func_03_7cb7__func_7dbc_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+261, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+264);
  if (!(F & FZ)) { CYCT(b_+264, b_+265); ret_effect(gb); return; }
  CYC(b_+264, b_+265);
  CYC(b_+265, b_+267); mem_wr(gb, HL, 0x9f);
  CYC(b_+267, b_+269); L = 0x46;
  CYC(b_+269, b_+271); A = 0x1e;
  CYC(b_+271, b_+272); mem_wr(gb, HL, A);
  CYC(b_+272, b_+275); mem_wr(gb, wTmpcbb4, A);
  CYC(b_+275, b_+278); A = mem_rd(gb, w1Link_yh);
  CYC(b_+278, b_+280); alu_sub(gb, 0x10);
  CYC(b_+280, b_+282); L = 0x4b;
  CYC(b_+282, b_+283); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+283, b_+284); L = alu_inc8(gb, L);
  CYC(b_+284, b_+287); A = mem_rd(gb, w1Link_xh);
  CYC(b_+287, b_+288); mem_wr(gb, HL, A);
  CYC(b_+288, b_+290); A = 0x50;
  CALL_C(b_+290, playSound_b00_hook, SYM(playSound_b00), b_+293);
  CALL_C(b_+293, blackTowerEscapeAttempt_incState_hook, SYM(blackTowerEscapeAttempt_incState), b_+296);
  TAIL(func_03_7cb7__state5);
}

void func_03_7cb7__state5_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+296, func_7e40_hook, SYM(func_7e40), b_+299);
  CALL_C(b_+299, blackTowerEscapeAttempt_decCBB4_hook, SYM(blackTowerEscapeAttempt_decCBB4), b_+302);
  if (!(F & FZ)) { CYCT(b_+302, b_+303); ret_effect(gb); return; }
  CYC(b_+302, b_+303);
  CYC(b_+303, b_+305); A = 0x0b;
  CYC(b_+305, b_+308); mem_wr(gb, wLinkForceState, A);
  CYC(b_+308, b_+310); A = 0x10;
  CYC(b_+310, b_+313); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+313, b_+316); SET_HL(w1Link_direction);
  CYC(b_+316, b_+318); A = 0x02;
  CYC(b_+318, b_+319); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+319, b_+321); mem_wr(gb, HL, 0x10);
  CYC(b_+321, b_+323); A = 0x07;
  CYC(b_+323, b_+326); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+326, b_+327); alu_xor(gb, A);
  CYC(b_+327, b_+330); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CALL_C(b_+330, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+333);
  CYC(b_+333, b_+335); mem_wr(gb, HL, 0x92);
  CYC(b_+335, b_+337); L = 0x43;
  CYC(b_+337, b_+339); mem_wr(gb, HL, 0x01);
  CALL_C(b_+339, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+342);
  CYC(b_+342, b_+344); mem_wr(gb, HL, 0x2c);
  CYC(b_+344, b_+346); L = 0x4b;
  CYC(b_+346, b_+349); A = mem_rd(gb, w1Link_yh);
  CYC(b_+349, b_+351); alu_add(gb, 0x10);
  CYC(b_+351, b_+352); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+352, b_+355); A = mem_rd(gb, w1Link_xh);
  CYC(b_+355, b_+356); L = alu_inc8(gb, L);
  CYC(b_+356, b_+357); mem_wr(gb, HL, A);
  CYC(b_+357, b_+360);
  TAIL(blackTowerEscapeAttempt_incState);
}

void func_03_7cb7__state6_hook(GB *gb) {
  BASE(func_03_7cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+360, func_7e40_hook, SYM(func_7e40), b_+363);
  CYC(b_+363, b_+366); A = mem_rd(gb, wTmpcbb5);
  CYC(b_+366, b_+368); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+368, b_+369); ret_effect(gb); return; }
  CYC(b_+368, b_+369);
  CYC(b_+369, b_+371); A = 0xf0;
  CALL_C(b_+371, playSound_b00_hook, SYM(playSound_b00), b_+374);
  CYC(b_+374, b_+375); alu_xor(gb, A);
  CYC(b_+375, b_+378); mem_wr(gb, wActiveMusic, A);
  CYC(b_+378, b_+379); A = alu_inc8(gb, A);
  CYC(b_+379, b_+382); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+382, b_+385); SET_HL(b_+388);
  CYC(b_+385, b_+388);
  TAIL(setWarpDestVariables);
}

void func_7e40_hook(GB *gb) {
  BASE(func_7e40);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); A = 0xb3;
  if (F & FZ) CALL_C_CC(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+13, b_+14); alu_or(gb, A);
  CYC(b_+14, b_+16); A = 0xff;
  if (F & FZ) {
    CYCT(b_+16, b_+19);
    TAIL(setScreenShakeCounter);
  }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void func_782a_hook(GB *gb) {
  BASE(func_782a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xeb;
  CALL_C(b_+2, findTileInRoom_hook, SYM(findTileInRoom), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); C = L;
  CYC(b_+7, b_+10); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+10, b_+12); alu_and(gb, 0x40);
  CYC(b_+12, b_+14); A = 0xfc;
  if (F & FZ) CYCT(b_+14, b_+16);
  else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+18); A = 0x3a;
  }
  CALL_C(b_+18, setTile_hook, SYM(setTile), b_+21);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+23); ret_effect(gb);
}
