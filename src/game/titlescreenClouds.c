#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t titlescreen_clouds_jump_table(GB *gb) {
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

static void titlescreen_clouds_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0, 0x18, 0x19, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x19, 0x1a, false); C = A;
  burn_rom(gb, 0, 0x1a, 0x1c, false); B = 0;
  burn_rom(gb, 0, 0x1c, 0x1d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1d, 0x1e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1e, 0x1f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x1f, 0x20, false); ret_effect(gb);
}

void introObject_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ca1, 0x4ca2); H = D;
  CYC(0x4ca2, 0x4ca4); L = 0x49;
  CYC(0x4ca4, 0x4ca5); C = mem_rd(gb, HL);
  CYC(0x4ca5, 0x4ca7); L = 0x50;
  CYC(0x4ca7, 0x4ca8); B = mem_rd(gb, HL);
  CALL_C(0x4ca8, getPositionOffsetForVelocity_hook, 0x2041, 0x4cab);
  if (F & FZ) { CYCT(0x4cab, 0x4cac); ret_effect(gb); return; }
  CYC(0x4cab, 0x4cac);
  CYC(0x4cac, 0x4cae); E = 0x76;
  CYC(0x4cae, 0x4caf); A = mem_rd(gb, DE);
  CYC(0x4caf, 0x4cb0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4cb0, 0x4cb1); mem_wr(gb, DE, A);
  CYC(0x4cb1, 0x4cb2); E = alu_inc8(gb, E);
  CYC(0x4cb2, 0x4cb3); L = alu_inc8(gb, L);
  CYC(0x4cb3, 0x4cb4); A = mem_rd(gb, DE);
  CYC(0x4cb4, 0x4cb5); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x4cb5, 0x4cb6); mem_wr(gb, DE, A);
  CYC(0x4cb6, 0x4cb8); E = 0x4c;
  CYC(0x4cb8, 0x4cb9); L = alu_inc8(gb, L);
  CYC(0x4cb9, 0x4cba); A = mem_rd(gb, DE);
  CYC(0x4cba, 0x4cbb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4cbb, 0x4cbc); mem_wr(gb, DE, A);
  CYC(0x4cbc, 0x4cbd); E = alu_inc8(gb, E);
  CYC(0x4cbd, 0x4cbe); L = alu_inc8(gb, L);
  CYC(0x4cbe, 0x4cbf); A = mem_rd(gb, DE);
  CYC(0x4cbf, 0x4cc0); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x4cc0, 0x4cc1); mem_wr(gb, DE, A);
  CYC(0x4cc1, 0x4cc2); ret_effect(gb);
}

void interactionCoded2__afterCall4c5b_hook(GB *gb) {
  CYC(0x4c5b, 0x4c5d); E = 0x42;
  CYC(0x4c5d, 0x4c5e); A = mem_rd(gb, DE);
  CYC(0x4c5e, 0x4c61); SET_HL(0x4c76);
  CYC(0x4c61, 0x4c62); titlescreen_clouds_add_double_index(gb, 0x4c62);
  CYC(0x4c62, 0x4c63); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4c63, 0x4c64); B = mem_rd(gb, HL);
  CYC(0x4c64, 0x4c65); H = D;
  CYC(0x4c65, 0x4c67); L = 0x77;
  CYC(0x4c67, 0x4c68); mem_wr(gb, HL, A);
  CYC(0x4c68, 0x4c6a); L = 0x4b;
  CYC(0x4c6a, 0x4c6b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4c6b, 0x4c6c); L = alu_inc8(gb, L);
  CYC(0x4c6c, 0x4c6d); mem_wr(gb, HL, B);
  CYC(0x4c6d, 0x4c6f); L = 0x49;
  CYC(0x4c6f, 0x4c71); mem_wr(gb, HL, 0x10);
  CYC(0x4c71, 0x4c73); L = 0x50;
  CYC(0x4c73, 0x4c75); mem_wr(gb, HL, 5);
  CYC(0x4c75, 0x4c76); ret_effect(gb);
}

void interactionCoded2__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c55, 0x4c57); A = 1;
  CYC(0x4c57, 0x4c58); mem_wr(gb, DE, A);
  CALL_C(0x4c58, interactionInitGraphics_hook, 0x15fb, 0x4c5b);
  interactionCoded2__afterCall4c5b_hook(gb);
}

void interactionCoded2__substate1_hook(GB *gb) {
  CYC(0x4c9b, 0x4c9e); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x4c9e, 0x4ca0); alu_cp(gb, 0x88);
  if (F & FZ) { CYCT(0x4ca0, 0x4ca1); ret_effect(gb); return; }
  CYC(0x4ca0, 0x4ca1);
  introObject_applySpeed_hook(gb);
}

void interactionCoded2__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c8f, 0x4c92); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x4c92, 0x4c94); alu_cp(gb, 0xe0);
  if (!(F & FZ)) { CYCT(0x4c94, 0x4c95); ret_effect(gb); return; }
  CYC(0x4c94, 0x4c95);
  CALL_C(0x4c95, interactionIncSubstate_hook, 0x23e5, 0x4c98);
  CALL_C(0x4c98, objectSetVisible82_hook, 0x1e69, 0x4c9b);
  interactionCoded2__substate1_hook(gb);
}

void interactionCoded2__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c7e, 0x4c81); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x4c81, 0x4c82); B = A;
  CYC(0x4c82, 0x4c84); E = 0x77;
  CYC(0x4c84, 0x4c85); A = mem_rd(gb, DE);
  CYC(0x4c85, 0x4c86); alu_sub(gb, B);
  CYC(0x4c86, 0x4c87); E = alu_inc8(gb, E);
  CYC(0x4c87, 0x4c89); E = 0x4b;
  CYC(0x4c89, 0x4c8a); mem_wr(gb, DE, A);
  CALL_C(0x4c8a, checkInteractionSubstate_hook, 0x2403, 0x4c8d);
  if (!(F & FZ)) { CYCT(0x4c8d, 0x4c8f); interactionCoded2__substate1_hook(gb); return; }
  CYC(0x4c8d, 0x4c8f);
  interactionCoded2__substate0_hook(gb);
}

void interactionCoded2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c4d, 0x4c4f); E = 0x44;
  CYC(0x4c4f, 0x4c50); A = mem_rd(gb, DE);
  CYC(0x4c50, 0x4c51); push_effect(gb, 0x4c51);
  switch (titlescreen_clouds_jump_table(gb)) {
    case 0x4c55: interactionCoded2__state0_hook(gb); return;
    case 0x4c7e: interactionCoded2__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
