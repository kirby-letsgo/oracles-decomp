#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(itemCode0bPost);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+49); alu_cp(gb, 0xf8);
  if (F & FC) {
    CYCT(b_+49, b_+51);
  } else {
    CYC(b_+49, b_+51);
    CYC(b_+51, b_+52); alu_xor(gb, A);
  }
  CYC(b_+52, b_+54); H = 0xd0;
  CYC(b_+54, b_+55); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+55, b_+56); C = A;
  CYC(b_+56, b_+58); A = 0x00;
  CYC(b_+58, b_+59); alu_sbc(gb, A);
  CYC(b_+59, b_+60); alu_rra(gb);
  CYC(b_+60, b_+62); C = alu_rr(gb, C);
  CYC(b_+62, b_+63); alu_rra(gb);
  CYC(b_+63, b_+65); C = alu_rr(gb, C);
  CYC(b_+65, b_+66); alu_xor(gb, A);
  for (;;) {
    CYC(b_+66, b_+67); alu_add(gb, C);
    CYC(b_+67, b_+68); E = alu_dec8(gb, E);
    if (!(F & FZ)) {
      CYCT(b_+68, b_+70);
    } else {
      CYC(b_+68, b_+70);
      break;
    }
  }
  CYC(b_+70, b_+71); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+72); alu_add(gb, B);
  CYC(b_+72, b_+73); H = D;
  CYC(b_+73, b_+74); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+74, b_+75); ret_effect(gb);
}

void itemCode0bPost_hook(GB *gb) {
  BASE(itemCode0bPost);
  CYC(b_+0, b_+3); A = W8(w1WeaponItem_id);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0a);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    TAIL(itemDelete);
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = W8(w1WeaponItem_var2f);
  CYC(b_+11, b_+13); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+16);
    TAIL(itemDelete);
  }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+20); A = W8(w1WeaponItem_zh);
  CYC(b_+20, b_+22); L = 0x0f;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+25); L = 0x06;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+26, b_+28);
  } else {
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+30); mem_wr(gb, HL, 0x03);
  }
  CYC(b_+30, b_+31); E = mem_rd(gb, HL);
  CYC(b_+31, b_+32); push_effect(gb, DE);
  CYC(b_+32, b_+34); B = 0x03;
  CYC(b_+34, b_+37); SET_HL(w1WeaponItem_yh);
  CYC(b_+37, b_+40); push_effect(gb, b_+40);
  switch_hook_set_position_component(gb);
  CYC(b_+40, b_+41); SET_DE(pop_effect(gb));
  CYC(b_+41, b_+43); B = 0x00;
  CYC(b_+43, b_+46); SET_HL(w1WeaponItem_xh);
  switch_hook_set_position_component(gb);
}

void itemCode0aPost_hook(GB *gb) {
  BASE(itemCode0aPost);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = W8(wSwitchHookState);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+8, b_+11);
    TAIL(itemDelete);
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL(func_5902);
}

void itemCode0b_hook(GB *gb) {
  BASE(itemCode0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+8);
  CALL_C(b_+8, itemIncState_hook, SYM(itemIncState), b_+11);
  CYC(b_+11, b_+13); L = 0x06;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x03);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CALL_C(b_+16, itemSetAnimation_hook, SYM(itemSetAnimation), b_+19);
  CYC(b_+19, b_+22);
  TAIL(objectSetVisible83);
}

static void switch_hook_start_retracting(GB *gb) {
  BASE(itemCode0a);
  CYC(b_+194, b_+195); H = D;
  CYC(b_+195, b_+197); L = 0x24;
  CYC(b_+197, b_+199); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(b_+199, b_+201); A = 0x02;
  CYC(b_+201, b_+204);
  itemSetState_hook(gb);
}

static void switch_hook_cancel_and_retract(GB *gb) {
  BASE(itemCode0a);
  CYC(b_+187, b_+188); alu_xor(gb, A);
  CYC(b_+188, b_+191); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+191, b_+194); W8(wSwitchHookState) = A;
  switch_hook_start_retracting(gb);
}

void itemCode0a_hook(GB *gb) {
  BASE(itemCode0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+5); W8(wDisableRingTransformations) = A;
  CYC(b_+5, b_+7); A = 0x80;
  CYC(b_+7, b_+10); W8(wcc92) = A;
  CYC(b_+10, b_+12); E = 0x04;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (switch_hook_jump_table(gb));
    if (jt_ == b_+22) { goto state0; }
    else if (jt_ == b_+83) { goto state1; }
    else if (jt_ == b_+204) { goto state2; }
    else if (jt_ == SYM(switchHookState3) && hook_is(gb, SYM(switchHookState3), switchHookState3_hook)) { switchHookState3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+22, b_+24); A = 0x1f;
  CALL_C(b_+24, loadWeaponGfx_b00_hook, SYM(loadWeaponGfx_b00), b_+27);
  CYC(b_+27, b_+30); SET_HL(b_+71);
  CALL_C(b_+30, applyOffsetTableHL_hook, SYM(applyOffsetTableHL), b_+33);
  CALL_C(b_+33, objectSetVisible82_hook, SYM(objectSetVisible82), b_+36);
  CALL_C(b_+36, loadAttributesAndGraphicsAndIncState_hook, SYM(loadAttributesAndGraphicsAndIncState), b_+39);
  CYC(b_+39, b_+42); SET_BC((SYM(galeSeedTryToWarpLink__substate3) + 1));
  CYC(b_+42, b_+45); A = W8(wSwitchHookLevel);
  CYC(b_+45, b_+46); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+46, b_+48);
  } else {
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+51); SET_BC(0x7826);
  }
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = 0x10;
  CYC(b_+54, b_+55); mem_wr(gb, HL, B);
  CYC(b_+55, b_+57); L = 0x06;
  CYC(b_+57, b_+58); mem_wr(gb, HL, C);
  CYC(b_+58, b_+60); L = 0x2f;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x01);
  CALL_C(b_+62, itemUpdateAngle_hook, SYM(itemUpdateAngle), b_+65);
  CYC(b_+65, b_+66); A = mem_rd(gb, HL);
  CYC(b_+66, b_+68); alu_add(gb, 0x02);
  CYC(b_+68, b_+71);
  TAIL(itemSetAnimation);

state1:
  CYC(b_+83, b_+85); E = 0x2a;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+87, b_+89);
  } else {
    CYC(b_+87, b_+89);
    CYC(b_+89, b_+91); alu_bit(gb, 5, A);
    if (!(F & FZ)) {
      CYCT(b_+91, b_+93);
      goto go_to_state3;
    }
    CYC(b_+91, b_+93);
    CYC(b_+93, b_+95);
    switch_hook_start_retracting(gb);
    return;
  }
  CYC(b_+95, b_+96); H = D;
  CYC(b_+96, b_+98); L = 0x2f;
  CYC(b_+98, b_+100); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+100, b_+103); itemDelete_hook(gb); return; }
  CYC(b_+100, b_+103);
  CALL_C(b_+103, itemDecCounter1_hook, SYM(itemDecCounter1), b_+106);
  if (F & FZ) { CYCT(b_+106, b_+108); switch_hook_start_retracting(gb); return; }
  CYC(b_+106, b_+108);
  CALL_C(b_+108, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+111);
  if (!(F & FC)) { CYCT(b_+111, b_+113); switch_hook_start_retracting(gb); return; }
  CYC(b_+111, b_+113);
  CALL_C(b_+113, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+116);
  if (!(F & FC)) {
    CYCT(b_+116, b_+118);
    goto no_collision;
  }
  CYC(b_+116, b_+118);
  CALL_C(b_+118, itemCheckCanPassSolidTile_hook, SYM(itemCheckCanPassSolidTile), b_+121);
  if (!(F & FZ)) { CYCT(b_+121, b_+123); goto collision; }
  CYC(b_+121, b_+123);

no_collision:
  CYC(b_+123, b_+125); E = 0x2f;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+128); alu_bit(gb, 3, A);
  if (!(F & FZ)) {
    CYCT(b_+128, b_+130);
  } else {
    CYC(b_+128, b_+130);
    CALL_C(b_+130, getFreeItemSlot_hook, SYM(getFreeItemSlot), b_+133);
    if (!(F & FZ)) {
      CYCT(b_+133, b_+135);
    } else {
      CYC(b_+133, b_+135);
      CYC(b_+135, b_+136); A = alu_inc8(gb, A);
      CYC(b_+136, b_+137); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+137, b_+139); mem_wr(gb, HL, 0x0b);
      CYC(b_+139, b_+140); H = D;
      CYC(b_+140, b_+142); L = 0x2f;
      CYC(b_+142, b_+144); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
    }
  }
  CALL_C(b_+144, updateSwitchHookSound_hook, SYM(updateSwitchHookSound), b_+147);
  CYC(b_+147, b_+150);
  TAIL(objectApplySpeed);

collision:
  CALL_C(b_+150, objectCreateClinkInteraction_hook, SYM(objectCreateClinkInteraction), b_+153);
  CYC(b_+153, b_+155); A = 0x88;
  CALL_C(b_+155, itemTryToBreakTile_hook, SYM(itemTryToBreakTile), b_+158);
  if (!(F & FC)) { CYCT(b_+158, b_+160); switch_hook_start_retracting(gb); return; }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); E = 0x02;
  CYC(b_+162, b_+164); A = 0x01;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);

go_to_state3:
  CYC(b_+165, b_+167); A = 0x03;
  CALL_C(b_+167, itemSetState_hook, SYM(itemSetState), b_+170);
  CYC(b_+170, b_+172); L = 0x24;
  CYC(b_+172, b_+174); mem_wr(gb, HL, mem_rd(gb, HL) & (uint8_t)~0x80);
  CYC(b_+174, b_+176); A = 0xff;
  CYC(b_+176, b_+179); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+179, b_+181); A = 0x01;
  CYC(b_+181, b_+184); W8(wSwitchHookState) = A;
  CYC(b_+184, b_+187);
  TAIL(resetLinkInvincibility);

state2:
  CYC(b_+204, b_+206); E = 0x05;
  CYC(b_+206, b_+207); A = mem_rd(gb, DE);
  CYC(b_+207, b_+208); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+208, b_+210);
    CYC(b_+245, b_+248); SET_HL(w1Link_yh);
    CALL_C(b_+248, objectTakePosition_hook, SYM(objectTakePosition), b_+251);
    CALL_C(b_+251, itemDecCounter1_hook, SYM(itemDecCounter1), b_+254);
    if (!(F & FZ)) { CYCT(b_+254, b_+255); ret_effect(gb); return; }
    CYC(b_+254, b_+255);
    CYC(b_+255, b_+258);
    TAIL(itemDelete);
  }
  CYC(b_+208, b_+210);
  CALL_C(b_+210, itemDecCounter1_hook, SYM(itemDecCounter1), b_+213);
  CALL_C(b_+213, updateSwitchHookSound_hook, SYM(updateSwitchHookSound), b_+216);
  CALL_C(b_+216, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+219);
  CYC(b_+219, b_+221); E = 0x09;
  CYC(b_+221, b_+222); mem_wr(gb, DE, A);
  CALL_C(b_+222, objectApplySpeed_hook, SYM(objectApplySpeed), b_+225);
  CYC(b_+225, b_+228); SET_BC(0x1008);
  CALL_C(b_+228, itemCheckWithinRangeOfLink_hook, SYM(itemCheckWithinRangeOfLink), b_+231);
  if (!(F & FC)) { CYCT(b_+231, b_+232); ret_effect(gb); return; }
  CYC(b_+231, b_+232);
  CALL_C(b_+232, itemIncSubstate_hook, SYM(itemIncSubstate), b_+235);
  CYC(b_+235, b_+236); L = alu_inc8(gb, L);
  CYC(b_+236, b_+238); mem_wr(gb, HL, 0x03);
  CYC(b_+238, b_+240); L = 0x2f;
  CYC(b_+240, b_+242); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+242, b_+245);
  TAIL(objectSetInvisible);
}

void func_5902_hook(GB *gb) {
  BASE(func_5902);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkRelatedObject2States_hook, SYM(checkRelatedObject2States), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+5);
  } else {
    CYC(b_+3, b_+5);
    if (F & FZ) {
      CYCT(b_+5, b_+7);
    } else {
      CYC(b_+5, b_+7);
      CYC(b_+7, b_+9); A = 0x05;
      CALL_C(b_+9, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+12);
      CYC(b_+12, b_+14); mem_wr(gb, HL, 0x03);
    }
  }
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+18, b_+21); W8(wSwitchHookState) = A;
  CYC(b_+21, b_+24);
  TAIL(itemDelete);
}

void checkRelatedObject2States_hook(GB *gb) {
  BASE(checkRelatedObject2States);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x02;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+8); A = 0x04;
    CALL_C(b_+8, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+11);
    CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+12, b_+14); alu_cp(gb, 0x03);
    if (!(F & FZ)) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
    CYC(b_+14, b_+15);
    CYC(b_+15, b_+16); A = mem_rd(gb, HL);
    CYC(b_+16, b_+18); alu_cp(gb, 0x03);
    if (!(F & FC)) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
    CYC(b_+18, b_+19);
    CYC(b_+19, b_+20); alu_or(gb, D);
  }
  CYC(b_+20, b_+21); alu_scf(gb);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void updateSwitchHookSound_hook(GB *gb) {
  BASE(updateSwitchHookSound);
  CYC(b_+0, b_+2); E = 0x06;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0xa7;
  CYC(b_+8, b_+11);
  TAIL(playSound_b00);
}

void checkCanPlaceDiamondOnTile_hook(GB *gb) {
  BASE(checkCanPlaceDiamondOnTile);
  CYC(b_+0, b_+2); H = 0xce;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); H = 0xcf;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0xda);
  CYC(b_+10, b_+11); ret_effect(gb);
}

static void switch_hook_update_other_positions(GB *gb, uint16_t sp0_) {
  BASE(switchHookState3);
  CALL_C(b_+292, checkRelatedObject2States_hook, SYM(checkRelatedObject2States), b_+295);
  if (!(F & FZ)) CALL_C_CC(b_+295, objectCopyPosition_hook, SYM(objectCopyPosition), b_+298);
  else CYC(b_+295, b_+298);
  CYC(b_+298, b_+299); push_effect(gb, DE);
  CYC(b_+299, b_+301); E = 0x0f;
  CYC(b_+301, b_+302); A = mem_rd(gb, DE);
  CYC(b_+302, b_+305); SET_DE(w1ReservedItemE_var3b);
  CYC(b_+305, b_+306); mem_wr(gb, DE, A);
  CYC(b_+306, b_+309); SET_HL(w1Link_y);
  CYC(b_+309, b_+311); E = 0x36;
  CYC(b_+311, b_+313); B = 0x06;
  CALL_C(b_+313, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+316);
  CYC(b_+316, b_+317); SET_DE(pop_effect(gb));
  CYC(b_+317, b_+318); ret_effect(gb);
}

void switchHookState3_hook(GB *gb) {
  BASE(switchHookState3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (switch_hook_jump_table(gb));
    if (jt_ == b_+12) { goto substate0; }
    else if (jt_ == b_+160) { goto substate1; }
    else if (jt_ == b_+172) { goto substate2; }
    else if (jt_ == b_+318) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x2f;
  CYC(b_+15, b_+17); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+17, b_+20); func_5902_hook(gb); return; }
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+22); L = 0x21;
  CYC(b_+22, b_+24); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+24, b_+27); itemAnimate_hook(gb); return; }
  CYC(b_+24, b_+27);
  CALL_C(b_+27, checkRelatedObject2States_hook, SYM(checkRelatedObject2States), b_+30);
  if (!(F & FC)) { CYCT(b_+30, b_+32); switch_hook_cancel_and_retract(gb); return; }
  CYC(b_+30, b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto object_collision; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = 0x08;
  CALL_C(b_+36, itemTryToBreakTile_hook, SYM(itemTryToBreakTile), b_+39);
  if (!(F & FC)) { CYCT(b_+39, b_+42); switch_hook_cancel_and_retract(gb); return; }
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+43); H = D;
  CYC(b_+43, b_+45); L = 0x03;
  CYC(b_+45, b_+47); A = mem_rd(gb, hFF8E);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+50); L = 0x3c;
  CYC(b_+50, b_+52); A = mem_rd(gb, hFF93);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+55); A = mem_rd(gb, hFF92);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  CALL_C(b_+56, itemMimicBgTile_hook, SYM(itemMimicBgTile), b_+59);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = 0x3c;
  CYC(b_+62, b_+63); C = mem_rd(gb, HL);
  CALL_C(b_+63, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+66);
  CALL_C(b_+66, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+69);
  CYC(b_+69, b_+71);
  goto initialize_swap;

object_collision:
  CYC(b_+71, b_+74); A = W8(w1ReservedInteraction1_id);
  CYC(b_+74, b_+76); alu_cp(gb, 0x14);
  if (F & FZ) { CYCT(b_+76, b_+78); goto reject_object; }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); A = 0x0b;
  CALL_C(b_+80, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+83);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+84, b_+85); L = alu_inc8(gb, L);
  CYC(b_+85, b_+86); C = mem_rd(gb, HL);
  CYC(b_+86, b_+87); B = A;
  CYC(b_+87, b_+90); SET_HL((SYM(itemCode05) + 3));
  CYC(b_+90, b_+92); E = 0x05;
  CALL_C(b_+92, interBankCall_hook, 0x008a, b_+95);
  CYC(b_+95, b_+97); B = alu_rl(gb, B);
  if (F & FC) { CYCT(b_+97, b_+99); goto reject_object; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); A = 0x0b;
  CALL_C(b_+101, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+104);
  CALL_C(b_+104, objectTakePosition_hook, SYM(objectTakePosition), b_+107);
  CALL_C(b_+107, objectSetInvisible_hook, SYM(objectSetInvisible), b_+110);

initialize_swap:
  CYC(b_+110, b_+112); A = 0x02;
  CYC(b_+112, b_+115); W8(wSwitchHookState) = A;
  CYC(b_+115, b_+117); A = 0x8e;
  CALL_C(b_+117, playSound_b00_hook, SYM(playSound_b00), b_+120);
  CALL_C(b_+120, itemIncSubstate_hook, SYM(itemIncSubstate), b_+123);
  CYC(b_+123, b_+125); L = 0x0f;
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); L = 0x2f;
  CYC(b_+129, b_+131); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+131, b_+134); SET_HL(w1ReservedItemE);
  CYC(b_+134, b_+136); A = 0x01;
  CYC(b_+136, b_+137); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+137, b_+139); mem_wr(gb, HL, 0x09);
  CYC(b_+139, b_+141); L = 0x04;
  CYC(b_+141, b_+142); alu_xor(gb, A);
  CYC(b_+142, b_+143); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+143, b_+144); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(b_+144, objectCopyPosition_hook, SYM(objectCopyPosition), b_+147);
  CYC(b_+147, b_+150);
  TAIL(resetLinkInvincibility);

reject_object:
  CYC(b_+150, b_+152); A = 0x05;
  CALL_C(b_+152, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+155);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x03);
  CYC(b_+157, b_+160);
  switch_hook_cancel_and_retract(gb);
  return;

substate1:
  CYC(b_+160, b_+161); H = D;
  CYC(b_+161, b_+163); L = 0x0f;
  CYC(b_+163, b_+164); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+164, b_+165); A = mem_rd(gb, HL);
  CYC(b_+165, b_+167); alu_cp(gb, 0xf1);
  if (F & FC) CALL_C_CC(b_+167, itemIncSubstate_hook, SYM(itemIncSubstate), b_+170);
  else CYC(b_+167, b_+170);
  CYC(b_+170, b_+172);
  switch_hook_update_other_positions(gb, sp0_);
  return;

substate2:
  CYC(b_+172, b_+173); push_effect(gb, DE);
  CYC(b_+173, b_+176); SET_HL(w1ReservedItemE_var36);
  CYC(b_+176, b_+179); SET_DE(w1ReservedItemE_var30);
  CYC(b_+179, b_+181); B = 0x06;
  do {
    CYC(b_+181, b_+182); A = mem_rd(gb, DE);
    CYC(b_+182, b_+183); C = mem_rd(gb, HL);
    CYC(b_+183, b_+184); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+184, b_+185); A = C;
    CYC(b_+185, b_+186); mem_wr(gb, DE, A);
    CYC(b_+186, b_+187); E = alu_inc8(gb, E);
    CYC(b_+187, b_+188); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+188, b_+190); else CYC(b_+188, b_+190);
  } while (!(F & FZ));
  CYC(b_+190, b_+191); SET_DE(pop_effect(gb));
  CYC(b_+191, b_+193); E = 0x02;
  CYC(b_+193, b_+194); A = mem_rd(gb, DE);
  CYC(b_+194, b_+195); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+195, b_+197);
    goto done_centering;
  }
  CYC(b_+195, b_+197);
  CYC(b_+197, b_+200); A = W8(w1Link_direction);
  CYC(b_+200, b_+201); L = A;
  CYC(b_+201, b_+202); alu_add(gb, A);
  CYC(b_+202, b_+203); alu_add(gb, L);
  CYC(b_+203, b_+206); SET_HL(SYM(itemCode0a__offsetsTable));
  CYC(b_+206, b_+207); switch_hook_add_a_to_hl(gb, b_+207);
  CYC(b_+207, b_+208); push_effect(gb, DE);
  CYC(b_+208, b_+211); SET_DE(w1ReservedItemE_var31);
  CYC(b_+211, b_+212); A = mem_rd(gb, DE);
  CYC(b_+212, b_+213); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+213, b_+214); mem_wr(gb, DE, A);
  CYC(b_+214, b_+215); SET_HL(HL + 1);
  CYC(b_+215, b_+217); E = 0x33;
  CYC(b_+217, b_+218); A = mem_rd(gb, DE);
  CYC(b_+218, b_+219); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+219, b_+220); mem_wr(gb, DE, A);
  CYC(b_+220, b_+222); E = 0x31;
  CALL_C(b_+222, getShortPositionFromDE_hook, SYM(getShortPositionFromDE), b_+225);
  CYC(b_+225, b_+226); SET_DE(pop_effect(gb));
  CYC(b_+226, b_+227); L = A;
  CALL_C(b_+227, checkCanPlaceDiamondOnTile_hook, SYM(checkCanPlaceDiamondOnTile), b_+230);
  if (!(F & FZ)) {
    CYC(b_+230, b_+232);
    CYC(b_+232, b_+233); E = L;
    CYC(b_+233, b_+236); A = W8(w1Link_direction);
    CYC(b_+236, b_+239); SET_BC(b_+288);
    CALL_C(b_+239, addAToBc_hook, 0x006d, b_+242);
    CYC(b_+242, b_+243); A = mem_rd(gb, BC);
    CYC(b_+243, b_+244); switch_hook_add_a_to_hl(gb, b_+244);
    CALL_C(b_+244, checkCanPlaceDiamondOnTile_hook, SYM(checkCanPlaceDiamondOnTile), b_+247);
    if (!(F & FZ)) {
      CYC(b_+247, b_+249);
      CYC(b_+249, b_+250); L = E;
    } else {
      CYCT(b_+247, b_+249);
    }
  } else {
    CYCT(b_+230, b_+232);
  }
  CYC(b_+250, b_+251); C = L;
  CYC(b_+251, b_+254); SET_HL(w1ReservedItemE_var31);
  CALL_C(b_+254, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+257);

done_centering:
  CYC(b_+257, b_+259); E = 0x0a;
  CYC(b_+259, b_+262); SET_HL(w1ReservedItemE_var30);
  CYC(b_+262, b_+264); B = 0x04;
  CALL_C(b_+264, copyMemory_hook, SYM(copyMemory), b_+267);
  CYC(b_+267, b_+270); SET_HL(w1Link_direction);
  CYC(b_+270, b_+271); A = mem_rd(gb, HL);
  CYC(b_+271, b_+273); alu_xor(gb, 0x02);
  CYC(b_+273, b_+274); mem_wr(gb, HL, A);
  CALL_C(b_+274, itemIncSubstate_hook, SYM(itemIncSubstate), b_+277);
  CALL_C(b_+277, checkRelatedObject2States_hook, SYM(checkRelatedObject2States), b_+280);
  if (!(F & FC)) {
    CYCT(b_+280, b_+282);
  } else {
    CYC(b_+280, b_+282);
    if (F & FZ) {
      CYCT(b_+282, b_+284);
    } else {
      CYC(b_+282, b_+284);
      CYC(b_+284, b_+286); mem_wr(gb, HL, 0x02);
    }
  }
  CYC(b_+286, b_+288);
  switch_hook_update_other_positions(gb, sp0_);
  return;

substate3:
  CYC(b_+318, b_+319); H = D;
  CYC(b_+319, b_+321); L = 0x0f;
  CYC(b_+321, b_+322); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+322, b_+325); push_effect(gb, b_+325);
  switch_hook_update_other_positions(gb, sp0_);
  CYC(b_+325, b_+327); E = 0x0f;
  CYC(b_+327, b_+328); A = mem_rd(gb, DE);
  CYC(b_+328, b_+329); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+329, b_+330); ret_effect(gb); return; }
  CYC(b_+329, b_+330);
  CALL_C(b_+330, checkRelatedObject2States_hook, SYM(checkRelatedObject2States), b_+333);
  if (!(F & FZ)) {
    CYCT(b_+333, b_+335);
    CYC(b_+366, b_+368); mem_wr(gb, HL, 0x03);
    goto delete;
  }
  CYC(b_+333, b_+335);
  CALL_C(b_+335, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+338);
  CALL_C(b_+338, checkCanPlaceDiamondOnTile_hook, SYM(checkCanPlaceDiamondOnTile), b_+341);
  if (!(F & FZ)) { CYCT(b_+341, b_+343); goto break_tile; }
  CYC(b_+341, b_+343);
  CYC(b_+343, b_+344); C = L;
  CYC(b_+344, b_+346); E = 0x3d;
  CYC(b_+346, b_+347); A = mem_rd(gb, DE);
  CYC(b_+347, b_+349); alu_cp(gb, 0xdb);
  if (!(F & FZ)) { CYCT(b_+349, b_+351); goto break_tile; }
  CYC(b_+349, b_+351);
  CALL_C(b_+351, setTile_hook, SYM(setTile), b_+354);
  CYC(b_+354, b_+356);
  goto delete;

break_tile:
  CYC(b_+356, b_+359); SET_HL((SYM(applyDamageToLink) + 41));
  CYC(b_+359, b_+361); E = 0x06;
  CALL_C(b_+361, interBankCall_hook, 0x008a, b_+364);
  CYC(b_+364, b_+366);

delete:
  CYC(b_+368, b_+369); alu_xor(gb, A);
  CYC(b_+369, b_+372); W8(wSwitchHookState) = A;
  CYC(b_+372, b_+375); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+375, b_+378);
  TAIL(itemDelete);
}

void itemCode09_hook(GB *gb) {
  BASE(itemCode09);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x2f;
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto state2;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = 0x04;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (switch_hook_jump_table(gb));
    if (jt_ == b_+17) { break; }
    else if (jt_ == b_+41) { goto state1; }
    else if (jt_ == b_+47) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+17, itemIncState_hook, SYM(itemIncState), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = 0x0a;
  CYC(b_+23, b_+25); E = 0x30;
  CYC(b_+25, b_+27); B = 0x06;
  CALL_C(b_+27, copyMemory_hook, SYM(copyMemory), b_+30);
  CYC(b_+30, b_+33); SET_HL(w1Link_y);
  CYC(b_+33, b_+35); B = 0x06;
  CALL_C(b_+35, copyMemory_hook, SYM(copyMemory), b_+38);
  CYC(b_+38, b_+41); setCameraFocusedObject_hook(gb);
  return;

state1:
  CYC(b_+41, b_+44); A = W8(w1WeaponItem_id);
  CYC(b_+44, b_+46); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+46, b_+47); ret_effect(gb); return;
  }
  CYC(b_+46, b_+47);

state2:
  CALL_C(b_+47, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+50);
  CYC(b_+50, b_+53); TAIL(itemDelete);
}

void func_5af5_hook(GB *gb) {
  BASE(func_5af5);
  CYC(b_+0, b_+3); SET_HL(w1ReservedItemE);
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = 0x2f;
  CYC(b_+8, b_+10); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+10, b_+11); ret_effect(gb);
}
