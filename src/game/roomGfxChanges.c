#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

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
