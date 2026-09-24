#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodeb6_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void interactionCodeb6_addAToHl(GB *gb, uint16_t return_address) {
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

static void interactionCodeb6_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_GASHA_SPOT
// ==================================================================================================
void interactionCodeb6_hook(GB *gb) {
  BASE(interactionCodeb6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (interactionCodeb6_jump_table(gb));
    if (jt_ == b_+O(20)) { goto state0; }
    else if (jt_ == b_+O(134)) { goto state1; }
    else if (jt_ == b_+O(158)) { goto state2; }
    else if (jt_ == b_+O(207)) { goto state3; }
    else if (jt_ == b_+O(250)) { goto state4; }
    else if (jt_ == b_+O(283)) { goto state5; }
    else if (jt_ == b_+O(467)) { goto state6; }
    else if (jt_ == b_+O(622)) { goto state7; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+O(20), b_+OE(22)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(22), b_+OE(23)); A = mem_rd(gb, DE);
  CYC(b_+O(23), b_+OE(24)); E = alu_inc8(gb, E);
  CYC(b_+O(24), b_+OE(25)); mem_wr(gb, DE, A);
  CYC(b_+O(25), b_+OE(28)); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(b_+O(28), checkFlag_hook, SYM(checkFlag), b_+OE(31));
  if (!(F & FZ)) { CYCT(b_+O(31), b_+OE(33)); goto seedPlanted; } // jr nz
  CYC(b_+O(31), b_+OE(33));
  CYC(b_+O(33), b_+OE(35)); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+O(35), b_+OE(36)); A = mem_rd(gb, DE);
  CYC(b_+O(36), b_+OE(37)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(37), b_+OE(39)); goto l_442b; } // jr nz
  CYC(b_+O(37), b_+OE(39));
  CYC(b_+O(39), b_+OE(41)); A = 0x28; // DISCOVERY_RING
  CALL_C(b_+O(41), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(44));
  if (!(F & FZ)) { CYCT(b_+O(44), b_+OE(46)); goto l_442b; } // jr nz
  CYC(b_+O(44), b_+OE(46));
  CYC(b_+O(46), b_+OE(48)); A = 0xa2; // SND_COMPASS
  CYC(b_+O(48), b_+OE(50)); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+O(50), b_+OE(51)); mem_wr(gb, DE, A);
  CALL_C(b_+O(51), playSound_b00_hook, SYM(playSound_b00), b_+OE(54));

l_442b:
  CALL_C(b_+O(54), objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+OE(57));
  CYC(b_+O(57), b_+OE(59)); alu_cp(gb, GV(0xd2, 0xe0)); // TILEINDEX_SOFT_SOIL
  if (!(F & FZ)) { CYCT(b_+O(59), b_+OE(60)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(59), b_+OE(60));

  // @unearthed
  CALL_C(b_+O(60), interactionIncState_hook, SYM(interactionIncState), b_+OE(63));
  CYC(b_+O(63), b_+OE(65)); A = 0x0a;
  CALL_C(b_+O(65), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(68));
  CYC(b_+O(68), b_+OE(70)); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+O(70), b_+OE(73)); TAIL(objectAddToAButtonSensitiveObjectList); // jp

seedPlanted:
  CYC(b_+O(73), b_+OE(74)); A = mem_rd(gb, DE);
  CYC(b_+O(74), b_+OE(77)); SET_HL(wGashaSpotKillCounters);
  CYC(b_+O(77), b_+OE(78)); interactionCodeb6_addAToHl(gb, b_+O(78));
  CYC(b_+O(78), b_+OE(79)); A = mem_rd(gb, HL);
  CYC(b_+O(79), b_+OE(81)); alu_cp(gb, 40);
  if (F & FC) { CYCT(b_+O(81), b_+OE(83)); goto delete_; } // jr c
  CYC(b_+O(81), b_+OE(83));

  // @killedEnoughEnemies
  CALL_C(b_+O(83), getFreePartSlot_hook, SYM(getFreePartSlot), b_+OE(86));
  if (!(F & FZ)) { CYCT(b_+O(86), b_+OE(87)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(86), b_+OE(87));
  CYC(b_+O(87), b_+OE(89)); mem_wr(gb, HL, 0x17); // PART_GASHA_TREE
  CYC(b_+O(89), b_+OE(90)); L = alu_inc8(gb, L);
  CYC(b_+O(90), b_+OE(92)); mem_wr(gb, HL, 0x01);
  CYC(b_+O(92), b_+OE(94)); L = 0xd6; // Part.relatedObj1
  CYC(b_+O(94), b_+OE(96)); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+O(96), b_+OE(97)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(97), b_+OE(98)); mem_wr(gb, HL, D);
  CYC(b_+O(98), b_+OE(99)); H = D;
  CYC(b_+O(99), b_+OE(101)); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+O(101), b_+OE(103)); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(103), b_+OE(104)); A = mem_rd(gb, DE);
  CYC(b_+O(104), b_+OE(105)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(105), b_+OE(107)); alu_add(gb, 0xf8);
  CYC(b_+O(107), b_+OE(108)); mem_wr(gb, DE, A);
  CYC(b_+O(108), b_+OE(110)); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(110), b_+OE(111)); A = mem_rd(gb, DE);
  CYC(b_+O(111), b_+OE(112)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(112), b_+OE(114)); alu_add(gb, 0x08);
  CYC(b_+O(114), b_+OE(115)); mem_wr(gb, DE, A);
  CYC(b_+O(115), b_+OE(117)); A = 0x04;
  CALL_C(b_+O(117), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(120));
  CYC(b_+O(120), b_+OE(122)); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(122), b_+OE(124)); mem_wr(gb, HL, 0x03);
  CYC(b_+O(124), b_+OE(126)); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(126), b_+OE(128)); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+O(128), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(131));
  CYC(b_+O(131), b_+OE(134)); TAIL(objectSetVisible83); // jp

state1:
  CYC(b_+O(134), b_+OE(136)); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+O(136), b_+OE(137)); A = mem_rd(gb, DE);
  CYC(b_+O(137), b_+OE(138)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(138), b_+OE(139)); ret_effect(gb); return; } // ret z
  CYC(b_+O(138), b_+OE(139));

  // @pressedAButton
  CYC(b_+O(139), b_+OE(140)); alu_xor(gb, A);
  CYC(b_+O(140), b_+OE(141)); mem_wr(gb, DE, A);
  CYC(b_+O(141), b_+OE(144)); SET_BC(0x3509); // TX_3509
  CYC(b_+O(144), b_+OE(147)); A = mem_rd(gb, wNumGashaSeeds);
  CYC(b_+O(147), b_+OE(148)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(148), b_+OE(150)); goto l_4490; } // jr z
  CYC(b_+O(148), b_+OE(150));
  CALL_C(b_+O(150), interactionIncState_hook, SYM(interactionIncState), b_+OE(153));
  CYC(b_+O(153), b_+OE(155)); C = 0x00; // <TX_3500

l_4490:
  CYC(b_+O(155), b_+OE(158)); TAIL(showText); // jp

state2:
  CYC(b_+O(158), b_+OE(161)); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+O(161), b_+OE(162)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(162), b_+OE(164)); goto l_449d; } // jr z
  CYC(b_+O(162), b_+OE(164));
  CYC(b_+O(164), b_+OE(166)); A = 0x01;
  CYC(b_+O(166), b_+OE(167)); mem_wr(gb, DE, A); // [state]
  RET(b_+O(167)); return;

l_449d:
  CALL_C(b_+O(168), objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+OE(171));
  CYC(b_+O(171), b_+OE(172)); C = L;
  CYC(b_+O(172), b_+OE(174)); A = 0xf5; // TILEINDEX_SOFT_SOIL_PLANTED
  CALL_C(b_+O(174), setTile_hook, SYM(setTile), b_+OE(177));
  CYC(b_+O(177), b_+OE(179)); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(179), b_+OE(180)); A = mem_rd(gb, DE);
  CYC(b_+O(180), b_+OE(183)); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(b_+O(183), setFlag_hook, SYM(setFlag), b_+OE(186));
  CYC(b_+O(186), b_+OE(187)); A = mem_rd(gb, DE);
  CYC(b_+O(187), b_+OE(189)); L = GV(0x4f, 0x4c); // wGashaSpotKillCounters low byte
  CYC(b_+O(189), b_+OE(190)); interactionCodeb6_addAToHl(gb, b_+O(190));
  CYC(b_+O(190), b_+OE(192)); mem_wr(gb, HL, 0x00);
  CYC(b_+O(192), b_+OE(194)); L = GV(0xbe, 0xba); // wNumGashaSeeds low byte
  CYC(b_+O(194), b_+OE(195)); A = mem_rd(gb, HL);
  CYC(b_+O(195), b_+OE(197)); alu_sub(gb, 0x01);
  CYC(b_+O(197), b_+OE(198)); alu_daa(gb);
  CYC(b_+O(198), b_+OE(199)); mem_wr(gb, HL, A);
  CYC(b_+O(199), b_+OE(201)); A = 0x5e; // SND_GETSEED
  CALL_C(b_+O(201), playSound_b00_hook, SYM(playSound_b00), b_+OE(204));

delete_:
  CYC(b_+O(204), b_+OE(207)); TAIL(interactionDelete); // jp

state3:
  CYC(b_+O(207), b_+OE(209)); E = INTERACTION_BASE + OBJ_VAR2A;
  CYC(b_+O(209), b_+OE(210)); A = mem_rd(gb, DE);
  CYC(b_+O(210), b_+OE(212)); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+O(212), b_+OE(213)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(212), b_+OE(213));
  CYC(b_+O(213), b_+OE(215)); A = 0x80; // DISABLE_ALL_BUT_INTERACTIONS
  CYC(b_+O(215), b_+OE(218)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(218), b_+OE(221)); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+O(221), b_+OE(222)); H = D;
  CYC(b_+O(222), b_+OE(224)); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(224), b_+OE(226)); mem_wr(gb, HL, 0x04);
  CYC(b_+O(226), b_+OE(228)); A = 0x06;
  CALL_C(b_+O(228), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(231));
  CYC(b_+O(231), b_+OE(234)); SET_BC(0xfec0); // -$140
  CALL_C(b_+O(234), objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+OE(237));
  CYC(b_+O(237), b_+OE(239)); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+O(239), b_+OE(241)); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+O(241), objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+OE(244));
  CYC(b_+O(244), b_+OE(246)); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(246), b_+OE(247)); mem_wr(gb, DE, A);
  CYC(b_+O(247), b_+OE(250)); TAIL(objectSetVisible80); // jp

state4:
  CYC(b_+O(250), b_+OE(253)); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+O(253), b_+OE(254)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(254), b_+OE(257)); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+O(254), b_+OE(257));
  CALL_C(b_+O(257), objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+OE(260));
  if (F & FC) { CYCT(b_+O(260), b_+OE(262)); goto l_4503; } // jr c
  CYC(b_+O(260), b_+OE(262));
  CALL_C(b_+O(262), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(265));
  CYC(b_+O(265), b_+OE(267)); C = 0x20;
  CYC(b_+O(267), b_+OE(270)); TAIL(objectUpdateSpeedZ_paramC); // jp

l_4503:
  CALL_C(b_+O(270), interactionIncState_hook, SYM(interactionIncState), b_+OE(273));
  CYC(b_+O(273), b_+OE(275)); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+O(275), b_+OE(277)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+O(277), b_+OE(280)); SET_BC(0x3501); // TX_3501
  CYC(b_+O(280), b_+OE(283)); TAIL(showText); // jp

state5:
  CYC(b_+O(283), b_+OE(286)); SET_HL(wGashaSpotFlags);
  CYC(b_+O(286), b_+OE(288)); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(288), b_+OE(290)); goto l_451d; } // jr nz
  CYC(b_+O(288), b_+OE(290));
  CYC(b_+O(290), b_+OE(292)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(b_+O(292), b_+OE(294)); B = 0x04; // GASHATREASURE_TIER3_RING
  CYCT(b_+O(294), b_+OE(296)); goto spawnTreasure; // jr

l_451d:
  CYC(b_+O(296), b_+OE(298)); C = 0x00;
  CYC(b_+O(298), b_+OE(301)); SET_HL(wGashaMaturity + 1);
  CYC(b_+O(301), b_+OE(302)); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+O(302), b_+OE(304)); A = alu_srl(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(304), b_+OE(306)); goto l_4533; } // jr nz
  CYC(b_+O(304), b_+OE(306));
  CYC(b_+O(306), b_+OE(307)); A = mem_rd(gb, HL);
  CYC(b_+O(307), b_+OE(308)); alu_rra(gb);
  CYC(b_+O(308), b_+OE(311)); SET_HL(b_+O(774)); // @gashaMaturityValues

l_452c:
  CYC(b_+O(311), b_+OE(312)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+O(312), b_+OE(314)); goto l_4533; } // jr nc
  CYC(b_+O(312), b_+OE(314));
  CYC(b_+O(314), b_+OE(315)); SET_HL(HL + 1); // inc hl
  CYC(b_+O(315), b_+OE(316)); C = alu_inc8(gb, C);
  CYCT(b_+O(316), b_+OE(318)); goto l_452c; // jr

l_4533:
  CYC(b_+O(318), b_+OE(320)); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(320), b_+OE(321)); A = mem_rd(gb, DE);
  CYC(b_+O(321), b_+OE(324)); SET_HL(b_+O(799)); // @gashaSpotRanks
  CYC(b_+O(324), b_+OE(325)); interactionCodeb6_addAToHl(gb, b_+O(325));
  CYC(b_+O(325), b_+OE(326)); A = mem_rd(gb, HL);
  CYC(b_+O(326), b_+OE(327)); interactionCodeb6_addAToHl(gb, b_+O(327));
  CYC(b_+O(327), b_+OE(328)); A = C;
  CYC(b_+O(328), b_+OE(329)); alu_add(gb, A);
  CYC(b_+O(329), b_+OE(330)); C = A;
  CYC(b_+O(330), b_+OE(331)); alu_add(gb, A);
  CYC(b_+O(331), b_+OE(332)); alu_add(gb, A);
  CYC(b_+O(332), b_+OE(333)); alu_add(gb, C);
  CYC(b_+O(333), b_+OE(334)); interactionCodeb6_addAToHl(gb, b_+O(334));
  CALL_C(b_+O(334), getRandomIndexFromProbabilityDistribution_hook, SYM(getRandomIndexFromProbabilityDistribution), b_+OE(337));
  CYC(b_+O(337), b_+OE(338)); A = B;
  CYC(b_+O(338), b_+OE(340)); alu_cp(gb, 0x06); // GASHATREASURE_POTION
  if (!(F & FZ)) { CYCT(b_+O(340), b_+OE(342)); goto notPotion; } // jr nz
  CYC(b_+O(340), b_+OE(342));
  CYC(b_+O(342), b_+OE(344)); A = 0x2f; // TREASURE_POTION
  CALL_C(b_+O(344), checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+OE(347));
  if (!(F & FC)) { CYCT(b_+O(347), b_+OE(349)); goto decGashaMaturity; } // jr nc
  CYC(b_+O(347), b_+OE(349));
  CYC(b_+O(349), b_+OE(352)); SET_HL(wLinkMaxHealth);
  CYC(b_+O(352), b_+OE(353)); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+O(353), b_+OE(354)); mem_wr(gb, HL, A);
  CYCT(b_+O(354), b_+OE(356)); goto decGashaMaturity; // jr

notPotion:
  CYC(b_+O(356), b_+OE(358)); alu_cp(gb, 0x00); // GASHATREASURE_HEART_PIECE
  if (!(F & FZ)) { CYCT(b_+O(358), b_+OE(360)); goto decGashaMaturity; } // jr nz
  CYC(b_+O(358), b_+OE(360));
  CYC(b_+O(360), b_+OE(363)); SET_HL(wGashaSpotFlags);
  CYC(b_+O(363), b_+OE(365)); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(365), b_+OE(367)); goto l_4565; } // jr z
  CYC(b_+O(365), b_+OE(367));
  CYC(b_+O(367), b_+OE(368)); B = alu_inc8(gb, B);

l_4565:
  CYC(b_+O(368), b_+OE(370)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)

decGashaMaturity:
  CYC(b_+O(370), b_+OE(373)); SET_HL(wGashaMaturity);
  CYC(b_+O(373), b_+OE(374)); A = mem_rd(gb, HL);
  CYC(b_+O(374), b_+OE(376)); alu_sub(gb, 0xc8); // 200
  CYC(b_+O(376), b_+OE(377)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(377), b_+OE(378)); A = mem_rd(gb, HL);
  CYC(b_+O(378), b_+OE(380)); alu_sbc(gb, 0x00);
  CYC(b_+O(380), b_+OE(381)); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+O(381), b_+OE(383)); goto spawnTreasure; } // jr nc
  CYC(b_+O(381), b_+OE(383));
  CYC(b_+O(383), b_+OE(384)); alu_xor(gb, A);
  CYC(b_+O(384), b_+OE(385)); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+O(385), b_+OE(386)); mem_wr(gb, HL, A);

spawnTreasure:
  CYC(b_+O(386), b_+OE(387)); A = B;
  CYC(b_+O(387), b_+OE(389)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(389), b_+OE(390)); mem_wr(gb, DE, A);
  CYC(b_+O(390), b_+OE(393)); SET_HL(b_+O(779)); // @gashaTreasures
  CYC(b_+O(393), b_+OE(394)); interactionCodeb6_addDoubleIndex(gb, b_+O(394));
  CYC(b_+O(394), b_+OE(395)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(395), b_+OE(396)); C = mem_rd(gb, HL);
  CYC(b_+O(396), b_+OE(398)); alu_cp(gb, 0x2d); // TREASURE_RING
  if (!(F & FZ)) { CYCT(b_+O(398), b_+OE(400)); goto l_4588; } // jr nz
  CYC(b_+O(398), b_+OE(400));
  CALL_C(b_+O(400), getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+OE(403));

l_4588:
  CYC(b_+O(403), b_+OE(404)); B = A;
  CALL_C(b_+O(404), giveTreasure_hook, SYM(giveTreasure), b_+OE(407));
  CYC(b_+O(407), b_+OE(410)); SET_HL(wLinkForceState);
  CYC(b_+O(410), b_+OE(412)); A = 0x04; // LINK_STATE_04
  CYC(b_+O(412), b_+OE(413)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(413), b_+OE(415)); mem_wr(gb, HL, 0x01); // [wcc50]
  CYC(b_+O(415), b_+OE(418)); SET_HL(w1Link_yh);
  CYC(b_+O(418), b_+OE(421)); SET_BC(0xf300);
  CALL_C(b_+O(421), objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+OE(424));
  CALL_C(b_+O(424), interactionIncState_hook, SYM(interactionIncState), b_+OE(427));
  CYC(b_+O(427), b_+OE(429)); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+O(429), b_+OE(431)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+O(431), b_+OE(433)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(433), b_+OE(434)); A = mem_rd(gb, DE);
  CYC(b_+O(434), b_+OE(436)); alu_cp(gb, 0x00); // GASHATREASURE_HEART_PIECE
  CYC(b_+O(436), b_+OE(438)); A = 0x4c; // SND_GETITEM
  if (!(F & FZ)) { CALL_C_CC(b_+O(438), playSound_b00_hook, SYM(playSound_b00), b_+OE(441)); } else { CYC(b_+O(438), b_+OE(441)); } // call nz
  CALL_C(b_+O(441), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(444));
  CYC(b_+O(444), b_+OE(446)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(446), b_+OE(447)); A = mem_rd(gb, DE);
  CYC(b_+O(447), b_+OE(450)); SET_HL(b_+O(457)); // @lowTextIndices
  CYC(b_+O(450), b_+OE(451)); interactionCodeb6_addAToHl(gb, b_+O(451));
  CYC(b_+O(451), b_+OE(452)); C = mem_rd(gb, HL);
  CYC(b_+O(452), b_+OE(454)); B = 0x35; // >TX_3500
  CYC(b_+O(454), b_+OE(457)); TAIL(showText); // jp

state6:
  CYC(b_+O(467), b_+OE(470)); SET_HL(wNumRupees);
  CYC(b_+O(470), b_+OE(473)); A = mem_rd(gb, wDisplayedRupees);
  CYC(b_+O(473), b_+OE(474)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(474), b_+OE(475)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(474), b_+OE(475));
  CYC(b_+O(475), b_+OE(476)); L = alu_inc8(gb, L);
  CYC(b_+O(476), b_+OE(479)); A = mem_rd(gb, wDisplayedRupees + 1);
  CYC(b_+O(479), b_+OE(480)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(480), b_+OE(481)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(480), b_+OE(481));
  CYC(b_+O(481), b_+OE(483)); L = GV(0xaa, 0xa2); // wLinkHealth low byte
  CYC(b_+O(483), b_+OE(486)); A = mem_rd(gb, wDisplayedHearts);
  CYC(b_+O(486), b_+OE(487)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(487), b_+OE(488)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(487), b_+OE(488));
  CYC(b_+O(488), b_+OE(490)); A = 0x91; // SND_FAIRYCUTSCENE
  CALL_C(b_+O(490), playSound_b00_hook, SYM(playSound_b00), b_+OE(493));
  CYC(b_+O(493), b_+OE(495)); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(495), b_+OE(496)); A = mem_rd(gb, DE);
  CYC(b_+O(496), b_+OE(499)); SET_HL(b_+O(1186)); // @gfxHeaderToLoadWhenTreeDisappears
  CYC(b_+O(499), b_+OE(500)); interactionCodeb6_addAToHl(gb, b_+O(500));
  CYC(b_+O(500), b_+OE(501)); A = mem_rd(gb, HL);
  CYC(b_+O(501), b_+OE(502)); push_effect(gb, AF); // push af
  CYC(b_+O(502), b_+OE(504)); alu_sub(gb, 0x39); // GFXH_GASHA_TREE_DISAPPEARED - OBJ_GFXH_04
  CYC(b_+O(504), b_+OE(507)); mem_wr(gb, wLoadedTreeGfxActive, A);
  CYC(b_+O(507), b_+OE(509)); A = 0x3d; // GFXH_GASHA_TREE_DISAPPEARED
  CALL_C(b_+O(509), loadGfxHeader_hook, SYM(loadGfxHeader), b_+OE(512));
  CYC(b_+O(512), b_+OE(513)); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+O(513), b_+OE(515)); alu_cp(gb, 0x3d);
  if (!(F & FZ)) { CALL_C_CC(b_+O(515), loadGfxHeader_hook, SYM(loadGfxHeader), b_+OE(518)); } else { CYC(b_+O(515), b_+OE(518)); } // call nz
  CYC(b_+O(518), b_+OE(520)); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(b_+O(520), b_+OE(521)); D = A;
  CYC(b_+O(521), b_+OE(522)); H = D;
  CYC(b_+O(522), b_+OE(524)); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+O(524), b_+OE(526)); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(526), b_+OE(527)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(527), b_+OE(528)); mem_wr(gb, DE, A);
  CYC(b_+O(528), b_+OE(530)); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(530), b_+OE(531)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(531), b_+OE(532)); mem_wr(gb, DE, A);
  CYC(b_+O(532), b_+OE(534)); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+O(534), b_+OE(536)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+O(536), objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+OE(539));
  CYC(b_+O(539), b_+OE(540)); alu_xor(gb, A);
  CYC(b_+O(540), b_+OE(541)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(541), b_+OE(542)); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+O(542), b_+OE(543)); A = L;
  CYC(b_+O(543), b_+OE(545)); alu_sub(gb, 0x10);
  CYC(b_+O(545), b_+OE(546)); L = A;
  CYC(b_+O(546), b_+OE(547)); alu_xor(gb, A);
  CYC(b_+O(547), b_+OE(548)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(548), b_+OE(549)); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+O(549), b_+OE(550)); H = A;
  CYC(b_+O(550), b_+OE(551)); E = L;
  CYC(b_+O(551), b_+OE(552)); A = L;
  CYC(b_+O(552), b_+OE(554)); alu_and(gb, 0xf0);
  CYC(b_+O(554), b_+OE(555)); L = A;
  CYC(b_+O(555), b_+OE(556)); A = E;
  CYC(b_+O(556), b_+OE(558)); alu_and(gb, 0x0f);
  CYC(b_+O(558), b_+OE(560)); L = alu_sla(gb, L);
  CYC(b_+O(560), b_+OE(562)); H = alu_rl(gb, H);
  CYC(b_+O(562), b_+OE(563)); alu_add(gb, L);
  CYC(b_+O(563), b_+OE(564)); L = A;
  CYC(b_+O(564), b_+OE(566)); L = alu_sla(gb, L);
  CYC(b_+O(566), b_+OE(568)); H = alu_rl(gb, H);
  CYC(b_+O(568), b_+OE(571)); SET_BC(w3VramTiles);
  CYC(b_+O(571), b_+OE(572)); alu_add_hl(gb, BC);
  CYC(b_+O(572), b_+OE(574)); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+O(574), b_+OE(575)); A = L;
  CYC(b_+O(575), b_+OE(576)); mem_wr(gb, DE, A);
  CYC(b_+O(576), b_+OE(577)); E = alu_inc8(gb, E);
  CYC(b_+O(577), b_+OE(578)); A = H;
  CYC(b_+O(578), b_+OE(579)); mem_wr(gb, DE, A);
  CYC(b_+O(579), b_+OE(582)); SET_BC(0x0400);
  CYC(b_+O(582), b_+OE(583)); alu_add_hl(gb, BC);
  CYC(b_+O(583), b_+OE(585)); A = hram_rd(gb, R_SVBK);
  CYC(b_+O(585), b_+OE(586)); push_effect(gb, AF); // push af
  CYC(b_+O(586), b_+OE(588)); A = 0x03; // :w3VramAttributes
  CYC(b_+O(588), b_+OE(590)); hram_wr(gb, R_SVBK, A);
  CYC(b_+O(590), b_+OE(592)); B = 0x04;

l_4645:
  CYC(b_+O(592), b_+OE(594)); C = 0x04;
  CYC(b_+O(594), b_+OE(595)); push_effect(gb, BC); // push bc

l_4648:
  CYC(b_+O(595), b_+OE(596)); A = mem_rd(gb, HL);
  CYC(b_+O(596), b_+OE(598)); alu_and(gb, 0xf0);
  CYC(b_+O(598), b_+OE(600)); alu_or(gb, 0x04);
  CYC(b_+O(600), b_+OE(601)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(601), b_+OE(602)); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+O(602), b_+OE(604)); goto l_4648; } // jr nz
  CYC(b_+O(602), b_+OE(604));
  CYC(b_+O(604), b_+OE(607)); SET_BC(0x001c);
  CYC(b_+O(607), b_+OE(608)); alu_add_hl(gb, BC);
  CYC(b_+O(608), b_+OE(609)); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+O(609), b_+OE(610)); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+O(610), b_+OE(612)); goto l_4645; } // jr nz
  CYC(b_+O(610), b_+OE(612));
  CYC(b_+O(612), b_+OE(613)); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+O(613), b_+OE(615)); hram_wr(gb, R_SVBK, A);
  CALL_C(b_+O(615), interactionIncState_hook, SYM(interactionIncState), b_+OE(618));
  CYC(b_+O(618), b_+OE(620)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(620), b_+OE(622)); mem_wr(gb, HL, 0x08);

state7:
  CYC(b_+O(622), b_+OE(623)); H = D;
  CYC(b_+O(623), b_+OE(625)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(625), b_+OE(626)); A = mem_rd(gb, HL);
  CYC(b_+O(626), b_+OE(627)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(627), b_+OE(629)); goto counter1Done; } // jr z
  CYC(b_+O(627), b_+OE(629));
  CYC(b_+O(629), b_+OE(630)); A = alu_dec8(gb, A);
  CYC(b_+O(630), b_+OE(631)); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+O(631), b_+OE(632)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(631), b_+OE(632));

counter1Done:
  CYC(b_+O(632), b_+OE(634)); A = 0x08;
  CYC(b_+O(634), b_+OE(635)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(635), b_+OE(636)); A = mem_rd(gb, HL);
  CYC(b_+O(636), b_+OE(637)); A = alu_inc8(gb, A);
  CYC(b_+O(637), b_+OE(638)); mem_wr(gb, HL, A);
  CYC(b_+O(638), b_+OE(640)); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+O(640), b_+OE(642)); goto counter2Done; } // jr nc
  CYC(b_+O(640), b_+OE(642));
  CYC(b_+O(642), b_+OE(645)); SET_HL(SYM(interactionCodeb6__treeDisappearanceFrames) - 1);
  CYC(b_+O(645), b_+OE(646)); interactionCodeb6_addAToHl(gb, b_+O(646));
  CYC(b_+O(646), b_+OE(647)); A = mem_rd(gb, HL);
  CYC(b_+O(647), b_+OE(648)); interactionCodeb6_addAToHl(gb, b_+O(648));
  CYC(b_+O(648), b_+OE(650)); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+O(650), b_+OE(651)); A = mem_rd(gb, DE);
  CYC(b_+O(651), b_+OE(652)); C = A;
  CYC(b_+O(652), b_+OE(653)); E = alu_inc8(gb, E);
  CYC(b_+O(653), b_+OE(654)); A = mem_rd(gb, DE);
  CYC(b_+O(654), b_+OE(655)); D = A;
  CYC(b_+O(655), b_+OE(656)); E = C;
  CYC(b_+O(656), b_+OE(657)); push_effect(gb, HL); // push hl
  CYC(b_+O(657), b_+OE(658)); push_effect(gb, DE); // push de
  CYC(b_+O(658), b_+OE(659)); SET_HL(pop_effect(gb)); // pop hl
  CYC(b_+O(659), b_+OE(660)); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+O(660), b_+OE(662)); A = hram_rd(gb, R_SVBK);
  CYC(b_+O(662), b_+OE(663)); push_effect(gb, AF); // push af
  CYC(b_+O(663), b_+OE(665)); A = 0x03; // :w3VramTiles
  CYC(b_+O(665), b_+OE(667)); hram_wr(gb, R_SVBK, A);
  CYC(b_+O(667), b_+OE(669)); B = 0x04;

l_4692:
  CYC(b_+O(669), b_+OE(671)); C = 0x04;
  CYC(b_+O(671), b_+OE(672)); push_effect(gb, BC); // push bc

l_4695:
  CYC(b_+O(672), b_+OE(673)); A = mem_rd(gb, DE);
  CYC(b_+O(673), b_+OE(674)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(674), b_+OE(675)); SET_DE(DE + 1); // inc de
  CYC(b_+O(675), b_+OE(676)); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+O(676), b_+OE(678)); goto l_4695; } // jr nz
  CYC(b_+O(676), b_+OE(678));
  CYC(b_+O(678), b_+OE(681)); SET_BC(0x001c);
  CYC(b_+O(681), b_+OE(682)); alu_add_hl(gb, BC);
  CYC(b_+O(682), b_+OE(683)); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+O(683), b_+OE(684)); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+O(684), b_+OE(686)); goto l_4692; } // jr nz
  CYC(b_+O(684), b_+OE(686));
  CYC(b_+O(686), b_+OE(687)); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+O(687), b_+OE(689)); hram_wr(gb, R_SVBK, A);
  CYC(b_+O(689), b_+OE(691)); A = 0x29; // UNCMP_GFXH_29
  CALL_C(b_+O(691), loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+OE(694));
  CYC(b_+O(694), b_+OE(696)); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(b_+O(696), b_+OE(697)); D = A;
  CYC(b_+O(697), b_+OE(699)); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+O(699), b_+OE(700)); A = mem_rd(gb, DE);
  CYC(b_+O(700), b_+OE(702)); alu_add(gb, 0x1f); // UNCMP_GFXH_20 - 1
  CALL_C(b_+O(702), loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+OE(705));
  CALL_C(b_+O(705), reloadTileMap_hook, SYM(reloadTileMap), b_+OE(708));
  CYC(b_+O(708), b_+OE(710)); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(b_+O(710), b_+OE(711)); D = A;
  RET(b_+O(711)); return;

counter2Done:
  CYC(b_+O(712), b_+OE(713)); alu_xor(gb, A);
  CYC(b_+O(713), b_+OE(716)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(716), b_+OE(719)); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+O(719), b_+OE(721)); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+O(721), b_+OE(722)); A = mem_rd(gb, DE);
  CYC(b_+O(722), b_+OE(725)); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(b_+O(725), unsetFlag_hook, SYM(unsetFlag), b_+OE(728));
  if (!game_seasons) {
    CYC(b_+728, b_+730); A = 0x4e; // TILEINDEX_GASHA_TREE_TL
    CALL_C(b_+730, findTileInRoom_hook, SYM(findTileInRoom), b_+733);
    if (!(F & FZ)) { CYCT(b_+733, b_+734); ret_effect(gb); return; } // ret nz
    CYC(b_+733, b_+734);
    CYC(b_+734, b_+736); E = INTERACTION_BASE + OBJ_VAR03;
    CYC(b_+736, b_+737); A = mem_rd(gb, DE);
    CYC(b_+737, b_+740); SET_BC(b_+758); // @tileReplacements
    CALL_C(b_+740, addAToBc_hook, 0x006d, b_+743);
    CYC(b_+743, b_+744); A = mem_rd(gb, BC);
    CYC(b_+744, b_+745); B = A;
    CYC(b_+745, b_+746); A = B;
    CYC(b_+746, b_+747); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
    CYC(b_+747, b_+748); mem_wr(gb, HL, A);
    CYC(b_+748, b_+750); A = 0x0f;
    CYC(b_+750, b_+751); alu_add(gb, L);
    CYC(b_+751, b_+752); L = A;
    CYC(b_+752, b_+753); A = B;
    CYC(b_+753, b_+754); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
    CYC(b_+754, b_+755); mem_wr(gb, HL, A);
  }
  CYC(b_+O(755), b_+OE(758)); TAIL(interactionDelete); // jp
}
