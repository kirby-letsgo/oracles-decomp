#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t somaria_jump_table(GB *gb) {
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

static void somaria_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void itemCode04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c49, itemTransferKnockbackToLink_hook, 0x4a22, 0x5c4c);
  CYC(0x5c4c, 0x5c4e); E = 0x04;
  CYC(0x5c4e, 0x5c4f); A = mem_rd(gb, DE);
  CYC(0x5c4f, 0x5c50); push_effect(gb, 0x5c50);
  switch (somaria_jump_table(gb)) {
    case 0x5c56: break;
    case 0x5c6a: goto state1;
    case 0x5ca3: CYC(0x5ca3, 0x5ca4); ret_effect(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CYC(0x5c56, 0x5c58); A = 0x1c;
  CALL_C(0x5c58, loadWeaponGfx_b00_hook, 0x166d, 0x5c5b);
  CALL_C(0x5c5b, loadAttributesAndGraphicsAndIncState_hook, 0x498c, 0x5c5e);
  CYC(0x5c5e, 0x5c60); A = 0x74;
  CALL_C(0x5c60, playSound_b00_hook, 0x0c98, 0x5c63);
  CYC(0x5c63, 0x5c64); alu_xor(gb, A);
  CALL_C(0x5c64, itemSetAnimation_hook, 0x49e2, 0x5c67);
  CYC(0x5c67, 0x5c6a); objectSetVisible82_hook(gb);
  return;

state1:
  CYC(0x5c6a, 0x5c6d); A = W8(w1ParentItem2_animParameter);
  CYC(0x5c6d, 0x5c6f); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x5c6f, 0x5c70); ret_effect(gb); return;
  }
  CYC(0x5c6f, 0x5c70);
  CALL_C(0x5c70, itemIncState_hook, 0x23ea, 0x5c73);
  CYC(0x5c73, 0x5c75); C = 0x18;
  CALL_C(0x5c75, findItemWithID_hook, 0x22b9, 0x5c78);
  if (!(F & FZ)) {
    CYCT(0x5c78, 0x5c7a);
  } else {
    CYC(0x5c78, 0x5c7a);
    CYC(0x5c7a, 0x5c7c); L = 0x2f;
    CYC(0x5c7c, 0x5c7e); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  }
  CYC(0x5c7e, 0x5c81); A = W8(w1Link_direction);
  CYC(0x5c81, 0x5c84); SET_HL(0x5ca4);
  CYC(0x5c84, 0x5c85); somaria_add_double_index(gb, 0x5c85);
  CYC(0x5c85, 0x5c88); A = W8(w1Link_yh);
  CYC(0x5c88, 0x5c89); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5c89, 0x5c8a); B = A;
  CYC(0x5c8a, 0x5c8b); SET_HL(HL + 1);
  CYC(0x5c8b, 0x5c8e); A = W8(w1Link_xh);
  CYC(0x5c8e, 0x5c8f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5c8f, 0x5c90); C = A;
  CALL_C(0x5c90, getFreeItemSlot_hook, 0x2cf9, 0x5c93);
  if (!(F & FZ)) {
    CYCT(0x5c93, 0x5c94); ret_effect(gb); return;
  }
  CYC(0x5c93, 0x5c94);
  CYC(0x5c94, 0x5c95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5c95, 0x5c96); L = alu_inc8(gb, L);
  CYC(0x5c96, 0x5c98); mem_wr(gb, HL, 0x18);
  CYC(0x5c98, 0x5c9a); L = 0x0b;
  CYC(0x5c9a, 0x5c9b); mem_wr(gb, HL, B);
  CYC(0x5c9b, 0x5c9e); A = W8(w1Link_zh);
  CYC(0x5c9e, 0x5ca0); L = 0x0f;
  CYC(0x5ca0, 0x5ca1); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5ca1, 0x5ca2); L = alu_dec8(gb, L);
  CYC(0x5ca2, 0x5ca3); mem_wr(gb, HL, C);
  CYC(0x5ca3, 0x5ca4); ret_effect(gb);
}

static void somaria_check_deletion_trigger(GB *gb) {
  CYC(0x5dd4, 0x5dd5); H = D;
  CYC(0x5dd5, 0x5dd7); L = 0x2f;
  CYC(0x5dd7, 0x5dd9); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(0x5dd9, 0x5dda); ret_effect(gb);
}

static void somaria_push_link_away(GB *gb) {
  CYC(0x5dda, 0x5ddc); E = 0x26;
  CYC(0x5ddc, 0x5dde); A = 0x07;
  CYC(0x5dde, 0x5ddf); mem_wr(gb, DE, A);
  CYC(0x5ddf, 0x5de2); SET_HL(w1Link);
  CYC(0x5de2, 0x5de5); preventObjectHFromPassingObjectD_hook(gb);
}

static void somaria_check_block_in_place(GB *gb) {
  CYC(0x5de5, 0x5de7); E = 0x32;
  CYC(0x5de7, 0x5de8); A = mem_rd(gb, DE);
  CYC(0x5de8, 0x5de9); L = A;
  CYC(0x5de9, 0x5deb); H = (uint8_t)(wRoomLayout >> 8);
  CYC(0x5deb, 0x5dec); A = mem_rd(gb, HL);
  CYC(0x5dec, 0x5dee); alu_cp(gb, 0xda);
  if (!(F & FZ)) {
    CYCT(0x5dee, 0x5def); ret_effect(gb); return;
  }
  CYC(0x5dee, 0x5def);
  CYC(0x5def, 0x5df1); H = (uint8_t)(wRoomCollisions >> 8);
  CYC(0x5df1, 0x5df2); A = mem_rd(gb, HL);
  CYC(0x5df2, 0x5df4); alu_cp(gb, 0x0f);
  CYC(0x5df4, 0x5df5); ret_effect(gb);
}

static void somaria_remove_block(GB *gb, uint16_t sp0_) {
  CYC(0x5df5, 0x5df8); push_effect(gb, 0x5df8); somaria_check_block_in_place(gb);
  if (!(F & FZ)) {
    CYCT(0x5df8, 0x5df9); ret_effect(gb); return;
  }
  CYC(0x5df8, 0x5df9);
  CYC(0x5df9, 0x5dfb); E = 0x32;
  CYC(0x5dfb, 0x5dfc); A = mem_rd(gb, DE);
  CALL_C(0x5dfc, getTileIndexFromRoomLayoutBuffer_hook, 0x15d7, 0x5dff);
  CYC(0x5dff, 0x5e02); setTile_hook(gb);
}

static void somaria_check_block_can_appear(GB *gb, uint16_t sp0_) {
  CYC(0x5e02, 0x5e05); A = W8(wActiveGroup);
  CYC(0x5e05, 0x5e07); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(0x5e07, 0x5e09);
  } else {
    CYC(0x5e07, 0x5e09);
    CYC(0x5e09, 0x5e0c); A = W8(wActiveRoom);
    CYC(0x5e0c, 0x5e0e); alu_cp(gb, 0xe8);
    if (F & FZ) {
      CYCT(0x5e0e, 0x5e10);
      goto disallow;
    }
    CYC(0x5e0e, 0x5e10);
  }
  CYC(0x5e10, 0x5e12); E = 0x0f;
  CYC(0x5e12, 0x5e13); A = mem_rd(gb, DE);
  CYC(0x5e13, 0x5e14); A = alu_dec8(gb, A);
  CYC(0x5e14, 0x5e16); alu_cp(gb, 0xfc);
  if (F & FC) {
    CYCT(0x5e16, 0x5e18);
    goto disallow;
  }
  CYC(0x5e16, 0x5e18);
  CALL_C(0x5e18, objectGetTileCollisions_hook, 0x14ad, 0x5e1b);
  if (!(F & FZ)) {
    CYCT(0x5e1b, 0x5e1c); ret_effect(gb); return;
  }
  CYC(0x5e1b, 0x5e1c);
  CYC(0x5e1c, 0x5e1f); A = W8(wTilesetFlags);
  CYC(0x5e1f, 0x5e21); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x5e21, 0x5e22); ret_effect(gb); return;
  }
  CYC(0x5e21, 0x5e22);
  CYC(0x5e22, 0x5e24); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x5e24, 0x5e25); ret_effect(gb); return;
  }
  CYC(0x5e24, 0x5e25);
  CYC(0x5e25, 0x5e26); A = L;
  CYC(0x5e26, 0x5e28); alu_add(gb, 0x10);
  CYC(0x5e28, 0x5e29); L = A;
  CYC(0x5e29, 0x5e2a); A = mem_rd(gb, HL);
  CYC(0x5e2a, 0x5e2c); alu_cp(gb, 0x0f);
  CYC(0x5e2c, 0x5e2d); ret_effect(gb);
  return;

disallow:
  CYC(0x5e2d, 0x5e2e); alu_or(gb, D);
  CYC(0x5e2e, 0x5e2f); ret_effect(gb);
}

static void somaria_align_on_tile(GB *gb, uint16_t sp0_) {
  CALL_C(0x5e52, objectCenterOnTile_hook, 0x20db, 0x5e55);
  CYC(0x5e55, 0x5e57); L = 0x0b;
  CYC(0x5e57, 0x5e58); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e58, 0x5e59); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e59, 0x5e5a); ret_effect(gb);
}

static void somaria_create_block_if_not_on_hazard(GB *gb, uint16_t sp0_) {
  CYC(0x5e2f, 0x5e32); push_effect(gb, 0x5e32); somaria_align_on_tile(gb, sp0_);
  CALL_C(0x5e32, objectGetTileAtPosition_hook, 0x1444, 0x5e35);
  CYC(0x5e35, 0x5e36); push_effect(gb, HL);
  CYC(0x5e36, 0x5e39); SET_HL(hazardCollisionTable);
  CALL_C(0x5e39, lookupCollisionTable_hook, 0x1e1f, 0x5e3c);
  CYC(0x5e3c, 0x5e3d); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(0x5e3d, 0x5e3f);
    CYC(0x5e50, 0x5e51); alu_or(gb, D);
    CYC(0x5e51, 0x5e52); ret_effect(gb);
    return;
  }
  CYC(0x5e3d, 0x5e3f);
  CYC(0x5e3f, 0x5e40); B = mem_rd(gb, HL);
  CYC(0x5e40, 0x5e42); mem_wr(gb, HL, 0xda);
  CYC(0x5e42, 0x5e44); H = (uint8_t)(wRoomCollisions >> 8);
  CYC(0x5e44, 0x5e46); mem_wr(gb, HL, 0x0f);
  CYC(0x5e46, 0x5e48); E = 0x32;
  CYC(0x5e48, 0x5e49); A = L;
  CYC(0x5e49, 0x5e4a); mem_wr(gb, DE, A);
  CYC(0x5e4a, 0x5e4b); C = A;
  CALL_C(0x5e4b, setTileInRoomLayoutBuffer_hook, 0x1426, 0x5e4e);
  CYC(0x5e4e, 0x5e4f); alu_xor(gb, A);
  CYC(0x5e4f, 0x5e50); ret_effect(gb);
}

void itemCode18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cac, 0x5cae); E = 0x04;
  CYC(0x5cae, 0x5caf); A = mem_rd(gb, DE);
  CYC(0x5caf, 0x5cb0); push_effect(gb, 0x5cb0);
  switch (somaria_jump_table(gb)) {
    case 0x5cba: break;
    case 0x5cd2: goto state1;
    case 0x5d64: goto state2;
    case 0x5d9f: goto state3;
    case 0x5d11: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5cba, itemMergeZPositionIfSidescrollingArea_hook, 0x4a4f, 0x5cbd);
  CYC(0x5cbd, 0x5cc0); push_effect(gb, 0x5cc0); somaria_align_on_tile(gb, sp0_);
  CALL_C(0x5cc0, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5cc3);
  CYC(0x5cc3, 0x5cc4); alu_xor(gb, A);
  CALL_C(0x5cc4, itemSetAnimation_hook, 0x49e2, 0x5cc7);
  CALL_C(0x5cc7, itemIncState_hook, 0x23ea, 0x5cca);
  CYC(0x5cca, 0x5ccc); A = 0x7b;
  CALL_C(0x5ccc, playSound_b00_hook, 0x0c98, 0x5ccf);
  CYC(0x5ccf, 0x5cd2); objectSetVisible83_hook(gb);
  return;

state1:
  CYC(0x5cd2, 0x5cd5); push_effect(gb, 0x5cd5); somaria_check_block_can_appear(gb, sp0_);
  if (F & FZ) {
    CYCT(0x5cd5, 0x5cd8); push_effect(gb, 0x5cd8); somaria_push_link_away(gb);
  } else {
    CYC(0x5cd5, 0x5cd8);
  }
  CALL_C(0x5cd8, itemAnimate_hook, 0x49d9, 0x5cdb);
  CYC(0x5cdb, 0x5cdd); E = 0x21;
  CYC(0x5cdd, 0x5cde); A = mem_rd(gb, DE);
  CYC(0x5cde, 0x5cdf); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5cdf, 0x5ce0); ret_effect(gb); return;
  }
  CYC(0x5cdf, 0x5ce0);
  CYC(0x5ce0, 0x5ce1); H = D;
  CYC(0x5ce1, 0x5ce3); L = 0x1b;
  CYC(0x5ce3, 0x5ce5); A = 0x0d;
  CYC(0x5ce5, 0x5ce6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5ce6, 0x5ce7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5ce7, 0x5ce9); mem_wr(gb, HL, 0x36);
  CYC(0x5ce9, 0x5ceb); L = 0x24;
  CYC(0x5ceb, 0x5ced); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

check_create_block:
  CYC(0x5ced, 0x5cf0); push_effect(gb, 0x5cf0); somaria_check_block_can_appear(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x5cf0, 0x5cf2);
    goto delete_with_puff;
  }
  CYC(0x5cf0, 0x5cf2);
  CYC(0x5cf2, 0x5cf5); push_effect(gb, 0x5cf5); somaria_create_block_if_not_on_hazard(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x5cf5, 0x5cf7);
    goto delete_with_puff;
  }
  CYC(0x5cf5, 0x5cf7);
  CYC(0x5cf7, 0x5cf8); H = D;
  CYC(0x5cf8, 0x5cfa); L = 0x0f;
  CYC(0x5cfa, 0x5cfb); mem_wr(gb, HL, A);
  CYC(0x5cfb, 0x5cfd); L = 0x05;
  CYC(0x5cfd, 0x5cfe); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5cfe, 0x5d00); mem_wr(gb, HL, 0x03);
  CYC(0x5d00, 0x5d02); L = 0x26;
  CYC(0x5d02, 0x5d04); A = 0x04;
  CYC(0x5d04, 0x5d05); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5d05, 0x5d06); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5d06, 0x5d08); L = 0x2f;
  CYC(0x5d08, 0x5d09); A = mem_rd(gb, HL);
  CYC(0x5d09, 0x5d0b); alu_and(gb, 0xf0);
  CYC(0x5d0b, 0x5d0c); mem_wr(gb, HL, A);
  CYC(0x5d0c, 0x5d0e); A = 0x01;
  CYC(0x5d0e, 0x5d11); itemSetAnimation_hook(gb);
  return;

state4:
  CYC(0x5d11, 0x5d13); E = 0x05;
  CYC(0x5d13, 0x5d14); A = mem_rd(gb, DE);
  CYC(0x5d14, 0x5d15); push_effect(gb, 0x5d15);
  switch (somaria_jump_table(gb)) {
    case 0x5d19: break;
    case 0x5d3a: goto state4_substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5d19, itemIncSubstate_hook, 0x23ef, 0x5d1c);
  CALL_C(0x5d1c, itemUpdateAngle_hook, 0x2cf0, 0x5d1f);
  CYC(0x5d1f, 0x5d22); SET_BC(0x1420);
  CYC(0x5d22, 0x5d25); A = W8(wBraceletLevel);
  CYC(0x5d25, 0x5d27); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x5d27, 0x5d29);
  } else {
    CYC(0x5d27, 0x5d29);
    CYC(0x5d29, 0x5d2c); SET_BC(0x1e15);
  }
  CYC(0x5d2c, 0x5d2e); L = 0x10;
  CYC(0x5d2e, 0x5d2f); mem_wr(gb, HL, B);
  CYC(0x5d2f, 0x5d31); L = 0x06;
  CYC(0x5d31, 0x5d32); mem_wr(gb, HL, C);
  CYC(0x5d32, 0x5d34); A = 0x71;
  CALL_C(0x5d34, playSound_b00_hook, 0x0c98, 0x5d37);
  CYC(0x5d37, 0x5d3a); push_effect(gb, 0x5d3a); somaria_remove_block(gb, sp0_);

state4_substate1:
  CALL_C(0x5d3a, itemUpdateDamageToApply_hook, 0x49c8, 0x5d3d);
  if (F & FC) {
    CYCT(0x5d3d, 0x5d3f);
    goto delete_with_puff;
  }
  CYC(0x5d3d, 0x5d3f);
  CYC(0x5d3f, 0x5d42); push_effect(gb, 0x5d42); somaria_check_deletion_trigger(gb);
  if (!(F & FZ)) {
    CYCT(0x5d42, 0x5d44);
    goto delete_with_puff;
  }
  CYC(0x5d42, 0x5d44);
  CALL_C(0x5d44, objectApplySpeed_hook, 0x201d, 0x5d47);
  CYC(0x5d47, 0x5d4a); push_effect(gb, 0x5d4a); somaria_push_link_away(gb);
  CALL_C(0x5d4a, itemDecCounter1_hook, 0x23d6, 0x5d4d);
  CYC(0x5d4d, 0x5d4f); L = 0x26;
  CYC(0x5d4f, 0x5d51); A = 0x04;
  CYC(0x5d51, 0x5d52); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5d52, 0x5d53); mem_wr(gb, HL, A);
  if (!(F & FZ)) {
    CYCT(0x5d53, 0x5d54); ret_effect(gb); return;
  }
  CYC(0x5d53, 0x5d54);
  CYC(0x5d54, 0x5d56);
  goto check_create_block;

remove_block_and_delete:
  CYC(0x5d56, 0x5d59); push_effect(gb, 0x5d59); somaria_remove_block(gb, sp0_);

delete_with_puff:
  CYC(0x5d59, 0x5d5a); H = D;
  CYC(0x5d5a, 0x5d5c); L = 0x2f;
  CYC(0x5d5c, 0x5d5e); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(0x5d5e, objectCreatePuff_hook, 0x24c1, 0x5d61);
  else CYC(0x5d5e, 0x5d61);

delete_self:
  CYC(0x5d61, 0x5d64); itemDelete_hook(gb);
  return;

state2:
  CYC(0x5d64, 0x5d66); E = 0x05;
  CYC(0x5d66, 0x5d67); A = mem_rd(gb, DE);
  CYC(0x5d67, 0x5d68); push_effect(gb, 0x5d68);
  switch (somaria_jump_table(gb)) {
    case 0x5d70: break;
    case 0x5d7e: goto state2_substate1;
    case 0x5d87: goto state2_substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5d70, itemIncSubstate_hook, 0x23ef, 0x5d73);
  CYC(0x5d73, 0x5d76); push_effect(gb, 0x5d76); somaria_remove_block(gb, sp0_);
  CALL_C(0x5d76, objectSetVisiblec1_hook, 0x1e3c, 0x5d79);
  CYC(0x5d79, 0x5d7b); A = 0x02;
  CYC(0x5d7b, 0x5d7e); itemSetAnimation_hook(gb);
  return;

state2_substate1:
  CALL_C(0x5d7e, itemUpdateDamageToApply_hook, 0x49c8, 0x5d81);
  if (!(F & FC)) {
    CYCT(0x5d81, 0x5d82); ret_effect(gb); return;
  }
  CYC(0x5d81, 0x5d82);
  CALL_C(0x5d82, dropLinkHeldItem_hook, 0x2c43, 0x5d85);
  CYC(0x5d85, 0x5d87);
  goto delete_with_puff;

state2_substate2:
  CALL_C(0x5d87, objectCheckWithinRoomBoundary_hook, 0x219f, 0x5d8a);
  if (!(F & FC)) {
    CYCT(0x5d8a, 0x5d8c);
    goto delete_self;
  }
  CYC(0x5d8a, 0x5d8c);
  CALL_C(0x5d8c, bombUpdateThrowingLaterally_hook, 0x639c, 0x5d8f);
  CYC(0x5d8f, 0x5d92); push_effect(gb, 0x5d92); somaria_check_deletion_trigger(gb);
  if (!(F & FZ)) {
    CYCT(0x5d92, 0x5d94);
    goto delete_with_puff;
  }
  CYC(0x5d92, 0x5d94);
  CYC(0x5d94, 0x5d96); L = 0x39;
  CYC(0x5d96, 0x5d97); C = mem_rd(gb, HL);
  CALL_C(0x5d97, itemUpdateThrowingVerticallyAndCheckHazards_hook, 0x4b3c, 0x5d9a);
  if (F & FC) {
    CYCT(0x5d9a, 0x5d9c);
    goto delete_self;
  }
  CYC(0x5d9a, 0x5d9c);
  if (F & FZ) {
    CYCT(0x5d9c, 0x5d9d); ret_effect(gb); return;
  }
  CYC(0x5d9c, 0x5d9d);
  CYC(0x5d9d, 0x5d9f);
  goto delete_with_puff;

state3:
  CYC(0x5d9f, 0x5da2); push_effect(gb, 0x5da2); somaria_check_block_in_place(gb);
  if (!(F & FZ)) {
    CYCT(0x5da2, 0x5da4);
    goto delete_with_puff;
  }
  CYC(0x5da2, 0x5da4);
  CALL_C(0x5da4, itemUpdateDamageToApply_hook, 0x49c8, 0x5da7);
  if (F & FC) {
    CYCT(0x5da7, 0x5da9);
    goto remove_block_and_delete;
  }
  CYC(0x5da7, 0x5da9);
  CYC(0x5da9, 0x5dac); push_effect(gb, 0x5dac); somaria_check_deletion_trigger(gb);
  if (!(F & FZ)) {
    CYCT(0x5dac, 0x5dae);
    goto remove_block_and_delete;
  }
  CYC(0x5dac, 0x5dae);
  CYC(0x5dae, 0x5db1); A = W8(wActiveTilePos);
  CYC(0x5db1, 0x5db3); L = 0x32;
  CYC(0x5db3, 0x5db4); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5db4, 0x5db6);
    goto remove_block_and_delete;
  }
  CYC(0x5db4, 0x5db6);
  CYC(0x5db6, 0x5db9); A = W8(wTilesetFlags);
  CYC(0x5db9, 0x5dbb); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x5dbb, 0x5dbd);
  } else {
    CYC(0x5dbb, 0x5dbd);
    CYC(0x5dbd, 0x5dbe); A = mem_rd(gb, HL);
    CYC(0x5dbe, 0x5dc0); alu_add(gb, 0x10);
    CYC(0x5dc0, 0x5dc1); C = A;
    CYC(0x5dc1, 0x5dc3); B = (uint8_t)(wRoomCollisions >> 8);
    CYC(0x5dc3, 0x5dc4); A = mem_rd(gb, BC);
    CYC(0x5dc4, 0x5dc6); alu_cp(gb, 0x0f);
    if (!(F & FZ)) {
      CYCT(0x5dc6, 0x5dc8);
      goto remove_block_and_delete;
    }
    CYC(0x5dc6, 0x5dc8);
  }
  CYC(0x5dc8, 0x5dca); L = 0x2f;
  CYC(0x5dca, 0x5dcc); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5dcc, 0x5dcf); objectAddToGrabbableObjectBuffer_hook(gb); return;
  }
  CYC(0x5dcc, 0x5dcf);
  CYC(0x5dcf, 0x5dd1); A = 0x04;
  CYC(0x5dd1, 0x5dd4); itemSetState_hook(gb);
}
