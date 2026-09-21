#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void tileReplacement_group2Map7e_hook(GB *gb);
void replaceVineTiles_hook(GB *gb);

static void tileReplacement_group5Mapb9_write_tiles(GB *gb);
static void tileReplacement_group1Map27_write_tiles(GB *gb);
void createInteraction90_hook(GB *gb);
void setTileToDoor_hook(GB *gb);
void replaceTiles_hook(GB *gb);
void tileReplacement_group5Mapc3_hook(GB *gb);
void tileReplacement_group5Mapc3__locFunc_hook(GB *gb);
void tileReplacement_group5Mapc3__func_04_672e_hook(GB *gb);
void tileReplacement_group2Mapf7_hook(GB *gb);

static uint16_t room_tile_changes_jump_table(GB *gb) {
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

void applyRoomSpecificTileChanges_hook(GB *gb) {
  BASE(applyRoomSpecificTileChanges);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveRoom);
  CYC(b_+3, b_+6); SET_HL(SYM(roomTileChangerCodeGroupTable));
  CALL_C(b_+6, findRoomSpecificData_hook, SYM(findRoomSpecificData), b_+9);
  if (!(F & FC)) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  hook_handoff(gb, room_tile_changes_jump_table(gb));
}

static void add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  pop_effect(gb);
}

void tileReplacement_group1Map58_hook(GB *gb) {
  BASE(tileReplacement_group1Map58);
  CYC(b_+0, b_+2); A = mem_rd(gb, hGameboyType);
  CYC(b_+2, b_+3); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL(wRoomLayout + 0x35);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xde);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void tileReplacement_group5Mapf5_hook(GB *gb) {
  BASE(tileReplacement_group5Mapf5);
  CYC(b_+0, b_+3); A = W8(wTwinrovaTileReplacementMode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    goto fill_with_lava;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto fill_with_ice;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+12, b_+14);
    goto normal_layout;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); W8(wTwinrovaTileReplacementMode) = A;
  CYC(b_+18, b_+21); SET_HL(b_+24);
  CYC(b_+21, b_+24); fillRectInRoomLayout_hook(gb);
  return;
normal_layout:
  CYC(b_+28, b_+31); W8(wTwinrovaTileReplacementMode) = A;
  CYC(b_+31, b_+33); A = 0xb9;
  CYC(b_+33, b_+36); loadGfxHeader_hook(gb);
  return;
fill_with_ice:
  CYC(b_+36, b_+39); W8(wTwinrovaTileReplacementMode) = A;
  CYC(b_+39, b_+42); SET_HL(b_+45);
  CYC(b_+42, b_+45); fillRectInRoomLayout_hook(gb);
  return;
fill_with_lava:
  CYC(b_+49, b_+52); W8(wTwinrovaTileReplacementMode) = A;
  CYC(b_+52, b_+54); A = 0xb8;
  CYC(b_+54, b_+57); loadGfxHeader_hook(gb);
}

void tileReplacement_group4Map1b_hook(GB *gb) {
  BASE(tileReplacement_group4Map1b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x1a);
  CYC(b_+9, b_+11); A = 0x09;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  tileReplacement_group2Map7e_hook(gb);
}

void tileReplacement_group2Map7e_hook(GB *gb) {
  BASE(tileReplacement_group2Map7e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wBoughtShopItems1);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); SET_HL(b_+28);
  CALL_C(b_+11, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+14);
  CYC(b_+14, b_+16); A = 0xf1;
  CYC(b_+16, b_+19); SET_HL(wRoomLayout + 0x25);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = 0x27;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+25); L = 0x32;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xa0);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void tileReplacement_group4Mapc9_hook(GB *gb) {
  BASE(tileReplacement_group4Mapc9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x27);
  CYC(b_+9, b_+11); A = 0x6d;
  CYC(b_+11, b_+14); set4Bytes_hook(gb);
}

void tileReplacement_group4Mapc7_hook(GB *gb) {
  BASE(tileReplacement_group4Mapc7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wSwitchState);
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x27);
  CYC(b_+9, b_+11); A = 0x6d;
  CALL_C(b_+11, set4Bytes_hook, SYM(set4Bytes), b_+14);
  CYC(b_+14, b_+16); A = 0xf4;
  CYC(b_+16, b_+18); L = 0x3d;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = 0x4d;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); L = 0x5d;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); L = 0x6d;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void tileReplacement_group4Map4c_hook(GB *gb) {
  BASE(tileReplacement_group4Map4c);
  CYC(b_+0, b_+3); SET_HL(wSwitchState);
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x43);
  CYC(b_+9, b_+11); A = 0x6a;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = 0x53;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = 0x63;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = 0x76;
  CYC(b_+20, b_+22); A = 0xf4;
  CYC(b_+22, b_+25); set4Bytes_hook(gb);
}

void tileReplacement_group4Map4e_hook(GB *gb) {
  BASE(tileReplacement_group4Map4e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wSwitchState);
  CYC(b_+3, b_+5); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x36);
  CYC(b_+9, b_+11); A = 0x6d;
  CALL_C(b_+11, set4Bytes_hook, SYM(set4Bytes), b_+14);
  CYC(b_+14, b_+16); A = 0xf4;
  CYC(b_+16, b_+18); L = 0x42;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = 0x52;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); L = 0x62;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); L = 0x4c;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); L = 0x5c;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = 0x6c;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); ret_effect(gb);
}

void tileReplacement_group4Map59_hook(GB *gb) {
  BASE(tileReplacement_group4Map59);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(b_+12);
  CYC(b_+9, b_+12); replaceTiles_hook(gb);
}

void tileReplacement_group4Map60_hook(GB *gb) {
  BASE(tileReplacement_group4Map60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0f;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+43);
  CALL_C(b_+9, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+12);
  CALL_C(b_+12, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x20);
  CYC(b_+17, b_+19); A = 0xf0;
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto write_tiles;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);
write_tiles:
  CYC(b_+22, b_+25); SET_HL(wRoomLayout + 0x57);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+28); L = 0x34;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x1d);
  CYC(b_+30, b_+32); L = 0x3a;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x1d);
  CYC(b_+34, b_+36); L = 0x74;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x1d);
  CYC(b_+38, b_+40); L = 0x7a;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x1d);
  CYC(b_+42, b_+43); ret_effect(gb);
}

void tileReplacement_group4Map52_hook(GB *gb) {
  BASE(tileReplacement_group4Map52);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0f;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x60;
  CYC(b_+8, b_+11); W8(wLoadingRoom) = A;
  CYC(b_+11, b_+14); SET_HL(SYM(loadRoomLayout));
  CYC(b_+14, b_+16); E = 0x00;
  CALL_C(b_+16, interBankCall_hook, 0x008a, b_+19);
  CYC(b_+19, b_+20); ret_effect(gb);
}

static void tileReplacement_group0And1Map38_common(GB *gb) {
  BASE(tileReplacement_group1Map38);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x73);
  CYC(b_+9, b_+11); A = 0xf9;
  CYC(b_+11, b_+14); set4Bytes_hook(gb);
}

void tileReplacement_group0Map38_hook(GB *gb) {
  BASE(tileReplacement_group0Map38);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x12;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8);
  tileReplacement_group0And1Map38_common(gb);
}

void tileReplacement_group1Map38_hook(GB *gb) {
  BASE(tileReplacement_group1Map38);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  tileReplacement_group0And1Map38_common(gb);
}

void tileReplacement_group0Map48_hook(GB *gb) {
  BASE(tileReplacement_group0Map48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x12;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x03);
  CYC(b_+9, b_+11); A = 0x3a;
  CYC(b_+11, b_+14); set4Bytes_hook(gb);
}

void tileReplacement_group5Map38_hook(GB *gb) {
  BASE(tileReplacement_group5Map38);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x6a;
  CYC(b_+8, b_+11); SET_HL(wRoomLayout + 0x39);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = 0x49;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = 0x59;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = 0x69;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void tileReplacement_group5Map25And43_common_hook(GB *gb) {
  BASE(tileReplacement_group5Map43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+13, b_+16); SET_HL(b_+25);
  CALL_C(b_+16, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+19);
  CYC(b_+19, b_+22); SET_HL(b_+29);
  CYC(b_+22, b_+25); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map25_hook(GB *gb) {
  BASE(tileReplacement_group5Map25);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(SYM(d6RetractingWallRectPresent));
  CALL_C(b_+9, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+12);
  CYC(b_+12, b_+14);
  tileReplacement_group5Map25And43_common_hook(gb);
}

void tileReplacement_group5Map43_hook(GB *gb) {
  BASE(tileReplacement_group5Map43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    CYC(b_+33, b_+36); SET_DE(b_+39);
    CYC(b_+36, b_+39); replaceTiles_hook(gb);
    return;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(SYM(d6RetractingWallRectPast));
  CALL_C(b_+10, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+13);
  tileReplacement_group5Map25And43_common_hook(gb);
}

void tileReplacement_group5Map95_hook(GB *gb) {
  BASE(tileReplacement_group5Map95);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x4d);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xb4);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0xb2);
  CYC(b_+14, b_+17); SET_HL(b_+26);
  CALL_C(b_+17, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+20);
  CYC(b_+20, b_+23); SET_HL(b_+30);
  CYC(b_+23, b_+26); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map4c_hook(GB *gb) {
  BASE(tileReplacement_group5Map4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wJabuWaterLevel);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+17);
  CALL_C(b_+9, fillRectInRoomLayout_hook, SYM(fillRectInRoomLayout), b_+12);
  CYC(b_+12, b_+14); L = 0x57;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x45);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void tileReplacement_group5Map4d_hook(GB *gb) {
  BASE(tileReplacement_group5Map4d);
  CYC(b_+0, b_+3); A = W8(wJabuWaterLevel);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+12);
  CYC(b_+9, b_+12); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map5c_hook(GB *gb) {
  BASE(tileReplacement_group5Map5c);
  CYC(b_+0, b_+3); A = W8(wDungeonFloor);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(wJabuWaterLevel);
  CYC(b_+7, b_+9); alu_and(gb, 0x07);
  CYC(b_+9, b_+10); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_DE(b_+17);
  CYC(b_+14, b_+17); drawRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map5d_hook(GB *gb) {
  BASE(tileReplacement_group5Map5d);
  CYC(b_+0, b_+3); A = W8(wDungeonFloor);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(wJabuWaterLevel);
  CYC(b_+7, b_+9); alu_and(gb, 0x07);
  CYC(b_+9, b_+10); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_DE(b_+17);
  CYC(b_+14, b_+17); drawRectInRoomLayout_hook(gb);
}

void tileReplacement_group7Map4a_hook(GB *gb) {
  BASE(tileReplacement_group7Map4a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+12);
  CYC(b_+9, b_+12); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group0Map5c_hook(GB *gb) {
  BASE(tileReplacement_group0Map5c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x34);
  CYC(b_+9, b_+11); A = 0x3a;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = 0x43;
  CYC(b_+14, b_+17); set3Bytes_hook(gb);
}

void tileReplacement_group0Map73_hook(GB *gb) {
  BASE(tileReplacement_group0Map73);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x73);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x3a);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x10);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x11);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x12);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x3a);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void tileReplacement_group0Mapac_hook(GB *gb) {
  BASE(tileReplacement_group0Mapac);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x33);
  CYC(b_+9, b_+11); A = 0xaf;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x43;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+18); ret_effect(gb);
}

void tileReplacement_group0Map2c_hook(GB *gb) {
  BASE(tileReplacement_group0Map2c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0017);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+9);
    TAIL(setTileToWitheredVine);
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); L = 0x06;
  CYC(b_+11, b_+14); SET_DE(b_+17);
  CYC(b_+14, b_+17); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Map1c_hook(GB *gb) {
  BASE(tileReplacement_group0Map1c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0017);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x66;
  CYC(b_+9, b_+12); SET_DE(b_+15);
  CYC(b_+12, b_+15); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapba_hook(GB *gb) {
  BASE(tileReplacement_group0Mapba);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(unsetFlag));
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+9);
    TAIL(setTileToWitheredVine);
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); L = 0x07;
  CYC(b_+11, b_+14); SET_DE(b_+23);
  CALL_C(b_+14, replaceVineTiles_hook, SYM(replaceVineTiles), b_+17);
  CYC(b_+17, b_+19); A = 0x8b;
  CYC(b_+19, b_+22); mem_wr(gb, wRoomLayout + 0x18, A);
  CYC(b_+22, b_+26); ret_effect(gb);
}

void tileReplacement_group0Mapaa_hook(GB *gb) {
  BASE(tileReplacement_group0Mapaa);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(unsetFlag));
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x77;
  CYC(b_+9, b_+12); SET_DE(b_+15);
  CYC(b_+12, b_+15); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapcc_hook(GB *gb) {
  BASE(tileReplacement_group0Mapcc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0311);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+9);
    TAIL(setTileToWitheredVine);
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); L = 0x00;
  CYC(b_+11, b_+14); SET_DE(b_+17);
  CYC(b_+14, b_+17); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapbc_hook(GB *gb) {
  BASE(tileReplacement_group0Mapbc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0311);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x70;
  CYC(b_+9, b_+12); SET_DE(b_+15);
  CYC(b_+12, b_+15); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapda_hook(GB *gb) {
  BASE(tileReplacement_group0Mapda);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0418);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+9);
    TAIL(setTileToWitheredVine);
  }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); L = 0x07;
  CYC(b_+11, b_+14); SET_DE(b_+17);
  CYC(b_+14, b_+17); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapca_hook(GB *gb) {
  BASE(tileReplacement_group0Mapca);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0418);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x77;
  CYC(b_+9, b_+12); SET_DE(b_+15);
  CYC(b_+12, b_+15); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Map61_hook(GB *gb) {
  BASE(tileReplacement_group0Map61);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0122);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    goto vine1;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_BC(0x0127);
  CALL_C(b_+11, getVinePosition_hook, SYM(getVinePosition), b_+14);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+17);
    TAIL(setTileToWitheredVine);
  }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+20); SET_HL(wRoomLayout + 0x06);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x4d);
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0xd5);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x55);
  CYC(b_+28, b_+30); L = 0x16;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x5d);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xd6);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x60);
  CYC(b_+38, b_+40); L = 0x27;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x8d);
  CYC(b_+42, b_+43); ret_effect(gb);
  return;
vine1:
  CYC(b_+43, b_+46); SET_HL(wRoomLayout + 0x01);
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x56);
  CYC(b_+48, b_+49); L = alu_inc8(gb, L);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0xd5);
  CYC(b_+51, b_+52); L = alu_inc8(gb, L);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x4d);
  CYC(b_+54, b_+56); L = 0x11;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x61);
  CYC(b_+58, b_+59); L = alu_inc8(gb, L);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0xd6);
  CYC(b_+61, b_+62); L = alu_inc8(gb, L);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x5d);
  CYC(b_+64, b_+66); L = 0x22;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x8d);
  CYC(b_+68, b_+69); ret_effect(gb);
}

void tileReplacement_group0Map51_hook(GB *gb) {
  BASE(tileReplacement_group0Map51);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0122);
  CALL_C(b_+3, getVinePosition_hook, SYM(getVinePosition), b_+6);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    goto vines1;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_BC(0x0127);
  CALL_C(b_+11, getVinePosition_hook, SYM(getVinePosition), b_+14);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+18); SET_HL(wRoomLayout + 0x76);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x5b);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xd4);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x45);
  CYC(b_+26, b_+27); ret_effect(gb);
  return;
vines1:
  CYC(b_+27, b_+30); SET_HL(wRoomLayout + 0x71);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x46);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xd4);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x5c);
  CYC(b_+38, b_+39); ret_effect(gb);
}

void replaceVineTiles_hook(GB *gb) {
  BASE(replaceVineTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); H = wRoomLayout >> 8;
  for (;;) {
    CYC(b_+2, b_+3); A = mem_rd(gb, DE);
    CYC(b_+3, b_+4); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+4, b_+6);
      break;
    }
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+7); A = mem_rd(gb, DE);
    CYC(b_+7, b_+8); SET_DE(DE + 1);
    CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+9, b_+10); L = alu_inc8(gb, L);
    CYC(b_+10, b_+11); A = mem_rd(gb, DE);
    CYC(b_+11, b_+12); SET_DE(DE + 1);
    CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+13, b_+15); A = 0x0d;
    CYC(b_+15, b_+16); add_a_to_hl_from_rst(gb, b_+16);
    CYC(b_+16, b_+18);
  }
  CYC(b_+18, b_+21); SET_DE(b_+37);
  CALL_C(b_+21, replaceTiles_hook, SYM(replaceTiles), b_+24);
  CYC(b_+24, b_+26); A = 0xd6;
  CALL_C(b_+26, findTileInRoom_hook, SYM(findTileInRoom), b_+29);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+30); ret_effect(gb);
    return;
  }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); A = L;
  CYC(b_+31, b_+33); alu_add(gb, 0x10);
  CYC(b_+33, b_+34); L = A;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x8d);
  CYC(b_+36, b_+37); ret_effect(gb);
}

void setTileToWitheredVine_hook(GB *gb) {
  BASE(setTileToWitheredVine);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); L = mem_rd(gb, HL);
  CYC(b_+1, b_+3); H = 0xcf;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  CALL_C(b_+5, retrieveTileCollisionValue_hook, SYM(retrieveTileCollisionValue), b_+8);
  CYC(b_+8, b_+9); SET_HL(pop_effect(gb));
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x8c);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void getVinePosition_hook(GB *gb) {
  BASE(getVinePosition);
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+4); SET_HL((wGroup1RoomFlags + 240));
  CYC(b_+4, b_+5); add_a_to_hl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_cp(gb, C);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void set3Bytes_hook(GB *gb);

void initializeVinePositions_hook(GB *gb) {
  BASE(initializeVinePositions);
  CYC(b_+0, b_+3); SET_HL(wGroup1RoomFlags + 0xf0);
  CYC(b_+3, b_+6); SET_DE(b_+11);
  CYC(b_+6, b_+8); B = 0x06;
  CYC(b_+8, b_+11); copyMemoryReverse_hook(gb);
}

void tileReplacement_group0Map54_hook(GB *gb) {
  BASE(tileReplacement_group0Map54);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wSwitchState) = A;
  CALL_C(b_+4, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x1d;
  CYC(b_+12, b_+15); SET_HL(wRoomLayout + 0x43);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+20); A = 0x1e;
  CYC(b_+20, b_+22); L = 0x53;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); A = 0x9e;
  CYC(b_+27, b_+30); mem_wr(gb, wRoomLayout + 0x68, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void tileReplacement_group0Map25_hook(GB *gb) {
  BASE(tileReplacement_group0Map25);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x25;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x1d;
  CYC(b_+8, b_+11); SET_HL(wRoomLayout + 0x50);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+16); A = 0x1e;
  CYC(b_+16, b_+19); SET_HL(wRoomLayout + 0x60);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void tileReplacement_group0Map3a_hook(GB *gb) {
  BASE(tileReplacement_group0Map3a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0a;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0xee;
  CYC(b_+8, b_+11); mem_wr(gb, wRoomLayout + 0x23, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void tileReplacement_group0Map0b_hook(GB *gb) {
  BASE(tileReplacement_group0Map0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wGroup0RoomFlags + 0x0a);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x43);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xdd);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void tileReplacement_group5Mapb9_hook(GB *gb) {
  BASE(tileReplacement_group5Mapb9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+5); SET_HL(wEssencesObtained);
  CALL_C(b_+5, checkFlag_hook, SYM(checkFlag), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_BC(b_+33);
  CYC(b_+12, b_+15); SET_HL(wRoomLayout + 0x41);
  CYC(b_+15, b_+18); push_effect(gb, b_+18);
  tileReplacement_group5Mapb9_write_tiles(gb);
  CYC(b_+18, b_+20); L = 0x51;
  tileReplacement_group5Mapb9_write_tiles(gb);
}

static void tileReplacement_group5Mapb9_write_tiles(GB *gb) {
  BASE(tileReplacement_group5Mapb9);
  CYC(b_+20, b_+22); A = 0x05;
  for (;;) {
    CYC(b_+22, b_+24); mem_wr(gb, hFF8D, A);
    CYC(b_+24, b_+25); A = mem_rd(gb, BC);
    CYC(b_+25, b_+26); SET_BC(BC + 1);
    CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+27, b_+29); A = mem_rd(gb, hFF8D);
    CYC(b_+29, b_+30); A = alu_dec8(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+30, b_+32);
      continue;
    }
    CYC(b_+30, b_+32);
    break;
  }
  CYC(b_+32, b_+33); ret_effect(gb);
}

void tileReplacement_group1Map27_hook(GB *gb) {
  BASE(tileReplacement_group1Map27);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); L = 0x15;
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+7, b_+9);
    goto next_flag_17;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_DE(0x3343);
  CYC(b_+12, b_+15); push_effect(gb, b_+15);
  tileReplacement_group1Map27_write_tiles(gb);
next_flag_17:
  CYC(b_+15, b_+17); L = 0x17;
  CYC(b_+17, b_+19); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+19, b_+21);
    goto next_flag_35;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); SET_DE(0x3424);
  CYC(b_+24, b_+27); push_effect(gb, b_+27);
  tileReplacement_group1Map27_write_tiles(gb);
next_flag_35:
  CYC(b_+27, b_+29); L = 0x35;
  CYC(b_+29, b_+31); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+31, b_+33);
    goto next_flag_37;
  }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); SET_DE(0x3545);
  CYC(b_+36, b_+39); push_effect(gb, b_+39);
  tileReplacement_group1Map27_write_tiles(gb);
next_flag_37:
  CYC(b_+39, b_+41); L = 0x37;
  CYC(b_+41, b_+43); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+43, b_+44); ret_effect(gb);
    return;
  }
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+47); SET_DE(0x3626);
  tileReplacement_group1Map27_write_tiles(gb);
}

static void tileReplacement_group1Map27_write_tiles(GB *gb) {
  BASE(tileReplacement_group1Map27);
  CYC(b_+47, b_+49); B = wRoomLayout >> 8;
  CYC(b_+49, b_+50); C = D;
  CYC(b_+50, b_+52); A = 0x3a;
  CYC(b_+52, b_+53); mem_wr(gb, BC, A);
  CYC(b_+53, b_+54); C = E;
  CYC(b_+54, b_+56); A = 0x02;
  CYC(b_+56, b_+57); mem_wr(gb, BC, A);
  CYC(b_+57, b_+58); ret_effect(gb);
}

void tileReplacement_group5Mapc2_hook(GB *gb) {
  BASE(tileReplacement_group5Mapc2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x56);
  CYC(b_+9, b_+11); A = 0x6d;
  set4Bytes_hook(gb);
}

void set4Bytes_hook(GB *gb) {
  BASE(set4Bytes);
  CYC(b_+0, b_+1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  set3Bytes_hook(gb);
}

void set3Bytes_hook(GB *gb) {
  BASE(set3Bytes);
  CYC(b_+0, b_+1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1, b_+2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+2, b_+3); mem_wr(gb, HL, A);
  CYC(b_+3, b_+4); ret_effect(gb);
}

void tileReplacement_group5Mape3_hook(GB *gb) {
  BASE(tileReplacement_group5Mape3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x26);
  CYC(b_+9, b_+11); A = 0x6d;
  CYC(b_+11, b_+13); set3Bytes_hook(gb);
}

void tileReplacement_group2Map90_hook(GB *gb) {
  BASE(tileReplacement_group2Map90);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_DE(b_+12);
  CYC(b_+9, b_+12); drawRectInRoomLayout_hook(gb);
}

void tileReplacement_group1Map8c_hook(GB *gb) {
  BASE(tileReplacement_group1Map8c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x04);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x30);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x32);
  CYC(b_+14, b_+16); A = 0x3a;
  CYC(b_+16, b_+18); L = 0x14;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = 0x34;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x02);
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x3a);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void tileReplacement_group2Map9e_hook(GB *gb) {
  BASE(tileReplacement_group2Map9e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wToggleBlocksState) = A;
  CALL_C(b_+4, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+9, b_+10); ret_effect(gb);
    return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); SET_HL(wRoomLayout + 0x13);
  CYC(b_+13, b_+15); A = 0x6d;
  CALL_C(b_+15, set3Bytes_hook, SYM(set3Bytes), b_+18);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+22); set3Bytes_hook(gb);
}

void tileReplacement_group0Mape0_hook(GB *gb) {
  BASE(tileReplacement_group0Mape0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wEssencesObtained);
  CYC(b_+3, b_+5); alu_bit(gb, 4, A);
  CYC(b_+5, b_+7); L = 0x46;
  if (!(F & FZ)) {
    CALL_C_CC(b_+7, setTileToDoor_hook, SYM(setTileToDoor), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  CYC(b_+10, b_+12); C = 0x1b;
  createInteraction90_hook(gb);
}

void createInteraction90_hook(GB *gb) {
  BASE(createInteraction90);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x90);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, C);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void tileReplacement_group0Mape1_hook(GB *gb) {
  BASE(tileReplacement_group0Mape1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x1c;
  CALL_C(b_+2, createInteraction90_hook, SYM(createInteraction90), b_+5);
  CYC(b_+5, b_+8); A = W8(wEssencesObtained);
  CYC(b_+8, b_+9); alu_rrca(gb);
  CYC(b_+9, b_+11); L = 0x26;
  if (F & FC) {
    CALL_C_CC(b_+11, setTileToDoor_hook, SYM(setTileToDoor), b_+14);
  } else {
    CYC(b_+11, b_+14);
  }
  CYC(b_+14, b_+15); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); L = 0x53;
  CYC(b_+18, b_+20); setTileToDoor_hook(gb);
}

void tileReplacement_group0Mape2_hook(GB *gb) {
  BASE(tileReplacement_group0Mape2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x1d;
  CALL_C(b_+2, createInteraction90_hook, SYM(createInteraction90), b_+5);
  CYC(b_+5, b_+8); A = W8(wEssencesObtained);
  CYC(b_+8, b_+10); alu_bit(gb, 2, A);
  if (F & FZ) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = 0x54;
  setTileToDoor_hook(gb);
}

void setTileToDoor_hook(GB *gb) {
  BASE(setTileToDoor);
  CYC(b_+0, b_+2); H = wRoomLayout >> 8;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0xdd);
  CYC(b_+4, b_+5); ret_effect(gb);
}

void tileReplacement_group4Mapea_hook(GB *gb) {
  BASE(tileReplacement_group4Mapea);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0xa3;
  CYC(b_+8, b_+11); SET_HL(wRoomLayout + 0x33);
  CALL_C(b_+11, set3Bytes_hook, SYM(set3Bytes), b_+14);
  CYC(b_+14, b_+16); L = 0x39;
  CALL_C(b_+16, set3Bytes_hook, SYM(set3Bytes), b_+19);
  CYC(b_+19, b_+21); A = 0xb7;
  CYC(b_+21, b_+23); L = 0x43;
  CALL_C(b_+23, set3Bytes_hook, SYM(set3Bytes), b_+26);
  CYC(b_+26, b_+28); L = 0x49;
  CALL_C(b_+28, set3Bytes_hook, SYM(set3Bytes), b_+31);
  CYC(b_+31, b_+33); A = 0x88;
  CYC(b_+33, b_+35); L = 0x53;
  CALL_C(b_+35, set3Bytes_hook, SYM(set3Bytes), b_+38);
  CYC(b_+38, b_+40); L = 0x59;
  CYC(b_+40, b_+43); set3Bytes_hook(gb);
}

void tileReplacement_group0Map98_hook(GB *gb) {
  BASE(tileReplacement_group0Map98);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wRickyState);
  CYC(b_+3, b_+5); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto remove_dirt;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+9, b_+11);
    goto remove_dirt;
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x48;
  CALL_C(b_+13, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+16);
  if (!(F & FC)) {
    CYCT(b_+16, b_+17); ret_effect(gb);
    return;
  }
  CYC(b_+16, b_+17);
remove_dirt:
  CYC(b_+17, b_+19); A = 0x3a;
  CYC(b_+19, b_+22); mem_wr(gb, wRoomLayout + 0x24, A);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void tileReplacement_group0Map76_hook(GB *gb) {
  BASE(tileReplacement_group0Map76);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, getBlackTowerProgress_hook, SYM(getBlackTowerProgress), b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+9); ret_effect(gb);
    return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_HL(wRoomLayout + 0x54);
  CYC(b_+12, b_+14); A = 0xa7;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void tileReplacement_group0Mapa5_hook(GB *gb) {
  BASE(tileReplacement_group0Mapa5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGroup1RoomFlags + 0xa5);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wRoomLayout + 0x22);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xee);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0xef);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void func_04_6ba8_hook(GB *gb) {
  BASE(func_04_6ba8);
  CYC(b_+0, b_+2); D = wRoomLayout >> 8;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  for (;;) {
    CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+5, b_+7); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(b_+7, b_+8); ret_effect(gb);
      return;
    }
    CYC(b_+7, b_+8);
    CYC(b_+8, b_+9); E = A;
    CYC(b_+9, b_+10); A = C;
    CYC(b_+10, b_+11); mem_wr(gb, DE, A);
    CYC(b_+11, b_+13);
  }
}

void fillRectInRoomLayout_hook(GB *gb) {
  BASE(fillRectInRoomLayout);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); E = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); D = A;
  CYC(b_+8, b_+10); H = 0xcf;
  for (;;) {
    CYC(b_+10, b_+11); A = D;
    CYC(b_+11, b_+12); L = E;
    CYC(b_+12, b_+13); push_effect(gb, BC);
    for (;;) {
      CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+14, b_+15); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(b_+15, b_+17);
        continue;
      }
      CYC(b_+15, b_+17);
      break;
    }
    CYC(b_+17, b_+18); A = E;
    CYC(b_+18, b_+20); alu_add(gb, 0x10);
    CYC(b_+20, b_+21); E = A;
    CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
    CYC(b_+22, b_+23); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+23, b_+25);
      continue;
    }
    CYC(b_+23, b_+25);
    break;
  }
  CYC(b_+25, b_+26); ret_effect(gb);
}

void drawRectInRoomLayout_hook(GB *gb) {
  BASE(drawRectInRoomLayout);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); SET_DE(DE + 1);
  CYC(b_+2, b_+4); H = 0xcf;
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+7); mem_wr(gb, hFF8B, A);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); SET_DE(DE + 1);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); SET_DE(DE + 1);
  CYC(b_+12, b_+14); mem_wr(gb, hFF8D, A);
  for (;;) {
    CYC(b_+14, b_+16); A = mem_rd(gb, hFF8D);
    CYC(b_+16, b_+17); B = A;
    for (;;) {
      CYC(b_+17, b_+18); A = mem_rd(gb, DE);
      CYC(b_+18, b_+19); SET_DE(DE + 1);
      CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+20, b_+21); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(b_+21, b_+23);
        continue;
      }
      CYC(b_+21, b_+23);
      break;
    }
    CYC(b_+23, b_+25); A = mem_rd(gb, hFF8B);
    CYC(b_+25, b_+27); alu_add(gb, 0x10);
    CYC(b_+27, b_+29); mem_wr(gb, hFF8B, A);
    CYC(b_+29, b_+30); L = A;
    CYC(b_+30, b_+31); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+31, b_+33);
      continue;
    }
    CYC(b_+31, b_+33);
    break;
  }
  CYC(b_+33, b_+34); ret_effect(gb);
}

static void tile_replacement_group5_mapc3(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(tileReplacement_group5Mapc3);
  if (entry == b_+25) goto loc_func;
  if (entry == b_+53) goto remove_wall;

  CALL_C(b_+0, tileReplacement_group5Mapc3__func_04_672e_hook, b_+53, b_+3);
  CALL_C(b_+3, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_BC(b_+38);
  CYC(b_+12, b_+15); SET_HL(wRoomLayout + 0x31);
  CALL_C(b_+15, tileReplacement_group5Mapc3__locFunc_hook, b_+25, b_+18);
  CYC(b_+18, b_+20); L = 0x41;
  CALL_C(b_+20, tileReplacement_group5Mapc3__locFunc_hook, b_+25, b_+23);
  CYC(b_+23, b_+25); L = 0x51;
loc_func:
  CYC(b_+25, b_+27); A = 0x05;
  for (;;) {
    CYC(b_+27, b_+29); H8(hFF8D) = A;
    CYC(b_+29, b_+30); A = mem_rd(gb, BC);
    CYC(b_+30, b_+31); SET_BC(BC + 1);
    CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+32, b_+34); A = H8(hFF8D);
    CYC(b_+34, b_+35); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+35, b_+37); continue; }
    CYC(b_+35, b_+37);
    break;
  }
  CYC(b_+37, b_+38); ret_effect(gb);
  return;

remove_wall:
  CYC(b_+53, b_+55); A = 0x04;
  CYC(b_+55, b_+58); SET_HL(wEssencesObtained);
  CALL_C(b_+58, checkFlag_hook, SYM(checkFlag), b_+61);
  if (F & FZ) { CYCT(b_+61, b_+62); ret_effect(gb); return; }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+65); SET_HL(b_+98);
  CYC(b_+65, b_+68); SET_BC(wRoomLayout + 0x06);
  CYC(b_+68, b_+70); A = 0x04;
  for (;;) {
    CYC(b_+70, b_+72); H8(hFF8D) = A;
    CYC(b_+72, b_+74); A = 0x04;
    for (;;) {
      CYC(b_+74, b_+76); H8(hFF8C) = A;
      CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+77, b_+78); alu_or(gb, A);
      if (F & FZ) CYCT(b_+78, b_+80);
      else {
        CYC(b_+78, b_+80);
        CYC(b_+80, b_+81); mem_wr(gb, BC, A);
      }
      CYC(b_+81, b_+82); SET_BC(BC + 1);
      CYC(b_+82, b_+84); A = H8(hFF8C);
      CYC(b_+84, b_+85); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+85, b_+87); continue; }
      CYC(b_+85, b_+87);
      break;
    }
    CYC(b_+87, b_+89); A = 0x0c;
    CALL_C(b_+89, addAToBc_hook, 0x006d, b_+92);
    CYC(b_+92, b_+94); A = H8(hFF8D);
    CYC(b_+94, b_+95); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+95, b_+97); continue; }
    CYC(b_+95, b_+97);
    break;
  }
  CYC(b_+97, b_+98); ret_effect(gb);
}

void tileReplacement_group5Mapc3_hook(GB *gb) { tile_replacement_group5_mapc3(gb, SYM(tileReplacement_group5Mapc3), gb->sp); }
void tileReplacement_group5Mapc3__locFunc_hook(GB *gb) { tile_replacement_group5_mapc3(gb, SYM(tileReplacement_group5Mapc3__locFunc), gb->sp); }
void tileReplacement_group5Mapc3__func_04_672e_hook(GB *gb) { tile_replacement_group5_mapc3(gb, SYM(tileReplacement_group5Mapc3__func_04_672e), gb->sp); }

void tileReplacement_group2Mapf7_hook(GB *gb) {
  BASE(tileReplacement_group2Mapf7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto insert_wall; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0xf0;
  CYC(b_+9, b_+11); C = 0x14;
  CALL_C(b_+11, setTile_hook, SYM(setTile), b_+14);
  CYC(b_+14, b_+16); A = 0xf0;
  CYC(b_+16, b_+18); C = 0x16;
  CYC(b_+18, b_+21); setTile_hook(gb);
  return;

insert_wall:
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_and(gb, 0xc0);
  CYC(b_+24, b_+26); alu_cp(gb, 0xc0);
  if (F & FZ) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+29, b_+31); goto write_wall; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = W8(wSeedTreeRefilledBitset);
  CYC(b_+34, b_+36); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
write_wall:
  CYC(b_+37, b_+40); SET_HL(b_+70);
  CYC(b_+40, b_+43); SET_BC(wRoomLayout + 0x03);
  CYC(b_+43, b_+45); A = 0x04;
  for (;;) {
    CYC(b_+45, b_+47); H8(hFF8D) = A;
    CYC(b_+47, b_+49); A = 0x05;
    for (;;) {
      CYC(b_+49, b_+51); H8(hFF8C) = A;
      CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+52, b_+53); mem_wr(gb, BC, A);
      CYC(b_+53, b_+54); SET_BC(BC + 1);
      CYC(b_+54, b_+56); A = H8(hFF8C);
      CYC(b_+56, b_+57); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+57, b_+59); continue; }
      CYC(b_+57, b_+59);
      break;
    }
    CYC(b_+59, b_+61); A = 0x0b;
    CALL_C(b_+61, addAToBc_hook, 0x006d, b_+64);
    CYC(b_+64, b_+66); A = H8(hFF8D);
    CYC(b_+66, b_+67); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+67, b_+69); continue; }
    CYC(b_+67, b_+69);
    break;
  }
  CYC(b_+69, b_+70); ret_effect(gb);
}
