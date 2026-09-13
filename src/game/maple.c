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

void specialObjectCode_maple_hook(GB *gb);
void mapleState0_hook(GB *gb);
void mapleState1_hook(GB *gb);
void mapleState2_hook(GB *gb);
void mapleState3_hook(GB *gb);
void mapleState4_hook(GB *gb);
void mapleState5_hook(GB *gb);
void mapleDecideNextAngle_hook(GB *gb);
void mapleCollideWithLink_hook(GB *gb);
void mapleUpdateOscillation_hook(GB *gb);

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
    case 0x6699: mapleState6(gb); return;
    case 0x66f5: mapleState7(gb); return;
    case 0x67fb: mapleState8(gb); return;
    case 0x6851: mapleState9(gb); return;
    case 0x67e5: mapleStateA(gb); return;
    case 0x68f0: mapleStateB(gb); return;
    case 0x6941: mapleStateC(gb); return;
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
    CYCT(0x640f, 0x6412); mapleDeleteSelf(gb); return;
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
  CALL_ROM(0x643b, 0x6667); // mapleUpdateAngle
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
    CYCT(0x6465, 0x6468); mapleDeleteSelf(gb); return;
  }
  CYC(0x6465, 0x6468);
  CYC(0x6468, 0x6469); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_ROM(0x6469, 0x6b6b); // mapleInitZPositionAndSpeed
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
  CYC(0x64c7, 0x64ca); mapleDecideAnimation(gb);
}

void mapleCollideWithLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x64ca, dropLinkHeldItem_hook, 0x2c43, 0x64cd);
  CALL_ROM(0x64cd, 0x6993); // mapleSpawnItemDrops
  CYC(0x64d0, 0x64d2); A = 0x01;
  CYC(0x64d2, 0x64d5); W8(wDisableScreenTransitions) = A;
  CYC(0x64d5, 0x64d8); W8(wMenuDisabled) = A;
  CYC(0x64d8, 0x64da); A = 0x3c;
  CYC(0x64da, 0x64dd); W8(wInstrumentsDisabledCounter) = A;
  CYC(0x64dd, 0x64df); E = 0x06;
  CYC(0x64df, 0x64e0); alu_xor(gb, A);
  CYC(0x64e0, 0x64e1); mem_wr(gb, DE, A);
  CALL_ROM(0x64e1, 0x6b7e); // mapleGetCardinalAngleTowardLink
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
  CALL_ROM(0x6552, 0x6962); // mapleKeepInBounds
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
  CALL_ROM(0x659f, 0x6b6b); // mapleInitZPositionAndSpeed
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
  CALL_ROM(0x65b2, 0x667b); // mapleDecideAnimation

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
  CYC(0x6627, 0x662a); mapleDecideItemToCollectAndUpdateTargetAngle(gb); return;

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
