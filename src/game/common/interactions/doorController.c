#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Data format: b0 = tile to transition into, b1 = tile to transition from.
#define doorControllerShutterTiles_bank08 SYM(interactionCode1e__shutterTiles)
#define doorControllerScriptSubidTable_bank08 SYM(interactionCode1e__scriptSubidTable)

static uint16_t doorController_jumpTable(GB *gb) {
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

static void doorController_addAToHl(GB *gb) {
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

static void doorController_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode1e@playSoundIfInScreenBoundary: plays sound A if the door is on screen.
// Reached only by static `call`s from interactionCode1e itself.
static void doorController_playSoundIfInScreenBoundary(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(297), b_+OE(299)); H8(hFF8B) = A;
  CALL_C(b_+O(299), objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+OE(302));
  if (!(F & FC)) {
    CYCT(b_+O(302), b_+OE(303)); ret_effect(gb); return;
  }
  CYC(b_+O(302), b_+OE(303));
  CYC(b_+O(303), b_+OE(305)); A = H8(hFF8B);
  CYC(b_+O(305), b_+OE(308)); playSound_b00_hook(gb);
}

// interactionCode1e@checkRespawnLink: force Link to respawn if he's on the same tile as
// this object.
static void doorController_checkRespawnLink(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+O(229), b_+OE(232)); A = W8(w1Link_yh);
  CYC(b_+O(232), b_+OE(234)); alu_and(gb, 0xf0);
  CYC(b_+O(234), b_+OE(235)); B = A;
  CYC(b_+O(235), b_+OE(238)); A = W8(w1Link_xh);
  CYC(b_+O(238), b_+OE(240)); A = alu_swap(gb, A);
  CYC(b_+O(240), b_+OE(242)); alu_and(gb, 0x0f);
  CYC(b_+O(242), b_+OE(243)); alu_or(gb, B);
  CYC(b_+O(243), b_+OE(244)); B = A;
  CYC(b_+O(244), b_+OE(246)); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+O(246), b_+OE(247)); A = mem_rd(gb, DE);
  CYC(b_+O(247), b_+OE(248)); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+O(248), b_+OE(249)); ret_effect(gb); return;
  }
  CYC(b_+O(248), b_+OE(249));
  CYC(b_+O(249), b_+OE(251)); A = 0x02;
  CYC(b_+O(251), b_+OE(254)); mem_wr(gb, wScreenTransitionDelay, A);
  CYC(b_+O(254), b_+OE(257)); respawnLink_hook(gb);
}

// interactionCode1e@func_47ee: for subids >= 4, count this door as one more closed door.
static void doorController_func_47ee(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+O(266), b_+OE(268)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(268), b_+OE(269)); A = mem_rd(gb, DE);
  CYC(b_+O(269), b_+OE(271)); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+O(271), b_+OE(272)); ret_effect(gb); return;
  }
  CYC(b_+O(271), b_+OE(272));
  CYC(b_+O(272), b_+OE(275)); SET_HL(wcc93);
  CYC(b_+O(275), b_+OE(276)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(276), b_+OE(277)); ret_effect(gb);
}

// interactionCode1e@func_47e5: if the door's tile is not solid, fall through into func_47ee.
static void doorController_func_47e5(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+O(257), b_+OE(259)); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+O(259), b_+OE(260)); A = mem_rd(gb, DE);
  CYC(b_+O(260), b_+OE(261)); C = A;
  CYC(b_+O(261), b_+OE(263)); B = wRoomCollisions >> 8;
  CYC(b_+O(263), b_+OE(264)); A = mem_rd(gb, BC);
  CYC(b_+O(264), b_+OE(265)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(265), b_+OE(266)); ret_effect(gb); return;
  }
  CYC(b_+O(265), b_+OE(266));
  doorController_func_47ee(gb);
}

// interactionCode1e@func_47f9: for subids >= 4, count one closed door fewer.
static void doorController_func_47f9(GB *gb) {
  BASE(interactionCode1e);
  CYC(b_+O(277), b_+OE(279)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(279), b_+OE(280)); A = mem_rd(gb, DE);
  CYC(b_+O(280), b_+OE(282)); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+O(282), b_+OE(283)); ret_effect(gb); return;
  }
  CYC(b_+O(282), b_+OE(283));
  CYC(b_+O(283), b_+OE(286)); SET_HL(wcc93);
  CYC(b_+O(286), b_+OE(287)); A = mem_rd(gb, HL);
  CYC(b_+O(287), b_+OE(288)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(288), b_+OE(289)); ret_effect(gb); return;
  }
  CYC(b_+O(288), b_+OE(289));
  CYC(b_+O(289), b_+OE(290)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+O(290), b_+OE(291)); A = mem_rd(gb, HL);
  CYC(b_+O(291), b_+OE(293)); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(b_+O(293), b_+OE(294)); ret_effect(gb); return;
  }
  CYC(b_+O(293), b_+OE(294));
  CYC(b_+O(294), b_+OE(296)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+O(296), b_+OE(297)); ret_effect(gb);
}

// @state1: run the script; it decides when to move to state 2 (open) or 3 (close).
static void doorController_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CALL_C(b_+O(72), interactionRunScript_hook, SYM(interactionRunScript), b_+OE(75));
  if (F & FC) {
    CYCT(b_+O(75), b_+OE(78)); TAIL(interactionDelete);
  }
  CYC(b_+O(75), b_+OE(78));
  CYC(b_+O(78), b_+OE(80)); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+O(80), b_+OE(81)); alu_xor(gb, A);
  CYC(b_+O(81), b_+OE(82)); mem_wr(gb, DE, A);
  CYC(b_+O(82), b_+OE(83)); ret_effect(gb);
}

// @gotoState1: back to state 1 / substate 0, then run state 1 immediately.
static void doorController_gotoState1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(218), b_+OE(220)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(220), b_+OE(222)); A = 0x01;
  CYC(b_+O(222), b_+OE(223)); mem_wr(gb, DE, A);
  CYC(b_+O(223), b_+OE(224)); E = alu_inc8(gb, E);
  CYC(b_+O(224), b_+OE(225)); alu_xor(gb, A);
  CYC(b_+O(225), b_+OE(226)); mem_wr(gb, DE, A);
  CYC(b_+O(226), b_+OE(229));
  doorController_state1(gb, sp0_);
}

// @setTileAndPlaySound: HL points at the final tile; write it and play the door sound.
static void doorController_setTileAndPlaySound(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(205), b_+OE(207)); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+O(207), b_+OE(208)); A = mem_rd(gb, DE);
  CYC(b_+O(208), b_+OE(209)); C = A;
  CYC(b_+O(209), b_+OE(210)); A = mem_rd(gb, HL);
  CALL_C(b_+O(210), setTile_hook, SYM(setTile), b_+OE(213));
  CYC(b_+O(213), b_+OE(215)); A = 0x70; // SND_DOORCLOSE
  CYC(b_+O(215), b_+OE(218)); push_effect(gb, b_+OE(218)); doorController_playSoundIfInScreenBoundary(gb, sp0_);
  doorController_gotoState1(gb, sp0_);
}

// @interleaveDoorTile: start the half-open/half-closed door animation tile.
static void doorController_interleaveDoorTile(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(101), b_+OE(103)); A = 0x70; // SND_DOORCLOSE
  CYC(b_+O(103), b_+OE(106)); push_effect(gb, b_+OE(106)); doorController_playSoundIfInScreenBoundary(gb, sp0_);
  CYC(b_+O(106), b_+OE(108)); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(108), b_+OE(109)); A = mem_rd(gb, DE);
  CYC(b_+O(109), b_+OE(112)); SET_HL(doorControllerShutterTiles_bank08);
  CYC(b_+O(112), b_+OE(113)); push_effect(gb, b_+OE(113)); doorController_addAToHl(gb);
  CYC(b_+O(113), b_+OE(115)); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+O(115), b_+OE(116)); A = mem_rd(gb, DE);
  CYC(b_+O(116), b_+OE(118)); H8(hFF8C) = A;
  CYC(b_+O(118), b_+OE(119)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(119), b_+OE(121)); H8(hFF8F) = A;
  CYC(b_+O(121), b_+OE(122)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(122), b_+OE(124)); H8(hFF8E) = A;
  CYC(b_+O(124), b_+OE(126)); alu_and(gb, 0x03);
  CALL_C(b_+O(126), setInterleavedTile_hook, SYM(setInterleavedTile), b_+OE(129));
  CYC(b_+O(129), b_+OE(131)); A = H8(hActiveObject);
  CYC(b_+O(131), b_+OE(132)); D = A;
  CYC(b_+O(132), b_+OE(133)); H = D;
  CYC(b_+O(133), b_+OE(135)); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+O(135), b_+OE(136)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(136), b_+OE(138)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(138), b_+OE(140)); mem_wr(gb, HL, 0x06);
  // Set the new tile in the room layout (without calling setTile, so no visual update yet).
  CYC(b_+O(140), b_+OE(142)); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+O(142), b_+OE(143)); C = mem_rd(gb, HL);
  CYC(b_+O(143), b_+OE(145)); B = wRoomLayout >> 8;
  CYC(b_+O(145), b_+OE(147)); A = H8(hFF8F);
  CYC(b_+O(147), b_+OE(148)); mem_wr(gb, BC, A);
  CYC(b_+O(148), b_+OE(149)); ret_effect(gb);
}

// @state0: init. "xh" is a parameter (0-7): a bit index for wActiveTriggers.
static void doorController_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(24), b_+OE(26)); A = 0x01;
  CYC(b_+O(26), b_+OE(27)); mem_wr(gb, DE, A);
  CYC(b_+O(27), b_+OE(28)); H = D;
  CYC(b_+O(28), b_+OE(30)); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(30), b_+OE(32)); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+O(32), b_+OE(33)); A = mem_rd(gb, HL);
  CYC(b_+O(33), b_+OE(34)); mem_wr(gb, DE, A);
  CYC(b_+O(34), b_+OE(36)); alu_and(gb, 0x07);
  CYC(b_+O(36), b_+OE(39)); SET_BC(bitTable);
  CYC(b_+O(39), b_+OE(40)); alu_add(gb, C);
  CYC(b_+O(40), b_+OE(41)); C = A;
  CYC(b_+O(41), b_+OE(42)); A = mem_rd(gb, BC);
  CYC(b_+O(42), b_+OE(44)); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+O(44), b_+OE(45)); mem_wr(gb, HL, A);
  // Convert short-form position in yh to a full y/x position
  CYC(b_+O(45), b_+OE(47)); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(47), b_+OE(49)); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+O(49), b_+OE(50)); A = mem_rd(gb, HL);
  CYC(b_+O(50), b_+OE(51)); mem_wr(gb, DE, A);
  CYC(b_+O(51), b_+OE(53)); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+O(53), setShortPosition_hook, SYM(setShortPosition), b_+OE(56));
  // Pick the script from the subid
  CYC(b_+O(56), b_+OE(58)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(58), b_+OE(59)); A = mem_rd(gb, DE);
  CYC(b_+O(59), b_+OE(62)); SET_HL(doorControllerScriptSubidTable_bank08);
  CYC(b_+O(62), b_+OE(63)); doorController_addDoubleIndex(gb, b_+O(63));
  CYC(b_+O(63), b_+OE(64)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(64), b_+OE(65)); H = mem_rd(gb, HL);
  CYC(b_+O(65), b_+OE(66)); L = A;
  CALL_C(b_+O(66), interactionSetScript_hook, SYM(interactionSetScript), b_+OE(69));
  CYC(b_+O(69), b_+OE(72)); push_effect(gb, b_+OE(72)); doorController_func_47e5(gb);
  doorController_state1(gb, sp0_);
}

// @state2: a door is opening
static void doorController_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(83), b_+OE(86)); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+O(86), b_+OE(87)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(87), b_+OE(88)); ret_effect(gb); return;
  }
  CYC(b_+O(87), b_+OE(88));
  CYC(b_+O(88), b_+OE(90)); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+O(90), b_+OE(91)); A = mem_rd(gb, DE);
  CYC(b_+O(91), b_+OE(92)); push_effect(gb, b_+OE(92));
  do { uint16_t jt_ = (doorController_jumpTable(gb));
    if (jt_ == b_+O(96)) { goto substate0; }
    else if (jt_ == b_+O(149)) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // The tile at this position must be solid
  CALL_C(b_+O(96), objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+OE(99));
  if (!(F & FC)) {
    CYCT(b_+O(99), b_+OE(101)); doorController_gotoState1(gb, sp0_); return;
  }
  CYC(b_+O(99), b_+OE(101));
  doorController_interleaveDoorTile(gb, sp0_);
  return;

substate1:
  CALL_C(b_+O(149), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(152));
  if (!(F & FZ)) {
    CYCT(b_+O(152), b_+OE(153)); ret_effect(gb); return;
  }
  CYC(b_+O(152), b_+OE(153));
  // Door will now open fully
  CYC(b_+O(153), b_+OE(156)); push_effect(gb, b_+OE(156)); doorController_func_47ee(gb);
  CYC(b_+O(156), b_+OE(158)); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(158), b_+OE(159)); A = mem_rd(gb, DE);
  CYC(b_+O(159), b_+OE(162)); SET_HL(doorControllerShutterTiles_bank08);
  CYC(b_+O(162), b_+OE(163)); push_effect(gb, b_+OE(163)); doorController_addAToHl(gb);
  CYC(b_+O(163), b_+OE(165));
  doorController_setTileAndPlaySound(gb, sp0_);
}

// @state3: a door is closing
static void doorController_state3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1e);
  CYC(b_+O(165), b_+OE(167)); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+O(167), b_+OE(168)); A = mem_rd(gb, DE);
  CYC(b_+O(168), b_+OE(169)); push_effect(gb, b_+OE(169));
  do { uint16_t jt_ = (doorController_jumpTable(gb));
    if (jt_ == b_+(game_seasons ? S(167) : 173)) { goto substate0; }
    else if (jt_ == b_+O(187)) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // The tile at this position must not be solid (a somaria block counts as not solid)
  if (!game_seasons) {
    CALL_C(b_+173, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+176);
    CYC(b_+176, b_+178); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
    if (F & FZ) {
      CYCT(b_+178, b_+180); doorController_interleaveDoorTile(gb, sp0_); return;
    }
    CYC(b_+178, b_+180);
  }
  CALL_C(b_+O(180), objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+OE(183));
  if (F & FC) {
    CYCT(b_+O(183), b_+OE(185)); doorController_gotoState1(gb, sp0_); return;
  }
  CYC(b_+O(183), b_+OE(185));
  CYC(b_+O(185), b_+OE(187));
  doorController_interleaveDoorTile(gb, sp0_);
  return;

substate1:
  CALL_C(b_+O(187), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(190));
  if (!(F & FZ)) {
    CYCT(b_+O(190), b_+OE(191)); ret_effect(gb); return;
  }
  CYC(b_+O(190), b_+OE(191));
  // Door will now close fully
  CYC(b_+O(191), b_+OE(194)); push_effect(gb, b_+OE(194)); doorController_checkRespawnLink(gb);
  CYC(b_+O(194), b_+OE(197)); push_effect(gb, b_+OE(197)); doorController_func_47f9(gb);
  CYC(b_+O(197), b_+OE(199)); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(199), b_+OE(200)); A = mem_rd(gb, DE);
  CYC(b_+O(200), b_+OE(203)); SET_HL(doorControllerShutterTiles_bank08);
  CYC(b_+O(203), b_+OE(204)); push_effect(gb, b_+OE(204)); doorController_addAToHl(gb);
  CYC(b_+O(204), b_+OE(205)); SET_HL(HL + 1);
  doorController_setTileAndPlaySound(gb, sp0_);
}

// INTERAC_DOOR_CONTROLLER: opens/closes shutter, key, boss and minecart doors under
// script control.
void interactionCode1e_hook(GB *gb) {
  BASE(interactionCode1e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+OE(3));
  CALL_C(b_+O(3), returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+OE(6));
  if (!game_seasons) {
    CYC(b_+6, b_+9); A = mem_rd(gb, wSwitchHookState);
    CYC(b_+9, b_+11); alu_cp(gb, 0x02);
    if (F & FZ) {
      CYCT(b_+11, b_+12); ret_effect(gb); return;
    }
    CYC(b_+11, b_+12);
  }
  CYC(b_+O(12), b_+OE(14)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(14), b_+OE(15)); A = mem_rd(gb, DE);
  CYC(b_+O(15), b_+OE(16)); push_effect(gb, b_+OE(16));
  do { uint16_t jt_ = (doorController_jumpTable(gb));
    if (jt_ == b_+O(24)) { doorController_state0(gb, sp0_); return; }
    else if (jt_ == b_+O(72)) { doorController_state1(gb, sp0_); return; }
    else if (jt_ == b_+O(83)) { doorController_state2(gb, sp0_); return; }
    else if (jt_ == b_+O(165)) { doorController_state3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
