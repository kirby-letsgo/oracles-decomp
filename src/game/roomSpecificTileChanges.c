#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

void tileReplacement_group2Map7e_hook(GB *gb);

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
  CYC(0x6614, 0x6617); replaceTiles(gb);
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
    CYC(0x66d1, 0x66d4); replaceTiles(gb);
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
    CYCT(0x6bb3, 0x6bb5);
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
