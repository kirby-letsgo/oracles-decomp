#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(itemCode03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x2f;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto delete_bomb;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+12);
    bombResetAnimationAndSetVisiblec1_hook(gb);
    return;
  }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+17);
    bombUpdateExplosion_hook(gb);
    return;
  }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+19); E = 0x04;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  do { uint16_t jt_ = (bomb_jump_table(gb));
    if (jt_ == b_+61) { goto held_state; }
    else if (jt_ == b_+43) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

delete_bomb:
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = 0x04;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+33); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
  } else {
    CYC(b_+33, b_+35);
    CYC(b_+35, b_+37); alu_bit(gb, 1, mem_rd(gb, HL));
    if (F & FZ) CALL_C_CC(b_+37, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+40);
    else CYC(b_+37, b_+40);
  }
  CYC(b_+40, b_+43);
  itemDelete_hook(gb);
  return;

state1:
  CYC(b_+43, b_+45); C = 0x20;
  CALL_C(b_+45, bombUpdateThrowingVerticallyAndCheckDelete_hook, SYM(bombUpdateThrowingVerticallyAndCheckDelete), b_+48);
  if (F & FC) {
    CYCT(b_+48, b_+49); ret_effect(gb); return;
  }
  CYC(b_+48, b_+49);
  CALL_C(b_+49, bombPullTowardPoint_hook, SYM(bombPullTowardPoint), b_+52);
  if (F & FC) {
    CYCT(b_+52, b_+55);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+52, b_+55);
  CALL_C(b_+55, itemUpdateConveyorBelt_hook, SYM(itemUpdateConveyorBelt), b_+58);
  CYC(b_+58, b_+61);
  bombUpdateAnimation_hook(gb);
  return;

held_state:
  CYC(b_+61, b_+63); E = 0x05;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); push_effect(gb, b_+65);
  do { uint16_t jt_ = (bomb_jump_table(gb));
    if (jt_ == b_+73) { goto held_state0; }
    else if (jt_ == b_+87) { goto held_state1; }
    else if (jt_ == b_+102) { goto held_state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

held_state0:
  CALL_C(b_+73, itemIncSubstate_hook, SYM(itemIncSubstate), b_+76);
  CYC(b_+76, b_+78); L = 0x2f;
  CYC(b_+78, b_+80); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+80, b_+82); L = 0x37;
  CYC(b_+82, b_+84); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  CALL_C(b_+84, bombInitializeIfNeeded_hook, SYM(bombInitializeIfNeeded), b_+87);

held_state1:
  CYC(b_+87, b_+89); A = 0x3b;
  CALL_C(b_+89, cpActiveRing_hook, SYM(cpActiveRing), b_+92);
  if (F & FZ) {
    CYCT(b_+92, b_+95);
    bombResetAnimationAndSetVisiblec1_hook(gb);
    return;
  }
  CYC(b_+92, b_+95);
  CALL_C(b_+95, bombUpdateAnimation_hook, SYM(bombUpdateAnimation), b_+98);
  if (F & FZ) {
    CYCT(b_+98, b_+99); ret_effect(gb); return;
  }
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+102);
  dropLinkHeldItem_hook(gb);
  return;

held_state2:
  CYC(b_+102, b_+104); A = 0x03;
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CALL_C(b_+105, bombUpdateThrowingLaterally_hook, SYM(bombUpdateThrowingLaterally), b_+108);
  CYC(b_+108, b_+110); E = 0x39;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); C = A;
  CALL_C(b_+112, bombUpdateThrowingVerticallyAndCheckDelete_hook, SYM(bombUpdateThrowingVerticallyAndCheckDelete), b_+115);
  if (F & FC) {
    CYCT(b_+115, b_+116); ret_effect(gb); return;
  }
  CYC(b_+115, b_+116);
  if (F & FZ) {
    CYCT(b_+116, b_+118);
    goto update_animation;
  }
  CYC(b_+116, b_+118);
  CALL_C(b_+118, itemBounce_hook, SYM(itemBounce), b_+121);
  if (F & FC) {
    CYCT(b_+121, b_+123);
    goto stopped_bouncing;
  }
  CYC(b_+121, b_+123);
  CALL_C(b_+123, bombPullTowardPoint_hook, SYM(bombPullTowardPoint), b_+126);
  if (F & FC) {
    CYCT(b_+126, b_+129);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+126, b_+129);

update_animation:
  CYC(b_+129, b_+132);
  bombUpdateAnimation_hook(gb);
  return;

stopped_bouncing:
  CYC(b_+132, b_+133); H = D;
  CYC(b_+133, b_+135); L = 0x04;
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x01);
  CYC(b_+137, b_+139); L = 0x2f;
  CYC(b_+139, b_+141); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(b_+141, b_+144);
  bombUpdateAnimation_hook(gb);
}

void bombUpdateThrowingVerticallyAndCheckDelete_hook(GB *gb) {
  BASE(bombUpdateThrowingVerticallyAndCheckDelete);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+4); A = W8(wTilesetFlags);
  CYC(b_+4, b_+6); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    goto check_boundary;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = 0x0b;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_sub(gb, 0x08);
  CYC(b_+13, b_+15); alu_cp(gb, 0xf0);
  CYC(b_+15, b_+16); alu_ccf(gb);
  if (F & FC) {
    CYCT(b_+16, b_+18);
    goto restore_bc;
  }
  CYC(b_+16, b_+18);

check_boundary:
  CALL_C(b_+18, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+21);

restore_bc:
  CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+22, b_+24);
    goto delete_bomb;
  }
  CYC(b_+22, b_+24);
  CALL_C(b_+24, itemUpdateThrowingVerticallyAndCheckHazards_hook, SYM(itemUpdateThrowingVerticallyAndCheckHazards), b_+27);
  if (!(F & FC)) {
    CYCT(b_+27, b_+28); ret_effect(gb); return;
  }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+31); SET_BC(GV(0x0050, 0x04ef));
  CYC(b_+31, b_+34); A = W8(wActiveGroup);
  CYC(b_+34, b_+35); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+35, b_+37);
    goto delete_bomb;
  }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+40); A = W8(wActiveRoom);
  CYC(b_+40, b_+41); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+41, b_+43);
    goto delete_bomb;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x01;
  CYC(b_+45, b_+48); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;

delete_bomb:
  CALL_C(b_+48, itemDelete_hook, SYM(itemDelete), b_+51);
  CYC(b_+51, b_+52); alu_scf(gb);
  CYC(b_+52, b_+53); ret_effect(gb);
}

void itemUpdateExplosion_hook(GB *gb) {
  BASE(itemUpdateExplosion);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x21;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+9);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); L = 0x24;
  CYC(b_+11, b_+13); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
  } else {
    CYC(b_+13, b_+15);
    CYC(b_+15, b_+17); mem_wr(gb, HL, 0x00);
  }
  CYC(b_+17, b_+18); C = mem_rd(gb, HL);
  CYC(b_+18, b_+20); L = 0x26;
  CYC(b_+20, b_+22); alu_and(gb, 0x1f);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+26); alu_bit(gb, 7, C);
  if (!(F & FZ)) CALL_C_CC(b_+26, explosionCheckAndApplyLinkCollision_hook, SYM(explosionCheckAndApplyLinkCollision), b_+29);
  else CYC(b_+26, b_+29);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = 0x06;
  CYC(b_+32, b_+34); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(b_+34, explosionTryToBreakNextTile_hook, SYM(explosionTryToBreakNextTile), b_+37);
  else CYC(b_+34, b_+37);
  CYC(b_+37, b_+40);
  itemAnimate_hook(gb);
}

void bombUpdateExplosion_hook(GB *gb) {
  BASE(bombUpdateExplosion);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x04;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+8);
    itemInitializeBombExplosion_hook(gb);
    return;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10);
  itemUpdateExplosion_hook(gb);
}

void bombUpdateAnimation_hook(GB *gb) {
  BASE(bombUpdateAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemAnimate_hook, SYM(itemAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x21;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+7, b_+8); ret_effect(gb); return;
  }
  CYC(b_+7, b_+8);
  itemInitializeBombExplosion_hook(gb);
}

void itemInitializeBombExplosion_hook(GB *gb) {
  BASE(itemInitializeBombExplosion);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x1b;
  CYC(b_+3, b_+5); A = 0x0a;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x0c);
  CYC(b_+9, b_+11); L = 0x24;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+13, b_+15); A = 0x0c;
  CALL_C(b_+15, cpActiveRing_hook, SYM(cpActiveRing), b_+18);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20);
  } else {
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+22); L = 0x28;
    CYC(b_+22, b_+23); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+24, b_+26); L = 0x04;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xff);
  CYC(b_+28, b_+30); L = 0x06;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x08);
  CYC(b_+32, b_+34); L = 0x2f;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); alu_or(gb, 0x50);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  CYC(b_+38, b_+40); L = 0x01;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+43, b_+45); alu_cp(gb, 0x03);
  CYC(b_+45, b_+47); A = 0x01;
  if (F & FZ) {
    CYCT(b_+47, b_+49);
  } else {
    CYC(b_+47, b_+49);
    CYC(b_+49, b_+51); A = 0x06;
  }
  CALL_C(b_+51, itemSetAnimation_hook, SYM(itemSetAnimation), b_+54);
  CALL_C(b_+54, objectSetVisible80_hook, SYM(objectSetVisible80), b_+57);
  CYC(b_+57, b_+59); A = 0x6f;
  CALL_C(b_+59, playSound_b00_hook, SYM(playSound_b00), b_+62);
  CYC(b_+62, b_+63); alu_or(gb, D);
  CYC(b_+63, b_+64); ret_effect(gb);
}

void bombInitializeIfNeeded_hook(GB *gb) {
  BASE(bombInitializeIfNeeded);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x37;
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(b_+8, decNumBombs_hook, SYM(decNumBombs), b_+11);
  CALL_C(b_+11, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+14);
  CALL_C(b_+14, itemMergeZPositionIfSidescrollingArea_hook, SYM(itemMergeZPositionIfSidescrollingArea), SYM(bombResetAnimationAndSetVisiblec1));
  bombResetAnimationAndSetVisiblec1_hook(gb);
}

void bombResetAnimationAndSetVisiblec1_hook(GB *gb) {
  BASE(bombResetAnimationAndSetVisiblec1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, itemSetAnimation_hook, SYM(itemSetAnimation), b_+4);
  CYC(b_+4, b_+7);
  objectSetVisiblec1_hook(gb);
}

void explosionCheckAndApplyLinkCollision_hook(GB *gb) {
  BASE(explosionCheckAndApplyLinkCollision);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x37;
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, w1Companion_id);
  CYC(b_+9, b_+11); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x30;
  CALL_C(b_+14, cpActiveRing_hook, SYM(cpActiveRing), b_+17);
  if (F & FZ) {
    CYCT(b_+17, b_+18); ret_effect(gb); return;
  }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+21);
  if (!(F & FC)) {
    CYCT(b_+21, b_+22); ret_effect(gb); return;
  }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0x26;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); C = A;
  CYC(b_+27, b_+28); alu_add(gb, A);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+31); L = 0x0f;
  CYC(b_+31, b_+34); A = mem_rd(gb, w1Link_zh);
  CYC(b_+34, b_+35); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); alu_add(gb, C);
  CYC(b_+36, b_+37); alu_cp(gb, B);
  if (!(F & FC)) {
    CYCT(b_+37, b_+38); ret_effect(gb); return;
  }
  CYC(b_+37, b_+38);
  CALL_C(b_+38, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+41);
  if (!(F & FC)) {
    CYCT(b_+41, b_+42); ret_effect(gb); return;
  }
  CYC(b_+41, b_+42);
  CALL_C(b_+42, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+45);
  CYC(b_+45, b_+46); H = D;
  CYC(b_+46, b_+48); L = 0x37;
  CYC(b_+48, b_+50); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+50, b_+52); L = 0x28;
  CYC(b_+52, b_+53); C = mem_rd(gb, HL);
  CYC(b_+53, b_+56); SET_HL(w1Link_damageToApply);
  CYC(b_+56, b_+57); mem_wr(gb, HL, C);
  CYC(b_+57, b_+59); L = 0x2d;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x0c);
  CYC(b_+61, b_+62); L = alu_dec8(gb, L);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x10);
  CYC(b_+65, b_+66); L = alu_dec8(gb, L);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x01);
  CYC(b_+68, b_+71);
  linkApplyDamage_b00_hook(gb);
}

void explosionTryToBreakNextTile_hook(GB *gb) {
  BASE(explosionTryToBreakNextTile);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+2, b_+3); L = A;
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); alu_add(gb, L);
  CYC(b_+5, b_+8); SET_HL(b_+68);
  CYC(b_+8, b_+9); bomb_add_a_to_hl_from_rst(gb, b_+9);
  CYC(b_+9, b_+12); A = W8(wTilesetFlags);
  CYC(b_+12, b_+14); alu_and(gb, 0x20);
  CYC(b_+14, b_+16); E = 0x0f;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19);
    goto calculate_position;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); alu_sub(gb, 0x02);
  CYC(b_+21, b_+22); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+22, b_+23); ret_effect(gb); return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);

calculate_position:
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+26); SET_HL(HL + 1);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); alu_add(gb, C);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+34); E = 0x00;
  CYC(b_+34, b_+36); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(b_+36, b_+38);
  } else {
    CYC(b_+36, b_+38);
    CYC(b_+38, b_+39); E = alu_dec8(gb, E);
  }
  CYC(b_+39, b_+41); L = 0x0b;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); alu_add(gb, B);
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+46); A = 0x00;
  CYC(b_+46, b_+47); alu_adc(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+48); ret_effect(gb); return;
  }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+49); L = alu_inc8(gb, L);
  CYC(b_+49, b_+51); E = 0x00;
  CYC(b_+51, b_+53); alu_bit(gb, 7, C);
  if (F & FZ) {
    CYCT(b_+53, b_+55);
  } else {
    CYC(b_+53, b_+55);
    CYC(b_+55, b_+56); E = alu_dec8(gb, E);
  }
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+58); alu_add(gb, C);
  CYC(b_+58, b_+59); C = A;
  CYC(b_+59, b_+61); A = 0x00;
  CYC(b_+61, b_+62); alu_adc(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+62, b_+63); ret_effect(gb); return;
  }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+65); A = 0x04;
  CYC(b_+65, b_+68);
  tryToBreakTile_hook(gb);
}

void bombUpdateThrowingLaterally_hook(GB *gb) {
  BASE(bombUpdateThrowingLaterally);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x3b;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+9); L = 0x10;
    CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  }
  CYC(b_+11, b_+13); L = 0x37;
  CYC(b_+13, b_+15); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(b_+15, itemBeginThrow_hook, SYM(itemBeginThrow), b_+18);
  else CYC(b_+15, b_+18);
  CYC(b_+18, b_+21);
  itemUpdateThrowingLaterally_hook(gb);
}
