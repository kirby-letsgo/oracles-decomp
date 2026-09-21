#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(itemCode04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemTransferKnockbackToLink_hook, SYM(itemTransferKnockbackToLink), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (somaria_jump_table(gb));
    if (jt_ == b_+13) { break; }
    else if (jt_ == b_+33) { goto state1; }
    else if (jt_ == b_+90) { CYC(b_+90, b_+91); ret_effect(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CYC(b_+13, b_+15); A = 0x1c;
  CALL_C(b_+15, loadWeaponGfx_b00_hook, SYM(loadWeaponGfx_b00), b_+18);
  CALL_C(b_+18, loadAttributesAndGraphicsAndIncState_hook, SYM(loadAttributesAndGraphicsAndIncState), b_+21);
  CYC(b_+21, b_+23); A = 0x74;
  CALL_C(b_+23, playSound_b00_hook, SYM(playSound_b00), b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CALL_C(b_+27, itemSetAnimation_hook, SYM(itemSetAnimation), b_+30);
  CYC(b_+30, b_+33); objectSetVisible82_hook(gb);
  return;

state1:
  CYC(b_+33, b_+36); A = W8(w1ParentItem2_animParameter);
  CYC(b_+36, b_+38); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+39); ret_effect(gb); return;
  }
  CYC(b_+38, b_+39);
  CALL_C(b_+39, itemIncState_hook, SYM(itemIncState), b_+42);
  CYC(b_+42, b_+44); C = 0x18;
  CALL_C(b_+44, findItemWithID_hook, SYM(findItemWithID), b_+47);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
  } else {
    CYC(b_+47, b_+49);
    CYC(b_+49, b_+51); L = 0x2f;
    CYC(b_+51, b_+53); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  }
  CYC(b_+53, b_+56); A = W8(w1Link_direction);
  CYC(b_+56, b_+59); SET_HL(b_+91);
  CYC(b_+59, b_+60); somaria_add_double_index(gb, b_+60);
  CYC(b_+60, b_+63); A = W8(w1Link_yh);
  CYC(b_+63, b_+64); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+64, b_+65); B = A;
  CYC(b_+65, b_+66); SET_HL(HL + 1);
  CYC(b_+66, b_+69); A = W8(w1Link_xh);
  CYC(b_+69, b_+70); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+70, b_+71); C = A;
  CALL_C(b_+71, getFreeItemSlot_hook, SYM(getFreeItemSlot), b_+74);
  if (!(F & FZ)) {
    CYCT(b_+74, b_+75); ret_effect(gb); return;
  }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+76, b_+77); L = alu_inc8(gb, L);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x18);
  CYC(b_+79, b_+81); L = 0x0b;
  CYC(b_+81, b_+82); mem_wr(gb, HL, B);
  CYC(b_+82, b_+85); A = W8(w1Link_zh);
  CYC(b_+85, b_+87); L = 0x0f;
  CYC(b_+87, b_+88); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+88, b_+89); L = alu_dec8(gb, L);
  CYC(b_+89, b_+90); mem_wr(gb, HL, C);
  CYC(b_+90, b_+91); ret_effect(gb);
}

static void somaria_check_deletion_trigger(GB *gb) {
  BASE(itemCode18);
  CYC(b_+296, b_+297); H = D;
  CYC(b_+297, b_+299); L = 0x2f;
  CYC(b_+299, b_+301); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(b_+301, b_+302); ret_effect(gb);
}

static void somaria_push_link_away(GB *gb) {
  BASE(itemCode18);
  CYC(b_+302, b_+304); E = 0x26;
  CYC(b_+304, b_+306); A = 0x07;
  CYC(b_+306, b_+307); mem_wr(gb, DE, A);
  CYC(b_+307, b_+310); SET_HL(w1Link);
  CYC(b_+310, b_+313); preventObjectHFromPassingObjectD_hook(gb);
}

static void somaria_check_block_in_place(GB *gb) {
  BASE(itemCode18);
  CYC(b_+313, b_+315); E = 0x32;
  CYC(b_+315, b_+316); A = mem_rd(gb, DE);
  CYC(b_+316, b_+317); L = A;
  CYC(b_+317, b_+319); H = (uint8_t)(wRoomLayout >> 8);
  CYC(b_+319, b_+320); A = mem_rd(gb, HL);
  CYC(b_+320, b_+322); alu_cp(gb, 0xda);
  if (!(F & FZ)) {
    CYCT(b_+322, b_+323); ret_effect(gb); return;
  }
  CYC(b_+322, b_+323);
  CYC(b_+323, b_+325); H = (uint8_t)(wRoomCollisions >> 8);
  CYC(b_+325, b_+326); A = mem_rd(gb, HL);
  CYC(b_+326, b_+328); alu_cp(gb, 0x0f);
  CYC(b_+328, b_+329); ret_effect(gb);
}

static void somaria_remove_block(GB *gb, uint16_t sp0_) {
  BASE(itemCode18);
  CYC(b_+329, b_+332); push_effect(gb, b_+332); somaria_check_block_in_place(gb);
  if (!(F & FZ)) {
    CYCT(b_+332, b_+333); ret_effect(gb); return;
  }
  CYC(b_+332, b_+333);
  CYC(b_+333, b_+335); E = 0x32;
  CYC(b_+335, b_+336); A = mem_rd(gb, DE);
  CALL_C(b_+336, getTileIndexFromRoomLayoutBuffer_hook, SYM(getTileIndexFromRoomLayoutBuffer), b_+339);
  CYC(b_+339, b_+342); setTile_hook(gb);
}

static void somaria_check_block_can_appear(GB *gb, uint16_t sp0_) {
  BASE(itemCode18);
  CYC(b_+342, b_+345); A = W8(wActiveGroup);
  CYC(b_+345, b_+347); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(b_+347, b_+349);
  } else {
    CYC(b_+347, b_+349);
    CYC(b_+349, b_+352); A = W8(wActiveRoom);
    CYC(b_+352, b_+354); alu_cp(gb, 0xe8);
    if (F & FZ) {
      CYCT(b_+354, b_+356);
      goto disallow;
    }
    CYC(b_+354, b_+356);
  }
  CYC(b_+356, b_+358); E = 0x0f;
  CYC(b_+358, b_+359); A = mem_rd(gb, DE);
  CYC(b_+359, b_+360); A = alu_dec8(gb, A);
  CYC(b_+360, b_+362); alu_cp(gb, 0xfc);
  if (F & FC) {
    CYCT(b_+362, b_+364);
    goto disallow;
  }
  CYC(b_+362, b_+364);
  CALL_C(b_+364, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+367);
  if (!(F & FZ)) {
    CYCT(b_+367, b_+368); ret_effect(gb); return;
  }
  CYC(b_+367, b_+368);
  CYC(b_+368, b_+371); A = W8(wTilesetFlags);
  CYC(b_+371, b_+373); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+373, b_+374); ret_effect(gb); return;
  }
  CYC(b_+373, b_+374);
  CYC(b_+374, b_+376); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+376, b_+377); ret_effect(gb); return;
  }
  CYC(b_+376, b_+377);
  CYC(b_+377, b_+378); A = L;
  CYC(b_+378, b_+380); alu_add(gb, 0x10);
  CYC(b_+380, b_+381); L = A;
  CYC(b_+381, b_+382); A = mem_rd(gb, HL);
  CYC(b_+382, b_+384); alu_cp(gb, 0x0f);
  CYC(b_+384, b_+385); ret_effect(gb);
  return;

disallow:
  CYC(b_+385, b_+386); alu_or(gb, D);
  CYC(b_+386, b_+387); ret_effect(gb);
}

static void somaria_align_on_tile(GB *gb, uint16_t sp0_) {
  BASE(itemCode18);
  CALL_C(b_+422, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+425);
  CYC(b_+425, b_+427); L = 0x0b;
  CYC(b_+427, b_+428); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+428, b_+429); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+429, b_+430); ret_effect(gb);
}

static void somaria_create_block_if_not_on_hazard(GB *gb, uint16_t sp0_) {
  BASE(itemCode18);
  CYC(b_+387, b_+390); push_effect(gb, b_+390); somaria_align_on_tile(gb, sp0_);
  CALL_C(b_+390, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+393);
  CYC(b_+393, b_+394); push_effect(gb, HL);
  CYC(b_+394, b_+397); SET_HL(hazardCollisionTable);
  CALL_C(b_+397, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+400);
  CYC(b_+400, b_+401); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+401, b_+403);
    CYC(b_+420, b_+421); alu_or(gb, D);
    CYC(b_+421, b_+422); ret_effect(gb);
    return;
  }
  CYC(b_+401, b_+403);
  CYC(b_+403, b_+404); B = mem_rd(gb, HL);
  CYC(b_+404, b_+406); mem_wr(gb, HL, 0xda);
  CYC(b_+406, b_+408); H = (uint8_t)(wRoomCollisions >> 8);
  CYC(b_+408, b_+410); mem_wr(gb, HL, 0x0f);
  CYC(b_+410, b_+412); E = 0x32;
  CYC(b_+412, b_+413); A = L;
  CYC(b_+413, b_+414); mem_wr(gb, DE, A);
  CYC(b_+414, b_+415); C = A;
  CALL_C(b_+415, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+418);
  CYC(b_+418, b_+419); alu_xor(gb, A);
  CYC(b_+419, b_+420); ret_effect(gb);
}

void itemCode18_hook(GB *gb) {
  BASE(itemCode18);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (somaria_jump_table(gb));
    if (jt_ == b_+14) { break; }
    else if (jt_ == b_+38) { goto state1; }
    else if (jt_ == b_+184) { goto state2; }
    else if (jt_ == b_+243) { goto state3; }
    else if (jt_ == b_+101) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+14, itemMergeZPositionIfSidescrollingArea_hook, SYM(itemMergeZPositionIfSidescrollingArea), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); somaria_align_on_tile(gb, sp0_);
  CALL_C(b_+20, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CALL_C(b_+24, itemSetAnimation_hook, SYM(itemSetAnimation), b_+27);
  CALL_C(b_+27, itemIncState_hook, SYM(itemIncState), b_+30);
  CYC(b_+30, b_+32); A = 0x7b;
  CALL_C(b_+32, playSound_b00_hook, SYM(playSound_b00), b_+35);
  CYC(b_+35, b_+38); objectSetVisible83_hook(gb);
  return;

state1:
  CYC(b_+38, b_+41); push_effect(gb, b_+41); somaria_check_block_can_appear(gb, sp0_);
  if (F & FZ) {
    CYCT(b_+41, b_+44); push_effect(gb, b_+44); somaria_push_link_away(gb);
  } else {
    CYC(b_+41, b_+44);
  }
  CALL_C(b_+44, itemAnimate_hook, SYM(itemAnimate), b_+47);
  CYC(b_+47, b_+49); E = 0x21;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = 0x1b;
  CYC(b_+55, b_+57); A = 0x0d;
  CYC(b_+57, b_+58); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+58, b_+59); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x36);
  CYC(b_+61, b_+63); L = 0x24;
  CYC(b_+63, b_+65); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

check_create_block:
  CYC(b_+65, b_+68); push_effect(gb, b_+68); somaria_check_block_can_appear(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+70);
    goto delete_with_puff;
  }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+73); push_effect(gb, b_+73); somaria_create_block_if_not_on_hazard(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+73, b_+75);
    goto delete_with_puff;
  }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+78); L = 0x0f;
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+81); L = 0x05;
  CYC(b_+81, b_+82); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x03);
  CYC(b_+84, b_+86); L = 0x26;
  CYC(b_+86, b_+88); A = 0x04;
  CYC(b_+88, b_+89); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+89, b_+90); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+90, b_+92); L = 0x2f;
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); alu_and(gb, 0xf0);
  CYC(b_+95, b_+96); mem_wr(gb, HL, A);
  CYC(b_+96, b_+98); A = 0x01;
  CYC(b_+98, b_+101); itemSetAnimation_hook(gb);
  return;

state4:
  CYC(b_+101, b_+103); E = 0x05;
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); push_effect(gb, b_+105);
  do { uint16_t jt_ = (somaria_jump_table(gb));
    if (jt_ == b_+109) { break; }
    else if (jt_ == b_+142) { goto state4_substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+109, itemIncSubstate_hook, SYM(itemIncSubstate), b_+112);
  CALL_C(b_+112, itemUpdateAngle_hook, SYM(itemUpdateAngle), b_+115);
  CYC(b_+115, b_+118); SET_BC(0x1420);
  CYC(b_+118, b_+121); A = W8(wBraceletLevel);
  CYC(b_+121, b_+123); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+123, b_+125);
  } else {
    CYC(b_+123, b_+125);
    CYC(b_+125, b_+128); SET_BC(0x1e15);
  }
  CYC(b_+128, b_+130); L = 0x10;
  CYC(b_+130, b_+131); mem_wr(gb, HL, B);
  CYC(b_+131, b_+133); L = 0x06;
  CYC(b_+133, b_+134); mem_wr(gb, HL, C);
  CYC(b_+134, b_+136); A = 0x71;
  CALL_C(b_+136, playSound_b00_hook, SYM(playSound_b00), b_+139);
  CYC(b_+139, b_+142); push_effect(gb, b_+142); somaria_remove_block(gb, sp0_);

state4_substate1:
  CALL_C(b_+142, itemUpdateDamageToApply_hook, SYM(itemUpdateDamageToApply), b_+145);
  if (F & FC) {
    CYCT(b_+145, b_+147);
    goto delete_with_puff;
  }
  CYC(b_+145, b_+147);
  CYC(b_+147, b_+150); push_effect(gb, b_+150); somaria_check_deletion_trigger(gb);
  if (!(F & FZ)) {
    CYCT(b_+150, b_+152);
    goto delete_with_puff;
  }
  CYC(b_+150, b_+152);
  CALL_C(b_+152, objectApplySpeed_hook, SYM(objectApplySpeed), b_+155);
  CYC(b_+155, b_+158); push_effect(gb, b_+158); somaria_push_link_away(gb);
  CALL_C(b_+158, itemDecCounter1_hook, SYM(itemDecCounter1), b_+161);
  CYC(b_+161, b_+163); L = 0x26;
  CYC(b_+163, b_+165); A = 0x04;
  CYC(b_+165, b_+166); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+166, b_+167); mem_wr(gb, HL, A);
  if (!(F & FZ)) {
    CYCT(b_+167, b_+168); ret_effect(gb); return;
  }
  CYC(b_+167, b_+168);
  CYC(b_+168, b_+170);
  goto check_create_block;

remove_block_and_delete:
  CYC(b_+170, b_+173); push_effect(gb, b_+173); somaria_remove_block(gb, sp0_);

delete_with_puff:
  CYC(b_+173, b_+174); H = D;
  CYC(b_+174, b_+176); L = 0x2f;
  CYC(b_+176, b_+178); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(b_+178, objectCreatePuff_hook, SYM(objectCreatePuff), b_+181);
  else CYC(b_+178, b_+181);

delete_self:
  CYC(b_+181, b_+184); itemDelete_hook(gb);
  return;

state2:
  CYC(b_+184, b_+186); E = 0x05;
  CYC(b_+186, b_+187); A = mem_rd(gb, DE);
  CYC(b_+187, b_+188); push_effect(gb, b_+188);
  do { uint16_t jt_ = (somaria_jump_table(gb));
    if (jt_ == b_+196) { break; }
    else if (jt_ == b_+210) { goto state2_substate1; }
    else if (jt_ == b_+219) { goto state2_substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+196, itemIncSubstate_hook, SYM(itemIncSubstate), b_+199);
  CYC(b_+199, b_+202); push_effect(gb, b_+202); somaria_remove_block(gb, sp0_);
  CALL_C(b_+202, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+205);
  CYC(b_+205, b_+207); A = 0x02;
  CYC(b_+207, b_+210); itemSetAnimation_hook(gb);
  return;

state2_substate1:
  CALL_C(b_+210, itemUpdateDamageToApply_hook, SYM(itemUpdateDamageToApply), b_+213);
  if (!(F & FC)) {
    CYCT(b_+213, b_+214); ret_effect(gb); return;
  }
  CYC(b_+213, b_+214);
  CALL_C(b_+214, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+217);
  CYC(b_+217, b_+219);
  goto delete_with_puff;

state2_substate2:
  CALL_C(b_+219, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+222);
  if (!(F & FC)) {
    CYCT(b_+222, b_+224);
    goto delete_self;
  }
  CYC(b_+222, b_+224);
  CALL_C(b_+224, bombUpdateThrowingLaterally_hook, SYM(bombUpdateThrowingLaterally), b_+227);
  CYC(b_+227, b_+230); push_effect(gb, b_+230); somaria_check_deletion_trigger(gb);
  if (!(F & FZ)) {
    CYCT(b_+230, b_+232);
    goto delete_with_puff;
  }
  CYC(b_+230, b_+232);
  CYC(b_+232, b_+234); L = 0x39;
  CYC(b_+234, b_+235); C = mem_rd(gb, HL);
  CALL_C(b_+235, itemUpdateThrowingVerticallyAndCheckHazards_hook, SYM(itemUpdateThrowingVerticallyAndCheckHazards), b_+238);
  if (F & FC) {
    CYCT(b_+238, b_+240);
    goto delete_self;
  }
  CYC(b_+238, b_+240);
  if (F & FZ) {
    CYCT(b_+240, b_+241); ret_effect(gb); return;
  }
  CYC(b_+240, b_+241);
  CYC(b_+241, b_+243);
  goto delete_with_puff;

state3:
  CYC(b_+243, b_+246); push_effect(gb, b_+246); somaria_check_block_in_place(gb);
  if (!(F & FZ)) {
    CYCT(b_+246, b_+248);
    goto delete_with_puff;
  }
  CYC(b_+246, b_+248);
  CALL_C(b_+248, itemUpdateDamageToApply_hook, SYM(itemUpdateDamageToApply), b_+251);
  if (F & FC) {
    CYCT(b_+251, b_+253);
    goto remove_block_and_delete;
  }
  CYC(b_+251, b_+253);
  CYC(b_+253, b_+256); push_effect(gb, b_+256); somaria_check_deletion_trigger(gb);
  if (!(F & FZ)) {
    CYCT(b_+256, b_+258);
    goto remove_block_and_delete;
  }
  CYC(b_+256, b_+258);
  CYC(b_+258, b_+261); A = W8(wActiveTilePos);
  CYC(b_+261, b_+263); L = 0x32;
  CYC(b_+263, b_+264); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+264, b_+266);
    goto remove_block_and_delete;
  }
  CYC(b_+264, b_+266);
  CYC(b_+266, b_+269); A = W8(wTilesetFlags);
  CYC(b_+269, b_+271); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+271, b_+273);
  } else {
    CYC(b_+271, b_+273);
    CYC(b_+273, b_+274); A = mem_rd(gb, HL);
    CYC(b_+274, b_+276); alu_add(gb, 0x10);
    CYC(b_+276, b_+277); C = A;
    CYC(b_+277, b_+279); B = (uint8_t)(wRoomCollisions >> 8);
    CYC(b_+279, b_+280); A = mem_rd(gb, BC);
    CYC(b_+280, b_+282); alu_cp(gb, 0x0f);
    if (!(F & FZ)) {
      CYCT(b_+282, b_+284);
      goto remove_block_and_delete;
    }
    CYC(b_+282, b_+284);
  }
  CYC(b_+284, b_+286); L = 0x2f;
  CYC(b_+286, b_+288); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+288, b_+291); TAIL(objectAddToGrabbableObjectBuffer);
  }
  CYC(b_+288, b_+291);
  CYC(b_+291, b_+293); A = 0x04;
  CYC(b_+293, b_+296); itemSetState_hook(gb);
}
