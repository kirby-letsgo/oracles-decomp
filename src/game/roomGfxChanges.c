#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void drawCollapsedWingDungeon_hook(GB *gb);
void roomTileChangesAfterLoad01_hook(GB *gb);
void roomTileChangesAfterLoad02_hook(GB *gb);
void roomTileChangesAfterLoad03_hook(GB *gb);
void roomTileChangesAfterLoad04_hook(GB *gb);
void drawRectangleToVramTiles_hook(GB *gb);
void drawRectangleToVramTiles_withParameters_hook(GB *gb);

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

static void add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t room_gfx_jump_table(GB *gb) {
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

static void draw_rectangle_to_vram_tiles_rows(GB *gb, uint16_t sp0_) {
  BASE(drawRectangleToVramTiles);
  for (;;) {
    CYC(b_+10, b_+11); push_effect(gb, BC);
    for (;;) {
      CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+12, b_+13); mem_wr(gb, DE, A);
      CYC(b_+13, b_+15); D |= 0x04;
      CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+16, b_+17); mem_wr(gb, DE, A);
      CYC(b_+17, b_+19); D &= (uint8_t)~0x04;
      CYC(b_+19, b_+20); SET_DE(DE + 1);
      CYC(b_+20, b_+21); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(b_+21, b_+23);
        continue;
      }
      CYC(b_+21, b_+23);
      break;
    }
    CYC(b_+23, b_+24); SET_BC(pop_effect(gb));
    CYC(b_+24, b_+26); A = 0x20;
    CYC(b_+26, b_+27); alu_sub(gb, C);
    CALL_C(b_+27, addAToDe_hook, 0x0068, b_+30);
    CYC(b_+30, b_+31); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+31, b_+33);
      continue;
    }
    CYC(b_+31, b_+33);
    break;
  }
  CYC(b_+33, b_+34); SET_AF(pop_effect(gb));
  CYC(b_+34, b_+36); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+36, b_+37); ret_effect(gb);
}

// 02:7a54
void getIndexOfGashaSpotInRoom_body_hook(GB *gb) {
  BASE(getIndexOfGashaSpotInRoom_body);
  CYC(b_+0, b_+2); C = 0;
  CYC(b_+2, b_+5); SET_HL(SYM(gashaSpotRooms));
  for (;;) {
    CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+6, b_+8);
      break;
    }
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); SET_HL(HL + 1);
    CYC(b_+9, b_+10); C = alu_inc8(gb, C);
    CYC(b_+10, b_+12);
  }
  CYC(b_+12, b_+13); A = C;
  CYC(b_+13, b_+16); SET_HL(wGashaSpotsPlantedBitset);
  CYC(b_+16, b_+19); TAIL(checkFlag);
}

// 02:7a77
void func_02_7a77_hook(GB *gb) {
  BASE(func_02_7a77);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkDungeonUsesToggleBlocks_hook, SYM(checkDungeonUsesToggleBlocks), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+7, b_+8); alu_or(gb, A);
  CYC(b_+8, b_+10); A = 0x3d;
  if (F & FZ) {
    CYCT(b_+10, b_+12);
  } else {
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+14); A = 0x3f;
  }
  CYC(b_+14, b_+17); TAIL(loadUncompressedGfxHeader);
}

void applyRoomSpecificTileChangesAfterGfxLoad_hook(GB *gb) {
  BASE(applyRoomSpecificTileChangesAfterGfxLoad);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(3)); A = W8(wActiveRoom);
  CYC(b_+O(3), b_+OE(6)); SET_HL(b_+O(34));
  CALL_C(b_+O(6), findRoomSpecificData_hook, SYM(findRoomSpecificData), b_+OE(9));
  if (!(F & FC)) {
    CYCT(b_+O(9), b_+OE(10)); ret_effect(gb);
    return;
  }
  CYC(b_+O(9), b_+OE(10));
  CYC(b_+O(10), b_+OE(11)); push_effect(gb, b_+OE(11));
  do { uint16_t jt_ = (room_gfx_jump_table(gb));
    if (jt_ == SYM(roomTileChangesAfterLoad00) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad00))) { roomTileChangesAfterLoad00_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad01) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad01))) { roomTileChangesAfterLoad01_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad02) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad02))) { roomTileChangesAfterLoad02_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad03) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad03))) { roomTileChangesAfterLoad03_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad04) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad04))) { roomTileChangesAfterLoad04_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad05) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad05))) { roomTileChangesAfterLoad05_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad06) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad06))) { roomTileChangesAfterLoad06_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad07) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad07))) { roomTileChangesAfterLoad07_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad08) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad08))) { roomTileChangesAfterLoad08_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad09) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad09))) { roomTileChangesAfterLoad09_hook(gb); return; }
    else if (jt_ == SYM(roomTileChangesAfterLoad0a) && hook_enabled_at(gb, SYM(roomTileChangesAfterLoad0a))) { roomTileChangesAfterLoad0a_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void roomTileChangesAfterLoad0a_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad0a);
  CYC(b_+0, b_+3); SET_HL(wRoomLayout + 0x79);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, HL);
    CYC(b_+4, b_+6); alu_sub(gb, 0x80);
    CYC(b_+6, b_+8); alu_cp(gb, 0x0a);
    if (!(F & FC)) {
      CYCT(b_+8, b_+10);
    } else {
      CYC(b_+8, b_+10);
      CYC(b_+10, b_+12); mem_wr(gb, HL, 0xf9);
    }
    CYC(b_+12, b_+13); L = alu_dec8(gb, L);
    if (!(F & FZ)) {
      CYCT(b_+13, b_+15);
      continue;
    }
    CYC(b_+13, b_+15);
    CYC(b_+15, b_+16); ret_effect(gb);
    return;
  }
}

void roomTileChangesAfterLoad09_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x14;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+12);
  CYC(b_+9, b_+12); TAIL(drawRectangleToVramTiles);
}

void roomTileChangesAfterLoad06_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, roomTileChangesAfterLoad0a_hook, SYM(roomTileChangesAfterLoad0a), b_+3);
  CALL_C(b_+3, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+6);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wGroup4RoomFlags + 0xfc);
  CYC(b_+10, b_+12); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+12, b_+13); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); A = 0xdc;
  CYC(b_+15, b_+18); mem_wr(gb, wRoomLayout + 0x57, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void roomTileChangesAfterLoad07_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(b_+12);
  CYC(b_+9, b_+12); TAIL(drawRectangleToVramTiles);
}

void drawCrownDungeonOpeningTiles_hook(GB *gb) {
  BASE(drawCrownDungeonOpeningTiles);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+4); SET_HL(b_+10);
  CYC(b_+4, b_+5); add_a_to_hl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); add_a_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+10); TAIL(drawRectangleToVramTiles);
}

void roomTileChangesAfterLoad00_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  TAIL(drawCollapsedWingDungeon);
}

void drawCollapsedWingDungeon_hook(GB *gb) {
  BASE(drawCollapsedWingDungeon);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x53;
  CALL_C(b_+2, loadGfxHeader_hook, SYM(loadGfxHeader), b_+5);
  CYC(b_+5, b_+8); SET_HL(b_+17);
  CALL_C(b_+8, copyRectangleFromTmpGfxBuffer_hook, SYM(copyRectangleFromTmpGfxBuffer), b_+11);
  CYC(b_+11, b_+14); SET_HL(b_+23);
  CYC(b_+14, b_+17); TAIL(copyRectangleToRoomLayoutAndCollisions);
}

void roomTileChangesAfterLoad02_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); TAIL(roomTileChangesAfterLoad01);
}

void roomTileChangesAfterLoad03_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  TAIL(roomTileChangesAfterLoad01);
}

void roomTileChangesAfterLoad01_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+6); SET_HL(SYM(treeGfxLocationsTable));
  CYC(b_+6, b_+7); add_double_index_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); A = W8(wActiveRoom);
  CYC(b_+13, b_+14); B = A;
  for (;;) {
    CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+15, b_+16); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+16, b_+17); ret_effect(gb);
      return;
    }
    CYC(b_+16, b_+17);
    CYC(b_+17, b_+18); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(b_+18, b_+20);
      break;
    }
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+21); SET_HL(HL + 1);
    CYC(b_+21, b_+22); SET_HL(HL + 1);
    CYC(b_+22, b_+23); SET_HL(HL + 1);
    CYC(b_+23, b_+25);
  }
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); D = mem_rd(gb, HL);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+33); mem_wr(gb, hFF93, A);
  CYC(b_+33, b_+36); SET_HL(SYM(treeTilesTable));
  CYC(b_+36, b_+37); add_double_index_to_hl_from_rst(gb, b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+39); H = mem_rd(gb, HL);
  CYC(b_+39, b_+40); L = A;
  CYC(b_+40, b_+43); SET_BC(0x0304);
  CALL_C(b_+43, drawRectangleToVramTiles_withParameters_hook, SYM(drawRectangleToVramTiles_withParameters), b_+46);
  CYC(b_+46, b_+48); A = mem_rd(gb, hFF93);
  CYC(b_+48, b_+50); alu_add(gb, 0x07);
  CYC(b_+50, b_+53); TAIL(loadTreeGfx);
}

void roomTileChangesAfterLoad08_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveRoom);
  CALL_C(b_+3, getIndexOfGashaSpotInRoom_body_hook, SYM(getIndexOfGashaSpotInRoom_body), b_+6);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = 0xd2;
  CALL_C(b_+9, findTileInRoom_hook, SYM(findTileInRoom), b_+12);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+13); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); E = L;
  CYC(b_+14, b_+16); D = 0xcf;
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+20); SET_HL(wGashaSpotKillCounters);
  CYC(b_+20, b_+21); add_a_to_hl_from_rst(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_cp(gb, 0x14);
  if (F & FC) {
    CYCT(b_+24, b_+26);
    CYC(b_+35, b_+38); SET_HL(b_+49);
  } else {
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+27); A = E;
    CYC(b_+27, b_+29); alu_sub(gb, 0x10);
    CYC(b_+29, b_+30); E = A;
    CYC(b_+30, b_+33); SET_HL(b_+53);
    CYC(b_+33, b_+35);
  }
  CALL_C(b_+38, copyRectangleToRoomLayoutAndCollisions_paramDe_hook, SYM(copyRectangleToRoomLayoutAndCollisions_paramDe), b_+41);
  CYC(b_+41, b_+44); SET_HL((SYM(pastMapTextIndices) + 126));
  CYC(b_+44, b_+46); E = 0x04;
  CYC(b_+46, b_+49); interBankCall_hook(gb);
}

void roomTileChangesAfterLoad05_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad05);
  CYC(b_+0, b_+3); SET_HL(wRoomLayout + 0x33);
  CYC(b_+3, b_+5); A = 0x0a;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); ret_effect(gb);
}

// 02:7d35
void readParametersForRectangleDrawing_hook(GB *gb) {
  BASE(readParametersForRectangleDrawing);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); E = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); D = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); ret_effect(gb);
}

void drawRectangleToVramTiles_withParameters_hook(GB *gb) {
  BASE(drawRectangleToVramTiles_withParameters);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, IO_SVBK - 0xff00);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+7, b_+9);
  draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void drawRectangleToVramTiles_hook(GB *gb) {
  BASE(drawRectangleToVramTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, IO_SVBK - 0xff00);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); hram_wr(gb, IO_SVBK - 0xff00, A);
  CALL_C(b_+7, readParametersForRectangleDrawing_hook, SYM(readParametersForRectangleDrawing), b_+10);
  draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void copyRectangleFromTmpGfxBuffer_paramBc_hook(GB *gb) {
  BASE(copyRectangleFromTmpGfxBuffer_paramBc);
  CYC(b_+0, b_+1); L = C;
  CYC(b_+1, b_+2); H = B;
  TAIL(copyRectangleFromTmpGfxBuffer);
}

// 02:7d6e
void copyRectangleFromTmpGfxBuffer_hook(GB *gb) {
  BASE(copyRectangleFromTmpGfxBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, IO_SVBK - 0xff00);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); E = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); D = A;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); H = mem_rd(gb, HL);
  CYC(b_+13, b_+14); L = A;

  for (;;) {
    CYC(b_+14, b_+15); push_effect(gb, BC);
    for (;;) {
      CYC(b_+15, b_+17); A = 0x02;
      CYC(b_+17, b_+19); hram_wr(gb, IO_SVBK - 0xff00, A);
      CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+20, b_+21); B = A;
      CYC(b_+21, b_+23); A = 0x03;
      CYC(b_+23, b_+25); hram_wr(gb, IO_SVBK - 0xff00, A);
      CYC(b_+25, b_+26); A = B;
      CYC(b_+26, b_+27); mem_wr(gb, DE, A);
      CYC(b_+27, b_+28); SET_DE(DE + 1);
      CYC(b_+28, b_+29); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(b_+29, b_+31);
        continue;
      }
      CYC(b_+29, b_+31);
      break;
    }
    CYC(b_+31, b_+32); SET_BC(pop_effect(gb));
    CYC(b_+32, b_+34); A = 0x20;
    CYC(b_+34, b_+35); alu_sub(gb, C);
    CALL_C(b_+35, addAToDe_hook, 0x0068, b_+38);
    CYC(b_+38, b_+40); A = 0x20;
    CYC(b_+40, b_+41); alu_sub(gb, C);
    CYC(b_+41, b_+42); add_a_to_hl_from_rst(gb, b_+42);
    CYC(b_+42, b_+43); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+43, b_+45);
      continue;
    }
    CYC(b_+43, b_+45);
    break;
  }
  CYC(b_+45, b_+46); SET_AF(pop_effect(gb));
  CYC(b_+46, b_+48); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+48, b_+49); ret_effect(gb);
}

void copyRectangleToRoomLayoutAndCollisions_paramDe_body_hook(GB *gb) {
  BASE(copyRectangleToRoomLayoutAndCollisions_paramDe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); B = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  for (;;) {
    CYC(b_+4, b_+5); push_effect(gb, BC);
    for (;;) {
      CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+6, b_+7); mem_wr(gb, DE, A);
      CYC(b_+7, b_+8); D = alu_dec8(gb, D);
      CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+9, b_+10); mem_wr(gb, DE, A);
      CYC(b_+10, b_+11); D = alu_inc8(gb, D);
      CYC(b_+11, b_+12); SET_DE(DE + 1);
      CYC(b_+12, b_+13); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(b_+13, b_+15);
        continue;
      }
      CYC(b_+13, b_+15);
      break;
    }
    CYC(b_+15, b_+16); SET_BC(pop_effect(gb));
    CYC(b_+16, b_+18); A = 0x10;
    CYC(b_+18, b_+19); alu_sub(gb, C);
    CALL_C(b_+19, addAToDe_hook, 0x0068, b_+22);
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

// 02:7d9f
void copyRectangleToRoomLayoutAndCollisions_hook(GB *gb) {
  BASE(copyRectangleToRoomLayoutAndCollisions);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); E = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); D = A;
  copyRectangleToRoomLayoutAndCollisions_paramDe_body_hook(gb);
}

// 02:7da3
void copyRectangleToRoomLayoutAndCollisions_paramDe_hook(GB *gb) {
  copyRectangleToRoomLayoutAndCollisions_paramDe_body_hook(gb);
}

void roomTileChangesAfterLoad04_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad04);
  CYC(b_+0, b_+3); SET_HL(wInShop);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, b_+10); TAIL(loadTreeGfx);
}

void checkLoadPastSignAndChestGfx_hook(GB *gb) {
  BASE(checkLoadPastSignAndChestGfx);
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wTilesetFlags);
  CYC(b_+9, b_+11); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+15); ret_effect(gb);
    return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+18); ret_effect(gb);
    return;
  }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x1c);
  if (F & FZ) {
    CYCT(b_+20, b_+21); ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); A = 0x37;
  CYC(b_+23, b_+26); TAIL(loadUncompressedGfxHeader);
}
