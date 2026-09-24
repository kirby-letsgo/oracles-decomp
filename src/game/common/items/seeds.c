#include "game/game.h"
#include "game/gen.h"

void s_slingshotCheckCanPassSolidTile_hook(GB *gb);

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void seedItemDelete_hook(GB *gb);
void emberSeedBurn_hook(GB *gb);
void seedUpdateAnimation_hook(GB *gb);
void scentSeedSmell_hook(GB *gb);
void galeSeedUpdateAnimationAndCounter_hook(GB *gb);
void galeSeedUpdateAnimation_hook(GB *gb);
void galeSeedTryToWarpLink_hook(GB *gb);
void seedItemCheckDiagonalCollision_hook(GB *gb);
void seedItemClearKnockback_hook(GB *gb);

static void seed_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static uint16_t seed_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void seedItemDelete_hook(GB *gb) {
  BASE(seedItemDelete);
  CYC(b_+0, b_+2); E = 0x02;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto delete_item;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); SET_HL(wIsSeedShooterInUse);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    goto delete_item;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

delete_item:
  CYC(b_+14, b_+17); TAIL(itemDelete);
}

void seedItemState3_hook(GB *gb) {
  BASE(seedItemState3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x20);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == SYM(emberSeedBurn) && hook_is(gb, SYM(emberSeedBurn), emberSeedBurn_hook)) { emberSeedBurn_hook(gb); return; }
    else if (jt_ == SYM(seedUpdateAnimation) && hook_is(gb, SYM(seedUpdateAnimation), seedUpdateAnimation_hook)) { seedUpdateAnimation_hook(gb); return; }
    else if (jt_ == SYM(galeSeedUpdateAnimationAndCounter) && hook_is(gb, SYM(galeSeedUpdateAnimationAndCounter), galeSeedUpdateAnimationAndCounter_hook)) { galeSeedUpdateAnimationAndCounter_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void emberSeedBurn_hook(GB *gb) {
  BASE(emberSeedBurn);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x06;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto break_tile;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, itemAnimate_hook, SYM(itemAnimate), b_+9);
  CALL_C(b_+9, itemUpdateDamageToApply_hook, SYM(itemUpdateDamageToApply), b_+12);
  CYC(b_+12, b_+14); L = 0x21;
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    goto update_position;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); L = 0x24;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x00);
  CYC(b_+21, b_+23); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25);
    goto delete_self;
  }
  CYC(b_+23, b_+25);

update_position:
  CYC(b_+25, b_+27); L = 0x0e;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+31); C = 0x1c;
  if (!(F & FZ)) {
    CYCT(b_+31, b_+34);
    TAIL(objectUpdateSpeedZ_paramC);
  }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+36); alu_bit(gb, 6, B);
  if (F & FZ) {
    CYCT(b_+36, b_+37); ret_effect(gb); return;
  }
  CYC(b_+36, b_+37);
  CALL_C(b_+37, objectCheckTileAtPositionIsWater_hook, SYM(objectCheckTileAtPositionIsWater), b_+40);
  if (F & FC) {
    CYCT(b_+40, b_+42);
    goto delete_self;
  }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); ret_effect(gb);
  return;

break_tile:
  CYC(b_+43, b_+45); A = 0x0c;
  CALL_C(b_+45, itemTryToBreakTile_hook, SYM(itemTryToBreakTile), b_+48);

delete_self:
  CYC(b_+48, b_+51); TAIL(seedItemDelete);
}

void seedUpdateAnimation_hook(GB *gb) {
  BASE(seedUpdateAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x24;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CALL_C(b_+4, itemAnimate_hook, SYM(itemAnimate), b_+7);
  CYC(b_+7, b_+9); E = 0x21;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); TAIL(seedItemDelete);
}

void seedItemState2_hook(GB *gb) {
  BASE(seedItemState2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x20);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == SYM(emberSeedBurn) && hook_is(gb, SYM(emberSeedBurn), emberSeedBurn_hook)) { emberSeedBurn_hook(gb); return; }
    else if (jt_ == SYM(scentSeedSmell) && hook_is(gb, SYM(scentSeedSmell), scentSeedSmell_hook)) { scentSeedSmell_hook(gb); return; }
    else if (jt_ == SYM(seedUpdateAnimation) && hook_is(gb, SYM(seedUpdateAnimation), seedUpdateAnimation_hook)) { seedUpdateAnimation_hook(gb); return; }
    else if (jt_ == SYM(galeSeedTryToWarpLink) && hook_is(gb, SYM(galeSeedTryToWarpLink), galeSeedTryToWarpLink_hook)) { galeSeedTryToWarpLink_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void scentSeedSmell_hook(GB *gb) {
  BASE(scentSeedSmell);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x06;
  CYC(b_+3, b_+6); A = W8(wFrameCounter);
  CYC(b_+6, b_+7); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+7, b_+9);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) {
      CYCT(b_+10, b_+13);
      TAIL(seedItemDelete);
    }
    CYC(b_+10, b_+13);
  }
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x1e);
  if (!(F & FC)) {
    CYCT(b_+16, b_+18);
  } else {
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+20); L = 0x1a;
    CYC(b_+20, b_+21); A = mem_rd(gb, HL);
    CYC(b_+21, b_+23); alu_xor(gb, 0x80);
    CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  }
  CYC(b_+24, b_+26); L = 0x0b;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+29); H8(hFFB2) = A;
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+33); H8(hFFB3) = A;
  CYC(b_+33, b_+35); A = 0xff;
  CYC(b_+35, b_+38); W8(wScentSeedActive) = A;
  CALL_C(b_+38, itemAnimate_hook, SYM(itemAnimate), b_+41);
  CALL_C(b_+41, bombPullTowardPoint_hook, SYM(bombPullTowardPoint), b_+44);
  if (F & FC) {
    CYCT(b_+44, b_+47);
    TAIL(seedItemDelete);
  }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+50); TAIL(itemUpdateSpeedZAndCheckHazards);
}

void galeSeedUpdateAnimationAndCounter_hook(GB *gb) {
  BASE(galeSeedUpdateAnimationAndCounter);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, galeSeedUpdateAnimation_hook, SYM(galeSeedUpdateAnimation), b_+3);
  CALL_C(b_+3, itemDecCounter1_hook, SYM(itemDecCounter1), b_+6);
  if (F & FZ) {
    CYCT(b_+6, b_+9);
    TAIL(seedItemDelete);
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 0x14);
  if (!(F & FC)) {
    CYCT(b_+12, b_+13); ret_effect(gb); return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); L = 0x1a;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_xor(gb, 0x80);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void galeSeedUpdateAnimation_hook(GB *gb) {
  BASE(galeSeedUpdateAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemAnimate_hook, SYM(itemAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x06;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = 0x1b;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  CYC(b_+13, b_+15); alu_and(gb, 0x0b);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void galeSeedTryToWarpLink_hook(GB *gb) {
  BASE(galeSeedTryToWarpLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), galeSeedUpdateAnimation_hook, SYM(galeSeedUpdateAnimation), b_+OE(3));
  CYC(b_+O(3), b_+OE(5)); E = 0x05;
  CYC(b_+O(5), b_+OE(6)); A = mem_rd(gb, DE);
  CYC(b_+O(6), b_+OE(7)); push_effect(gb, b_+OE(7));
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == b_+O(15)) { goto substate0; }
    else if (jt_ == b_+O(91)) { goto substate1; }
    else if (jt_ == b_+O(113)) { goto substate2; }
    else if (jt_ == b_+O(156)) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+O(15), b_+OE(18)); A = W8(wTilesetFlags);
  if (!game_seasons) {
    CYC(b_+18, b_+19); alu_rrca(gb);
    if (!(F & FC)) {
      CYCT(b_+19, b_+21);
      goto set_substate3;
    }
    CYC(b_+19, b_+21);
  } else {
    CYC(b_+S(18), b_+S(19)); A = alu_dec8(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+S(19), b_+S(21));
      goto set_substate3;
    }
    CYC(b_+S(19), b_+S(21));
  }
  CYC(b_+O(21), b_+OE(24)); A = W8(wWarpsDisabled);
  CYC(b_+O(24), b_+OE(25)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(25), b_+OE(27));
    TAIL(galeSeedUpdateAnimationAndCounter);
  }
  CYC(b_+O(25), b_+OE(27));
  CYC(b_+O(27), b_+OE(30)); A = W8(wLinkObjectIndex);
  CYC(b_+O(30), b_+OE(31)); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+O(31), b_+OE(33));
    TAIL(galeSeedUpdateAnimationAndCounter);
  }
  CYC(b_+O(31), b_+OE(33));
  CYC(b_+O(33), b_+OE(36)); A = W8(wLinkGrabState2);
  CYC(b_+O(36), b_+OE(38)); alu_and(gb, 0xf0);
  CYC(b_+O(38), b_+OE(40)); alu_cp(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+O(40), b_+OE(42));
    TAIL(galeSeedUpdateAnimationAndCounter);
  }
  CYC(b_+O(40), b_+OE(42));
  if (!game_seasons) CALL_C(b_+42, checkLinkVulnerableAndIDZero_hook, SYM(checkLinkVulnerableAndIDZero), b_+45);
  else CALL_C(b_+S(42), checkLinkID0AndControlNormal_hook, SYM(checkLinkID0AndControlNormal), b_+S(45));
  if (!(F & FC)) {
    CYCT(b_+O(45), b_+OE(47));
    TAIL(galeSeedUpdateAnimationAndCounter);
  }
  CYC(b_+O(45), b_+OE(47));
  CALL_C(b_+O(47), objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+OE(50));
  if (!(F & FC)) {
    CYCT(b_+O(50), b_+OE(52));
    TAIL(galeSeedUpdateAnimationAndCounter);
  }
  CYC(b_+O(50), b_+OE(52));
  CYC(b_+O(52), b_+OE(55)); SET_HL(w1Link);
  CALL_C(b_+O(55), objectTakePosition_hook, SYM(objectTakePosition), b_+OE(58));
  CYC(b_+O(58), b_+OE(60)); E = 0x07;
  CYC(b_+O(60), b_+OE(62)); A = 0x3c;
  CYC(b_+O(62), b_+OE(63)); mem_wr(gb, DE, A);
  CYC(b_+O(63), b_+OE(65)); E = 0x05;
  CYC(b_+O(65), b_+OE(67)); A = 0x01;
  CYC(b_+O(67), b_+OE(68)); mem_wr(gb, DE, A);
  CYC(b_+O(68), b_+OE(71)); W8(wMenuDisabled) = A;
  CYC(b_+O(71), b_+OE(74)); W8(wLinkCanPassNpcs) = A;
  CYC(b_+O(74), b_+OE(77)); W8(wDisableScreenTransitions) = A;
  CYC(b_+O(77), b_+OE(79)); A = 0x07;
  CYC(b_+O(79), b_+OE(82)); W8(wLinkForceState) = A;
  CYC(b_+O(82), b_+OE(85));
  TAIL(objectSetVisible80);

set_substate3:
  CYC(b_+O(85), b_+OE(87)); E = 0x05;
  CYC(b_+O(87), b_+OE(89)); A = 0x03;
  CYC(b_+O(89), b_+OE(90)); mem_wr(gb, DE, A);
  CYC(b_+O(90), b_+OE(91)); ret_effect(gb);
  return;

substate1:
  CYC(b_+O(91), b_+OE(94)); A = W8(wLinkDeathTrigger);
  CYC(b_+O(94), b_+OE(95)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(95), b_+OE(97));
    goto set_substate3;
  }
  CYC(b_+O(95), b_+OE(97));
  CYC(b_+O(97), b_+OE(98)); H = D;
  CYC(b_+O(98), b_+OE(100)); L = 0x07;
  CYC(b_+O(100), b_+OE(101)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+O(101), b_+OE(103));
    goto begin_rising;
  }
  CYC(b_+O(101), b_+OE(103));
  CYC(b_+O(103), b_+OE(106)); A = W8(wActiveGroup);
  CYC(b_+O(106), b_+OE(107)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(107), b_+OE(109));
    goto flicker_and_copy;
  }
  CYC(b_+O(107), b_+OE(109));
  CYC(b_+O(109), b_+OE(110)); ret_effect(gb);
  return;

begin_rising:
  CYC(b_+O(110), b_+OE(112)); A = 0x02;
  CYC(b_+O(112), b_+OE(113)); mem_wr(gb, DE, A);

substate2:
  CYC(b_+O(113), b_+OE(114)); H = D;
  CYC(b_+O(114), b_+OE(116)); L = 0x0f;
  CYC(b_+O(116), b_+OE(117)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+O(117), b_+OE(118)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+O(118), b_+OE(120)); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+O(120), b_+OE(122));
    goto flicker_and_copy;
  }
  CYC(b_+O(120), b_+OE(122));
  CYC(b_+O(122), b_+OE(124)); A = 0x02;
  CYC(b_+O(124), b_+OE(127)); mem_wr(gb, w1Link_substate, A);
  CYC(b_+O(127), b_+OE(129)); A = 0x16;
  CYC(b_+O(129), b_+OE(132)); W8(wCutsceneTrigger) = A;
  CYC(b_+O(132), b_+OE(134)); A = 0x05;
  CALL_C(b_+O(134), openMenu_hook, SYM(openMenu), b_+OE(137));
  CYC(b_+O(137), b_+OE(140));
  TAIL(seedItemDelete);

flicker_and_copy:
  CYC(b_+O(140), b_+OE(142)); E = 0x1a;
  CYC(b_+O(142), b_+OE(143)); A = mem_rd(gb, DE);
  CYC(b_+O(143), b_+OE(145)); alu_xor(gb, 0x80);
  CYC(b_+O(145), b_+OE(146)); mem_wr(gb, DE, A);
  CYC(b_+O(146), b_+OE(147)); alu_xor(gb, A);
  CYC(b_+O(147), b_+OE(150)); W8(wLinkSwimmingState) = A;
  CYC(b_+O(150), b_+OE(153)); SET_HL(w1Link);
  CYC(b_+O(153), b_+OE(156));
  TAIL(objectCopyPosition);

substate3:
  CALL_C(b_+O(156), itemDecCounter2_hook, SYM(itemDecCounter2), b_+OE(159));
  if (F & FZ) {
    CYCT(b_+O(159), b_+OE(162));
    TAIL(seedItemDelete);
  }
  CYC(b_+O(159), b_+OE(162));
  CYC(b_+O(162), b_+OE(164)); L = 0x1a;
  CYC(b_+O(164), b_+OE(165)); A = mem_rd(gb, HL);
  CYC(b_+O(165), b_+OE(167)); alu_xor(gb, 0x80);
  CYC(b_+O(167), b_+OE(168)); mem_wr(gb, HL, A);
  CYC(b_+O(168), b_+OE(169)); ret_effect(gb);
}

void seedItemUpdateBouncing_hook(GB *gb) {
  BASE(seedItemUpdateBouncing);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+6); SET_HL(SYM(seedsDontBounceTilesTable));
  CALL_C(b_+6, findByteInCollisionTable_hook, SYM(findByteInCollisionTable), b_+9);
  if (F & FC) {
    CYCT(b_+9, b_+11);
    goto unset_zero;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = 0x09;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_bit(gb, 2, A);
  if (F & FZ) {
    CYCT(b_+16, b_+18);
    goto moving_straight;
  }
  CYC(b_+16, b_+18);
  CALL_C(b_+18, seedItemCheckDiagonalCollision_hook, SYM(seedItemCheckDiagonalCollision), b_+21);
  CYC(b_+21, b_+22); push_effect(gb, AF);
  CALL_C(b_+22, itemCheckCanPassSolidTile_hook, SYM(itemCheckCanPassSolidTile), b_+25);
  CYC(b_+25, b_+26); SET_AF(pop_effect(gb));
  if (F & FZ) {
    CYCT(b_+26, b_+28);
    goto set_zero;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30);
  goto bounce;

moving_straight:
  CYC(b_+30, b_+32); E = 0x33;
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+37);
  if (!(F & FC)) {
    CYCT(b_+37, b_+39);
    goto set_zero;
  }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); E = 0x33;
  CYC(b_+41, b_+43); A = 0x03;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CALL_C(b_+44, itemCheckCanPassSolidTile_hook, SYM(itemCheckCanPassSolidTile), b_+47);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
    goto set_zero;
  }
  CYC(b_+47, b_+49);

bounce:
  CALL_C(b_+49, seedItemClearKnockback_hook, SYM(seedItemClearKnockback), b_+52);
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = 0x34;
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+56, b_+58);
    goto unset_zero;
  }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); L = 0x33;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+63); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+63, b_+65);
    goto reverse_both;
  }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); C = A;
  CYC(b_+66, b_+68); E = 0x09;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); alu_rrca(gb);
  CYC(b_+70, b_+71); alu_rrca(gb);
  CYC(b_+71, b_+73); alu_and(gb, 0x06);
  CYC(b_+73, b_+74); alu_add(gb, C);
  CYC(b_+74, b_+77); SET_HL(b_+91);
  CYC(b_+77, b_+78); seed_add_a_to_hl_from_rst(gb, b_+78);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);

set_zero:
  CYC(b_+80, b_+81); alu_xor(gb, A);
  CYC(b_+81, b_+82); ret_effect(gb);
  return;

reverse_both:
  CYC(b_+82, b_+84); L = 0x09;
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); alu_xor(gb, 0x10);
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);
  CYC(b_+88, b_+89); alu_xor(gb, A);
  CYC(b_+89, b_+90); ret_effect(gb);
  return;

unset_zero:
  CYC(b_+90, b_+91); alu_or(gb, D);
  CYC(b_+91, b_+92); ret_effect(gb);
}

void seedItemCheckDiagonalCollision_hook(GB *gb) {
  BASE(seedItemCheckDiagonalCollision);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_rrca(gb);
  CYC(b_+1, b_+3); alu_and(gb, 0x0c);
  CYC(b_+3, b_+6); SET_HL(b_+75);
  CYC(b_+6, b_+7); seed_add_a_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+10); H8(hFF8A) = A;
  CYC(b_+10, b_+12); E = 0x33;
  CYC(b_+12, b_+14); A = 0x40;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);

next_component:
  CYC(b_+15, b_+17); E = 0x0b;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+21); SET_HL(HL + 1);
  CYC(b_+21, b_+23); E = 0x0d;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+26); C = A;
  CYC(b_+26, b_+27); SET_HL(HL + 1);
  CYC(b_+27, b_+28); push_effect(gb, HL);
  CALL_C(b_+28, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+31);
  if (!(F & FC)) {
    CYCT(b_+31, b_+33);
    goto next;
  }
  CYC(b_+31, b_+33);
  CALL_C(b_+33, getTileAtPosition_hook, SYM(getTileAtPosition), b_+36);
  CYC(b_+36, b_+39); SET_HL(SYM(seedsDontBounceTilesTable));
  CALL_C(b_+39, findByteInCollisionTable_hook, SYM(findByteInCollisionTable), b_+42);
  CYC(b_+42, b_+43); alu_ccf(gb);
  if (!(F & FC)) {
    CYCT(b_+43, b_+45);
    goto next;
  }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+46); H = D;
  CYC(b_+46, b_+48); L = 0x09;
  CYC(b_+48, b_+49); B = mem_rd(gb, HL);
  CALL_C(b_+49, checkTileIsPassableFromDirection_hook, SYM(checkTileIsPassableFromDirection), b_+52);
  CYC(b_+52, b_+53); alu_ccf(gb);
  if (F & FC) {
    CYCT(b_+53, b_+55);
    goto next;
  }
  CYC(b_+53, b_+55);
  if (F & FZ) {
    CYCT(b_+55, b_+57);
    goto next;
  }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = 0x3e;
  CYC(b_+60, b_+61); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+61, b_+62); alu_rlca(gb);

next:
  CYC(b_+62, b_+63); H = D;
  CYC(b_+63, b_+65); L = 0x33;
  CYC(b_+65, b_+67); mem_wr(gb, HL, alu_rl(gb, mem_rd(gb, HL)));
  CYC(b_+67, b_+68); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+68, b_+70);
    goto next_component;
  }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); E = 0x33;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+74); alu_or(gb, A);
  CYC(b_+74, b_+75); ret_effect(gb);
}

void func_50f4_hook(GB *gb) {
  BASE(func_50f4);
  CYC(b_+0, b_+2); E = 0x09;
  CYC(b_+2, b_+4); L = 0x2c;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+9); SET_HL(SYM(data_5114));
  CYC(b_+9, b_+10); seed_add_a_to_hl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    TAIL(seedItemClearKnockback);
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = 0x34;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+19, b_+21);
    goto unset_zero;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = C;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+25); ret_effect(gb);
  return;

unset_zero:
  CYC(b_+25, b_+26); alu_or(gb, D);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void seedItemClearKnockback_hook(GB *gb) {
  BASE(seedItemClearKnockback);
  CYC(b_+0, b_+2); E = 0x2d;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); ret_effect(gb);
}

static void seed_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void seed_state1_load_gfx_vars(GB *gb) {
  BASE(seedItemState1);
  CYC(b_+O(261), b_+OE(262)); alu_add(gb, A);
  CYC(b_+O(262), b_+OE(265)); SET_HL(b_+O(155));
  CYC(b_+O(265), b_+OE(266)); seed_add_double_index_from_rst(gb, b_+OE(266));
  CYC(b_+O(266), b_+OE(268)); E = 0x1b;
  CYC(b_+O(268), b_+OE(269)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(269), b_+OE(270)); mem_wr(gb, DE, A);
  CYC(b_+O(270), b_+OE(271)); E = alu_inc8(gb, E);
  CYC(b_+O(271), b_+OE(272)); mem_wr(gb, DE, A);
  CYC(b_+O(272), b_+OE(273)); E = alu_inc8(gb, E);
  CYC(b_+O(273), b_+OE(274)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(274), b_+OE(275)); mem_wr(gb, DE, A);
  CYC(b_+O(275), b_+OE(276)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(276), b_+OE(278)); E = 0x06;
  CYC(b_+O(278), b_+OE(279)); mem_wr(gb, DE, A);
  CYC(b_+O(279), b_+OE(280)); A = mem_rd(gb, HL);
  CYC(b_+O(280), b_+OE(283)); playSound_b00_hook(gb);
}

static void seed_state1_init_state3(GB *gb) {
  BASE(seedItemState1);
  CYC(b_+O(253), b_+OE(255)); E = 0x04;
  CYC(b_+O(255), b_+OE(257)); A = 0x03;
  CYC(b_+O(257), b_+OE(258)); mem_wr(gb, DE, A);
  CYC(b_+O(258), b_+OE(260)); E = 0x01;
  CYC(b_+O(260), b_+OE(261)); A = mem_rd(gb, DE);
  seed_state1_load_gfx_vars(gb);
}

static void seed_state1_break_tile_with_gale_seed(GB *gb) {
  BASE(seedItemState1);
  CYC(b_+O(192), b_+OE(194)); A = 0x0d;
  CYC(b_+O(194), b_+OE(197)); itemTryToBreakTile_hook(gb);
}

void seedItemState1_hook(GB *gb) {
  BASE(seedItemState1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), itemUpdateDamageToApply_hook, SYM(itemUpdateDamageToApply), b_+OE(3));
  if (!game_seasons) {
    if (F & FZ) {
      CYCT(b_+3, b_+5);
      goto no_collision;
    }
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+7); alu_bit(gb, 4, A);
    if (F & FZ) {
      CYCT(b_+7, b_+9);
      goto collided_with_enemy;
    }
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
    CALL_C(b_+11, func_50f4_hook, SYM(func_50f4), b_+14);
    if (F & FZ) {
      CYCT(b_+14, b_+16);
      goto update_position;
    }
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+18);
    goto collided_with_wall;
  } else {
    if (!(F & FZ)) {
      CYCT(b_+S(3), b_+S(5));
      goto collided_with_enemy;
    }
    CYC(b_+S(3), b_+S(5));
  }

no_collision:
  CYC(b_+O(18), b_+OE(20)); E = 0x02;
  CYC(b_+O(20), b_+OE(21)); A = mem_rd(gb, DE);
  CYC(b_+O(21), b_+OE(22)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(22), b_+OE(24));
    goto satchel_update;
  }
  CYC(b_+O(22), b_+OE(24));
  if (!game_seasons) CALL_C(b_+24, seedItemUpdateBouncing_hook, SYM(seedItemUpdateBouncing), b_+27);
  else CALL_C(b_+S(11), s_slingshotCheckCanPassSolidTile_hook, SYM(slingshotCheckCanPassSolidTile), b_+S(14));
  if (!(F & FZ)) {
    CYCT(b_+O(27), b_+OE(29));
    goto collided_with_wall;
  }
  CYC(b_+O(27), b_+OE(29));

update_position:
  if (!game_seasons) CALL_C(b_+29, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+32);
  else CALL_C(b_+S(16), objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+S(19));
  if (F & FC) {
    CYCT(b_+O(32), b_+OE(35));
    TAIL(objectApplySpeed);
  }
  CYC(b_+O(32), b_+OE(35));
  CYC(b_+O(35), b_+OE(38)); seedItemDelete_hook(gb);
  return;

satchel_update:
  CYC(b_+O(38), b_+OE(39)); H = D;
  CYC(b_+O(39), b_+OE(41)); L = 0x3b;
  CYC(b_+O(41), b_+OE(43)); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+O(43), b_+OE(45));
  } else {
    CYC(b_+O(43), b_+OE(45));
    CYC(b_+O(45), b_+OE(47)); L = 0x10;
    CYC(b_+O(47), b_+OE(49)); mem_wr(gb, HL, 0x00);
  }
  CALL_C(b_+O(49), objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+OE(52));
  if (!(F & FC)) {
    CYCT(b_+O(52), b_+OE(55));
    TAIL(seedItemDelete);
  }
  CYC(b_+O(52), b_+OE(55));
  CALL_C(b_+O(55), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(58));
  CYC(b_+O(58), b_+OE(60)); C = 0x1c;
  CALL_C(b_+O(60), itemUpdateThrowingVerticallyAndCheckHazards_hook, SYM(itemUpdateThrowingVerticallyAndCheckHazards), b_+OE(63));
  if (F & FC) {
    CYCT(b_+O(63), b_+OE(66));
    TAIL(seedItemDelete);
  }
  CYC(b_+O(63), b_+OE(66));
  if (F & FZ) {
    CYCT(b_+O(66), b_+OE(67)); ret_effect(gb); return;
  }
  CYC(b_+O(66), b_+OE(67));
  CYC(b_+O(67), b_+OE(69)); A = 0x52;
  CALL_C(b_+O(69), playSound_b00_hook, SYM(playSound_b00), b_+OE(72));
  CALL_C(b_+O(72), itemAnimate_hook, SYM(itemAnimate), b_+OE(75));
  CYC(b_+O(75), b_+OE(77)); E = 0x01;
  CYC(b_+O(77), b_+OE(78)); A = mem_rd(gb, DE);
  CYC(b_+O(78), b_+OE(80)); alu_sub(gb, 0x20);
  CYC(b_+O(80), b_+OE(81)); push_effect(gb, b_+OE(81));
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == b_+O(133)) { goto ember_standard; }
    else if (jt_ == b_+O(139)) { goto scent_landed; }
    else if (jt_ == SYM(seedItemDelete) && hook_is(gb, SYM(seedItemDelete), seedItemDelete_hook)) { seedItemDelete_hook(gb); return; }
    else if (jt_ == b_+O(167)) { goto gale_landed; }
    else if (jt_ == b_+O(246)) { goto mystery_standard; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

collided_with_wall:
  CALL_C(b_+O(91), itemAnimate_hook, SYM(itemAnimate), b_+OE(94));
  CYC(b_+O(94), b_+OE(96)); E = 0x01;
  CYC(b_+O(96), b_+OE(97)); A = mem_rd(gb, DE);
  CYC(b_+O(97), b_+OE(99)); alu_sub(gb, 0x20);
  CYC(b_+O(99), b_+OE(100)); push_effect(gb, b_+OE(100));
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == b_+O(133)) { goto ember_standard; }
    else if (jt_ == b_+O(161)) { goto scent_or_pegasus_collided; }
    else if (jt_ == b_+O(197)) { goto gale_collided_with_wall; }
    else if (jt_ == b_+O(246)) { goto mystery_standard; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

collided_with_enemy:
  CALL_C(b_+O(110), itemAnimate_hook, SYM(itemAnimate), b_+OE(113));
  CYC(b_+O(113), b_+OE(115)); E = 0x24;
  CYC(b_+O(115), b_+OE(116)); alu_xor(gb, A);
  CYC(b_+O(116), b_+OE(117)); mem_wr(gb, DE, A);
  CYC(b_+O(117), b_+OE(119)); E = 0x01;
  CYC(b_+O(119), b_+OE(120)); A = mem_rd(gb, DE);
  CYC(b_+O(120), b_+OE(122)); alu_sub(gb, 0x20);
  CYC(b_+O(122), b_+OE(123)); push_effect(gb, b_+OE(123));
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == b_+O(133)) { goto ember_standard; }
    else if (jt_ == b_+O(161)) { goto scent_or_pegasus_collided; }
    else if (jt_ == SYM(seedItemDelete) && hook_is(gb, SYM(seedItemDelete), seedItemDelete_hook)) { seedItemDelete_hook(gb); return; }
    else if (jt_ == b_+O(217)) { goto mystery_collided_with_enemy; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

ember_standard:
  CYC(b_+O(133), b_+OE(136)); push_effect(gb, b_+OE(136)); seed_state1_init_state3(gb);
  CYC(b_+O(136), b_+OE(139)); objectSetVisible82_hook(gb);
  return;

scent_landed:
  CYC(b_+O(139), b_+OE(141)); A = 0x27;
  CYC(b_+O(141), b_+OE(144)); push_effect(gb, b_+OE(144)); seed_state1_load_gfx_vars(gb);
  CYC(b_+O(144), b_+OE(146)); A = 0x02;
  CALL_C(b_+O(146), itemSetState_hook, SYM(itemSetState), b_+OE(149));
  CYC(b_+O(149), b_+OE(151)); L = 0x24;
  CYC(b_+O(151), b_+OE(153)); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(b_+O(153), b_+OE(155)); A = 0x01;
  CALL_C(b_+O(155), itemSetAnimation_hook, SYM(itemSetAnimation), b_+OE(158));
  CYC(b_+O(158), b_+OE(161)); objectSetVisible83_hook(gb);
  return;

scent_or_pegasus_collided:
  CYC(b_+O(161), b_+OE(163)); E = 0x24;
  CYC(b_+O(163), b_+OE(164)); alu_xor(gb, A);
  CYC(b_+O(164), b_+OE(165)); mem_wr(gb, DE, A);
  CYC(b_+O(165), b_+OE(167));
  goto init_state3;

gale_landed:
  CYC(b_+O(167), b_+OE(170)); push_effect(gb, b_+OE(170)); seed_state1_break_tile_with_gale_seed(gb);
  CYC(b_+O(170), b_+OE(172)); A = 0x25;
  CYC(b_+O(172), b_+OE(175)); push_effect(gb, b_+OE(175)); seed_state1_load_gfx_vars(gb);
  CYC(b_+O(175), b_+OE(177)); A = 0x02;
  CALL_C(b_+O(177), itemSetState_hook, SYM(itemSetState), b_+OE(180));
  CYC(b_+O(180), b_+OE(182)); L = 0x24;
  CYC(b_+O(182), b_+OE(183)); alu_xor(gb, A);
  CYC(b_+O(183), b_+OE(184)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(184), b_+OE(185)); L = alu_inc8(gb, L);
  CYC(b_+O(185), b_+OE(187)); A = 0x02;
  CYC(b_+O(187), b_+OE(188)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(188), b_+OE(189)); mem_wr(gb, HL, A);
  CYC(b_+O(189), b_+OE(192)); objectSetVisible82_hook(gb);
  return;

gale_collided_with_wall:
  CYC(b_+O(197), b_+OE(200)); push_effect(gb, b_+OE(200)); seed_state1_break_tile_with_gale_seed(gb);
  CYC(b_+O(200), b_+OE(202)); A = 0x26;
  CYC(b_+O(202), b_+OE(205)); push_effect(gb, b_+OE(205)); seed_state1_load_gfx_vars(gb);
  CYC(b_+O(205), b_+OE(207)); A = 0x03;
  CALL_C(b_+O(207), itemSetState_hook, SYM(itemSetState), b_+OE(210));
  CYC(b_+O(210), b_+OE(212)); L = 0x24;
  CYC(b_+O(212), b_+OE(214)); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(b_+O(214), b_+OE(217)); objectSetVisible82_hook(gb);
  return;

mystery_collided_with_enemy:
  CYC(b_+O(217), b_+OE(218)); H = D;
  CYC(b_+O(218), b_+OE(220)); L = 0x2a;
  CYC(b_+O(220), b_+OE(222)); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+O(222), b_+OE(224));
    goto mystery_standard;
  }
  CYC(b_+O(222), b_+OE(224));
  CYC(b_+O(224), b_+OE(226)); L = 0x03;
  CYC(b_+O(226), b_+OE(227)); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+O(227), b_+OE(229)); alu_add(gb, 0x20);
  CYC(b_+O(229), b_+OE(230)); L = alu_dec8(gb, L);
  CYC(b_+O(230), b_+OE(231)); mem_wr(gb, HL, A);
  CALL_C(b_+O(231), itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+OE(234));
  CYC(b_+O(234), b_+OE(235)); alu_xor(gb, A);
  CALL_C(b_+O(235), itemSetAnimation_hook, SYM(itemSetAnimation), b_+OE(238));
  CYC(b_+O(238), b_+OE(240)); E = 0x29;
  CYC(b_+O(240), b_+OE(242)); A = 0xff;
  CYC(b_+O(242), b_+OE(243)); mem_wr(gb, DE, A);
  CYC(b_+O(243), b_+OE(246));
  goto collided_with_enemy;

mystery_standard:
  CYC(b_+O(246), b_+OE(248)); E = 0x24;
  CYC(b_+O(248), b_+OE(249)); alu_xor(gb, A);
  CYC(b_+O(249), b_+OE(250)); mem_wr(gb, DE, A);
  CALL_C(b_+O(250), objectSetVisible82_hook, SYM(objectSetVisible82), b_+OE(253));

init_state3:
  seed_state1_init_state3(gb);
}

void itemCode20_hook(GB *gb) {
  BASE(itemCode20);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (seed_jump_table(gb));
    if (jt_ == b_+O(12)) { break; }
    else if (jt_ == SYM(seedItemState1) && hook_is(gb, SYM(seedItemState1), seedItemState1_hook)) { seedItemState1_hook(gb); return; }
    else if (jt_ == SYM(seedItemState2) && hook_is(gb, SYM(seedItemState2), seedItemState2_hook)) { seedItemState2_hook(gb); return; }
    else if (jt_ == SYM(seedItemState3) && hook_is(gb, SYM(seedItemState3), seedItemState3_hook)) { seedItemState3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+O(12), itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+OE(15));
  CYC(b_+O(15), b_+OE(16)); alu_xor(gb, A);
  CALL_C(b_+O(16), itemSetAnimation_hook, SYM(itemSetAnimation), b_+OE(19));
  CALL_C(b_+O(19), objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+OE(22));
  CALL_C(b_+O(22), itemIncState_hook, SYM(itemIncState), b_+OE(25));
  CYC(b_+O(25), b_+OE(28)); SET_BC(0xffe0);
  CALL_C(b_+O(28), objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+OE(31));
  if (!game_seasons) {
    CYC(b_+31, b_+33); L = 0x02;
    CYC(b_+33, b_+34); A = mem_rd(gb, HL);
    CYC(b_+34, b_+35); alu_or(gb, A);
    if (F & FZ) CALL_C_CC(b_+35, itemUpdateAngle_hook, SYM(itemUpdateAngle), b_+38);
    else CYC(b_+35, b_+38);
    CYC(b_+38, b_+40); L = 0x34;
    CYC(b_+40, b_+42); mem_wr(gb, HL, 0x03);
  } else {
    CALL_C(b_+S(31), itemUpdateAngle_hook, SYM(itemUpdateAngle), b_+S(34));
  }
  CYC(b_+O(42), b_+OE(44)); L = 0x02;
  CYC(b_+O(44), b_+OE(45)); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+O(45), b_+OE(46)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(46), b_+OE(48));
    goto shooter;
  }
  CYC(b_+O(46), b_+OE(48));
  CYC(b_+O(48), b_+OE(49)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(49), b_+OE(51)); alu_cp(gb, 0x23);
  if (!(F & FZ)) {
    CYCT(b_+O(51), b_+OE(53));
    if (!game_seasons) {
      CYC(b_+64, b_+67); SET_HL(b_+124);
      CALL_C(b_+67, applyOffsetTableHL_hook, SYM(applyOffsetTableHL), b_+70);
    }
    CYC(b_+O(70), b_+OE(72)); A = 0x1e;
    CYC(b_+O(72), b_+OE(74));
    goto set_speed;
  }
  CYC(b_+O(51), b_+OE(53));
  CYC(b_+O(53), b_+OE(55)); L = 0x0f;
  CYC(b_+O(55), b_+OE(56)); A = mem_rd(gb, HL);
  CYC(b_+O(56), b_+OE(58)); alu_add(gb, 0xf8);
  CYC(b_+O(58), b_+OE(59)); mem_wr(gb, HL, A);
  CYC(b_+O(59), b_+OE(61)); L = 0x09;
  CYC(b_+O(61), b_+OE(63)); mem_wr(gb, HL, 0xff);
  CYC(b_+O(63), b_+OE(64)); ret_effect(gb);
  return;

shooter:
  if (!game_seasons) {
    CYC(b_+74, b_+76); E = 0x09;
    CYC(b_+76, b_+77); A = mem_rd(gb, DE);
    CYC(b_+77, b_+78); alu_rrca(gb);
    CYC(b_+78, b_+81); SET_HL(b_+136);
    CYC(b_+81, b_+82); seed_add_a_to_hl_from_rst(gb, b_+82);
    CYC(b_+82, b_+83); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+83, b_+84); C = mem_rd(gb, HL);
    CYC(b_+84, b_+85); B = A;
    CYC(b_+85, b_+86); H = D;
    CYC(b_+86, b_+88); L = 0x0f;
    CYC(b_+88, b_+89); A = mem_rd(gb, HL);
    CYC(b_+89, b_+91); alu_add(gb, 0xfe);
    CYC(b_+91, b_+92); mem_wr(gb, HL, A);
    CALL_C(b_+92, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+95);
  } else {
    CYC(b_+S(60), b_+S(63)); SET_HL(b_+S(105));
    CYC(b_+S(63), b_+S(64)); seed_add_a_to_hl_from_rst(gb, b_+S(64));
    CYC(b_+S(64), b_+S(66)); E = 0x09;
    CYC(b_+S(66), b_+S(67)); A = mem_rd(gb, DE);
    CYC(b_+S(67), b_+S(68)); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+S(68), b_+S(70)); alu_and(gb, 0x1f);
    CYC(b_+S(70), b_+S(71)); mem_wr(gb, DE, A);
  }
  CYC(b_+O(95), b_+OE(98)); SET_HL(wIsSeedShooterInUse);
  CYC(b_+O(98), b_+OE(99)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(99), b_+OE(101)); A = 0x78;

set_speed:
  CYC(b_+O(101), b_+OE(103)); E = 0x10;
  CYC(b_+O(103), b_+OE(104)); mem_wr(gb, DE, A);
  if (game_seasons) {
    CYC(b_+S(80), b_+S(83)); SET_HL(b_+S(109));
    CALL_C(b_+S(83), applyOffsetTableHL_hook, SYM(applyOffsetTableHL), b_+S(86));
  }
  CYC(b_+O(104), b_+OE(106)); E = 0x01;
  CYC(b_+O(106), b_+OE(107)); A = mem_rd(gb, DE);
  CYC(b_+O(107), b_+OE(109)); alu_cp(gb, 0x24);
  if (!(F & FZ)) {
    CYCT(b_+O(109), b_+OE(110)); ret_effect(gb); return;
  }
  CYC(b_+O(109), b_+OE(110));
  CALL_C(b_+O(110), getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+OE(113));
  CYC(b_+O(113), b_+OE(115)); alu_and(gb, 0x03);
  CYC(b_+O(115), b_+OE(117)); E = 0x03;
  CYC(b_+O(117), b_+OE(118)); mem_wr(gb, DE, A);
  CYC(b_+O(118), b_+OE(120)); alu_add(gb, 0x9b);
  CYC(b_+O(120), b_+OE(122)); E = 0x24;
  CYC(b_+O(122), b_+OE(123)); mem_wr(gb, DE, A);
  CYC(b_+O(123), b_+OE(124)); ret_effect(gb);
}
