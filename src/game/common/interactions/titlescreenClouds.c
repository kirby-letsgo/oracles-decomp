#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(introObject_applySpeed), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(introObject_applySpeed), (from), (to), true)

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
  BASE(introObject_applySpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x49;
  CYC(b_+3, b_+4); C = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = 0x50;
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CALL_C(b_+7, getPositionOffsetForVelocity_hook, SYM(getPositionOffsetForVelocity), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = 0x76;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); E = 0x4c;
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, SYM(interactionCoded3)); ret_effect(gb);
}

void interactionCoded2__afterCall4c5b_hook(GB *gb) {
  BASE(interactionCoded2);
  CYC(b_+14, b_+16); E = 0x42;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+20); SET_HL(b_+41);
  CYC(b_+20, b_+21); titlescreen_clouds_add_double_index(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); B = mem_rd(gb, HL);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = 0x77;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = 0x4b;
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); mem_wr(gb, HL, B);
  CYC(b_+32, b_+34); L = 0x49;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x10);
  CYC(b_+36, b_+38); L = 0x50;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 5);
  CYC(b_+40, b_+41); ret_effect(gb);
}

void interactionCoded2__state0_hook(GB *gb) {
  BASE(interactionCoded2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  interactionCoded2__afterCall4c5b_hook(gb);
}

void interactionCoded2__substate1_hook(GB *gb) {
  BASE(interactionCoded2);
  CYC(b_+78, b_+81); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+81, b_+83); alu_cp(gb, 0x88);
  if (F & FZ) { CYCT(b_+83, SYM(introObject_applySpeed)); ret_effect(gb); return; }
  CYC(b_+83, SYM(introObject_applySpeed));
  introObject_applySpeed_hook(gb);
}

void interactionCoded2__substate0_hook(GB *gb) {
  BASE(interactionCoded2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+66, b_+69); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+69, b_+71); alu_cp(gb, 0xe0);
  if (!(F & FZ)) { CYCT(b_+71, b_+72); ret_effect(gb); return; }
  CYC(b_+71, b_+72);
  CALL_C(b_+72, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+75);
  CALL_C(b_+75, objectSetVisible82_hook, SYM(objectSetVisible82), b_+78);
  interactionCoded2__substate1_hook(gb);
}

void interactionCoded2__state1_hook(GB *gb) {
  BASE(interactionCoded2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+49, b_+52); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+52, b_+53); B = A;
  CYC(b_+53, b_+55); E = 0x77;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); alu_sub(gb, B);
  CYC(b_+57, b_+58); E = alu_inc8(gb, E);
  CYC(b_+58, b_+60); E = 0x4b;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CALL_C(b_+61, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); interactionCoded2__substate1_hook(gb); return; }
  CYC(b_+64, b_+66);
  interactionCoded2__substate0_hook(gb);
}

void interactionCoded2_hook(GB *gb) {
  BASE(interactionCoded2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (titlescreen_clouds_jump_table(gb));
    if (jt_ == b_+8) { interactionCoded2__state0_hook(gb); return; }
    else if (jt_ == b_+49) { interactionCoded2__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
