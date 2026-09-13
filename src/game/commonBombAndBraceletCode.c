#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void braceletCheckBreakable_hook(GB *gb);

static void common_bomb_add_a_to_hl(GB *gb, uint16_t return_address) {
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
  ret_effect(gb);
}

static void common_bomb_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void itemBeginThrow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63b1, itemSetVar3cToFF_hook, 0x49c2, 0x63b4);
  CYC(0x63b4, 0x63b7); A = W8(w1Link_direction);
  CYC(0x63b7, 0x63ba); SET_HL(0x6402);
  CYC(0x63ba, 0x63bb); common_bomb_add_a_to_hl(gb, 0x63bb);
  CYC(0x63bb, 0x63bc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63bc, 0x63bd); C = mem_rd(gb, HL);
  CYC(0x63bd, 0x63be); H = D;
  CYC(0x63be, 0x63c0); L = 0x0b;
  CYC(0x63c0, 0x63c1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x63c1, 0x63c2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x63c2, 0x63c3); L = alu_inc8(gb, L);
  CYC(0x63c3, 0x63c4); A = mem_rd(gb, HL);
  CYC(0x63c4, 0x63c5); alu_add(gb, C);
  CYC(0x63c5, 0x63c6); mem_wr(gb, HL, A);
  CYC(0x63c6, 0x63c8); L = 0x00;
  CYC(0x63c8, 0x63ca); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(0x63ca, 0x63cc); L = 0x37;
  CYC(0x63cc, 0x63ce); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(0x63ce, 0x63cf); L = alu_inc8(gb, L);
  CYC(0x63cf, 0x63d0); A = mem_rd(gb, HL);
  CYC(0x63d0, 0x63d2); alu_and(gb, 0xf0);
  CYC(0x63d2, 0x63d4); A = alu_swap(gb, A);
  CYC(0x63d4, 0x63d5); alu_add(gb, A);
  CYC(0x63d5, 0x63d8); SET_HL(0x64ba);
  CYC(0x63d8, 0x63d9); common_bomb_add_double_index(gb, 0x63d9);
  CYC(0x63d9, 0x63da); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63da, 0x63dc); E = 0x39;
  CYC(0x63dc, 0x63dd); mem_wr(gb, DE, A);
  CYC(0x63dd, 0x63df); E = 0x09;
  CYC(0x63df, 0x63e0); A = mem_rd(gb, DE);
  CYC(0x63e0, 0x63e1); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x63e1, 0x63e3);
    goto clear_speed;
  }
  CYC(0x63e1, 0x63e3);
  CYC(0x63e3, 0x63e5); E = 0x14;
  CYC(0x63e5, 0x63e6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63e6, 0x63e7); mem_wr(gb, DE, A);
  CYC(0x63e7, 0x63e8); E = alu_inc8(gb, E);
  CYC(0x63e8, 0x63ea); A = 0xff;
  CYC(0x63ea, 0x63eb); mem_wr(gb, DE, A);
  CYC(0x63eb, 0x63ed); A = 0x12;
  CALL_C(0x63ed, cpActiveRing_hook, 0x23b0, 0x63f0);
  if (!(F & FZ)) {
    CYCT(0x63f0, 0x63f2);
  } else {
    CYC(0x63f0, 0x63f2);
    CYC(0x63f2, 0x63f3); SET_HL(HL + 1);
  }
  CYC(0x63f3, 0x63f5); E = 0x10;
  CYC(0x63f5, 0x63f6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63f6, 0x63f7); mem_wr(gb, DE, A);
  CYC(0x63f7, 0x63f8); ret_effect(gb);
  return;

clear_speed:
  CYC(0x63f8, 0x63f9); H = D;
  CYC(0x63f9, 0x63fb); L = 0x10;
  CYC(0x63fb, 0x63fc); alu_xor(gb, A);
  CYC(0x63fc, 0x63fd); mem_wr(gb, HL, A);
  CYC(0x63fd, 0x63ff); L = 0x14;
  CYC(0x63ff, 0x6400); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6400, 0x6401); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6401, 0x6402); ret_effect(gb);
}

void itemUpdateThrowingLaterally_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6407, 0x6409); E = 0x38;
  CYC(0x6409, 0x640a); A = mem_rd(gb, DE);
  CYC(0x640a, 0x640c); alu_cp(gb, 0x40);
  if (!(F & FC)) {
    CYCT(0x640c, 0x640e);
    goto normal;
  }
  CYC(0x640c, 0x640e);
  CYC(0x640e, 0x6410); alu_cp(gb, 0x30);
  if (!(F & FC)) {
    CYCT(0x6410, 0x6412);
    goto weight3;
  }
  CYC(0x6410, 0x6412);

normal:
  CYC(0x6412, 0x6414); E = 0x09;
  CYC(0x6414, 0x6415); A = mem_rd(gb, DE);
  CYC(0x6415, 0x6417); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x6417, 0x6419);
    goto unset_z;
  }
  CYC(0x6417, 0x6419);
  CYC(0x6419, 0x641b); alu_and(gb, 0x18);
  CYC(0x641b, 0x641c); alu_rrca(gb);
  CYC(0x641c, 0x641d); alu_rrca(gb);
  CYC(0x641d, 0x6420); SET_HL(0x4c33);
  CYC(0x6420, 0x6421); common_bomb_add_a_to_hl(gb, 0x6421);
  CYC(0x6421, 0x6422); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6422, 0x6423); C = mem_rd(gb, HL);
  CYC(0x6423, 0x6424); H = D;
  CYC(0x6424, 0x6426); L = 0x0b;
  CYC(0x6426, 0x6427); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6427, 0x6429); alu_cp(gb, 0xb0);
  if (!(F & FC)) {
    CYCT(0x6429, 0x642b);
    goto no_collision;
  }
  CYC(0x6429, 0x642b);
  CYC(0x642b, 0x642c); B = A;
  CYC(0x642c, 0x642e); L = 0x0d;
  CYC(0x642e, 0x642f); A = C;
  CYC(0x642f, 0x6430); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6430, 0x6431); C = A;
  CALL_C(0x6431, checkTileCollisionAt_allowHoles_hook, 0x14d1, 0x6434);
  if (!(F & FC)) {
    CYCT(0x6434, 0x6436);
    goto no_collision;
  }
  CYC(0x6434, 0x6436);
  CALL_C(0x6436, itemCheckCanPassSolidTileAt_hook, 0x4b90, 0x6439);
  if (F & FZ) {
    CYCT(0x6439, 0x643b);
    goto no_collision;
  }
  CYC(0x6439, 0x643b);
  CYC(0x643b, 0x643d);
  goto collision;

weight3:
  CYC(0x643d, 0x643e); H = D;
  CYC(0x643e, 0x6440); L = 0x0b;
  CYC(0x6440, 0x6441); B = mem_rd(gb, HL);
  CYC(0x6441, 0x6443); L = 0x0d;
  CYC(0x6443, 0x6444); C = mem_rd(gb, HL);
  CYC(0x6444, 0x6446); E = 0x09;
  CYC(0x6446, 0x6447); A = mem_rd(gb, DE);
  CYC(0x6447, 0x6449); alu_and(gb, 0x18);
  CYC(0x6449, 0x644c); SET_HL(0x649a);
  CYC(0x644c, 0x644d); common_bomb_add_a_to_hl(gb, 0x644d);
  CYC(0x644d, 0x644f); E = 0x04;

check_corner:
  CYC(0x644f, 0x6450); push_effect(gb, BC);
  CYC(0x6450, 0x6451); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6451, 0x6452); alu_add(gb, B);
  CYC(0x6452, 0x6453); B = A;
  CYC(0x6453, 0x6454); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6454, 0x6455); alu_add(gb, C);
  CYC(0x6455, 0x6456); C = A;
  CYC(0x6456, 0x6457); push_effect(gb, HL);
  CALL_C(0x6457, checkTileCollisionAt_allowHoles_hook, 0x14d1, 0x645a);
  CYC(0x645a, 0x645b); SET_HL(pop_effect(gb));
  CYC(0x645b, 0x645c); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(0x645c, 0x645e);
    goto collision;
  }
  CYC(0x645c, 0x645e);
  CYC(0x645e, 0x645f); E = alu_dec8(gb, E);
  if (!(F & FZ)) {
    CYCT(0x645f, 0x6461);
    goto check_corner;
  }
  CYC(0x645f, 0x6461);
  CYC(0x6461, 0x6463);
  goto no_collision;

collision:
  CALL_C(0x6463, braceletCheckBreakable_hook, 0x636d, 0x6466);
  if (!(F & FZ)) {
    CYCT(0x6466, 0x6468);
    goto set_z;
  }
  CYC(0x6466, 0x6468);
  CYC(0x6468, 0x646a); E = 0x09;
  CYC(0x646a, 0x646c); A = 0xff;
  CYC(0x646c, 0x646d); mem_wr(gb, DE, A);

no_collision:
  CYC(0x646d, 0x6470); A = W8(wTilesetFlags);
  CYC(0x6470, 0x6472); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x6472, 0x6474);
    goto apply_speed;
  }
  CYC(0x6472, 0x6474);
  CYC(0x6474, 0x6476); E = 0x09;
  CYC(0x6476, 0x6477); A = mem_rd(gb, DE);
  CYC(0x6477, 0x6479); alu_and(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x6479, 0x647b);
    goto unset_z;
  }
  CYC(0x6479, 0x647b);

apply_speed:
  CALL_C(0x647b, objectApplySpeed_hook, 0x201d, 0x647e);

unset_z:
  CYC(0x647e, 0x647f); alu_or(gb, D);
  CYC(0x647f, 0x6480); ret_effect(gb);
  return;

set_z:
  CYC(0x6480, 0x6481); alu_xor(gb, A);
  CYC(0x6481, 0x6482); ret_effect(gb);
}

void itemBounce_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6482, 0x6484); A = 0x52;
  CALL_C(0x6484, playSound_b00_hook, 0x0c98, 0x6487);
  CALL_C(0x6487, objectNegateAndHalveSpeedZ_hook, 0x2374, 0x648a);
  if (F & FC) {
    CYCT(0x648a, 0x648b); ret_effect(gb); return;
  }
  CYC(0x648a, 0x648b);
  CYC(0x648b, 0x648d); E = 0x10;
  CYC(0x648d, 0x648e); A = mem_rd(gb, DE);
  CYC(0x648e, 0x648f); E = A;
  CYC(0x648f, 0x6492); SET_HL(0x64d2);
  CALL_C(0x6492, lookupKey_hook, 0x1e06, 0x6495);
  CYC(0x6495, 0x6497); E = 0x10;
  CYC(0x6497, 0x6498); mem_wr(gb, DE, A);
  CYC(0x6498, 0x6499); alu_or(gb, A);
  CYC(0x6499, 0x649a); ret_effect(gb);
}
