#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

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
