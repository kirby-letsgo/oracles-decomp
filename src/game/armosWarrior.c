#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void armosWarrior_state_uninitialized_hook(GB *gb);
void armosWarrior_state_spawner_hook(GB *gb);
void armosWarrior_state_stub_hook(GB *gb);
void armosWarrior_parent_hook(GB *gb);
void armosWarrior_parent_state8_hook(GB *gb);
void armosWarrior_parent_state9_hook(GB *gb);
void armosWarrior_parent_stateA_hook(GB *gb);
void armosWarrior_parent_stateB_hook(GB *gb);
void armosWarrior_parent_updateBoxMovement_hook(GB *gb);
void armosWarrior_parent_animate_hook(GB *gb);
void armosWarrior_parent_stateC_hook(GB *gb);
void armosWarrior_parent_stateD_hook(GB *gb);
void armosWarrior_parent_stateE_hook(GB *gb);
void armosWarrior_parent_stateF_hook(GB *gb);
void armosWarrior_parent_state10_hook(GB *gb);
void armosWarrior_shield_hook(GB *gb);
void armosWarrior_sword_hook(GB *gb);
void armosWarrior_sword_state8_hook(GB *gb);
void armosWarrior_sword_state9_hook(GB *gb);
void armosWarrior_sword_stateA_hook(GB *gb);
void armosWarrior_sword_stateB_hook(GB *gb);
void armosWarrior_sword_updatePosition_hook(GB *gb);
void armosWarrior_sword_stateC_hook(GB *gb);
void armosWarrior_shield_updatePosition_hook(GB *gb);
void armosWarrior_sword_updateCollisionBox_hook(GB *gb);
void armosWarrior_sword_setPositionAsHeld_hook(GB *gb);
void armosWarrior_sword_checkCollisionWithShield_hook(GB *gb);
void armosWarrior_parent_checkReachedTurningPoint_hook(GB *gb);
void armosWarrior_sword_checkWentTooFar_hook(GB *gb);
void armosWarrior_sword_playSlashSound_hook(GB *gb);
static void armosWarrior_checkIntersection_hook(GB *gb);
static void armosWarrior_checkPositionComponent_hook(GB *gb);

static uint16_t armosWarrior_jump_table(GB *gb) {
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

static void armosWarrior_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void armosWarrior_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// ENEMY_ARMOS_WARRIOR
//
// Variables (for parent only, subid 1):
//   var30: "Turn" direction (should be 8 or -8)
//   var31: Shield
//   var32: Sword
//
// Variables (for shield only, subid 2):
//   relatedObj1: parent
//   relatedObj2: shield
//   var30: Animation index (0 or 1)
//   var31: Animation base (multiple of 2, for broken shield animation)
//   var32: Hits until destruction
//
// Variables (for sword only, subid 3):
//   relatedObj1: parent
//   relatedObj2: shield
//   var30/var31: Target position
//   var32/var33: Base position (yh and xh are manipulated by the animation to fix their
//                collision box, so need to be reset to these values each frame)
//   var34: If nonzero, checks for collision with shield
// ==================================================================================================
void enemyCode73_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4c8f, 0x4c91); goto normalStatus; } // jr z
  CYC(0x4c8f, 0x4c91);
  CYC(0x4c91, 0x4c93); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4c93); return; } // ret c
  CYC(0x4c93, 0x4c94);
  if (!(F & FZ)) { CYCT(0x4c94, 0x4c96); goto normalStatus; } // jr nz
  CYC(0x4c94, 0x4c96);

  // ENEMYSTATUS_DEAD
  CYC(0x4c96, 0x4c98); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4c98, 0x4c99); A = mem_rd(gb, DE);
  CYC(0x4c99, 0x4c9a); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4c9a, 0x4c9d); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(0x4c9a, 0x4c9d);
  CYC(0x4c9d, 0x4c9e); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4c9e, 0x4ca0); goto delete_; } // jr nz
  CYC(0x4c9e, 0x4ca0);

  // Subid 2 (shield) just destroyed

  // Destroy sword
  CALL_C(0x4ca0, ecom_killRelatedObj2_b0f_hook, 0x4474, 0x4ca3);

  // Set some variables on parent
  CYC(0x4ca3, 0x4ca5); A = 0x04; // Object.state
  CALL_C(0x4ca5, objectGetRelatedObject1Var_hook, 0x2160, 0x4ca8);
  CYC(0x4ca8, 0x4caa); mem_wr(gb, HL, 0x0d);
  CYC(0x4caa, 0x4cac); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4cac, 0x4cae); mem_wr(gb, HL, 0x5a);
  CYC(0x4cae, 0x4cb0); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x4cb0, 0x4cb2); mem_wr(gb, HL, 0x60);

delete_:
  CYC(0x4cb2, 0x4cb5);
  enemyDelete_hook(gb); return; // jp

normalStatus:
  CALL_C(0x4cb5, ecom_getSubidAndCpStateTo08_b0f_hook, 0x4426, 0x4cb8);
  if (!(F & FC)) { CYCT(0x4cb8, 0x4cba); goto normalState; } // jr nc
  CYC(0x4cb8, 0x4cba);
  {
    CYC(0x4cba, 0x4cbb); push_effect(gb, 0x4cbb);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == 0x4cd4) { armosWarrior_state_uninitialized_hook(gb); return; }
    if (target == 0x4ce6) { armosWarrior_state_spawner_hook(gb); return; }
    if (target == 0x4d26) { armosWarrior_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x4ccb, 0x4ccc); B = alu_dec8(gb, B);
  CYC(0x4ccc, 0x4ccd); A = B;
  {
    CYC(0x4ccd, 0x4cce); push_effect(gb, 0x4cce);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == 0x4d27) { armosWarrior_parent_hook(gb); return; }
    if (target == 0x4eb6) { armosWarrior_shield_hook(gb); return; }
    if (target == 0x4f05) { armosWarrior_sword_hook(gb); return; }
    HANDOFF(target);
  }
}

void armosWarrior_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4cd4, 0x4cd5); A = B;
  CYC(0x4cd5, 0x4cd6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4cd6, 0x4cd9); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(0x4cd6, 0x4cd9);

  // Spawner only
  CYC(0x4cd9, 0x4cda); A = alu_inc8(gb, A);
  CYC(0x4cda, 0x4cdb); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x4cdb, 0x4cde); W8(wDisabledObjects) = A;
  CYC(0x4cde, 0x4ce1); W8(wMenuDisabled) = A;
  CYC(0x4ce1, 0x4ce3); A = 0x73; // ENEMY_ARMOS_WARRIOR
  CYC(0x4ce3, 0x4ce6);
  enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

void armosWarrior_state_spawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ce6, 0x4ce8); B = 0x03;
  CALL_C(0x4ce8, checkBEnemySlotsAvailable_hook, 0x20f7, 0x4ceb);
  if (!(F & FZ)) { RET_TAKEN(0x4ceb); return; } // ret nz
  CYC(0x4ceb, 0x4cec);
  CYC(0x4cec, 0x4cee); C = 0x0c;
  CALL_C(0x4cee, ecom_setZAboveScreen_b0f_hook, 0x4446, 0x4cf1);

  // Spawn parent
  CYC(0x4cf1, 0x4cf3); B = 0x73; // ENEMY_ARMOS_WARRIOR
  CALL_C(0x4cf3, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x4cf6);
  CYC(0x4cf6, 0x4cf7); C = H;

  // Spawn shield
  CALL_C(0x4cf7, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x4cfa);
  CYC(0x4cfa, 0x4cfb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [shield.subid] = 2

  // [shield.relatedObj1] = parent
  CYC(0x4cfb, 0x4cfd); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x4cfd, 0x4cff); A = ENEMY_BASE; // Enemy.start
  CYC(0x4cff, 0x4d00); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4d00, 0x4d01); mem_wr(gb, HL, C);
  CALL_C(0x4d01, objectCopyPosition_hook, 0x2242, 0x4d04);
  CYC(0x4d04, 0x4d05); push_effect(gb, HL);

  // Spawn sword
  CALL_C(0x4d05, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x4d08);
  CYC(0x4d08, 0x4d0a); mem_wr(gb, HL, 0x03); // [sword.subid] = 3

  // [sword.relatedObj1] = parent
  CYC(0x4d0a, 0x4d0c); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x4d0c, 0x4d0e); A = ENEMY_BASE; // Enemy.start
  CYC(0x4d0e, 0x4d0f); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4d0f, 0x4d10); mem_wr(gb, HL, C);
  CALL_C(0x4d10, objectCopyPosition_hook, 0x2242, 0x4d13);

  // [parent.var31] = shield
  // [parent.var32] = sword
  CYC(0x4d13, 0x4d14); B = H;
  CYC(0x4d14, 0x4d15); SET_HL(pop_effect(gb));
  CYC(0x4d15, 0x4d16); A = H;
  CYC(0x4d16, 0x4d17); H = C;
  CYC(0x4d17, 0x4d19); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4d19, 0x4d1a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4d1a, 0x4d1b); mem_wr(gb, HL, B);
  CALL_C(0x4d1b, objectCopyPosition_hook, 0x2242, 0x4d1e);

  // Transfer enabled byte to parent
  CYC(0x4d1e, 0x4d20); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x4d20, 0x4d21); E = L;
  CYC(0x4d21, 0x4d22); A = mem_rd(gb, DE);
  CYC(0x4d22, 0x4d23); mem_wr(gb, HL, A);
  CYC(0x4d23, 0x4d26);
  enemyDelete_hook(gb); return; // jp
}

void armosWarrior_state_stub_hook(GB *gb) {
  RET(0x4d26); return; // ret
}

void armosWarrior_parent_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4d27, 0x4d28); A = mem_rd(gb, DE);
  CYC(0x4d28, 0x4d2a); alu_sub(gb, 0x08);
  {
    CYC(0x4d2a, 0x4d2b); push_effect(gb, 0x4d2b);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == 0x4d3d) { armosWarrior_parent_state8_hook(gb); return; }
    if (target == 0x4d57) { armosWarrior_parent_state9_hook(gb); return; }
    if (target == 0x4dd0) { armosWarrior_parent_stateA_hook(gb); return; }
    if (target == 0x4dfa) { armosWarrior_parent_stateB_hook(gb); return; }
    if (target == 0x4e13) { armosWarrior_parent_stateC_hook(gb); return; }
    if (target == 0x4e2e) { armosWarrior_parent_stateD_hook(gb); return; }
    if (target == 0x4e66) { armosWarrior_parent_stateE_hook(gb); return; }
    if (target == 0x4e74) { armosWarrior_parent_stateF_hook(gb); return; }
    if (target == 0x4e9d) { armosWarrior_parent_state10_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for door to close
void armosWarrior_parent_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4d3d, 0x4d40); A = W8(wcc93);
  CYC(0x4d40, 0x4d41); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4d41); return; } // ret nz
  CYC(0x4d41, 0x4d42);
  CYC(0x4d42, 0x4d45); SET_BC(0x0108);
  CALL_C(0x4d45, enemyBoss_spawnShadow_b0f_hook, 0x4534, 0x4d48);
  if (!(F & FZ)) { RET_TAKEN(0x4d48); return; } // ret nz
  CYC(0x4d48, 0x4d49);
  CYC(0x4d49, 0x4d4a); H = D;
  CYC(0x4d4a, 0x4d4b); L = E;
  CYC(0x4d4b, 0x4d4c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4d4c, 0x4d4e); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4d4e, 0x4d50); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x4d50, 0x4d52); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4d52, 0x4d54); mem_wr(gb, HL, 0x60); // ENEMYCOLLISION_ARMOS_WARRIOR_PROTECTED
  CYC(0x4d54, 0x4d57);
  objectSetVisible82_hook(gb); return; // jp
}

// Cutscene before fight starts (falling from sky)
void armosWarrior_parent_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4d57, 0x4d58); E = alu_inc8(gb, E);
  CYC(0x4d58, 0x4d59); A = mem_rd(gb, DE);
  {
    CYC(0x4d59, 0x4d5a); push_effect(gb, 0x4d5a);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == 0x4d64) goto substate0;
    if (target == 0x4d84) goto substate1;
    if (target == 0x4d90) goto substate2;
    if (target == 0x4d9f) goto substate3;
    if (target == 0x4dba) goto substate4;
    HANDOFF(target);
  }

substate0:
  CYC(0x4d64, 0x4d66); C = 0x20;
  CALL_C(0x4d66, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4d69);
  if (!(F & FZ)) { RET_TAKEN(0x4d69); return; } // ret nz
  CYC(0x4d69, 0x4d6a);

  // Hit the ground
  CYC(0x4d6a, 0x4d6c); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4d6c, 0x4d6d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d6d, 0x4d6e); L = alu_inc8(gb, L);
  CYC(0x4d6e, 0x4d70); A = 0x1a;
  CYC(0x4d70, 0x4d71); mem_wr(gb, HL, A); // [counter1]
  CYC(0x4d71, 0x4d74); W8(wScreenShakeCounterY) = A;
  CYC(0x4d74, 0x4d77); W8(wScreenShakeCounterX) = A;

  // [sword.zh] = [parent.zh]
  CYC(0x4d77, 0x4d79); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4d79, 0x4d7a); H = mem_rd(gb, HL);
  CYC(0x4d7a, 0x4d7c); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x4d7c, 0x4d7d); E = L;
  CYC(0x4d7d, 0x4d7e); A = mem_rd(gb, DE);
  CYC(0x4d7e, 0x4d7f); mem_wr(gb, HL, A);
  CYC(0x4d7f, 0x4d81); A = 0x81; // SND_STRONG_POUND
  CYC(0x4d81, 0x4d84);
  playSound_b00_hook(gb); return; // jp

substate1:
  CALL_C(0x4d84, ecom_decCounter1_b0f_hook, 0x439a, 0x4d87);
  if (!(F & FZ)) { RET_TAKEN(0x4d87); return; } // ret nz
  CYC(0x4d87, 0x4d88);
  CYC(0x4d88, 0x4d89); L = E;
  CYC(0x4d89, 0x4d8a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(0x4d8a, 0x4d8d); SET_BC(0x2f01); // TX_2f01
  CYC(0x4d8d, 0x4d90);
  showText_hook(gb); return; // jp

substate2:
  CYC(0x4d90, 0x4d91); H = D;
  CYC(0x4d91, 0x4d92); L = E;
  CYC(0x4d92, 0x4d93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(0x4d93, 0x4d95); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4d95, 0x4d97); mem_wr(gb, HL, 0x1e);
  CALL_C(0x4d97, enemyBoss_beginMiniboss_b0f_hook, 0x4580, 0x4d9a);
  CYC(0x4d9a, 0x4d9c); A = 0x02;
  CYC(0x4d9c, 0x4d9f);
  enemySetAnimation_hook(gb); return; // jp

substate3:
  CALL_C(0x4d9f, ecom_decCounter1_b0f_hook, 0x439a, 0x4da2);
  if (!(F & FZ)) { RET_TAKEN(0x4da2); return; } // ret nz
  CYC(0x4da2, 0x4da3);
  CYC(0x4da3, 0x4da5); mem_wr(gb, HL, 0x46); // [counter1]
  CYC(0x4da5, 0x4da7); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4da7, 0x4da9); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x4da9, 0x4daa); L = E;
  CYC(0x4daa, 0x4dab); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]

  // [sword.yh] -= 2
  CYC(0x4dab, 0x4dad); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4dad, 0x4dae); H = mem_rd(gb, HL);
  CYC(0x4dae, 0x4db0); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4db0, 0x4db1); A = mem_rd(gb, HL);
  CYC(0x4db1, 0x4db3); alu_sub(gb, 0x02);
  CYC(0x4db3, 0x4db4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a

  // [sword.xh] -= 1
  CYC(0x4db4, 0x4db5); L = alu_inc8(gb, L);
  CYC(0x4db5, 0x4db6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

  CYC(0x4db6, 0x4db7); alu_xor(gb, A);
  CYC(0x4db7, 0x4dba);
  enemySetAnimation_hook(gb); return; // jp

// Sword moving up, parent moving down
substate4:
  CALL_C(0x4dba, ecom_decCounter1_b0f_hook, 0x439a, 0x4dbd);
  if (!(F & FZ)) { CYCT(0x4dbd, 0x4dbf); goto applySpeed; } // jr nz
  CYC(0x4dbd, 0x4dbf);
  CYC(0x4dbf, 0x4dc1); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4dc1, 0x4dc2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4dc2, 0x4dc4); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4dc4, 0x4dc6); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(0x4dc6, 0x4dc8); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4dc8, 0x4dca); mem_wr(gb, HL, 0x08);

applySpeed:
  CALL_C(0x4dca, objectApplySpeed_hook, 0x201d, 0x4dcd);
  CYC(0x4dcd, 0x4dd0);
  enemyAnimate_hook(gb); return; // jp
}

// Deciding which direction to move in next
void armosWarrior_parent_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // If the armos is moving directly toward his sword, reverse direction
  CYC(0x4dd0, 0x4dd2); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4dd2, 0x4dd3); A = mem_rd(gb, DE);
  CYC(0x4dd3, 0x4dd4); H = A;
  CYC(0x4dd4, 0x4dd6); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4dd6, 0x4dd7); B = mem_rd(gb, HL);
  CYC(0x4dd7, 0x4dd9); L = ENEMY_BASE + OBJ_XH;
  CYC(0x4dd9, 0x4dda); C = mem_rd(gb, HL);
  CALL_C(0x4dda, objectGetRelativeAngle_hook, 0x1ea4, 0x4ddd);
  CYC(0x4ddd, 0x4ddf); alu_add(gb, 0x04);
  CYC(0x4ddf, 0x4de1); alu_and(gb, 0x18);
  CYC(0x4de1, 0x4de2); B = A;
  CYC(0x4de2, 0x4de4); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4de4, 0x4de5); A = mem_rd(gb, DE);
  CYC(0x4de5, 0x4de6); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x4de6, 0x4de8); goto haveTurn; } // jr nz
  CYC(0x4de6, 0x4de8);

  // Reverse direction
  CYC(0x4de8, 0x4dea); alu_xor(gb, 0x10);
  CYC(0x4dea, 0x4deb); mem_wr(gb, DE, A);
  CYC(0x4deb, 0x4ded); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4ded, 0x4dee); A = mem_rd(gb, DE);
  CYC(0x4dee, 0x4def); alu_cpl(gb);
  CYC(0x4def, 0x4df0); A = alu_inc8(gb, A);
  CYC(0x4df0, 0x4df1); mem_wr(gb, DE, A);

haveTurn:
  CALL_C(0x4df1, ecom_incState_b0f_hook, 0x4000, 0x4df4);
  CYC(0x4df4, 0x4df6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4df6, 0x4df8); mem_wr(gb, HL, 0x4b);
  CYC(0x4df8, 0x4dfa);
  armosWarrior_parent_animate_hook(gb); return; // jr
}

// Moving in "box" pattern for [counter1] frames
void armosWarrior_parent_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4dfa, ecom_decCounter1_b0f_hook, 0x439a, 0x4dfd);
  if (!(F & FZ)) { CYCT(0x4dfd, 0x4dff); armosWarrior_parent_updateBoxMovement_hook(gb); return; } // jr nz
  CYC(0x4dfd, 0x4dff);
  CYC(0x4dff, 0x4e00); L = E;
  CYC(0x4e00, 0x4e01); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  armosWarrior_parent_updateBoxMovement_hook(gb); return; // fallthrough
}

void armosWarrior_parent_updateBoxMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e01, armosWarrior_parent_checkReachedTurningPoint_hook, 0x50d5, 0x4e04);
  if (!(F & FZ)) { CYCT(0x4e04, 0x4e06); armosWarrior_parent_animate_hook(gb); return; } // jr nz
  CYC(0x4e04, 0x4e06);

  // Hit one of the turning points in his movement pattern; turn 90 degrees
  CYC(0x4e06, 0x4e07); H = D;
  CYC(0x4e07, 0x4e09); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4e09, 0x4e0b); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4e0b, 0x4e0c); A = mem_rd(gb, DE);
  CYC(0x4e0c, 0x4e0d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4e0d, 0x4e0f); alu_and(gb, 0x18);
  CYC(0x4e0f, 0x4e10); mem_wr(gb, DE, A);
  armosWarrior_parent_animate_hook(gb); return; // fallthrough
}

void armosWarrior_parent_animate_hook(GB *gb) {
  CYC(0x4e10, 0x4e13);
  enemyAnimate_hook(gb); return; // jp
}

// Shield just hit
void armosWarrior_parent_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e13, enemyAnimate_hook, 0x2818, 0x4e16);
  CALL_C(0x4e16, ecom_decCounter1_b0f_hook, 0x439a, 0x4e19);
  if (!(F & FZ)) { CYCT(0x4e19, 0x4e1b); armosWarrior_parent_updateBoxMovement_hook(gb); return; } // jr nz
  CYC(0x4e19, 0x4e1b);
  CYC(0x4e1b, 0x4e1d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4e1d, 0x4e1f); mem_wr(gb, HL, 0x0a);

  // Set speed based on number of shield hits
  CYC(0x4e1f, 0x4e21); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4e21, 0x4e22); H = mem_rd(gb, HL);
  CYC(0x4e22, 0x4e24); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4e24, 0x4e25); A = mem_rd(gb, HL);
  CYC(0x4e25, 0x4e28); SET_HL(0x5154); // armosWarrior_parent_speedVals
  CYC(0x4e28, 0x4e29); armosWarrior_addAToHl_from_rst(gb, 0x4e29);
  CYC(0x4e29, 0x4e2b); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4e2b, 0x4e2c); A = mem_rd(gb, HL);
  CYC(0x4e2c, 0x4e2d); mem_wr(gb, DE, A);
  RET(0x4e2d); return; // ret
}

// Shield just destroyed
void armosWarrior_parent_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e2e, ecom_decCounter1_b0f_hook, 0x439a, 0x4e31);
  if (F & FZ) { CYCT(0x4e31, 0x4e33); goto gotoNextState; } // jr z
  CYC(0x4e31, 0x4e33);

  // Create debris at random offset every 8 frames
  CYC(0x4e33, 0x4e34); A = mem_rd(gb, HL);
  CYC(0x4e34, 0x4e36); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x4e36); return; } // ret nz
  CYC(0x4e36, 0x4e37);
  CALL_C(0x4e37, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4e3a);
  CYC(0x4e3a, 0x4e3b); C = A;
  CYC(0x4e3b, 0x4e3d); alu_and(gb, 0x70);
  CYC(0x4e3d, 0x4e3f); A = alu_swap(gb, A);
  CYC(0x4e3f, 0x4e41); alu_sub(gb, 0x04);
  CYC(0x4e41, 0x4e42); B = A;
  CYC(0x4e42, 0x4e43); A = C;
  CYC(0x4e43, 0x4e45); alu_and(gb, 0x0f);
  CYC(0x4e45, 0x4e46); C = A;
  CALL_C(0x4e46, getFreeInteractionSlot_hook, 0x3aef, 0x4e49);
  if (!(F & FZ)) { RET_TAKEN(0x4e49); return; } // ret nz
  CYC(0x4e49, 0x4e4a);
  CYC(0x4e4a, 0x4e4c); mem_wr(gb, HL, 0x06); // INTERAC_ROCKDEBRIS
  CYC(0x4e4c, 0x4e4f);
  objectCopyPositionWithOffset_hook(gb); return; // jp

gotoNextState:
  CYC(0x4e4f, 0x4e51); mem_wr(gb, HL, 0x1e); // [counter1]
  CYC(0x4e51, 0x4e52); L = E;
  CYC(0x4e52, 0x4e53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4e53, 0x4e55); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4e55, 0x4e57); mem_wr(gb, HL, 0x44); // ENEMYCOLLISION_STANDARD_MINIBOSS
  CYC(0x4e57, 0x4e59); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4e59, 0x4e5b); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x4e5b, 0x4e5e); SET_BC(0x2f02); // TX_2f02
  CALL_C(0x4e5e, showText_hook, 0x1872, 0x4e61);
  CYC(0x4e61, 0x4e63); A = 0x01;
  CYC(0x4e63, 0x4e66);
  enemySetAnimation_hook(gb); return; // jp
}

// Standing still before charging Link
void armosWarrior_parent_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e66, enemyAnimate_hook, 0x2818, 0x4e69);
  CALL_C(0x4e69, ecom_decCounter1_b0f_hook, 0x439a, 0x4e6c);
  if (!(F & FZ)) { CYCT(0x4e6c, 0x4e6e); armosWarrior_parent_animate_hook(gb); return; } // jr nz
  CYC(0x4e6c, 0x4e6e);
  CYC(0x4e6e, 0x4e70); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4e70, 0x4e71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4e71, 0x4e74);
  ecom_updateAngleTowardTarget_b0f_hook(gb); return; // jp
}

// Charging
void armosWarrior_parent_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e74, enemyAnimate_hook, 0x2818, 0x4e77);
  CYC(0x4e77, 0x4e79); A = 0x01;
  CALL_C(0x4e79, ecom_getSideviewAdjacentWallsBitset_b0f_hook, 0x420b, 0x4e7c);
  if (F & FZ) { CYCT(0x4e7c, 0x4e7f); objectApplySpeed_hook(gb); return; } // jp z
  CYC(0x4e7c, 0x4e7f);

  // Hit wall
  CALL_C(0x4e7f, ecom_incState_b0f_hook, 0x4000, 0x4e82);
  CYC(0x4e82, 0x4e84); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4e84, 0x4e85); A = mem_rd(gb, HL);
  CYC(0x4e85, 0x4e87); alu_xor(gb, 0x10);
  CYC(0x4e87, 0x4e88); mem_wr(gb, HL, A);
  CYC(0x4e88, 0x4e8a); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x4e8a, 0x4e8c); A = 0x80;
  CYC(0x4e8c, 0x4e8d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4e8d, 0x4e8f); mem_wr(gb, HL, 0xfe);
  CYC(0x4e8f, 0x4e91); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4e91, 0x4e93); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x4e93, 0x4e95); A = 0x1e;
  CALL_C(0x4e95, setScreenShakeCounter_hook, 0x24bb, 0x4e98);
  CYC(0x4e98, 0x4e9a); A = 0x81; // SND_STRONG_POUND
  CYC(0x4e9a, 0x4e9d);
  playSound_b00_hook(gb); return; // jp
}

// Recoiling from hitting wall
void armosWarrior_parent_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4e9d, enemyAnimate_hook, 0x2818, 0x4ea0);
  CYC(0x4ea0, 0x4ea2); C = 0x16;
  CALL_C(0x4ea2, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4ea5);
  if (!(F & FZ)) { CYCT(0x4ea5, 0x4ea8); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x4ea5, 0x4ea8);

  // Hit ground
  CYC(0x4ea8, 0x4ea9); H = D;
  CYC(0x4ea9, 0x4eab); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4eab, 0x4ead); mem_wr(gb, HL, 0x0e);
  CYC(0x4ead, 0x4eaf); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4eaf, 0x4eb1); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x4eb1, 0x4eb3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4eb3, 0x4eb5); mem_wr(gb, HL, 0x3c);
  RET(0x4eb5); return; // ret
}

void armosWarrior_shield_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4eb6, 0x4eb7); A = mem_rd(gb, DE);
  CYC(0x4eb7, 0x4eb9); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(0x4eb9, 0x4ebb); goto state8; } // jr z
  CYC(0x4eb9, 0x4ebb);

  // Delete self if no hits remaining
  CYC(0x4ebb, 0x4ebc); H = D;
  CYC(0x4ebc, 0x4ebe); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4ebe, 0x4ebf); A = mem_rd(gb, HL);
  CYC(0x4ebf, 0x4ec0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4ec0, 0x4ec3); ecom_killObjectH_b0f_hook(gb); return; } // jp z
  CYC(0x4ec0, 0x4ec3);
  CYC(0x4ec3, 0x4ec5); A = 0x21; // Object.animParameter
  CALL_C(0x4ec5, objectGetRelatedObject1Var_hook, 0x2160, 0x4ec8);
  CYC(0x4ec8, 0x4eca); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4eca, 0x4ecb); A = mem_rd(gb, DE);
  CYC(0x4ecb, 0x4ecc); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4ecc, 0x4ece); goto updatePos; } // jr z
  CYC(0x4ecc, 0x4ece);
  CYC(0x4ece, 0x4ecf); A = mem_rd(gb, HL);
  CYC(0x4ecf, 0x4ed0); mem_wr(gb, DE, A);
  CYC(0x4ed0, 0x4ed2); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4ed2, 0x4ed3); A = mem_rd(gb, DE);
  CYC(0x4ed3, 0x4ed4); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x4ed4, enemySetAnimation_hook, 0x282b, 0x4ed7);

updatePos:
  CYC(0x4ed7, 0x4eda);
  armosWarrior_shield_updatePosition_hook(gb); return; // jp

// Uninitialized
state8:
  CYC(0x4eda, 0x4edd); A = W8(wcc93);
  CYC(0x4edd, 0x4ede); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4ede); return; } // ret nz
  CYC(0x4ede, 0x4edf);
  CYC(0x4edf, 0x4ee0); H = D;
  CYC(0x4ee0, 0x4ee1); L = E;
  CYC(0x4ee1, 0x4ee2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4ee2, 0x4ee4); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4ee4, 0x4ee6); mem_wr(gb, HL, 0x61); // ENEMYCOLLISION_ARMOS_WARRIOR_SHIELD
  CYC(0x4ee6, 0x4ee8); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4ee8, 0x4eea); mem_wr(gb, HL, 0x03);

  // [shield.relatedObj2] = sword (parent.var32)
  CYC(0x4eea, 0x4eec); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x4eec, 0x4eed); H = mem_rd(gb, HL);
  CYC(0x4eed, 0x4eef); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4eef, 0x4ef1); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x4ef1, 0x4ef3); A = ENEMY_BASE; // Enemy.start
  CYC(0x4ef3, 0x4ef4); mem_wr(gb, DE, A);
  CYC(0x4ef4, 0x4ef5); E = alu_inc8(gb, E);
  CYC(0x4ef5, 0x4ef6); A = mem_rd(gb, HL);
  CYC(0x4ef6, 0x4ef7); mem_wr(gb, DE, A);

  CYC(0x4ef7, 0x4ef9); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4ef9, 0x4efb); A = 0x03;
  CYC(0x4efb, 0x4efc); mem_wr(gb, DE, A);

  CALL_C(0x4efc, enemySetAnimation_hook, 0x282b, 0x4eff);
  CALL_C(0x4eff, armosWarrior_shield_updatePosition_hook, 0x503a, 0x4f02);
  CYC(0x4f02, 0x4f05);
  objectSetVisible81_hook(gb); return; // jp
}

void armosWarrior_sword_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f05, 0x4f07); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4f07, 0x4f08); A = mem_rd(gb, DE);
  CYC(0x4f08, 0x4f0a); alu_cp(gb, 0x0b);
  if (F & FC) { CYC(0x4f0a, 0x4f0d); } else { CALL_C_CC(0x4f0a, armosWarrior_sword_playSlashSound_hook, 0x5167, 0x4f0d); } // call nc
  CYC(0x4f0d, 0x4f0f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4f0f, 0x4f10); A = mem_rd(gb, DE);
  CYC(0x4f10, 0x4f12); alu_sub(gb, 0x08);
  {
    CYC(0x4f12, 0x4f13); push_effect(gb, 0x4f13);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == 0x4f1d) { armosWarrior_sword_state8_hook(gb); return; }
    if (target == 0x4f42) { armosWarrior_sword_state9_hook(gb); return; }
    if (target == 0x4f7d) { armosWarrior_sword_stateA_hook(gb); return; }
    if (target == 0x4f9c) { armosWarrior_sword_stateB_hook(gb); return; }
    if (target == 0x4fe5) { armosWarrior_sword_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for door to close
void armosWarrior_sword_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f1d, 0x4f20); A = W8(wcc93);
  CYC(0x4f20, 0x4f21); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4f21); return; } // ret nz
  CYC(0x4f21, 0x4f22);
  CYC(0x4f22, 0x4f23); H = D;
  CYC(0x4f23, 0x4f24); L = E;
  CYC(0x4f24, 0x4f25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4f25, 0x4f27); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4f27, 0x4f29); mem_wr(gb, HL, 0x62); // ENEMYCOLLISION_ARMOS_WARRIOR_SWORD
  CYC(0x4f29, 0x4f2b); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4f2b, 0x4f2d); mem_wr(gb, HL, 0x05); // SPEED_20
  CALL_C(0x4f2d, armosWarrior_sword_setPositionAsHeld_hook, 0x507a, 0x4f30);

  // [sword.relatedObj2] = shield (parent.var31)
  CYC(0x4f30, 0x4f32); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4f32, 0x4f34); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x4f34, 0x4f36); A = ENEMY_BASE; // Enemy.start
  CYC(0x4f36, 0x4f37); mem_wr(gb, DE, A);
  CYC(0x4f37, 0x4f38); E = alu_inc8(gb, E);
  CYC(0x4f38, 0x4f39); A = mem_rd(gb, HL);
  CYC(0x4f39, 0x4f3a); mem_wr(gb, DE, A);

  CYC(0x4f3a, 0x4f3c); A = 0x09;
  CALL_C(0x4f3c, enemySetAnimation_hook, 0x282b, 0x4f3f);
  CYC(0x4f3f, 0x4f42);
  objectSetVisible80_hook(gb); return; // jp
}

// Waiting for initial cutscene to end, then moving upward before fight starts
void armosWarrior_sword_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f42, 0x4f44); A = 0x05; // Object.substate
  CALL_C(0x4f44, objectGetRelatedObject1Var_hook, 0x2160, 0x4f47);
  CYC(0x4f47, 0x4f48); A = mem_rd(gb, HL);
  CYC(0x4f48, 0x4f49); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4f49, 0x4f4c); armosWarrior_sword_setPositionAsHeld_hook(gb); return; } // jp z
  CYC(0x4f49, 0x4f4c);
  CYC(0x4f4c, 0x4f4e); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(0x4f4e); return; } // ret c
  CYC(0x4f4e, 0x4f4f);
  if (F & FZ) { CYCT(0x4f4f, 0x4f51); goto parentSubstate3; } // jr z
  CYC(0x4f4f, 0x4f51);
  CYC(0x4f51, 0x4f52); L = alu_dec8(gb, L);
  CYC(0x4f52, 0x4f53); A = mem_rd(gb, HL); // [parent.state]
  CYC(0x4f53, 0x4f55); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x4f55, 0x4f57); goto gotoStateA; } // jr nc
  CYC(0x4f55, 0x4f57);
  CALL_C(0x4f57, armosWarrior_sword_playSlashSound_hook, 0x5167, 0x4f5a);
  CYC(0x4f5a, 0x4f5d);
  enemyAnimate_hook(gb); return; // jp

gotoStateA:
  CYC(0x4f5d, 0x4f5e); H = D;
  CYC(0x4f5e, 0x4f5f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4f5f, 0x4f61); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4f61, 0x4f63); mem_wr(gb, HL, 0x01);

  // Save position
  CYC(0x4f63, 0x4f65); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4f65, 0x4f67); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4f67, 0x4f68); A = mem_rd(gb, DE);
  CYC(0x4f68, 0x4f69); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4f69, 0x4f6b); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4f6b, 0x4f6c); A = mem_rd(gb, DE);
  CYC(0x4f6c, 0x4f6d); mem_wr(gb, HL, A);
  RET(0x4f6d); return; // ret

parentSubstate3:
  CYC(0x4f6e, 0x4f6f); H = D;
  CYC(0x4f6f, 0x4f71); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4f71, 0x4f72); A = mem_rd(gb, HL);
  CYC(0x4f72, 0x4f73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f73, 0x4f74); alu_or(gb, A);
  CYC(0x4f74, 0x4f76); A = 0x0a;
  if (F & FZ) { CYCT(0x4f76, 0x4f79); enemySetAnimation_hook(gb); return; } // jp z
  CYC(0x4f76, 0x4f79);
  CYC(0x4f79, 0x4f7b); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4f7b, 0x4f7c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x4f7c); return; // ret
}

// Staying still before charging toward Link
void armosWarrior_sword_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4f7d, ecom_decCounter1_b0f_hook, 0x439a, 0x4f80);
  if (!(F & FZ)) { RET_TAKEN(0x4f80); return; } // ret nz
  CYC(0x4f80, 0x4f81);
  CYC(0x4f81, 0x4f82); L = E;
  CYC(0x4f82, 0x4f83); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4f83, 0x4f85); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4f85, 0x4f87); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x4f87, 0x4f89); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4f89, 0x4f8b); mem_wr(gb, HL, 0x96);

  // Write target position to var30/var31
  CYC(0x4f8b, 0x4f8d); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4f8d, 0x4f8f); A = hram_rd(gb, hEnemyTargetY & 0xff);
  CYC(0x4f8f, 0x4f90); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4f90, 0x4f92); A = hram_rd(gb, hEnemyTargetX & 0xff);
  CYC(0x4f92, 0x4f93); mem_wr(gb, HL, A);

  CALL_C(0x4f93, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x4f96);
  CALL_C(0x4f96, enemyAnimate_hook, 0x2818, 0x4f99);
  CYC(0x4f99, 0x4f9c);
  armosWarrior_sword_updateCollisionBox_hook(gb); return; // jp
}

// Charging toward target position
void armosWarrior_sword_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4f9c, armosWarrior_sword_checkCollisionWithShield_hook, 0x5085, 0x4f9f);
  CYC(0x4f9f, 0x4fa2); A = W8(wFrameCounter);
  CYC(0x4fa2, 0x4fa4); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x4fa4, 0x4fa6); goto checkWentTooFar; } // jr nz
  CYC(0x4fa4, 0x4fa6);

  // Update angle toward target position every 4 frames
  CYC(0x4fa6, 0x4fa7); H = D;
  CYC(0x4fa7, 0x4fa9); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(0x4fa9, ecom_readPositionVars_b0f_hook, 0x4439, 0x4fac);
  CALL_C(0x4fac, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x4faf);
  CYC(0x4faf, 0x4fb1); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4fb1, 0x4fb2); mem_wr(gb, DE, A);

checkWentTooFar:
  CALL_C(0x4fb2, armosWarrior_sword_checkWentTooFar_hook, 0x50fc, 0x4fb5);
  if (F & FC) { CYCT(0x4fb5, 0x4fb7); goto beginSlowingDown; } // jr c
  CYC(0x4fb5, 0x4fb7);

  // If within 28 pixels of target position, start slowing down
  CYC(0x4fb7, 0x4fb9); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4fb9, 0x4fba); A = mem_rd(gb, HL);
  CYC(0x4fba, 0x4fbb); alu_sub(gb, B);
  CYC(0x4fbb, 0x4fbd); alu_add(gb, 28);
  CYC(0x4fbd, 0x4fbf); alu_cp(gb, 57);
  if (!(F & FC)) { CYCT(0x4fbf, 0x4fc1); goto notSlowingDown; } // jr nc
  CYC(0x4fbf, 0x4fc1);
  CYC(0x4fc1, 0x4fc2); L = alu_inc8(gb, L);
  CYC(0x4fc2, 0x4fc3); A = mem_rd(gb, HL);
  CYC(0x4fc3, 0x4fc4); alu_sub(gb, C);
  CYC(0x4fc4, 0x4fc6); alu_add(gb, 28);
  CYC(0x4fc6, 0x4fc8); alu_cp(gb, 57);
  if (!(F & FC)) { CYCT(0x4fc8, 0x4fca); goto notSlowingDown; } // jr nc
  CYC(0x4fc8, 0x4fca);

beginSlowingDown:
  CYC(0x4fca, 0x4fcc); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4fcc, 0x4fcd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4fcd, 0x4fcf); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4fcf, 0x4fd1); mem_wr(gb, HL, 0x70);

notSlowingDown:
  CALL_C(0x4fd1, enemyAnimate_hook, 0x2818, 0x4fd4);
  armosWarrior_sword_updatePosition_hook(gb); return; // fallthrough
}

void armosWarrior_sword_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4fd4, ecom_applyVelocityForTopDownEnemy_b0f_hook, 0x4146, 0x4fd7);

  // Save position
  CYC(0x4fd7, 0x4fd8); H = D;
  CYC(0x4fd8, 0x4fda); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4fda, 0x4fdc); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4fdc, 0x4fdd); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4fdd, 0x4fde); mem_wr(gb, DE, A);
  CYC(0x4fde, 0x4fdf); E = alu_inc8(gb, E);
  CYC(0x4fdf, 0x4fe0); L = alu_inc8(gb, L);
  CYC(0x4fe0, 0x4fe1); A = mem_rd(gb, HL);
  CYC(0x4fe1, 0x4fe2); mem_wr(gb, DE, A);
  CYC(0x4fe2, 0x4fe5);
  armosWarrior_sword_updateCollisionBox_hook(gb); return; // jp
}

// Slowing down
void armosWarrior_sword_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4fe5, armosWarrior_sword_checkCollisionWithShield_hook, 0x5085, 0x4fe8);
  CALL_C(0x4fe8, ecom_decCounter1_b0f_hook, 0x439a, 0x4feb);
  if (F & FZ) { CYCT(0x4feb, 0x4fed); goto stoppedMoving; } // jr z
  CYC(0x4feb, 0x4fed);
  CYC(0x4fed, 0x4fee); A = mem_rd(gb, HL); // [counter1]
  CYC(0x4fee, 0x4ff0); A = alu_swap(gb, A);
  CYC(0x4ff0, 0x4ff1); alu_rrca(gb);
  CYC(0x4ff1, 0x4ff3); alu_and(gb, 0x03);
  CYC(0x4ff3, 0x4ff6); SET_HL(0x5150); // armosWarrior_sword_speedVals
  CYC(0x4ff6, 0x4ff7); armosWarrior_addAToHl_from_rst(gb, 0x4ff7);
  CYC(0x4ff7, 0x4ff9); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4ff9, 0x4ffa); A = mem_rd(gb, HL);
  CYC(0x4ffa, 0x4ffb); mem_wr(gb, DE, A);

  // Restore position (which was manipulated for shield collision detection)
  CYC(0x4ffb, 0x4ffc); H = D;
  CYC(0x4ffc, 0x4ffe); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4ffe, 0x5000); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5000, 0x5001); A = mem_rd(gb, DE);
  CYC(0x5001, 0x5002); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5002, 0x5003); E = alu_inc8(gb, E);
  CYC(0x5003, 0x5004); L = alu_inc8(gb, L);
  CYC(0x5004, 0x5005); A = mem_rd(gb, DE);
  CYC(0x5005, 0x5006); mem_wr(gb, HL, A);

  CYC(0x5006, 0x5008); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5008, 0x5009); A = mem_rd(gb, DE);
  CYC(0x5009, 0x500b); alu_cp(gb, 30);
  if (!(F & FC)) { CYCT(0x500b, 0x500d); goto haveDirection; } // jr nc
  CYC(0x500b, 0x500d);
  CYC(0x500d, 0x500e); alu_rrca(gb);

haveDirection:
  if (!(F & FC)) { CALL_C_CC(0x500e, enemyAnimate_hook, 0x2818, 0x5011); } else { CYC(0x500e, 0x5011); } // call nc
  CYC(0x5011, 0x5013);
  armosWarrior_sword_updatePosition_hook(gb); return; // jr

stoppedMoving:
  CYC(0x5013, 0x5015); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5015, 0x5016); A = mem_rd(gb, DE);
  CYC(0x5016, 0x5018); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x5018, 0x501a); goto atRest; } // jr z
  CYC(0x5018, 0x501a);
  CYC(0x501a, 0x501c); mem_wr(gb, HL, 0x02); // [counter1]
  CYC(0x501c, 0x501f);
  enemyAnimate_hook(gb); return; // jp

atRest:
  CYC(0x501f, 0x5021); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5021, 0x5023); mem_wr(gb, HL, 0x0a);
  CYC(0x5023, 0x5025); L = ENEMY_BASE + OBJ_VAR34;
  CYC(0x5025, 0x5027); mem_wr(gb, HL, 0x00);

  // Set counter1 (frames to rest) based on number of hits until shield destroyed
  CYC(0x5027, 0x5029); A = 0x32; // Object.var32
  CALL_C(0x5029, objectGetRelatedObject2Var_hook, 0x2164, 0x502c);
  CYC(0x502c, 0x502d); A = mem_rd(gb, HL);
  CYC(0x502d, 0x502f); A = alu_swap(gb, A);
  CYC(0x502f, 0x5030); alu_rlca(gb);
  CYC(0x5030, 0x5032); alu_add(gb, 30);
  CYC(0x5032, 0x5034); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5034, 0x5035); mem_wr(gb, DE, A);
  CYC(0x5035, 0x5037); A = 0x0a;
  CYC(0x5037, 0x503a);
  enemySetAnimation_hook(gb); return; // jp
}

// Shield copies parent's position plus an offset
void armosWarrior_shield_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x503a, 0x503c); A = 0x0b; // Object.yh
  CALL_C(0x503c, objectGetRelatedObject1Var_hook, 0x2160, 0x503f);
  CYC(0x503f, 0x5040); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) -- [parent.yh]
  CYC(0x5040, 0x5041); B = A;
  CYC(0x5041, 0x5042); L = alu_inc8(gb, L);
  CYC(0x5042, 0x5043); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) -- [parent.xh]
  CYC(0x5043, 0x5044); C = A;
  CYC(0x5044, 0x5045); L = alu_inc8(gb, L);
  CYC(0x5045, 0x5046); E = L;
  CYC(0x5046, 0x5047); A = mem_rd(gb, HL);
  CYC(0x5047, 0x5048); mem_wr(gb, DE, A); // [shield.zh] = [parent.zh]

  CYC(0x5048, 0x504a); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x504a, 0x504b); A = mem_rd(gb, DE);
  CYC(0x504b, 0x504e); SET_HL(0x512c); // armosWarrior_shield_YXOffsets
  CYC(0x504e, 0x504f); armosWarrior_addDoubleIndexToHl_from_rst(gb, 0x504f);
  CYC(0x504f, 0x5051); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5051, 0x5052); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5052, 0x5053); alu_add(gb, B);
  CYC(0x5053, 0x5054); mem_wr(gb, DE, A);
  CYC(0x5054, 0x5056); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5056, 0x5057); A = mem_rd(gb, HL);
  CYC(0x5057, 0x5058); alu_add(gb, C);
  CYC(0x5058, 0x5059); mem_wr(gb, DE, A);
  RET(0x5059); return; // ret
}

// Updates collisionRadiusY/X based on animParameter, also adds an offset to Y/X position.
void armosWarrior_sword_updateCollisionBox_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x505a, 0x505c); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x505c, 0x505d); A = mem_rd(gb, DE);
  CYC(0x505d, 0x505e); alu_add(gb, A); // add a
  CYC(0x505e, 0x5061); SET_HL(0x5130); // armosWarrior_sword_collisionBoxes
  CYC(0x5061, 0x5062); armosWarrior_addDoubleIndexToHl_from_rst(gb, 0x5062);
  CYC(0x5062, 0x5064); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5064, 0x5065); A = mem_rd(gb, DE);
  CYC(0x5065, 0x5066); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5066, 0x5068); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5068, 0x5069); mem_wr(gb, DE, A);
  CYC(0x5069, 0x506a); SET_HL(HL + 1); // inc hl
  CYC(0x506a, 0x506c); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x506c, 0x506d); A = mem_rd(gb, DE);
  CYC(0x506d, 0x506e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x506e, 0x5070); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5070, 0x5071); mem_wr(gb, DE, A);
  CYC(0x5071, 0x5072); SET_HL(HL + 1); // inc hl
  CYC(0x5072, 0x5074); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5074, 0x5075); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5075, 0x5076); mem_wr(gb, DE, A);
  CYC(0x5076, 0x5077); E = alu_inc8(gb, E);
  CYC(0x5077, 0x5078); A = mem_rd(gb, HL);
  CYC(0x5078, 0x5079); mem_wr(gb, DE, A);
  RET(0x5079); return; // ret
}

// Sets the sword's position assuming it's being held by the parent.
void armosWarrior_sword_setPositionAsHeld_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x507a, 0x507c); A = 0x0b; // Object.yh
  CALL_C(0x507c, objectGetRelatedObject1Var_hook, 0x2160, 0x507f);
  CYC(0x507f, 0x5082); SET_BC(0xf4fa);
  CYC(0x5082, 0x5085);
  objectTakePositionWithOffset_hook(gb); return; // jp
}

static void armosWarrior_checkIntersection_hook(GB *gb) {
  // b = [sword.collisionRadius] + [shield.collisionRadius]
  CYC(0x50c7, 0x50c8); E = L;
  CYC(0x50c8, 0x50c9); A = mem_rd(gb, DE);
  CYC(0x50c9, 0x50ca); alu_add(gb, mem_rd(gb, HL));
  CYC(0x50ca, 0x50cb); B = A;

  // a = [sword.pos] - [shield.pos]
  CYC(0x50cb, 0x50cc); L = C;
  CYC(0x50cc, 0x50cd); E = L;
  CYC(0x50cd, 0x50ce); A = mem_rd(gb, DE);
  CYC(0x50ce, 0x50cf); alu_sub(gb, mem_rd(gb, HL));

  CYC(0x50cf, 0x50d0); alu_add(gb, B);
  CYC(0x50d0, 0x50d2); B = alu_sla(gb, B);
  CYC(0x50d2, 0x50d3); B = alu_inc8(gb, B);
  CYC(0x50d3, 0x50d4); alu_cp(gb, B);
  RET(0x50d4); return; // ret
}

void armosWarrior_sword_checkCollisionWithShield_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5085, 0x5087); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x5087, 0x5088); A = mem_rd(gb, DE);
  CYC(0x5088, 0x5089); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x5089); return; } // ret z
  CYC(0x5089, 0x508a);

  // Check if sword and shield collide
  CYC(0x508a, 0x508c); A = 0x26; // Object.collisionRadiusY
  CALL_C(0x508c, objectGetRelatedObject2Var_hook, 0x2164, 0x508f);
  CYC(0x508f, 0x5091); C = ENEMY_BASE + OBJ_YH;
  CYC(0x5091, 0x5094); push_effect(gb, 0x5094); armosWarrior_checkIntersection_hook(gb);
  if (!(F & FC)) { RET_TAKEN(0x5094); return; } // ret nc
  CYC(0x5094, 0x5095);
  CYC(0x5095, 0x5097); C = ENEMY_BASE + OBJ_XH;
  CYC(0x5097, 0x5099); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x5099, 0x509c); push_effect(gb, 0x509c); armosWarrior_checkIntersection_hook(gb);
  if (!(F & FC)) { RET_TAKEN(0x509c); return; } // ret nc
  CYC(0x509c, 0x509d);

  // They've collided
  CYC(0x509d, 0x509f); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x509f, 0x50a1); A = 0x01;
  CYC(0x50a1, 0x50a2); mem_wr(gb, DE, A);

  // Set various variables on the shield
  CYC(0x50a2, 0x50a4); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x50a4, 0x50a6); mem_wr(gb, HL, 0x18);

  // [Hits until destruction]--
  CYC(0x50a6, 0x50a8); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x50a8, 0x50a9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

  CYC(0x50a9, 0x50ab); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x50ab, 0x50ac); A = mem_rd(gb, HL);
  CYC(0x50ac, 0x50ae); alu_add(gb, 0x02);
  CYC(0x50ae, 0x50af); mem_wr(gb, HL, A);

  // h = [shield.relatedObj1] = parent
  CYC(0x50af, 0x50b1); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x50b1, 0x50b2); H = mem_rd(gb, HL);

  CYC(0x50b2, 0x50b4); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x50b4, 0x50b6); mem_wr(gb, HL, 0x3c);

  CYC(0x50b6, 0x50b8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x50b8, 0x50ba); mem_wr(gb, HL, 0x0c);

  CYC(0x50ba, 0x50bc); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x50bc, 0x50be); mem_wr(gb, HL, 0x78); // SPEED_300

  CYC(0x50be, 0x50c0); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x50c0, 0x50c2); mem_wr(gb, HL, 0x18);

  CYC(0x50c2, 0x50c4); A = 0x63; // SND_BOSS_DAMAGE
  CYC(0x50c4, 0x50c7);
  playSound_b00_hook(gb); return; // jp
}

// The armos always moves in a "box" pattern in his first phase, this checks if he's
// reached one of the "corners" of the box where he must turn.
//
// @param[out] zflag z if hit a turning point
void armosWarrior_parent_checkReachedTurningPoint_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50d5, 0x50d7); B = 0x31;
  CYC(0x50d7, 0x50d9); E = ENEMY_BASE + OBJ_YH;
  CYC(0x50d9, 0x50da); A = mem_rd(gb, DE);
  CYC(0x50da, 0x50dc); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(0x50dc, 0x50de); goto hitCorner; } // jr c
  CYC(0x50dc, 0x50de);
  CYC(0x50de, 0x50e0); B = 0x7f;
  CYC(0x50e0, 0x50e2); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(0x50e2, 0x50e4); goto hitCorner; } // jr nc
  CYC(0x50e2, 0x50e4);
  CYC(0x50e4, 0x50e6); B = 0xbf;
  CYC(0x50e6, 0x50e8); E = ENEMY_BASE + OBJ_XH;
  CYC(0x50e8, 0x50e9); A = mem_rd(gb, DE);
  CYC(0x50e9, 0x50eb); alu_cp(gb, 0xc0);
  if (!(F & FC)) { CYCT(0x50eb, 0x50ed); goto hitCorner; } // jr nc
  CYC(0x50eb, 0x50ed);
  CYC(0x50ed, 0x50ef); B = 0x31;
  CYC(0x50ef, 0x50f1); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(0x50f1, 0x50f3); goto hitCorner; } // jr c
  CYC(0x50f1, 0x50f3);
  CALL_C(0x50f3, objectApplySpeed_hook, 0x201d, 0x50f6);
  CYC(0x50f6, 0x50f7); alu_or(gb, D);
  RET(0x50f7); return; // ret

hitCorner:
  CYC(0x50f8, 0x50f9); A = B;
  CYC(0x50f9, 0x50fa); mem_wr(gb, DE, A);
  CYC(0x50fa, 0x50fb); alu_xor(gb, A);
  RET(0x50fb); return; // ret
}

static void armosWarrior_checkPositionComponent_hook(GB *gb) {
  // If bit 0 of the data structure is set, it's an upper / left boundary
  CYC(0x5123, 0x5125); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x5125, 0x5127); goto boundary; } // jr nz
  CYC(0x5125, 0x5127);
  CYC(0x5127, 0x5128); alu_cp(gb, E);
  RET(0x5128); return; // ret

boundary:
  CYC(0x5129, 0x512a); alu_cp(gb, E);
  CYC(0x512a, 0x512b); alu_ccf(gb);
  RET(0x512b); return; // ret
}

// @param[out] bc Position of sword
// @param[out] cflag c if the sword has gone to far and should stop now
void armosWarrior_sword_checkWentTooFar_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Fix position, store it in bc
  CYC(0x50fc, 0x50fd); H = D;
  CYC(0x50fd, 0x50ff); L = ENEMY_BASE + OBJ_YH;
  CYC(0x50ff, 0x5101); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5101, 0x5102); A = mem_rd(gb, DE);
  CYC(0x5102, 0x5103); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5103, 0x5104); B = A;
  CYC(0x5104, 0x5105); E = alu_inc8(gb, E);
  CYC(0x5105, 0x5106); L = alu_inc8(gb, L);
  CYC(0x5106, 0x5107); A = mem_rd(gb, DE);
  CYC(0x5107, 0x5108); mem_wr(gb, HL, A);
  CYC(0x5108, 0x5109); C = A;

  // Read in boundary data based on the angle, determine if the sword has gone past
  CYC(0x5109, 0x510b); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x510b, 0x510c); A = mem_rd(gb, DE);
  CYC(0x510c, 0x510e); alu_add(gb, 0x02);
  CYC(0x510e, 0x5110); alu_and(gb, 0x1c);
  CYC(0x5110, 0x5111); alu_rrca(gb);
  CYC(0x5111, 0x5114); SET_HL(0x5157); // armosWarrior_sword_angleBoundaries
  CYC(0x5114, 0x5115); armosWarrior_addAToHl_from_rst(gb, 0x5115);

  CYC(0x5115, 0x5116); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5116, 0x5117); E = B;
  CYC(0x5117, 0x511a); push_effect(gb, 0x511a); armosWarrior_checkPositionComponent_hook(gb);
  if (F & FC) { CYCT(0x511a, 0x511c); goto done; } // jr c
  CYC(0x511a, 0x511c);
  CYC(0x511c, 0x511d); E = C;
  CYC(0x511d, 0x511e); A = mem_rd(gb, HL);
  CYC(0x511e, 0x5121); push_effect(gb, 0x5121); armosWarrior_checkPositionComponent_hook(gb);

done:
  CYC(0x5121, 0x5122); H = D;
  RET(0x5122); return; // ret
}

void armosWarrior_sword_playSlashSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5167, 0x516a); A = W8(wFrameCounter);
  CYC(0x516a, 0x516c); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x516c); return; } // ret nz
  CYC(0x516c, 0x516d);
  CYC(0x516d, 0x516f); A = 0x74; // SND_SWORDSLASH
  CYC(0x516f, 0x5172);
  playSound_b00_hook(gb); return; // jp
}
