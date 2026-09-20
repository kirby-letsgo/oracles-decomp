#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(fake_drawRectangleToVramTiles);
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
  CYC(b_+34, b_+36); mem_wr(gb, IO_SVBK, A);
  CYC(b_+36, b_+37); ret_effect(gb);
}

static void fake_copy_rectangle_from_vram_tiles_rows(GB *gb, uint16_t sp0_) {
  BASE(fake_copyRectangleFromVramTilesToAddress);
  for (;;) {
    CYC(b_+14, b_+15); push_effect(gb, BC);
    for (;;) {
      CYC(b_+15, b_+17); A = 0x02;
      CYC(b_+17, b_+19); mem_wr(gb, IO_SVBK, A);
      CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+20, b_+21); B = A;
      CYC(b_+21, b_+23); A = 0x03;
      CYC(b_+23, b_+25); mem_wr(gb, IO_SVBK, A);
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
    CYC(b_+41, b_+42); fake_add_a_to_hl_from_rst(gb, b_+42);
    CYC(b_+42, b_+43); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+43, b_+45);
      continue;
    }
    CYC(b_+43, b_+45);
    break;
  }
  CYC(b_+45, b_+46); SET_AF(pop_effect(gb));
  CYC(b_+46, b_+48); mem_wr(gb, IO_SVBK, A);
  CYC(b_+48, b_+49); ret_effect(gb);
}

static void fake_copy_rectangle_to_room_layout_rows(GB *gb, uint16_t sp0_) {
  BASE(fake_copyRectangleToRoomLayoutAndCollisions_paramDe);
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

void fake_readParametersForRectangleDrawing_hook(GB *gb) {
  BASE(fake_readParametersForRectangleDrawing);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); B = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); ret_effect(gb);
}

void fake_drawRectangleToVramTiles_withParameters_hook(GB *gb) {
  BASE(fake_drawRectangleToVramTiles_withParameters);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); mem_wr(gb, IO_SVBK, A);
  CYC(b_+7, b_+9);
  fake_draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void fake_drawRectangleToVramTiles_hook(GB *gb) {
  BASE(fake_drawRectangleToVramTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x03;
  CYC(b_+5, b_+7); mem_wr(gb, IO_SVBK, A);
  CALL_ROM(b_+7, (SYM(rectangleData_02_7de1) + 2));
  fake_draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void fake_drawRectangleToVramTiles__nextRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  fake_draw_rectangle_to_vram_tiles_rows(gb, sp0_);
}

void fake_copyRectangleFromVramTilesToAddress_paramBc_hook(GB *gb) {
  BASE(fake_copyRectangleFromVramTilesToAddress_paramBc);
  CYC(b_+0, b_+1); L = C;
  CYC(b_+1, b_+2); H = B;
  fake_copyRectangleFromVramTilesToAddress_hook(gb);
}

void fake_copyRectangleFromVramTilesToAddress_hook(GB *gb) {
  BASE(fake_copyRectangleFromVramTilesToAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_SVBK);
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
  fake_copy_rectangle_from_vram_tiles_rows(gb, sp0_);
}

void fake_copyRectangleFromVramTilesToAddress__nextRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  fake_copy_rectangle_from_vram_tiles_rows(gb, sp0_);
}

void fake_copyRectangleToRoomLayoutAndCollisions_hook(GB *gb) {
  BASE(fake_copyRectangleToRoomLayoutAndCollisions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); E = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); D = A;
  CYC(SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe), (SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 1)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC((SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 1), (SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 2)); B = A;
  CYC((SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 2), (SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 3)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC((SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 3), (SYM(fake_copyRectangleToRoomLayoutAndCollisions_paramDe) + 4)); C = A;
  fake_copy_rectangle_to_room_layout_rows(gb, sp0_);
}

void fake_copyRectangleToRoomLayoutAndCollisions_paramDe_hook(GB *gb) {
  BASE(fake_copyRectangleToRoomLayoutAndCollisions_paramDe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); B = A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); C = A;
  fake_copy_rectangle_to_room_layout_rows(gb, sp0_);
}

void fake_copyRectangleToRoomLayoutAndCollisions_paramDe__nextRow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  fake_copy_rectangle_to_room_layout_rows(gb, sp0_);
}

void fake_roomTileChangesAfterLoad04_hook(GB *gb) {
  BASE(fake_roomTileChangesAfterLoad04);
  CYC(b_+0, b_+3); SET_HL(wInShop);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, b_+10); loadObjectGfx2_hook(gb);
}

void fake_checkLoadPastSignAndChestGfx_hook(GB *gb) {
  BASE(fake_checkLoadPastSignAndChestGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
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
  CYC(b_+23, b_+26);
  hook_continue(gb, (SYM(loadUncompressedGfxHeader) + 5), sp0_);
}
