#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void seedItemDelete_hook(GB *gb);
void emberSeedBurn_hook(GB *gb);
void seedUpdateAnimation_hook(GB *gb);
void scentSeedSmell_hook(GB *gb);
void galeSeedUpdateAnimationAndCounter_hook(GB *gb);
void galeSeedUpdateAnimation_hook(GB *gb);
void galeSeedTryToWarpLink_hook(GB *gb);
void seedItemCheckDiagonalCollision_hook(GB *gb);
void seedItemClearKnockback_hook(GB *gb);

static void seed_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t seed_jump_table(GB *gb) {
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

void seedItemDelete_hook(GB *gb) {
  CYC(0x4ec0, 0x4ec2); E = 0x02;
  CYC(0x4ec2, 0x4ec3); A = mem_rd(gb, DE);
  CYC(0x4ec3, 0x4ec4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4ec4, 0x4ec6);
    goto delete_item;
  }
  CYC(0x4ec4, 0x4ec6);
  CYC(0x4ec6, 0x4ec9); SET_HL(wIsSeedShooterInUse);
  CYC(0x4ec9, 0x4eca); A = mem_rd(gb, HL);
  CYC(0x4eca, 0x4ecb); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4ecb, 0x4ecd);
    goto delete_item;
  }
  CYC(0x4ecb, 0x4ecd);
  CYC(0x4ecd, 0x4ece); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

delete_item:
  CYC(0x4ece, 0x4ed1); itemDelete_hook(gb);
}

void seedItemState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ed1, 0x4ed3); E = 0x01;
  CYC(0x4ed3, 0x4ed4); A = mem_rd(gb, DE);
  CYC(0x4ed4, 0x4ed6); alu_sub(gb, 0x20);
  CYC(0x4ed6, 0x4ed7); push_effect(gb, 0x4ed7);
  switch (seed_jump_table(gb)) {
    case 0x4ee1: emberSeedBurn_hook(gb); return;
    case 0x4f14: seedUpdateAnimation_hook(gb); return;
    case 0x4f65: galeSeedUpdateAnimationAndCounter_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void emberSeedBurn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ee1, 0x4ee2); H = D;
  CYC(0x4ee2, 0x4ee4); L = 0x06;
  CYC(0x4ee4, 0x4ee5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x4ee5, 0x4ee7);
    goto break_tile;
  }
  CYC(0x4ee5, 0x4ee7);
  CALL_C(0x4ee7, itemAnimate_hook, 0x49d9, 0x4eea);
  CALL_C(0x4eea, itemUpdateDamageToApply_hook, 0x49c8, 0x4eed);
  CYC(0x4eed, 0x4eef); L = 0x21;
  CYC(0x4eef, 0x4ef0); B = mem_rd(gb, HL);
  if (F & FZ) {
    CYCT(0x4ef0, 0x4ef2);
    goto update_position;
  }
  CYC(0x4ef0, 0x4ef2);
  CYC(0x4ef2, 0x4ef4); L = 0x24;
  CYC(0x4ef4, 0x4ef6); mem_wr(gb, HL, 0x00);
  CYC(0x4ef6, 0x4ef8); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CYCT(0x4ef8, 0x4efa);
    goto delete_self;
  }
  CYC(0x4ef8, 0x4efa);

update_position:
  CYC(0x4efa, 0x4efc); L = 0x0e;
  CYC(0x4efc, 0x4efd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4efd, 0x4efe); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4efe, 0x4f00); C = 0x1c;
  if (!(F & FZ)) {
    CYCT(0x4f00, 0x4f03);
    objectUpdateSpeedZ_paramC_hook(gb);
    return;
  }
  CYC(0x4f00, 0x4f03);
  CYC(0x4f03, 0x4f05); alu_bit(gb, 6, B);
  if (F & FZ) {
    CYCT(0x4f05, 0x4f06); ret_effect(gb); return;
  }
  CYC(0x4f05, 0x4f06);
  CALL_C(0x4f06, objectCheckTileAtPositionIsWater_hook, 0x22a9, 0x4f09);
  if (F & FC) {
    CYCT(0x4f09, 0x4f0b);
    goto delete_self;
  }
  CYC(0x4f09, 0x4f0b);
  CYC(0x4f0b, 0x4f0c); ret_effect(gb);
  return;

break_tile:
  CYC(0x4f0c, 0x4f0e); A = 0x0c;
  CALL_C(0x4f0e, itemTryToBreakTile_hook, 0x2bef, 0x4f11);

delete_self:
  CYC(0x4f11, 0x4f14); seedItemDelete_hook(gb);
}

void seedUpdateAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f14, 0x4f16); E = 0x24;
  CYC(0x4f16, 0x4f17); alu_xor(gb, A);
  CYC(0x4f17, 0x4f18); mem_wr(gb, DE, A);
  CALL_C(0x4f18, itemAnimate_hook, 0x49d9, 0x4f1b);
  CYC(0x4f1b, 0x4f1d); E = 0x21;
  CYC(0x4f1d, 0x4f1e); A = mem_rd(gb, DE);
  CYC(0x4f1e, 0x4f1f); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x4f1f, 0x4f20); ret_effect(gb); return;
  }
  CYC(0x4f1f, 0x4f20);
  CYC(0x4f20, 0x4f23); seedItemDelete_hook(gb);
}

void seedItemState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f23, 0x4f25); E = 0x01;
  CYC(0x4f25, 0x4f26); A = mem_rd(gb, DE);
  CYC(0x4f26, 0x4f28); alu_sub(gb, 0x20);
  CYC(0x4f28, 0x4f29); push_effect(gb, 0x4f29);
  switch (seed_jump_table(gb)) {
    case 0x4ee1: emberSeedBurn_hook(gb); return;
    case 0x4f33: scentSeedSmell_hook(gb); return;
    case 0x4f14: seedUpdateAnimation_hook(gb); return;
    case 0x4f8c: galeSeedTryToWarpLink_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void scentSeedSmell_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f33, 0x4f34); H = D;
  CYC(0x4f34, 0x4f36); L = 0x06;
  CYC(0x4f36, 0x4f39); A = W8(wFrameCounter);
  CYC(0x4f39, 0x4f3a); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x4f3a, 0x4f3c);
  } else {
    CYC(0x4f3a, 0x4f3c);
    CYC(0x4f3c, 0x4f3d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) {
      CYCT(0x4f3d, 0x4f40);
      seedItemDelete_hook(gb);
      return;
    }
    CYC(0x4f3d, 0x4f40);
  }
  CYC(0x4f40, 0x4f41); A = mem_rd(gb, HL);
  CYC(0x4f41, 0x4f43); alu_cp(gb, 0x1e);
  if (!(F & FC)) {
    CYCT(0x4f43, 0x4f45);
  } else {
    CYC(0x4f43, 0x4f45);
    CYC(0x4f45, 0x4f47); L = 0x1a;
    CYC(0x4f47, 0x4f48); A = mem_rd(gb, HL);
    CYC(0x4f48, 0x4f4a); alu_xor(gb, 0x80);
    CYC(0x4f4a, 0x4f4b); mem_wr(gb, HL, A);
  }
  CYC(0x4f4b, 0x4f4d); L = 0x0b;
  CYC(0x4f4d, 0x4f4e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f4e, 0x4f50); H8(hFFB2) = A;
  CYC(0x4f50, 0x4f51); L = alu_inc8(gb, L);
  CYC(0x4f51, 0x4f52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f52, 0x4f54); H8(hFFB3) = A;
  CYC(0x4f54, 0x4f56); A = 0xff;
  CYC(0x4f56, 0x4f59); W8(wScentSeedActive) = A;
  CALL_C(0x4f59, itemAnimate_hook, 0x49d9, 0x4f5c);
  CALL_C(0x4f5c, bombPullTowardPoint_hook, 0x4a83, 0x4f5f);
  if (F & FC) {
    CYCT(0x4f5f, 0x4f62);
    seedItemDelete_hook(gb);
    return;
  }
  CYC(0x4f5f, 0x4f62);
  CYC(0x4f62, 0x4f65); itemUpdateSpeedZAndCheckHazards_hook(gb);
}

void galeSeedUpdateAnimationAndCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f65, galeSeedUpdateAnimation_hook, 0x4f79, 0x4f68);
  CALL_C(0x4f68, itemDecCounter1_hook, 0x23d6, 0x4f6b);
  if (F & FZ) {
    CYCT(0x4f6b, 0x4f6e);
    seedItemDelete_hook(gb);
    return;
  }
  CYC(0x4f6b, 0x4f6e);
  CYC(0x4f6e, 0x4f6f); A = mem_rd(gb, HL);
  CYC(0x4f6f, 0x4f71); alu_cp(gb, 0x14);
  if (!(F & FC)) {
    CYCT(0x4f71, 0x4f72); ret_effect(gb); return;
  }
  CYC(0x4f71, 0x4f72);
  CYC(0x4f72, 0x4f74); L = 0x1a;
  CYC(0x4f74, 0x4f75); A = mem_rd(gb, HL);
  CYC(0x4f75, 0x4f77); alu_xor(gb, 0x80);
  CYC(0x4f77, 0x4f78); mem_wr(gb, HL, A);
  CYC(0x4f78, 0x4f79); ret_effect(gb);
}

void galeSeedUpdateAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f79, itemAnimate_hook, 0x49d9, 0x4f7c);
  CYC(0x4f7c, 0x4f7e); E = 0x06;
  CYC(0x4f7e, 0x4f7f); A = mem_rd(gb, DE);
  CYC(0x4f7f, 0x4f81); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x4f81, 0x4f82); ret_effect(gb); return;
  }
  CYC(0x4f81, 0x4f82);
  CYC(0x4f82, 0x4f84); E = 0x1b;
  CYC(0x4f84, 0x4f85); A = mem_rd(gb, DE);
  CYC(0x4f85, 0x4f86); A = alu_inc8(gb, A);
  CYC(0x4f86, 0x4f88); alu_and(gb, 0x0b);
  CYC(0x4f88, 0x4f89); mem_wr(gb, DE, A);
  CYC(0x4f89, 0x4f8a); E = alu_inc8(gb, E);
  CYC(0x4f8a, 0x4f8b); mem_wr(gb, DE, A);
  CYC(0x4f8b, 0x4f8c); ret_effect(gb);
}

void galeSeedTryToWarpLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f8c, galeSeedUpdateAnimation_hook, 0x4f79, 0x4f8f);
  CYC(0x4f8f, 0x4f91); E = 0x05;
  CYC(0x4f91, 0x4f92); A = mem_rd(gb, DE);
  CYC(0x4f92, 0x4f93); push_effect(gb, 0x4f93);
  switch (seed_jump_table(gb)) {
    case 0x4f9b: goto substate0;
    case 0x4fe7: goto substate1;
    case 0x4ffd: goto substate2;
    case 0x5028: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4f9b, 0x4f9e); A = W8(wTilesetFlags);
  CYC(0x4f9e, 0x4f9f); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x4f9f, 0x4fa1);
    goto set_substate3;
  }
  CYC(0x4f9f, 0x4fa1);
  CYC(0x4fa1, 0x4fa4); A = W8(wWarpsDisabled);
  CYC(0x4fa4, 0x4fa5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4fa5, 0x4fa7);
    galeSeedUpdateAnimationAndCounter_hook(gb);
    return;
  }
  CYC(0x4fa5, 0x4fa7);
  CYC(0x4fa7, 0x4faa); A = W8(wLinkObjectIndex);
  CYC(0x4faa, 0x4fab); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x4fab, 0x4fad);
    galeSeedUpdateAnimationAndCounter_hook(gb);
    return;
  }
  CYC(0x4fab, 0x4fad);
  CYC(0x4fad, 0x4fb0); A = W8(wLinkGrabState2);
  CYC(0x4fb0, 0x4fb2); alu_and(gb, 0xf0);
  CYC(0x4fb2, 0x4fb4); alu_cp(gb, 0x40);
  if (F & FZ) {
    CYCT(0x4fb4, 0x4fb6);
    galeSeedUpdateAnimationAndCounter_hook(gb);
    return;
  }
  CYC(0x4fb4, 0x4fb6);
  CALL_C(0x4fb6, checkLinkVulnerableAndIDZero_hook, 0x1d20, 0x4fb9);
  if (!(F & FC)) {
    CYCT(0x4fb9, 0x4fbb);
    galeSeedUpdateAnimationAndCounter_hook(gb);
    return;
  }
  CYC(0x4fb9, 0x4fbb);
  CALL_C(0x4fbb, objectCheckCollidedWithLink_hook, 0x1c41, 0x4fbe);
  if (!(F & FC)) {
    CYCT(0x4fbe, 0x4fc0);
    galeSeedUpdateAnimationAndCounter_hook(gb);
    return;
  }
  CYC(0x4fbe, 0x4fc0);
  CYC(0x4fc0, 0x4fc3); SET_HL(w1Link);
  CALL_C(0x4fc3, objectTakePosition_hook, 0x2274, 0x4fc6);
  CYC(0x4fc6, 0x4fc8); E = 0x07;
  CYC(0x4fc8, 0x4fca); A = 0x3c;
  CYC(0x4fca, 0x4fcb); mem_wr(gb, DE, A);
  CYC(0x4fcb, 0x4fcd); E = 0x05;
  CYC(0x4fcd, 0x4fcf); A = 0x01;
  CYC(0x4fcf, 0x4fd0); mem_wr(gb, DE, A);
  CYC(0x4fd0, 0x4fd3); W8(wMenuDisabled) = A;
  CYC(0x4fd3, 0x4fd6); W8(wLinkCanPassNpcs) = A;
  CYC(0x4fd6, 0x4fd9); W8(wDisableScreenTransitions) = A;
  CYC(0x4fd9, 0x4fdb); A = 0x07;
  CYC(0x4fdb, 0x4fde); W8(wLinkForceState) = A;
  CYC(0x4fde, 0x4fe1);
  objectSetVisible80_hook(gb);
  return;

set_substate3:
  CYC(0x4fe1, 0x4fe3); E = 0x05;
  CYC(0x4fe3, 0x4fe5); A = 0x03;
  CYC(0x4fe5, 0x4fe6); mem_wr(gb, DE, A);
  CYC(0x4fe6, 0x4fe7); ret_effect(gb);
  return;

substate1:
  CYC(0x4fe7, 0x4fea); A = W8(wLinkDeathTrigger);
  CYC(0x4fea, 0x4feb); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4feb, 0x4fed);
    goto set_substate3;
  }
  CYC(0x4feb, 0x4fed);
  CYC(0x4fed, 0x4fee); H = D;
  CYC(0x4fee, 0x4ff0); L = 0x07;
  CYC(0x4ff0, 0x4ff1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x4ff1, 0x4ff3);
    goto begin_rising;
  }
  CYC(0x4ff1, 0x4ff3);
  CYC(0x4ff3, 0x4ff6); A = W8(wActiveGroup);
  CYC(0x4ff6, 0x4ff7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4ff7, 0x4ff9);
    goto flicker_and_copy;
  }
  CYC(0x4ff7, 0x4ff9);
  CYC(0x4ff9, 0x4ffa); ret_effect(gb);
  return;

begin_rising:
  CYC(0x4ffa, 0x4ffc); A = 0x02;
  CYC(0x4ffc, 0x4ffd); mem_wr(gb, DE, A);

substate2:
  CYC(0x4ffd, 0x4ffe); H = D;
  CYC(0x4ffe, 0x5000); L = 0x0f;
  CYC(0x5000, 0x5001); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5001, 0x5002); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5002, 0x5004); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x5004, 0x5006);
    goto flicker_and_copy;
  }
  CYC(0x5004, 0x5006);
  CYC(0x5006, 0x5008); A = 0x02;
  CYC(0x5008, 0x500b); mem_wr(gb, w1Link_substate, A);
  CYC(0x500b, 0x500d); A = 0x16;
  CYC(0x500d, 0x5010); W8(wCutsceneTrigger) = A;
  CYC(0x5010, 0x5012); A = 0x05;
  CALL_C(0x5012, openMenu_hook, 0x1ab0, 0x5015);
  CYC(0x5015, 0x5018);
  seedItemDelete_hook(gb);
  return;

flicker_and_copy:
  CYC(0x5018, 0x501a); E = 0x1a;
  CYC(0x501a, 0x501b); A = mem_rd(gb, DE);
  CYC(0x501b, 0x501d); alu_xor(gb, 0x80);
  CYC(0x501d, 0x501e); mem_wr(gb, DE, A);
  CYC(0x501e, 0x501f); alu_xor(gb, A);
  CYC(0x501f, 0x5022); W8(wLinkSwimmingState) = A;
  CYC(0x5022, 0x5025); SET_HL(w1Link);
  CYC(0x5025, 0x5028);
  objectCopyPosition_hook(gb);
  return;

substate3:
  CALL_C(0x5028, itemDecCounter2_hook, 0x23db, 0x502b);
  if (F & FZ) {
    CYCT(0x502b, 0x502e);
    seedItemDelete_hook(gb);
    return;
  }
  CYC(0x502b, 0x502e);
  CYC(0x502e, 0x5030); L = 0x1a;
  CYC(0x5030, 0x5031); A = mem_rd(gb, HL);
  CYC(0x5031, 0x5033); alu_xor(gb, 0x80);
  CYC(0x5033, 0x5034); mem_wr(gb, HL, A);
  CYC(0x5034, 0x5035); ret_effect(gb);
}

void seedItemUpdateBouncing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5035, objectGetTileAtPosition_hook, 0x1444, 0x5038);
  CYC(0x5038, 0x503b); SET_HL(0x5134);
  CALL_C(0x503b, findByteInCollisionTable_hook, 0x1e29, 0x503e);
  if (F & FC) {
    CYCT(0x503e, 0x5040);
    goto unset_zero;
  }
  CYC(0x503e, 0x5040);
  CYC(0x5040, 0x5042); E = 0x09;
  CYC(0x5042, 0x5043); A = mem_rd(gb, DE);
  CYC(0x5043, 0x5045); alu_bit(gb, 2, A);
  if (F & FZ) {
    CYCT(0x5045, 0x5047);
    goto moving_straight;
  }
  CYC(0x5045, 0x5047);
  CALL_C(0x5047, seedItemCheckDiagonalCollision_hook, 0x5099, 0x504a);
  CYC(0x504a, 0x504b); push_effect(gb, AF);
  CALL_C(0x504b, itemCheckCanPassSolidTile_hook, 0x4b95, 0x504e);
  CYC(0x504e, 0x504f); SET_AF(pop_effect(gb));
  if (F & FZ) {
    CYCT(0x504f, 0x5051);
    goto set_zero;
  }
  CYC(0x504f, 0x5051);
  CYC(0x5051, 0x5053);
  goto bounce;

moving_straight:
  CYC(0x5053, 0x5055); E = 0x33;
  CYC(0x5055, 0x5056); alu_xor(gb, A);
  CYC(0x5056, 0x5057); mem_wr(gb, DE, A);
  CALL_C(0x5057, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x505a);
  if (!(F & FC)) {
    CYCT(0x505a, 0x505c);
    goto set_zero;
  }
  CYC(0x505a, 0x505c);
  CYC(0x505c, 0x505e); E = 0x33;
  CYC(0x505e, 0x5060); A = 0x03;
  CYC(0x5060, 0x5061); mem_wr(gb, DE, A);
  CALL_C(0x5061, itemCheckCanPassSolidTile_hook, 0x4b95, 0x5064);
  if (F & FZ) {
    CYCT(0x5064, 0x5066);
    goto set_zero;
  }
  CYC(0x5064, 0x5066);

bounce:
  CALL_C(0x5066, seedItemClearKnockback_hook, 0x510f, 0x5069);
  CYC(0x5069, 0x506a); H = D;
  CYC(0x506a, 0x506c); L = 0x34;
  CYC(0x506c, 0x506d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x506d, 0x506f);
    goto unset_zero;
  }
  CYC(0x506d, 0x506f);
  CYC(0x506f, 0x5071); L = 0x33;
  CYC(0x5071, 0x5072); A = mem_rd(gb, HL);
  CYC(0x5072, 0x5074); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x5074, 0x5076);
    goto reverse_both;
  }
  CYC(0x5074, 0x5076);
  CYC(0x5076, 0x5077); C = A;
  CYC(0x5077, 0x5079); E = 0x09;
  CYC(0x5079, 0x507a); A = mem_rd(gb, DE);
  CYC(0x507a, 0x507b); alu_rrca(gb);
  CYC(0x507b, 0x507c); alu_rrca(gb);
  CYC(0x507c, 0x507e); alu_and(gb, 0x06);
  CYC(0x507e, 0x507f); alu_add(gb, C);
  CYC(0x507f, 0x5082); SET_HL(0x5090);
  CYC(0x5082, 0x5083); seed_add_a_to_hl_from_rst(gb, 0x5083);
  CYC(0x5083, 0x5084); A = mem_rd(gb, HL);
  CYC(0x5084, 0x5085); mem_wr(gb, DE, A);

set_zero:
  CYC(0x5085, 0x5086); alu_xor(gb, A);
  CYC(0x5086, 0x5087); ret_effect(gb);
  return;

reverse_both:
  CYC(0x5087, 0x5089); L = 0x09;
  CYC(0x5089, 0x508a); A = mem_rd(gb, HL);
  CYC(0x508a, 0x508c); alu_xor(gb, 0x10);
  CYC(0x508c, 0x508d); mem_wr(gb, HL, A);
  CYC(0x508d, 0x508e); alu_xor(gb, A);
  CYC(0x508e, 0x508f); ret_effect(gb);
  return;

unset_zero:
  CYC(0x508f, 0x5090); alu_or(gb, D);
  CYC(0x5090, 0x5091); ret_effect(gb);
}

void seedItemCheckDiagonalCollision_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5099, 0x509a); alu_rrca(gb);
  CYC(0x509a, 0x509c); alu_and(gb, 0x0c);
  CYC(0x509c, 0x509f); SET_HL(0x50e4);
  CYC(0x509f, 0x50a0); seed_add_a_to_hl_from_rst(gb, 0x50a0);
  CYC(0x50a0, 0x50a1); alu_xor(gb, A);
  CYC(0x50a1, 0x50a3); H8(hFF8A) = A;
  CYC(0x50a3, 0x50a5); E = 0x33;
  CYC(0x50a5, 0x50a7); A = 0x40;
  CYC(0x50a7, 0x50a8); mem_wr(gb, DE, A);

next_component:
  CYC(0x50a8, 0x50aa); E = 0x0b;
  CYC(0x50aa, 0x50ab); A = mem_rd(gb, DE);
  CYC(0x50ab, 0x50ac); alu_add(gb, mem_rd(gb, HL));
  CYC(0x50ac, 0x50ad); B = A;
  CYC(0x50ad, 0x50ae); SET_HL(HL + 1);
  CYC(0x50ae, 0x50b0); E = 0x0d;
  CYC(0x50b0, 0x50b1); A = mem_rd(gb, DE);
  CYC(0x50b1, 0x50b2); alu_add(gb, mem_rd(gb, HL));
  CYC(0x50b2, 0x50b3); C = A;
  CYC(0x50b3, 0x50b4); SET_HL(HL + 1);
  CYC(0x50b4, 0x50b5); push_effect(gb, HL);
  CALL_C(0x50b5, checkTileCollisionAt_allowHoles_hook, 0x14d1, 0x50b8);
  if (!(F & FC)) {
    CYCT(0x50b8, 0x50ba);
    goto next;
  }
  CYC(0x50b8, 0x50ba);
  CALL_C(0x50ba, getTileAtPosition_hook, 0x1447, 0x50bd);
  CYC(0x50bd, 0x50c0); SET_HL(0x5134);
  CALL_C(0x50c0, findByteInCollisionTable_hook, 0x1e29, 0x50c3);
  CYC(0x50c3, 0x50c4); alu_ccf(gb);
  if (!(F & FC)) {
    CYCT(0x50c4, 0x50c6);
    goto next;
  }
  CYC(0x50c4, 0x50c6);
  CYC(0x50c6, 0x50c7); H = D;
  CYC(0x50c7, 0x50c9); L = 0x09;
  CYC(0x50c9, 0x50ca); B = mem_rd(gb, HL);
  CALL_C(0x50ca, checkTileIsPassableFromDirection_hook, 0x4bbf, 0x50cd);
  CYC(0x50cd, 0x50ce); alu_ccf(gb);
  if (F & FC) {
    CYCT(0x50ce, 0x50d0);
    goto next;
  }
  CYC(0x50ce, 0x50d0);
  if (F & FZ) {
    CYCT(0x50d0, 0x50d2);
    goto next;
  }
  CYC(0x50d0, 0x50d2);
  CYC(0x50d2, 0x50d3); H = D;
  CYC(0x50d3, 0x50d5); L = 0x3e;
  CYC(0x50d5, 0x50d6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x50d6, 0x50d7); alu_rlca(gb);

next:
  CYC(0x50d7, 0x50d8); H = D;
  CYC(0x50d8, 0x50da); L = 0x33;
  CYC(0x50da, 0x50dc); mem_wr(gb, HL, alu_rl(gb, mem_rd(gb, HL)));
  CYC(0x50dc, 0x50dd); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x50dd, 0x50df);
    goto next_component;
  }
  CYC(0x50dd, 0x50df);
  CYC(0x50df, 0x50e1); E = 0x33;
  CYC(0x50e1, 0x50e2); A = mem_rd(gb, DE);
  CYC(0x50e2, 0x50e3); alu_or(gb, A);
  CYC(0x50e3, 0x50e4); ret_effect(gb);
}

void func_50f4_hook(GB *gb) {
  CYC(0x50f4, 0x50f6); E = 0x09;
  CYC(0x50f6, 0x50f8); L = 0x2c;
  CYC(0x50f8, 0x50f9); A = mem_rd(gb, DE);
  CYC(0x50f9, 0x50fa); alu_add(gb, mem_rd(gb, HL));
  CYC(0x50fa, 0x50fd); SET_HL(0x5114);
  CYC(0x50fd, 0x50fe); seed_add_a_to_hl_from_rst(gb, 0x50fe);
  CYC(0x50fe, 0x50ff); C = mem_rd(gb, HL);
  CYC(0x50ff, 0x5100); A = mem_rd(gb, DE);
  CYC(0x5100, 0x5101); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x5101, 0x5103);
    seedItemClearKnockback_hook(gb);
    return;
  }
  CYC(0x5101, 0x5103);
  CYC(0x5103, 0x5104); H = D;
  CYC(0x5104, 0x5106); L = 0x34;
  CYC(0x5106, 0x5107); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x5107, 0x5109);
    goto unset_zero;
  }
  CYC(0x5107, 0x5109);
  CYC(0x5109, 0x510a); A = C;
  CYC(0x510a, 0x510b); mem_wr(gb, DE, A);
  CYC(0x510b, 0x510c); alu_xor(gb, A);
  CYC(0x510c, 0x510d); ret_effect(gb);
  return;

unset_zero:
  CYC(0x510d, 0x510e); alu_or(gb, D);
  CYC(0x510e, 0x510f); ret_effect(gb);
}

void seedItemClearKnockback_hook(GB *gb) {
  CYC(0x510f, 0x5111); E = 0x2d;
  CYC(0x5111, 0x5112); alu_xor(gb, A);
  CYC(0x5112, 0x5113); mem_wr(gb, DE, A);
  CYC(0x5113, 0x5114); ret_effect(gb);
}

static void seed_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void seed_state1_load_gfx_vars(GB *gb) {
  CYC(0x4e8a, 0x4e8b); alu_add(gb, A);
  CYC(0x4e8b, 0x4e8e); SET_HL(0x4e20);
  CYC(0x4e8e, 0x4e8f); seed_add_double_index_from_rst(gb, 0x4e8f);
  CYC(0x4e8f, 0x4e91); E = 0x1b;
  CYC(0x4e91, 0x4e92); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4e92, 0x4e93); mem_wr(gb, DE, A);
  CYC(0x4e93, 0x4e94); E = alu_inc8(gb, E);
  CYC(0x4e94, 0x4e95); mem_wr(gb, DE, A);
  CYC(0x4e95, 0x4e96); E = alu_inc8(gb, E);
  CYC(0x4e96, 0x4e97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4e97, 0x4e98); mem_wr(gb, DE, A);
  CYC(0x4e98, 0x4e99); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4e99, 0x4e9b); E = 0x06;
  CYC(0x4e9b, 0x4e9c); mem_wr(gb, DE, A);
  CYC(0x4e9c, 0x4e9d); A = mem_rd(gb, HL);
  CYC(0x4e9d, 0x4ea0); playSound_b00_hook(gb);
}

static void seed_state1_init_state3(GB *gb) {
  CYC(0x4e82, 0x4e84); E = 0x04;
  CYC(0x4e84, 0x4e86); A = 0x03;
  CYC(0x4e86, 0x4e87); mem_wr(gb, DE, A);
  CYC(0x4e87, 0x4e89); E = 0x01;
  CYC(0x4e89, 0x4e8a); A = mem_rd(gb, DE);
  seed_state1_load_gfx_vars(gb);
}

static void seed_state1_break_tile_with_gale_seed(GB *gb) {
  CYC(0x4e45, 0x4e47); A = 0x0d;
  CYC(0x4e47, 0x4e4a); itemTryToBreakTile_hook(gb);
}

void seedItemState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d85, itemUpdateDamageToApply_hook, 0x49c8, 0x4d88);
  if (F & FZ) {
    CYCT(0x4d88, 0x4d8a);
    goto no_collision;
  }
  CYC(0x4d88, 0x4d8a);
  CYC(0x4d8a, 0x4d8c); alu_bit(gb, 4, A);
  if (F & FZ) {
    CYCT(0x4d8c, 0x4d8e);
    goto collided_with_enemy;
  }
  CYC(0x4d8c, 0x4d8e);
  CYC(0x4d8e, 0x4d90); mem_wr(gb, HL, 0x00);
  CALL_C(0x4d90, func_50f4_hook, 0x50f4, 0x4d93);
  if (F & FZ) {
    CYCT(0x4d93, 0x4d95);
    goto update_position;
  }
  CYC(0x4d93, 0x4d95);
  CYC(0x4d95, 0x4d97);
  goto collided_with_wall;

no_collision:
  CYC(0x4d97, 0x4d99); E = 0x02;
  CYC(0x4d99, 0x4d9a); A = mem_rd(gb, DE);
  CYC(0x4d9a, 0x4d9b); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4d9b, 0x4d9d);
    goto satchel_update;
  }
  CYC(0x4d9b, 0x4d9d);
  CALL_C(0x4d9d, seedItemUpdateBouncing_hook, 0x5035, 0x4da0);
  if (!(F & FZ)) {
    CYCT(0x4da0, 0x4da2);
    goto collided_with_wall;
  }
  CYC(0x4da0, 0x4da2);

update_position:
  CALL_C(0x4da2, objectCheckWithinRoomBoundary_hook, 0x219f, 0x4da5);
  if (F & FC) {
    CYCT(0x4da5, 0x4da8);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x4da5, 0x4da8);
  CYC(0x4da8, 0x4dab); seedItemDelete_hook(gb);
  return;

satchel_update:
  CYC(0x4dab, 0x4dac); H = D;
  CYC(0x4dac, 0x4dae); L = 0x3b;
  CYC(0x4dae, 0x4db0); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4db0, 0x4db2);
  } else {
    CYC(0x4db0, 0x4db2);
    CYC(0x4db2, 0x4db4); L = 0x10;
    CYC(0x4db4, 0x4db6); mem_wr(gb, HL, 0x00);
  }
  CALL_C(0x4db6, objectCheckWithinRoomBoundary_hook, 0x219f, 0x4db9);
  if (!(F & FC)) {
    CYCT(0x4db9, 0x4dbc);
    seedItemDelete_hook(gb);
    return;
  }
  CYC(0x4db9, 0x4dbc);
  CALL_C(0x4dbc, objectApplySpeed_hook, 0x201d, 0x4dbf);
  CYC(0x4dbf, 0x4dc1); C = 0x1c;
  CALL_C(0x4dc1, itemUpdateThrowingVerticallyAndCheckHazards_hook, 0x4b3c, 0x4dc4);
  if (F & FC) {
    CYCT(0x4dc4, 0x4dc7);
    seedItemDelete_hook(gb);
    return;
  }
  CYC(0x4dc4, 0x4dc7);
  if (F & FZ) {
    CYCT(0x4dc7, 0x4dc8); ret_effect(gb); return;
  }
  CYC(0x4dc7, 0x4dc8);
  CYC(0x4dc8, 0x4dca); A = 0x52;
  CALL_C(0x4dca, playSound_b00_hook, 0x0c98, 0x4dcd);
  CALL_C(0x4dcd, itemAnimate_hook, 0x49d9, 0x4dd0);
  CYC(0x4dd0, 0x4dd2); E = 0x01;
  CYC(0x4dd2, 0x4dd3); A = mem_rd(gb, DE);
  CYC(0x4dd3, 0x4dd5); alu_sub(gb, 0x20);
  CYC(0x4dd5, 0x4dd6); push_effect(gb, 0x4dd6);
  switch (seed_jump_table(gb)) {
    case 0x4e0a: goto ember_standard;
    case 0x4e10: goto scent_landed;
    case 0x4ec0: seedItemDelete_hook(gb); return;
    case 0x4e2c: goto gale_landed;
    case 0x4e7b: goto mystery_standard;
    default: hook_continue(gb, HL, sp0_); return;
  }

collided_with_wall:
  CALL_C(0x4de0, itemAnimate_hook, 0x49d9, 0x4de3);
  CYC(0x4de3, 0x4de5); E = 0x01;
  CYC(0x4de5, 0x4de6); A = mem_rd(gb, DE);
  CYC(0x4de6, 0x4de8); alu_sub(gb, 0x20);
  CYC(0x4de8, 0x4de9); push_effect(gb, 0x4de9);
  switch (seed_jump_table(gb)) {
    case 0x4e0a: goto ember_standard;
    case 0x4e26: goto scent_or_pegasus_collided;
    case 0x4e4a: goto gale_collided_with_wall;
    case 0x4e7b: goto mystery_standard;
    default: hook_continue(gb, HL, sp0_); return;
  }

collided_with_enemy:
  CALL_C(0x4df3, itemAnimate_hook, 0x49d9, 0x4df6);
  CYC(0x4df6, 0x4df8); E = 0x24;
  CYC(0x4df8, 0x4df9); alu_xor(gb, A);
  CYC(0x4df9, 0x4dfa); mem_wr(gb, DE, A);
  CYC(0x4dfa, 0x4dfc); E = 0x01;
  CYC(0x4dfc, 0x4dfd); A = mem_rd(gb, DE);
  CYC(0x4dfd, 0x4dff); alu_sub(gb, 0x20);
  CYC(0x4dff, 0x4e00); push_effect(gb, 0x4e00);
  switch (seed_jump_table(gb)) {
    case 0x4e0a: goto ember_standard;
    case 0x4e26: goto scent_or_pegasus_collided;
    case 0x4ec0: seedItemDelete_hook(gb); return;
    case 0x4e5e: goto mystery_collided_with_enemy;
    default: hook_continue(gb, HL, sp0_); return;
  }

ember_standard:
  CYC(0x4e0a, 0x4e0d); push_effect(gb, 0x4e0d); seed_state1_init_state3(gb);
  CYC(0x4e0d, 0x4e10); objectSetVisible82_hook(gb);
  return;

scent_landed:
  CYC(0x4e10, 0x4e12); A = 0x27;
  CYC(0x4e12, 0x4e15); push_effect(gb, 0x4e15); seed_state1_load_gfx_vars(gb);
  CYC(0x4e15, 0x4e17); A = 0x02;
  CALL_C(0x4e17, itemSetState_hook, 0x2a7e, 0x4e1a);
  CYC(0x4e1a, 0x4e1c); L = 0x24;
  CYC(0x4e1c, 0x4e1e); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(0x4e1e, 0x4e20); A = 0x01;
  CALL_C(0x4e20, itemSetAnimation_hook, 0x49e2, 0x4e23);
  CYC(0x4e23, 0x4e26); objectSetVisible83_hook(gb);
  return;

scent_or_pegasus_collided:
  CYC(0x4e26, 0x4e28); E = 0x24;
  CYC(0x4e28, 0x4e29); alu_xor(gb, A);
  CYC(0x4e29, 0x4e2a); mem_wr(gb, DE, A);
  CYC(0x4e2a, 0x4e2c);
  goto init_state3;

gale_landed:
  CYC(0x4e2c, 0x4e2f); push_effect(gb, 0x4e2f); seed_state1_break_tile_with_gale_seed(gb);
  CYC(0x4e2f, 0x4e31); A = 0x25;
  CYC(0x4e31, 0x4e34); push_effect(gb, 0x4e34); seed_state1_load_gfx_vars(gb);
  CYC(0x4e34, 0x4e36); A = 0x02;
  CALL_C(0x4e36, itemSetState_hook, 0x2a7e, 0x4e39);
  CYC(0x4e39, 0x4e3b); L = 0x24;
  CYC(0x4e3b, 0x4e3c); alu_xor(gb, A);
  CYC(0x4e3c, 0x4e3d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e3d, 0x4e3e); L = alu_inc8(gb, L);
  CYC(0x4e3e, 0x4e40); A = 0x02;
  CYC(0x4e40, 0x4e41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e41, 0x4e42); mem_wr(gb, HL, A);
  CYC(0x4e42, 0x4e45); objectSetVisible82_hook(gb);
  return;

gale_collided_with_wall:
  CYC(0x4e4a, 0x4e4d); push_effect(gb, 0x4e4d); seed_state1_break_tile_with_gale_seed(gb);
  CYC(0x4e4d, 0x4e4f); A = 0x26;
  CYC(0x4e4f, 0x4e52); push_effect(gb, 0x4e52); seed_state1_load_gfx_vars(gb);
  CYC(0x4e52, 0x4e54); A = 0x03;
  CALL_C(0x4e54, itemSetState_hook, 0x2a7e, 0x4e57);
  CYC(0x4e57, 0x4e59); L = 0x24;
  CYC(0x4e59, 0x4e5b); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(0x4e5b, 0x4e5e); objectSetVisible82_hook(gb);
  return;

mystery_collided_with_enemy:
  CYC(0x4e5e, 0x4e5f); H = D;
  CYC(0x4e5f, 0x4e61); L = 0x2a;
  CYC(0x4e61, 0x4e63); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4e63, 0x4e65);
    goto mystery_standard;
  }
  CYC(0x4e63, 0x4e65);
  CYC(0x4e65, 0x4e67); L = 0x03;
  CYC(0x4e67, 0x4e68); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4e68, 0x4e6a); alu_add(gb, 0x20);
  CYC(0x4e6a, 0x4e6b); L = alu_dec8(gb, L);
  CYC(0x4e6b, 0x4e6c); mem_wr(gb, HL, A);
  CALL_C(0x4e6c, itemLoadAttributesAndGraphics_hook, 0x4993, 0x4e6f);
  CYC(0x4e6f, 0x4e70); alu_xor(gb, A);
  CALL_C(0x4e70, itemSetAnimation_hook, 0x49e2, 0x4e73);
  CYC(0x4e73, 0x4e75); E = 0x29;
  CYC(0x4e75, 0x4e77); A = 0xff;
  CYC(0x4e77, 0x4e78); mem_wr(gb, DE, A);
  CYC(0x4e78, 0x4e7b);
  goto collided_with_enemy;

mystery_standard:
  CYC(0x4e7b, 0x4e7d); E = 0x24;
  CYC(0x4e7d, 0x4e7e); alu_xor(gb, A);
  CYC(0x4e7e, 0x4e7f); mem_wr(gb, DE, A);
  CALL_C(0x4e7f, objectSetVisible82_hook, 0x1e69, 0x4e82);

init_state3:
  seed_state1_init_state3(gb);
}

void itemCode20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ced, 0x4cef); E = 0x04;
  CYC(0x4cef, 0x4cf0); A = mem_rd(gb, DE);
  CYC(0x4cf0, 0x4cf1); push_effect(gb, 0x4cf1);
  switch (seed_jump_table(gb)) {
    case 0x4cf9: break;
    case 0x4d85: seedItemState1_hook(gb); return;
    case 0x4f23: seedItemState2_hook(gb); return;
    case 0x4ed1: seedItemState3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x4cf9, itemLoadAttributesAndGraphics_hook, 0x4993, 0x4cfc);
  CYC(0x4cfc, 0x4cfd); alu_xor(gb, A);
  CALL_C(0x4cfd, itemSetAnimation_hook, 0x49e2, 0x4d00);
  CALL_C(0x4d00, objectSetVisiblec1_hook, 0x1e3c, 0x4d03);
  CALL_C(0x4d03, itemIncState_hook, 0x23ea, 0x4d06);
  CYC(0x4d06, 0x4d09); SET_BC(0xffe0);
  CALL_C(0x4d09, objectSetSpeedZ_hook, 0x239d, 0x4d0c);
  CYC(0x4d0c, 0x4d0e); L = 0x02;
  CYC(0x4d0e, 0x4d0f); A = mem_rd(gb, HL);
  CYC(0x4d0f, 0x4d10); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(0x4d10, itemUpdateAngle_hook, 0x2cf0, 0x4d13);
  else CYC(0x4d10, 0x4d13);
  CYC(0x4d13, 0x4d15); L = 0x34;
  CYC(0x4d15, 0x4d17); mem_wr(gb, HL, 0x03);
  CYC(0x4d17, 0x4d19); L = 0x02;
  CYC(0x4d19, 0x4d1a); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4d1a, 0x4d1b); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4d1b, 0x4d1d);
    goto shooter;
  }
  CYC(0x4d1b, 0x4d1d);
  CYC(0x4d1d, 0x4d1e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4d1e, 0x4d20); alu_cp(gb, 0x23);
  if (!(F & FZ)) {
    CYCT(0x4d20, 0x4d22);
    CYC(0x4d2d, 0x4d30); SET_HL(0x4d69);
    CALL_C(0x4d30, applyOffsetTableHL_hook, 0x4a36, 0x4d33);
    CYC(0x4d33, 0x4d35); A = 0x1e;
    CYC(0x4d35, 0x4d37);
    goto set_speed;
  }
  CYC(0x4d20, 0x4d22);
  CYC(0x4d22, 0x4d24); L = 0x0f;
  CYC(0x4d24, 0x4d25); A = mem_rd(gb, HL);
  CYC(0x4d25, 0x4d27); alu_add(gb, 0xf8);
  CYC(0x4d27, 0x4d28); mem_wr(gb, HL, A);
  CYC(0x4d28, 0x4d2a); L = 0x09;
  CYC(0x4d2a, 0x4d2c); mem_wr(gb, HL, 0xff);
  CYC(0x4d2c, 0x4d2d); ret_effect(gb);
  return;

shooter:
  CYC(0x4d37, 0x4d39); E = 0x09;
  CYC(0x4d39, 0x4d3a); A = mem_rd(gb, DE);
  CYC(0x4d3a, 0x4d3b); alu_rrca(gb);
  CYC(0x4d3b, 0x4d3e); SET_HL(0x4d75);
  CYC(0x4d3e, 0x4d3f); seed_add_a_to_hl_from_rst(gb, 0x4d3f);
  CYC(0x4d3f, 0x4d40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4d40, 0x4d41); C = mem_rd(gb, HL);
  CYC(0x4d41, 0x4d42); B = A;
  CYC(0x4d42, 0x4d43); H = D;
  CYC(0x4d43, 0x4d45); L = 0x0f;
  CYC(0x4d45, 0x4d46); A = mem_rd(gb, HL);
  CYC(0x4d46, 0x4d48); alu_add(gb, 0xfe);
  CYC(0x4d48, 0x4d49); mem_wr(gb, HL, A);
  CALL_C(0x4d49, objectCopyPositionWithOffset_hook, 0x225a, 0x4d4c);
  CYC(0x4d4c, 0x4d4f); SET_HL(wIsSeedShooterInUse);
  CYC(0x4d4f, 0x4d50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d50, 0x4d52); A = 0x78;

set_speed:
  CYC(0x4d52, 0x4d54); E = 0x10;
  CYC(0x4d54, 0x4d55); mem_wr(gb, DE, A);
  CYC(0x4d55, 0x4d57); E = 0x01;
  CYC(0x4d57, 0x4d58); A = mem_rd(gb, DE);
  CYC(0x4d58, 0x4d5a); alu_cp(gb, 0x24);
  if (!(F & FZ)) {
    CYCT(0x4d5a, 0x4d5b); ret_effect(gb); return;
  }
  CYC(0x4d5a, 0x4d5b);
  CALL_C(0x4d5b, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4d5e);
  CYC(0x4d5e, 0x4d60); alu_and(gb, 0x03);
  CYC(0x4d60, 0x4d62); E = 0x03;
  CYC(0x4d62, 0x4d63); mem_wr(gb, DE, A);
  CYC(0x4d63, 0x4d65); alu_add(gb, 0x9b);
  CYC(0x4d65, 0x4d67); E = 0x24;
  CYC(0x4d67, 0x4d68); mem_wr(gb, DE, A);
  CYC(0x4d68, 0x4d69); ret_effect(gb);
}
