#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodea0), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodea0), (from), (to), true)

static uint16_t floating_image_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 4, 6, true);
  }
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void floating_image_state0_tail(GB *gb) {
  BASE(interactionCodea0);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); B = 3;
  CYC(b_+20, b_+22); L = 0x43;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+26);
  } else {
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+28); B = 0x1d;
  }
  CYC(b_+28, b_+30); L = 0x49;
  CYC(b_+30, b_+31); mem_wr(gb, HL, B);
  CYC(b_+31, b_+33); L = 0x50;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x0f);
  CYC(b_+35, b_+37); L = 0x46;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x46);
  CYC(b_+39, b_+42); objectSetVisible80_hook(gb);
}

void interactionCodea0__afterCall40a6_hook(GB *gb) {
  floating_image_state0_tail(gb);
}

void interactionCodea0__state0_hook(GB *gb) {
  BASE(interactionCodea0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+14);
  CALL_C(b_+14, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+17);
  interactionCodea0__afterCall40a6_hook(gb);
}

void interactionCodea0__state1_hook(GB *gb) {
  BASE(interactionCodea0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+42, b_+44); E = 0x42;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+46, b_+48);
  } else {
    CYC(b_+46, b_+48);
  }
  CALL_C(b_+48, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+51);
  if (F & FZ) {
    CYCT(b_+51, b_+54); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+51, b_+54);
  CALL_C(b_+54, objectApplySpeed_hook, SYM(objectApplySpeed), b_+57);
  CYC(b_+57, b_+59); E = 0x70;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+63); A = W8(wFrameCounter);
  CYC(b_+63, b_+65); alu_and(gb, 7);
  if (!(F & FZ)) {
    CYCT(b_+65, b_+66); ret_effect(gb);
    return;
  }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+67); push_effect(gb, DE);
  CYC(b_+67, b_+68); H = D;
  CYC(b_+68, b_+71); A = W8(wFrameCounter);
  CYC(b_+71, b_+73); alu_and(gb, 0x38);
  CYC(b_+73, b_+75); A = alu_swap(gb, A);
  CYC(b_+75, b_+76); alu_rlca(gb);
  CYC(b_+76, b_+79); SET_DE(b_+91);
  CALL_C(b_+79, addAToDe_hook, 0x0068, b_+82);
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+85); L = 0x70;
  CYC(b_+85, b_+86); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+86, b_+88); L = 0x4d;
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);
  CYC(b_+89, b_+90); SET_DE(pop_effect(gb));
  CYC(b_+90, b_+91); ret_effect(gb);
}

void interactionCodea0_hook(GB *gb) {
  BASE(interactionCodea0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (floating_image_jump_table(gb));
    if (jt_ == b_+8) { interactionCodea0__state0_hook(gb); return; }
    else if (jt_ == b_+42) { interactionCodea0__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
