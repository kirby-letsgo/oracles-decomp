#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

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

// 04:69c7
void setTileToWitheredVine_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69c7, 0x69c8); L = mem_rd(gb, HL);
  CYC(0x69c8, 0x69ca); H = 0xcf;
  CYC(0x69ca, 0x69cb); A = mem_rd(gb, HL);
  CYC(0x69cb, 0x69cc); push_effect(gb, HL);
  CALL_C(0x69cc, retrieveTileCollisionValue_hook, 0x156e, 0x69cf);
  CYC(0x69cf, 0x69d0); SET_HL(pop_effect(gb));
  CYC(0x69d0, 0x69d1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x69d1, 0x69d2); ret_effect(gb);
    return;
  }
  CYC(0x69d1, 0x69d2);
  CYC(0x69d2, 0x69d4); mem_wr(gb, HL, 0x8c);
  CYC(0x69d4, 0x69d5); ret_effect(gb);
}

// 04:69d5
void getVinePosition_hook(GB *gb) {
  CYC(0x69d5, 0x69d6); A = B;
  CYC(0x69d6, 0x69d9); SET_HL(0xc8f0);
  CYC(0x69d9, 0x69da); add_a_to_hl_from_rst(gb, 0x69da);
  CYC(0x69da, 0x69db); A = mem_rd(gb, HL);
  CYC(0x69db, 0x69dc); alu_cp(gb, C);
  CYC(0x69dc, 0x69dd); ret_effect(gb);
}

// 04:6bb5
void fillRectInRoomLayout_hook(GB *gb) {
  CYC(0x6bb5, 0x6bb6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6bb6, 0x6bb7); E = A;
  CYC(0x6bb7, 0x6bb8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6bb8, 0x6bb9); B = A;
  CYC(0x6bb9, 0x6bba); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6bba, 0x6bbb); C = A;
  CYC(0x6bbb, 0x6bbc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6bbc, 0x6bbd); D = A;
  CYC(0x6bbd, 0x6bbf); H = 0xcf;
  for (;;) {
    CYC(0x6bbf, 0x6bc0); A = D;
    CYC(0x6bc0, 0x6bc1); L = E;
    CYC(0x6bc1, 0x6bc2); push_effect(gb, BC);
    for (;;) {
      CYC(0x6bc2, 0x6bc3); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x6bc3, 0x6bc4); C = alu_dec8(gb, C);
      if (!(F & FZ)) {
        CYCT(0x6bc4, 0x6bc6);
        continue;
      }
      CYC(0x6bc4, 0x6bc6);
      break;
    }
    CYC(0x6bc6, 0x6bc7); A = E;
    CYC(0x6bc7, 0x6bc9); alu_add(gb, 0x10);
    CYC(0x6bc9, 0x6bca); E = A;
    CYC(0x6bca, 0x6bcb); SET_BC(pop_effect(gb));
    CYC(0x6bcb, 0x6bcc); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x6bcc, 0x6bce);
      continue;
    }
    CYC(0x6bcc, 0x6bce);
    break;
  }
  CYC(0x6bce, 0x6bcf); ret_effect(gb);
}

// 04:6bcf
void drawRectInRoomLayout_hook(GB *gb) {
  CYC(0x6bcf, 0x6bd0); A = mem_rd(gb, DE);
  CYC(0x6bd0, 0x6bd1); SET_DE(DE + 1);
  CYC(0x6bd1, 0x6bd3); H = 0xcf;
  CYC(0x6bd3, 0x6bd4); L = A;
  CYC(0x6bd4, 0x6bd6); hram_wr(gb, 0x8b, A);
  CYC(0x6bd6, 0x6bd7); A = mem_rd(gb, DE);
  CYC(0x6bd7, 0x6bd8); SET_DE(DE + 1);
  CYC(0x6bd8, 0x6bd9); C = A;
  CYC(0x6bd9, 0x6bda); A = mem_rd(gb, DE);
  CYC(0x6bda, 0x6bdb); SET_DE(DE + 1);
  CYC(0x6bdb, 0x6bdd); hram_wr(gb, 0x8d, A);
  for (;;) {
    CYC(0x6bdd, 0x6bdf); A = hram_rd(gb, 0x8d);
    CYC(0x6bdf, 0x6be0); B = A;
    for (;;) {
      CYC(0x6be0, 0x6be1); A = mem_rd(gb, DE);
      CYC(0x6be1, 0x6be2); SET_DE(DE + 1);
      CYC(0x6be2, 0x6be3); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x6be3, 0x6be4); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(0x6be4, 0x6be6);
        continue;
      }
      CYC(0x6be4, 0x6be6);
      break;
    }
    CYC(0x6be6, 0x6be8); A = hram_rd(gb, 0x8b);
    CYC(0x6be8, 0x6bea); alu_add(gb, 0x10);
    CYC(0x6bea, 0x6bec); hram_wr(gb, 0x8b, A);
    CYC(0x6bec, 0x6bed); L = A;
    CYC(0x6bed, 0x6bee); C = alu_dec8(gb, C);
    if (!(F & FZ)) {
      CYCT(0x6bee, 0x6bf0);
      continue;
    }
    CYC(0x6bee, 0x6bf0);
    break;
  }
  CYC(0x6bf0, 0x6bf1); ret_effect(gb);
}
