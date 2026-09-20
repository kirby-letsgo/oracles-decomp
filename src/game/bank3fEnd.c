#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(func_7cf8), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(func_7cf8), (from), (to), true)

static void func_7cf8_set_bits(GB *gb) {
  BASE(func_7cf8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); ret_effect(gb);
}

void func_7cf8_hook(GB *gb) {
  BASE(func_7cf8);
  CYC(b_+0, b_+3); SET_HL((wGroup0RoomFlags + 2));
  CYC(b_+3, b_+6); push_effect(gb, b_+6);
  func_7cf8_set_bits(gb);
  CYC(b_+6, b_+8); L = 0x12;
  func_7cf8_set_bits(gb);
}

void func_7ca7_hook(GB *gb) {
  BASE(tuniNut_state3);
  uint16_t sp0_ = gb->sp;
  CYC(SYM(func_7ca7), (SYM(func_7ca7) + 2));
  CYC(b_+110, b_+111); C = H;
  CALL_C(b_+111, loseTreasure_hook, SYM(loseTreasure), b_+114);
  CALL_ROM(b_+114, b_+138);
  CYC(b_+117, b_+118); alu_xor(gb, A);
  CYC(b_+118, b_+121); W8(wDisabledObjects) = A;
  CYC(b_+121, b_+124); W8(wMenuDisabled) = A;
  CYC(b_+124, b_+127); SET_HL(wRoomLayoutEnd);
  CYC(b_+127, b_+129); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+129, b_+132); A = W8(wActiveMusic);
  CALL_C(b_+132, playSound_b00_hook, SYM(playSound_b00), b_+135);
  CYC(b_+135, b_+138);
  tuniNut_gotoState4_hook(gb);
}

void func_7caf_hook(GB *gb) {
  BASE(func_7caf);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_ROM(b_+2, (SYM(objectUpdateSpeedZ_sidescroll_givenYOffset) + 27));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x77;
  CALL_ROM(b_+8, (SYM(setMusicVolume) + 4));
  CYC(b_+11, b_+13); E = 0x46;
  CYC(b_+13, b_+15); A = 0x5a;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+18); A = 0x5b;
  CALL_ROM(b_+18, (SYM(setMusicVolume) + 4));
  CYC(b_+21, SYM(func_7cc7));
  hook_continue(gb, (SYM(hazardCollisionTable__overworld) + 14), sp0_);
}

void func_7cc7__afterCall7cce_hook(GB *gb) {
  BASE(func_7cc7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+7, SYM(func_7cd1));
  hook_continue(gb, (SYM(hazardCollisionTable__overworld) + 14), sp0_);
}

void func_7cc7_hook(GB *gb) {
  BASE(func_7cc7);
  CALL_ROM(b_+0, (SYM(hazardCollisionTable) + 1));
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CALL_ROM(b_+4, (SYM(mainThreadStart) + 1));
  func_7cc7__afterCall7cce_hook(gb);
}

static void func_7cd1_tail(GB *gb, uint16_t sp0_) {
  BASE(tuniNut_beginMovingIntoPlace);
  CYC(b_+10, b_+11); alu_rlca(gb);
  CYC(b_+11, b_+12); alu_add(gb, H);
  CALL_C(b_+12, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+15);
  CYC(b_+15, b_+17); L = 0x56;
  CYC(b_+17, b_+18); A = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+20); A = D;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CALL_C(b_+21, darkenRoomLightly_hook, SYM(darkenRoomLightly), b_+24);
  CYC(b_+24, b_+26); A = 0xf0;
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CALL_C(b_+29, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+32);
  CYC(b_+32, SYM(tuniNut_state3));
  interactionIncState_hook(gb);
}

void func_7cd1_hook(GB *gb) {
  BASE(func_7cd1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x29;
  CALL_ROM(b_+7, (SYM(clearPaletteFadeVariables) + 13));
  CYC(b_+10, b_+12); A = 0x4c;
  CALL_ROM(b_+12, (SYM(checkTreasureObtained) + 25));
  CALL_C(b_+15, func_7cf8_hook, SYM(func_7cf8), b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); W8(wDisabledObjects) = A;
  CYC(b_+22, b_+25); W8(wMenuDisabled) = A;
  CYC(b_+25, b_+28); SET_HL(wRoomLayoutEnd);
  CYC(b_+28, b_+30); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+30, b_+33); A = W8(wActiveMusic);
  CALL_ROM(b_+33, (SYM(setMusicVolume) + 4));
  CYC(b_+36, SYM(func_7cf8));
  func_7cd1_tail(gb, sp0_);
}
