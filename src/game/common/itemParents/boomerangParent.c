#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (boomerang_parent_jump_table(gb));
    if (jt_ == b_+(game_seasons ? S(10) : 8)) { break; }
    else if (jt_ == b_+O(71)) { goto state1; }
    else if (game_seasons && jt_ == b_+S(67)) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  if (!game_seasons) {
    CALL_C(b_+8, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+11);
    if (!(F & FZ)) { CYCT(b_+11, b_+14); clearParentItem_hook(gb); return; }
    CYC(b_+11, b_+14);
    CYC(b_+14, b_+17); A = W8(w1ParentItem2_id);
    CYC(b_+17, b_+19); alu_cp(gb, 0x0a);
    if (F & FZ) { CYCT(b_+19, b_+22); clearParentItem_hook(gb); return; }
    CYC(b_+19, b_+22);
  }
  CYC(b_+O(22), b_+OE(25)); A = W8(wLinkSwimmingState);
  CYC(b_+O(25), b_+OE(26)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(26), b_+OE(29)); clearParentItem_hook(gb); return; }
  CYC(b_+O(26), b_+OE(29));
  CALL_C(b_+O(29), parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+OE(32));
  if (game_seasons) {
    CYC(b_+S(20), b_+S(23)); A = W8(wBoomerangLevel);
    CYC(b_+S(23), b_+S(25)); alu_cp(gb, 0x02);
  }
  CYC(b_+O(32), b_+OE(34)); A = 1;
  if (game_seasons) {
    if (!(F & FZ)) CYCT(b_+S(27), b_+S(29));
    else { CYC(b_+S(27), b_+S(29)); CYC(b_+S(29), b_+S(30)); A = alu_inc8(gb, A); }
  }
  CYC(b_+O(34), b_+OE(36)); E = 0x04;
  CYC(b_+O(36), b_+OE(37)); mem_wr(gb, DE, A);
  CYC(b_+O(37), b_+OE(38)); A = alu_dec8(gb, A);
  CYC(b_+O(38), b_+OE(39)); C = A;
  CYC(b_+O(39), b_+OE(41)); E = 1;
  CYC(b_+O(41), b_+OE(43)); A = mem_rd(gb, DE); B = A;
  CYC(b_+O(43), b_+OE(45)); E = 1;
  CALL_C(b_+O(45), itemCreateChildWithID_hook, SYM(itemCreateChildWithID), b_+OE(48));
  if (F & FC) { CYCT(b_+O(48), b_+OE(51)); clearParentItem_hook(gb); return; }
  CYC(b_+O(48), b_+OE(51));
  CYC(b_+O(51), b_+OE(54)); A = W8(wLinkAngle);
  CYC(b_+O(54), b_+OE(56)); alu_bit(gb, 7, A);
  if (F & FZ) CYCT(b_+O(56), b_+OE(58));
  else {
    CYC(b_+O(56), b_+OE(58));
    CYC(b_+O(58), b_+OE(61)); A = W8(w1Link_direction);
    CYC(b_+O(61), b_+OE(63)); A = alu_swap(gb, A);
    CYC(b_+O(63), b_+OE(64)); alu_rrca(gb);
  }
  CYC(b_+O(64), b_+OE(66)); L = 0x09;
  CYC(b_+O(66), b_+OE(67)); mem_wr(gb, HL, A);
  CYC(b_+O(67), b_+OE(69)); L = 0x34;
  CYC(b_+O(69), b_+OE(71)); mem_wr(gb, HL, A); ret_effect(gb); return;

state2:
  CALL_C(b_+S(67), parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+S(70));
  if (F & FZ) { CYCT(b_+S(70), b_+S(72)); goto cancel_control; }
  CYC(b_+S(70), b_+S(72));
  CYC(b_+S(72), b_+S(74)); A = 0x17;
  CALL_C(b_+S(74), objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+S(77));
  CYC(b_+S(77), b_+S(78)); A = mem_rd(gb, HL);
  CYC(b_+S(78), b_+S(79)); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+S(79), b_+S(81)); goto cancel_control; }
  CYC(b_+S(79), b_+S(81));
  CYC(b_+S(81), b_+S(84)); A = W8(wLinkAngle);
  CYC(b_+S(84), b_+S(85)); C = A;
  CYC(b_+S(85), b_+S(87)); A = 0xff;
  CYC(b_+S(87), b_+S(90)); W8(wLinkAngle) = A;
  CYC(b_+S(90), b_+S(93)); A = mem_rd(gb, wFrameCounter);
  CYC(b_+S(93), b_+S(94)); alu_rrca(gb);
  if (F & FC) { CYCT(b_+S(94), b_+S(96)); goto dont_change_direction; }
  CYC(b_+S(94), b_+S(96));
  CYC(b_+S(96), b_+S(97)); A = C;
  CYC(b_+S(97), b_+S(98)); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+S(98), b_+S(100)); goto set_target_angle; }
  CYC(b_+S(98), b_+S(100));
dont_change_direction:
  CYC(b_+S(100), b_+S(102)); L = 0x09;
  CYC(b_+S(102), b_+S(103)); C = mem_rd(gb, HL);
set_target_angle:
  CYC(b_+S(103), b_+S(105)); L = 0x34;
  CYC(b_+S(105), b_+S(106)); mem_wr(gb, HL, C);
  CYC(b_+S(106), b_+S(107)); ret_effect(gb); return;
cancel_control:
  CYC(b_+S(107), b_+S(109)); E = 0x04;
  CYC(b_+S(109), b_+S(111)); A = 0x01;
  CYC(b_+S(111), b_+S(112)); mem_wr(gb, DE, A);

state1:
  CYC(b_+O(71), b_+OE(73)); E = 0x21;
  CYC(b_+O(73), b_+OE(74)); A = mem_rd(gb, DE);
  CYC(b_+O(74), b_+OE(75)); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+O(75), b_+OE(78)); specialObjectAnimate_optimized_hook(gb); return; }
  CYC(b_+O(75), b_+OE(78)); CYC(b_+O(78), b_+OE(81)); clearParentItem_hook(gb);
}
