#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(initializeParentItem), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(initializeParentItem), (from), (to), true)

void clearLinkUsingItem1_hook(GB *gb);
void itemEnableLinkMovement_hook(GB *gb);
void itemEnableLinkTurning_hook(GB *gb);
void itemIndexToBit_hook(GB *gb);

static void parent_item_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void parent_item_jump_table_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
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
}

void initializeParentItem_hook(GB *gb) {
  BASE(initializeParentItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); alu_and(gb, 0xf0);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); L = 0x00;
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, E);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, D);
  CYC(b_+11, SYM(chooseParentItemSlot)); ret_effect(gb);
}

void checkShopInput_hook(GB *gb) {
  BASE(checkShopInput);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkGrabState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = W8(wGameKeysJustPressed);
  CYC(b_+8, b_+10); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, checkGrabbableObjects_hook, SYM(checkGrabbableObjects), b_+14);
  if (!(F & FC)) {
    CYCT(b_+14, b_+15); ret_effect(gb); return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x83;
  CYC(b_+17, b_+20); W8(wLinkGrabState) = A;
  CYC(b_+20, SYM(parentItemUpdate)); ret_effect(gb);
}

void clearParentItem_hook(GB *gb) {
  BASE(clearParentItem);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, clearLinkUsingItem1_hook, SYM(clearLinkUsingItem1), b_+3);
  CALL_C(b_+3, itemEnableLinkTurning_hook, SYM(itemEnableLinkTurning), b_+6);
  CALL_C(b_+6, itemEnableLinkMovement_hook, SYM(itemEnableLinkMovement), b_+9);
  CYC(b_+9, b_+11); E = 0x00;
  CYC(b_+11, SYM(clearParentItemH)); objectDelete_de_hook(gb);
}

void clearParentItemH_hook(GB *gb) {
  BASE(clearParentItemH);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+2); D = H;
  CALL_C(b_+2, clearParentItem_hook, SYM(clearParentItem), b_+5);
  CYC(b_+5, b_+6); SET_DE(pop_effect(gb));
  CYC(b_+6, SYM(parentItemCode_shield)); ret_effect(gb);
}

void clearAllParentItems_body_hook(GB *gb) {
  BASE(clearAllParentItems_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); D = 0xd2;
  do {
    CALL_C(b_+3, clearParentItem_hook, SYM(clearParentItem), b_+6);
    CYC(b_+6, b_+7); D = alu_inc8(gb, D);
    CYC(b_+7, b_+8); A = D;
    CYC(b_+8, b_+10); alu_cp(gb, 0xd6);
    if (F & FC) {
      CYCT(b_+10, b_+12);
    } else {
      CYC(b_+10, b_+12);
      break;
    }
  } while (1);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); W8(wUsingShield) = A;
  CYC(b_+16, b_+19); W8(wcc63) = A;
  CYC(b_+19, b_+22); W8(wMagnetGloveState) = A;
  CYC(b_+22, b_+23); SET_DE(pop_effect(gb));
  CYC(b_+23, SYM(updateParentItemButtonAssignment_body)); ret_effect(gb);
}

void updateParentItemButtonAssignment_body_hook(GB *gb) {
  BASE(updateParentItemButtonAssignment_body);
  CYC(b_+0, b_+2); H = 0xd2;
  do {
    CYC(b_+2, b_+4); L = 0x00;
    CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+5, b_+6); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+6, b_+8);
      CYC(b_+24, b_+25); alu_xor(gb, A);
    } else {
      CYC(b_+6, b_+8);
      CYC(b_+8, b_+11); A = W8(wInventoryB);
      CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
      CYC(b_+12, b_+14); A = 0x02;
      if (F & FZ) {
        CYCT(b_+14, b_+16);
      } else {
        CYC(b_+14, b_+16);
        CYC(b_+16, b_+19); A = W8(wInventoryA);
        CYC(b_+19, b_+20); alu_cp(gb, mem_rd(gb, HL));
        CYC(b_+20, b_+22); A = 0x01;
        if (F & FZ) {
          CYCT(b_+22, b_+24);
        } else {
          CYC(b_+22, b_+24);
          CYC(b_+24, b_+25); alu_xor(gb, A);
        }
      }
    }
    CYC(b_+25, b_+27); L = 0x03;
    CYC(b_+27, b_+28); mem_wr(gb, HL, A);
    CYC(b_+28, b_+29); H = alu_inc8(gb, H);
    CYC(b_+29, b_+30); A = H;
    CYC(b_+30, b_+32); alu_cp(gb, 0xd6);
    if (F & FC) {
      CYCT(b_+32, b_+34);
    } else {
      CYC(b_+32, b_+34);
      break;
    }
  } while (1);
  CYC(b_+34, SYM(checkUseItems_b06)); ret_effect(gb);
}

void chooseParentItemSlot_hook(GB *gb) {
  BASE(chooseParentItemSlot);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); alu_and(gb, 0x0f);
  CYC(b_+3, b_+4); parent_item_jump_table_from_rst(gb, b_+4);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+16) { goto thing4; }
    else if (jt_ == b_+21) { goto thing2; }
    else if (jt_ == b_+32) { goto thing1; }
    else if (jt_ == b_+42) { goto thing3; }
    else if (jt_ == b_+70) { goto thing5; }
    else if (jt_ == b_+76) { goto thing0; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

thing4:
  CYC(b_+16, b_+19); A = mem_rd(gb, w1ParentItem2_enabled);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+21); ret_effect(gb); return;
  }
  CYC(b_+20, b_+21);

thing2:
  CYC(b_+21, b_+24); SET_HL(w1ParentItem3_id);
  CYC(b_+24, b_+25); A = E;
  CYC(b_+25, b_+26); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+26, b_+28); goto thing0;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); H = alu_inc8(gb, H);
  CYC(b_+29, b_+30); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+30, b_+32); goto thing0;
  }
  CYC(b_+30, b_+32);

thing1:
  CYC(b_+32, b_+35); SET_HL(w1ParentItem3_enabled);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+37, b_+38); ret_effect(gb); return;
  }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+39); H = alu_inc8(gb, H);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); alu_or(gb, A);
  CYC(b_+41, b_+42); ret_effect(gb); return;

thing3:
  CYC(b_+42, b_+45); SET_HL(w1ParentItem2_enabled);
  CYC(b_+45, b_+46); A = C;
  CYC(b_+46, b_+48); alu_and(gb, 0xf0);
  CYC(b_+48, b_+49); A = alu_inc8(gb, A);
  CYC(b_+49, b_+50); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+50, b_+52); goto thing0;
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); push_effect(gb, DE);
  CYC(b_+53, b_+54); push_effect(gb, BC);
  CALL_C(b_+54, clearParentItemH_hook, SYM(clearParentItemH), b_+57);
  CYC(b_+57, b_+58); SET_BC(pop_effect(gb));
  CYC(b_+58, b_+59); SET_DE(pop_effect(gb));
  CYC(b_+59, b_+62); SET_HL(w1ParentItem2);
  CYC(b_+62, b_+63); alu_xor(gb, A);
  CYC(b_+63, b_+66); W8(wMagnetGloveState) = A;
  CYC(b_+66, b_+69); W8(wcc63) = A;
  CYC(b_+69, b_+70); ret_effect(gb); return;

thing5:
  CYC(b_+70, b_+73); SET_HL(w1ParentItem5_enabled);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+75); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+75, b_+76); ret_effect(gb); return;
  }
  CYC(b_+75, b_+76);

thing0:
  CYC(b_+76, b_+77); alu_or(gb, H);
  CYC(b_+77, SYM(checkShopInput)); ret_effect(gb);
}

void parentItemUpdate_hook(GB *gb) {
  BASE(parentItemUpdate);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = E;
  CYC(b_+1, b_+3); H8(hActiveObjectType) = A;
  CYC(b_+3, b_+4); A = D;
  CYC(b_+4, b_+6); H8(hActiveObject) = A;
  CALL_C(b_+6, itemIndexToBit_hook, SYM(itemIndexToBit), b_+9);
  CYC(b_+9, b_+12); SET_HL(wcc95);
  CYC(b_+12, b_+13); alu_cpl(gb);
  CYC(b_+13, b_+14); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); E = 0x01;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); parent_item_jump_table_from_rst(gb, b_+19);
  hook_continue(gb, HL, sp0_);
}

void checkItemUsed_hook(GB *gb) {
  BASE(checkItemUsed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); H = 0xc6;
  CYC(b_+2, b_+3); L = E;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7); goto check_item;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wActiveRing);
  CYC(b_+10, b_+12); alu_cp(gb, 0x0b);
  if (F & FZ) {
    CYCT(b_+12, b_+14); goto punch;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 0x3d);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);

punch:
  CYC(b_+17, b_+19); L = 0x88;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+21, b_+22); ret_effect(gb); return;
  }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); A = 0x02;

check_item:
  CYC(b_+24, b_+26); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(b_+26, b_+27); ret_effect(gb); return;
  }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); E = A;
  CYC(b_+28, b_+31); SET_HL(SYM(itemUsageParameterTable));
  CYC(b_+31, b_+32); parent_item_add_double_index_from_rst(gb, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+33, b_+34); C = A;
  CYC(b_+34, b_+35); L = mem_rd(gb, HL);
  CYC(b_+35, b_+37); H = 0xcc;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); alu_and(gb, D);
  if (F & FZ) {
    CYCT(b_+39, b_+40); ret_effect(gb); return;
  }
  CYC(b_+39, b_+40);
  CALL_C(b_+40, chooseParentItemSlot_hook, SYM(chooseParentItemSlot), b_+43);
  if (!(F & FZ)) {
    CYCT(b_+43, SYM(initializeParentItem)); ret_effect(gb); return;
  }
  CYC(b_+43, SYM(initializeParentItem));
  initializeParentItem_hook(gb);
}

void checkUseItems_b06_hook(GB *gb) {
  BASE(checkUseItems_b06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wUsingShield) = A;
  CYC(b_+4, b_+7); SET_HL(wSwordDisabledCounter);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
  } else {
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+12); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+12, b_+15); SET_HL(wLinkUsingItem1);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_and(gb, 0x0f);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+22); A = W8(wcc63);
  CYC(b_+22, b_+23); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+23, b_+25); goto items_disabled;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); A = W8(wInShop);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+32); checkShopInput_hook(gb); return;
  }
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+35); A = W8(wcc95);
  CYC(b_+35, b_+36); B = A;
  CYC(b_+36, b_+39); A = W8(wLinkInAir);
  CYC(b_+39, b_+40); alu_or(gb, B);
  CYC(b_+40, b_+41); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+41, b_+43); goto update_parent_items;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); A = W8(wccd8);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+50); A = W8(wLinkGrabState);
  CYC(b_+50, b_+51); alu_or(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+51, b_+53); goto update_parent_items;
  }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+56); A = W8(wLinkClimbingVine);
  CYC(b_+56, b_+57); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+57, b_+59); goto update_parent_items;
  }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+62); A = W8(wTilesetFlags);
  CYC(b_+62, b_+64); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+64, b_+66); goto sidescroll;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(b_+68, b_+70); goto normal;
  }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+73); SET_DE((SYM(addDecimalToHlRef) + 8));
  CALL_C(b_+73, checkItemUsed_hook, SYM(checkItemUsed), b_+76);
  CYC(b_+76, b_+78); goto update_parent_items;

normal:
  CYC(b_+78, b_+81); A = W8(wLinkSwimmingState);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+82, b_+84); goto check_ab;
  }
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+86); goto update_parent_items;

sidescroll:
  CYC(b_+86, b_+89); A = W8(wLinkSwimmingState);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+90, b_+92); goto check_ab;
  }
  CYC(b_+90, b_+92);
  CYC(b_+92, b_+95); SET_HL(w1Link_var2f);
  CYC(b_+95, b_+97); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+97, b_+99); goto check_b;
  }
  CYC(b_+97, b_+99);

check_ab:
  CYC(b_+99, b_+102); SET_DE((SYM(addDecimalToHlRef) + 8));
  CALL_C(b_+102, checkItemUsed_hook, SYM(checkItemUsed), b_+105);

check_b:
  CYC(b_+105, b_+108); SET_DE((SYM(pollInput) + 27));
  CALL_C(b_+108, checkItemUsed_hook, SYM(checkItemUsed), b_+111);

update_parent_items:
  CYC(b_+111, b_+114); SET_DE(w1ParentItem2);
  do {
    CYC(b_+114, b_+116); E = 0x00;
    CYC(b_+116, b_+117); A = mem_rd(gb, DE);
    CYC(b_+117, b_+118); alu_or(gb, A);
    if (!(F & FZ)) {
      CALL_C_CC(b_+118, parentItemUpdate_hook, SYM(parentItemUpdate), b_+121);
    } else {
      CYC(b_+118, b_+121);
    }
    CYC(b_+121, b_+122); D = alu_inc8(gb, D);
    CYC(b_+122, b_+123); A = D;
    CYC(b_+123, b_+125); alu_cp(gb, 0xd6);
    if (F & FC) {
      CYCT(b_+125, b_+127);
    } else {
      CYC(b_+125, b_+127);
      break;
    }
  } while (1);
  CYC(b_+127, b_+128); alu_xor(gb, A);
  CYC(b_+128, b_+130); H8(hActiveObjectType) = A;
  CYC(b_+130, b_+132); D = 0xd0;
  CYC(b_+132, b_+133); A = D;
  CYC(b_+133, b_+135); H8(hActiveObject) = A;
  CYC(b_+135, b_+136); ret_effect(gb); return;

items_disabled:
  CYC(b_+136, b_+138); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(b_+138, b_+140); goto update_parent_items;
  }
  CYC(b_+138, b_+140);
  CALL_C(b_+140, clearAllParentItems_body_hook, SYM(clearAllParentItems_body), b_+143);
  CYC(b_+143, b_+146); SET_HL(w1ParentItem2);
  CYC(b_+146, b_+149); SET_DE(0xff05);
  CYC(b_+149, b_+151); C = 0xf1;
  CALL_C(b_+151, initializeParentItem_hook, SYM(initializeParentItem), b_+154);
  CYC(b_+154, b_+156); A = 0x80;
  CYC(b_+156, b_+159); W8(wcc63) = A;
  CYC(b_+159, SYM(checkItemUsed)); goto update_parent_items;
}

void functionCaller_b06_hook(GB *gb) {
  BASE(functionCaller_b06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+2); parent_item_jump_table_from_rst(gb, b_+2);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(clearAllParentItems_body)) { clearAllParentItems_body_hook(gb); return; }
    else if (jt_ == SYM(updateParentItemButtonAssignment_body)) { updateParentItemButtonAssignment_body_hook(gb); return; }
    else if (jt_ == SYM(checkUseItems_b06)) { checkUseItems_b06_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
