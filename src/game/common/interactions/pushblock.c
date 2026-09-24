#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define pushableTilePropertiesTable_bank08 SYM(pushableTilePropertiesTable)

#define OBJ_VAR30 0x30
#define OBJ_VAR31 0x31

#define TILEINDEX_BUTTON 0x0c
#define TILEINDEX_RED_PUSHABLE_BLOCK 0x2c

static uint16_t pushblock_jumpTable(GB *gb) {
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

static void pushblock_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

// interactionCode14@updateZPositionForButton: raise z by 2 pixels when sitting on an unpressed button.
static void pushblock_updateZPositionForButton(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+O(154), b_+OE(157)); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+O(157), b_+OE(159)); alu_and(gb, 0x18);
  if (F & FZ) {
    CYCT(b_+O(159), b_+OE(160)); ret_effect(gb); return;
  }
  CYC(b_+O(159), b_+OE(160));
  CALL_C(b_+O(160), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(163));
  CYC(b_+O(163), b_+OE(164)); C = A;
  CYC(b_+O(164), b_+OE(166)); B = wRoomLayout >> 8;
  CYC(b_+O(166), b_+OE(167)); A = mem_rd(gb, BC);
  CYC(b_+O(167), b_+OE(169)); alu_cp(gb, TILEINDEX_BUTTON);
  CYC(b_+O(169), b_+OE(171)); A = 0xfe;
  if (F & FZ) {
    CYCT(b_+O(171), b_+OE(173));
  } else {
    CYC(b_+O(171), b_+OE(173));
    CYC(b_+O(173), b_+OE(174)); alu_xor(gb, A);
  }
  CYC(b_+O(174), b_+OE(176)); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+O(176), b_+OE(177)); mem_wr(gb, DE, A);
  CYC(b_+O(177), b_+OE(178)); ret_effect(gb);
}

// interactionCode14@replaceTileUnderneathBlock: restore the ground tile at var30 (from the room
// layout buffer if non-solid, else var32). Tails into setTile, whose ret pops our return address.
static void pushblock_replaceTileUnderneathBlock(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+O(178), b_+OE(180)); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+O(180), b_+OE(181)); A = mem_rd(gb, DE);
  CYC(b_+O(181), b_+OE(182)); C = A;
  CALL_C(b_+O(182), getTileIndexFromRoomLayoutBuffer_paramC_hook, SYM(getTileIndexFromRoomLayoutBuffer_paramC), b_+OE(185));
  if (!(F & FC)) {
    CYCT(b_+O(185), b_+OE(188)); TAIL(setTile);
  }
  CYC(b_+O(185), b_+OE(188));
  CYC(b_+O(188), b_+OE(190)); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+O(190), b_+OE(191)); A = mem_rd(gb, DE);
  CYC(b_+O(191), b_+OE(194)); setTile_hook(gb);
}

// interactionCode14@checkRotatingCubePermitsPushing: carry set means this block may not be pushed.
static void pushblock_checkRotatingCubePermitsPushing(GB *gb) {
  BASE(interactionCode14);
  AGES_ONLY();
  CYC(b_+194, b_+197); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+197, b_+198); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+198, b_+199); ret_effect(gb); return;
  }
  CYC(b_+198, b_+199);
  CYC(b_+199, b_+202); A = mem_rd(gb, wRotatingCubeColor);
  CYC(b_+202, b_+204); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+204, b_+206); goto deny;
  }
  CYC(b_+204, b_+206);
  CYC(b_+206, b_+208); alu_and(gb, 0x7f);
  CYC(b_+208, b_+209); B = A;
  CYC(b_+209, b_+211); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(b_+211, b_+212); A = mem_rd(gb, DE);
  CYC(b_+212, b_+214); alu_sub(gb, TILEINDEX_RED_PUSHABLE_BLOCK);
  CYC(b_+214, b_+215); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+215, b_+216); ret_effect(gb); return;
  }
  CYC(b_+215, b_+216);
deny:
  CYC(b_+216, b_+217); alu_scf(gb);
  CYC(b_+217, b_+218); ret_effect(gb);
}

// interactionCode14@loadPushableTileProperties: look up var31 in the pushable tile table for the
// active collision set and copy its 4 bytes into var31-var34.
static void pushblock_loadPushableTileProperties(GB *gb) {
  BASE(interactionCode14);
  CYC(b_+O(218), b_+OE(221)); A = GVW(wActiveCollisions, wActiveGroup);
  CYC(b_+O(221), b_+OE(224)); SET_HL(pushableTilePropertiesTable_bank08);
  CYC(b_+O(224), b_+OE(225)); push_effect(gb, b_+OE(225)); pushblock_addAToHl(gb);
  CYC(b_+O(225), b_+OE(226)); A = mem_rd(gb, HL);
  CYC(b_+O(226), b_+OE(227)); push_effect(gb, b_+OE(227)); pushblock_addAToHl(gb);
  CYC(b_+O(227), b_+OE(229)); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(b_+O(229), b_+OE(230)); A = mem_rd(gb, DE);
  CYC(b_+O(230), b_+OE(231)); B = A;
search:
  CYC(b_+O(231), b_+OE(232)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(232), b_+OE(233)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(233), b_+OE(234)); ret_effect(gb); return;
  }
  CYC(b_+O(233), b_+OE(234));
  CYC(b_+O(234), b_+OE(235)); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+O(235), b_+OE(237)); goto match;
  }
  CYC(b_+O(235), b_+OE(237));
  CYC(b_+O(237), b_+OE(238)); SET_HL(HL + 1);
  CYC(b_+O(238), b_+OE(239)); SET_HL(HL + 1);
  CYC(b_+O(239), b_+OE(240)); SET_HL(HL + 1);
  CYC(b_+O(240), b_+OE(242)); goto search;
match:
  CYC(b_+O(242), b_+OE(243)); mem_wr(gb, DE, A);
  CYC(b_+O(243), b_+OE(244)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(244), b_+OE(245)); E = alu_inc8(gb, E);
  CYC(b_+O(245), b_+OE(246)); mem_wr(gb, DE, A);
  CYC(b_+O(246), b_+OE(247)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(247), b_+OE(248)); E = alu_inc8(gb, E);
  CYC(b_+O(248), b_+OE(249)); mem_wr(gb, DE, A);
  CYC(b_+O(249), b_+OE(250)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(250), b_+OE(251)); E = alu_inc8(gb, E);
  CYC(b_+O(251), b_+OE(252)); mem_wr(gb, DE, A);
  CYC(b_+O(252), b_+OE(253)); ret_effect(gb);
}

// interactionCode14@func_449d: the block has stopped; settle it and delete self.
static void pushblock_func_449d(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CALL_C(b_+O(109), objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+OE(112));
  if (F & FC) {
    CYCT(b_+O(112), b_+OE(115)); TAIL(interactionDelete);
  }
  CYC(b_+O(112), b_+OE(115));
  CALL_C(b_+O(115), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(118));
  CYC(b_+O(118), b_+OE(120)); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+O(120), b_+OE(121)); mem_wr(gb, DE, A);
  CYC(b_+O(121), b_+OE(123)); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+O(123), b_+OE(124)); A = mem_rd(gb, DE);
  CYC(b_+O(124), b_+OE(125)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(125), b_+OE(127)); goto checkSound;
  }
  CYC(b_+O(125), b_+OE(127));
  CYC(b_+O(127), b_+OE(128)); B = A;
  CYC(b_+O(128), b_+OE(130)); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+O(130), b_+OE(131)); A = mem_rd(gb, DE);
  CYC(b_+O(131), b_+OE(132)); C = A;
  CYC(b_+O(132), b_+OE(133)); A = B;
  CALL_C(b_+O(133), setTile_hook, SYM(setTile), b_+OE(136));
checkSound:
  CYC(b_+O(136), b_+OE(138)); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+O(138), b_+OE(139)); A = mem_rd(gb, DE);
  CYC(b_+O(139), b_+OE(140)); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+O(140), b_+OE(142)); goto delete;
  }
  CYC(b_+O(140), b_+OE(142));
  CYC(b_+O(142), b_+OE(143)); alu_xor(gb, A);
  CYC(b_+O(143), b_+OE(146)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(146), b_+OE(148)); A = 0x4d;
  CALL_C(b_+O(148), playSound_b00_hook, SYM(playSound_b00), b_+OE(151));
delete:
  CYC(b_+O(151), b_+OE(154)); interactionDelete_hook(gb);
}

// interactionCode14@state1 (also reached by fallthrough from state0): move the block until
// counter1 expires.
static void pushblock_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+O(96), b_+OE(99)); push_effect(gb, b_+OE(99)); pushblock_updateZPositionForButton(gb, sp0_);
  CALL_C(b_+O(99), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(102));
  CALL_C(b_+O(102), objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+OE(105));
  CALL_C(b_+O(105), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(108));
  if (!(F & FZ)) {
    CYCT(b_+O(108), b_+OE(109)); ret_effect(gb); return;
  }
  CYC(b_+O(108), b_+OE(109));
  pushblock_func_449d(gb, sp0_);
}

// interactionCode14@state2@func_438a (Seasons): collisions of the tile ahead of the block.
static void pushblock_tileAhead(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+S(169), b_+S(171)); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+S(171), b_+S(172)); A = mem_rd(gb, DE);
  CALL_C(b_+S(172), convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+S(175));
  CYC(b_+S(175), b_+S(178)); SET_HL(b_+S(193));
  CYC(b_+S(178), b_+S(179)); push_effect(gb, b_+S(179));
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
  CYC(b_+S(179), b_+S(181)); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+S(181), b_+S(182)); A = mem_rd(gb, DE);
  CYC(b_+S(182), b_+S(183)); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+S(183), b_+S(184)); B = A;
  CYC(b_+S(184), b_+S(185)); SET_HL(HL + 1);
  CYC(b_+S(185), b_+S(187)); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+S(187), b_+S(188)); A = mem_rd(gb, DE);
  CYC(b_+S(188), b_+S(189)); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+S(189), b_+S(190)); C = A;
  CYC(b_+S(190), b_+S(193)); TAIL(getTileCollisionsAtPosition);
}

// interactionCode14@state2 (Seasons): slide at SPEED_1c0 until something solid is ahead.
static void pushblock_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+S(144), b_+S(147)); push_effect(gb, b_+S(147)); pushblock_updateZPositionForButton(gb, sp0_);
  CYC(b_+S(147), b_+S(149)); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+S(149), b_+S(151)); A = 0x46;
  CYC(b_+S(151), b_+S(152)); mem_wr(gb, DE, A);
  CALL_C(b_+S(152), objectApplySpeed_hook, SYM(objectApplySpeed), b_+S(155));
  CALL_C(b_+S(155), objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+S(158));
  CYC(b_+S(158), b_+S(161)); push_effect(gb, b_+S(161)); pushblock_tileAhead(gb, sp0_);
  if (F & FZ) { CYCT(b_+S(161), b_+S(162)); ret_effect(gb); return; }
  CYC(b_+S(161), b_+S(162));
  CYC(b_+S(162), b_+S(164)); A = 0x50;
  CALL_C(b_+S(164), playSound_b00_hook, SYM(playSound_b00), b_+S(167));
  CYC(b_+S(167), b_+S(169));
  pushblock_func_449d(gb, sp0_);
}

// interactionCode14@state0: block just pushed. Falls through into state1.
static void pushblock_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+O(8), b_+OE(10)); A = 0x01;
  CYC(b_+O(10), b_+OE(11)); mem_wr(gb, DE, A);
  CALL_C(b_+O(11), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(14));
  CYC(b_+O(14), b_+OE(16)); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+O(16), b_+OE(17)); A = mem_rd(gb, DE);
  CYC(b_+O(17), b_+OE(18)); C = A;
  CYC(b_+O(18), b_+OE(20)); B = wRoomLayout >> 8;
  CYC(b_+O(20), b_+OE(21)); A = mem_rd(gb, BC);
  CYC(b_+O(21), b_+OE(23)); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(b_+O(23), b_+OE(24)); mem_wr(gb, DE, A);
  CALL_C(b_+O(24), objectMimicBgTile_hook, SYM(objectMimicBgTile), b_+OE(27));
  if (!game_seasons) {
    CYC(b_+27, b_+30); push_effect(gb, b_+30); pushblock_checkRotatingCubePermitsPushing(gb);
    if (F & FC) {
      CYCT(b_+30, b_+33); TAIL(interactionDelete);
    }
    CYC(b_+30, b_+33);
  }
  CYC(b_+O(33), b_+OE(35)); A = 0x06;
  CALL_C(b_+O(35), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(38));
  CYC(b_+O(38), b_+OE(41)); push_effect(gb, b_+OE(41)); pushblock_loadPushableTileProperties(gb);
  CYC(b_+O(41), b_+OE(42)); H = D;
  CYC(b_+O(42), b_+OE(44)); L = INTERACTION_BASE + OBJ_VAR34;
  if (game_seasons) {
    CYC(b_+S(40), b_+S(41)); A = mem_rd(gb, HL);
    CYC(b_+S(41), b_+S(43)); alu_and(gb, 0x02);
    if (F & FZ) CYCT(b_+S(43), b_+S(45));
    else {
      CYC(b_+S(43), b_+S(45));
      CYC(b_+S(45), b_+S(47)); E = INTERACTION_BASE + OBJ_STATE;
      CYC(b_+S(47), b_+S(49)); A = 0x02;
      CYC(b_+S(49), b_+S(50)); mem_wr(gb, DE, A);
    }
  }
  CYC(b_+O(44), b_+OE(46)); alu_bit(gb, 2, mem_rd(gb, HL));
  CYC(b_+O(46), b_+OE(48)); A = 0x01;
  if (!(F & FZ)) {
    CALL_C_CC(b_+O(48), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(51));
  } else {
    CYC(b_+O(48), b_+OE(51));
  }
  CYC(b_+O(51), b_+OE(52)); H = D;
  CYC(b_+O(52), b_+(game_seasons ? S(61) : 55)); SET_BC(0x1420);
  if (!game_seasons) {
    CYC(b_+55, b_+58); A = mem_rd(gb, wBraceletLevel);
    CYC(b_+58, b_+60); alu_cp(gb, 0x02);
    if (!(F & FZ)) {
      CYCT(b_+60, b_+62); goto setSpeed;
    }
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_VAR34;
    CYC(b_+64, b_+66); alu_bit(gb, 5, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(b_+66, b_+68); goto setSpeed;
    }
    CYC(b_+66, b_+68);
    CYC(b_+68, b_+71); SET_BC(0x1e15);
  }
setSpeed:
  CYC(b_+O(71), b_+OE(73)); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+O(73), b_+OE(74)); mem_wr(gb, HL, B);
  CYC(b_+O(74), b_+OE(76)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(76), b_+OE(77)); mem_wr(gb, HL, C);
  CYC(b_+O(77), b_+OE(79)); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(79), b_+OE(80)); A = mem_rd(gb, HL);
  CYC(b_+O(80), b_+OE(82)); alu_or(gb, 0x80);
  CYC(b_+O(82), b_+OE(85)); mem_wr(gb, wBlockPushAngle, A);
  CYC(b_+O(85), b_+OE(88)); push_effect(gb, b_+OE(88)); pushblock_replaceTileUnderneathBlock(gb, sp0_);
  CALL_C(b_+O(88), objectSetVisible82_hook, SYM(objectSetVisible82), b_+OE(91));
  CYC(b_+O(91), b_+OE(93)); A = 0x71;
  CALL_C(b_+O(93), playSound_b00_hook, SYM(playSound_b00), b_+OE(96));
  pushblock_state1(gb, sp0_);
}

void interactionCode14_hook(GB *gb) {
  BASE(interactionCode14);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (pushblock_jumpTable(gb));
    if (jt_ == b_+O(8)) { pushblock_state0(gb, sp0_); return; }
    else if (jt_ == b_+O(96)) { pushblock_state1(gb, sp0_); return; }
    else if (game_seasons && jt_ == b_+S(144)) { pushblock_state2(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
