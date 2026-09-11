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
