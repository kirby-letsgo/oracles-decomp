#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode78);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYCT(b_+7, b_+9); goto normalStatus; // jr

dead:
  CALL_C(b_+9, pumpkinHead_noHealth_hook, SYM(pumpkinHead_noHealth), b_+12);
  if (F & FZ) { RET_TAKEN(b_+12); return; } // ret z
  CYC(b_+12, b_+13);

normalStatus:
  CALL_C(b_+13, ecom_getSubidAndCpStateTo08_b0f_hook, SYM(ecom_getSubidAndCpStateTo08_b0f), b_+16);
  if (F & FC) { CYCT(b_+16, b_+18); goto commonState; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); B = alu_dec8(gb, B);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == SYM(pumpkinHead_body)) { pumpkinHead_body_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost)) { pumpkinHead_ghost_hook(gb); return; }
    if (target == SYM(pumpkinHead_head)) { pumpkinHead_head_hook(gb); return; }
    HANDOFF(target);
  }

commonState:
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == SYM(pumpkinHead_state_uninitialized)) { pumpkinHead_state_uninitialized_hook(gb); return; }
    if (target == SYM(pumpkinHead_state_spawner)) { pumpkinHead_state_spawner_hook(gb); return; }
    if (target == SYM(pumpkinHead_state_grabbed)) { pumpkinHead_state_grabbed_hook(gb); return; }
    if (target == SYM(pumpkinHead_state_stub)) { pumpkinHead_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

void pumpkinHead_state_uninitialized_hook(GB *gb) {
  BASE(pumpkinHead_state_uninitialized);
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+5); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(b_+2, b_+5);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+7, b_+9); A = 0x78; // ENEMY_PUMPKIN_HEAD
  CYC(b_+9, b_+11); B = 0x00;
  CYC(b_+11, b_+14); enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

static void pumpkinHead_state_spawner_commonInit(GB *gb) {
  BASE(pumpkinHead_state_spawner);
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [subid]++
  CYC(b_+56, b_+58); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+58, b_+60); mem_wr(gb, HL, ENEMY_BASE); // ld (hl),Enemy.start
  CYC(b_+60, b_+61); L = alu_inc8(gb, L);
  CYC(b_+61, b_+62); mem_wr(gb, HL, C);
  CYC(b_+62, b_+65); objectCopyPosition_hook(gb); return; // jp
}

// Subid 0 (spawner). Waits for doors to close, spawns body/ghost/head, deletes self.
void pumpkinHead_state_spawner_hook(GB *gb) {
  BASE(pumpkinHead_state_spawner);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); B = 0x03;
  CALL_C(b_+7, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); B = 0x78; // ENEMY_PUMPKIN_HEAD
  CALL_C(b_+13, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+16);
  CALL_C(b_+16, objectCopyPosition_hook, SYM(objectCopyPosition), b_+19);
  CYC(b_+19, b_+20); C = H;
  CALL_C(b_+20, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+23);
  CYC(b_+23, b_+26); push_effect(gb, b_+26); pumpkinHead_state_spawner_commonInit(gb);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+28, b_+29); E = L;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+32); A = H;
  CYC(b_+32, b_+33); H = C;
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+36, b_+38); mem_wr(gb, HL, ENEMY_BASE); // ld (hl),Enemy.start
  CALL_C(b_+38, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+41);
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+42, b_+45); push_effect(gb, b_+45); pumpkinHead_state_spawner_commonInit(gb);
  CYC(b_+45, b_+46); A = H;
  CYC(b_+46, b_+47); H = C;
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_RELATED2 + 1; // Enemy.relatedObj2+1
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+50, b_+52); mem_wr(gb, HL, ENEMY_BASE); // ld (hl),Enemy.start
  CYC(b_+52, b_+55); enemyDelete_hook(gb); return; // jp
}

void pumpkinHead_state_grabbed_hook(GB *gb) {
  BASE(pumpkinHead_state_grabbed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == b_+11) goto justGrabbed;
    if (target == b_+56) goto beingHeld;
    if (target == b_+77) goto released;
    if (target == b_+78) goto atRest;
    HANDOFF(target);
  }

justGrabbed:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+18, b_+20); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_direction);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+26, b_+28); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); A = OBJ_RELATED1 + 1; // Object.relatedObj1+1
  CALL_C(b_+32, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+35);
  CYC(b_+35, b_+36); H = mem_rd(gb, HL);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x13);
  CYC(b_+41, b_+43); alu_cp(gb, 0x13);
  if (F & FC) { CYCT(b_+43, b_+45); goto L_6242; } // jr nc
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0xf8);
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0xf4);

L_6242:
  CYC(b_+53, b_+56); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  CYC(b_+56, b_+59); A = mem_rd(gb, w1Link_direction);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+62, b_+63); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+63); return; } // ret z
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  CYC(b_+65, b_+67); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+67, b_+68); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+68, b_+70); alu_and(gb, 0x03);
  CYC(b_+70, b_+71); alu_add(gb, A); // add a
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+73, b_+74); mem_wr(gb, HL, A);
  CYC(b_+74, b_+77); enemySetAnimation_hook(gb); return; // jp

released:
  RET(b_+77); return; // ret

atRest:
  CYC(b_+78, b_+80); A = OBJ_RELATED1 + 1; // Object.relatedObj1+1
  CALL_C(b_+80, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+83);
  CYC(b_+83, b_+84); H = mem_rd(gb, HL);
  CYC(b_+84, b_+86); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x15);
  CYC(b_+88, b_+89); H = D;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x16);
  CYC(b_+91, b_+94); objectSetVisiblec2_hook(gb); return; // jp
}

void pumpkinHead_state_stub_hook(GB *gb) {
  BASE(pumpkinHead_state_stub);
  RET(b_+0); return; // ret
}

void pumpkinHead_body_hook(GB *gb) {
  BASE(pumpkinHead_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == SYM(pumpkinHead_body_state08)) { pumpkinHead_body_state08_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state09)) { pumpkinHead_body_state09_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state0a)) { pumpkinHead_body_state0a_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state0b)) { pumpkinHead_body_state0b_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state0c)) { pumpkinHead_body_state0c_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state0d)) { pumpkinHead_body_state0d_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state0e)) { pumpkinHead_body_state0e_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state0f)) { pumpkinHead_body_state0f_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state10)) { pumpkinHead_body_state10_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state11)) { pumpkinHead_body_state11_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state12)) { pumpkinHead_body_state12_hook(gb); return; }
    if (target == SYM(pumpkinHead_body_state13)) { pumpkinHead_body_state13_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void pumpkinHead_body_state08_hook(GB *gb) {
  BASE(pumpkinHead_body_state08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x0106);
  CALL_C(b_+3, enemyBoss_spawnShadow_b0f_hook, SYM(enemyBoss_spawnShadow_b0f), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CALL_C(b_+16, objectSetVisible83_hook, SYM(objectSetVisible83), b_+19);
  CYC(b_+19, b_+21); C = 0x08;
  CALL_C(b_+21, ecom_setZAboveScreen_b0f_hook, SYM(ecom_setZAboveScreen_b0f), b_+24);
  CYC(b_+24, b_+26); A = 0x0d;
  CYC(b_+26, b_+29); enemySetAnimation_hook(gb); return; // jp
}

// Falling from ceiling
void pumpkinHead_body_state09_hook(GB *gb) {
  BASE(pumpkinHead_body_state09);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+17, b_+19); A = 0x1e; // 30
  pumpkinHead_body_shakeScreen_hook(gb); return; // fallthrough
}

void pumpkinHead_body_shakeScreen_hook(GB *gb) {
  BASE(pumpkinHead_body_shakeScreen);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+3);
  CYC(b_+3, b_+5); A = 0x70; // SND_DOORCLOSE
  CYC(b_+5, b_+8); playSound_b00_hook(gb); return; // jp
}

// Waiting for head to catch up with body
void pumpkinHead_body_state0a_hook(GB *gb) {
  BASE(pumpkinHead_body_state0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ZH; // Object.zh
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0xf0);
  if (F & FC) { RET_TAKEN(b_+8); return; } // ret c
  CYC(b_+8, b_+9);
  CALL_C(b_+9, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CALL_C(b_+13, pumpkinHead_body_chooseRandomStompTimerAndCount_hook, SYM(pumpkinHead_body_chooseRandomStompTimerAndCount), b_+16);
  CYCT(b_+16, b_+18); pumpkinHead_body_beginMoving_hook(gb); return; // jr
}

// Walking around
void pumpkinHead_body_state0b_hook(GB *gb) {
  BASE(pumpkinHead_body_state0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, pumpkinHead_body_countdownUntilStomp_hook, SYM(pumpkinHead_body_countdownUntilStomp), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+7);
  if (F & FZ) { CYCT(b_+7, b_+9); pumpkinHead_body_chooseNextAction_hook(gb); return; } // jr z
  CYC(b_+7, b_+9);
  CALL_C(b_+9, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0f), b_+12);
  if (F & FZ) { CYCT(b_+12, b_+14); pumpkinHead_body_chooseNextAction_hook(gb); return; } // jr z
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); enemyAnimate_hook(gb); return; // jp
}

void pumpkinHead_body_chooseNextAction_hook(GB *gb) {
  BASE(pumpkinHead_body_chooseNextAction);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); pumpkinHead_body_beginMoving_hook(gb); return; } // jr nz
  CYC(b_+12, b_+14);
  CALL_C(b_+14, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+19, b_+21); pumpkinHead_body_beginMoving_hook(gb); return; } // jr c
  CYC(b_+19, b_+21);
  CALL_C(b_+21, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x38);
  CYC(b_+28, b_+30); A = OBJ_STATE; // Object.state
  CALL_C(b_+30, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+33);
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYCT(b_+34, b_+36); pumpkinHead_body_updateAnimationFromAngle_hook(gb); return; // jr
}

// Head is firing projectiles; waiting for it to finish.
void pumpkinHead_body_state0c_hook(GB *gb) {
  BASE(pumpkinHead_body_state0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  pumpkinHead_body_beginMoving_hook(gb); return; // fallthrough
}

void pumpkinHead_body_beginMoving_hook(GB *gb) {
  BASE(pumpkinHead_body_beginMoving);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0b);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(b_+9, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  CYC(b_+14, b_+17); SET_HL(SYM(pumpkinHead_body_walkDurations)); // pumpkinHead_body_walkDurations
  CYC(b_+17, b_+18); pumpkinHead_addAToHl_from_rst(gb, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, ecom_setRandomCardinalAngle_b0f_hook, SYM(ecom_setRandomCardinalAngle_b0f), SYM(pumpkinHead_body_updateAnimationFromAngle));
  pumpkinHead_body_updateAnimationFromAngle_hook(gb); return; // fallthrough
}

void pumpkinHead_body_updateAnimationFromAngle_hook(GB *gb) {
  BASE(pumpkinHead_body_updateAnimationFromAngle);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+10); SET_HL(SYM(pumpkinHead_body_collisionRadiusXVals)); // pumpkinHead_body_collisionRadiusXVals
  CYC(b_+10, b_+11); pumpkinHead_addAToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+18); alu_add(gb, 0x0b);
  CYC(b_+18, b_+21); enemySetAnimation_hook(gb); return; // jp
}

// Preparing to stomp
void pumpkinHead_body_state0d_hook(GB *gb) {
  BASE(pumpkinHead_body_state0d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); pumpkinHead_body_beginStomp_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; } // ret nc
  CYC(b_+7, b_+8);
  CALL_C(b_+8, ecom_updateCardinalAngleTowardTarget_b0f_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0f), b_+11);
  CYCT(b_+11, b_+13); pumpkinHead_body_updateAnimationFromAngle_hook(gb); return; // jr
}

void pumpkinHead_body_beginStomp_hook(GB *gb) {
  BASE(pumpkinHead_body_beginStomp);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x0e);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+6, b_+8); A = 0x60;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xfc); // speedZ = -0x3a0
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+15, b_+17); A = OBJ_STATE; // Object.state
  CALL_C(b_+17, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+20);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x0c);
  CYC(b_+22, b_+24); A = OBJ_STATE; // Object.state
  CALL_C(b_+24, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x0e);
  CALL_C(b_+29, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+32);
  CYC(b_+32, b_+34); alu_add(gb, 0x04);
  CYC(b_+34, b_+36); alu_and(gb, 0x18);
  CYC(b_+36, b_+38); A = alu_swap(gb, A);
  CYC(b_+38, b_+39); alu_rlca(gb);
  CYC(b_+39, b_+40); B = A;
  CYC(b_+40, b_+43); SET_HL(SYM(pumpkinHead_body_collisionRadiusXVals)); // pumpkinHead_body_collisionRadiusXVals
  CYC(b_+43, b_+44); pumpkinHead_addAToHl_from_rst(gb, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); A = B;
  CYC(b_+49, b_+51); alu_add(gb, 0x0b);
  CALL_C(b_+51, enemySetAnimation_hook, SYM(enemySetAnimation), b_+54);
  CYC(b_+54, b_+57); objectSetVisible81_hook(gb); return; // jp
}

// In midair during stomp
void pumpkinHead_body_state0e_hook(GB *gb) {
  BASE(pumpkinHead_body_state0e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+11, b_+13); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+17); A = 0x0f;
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_63a8; } // jr nz
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 0x1e;

L_63a8:
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+26); A = 0x14;
  CALL_C(b_+26, pumpkinHead_body_shakeScreen_hook, SYM(pumpkinHead_body_shakeScreen), b_+29);
  CYC(b_+29, b_+32); objectSetVisible83_hook(gb); return; // jp
}

// Landed after a stomp
void pumpkinHead_body_state0f_hook(GB *gb) {
  BASE(pumpkinHead_body_state0f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+7, b_+9); pumpkinHead_body_beginStomp_hook(gb); return; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); pumpkinHead_body_beginMoving_hook(gb); return; // jp
}

// Body has been destroyed
void pumpkinHead_body_state10_hook(GB *gb) {
  BASE(pumpkinHead_body_state10);
  RET(b_+0); return; // ret
}

// Head has moved up, body will now regenerate
void pumpkinHead_body_state11_hook(GB *gb) {
  BASE(pumpkinHead_body_state11);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x10);
  CYC(b_+11, b_+14); objectCreatePuff_hook(gb); return; // jp
}

// Delay before making body visible
void pumpkinHead_body_state12_hook(GB *gb) {
  BASE(pumpkinHead_body_state12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x1e); // [counter1] = 30
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CALL_C(b_+8, objectSetVisible83_hook, SYM(objectSetVisible83), b_+11);
  CYC(b_+11, b_+13); A = 0x0d;
  CYC(b_+13, b_+16); enemySetAnimation_hook(gb); return; // jp
}

// Body has regenerated, waiting a moment before resuming
void pumpkinHead_body_state13_hook(GB *gb) {
  BASE(pumpkinHead_body_state13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(b_+8, pumpkinHead_body_chooseRandomStompTimerAndCount_hook, SYM(pumpkinHead_body_chooseRandomStompTimerAndCount), b_+11);
  CYC(b_+11, b_+14); pumpkinHead_body_beginMoving_hook(gb); return; // jp
}

void pumpkinHead_ghost_hook(GB *gb) {
  BASE(pumpkinHead_ghost);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == SYM(pumpkinHead_ghost_state08)) { pumpkinHead_ghost_state08_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state09)) { pumpkinHead_ghost_state09_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state0a)) { pumpkinHead_ghost_state0a_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state0b)) { pumpkinHead_ghost_state0b_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state0c)) { pumpkinHead_ghost_state0c_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state0d)) { pumpkinHead_ghost_state0d_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state0e)) { pumpkinHead_ghost_state0e_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state0f)) { pumpkinHead_ghost_state0f_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state10)) { pumpkinHead_ghost_state10_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state11)) { pumpkinHead_ghost_state11_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state12)) { pumpkinHead_ghost_state12_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state13)) { pumpkinHead_ghost_state13_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state14)) { pumpkinHead_ghost_state14_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state15)) { pumpkinHead_ghost_state15_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state16)) { pumpkinHead_ghost_state16_hook(gb); return; }
    if (target == SYM(pumpkinHead_ghost_state17)) { pumpkinHead_ghost_state17_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void pumpkinHead_ghost_state08_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x5e); // ENEMYCOLLISION_PUMPKIN_HEAD_GHOST
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+9, b_+11); A = 0x05;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+19, b_+21); A = 0x06;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CALL_C(b_+23, objectSetVisible83_hook, SYM(objectSetVisible83), b_+26);
  CYC(b_+26, b_+28); C = 0x20;
  CALL_C(b_+28, ecom_setZAboveScreen_b0f_hook, SYM(ecom_setZAboveScreen_b0f), b_+31);
  CYC(b_+31, b_+33); A = 0x0a;
  CYC(b_+33, b_+36); enemySetAnimation_hook(gb); return; // jp
}

// Falling from ceiling. (Also called by "head" state 9.)
void pumpkinHead_ghost_state09_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state09);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0xf0);
  if (F & FC) { RET_TAKEN(b_+10); return; } // ret c
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xf0);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+16); return; // ret
}

// Waiting for head to fall into place
void pumpkinHead_ghost_state0a_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0xf0);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+16); A = 0x0b;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, objectSetInvisible_hook, SYM(objectSetInvisible), SYM(pumpkinHead_ghost_state0b));
  pumpkinHead_ghost_state0b_hook(gb); return; // fallthrough
}

// Copy body's position
void pumpkinHead_ghost_state0b_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ENABLED; // Object.enabled
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+8); objectTakePosition_hook(gb); return; // jp
}

// Body just began stomping; is moving upward
void pumpkinHead_ghost_state0c_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook, SYM(pumpkinHead_ghostOrHead_updatePositionWhileStompingUp), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CALL_C(b_+12, objectSetVisible81_hook, SYM(objectSetVisible81), SYM(pumpkinHead_ghost_state0d));
  pumpkinHead_ghost_state0d_hook(gb); return; // fallthrough
}

// Body is stomping; moving downward
void pumpkinHead_ghost_state0d_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x28;
  CALL_C(b_+2, pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook, SYM(pumpkinHead_ghostOrHead_updatePositionWhileStompingDown), b_+5);
  if (F & FC) { RET_TAKEN(b_+5); return; } // ret c
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xf0); // [zh] = 0
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+11, b_+14); objectSetVisible83_hook(gb); return; // jp
}

// Reached target z-position after stomping; waiting for head to catch up
void pumpkinHead_ghost_state0e_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state0e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0xee);
  if (F & FC) { RET_TAKEN(b_+11); return; } // ret c
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+16); A = 0x0b;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+20); objectSetInvisible_hook(gb); return; // jp
}

// Body just destroyed
void pumpkinHead_ghost_state0f_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+5, b_+7); A = 0xe0;
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xfe); // speedZ = -0x120
  CYC(b_+10, b_+13); objectSetInvisible_hook(gb); return; // jp
}

// Falling to ground after body disappeared
void pumpkinHead_ghost_state10_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x28;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x08);
  RET(b_+13); return; // ret
}

// Delay before going to next state?
void pumpkinHead_ghost_state11_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  RET(b_+6); return; // ret
}

// Waiting for head to be picked up
void pumpkinHead_ghost_state12_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state12);
  RET(b_+0); return; // ret
}

// Link just grabbed the head; ghost runs away
void pumpkinHead_ghost_state13_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state13);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3c); // 60
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(b_+20, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+23);
  CALL_C(b_+23, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0f), b_+26);
  CYC(b_+26, b_+28); A = 0x0a;
  CYC(b_+28, b_+31); enemySetAnimation_hook(gb); return; // jp
}

// Falling to ground, then running away with angle computed earlier
void pumpkinHead_ghost_state14_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state14);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_64e5; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(b_+14, objectSetVisible82_hook, SYM(objectSetVisible82), b_+17);

L_64e5:
  CALL_C(b_+17, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0f), b_+20);
  CYC(b_+20, b_+23); enemyAnimate_hook(gb); return; // jp
}

// Stopped running away, or head just landed on ground
void pumpkinHead_ghost_state15_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state15);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x78); // 120
  CYC(b_+7, b_+9); A = 0x09;
  CYC(b_+9, b_+12); enemySetAnimation_hook(gb); return; // jp
}

// After [counter1] frames, will choose which direction to move in next
void pumpkinHead_ghost_state16_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state16);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto checkHeadOnGround; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [state] -= 2
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x1c);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYCT(b_+18, b_+20); goto setAnim; // jr

checkHeadOnGround:
  CYC(b_+20, b_+22); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(b_+22, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+25);
  CYC(b_+25, b_+26); H = mem_rd(gb, HL);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+31, b_+34); enemyAnimate_hook(gb); return; } // jp z
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [this.state]
  CYC(b_+36, b_+38); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(b_+38, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+41);
  CYC(b_+41, b_+42); H = mem_rd(gb, HL);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+44, b_+46); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); L = alu_inc8(gb, L);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);

setAnim:
  CYC(b_+52, b_+54); A = 0x0a;
  CYC(b_+54, b_+57); enemySetAnimation_hook(gb); return; // jp
}

// Moving toward head (or where head used to be)
void pumpkinHead_ghost_state17_hook(GB *gb) {
  BASE(pumpkinHead_ghost_state17);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x33; // Enemy.var33
  CALL_C(b_+3, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x08);
  CYC(b_+9, b_+11); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(b_+11, b_+13); goto moveTowardHead; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = H8(hFF8F);
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CYC(b_+16, b_+18); alu_add(gb, 0x08);
  CYC(b_+18, b_+20); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(b_+20, b_+22); goto moveTowardHead; } // jr nc
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = OBJ_RELATED2 + 1; // Object.relatedObj2+1
  CALL_C(b_+24, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+27);
  CYC(b_+27, b_+28); H = mem_rd(gb, HL);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+33); return; } // ret z
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x13);
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x0b);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+43, b_+46); objectSetInvisible_hook(gb); return; // jp

moveTowardHead:
  CALL_C(b_+46, ecom_moveTowardPosition_b0f_hook, SYM(ecom_moveTowardPosition_b0f), b_+49);
  CYC(b_+49, b_+52); enemyAnimate_hook(gb); return; // jp
}

void pumpkinHead_head_hook(GB *gb) {
  BASE(pumpkinHead_head);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = pumpkinHead_jump_table(gb);
    if (target == SYM(pumpkinHead_head_state08)) { pumpkinHead_head_state08_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state09)) { pumpkinHead_head_state09_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state0a)) { pumpkinHead_head_state0a_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state0b)) { pumpkinHead_head_state0b_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state0c)) { pumpkinHead_head_state0c_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state0d)) { pumpkinHead_head_state0d_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state0e)) { pumpkinHead_head_state0e_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state0f)) { pumpkinHead_head_state0f_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state10)) { pumpkinHead_head_state10_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state11)) { pumpkinHead_head_state11_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state12)) { pumpkinHead_head_state12_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state13)) { pumpkinHead_head_state13_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state14)) { pumpkinHead_head_state14_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state15)) { pumpkinHead_head_state15_hook(gb); return; }
    if (target == SYM(pumpkinHead_head_state16)) { pumpkinHead_head_state16_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void pumpkinHead_head_state08_hook(GB *gb) {
  BASE(pumpkinHead_head_state08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0xff);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x5d); // ENEMYCOLLISION_PUMPKIN_HEAD_HEAD
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x06);
  CALL_C(b_+15, objectSetVisible82_hook, SYM(objectSetVisible82), b_+18);
  CYC(b_+18, b_+20); C = 0x30;
  CALL_C(b_+20, ecom_setZAboveScreen_b0f_hook, SYM(ecom_setZAboveScreen_b0f), b_+23);
  CYC(b_+23, b_+25); A = 0x04;
  CYC(b_+25, b_+27); B = 0x00;
  pumpkinHead_head_setAnimation_hook(gb); return; // fallthrough
}

void pumpkinHead_head_setAnimation_hook(GB *gb) {
  BASE(pumpkinHead_head_setAnimation);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+4); alu_add(gb, B);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); A = alu_srl(gb, A);
  CYC(b_+7, b_+10); SET_HL(b_+19); // @collisionRadiusXVals
  CYC(b_+10, b_+11); pumpkinHead_addAToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+19); enemySetAnimation_hook(gb); return; // jp
}

void pumpkinHead_head_state09_hook(GB *gb) {
  BASE(pumpkinHead_head_state09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, pumpkinHead_ghost_state09_hook, SYM(pumpkinHead_ghost_state09), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x2e; // MUS_BOSS
  CYC(b_+6, b_+9); mem_wr(gb, wActiveMusic, A);
  CYC(b_+9, b_+12); playSound_b00_hook(gb); return; // jp
}

// Head follows body. Called by other states.
void pumpkinHead_head_state0a_hook(GB *gb) {
  BASE(pumpkinHead_head_state0a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectSetPriorityRelativeToLink_hook, SYM(objectSetPriorityRelativeToLink), b_+3);
  CYC(b_+3, b_+5); A = OBJ_ANIM_PARAMETER; // Object.animParameter
  CALL_C(b_+5, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CYC(b_+10, b_+13); SET_HL(b_+44); // @headZOffsets
  CYC(b_+13, b_+14); pumpkinHead_addDoubleIndexToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+18); B = 0x00;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  SET_HL(pop_effect(gb));
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); push_effect(gb, AF);
  CALL_C(b_+21, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+24);
  SET_AF(pop_effect(gb));
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); E = L;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+33, b_+36); enemyAnimate_hook(gb); return; } // jp z
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+39); alu_rrca(gb);
  CYC(b_+39, b_+40); alu_rrca(gb);
  CYC(b_+40, b_+42); B = 0x00;
  CYCT(b_+42, b_+44); pumpkinHead_head_setAnimation_hook(gb); return; // jr
}

// Preparing to fire projectiles
void pumpkinHead_head_state0b_hook(GB *gb) {
  BASE(pumpkinHead_head_state0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x14); // 20
  CALL_C(b_+7, pumpkinHead_head_state0a_hook, SYM(pumpkinHead_head_state0a), b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_rrca(gb);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+17); B = 0x01;
  CYC(b_+17, b_+20); pumpkinHead_head_setAnimation_hook(gb); return; // jp
}

// Delay before firing projectile
void pumpkinHead_head_state0c_hook(GB *gb) {
  BASE(pumpkinHead_head_state0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); objectSetPriorityRelativeToLink_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x24); // [counter1] = 36
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_rrca(gb);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+17); B = 0x00;
  CALL_C(b_+17, pumpkinHead_head_setAnimation_hook, SYM(pumpkinHead_head_setAnimation), b_+20);
  CALL_C(b_+20, getFreePartSlot_hook, SYM(getFreePartSlot), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x42); // PART_PUMPKIN_HEAD_PROJECTILE
  CYC(b_+26, b_+28); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CALL_C(b_+32, objectCopyPosition_hook, SYM(objectCopyPosition), b_+35);
  CYC(b_+35, b_+37); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CYC(b_+37, b_+40); playSound_b00_hook(gb); return; // jp
}

// Delay after firing projectile
void pumpkinHead_head_state0d_hook(GB *gb) {
  BASE(pumpkinHead_head_state0d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); objectSetPriorityRelativeToLink_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x0a);
  CYCT(b_+9, b_+11); pumpkinHead_head_state0a_hook(gb); return; // jr
}

// Began a stomp; moving up
void pumpkinHead_head_state0e_hook(GB *gb) {
  BASE(pumpkinHead_head_state0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook, SYM(pumpkinHead_ghostOrHead_updatePositionWhileStompingUp), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto movingDown; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); E = L;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); alu_add(gb, 0x04);
  CYC(b_+15, b_+17); alu_and(gb, 0x18);
  CYC(b_+17, b_+18); alu_rrca(gb);
  CYC(b_+18, b_+19); alu_rrca(gb);
  CYC(b_+19, b_+21); B = 0x00;
  CYC(b_+21, b_+24); pumpkinHead_head_setAnimation_hook(gb); return; // jp

movingDown:
  CALL_C(b_+24, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+27);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+29, b_+30); alu_xor(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CALL_C(b_+32, objectSetVisible80_hook, SYM(objectSetVisible80), SYM(pumpkinHead_head_state0f));
  pumpkinHead_head_state0f_hook(gb); return; // fallthrough
}

// Body is stomping; moving down
void pumpkinHead_head_state0f_hook(GB *gb) {
  BASE(pumpkinHead_head_state0f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook, SYM(pumpkinHead_ghostOrHead_updatePositionWhileStompingDown), b_+5);
  if (F & FC) { RET_TAKEN(b_+5); return; } // ret c
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xf0); // [zh] = 0
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0a);
  CYC(b_+12, b_+15); objectSetVisible82_hook(gb); return; // jp
}

// Body just destroyed
void pumpkinHead_head_state10_hook(GB *gb) {
  BASE(pumpkinHead_head_state10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+5, b_+7); A = 0xe0;
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xfe); // speedZ = -0x120
  CYC(b_+10, b_+13); objectSetVisiblec2_hook(gb); return; // jp
}

// Head falling down after body destroyed
void pumpkinHead_head_state11_hook(GB *gb) {
  BASE(pumpkinHead_head_state11);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x78); // 120
  RET(b_+13); return; // ret
}

// Head is grabbable for 120 frames
void pumpkinHead_head_state12_hook(GB *gb) {
  BASE(pumpkinHead_head_state12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); pumpkinHead_head_state16_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+8, b_+10); A = OBJ_RELATED1 + 1; // Object.relatedObj1+1
  CALL_C(b_+10, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+13);
  CYC(b_+13, b_+14); H = mem_rd(gb, HL);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0b);
  RET(b_+18); return; // ret
}

// Ghost just re-entered head, or head timed out before Link grabbed it
void pumpkinHead_head_state13_hook(GB *gb) {
  BASE(pumpkinHead_head_state13);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10); // 16
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x0a);
  CYC(b_+11, b_+13); A = 0x08;
  CYC(b_+13, b_+16); enemySetAnimation_hook(gb); return; // jp
}

// Delay before moving back up, respawning body
void pumpkinHead_head_state14_hook(GB *gb) {
  BASE(pumpkinHead_head_state14);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+8, b_+10); A = 0x00;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xfe); // speedZ = -0x200
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x06);
  CYC(b_+17, b_+19); A = 0x04;
  CYC(b_+19, b_+22); enemySetAnimation_hook(gb); return; // jp
}

// Head moving up
void pumpkinHead_head_state15_hook(GB *gb) {
  BASE(pumpkinHead_head_state15);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0xf1);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xf0);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x0a);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0c);
  CALL_C(b_+21, objectSetVisible82_hook, SYM(objectSetVisible82), b_+24);
  CYC(b_+24, b_+26); A = OBJ_STATE; // Object.state
  CALL_C(b_+26, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x11);
  CALL_C(b_+31, objectCopyPosition_hook, SYM(objectCopyPosition), b_+34);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x00);
  CYC(b_+38, b_+40); A = 0x04;
  CYC(b_+40, b_+42); B = 0x00;
  CYC(b_+42, b_+45); pumpkinHead_head_setAnimation_hook(gb); return; // jp
}

// Head has just come to rest after being thrown.
// Called by other states (to make it grabbable).
void pumpkinHead_head_state16_hook(GB *gb) {
  BASE(pumpkinHead_head_state16);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_HEALTH; // Object.health
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CALL_C(b_+8, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+11);
  CYC(b_+11, b_+14); objectPushLinkAwayOnCollision_hook(gb); return; // jp
}

// @param[out] zflag z if time to stomp
void pumpkinHead_body_countdownUntilStomp_hook(GB *gb) {
  BASE(pumpkinHead_body_countdownUntilStomp);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CALL_C(b_+5, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x0d);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x3c); // 60
  pumpkinHead_body_chooseRandomStompTimerAndCount_hook(gb); return; // fallthrough
}

// Randomly sets the duration until a stomp occurs, and the number of stomps to perform.
void pumpkinHead_body_chooseRandomStompTimerAndCount_hook(GB *gb) {
  BASE(pumpkinHead_body_chooseRandomStompTimerAndCount);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x0701);
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0f_hook, SYM(ecom_randomBitwiseAndBCE_b0f), b_+6);
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+10); SET_HL(b_+23); // @counter2Vals
  CYC(b_+10, b_+11); pumpkinHead_addAToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+18); A = C;
  CYC(b_+18, b_+20); alu_add(gb, 0x02);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  RET(b_+22); return; // ret
}

// @param[out] zflag z if body is moving down
void pumpkinHead_ghostOrHead_updatePositionWhileStompingUp_hook(GB *gb) {
  BASE(pumpkinHead_ghostOrHead_updatePositionWhileStompingUp);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_SPEED_Z + 1; // Object.speedZ+1
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CALL_C(b_+8, objectTakePosition_hook, SYM(objectTakePosition), b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_sub(gb, 0x10);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return; // ret
}

// @param c Gravity
// @param[out] hl Enemy.zh
// @param[out] cflag nc if reached target z-position
void pumpkinHead_ghostOrHead_updatePositionWhileStompingDown_hook(GB *gb) {
  BASE(pumpkinHead_ghostOrHead_updatePositionWhileStompingDown);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0xf0);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); push_effect(gb, AF);
  CYC(b_+10, b_+12); A = OBJ_ENABLED; // Object.enabled
  CALL_C(b_+12, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+15);
  CALL_C(b_+15, objectTakePosition_hook, SYM(objectTakePosition), b_+18);
  SET_AF(pop_effect(gb));
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return; // ret
}

void pumpkinHead_noHealth_hook(GB *gb) {
  BASE(pumpkinHead_noHealth);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto bodyHealthZero; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto ghostHealthZero; } // jr z
  CYC(b_+7, b_+9);

headHealthZero:
  CALL_C(b_+9, objectCreatePuff_hook, SYM(objectCreatePuff), b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+16, b_+18); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto pumpkinHeadDelete; } // jr nz
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x02);
  if (F & FC) CALL_C_CC(b_+23, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+26); else CYC(b_+23, b_+26); // call c

pumpkinHeadDelete:
  CYC(b_+26, b_+29); enemyDelete_hook(gb); return; // jp

ghostHealthZero:
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_6783; } // jr nz
  CYC(b_+33, b_+35);
  CALL_C(b_+35, ecom_killRelatedObj1_b0f_hook, SYM(ecom_killRelatedObj1_b0f), b_+38);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_RELATED2 + 1; // Enemy.relatedObj2+1
  CYC(b_+40, b_+41); H = mem_rd(gb, HL);
  CALL_C(b_+41, ecom_killObjectH_b0f_hook, SYM(ecom_killObjectH_b0f), b_+44);

L_6783:
  CALL_C(b_+44, enemyBoss_dead_b0f_hook, SYM(enemyBoss_dead_b0f), b_+47);
  CYC(b_+47, b_+48); alu_xor(gb, A);
  RET(b_+48); return; // ret

bodyHealthZero:
  CYC(b_+49, b_+51); A = OBJ_HEALTH; // Object.health
  CALL_C(b_+51, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+54);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+56); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+56, b_+59); enemyDelete_hook(gb); return; } // jp z
  CYC(b_+56, b_+59);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x08);
  CYC(b_+64, b_+66); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x10);
  CYC(b_+68, b_+70); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x00);
  CYC(b_+72, b_+74); A = OBJ_STATE; // Object.state
  CALL_C(b_+74, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+77);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x0f);
  CYC(b_+79, b_+81); A = OBJ_STATE; // Object.state
  CALL_C(b_+81, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+84);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x10);
  CALL_C(b_+86, objectCreatePuff_hook, SYM(objectCreatePuff), b_+89);
  CYC(b_+89, b_+92); objectSetInvisible_hook(gb); return; // jp
}
