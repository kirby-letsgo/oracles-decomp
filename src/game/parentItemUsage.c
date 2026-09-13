#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4980, 0x4981); A = C;
  CYC(0x4981, 0x4983); alu_and(gb, 0xf0);
  CYC(0x4983, 0x4984); A = alu_inc8(gb, A);
  CYC(0x4984, 0x4986); L = 0x00;
  CYC(0x4986, 0x4987); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4987, 0x4988); mem_wr(gb, HL, E);
  CYC(0x4988, 0x4989); L = alu_inc8(gb, L);
  CYC(0x4989, 0x498a); L = alu_inc8(gb, L);
  CYC(0x498a, 0x498b); mem_wr(gb, HL, D);
  CYC(0x498b, 0x498c); ret_effect(gb);
}

void checkShopInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49da, 0x49dd); A = W8(wLinkGrabState);
  CYC(0x49dd, 0x49de); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x49de, 0x49df); ret_effect(gb); return;
  }
  CYC(0x49de, 0x49df);
  CYC(0x49df, 0x49e2); A = W8(wGameKeysJustPressed);
  CYC(0x49e2, 0x49e4); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(0x49e4, 0x49e5); ret_effect(gb); return;
  }
  CYC(0x49e4, 0x49e5);
  CALL_C(0x49e5, checkGrabbableObjects_hook, 0x1c97, 0x49e8);
  if (!(F & FC)) {
    CYCT(0x49e8, 0x49e9); ret_effect(gb); return;
  }
  CYC(0x49e8, 0x49e9);
  CYC(0x49e9, 0x49eb); A = 0x83;
  CYC(0x49eb, 0x49ee); W8(wLinkGrabState) = A;
  CYC(0x49ee, 0x49ef); ret_effect(gb);
}

void clearParentItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a42, clearLinkUsingItem1, 0x5450, 0x4a45);
  CALL_C(0x4a45, itemEnableLinkTurning, 0x5479, 0x4a48);
  CALL_C(0x4a48, itemEnableLinkMovement, 0x5466, 0x4a4b);
  CYC(0x4a4b, 0x4a4d); E = 0x00;
  CYC(0x4a4d, 0x4a50); objectDelete_de_hook(gb);
}

void clearParentItemH_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a50, 0x4a51); push_effect(gb, DE);
  CYC(0x4a51, 0x4a52); D = H;
  CALL_C(0x4a52, clearParentItem_hook, 0x4a42, 0x4a55);
  CYC(0x4a55, 0x4a56); SET_DE(pop_effect(gb));
  CYC(0x4a56, 0x4a57); ret_effect(gb);
}

void clearAllParentItems_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4878, 0x4879); push_effect(gb, DE);
  CYC(0x4879, 0x487b); D = 0xd2;
  do {
    CALL_C(0x487b, clearParentItem_hook, 0x4a42, 0x487e);
    CYC(0x487e, 0x487f); D = alu_inc8(gb, D);
    CYC(0x487f, 0x4880); A = D;
    CYC(0x4880, 0x4882); alu_cp(gb, 0xd6);
    if (F & FC) {
      CYCT(0x4882, 0x4884);
    } else {
      CYC(0x4882, 0x4884);
      break;
    }
  } while (1);
  CYC(0x4884, 0x4885); alu_xor(gb, A);
  CYC(0x4885, 0x4888); W8(wUsingShield) = A;
  CYC(0x4888, 0x488b); W8(wcc63) = A;
  CYC(0x488b, 0x488e); W8(wMagnetGloveState) = A;
  CYC(0x488e, 0x488f); SET_DE(pop_effect(gb));
  CYC(0x488f, 0x4890); ret_effect(gb);
}

void updateParentItemButtonAssignment_body_hook(GB *gb) {
  CYC(0x4890, 0x4892); H = 0xd2;
  do {
    CYC(0x4892, 0x4894); L = 0x00;
    CYC(0x4894, 0x4895); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4895, 0x4896); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x4896, 0x4898);
      CYC(0x48a8, 0x48a9); alu_xor(gb, A);
    } else {
      CYC(0x4896, 0x4898);
      CYC(0x4898, 0x489b); A = W8(wInventoryB);
      CYC(0x489b, 0x489c); alu_cp(gb, mem_rd(gb, HL));
      CYC(0x489c, 0x489e); A = 0x02;
      if (F & FZ) {
        CYCT(0x489e, 0x48a0);
      } else {
        CYC(0x489e, 0x48a0);
        CYC(0x48a0, 0x48a3); A = W8(wInventoryA);
        CYC(0x48a3, 0x48a4); alu_cp(gb, mem_rd(gb, HL));
        CYC(0x48a4, 0x48a6); A = 0x01;
        if (F & FZ) {
          CYCT(0x48a6, 0x48a8);
        } else {
          CYC(0x48a6, 0x48a8);
          CYC(0x48a8, 0x48a9); alu_xor(gb, A);
        }
      }
    }
    CYC(0x48a9, 0x48ab); L = 0x03;
    CYC(0x48ab, 0x48ac); mem_wr(gb, HL, A);
    CYC(0x48ac, 0x48ad); H = alu_inc8(gb, H);
    CYC(0x48ad, 0x48ae); A = H;
    CYC(0x48ae, 0x48b0); alu_cp(gb, 0xd6);
    if (F & FC) {
      CYCT(0x48b0, 0x48b2);
    } else {
      CYC(0x48b0, 0x48b2);
      break;
    }
  } while (1);
  CYC(0x48b2, 0x48b3); ret_effect(gb);
}

void chooseParentItemSlot_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x498c, 0x498d); A = C;
  CYC(0x498d, 0x498f); alu_and(gb, 0x0f);
  CYC(0x498f, 0x4990); parent_item_jump_table_from_rst(gb, 0x4990);
  switch (HL) {
    case 0x499c: goto thing4;
    case 0x49a1: goto thing2;
    case 0x49ac: goto thing1;
    case 0x49b6: goto thing3;
    case 0x49d2: goto thing5;
    case 0x49d8: goto thing0;
    default: hook_continue(gb, HL, sp0_); return;
  }

thing4:
  CYC(0x499c, 0x499f); A = mem_rd(gb, w1ParentItem2_enabled);
  CYC(0x499f, 0x49a0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x49a0, 0x49a1); ret_effect(gb); return;
  }
  CYC(0x49a0, 0x49a1);

thing2:
  CYC(0x49a1, 0x49a4); SET_HL(w1ParentItem3_id);
  CYC(0x49a4, 0x49a5); A = E;
  CYC(0x49a5, 0x49a6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x49a6, 0x49a8); goto thing0;
  }
  CYC(0x49a6, 0x49a8);
  CYC(0x49a8, 0x49a9); H = alu_inc8(gb, H);
  CYC(0x49a9, 0x49aa); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x49aa, 0x49ac); goto thing0;
  }
  CYC(0x49aa, 0x49ac);

thing1:
  CYC(0x49ac, 0x49af); SET_HL(w1ParentItem3_enabled);
  CYC(0x49af, 0x49b0); A = mem_rd(gb, HL);
  CYC(0x49b0, 0x49b1); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x49b1, 0x49b2); ret_effect(gb); return;
  }
  CYC(0x49b1, 0x49b2);
  CYC(0x49b2, 0x49b3); H = alu_inc8(gb, H);
  CYC(0x49b3, 0x49b4); A = mem_rd(gb, HL);
  CYC(0x49b4, 0x49b5); alu_or(gb, A);
  CYC(0x49b5, 0x49b6); ret_effect(gb); return;

thing3:
  CYC(0x49b6, 0x49b9); SET_HL(w1ParentItem2_enabled);
  CYC(0x49b9, 0x49ba); A = C;
  CYC(0x49ba, 0x49bc); alu_and(gb, 0xf0);
  CYC(0x49bc, 0x49bd); A = alu_inc8(gb, A);
  CYC(0x49bd, 0x49be); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x49be, 0x49c0); goto thing0;
  }
  CYC(0x49be, 0x49c0);
  CYC(0x49c0, 0x49c1); push_effect(gb, DE);
  CYC(0x49c1, 0x49c2); push_effect(gb, BC);
  CALL_C(0x49c2, clearParentItemH_hook, 0x4a50, 0x49c5);
  CYC(0x49c5, 0x49c6); SET_BC(pop_effect(gb));
  CYC(0x49c6, 0x49c7); SET_DE(pop_effect(gb));
  CYC(0x49c7, 0x49ca); SET_HL(w1ParentItem2);
  CYC(0x49ca, 0x49cb); alu_xor(gb, A);
  CYC(0x49cb, 0x49ce); W8(wMagnetGloveState) = A;
  CYC(0x49ce, 0x49d1); W8(wcc63) = A;
  CYC(0x49d1, 0x49d2); ret_effect(gb); return;

thing5:
  CYC(0x49d2, 0x49d5); SET_HL(w1ParentItem5_enabled);
  CYC(0x49d5, 0x49d6); A = mem_rd(gb, HL);
  CYC(0x49d6, 0x49d7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x49d7, 0x49d8); ret_effect(gb); return;
  }
  CYC(0x49d7, 0x49d8);

thing0:
  CYC(0x49d8, 0x49d9); alu_or(gb, H);
  CYC(0x49d9, 0x49da); ret_effect(gb);
}

void parentItemUpdate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49ef, 0x49f0); A = E;
  CYC(0x49f0, 0x49f2); H8(hActiveObjectType) = A;
  CYC(0x49f2, 0x49f3); A = D;
  CYC(0x49f3, 0x49f5); H8(hActiveObject) = A;
  CALL_C(0x49f5, itemIndexToBit, 0x548c, 0x49f8);
  CYC(0x49f8, 0x49fb); SET_HL(wcc95);
  CYC(0x49fb, 0x49fc); alu_cpl(gb);
  CYC(0x49fc, 0x49fd); alu_and(gb, mem_rd(gb, HL));
  CYC(0x49fd, 0x49fe); mem_wr(gb, HL, A);
  CYC(0x49fe, 0x4a00); E = 0x01;
  CYC(0x4a00, 0x4a01); A = mem_rd(gb, DE);
  CYC(0x4a01, 0x4a02); parent_item_jump_table_from_rst(gb, 0x4a02);
  hook_continue(gb, HL, sp0_);
}

void checkItemUsed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4954, 0x4956); H = 0xc6;
  CYC(0x4956, 0x4957); L = E;
  CYC(0x4957, 0x4958); A = mem_rd(gb, HL);
  CYC(0x4958, 0x4959); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4959, 0x495b); goto check_item;
  }
  CYC(0x4959, 0x495b);
  CYC(0x495b, 0x495e); A = W8(wActiveRing);
  CYC(0x495e, 0x4960); alu_cp(gb, 0x0b);
  if (F & FZ) {
    CYCT(0x4960, 0x4962); goto punch;
  }
  CYC(0x4960, 0x4962);
  CYC(0x4962, 0x4964); alu_cp(gb, 0x3d);
  if (!(F & FZ)) {
    CYCT(0x4964, 0x4965); ret_effect(gb); return;
  }
  CYC(0x4964, 0x4965);

punch:
  CYC(0x4965, 0x4967); L = 0x88;
  CYC(0x4967, 0x4968); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4968, 0x4969); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4969, 0x496a); ret_effect(gb); return;
  }
  CYC(0x4969, 0x496a);
  CYC(0x496a, 0x496c); A = 0x02;

check_item:
  CYC(0x496c, 0x496e); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(0x496e, 0x496f); ret_effect(gb); return;
  }
  CYC(0x496e, 0x496f);
  CYC(0x496f, 0x4970); E = A;
  CYC(0x4970, 0x4973); SET_HL(0x55be);
  CYC(0x4973, 0x4974); parent_item_add_double_index_from_rst(gb, 0x4974);
  CYC(0x4974, 0x4975); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4975, 0x4976); C = A;
  CYC(0x4976, 0x4977); L = mem_rd(gb, HL);
  CYC(0x4977, 0x4979); H = 0xcc;
  CYC(0x4979, 0x497a); A = mem_rd(gb, HL);
  CYC(0x497a, 0x497b); alu_and(gb, D);
  if (F & FZ) {
    CYCT(0x497b, 0x497c); ret_effect(gb); return;
  }
  CYC(0x497b, 0x497c);
  CALL_C(0x497c, chooseParentItemSlot_hook, 0x498c, 0x497f);
  if (!(F & FZ)) {
    CYCT(0x497f, 0x4980); ret_effect(gb); return;
  }
  CYC(0x497f, 0x4980);
  initializeParentItem_hook(gb);
}

void checkUseItems_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x48b3, 0x48b4); alu_xor(gb, A);
  CYC(0x48b4, 0x48b7); W8(wUsingShield) = A;
  CYC(0x48b7, 0x48ba); SET_HL(wSwordDisabledCounter);
  CYC(0x48ba, 0x48bb); A = mem_rd(gb, HL);
  CYC(0x48bb, 0x48bc); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x48bc, 0x48be);
  } else {
    CYC(0x48bc, 0x48be);
    CYC(0x48be, 0x48bf); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(0x48bf, 0x48c2); SET_HL(wLinkUsingItem1);
  CYC(0x48c2, 0x48c3); A = mem_rd(gb, HL);
  CYC(0x48c3, 0x48c5); alu_and(gb, 0x0f);
  CYC(0x48c5, 0x48c6); mem_wr(gb, HL, A);
  CYC(0x48c6, 0x48c9); A = W8(wcc63);
  CYC(0x48c9, 0x48ca); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x48ca, 0x48cc); goto items_disabled;
  }
  CYC(0x48ca, 0x48cc);
  CYC(0x48cc, 0x48cf); A = W8(wInShop);
  CYC(0x48cf, 0x48d0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x48d0, 0x48d3); checkShopInput_hook(gb); return;
  }
  CYC(0x48d0, 0x48d3);
  CYC(0x48d3, 0x48d6); A = W8(wcc95);
  CYC(0x48d6, 0x48d7); B = A;
  CYC(0x48d7, 0x48da); A = W8(wLinkInAir);
  CYC(0x48da, 0x48db); alu_or(gb, B);
  CYC(0x48db, 0x48dc); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x48dc, 0x48de); goto update_parent_items;
  }
  CYC(0x48dc, 0x48de);
  CYC(0x48de, 0x48e1); A = W8(wccd8);
  CYC(0x48e1, 0x48e2); B = A;
  CYC(0x48e2, 0x48e5); A = W8(wLinkGrabState);
  CYC(0x48e5, 0x48e6); alu_or(gb, B);
  if (!(F & FZ)) {
    CYCT(0x48e6, 0x48e8); goto update_parent_items;
  }
  CYC(0x48e6, 0x48e8);
  CYC(0x48e8, 0x48eb); A = W8(wLinkClimbingVine);
  CYC(0x48eb, 0x48ec); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x48ec, 0x48ee); goto update_parent_items;
  }
  CYC(0x48ec, 0x48ee);
  CYC(0x48ee, 0x48f1); A = W8(wTilesetFlags);
  CYC(0x48f1, 0x48f3); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x48f3, 0x48f5); goto sidescroll;
  }
  CYC(0x48f3, 0x48f5);
  CYC(0x48f5, 0x48f7); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(0x48f7, 0x48f9); goto normal;
  }
  CYC(0x48f7, 0x48f9);
  CYC(0x48f9, 0x48fc); SET_DE(0x0189);
  CALL_C(0x48fc, checkItemUsed_hook, 0x4954, 0x48ff);
  CYC(0x48ff, 0x4901); goto update_parent_items;

normal:
  CYC(0x4901, 0x4904); A = W8(wLinkSwimmingState);
  CYC(0x4904, 0x4905); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4905, 0x4907); goto check_ab;
  }
  CYC(0x4905, 0x4907);
  CYC(0x4907, 0x4909); goto update_parent_items;

sidescroll:
  CYC(0x4909, 0x490c); A = W8(wLinkSwimmingState);
  CYC(0x490c, 0x490d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x490d, 0x490f); goto check_ab;
  }
  CYC(0x490d, 0x490f);
  CYC(0x490f, 0x4912); SET_HL(w1Link_var2f);
  CYC(0x4912, 0x4914); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4914, 0x4916); goto check_b;
  }
  CYC(0x4914, 0x4916);

check_ab:
  CYC(0x4916, 0x4919); SET_DE(0x0189);
  CALL_C(0x4919, checkItemUsed_hook, 0x4954, 0x491c);

check_b:
  CYC(0x491c, 0x491f); SET_DE(0x0288);
  CALL_C(0x491f, checkItemUsed_hook, 0x4954, 0x4922);

update_parent_items:
  CYC(0x4922, 0x4925); SET_DE(w1ParentItem2);
  do {
    CYC(0x4925, 0x4927); E = 0x00;
    CYC(0x4927, 0x4928); A = mem_rd(gb, DE);
    CYC(0x4928, 0x4929); alu_or(gb, A);
    if (!(F & FZ)) {
      CALL_C_CC(0x4929, parentItemUpdate_hook, 0x49ef, 0x492c);
    } else {
      CYC(0x4929, 0x492c);
    }
    CYC(0x492c, 0x492d); D = alu_inc8(gb, D);
    CYC(0x492d, 0x492e); A = D;
    CYC(0x492e, 0x4930); alu_cp(gb, 0xd6);
    if (F & FC) {
      CYCT(0x4930, 0x4932);
    } else {
      CYC(0x4930, 0x4932);
      break;
    }
  } while (1);
  CYC(0x4932, 0x4933); alu_xor(gb, A);
  CYC(0x4933, 0x4935); H8(hActiveObjectType) = A;
  CYC(0x4935, 0x4937); D = 0xd0;
  CYC(0x4937, 0x4938); A = D;
  CYC(0x4938, 0x493a); H8(hActiveObject) = A;
  CYC(0x493a, 0x493b); ret_effect(gb); return;

items_disabled:
  CYC(0x493b, 0x493d); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x493d, 0x493f); goto update_parent_items;
  }
  CYC(0x493d, 0x493f);
  CALL_C(0x493f, clearAllParentItems_body_hook, 0x4878, 0x4942);
  CYC(0x4942, 0x4945); SET_HL(w1ParentItem2);
  CYC(0x4945, 0x4948); SET_DE(0xff05);
  CYC(0x4948, 0x494a); C = 0xf1;
  CALL_C(0x494a, initializeParentItem_hook, 0x4980, 0x494d);
  CYC(0x494d, 0x494f); A = 0x80;
  CYC(0x494f, 0x4952); W8(wcc63) = A;
  CYC(0x4952, 0x4954); goto update_parent_items;
}

void functionCaller_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4870, 0x4871); A = C;
  CYC(0x4871, 0x4872); parent_item_jump_table_from_rst(gb, 0x4872);
  switch (HL) {
    case 0x4878: clearAllParentItems_body_hook(gb); return;
    case 0x4890: updateParentItemButtonAssignment_body_hook(gb); return;
    case 0x48b3: checkUseItems_b06_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
