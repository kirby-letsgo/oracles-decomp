#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  CYC(0x40a6, 0x40a7); H = D;
  CYC(0x40a7, 0x40a9); B = 3;
  CYC(0x40a9, 0x40ab); L = 0x43;
  CYC(0x40ab, 0x40ac); A = mem_rd(gb, HL);
  CYC(0x40ac, 0x40ad); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x40ad, 0x40af);
  } else {
    CYC(0x40ad, 0x40af);
    CYC(0x40af, 0x40b1); B = 0x1d;
  }
  CYC(0x40b1, 0x40b3); L = 0x49;
  CYC(0x40b3, 0x40b4); mem_wr(gb, HL, B);
  CYC(0x40b4, 0x40b6); L = 0x50;
  CYC(0x40b6, 0x40b8); mem_wr(gb, HL, 0x0f);
  CYC(0x40b8, 0x40ba); L = 0x46;
  CYC(0x40ba, 0x40bc); mem_wr(gb, HL, 0x46);
  CYC(0x40bc, 0x40bf); objectSetVisible80_hook(gb);
}

void interactionCodea0__afterCall40a6_hook(GB *gb) {
  floating_image_state0_tail(gb);
}

void interactionCodea0__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x409d, 0x409f); A = 1;
  CYC(0x409f, 0x40a0); mem_wr(gb, DE, A);
  CALL_C(0x40a0, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x40a3);
  CALL_C(0x40a3, interactionInitGraphics_hook, 0x15fb, 0x40a6);
  interactionCodea0__afterCall40a6_hook(gb);
}

void interactionCodea0__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40bf, 0x40c1); E = 0x42;
  CYC(0x40c1, 0x40c2); A = mem_rd(gb, DE);
  CYC(0x40c2, 0x40c3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x40c3, 0x40c5);
  } else {
    CYC(0x40c3, 0x40c5);
  }
  CALL_C(0x40c5, interactionDecCounter1_hook, 0x23cc, 0x40c8);
  if (F & FZ) {
    CYCT(0x40c8, 0x40cb); interactionDelete_hook(gb);
    return;
  }
  CYC(0x40c8, 0x40cb);
  CALL_C(0x40cb, objectApplySpeed_hook, 0x201d, 0x40ce);
  CYC(0x40ce, 0x40d0); E = 0x70;
  CYC(0x40d0, 0x40d1); mem_wr(gb, DE, A);
  CYC(0x40d1, 0x40d4); A = W8(wFrameCounter);
  CYC(0x40d4, 0x40d6); alu_and(gb, 7);
  if (!(F & FZ)) {
    CYCT(0x40d6, 0x40d7); ret_effect(gb);
    return;
  }
  CYC(0x40d6, 0x40d7);
  CYC(0x40d7, 0x40d8); push_effect(gb, DE);
  CYC(0x40d8, 0x40d9); H = D;
  CYC(0x40d9, 0x40dc); A = W8(wFrameCounter);
  CYC(0x40dc, 0x40de); alu_and(gb, 0x38);
  CYC(0x40de, 0x40e0); A = alu_swap(gb, A);
  CYC(0x40e0, 0x40e1); alu_rlca(gb);
  CYC(0x40e1, 0x40e4); SET_DE(0x40f0);
  CALL_C(0x40e4, addAToDe_hook, 0x0068, 0x40e7);
  CYC(0x40e7, 0x40e8); A = mem_rd(gb, DE);
  CYC(0x40e8, 0x40ea); L = 0x70;
  CYC(0x40ea, 0x40eb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x40eb, 0x40ed); L = 0x4d;
  CYC(0x40ed, 0x40ee); mem_wr(gb, HL, A);
  CYC(0x40ee, 0x40ef); SET_DE(pop_effect(gb));
  CYC(0x40ef, 0x40f0); ret_effect(gb);
}

void interactionCodea0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4095, 0x4097); E = 0x44;
  CYC(0x4097, 0x4098); A = mem_rd(gb, DE);
  CYC(0x4098, 0x4099); push_effect(gb, 0x4099);
  switch (floating_image_jump_table(gb)) {
    case 0x409d: interactionCodea0__state0_hook(gb); return;
    case 0x40bf: interactionCodea0__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
