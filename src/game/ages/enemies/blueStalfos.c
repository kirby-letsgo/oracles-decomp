#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5d9c, 0x5d9e); goto normalStatus; } // jr z
  CYC(0x5d9c, 0x5d9e);
  CYC(0x5d9e, 0x5da0); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5da0); return; } // ret c
  CYC(0x5da0, 0x5da1);
  if (F & FZ) { CYCT(0x5da1, 0x5da4); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(0x5da1, 0x5da4);
  CYC(0x5da4, 0x5da5); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5da5, 0x5da7); goto normalStatus; } // jr nz
  CYC(0x5da5, 0x5da7);

normalStatus:
  CALL_C(0x5da7, ecom_getSubidAndCpStateTo08_b0f_hook, 0x4426, 0x5daa);
  if (!(F & FC)) { CYCT(0x5daa, 0x5dac); goto normalState; } // jr nc
  CYC(0x5daa, 0x5dac);
  CYC(0x5dac, 0x5dad); push_effect(gb, 0x5dad);
  {
    uint16_t target = blueStalfos_jump_table(gb);
    if (target == 0x5dc6) { blueStalfos_state_uninitialized_hook(gb); return; }
    if (target == 0x5ddd) { blueStalfos_state_spawner_hook(gb); return; }
    if (target == 0x5e0f) { blueStalfos_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x5dbd, 0x5dbe); B = alu_dec8(gb, B);
  CYC(0x5dbe, 0x5dbf); A = B;
  CYC(0x5dbf, 0x5dc0); push_effect(gb, 0x5dc0);
  {
    uint16_t target = blueStalfos_jump_table(gb);
    if (target == 0x5e10) { blueStalfos_subid1_hook(gb); return; }
    if (target == 0x5ff1) { blueStalfos_subid2_hook(gb); return; }
    if (target == 0x6067) { blueStalfos_subid3_hook(gb); return; }
    HANDOFF(target);
  }
}

void blueStalfos_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5dc6, 0x5dc7); A = B;
  CYC(0x5dc7, 0x5dc9); alu_sub(gb, 0x02);
  if (F & FC) { CALL_C_CC(0x5dc9, objectSetVisible82_hook, 0x1e69, 0x5dcc); } else { CYC(0x5dc9, 0x5dcc); } // call c
  CYC(0x5dcc, 0x5dcd); A = B;
  CYC(0x5dcd, 0x5dce); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5dce, 0x5dd1); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(0x5dce, 0x5dd1);
  CALL_C(0x5dd1, ecom_incState_b0f_hook, 0x4000, 0x5dd4);
  CYC(0x5dd4, 0x5dd6); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5dd6, 0x5dd8); mem_wr(gb, HL, 0xff);
  CYC(0x5dd8, 0x5dda); A = 0x77; // ENEMY_BLUE_STALFOS
  CYC(0x5dda, 0x5ddd); enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

void blueStalfos_state_spawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ddd, 0x5ddf); B = 0x03;
  CALL_C(0x5ddf, checkBEnemySlotsAvailable_hook, 0x20f7, 0x5de2);
  if (!(F & FZ)) { RET_TAKEN(0x5de2); return; } // ret nz
  CYC(0x5de2, 0x5de3);
  CYC(0x5de3, 0x5de5); B = 0x77; // ENEMY_BLUE_STALFOS
  CALL_C(0x5de5, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x5de8);
  CALL_C(0x5de8, objectCopyPosition_hook, 0x2242, 0x5deb);
  CYC(0x5deb, 0x5ded); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x5ded, 0x5dee); E = L;
  CYC(0x5dee, 0x5def); A = mem_rd(gb, DE);
  CYC(0x5def, 0x5df0); mem_wr(gb, HL, A);
  CYC(0x5df0, 0x5df1); C = H;
  CALL_C(0x5df1, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x5df4);
  CYC(0x5df4, 0x5df5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 2
  CYC(0x5df5, 0x5df7); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5df7, 0x5df9); A = ENEMY_BASE; // Enemy.start
  CYC(0x5df9, 0x5dfa); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5dfa, 0x5dfb); mem_wr(gb, HL, C);
  CALL_C(0x5dfb, objectCopyPosition_hook, 0x2242, 0x5dfe);
  CALL_C(0x5dfe, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x5e01);
  CYC(0x5e01, 0x5e03); mem_wr(gb, HL, 0x03); // [subid] = 3
  CYC(0x5e03, 0x5e05); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5e05, 0x5e07); A = ENEMY_BASE; // Enemy.start
  CYC(0x5e07, 0x5e08); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5e08, 0x5e09); mem_wr(gb, HL, C);
  CALL_C(0x5e09, objectCopyPosition_hook, 0x2242, 0x5e0c);
  CYC(0x5e0c, 0x5e0f); enemyDelete_hook(gb); return; // jp
}

void blueStalfos_state_stub_hook(GB *gb) {
  RET(0x5e0f); return;
}

void blueStalfos_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e10, 0x5e11); A = mem_rd(gb, DE);
  CYC(0x5e11, 0x5e13); alu_sub(gb, 0x08);
  CYC(0x5e13, 0x5e14); push_effect(gb, 0x5e14);
  {
    uint16_t target = blueStalfos_jump_table(gb);
    if (target == 0x5e34) { blueStalfos_main_state08_hook(gb); return; }
    if (target == 0x5e47) { blueStalfos_main_state09_hook(gb); return; }
    if (target == 0x5e70) { blueStalfos_main_state0a_hook(gb); return; }
    if (target == 0x5e99) { blueStalfos_main_state0b_hook(gb); return; }
    if (target == 0x5ea3) { blueStalfos_main_state0c_hook(gb); return; }
    if (target == 0x5ed6) { blueStalfos_main_state0d_hook(gb); return; }
    if (target == 0x5ee9) { blueStalfos_main_state0e_hook(gb); return; }
    if (target == 0x5ef7) { blueStalfos_main_state0f_hook(gb); return; }
    if (target == 0x5f0f) { blueStalfos_main_state10_hook(gb); return; }
    if (target == 0x5f1d) { blueStalfos_main_state11_hook(gb); return; }
    if (target == 0x5f3b) { blueStalfos_main_state12_hook(gb); return; }
    if (target == 0x5f5b) { blueStalfos_main_state13_hook(gb); return; }
    if (target == 0x5f70) { blueStalfos_main_state14_hook(gb); return; }
    if (target == 0x5f96) { blueStalfos_main_state15_hook(gb); return; }
    if (target == 0x5fac) { blueStalfos_main_state16_hook(gb); return; }
    if (target == 0x5fd7) { blueStalfos_main_state17_hook(gb); return; }
    HANDOFF(target);
  }
}

void blueStalfos_main_state08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e34, 0x5e37); SET_BC(0x010b);
  CALL_C(0x5e37, enemyBoss_spawnShadow_b0f_hook, 0x4534, 0x5e3a);
  if (!(F & FZ)) { RET_TAKEN(0x5e3a); return; } // ret nz
  CYC(0x5e3a, 0x5e3b);
  CALL_C(0x5e3b, ecom_incState_b0f_hook, 0x4000, 0x5e3e);
  CYC(0x5e3e, 0x5e40); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5e40, 0x5e42); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x5e42, 0x5e44); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5e44, 0x5e46); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  RET(0x5e46); return;
}

// Moving down before fight starts
void blueStalfos_main_state09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e47, objectApplySpeed_hook, 0x201d, 0x5e4a);
  CYC(0x5e4a, 0x5e4c); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5e4c, 0x5e4d); A = mem_rd(gb, DE);
  CYC(0x5e4d, 0x5e4f); alu_cp(gb, 0x58);
  if (!(F & FZ)) { CYCT(0x5e4f, 0x5e51); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(0x5e4f, 0x5e51);
  CALL_C(0x5e51, ecom_incState_b0f_hook, 0x4000, 0x5e54);
  CYC(0x5e54, 0x5e56); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5e56, 0x5e58); mem_wr(gb, HL, 0x40); // 64
  CYC(0x5e58, 0x5e5a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5e5a, 0x5e5c); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x5e5c, 0x5e5e); A = 0x2d; // MUS_MINIBOSS
  CYC(0x5e5e, 0x5e61); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x5e61, playSound_b00_hook, 0x0c98, 0x5e64);
  CYC(0x5e64, 0x5e66); E = 0x0f;
  CYC(0x5e66, 0x5e69); SET_BC(0x3030);
  CALL_C(0x5e69, ecom_randomBitwiseAndBCE_b0f_hook, 0x434f, 0x5e6c);
  CYC(0x5e6c, 0x5e6d); A = E;
  CYC(0x5e6d, 0x5e70); blueStalfos_main_moveToQuadrant_hook(gb); return; // jp
}

// Moving to position in var30/var31
void blueStalfos_main_state0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e70, 0x5e71); H = D;
  CYC(0x5e71, 0x5e73); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(0x5e73, ecom_readPositionVars_b0f_hook, 0x4439, 0x5e76);
  CYC(0x5e76, 0x5e77); alu_sub(gb, C);
  CYC(0x5e77, 0x5e79); alu_add(gb, 0x04);
  CYC(0x5e79, 0x5e7b); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x5e7b, 0x5e7d); goto moveToPosition; } // jr nc
  CYC(0x5e7b, 0x5e7d);
  CYC(0x5e7d, 0x5e7f); A = hram_rd(gb, 0x8f);
  CYC(0x5e7f, 0x5e80); alu_sub(gb, B);
  CYC(0x5e80, 0x5e82); alu_add(gb, 0x04);
  CYC(0x5e82, 0x5e84); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x5e84, 0x5e86); goto moveToPosition; } // jr nc
  CYC(0x5e84, 0x5e86);
  CYC(0x5e86, 0x5e87); H = D;
  CYC(0x5e87, 0x5e89); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5e89, 0x5e8b); mem_wr(gb, HL, 0x0b);
  CYC(0x5e8b, 0x5e8d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5e8d, 0x5e8f); mem_wr(gb, HL, 0x10); // 16
  CYCT(0x5e8f, 0x5e91); blueStalfos_main_animate_hook(gb); return; // jr

moveToPosition:
  CALL_C(0x5e91, blueStalfos_main_accelerate_hook, 0x6153, 0x5e94);
  CALL_C(0x5e94, ecom_moveTowardPosition_b0f_hook, 0x4430, 0x5e97);
  CYCT(0x5e97, 0x5e99); blueStalfos_main_animate_hook(gb); return; // jr
}

// Reached position, standing still for [counter1] frames
void blueStalfos_main_state0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e99, ecom_decCounter1_b0f_hook, 0x439a, 0x5e9c);
  if (!(F & FZ)) { CYCT(0x5e9c, 0x5e9e); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(0x5e9c, 0x5e9e);
  CYC(0x5e9e, 0x5e9f); L = E;
  CYC(0x5e9f, 0x5ea0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  blueStalfos_main_animate_hook(gb); return; // fallthrough
}

void blueStalfos_main_animate_hook(GB *gb) {
  CYC(0x5ea0, 0x5ea3); enemyAnimate_hook(gb); return; // jp
}

// Decide which attack to do
void blueStalfos_main_state0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ea3, 0x5ea5); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5ea5, 0x5ea6); A = mem_rd(gb, DE);
  CYC(0x5ea6, 0x5ea8); alu_add(gb, 0x10);
  CYC(0x5ea8, 0x5ea9); B = A;
  CYC(0x5ea9, 0x5eab); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5eab, 0x5eac); A = mem_rd(gb, DE);
  CYC(0x5eac, 0x5eae); alu_add(gb, 0x04);
  CYC(0x5eae, 0x5eaf); C = A;
  CYC(0x5eaf, 0x5eb1); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5eb1, 0x5eb2); alu_sub(gb, B);
  CYC(0x5eb2, 0x5eb4); alu_add(gb, 0x14);
  CYC(0x5eb4, 0x5eb6); alu_cp(gb, 0x29);
  if (!(F & FC)) { CYCT(0x5eb6, 0x5eb8); goto projectileAttack; } // jr nc
  CYC(0x5eb6, 0x5eb8);
  CYC(0x5eb8, 0x5eba); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5eba, 0x5ebb); alu_sub(gb, C);
  CYC(0x5ebb, 0x5ebd); alu_add(gb, 0x12);
  CYC(0x5ebd, 0x5ebf); alu_cp(gb, 0x25);
  if (F & FC) { CYCT(0x5ebf, 0x5ec2); blueStalfos_main_beginSickleAttack_hook(gb); return; } // jp c
  CYC(0x5ebf, 0x5ec2);

projectileAttack:
  CYC(0x5ec2, 0x5ec4); B = 0x3d; // PART_BLUE_STALFOS_PROJECTILE
  CALL_C(0x5ec4, ecom_spawnProjectile_b0f_hook, 0x437c, 0x5ec7);
  if (!(F & FZ)) { RET_TAKEN(0x5ec7); return; } // ret nz
  CYC(0x5ec7, 0x5ec8);
  CYC(0x5ec8, 0x5ec9); H = D;
  CYC(0x5ec9, 0x5ecb); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5ecb, 0x5ecd); mem_wr(gb, HL, 0x78); // 120
  CYC(0x5ecd, 0x5ecf); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5ecf, 0x5ed1); mem_wr(gb, HL, 0x0e);
  CYC(0x5ed1, 0x5ed3); A = 0x02;
  CYC(0x5ed3, 0x5ed6); enemySetAnimation_hook(gb); return; // jp
}

// Sickle attack
void blueStalfos_main_state0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5ed6, enemyAnimate_hook, 0x2818, 0x5ed9);
  CYC(0x5ed9, 0x5edb); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5edb, 0x5edc); A = mem_rd(gb, DE);
  CYC(0x5edc, 0x5edd); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x5edd, 0x5edf); blueStalfos_main_finishedAttack_hook(gb); return; } // jr z
  CYC(0x5edd, 0x5edf);
  CYC(0x5edf, 0x5ee0); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5ee0); return; } // ret nz
  CYC(0x5ee0, 0x5ee1);
  CYC(0x5ee1, 0x5ee3); A = 0x08;
  CYC(0x5ee3, 0x5ee4); mem_wr(gb, DE, A); // [animParameter]
  CYC(0x5ee4, 0x5ee6); A = 0x6b; // SND_SWORDSPIN
  CYC(0x5ee6, 0x5ee9); playSound_b00_hook(gb); return; // jp
}

// Charging a projectile
void blueStalfos_main_state0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5ee9, ecom_decCounter1_b0f_hook, 0x439a, 0x5eec);
  if (!(F & FZ)) { CYCT(0x5eec, 0x5eee); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(0x5eec, 0x5eee);
  CYC(0x5eee, 0x5ef0); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(0x5ef0, 0x5ef1); L = E;
  CYC(0x5ef1, 0x5ef2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5ef2, 0x5ef4); A = 0x03;
  CYC(0x5ef4, 0x5ef7); enemySetAnimation_hook(gb); return; // jp
}

// Just fired projectile
void blueStalfos_main_state0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5ef7, ecom_decCounter1_b0f_hook, 0x439a, 0x5efa);
  if (!(F & FZ)) { RET_TAKEN(0x5efa); return; } // ret nz
  CYC(0x5efa, 0x5efb);
  blueStalfos_main_finishedAttack_hook(gb); return; // fallthrough
}

void blueStalfos_main_finishedAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5efb, 0x5efc); H = D;
  CYC(0x5efc, 0x5efe); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5efe, 0x5f00); mem_wr(gb, HL, 0x0a);
  CYC(0x5f00, 0x5f02); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5f02, 0x5f04); mem_wr(gb, HL, 0x40); // 64
  CYC(0x5f04, 0x5f06); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5f06, 0x5f08); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x5f08, 0x5f09); alu_xor(gb, A);
  CALL_C(0x5f09, enemySetAnimation_hook, 0x282b, 0x5f0c);
  CYC(0x5f0c, 0x5f0f); blueStalfos_main_decideNextPosition_hook(gb); return; // jp
}

// Link just turned into a baby; about to turn transparent and warp to top of room
void blueStalfos_main_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f0f, ecom_decCounter1_b0f_hook, 0x439a, 0x5f12);
  if (!(F & FZ)) { CYCT(0x5f12, 0x5f14); blueStalfos_main_animate_hook(gb); return; } // jr nz
  CYC(0x5f12, 0x5f14);
  CYC(0x5f14, 0x5f16); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(0x5f16, 0x5f17); L = E;
  CYC(0x5f17, 0x5f18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5f18, 0x5f1a); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5f1a, 0x5f1c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(0x5f1c); return;
}

// Now transparent; waiting for [counter1] frames before warping
void blueStalfos_main_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f1d, ecom_decCounter1_b0f_hook, 0x439a, 0x5f20);
  if (!(F & FZ)) { CYCT(0x5f20, 0x5f23); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(0x5f20, 0x5f23);
  CYC(0x5f23, 0x5f25); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(0x5f25, 0x5f26); L = E;
  CYC(0x5f26, 0x5f27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5f27, 0x5f29); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5f29, 0x5f2b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5f2b, 0x5f2d); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5f2d, 0x5f2f); mem_wr(gb, HL, 0x0c);
  CYC(0x5f2f, 0x5f31); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5f31, 0x5f33); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5f33, 0x5f34); mem_wr(gb, HL, A);
  CYC(0x5f34, 0x5f35); alu_xor(gb, A);
  CALL_C(0x5f35, enemySetAnimation_hook, 0x282b, 0x5f38);
  CYC(0x5f38, 0x5f3b); objectSetInvisible_hook(gb); return; // jp
}

// Just warped to top of room; standing in place
void blueStalfos_main_state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f3b, ecom_decCounter1_b0f_hook, 0x439a, 0x5f3e);
  if (!(F & FZ)) { CYCT(0x5f3e, 0x5f41); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(0x5f3e, 0x5f41);
  CYC(0x5f41, 0x5f43); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5f43, 0x5f45); mem_wr(gb, HL, 0x10);
  CYC(0x5f45, 0x5f46); L = E;
  CYC(0x5f46, 0x5f47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CALL_C(0x5f47, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5f4a);
  CYC(0x5f4a, 0x5f4c); alu_and(gb, 0x03);
  CYC(0x5f4c, 0x5f4f); SET_HL(0x5f57);
  CYC(0x5f4f, 0x5f50); blueStalfos_addAToHl_from_rst(gb, 0x5f50);
  CYC(0x5f50, 0x5f52); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5f52, 0x5f53); A = mem_rd(gb, HL);
  CYC(0x5f53, 0x5f54); mem_wr(gb, DE, A);
  CYC(0x5f54, 0x5f5b); objectSetVisible82_hook(gb); return; // jp
}

// Moving down toward baby Link before attacking with sickle
void blueStalfos_main_state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f5b, 0x5f5c); H = D;
  CYC(0x5f5c, 0x5f5e); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5f5e, 0x5f60); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5f60, 0x5f61); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5f61, 0x5f63); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(0x5f63, 0x5f66); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x5f63, 0x5f66);
  blueStalfos_main_beginSickleAttack_hook(gb); return; // fallthrough
}

void blueStalfos_main_beginSickleAttack_hook(GB *gb) {
  CYC(0x5f66, 0x5f68); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5f68, 0x5f6a); A = 0x0d;
  CYC(0x5f6a, 0x5f6b); mem_wr(gb, DE, A);
  CYC(0x5f6b, 0x5f6d); A = 0x01;
  CYC(0x5f6d, 0x5f70); enemySetAnimation_hook(gb); return; // jp
}

// Just hit by PART_BLUE_STALFOS_PROJECTILE; turning into a small bat
void blueStalfos_main_state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f70, blueStalfos_createPuff_hook, 0x6183, 0x5f73);
  if (!(F & FZ)) { RET_TAKEN(0x5f73); return; } // ret nz
  CYC(0x5f73, 0x5f74);
  CYC(0x5f74, 0x5f76); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5f76, 0x5f77); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5f77, 0x5f79); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5f79, 0x5f7b); mem_wr(gb, HL, 0x02);
  CYC(0x5f7b, 0x5f7c); L = alu_inc8(gb, L);
  CYC(0x5f7c, 0x5f7e); mem_wr(gb, HL, 0x06); // [collisionRadiusX]
  CYC(0x5f7e, 0x5f80); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5f80, 0x5f82); mem_wr(gb, HL, 0xf0);
  CYC(0x5f82, 0x5f83); L = alu_inc8(gb, L);
  CYC(0x5f83, 0x5f85); mem_wr(gb, HL, 0x00); // [counter2]
  CYC(0x5f85, 0x5f87); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5f87, 0x5f89); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CALL_C(0x5f89, objectSetInvisible_hook, 0x1e7b, 0x5f8c);
  CYC(0x5f8c, 0x5f8e); A = 0x85; // SND_SCENT_SEED
  CALL_C(0x5f8e, playSound_b00_hook, 0x0c98, 0x5f91);
  CYC(0x5f91, 0x5f93); A = 0x04;
  CYC(0x5f93, 0x5f96); enemySetAnimation_hook(gb); return; // jp
}

// Transforming into bat
void blueStalfos_main_state15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f96, 0x5f98); A = OBJ_ANIM_PARAMETER;
  CALL_C(0x5f98, objectGetRelatedObject2Var_hook, 0x2164, 0x5f9b);
  CYC(0x5f9b, 0x5f9d); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x5f9d); return; } // ret nz
  CYC(0x5f9d, 0x5f9e);
  CALL_C(0x5f9e, ecom_incState_b0f_hook, 0x4000, 0x5fa1);
  CYC(0x5fa1, 0x5fa3); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5fa3, 0x5fa5); mem_wr(gb, HL, 0x65); // ENEMYCOLLISION_BLUE_STALFOS_BAT
  CYC(0x5fa5, 0x5fa7); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5fa7, 0x5fa9); mem_wr(gb, HL, 0x00);
  CYC(0x5fa9, 0x5fac); objectSetVisiblec2_hook(gb); return; // jp
}

// Flying around as a bat
void blueStalfos_main_state16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5fac, ecom_decCounter1_b0f_hook, 0x439a, 0x5faf);
  if (!(F & FZ)) { CYCT(0x5faf, 0x5fb1); goto flyAround; } // jr nz
  CYC(0x5faf, 0x5fb1);
  CYC(0x5fb1, 0x5fb2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CALL_C(0x5fb2, blueStalfos_createPuff_hook, 0x6183, 0x5fb5);
  if (!(F & FZ)) { RET_TAKEN(0x5fb5); return; } // ret nz
  CYC(0x5fb5, 0x5fb6);
  CYC(0x5fb6, 0x5fb8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5fb8, 0x5fb9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5fb9, 0x5fbb); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5fbb, 0x5fbd); mem_wr(gb, HL, 0x48); // ENEMYCOLLISION_BLUE_STALFOS
  CYC(0x5fbd, 0x5fbf); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5fbf, 0x5fc1); mem_wr(gb, HL, 0xff);
  CYC(0x5fc1, 0x5fc4); objectSetInvisible_hook(gb); return; // jp

flyAround:
  CALL_C(0x5fc4, ecom_decCounter2_b0f_hook, 0x43a3, 0x5fc7);
  if (!(F & FZ)) { CYCT(0x5fc7, 0x5fc9); goto L_5fce; } // jr nz
  CYC(0x5fc7, 0x5fc9);
  CYC(0x5fc9, 0x5fcb); mem_wr(gb, HL, 0x1e); // [counter2] = 30
  CALL_C(0x5fcb, ecom_setRandomAngle_b0f_hook, 0x43cf, 0x5fce);

L_5fce:
  CALL_C(0x5fce, ecom_bounceOffWallsAndHoles_b0f_hook, 0x42de, 0x5fd1);
  CALL_C(0x5fd1, objectApplySpeed_hook, 0x201d, 0x5fd4);
  CYC(0x5fd4, 0x5fd7); enemyAnimate_hook(gb); return; // jp
}

// Transforming back into stalfos
void blueStalfos_main_state17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fd7, 0x5fd9); A = OBJ_ANIM_PARAMETER;
  CALL_C(0x5fd9, objectGetRelatedObject2Var_hook, 0x2164, 0x5fdc);
  CYC(0x5fdc, 0x5fde); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x5fde); return; } // ret nz
  CYC(0x5fde, 0x5fdf);
  CYC(0x5fdf, 0x5fe1); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5fe1, 0x5fe3); A = 0x08;
  CYC(0x5fe3, 0x5fe4); mem_wr(gb, DE, A);
  CYC(0x5fe4, 0x5fe5); E = alu_inc8(gb, E);
  CYC(0x5fe5, 0x5fe6); mem_wr(gb, DE, A);
  CALL_C(0x5fe6, blueStalfos_main_finishedAttack_hook, 0x5efb, 0x5fe9);
  CYC(0x5fe9, 0x5feb); A = 0x85; // SND_SCENT_SEED
  CALL_C(0x5feb, playSound_b00_hook, 0x0c98, 0x5fee);
  CYC(0x5fee, 0x5ff1); objectSetVisible82_hook(gb); return; // jp
}

// Hitbox for the sickle (invisible)
void blueStalfos_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ff1, 0x5ff2); A = mem_rd(gb, DE);
  CYC(0x5ff2, 0x5ff4); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(0x5ff4, 0x5ff6); blueStalfos_initSubid2Or3_hook(gb); return; } // jr z
  CYC(0x5ff4, 0x5ff6);

  CYC(0x5ff6, 0x5ff8); A = OBJ_ID;
  CALL_C(0x5ff8, objectGetRelatedObject1Var_hook, 0x2160, 0x5ffb);
  CYC(0x5ffb, 0x5ffc); A = mem_rd(gb, HL);
  CYC(0x5ffc, 0x5ffe); alu_cp(gb, 0x77); // ENEMY_BLUE_STALFOS
  if (!(F & FZ)) { CYCT(0x5ffe, 0x6001); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x5ffe, 0x6001);
  CYC(0x6001, 0x6003); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6003, 0x6004); E = L;
  CYC(0x6004, 0x6005); A = mem_rd(gb, HL);
  CYC(0x6005, 0x6006); mem_wr(gb, DE, A);
  CYC(0x6006, 0x6008); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6008, 0x6009); A = mem_rd(gb, HL);
  CYC(0x6009, 0x600b); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x600b, 0x600d); goto L_600f; } // jr nz
  CYC(0x600b, 0x600d);
  CYC(0x600d, 0x600f); A = 0x0c;

L_600f:
  CYC(0x600f, 0x6012); SET_BC(0x6033);
  CALL_C(0x6012, addAToBc_hook, 0x006d, 0x6015);
  CYC(0x6015, 0x6017); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6017, 0x6018); E = L;
  CYC(0x6018, 0x6019); A = mem_rd(gb, BC);
  CYC(0x6019, 0x601a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x601a, 0x601b); mem_wr(gb, DE, A);
  CYC(0x601b, 0x601c); SET_BC(BC + 1);
  CYC(0x601c, 0x601e); L = ENEMY_BASE + OBJ_XH;
  CYC(0x601e, 0x601f); E = L;
  CYC(0x601f, 0x6020); A = mem_rd(gb, BC);
  CYC(0x6020, 0x6021); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6021, 0x6022); mem_wr(gb, DE, A);
  CYC(0x6022, 0x6023); SET_BC(BC + 1);
  CYC(0x6023, 0x6025); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6025, 0x6026); A = mem_rd(gb, BC);
  CYC(0x6026, 0x6027); mem_wr(gb, DE, A);
  CYC(0x6027, 0x6028); SET_BC(BC + 1);
  CYC(0x6028, 0x6029); E = alu_inc8(gb, E);
  CYC(0x6029, 0x602a); A = mem_rd(gb, BC);
  CYC(0x602a, 0x602b); mem_wr(gb, DE, A);
  CYC(0x602b, 0x602c); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x602c); return; } // ret nz
  CYC(0x602c, 0x602d);
  CYC(0x602d, 0x602e); H = D;
  CYC(0x602e, 0x6030); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6030, 0x6032); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(0x6032); return;
}

void blueStalfos_initSubid2Or3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6057, 0x6058); H = D;
  CYC(0x6058, 0x6059); L = E;
  CYC(0x6059, 0x605a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x605a, 0x605c); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x605c, 0x605e); mem_wr(gb, HL, 0x66); // ENEMYCOLLISION_BLUE_STALFOS_SICKLE
  CYC(0x605e, 0x6060); A = OBJ_ZH;
  CALL_C(0x6060, objectGetRelatedObject1Var_hook, 0x2160, 0x6063);
  CYC(0x6063, 0x6064); E = L;
  CYC(0x6064, 0x6065); A = mem_rd(gb, HL);
  CYC(0x6065, 0x6066); mem_wr(gb, DE, A);
  RET(0x6066); return;
}

// "Afterimage" of blue stalfos visible while moving
void blueStalfos_subid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6067, 0x6068); A = mem_rd(gb, DE);
  CYC(0x6068, 0x606a); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(0x606a, 0x606c); goto state8; } // jr z
  CYC(0x606a, 0x606c);

  CYC(0x606c, 0x606e); A = OBJ_ID;
  CALL_C(0x606e, objectGetRelatedObject1Var_hook, 0x2160, 0x6071);
  CYC(0x6071, 0x6072); A = mem_rd(gb, HL);
  CYC(0x6072, 0x6074); alu_cp(gb, 0x77); // ENEMY_BLUE_STALFOS
  if (!(F & FZ)) { CYCT(0x6074, 0x6077); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x6074, 0x6077);
  CYC(0x6077, 0x6079); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6079, 0x607a); A = mem_rd(gb, HL);
  CYC(0x607a, 0x607c); alu_cp(gb, 0x12);
  if (F & FZ) { CYCT(0x607c, 0x607f); blueStalfos_afterImage_resetPositionVars_hook(gb); return; } // jp z
  CYC(0x607c, 0x607f);
  CYC(0x607f, 0x6081); alu_cp(gb, 0x14);
  if (F & FC) { CALL_C_CC(0x6081, objectSetVisible82_hook, 0x1e69, 0x6084); } else { CYC(0x6081, 0x6084); } // call nc
  CYC(0x6084, 0x6086); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6086, 0x6088); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6088, 0x6089); A = mem_rd(gb, DE);
  CYC(0x6089, 0x608a); B = A;
  CYC(0x608a, 0x608b); A = mem_rd(gb, HL);
  CYC(0x608b, 0x608c); alu_sub(gb, B);
  CYC(0x608c, 0x608e); alu_add(gb, 0x08);
  CYC(0x608e, 0x6090); alu_and(gb, 0x0f);
  CYC(0x6090, 0x6092); A = alu_swap(gb, A);
  CYC(0x6092, 0x6093); C = A;
  CYC(0x6093, 0x6094); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6094, 0x6095); mem_wr(gb, DE, A);
  CYC(0x6095, 0x6096); L = alu_inc8(gb, L);
  CYC(0x6096, 0x6097); E = alu_inc8(gb, E);
  CYC(0x6097, 0x6098); A = mem_rd(gb, DE);
  CYC(0x6098, 0x6099); B = A;
  CYC(0x6099, 0x609a); A = mem_rd(gb, HL);
  CYC(0x609a, 0x609b); alu_sub(gb, B);
  CYC(0x609b, 0x609d); alu_add(gb, 0x08);
  CYC(0x609d, 0x609f); alu_and(gb, 0x0f);
  CYC(0x609f, 0x60a0); alu_or(gb, C);
  CYC(0x60a0, 0x60a1); C = A;
  CYC(0x60a1, 0x60a2); A = mem_rd(gb, HL);
  CYC(0x60a2, 0x60a3); mem_wr(gb, DE, A);
  CYC(0x60a3, 0x60a5); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x60a5, 0x60a6); A = mem_rd(gb, DE);
  CYC(0x60a6, 0x60a8); alu_add(gb, ENEMY_BASE + 0x33); // Enemy.var33
  CYC(0x60a8, 0x60a9); E = A;
  CYC(0x60a9, 0x60aa); A = C;
  CYC(0x60aa, 0x60ab); mem_wr(gb, DE, A);
  CYC(0x60ab, 0x60ad); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x60ad, 0x60ae); A = mem_rd(gb, DE);
  CYC(0x60ae, 0x60af); A = alu_inc8(gb, A);
  CYC(0x60af, 0x60b1); alu_and(gb, 0x07);
  CYC(0x60b1, 0x60b2); mem_wr(gb, DE, A);
  CYC(0x60b2, 0x60b4); alu_add(gb, ENEMY_BASE + 0x33); // Enemy.var33
  CYC(0x60b4, 0x60b5); E = A;
  CYC(0x60b5, 0x60b6); A = mem_rd(gb, DE);
  CYC(0x60b6, 0x60b8); alu_cp(gb, 0x88);
  CYC(0x60b8, 0x60b9); B = A;
  if (F & FZ) { CYCT(0x60b9, 0x60bc); objectSetInvisible_hook(gb); return; } // jp z
  CYC(0x60b9, 0x60bc);
  CYC(0x60bc, 0x60bd); H = D;
  CYC(0x60bd, 0x60bf); L = ENEMY_BASE + OBJ_YH;
  CYC(0x60bf, 0x60c1); alu_and(gb, 0xf0);
  CYC(0x60c1, 0x60c3); A = alu_swap(gb, A);
  CYC(0x60c3, 0x60c5); alu_sub(gb, 0x08);
  CYC(0x60c5, 0x60c6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x60c6, 0x60c7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x60c7, 0x60c8); L = alu_inc8(gb, L);
  CYC(0x60c8, 0x60c9); A = B;
  CYC(0x60c9, 0x60cb); alu_and(gb, 0x0f);
  CYC(0x60cb, 0x60cd); alu_sub(gb, 0x08);
  CYC(0x60cd, 0x60ce); alu_add(gb, mem_rd(gb, HL));
  CYC(0x60ce, 0x60cf); mem_wr(gb, HL, A);
  CYC(0x60cf, 0x60d2); ecom_flickerVisibility_b0f_hook(gb); return; // jp

state8:
  CALL_C(0x60d2, blueStalfos_initSubid2Or3_hook, 0x6057, 0x60d5);
  CALL_C(0x60d5, blueStalfos_afterImage_resetPositionVars_hook, 0x6164, 0x60d8);
  CYC(0x60d8, 0x60da); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x60da, 0x60dc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(0x60dc, objectSetVisible83_hook, 0x1e72, 0x60df);
  CYC(0x60df, 0x60e2); objectSetInvisible_hook(gb); return; // jp
}

// Decides the next position for the blue stalfos. It will always choose a different
// quadrant of the screen from the one it's in already.
void blueStalfos_main_decideNextPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x60e2, 0x60e4); E = 0x03;
  CYC(0x60e4, 0x60e7); SET_BC(0x3030);
  CALL_C(0x60e7, ecom_randomBitwiseAndBCE_b0f_hook, 0x434f, 0x60ea);
  CYC(0x60ea, 0x60eb); H = E;
  CYC(0x60eb, 0x60ed); L = 0x00;
  CYC(0x60ed, 0x60ef); E = ENEMY_BASE + OBJ_YH;
  CYC(0x60ef, 0x60f0); A = mem_rd(gb, DE);
  CYC(0x60f0, 0x60f2); alu_cp(gb, 0x58);
  if (F & FC) { CYCT(0x60f2, 0x60f4); goto L_60f6; } // jr c
  CYC(0x60f2, 0x60f4);
  CYC(0x60f4, 0x60f6); L = 0x02;

L_60f6:
  CYC(0x60f6, 0x60f8); E = ENEMY_BASE + OBJ_XH;
  CYC(0x60f8, 0x60f9); A = mem_rd(gb, DE);
  CYC(0x60f9, 0x60fb); alu_cp(gb, 0x78);
  if (F & FC) { CYCT(0x60fb, 0x60fd); goto L_60fe; } // jr c
  CYC(0x60fb, 0x60fd);
  CYC(0x60fd, 0x60fe); L = alu_inc8(gb, L);

L_60fe:
  CYC(0x60fe, 0x60ff); A = L;
  CYC(0x60ff, 0x6100); alu_add(gb, A);
  CYC(0x6100, 0x6101); alu_add(gb, A);
  CYC(0x6101, 0x6102); alu_add(gb, H);
  blueStalfos_main_moveToQuadrant_hook(gb); return; // fallthrough
}

// @param a Position index to use
// @param bc Offset to be added to target position
void blueStalfos_main_moveToQuadrant_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6102, 0x6105); SET_HL(0x613b);
  CYC(0x6105, 0x6106); blueStalfos_addAToHl_from_rst(gb, 0x6106);
  CALL_C(0x6106, blueStalfos_main_moveToQuadrant_getLinkQuadrant_hook, 0x6127, 0x6109);
  CYC(0x6109, 0x610a); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x610a, 0x610c); goto moveToLinksPosition; } // jr z
  CYC(0x610a, 0x610c);
  CYC(0x610c, 0x610d); A = mem_rd(gb, HL);
  CYC(0x610d, 0x6110); SET_HL(0x614b);
  CYC(0x6110, 0x6111); blueStalfos_addAToHl_from_rst(gb, 0x6111);
  CYC(0x6111, 0x6113); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6113, 0x6114); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6114, 0x6115); alu_add(gb, B);
  CYC(0x6115, 0x6116); mem_wr(gb, DE, A);
  CYC(0x6116, 0x6117); E = alu_inc8(gb, E);
  CYC(0x6117, 0x6118); A = mem_rd(gb, HL);
  CYC(0x6118, 0x6119); alu_add(gb, C);
  CYC(0x6119, 0x611a); mem_wr(gb, DE, A);
  RET(0x611a); return;

moveToLinksPosition:
  CYC(0x611b, 0x611d); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x611d, 0x611f); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x611f, 0x6121); alu_sub(gb, 0x14);
  CYC(0x6121, 0x6122); mem_wr(gb, DE, A);
  CYC(0x6122, 0x6123); E = alu_inc8(gb, E);
  CYC(0x6123, 0x6125); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x6125, 0x6126); mem_wr(gb, DE, A);
  RET(0x6126); return;
}

// @param[out] a The quadrant of the screen Link is in.
// (0/2/4/6 for up/left, up/right, down/left, down/right)
// Bare local (@getLinkQuadrant), not previously hooked; promoted to a full routine here since it's
// reached via a genuine call/ret from blueStalfos_main_moveToQuadrant.
void blueStalfos_main_moveToQuadrant_getLinkQuadrant_hook(GB *gb) {
  CYC(0x6127, 0x6129); E = 0x00;
  CYC(0x6129, 0x612b); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x612b, 0x612d); alu_cp(gb, 0x58); // (LARGE_ROOM_HEIGHT<<4)/2
  if (F & FC) { CYCT(0x612d, 0x612f); goto skipY; } // jr c
  CYC(0x612d, 0x612f);
  CYC(0x612f, 0x6131); E = 0x02;

skipY:
  CYC(0x6131, 0x6133); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x6133, 0x6135); alu_cp(gb, 0x78); // (LARGE_ROOM_WIDTH<<4)/2
  if (F & FC) { CYCT(0x6135, 0x6137); goto skipX; } // jr c
  CYC(0x6135, 0x6137);
  CYC(0x6137, 0x6138); E = alu_inc8(gb, E);

skipX:
  CYC(0x6138, 0x6139); A = E;
  CYC(0x6139, 0x613a); alu_add(gb, A);
  RET(0x613a); return;
}

void blueStalfos_main_accelerate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6153, 0x6155); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6155, 0x6156); A = mem_rd(gb, DE);
  CYC(0x6156, 0x6157); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6157); return; } // ret z
  CYC(0x6157, 0x6158);
  CYC(0x6158, 0x6159); A = alu_dec8(gb, A);
  CYC(0x6159, 0x615a); mem_wr(gb, DE, A);
  CYC(0x615a, 0x615c); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x615c); return; } // ret nz
  CYC(0x615c, 0x615d);
  CYC(0x615d, 0x615f); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x615f, 0x6160); A = mem_rd(gb, DE);
  CYC(0x6160, 0x6162); alu_add(gb, 0x05);
  CYC(0x6162, 0x6163); mem_wr(gb, DE, A);
  RET(0x6163); return;
}

void blueStalfos_afterImage_resetPositionVars_hook(GB *gb) {
  CYC(0x6164, 0x6166); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6166, 0x6167); E = L;
  CYC(0x6167, 0x6168); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6168, 0x6169); mem_wr(gb, DE, A);
  CYC(0x6169, 0x616b); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x616b, 0x616c); mem_wr(gb, DE, A);
  CYC(0x616c, 0x616d); L = alu_inc8(gb, L);
  CYC(0x616d, 0x616e); E = L;
  CYC(0x616e, 0x616f); A = mem_rd(gb, HL);
  CYC(0x616f, 0x6170); mem_wr(gb, DE, A);
  CYC(0x6170, 0x6172); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6172, 0x6173); mem_wr(gb, DE, A);
  CYC(0x6173, 0x6174); H = D;
  CYC(0x6174, 0x6176); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6176, 0x6177); alu_xor(gb, A);
  CYC(0x6177, 0x6178); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6178, 0x617a); A = 0x88;
  CYC(0x617a, 0x617b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x617b, 0x617c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x617c, 0x617d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x617d, 0x617e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x617e, 0x617f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x617f, 0x6180); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6180, 0x6181); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6181, 0x6182); mem_wr(gb, HL, A);
  RET(0x6182); return;
}

void blueStalfos_createPuff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6183, 0x6186); SET_BC(0x0502); // INTERAC_PUFF, subid 2
  CALL_C(0x6186, objectCreateInteraction_hook, 0x24c5, 0x6189);
  if (!(F & FZ)) { RET_TAKEN(0x6189); return; } // ret nz
  CYC(0x6189, 0x618a);
  CYC(0x618a, 0x618b); A = H;
  CYC(0x618b, 0x618c); H = D;
  CYC(0x618c, 0x618e); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x618e, 0x618f); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x618f, 0x6191); mem_wr(gb, HL, 0x40); // Interaction.start
  RET(0x6191); return;
}
