#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_TOKAY), bank 0x15.

void tokayGame_resetRoomFlag40_hook(GB *gb) {
  BASE(tokayGame_resetRoomFlag40);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  RET(b_+5); return;
}

void tokayGame_resetRoomFlag80_hook(GB *gb) {
  BASE(tokayGame_resetRoomFlag80);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(b_+5); return;
}

void tokayGame_checkRupees_hook(GB *gb);

// For wild tokay game, this sets var3d to 0 if Link has enough rupees, and determines the
// prize (writes 5 to cfdd if the prize will be a ring (1/8 chance), 4 otherwise).
void tokayGame_determinePrizeAndCheckRupees_hook(GB *gb) {
  BASE(tokayGame_determinePrizeAndCheckRupees);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+4); A = W8(wWildTokayGameLevel);
  CYC(b_+4, b_+6); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto write_prize; } // jr nz
  CYC(b_+6, b_+8);
  CALL_C(b_+8, getRandomNumber_hook, SYM(getRandomNumber), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x07);
  CYC(b_+13, b_+15); A = 0x04;
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto write_prize; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);

write_prize:
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcfc0_wildTokay_cfdd, A);
  TAIL(tokayGame_checkRupees); // fallthrough
}

void tokayGame_checkRupees_hook(GB *gb) {
  BASE(tokayGame_checkRupees);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_wildTokay_cfdd);
  CYC(b_+3, b_+6); SET_BC(b_+23); // @gfx
  CALL_C(b_+6, addAToBc_hook, 0x006d, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); A = 0x04; // RUPEEVAL_10
  CALL_C(b_+16, cpRupeeValue_hook, SYM(cpRupeeValue), b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

void tokayGame_createAccessoryForPrize_hook(GB *gb) {
  BASE(tokayGame_createAccessoryForPrize);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+3);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CALL_C(b_+7, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x63); // INTERAC_ACCESSORY
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x40); // Interaction.enabled
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+24); mem_wr(gb, HL, D);
  RET(b_+24); return;
}

// Link jumps in the cutscene where he's robbed.
void tokayMakeLinkJump_hook(GB *gb) {
  BASE(tokayMakeLinkJump);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x81;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkInAir, A);
  CYC(b_+5, b_+8); SET_HL(w1Link_speedZ);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xfe);
  CYC(b_+13, b_+15); A = 0x53; // SND_JUMP
  CYC(b_+15, b_+18); TAIL(playSound_b00); // jp
}

void tokayGiveShieldUpgradeToLink_hook(GB *gb) {
  BASE(tokayGiveShieldUpgradeToLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x01;
  CYC(b_+2, b_+4); C = 0x01;
  CYC(b_+4, b_+7); A = W8(wShieldLevel);
  CYC(b_+7, b_+9); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+9, b_+11); goto have_tier; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); C = alu_inc8(gb, C);

have_tier:
  CALL_C(b_+12, createTreasure_hook, SYM(createTreasure), b_+15);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+19); SET_DE(w1Link_yh);
  CYC(b_+19, b_+22); TAIL(objectCopyPosition_rawAddress); // jp
}

// Creates a treasure object at Link's position which he will immediately pick up.
void tokayGiveItemToLink_hook(GB *gb) {
  BASE(tokayGiveItemToLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // [treasure.subid] = [tokay.var03]
  CYC(b_+11, b_+12); E = alu_dec8(gb, E);
  CYC(b_+12, b_+14); B = 0x06;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x06); // tokay holding the sword
  if (F & FZ) { CYCT(b_+17, b_+19); goto write_treasure_index; } // jr z
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); B = 0x01;

write_treasure_index:
  CYC(b_+21, b_+22); mem_wr(gb, HL, B);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0a); // tokay holding seed satchel
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto set_counter; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x24; // TREASURE_MYSTERY_SEEDS
  CALL_C(b_+28, giveTreasure_hook, SYM(giveTreasure), b_+31);
  CALL_C(b_+31, refillSeedSatchel_hook, SYM(refillSeedSatchel), b_+34);
  CYC(b_+34, b_+35); push_effect(gb, HL);
  CYC(b_+35, b_+38); SET_HL(wSeedSatchelLevel);
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+39, b_+40); SET_HL(pop_effect(gb));

set_counter:
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); A = 0x03;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+48); SET_DE(w1Link_yh);
  CYC(b_+48, b_+51); TAIL(objectCopyPosition_rawAddress); // jp
}

void tokayGame_givePrizeToLink_hook(GB *gb) {
  BASE(tokayGame_givePrizeToLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_wildTokay_cfdd);
  CYC(b_+3, b_+5); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+5, b_+7); goto random_ring; } // jr z
  CYC(b_+5, b_+7);
  CALL_C(b_+7, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+17); A = W8(wWildTokayGameLevel);
  CYC(b_+17, b_+20); SET_BC(b_+61); // @prizes
  CALL_C(b_+20, addDoubleIndexToBc_hook, 0x007e, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, BC);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); SET_BC(BC + 1);
  CYC(b_+26, b_+27); A = mem_rd(gb, BC);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); A = 0x03;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+36); SET_DE(w1Link_yh);
  CALL_C(b_+36, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+39);
  CYC(b_+39, b_+41); goto inc_level; // jr $5bb2

random_ring:
  CYC(b_+41, b_+43); C = 0x02;
  CALL_C(b_+43, getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+46);
  CYC(b_+46, b_+47); B = C;
  CYC(b_+47, b_+49); C = 0x00;
  CALL_C(b_+49, giveRingToLink_hook, SYM(giveRingToLink), b_+52);

inc_level:
  CYC(b_+52, b_+55); SET_HL(wWildTokayGameLevel);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+58); alu_cp(gb, 0x04);
  if (F & FZ) { RET_TAKEN(b_+58); return; }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+60); return;
}

// Searches for an interaction of type INTERAC_TOKAY_SHOP_ITEM, and stores the high byte of its
// address in var3f (or writes 0 if none is found).
void tokayFindShopItem_hook(GB *gb) {
  BASE(tokayFindShopItem);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+6); C = 0x81; // INTERAC_TOKAY_SHOP_ITEM
  CALL_C(b_+6, objectFindSameTypeObjectWithID_hook, SYM(objectFindSameTypeObjectWithID), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11); return;
}

// Sets var3f to the number of ember seeds you have.
void tokayCheckHaveEmberSeeds_hook(GB *gb) {
  BASE(tokayCheckHaveEmberSeeds);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+6); A = 0x20; // TREASURE_EMBER_SEEDS
  CALL_C(b_+6, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+9);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

void tokayDecNumEmberSeeds_hook(GB *gb) {
  BASE(tokayDecNumEmberSeeds);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+5, b_+8); A = W8(wNumEmberSeeds);
  CYC(b_+8, b_+10); alu_sub(gb, 0x01);
  CYC(b_+10, b_+11); alu_daa(gb);
  CYC(b_+11, b_+14); mem_wr(gb, wNumEmberSeeds, A);
  RET(b_+14); return;
}

void tokayUpdateAnimationFromAngle_hook(GB *gb);

void tokayTurnToFaceLink_hook(GB *gb) {
  BASE(tokayTurnToFaceLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+7); alu_add(gb, 0x04);
  CYC(b_+7, b_+9); alu_and(gb, 0x18);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  TAIL(tokayUpdateAnimationFromAngle); // fallthrough
}

void tokayUpdateAnimationFromAngle_hook(GB *gb) {
  BASE(tokayUpdateAnimationFromAngle);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionSetAnimation); // jp
}

// Turn to the opposite direction.
void tokayFlipDirection_hook(GB *gb) {
  BASE(tokayFlipDirection);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x10);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); TAIL(tokayUpdateAnimationFromAngle); // jr
}

// Removes the seedling from Link's inventory, and sets flag on the present and past versions of
// the room to indicate that it's been planted.
void tokayPlantScentSeedling_hook(GB *gb) {
  BASE(tokayPlantScentSeedling);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+5, b_+6); H = alu_dec8(gb, H);
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+8, b_+10); A = 0x4d; // TREASURE_SCENT_SEEDLING
  CYC(b_+10, b_+13); TAIL(loseTreasure); // jp
}

void tokayGiveBombUpgrade_hook(GB *gb) {
  BASE(tokayGiveBombUpgrade);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wMaxBombs);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_add(gb, 0x20);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+11); TAIL(setStatusBarNeedsRefreshBit1); // jp
}

void tokayCreateExclamationMark_hook(GB *gb) {
  BASE(tokayCreateExclamationMark);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf3f3);
  CYC(b_+3, b_+5); A = 0x1e;
  CYC(b_+5, b_+8); TAIL(objectCreateExclamationMark); // jp
}
