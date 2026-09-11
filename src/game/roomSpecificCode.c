#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x12, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x12, (from), (to), true)

void roomSpecificCode0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58ca, 0x58cc); A = 0x0e;
  CALL_C(0x58cc, checkGlobalFlag_hook, 0x31f3, 0x58cf);
  if (!(F & FZ)) {
    CYCT(0x58cf, 0x58d0); ret_effect(gb);
    return;
  }
  CYC(0x58cf, 0x58d0);
  CYC(0x58d0, 0x58d3); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(0x58d3, 0x58d5); B = 0x10;
  CYC(0x58d5, 0x58d8); clearMemory_hook(gb);
}

void room_specific_code_create_spinner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x58de, getFreeInteractionSlot_hook, 0x3aef, 0x58e1);
  if (!(F & FZ)) {
    CYCT(0x58e1, 0x58e2); ret_effect(gb);
    return;
  }
  CYC(0x58e1, 0x58e2);
  CYC(0x58e2, 0x58e4); mem_wr(gb, HL, 0x7d);
  CYC(0x58e4, 0x58e6); L = 0x4b;
  CYC(0x58e6, 0x58e8); mem_wr(gb, HL, 0x57);
  CYC(0x58e8, 0x58ea); L = 0x4d;
  CYC(0x58ea, 0x58ec); mem_wr(gb, HL, 0x01);
  CYC(0x58ec, 0x58ed); ret_effect(gb);
}

void roomSpecificCode1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58d8, 0x58da); A = 0x0f;
  CALL_C(0x58da, checkGlobalFlag_hook, 0x31f3, 0x58dd);
  if (!(F & FZ)) {
    CYCT(0x58dd, 0x58de); ret_effect(gb);
    return;
  }
  CYC(0x58dd, 0x58de); room_specific_code_create_spinner_hook(gb);
}

void roomSpecificCode2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58ed, 0x58ef); A = 0x0f;
  CALL_C(0x58ef, checkGlobalFlag_hook, 0x31f3, 0x58f2);
  if (F & FZ) {
    CYCT(0x58f2, 0x58f3); ret_effect(gb);
    return;
  }
  CYC(0x58f2, 0x58f3);
  CYC(0x58f3, 0x58f5); room_specific_code_create_spinner_hook(gb);
}

void roomSpecificCode3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x58f5, getThisRoomFlags_hook, 0x197d, 0x58f8);
  CYC(0x58f8, 0x58fa); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x58fa, 0x58fb); ret_effect(gb);
    return;
  }
  CYC(0x58fa, 0x58fb);
  CYC(0x58fb, 0x58fd); A = 0x24;
  CALL_C(0x58fd, checkTreasureObtained_hook, 0x1748, 0x5900);
  if (!(F & FC)) {
    CYCT(0x5900, 0x5901); ret_effect(gb);
    return;
  }
  CYC(0x5900, 0x5901);
  CYC(0x5901, 0x5904); SET_HL(wcc05);
  CYC(0x5904, 0x5906); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 1));
  CALL_C(0x5906, getFreeInteractionSlot_hook, 0x3aef, 0x5909);
  if (!(F & FZ)) {
    CYCT(0x5909, 0x590a); ret_effect(gb);
    return;
  }
  CYC(0x5909, 0x590a);
  CYC(0x590a, 0x590c); mem_wr(gb, HL, 0x40);
  CYC(0x590c, 0x590d); L = alu_inc8(gb, L);
  CYC(0x590d, 0x590f); mem_wr(gb, HL, 0x0a);
  CYC(0x590f, 0x5911); A = 0x01;
  CYC(0x5911, 0x5914); W8(wDiggingUpEnemiesForbidden) = A;
  CYC(0x5914, 0x5915); ret_effect(gb);
}

void roomSpecificCode7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5915, 0x5917); A = 0x15;
  CALL_C(0x5917, checkGlobalFlag_hook, 0x31f3, 0x591a);
  if (F & FZ) {
    CYCT(0x591a, 0x591b); ret_effect(gb);
    return;
  }
  CYC(0x591a, 0x591b);
  CALL_C(0x591b, getThisRoomFlags_hook, 0x197d, 0x591e);
  CYC(0x591e, 0x5920); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x5920, 0x5921); ret_effect(gb);
    return;
  }
  CYC(0x5920, 0x5921);
  CYC(0x5921, 0x5923); A = 0x35;
  CYC(0x5923, 0x5926); W8(wActiveMusic2) = A;
  CYC(0x5926, 0x5927); ret_effect(gb);
}

void roomSpecificCode5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5927, 0x5929); A = 0x11;
  CALL_C(0x5929, checkGlobalFlag_hook, 0x31f3, 0x592c);
  if (!(F & FZ)) {
    CYCT(0x592c, 0x592d); ret_effect(gb);
    return;
  }
  CYC(0x592c, 0x592d);
  CYC(0x592d, 0x592f); A = 0x1f;
  CYC(0x592f, 0x5932); W8(wActiveMusic2) = A;
  CYC(0x5932, 0x5933); ret_effect(gb);
}

void roomSpecificCode4_hook(GB *gb) {
  CYC(0x5933, 0x5935); A = 0x06;
  CYC(0x5935, 0x5938); W8(wMinimapRoom) = A;
  CYC(0x5938, 0x593b); SET_HL(wGroup1RoomFlags + 0x06);
  CYC(0x593b, 0x593d); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 4));
  CYC(0x593d, 0x593e); ret_effect(gb);
}

void roomSpecificCode8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x593e, 0x5941); A = W8(wScreenTransitionDirection);
  CYC(0x5941, 0x5943); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x5943, 0x5944); ret_effect(gb);
    return;
  }
  CYC(0x5943, 0x5944);
  CYC(0x5944, 0x5946); A = 0x40;
  CALL_C(0x5946, checkGlobalFlag_hook, 0x31f3, 0x5949);
  if (!(F & FZ)) {
    CYCT(0x5949, 0x594a); ret_effect(gb);
    return;
  }
  CYC(0x5949, 0x594a);
  CYC(0x594a, 0x594c); A = 0x35;
  CYC(0x594c, 0x594f); W8(wActiveMusic2) = A;
  CYC(0x594f, 0x5950); ret_effect(gb);
}

void roomSpecificCode9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5950, 0x5952); A = 0x14;
  CALL_C(0x5952, checkGlobalFlag_hook, 0x31f3, 0x5955);
  if (F & FZ) {
    CYCT(0x5955, 0x5956); ret_effect(gb);
    return;
  }
  CYC(0x5955, 0x5956);
  CYC(0x5956, 0x5958); A = 0x08;
  CYC(0x5958, 0x595b); W8(wActiveMusic2) = A;
  CYC(0x595b, 0x595c); ret_effect(gb);
}

void roomSpecificCodeA_hook(GB *gb) {
  CYC(0x595c, 0x595f); SET_HL(wMinimapGroup);
  CYC(0x595f, 0x5961); mem_wr(gb, HL, 0x00);
  CYC(0x5961, 0x5962); L = alu_inc8(gb, L);
  CYC(0x5962, 0x5964); mem_wr(gb, HL, 0x3c);
  CYC(0x5964, 0x5965); ret_effect(gb);
}

void roomSpecificCodeB_hook(GB *gb) {
  CYC(0x5965, 0x5968); SET_HL(wMinimapGroup);
  CYC(0x5968, 0x596a); mem_wr(gb, HL, 0x01);
  CYC(0x596a, 0x596b); L = alu_inc8(gb, L);
  CYC(0x596b, 0x596d); mem_wr(gb, HL, 0x3c);
  CYC(0x596d, 0x596e); ret_effect(gb);
}

void roomSpecificCodeC_hook(GB *gb) {
  CYC(0x596e, 0x5971); SET_HL(wActiveMusic);
  CYC(0x5971, 0x5972); A = mem_rd(gb, HL);
  CYC(0x5972, 0x5973); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5973, 0x5974); ret_effect(gb);
    return;
  }
  CYC(0x5973, 0x5974);
  CYC(0x5974, 0x5976); mem_wr(gb, HL, 0xff);
  CYC(0x5976, 0x5977); ret_effect(gb);
}
