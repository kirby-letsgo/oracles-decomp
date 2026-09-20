#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

#define mapleShadowPathsTable_bank05 0x6c52
#define mapleMovementPatternIndices_bank05 0x6c6c
#define mapleMovementPatternTable_bank05 0x6c7c
#define mapleKnockbackSpeeds_bank05 0x6527
#define mapleWallOffsets_bank05 0x658f
#define mapleNormalEncounterText_bank05 0x663e
#define mapleCollisionRadii_bank05 0x6642
#define mapleItemBitmasks_bank05 0x6a34
#define mapleItemDropDistributionTable_bank05 0x6a38
#define mapleLinkItemDropDistribution_bank05 0x6a58
#define mapleItemDropTreasureIndices_bank05 0x6a75
#define mapleItemValues_bank05 0x6a66
#define mapleState9TextIndices_bank05 0x6897
#define mapleItemIDs_bank05 0x6b37

void specialObjectCode_maple_hook(GB *gb);
void mapleState0_hook(GB *gb);
void mapleState1_hook(GB *gb);
void mapleState2_hook(GB *gb);
void mapleState3_hook(GB *gb);
void mapleState4_hook(GB *gb);
void mapleState5_hook(GB *gb);
void mapleState6_hook(GB *gb);
void mapleState7_hook(GB *gb);
void mapleStateA_hook(GB *gb);
void mapleState8_hook(GB *gb);
void mapleState9_hook(GB *gb);
void mapleEndEncounter_hook(GB *gb);
void mapleDeleteSelf_hook(GB *gb);
void mapleStateB_hook(GB *gb);
void mapleStateC_hook(GB *gb);
void mapleDecideNextAngle_hook(GB *gb);
void mapleCollideWithLink_hook(GB *gb);
void mapleUpdateOscillation_hook(GB *gb);
void mapleUpdateAngle_hook(GB *gb);
void mapleDecideAnimation_hook(GB *gb);
void mapleKeepInBounds_hook(GB *gb);
void mapleSpawnItemDrops_hook(GB *gb);
void mapleSpawnItemDrop_hook(GB *gb);
void mapleCheckLinkCanDropItem_hook(GB *gb);
void mapleInitZPositionAndSpeed_hook(GB *gb);
void mapleGetCardinalAngleTowardLink_hook(GB *gb);
void mapleFindUnexplodedBomb_hook(GB *gb);
void mapleFindNextUnexplodedBomb_hook(GB *gb);
void mapleSpawnItemDrop_variant_hook(GB *gb);
void mapleDecideItemToCollect_hook(GB *gb);
void mapleDecideItemToCollectAndUpdateTargetAngle_hook(GB *gb);
void mapleSetTargetDirectionToRelatedObj2_hook(GB *gb);
void mapleFunc_6c27_hook(GB *gb);
void mapleIncrementMeetingCounter_hook(GB *gb);

static uint16_t maple_jump_table(GB *gb) {
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

static void maple_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void maple_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void specialObjectCode_maple_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x636c, companionRetIfInactiveWithoutStateCheck_hook, 0x488e, 0x636f);
  CYC(0x636f, 0x6371); E = 0x04;
  CYC(0x6371, 0x6372); A = mem_rd(gb, DE);
  CYC(0x6372, 0x6373); push_effect(gb, 0x6373);
  switch (maple_jump_table(gb)) {
    case 0x638d: mapleState0_hook(gb); return;
    case 0x6407: mapleState1_hook(gb); return;
    case 0x6424: mapleState2_hook(gb); return;
    case 0x652a: mapleState3_hook(gb); return;
    case 0x641a: mapleState4_hook(gb); return;
    case 0x6597: mapleState5_hook(gb); return;
    case 0x6699: mapleState6_hook(gb); return;
    case 0x66f5: mapleState7_hook(gb); return;
    case 0x67fb: mapleState8_hook(gb); return;
    case 0x6851: mapleState9_hook(gb); return;
    case 0x67e5: mapleStateA_hook(gb); return;
    case 0x68f0: mapleStateB_hook(gb); return;
    case 0x6941: mapleStateC_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void mapleState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x638d, 0x638e); alu_xor(gb, A);
  CYC(0x638e, 0x6391); W8(wcc85) = A;
  CALL_C(0x6391, specialObjectSetOamVariables_hook, 0x41f7, 0x6394);
  CYC(0x6394, 0x6396); C = 0x02;
  CYC(0x6396, 0x6399); A = W8(wMapleState);
  CYC(0x6399, 0x639b); alu_and(gb, 0x0f);
  CYC(0x639b, 0x639d); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x639d, 0x639f);
    goto variation_decided;
  }
  CYC(0x639d, 0x639f);
  CYC(0x639f, 0x63a0); C = alu_dec8(gb, C);
  CYC(0x63a0, 0x63a2); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(0x63a2, 0x63a4);
    goto variation_decided;
  }
  CYC(0x63a2, 0x63a4);
  CYC(0x63a4, 0x63a5); C = alu_dec8(gb, C);

variation_decided:
  CYC(0x63a5, 0x63a6); A = C;
  CYC(0x63a6, 0x63a8); E = 0x3f;
  CYC(0x63a8, 0x63a9); mem_wr(gb, DE, A);
  CYC(0x63a9, 0x63aa); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x63aa, 0x63ac);
  } else {
    CYC(0x63aa, 0x63ac);
    CYC(0x63ac, 0x63ae); A = 0x01;
  }
  CYC(0x63ae, 0x63b0); E = 0x28;
  CYC(0x63b0, 0x63b1); mem_wr(gb, DE, A);
  CYC(0x63b1, 0x63b2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x63b2, 0x63b4);
    goto vehicle_decided;
  }
  CYC(0x63b2, 0x63b4);
  CALL_C(0x63b4, checkIsLinkedGame_hook, 0x1992, 0x63b7);
  if (F & FZ) {
    CYCT(0x63b7, 0x63b9);
    goto vehicle_decided;
  }
  CYC(0x63b7, 0x63b9);
  CYC(0x63b9, 0x63bb); A = 0x02;
  CYC(0x63bb, 0x63bc); mem_wr(gb, DE, A);

vehicle_decided:
  CALL_C(0x63bc, itemIncState_hook, 0x23ea, 0x63bf);
  CYC(0x63bf, 0x63c1); L = 0x0b;
  CYC(0x63c1, 0x63c3); A = 0x10;
  CYC(0x63c3, 0x63c4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x63c4, 0x63c5); L = alu_inc8(gb, L);
  CYC(0x63c5, 0x63c7); mem_wr(gb, HL, 0xb8);
  CYC(0x63c7, 0x63c9); L = 0x0f;
  CYC(0x63c9, 0x63cb); A = 0x88;
  CYC(0x63cb, 0x63cc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x63cc, 0x63ce); mem_wr(gb, HL, 0x32);
  CYC(0x63ce, 0x63d0); L = 0x26;
  CYC(0x63d0, 0x63d2); A = 0x08;
  CYC(0x63d2, 0x63d3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x63d3, 0x63d4); mem_wr(gb, HL, A);
  CYC(0x63d4, 0x63d6); L = 0x2d;
  CYC(0x63d6, 0x63d8); mem_wr(gb, HL, 0x03);
  CALL_C(0x63d8, getRandomNumber_hook, 0x043e, 0x63db);
  CYC(0x63db, 0x63dd); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(0x63dd, 0x63df);
  } else {
    CYC(0x63dd, 0x63df);
    CYC(0x63df, 0x63e1); A = 0x01;
  }
  CYC(0x63e1, 0x63e3); E = 0x03;
  CYC(0x63e3, 0x63e4); mem_wr(gb, DE, A);
  CYC(0x63e4, 0x63e7); SET_HL(mapleShadowPathsTable_bank05);
  CYC(0x63e7, 0x63e8); maple_add_double_index(gb, 0x63e8);
  CYC(0x63e8, 0x63e9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63e9, 0x63ea); H = mem_rd(gb, HL);
  CYC(0x63ea, 0x63eb); L = A;
  CYC(0x63eb, 0x63ed); E = 0x3a;
  CYC(0x63ed, 0x63ee); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63ee, 0x63ef); mem_wr(gb, DE, A);
  CYC(0x63ef, 0x63f0); E = alu_inc8(gb, E);
  CYC(0x63f0, 0x63f1); mem_wr(gb, DE, A);
  CYC(0x63f1, 0x63f3); E = 0x18;
  CYC(0x63f3, 0x63f4); A = L;
  CYC(0x63f4, 0x63f5); mem_wr(gb, DE, A);
  CYC(0x63f5, 0x63f6); E = alu_inc8(gb, E);
  CYC(0x63f6, 0x63f7); A = H;
  CYC(0x63f7, 0x63f8); mem_wr(gb, DE, A);
  CYC(0x63f8, 0x63f9); A = mem_rd(gb, HL);
  CYC(0x63f9, 0x63fb); E = 0x09;
  CYC(0x63fb, 0x63fc); mem_wr(gb, DE, A);
  CALL_C(0x63fc, mapleDecideNextAngle_hook, 0x64ad, 0x63ff);
  CALL_C(0x63ff, objectSetVisiblec0_hook, 0x1e33, 0x6402);
  CYC(0x6402, 0x6404); A = 0x19;
  CYC(0x6404, 0x6407); specialObjectSetAnimation_hook(gb);
}

void mapleState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6407, mapleState4_hook, 0x641a, 0x640a);
  if (!(F & FZ)) {
    CYCT(0x640a, 0x640b); ret_effect(gb); return;
  }
  CYC(0x640a, 0x640b);
  CYC(0x640b, 0x640e); A = W8(wMenuDisabled);
  CYC(0x640e, 0x640f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x640f, 0x6412); mapleDeleteSelf_hook(gb); return;
  }
  CYC(0x640f, 0x6412);
  CYC(0x6412, 0x6414); A = 0x2b;
  CYC(0x6414, 0x6417); W8(wActiveMusic) = A;
  CYC(0x6417, 0x641a); playSound_b00_hook(gb);
}

void mapleState4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x641a, 0x641d); SET_HL(w1Companion_knockbackCounter);
  CYC(0x641d, 0x641e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x641e, 0x641f); ret_effect(gb); return;
  }
  CYC(0x641e, 0x641f);
  CALL_C(0x641f, itemIncState_hook, 0x23ea, 0x6422);
  CYC(0x6422, 0x6423); alu_xor(gb, A);
  CYC(0x6423, 0x6424); ret_effect(gb);
}

void mapleState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6424, 0x6427); A = W8(wTextIsActive);
  CYC(0x6427, 0x6428); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6428, 0x642a);
    goto animate;
  }
  CYC(0x6428, 0x642a);
  CYC(0x642a, 0x642d); SET_HL(w1Companion_counter2);
  CYC(0x642d, 0x642e); A = mem_rd(gb, HL);
  CYC(0x642e, 0x642f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x642f, 0x6431);
  } else {
    CYC(0x642f, 0x6431);
    CYC(0x6431, 0x6432); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x6432, 0x6433); ret_effect(gb); return;
  }
  CYC(0x6433, 0x6435); L = 0x3d;
  CYC(0x6435, 0x6436); A = mem_rd(gb, HL);
  CYC(0x6436, 0x6438); L = 0x09;
  CYC(0x6438, 0x6439); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6439, 0x643b);
    goto at_target_angle;
  }
  CYC(0x6439, 0x643b);
  CALL_C(0x643b, mapleUpdateAngle_hook, 0x6667, 0x643e);
  CYC(0x643e, 0x6440);
  goto move;

at_target_angle:
  CYC(0x6440, 0x6442); L = 0x06;
  CYC(0x6442, 0x6443); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CALL_C_CC(0x6443, mapleDecideNextAngle_hook, 0x64ad, 0x6446);
  } else {
    CYC(0x6443, 0x6446);
  }
  if (F & FZ) {
    CYCT(0x6446, 0x6448);
    goto choose_path;
  }
  CYC(0x6446, 0x6448);

move:
  CALL_C(0x6448, objectApplySpeed_hook, 0x201d, 0x644b);
  CYC(0x644b, 0x644d); E = 0x3e;
  CYC(0x644d, 0x644e); A = mem_rd(gb, DE);
  CYC(0x644e, 0x644f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x644f, 0x6450); ret_effect(gb); return;
  }
  CYC(0x644f, 0x6450);
  CALL_C(0x6450, checkLinkVulnerableAndIDZero_hook, 0x1d20, 0x6453);
  if (!(F & FC)) {
    CYCT(0x6453, 0x6455);
    goto animate;
  }
  CYC(0x6453, 0x6455);
  CALL_C(0x6455, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x6458);
  if (F & FC) {
    CYCT(0x6458, 0x645a); mapleCollideWithLink_hook(gb); return;
  }
  CYC(0x6458, 0x645a);

animate:
  CALL_C(0x645a, mapleUpdateOscillation_hook, 0x6648, 0x645d);
  CYC(0x645d, 0x6460); specialObjectAnimate_hook(gb); return;

choose_path:
  CYC(0x6460, 0x6463); SET_HL(w1Companion_var3e);
  CYC(0x6463, 0x6464); A = mem_rd(gb, HL);
  CYC(0x6464, 0x6465); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6465, 0x6468); mapleDeleteSelf_hook(gb); return;
  }
  CYC(0x6465, 0x6468);
  CYC(0x6468, 0x6469); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x6469, mapleInitZPositionAndSpeed_hook, 0x6b6b, 0x646c);
  CYC(0x646c, 0x646e); L = 0x10;
  CYC(0x646e, 0x6470); mem_wr(gb, HL, 0x50);
  CYC(0x6470, 0x6472); L = 0x07;
  CYC(0x6472, 0x6474); mem_wr(gb, HL, 0x3c);
  CYC(0x6474, 0x6476); E = 0x3f;
  CYC(0x6476, 0x6477); A = mem_rd(gb, DE);
  CYC(0x6477, 0x6479); E = 0x03;
  CYC(0x6479, 0x647a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x647a, 0x647c);
    goto variation_mask_ready;
  }
  CYC(0x647a, 0x647c);
  CYC(0x647c, 0x647e); E = (uint8_t)(E | 0x04);
  CYC(0x647e, 0x6480); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x6480, 0x6482);
    goto variation_mask_ready;
  }
  CYC(0x6480, 0x6482);
  CYC(0x6482, 0x6484); E = (uint8_t)(E | 0x08);

variation_mask_ready:
  CALL_C(0x6484, getRandomNumber_hook, 0x043e, 0x6487);
  CYC(0x6487, 0x6488); alu_and(gb, E);
  CYC(0x6488, 0x648b); SET_HL(mapleMovementPatternIndices_bank05);
  CYC(0x648b, 0x648c); maple_add_a_to_hl(gb, 0x648c);
  CYC(0x648c, 0x648d); A = mem_rd(gb, HL);
  CYC(0x648d, 0x6490); SET_HL(mapleMovementPatternTable_bank05);
  CYC(0x6490, 0x6491); maple_add_double_index(gb, 0x6491);
  CYC(0x6491, 0x6493); E = 0x0b;
  CYC(0x6493, 0x6494); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6494, 0x6495); H = mem_rd(gb, HL);
  CYC(0x6495, 0x6496); L = A;
  CYC(0x6496, 0x6497); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6497, 0x6498); mem_wr(gb, DE, A);
  CYC(0x6498, 0x649a); E = 0x0d;
  CYC(0x649a, 0x649b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x649b, 0x649c); mem_wr(gb, DE, A);
  CYC(0x649c, 0x649d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x649d, 0x649f); E = 0x3a;
  CYC(0x649f, 0x64a0); mem_wr(gb, DE, A);
  CYC(0x64a0, 0x64a1); E = alu_inc8(gb, E);
  CYC(0x64a1, 0x64a2); mem_wr(gb, DE, A);
  CYC(0x64a2, 0x64a3); A = mem_rd(gb, HL);
  CYC(0x64a3, 0x64a5); E = 0x09;
  CYC(0x64a5, 0x64a6); mem_wr(gb, DE, A);
  CYC(0x64a6, 0x64a8); E = 0x18;
  CYC(0x64a8, 0x64a9); A = L;
  CYC(0x64a9, 0x64aa); mem_wr(gb, DE, A);
  CYC(0x64aa, 0x64ab); E = alu_inc8(gb, E);
  CYC(0x64ab, 0x64ac); A = H;
  CYC(0x64ac, 0x64ad); mem_wr(gb, DE, A);
  mapleDecideNextAngle_hook(gb);
}

void mapleDecideNextAngle_hook(GB *gb) {
  CYC(0x64ad, 0x64b0); SET_HL(w1Companion_relatedObj2);
  CYC(0x64b0, 0x64b1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x64b1, 0x64b2); H = mem_rd(gb, HL);
  CYC(0x64b2, 0x64b3); L = A;
  CYC(0x64b3, 0x64b5); E = 0x3d;
  CYC(0x64b5, 0x64b6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x64b6, 0x64b7); mem_wr(gb, DE, A);
  CYC(0x64b7, 0x64b8); C = A;
  CYC(0x64b8, 0x64ba); E = 0x06;
  CYC(0x64ba, 0x64bb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x64bb, 0x64bc); mem_wr(gb, DE, A);
  CYC(0x64bc, 0x64be); E = 0x18;
  CYC(0x64be, 0x64bf); A = L;
  CYC(0x64bf, 0x64c0); mem_wr(gb, DE, A);
  CYC(0x64c0, 0x64c1); E = alu_inc8(gb, E);
  CYC(0x64c1, 0x64c2); A = H;
  CYC(0x64c2, 0x64c3); mem_wr(gb, DE, A);
  CYC(0x64c3, 0x64c4); A = C;
  CYC(0x64c4, 0x64c6); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x64c6, 0x64c7); ret_effect(gb); return;
  }
  CYC(0x64c6, 0x64c7);
  CYC(0x64c7, 0x64ca); mapleDecideAnimation_hook(gb);
}

void mapleCollideWithLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x64ca, dropLinkHeldItem_hook, 0x2c43, 0x64cd);
  CALL_C(0x64cd, mapleSpawnItemDrops_hook, 0x6993, 0x64d0);
  CYC(0x64d0, 0x64d2); A = 0x01;
  CYC(0x64d2, 0x64d5); W8(wDisableScreenTransitions) = A;
  CYC(0x64d5, 0x64d8); W8(wMenuDisabled) = A;
  CYC(0x64d8, 0x64da); A = 0x3c;
  CYC(0x64da, 0x64dd); W8(wInstrumentsDisabledCounter) = A;
  CYC(0x64dd, 0x64df); E = 0x06;
  CYC(0x64df, 0x64e0); alu_xor(gb, A);
  CYC(0x64e0, 0x64e1); mem_wr(gb, DE, A);
  CALL_C(0x64e1, mapleGetCardinalAngleTowardLink_hook, 0x6b7e, 0x64e4);
  CYC(0x64e4, 0x64e5); B = A;
  CYC(0x64e5, 0x64e8); SET_HL(w1Link_knockbackCounter);
  CYC(0x64e8, 0x64ea); mem_wr(gb, HL, 0x18);
  CYC(0x64ea, 0x64ec); E = 0x09;
  CYC(0x64ec, 0x64ee); L = (uint8_t)w1Link_knockbackAngle;
  CYC(0x64ee, 0x64ef); mem_wr(gb, HL, A);
  CYC(0x64ef, 0x64f1); alu_xor(gb, 0x10);
  CYC(0x64f1, 0x64f2); mem_wr(gb, DE, A);
  CYC(0x64f2, 0x64f4); E = 0x28;
  CYC(0x64f4, 0x64f5); A = mem_rd(gb, DE);
  CYC(0x64f5, 0x64f8); SET_HL(mapleKnockbackSpeeds_bank05);
  CYC(0x64f8, 0x64f9); maple_add_a_to_hl(gb, 0x64f9);
  CYC(0x64f9, 0x64fa); A = mem_rd(gb, HL);
  CYC(0x64fa, 0x64fc); E = 0x10;
  CYC(0x64fc, 0x64fd); mem_wr(gb, DE, A);
  CYC(0x64fd, 0x64ff); E = 0x3a;
  CYC(0x64ff, 0x6501); A = 0xfc;
  CYC(0x6501, 0x6502); mem_wr(gb, DE, A);
  CYC(0x6502, 0x6504); A = 0x0f;
  CYC(0x6504, 0x6507); W8(wScreenShakeCounterX) = A;
  CYC(0x6507, 0x6509); E = 0x04;
  CYC(0x6509, 0x650b); A = 0x03;
  CYC(0x650b, 0x650c); mem_wr(gb, DE, A);
  CYC(0x650c, 0x650d); A = B;
  CYC(0x650d, 0x650f); alu_add(gb, 0x04);
  CYC(0x650f, 0x6510); alu_add(gb, A);
  CYC(0x6510, 0x6511); alu_add(gb, A);
  CYC(0x6511, 0x6513); A = alu_swap(gb, A);
  CYC(0x6513, 0x6515); alu_and(gb, 0x01);
  CYC(0x6515, 0x6517); alu_xor(gb, 0x01);
  CYC(0x6517, 0x6519); alu_add(gb, 0x10);
  CYC(0x6519, 0x651a); B = A;
  CYC(0x651a, 0x651c); E = 0x28;
  CYC(0x651c, 0x651d); A = mem_rd(gb, DE);
  CYC(0x651d, 0x651e); alu_add(gb, A);
  CYC(0x651e, 0x651f); alu_add(gb, B);
  CALL_C(0x651f, specialObjectSetAnimation_hook, 0x2b0a, 0x6522);
  CYC(0x6522, 0x6524); A = 0x85;
  CYC(0x6524, 0x6527); playSound_b00_hook(gb);
}

void mapleState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x652a, 0x652d); A = W8(w1Link_knockbackCounter);
  CYC(0x652d, 0x652e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x652e, 0x6530);
  } else {
    CYC(0x652e, 0x6530);
    CYC(0x6530, 0x6532); A = 0x01;
    CYC(0x6532, 0x6535); W8(wDisabledObjects) = A;
  }
  CYC(0x6535, 0x6536); H = D;
  CYC(0x6536, 0x6538); E = 0x3a;
  CYC(0x6538, 0x6539); A = mem_rd(gb, DE);
  CYC(0x6539, 0x653a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x653a, 0x653c);
    goto animate;
  }
  CYC(0x653a, 0x653c);
  CYC(0x653c, 0x653e); E = 0x0f;
  CYC(0x653e, 0x653f); A = mem_rd(gb, DE);
  CYC(0x653f, 0x6540); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6540, 0x6542);
    goto apply_knockback;
  }
  CYC(0x6540, 0x6542);
  CYC(0x6542, 0x6544); E = 0x3a;
  CYC(0x6544, 0x6546); L = 0x15;
  CYC(0x6546, 0x6547); A = mem_rd(gb, DE);
  CYC(0x6547, 0x6548); A = alu_inc8(gb, A);
  CYC(0x6548, 0x6549); mem_wr(gb, DE, A);
  CYC(0x6549, 0x654a); mem_wr(gb, HL, A);

apply_knockback:
  CYC(0x654a, 0x654c); C = 0x40;
  CALL_C(0x654c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x654f);
  CALL_C(0x654f, objectApplySpeed_hook, 0x201d, 0x6552);
  CALL_C(0x6552, mapleKeepInBounds_hook, 0x6962, 0x6555);
  CALL_C(0x6555, objectGetTileCollisions_hook, 0x14ad, 0x6558);
  if (F & FZ) {
    CYCT(0x6558, 0x6559); ret_effect(gb); return;
  }
  CYC(0x6558, 0x6559);
  CYC(0x6559, 0x655b);
  goto counteract_wall_speed;

animate:
  CYC(0x655b, 0x655e); A = W8(wDisabledObjects);
  CYC(0x655e, 0x655f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x655f, 0x6560); ret_effect(gb); return;
  }
  CYC(0x655f, 0x6560);
  CYC(0x6560, 0x6562); E = 0x21;
  CYC(0x6562, 0x6563); A = mem_rd(gb, DE);
  CYC(0x6563, 0x6565); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x6565, 0x6568); specialObjectAnimate_hook(gb); return;
  }
  CYC(0x6565, 0x6568);
  CYC(0x6568, 0x656a); E = 0x2d;
  CYC(0x656a, 0x656c); A = 0x78;
  CYC(0x656c, 0x656d); mem_wr(gb, DE, A);
  CYC(0x656d, 0x656f); E = 0x04;
  CYC(0x656f, 0x6571); A = 0x04;
  CYC(0x6571, 0x6572); mem_wr(gb, DE, A);
  CYC(0x6572, 0x6573); ret_effect(gb); return;

counteract_wall_speed:
  CYC(0x6573, 0x6575); E = 0x09;
  CALL_C(0x6575, convertAngleDeToDirection_hook, 0x26f8, 0x6578);
  CYC(0x6578, 0x657b); SET_HL(mapleWallOffsets_bank05);
  CYC(0x657b, 0x657c); maple_add_double_index(gb, 0x657c);
  CYC(0x657c, 0x657e); E = 0x0b;
  CYC(0x657e, 0x657f); A = mem_rd(gb, DE);
  CYC(0x657f, 0x6580); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6580, 0x6581); B = A;
  CYC(0x6581, 0x6582); SET_HL(HL + 1);
  CYC(0x6582, 0x6584); E = 0x0d;
  CYC(0x6584, 0x6585); A = mem_rd(gb, DE);
  CYC(0x6585, 0x6586); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6586, 0x6587); C = A;
  CYC(0x6587, 0x6588); H = D;
  CYC(0x6588, 0x658a); L = 0x0b;
  CYC(0x658a, 0x658b); mem_wr(gb, HL, B);
  CYC(0x658b, 0x658d); L = 0x0d;
  CYC(0x658d, 0x658e); mem_wr(gb, HL, C);
  CYC(0x658e, 0x658f); ret_effect(gb);
}

void mapleState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6597, 0x659a); SET_HL(w1Companion_counter1);
  CYC(0x659a, 0x659b); A = mem_rd(gb, HL);
  CYC(0x659b, 0x659c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x659c, 0x659e);
    goto float_up;
  }
  CYC(0x659c, 0x659e);
  CYC(0x659e, 0x659f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x659f, mapleInitZPositionAndSpeed_hook, 0x6b6b, 0x65a2);
  CYC(0x65a2, 0x65a4); L = 0x0f;
  CYC(0x65a4, 0x65a6); mem_wr(gb, HL, 0xff);
  CYC(0x65a6, 0x65a8); A = 0x01;
  CYC(0x65a8, 0x65aa); L = 0x3a;
  CYC(0x65aa, 0x65ab); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x65ab, 0x65ac); mem_wr(gb, HL, A);
  CYC(0x65ac, 0x65ae); E = 0x09;
  CYC(0x65ae, 0x65af); A = mem_rd(gb, DE);
  CYC(0x65af, 0x65b1); alu_xor(gb, 0x10);
  CYC(0x65b1, 0x65b2); mem_wr(gb, DE, A);
  CALL_C(0x65b2, mapleDecideAnimation_hook, 0x667b, 0x65b5);

float_up:
  CYC(0x65b5, 0x65b7); E = 0x28;
  CYC(0x65b7, 0x65b8); A = mem_rd(gb, DE);
  CYC(0x65b8, 0x65b9); C = A;
  CYC(0x65b9, 0x65bb); E = 0x0f;
  CYC(0x65bb, 0x65bc); A = mem_rd(gb, DE);
  CYC(0x65bc, 0x65bd); A = alu_dec8(gb, A);
  CYC(0x65bd, 0x65be); mem_wr(gb, DE, A);
  CYC(0x65be, 0x65c0); alu_cp(gb, 0xf9);
  if (!(F & FC)) {
    CYCT(0x65c0, 0x65c1); ret_effect(gb); return;
  }
  CYC(0x65c0, 0x65c1);
  CYC(0x65c1, 0x65c2); A = C;
  CYC(0x65c2, 0x65c3); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x65c3, 0x65c5);
    goto finished_floating_up;
  }
  CYC(0x65c3, 0x65c5);
  CYC(0x65c5, 0x65c6); A = mem_rd(gb, DE);
  CYC(0x65c6, 0x65c8); alu_cp(gb, 0xe9);
  if (!(F & FC)) {
    CYCT(0x65c8, 0x65c9); ret_effect(gb); return;
  }
  CYC(0x65c8, 0x65c9);

finished_floating_up:
  CYC(0x65c9, 0x65cc); A = W8(wMapleState);
  CYC(0x65cc, 0x65ce); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(0x65ce, 0x65d0);
    goto exchange_touching_book;
  }
  CYC(0x65ce, 0x65d0);
  CYC(0x65d0, 0x65d2); L = 0x04;
  CYC(0x65d2, 0x65d4); mem_wr(gb, HL, 0x06);
  CYC(0x65d4, 0x65d6); E = 0x28;
  CYC(0x65d6, 0x65d7); A = mem_rd(gb, DE);
  CYC(0x65d7, 0x65da); SET_HL(mapleCollisionRadii_bank05);
  CYC(0x65da, 0x65db); maple_add_double_index(gb, 0x65db);
  CYC(0x65db, 0x65dd); E = 0x26;
  CYC(0x65dd, 0x65de); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65de, 0x65df); mem_wr(gb, DE, A);
  CYC(0x65df, 0x65e0); E = alu_inc8(gb, E);
  CYC(0x65e0, 0x65e1); A = mem_rd(gb, HL);
  CYC(0x65e1, 0x65e2); mem_wr(gb, DE, A);
  CYC(0x65e2, 0x65e5); A = W8(wActiveGroup);
  CYC(0x65e5, 0x65e6); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x65e6, 0x65e8);
    goto normal_encounter;
  }
  CYC(0x65e6, 0x65e8);
  CYC(0x65e8, 0x65eb); A = W8(wMapleState);
  CYC(0x65eb, 0x65ed); alu_and(gb, 0x0f);
  CYC(0x65ed, 0x65f0); SET_BC(0x0712);
  if (F & FZ) {
    CYCT(0x65f0, 0x65f2);
    goto set_past_encounter_flag;
  }
  CYC(0x65f0, 0x65f2);
  CYC(0x65f2, 0x65f4); A = 0x44;
  CALL_C(0x65f4, checkGlobalFlag_hook, 0x31f3, 0x65f7);
  CYC(0x65f7, 0x65fa); SET_BC(0x0713);
  if (!(F & FZ)) {
    CYCT(0x65fa, 0x65fc);
    goto normal_encounter;
  }
  CYC(0x65fa, 0x65fc);

set_past_encounter_flag:
  CYC(0x65fc, 0x65fe); A = 0x44;
  CALL_C(0x65fe, setGlobalFlag_hook, 0x31f9, 0x6601);
  CYC(0x6601, 0x6603);
  goto show_text;

normal_encounter:
  CYC(0x6603, 0x6606); A = W8(wMapleState);
  CYC(0x6606, 0x6608); alu_and(gb, 0x0f);
  CYC(0x6608, 0x660b); SET_BC(0x0700);
  if (F & FZ) {
    CYCT(0x660b, 0x660d);
    goto show_text;
  }
  CYC(0x660b, 0x660d);
  CYC(0x660d, 0x660f); C = 0x05;
  CYC(0x660f, 0x6611); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(0x6611, 0x6613);
    goto show_text;
  }
  CYC(0x6611, 0x6613);
  CALL_C(0x6613, getRandomNumber_hook, 0x043e, 0x6616);
  CYC(0x6616, 0x6618); alu_and(gb, 0x03);
  CYC(0x6618, 0x661b); SET_HL(mapleNormalEncounterText_bank05);
  CYC(0x661b, 0x661c); maple_add_a_to_hl(gb, 0x661c);
  CYC(0x661c, 0x661d); C = mem_rd(gb, HL);

show_text:
  CALL_C(0x661d, showText_hook, 0x1872, 0x6620);
  CYC(0x6620, 0x6621); alu_xor(gb, A);
  CYC(0x6621, 0x6624); W8(wDisabledObjects) = A;
  CYC(0x6624, 0x6627); W8(wMenuDisabled) = A;
  CYC(0x6627, 0x662a); mapleDecideItemToCollectAndUpdateTargetAngle_hook(gb); return;

exchange_touching_book:
  CYC(0x662a, 0x662c); A = 0x0b;
  CYC(0x662c, 0x662e); L = 0x04;
  CYC(0x662e, 0x662f); mem_wr(gb, HL, A);
  CYC(0x662f, 0x6631); L = 0x08;
  CYC(0x6631, 0x6632); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6632, 0x6634); mem_wr(gb, HL, 0xff);
  CYC(0x6634, 0x6636); L = 0x10;
  CYC(0x6636, 0x6638); mem_wr(gb, HL, 0x28);
  CYC(0x6638, 0x663b); SET_BC(0x070d);
  CYC(0x663b, 0x663e); showText_hook(gb);
}

void mapleUpdateOscillation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6648, 0x6649); H = D;
  CYC(0x6649, 0x664b); E = 0x28;
  CYC(0x664b, 0x664c); A = mem_rd(gb, DE);
  CYC(0x664c, 0x664e); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x664e, 0x664f); ret_effect(gb); return;
  }
  CYC(0x664e, 0x664f);
  CYC(0x664f, 0x6651); C = 0x00;
  CALL_C(0x6651, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6654);
  CYC(0x6654, 0x6656); L = 0x3c;
  CYC(0x6656, 0x6657); A = mem_rd(gb, HL);
  CYC(0x6657, 0x6658); A = alu_dec8(gb, A);
  CYC(0x6658, 0x6659); mem_wr(gb, HL, A);
  if (!(F & FZ)) {
    CYCT(0x6659, 0x665a); ret_effect(gb); return;
  }
  CYC(0x6659, 0x665a);
  CYC(0x665a, 0x665c); A = 0x16;
  CYC(0x665c, 0x665d); mem_wr(gb, HL, A);
  CYC(0x665d, 0x665f); L = 0x14;
  CYC(0x665f, 0x6660); A = mem_rd(gb, HL);
  CYC(0x6660, 0x6661); alu_cpl(gb);
  CYC(0x6661, 0x6662); A = alu_inc8(gb, A);
  CYC(0x6662, 0x6663); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6663, 0x6664); A = mem_rd(gb, HL);
  CYC(0x6664, 0x6665); alu_cpl(gb);
  CYC(0x6665, 0x6666); mem_wr(gb, HL, A);
  CYC(0x6666, 0x6667); ret_effect(gb);
}

void mapleUpdateAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6667, 0x666a); SET_HL(w1Companion_var3b);
  CYC(0x666a, 0x666b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x666b, 0x666c); ret_effect(gb); return;
  }
  CYC(0x666b, 0x666c);
  CYC(0x666c, 0x666e); E = 0x3a;
  CYC(0x666e, 0x666f); A = mem_rd(gb, DE);
  CYC(0x666f, 0x6670); mem_wr(gb, HL, A);
  CYC(0x6670, 0x6672); L = 0x09;
  CYC(0x6672, 0x6674); E = 0x3d;
  CYC(0x6674, 0x6675); L = mem_rd(gb, HL);
  CYC(0x6675, 0x6677); H8(hFF8B) = A;
  CYC(0x6677, 0x6678); A = mem_rd(gb, DE);
  CALL_C(0x6678, objectNudgeAngleTowards_hook, 0x1fd4, 0x667b);
  mapleDecideAnimation_hook(gb);
}

void mapleDecideAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x667b, 0x667d); E = 0x3e;
  CYC(0x667d, 0x667e); A = mem_rd(gb, DE);
  CYC(0x667e, 0x667f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x667f, 0x6681);
    goto done;
  }
  CYC(0x667f, 0x6681);
  CYC(0x6681, 0x6682); H = D;
  CYC(0x6682, 0x6684); L = 0x09;
  CYC(0x6684, 0x6685); A = mem_rd(gb, HL);
  CALL_C(0x6685, convertAngleToDirection_hook, 0x26f9, 0x6688);
  CYC(0x6688, 0x668a); alu_add(gb, 0x04);
  CYC(0x668a, 0x668b); B = A;
  CYC(0x668b, 0x668d); E = 0x28;
  CYC(0x668d, 0x668e); A = mem_rd(gb, DE);
  CYC(0x668e, 0x668f); alu_add(gb, A);
  CYC(0x668f, 0x6690); alu_add(gb, A);
  CYC(0x6690, 0x6691); alu_add(gb, B);
  CYC(0x6691, 0x6693); L = 0x30;
  CYC(0x6693, 0x6694); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CALL_C_CC(0x6694, specialObjectSetAnimation_hook, 0x2b0a, 0x6697);
  } else {
    CYC(0x6694, 0x6697);
  }

done:
  CYC(0x6697, 0x6698); alu_or(gb, D);
  CYC(0x6698, 0x6699); ret_effect(gb);
}

void mapleState6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6699, mapleUpdateOscillation_hook, 0x6648, 0x669c);
  CALL_C(0x669c, specialObjectAnimate_hook, 0x2aef, 0x669f);
  CALL_C(0x669f, retIfTextIsActive_hook, 0x1859, 0x66a2);
  CYC(0x66a2, 0x66a5); A = W8(wActiveMusic);
  CYC(0x66a5, 0x66a7); alu_cp(gb, 0x2c);
  if (F & FZ) {
    CYCT(0x66a7, 0x66a9);
    goto update_angle;
  }
  CYC(0x66a7, 0x66a9);
  CYC(0x66a9, 0x66ab); A = 0x2c;
  CYC(0x66ab, 0x66ae); W8(wActiveMusic) = A;
  CALL_C(0x66ae, playSound_b00_hook, 0x0c98, 0x66b1);

update_angle:
  CYC(0x66b1, 0x66b3); L = 0x3d;
  CYC(0x66b3, 0x66b4); A = mem_rd(gb, HL);
  CYC(0x66b4, 0x66b6); L = 0x09;
  CYC(0x66b6, 0x66b7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CALL_C_CC(0x66b7, mapleUpdateAngle_hook, 0x6667, 0x66ba);
  } else {
    CYC(0x66b7, 0x66ba);
  }
  CALL_C(0x66ba, mapleDecideItemToCollectAndUpdateTargetAngle_hook, 0x6b84, 0x66bd);
  CALL_C(0x66bd, objectApplySpeed_hook, 0x201d, 0x66c0);
  CYC(0x66c0, 0x66c2); E = 0x18;
  CYC(0x66c2, 0x66c3); A = mem_rd(gb, DE);
  CYC(0x66c3, 0x66c4); H = A;
  CYC(0x66c4, 0x66c5); E = alu_inc8(gb, E);
  CYC(0x66c5, 0x66c6); A = mem_rd(gb, DE);
  CYC(0x66c6, 0x66c7); L = A;
  CALL_C(0x66c7, checkObjectsCollided_hook, 0x1d5a, 0x66ca);
  if (!(F & FC)) {
    CYCT(0x66ca, 0x66cd);
    mapleKeepInBounds_hook(gb);
    return;
  }
  CYC(0x66ca, 0x66cd);
  CYC(0x66cd, 0x66cf); E = 0x18;
  CYC(0x66cf, 0x66d0); A = mem_rd(gb, DE);
  CYC(0x66d0, 0x66d1); H = A;
  CYC(0x66d1, 0x66d2); E = alu_inc8(gb, E);
  CYC(0x66d2, 0x66d3); A = mem_rd(gb, DE);
  CYC(0x66d3, 0x66d5); alu_or(gb, 0x04);
  CYC(0x66d5, 0x66d6); L = A;
  CYC(0x66d6, 0x66d8); mem_wr(gb, HL, 0x04);
  CYC(0x66d8, 0x66d9); L = alu_inc8(gb, L);
  CYC(0x66d9, 0x66db); mem_wr(gb, HL, 0x00);
  CYC(0x66db, 0x66dc); A = mem_rd(gb, DE);
  CYC(0x66dc, 0x66de); alu_or(gb, 0x03);
  CYC(0x66de, 0x66df); L = A;
  CYC(0x66df, 0x66e0); A = mem_rd(gb, HL);
  CYC(0x66e0, 0x66e2); E = 0x2e;
  CYC(0x66e2, 0x66e3); mem_wr(gb, DE, A);
  CYC(0x66e3, 0x66e5); E = 0x04;
  CYC(0x66e5, 0x66e7); A = 0x07;
  CYC(0x66e7, 0x66e8); mem_wr(gb, DE, A);
  CYC(0x66e8, 0x66ea); E = 0x28;
  CYC(0x66ea, 0x66eb); A = mem_rd(gb, DE);
  CYC(0x66eb, 0x66ec); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x66ec, mapleFunc_6c27_hook, 0x6c27, 0x66ef);
  } else {
    CYC(0x66ec, 0x66ef);
  }
  if (F & FZ) {
    CYCT(0x66ef, 0x66f0); ret_effect(gb); return;
  }
  CYC(0x66ef, 0x66f0);
  CYC(0x66f0, 0x66f2); alu_add(gb, 0x16);
  CYC(0x66f2, 0x66f5); specialObjectSetAnimation_hook(gb);
}

void mapleState7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x66f5, specialObjectAnimate_hook, 0x2aef, 0x66f8);
  CYC(0x66f8, 0x66fa); E = 0x28;
  CYC(0x66fa, 0x66fb); A = mem_rd(gb, DE);
  CYC(0x66fb, 0x66fd); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x66fd, 0x6700);
    goto any_vehicle;
  }
  CYC(0x66fd, 0x6700);
  CYC(0x6700, 0x6702); E = 0x26;
  CYC(0x6702, 0x6704); A = 0x08;
  CYC(0x6704, 0x6705); mem_wr(gb, DE, A);
  CYC(0x6705, 0x6706); E = alu_inc8(gb, E);
  CYC(0x6706, 0x6708); A = 0x0a;
  CYC(0x6708, 0x6709); mem_wr(gb, DE, A);
  CALL_C(0x6709, mapleFindUnexplodedBomb_hook, 0x6b40, 0x670c);
  if (!(F & FZ)) {
    CYCT(0x670c, 0x670e);
    goto find_next_bomb;
  }
  CYC(0x670c, 0x670e);
  CALL_C(0x670e, checkObjectsCollided_hook, 0x1d5a, 0x6711);
  if (F & FC) {
    CYCT(0x6711, 0x6713);
    goto explosive_bomb_near_maple;
  }
  CYC(0x6711, 0x6713);

find_next_bomb:
  CALL_C(0x6713, mapleFindNextUnexplodedBomb_hook, 0x6b4e, 0x6716);
  if (!(F & FZ)) {
    CYCT(0x6716, 0x6718);
    goto update_item_being_collected;
  }
  CYC(0x6716, 0x6718);
  CALL_C(0x6718, checkObjectsCollided_hook, 0x1d5a, 0x671b);
  if (F & FC) {
    CYCT(0x671b, 0x671d);
    goto explosive_bomb_near_maple;
  }
  CYC(0x671b, 0x671d);
  CYC(0x671d, 0x671f); E = 0x16;
  CYC(0x671f, 0x6720); alu_xor(gb, A);
  CYC(0x6720, 0x6721); mem_wr(gb, DE, A);
  CYC(0x6721, 0x6722); E = alu_inc8(gb, E);
  CYC(0x6722, 0x6723); mem_wr(gb, DE, A);
  CYC(0x6723, 0x6725);
  goto update_item_being_collected;

explosive_bomb_near_maple:
  CYC(0x6725, 0x6727); L = 0x2f;
  CYC(0x6727, 0x6729); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x6729, 0x672b); B = 0x00;
  CYC(0x672b, 0x672d); L = 0x0b;
  CYC(0x672d, 0x672e); E = L;
  CYC(0x672e, 0x672f); A = mem_rd(gb, DE);
  CYC(0x672f, 0x6730); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6730, 0x6732);
    goto update_bomb_x;
  }
  CYC(0x6730, 0x6732);
  CYC(0x6732, 0x6733); B = alu_inc8(gb, B);
  if (F & FC) {
    CYCT(0x6733, 0x6735);
    goto decrement_bomb_y;
  }
  CYC(0x6733, 0x6735);
  CYC(0x6735, 0x6736); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6736, 0x6738);
  goto update_bomb_x;

decrement_bomb_y:
  CYC(0x6738, 0x6739); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

update_bomb_x:
  CYC(0x6739, 0x673b); L = 0x0d;
  CYC(0x673b, 0x673c); E = L;
  CYC(0x673c, 0x673d); A = mem_rd(gb, DE);
  CYC(0x673d, 0x673e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x673e, 0x6740);
    goto check_bomb_position;
  }
  CYC(0x673e, 0x6740);
  CYC(0x6740, 0x6741); B = alu_inc8(gb, B);
  if (F & FC) {
    CYCT(0x6741, 0x6743);
    goto decrement_bomb_x;
  }
  CYC(0x6741, 0x6743);
  CYC(0x6743, 0x6744); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6744, 0x6746);
  goto check_bomb_position;

decrement_bomb_x:
  CYC(0x6746, 0x6747); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

check_bomb_position:
  CYC(0x6747, 0x6748); A = B;
  CYC(0x6748, 0x6749); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6749, 0x674b);
    goto update_item_being_collected;
  }
  CYC(0x6749, 0x674b);
  CYC(0x674b, 0x674d); L = 0x0e;
  CYC(0x674d, 0x674e); A = mem_rd(gb, HL);
  CYC(0x674e, 0x6750); alu_sub(gb, 0x40);
  CYC(0x6750, 0x6751); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6751, 0x6752); A = mem_rd(gb, HL);
  CYC(0x6752, 0x6754); alu_sbc(gb, 0x00);
  CYC(0x6754, 0x6755); mem_wr(gb, HL, A);
  CYC(0x6755, 0x6757); alu_cp(gb, 0xf8);
  if (!(F & FZ)) {
    CYCT(0x6757, 0x6759);
    goto update_item_being_collected;
  }
  CYC(0x6757, 0x6759);
  CYC(0x6759, 0x675b); L = 0x2f;
  CYC(0x675b, 0x675d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x20));
  CYC(0x675d, 0x675f); A = 0x1a;
  CALL_C(0x675f, specialObjectSetAnimation_hook, 0x2b0a, 0x6762);
  CYC(0x6762, 0x6763); H = D;
  CYC(0x6763, 0x6765); L = 0x04;
  CYC(0x6765, 0x6767); mem_wr(gb, HL, 0x08);
  CYC(0x6767, 0x6768); L = alu_inc8(gb, L);
  CYC(0x6768, 0x676a); mem_wr(gb, HL, 0x00);
  CYC(0x676a, 0x676c); L = 0x07;
  CYC(0x676c, 0x676e); mem_wr(gb, HL, 0x20);
  CYC(0x676e, 0x6770); E = 0x18;
  CYC(0x6770, 0x6771); A = mem_rd(gb, DE);
  CYC(0x6771, 0x6772); H = A;
  CYC(0x6772, 0x6773); E = alu_inc8(gb, E);
  CYC(0x6773, 0x6774); A = mem_rd(gb, DE);
  CYC(0x6774, 0x6775); L = A;
  CYC(0x6775, 0x6776); A = mem_rd(gb, HL);
  CYC(0x6776, 0x6777); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6777, 0x6779);
    goto update_item_being_collected;
  }
  CYC(0x6777, 0x6779);
  CYC(0x6779, 0x677a); A = mem_rd(gb, DE);
  CYC(0x677a, 0x677c); alu_add(gb, 0x04);
  CYC(0x677c, 0x677d); L = A;
  CYC(0x677d, 0x677f); mem_wr(gb, HL, 0x01);
  CYC(0x677f, 0x6781); alu_add(gb, 0x05);
  CYC(0x6781, 0x6782); L = A;
  CYC(0x6782, 0x6784); mem_wr(gb, HL, 0x80);
  CYC(0x6784, 0x6785); alu_xor(gb, A);
  CYC(0x6785, 0x6787); E = 0x18;
  CYC(0x6787, 0x6788); mem_wr(gb, DE, A);

update_item_being_collected:
  CYC(0x6788, 0x678a); E = 0x26;
  CYC(0x678a, 0x678c); A = 0x02;
  CYC(0x678c, 0x678d); mem_wr(gb, DE, A);
  CYC(0x678d, 0x678e); E = alu_inc8(gb, E);
  CYC(0x678e, 0x6790); A = 0x02;
  CYC(0x6790, 0x6791); mem_wr(gb, DE, A);

any_vehicle:
  CYC(0x6791, 0x6793); E = 0x18;
  CYC(0x6793, 0x6794); A = mem_rd(gb, DE);
  CYC(0x6794, 0x6795); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6795, 0x6796); ret_effect(gb); return;
  }
  CYC(0x6795, 0x6796);
  CYC(0x6796, 0x6797); H = A;
  CYC(0x6797, 0x6798); E = alu_inc8(gb, E);
  CYC(0x6798, 0x6799); A = mem_rd(gb, DE);
  CYC(0x6799, 0x679a); L = A;
  CYC(0x679a, 0x679b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x679b, 0x679c); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x679c, 0x679e);
    goto item_collected;
  }
  CYC(0x679c, 0x679e);
  CYC(0x679e, 0x679f); L = alu_inc8(gb, L);
  CYC(0x679f, 0x67a1); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x67a1, 0x67a3);
    goto item_collected;
  }
  CYC(0x67a1, 0x67a3);
  CYC(0x67a3, 0x67a5); E = 0x25;
  CYC(0x67a5, 0x67a6); A = mem_rd(gb, DE);
  CYC(0x67a6, 0x67a7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x67a7, 0x67a8); ret_effect(gb); return;
  }
  CYC(0x67a7, 0x67a8);
  CYC(0x67a8, 0x67aa); E = 0x18;
  CYC(0x67aa, 0x67ab); A = mem_rd(gb, DE);
  CYC(0x67ab, 0x67ac); H = A;
  CYC(0x67ac, 0x67ae); L = 0xc3;
  CYC(0x67ae, 0x67b0); A = 0x80;
  CYC(0x67b0, 0x67b1); mem_wr(gb, HL, A);
  CYC(0x67b1, 0x67b2); alu_xor(gb, A);
  CYC(0x67b2, 0x67b4); L = 0xeb;
  CYC(0x67b4, 0x67b5); mem_wr(gb, HL, A);
  CYC(0x67b5, 0x67b7); L = 0xe4;
  CYC(0x67b7, 0x67b8); mem_wr(gb, HL, A);
  CYC(0x67b8, 0x67ba); E = 0x2e;
  CYC(0x67ba, 0x67bb); A = mem_rd(gb, DE);
  CYC(0x67bb, 0x67be); SET_HL(mapleItemValues_bank05);
  CYC(0x67be, 0x67bf); maple_add_a_to_hl(gb, 0x67bf);
  CYC(0x67bf, 0x67c1); A = 0x0e;
  CYC(0x67c1, 0x67c2); mem_wr(gb, DE, A);
  CYC(0x67c2, 0x67c4); E = 0x29;
  CYC(0x67c4, 0x67c5); A = mem_rd(gb, DE);
  CYC(0x67c5, 0x67c6); B = A;
  CYC(0x67c6, 0x67c7); A = mem_rd(gb, HL);
  CYC(0x67c7, 0x67c8); alu_add(gb, B);
  CYC(0x67c8, 0x67c9); mem_wr(gb, DE, A);
  CYC(0x67c9, 0x67cb); E = 0x28;
  CYC(0x67cb, 0x67cc); A = mem_rd(gb, DE);
  CYC(0x67cc, 0x67cd); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x67cd, 0x67cf);
    goto item_collected;
  }
  CYC(0x67cd, 0x67cf);
  CYC(0x67cf, 0x67d1); A = 0x0a;
  CYC(0x67d1, 0x67d3);
  goto set_state;

item_collected:
  CYC(0x67d3, 0x67d4); H = D;
  CYC(0x67d4, 0x67d6); L = 0x16;
  CYC(0x67d6, 0x67d7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x67d7, 0x67d8); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x67d8, 0x67d9); ret_effect(gb); return;
  }
  CYC(0x67d8, 0x67d9);
  CYC(0x67d9, 0x67db); A = 0x06;

set_state:
  CYC(0x67db, 0x67dd); E = 0x04;
  CYC(0x67dd, 0x67de); mem_wr(gb, DE, A);
  CYC(0x67de, 0x67e0); E = 0x3d;
  CYC(0x67e0, 0x67e1); A = mem_rd(gb, DE);
  CYC(0x67e1, 0x67e3); E = 0x09;
  CYC(0x67e3, 0x67e4); mem_wr(gb, DE, A);
  CYC(0x67e4, 0x67e5); ret_effect(gb);
}

void mapleStateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x67e5, specialObjectAnimate_hook, 0x2aef, 0x67e8);
  CALL_C(0x67e8, itemDecCounter2_hook, 0x23db, 0x67eb);
  if (!(F & FZ)) {
    CYCT(0x67eb, 0x67ec); ret_effect(gb); return;
  }
  CYC(0x67eb, 0x67ec);
  CYC(0x67ec, 0x67ee); L = 0x04;
  CYC(0x67ee, 0x67f0); mem_wr(gb, HL, 0x06);
  CYC(0x67f0, 0x67f2); L = 0x08;
  CYC(0x67f2, 0x67f3); A = mem_rd(gb, HL);
  CYC(0x67f3, 0x67f5); L = 0x0f;
  CYC(0x67f5, 0x67f6); mem_wr(gb, HL, A);
  CYC(0x67f6, 0x67f8); A = 0x04;
  CYC(0x67f8, 0x67fb); specialObjectSetAnimation_hook(gb);
}

void mapleState8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x67fb, specialObjectAnimate_hook, 0x2aef, 0x67fe);
  CYC(0x67fe, 0x6800); E = 0x05;
  CYC(0x6800, 0x6801); A = mem_rd(gb, DE);
  CYC(0x6801, 0x6802); push_effect(gb, 0x6802);
  switch (maple_jump_table(gb)) {
    case 0x680a: goto substate0;
    case 0x681c: goto substate1;
    case 0x682b: goto substate2;
    case 0x6838: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x680a, itemDecCounter2_hook, 0x23db, 0x680d);
  if (!(F & FZ)) {
    CYCT(0x680d, 0x680e); ret_effect(gb); return;
  }
  CYC(0x680d, 0x680e);
  CYC(0x680e, 0x6810); L = 0x05;
  CYC(0x6810, 0x6812); mem_wr(gb, HL, 0x01);
  CYC(0x6812, 0x6814); L = 0x14;
  CYC(0x6814, 0x6815); alu_xor(gb, A);
  CYC(0x6815, 0x6816); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6816, 0x6817); mem_wr(gb, HL, A);
  CYC(0x6817, 0x6819); A = 0x13;
  CYC(0x6819, 0x681c); specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CYC(0x681c, 0x681e); C = 0x40;
  CALL_C(0x681e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6821);
  if (!(F & FZ)) {
    CYCT(0x6821, 0x6822); ret_effect(gb); return;
  }
  CYC(0x6821, 0x6822);
  CYC(0x6822, 0x6824); L = 0x05;
  CYC(0x6824, 0x6826); mem_wr(gb, HL, 0x02);
  CYC(0x6826, 0x6828); L = 0x07;
  CYC(0x6828, 0x682a); mem_wr(gb, HL, 0x40);
  CYC(0x682a, 0x682b); ret_effect(gb); return;

substate2:
  CALL_C(0x682b, itemDecCounter2_hook, 0x23db, 0x682e);
  if (!(F & FZ)) {
    CYCT(0x682e, 0x682f); ret_effect(gb); return;
  }
  CYC(0x682e, 0x682f);
  CYC(0x682f, 0x6831); L = 0x05;
  CYC(0x6831, 0x6833); mem_wr(gb, HL, 0x03);
  CYC(0x6833, 0x6835); A = 0x08;
  CYC(0x6835, 0x6838); specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CYC(0x6838, 0x6839); H = D;
  CYC(0x6839, 0x683b); L = 0x0f;
  CYC(0x683b, 0x683c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x683c, 0x683d); A = mem_rd(gb, HL);
  CYC(0x683d, 0x683f); alu_cp(gb, 0xe9);
  if (!(F & FC)) {
    CYCT(0x683f, 0x6840); ret_effect(gb); return;
  }
  CYC(0x683f, 0x6840);
  CYC(0x6840, 0x6842); L = 0x04;
  CYC(0x6842, 0x6844); mem_wr(gb, HL, 0x06);
  CYC(0x6844, 0x6846); L = 0x29;
  CYC(0x6846, 0x6847); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6847, 0x6849); L = 0x14;
  CYC(0x6849, 0x684b); A = 0x40;
  CYC(0x684b, 0x684c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x684c, 0x684e); mem_wr(gb, HL, 0x00);
  CYC(0x684e, 0x6851); mapleDecideItemToCollectAndUpdateTargetAngle_hook(gb);
}

void mapleState9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6851, specialObjectAnimate_hook, 0x2aef, 0x6854);
  CYC(0x6854, 0x6856); E = 0x05;
  CYC(0x6856, 0x6857); A = mem_rd(gb, DE);
  CYC(0x6857, 0x6858); push_effect(gb, 0x6858);
  switch (maple_jump_table(gb)) {
    case 0x685e: goto substate0;
    case 0x689f: goto substate1;
    case 0x68c5: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x685e, retIfTextIsActive_hook, 0x1859, 0x6861);
  CYC(0x6861, 0x6863); A = 0x3c;
  CYC(0x6863, 0x6866); W8(wInstrumentsDisabledCounter) = A;
  CYC(0x6866, 0x6868); A = 0x01;
  CYC(0x6868, 0x6869); mem_wr(gb, DE, A);
  CYC(0x6869, 0x686a); H = D;
  CYC(0x686a, 0x686c); L = 0x29;
  CYC(0x686c, 0x686d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x686d, 0x686e); B = A;
  CYC(0x686e, 0x686f); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x686f, 0x6871);
    goto show_text;
  }
  CYC(0x686f, 0x6871);
  CYC(0x6871, 0x6872); A = mem_rd(gb, HL);
  CYC(0x6872, 0x6873); alu_cp(gb, B);
  CYC(0x6873, 0x6875); A = 0x01;
  if (F & FZ) {
    CYCT(0x6875, 0x6877);
    goto show_text;
  }
  CYC(0x6875, 0x6877);
  CYC(0x6877, 0x6878); A = alu_inc8(gb, A);
  if (F & FC) {
    CYCT(0x6878, 0x687a);
    goto show_text;
  }
  CYC(0x6878, 0x687a);
  CYC(0x687a, 0x687b); A = alu_inc8(gb, A);

show_text:
  CYC(0x687b, 0x687e); SET_HL(mapleState9TextIndices_bank05);
  CYC(0x687e, 0x687f); maple_add_double_index(gb, 0x687f);
  CYC(0x687f, 0x6880); C = mem_rd(gb, HL);
  CYC(0x6880, 0x6881); SET_HL(HL + 1);
  CYC(0x6881, 0x6882); B = mem_rd(gb, HL);
  CALL_C(0x6882, showText_hook, 0x1872, 0x6885);
  CALL_C(0x6885, mapleGetCardinalAngleTowardLink_hook, 0x6b7e, 0x6888);
  CALL_C(0x6888, convertAngleToDirection_hook, 0x26f9, 0x688b);
  CYC(0x688b, 0x688d); alu_add(gb, 0x04);
  CYC(0x688d, 0x688e); B = A;
  CYC(0x688e, 0x6890); E = 0x28;
  CYC(0x6890, 0x6891); A = mem_rd(gb, DE);
  CYC(0x6891, 0x6892); alu_add(gb, A);
  CYC(0x6892, 0x6893); alu_add(gb, A);
  CYC(0x6893, 0x6894); alu_add(gb, B);
  CYC(0x6894, 0x6897); specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x689f, mapleUpdateOscillation_hook, 0x6648, 0x68a2);
  CALL_C(0x68a2, retIfTextIsActive_hook, 0x1859, 0x68a5);
  CYC(0x68a5, 0x68a7); A = 0x80;
  CYC(0x68a7, 0x68aa); W8(wTextIsActive) = A;
  CYC(0x68aa, 0x68ac); A = 0x1f;
  CYC(0x68ac, 0x68af); W8(wDisabledObjects) = A;
  CYC(0x68af, 0x68b1); L = 0x09;
  CYC(0x68b1, 0x68b3); mem_wr(gb, HL, 0x18);
  CYC(0x68b3, 0x68b5); L = 0x10;
  CYC(0x68b5, 0x68b7); mem_wr(gb, HL, 0x78);
  CYC(0x68b7, 0x68b9); L = 0x05;
  CYC(0x68b9, 0x68bb); mem_wr(gb, HL, 0x02);
  CYC(0x68bb, 0x68bd); E = 0x28;
  CYC(0x68bd, 0x68be); A = mem_rd(gb, DE);
  CYC(0x68be, 0x68bf); alu_add(gb, A);
  CYC(0x68bf, 0x68c0); alu_add(gb, A);
  CYC(0x68c0, 0x68c2); alu_add(gb, 0x07);
  CYC(0x68c2, 0x68c5); specialObjectSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(0x68c5, mapleUpdateOscillation_hook, 0x6648, 0x68c8);
  CALL_C(0x68c8, objectApplySpeed_hook, 0x201d, 0x68cb);
  CALL_C(0x68cb, objectCheckWithinScreenBoundary_hook, 0x2184, 0x68ce);
  if (F & FC) {
    CYCT(0x68ce, 0x68cf); ret_effect(gb); return;
  }
  CYC(0x68ce, 0x68cf);
  mapleEndEncounter_hook(gb);
}

void mapleEndEncounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x68cf, 0x68d0); alu_xor(gb, A);
  CYC(0x68d0, 0x68d3); W8(wTextIsActive) = A;
  CYC(0x68d3, 0x68d6); W8(wDisabledObjects) = A;
  CYC(0x68d6, 0x68d9); W8(wMenuDisabled) = A;
  CYC(0x68d9, 0x68dc); W8(wDisableScreenTransitions) = A;
  CALL_C(0x68dc, mapleIncrementMeetingCounter_hook, 0x6c42, 0x68df);
  mapleDeleteSelf_hook(gb);
}

void mapleDeleteSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x68df, 0x68e2); A = W8(wActiveMusic2);
  CYC(0x68e2, 0x68e5); W8(wActiveMusic) = A;
  CALL_C(0x68e5, playSound_b00_hook, 0x0c98, 0x68e8);
  CYC(0x68e8, 0x68e9); SET_AF(pop_effect(gb));
  CYC(0x68e9, 0x68ea); alu_xor(gb, A);
  CYC(0x68ea, 0x68ed); W8(wIsMaplePresent) = A;
  CYC(0x68ed, 0x68f0); itemDelete_hook(gb);
}

void mapleStateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x68f0, 0x68f1); E = alu_inc8(gb, E);
  CYC(0x68f1, 0x68f2); A = mem_rd(gb, DE);
  CYC(0x68f2, 0x68f3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x68f3, 0x68f5);
    goto substate1;
  }
  CYC(0x68f3, 0x68f5);
  CALL_C(0x68f5, mapleUpdateOscillation_hook, 0x6648, 0x68f8);
  CYC(0x68f8, 0x68fa); E = 0x08;
  CYC(0x68fa, 0x68fb); A = mem_rd(gb, DE);
  CYC(0x68fb, 0x68fd); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x68fd, 0x68ff);
    goto face_link;
  }
  CYC(0x68fd, 0x68ff);
  CYC(0x68ff, 0x6901); alu_and(gb, 0x03);
  CYC(0x6901, 0x6903);
  goto determine_animation;

face_link:
  CALL_C(0x6903, objectGetAngleTowardLink_hook, 0x1e9c, 0x6906);
  CALL_C(0x6906, convertAngleToDirection_hook, 0x26f9, 0x6909);
  CYC(0x6909, 0x690a); H = D;
  CYC(0x690a, 0x690c); L = 0x08;
  CYC(0x690c, 0x690d); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x690d, 0x690e); mem_wr(gb, HL, A);
  if (F & FZ) {
    CYCT(0x690e, 0x6910);
    goto wait_for_text;
  }
  CYC(0x690e, 0x6910);

determine_animation:
  CYC(0x6910, 0x6912); alu_add(gb, 0x04);
  CYC(0x6912, 0x6913); B = A;
  CYC(0x6913, 0x6915); E = 0x28;
  CYC(0x6915, 0x6916); A = mem_rd(gb, DE);
  CYC(0x6916, 0x6917); alu_add(gb, A);
  CYC(0x6917, 0x6918); alu_add(gb, A);
  CYC(0x6918, 0x6919); alu_add(gb, B);
  CALL_C(0x6919, specialObjectSetAnimation_hook, 0x2b0a, 0x691c);

wait_for_text:
  CALL_C(0x691c, retIfTextIsActive_hook, 0x1859, 0x691f);
  CYC(0x691f, 0x6922); SET_HL(wMapleState);
  CYC(0x6922, 0x6924); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x20));
  CYC(0x6924, 0x6926); E = 0x09;
  CYC(0x6926, 0x6927); A = mem_rd(gb, DE);
  CYC(0x6927, 0x6928); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x6928, 0x692b);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x6928, 0x692b);
  CYC(0x692b, 0x692c); ret_effect(gb); return;

substate1:
  CYC(0x692c, 0x692d); A = alu_dec8(gb, A);
  CYC(0x692d, 0x692e); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(0x692e, 0x692f); ret_effect(gb); return;
  }
  CYC(0x692e, 0x692f);
  CYC(0x692f, 0x6932); SET_BC(0x0711);
  CALL_C(0x6932, showText_hook, 0x1872, 0x6935);
  CYC(0x6935, 0x6937); E = 0x09;
  CYC(0x6937, 0x6939); A = 0x18;
  CYC(0x6939, 0x693a); mem_wr(gb, DE, A);
  CALL_C(0x693a, itemIncState_hook, 0x23ea, 0x693d);
  CYC(0x693d, 0x693f); L = 0x10;
  CYC(0x693f, 0x6941); mem_wr(gb, HL, 0x78);
  mapleStateC_hook(gb);
}

void mapleStateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6941, mapleUpdateOscillation_hook, 0x6648, 0x6944);
  CALL_C(0x6944, retIfTextIsActive_hook, 0x1859, 0x6947);
  CALL_C(0x6947, objectApplySpeed_hook, 0x201d, 0x694a);
  CYC(0x694a, 0x694c); E = 0x28;
  CYC(0x694c, 0x694d); A = mem_rd(gb, DE);
  CYC(0x694d, 0x694e); alu_add(gb, A);
  CYC(0x694e, 0x694f); alu_add(gb, A);
  CYC(0x694f, 0x6951); alu_add(gb, 0x07);
  CYC(0x6951, 0x6954); SET_HL(wMapleState);
  CYC(0x6954, 0x6956); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(0x6956, 0x6958); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x10));
  if (!(F & FZ)) {
    CALL_C_CC(0x6958, specialObjectSetAnimation_hook, 0x2b0a, 0x695b);
  } else {
    CYC(0x6958, 0x695b);
  }
  CALL_C(0x695b, objectCheckWithinScreenBoundary_hook, 0x2184, 0x695e);
  if (F & FC) {
    CYCT(0x695e, 0x695f); ret_effect(gb); return;
  }
  CYC(0x695e, 0x695f);
  CYC(0x695f, 0x6962); mapleEndEncounter_hook(gb);
}

void mapleKeepInBounds_hook(GB *gb) {
  CYC(0x6962, 0x6964); E = 0x0b;
  CYC(0x6964, 0x6965); A = mem_rd(gb, DE);
  CYC(0x6965, 0x6967); alu_cp(gb, 0xf0);
  if (F & FC) {
    CYCT(0x6967, 0x6969);
  } else {
    CYC(0x6967, 0x6969);
    CYC(0x6969, 0x696a); alu_xor(gb, A);
  }
  CYC(0x696a, 0x696c); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(0x696c, 0x696e);
    goto check_y_upper;
  }
  CYC(0x696c, 0x696e);
  CYC(0x696e, 0x6970); A = 0x20;
  CYC(0x6970, 0x6971); mem_wr(gb, DE, A);
  CYC(0x6971, 0x6973);
  goto check_x;

check_y_upper:
  CYC(0x6973, 0x6975); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(0x6975, 0x6977);
    goto check_x;
  }
  CYC(0x6975, 0x6977);
  CYC(0x6977, 0x6979); A = 0x78;
  CYC(0x6979, 0x697a); mem_wr(gb, DE, A);

check_x:
  CYC(0x697a, 0x697c); E = 0x0d;
  CYC(0x697c, 0x697d); A = mem_rd(gb, DE);
  CYC(0x697d, 0x697f); alu_cp(gb, 0xf0);
  if (F & FC) {
    CYCT(0x697f, 0x6981);
  } else {
    CYC(0x697f, 0x6981);
    CYC(0x6981, 0x6982); alu_xor(gb, A);
  }
  CYC(0x6982, 0x6984); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(0x6984, 0x6986);
    goto check_x_upper;
  }
  CYC(0x6984, 0x6986);
  CYC(0x6986, 0x6988); A = 0x08;
  CYC(0x6988, 0x6989); mem_wr(gb, DE, A);
  CYC(0x6989, 0x698b);
  goto done;

check_x_upper:
  CYC(0x698b, 0x698d); alu_cp(gb, 0x98);
  if (F & FC) {
    CYCT(0x698d, 0x698f);
    goto done;
  }
  CYC(0x698d, 0x698f);
  CYC(0x698f, 0x6991); A = 0x98;
  CYC(0x6991, 0x6992); mem_wr(gb, DE, A);

done:
  CYC(0x6992, 0x6993); ret_effect(gb);
}

static void maple_spawn_item_drop_check(GB *gb, uint16_t sp0_) {
  CYC(0x69fc, 0x69fd); push_effect(gb, AF);
  CYC(0x69fd, 0x6a00); SET_HL(mapleItemDropTreasureIndices_bank05);
  CYC(0x6a00, 0x6a01); maple_add_a_to_hl(gb, 0x6a01);
  CYC(0x6a01, 0x6a02); A = mem_rd(gb, HL);
  CALL_C(0x6a02, checkTreasureObtained_hook, 0x1748, 0x6a05);
  CYC(0x6a05, 0x6a06); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(0x6a06, 0x6a08);
    goto obtained;
  }
  CYC(0x6a06, 0x6a08);
  CYC(0x6a08, 0x6a09); alu_or(gb, D);
  CYC(0x6a09, 0x6a0a); ret_effect(gb); return;

obtained:
  CYC(0x6a0a, 0x6a0b); A = H;
  CYC(0x6a0b, 0x6a0d); H8(hFF8B) = A;
  CYC(0x6a0d, 0x6a0f); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(0x6a0f, 0x6a12); mapleSpawnItemDrop_hook(gb); return;
  }
  CYC(0x6a0f, 0x6a12);
  CYC(0x6a12, 0x6a13); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6a13, 0x6a15);
    goto not_heart_piece;
  }
  CYC(0x6a13, 0x6a15);
  CYC(0x6a15, 0x6a18); A = W8(wMapleState);
  CYC(0x6a18, 0x6a1a); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6a1a, 0x6a1b); ret_effect(gb); return;
  }
  CYC(0x6a1a, 0x6a1b);
  CYC(0x6a1b, 0x6a1d); E = 0x2b;
  CYC(0x6a1d, 0x6a1e); A = mem_rd(gb, DE);
  CYC(0x6a1e, 0x6a1f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6a1f, 0x6a20); ret_effect(gb); return;
  }
  CYC(0x6a1f, 0x6a20);
  CYC(0x6a20, 0x6a21); A = alu_inc8(gb, A);
  CYC(0x6a21, 0x6a22); mem_wr(gb, DE, A);
  CYC(0x6a22, 0x6a24);
  goto spawn_item;

not_heart_piece:
  CYC(0x6a24, 0x6a25); A = alu_dec8(gb, A);
  CYC(0x6a25, 0x6a28); SET_HL(mapleItemBitmasks_bank05);
  CYC(0x6a28, 0x6a29); maple_add_a_to_hl(gb, 0x6a29);
  CYC(0x6a29, 0x6a2a); B = mem_rd(gb, HL);
  CYC(0x6a2a, 0x6a2c); E = 0x2c;
  CYC(0x6a2c, 0x6a2d); A = mem_rd(gb, DE);
  CYC(0x6a2d, 0x6a2e); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6a2e, 0x6a2f); ret_effect(gb); return;
  }
  CYC(0x6a2e, 0x6a2f);
  CYC(0x6a2f, 0x6a30); A = mem_rd(gb, DE);
  CYC(0x6a30, 0x6a31); alu_or(gb, B);
  CYC(0x6a31, 0x6a32); mem_wr(gb, DE, A);

spawn_item:
  CYC(0x6a32, 0x6a34); mapleSpawnItemDrop_variant_hook(gb);
}

void mapleSpawnItemDrops_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6993, 0x6995); A = 0x41;
  CALL_C(0x6995, checkTreasureObtained_hook, 0x1748, 0x6998);
  if (!(F & FC)) {
    CYCT(0x6998, 0x699a);
    goto no_trade_item;
  }
  CYC(0x6998, 0x699a);
  CYC(0x699a, 0x699c); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x699c, 0x699e);
    goto no_trade_item;
  }
  CYC(0x699c, 0x699e);
  CYC(0x699e, 0x69a0); B = 0xa5;
  CALL_C(0x69a0, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x69a3);
  if (!(F & FZ)) {
    CYCT(0x69a3, 0x69a4); ret_effect(gb); return;
  }
  CYC(0x69a3, 0x69a4);
  CYC(0x69a4, 0x69a7); SET_HL(wMapleState);
  CYC(0x69a7, 0x69a9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x10));
  CYC(0x69a9, 0x69aa); ret_effect(gb); return;

no_trade_item:
  CYC(0x69aa, 0x69ac); E = 0x2a;
  CYC(0x69ac, 0x69ad); alu_xor(gb, A);
  CYC(0x69ad, 0x69ae); mem_wr(gb, DE, A);
  CYC(0x69ae, 0x69b0); E = 0x29;
  CYC(0x69b0, 0x69b1); mem_wr(gb, DE, A);
  CYC(0x69b1, 0x69b3); E = 0x06;
  CYC(0x69b3, 0x69b5); A = 0x05;
  CYC(0x69b5, 0x69b6); mem_wr(gb, DE, A);

next_maple_item:
  CYC(0x69b6, 0x69b8); E = 0x03;
  CYC(0x69b8, 0x69b9); A = mem_rd(gb, DE);
  CYC(0x69b9, 0x69bc); SET_HL(mapleItemDropDistributionTable_bank05);
  CYC(0x69bc, 0x69bd); maple_add_double_index(gb, 0x69bd);
  CYC(0x69bd, 0x69be); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x69be, 0x69bf); H = mem_rd(gb, HL);
  CYC(0x69bf, 0x69c0); L = A;
  CALL_C(0x69c0, getRandomIndexFromProbabilityDistribution_hook, 0x0464, 0x69c3);
  CYC(0x69c3, 0x69c4); A = B;
  CYC(0x69c4, 0x69c7); push_effect(gb, 0x69c7);
  uint16_t local_sp0 = gb->sp;
  maple_spawn_item_drop_check(gb, local_sp0);
  if (gb->pc != 0x69c7 || gb->sp != (uint16_t)(local_sp0 + 2)) return;
  if (F & FC) {
    CYCT(0x69c7, 0x69c9);
    goto maple_item_done;
  }
  CYC(0x69c7, 0x69c9);
  if (!(F & FZ)) {
    CYCT(0x69c9, 0x69cb);
    goto next_maple_item;
  }
  CYC(0x69c9, 0x69cb);

maple_item_done:
  CYC(0x69cb, 0x69cd); E = 0x06;
  CYC(0x69cd, 0x69ce); A = mem_rd(gb, DE);
  CYC(0x69ce, 0x69cf); A = alu_dec8(gb, A);
  CYC(0x69cf, 0x69d0); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(0x69d0, 0x69d2);
    goto next_maple_item;
  }
  CYC(0x69d0, 0x69d2);
  CYC(0x69d2, 0x69d4); A = 0x20;
  CYC(0x69d4, 0x69d6); H8(hFF8C) = A;
  CYC(0x69d6, 0x69d8); E = 0x06;
  CYC(0x69d8, 0x69da); A = 0x05;
  CYC(0x69da, 0x69db); mem_wr(gb, DE, A);

next_link_item:
  CYC(0x69db, 0x69dd); A = H8(hFF8C);
  CYC(0x69dd, 0x69de); A = alu_dec8(gb, A);
  CYC(0x69de, 0x69e0); H8(hFF8C) = A;
  if (F & FZ) {
    CYCT(0x69e0, 0x69e2);
    goto done;
  }
  CYC(0x69e0, 0x69e2);
  CYC(0x69e2, 0x69e5); SET_HL(mapleLinkItemDropDistribution_bank05);
  CALL_C(0x69e5, getRandomIndexFromProbabilityDistribution_hook, 0x0464, 0x69e8);
  CALL_C(0x69e8, mapleCheckLinkCanDropItem_hook, 0x6bb4, 0x69eb);
  if (F & FZ) {
    CYCT(0x69eb, 0x69ed);
    goto next_link_item;
  }
  CYC(0x69eb, 0x69ed);
  CYC(0x69ed, 0x69ef); D = 0xd0;
  CALL_C(0x69ef, mapleSpawnItemDrop_hook, 0x6a83, 0x69f2);
  CYC(0x69f2, 0x69f4); D = 0xd1;
  CYC(0x69f4, 0x69f6); E = 0x06;
  CYC(0x69f6, 0x69f7); A = mem_rd(gb, DE);
  CYC(0x69f7, 0x69f8); A = alu_dec8(gb, A);
  CYC(0x69f8, 0x69f9); mem_wr(gb, DE, A);
  if (!(F & FZ)) {
    CYCT(0x69f9, 0x69fb);
    goto next_link_item;
  }
  CYC(0x69f9, 0x69fb);

done:
  CYC(0x69fb, 0x69fc); ret_effect(gb);
}

void mapleSpawnItemDrop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6a83, getFreePartSlot_hook, 0x3e8e, 0x6a86);
  CYC(0x6a86, 0x6a87); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(0x6a87, 0x6a88); ret_effect(gb); return;
  }
  CYC(0x6a87, 0x6a88);
  CYC(0x6a88, 0x6a8a); mem_wr(gb, HL, 0x14);
  CYC(0x6a8a, 0x6a8c); E = 0x0b;
  CALL_C(0x6a8c, objectCopyPosition_rawAddress_hook, 0x2247, 0x6a8f);
  CYC(0x6a8f, 0x6a91); A = H8(hFF8B);
  CYC(0x6a91, 0x6a93); L = 0xc3;
  CYC(0x6a93, 0x6a94); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6a94, 0x6a95); mem_wr(gb, HL, A);
  CYC(0x6a95, 0x6a96); alu_xor(gb, A);
  CYC(0x6a96, 0x6a97); ret_effect(gb);
}

void mapleSpawnItemDrop_variant_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6a97, getFreePartSlot_hook, 0x3e8e, 0x6a9a);
  CYC(0x6a9a, 0x6a9b); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(0x6a9b, 0x6a9c); ret_effect(gb); return;
  }
  CYC(0x6a9b, 0x6a9c);
  CYC(0x6a9c, 0x6a9e); mem_wr(gb, HL, 0x15);
  CYC(0x6a9e, 0x6aa0); L = 0xc2;
  CYC(0x6aa0, 0x6aa2); A = H8(hFF8B);
  CYC(0x6aa2, 0x6aa3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6aa3, 0x6aa4); mem_wr(gb, HL, A);
  CALL_C(0x6aa4, objectCopyPosition_hook, 0x2242, 0x6aa7);
  CYC(0x6aa7, 0x6aa8); alu_or(gb, A);
  CYC(0x6aa8, 0x6aa9); ret_effect(gb);
}

void mapleDecideItemToCollect_hook(GB *gb) {
  CYC(0x6aa9, 0x6aab); B = 0x00;

id_loop1:
  CYC(0x6aab, 0x6aae); SET_HL(PART_SLOTS);

part_loop1:
  CYC(0x6aae, 0x6ab0); L = 0xc0;
  CYC(0x6ab0, 0x6ab1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6ab1, 0x6ab2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6ab2, 0x6ab4);
    goto next_part1;
  }
  CYC(0x6ab2, 0x6ab4);
  CYC(0x6ab4, 0x6ab5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6ab5, 0x6ab7); alu_cp(gb, 0x15);
  if (!(F & FZ)) {
    CYCT(0x6ab7, 0x6ab9);
    goto next_part1;
  }
  CYC(0x6ab7, 0x6ab9);
  CYC(0x6ab9, 0x6aba); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6aba, 0x6abb); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6abb, 0x6abd);
    goto next_part1;
  }
  CYC(0x6abb, 0x6abd);
  CYC(0x6abd, 0x6abe); L = alu_dec8(gb, L);
  CYC(0x6abe, 0x6abf); alu_xor(gb, A);
  CYC(0x6abf, 0x6ac0); ret_effect(gb); return;

next_part1:
  CYC(0x6ac0, 0x6ac1); H = alu_inc8(gb, H);
  CYC(0x6ac1, 0x6ac2); A = H;
  CYC(0x6ac2, 0x6ac4); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(0x6ac4, 0x6ac6);
    goto part_loop1;
  }
  CYC(0x6ac4, 0x6ac6);
  CYC(0x6ac6, 0x6ac7); B = alu_inc8(gb, B);
  CYC(0x6ac7, 0x6ac8); A = B;
  CYC(0x6ac8, 0x6aca); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(0x6aca, 0x6acc);
    goto id_loop1;
  }
  CYC(0x6aca, 0x6acc);
  CYC(0x6acc, 0x6acd); alu_xor(gb, A);
  CYC(0x6acd, 0x6acf); C = 0x00;
  CYC(0x6acf, 0x6ad2); SET_HL(mapleItemIDs_bank05);
  CYC(0x6ad2, 0x6ad3); maple_add_a_to_hl(gb, 0x6ad3);
  CYC(0x6ad3, 0x6ad4); A = mem_rd(gb, HL);
  CYC(0x6ad4, 0x6ad5); B = A;
  CYC(0x6ad5, 0x6ad6); alu_xor(gb, A);
  CYC(0x6ad6, 0x6ad8); H8(hFF91) = A;

id_loop2:
  CYC(0x6ad8, 0x6adb); SET_HL(PART_SLOTS);

part_loop2:
  CYC(0x6adb, 0x6add); L = 0xc0;
  CYC(0x6add, 0x6ade); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6ade, 0x6adf); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6adf, 0x6ae1);
    goto next_part2;
  }
  CYC(0x6adf, 0x6ae1);
  CYC(0x6ae1, 0x6ae2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6ae2, 0x6ae4); alu_cp(gb, 0x14);
  if (!(F & FZ)) {
    CYCT(0x6ae4, 0x6ae6);
    goto next_part2;
  }
  CYC(0x6ae4, 0x6ae6);
  CYC(0x6ae6, 0x6ae7); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6ae7, 0x6ae8); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6ae8, 0x6aea);
    goto next_part2;
  }
  CYC(0x6ae8, 0x6aea);
  CYC(0x6aea, 0x6aec); L = 0xcb;
  CYC(0x6aec, 0x6aed); L = mem_rd(gb, HL);
  CYC(0x6aed, 0x6aef); E = 0x0b;
  CYC(0x6aef, 0x6af0); A = mem_rd(gb, DE);
  CYC(0x6af0, 0x6af1); alu_sub(gb, L);
  if (!(F & FC)) {
    CYCT(0x6af1, 0x6af3);
    goto y_distance_done;
  }
  CYC(0x6af1, 0x6af3);
  CYC(0x6af3, 0x6af4); alu_cpl(gb);
  CYC(0x6af4, 0x6af5); A = alu_inc8(gb, A);

y_distance_done:
  CYC(0x6af5, 0x6af7); H8(hFF8C) = A;
  CYC(0x6af7, 0x6af9); L = 0xcd;
  CYC(0x6af9, 0x6afa); L = mem_rd(gb, HL);
  CYC(0x6afa, 0x6afc); E = 0x0d;
  CYC(0x6afc, 0x6afd); A = mem_rd(gb, DE);
  CYC(0x6afd, 0x6afe); alu_sub(gb, L);
  if (!(F & FC)) {
    CYCT(0x6afe, 0x6b00);
    goto x_distance_done;
  }
  CYC(0x6afe, 0x6b00);
  CYC(0x6b00, 0x6b01); alu_cpl(gb);
  CYC(0x6b01, 0x6b02); A = alu_inc8(gb, A);

x_distance_done:
  CYC(0x6b02, 0x6b03); L = A;
  CYC(0x6b03, 0x6b05); A = H8(hFF8C);
  CYC(0x6b05, 0x6b06); alu_add(gb, L);
  CYC(0x6b06, 0x6b07); L = A;
  CYC(0x6b07, 0x6b09); A = H8(hFF91);
  CYC(0x6b09, 0x6b0a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6b0a, 0x6b0c);
    goto select_item;
  }
  CYC(0x6b0a, 0x6b0c);
  CYC(0x6b0c, 0x6b0e); A = H8(hFF8D);
  CYC(0x6b0e, 0x6b0f); alu_cp(gb, L);
  if (F & FC) {
    CYCT(0x6b0f, 0x6b11);
    goto next_part2;
  }
  CYC(0x6b0f, 0x6b11);

select_item:
  CYC(0x6b11, 0x6b12); A = L;
  CYC(0x6b12, 0x6b14); H8(hFF8D) = A;
  CYC(0x6b14, 0x6b15); A = H;
  CYC(0x6b15, 0x6b17); H8(hFF91) = A;

next_part2:
  CYC(0x6b17, 0x6b18); H = alu_inc8(gb, H);
  CYC(0x6b18, 0x6b19); A = H;
  CYC(0x6b19, 0x6b1b); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(0x6b1b, 0x6b1d);
    goto part_loop2;
  }
  CYC(0x6b1b, 0x6b1d);
  CYC(0x6b1d, 0x6b1f); A = H8(hFF91);
  CYC(0x6b1f, 0x6b20); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6b20, 0x6b22);
    goto found_item;
  }
  CYC(0x6b20, 0x6b22);
  CYC(0x6b22, 0x6b23); C = alu_inc8(gb, C);
  CYC(0x6b23, 0x6b24); A = C;
  CYC(0x6b24, 0x6b26); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(0x6b26, 0x6b28);
    goto no_items_left;
  }
  CYC(0x6b26, 0x6b28);
  CYC(0x6b28, 0x6b2b); SET_HL(mapleItemIDs_bank05);
  CYC(0x6b2b, 0x6b2c); maple_add_a_to_hl(gb, 0x6b2c);
  CYC(0x6b2c, 0x6b2d); A = mem_rd(gb, HL);
  CYC(0x6b2d, 0x6b2e); B = A;
  CYC(0x6b2e, 0x6b30);
  goto id_loop2;

no_items_left:
  CYC(0x6b30, 0x6b31); alu_and(gb, D);
  CYC(0x6b31, 0x6b32); ret_effect(gb); return;

found_item:
  CYC(0x6b32, 0x6b33); H = A;
  CYC(0x6b33, 0x6b35); L = 0xc0;
  CYC(0x6b35, 0x6b36); alu_xor(gb, A);
  CYC(0x6b36, 0x6b37); ret_effect(gb);
}

static void maple_find_bomb_finish(GB *gb) {
  CYC(0x6b54, 0x6b56); L = 0x2f;
  CYC(0x6b56, 0x6b57); A = mem_rd(gb, HL);
  CYC(0x6b57, 0x6b59); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6b59, 0x6b5b);
    goto found;
  }
  CYC(0x6b59, 0x6b5b);
  CYC(0x6b5b, 0x6b5d); alu_and(gb, 0x60);
  if (!(F & FZ)) {
    CYCT(0x6b5d, 0x6b5e); ret_effect(gb); return;
  }
  CYC(0x6b5d, 0x6b5e);
  CYC(0x6b5e, 0x6b60); L = 0x0f;
  CYC(0x6b60, 0x6b62); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x6b62, 0x6b63); ret_effect(gb); return;
  }
  CYC(0x6b62, 0x6b63);

found:
  CYC(0x6b63, 0x6b65); E = 0x16;
  CYC(0x6b65, 0x6b66); A = H;
  CYC(0x6b66, 0x6b67); mem_wr(gb, DE, A);
  CYC(0x6b67, 0x6b68); E = alu_inc8(gb, E);
  CYC(0x6b68, 0x6b69); alu_xor(gb, A);
  CYC(0x6b69, 0x6b6a); mem_wr(gb, DE, A);
  CYC(0x6b6a, 0x6b6b); ret_effect(gb);
}

void mapleFindUnexplodedBomb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b40, 0x6b42); E = 0x16;
  CYC(0x6b42, 0x6b43); alu_xor(gb, A);
  CYC(0x6b43, 0x6b44); mem_wr(gb, DE, A);
  CYC(0x6b44, 0x6b45); E = alu_inc8(gb, E);
  CYC(0x6b45, 0x6b46); mem_wr(gb, DE, A);
  CYC(0x6b46, 0x6b48); C = 0x03;
  CALL_C(0x6b48, findItemWithID_hook, 0x22b9, 0x6b4b);
  if (!(F & FZ)) {
    CYCT(0x6b4b, 0x6b4c); ret_effect(gb); return;
  }
  CYC(0x6b4b, 0x6b4c);
  CYC(0x6b4c, 0x6b4e);
  maple_find_bomb_finish(gb);
}

void mapleFindNextUnexplodedBomb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b4e, 0x6b50); C = 0x03;
  CALL_C(0x6b50, findItemWithID_startingAfterH_hook, 0x22c0, 0x6b53);
  if (!(F & FZ)) {
    CYCT(0x6b53, 0x6b54); ret_effect(gb); return;
  }
  CYC(0x6b53, 0x6b54);
  maple_find_bomb_finish(gb);
}

void mapleInitZPositionAndSpeed_hook(GB *gb) {
  CYC(0x6b6b, 0x6b6c); H = D;
  CYC(0x6b6c, 0x6b6e); L = 0x0f;
  CYC(0x6b6e, 0x6b70); A = 0xf8;
  CYC(0x6b70, 0x6b71); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6b71, 0x6b73); L = 0x14;
  CYC(0x6b73, 0x6b75); mem_wr(gb, HL, 0x40);
  CYC(0x6b75, 0x6b76); L = alu_inc8(gb, L);
  CYC(0x6b76, 0x6b78); mem_wr(gb, HL, 0x00);
  CYC(0x6b78, 0x6b7a); L = 0x3c;
  CYC(0x6b7a, 0x6b7c); A = 0x16;
  CYC(0x6b7c, 0x6b7d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6b7d, 0x6b7e); ret_effect(gb);
}

void mapleGetCardinalAngleTowardLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b7e, objectGetAngleTowardLink_hook, 0x1e9c, 0x6b81);
  CYC(0x6b81, 0x6b83); alu_and(gb, 0x18);
  CYC(0x6b83, 0x6b84); ret_effect(gb);
}

void mapleDecideItemToCollectAndUpdateTargetAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b84, mapleDecideItemToCollect_hook, 0x6aa9, 0x6b87);
  if (!(F & FZ)) {
    CYCT(0x6b87, 0x6b89);
    goto no_more_items;
  }
  CYC(0x6b87, 0x6b89);
  CYC(0x6b89, 0x6b8b); E = 0x18;
  CYC(0x6b8b, 0x6b8c); A = H;
  CYC(0x6b8c, 0x6b8d); mem_wr(gb, DE, A);
  CYC(0x6b8d, 0x6b8e); E = alu_inc8(gb, E);
  CYC(0x6b8e, 0x6b8f); A = L;
  CYC(0x6b8f, 0x6b90); mem_wr(gb, DE, A);
  CYC(0x6b90, 0x6b92); E = 0x25;
  CYC(0x6b92, 0x6b93); alu_xor(gb, A);
  CYC(0x6b93, 0x6b94); mem_wr(gb, DE, A);
  CYC(0x6b94, 0x6b96); mapleSetTargetDirectionToRelatedObj2_hook(gb); return;

no_more_items:
  CYC(0x6b96, 0x6b98); E = 0x04;
  CYC(0x6b98, 0x6b9a); A = 0x09;
  CYC(0x6b9a, 0x6b9b); mem_wr(gb, DE, A);
  CYC(0x6b9b, 0x6b9c); E = alu_inc8(gb, E);
  CYC(0x6b9c, 0x6b9d); alu_xor(gb, A);
  CYC(0x6b9d, 0x6b9e); mem_wr(gb, DE, A);
  CYC(0x6b9e, 0x6b9f); ret_effect(gb);
}

void mapleSetTargetDirectionToRelatedObj2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b9f, 0x6ba1); E = 0x18;
  CYC(0x6ba1, 0x6ba2); A = mem_rd(gb, DE);
  CYC(0x6ba2, 0x6ba3); H = A;
  CYC(0x6ba3, 0x6ba4); E = alu_inc8(gb, E);
  CYC(0x6ba4, 0x6ba5); A = mem_rd(gb, DE);
  CYC(0x6ba5, 0x6ba7); alu_or(gb, 0x0b);
  CYC(0x6ba7, 0x6ba8); L = A;
  CYC(0x6ba8, 0x6ba9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6ba9, 0x6baa); B = A;
  CYC(0x6baa, 0x6bab); L = alu_inc8(gb, L);
  CYC(0x6bab, 0x6bac); A = mem_rd(gb, HL);
  CYC(0x6bac, 0x6bad); C = A;
  CALL_C(0x6bad, objectGetRelativeAngle_hook, 0x1ea4, 0x6bb0);
  CYC(0x6bb0, 0x6bb2); E = 0x3d;
  CYC(0x6bb2, 0x6bb3); mem_wr(gb, DE, A);
  CYC(0x6bb3, 0x6bb4); ret_effect(gb);
}

void mapleCheckLinkCanDropItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bb4, 0x6bb5); A = B;
  CYC(0x6bb5, 0x6bb7); alu_sub(gb, 0x05);
  CYC(0x6bb7, 0x6bb8); B = A;
  CYC(0x6bb8, 0x6bb9); push_effect(gb, 0x6bb9);
  switch (maple_jump_table(gb)) {
    case 0x6bcb: goto one_rupee;
    case 0x6bda: goto bombs;
    case 0x6bf2: goto seed;
    case 0x6c0f: goto heart;
    default: hook_continue(gb, HL, sp0_); return;
  }

one_rupee:
  CYC(0x6bcb, 0x6bce); SET_HL(wNumRupees);
  CYC(0x6bce, 0x6bcf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6bcf, 0x6bd0); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6bd0, 0x6bd1); ret_effect(gb); return;
  }
  CYC(0x6bd0, 0x6bd1);
  CYC(0x6bd1, 0x6bd3); A = 0x01;
  CALL_C(0x6bd3, removeRupeeValue_hook, 0x1778, 0x6bd6);
  CYC(0x6bd6, 0x6bd8); A = 0x0c;
  CYC(0x6bd8, 0x6bda);
  goto set_item_index;

bombs:
  CYC(0x6bda, 0x6bdc); A = 0x0a;
  CYC(0x6bdc, 0x6bde); H8(hFF8B) = A;
  CALL_C(0x6bde, checkTreasureObtained_hook, 0x1748, 0x6be1);
  if (!(F & FC)) {
    CYCT(0x6be1, 0x6be3);
    goto cannot_drop;
  }
  CYC(0x6be1, 0x6be3);
  CYC(0x6be3, 0x6be6); SET_HL(wNumBombs);
  CYC(0x6be6, 0x6be7); A = mem_rd(gb, HL);
  CYC(0x6be7, 0x6be9); alu_sub(gb, 0x04);
  if (F & FC) {
    CYCT(0x6be9, 0x6beb);
    goto cannot_drop;
  }
  CYC(0x6be9, 0x6beb);
  CYC(0x6beb, 0x6bec); alu_daa(gb);
  CYC(0x6bec, 0x6bed); mem_wr(gb, HL, A);
  CALL_C(0x6bed, setStatusBarNeedsRefreshBit1_hook, 0x17d8, 0x6bf0);
  CYC(0x6bf0, 0x6bf1); alu_or(gb, D);
  CYC(0x6bf1, 0x6bf2); ret_effect(gb); return;

seed:
  CYC(0x6bf2, 0x6bf3); A = B;
  CYC(0x6bf3, 0x6bf5); alu_add(gb, 0x05);
  CYC(0x6bf5, 0x6bf7); H8(hFF8B) = A;
  CALL_C(0x6bf7, checkTreasureObtained_hook, 0x1748, 0x6bfa);
  if (!(F & FC)) {
    CYCT(0x6bfa, 0x6bfc);
    goto cannot_drop;
  }
  CYC(0x6bfa, 0x6bfc);
  CYC(0x6bfc, 0x6bfd); A = B;
  CYC(0x6bfd, 0x6c00); SET_HL(wNumEmberSeeds);
  CYC(0x6c00, 0x6c01); maple_add_a_to_hl(gb, 0x6c01);
  CYC(0x6c01, 0x6c02); A = mem_rd(gb, HL);
  CYC(0x6c02, 0x6c04); alu_sub(gb, 0x05);
  if (F & FC) {
    CYCT(0x6c04, 0x6c06);
    goto cannot_drop;
  }
  CYC(0x6c04, 0x6c06);
  CYC(0x6c06, 0x6c07); alu_daa(gb);
  CYC(0x6c07, 0x6c08); mem_wr(gb, HL, A);
  CALL_C(0x6c08, setStatusBarNeedsRefreshBit1_hook, 0x17d8, 0x6c0b);
  CYC(0x6c0b, 0x6c0c); alu_or(gb, D);
  CYC(0x6c0c, 0x6c0d); ret_effect(gb); return;

cannot_drop:
  CYC(0x6c0d, 0x6c0e); alu_xor(gb, A);
  CYC(0x6c0e, 0x6c0f); ret_effect(gb); return;

heart:
  CYC(0x6c0f, 0x6c12); SET_HL(wLinkHealth);
  CYC(0x6c12, 0x6c13); A = mem_rd(gb, HL);
  CYC(0x6c13, 0x6c15); alu_cp(gb, 0x0c);
  if (!(F & FC)) {
    CYCT(0x6c15, 0x6c17);
  } else {
    CYC(0x6c15, 0x6c17);
    CYC(0x6c17, 0x6c18); alu_xor(gb, A);
    CYC(0x6c18, 0x6c19); ret_effect(gb); return;
  }
  CYC(0x6c19, 0x6c1b); alu_sub(gb, 0x04);
  CYC(0x6c1b, 0x6c1c); mem_wr(gb, HL, A);
  CYC(0x6c1c, 0x6c1f); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x6c1f, 0x6c21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x04));
  CYC(0x6c21, 0x6c23); A = 0x0b;

set_item_index:
  CYC(0x6c23, 0x6c25); H8(hFF8B) = A;
  CYC(0x6c25, 0x6c26); alu_or(gb, D);
  CYC(0x6c26, 0x6c27); ret_effect(gb);
}

void mapleFunc_6c27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c27, 0x6c29); E = 0x07;
  CYC(0x6c29, 0x6c2b); A = 0x30;
  CYC(0x6c2b, 0x6c2c); mem_wr(gb, DE, A);
  CYC(0x6c2c, 0x6c2e); E = 0x0f;
  CYC(0x6c2e, 0x6c2f); A = mem_rd(gb, DE);
  CYC(0x6c2f, 0x6c31); E = 0x08;
  CYC(0x6c31, 0x6c32); mem_wr(gb, DE, A);
  CALL_C(0x6c32, objectGetTileCollisions_hook, 0x14ad, 0x6c35);
  if (!(F & FZ)) {
    CYCT(0x6c35, 0x6c37);
    goto collision;
  }
  CYC(0x6c35, 0x6c37);
  CYC(0x6c37, 0x6c39); E = 0x0f;
  CYC(0x6c39, 0x6c3a); alu_xor(gb, A);
  CYC(0x6c3a, 0x6c3b); mem_wr(gb, DE, A);
  CYC(0x6c3b, 0x6c3c); alu_or(gb, D);
  CYC(0x6c3c, 0x6c3e); E = 0x28;
  CYC(0x6c3e, 0x6c3f); A = mem_rd(gb, DE);
  CYC(0x6c3f, 0x6c40); ret_effect(gb); return;

collision:
  CYC(0x6c40, 0x6c41); alu_xor(gb, A);
  CYC(0x6c41, 0x6c42); ret_effect(gb);
}

void mapleIncrementMeetingCounter_hook(GB *gb) {
  CYC(0x6c42, 0x6c45); SET_HL(wMapleState);
  CYC(0x6c45, 0x6c46); A = mem_rd(gb, HL);
  CYC(0x6c46, 0x6c48); alu_and(gb, 0x0f);
  CYC(0x6c48, 0x6c49); B = A;
  CYC(0x6c49, 0x6c4b); alu_cp(gb, 0x0f);
  if (!(F & FC)) {
    CYCT(0x6c4b, 0x6c4d);
  } else {
    CYC(0x6c4b, 0x6c4d);
    CYC(0x6c4d, 0x6c4e); B = alu_inc8(gb, B);
  }
  CYC(0x6c4e, 0x6c4f); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x6c4f, 0x6c50); alu_or(gb, B);
  CYC(0x6c50, 0x6c51); mem_wr(gb, HL, A);
  CYC(0x6c51, 0x6c52); ret_effect(gb);
}
