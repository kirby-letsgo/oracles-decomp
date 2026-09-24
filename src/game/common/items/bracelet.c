#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void braceletCheckBreakable_hook(GB *gb);
void braceletCheckDeleteSelfWhileThrowing_hook(GB *gb);
void itemBeginThrow_hook(GB *gb);
void itemUpdateThrowingLaterally_hook(GB *gb);

static uint16_t bracelet_jump_table(GB *gb) {
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

void itemCode16_hook(GB *gb) {
  BASE(itemCode16);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (bracelet_jump_table(gb));
    if (jt_ == b_+O(12)) { goto state0; }
    else if (jt_ == b_+O(36)) { goto held; }
    else if (jt_ == b_+O(102)) { goto thrown; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+O(12), itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+OE(15));
  CYC(b_+O(15), b_+OE(16)); H = D;
  CYC(b_+O(16), b_+OE(18)); L = 0x00;
  CYC(b_+O(18), b_+OE(20)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
  CYC(b_+O(20), b_+OE(22)); L = 0x02;
  CYC(b_+O(22), b_+OE(23)); A = mem_rd(gb, HL);
  CYC(b_+O(23), b_+OE(24)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(24), b_+OE(26));
    goto not_tile;
  }
  CYC(b_+O(24), b_+OE(26));
  CYC(b_+O(26), b_+OE(28)); L = 0x04;
  CYC(b_+O(28), b_+OE(30)); mem_wr(gb, HL, 0x02);
  CALL_C(b_+O(30), itemMimicBgTile_hook, SYM(itemMimicBgTile), b_+OE(33));
  CYC(b_+O(33), b_+OE(36));
  TAIL(objectSetVisiblec0);

held:
  CYC(b_+O(36), b_+OE(37)); H = D;
  CYC(b_+O(37), b_+OE(39)); L = 0x05;
  CYC(b_+O(39), b_+OE(40)); A = mem_rd(gb, HL);
  CYC(b_+O(40), b_+OE(41)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(41), b_+OE(42)); ret_effect(gb); return; }
  CYC(b_+O(41), b_+OE(42));
  CYC(b_+O(42), b_+OE(44)); L = 0x27;
  CYC(b_+O(44), b_+OE(46)); A = 0x06;
  CYC(b_+O(46), b_+OE(47)); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+O(47), b_+OE(48)); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+O(48), b_+OE(49)); L = alu_dec8(gb, L);
  CYC(b_+O(49), b_+OE(51)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+O(51), b_+OE(53));
  goto throw_item;

not_tile:
  CALL_C(b_+O(53), braceletCheckDeleteSelfWhileThrowing_hook, SYM(braceletCheckDeleteSelfWhileThrowing), b_+OE(56));
  CYC(b_+O(56), b_+OE(57)); A = H;
  CYC(b_+O(57), b_+OE(59)); alu_cp(gb, 0xd1);
  if (F & FZ) {
    CYCT(b_+O(59), b_+OE(61));
    goto copy_collisions;
  }
  CYC(b_+O(59), b_+OE(61));
  CYC(b_+O(61), b_+OE(62)); A = L;
  CYC(b_+O(62), b_+OE(64)); alu_cp(gb, 0x40);
  if (F & FC) {
    CYCT(b_+O(64), b_+OE(66));
    goto throw_item;
  }
  CYC(b_+O(64), b_+OE(66));

copy_collisions:
  CYC(b_+O(66), b_+OE(68)); A = 0x09;
  CALL_C(b_+O(68), objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+OE(71));
  CYC(b_+O(71), b_+OE(73)); E = 0x09;
  CYC(b_+O(73), b_+OE(74)); A = mem_rd(gb, DE);
  CYC(b_+O(74), b_+OE(75)); mem_wr(gb, HL, A);
  CYC(b_+O(75), b_+OE(76)); A = L;
  CYC(b_+O(76), b_+OE(78)); alu_add(gb, 0x1d);
  CYC(b_+O(78), b_+OE(79)); L = A;
  CYC(b_+O(79), b_+OE(81)); E = 0x26;
  CYC(b_+O(81), b_+OE(82)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(82), b_+OE(83)); mem_wr(gb, DE, A);
  CYC(b_+O(83), b_+OE(84)); E = alu_inc8(gb, E);
  CYC(b_+O(84), b_+OE(85)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(85), b_+OE(86)); mem_wr(gb, DE, A);
  CYC(b_+O(86), b_+OE(87)); H = D;
  CYC(b_+O(87), b_+OE(89)); L = 0x24;
  CYC(b_+O(89), b_+OE(91)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

throw_item:
  CALL_C(b_+O(91), itemBeginThrow_hook, SYM(itemBeginThrow), b_+OE(94));
  CYC(b_+O(94), b_+OE(95)); H = D;
  CYC(b_+O(95), b_+OE(97)); L = 0x04;
  CYC(b_+O(97), b_+OE(99)); mem_wr(gb, HL, 0x03);
  CYC(b_+O(99), b_+OE(100)); L = alu_inc8(gb, L);
  CYC(b_+O(100), b_+OE(102)); mem_wr(gb, HL, 0x00);

thrown:
  CALL_C(b_+O(102), braceletCheckDeleteSelfWhileThrowing_hook, SYM(braceletCheckDeleteSelfWhileThrowing), b_+OE(105));
  CALL_C(b_+O(105), itemUpdateThrowingLaterally_hook, SYM(itemUpdateThrowingLaterally), b_+OE(108));
  if (F & FZ) {
    CYCT(b_+O(108), b_+OE(110));
    if (game_seasons) goto pre_destroy;
    goto destroy;
  }
  CYC(b_+O(108), b_+OE(110));
  CYC(b_+O(110), b_+OE(112)); E = 0x39;
  CYC(b_+O(112), b_+OE(113)); A = mem_rd(gb, DE);
  CYC(b_+O(113), b_+OE(114)); C = A;
  CALL_C(b_+O(114), itemUpdateThrowingVertically_hook, SYM(itemUpdateThrowingVertically), b_+OE(117));
  if (!(F & FC)) {
    CYCT(b_+O(117), b_+OE(119));
    goto no_collision;
  }
  CYC(b_+O(117), b_+OE(119));
  CALL_C(b_+O(119), braceletCheckBreakable_hook, SYM(braceletCheckBreakable), b_+OE(122));
  if (!(F & FZ)) {
    CYCT(b_+O(122), b_+OE(124));
    if (game_seasons) goto pre_destroy;
    goto destroy;
  }
  CYC(b_+O(122), b_+OE(124));
  if (game_seasons) {
    if (!(F & FC)) CYCT(b_+S(124), b_+S(126));
    else {
      CYC(b_+S(124), b_+S(126));
      CALL_C(b_+S(126), objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+S(129));
      if (F & FC) { CYCT(b_+S(129), b_+S(130)); ret_effect(gb); return; }
      CYC(b_+S(129), b_+S(130));
    }
  }
  CALL_C(b_+O(124), itemBounce_hook, SYM(itemBounce), b_+OE(127));
  if (F & FC) {
    CYCT(b_+O(127), b_+OE(129));
    goto release;
  }
  CYC(b_+O(127), b_+OE(129));

no_collision:
  CYC(b_+O(129), b_+OE(131)); E = 0x02;
  CYC(b_+O(131), b_+OE(132)); A = mem_rd(gb, DE);
  CYC(b_+O(132), b_+OE(133)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(133), b_+OE(134)); ret_effect(gb); return; }
  CYC(b_+O(133), b_+OE(134));
  CYC(b_+O(134), b_+OE(136)); A = 0x0b;
  CALL_C(b_+O(136), objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+OE(139));
  CYC(b_+O(139), b_+OE(142));
  TAIL(objectCopyPosition);

release:
  if (game_seasons) {
    CYC(b_+S(148), b_+S(150)); E = 0x02;
    CYC(b_+S(150), b_+S(151)); A = mem_rd(gb, DE);
    CYC(b_+S(151), b_+S(153)); alu_cp(gb, 0xd7);
    if (F & FZ) { CYCT(b_+S(153), b_+S(155)); goto create_puff; }
    CYC(b_+S(153), b_+S(155));
  }
  CYC(b_+O(142), b_+OE(144)); A = 0x05;
  CALL_C(b_+O(144), objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+OE(147));
  CYC(b_+O(147), b_+OE(149)); mem_wr(gb, HL, 0x03);
  CYC(b_+O(149), b_+OE(152));
  TAIL(itemDelete);

pre_destroy:
  CYC(b_+S(165), b_+S(167)); E = 0x02;
  CYC(b_+S(167), b_+S(168)); A = mem_rd(gb, DE);
  CYC(b_+S(168), b_+S(170)); alu_cp(gb, 0xd7);
  if (!(F & FZ)) { CYCT(b_+S(170), b_+S(172)); goto destroy; }
  CYC(b_+S(170), b_+S(172));
create_puff:
  CALL_C(b_+S(172), objectCreatePuff_hook, SYM(objectCreatePuff), b_+S(175));
  CYC(b_+S(175), b_+S(178));
  TAIL(itemDelete);

destroy:
  CALL_C(b_+O(152), objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+OE(155));
  if (F & FC) { CYCT(b_+O(155), b_+OE(156)); ret_effect(gb); return; }
  CYC(b_+O(155), b_+OE(156));
  CYC(b_+O(156), b_+OE(159)); SET_HL((SYM(applyDamageToLink) + 41));
  CYC(b_+O(159), b_+OE(161)); E = 0x06;
  CALL_C(b_+O(161), interBankCall_hook, 0x008a, b_+OE(164));
  CYC(b_+O(164), b_+OE(167));
  TAIL(itemDelete);
}

void braceletCheckBreakable_hook(GB *gb) {
  BASE(braceletCheckBreakable);
  CYC(b_+O(0), b_+OE(2)); E = 0x02;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(4), b_+OE(5)); ret_effect(gb); return; }
  CYC(b_+O(4), b_+OE(5));
  if (game_seasons) { CYC(b_+S(5), b_+S(7)); alu_cp(gb, 0xd7); }   // flags only; a leftover compare
  CYC(b_+O(5), b_+OE(6)); alu_scf(gb);
  CYC(b_+O(6), b_+OE(7)); ret_effect(gb);
}

void braceletCheckDeleteSelfWhileThrowing_hook(GB *gb) {
  BASE(braceletCheckDeleteSelfWhileThrowing);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x02;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto throwing_tile;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CALL_C(b_+7, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+10);
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+12, b_+14);
    goto delete_self;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = L;
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+18); L = A;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+21); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
    goto delete_self;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);

delete_self:
  CYC(b_+27, b_+28); SET_AF(pop_effect(gb));
  CYC(b_+28, b_+31);
  TAIL(itemDelete);

throwing_tile:
  CALL_C(b_+31, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+34);
  if (!(F & FC)) {
    CYCT(b_+34, b_+36);
    goto delete_self;
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = 0x05;
  CYC(b_+39, b_+40); ret_effect(gb);
}
