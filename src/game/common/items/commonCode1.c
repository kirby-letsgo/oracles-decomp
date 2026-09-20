#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemNextAnimationFrame_hook(GB *gb);
void itemLoadAttributesAndGraphics_hook(GB *gb);
void itemMergeZPositionIfSidescrollingArea_hook(GB *gb);
void itemUpdateThrowingVertically_hook(GB *gb);
void checkTileIsPassableFromDirection_hook(GB *gb);

static void item_update_throwing_check_hole_or_water(GB *gb, uint16_t sp0_);
static void item_check_can_pass_solid_tile_body(GB *gb, uint16_t sp0_);

static void item_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static void item_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void loadAttributesAndGraphicsAndIncState_hook(GB *gb) {
  BASE(loadAttributesAndGraphicsAndIncState);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIncState_hook, SYM(itemIncState), b_+3);
  CYC(b_+3, b_+5); L = 0x00;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x03);
  itemLoadAttributesAndGraphics_hook(gb);
}

void itemLoadAttributesAndGraphics_hook(GB *gb) {
  BASE(itemLoadAttributesAndGraphics);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+7); SET_HL(SYM(itemAttributes));
  CYC(b_+7, b_+8); item_add_double_index_from_rst(gb, b_+8);
  CYC(b_+8, b_+10); E = 0x24;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = 0x26;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); E = alu_inc8(gb, E);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+24); alu_and(gb, 0x0f);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); C = A;
  CYC(b_+29, b_+30); E = alu_inc8(gb, E);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); E = 0x3a;
  CYC(b_+34, b_+35); A = C;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CALL_C(b_+36, itemSetVar3cToFF_hook, SYM(itemSetVar3cToFF), b_+39);
  CYC(b_+39, b_+42); SET_HL((SYM(collisionEffect3c__ringProtections) + 5));
  CYC(b_+42, b_+44); E = 0x3f;
  CYC(b_+44, b_+47); interBankCall_hook(gb);
}

void itemSetVar3cToFF_hook(GB *gb) {
  BASE(itemSetVar3cToFF);
  CYC(b_+0, b_+2); E = 0x3c;
  CYC(b_+2, b_+4); A = 0xff;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void itemUpdateDamageToApply_hook(GB *gb) {
  BASE(itemUpdateDamageToApply);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x25;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x00);
  CYC(b_+6, b_+8); L = 0x29;
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+11); alu_rlca(gb);
  CYC(b_+11, b_+13); L = 0x2a;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void itemAnimate_hook(GB *gb) {
  BASE(itemAnimate);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x20;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); L = 0x22;
  CYC(b_+7, b_+9); itemNextAnimationFrame_hook(gb);
}

void itemSetAnimation_hook(GB *gb) {
  BASE(itemSetAnimation);
  CYC(b_+0, b_+1); alu_add(gb, A);
  CYC(b_+1, b_+2); C = A;
  CYC(b_+2, b_+4); B = 0x00;
  CYC(b_+4, b_+6); E = 0x01;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+10); SET_HL(SYM(itemAnimationTable));
  CYC(b_+10, b_+11); item_add_double_index_from_rst(gb, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); H = mem_rd(gb, HL);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+15); alu_add_hl(gb, BC);
  itemNextAnimationFrame_hook(gb);
}

void itemNextAnimationFrame_hook(GB *gb) {
  BASE(itemNextAnimationFrame);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); H = mem_rd(gb, HL);
  CYC(b_+2, b_+3); L = A;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); B = A;
    CYC(b_+9, b_+10); C = mem_rd(gb, HL);
    CYC(b_+10, b_+11); alu_add_hl(gb, BC);
    CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  }
  CYC(b_+12, b_+14); E = 0x20;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+19); B = 0x00;
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); A = L;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); A = H;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = 0x01;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+34); SET_HL(SYM(itemOamDataTable));
  CYC(b_+34, b_+35); item_add_double_index_from_rst(gb, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); H = mem_rd(gb, HL);
  CYC(b_+37, b_+38); L = A;
  CYC(b_+38, b_+39); alu_add_hl(gb, BC);
  CYC(b_+39, b_+41); E = 0x1e;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+44); E = alu_inc8(gb, E);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+47); alu_and(gb, 0x3f);
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); ret_effect(gb);
}

void itemTransferKnockbackToLink_hook(GB *gb) {
  BASE(itemTransferKnockbackToLink);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x2d;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+9); L = alu_dec8(gb, L);
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  CYC(b_+10, b_+13); SET_HL(w1Link_knockbackCounter);
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+14, b_+16);
  } else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  }
  CYC(b_+17, b_+18); L = alu_dec8(gb, L);
  CYC(b_+18, b_+19); mem_wr(gb, HL, B);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void applyOffsetTableHL_hook(GB *gb) {
  BASE(applyOffsetTableHL);
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); alu_add(gb, E);
  CYC(b_+6, b_+7); item_add_a_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); E = 0x0b;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+15); E = 0x0d;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+21); E = 0x0f;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); ret_effect(gb);
}

void itemMergeZPositionIfSidescrollingArea_hook(GB *gb) {
  BASE(itemMergeZPositionIfSidescrollingArea);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+4); A = W8(wTilesetFlags);
  CYC(b_+4, b_+6); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); E = 0x0b;
  CYC(b_+9, b_+11); L = 0x0f;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+18); alu_or(gb, D);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void itemUpdateSpeedZAndCheckHazards_hook(GB *gb) {
  BASE(itemUpdateSpeedZAndCheckHazards);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+2); E = 0x0f;
  CYC(b_+2, b_+3); A = E;
  CYC(b_+3, b_+5); H8(hFF8B) = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+7, b_+9);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+10); alu_rrca(gb);
    CYC(b_+10, b_+12); H8(hFF8B) = A;
    CALL_C(b_+12, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+15);
    if (!(F & FZ)) {
      CYCT(b_+15, b_+17);
      goto above_ground;
    }
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+19); H8(hFF8B) = A;
  }
  CALL_C(b_+19, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+22);
  if (!(F & FC)) {
    CYCT(b_+22, b_+24);
    goto above_ground;
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+27); A = 0xff;
  CYC(b_+27, b_+28); ret_effect(gb);
  return;

above_ground:
  CYC(b_+28, b_+30); A = H8(hFF8B);
  CYC(b_+30, b_+31); alu_rlca(gb);
  CYC(b_+31, b_+32); alu_or(gb, A);
  CYC(b_+32, b_+33); ret_effect(gb);
}

void bombPullTowardPoint_hook(GB *gb) {
  BASE(bombPullTowardPoint);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0f;
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto end;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = 0x31;
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); C = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); alu_or(gb, B);
  if (F & FZ) {
    CYCT(b_+14, b_+15); ret_effect(gb); return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); push_effect(gb, BC);
  CALL_C(b_+16, objectCheckContainsPoint_hook, SYM(objectCheckContainsPoint), b_+19);
  CYC(b_+19, b_+20); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+20, b_+21); ret_effect(gb); return;
  }
  CYC(b_+20, b_+21);
  CALL_C(b_+21, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+24);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+27); B = 0x0a;
  CYC(b_+27, b_+29); E = 0x09;
  CALL_C(b_+29, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+32);

end:
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+34); ret_effect(gb);
}

void itemUpdateThrowingVertically_hook(GB *gb) {
  BASE(itemUpdateThrowingVertically);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, itemMergeZPositionIfSidescrollingArea_hook, SYM(itemMergeZPositionIfSidescrollingArea), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    goto sidescrolling;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto unset_collision;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); push_effect(gb, b_+13);
  item_update_throwing_check_hole_or_water(gb, gb->sp);
  CYC(b_+13, b_+15); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+15, b_+17); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+17, b_+18); alu_scf(gb);
  CYC(b_+18, b_+19); ret_effect(gb);
  return;

unset_collision:
  CYC(b_+19, b_+21); L = 0x3b;
  CYC(b_+21, b_+23); mem_wr(gb, HL, mem_rd(gb, HL) & 0xef);
  CYC(b_+23, b_+24); alu_or(gb, D);
  CYC(b_+24, b_+25); ret_effect(gb);
  return;

set_collision:
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = 0x3b;
  CYC(b_+28, b_+30); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+32, b_+33); alu_scf(gb);
  CYC(b_+33, b_+34); ret_effect(gb);
  return;

sidescrolling:
  CYC(b_+34, b_+35); push_effect(gb, BC);
  CYC(b_+35, b_+38); push_effect(gb, b_+38);
  item_update_throwing_check_hole_or_water(gb, gb->sp);
  CYC(b_+38, b_+40); L = 0x15;
  CYC(b_+40, b_+42); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    goto not_moving_up;
  }
  CYC(b_+42, b_+44);
  CALL_C(b_+44, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+47);
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+49); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+49, b_+51);
    goto no_ceiling_collision;
  }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); B = 0x03;
  CYC(b_+53, b_+55);
  goto update_gravity;

not_moving_up:
  CYC(b_+55, b_+57); L = 0x0b;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+58, b_+60); alu_add(gb, 0x05);
  CYC(b_+60, b_+61); B = A;
  CYC(b_+61, b_+62); L = alu_inc8(gb, L);
  CYC(b_+62, b_+63); C = mem_rd(gb, HL);
  CALL_C(b_+63, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+66);
  CYC(b_+66, b_+67); H = D;
  CYC(b_+67, b_+68); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+68, b_+70);
    goto set_collision;
  }
  CYC(b_+68, b_+70);

no_ceiling_collision:
  CYC(b_+70, b_+72); L = 0x3b;
  CYC(b_+72, b_+74); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+74, b_+76); B = 0x03;
  if (F & FZ) {
    CYCT(b_+76, b_+78);
  } else {
    CYC(b_+76, b_+78);
    CYC(b_+78, b_+80); B = 0x01;
    CYC(b_+80, b_+82); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(b_+82, b_+84);
      goto unset_collision;
    }
    CYC(b_+82, b_+84);
  }
  CYC(b_+84, b_+86); E = 0x14;
  CYC(b_+86, b_+88); L = 0x0a;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+90, b_+91); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+91, b_+92); E = alu_inc8(gb, E);
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+94); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+94, b_+95); mem_wr(gb, HL, A); SET_HL(HL + 1);

update_gravity:
  CYC(b_+95, b_+97); L = 0x14;
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+99); alu_add(gb, C);
  CYC(b_+99, b_+100); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+103); alu_adc(gb, 0x00);
  CYC(b_+103, b_+104); mem_wr(gb, HL, A);
  CYC(b_+104, b_+106); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+108);
    goto unset_collision;
  }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+109); alu_cp(gb, B);
  if (F & FC) {
    CYCT(b_+109, b_+111);
    goto unset_collision;
  }
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+112); mem_wr(gb, HL, B);
  CYC(b_+112, b_+113); L = alu_dec8(gb, L);
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x00);
  CYC(b_+115, b_+117);
  goto unset_collision;
}

static void item_update_throwing_check_hole_or_water(GB *gb, uint16_t sp0_) {
  BASE(itemUpdateThrowingVertically);
  CALL_C(b_+117, itemMergeZPositionIfSidescrollingArea_hook, SYM(itemMergeZPositionIfSidescrollingArea), b_+120);
  if (!(F & FZ)) {
    CYCT(b_+120, b_+122);
  } else {
    CYC(b_+120, b_+122);
    CYC(b_+122, b_+124); L = 0x0f;
    CYC(b_+124, b_+126); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(b_+126, b_+128);
      goto update_collision;
    }
    CYC(b_+126, b_+128);
  }
  CALL_C(b_+128, objectCheckIsOverHazard_hook, SYM(objectCheckIsOverHazard), b_+131);
  CYC(b_+131, b_+132); H = D;

update_collision:
  CYC(b_+132, b_+133); B = A;
  CYC(b_+133, b_+135); L = 0x3b;
  CYC(b_+135, b_+136); A = mem_rd(gb, HL);
  CYC(b_+136, b_+137); C = A;
  CYC(b_+137, b_+139); alu_and(gb, 0xb8);
  CYC(b_+139, b_+141); alu_xor(gb, 0x80);
  CYC(b_+141, b_+142); alu_or(gb, B);
  CYC(b_+142, b_+143); mem_wr(gb, HL, A);
  CYC(b_+143, b_+144); A = B;
  CYC(b_+144, b_+145); alu_xor(gb, C);
  CYC(b_+145, b_+146); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+146, b_+148);
  } else {
    CYC(b_+146, b_+148);
    CYC(b_+148, b_+150); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  }
  CYC(b_+150, b_+151); ret_effect(gb);
}

static void item_update_throwing_create_splash(GB *gb, uint16_t sp0_) {
  BASE(itemUpdateThrowingVerticallyAndCheckHazards);
  CALL_C(b_+59, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+62);
  CYC(b_+62, b_+63); alu_scf(gb);
  CYC(b_+63, b_+64); ret_effect(gb);
}

void itemUpdateThrowingVerticallyAndCheckHazards_hook(GB *gb) {
  BASE(itemUpdateThrowingVerticallyAndCheckHazards);
  uint16_t sp0_ = gb->sp;

  CALL_C(b_+0, itemUpdateThrowingVertically_hook, SYM(itemUpdateThrowingVertically), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+5);
    goto landed;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = W8(wTilesetFlags);
  CYC(b_+8, b_+10); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto no_hazard;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); B = 0x04;
  CYC(b_+14, b_+16); alu_bit(gb, 2, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    item_update_throwing_create_splash(gb, sp0_);
    return;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); B = 0x03;
  CYC(b_+20, b_+22); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+22, b_+25); push_effect(gb, b_+25);
    item_update_throwing_create_splash(gb, gb->sp);
  } else {
    CYC(b_+22, b_+25);
  }

no_hazard:
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); ret_effect(gb);
  return;

landed:
  CYC(b_+27, b_+30); A = W8(wTilesetFlags);
  CYC(b_+30, b_+32); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+34);
    goto no_collisions;
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = 0x3b;
  CYC(b_+37, b_+39); B = 0x03;
  CYC(b_+39, b_+41); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+41, b_+43);
    item_update_throwing_create_splash(gb, sp0_);
    return;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); B = 0x0f;
  CYC(b_+45, b_+47); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
    goto create_hole_animation;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); B = 0x04;
  CYC(b_+51, b_+53); alu_bit(gb, 2, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+53, b_+55);
    item_update_throwing_create_splash(gb, sp0_);
    return;
  }
  CYC(b_+53, b_+55);

no_collisions:
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, b_+58); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+58, b_+59); ret_effect(gb);
  return;

create_hole_animation:
  CALL_C(b_+64, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+67);
  CYC(b_+67, b_+68); alu_scf(gb);
  CYC(b_+68, b_+69); ret_effect(gb);
}

void objectCreateClinkInteraction_hook(GB *gb) {
  BASE(objectCreateClinkInteraction);
  CYC(b_+0, b_+2); B = 0x07;
  CYC(b_+2, b_+5); objectCreateInteractionWithSubid00_hook(gb);
}

void cpRelatedObject1ID_hook(GB *gb) {
  BASE(cpRelatedObject1ID);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); E = 0x01;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+10); ret_effect(gb);
}

void itemCheckCanPassSolidTileAt_hook(GB *gb) {
  BASE(itemCheckCanPassSolidTileAt);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getTileAtPosition_hook, SYM(getTileAtPosition), b_+3);
  CYC(b_+3, b_+5);
  item_check_can_pass_solid_tile_body(gb, sp0_);
}

void itemCheckCanPassSolidTile_hook(GB *gb) {
  BASE(itemCheckCanPassSolidTile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  item_check_can_pass_solid_tile_body(gb, sp0_);
}

static void item_check_can_pass_solid_tile_body(GB *gb, uint16_t sp0_) {
  BASE(itemCheckCanPassSolidTile);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+5); A = L;
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0x3c;
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
  } else {
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+13); A = E;
    CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+14, b_+15); ret_effect(gb); return;
    }
    CYC(b_+14, b_+15);
  }
  CYC(b_+15, b_+16); mem_wr(gb, HL, E);
  CYC(b_+16, b_+18); L = 0x09;
  CYC(b_+18, b_+19); B = mem_rd(gb, HL);
  CALL_C(b_+19, checkTileIsPassableFromDirection_hook, SYM(checkTileIsPassableFromDirection), b_+22);
  if (!(F & FC)) {
    CYCT(b_+22, b_+24);
    goto collision;
  }
  CYC(b_+22, b_+24);
  if (F & FZ) {
    CYCT(b_+24, b_+25); ret_effect(gb); return;
  }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = 0x3e;
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+32); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+32, b_+33); ret_effect(gb); return;
  }
  CYC(b_+32, b_+33);

collision:
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = 0x3c;
  CYC(b_+36, b_+38); A = 0xff;
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+41); alu_or(gb, D);
  CYC(b_+41, b_+42); ret_effect(gb);
}

void checkTileIsPassableFromDirection_hook(GB *gb) {
  BASE(checkTileIsPassableFromDirection);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(SYM(itemPassableTilesTable));
  CALL_C(b_+3, findByteInCollisionTable_paramE_hook, SYM(findByteInCollisionTable_paramE), b_+6);
  if (F & FC) {
    CYCT(b_+6, b_+8);
    goto can_pass_without_elevation_change;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+12); SET_HL(SYM(angleTable));
  CYC(b_+12, b_+13); item_add_a_to_hl_from_rst(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); push_effect(gb, AF);
  CYC(b_+15, b_+18); A = W8(wActiveCollisions);
  CYC(b_+18, b_+21); SET_HL(SYM(itemPassableCliffTilesTable));
  CYC(b_+21, b_+22); item_add_double_index_from_rst(gb, b_+22);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+24); H = mem_rd(gb, HL);
  CYC(b_+24, b_+25); L = A;
  CYC(b_+25, b_+26); SET_AF(pop_effect(gb));
  CYC(b_+26, b_+28); A = alu_srl(gb, A);
  if (!(F & FC)) {
    CYCT(b_+28, b_+30);
    goto check_one_direction_only;
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); item_add_a_to_hl_from_rst(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+33); push_effect(gb, HL);
  CYC(b_+33, b_+34); item_add_a_to_hl_from_rst(gb, b_+34);
  CALL_C(b_+34, lookupKey_hook, SYM(lookupKey), b_+37);
  CYC(b_+37, b_+38); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+38, b_+40);
    goto can_pass_with_elevation_change;
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); SET_HL(HL + 1);
  CYC(b_+41, b_+43);
  goto lookup_direction;

check_one_direction_only:
  CYC(b_+43, b_+44); item_add_a_to_hl_from_rst(gb, b_+44);

lookup_direction:
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); item_add_a_to_hl_from_rst(gb, b_+46);
  CALL_C(b_+46, lookupKey_hook, SYM(lookupKey), b_+49);
  if (!(F & FC)) {
    CYCT(b_+49, b_+50); ret_effect(gb); return;
  }
  CYC(b_+49, b_+50);

can_pass_with_elevation_change:
  CYC(b_+50, b_+51); alu_or(gb, A);
  CYC(b_+51, b_+52); alu_scf(gb);
  CYC(b_+52, b_+53); ret_effect(gb);
  return;

can_pass_without_elevation_change:
  CYC(b_+53, b_+54); alu_xor(gb, A);
  CYC(b_+54, b_+55); alu_scf(gb);
  CYC(b_+55, b_+56); ret_effect(gb);
}

void itemUpdateConveyorBelt_hook(GB *gb) {
  BASE(itemUpdateConveyorBelt);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x0f;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0500);
  CALL_C(b_+8, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+11);
  CYC(b_+11, b_+14); SET_HL(SYM(itemConveyorTilesTable));
  CALL_C(b_+14, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+17);
  if (!(F & FC)) {
    CYCT(b_+17, b_+18); ret_effect(gb); return;
  }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); push_effect(gb, AF);
  CYC(b_+19, b_+20); alu_rrca(gb);
  CYC(b_+20, b_+21); alu_rrca(gb);
  CYC(b_+21, b_+24); SET_HL(SYM(bombEdgeOffsets));
  CYC(b_+24, b_+25); item_add_a_to_hl_from_rst(gb, b_+25);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); C = mem_rd(gb, HL);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = 0x0b;
  CYC(b_+30, b_+31); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); B = A;
  CYC(b_+32, b_+34); L = 0x0d;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_add(gb, C);
  CYC(b_+36, b_+37); C = A;
  CALL_C(b_+37, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+40);
  CYC(b_+40, b_+42); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    goto done;
  }
  CYC(b_+42, b_+44);
  CALL_C(b_+44, checkGivenCollision_allowHoles_hook, SYM(checkGivenCollision_allowHoles), b_+47);
  if (F & FC) {
    CYCT(b_+47, b_+49);
    goto done;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); SET_AF(pop_effect(gb));
  CYC(b_+50, b_+51); C = A;
  CYC(b_+51, b_+53); B = 0x14;
  CYC(b_+53, b_+55); E = 0x09;
  CYC(b_+55, b_+58); objectApplyGivenSpeed_hook(gb);
  return;

done:
  CYC(b_+58, b_+59); SET_AF(pop_effect(gb));
  CYC(b_+59, b_+60); ret_effect(gb);
}
