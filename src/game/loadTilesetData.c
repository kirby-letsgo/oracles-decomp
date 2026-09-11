#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

void checkTilesetOverride_hook(GB *gb);

static void load_tileset_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static void load_tileset_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void load_tileset_data(GB *gb, uint16_t sp0_) {
  CYC(0x6d94, 0x6d96); alu_and(gb, 0x80);
  CYC(0x6d96, 0x6d98); H8(hFF8B) = A;
  CYC(0x6d98, 0x6d9a); A = H8(hFF8D);
  CYC(0x6d9a, 0x6d9c); alu_and(gb, 0x7f);
  CALL_C(0x6d9c, multiplyABy8_hook, 0x01b7, 0x6d9f);
  CYC(0x6d9f, 0x6da2); SET_HL(0x4f9c);
  CYC(0x6da2, 0x6da3); alu_add_hl(gb, BC);
  CYC(0x6da3, 0x6da4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6da4, 0x6da5); E = A;
  CYC(0x6da5, 0x6da6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6da6, 0x6da9); W8(wTilesetFlags) = A;
  CYC(0x6da9, 0x6dab); alu_bit(gb, 3, A);
  if (F & FZ) {
    CYCT(0x6dab, 0x6dad);
    CYC(0x6db5, 0x6db7); A = 0xff;
    CYC(0x6db7, 0x6dba); W8(wDungeonIndex) = A;
  } else {
    CYC(0x6dab, 0x6dad);
    CYC(0x6dad, 0x6dae); A = E;
    CYC(0x6dae, 0x6db0); alu_and(gb, 0x0f);
    CYC(0x6db0, 0x6db3); W8(wDungeonIndex) = A;
    CYC(0x6db3, 0x6db5);
  }
  CYC(0x6dba, 0x6dbb); A = E;
  CYC(0x6dbb, 0x6dbd); A = alu_swap(gb, A);
  CYC(0x6dbd, 0x6dbf); alu_and(gb, 0x07);
  CYC(0x6dbf, 0x6dc2); W8(wActiveCollisions) = A;
  CYC(0x6dc2, 0x6dc4); B = 0x06;
  CYC(0x6dc4, 0x6dc7); SET_DE(wTilesetUniqueGfx);
  do {
    CYC(0x6dc7, 0x6dc8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6dc8, 0x6dc9); mem_wr(gb, DE, A);
    CYC(0x6dc9, 0x6dca); E = alu_inc8(gb, E);
    CYC(0x6dca, 0x6dcb); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x6dcb, 0x6dcd);
    else CYC(0x6dcb, 0x6dcd);
  } while (!(F & FZ));
  CYC(0x6dcd, 0x6dcf); E = 0x20;
  CYC(0x6dcf, 0x6dd0); A = mem_rd(gb, DE);
  CYC(0x6dd0, 0x6dd1); B = A;
  CYC(0x6dd1, 0x6dd3); A = H8(hFF8B);
  CYC(0x6dd3, 0x6dd4); alu_or(gb, B);
  CYC(0x6dd4, 0x6dd5); mem_wr(gb, DE, A);
  CYC(0x6dd5, 0x6dd6); ret_effect(gb);
}

void loadTilesetData_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d7a, getAdjustedRoomGroup_hook, 0x6dd6, 0x6d7d);
  CYC(0x6d7d, 0x6d80); SET_HL(0x52d4);
  CYC(0x6d80, 0x6d81); load_tileset_add_double_index_to_hl_from_rst(gb, 0x6d81);
  CYC(0x6d81, 0x6d82); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d82, 0x6d83); H = mem_rd(gb, HL);
  CYC(0x6d83, 0x6d84); L = A;
  CYC(0x6d84, 0x6d87); A = W8(wActiveRoom);
  CYC(0x6d87, 0x6d88); load_tileset_add_a_to_hl_from_rst(gb, 0x6d88);
  CYC(0x6d88, 0x6d89); A = mem_rd(gb, HL);
  CYC(0x6d89, 0x6d8b); H8(hFF8D) = A;
  CYC(0x6d8b, 0x6d8e); push_effect(gb, 0x6d8e);
  load_tileset_data(gb, sp0_);
  CALL_C(0x6d8e, checkTilesetOverride_hook, 0x6de7, 0x6d91);
  if (!(F & FC)) {
    CYCT(0x6d91, 0x6d92); ret_effect(gb);
    return;
  }
  CYC(0x6d91, 0x6d92);
  CYC(0x6d92, 0x6d94); A = H8(hFF8D);
  load_tileset_data(gb, sp0_);
}

void getAdjustedRoomGroup_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;

  CYC(0x6dd6, 0x6dd9); A = mem_rd(gb, wActiveGroup);
  CYC(0x6dd9, 0x6dda); B = A;
  CYC(0x6dda, 0x6ddc); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(0x6ddc, 0x6ddd); ret_effect(gb);
    return;
  }
  CYC(0x6ddc, 0x6ddd);

  CALL_C(0x6ddd, getThisRoomFlags_hook, 0x197d, 0x6de0);
  CYC(0x6de0, 0x6de1); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x6de1, 0x6de3);
  } else {
    CYC(0x6de1, 0x6de3);
    CYC(0x6de3, 0x6de5); B |= 0x02;
  }
  CYC(0x6de5, 0x6de6); A = B;
  CYC(0x6de6, 0x6de7); ret_effect(gb);
}

void checkTilesetOverride_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_ROM(0x6de7, 0x6e0d);
  if (F & FC) {
    CYCT(0x6dea, 0x6deb); ret_effect(gb);
    return;
  }
  CYC(0x6dea, 0x6deb);
  CALL_ROM(0x6deb, 0x6e28);
  if (F & FC) {
    CYCT(0x6dee, 0x6def); ret_effect(gb);
    return;
  }
  CYC(0x6dee, 0x6def);
  CYC(0x6def, 0x6df2); A = W8(wActiveGroup);
  CYC(0x6df2, 0x6df3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6df3, 0x6df5);
    goto no_change;
  }
  CYC(0x6df3, 0x6df5);
  CYC(0x6df5, 0x6df8); A = W8(wLoadingRoomPack);
  CYC(0x6df8, 0x6dfa); alu_cp(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(0x6dfa, 0x6dfc);
    goto no_change;
  }
  CYC(0x6dfa, 0x6dfc);
  CYC(0x6dfc, 0x6dff); A = W8(wAnimalCompanion);
  CYC(0x6dff, 0x6e01); alu_sub(gb, 0x0b);
  if (F & FZ) {
    CYCT(0x6e01, 0x6e03);
    goto no_change;
  }
  CYC(0x6e01, 0x6e03);
  CYC(0x6e03, 0x6e04); B = A;
  CYC(0x6e04, 0x6e06); A = H8(hFF8D);
  CYC(0x6e06, 0x6e07); alu_add(gb, B);
  CYC(0x6e07, 0x6e09); H8(hFF8D) = A;
  CYC(0x6e09, 0x6e0a); alu_scf(gb);
  CYC(0x6e0a, 0x6e0b); ret_effect(gb);
  return;

no_change:
  CYC(0x6e0b, 0x6e0c); alu_xor(gb, A);
  CYC(0x6e0c, 0x6e0d); ret_effect(gb);
}

static void add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L); L = A;
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void setPastCliffPalettesToRed_hook(GB *gb) {
  CYC(0x6e63, 0x6e66); A = W8(wActiveCollisions);
  CYC(0x6e66, 0x6e67); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6e67, 0x6e69);
    goto done;
  }
  CYC(0x6e67, 0x6e69);
  CYC(0x6e69, 0x6e6c); A = W8(wTilesetFlags);
  CYC(0x6e6c, 0x6e6e); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6e6e, 0x6e70);
    goto done;
  }
  CYC(0x6e6e, 0x6e70);
  CYC(0x6e70, 0x6e73); A = W8(wActiveRoom);
  CYC(0x6e73, 0x6e75); alu_cp(gb, 0x38);
  if (F & FZ) {
    CYCT(0x6e75, 0x6e76); ret_effect(gb);
    return;
  }
  CYC(0x6e75, 0x6e76);
  CYC(0x6e76, 0x6e78); A = 0x03;
  CYC(0x6e78, 0x6e7a); mem_wr(gb, 0xff70, A);
  CYC(0x6e7a, 0x6e7d); SET_HL(w3TileMappingData + 0x204);
  CYC(0x6e7d, 0x6e7f); D = 0x06;
  for (;;) {
    CYC(0x6e7f, 0x6e81); B = 0x04;
    for (;;) {
      CYC(0x6e81, 0x6e82); A = mem_rd(gb, HL);
      CYC(0x6e82, 0x6e84); alu_and(gb, 0x07);
      CYC(0x6e84, 0x6e85); alu_cp(gb, D);
      if (!(F & FZ)) {
        CYCT(0x6e85, 0x6e87);
      } else {
        CYC(0x6e85, 0x6e87);
        CYC(0x6e87, 0x6e88); A = mem_rd(gb, HL);
        CYC(0x6e88, 0x6e8a); alu_and(gb, 0xf8);
        CYC(0x6e8a, 0x6e8b); mem_wr(gb, HL, A);
      }
      CYC(0x6e8b, 0x6e8c); SET_HL(HL + 1);
      CYC(0x6e8c, 0x6e8d); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(0x6e8d, 0x6e8f);
        continue;
      }
      CYC(0x6e8d, 0x6e8f);
      break;
    }
    CYC(0x6e8f, 0x6e91); A = 0x04;
    CYC(0x6e91, 0x6e92); push_effect(gb, 0x6e92);
    add_a_to_hl(gb);
    CYC(0x6e92, 0x6e93); A = H;
    CYC(0x6e93, 0x6e95); alu_cp(gb, 0xd4);
    if (F & FC) {
      CYCT(0x6e95, 0x6e97);
      continue;
    }
    CYC(0x6e95, 0x6e97);
    break;
  }
done:
  CYC(0x6e97, 0x6e98); alu_xor(gb, A);
  CYC(0x6e98, 0x6e9a); mem_wr(gb, 0xff70, A);
  CYC(0x6e9a, 0x6e9b); ret_effect(gb);
}

void func_04_6e9b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e9b, 0x6e9d); A = 0x02;
  CYC(0x6e9d, 0x6e9f); mem_wr(gb, 0xff70, A);
  CYC(0x6e9f, 0x6ea2); SET_HL(wRoomLayout);
  CYC(0x6ea2, 0x6ea5); SET_DE(w2TmpGfxBuffer);
  CYC(0x6ea5, 0x6ea7); B = 0xc0;
  CALL_C(0x6ea7, copyMemory_hook, 0x0486, 0x6eaa);
  CYC(0x6eaa, 0x6ead); SET_HL(wRoomCollisions);
  CYC(0x6ead, 0x6eb0); SET_DE(w2TmpGfxBuffer + 0x100);
  CYC(0x6eb0, 0x6eb2); B = 0xc0;
  CALL_C(0x6eb2, copyMemory_hook, 0x0486, 0x6eb5);
  CYC(0x6eb5, 0x6eb8); SET_HL(w3RoomLayoutBuffer);
  CYC(0x6eb8, 0x6ebb); SET_DE(w2TmpGfxBuffer + 0x200);
  CYC(0x6ebb, 0x6ebd); B = 0xc0;
  for (;;) {
    CYC(0x6ebd, 0x6ebf); A = 0x03;
    CYC(0x6ebf, 0x6ec1); mem_wr(gb, 0xff70, A);
    CYC(0x6ec1, 0x6ec2); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6ec2, 0x6ec3); C = A;
    CYC(0x6ec3, 0x6ec5); A = 0x02;
    CYC(0x6ec5, 0x6ec7); mem_wr(gb, 0xff70, A);
    CYC(0x6ec7, 0x6ec8); A = C;
    CYC(0x6ec8, 0x6ec9); mem_wr(gb, DE, A);
    CYC(0x6ec9, 0x6eca); SET_DE(DE + 1);
    CYC(0x6eca, 0x6ecb); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x6ecb, 0x6ecd);
      continue;
    }
    CYC(0x6ecb, 0x6ecd);
    break;
  }
  CYC(0x6ecd, 0x6ece); alu_xor(gb, A);
  CYC(0x6ece, 0x6ed0); mem_wr(gb, 0xff70, A);
  CYC(0x6ed0, 0x6ed1); ret_effect(gb);
}

void func_04_6ed1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ed1, 0x6ed3); A = 0x02;
  CYC(0x6ed3, 0x6ed5); mem_wr(gb, 0xff70, A);
  CYC(0x6ed5, 0x6ed8); SET_HL(wRoomLayout);
  CYC(0x6ed8, 0x6edb); SET_DE(w2TmpGfxBuffer);
  CYC(0x6edb, 0x6edd); B = 0xc0;
  CALL_C(0x6edd, copyMemoryReverse_hook, 0x047f, 0x6ee0);
  CYC(0x6ee0, 0x6ee3); SET_HL(wRoomCollisions);
  CYC(0x6ee3, 0x6ee6); SET_DE(w2TmpGfxBuffer + 0x100);
  CYC(0x6ee6, 0x6ee8); B = 0xc0;
  CALL_C(0x6ee8, copyMemoryReverse_hook, 0x047f, 0x6eeb);
  CYC(0x6eeb, 0x6eee); SET_HL(w3RoomLayoutBuffer);
  CYC(0x6eee, 0x6ef1); SET_DE(w2TmpGfxBuffer + 0x200);
  CYC(0x6ef1, 0x6ef3); B = 0xc0;
  for (;;) {
    CYC(0x6ef3, 0x6ef5); A = 0x02;
    CYC(0x6ef5, 0x6ef7); mem_wr(gb, 0xff70, A);
    CYC(0x6ef7, 0x6ef8); A = mem_rd(gb, DE);
    CYC(0x6ef8, 0x6ef9); SET_DE(DE + 1);
    CYC(0x6ef9, 0x6efa); C = A;
    CYC(0x6efa, 0x6efc); A = 0x03;
    CYC(0x6efc, 0x6efe); mem_wr(gb, 0xff70, A);
    CYC(0x6efe, 0x6eff); A = C;
    CYC(0x6eff, 0x6f00); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x6f00, 0x6f01); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x6f01, 0x6f03);
      continue;
    }
    CYC(0x6f01, 0x6f03);
    break;
  }
  CYC(0x6f03, 0x6f04); alu_xor(gb, A);
  CYC(0x6f04, 0x6f06); mem_wr(gb, 0xff70, A);
  CYC(0x6f06, 0x6f07); ret_effect(gb);
}

static void func_04_6f07_locfunc(GB *gb) {
  for (;;) {
    CYC(0x6f1c, 0x6f1e); A = 0x03;
    CYC(0x6f1e, 0x6f20); mem_wr(gb, 0xff70, A);
    CYC(0x6f20, 0x6f21); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6f21, 0x6f23); H8(hFF8B) = A;
    CYC(0x6f23, 0x6f25); A = 0x06;
    CYC(0x6f25, 0x6f27); mem_wr(gb, 0xff70, A);
    CYC(0x6f27, 0x6f29); A = H8(hFF8B);
    CYC(0x6f29, 0x6f2a); mem_wr(gb, DE, A);
    CYC(0x6f2a, 0x6f2b); SET_DE(DE + 1);
    CYC(0x6f2b, 0x6f2c); SET_BC(BC - 1);
    CYC(0x6f2c, 0x6f2d); A = B;
    CYC(0x6f2d, 0x6f2e); alu_or(gb, C);
    if (!(F & FZ)) {
      CYCT(0x6f2e, 0x6f30);
      continue;
    }
    CYC(0x6f2e, 0x6f30);
    break;
  }
  CYC(0x6f30, 0x6f31); ret_effect(gb);
}

void func_04_6f07_hook(GB *gb) {
  CYC(0x6f07, 0x6f0a); SET_HL(w3VramTiles);
  CYC(0x6f0a, 0x6f0d); SET_DE(w6TileBuffer);
  CYC(0x6f0d, 0x6f10); SET_BC(0x0200);
  CYC(0x6f10, 0x6f13); push_effect(gb, 0x6f13);
  func_04_6f07_locfunc(gb);
  CYC(0x6f13, 0x6f16); SET_HL(w3VramAttributes);
  CYC(0x6f16, 0x6f19); SET_DE(w6AttributeBuffer);
  CYC(0x6f19, 0x6f1c); SET_BC(0x0200);
  func_04_6f07_locfunc(gb);
}

static void func_04_6f31_locfunc(GB *gb) {
  for (;;) {
    CYC(0x6f46, 0x6f48); A = 0x06;
    CYC(0x6f48, 0x6f4a); mem_wr(gb, 0xff70, A);
    CYC(0x6f4a, 0x6f4b); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6f4b, 0x6f4d); H8(hFF8B) = A;
    CYC(0x6f4d, 0x6f4f); A = 0x03;
    CYC(0x6f4f, 0x6f51); mem_wr(gb, 0xff70, A);
    CYC(0x6f51, 0x6f53); A = H8(hFF8B);
    CYC(0x6f53, 0x6f54); mem_wr(gb, DE, A);
    CYC(0x6f54, 0x6f55); SET_DE(DE + 1);
    CYC(0x6f55, 0x6f56); SET_BC(BC - 1);
    CYC(0x6f56, 0x6f57); A = B;
    CYC(0x6f57, 0x6f58); alu_or(gb, C);
    if (!(F & FZ)) {
      CYCT(0x6f58, 0x6f5a);
      continue;
    }
    CYC(0x6f58, 0x6f5a);
    break;
  }
  CYC(0x6f5a, 0x6f5b); ret_effect(gb);
}

void func_04_6f31_hook(GB *gb) {
  CYC(0x6f31, 0x6f34); SET_HL(w6TileBuffer);
  CYC(0x6f34, 0x6f37); SET_DE(w3VramTiles);
  CYC(0x6f37, 0x6f3a); SET_BC(0x0200);
  CYC(0x6f3a, 0x6f3d); push_effect(gb, 0x6f3d);
  func_04_6f31_locfunc(gb);
  CYC(0x6f3d, 0x6f40); SET_HL(w6AttributeBuffer);
  CYC(0x6f40, 0x6f43); SET_DE(w3VramAttributes);
  CYC(0x6f43, 0x6f46); SET_BC(0x0200);
  func_04_6f31_locfunc(gb);
}
