#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t intro_bird_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void intro_bird_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x12, 0x13, false);
    burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x14, 0x15, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0, 0x12, 0x13, true); ret_effect(gb);
  }
}

void interactionCoded3__initializePositionAndCounter1_hook(GB *gb) {
  CYC(0x4cf0, 0x4cf1); B = A;
  CYC(0x4cf1, 0x4cf2); alu_add(gb, A);
  CYC(0x4cf2, 0x4cf3); alu_add(gb, B);
  CYC(0x4cf3, 0x4cf6); SET_HL(0x4d5f);
  CYC(0x4cf6, 0x4cf7); push_effect(gb, 0x4cf7); intro_bird_add_a_to_hl(gb);
  CYC(0x4cf7, 0x4cf8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4cf8, 0x4cf9); B = mem_rd(gb, HL);
  CYC(0x4cf9, 0x4cfa); L = alu_inc8(gb, L);
  CYC(0x4cfa, 0x4cfb); C = mem_rd(gb, HL);
  CYC(0x4cfb, 0x4cfc); H = D;
  CYC(0x4cfc, 0x4cfe); L = 0x77;
  CYC(0x4cfe, 0x4cff); mem_wr(gb, HL, A);
  CYC(0x4cff, 0x4d01); L = 0x4b;
  CYC(0x4d01, 0x4d02); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4d02, 0x4d03); L = alu_inc8(gb, L);
  CYC(0x4d03, 0x4d04); mem_wr(gb, HL, B);
  CYC(0x4d04, 0x4d06); L = 0x46;
  CYC(0x4d06, 0x4d07); mem_wr(gb, HL, C);
  CYC(0x4d07, 0x4d08); ret_effect(gb);
}

void interactionCoded3__afterCall4cd0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4cd0, 0x4cd1); H = D;
  CYC(0x4cd1, 0x4cd3); L = 0x47;
  CYC(0x4cd3, 0x4cd5); mem_wr(gb, HL, 45);
  CYC(0x4cd5, 0x4cd7); L = 0x42;
  CYC(0x4cd7, 0x4cd8); A = mem_rd(gb, HL);
  CYC(0x4cd8, 0x4cda); B = 0;
  CYC(0x4cda, 0x4cdc); C = 0x1a;
  CYC(0x4cdc, 0x4cde); alu_cp(gb, 4);
  if (F & FC) CYCT(0x4cde, 0x4ce0);
  else {
    CYC(0x4cde, 0x4ce0);
    CYC(0x4ce0, 0x4ce1); B = alu_inc8(gb, B);
    CYC(0x4ce1, 0x4ce3); C = 6;
  }
  CYC(0x4ce3, 0x4ce5); L = 0x49;
  CYC(0x4ce5, 0x4ce6); mem_wr(gb, HL, C);
  CYC(0x4ce6, 0x4ce8); L = 0x50;
  CYC(0x4ce8, 0x4cea); mem_wr(gb, HL, 0x32);
  CYC(0x4cea, 0x4ceb); push_effect(gb, AF);
  CYC(0x4ceb, 0x4cec); A = B;
  CALL_C(0x4cec, interactionSetAnimation_hook, 0x262e, 0x4cef);
  CYC(0x4cef, 0x4cf0); SET_AF(pop_effect(gb));
  interactionCoded3__initializePositionAndCounter1_hook(gb);
}

void interactionCoded3__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4cca, 0x4ccc); A = 1;
  CYC(0x4ccc, 0x4ccd); mem_wr(gb, DE, A);
  CALL_C(0x4ccd, interactionInitGraphics_hook, 0x15fb, 0x4cd0);
  interactionCoded3__afterCall4cd0_hook(gb);
}

void interactionCoded3__substate0_hook(GB *gb) {
  CYC(0x4d1e, 0x4d21); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x4d21, 0x4d23); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x4d23, 0x4d24); ret_effect(gb); return; }
  CYC(0x4d23, 0x4d24);
  CYC(0x4d24, 0x4d27); interactionIncSubstate_hook(gb);
}

void interactionCoded3__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d27, interactionDecCounter1_hook, 0x23cc, 0x4d2a);
  if (!(F & FZ)) { CYCT(0x4d2a, 0x4d2b); ret_effect(gb); return; }
  CYC(0x4d2a, 0x4d2b);
  CALL_C(0x4d2b, interactionIncSubstate_hook, 0x23e5, 0x4d2e);
  CYC(0x4d2e, 0x4d31); objectSetVisible82_hook(gb);
}

void interactionCoded3__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d31, 0x4d33); E = 0x47;
  CYC(0x4d33, 0x4d34); A = mem_rd(gb, DE);
  CYC(0x4d34, 0x4d35); alu_or(gb, A);
  if (F & FZ) CYCT(0x4d35, 0x4d37);
  else {
    CYC(0x4d35, 0x4d37);
    CYC(0x4d37, 0x4d38); A = alu_dec8(gb, A);
    CYC(0x4d38, 0x4d39); mem_wr(gb, DE, A);
  }
  CALL_C(0x4d39, interactionAnimate_hook, 0x261b, 0x4d3c);
  CALL_C(0x4d3c, introObject_applySpeed_hook, 0x4ca1, 0x4d3f);
  CYC(0x4d3f, 0x4d41); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(0x4d41, 0x4d42); ret_effect(gb); return; }
  CYC(0x4d41, 0x4d42);
  CYC(0x4d42, 0x4d43); H = D;
  CYC(0x4d43, 0x4d45); L = 0x47;
  CYC(0x4d45, 0x4d46); A = mem_rd(gb, HL);
  CYC(0x4d46, 0x4d47); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4d47, 0x4d4a); interactionDelete_hook(gb); return; }
  CYC(0x4d47, 0x4d4a);
  CYC(0x4d4a, 0x4d4c); L = 0x45;
  CYC(0x4d4c, 0x4d4d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4d4d, 0x4d4f); L = 0x42;
  CYC(0x4d4f, 0x4d50); A = mem_rd(gb, HL);
  CALL_C(0x4d50, interactionCoded3__initializePositionAndCounter1_hook, 0x4cf0, 0x4d53);
  CALL_C(0x4d53, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4d56);
  CYC(0x4d56, 0x4d58); alu_and(gb, 0x0f);
  CYC(0x4d58, 0x4d59); H = D;
  CYC(0x4d59, 0x4d5b); L = 0x46;
  CYC(0x4d5b, 0x4d5c); mem_wr(gb, HL, A);
  CYC(0x4d5c, 0x4d5f); objectSetInvisible_hook(gb);
}

void interactionCoded3__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d08, 0x4d0b); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x4d0b, 0x4d0c); B = A;
  CYC(0x4d0c, 0x4d0e); E = 0x77;
  CYC(0x4d0e, 0x4d0f); A = mem_rd(gb, DE);
  CYC(0x4d0f, 0x4d10); alu_sub(gb, B);
  CYC(0x4d10, 0x4d11); E = alu_inc8(gb, E);
  CYC(0x4d11, 0x4d13); E = 0x4b;
  CYC(0x4d13, 0x4d14); mem_wr(gb, DE, A);
  CYC(0x4d14, 0x4d16); E = 0x45;
  CYC(0x4d16, 0x4d17); A = mem_rd(gb, DE);
  CYC(0x4d17, 0x4d18); push_effect(gb, 0x4d18);
  switch (intro_bird_jump_table(gb)) {
    case 0x4d1e: interactionCoded3__substate0_hook(gb); return;
    case 0x4d27: interactionCoded3__substate1_hook(gb); return;
    case 0x4d31: interactionCoded3__substate2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void interactionCoded3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4cc2, 0x4cc4); E = 0x44;
  CYC(0x4cc4, 0x4cc5); A = mem_rd(gb, DE);
  CYC(0x4cc5, 0x4cc6); push_effect(gb, 0x4cc6);
  switch (intro_bird_jump_table(gb)) {
    case 0x4cca: interactionCoded3__state0_hook(gb); return;
    case 0x4d08: interactionCoded3__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
