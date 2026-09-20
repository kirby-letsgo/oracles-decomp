#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
// Every internal label below is reached only via jr/jp within this one routine (the ages build
// strips the ROM_SEASONS-only block at the top of @state0 entirely) -- none are entered via a
// real `call`, so this whole interaction is a single C function with goto labels.
// ==================================================================================================
void interactionCodece_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x495d, 0x495f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x495f, 0x4960); A = mem_rd(gb, DE);
  CYC(0x4960, 0x4961); push_effect(gb, 0x4961);
  switch (interactionCodece_jump_table(gb)) {
    case 0x4967: goto state0;
    case 0x49d2: goto state1;
    case 0x4a37: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4967, 0x4969); A = 0x01;
  CYC(0x4969, 0x496a); mem_wr(gb, DE, A);
  CALL_C(0x496a, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x496d);
  CYC(0x496d, 0x496f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x496f, 0x4970); A = mem_rd(gb, DE);
  CYC(0x4970, 0x4972); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x4972, 0x4974); goto mimicBush; } // jr nz
  CYC(0x4972, 0x4974);
  CYC(0x4974, 0x4976); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(0x4976, 0x4978); goto sellingShield; } // jr z
  CYC(0x4976, 0x4978);
  CYC(0x4978, 0x497a); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x497a, 0x497c); goto sellingShield; } // jr z
  CYC(0x497a, 0x497c);
  CYC(0x497c, 0x497e); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(0x497e, 0x4980); goto afterSellingShield; } // jr nz
  CYC(0x497e, 0x4980);

sellingShield:
  CYC(0x4980, 0x4981); C = A;
  CYC(0x4981, 0x4984); A = mem_rd(gb, wShieldLevel);
  CYC(0x4984, 0x4985); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4985, 0x4987); goto sellingShieldSetPrice; } // jr z
  CYC(0x4985, 0x4987);
  CYC(0x4987, 0x4988); A = alu_dec8(gb, A);

sellingShieldSetPrice:
  CYC(0x4988, 0x4989); alu_add(gb, C);
  CYC(0x4989, 0x498a); mem_wr(gb, DE, A);
  CYC(0x498a, 0x498d); SET_HL(0x4af9); // @itemPrices
  CYC(0x498d, 0x498e); interactionCodece_addDoubleIndex(gb, 0x498e);
  CYC(0x498e, 0x498f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x498f, 0x4990); B = mem_rd(gb, HL);
  CYC(0x4990, 0x4993); SET_HL(wTextNumberSubstitution);
  CYC(0x4993, 0x4994); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4994, 0x4995); mem_wr(gb, HL, B);

afterSellingShield:
  CYC(0x4995, 0x4997); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x4997, 0x4999); A = 0x06;
  CYC(0x4999, 0x499a); mem_wr(gb, DE, A);
  CYC(0x499a, 0x499b); E = alu_inc8(gb, E);
  CYC(0x499b, 0x499c); mem_wr(gb, DE, A);
  CALL_C(0x499c, interactionInitGraphics_hook, 0x15fb, 0x499f);
  CALL_C(0x499f, objectMakeTileSolid_hook, 0x20b2, 0x49a2);
  CYC(0x49a2, 0x49a4); H = 0xcf; // >wRoomLayout
  CYC(0x49a4, 0x49a6); mem_wr(gb, HL, 0x00);
  CALL_C(0x49a6, objectSetVisible80_hook, 0x1e57, 0x49a9);
  CYC(0x49a9, 0x49ab); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x49ab, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x49ae);
  CALL_C(0x49ae, getFreeInteractionSlot_hook, 0x3aef, 0x49b1);
  CYC(0x49b1, 0x49b3); A = 0xce; // INTERAC_BUSINESS_SCRUB
  CYC(0x49b3, 0x49b4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x49b4, 0x49b6); A = 0x80;
  CYC(0x49b6, 0x49b7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x49b7, 0x49b9); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x49b9, 0x49ba); mem_wr(gb, HL, D);
  CYC(0x49ba, 0x49bd); objectCopyPosition_hook(gb); return; // jp

// Subid $80 initialization (the bush above the scrub)
mimicBush:
  CYC(0x49bd, 0x49c0); A = mem_rd(gb, wActiveGroup);
  CYC(0x49c0, 0x49c1); alu_or(gb, A);
  CYC(0x49c1, 0x49c3); A = 0xc5; // TILEINDEX_OVERWORLD_BUSH_1
  if (F & FZ) { CYCT(0x49c3, 0x49c5); goto mimicBushSetTile; } // jr z
  CYC(0x49c3, 0x49c5);
  CYC(0x49c5, 0x49c7); A = 0xc5; // TILEINDEX_OVERWORLD_BUSH_1

mimicBushSetTile:
  CALL_C(0x49c7, objectMimicBgTile_hook, 0x233b, 0x49ca);
  CYC(0x49ca, 0x49cc); A = 0x05;
  CALL_C(0x49cc, interactionSetAnimation_hook, 0x262e, 0x49cf);
  CYC(0x49cf, 0x49d2); objectSetVisible80_hook(gb); return; // jp

state1:
  CYC(0x49d2, 0x49d5); A = mem_rd(gb, wScrollMode);
  CYC(0x49d5, 0x49d7); alu_and(gb, 0x0e); // SCROLLMODE_08 | SCROLLMODE_04 | SCROLLMODE_02
  if (!(F & FZ)) { CYCT(0x49d7, 0x49d8); ret_effect(gb); return; } // ret nz
  CYC(0x49d7, 0x49d8);
  CYC(0x49d8, 0x49da); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x49da, 0x49db); A = mem_rd(gb, DE);
  CYC(0x49db, 0x49dd); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x49dd, 0x49df); goto subid80State1; } // jr nz
  CYC(0x49dd, 0x49df);
  CALL_C(0x49df, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x49e2);
  CALL_C(0x49e2, interactionAnimate_hook, 0x261b, 0x49e5);
  CYC(0x49e5, 0x49e7); C = 0x20;
  CALL_C(0x49e7, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x49ea);
  CYC(0x49ea, 0x49ec); E = INTERACTION_BASE + OBJ_VAR39;
  if (F & FC) { CYCT(0x49ec, 0x49ee); goto linkIsClose; } // jr c
  CYC(0x49ec, 0x49ee);

  // Link not close
  CYC(0x49ee, 0x49ef); A = mem_rd(gb, DE);
  CYC(0x49ef, 0x49f0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x49f0, 0x49f1); ret_effect(gb); return; } // ret z
  CYC(0x49f0, 0x49f1);
  CYC(0x49f1, 0x49f2); alu_xor(gb, A);
  CYC(0x49f2, 0x49f3); mem_wr(gb, DE, A);
  CYC(0x49f3, 0x49f5); A = 0x03;
  CYC(0x49f5, 0x49f8); interactionSetAnimation_hook(gb); return; // jp

linkIsClose:
  CYC(0x49f8, 0x49f9); A = mem_rd(gb, DE);
  CYC(0x49f9, 0x49fa); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x49fa, 0x49fc); goto afterTalkedCheck; } // jr nz
  CYC(0x49fa, 0x49fc);
  CYC(0x49fc, 0x49fd); A = alu_inc8(gb, A);
  CYC(0x49fd, 0x49fe); mem_wr(gb, DE, A);
  CYC(0x49fe, 0x4a00); A = 0x01;
  CYC(0x4a00, 0x4a03); interactionSetAnimation_hook(gb); return; // jp

afterTalkedCheck:
  CYC(0x4a03, 0x4a05); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x4a05, 0x4a06); A = mem_rd(gb, DE);
  CYC(0x4a06, 0x4a07); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4a07, 0x4a08); ret_effect(gb); return; } // ret z
  CYC(0x4a07, 0x4a08);

  // Link talked to the scrub
  CALL_C(0x4a08, interactionIncState_hook, 0x23e0, 0x4a0b);
  CYC(0x4a0b, 0x4a0d); A = 0x02;
  CALL_C(0x4a0d, interactionSetAnimation_hook, 0x262e, 0x4a10);
  CYC(0x4a10, 0x4a12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4a12, 0x4a13); A = mem_rd(gb, DE);
  CYC(0x4a13, 0x4a16); SET_HL(0x4ace); // @offerItemTextIndices
  CYC(0x4a16, 0x4a17); interactionCodece_addAToHl(gb, 0x4a17);
  CYC(0x4a17, 0x4a18); C = mem_rd(gb, HL);
  CYC(0x4a18, 0x4a1a); B = 0x45; // >TX_4500
  CYC(0x4a1a, 0x4a1d); showTextNonExitable_hook(gb); return; // jp

// Subid $80: the bush above the scrub
subid80State1:
  CYC(0x4a1d, 0x4a1f); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x4a1f, 0x4a20); A = mem_rd(gb, DE);
  CYC(0x4a20, 0x4a21); H = A;
  CYC(0x4a21, 0x4a23); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x4a23, 0x4a24); A = mem_rd(gb, HL);
  CYC(0x4a24, 0x4a26); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x4a26, 0x4a27); mem_wr(gb, DE, A);
  CYC(0x4a27, 0x4a29); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x4a29, 0x4a2a); B = mem_rd(gb, HL);
  CYC(0x4a2a, 0x4a2c); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4a2c, 0x4a2d); A = mem_rd(gb, HL);
  CYC(0x4a2d, 0x4a30); SET_HL(0x4ad7); // @bushYOffsets
  CYC(0x4a30, 0x4a31); interactionCodece_addAToHl(gb, 0x4a31);
  CYC(0x4a31, 0x4a33); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4a33, 0x4a34); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4a34, 0x4a35); alu_add(gb, B);
  CYC(0x4a35, 0x4a36); mem_wr(gb, DE, A);
  RET(0x4a36); return;

state2:
  CALL_C(0x4a37, interactionAnimate_hook, 0x261b, 0x4a3a);
  CYC(0x4a3a, 0x4a3d); A = mem_rd(gb, wTextIsActive);
  CYC(0x4a3d, 0x4a3f); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(0x4a3f, 0x4a40); ret_effect(gb); return; } // ret nz
  CYC(0x4a3f, 0x4a40);

  // Link just finished talking to the scrub
  CYC(0x4a40, 0x4a43); A = mem_rd(gb, wSelectedTextOption);
  CYC(0x4a43, 0x4a45); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x4a45, 0x4a47); goto label_0b_103; } // jr z
  CYC(0x4a45, 0x4a47);
  CYC(0x4a47, 0x4a49); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4a49, 0x4a4b); A = 0x01;
  CYC(0x4a4b, 0x4a4c); mem_wr(gb, DE, A);
  CYC(0x4a4c, 0x4a4d); alu_xor(gb, A);
  CYC(0x4a4d, 0x4a50); mem_wr(gb, wTextIsActive, A);
  CYC(0x4a50, 0x4a52); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x4a52, 0x4a53); mem_wr(gb, DE, A);
  CYC(0x4a53, 0x4a54); A = alu_dec8(gb, A);
  CYC(0x4a54, 0x4a57); mem_wr(gb, wSelectedTextOption, A);
  CYC(0x4a57, 0x4a59); A = 0x04;
  CYC(0x4a59, 0x4a5c); interactionSetAnimation_hook(gb); return; // jp

label_0b_103:
  CYC(0x4a5c, 0x4a5f); A = mem_rd(gb, wSelectedTextOption);
  CYC(0x4a5f, 0x4a60); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4a60, 0x4a62); goto agreedToBuy; } // jr z
  CYC(0x4a60, 0x4a62);

  // Declined to buy
  CYC(0x4a62, 0x4a65); SET_BC(0x4506); // TX_4506
  CYC(0x4a65, 0x4a67); goto showTextLabel; // jr $4ac8

agreedToBuy:
  CYC(0x4a67, 0x4a69); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4a69, 0x4a6a); A = mem_rd(gb, DE);
  CYC(0x4a6a, 0x4a6d); SET_HL(0x4ada); // @rupeeValues
  CYC(0x4a6d, 0x4a6e); interactionCodece_addAToHl(gb, 0x4a6e);
  CYC(0x4a6e, 0x4a6f); A = mem_rd(gb, HL);
  CYC(0x4a6f, 0x4a71); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x4a71, 0x4a72); mem_wr(gb, DE, A);
  CALL_C(0x4a72, cpRupeeValue_hook, 0x1765, 0x4a75);
  if (F & FZ) { CYCT(0x4a75, 0x4a77); goto enoughRupees; } // jr z
  CYC(0x4a75, 0x4a77);

  // Not enough rupees
  CYC(0x4a77, 0x4a7a); SET_BC(0x4507); // TX_4507
  CYC(0x4a7a, 0x4a7c); goto showTextLabel; // jr $4ac8

enoughRupees:
  CYC(0x4a7c, 0x4a7e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4a7e, 0x4a7f); A = mem_rd(gb, DE);
  CYC(0x4a7f, 0x4a82); SET_HL(0x4ae3); // @treasuresToSell
  CYC(0x4a82, 0x4a83); interactionCodece_addDoubleIndex(gb, 0x4a83);
  CYC(0x4a83, 0x4a84); A = mem_rd(gb, HL);
  CYC(0x4a84, 0x4a86); alu_cp(gb, 0x03); // TREASURE_BOMBS
  if (F & FZ) { CYCT(0x4a86, 0x4a88); goto giveBombs; } // jr z
  CYC(0x4a86, 0x4a88);
  CYC(0x4a88, 0x4a8a); alu_cp(gb, 0x20); // TREASURE_EMBER_SEEDS
  if (!(F & FZ)) { CYCT(0x4a8a, 0x4a8c); goto giveShield; } // jr nz
  CYC(0x4a8a, 0x4a8c);

  // giveEmberSeeds (falls through, never a goto target)
  CYC(0x4a8c, 0x4a8f); A = mem_rd(gb, wSeedSatchelLevel);
  CYC(0x4a8f, 0x4a92); SET_BC(0x4aca); // @maxSatchelCapacities-1
  CALL_C(0x4a92, addAToBc_hook, 0x006d, 0x4a95);
  CYC(0x4a95, 0x4a96); A = mem_rd(gb, BC);
  CYC(0x4a96, 0x4a97); C = A;
  CYC(0x4a97, 0x4a9a); A = mem_rd(gb, wNumEmberSeeds);
  CYC(0x4a9a, 0x4a9b); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x4a9b, 0x4a9d); goto giveTreasure; } // jr nz
  CYC(0x4a9b, 0x4a9d);
  CYC(0x4a9d, 0x4a9f); goto alreadyHaveTreasure; // jr $4ab0

giveBombs:
  CYC(0x4a9f, 0x4aa2); SET_BC(wNumBombs);
  CYC(0x4aa2, 0x4aa3); A = mem_rd(gb, BC);
  CYC(0x4aa3, 0x4aa4); C = alu_inc8(gb, C);
  CYC(0x4aa4, 0x4aa5); E = A;
  CYC(0x4aa5, 0x4aa6); A = mem_rd(gb, BC);
  CYC(0x4aa6, 0x4aa7); alu_cp(gb, E);
  if (!(F & FZ)) { CYCT(0x4aa7, 0x4aa9); goto giveTreasure; } // jr nz
  CYC(0x4aa7, 0x4aa9);
  CYC(0x4aa9, 0x4aab); goto alreadyHaveTreasure; // jr $4ab0

giveShield:
  CALL_C(0x4aab, checkTreasureObtained_hook, 0x1748, 0x4aae);
  if (!(F & FC)) { CYCT(0x4aae, 0x4ab0); goto giveTreasure; } // jr nc
  CYC(0x4aae, 0x4ab0);

alreadyHaveTreasure:
  CYC(0x4ab0, 0x4ab3); SET_BC(0x4508); // TX_4508
  CYC(0x4ab3, 0x4ab5); goto showTextLabel; // jr $4ac8

giveTreasure:
  CYC(0x4ab5, 0x4ab6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4ab6, 0x4ab7); C = mem_rd(gb, HL);
  CALL_C(0x4ab7, giveTreasure_hook, 0x171c, 0x4aba);
  CYC(0x4aba, 0x4abc); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x4abc, 0x4abd); A = mem_rd(gb, DE);
  CALL_C(0x4abd, removeRupeeValue_hook, 0x1778, 0x4ac0);
  CYC(0x4ac0, 0x4ac2); A = 0x5e; // SND_GETSEED
  CALL_C(0x4ac2, playSound_b00_hook, 0x0c98, 0x4ac5);
  CYC(0x4ac5, 0x4ac8); SET_BC(0x4505); // TX_4505

showTextLabel:
  CYC(0x4ac8, 0x4acb); showText_hook(gb); return; // jp
}
