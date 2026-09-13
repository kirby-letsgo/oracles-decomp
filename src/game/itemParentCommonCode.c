#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void itemIndexToBit_hook(GB *gb);
void andHlWithGameKeysPressed_hook(GB *gb);
void itemCreateChildAndDeleteOnFailure_hook(GB *gb);
void itemCreateChild_hook(GB *gb);
void itemCreateChildWithID_hook(GB *gb);
void getFreeItemSlotWithObjectCap_hook(GB *gb);
void isLinkUnderwater_hook(GB *gb);

void itemCreateChildIfDoesntExistAlready_hook(GB *gb) {
  CYC(0x53d4, 0x53d6); E = 0x01;
  itemCreateChildAndDeleteOnFailure_hook(gb);
}

void itemCreateChildAndDeleteOnFailure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x53d6, itemCreateChild_hook, 0x53dd, 0x53d9);
  if (!(F & FC)) {
    CYCT(0x53d9, 0x53da); ret_effect(gb); return;
  }
  CYC(0x53d9, 0x53da);
  CYC(0x53da, 0x53dd); clearParentItem_hook(gb);
}

void itemCreateChild_hook(GB *gb) {
  CYC(0x53dd, 0x53df); C = 0x00;
  CYC(0x53df, 0x53e0); H = D;
  CYC(0x53e0, 0x53e2); L = 0x01;
  CYC(0x53e2, 0x53e3); B = mem_rd(gb, HL);
  itemCreateChildWithID_hook(gb);
}

void itemCreateChildWithID_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53e3, 0x53e4); H = D;
  CYC(0x53e4, 0x53e6); L = 0x19;
  CYC(0x53e6, 0x53e7); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x53e7, 0x53e8); L = mem_rd(gb, HL);
  CYC(0x53e8, 0x53e9); H = A;
  CYC(0x53e9, 0x53eb); alu_cp(gb, 0x01);
  CYC(0x53eb, 0x53ec); alu_scf(gb);
  if (F & FZ) {
    CYCT(0x53ec, 0x53ed); ret_effect(gb); return;
  }
  CYC(0x53ec, 0x53ed);
  CYC(0x53ed, 0x53ef); alu_cp(gb, 0xd0);
  if (F & FZ) {
    CALL_C_CC(0x53ef, getFreeItemSlotWithObjectCap_hook, 0x5416, 0x53f2);
  } else {
    CYC(0x53ef, 0x53f2);
  }
  if (F & FC) {
    CYCT(0x53f2, 0x53f3); ret_effect(gb); return;
  }
  CYC(0x53f2, 0x53f3);
  CYC(0x53f3, 0x53f4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53f4, 0x53f5); L = alu_inc8(gb, L);
  CYC(0x53f5, 0x53f6); A = B;
  CYC(0x53f6, 0x53f7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x53f7, 0x53f8); A = C;
  CYC(0x53f8, 0x53f9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x53f9, 0x53fa); alu_xor(gb, A);
  CYC(0x53fa, 0x53fb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x53fb, 0x53fc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x53fc, 0x53fd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x53fd, 0x53fe); push_effect(gb, DE);
  CYC(0x53fe, 0x5401); SET_DE(w1Link_direction);
  CYC(0x5401, 0x5403); L = 0x08;
  CYC(0x5403, 0x5405); B = 0x08;
  CALL_C(0x5405, copyMemoryReverse_hook, 0x047f, 0x5408);
  CYC(0x5408, 0x5409); SET_DE(pop_effect(gb));
  CYC(0x5409, 0x540b); L = 0x16;
  CYC(0x540b, 0x540c); alu_xor(gb, A);
  CYC(0x540c, 0x540d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x540d, 0x540e); mem_wr(gb, HL, D);
  CYC(0x540e, 0x5410); E = 0x18;
  CYC(0x5410, 0x5411); mem_wr(gb, DE, A);
  CYC(0x5411, 0x5412); E = alu_inc8(gb, E);
  CYC(0x5412, 0x5413); A = H;
  CYC(0x5413, 0x5414); mem_wr(gb, DE, A);
  CYC(0x5414, 0x5415); alu_xor(gb, A);
  CYC(0x5415, 0x5416); ret_effect(gb);
}

void getFreeItemSlotWithObjectCap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5416, 0x5419); SET_HL(0xd701);

item_loop:
  CYC(0x5419, 0x541a); A = mem_rd(gb, HL);
  CYC(0x541a, 0x541b); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x541b, 0x541d); goto next_item;
  }
  CYC(0x541b, 0x541d);
  CYC(0x541d, 0x541e); L = alu_inc8(gb, L);
  CYC(0x541e, 0x541f); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x541f, 0x5420); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x5420, 0x5422); goto next_item;
  }
  CYC(0x5420, 0x5422);
  CYC(0x5422, 0x5423); E = alu_dec8(gb, E);
  if (F & FZ) {
    CYCT(0x5423, 0x5425); goto failure;
  }
  CYC(0x5423, 0x5425);

next_item:
  CYC(0x5425, 0x5426); H = alu_inc8(gb, H);
  CYC(0x5426, 0x5427); A = H;
  CYC(0x5427, 0x5429); alu_cp(gb, 0xdc);
  if (F & FC) {
    CYCT(0x5429, 0x542b); goto item_loop;
  }
  CYC(0x5429, 0x542b);
  CALL_C(0x542b, getFreeItemSlot_hook, 0x2cf9, 0x542e);
  if (F & FZ) {
    CYCT(0x542e, 0x542f); ret_effect(gb); return;
  }
  CYC(0x542e, 0x542f);

failure:
  CYC(0x542f, 0x5430); alu_scf(gb);
  CYC(0x5430, 0x5431); ret_effect(gb);
}

void getNumFreeItemSlots_hook(GB *gb) {
  CYC(0x5431, 0x5434); SET_HL(0xd700);
  CYC(0x5434, 0x5436); B = 0x00;
  do {
    CYC(0x5436, 0x5437); A = mem_rd(gb, HL);
    CYC(0x5437, 0x5438); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(0x5438, 0x543a);
    } else {
      CYC(0x5438, 0x543a);
      CYC(0x543a, 0x543b); B = alu_inc8(gb, B);
    }
    CYC(0x543b, 0x543c); H = alu_inc8(gb, H);
    CYC(0x543c, 0x543d); A = H;
    CYC(0x543d, 0x543f); alu_cp(gb, 0xdc);
    if (F & FC) CYCT(0x543f, 0x5441); else CYC(0x543f, 0x5441);
  } while (F & FC);
  CYC(0x5441, 0x5442); A = B;
  CYC(0x5442, 0x5443); alu_or(gb, A);
  CYC(0x5443, 0x5444); ret_effect(gb);
}

void setLinkUsingItem1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5444, itemIndexToBit_hook, 0x548c, 0x5447);
  CYC(0x5447, 0x5449); A = alu_swap(gb, A);
  CYC(0x5449, 0x544a); alu_or(gb, mem_rd(gb, HL));
  CYC(0x544a, 0x544d); SET_HL(wLinkUsingItem1);
  CYC(0x544d, 0x544e); alu_or(gb, mem_rd(gb, HL));
  CYC(0x544e, 0x544f); mem_wr(gb, HL, A);
  CYC(0x544f, 0x5450); ret_effect(gb);
}

void clearLinkUsingItem1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5450, itemIndexToBit_hook, 0x548c, 0x5453);
  CYC(0x5453, 0x5455); A = alu_swap(gb, A);
  CYC(0x5455, 0x5456); alu_or(gb, mem_rd(gb, HL));
  CYC(0x5456, 0x5457); alu_cpl(gb);
  CYC(0x5457, 0x545a); SET_HL(wLinkUsingItem1);
  CYC(0x545a, 0x545b); alu_and(gb, mem_rd(gb, HL));
  CYC(0x545b, 0x545c); mem_wr(gb, HL, A);
  CYC(0x545c, 0x545d); ret_effect(gb);
}

void itemDisableLinkMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x545d, itemIndexToBit_hook, 0x548c, 0x5460);
  CYC(0x5460, 0x5463); SET_HL(wLinkImmobilized);
  CYC(0x5463, 0x5464); alu_or(gb, mem_rd(gb, HL));
  CYC(0x5464, 0x5465); mem_wr(gb, HL, A);
  CYC(0x5465, 0x5466); ret_effect(gb);
}

void itemEnableLinkMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5466, itemIndexToBit_hook, 0x548c, 0x5469);
  CYC(0x5469, 0x546c); SET_HL(wLinkImmobilized);
  CYC(0x546c, 0x546d); alu_cpl(gb);
  CYC(0x546d, 0x546e); alu_and(gb, mem_rd(gb, HL));
  CYC(0x546e, 0x546f); mem_wr(gb, HL, A);
  CYC(0x546f, 0x5470); ret_effect(gb);
}

void itemDisableLinkTurning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5470, itemIndexToBit_hook, 0x548c, 0x5473);
  CYC(0x5473, 0x5476); SET_HL(wLinkTurningDisabled);
  CYC(0x5476, 0x5477); alu_or(gb, mem_rd(gb, HL));
  CYC(0x5477, 0x5478); mem_wr(gb, HL, A);
  CYC(0x5478, 0x5479); ret_effect(gb);
}

void itemEnableLinkTurning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5479, itemIndexToBit_hook, 0x548c, 0x547c);
  CYC(0x547c, 0x547f); SET_HL(wLinkTurningDisabled);
  CYC(0x547f, 0x5480); alu_cpl(gb);
  CYC(0x5480, 0x5481); alu_and(gb, mem_rd(gb, HL));
  CYC(0x5481, 0x5482); mem_wr(gb, HL, A);
  CYC(0x5482, 0x5483); ret_effect(gb);
}

void setCc95Bit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5483, itemIndexToBit_hook, 0x548c, 0x5486);
  CYC(0x5486, 0x5489); SET_HL(wcc95);
  CYC(0x5489, 0x548a); alu_or(gb, mem_rd(gb, HL));
  CYC(0x548a, 0x548b); mem_wr(gb, HL, A);
  CYC(0x548b, 0x548c); ret_effect(gb);
}

void itemIndexToBit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x548c, 0x548d); A = D;
  CYC(0x548d, 0x548f); alu_sub(gb, 0xd2);
  CYC(0x548f, 0x5492); SET_HL(0x00f8);
  CYC(0x5492, 0x5493); alu_add(gb, L);
  CYC(0x5493, 0x5494); L = A;
  CYC(0x5494, 0x5495); A = mem_rd(gb, HL);
  CYC(0x5495, 0x5496); ret_effect(gb);
}

void parentItemCheckButtonPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5496, 0x5497); H = D;
  CYC(0x5497, 0x5499); L = 0x03;
  andHlWithGameKeysPressed_hook(gb);
}

void andHlWithGameKeysPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5499, 0x549c); A = W8(wGameKeysPressed);
  CYC(0x549c, 0x549d); alu_and(gb, mem_rd(gb, HL));
  CYC(0x549d, 0x549e); ret_effect(gb);
}

void checkLinkOnGround_hook(GB *gb) {
  CYC(0x54c4, 0x54c7); A = W8(wLinkObjectIndex);
  CYC(0x54c7, 0x54c9); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x54c9, 0x54ca); ret_effect(gb); return;
  }
  CYC(0x54c9, 0x54ca);
  CYC(0x54ca, 0x54cd); SET_HL(wLinkInAir);
  CYC(0x54cd, 0x54ce); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54ce, 0x54cf); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x54cf, 0x54d0); ret_effect(gb); return;
  }
  CYC(0x54cf, 0x54d0);
  CYC(0x54d0, 0x54d2); isLinkUnderwater_hook(gb);
}

void isLinkUnderwater_hook(GB *gb) {
  CYC(0x54d2, 0x54d5); A = W8(w1Link_var2f);
  CYC(0x54d5, 0x54d7); alu_bit(gb, 7, A);
  CYC(0x54d7, 0x54d8); ret_effect(gb);
}

void isLinkInHole_hook(GB *gb) {
  CYC(0x54d8, 0x54db); A = W8(wActiveTileType);
  CYC(0x54db, 0x54dc); A = alu_dec8(gb, A);
  CYC(0x54dc, 0x54de); alu_cp(gb, 0x02);
  CYC(0x54de, 0x54df); ret_effect(gb);
}
