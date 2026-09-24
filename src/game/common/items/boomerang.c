#include "game/game.h"
#include "game/gen.h"

void s_magicBoomerangTryToBreakTile_hook(GB *gb);

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemCheckWithinRangeOfLink_hook(GB *gb);

static uint16_t boomerang_jump_table(GB *gb) {
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

void itemCode06_hook(GB *gb) {
  BASE(itemCode06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (boomerang_jump_table(gb));
    if (jt_ == b_+O(14)) { goto state0; }
    else if (jt_ == b_+(game_seasons ? S(86) : 63)) { goto state1; }
    else if (jt_ == b_+O(147)) { goto state2; }
    else if (jt_ == b_+O(164)) { goto state3; }
    else if (jt_ == b_+O(192)) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+O(14), itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+OE(17));
  if (!game_seasons) {
    CYC(b_+17, b_+19); A = 0x18;
  } else {
    CYC(b_+S(17), b_+S(19)); E = 0x02;
    CYC(b_+S(19), b_+S(20)); A = mem_rd(gb, DE);
    CYC(b_+S(20), b_+S(22)); alu_add(gb, 0x18);
  }
  CALL_C(b_+O(19), loadWeaponGfx_b00_hook, SYM(loadWeaponGfx_b00), b_+OE(22));
  CALL_C(b_+O(22), itemIncState_hook, SYM(itemIncState), b_+OE(25));
  if (!game_seasons) {
    CYC(b_+25, b_+27); L = 0x10;
    CYC(b_+27, b_+29); mem_wr(gb, HL, 0x41);
    CYC(b_+29, b_+31); L = 0x06;
    CYC(b_+31, b_+33); mem_wr(gb, HL, 0x28);
  } else {
    CYC(b_+S(28), b_+S(31)); SET_BC(0x4128);
    CYC(b_+S(31), b_+S(33)); L = 0x02;
    CYC(b_+S(33), b_+S(35)); alu_bit(gb, 0, mem_rd(gb, HL));
    if (F & FZ) CYCT(b_+S(35), b_+S(37));
    else {
      CYC(b_+S(35), b_+S(37));
      CYC(b_+S(37), b_+S(39)); L = 0x24;
      CYC(b_+S(39), b_+S(41)); mem_wr(gb, HL, 0x96);
      CYC(b_+S(41), b_+S(43)); L = 0x1b;
      CYC(b_+S(43), b_+S(45)); A = 0x0c;
      CYC(b_+S(45), b_+S(46)); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+S(46), b_+S(47)); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+S(47), b_+S(50)); SET_BC(0x5f78);
    }
    CYC(b_+S(50), b_+S(52)); L = 0x10;
    CYC(b_+S(52), b_+S(53)); mem_wr(gb, HL, B);
    CYC(b_+S(53), b_+S(55)); L = 0x06;
    CYC(b_+S(55), b_+S(56)); mem_wr(gb, HL, C);
  }
  CYC(b_+O(33), b_+OE(35)); C = 0xff;
  CYC(b_+O(35), b_+OE(37)); A = 0x0d;
  CALL_C(b_+O(37), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(40));
  if (F & FZ) {
    CYCT(b_+O(40), b_+OE(42));
    goto increase_damage;
  }
  CYC(b_+O(40), b_+OE(42));
  CYC(b_+O(42), b_+OE(44)); A = 0x29;
  CALL_C(b_+O(44), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(47));
  if (!(F & FZ)) {
    CYCT(b_+O(47), b_+OE(49));
    goto make_visible;
  }
  CYC(b_+O(47), b_+OE(49));
  CYC(b_+O(49), b_+OE(51)); C = 0xfe;

increase_damage:
  CYC(b_+O(51), b_+OE(53)); L = 0x28;
  CYC(b_+O(53), b_+OE(54)); A = mem_rd(gb, HL);
  CYC(b_+O(54), b_+OE(55)); alu_add(gb, C);
  CYC(b_+O(55), b_+OE(56)); mem_wr(gb, HL, A);

make_visible:
  CALL_C(b_+O(56), objectSetVisible82_hook, SYM(objectSetVisible82), b_+OE(59));
  CYC(b_+O(59), b_+OE(60)); alu_xor(gb, A);
  CYC(b_+O(60), b_+OE(63));
  TAIL(itemSetAnimation);

state1:
  if (game_seasons) CALL_C(b_+S(86), s_magicBoomerangTryToBreakTile_hook, SYM(magicBoomerangTryToBreakTile), b_+S(89));
  CYC(b_+O(63), b_+OE(65)); E = 0x2a;
  CYC(b_+O(65), b_+OE(66)); A = mem_rd(gb, DE);
  CYC(b_+O(66), b_+OE(67)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(67), b_+OE(69));
    goto return_to_link;
  }
  CYC(b_+O(67), b_+OE(69));
  CALL_C(b_+O(69), objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+OE(72));
  if (!(F & FC)) {
    CYCT(b_+O(72), b_+OE(74));
    goto no_collision;
  }
  CYC(b_+O(72), b_+OE(74));
  CALL_C(b_+O(74), itemCheckCanPassSolidTile_hook, SYM(itemCheckCanPassSolidTile), b_+OE(77));
  if (!(F & FZ)) {
    CYCT(b_+O(77), b_+OE(79));
    goto hit_wall;
  }
  CYC(b_+O(77), b_+OE(79));

no_collision:
  CALL_C(b_+O(79), objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+OE(82));
  if (!(F & FC)) {
    CYCT(b_+O(82), b_+OE(84));
    goto return_to_link;
  }
  CYC(b_+O(82), b_+OE(84));
  CYC(b_+O(84), b_+OE(86)); E = 0x34;
  CYC(b_+O(86), b_+OE(87)); A = mem_rd(gb, DE);
  CALL_C(b_+O(87), objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+OE(90));
  CALL_C(b_+O(90), itemDecCounter1_hook, SYM(itemDecCounter1), b_+OE(93));
  if (!(F & FZ)) {
    CYCT(b_+O(93), b_+OE(95));
    goto update_speed_and_animation;
  }
  CYC(b_+O(93), b_+OE(95));

return_to_link:
  CALL_C(b_+O(95), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(98));
  CYC(b_+O(98), b_+OE(99)); C = A;
  CYC(b_+O(99), b_+OE(100)); H = D;
  CYC(b_+O(100), b_+OE(102)); L = 0x0b;
  CYC(b_+O(102), b_+OE(104)); A = 0xf0;
  CYC(b_+O(104), b_+OE(105)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+O(105), b_+OE(107));
    goto set_angle;
  }
  CYC(b_+O(105), b_+OE(107));
  CYC(b_+O(107), b_+OE(109)); L = 0x0d;
  CYC(b_+O(109), b_+OE(110)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+O(110), b_+OE(112));
    goto set_angle;
  }
  CYC(b_+O(110), b_+OE(112));
  CYC(b_+O(112), b_+OE(114)); L = 0x09;
  CYC(b_+O(114), b_+OE(115)); A = C;
  CYC(b_+O(115), b_+OE(116)); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+O(116), b_+OE(118)); alu_add(gb, 0x08);
  CYC(b_+O(118), b_+OE(120)); alu_cp(gb, 0x11);
  if (F & FC) {
    CYCT(b_+O(120), b_+OE(122));
    goto next_state;
  }
  CYC(b_+O(120), b_+OE(122));

set_angle:
  CYC(b_+O(122), b_+OE(124)); L = 0x09;
  CYC(b_+O(124), b_+OE(125)); mem_wr(gb, HL, C);
  CYC(b_+O(125), b_+OE(127));
  goto next_state;

hit_wall:
  CALL_C(b_+O(127), objectCreateClinkInteraction_hook, SYM(objectCreateClinkInteraction), b_+OE(130));
  CYC(b_+O(130), b_+OE(131)); H = D;
  CYC(b_+O(131), b_+OE(133)); L = 0x09;
  CYC(b_+O(133), b_+OE(134)); A = mem_rd(gb, HL);
  CYC(b_+O(134), b_+OE(136)); alu_xor(gb, 0x10);
  CYC(b_+O(136), b_+OE(137)); mem_wr(gb, HL, A);

next_state:
  CYC(b_+O(137), b_+OE(139)); L = 0x04;
  CYC(b_+O(139), b_+OE(140)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(140), b_+OE(142)); L = 0x16;
  CYC(b_+O(142), b_+OE(143)); alu_xor(gb, A);
  CYC(b_+O(143), b_+OE(144)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(144), b_+OE(145)); mem_wr(gb, HL, A);
  CYC(b_+O(145), b_+OE(147));
  goto update_speed_and_animation;

state2:
  CALL_C(b_+O(147), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(150));
  CALL_C(b_+O(150), objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+OE(153));
  CYC(b_+O(153), b_+OE(156)); SET_BC(0x140a);
  CALL_C(b_+O(156), itemCheckWithinRangeOfLink_hook, SYM(itemCheckWithinRangeOfLink), b_+OE(159));
  if (F & FC) CALL_C_CC(b_+O(159), itemIncState_hook, SYM(itemIncState), b_+OE(162));
  else CYC(b_+O(159), b_+OE(162));
  CYC(b_+O(162), b_+OE(164));
  goto break_tile_and_update;

state3:
  CALL_C(b_+O(164), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(167));
  CYC(b_+O(167), b_+OE(169)); E = 0x09;
  CYC(b_+O(169), b_+OE(170)); mem_wr(gb, DE, A);
  CYC(b_+O(170), b_+OE(173)); SET_BC(0x0402);
  CALL_C(b_+O(173), itemCheckWithinRangeOfLink_hook, SYM(itemCheckWithinRangeOfLink), b_+OE(176));
  if (!(F & FC)) {
    CYCT(b_+O(176), b_+OE(178));
    goto break_tile_and_update;
  }
  CYC(b_+O(176), b_+OE(178));
  CALL_C(b_+O(178), itemIncState_hook, SYM(itemIncState), b_+OE(181));
  CYC(b_+O(181), b_+OE(183)); L = 0x06;
  CYC(b_+O(183), b_+OE(185)); mem_wr(gb, HL, 0x04);
  CYC(b_+O(185), b_+OE(187)); L = 0x24;
  CYC(b_+O(187), b_+OE(189)); mem_wr(gb, HL, 0x00);
  CYC(b_+O(189), b_+OE(192));
  TAIL(objectSetInvisible);

state4:
  CALL_C(b_+O(192), itemDecCounter1_hook, SYM(itemDecCounter1), b_+OE(195));
  if (F & FZ) {
    CYCT(b_+O(195), b_+OE(198));
    TAIL(itemDelete);
  }
  CYC(b_+O(195), b_+OE(198));
  CYC(b_+O(198), b_+OE(201)); A = W8(wLinkObjectIndex);
  CYC(b_+O(201), b_+OE(202)); H = A;
  CYC(b_+O(202), b_+OE(204)); L = 0x0b;
  CYC(b_+O(204), b_+OE(207));
  TAIL(objectTakePosition);

break_tile_and_update:
  if (game_seasons) CALL_C(b_+S(233), s_magicBoomerangTryToBreakTile_hook, SYM(magicBoomerangTryToBreakTile), b_+S(236));

update_speed_and_animation:
  CALL_C(b_+O(207), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(210));
  CYC(b_+O(210), b_+OE(211)); H = D;
  CYC(b_+O(211), b_+OE(213)); L = 0x21;
  CYC(b_+O(213), b_+OE(214)); A = mem_rd(gb, HL);
  CYC(b_+O(214), b_+OE(215)); alu_or(gb, A);
  CYC(b_+O(215), b_+OE(217)); mem_wr(gb, HL, 0x00);
  CYC(b_+O(217), b_+OE(219)); A = 0x78;
  if (!(F & FZ)) CALL_C_CC(b_+O(219), playSound_b00_hook, SYM(playSound_b00), b_+OE(222));
  else CYC(b_+O(219), b_+OE(222));
  CYC(b_+O(222), b_+OE(225));
  TAIL(itemAnimate);
}

void itemCheckWithinRangeOfLink_hook(GB *gb) {
  BASE(itemCheckWithinRangeOfLink);
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+5); E = 0x0b;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); alu_add(gb, C);
  CYC(b_+8, b_+9); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); L = 0x0d;
  CYC(b_+12, b_+14); E = 0x0d;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); alu_add(gb, C);
  CYC(b_+17, b_+18); alu_cp(gb, B);
  CYC(b_+18, b_+19); ret_effect(gb);
}
