#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t link_ship_jump_table(GB *gb) {
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

static void link_ship_add_a_to_hl(GB *gb) {
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

void interactionCoded4__afterCall4d97_hook(GB *gb) {
  CYC(0x4d97, 0x4d9a); objectSetVisible82_hook(gb);
}

void interactionCoded4__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d7f, 0x4d81); A = 1;
  CYC(0x4d81, 0x4d82); mem_wr(gb, DE, A);
  CYC(0x4d82, 0x4d83); H = D;
  CYC(0x4d83, 0x4d85); L = 0x42;
  CYC(0x4d85, 0x4d86); A = mem_rd(gb, HL);
  CYC(0x4d86, 0x4d87); B = A;
  CYC(0x4d87, 0x4d89); alu_and(gb, 0x0f);
  CYC(0x4d89, 0x4d8a); mem_wr(gb, HL, A);
  CYC(0x4d8a, 0x4d8b); A = B;
  CYC(0x4d8b, 0x4d8d); A = alu_swap(gb, A);
  CYC(0x4d8d, 0x4d8f); alu_and(gb, 0x0f);
  CYC(0x4d8f, 0x4d90); alu_add(gb, A);
  CYC(0x4d90, 0x4d91); alu_add(gb, A);
  CYC(0x4d91, 0x4d93); L = 0x46;
  CYC(0x4d93, 0x4d94); mem_wr(gb, HL, A);
  CALL_C(0x4d94, interactionInitGraphics_hook, 0x15fb, 0x4d97);
  interactionCoded4__afterCall4d97_hook(gb);
}

void interactionCoded4__ship_hook(GB *gb) {
  CYC(0x4da9, 0x4dac); A = mem_rd(gb, wFrameCounter);
  CYC(0x4dac, 0x4dad); B = A;
  CYC(0x4dad, 0x4daf); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(0x4daf, 0x4db0); ret_effect(gb); return; }
  CYC(0x4daf, 0x4db0);
  CYC(0x4db0, 0x4db1); A = B;
  CYC(0x4db1, 0x4db3); alu_and(gb, 0xe0);
  CYC(0x4db3, 0x4db5); A = alu_swap(gb, A);
  CYC(0x4db5, 0x4db6); alu_rrca(gb);
  CYC(0x4db6, 0x4db9); SET_HL(0x4dbf);
  CYC(0x4db9, 0x4dba); push_effect(gb, 0x4dba); link_ship_add_a_to_hl(gb);
  CYC(0x4dba, 0x4dbc); E = 0x4f;
  CYC(0x4dbc, 0x4dbd); A = mem_rd(gb, HL);
  CYC(0x4dbd, 0x4dbe); mem_wr(gb, DE, A);
  CYC(0x4dbe, 0x4dbf); ret_effect(gb);
}

void interactionCoded4__seagull_hook(GB *gb) {
  CYC(0x4dc7, 0x4dc8); A = mem_rd(gb, HL);
  CYC(0x4dc8, 0x4dca); alu_and(gb, 7);
  if (!(F & FZ)) { CYCT(0x4dca, 0x4dcb); ret_effect(gb); return; }
  CYC(0x4dca, 0x4dcb);
  CYC(0x4dcb, 0x4dcc); A = mem_rd(gb, HL);
  CYC(0x4dcc, 0x4dce); alu_and(gb, 0x38);
  CYC(0x4dce, 0x4dd0); A = alu_swap(gb, A);
  CYC(0x4dd0, 0x4dd1); alu_rlca(gb);
  CYC(0x4dd1, 0x4dd4); SET_HL(0x4dbf);
  CYC(0x4dd4, 0x4dd5); push_effect(gb, 0x4dd5); link_ship_add_a_to_hl(gb);
  CYC(0x4dd5, 0x4dd7); E = 0x4f;
  CYC(0x4dd7, 0x4dd8); A = mem_rd(gb, HL);
  CYC(0x4dd8, 0x4dd9); mem_wr(gb, DE, A);
  CYC(0x4dd9, 0x4dda); ret_effect(gb);
}

void interactionCoded4__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d9a, 0x4d9c); E = 0x42;
  CYC(0x4d9c, 0x4d9d); A = mem_rd(gb, DE);
  CYC(0x4d9d, 0x4d9f); alu_cp(gb, 2);
  if (F & FZ) { CYCT(0x4d9f, 0x4da0); ret_effect(gb); return; }
  CYC(0x4d9f, 0x4da0);
  CALL_C(0x4da0, interactionDecCounter1_hook, 0x23cc, 0x4da3);
  CYC(0x4da3, 0x4da5); E = 0x42;
  CYC(0x4da5, 0x4da6); A = mem_rd(gb, DE);
  CYC(0x4da6, 0x4da7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4da7, 0x4da9); interactionCoded4__seagull_hook(gb); return; }
  CYC(0x4da7, 0x4da9);
  interactionCoded4__ship_hook(gb);
}

void interactionCoded4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d77, 0x4d79); E = 0x44;
  CYC(0x4d79, 0x4d7a); A = mem_rd(gb, DE);
  CYC(0x4d7a, 0x4d7b); push_effect(gb, 0x4d7b);
  switch (link_ship_jump_table(gb)) {
    case 0x4d7f: interactionCoded4__state0_hook(gb); return;
    case 0x4d9a: interactionCoded4__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
