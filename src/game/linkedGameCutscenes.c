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
