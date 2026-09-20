#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

#define swingableItemAnimationData_bank07 0x600e

void itemSetPositionInSwordArc_hook(GB *gb);

static void post_update_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void post_update_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  }
  ret_effect(gb);
}

void label_07_227_hook(GB *gb) {
  CYC(0x5fe8, 0x5fea); C = 0x10;
  CYC(0x5fea, 0x5feb); A = mem_rd(gb, HL);
  CYC(0x5feb, 0x5fed); alu_and(gb, 0x1f);
  CYC(0x5fed, 0x5fee); alu_cp(gb, C);
  if (!(F & FC)) {
    CYCT(0x5fee, 0x5ff0);
  } else {
    CYC(0x5fee, 0x5ff0);
    CYC(0x5ff0, 0x5ff2); A = alu_srl(gb, A);
    CYC(0x5ff2, 0x5ff3); C = A;
    CYC(0x5ff3, 0x5ff6); A = W8(w1Link_direction);
    CYC(0x5ff6, 0x5ff7); alu_add(gb, A);
    CYC(0x5ff7, 0x5ff8); alu_add(gb, A);
    CYC(0x5ff8, 0x5ff9); alu_add(gb, C);
    CYC(0x5ff9, 0x5ffb); C = 0x00;
  }
  CYC(0x5ffb, 0x5ffe); SET_HL(swingableItemAnimationData_bank07);
  CYC(0x5ffe, 0x5fff); post_update_add_a_to_hl(gb, 0x5fff);
  CYC(0x5fff, 0x6000); A = mem_rd(gb, HL);
  CYC(0x6000, 0x6002); alu_and(gb, 0xf0);
  CYC(0x6002, 0x6004); A = alu_swap(gb, A);
  CYC(0x6004, 0x6005); alu_add(gb, C);
  CYC(0x6005, 0x6007); E = 0x30;
  CYC(0x6007, 0x6008); mem_wr(gb, DE, A);
  CYC(0x6008, 0x6009); A = mem_rd(gb, HL);
  CYC(0x6009, 0x600b); alu_and(gb, 0x07);
  CYC(0x600b, 0x600e); itemSetAnimation_hook(gb);
}

void updateSwingableItemAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fca, 0x5fcc); L = 0x21;
  CYC(0x5fcc, 0x5fce); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(0x5fce, 0x5fd0);
    label_07_227_hook(gb);
    return;
  }
  CYC(0x5fce, 0x5fd0);
  CYC(0x5fd0, 0x5fd2); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5fd2, 0x5fd4);
    label_07_227_hook(gb);
    return;
  }
  CYC(0x5fd2, 0x5fd4);
  CYC(0x5fd4, 0x5fd6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(0x5fd6, 0x5fd7); A = mem_rd(gb, HL);
  CYC(0x5fd7, 0x5fd9); alu_and(gb, 0x1f);
  CYC(0x5fd9, 0x5fdb); alu_cp(gb, 0x10);
  if (!(F & FC)) {
    CYCT(0x5fdb, 0x5fdd);
  } else {
    CYC(0x5fdb, 0x5fdd);
    CYC(0x5fdd, 0x5fe0); A = W8(w1Link_direction);
    CYC(0x5fe0, 0x5fe1); alu_add(gb, A);
  }
  CYC(0x5fe1, 0x5fe3); alu_and(gb, 0x07);
  CYC(0x5fe3, 0x5fe4); push_effect(gb, HL);
  CALL_C(0x5fe4, tryBreakTileWithSword_calculateLevel_hook, 0x6193, 0x5fe7);
  CYC(0x5fe7, 0x5fe8); SET_HL(pop_effect(gb));
  label_07_227_hook(gb);
}

void updateBiggoronSwordAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6026, 0x6028); B = 0x00;
  CYC(0x6028, 0x602a); L = 0x21;
  CYC(0x602a, 0x602c); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x602c, 0x602e);
  } else {
    CYC(0x602c, 0x602e);
    CYC(0x602e, 0x6030); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
    CYC(0x6030, 0x6031); B = alu_inc8(gb, B);
  }
  CYC(0x6031, 0x6032); A = mem_rd(gb, HL);
  CYC(0x6032, 0x6034); alu_and(gb, 0x0e);
  CYC(0x6034, 0x6035); alu_rrca(gb);
  CYC(0x6035, 0x6036); C = A;
  CYC(0x6036, 0x6039); A = W8(w1Link_direction);
  CYC(0x6039, 0x603b); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYC(0x603b, 0x603d);
    CYC(0x603d, 0x603e); A = C;
    CYC(0x603e, 0x6040);
  } else {
    CYCT(0x603b, 0x603d);
    CYC(0x6040, 0x6041); A = alu_inc8(gb, A);
    CYC(0x6041, 0x6042); alu_add(gb, A);
    CYC(0x6042, 0x6043); alu_sub(gb, C);
  }
  CYC(0x6043, 0x6045); alu_and(gb, 0x07);
  CYC(0x6045, 0x6047); alu_bit(gb, 0, B);
  if (F & FZ) {
    CYCT(0x6047, 0x6049);
  } else {
    CYC(0x6047, 0x6049);
    CYC(0x6049, 0x604a); push_effect(gb, AF);
    CYC(0x604a, 0x604b); C = A;
    CYC(0x604b, 0x604d); A = 0x02;
    CALL_C(0x604d, tryBreakTileWithSword_hook, 0x619d, 0x6050);
    CYC(0x6050, 0x6051); SET_AF(pop_effect(gb));
  }
  CYC(0x6051, 0x6053); E = 0x30;
  CYC(0x6053, 0x6054); mem_wr(gb, DE, A);
  CYC(0x6054, 0x6057);
  itemSetAnimation_hook(gb);
}

void itemCode08Post_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6057, cpRelatedObject1ID_hook, 0x4b86, 0x605a);
  if (!(F & FZ)) {
    CYCT(0x605a, 0x605d);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x605a, 0x605d);
  CYC(0x605d, 0x6060); SET_HL(w1Link_yh);
  CALL_C(0x6060, objectTakePosition_hook, 0x2274, 0x6063);
  CYC(0x6063, 0x6066); A = W8(wFrameCounter);
  CYC(0x6066, 0x6067); alu_rrca(gb);
  CYC(0x6067, 0x6068); alu_rrca(gb);
  CYC(0x6068, 0x606b); A = W8(w1Link_direction);
  CYC(0x606b, 0x606c); alu_adc(gb, A);
  CYC(0x606c, 0x606e); E = 0x30;
  CYC(0x606e, 0x606f); mem_wr(gb, DE, A);
  CYC(0x606f, 0x6072);
  itemSetAnimation_hook(gb);
}

void itemCode13Post_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6072, cpRelatedObject1ID_hook, 0x4b86, 0x6075);
  if (!(F & FZ)) {
    CYCT(0x6075, 0x6078);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x6075, 0x6078);
  CYC(0x6078, 0x607b); SET_HL(w1Link_yh);
  CALL_C(0x607b, objectTakePosition_hook, 0x2274, 0x607e);
  CYC(0x607e, 0x6081); A = W8(w1Link_direction);
  CYC(0x6081, 0x6083); E = 0x30;
  CYC(0x6083, 0x6084); mem_wr(gb, DE, A);
  CYC(0x6084, 0x6087);
  itemSetAnimation_hook(gb);
}

void itemCode1ePost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6087, cpRelatedObject1ID_hook, 0x4b86, 0x608a);
  if (!(F & FZ)) {
    CYCT(0x608a, 0x608d);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x608a, 0x608d);
  CYC(0x608d, 0x608f); L = 0x21;
  CYC(0x608f, 0x6090); A = mem_rd(gb, HL);
  CYC(0x6090, 0x6092); alu_and(gb, 0x06);
  CYC(0x6092, 0x6093); alu_add(gb, A);
  CYC(0x6093, 0x6094); B = A;
  CYC(0x6094, 0x6097); A = W8(w1Link_direction);
  CYC(0x6097, 0x6098); alu_add(gb, B);
  CYC(0x6098, 0x609a); E = 0x30;
  CYC(0x609a, 0x609b); mem_wr(gb, DE, A);
  CYC(0x609b, 0x609e); SET_HL(0x60fa);
  CYC(0x609e, 0x60a0);
  itemSetPositionInSwordArc_hook(gb);
}

void itemCode00Post_hook(GB *gb) {
  CYC(0x60a0, 0x60a3); A = W8(w1Link_direction);
  CYC(0x60a3, 0x60a5); alu_add(gb, 0x18);
  CYC(0x60a5, 0x60a8); SET_HL(0x60fa);
  CYC(0x60a8, 0x60aa);
  itemSetPositionInSwordArc_hook(gb);
}

void itemCode0cPost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60aa, cpRelatedObject1ID_hook, 0x4b86, 0x60ad);
  if (!(F & FZ)) {
    CYCT(0x60ad, 0x60b0);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x60ad, 0x60b0);
  CALL_C(0x60b0, updateBiggoronSwordAnimation_hook, 0x6026, 0x60b3);
  CYC(0x60b3, 0x60b5); E = 0x30;
  CYC(0x60b5, 0x60b6); A = mem_rd(gb, DE);
  CYC(0x60b6, 0x60b9); SET_HL(0x616a);
  CALL_C(0x60b9, itemSetPositionInSwordArc_hook, 0x60d4, 0x60bc);
  CYC(0x60bc, 0x60bf);
  itemCalculateSwordDamage_hook(gb);
}

void itemCode04Post_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60bf, cpRelatedObject1ID_hook, 0x4b86, 0x60c2);
  if (!(F & FZ)) {
    CYCT(0x60c2, 0x60c5);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x60c2, 0x60c5);
  CALL_C(0x60c5, updateSwingableItemAnimation_hook, 0x5fca, 0x60c8);
  CYC(0x60c8, 0x60ca); E = 0x30;
  CYC(0x60ca, 0x60cb); A = mem_rd(gb, DE);
  CYC(0x60cb, 0x60ce); SET_HL(0x60fa);
  CALL_C(0x60ce, itemSetPositionInSwordArc_hook, 0x60d4, 0x60d1);
  CYC(0x60d1, 0x60d4);
  itemCalculateSwordDamage_hook(gb);
}

void itemSetPositionInSwordArc_hook(GB *gb) {
  CYC(0x60d4, 0x60d5); alu_add(gb, A);
  CYC(0x60d5, 0x60d6); post_update_add_double_index(gb, 0x60d6);
  itemInitializeFromLinkPosition_hook(gb);
}

void itemInitializeFromLinkPosition_hook(GB *gb) {
  CYC(0x60d6, 0x60d8); E = 0x26;
  CYC(0x60d8, 0x60d9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60d9, 0x60da); mem_wr(gb, DE, A);
  CYC(0x60da, 0x60db); E = alu_inc8(gb, E);
  CYC(0x60db, 0x60dc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60dc, 0x60dd); mem_wr(gb, DE, A);
  CYC(0x60dd, 0x60e0); A = W8(wLinkRaisedFloorOffset);
  CYC(0x60e0, 0x60e1); B = A;
  CYC(0x60e1, 0x60e4); A = W8(w1Link_yh);
  CYC(0x60e4, 0x60e5); alu_add(gb, B);
  CYC(0x60e5, 0x60e6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x60e6, 0x60e8); E = 0x0b;
  CYC(0x60e8, 0x60e9); mem_wr(gb, DE, A);
  CYC(0x60e9, 0x60ea); SET_HL(HL + 1);
  CYC(0x60ea, 0x60ec); E = 0x0d;
  CYC(0x60ec, 0x60ef); A = W8(w1Link_xh);
  CYC(0x60ef, 0x60f0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x60f0, 0x60f1); mem_wr(gb, DE, A);
  CYC(0x60f1, 0x60f4); A = W8(w1Link_zh);
  CYC(0x60f4, 0x60f6); E = 0x0f;
  CYC(0x60f6, 0x60f8); alu_sub(gb, 0x02);
  CYC(0x60f8, 0x60f9); mem_wr(gb, DE, A);
  CYC(0x60f9, 0x60fa); ret_effect(gb);
}
