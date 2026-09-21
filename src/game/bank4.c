#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void label_04_032_hook(GB *gb);
void label_04_033_hook(GB *gb);
void setWarpDestDefault_hook(GB *gb);
void func_04_4732_hook(GB *gb);
void getLinkWarpQuadrant_hook(GB *gb);
void updateChangedTileQueue_hook(GB *gb);
void updateChangedTileQueue__handleSingleEntry6c46_hook(GB *gb);
void write4BytesToVramLayout_hook(GB *gb);
void getVramSubtileAddressOfTile_hook(GB *gb);
void setInterleavedTile_body_hook(GB *gb);
void setInterleavedTile_body__copy2Bytes6cf3_hook(GB *gb);
void setInterleavedTile_body__interleaveDiagonally6cfa_hook(GB *gb);
void setInterleavedTile_body__copy2BytesSeparated6d0f_hook(GB *gb);
void setInterleavedTile_body__queueWrite6d18_hook(GB *gb);
void queueTileWriteAtVBlank_hook(GB *gb);
void queueTileWriteAtVBlank__copy2Bytes6d4d_hook(GB *gb);
void queueTileWriteAtVBlank__getTilePositionInVram6d54_hook(GB *gb);
void generateW3VramTilesAndAttributes_hook(GB *gb);

static void bank4_add_double_index_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void bank4_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void b4_vblank_function(GB *gb, uint16_t base, uint8_t column) {
  BANKOF(findWarpSourceAndDest);
  CYC(base, base + 1); H = B;
  CYC(base + 1, base + 2); L = E;
  CYC(base + 2, base + 4); B = 0x04;
  for (;;) {
    for (uint8_t row = 0; row < 8; row++) {
      uint16_t pc = (uint16_t)(base + 4 + row * 4);
      CYC(pc, pc + 2); E = (uint8_t)(column + row * 0x20);
      CYC(pc + 2, pc + 3); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(pc + 3, pc + 4); mem_wr(gb, DE, A);
    }
    CYC(base + 0x24, base + 0x25); D = alu_inc8(gb, D);
    CYC(base + 0x25, base + 0x26); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(base + 0x26, base + 0x28); continue; }
    CYC(base + 0x26, base + 0x28);
    break;
  }
  CYC(base + 0x28, base + 0x29); L = C;
  CYC(base + 0x29, base + 0x2b); H = 0xc4;
  CYC(base + 0x2b, base + 0x2e); vblankFunctionRet_hook(gb);
}

void b4VBlankFunction0_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction0), 0x00);
}

void b4VBlankFunction1_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction1), 0x01);
}

void b4VBlankFunction2_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction2), 0x02);
}

void b4VBlankFunction3_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction3), 0x03);
}

void b4VBlankFunction4_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction4), 0x04);
}

void b4VBlankFunction5_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction5), 0x05);
}

void b4VBlankFunction6_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction6), 0x06);
}

void b4VBlankFunction7_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction7), 0x07);
}

void b4VBlankFunction8_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction8), 0x08);
}

void b4VBlankFunction9_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction9), 0x09);
}

void b4VBlankFunction10_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction10), 0x0a);
}

void b4VBlankFunction11_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction11), 0x0b);
}

void b4VBlankFunction12_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction12), 0x0c);
}

void b4VBlankFunction13_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction13), 0x0d);
}

void b4VBlankFunction14_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction14), 0x0e);
}

void b4VBlankFunction15_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction15), 0x0f);
}

void b4VBlankFunction16_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction16), 0x10);
}

void b4VBlankFunction17_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction17), 0x11);
}

void b4VBlankFunction18_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction18), 0x12);
}

void b4VBlankFunction19_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction19), 0x13);
}

void b4VBlankFunction20_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction20), 0x14);
}

void b4VBlankFunction21_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction21), 0x15);
}

void b4VBlankFunction22_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction22), 0x16);
}

void b4VBlankFunction23_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction23), 0x17);
}

void b4VBlankFunction24_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction24), 0x18);
}

void b4VBlankFunction25_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction25), 0x19);
}

void b4VBlankFunction26_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction26), 0x1a);
}

void b4VBlankFunction27_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction27), 0x1b);
}

void b4VBlankFunction28_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction28), 0x1c);
}

void b4VBlankFunction29_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction29), 0x1d);
}

void b4VBlankFunction30_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction30), 0x1e);
}

void b4VBlankFunction31_hook(GB *gb) {
  b4_vblank_function(gb, SYM(b4VBlankFunction31), 0x1f);
}

static void applyWarpDest_finish(GB *gb) {
  BASE(label_04_033);
  CYC(b_+20, b_+22); A = 0x0a;
  CYC(b_+22, b_+25); W8(wLinkForceState) = A;
  CYC(b_+25, b_+28); A = W8(wActiveGroup);
  CYC(b_+28, b_+30); mem_wr(gb, hFF8B, A);
  CYC(b_+30, b_+33); A = W8(wWarpDestGroup);
  CYC(b_+33, b_+35); alu_and(gb, 0x07);
  CYC(b_+35, b_+38); W8(wActiveGroup) = A;
  CYC(b_+38, b_+41); A = W8(wWarpDestRoom);
  CYC(b_+41, b_+44); W8(wActiveRoom) = A;
  CYC(b_+44, b_+47); SET_HL(w1Link_enabled);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x03);
  CYC(b_+49, b_+52); A = W8(wWarpDestPos);
  CYC(b_+52, b_+53); B = A;
  CYC(b_+53, b_+55); alu_and(gb, 0xf0);
  CYC(b_+55, b_+57); alu_or(gb, 0x08);
  CYC(b_+57, b_+59); L = w1Link_yh & 0xff;
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+60, b_+61); L = alu_inc8(gb, L);
  CYC(b_+61, b_+62); A = B;
  CYC(b_+62, b_+64); alu_and(gb, 0x0f);
  CYC(b_+64, b_+66); alu_swap_a(gb);
  CYC(b_+66, b_+68); alu_or(gb, 0x08);
  CYC(b_+68, b_+69); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+69, b_+72); loadScreenMusicAndSetRoomPack_hook(gb);
}

void vblankRunBank4Function_b04_hook(GB *gb) {
  BASE(vblankRunBank4Function_b04);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+2, b_+4); hram_wr(gb, 0x4f, A);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); E = A;
  CYC(b_+6, b_+8); B = 0xcd;
  CYC(b_+8, b_+10); D = 0x98;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); C = L;
  CYC(b_+14, b_+15); L = A;
  CYC(b_+15, b_+16); hook_handoff(gb, HL);
}

void label_04_033_hook(GB *gb) {
  BASE(label_04_033);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+3); B = 0x00;
  CYC(b_+3, b_+4); alu_add_hl(gb, BC);
  CYC(b_+4, b_+5); alu_add_hl(gb, BC);
  CYC(b_+5, b_+6); alu_add_hl(gb, BC);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+10); W8(wWarpDestRoom) = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+14); W8(wWarpDestPos) = A;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+17); alu_or(gb, 0x80);
  CYC(b_+17, b_+20); W8(wWarpTransition) = A;
  applyWarpDest_finish(gb);
}

void label_04_032_hook(GB *gb) {
  BASE(label_04_032);
  CYC(b_+0, b_+3); SET_HL(SYM(warpDestTable));
  CYC(b_+3, b_+4); bank4_add_double_index_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+10); A = W8(wWarpDestRoom);
  label_04_033_hook(gb);
}

void applyWarpDest_b04_hook(GB *gb) {
  BASE(applyWarpDest_b04);
  CYC(b_+0, b_+3); A = W8(wWarpDestGroup);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    applyWarpDest_finish(gb);
    return;
  }
  CYC(b_+5, b_+7);
  label_04_032_hook(gb);
}

void findWarpSourceAndDest_hook(GB *gb) {
  BASE(findWarpSourceAndDest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wDisableWarps);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+7);
    TAIL(setWarpDestDefault);
  }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+10); A = W8(wActiveGroup);
  CYC(b_+10, b_+13); SET_HL(SYM(warpSourcesTable));
  CYC(b_+13, b_+14); bank4_add_double_index_to_hl(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+20); A = W8(wActiveRoom);
  CYC(b_+20, b_+21); B = A;

  for (;;) {
    CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+22, b_+24); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(b_+24, b_+26);
      break;
    }
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+28); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(b_+28, b_+30);
      goto found;
    }
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+32); alu_bit(gb, 6, A);
    if (!(F & FZ)) {
      CYCT(b_+32, b_+34);
      CYC(b_+47, b_+48); A = mem_rd(gb, HL);
      CYC(b_+48, b_+49); alu_cp(gb, B);
      if (!(F & FZ)) {
        CYCT(b_+49, b_+51);
        goto skip;
      }
      CYC(b_+49, b_+51);
      CYC(b_+51, b_+52); SET_HL(HL + 1);
      CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+53, b_+54); H = mem_rd(gb, HL);
      CYC(b_+54, b_+55); L = A;
      CYC(b_+55, b_+57); A = mem_rd(gb, hFF8D);
      CYC(b_+57, b_+58); B = A;
      CYC(b_+58, b_+60);
      continue;
    }
    CYC(b_+32, b_+34);
    CYC(b_+34, b_+36); alu_and(gb, 0x0f);
    if (!(F & FZ)) {
      CYCT(b_+36, b_+38);
      goto skip;
    }
    CYC(b_+36, b_+38);
    CYC(b_+38, b_+39); A = mem_rd(gb, HL);
    CYC(b_+39, b_+40); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(b_+40, b_+42);
      goto found;
    }
    CYC(b_+40, b_+42);

skip:
    CYC(b_+42, b_+43); SET_HL(HL + 1);
    CYC(b_+43, b_+44); SET_HL(HL + 1);
    CYC(b_+44, b_+45); SET_HL(HL + 1);
    CYC(b_+45, b_+47);
  }

  CYC(b_+81, b_+84); A = W8(wTilesetFlags);
  CYC(b_+84, b_+86); alu_and(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+86, b_+88);
    TAIL(setWarpDestDefault);
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); A = mem_rd(gb, hFF8C);
  CYC(b_+90, b_+91); alu_rrca(gb);
  CYC(b_+91, b_+93); B = 0x01;
  if (!(F & FC)) {
    CYCT(b_+93, b_+95);
  } else {
    CYC(b_+93, b_+95);
    CYC(b_+95, b_+97); B = 0xff;
  }
  CYC(b_+97, b_+100); A = W8(wDungeonFloor);
  CYC(b_+100, b_+101); alu_add(gb, B);
  CYC(b_+101, b_+104); W8(wDungeonFloor) = A;
  CALL_C(b_+104, getActiveRoomFromDungeonMapPosition_hook, SYM(getActiveRoomFromDungeonMapPosition), b_+107);
  CYC(b_+107, b_+110); W8(wWarpDestRoom) = A;
  CYC(b_+110, b_+112); A = mem_rd(gb, hFF8D);
  CYC(b_+112, b_+115); W8(wWarpDestPos) = A;
  CYC(b_+115, b_+118); A = W8(wActiveGroup);
  CYC(b_+118, b_+120); alu_or(gb, 0x80);
  CYC(b_+120, b_+123); W8(wWarpDestGroup) = A;
  CYC(b_+123, b_+124); alu_xor(gb, A);
  CYC(b_+124, b_+127); W8(wWarpTransition) = A;
  CYC(b_+127, b_+129); A = 0x03;
  CYC(b_+129, b_+132); W8(wWarpTransition2) = A;
  CYC(b_+132, b_+134); A = 0x6e;
  CYC(b_+134, b_+137); playSound_b00_hook(gb);
  return;

found:
  CYC(b_+60, b_+61); SET_HL(HL + 1);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+62, b_+65); W8(wWarpDestRoom) = A;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+66, b_+67); B = A;
  CYC(b_+67, b_+69); alu_swap_a(gb);
  CYC(b_+69, b_+71); alu_and(gb, 0x0f);
  CYC(b_+71, b_+74); W8(wWarpDestGroup) = A;
  CYC(b_+74, b_+75); A = B;
  CYC(b_+75, b_+77); alu_and(gb, 0x0f);
  CYC(b_+77, b_+80); W8(wWarpTransition) = A;
  CYC(b_+80, b_+81); ret_effect(gb);
}

void setWarpDestDefault_hook(GB *gb) {
  BASE(setWarpDestDefault);
  CYC(b_+0, b_+3); SET_HL(wWarpDestGroup);
  CYC(b_+3, b_+6); A = W8(wActiveGroup);
  CYC(b_+6, b_+8); alu_or(gb, 0x80);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+12); A = W8(wActiveRoom);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); A = mem_rd(gb, hFF8D);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x03);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void findScreenEdgeWarpSource_hook(GB *gb) {
  BASE(findScreenEdgeWarpSource);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wScreenTransitionDirection);
  CYC(b_+9, b_+10); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, getLinkWarpQuadrant_hook, SYM(getLinkWarpQuadrant), b_+14);
  CYC(b_+14, b_+17); SET_HL(0x00f8);
  CYC(b_+17, b_+18); alu_add(gb, L);
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+20); B = mem_rd(gb, HL);
  CYC(b_+20, b_+23); A = W8(wActiveGroup);
  CYC(b_+23, b_+26); SET_HL(SYM(warpSourcesTable));
  CYC(b_+26, b_+27); bank4_add_double_index_to_hl(gb, b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); H = mem_rd(gb, HL);
  CYC(b_+29, b_+30); L = A;
  CYC(b_+30, b_+33); A = W8(wActiveRoom);
  CYC(b_+33, b_+34); C = A;

  for (;;) {
    CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+35, b_+37); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(b_+37, b_+38); ret_effect(gb);
      return;
    }
    CYC(b_+37, b_+38);
    CYC(b_+38, b_+40); alu_bit(gb, 6, A);
    if (!(F & FZ)) {
      CYCT(b_+40, b_+42);
      CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+56, b_+57); alu_cp(gb, C);
      if (!(F & FZ)) {
        CYCT(b_+57, b_+59);
        goto edge_skip;
      }
      CYC(b_+57, b_+59);
      CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+60, b_+61); H = mem_rd(gb, HL);
      CYC(b_+61, b_+62); L = A;
      CYC(b_+62, b_+64);
      continue;
    }
    CYC(b_+40, b_+42);
    CYC(b_+42, b_+43); E = A;
    CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+44, b_+45); alu_cp(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+45, b_+47);
      goto edge_skip;
    }
    CYC(b_+45, b_+47);
    CYC(b_+47, b_+48); A = E;
    CYC(b_+48, b_+49); alu_and(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+49, b_+51);
      break;
    }
    CYC(b_+49, b_+51);

edge_skip:
    CYC(b_+51, b_+52); SET_HL(HL + 1);
    CYC(b_+52, b_+53); SET_HL(HL + 1);
    CYC(b_+53, b_+55);
  }

  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+68); W8(wWarpDestRoom) = A;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+69, b_+70); B = A;
  CYC(b_+70, b_+72); alu_swap_a(gb);
  CYC(b_+72, b_+74); alu_and(gb, 0x0f);
  CYC(b_+74, b_+77); W8(wWarpDestGroup) = A;
  CYC(b_+77, b_+78); A = B;
  CYC(b_+78, b_+80); alu_and(gb, 0x0f);
  CYC(b_+80, b_+81); B = A;
  CYC(b_+81, b_+84); A = W8(wScreenTransitionDirection);
  CYC(b_+84, b_+85); alu_rlca(gb);
  CYC(b_+85, b_+87); alu_swap_a(gb);
  CYC(b_+87, b_+89); alu_and(gb, 0x40);
  CYC(b_+89, b_+90); alu_or(gb, B);
  CYC(b_+90, b_+93); W8(wWarpTransition) = A;
  CYC(b_+93, b_+96); A = W8(wLinkObjectIndex);
  CYC(b_+96, b_+98); alu_cp(gb, 0xd0);
  if (!(F & FZ)) {
    CALL_C_CC(b_+98, func_04_4732_hook, SYM(func_04_4732), b_+101);
  } else {
    CYC(b_+98, b_+101);
  }
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+105); W8(wTmpcec0) = A;
  CYC(b_+105, b_+106); ret_effect(gb);
}

void func_04_4732_hook(GB *gb) {
  BASE(func_04_4732);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CALL_C(b_+1, dismountCompanionAndSetRememberedPositionToScreenCenter_hook, SYM(dismountCompanionAndSetRememberedPositionToScreenCenter), b_+4);
  CYC(b_+4, b_+6); A = 0x01;
  CYC(b_+6, b_+9); W8(wWarpTransition) = A;
  CYC(b_+9, b_+11); A = 0x01;
  CYC(b_+11, b_+14); W8(wWarpTransition2) = A;
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, b_+16); ret_effect(gb);
}

void getLinkWarpQuadrant_hook(GB *gb) {
  BASE(getLinkWarpQuadrant);
  CYC(b_+0, b_+3); A = W8(wScreenTransitionDirection);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(wActiveGroup);
  CYC(b_+7, b_+9); alu_cp(gb, 0x04);
  CYC(b_+9, b_+12); A = W8(w1Link_xh);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
    CYC(b_+20, b_+22); alu_cp(gb, 0x80);
    CYC(b_+22, b_+23); A = B;
    if (F & FC) {
      CYCT(b_+23, b_+24); ret_effect(gb);
      return;
    }
    CYC(b_+23, b_+24);
    CYC(b_+24, b_+25); A = alu_inc8(gb, A);
    CYC(b_+25, b_+26); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, GV(0x58, 0x60));
  CYC(b_+16, b_+17); A = B;
  if (F & FC) {
    CYCT(b_+17, b_+18); ret_effect(gb);
    return;
  }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void replaceToggleBlocks_hook(GB *gb) {
  BASE(replaceToggleBlocks);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkDungeonUsesToggleBlocks_hook, SYM(checkDungeonUsesToggleBlocks), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_HL((SYM(group3WarpSources) + 197));
  CYC(b_+7, b_+9); E = 0x02;
  CALL_C(b_+9, interBankCall_hook, 0x008a, b_+12);
  CYC(b_+12, b_+15); SET_DE(b_+27);
  CYC(b_+15, b_+18); A = W8(wToggleBlocksState);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
  } else {
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+24); SET_DE(b_+32);
  }
  CYC(b_+24, b_+27); replaceTiles_hook(gb);
}

void replaceJabuTilesIfUnderwater_hook(GB *gb) {
  BASE(replaceJabuTilesIfUnderwater);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wTilesetFlags);
  CYC(b_+9, b_+11); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); A = W8(wDungeonFloor);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+19); A = W8(wJabuWaterLevel);
  CYC(b_+19, b_+21); alu_and(gb, 0x07);
  CYC(b_+21, b_+22); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); SET_DE(b_+35);
  CALL_C(b_+26, replaceTiles_hook, SYM(replaceTiles), b_+29);
  CYC(b_+29, b_+32); SET_DE(b_+46);
  CYC(b_+32, b_+35); replaceTiles_hook(gb);
}

static void replace_shutter_tail(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(replaceShutterForLinkEntering);
  if (entry == b_+17) {
    CYC(b_+17, b_+20); SET_DE(b_+135);
    CALL_C(b_+20, addDoubleIndexToDe_hook, 0x0072, b_+23);
    CYC(b_+23, b_+24); A = mem_rd(gb, DE);
    CYC(b_+24, b_+26); H8(hFF8B) = A;
    CYC(b_+26, b_+27); SET_DE(DE + 1);
    CYC(b_+27, b_+28); A = mem_rd(gb, DE);
    CYC(b_+28, b_+29); E = A;
    CYC(b_+29, b_+32); A = W8(wScrollMode);
    CYC(b_+32, b_+34); alu_and(gb, 0x08);
    if (F & FZ) { CYCT(b_+34, b_+36); goto done; }
    CYC(b_+34, b_+36);
    CYC(b_+36, b_+39); A = W8(wLinkObjectIndex);
    CYC(b_+39, b_+40); H = A;
    CYC(b_+40, b_+43); A = W8(wScreenTransitionDirection);
    CYC(b_+43, b_+45); alu_xor(gb, 0x02);
    CYC(b_+45, b_+46); D = A;
    CYC(b_+46, b_+47); A = E;
    CYC(b_+47, b_+49); alu_and(gb, 0x03);
    CYC(b_+49, b_+50); alu_cp(gb, D);
    if (!(F & FZ)) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
    CYC(b_+50, b_+51);
    CYC(b_+51, b_+54); A = W8(wScreenTransitionDirection);
    CYC(b_+54, b_+56); alu_bit(gb, 0, A);
    if (!(F & FZ)) { CYCT(b_+56, b_+58); goto horizontal; }
    CYC(b_+56, b_+58);
    CYC(b_+58, b_+60); alu_and(gb, 0x02);
    CYC(b_+60, b_+62); L = 0x0d;
    CYC(b_+62, b_+63); A = mem_rd(gb, HL);
    if (!(F & FZ)) { CYCT(b_+63, b_+65); goto down; }
    CYC(b_+63, b_+65);
    goto up;
  }
  if (entry == b_+65) goto up;
  if (entry == b_+73) goto down;
  if (entry == b_+79) goto horizontal;
  if (entry == b_+86) goto right;
  if (entry == b_+90) goto left;
  if (entry == b_+94) goto do_replacement;
  goto done;

up:
  CYC(b_+65, b_+67); alu_and(gb, 0xf0);
  CYC(b_+67, b_+69); alu_swap_a(gb);
  CYC(b_+69, b_+71); alu_or(gb, 0xa0);
  CYC(b_+71, b_+73); goto do_replacement;
down:
  CYC(b_+73, b_+75); alu_and(gb, 0xf0);
  CYC(b_+75, b_+77); alu_swap_a(gb);
  CYC(b_+77, b_+79); goto do_replacement;
horizontal:
  CYC(b_+79, b_+81); alu_and(gb, 0x02);
  CYC(b_+81, b_+83); L = 0x0b;
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+84, b_+86); goto left; }
  CYC(b_+84, b_+86);
right:
  CYC(b_+86, b_+88); alu_and(gb, 0xf0);
  CYC(b_+88, b_+90); goto do_replacement;
left:
  CYC(b_+90, b_+92); alu_and(gb, 0xf0);
  CYC(b_+92, b_+94); alu_or(gb, 0x0e);
do_replacement:
  CYC(b_+94, b_+95); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+95, b_+97); goto done; }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+98); push_effect(gb, BC);
  CYC(b_+98, b_+99); C = A;
  CYC(b_+99, b_+100); A = mem_rd(gb, BC);
  CYC(b_+100, b_+102); alu_sub(gb, 0x78);
  CYC(b_+102, b_+104); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(b_+104, b_+106);
  } else {
    CYC(b_+104, b_+106);
    CYC(b_+106, b_+108); A = H8(hFF8B);
    CYC(b_+108, b_+109); mem_wr(gb, BC, A);
  }
  CYC(b_+109, b_+110); SET_BC(pop_effect(gb));
done:
  CYC(b_+110, b_+111); A = E;
  CYC(b_+111, b_+113); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+113, b_+114); ret_effect(gb); return; }
  CYC(b_+113, b_+114);
  CYC(b_+114, b_+116); alu_and(gb, 0x7f);
  CYC(b_+116, b_+117); E = A;
  CYC(b_+117, b_+120); A = W8(wTilesetFlags);
  CYC(b_+120, b_+122); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+122, b_+123); ret_effect(gb); return; }
  CYC(b_+122, b_+123);
  CALL_C(b_+123, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+126);
  if (!(F & FZ)) { CYCT(b_+126, b_+127); ret_effect(gb); return; }
  CYC(b_+126, b_+127);
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x1e);
  CYC(b_+129, b_+130); L = alu_inc8(gb, L);
  CYC(b_+130, b_+131); mem_wr(gb, HL, E);
  CYC(b_+131, b_+133); L = 0x4b;
  CYC(b_+133, b_+134); mem_wr(gb, HL, C);
  CYC(b_+134, b_+135); ret_effect(gb);
}

void replaceShutterForLinkEntering__temporarilyOpenDoor61e9_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__temporarilyOpenDoor), sp0_); }
void replaceShutterForLinkEntering__up6219_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__up), sp0_); }
void replaceShutterForLinkEntering__down6221_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__down), sp0_); }
void replaceShutterForLinkEntering__horizontal6227_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__horizontal), sp0_); }
void replaceShutterForLinkEntering__right622e_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__right), sp0_); }
void replaceShutterForLinkEntering__left6232_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__left), sp0_); }
void replaceShutterForLinkEntering__doReplacement6236_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__doReplacement), sp0_); }
void replaceShutterForLinkEntering__doneReplacement6246_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, SYM(replaceShutterForLinkEntering__doneReplacement), sp0_); }

void replaceShutterForLinkEntering_hook(GB *gb) {
  BASE(replaceShutterForLinkEntering);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(wRoomLayout + 0xae);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, BC);
    CYC(b_+4, b_+5); push_effect(gb, BC);
    CYC(b_+5, b_+7); alu_sub(gb, 0x78);
    CYC(b_+7, b_+9); alu_cp(gb, 0x08);
    if (F & FC) CALL_C_CC(b_+9, replaceShutterForLinkEntering__temporarilyOpenDoor61e9_hook, b_+17, b_+12);
    else CYC(b_+9, b_+12);
    CYC(b_+12, b_+13); SET_BC(pop_effect(gb));
    CYC(b_+13, b_+14); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+14, b_+16); continue; }
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); ret_effect(gb);
    return;
  }
}

void replaceOpenedChest_hook(GB *gb) {
  BASE(replaceOpenedChest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, getChestData_hook, SYM(getChestData), b_+9);
  CYC(b_+9, b_+11); D = wRoomLayout >> 8;
  CYC(b_+11, b_+13); A = 0xf0;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

static void replace_switch_tiles(GB *gb, uint16_t entry) {
  BASE(replaceSwitchTiles);
  if (entry == b_+0) {
    CYC(b_+0, b_+3); SET_HL(b_+46);
    CYC(b_+3, b_+6); A = W8(wActiveGroup);
    CYC(b_+6, b_+8); alu_sub(gb, 0x04);
    if (F & FZ) {
      CYCT(b_+8, b_+10);
    } else {
      CYC(b_+8, b_+10);
      CYC(b_+10, b_+11); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
      CYC(b_+11, b_+12);
      CYC(b_+12, b_+15); SET_HL(GV(b_+95, 0x5fd1));
    }
    CYC(b_+15, b_+18); A = W8(wActiveRoom);
    CYC(b_+18, b_+19); B = A;
    CYC(b_+19, b_+22); A = W8(wSwitchState);
    CYC(b_+22, b_+23); C = A;
    CYC(b_+23, b_+25); D = wRoomLayout >> 8;
  } else if (entry == b_+41) {
    goto skip3;
  } else if (entry == b_+42) {
    goto skip2;
  }
next:
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+27, b_+28); ret_effect(gb); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto skip3; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+33); alu_and(gb, C);
  if (F & FZ) { CYCT(b_+33, b_+35); goto skip2; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); E = mem_rd(gb, HL);
  CYC(b_+37, b_+38); SET_HL(HL + 1);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+41); goto next;
skip3:
  CYC(b_+41, b_+42); SET_HL(HL + 1);
skip2:
  CYC(b_+42, b_+43); SET_HL(HL + 1);
  CYC(b_+43, b_+44); SET_HL(HL + 1);
  CYC(b_+44, b_+46); goto next;
}

void replaceSwitchTiles_hook(GB *gb) { replace_switch_tiles(gb, SYM(replaceSwitchTiles)); }
void replaceSwitchTiles__next6297_hook(GB *gb) { replace_switch_tiles(gb, SYM(replaceSwitchTiles__next)); }
void replaceSwitchTiles__skip3Bytes62a7_hook(GB *gb) { replace_switch_tiles(gb, SYM(replaceSwitchTiles__skip3Bytes)); }
void replaceSwitchTiles__skip2Bytes62a8_hook(GB *gb) { replace_switch_tiles(gb, SYM(replaceSwitchTiles__skip2Bytes)); }

static void apply_single_tile_changes(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(applySingleTileChanges);
  if (entry == b_+0) {
    CYC(b_+0, b_+3); A = W8(wActiveRoom);
    CYC(b_+3, b_+4); B = A;
    CALL_C(b_+4, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+7);
    CYC(b_+7, b_+8); C = A;
    CYC(b_+8, b_+10); D = wRoomLayout >> 8;
    CYC(b_+10, b_+13); A = W8(wActiveGroup);
    CYC(b_+13, b_+16); SET_HL(SYM(singleTileChangeGroupTable));
    CYC(b_+16, b_+17); bank4_add_double_index_to_hl(gb, b_+17);
    CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+18, b_+19); H = mem_rd(gb, HL);
    CYC(b_+19, b_+20); L = A;
  } else if (entry == b_+41) {
    goto match;
  } else if (entry == b_+48) {
    goto not_match;
  } else if (entry == b_+56) {
    goto unlinked;
  } else if (entry == b_+63) {
    goto linked;
  } else if (entry == b_+70) {
    goto finished;
  }
next:
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto not_match; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0xf0);
  if (F & FZ) { CYCT(b_+27, b_+29); goto unlinked; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); alu_cp(gb, 0xf1);
  if (F & FZ) { CYCT(b_+31, b_+33); goto linked; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0xf2);
  if (F & FZ) { CYCT(b_+35, b_+37); goto finished; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); alu_and(gb, C);
  if (F & FZ) { CYCT(b_+39, b_+41); goto not_match; }
  CYC(b_+39, b_+41);
match:
  CYC(b_+41, b_+42); SET_HL(HL + 1);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); E = A;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48); goto next;
not_match:
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+50); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); SET_HL(HL + 1);
  CYC(b_+52, b_+53); SET_HL(HL + 1);
  CYC(b_+53, b_+54); SET_HL(HL + 1);
  CYC(b_+54, b_+56); goto next;
unlinked:
  CALL_C(b_+56, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+59);
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto not_match; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); goto match;
linked:
  CALL_C(b_+63, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+66);
  if (F & FZ) { CYCT(b_+66, b_+68); goto not_match; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); goto match;
finished:
  CYC(b_+70, b_+72); A = 0x14;
  CYC(b_+72, b_+73); push_effect(gb, HL);
  CALL_C(b_+73, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+76);
  CYC(b_+76, b_+77); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(b_+77, b_+78); ret_effect(gb); return; }
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+80); goto match;
}

void applySingleTileChanges_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges), sp0_); }
void applySingleTileChanges__next62f2_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges__next), sp0_); }
void applySingleTileChanges__match6307_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges__match), sp0_); }
void applySingleTileChanges__notMatch630e_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges__notMatch), sp0_); }
void applySingleTileChanges__unlinkedOnly6316_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges__unlinkedOnly), sp0_); }
void applySingleTileChanges__linkedOnly631d_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges__linkedOnly), sp0_); }
void applySingleTileChanges__finishedGameOnly6324_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, SYM(applySingleTileChanges__finishedGameOnly), sp0_); }

static void update_changed_tile_queue(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(updateChangedTileQueue);
  if (entry == b_+0) {
    CYC(b_+0, b_+3); A = W8(wScrollMode);
    CYC(b_+3, b_+5); alu_and(gb, 0x0e);
    if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
    CYC(b_+5, b_+6);
    CYC(b_+6, b_+8); B = 0x04;
    for (;;) {
      CYC(b_+8, b_+9); push_effect(gb, BC);
      CALL_C(b_+9, updateChangedTileQueue__handleSingleEntry6c46_hook, b_+20, b_+12);
      CYC(b_+12, b_+13); SET_BC(pop_effect(gb));
      CYC(b_+13, b_+14); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+14, b_+16); continue; }
      CYC(b_+14, b_+16);
      break;
    }
    CYC(b_+16, b_+17); alu_xor(gb, A);
    CYC(b_+17, b_+19); mem_wr(gb, IO_SVBK, A);
    CYC(b_+19, b_+20); ret_effect(gb);
    return;
  }

  CYC(b_+20, b_+23); A = W8(wChangedTileQueueHead);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+27); A = W8(wChangedTileQueueTail);
  CYC(b_+27, b_+28); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+28, b_+29); ret_effect(gb); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+30); B = alu_inc8(gb, B);
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+33); alu_and(gb, 0x1f);
  CYC(b_+33, b_+36); W8(wChangedTileQueueHead) = A;
  CYC(b_+36, b_+39); SET_HL(w2ChangedTileQueue);
  CYC(b_+39, b_+40); bank4_add_double_index_to_hl(gb, b_+40);
  CYC(b_+40, b_+42); A = 0x02;
  CYC(b_+42, b_+44); mem_wr(gb, IO_SVBK, A);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); C = mem_rd(gb, HL);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+48); A = C;
  CYC(b_+48, b_+50); H8(hFF8C) = A;
  CYC(b_+50, b_+52); A = mem_rd(gb, IO_SVBK);
  CYC(b_+52, b_+53); push_effect(gb, AF);
  CYC(b_+53, b_+55); A = 0x03;
  CYC(b_+55, b_+57); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+57, getVramSubtileAddressOfTile_hook, SYM(getVramSubtileAddressOfTile), b_+60);
  CYC(b_+60, b_+61); A = B;
  CALL_C(b_+61, setHlToTileMappingDataPlusATimes8_hook, SYM(setHlToTileMappingDataPlusATimes8), b_+64);
  CYC(b_+64, b_+65); push_effect(gb, HL);
  CYC(b_+65, b_+66); push_effect(gb, DE);
  CALL_C(b_+66, write4BytesToVramLayout_hook, SYM(write4BytesToVramLayout), b_+69);
  CYC(b_+69, b_+70); SET_DE(pop_effect(gb));
  CYC(b_+70, b_+72); A = 0x04;
  CYC(b_+72, b_+73); alu_add(gb, D);
  CYC(b_+73, b_+74); D = A;
  CALL_C(b_+74, write4BytesToVramLayout_hook, SYM(write4BytesToVramLayout), b_+77);
  CYC(b_+77, b_+79); A = H8(hFF8C);
  CYC(b_+79, b_+80); SET_HL(pop_effect(gb));
  CALL_C(b_+80, queueTileWriteAtVBlank_hook, SYM(queueTileWriteAtVBlank), b_+83);
  CYC(b_+83, b_+84); SET_AF(pop_effect(gb));
  CYC(b_+84, b_+86); mem_wr(gb, IO_SVBK, A);
  CYC(b_+86, b_+87); ret_effect(gb);
}

void updateChangedTileQueue_hook(GB *gb) { uint16_t sp0_ = gb->sp; update_changed_tile_queue(gb, SYM(updateChangedTileQueue), sp0_); }
void updateChangedTileQueue__handleSingleEntry6c46_hook(GB *gb) { uint16_t sp0_ = gb->sp; update_changed_tile_queue(gb, SYM(updateChangedTileQueue__handleSingleEntry), sp0_); }

void write4BytesToVramLayout_hook(GB *gb) {
  BASE(write4BytesToVramLayout);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); mem_wr(gb, DE, A);
  CYC(b_+2, b_+3); E = alu_inc8(gb, E);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x1f;
  CYC(b_+7, b_+8); alu_add(gb, E);
  CYC(b_+8, b_+9); E = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); E = alu_inc8(gb, E);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void getVramSubtileAddressOfTile_hook(GB *gb) {
  BASE(getVramSubtileAddressOfTile);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); A = alu_swap(gb, A);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+8); SET_HL(b_+20);
  CYC(b_+8, b_+9); bank4_add_double_index_to_hl(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); H = mem_rd(gb, HL);
  CYC(b_+11, b_+12); L = A;
  CYC(b_+12, b_+13); A = C;
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); bank4_add_a_to_hl(gb, b_+17);
  CYC(b_+17, b_+18); E = L;
  CYC(b_+18, b_+19); D = H;
  CYC(b_+19, b_+20); ret_effect(gb);
}

static void set_interleaved_tile_body(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(setInterleavedTile_body);
  if (entry == b_+64) goto copy_2_bytes;
  if (entry == b_+71) goto interleave_diagonally;
  if (entry == b_+92) goto copy_2_bytes_separated;
  if (entry == b_+101) goto queue_write;

  CYC(b_+0, b_+2); H8(hFF8B) = A;
  CYC(b_+2, b_+4); A = mem_rd(gb, IO_SVBK);
  CYC(b_+4, b_+5); push_effect(gb, AF);
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, b_+9); mem_wr(gb, IO_SVBK, A);
  CYC(b_+9, b_+11); A = H8(hFF8F);
  CALL_C(b_+11, setHlToTileMappingDataPlusATimes8_hook, SYM(setHlToTileMappingDataPlusATimes8), b_+14);
  CYC(b_+14, b_+17); SET_DE(wEnemyPlacement_cec8);
  CYC(b_+17, b_+19); B = 0x08;
  for (;;) {
    CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+20, b_+21); mem_wr(gb, DE, A);
    CYC(b_+21, b_+22); SET_DE(DE + 1);
    CYC(b_+22, b_+23); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+23, b_+25); continue; }
    CYC(b_+23, b_+25);
    break;
  }
  CYC(b_+25, b_+27); A = H8(hFF8E);
  CALL_C(b_+27, setHlToTileMappingDataPlusATimes8_hook, SYM(setHlToTileMappingDataPlusATimes8), b_+30);
  CYC(b_+30, b_+33); SET_DE(wEnemyPlacement_cec8);
  CYC(b_+33, b_+35); A = H8(hFF8B);
  CYC(b_+35, b_+37); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto interleave_diagonally; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto copy_second_pair; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); SET_HL(HL + 1);
  CYC(b_+44, b_+45); SET_HL(HL + 1);
  CALL_C(b_+45, setInterleavedTile_body__copy2Bytes6cf3_hook, b_+64, b_+48);
  CYC(b_+48, b_+50); goto copy_final_pair;
copy_second_pair:
  CYC(b_+50, b_+51); SET_DE(DE + 1);
  CYC(b_+51, b_+52); SET_DE(DE + 1);
  CALL_C(b_+52, setInterleavedTile_body__copy2Bytes6cf3_hook, b_+64, b_+55);
copy_final_pair:
  CYC(b_+55, b_+56); SET_HL(HL + 1);
  CYC(b_+56, b_+57); SET_HL(HL + 1);
  CYC(b_+57, b_+58); SET_DE(DE + 1);
  CYC(b_+58, b_+59); SET_DE(DE + 1);
  CALL_C(b_+59, setInterleavedTile_body__copy2Bytes6cf3_hook, b_+64, b_+62);
  CYC(b_+62, b_+64); goto queue_write;

copy_2_bytes:
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+67); SET_DE(DE + 1);
  CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+70); SET_DE(DE + 1);
  CYC(b_+70, b_+71); ret_effect(gb);
  return;

interleave_diagonally:
  CYC(b_+71, b_+73); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+73, b_+75); goto diagonal_second_pair; }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); SET_DE(DE + 1);
  CALL_C(b_+76, setInterleavedTile_body__copy2BytesSeparated6d0f_hook, b_+92, b_+79);
  CYC(b_+79, b_+81); goto diagonal_final_pair;
diagonal_second_pair:
  CYC(b_+81, b_+82); SET_HL(HL + 1);
  CALL_C(b_+82, setInterleavedTile_body__copy2BytesSeparated6d0f_hook, b_+92, b_+85);
diagonal_final_pair:
  CYC(b_+85, b_+86); SET_HL(HL + 1);
  CYC(b_+86, b_+87); SET_DE(DE + 1);
  CALL_C(b_+87, setInterleavedTile_body__copy2BytesSeparated6d0f_hook, b_+92, b_+90);
  CYC(b_+90, b_+92); goto queue_write;

copy_2_bytes_separated:
  CYC(b_+92, b_+93); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+95); SET_DE(DE + 1);
  CYC(b_+95, b_+96); SET_HL(HL + 1);
  CYC(b_+96, b_+97); SET_DE(DE + 1);
  CYC(b_+97, b_+98); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+100); SET_DE(DE + 1);
  CYC(b_+100, b_+101); ret_effect(gb);
  return;

queue_write:
  CYC(b_+101, b_+103); A = H8(hFF8C);
  CYC(b_+103, b_+106); SET_HL(wEnemyPlacement_cec8);
  CALL_C(b_+106, queueTileWriteAtVBlank_hook, SYM(queueTileWriteAtVBlank), b_+109);
  CYC(b_+109, b_+110); SET_AF(pop_effect(gb));
  CYC(b_+110, b_+112); mem_wr(gb, IO_SVBK, A);
  CYC(b_+112, b_+113); ret_effect(gb);
}

void setInterleavedTile_body_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, SYM(setInterleavedTile_body), sp0_); }
void setInterleavedTile_body__copy2Bytes6cf3_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, SYM(setInterleavedTile_body__copy2Bytes), sp0_); }
void setInterleavedTile_body__interleaveDiagonally6cfa_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, SYM(setInterleavedTile_body__interleaveDiagonally), sp0_); }
void setInterleavedTile_body__copy2BytesSeparated6d0f_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, SYM(setInterleavedTile_body__copy2BytesSeparated), sp0_); }
void setInterleavedTile_body__queueWrite6d18_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, SYM(setInterleavedTile_body__queueWrite), sp0_); }

static void queue_tile_write_at_vblank(GB *gb, uint16_t entry, uint16_t sp0_) {
  BASE(queueTileWriteAtVBlank);
  if (entry == b_+41) goto copy_2_bytes;
  if (entry == b_+48) goto get_tile_position_in_vram;

  CYC(b_+0, b_+1); push_effect(gb, HL);
  CALL_C(b_+1, queueTileWriteAtVBlank__getTilePositionInVram6d54_hook, b_+48, b_+4);
  CYC(b_+4, b_+6); alu_add(gb, 0x20);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); A = H8(hVBlankFunctionQueueTail);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); H = 0xc4;
  CYC(b_+12, b_+15); A = mem_rd(gb, SYM(vblankCopyTileFunctionOffset));
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, E);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); mem_wr(gb, HL, D);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+21); E = L;
  CYC(b_+21, b_+22); D = H;
  CYC(b_+22, b_+23); SET_HL(pop_effect(gb));
  CYC(b_+23, b_+25); B = 0x02;
  for (;;) {
    CALL_C(b_+25, queueTileWriteAtVBlank__copy2Bytes6d4d_hook, b_+41, b_+28);
    CYC(b_+28, b_+29); A = C;
    CYC(b_+29, b_+30); mem_wr(gb, DE, A);
    CYC(b_+30, b_+31); E = alu_inc8(gb, E);
    CALL_C(b_+31, queueTileWriteAtVBlank__copy2Bytes6d4d_hook, b_+41, b_+34);
    CYC(b_+34, b_+35); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+35, b_+37); continue; }
    CYC(b_+35, b_+37);
    break;
  }
  CYC(b_+37, b_+38); A = E;
  CYC(b_+38, b_+40); H8(hVBlankFunctionQueueTail) = A;
  CYC(b_+40, b_+41); ret_effect(gb);
  return;

copy_2_bytes:
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+44); E = alu_inc8(gb, E);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); E = alu_inc8(gb, E);
  CYC(b_+47, b_+48); ret_effect(gb);
  return;

get_tile_position_in_vram:
  CYC(b_+48, b_+49); E = A;
  CYC(b_+49, b_+51); alu_and(gb, 0xf0);
  CYC(b_+51, b_+53); A = alu_swap(gb, A);
  CYC(b_+53, b_+54); D = A;
  CYC(b_+54, b_+55); A = E;
  CYC(b_+55, b_+57); alu_and(gb, 0x0f);
  CYC(b_+57, b_+58); alu_add(gb, A);
  CYC(b_+58, b_+59); E = A;
  CYC(b_+59, b_+62); A = W8(wScreenOffsetX);
  CYC(b_+62, b_+64); A = alu_swap(gb, A);
  CYC(b_+64, b_+65); alu_add(gb, A);
  CYC(b_+65, b_+66); alu_add(gb, E);
  CYC(b_+66, b_+68); alu_and(gb, 0x1f);
  CYC(b_+68, b_+69); E = A;
  CYC(b_+69, b_+72); A = W8(wScreenOffsetY);
  CYC(b_+72, b_+74); A = alu_swap(gb, A);
  CYC(b_+74, b_+75); alu_add(gb, D);
  CYC(b_+75, b_+77); alu_and(gb, 0x0f);
  CYC(b_+77, b_+80); SET_HL(SYM(vramBgMapTable));
  CYC(b_+80, b_+81); bank4_add_double_index_to_hl(gb, b_+81);
  CYC(b_+81, b_+82); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+82, b_+83); alu_add(gb, E);
  CYC(b_+83, b_+84); E = A;
  CYC(b_+84, b_+85); D = mem_rd(gb, HL);
  CYC(b_+85, b_+86); ret_effect(gb);
}

void queueTileWriteAtVBlank_hook(GB *gb) { uint16_t sp0_ = gb->sp; queue_tile_write_at_vblank(gb, SYM(queueTileWriteAtVBlank), sp0_); }
void queueTileWriteAtVBlank__copy2Bytes6d4d_hook(GB *gb) { uint16_t sp0_ = gb->sp; queue_tile_write_at_vblank(gb, SYM(queueTileWriteAtVBlank__copy2Bytes), sp0_); }
void queueTileWriteAtVBlank__getTilePositionInVram6d54_hook(GB *gb) { uint16_t sp0_ = gb->sp; queue_tile_write_at_vblank(gb, SYM(queueTileWriteAtVBlank__getTilePositionInVram), sp0_); }

void generateW3VramTilesAndAttributes_hook(GB *gb) {
  BASE(generateW3VramTilesAndAttributes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); SET_HL(wRoomLayout);
  CYC(b_+7, b_+10); SET_DE(w3VramTiles);
  CYC(b_+10, b_+12); C = 0x0b;
  for (;;) {
    CYC(b_+12, b_+14); B = 0x10;
    for (;;) {
      CYC(b_+14, b_+15); push_effect(gb, BC);
      CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+16, b_+17); push_effect(gb, HL);
      CALL_C(b_+17, setHlToTileMappingDataPlusATimes8_hook, SYM(setHlToTileMappingDataPlusATimes8), b_+20);
      CYC(b_+20, b_+21); push_effect(gb, DE);
      CALL_C(b_+21, write4BytesToVramLayout_hook, SYM(write4BytesToVramLayout), b_+24);
      CYC(b_+24, b_+25); SET_DE(pop_effect(gb));
      CYC(b_+25, b_+27); D = (uint8_t)(D | 0x04);
      CALL_C(b_+27, write4BytesToVramLayout_hook, SYM(write4BytesToVramLayout), b_+30);
      CYC(b_+30, b_+32); D = (uint8_t)(D & ~0x04);
      CYC(b_+32, b_+33); A = E;
      CYC(b_+33, b_+35); alu_sub(gb, 0x1f);
      CYC(b_+35, b_+36); E = A;
      CYC(b_+36, b_+37); SET_HL(pop_effect(gb));
      CYC(b_+37, b_+38); SET_BC(pop_effect(gb));
      CYC(b_+38, b_+39); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+39, b_+41); continue; }
      CYC(b_+39, b_+41);
      break;
    }
    CYC(b_+41, b_+43); A = 0x20;
    CALL_C(b_+43, addAToDe_hook, 0x0068, b_+46);
    CYC(b_+46, b_+47); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+47, b_+49); continue; }
    CYC(b_+47, b_+49);
    break;
  }
  CYC(b_+49, b_+50); ret_effect(gb);
}
