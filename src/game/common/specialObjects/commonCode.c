#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void linkAdjustGivenAngleInSidescrollingArea_hook(GB *gb);
void companionTryToBreakTileFromMoving_hook(GB *gb);
void companionCalculateAdjacentWallsBitset_hook(GB *gb);
void checkCollisionForCompanion_hook(GB *gb);
void specialObjectGetRelativeTileFromHl_hook(GB *gb);
void specialObjectCheckFacingWall_hook(GB *gb);
void companionUpdateDirectionAndSetAnimation_hook(GB *gb);
void companionSetAnimation_hook(GB *gb);
void setLinkMountingSpeed_hook(GB *gb);
void companionGotoHazardHandlingState_hook(GB *gb);
void companionDismount_hook(GB *gb);
void saveLinkLocalRespawnAndCompanionPosition_hook(GB *gb);
void specialObjectUpdatePositionGivenVelocity_hook(GB *gb);
void companionRetIfInactiveWithoutStateCheck_hook(GB *gb);

static void companion_ret_if_inactive_return_from_caller(GB *gb) {
  BASE(companionRetIfInactiveWithoutStateCheck);
  CYC(b_+19, b_+20); SET_AF(pop_effect(gb));
  CYC(b_+20, b_+21); ret_effect(gb);
}

static void common_code_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void common_code_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t common_code_jump_table(GB *gb) {
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

void specialObjectSetOamVariables_hook(GB *gb) {
  BASE(specialObjectSetOamVariables);
  CYC(b_+0, b_+2); E = 0x32;
  CYC(b_+2, b_+4); A = 0xff;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = 0x01;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+11); SET_HL(b_+22);
  CYC(b_+11, b_+12); common_code_add_double_index(gb, b_+12);
  CYC(b_+12, b_+14); E = 0x1d;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_dec8(gb, E);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_dec8(gb, E);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void dealSpikeDamageToLink_hook(GB *gb) {
  BASE(dealSpikeDamageToLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkRidingObject);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = 0x2b;
  CYC(b_+7, b_+8); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x28);
  CYC(b_+11, b_+13); A = 0x1d;
  CALL_C(b_+13, cpActiveRing_hook, SYM(cpActiveRing), b_+16);
  CYC(b_+16, b_+18); A = 0xfc;
  if (F & FZ) {
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+22); A = alu_sra(gb, A);
  } else {
    CYCT(b_+18, b_+20);
  }
  CYC(b_+22, b_+24); L = 0x25;
  CYC(b_+24, b_+25); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+28); L = 0x2a;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x80);
  CYC(b_+30, b_+32); L = 0x2d;
  CYC(b_+32, b_+34); A = 0x0a;
  CYC(b_+34, b_+35); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+38); E = 0x09;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); alu_xor(gb, 0x10);
  CYC(b_+41, b_+43); L = 0x2c;
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+46); A = 0x5f;
  CALL_C(b_+46, playSound_b00_hook, SYM(playSound_b00), b_+49);
  CYC(b_+49, b_+51);
  TAIL(linkApplyDamage_b5);
}

void updateLinkDamageTaken_hook(GB *gb) {
  BASE(updateLinkDamageTaken);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(GV((SYM(companionDismount) + 56), 0x4647));
  CYC(b_+3, b_+5); E = 0x06;
  CALL_C(b_+5, interBankCall_hook, 0x008a, SYM(linkApplyDamage_b5));
  TAIL(linkApplyDamage_b5);
}

void updateLinkInvincibilityCounter_hook(GB *gb) {
  BASE(updateLinkInvincibilityCounter);
  CYC(b_+0, b_+3); SET_HL(w1Link_invincibilityCounter);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto increment;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    goto normal_flags;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = W8(wFrameCounter);
  CYC(b_+16, b_+18); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20);
    goto normal_flags;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); L = 0x1c;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x0d);
  CYC(b_+24, b_+25); ret_effect(gb);
  return;

increment:
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

normal_flags:
  CYC(b_+26, b_+28); L = 0x1b;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void sidescrollUpdateActiveTile_hook(GB *gb) {
  BASE(sidescrollUpdateActiveTile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+6); W8(wActiveTileIndex) = A;
  CYC(b_+6, b_+9); SET_HL(SYM(tileTypesTable));
  CALL_C(b_+9, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+12);
  CYC(b_+12, b_+15); W8(wActiveTileType) = A;
  CYC(b_+15, b_+18); SET_BC(0x0800);
  CALL_C(b_+18, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+21);
  CYC(b_+21, b_+24); SET_HL(SYM(tileTypesTable));
  CALL_C(b_+24, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+27);
  CYC(b_+27, b_+30); W8(wLastActiveTileType) = A;
  CYC(b_+30, b_+31); ret_effect(gb);
}

static void link_apply_tile_types_adjust_conveyor(GB *gb) {
  BASE(linkApplyTileTypes);
  CYC(b_+O(306), b_+OE(308)); A = 0x01;
  CYC(b_+O(308), b_+OE(311)); W8(wcc92) = A;
  CYC(b_+O(311), b_+OE(314)); A = W8(wActiveTileType);
  CYC(b_+O(314), b_+OE(315)); alu_sub(gb, C);
  CYC(b_+O(315), b_+OE(318)); SET_HL(b_+O(323));
  CYC(b_+O(318), b_+OE(319)); common_code_add_a_to_hl(gb, b_+O(319));
  CYC(b_+O(319), b_+OE(320)); C = mem_rd(gb, HL);
  CYC(b_+O(320), b_+OE(323));
  specialObjectUpdatePositionGivenVelocity_hook(gb);
}

void linkApplyTileTypes_hook(GB *gb) {
  BASE(linkApplyTileTypes);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(1)); alu_xor(gb, A);
  CYC(b_+O(1), b_+OE(4)); W8(wIsTileSlippery) = A;
  CYC(b_+O(4), b_+OE(7)); A = W8(wLinkInAir);
  CYC(b_+O(7), b_+OE(8)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(8), b_+OE(11));
    goto normal;
  }
  CYC(b_+O(8), b_+OE(11));
  if (!game_seasons) { CYC(b_+11, b_+14); W8(wLinkRaisedFloorOffset) = A; }
  CYC(b_+O(14), b_+OE(17)); push_effect(gb, b_+OE(17));
  CYC(b_+O(335), b_+OE(338)); SET_BC(0x0500);
  CALL_C(b_+O(338), objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+OE(341));
  CYC(b_+O(341), b_+OE(342)); C = A;
  CYC(b_+O(342), b_+OE(343)); B = L;
  CYC(b_+O(343), b_+OE(346)); SET_HL(wActiveTilePos);
  CYC(b_+O(346), b_+OE(347)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(347), b_+OE(348)); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+O(348), b_+OE(350));
    goto update_active_tile;
  }
  CYC(b_+O(348), b_+OE(350));
  CYC(b_+O(350), b_+OE(351)); A = mem_rd(gb, HL);
  CYC(b_+O(351), b_+OE(352)); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+O(352), b_+OE(354));
    goto increment_standing_counter;
  }
  CYC(b_+O(352), b_+OE(354));

update_active_tile:
  CYC(b_+O(354), b_+OE(356)); L = GV(0x99, 0xb3);
  CYC(b_+O(356), b_+OE(357)); A = mem_rd(gb, HL);
  CYC(b_+O(357), b_+OE(358)); mem_wr(gb, HL, B);
  CYC(b_+O(358), b_+OE(359)); B = A;
  CYC(b_+O(359), b_+OE(360)); L = alu_inc8(gb, L);
  CYC(b_+O(360), b_+OE(361)); mem_wr(gb, HL, C);
  CYC(b_+O(361), b_+OE(362)); L = alu_inc8(gb, L);
  CYC(b_+O(362), b_+OE(364)); mem_wr(gb, HL, 0x00);

increment_standing_counter:
  CYC(b_+O(364), b_+OE(366)); L = GV(0x9b, 0xb5);
  CYC(b_+O(366), b_+OE(367)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(367), b_+OE(368)); L = alu_inc8(gb, L);
  CYC(b_+O(368), b_+OE(369)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(369), b_+OE(370)); mem_wr(gb, HL, A);
  CYC(b_+O(370), b_+OE(371)); A = C;
  CYC(b_+O(371), b_+OE(374)); SET_HL(SYM(tileTypesTable));
  CYC(b_+O(374), b_+OE(377));
  lookupCollisionTable_hook(gb);
  CYC(b_+O(17), b_+OE(20)); W8(wActiveTileType) = A;
  CYC(b_+O(20), b_+OE(21)); push_effect(gb, b_+OE(21));
  do { uint16_t jt_ = (common_code_jump_table(gb));
    if (!game_seasons && jt_ == b_+71) { goto raisable_floor; }
    else if (jt_ == b_+O(76)) { goto normal; }
    else if (jt_ == b_+O(87)) { goto puddle; }
    else if (jt_ == b_+O(107)) { goto stump; }
    else if (jt_ == b_+O(118)) { goto vines; }
    else if (jt_ == b_+O(126)) { goto not_swimming; }
    else if (jt_ == b_+O(131)) { goto cracked_floor; }
    else if (jt_ == b_+O(158)) { goto hole; }
    else if (jt_ == b_+O(207)) { goto ice; }
    else if (!game_seasons && jt_ == b_+221) { goto cracked_ice; }
    else if (game_seasons && jt_ == b_+S(200)) { goto cracked_ice; }
    else if (jt_ == b_+O(222)) { goto swimming; }
    else if ((!game_seasons && jt_ == b_+257) || (game_seasons && jt_ == b_+S(245))) { goto lava; }
    else if (jt_ == b_+O(288)) { goto conveyor; }
    else if (jt_ == b_+O(327)) { goto current; }
    else if (jt_ == SYM(dealSpikeDamageToLink) && hook_is(gb, SYM(dealSpikeDamageToLink), dealSpikeDamageToLink_hook)) { dealSpikeDamageToLink_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

raisable_floor:
  if (!game_seasons) {
    CYC(b_+71, b_+73); A = 0xfd;
    CYC(b_+73, b_+76); W8(wLinkRaisedFloorOffset) = A;
  }

normal:
  CYC(b_+O(76), b_+OE(77)); alu_xor(gb, A);
  CYC(b_+O(77), b_+OE(80)); W8(wActiveTileType) = A;
  CYC(b_+O(80), b_+OE(83)); W8(wStandingOnTileCounter) = A;
  CYC(b_+O(83), b_+OE(86)); W8(wLinkSwimmingState) = A;
  CYC(b_+O(86), b_+OE(87)); ret_effect(gb);
  return;

puddle:
  CYC(b_+O(87), b_+OE(88)); H = D;
  CYC(b_+O(88), b_+OE(90)); L = 0x21;
  CYC(b_+O(90), b_+OE(92)); alu_bit(gb, 5, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+O(92), b_+OE(94));
    goto normal;
  }
  CYC(b_+O(92), b_+OE(94));
  CYC(b_+O(94), b_+OE(96)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 5)));
  CYC(b_+O(96), b_+OE(99)); A = W8(wLinkImmobilized);
  CYC(b_+O(99), b_+OE(100)); alu_or(gb, A);
  CYC(b_+O(100), b_+OE(102)); A = 0x87;
  if (F & FZ) {
    CALL_C_CC(b_+O(102), playSound_b00_hook, SYM(playSound_b00), b_+OE(105));
  } else {
    CYC(b_+O(102), b_+OE(105));
  }
  CYC(b_+O(105), b_+OE(107));
  goto normal;

stump:
  CYC(b_+O(107), b_+OE(108)); H = D;
  CYC(b_+O(108), b_+OE(110)); L = 0x33;
  CYC(b_+O(110), b_+OE(112)); mem_wr(gb, HL, 0xff);
  CYC(b_+O(112), b_+OE(114)); L = 0x24;
  CYC(b_+O(114), b_+OE(116)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+O(116), b_+OE(118));
  goto not_swimming;

vines:
  CALL_C(b_+O(118), dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+OE(121));
  CYC(b_+O(121), b_+OE(123)); A = 0xff;
  CYC(b_+O(123), b_+OE(126)); W8(wLinkClimbingVine) = A;

not_swimming:
  CYC(b_+O(126), b_+OE(127)); alu_xor(gb, A);
  CYC(b_+O(127), b_+OE(130)); W8(wLinkSwimmingState) = A;
  CYC(b_+O(130), b_+OE(131)); ret_effect(gb);
  return;

cracked_floor:
  CYC(b_+O(131), b_+OE(133)); A = 0x22;
  CALL_C(b_+O(133), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(136));
  if (F & FZ) {
    CYCT(b_+O(136), b_+OE(138));
    goto normal;
  }
  CYC(b_+O(136), b_+OE(138));
  CYC(b_+O(138), b_+OE(141)); A = W8(wStandingOnTileCounter);
  CYC(b_+O(141), b_+OE(143)); alu_cp(gb, 0x20);
  if (F & FC) {
    CYCT(b_+O(143), b_+OE(145));
    goto not_swimming;
  }
  CYC(b_+O(143), b_+OE(145));
  CYC(b_+O(145), b_+OE(148)); A = W8(wActiveTilePos);
  CYC(b_+O(148), b_+OE(149)); C = A;
  CYC(b_+O(149), b_+OE(151)); A = 0xf3;
  CALL_C(b_+O(151), breakCrackedFloor_hook, SYM(breakCrackedFloor), b_+OE(154));
  CYC(b_+O(154), b_+OE(155)); alu_xor(gb, A);
  CYC(b_+O(155), b_+OE(158)); W8(wStandingOnTileCounter) = A;

hole:
  if (!game_seasons) {      // no holes in sidescrolling areas: Ages only
    CYC(b_+158, b_+161); A = W8(wTilesetFlags);
    CYC(b_+161, b_+163); alu_and(gb, 0x40);
    if (!(F & FZ)) {
      CYCT(b_+163, b_+165);
      goto normal;
    }
    CYC(b_+163, b_+165);
  }
  CYC(b_+O(165), b_+OE(166)); alu_xor(gb, A);
  CYC(b_+O(166), b_+OE(169)); W8(wLinkSwimmingState) = A;
  CYC(b_+O(169), b_+OE(172)); A = W8(wLinkRidingObject);
  CYC(b_+O(172), b_+OE(173)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(173), b_+OE(175));
    goto normal;
  }
  CYC(b_+O(173), b_+OE(175));
  CYC(b_+O(175), b_+OE(178)); A = W8(wMagnetGloveState);
  CYC(b_+O(178), b_+OE(180)); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+O(180), b_+OE(182));
    goto normal;
  }
  CYC(b_+O(180), b_+OE(182));
  CYC(b_+O(182), b_+OE(185)); SET_HL(wLastActiveTileType);
  CYC(b_+O(185), b_+OE(186)); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+O(186), b_+OE(187)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+O(187), b_+OE(189));
    goto pull_into_hole;
  }
  CYC(b_+O(187), b_+OE(189));
  CYC(b_+O(189), b_+OE(191)); L = GV(0x99, 0xb3);
  CYC(b_+O(191), b_+OE(192)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(192), b_+OE(193)); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+O(193), b_+OE(195));
    goto pull_into_hole;
  }
  CYC(b_+O(193), b_+OE(195));
  CYC(b_+O(195), b_+OE(196)); L = alu_inc8(gb, L);
  CYC(b_+O(196), b_+OE(198)); A = 0x0e;
  CYC(b_+O(198), b_+OE(199)); mem_wr(gb, HL, A);

pull_into_hole:
  CYC(b_+O(199), b_+OE(201)); A = 0x80;
  CYC(b_+O(201), b_+OE(204)); W8(wcc92) = A;
  CYC(b_+O(204), b_+OE(207));
  TAIL(linkPullIntoHole);

ice:
  CYC(b_+O(207), b_+OE(209)); A = 0x21;
  CALL_C(b_+O(209), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(212));
  if (F & FZ) {
    CYCT(b_+O(212), b_+OE(214));
    goto not_swimming;
  }
  CYC(b_+O(212), b_+OE(214));
  CYC(b_+O(214), b_+OE(217)); SET_HL(wIsTileSlippery);
  CYC(b_+O(217), b_+OE(219)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+O(219), b_+OE(221));
  goto not_swimming;

cracked_ice:
  if (!game_seasons) { CYC(b_+221, b_+222); ret_effect(gb); return; }
  CYC(b_+S(200), b_+S(203)); A = W8(wStandingOnTileCounter);     // Seasons: the ice breaks after a while
  CYC(b_+S(203), b_+S(205)); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(b_+S(205), b_+S(207)); goto ice; }
  CYC(b_+S(205), b_+S(207));
  CYC(b_+S(207), b_+S(210)); A = W8(wActiveTilePos);
  CYC(b_+S(210), b_+S(211)); C = A;
  CYC(b_+S(211), b_+S(213)); A = 0xfd;
  CALL_C(b_+S(213), setTile_hook, SYM(setTile), b_+S(216));

swimming:
  CYC(b_+O(222), b_+OE(225)); A = W8(wLinkRidingObject);
  CYC(b_+O(225), b_+OE(226)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(226), b_+OE(229));
    goto normal;
  }
  CYC(b_+O(226), b_+OE(229));
  CYC(b_+O(229), b_+OE(232)); A = W8(wLinkSwimmingState);
  CYC(b_+O(232), b_+OE(233)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(233), b_+OE(234)); ret_effect(gb); return;
  }
  CYC(b_+O(233), b_+OE(234));
  if (!game_seasons) {      // mermaid suit: Ages only
    CYC(b_+234, b_+237); A = mem_rd(gb, w1Link_var2f);
    CYC(b_+237, b_+239); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(b_+239, b_+240); ret_effect(gb); return;
    }
    CYC(b_+239, b_+240);
  }
  CYC(b_+O(240), b_+OE(241)); alu_xor(gb, A);
  CYC(b_+O(241), b_+OE(243)); E = 0x35;
  CYC(b_+O(243), b_+OE(244)); mem_wr(gb, DE, A);
  CYC(b_+O(244), b_+OE(246)); E = 0x2d;
  CYC(b_+O(246), b_+OE(247)); mem_wr(gb, DE, A);
  CYC(b_+O(247), b_+OE(248)); A = alu_inc8(gb, A);
  CYC(b_+O(248), b_+OE(251)); W8(wLinkSwimmingState) = A;
  CYC(b_+O(251), b_+OE(253)); A = 0x80;
  CYC(b_+O(253), b_+OE(256)); W8(wcc92) = A;
  CYC(b_+O(256), b_+OE(257)); ret_effect(gb);
  return;

lava:
  if (!game_seasons) {
    CYC(b_+257, b_+260); A = W8(wLinkRidingObject);
    CYC(b_+260, b_+261); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+261, b_+264);
      goto normal;
    }
    CYC(b_+261, b_+264);
  } else {                  // the magnet gloves hold Link above lava
    CYC(b_+S(245), b_+S(248)); A = W8(wMagnetGloveState);
    CYC(b_+S(248), b_+S(250)); alu_bit(gb, 6, A);
    if (!(F & FZ)) {
      CYCT(b_+S(250), b_+S(253));
      goto normal;
    }
    CYC(b_+S(250), b_+S(253));
  }
  CYC(b_+O(264), b_+OE(266)); A = 0x80;
  CYC(b_+O(266), b_+OE(269)); W8(wcc92) = A;
  CYC(b_+O(269), b_+OE(271)); E = 0x2d;
  CYC(b_+O(271), b_+OE(272)); alu_xor(gb, A);
  CYC(b_+O(272), b_+OE(273)); mem_wr(gb, DE, A);
  CYC(b_+O(273), b_+OE(276)); A = W8(wLinkSwimmingState);
  CYC(b_+O(276), b_+OE(277)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(277), b_+OE(278)); ret_effect(gb); return;
  }
  CYC(b_+O(277), b_+OE(278));
  CYC(b_+O(278), b_+OE(279)); alu_xor(gb, A);
  CYC(b_+O(279), b_+OE(281)); E = 0x35;
  CYC(b_+O(281), b_+OE(282)); mem_wr(gb, DE, A);
  CYC(b_+O(282), b_+OE(284)); A = 0x41;
  CYC(b_+O(284), b_+OE(287)); W8(wLinkSwimmingState) = A;
  CYC(b_+O(287), b_+OE(288)); ret_effect(gb);
  return;

conveyor:
  CYC(b_+O(288), b_+OE(291)); A = W8(wLinkRidingObject);
  CYC(b_+O(291), b_+OE(292)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(292), b_+OE(295));
    goto normal;
  }
  CYC(b_+O(292), b_+OE(295));
  CYC(b_+O(295), b_+OE(297)); A = 0x23;
  CALL_C(b_+O(297), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(300));
  if (F & FZ) {
    CYCT(b_+O(300), b_+OE(303));
    goto normal;
  }
  CYC(b_+O(300), b_+OE(303));
  CYC(b_+O(303), b_+OE(306)); SET_BC(0x1409);
  link_apply_tile_types_adjust_conveyor(gb);
  return;

current:
  CYC(b_+O(327), b_+OE(330)); SET_BC(0x1e12);
  CYC(b_+O(330), b_+OE(333)); push_effect(gb, b_+OE(333));
  link_apply_tile_types_adjust_conveyor(gb);
  CYC(b_+O(333), b_+OE(335));
  goto swimming;
}

void linkAdjustAngleInSidescrollingArea_hook(GB *gb) {
  BASE(linkAdjustAngleInSidescrollingArea);
  CYC(b_+0, b_+2); L = 0x09;
  TAIL(linkAdjustGivenAngleInSidescrollingArea);
}

void linkAdjustGivenAngleInSidescrollingArea_hook(GB *gb) {
  BASE(linkAdjustGivenAngleInSidescrollingArea);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); E = L;
  CYC(b_+2, b_+5); A = W8(wTilesetFlags);
  CYC(b_+5, b_+7); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+7, b_+8); ret_effect(gb); return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+15); SET_HL(b_+19);
  CYC(b_+15, b_+16); common_code_add_a_to_hl(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void companionPreventLinkFromPassing_noExtraChecks_hook(GB *gb) {
  BASE(companionPreventLinkFromPassing_noExtraChecks);
  CYC(b_+0, b_+3); SET_HL(w1Link);
  CYC(b_+3, b_+6);
  TAIL(preventObjectHFromPassingObjectD);
}

void companionUpdateMovement_hook(GB *gb) {
  BASE(companionUpdateMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCalculateAdjacentWallsBitset_hook, SYM(companionCalculateAdjacentWallsBitset), b_+3);
  CALL_C(b_+3, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x0f;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  TAIL(companionTryToBreakTileFromMoving);
}

void companionTryToBreakTileFromMoving_hook(GB *gb) {
  BASE(companionTryToBreakTileFromMoving);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0b;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_add(gb, 0x05);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+9); L = 0x0d;
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CYC(b_+10, b_+12); A = 0x13;
  CYC(b_+12, b_+15);
  TAIL(tryToBreakTile);
}

void companionCalculateAdjacentWallsBitset_hook(GB *gb) {
  BASE(companionCalculateAdjacentWallsBitset);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x33;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = 0x0b;
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CYC(b_+8, b_+10); L = 0x0d;
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+15); H8(hFF8B) = A;
  CYC(b_+15, b_+18); SET_HL(b_+40);

  do {
    CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+19, b_+20); alu_add(gb, B);
    CYC(b_+20, b_+21); B = A;
    CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+22, b_+23); alu_add(gb, C);
    CYC(b_+23, b_+24); C = A;
    CYC(b_+24, b_+25); push_effect(gb, HL);
    CALL_C(b_+25, checkCollisionForCompanion_hook, SYM(checkCollisionForCompanion), b_+28);
    CYC(b_+28, b_+29); SET_HL(pop_effect(gb));
    CYC(b_+29, b_+31); A = H8(hFF8B);
    CYC(b_+31, b_+32); alu_rla(gb);
    CYC(b_+32, b_+34); H8(hFF8B) = A;
    if (!(F & FC)) CYCT(b_+34, b_+36);
    else CYC(b_+34, b_+36);
  } while (!(F & FC));

  CYC(b_+36, b_+38); E = 0x33;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); ret_effect(gb);
}

void checkCollisionForCompanion_hook(GB *gb) {
  BASE(checkCollisionForCompanion);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getTileAtPosition_hook, SYM(getTileAtPosition), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0xd6);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    goto set_collision;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); alu_cp(gb, 0xd5);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto set_collision;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_cp(gb, 0xd4);
  CYC(b_+14, b_+16); A = 0x03;
  if (F & FZ) {
    CYCT(b_+16, b_+19);
    TAIL(checkGivenCollision_allowHoles);
  }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); E = 0x01;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0b);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+26);
    goto not_ricky;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); E = 0x0f;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+31); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
    goto check_collision;
  }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36);
  goto check_collision;

not_ricky:
  CYC(b_+36, b_+38); alu_cp(gb, 0x0c);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+40);
    goto check_collision;
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+43); alu_cp(gb, 0xfe);
  if (!(F & FC)) {
    CYCT(b_+43, b_+44); ret_effect(gb); return;
  }
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+46);
  goto check_collision;

set_collision:
  CYC(b_+46, b_+47); alu_scf(gb);
  CYC(b_+47, b_+48); ret_effect(gb);
  return;

check_collision:
  CYC(b_+48, b_+51);
  TAIL(checkCollisionPosition_disallowSmallBridges);
}

void specialObjectGetRelativeTileWithDirectionTable_hook(GB *gb) {
  BASE(specialObjectGetRelativeTileWithDirectionTable);
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); common_code_add_double_index(gb, SYM(specialObjectGetRelativeTileFromHl));
  TAIL(specialObjectGetRelativeTileFromHl);
}

void specialObjectGetRelativeTileFromHl_hook(GB *gb) {
  BASE(specialObjectGetRelativeTileFromHl);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); B = A;
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CALL_C(b_+3, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+6);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+9); H = wRoomCollisions >> 8;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void specialObjectCheckMovingAwayFromWall_hook(GB *gb) {
  BASE(specialObjectCheckMovingAwayFromWall);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x09;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); alu_add(gb, 0x10);
  CYC(b_+9, b_+11); alu_and(gb, 0x1f);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CALL_C(b_+12, specialObjectCheckFacingWall_hook, SYM(specialObjectCheckFacingWall), b_+15);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+18); L = 0x09;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_add(gb, 0x10);
  CYC(b_+21, b_+23); alu_and(gb, 0x1f);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+25); A = C;
  CYC(b_+25, b_+26); alu_or(gb, A);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void specialObjectCheckMovingTowardWall_hook(GB *gb) {
  BASE(specialObjectCheckMovingTowardWall);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x09;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  TAIL(specialObjectCheckFacingWall);
}

void specialObjectCheckFacingWall_hook(GB *gb) {
  BASE(specialObjectCheckFacingWall);
  CYC(b_+0, b_+3); SET_BC(0x0000);
  CYC(b_+3, b_+5); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto check_vertical;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x18);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto check_vertical;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); L = 0x33;
  CYC(b_+13, b_+14); B = mem_rd(gb, HL);
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+19); alu_and(gb, 0x03);
  CYC(b_+19, b_+21); A = 0x30;
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
  } else {
    CYC(b_+21, b_+23);
    CYC(b_+23, b_+25); A = 0xc0;
  }
  CYC(b_+25, b_+26); alu_and(gb, B);
  CYC(b_+26, b_+27); B = A;

check_vertical:
  CYC(b_+27, b_+29); L = 0x09;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_and(gb, 0x0f);
  if (F & FZ) {
    CYCT(b_+32, b_+34);
    goto done;
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); L = 0x33;
  CYC(b_+37, b_+38); C = mem_rd(gb, HL);
  CYC(b_+38, b_+40); alu_bit(gb, 4, A);
  CYC(b_+40, b_+42); A = 0x03;
  if (F & FZ) {
    CYCT(b_+42, b_+44);
  } else {
    CYC(b_+42, b_+44);
    CYC(b_+44, b_+46); A = 0x0c;
  }
  CYC(b_+46, b_+47); alu_and(gb, C);
  CYC(b_+47, b_+48); C = A;

done:
  CYC(b_+48, b_+49); A = B;
  CYC(b_+49, b_+50); alu_or(gb, C);
  CYC(b_+50, b_+51); ret_effect(gb);
}

void companionCreateItem_hook(GB *gb) {
  BASE(companionCreateWeaponItem);
  uint16_t sp0_ = gb->sp;
  CALL_C(SYM(companionCreateItem), getFreeItemSlot_hook, SYM(getFreeItemSlot), (SYM(companionCreateItem) + 3));
  if (!(F & FZ)) {
    CYCT((SYM(companionCreateItem) + 3), (SYM(companionCreateItem) + 4)); ret_effect(gb); return;
  }
  CYC((SYM(companionCreateItem) + 3), (SYM(companionCreateItem) + 4));
  CYC((SYM(companionCreateItem) + 4), (SYM(companionCreateItem) + 6));
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, C);
  CYC(b_+11, b_+13); L = 0x28;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf9);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void companionCreateWeaponItem_hook(GB *gb) {
  BASE(companionCreateWeaponItem);
  CYC(b_+0, b_+3); SET_HL(w1WeaponItem_enabled);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, C);
  CYC(b_+11, b_+13); L = 0x28;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf9);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void companionUpdateDirectionAndAnimate_hook(GB *gb) {
  BASE(companionUpdateDirectionAndAnimate);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); mem_wr(gb, w1Link_direction, A);
  CYC(b_+6, b_+8); E = 0x04;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+11, b_+14); TAIL(specialObjectAnimate);
  }
  CYC(b_+11, b_+14);
  CALL_C(b_+14, updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+17);
  CYC(b_+17, b_+20); SET_HL(w1Companion_direction);
  CYC(b_+20, b_+21); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+21, b_+24); TAIL(specialObjectAnimate);
  }
  CYC(b_+21, b_+24);
  TAIL(companionUpdateDirectionAndSetAnimation);
}

void companionUpdateDirectionAndSetAnimation_hook(GB *gb) {
  BASE(companionUpdateDirectionAndSetAnimation);
  CYC(b_+0, b_+2); E = 0x09;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+9); E = alu_dec8(gb, E);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  TAIL(companionSetAnimation);
}

void companionSetAnimation_hook(GB *gb) {
  BASE(companionSetAnimation);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); A = C;
  CYC(b_+2, b_+4); L = 0x08;
  CYC(b_+4, b_+5); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+5, b_+7); L = 0x38;
  CYC(b_+7, b_+8); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+11);
  TAIL(specialObjectSetAnimation);
}

void companionTryToMount_hook(GB *gb) {
  BASE(companionTryToMount);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wActiveTileType);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto cant_mount;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wDisallowMountingCompanion);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    goto cant_mount;
  }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, checkLinkVulnerableAndIDZero_hook, SYM(checkLinkVulnerableAndIDZero), b_+16);
  if (F & FC) {
    CYCT(b_+16, b_+18);
    goto try_mounting;
  }
  CYC(b_+16, b_+18);

cant_mount:
  CYC(b_+18, b_+19); alu_or(gb, D);
  CYC(b_+19, b_+20); ret_effect(gb);
  return;

try_mounting:
  CYC(b_+20, b_+23); A = W8(w1Link_state);
  CYC(b_+23, b_+25); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+26); ret_effect(gb); return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); A = W8(wLinkSwimmingState);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+31); ret_effect(gb); return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); A = W8(wLinkGrabState);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+35, b_+36); ret_effect(gb); return;
  }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+39); A = W8(wLinkInAir);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+40, b_+41); ret_effect(gb); return;
  }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); A = alu_inc8(gb, A);
  CYC(b_+42, b_+45); W8(wDisableWarpTiles) = A;
  CYC(b_+45, b_+48); W8(wWarpsDisabled) = A;
  CYC(b_+48, b_+50); E = 0x04;
  CYC(b_+50, b_+52); A = 0x03;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); A = 0xff;
  TAIL(setLinkMountingSpeed);
}

void setLinkMountingSpeed_hook(GB *gb) {
  BASE(setLinkMountingSpeed);
  CYC(b_+0, b_+3); W8(wLinkAngle) = A;
  CYC(b_+3, b_+5); A = 0x81;
  CYC(b_+5, b_+8); W8(wLinkInAir) = A;
  CYC(b_+8, b_+11); W8(wDisableScreenTransitions) = A;
  CYC(b_+11, b_+14); SET_HL(w1Link_angle);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = 0x10;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x14);
  CYC(b_+19, b_+21); L = 0x14;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x40);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xfe);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void companionCheckHazards_hook(GB *gb) {
  BASE(companionCheckHazards);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+3);
  CYC(b_+3, b_+4); H = D;
  if (!(F & FC)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  TAIL(companionGotoHazardHandlingState);
}

void companionGotoHazardHandlingState_hook(GB *gb) {
  BASE(companionGotoHazardHandlingState);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+3); L = 0x04;
  CYC(b_+3, b_+5); A = 0x04;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+11); L = 0x01;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+14, b_+16);
  } else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+19); W8(wDisableScreenTransitions) = A;
    CYC(b_+19, b_+21); A = 0x87;
    CALL_C(b_+21, playSound_b00_hook, SYM(playSound_b00), b_+24);
  }
  CYC(b_+24, b_+25); SET_AF(pop_effect(gb));
  CYC(b_+25, b_+26); alu_scf(gb);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void companionDismountAndSavePosition_hook(GB *gb) {
  BASE(companionDismountAndSavePosition);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionDismount_hook, SYM(companionDismount), b_+3);
  CYC(b_+3, b_+5); E = 0x01;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+9); SET_HL(wAnimalCompanion);
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto normal_dismount;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_cp(gb, 0x0b);
  if (F & FZ) {
    CYCT(b_+14, b_+16);
    CYC(b_+22, b_+24);
    goto normal_dismount;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+18, b_+20);
    CYC(b_+24, b_+26);
    goto normal_dismount;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22);

normal_dismount:
  CYC(b_+34, b_+36);
  TAIL(saveLinkLocalRespawnAndCompanionPosition);
}

void companionDismount_hook(GB *gb) {
  BASE(companionDismount);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, setLinkID_hook, SYM(setLinkID), b_+4);
  CYC(b_+4, b_+7); SET_HL(w1Link_oamFlagsBackup);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); L = 0x25;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+18); L = 0x2b;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = 0x3c;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+27); W8(wLinkForceState) = A;
  CYC(b_+27, b_+30); W8(wcc50) = A;
  CYC(b_+30, b_+32); L = 0x00;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x01);
  CYC(b_+34, b_+36); L = 0x08;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+37, b_+39); A = alu_swap(gb, A);
  CYC(b_+39, b_+41); A = alu_srl(gb, A);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);
  CALL_C(b_+42, setLinkMountingSpeed_hook, SYM(setLinkMountingSpeed), b_+45);
  CYC(b_+45, b_+48); SET_HL(w1Link_angle);
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0xff);
  CALL_C(b_+50, objectCopyPosition_hook, SYM(objectCopyPosition), b_+53);
  CYC(b_+53, b_+54); L = alu_dec8(gb, L);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0xf8);
  CYC(b_+56, b_+57); A = H;
  CYC(b_+57, b_+60); W8(wLinkObjectIndex) = A;
  CYC(b_+60, b_+61); alu_xor(gb, A);
  CYC(b_+61, b_+64); W8(wDisableWarpTiles) = A;
  CYC(b_+64, b_+67); W8(wWarpsDisabled) = A;
  CYC(b_+67, b_+70); W8(wForceCompanionDismount) = A;
  CYC(b_+70, b_+73); W8(wDisableScreenTransitions) = A;
  CYC(b_+73, b_+76);
  TAIL(setCameraFocusedObjectToLink);
}

void saveLinkLocalRespawnAndCompanionPosition_hook(GB *gb) {
  BASE(saveLinkLocalRespawnAndCompanionPosition);
  CYC(b_+0, b_+3); SET_HL(wRememberedCompanionId);
  CYC(b_+3, b_+6); A = W8(w1Companion_id);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+10); A = W8(wActiveGroup);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+14); A = W8(wActiveRoom);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+18); A = W8(w1Companion_direction);
  CYC(b_+18, b_+21); W8(wLinkLocalRespawnDir) = A;
  CYC(b_+21, b_+24); A = W8(w1Companion_yh);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+28); W8(wLinkLocalRespawnY) = A;
  CYC(b_+28, b_+31); A = W8(w1Companion_xh);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+32, b_+35); W8(wLinkLocalRespawnX) = A;
  CYC(b_+35, b_+36); ret_effect(gb);
}

void companionDragToCenterOfHole_hook(GB *gb) {
  BASE(companionDragToCenterOfHole);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x3d;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+7); alu_xor(gb, A);
    CYC(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+11); SET_BC(0x0500);
  CALL_C(b_+11, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+14);
  CYC(b_+14, b_+15); C = L;
  CALL_C(b_+15, convertShortToLongPosition_paramC_hook, SYM(convertShortToLongPosition_paramC), b_+18);
  CYC(b_+18, b_+20); E = 0x0d;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_cp(gb, C);
  CYC(b_+22, b_+24); C = 0x00;
  if (F & FZ) {
    CYCT(b_+24, b_+26);
  } else {
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+29); SET_HL(0x0040);
    if (F & FC) {
      CYCT(b_+29, b_+31);
    } else {
      CYC(b_+29, b_+31);
      CYC(b_+31, b_+34); SET_HL(0xffc0);
    }
    CYC(b_+34, b_+35); E = alu_dec8(gb, E);
    CYC(b_+35, b_+36); A = mem_rd(gb, DE);
    CYC(b_+36, b_+37); alu_add(gb, L);
    CYC(b_+37, b_+38); mem_wr(gb, DE, A);
    CYC(b_+38, b_+39); E = alu_inc8(gb, E);
    CYC(b_+39, b_+40); A = mem_rd(gb, DE);
    CYC(b_+40, b_+41); alu_adc(gb, H);
    CYC(b_+41, b_+42); mem_wr(gb, DE, A);
    CYC(b_+42, b_+43); C = alu_dec8(gb, C);
  }
  CYC(b_+43, b_+45); E = 0x0b;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
  } else {
    CYC(b_+47, b_+49);
    CYC(b_+49, b_+52); SET_HL(0x0040);
    if (F & FC) {
      CYCT(b_+52, b_+54);
    } else {
      CYC(b_+52, b_+54);
      CYC(b_+54, b_+57); SET_HL(0xffc0);
    }
    CYC(b_+57, b_+58); E = alu_dec8(gb, E);
    CYC(b_+58, b_+59); A = mem_rd(gb, DE);
    CYC(b_+59, b_+60); alu_add(gb, L);
    CYC(b_+60, b_+61); mem_wr(gb, DE, A);
    CYC(b_+61, b_+62); E = alu_inc8(gb, E);
    CYC(b_+62, b_+63); A = mem_rd(gb, DE);
    CYC(b_+63, b_+64); alu_adc(gb, H);
    CYC(b_+64, b_+65); mem_wr(gb, DE, A);
    CYC(b_+65, b_+66); C = alu_dec8(gb, C);
  }
  CYC(b_+66, b_+67); H = D;
  CYC(b_+67, b_+68); A = C;
  CYC(b_+68, b_+69); alu_or(gb, A);
  CYC(b_+69, b_+70); ret_effect(gb);
}

void companionRespawn_hook(GB *gb) {
  BASE(companionRespawn);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wDisableScreenTransitions) = A;
  CYC(b_+4, b_+7); W8(wLinkForceState) = A;
  CYC(b_+7, b_+10); W8(wcc50) = A;
  CALL_C(b_+10, specialObjectSetCoordinatesToRespawnYX_hook, SYM(specialObjectSetCoordinatesToRespawnYX), b_+13);
  CALL_C(b_+13, objectCheckSimpleCollision_hook, SYM(objectCheckSimpleCollision), b_+16);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto invalid_position;
  }
  CYC(b_+16, b_+18);
  CALL_C(b_+18, objectGetPosition_hook, SYM(objectGetPosition), b_+21);
  CALL_C(b_+21, checkCollisionForCompanion_hook, SYM(checkCollisionForCompanion), b_+24);
  if (F & FC) {
    CYCT(b_+24, b_+26);
    goto invalid_position;
  }
  CYC(b_+24, b_+26);
  CALL_C(b_+26, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+29);
  if (!(F & FC)) {
    CYCT(b_+29, b_+31);
    goto apply_damage_and_set_state;
  }
  CYC(b_+29, b_+31);

invalid_position:
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+34); L = 0x0b;
  CYC(b_+34, b_+37); A = W8(wLastAnimalMountPointY);
  CYC(b_+37, b_+40); W8(wLinkLocalRespawnY) = A;
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+45); A = W8(wLastAnimalMountPointX);
  CYC(b_+45, b_+48); W8(wLinkLocalRespawnX) = A;
  CYC(b_+48, b_+49); mem_wr(gb, HL, A); SET_HL(HL + 1);

apply_damage_and_set_state:
  CYC(b_+49, b_+52); A = W8(wLinkObjectIndex);
  CYC(b_+52, b_+53); alu_rrca(gb);
  CYC(b_+53, b_+55); A = 0x01;
  if (!(F & FC)) {
    CYCT(b_+55, b_+57);
    goto set_state;
  }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); A = 0xfe;
  CYC(b_+59, b_+62); W8(w1Link_damageToApply) = A;
  CYC(b_+62, b_+64); A = 0x40;
  CYC(b_+64, b_+67); W8(w1Link_invincibilityCounter) = A;
  CYC(b_+67, b_+69); A = 0x05;

set_state:
  CYC(b_+69, b_+70); H = D;
  CYC(b_+70, b_+72); L = 0x04;
  CYC(b_+72, b_+73); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+73, b_+74); alu_xor(gb, A);
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+77); L = 0x3d;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+81); W8(wDisableScreenTransitions) = A;
  CYC(b_+81, b_+83); L = 0x24;
  CYC(b_+83, b_+85); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+85, b_+86); ret_effect(gb);
}

void companionCheckHopDownCliff_hook(GB *gb) {
  BASE(companionCheckHopDownCliff);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkAngle);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); alu_and(gb, 0xe7);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); E = 0x09;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, specialObjectCheckMovingTowardWall_hook, SYM(specialObjectCheckMovingTowardWall), b_+15);
  CYC(b_+15, b_+17); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
    goto check_tile;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+21, b_+23);
    goto check_tile;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); alu_cp(gb, 0x30);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+26); ret_effect(gb); return;
  }
  CYC(b_+25, b_+26);

check_tile:
  CYC(b_+26, b_+28); E = 0x08;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+32); SET_HL(b_+90);
  CYC(b_+32, b_+33); common_code_add_double_index(gb, b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+35); B = A;
  CYC(b_+35, b_+36); C = mem_rd(gb, HL);
  CALL_C(b_+36, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+39);
  CYC(b_+39, b_+41); alu_cp(gb, GV(0xd4, 0xdd));
  if (F & FZ) {
    CYCT(b_+41, b_+43);
    CYC(b_+53, b_+55); A = 0x10;
    goto cliff_tile;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); SET_HL(SYM(cliffTilesTable));
  CALL_C(b_+46, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+49);
  if (F & FC) {
    CYCT(b_+49, b_+51);
    goto cliff_tile;
  }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); alu_or(gb, D);
  CYC(b_+52, b_+53); ret_effect(gb);
  return;

cliff_tile:
  CYC(b_+55, b_+56); H = D;
  CYC(b_+56, b_+58); L = 0x09;
  CYC(b_+58, b_+59); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+59, b_+60); ret_effect(gb); return;
  }
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+62); A = 0x80;
  CYC(b_+62, b_+65); W8(wLinkInAir) = A;
  CYC(b_+65, b_+68); SET_BC(0xfd40);
  CALL_C(b_+68, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+71);
  CYC(b_+71, b_+73); L = 0x10;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x50);
  CYC(b_+75, b_+77); L = 0x06;
  CYC(b_+77, b_+79); A = 0x14;
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+80, b_+81); alu_xor(gb, A);
  CYC(b_+81, b_+82); mem_wr(gb, HL, A);
  CYC(b_+82, b_+84); L = 0x04;
  CYC(b_+84, b_+86); A = 0x07;
  CYC(b_+86, b_+87); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+87, b_+88); alu_xor(gb, A);
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);
  CYC(b_+89, b_+90); ret_effect(gb);
}

void companionFinalizeMounting_hook(GB *gb) {
  BASE(companionFinalizeMounting);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x00;
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+5, b_+7); L = 0x04;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x05);
  CYC(b_+9, b_+11); L = 0x09;
  CYC(b_+11, b_+13); A = 0xff;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = 0x3c;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = 0x1a;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_and(gb, 0xc0);
  CYC(b_+22, b_+24); alu_or(gb, 0x01);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+28); L = 0x3d;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+32); W8(wLinkInAir) = A;
  CYC(b_+32, b_+35); W8(wDisableScreenTransitions) = A;
  CYC(b_+35, b_+38); SET_BC(wLastAnimalMountPointY);
  CYC(b_+38, b_+40); L = 0x0b;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); mem_wr(gb, BC, A);
  CYC(b_+42, b_+43); C = alu_inc8(gb, C);
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); mem_wr(gb, BC, A);
  CYC(b_+46, b_+47); A = D;
  CYC(b_+47, b_+50); W8(wLinkObjectIndex) = A;
  CALL_C(b_+50, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+53);
  CYC(b_+53, b_+55); A = 0x09;
  CYC(b_+55, b_+58);
  TAIL(setLinkID);
}

void companionFunc_47d8_hook(GB *gb) {
  BASE(companionFunc_47d8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x3c;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wLinkDeathTrigger);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); E = 0x0e;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+19); L = 0x04;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x09);
  CYC(b_+21, b_+23); E = 0x1b;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); E = 0x1a;
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); H = 0xd0;
  CALL_C(b_+31, objectCopyPosition_hook, SYM(objectCopyPosition), b_+34);
  CYC(b_+34, b_+35); A = H;
  CYC(b_+35, b_+38); W8(wLinkObjectIndex) = A;
  CALL_C(b_+38, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+41);
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CALL_C(b_+42, setLinkID_hook, SYM(setLinkID), b_+45);
  CYC(b_+45, b_+46); alu_or(gb, D);
  CYC(b_+46, b_+47); ret_effect(gb);
}

static void companion_set_state_and_clear_substate(GB *gb) {
  BASE(companionSetAnimationAndGotoState5);
  CYC(b_+5, b_+7); E = 0x04;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); E = alu_inc8(gb, E);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void companionGotoDismountState_hook(GB *gb) {
  BASE(companionGotoDismountState);
  CYC(b_+0, b_+2); E = 0x38;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    CYC(b_+11, b_+13); A = 0x06;
    CYC(b_+13, b_+15);
    companion_set_state_and_clear_substate(gb);
    return;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+10); W8(wForceCompanionDismount) = A;
  CYC(b_+10, b_+11); ret_effect(gb);
}

void companionSetAnimationAndGotoState5_hook(GB *gb) {
  BASE(companionSetAnimationAndGotoState5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionSetAnimation_hook, SYM(companionSetAnimation), b_+3);
  CYC(b_+3, b_+5); A = 0x05;
  companion_set_state_and_clear_substate(gb);
}

void companionCheckCanSpawn_hook(GB *gb) {
  BASE(companionCheckCanSpawn);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto can_spawn;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
  } else {
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); A = alu_inc8(gb, A);
    CYC(b_+12, b_+13); mem_wr(gb, DE, A);
    CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
    CYC(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+20);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+23); A = 0x02;
  CYC(b_+23, b_+25); hram_wr(gb, R_SVBK, A);
  CYC(b_+25, b_+26); A = B;
  CYC(b_+26, b_+29); SET_HL(w2SolidObjectPositions);
  CALL_C(b_+29, checkFlag_hook, SYM(checkFlag), b_+32);
  CYC(b_+32, b_+34); A = 0x00;
  CYC(b_+34, b_+36); hram_wr(gb, R_SVBK, A);
  if (F & FZ) {
    CYCT(b_+36, b_+38);
  } else {
    CYC(b_+36, b_+38);
    CYC(b_+38, b_+39); SET_AF(pop_effect(gb));
    CYC(b_+39, b_+42);
    TAIL(itemDelete);
  }
  CYC(b_+42, b_+44); E = 0x0b;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); alu_add(gb, 0x05);
  CYC(b_+47, b_+48); B = A;
  CYC(b_+48, b_+50); E = 0x0d;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); C = A;
  CALL_C(b_+52, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+55);
  CYC(b_+55, b_+57); alu_cp(gb, 0x10);
  if (F & FZ) {
    CYCT(b_+57, b_+59);
    goto try_last_mount_point;
  }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+61, b_+63);
    goto can_spawn;
  }
  CYC(b_+61, b_+63);

try_last_mount_point:
  CYC(b_+63, b_+66); SET_HL(wLastAnimalMountPointY);
  CYC(b_+66, b_+67); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+67, b_+69); E = 0x0b;
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+73); E = 0x0d;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CALL_C(b_+74, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+77);
  if (F & FZ) {
    CYCT(b_+77, b_+79);
    goto can_spawn;
  }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); SET_AF(pop_effect(gb));
  CYC(b_+80, b_+83);
  TAIL(itemDelete);

can_spawn:
  CALL_C(b_+83, specialObjectSetOamVariables_hook, SYM(specialObjectSetOamVariables), b_+86);
  CYC(b_+86, b_+89); SET_HL(w1Companion_var03);
  CYC(b_+89, b_+90); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+90, b_+91); A = alu_inc8(gb, A);
  CYC(b_+91, b_+92); mem_wr(gb, HL, A);
  CYC(b_+92, b_+94); L = 0x24;
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x80);
  CYC(b_+96, b_+97); ret_effect(gb);
}

void companionRetIfInactive_hook(GB *gb) {
  BASE(companionRetIfInactive);
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wTextIsActive);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11);
    companion_ret_if_inactive_return_from_caller(gb);
    return;
  }
  CYC(b_+9, b_+11);
  TAIL(companionRetIfInactiveWithoutStateCheck);
}

void companionRetIfInactiveWithoutStateCheck_hook(GB *gb) {
  BASE(companionRetIfInactiveWithoutStateCheck);
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    companion_ret_if_inactive_return_from_caller(gb);
    return;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wPaletteThread_mode);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    companion_ret_if_inactive_return_from_caller(gb);
    return;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = W8(wDisabledObjects);
  CYC(b_+16, b_+18); alu_and(gb, 0xa0);
  if (F & FZ) {
    CYCT(b_+18, b_+19); ret_effect(gb); return;
  }
  CYC(b_+18, b_+19);
  companion_ret_if_inactive_return_from_caller(gb);
}

void companionSetAnimationToVar3f_hook(GB *gb) {
  BASE(companionSetAnimationToVar3f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x3f;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = 0x30;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+7, b_+10);
    TAIL(specialObjectSetAnimation);
  }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void companionFlashFromChargingAnimation_hook(GB *gb) {
  BASE(companionFlashFromChargingAnimation);
  CYC(b_+0, b_+3); SET_HL(w1Link_oamFlagsBackup);
  CYC(b_+3, b_+6); A = W8(wFrameCounter);
  CYC(b_+6, b_+8); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+17, b_+18); mem_wr(gb, HL, A);
    CYC(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+13); alu_and(gb, 0xf8);
  CYC(b_+13, b_+14); alu_or(gb, C);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}

// companionCheckMountingComplete@nudgeLinkTowardCompanion: steps the coordinate at hl toward
// the companion, direction from the carry the caller's compare left.
static void companion_nudge_link_toward_companion(GB *gb) {
  BASE(companionCheckMountingComplete__nudgeLinkTowardCompanion);
  if (F & FC) {
    CYCT(b_+0, b_+2);
    CYC(b_+4, b_+5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+4); ret_effect(gb);
}

void companionCheckMountingComplete_hook(GB *gb) {
  BASE(companionCheckMountingComplete);
  CYC(b_+0, b_+3); A = W8(wDisallowMountingCompanion);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto stop_mounting;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(w1Link_state);
  CYC(b_+9, b_+11); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    goto stop_mounting;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = W8(wLinkGrabState);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
    goto continue_mounting;
  }
  CYC(b_+17, b_+19);

stop_mounting:
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+23); W8(wDisableWarpTiles) = A;
  CYC(b_+23, b_+26); W8(wWarpsDisabled) = A;
  CYC(b_+26, b_+29); W8(wDisableScreenTransitions) = A;
  CYC(b_+29, b_+31); A = 0x01;
  CYC(b_+31, b_+33); E = 0x04;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); alu_or(gb, D);
  CYC(b_+35, b_+36); ret_effect(gb);
  return;

continue_mounting:
  CYC(b_+36, b_+39); SET_HL(w1Link_yh);
  CYC(b_+39, b_+41); E = 0x0b;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+43, b_+46); push_effect(gb, b_+46);
    companion_nudge_link_toward_companion(gb);
  } else {
    CYC(b_+43, b_+46);
  }
  CYC(b_+46, b_+48); E = 0x0d;
  CYC(b_+48, b_+49); L = E;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+51, b_+54); push_effect(gb, b_+54);
    companion_nudge_link_toward_companion(gb);
  } else {
    CYC(b_+51, b_+54);
  }
  CYC(b_+54, b_+56); L = 0x15;
  CYC(b_+56, b_+58); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+58, b_+59); ret_effect(gb); return; }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+61); L = 0x0f;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_cp(gb, 0xfc);
  if (F & FC) { CYCT(b_+64, b_+65); ret_effect(gb); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+66); alu_xor(gb, A);
  CYC(b_+66, b_+67); ret_effect(gb);
}

void companionCheckEnableTerrainEffects_hook(GB *gb) {
  BASE(companionCheckEnableTerrainEffects);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x00;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = 0x3c;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+12); W8(wWarpsDisabled) = A;
  CYC(b_+12, b_+14); L = 0x0f;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+17); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19);
    goto enable_terrain_effects;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); SET_BC(0x0500);
  CALL_C(b_+22, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+25);
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); alu_cp(gb, 0xf9);
  if (!(F & FZ)) {
    CYCT(b_+28, b_+30);
    CYC(b_+35, b_+37); L = 0x0f;
    CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);
    goto enable_terrain_effects;
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); L = 0x1a;
  CYC(b_+32, b_+34); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+34, b_+35); ret_effect(gb);
  return;

enable_terrain_effects:
  CYC(b_+39, b_+41); L = 0x1a;
  CYC(b_+41, b_+43); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+43, b_+44); ret_effect(gb);
}

void companionSetPriorityRelativeToLink_hook(GB *gb) {
  BASE(companionSetPriorityRelativeToLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+3);
  CYC(b_+3, b_+4); B = alu_dec8(gb, B);
  CYC(b_+4, b_+6); alu_and(gb, 0xc0);
  CYC(b_+6, b_+7); alu_or(gb, B);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void companionDecCounter1ToJumpDownCliff_hook(GB *gb) {
  BASE(companionDecCounter1ToJumpDownCliff);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x06;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto animate;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); A = 0x53;
  CYC(b_+10, b_+11); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); alu_scf(gb);
  CYC(b_+17, b_+18); ret_effect(gb);
  return;

animate:
  CALL_C(b_+18, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+21);
  CALL_C(b_+21, objectApplySpeed_hook, SYM(objectApplySpeed), b_+24);
  CYC(b_+24, b_+26); C = 0x40;
  CALL_C(b_+26, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+29);
  CYC(b_+29, b_+30); alu_or(gb, D);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void companionDecCounter1IfNonzero_hook(GB *gb) {
  BASE(companionDecCounter1IfNonzero);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x06;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); ret_effect(gb);
}

void companionAnimateDrowningOrFallingThenRespawn_hook(GB *gb) {
  BASE(companionAnimateDrowningOrFallingThenRespawn);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x21;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+7, b_+8); ret_effect(gb); return;
  }
  CYC(b_+7, b_+8);
  CALL_C(b_+8, companionRespawn_hook, SYM(companionRespawn), b_+11);
  CYC(b_+11, b_+12); alu_scf(gb);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void companionInitializeOnEnteringScreen_hook(GB *gb) {
  BASE(companionInitializeOnEnteringScreen);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionCheckCanSpawn_hook, SYM(companionCheckCanSpawn), b_+3);
  CYC(b_+3, b_+5); L = 0x04;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0c);
  CYC(b_+7, b_+9); L = 0x03;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+12); L = 0x07;
  CYC(b_+12, b_+15);
  TAIL(objectSetVisiblec1);
}

void companionRetIfNotFinishedWalkingIn_hook(GB *gb) {
  BASE(companionRetIfNotFinishedWalkingIn);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectGetRelativeTileWithDirectionTable_hook, SYM(specialObjectGetRelativeTileWithDirectionTable), b_+3);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = 0x07;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  if (F & FZ) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); SET_AF(pop_effect(gb));
  CYC(b_+12, b_+13); ret_effect(gb);
}

void companionForceMount_hook(GB *gb) {
  BASE(companionForceMount);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wMenuDisabled);
  CYC(b_+3, b_+4); push_effect(gb, AF);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+8); W8(wMenuDisabled) = A;
  CYC(b_+8, b_+11); W8(w1Link_invincibilityCounter) = A;
  CALL_C(b_+11, companionTryToMount_hook, SYM(companionTryToMount), b_+14);
  CYC(b_+14, b_+15); SET_AF(pop_effect(gb));
  CYC(b_+15, b_+18); W8(wMenuDisabled) = A;
  CYC(b_+18, b_+19); ret_effect(gb);
}

void companionDecCounter1_hook(GB *gb) {
  BASE(companionDecCounter1);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x06;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void specialObjectTryToBreakTile_source05_hook(GB *gb) {
  BASE(specialObjectTryToBreakTile_source05);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = (uint8_t)w1Link_yh;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); C = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_add(gb, 0x05);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); A = 0x05;
  CYC(b_+11, b_+14);
  TAIL(tryToBreakTile);
}
