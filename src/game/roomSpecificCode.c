#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t room_specific_code_jump_table(GB *gb) {
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

void runRoomSpecificCode_hook(GB *gb) {
  BASE(runRoomSpecificCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveRoom);
  CYC(b_+3, b_+6); SET_HL(SYM(roomSpecificCodeGroupTable));
  CALL_C(b_+6, findRoomSpecificData_hook, SYM(findRoomSpecificData), b_+9);
  if (!(F & FC)) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  hook_handoff(gb, room_specific_code_jump_table(gb));
}

void roomSpecificCode0_hook(GB *gb) {
  BASE(roomSpecificCode0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0e;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(b_+9, b_+11); B = 0x10;
  CYC(b_+11, b_+14); clearMemory_hook(gb);
}

void room_specific_code_create_spinner_hook(GB *gb) {
  BASE(roomSpecificCode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+6, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x7d);
  CYC(b_+12, b_+14); L = 0x4b;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x57);
  CYC(b_+16, b_+18); L = 0x4d;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x01);
  CYC(b_+20, b_+21); ret_effect(gb);
}

void roomSpecificCode1_hook(GB *gb) {
  BASE(roomSpecificCode1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0f;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6); room_specific_code_create_spinner_hook(gb);
}

void roomSpecificCode2_hook(GB *gb) {
  BASE(roomSpecificCode2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0f;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); room_specific_code_create_spinner_hook(gb);
}

void roomSpecificCode3_hook(GB *gb) {
  BASE(roomSpecificCode3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x24;
  CALL_C(b_+8, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+11);
  if (!(F & FC)) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_HL(wcc05);
  CYC(b_+15, b_+17); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 1));
  CALL_C(b_+17, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+20);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+21); ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x40);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x0a);
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+31); W8(wDiggingUpEnemiesForbidden) = A;
  CYC(b_+31, b_+32); ret_effect(gb);
}

void roomSpecificCode7_hook(GB *gb) {
  BASE(roomSpecificCode7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x15;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+9);
  CYC(b_+9, b_+11); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x35;
  CYC(b_+14, b_+17); W8(wActiveMusic2) = A;
  CYC(b_+17, b_+18); ret_effect(gb);
}

void roomSpecificCode5_hook(GB *gb) {
  BASE(roomSpecificCode5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x11;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x1f;
  CYC(b_+8, b_+11); W8(wActiveMusic2) = A;
  CYC(b_+11, b_+12); ret_effect(gb);
}

void roomSpecificCode4_hook(GB *gb) {
  BASE(roomSpecificCode4);
  CYC(b_+0, b_+2); A = 0x06;
  CYC(b_+2, b_+5); W8(wMinimapRoom) = A;
  CYC(b_+5, b_+8); SET_HL(wGroup1RoomFlags + 0x06);
  CYC(b_+8, b_+10); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 4));
  CYC(b_+10, b_+11); ret_effect(gb);
}

void roomSpecificCode8_hook(GB *gb) {
  BASE(roomSpecificCode8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wScreenTransitionDirection);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x40;
  CALL_C(b_+8, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+11);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x35;
  CYC(b_+14, b_+17); W8(wActiveMusic2) = A;
  CYC(b_+17, b_+18); ret_effect(gb);
}

void roomSpecificCode9_hook(GB *gb) {
  BASE(roomSpecificCode9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x14;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x08;
  CYC(b_+8, b_+11); W8(wActiveMusic2) = A;
  CYC(b_+11, b_+12); ret_effect(gb);
}

void roomSpecificCodeA_hook(GB *gb) {
  BASE(roomSpecificCodeA);
  CYC(b_+0, b_+3); SET_HL(wMinimapGroup);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x3c);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void roomSpecificCodeB_hook(GB *gb) {
  BASE(roomSpecificCodeB);
  CYC(b_+0, b_+3); SET_HL(wMinimapGroup);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x3c);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void roomSpecificCodeC_hook(GB *gb) {
  BASE(roomSpecificCodeC);
  CYC(b_+0, b_+3); SET_HL(wActiveMusic);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xff);
  CYC(b_+8, b_+9); ret_effect(gb);
}
