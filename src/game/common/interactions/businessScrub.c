#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodece_jump_table(GB *gb) {
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

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodece_addAToHl(GB *gb, uint16_t return_address) {
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

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodece_addDoubleIndex(GB *gb, uint16_t return_address) {
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
// INTERAC_BUSINESS_SCRUB
//
// Variables:
//   var38: Number of rupees to spend (1-byte value, converted with "rupeeValue" methods)
//   var39: Set when Link is close to the scrub (he pops out of his bush)
//
// Every internal label below is reached only via jr/jp within this one routine; none are entered
// via a real `call`, so this whole interaction is a single C function with goto labels.
// ==================================================================================================
void interactionCodece_hook(GB *gb) {
  BASE(interactionCodece);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (interactionCodece_jump_table(gb));
    if (jt_ == b_+O(10)) { goto state0; }
    else if (jt_ == b_+O(117)) { goto state1; }
    else if (jt_ == b_+O(218)) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+O(10), b_+OE(12)); A = 0x01;
  CYC(b_+O(12), b_+OE(13)); mem_wr(gb, DE, A);
  CALL_C(b_+O(13), interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+OE(16));
  if (game_seasons) {
    // Subid 6 sells the cheap shield: delete once Moosh is here or essence 4 is obtained.
    CYC(b_+S(16), b_+S(18)); E = INTERACTION_BASE + OBJ_SUBID;
    CYC(b_+S(18), b_+S(19)); A = mem_rd(gb, DE);
    CYC(b_+S(19), b_+S(21)); alu_cp(gb, 0x06);
    if (!(F & FZ)) { CYCT(b_+S(21), b_+S(23)); goto checkSubid; }
    CYC(b_+S(21), b_+S(23));
    CYC(b_+S(23), b_+S(26)); SET_HL(w1Companion);
    CYC(b_+S(26), b_+S(27)); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+S(27), b_+S(28)); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+S(28), b_+S(30)); goto noCompanion; }
    CYC(b_+S(28), b_+S(30));
    CYC(b_+S(30), b_+S(31)); A = mem_rd(gb, HL);
    CYC(b_+S(31), b_+S(33)); alu_cp(gb, 0x0d); // SPECIALOBJECT_MOOSH
    if (F & FZ) { CYCT(b_+S(33), b_+S(35)); goto moosh; }
    CYC(b_+S(33), b_+S(35));
noCompanion:
    CYC(b_+S(35), b_+S(38)); A = W8(wEssencesObtained);
    CYC(b_+S(38), b_+S(40)); alu_bit(gb, 3, A);
    if (!(F & FZ)) { CYCT(b_+S(40), b_+S(42)); goto checkSubid; }
    CYC(b_+S(40), b_+S(42));
moosh:
    CYC(b_+S(42), b_+S(45)); TAIL(interactionDelete);
  }
checkSubid:
  CYC(b_+O(16), b_+OE(18)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(18), b_+OE(19)); A = mem_rd(gb, DE);
  CYC(b_+O(19), b_+OE(21)); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+O(21), b_+OE(23)); goto mimicBush; } // jr nz
  CYC(b_+O(21), b_+OE(23));
  CYC(b_+O(23), b_+OE(25)); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+O(25), b_+OE(27)); goto sellingShield; } // jr z
  CYC(b_+O(25), b_+OE(27));
  CYC(b_+O(27), b_+OE(29)); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+O(29), b_+OE(31)); goto sellingShield; } // jr z
  CYC(b_+O(29), b_+OE(31));
  CYC(b_+O(31), b_+OE(33)); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+O(33), b_+OE(35)); goto afterSellingShield; } // jr nz
  CYC(b_+O(33), b_+OE(35));

sellingShield:
  CYC(b_+O(35), b_+OE(36)); C = A;
  CYC(b_+O(36), b_+OE(39)); A = mem_rd(gb, wShieldLevel);
  CYC(b_+O(39), b_+OE(40)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(40), b_+OE(42)); goto sellingShieldSetPrice; } // jr z
  CYC(b_+O(40), b_+OE(42));
  CYC(b_+O(42), b_+OE(43)); A = alu_dec8(gb, A);

sellingShieldSetPrice:
  CYC(b_+O(43), b_+OE(44)); alu_add(gb, C);
  CYC(b_+O(44), b_+OE(45)); mem_wr(gb, DE, A);
  CYC(b_+O(45), b_+OE(48)); SET_HL(b_+(game_seasons ? S(445) : 412)); // @itemPrices
  CYC(b_+O(48), b_+OE(49)); interactionCodece_addDoubleIndex(gb, b_+O(49));
  CYC(b_+O(49), b_+OE(50)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(50), b_+OE(51)); B = mem_rd(gb, HL);
  CYC(b_+O(51), b_+OE(54)); SET_HL(wTextNumberSubstitution);
  CYC(b_+O(54), b_+OE(55)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(55), b_+OE(56)); mem_wr(gb, HL, B);

afterSellingShield:
  CYC(b_+O(56), b_+OE(58)); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+O(58), b_+OE(60)); A = 0x06;
  CYC(b_+O(60), b_+OE(61)); mem_wr(gb, DE, A);
  CYC(b_+O(61), b_+OE(62)); E = alu_inc8(gb, E);
  CYC(b_+O(62), b_+OE(63)); mem_wr(gb, DE, A);
  CALL_C(b_+O(63), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(66));
  CALL_C(b_+O(66), objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+OE(69));
  CYC(b_+O(69), b_+OE(71)); H = 0xcf; // >wRoomLayout
  CYC(b_+O(71), b_+OE(73)); mem_wr(gb, HL, 0x00);
  CALL_C(b_+O(73), objectSetVisible80_hook, SYM(objectSetVisible80), b_+OE(76));
  CYC(b_+O(76), b_+OE(78)); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+O(78), objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+OE(81));
  CALL_C(b_+O(81), getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+OE(84));
  CYC(b_+O(84), b_+OE(86)); A = 0xce; // INTERAC_BUSINESS_SCRUB
  CYC(b_+O(86), b_+OE(87)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(87), b_+OE(89)); A = 0x80;
  CYC(b_+O(89), b_+OE(90)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(90), b_+OE(92)); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+O(92), b_+OE(93)); mem_wr(gb, HL, D);
  CYC(b_+O(93), b_+OE(96)); TAIL(objectCopyPosition); // jp

// Subid $80 initialization (the bush above the scrub)
mimicBush:
  CYC(b_+O(96), b_+OE(99)); A = mem_rd(gb, wActiveGroup);
  CYC(b_+O(99), b_+OE(100)); alu_or(gb, A);
  CYC(b_+O(100), b_+OE(102)); A = GV(0xc5, 0xc4); // TILEINDEX_OVERWORLD_BUSH_1
  if (F & FZ) { CYCT(b_+O(102), b_+OE(104)); goto mimicBushSetTile; } // jr z
  CYC(b_+O(102), b_+OE(104));
  CYC(b_+O(104), b_+OE(106)); A = GV(0xc5, 0x20); // TILEINDEX_OVERWORLD_BUSH_1, TILEINDEX_DUNGEON_BUSH

mimicBushSetTile:
  CALL_C(b_+O(106), objectMimicBgTile_hook, SYM(objectMimicBgTile), b_+OE(109));
  CYC(b_+O(109), b_+OE(111)); A = 0x05;
  CALL_C(b_+O(111), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(114));
  CYC(b_+O(114), b_+OE(117)); TAIL(objectSetVisible80); // jp

state1:
  CYC(b_+O(117), b_+OE(120)); A = mem_rd(gb, wScrollMode);
  CYC(b_+O(120), b_+OE(122)); alu_and(gb, 0x0e); // SCROLLMODE_08 | SCROLLMODE_04 | SCROLLMODE_02
  if (!(F & FZ)) { CYCT(b_+O(122), b_+OE(123)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(122), b_+OE(123));
  CYC(b_+O(123), b_+OE(125)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(125), b_+OE(126)); A = mem_rd(gb, DE);
  CYC(b_+O(126), b_+OE(128)); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+O(128), b_+OE(130)); goto subid80State1; } // jr nz
  CYC(b_+O(128), b_+OE(130));
  CALL_C(b_+O(130), objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+OE(133));
  CALL_C(b_+O(133), interactionAnimate_hook, SYM(interactionAnimate), b_+OE(136));
  CYC(b_+O(136), b_+OE(138)); C = 0x20;
  CALL_C(b_+O(138), objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+OE(141));
  CYC(b_+O(141), b_+OE(143)); E = INTERACTION_BASE + OBJ_VAR39;
  if (F & FC) { CYCT(b_+O(143), b_+OE(145)); goto linkIsClose; } // jr c
  CYC(b_+O(143), b_+OE(145));

  // Link not close
  CYC(b_+O(145), b_+OE(146)); A = mem_rd(gb, DE);
  CYC(b_+O(146), b_+OE(147)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(147), b_+OE(148)); ret_effect(gb); return; } // ret z
  CYC(b_+O(147), b_+OE(148));
  CYC(b_+O(148), b_+OE(149)); alu_xor(gb, A);
  CYC(b_+O(149), b_+OE(150)); mem_wr(gb, DE, A);
  CYC(b_+O(150), b_+OE(152)); A = 0x03;
  CYC(b_+O(152), b_+OE(155)); TAIL(interactionSetAnimation); // jp

linkIsClose:
  CYC(b_+O(155), b_+OE(156)); A = mem_rd(gb, DE);
  CYC(b_+O(156), b_+OE(157)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(157), b_+OE(159)); goto afterTalkedCheck; } // jr nz
  CYC(b_+O(157), b_+OE(159));
  CYC(b_+O(159), b_+OE(160)); A = alu_inc8(gb, A);
  CYC(b_+O(160), b_+OE(161)); mem_wr(gb, DE, A);
  CYC(b_+O(161), b_+OE(163)); A = 0x01;
  CYC(b_+O(163), b_+OE(166)); TAIL(interactionSetAnimation); // jp

afterTalkedCheck:
  CYC(b_+O(166), b_+OE(168)); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+O(168), b_+OE(169)); A = mem_rd(gb, DE);
  CYC(b_+O(169), b_+OE(170)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(170), b_+OE(171)); ret_effect(gb); return; } // ret z
  CYC(b_+O(170), b_+OE(171));

  // Link talked to the scrub
  CALL_C(b_+O(171), interactionIncState_hook, SYM(interactionIncState), b_+OE(174));
  CYC(b_+O(174), b_+OE(176)); A = 0x02;
  CALL_C(b_+O(176), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(179));
  CYC(b_+O(179), b_+OE(181)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(181), b_+OE(182)); A = mem_rd(gb, DE);
  CYC(b_+O(182), b_+OE(185)); SET_HL(b_+(game_seasons ? S(398) : 369)); // @offerItemTextIndices
  CYC(b_+O(185), b_+OE(186)); interactionCodece_addAToHl(gb, b_+O(186));
  CYC(b_+O(186), b_+OE(187)); C = mem_rd(gb, HL);
  CYC(b_+O(187), b_+OE(189)); B = 0x45; // >TX_4500
  CYC(b_+O(189), b_+OE(192)); TAIL(showTextNonExitable); // jp

// Subid $80: the bush above the scrub
subid80State1:
  CYC(b_+O(192), b_+OE(194)); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+O(194), b_+OE(195)); A = mem_rd(gb, DE);
  CYC(b_+O(195), b_+OE(196)); H = A;
  CYC(b_+O(196), b_+OE(198)); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+O(198), b_+OE(199)); A = mem_rd(gb, HL);
  CYC(b_+O(199), b_+OE(201)); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+O(201), b_+OE(202)); mem_wr(gb, DE, A);
  CYC(b_+O(202), b_+OE(204)); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(204), b_+OE(205)); B = mem_rd(gb, HL);
  CYC(b_+O(205), b_+OE(207)); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+O(207), b_+OE(208)); A = mem_rd(gb, HL);
  CYC(b_+O(208), b_+OE(211)); SET_HL(b_+(game_seasons ? S(409) : 378)); // @bushYOffsets
  CYC(b_+O(211), b_+OE(212)); interactionCodece_addAToHl(gb, b_+O(212));
  CYC(b_+O(212), b_+OE(214)); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(214), b_+OE(215)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(215), b_+OE(216)); alu_add(gb, B);
  CYC(b_+O(216), b_+OE(217)); mem_wr(gb, DE, A);
  RET(b_+O(217)); return;

state2:
  CALL_C(b_+O(218), interactionAnimate_hook, SYM(interactionAnimate), b_+OE(221));
  CYC(b_+O(221), b_+OE(224)); A = mem_rd(gb, wTextIsActive);
  CYC(b_+O(224), b_+OE(226)); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+O(226), b_+OE(227)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(226), b_+OE(227));

  // Link just finished talking to the scrub
  CYC(b_+O(227), b_+OE(230)); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+O(230), b_+OE(232)); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+O(232), b_+OE(234)); goto label_0b_103; } // jr z
  CYC(b_+O(232), b_+OE(234));
  CYC(b_+O(234), b_+OE(236)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(236), b_+OE(238)); A = 0x01;
  CYC(b_+O(238), b_+OE(239)); mem_wr(gb, DE, A);
  CYC(b_+O(239), b_+OE(240)); alu_xor(gb, A);
  CYC(b_+O(240), b_+OE(243)); mem_wr(gb, wTextIsActive, A);
  CYC(b_+O(243), b_+OE(245)); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+O(245), b_+OE(246)); mem_wr(gb, DE, A);
  CYC(b_+O(246), b_+OE(247)); A = alu_dec8(gb, A);
  CYC(b_+O(247), b_+OE(250)); mem_wr(gb, wSelectedTextOption, A);
  CYC(b_+O(250), b_+OE(252)); A = 0x04;
  CYC(b_+O(252), b_+OE(255)); TAIL(interactionSetAnimation); // jp

label_0b_103:
  CYC(b_+O(255), b_+OE(258)); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+O(258), b_+OE(259)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(259), b_+OE(261)); goto agreedToBuy; } // jr z
  CYC(b_+O(259), b_+OE(261));

  // Declined to buy
  CYC(b_+O(261), b_+OE(264)); SET_BC(0x4506); // TX_4506
  CYC(b_+O(264), b_+OE(266)); goto showTextLabel; // jr $4ac8

agreedToBuy:
  CYC(b_+O(266), b_+OE(268)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(268), b_+OE(269)); A = mem_rd(gb, DE);
  CYC(b_+O(269), b_+OE(272)); SET_HL(b_+(game_seasons ? S(412) : 381)); // @rupeeValues
  CYC(b_+O(272), b_+OE(273)); interactionCodece_addAToHl(gb, b_+O(273));
  CYC(b_+O(273), b_+OE(274)); A = mem_rd(gb, HL);
  CYC(b_+O(274), b_+OE(276)); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+O(276), b_+OE(277)); mem_wr(gb, DE, A);
  CALL_C(b_+O(277), cpRupeeValue_hook, SYM(cpRupeeValue), b_+OE(280));
  if (F & FZ) { CYCT(b_+O(280), b_+OE(282)); goto enoughRupees; } // jr z
  CYC(b_+O(280), b_+OE(282));

  // Not enough rupees
  CYC(b_+O(282), b_+OE(285)); SET_BC(0x4507); // TX_4507
  CYC(b_+O(285), b_+OE(287)); goto showTextLabel; // jr $4ac8

enoughRupees:
  CYC(b_+O(287), b_+OE(289)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(289), b_+OE(290)); A = mem_rd(gb, DE);
  CYC(b_+O(290), b_+OE(293)); SET_HL(b_+(game_seasons ? S(423) : 390)); // @treasuresToSell
  CYC(b_+O(293), b_+OE(294)); interactionCodece_addDoubleIndex(gb, b_+O(294));
  CYC(b_+O(294), b_+OE(295)); A = mem_rd(gb, HL);
  CYC(b_+O(295), b_+OE(297)); alu_cp(gb, 0x03); // TREASURE_BOMBS
  if (F & FZ) { CYCT(b_+O(297), b_+OE(299)); goto giveBombs; } // jr z
  CYC(b_+O(297), b_+OE(299));
  CYC(b_+O(299), b_+OE(301)); alu_cp(gb, 0x20); // TREASURE_EMBER_SEEDS
  if (!(F & FZ)) { CYCT(b_+O(301), b_+OE(303)); goto giveShield; } // jr nz
  CYC(b_+O(301), b_+OE(303));

  // giveEmberSeeds (falls through, never a goto target)
  CYC(b_+O(303), b_+OE(306)); A = mem_rd(gb, wSeedSatchelLevel);
  CYC(b_+O(306), b_+OE(309)); SET_BC(b_+(game_seasons ? S(394) : 365)); // @maxSatchelCapacities-1
  CALL_C(b_+O(309), addAToBc_hook, 0x006d, b_+OE(312));
  CYC(b_+O(312), b_+OE(313)); A = mem_rd(gb, BC);
  CYC(b_+O(313), b_+OE(314)); C = A;
  CYC(b_+O(314), b_+OE(317)); A = mem_rd(gb, wNumEmberSeeds);
  CYC(b_+O(317), b_+OE(318)); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+O(318), b_+OE(320)); goto giveTreasure; } // jr nz
  CYC(b_+O(318), b_+OE(320));
  CYC(b_+O(320), b_+OE(322)); goto alreadyHaveTreasure; // jr $4ab0

giveBombs:
  CYC(b_+O(322), b_+OE(325)); SET_BC(wNumBombs);
  CYC(b_+O(325), b_+OE(326)); A = mem_rd(gb, BC);
  CYC(b_+O(326), b_+OE(327)); C = alu_inc8(gb, C);
  CYC(b_+O(327), b_+OE(328)); E = A;
  CYC(b_+O(328), b_+OE(329)); A = mem_rd(gb, BC);
  CYC(b_+O(329), b_+OE(330)); alu_cp(gb, E);
  if (!(F & FZ)) { CYCT(b_+O(330), b_+OE(332)); goto giveTreasure; } // jr nz
  CYC(b_+O(330), b_+OE(332));
  CYC(b_+O(332), b_+OE(334)); goto alreadyHaveTreasure; // jr $4ab0

giveShield:
  CALL_C(b_+O(334), checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+OE(337));
  if (!(F & FC)) { CYCT(b_+O(337), b_+OE(339)); goto giveTreasure; } // jr nc
  CYC(b_+O(337), b_+OE(339));

alreadyHaveTreasure:
  CYC(b_+O(339), b_+OE(342)); SET_BC(0x4508); // TX_4508
  CYC(b_+O(342), b_+OE(344)); goto showTextLabel; // jr $4ac8

giveTreasure:
  CYC(b_+O(344), b_+OE(345)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(345), b_+OE(346)); C = mem_rd(gb, HL);
  CALL_C(b_+O(346), giveTreasure_hook, SYM(giveTreasure), b_+OE(349));
  CYC(b_+O(349), b_+OE(351)); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+O(351), b_+OE(352)); A = mem_rd(gb, DE);
  CALL_C(b_+O(352), removeRupeeValue_hook, SYM(removeRupeeValue), b_+OE(355));
  CYC(b_+O(355), b_+OE(357)); A = 0x5e; // SND_GETSEED
  CALL_C(b_+O(357), playSound_b00_hook, SYM(playSound_b00), b_+OE(360));
  CYC(b_+O(360), b_+OE(363)); SET_BC(0x4505); // TX_4505

showTextLabel:
  CYC(b_+O(363), b_+OE(366)); TAIL(showText); // jp
}
