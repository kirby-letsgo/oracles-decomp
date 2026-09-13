#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

static void func_7cf8_set_bits(GB *gb) {
  CYC(0x7d00, 0x7d02); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7d02, 0x7d03); L = alu_inc8(gb, L);
  CYC(0x7d03, 0x7d05); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7d05, 0x7d06); L = alu_inc8(gb, L);
  CYC(0x7d06, 0x7d08); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7d08, 0x7d09); L = alu_inc8(gb, L);
  CYC(0x7d09, 0x7d0a); ret_effect(gb);
}

void func_7cf8_hook(GB *gb) {
  CYC(0x7cf8, 0x7cfb); SET_HL(0xc702);
  CYC(0x7cfb, 0x7cfe); push_effect(gb, 0x7cfe);
  func_7cf8_set_bits(gb);
  CYC(0x7cfe, 0x7d00); L = 0x12;
  func_7cf8_set_bits(gb);
}

void func_7ca7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ca7, 0x7ca9);
  CYC(0x7c79, 0x7c7a); C = H;
  CALL_C(0x7c7a, loseTreasure_hook, 0x1733, 0x7c7d);
  CALL_ROM(0x7c7d, 0x7c95);
  CYC(0x7c80, 0x7c81); alu_xor(gb, A);
  CYC(0x7c81, 0x7c84); W8(wDisabledObjects) = A;
  CYC(0x7c84, 0x7c87); W8(wMenuDisabled) = A;
  CYC(0x7c87, 0x7c8a); SET_HL(0xcfc0);
  CYC(0x7c8a, 0x7c8c); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7c8c, 0x7c8f); A = W8(wActiveMusic);
  CALL_C(0x7c8f, playSound_b00_hook, 0x0c98, 0x7c92);
  CYC(0x7c92, 0x7c95);
  tuniNut_gotoState4_hook(gb);
}

void func_7caf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7caf, 0x7cb1); C = 0x20;
  CALL_ROM(0x7cb1, 0x1f83);
  if (!(F & FZ)) {
    CYCT(0x7cb4, 0x7cb5); ret_effect(gb); return;
  }
  CYC(0x7cb4, 0x7cb5);
  CYC(0x7cb5, 0x7cb7); A = 0x77;
  CALL_ROM(0x7cb7, 0x0cb1);
  CYC(0x7cba, 0x7cbc); E = 0x46;
  CYC(0x7cbc, 0x7cbe); A = 0x5a;
  CYC(0x7cbe, 0x7cbf); mem_wr(gb, DE, A);
  CYC(0x7cbf, 0x7cc1); A = 0x5b;
  CALL_ROM(0x7cc1, 0x0cb1);
  CYC(0x7cc4, 0x7cc7);
  hook_continue(gb, 0x2422, sp0_);
}

void func_7cc7__afterCall7cce_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7cce, 0x7cd1);
  hook_continue(gb, 0x2422, sp0_);
}

void func_7cc7_hook(GB *gb) {
  CALL_ROM(0x7cc7, 0x2409);
  if (!(F & FZ)) {
    CYCT(0x7cca, 0x7ccb); ret_effect(gb); return;
  }
  CYC(0x7cca, 0x7ccb);
  CALL_ROM(0x7ccb, 0x33a2);
  func_7cc7__afterCall7cce_hook(gb);
}

static void func_7cd1_tail(GB *gb, uint16_t sp0_) {
  CYC(0x7bf2, 0x7bf3); alu_rlca(gb);
  CYC(0x7bf3, 0x7bf4); alu_add(gb, H);
  CALL_C(0x7bf4, objectCreateInteraction_hook, 0x24c5, 0x7bf7);
  CYC(0x7bf7, 0x7bf9); L = 0x56;
  CYC(0x7bf9, 0x7bfa); A = E;
  CYC(0x7bfa, 0x7bfb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7bfb, 0x7bfc); A = D;
  CYC(0x7bfc, 0x7bfd); mem_wr(gb, HL, A);
  CALL_C(0x7bfd, darkenRoomLightly_hook, 0x32f8, 0x7c00);
  CYC(0x7c00, 0x7c02); A = 0xf0;
  CALL_C(0x7c02, playSound_b00_hook, 0x0c98, 0x7c05);
  CALL_C(0x7c05, objectSetVisiblec0_hook, 0x1e33, 0x7c08);
  CYC(0x7c08, 0x7c0b);
  interactionIncState_hook(gb);
}

void func_7cd1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7cd1, 0x7cd4); A = W8(wPaletteThread_mode);
  CYC(0x7cd4, 0x7cd5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7cd5, 0x7cd6); ret_effect(gb); return;
  }
  CYC(0x7cd5, 0x7cd6);
  CYC(0x7cd6, 0x7cd8); A = 0x29;
  CALL_ROM(0x7cd8, 0x324b);
  CYC(0x7cdb, 0x7cdd); A = 0x4c;
  CALL_ROM(0x7cdd, 0x1761);
  CALL_C(0x7ce0, func_7cf8_hook, 0x7cf8, 0x7ce3);
  CYC(0x7ce3, 0x7ce4); alu_xor(gb, A);
  CYC(0x7ce4, 0x7ce7); W8(wDisabledObjects) = A;
  CYC(0x7ce7, 0x7cea); W8(wMenuDisabled) = A;
  CYC(0x7cea, 0x7ced); SET_HL(0xcfc0);
  CYC(0x7ced, 0x7cef); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7cef, 0x7cf2); A = W8(wActiveMusic);
  CALL_ROM(0x7cf2, 0x0cb1);
  CYC(0x7cf5, 0x7cf8);
  func_7cd1_tail(gb, sp0_);
}
