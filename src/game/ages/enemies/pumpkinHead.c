#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb);

void enemyCode78_hook(GB *gb);
void pumpkinHead_state_uninitialized_hook(GB *gb);
void pumpkinHead_state_spawner_hook(GB *gb);
void pumpkinHead_state_grabbed_hook(GB *gb);
void pumpkinHead_state_stub_hook(GB *gb);
void pumpkinHead_body_hook(GB *gb);
void pumpkinHead_body_state08_hook(GB *gb);
void pumpkinHead_body_state09_hook(GB *gb);
void pumpkinHead_body_shakeScreen_hook(GB *gb);
void pumpkinHead_body_state0a_hook(GB *gb);
void pumpkinHead_body_state0b_hook(GB *gb);
void pumpkinHead_body_chooseNextAction_hook(GB *gb);
void pumpkinHead_body_state0c_hook(GB *gb);
void pumpkinHead_body_beginMoving_hook(GB *gb);
void pumpkinHead_body_updateAnimationFromAngle_hook(GB *gb);
void pumpkinHead_body_state0d_hook(GB *gb);
void pumpkinHead_body_beginStomp_hook(GB *gb);
void pumpkinHead_body_state0e_hook(GB *gb);
void pumpkinHead_body_state0f_hook(GB *gb);
void pumpkinHead_body_state10_hook(GB *gb);
void pumpkinHead_body_state11_hook(GB *gb);
void pumpkinHead_body_state12_hook(GB *gb);
void pumpkinHead_body_state13_hook(GB *gb);
void pumpkinHead_ghost_hook(GB *gb);
void pumpkinHead_ghost_state08_hook(GB *gb);
void pumpkinHead_ghost_state09_hook(GB *gb);
void pumpkinHead_ghost_state0a_hook(GB *gb);
void pumpkinHead_ghost_state0b_hook(GB *gb);
void pumpkinHead_ghost_state0c_hook(GB *gb);
void pumpkinHead_ghost_state0d_hook(GB *gb);
void pumpkinHead_ghost_state0e_hook(GB *gb);
void pumpkinHead_ghost_state0f_hook(GB *gb);
void pumpkinHead_ghost_state10_hook(GB *gb);
void pumpkinHead_ghost_state11_hook(GB *gb);
void pumpkinHead_ghost_state12_hook(GB *gb);
void pumpkinHead_ghost_state13_hook(GB *gb);
void pumpkinHead_ghost_state14_hook(GB *gb);
void pumpkinHead_ghost_state15_hook(GB *gb);
void pumpkinHead_ghost_state16_hook(GB *gb);
void pumpkinHead_ghost_state17_hook(GB *gb);
void pumpkinHead_head_hook(GB *gb);
void pumpkinHead_head_state08_hook(GB *gb);
void pumpkinHead_head_setAnimation_hook(GB *gb);
void pumpkinHead_head_state09_hook(GB *gb);
void pumpkinHead_head_state0a_hook(GB *gb);
void pumpkinHead_head_state0b_hook(GB *gb);
void pumpkinHead_head_state0c_hook(GB *gb);
void pumpkinHead_head_state0d_hook(GB *gb);
void pumpkinHead_head_state0e_hook(GB *gb);
void pumpkinHead_head_state0f_hook(GB *gb);
void pumpkinHead_head_state10_hook(GB *gb);
void pumpkinHead_head_state11_hook(GB *gb);
void pumpkinHead_head_state12_hook(GB *gb);
void pumpkinHead_head_state13_hook(GB *gb);
void pumpkinHead_head_state14_hook(GB *gb);
void pumpkinHead_head_state15_hook(GB *gb);
void pumpkinHead_head_state16_hook(GB *gb);
void pumpkinHead_body_countdownUntilStomp_hook(GB *gb);
void pumpkinHead_body_chooseRandomStompTimerAndCount_hook(GB *gb);
void pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook(GB *gb);
void pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook(GB *gb);
void pumpkinHead_noHealth_hook(GB *gb);

static uint16_t pumpkinHead_jump_table(GB *gb) {
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

static void pumpkinHead_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void pumpkinHead_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_PUMPKIN_HEAD
//
// Variables (body, subid 1):
//   relatedObj1: Reference to ghost
//   relatedObj2: Reference to head
//   var30: Stomp counter (stops stomping when it reaches 0)
//
// Variables (ghost, subid 2):
//   relatedObj1: Reference to body
//   var33/var34: Head's position (where ghost is moving toward)
//
// Variables (head, subid 3):
//   relatedObj1: Reference to body
//   var31: Link's direction last frame
//   var32: Head's orientation when it was picked up
// ==================================================================================================
void enemyCode78_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6192, 0x6194); goto normalStatus; } // jr z
  CYC(0x6192, 0x6194);
  CYC(0x6194, 0x6196); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6196); return; } // ret c
  CYC(0x6196, 0x6197);
  if (F & FZ) { CYCT(0x6197, 0x6199); goto dead; } // jr z
  CYC(0x6197, 0x6199);
  CYCT(0x6199, 0x619b); goto normalStatus; // jr

dead:
  CALL_C(0x619b, pumpkinHead_noHealth_hook, 0x6757, 0x619e);
  if (F & FZ) { RET_TAKEN(0x619e); return; } // ret z
  CYC(0x619e, 0x619f);

normalStatus:
  CALL_C(0x619f, ecom_getSubidAndCpStateTo08_b0f_hook, 0x4426, 0x61a2);
  if (F & FC) { CYCT(0x61a2, 0x61a4); goto commonState; } // jr c
  CYC(0x61a2, 0x61a4);
  CYC(0x61a4, 0x61a5); B = alu_dec8(gb, B);
  CYC(0x61a5, 0x61a6); A = B;
  CYC(0x61a6, 0x61a7); push_effect(gb, 0x61a7);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == 0x626c) { pumpkinHead_body_hook(gb); return; }
    if (target == 0x63ec) { pumpkinHead_ghost_hook(gb); return; }
    if (target == 0x6564) { pumpkinHead_head_hook(gb); return; }
    HANDOFF(target);
  }

commonState:
  CYC(0x61ad, 0x61ae); push_effect(gb, 0x61ae);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == 0x61be) { pumpkinHead_state_uninitialized_hook(gb); return; }
    if (target == 0x61cc) { pumpkinHead_state_spawner_hook(gb); return; }
    if (target == 0x620d) { pumpkinHead_state_grabbed_hook(gb); return; }
    if (target == 0x626b) { pumpkinHead_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

void pumpkinHead_state_uninitialized_hook(GB *gb) {
  CYC(0x61be, 0x61bf); A = B;
  CYC(0x61bf, 0x61c0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x61c0, 0x61c3); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(0x61c0, 0x61c3);
  CYC(0x61c3, 0x61c4); A = alu_inc8(gb, A);
  CYC(0x61c4, 0x61c5); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x61c5, 0x61c7); A = 0x78; // ENEMY_PUMPKIN_HEAD
  CYC(0x61c7, 0x61c9); B = 0x00;
  CYC(0x61c9, 0x61cc); enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

static void pumpkinHead_state_spawner_commonInit(GB *gb) {
  CYC(0x6203, 0x6204); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [subid]++
  CYC(0x6204, 0x6206); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x6206, 0x6208); mem_wr(gb, HL, ENEMY_BASE); // ld (hl),Enemy.start
  CYC(0x6208, 0x6209); L = alu_inc8(gb, L);
  CYC(0x6209, 0x620a); mem_wr(gb, HL, C);
  CYC(0x620a, 0x620d); objectCopyPosition_hook(gb); return; // jp
}

// Subid 0 (spawner). Waits for doors to close, spawns body/ghost/head, deletes self.
void pumpkinHead_state_spawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x61cc, 0x61cf); A = mem_rd(gb, wcc93);
  CYC(0x61cf, 0x61d0); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x61d0); return; } // ret nz
  CYC(0x61d0, 0x61d1);
  CYC(0x61d1, 0x61d3); B = 0x03;
  CALL_C(0x61d3, checkBEnemySlotsAvailable_hook, 0x20f7, 0x61d6);
  if (!(F & FZ)) { RET_TAKEN(0x61d6); return; } // ret nz
  CYC(0x61d6, 0x61d7);
  CYC(0x61d7, 0x61d9); B = 0x78; // ENEMY_PUMPKIN_HEAD
  CALL_C(0x61d9, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x61dc);
  CALL_C(0x61dc, objectCopyPosition_hook, 0x2242, 0x61df);
  CYC(0x61df, 0x61e0); C = H;
  CALL_C(0x61e0, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x61e3);
  CYC(0x61e3, 0x61e6); push_effect(gb, 0x61e6); pumpkinHead_state_spawner_commonInit(gb);
  CYC(0x61e6, 0x61e8); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x61e8, 0x61e9); E = L;
  CYC(0x61e9, 0x61ea); A = mem_rd(gb, DE);
  CYC(0x61ea, 0x61eb); mem_wr(gb, HL, A);
  CYC(0x61eb, 0x61ec); A = H;
  CYC(0x61ec, 0x61ed); H = C;
  CYC(0x61ed, 0x61ef); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x61ef, 0x61f0); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x61f0, 0x61f2); mem_wr(gb, HL, ENEMY_BASE); // ld (hl),Enemy.start
  CALL_C(0x61f2, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x61f5);
  CYC(0x61f5, 0x61f6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x61f6, 0x61f9); push_effect(gb, 0x61f9); pumpkinHead_state_spawner_commonInit(gb);
  CYC(0x61f9, 0x61fa); A = H;
  CYC(0x61fa, 0x61fb); H = C;
  CYC(0x61fb, 0x61fd); L = ENEMY_BASE + OBJ_RELATED2 + 1; // Enemy.relatedObj2+1
  CYC(0x61fd, 0x61fe); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x61fe, 0x6200); mem_wr(gb, HL, ENEMY_BASE); // ld (hl),Enemy.start
  CYC(0x6200, 0x6203); enemyDelete_hook(gb); return; // jp
}

void pumpkinHead_state_grabbed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x620d, 0x620e); E = alu_inc8(gb, E);
  CYC(0x620e, 0x620f); A = mem_rd(gb, DE);
  CYC(0x620f, 0x6210); push_effect(gb, 0x6210);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == 0x6218) goto justGrabbed;
    if (target == 0x6245) goto beingHeld;
    if (target == 0x625a) goto released;
    if (target == 0x625b) goto atRest;
    HANDOFF(target);
  }

justGrabbed:
  CYC(0x6218, 0x6219); H = D;
  CYC(0x6219, 0x621a); L = E;
  CYC(0x621a, 0x621b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x621b, 0x621c); alu_xor(gb, A);
  CYC(0x621c, 0x621f); mem_wr(gb, wLinkGrabState2, A);
  CYC(0x621f, 0x6221); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6221, 0x6224); A = mem_rd(gb, w1Link_direction);
  CYC(0x6224, 0x6225); mem_wr(gb, HL, A);
  CYC(0x6225, 0x6227); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6227, 0x6229); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6229, 0x622a); A = mem_rd(gb, HL);
  CYC(0x622a, 0x622b); mem_wr(gb, DE, A);
  CYC(0x622b, 0x622d); A = OBJ_RELATED1 + 1; // Object.relatedObj1+1
  CALL_C(0x622d, objectGetRelatedObject1Var_hook, 0x2160, 0x6230);
  CYC(0x6230, 0x6231); H = mem_rd(gb, HL);
  CYC(0x6231, 0x6233); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6233, 0x6234); A = mem_rd(gb, HL);
  CYC(0x6234, 0x6236); mem_wr(gb, HL, 0x13);
  CYC(0x6236, 0x6238); alu_cp(gb, 0x13);
  if (F & FC) { CYCT(0x6238, 0x623a); goto L_6242; } // jr nc
  CYC(0x6238, 0x623a);
  CYC(0x623a, 0x623c); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x623c, 0x623e); mem_wr(gb, HL, 0xf8);
  CYC(0x623e, 0x6240); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x6240, 0x6242); mem_wr(gb, HL, 0xf4);

L_6242:
  CYC(0x6242, 0x6245); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  CYC(0x6245, 0x6248); A = mem_rd(gb, w1Link_direction);
  CYC(0x6248, 0x6249); H = D;
  CYC(0x6249, 0x624b); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x624b, 0x624c); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x624c); return; } // ret z
  CYC(0x624c, 0x624d);
  CYC(0x624d, 0x624e); mem_wr(gb, HL, A);
  CYC(0x624e, 0x6250); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6250, 0x6251); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6251, 0x6253); alu_and(gb, 0x03);
  CYC(0x6253, 0x6254); alu_add(gb, A); // add a
  CYC(0x6254, 0x6256); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6256, 0x6257); mem_wr(gb, HL, A);
  CYC(0x6257, 0x625a); enemySetAnimation_hook(gb); return; // jp

released:
  RET(0x625a); return; // ret

atRest:
  CYC(0x625b, 0x625d); A = OBJ_RELATED1 + 1; // Object.relatedObj1+1
  CALL_C(0x625d, objectGetRelatedObject1Var_hook, 0x2160, 0x6260);
  CYC(0x6260, 0x6261); H = mem_rd(gb, HL);
  CYC(0x6261, 0x6263); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6263, 0x6265); mem_wr(gb, HL, 0x15);
  CYC(0x6265, 0x6266); H = D;
  CYC(0x6266, 0x6268); mem_wr(gb, HL, 0x16);
  CYC(0x6268, 0x626b); objectSetVisiblec2_hook(gb); return; // jp
}

void pumpkinHead_state_stub_hook(GB *gb) {
  RET(0x626b); return; // ret
}

void pumpkinHead_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x626c, 0x626d); A = mem_rd(gb, DE);
  CYC(0x626d, 0x626f); alu_sub(gb, 0x08);
  CYC(0x626f, 0x6270); push_effect(gb, 0x6270);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == 0x6288) { pumpkinHead_body_state08_hook(gb); return; }
    if (target == 0x62a5) { pumpkinHead_body_state09_hook(gb); return; }
    if (target == 0x62c0) { pumpkinHead_body_state0a_hook(gb); return; }
    if (target == 0x62d2) { pumpkinHead_body_state0b_hook(gb); return; }
    if (target == 0x6307) { pumpkinHead_body_state0c_hook(gb); return; }
    if (target == 0x634d) { pumpkinHead_body_state0d_hook(gb); return; }
    if (target == 0x6393) { pumpkinHead_body_state0e_hook(gb); return; }
    if (target == 0x63b3) { pumpkinHead_body_state0f_hook(gb); return; }
    if (target == 0x63bf) { pumpkinHead_body_state10_hook(gb); return; }
    if (target == 0x63c0) { pumpkinHead_body_state11_hook(gb); return; }
    if (target == 0x63ce) { pumpkinHead_body_state12_hook(gb); return; }
    if (target == 0x63de) { pumpkinHead_body_state13_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void pumpkinHead_body_state08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6288, 0x628b); SET_BC(0x0106);
  CALL_C(0x628b, enemyBoss_spawnShadow_b0f_hook, 0x4534, 0x628e);
  if (!(F & FZ)) { RET_TAKEN(0x628e); return; } // ret nz
  CYC(0x628e, 0x628f);
  CYC(0x628f, 0x6290); H = D;
  CYC(0x6290, 0x6291); L = E;
  CYC(0x6291, 0x6292); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6292, 0x6294); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(0x6294, 0x6296); A = 0x01;
  CYC(0x6296, 0x6297); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x6297, 0x6298); mem_wr(gb, HL, A);
  CALL_C(0x6298, objectSetVisible83_hook, 0x1e72, 0x629b);
  CYC(0x629b, 0x629d); C = 0x08;
  CALL_C(0x629d, ecom_setZAboveScreen_b0f_hook, 0x4446, 0x62a0);
  CYC(0x62a0, 0x62a2); A = 0x0d;
  CYC(0x62a2, 0x62a5); enemySetAnimation_hook(gb); return; // jp
}

// Falling from ceiling
void pumpkinHead_body_state09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x62a5, 0x62a7); C = 0x10;
  CALL_C(0x62a7, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x62aa);
  if (!(F & FZ)) { RET_TAKEN(0x62aa); return; } // ret nz
  CYC(0x62aa, 0x62ab);
  CYC(0x62ab, 0x62ad); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x62ad, 0x62ae); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x62ae, 0x62b0); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x62b0, 0x62b2); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x62b2, 0x62b4); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x62b4, 0x62b6); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x62b6, 0x62b8); A = 0x1e; // 30
  pumpkinHead_body_shakeScreen_hook(gb); return; // fallthrough
}

void pumpkinHead_body_shakeScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62b8, setScreenShakeCounter_hook, 0x24bb, 0x62bb);
  CYC(0x62bb, 0x62bd); A = 0x70; // SND_DOORCLOSE
  CYC(0x62bd, 0x62c0); playSound_b00_hook(gb); return; // jp
}

// Waiting for head to catch up with body
void pumpkinHead_body_state0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x62c0, 0x62c2); A = OBJ_ZH; // Object.zh
  CALL_C(0x62c2, objectGetRelatedObject2Var_hook, 0x2164, 0x62c5);
  CYC(0x62c5, 0x62c6); A = mem_rd(gb, HL);
  CYC(0x62c6, 0x62c8); alu_cp(gb, 0xf0);
  if (F & FC) { RET_TAKEN(0x62c8); return; } // ret c
  CYC(0x62c8, 0x62c9);
  CALL_C(0x62c9, ecom_decCounter1_b0f_hook, 0x439a, 0x62cc);
  if (!(F & FZ)) { RET_TAKEN(0x62cc); return; } // ret nz
  CYC(0x62cc, 0x62cd);
  CALL_C(0x62cd, pumpkinHead_body_chooseRandomStompTimerAndCount_hook, 0x670f, 0x62d0);
  CYCT(0x62d0, 0x62d2); pumpkinHead_body_beginMoving_hook(gb); return; // jr
}

// Walking around
void pumpkinHead_body_state0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62d2, pumpkinHead_body_countdownUntilStomp_hook, 0x66fe, 0x62d5);
  if (F & FZ) { RET_TAKEN(0x62d5); return; } // ret z
  CYC(0x62d5, 0x62d6);
  CALL_C(0x62d6, ecom_decCounter1_b0f_hook, 0x439a, 0x62d9);
  if (F & FZ) { CYCT(0x62d9, 0x62db); pumpkinHead_body_chooseNextAction_hook(gb); return; } // jr z
  CYC(0x62d9, 0x62db);
  CALL_C(0x62db, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, 0x4156, 0x62de);
  if (F & FZ) { CYCT(0x62de, 0x62e0); pumpkinHead_body_chooseNextAction_hook(gb); return; } // jr z
  CYC(0x62de, 0x62e0);
  CYC(0x62e0, 0x62e3); enemyAnimate_hook(gb); return; // jp
}

void pumpkinHead_body_chooseNextAction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62e3, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x62e6);
  CYC(0x62e6, 0x62e8); alu_add(gb, 0x04);
  CYC(0x62e8, 0x62ea); alu_and(gb, 0x18);
  CYC(0x62ea, 0x62eb); B = A;
  CYC(0x62eb, 0x62ed); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x62ed, 0x62ee); A = mem_rd(gb, DE);
  CYC(0x62ee, 0x62ef); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x62ef, 0x62f1); pumpkinHead_body_beginMoving_hook(gb); return; } // jr nz
  CYC(0x62ef, 0x62f1);
  CALL_C(0x62f1, getRandomNumber_noPreserveVars_hook, 0x0453, 0x62f4);
  CYC(0x62f4, 0x62f6); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(0x62f6, 0x62f8); pumpkinHead_body_beginMoving_hook(gb); return; } // jr c
  CYC(0x62f6, 0x62f8);
  CALL_C(0x62f8, ecom_incState_b0f_hook, 0x4000, 0x62fb);
  CYC(0x62fb, 0x62fd); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x62fd, 0x62ff); mem_wr(gb, HL, 0x38);
  CYC(0x62ff, 0x6301); A = OBJ_STATE; // Object.state
  CALL_C(0x6301, objectGetRelatedObject2Var_hook, 0x2164, 0x6304);
  CYC(0x6304, 0x6305); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYCT(0x6305, 0x6307); pumpkinHead_body_updateAnimationFromAngle_hook(gb); return; // jr
}

// Head is firing projectiles; waiting for it to finish.
void pumpkinHead_body_state0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6307, ecom_decCounter1_b0f_hook, 0x439a, 0x630a);
  if (!(F & FZ)) { RET_TAKEN(0x630a); return; } // ret nz
  CYC(0x630a, 0x630b);
  pumpkinHead_body_beginMoving_hook(gb); return; // fallthrough
}

void pumpkinHead_body_beginMoving_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x630b, 0x630c); H = D;
  CYC(0x630c, 0x630e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x630e, 0x6310); mem_wr(gb, HL, 0x0b);
  CYC(0x6310, 0x6312); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6312, 0x6314); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(0x6314, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6317);
  CYC(0x6317, 0x6319); alu_and(gb, 0x0f);
  CYC(0x6319, 0x631c); SET_HL(0x6339); // pumpkinHead_body_walkDurations
  CYC(0x631c, 0x631d); pumpkinHead_addAToHl_from_rst(gb, 0x631d);
  CYC(0x631d, 0x631f); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x631f, 0x6320); A = mem_rd(gb, HL);
  CYC(0x6320, 0x6321); mem_wr(gb, DE, A);
  CALL_C(0x6321, ecom_setRandomCardinalAngle_b0f_hook, 0x43c6, 0x6324);
  pumpkinHead_body_updateAnimationFromAngle_hook(gb); return; // fallthrough
}

void pumpkinHead_body_updateAnimationFromAngle_hook(GB *gb) {
  CYC(0x6324, 0x6326); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6326, 0x6327); A = mem_rd(gb, DE);
  CYC(0x6327, 0x6329); A = alu_swap(gb, A);
  CYC(0x6329, 0x632a); alu_rlca(gb);
  CYC(0x632a, 0x632b); B = A;
  CYC(0x632b, 0x632e); SET_HL(0x6349); // pumpkinHead_body_collisionRadiusXVals
  CYC(0x632e, 0x632f); pumpkinHead_addAToHl_from_rst(gb, 0x632f);
  CYC(0x632f, 0x6331); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x6331, 0x6332); A = mem_rd(gb, HL);
  CYC(0x6332, 0x6333); mem_wr(gb, DE, A);
  CYC(0x6333, 0x6334); A = B;
  CYC(0x6334, 0x6336); alu_add(gb, 0x0b);
  CYC(0x6336, 0x6339); enemySetAnimation_hook(gb); return; // jp
}

// Preparing to stomp
void pumpkinHead_body_state0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x634d, ecom_decCounter1_b0f_hook, 0x439a, 0x6350);
  if (F & FZ) { CYCT(0x6350, 0x6352); pumpkinHead_body_beginStomp_hook(gb); return; } // jr z
  CYC(0x6350, 0x6352);
  CYC(0x6352, 0x6353); A = mem_rd(gb, HL);
  CYC(0x6353, 0x6354); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x6354); return; } // ret nc
  CYC(0x6354, 0x6355);
  CALL_C(0x6355, ecom_updateCardinalAngleTowardTarget_b0f_hook, 0x43b4, 0x6358);
  CYCT(0x6358, 0x635a); pumpkinHead_body_updateAnimationFromAngle_hook(gb); return; // jr
}

void pumpkinHead_body_beginStomp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x635a, 0x635c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x635c, 0x635e); mem_wr(gb, HL, 0x0e);
  CYC(0x635e, 0x6360); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6360, 0x6362); A = 0x60;
  CYC(0x6362, 0x6363); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6363, 0x6365); mem_wr(gb, HL, 0xfc); // speedZ = -0x3a0
  CYC(0x6365, 0x6367); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6367, 0x6369); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x6369, 0x636b); A = OBJ_STATE; // Object.state
  CALL_C(0x636b, objectGetRelatedObject1Var_hook, 0x2160, 0x636e);
  CYC(0x636e, 0x6370); mem_wr(gb, HL, 0x0c);
  CYC(0x6370, 0x6372); A = OBJ_STATE; // Object.state
  CALL_C(0x6372, objectGetRelatedObject2Var_hook, 0x2164, 0x6375);
  CYC(0x6375, 0x6377); mem_wr(gb, HL, 0x0e);
  CALL_C(0x6377, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x637a);
  CYC(0x637a, 0x637c); alu_add(gb, 0x04);
  CYC(0x637c, 0x637e); alu_and(gb, 0x18);
  CYC(0x637e, 0x6380); A = alu_swap(gb, A);
  CYC(0x6380, 0x6381); alu_rlca(gb);
  CYC(0x6381, 0x6382); B = A;
  CYC(0x6382, 0x6385); SET_HL(0x6349); // pumpkinHead_body_collisionRadiusXVals
  CYC(0x6385, 0x6386); pumpkinHead_addAToHl_from_rst(gb, 0x6386);
  CYC(0x6386, 0x6388); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x6388, 0x6389); A = mem_rd(gb, HL);
  CYC(0x6389, 0x638a); mem_wr(gb, DE, A);
  CYC(0x638a, 0x638b); A = B;
  CYC(0x638b, 0x638d); alu_add(gb, 0x0b);
  CALL_C(0x638d, enemySetAnimation_hook, 0x282b, 0x6390);
  CYC(0x6390, 0x6393); objectSetVisible81_hook(gb); return; // jp
}

// In midair during stomp
void pumpkinHead_body_state0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6393, 0x6395); C = 0x30;
  CALL_C(0x6395, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6398);
  if (!(F & FZ)) { CYCT(0x6398, 0x639b); ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook(gb); return; } // jp nz
  CYC(0x6398, 0x639b);
  CYC(0x639b, 0x639d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x639d, 0x639e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x639e, 0x63a0); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x63a0, 0x63a1); A = mem_rd(gb, DE);
  CYC(0x63a1, 0x63a2); A = alu_dec8(gb, A);
  CYC(0x63a2, 0x63a4); A = 0x0f;
  if (!(F & FZ)) { CYCT(0x63a4, 0x63a6); goto L_63a8; } // jr nz
  CYC(0x63a4, 0x63a6);
  CYC(0x63a6, 0x63a8); A = 0x1e;

L_63a8:
  CYC(0x63a8, 0x63aa); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x63aa, 0x63ab); mem_wr(gb, HL, A);
  CYC(0x63ab, 0x63ad); A = 0x14;
  CALL_C(0x63ad, pumpkinHead_body_shakeScreen_hook, 0x62b8, 0x63b0);
  CYC(0x63b0, 0x63b3); objectSetVisible83_hook(gb); return; // jp
}

// Landed after a stomp
void pumpkinHead_body_state0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63b3, ecom_decCounter1_b0f_hook, 0x439a, 0x63b6);
  if (!(F & FZ)) { RET_TAKEN(0x63b6); return; } // ret nz
  CYC(0x63b6, 0x63b7);
  CYC(0x63b7, 0x63b9); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x63b9, 0x63ba); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x63ba, 0x63bc); pumpkinHead_body_beginStomp_hook(gb); return; } // jr nz
  CYC(0x63ba, 0x63bc);
  CYC(0x63bc, 0x63bf); pumpkinHead_body_beginMoving_hook(gb); return; // jp
}

// Body has been destroyed
void pumpkinHead_body_state10_hook(GB *gb) {
  RET(0x63bf); return; // ret
}

// Head has moved up, body will now regenerate
void pumpkinHead_body_state11_hook(GB *gb) {
  CYC(0x63c0, 0x63c1); H = D;
  CYC(0x63c1, 0x63c2); L = E;
  CYC(0x63c2, 0x63c3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x63c3, 0x63c5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x63c5, 0x63c7); mem_wr(gb, HL, 0x08);
  CYC(0x63c7, 0x63c9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x63c9, 0x63cb); mem_wr(gb, HL, 0x10);
  CYC(0x63cb, 0x63ce); objectCreatePuff_hook(gb); return; // jp
}

// Delay before making body visible
void pumpkinHead_body_state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63ce, ecom_decCounter1_b0f_hook, 0x439a, 0x63d1);
  if (!(F & FZ)) { RET_TAKEN(0x63d1); return; } // ret nz
  CYC(0x63d1, 0x63d2);
  CYC(0x63d2, 0x63d4); mem_wr(gb, HL, 0x1e); // [counter1] = 30
  CYC(0x63d4, 0x63d5); L = E;
  CYC(0x63d5, 0x63d6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CALL_C(0x63d6, objectSetVisible83_hook, 0x1e72, 0x63d9);
  CYC(0x63d9, 0x63db); A = 0x0d;
  CYC(0x63db, 0x63de); enemySetAnimation_hook(gb); return; // jp
}

// Body has regenerated, waiting a moment before resuming
void pumpkinHead_body_state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63de, ecom_decCounter1_b0f_hook, 0x439a, 0x63e1);
  if (!(F & FZ)) { RET_TAKEN(0x63e1); return; } // ret nz
  CYC(0x63e1, 0x63e2);
  CYC(0x63e2, 0x63e4); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x63e4, 0x63e6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(0x63e6, pumpkinHead_body_chooseRandomStompTimerAndCount_hook, 0x670f, 0x63e9);
  CYC(0x63e9, 0x63ec); pumpkinHead_body_beginMoving_hook(gb); return; // jp
}

void pumpkinHead_ghost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x63ec, 0x63ed); A = mem_rd(gb, DE);
  CYC(0x63ed, 0x63ef); alu_sub(gb, 0x08);
  CYC(0x63ef, 0x63f0); push_effect(gb, 0x63f0);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == 0x6410) { pumpkinHead_ghost_state08_hook(gb); return; }
    if (target == 0x6434) { pumpkinHead_ghost_state09_hook(gb); return; }
    if (target == 0x6445) { pumpkinHead_ghost_state0a_hook(gb); return; }
    if (target == 0x6459) { pumpkinHead_ghost_state0b_hook(gb); return; }
    if (target == 0x6461) { pumpkinHead_ghost_state0c_hook(gb); return; }
    if (target == 0x6470) { pumpkinHead_ghost_state0d_hook(gb); return; }
    if (target == 0x647e) { pumpkinHead_ghost_state0e_hook(gb); return; }
    if (target == 0x6492) { pumpkinHead_ghost_state0f_hook(gb); return; }
    if (target == 0x649f) { pumpkinHead_ghost_state10_hook(gb); return; }
    if (target == 0x64ad) { pumpkinHead_ghost_state11_hook(gb); return; }
    if (target == 0x64b4) { pumpkinHead_ghost_state12_hook(gb); return; }
    if (target == 0x64b5) { pumpkinHead_ghost_state13_hook(gb); return; }
    if (target == 0x64d4) { pumpkinHead_ghost_state14_hook(gb); return; }
    if (target == 0x64eb) { pumpkinHead_ghost_state15_hook(gb); return; }
    if (target == 0x64f7) { pumpkinHead_ghost_state16_hook(gb); return; }
    if (target == 0x6530) { pumpkinHead_ghost_state17_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void pumpkinHead_ghost_state08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6410, 0x6411); H = D;
  CYC(0x6411, 0x6412); L = E;
  CYC(0x6412, 0x6413); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6413, 0x6415); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6415, 0x6417); mem_wr(gb, HL, 0x5e); // ENEMYCOLLISION_PUMPKIN_HEAD_GHOST
  CYC(0x6417, 0x6419); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(0x6419, 0x641b); A = 0x05;
  CYC(0x641b, 0x641c); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x641c, 0x641d); mem_wr(gb, HL, A);
  CYC(0x641d, 0x641f); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x641f, 0x6421); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6421, 0x6423); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6423, 0x6425); A = 0x06;
  CYC(0x6425, 0x6426); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6426, 0x6427); mem_wr(gb, HL, A);
  CALL_C(0x6427, objectSetVisible83_hook, 0x1e72, 0x642a);
  CYC(0x642a, 0x642c); C = 0x20;
  CALL_C(0x642c, ecom_setZAboveScreen_b0f_hook, 0x4446, 0x642f);
  CYC(0x642f, 0x6431); A = 0x0a;
  CYC(0x6431, 0x6434); enemySetAnimation_hook(gb); return; // jp
}

// Falling from ceiling. (Also called by "head" state 9.)
void pumpkinHead_ghost_state09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6434, 0x6436); C = 0x10;
  CALL_C(0x6436, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6439);
  CYC(0x6439, 0x643b); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x643b, 0x643c); A = mem_rd(gb, HL);
  CYC(0x643c, 0x643e); alu_cp(gb, 0xf0);
  if (F & FC) { RET_TAKEN(0x643e); return; } // ret c
  CYC(0x643e, 0x643f);
  CYC(0x643f, 0x6441); mem_wr(gb, HL, 0xf0);
  CYC(0x6441, 0x6443); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6443, 0x6444); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x6444); return; // ret
}

// Waiting for head to fall into place
void pumpkinHead_ghost_state0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6445, 0x6447); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(0x6447, objectGetRelatedObject1Var_hook, 0x2160, 0x644a);
  CYC(0x644a, 0x644b); H = mem_rd(gb, HL);
  CYC(0x644b, 0x644d); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x644d, 0x644e); A = mem_rd(gb, HL);
  CYC(0x644e, 0x6450); alu_cp(gb, 0xf0);
  if (!(F & FZ)) { RET_TAKEN(0x6450); return; } // ret nz
  CYC(0x6450, 0x6451);
  CYC(0x6451, 0x6453); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6453, 0x6455); A = 0x0b;
  CYC(0x6455, 0x6456); mem_wr(gb, DE, A);
  CALL_C(0x6456, objectSetInvisible_hook, 0x1e7b, 0x6459);
  pumpkinHead_ghost_state0b_hook(gb); return; // fallthrough
}

// Copy body's position
void pumpkinHead_ghost_state0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6459, 0x645b); A = OBJ_ENABLED; // Object.enabled
  CALL_C(0x645b, objectGetRelatedObject1Var_hook, 0x2160, 0x645e);
  CYC(0x645e, 0x6461); objectTakePosition_hook(gb); return; // jp
}

// Body just began stomping; is moving upward
void pumpkinHead_ghost_state0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6461, pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook, 0x672e, 0x6464);
  if (!(F & FZ)) { RET_TAKEN(0x6464); return; } // ret nz
  CYC(0x6464, 0x6465);
  CALL_C(0x6465, ecom_incState_b0f_hook, 0x4000, 0x6468);
  CYC(0x6468, 0x646a); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x646a, 0x646b); alu_xor(gb, A);
  CYC(0x646b, 0x646c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x646c, 0x646d); mem_wr(gb, HL, A);
  CALL_C(0x646d, objectSetVisible81_hook, 0x1e60, 0x6470);
  pumpkinHead_ghost_state0d_hook(gb); return; // fallthrough
}

// Body is stomping; moving downward
void pumpkinHead_ghost_state0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6470, 0x6472); C = 0x28;
  CALL_C(0x6472, pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook, 0x6740, 0x6475);
  if (F & FC) { RET_TAKEN(0x6475); return; } // ret c
  CYC(0x6475, 0x6476);
  CYC(0x6476, 0x6478); mem_wr(gb, HL, 0xf0); // [zh] = 0
  CYC(0x6478, 0x647a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x647a, 0x647b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x647b, 0x647e); objectSetVisible83_hook(gb); return; // jp
}

// Reached target z-position after stomping; waiting for head to catch up
void pumpkinHead_ghost_state0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x647e, 0x6480); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(0x6480, objectGetRelatedObject1Var_hook, 0x2160, 0x6483);
  CYC(0x6483, 0x6484); H = mem_rd(gb, HL);
  CYC(0x6484, 0x6486); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6486, 0x6487); A = mem_rd(gb, HL);
  CYC(0x6487, 0x6489); alu_cp(gb, 0xee);
  if (F & FC) { RET_TAKEN(0x6489); return; } // ret c
  CYC(0x6489, 0x648a);
  CYC(0x648a, 0x648c); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x648c, 0x648e); A = 0x0b;
  CYC(0x648e, 0x648f); mem_wr(gb, DE, A);
  CYC(0x648f, 0x6492); objectSetInvisible_hook(gb); return; // jp
}

// Body just destroyed
void pumpkinHead_ghost_state0f_hook(GB *gb) {
  CYC(0x6492, 0x6493); H = D;
  CYC(0x6493, 0x6494); L = E;
  CYC(0x6494, 0x6495); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6495, 0x6497); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6497, 0x6499); A = 0xe0;
  CYC(0x6499, 0x649a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x649a, 0x649c); mem_wr(gb, HL, 0xfe); // speedZ = -0x120
  CYC(0x649c, 0x649f); objectSetInvisible_hook(gb); return; // jp
}

// Falling to ground after body disappeared
void pumpkinHead_ghost_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x649f, 0x64a1); C = 0x28;
  CALL_C(0x64a1, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x64a4);
  if (!(F & FZ)) { RET_TAKEN(0x64a4); return; } // ret nz
  CYC(0x64a4, 0x64a5);
  CYC(0x64a5, 0x64a7); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x64a7, 0x64a8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x64a8, 0x64aa); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x64aa, 0x64ac); mem_wr(gb, HL, 0x08);
  RET(0x64ac); return; // ret
}

// Delay before going to next state?
void pumpkinHead_ghost_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x64ad, ecom_decCounter1_b0f_hook, 0x439a, 0x64b0);
  if (!(F & FZ)) { RET_TAKEN(0x64b0); return; } // ret nz
  CYC(0x64b0, 0x64b1);
  CYC(0x64b1, 0x64b2); L = E;
  CYC(0x64b2, 0x64b3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  RET(0x64b3); return; // ret
}

// Waiting for head to be picked up
void pumpkinHead_ghost_state12_hook(GB *gb) {
  RET(0x64b4); return; // ret
}

// Link just grabbed the head; ghost runs away
void pumpkinHead_ghost_state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x64b5, 0x64b6); H = D;
  CYC(0x64b6, 0x64b7); L = E;
  CYC(0x64b7, 0x64b8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x64b8, 0x64ba); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x64ba, 0x64bc); mem_wr(gb, HL, 0x3c); // 60
  CYC(0x64bc, 0x64be); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x64be, 0x64c0); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(0x64c0, 0x64c2); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x64c2, 0x64c3); alu_xor(gb, A);
  CYC(0x64c3, 0x64c4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x64c4, 0x64c5); mem_wr(gb, HL, A);
  CYC(0x64c5, 0x64c7); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x64c7, 0x64c9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(0x64c9, objectSetVisiblec2_hook, 0x1e45, 0x64cc);
  CALL_C(0x64cc, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, 0x43ab, 0x64cf);
  CYC(0x64cf, 0x64d1); A = 0x0a;
  CYC(0x64d1, 0x64d4); enemySetAnimation_hook(gb); return; // jp
}

// Falling to ground, then running away with angle computed earlier
void pumpkinHead_ghost_state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x64d4, 0x64d6); C = 0x20;
  CALL_C(0x64d6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x64d9);
  if (!(F & FZ)) { RET_TAKEN(0x64d9); return; } // ret nz
  CYC(0x64d9, 0x64da);
  CALL_C(0x64da, ecom_decCounter1_b0f_hook, 0x439a, 0x64dd);
  if (!(F & FZ)) { CYCT(0x64dd, 0x64df); goto L_64e5; } // jr nz
  CYC(0x64dd, 0x64df);
  CYC(0x64df, 0x64e1); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x64e1, 0x64e2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(0x64e2, objectSetVisible82_hook, 0x1e69, 0x64e5);

L_64e5:
  CALL_C(0x64e5, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, 0x4156, 0x64e8);
  CYC(0x64e8, 0x64eb); enemyAnimate_hook(gb); return; // jp
}

// Stopped running away, or head just landed on ground
void pumpkinHead_ghost_state15_hook(GB *gb) {
  CYC(0x64eb, 0x64ec); H = D;
  CYC(0x64ec, 0x64ed); L = E;
  CYC(0x64ed, 0x64ee); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x64ee, 0x64f0); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x64f0, 0x64f2); mem_wr(gb, HL, 0x78); // 120
  CYC(0x64f2, 0x64f4); A = 0x09;
  CYC(0x64f4, 0x64f7); enemySetAnimation_hook(gb); return; // jp
}

// After [counter1] frames, will choose which direction to move in next
void pumpkinHead_ghost_state16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x64f7, ecom_decCounter1_b0f_hook, 0x439a, 0x64fa);
  if (!(F & FZ)) { CYCT(0x64fa, 0x64fc); goto checkHeadOnGround; } // jr nz
  CYC(0x64fa, 0x64fc);
  CYC(0x64fc, 0x64fe); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(0x64fe, 0x64ff); L = E;
  CYC(0x64ff, 0x6500); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x6500, 0x6501); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [state] -= 2
  CALL_C(0x6501, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6504);
  CYC(0x6504, 0x6506); alu_and(gb, 0x1c);
  CYC(0x6506, 0x6508); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6508, 0x6509); mem_wr(gb, DE, A);
  CYCT(0x6509, 0x650b); goto setAnim; // jr

checkHeadOnGround:
  CYC(0x650b, 0x650d); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(0x650d, objectGetRelatedObject1Var_hook, 0x2160, 0x6510);
  CYC(0x6510, 0x6511); H = mem_rd(gb, HL);
  CYC(0x6511, 0x6513); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6513, 0x6514); A = mem_rd(gb, HL);
  CYC(0x6514, 0x6516); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x6516, 0x6519); enemyAnimate_hook(gb); return; } // jp z
  CYC(0x6516, 0x6519);
  CYC(0x6519, 0x651a); H = D;
  CYC(0x651a, 0x651b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [this.state]
  CYC(0x651b, 0x651d); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(0x651d, objectGetRelatedObject1Var_hook, 0x2160, 0x6520);
  CYC(0x6520, 0x6521); H = mem_rd(gb, HL);
  CYC(0x6521, 0x6523); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6523, 0x6525); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x6525, 0x6526); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6526, 0x6527); mem_wr(gb, DE, A);
  CYC(0x6527, 0x6528); L = alu_inc8(gb, L);
  CYC(0x6528, 0x6529); E = alu_inc8(gb, E);
  CYC(0x6529, 0x652a); A = mem_rd(gb, HL);
  CYC(0x652a, 0x652b); mem_wr(gb, DE, A);

setAnim:
  CYC(0x652b, 0x652d); A = 0x0a;
  CYC(0x652d, 0x6530); enemySetAnimation_hook(gb); return; // jp
}

// Moving toward head (or where head used to be)
void pumpkinHead_ghost_state17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6530, 0x6531); H = D;
  CYC(0x6531, 0x6533); L = ENEMY_BASE + 0x33; // Enemy.var33
  CALL_C(0x6533, ecom_readPositionVars_b0f_hook, 0x4439, 0x6536);
  CYC(0x6536, 0x6537); alu_sub(gb, C);
  CYC(0x6537, 0x6539); alu_add(gb, 0x08);
  CYC(0x6539, 0x653b); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(0x653b, 0x653d); goto moveTowardHead; } // jr nc
  CYC(0x653b, 0x653d);
  CYC(0x653d, 0x653f); A = H8(hFF8F);
  CYC(0x653f, 0x6540); alu_sub(gb, B);
  CYC(0x6540, 0x6542); alu_add(gb, 0x08);
  CYC(0x6542, 0x6544); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(0x6544, 0x6546); goto moveTowardHead; } // jr nc
  CYC(0x6544, 0x6546);
  CYC(0x6546, 0x6548); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(0x6548, objectGetRelatedObject1Var_hook, 0x2160, 0x654b);
  CYC(0x654b, 0x654c); H = mem_rd(gb, HL);
  CYC(0x654c, 0x654e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x654e, 0x654f); A = mem_rd(gb, HL);
  CYC(0x654f, 0x6551); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(0x6551); return; } // ret z
  CYC(0x6551, 0x6552);
  CYC(0x6552, 0x6554); mem_wr(gb, HL, 0x13);
  CYC(0x6554, 0x6555); H = D;
  CYC(0x6555, 0x6557); mem_wr(gb, HL, 0x0b);
  CYC(0x6557, 0x6559); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6559, 0x655b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x655b, 0x655e); objectSetInvisible_hook(gb); return; // jp

moveTowardHead:
  CALL_C(0x655e, ecom_moveTowardPosition_b0f_hook, 0x4430, 0x6561);
  CYC(0x6561, 0x6564); enemyAnimate_hook(gb); return; // jp
}

void pumpkinHead_head_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6564, 0x6565); A = mem_rd(gb, DE);
  CYC(0x6565, 0x6567); alu_sub(gb, 0x08);
  CYC(0x6567, 0x6568); push_effect(gb, 0x6568);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == 0x6586) { pumpkinHead_head_state08_hook(gb); return; }
    if (target == 0x65b8) { pumpkinHead_head_state09_hook(gb); return; }
    if (target == 0x65c4) { pumpkinHead_head_state0a_hook(gb); return; }
    if (target == 0x65f6) { pumpkinHead_head_state0b_hook(gb); return; }
    if (target == 0x660a) { pumpkinHead_head_state0c_hook(gb); return; }
    if (target == 0x6632) { pumpkinHead_head_state0d_hook(gb); return; }
    if (target == 0x663d) { pumpkinHead_head_state0e_hook(gb); return; }
    if (target == 0x6660) { pumpkinHead_head_state0f_hook(gb); return; }
    if (target == 0x666f) { pumpkinHead_head_state10_hook(gb); return; }
    if (target == 0x667c) { pumpkinHead_head_state11_hook(gb); return; }
    if (target == 0x668a) { pumpkinHead_head_state12_hook(gb); return; }
    if (target == 0x669d) { pumpkinHead_head_state13_hook(gb); return; }
    if (target == 0x66ad) { pumpkinHead_head_state14_hook(gb); return; }
    if (target == 0x66c3) { pumpkinHead_head_state15_hook(gb); return; }
    if (target == 0x66f0) { pumpkinHead_head_state16_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void pumpkinHead_head_state08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6586, 0x6587); H = D;
  CYC(0x6587, 0x6588); L = E;
  CYC(0x6588, 0x6589); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6589, 0x658b); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x658b, 0x658d); mem_wr(gb, HL, 0xff);
  CYC(0x658d, 0x658f); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x658f, 0x6591); mem_wr(gb, HL, 0x5d); // ENEMYCOLLISION_PUMPKIN_HEAD_HEAD
  CYC(0x6591, 0x6593); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6593, 0x6595); mem_wr(gb, HL, 0x06);
  CALL_C(0x6595, objectSetVisible82_hook, 0x1e69, 0x6598);
  CYC(0x6598, 0x659a); C = 0x30;
  CALL_C(0x659a, ecom_setZAboveScreen_b0f_hook, 0x4446, 0x659d);
  CYC(0x659d, 0x659f); A = 0x04;
  CYC(0x659f, 0x65a1); B = 0x00;
  pumpkinHead_head_setAnimation_hook(gb); return; // fallthrough
}

void pumpkinHead_head_setAnimation_hook(GB *gb) {
  CYC(0x65a1, 0x65a3); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x65a3, 0x65a4); mem_wr(gb, DE, A);
  CYC(0x65a4, 0x65a5); alu_add(gb, B);
  CYC(0x65a5, 0x65a6); B = A;
  CYC(0x65a6, 0x65a8); A = alu_srl(gb, A);
  CYC(0x65a8, 0x65ab); SET_HL(0x65b4); // @collisionRadiusXVals
  CYC(0x65ab, 0x65ac); pumpkinHead_addAToHl_from_rst(gb, 0x65ac);
  CYC(0x65ac, 0x65ae); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x65ae, 0x65af); A = mem_rd(gb, HL);
  CYC(0x65af, 0x65b0); mem_wr(gb, DE, A);
  CYC(0x65b0, 0x65b1); A = B;
  CYC(0x65b1, 0x65b4); enemySetAnimation_hook(gb); return; // jp
}

void pumpkinHead_head_state09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x65b8, pumpkinHead_ghost_state09_hook, 0x6434, 0x65bb);
  if (F & FC) { RET_TAKEN(0x65bb); return; } // ret c
  CYC(0x65bb, 0x65bc);
  CYC(0x65bc, 0x65be); A = 0x2e; // MUS_BOSS
  CYC(0x65be, 0x65c1); mem_wr(gb, wActiveMusic, A);
  CYC(0x65c1, 0x65c4); playSound_b00_hook(gb); return; // jp
}

// Head follows body. Called by other states.
void pumpkinHead_head_state0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x65c4, objectSetPriorityRelativeToLink_hook, 0x22dc, 0x65c7);
  CYC(0x65c7, 0x65c9); A = OBJ_ANIM_PARAMETER; // Object.animParameter
  CALL_C(0x65c9, objectGetRelatedObject1Var_hook, 0x2160, 0x65cc);
  CYC(0x65cc, 0x65cd); A = mem_rd(gb, HL);
  CYC(0x65cd, 0x65ce); push_effect(gb, HL);
  CYC(0x65ce, 0x65d1); SET_HL(0x65f0); // @headZOffsets
  CYC(0x65d1, 0x65d2); pumpkinHead_addDoubleIndexToHl_from_rst(gb, 0x65d2);
  CYC(0x65d2, 0x65d3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x65d3, 0x65d4); C = A;
  CYC(0x65d4, 0x65d6); B = 0x00;
  CYC(0x65d6, 0x65d7); A = mem_rd(gb, HL);
  SET_HL(pop_effect(gb));
  CYC(0x65d7, 0x65d8);
  CYC(0x65d8, 0x65d9); push_effect(gb, AF);
  CALL_C(0x65d9, objectTakePositionWithOffset_hook, 0x2277, 0x65dc);
  SET_AF(pop_effect(gb));
  CYC(0x65dc, 0x65dd);
  CYC(0x65dd, 0x65df); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x65df, 0x65e0); mem_wr(gb, DE, A);
  CYC(0x65e0, 0x65e2); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x65e2, 0x65e3); E = L;
  CYC(0x65e3, 0x65e4); A = mem_rd(gb, DE);
  CYC(0x65e4, 0x65e5); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x65e5, 0x65e8); enemyAnimate_hook(gb); return; } // jp z
  CYC(0x65e5, 0x65e8);
  CYC(0x65e8, 0x65e9); A = mem_rd(gb, HL);
  CYC(0x65e9, 0x65ea); mem_wr(gb, DE, A);
  CYC(0x65ea, 0x65eb); alu_rrca(gb);
  CYC(0x65eb, 0x65ec); alu_rrca(gb);
  CYC(0x65ec, 0x65ee); B = 0x00;
  CYCT(0x65ee, 0x65f0); pumpkinHead_head_setAnimation_hook(gb); return; // jr
}

// Preparing to fire projectiles
void pumpkinHead_head_state0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x65f6, 0x65f7); H = D;
  CYC(0x65f7, 0x65f8); L = E;
  CYC(0x65f8, 0x65f9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x65f9, 0x65fb); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x65fb, 0x65fd); mem_wr(gb, HL, 0x14); // 20
  CALL_C(0x65fd, pumpkinHead_head_state0a_hook, 0x65c4, 0x6600);
  CYC(0x6600, 0x6602); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6602, 0x6603); A = mem_rd(gb, DE);
  CYC(0x6603, 0x6604); alu_rrca(gb);
  CYC(0x6604, 0x6605); alu_rrca(gb);
  CYC(0x6605, 0x6607); B = 0x01;
  CYC(0x6607, 0x660a); pumpkinHead_head_setAnimation_hook(gb); return; // jp
}

// Delay before firing projectile
void pumpkinHead_head_state0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x660a, ecom_decCounter1_b0f_hook, 0x439a, 0x660d);
  if (!(F & FZ)) { CYCT(0x660d, 0x6610); objectSetPriorityRelativeToLink_hook(gb); return; } // jp nz
  CYC(0x660d, 0x6610);
  CYC(0x6610, 0x6612); mem_wr(gb, HL, 0x24); // [counter1] = 36
  CYC(0x6612, 0x6613); L = E;
  CYC(0x6613, 0x6614); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6614, 0x6616); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6616, 0x6617); A = mem_rd(gb, HL);
  CYC(0x6617, 0x6618); alu_rrca(gb);
  CYC(0x6618, 0x6619); alu_rrca(gb);
  CYC(0x6619, 0x661b); B = 0x00;
  CALL_C(0x661b, pumpkinHead_head_setAnimation_hook, 0x65a1, 0x661e);
  CALL_C(0x661e, getFreePartSlot_hook, 0x3e8e, 0x6621);
  if (!(F & FZ)) { RET_TAKEN(0x6621); return; } // ret nz
  CYC(0x6621, 0x6622);
  CYC(0x6622, 0x6624); mem_wr(gb, HL, 0x42); // PART_PUMPKIN_HEAD_PROJECTILE
  CYC(0x6624, 0x6626); L = PART_BASE + OBJ_ANGLE;
  CYC(0x6626, 0x6628); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6628, 0x6629); A = mem_rd(gb, DE);
  CYC(0x6629, 0x662a); mem_wr(gb, HL, A);
  CALL_C(0x662a, objectCopyPosition_hook, 0x2242, 0x662d);
  CYC(0x662d, 0x662f); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CYC(0x662f, 0x6632); playSound_b00_hook(gb); return; // jp
}

// Delay after firing projectile
void pumpkinHead_head_state0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6632, ecom_decCounter1_b0f_hook, 0x439a, 0x6635);
  if (!(F & FZ)) { CYCT(0x6635, 0x6638); objectSetPriorityRelativeToLink_hook(gb); return; } // jp nz
  CYC(0x6635, 0x6638);
  CYC(0x6638, 0x6639); L = E;
  CYC(0x6639, 0x663b); mem_wr(gb, HL, 0x0a);
  CYCT(0x663b, 0x663d); pumpkinHead_head_state0a_hook(gb); return; // jr
}

// Began a stomp; moving up
void pumpkinHead_head_state0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x663d, pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook, 0x672e, 0x6640);
  if (F & FZ) { CYCT(0x6640, 0x6642); goto movingDown; } // jr z
  CYC(0x6640, 0x6642);
  CYC(0x6642, 0x6644); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6644, 0x6645); E = L;
  CYC(0x6645, 0x6646); A = mem_rd(gb, DE);
  CYC(0x6646, 0x6647); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6647); return; } // ret z
  CYC(0x6647, 0x6648);
  CYC(0x6648, 0x6649); A = mem_rd(gb, HL);
  CYC(0x6649, 0x664a); mem_wr(gb, DE, A);
  CYC(0x664a, 0x664c); alu_add(gb, 0x04);
  CYC(0x664c, 0x664e); alu_and(gb, 0x18);
  CYC(0x664e, 0x664f); alu_rrca(gb);
  CYC(0x664f, 0x6650); alu_rrca(gb);
  CYC(0x6650, 0x6652); B = 0x00;
  CYC(0x6652, 0x6655); pumpkinHead_head_setAnimation_hook(gb); return; // jp

movingDown:
  CALL_C(0x6655, ecom_incState_b0f_hook, 0x4000, 0x6658);
  CYC(0x6658, 0x665a); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x665a, 0x665b); alu_xor(gb, A);
  CYC(0x665b, 0x665c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x665c, 0x665d); mem_wr(gb, HL, A);
  CALL_C(0x665d, objectSetVisible80_hook, 0x1e57, 0x6660);
  pumpkinHead_head_state0f_hook(gb); return; // fallthrough
}

// Body is stomping; moving down
void pumpkinHead_head_state0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6660, 0x6662); C = 0x20;
  CALL_C(0x6662, pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook, 0x6740, 0x6665);
  if (F & FC) { RET_TAKEN(0x6665); return; } // ret c
  CYC(0x6665, 0x6666);
  CYC(0x6666, 0x6668); mem_wr(gb, HL, 0xf0); // [zh] = 0
  CYC(0x6668, 0x666a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x666a, 0x666c); mem_wr(gb, HL, 0x0a);
  CYC(0x666c, 0x666f); objectSetVisible82_hook(gb); return; // jp
}

// Body just destroyed
void pumpkinHead_head_state10_hook(GB *gb) {
  CYC(0x666f, 0x6670); H = D;
  CYC(0x6670, 0x6671); L = E;
  CYC(0x6671, 0x6672); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6672, 0x6674); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6674, 0x6676); A = 0xe0;
  CYC(0x6676, 0x6677); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6677, 0x6679); mem_wr(gb, HL, 0xfe); // speedZ = -0x120
  CYC(0x6679, 0x667c); objectSetVisiblec2_hook(gb); return; // jp
}

// Head falling down after body destroyed
void pumpkinHead_head_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x667c, 0x667e); C = 0x20;
  CALL_C(0x667e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6681);
  if (!(F & FZ)) { RET_TAKEN(0x6681); return; } // ret nz
  CYC(0x6681, 0x6682);
  CYC(0x6682, 0x6684); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6684, 0x6685); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6685, 0x6687); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6687, 0x6689); mem_wr(gb, HL, 0x78); // 120
  RET(0x6689); return; // ret
}

// Head is grabbable for 120 frames
void pumpkinHead_head_state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x668a, ecom_decCounter1_b0f_hook, 0x439a, 0x668d);
  if (!(F & FZ)) { CYCT(0x668d, 0x6690); pumpkinHead_head_state16_hook(gb); return; } // jp nz
  CYC(0x668d, 0x6690);
  CYC(0x6690, 0x6691); L = E;
  CYC(0x6691, 0x6692); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6692, 0x6694); A = OBJ_RELATED1 + 1; // Object.relatedObj1+1
  CALL_C(0x6694, objectGetRelatedObject1Var_hook, 0x2160, 0x6697);
  CYC(0x6697, 0x6698); H = mem_rd(gb, HL);
  CYC(0x6698, 0x669a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x669a, 0x669c); mem_wr(gb, HL, 0x0b);
  RET(0x669c); return; // ret
}

// Ghost just re-entered head, or head timed out before Link grabbed it
void pumpkinHead_head_state13_hook(GB *gb) {
  CYC(0x669d, 0x669e); H = D;
  CYC(0x669e, 0x669f); L = E;
  CYC(0x669f, 0x66a0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x66a0, 0x66a2); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x66a2, 0x66a4); mem_wr(gb, HL, 0x10); // 16
  CYC(0x66a4, 0x66a6); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x66a6, 0x66a8); mem_wr(gb, HL, 0x0a);
  CYC(0x66a8, 0x66aa); A = 0x08;
  CYC(0x66aa, 0x66ad); enemySetAnimation_hook(gb); return; // jp
}

// Delay before moving back up, respawning body
void pumpkinHead_head_state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x66ad, ecom_decCounter1_b0f_hook, 0x439a, 0x66b0);
  if (!(F & FZ)) { RET_TAKEN(0x66b0); return; } // ret nz
  CYC(0x66b0, 0x66b1);
  CYC(0x66b1, 0x66b2); L = E;
  CYC(0x66b2, 0x66b3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x66b3, 0x66b5); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x66b5, 0x66b7); A = 0x00;
  CYC(0x66b7, 0x66b8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x66b8, 0x66ba); mem_wr(gb, HL, 0xfe); // speedZ = -0x200
  CYC(0x66ba, 0x66bc); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x66bc, 0x66be); mem_wr(gb, HL, 0x06);
  CYC(0x66be, 0x66c0); A = 0x04;
  CYC(0x66c0, 0x66c3); enemySetAnimation_hook(gb); return; // jp
}

// Head moving up
void pumpkinHead_head_state15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x66c3, 0x66c5); C = 0x20;
  CALL_C(0x66c5, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x66c8);
  CYC(0x66c8, 0x66ca); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x66ca, 0x66cb); A = mem_rd(gb, HL);
  CYC(0x66cb, 0x66cd); alu_cp(gb, 0xf1);
  if (!(F & FC)) { RET_TAKEN(0x66cd); return; } // ret nc
  CYC(0x66cd, 0x66ce);
  CYC(0x66ce, 0x66d0); mem_wr(gb, HL, 0xf0);
  CYC(0x66d0, 0x66d2); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x66d2, 0x66d4); mem_wr(gb, HL, 0x0a);
  CYC(0x66d4, 0x66d6); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x66d6, 0x66d8); mem_wr(gb, HL, 0x0c);
  CALL_C(0x66d8, objectSetVisible82_hook, 0x1e69, 0x66db);
  CYC(0x66db, 0x66dd); A = OBJ_STATE; // Object.state
  CALL_C(0x66dd, objectGetRelatedObject1Var_hook, 0x2160, 0x66e0);
  CYC(0x66e0, 0x66e2); mem_wr(gb, HL, 0x11);
  CALL_C(0x66e2, objectCopyPosition_hook, 0x2242, 0x66e5);
  CYC(0x66e5, 0x66e7); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x66e7, 0x66e9); mem_wr(gb, HL, 0x00);
  CYC(0x66e9, 0x66eb); A = 0x04;
  CYC(0x66eb, 0x66ed); B = 0x00;
  CYC(0x66ed, 0x66f0); pumpkinHead_head_setAnimation_hook(gb); return; // jp
}

// Head has just come to rest after being thrown.
// Called by other states (to make it grabbable).
void pumpkinHead_head_state16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x66f0, 0x66f2); A = OBJ_HEALTH; // Object.health
  CALL_C(0x66f2, objectGetRelatedObject1Var_hook, 0x2160, 0x66f5);
  CYC(0x66f5, 0x66f6); A = mem_rd(gb, HL);
  CYC(0x66f6, 0x66f7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x66f7); return; } // ret z
  CYC(0x66f7, 0x66f8);
  CALL_C(0x66f8, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x66fb);
  CYC(0x66fb, 0x66fe); objectPushLinkAwayOnCollision_hook(gb); return; // jp
}

// @param[out] zflag z if time to stomp
void pumpkinHead_body_countdownUntilStomp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x66fe, 0x6701); A = mem_rd(gb, wFrameCounter);
  CYC(0x6701, 0x6702); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x6702); return; } // ret c
  CYC(0x6702, 0x6703);
  CALL_C(0x6703, ecom_decCounter2_b0f_hook, 0x43a3, 0x6706);
  if (!(F & FZ)) { RET_TAKEN(0x6706); return; } // ret nz
  CYC(0x6706, 0x6707);
  CYC(0x6707, 0x6709); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6709, 0x670b); mem_wr(gb, HL, 0x0d);
  CYC(0x670b, 0x670d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x670d, 0x670f); mem_wr(gb, HL, 0x3c); // 60
  pumpkinHead_body_chooseRandomStompTimerAndCount_hook(gb); return; // fallthrough
}

// Randomly sets the duration until a stomp occurs, and the number of stomps to perform.
void pumpkinHead_body_chooseRandomStompTimerAndCount_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x670f, 0x6712); SET_BC(0x0701);
  CALL_C(0x6712, ecom_randomBitwiseAndBCE_b0f_hook, 0x434f, 0x6715);
  CYC(0x6715, 0x6716); A = B;
  CYC(0x6716, 0x6719); SET_HL(0x6726); // @counter2Vals
  CYC(0x6719, 0x671a); pumpkinHead_addAToHl_from_rst(gb, 0x671a);
  CYC(0x671a, 0x671c); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x671c, 0x671d); A = mem_rd(gb, HL);
  CYC(0x671d, 0x671e); mem_wr(gb, DE, A);
  CYC(0x671e, 0x6720); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6720, 0x6721); A = C;
  CYC(0x6721, 0x6723); alu_add(gb, 0x02);
  CYC(0x6723, 0x6724); mem_wr(gb, DE, A);
  CYC(0x6724, 0x6725); alu_xor(gb, A);
  RET(0x6725); return; // ret
}

// @param[out] zflag z if body is moving down
void pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x672e, 0x6730); A = OBJ_SPEED_Z + 1; // Object.speedZ+1
  CALL_C(0x6730, objectGetRelatedObject1Var_hook, 0x2160, 0x6733);
  CYC(0x6733, 0x6735); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6735); return; } // ret z
  CYC(0x6735, 0x6736);
  CALL_C(0x6736, objectTakePosition_hook, 0x2274, 0x6739);
  CYC(0x6739, 0x673b); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x673b, 0x673c); A = mem_rd(gb, DE);
  CYC(0x673c, 0x673e); alu_sub(gb, 0x10);
  CYC(0x673e, 0x673f); mem_wr(gb, DE, A);
  RET(0x673f); return; // ret
}

// @param c Gravity
// @param[out] hl Enemy.zh
// @param[out] cflag nc if reached target z-position
void pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6740, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6743);
  CYC(0x6743, 0x6745); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6745, 0x6746); A = mem_rd(gb, HL);
  CYC(0x6746, 0x6748); alu_cp(gb, 0xf0);
  if (!(F & FC)) { RET_TAKEN(0x6748); return; } // ret nc
  CYC(0x6748, 0x6749);
  CYC(0x6749, 0x674a); push_effect(gb, AF);
  CYC(0x674a, 0x674c); A = OBJ_ENABLED; // Object.enabled
  CALL_C(0x674c, objectGetRelatedObject1Var_hook, 0x2160, 0x674f);
  CALL_C(0x674f, objectTakePosition_hook, 0x2274, 0x6752);
  SET_AF(pop_effect(gb));
  CYC(0x6752, 0x6753);
  CYC(0x6753, 0x6755); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x6755, 0x6756); mem_wr(gb, DE, A);
  RET(0x6756); return; // ret
}

void pumpkinHead_noHealth_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6757, 0x6759); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6759, 0x675a); A = mem_rd(gb, DE);
  CYC(0x675a, 0x675b); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x675b, 0x675d); goto bodyHealthZero; } // jr z
  CYC(0x675b, 0x675d);
  CYC(0x675d, 0x675e); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x675e, 0x6760); goto ghostHealthZero; } // jr z
  CYC(0x675e, 0x6760);

headHealthZero:
  CALL_C(0x6760, objectCreatePuff_hook, 0x24c1, 0x6763);
  CYC(0x6763, 0x6764); H = D;
  CYC(0x6764, 0x6766); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6766, 0x6767); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6767, 0x6769); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x6769, 0x676b); goto pumpkinHeadDelete; } // jr nz
  CYC(0x6769, 0x676b);
  CYC(0x676b, 0x676c); A = mem_rd(gb, HL);
  CYC(0x676c, 0x676e); alu_cp(gb, 0x02);
  if (F & FC) CALL_C_CC(0x676e, dropLinkHeldItem_hook, 0x2c43, 0x6771); else CYC(0x676e, 0x6771); // call c

pumpkinHeadDelete:
  CYC(0x6771, 0x6774); enemyDelete_hook(gb); return; // jp

ghostHealthZero:
  CYC(0x6774, 0x6776); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6776, 0x6777); A = mem_rd(gb, DE);
  CYC(0x6777, 0x6778); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6778, 0x677a); goto L_6783; } // jr nz
  CYC(0x6778, 0x677a);
  CALL_C(0x677a, ecom_killRelatedObj1_b0f_hook, 0x446d, 0x677d);
  CYC(0x677d, 0x677f); L = ENEMY_BASE + OBJ_RELATED2 + 1; // Enemy.relatedObj2+1
  CYC(0x677f, 0x6780); H = mem_rd(gb, HL);
  CALL_C(0x6780, ecom_killObjectH_b0f_hook, 0x445e, 0x6783);

L_6783:
  CALL_C(0x6783, enemyBoss_dead_b0f_hook, 0x44f0, 0x6786);
  CYC(0x6786, 0x6787); alu_xor(gb, A);
  RET(0x6787); return; // ret

bodyHealthZero:
  CYC(0x6788, 0x678a); A = OBJ_HEALTH; // Object.health
  CALL_C(0x678a, objectGetRelatedObject1Var_hook, 0x2160, 0x678d);
  CYC(0x678d, 0x678e); A = mem_rd(gb, HL);
  CYC(0x678e, 0x678f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x678f, 0x6792); enemyDelete_hook(gb); return; } // jp z
  CYC(0x678f, 0x6792);
  CYC(0x6792, 0x6793); H = D;
  CYC(0x6793, 0x6795); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6795, 0x6797); mem_wr(gb, HL, 0x08);
  CYC(0x6797, 0x6799); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6799, 0x679b); mem_wr(gb, HL, 0x10);
  CYC(0x679b, 0x679d); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x679d, 0x679f); mem_wr(gb, HL, 0x00);
  CYC(0x679f, 0x67a1); A = OBJ_STATE; // Object.state
  CALL_C(0x67a1, objectGetRelatedObject1Var_hook, 0x2160, 0x67a4);
  CYC(0x67a4, 0x67a6); mem_wr(gb, HL, 0x0f);
  CYC(0x67a6, 0x67a8); A = OBJ_STATE; // Object.state
  CALL_C(0x67a8, objectGetRelatedObject2Var_hook, 0x2164, 0x67ab);
  CYC(0x67ab, 0x67ad); mem_wr(gb, HL, 0x10);
  CALL_C(0x67ad, objectCreatePuff_hook, 0x24c1, 0x67b0);
  CYC(0x67b0, 0x67b3); objectSetInvisible_hook(gb); return; // jp
}
