#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

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
  b4_vblank_function(gb, 0x4000, 0x00);
}

void b4VBlankFunction1_hook(GB *gb) {
  b4_vblank_function(gb, 0x402e, 0x01);
}

void b4VBlankFunction2_hook(GB *gb) {
  b4_vblank_function(gb, 0x405c, 0x02);
}

void b4VBlankFunction3_hook(GB *gb) {
  b4_vblank_function(gb, 0x408a, 0x03);
}

void b4VBlankFunction4_hook(GB *gb) {
  b4_vblank_function(gb, 0x40b8, 0x04);
}

void b4VBlankFunction5_hook(GB *gb) {
  b4_vblank_function(gb, 0x40e6, 0x05);
}

void b4VBlankFunction6_hook(GB *gb) {
  b4_vblank_function(gb, 0x4114, 0x06);
}

void b4VBlankFunction7_hook(GB *gb) {
  b4_vblank_function(gb, 0x4142, 0x07);
}

void b4VBlankFunction8_hook(GB *gb) {
  b4_vblank_function(gb, 0x4170, 0x08);
}

void b4VBlankFunction9_hook(GB *gb) {
  b4_vblank_function(gb, 0x419e, 0x09);
}

void b4VBlankFunction10_hook(GB *gb) {
  b4_vblank_function(gb, 0x41cc, 0x0a);
}

void b4VBlankFunction11_hook(GB *gb) {
  b4_vblank_function(gb, 0x41fa, 0x0b);
}

void b4VBlankFunction12_hook(GB *gb) {
  b4_vblank_function(gb, 0x4228, 0x0c);
}

void b4VBlankFunction13_hook(GB *gb) {
  b4_vblank_function(gb, 0x4256, 0x0d);
}

void b4VBlankFunction14_hook(GB *gb) {
  b4_vblank_function(gb, 0x4284, 0x0e);
}

void b4VBlankFunction15_hook(GB *gb) {
  b4_vblank_function(gb, 0x42b2, 0x0f);
}

void b4VBlankFunction16_hook(GB *gb) {
  b4_vblank_function(gb, 0x42e0, 0x10);
}

void b4VBlankFunction17_hook(GB *gb) {
  b4_vblank_function(gb, 0x430e, 0x11);
}

void b4VBlankFunction18_hook(GB *gb) {
  b4_vblank_function(gb, 0x433c, 0x12);
}

void b4VBlankFunction19_hook(GB *gb) {
  b4_vblank_function(gb, 0x436a, 0x13);
}

void b4VBlankFunction20_hook(GB *gb) {
  b4_vblank_function(gb, 0x4398, 0x14);
}

void b4VBlankFunction21_hook(GB *gb) {
  b4_vblank_function(gb, 0x43c6, 0x15);
}

void b4VBlankFunction22_hook(GB *gb) {
  b4_vblank_function(gb, 0x43f4, 0x16);
}

void b4VBlankFunction23_hook(GB *gb) {
  b4_vblank_function(gb, 0x4422, 0x17);
}

void b4VBlankFunction24_hook(GB *gb) {
  b4_vblank_function(gb, 0x4450, 0x18);
}

void b4VBlankFunction25_hook(GB *gb) {
  b4_vblank_function(gb, 0x447e, 0x19);
}

void b4VBlankFunction26_hook(GB *gb) {
  b4_vblank_function(gb, 0x44ac, 0x1a);
}

void b4VBlankFunction27_hook(GB *gb) {
  b4_vblank_function(gb, 0x44da, 0x1b);
}

void b4VBlankFunction28_hook(GB *gb) {
  b4_vblank_function(gb, 0x4508, 0x1c);
}

void b4VBlankFunction29_hook(GB *gb) {
  b4_vblank_function(gb, 0x4536, 0x1d);
}

void b4VBlankFunction30_hook(GB *gb) {
  b4_vblank_function(gb, 0x4564, 0x1e);
}

void b4VBlankFunction31_hook(GB *gb) {
  b4_vblank_function(gb, 0x4592, 0x1f);
}

static void applyWarpDest_finish(GB *gb) {
  CYC(0x45f5, 0x45f7); A = 0x0a;
  CYC(0x45f7, 0x45fa); W8(wLinkForceState) = A;
  CYC(0x45fa, 0x45fd); A = W8(wActiveGroup);
  CYC(0x45fd, 0x45ff); hram_wr(gb, 0x8b, A);
  CYC(0x45ff, 0x4602); A = W8(wWarpDestGroup);
  CYC(0x4602, 0x4604); alu_and(gb, 0x07);
  CYC(0x4604, 0x4607); W8(wActiveGroup) = A;
  CYC(0x4607, 0x460a); A = W8(wWarpDestRoom);
  CYC(0x460a, 0x460d); W8(wActiveRoom) = A;
  CYC(0x460d, 0x4610); SET_HL(w1Link_enabled);
  CYC(0x4610, 0x4612); mem_wr(gb, HL, 0x03);
  CYC(0x4612, 0x4615); A = W8(wWarpDestPos);
  CYC(0x4615, 0x4616); B = A;
  CYC(0x4616, 0x4618); alu_and(gb, 0xf0);
  CYC(0x4618, 0x461a); alu_or(gb, 0x08);
  CYC(0x461a, 0x461c); L = w1Link_yh & 0xff;
  CYC(0x461c, 0x461d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x461d, 0x461e); L = alu_inc8(gb, L);
  CYC(0x461e, 0x461f); A = B;
  CYC(0x461f, 0x4621); alu_and(gb, 0x0f);
  CYC(0x4621, 0x4623); alu_swap_a(gb);
  CYC(0x4623, 0x4625); alu_or(gb, 0x08);
  CYC(0x4625, 0x4626); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4626, 0x4629); loadScreenMusicAndSetRoomPack_hook(gb);
}

void vblankRunBank4Function_b04_hook(GB *gb) {
  CYC(0x45c0, 0x45c1); SET_HL(pop_effect(gb));
  CYC(0x45c1, 0x45c2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45c2, 0x45c4); hram_wr(gb, 0x4f, A);
  CYC(0x45c4, 0x45c5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45c5, 0x45c6); E = A;
  CYC(0x45c6, 0x45c8); B = 0xcd;
  CYC(0x45c8, 0x45ca); D = 0x98;
  CYC(0x45ca, 0x45cb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45cb, 0x45cc); H = mem_rd(gb, HL);
  CYC(0x45cc, 0x45cd); L = alu_inc8(gb, L);
  CYC(0x45cd, 0x45ce); C = L;
  CYC(0x45ce, 0x45cf); L = A;
  CYC(0x45cf, 0x45d0); hook_handoff(gb, HL);
}

void label_04_033_hook(GB *gb) {
  CYC(0x45e1, 0x45e2); C = A;
  CYC(0x45e2, 0x45e4); B = 0x00;
  CYC(0x45e4, 0x45e5); alu_add_hl(gb, BC);
  CYC(0x45e5, 0x45e6); alu_add_hl(gb, BC);
  CYC(0x45e6, 0x45e7); alu_add_hl(gb, BC);
  CYC(0x45e7, 0x45e8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45e8, 0x45eb); W8(wWarpDestRoom) = A;
  CYC(0x45eb, 0x45ec); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45ec, 0x45ef); W8(wWarpDestPos) = A;
  CYC(0x45ef, 0x45f0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45f0, 0x45f2); alu_or(gb, 0x80);
  CYC(0x45f2, 0x45f5); W8(wWarpTransition) = A;
  applyWarpDest_finish(gb);
}

void label_04_032_hook(GB *gb) {
  CYC(0x45d7, 0x45da); SET_HL(0x6f5b);
  CYC(0x45da, 0x45db); bank4_add_double_index_to_hl(gb, 0x45db);
  CYC(0x45db, 0x45dc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45dc, 0x45dd); H = mem_rd(gb, HL);
  CYC(0x45dd, 0x45de); L = A;
  CYC(0x45de, 0x45e1); A = W8(wWarpDestRoom);
  label_04_033_hook(gb);
}

void applyWarpDest_b04_hook(GB *gb) {
  CYC(0x45d0, 0x45d3); A = W8(wWarpDestGroup);
  CYC(0x45d3, 0x45d5); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x45d5, 0x45d7);
    applyWarpDest_finish(gb);
    return;
  }
  CYC(0x45d5, 0x45d7);
  label_04_032_hook(gb);
}

void findWarpSourceAndDest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4629, 0x462c); A = W8(wDisableWarps);
  CYC(0x462c, 0x462d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x462d, 0x4630);
    setWarpDestDefault_hook(gb);
    return;
  }
  CYC(0x462d, 0x4630);
  CYC(0x4630, 0x4633); A = W8(wActiveGroup);
  CYC(0x4633, 0x4636); SET_HL(0x759e);
  CYC(0x4636, 0x4637); bank4_add_double_index_to_hl(gb, 0x4637);
  CYC(0x4637, 0x4638); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4638, 0x4639); H = mem_rd(gb, HL);
  CYC(0x4639, 0x463a); L = A;
  CYC(0x463a, 0x463d); A = W8(wActiveRoom);
  CYC(0x463d, 0x463e); B = A;

  for (;;) {
    CYC(0x463e, 0x463f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x463f, 0x4641); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(0x4641, 0x4643);
      break;
    }
    CYC(0x4641, 0x4643);
    CYC(0x4643, 0x4645); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(0x4645, 0x4647);
      goto found;
    }
    CYC(0x4645, 0x4647);
    CYC(0x4647, 0x4649); alu_bit(gb, 6, A);
    if (!(F & FZ)) {
      CYCT(0x4649, 0x464b);
      CYC(0x4658, 0x4659); A = mem_rd(gb, HL);
      CYC(0x4659, 0x465a); alu_cp(gb, B);
      if (!(F & FZ)) {
        CYCT(0x465a, 0x465c);
        goto skip;
      }
      CYC(0x465a, 0x465c);
      CYC(0x465c, 0x465d); SET_HL(HL + 1);
      CYC(0x465d, 0x465e); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x465e, 0x465f); H = mem_rd(gb, HL);
      CYC(0x465f, 0x4660); L = A;
      CYC(0x4660, 0x4662); A = hram_rd(gb, 0x8d);
      CYC(0x4662, 0x4663); B = A;
      CYC(0x4663, 0x4665);
      continue;
    }
    CYC(0x4649, 0x464b);
    CYC(0x464b, 0x464d); alu_and(gb, 0x0f);
    if (!(F & FZ)) {
      CYCT(0x464d, 0x464f);
      goto skip;
    }
    CYC(0x464d, 0x464f);
    CYC(0x464f, 0x4650); A = mem_rd(gb, HL);
    CYC(0x4650, 0x4651); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(0x4651, 0x4653);
      goto found;
    }
    CYC(0x4651, 0x4653);

skip:
    CYC(0x4653, 0x4654); SET_HL(HL + 1);
    CYC(0x4654, 0x4655); SET_HL(HL + 1);
    CYC(0x4655, 0x4656); SET_HL(HL + 1);
    CYC(0x4656, 0x4658);
  }

  CYC(0x467a, 0x467d); A = W8(wTilesetFlags);
  CYC(0x467d, 0x467f); alu_and(gb, 0x08);
  if (F & FZ) {
    CYCT(0x467f, 0x4681);
    setWarpDestDefault_hook(gb);
    return;
  }
  CYC(0x467f, 0x4681);
  CYC(0x4681, 0x4683); A = hram_rd(gb, 0x8c);
  CYC(0x4683, 0x4684); alu_rrca(gb);
  CYC(0x4684, 0x4686); B = 0x01;
  if (!(F & FC)) {
    CYCT(0x4686, 0x4688);
  } else {
    CYC(0x4686, 0x4688);
    CYC(0x4688, 0x468a); B = 0xff;
  }
  CYC(0x468a, 0x468d); A = W8(wDungeonFloor);
  CYC(0x468d, 0x468e); alu_add(gb, B);
  CYC(0x468e, 0x4691); W8(wDungeonFloor) = A;
  CALL_C(0x4691, getActiveRoomFromDungeonMapPosition_hook, 0x2e12, 0x4694);
  CYC(0x4694, 0x4697); W8(wWarpDestRoom) = A;
  CYC(0x4697, 0x4699); A = hram_rd(gb, 0x8d);
  CYC(0x4699, 0x469c); W8(wWarpDestPos) = A;
  CYC(0x469c, 0x469f); A = W8(wActiveGroup);
  CYC(0x469f, 0x46a1); alu_or(gb, 0x80);
  CYC(0x46a1, 0x46a4); W8(wWarpDestGroup) = A;
  CYC(0x46a4, 0x46a5); alu_xor(gb, A);
  CYC(0x46a5, 0x46a8); W8(wWarpTransition) = A;
  CYC(0x46a8, 0x46aa); A = 0x03;
  CYC(0x46aa, 0x46ad); W8(wWarpTransition2) = A;
  CYC(0x46ad, 0x46af); A = 0x6e;
  CYC(0x46af, 0x46b2); playSound_b00_hook(gb);
  return;

found:
  CYC(0x4665, 0x4666); SET_HL(HL + 1);
  CYC(0x4666, 0x4667); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4667, 0x466a); W8(wWarpDestRoom) = A;
  CYC(0x466a, 0x466b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x466b, 0x466c); B = A;
  CYC(0x466c, 0x466e); alu_swap_a(gb);
  CYC(0x466e, 0x4670); alu_and(gb, 0x0f);
  CYC(0x4670, 0x4673); W8(wWarpDestGroup) = A;
  CYC(0x4673, 0x4674); A = B;
  CYC(0x4674, 0x4676); alu_and(gb, 0x0f);
  CYC(0x4676, 0x4679); W8(wWarpTransition) = A;
  CYC(0x4679, 0x467a); ret_effect(gb);
}

void setWarpDestDefault_hook(GB *gb) {
  CYC(0x46b2, 0x46b5); SET_HL(wWarpDestGroup);
  CYC(0x46b5, 0x46b8); A = W8(wActiveGroup);
  CYC(0x46b8, 0x46ba); alu_or(gb, 0x80);
  CYC(0x46ba, 0x46bb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x46bb, 0x46be); A = W8(wActiveRoom);
  CYC(0x46be, 0x46bf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x46bf, 0x46c1); mem_wr(gb, HL, 0x00);
  CYC(0x46c1, 0x46c2); L = alu_inc8(gb, L);
  CYC(0x46c2, 0x46c4); A = hram_rd(gb, 0x8d);
  CYC(0x46c4, 0x46c5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x46c5, 0x46c7); mem_wr(gb, HL, 0x03);
  CYC(0x46c7, 0x46c8); ret_effect(gb);
}

void findScreenEdgeWarpSource_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46c8, 0x46cb); A = W8(wScrollMode);
  CYC(0x46cb, 0x46cd); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(0x46cd, 0x46ce); ret_effect(gb);
    return;
  }
  CYC(0x46cd, 0x46ce);
  CYC(0x46ce, 0x46d1); A = W8(wScreenTransitionDirection);
  CYC(0x46d1, 0x46d2); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x46d2, 0x46d3); ret_effect(gb);
    return;
  }
  CYC(0x46d2, 0x46d3);
  CALL_C(0x46d3, getLinkWarpQuadrant_hook, 0x4742, 0x46d6);
  CYC(0x46d6, 0x46d9); SET_HL(0x00f8);
  CYC(0x46d9, 0x46da); alu_add(gb, L);
  CYC(0x46da, 0x46db); L = A;
  CYC(0x46db, 0x46dc); B = mem_rd(gb, HL);
  CYC(0x46dc, 0x46df); A = W8(wActiveGroup);
  CYC(0x46df, 0x46e2); SET_HL(0x759e);
  CYC(0x46e2, 0x46e3); bank4_add_double_index_to_hl(gb, 0x46e3);
  CYC(0x46e3, 0x46e4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x46e4, 0x46e5); H = mem_rd(gb, HL);
  CYC(0x46e5, 0x46e6); L = A;
  CYC(0x46e6, 0x46e9); A = W8(wActiveRoom);
  CYC(0x46e9, 0x46ea); C = A;

  for (;;) {
    CYC(0x46ea, 0x46eb); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x46eb, 0x46ed); alu_bit(gb, 7, A);
    if (!(F & FZ)) {
      CYCT(0x46ed, 0x46ee); ret_effect(gb);
      return;
    }
    CYC(0x46ed, 0x46ee);
    CYC(0x46ee, 0x46f0); alu_bit(gb, 6, A);
    if (!(F & FZ)) {
      CYCT(0x46f0, 0x46f2);
      CYC(0x46ff, 0x4700); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x4700, 0x4701); alu_cp(gb, C);
      if (!(F & FZ)) {
        CYCT(0x4701, 0x4703);
        goto edge_skip;
      }
      CYC(0x4701, 0x4703);
      CYC(0x4703, 0x4704); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x4704, 0x4705); H = mem_rd(gb, HL);
      CYC(0x4705, 0x4706); L = A;
      CYC(0x4706, 0x4708);
      continue;
    }
    CYC(0x46f0, 0x46f2);
    CYC(0x46f2, 0x46f3); E = A;
    CYC(0x46f3, 0x46f4); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x46f4, 0x46f5); alu_cp(gb, C);
    if (!(F & FZ)) {
      CYCT(0x46f5, 0x46f7);
      goto edge_skip;
    }
    CYC(0x46f5, 0x46f7);
    CYC(0x46f7, 0x46f8); A = E;
    CYC(0x46f8, 0x46f9); alu_and(gb, B);
    if (!(F & FZ)) {
      CYCT(0x46f9, 0x46fb);
      break;
    }
    CYC(0x46f9, 0x46fb);

edge_skip:
    CYC(0x46fb, 0x46fc); SET_HL(HL + 1);
    CYC(0x46fc, 0x46fd); SET_HL(HL + 1);
    CYC(0x46fd, 0x46ff);
  }

  CYC(0x4708, 0x4709); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4709, 0x470c); W8(wWarpDestRoom) = A;
  CYC(0x470c, 0x470d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x470d, 0x470e); B = A;
  CYC(0x470e, 0x4710); alu_swap_a(gb);
  CYC(0x4710, 0x4712); alu_and(gb, 0x0f);
  CYC(0x4712, 0x4715); W8(wWarpDestGroup) = A;
  CYC(0x4715, 0x4716); A = B;
  CYC(0x4716, 0x4718); alu_and(gb, 0x0f);
  CYC(0x4718, 0x4719); B = A;
  CYC(0x4719, 0x471c); A = W8(wScreenTransitionDirection);
  CYC(0x471c, 0x471d); alu_rlca(gb);
  CYC(0x471d, 0x471f); alu_swap_a(gb);
  CYC(0x471f, 0x4721); alu_and(gb, 0x40);
  CYC(0x4721, 0x4722); alu_or(gb, B);
  CYC(0x4722, 0x4725); W8(wWarpTransition) = A;
  CYC(0x4725, 0x4728); A = W8(wLinkObjectIndex);
  CYC(0x4728, 0x472a); alu_cp(gb, 0xd0);
  if (!(F & FZ)) {
    CALL_C_CC(0x472a, func_04_4732_hook, 0x4732, 0x472d);
  } else {
    CYC(0x472a, 0x472d);
  }
  CYC(0x472d, 0x472e); alu_xor(gb, A);
  CYC(0x472e, 0x4731); W8(wTmpcec0) = A;
  CYC(0x4731, 0x4732); ret_effect(gb);
}

void func_04_4732_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4732, 0x4733); push_effect(gb, HL);
  CALL_C(0x4733, dismountCompanionAndSetRememberedPositionToScreenCenter_hook, 0x3431, 0x4736);
  CYC(0x4736, 0x4738); A = 0x01;
  CYC(0x4738, 0x473b); W8(wWarpTransition) = A;
  CYC(0x473b, 0x473d); A = 0x01;
  CYC(0x473d, 0x4740); W8(wWarpTransition2) = A;
  CYC(0x4740, 0x4741); SET_HL(pop_effect(gb));
  CYC(0x4741, 0x4742); ret_effect(gb);
}

void getLinkWarpQuadrant_hook(GB *gb) {
  CYC(0x4742, 0x4745); A = W8(wScreenTransitionDirection);
  CYC(0x4745, 0x4746); B = A;
  CYC(0x4746, 0x4749); A = W8(wActiveGroup);
  CYC(0x4749, 0x474b); alu_cp(gb, 0x04);
  CYC(0x474b, 0x474e); A = W8(w1Link_xh);
  if (!(F & FC)) {
    CYCT(0x474e, 0x4750);
    CYC(0x4756, 0x4758); alu_cp(gb, 0x80);
    CYC(0x4758, 0x4759); A = B;
    if (F & FC) {
      CYCT(0x4759, 0x475a); ret_effect(gb);
      return;
    }
    CYC(0x4759, 0x475a);
    CYC(0x475a, 0x475b); A = alu_inc8(gb, A);
    CYC(0x475b, 0x475c); ret_effect(gb);
    return;
  }
  CYC(0x474e, 0x4750);
  CYC(0x4750, 0x4752); alu_cp(gb, 0x58);
  CYC(0x4752, 0x4753); A = B;
  if (F & FC) {
    CYCT(0x4753, 0x4754); ret_effect(gb);
    return;
  }
  CYC(0x4753, 0x4754);
  CYC(0x4754, 0x4755); A = alu_inc8(gb, A);
  CYC(0x4755, 0x4756); ret_effect(gb);
}

void replaceToggleBlocks_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x617c, checkDungeonUsesToggleBlocks_hook, 0x364b, 0x617f);
  if (F & FZ) { CYCT(0x617f, 0x6180); ret_effect(gb); return; }
  CYC(0x617f, 0x6180);
  CYC(0x6180, 0x6183); SET_HL(0x7a77);
  CYC(0x6183, 0x6185); E = 0x02;
  CALL_C(0x6185, interBankCall_hook, 0x008a, 0x6188);
  CYC(0x6188, 0x618b); SET_DE(0x6197);
  CYC(0x618b, 0x618e); A = W8(wToggleBlocksState);
  CYC(0x618e, 0x618f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x618f, 0x6191);
  } else {
    CYC(0x618f, 0x6191);
    CYC(0x6191, 0x6194); SET_DE(0x619c);
  }
  CYC(0x6194, 0x6197); replaceTiles_hook(gb);
}

void replaceJabuTilesIfUnderwater_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61a1, 0x61a4); A = W8(wDungeonIndex);
  CYC(0x61a4, 0x61a6); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x61a6, 0x61a7); ret_effect(gb); return; }
  CYC(0x61a6, 0x61a7);
  CYC(0x61a7, 0x61aa); A = W8(wTilesetFlags);
  CYC(0x61aa, 0x61ac); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x61ac, 0x61ad); ret_effect(gb); return; }
  CYC(0x61ac, 0x61ad);
  CYC(0x61ad, 0x61b0); A = W8(wDungeonFloor);
  CYC(0x61b0, 0x61b1); B = A;
  CYC(0x61b1, 0x61b4); A = W8(wJabuWaterLevel);
  CYC(0x61b4, 0x61b6); alu_and(gb, 0x07);
  CYC(0x61b6, 0x61b7); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x61b7, 0x61b8); ret_effect(gb); return; }
  CYC(0x61b7, 0x61b8);
  CYC(0x61b8, 0x61bb); SET_DE(0x61c4);
  CALL_C(0x61bb, replaceTiles_hook, 0x6096, 0x61be);
  CYC(0x61be, 0x61c1); SET_DE(0x61cf);
  CYC(0x61c1, 0x61c4); replaceTiles_hook(gb);
}

static void replace_shutter_tail(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x61e9) {
    CYC(0x61e9, 0x61ec); SET_DE(0x625f);
    CALL_C(0x61ec, addDoubleIndexToDe_hook, 0x0072, 0x61ef);
    CYC(0x61ef, 0x61f0); A = mem_rd(gb, DE);
    CYC(0x61f0, 0x61f2); H8(hFF8B) = A;
    CYC(0x61f2, 0x61f3); SET_DE(DE + 1);
    CYC(0x61f3, 0x61f4); A = mem_rd(gb, DE);
    CYC(0x61f4, 0x61f5); E = A;
    CYC(0x61f5, 0x61f8); A = W8(wScrollMode);
    CYC(0x61f8, 0x61fa); alu_and(gb, 0x08);
    if (F & FZ) { CYCT(0x61fa, 0x61fc); goto done; }
    CYC(0x61fa, 0x61fc);
    CYC(0x61fc, 0x61ff); A = W8(wLinkObjectIndex);
    CYC(0x61ff, 0x6200); H = A;
    CYC(0x6200, 0x6203); A = W8(wScreenTransitionDirection);
    CYC(0x6203, 0x6205); alu_xor(gb, 0x02);
    CYC(0x6205, 0x6206); D = A;
    CYC(0x6206, 0x6207); A = E;
    CYC(0x6207, 0x6209); alu_and(gb, 0x03);
    CYC(0x6209, 0x620a); alu_cp(gb, D);
    if (!(F & FZ)) { CYCT(0x620a, 0x620b); ret_effect(gb); return; }
    CYC(0x620a, 0x620b);
    CYC(0x620b, 0x620e); A = W8(wScreenTransitionDirection);
    CYC(0x620e, 0x6210); alu_bit(gb, 0, A);
    if (!(F & FZ)) { CYCT(0x6210, 0x6212); goto horizontal; }
    CYC(0x6210, 0x6212);
    CYC(0x6212, 0x6214); alu_and(gb, 0x02);
    CYC(0x6214, 0x6216); L = 0x0d;
    CYC(0x6216, 0x6217); A = mem_rd(gb, HL);
    if (!(F & FZ)) { CYCT(0x6217, 0x6219); goto down; }
    CYC(0x6217, 0x6219);
    goto up;
  }
  if (entry == 0x6219) goto up;
  if (entry == 0x6221) goto down;
  if (entry == 0x6227) goto horizontal;
  if (entry == 0x622e) goto right;
  if (entry == 0x6232) goto left;
  if (entry == 0x6236) goto do_replacement;
  goto done;

up:
  CYC(0x6219, 0x621b); alu_and(gb, 0xf0);
  CYC(0x621b, 0x621d); alu_swap_a(gb);
  CYC(0x621d, 0x621f); alu_or(gb, 0xa0);
  CYC(0x621f, 0x6221); goto do_replacement;
down:
  CYC(0x6221, 0x6223); alu_and(gb, 0xf0);
  CYC(0x6223, 0x6225); alu_swap_a(gb);
  CYC(0x6225, 0x6227); goto do_replacement;
horizontal:
  CYC(0x6227, 0x6229); alu_and(gb, 0x02);
  CYC(0x6229, 0x622b); L = 0x0b;
  CYC(0x622b, 0x622c); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(0x622c, 0x622e); goto left; }
  CYC(0x622c, 0x622e);
right:
  CYC(0x622e, 0x6230); alu_and(gb, 0xf0);
  CYC(0x6230, 0x6232); goto do_replacement;
left:
  CYC(0x6232, 0x6234); alu_and(gb, 0xf0);
  CYC(0x6234, 0x6236); alu_or(gb, 0x0e);
do_replacement:
  CYC(0x6236, 0x6237); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x6237, 0x6239); goto done; }
  CYC(0x6237, 0x6239);
  CYC(0x6239, 0x623a); push_effect(gb, BC);
  CYC(0x623a, 0x623b); C = A;
  CYC(0x623b, 0x623c); A = mem_rd(gb, BC);
  CYC(0x623c, 0x623e); alu_sub(gb, 0x78);
  CYC(0x623e, 0x6240); alu_cp(gb, 0x08);
  if (!(F & FC)) {
    CYCT(0x6240, 0x6242);
  } else {
    CYC(0x6240, 0x6242);
    CYC(0x6242, 0x6244); A = H8(hFF8B);
    CYC(0x6244, 0x6245); mem_wr(gb, BC, A);
  }
  CYC(0x6245, 0x6246); SET_BC(pop_effect(gb));
done:
  CYC(0x6246, 0x6247); A = E;
  CYC(0x6247, 0x6249); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6249, 0x624a); ret_effect(gb); return; }
  CYC(0x6249, 0x624a);
  CYC(0x624a, 0x624c); alu_and(gb, 0x7f);
  CYC(0x624c, 0x624d); E = A;
  CYC(0x624d, 0x6250); A = W8(wTilesetFlags);
  CYC(0x6250, 0x6252); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(0x6252, 0x6253); ret_effect(gb); return; }
  CYC(0x6252, 0x6253);
  CALL_C(0x6253, getFreeInteractionSlot_hook, 0x3aef, 0x6256);
  if (!(F & FZ)) { CYCT(0x6256, 0x6257); ret_effect(gb); return; }
  CYC(0x6256, 0x6257);
  CYC(0x6257, 0x6259); mem_wr(gb, HL, 0x1e);
  CYC(0x6259, 0x625a); L = alu_inc8(gb, L);
  CYC(0x625a, 0x625b); mem_wr(gb, HL, E);
  CYC(0x625b, 0x625d); L = 0x4b;
  CYC(0x625d, 0x625e); mem_wr(gb, HL, C);
  CYC(0x625e, 0x625f); ret_effect(gb);
}

void replaceShutterForLinkEntering__temporarilyOpenDoor61e9_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x61e9, sp0_); }
void replaceShutterForLinkEntering__up6219_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x6219, sp0_); }
void replaceShutterForLinkEntering__down6221_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x6221, sp0_); }
void replaceShutterForLinkEntering__horizontal6227_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x6227, sp0_); }
void replaceShutterForLinkEntering__right622e_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x622e, sp0_); }
void replaceShutterForLinkEntering__left6232_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x6232, sp0_); }
void replaceShutterForLinkEntering__doReplacement6236_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x6236, sp0_); }
void replaceShutterForLinkEntering__doneReplacement6246_hook(GB *gb) { uint16_t sp0_ = gb->sp; replace_shutter_tail(gb, 0x6246, sp0_); }

void replaceShutterForLinkEntering_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61d8, 0x61db); SET_BC(wRoomLayout + 0xae);
  for (;;) {
    CYC(0x61db, 0x61dc); A = mem_rd(gb, BC);
    CYC(0x61dc, 0x61dd); push_effect(gb, BC);
    CYC(0x61dd, 0x61df); alu_sub(gb, 0x78);
    CYC(0x61df, 0x61e1); alu_cp(gb, 0x08);
    if (F & FC) CALL_C_CC(0x61e1, replaceShutterForLinkEntering__temporarilyOpenDoor61e9_hook, 0x61e9, 0x61e4);
    else CYC(0x61e1, 0x61e4);
    CYC(0x61e4, 0x61e5); SET_BC(pop_effect(gb));
    CYC(0x61e5, 0x61e6); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x61e6, 0x61e8); continue; }
    CYC(0x61e6, 0x61e8);
    CYC(0x61e8, 0x61e9); ret_effect(gb);
    return;
  }
}

void replaceOpenedChest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x626f, getThisRoomFlags_hook, 0x197d, 0x6272);
  CYC(0x6272, 0x6274); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(0x6274, 0x6275); ret_effect(gb); return; }
  CYC(0x6274, 0x6275);
  CALL_C(0x6275, getChestData_hook, 0x10cc, 0x6278);
  CYC(0x6278, 0x627a); D = wRoomLayout >> 8;
  CYC(0x627a, 0x627c); A = 0xf0;
  CYC(0x627c, 0x627d); mem_wr(gb, DE, A);
  CYC(0x627d, 0x627e); ret_effect(gb);
}

static void replace_switch_tiles(GB *gb, uint16_t entry) {
  if (entry == 0x627e) {
    CYC(0x627e, 0x6281); SET_HL(0x62ac);
    CYC(0x6281, 0x6284); A = W8(wActiveGroup);
    CYC(0x6284, 0x6286); alu_sub(gb, 0x04);
    if (F & FZ) {
      CYCT(0x6286, 0x6288);
    } else {
      CYC(0x6286, 0x6288);
      CYC(0x6288, 0x6289); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(0x6289, 0x628a); ret_effect(gb); return; }
      CYC(0x6289, 0x628a);
      CYC(0x628a, 0x628d); SET_HL(0x62dd);
    }
    CYC(0x628d, 0x6290); A = W8(wActiveRoom);
    CYC(0x6290, 0x6291); B = A;
    CYC(0x6291, 0x6294); A = W8(wSwitchState);
    CYC(0x6294, 0x6295); C = A;
    CYC(0x6295, 0x6297); D = wRoomLayout >> 8;
  } else if (entry == 0x62a7) {
    goto skip3;
  } else if (entry == 0x62a8) {
    goto skip2;
  }
next:
  CYC(0x6297, 0x6298); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6298, 0x6299); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6299, 0x629a); ret_effect(gb); return; }
  CYC(0x6299, 0x629a);
  CYC(0x629a, 0x629b); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x629b, 0x629d); goto skip3; }
  CYC(0x629b, 0x629d);
  CYC(0x629d, 0x629e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x629e, 0x629f); alu_and(gb, C);
  if (F & FZ) { CYCT(0x629f, 0x62a1); goto skip2; }
  CYC(0x629f, 0x62a1);
  CYC(0x62a1, 0x62a2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x62a2, 0x62a3); E = mem_rd(gb, HL);
  CYC(0x62a3, 0x62a4); SET_HL(HL + 1);
  CYC(0x62a4, 0x62a5); mem_wr(gb, DE, A);
  CYC(0x62a5, 0x62a7); goto next;
skip3:
  CYC(0x62a7, 0x62a8); SET_HL(HL + 1);
skip2:
  CYC(0x62a8, 0x62a9); SET_HL(HL + 1);
  CYC(0x62a9, 0x62aa); SET_HL(HL + 1);
  CYC(0x62aa, 0x62ac); goto next;
}

void replaceSwitchTiles_hook(GB *gb) { replace_switch_tiles(gb, 0x627e); }
void replaceSwitchTiles__next6297_hook(GB *gb) { replace_switch_tiles(gb, 0x6297); }
void replaceSwitchTiles__skip3Bytes62a7_hook(GB *gb) { replace_switch_tiles(gb, 0x62a7); }
void replaceSwitchTiles__skip2Bytes62a8_hook(GB *gb) { replace_switch_tiles(gb, 0x62a8); }

static void apply_single_tile_changes(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x62de) {
    CYC(0x62de, 0x62e1); A = W8(wActiveRoom);
    CYC(0x62e1, 0x62e2); B = A;
    CALL_C(0x62e2, getThisRoomFlags_hook, 0x197d, 0x62e5);
    CYC(0x62e5, 0x62e6); C = A;
    CYC(0x62e6, 0x62e8); D = wRoomLayout >> 8;
    CYC(0x62e8, 0x62eb); A = W8(wActiveGroup);
    CYC(0x62eb, 0x62ee); SET_HL(0x632e);
    CYC(0x62ee, 0x62ef); bank4_add_double_index_to_hl(gb, 0x62ef);
    CYC(0x62ef, 0x62f0); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x62f0, 0x62f1); H = mem_rd(gb, HL);
    CYC(0x62f1, 0x62f2); L = A;
  } else if (entry == 0x6307) {
    goto match;
  } else if (entry == 0x630e) {
    goto not_match;
  } else if (entry == 0x6316) {
    goto unlinked;
  } else if (entry == 0x631d) {
    goto linked;
  } else if (entry == 0x6324) {
    goto finished;
  }
next:
  CYC(0x62f2, 0x62f3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x62f3, 0x62f4); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x62f4, 0x62f6); goto not_match; }
  CYC(0x62f4, 0x62f6);
  CYC(0x62f6, 0x62f7); A = mem_rd(gb, HL);
  CYC(0x62f7, 0x62f9); alu_cp(gb, 0xf0);
  if (F & FZ) { CYCT(0x62f9, 0x62fb); goto unlinked; }
  CYC(0x62f9, 0x62fb);
  CYC(0x62fb, 0x62fd); alu_cp(gb, 0xf1);
  if (F & FZ) { CYCT(0x62fd, 0x62ff); goto linked; }
  CYC(0x62fd, 0x62ff);
  CYC(0x62ff, 0x6301); alu_cp(gb, 0xf2);
  if (F & FZ) { CYCT(0x6301, 0x6303); goto finished; }
  CYC(0x6301, 0x6303);
  CYC(0x6303, 0x6304); A = mem_rd(gb, HL);
  CYC(0x6304, 0x6305); alu_and(gb, C);
  if (F & FZ) { CYCT(0x6305, 0x6307); goto not_match; }
  CYC(0x6305, 0x6307);
match:
  CYC(0x6307, 0x6308); SET_HL(HL + 1);
  CYC(0x6308, 0x6309); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6309, 0x630a); E = A;
  CYC(0x630a, 0x630b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x630b, 0x630c); mem_wr(gb, DE, A);
  CYC(0x630c, 0x630e); goto next;
not_match:
  CYC(0x630e, 0x630f); A = mem_rd(gb, HL);
  CYC(0x630f, 0x6310); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6310, 0x6311); ret_effect(gb); return; }
  CYC(0x6310, 0x6311);
  CYC(0x6311, 0x6312); SET_HL(HL + 1);
  CYC(0x6312, 0x6313); SET_HL(HL + 1);
  CYC(0x6313, 0x6314); SET_HL(HL + 1);
  CYC(0x6314, 0x6316); goto next;
unlinked:
  CALL_C(0x6316, checkIsLinkedGame_hook, 0x1992, 0x6319);
  if (!(F & FZ)) { CYCT(0x6319, 0x631b); goto not_match; }
  CYC(0x6319, 0x631b);
  CYC(0x631b, 0x631d); goto match;
linked:
  CALL_C(0x631d, checkIsLinkedGame_hook, 0x1992, 0x6320);
  if (F & FZ) { CYCT(0x6320, 0x6322); goto not_match; }
  CYC(0x6320, 0x6322);
  CYC(0x6322, 0x6324); goto match;
finished:
  CYC(0x6324, 0x6326); A = 0x14;
  CYC(0x6326, 0x6327); push_effect(gb, HL);
  CALL_C(0x6327, checkGlobalFlag_hook, 0x31f3, 0x632a);
  CYC(0x632a, 0x632b); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(0x632b, 0x632c); ret_effect(gb); return; }
  CYC(0x632b, 0x632c);
  CYC(0x632c, 0x632e); goto match;
}

void applySingleTileChanges_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x62de, sp0_); }
void applySingleTileChanges__next62f2_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x62f2, sp0_); }
void applySingleTileChanges__match6307_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x6307, sp0_); }
void applySingleTileChanges__notMatch630e_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x630e, sp0_); }
void applySingleTileChanges__unlinkedOnly6316_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x6316, sp0_); }
void applySingleTileChanges__linkedOnly631d_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x631d, sp0_); }
void applySingleTileChanges__finishedGameOnly6324_hook(GB *gb) { uint16_t sp0_ = gb->sp; apply_single_tile_changes(gb, 0x6324, sp0_); }

static void update_changed_tile_queue(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x6c32) {
    CYC(0x6c32, 0x6c35); A = W8(wScrollMode);
    CYC(0x6c35, 0x6c37); alu_and(gb, 0x0e);
    if (!(F & FZ)) { CYCT(0x6c37, 0x6c38); ret_effect(gb); return; }
    CYC(0x6c37, 0x6c38);
    CYC(0x6c38, 0x6c3a); B = 0x04;
    for (;;) {
      CYC(0x6c3a, 0x6c3b); push_effect(gb, BC);
      CALL_C(0x6c3b, updateChangedTileQueue__handleSingleEntry6c46_hook, 0x6c46, 0x6c3e);
      CYC(0x6c3e, 0x6c3f); SET_BC(pop_effect(gb));
      CYC(0x6c3f, 0x6c40); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x6c40, 0x6c42); continue; }
      CYC(0x6c40, 0x6c42);
      break;
    }
    CYC(0x6c42, 0x6c43); alu_xor(gb, A);
    CYC(0x6c43, 0x6c45); mem_wr(gb, IO_SVBK, A);
    CYC(0x6c45, 0x6c46); ret_effect(gb);
    return;
  }

  CYC(0x6c46, 0x6c49); A = W8(wChangedTileQueueHead);
  CYC(0x6c49, 0x6c4a); B = A;
  CYC(0x6c4a, 0x6c4d); A = W8(wChangedTileQueueTail);
  CYC(0x6c4d, 0x6c4e); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x6c4e, 0x6c4f); ret_effect(gb); return; }
  CYC(0x6c4e, 0x6c4f);
  CYC(0x6c4f, 0x6c50); B = alu_inc8(gb, B);
  CYC(0x6c50, 0x6c51); A = B;
  CYC(0x6c51, 0x6c53); alu_and(gb, 0x1f);
  CYC(0x6c53, 0x6c56); W8(wChangedTileQueueHead) = A;
  CYC(0x6c56, 0x6c59); SET_HL(w2ChangedTileQueue);
  CYC(0x6c59, 0x6c5a); bank4_add_double_index_to_hl(gb, 0x6c5a);
  CYC(0x6c5a, 0x6c5c); A = 0x02;
  CYC(0x6c5c, 0x6c5e); mem_wr(gb, IO_SVBK, A);
  CYC(0x6c5e, 0x6c5f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6c5f, 0x6c60); C = mem_rd(gb, HL);
  CYC(0x6c60, 0x6c61); B = A;
  CYC(0x6c61, 0x6c62); A = C;
  CYC(0x6c62, 0x6c64); H8(hFF8C) = A;
  CYC(0x6c64, 0x6c66); A = mem_rd(gb, IO_SVBK);
  CYC(0x6c66, 0x6c67); push_effect(gb, AF);
  CYC(0x6c67, 0x6c69); A = 0x03;
  CYC(0x6c69, 0x6c6b); mem_wr(gb, IO_SVBK, A);
  CALL_C(0x6c6b, getVramSubtileAddressOfTile_hook, 0x6c89, 0x6c6e);
  CYC(0x6c6e, 0x6c6f); A = B;
  CALL_C(0x6c6f, setHlToTileMappingDataPlusATimes8_hook, 0x3a94, 0x6c72);
  CYC(0x6c72, 0x6c73); push_effect(gb, HL);
  CYC(0x6c73, 0x6c74); push_effect(gb, DE);
  CALL_C(0x6c74, write4BytesToVramLayout_hook, 0x6c23, 0x6c77);
  CYC(0x6c77, 0x6c78); SET_DE(pop_effect(gb));
  CYC(0x6c78, 0x6c7a); A = 0x04;
  CYC(0x6c7a, 0x6c7b); alu_add(gb, D);
  CYC(0x6c7b, 0x6c7c); D = A;
  CALL_C(0x6c7c, write4BytesToVramLayout_hook, 0x6c23, 0x6c7f);
  CYC(0x6c7f, 0x6c81); A = H8(hFF8C);
  CYC(0x6c81, 0x6c82); SET_HL(pop_effect(gb));
  CALL_C(0x6c82, queueTileWriteAtVBlank_hook, 0x6d24, 0x6c85);
  CYC(0x6c85, 0x6c86); SET_AF(pop_effect(gb));
  CYC(0x6c86, 0x6c88); mem_wr(gb, IO_SVBK, A);
  CYC(0x6c88, 0x6c89); ret_effect(gb);
}

void updateChangedTileQueue_hook(GB *gb) { uint16_t sp0_ = gb->sp; update_changed_tile_queue(gb, 0x6c32, sp0_); }
void updateChangedTileQueue__handleSingleEntry6c46_hook(GB *gb) { uint16_t sp0_ = gb->sp; update_changed_tile_queue(gb, 0x6c46, sp0_); }

void write4BytesToVramLayout_hook(GB *gb) {
  CYC(0x6c23, 0x6c24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6c24, 0x6c25); mem_wr(gb, DE, A);
  CYC(0x6c25, 0x6c26); E = alu_inc8(gb, E);
  CYC(0x6c26, 0x6c27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6c27, 0x6c28); mem_wr(gb, DE, A);
  CYC(0x6c28, 0x6c2a); A = 0x1f;
  CYC(0x6c2a, 0x6c2b); alu_add(gb, E);
  CYC(0x6c2b, 0x6c2c); E = A;
  CYC(0x6c2c, 0x6c2d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6c2d, 0x6c2e); mem_wr(gb, DE, A);
  CYC(0x6c2e, 0x6c2f); E = alu_inc8(gb, E);
  CYC(0x6c2f, 0x6c30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6c30, 0x6c31); mem_wr(gb, DE, A);
  CYC(0x6c31, 0x6c32); ret_effect(gb);
}

void getVramSubtileAddressOfTile_hook(GB *gb) {
  CYC(0x6c89, 0x6c8a); A = C;
  CYC(0x6c8a, 0x6c8c); A = alu_swap(gb, A);
  CYC(0x6c8c, 0x6c8e); alu_and(gb, 0x0f);
  CYC(0x6c8e, 0x6c91); SET_HL(0x6c9d);
  CYC(0x6c91, 0x6c92); bank4_add_double_index_to_hl(gb, 0x6c92);
  CYC(0x6c92, 0x6c93); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6c93, 0x6c94); H = mem_rd(gb, HL);
  CYC(0x6c94, 0x6c95); L = A;
  CYC(0x6c95, 0x6c96); A = C;
  CYC(0x6c96, 0x6c98); alu_and(gb, 0x0f);
  CYC(0x6c98, 0x6c99); alu_add(gb, A);
  CYC(0x6c99, 0x6c9a); bank4_add_a_to_hl(gb, 0x6c9a);
  CYC(0x6c9a, 0x6c9b); E = L;
  CYC(0x6c9b, 0x6c9c); D = H;
  CYC(0x6c9c, 0x6c9d); ret_effect(gb);
}

static void set_interleaved_tile_body(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x6cf3) goto copy_2_bytes;
  if (entry == 0x6cfa) goto interleave_diagonally;
  if (entry == 0x6d0f) goto copy_2_bytes_separated;
  if (entry == 0x6d18) goto queue_write;

  CYC(0x6cb3, 0x6cb5); H8(hFF8B) = A;
  CYC(0x6cb5, 0x6cb7); A = mem_rd(gb, IO_SVBK);
  CYC(0x6cb7, 0x6cb8); push_effect(gb, AF);
  CYC(0x6cb8, 0x6cba); A = 0x03;
  CYC(0x6cba, 0x6cbc); mem_wr(gb, IO_SVBK, A);
  CYC(0x6cbc, 0x6cbe); A = H8(hFF8F);
  CALL_C(0x6cbe, setHlToTileMappingDataPlusATimes8_hook, 0x3a94, 0x6cc1);
  CYC(0x6cc1, 0x6cc4); SET_DE(wEnemyPlacement_cec8);
  CYC(0x6cc4, 0x6cc6); B = 0x08;
  for (;;) {
    CYC(0x6cc6, 0x6cc7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6cc7, 0x6cc8); mem_wr(gb, DE, A);
    CYC(0x6cc8, 0x6cc9); SET_DE(DE + 1);
    CYC(0x6cc9, 0x6cca); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x6cca, 0x6ccc); continue; }
    CYC(0x6cca, 0x6ccc);
    break;
  }
  CYC(0x6ccc, 0x6cce); A = H8(hFF8E);
  CALL_C(0x6cce, setHlToTileMappingDataPlusATimes8_hook, 0x3a94, 0x6cd1);
  CYC(0x6cd1, 0x6cd4); SET_DE(wEnemyPlacement_cec8);
  CYC(0x6cd4, 0x6cd6); A = H8(hFF8B);
  CYC(0x6cd6, 0x6cd8); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6cd8, 0x6cda); goto interleave_diagonally; }
  CYC(0x6cd8, 0x6cda);
  CYC(0x6cda, 0x6cdc); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x6cdc, 0x6cde); goto copy_second_pair; }
  CYC(0x6cdc, 0x6cde);
  CYC(0x6cde, 0x6cdf); SET_HL(HL + 1);
  CYC(0x6cdf, 0x6ce0); SET_HL(HL + 1);
  CALL_C(0x6ce0, setInterleavedTile_body__copy2Bytes6cf3_hook, 0x6cf3, 0x6ce3);
  CYC(0x6ce3, 0x6ce5); goto copy_final_pair;
copy_second_pair:
  CYC(0x6ce5, 0x6ce6); SET_DE(DE + 1);
  CYC(0x6ce6, 0x6ce7); SET_DE(DE + 1);
  CALL_C(0x6ce7, setInterleavedTile_body__copy2Bytes6cf3_hook, 0x6cf3, 0x6cea);
copy_final_pair:
  CYC(0x6cea, 0x6ceb); SET_HL(HL + 1);
  CYC(0x6ceb, 0x6cec); SET_HL(HL + 1);
  CYC(0x6cec, 0x6ced); SET_DE(DE + 1);
  CYC(0x6ced, 0x6cee); SET_DE(DE + 1);
  CALL_C(0x6cee, setInterleavedTile_body__copy2Bytes6cf3_hook, 0x6cf3, 0x6cf1);
  CYC(0x6cf1, 0x6cf3); goto queue_write;

copy_2_bytes:
  CYC(0x6cf3, 0x6cf4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6cf4, 0x6cf5); mem_wr(gb, DE, A);
  CYC(0x6cf5, 0x6cf6); SET_DE(DE + 1);
  CYC(0x6cf6, 0x6cf7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6cf7, 0x6cf8); mem_wr(gb, DE, A);
  CYC(0x6cf8, 0x6cf9); SET_DE(DE + 1);
  CYC(0x6cf9, 0x6cfa); ret_effect(gb);
  return;

interleave_diagonally:
  CYC(0x6cfa, 0x6cfc); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x6cfc, 0x6cfe); goto diagonal_second_pair; }
  CYC(0x6cfc, 0x6cfe);
  CYC(0x6cfe, 0x6cff); SET_DE(DE + 1);
  CALL_C(0x6cff, setInterleavedTile_body__copy2BytesSeparated6d0f_hook, 0x6d0f, 0x6d02);
  CYC(0x6d02, 0x6d04); goto diagonal_final_pair;
diagonal_second_pair:
  CYC(0x6d04, 0x6d05); SET_HL(HL + 1);
  CALL_C(0x6d05, setInterleavedTile_body__copy2BytesSeparated6d0f_hook, 0x6d0f, 0x6d08);
diagonal_final_pair:
  CYC(0x6d08, 0x6d09); SET_HL(HL + 1);
  CYC(0x6d09, 0x6d0a); SET_DE(DE + 1);
  CALL_C(0x6d0a, setInterleavedTile_body__copy2BytesSeparated6d0f_hook, 0x6d0f, 0x6d0d);
  CYC(0x6d0d, 0x6d0f); goto queue_write;

copy_2_bytes_separated:
  CYC(0x6d0f, 0x6d10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d10, 0x6d11); mem_wr(gb, DE, A);
  CYC(0x6d11, 0x6d12); SET_DE(DE + 1);
  CYC(0x6d12, 0x6d13); SET_HL(HL + 1);
  CYC(0x6d13, 0x6d14); SET_DE(DE + 1);
  CYC(0x6d14, 0x6d15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d15, 0x6d16); mem_wr(gb, DE, A);
  CYC(0x6d16, 0x6d17); SET_DE(DE + 1);
  CYC(0x6d17, 0x6d18); ret_effect(gb);
  return;

queue_write:
  CYC(0x6d18, 0x6d1a); A = H8(hFF8C);
  CYC(0x6d1a, 0x6d1d); SET_HL(wEnemyPlacement_cec8);
  CALL_C(0x6d1d, queueTileWriteAtVBlank_hook, 0x6d24, 0x6d20);
  CYC(0x6d20, 0x6d21); SET_AF(pop_effect(gb));
  CYC(0x6d21, 0x6d23); mem_wr(gb, IO_SVBK, A);
  CYC(0x6d23, 0x6d24); ret_effect(gb);
}

void setInterleavedTile_body_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, 0x6cb3, sp0_); }
void setInterleavedTile_body__copy2Bytes6cf3_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, 0x6cf3, sp0_); }
void setInterleavedTile_body__interleaveDiagonally6cfa_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, 0x6cfa, sp0_); }
void setInterleavedTile_body__copy2BytesSeparated6d0f_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, 0x6d0f, sp0_); }
void setInterleavedTile_body__queueWrite6d18_hook(GB *gb) { uint16_t sp0_ = gb->sp; set_interleaved_tile_body(gb, 0x6d18, sp0_); }

static void queue_tile_write_at_vblank(GB *gb, uint16_t entry, uint16_t sp0_) {
  if (entry == 0x6d4d) goto copy_2_bytes;
  if (entry == 0x6d54) goto get_tile_position_in_vram;

  CYC(0x6d24, 0x6d25); push_effect(gb, HL);
  CALL_C(0x6d25, queueTileWriteAtVBlank__getTilePositionInVram6d54_hook, 0x6d54, 0x6d28);
  CYC(0x6d28, 0x6d2a); alu_add(gb, 0x20);
  CYC(0x6d2a, 0x6d2b); C = A;
  CYC(0x6d2b, 0x6d2d); A = H8(hVBlankFunctionQueueTail);
  CYC(0x6d2d, 0x6d2e); L = A;
  CYC(0x6d2e, 0x6d30); H = 0xc4;
  CYC(0x6d30, 0x6d33); A = mem_rd(gb, 0x0a89);
  CYC(0x6d33, 0x6d34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d34, 0x6d35); mem_wr(gb, HL, E);
  CYC(0x6d35, 0x6d36); L = alu_inc8(gb, L);
  CYC(0x6d36, 0x6d37); mem_wr(gb, HL, D);
  CYC(0x6d37, 0x6d38); L = alu_inc8(gb, L);
  CYC(0x6d38, 0x6d39); E = L;
  CYC(0x6d39, 0x6d3a); D = H;
  CYC(0x6d3a, 0x6d3b); SET_HL(pop_effect(gb));
  CYC(0x6d3b, 0x6d3d); B = 0x02;
  for (;;) {
    CALL_C(0x6d3d, queueTileWriteAtVBlank__copy2Bytes6d4d_hook, 0x6d4d, 0x6d40);
    CYC(0x6d40, 0x6d41); A = C;
    CYC(0x6d41, 0x6d42); mem_wr(gb, DE, A);
    CYC(0x6d42, 0x6d43); E = alu_inc8(gb, E);
    CALL_C(0x6d43, queueTileWriteAtVBlank__copy2Bytes6d4d_hook, 0x6d4d, 0x6d46);
    CYC(0x6d46, 0x6d47); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x6d47, 0x6d49); continue; }
    CYC(0x6d47, 0x6d49);
    break;
  }
  CYC(0x6d49, 0x6d4a); A = E;
  CYC(0x6d4a, 0x6d4c); H8(hVBlankFunctionQueueTail) = A;
  CYC(0x6d4c, 0x6d4d); ret_effect(gb);
  return;

copy_2_bytes:
  CYC(0x6d4d, 0x6d4e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d4e, 0x6d4f); mem_wr(gb, DE, A);
  CYC(0x6d4f, 0x6d50); E = alu_inc8(gb, E);
  CYC(0x6d50, 0x6d51); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d51, 0x6d52); mem_wr(gb, DE, A);
  CYC(0x6d52, 0x6d53); E = alu_inc8(gb, E);
  CYC(0x6d53, 0x6d54); ret_effect(gb);
  return;

get_tile_position_in_vram:
  CYC(0x6d54, 0x6d55); E = A;
  CYC(0x6d55, 0x6d57); alu_and(gb, 0xf0);
  CYC(0x6d57, 0x6d59); A = alu_swap(gb, A);
  CYC(0x6d59, 0x6d5a); D = A;
  CYC(0x6d5a, 0x6d5b); A = E;
  CYC(0x6d5b, 0x6d5d); alu_and(gb, 0x0f);
  CYC(0x6d5d, 0x6d5e); alu_add(gb, A);
  CYC(0x6d5e, 0x6d5f); E = A;
  CYC(0x6d5f, 0x6d62); A = W8(wScreenOffsetX);
  CYC(0x6d62, 0x6d64); A = alu_swap(gb, A);
  CYC(0x6d64, 0x6d65); alu_add(gb, A);
  CYC(0x6d65, 0x6d66); alu_add(gb, E);
  CYC(0x6d66, 0x6d68); alu_and(gb, 0x1f);
  CYC(0x6d68, 0x6d69); E = A;
  CYC(0x6d69, 0x6d6c); A = W8(wScreenOffsetY);
  CYC(0x6d6c, 0x6d6e); A = alu_swap(gb, A);
  CYC(0x6d6e, 0x6d6f); alu_add(gb, D);
  CYC(0x6d6f, 0x6d71); alu_and(gb, 0x0f);
  CYC(0x6d71, 0x6d74); SET_HL(0x36d6);
  CYC(0x6d74, 0x6d75); bank4_add_double_index_to_hl(gb, 0x6d75);
  CYC(0x6d75, 0x6d76); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d76, 0x6d77); alu_add(gb, E);
  CYC(0x6d77, 0x6d78); E = A;
  CYC(0x6d78, 0x6d79); D = mem_rd(gb, HL);
  CYC(0x6d79, 0x6d7a); ret_effect(gb);
}

void queueTileWriteAtVBlank_hook(GB *gb) { uint16_t sp0_ = gb->sp; queue_tile_write_at_vblank(gb, 0x6d24, sp0_); }
void queueTileWriteAtVBlank__copy2Bytes6d4d_hook(GB *gb) { uint16_t sp0_ = gb->sp; queue_tile_write_at_vblank(gb, 0x6d4d, sp0_); }
void queueTileWriteAtVBlank__getTilePositionInVram6d54_hook(GB *gb) { uint16_t sp0_ = gb->sp; queue_tile_write_at_vblank(gb, 0x6d54, sp0_); }

void generateW3VramTilesAndAttributes_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bf1, 0x6bf3); A = 0x03;
  CYC(0x6bf3, 0x6bf5); mem_wr(gb, IO_SVBK, A);
  CYC(0x6bf5, 0x6bf8); SET_HL(wRoomLayout);
  CYC(0x6bf8, 0x6bfb); SET_DE(w3VramTiles);
  CYC(0x6bfb, 0x6bfd); C = 0x0b;
  for (;;) {
    CYC(0x6bfd, 0x6bff); B = 0x10;
    for (;;) {
      CYC(0x6bff, 0x6c00); push_effect(gb, BC);
      CYC(0x6c00, 0x6c01); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x6c01, 0x6c02); push_effect(gb, HL);
      CALL_C(0x6c02, setHlToTileMappingDataPlusATimes8_hook, 0x3a94, 0x6c05);
      CYC(0x6c05, 0x6c06); push_effect(gb, DE);
      CALL_C(0x6c06, write4BytesToVramLayout_hook, 0x6c23, 0x6c09);
      CYC(0x6c09, 0x6c0a); SET_DE(pop_effect(gb));
      CYC(0x6c0a, 0x6c0c); D = (uint8_t)(D | 0x04);
      CALL_C(0x6c0c, write4BytesToVramLayout_hook, 0x6c23, 0x6c0f);
      CYC(0x6c0f, 0x6c11); D = (uint8_t)(D & ~0x04);
      CYC(0x6c11, 0x6c12); A = E;
      CYC(0x6c12, 0x6c14); alu_sub(gb, 0x1f);
      CYC(0x6c14, 0x6c15); E = A;
      CYC(0x6c15, 0x6c16); SET_HL(pop_effect(gb));
      CYC(0x6c16, 0x6c17); SET_BC(pop_effect(gb));
      CYC(0x6c17, 0x6c18); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x6c18, 0x6c1a); continue; }
      CYC(0x6c18, 0x6c1a);
      break;
    }
    CYC(0x6c1a, 0x6c1c); A = 0x20;
    CALL_C(0x6c1c, addAToDe_hook, 0x0068, 0x6c1f);
    CYC(0x6c1f, 0x6c20); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x6c20, 0x6c22); continue; }
    CYC(0x6c20, 0x6c22);
    break;
  }
  CYC(0x6c22, 0x6c23); ret_effect(gb);
}
