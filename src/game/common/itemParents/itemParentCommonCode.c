#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void itemIndexToBit_hook(GB *gb);
void andHlWithGameKeysPressed_hook(GB *gb);
void itemCreateChildAndDeleteOnFailure_hook(GB *gb);
void itemCreateChild_hook(GB *gb);
void itemCreateChildWithID_hook(GB *gb);
void getFreeItemSlotWithObjectCap_hook(GB *gb);
void isLinkUnderwater_hook(GB *gb);
void checkNoOtherParentItemsInUse_hook(GB *gb);
void parentItemCode_magnetGloves_hook(GB *gb);

static void item_parent_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void check_no_other_parent_items_in_use(GB *gb, bool advance_first) {
  BASE(checkNoOtherParentItemsInUse);
  if (advance_first) goto next_item;

item_loop:
  CYC(b_+3, b_+4); A = D;
  CYC(b_+4, b_+5); alu_cp(gb, H);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto next_item;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);

next_item:
  CYC(b_+10, b_+11); H = alu_inc8(gb, H);
  CYC(b_+11, b_+12); A = H;
  CYC(b_+12, b_+14); alu_cp(gb, 0xd6);
  if (F & FC) {
    CYCT(b_+14, b_+16);
    goto item_loop;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+18); ret_effect(gb);
}

void parentItemCode_magnetGloves_hook(GB *gb) {
  BASE(parentItemCode_magnetGloves);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkNoOtherParentItemsInUse_hook, SYM(checkNoOtherParentItemsInUse), b_+3);

item_loop:
  CYC(b_+3, b_+4); push_effect(gb, HL);
  if (!(F & FZ)) {
    CALL_C_CC(b_+4, clearParentItemH_hook, SYM(clearParentItemH), b_+7);
  } else {
    CYC(b_+4, b_+7);
  }
  CYC(b_+7, b_+8); SET_HL(pop_effect(gb));
  CYC(b_+8, b_+11); push_effect(gb, b_+11);
  check_no_other_parent_items_in_use(gb, true);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    goto item_loop;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void checkNoOtherParentItemsInUse_hook(GB *gb) {
  BASE(checkNoOtherParentItemsInUse);
  CYC(b_+0, b_+3); SET_HL(w1ParentItem2_enabled);
  check_no_other_parent_items_in_use(gb, false);
}

void parentItemLoadAnimationAndIncState_hook(GB *gb) {
  BASE(parentItemLoadAnimationAndIncState);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemDisableLinkMovement_hook, SYM(itemDisableLinkMovement), b_+3);
  CALL_C(b_+3, itemDisableLinkTurning_hook, SYM(itemDisableLinkTurning), b_+6);
  CYC(b_+6, b_+8); E = 0x04;
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = 0x01;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+17); SET_HL(SYM(linkItemAnimationTable));
  CYC(b_+17, b_+18); item_parent_add_double_index_from_rst(gb, b_+18);
  CYC(b_+18, b_+20); E = 0x18;
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_and(gb, 0x0f);
  CYC(b_+26, b_+28); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+28, b_+30);
  } else {
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+32); alu_or(gb, 0xd0);
  }
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+35); B = A;
  CYC(b_+35, b_+37); A = alu_swap(gb, A);
  CYC(b_+37, b_+39); alu_and(gb, 0x07);
  CYC(b_+39, b_+41); E = 0x3f;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); C = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CALL_C_CC(b_+45, setLinkUsingItem1_hook, SYM(setLinkUsingItem1), b_+48);
  } else {
    CYC(b_+45, b_+48);
  }
  CYC(b_+48, b_+51); A = W8(w1Companion_id);
  CYC(b_+51, b_+53); alu_cp(gb, 0x13);
  CYC(b_+53, b_+54); A = C;
  if (F & FZ) {
    CYCT(b_+54, b_+56);
    goto set_animation;
  }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); A = W8(w1Link_var2f);
  CYC(b_+59, b_+61); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+61, b_+63);
    goto not_underwater;
  }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); A = C;
  CYC(b_+64, b_+66); alu_cp(gb, 0x22);
  if (!(F & FZ)) {
    CYCT(b_+66, b_+68);
    goto set_animation;
  }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); A = 0x2d;
  CYC(b_+70, b_+72);
  goto set_animation;

not_underwater:
  CYC(b_+72, b_+75); A = W8(wLinkObjectIndex);
  CYC(b_+75, b_+76); alu_rrca(gb);
  CYC(b_+76, b_+77); A = C;
  if (!(F & FC)) {
    CYCT(b_+77, b_+79);
    goto set_animation;
  }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+81); alu_cp(gb, 0x20);
  if (F & FC) {
    CYCT(b_+81, b_+83);
    goto set_animation;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); alu_cp(gb, 0x24);
  if (!(F & FC)) {
    CYCT(b_+85, b_+87);
    goto set_animation;
  }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); alu_add(gb, 0x04);

set_animation:
  CYC(b_+89, b_+92); TAIL(specialObjectSetAnimationWithLinkData);
}

void itemCreateChildIfDoesntExistAlready_hook(GB *gb) {
  BASE(itemCreateChildIfDoesntExistAlready);
  CYC(b_+0, b_+2); E = 0x01;
  TAIL(itemCreateChildAndDeleteOnFailure);
}

void itemCreateChildAndDeleteOnFailure_hook(GB *gb) {
  BASE(itemCreateChildAndDeleteOnFailure);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemCreateChild_hook, SYM(itemCreateChild), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); TAIL(clearParentItem);
}

void itemCreateChild_hook(GB *gb) {
  BASE(itemCreateChild);
  CYC(b_+0, b_+2); C = 0x00;
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0x01;
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  TAIL(itemCreateChildWithID);
}

void itemCreateChildWithID_hook(GB *gb) {
  BASE(itemCreateChildWithID);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x19;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); L = mem_rd(gb, HL);
  CYC(b_+5, b_+6); H = A;
  CYC(b_+6, b_+8); alu_cp(gb, 0x01);
  CYC(b_+8, b_+9); alu_scf(gb);
  if (F & FZ) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0xd0);
  if (F & FZ) {
    CALL_C_CC(b_+12, getFreeItemSlotWithObjectCap_hook, SYM(getFreeItemSlotWithObjectCap), b_+15);
  } else {
    CYC(b_+12, b_+15);
  }
  if (F & FC) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); A = C;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); push_effect(gb, DE);
  CYC(b_+27, b_+30); SET_DE(w1Link_direction);
  CYC(b_+30, b_+32); L = 0x08;
  CYC(b_+32, b_+34); B = 0x08;
  CALL_C(b_+34, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+37);
  CYC(b_+37, b_+38); SET_DE(pop_effect(gb));
  CYC(b_+38, b_+40); L = 0x16;
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+42, b_+43); mem_wr(gb, HL, D);
  CYC(b_+43, b_+45); E = 0x18;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); E = alu_inc8(gb, E);
  CYC(b_+47, b_+48); A = H;
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); alu_xor(gb, A);
  CYC(b_+50, b_+51); ret_effect(gb);
}

void getFreeItemSlotWithObjectCap_hook(GB *gb) {
  BASE(getFreeItemSlotWithObjectCap);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL((w4Filler5 + 1));

item_loop:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7); goto next_item;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+9, b_+10); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12); goto next_item;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); E = alu_dec8(gb, E);
  if (F & FZ) {
    CYCT(b_+13, b_+15); goto failure;
  }
  CYC(b_+13, b_+15);

next_item:
  CYC(b_+15, b_+16); H = alu_inc8(gb, H);
  CYC(b_+16, b_+17); A = H;
  CYC(b_+17, b_+19); alu_cp(gb, 0xdc);
  if (F & FC) {
    CYCT(b_+19, b_+21); goto item_loop;
  }
  CYC(b_+19, b_+21);
  CALL_C(b_+21, getFreeItemSlot_hook, SYM(getFreeItemSlot), b_+24);
  if (F & FZ) {
    CYCT(b_+24, b_+25); ret_effect(gb); return;
  }
  CYC(b_+24, b_+25);

failure:
  CYC(b_+25, b_+26); alu_scf(gb);
  CYC(b_+26, b_+27); ret_effect(gb);
}

void getNumFreeItemSlots_hook(GB *gb) {
  BASE(getNumFreeItemSlots);
  CYC(b_+0, b_+3); SET_HL(w4Filler5);
  CYC(b_+3, b_+5); B = 0x00;
  do {
    CYC(b_+5, b_+6); A = mem_rd(gb, HL);
    CYC(b_+6, b_+7); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+7, b_+9);
    } else {
      CYC(b_+7, b_+9);
      CYC(b_+9, b_+10); B = alu_inc8(gb, B);
    }
    CYC(b_+10, b_+11); H = alu_inc8(gb, H);
    CYC(b_+11, b_+12); A = H;
    CYC(b_+12, b_+14); alu_cp(gb, 0xdc);
    if (F & FC) CYCT(b_+14, b_+16); else CYC(b_+14, b_+16);
  } while (F & FC);
  CYC(b_+16, b_+17); A = B;
  CYC(b_+17, b_+18); alu_or(gb, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void setLinkUsingItem1_hook(GB *gb) {
  BASE(setLinkUsingItem1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+9); SET_HL(wLinkUsingItem1);
  CYC(b_+9, b_+10); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void clearLinkUsingItem1_hook(GB *gb) {
  BASE(clearLinkUsingItem1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+7); alu_cpl(gb);
  CYC(b_+7, b_+10); SET_HL(wLinkUsingItem1);
  CYC(b_+10, b_+11); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void itemDisableLinkMovement_hook(GB *gb) {
  BASE(itemDisableLinkMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+6); SET_HL(wLinkImmobilized);
  CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void itemEnableLinkMovement_hook(GB *gb) {
  BASE(itemEnableLinkMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+6); SET_HL(wLinkImmobilized);
  CYC(b_+6, b_+7); alu_cpl(gb);
  CYC(b_+7, b_+8); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void itemDisableLinkTurning_hook(GB *gb) {
  BASE(itemDisableLinkTurning);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+6); SET_HL(wLinkTurningDisabled);
  CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void itemEnableLinkTurning_hook(GB *gb) {
  BASE(itemEnableLinkTurning);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+6); SET_HL(wLinkTurningDisabled);
  CYC(b_+6, b_+7); alu_cpl(gb);
  CYC(b_+7, b_+8); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void setCc95Bit_hook(GB *gb) {
  BASE(setCc95Bit);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemIndexToBit_hook, SYM(itemIndexToBit), b_+3);
  CYC(b_+3, b_+6); SET_HL(wcc95);
  CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void itemIndexToBit_hook(GB *gb) {
  BASE(itemIndexToBit);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = D;
  CYC(b_+1, b_+3); alu_sub(gb, 0xd2);
  CYC(b_+3, b_+6); SET_HL(0x00f8);
  CYC(b_+6, b_+7); alu_add(gb, L);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void parentItemCheckButtonPressed_hook(GB *gb) {
  BASE(parentItemCheckButtonPressed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x03;
  TAIL(andHlWithGameKeysPressed);
}

void andHlWithGameKeysPressed_hook(GB *gb) {
  BASE(andHlWithGameKeysPressed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wGameKeysPressed);
  CYC(b_+3, b_+4); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void clearParentItemIfCantUseSword_hook(GB *gb) {
  BASE(clearParentItemIfCantUseSword);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wcc95);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+4, b_+6);
    goto cant_use_sword;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wLinkClimbingVine);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    goto cant_use_sword;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = W8(wccd8);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+19); A = W8(wSwordDisabledCounter);
  CYC(b_+19, b_+20); alu_or(gb, B);
  if (F & FZ) {
    CYCT(b_+20, b_+21); ret_effect(gb); return;
  }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); E = 0x04;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_or(gb, A);
  CYC(b_+25, b_+27); A = 0x5a;
  if (F & FZ) {
    CALL_C_CC(b_+27, playSound_b00_hook, SYM(playSound_b00), b_+30);
  } else {
    CYC(b_+27, b_+30);
  }

cant_use_sword:
  CYC(b_+30, b_+31); SET_AF(pop_effect(gb));
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+35); W8(wcc63) = A;
  CYC(b_+35, b_+38); TAIL(clearParentItem);
}

void checkLinkOnGround_hook(GB *gb) {
  BASE(checkLinkOnGround);
  CYC(b_+O(0), b_+OE(3)); A = W8(wLinkObjectIndex);
  CYC(b_+O(3), b_+OE(5)); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+O(5), b_+OE(6)); ret_effect(gb); return;
  }
  CYC(b_+O(5), b_+OE(6));
  CYC(b_+O(6), b_+OE(9)); SET_HL(wLinkInAir);
  CYC(b_+O(9), b_+OE(10)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(10), b_+OE(11)); alu_or(gb, mem_rd(gb, HL));
  if (game_seasons) { CYC(b_+S(11), b_+S(12)); ret_effect(gb); return; }
  if (!game_seasons) {
    if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
    CYC(b_+11, b_+12);
    CYC(b_+12, b_+14); TAIL(isLinkUnderwater);
  }
}

void isLinkUnderwater_hook(GB *gb) {
  BASE(isLinkUnderwater);
  CYC(b_+0, b_+3); A = W8(w1Link_var2f);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void isLinkInHole_hook(GB *gb) {
  BASE(isLinkInHole);
  CYC(b_+0, b_+3); A = W8(wActiveTileType);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+6); alu_cp(gb, 0x02);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void updateGrabbedObjectPosition_hook(GB *gb) {
  BASE(updateGrabbedObjectPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkGrabState2);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(w1Link);
  CYC(b_+9, b_+10); A = E;
  CYC(b_+10, b_+12); H8(hActiveObjectType) = A;
  CYC(b_+12, b_+13); A = D;
  CYC(b_+13, b_+15); H8(hActiveObject) = A;
  CYC(b_+15, b_+18); A = W8(wLinkGrabState);
  CYC(b_+18, b_+20); alu_cp(gb, 0x83);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
  } else {
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+24); E = 0x21;
    CYC(b_+24, b_+25); A = mem_rd(gb, DE);
    CYC(b_+25, b_+27); alu_and(gb, 0x0f);
    CYC(b_+27, b_+28); alu_add(gb, B);
    CYC(b_+28, b_+29); B = A;
  }
  CYC(b_+29, b_+31); E = 0x08;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_add(gb, B);
  CYC(b_+33, b_+36); SET_HL(b_+63);
  CYC(b_+36, b_+37); item_parent_add_double_index_from_rst(gb, b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+39); B = A;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+40, b_+41); C = A;
  CYC(b_+41, b_+43); A = 0x0b;
  CALL_C(b_+43, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+46);
  CYC(b_+46, b_+48); E = 0x0b;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+51); L = alu_inc8(gb, L);
  CYC(b_+51, b_+53); E = 0x0d;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); alu_add(gb, C);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+59); E = 0x0f;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_add(gb, B);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+62, b_+63); ret_effect(gb);
}
