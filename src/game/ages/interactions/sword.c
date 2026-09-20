#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static void sword_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void sword_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t sword_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void itemCode05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e8f, itemTransferKnockbackToLink_hook, 0x4a22, 0x5e92);
  CYC(0x5e92, 0x5e94); E = 0x04;
  CYC(0x5e94, 0x5e95); A = mem_rd(gb, DE);
  CYC(0x5e95, 0x5e96); push_effect(gb, 0x5e96);
  switch (sword_jump_table(gb)) {
    case 0x5eac:
      CYC(0x5eac, 0x5eae); A = 0x1a;
      CALL_C(0x5eae, loadWeaponGfx_b00_hook, 0x166d, 0x5eb1);
      CALL_C(0x5eb1, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5eb4);
      CYC(0x5eb4, 0x5eb6); alu_and(gb, 0x07);
      CYC(0x5eb6, 0x5eb9); SET_HL(0x5ea4);
      CYC(0x5eb9, 0x5eba); sword_add_a_to_hl(gb, 0x5eba);
      CYC(0x5eba, 0x5ebb); A = mem_rd(gb, HL);
      CALL_C(0x5ebb, playSound_b00_hook, 0x0c98, 0x5ebe);
      CYC(0x5ebe, 0x5ec0); E = 0x31;
      CYC(0x5ec0, 0x5ec1); alu_xor(gb, A);
      CYC(0x5ec1, 0x5ec2); mem_wr(gb, DE, A);
    case 0x5ec2:
      CALL_C(0x5ec2, loadAttributesAndGraphicsAndIncState_hook, 0x498c, 0x5ec5);
      CYC(0x5ec5, 0x5ec8); A = W8(wSwordLevel);
      CYC(0x5ec8, 0x5ecb); SET_HL(0x5ef9);
      CYC(0x5ecb, 0x5ecc); sword_add_double_index(gb, 0x5ecc);
      CYC(0x5ecc, 0x5ece); E = 0x24;
      CYC(0x5ece, 0x5ecf); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x5ecf, 0x5ed0); mem_wr(gb, DE, A);
      CYC(0x5ed0, 0x5ed1); C = mem_rd(gb, HL);
      CYC(0x5ed1, 0x5ed3); E = 0x31;
      CYC(0x5ed3, 0x5ed4); A = mem_rd(gb, DE);
      CYC(0x5ed4, 0x5ed5); alu_or(gb, A);
      CYC(0x5ed5, 0x5ed6); A = C;
      CYC(0x5ed6, 0x5ed7); mem_wr(gb, DE, A);
      if (!(F & FZ)) {
        CYCT(0x5ed7, 0x5ed9);
        goto set_damage;
      }
      CYC(0x5ed7, 0x5ed9);
      CYC(0x5ed9, 0x5edb); A = 0x3e;
      CALL_C(0x5edb, cpActiveRing_hook, 0x23b0, 0x5ede);
      if (!(F & FZ)) {
        CYCT(0x5ede, 0x5ee0);
        goto set_damage;
      }
      CYC(0x5ede, 0x5ee0);
      CALL_C(0x5ee0, getRandomNumber_hook, 0x043e, 0x5ee3);
      CYC(0x5ee3, 0x5ee4); alu_or(gb, A);
      CYC(0x5ee4, 0x5ee6); C = 0xff;
      if (!(F & FZ)) {
        CYCT(0x5ee6, 0x5ee8);
        goto set_damage;
      }
      CYC(0x5ee6, 0x5ee8);
      CYC(0x5ee8, 0x5eea); A = 0xd2;
      CALL_C(0x5eea, playSound_b00_hook, 0x0c98, 0x5eed);
      CYC(0x5eed, 0x5eef); C = 0xf4;
set_damage:
      CYC(0x5eef, 0x5ef1); E = 0x3a;
      CYC(0x5ef1, 0x5ef2); A = C;
      CYC(0x5ef2, 0x5ef3); mem_wr(gb, DE, A);
      CYC(0x5ef3, 0x5ef5); E = 0x04;
      CYC(0x5ef5, 0x5ef7); A = 0x01;
      CYC(0x5ef7, 0x5ef8); mem_wr(gb, DE, A);
      CYC(0x5ef8, 0x5efb); objectSetVisible82_hook(gb); return;
    case 0x5f01:
      CYC(0x5f01, 0x5f03); E = 0x24;
      CYC(0x5f03, 0x5f05); A = 0x88;
      CYC(0x5f05, 0x5f06); mem_wr(gb, DE, A);
    case 0x5f06:
      CYC(0x5f06, 0x5f07); H = D;
      CYC(0x5f07, 0x5f09); L = 0x1b;
      CYC(0x5f09, 0x5f0a); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x5f0a, 0x5f0b); mem_wr(gb, HL, A);
      CYC(0x5f0b, 0x5f0c); ret_effect(gb); return;
    case 0x5f0c:
      CYC(0x5f0c, 0x5f0e); E = 0x31;
      CYC(0x5f0e, 0x5f0f); A = mem_rd(gb, DE);
      CYC(0x5f0f, 0x5f11); E = 0x3a;
      CYC(0x5f11, 0x5f12); mem_wr(gb, DE, A);
      CYC(0x5f12, 0x5f13); ret_effect(gb); return;
    case 0x5f13:
      CYC(0x5f13, 0x5f14); H = D;
      CYC(0x5f14, 0x5f16); L = 0x06;
      CYC(0x5f16, 0x5f17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(0x5f17, 0x5f19); alu_bit(gb, 2, mem_rd(gb, HL));
      CYC(0x5f19, 0x5f1b); L = 0x1b;
      CYC(0x5f1b, 0x5f1c); A = mem_rd(gb, HL); SET_HL(HL + 1);
      if (!(F & FZ)) CYCT(0x5f1c, 0x5f1e);
      else {
        CYC(0x5f1c, 0x5f1e);
        CYC(0x5f1e, 0x5f20); A = 0x0d;
      }
      CYC(0x5f20, 0x5f21); mem_wr(gb, HL, A);
      CYC(0x5f21, 0x5f22); ret_effect(gb); return;
    case 0x5f22:
      CYC(0x5f22, 0x5f24); A = 0x08;
      CALL_C(0x5f24, tryBreakTileWithSword_calculateLevel_hook, 0x6193, 0x5f27);
      CYC(0x5f27, 0x5f2a); itemDelete_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t bank09_jump_table_sword(GB *gb) {
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

void interactionCode5e__afterCall6e68_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e68, objectSetInvisible_hook, 0x1e7b, 0x6e6b);
  CYC(0x6e6b, 0x6e6d); A = 0x00;
  CALL_C(0x6e6d, objectGetRelatedObject1Var_hook, 0x2160, 0x6e70);
  CYC(0x6e70, 0x6e72); L = 0x7f;
  CYC(0x6e72, 0x6e73); A = mem_rd(gb, HL);
  CYC(0x6e73, 0x6e74); A = alu_inc8(gb, A);
  CYC(0x6e74, 0x6e76); L = 0x40;
  CYC(0x6e76, 0x6e77); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6e77, 0x6e7a); interactionDelete_hook(gb); return;
  }
  CYC(0x6e77, 0x6e7a);
  CYC(0x6e7a, 0x6e7c); L = 0x61;
  CYC(0x6e7c, 0x6e7d); A = mem_rd(gb, HL);
  CYC(0x6e7d, 0x6e7e); B = A;
  CYC(0x6e7e, 0x6e80); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6e80, 0x6e81); ret_effect(gb); return;
  }
  CYC(0x6e80, 0x6e81);
  CYC(0x6e81, 0x6e82); A = B;
  CYC(0x6e82, 0x6e84); alu_and(gb, 0x7f);
  PUSH(0x6e84, HL);
  CYC(0x6e85, 0x6e86); H = D;
  CYC(0x6e86, 0x6e88); L = 0x77;
  CYC(0x6e88, 0x6e89); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6e89, 0x6e8b); goto L_6e8f;
  }
  CYC(0x6e89, 0x6e8b);
  CYC(0x6e8b, 0x6e8c); mem_wr(gb, HL, A);
  CALL_C(0x6e8c, interactionSetAnimation_hook, 0x262e, 0x6e8f);
L_6e8f:
  SET_HL(POP(0x6e8f));
  CALL_C(0x6e90, objectTakePosition_hook, 0x2274, 0x6e93);
  CYC(0x6e93, 0x6e96); objectSetVisible83_hook(gb); return;
}

void interactionCode5e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e55, 0x6e57); E = 0x44;
  CYC(0x6e57, 0x6e58); A = mem_rd(gb, DE);
  CYC(0x6e58, 0x6e59); push_effect(gb, 0x6e59);
  SET_HL(bank09_jump_table_sword(gb));
  switch (HL) {
    case 0x6e5d: goto L_6e5d;
    case 0x6e68: goto L_6e68;
    default: HANDOFF(HL);
  }
L_6e5d:
  CYC(0x6e5d, 0x6e5f); A = 0x01;
  CYC(0x6e5f, 0x6e60); mem_wr(gb, DE, A);
  CYC(0x6e60, 0x6e62); A = 0xff;
  CYC(0x6e62, 0x6e64); E = 0x77;
  CYC(0x6e64, 0x6e65); mem_wr(gb, DE, A);
  CALL_C(0x6e65, interactionInitGraphics_hook, 0x15fb, 0x6e68);
  interactionCode5e__afterCall6e68_hook(gb);
  return;
L_6e68:
  interactionCode5e__afterCall6e68_hook(gb);
}
