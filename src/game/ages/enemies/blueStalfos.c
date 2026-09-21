#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb);
void ecom_incState_b0f_hook(GB *gb);
void ecom_setSpeedAndState8_b0f_hook(GB *gb);
void ecom_spawnUncountedEnemyWithSubid01_b0f_hook(GB *gb);
void ecom_decCounter1_b0f_hook(GB *gb);
void ecom_decCounter2_b0f_hook(GB *gb);
void ecom_readPositionVars_b0f_hook(GB *gb);
void ecom_moveTowardPosition_b0f_hook(GB *gb);
void ecom_randomBitwiseAndBCE_b0f_hook(GB *gb);
void ecom_spawnProjectile_b0f_hook(GB *gb);
void ecom_setRandomAngle_b0f_hook(GB *gb);
void ecom_bounceOffWallsAndHoles_b0f_hook(GB *gb);
void ecom_flickerVisibility_b0f_hook(GB *gb);

void enemyBoss_dead_b0f_hook(GB *gb);
void enemyBoss_spawnShadow_b0f_hook(GB *gb);
void enemyBoss_initializeRoom_b0f_hook(GB *gb);

void enemyCode77_hook(GB *gb);
void blueStalfos_state_uninitialized_hook(GB *gb);
void blueStalfos_state_spawner_hook(GB *gb);
void blueStalfos_state_stub_hook(GB *gb);
void blueStalfos_subid1_hook(GB *gb);
void blueStalfos_main_state08_hook(GB *gb);
void blueStalfos_main_state09_hook(GB *gb);
void blueStalfos_main_state0a_hook(GB *gb);
void blueStalfos_main_state0b_hook(GB *gb);
void blueStalfos_main_animate_hook(GB *gb);
void blueStalfos_main_state0c_hook(GB *gb);
void blueStalfos_main_state0d_hook(GB *gb);
void blueStalfos_main_state0e_hook(GB *gb);
void blueStalfos_main_state0f_hook(GB *gb);
void blueStalfos_main_finishedAttack_hook(GB *gb);
void blueStalfos_main_state10_hook(GB *gb);
void blueStalfos_main_state11_hook(GB *gb);
void blueStalfos_main_state12_hook(GB *gb);
void blueStalfos_main_state13_hook(GB *gb);
void blueStalfos_main_beginSickleAttack_hook(GB *gb);
void blueStalfos_main_state14_hook(GB *gb);
void blueStalfos_main_state15_hook(GB *gb);
void blueStalfos_main_state16_hook(GB *gb);
void blueStalfos_main_state17_hook(GB *gb);
void blueStalfos_subid2_hook(GB *gb);
void blueStalfos_initSubid2Or3_hook(GB *gb);
void blueStalfos_subid3_hook(GB *gb);
void blueStalfos_main_decideNextPosition_hook(GB *gb);
void blueStalfos_main_moveToQuadrant_hook(GB *gb);
void blueStalfos_main_moveToQuadrant_getLinkQuadrant_hook(GB *gb);
void blueStalfos_main_accelerate_hook(GB *gb);
void blueStalfos_afterImage_resetPositionVars_hook(GB *gb);
void blueStalfos_createPuff_hook(GB *gb);

static uint16_t blueStalfos_jump_table(GB *gb) {
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

static void blueStalfos_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_BLUE_STALFOS
// ==================================================================================================
void enemyCode77_hook(GB *gb) {
  BASE(enemyCode77);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto normalStatus; } // jr nz
  CYC(b_+9, b_+11);

normalStatus:
  CALL_C(b_+11, ecom_getSubidAndCpStateTo08_b0f_hook, SYM(ecom_getSubidAndCpStateTo08_b0f), b_+14);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto normalState; } // jr nc
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  {
    uint16_t target = blueStalfos_jump_table(gb);
    if (target == SYM(blueStalfos_state_uninitialized)) { blueStalfos_state_uninitialized_hook(gb); return; }
    if (target == SYM(blueStalfos_state_spawner)) { blueStalfos_state_spawner_hook(gb); return; }
    if (target == SYM(blueStalfos_state_stub)) { blueStalfos_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+33, b_+34); B = alu_dec8(gb, B);
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+36); push_effect(gb, b_+36);
  {
    uint16_t target = blueStalfos_jump_table(gb);
    if (target == SYM(blueStalfos_subid1)) { blueStalfos_subid1_hook(gb); return; }
    if (target == SYM(blueStalfos_subid2)) { blueStalfos_subid2_hook(gb); return; }
    if (target == SYM(blueStalfos_subid3)) { blueStalfos_subid3_hook(gb); return; }
    HANDOFF(target);
  }
}

void blueStalfos_state_uninitialized_hook(GB *gb) {
  BASE(blueStalfos_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); alu_sub(gb, 0x02);
  if (F & FC) { CALL_C_CC(b_+3, objectSetVisible82_hook, SYM(objectSetVisible82), b_+6); } else { CYC(b_+3, b_+6); } // call c
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CALL_C(b_+11, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xff);
  CYC(b_+18, b_+20); A = 0x77; // ENEMY_BLUE_STALFOS
  CYC(b_+20, b_+23); TAIL(enemyBoss_initializeRoom_b0f); // jp
}

void blueStalfos_state_spawner_hook(GB *gb) {
  BASE(blueStalfos_state_spawner);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x03;
  CALL_C(b_+2, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x77; // ENEMY_BLUE_STALFOS
  CALL_C(b_+8, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+11);
  CALL_C(b_+11, objectCopyPosition_hook, SYM(objectCopyPosition), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+16, b_+17); E = L;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+20); C = H;
  CALL_C(b_+20, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 2
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+26, b_+28); A = ENEMY_BASE; // Enemy.start
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+29, b_+30); mem_wr(gb, HL, C);
  CALL_C(b_+30, objectCopyPosition_hook, SYM(objectCopyPosition), b_+33);
  CALL_C(b_+33, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x03); // [subid] = 3
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+40, b_+42); A = ENEMY_BASE; // Enemy.start
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+43, b_+44); mem_wr(gb, HL, C);
  CALL_C(b_+44, objectCopyPosition_hook, SYM(objectCopyPosition), b_+47);
  CYC(b_+47, b_+50); TAIL(enemyDelete); // jp
}

void blueStalfos_state_stub_hook(GB *gb) {
  BASE(blueStalfos_state_stub);
  RET(b_+0); return;
}

void blueStalfos_subid1_hook(GB *gb) {
  BASE(blueStalfos_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = blueStalfos_jump_table(gb);
    if (target == SYM(blueStalfos_main_state08)) { blueStalfos_main_state08_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state09)) { blueStalfos_main_state09_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state0a)) { blueStalfos_main_state0a_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state0b)) { blueStalfos_main_state0b_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state0c)) { blueStalfos_main_state0c_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state0d)) { blueStalfos_main_state0d_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state0e)) { blueStalfos_main_state0e_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state0f)) { blueStalfos_main_state0f_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state10)) { blueStalfos_main_state10_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state11)) { blueStalfos_main_state11_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state12)) { blueStalfos_main_state12_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state13)) { blueStalfos_main_state13_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state14)) { blueStalfos_main_state14_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state15)) { blueStalfos_main_state15_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state16)) { blueStalfos_main_state16_hook(gb); return; }
    if (target == SYM(blueStalfos_main_state17)) { blueStalfos_main_state17_hook(gb); return; }
    HANDOFF(target);
  }
}

void blueStalfos_main_state08_hook(GB *gb) {
  BASE(blueStalfos_main_state08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x010b);
  CALL_C(b_+3, enemyBoss_spawnShadow_b0f_hook, SYM(enemyBoss_spawnShadow_b0f), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  CALL_C(b_+7, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  RET(b_+18); return;
}

// Moving down before fight starts
void blueStalfos_main_state09_hook(GB *gb) {
  BASE(blueStalfos_main_state09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x58);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(b_+8, b_+10);
  CALL_C(b_+10, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x40); // 64
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+21, b_+23); A = 0x2d; // MUS_MINIBOSS
  CYC(b_+23, b_+26); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CYC(b_+29, b_+31); E = 0x0f;
  CYC(b_+31, b_+34); SET_BC(0x3030);
  CALL_C(b_+34, ecom_randomBitwiseAndBCE_b0f_hook, SYM(ecom_randomBitwiseAndBCE_b0f), b_+37);
  CYC(b_+37, b_+38); A = E;
  CYC(b_+38, b_+41); TAIL(blueStalfos_main_moveToQuadrant); // jp
}

// Moving to position in var30/var31
void blueStalfos_main_state0a_hook(GB *gb) {
  BASE(blueStalfos_main_state0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(b_+3, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x04);
  CYC(b_+9, b_+11); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto moveToPosition; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = mem_rd(gb, hFF8F);
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CYC(b_+16, b_+18); alu_add(gb, 0x04);
  CYC(b_+18, b_+20); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto moveToPosition; } // jr nc
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x0b);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x10); // 16
  CYCT(b_+31, b_+33); TAIL(blueStalfos_main_animate); // jr

moveToPosition:
  CALL_C(b_+33, blueStalfos_main_accelerate_hook, SYM(blueStalfos_main_accelerate), b_+36);
  CALL_C(b_+36, ecom_moveTowardPosition_b0f_hook, SYM(ecom_moveTowardPosition_b0f), b_+39);
  CYCT(b_+39, b_+41); TAIL(blueStalfos_main_animate); // jr
}

// Reached position, standing still for [counter1] frames
void blueStalfos_main_state0b_hook(GB *gb) {
  BASE(blueStalfos_main_state0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  TAIL(blueStalfos_main_animate); // fallthrough
}

void blueStalfos_main_animate_hook(GB *gb) {
  BASE(blueStalfos_main_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// Decide which attack to do
void blueStalfos_main_state0c_hook(GB *gb) {
  BASE(blueStalfos_main_state0c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x10);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_add(gb, 0x04);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+17); alu_add(gb, 0x14);
  CYC(b_+17, b_+19); alu_cp(gb, 0x29);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto projectileAttack; } // jr nc
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+23, b_+24); alu_sub(gb, C);
  CYC(b_+24, b_+26); alu_add(gb, 0x12);
  CYC(b_+26, b_+28); alu_cp(gb, 0x25);
  if (F & FC) { CYCT(b_+28, b_+31); blueStalfos_main_beginSickleAttack_hook(gb); return; } // jp c
  CYC(b_+28, b_+31);

projectileAttack:
  CYC(b_+31, b_+33); B = 0x3d; // PART_BLUE_STALFOS_PROJECTILE
  CALL_C(b_+33, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x78); // 120
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x0e);
  CYC(b_+46, b_+48); A = 0x02;
  CYC(b_+48, b_+51); TAIL(enemySetAnimation); // jp
}

// Sickle attack
void blueStalfos_main_state0d_hook(GB *gb) {
  BASE(blueStalfos_main_state0d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); blueStalfos_main_finishedAttack_hook(gb); return; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x08;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A); // [animParameter]
  CYC(b_+14, b_+16); A = 0x6b; // SND_SWORDSPIN
  CYC(b_+16, b_+19); TAIL(playSound_b00); // jp
}

// Charging a projectile
void blueStalfos_main_state0e_hook(GB *gb) {
  BASE(blueStalfos_main_state0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+9, b_+11); A = 0x03;
  CYC(b_+11, b_+14); TAIL(enemySetAnimation); // jp
}

// Just fired projectile
void blueStalfos_main_state0f_hook(GB *gb) {
  BASE(blueStalfos_main_state0f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  TAIL(blueStalfos_main_finishedAttack); // fallthrough
}

void blueStalfos_main_finishedAttack_hook(GB *gb) {
  BASE(blueStalfos_main_finishedAttack);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0a);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x40); // 64
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CALL_C(b_+14, enemySetAnimation_hook, SYM(enemySetAnimation), b_+17);
  CYC(b_+17, b_+20); TAIL(blueStalfos_main_decideNextPosition); // jp
}

// Link just turned into a baby; about to turn transparent and warp to top of room
void blueStalfos_main_state10_hook(GB *gb) {
  BASE(blueStalfos_main_state10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(b_+13); return;
}

// Now transparent; waiting for [counter1] frames before warping
void blueStalfos_main_state11_hook(GB *gb) {
  BASE(blueStalfos_main_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0c);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+20, b_+22); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CALL_C(b_+24, enemySetAnimation_hook, SYM(enemySetAnimation), b_+27);
  CYC(b_+27, b_+30); TAIL(objectSetInvisible); // jp
}

// Just warped to top of room; standing in place
void blueStalfos_main_state12_hook(GB *gb) {
  BASE(blueStalfos_main_state12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x10);
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CALL_C(b_+12, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x03);
  CYC(b_+17, b_+20); SET_HL(b_+28);
  CYC(b_+20, b_+21); blueStalfos_addAToHl_from_rst(gb, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+32); TAIL(objectSetVisible82); // jp
}

// Moving down toward baby Link before attacking with sickle
void blueStalfos_main_state13_hook(GB *gb) {
  BASE(blueStalfos_main_state13);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(b_+8, b_+11); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+8, b_+11);
  TAIL(blueStalfos_main_beginSickleAttack); // fallthrough
}

void blueStalfos_main_beginSickleAttack_hook(GB *gb) {
  BASE(blueStalfos_main_beginSickleAttack);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x0d;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); TAIL(enemySetAnimation); // jp
}

// Just hit by PART_BLUE_STALFOS_PROJECTILE; turning into a small bat
void blueStalfos_main_state14_hook(GB *gb) {
  BASE(blueStalfos_main_state14);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, blueStalfos_createPuff_hook, SYM(blueStalfos_createPuff), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x02);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x06); // [collisionRadiusX]
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xf0);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x00); // [counter2]
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CALL_C(b_+25, objectSetInvisible_hook, SYM(objectSetInvisible), b_+28);
  CYC(b_+28, b_+30); A = 0x85; // SND_SCENT_SEED
  CALL_C(b_+30, playSound_b00_hook, SYM(playSound_b00), b_+33);
  CYC(b_+33, b_+35); A = 0x04;
  CYC(b_+35, b_+38); TAIL(enemySetAnimation); // jp
}

// Transforming into bat
void blueStalfos_main_state15_hook(GB *gb) {
  BASE(blueStalfos_main_state15);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ANIM_PARAMETER;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CALL_C(b_+8, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x65); // ENEMYCOLLISION_BLUE_STALFOS_BAT
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  CYC(b_+19, b_+22); TAIL(objectSetVisiblec2); // jp
}

// Flying around as a bat
void blueStalfos_main_state16_hook(GB *gb) {
  BASE(blueStalfos_main_state16);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto flyAround; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CALL_C(b_+6, blueStalfos_createPuff_hook, SYM(blueStalfos_createPuff), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x48); // ENEMYCOLLISION_BLUE_STALFOS
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0xff);
  CYC(b_+21, b_+24); TAIL(objectSetInvisible); // jp

flyAround:
  CALL_C(b_+24, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+27);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto L_5fce; } // jr nz
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x1e); // [counter2] = 30
  CALL_C(b_+31, ecom_setRandomAngle_b0f_hook, SYM(ecom_setRandomAngle_b0f), b_+34);

L_5fce:
  CALL_C(b_+34, ecom_bounceOffWallsAndHoles_b0f_hook, SYM(ecom_bounceOffWallsAndHoles_b0f), b_+37);
  CALL_C(b_+37, objectApplySpeed_hook, SYM(objectApplySpeed), b_+40);
  CYC(b_+40, b_+43); TAIL(enemyAnimate); // jp
}

// Transforming back into stalfos
void blueStalfos_main_state17_hook(GB *gb) {
  BASE(blueStalfos_main_state17);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ANIM_PARAMETER;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+10, b_+12); A = 0x08;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, blueStalfos_main_finishedAttack_hook, SYM(blueStalfos_main_finishedAttack), b_+18);
  CYC(b_+18, b_+20); A = 0x85; // SND_SCENT_SEED
  CALL_C(b_+20, playSound_b00_hook, SYM(playSound_b00), b_+23);
  CYC(b_+23, b_+26); TAIL(objectSetVisible82); // jp
}

// Hitbox for the sickle (invisible)
void blueStalfos_subid2_hook(GB *gb) {
  BASE(blueStalfos_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+5); blueStalfos_initSubid2Or3_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+7); A = OBJ_ID;
  CALL_C(b_+7, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x77); // ENEMY_BLUE_STALFOS
  if (!(F & FZ)) { CYCT(b_+13, b_+16); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+18, b_+19); E = L;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto L_600f; } // jr nz
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); A = 0x0c;

L_600f:
  CYC(b_+30, b_+33); SET_BC(b_+66);
  CALL_C(b_+33, addAToBc_hook, 0x006d, b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+38, b_+39); E = L;
  CYC(b_+39, b_+40); A = mem_rd(gb, BC);
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); SET_BC(BC + 1);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+45, b_+46); E = L;
  CYC(b_+46, b_+47); A = mem_rd(gb, BC);
  CYC(b_+47, b_+48); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); SET_BC(BC + 1);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+52, b_+53); A = mem_rd(gb, BC);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); SET_BC(BC + 1);
  CYC(b_+55, b_+56); E = alu_inc8(gb, E);
  CYC(b_+56, b_+57); A = mem_rd(gb, BC);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; } // ret nz
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+63, b_+65); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(b_+65); return;
}

void blueStalfos_initSubid2Or3_hook(GB *gb) {
  BASE(blueStalfos_initSubid2Or3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x66); // ENEMYCOLLISION_BLUE_STALFOS_SICKLE
  CYC(b_+7, b_+9); A = OBJ_ZH;
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+13); E = L;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}

// "Afterimage" of blue stalfos visible while moving
void blueStalfos_subid3_hook(GB *gb) {
  BASE(blueStalfos_subid3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state8; } // jr z
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+7); A = OBJ_ID;
  CALL_C(b_+7, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x77); // ENEMY_BLUE_STALFOS
  if (!(F & FZ)) { CYCT(b_+13, b_+16); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0x12);
  if (F & FZ) { CYCT(b_+21, b_+24); blueStalfos_afterImage_resetPositionVars_hook(gb); return; } // jp z
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); alu_cp(gb, 0x14);
  if (F & FC) { CALL_C_CC(b_+26, objectSetVisible82_hook, SYM(objectSetVisible82), b_+29); } else { CYC(b_+26, b_+29); } // call nc
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+31, b_+33); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); B = A;
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+37); alu_sub(gb, B);
  CYC(b_+37, b_+39); alu_add(gb, 0x08);
  CYC(b_+39, b_+41); alu_and(gb, 0x0f);
  CYC(b_+41, b_+43); A = alu_swap(gb, A);
  CYC(b_+43, b_+44); C = A;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+48); E = alu_inc8(gb, E);
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); B = A;
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); alu_sub(gb, B);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+56); alu_and(gb, 0x0f);
  CYC(b_+56, b_+57); alu_or(gb, C);
  CYC(b_+57, b_+58); C = A;
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+62); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+65); alu_add(gb, ENEMY_BASE + 0x33); // Enemy.var33
  CYC(b_+65, b_+66); E = A;
  CYC(b_+66, b_+67); A = C;
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  CYC(b_+68, b_+70); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  CYC(b_+71, b_+72); A = alu_inc8(gb, A);
  CYC(b_+72, b_+74); alu_and(gb, 0x07);
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+77); alu_add(gb, ENEMY_BASE + 0x33); // Enemy.var33
  CYC(b_+77, b_+78); E = A;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+81); alu_cp(gb, 0x88);
  CYC(b_+81, b_+82); B = A;
  if (F & FZ) { CYCT(b_+82, b_+85); objectSetInvisible_hook(gb); return; } // jp z
  CYC(b_+82, b_+85);
  CYC(b_+85, b_+86); H = D;
  CYC(b_+86, b_+88); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+88, b_+90); alu_and(gb, 0xf0);
  CYC(b_+90, b_+92); A = alu_swap(gb, A);
  CYC(b_+92, b_+94); alu_sub(gb, 0x08);
  CYC(b_+94, b_+95); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+98); A = B;
  CYC(b_+98, b_+100); alu_and(gb, 0x0f);
  CYC(b_+100, b_+102); alu_sub(gb, 0x08);
  CYC(b_+102, b_+103); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+103, b_+104); mem_wr(gb, HL, A);
  CYC(b_+104, b_+107); TAIL(ecom_flickerVisibility_b0f); // jp

state8:
  CALL_C(b_+107, blueStalfos_initSubid2Or3_hook, SYM(blueStalfos_initSubid2Or3), b_+110);
  CALL_C(b_+110, blueStalfos_afterImage_resetPositionVars_hook, SYM(blueStalfos_afterImage_resetPositionVars), b_+113);
  CYC(b_+113, b_+115); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+115, b_+117); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+117, objectSetVisible83_hook, SYM(objectSetVisible83), b_+120);
  CYC(b_+120, b_+123); TAIL(objectSetInvisible); // jp
}

// Decides the next position for the blue stalfos. It will always choose a different
// quadrant of the screen from the one it's in already.
void blueStalfos_main_decideNextPosition_hook(GB *gb) {
  BASE(blueStalfos_main_decideNextPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+5); SET_BC(0x3030);
  CALL_C(b_+5, ecom_randomBitwiseAndBCE_b0f_hook, SYM(ecom_randomBitwiseAndBCE_b0f), b_+8);
  CYC(b_+8, b_+9); H = E;
  CYC(b_+9, b_+11); L = 0x00;
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x58);
  if (F & FC) { CYCT(b_+16, b_+18); goto L_60f6; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); L = 0x02;

L_60f6:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_cp(gb, 0x78);
  if (F & FC) { CYCT(b_+25, b_+27); goto L_60fe; } // jr c
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);

L_60fe:
  CYC(b_+28, b_+29); A = L;
  CYC(b_+29, b_+30); alu_add(gb, A);
  CYC(b_+30, b_+31); alu_add(gb, A);
  CYC(b_+31, b_+32); alu_add(gb, H);
  TAIL(blueStalfos_main_moveToQuadrant); // fallthrough
}

// @param a Position index to use
// @param bc Offset to be added to target position
void blueStalfos_main_moveToQuadrant_hook(GB *gb) {
  BASE(blueStalfos_main_moveToQuadrant);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(b_+57);
  CYC(b_+3, b_+4); blueStalfos_addAToHl_from_rst(gb, b_+4);
  CALL_L(b_+4, blueStalfos_main_moveToQuadrant_getLinkQuadrant_hook, b_+7);
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+8, b_+10); goto moveToLinksPosition; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+14); SET_HL(b_+73);
  CYC(b_+14, b_+15); blueStalfos_addAToHl_from_rst(gb, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+18, b_+19); alu_add(gb, B);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); E = alu_inc8(gb, E);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); alu_add(gb, C);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return;

moveToLinksPosition:
  CYC(b_+25, b_+27); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+27, b_+29); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+29, b_+31); alu_sub(gb, 0x14);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); E = alu_inc8(gb, E);
  CYC(b_+33, b_+35); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  RET(b_+36); return;
}

// @param[out] a The quadrant of the screen Link is in.
// (0/2/4/6 for up/left, up/right, down/left, down/right)
// Bare local (@getLinkQuadrant), not previously hooked; promoted to a full routine here since it's
// reached via a genuine call/ret from blueStalfos_main_moveToQuadrant.
void blueStalfos_main_moveToQuadrant_getLinkQuadrant_hook(GB *gb) {
  BASE(blueStalfos_main_moveToQuadrant);
  CYC(b_+37, b_+39); E = 0x00;
  CYC(b_+39, b_+41); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+41, b_+43); alu_cp(gb, 0x58); // (LARGE_ROOM_HEIGHT<<4)/2
  if (F & FC) { CYCT(b_+43, b_+45); goto skipY; } // jr c
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); E = 0x02;

skipY:
  CYC(b_+47, b_+49); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+49, b_+51); alu_cp(gb, 0x78); // (LARGE_ROOM_WIDTH<<4)/2
  if (F & FC) { CYCT(b_+51, b_+53); goto skipX; } // jr c
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+54); E = alu_inc8(gb, E);

skipX:
  CYC(b_+54, b_+55); A = E;
  CYC(b_+55, b_+56); alu_add(gb, A);
  RET(b_+56); return;
}

void blueStalfos_main_accelerate_hook(GB *gb) {
  BASE(blueStalfos_main_accelerate);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_add(gb, 0x05);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return;
}

void blueStalfos_afterImage_resetPositionVars_hook(GB *gb) {
  BASE(blueStalfos_afterImage_resetPositionVars);
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); E = L;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); E = L;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+20, b_+22); A = 0x88;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  RET(b_+30); return;
}

void blueStalfos_createPuff_hook(GB *gb) {
  BASE(blueStalfos_createPuff);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x0502); // INTERAC_PUFF, subid 2
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x40); // Interaction.start
  RET(b_+14); return;
}
