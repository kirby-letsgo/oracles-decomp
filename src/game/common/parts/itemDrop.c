#include "game/game.h"
#include "game/gen.h"

void s_itemDrop_pullOreChunksWithMagnetGloves_hook(GB *gb);

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/parts/itemDrop.s (PART_ITEM_DROP). Part.* fields are read/written through
// D/E or H/L exactly as the ROM does; D already holds the active part's WRAM page (set by the
// part dispatcher before calling in), so only the low-byte field offset needs to appear here.

static uint16_t itemDrop_jump_table(GB *gb) {
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

static void itemDrop_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void itemDrop_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode01_hook(GB *gb);
void itemDrop_initGfx_hook(GB *gb);
void itemDrop_countdownToDisappear_hook(GB *gb);
void itemDrop_initSpeed_hook(GB *gb);
void itemDrop_updateSpeed_hook(GB *gb);
void itemDrop_spawnEnemy_hook(GB *gb);
void itemDrop_checkSidescrollingConditions_hook(GB *gb);
void itemDrop_checkHitGround_hook(GB *gb);
void itemDrop_checkOnHazard_hook(GB *gb);
void itemDrop_updateFairyMovement_hook(GB *gb);
void itemDrop_chooseRandomFairyMovement_hook(GB *gb);
void itemDrop_moveTowardPoint_hook(GB *gb);
void itemDrop_applySpeed_hook(GB *gb);

void partCode01__linkCollectedItem_hook(GB *gb);
void partCode01__getRelatedObj1ID_hook(GB *gb);
void partCode01__checkCollidedWithLink_hook(GB *gb);
void partCode01__afterCollisionCheck_hook(GB *gb);

void partCode01_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) {
    CYCT(b_+O(0), b_+OE(2)); // jr z,@normalStatus
  } else {
    CYC(b_+O(0), b_+OE(2));
    CYC(b_+O(2), b_+OE(4)); alu_cp(gb, 0x02); // cp PARTSTATUS_DEAD
    if (F & FZ) {
      CYCT(b_+O(4), b_+OE(7)); // jp z,@linkCollectedItem
      TAIL(partCode01__linkCollectedItem);
    }
    CYC(b_+O(4), b_+OE(7));
    CYC(b_+O(7), b_+OE(9)); E = 0xc4; // Part.state
    CYC(b_+O(9), b_+OE(11)); A = 0x03;
    CYC(b_+O(11), b_+OE(12)); mem_wr(gb, DE, A);
  }

  // @normalStatus
  CYC(b_+O(12), b_+OE(15)); push_effect(gb, b_+OE(15));
  TAIL(partCode01__checkCollidedWithLink);
}

void partCode01__afterCollisionCheck_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(15), b_+OE(17)); E = 0xc4; // Part.state
  CYC(b_+O(17), b_+OE(18)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(18), b_+OE(19)); push_effect(gb, b_+OE(19));
    uint16_t target = itemDrop_jump_table(gb);
    if (target == b_+O(27)) goto state0;
    if (target == b_+O(104)) goto state1;
    if (target == b_+O(161)) goto state2;
    if (target == b_+O(198)) goto state3;
    hook_continue(gb, target, sp0_); return;
  }

state0:
  CYC(b_+O(27), b_+OE(30)); A = W8(wIsMaplePresent);
  CYC(b_+O(30), b_+OE(31)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(31), b_+OE(34)); partDelete_hook(gb); return; } // jp nz
  CYC(b_+O(31), b_+OE(34));
  CYC(b_+O(34), b_+OE(36)); E = 0xc2; // Part.subid
  CYC(b_+O(36), b_+OE(37)); A = mem_rd(gb, DE);
  CYC(b_+O(37), b_+OE(39)); alu_cp(gb, 0x0f); // ITEM_DROP_100_RUPEES_OR_ENEMY
  if (!(F & FZ)) {
    CYCT(b_+O(39), b_+OE(41)); // jr nz,@normalItem
  } else {
    CYC(b_+O(39), b_+OE(41));
    CALL_C(b_+O(41), getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+OE(44));
    CYC(b_+O(44), b_+OE(46)); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+O(46), b_+OE(49)); itemDrop_spawnEnemy_hook(gb); return; } // jp c
    CYC(b_+O(46), b_+OE(49));
    if (game_seasons) {
      CYC(b_+S(49), b_+S(52)); A = W8(wTilesetFlags);
      CYC(b_+S(52), b_+S(54)); alu_cp(gb, 0x81); // TILESETFLAG_SUBROSIA|TILESETFLAG_OUTDOORS
      if (F & FZ) { CYCT(b_+S(54), b_+S(57)); partDelete_hook(gb); return; } // jp z
      CYC(b_+S(54), b_+S(57));
    }
  }

  // @normalItem
  CALL_C(b_+O(49), itemDrop_initGfx_hook, SYM(itemDrop_initGfx), b_+OE(52));
  CYC(b_+O(52), b_+OE(53)); H = D;
  CYC(b_+O(53), b_+OE(55)); L = 0xd4; // Part.speedZ
  CYC(b_+O(55), b_+OE(57)); A = 0xa0; // <(-$160)
  CYC(b_+O(57), b_+OE(58)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(58), b_+OE(60)); mem_wr(gb, HL, 0xfe); // >(-$160)
  CYC(b_+O(60), b_+OE(62)); L = 0xc4; // Part.state
  CYC(b_+O(62), b_+OE(63)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(63), b_+OE(66)); A = W8(wTilesetFlags);
  CYC(b_+O(66), b_+OE(68)); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) {
    CYCT(b_+O(68), b_+OE(70)); // jr z,@label_11_008
    goto label_11_008;
  }
  CYC(b_+O(68), b_+OE(70));
  CYC(b_+O(70), b_+OE(71)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 2
  CYC(b_+O(71), b_+OE(73)); L = 0xe4; // Part.collisionType
  CYC(b_+O(73), b_+OE(75)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+O(75), b_+OE(77)); L = 0xc6; // Part.counter1
  CYC(b_+O(77), b_+OE(79)); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+O(79), objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+OE(82));
  if (!(F & FC)) { CYCT(b_+O(82), b_+OE(84)); goto label_11_008; } // jr nc
  CYC(b_+O(82), b_+OE(84));
  CYC(b_+O(84), b_+OE(85)); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+O(85), b_+OE(87)); goto label_11_008; } // jr nc
  CYC(b_+O(85), b_+OE(87));
  CYC(b_+O(87), b_+OE(89)); E = 0xf4; // Part.var34
  CYC(b_+O(89), b_+OE(91)); A = 0x01;
  CYC(b_+O(91), b_+OE(92)); mem_wr(gb, DE, A);

label_11_008:
  CYC(b_+O(92), b_+OE(94)); E = 0xc2; // Part.subid
  CYC(b_+O(94), b_+OE(95)); A = mem_rd(gb, DE);
  CALL_C(b_+O(95), itemDrop_initSpeed_hook, SYM(itemDrop_initSpeed), b_+OE(98));
  CYC(b_+O(98), b_+OE(100)); E = 0xc2; // Part.subid
  CYC(b_+O(100), b_+OE(101)); A = mem_rd(gb, DE);
  CYC(b_+O(101), b_+OE(104)); TAIL(partSetAnimation); // jp

state1:
  CALL_C(b_+O(104), partCommon_getTileCollisionInFront_allowHoles_hook, SYM(partCommon_getTileCollisionInFront_allowHoles), b_+OE(107));
  if (!(F & FC)) {
    CALL_C_CC(b_+O(107), itemDrop_updateSpeed_hook, SYM(itemDrop_updateSpeed), b_+OE(110)); // call nc
  } else {
    CYC(b_+O(107), b_+OE(110));
  }
  CYC(b_+O(110), b_+OE(112)); C = 0x20;
  CALL_C(b_+O(112), objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+OE(115));
  if (F & FC) { CYCT(b_+O(115), b_+OE(117)); goto doneBouncing; } // jr c
  CYC(b_+O(115), b_+OE(117));
  CALL_C(b_+O(117), itemDrop_checkHitGround_hook, SYM(itemDrop_checkHitGround), b_+OE(120));
  if (!(F & FC)) { CYCT(b_+O(120), b_+OE(122)); goto label_11_010; } // jr nc
  CYC(b_+O(120), b_+OE(122));

doneBouncing:
  CYC(b_+O(122), b_+OE(123)); H = D;
  CYC(b_+O(123), b_+OE(125)); L = 0xc4; // Part.state
  CYC(b_+O(125), b_+OE(126)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(126), b_+OE(128)); L = 0xc6; // Part.counter1
  CYC(b_+O(128), b_+OE(130)); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+O(130), objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+OE(133));

label_11_010:
  if (game_seasons) {
    CALL_C(b_+S(141), s_itemDrop_pullOreChunksWithMagnetGloves_hook, SYM(itemDrop_pullOreChunksWithMagnetGloves), b_+S(144));
    if (F & FC) { CYCT(b_+S(144), b_+S(146)); goto on_ground; } // jr c
    CYC(b_+S(144), b_+S(146));
  }
  CALL_C(b_+O(133), itemDrop_checkOnHazard_hook, SYM(itemDrop_checkOnHazard), b_+OE(136));
  if (F & FC) { RET_TAKEN(b_+O(136)); return; } // ret c
  CYC(b_+O(136), b_+OE(137));
on_ground:
  CYC(b_+O(137), b_+OE(139)); E = 0xcf; // Part.zh
  CYC(b_+O(139), b_+OE(140)); A = mem_rd(gb, DE);
  CYC(b_+O(140), b_+OE(141)); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+O(141)); return; } // ret c
  CYC(b_+O(141), b_+OE(142));
  CYC(b_+O(142), b_+OE(145)); SET_BC(0x0500);
  CALL_C(b_+O(145), objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+OE(148));
  CYC(b_+O(148), b_+OE(151)); SET_HL(SYM(itemDropConveyorTilesTable)); // itemDropConveyorTilesTable
  CALL_C(b_+O(151), lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+OE(154));
  if (!(F & FC)) { RET_TAKEN(b_+O(154)); return; } // ret nc
  CYC(b_+O(154), b_+OE(155));
  CYC(b_+O(155), b_+OE(156)); C = A;
  CYC(b_+O(156), b_+OE(158)); B = 0x14; // SPEED_80
  CYC(b_+O(158), b_+OE(161)); TAIL(itemDrop_applySpeed); // jp

state2:
  CALL_C(b_+O(161), itemDrop_checkSidescrollingConditions_hook, SYM(itemDrop_checkSidescrollingConditions), b_+OE(164));
  CALL_C(b_+O(164), itemDrop_moveTowardPoint_hook, SYM(itemDrop_moveTowardPoint), b_+OE(167));
  if (F & FC) { CYCT(b_+O(167), b_+OE(170)); goto reachedPoint; } // jp c
  CYC(b_+O(167), b_+OE(170));
  CALL_C(b_+O(170), itemDrop_countdownToDisappear_hook, SYM(itemDrop_countdownToDisappear), b_+OE(173));
  if (F & FC) { CYCT(b_+O(173), b_+OE(176)); partDelete_hook(gb); return; } // jp c
  CYC(b_+O(173), b_+OE(176));
  CYC(b_+O(176), b_+OE(178)); E = 0xc2; // Part.subid
  CYC(b_+O(178), b_+OE(179)); A = mem_rd(gb, DE);
  CYC(b_+O(179), b_+OE(180)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(180), b_+OE(182)); goto label_11_010; } // jr nz
  CYC(b_+O(180), b_+OE(182));
  CYC(b_+O(182), b_+OE(185)); TAIL(itemDrop_updateFairyMovement); // jp

reachedPoint:
  CYC(b_+O(185), b_+OE(186)); H = D;
  CYC(b_+O(186), b_+OE(188)); L = 0xf1; // Part.var31
  CYC(b_+O(188), b_+OE(189)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(189), b_+OE(190)); C = mem_rd(gb, HL); // Part.var32
  CYC(b_+O(190), b_+OE(192)); L = 0xcb; // Part.yh
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(192), b_+OE(193));
  CYC(b_+O(193), b_+OE(194)); L = L + 1; // Part.xh
  CYC(b_+O(194), b_+OE(195)); mem_wr(gb, HL, C);
  CYC(b_+O(195), b_+OE(198)); TAIL(partDelete); // jp

state3:
  CYC(b_+O(198), b_+OE(200)); E = 0xc5; // Part.substate
  CYC(b_+O(200), b_+OE(201)); A = mem_rd(gb, DE);
  CYC(b_+O(201), b_+OE(202)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(202), b_+OE(205)); // call z
    push_effect(gb, b_+OE(205));
    partCode01__getRelatedObj1ID_hook(gb);
  } else {
    CYC(b_+O(202), b_+OE(205));
  }
  CALL_C(b_+O(205), objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+OE(208));
  if (F & FC) { CYCT(b_+O(208), b_+OE(211)); partCode01__linkCollectedItem_hook(gb); return; } // jp c
  CYC(b_+O(208), b_+OE(211));
  CYC(b_+O(211), b_+OE(213)); A = 0x00; // Object.enabled
  CALL_C(b_+O(213), objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+OE(216));
  CYC(b_+O(216), b_+OE(217)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(217), b_+OE(218)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(218), b_+OE(220)); goto label_11_006; } // jr z
  CYC(b_+O(218), b_+OE(220));
  CYC(b_+O(220), b_+OE(222)); E = 0xf0; // Part.var30
  CYC(b_+O(222), b_+OE(223)); A = mem_rd(gb, DE);
  CYC(b_+O(223), b_+OE(224)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(224), b_+OE(227)); objectTakePosition_hook(gb); return; } // jp z
  CYC(b_+O(224), b_+OE(227));

label_11_006:
  CYC(b_+O(227), b_+OE(230)); TAIL(partDelete); // jp
}

void partCode01__getRelatedObj1ID_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(230), b_+OE(231)); H = D;
  CYC(b_+O(231), b_+OE(232)); L = E;
  CYC(b_+O(232), b_+OE(233)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+O(233), b_+OE(235)); L = 0xcf; // Part.zh
  CYC(b_+O(235), b_+OE(237)); mem_wr(gb, HL, 0x00);
  CYC(b_+O(237), b_+OE(239)); A = 0x01; // Object.id
  CALL_C(b_+O(239), objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+OE(242));
  CYC(b_+O(242), b_+OE(243)); A = mem_rd(gb, HL);
  CYC(b_+O(243), b_+OE(245)); E = 0xf0; // Part.var30
  CYC(b_+O(245), b_+OE(246)); mem_wr(gb, DE, A);
  CYC(b_+O(246), b_+OE(249)); TAIL(objectSetVisible80);// jp, real ret happens inside its chain
}

void partCode01__checkCollidedWithLink_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(249), b_+OE(251)); E = 0xe4; // Part.collisionType
  CYC(b_+O(251), b_+OE(252)); A = mem_rd(gb, DE);
  CYC(b_+O(252), b_+OE(253)); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+O(253)); partCode01__afterCollisionCheck_hook(gb); return; } // ret nc
  CYC(b_+O(253), b_+OE(254));
  CALL_C(b_+O(254), objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+OE(257));
  if (!(F & FC)) { RET_TAKEN(b_+O(257)); partCode01__afterCollisionCheck_hook(gb); return; } // ret nc
  CYC(b_+O(257), b_+OE(258));
  CYC(b_+O(258), b_+OE(259)); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  TAIL(partCode01__linkCollectedItem);
}

void partCode01__linkCollectedItem_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(259), b_+OE(262)); A = W8(wLinkDeathTrigger);
  CYC(b_+O(262), b_+OE(263)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(263), b_+OE(265)); goto deleteSelf; } // jr nz
  CYC(b_+O(263), b_+OE(265));
  CYC(b_+O(265), b_+OE(267)); E = 0xc2; // Part.subid
  CYC(b_+O(267), b_+OE(268)); A = mem_rd(gb, DE);
  CYC(b_+O(268), b_+OE(269)); alu_add(gb, A);
  CYC(b_+O(269), b_+OE(272)); SET_HL(GV(b_+O(315), 0x425b)); // @itemDropTreasureTable
  CYC(b_+O(272), b_+OE(273)); itemDrop_addDoubleIndexToHl_from_rst(gb, b_+OE(273));
  CYC(b_+O(273), b_+OE(274)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(274), b_+OE(275)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(275), b_+OE(277)); goto deleteSelf; } // jr z
  CYC(b_+O(275), b_+OE(277));
  CYC(b_+O(277), b_+OE(278)); B = A;
  CYC(b_+O(278), b_+OE(280)); A = 0x26; // GOLD_JOY_RING
  CALL_C(b_+O(280), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(283));
  CYC(b_+O(283), b_+OE(284)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) { CYCT(b_+O(284), b_+OE(286)); goto doubleDrop; } // jr z
  CYC(b_+O(284), b_+OE(286));
  CYC(b_+O(286), b_+OE(287)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(287), b_+OE(289)); goto giveDrop; } // jr z
  CYC(b_+O(287), b_+OE(289));
  CALL_C(b_+O(289), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(292));
  if (!(F & FZ)) { CYCT(b_+O(292), b_+OE(294)); goto giveDrop; } // jr nz
  CYC(b_+O(292), b_+OE(294));

doubleDrop:
  CYC(b_+O(294), b_+OE(295)); SET_HL(HL + 1);

giveDrop:
  CYC(b_+O(295), b_+OE(296)); C = mem_rd(gb, HL);
  CYC(b_+O(296), b_+OE(297)); A = B;
  CALL_C(b_+O(297), giveTreasure_hook, SYM(giveTreasure), b_+OE(300));
  CYC(b_+O(300), b_+OE(302)); E = 0xc2; // Part.subid
  CYC(b_+O(302), b_+OE(303)); A = mem_rd(gb, DE);
  CYC(b_+O(303), b_+OE(305)); alu_cp(gb, 0x0e); // ITEM_DROP_50_ORE_CHUNKS
  if (!(F & FZ)) { CYCT(b_+O(305), b_+OE(307)); goto deleteSelf; } // jr nz
  CYC(b_+O(305), b_+OE(307));
  CALL_C(b_+O(307), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(310));
  CYC(b_+O(310), b_+OE(312)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);

deleteSelf:
  CYC(b_+O(312), b_+OE(315)); TAIL(partDelete); // jp
}

void itemDrop_initGfx_hook(GB *gb) {
  BASE(itemDrop_initGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+21); // @spriteData
  CYC(b_+6, b_+7); itemDrop_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); E = 0xdd; // Part.oamTileIndexBase
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = A + mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+14); E = 0xdc; // Part.oamFlags
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A); // [oamFlags]
  CYC(b_+16, b_+17); E = 0xdb; // Part.oamFlagsBackup
  CYC(b_+17, b_+18); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(b_+18, b_+21); TAIL(objectSetVisiblec1); // jp
}

void itemDrop_countdownToDisappear_hook(GB *gb) {
  BASE(itemDrop_countdownToDisappear);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+4); alu_xor(gb, D);
  CYC(b_+4, b_+5); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0xf3; // Part.var33
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13); // jr z,L_42d6
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
    CYC(b_+14, b_+15);
    CYC(b_+15, b_+17); L = 0xe4; // Part.collisionType
    CYC(b_+17, b_+19); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  }
  CALL_C(b_+19, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+22);
  if (F & FZ) { CYCT(b_+22, b_+24); goto disappear; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 60);
  if (!(F & FC)) { RET_TAKEN(b_+27); return; } // ret nc
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); L = 0xda; // Part.visible
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_xor(gb, 0x80);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); ret_effect(gb); return;

disappear:
  CYC(b_+35, b_+36); alu_scf(gb);
  CYC(b_+36, b_+37); ret_effect(gb); return;
}

void itemDrop_initSpeed_hook(GB *gb) {
  BASE(itemDrop_initSpeed);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+2, b_+4); goto fairy; } // jr z
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); E = 0xc3; // Part.var03
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = 0xd0; // Part.speed
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x19); // SPEED_a0
  CYC(b_+13, b_+14); ret_effect(gb); return;

fairy:
  CYC(b_+14, b_+16); L = 0xcf; // Part.zh
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  CYC(b_+19, b_+21); L = 0xcb; // Part.yh
  CYC(b_+21, b_+22); A = A + mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+26); TAIL(itemDrop_chooseRandomFairyMovement); // jp
}

void itemDrop_updateSpeed_hook(GB *gb) {
  BASE(itemDrop_updateSpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); TAIL(objectApplySpeed); // jp
}

void itemDrop_spawnEnemy_hook(GB *gb) {
  BASE(itemDrop_spawnEnemy);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(1)); C = A;
  CYC(b_+O(1), b_+OE(4)); A = W8(wDiggingUpEnemiesForbidden);
  CYC(b_+O(4), b_+OE(5)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(5), b_+OE(7)); goto delete_; } // jr nz
  CYC(b_+O(5), b_+OE(7));
  if (game_seasons) {
    CYC(b_+S(7), b_+S(9)); B = 0x2d;
    CYC(b_+S(9), b_+S(12)); A = W8(wTilesetFlags);
    CYC(b_+S(12), b_+S(14)); alu_cp(gb, 0x81);
    if (F & FZ) { CYCT(b_+S(14), b_+S(16)); goto spawn; }
    CYC(b_+S(14), b_+S(16));
  }
  CYC(b_+O(7), b_+OE(8)); A = C;
  CYC(b_+O(8), b_+OE(10)); alu_and(gb, 0x07);
  CYC(b_+O(10), b_+OE(13)); SET_HL(b_+O(33)); // @enemiesToSpawn
  CYC(b_+O(13), b_+OE(14)); itemDrop_addAToHl_from_rst(gb, b_+OE(14));
  CYC(b_+O(14), b_+OE(15)); B = mem_rd(gb, HL);
spawn:
  CALL_C(b_+O(15), getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+OE(18));
  if (!(F & FZ)) { CYCT(b_+O(18), b_+OE(20)); goto delete_; } // jr nz
  CYC(b_+O(18), b_+OE(20));
  CYC(b_+O(20), b_+OE(21)); mem_wr(gb, HL, B);
  CALL_C(b_+O(21), objectCopyPosition_hook, SYM(objectCopyPosition), b_+OE(24));
  CYC(b_+O(24), b_+OE(26)); E = 0xc3; // Part.var03
  CYC(b_+O(26), b_+OE(27)); A = mem_rd(gb, DE);
  CYC(b_+O(27), b_+OE(29)); L = 0x82; // Enemy.subid
  CYC(b_+O(29), b_+OE(30)); mem_wr(gb, HL, A);

delete_:
  CYC(b_+O(30), b_+OE(33)); TAIL(partDelete); // jp
}

void itemDrop_checkSidescrollingConditions_hook(GB *gb) {
  BASE(itemDrop_checkSidescrollingConditions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0xc2; // Part.subid
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x20;
  CALL_C(b_+13, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+16);
  if (F & FC) { CYCT(b_+16, b_+18); goto checkY; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); E = 0xf4; // Part.var34
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+22, b_+24); goto checkY; } // jr nc
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); B = 0x01;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); // [speedZ+1]
  CYC(b_+27, b_+29); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+29, b_+31); goto after_sign; } // jr z
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); B = 0xff;
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);

after_sign:
  CYC(b_+34, b_+36); alu_cp(gb, 0x01);
  if (F & FC) { RET_TAKEN(b_+36); return; } // ret c
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); mem_wr(gb, HL, B); // [speedZ+1]
  CYC(b_+38, b_+39); L = L - 1;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x00); // [speedZ]

checkY:
  CYC(b_+41, b_+43); E = 0xcb; // Part.yh
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(b_+46); return; } // ret c
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  CYC(b_+48, b_+51); TAIL(partDelete); // jp
}

void itemDrop_checkHitGround_hook(GB *gb) {
  BASE(itemDrop_checkHitGround);
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto fairy; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = 0xd5; // Part.speedZ+1
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0xe4; // Part.collisionType
  CYC(b_+15, b_+17); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+17, b_+18); ret_effect(gb); return;

fairy:
  CYC(b_+18, b_+20); E = 0xcf; // Part.zh
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_cp(gb, 0xfa);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; } // ret nc
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+26); L = E;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xfa); // [Part.zh]
  CYC(b_+28, b_+30); L = 0xf3; // Part.var33
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x05);
  CYC(b_+32, b_+33); ret_effect(gb); return;
}

void itemDrop_checkOnHazard_hook(GB *gb) {
  BASE(itemDrop_checkOnHazard);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+3);
  if (F & FC) { CYCT(b_+3, b_+5); goto onHazard; } // jr c
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = 0xf4; // Part.var34
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; } // ret nc
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); B = 0x03; // INTERAC_SPLASH
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); goto onWaterSidescrolling; // jr (unconditional)

onHazard:
  CYC(b_+15, b_+16); alu_rrca(gb);
  if (F & FC) { CYCT(b_+16, b_+18); goto onWater; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_rrca(gb);
  CYC(b_+19, b_+21); B = 0x04; // INTERAC_LAVASPLASH
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto replaceWithAnimation; } // jr nc
  CYC(b_+21, b_+23);
  CALL_C(b_+23, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+26);
  CYC(b_+26, b_+28); goto delete_; // jr (unconditional)

replaceWithAnimation:
  CALL_C(b_+28, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+31);

delete_:
  CALL_C(b_+31, partDelete_hook, SYM(partDelete), b_+34);
  CYC(b_+34, b_+35); alu_scf(gb);
  CYC(b_+35, b_+36); ret_effect(gb); return;

onWater:
  CYC(b_+36, b_+38); B = 0x03; // INTERAC_SPLASH
  CYC(b_+38, b_+41); A = W8(wTilesetFlags);
  CYC(b_+41, b_+43); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) { CYCT(b_+43, b_+45); goto replaceWithAnimation; } // jr z
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); E = 0xf4; // Part.var34
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_rrca(gb);
  CYC(b_+49, b_+50); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+50); return; } // ret nc
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); A = 0x01;

onWaterSidescrolling:
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+57); TAIL(objectCreateInteractionWithSubid00); // jp
}

void itemDrop_updateFairyMovement_hook(GB *gb) {
  BASE(itemDrop_updateFairyMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc7; // Part.counter2
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+4, b_+6); itemDrop_chooseRandomFairyMovement_hook(gb); return; } // jr z
  CYC(b_+4, b_+6);
  CALL_C(b_+6, partCommon_getTileCollisionInFront_hook, SYM(partCommon_getTileCollisionInFront), b_+9);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  TAIL(itemDrop_chooseRandomFairyMovement); // fallthrough
}

void itemDrop_chooseRandomFairyMovement_hook(GB *gb) {
  BASE(itemDrop_chooseRandomFairyMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x3e);
  CYC(b_+5, b_+7); A = A + 0x08;
  CYC(b_+7, b_+9); E = 0xc7; // Part.counter2
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x03);
  CYC(b_+15, b_+18); SET_HL(b_+49); // @speedTable
  CYC(b_+18, b_+19); itemDrop_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+21); E = 0xd0; // Part.speed
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 0x1e);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = 0xc9; // Part.angle
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+34); return; } // ret z
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+37, b_+39); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); A = A & ~0x20;
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto set_flags; } // jr nz
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); A = A | 0x20;

set_flags:
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+49); ret_effect(gb); return;
}

void itemDrop_moveTowardPoint_hook(GB *gb) {
  BASE(itemDrop_moveTowardPoint);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0xf1; // Part.var31
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+5); B = mem_rd(gb, HL); // [var31]
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); C = mem_rd(gb, HL); // [var32]
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); alu_or(gb, B);
  if (F & FZ) { RET_TAKEN(b_+9); return; } // ret z
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); push_effect(gb, BC);
  CALL_C(b_+11, objectCheckContainsPoint_hook, SYM(objectCheckContainsPoint), b_+14);
  CYC(b_+14, b_+15); SET_BC(pop_effect(gb));
  if (F & FC) { RET_TAKEN(b_+15); return; } // ret c
  CYC(b_+15, b_+16);
  CALL_C(b_+16, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+19);
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+22); B = 0x0a; // SPEED_40
  CYC(b_+22, b_+24); E = 0xc9; // Part.angle
  CALL_C(b_+24, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+29); ret_effect(gb); return;
}

void itemDrop_applySpeed_hook(GB *gb) {
  BASE(itemDrop_applySpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); A = C;
  CALL_C(b_+2, partCommon_getTileCollisionAtAngle_allowHoles_hook, SYM(partCommon_getTileCollisionAtAngle_allowHoles), b_+5);
  CYC(b_+5, b_+6); SET_BC(pop_effect(gb));
  if (F & FC) { RET_TAKEN(b_+6); return; } // ret c
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); E = 0xc9; // Part.angle
  CALL_C(b_+9, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+12);
  CYC(b_+12, b_+13); alu_scf(gb);
  CYC(b_+13, b_+14); ret_effect(gb); return;
}
