#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemNextAnimationFrame_hook(GB *gb);
void itemMergeZPositionIfSidescrollingArea_hook(GB *gb);
void itemUpdateThrowingVertically_hook(GB *gb);

static void item_update_throwing_check_hole_or_water(GB *gb, uint16_t sp0_);

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

void itemSetVar3cToFF_hook(GB *gb) {
  CYC(0x49c2, 0x49c4); E = 0x3c;
  CYC(0x49c4, 0x49c6); A = 0xff;
  CYC(0x49c6, 0x49c7); mem_wr(gb, DE, A);
  CYC(0x49c7, 0x49c8); ret_effect(gb);
}

void itemUpdateDamageToApply_hook(GB *gb) {
  CYC(0x49c8, 0x49c9); H = D;
  CYC(0x49c9, 0x49cb); L = 0x25;
  CYC(0x49cb, 0x49cc); A = mem_rd(gb, HL);
  CYC(0x49cc, 0x49ce); mem_wr(gb, HL, 0x00);
  CYC(0x49ce, 0x49d0); L = 0x29;
  CYC(0x49d0, 0x49d1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x49d1, 0x49d2); mem_wr(gb, HL, A);
  CYC(0x49d2, 0x49d3); alu_rlca(gb);
  CYC(0x49d3, 0x49d5); L = 0x2a;
  CYC(0x49d5, 0x49d6); A = mem_rd(gb, HL);
  CYC(0x49d6, 0x49d7); A = alu_dec8(gb, A);
  CYC(0x49d7, 0x49d8); A = alu_inc8(gb, A);
  CYC(0x49d8, 0x49d9); ret_effect(gb);
}

void itemAnimate_hook(GB *gb) {
  CYC(0x49d9, 0x49da); H = D;
  CYC(0x49da, 0x49dc); L = 0x20;
  CYC(0x49dc, 0x49dd); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x49dd, 0x49de); ret_effect(gb); return;
  }
  CYC(0x49dd, 0x49de);
  CYC(0x49de, 0x49e0); L = 0x22;
  CYC(0x49e0, 0x49e2); itemNextAnimationFrame_hook(gb);
}

void itemSetAnimation_hook(GB *gb) {
  CYC(0x49e2, 0x49e3); alu_add(gb, A);
  CYC(0x49e3, 0x49e4); C = A;
  CYC(0x49e4, 0x49e6); B = 0x00;
  CYC(0x49e6, 0x49e8); E = 0x01;
  CYC(0x49e8, 0x49e9); A = mem_rd(gb, DE);
  CYC(0x49e9, 0x49ec); SET_HL(0x6663);
  CYC(0x49ec, 0x49ed); item_add_double_index_from_rst(gb, 0x49ed);
  CYC(0x49ed, 0x49ee); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49ee, 0x49ef); H = mem_rd(gb, HL);
  CYC(0x49ef, 0x49f0); L = A;
  CYC(0x49f0, 0x49f1); alu_add_hl(gb, BC);
  itemNextAnimationFrame_hook(gb);
}

void itemNextAnimationFrame_hook(GB *gb) {
  CYC(0x49f1, 0x49f2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49f2, 0x49f3); H = mem_rd(gb, HL);
  CYC(0x49f3, 0x49f4); L = A;
  CYC(0x49f4, 0x49f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49f5, 0x49f7); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x49f7, 0x49f9);
  } else {
    CYC(0x49f7, 0x49f9);
    CYC(0x49f9, 0x49fa); B = A;
    CYC(0x49fa, 0x49fb); C = mem_rd(gb, HL);
    CYC(0x49fb, 0x49fc); alu_add_hl(gb, BC);
    CYC(0x49fc, 0x49fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  }
  CYC(0x49fd, 0x49ff); E = 0x20;
  CYC(0x49ff, 0x4a00); mem_wr(gb, DE, A);
  CYC(0x4a00, 0x4a01); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a01, 0x4a02); C = A;
  CYC(0x4a02, 0x4a04); B = 0x00;
  CYC(0x4a04, 0x4a05); E = alu_inc8(gb, E);
  CYC(0x4a05, 0x4a06); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a06, 0x4a07); mem_wr(gb, DE, A);
  CYC(0x4a07, 0x4a08); E = alu_inc8(gb, E);
  CYC(0x4a08, 0x4a09); A = L;
  CYC(0x4a09, 0x4a0a); mem_wr(gb, DE, A);
  CYC(0x4a0a, 0x4a0b); E = alu_inc8(gb, E);
  CYC(0x4a0b, 0x4a0c); A = H;
  CYC(0x4a0c, 0x4a0d); mem_wr(gb, DE, A);
  CYC(0x4a0d, 0x4a0f); E = 0x01;
  CYC(0x4a0f, 0x4a10); A = mem_rd(gb, DE);
  CYC(0x4a10, 0x4a13); SET_HL(0x66c3);
  CYC(0x4a13, 0x4a14); item_add_double_index_from_rst(gb, 0x4a14);
  CYC(0x4a14, 0x4a15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a15, 0x4a16); H = mem_rd(gb, HL);
  CYC(0x4a16, 0x4a17); L = A;
  CYC(0x4a17, 0x4a18); alu_add_hl(gb, BC);
  CYC(0x4a18, 0x4a1a); E = 0x1e;
  CYC(0x4a1a, 0x4a1b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a1b, 0x4a1c); mem_wr(gb, DE, A);
  CYC(0x4a1c, 0x4a1d); E = alu_inc8(gb, E);
  CYC(0x4a1d, 0x4a1e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a1e, 0x4a20); alu_and(gb, 0x3f);
  CYC(0x4a20, 0x4a21); mem_wr(gb, DE, A);
  CYC(0x4a21, 0x4a22); ret_effect(gb);
}

void itemTransferKnockbackToLink_hook(GB *gb) {
  CYC(0x4a22, 0x4a23); H = D;
  CYC(0x4a23, 0x4a25); L = 0x2d;
  CYC(0x4a25, 0x4a26); A = mem_rd(gb, HL);
  CYC(0x4a26, 0x4a27); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4a27, 0x4a28); ret_effect(gb); return;
  }
  CYC(0x4a27, 0x4a28);
  CYC(0x4a28, 0x4a2a); mem_wr(gb, HL, 0x00);
  CYC(0x4a2a, 0x4a2b); L = alu_dec8(gb, L);
  CYC(0x4a2b, 0x4a2c); B = mem_rd(gb, HL);
  CYC(0x4a2c, 0x4a2f); SET_HL(0xd02d);
  CYC(0x4a2f, 0x4a30); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x4a30, 0x4a32);
  } else {
    CYC(0x4a30, 0x4a32);
    CYC(0x4a32, 0x4a33); mem_wr(gb, HL, A);
  }
  CYC(0x4a33, 0x4a34); L = alu_dec8(gb, L);
  CYC(0x4a34, 0x4a35); mem_wr(gb, HL, B);
  CYC(0x4a35, 0x4a36); ret_effect(gb);
}

void applyOffsetTableHL_hook(GB *gb) {
  CYC(0x4a36, 0x4a38); E = 0x08;
  CYC(0x4a38, 0x4a39); A = mem_rd(gb, DE);
  CYC(0x4a39, 0x4a3a); E = A;
  CYC(0x4a3a, 0x4a3b); alu_add(gb, A);
  CYC(0x4a3b, 0x4a3c); alu_add(gb, E);
  CYC(0x4a3c, 0x4a3d); item_add_a_to_hl_from_rst(gb, 0x4a3d);
  CYC(0x4a3d, 0x4a3f); E = 0x0b;
  CYC(0x4a3f, 0x4a40); A = mem_rd(gb, DE);
  CYC(0x4a40, 0x4a41); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4a41, 0x4a42); mem_wr(gb, DE, A);
  CYC(0x4a42, 0x4a43); SET_HL(HL + 1);
  CYC(0x4a43, 0x4a45); E = 0x0d;
  CYC(0x4a45, 0x4a46); A = mem_rd(gb, DE);
  CYC(0x4a46, 0x4a47); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4a47, 0x4a48); mem_wr(gb, DE, A);
  CYC(0x4a48, 0x4a49); SET_HL(HL + 1);
  CYC(0x4a49, 0x4a4b); E = 0x0f;
  CYC(0x4a4b, 0x4a4c); A = mem_rd(gb, DE);
  CYC(0x4a4c, 0x4a4d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4a4d, 0x4a4e); mem_wr(gb, DE, A);
  CYC(0x4a4e, 0x4a4f); ret_effect(gb);
}

void itemMergeZPositionIfSidescrollingArea_hook(GB *gb) {
  CYC(0x4a4f, 0x4a50); H = D;
  CYC(0x4a50, 0x4a53); A = W8(wTilesetFlags);
  CYC(0x4a53, 0x4a55); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x4a55, 0x4a56); ret_effect(gb); return;
  }
  CYC(0x4a55, 0x4a56);
  CYC(0x4a56, 0x4a58); E = 0x0b;
  CYC(0x4a58, 0x4a5a); L = 0x0f;
  CYC(0x4a5a, 0x4a5b); A = mem_rd(gb, DE);
  CYC(0x4a5b, 0x4a5c); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4a5c, 0x4a5d); mem_wr(gb, DE, A);
  CYC(0x4a5d, 0x4a5e); alu_xor(gb, A);
  CYC(0x4a5e, 0x4a5f); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4a5f, 0x4a60); mem_wr(gb, HL, A);
  CYC(0x4a60, 0x4a61); alu_or(gb, D);
  CYC(0x4a61, 0x4a62); ret_effect(gb);
}

void itemUpdateSpeedZAndCheckHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4a62, 0x4a64); E = 0x0f;
  CYC(0x4a64, 0x4a65); A = E;
  CYC(0x4a65, 0x4a67); H8(hFF8B) = A;
  CYC(0x4a67, 0x4a68); A = mem_rd(gb, DE);
  CYC(0x4a68, 0x4a69); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x4a69, 0x4a6b);
  } else {
    CYC(0x4a69, 0x4a6b);
    CYC(0x4a6b, 0x4a6c); alu_rrca(gb);
    CYC(0x4a6c, 0x4a6e); H8(hFF8B) = A;
    CALL_C(0x4a6e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4a71);
    if (!(F & FZ)) {
      CYCT(0x4a71, 0x4a73);
      goto above_ground;
    }
    CYC(0x4a71, 0x4a73);
    CYC(0x4a73, 0x4a75); H8(hFF8B) = A;
  }
  CALL_C(0x4a75, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x4a78);
  if (!(F & FC)) {
    CYCT(0x4a78, 0x4a7a);
    goto above_ground;
  }
  CYC(0x4a78, 0x4a7a);
  CYC(0x4a7a, 0x4a7b); SET_HL(pop_effect(gb));
  CYC(0x4a7b, 0x4a7d); A = 0xff;
  CYC(0x4a7d, 0x4a7e); ret_effect(gb);
  return;

above_ground:
  CYC(0x4a7e, 0x4a80); A = H8(hFF8B);
  CYC(0x4a80, 0x4a81); alu_rlca(gb);
  CYC(0x4a81, 0x4a82); alu_or(gb, A);
  CYC(0x4a82, 0x4a83); ret_effect(gb);
}

void bombPullTowardPoint_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4a83, 0x4a84); H = D;
  CYC(0x4a84, 0x4a86); L = 0x0f;
  CYC(0x4a86, 0x4a88); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x4a88, 0x4a8a);
    goto end;
  }
  CYC(0x4a88, 0x4a8a);
  CYC(0x4a8a, 0x4a8c); L = 0x31;
  CYC(0x4a8c, 0x4a8d); B = mem_rd(gb, HL);
  CYC(0x4a8d, 0x4a8e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4a8e, 0x4a8f); C = mem_rd(gb, HL);
  CYC(0x4a8f, 0x4a90); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4a90, 0x4a91); alu_or(gb, B);
  if (F & FZ) {
    CYCT(0x4a91, 0x4a92); ret_effect(gb); return;
  }
  CYC(0x4a91, 0x4a92);
  CYC(0x4a92, 0x4a93); push_effect(gb, BC);
  CALL_C(0x4a93, objectCheckContainsPoint_hook, 0x1bdb, 0x4a96);
  CYC(0x4a96, 0x4a97); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(0x4a97, 0x4a98); ret_effect(gb); return;
  }
  CYC(0x4a97, 0x4a98);
  CALL_C(0x4a98, objectGetRelativeAngle_hook, 0x1ea4, 0x4a9b);
  CYC(0x4a9b, 0x4a9c); C = A;
  CYC(0x4a9c, 0x4a9e); B = 0x0a;
  CYC(0x4a9e, 0x4aa0); E = 0x09;
  CALL_C(0x4aa0, objectApplyGivenSpeed_hook, 0x2029, 0x4aa3);

end:
  CYC(0x4aa3, 0x4aa4); alu_xor(gb, A);
  CYC(0x4aa4, 0x4aa5); ret_effect(gb);
}

void itemUpdateThrowingVertically_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x4aa5, itemMergeZPositionIfSidescrollingArea_hook, 0x4a4f, 0x4aa8);
  if (!(F & FZ)) {
    CYCT(0x4aa8, 0x4aaa);
    goto sidescrolling;
  }
  CYC(0x4aa8, 0x4aaa);
  CALL_C(0x4aaa, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4aad);
  if (!(F & FZ)) {
    CYCT(0x4aad, 0x4aaf);
    goto unset_collision;
  }
  CYC(0x4aad, 0x4aaf);
  CYC(0x4aaf, 0x4ab2); push_effect(gb, 0x4ab2);
  item_update_throwing_check_hole_or_water(gb, gb->sp);
  CYC(0x4ab2, 0x4ab4); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(0x4ab4, 0x4ab6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x4ab6, 0x4ab7); alu_scf(gb);
  CYC(0x4ab7, 0x4ab8); ret_effect(gb);
  return;

unset_collision:
  CYC(0x4ab8, 0x4aba); L = 0x3b;
  CYC(0x4aba, 0x4abc); mem_wr(gb, HL, mem_rd(gb, HL) & 0xef);
  CYC(0x4abc, 0x4abd); alu_or(gb, D);
  CYC(0x4abd, 0x4abe); ret_effect(gb);
  return;

set_collision:
  CYC(0x4abe, 0x4abf); H = D;
  CYC(0x4abf, 0x4ac1); L = 0x3b;
  CYC(0x4ac1, 0x4ac3); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(0x4ac3, 0x4ac5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x4ac5, 0x4ac6); alu_scf(gb);
  CYC(0x4ac6, 0x4ac7); ret_effect(gb);
  return;

sidescrolling:
  CYC(0x4ac7, 0x4ac8); push_effect(gb, BC);
  CYC(0x4ac8, 0x4acb); push_effect(gb, 0x4acb);
  item_update_throwing_check_hole_or_water(gb, gb->sp);
  CYC(0x4acb, 0x4acd); L = 0x15;
  CYC(0x4acd, 0x4acf); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4acf, 0x4ad1);
    goto not_moving_up;
  }
  CYC(0x4acf, 0x4ad1);
  CALL_C(0x4ad1, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x4ad4);
  CYC(0x4ad4, 0x4ad5); H = D;
  CYC(0x4ad5, 0x4ad6); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x4ad6, 0x4ad8);
    goto no_ceiling_collision;
  }
  CYC(0x4ad6, 0x4ad8);
  CYC(0x4ad8, 0x4ada); B = 0x03;
  CYC(0x4ada, 0x4adc);
  goto update_gravity;

not_moving_up:
  CYC(0x4adc, 0x4ade); L = 0x0b;
  CYC(0x4ade, 0x4adf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4adf, 0x4ae1); alu_add(gb, 0x05);
  CYC(0x4ae1, 0x4ae2); B = A;
  CYC(0x4ae2, 0x4ae3); L = alu_inc8(gb, L);
  CYC(0x4ae3, 0x4ae4); C = mem_rd(gb, HL);
  CALL_C(0x4ae4, checkTileCollisionAt_allowHoles_hook, 0x14d1, 0x4ae7);
  CYC(0x4ae7, 0x4ae8); H = D;
  CYC(0x4ae8, 0x4ae9); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(0x4ae9, 0x4aeb);
    goto set_collision;
  }
  CYC(0x4ae9, 0x4aeb);

no_ceiling_collision:
  CYC(0x4aeb, 0x4aed); L = 0x3b;
  CYC(0x4aed, 0x4aef); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x4aef, 0x4af1); B = 0x03;
  if (F & FZ) {
    CYCT(0x4af1, 0x4af3);
  } else {
    CYC(0x4af1, 0x4af3);
    CYC(0x4af3, 0x4af5); B = 0x01;
    CYC(0x4af5, 0x4af7); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(0x4af7, 0x4af9);
      goto unset_collision;
    }
    CYC(0x4af7, 0x4af9);
  }
  CYC(0x4af9, 0x4afb); E = 0x14;
  CYC(0x4afb, 0x4afd); L = 0x0a;
  CYC(0x4afd, 0x4afe); A = mem_rd(gb, DE);
  CYC(0x4afe, 0x4aff); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4aff, 0x4b00); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b00, 0x4b01); E = alu_inc8(gb, E);
  CYC(0x4b01, 0x4b02); A = mem_rd(gb, DE);
  CYC(0x4b02, 0x4b03); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x4b03, 0x4b04); mem_wr(gb, HL, A); SET_HL(HL + 1);

update_gravity:
  CYC(0x4b04, 0x4b06); L = 0x14;
  CYC(0x4b06, 0x4b07); A = mem_rd(gb, HL);
  CYC(0x4b07, 0x4b08); alu_add(gb, C);
  CYC(0x4b08, 0x4b09); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b09, 0x4b0a); A = mem_rd(gb, HL);
  CYC(0x4b0a, 0x4b0c); alu_adc(gb, 0x00);
  CYC(0x4b0c, 0x4b0d); mem_wr(gb, HL, A);
  CYC(0x4b0d, 0x4b0f); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x4b0f, 0x4b11);
    goto unset_collision;
  }
  CYC(0x4b0f, 0x4b11);
  CYC(0x4b11, 0x4b12); alu_cp(gb, B);
  if (F & FC) {
    CYCT(0x4b12, 0x4b14);
    goto unset_collision;
  }
  CYC(0x4b12, 0x4b14);
  CYC(0x4b14, 0x4b15); mem_wr(gb, HL, B);
  CYC(0x4b15, 0x4b16); L = alu_dec8(gb, L);
  CYC(0x4b16, 0x4b18); mem_wr(gb, HL, 0x00);
  CYC(0x4b18, 0x4b1a);
  goto unset_collision;
}

static void item_update_throwing_check_hole_or_water(GB *gb, uint16_t sp0_) {
  CALL_C(0x4b1a, itemMergeZPositionIfSidescrollingArea_hook, 0x4a4f, 0x4b1d);
  if (!(F & FZ)) {
    CYCT(0x4b1d, 0x4b1f);
  } else {
    CYC(0x4b1d, 0x4b1f);
    CYC(0x4b1f, 0x4b21); L = 0x0f;
    CYC(0x4b21, 0x4b23); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(0x4b23, 0x4b25);
      goto update_collision;
    }
    CYC(0x4b23, 0x4b25);
  }
  CALL_C(0x4b25, objectCheckIsOverHazard_hook, 0x2216, 0x4b28);
  CYC(0x4b28, 0x4b29); H = D;

update_collision:
  CYC(0x4b29, 0x4b2a); B = A;
  CYC(0x4b2a, 0x4b2c); L = 0x3b;
  CYC(0x4b2c, 0x4b2d); A = mem_rd(gb, HL);
  CYC(0x4b2d, 0x4b2e); C = A;
  CYC(0x4b2e, 0x4b30); alu_and(gb, 0xb8);
  CYC(0x4b30, 0x4b32); alu_xor(gb, 0x80);
  CYC(0x4b32, 0x4b33); alu_or(gb, B);
  CYC(0x4b33, 0x4b34); mem_wr(gb, HL, A);
  CYC(0x4b34, 0x4b35); A = B;
  CYC(0x4b35, 0x4b36); alu_xor(gb, C);
  CYC(0x4b36, 0x4b37); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x4b37, 0x4b39);
  } else {
    CYC(0x4b37, 0x4b39);
    CYC(0x4b39, 0x4b3b); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  }
  CYC(0x4b3b, 0x4b3c); ret_effect(gb);
}

static void item_update_throwing_create_splash(GB *gb, uint16_t sp0_) {
  CALL_C(0x4b77, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x4b7a);
  CYC(0x4b7a, 0x4b7b); alu_scf(gb);
  CYC(0x4b7b, 0x4b7c); ret_effect(gb);
}

void itemUpdateThrowingVerticallyAndCheckHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CALL_C(0x4b3c, itemUpdateThrowingVertically_hook, 0x4aa5, 0x4b3f);
  if (F & FC) {
    CYCT(0x4b3f, 0x4b41);
    goto landed;
  }
  CYC(0x4b3f, 0x4b41);
  CYC(0x4b41, 0x4b44); A = W8(wTilesetFlags);
  CYC(0x4b44, 0x4b46); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x4b46, 0x4b48);
    goto no_hazard;
  }
  CYC(0x4b46, 0x4b48);
  CYC(0x4b48, 0x4b4a); B = 0x04;
  CYC(0x4b4a, 0x4b4c); alu_bit(gb, 2, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4b4c, 0x4b4e);
    item_update_throwing_create_splash(gb, sp0_);
    return;
  }
  CYC(0x4b4c, 0x4b4e);
  CYC(0x4b4e, 0x4b50); B = 0x03;
  CYC(0x4b50, 0x4b52); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4b52, 0x4b55); push_effect(gb, 0x4b55);
    item_update_throwing_create_splash(gb, gb->sp);
  } else {
    CYC(0x4b52, 0x4b55);
  }

no_hazard:
  CYC(0x4b55, 0x4b56); alu_xor(gb, A);
  CYC(0x4b56, 0x4b57); ret_effect(gb);
  return;

landed:
  CYC(0x4b57, 0x4b5a); A = W8(wTilesetFlags);
  CYC(0x4b5a, 0x4b5c); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x4b5c, 0x4b5e);
    goto no_collisions;
  }
  CYC(0x4b5c, 0x4b5e);
  CYC(0x4b5e, 0x4b5f); H = D;
  CYC(0x4b5f, 0x4b61); L = 0x3b;
  CYC(0x4b61, 0x4b63); B = 0x03;
  CYC(0x4b63, 0x4b65); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4b65, 0x4b67);
    item_update_throwing_create_splash(gb, sp0_);
    return;
  }
  CYC(0x4b65, 0x4b67);
  CYC(0x4b67, 0x4b69); B = 0x0f;
  CYC(0x4b69, 0x4b6b); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4b6b, 0x4b6d);
    goto create_hole_animation;
  }
  CYC(0x4b6b, 0x4b6d);
  CYC(0x4b6d, 0x4b6f); B = 0x04;
  CYC(0x4b6f, 0x4b71); alu_bit(gb, 2, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4b71, 0x4b73);
    item_update_throwing_create_splash(gb, sp0_);
    return;
  }
  CYC(0x4b71, 0x4b73);

no_collisions:
  CYC(0x4b73, 0x4b74); alu_xor(gb, A);
  CYC(0x4b74, 0x4b76); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(0x4b76, 0x4b77); ret_effect(gb);
  return;

create_hole_animation:
  CALL_C(0x4b7c, objectCreateFallingDownHoleInteraction_hook, 0x24d1, 0x4b7f);
  CYC(0x4b7f, 0x4b80); alu_scf(gb);
  CYC(0x4b80, 0x4b81); ret_effect(gb);
}

void objectCreateClinkInteraction_hook(GB *gb) {
  CYC(0x4b81, 0x4b83); B = 0x07;
  CYC(0x4b83, 0x4b86); objectCreateInteractionWithSubid00_hook(gb);
}
