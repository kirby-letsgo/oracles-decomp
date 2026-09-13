#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t sword_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static bool sword_parent_check_and_ret_for_poke(GB *gb, uint16_t sp0_) {
  CYC(0x4d07, 0x4d08); alu_xor(gb, A);
  CYC(0x4d08, 0x4d0a); E = 0x02;
  CYC(0x4d0a, 0x4d0b); mem_wr(gb, DE, A);
  CYC(0x4d0b, 0x4d0e); A = W8(w1WeaponItem_var2a);
  CYC(0x4d0e, 0x4d10); alu_cp(gb, 0x04);
  if (F & FZ) CYCT(0x4d10, 0x4d12);
  else {
    CYC(0x4d10, 0x4d12);
    CYC(0x4d12, 0x4d13); alu_or(gb, A);
    if (!(F & FZ)) CYCT(0x4d13, 0x4d15);
    else {
      CYC(0x4d13, 0x4d15);
      CALL_C(0x4d15, checkLinkPushingAgainstWall_hook, 0x2b39, 0x4d18);
      if (!(F & FC)) {
        CYCT(0x4d18, 0x4d19);
        ret_effect(gb);
        return false;
      }
      CYC(0x4d18, 0x4d19);
    }
    if (!(F & FZ)) goto escape;
  }
  CYC(0x4d19, 0x4d1b); E = 0x02;
  CYC(0x4d1b, 0x4d1d); A = 0x01;
  CYC(0x4d1d, 0x4d1e); mem_wr(gb, DE, A);

escape:
  CYC(0x4d1e, 0x4d1f); SET_HL(pop_effect(gb));
  CYC(0x4d1f, 0x4d20); alu_xor(gb, A);
  CYC(0x4d20, 0x4d23); W8(w1WeaponItem_collisionType) = A;
  return true;
}

static void sword_parent_create_beam(GB *gb, uint16_t sp0_) {
  CYC(0x4d56, 0x4d59); SET_BC(0x2700);
  CYC(0x4d59, 0x4d5b); E = 0x01;
  CALL_C(0x4d5b, getFreeItemSlotWithObjectCap_hook, 0x5416, 0x4d5e);
  if (F & FC) { CYCT(0x4d5e, 0x4d5f); ret_effect(gb); return; }
  CYC(0x4d5e, 0x4d5f);
  CYC(0x4d5f, 0x4d60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d60, 0x4d61); L = alu_inc8(gb, L);
  CYC(0x4d61, 0x4d62); A = B;
  CYC(0x4d62, 0x4d63); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4d63, 0x4d64); A = C;
  CYC(0x4d64, 0x4d65); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4d65, 0x4d66); push_effect(gb, DE);
  CYC(0x4d66, 0x4d69); SET_DE(w1Link_direction);
  CYC(0x4d69, 0x4d6b); L = 0x08;
  CYC(0x4d6b, 0x4d6d); B = 0x08;
  CALL_C(0x4d6d, copyMemoryReverse_hook, 0x047f, 0x4d70);
  CYC(0x4d70, 0x4d71); SET_DE(pop_effect(gb));
  CYC(0x4d71, 0x4d72); F = (F & FZ) | FC;
  CYC(0x4d72, 0x4d73); ret_effect(gb);
}

void parentItemCode_sword_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4b82, clearParentItemIfCantUseSword_hook, 0x549e, 0x4b85);
  CYC(0x4b85, 0x4b87); E = 0x04;
  CYC(0x4b87, 0x4b88); A = mem_rd(gb, DE);
  CYC(0x4b88, 0x4b89); push_effect(gb, 0x4b89);
  switch (sword_parent_jump_table(gb)) {
    case 0x4b97: goto state0;
    case 0x4bcb: goto state1;
    case 0x4c45: goto state2;
    case 0x4c84: goto state3;
    case 0x4ccd: goto state4;
    case 0x4ce4: goto state5;
    case 0x4bf9: goto state6;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4b97, 0x4b9a); SET_HL(wcc63);
  CYC(0x4b9a, 0x4b9c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) CYCT(0x4b9c, 0x4b9e);
  else {
    CYC(0x4b9c, 0x4b9e);
    CYC(0x4b9e, 0x4ba0); mem_wr(gb, HL, 0x00);
    CALL_C(0x4ba0, updateLinkDirectionFromAngle_hook, 0x2b64, 0x4ba3);
    CYC(0x4ba3, 0x4ba6); A = W8(wLinkHealth);
    CYC(0x4ba6, 0x4ba8); alu_cp(gb, 0x05);
    if (F & FC) CYCT(0x4ba8, 0x4baa);
    else {
      CYC(0x4ba8, 0x4baa);
      CYC(0x4baa, 0x4bac); A = 0x32;
      CALL_C(0x4bac, cpActiveRing_hook, 0x23b0, 0x4baf);
      if (!(F & FZ)) CYCT(0x4baf, 0x4bb1);
      else {
        CYC(0x4baf, 0x4bb1);
        CYC(0x4bb1, 0x4bb3); E = 0x3a;
        CYC(0x4bb3, 0x4bb5); A = 0xf8;
        CYC(0x4bb5, 0x4bb6); mem_wr(gb, DE, A);
      }
    }
  }
  CYC(0x4bb6, 0x4bb9); SET_HL(w1WeaponItem_enabled);
  CYC(0x4bb9, 0x4bba); A = mem_rd(gb, HL);
  CYC(0x4bba, 0x4bbb); alu_or(gb, A);
  CYC(0x4bbb, 0x4bbd); B = 0x40;
  if (!(F & FZ)) CALL_C_CC(0x4bbd, clearMemory_hook, 0x046f, 0x4bc0);
  else CYC(0x4bbd, 0x4bc0);
  CYC(0x4bc0, 0x4bc1); H = D;
  CYC(0x4bc1, 0x4bc3); L = 0x00;
  CYC(0x4bc3, 0x4bc5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(0x4bc5, parentItemLoadAnimationAndIncState_hook, 0x5378, 0x4bc8);
  CYC(0x4bc8, 0x4bcb); itemCreateChild_hook(gb); return;

state1:
  CYC(0x4bcb, 0x4bce); A = W8(wcc63);
  CYC(0x4bce, 0x4bcf); alu_rlca(gb);
  if (F & FC) { CYCT(0x4bcf, 0x4bd2); goto spin_start; }
  CYC(0x4bcf, 0x4bd2);
  CALL_C(0x4bd2, specialObjectAnimate_optimized_hook, 0x441e, 0x4bd5);
  CYC(0x4bd5, 0x4bd6); H = D;
  CYC(0x4bd6, 0x4bd8); E = 0x21;
  CYC(0x4bd8, 0x4bd9); A = mem_rd(gb, DE);
  CYC(0x4bd9, 0x4bda); alu_or(gb, A);
  if (F & FZ) CYCT(0x4bda, 0x4bdc);
  else {
    CYC(0x4bda, 0x4bdc);
    CYC(0x4bdc, 0x4bde); L = 0x3a;
    CYC(0x4bde, 0x4be0); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) CYCT(0x4be0, 0x4be2);
    else {
      CYC(0x4be0, 0x4be2);
      CYC(0x4be2, 0x4be4); L = 0x00;
      CYC(0x4be4, 0x4be6); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
    }
  }
  CYC(0x4be6, 0x4be7); L = E;
  CYC(0x4be7, 0x4be9); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x4be9, 0x4beb); goto state6; }
  CYC(0x4be9, 0x4beb);
  CYC(0x4beb, 0x4bed); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(0x4bed, 0x4bee); ret_effect(gb); return; }
  CYC(0x4bed, 0x4bee);
  CYC(0x4bee, 0x4bf0); mem_wr(gb, HL, mem_rd(gb, HL) & 0xdf);
  CYC(0x4bf0, 0x4bf3); A = W8(wSwordLevel);
  CYC(0x4bf3, 0x4bf5); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x4bf5, 0x4bf8); goto check_create_beam; }
  CYC(0x4bf5, 0x4bf8);
  CYC(0x4bf8, 0x4bf9); ret_effect(gb); return;

state6:
  CYC(0x4bf9, 0x4bfc); A = W8(w1WeaponItem_var2a);
  CYC(0x4bfc, 0x4bfd); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4bfd, 0x4c00); goto enemy_contact; }
  CYC(0x4bfd, 0x4c00);
  CYC(0x4c00, 0x4c03); A = W8(wLinkObjectIndex);
  CYC(0x4c03, 0x4c04); alu_rrca(gb);
  if (F & FC) { CYCT(0x4c04, 0x4c07); goto delete_self; }
  CYC(0x4c04, 0x4c07);
  CALL_C(0x4c07, parentItemCheckButtonPressed_hook, 0x5496, 0x4c0a);
  if (F & FZ) { CYCT(0x4c0a, 0x4c0d); goto delete_self; }
  CYC(0x4c0a, 0x4c0d);
  CYC(0x4c0d, 0x4c0f); A = 0x01;
  CYC(0x4c0f, 0x4c12); W8(wcc63) = A;
  CYC(0x4c12, 0x4c13); A = alu_inc8(gb, A);
  CYC(0x4c13, 0x4c16); W8(w1WeaponItem_state) = A;
  CYC(0x4c16, 0x4c18); A = 0x89;
  CYC(0x4c18, 0x4c1b); W8(w1WeaponItem_collisionType) = A;
  CYC(0x4c1b, 0x4c1d); L = 0x04;
  CYC(0x4c1d, 0x4c1f); mem_wr(gb, HL, 0x02);
  CYC(0x4c1f, 0x4c20); L = alu_inc8(gb, L);
  CYC(0x4c20, 0x4c21); alu_xor(gb, A);
  CYC(0x4c21, 0x4c22); mem_wr(gb, HL, A);
  CYC(0x4c22, 0x4c24); L = 0x3a;
  CYC(0x4c24, 0x4c25); mem_wr(gb, HL, A);
  CYC(0x4c25, 0x4c27); L = 0x3f;
  CYC(0x4c27, 0x4c28); mem_wr(gb, HL, A);
  CYC(0x4c28, 0x4c2a); L = 0x06;
  CYC(0x4c2a, 0x4c2c); mem_wr(gb, HL, 0x28);
  CYC(0x4c2c, 0x4c2f); itemEnableLinkMovement_hook(gb); return;

enemy_contact:
  CYC(0x4c2f, 0x4c31); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x4c31, 0x4c34); goto delete_self; }
  CYC(0x4c31, 0x4c34);
  CYC(0x4c34, 0x4c36); E = 0x3a;
  CYC(0x4c36, 0x4c37); A = mem_rd(gb, DE);
  CYC(0x4c37, 0x4c38); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4c38, 0x4c3b); goto delete_self; }
  CYC(0x4c38, 0x4c3b);
  CYC(0x4c3b, 0x4c3e); SET_HL(w1Link_damageToApply);
  CYC(0x4c3e, 0x4c3f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4c3f, 0x4c40); mem_wr(gb, HL, A);
  CYC(0x4c40, 0x4c41); alu_xor(gb, A);
  CYC(0x4c41, 0x4c42); mem_wr(gb, DE, A);
  CYC(0x4c42, 0x4c45); goto delete_self;

state2:
  CYC(0x4c45, 0x4c48); A = W8(wLinkObjectIndex);
  CYC(0x4c48, 0x4c49); alu_rrca(gb);
  if (F & FC) { CYCT(0x4c49, 0x4c4c); goto delete_self; }
  CYC(0x4c49, 0x4c4c);
  CALL_C(0x4c4c, parentItemCheckButtonPressed_hook, 0x5496, 0x4c4f);
  if (F & FZ) { CYCT(0x4c4f, 0x4c52); goto delete_self; }
  CYC(0x4c4f, 0x4c52);
  CYC(0x4c52, 0x4c55); push_effect(gb, 0x4c55);
  if (sword_parent_check_and_ret_for_poke(gb, sp0_)) goto trigger_poke;
  CYC(0x4c55, 0x4c57); A = 0x16;
  CALL_C(0x4c57, cpActiveRing_hook, 0x23b0, 0x4c5a);
  CYC(0x4c5a, 0x4c5c); C = 0x01;
  if (!(F & FZ)) CYCT(0x4c5c, 0x4c5e);
  else { CYC(0x4c5c, 0x4c5e); CYC(0x4c5e, 0x4c60); C = 0x04; }
  CYC(0x4c60, 0x4c62); L = 0x06;
  CYC(0x4c62, 0x4c63); A = mem_rd(gb, HL);
  CYC(0x4c63, 0x4c64); alu_sub(gb, C);
  CYC(0x4c64, 0x4c65); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(0x4c65, 0x4c66); ret_effect(gb); return; }
  CYC(0x4c65, 0x4c66);
  CYC(0x4c66, 0x4c68); A = 0x31;
  CALL_C(0x4c68, cpActiveRing_hook, 0x23b0, 0x4c6b);
  if (!(F & FZ)) { CYCT(0x4c6b, 0x4c6d); goto charge_without_beam; }
  CYC(0x4c6b, 0x4c6d);
  CYC(0x4c6d, 0x4c70); push_effect(gb, 0x4c70); sword_parent_create_beam(gb, sp0_);
  CYC(0x4c70, 0x4c73); goto trigger_poke;

charge_without_beam:
  CYC(0x4c73, 0x4c75); L = 0x04;
  CYC(0x4c75, 0x4c76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4c76, 0x4c78); L = 0x00;
  CYC(0x4c78, 0x4c7a); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x4c7a, 0x4c7c); A = 0x03;
  CYC(0x4c7c, 0x4c7f); W8(w1WeaponItem_state) = A;
  CYC(0x4c7f, 0x4c81); A = 0x4f;
  CYC(0x4c81, 0x4c84); playSound_b00_hook(gb); return;

state3:
  CYC(0x4c84, 0x4c87); push_effect(gb, 0x4c87);
  if (sword_parent_check_and_ret_for_poke(gb, sp0_)) goto trigger_poke;
  CALL_C(0x4c87, parentItemCheckButtonPressed_hook, 0x5496, 0x4c8a);
  if (!(F & FZ)) { CYCT(0x4c8a, 0x4c8b); ret_effect(gb); return; }
  CYC(0x4c8a, 0x4c8b);

spin_start:
  CYC(0x4c8b, 0x4c8c); H = D;
  CYC(0x4c8c, 0x4c8e); A = 0x02;
  CYC(0x4c8e, 0x4c91); W8(wcc63) = A;
  CYC(0x4c91, 0x4c93); L = 0x04;
  CYC(0x4c93, 0x4c95); mem_wr(gb, HL, 0x04);
  CYC(0x4c95, 0x4c97); A = 0x2f;
  CALL_C(0x4c97, cpActiveRing_hook, 0x23b0, 0x4c9a);
  CYC(0x4c9a, 0x4c9c); A = 0x05;
  if (!(F & FZ)) CYCT(0x4c9c, 0x4c9e);
  else { CYC(0x4c9c, 0x4c9e); CYC(0x4c9e, 0x4ca0); A = 0x09; }
  CYC(0x4ca0, 0x4ca2); L = 0x06;
  CYC(0x4ca2, 0x4ca3); mem_wr(gb, HL, A);
  CYC(0x4ca3, 0x4ca5); L = 0x3f;
  CYC(0x4ca5, 0x4ca7); mem_wr(gb, HL, 0x0f);
  CALL_C(0x4ca7, isLinkUnderwater_hook, 0x54d2, 0x4caa);
  CYC(0x4caa, 0x4cac); C = 0x28;
  if (F & FZ) CYCT(0x4cac, 0x4cae);
  else { CYC(0x4cac, 0x4cae); CYC(0x4cae, 0x4cb0); C = 0x30; }
  CYC(0x4cb0, 0x4cb3); A = W8(w1Link_direction);
  CYC(0x4cb3, 0x4cb4); alu_add(gb, C);
  CALL_C(0x4cb4, specialObjectSetAnimationWithLinkData_hook, 0x4412, 0x4cb7);
  CYC(0x4cb7, 0x4cb8); H = D;
  CYC(0x4cb8, 0x4cba); L = 0x21;
  CYC(0x4cba, 0x4cbc); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(0x4cbc, 0x4cbf); SET_HL(w1WeaponItem_state);
  CYC(0x4cbf, 0x4cc1); mem_wr(gb, HL, 0x04);
  CYC(0x4cc1, 0x4cc3); L = 0x3a;
  CYC(0x4cc3, 0x4cc5); mem_wr(gb, HL, alu_sla(gb, mem_rd(gb, HL)));
  CALL_C(0x4cc5, itemDisableLinkMovement_hook, 0x545d, 0x4cc8);
  CYC(0x4cc8, 0x4cca); A = 0x6b;
  CYC(0x4cca, 0x4ccd); playSound_b00_hook(gb); return;

state4:
  CALL_C(0x4ccd, specialObjectAnimate_optimized_hook, 0x441e, 0x4cd0);
  CYC(0x4cd0, 0x4cd1); H = D;
  CYC(0x4cd1, 0x4cd3); L = 0x21;
  CYC(0x4cd3, 0x4cd5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4cd5, 0x4cd6); ret_effect(gb); return; }
  CYC(0x4cd5, 0x4cd6);
  CYC(0x4cd6, 0x4cd8); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4cd8, 0x4cda); L = 0x06;
  CYC(0x4cda, 0x4cdb); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x4cdb, 0x4cdc); ret_effect(gb); return; }
  CYC(0x4cdb, 0x4cdc);
  CYC(0x4cdc, 0x4cde); A = 0x05;
  CYC(0x4cde, 0x4ce1); W8(w1WeaponItem_state) = A;
  CYC(0x4ce1, 0x4ce4); goto delete_self;

state5:
  CALL_C(0x4ce4, specialObjectAnimate_optimized_hook, 0x441e, 0x4ce7);
  CYC(0x4ce7, 0x4ce8); H = D;
  CYC(0x4ce8, 0x4cea); L = 0x21;
  CYC(0x4cea, 0x4cec); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4cec, 0x4ced); ret_effect(gb); return; }
  CYC(0x4cec, 0x4ced);
  CYC(0x4ced, 0x4cef); L = 0x02;
  CYC(0x4cef, 0x4cf0); A = mem_rd(gb, HL);
  CYC(0x4cf0, 0x4cf1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4cf1, 0x4cf3); goto delete_self; }
  CYC(0x4cf1, 0x4cf3);
  CYC(0x4cf3, 0x4cf5); A = 0x06;
  CYC(0x4cf5, 0x4cf8); W8(w1WeaponItem_state) = A;
  CYC(0x4cf8, 0x4cfa); L = 0x04;
  CYC(0x4cfa, 0x4cfb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4cfb, 0x4cfc); alu_xor(gb, A);
  CYC(0x4cfc, 0x4cff); W8(w1WeaponItem_var2a) = A;
  CYC(0x4cff, 0x4d00); ret_effect(gb); return;

delete_self:
  CYC(0x4d00, 0x4d01); alu_xor(gb, A);
  CYC(0x4d01, 0x4d04); W8(wcc63) = A;
  CYC(0x4d04, 0x4d07); clearParentItem_hook(gb); return;

trigger_poke:
  CYC(0x4d23, 0x4d24); H = D;
  CYC(0x4d24, 0x4d26); L = 0x3f;
  CYC(0x4d26, 0x4d28); mem_wr(gb, HL, 0x08);
  CYC(0x4d28, 0x4d2a); L = 0x04;
  CYC(0x4d2a, 0x4d2c); mem_wr(gb, HL, 0x05);
  CALL_C(0x4d2c, itemDisableLinkMovement_hook, 0x545d, 0x4d2f);
  CALL_C(0x4d2f, isLinkUnderwater_hook, 0x54d2, 0x4d32);
  CYC(0x4d32, 0x4d34); A = 0x1f;
  if (F & FZ) CYCT(0x4d34, 0x4d36);
  else { CYC(0x4d34, 0x4d36); CYC(0x4d36, 0x4d38); A = 0x2c; }
  CYC(0x4d38, 0x4d3b); specialObjectSetAnimationWithLinkData_hook(gb); return;

check_create_beam:
  CYC(0x4d3b, 0x4d3d); C = 0x08;
  CYC(0x4d3d, 0x4d3f); A = 0x17;
  CALL_C(0x4d3f, cpActiveRing_hook, 0x23b0, 0x4d42);
  if (F & FZ) CYCT(0x4d42, 0x4d44);
  else {
    CYC(0x4d42, 0x4d44);
    CYC(0x4d44, 0x4d46); C = 0x0c;
    CYC(0x4d46, 0x4d48); A = 0x18;
    CALL_C(0x4d48, cpActiveRing_hook, 0x23b0, 0x4d4b);
    if (F & FZ) CYCT(0x4d4b, 0x4d4d);
    else { CYC(0x4d4b, 0x4d4d); CYC(0x4d4d, 0x4d4f); C = 0x00; }
  }
  CYC(0x4d4f, 0x4d52); SET_HL(wLinkHealth);
  CYC(0x4d52, 0x4d53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4d53, 0x4d54); alu_add(gb, C);
  CYC(0x4d54, 0x4d55); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x4d55, 0x4d56); ret_effect(gb); return; }
  CYC(0x4d55, 0x4d56);
  sword_parent_create_beam(gb, sp0_);
}
