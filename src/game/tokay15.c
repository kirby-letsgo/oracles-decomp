#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_TOKAY), bank 0x15.

void tokayGame_resetRoomFlag40_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(0x5acc, getThisRoomFlags_hook, 0x197d, 0x5acf);
  CYC(0x5acf, 0x5ad1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  RET(0x5ad1); return;
}

void tokayGame_resetRoomFlag80_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(0x5ad2, getThisRoomFlags_hook, 0x197d, 0x5ad5);
  CYC(0x5ad5, 0x5ad7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(0x5ad7); return;
}

void tokayGame_checkRupees_hook(GB *gb);

// For wild tokay game, this sets var3d to 0 if Link has enough rupees, and determines the
// prize (writes 5 to cfdd if the prize will be a ring (1/8 chance), 4 otherwise).
void tokayGame_determinePrizeAndCheckRupees_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ad8, 0x5ad9); H = D;
  CYC(0x5ad9, 0x5adc); A = W8(wWildTokayGameLevel);
  CYC(0x5adc, 0x5ade); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x5ade, 0x5ae0); goto write_prize; } // jr nz
  CYC(0x5ade, 0x5ae0);
  CALL_C(0x5ae0, getRandomNumber_hook, 0x043e, 0x5ae3);
  CYC(0x5ae3, 0x5ae5); alu_and(gb, 0x07);
  CYC(0x5ae5, 0x5ae7); A = 0x04;
  if (!(F & FZ)) { CYCT(0x5ae7, 0x5ae9); goto write_prize; } // jr nz
  CYC(0x5ae7, 0x5ae9);
  CYC(0x5ae9, 0x5aea); A = alu_inc8(gb, A);

write_prize:
  CYC(0x5aea, 0x5aed); mem_wr(gb, wTmpcfc0_wildTokay_cfdd, A);
  tokayGame_checkRupees_hook(gb); return; // fallthrough
}

void tokayGame_checkRupees_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5aed, 0x5af0); A = mem_rd(gb, wTmpcfc0_wildTokay_cfdd);
  CYC(0x5af0, 0x5af3); SET_BC(0x5b04); // @gfx
  CALL_C(0x5af3, addAToBc_hook, 0x006d, 0x5af6);
  CYC(0x5af6, 0x5af7); A = mem_rd(gb, BC);
  CYC(0x5af7, 0x5af8); H = D;
  CYC(0x5af8, 0x5afa); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5afa, 0x5afb); mem_wr(gb, HL, A);
  CYC(0x5afb, 0x5afd); A = 0x04; // RUPEEVAL_10
  CALL_C(0x5afd, cpRupeeValue_hook, 0x1765, 0x5b00);
  CYC(0x5b00, 0x5b02); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x5b02, 0x5b03); mem_wr(gb, DE, A);
  RET(0x5b03); return;
}

void tokayGame_createAccessoryForPrize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b0a, interactionSetAnimation_hook, 0x262e, 0x5b0d);
  CYC(0x5b0d, 0x5b0e); alu_xor(gb, A);
  CYC(0x5b0e, 0x5b10); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x5b10, 0x5b11); mem_wr(gb, DE, A);
  CALL_C(0x5b11, getFreeInteractionSlot_hook, 0x3aef, 0x5b14);
  if (!(F & FZ)) { RET_TAKEN(0x5b14); return; }
  CYC(0x5b14, 0x5b15);
  CYC(0x5b15, 0x5b17); mem_wr(gb, HL, 0x63); // INTERAC_ACCESSORY
  CYC(0x5b17, 0x5b18); L = alu_inc8(gb, L);
  CYC(0x5b18, 0x5b1a); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5b1a, 0x5b1b); A = mem_rd(gb, DE);
  CYC(0x5b1b, 0x5b1c); mem_wr(gb, HL, A);
  CYC(0x5b1c, 0x5b1e); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x5b1e, 0x5b20); mem_wr(gb, HL, 0x40); // Interaction.enabled
  CYC(0x5b20, 0x5b21); L = alu_inc8(gb, L);
  CYC(0x5b21, 0x5b22); mem_wr(gb, HL, D);
  RET(0x5b22); return;
}

// Link jumps in the cutscene where he's robbed.
void tokayMakeLinkJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5b23, 0x5b25); A = 0x81;
  CYC(0x5b25, 0x5b28); mem_wr(gb, wLinkInAir, A);
  CYC(0x5b28, 0x5b2b); SET_HL(w1Link_speedZ);
  CYC(0x5b2b, 0x5b2d); mem_wr(gb, HL, 0x00);
  CYC(0x5b2d, 0x5b2e); L = alu_inc8(gb, L);
  CYC(0x5b2e, 0x5b30); mem_wr(gb, HL, 0xfe);
  CYC(0x5b30, 0x5b32); A = 0x53; // SND_JUMP
  CYC(0x5b32, 0x5b35); playSound_b00_hook(gb); return; // jp
}

void tokayGiveShieldUpgradeToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b35, 0x5b37); B = 0x01;
  CYC(0x5b37, 0x5b39); C = 0x01;
  CYC(0x5b39, 0x5b3c); A = W8(wShieldLevel);
  CYC(0x5b3c, 0x5b3e); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x5b3e, 0x5b40); goto have_tier; } // jr c
  CYC(0x5b3e, 0x5b40);
  CYC(0x5b40, 0x5b41); C = alu_inc8(gb, C);

have_tier:
  CALL_C(0x5b41, createTreasure_hook, 0x27d4, 0x5b44);
  if (!(F & FZ)) { RET_TAKEN(0x5b44); return; }
  CYC(0x5b44, 0x5b45);
  CYC(0x5b45, 0x5b48); SET_DE(w1Link_yh);
  CYC(0x5b48, 0x5b4b); objectCopyPosition_rawAddress_hook(gb); return; // jp
}

// Creates a treasure object at Link's position which he will immediately pick up.
void tokayGiveItemToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b4b, getFreeInteractionSlot_hook, 0x3aef, 0x5b4e);
  if (!(F & FZ)) { RET_TAKEN(0x5b4e); return; }
  CYC(0x5b4e, 0x5b4f);
  CYC(0x5b4f, 0x5b51); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(0x5b51, 0x5b52); L = alu_inc8(gb, L);
  CYC(0x5b52, 0x5b54); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5b54, 0x5b55); A = mem_rd(gb, DE);
  CYC(0x5b55, 0x5b56); mem_wr(gb, HL, A); SET_HL(HL + 1); // [treasure.subid] = [tokay.var03]
  CYC(0x5b56, 0x5b57); E = alu_dec8(gb, E);
  CYC(0x5b57, 0x5b59); B = 0x06;
  CYC(0x5b59, 0x5b5a); A = mem_rd(gb, DE);
  CYC(0x5b5a, 0x5b5c); alu_cp(gb, 0x06); // tokay holding the sword
  if (F & FZ) { CYCT(0x5b5c, 0x5b5e); goto write_treasure_index; } // jr z
  CYC(0x5b5c, 0x5b5e);
  CYC(0x5b5e, 0x5b60); B = 0x01;

write_treasure_index:
  CYC(0x5b60, 0x5b61); mem_wr(gb, HL, B);
  CYC(0x5b61, 0x5b63); alu_cp(gb, 0x0a); // tokay holding seed satchel
  if (!(F & FZ)) { CYCT(0x5b63, 0x5b65); goto set_counter; } // jr nz
  CYC(0x5b63, 0x5b65);
  CYC(0x5b65, 0x5b67); A = 0x24; // TREASURE_MYSTERY_SEEDS
  CALL_C(0x5b67, giveTreasure_hook, 0x171c, 0x5b6a);
  CALL_C(0x5b6a, refillSeedSatchel_hook, 0x180c, 0x5b6d);
  CYC(0x5b6d, 0x5b6e); push_effect(gb, HL);
  CYC(0x5b6e, 0x5b71); SET_HL(wSeedSatchelLevel);
  CYC(0x5b71, 0x5b72); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5b72, 0x5b73); SET_HL(pop_effect(gb));

set_counter:
  CYC(0x5b73, 0x5b75); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5b75, 0x5b77); A = 0x03;
  CYC(0x5b77, 0x5b78); mem_wr(gb, DE, A);
  CYC(0x5b78, 0x5b7b); SET_DE(w1Link_yh);
  CYC(0x5b7b, 0x5b7e); objectCopyPosition_rawAddress_hook(gb); return; // jp
}

void tokayGame_givePrizeToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b7e, 0x5b81); A = mem_rd(gb, wTmpcfc0_wildTokay_cfdd);
  CYC(0x5b81, 0x5b83); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x5b83, 0x5b85); goto random_ring; } // jr z
  CYC(0x5b83, 0x5b85);
  CALL_C(0x5b85, getFreeInteractionSlot_hook, 0x3aef, 0x5b88);
  if (!(F & FZ)) { RET_TAKEN(0x5b88); return; }
  CYC(0x5b88, 0x5b89);
  CYC(0x5b89, 0x5b8b); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(0x5b8b, 0x5b8c); L = alu_inc8(gb, L);
  CYC(0x5b8c, 0x5b8f); A = W8(wWildTokayGameLevel);
  CYC(0x5b8f, 0x5b92); SET_BC(0x5bbb); // @prizes
  CALL_C(0x5b92, addDoubleIndexToBc_hook, 0x007e, 0x5b95);
  CYC(0x5b95, 0x5b96); A = mem_rd(gb, BC);
  CYC(0x5b96, 0x5b97); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5b97, 0x5b98); SET_BC(BC + 1);
  CYC(0x5b98, 0x5b99); A = mem_rd(gb, BC);
  CYC(0x5b99, 0x5b9a); mem_wr(gb, HL, A);
  CYC(0x5b9a, 0x5b9c); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5b9c, 0x5b9e); A = 0x03;
  CYC(0x5b9e, 0x5b9f); mem_wr(gb, DE, A);
  CYC(0x5b9f, 0x5ba2); SET_DE(w1Link_yh);
  CALL_C(0x5ba2, objectCopyPosition_rawAddress_hook, 0x2247, 0x5ba5);
  CYC(0x5ba5, 0x5ba7); goto inc_level; // jr $5bb2

random_ring:
  CYC(0x5ba7, 0x5ba9); C = 0x02;
  CALL_C(0x5ba9, getRandomRingOfGivenTier_hook, 0x17e0, 0x5bac);
  CYC(0x5bac, 0x5bad); B = C;
  CYC(0x5bad, 0x5baf); C = 0x00;
  CALL_C(0x5baf, giveRingToLink_hook, 0x27b4, 0x5bb2);

inc_level:
  CYC(0x5bb2, 0x5bb5); SET_HL(wWildTokayGameLevel);
  CYC(0x5bb5, 0x5bb6); A = mem_rd(gb, HL);
  CYC(0x5bb6, 0x5bb8); alu_cp(gb, 0x04);
  if (F & FZ) { RET_TAKEN(0x5bb8); return; }
  CYC(0x5bb8, 0x5bb9);
  CYC(0x5bb9, 0x5bba); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x5bba); return;
}

// Searches for an interaction of type INTERAC_TOKAY_SHOP_ITEM, and stores the high byte of its
// address in var3f (or writes 0 if none is found).
void tokayFindShopItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5bc5, 0x5bc7); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5bc7, 0x5bc8); alu_xor(gb, A);
  CYC(0x5bc8, 0x5bc9); mem_wr(gb, DE, A);
  CYC(0x5bc9, 0x5bcb); C = 0x81; // INTERAC_TOKAY_SHOP_ITEM
  CALL_C(0x5bcb, objectFindSameTypeObjectWithID_hook, 0x22c8, 0x5bce);
  if (!(F & FZ)) { RET_TAKEN(0x5bce); return; }
  CYC(0x5bce, 0x5bcf);
  CYC(0x5bcf, 0x5bd0); mem_wr(gb, DE, A);
  RET(0x5bd0); return;
}

// Sets var3f to the number of ember seeds you have.
void tokayCheckHaveEmberSeeds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5bd1, 0x5bd2); alu_xor(gb, A);
  CYC(0x5bd2, 0x5bd4); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5bd4, 0x5bd5); mem_wr(gb, DE, A);
  CYC(0x5bd5, 0x5bd7); A = 0x20; // TREASURE_EMBER_SEEDS
  CALL_C(0x5bd7, checkTreasureObtained_hook, 0x1748, 0x5bda);
  if (!(F & FC)) { RET_TAKEN(0x5bda); return; }
  CYC(0x5bda, 0x5bdb);
  CYC(0x5bdb, 0x5bdc); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5bdc); return; }
  CYC(0x5bdc, 0x5bdd);
  CYC(0x5bdd, 0x5bde); mem_wr(gb, DE, A);
  RET(0x5bde); return;
}

void tokayDecNumEmberSeeds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5bdf, 0x5be1); A = 0xff;
  CYC(0x5be1, 0x5be4); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(0x5be4, 0x5be7); A = W8(wNumEmberSeeds);
  CYC(0x5be7, 0x5be9); alu_sub(gb, 0x01);
  CYC(0x5be9, 0x5bea); alu_daa(gb);
  CYC(0x5bea, 0x5bed); mem_wr(gb, wNumEmberSeeds, A);
  RET(0x5bed); return;
}

void tokayUpdateAnimationFromAngle_hook(GB *gb);

void tokayTurnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5bee, objectGetAngleTowardLink_hook, 0x1e9c, 0x5bf1);
  CYC(0x5bf1, 0x5bf3); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5bf3, 0x5bf5); alu_add(gb, 0x04);
  CYC(0x5bf5, 0x5bf7); alu_and(gb, 0x18);
  CYC(0x5bf7, 0x5bf8); mem_wr(gb, DE, A);
  tokayUpdateAnimationFromAngle_hook(gb); return; // fallthrough
}

void tokayUpdateAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5bf8, convertAngleDeToDirection_hook, 0x26f8, 0x5bfb);
  CYC(0x5bfb, 0x5bfe); interactionSetAnimation_hook(gb); return; // jp
}

// Turn to the opposite direction.
void tokayFlipDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5bfe, 0x5c00); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5c00, 0x5c01); A = mem_rd(gb, DE);
  CYC(0x5c01, 0x5c03); alu_xor(gb, 0x10);
  CYC(0x5c03, 0x5c04); mem_wr(gb, DE, A);
  CYC(0x5c04, 0x5c06); tokayUpdateAnimationFromAngle_hook(gb); return; // jr
}

// Removes the seedling from Link's inventory, and sets flag on the present and past versions of
// the room to indicate that it's been planted.
void tokayPlantScentSeedling_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c06, getThisRoomFlags_hook, 0x197d, 0x5c09);
  CYC(0x5c09, 0x5c0b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5c0b, 0x5c0c); H = alu_dec8(gb, H);
  CYC(0x5c0c, 0x5c0e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5c0e, 0x5c10); A = 0x4d; // TREASURE_SCENT_SEEDLING
  CYC(0x5c10, 0x5c13); loseTreasure_hook(gb); return; // jp
}

void tokayGiveBombUpgrade_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5c13, 0x5c16); SET_HL(wMaxBombs);
  CYC(0x5c16, 0x5c17); A = mem_rd(gb, HL);
  CYC(0x5c17, 0x5c19); alu_add(gb, 0x20);
  CYC(0x5c19, 0x5c1a); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5c1a, 0x5c1b); mem_wr(gb, HL, A);
  CYC(0x5c1b, 0x5c1e); setStatusBarNeedsRefreshBit1_hook(gb); return; // jp
}

void tokayCreateExclamationMark_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5c1e, 0x5c21); SET_BC(0xf3f3);
  CYC(0x5c21, 0x5c23); A = 0x1e;
  CYC(0x5c23, 0x5c26); objectCreateExclamationMark_hook(gb); return; // jp
}
