#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

void fake_copyRectangleFromVramTilesToAddress_hook(GB *gb);

static void fake_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static void fake_draw_rectangle_to_vram_tiles_rows(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x7dff, 0x7e00); push_effect(gb, BC);
    for (;;) {
      CYC(0x7e00, 0x7e01); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7e01, 0x7e02); mem_wr(gb, DE, A);
      CYC(0x7e02, 0x7e04); D |= 0x04;
      CYC(0x7e04, 0x7e05); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7e05, 0x7e06); mem_wr(gb, DE, A);
      CYC(0x7e06, 0x7e08); D &= (uint8_t)~0x04;
      CYC(0x7e08, 0x7e09); SET_DE(DE + 1);
      CYC(0x7e09, 0x7e0a); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x7e0a, 0x7e0c);
        continue;
      }
      CYC(0x7e0a, 0x7e0c);
      break;
    }
    CYC(0x7e0c, 0x7e0d); SET_BC(pop_effect(gb));
    CYC(0x7e0d, 0x7e0f); A = 0x20;
    CYC(0x7e0f, 0x7e10); alu_sub(gb, C);
    CALL_C(0x7e10, addAToDe_hook, 0x0068, 0x7e13);
    CYC(0x7e13, 0x7e14); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x7e14, 0x7e16);
      continue;
    }
    CYC(0x7e14, 0x7e16);
    break;
  }
  CYC(0x7e16, 0x7e17); SET_AF(pop_effect(gb));
  CYC(0x7e17, 0x7e19); mem_wr(gb, IO_SVBK, A);
  CYC(0x7e19, 0x7e1a); ret_effect(gb);
}

static void fake_copy_rectangle_from_vram_tiles_rows(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x7e2a, 0x7e2b); push_effect(gb, BC);
    for (;;) {
      CYC(0x7e2b, 0x7e2d); A = 0x02;
      CYC(0x7e2d, 0x7e2f); mem_wr(gb, IO_SVBK, A);
      CYC(0x7e2f, 0x7e30); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7e30, 0x7e31); B = A;
      CYC(0x7e31, 0x7e33); A = 0x03;
      CYC(0x7e33, 0x7e35); mem_wr(gb, IO_SVBK, A);
      CYC(0x7e35, 0x7e36); A = B;
      CYC(0x7e36, 0x7e37); mem_wr(gb, DE, A);
      CYC(0x7e37, 0x7e38); SET_DE(DE + 1);
      CYC(0x7e38, 0x7e39); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x7e39, 0x7e3b);
        continue;
      }
      CYC(0x7e39, 0x7e3b);
      break;
    }
    CYC(0x7e3b, 0x7e3c); SET_BC(pop_effect(gb));
    CYC(0x7e3c, 0x7e3e); A = 0x20;
    CYC(0x7e3e, 0x7e3f); alu_sub(gb, C);
    CALL_C(0x7e3f, addAToDe_hook, 0x0068, 0x7e42);
    CYC(0x7e42, 0x7e44); A = 0x20;
    CYC(0x7e44, 0x7e45); alu_sub(gb, C);
    CYC(0x7e45, 0x7e46); fake_add_a_to_hl_from_rst(gb, 0x7e46);
    CYC(0x7e46, 0x7e47); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x7e47, 0x7e49);
      continue;
    }
    CYC(0x7e47, 0x7e49);
    break;
  }
  CYC(0x7e49, 0x7e4a); SET_AF(pop_effect(gb));
  CYC(0x7e4a, 0x7e4c); mem_wr(gb, IO_SVBK, A);
  CYC(0x7e4c, 0x7e4d); ret_effect(gb);
}

static void fake_copy_rectangle_to_room_layout_rows(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x7e55, 0x7e56); push_effect(gb, BC);
    for (;;) {
      CYC(0x7e56, 0x7e57); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7e57, 0x7e58); mem_wr(gb, DE, A);
      CYC(0x7e58, 0x7e59); D = alu_dec8(gb, D);
      CYC(0x7e59, 0x7e5a); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x7e5a, 0x7e5b); mem_wr(gb, DE, A);
      CYC(0x7e5b, 0x7e5c); D = alu_inc8(gb, D);
      CYC(0x7e5c, 0x7e5d); SET_DE(DE + 1);
      CYC(0x7e5d, 0x7e5e); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x7e5e, 0x7e60);
        continue;
      }
      CYC(0x7e5e, 0x7e60);
      break;
    }
    CYC(0x7e60, 0x7e61); SET_BC(pop_effect(gb));
    CYC(0x7e61, 0x7e63); A = 0x10;
    CYC(0x7e63, 0x7e64); alu_sub(gb, C);
    CALL_C(0x7e64, addAToDe_hook, 0x0068, 0x7e67);
    CYC(0x7e67, 0x7e68); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x7e68, 0x7e6a);
      continue;
    }
    CYC(0x7e68, 0x7e6a);
    break;
  }
  CYC(0x7e6a, 0x7e6b); ret_effect(gb);
}

void fake_readParametersForRectangleDrawing_hook(GB *gb) {
  CYC(0x7de7, 0x7de8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7de8, 0x7de9); B = A;
  CYC(0x7de9, 0x7dea); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7dea, 0x7deb); C = A;
  CYC(0x7deb, 0x7dec); ret_effect(gb);
}

void fake_drawRectangleToVramTiles_withParameters_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7dec, 0x7dee); A = mem_rd(gb, IO_SVBK);
  CYC(0x7dee, 0x7def); push_effect(gb, AF);
  CYC(0x7def, 0x7df1); A = 0x03;
  CYC(0x7df1, 0x7df3); mem_wr(gb, IO_SVBK, A);
  CYC(0x7df3, 0x7df5);
  fake_draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void fake_drawRectangleToVramTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7df5, 0x7df7); A = mem_rd(gb, IO_SVBK);
  CYC(0x7df7, 0x7df8); push_effect(gb, AF);
  CYC(0x7df8, 0x7dfa); A = 0x03;
  CYC(0x7dfa, 0x7dfc); mem_wr(gb, IO_SVBK, A);
  CALL_ROM(0x7dfc, 0x7de3);
  fake_draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void fake_drawRectangleToVramTiles__nextRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  fake_draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void fake_copyRectangleFromVramTilesToAddress_paramBc_hook(GB *gb) {
  CYC(0x7e1a, 0x7e1b); L = C;
  CYC(0x7e1b, 0x7e1c); H = B;
  fake_copyRectangleFromVramTilesToAddress_hook(gb);
}

void fake_copyRectangleFromVramTilesToAddress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e1c, 0x7e1e); A = mem_rd(gb, IO_SVBK);
  CYC(0x7e1e, 0x7e1f); push_effect(gb, AF);
  CYC(0x7e1f, 0x7e20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e20, 0x7e21); B = A;
  CYC(0x7e21, 0x7e22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e22, 0x7e23); C = A;
  CYC(0x7e23, 0x7e24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e24, 0x7e25); E = A;
  CYC(0x7e25, 0x7e26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e26, 0x7e27); D = A;
  CYC(0x7e27, 0x7e28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e28, 0x7e29); H = mem_rd(gb, HL);
  CYC(0x7e29, 0x7e2a); L = A;
  fake_copy_rectangle_from_vram_tiles_rows(gb, sp0_);
}

void fake_copyRectangleFromVramTilesToAddress__nextRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  fake_copy_rectangle_from_vram_tiles_rows(gb, sp0_);
}

void fake_copyRectangleToRoomLayoutAndCollisions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e4d, 0x7e4e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e4e, 0x7e4f); E = A;
  CYC(0x7e4f, 0x7e50); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e50, 0x7e51); D = A;
  CYC(0x7e51, 0x7e52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e52, 0x7e53); B = A;
  CYC(0x7e53, 0x7e54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e54, 0x7e55); C = A;
  fake_copy_rectangle_to_room_layout_rows(gb, sp0_);
}

void fake_copyRectangleToRoomLayoutAndCollisions_paramDe_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e51, 0x7e52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e52, 0x7e53); B = A;
  CYC(0x7e53, 0x7e54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e54, 0x7e55); C = A;
  fake_copy_rectangle_to_room_layout_rows(gb, sp0_);
}

void fake_copyRectangleToRoomLayoutAndCollisions_paramDe__nextRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  fake_copy_rectangle_to_room_layout_rows(gb, sp0_);
}

void fake_roomTileChangesAfterLoad04_hook(GB *gb) {
  CYC(0x7e6b, 0x7e6e); SET_HL(wInShop);
  CYC(0x7e6e, 0x7e70); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(0x7e70, 0x7e72); A = 0x03;
  CYC(0x7e72, 0x7e75); loadObjectGfx2_hook(gb);
}

void fake_checkLoadPastSignAndChestGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e75, 0x7e78); A = W8(wDungeonIndex);
  CYC(0x7e78, 0x7e7a); alu_cp(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x7e7a, 0x7e7b); ret_effect(gb);
    return;
  }
  CYC(0x7e7a, 0x7e7b);
  CYC(0x7e7b, 0x7e7e); A = W8(wTilesetFlags);
  CYC(0x7e7e, 0x7e80); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x7e80, 0x7e81); ret_effect(gb);
    return;
  }
  CYC(0x7e80, 0x7e81);
  CYC(0x7e81, 0x7e83); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x7e83, 0x7e84); ret_effect(gb);
    return;
  }
  CYC(0x7e83, 0x7e84);
  CYC(0x7e84, 0x7e86); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x7e86, 0x7e87); ret_effect(gb);
    return;
  }
  CYC(0x7e86, 0x7e87);
  CYC(0x7e87, 0x7e89); alu_and(gb, 0x1c);
  if (F & FZ) {
    CYCT(0x7e89, 0x7e8a); ret_effect(gb);
    return;
  }
  CYC(0x7e89, 0x7e8a);
  CYC(0x7e8a, 0x7e8c); A = 0x37;
  CYC(0x7e8c, 0x7e8f);
  hook_continue(gb, 0x05df, sp0_);
}
