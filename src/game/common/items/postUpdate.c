#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(label_07_227), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(label_07_227), (from), (to), true)

#define swingableItemAnimationData_bank07 SYM(label_07_227__data)

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
  BASE(label_07_227);
  CYC(b_+0, b_+2); C = 0x10;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+6); alu_cp(gb, C);
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+10); A = alu_srl(gb, A);
    CYC(b_+10, b_+11); C = A;
    CYC(b_+11, b_+14); A = W8(w1Link_direction);
    CYC(b_+14, b_+15); alu_add(gb, A);
    CYC(b_+15, b_+16); alu_add(gb, A);
    CYC(b_+16, b_+17); alu_add(gb, C);
    CYC(b_+17, b_+19); C = 0x00;
  }
  CYC(b_+19, b_+22); SET_HL(swingableItemAnimationData_bank07);
  CYC(b_+22, b_+23); post_update_add_a_to_hl(gb, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_and(gb, 0xf0);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+31); E = 0x30;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_and(gb, 0x07);
  CYC(b_+35, b_+38); itemSetAnimation_hook(gb);
}

void updateSwingableItemAnimation_hook(GB *gb) {
  BASE(updateSwingableItemAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = 0x21;
  CYC(b_+2, b_+4); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    label_07_227_hook(gb);
    return;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    label_07_227_hook(gb);
    return;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_and(gb, 0x1f);
  CYC(b_+15, b_+17); alu_cp(gb, 0x10);
  if (!(F & FC)) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+22); A = W8(w1Link_direction);
    CYC(b_+22, b_+23); alu_add(gb, A);
  }
  CYC(b_+23, b_+25); alu_and(gb, 0x07);
  CYC(b_+25, b_+26); push_effect(gb, HL);
  CALL_C(b_+26, tryBreakTileWithSword_calculateLevel_hook, SYM(tryBreakTileWithSword_calculateLevel), b_+29);
  CYC(b_+29, SYM(label_07_227)); SET_HL(pop_effect(gb));
  label_07_227_hook(gb);
}

void updateBiggoronSwordAnimation_hook(GB *gb) {
  BASE(updateBiggoronSwordAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+4); L = 0x21;
  CYC(b_+4, b_+6); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
    CYC(b_+10, b_+11); B = alu_inc8(gb, B);
  }
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_and(gb, 0x0e);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+19); A = W8(w1Link_direction);
  CYC(b_+19, b_+21); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYC(b_+21, b_+23);
    CYC(b_+23, b_+24); A = C;
    CYC(b_+24, b_+26);
  } else {
    CYCT(b_+21, b_+23);
    CYC(b_+26, b_+27); A = alu_inc8(gb, A);
    CYC(b_+27, b_+28); alu_add(gb, A);
    CYC(b_+28, b_+29); alu_sub(gb, C);
  }
  CYC(b_+29, b_+31); alu_and(gb, 0x07);
  CYC(b_+31, b_+33); alu_bit(gb, 0, B);
  if (F & FZ) {
    CYCT(b_+33, b_+35);
  } else {
    CYC(b_+33, b_+35);
    CYC(b_+35, b_+36); push_effect(gb, AF);
    CYC(b_+36, b_+37); C = A;
    CYC(b_+37, b_+39); A = 0x02;
    CALL_C(b_+39, tryBreakTileWithSword_hook, SYM(tryBreakTileWithSword), b_+42);
    CYC(b_+42, b_+43); SET_AF(pop_effect(gb));
  }
  CYC(b_+43, b_+45); E = 0x30;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, SYM(itemCode08Post));
  itemSetAnimation_hook(gb);
}

void itemCode08Post_hook(GB *gb) {
  BASE(itemCode08Post);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); SET_HL(w1Link_yh);
  CALL_C(b_+9, objectTakePosition_hook, SYM(objectTakePosition), b_+12);
  CYC(b_+12, b_+15); A = W8(wFrameCounter);
  CYC(b_+15, b_+16); alu_rrca(gb);
  CYC(b_+16, b_+17); alu_rrca(gb);
  CYC(b_+17, b_+20); A = W8(w1Link_direction);
  CYC(b_+20, b_+21); alu_adc(gb, A);
  CYC(b_+21, b_+23); E = 0x30;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, SYM(itemCode13Post));
  itemSetAnimation_hook(gb);
}

void itemCode13Post_hook(GB *gb) {
  BASE(itemCode13Post);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); SET_HL(w1Link_yh);
  CALL_C(b_+9, objectTakePosition_hook, SYM(objectTakePosition), b_+12);
  CYC(b_+12, b_+15); A = W8(w1Link_direction);
  CYC(b_+15, b_+17); E = 0x30;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, SYM(itemCode1ePost));
  itemSetAnimation_hook(gb);
}

void itemCode1ePost_hook(GB *gb) {
  BASE(itemCode1ePost);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = 0x21;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0x06);
  CYC(b_+11, b_+12); alu_add(gb, A);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+16); A = W8(w1Link_direction);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+19); E = 0x30;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); SET_HL(SYM(swordArcData));
  CYC(b_+23, SYM(itemCode00Post));
  itemSetPositionInSwordArc_hook(gb);
}

void itemCode00Post_hook(GB *gb) {
  BASE(itemCode00Post);
  CYC(b_+0, b_+3); A = W8(w1Link_direction);
  CYC(b_+3, b_+5); alu_add(gb, 0x18);
  CYC(b_+5, b_+8); SET_HL(SYM(swordArcData));
  CYC(b_+8, SYM(itemCode0cPost));
  itemSetPositionInSwordArc_hook(gb);
}

void itemCode0cPost_hook(GB *gb) {
  BASE(itemCode0cPost);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, updateBiggoronSwordAnimation_hook, SYM(updateBiggoronSwordAnimation), b_+9);
  CYC(b_+9, b_+11); E = 0x30;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+15); SET_HL(SYM(biggoronSwordArcData));
  CALL_C(b_+15, itemSetPositionInSwordArc_hook, SYM(itemSetPositionInSwordArc), b_+18);
  CYC(b_+18, SYM(itemCode04Post));
  itemCalculateSwordDamage_hook(gb);
}

void itemCode04Post_hook(GB *gb) {
  BASE(itemCode04Post);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRelatedObject1ID_hook, SYM(cpRelatedObject1ID), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    itemDelete_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, updateSwingableItemAnimation_hook, SYM(updateSwingableItemAnimation), b_+9);
  CYC(b_+9, b_+11); E = 0x30;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+15); SET_HL(SYM(swordArcData));
  CALL_C(b_+15, itemSetPositionInSwordArc_hook, SYM(itemSetPositionInSwordArc), b_+18);
  CYC(b_+18, SYM(itemSetPositionInSwordArc));
  itemCalculateSwordDamage_hook(gb);
}

void itemSetPositionInSwordArc_hook(GB *gb) {
  BASE(itemSetPositionInSwordArc);
  CYC(b_+0, b_+1); alu_add(gb, A);
  CYC(b_+1, SYM(itemInitializeFromLinkPosition)); post_update_add_double_index(gb, SYM(itemInitializeFromLinkPosition));
  itemInitializeFromLinkPosition_hook(gb);
}

void itemInitializeFromLinkPosition_hook(GB *gb) {
  BASE(itemInitializeFromLinkPosition);
  CYC(b_+0, b_+2); E = 0x26;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+10); A = W8(wLinkRaisedFloorOffset);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); A = W8(w1Link_yh);
  CYC(b_+14, b_+15); alu_add(gb, B);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+18); E = 0x0b;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); SET_HL(HL + 1);
  CYC(b_+20, b_+22); E = 0x0d;
  CYC(b_+22, b_+25); A = W8(w1Link_xh);
  CYC(b_+25, b_+26); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30); A = W8(w1Link_zh);
  CYC(b_+30, b_+32); E = 0x0f;
  CYC(b_+32, b_+34); alu_sub(gb, 0x02);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, SYM(swordArcData)); ret_effect(gb);
}
