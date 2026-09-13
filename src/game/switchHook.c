#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void func_5902_hook(GB *gb);
void switchHookState3_hook(GB *gb);
void checkRelatedObject2States_hook(GB *gb);
void updateSwitchHookSound_hook(GB *gb);
void checkCanPlaceDiamondOnTile_hook(GB *gb);

static uint16_t switch_hook_jump_table(GB *gb) {
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

static void switch_hook_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void switch_hook_set_position_component(GB *gb) {
  CYC(0x57bf, 0x57c0); A = mem_rd(gb, HL);
  CYC(0x57c0, 0x57c2); alu_cp(gb, 0xf8);
  if (F & FC) {
    CYCT(0x57c2, 0x57c4);
  } else {
    CYC(0x57c2, 0x57c4);
    CYC(0x57c4, 0x57c5); alu_xor(gb, A);
  }
  CYC(0x57c5, 0x57c7); H = 0xd0;
  CYC(0x57c7, 0x57c8); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x57c8, 0x57c9); C = A;
  CYC(0x57c9, 0x57cb); A = 0x00;
  CYC(0x57cb, 0x57cc); alu_sbc(gb, A);
  CYC(0x57cc, 0x57cd); alu_rra(gb);
  CYC(0x57cd, 0x57cf); C = alu_rr(gb, C);
  CYC(0x57cf, 0x57d0); alu_rra(gb);
  CYC(0x57d0, 0x57d2); C = alu_rr(gb, C);
  CYC(0x57d2, 0x57d3); alu_xor(gb, A);
  for (;;) {
    CYC(0x57d3, 0x57d4); alu_add(gb, C);
    CYC(0x57d4, 0x57d5); E = alu_dec8(gb, E);
    if (!(F & FZ)) {
      CYCT(0x57d5, 0x57d7);
    } else {
      CYC(0x57d5, 0x57d7);
      break;
    }
  }
  CYC(0x57d7, 0x57d8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x57d8, 0x57d9); alu_add(gb, B);
  CYC(0x57d9, 0x57da); H = D;
  CYC(0x57da, 0x57db); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x57db, 0x57dc); ret_effect(gb);
}

void itemCode0bPost_hook(GB *gb) {
  CYC(0x5791, 0x5794); A = W8(w1WeaponItem_id);
  CYC(0x5794, 0x5796); alu_cp(gb, 0x0a);
  if (!(F & FZ)) {
    CYCT(0x5796, 0x5799);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x5796, 0x5799);
  CYC(0x5799, 0x579c); A = W8(w1WeaponItem_var2f);
  CYC(0x579c, 0x579e); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(0x579e, 0x57a1);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x579e, 0x57a1);
  CYC(0x57a1, 0x57a2); H = D;
  CYC(0x57a2, 0x57a5); A = W8(w1WeaponItem_zh);
  CYC(0x57a5, 0x57a7); L = 0x0f;
  CYC(0x57a7, 0x57a8); mem_wr(gb, HL, A);
  CYC(0x57a8, 0x57aa); L = 0x06;
  CYC(0x57aa, 0x57ab); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x57ab, 0x57ad);
  } else {
    CYC(0x57ab, 0x57ad);
    CYC(0x57ad, 0x57af); mem_wr(gb, HL, 0x03);
  }
  CYC(0x57af, 0x57b0); E = mem_rd(gb, HL);
  CYC(0x57b0, 0x57b1); push_effect(gb, DE);
  CYC(0x57b1, 0x57b3); B = 0x03;
  CYC(0x57b3, 0x57b6); SET_HL(w1WeaponItem_yh);
  CYC(0x57b6, 0x57b9); push_effect(gb, 0x57b9);
  switch_hook_set_position_component(gb);
  CYC(0x57b9, 0x57ba); SET_DE(pop_effect(gb));
  CYC(0x57ba, 0x57bc); B = 0x00;
  CYC(0x57bc, 0x57bf); SET_HL(w1WeaponItem_xh);
  switch_hook_set_position_component(gb);
}

void itemCode0aPost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57f2, cpRelatedObject1ID_hook, 0x4b86, 0x57f5);
  if (F & FZ) {
    CYCT(0x57f5, 0x57f6); ret_effect(gb); return;
  }
  CYC(0x57f5, 0x57f6);
  CYC(0x57f6, 0x57f9); A = W8(wSwitchHookState);
  CYC(0x57f9, 0x57fa); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x57fa, 0x57fd);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x57fa, 0x57fd);
  CYC(0x57fd, 0x5800);
  func_5902_hook(gb);
}

void itemCode0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57dc, 0x57de); E = 0x04;
  CYC(0x57de, 0x57df); A = mem_rd(gb, DE);
  CYC(0x57df, 0x57e0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x57e0, 0x57e1); ret_effect(gb); return; }
  CYC(0x57e0, 0x57e1);
  CALL_C(0x57e1, itemLoadAttributesAndGraphics_hook, 0x4993, 0x57e4);
  CALL_C(0x57e4, itemIncState_hook, 0x23ea, 0x57e7);
  CYC(0x57e7, 0x57e9); L = 0x06;
  CYC(0x57e9, 0x57eb); mem_wr(gb, HL, 0x03);
  CYC(0x57eb, 0x57ec); alu_xor(gb, A);
  CALL_C(0x57ec, itemSetAnimation_hook, 0x49e2, 0x57ef);
  CYC(0x57ef, 0x57f2);
  objectSetVisible83_hook(gb);
}

static void switch_hook_start_retracting(GB *gb) {
  CYC(0x58c2, 0x58c3); H = D;
  CYC(0x58c3, 0x58c5); L = 0x24;
  CYC(0x58c5, 0x58c7); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(0x58c7, 0x58c9); A = 0x02;
  CYC(0x58c9, 0x58cc);
  itemSetState_hook(gb);
}

static void switch_hook_cancel_and_retract(GB *gb) {
  CYC(0x58bb, 0x58bc); alu_xor(gb, A);
  CYC(0x58bc, 0x58bf); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x58bf, 0x58c2); W8(wSwitchHookState) = A;
  switch_hook_start_retracting(gb);
}

void itemCode0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5800, 0x5802); A = 0x08;
  CYC(0x5802, 0x5805); W8(wDisableRingTransformations) = A;
  CYC(0x5805, 0x5807); A = 0x80;
  CYC(0x5807, 0x580a); W8(wcc92) = A;
  CYC(0x580a, 0x580c); E = 0x04;
  CYC(0x580c, 0x580d); A = mem_rd(gb, DE);
  CYC(0x580d, 0x580e); push_effect(gb, 0x580e);
  switch (switch_hook_jump_table(gb)) {
    case 0x5816: goto state0;
    case 0x5853: goto state1;
    case 0x58cc: goto state2;
    case 0x591a: switchHookState3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x5816, 0x5818); A = 0x1f;
  CALL_C(0x5818, loadWeaponGfx_b00_hook, 0x166d, 0x581b);
  CYC(0x581b, 0x581e); SET_HL(0x5847);
  CALL_C(0x581e, applyOffsetTableHL_hook, 0x4a36, 0x5821);
  CALL_C(0x5821, objectSetVisible82_hook, 0x1e69, 0x5824);
  CALL_C(0x5824, loadAttributesAndGraphicsAndIncState_hook, 0x498c, 0x5827);
  CYC(0x5827, 0x582a); SET_BC(0x5029);
  CYC(0x582a, 0x582d); A = W8(wSwitchHookLevel);
  CYC(0x582d, 0x582e); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x582e, 0x5830);
  } else {
    CYC(0x582e, 0x5830);
    CYC(0x5830, 0x5833); SET_BC(0x7826);
  }
  CYC(0x5833, 0x5834); H = D;
  CYC(0x5834, 0x5836); L = 0x10;
  CYC(0x5836, 0x5837); mem_wr(gb, HL, B);
  CYC(0x5837, 0x5839); L = 0x06;
  CYC(0x5839, 0x583a); mem_wr(gb, HL, C);
  CYC(0x583a, 0x583c); L = 0x2f;
  CYC(0x583c, 0x583e); mem_wr(gb, HL, 0x01);
  CALL_C(0x583e, itemUpdateAngle_hook, 0x2cf0, 0x5841);
  CYC(0x5841, 0x5842); A = mem_rd(gb, HL);
  CYC(0x5842, 0x5844); alu_add(gb, 0x02);
  CYC(0x5844, 0x5847);
  itemSetAnimation_hook(gb);
  return;

state1:
  CYC(0x5853, 0x5855); E = 0x2a;
  CYC(0x5855, 0x5856); A = mem_rd(gb, DE);
  CYC(0x5856, 0x5857); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5857, 0x5859);
  } else {
    CYC(0x5857, 0x5859);
    CYC(0x5859, 0x585b); alu_bit(gb, 5, A);
    if (!(F & FZ)) {
      CYCT(0x585b, 0x585d);
      goto go_to_state3;
    }
    CYC(0x585b, 0x585d);
    CYC(0x585d, 0x585f);
    switch_hook_start_retracting(gb);
    return;
  }
  CYC(0x585f, 0x5860); H = D;
  CYC(0x5860, 0x5862); L = 0x2f;
  CYC(0x5862, 0x5864); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5864, 0x5867); itemDelete_hook(gb); return; }
  CYC(0x5864, 0x5867);
  CALL_C(0x5867, itemDecCounter1_hook, 0x23d6, 0x586a);
  if (F & FZ) { CYCT(0x586a, 0x586c); switch_hook_start_retracting(gb); return; }
  CYC(0x586a, 0x586c);
  CALL_C(0x586c, objectCheckWithinRoomBoundary_hook, 0x219f, 0x586f);
  if (!(F & FC)) { CYCT(0x586f, 0x5871); switch_hook_start_retracting(gb); return; }
  CYC(0x586f, 0x5871);
  CALL_C(0x5871, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x5874);
  if (!(F & FC)) {
    CYCT(0x5874, 0x5876);
    goto no_collision;
  }
  CYC(0x5874, 0x5876);
  CALL_C(0x5876, itemCheckCanPassSolidTile_hook, 0x4b95, 0x5879);
  if (!(F & FZ)) { CYCT(0x5879, 0x587b); goto collision; }
  CYC(0x5879, 0x587b);

no_collision:
  CYC(0x587b, 0x587d); E = 0x2f;
  CYC(0x587d, 0x587e); A = mem_rd(gb, DE);
  CYC(0x587e, 0x5880); alu_bit(gb, 3, A);
  if (!(F & FZ)) {
    CYCT(0x5880, 0x5882);
  } else {
    CYC(0x5880, 0x5882);
    CALL_C(0x5882, getFreeItemSlot_hook, 0x2cf9, 0x5885);
    if (!(F & FZ)) {
      CYCT(0x5885, 0x5887);
    } else {
      CYC(0x5885, 0x5887);
      CYC(0x5887, 0x5888); A = alu_inc8(gb, A);
      CYC(0x5888, 0x5889); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x5889, 0x588b); mem_wr(gb, HL, 0x0b);
      CYC(0x588b, 0x588c); H = D;
      CYC(0x588c, 0x588e); L = 0x2f;
      CYC(0x588e, 0x5890); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
    }
  }
  CALL_C(0x5890, updateSwitchHookSound_hook, 0x5aaa, 0x5893);
  CYC(0x5893, 0x5896);
  objectApplySpeed_hook(gb);
  return;

collision:
  CALL_C(0x5896, objectCreateClinkInteraction_hook, 0x4b81, 0x5899);
  CYC(0x5899, 0x589b); A = 0x88;
  CALL_C(0x589b, itemTryToBreakTile_hook, 0x2bef, 0x589e);
  if (!(F & FC)) { CYCT(0x589e, 0x58a0); switch_hook_start_retracting(gb); return; }
  CYC(0x589e, 0x58a0);
  CYC(0x58a0, 0x58a2); E = 0x02;
  CYC(0x58a2, 0x58a4); A = 0x01;
  CYC(0x58a4, 0x58a5); mem_wr(gb, DE, A);

go_to_state3:
  CYC(0x58a5, 0x58a7); A = 0x03;
  CALL_C(0x58a7, itemSetState_hook, 0x2a7e, 0x58aa);
  CYC(0x58aa, 0x58ac); L = 0x24;
  CYC(0x58ac, 0x58ae); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(0x58ae, 0x58b0); A = 0xff;
  CYC(0x58b0, 0x58b3); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x58b3, 0x58b5); A = 0x01;
  CYC(0x58b5, 0x58b8); W8(wSwitchHookState) = A;
  CYC(0x58b8, 0x58bb);
  resetLinkInvincibility_hook(gb);
  return;

state2:
  CYC(0x58cc, 0x58ce); E = 0x05;
  CYC(0x58ce, 0x58cf); A = mem_rd(gb, DE);
  CYC(0x58cf, 0x58d0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x58d0, 0x58d2);
    CYC(0x58f5, 0x58f8); SET_HL(w1Link_yh);
    CALL_C(0x58f8, objectTakePosition_hook, 0x2274, 0x58fb);
    CALL_C(0x58fb, itemDecCounter1_hook, 0x23d6, 0x58fe);
    if (!(F & FZ)) { CYCT(0x58fe, 0x58ff); ret_effect(gb); return; }
    CYC(0x58fe, 0x58ff);
    CYC(0x58ff, 0x5902);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x58d0, 0x58d2);
  CALL_C(0x58d2, itemDecCounter1_hook, 0x23d6, 0x58d5);
  CALL_C(0x58d5, updateSwitchHookSound_hook, 0x5aaa, 0x58d8);
  CALL_C(0x58d8, objectGetAngleTowardLink_hook, 0x1e9c, 0x58db);
  CYC(0x58db, 0x58dd); E = 0x09;
  CYC(0x58dd, 0x58de); mem_wr(gb, DE, A);
  CALL_C(0x58de, objectApplySpeed_hook, 0x201d, 0x58e1);
  CYC(0x58e1, 0x58e4); SET_BC(0x1008);
  CALL_C(0x58e4, itemCheckWithinRangeOfLink_hook, 0x577e, 0x58e7);
  if (!(F & FC)) { CYCT(0x58e7, 0x58e8); ret_effect(gb); return; }
  CYC(0x58e7, 0x58e8);
  CALL_C(0x58e8, itemIncSubstate_hook, 0x23ef, 0x58eb);
  CYC(0x58eb, 0x58ec); L = alu_inc8(gb, L);
  CYC(0x58ec, 0x58ee); mem_wr(gb, HL, 0x03);
  CYC(0x58ee, 0x58f0); L = 0x2f;
  CYC(0x58f0, 0x58f2); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x58f2, 0x58f5);
  objectSetInvisible_hook(gb);
}

void func_5902_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5902, checkRelatedObject2States_hook, 0x5a94, 0x5905);
  if (!(F & FC)) {
    CYCT(0x5905, 0x5907);
  } else {
    CYC(0x5905, 0x5907);
    if (F & FZ) {
      CYCT(0x5907, 0x5909);
    } else {
      CYC(0x5907, 0x5909);
      CYC(0x5909, 0x590b); A = 0x05;
      CALL_C(0x590b, objectGetRelatedObject2Var_hook, 0x2164, 0x590e);
      CYC(0x590e, 0x5910); mem_wr(gb, HL, 0x03);
    }
  }
  CYC(0x5910, 0x5911); alu_xor(gb, A);
  CYC(0x5911, 0x5914); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x5914, 0x5917); W8(wSwitchHookState) = A;
  CYC(0x5917, 0x591a);
  itemDelete_hook(gb);
}

void checkRelatedObject2States_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5a94, 0x5a96); E = 0x02;
  CYC(0x5a96, 0x5a97); A = mem_rd(gb, DE);
  CYC(0x5a97, 0x5a98); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x5a98, 0x5a9a);
  } else {
    CYC(0x5a98, 0x5a9a);
    CYC(0x5a9a, 0x5a9c); A = 0x04;
    CALL_C(0x5a9c, objectGetRelatedObject2Var_hook, 0x2164, 0x5a9f);
    CYC(0x5a9f, 0x5aa0); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5aa0, 0x5aa2); alu_cp(gb, 0x03);
    if (!(F & FZ)) { CYCT(0x5aa2, 0x5aa3); ret_effect(gb); return; }
    CYC(0x5aa2, 0x5aa3);
    CYC(0x5aa3, 0x5aa4); A = mem_rd(gb, HL);
    CYC(0x5aa4, 0x5aa6); alu_cp(gb, 0x03);
    if (!(F & FC)) { CYCT(0x5aa6, 0x5aa7); ret_effect(gb); return; }
    CYC(0x5aa6, 0x5aa7);
    CYC(0x5aa7, 0x5aa8); alu_or(gb, D);
  }
  CYC(0x5aa8, 0x5aa9); alu_scf(gb);
  CYC(0x5aa9, 0x5aaa); ret_effect(gb);
}

void updateSwitchHookSound_hook(GB *gb) {
  CYC(0x5aaa, 0x5aac); E = 0x06;
  CYC(0x5aac, 0x5aad); A = mem_rd(gb, DE);
  CYC(0x5aad, 0x5aaf); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(0x5aaf, 0x5ab0); ret_effect(gb); return; }
  CYC(0x5aaf, 0x5ab0);
  CYC(0x5ab0, 0x5ab2); A = 0xa7;
  CYC(0x5ab2, 0x5ab5);
  playSound_b00_hook(gb);
}

void checkCanPlaceDiamondOnTile_hook(GB *gb) {
  CYC(0x5ab5, 0x5ab7); H = 0xce;
  CYC(0x5ab7, 0x5ab8); A = mem_rd(gb, HL);
  CYC(0x5ab8, 0x5ab9); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5ab9, 0x5aba); ret_effect(gb); return; }
  CYC(0x5ab9, 0x5aba);
  CYC(0x5aba, 0x5abc); H = 0xcf;
  CYC(0x5abc, 0x5abd); A = mem_rd(gb, HL);
  CYC(0x5abd, 0x5abf); alu_cp(gb, 0xda);
  CYC(0x5abf, 0x5ac0); ret_effect(gb);
}

static void switch_hook_update_other_positions(GB *gb, uint16_t sp0_) {
  CALL_C(0x5a3e, checkRelatedObject2States_hook, 0x5a94, 0x5a41);
  if (!(F & FZ)) CALL_C_CC(0x5a41, objectCopyPosition_hook, 0x2242, 0x5a44);
  else CYC(0x5a41, 0x5a44);
  CYC(0x5a44, 0x5a45); push_effect(gb, DE);
  CYC(0x5a45, 0x5a47); E = 0x0f;
  CYC(0x5a47, 0x5a48); A = mem_rd(gb, DE);
  CYC(0x5a48, 0x5a4b); SET_DE(w1ReservedItemE_var3b);
  CYC(0x5a4b, 0x5a4c); mem_wr(gb, DE, A);
  CYC(0x5a4c, 0x5a4f); SET_HL(w1Link_y);
  CYC(0x5a4f, 0x5a51); E = 0x36;
  CYC(0x5a51, 0x5a53); B = 0x06;
  CALL_C(0x5a53, copyMemoryReverse_hook, 0x047f, 0x5a56);
  CYC(0x5a56, 0x5a57); SET_DE(pop_effect(gb));
  CYC(0x5a57, 0x5a58); ret_effect(gb);
}

void switchHookState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x591a, 0x591c); E = 0x05;
  CYC(0x591c, 0x591d); A = mem_rd(gb, DE);
  CYC(0x591d, 0x591e); push_effect(gb, 0x591e);
  switch (switch_hook_jump_table(gb)) {
    case 0x5926: goto substate0;
    case 0x59ba: goto substate1;
    case 0x59c6: goto substate2;
    case 0x5a58: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x5926, 0x5927); H = D;
  CYC(0x5927, 0x5929); L = 0x2f;
  CYC(0x5929, 0x592b); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x592b, 0x592e); func_5902_hook(gb); return; }
  CYC(0x592b, 0x592e);
  CYC(0x592e, 0x5930); L = 0x21;
  CYC(0x5930, 0x5932); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5932, 0x5935); itemAnimate_hook(gb); return; }
  CYC(0x5932, 0x5935);
  CALL_C(0x5935, checkRelatedObject2States_hook, 0x5a94, 0x5938);
  if (!(F & FC)) { CYCT(0x5938, 0x593a); switch_hook_cancel_and_retract(gb); return; }
  CYC(0x5938, 0x593a);
  if (!(F & FZ)) { CYCT(0x593a, 0x593c); goto object_collision; }
  CYC(0x593a, 0x593c);
  CYC(0x593c, 0x593e); A = 0x08;
  CALL_C(0x593e, itemTryToBreakTile_hook, 0x2bef, 0x5941);
  if (!(F & FC)) { CYCT(0x5941, 0x5944); switch_hook_cancel_and_retract(gb); return; }
  CYC(0x5941, 0x5944);
  CYC(0x5944, 0x5945); H = D;
  CYC(0x5945, 0x5947); L = 0x03;
  CYC(0x5947, 0x5949); A = mem_rd(gb, hFF8E);
  CYC(0x5949, 0x594a); mem_wr(gb, HL, A);
  CYC(0x594a, 0x594c); L = 0x3c;
  CYC(0x594c, 0x594e); A = mem_rd(gb, hFF93);
  CYC(0x594e, 0x594f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x594f, 0x5951); A = mem_rd(gb, hFF92);
  CYC(0x5951, 0x5952); mem_wr(gb, HL, A);
  CALL_C(0x5952, itemMimicBgTile_hook, 0x628e, 0x5955);
  CYC(0x5955, 0x5956); H = D;
  CYC(0x5956, 0x5958); L = 0x3c;
  CYC(0x5958, 0x5959); C = mem_rd(gb, HL);
  CALL_C(0x5959, objectSetShortPosition_hook, 0x20c3, 0x595c);
  CALL_C(0x595c, objectSetVisiblec2_hook, 0x1e45, 0x595f);
  CYC(0x595f, 0x5961);
  goto initialize_swap;

object_collision:
  CYC(0x5961, 0x5964); A = W8(w1ReservedInteraction1_id);
  CYC(0x5964, 0x5966); alu_cp(gb, 0x14);
  if (F & FZ) { CYCT(0x5966, 0x5968); goto reject_object; }
  CYC(0x5966, 0x5968);
  CYC(0x5968, 0x596a); A = 0x0b;
  CALL_C(0x596a, objectGetRelatedObject2Var_hook, 0x2164, 0x596d);
  CYC(0x596d, 0x596e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x596e, 0x596f); L = alu_inc8(gb, L);
  CYC(0x596f, 0x5970); C = mem_rd(gb, HL);
  CYC(0x5970, 0x5971); B = A;
  CYC(0x5971, 0x5974); SET_HL(0x5e92);
  CYC(0x5974, 0x5976); E = 0x05;
  CALL_C(0x5976, interBankCall_hook, 0x008a, 0x5979);
  CYC(0x5979, 0x597b); B = alu_rl(gb, B);
  if (F & FC) { CYCT(0x597b, 0x597d); goto reject_object; }
  CYC(0x597b, 0x597d);
  CYC(0x597d, 0x597f); A = 0x0b;
  CALL_C(0x597f, objectGetRelatedObject2Var_hook, 0x2164, 0x5982);
  CALL_C(0x5982, objectTakePosition_hook, 0x2274, 0x5985);
  CALL_C(0x5985, objectSetInvisible_hook, 0x1e7b, 0x5988);

initialize_swap:
  CYC(0x5988, 0x598a); A = 0x02;
  CYC(0x598a, 0x598d); W8(wSwitchHookState) = A;
  CYC(0x598d, 0x598f); A = 0x8e;
  CALL_C(0x598f, playSound_b00_hook, 0x0c98, 0x5992);
  CALL_C(0x5992, itemIncSubstate_hook, 0x23ef, 0x5995);
  CYC(0x5995, 0x5997); L = 0x0f;
  CYC(0x5997, 0x5999); mem_wr(gb, HL, 0x00);
  CYC(0x5999, 0x599b); L = 0x2f;
  CYC(0x599b, 0x599d); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(0x599d, 0x59a0); SET_HL(w1ReservedItemE);
  CYC(0x59a0, 0x59a2); A = 0x01;
  CYC(0x59a2, 0x59a3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x59a3, 0x59a5); mem_wr(gb, HL, 0x09);
  CYC(0x59a5, 0x59a7); L = 0x04;
  CYC(0x59a7, 0x59a8); alu_xor(gb, A);
  CYC(0x59a8, 0x59a9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x59a9, 0x59aa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(0x59aa, objectCopyPosition_hook, 0x2242, 0x59ad);
  CYC(0x59ad, 0x59b0);
  resetLinkInvincibility_hook(gb);
  return;

reject_object:
  CYC(0x59b0, 0x59b2); A = 0x05;
  CALL_C(0x59b2, objectGetRelatedObject2Var_hook, 0x2164, 0x59b5);
  CYC(0x59b5, 0x59b7); mem_wr(gb, HL, 0x03);
  CYC(0x59b7, 0x59ba);
  switch_hook_cancel_and_retract(gb);
  return;

substate1:
  CYC(0x59ba, 0x59bb); H = D;
  CYC(0x59bb, 0x59bd); L = 0x0f;
  CYC(0x59bd, 0x59be); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x59be, 0x59bf); A = mem_rd(gb, HL);
  CYC(0x59bf, 0x59c1); alu_cp(gb, 0xf1);
  if (F & FC) CALL_C_CC(0x59c1, itemIncSubstate_hook, 0x23ef, 0x59c4);
  else CYC(0x59c1, 0x59c4);
  CYC(0x59c4, 0x59c6);
  switch_hook_update_other_positions(gb, sp0_);
  return;

substate2:
  CYC(0x59c6, 0x59c7); push_effect(gb, DE);
  CYC(0x59c7, 0x59ca); SET_HL(0xde36);
  CYC(0x59ca, 0x59cd); SET_DE(0xde30);
  CYC(0x59cd, 0x59cf); B = 0x06;
  do {
    CYC(0x59cf, 0x59d0); A = mem_rd(gb, DE);
    CYC(0x59d0, 0x59d1); C = mem_rd(gb, HL);
    CYC(0x59d1, 0x59d2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x59d2, 0x59d3); A = C;
    CYC(0x59d3, 0x59d4); mem_wr(gb, DE, A);
    CYC(0x59d4, 0x59d5); E = alu_inc8(gb, E);
    CYC(0x59d5, 0x59d6); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x59d6, 0x59d8); else CYC(0x59d6, 0x59d8);
  } while (!(F & FZ));
  CYC(0x59d8, 0x59d9); SET_DE(pop_effect(gb));
  CYC(0x59d9, 0x59db); E = 0x02;
  CYC(0x59db, 0x59dc); A = mem_rd(gb, DE);
  CYC(0x59dc, 0x59dd); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x59dd, 0x59df);
    goto done_centering;
  }
  CYC(0x59dd, 0x59df);
  CYC(0x59df, 0x59e2); A = W8(w1Link_direction);
  CYC(0x59e2, 0x59e3); L = A;
  CYC(0x59e3, 0x59e4); alu_add(gb, A);
  CYC(0x59e4, 0x59e5); alu_add(gb, L);
  CYC(0x59e5, 0x59e8); SET_HL(0x5847);
  CYC(0x59e8, 0x59e9); switch_hook_add_a_to_hl(gb, 0x59e9);
  CYC(0x59e9, 0x59ea); push_effect(gb, DE);
  CYC(0x59ea, 0x59ed); SET_DE(0xde31);
  CYC(0x59ed, 0x59ee); A = mem_rd(gb, DE);
  CYC(0x59ee, 0x59ef); alu_add(gb, mem_rd(gb, HL));
  CYC(0x59ef, 0x59f0); mem_wr(gb, DE, A);
  CYC(0x59f0, 0x59f1); SET_HL(HL + 1);
  CYC(0x59f1, 0x59f3); E = 0x33;
  CYC(0x59f3, 0x59f4); A = mem_rd(gb, DE);
  CYC(0x59f4, 0x59f5); alu_add(gb, mem_rd(gb, HL));
  CYC(0x59f5, 0x59f6); mem_wr(gb, DE, A);
  CYC(0x59f6, 0x59f8); E = 0x31;
  CALL_C(0x59f8, getShortPositionFromDE_hook, 0x209b, 0x59fb);
  CYC(0x59fb, 0x59fc); SET_DE(pop_effect(gb));
  CYC(0x59fc, 0x59fd); L = A;
  CALL_C(0x59fd, checkCanPlaceDiamondOnTile_hook, 0x5ab5, 0x5a00);
  if (!(F & FZ)) {
    CYC(0x5a00, 0x5a02);
    CYC(0x5a02, 0x5a03); E = L;
    CYC(0x5a03, 0x5a06); A = W8(w1Link_direction);
    CYC(0x5a06, 0x5a09); SET_BC(0x5a3a);
    CALL_C(0x5a09, addAToBc_hook, 0x006d, 0x5a0c);
    CYC(0x5a0c, 0x5a0d); A = mem_rd(gb, BC);
    CYC(0x5a0d, 0x5a0e); switch_hook_add_a_to_hl(gb, 0x5a0e);
    CALL_C(0x5a0e, checkCanPlaceDiamondOnTile_hook, 0x5ab5, 0x5a11);
    if (!(F & FZ)) {
      CYC(0x5a11, 0x5a13);
      CYC(0x5a13, 0x5a14); L = E;
    } else {
      CYCT(0x5a11, 0x5a13);
    }
  } else {
    CYCT(0x5a00, 0x5a02);
  }
  CYC(0x5a14, 0x5a15); C = L;
  CYC(0x5a15, 0x5a18); SET_HL(0xde31);
  CALL_C(0x5a18, setShortPosition_paramC_hook, 0x20b9, 0x5a1b);

done_centering:
  CYC(0x5a1b, 0x5a1d); E = 0x0a;
  CYC(0x5a1d, 0x5a20); SET_HL(0xde30);
  CYC(0x5a20, 0x5a22); B = 0x04;
  CALL_C(0x5a22, copyMemory_hook, 0x0486, 0x5a25);
  CYC(0x5a25, 0x5a28); SET_HL(w1Link_direction);
  CYC(0x5a28, 0x5a29); A = mem_rd(gb, HL);
  CYC(0x5a29, 0x5a2b); alu_xor(gb, 0x02);
  CYC(0x5a2b, 0x5a2c); mem_wr(gb, HL, A);
  CALL_C(0x5a2c, itemIncSubstate_hook, 0x23ef, 0x5a2f);
  CALL_C(0x5a2f, checkRelatedObject2States_hook, 0x5a94, 0x5a32);
  if (!(F & FC)) {
    CYCT(0x5a32, 0x5a34);
  } else {
    CYC(0x5a32, 0x5a34);
    if (F & FZ) {
      CYCT(0x5a34, 0x5a36);
    } else {
      CYC(0x5a34, 0x5a36);
      CYC(0x5a36, 0x5a38); mem_wr(gb, HL, 0x02);
    }
  }
  CYC(0x5a38, 0x5a3a);
  switch_hook_update_other_positions(gb, sp0_);
  return;

substate3:
  CYC(0x5a58, 0x5a59); H = D;
  CYC(0x5a59, 0x5a5b); L = 0x0f;
  CYC(0x5a5b, 0x5a5c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5a5c, 0x5a5f); push_effect(gb, 0x5a5f);
  switch_hook_update_other_positions(gb, sp0_);
  CYC(0x5a5f, 0x5a61); E = 0x0f;
  CYC(0x5a61, 0x5a62); A = mem_rd(gb, DE);
  CYC(0x5a62, 0x5a63); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5a63, 0x5a64); ret_effect(gb); return; }
  CYC(0x5a63, 0x5a64);
  CALL_C(0x5a64, checkRelatedObject2States_hook, 0x5a94, 0x5a67);
  if (!(F & FZ)) {
    CYCT(0x5a67, 0x5a69);
    CYC(0x5a88, 0x5a8a); mem_wr(gb, HL, 0x03);
    goto delete;
  }
  CYC(0x5a67, 0x5a69);
  CALL_C(0x5a69, objectGetTileCollisions_hook, 0x14ad, 0x5a6c);
  CALL_C(0x5a6c, checkCanPlaceDiamondOnTile_hook, 0x5ab5, 0x5a6f);
  if (!(F & FZ)) { CYCT(0x5a6f, 0x5a71); goto break_tile; }
  CYC(0x5a6f, 0x5a71);
  CYC(0x5a71, 0x5a72); C = L;
  CYC(0x5a72, 0x5a74); E = 0x3d;
  CYC(0x5a74, 0x5a75); A = mem_rd(gb, DE);
  CYC(0x5a75, 0x5a77); alu_cp(gb, 0xdb);
  if (!(F & FZ)) { CYCT(0x5a77, 0x5a79); goto break_tile; }
  CYC(0x5a77, 0x5a79);
  CALL_C(0x5a79, setTile_hook, 0x3a9c, 0x5a7c);
  CYC(0x5a7c, 0x5a7e);
  goto delete;

break_tile:
  CYC(0x5a7e, 0x5a81); SET_HL(0x4809);
  CYC(0x5a81, 0x5a83); E = 0x06;
  CALL_C(0x5a83, interBankCall_hook, 0x008a, 0x5a86);
  CYC(0x5a86, 0x5a88);

delete:
  CYC(0x5a8a, 0x5a8b); alu_xor(gb, A);
  CYC(0x5a8b, 0x5a8e); W8(wSwitchHookState) = A;
  CYC(0x5a8e, 0x5a91); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x5a91, 0x5a94);
  itemDelete_hook(gb);
}
