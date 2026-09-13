#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void bombUpdateThrowingVerticallyAndCheckDelete_hook(GB *gb);
void itemUpdateExplosion_hook(GB *gb);
void bombUpdateExplosion_hook(GB *gb);
void bombUpdateAnimation_hook(GB *gb);
void itemInitializeBombExplosion_hook(GB *gb);
void bombInitializeIfNeeded_hook(GB *gb);
void bombResetAnimationAndSetVisiblec1_hook(GB *gb);
void explosionCheckAndApplyLinkCollision_hook(GB *gb);
void explosionTryToBreakNextTile_hook(GB *gb);
void bombUpdateThrowingLaterally_hook(GB *gb);
void itemBounce_hook(GB *gb);
void itemBeginThrow_hook(GB *gb);
void itemUpdateThrowingLaterally_hook(GB *gb);

static void bomb_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t bomb_jump_table(GB *gb) {
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

void itemCode03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54a0, 0x54a2); E = 0x2f;
  CYC(0x54a2, 0x54a3); A = mem_rd(gb, DE);
  CYC(0x54a3, 0x54a5); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x54a5, 0x54a7);
    goto delete_bomb;
  }
  CYC(0x54a5, 0x54a7);
  CYC(0x54a7, 0x54a9); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x54a9, 0x54ac);
    bombResetAnimationAndSetVisiblec1_hook(gb);
    return;
  }
  CYC(0x54a9, 0x54ac);
  CYC(0x54ac, 0x54ae); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(0x54ae, 0x54b1);
    bombUpdateExplosion_hook(gb);
    return;
  }
  CYC(0x54ae, 0x54b1);
  CYC(0x54b1, 0x54b3); E = 0x04;
  CYC(0x54b3, 0x54b4); A = mem_rd(gb, DE);
  CYC(0x54b4, 0x54b5); push_effect(gb, 0x54b5);
  switch (bomb_jump_table(gb)) {
    case 0x54dd: goto held_state;
    case 0x54cb: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

delete_bomb:
  CYC(0x54bb, 0x54bc); H = D;
  CYC(0x54bc, 0x54be); L = 0x04;
  CYC(0x54be, 0x54bf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54bf, 0x54c1); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x54c1, 0x54c3);
  } else {
    CYC(0x54c1, 0x54c3);
    CYC(0x54c3, 0x54c5); alu_bit(gb, 1, mem_rd(gb, HL));
    if (F & FZ) CALL_C_CC(0x54c5, dropLinkHeldItem_hook, 0x2c43, 0x54c8);
    else CYC(0x54c5, 0x54c8);
  }
  CYC(0x54c8, 0x54cb);
  itemDelete_hook(gb);
  return;

state1:
  CYC(0x54cb, 0x54cd); C = 0x20;
  CALL_C(0x54cd, bombUpdateThrowingVerticallyAndCheckDelete_hook, 0x5530, 0x54d0);
  if (F & FC) {
    CYCT(0x54d0, 0x54d1); ret_effect(gb); return;
  }
  CYC(0x54d0, 0x54d1);
  CALL_C(0x54d1, bombPullTowardPoint_hook, 0x4a83, 0x54d4);
  if (F & FC) {
    CYCT(0x54d4, 0x54d7);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x54d4, 0x54d7);
  CALL_C(0x54d7, itemUpdateConveyorBelt_hook, 0x4bf7, 0x54da);
  CYC(0x54da, 0x54dd);
  bombUpdateAnimation_hook(gb);
  return;

held_state:
  CYC(0x54dd, 0x54df); E = 0x05;
  CYC(0x54df, 0x54e0); A = mem_rd(gb, DE);
  CYC(0x54e0, 0x54e1); push_effect(gb, 0x54e1);
  switch (bomb_jump_table(gb)) {
    case 0x54e9: goto held_state0;
    case 0x54f7: goto held_state1;
    case 0x5506: goto held_state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

held_state0:
  CALL_C(0x54e9, itemIncSubstate_hook, 0x23ef, 0x54ec);
  CYC(0x54ec, 0x54ee); L = 0x2f;
  CYC(0x54ee, 0x54f0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x54f0, 0x54f2); L = 0x37;
  CYC(0x54f2, 0x54f4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  CALL_C(0x54f4, bombInitializeIfNeeded_hook, 0x55df, 0x54f7);

held_state1:
  CYC(0x54f7, 0x54f9); A = 0x3b;
  CALL_C(0x54f9, cpActiveRing_hook, 0x23b0, 0x54fc);
  if (F & FZ) {
    CYCT(0x54fc, 0x54ff);
    bombResetAnimationAndSetVisiblec1_hook(gb);
    return;
  }
  CYC(0x54fc, 0x54ff);
  CALL_C(0x54ff, bombUpdateAnimation_hook, 0x5597, 0x5502);
  if (F & FZ) {
    CYCT(0x5502, 0x5503); ret_effect(gb); return;
  }
  CYC(0x5502, 0x5503);
  CYC(0x5503, 0x5506);
  dropLinkHeldItem_hook(gb);
  return;

held_state2:
  CYC(0x5506, 0x5508); A = 0x03;
  CYC(0x5508, 0x5509); mem_wr(gb, DE, A);
  CALL_C(0x5509, bombUpdateThrowingLaterally_hook, 0x639c, 0x550c);
  CYC(0x550c, 0x550e); E = 0x39;
  CYC(0x550e, 0x550f); A = mem_rd(gb, DE);
  CYC(0x550f, 0x5510); C = A;
  CALL_C(0x5510, bombUpdateThrowingVerticallyAndCheckDelete_hook, 0x5530, 0x5513);
  if (F & FC) {
    CYCT(0x5513, 0x5514); ret_effect(gb); return;
  }
  CYC(0x5513, 0x5514);
  if (F & FZ) {
    CYCT(0x5514, 0x5516);
    goto update_animation;
  }
  CYC(0x5514, 0x5516);
  CALL_C(0x5516, itemBounce_hook, 0x6482, 0x5519);
  if (F & FC) {
    CYCT(0x5519, 0x551b);
    goto stopped_bouncing;
  }
  CYC(0x5519, 0x551b);
  CALL_C(0x551b, bombPullTowardPoint_hook, 0x4a83, 0x551e);
  if (F & FC) {
    CYCT(0x551e, 0x5521);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x551e, 0x5521);

update_animation:
  CYC(0x5521, 0x5524);
  bombUpdateAnimation_hook(gb);
  return;

stopped_bouncing:
  CYC(0x5524, 0x5525); H = D;
  CYC(0x5525, 0x5527); L = 0x04;
  CYC(0x5527, 0x5529); mem_wr(gb, HL, 0x01);
  CYC(0x5529, 0x552b); L = 0x2f;
  CYC(0x552b, 0x552d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(0x552d, 0x5530);
  bombUpdateAnimation_hook(gb);
}

void bombUpdateThrowingVerticallyAndCheckDelete_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5530, 0x5531); push_effect(gb, BC);
  CYC(0x5531, 0x5534); A = W8(wTilesetFlags);
  CYC(0x5534, 0x5536); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x5536, 0x5538);
    goto check_boundary;
  }
  CYC(0x5536, 0x5538);
  CYC(0x5538, 0x553a); E = 0x0b;
  CYC(0x553a, 0x553b); A = mem_rd(gb, DE);
  CYC(0x553b, 0x553d); alu_sub(gb, 0x08);
  CYC(0x553d, 0x553f); alu_cp(gb, 0xf0);
  CYC(0x553f, 0x5540); alu_ccf(gb);
  if (F & FC) {
    CYCT(0x5540, 0x5542);
    goto restore_bc;
  }
  CYC(0x5540, 0x5542);

check_boundary:
  CALL_C(0x5542, objectCheckWithinRoomBoundary_hook, 0x219f, 0x5545);

restore_bc:
  CYC(0x5545, 0x5546); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x5546, 0x5548);
    goto delete_bomb;
  }
  CYC(0x5546, 0x5548);
  CALL_C(0x5548, itemUpdateThrowingVerticallyAndCheckHazards_hook, 0x4b3c, 0x554b);
  if (!(F & FC)) {
    CYCT(0x554b, 0x554c); ret_effect(gb); return;
  }
  CYC(0x554b, 0x554c);
  CYC(0x554c, 0x554f); SET_BC(0x0050);
  CYC(0x554f, 0x5552); A = W8(wActiveGroup);
  CYC(0x5552, 0x5553); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5553, 0x5555);
    goto delete_bomb;
  }
  CYC(0x5553, 0x5555);
  CYC(0x5555, 0x5558); A = W8(wActiveRoom);
  CYC(0x5558, 0x5559); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x5559, 0x555b);
    goto delete_bomb;
  }
  CYC(0x5559, 0x555b);
  CYC(0x555b, 0x555d); A = 0x01;
  CYC(0x555d, 0x5560); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;

delete_bomb:
  CALL_C(0x5560, itemDelete_hook, 0x2ce2, 0x5563);
  CYC(0x5563, 0x5564); alu_scf(gb);
  CYC(0x5564, 0x5565); ret_effect(gb);
}

void itemUpdateExplosion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5565, 0x5566); H = D;
  CYC(0x5566, 0x5568); L = 0x21;
  CYC(0x5568, 0x5569); A = mem_rd(gb, HL);
  CYC(0x5569, 0x556b); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x556b, 0x556e);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x556b, 0x556e);
  CYC(0x556e, 0x5570); L = 0x24;
  CYC(0x5570, 0x5572); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(0x5572, 0x5574);
  } else {
    CYC(0x5572, 0x5574);
    CYC(0x5574, 0x5576); mem_wr(gb, HL, 0x00);
  }
  CYC(0x5576, 0x5577); C = mem_rd(gb, HL);
  CYC(0x5577, 0x5579); L = 0x26;
  CYC(0x5579, 0x557b); alu_and(gb, 0x1f);
  CYC(0x557b, 0x557c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x557c, 0x557d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x557d, 0x557f); alu_bit(gb, 7, C);
  if (!(F & FZ)) CALL_C_CC(0x557f, explosionCheckAndApplyLinkCollision_hook, 0x55f7, 0x5582);
  else CYC(0x557f, 0x5582);
  CYC(0x5582, 0x5583); H = D;
  CYC(0x5583, 0x5585); L = 0x06;
  CYC(0x5585, 0x5587); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(0x5587, explosionTryToBreakNextTile_hook, 0x563e, 0x558a);
  else CYC(0x5587, 0x558a);
  CYC(0x558a, 0x558d);
  itemAnimate_hook(gb);
}

void bombUpdateExplosion_hook(GB *gb) {
  CYC(0x558d, 0x558e); H = D;
  CYC(0x558e, 0x5590); L = 0x04;
  CYC(0x5590, 0x5591); A = mem_rd(gb, HL);
  CYC(0x5591, 0x5593); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x5593, 0x5595);
    itemInitializeBombExplosion_hook(gb);
    return;
  }
  CYC(0x5593, 0x5595);
  CYC(0x5595, 0x5597);
  itemUpdateExplosion_hook(gb);
}

void bombUpdateAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5597, itemAnimate_hook, 0x49d9, 0x559a);
  CYC(0x559a, 0x559c); E = 0x21;
  CYC(0x559c, 0x559d); A = mem_rd(gb, DE);
  CYC(0x559d, 0x559e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x559e, 0x559f); ret_effect(gb); return;
  }
  CYC(0x559e, 0x559f);
  itemInitializeBombExplosion_hook(gb);
}

void itemInitializeBombExplosion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x559f, 0x55a0); H = D;
  CYC(0x55a0, 0x55a2); L = 0x1b;
  CYC(0x55a2, 0x55a4); A = 0x0a;
  CYC(0x55a4, 0x55a5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x55a5, 0x55a6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x55a6, 0x55a8); mem_wr(gb, HL, 0x0c);
  CYC(0x55a8, 0x55aa); L = 0x24;
  CYC(0x55aa, 0x55ac); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x55ac, 0x55ae); A = 0x0c;
  CALL_C(0x55ae, cpActiveRing_hook, 0x23b0, 0x55b1);
  if (!(F & FZ)) {
    CYCT(0x55b1, 0x55b3);
  } else {
    CYC(0x55b1, 0x55b3);
    CYC(0x55b3, 0x55b5); L = 0x28;
    CYC(0x55b5, 0x55b6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x55b6, 0x55b7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(0x55b7, 0x55b9); L = 0x04;
  CYC(0x55b9, 0x55bb); mem_wr(gb, HL, 0xff);
  CYC(0x55bb, 0x55bd); L = 0x06;
  CYC(0x55bd, 0x55bf); mem_wr(gb, HL, 0x08);
  CYC(0x55bf, 0x55c1); L = 0x2f;
  CYC(0x55c1, 0x55c2); A = mem_rd(gb, HL);
  CYC(0x55c2, 0x55c4); alu_or(gb, 0x50);
  CYC(0x55c4, 0x55c5); mem_wr(gb, HL, A);
  CYC(0x55c5, 0x55c7); L = 0x01;
  CYC(0x55c7, 0x55c8); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x55c8, 0x55ca); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x55ca, 0x55cc); alu_cp(gb, 0x03);
  CYC(0x55cc, 0x55ce); A = 0x01;
  if (F & FZ) {
    CYCT(0x55ce, 0x55d0);
  } else {
    CYC(0x55ce, 0x55d0);
    CYC(0x55d0, 0x55d2); A = 0x06;
  }
  CALL_C(0x55d2, itemSetAnimation_hook, 0x49e2, 0x55d5);
  CALL_C(0x55d5, objectSetVisible80_hook, 0x1e57, 0x55d8);
  CYC(0x55d8, 0x55da); A = 0x6f;
  CALL_C(0x55da, playSound_b00_hook, 0x0c98, 0x55dd);
  CYC(0x55dd, 0x55de); alu_or(gb, D);
  CYC(0x55de, 0x55df); ret_effect(gb);
}

void bombInitializeIfNeeded_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55df, 0x55e0); H = D;
  CYC(0x55e0, 0x55e2); L = 0x37;
  CYC(0x55e2, 0x55e4); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x55e4, 0x55e5); ret_effect(gb); return;
  }
  CYC(0x55e4, 0x55e5);
  CYC(0x55e5, 0x55e7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(0x55e7, decNumBombs_hook, 0x17c8, 0x55ea);
  CALL_C(0x55ea, itemLoadAttributesAndGraphics_hook, 0x4993, 0x55ed);
  CALL_C(0x55ed, itemMergeZPositionIfSidescrollingArea_hook, 0x4a4f, 0x55f0);
  bombResetAnimationAndSetVisiblec1_hook(gb);
}

void bombResetAnimationAndSetVisiblec1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55f0, 0x55f1); alu_xor(gb, A);
  CALL_C(0x55f1, itemSetAnimation_hook, 0x49e2, 0x55f4);
  CYC(0x55f4, 0x55f7);
  objectSetVisiblec1_hook(gb);
}

void explosionCheckAndApplyLinkCollision_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55f7, 0x55f8); H = D;
  CYC(0x55f8, 0x55fa); L = 0x37;
  CYC(0x55fa, 0x55fc); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x55fc, 0x55fd); ret_effect(gb); return;
  }
  CYC(0x55fc, 0x55fd);
  CYC(0x55fd, 0x5600); A = mem_rd(gb, w1Companion_id);
  CYC(0x5600, 0x5602); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x5602, 0x5603); ret_effect(gb); return;
  }
  CYC(0x5602, 0x5603);
  CYC(0x5603, 0x5605); A = 0x30;
  CALL_C(0x5605, cpActiveRing_hook, 0x23b0, 0x5608);
  if (F & FZ) {
    CYCT(0x5608, 0x5609); ret_effect(gb); return;
  }
  CYC(0x5608, 0x5609);
  CALL_C(0x5609, checkLinkVulnerable_hook, 0x1d28, 0x560c);
  if (!(F & FC)) {
    CYCT(0x560c, 0x560d); ret_effect(gb); return;
  }
  CYC(0x560c, 0x560d);
  CYC(0x560d, 0x560e); H = D;
  CYC(0x560e, 0x5610); L = 0x26;
  CYC(0x5610, 0x5611); A = mem_rd(gb, HL);
  CYC(0x5611, 0x5612); C = A;
  CYC(0x5612, 0x5613); alu_add(gb, A);
  CYC(0x5613, 0x5614); B = A;
  CYC(0x5614, 0x5616); L = 0x0f;
  CYC(0x5616, 0x5619); A = mem_rd(gb, w1Link_zh);
  CYC(0x5619, 0x561a); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x561a, 0x561b); alu_add(gb, C);
  CYC(0x561b, 0x561c); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(0x561c, 0x561d); ret_effect(gb); return;
  }
  CYC(0x561c, 0x561d);
  CALL_C(0x561d, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x5620);
  if (!(F & FC)) {
    CYCT(0x5620, 0x5621); ret_effect(gb); return;
  }
  CYC(0x5620, 0x5621);
  CALL_C(0x5621, objectGetAngleTowardLink_hook, 0x1e9c, 0x5624);
  CYC(0x5624, 0x5625); H = D;
  CYC(0x5625, 0x5627); L = 0x37;
  CYC(0x5627, 0x5629); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x5629, 0x562b); L = 0x28;
  CYC(0x562b, 0x562c); C = mem_rd(gb, HL);
  CYC(0x562c, 0x562f); SET_HL(w1Link_damageToApply);
  CYC(0x562f, 0x5630); mem_wr(gb, HL, C);
  CYC(0x5630, 0x5632); L = 0x2d;
  CYC(0x5632, 0x5634); mem_wr(gb, HL, 0x0c);
  CYC(0x5634, 0x5635); L = alu_dec8(gb, L);
  CYC(0x5635, 0x5636); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5636, 0x5638); mem_wr(gb, HL, 0x10);
  CYC(0x5638, 0x5639); L = alu_dec8(gb, L);
  CYC(0x5639, 0x563b); mem_wr(gb, HL, 0x01);
  CYC(0x563b, 0x563e);
  linkApplyDamage_b00_hook(gb);
}

void explosionTryToBreakNextTile_hook(GB *gb) {
  CYC(0x563e, 0x563f); A = mem_rd(gb, HL);
  CYC(0x563f, 0x5640); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5640, 0x5641); L = A;
  CYC(0x5641, 0x5642); alu_add(gb, A);
  CYC(0x5642, 0x5643); alu_add(gb, L);
  CYC(0x5643, 0x5646); SET_HL(0x5682);
  CYC(0x5646, 0x5647); bomb_add_a_to_hl_from_rst(gb, 0x5647);
  CYC(0x5647, 0x564a); A = W8(wTilesetFlags);
  CYC(0x564a, 0x564c); alu_and(gb, 0x20);
  CYC(0x564c, 0x564e); E = 0x0f;
  CYC(0x564e, 0x564f); A = mem_rd(gb, DE);
  if (!(F & FZ)) {
    CYCT(0x564f, 0x5651);
    goto calculate_position;
  }
  CYC(0x564f, 0x5651);
  CYC(0x5651, 0x5653); alu_sub(gb, 0x02);
  CYC(0x5653, 0x5654); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x5654, 0x5655); ret_effect(gb); return;
  }
  CYC(0x5654, 0x5655);
  CYC(0x5655, 0x5656); alu_xor(gb, A);

calculate_position:
  CYC(0x5656, 0x5657); C = A;
  CYC(0x5657, 0x5658); SET_HL(HL + 1);
  CYC(0x5658, 0x5659); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5659, 0x565a); alu_add(gb, C);
  CYC(0x565a, 0x565b); B = A;
  CYC(0x565b, 0x565c); A = mem_rd(gb, HL);
  CYC(0x565c, 0x565d); C = A;
  CYC(0x565d, 0x565e); H = D;
  CYC(0x565e, 0x5660); E = 0x00;
  CYC(0x5660, 0x5662); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(0x5662, 0x5664);
  } else {
    CYC(0x5662, 0x5664);
    CYC(0x5664, 0x5665); E = alu_dec8(gb, E);
  }
  CYC(0x5665, 0x5667); L = 0x0b;
  CYC(0x5667, 0x5668); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5668, 0x5669); alu_add(gb, B);
  CYC(0x5669, 0x566a); B = A;
  CYC(0x566a, 0x566c); A = 0x00;
  CYC(0x566c, 0x566d); alu_adc(gb, E);
  if (!(F & FZ)) {
    CYCT(0x566d, 0x566e); ret_effect(gb); return;
  }
  CYC(0x566d, 0x566e);
  CYC(0x566e, 0x566f); L = alu_inc8(gb, L);
  CYC(0x566f, 0x5671); E = 0x00;
  CYC(0x5671, 0x5673); alu_bit(gb, 7, C);
  if (F & FZ) {
    CYCT(0x5673, 0x5675);
  } else {
    CYC(0x5673, 0x5675);
    CYC(0x5675, 0x5676); E = alu_dec8(gb, E);
  }
  CYC(0x5676, 0x5677); A = mem_rd(gb, HL);
  CYC(0x5677, 0x5678); alu_add(gb, C);
  CYC(0x5678, 0x5679); C = A;
  CYC(0x5679, 0x567b); A = 0x00;
  CYC(0x567b, 0x567c); alu_adc(gb, E);
  if (!(F & FZ)) {
    CYCT(0x567c, 0x567d); ret_effect(gb); return;
  }
  CYC(0x567c, 0x567d);
  CYC(0x567d, 0x567f); A = 0x04;
  CYC(0x567f, 0x5682);
  tryToBreakTile_hook(gb);
}

void bombUpdateThrowingLaterally_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x639c, 0x639d); H = D;
  CYC(0x639d, 0x639f); L = 0x3b;
  CYC(0x639f, 0x63a1); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x63a1, 0x63a3);
  } else {
    CYC(0x63a1, 0x63a3);
    CYC(0x63a3, 0x63a5); L = 0x10;
    CYC(0x63a5, 0x63a7); mem_wr(gb, HL, 0x00);
  }
  CYC(0x63a7, 0x63a9); L = 0x37;
  CYC(0x63a9, 0x63ab); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(0x63ab, itemBeginThrow_hook, 0x63b1, 0x63ae);
  else CYC(0x63ab, 0x63ae);
  CYC(0x63ae, 0x63b1);
  itemUpdateThrowingLaterally_hook(gb);
}
