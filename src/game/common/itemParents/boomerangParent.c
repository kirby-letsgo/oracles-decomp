#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(parentItemCode_boomerang), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(parentItemCode_boomerang), (from), (to), true)

static uint16_t boomerang_parent_jump_table(GB *gb) {
  burn_rom(gb, 0, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 0x0004, 0x0006, false); burn_rom(gb, 0, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 0x0004, 0x0006, true);
  burn_rom(gb, 0, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0, 0x000a, 0x000b, false);
  return HL;
}

void parentItemCode_boomerang_hook(GB *gb) {
  BASE(parentItemCode_boomerang);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boomerang_parent_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+71) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+8, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); clearParentItem_hook(gb); return; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); A = W8(w1ParentItem2_id);
  CYC(b_+17, b_+19); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+19, b_+22); clearParentItem_hook(gb); return; }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+25); A = W8(wLinkSwimmingState);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); clearParentItem_hook(gb); return; }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+32);
  CYC(b_+32, b_+34); A = 1;
  CYC(b_+34, b_+36); E = 0x04;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+38); A = alu_dec8(gb, A);
  CYC(b_+38, b_+39); C = A;
  CYC(b_+39, b_+41); E = 1;
  CYC(b_+41, b_+43); A = mem_rd(gb, DE); B = A;
  CYC(b_+43, b_+45); E = 1;
  CALL_C(b_+45, itemCreateChildWithID_hook, SYM(itemCreateChildWithID), b_+48);
  if (F & FC) { CYCT(b_+48, b_+51); clearParentItem_hook(gb); return; }
  CYC(b_+48, b_+51);
  CYC(b_+51, b_+54); A = W8(wLinkAngle);
  CYC(b_+54, b_+56); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(b_+56, b_+58);
  else {
    CYC(b_+56, b_+58);
    CYC(b_+58, b_+61); A = W8(w1Link_direction);
    CYC(b_+61, b_+63); A = alu_swap(gb, A);
    CYC(b_+63, b_+64); alu_rrca(gb);
  }
  CYC(b_+64, b_+66); L = 0x09;
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  CYC(b_+67, b_+69); L = 0x34;
  CYC(b_+69, b_+71); mem_wr(gb, HL, A); ret_effect(gb); return;

state1:
  CYC(b_+71, b_+73); E = 0x21;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+75, b_+78); specialObjectAnimate_optimized_hook(gb); return; }
  CYC(b_+75, b_+78); CYC(b_+78, SYM(parentItemCode_bombchu)); clearParentItem_hook(gb);
}
