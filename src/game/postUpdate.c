#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void updateSwingableItemAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fca, 0x5fcc); L = 0x21;
  CYC(0x5fcc, 0x5fce); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(0x5fce, 0x5fd0);
    label_07_227(gb);
    return;
  }
  CYC(0x5fce, 0x5fd0);
  CYC(0x5fd0, 0x5fd2); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5fd2, 0x5fd4);
    label_07_227(gb);
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
  label_07_227(gb);
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
  CALL_C(0x60ce, itemSetPositionInSwordArc, 0x60d4, 0x60d1);
  CYC(0x60d1, 0x60d4);
  itemCalculateSwordDamage_hook(gb);
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
