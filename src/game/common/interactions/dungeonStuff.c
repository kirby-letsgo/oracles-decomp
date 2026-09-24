#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// interactionCode12@dungeonTextIndices: low byte of TX_02xx, one per dungeon.
#define dungeonTextIndices_bank08 SYM(interactionCode12__dungeonTextIndices)
// interactionCode12@initialSpinnerValues: initial wSpinnerState, one per dungeon.
#define initialSpinnerValues_bank08 SYM(interactionCode12__initialSpinnerValues)
// interactionCode12@replacementTiles: staircase tiles replacing tiles $40-$43.
#define replacementTiles_bank08 SYM(interactionCode12__replacementTiles)

// mainScripts.* (bank $0c script data, referenced by address only).
#define dropSmallKeyWhenNoEnemiesScript_bank0c SYM(dropSmallKeyWhenNoEnemiesScript)
#define createChestWhenNoEnemiesScript_bank0c SYM(createChestWhenNoEnemiesScript)
#define setRoomFlagBit7WhenNoEnemiesScript_bank0c SYM(setRoomFlagBit7WhenNoEnemiesScript)

#define LARGE_ROOM_HEIGHT 11

static uint16_t dungeonStuff_jumpTable(GB *gb) {
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

static void dungeonStuff_addAToHl(GB *gb) {
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

// interactionCode12@createPuff: spawn INTERAC_PUFF at the short position in C. Reached only
// by a static `call` from createStaircaseTile; not independently hookable.
static void dungeonStuff_createPuff(GB *gb, uint16_t sp0_) {
  BASE(interactionCode12);
  CALL_C(b_+O(242), getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+OE(245));
  if (!(F & FZ)) {
    CYCT(b_+O(245), b_+OE(246)); ret_effect(gb); return;
  }
  CYC(b_+O(245), b_+OE(246));
  CYC(b_+O(246), b_+OE(248)); mem_wr(gb, HL, 0x05);
  CYC(b_+O(248), b_+OE(250)); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(250), b_+OE(253)); setShortPosition_paramC_hook(gb);
}

// interactionCode12@createStaircaseTile: replace tile $40+A at position C with a staircase
// and puff. Reached only by a static `call c` from interactionCode12 subid 4; not
// independently hookable.
static void dungeonStuff_createStaircaseTile(GB *gb, uint16_t sp0_) {
  BASE(interactionCode12);
  CYC(b_+O(222), b_+OE(223)); push_effect(gb, BC);
  CYC(b_+O(223), b_+OE(224)); push_effect(gb, HL);
  CYC(b_+O(224), b_+OE(227)); SET_HL(replacementTiles_bank08);
  CYC(b_+O(227), b_+OE(228)); push_effect(gb, b_+OE(228)); dungeonStuff_addAToHl(gb);
  CYC(b_+O(228), b_+OE(229)); A = mem_rd(gb, HL);
  CALL_C(b_+O(229), setTile_hook, SYM(setTile), b_+OE(232));
  CYC(b_+O(232), b_+OE(235)); push_effect(gb, b_+OE(235)); dungeonStuff_createPuff(gb, sp0_);
  CYC(b_+O(235), b_+OE(236)); SET_HL(pop_effect(gb));
  CYC(b_+O(236), b_+OE(237)); SET_BC(pop_effect(gb));
  CYC(b_+O(237), b_+OE(238)); ret_effect(gb);
}

// @subid05 (Seasons): spawn the magnet ball at this position once w1MagnetBall is free.
static void dungeonStuff_subid05(GB *gb, uint16_t sp0_) {
  BASE(interactionCode12);
  CYC(b_+S(252), b_+S(254)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+S(254), b_+S(255)); A = mem_rd(gb, DE);
  CYC(b_+S(255), b_+S(256)); push_effect(gb, b_+S(256));
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+S(262)) { break; }
    else if (jt_ == b_+S(271)) { goto state1; }
    else if (jt_ == b_+S(300)) { CYC(b_+S(300), b_+S(303)); TAIL(interactionDelete); }
    else { HANDOFF(HL); }
  } while (0);
  CYC(b_+S(262), b_+S(264)); A = 0x01;
  CYC(b_+S(264), b_+S(265)); mem_wr(gb, DE, A);
  CALL_C(b_+S(265), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+S(268));
  CALL_C(b_+S(268), objectSetVisible82_hook, SYM(objectSetVisible82), b_+S(271));
state1:
  CYC(b_+S(271), b_+S(274)); SET_HL(w1MagnetBall);
  CYC(b_+S(274), b_+S(275)); A = mem_rd(gb, HL);
  CYC(b_+S(275), b_+S(276)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+S(276)); return; }
  CYC(b_+S(276), b_+S(277));
  CYC(b_+S(277), b_+S(279)); mem_wr(gb, HL, 0x01);
  CYC(b_+S(279), b_+S(280)); L = alu_inc8(gb, L);
  CYC(b_+S(280), b_+S(282)); mem_wr(gb, HL, 0x29);
  CALL_C(b_+S(282), objectCopyPosition_hook, SYM(objectCopyPosition), b_+S(285));
  CYC(b_+S(285), b_+S(287)); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+S(287), b_+S(289)); L = OBJ_RELATED1;
  CYC(b_+S(289), b_+S(290)); A = mem_rd(gb, DE);
  CYC(b_+S(290), b_+S(291)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+S(291), b_+S(292)); E = alu_inc8(gb, E);
  CYC(b_+S(292), b_+S(293)); A = mem_rd(gb, DE);
  CYC(b_+S(293), b_+S(294)); mem_wr(gb, HL, A);
  CYC(b_+S(294), b_+S(296)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+S(296), b_+S(298)); A = 0x02;
  CYC(b_+S(298), b_+S(299)); mem_wr(gb, DE, A);
  CYC(b_+S(299), b_+S(300)); ret_effect(gb);
}

void interactionCode12_hook(GB *gb) {
  BASE(interactionCode12);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+O(14)) { goto subid00; }
    else if (jt_ == b_+O(112)) { goto subid01; }
    else if (jt_ == b_+O(139)) { goto subid02; }
    else if (jt_ == b_+O(165)) { goto subid03; }
    else if (jt_ == b_+O(181)) { goto subid04; }
    else if (game_seasons && jt_ == b_+S(252)) { dungeonStuff_subid05(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

subid00:
  CALL_C(b_+O(14), checkInteractionState_hook, SYM(checkInteractionState), b_+OE(17));
  if (!(F & FZ)) {
    CYCT(b_+O(17), b_+OE(19)); goto initialized;
  }
  CYC(b_+O(17), b_+OE(19));
  CYC(b_+O(19), b_+OE(22)); A = mem_rd(gb, wScrollMode);
  CYC(b_+O(22), b_+OE(24)); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+O(24), b_+OE(27)); TAIL(interactionDelete);
  }
  CYC(b_+O(24), b_+OE(27));
  CYC(b_+O(27), b_+OE(30)); A = W8(w1Link_yh);
  CYC(b_+O(30), b_+OE(32)); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(b_+O(32), b_+OE(35)); TAIL(interactionDelete);
  }
  CYC(b_+O(32), b_+OE(35));
  CALL_C(b_+O(35), interactionIncState_hook, SYM(interactionIncState), b_+OE(38));
  CYC(b_+O(38), b_+OE(40)); A = 0x08;
  CALL_C(b_+O(40), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(43));
  CALL_C(b_+O(43), initializeDungeonStuff_hook, SYM(initializeDungeonStuff), b_+OE(46));
  CYC(b_+O(46), b_+OE(49)); A = mem_rd(gb, wDungeonIndex);
  if (!game_seasons) {
    CYC(b_+49, b_+52); SET_HL(initialSpinnerValues_bank08);
    CYC(b_+52, b_+53); push_effect(gb, b_+53); dungeonStuff_addAToHl(gb);
    CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  } else {
    CYC(b_+S(51), b_+S(53)); alu_cp(gb, 0x07);
    if (!(F & FZ)) CYCT(b_+S(53), b_+S(55));
    else {
      CYC(b_+S(53), b_+S(55));
      CYC(b_+S(55), b_+S(57)); A = 0x05;
      CYC(b_+S(57), b_+S(60)); W8(wToggleBlocksState) = A;
    }
    CYC(b_+S(60), b_+S(63)); A = mem_rd(gb, wDungeonIndex);
    CYC(b_+S(63), b_+S(65)); alu_cp(gb, 0x08);
    if (!(F & FZ)) { CYCT(b_+S(65), b_+S(67)); goto initialized; }
    CYC(b_+S(65), b_+S(67));
    CYC(b_+S(67), b_+S(69)); A = 0x01;
  }
  CYC(b_+O(54), b_+OE(57)); mem_wr(gb, wSpinnerState, A);

initialized:
  CALL_C(b_+O(57), objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+OE(60));
  if (!(F & FC)) {
    CYCT(b_+O(60), b_+OE(61)); ret_effect(gb); return;
  }
  CYC(b_+O(60), b_+OE(61));
  CYC(b_+O(61), b_+OE(64)); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+O(64), b_+OE(67)); SET_HL(dungeonTextIndices_bank08);
  CYC(b_+O(67), b_+OE(68)); push_effect(gb, b_+OE(68)); dungeonStuff_addAToHl(gb);
  CYC(b_+O(68), b_+OE(69)); C = mem_rd(gb, HL);
  CYC(b_+O(69), b_+OE(71)); B = 0x02;
  CALL_C(b_+O(71), showText_hook, SYM(showText), b_+OE(74));
  CALL_C(b_+O(74), setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+OE(77));
  CYC(b_+O(77), b_+OE(80)); interactionDelete_hook(gb);
  return;

subid01:
  CALL_C(b_+O(112), returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+OE(115));
  CYC(b_+O(115), b_+OE(117)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(117), b_+OE(118)); A = mem_rd(gb, DE);
  CYC(b_+O(118), b_+OE(119)); push_effect(gb, b_+OE(119));
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+O(123)) { goto subid01_substate0; }
    else if (jt_ == b_+O(132)) { goto runScript; }
    else { HANDOFF(HL); }
  } while (0);

subid01_substate0:
  CYC(b_+O(123), b_+OE(125)); A = 0x01;
  CYC(b_+O(125), b_+OE(126)); mem_wr(gb, DE, A);
  CYC(b_+O(126), b_+OE(129)); SET_HL(dropSmallKeyWhenNoEnemiesScript_bank0c);
  CALL_C(b_+O(129), interactionSetScript_hook, SYM(interactionSetScript), b_+OE(132));

runScript:
  CALL_C(b_+O(132), interactionRunScript_hook, SYM(interactionRunScript), b_+OE(135));
  if (F & FC) {
    CYCT(b_+O(135), b_+OE(138)); TAIL(interactionDelete);
  }
  CYC(b_+O(135), b_+OE(138));
  CYC(b_+O(138), b_+OE(139)); ret_effect(gb);
  return;

subid02:
  CYC(b_+O(139), b_+OE(141)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(141), b_+OE(142)); A = mem_rd(gb, DE);
  CYC(b_+O(142), b_+OE(143)); push_effect(gb, b_+OE(143));
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+O(149)) { goto subid02_substate0; }
    else if (jt_ == b_+O(132)) { goto runScript; }
    else if (jt_ == b_+O(160)) { goto subid02_substate2; }
    else { HANDOFF(HL); }
  } while (0);

subid02_substate0:
  CYC(b_+O(149), b_+OE(151)); A = 0x01;
  CYC(b_+O(151), b_+OE(152)); mem_wr(gb, DE, A);
  CYC(b_+O(152), b_+OE(155)); SET_HL(createChestWhenNoEnemiesScript_bank0c);
  CALL_C(b_+O(155), interactionSetScript_hook, SYM(interactionSetScript), b_+OE(158));
  CYC(b_+O(158), b_+OE(160)); goto runScript;

subid02_substate2:
  CALL_C(b_+O(160), objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+OE(163));
  CYC(b_+O(163), b_+OE(165)); goto runScript;

subid03:
  CALL_C(b_+O(165), checkInteractionState_hook, SYM(checkInteractionState), b_+OE(168));
  if (!(F & FZ)) {
    CYCT(b_+O(168), b_+OE(170)); goto runScript;
  }
  CYC(b_+O(168), b_+OE(170));
  CYC(b_+O(170), b_+OE(172)); A = 0x01;
  CYC(b_+O(172), b_+OE(173)); mem_wr(gb, DE, A);
  CYC(b_+O(173), b_+OE(176)); SET_HL(setRoomFlagBit7WhenNoEnemiesScript_bank0c);
  CALL_C(b_+O(176), interactionSetScript_hook, SYM(interactionSetScript), b_+OE(179));
  CYC(b_+O(179), b_+OE(181)); goto runScript;

subid04:
  CALL_C(b_+O(181), returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+OE(184));
  CALL_C(b_+O(184), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(187));
  CYC(b_+O(187), b_+OE(189)); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+O(189), b_+OE(192)); TAIL(interactionDelete);
  }
  CYC(b_+O(189), b_+OE(192));
  CYC(b_+O(192), b_+OE(195)); A = mem_rd(gb, wNumEnemies);
  CYC(b_+O(195), b_+OE(196)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(196), b_+OE(197)); ret_effect(gb); return;
  }
  CYC(b_+O(196), b_+OE(197));
  CYC(b_+O(197), b_+OE(199)); A = 0x4d;
  CALL_C(b_+O(199), playSound_b00_hook, SYM(playSound_b00), b_+OE(202));
  CALL_C(b_+O(202), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(205));
  CYC(b_+O(205), b_+OE(207)); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  CYC(b_+O(207), b_+OE(210)); SET_BC(wRoomLayout + LARGE_ROOM_HEIGHT * 16 - 1);
  for (;;) {
    CYC(b_+O(210), b_+OE(211)); A = mem_rd(gb, BC);
    CYC(b_+O(211), b_+OE(213)); alu_sub(gb, 0x40);
    CYC(b_+O(213), b_+OE(215)); alu_cp(gb, 0x04);
    if (F & FC) {
      CYCT(b_+O(215), b_+OE(218)); push_effect(gb, b_+OE(218)); dungeonStuff_createStaircaseTile(gb, sp0_);
    } else {
      CYC(b_+O(215), b_+OE(218));
    }
    CYC(b_+O(218), b_+OE(219)); C = alu_dec8(gb, C);
    if (F & FZ) {
      CYC(b_+O(219), b_+OE(221));
      break;
    }
    CYCT(b_+O(219), b_+OE(221));
  }
  CYC(b_+O(221), b_+OE(222)); ret_effect(gb);
}
