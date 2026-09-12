#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

void tileReplacement_group2Map7e_hook(GB *gb);
void replaceVineTiles_hook(GB *gb);

static void tileReplacement_group5Mapb9_write_tiles(GB *gb);
static void tileReplacement_group1Map27_write_tiles(GB *gb);
void createInteraction90_hook(GB *gb);
void setTileToDoor_hook(GB *gb);
void replaceTiles_hook(GB *gb);

static uint16_t room_tile_changes_jump_table(GB *gb) {
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

void applyRoomSpecificTileChanges_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x642c, 0x642f); A = W8(wActiveRoom);
  CYC(0x642f, 0x6432); SET_HL(0x64a7);
  CALL_C(0x6432, findRoomSpecificData_hook, 0x1dfe, 0x6435);
  if (!(F & FC)) {
    CYCT(0x6435, 0x6436); ret_effect(gb);
    return;
  }
  CYC(0x6435, 0x6436);
  CYC(0x6436, 0x6437); push_effect(gb, 0x6437);
  hook_handoff(gb, room_tile_changes_jump_table(gb));
}

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

void tileReplacement_group1Map58_hook(GB *gb) {
  CYC(0x6533, 0x6535); A = hram_rd(gb, 0x96);
  CYC(0x6535, 0x6536); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x6536, 0x6537); ret_effect(gb);
    return;
  }
  CYC(0x6536, 0x6537);
  CYC(0x6537, 0x653a); SET_HL(wRoomLayout + 0x35);
  CYC(0x653a, 0x653c); mem_wr(gb, HL, 0xde);
  CYC(0x653c, 0x653d); ret_effect(gb);
}

void tileReplacement_group5Mapf5_hook(GB *gb) {
  CYC(0x653d, 0x6540); A = W8(wTwinrovaTileReplacementMode);
  CYC(0x6540, 0x6541); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6541, 0x6542); ret_effect(gb);
    return;
  }
  CYC(0x6541, 0x6542);
  CYC(0x6542, 0x6543); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x6543, 0x6545);
    goto fill_with_lava;
  }
  CYC(0x6543, 0x6545);
  CYC(0x6545, 0x6546); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x6546, 0x6548);
    goto fill_with_ice;
  }
  CYC(0x6546, 0x6548);
  CYC(0x6548, 0x6549); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x6549, 0x654b);
    goto normal_layout;
  }
  CYC(0x6549, 0x654b);
  CYC(0x654b, 0x654c); alu_xor(gb, A);
  CYC(0x654c, 0x654f); W8(wTwinrovaTileReplacementMode) = A;
  CYC(0x654f, 0x6552); SET_HL(0x6555);
  CYC(0x6552, 0x6555); fillRectInRoomLayout_hook(gb);
  return;
normal_layout:
  CYC(0x6559, 0x655c); W8(wTwinrovaTileReplacementMode) = A;
  CYC(0x655c, 0x655e); A = 0xb9;
  CYC(0x655e, 0x6561); loadGfxHeader_hook(gb);
  return;
fill_with_ice:
  CYC(0x6561, 0x6564); W8(wTwinrovaTileReplacementMode) = A;
  CYC(0x6564, 0x6567); SET_HL(0x656a);
  CYC(0x6567, 0x656a); fillRectInRoomLayout_hook(gb);
  return;
fill_with_lava:
  CYC(0x656e, 0x6571); W8(wTwinrovaTileReplacementMode) = A;
  CYC(0x6571, 0x6573); A = 0xb8;
  CYC(0x6573, 0x6576); loadGfxHeader_hook(gb);
}

void tileReplacement_group4Map1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6576, getThisRoomFlags_hook, 0x197d, 0x6579);
  CYC(0x6579, 0x657b); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x657b, 0x657c); ret_effect(gb);
    return;
  }
  CYC(0x657b, 0x657c);
  CYC(0x657c, 0x657f); SET_HL(wRoomLayout + 0x1a);
  CYC(0x657f, 0x6581); A = 0x09;
  CYC(0x6581, 0x6582); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6582, 0x6583); L = alu_inc8(gb, L);
  CYC(0x6583, 0x6584); mem_wr(gb, HL, A);
  tileReplacement_group2Map7e_hook(gb);
}

void tileReplacement_group2Map7e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6584, 0x6587); A = W8(wBoughtShopItems1);
  CYC(0x6587, 0x6589); alu_and(gb, 0x0f);
  CYC(0x6589, 0x658b); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x658b, 0x658c); ret_effect(gb);
    return;
  }
  CYC(0x658b, 0x658c);
  CYC(0x658c, 0x658f); SET_HL(0x65a0);
  CALL_C(0x658f, fillRectInRoomLayout_hook, 0x6bb5, 0x6592);
  CYC(0x6592, 0x6594); A = 0xf1;
  CYC(0x6594, 0x6597); SET_HL(wRoomLayout + 0x25);
  CYC(0x6597, 0x6598); mem_wr(gb, HL, A);
  CYC(0x6598, 0x659a); L = 0x27;
  CYC(0x659a, 0x659b); mem_wr(gb, HL, A);
  CYC(0x659b, 0x659d); L = 0x32;
  CYC(0x659d, 0x659f); mem_wr(gb, HL, 0xa0);
  CYC(0x659f, 0x65a0); ret_effect(gb);
}

void tileReplacement_group4Mapc9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x65a4, getThisRoomFlags_hook, 0x197d, 0x65a7);
  CYC(0x65a7, 0x65a9); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x65a9, 0x65aa); ret_effect(gb);
    return;
  }
  CYC(0x65a9, 0x65aa);
  CYC(0x65aa, 0x65ad); SET_HL(wRoomLayout + 0x27);
  CYC(0x65ad, 0x65af); A = 0x6d;
  CYC(0x65af, 0x65b2); set4Bytes_hook(gb);
}

void tileReplacement_group4Mapc7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65b2, 0x65b5); SET_HL(wSwitchState);
  CYC(0x65b5, 0x65b7); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x65b7, 0x65b8); ret_effect(gb);
    return;
  }
  CYC(0x65b7, 0x65b8);
  CYC(0x65b8, 0x65bb); SET_HL(wRoomLayout + 0x27);
  CYC(0x65bb, 0x65bd); A = 0x6d;
  CALL_C(0x65bd, set4Bytes_hook, 0x6aac, 0x65c0);
  CYC(0x65c0, 0x65c2); A = 0xf4;
  CYC(0x65c2, 0x65c4); L = 0x3d;
  CYC(0x65c4, 0x65c5); mem_wr(gb, HL, A);
  CYC(0x65c5, 0x65c7); L = 0x4d;
  CYC(0x65c7, 0x65c8); mem_wr(gb, HL, A);
  CYC(0x65c8, 0x65ca); L = 0x5d;
  CYC(0x65ca, 0x65cb); mem_wr(gb, HL, A);
  CYC(0x65cb, 0x65cd); L = 0x6d;
  CYC(0x65cd, 0x65ce); mem_wr(gb, HL, A);
  CYC(0x65ce, 0x65cf); ret_effect(gb);
}

void tileReplacement_group4Map4c_hook(GB *gb) {
  CYC(0x65cf, 0x65d2); SET_HL(wSwitchState);
  CYC(0x65d2, 0x65d4); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x65d4, 0x65d5); ret_effect(gb);
    return;
  }
  CYC(0x65d4, 0x65d5);
  CYC(0x65d5, 0x65d8); SET_HL(wRoomLayout + 0x43);
  CYC(0x65d8, 0x65da); A = 0x6a;
  CYC(0x65da, 0x65db); mem_wr(gb, HL, A);
  CYC(0x65db, 0x65dd); L = 0x53;
  CYC(0x65dd, 0x65de); mem_wr(gb, HL, A);
  CYC(0x65de, 0x65e0); L = 0x63;
  CYC(0x65e0, 0x65e1); mem_wr(gb, HL, A);
  CYC(0x65e1, 0x65e3); L = 0x76;
  CYC(0x65e3, 0x65e5); A = 0xf4;
  CYC(0x65e5, 0x65e8); set4Bytes_hook(gb);
}

void tileReplacement_group4Map4e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65e8, 0x65eb); SET_HL(wSwitchState);
  CYC(0x65eb, 0x65ed); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x65ed, 0x65ee); ret_effect(gb);
    return;
  }
  CYC(0x65ed, 0x65ee);
  CYC(0x65ee, 0x65f1); SET_HL(wRoomLayout + 0x36);
  CYC(0x65f1, 0x65f3); A = 0x6d;
  CALL_C(0x65f3, set4Bytes_hook, 0x6aac, 0x65f6);
  CYC(0x65f6, 0x65f8); A = 0xf4;
  CYC(0x65f8, 0x65fa); L = 0x42;
  CYC(0x65fa, 0x65fb); mem_wr(gb, HL, A);
  CYC(0x65fb, 0x65fd); L = 0x52;
  CYC(0x65fd, 0x65fe); mem_wr(gb, HL, A);
  CYC(0x65fe, 0x6600); L = 0x62;
  CYC(0x6600, 0x6601); mem_wr(gb, HL, A);
  CYC(0x6601, 0x6603); L = 0x4c;
  CYC(0x6603, 0x6604); mem_wr(gb, HL, A);
  CYC(0x6604, 0x6606); L = 0x5c;
  CYC(0x6606, 0x6607); mem_wr(gb, HL, A);
  CYC(0x6607, 0x6609); L = 0x6c;
  CYC(0x6609, 0x660a); mem_wr(gb, HL, A);
  CYC(0x660a, 0x660b); ret_effect(gb);
}

void tileReplacement_group4Map59_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x660b, getThisRoomFlags_hook, 0x197d, 0x660e);
  CYC(0x660e, 0x6610); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6610, 0x6611); ret_effect(gb);
    return;
  }
  CYC(0x6610, 0x6611);
  CYC(0x6611, 0x6614); SET_DE(0x6617);
  CYC(0x6614, 0x6617); replaceTiles_hook(gb);
}

void tileReplacement_group4Map60_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x661a, 0x661c); A = 0x0f;
  CALL_C(0x661c, checkGlobalFlag_hook, 0x31f3, 0x661f);
  if (F & FZ) {
    CYCT(0x661f, 0x6620); ret_effect(gb);
    return;
  }
  CYC(0x661f, 0x6620);
  CYC(0x6620, 0x6623); SET_HL(0x6645);
  CALL_C(0x6623, fillRectInRoomLayout_hook, 0x6bb5, 0x6626);
  CALL_C(0x6626, getThisRoomFlags_hook, 0x197d, 0x6629);
  CYC(0x6629, 0x662b); alu_and(gb, 0x20);
  CYC(0x662b, 0x662d); A = 0xf0;
  if (!(F & FZ)) {
    CYCT(0x662d, 0x662f);
    goto write_tiles;
  }
  CYC(0x662d, 0x662f);
  CYC(0x662f, 0x6630); A = alu_inc8(gb, A);
write_tiles:
  CYC(0x6630, 0x6633); SET_HL(wRoomLayout + 0x57);
  CYC(0x6633, 0x6634); mem_wr(gb, HL, A);
  CYC(0x6634, 0x6636); L = 0x34;
  CYC(0x6636, 0x6638); mem_wr(gb, HL, 0x1d);
  CYC(0x6638, 0x663a); L = 0x3a;
  CYC(0x663a, 0x663c); mem_wr(gb, HL, 0x1d);
  CYC(0x663c, 0x663e); L = 0x74;
  CYC(0x663e, 0x6640); mem_wr(gb, HL, 0x1d);
  CYC(0x6640, 0x6642); L = 0x7a;
  CYC(0x6642, 0x6644); mem_wr(gb, HL, 0x1d);
  CYC(0x6644, 0x6645); ret_effect(gb);
}

void tileReplacement_group4Map52_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6649, 0x664b); A = 0x0f;
  CALL_C(0x664b, checkGlobalFlag_hook, 0x31f3, 0x664e);
  if (F & FZ) {
    CYCT(0x664e, 0x664f); ret_effect(gb);
    return;
  }
  CYC(0x664e, 0x664f);
  CYC(0x664f, 0x6651); A = 0x60;
  CYC(0x6651, 0x6654); W8(wLoadingRoom) = A;
  CYC(0x6654, 0x6657); SET_HL(0x38dc);
  CYC(0x6657, 0x6659); E = 0x00;
  CALL_C(0x6659, interBankCall_hook, 0x008a, 0x665c);
  CYC(0x665c, 0x665d); ret_effect(gb);
}

static void tileReplacement_group0And1Map38_common(GB *gb) {
  CYC(0x666b, 0x666e); SET_HL(wRoomLayout + 0x73);
  CYC(0x666e, 0x6670); A = 0xf9;
  CYC(0x6670, 0x6673); set4Bytes_hook(gb);
}

void tileReplacement_group0Map38_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x665d, 0x665f); A = 0x12;
  CALL_C(0x665f, checkGlobalFlag_hook, 0x31f3, 0x6662);
  if (F & FZ) {
    CYCT(0x6662, 0x6663); ret_effect(gb);
    return;
  }
  CYC(0x6662, 0x6663);
  CYC(0x6663, 0x6665);
  tileReplacement_group0And1Map38_common(gb);
}

void tileReplacement_group1Map38_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6665, getThisRoomFlags_hook, 0x197d, 0x6668);
  CYC(0x6668, 0x666a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x666a, 0x666b); ret_effect(gb);
    return;
  }
  CYC(0x666a, 0x666b);
  tileReplacement_group0And1Map38_common(gb);
}

void tileReplacement_group0Map48_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6673, 0x6675); A = 0x12;
  CALL_C(0x6675, checkGlobalFlag_hook, 0x31f3, 0x6678);
  if (F & FZ) {
    CYCT(0x6678, 0x6679); ret_effect(gb);
    return;
  }
  CYC(0x6678, 0x6679);
  CYC(0x6679, 0x667c); SET_HL(wRoomLayout + 0x03);
  CYC(0x667c, 0x667e); A = 0x3a;
  CYC(0x667e, 0x6681); set4Bytes_hook(gb);
}

void tileReplacement_group5Map38_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6681, getThisRoomFlags_hook, 0x197d, 0x6684);
  CYC(0x6684, 0x6686); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6686, 0x6687); ret_effect(gb);
    return;
  }
  CYC(0x6686, 0x6687);
  CYC(0x6687, 0x6689); A = 0x6a;
  CYC(0x6689, 0x668c); SET_HL(wRoomLayout + 0x39);
  CYC(0x668c, 0x668d); mem_wr(gb, HL, A);
  CYC(0x668d, 0x668f); L = 0x49;
  CYC(0x668f, 0x6690); mem_wr(gb, HL, A);
  CYC(0x6690, 0x6692); L = 0x59;
  CYC(0x6692, 0x6693); mem_wr(gb, HL, A);
  CYC(0x6693, 0x6695); L = 0x69;
  CYC(0x6695, 0x6696); mem_wr(gb, HL, A);
  CYC(0x6696, 0x6697); ret_effect(gb);
}

void tileReplacement_group5Map25And43_common_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66ba, 0x66bd); SET_HL(0x66c6);
  CALL_C(0x66bd, fillRectInRoomLayout_hook, 0x6bb5, 0x66c0);
  CYC(0x66c0, 0x66c3); SET_HL(0x66ca);
  CYC(0x66c3, 0x66c6); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map25_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6697, getThisRoomFlags_hook, 0x197d, 0x669a);
  CYC(0x669a, 0x669c); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x669c, 0x669d); ret_effect(gb);
    return;
  }
  CYC(0x669c, 0x669d);
  CYC(0x669d, 0x66a0); SET_HL(0x66a5);
  CALL_C(0x66a0, fillRectInRoomLayout_hook, 0x6bb5, 0x66a3);
  CYC(0x66a3, 0x66a5);
  tileReplacement_group5Map25And43_common_hook(gb);
}

void tileReplacement_group5Map43_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66ad, getThisRoomFlags_hook, 0x197d, 0x66b0);
  CYC(0x66b0, 0x66b2); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x66b2, 0x66b4);
    CYC(0x66ce, 0x66d1); SET_DE(0x66d4);
    CYC(0x66d1, 0x66d4); replaceTiles_hook(gb);
    return;
  }
  CYC(0x66b2, 0x66b4);
  CYC(0x66b4, 0x66b7); SET_HL(0x66a9);
  CALL_C(0x66b7, fillRectInRoomLayout_hook, 0x6bb5, 0x66ba);
  tileReplacement_group5Map25And43_common_hook(gb);
}

void tileReplacement_group5Map95_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66d7, getThisRoomFlags_hook, 0x197d, 0x66da);
  CYC(0x66da, 0x66dc); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x66dc, 0x66dd); ret_effect(gb);
    return;
  }
  CYC(0x66dc, 0x66dd);
  CYC(0x66dd, 0x66e0); SET_HL(wRoomLayout + 0x4d);
  CYC(0x66e0, 0x66e2); mem_wr(gb, HL, 0xb4);
  CYC(0x66e2, 0x66e3); L = alu_inc8(gb, L);
  CYC(0x66e3, 0x66e5); mem_wr(gb, HL, 0xb2);
  CYC(0x66e5, 0x66e8); SET_HL(0x66f1);
  CALL_C(0x66e8, fillRectInRoomLayout_hook, 0x6bb5, 0x66eb);
  CYC(0x66eb, 0x66ee); SET_HL(0x66f5);
  CYC(0x66ee, 0x66f1); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map4c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67c5, 0x67c8); A = W8(wJabuWaterLevel);
  CYC(0x67c8, 0x67ca); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(0x67ca, 0x67cb); ret_effect(gb);
    return;
  }
  CYC(0x67ca, 0x67cb);
  CYC(0x67cb, 0x67ce); SET_HL(0x67d6);
  CALL_C(0x67ce, fillRectInRoomLayout_hook, 0x6bb5, 0x67d1);
  CYC(0x67d1, 0x67d3); L = 0x57;
  CYC(0x67d3, 0x67d5); mem_wr(gb, HL, 0x45);
  CYC(0x67d5, 0x67d6); ret_effect(gb);
}

void tileReplacement_group5Map4d_hook(GB *gb) {
  CYC(0x67da, 0x67dd); A = W8(wJabuWaterLevel);
  CYC(0x67dd, 0x67df); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(0x67df, 0x67e0); ret_effect(gb);
    return;
  }
  CYC(0x67df, 0x67e0);
  CYC(0x67e0, 0x67e3); SET_HL(0x67e6);
  CYC(0x67e3, 0x67e6); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map5c_hook(GB *gb) {
  CYC(0x67ea, 0x67ed); A = W8(wDungeonFloor);
  CYC(0x67ed, 0x67ee); B = A;
  CYC(0x67ee, 0x67f1); A = W8(wJabuWaterLevel);
  CYC(0x67f1, 0x67f3); alu_and(gb, 0x07);
  CYC(0x67f3, 0x67f4); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x67f4, 0x67f5); ret_effect(gb);
    return;
  }
  CYC(0x67f4, 0x67f5);
  CYC(0x67f5, 0x67f8); SET_DE(0x67fb);
  CYC(0x67f8, 0x67fb); drawRectInRoomLayout_hook(gb);
}

void tileReplacement_group5Map5d_hook(GB *gb) {
  CYC(0x6817, 0x681a); A = W8(wDungeonFloor);
  CYC(0x681a, 0x681b); B = A;
  CYC(0x681b, 0x681e); A = W8(wJabuWaterLevel);
  CYC(0x681e, 0x6820); alu_and(gb, 0x07);
  CYC(0x6820, 0x6821); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6821, 0x6822); ret_effect(gb);
    return;
  }
  CYC(0x6821, 0x6822);
  CYC(0x6822, 0x6825); SET_DE(0x6828);
  CYC(0x6825, 0x6828); drawRectInRoomLayout_hook(gb);
}

void tileReplacement_group7Map4a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6844, getThisRoomFlags_hook, 0x197d, 0x6847);
  CYC(0x6847, 0x6849); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6849, 0x684a); ret_effect(gb);
    return;
  }
  CYC(0x6849, 0x684a);
  CYC(0x684a, 0x684d); SET_HL(0x6850);
  CYC(0x684d, 0x6850); fillRectInRoomLayout_hook(gb);
}

void tileReplacement_group0Map5c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6854, getThisRoomFlags_hook, 0x197d, 0x6857);
  CYC(0x6857, 0x6859); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6859, 0x685a); ret_effect(gb);
    return;
  }
  CYC(0x6859, 0x685a);
  CYC(0x685a, 0x685d); SET_HL(wRoomLayout + 0x34);
  CYC(0x685d, 0x685f); A = 0x3a;
  CYC(0x685f, 0x6860); mem_wr(gb, HL, A);
  CYC(0x6860, 0x6862); L = 0x43;
  CYC(0x6862, 0x6865); set3Bytes_hook(gb);
}

void tileReplacement_group0Map73_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6865, getThisRoomFlags_hook, 0x197d, 0x6868);
  CYC(0x6868, 0x686a); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x686a, 0x686b); ret_effect(gb);
    return;
  }
  CYC(0x686a, 0x686b);
  CYC(0x686b, 0x686e); SET_HL(wRoomLayout + 0x73);
  CYC(0x686e, 0x6870); mem_wr(gb, HL, 0x3a);
  CYC(0x6870, 0x6871); L = alu_inc8(gb, L);
  CYC(0x6871, 0x6873); mem_wr(gb, HL, 0x10);
  CYC(0x6873, 0x6874); L = alu_inc8(gb, L);
  CYC(0x6874, 0x6876); mem_wr(gb, HL, 0x11);
  CYC(0x6876, 0x6877); L = alu_inc8(gb, L);
  CYC(0x6877, 0x6879); mem_wr(gb, HL, 0x12);
  CYC(0x6879, 0x687a); L = alu_inc8(gb, L);
  CYC(0x687a, 0x687c); mem_wr(gb, HL, 0x3a);
  CYC(0x687c, 0x687d); ret_effect(gb);
}

void tileReplacement_group0Mapac_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x687d, getThisRoomFlags_hook, 0x197d, 0x6880);
  CYC(0x6880, 0x6882); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x6882, 0x6883); ret_effect(gb);
    return;
  }
  CYC(0x6882, 0x6883);
  CYC(0x6883, 0x6886); SET_HL(wRoomLayout + 0x33);
  CYC(0x6886, 0x6888); A = 0xaf;
  CYC(0x6888, 0x6889); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6889, 0x688a); mem_wr(gb, HL, A);
  CYC(0x688a, 0x688c); L = 0x43;
  CYC(0x688c, 0x688d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x688d, 0x688e); mem_wr(gb, HL, A);
  CYC(0x688e, 0x688f); ret_effect(gb);
}

void tileReplacement_group0Map2c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x688f, 0x6892); SET_BC(0x0017);
  CALL_C(0x6892, getVinePosition_hook, 0x69d5, 0x6895);
  if (!(F & FZ)) {
    CYCT(0x6895, 0x6898);
    setTileToWitheredVine_hook(gb);
    return;
  }
  CYC(0x6895, 0x6898);
  CYC(0x6898, 0x689a); L = 0x06;
  CYC(0x689a, 0x689d); SET_DE(0x68a0);
  CYC(0x689d, 0x68a0); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Map1c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68a3, 0x68a6); SET_BC(0x0017);
  CALL_C(0x68a6, getVinePosition_hook, 0x69d5, 0x68a9);
  if (!(F & FZ)) {
    CYCT(0x68a9, 0x68aa); ret_effect(gb);
    return;
  }
  CYC(0x68a9, 0x68aa);
  CYC(0x68aa, 0x68ac); L = 0x66;
  CYC(0x68ac, 0x68af); SET_DE(0x68b2);
  CYC(0x68af, 0x68b2); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapba_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68b7, 0x68ba); SET_BC(0x0218);
  CALL_C(0x68ba, getVinePosition_hook, 0x69d5, 0x68bd);
  if (!(F & FZ)) {
    CYCT(0x68bd, 0x68c0);
    setTileToWitheredVine_hook(gb);
    return;
  }
  CYC(0x68bd, 0x68c0);
  CYC(0x68c0, 0x68c2); L = 0x07;
  CYC(0x68c2, 0x68c5); SET_DE(0x68ce);
  CALL_C(0x68c5, replaceVineTiles_hook, 0x699b, 0x68c8);
  CYC(0x68c8, 0x68ca); A = 0x8b;
  CYC(0x68ca, 0x68cd); mem_wr(gb, wRoomLayout + 0x18, A);
  CYC(0x68cd, 0x68d1); ret_effect(gb);
}

void tileReplacement_group0Mapaa_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68d1, 0x68d4); SET_BC(0x0218);
  CALL_C(0x68d4, getVinePosition_hook, 0x69d5, 0x68d7);
  if (!(F & FZ)) {
    CYCT(0x68d7, 0x68d8); ret_effect(gb);
    return;
  }
  CYC(0x68d7, 0x68d8);
  CYC(0x68d8, 0x68da); L = 0x77;
  CYC(0x68da, 0x68dd); SET_DE(0x68e0);
  CYC(0x68dd, 0x68e0); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapcc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68e3, 0x68e6); SET_BC(0x0311);
  CALL_C(0x68e6, getVinePosition_hook, 0x69d5, 0x68e9);
  if (!(F & FZ)) {
    CYCT(0x68e9, 0x68ec);
    setTileToWitheredVine_hook(gb);
    return;
  }
  CYC(0x68e9, 0x68ec);
  CYC(0x68ec, 0x68ee); L = 0x00;
  CYC(0x68ee, 0x68f1); SET_DE(0x68f4);
  CYC(0x68f1, 0x68f4); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapbc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68f7, 0x68fa); SET_BC(0x0311);
  CALL_C(0x68fa, getVinePosition_hook, 0x69d5, 0x68fd);
  if (!(F & FZ)) {
    CYCT(0x68fd, 0x68fe); ret_effect(gb);
    return;
  }
  CYC(0x68fd, 0x68fe);
  CYC(0x68fe, 0x6900); L = 0x70;
  CYC(0x6900, 0x6903); SET_DE(0x6906);
  CYC(0x6903, 0x6906); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapda_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6909, 0x690c); SET_BC(0x0418);
  CALL_C(0x690c, getVinePosition_hook, 0x69d5, 0x690f);
  if (!(F & FZ)) {
    CYCT(0x690f, 0x6912);
    setTileToWitheredVine_hook(gb);
    return;
  }
  CYC(0x690f, 0x6912);
  CYC(0x6912, 0x6914); L = 0x07;
  CYC(0x6914, 0x6917); SET_DE(0x691a);
  CYC(0x6917, 0x691a); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Mapca_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x691d, 0x6920); SET_BC(0x0418);
  CALL_C(0x6920, getVinePosition_hook, 0x69d5, 0x6923);
  if (!(F & FZ)) {
    CYCT(0x6923, 0x6924); ret_effect(gb);
    return;
  }
  CYC(0x6923, 0x6924);
  CYC(0x6924, 0x6926); L = 0x77;
  CYC(0x6926, 0x6929); SET_DE(0x692c);
  CYC(0x6929, 0x692c); replaceVineTiles_hook(gb);
}

void tileReplacement_group0Map61_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x692f, 0x6932); SET_BC(0x0122);
  CALL_C(0x6932, getVinePosition_hook, 0x69d5, 0x6935);
  if (F & FZ) {
    CYCT(0x6935, 0x6937);
    goto vine1;
  }
  CYC(0x6935, 0x6937);
  CYC(0x6937, 0x693a); SET_BC(0x0127);
  CALL_C(0x693a, getVinePosition_hook, 0x69d5, 0x693d);
  if (!(F & FZ)) {
    CYCT(0x693d, 0x6940);
    setTileToWitheredVine_hook(gb);
    return;
  }
  CYC(0x693d, 0x6940);
  CYC(0x6940, 0x6943); SET_HL(wRoomLayout + 0x06);
  CYC(0x6943, 0x6945); mem_wr(gb, HL, 0x4d);
  CYC(0x6945, 0x6946); L = alu_inc8(gb, L);
  CYC(0x6946, 0x6948); mem_wr(gb, HL, 0xd5);
  CYC(0x6948, 0x6949); L = alu_inc8(gb, L);
  CYC(0x6949, 0x694b); mem_wr(gb, HL, 0x55);
  CYC(0x694b, 0x694d); L = 0x16;
  CYC(0x694d, 0x694f); mem_wr(gb, HL, 0x5d);
  CYC(0x694f, 0x6950); L = alu_inc8(gb, L);
  CYC(0x6950, 0x6952); mem_wr(gb, HL, 0xd6);
  CYC(0x6952, 0x6953); L = alu_inc8(gb, L);
  CYC(0x6953, 0x6955); mem_wr(gb, HL, 0x60);
  CYC(0x6955, 0x6957); L = 0x27;
  CYC(0x6957, 0x6959); mem_wr(gb, HL, 0x8d);
  CYC(0x6959, 0x695a); ret_effect(gb);
  return;
vine1:
  CYC(0x695a, 0x695d); SET_HL(wRoomLayout + 0x01);
  CYC(0x695d, 0x695f); mem_wr(gb, HL, 0x56);
  CYC(0x695f, 0x6960); L = alu_inc8(gb, L);
  CYC(0x6960, 0x6962); mem_wr(gb, HL, 0xd5);
  CYC(0x6962, 0x6963); L = alu_inc8(gb, L);
  CYC(0x6963, 0x6965); mem_wr(gb, HL, 0x4d);
  CYC(0x6965, 0x6967); L = 0x11;
  CYC(0x6967, 0x6969); mem_wr(gb, HL, 0x61);
  CYC(0x6969, 0x696a); L = alu_inc8(gb, L);
  CYC(0x696a, 0x696c); mem_wr(gb, HL, 0xd6);
  CYC(0x696c, 0x696d); L = alu_inc8(gb, L);
  CYC(0x696d, 0x696f); mem_wr(gb, HL, 0x5d);
  CYC(0x696f, 0x6971); L = 0x22;
  CYC(0x6971, 0x6973); mem_wr(gb, HL, 0x8d);
  CYC(0x6973, 0x6974); ret_effect(gb);
}

void tileReplacement_group0Map51_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6974, 0x6977); SET_BC(0x0122);
  CALL_C(0x6977, getVinePosition_hook, 0x69d5, 0x697a);
  if (F & FZ) {
    CYCT(0x697a, 0x697c);
    goto vines1;
  }
  CYC(0x697a, 0x697c);
  CYC(0x697c, 0x697f); SET_BC(0x0127);
  CALL_C(0x697f, getVinePosition_hook, 0x69d5, 0x6982);
  if (!(F & FZ)) {
    CYCT(0x6982, 0x6983); ret_effect(gb);
    return;
  }
  CYC(0x6982, 0x6983);
  CYC(0x6983, 0x6986); SET_HL(wRoomLayout + 0x76);
  CYC(0x6986, 0x6988); mem_wr(gb, HL, 0x5b);
  CYC(0x6988, 0x6989); L = alu_inc8(gb, L);
  CYC(0x6989, 0x698b); mem_wr(gb, HL, 0xd4);
  CYC(0x698b, 0x698c); L = alu_inc8(gb, L);
  CYC(0x698c, 0x698e); mem_wr(gb, HL, 0x45);
  CYC(0x698e, 0x698f); ret_effect(gb);
  return;
vines1:
  CYC(0x698f, 0x6992); SET_HL(wRoomLayout + 0x71);
  CYC(0x6992, 0x6994); mem_wr(gb, HL, 0x46);
  CYC(0x6994, 0x6995); L = alu_inc8(gb, L);
  CYC(0x6995, 0x6997); mem_wr(gb, HL, 0xd4);
  CYC(0x6997, 0x6998); L = alu_inc8(gb, L);
  CYC(0x6998, 0x699a); mem_wr(gb, HL, 0x5c);
  CYC(0x699a, 0x699b); ret_effect(gb);
}

void replaceVineTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x699b, 0x699d); H = wRoomLayout >> 8;
  for (;;) {
    CYC(0x699d, 0x699e); A = mem_rd(gb, DE);
    CYC(0x699e, 0x699f); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x699f, 0x69a1);
      break;
    }
    CYC(0x699f, 0x69a1);
    CYC(0x69a1, 0x69a2); A = mem_rd(gb, DE);
    CYC(0x69a2, 0x69a3); SET_DE(DE + 1);
    CYC(0x69a3, 0x69a4); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x69a4, 0x69a5); L = alu_inc8(gb, L);
    CYC(0x69a5, 0x69a6); A = mem_rd(gb, DE);
    CYC(0x69a6, 0x69a7); SET_DE(DE + 1);
    CYC(0x69a7, 0x69a8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x69a8, 0x69aa); A = 0x0d;
    CYC(0x69aa, 0x69ab); add_a_to_hl_from_rst(gb, 0x69ab);
    CYC(0x69ab, 0x69ad);
  }
  CYC(0x69ad, 0x69b0); SET_DE(0x69c0);
  CALL_C(0x69b0, replaceTiles_hook, 0x6096, 0x69b3);
  CYC(0x69b3, 0x69b5); A = 0xd6;
  CALL_C(0x69b5, findTileInRoom_hook, 0x15cc, 0x69b8);
  if (!(F & FZ)) {
    CYCT(0x69b8, 0x69b9); ret_effect(gb);
    return;
  }
  CYC(0x69b8, 0x69b9);
  CYC(0x69b9, 0x69ba); A = L;
  CYC(0x69ba, 0x69bc); alu_add(gb, 0x10);
  CYC(0x69bc, 0x69bd); L = A;
  CYC(0x69bd, 0x69bf); mem_wr(gb, HL, 0x8d);
  CYC(0x69bf, 0x69c0); ret_effect(gb);
}

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

void getVinePosition_hook(GB *gb) {
  CYC(0x69d5, 0x69d6); A = B;
  CYC(0x69d6, 0x69d9); SET_HL(0xc8f0);
  CYC(0x69d9, 0x69da); add_a_to_hl_from_rst(gb, 0x69da);
  CYC(0x69da, 0x69db); A = mem_rd(gb, HL);
  CYC(0x69db, 0x69dc); alu_cp(gb, C);
  CYC(0x69dc, 0x69dd); ret_effect(gb);
}

void set3Bytes_hook(GB *gb);

void initializeVinePositions_hook(GB *gb) {
  CYC(0x69dd, 0x69e0); SET_HL(wGroup1RoomFlags + 0xf0);
  CYC(0x69e0, 0x69e3); SET_DE(0x69e8);
  CYC(0x69e3, 0x69e5); B = 0x06;
  CYC(0x69e5, 0x69e8); copyMemoryReverse_hook(gb);
}

void tileReplacement_group0Map54_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69ee, 0x69ef); alu_xor(gb, A);
  CYC(0x69ef, 0x69f2); W8(wSwitchState) = A;
  CALL_C(0x69f2, getThisRoomFlags_hook, 0x197d, 0x69f5);
  CYC(0x69f5, 0x69f7); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x69f7, 0x69f8); ret_effect(gb);
    return;
  }
  CYC(0x69f7, 0x69f8);
  CYC(0x69f8, 0x69fa); A = 0x1d;
  CYC(0x69fa, 0x69fd); SET_HL(wRoomLayout + 0x43);
  CYC(0x69fd, 0x69fe); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x69fe, 0x69ff); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x69ff, 0x6a00); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a00, 0x6a02); A = 0x1e;
  CYC(0x6a02, 0x6a04); L = 0x53;
  CYC(0x6a04, 0x6a05); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a05, 0x6a06); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a06, 0x6a07); mem_wr(gb, HL, A);
  CYC(0x6a07, 0x6a09); A = 0x9e;
  CYC(0x6a09, 0x6a0c); mem_wr(gb, wRoomLayout + 0x68, A);
  CYC(0x6a0c, 0x6a0d); ret_effect(gb);
}

void tileReplacement_group0Map25_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a0d, 0x6a0f); A = 0x25;
  CALL_C(0x6a0f, checkGlobalFlag_hook, 0x31f3, 0x6a12);
  if (F & FZ) {
    CYCT(0x6a12, 0x6a13); ret_effect(gb);
    return;
  }
  CYC(0x6a12, 0x6a13);
  CYC(0x6a13, 0x6a15); A = 0x1d;
  CYC(0x6a15, 0x6a18); SET_HL(wRoomLayout + 0x50);
  CYC(0x6a18, 0x6a19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a19, 0x6a1a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a1a, 0x6a1b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a1b, 0x6a1d); A = 0x1e;
  CYC(0x6a1d, 0x6a20); SET_HL(wRoomLayout + 0x60);
  CYC(0x6a20, 0x6a21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a21, 0x6a22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a22, 0x6a23); mem_wr(gb, HL, A);
  CYC(0x6a23, 0x6a24); ret_effect(gb);
}

void tileReplacement_group0Map3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a24, 0x6a26); A = 0x0a;
  CALL_C(0x6a26, checkGlobalFlag_hook, 0x31f3, 0x6a29);
  if (F & FZ) {
    CYCT(0x6a29, 0x6a2a); ret_effect(gb);
    return;
  }
  CYC(0x6a29, 0x6a2a);
  CYC(0x6a2a, 0x6a2c); A = 0xee;
  CYC(0x6a2c, 0x6a2f); mem_wr(gb, wRoomLayout + 0x23, A);
  CYC(0x6a2f, 0x6a30); ret_effect(gb);
}

void tileReplacement_group0Map0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a30, 0x6a33); SET_HL(wGroup0RoomFlags + 0x0a);
  CYC(0x6a33, 0x6a35); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6a35, 0x6a36); ret_effect(gb);
    return;
  }
  CYC(0x6a35, 0x6a36);
  CYC(0x6a36, 0x6a39); SET_HL(wRoomLayout + 0x43);
  CYC(0x6a39, 0x6a3b); mem_wr(gb, HL, 0xdd);
  CYC(0x6a3b, 0x6a3c); ret_effect(gb);
}

void tileReplacement_group5Mapb9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a3c, 0x6a3e); A = 0x03;
  CYC(0x6a3e, 0x6a41); SET_HL(wEssencesObtained);
  CALL_C(0x6a41, checkFlag_hook, 0x0205, 0x6a44);
  if (F & FZ) {
    CYCT(0x6a44, 0x6a45); ret_effect(gb);
    return;
  }
  CYC(0x6a44, 0x6a45);
  CYC(0x6a45, 0x6a48); SET_BC(0x6a5d);
  CYC(0x6a48, 0x6a4b); SET_HL(wRoomLayout + 0x41);
  CYC(0x6a4b, 0x6a4e); push_effect(gb, 0x6a4e);
  tileReplacement_group5Mapb9_write_tiles(gb);
  CYC(0x6a4e, 0x6a50); L = 0x51;
  tileReplacement_group5Mapb9_write_tiles(gb);
}

static void tileReplacement_group5Mapb9_write_tiles(GB *gb) {
  CYC(0x6a50, 0x6a52); A = 0x05;
  for (;;) {
    CYC(0x6a52, 0x6a54); hram_wr(gb, 0x8d, A);
    CYC(0x6a54, 0x6a55); A = mem_rd(gb, BC);
    CYC(0x6a55, 0x6a56); SET_BC(BC + 1);
    CYC(0x6a56, 0x6a57); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x6a57, 0x6a59); A = hram_rd(gb, 0x8d);
    CYC(0x6a59, 0x6a5a); A = alu_dec8(gb, A);
    if (!(F & FZ)) {
      CYCT(0x6a5a, 0x6a5c);
      continue;
    }
    CYC(0x6a5a, 0x6a5c);
    break;
  }
  CYC(0x6a5c, 0x6a5d); ret_effect(gb);
}

void tileReplacement_group1Map27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6a67, getThisRoomFlags_hook, 0x197d, 0x6a6a);
  CYC(0x6a6a, 0x6a6c); L = 0x15;
  CYC(0x6a6c, 0x6a6e); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6a6e, 0x6a70);
    goto next_flag_17;
  }
  CYC(0x6a6e, 0x6a70);
  CYC(0x6a70, 0x6a73); SET_DE(0x3343);
  CYC(0x6a73, 0x6a76); push_effect(gb, 0x6a76);
  tileReplacement_group1Map27_write_tiles(gb);
next_flag_17:
  CYC(0x6a76, 0x6a78); L = 0x17;
  CYC(0x6a78, 0x6a7a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6a7a, 0x6a7c);
    goto next_flag_35;
  }
  CYC(0x6a7a, 0x6a7c);
  CYC(0x6a7c, 0x6a7f); SET_DE(0x3424);
  CYC(0x6a7f, 0x6a82); push_effect(gb, 0x6a82);
  tileReplacement_group1Map27_write_tiles(gb);
next_flag_35:
  CYC(0x6a82, 0x6a84); L = 0x35;
  CYC(0x6a84, 0x6a86); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6a86, 0x6a88);
    goto next_flag_37;
  }
  CYC(0x6a86, 0x6a88);
  CYC(0x6a88, 0x6a8b); SET_DE(0x3545);
  CYC(0x6a8b, 0x6a8e); push_effect(gb, 0x6a8e);
  tileReplacement_group1Map27_write_tiles(gb);
next_flag_37:
  CYC(0x6a8e, 0x6a90); L = 0x37;
  CYC(0x6a90, 0x6a92); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6a92, 0x6a93); ret_effect(gb);
    return;
  }
  CYC(0x6a92, 0x6a93);
  CYC(0x6a93, 0x6a96); SET_DE(0x3626);
  tileReplacement_group1Map27_write_tiles(gb);
}

static void tileReplacement_group1Map27_write_tiles(GB *gb) {
  CYC(0x6a96, 0x6a98); B = wRoomLayout >> 8;
  CYC(0x6a98, 0x6a99); C = D;
  CYC(0x6a99, 0x6a9b); A = 0x3a;
  CYC(0x6a9b, 0x6a9c); mem_wr(gb, BC, A);
  CYC(0x6a9c, 0x6a9d); C = E;
  CYC(0x6a9d, 0x6a9f); A = 0x02;
  CYC(0x6a9f, 0x6aa0); mem_wr(gb, BC, A);
  CYC(0x6aa0, 0x6aa1); ret_effect(gb);
}

void tileReplacement_group5Mapc2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6aa1, getThisRoomFlags_hook, 0x197d, 0x6aa4);
  CYC(0x6aa4, 0x6aa6); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6aa6, 0x6aa7); ret_effect(gb);
    return;
  }
  CYC(0x6aa6, 0x6aa7);
  CYC(0x6aa7, 0x6aaa); SET_HL(wRoomLayout + 0x56);
  CYC(0x6aaa, 0x6aac); A = 0x6d;
  set4Bytes_hook(gb);
}

void set4Bytes_hook(GB *gb) {
  CYC(0x6aac, 0x6aad); mem_wr(gb, HL, A); SET_HL(HL + 1);
  set3Bytes_hook(gb);
}

void set3Bytes_hook(GB *gb) {
  CYC(0x6aad, 0x6aae); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6aae, 0x6aaf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6aaf, 0x6ab0); mem_wr(gb, HL, A);
  CYC(0x6ab0, 0x6ab1); ret_effect(gb);
}

void tileReplacement_group5Mape3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6ab1, getThisRoomFlags_hook, 0x197d, 0x6ab4);
  CYC(0x6ab4, 0x6ab6); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6ab6, 0x6ab7); ret_effect(gb);
    return;
  }
  CYC(0x6ab6, 0x6ab7);
  CYC(0x6ab7, 0x6aba); SET_HL(wRoomLayout + 0x26);
  CYC(0x6aba, 0x6abc); A = 0x6d;
  CYC(0x6abc, 0x6abe); set3Bytes_hook(gb);
}

void tileReplacement_group2Map90_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6abe, getThisRoomFlags_hook, 0x197d, 0x6ac1);
  CYC(0x6ac1, 0x6ac3); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(0x6ac3, 0x6ac4); ret_effect(gb);
    return;
  }
  CYC(0x6ac3, 0x6ac4);
  CYC(0x6ac4, 0x6ac7); SET_DE(0x6aca);
  CYC(0x6ac7, 0x6aca); drawRectInRoomLayout_hook(gb);
}

void tileReplacement_group1Map8c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6ad9, getThisRoomFlags_hook, 0x197d, 0x6adc);
  CYC(0x6adc, 0x6ade); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(0x6ade, 0x6adf); ret_effect(gb);
    return;
  }
  CYC(0x6ade, 0x6adf);
  CYC(0x6adf, 0x6ae2); SET_HL(wRoomLayout + 0x04);
  CYC(0x6ae2, 0x6ae4); mem_wr(gb, HL, 0x30);
  CYC(0x6ae4, 0x6ae5); L = alu_inc8(gb, L);
  CYC(0x6ae5, 0x6ae7); mem_wr(gb, HL, 0x32);
  CYC(0x6ae7, 0x6ae9); A = 0x3a;
  CYC(0x6ae9, 0x6aeb); L = 0x14;
  CYC(0x6aeb, 0x6aec); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6aec, 0x6aed); mem_wr(gb, HL, A);
  CYC(0x6aed, 0x6aef); L = 0x34;
  CYC(0x6aef, 0x6af1); mem_wr(gb, HL, 0x02);
  CYC(0x6af1, 0x6af2); L = alu_inc8(gb, L);
  CYC(0x6af2, 0x6af4); mem_wr(gb, HL, 0x3a);
  CYC(0x6af4, 0x6af5); ret_effect(gb);
}

void tileReplacement_group2Map9e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6af5, 0x6af6); alu_xor(gb, A);
  CYC(0x6af6, 0x6af9); W8(wToggleBlocksState) = A;
  CALL_C(0x6af9, getThisRoomFlags_hook, 0x197d, 0x6afc);
  CYC(0x6afc, 0x6afe); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6afe, 0x6aff); ret_effect(gb);
    return;
  }
  CYC(0x6afe, 0x6aff);
  CYC(0x6aff, 0x6b02); SET_HL(wRoomLayout + 0x13);
  CYC(0x6b02, 0x6b04); A = 0x6d;
  CALL_C(0x6b04, set3Bytes_hook, 0x6aad, 0x6b07);
  CYC(0x6b07, 0x6b08); L = alu_inc8(gb, L);
  CYC(0x6b08, 0x6b0b); set3Bytes_hook(gb);
}

void tileReplacement_group0Mape0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b0b, 0x6b0e); A = W8(wEssencesObtained);
  CYC(0x6b0e, 0x6b10); alu_bit(gb, 4, A);
  CYC(0x6b10, 0x6b12); L = 0x46;
  if (!(F & FZ)) {
    CALL_C_CC(0x6b12, setTileToDoor_hook, 0x6b41, 0x6b15);
  } else {
    CYC(0x6b12, 0x6b15);
  }
  CYC(0x6b15, 0x6b17); C = 0x1b;
  createInteraction90_hook(gb);
}

void createInteraction90_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b17, getFreeInteractionSlot_hook, 0x3aef, 0x6b1a);
  if (!(F & FZ)) {
    CYCT(0x6b1a, 0x6b1b); ret_effect(gb);
    return;
  }
  CYC(0x6b1a, 0x6b1b);
  CYC(0x6b1b, 0x6b1d); mem_wr(gb, HL, 0x90);
  CYC(0x6b1d, 0x6b1e); L = alu_inc8(gb, L);
  CYC(0x6b1e, 0x6b1f); mem_wr(gb, HL, C);
  CYC(0x6b1f, 0x6b20); ret_effect(gb);
}

void tileReplacement_group0Mape1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b20, 0x6b22); C = 0x1c;
  CALL_C(0x6b22, createInteraction90_hook, 0x6b17, 0x6b25);
  CYC(0x6b25, 0x6b28); A = W8(wEssencesObtained);
  CYC(0x6b28, 0x6b29); alu_rrca(gb);
  CYC(0x6b29, 0x6b2b); L = 0x26;
  if (F & FC) {
    CALL_C_CC(0x6b2b, setTileToDoor_hook, 0x6b41, 0x6b2e);
  } else {
    CYC(0x6b2b, 0x6b2e);
  }
  CYC(0x6b2e, 0x6b2f); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x6b2f, 0x6b30); ret_effect(gb);
    return;
  }
  CYC(0x6b2f, 0x6b30);
  CYC(0x6b30, 0x6b32); L = 0x53;
  CYC(0x6b32, 0x6b34); setTileToDoor_hook(gb);
}

void tileReplacement_group0Mape2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b34, 0x6b36); C = 0x1d;
  CALL_C(0x6b36, createInteraction90_hook, 0x6b17, 0x6b39);
  CYC(0x6b39, 0x6b3c); A = W8(wEssencesObtained);
  CYC(0x6b3c, 0x6b3e); alu_bit(gb, 2, A);
  if (F & FZ) {
    CYCT(0x6b3e, 0x6b3f); ret_effect(gb);
    return;
  }
  CYC(0x6b3e, 0x6b3f);
  CYC(0x6b3f, 0x6b41); L = 0x54;
  setTileToDoor_hook(gb);
}

void setTileToDoor_hook(GB *gb) {
  CYC(0x6b41, 0x6b43); H = wRoomLayout >> 8;
  CYC(0x6b43, 0x6b45); mem_wr(gb, HL, 0xdd);
  CYC(0x6b45, 0x6b46); ret_effect(gb);
}

void tileReplacement_group4Mapea_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b46, getThisRoomFlags_hook, 0x197d, 0x6b49);
  CYC(0x6b49, 0x6b4b); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6b4b, 0x6b4c); ret_effect(gb);
    return;
  }
  CYC(0x6b4b, 0x6b4c);
  CYC(0x6b4c, 0x6b4e); A = 0xa3;
  CYC(0x6b4e, 0x6b51); SET_HL(wRoomLayout + 0x33);
  CALL_C(0x6b51, set3Bytes_hook, 0x6aad, 0x6b54);
  CYC(0x6b54, 0x6b56); L = 0x39;
  CALL_C(0x6b56, set3Bytes_hook, 0x6aad, 0x6b59);
  CYC(0x6b59, 0x6b5b); A = 0xb7;
  CYC(0x6b5b, 0x6b5d); L = 0x43;
  CALL_C(0x6b5d, set3Bytes_hook, 0x6aad, 0x6b60);
  CYC(0x6b60, 0x6b62); L = 0x49;
  CALL_C(0x6b62, set3Bytes_hook, 0x6aad, 0x6b65);
  CYC(0x6b65, 0x6b67); A = 0x88;
  CYC(0x6b67, 0x6b69); L = 0x53;
  CALL_C(0x6b69, set3Bytes_hook, 0x6aad, 0x6b6c);
  CYC(0x6b6c, 0x6b6e); L = 0x59;
  CYC(0x6b6e, 0x6b71); set3Bytes_hook(gb);
}

void tileReplacement_group0Map98_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b71, 0x6b74); A = W8(wRickyState);
  CYC(0x6b74, 0x6b76); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x6b76, 0x6b78);
    goto remove_dirt;
  }
  CYC(0x6b76, 0x6b78);
  CYC(0x6b78, 0x6b7a); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x6b7a, 0x6b7c);
    goto remove_dirt;
  }
  CYC(0x6b7a, 0x6b7c);
  CYC(0x6b7c, 0x6b7e); A = 0x48;
  CALL_C(0x6b7e, checkTreasureObtained_hook, 0x1748, 0x6b81);
  if (!(F & FC)) {
    CYCT(0x6b81, 0x6b82); ret_effect(gb);
    return;
  }
  CYC(0x6b81, 0x6b82);
remove_dirt:
  CYC(0x6b82, 0x6b84); A = 0x3a;
  CYC(0x6b84, 0x6b87); mem_wr(gb, wRoomLayout + 0x24, A);
  CYC(0x6b87, 0x6b88); ret_effect(gb);
}

void tileReplacement_group0Map76_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b88, checkIsLinkedGame_hook, 0x1992, 0x6b8b);
  if (F & FZ) {
    CYCT(0x6b8b, 0x6b8c); ret_effect(gb);
    return;
  }
  CYC(0x6b8b, 0x6b8c);
  CALL_C(0x6b8c, getBlackTowerProgress_hook, 0x36c0, 0x6b8f);
  CYC(0x6b8f, 0x6b90); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6b90, 0x6b91); ret_effect(gb);
    return;
  }
  CYC(0x6b90, 0x6b91);
  CYC(0x6b91, 0x6b94); SET_HL(wRoomLayout + 0x54);
  CYC(0x6b94, 0x6b96); A = 0xa7;
  CYC(0x6b96, 0x6b97); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6b97, 0x6b98); mem_wr(gb, HL, A);
  CYC(0x6b98, 0x6b99); ret_effect(gb);
}

void tileReplacement_group0Mapa5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b99, 0x6b9c); A = mem_rd(gb, wGroup1RoomFlags + 0xa5);
  CYC(0x6b9c, 0x6b9e); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x6b9e, 0x6b9f); ret_effect(gb);
    return;
  }
  CYC(0x6b9e, 0x6b9f);
  CYC(0x6b9f, 0x6ba2); SET_HL(wRoomLayout + 0x22);
  CYC(0x6ba2, 0x6ba4); mem_wr(gb, HL, 0xee);
  CYC(0x6ba4, 0x6ba5); L = alu_inc8(gb, L);
  CYC(0x6ba5, 0x6ba7); mem_wr(gb, HL, 0xef);
  CYC(0x6ba7, 0x6ba8); ret_effect(gb);
}

void func_04_6ba8_hook(GB *gb) {
  CYC(0x6ba8, 0x6baa); D = wRoomLayout >> 8;
  CYC(0x6baa, 0x6bab); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6bab, 0x6bac); C = A;
  for (;;) {
    CYC(0x6bac, 0x6bad); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x6bad, 0x6baf); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(0x6baf, 0x6bb0); ret_effect(gb);
      return;
    }
    CYC(0x6baf, 0x6bb0);
    CYC(0x6bb0, 0x6bb1); E = A;
    CYC(0x6bb1, 0x6bb2); A = C;
    CYC(0x6bb2, 0x6bb3); mem_wr(gb, DE, A);
    CYC(0x6bb3, 0x6bb5);
  }
}

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
