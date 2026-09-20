#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode73), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode73), (from), (to), true)

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
  BASE(enemyCode73);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);

  // ENEMYSTATUS_DEAD
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+14); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto delete_; } // jr nz
  CYC(b_+15, b_+17);

  // Subid 2 (shield) just destroyed

  // Destroy sword
  CALL_C(b_+17, ecom_killRelatedObj2_b0f_hook, SYM(ecom_killRelatedObj2_b0f), b_+20);

  // Set some variables on parent
  CYC(b_+20, b_+22); A = 0x04; // Object.state
  CALL_C(b_+22, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+25);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x0d);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x5a);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x60);

delete_:
  CYC(b_+35, b_+38);
  enemyDelete_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+38, ecom_getSubidAndCpStateTo08_b0f_hook, SYM(ecom_getSubidAndCpStateTo08_b0f), b_+41);
  if (!(F & FC)) { CYCT(b_+41, b_+43); goto normalState; } // jr nc
  CYC(b_+41, b_+43);
  {
    CYC(b_+43, b_+44); push_effect(gb, b_+44);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == SYM(armosWarrior_state_uninitialized)) { armosWarrior_state_uninitialized_hook(gb); return; }
    if (target == SYM(armosWarrior_state_spawner)) { armosWarrior_state_spawner_hook(gb); return; }
    if (target == SYM(armosWarrior_state_stub)) { armosWarrior_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+60, b_+61); B = alu_dec8(gb, B);
  CYC(b_+61, b_+62); A = B;
  {
    CYC(b_+62, b_+63); push_effect(gb, b_+63);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == SYM(armosWarrior_parent)) { armosWarrior_parent_hook(gb); return; }
    if (target == SYM(armosWarrior_shield)) { armosWarrior_shield_hook(gb); return; }
    if (target == SYM(armosWarrior_sword)) { armosWarrior_sword_hook(gb); return; }
    HANDOFF(target);
  }
}

void armosWarrior_state_uninitialized_hook(GB *gb) {
  BASE(armosWarrior_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+5); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(b_+2, b_+5);

  // Spawner only
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+7, b_+10); W8(wDisabledObjects) = A;
  CYC(b_+10, b_+13); W8(wMenuDisabled) = A;
  CYC(b_+13, b_+15); A = 0x73; // ENEMY_ARMOS_WARRIOR
  CYC(b_+15, SYM(armosWarrior_state_spawner));
  enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

void armosWarrior_state_spawner_hook(GB *gb) {
  BASE(armosWarrior_state_spawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x03;
  CALL_C(b_+2, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); C = 0x0c;
  CALL_C(b_+8, ecom_setZAboveScreen_b0f_hook, SYM(ecom_setZAboveScreen_b0f), b_+11);

  // Spawn parent
  CYC(b_+11, b_+13); B = 0x73; // ENEMY_ARMOS_WARRIOR
  CALL_C(b_+13, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+16);
  CYC(b_+16, b_+17); C = H;

  // Spawn shield
  CALL_C(b_+17, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [shield.subid] = 2

  // [shield.relatedObj1] = parent
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+23, b_+25); A = ENEMY_BASE; // Enemy.start
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+26, b_+27); mem_wr(gb, HL, C);
  CALL_C(b_+27, objectCopyPosition_hook, SYM(objectCopyPosition), b_+30);
  CYC(b_+30, b_+31); push_effect(gb, HL);

  // Spawn sword
  CALL_C(b_+31, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x03); // [sword.subid] = 3

  // [sword.relatedObj1] = parent
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+38, b_+40); A = ENEMY_BASE; // Enemy.start
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+41, b_+42); mem_wr(gb, HL, C);
  CALL_C(b_+42, objectCopyPosition_hook, SYM(objectCopyPosition), b_+45);

  // [parent.var31] = shield
  // [parent.var32] = sword
  CYC(b_+45, b_+46); B = H;
  CYC(b_+46, b_+47); SET_HL(pop_effect(gb));
  CYC(b_+47, b_+48); A = H;
  CYC(b_+48, b_+49); H = C;
  CYC(b_+49, b_+51); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+52, b_+53); mem_wr(gb, HL, B);
  CALL_C(b_+53, objectCopyPosition_hook, SYM(objectCopyPosition), b_+56);

  // Transfer enabled byte to parent
  CYC(b_+56, b_+58); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+58, b_+59); E = L;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); mem_wr(gb, HL, A);
  CYC(b_+61, SYM(armosWarrior_state_stub));
  enemyDelete_hook(gb); return; // jp
}

void armosWarrior_state_stub_hook(GB *gb) {
  BASE(armosWarrior_state_stub);
  RET(b_+0); return; // ret
}

void armosWarrior_parent_hook(GB *gb) {
  BASE(armosWarrior_parent);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == SYM(armosWarrior_parent_state8)) { armosWarrior_parent_state8_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_state9)) { armosWarrior_parent_state9_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_stateA)) { armosWarrior_parent_stateA_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_stateB)) { armosWarrior_parent_stateB_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_stateC)) { armosWarrior_parent_stateC_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_stateD)) { armosWarrior_parent_stateD_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_stateE)) { armosWarrior_parent_stateE_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_stateF)) { armosWarrior_parent_stateF_hook(gb); return; }
    if (target == SYM(armosWarrior_parent_state10)) { armosWarrior_parent_state10_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for door to close
void armosWarrior_parent_state8_hook(GB *gb) {
  BASE(armosWarrior_parent_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0108);
  CALL_C(b_+8, enemyBoss_spawnShadow_b0f_hook, SYM(enemyBoss_spawnShadow_b0f), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x60); // ENEMYCOLLISION_ARMOS_WARRIOR_PROTECTED
  CYC(b_+23, SYM(armosWarrior_parent_state9));
  objectSetVisible82_hook(gb); return; // jp
}

// Cutscene before fight starts (falling from sky)
void armosWarrior_parent_state9_hook(GB *gb) {
  BASE(armosWarrior_parent_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == b_+13) goto substate0;
    if (target == b_+45) goto substate1;
    if (target == b_+57) goto substate2;
    if (target == b_+72) goto substate3;
    if (target == b_+99) goto substate4;
    HANDOFF(target);
  }

substate0:
  CYC(b_+13, b_+15); C = 0x20;
  CALL_C(b_+15, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+18);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);

  // Hit the ground
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+25); A = 0x1a;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); // [counter1]
  CYC(b_+26, b_+29); W8(wScreenShakeCounterY) = A;
  CYC(b_+29, b_+32); W8(wScreenShakeCounterX) = A;

  // [sword.zh] = [parent.zh]
  CYC(b_+32, b_+34); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+34, b_+35); H = mem_rd(gb, HL);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+37, b_+38); E = L;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); A = 0x81; // SND_STRONG_POUND
  CYC(b_+42, b_+45);
  playSound_b00_hook(gb); return; // jp

substate1:
  CALL_C(b_+45, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+48);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; } // ret nz
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+50); L = E;
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+51, b_+54); SET_BC((SYM(updateEnemies__next) + 1)); // TX_2f01
  CYC(b_+54, b_+57);
  showText_hook(gb); return; // jp

substate2:
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+59); L = E;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+64, enemyBoss_beginMiniboss_b0f_hook, SYM(enemyBoss_beginMiniboss_b0f), b_+67);
  CYC(b_+67, b_+69); A = 0x02;
  CYC(b_+69, b_+72);
  enemySetAnimation_hook(gb); return; // jp

substate3:
  CALL_C(b_+72, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+75);
  if (!(F & FZ)) { RET_TAKEN(b_+75); return; } // ret nz
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x46); // [counter1]
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+82, b_+83); L = E;
  CYC(b_+83, b_+84); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]

  // [sword.yh] -= 2
  CYC(b_+84, b_+86); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+86, b_+87); H = mem_rd(gb, HL);
  CYC(b_+87, b_+89); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL);
  CYC(b_+90, b_+92); alu_sub(gb, 0x02);
  CYC(b_+92, b_+93); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a

  // [sword.xh] -= 1
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

  CYC(b_+95, b_+96); alu_xor(gb, A);
  CYC(b_+96, b_+99);
  enemySetAnimation_hook(gb); return; // jp

// Sword moving up, parent moving down
substate4:
  CALL_C(b_+99, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+104); goto applySpeed; } // jr nz
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+106, b_+107); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+107, b_+109); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+111, b_+113); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x08);

applySpeed:
  CALL_C(b_+115, objectApplySpeed_hook, SYM(objectApplySpeed), b_+118);
  CYC(b_+118, SYM(armosWarrior_parent_stateA));
  enemyAnimate_hook(gb); return; // jp
}

// Deciding which direction to move in next
void armosWarrior_parent_stateA_hook(GB *gb) {
  BASE(armosWarrior_parent_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // If the armos is moving directly toward his sword, reverse direction
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CALL_C(b_+10, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+13);
  CYC(b_+13, b_+15); alu_add(gb, 0x04);
  CYC(b_+15, b_+17); alu_and(gb, 0x18);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto haveTurn; } // jr nz
  CYC(b_+22, b_+24);

  // Reverse direction
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_cpl(gb);
  CYC(b_+31, b_+32); A = alu_inc8(gb, A);
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);

haveTurn:
  CALL_C(b_+33, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x4b);
  CYC(b_+40, SYM(armosWarrior_parent_stateB));
  armosWarrior_parent_animate_hook(gb); return; // jr
}

// Moving in "box" pattern for [counter1] frames
void armosWarrior_parent_stateB_hook(GB *gb) {
  BASE(armosWarrior_parent_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); armosWarrior_parent_updateBoxMovement_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, SYM(armosWarrior_parent_updateBoxMovement)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  armosWarrior_parent_updateBoxMovement_hook(gb); return; // fallthrough
}

void armosWarrior_parent_updateBoxMovement_hook(GB *gb) {
  BASE(armosWarrior_parent_updateBoxMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, armosWarrior_parent_checkReachedTurningPoint_hook, SYM(armosWarrior_parent_checkReachedTurningPoint), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); armosWarrior_parent_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);

  // Hit one of the turning points in his movement pattern; turn 90 degrees
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); alu_and(gb, 0x18);
  CYC(b_+14, SYM(armosWarrior_parent_animate)); mem_wr(gb, DE, A);
  armosWarrior_parent_animate_hook(gb); return; // fallthrough
}

void armosWarrior_parent_animate_hook(GB *gb) {
  BASE(armosWarrior_parent_animate);
  CYC(b_+0, SYM(armosWarrior_parent_stateC));
  enemyAnimate_hook(gb); return; // jp
}

// Shield just hit
void armosWarrior_parent_stateC_hook(GB *gb) {
  BASE(armosWarrior_parent_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); armosWarrior_parent_updateBoxMovement_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0a);

  // Set speed based on number of shield hits
  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+21); SET_HL(SYM(armosWarrior_parent_speedVals)); // armosWarrior_parent_speedVals
  CYC(b_+21, b_+22); armosWarrior_addAToHl_from_rst(gb, b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return; // ret
}

// Shield just destroyed
void armosWarrior_parent_stateD_hook(GB *gb) {
  BASE(armosWarrior_parent_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto gotoNextState; } // jr z
  CYC(b_+3, b_+5);

  // Create debris at random offset every 8 frames
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CALL_C(b_+9, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+12);
  CYC(b_+12, b_+13); C = A;
  CYC(b_+13, b_+15); alu_and(gb, 0x70);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+19); alu_sub(gb, 0x04);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+21); A = C;
  CYC(b_+21, b_+23); alu_and(gb, 0x0f);
  CYC(b_+23, b_+24); C = A;
  CALL_C(b_+24, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+27);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; } // ret nz
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x06); // INTERAC_ROCKDEBRIS
  CYC(b_+30, b_+33);
  objectCopyPositionWithOffset_hook(gb); return; // jp

gotoNextState:
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x1e); // [counter1]
  CYC(b_+35, b_+36); L = E;
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x44); // ENEMYCOLLISION_STANDARD_MINIBOSS
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+45, b_+48); SET_BC((SYM(updateEnemies__next) + 2)); // TX_2f02
  CALL_C(b_+48, showText_hook, SYM(showText), b_+51);
  CYC(b_+51, b_+53); A = 0x01;
  CYC(b_+53, SYM(armosWarrior_parent_stateE));
  enemySetAnimation_hook(gb); return; // jp
}

// Standing still before charging Link
void armosWarrior_parent_stateE_hook(GB *gb) {
  BASE(armosWarrior_parent_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); armosWarrior_parent_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, SYM(armosWarrior_parent_stateF));
  ecom_updateAngleTowardTarget_b0f_hook(gb); return; // jp
}

// Charging
void armosWarrior_parent_stateF_hook(GB *gb) {
  BASE(armosWarrior_parent_stateF);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); A = 0x01;
  CALL_C(b_+5, ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); objectApplySpeed_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);

  // Hit wall
  CALL_C(b_+11, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_xor(gb, 0x10);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+22, b_+24); A = 0x80;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xfe);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+31, b_+33); A = 0x1e;
  CALL_C(b_+33, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+36);
  CYC(b_+36, b_+38); A = 0x81; // SND_STRONG_POUND
  CYC(b_+38, SYM(armosWarrior_parent_state10));
  playSound_b00_hook(gb); return; // jp
}

// Recoiling from hitting wall
void armosWarrior_parent_state10_hook(GB *gb) {
  BASE(armosWarrior_parent_state10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); C = 0x16;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);

  // Hit ground
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0e);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x3c);
  RET(b_+24); return; // ret
}

void armosWarrior_shield_hook(GB *gb) {
  BASE(armosWarrior_shield);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state8; } // jr z
  CYC(b_+3, b_+5);

  // Delete self if no hits remaining
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+13); ecom_killObjectH_b0f_hook(gb); return; } // jp z
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x21; // Object.animParameter
  CALL_C(b_+15, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+22, b_+24); goto updatePos; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+30, enemySetAnimation_hook, SYM(enemySetAnimation), b_+33);

updatePos:
  CYC(b_+33, b_+36);
  armosWarrior_shield_updatePosition_hook(gb); return; // jp

// Uninitialized
state8:
  CYC(b_+36, b_+39); A = W8(wcc93);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+40); return; } // ret nz
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+43); L = E;
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x61); // ENEMYCOLLISION_ARMOS_WARRIOR_SHIELD
  CYC(b_+48, b_+50); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x03);

  // [shield.relatedObj2] = sword (parent.var32)
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+54, b_+55); H = mem_rd(gb, HL);
  CYC(b_+55, b_+57); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+57, b_+59); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+59, b_+61); A = ENEMY_BASE; // Enemy.start
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+63); E = alu_inc8(gb, E);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);

  CYC(b_+65, b_+67); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+67, b_+69); A = 0x03;
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);

  CALL_C(b_+70, enemySetAnimation_hook, SYM(enemySetAnimation), b_+73);
  CALL_C(b_+73, armosWarrior_shield_updatePosition_hook, SYM(armosWarrior_shield_updatePosition), b_+76);
  CYC(b_+76, SYM(armosWarrior_sword));
  objectSetVisible81_hook(gb); return; // jp
}

void armosWarrior_sword_hook(GB *gb) {
  BASE(armosWarrior_sword);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0b);
  if (F & FC) { CYC(b_+5, b_+8); } else { CALL_C_CC(b_+5, armosWarrior_sword_playSlashSound_hook, SYM(armosWarrior_sword_playSlashSound), b_+8); } // call nc
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_sub(gb, 0x08);
  {
    CYC(b_+13, b_+14); push_effect(gb, b_+14);
    uint16_t target = armosWarrior_jump_table(gb);
    if (target == SYM(armosWarrior_sword_state8)) { armosWarrior_sword_state8_hook(gb); return; }
    if (target == SYM(armosWarrior_sword_state9)) { armosWarrior_sword_state9_hook(gb); return; }
    if (target == SYM(armosWarrior_sword_stateA)) { armosWarrior_sword_stateA_hook(gb); return; }
    if (target == SYM(armosWarrior_sword_stateB)) { armosWarrior_sword_stateB_hook(gb); return; }
    if (target == SYM(armosWarrior_sword_stateC)) { armosWarrior_sword_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for door to close
void armosWarrior_sword_state8_hook(GB *gb) {
  BASE(armosWarrior_sword_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x62); // ENEMYCOLLISION_ARMOS_WARRIOR_SWORD
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x05); // SPEED_20
  CALL_C(b_+16, armosWarrior_sword_setPositionAsHeld_hook, SYM(armosWarrior_sword_setPositionAsHeld), b_+19);

  // [sword.relatedObj2] = shield (parent.var31)
  CYC(b_+19, b_+21); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+23, b_+25); A = ENEMY_BASE; // Enemy.start
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);

  CYC(b_+29, b_+31); A = 0x09;
  CALL_C(b_+31, enemySetAnimation_hook, SYM(enemySetAnimation), b_+34);
  CYC(b_+34, SYM(armosWarrior_sword_state9));
  objectSetVisible80_hook(gb); return; // jp
}

// Waiting for initial cutscene to end, then moving upward before fight starts
void armosWarrior_sword_state9_hook(GB *gb) {
  BASE(armosWarrior_sword_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05; // Object.substate
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+10); armosWarrior_sword_setPositionAsHeld_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+12); return; } // ret c
  CYC(b_+12, b_+13);
  if (F & FZ) { CYCT(b_+13, b_+15); goto parentSubstate3; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); L = alu_dec8(gb, L);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); // [parent.state]
  CYC(b_+17, b_+19); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto gotoStateA; } // jr nc
  CYC(b_+19, b_+21);
  CALL_C(b_+21, armosWarrior_sword_playSlashSound_hook, SYM(armosWarrior_sword_playSlashSound), b_+24);
  CYC(b_+24, b_+27);
  enemyAnimate_hook(gb); return; // jp

gotoStateA:
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x01);

  // Save position
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+35, b_+37); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  RET(b_+43); return; // ret

parentSubstate3:
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+50); alu_or(gb, A);
  CYC(b_+50, b_+52); A = 0x0a;
  if (F & FZ) { CYCT(b_+52, b_+55); enemySetAnimation_hook(gb); return; } // jp z
  CYC(b_+52, b_+55);
  CYC(b_+55, b_+57); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+57, b_+58); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+58); return; // ret
}

// Staying still before charging toward Link
void armosWarrior_sword_stateA_hook(GB *gb) {
  BASE(armosWarrior_sword_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x96);

  // Write target position to var30/var31
  CYC(b_+14, b_+16); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+16, b_+18); A = hram_rd(gb, hEnemyTargetY & 0xff);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+19, b_+21); A = hram_rd(gb, hEnemyTargetX & 0xff);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);

  CALL_C(b_+22, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+25);
  CALL_C(b_+25, enemyAnimate_hook, SYM(enemyAnimate), b_+28);
  CYC(b_+28, SYM(armosWarrior_sword_stateB));
  armosWarrior_sword_updateCollisionBox_hook(gb); return; // jp
}

// Charging toward target position
void armosWarrior_sword_stateB_hook(GB *gb) {
  BASE(armosWarrior_sword_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, armosWarrior_sword_checkCollisionWithShield_hook, SYM(armosWarrior_sword_checkCollisionWithShield), b_+3);
  CYC(b_+3, b_+6); A = W8(wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto checkWentTooFar; } // jr nz
  CYC(b_+8, b_+10);

  // Update angle toward target position every 4 frames
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(b_+13, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+16);
  CALL_C(b_+16, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);

checkWentTooFar:
  CALL_C(b_+22, armosWarrior_sword_checkWentTooFar_hook, SYM(armosWarrior_sword_checkWentTooFar), b_+25);
  if (F & FC) { CYCT(b_+25, b_+27); goto beginSlowingDown; } // jr c
  CYC(b_+25, b_+27);

  // If within 28 pixels of target position, start slowing down
  CYC(b_+27, b_+29); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); alu_sub(gb, B);
  CYC(b_+31, b_+33); alu_add(gb, 28);
  CYC(b_+33, b_+35); alu_cp(gb, 57);
  if (!(F & FC)) { CYCT(b_+35, b_+37); goto notSlowingDown; } // jr nc
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+40); alu_sub(gb, C);
  CYC(b_+40, b_+42); alu_add(gb, 28);
  CYC(b_+42, b_+44); alu_cp(gb, 57);
  if (!(F & FC)) { CYCT(b_+44, b_+46); goto notSlowingDown; } // jr nc
  CYC(b_+44, b_+46);

beginSlowingDown:
  CYC(b_+46, b_+48); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x70);

notSlowingDown:
  CALL_C(b_+53, enemyAnimate_hook, SYM(enemyAnimate), SYM(armosWarrior_sword_updatePosition));
  armosWarrior_sword_updatePosition_hook(gb); return; // fallthrough
}

void armosWarrior_sword_updatePosition_hook(GB *gb) {
  BASE(armosWarrior_sword_updatePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_applyVelocityForTopDownEnemy_b0f_hook, SYM(ecom_applyVelocityForTopDownEnemy_b0f), b_+3);

  // Save position
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+8); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); E = alu_inc8(gb, E);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, SYM(armosWarrior_sword_stateC));
  armosWarrior_sword_updateCollisionBox_hook(gb); return; // jp
}

// Slowing down
void armosWarrior_sword_stateC_hook(GB *gb) {
  BASE(armosWarrior_sword_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, armosWarrior_sword_checkCollisionWithShield_hook, SYM(armosWarrior_sword_checkCollisionWithShield), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto stoppedMoving; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  CYC(b_+14, b_+17); SET_HL(SYM(armosWarrior_sword_speedVals)); // armosWarrior_sword_speedVals
  CYC(b_+17, b_+18); armosWarrior_addAToHl_from_rst(gb, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);

  // Restore position (which was manipulated for shield collision detection)
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+25, b_+27); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+29, b_+30); E = alu_inc8(gb, E);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);

  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_cp(gb, 30);
  if (!(F & FC)) { CYCT(b_+38, b_+40); goto haveDirection; } // jr nc
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); alu_rrca(gb);

haveDirection:
  if (!(F & FC)) { CALL_C_CC(b_+41, enemyAnimate_hook, SYM(enemyAnimate), b_+44); } else { CYC(b_+41, b_+44); } // call nc
  CYC(b_+44, b_+46);
  armosWarrior_sword_updatePosition_hook(gb); return; // jr

stoppedMoving:
  CYC(b_+46, b_+48); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+51); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+51, b_+53); goto atRest; } // jr z
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x02); // [counter1]
  CYC(b_+55, b_+58);
  enemyAnimate_hook(gb); return; // jp

atRest:
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x0a);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);

  // Set counter1 (frames to rest) based on number of hits until shield destroyed
  CYC(b_+66, b_+68); A = 0x32; // Object.var32
  CALL_C(b_+68, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+71);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); A = alu_swap(gb, A);
  CYC(b_+74, b_+75); alu_rlca(gb);
  CYC(b_+75, b_+77); alu_add(gb, 30);
  CYC(b_+77, b_+79); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+82); A = 0x0a;
  CYC(b_+82, SYM(armosWarrior_shield_updatePosition));
  enemySetAnimation_hook(gb); return; // jp
}

// Shield copies parent's position plus an offset
void armosWarrior_shield_updatePosition_hook(GB *gb) {
  BASE(armosWarrior_shield_updatePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b; // Object.yh
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) -- [parent.yh]
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl) -- [parent.xh]
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); E = L;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A); // [shield.zh] = [parent.zh]

  CYC(b_+14, b_+16); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+20); SET_HL(SYM(armosWarrior_shield_YXOffsets)); // armosWarrior_shield_YXOffsets
  CYC(b_+20, b_+21); armosWarrior_addDoubleIndexToHl_from_rst(gb, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+24, b_+25); alu_add(gb, B);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); alu_add(gb, C);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return; // ret
}

// Updates collisionRadiusY/X based on animParameter, also adds an offset to Y/X position.
void armosWarrior_sword_updateCollisionBox_hook(GB *gb) {
  BASE(armosWarrior_sword_updateCollisionBox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, A); // add a
  CYC(b_+4, b_+7); SET_HL(SYM(armosWarrior_sword_collisionBoxes)); // armosWarrior_sword_collisionBoxes
  CYC(b_+7, b_+8); armosWarrior_addDoubleIndexToHl_from_rst(gb, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); SET_HL(HL + 1); // inc hl
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); SET_HL(HL + 1); // inc hl
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return; // ret
}

// Sets the sword's position assuming it's being held by the parent.
void armosWarrior_sword_setPositionAsHeld_hook(GB *gb) {
  BASE(armosWarrior_sword_setPositionAsHeld);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b; // Object.yh
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+8); SET_BC(0xf4fa);
  CYC(b_+8, SYM(armosWarrior_sword_checkCollisionWithShield));
  objectTakePositionWithOffset_hook(gb); return; // jp
}

static void armosWarrior_checkIntersection_hook(GB *gb) {
  BASE(armosWarrior_sword_checkCollisionWithShield);
  // b = [sword.collisionRadius] + [shield.collisionRadius]
  CYC(b_+66, b_+67); E = L;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+69, b_+70); B = A;

  // a = [sword.pos] - [shield.pos]
  CYC(b_+70, b_+71); L = C;
  CYC(b_+71, b_+72); E = L;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+74); alu_sub(gb, mem_rd(gb, HL));

  CYC(b_+74, b_+75); alu_add(gb, B);
  CYC(b_+75, b_+77); B = alu_sla(gb, B);
  CYC(b_+77, b_+78); B = alu_inc8(gb, B);
  CYC(b_+78, b_+79); alu_cp(gb, B);
  RET(b_+79); return; // ret
}

void armosWarrior_sword_checkCollisionWithShield_hook(GB *gb) {
  BASE(armosWarrior_sword_checkCollisionWithShield);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);

  // Check if sword and shield collide
  CYC(b_+5, b_+7); A = 0x26; // Object.collisionRadiusY
  CALL_C(b_+7, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+10);
  CYC(b_+10, b_+12); C = ENEMY_BASE + OBJ_YH;
  CYC(b_+12, b_+15); push_effect(gb, b_+15); armosWarrior_checkIntersection_hook(gb);
  if (!(F & FC)) { RET_TAKEN(b_+15); return; } // ret nc
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); C = ENEMY_BASE + OBJ_XH;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+20, b_+23); push_effect(gb, b_+23); armosWarrior_checkIntersection_hook(gb);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; } // ret nc
  CYC(b_+23, b_+24);

  // They've collided
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);

  // Set various variables on the shield
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x18);

  // [Hits until destruction]--
  CYC(b_+33, b_+35); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

  CYC(b_+36, b_+38); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_add(gb, 0x02);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);

  // h = [shield.relatedObj1] = parent
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+44, b_+45); H = mem_rd(gb, HL);

  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x3c);

  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x0c);

  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x78); // SPEED_300

  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x18);

  CYC(b_+61, b_+63); A = 0x63; // SND_BOSS_DAMAGE
  CYC(b_+63, b_+66);
  playSound_b00_hook(gb); return; // jp
}

// The armos always moves in a "box" pattern in his first phase, this checks if he's
// reached one of the "corners" of the box where he must turn.
//
// @param[out] zflag z if hit a turning point
void armosWarrior_parent_checkReachedTurningPoint_hook(GB *gb) {
  BASE(armosWarrior_parent_checkReachedTurningPoint);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x31;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(b_+7, b_+9); goto hitCorner; } // jr c
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x7f;
  CYC(b_+11, b_+13); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto hitCorner; } // jr nc
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); B = 0xbf;
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_cp(gb, 0xc0);
  if (!(F & FC)) { CYCT(b_+22, b_+24); goto hitCorner; } // jr nc
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); B = 0x31;
  CYC(b_+26, b_+28); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(b_+28, b_+30); goto hitCorner; } // jr c
  CYC(b_+28, b_+30);
  CALL_C(b_+30, objectApplySpeed_hook, SYM(objectApplySpeed), b_+33);
  CYC(b_+33, b_+34); alu_or(gb, D);
  RET(b_+34); return; // ret

hitCorner:
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+38); alu_xor(gb, A);
  RET(b_+38); return; // ret
}

static void armosWarrior_checkPositionComponent_hook(GB *gb) {
  BASE(armosWarrior_sword_checkWentTooFar);
  // If bit 0 of the data structure is set, it's an upper / left boundary
  CYC(b_+39, b_+41); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto boundary; } // jr nz
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); alu_cp(gb, E);
  RET(b_+44); return; // ret

boundary:
  CYC(b_+45, b_+46); alu_cp(gb, E);
  CYC(b_+46, b_+47); alu_ccf(gb);
  RET(b_+47); return; // ret
}

// @param[out] bc Position of sword
// @param[out] cflag c if the sword has gone to far and should stop now
void armosWarrior_sword_checkWentTooFar_hook(GB *gb) {
  BASE(armosWarrior_sword_checkWentTooFar);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Fix position, store it in bc
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+9); E = alu_inc8(gb, E);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+13); C = A;

  // Read in boundary data based on the angle, determine if the sword has gone past
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_add(gb, 0x02);
  CYC(b_+18, b_+20); alu_and(gb, 0x1c);
  CYC(b_+20, b_+21); alu_rrca(gb);
  CYC(b_+21, b_+24); SET_HL(SYM(armosWarrior_sword_angleBoundaries)); // armosWarrior_sword_angleBoundaries
  CYC(b_+24, b_+25); armosWarrior_addAToHl_from_rst(gb, b_+25);

  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+26, b_+27); E = B;
  CYC(b_+27, b_+30); push_effect(gb, b_+30); armosWarrior_checkPositionComponent_hook(gb);
  if (F & FC) { CYCT(b_+30, b_+32); goto done; } // jr c
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); E = C;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+37); push_effect(gb, b_+37); armosWarrior_checkPositionComponent_hook(gb);

done:
  CYC(b_+37, b_+38); H = D;
  RET(b_+38); return; // ret
}

void armosWarrior_sword_playSlashSound_hook(GB *gb) {
  BASE(armosWarrior_sword_playSlashSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x74; // SND_SWORDSLASH
  CYC(b_+8, SYM(enemyCode74));
  playSound_b00_hook(gb); return; // jp
}
