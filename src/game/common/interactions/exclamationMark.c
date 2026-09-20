#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void objectCreateExclamationMark_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x406d, 0x406f); H8(0xff8b) = A;
  CALL_C(0x406f, getFreeInteractionSlot_hook, 0x3aef, 0x4072);
  if (!(F & FZ)) {
    CYCT(0x4072, 0x4073); ret_effect(gb);
    return;
  }
  CYC(0x4072, 0x4073);
  CYC(0x4073, 0x4075); mem_wr(gb, HL, 0x9f);
  CYC(0x4075, 0x4077); L = 0x46;
  CYC(0x4077, 0x4079); A = H8(0xff8b);
  CYC(0x4079, 0x407a); mem_wr(gb, HL, A);
  CALL_C(0x407a, objectCopyPositionWithOffset_hook, 0x225a, 0x407d);
  CYC(0x407d, 0x407e); push_effect(gb, HL);
  CYC(0x407e, 0x4080); A = 0x50;
  CALL_ROM(0x4080, 0x0c98);
  CYC(0x4083, 0x4084); SET_HL(pop_effect(gb));
  CYC(0x4084, 0x4085); ret_effect(gb);
}

void objectCreateFloatingImage_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4085, getFreeInteractionSlot_hook, 0x3aef, 0x4088);
  if (!(F & FZ)) {
    CYCT(0x4088, 0x4089); ret_effect(gb);
    return;
  }
  CYC(0x4088, 0x4089);
  CYC(0x4089, 0x408b); mem_wr(gb, HL, 0xa0);
  CYC(0x408b, 0x408c); L = alu_inc8(gb, L);
  CYC(0x408c, 0x408e); A = H8(0xff8d);
  CYC(0x408e, 0x408f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x408f, 0x4091); A = H8(0xff8b);
  CYC(0x4091, 0x4092); mem_wr(gb, HL, A);
  CYC(0x4092, 0x4095); objectCopyPositionWithOffset_hook(gb);
}

static uint16_t exclamation_jump_table(GB *gb) {
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

void interactionCode9f__afterCall405b_hook(GB *gb) {
  CYC(0x405b, 0x405e); objectSetVisible80_hook(gb);
}

void interactionCode9f__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4050, 0x4052); A = 1;
  CYC(0x4052, 0x4053); mem_wr(gb, DE, A);
  CYC(0x4053, 0x4054); H = D;
  CYC(0x4054, 0x4056); L = 0x40;
  CYC(0x4056, 0x4058); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(0x4058, interactionInitGraphics_hook, 0x15fb, 0x405b);
  interactionCode9f__afterCall405b_hook(gb);
}

void interactionCode9f__state1_hook(GB *gb) {
  CYC(0x405e, 0x405f); H = D;
  CYC(0x405f, 0x4061); L = 0x46;
  CYC(0x4061, 0x4062); A = mem_rd(gb, HL);
  CYC(0x4062, 0x4063); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x4063, 0x4066); interactionAnimate_hook(gb);
    return;
  }
  CYC(0x4063, 0x4066);
  CYC(0x4066, 0x4067); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x4067, 0x406a); interactionAnimate_hook(gb);
    return;
  }
  CYC(0x4067, 0x406a);
  CYC(0x406a, 0x406d); interactionDelete_hook(gb);
}

void interactionCode9f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4048, 0x404a); E = 0x44;
  CYC(0x404a, 0x404b); A = mem_rd(gb, DE);
  CYC(0x404b, 0x404c); push_effect(gb, 0x404c);
  switch (exclamation_jump_table(gb)) {
    case 0x4050: interactionCode9f__state0_hook(gb); return;
    case 0x405e: interactionCode9f__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
