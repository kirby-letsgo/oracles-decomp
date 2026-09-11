#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

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
  for (;;) {
    CYC(0x7d51, 0x7d52); push_effect(gb, BC);
    for (;;) {
      CYC(0x7d52, 0x7d53); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7d53, 0x7d54); mem_wr(gb, DE, A);
      CYC(0x7d54, 0x7d56); D |= 0x04;
      CYC(0x7d56, 0x7d57); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7d57, 0x7d58); mem_wr(gb, DE, A);
      CYC(0x7d58, 0x7d5a); D &= (uint8_t)~0x04;
      CYC(0x7d5a, 0x7d5b); SET_DE(DE + 1);
      CYC(0x7d5b, 0x7d5c); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x7d5c, 0x7d5e);
        continue;
      }
      CYC(0x7d5c, 0x7d5e);
      break;
    }
    CYC(0x7d5e, 0x7d5f); SET_BC(pop_effect(gb));
    CYC(0x7d5f, 0x7d61); A = 0x20;
    CYC(0x7d61, 0x7d62); alu_sub(gb, C);
    CALL_C(0x7d62, addAToDe_hook, 0x0068, 0x7d65);
    CYC(0x7d65, 0x7d66); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x7d66, 0x7d68);
      continue;
    }
    CYC(0x7d66, 0x7d68);
    break;
  }
  CYC(0x7d68, 0x7d69); SET_AF(pop_effect(gb));
  CYC(0x7d69, 0x7d6b); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x7d6b, 0x7d6c); ret_effect(gb);
}

// 02:7a54
void getIndexOfGashaSpotInRoom_body_hook(GB *gb) {
  CYC(0x7a54, 0x7a56); C = 0;
  CYC(0x7a56, 0x7a59); SET_HL(0x7a67);
  for (;;) {
    CYC(0x7a59, 0x7a5a); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x7a5a, 0x7a5c);
      break;
    }
    CYC(0x7a5a, 0x7a5c);
    CYC(0x7a5c, 0x7a5d); SET_HL(HL + 1);
    CYC(0x7a5d, 0x7a5e); C = alu_inc8(gb, C);
    CYC(0x7a5e, 0x7a60);
  }
  CYC(0x7a60, 0x7a61); A = C;
  CYC(0x7a61, 0x7a64); SET_HL(wGashaSpotsPlantedBitset);
  CYC(0x7a64, 0x7a67); checkFlag_hook(gb);
}

// 02:7a77
void func_02_7a77_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7a77, checkDungeonUsesToggleBlocks_hook, 0x364b, 0x7a7a);
  if (F & FZ) {
    CYCT(0x7a7a, 0x7a7b); ret_effect(gb);
    return;
  }
  CYC(0x7a7a, 0x7a7b);
  CYC(0x7a7b, 0x7a7e); A = mem_rd(gb, wToggleBlocksState);
  CYC(0x7a7e, 0x7a7f); alu_or(gb, A);
  CYC(0x7a7f, 0x7a81); A = 0x3d;
  if (F & FZ) {
    CYCT(0x7a81, 0x7a83);
  } else {
    CYC(0x7a81, 0x7a83);
    CYC(0x7a83, 0x7a85); A = 0x3f;
  }
  CYC(0x7a85, 0x7a88); loadUncompressedGfxHeader_hook(gb);
}

void applyRoomSpecificTileChangesAfterGfxLoad_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a88, 0x7a8b); A = W8(wActiveRoom);
  CYC(0x7a8b, 0x7a8e); SET_HL(0x7aaa);
  CALL_C(0x7a8e, findRoomSpecificData_hook, 0x1dfe, 0x7a91);
  if (!(F & FC)) {
    CYCT(0x7a91, 0x7a92); ret_effect(gb);
    return;
  }
  CYC(0x7a91, 0x7a92);
  CYC(0x7a92, 0x7a93); push_effect(gb, 0x7a93);
  switch (room_gfx_jump_table(gb)) {
    case 0x7bfc: roomTileChangesAfterLoad00_hook(gb); return;
    case 0x7c3d: roomTileChangesAfterLoad01_hook(gb); return;
    case 0x7c2f: roomTileChangesAfterLoad02_hook(gb); return;
    case 0x7c37: roomTileChangesAfterLoad03_hook(gb); return;
    case 0x7dbd: roomTileChangesAfterLoad04_hook(gb); return;
    case 0x7d2b: roomTileChangesAfterLoad05_hook(gb); return;
    case 0x7b30: roomTileChangesAfterLoad06_hook(gb); return;
    case 0x7b43: roomTileChangesAfterLoad07_hook(gb); return;
    case 0x7cec: roomTileChangesAfterLoad08_hook(gb); return;
    case 0x7b14: roomTileChangesAfterLoad09_hook(gb); return;
    case 0x7b04: roomTileChangesAfterLoad0a_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void roomTileChangesAfterLoad0a_hook(GB *gb) {
  CYC(0x7b04, 0x7b07); SET_HL(wRoomLayout + 0x79);
  for (;;) {
    CYC(0x7b07, 0x7b08); A = mem_rd(gb, HL);
    CYC(0x7b08, 0x7b0a); alu_sub(gb, 0x80);
    CYC(0x7b0a, 0x7b0c); alu_cp(gb, 0x0a);
    if (!(F & FC)) {
      CYCT(0x7b0c, 0x7b0e);
    } else {
      CYC(0x7b0c, 0x7b0e);
      CYC(0x7b0e, 0x7b10); mem_wr(gb, HL, 0xf9);
    }
    CYC(0x7b10, 0x7b11); L = alu_dec8(gb, L);
    if (!(F & FZ)) {
      CYCT(0x7b11, 0x7b13);
      continue;
    }
    CYC(0x7b11, 0x7b13);
    CYC(0x7b13, 0x7b14); ret_effect(gb);
    return;
  }
}

void roomTileChangesAfterLoad09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b14, 0x7b16); A = 0x14;
  CALL_C(0x7b16, checkGlobalFlag_hook, 0x31f3, 0x7b19);
  if (F & FZ) {
    CYCT(0x7b19, 0x7b1a); ret_effect(gb);
    return;
  }
  CYC(0x7b19, 0x7b1a);
  CYC(0x7b1a, 0x7b1d); SET_HL(0x7b20);
  CYC(0x7b1d, 0x7b20); drawRectangleToVramTiles_hook(gb);
}

void roomTileChangesAfterLoad06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b30, roomTileChangesAfterLoad0a_hook, 0x7b04, 0x7b33);
  CALL_C(0x7b33, checkIsLinkedGame_hook, 0x1992, 0x7b36);
  if (F & FZ) {
    CYCT(0x7b36, 0x7b37); ret_effect(gb);
    return;
  }
  CYC(0x7b36, 0x7b37);
  CYC(0x7b37, 0x7b3a); A = mem_rd(gb, wGroup4RoomFlags + 0xfc);
  CYC(0x7b3a, 0x7b3c); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x7b3c, 0x7b3d); ret_effect(gb);
    return;
  }
  CYC(0x7b3c, 0x7b3d);
  CYC(0x7b3d, 0x7b3f); A = 0xdc;
  CYC(0x7b3f, 0x7b42); mem_wr(gb, wRoomLayout + 0x57, A);
  CYC(0x7b42, 0x7b43); ret_effect(gb);
}

void roomTileChangesAfterLoad07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7b43, getThisRoomFlags_hook, 0x197d, 0x7b46);
  CYC(0x7b46, 0x7b48); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x7b48, 0x7b49); ret_effect(gb);
    return;
  }
  CYC(0x7b48, 0x7b49);
  CYC(0x7b49, 0x7b4c); SET_HL(0x7b4f);
  CYC(0x7b4c, 0x7b4f); drawRectangleToVramTiles_hook(gb);
}

void drawCrownDungeonOpeningTiles_hook(GB *gb) {
  CYC(0x7b83, 0x7b84); A = C;
  CYC(0x7b84, 0x7b87); SET_HL(0x7b8d);
  CYC(0x7b87, 0x7b88); add_a_to_hl_from_rst(gb, 0x7b88);
  CYC(0x7b88, 0x7b89); A = mem_rd(gb, HL);
  CYC(0x7b89, 0x7b8a); add_a_to_hl_from_rst(gb, 0x7b8a);
  CYC(0x7b8a, 0x7b8d); drawRectangleToVramTiles_hook(gb);
}

void roomTileChangesAfterLoad00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7bfc, getThisRoomFlags_hook, 0x197d, 0x7bff);
  CYC(0x7bff, 0x7c01); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x7c01, 0x7c02); ret_effect(gb);
    return;
  }
  CYC(0x7c01, 0x7c02);
  drawCollapsedWingDungeon_hook(gb);
}

void drawCollapsedWingDungeon_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c02, 0x7c04); A = 0x53;
  CALL_C(0x7c04, loadGfxHeader_hook, 0x0626, 0x7c07);
  CYC(0x7c07, 0x7c0a); SET_HL(0x7c13);
  CALL_C(0x7c0a, copyRectangleFromTmpGfxBuffer_hook, 0x7d6e, 0x7c0d);
  CYC(0x7c0d, 0x7c10); SET_HL(0x7c19);
  CYC(0x7c10, 0x7c13); copyRectangleToRoomLayoutAndCollisions_hook(gb);
}

void roomTileChangesAfterLoad02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7c2f, getThisRoomFlags_hook, 0x197d, 0x7c32);
  CYC(0x7c32, 0x7c34); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x7c34, 0x7c35); ret_effect(gb);
    return;
  }
  CYC(0x7c34, 0x7c35);
  CYC(0x7c35, 0x7c37); roomTileChangesAfterLoad01_hook(gb);
}

void roomTileChangesAfterLoad03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7c37, getThisRoomFlags_hook, 0x197d, 0x7c3a);
  CYC(0x7c3a, 0x7c3c); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x7c3c, 0x7c3d); ret_effect(gb);
    return;
  }
  CYC(0x7c3c, 0x7c3d);
  roomTileChangesAfterLoad01_hook(gb);
}

void roomTileChangesAfterLoad01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c3d, 0x7c40); A = W8(wActiveGroup);
  CYC(0x7c40, 0x7c43); SET_HL(0x7c72);
  CYC(0x7c43, 0x7c44); add_double_index_to_hl_from_rst(gb, 0x7c44);
  CYC(0x7c44, 0x7c45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7c45, 0x7c46); H = mem_rd(gb, HL);
  CYC(0x7c46, 0x7c47); L = A;
  CYC(0x7c47, 0x7c4a); A = W8(wActiveRoom);
  CYC(0x7c4a, 0x7c4b); B = A;
  for (;;) {
    CYC(0x7c4b, 0x7c4c); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x7c4c, 0x7c4d); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x7c4d, 0x7c4e); ret_effect(gb);
      return;
    }
    CYC(0x7c4d, 0x7c4e);
    CYC(0x7c4e, 0x7c4f); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(0x7c4f, 0x7c51);
      break;
    }
    CYC(0x7c4f, 0x7c51);
    CYC(0x7c51, 0x7c52); SET_HL(HL + 1);
    CYC(0x7c52, 0x7c53); SET_HL(HL + 1);
    CYC(0x7c53, 0x7c54); SET_HL(HL + 1);
    CYC(0x7c54, 0x7c56);
  }
  CYC(0x7c56, 0x7c57); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7c57, 0x7c58); B = A;
  CYC(0x7c58, 0x7c59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7c59, 0x7c5a); D = mem_rd(gb, HL);
  CYC(0x7c5a, 0x7c5b); E = A;
  CYC(0x7c5b, 0x7c5c); A = B;
  CYC(0x7c5c, 0x7c5e); hram_wr(gb, 0x93, A);
  CYC(0x7c5e, 0x7c61); SET_HL(0x7c9c);
  CYC(0x7c61, 0x7c62); add_double_index_to_hl_from_rst(gb, 0x7c62);
  CYC(0x7c62, 0x7c63); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7c63, 0x7c64); H = mem_rd(gb, HL);
  CYC(0x7c64, 0x7c65); L = A;
  CYC(0x7c65, 0x7c68); SET_BC(0x0304);
  CALL_C(0x7c68, drawRectangleToVramTiles_withParameters_hook, 0x7d3e, 0x7c6b);
  CYC(0x7c6b, 0x7c6d); A = hram_rd(gb, 0x93);
  CYC(0x7c6d, 0x7c6f); alu_add(gb, 0x07);
  CYC(0x7c6f, 0x7c72); loadTreeGfx_hook(gb);
}

void roomTileChangesAfterLoad08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7cec, 0x7cef); A = W8(wActiveRoom);
  CALL_C(0x7cef, getIndexOfGashaSpotInRoom_body_hook, 0x7a54, 0x7cf2);
  if (F & FZ) {
    CYCT(0x7cf2, 0x7cf3); ret_effect(gb);
    return;
  }
  CYC(0x7cf2, 0x7cf3);
  CYC(0x7cf3, 0x7cf5); A = 0xd2;
  CALL_C(0x7cf5, findTileInRoom_hook, 0x15cc, 0x7cf8);
  if (!(F & FZ)) {
    CYCT(0x7cf8, 0x7cf9); ret_effect(gb);
    return;
  }
  CYC(0x7cf8, 0x7cf9);
  CYC(0x7cf9, 0x7cfa); E = L;
  CYC(0x7cfa, 0x7cfc); D = 0xcf;
  CYC(0x7cfc, 0x7cfd); A = C;
  CYC(0x7cfd, 0x7d00); SET_HL(wGashaSpotKillCounters);
  CYC(0x7d00, 0x7d01); add_a_to_hl_from_rst(gb, 0x7d01);
  CYC(0x7d01, 0x7d02); A = mem_rd(gb, HL);
  CYC(0x7d02, 0x7d04); alu_cp(gb, 0x14);
  if (F & FC) {
    CYCT(0x7d04, 0x7d06);
    CYC(0x7d0f, 0x7d12); SET_HL(0x7d1d);
  } else {
    CYC(0x7d04, 0x7d06);
    CYC(0x7d06, 0x7d07); A = E;
    CYC(0x7d07, 0x7d09); alu_sub(gb, 0x10);
    CYC(0x7d09, 0x7d0a); E = A;
    CYC(0x7d0a, 0x7d0d); SET_HL(0x7d21);
    CYC(0x7d0d, 0x7d0f);
  }
  CALL_C(0x7d12, copyRectangleToRoomLayoutAndCollisions_paramDe_hook, 0x7da3, 0x7d15);
  CYC(0x7d15, 0x7d18); SET_HL(0x6bf1);
  CYC(0x7d18, 0x7d1a); E = 0x04;
  CYC(0x7d1a, 0x7d1d); interBankCall_hook(gb);
}

void roomTileChangesAfterLoad05_hook(GB *gb) {
  CYC(0x7d2b, 0x7d2e); SET_HL(wRoomLayout + 0x33);
  CYC(0x7d2e, 0x7d30); A = 0x0a;
  CYC(0x7d30, 0x7d31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7d31, 0x7d32); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7d32, 0x7d33); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7d33, 0x7d34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7d34, 0x7d35); ret_effect(gb);
}

// 02:7d35
void readParametersForRectangleDrawing_hook(GB *gb) {
  CYC(0x7d35, 0x7d36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d36, 0x7d37); E = A;
  CYC(0x7d37, 0x7d38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d38, 0x7d39); D = A;
  CYC(0x7d39, 0x7d3a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d3a, 0x7d3b); B = A;
  CYC(0x7d3b, 0x7d3c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d3c, 0x7d3d); C = A;
  CYC(0x7d3d, 0x7d3e); ret_effect(gb);
}

void drawRectangleToVramTiles_withParameters_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d3e, 0x7d40); A = hram_rd(gb, IO_SVBK - 0xff00);
  CYC(0x7d40, 0x7d41); push_effect(gb, AF);
  CYC(0x7d41, 0x7d43); A = 0x03;
  CYC(0x7d43, 0x7d45); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x7d45, 0x7d47);
  draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void drawRectangleToVramTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d47, 0x7d49); A = hram_rd(gb, IO_SVBK - 0xff00);
  CYC(0x7d49, 0x7d4a); push_effect(gb, AF);
  CYC(0x7d4a, 0x7d4c); A = 0x03;
  CYC(0x7d4c, 0x7d4e); hram_wr(gb, IO_SVBK - 0xff00, A);
  CALL_C(0x7d4e, readParametersForRectangleDrawing_hook, 0x7d35, 0x7d51);
  draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void copyRectangleFromTmpGfxBuffer_paramBc_hook(GB *gb) {
  CYC(0x7d6c, 0x7d6d); L = C;
  CYC(0x7d6d, 0x7d6e); H = B;
  copyRectangleFromTmpGfxBuffer_hook(gb);
}

// 02:7d6e
void copyRectangleFromTmpGfxBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d6e, 0x7d70); A = hram_rd(gb, IO_SVBK - 0xff00);
  CYC(0x7d70, 0x7d71); push_effect(gb, AF);
  CYC(0x7d71, 0x7d72); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d72, 0x7d73); B = A;
  CYC(0x7d73, 0x7d74); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d74, 0x7d75); C = A;
  CYC(0x7d75, 0x7d76); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d76, 0x7d77); E = A;
  CYC(0x7d77, 0x7d78); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d78, 0x7d79); D = A;
  CYC(0x7d79, 0x7d7a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7d7a, 0x7d7b); H = mem_rd(gb, HL);
  CYC(0x7d7b, 0x7d7c); L = A;

  for (;;) {
    CYC(0x7d7c, 0x7d7d); push_effect(gb, BC);
    for (;;) {
      CYC(0x7d7d, 0x7d7f); A = 0x02;
      CYC(0x7d7f, 0x7d81); hram_wr(gb, IO_SVBK - 0xff00, A);
      CYC(0x7d81, 0x7d82); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7d82, 0x7d83); B = A;
      CYC(0x7d83, 0x7d85); A = 0x03;
      CYC(0x7d85, 0x7d87); hram_wr(gb, IO_SVBK - 0xff00, A);
      CYC(0x7d87, 0x7d88); A = B;
      CYC(0x7d88, 0x7d89); mem_wr(gb, DE, A);
      CYC(0x7d89, 0x7d8a); SET_DE(DE + 1);
      CYC(0x7d8a, 0x7d8b); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x7d8b, 0x7d8d);
        continue;
      }
      CYC(0x7d8b, 0x7d8d);
      break;
    }
    CYC(0x7d8d, 0x7d8e); SET_BC(pop_effect(gb));
    CYC(0x7d8e, 0x7d90); A = 0x20;
    CYC(0x7d90, 0x7d91); alu_sub(gb, C);
    CALL_C(0x7d91, addAToDe_hook, 0x0068, 0x7d94);
    CYC(0x7d94, 0x7d96); A = 0x20;
    CYC(0x7d96, 0x7d97); alu_sub(gb, C);
    CYC(0x7d97, 0x7d98); add_a_to_hl_from_rst(gb, 0x7d98);
    CYC(0x7d98, 0x7d99); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x7d99, 0x7d9b);
      continue;
    }
    CYC(0x7d99, 0x7d9b);
    break;
  }
  CYC(0x7d9b, 0x7d9c); SET_AF(pop_effect(gb));
  CYC(0x7d9c, 0x7d9e); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x7d9e, 0x7d9f); ret_effect(gb);
}

void copyRectangleToRoomLayoutAndCollisions_paramDe_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7da3, 0x7da4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7da4, 0x7da5); B = A;
  CYC(0x7da5, 0x7da6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7da6, 0x7da7); C = A;
  for (;;) {
    CYC(0x7da7, 0x7da8); push_effect(gb, BC);
    for (;;) {
      CYC(0x7da8, 0x7da9); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7da9, 0x7daa); mem_wr(gb, DE, A);
      CYC(0x7daa, 0x7dab); D = alu_dec8(gb, D);
      CYC(0x7dab, 0x7dac); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7dac, 0x7dad); mem_wr(gb, DE, A);
      CYC(0x7dad, 0x7dae); D = alu_inc8(gb, D);
      CYC(0x7dae, 0x7daf); SET_DE(DE + 1);
      CYC(0x7daf, 0x7db0); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x7db0, 0x7db2);
        continue;
      }
      CYC(0x7db0, 0x7db2);
      break;
    }
    CYC(0x7db2, 0x7db3); SET_BC(pop_effect(gb));
    CYC(0x7db3, 0x7db5); A = 0x10;
    CYC(0x7db5, 0x7db6); alu_sub(gb, C);
    CALL_C(0x7db6, addAToDe_hook, 0x0068, 0x7db9);
    CYC(0x7db9, 0x7dba); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x7dba, 0x7dbc);
      continue;
    }
    CYC(0x7dba, 0x7dbc);
    break;
  }
  CYC(0x7dbc, 0x7dbd); ret_effect(gb);
}

// 02:7d9f
void copyRectangleToRoomLayoutAndCollisions_hook(GB *gb) {
  CYC(0x7d9f, 0x7da0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7da0, 0x7da1); E = A;
  CYC(0x7da1, 0x7da2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7da2, 0x7da3); D = A;
  copyRectangleToRoomLayoutAndCollisions_paramDe_body_hook(gb);
}

// 02:7da3
void copyRectangleToRoomLayoutAndCollisions_paramDe_hook(GB *gb) {
  copyRectangleToRoomLayoutAndCollisions_paramDe_body_hook(gb);
}

void roomTileChangesAfterLoad04_hook(GB *gb) {
  CYC(0x7dbd, 0x7dc0); SET_HL(wInShop);
  CYC(0x7dc0, 0x7dc2); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(0x7dc2, 0x7dc4); A = 0x03;
  CYC(0x7dc4, 0x7dc7); loadTreeGfx_hook(gb);
}

void checkLoadPastSignAndChestGfx_hook(GB *gb) {
  CYC(0x7dc7, 0x7dca); A = W8(wDungeonIndex);
  CYC(0x7dca, 0x7dcc); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x7dcc, 0x7dcd); ret_effect(gb);
    return;
  }
  CYC(0x7dcc, 0x7dcd);
  CYC(0x7dcd, 0x7dd0); A = W8(wTilesetFlags);
  CYC(0x7dd0, 0x7dd2); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x7dd2, 0x7dd3); ret_effect(gb);
    return;
  }
  CYC(0x7dd2, 0x7dd3);
  CYC(0x7dd3, 0x7dd5); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x7dd5, 0x7dd6); ret_effect(gb);
    return;
  }
  CYC(0x7dd5, 0x7dd6);
  CYC(0x7dd6, 0x7dd8); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x7dd8, 0x7dd9); ret_effect(gb);
    return;
  }
  CYC(0x7dd8, 0x7dd9);
  CYC(0x7dd9, 0x7ddb); alu_and(gb, 0x1c);
  if (F & FZ) {
    CYCT(0x7ddb, 0x7ddc); ret_effect(gb);
    return;
  }
  CYC(0x7ddb, 0x7ddc);
  CYC(0x7ddc, 0x7dde); A = 0x37;
  CYC(0x7dde, 0x7de1); loadUncompressedGfxHeader_hook(gb);
}
