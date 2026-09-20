#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode56_body), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode56_body), (from), (to), true)

// object_code/ages/enemies/kingMoblinMinionMain.s, bank $10.

static void kingMoblinMinion_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void kingMoblinMinion_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t kingMoblinMinion_jump_table(GB *gb) {
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

void enemyCode56_body_hook(GB *gb) {
  BASE(enemyCode56_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (kingMoblinMinion_jump_table(gb));
    if (jt_ == SYM(kingMoblinMinion_state0)) { kingMoblinMinion_state0_hook(gb); return; }
    else if (jt_ == SYM(enemyAnimate)) { enemyAnimate_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state2)) { kingMoblinMinion_state2_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state3)) { kingMoblinMinion_state3_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state4)) { kingMoblinMinion_state4_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state5)) { kingMoblinMinion_state5_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state6)) { kingMoblinMinion_state6_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state7)) { kingMoblinMinion_state7_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state8)) { kingMoblinMinion_state8_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_state9)) { kingMoblinMinion_state9_hook(gb); return; }
    else if (jt_ == SYM(kingMoblinMinion_stateA)) { kingMoblinMinion_stateA_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

// $6ddd is a private table of 4 pointers (left/top/right/bottom flame coordinate data), not code;
// it is only reached through the RST $18 index below, so it needs no hook entry of its own.
void blackTower_getMovingFlamesNextTileCoords_hook(GB *gb) {
  BASE(blackTower_getMovingFlamesNextTileCoords);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+33);
  CYC(b_+6, b_+7);
  kingMoblinMinion_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); E = 0xc7;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14);
  kingMoblinMinion_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+18); alu_and(gb, 0xf0);
  CYC(b_+18, b_+20); alu_add(gb, 0x08);
  CYC(b_+20, b_+22); E = 0xf0;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+27); alu_and(gb, 0x0f);
  CYC(b_+27, b_+29); A = alu_swap(gb, A);
  CYC(b_+29, b_+31); alu_add(gb, 0x08);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  RET(b_+32); return;
}

// $6ccf is a private 4-byte-per-subid data table (counter1, direction, yh, xh), not code; it is
// only reached through the RST $18 index below, so it needs no hook entry of its own.
void kingMoblinMinion_state0_hook(GB *gb) {
  BASE(kingMoblinMinion_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x50);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+14); SET_HL(b_+39);
  CYC(b_+14, b_+15);
  kingMoblinMinion_addDoubleIndexToHl_from_rst(gb, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); A = 0x02;
  CALL_C(b_+33, enemySetAnimation_hook, SYM(enemySetAnimation), b_+36);
  CYC(b_+36, b_+39);
  objectSetVisiblec2_hook(gb);
}

void kingMoblinMinion_state2_hook(GB *gb) {
  BASE(kingMoblinMinion_state2);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0c);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, SYM(kingMoblinMinion_state3));
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_state3_hook(GB *gb) {
  BASE(kingMoblinMinion_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    kingMoblinMinion_animate_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); B = 0x47;
  CALL_C(b_+7, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+14);
  CYC(b_+14, b_+16); A = 0x02;
  CYC(b_+16, SYM(kingMoblinMinion_state4));
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_state4_hook(GB *gb) {
  BASE(kingMoblinMinion_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  CYC(b_+3, b_+4); L = E;
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto jump;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wScreenShakeCounterY);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+10, b_+12);
    kingMoblinMinion_animate_hook(gb);
    return;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x07);
  CYC(b_+14, b_+16);
  kingMoblinMinion_animate_hook(gb);
  return;
jump:
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+19, b_+21); A = 0x80;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, SYM(kingMoblinMinion_animate)); mem_wr(gb, HL, 0xfe);
  kingMoblinMinion_animate_hook(gb);
}

void kingMoblinMinion_animate_hook(GB *gb) {
  BASE(kingMoblinMinion_animate);
  CYC(b_+0, SYM(kingMoblinMinion_state5));
  enemyAnimate_hook(gb);
}

void kingMoblinMinion_state5_hook(GB *gb) {
  BASE(kingMoblinMinion_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto landed;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+8, b_+9); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+13);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); A = 0x04;
  CALL_C(b_+16, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+22, b_+23); mem_wr(gb, HL, B);
  RET(b_+23); return;
landed:
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x10);
  CYC(b_+31, SYM(kingMoblinMinion_state6));
  kingMoblinMinion_animate_hook(gb);
}

void kingMoblinMinion_state6_hook(GB *gb) {
  BASE(kingMoblinMinion_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    kingMoblinMinion_animate_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0xc8);
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x02);
  CYC(b_+10, SYM(kingMoblinMinion_state7));
  kingMoblinMinion_animate_hook(gb);
}

// $6d6e is a private per-subid data table (bomb-throw angle pairs), not code; it is only read
// through the RST $10 index below, so it needs no hook entry of its own.
void kingMoblinMinion_state7_hook(GB *gb) {
  BASE(kingMoblinMinion_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x18);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);
  CYC(b_+12, b_+13); C = A;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+17); SET_HL(b_+47);
  CYC(b_+17, b_+18);
  kingMoblinMinion_addAToHl_from_rst(gb, b_+18);
  CYC(b_+18, b_+19); B = mem_rd(gb, HL);
  CYC(b_+19, b_+21); A = 0x04;
  CALL_C(b_+21, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+24);
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+28); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+30); mem_wr(gb, HL, B);
  CYC(b_+30, b_+32); L = PART_BASE + OBJ_SPEED;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x37);
  CYC(b_+34, b_+36); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+36, b_+37); A = 0x00;
  CYC(b_+37, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0xff);
  CYC(b_+41, b_+43); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+43, b_+44); mem_wr(gb, HL, 0x81);
  CYC(b_+44, b_+45); A = C;
  CYC(b_+45, b_+48);
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_state8_hook(GB *gb) {
  BASE(kingMoblinMinion_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+8, b_+10); A = 0xc0;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xfe);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+17); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+17, b_+19); C = 0xf4;
  if (F & FZ) {
    CYCT(b_+19, b_+21);
    goto exclaim;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); C = 0x0c;
exclaim:
  CYC(b_+23, b_+25); B = 0xf8;
  CYC(b_+25, b_+27); A = 0x1e;
  CALL_C(b_+27, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), SYM(kingMoblinMinion_state9));
  kingMoblinMinion_state9_hook(gb);
}

void kingMoblinMinion_state9_hook(GB *gb) {
  BASE(kingMoblinMinion_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x0c);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x08);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, SYM(kingMoblinMinion_stateA));
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_stateA_hook(GB *gb) {
  BASE(kingMoblinMinion_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    goto animate;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    goto delete_;
  }
  CYC(b_+8, b_+10);
  CALL_C(b_+10, objectApplySpeed_hook, SYM(objectApplySpeed), b_+13);
animate:
  CYC(b_+13, b_+16);
  enemyAnimate_hook(gb);
  return;
delete_:
  CYC(b_+16, b_+18); A = 0x33;
  CALL_C(b_+18, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x01);
  CYC(b_+23, SYM(blackTower_getMovingFlamesNextTileCoords));
  enemyDelete_hook(gb);
}
