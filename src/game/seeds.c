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
