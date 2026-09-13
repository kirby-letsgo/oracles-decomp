#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void braceletCheckBreakable_hook(GB *gb);
void braceletCheckDeleteSelfWhileThrowing_hook(GB *gb);
void itemBeginThrow_hook(GB *gb);
void itemUpdateThrowingLaterally_hook(GB *gb);

static uint16_t bracelet_jump_table(GB *gb) {
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

void itemCode16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x62c6, 0x62c8); E = 0x04;
  CYC(0x62c8, 0x62c9); A = mem_rd(gb, DE);
  CYC(0x62c9, 0x62ca); push_effect(gb, 0x62ca);
  switch (bracelet_jump_table(gb)) {
    case 0x62d2: goto state0;
    case 0x62ea: goto held;
    case 0x632c: goto thrown;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x62d2, itemLoadAttributesAndGraphics_hook, 0x4993, 0x62d5);
  CYC(0x62d5, 0x62d6); H = D;
  CYC(0x62d6, 0x62d8); L = 0x00;
  CYC(0x62d8, 0x62da); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
  CYC(0x62da, 0x62dc); L = 0x02;
  CYC(0x62dc, 0x62dd); A = mem_rd(gb, HL);
  CYC(0x62dd, 0x62de); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x62de, 0x62e0);
    goto not_tile;
  }
  CYC(0x62de, 0x62e0);
  CYC(0x62e0, 0x62e2); L = 0x04;
  CYC(0x62e2, 0x62e4); mem_wr(gb, HL, 0x02);
  CALL_C(0x62e4, itemMimicBgTile_hook, 0x628e, 0x62e7);
  CYC(0x62e7, 0x62ea);
  objectSetVisiblec0_hook(gb);
  return;

held:
  CYC(0x62ea, 0x62eb); H = D;
  CYC(0x62eb, 0x62ed); L = 0x05;
  CYC(0x62ed, 0x62ee); A = mem_rd(gb, HL);
  CYC(0x62ee, 0x62ef); alu_or(gb, A);
  if (F & FZ) { CYCT(0x62ef, 0x62f0); ret_effect(gb); return; }
  CYC(0x62ef, 0x62f0);
  CYC(0x62f0, 0x62f2); L = 0x27;
  CYC(0x62f2, 0x62f4); A = 0x06;
  CYC(0x62f4, 0x62f5); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x62f5, 0x62f6); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x62f6, 0x62f7); L = alu_dec8(gb, L);
  CYC(0x62f7, 0x62f9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x62f9, 0x62fb);
  goto throw_item;

not_tile:
  CALL_C(0x62fb, braceletCheckDeleteSelfWhileThrowing_hook, 0x6374, 0x62fe);
  CYC(0x62fe, 0x62ff); A = H;
  CYC(0x62ff, 0x6301); alu_cp(gb, 0xd1);
  if (F & FZ) {
    CYCT(0x6301, 0x6303);
    goto copy_collisions;
  }
  CYC(0x6301, 0x6303);
  CYC(0x6303, 0x6304); A = L;
  CYC(0x6304, 0x6306); alu_cp(gb, 0x40);
  if (F & FC) {
    CYCT(0x6306, 0x6308);
    goto throw_item;
  }
  CYC(0x6306, 0x6308);

copy_collisions:
  CYC(0x6308, 0x630a); A = 0x09;
  CALL_C(0x630a, objectGetRelatedObject2Var_hook, 0x2164, 0x630d);
  CYC(0x630d, 0x630f); E = 0x09;
  CYC(0x630f, 0x6310); A = mem_rd(gb, DE);
  CYC(0x6310, 0x6311); mem_wr(gb, HL, A);
  CYC(0x6311, 0x6312); A = L;
  CYC(0x6312, 0x6314); alu_add(gb, 0x1d);
  CYC(0x6314, 0x6315); L = A;
  CYC(0x6315, 0x6317); E = 0x26;
  CYC(0x6317, 0x6318); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6318, 0x6319); mem_wr(gb, DE, A);
  CYC(0x6319, 0x631a); E = alu_inc8(gb, E);
  CYC(0x631a, 0x631b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x631b, 0x631c); mem_wr(gb, DE, A);
  CYC(0x631c, 0x631d); H = D;
  CYC(0x631d, 0x631f); L = 0x24;
  CYC(0x631f, 0x6321); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

throw_item:
  CALL_C(0x6321, itemBeginThrow_hook, 0x63b1, 0x6324);
  CYC(0x6324, 0x6325); H = D;
  CYC(0x6325, 0x6327); L = 0x04;
  CYC(0x6327, 0x6329); mem_wr(gb, HL, 0x03);
  CYC(0x6329, 0x632a); L = alu_inc8(gb, L);
  CYC(0x632a, 0x632c); mem_wr(gb, HL, 0x00);

thrown:
  CALL_C(0x632c, braceletCheckDeleteSelfWhileThrowing_hook, 0x6374, 0x632f);
  CALL_C(0x632f, itemUpdateThrowingLaterally_hook, 0x6407, 0x6332);
  if (F & FZ) {
    CYCT(0x6332, 0x6334);
    goto destroy;
  }
  CYC(0x6332, 0x6334);
  CYC(0x6334, 0x6336); E = 0x39;
  CYC(0x6336, 0x6337); A = mem_rd(gb, DE);
  CYC(0x6337, 0x6338); C = A;
  CALL_C(0x6338, itemUpdateThrowingVertically_hook, 0x4aa5, 0x633b);
  if (!(F & FC)) {
    CYCT(0x633b, 0x633d);
    goto no_collision;
  }
  CYC(0x633b, 0x633d);
  CALL_C(0x633d, braceletCheckBreakable_hook, 0x636d, 0x6340);
  if (!(F & FZ)) {
    CYCT(0x6340, 0x6342);
    goto destroy;
  }
  CYC(0x6340, 0x6342);
  CALL_C(0x6342, itemBounce_hook, 0x6482, 0x6345);
  if (F & FC) {
    CYCT(0x6345, 0x6347);
    goto release;
  }
  CYC(0x6345, 0x6347);

no_collision:
  CYC(0x6347, 0x6349); E = 0x02;
  CYC(0x6349, 0x634a); A = mem_rd(gb, DE);
  CYC(0x634a, 0x634b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x634b, 0x634c); ret_effect(gb); return; }
  CYC(0x634b, 0x634c);
  CYC(0x634c, 0x634e); A = 0x0b;
  CALL_C(0x634e, objectGetRelatedObject2Var_hook, 0x2164, 0x6351);
  CYC(0x6351, 0x6354);
  objectCopyPosition_hook(gb);
  return;

release:
  CYC(0x6354, 0x6356); A = 0x05;
  CALL_C(0x6356, objectGetRelatedObject2Var_hook, 0x2164, 0x6359);
  CYC(0x6359, 0x635b); mem_wr(gb, HL, 0x03);
  CYC(0x635b, 0x635e);
  itemDelete_hook(gb);
  return;

destroy:
  CALL_C(0x635e, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x6361);
  if (F & FC) { CYCT(0x6361, 0x6362); ret_effect(gb); return; }
  CYC(0x6361, 0x6362);
  CYC(0x6362, 0x6365); SET_HL(0x4809);
  CYC(0x6365, 0x6367); E = 0x06;
  CALL_C(0x6367, interBankCall_hook, 0x008a, 0x636a);
  CYC(0x636a, 0x636d);
  itemDelete_hook(gb);
}

void braceletCheckBreakable_hook(GB *gb) {
  CYC(0x636d, 0x636f); E = 0x02;
  CYC(0x636f, 0x6370); A = mem_rd(gb, DE);
  CYC(0x6370, 0x6371); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6371, 0x6372); ret_effect(gb); return; }
  CYC(0x6371, 0x6372);
  CYC(0x6372, 0x6373); alu_scf(gb);
  CYC(0x6373, 0x6374); ret_effect(gb);
}

void braceletCheckDeleteSelfWhileThrowing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6374, 0x6376); E = 0x02;
  CYC(0x6376, 0x6377); A = mem_rd(gb, DE);
  CYC(0x6377, 0x6378); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6378, 0x637a);
    goto throwing_tile;
  }
  CYC(0x6378, 0x637a);
  CYC(0x637a, 0x637b); alu_xor(gb, A);
  CALL_C(0x637b, objectGetRelatedObject2Var_hook, 0x2164, 0x637e);
  CYC(0x637e, 0x6380); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6380, 0x6382);
    goto delete_self;
  }
  CYC(0x6380, 0x6382);
  CYC(0x6382, 0x6383); A = L;
  CYC(0x6383, 0x6385); alu_add(gb, 0x04);
  CYC(0x6385, 0x6386); L = A;
  CYC(0x6386, 0x6387); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6387, 0x6389); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x6389, 0x638b);
    goto delete_self;
  }
  CYC(0x6389, 0x638b);
  CYC(0x638b, 0x638c); A = mem_rd(gb, HL);
  CYC(0x638c, 0x638e); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x638e, 0x638f); ret_effect(gb); return; }
  CYC(0x638e, 0x638f);

delete_self:
  CYC(0x638f, 0x6390); SET_AF(pop_effect(gb));
  CYC(0x6390, 0x6393);
  itemDelete_hook(gb);
  return;

throwing_tile:
  CALL_C(0x6393, objectCheckWithinRoomBoundary_hook, 0x219f, 0x6396);
  if (!(F & FC)) {
    CYCT(0x6396, 0x6398);
    goto delete_self;
  }
  CYC(0x6396, 0x6398);
  CYC(0x6398, 0x6399); H = D;
  CYC(0x6399, 0x639b); L = 0x05;
  CYC(0x639b, 0x639c); ret_effect(gb);
}
