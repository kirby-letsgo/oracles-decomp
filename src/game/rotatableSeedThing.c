#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t rotatableSeedThing_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void rotatableSeedThing_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// PART_ROTATABLE_SEED_THING
void partCode33_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x64dd, 0x64df); E = 0xc2; // Part.subid
  CYC(0x64df, 0x64e0); A = mem_rd(gb, DE);
  CYC(0x64e0, 0x64e1); B = A;
  CYC(0x64e1, 0x64e3); alu_and(gb, 0x03);
  CYC(0x64e3, 0x64e5); E = 0xc4; // Part.state
  {
    CYC(0x64e5, 0x64e6); push_effect(gb, 0x64e6);
    uint16_t target = rotatableSeedThing_jump_table(gb);
    if (target == 0x653e) goto subid1;
    if (target == 0x6566) goto subid2;
    if (target == 0x65c0) goto subid3;
    goto subid0;
  }

subid0:
  CYC(0x64ee, 0x64ef); A = mem_rd(gb, DE);
  CYC(0x64ef, 0x64f0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x64f0, 0x64f2); goto subid0_state0; } // jr z
  CYC(0x64f0, 0x64f2);

func_64f2:
  CALL_C(0x64f2, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x64f5);
  if (!(F & FZ)) { RET_TAKEN(0x64f5); return; } // ret nz
  CYC(0x64f5, 0x64f6);
  CYC(0x64f6, 0x64f8); E = 0xf0; // Part.var30
  CYC(0x64f8, 0x64f9); A = mem_rd(gb, DE);
  CYC(0x64f9, 0x64fa); mem_wr(gb, HL, A);
  CYC(0x64fa, 0x64fd); goto func_657e; // jp

subid0_state0:
  CYC(0x64fd, 0x64fe); C = B;
  CYC(0x64fe, 0x6500); C = alu_rlc(gb, C);
  CYC(0x6500, 0x6502); A = 0x01;
  if (!(F & FC)) { CYCT(0x6502, 0x6504); goto L_6506; } // jr nc
  CYC(0x6502, 0x6504);
  CYC(0x6504, 0x6506); A = 0xff;

L_6506:
  CYC(0x6506, 0x6507); H = D;
  CYC(0x6507, 0x6509); L = 0xf1; // Part.var31
  CYC(0x6509, 0x650a); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x650a, 0x650c); C = alu_rlc(gb, C);
  CYC(0x650c, 0x650e); A = 0x3c;
  if (!(F & FC)) { CYCT(0x650e, 0x6510); goto L_6511; } // jr nc
  CYC(0x650e, 0x6510);
  CYC(0x6510, 0x6511); alu_add(gb, A);

L_6511:
  CYC(0x6511, 0x6512); mem_wr(gb, HL, A);
  CYC(0x6512, 0x6514); L = 0xc6; // Part.counter1
  CYC(0x6514, 0x6515); mem_wr(gb, HL, A);

func_6515:
  CYC(0x6515, 0x6516); A = B;
  CYC(0x6516, 0x6517); alu_rrca(gb);
  CYC(0x6517, 0x6518); alu_rrca(gb);
  CYC(0x6518, 0x651a); alu_and(gb, 0x03);
  CYC(0x651a, 0x651c); E = 0xc8;
  CYC(0x651c, 0x651d); mem_wr(gb, DE, A);
  CYC(0x651d, 0x6520); push_effect(gb, 0x6520); goto func_6588; // call $6588 (own burn; local, inlined)

func_6515_afterFunc6588:
  CALL_C(0x6520, objectMakeTileSolid_hook, 0x20b2, 0x6523);
  CYC(0x6523, 0x6525); H = 0xcf;
  CYC(0x6525, 0x6527); mem_wr(gb, HL, 0x0a);
  CALL_C(0x6527, objectSetVisible83_hook, 0x1e72, 0x652a);
  CALL_C(0x652a, getFreePartSlot_hook, 0x3e8e, 0x652d);
  if (!(F & FZ)) {
    RET_TAKEN(0x652d);
    if (gb->pc == 0x6551 && gb->sp == sp0_) goto func_6551;
    if (gb->pc == 0x65b5 && gb->sp == sp0_) goto subid2_state0_afterFunc6515;
    return;
  } // ret nz
  CYC(0x652d, 0x652e);
  CYC(0x652e, 0x6530); mem_wr(gb, HL, 0x33); // PART_ROTATABLE_SEED_THING
  CYC(0x6530, 0x6531); L = alu_inc8(gb, L);
  CYC(0x6531, 0x6533); mem_wr(gb, HL, 0x03);
  CYC(0x6533, 0x6535); L = 0xd6; // Part.relatedObj1
  CYC(0x6535, 0x6537); A = 0xc0;
  CYC(0x6537, 0x6538); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6538, 0x6539); mem_wr(gb, HL, D);
  CYC(0x6539, 0x653a); H = D;
  CYC(0x653a, 0x653c); L = 0xc4; // Part.state
  CYC(0x653c, 0x653d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x653d);
  if (gb->pc == 0x6551 && gb->sp == sp0_) goto func_6551;
  if (gb->pc == 0x65b5 && gb->sp == sp0_) goto subid2_state0_afterFunc6515;
  return; // ret

subid1:
  CYC(0x653e, 0x653f); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(0x653f, 0x6541); goto subid1_state0; } // jr z
  CYC(0x653f, 0x6541);
  CYC(0x6541, 0x6542); H = D;
  CYC(0x6542, 0x6544); L = 0xc3; // Part.var03
  CYC(0x6544, 0x6545); A = mem_rd(gb, HL);
  CYC(0x6545, 0x6547); L = 0xd8; // Part.relatedObj2
  CYC(0x6547, 0x6548); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6548, 0x6549); H = mem_rd(gb, HL);
  CYC(0x6549, 0x654a); L = A;
  CYC(0x654a, 0x654b); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x654b); return; } // ret z
  CYC(0x654b, 0x654c);
  CYC(0x654c, 0x654e); goto func_64f2; // jr

subid1_state0:
  CYC(0x654e, 0x6551); push_effect(gb, 0x6551); goto subid0_state0; // call $64fd (own burn; local, inlined)

func_6551:
  CYC(0x6551, 0x6553); E = 0xc2; // Part.subid
  CYC(0x6553, 0x6554); A = mem_rd(gb, DE);
  CYC(0x6554, 0x6556); alu_bit(gb, 4, A);
  CYC(0x6556, 0x6559); SET_HL(wToggleBlocksState);
  if (F & FZ) { CYCT(0x6559, 0x655b); goto L_655e; } // jr z
  CYC(0x6559, 0x655b);
  CYC(0x655b, 0x655e); SET_HL(wActiveTriggers);

L_655e:
  CYC(0x655e, 0x6560); E = 0xd8; // Part.relatedObj2
  CYC(0x6560, 0x6561); A = L;
  CYC(0x6561, 0x6562); mem_wr(gb, DE, A);
  CYC(0x6562, 0x6563); E = alu_inc8(gb, E);
  CYC(0x6563, 0x6564); A = H;
  CYC(0x6564, 0x6565); mem_wr(gb, DE, A);
  RET(0x6565);
  if (gb->pc == 0x65b8 && gb->sp == sp0_) goto subid2_state0_afterFunc6551;
  return; // ret

subid2:
  CYC(0x6566, 0x6567); A = mem_rd(gb, DE);
  CYC(0x6567, 0x6568); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6568, 0x656a); goto subid2_state0; } // jr z
  CYC(0x6568, 0x656a);
  CYC(0x656a, 0x656b); H = D;
  CYC(0x656b, 0x656d); L = 0xf2; // Part.var32
  CYC(0x656d, 0x656e); E = L;
  CYC(0x656e, 0x656f); B = mem_rd(gb, HL);
  CYC(0x656f, 0x6571); L = 0xc3; // Part.var03
  CYC(0x6571, 0x6572); C = mem_rd(gb, HL);
  CYC(0x6572, 0x6574); L = 0xd8; // Part.relatedObj2
  CYC(0x6574, 0x6575); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6575, 0x6576); H = mem_rd(gb, HL);
  CYC(0x6576, 0x6577); L = A;
  CYC(0x6577, 0x6578); A = mem_rd(gb, HL);
  CYC(0x6578, 0x6579); alu_and(gb, C);
  CYC(0x6579, 0x657a); C = A;
  CYC(0x657a, 0x657b); alu_xor(gb, B);
  if (F & FZ) { RET_TAKEN(0x657b); return; } // ret z
  CYC(0x657b, 0x657c);
  CYC(0x657c, 0x657d); A = C;
  CYC(0x657d, 0x657e); mem_wr(gb, DE, A);

func_657e:
  CYC(0x657e, 0x657f); H = D;
  CYC(0x657f, 0x6581); L = 0xf1; // Part.var31
  CYC(0x6581, 0x6583); E = 0xc8;
  CYC(0x6583, 0x6584); A = mem_rd(gb, DE);
  CYC(0x6584, 0x6585); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6585, 0x6587); alu_and(gb, 0x03);
  CYC(0x6587, 0x6588); mem_wr(gb, DE, A);

func_6588:
  CYC(0x6588, 0x6589); B = A;
  CYC(0x6589, 0x658c); SET_HL(0x6598); // table_6598
  CYC(0x658c, 0x658d); rotatableSeedThing_addDoubleIndexToHl_from_rst(gb, 0x658d);
  CYC(0x658d, 0x658f); E = 0xe6; // Part.collisionRadiusY
  CYC(0x658f, 0x6590); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6590, 0x6591); mem_wr(gb, DE, A);
  CYC(0x6591, 0x6592); E = alu_inc8(gb, E);
  CYC(0x6592, 0x6593); A = mem_rd(gb, HL);
  CYC(0x6593, 0x6594); mem_wr(gb, DE, A);
  CYC(0x6594, 0x6595); A = B;
  CYC(0x6595, 0x6598); partSetAnimation_hook(gb);
  if (gb->pc == 0x6520 && gb->sp == sp0_) goto func_6515_afterFunc6588;
  return; // jp

subid2_state0:
  CYC(0x65a0, 0x65a1); C = B;
  CYC(0x65a1, 0x65a3); C = alu_rlc(gb, C);
  CYC(0x65a3, 0x65a5); A = 0x01;
  if (!(F & FC)) { CYCT(0x65a5, 0x65a7); goto L_65a9; } // jr nc
  CYC(0x65a5, 0x65a7);
  CYC(0x65a7, 0x65a9); A = 0xff;

L_65a9:
  CYC(0x65a9, 0x65ab); C = alu_rlc(gb, C);
  if (!(F & FC)) { CYCT(0x65ab, 0x65ad); goto L_65ae; } // jr nc
  CYC(0x65ab, 0x65ad);
  CYC(0x65ad, 0x65ae); alu_add(gb, A);

L_65ae:
  CYC(0x65ae, 0x65af); H = D;
  CYC(0x65af, 0x65b1); L = 0xf1; // Part.var31
  CYC(0x65b1, 0x65b2); mem_wr(gb, HL, A);
  CYC(0x65b2, 0x65b5); push_effect(gb, 0x65b5); goto func_6515; // call $6515 (own burn; local, inlined)

subid2_state0_afterFunc6515:
  CYC(0x65b5, 0x65b8); push_effect(gb, 0x65b8); goto func_6551; // call $6551 (own burn; local, inlined)

subid2_state0_afterFunc6551:
  CYC(0x65b8, 0x65ba); E = 0xc3; // Part.var03
  CYC(0x65ba, 0x65bb); A = mem_rd(gb, DE);
  CYC(0x65bb, 0x65bc); alu_and(gb, mem_rd(gb, HL));
  CYC(0x65bc, 0x65be); E = 0xf2; // Part.var32
  CYC(0x65be, 0x65bf); mem_wr(gb, DE, A);
  RET(0x65bf); return; // ret

subid3:
  CYC(0x65c0, 0x65c1); A = mem_rd(gb, DE);
  CYC(0x65c1, 0x65c2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x65c2, 0x65c4); func_65d5_hook(gb); return; } // jr z
  CYC(0x65c2, 0x65c4);
  CYC(0x65c4, 0x65c6); A = 0x21;
  CALL_C(0x65c6, objectGetRelatedObject1Var_hook, 0x2160, 0x65c9);
  CYC(0x65c9, 0x65ca); E = L;
  CYC(0x65ca, 0x65cb); A = mem_rd(gb, HL);
  CYC(0x65cb, 0x65cc); mem_wr(gb, DE, A);
  CYC(0x65cc, 0x65ce); L = 0xe6; // Part.collisionRadiusY
  CYC(0x65ce, 0x65cf); E = L;
  CYC(0x65cf, 0x65d0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x65d0, 0x65d1); mem_wr(gb, DE, A);
  CYC(0x65d1, 0x65d2); E = alu_inc8(gb, E);
  CYC(0x65d2, 0x65d3); A = mem_rd(gb, HL);
  CYC(0x65d3, 0x65d4); mem_wr(gb, DE, A);
  RET(0x65d4); return; // ret
}

void func_65d5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65d5, 0x65d7); A = 0x0b;
  CALL_C(0x65d7, objectGetRelatedObject1Var_hook, 0x2160, 0x65da);
  CYC(0x65da, 0x65dd); SET_BC(0x0c00);
  CALL_C(0x65dd, objectTakePositionWithOffset_hook, 0x2277, 0x65e0);
  CYC(0x65e0, 0x65e1); H = D;
  CYC(0x65e1, 0x65e3); L = 0xc4; // Part.state
  CYC(0x65e3, 0x65e4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x65e4, 0x65e6); L = 0xcf; // Part.zh
  CYC(0x65e6, 0x65e8); mem_wr(gb, HL, 0xf2);
  RET(0x65e8); return; // ret
}
