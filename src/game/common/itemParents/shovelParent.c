#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t shovel_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void shovel_parent_add_double_index(GB *gb) {
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void parentItemCode_shovel_hook(GB *gb) {
  BASE(parentItemCode_shovel);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (shovel_parent_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+17) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, checkLinkOnGround_hook, SYM(checkLinkOnGround), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); clearParentItem_hook(gb); return; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); TAIL(parentItemLoadAnimationAndIncState);

state1:
  CALL_C(b_+17, specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+20);
  CYC(b_+20, b_+22); E = 0x21;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); clearParentItem_hook(gb); return; }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+29); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+29, b_+30); ret_effect(gb); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, itemCreateChildIfDoesntExistAlready_hook, SYM(itemCreateChildIfDoesntExistAlready), b_+34);
  CYC(b_+34, b_+35); push_effect(gb, HL);
  CYC(b_+35, b_+37); L = 0x08;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+41); SET_HL(b_+54);
  CYC(b_+41, b_+42); push_effect(gb, b_+42); shovel_parent_add_double_index(gb);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); C = mem_rd(gb, HL);
  CYC(b_+44, b_+45); SET_HL(pop_effect(gb));
  CYC(b_+45, b_+47); L = 0x0b;
  CYC(b_+47, b_+48); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+49); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); alu_add(gb, C);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+54); ret_effect(gb);
}
