#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode7f_jump_table(GB *gb) {
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

static void interaction7f_subid00_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void interaction7f_subid00_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void interaction7f_subid01_hook(GB *gb) {
  BASE(interaction7f_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), checkInteractionState_hook, SYM(checkInteractionState), b_+OE(3));
  if (!(F & FZ)) { CYCT(b_+O(3), b_+OE(6)); objectPreventLinkFromPassing_hook(gb); return; } // jp nz
  CYC(b_+O(3), b_+OE(6));
  CYC(b_+O(6), b_+OE(8)); A = 0x01;
  CYC(b_+O(8), b_+OE(9)); mem_wr(gb, DE, A);
  CYC(b_+O(9), b_+OE(12)); SET_BC(0x060a);
  CALL_C(b_+O(12), objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+OE(15));
  CALL_C(b_+O(15), objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+OE(18));
  CYC(b_+O(18), b_+OE(19)); H = alu_dec8(gb, H);
  CYC(b_+O(19), b_+OE(21)); mem_wr(gb, HL, 0x0f);
  if (game_seasons) {
    CYC(b_+S(21), b_+S(24)); A = W8(wDungeonIndex);
    CYC(b_+S(24), b_+S(26)); alu_cp(gb, 0x06);
    if (!(F & FZ)) CYCT(b_+S(26), b_+S(28));
    else {
      CYC(b_+S(26), b_+S(28));
      CYC(b_+S(28), b_+S(31)); SET_HL(wRoomCollisions + 0x24);
      CYC(b_+S(31), b_+S(33)); mem_wr(gb, HL, 0x05);
      CYC(b_+S(33), b_+S(34)); L = alu_inc8(gb, L);
      CYC(b_+S(34), b_+S(36)); mem_wr(gb, HL, 0x0a);
    }
  }
  CALL_C(b_+O(21), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(24));
  CYC(b_+O(24), b_+OE(27)); TAIL(objectSetVisible83); // jp
}

// interaction7f_subid02@copyEssencePosition: reached by one genuine call, from @state1's
// return address 0x479b below; never separately hooked.
void interaction7f_subid02_hook(GB *gb) {
  BASE(interaction7f_subid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction7f_subid02@state0
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+14); TAIL(objectSetVisible82); // jp

state1:
  CYC(b_+14, b_+17); push_effect(gb, b_+17); goto copyEssencePosition;
afterCopy:
  CALL_C(b_+17, interactionAnimate_hook, SYM(interactionAnimate), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+25); return; } // ret z
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+30, b_+32); A = 0x80;
  CYC(b_+32, b_+33); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  RET(b_+34); return; // ret

copyEssencePosition:
  CYC(b_+35, b_+37); A = 0x00; // Object.enabled
  CALL_C(b_+37, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+40);
  CYC(b_+40, b_+43); objectTakePosition_hook(gb);
  if (gb->pc == b_+17 && gb->sp == sp0_) goto afterCopy;
  return;
}

void interaction7f_subid00_hook(GB *gb) {
  BASE(interaction7f_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
    uint16_t target = interactionCode7f_jump_table(gb);
    if (target == b_+O(20)) goto state0;
    if (target == b_+O(140)) goto state1;
    if (target == b_+O(247)) goto state2;
    if (target == b_+O(268)) goto state3;
    if (target == b_+O(287)) goto state4;
    if (target == b_+O(356)) goto state5;
    if (target == b_+O(368)) goto state6;
    if (target == b_+O(379)) goto state7;
    hook_continue(gb, target, sp0_); return;
  }

state0:
  CYC(b_+O(20), b_+OE(22)); A = 0x01;
  CYC(b_+O(22), b_+OE(23)); mem_wr(gb, DE, A);
  CALL_C(b_+O(23), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(26));
  CYC(b_+O(26), b_+OE(28)); A = 0x04;
  CALL_C(b_+O(28), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(31));
  CYC(b_+O(31), b_+OE(34)); SET_BC(0x7f01); // INTERAC_ESSENCE, $01
  CALL_C(b_+O(34), objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+OE(37));
  CALL_C(b_+O(37), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(40));
  CYC(b_+O(40), b_+OE(42)); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+O(42), b_+OE(45)); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+O(42), b_+OE(45));
  CYC(b_+O(45), b_+OE(48)); SET_HL(w1ReservedInteraction1);
  CYC(b_+O(48), b_+OE(50)); B = 0x40;
  CALL_C(b_+O(50), clearMemory_hook, SYM(clearMemory), b_+OE(53));
  CYC(b_+O(53), b_+OE(56)); SET_HL(w1ReservedInteraction1);
  CYC(b_+O(56), b_+OE(58)); mem_wr(gb, HL, 0x81);
  CYC(b_+O(58), b_+OE(59)); L = alu_inc8(gb, L);
  CYC(b_+O(59), b_+OE(61)); mem_wr(gb, HL, 0x7f); // INTERAC_ESSENCE
  CYC(b_+O(61), b_+OE(62)); L = alu_inc8(gb, L);
  CYC(b_+O(62), b_+OE(64)); mem_wr(gb, HL, 0x02);
  CALL_C(b_+O(64), objectCopyPosition_hook, SYM(objectCopyPosition), b_+OE(67));
  CYC(b_+O(67), b_+OE(69)); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+O(69), b_+OE(71)); A = H8(hActiveObjectType);
  CYC(b_+O(71), b_+OE(72)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(72), b_+OE(74)); A = H8(hActiveObject);
  CYC(b_+O(74), b_+OE(75)); mem_wr(gb, HL, A);
  CYC(b_+O(75), b_+OE(76)); H = D;
  CYC(b_+O(76), b_+OE(78)); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+O(78), b_+OE(80)); mem_wr(gb, HL, (uint8_t)-0x10);
  CYC(b_+O(80), b_+OE(83)); A = W8(wDungeonIndex);
  CYC(b_+O(83), b_+OE(84)); A = alu_dec8(gb, A);
  if (!game_seasons) {
    CYC(b_+84, b_+86); alu_cp(gb, 0x0b);
    if (!(F & FZ)) { CYCT(b_+86, b_+88); goto afterDungeonOverride; } // jr nz
    CYC(b_+86, b_+88);
    CYC(b_+88, b_+90); A = 0x05;
  }

afterDungeonOverride:
  CYC(b_+O(90), b_+OE(92)); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(92), b_+OE(93)); mem_wr(gb, HL, A);
  CYC(b_+O(93), b_+OE(94)); B = A;
  CYC(b_+O(94), b_+OE(95)); alu_add(gb, A);
  CYC(b_+O(95), b_+OE(96)); alu_add(gb, B);
  CYC(b_+O(96), b_+OE(99)); SET_HL(b_+O(116)); // interaction7f_subid00@essenceOamData
  CYC(b_+O(99), b_+OE(100)); interaction7f_subid00_addAToHl_from_rst(gb, b_+OE(100));
  CYC(b_+O(100), b_+OE(102)); E = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+O(102), b_+OE(103)); A = mem_rd(gb, DE);
  CYC(b_+O(103), b_+OE(104)); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+O(104), b_+OE(105)); SET_HL(HL + 1); // inc hl
  CYC(b_+O(105), b_+OE(106)); mem_wr(gb, DE, A);
  CYC(b_+O(106), b_+OE(107)); E = alu_dec8(gb, E);
  CYC(b_+O(107), b_+OE(108)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(108), b_+OE(109)); mem_wr(gb, DE, A);
  CYC(b_+O(109), b_+OE(110)); A = mem_rd(gb, HL);
  CALL_C(b_+O(110), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(113));
  CYC(b_+O(113), b_+OE(116)); TAIL(objectSetVisible81); // jp

state1:
  CYC(b_+O(140), b_+OE(143)); A = W8(wFrameCounter);
  CYC(b_+O(143), b_+OE(145)); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+O(145)); return; } // ret nz
  CYC(b_+O(145), b_+OE(146));
  CYC(b_+O(146), b_+OE(147)); H = D;
  CYC(b_+O(147), b_+OE(149)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(149), b_+OE(150)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(150), b_+OE(151)); A = mem_rd(gb, HL);
  CYC(b_+O(151), b_+OE(153)); alu_and(gb, 0x0f);
  CYC(b_+O(153), b_+OE(156)); SET_HL(b_+O(231)); // interaction7f_subid00@essenceFloatOffsets
  CYC(b_+O(156), b_+OE(157)); interaction7f_subid00_addAToHl_from_rst(gb, b_+OE(157));
  CYC(b_+O(157), b_+OE(158)); A = mem_rd(gb, HL);
  CYC(b_+O(158), b_+OE(160)); alu_add(gb, 0xf0);
  CYC(b_+O(160), b_+OE(162)); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+O(162), b_+OE(163)); mem_wr(gb, DE, A);
  CYC(b_+O(163), b_+OE(166)); A = W8(wLinkInAir);
  CYC(b_+O(166), b_+OE(167)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+O(167)); return; } // ret nz
  CYC(b_+O(167), b_+OE(168));
  CYC(b_+O(168), b_+OE(171)); A = W8(wLinkGrabState);
  CYC(b_+O(171), b_+OE(172)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+O(172)); return; } // ret nz
  CYC(b_+O(172), b_+OE(173));
  CYC(b_+O(173), b_+OE(175)); B = 0x04;
  CALL_C(b_+O(175), objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+OE(178));
  if (!(F & FC)) { RET_TAKEN(b_+O(178)); return; } // ret nc
  CYC(b_+O(178), b_+OE(179));
  CYC(b_+O(179), b_+OE(181)); C = 0x14;
  CALL_C(b_+O(181), objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+OE(184));
  if (!(F & FC)) { RET_TAKEN(b_+O(184)); return; } // ret nc
  CYC(b_+O(184), b_+OE(185));
  CYC(b_+O(185), b_+OE(187)); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+O(187)); return; } // ret nz
  CYC(b_+O(187), b_+OE(188));
  CALL_C(b_+O(188), clearAllParentItems_hook, SYM(clearAllParentItems), b_+OE(191));
  CYC(b_+O(191), b_+OE(193)); A = 0x81;
  CYC(b_+O(193), b_+OE(196)); W8(wDisabledObjects) = A;
  CYC(b_+O(196), b_+OE(199)); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+O(199), b_+OE(202)); SET_HL(w1Link_direction);
  CYC(b_+O(202), b_+OE(204)); mem_wr(gb, HL, 0x00); // DIR_UP
  CALL_C(b_+O(204), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(207));
  CYC(b_+O(207), b_+OE(208)); H = D;
  CYC(b_+O(208), b_+OE(210)); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(210), b_+OE(211)); mem_wr(gb, HL, A);
  CYC(b_+O(211), b_+OE(213)); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+O(213), b_+OE(215)); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+O(215), b_+OE(217)); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(217), b_+OE(218)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+O(218), darkenRoom_hook, SYM(darkenRoom), b_+OE(221));
  CYC(b_+O(221), b_+OE(223)); A = 0x77; // SND_DROPESSENCE
  CALL_C(b_+O(223), playSound_b00_hook, SYM(playSound_b00), b_+OE(226));
  CYC(b_+O(226), b_+OE(228)); A = 0xfc; // SNDCTRL_SLOW_FADEOUT
  CYC(b_+O(228), b_+OE(231)); TAIL(playSound_b00); // jp

state2:
  CALL_C(b_+O(247), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(250));
  CYC(b_+O(250), b_+OE(252)); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(252), b_+OE(253)); mem_wr(gb, DE, A);
  CALL_C(b_+O(253), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(256));
  CALL_C(b_+O(256), objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+OE(259));
  if (!(F & FC)) { RET_TAKEN(b_+O(259)); return; } // ret nc
  CYC(b_+O(259), b_+OE(260));
  CYC(b_+O(260), b_+OE(262)); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+O(262), b_+OE(264)); A = 0x06;
  CYC(b_+O(264), b_+OE(265)); mem_wr(gb, DE, A);
  CYC(b_+O(265), b_+OE(268)); TAIL(interactionIncState); // jp

state3:
  CYC(b_+O(268), b_+OE(270)); C = 0x08;
  CALL_C(b_+O(270), objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+OE(273));
  if (F & FZ) { CYCT(b_+O(273), b_+OE(275)); goto afterSpeedZCheck; } // jr z
  CYC(b_+O(273), b_+OE(275));
  CALL_C(b_+O(275), objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+OE(278));
  if (!(F & FC)) { RET_TAKEN(b_+O(278)); return; } // ret nc
  CYC(b_+O(278), b_+OE(279));

afterSpeedZCheck:
  CYC(b_+O(279), b_+OE(280)); H = D;
  CYC(b_+O(280), b_+OE(282)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(282), b_+OE(284)); mem_wr(gb, HL, 30);
  CYC(b_+O(284), b_+OE(287)); TAIL(interactionIncState); // jp

state4:
  CALL_C(b_+O(287), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(290));
  if (!(F & FZ)) { RET_TAKEN(b_+O(290)); return; } // ret nz
  CYC(b_+O(290), b_+OE(291));
  CYC(b_+O(291), b_+OE(293)); A = 0x04; // LINK_STATE_04
  CYC(b_+O(293), b_+OE(296)); W8(wLinkForceState) = A;
  CYC(b_+O(296), b_+OE(298)); A = 0x01;
  CYC(b_+O(298), b_+OE(301)); W8(wcc50) = A;
  CALL_C(b_+O(301), interactionIncState_hook, SYM(interactionIncState), b_+OE(304));
  CYC(b_+O(304), b_+OE(307)); A = W8(w1Link_yh);
  CYC(b_+O(307), b_+OE(309)); alu_sub(gb, 0x0e);
  CYC(b_+O(309), b_+OE(311)); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(311), b_+OE(312)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(312), b_+OE(313)); L = alu_inc8(gb, L);
  CYC(b_+O(313), b_+OE(316)); A = W8(w1Link_xh);
  CYC(b_+O(316), b_+OE(317)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(317), b_+OE(318)); L = alu_inc8(gb, L);
  CYC(b_+O(318), b_+OE(319)); alu_xor(gb, A);
  CYC(b_+O(319), b_+OE(320)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(320), b_+OE(321)); mem_wr(gb, HL, A);
  CYC(b_+O(321), b_+OE(323)); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(323), b_+OE(324)); A = mem_rd(gb, HL);
  CYC(b_+O(324), b_+OE(327)); SET_HL(b_+O(348)); // interaction7f_subid00@getEssenceTextTable
  CYC(b_+O(327), b_+OE(328)); interaction7f_subid00_addAToHl_from_rst(gb, b_+OE(328));
  CYC(b_+O(328), b_+OE(330)); B = 0x00; // >TX_0000
  CYC(b_+O(330), b_+OE(331)); C = mem_rd(gb, HL);
  CALL_C(b_+O(331), showText_hook, SYM(showText), b_+OE(334));
  CALL_C(b_+O(334), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(337));
  CYC(b_+O(337), b_+OE(339)); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 5)); // ROOMFLAG_BIT_ITEM
  CYC(b_+O(339), b_+OE(341)); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(341), b_+OE(342)); A = mem_rd(gb, DE);
  CYC(b_+O(342), b_+OE(343)); C = A;
  CYC(b_+O(343), b_+OE(345)); A = 0x40; // TREASURE_ESSENCE
  CYC(b_+O(345), b_+OE(348)); TAIL(giveTreasure); // jp

state5:
  CALL_C(b_+O(356), retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+OE(359));
  CALL_C(b_+O(359), interactionIncState_hook, SYM(interactionIncState), b_+OE(362));
  CYC(b_+O(362), b_+OE(365)); SET_HL(GV(0x49c8, 0x49cc)); // mainScripts.essenceScript_essenceGetCutscene
  CYC(b_+O(365), b_+OE(368)); TAIL(interactionSetScript); // jp

state6:
  CALL_C(b_+O(368), interactionRunScript_hook, SYM(interactionRunScript), b_+OE(371));
  if (!(F & FC)) { RET_TAKEN(b_+O(371)); return; } // ret nc
  CYC(b_+O(371), b_+OE(372));
  CALL_C(b_+O(372), interactionIncState_hook, SYM(interactionIncState), b_+OE(375));
  CYC(b_+O(375), b_+OE(377)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(377), b_+OE(379)); mem_wr(gb, HL, 30);

state7:
  CALL_C(b_+O(379), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(382));
  if (!(F & FZ)) { RET_TAKEN(b_+O(382)); return; } // ret nz
  CYC(b_+O(382), b_+OE(383));
  CYC(b_+O(383), b_+OE(385)); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(385), b_+OE(386)); A = mem_rd(gb, HL);
  CYC(b_+O(386), b_+OE(387)); alu_add(gb, A);
  CYC(b_+O(387), b_+OE(390)); SET_HL(b_+O(419)); // interaction7f_subid00@essenceWarps
  CYC(b_+O(390), b_+OE(391)); interaction7f_subid00_addDoubleIndexToHl_from_rst(gb, b_+OE(391));
  CYC(b_+O(391), b_+OE(392)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(392), b_+OE(395)); W8(wWarpDestGroup) = A;
  CYC(b_+O(395), b_+OE(396)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(396), b_+OE(399)); W8(wWarpDestRoom) = A;
  CYC(b_+O(399), b_+OE(400)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(400), b_+OE(403)); W8(wWarpDestPos) = A;
  CYC(b_+O(403), b_+OE(404)); A = mem_rd(gb, HL);
  CYC(b_+O(404), b_+OE(407)); W8(wWarpTransition) = A;
  CYC(b_+O(407), b_+OE(409)); A = 0x83;
  CYC(b_+O(409), b_+OE(412)); W8(wWarpTransition2) = A;
  CYC(b_+O(412), b_+OE(413)); alu_xor(gb, A);
  CYC(b_+O(413), b_+OE(416)); W8(wActiveMusic) = A;
  CYC(b_+O(416), b_+OE(419)); TAIL(clearStaticObjects); // jp
}

// INTERAC_ESSENCE
void interactionCode7f_hook(GB *gb) {
  BASE(interactionCode7f);
  CYC(b_+0, b_+3); A = W8(wLinkDeathTrigger);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = interactionCode7f_jump_table(gb);
    if (target == SYM(interaction7f_subid00)) { interaction7f_subid00_hook(gb); return; }
    if (target == SYM(interaction7f_subid01)) { interaction7f_subid01_hook(gb); return; }
    TAIL(interaction7f_subid02);
  }
}
