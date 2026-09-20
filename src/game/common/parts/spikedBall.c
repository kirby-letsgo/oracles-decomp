#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCode2a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+7, b_+9); alu_sub(gb, 0x01); // ITEMCOLLISION_L1_SHIELD
  CYC(b_+9, b_+11); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto normalStatus; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0x2b; // Object.invincibilityCounter
  CALL_C(b_+15, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_5781; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0xf4);
L_5781:
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = 0xd5; // Part.speedZ+1
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_rlca(gb);
  if (F & FC) { CYCT(b_+29, b_+31); goto normalStatus; } // jr c
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);

normalStatus:
  CYC(b_+34, b_+36); E = 0xc2; // Part.subid
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); B = A;
  CYC(b_+38, b_+40); E = 0xc4; // Part.state
  CYC(b_+40, b_+41); A = B;
  {
    CYC(b_+41, b_+42); push_effect(gb, b_+42);
    uint16_t target = spikedBall_jump_table(gb);
    if (target == SYM(spikedBall_head)) { spikedBall_head_hook(gb); return; }
    spikedBall_chain_hook(gb); return;
  }
}

void spikedBall_head_hook(GB *gb) {
  BASE(spikedBall_head);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01; // Object.id
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x4b); // ENEMY_BALL_AND_CHAIN_SOLDIER
  if (!(F & FZ)) { CYCT(b_+8, b_+11); partDelete_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); B = H;
  CALL_C(b_+12, spikedBall_updateStateFromParent_hook, SYM(spikedBall_updateStateFromParent), b_+15);
  CYC(b_+15, b_+17); E = 0xc4; // Part.state
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); push_effect(gb, b_+19);
  {
    uint16_t target = spikedBall_jump_table(gb);
    if (target == SYM(spikedBall_head_state0)) { spikedBall_head_state0_hook(gb); return; }
    if (target == SYM(spikedBall_head_state1)) { spikedBall_head_state1_hook(gb); return; }
    if (target == SYM(spikedBall_head_state2)) { spikedBall_head_state2_hook(gb); return; }
    if (target == SYM(spikedBall_head_state3)) { spikedBall_head_state3_hook(gb); return; }
    if (target == SYM(spikedBall_head_state4)) { spikedBall_head_state4_hook(gb); return; }
    spikedBall_head_state5_hook(gb); return;
  }
}

void spikedBall_head_state0_hook(GB *gb) {
  BASE(spikedBall_head_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E; // Part.state
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xe4; // Part.collisionType
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(b_+7, objectSetVisible81_hook, SYM(objectSetVisible81), SYM(spikedBall_head_state1));
  spikedBall_head_state1_hook(gb); // falls through
}

void spikedBall_head_state1_hook(GB *gb) {
  BASE(spikedBall_head_state1);
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x1f);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); spikedBall_head_setDefaultDistanceAway_hook(gb); return; // jr
}

void spikedBall_head_state2_hook(GB *gb) {
  BASE(spikedBall_head_state2);
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x02);
  CYC(b_+5, b_+7); alu_and(gb, 0x1f);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  spikedBall_head_setDefaultDistanceAway_hook(gb); // falls through
}

void spikedBall_head_setDefaultDistanceAway_hook(GB *gb) {
  BASE(spikedBall_head_setDefaultDistanceAway);
  CYC(b_+0, b_+2); E = 0xf0; // Part.var30
  CYC(b_+2, b_+4); A = 0x0a;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  spikedBall_updatePosition_hook(gb); // falls through
}

void spikedBall_updatePosition_hook(GB *gb) {
  BASE(spikedBall_updatePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, spikedBall_copyParentPosition_hook, SYM(spikedBall_copyParentPosition), b_+3);
  CYC(b_+3, b_+5); E = 0xf0; // Part.var30
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); E = 0xc9; // Part.angle
  CYC(b_+8, b_+11); objectSetPositionInCircleArc_hook(gb); return; // jp
}

void spikedBall_head_state3_hook(GB *gb) {
  BASE(spikedBall_head_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, spikedBall_copyParentPosition_hook, SYM(spikedBall_copyParentPosition), b_+3);
  CYC(b_+3, b_+5); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+5, b_+7); mem_wr(gb, hFF8F, A);
  CYC(b_+7, b_+9); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+9, b_+11); mem_wr(gb, hFF8E, A);
  CYC(b_+11, b_+12); push_effect(gb, HL);
  CALL_C(b_+12, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+15);
  CYC(b_+15, b_+16); SET_BC(pop_effect(gb));
  CYC(b_+16, b_+18); alu_xor(gb, 0x10);
  CYC(b_+18, b_+19); E = A;
  CYC(b_+19, b_+21); alu_sub(gb, 0x06);
  CYC(b_+21, b_+23); alu_and(gb, 0x1f);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = 0xc9; // Part.angle
  CYC(b_+26, b_+27); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
  CYC(b_+28, b_+30); alu_and(gb, 0x1f);
  CYC(b_+30, b_+32); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+32, b_+34); spikedBall_head_state2_hook(gb); return; } // jr nc
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); A = E;
  CYC(b_+35, b_+37); alu_sub(gb, 0x03);
  CYC(b_+37, b_+39); alu_and(gb, 0x1f);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); L = 0xc4; // Part.state
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+43, b_+45); L = 0xf0; // Part.var30
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x0d);
  CYC(b_+47, b_+50); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_head_state4_hook(GB *gb) {
  BASE(spikedBall_head_state4);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E; // Part.state
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xc6; // Part.counter1
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x00);
  CYC(b_+7, b_+9); L = 0xc9; // Part.angle
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_add(gb, 0x03);
  CYC(b_+12, b_+14); alu_and(gb, 0x1f);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = 0xf0; // Part.var30
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x12);
  CYC(b_+19, b_+21); L = 0xd0; // Part.speed
  CYC(b_+21, b_+23); A = 0x40;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x03);
  CYC(b_+26, b_+29); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_head_state5_hook(GB *gb) {
  BASE(spikedBall_head_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, spikedBall_checkCollisionWithItem_hook, SYM(spikedBall_checkCollisionWithItem), b_+3);
  CALL_C(b_+3, spikedBall_head_updateDistanceFromOrigin_hook, SYM(spikedBall_head_updateDistanceFromOrigin), b_+6);
  CYC(b_+6, b_+9); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_chain_hook(GB *gb) {
  BASE(spikedBall_chain);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE); // Part.state (DE set by partCode2a)
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto chain_state1; } // jr nz
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CALL_C(b_+6, partSetAnimation_hook, SYM(partSetAnimation), b_+9);
  CALL_C(b_+9, objectSetVisible81_hook, SYM(objectSetVisible81), b_+12);

chain_state1:
  CYC(b_+12, b_+14); A = 0x01; // Object.id
  CALL_C(b_+14, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_cp(gb, 0x2a); // PART_SPIKED_BALL
  if (!(F & FZ)) { CYCT(b_+20, b_+23); partDelete_hook(gb); return; } // jp nz
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+25); L = 0xc9; // Part.angle
  CYC(b_+25, b_+26); E = L;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CALL_C(b_+28, spikedBall_chain_updateDistanceFromOrigin_hook, SYM(spikedBall_chain_updateDistanceFromOrigin), b_+31);
  CYC(b_+31, b_+33); L = 0xd7; // Part.relatedObj1+1
  CYC(b_+33, b_+34); B = mem_rd(gb, HL);
  CYC(b_+34, b_+37); spikedBall_updatePosition_hook(gb); return; // jp
}

void spikedBall_copyParentPosition_hook(GB *gb) {
  BASE(spikedBall_copyParentPosition);
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = 0x8b; // Enemy.yh
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_sub(gb, 0x05);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_sub(gb, 0x05);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); E = 0xcf; // Part.zh
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return; // ret
}

void spikedBall_checkCollisionWithItem_hook(GB *gb) {
  BASE(spikedBall_checkCollisionWithItem);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xea; // Part.var2a
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { RET_TAKEN(b_+9); return; } // ret z
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); L = 0xd1; // Part.speed+1
  CYC(b_+12, b_+14); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  RET(b_+18); return; // ret
}

void spikedBall_head_updateDistanceFromOrigin_hook(GB *gb) {
  BASE(spikedBall_head_updateDistanceFromOrigin);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = 0xf0; // Part.var30
  CYC(b_+3, b_+5); L = 0xd1; // Part.speed+1
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(b_+9, b_+11); goto fullyRetracted; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); L = alu_dec8(gb, L);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_sub(gb, 0x20);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_sbc(gb, 0x00);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  RET(b_+21); return; // ret

fullyRetracted:
  CYC(b_+22, b_+24); A = 0x06; // Object.counter1
  CALL_C(b_+24, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x00);
  RET(b_+29); return; // ret
}

void spikedBall_updateStateFromParent_hook(GB *gb) {
  BASE(spikedBall_updateStateFromParent);
  CYC(b_+0, b_+2); L = 0xb0; // Enemy.var30
  CYC(b_+2, b_+4); E = 0xc4; // Part.state
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+8, b_+10); goto L_58b5; } // jr c
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+11); return; } // ret z
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+15); return; } // ret z
  CYC(b_+15, b_+16);

L_58b5:
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, A);
  CYC(b_+18, b_+20); C = 0x01;
  if (F & FZ) { CYCT(b_+20, b_+22); goto L_58c0; } // jr z
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); C = alu_inc8(gb, C);
  CYC(b_+23, b_+24); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto L_58c0; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); C = alu_inc8(gb, C);

L_58c0:
  CYC(b_+27, b_+29); E = 0xc4; // Part.state
  CYC(b_+29, b_+30); A = C;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return; // ret
}

void spikedBall_chain_updateDistanceFromOrigin_hook(GB *gb) {
  BASE(spikedBall_chain_updateDistanceFromOrigin);
  CYC(b_+0, b_+2); L = 0xf0; // Part.var30
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+5); E = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  {
    CYC(b_+7, b_+8); push_effect(gb, b_+8);
    uint16_t target = spikedBall_jump_table(gb);
    if (target == b_+14) goto subid1;
    if (target == b_+27) goto subid2;
    goto subid3;
  }

subid1:
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, b_+16); E = L;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); A = alu_srl(gb, A);
  CYC(b_+19, b_+21); A = alu_srl(gb, A);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); alu_add(gb, A);
  CYC(b_+23, b_+24); alu_add(gb, B);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return; // ret

subid2:
  CYC(b_+27, b_+28); SET_HL(pop_effect(gb));
  CYC(b_+28, b_+29); E = L;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); A = alu_srl(gb, A);
  CYC(b_+32, b_+34); A = alu_srl(gb, A);
  CYC(b_+34, b_+35); alu_add(gb, A);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  RET(b_+36); return; // ret

subid3:
  CYC(b_+37, b_+38); SET_HL(pop_effect(gb));
  CYC(b_+38, b_+39); E = L;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); A = alu_srl(gb, A);
  CYC(b_+42, b_+44); A = alu_srl(gb, A);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  RET(b_+45); return; // ret
}
