#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t spikedBall_jump_table(GB *gb) {
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

void partCode2a_hook(GB *gb);
void spikedBall_head_hook(GB *gb);
void spikedBall_head_state0_hook(GB *gb);
void spikedBall_head_state1_hook(GB *gb);
void spikedBall_head_state2_hook(GB *gb);
void spikedBall_head_setDefaultDistanceAway_hook(GB *gb);
void spikedBall_updatePosition_hook(GB *gb);
void spikedBall_head_state3_hook(GB *gb);
void spikedBall_head_state4_hook(GB *gb);
void spikedBall_head_state5_hook(GB *gb);
void spikedBall_chain_hook(GB *gb);
void spikedBall_copyParentPosition_hook(GB *gb);
void spikedBall_checkCollisionWithItem_hook(GB *gb);
void spikedBall_head_updateDistanceFromOrigin_hook(GB *gb);
void spikedBall_updateStateFromParent_hook(GB *gb);
void spikedBall_chain_updateDistanceFromOrigin_hook(GB *gb);

void partCode2a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5769, 0x576b); goto normalStatus; } // jr z
  CYC(0x5769, 0x576b);
  CYC(0x576b, 0x576d); E = 0xea; // Part.var2a
  CYC(0x576d, 0x576e); A = mem_rd(gb, DE);
  CYC(0x576e, 0x5770); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x5770, 0x5772); alu_sub(gb, 0x01); // ITEMCOLLISION_L1_SHIELD
  CYC(0x5772, 0x5774); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x5774, 0x5776); goto normalStatus; } // jr nc
  CYC(0x5774, 0x5776);
  CYC(0x5776, 0x5778); A = 0x2b; // Object.invincibilityCounter
  CALL_C(0x5778, objectGetRelatedObject1Var_hook, 0x2160, 0x577b);
  CYC(0x577b, 0x577c); A = mem_rd(gb, HL);
  CYC(0x577c, 0x577d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x577d, 0x577f); goto L_5781; } // jr nz
  CYC(0x577d, 0x577f);
  CYC(0x577f, 0x5781); mem_wr(gb, HL, 0xf4);
L_5781:
  CYC(0x5781, 0x5782); H = D;
  CYC(0x5782, 0x5784); L = 0xd5; // Part.speedZ+1
  CYC(0x5784, 0x5785); A = mem_rd(gb, HL);
  CYC(0x5785, 0x5786); alu_rlca(gb);
  if (F & FC) { CYCT(0x5786, 0x5788); goto normalStatus; } // jr c
  CYC(0x5786, 0x5788);
  CYC(0x5788, 0x5789); alu_xor(gb, A);
  CYC(0x5789, 0x578a); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x578a, 0x578b); mem_wr(gb, HL, A);

normalStatus:
  CYC(0x578b, 0x578d); E = 0xc2; // Part.subid
  CYC(0x578d, 0x578e); A = mem_rd(gb, DE);
  CYC(0x578e, 0x578f); B = A;
  CYC(0x578f, 0x5791); E = 0xc4; // Part.state
  CYC(0x5791, 0x5792); A = B;
  {
    CYC(0x5792, 0x5793); push_effect(gb, 0x5793);
    uint16_t target = spikedBall_jump_table(gb);
    if (target == 0x579b) { spikedBall_head_hook(gb); return; }
    spikedBall_chain_hook(gb); return;
  }
}

void spikedBall_head_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x579b, 0x579d); A = 0x01; // Object.id
  CALL_C(0x579d, objectGetRelatedObject1Var_hook, 0x2160, 0x57a0);
  CYC(0x57a0, 0x57a1); A = mem_rd(gb, HL);
  CYC(0x57a1, 0x57a3); alu_cp(gb, 0x4b); // ENEMY_BALL_AND_CHAIN_SOLDIER
  if (!(F & FZ)) { CYCT(0x57a3, 0x57a6); partDelete_hook(gb); return; } // jp nz
  CYC(0x57a3, 0x57a6);
  CYC(0x57a6, 0x57a7); B = H;
  CALL_C(0x57a7, spikedBall_updateStateFromParent_hook, 0x58a5, 0x57aa);
  CYC(0x57aa, 0x57ac); E = 0xc4; // Part.state
  CYC(0x57ac, 0x57ad); A = mem_rd(gb, DE);
  CYC(0x57ad, 0x57ae); push_effect(gb, 0x57ae);
  {
    uint16_t target = spikedBall_jump_table(gb);
    if (target == 0x57ba) { spikedBall_head_state0_hook(gb); return; }
    if (target == 0x57c4) { spikedBall_head_state1_hook(gb); return; }
    if (target == 0x57cd) { spikedBall_head_state2_hook(gb); return; }
    if (target == 0x57e5) { spikedBall_head_state3_hook(gb); return; }
    if (target == 0x5817) { spikedBall_head_state4_hook(gb); return; }
    spikedBall_head_state5_hook(gb); return;
  }
}

void spikedBall_head_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x57ba, 0x57bb); H = D;
  CYC(0x57bb, 0x57bc); L = E; // Part.state
  CYC(0x57bc, 0x57bd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x57bd, 0x57bf); L = 0xe4; // Part.collisionType
  CYC(0x57bf, 0x57c1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(0x57c1, objectSetVisible81_hook, 0x1e60, 0x57c4);
  spikedBall_head_state1_hook(gb); // falls through
}

void spikedBall_head_state1_hook(GB *gb) {
  CYC(0x57c4, 0x57c6); E = 0xc9; // Part.angle
  CYC(0x57c6, 0x57c7); A = mem_rd(gb, DE);
  CYC(0x57c7, 0x57c8); A = alu_inc8(gb, A);
  CYC(0x57c8, 0x57ca); alu_and(gb, 0x1f);
  CYC(0x57ca, 0x57cb); mem_wr(gb, DE, A);
  CYC(0x57cb, 0x57cd); spikedBall_head_setDefaultDistanceAway_hook(gb); return; // jr
}

void spikedBall_head_state2_hook(GB *gb) {
  CYC(0x57cd, 0x57cf); E = 0xc9; // Part.angle
  CYC(0x57cf, 0x57d0); A = mem_rd(gb, DE);
  CYC(0x57d0, 0x57d2); alu_add(gb, 0x02);
  CYC(0x57d2, 0x57d4); alu_and(gb, 0x1f);
  CYC(0x57d4, 0x57d5); mem_wr(gb, DE, A);
  spikedBall_head_setDefaultDistanceAway_hook(gb); // falls through
}

void spikedBall_head_setDefaultDistanceAway_hook(GB *gb) {
  CYC(0x57d5, 0x57d7); E = 0xf0; // Part.var30
  CYC(0x57d7, 0x57d9); A = 0x0a;
  CYC(0x57d9, 0x57da); mem_wr(gb, DE, A);
  spikedBall_updatePosition_hook(gb); // falls through
}

void spikedBall_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57da, spikedBall_copyParentPosition_hook, 0x5862, 0x57dd);
  CYC(0x57dd, 0x57df); E = 0xf0; // Part.var30
  CYC(0x57df, 0x57e0); A = mem_rd(gb, DE);
  CYC(0x57e0, 0x57e2); E = 0xc9; // Part.angle
  CYC(0x57e2, 0x57e5); objectSetPositionInCircleArc_hook(gb); return; // jp
}

void spikedBall_head_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57e5, spikedBall_copyParentPosition_hook, 0x5862, 0x57e8);
  CYC(0x57e8, 0x57ea); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x57ea, 0x57ec); hram_wr(gb, 0x8f, A);
  CYC(0x57ec, 0x57ee); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x57ee, 0x57f0); hram_wr(gb, 0x8e, A);
  CYC(0x57f0, 0x57f1); push_effect(gb, HL);
  CALL_C(0x57f1, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x57f4);
  CYC(0x57f4, 0x57f5); SET_BC(pop_effect(gb));
  CYC(0x57f5, 0x57f7); alu_xor(gb, 0x10);
  CYC(0x57f7, 0x57f8); E = A;
  CYC(0x57f8, 0x57fa); alu_sub(gb, 0x06);
  CYC(0x57fa, 0x57fc); alu_and(gb, 0x1f);
  CYC(0x57fc, 0x57fd); H = D;
  CYC(0x57fd, 0x57ff); L = 0xc9; // Part.angle
  CYC(0x57ff, 0x5800); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5800, 0x5801); A = alu_inc8(gb, A);
  CYC(0x5801, 0x5803); alu_and(gb, 0x1f);
  CYC(0x5803, 0x5805); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x5805, 0x5807); spikedBall_head_state2_hook(gb); return; } // jr nc
  CYC(0x5805, 0x5807);
  CYC(0x5807, 0x5808); A = E;
  CYC(0x5808, 0x580a); alu_sub(gb, 0x03);
  CYC(0x580a, 0x580c); alu_and(gb, 0x1f);
  CYC(0x580c, 0x580d); mem_wr(gb, HL, A);
  CYC(0x580d, 0x580f); L = 0xc4; // Part.state
  CYC(0x580f, 0x5810); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5810, 0x5812); L = 0xf0; // Part.var30
  CYC(0x5812, 0x5814); mem_wr(gb, HL, 0x0d);
  CYC(0x5814, 0x5817); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_head_state4_hook(GB *gb) {
  CYC(0x5817, 0x5818); H = D;
  CYC(0x5818, 0x5819); L = E; // Part.state
  CYC(0x5819, 0x581a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x581a, 0x581c); L = 0xc6; // Part.counter1
  CYC(0x581c, 0x581e); mem_wr(gb, HL, 0x00);
  CYC(0x581e, 0x5820); L = 0xc9; // Part.angle
  CYC(0x5820, 0x5821); A = mem_rd(gb, HL);
  CYC(0x5821, 0x5823); alu_add(gb, 0x03);
  CYC(0x5823, 0x5825); alu_and(gb, 0x1f);
  CYC(0x5825, 0x5826); mem_wr(gb, HL, A);
  CYC(0x5826, 0x5828); L = 0xf0; // Part.var30
  CYC(0x5828, 0x582a); mem_wr(gb, HL, 0x12);
  CYC(0x582a, 0x582c); L = 0xd0; // Part.speed
  CYC(0x582c, 0x582e); A = 0x40;
  CYC(0x582e, 0x582f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x582f, 0x5831); mem_wr(gb, HL, 0x03);
  CYC(0x5831, 0x5834); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_head_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5834, spikedBall_checkCollisionWithItem_hook, 0x5874, 0x5837);
  CALL_C(0x5837, spikedBall_head_updateDistanceFromOrigin_hook, 0x5887, 0x583a);
  CYC(0x583a, 0x583d); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_chain_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x583d, 0x583e); A = mem_rd(gb, DE); // Part.state (DE set by partCode2a)
  CYC(0x583e, 0x583f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x583f, 0x5841); goto chain_state1; } // jr nz
  CYC(0x583f, 0x5841);
  CYC(0x5841, 0x5842); A = alu_inc8(gb, A);
  CYC(0x5842, 0x5843); mem_wr(gb, DE, A);
  CALL_C(0x5843, partSetAnimation_hook, 0x2988, 0x5846);
  CALL_C(0x5846, objectSetVisible81_hook, 0x1e60, 0x5849);

chain_state1:
  CYC(0x5849, 0x584b); A = 0x01; // Object.id
  CALL_C(0x584b, objectGetRelatedObject1Var_hook, 0x2160, 0x584e);
  CYC(0x584e, 0x584f); A = mem_rd(gb, HL);
  CYC(0x584f, 0x5851); alu_cp(gb, 0x2a); // PART_SPIKED_BALL
  if (!(F & FZ)) { CYCT(0x5851, 0x5854); partDelete_hook(gb); return; } // jp nz
  CYC(0x5851, 0x5854);
  CYC(0x5854, 0x5856); L = 0xc9; // Part.angle
  CYC(0x5856, 0x5857); E = L;
  CYC(0x5857, 0x5858); A = mem_rd(gb, HL);
  CYC(0x5858, 0x5859); mem_wr(gb, DE, A);
  CALL_C(0x5859, spikedBall_chain_updateDistanceFromOrigin_hook, 0x58c5, 0x585c);
  CYC(0x585c, 0x585e); L = 0xd7; // Part.relatedObj1+1
  CYC(0x585e, 0x585f); B = mem_rd(gb, HL);
  CYC(0x585f, 0x5862); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_copyParentPosition_hook(GB *gb) {
  CYC(0x5862, 0x5863); H = B;
  CYC(0x5863, 0x5865); L = 0x8b; // Enemy.yh
  CYC(0x5865, 0x5866); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5866, 0x5868); alu_sub(gb, 0x05);
  CYC(0x5868, 0x5869); B = A;
  CYC(0x5869, 0x586a); L = alu_inc8(gb, L);
  CYC(0x586a, 0x586b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x586b, 0x586d); alu_sub(gb, 0x05);
  CYC(0x586d, 0x586e); C = A;
  CYC(0x586e, 0x586f); L = alu_inc8(gb, L);
  CYC(0x586f, 0x5870); A = mem_rd(gb, HL);
  CYC(0x5870, 0x5872); E = 0xcf; // Part.zh
  CYC(0x5872, 0x5873); mem_wr(gb, DE, A);
  RET(0x5873); return; // ret
}

void spikedBall_checkCollisionWithItem_hook(GB *gb) {
  CYC(0x5874, 0x5875); H = D;
  CYC(0x5875, 0x5877); L = 0xea; // Part.var2a
  CYC(0x5877, 0x5879); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5879); return; } // ret z
  CYC(0x5879, 0x587a);
  CYC(0x587a, 0x587b); A = mem_rd(gb, HL);
  CYC(0x587b, 0x587d); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { RET_TAKEN(0x587d); return; } // ret z
  CYC(0x587d, 0x587e);
  CYC(0x587e, 0x5880); L = 0xd1; // Part.speed+1
  CYC(0x5880, 0x5882); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x5882); return; } // ret nz
  CYC(0x5882, 0x5883);
  CYC(0x5883, 0x5884); alu_xor(gb, A);
  CYC(0x5884, 0x5885); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5885, 0x5886); mem_wr(gb, HL, A);
  RET(0x5886); return; // ret
}

void spikedBall_head_updateDistanceFromOrigin_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5887, 0x5888); H = D;
  CYC(0x5888, 0x588a); E = 0xf0; // Part.var30
  CYC(0x588a, 0x588c); L = 0xd1; // Part.speed+1
  CYC(0x588c, 0x588d); A = mem_rd(gb, DE);
  CYC(0x588d, 0x588e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x588e, 0x5890); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(0x5890, 0x5892); goto fullyRetracted; } // jr c
  CYC(0x5890, 0x5892);
  CYC(0x5892, 0x5893); mem_wr(gb, DE, A);
  CYC(0x5893, 0x5894); L = alu_dec8(gb, L);
  CYC(0x5894, 0x5895); A = mem_rd(gb, HL);
  CYC(0x5895, 0x5897); alu_sub(gb, 0x20);
  CYC(0x5897, 0x5898); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5898, 0x5899); A = mem_rd(gb, HL);
  CYC(0x5899, 0x589b); alu_sbc(gb, 0x00);
  CYC(0x589b, 0x589c); mem_wr(gb, HL, A);
  RET(0x589c); return; // ret

fullyRetracted:
  CYC(0x589d, 0x589f); A = 0x06; // Object.counter1
  CALL_C(0x589f, objectGetRelatedObject1Var_hook, 0x2160, 0x58a2);
  CYC(0x58a2, 0x58a4); mem_wr(gb, HL, 0x00);
  RET(0x58a4); return; // ret
}

void spikedBall_updateStateFromParent_hook(GB *gb) {
  CYC(0x58a5, 0x58a7); L = 0xb0; // Enemy.var30
  CYC(0x58a7, 0x58a9); E = 0xc4; // Part.state
  CYC(0x58a9, 0x58aa); A = mem_rd(gb, DE);
  CYC(0x58aa, 0x58ab); A = alu_dec8(gb, A);
  CYC(0x58ab, 0x58ad); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x58ad, 0x58af); goto L_58b5; } // jr c
  CYC(0x58ad, 0x58af);
  CYC(0x58af, 0x58b0); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(0x58b0); return; } // ret z
  CYC(0x58b0, 0x58b1);
  CYC(0x58b1, 0x58b2); A = mem_rd(gb, HL);
  CYC(0x58b2, 0x58b4); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(0x58b4); return; } // ret z
  CYC(0x58b4, 0x58b5);

L_58b5:
  CYC(0x58b5, 0x58b6); A = mem_rd(gb, HL);
  CYC(0x58b6, 0x58b7); alu_or(gb, A);
  CYC(0x58b7, 0x58b9); C = 0x01;
  if (F & FZ) { CYCT(0x58b9, 0x58bb); goto L_58c0; } // jr z
  CYC(0x58b9, 0x58bb);
  CYC(0x58bb, 0x58bc); C = alu_inc8(gb, C);
  CYC(0x58bc, 0x58bd); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x58bd, 0x58bf); goto L_58c0; } // jr z
  CYC(0x58bd, 0x58bf);
  CYC(0x58bf, 0x58c0); C = alu_inc8(gb, C);

L_58c0:
  CYC(0x58c0, 0x58c2); E = 0xc4; // Part.state
  CYC(0x58c2, 0x58c3); A = C;
  CYC(0x58c3, 0x58c4); mem_wr(gb, DE, A);
  RET(0x58c4); return; // ret
}

void spikedBall_chain_updateDistanceFromOrigin_hook(GB *gb) {
  CYC(0x58c5, 0x58c7); L = 0xf0; // Part.var30
  CYC(0x58c7, 0x58c8); push_effect(gb, HL);
  CYC(0x58c8, 0x58ca); E = 0xc2; // Part.subid
  CYC(0x58ca, 0x58cb); A = mem_rd(gb, DE);
  CYC(0x58cb, 0x58cc); A = alu_dec8(gb, A);
  {
    CYC(0x58cc, 0x58cd); push_effect(gb, 0x58cd);
    uint16_t target = spikedBall_jump_table(gb);
    if (target == 0x58d3) goto subid1;
    if (target == 0x58e0) goto subid2;
    goto subid3;
  }

subid1:
  CYC(0x58d3, 0x58d4); SET_HL(pop_effect(gb));
  CYC(0x58d4, 0x58d5); E = L;
  CYC(0x58d5, 0x58d6); A = mem_rd(gb, HL);
  CYC(0x58d6, 0x58d8); A = alu_srl(gb, A);
  CYC(0x58d8, 0x58da); A = alu_srl(gb, A);
  CYC(0x58da, 0x58db); B = A;
  CYC(0x58db, 0x58dc); alu_add(gb, A);
  CYC(0x58dc, 0x58dd); alu_add(gb, B);
  CYC(0x58dd, 0x58de); A = alu_inc8(gb, A);
  CYC(0x58de, 0x58df); mem_wr(gb, DE, A);
  RET(0x58df); return; // ret

subid2:
  CYC(0x58e0, 0x58e1); SET_HL(pop_effect(gb));
  CYC(0x58e1, 0x58e2); E = L;
  CYC(0x58e2, 0x58e3); A = mem_rd(gb, HL);
  CYC(0x58e3, 0x58e5); A = alu_srl(gb, A);
  CYC(0x58e5, 0x58e7); A = alu_srl(gb, A);
  CYC(0x58e7, 0x58e8); alu_add(gb, A);
  CYC(0x58e8, 0x58e9); mem_wr(gb, DE, A);
  RET(0x58e9); return; // ret

subid3:
  CYC(0x58ea, 0x58eb); SET_HL(pop_effect(gb));
  CYC(0x58eb, 0x58ec); E = L;
  CYC(0x58ec, 0x58ed); A = mem_rd(gb, HL);
  CYC(0x58ed, 0x58ef); A = alu_srl(gb, A);
  CYC(0x58ef, 0x58f1); A = alu_srl(gb, A);
  CYC(0x58f1, 0x58f2); mem_wr(gb, DE, A);
  RET(0x58f2); return; // ret
}
