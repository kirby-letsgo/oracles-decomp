#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode3d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode3d), (from), (to), true)

void enemyCode3d_runState_hook(GB *gb);
void swordEnemy_state_uninitialized_hook(GB *gb);
void swordEnemy_state_switchHook_hook(GB *gb);
void swordEnemy_state_scentSeed_hook(GB *gb);
void swordEnemy_state_stub_hook(GB *gb);
void swordEnemy_state8_hook(GB *gb);
void swordEnemy_animate_hook(GB *gb);
void swordEnemy_state9_hook(GB *gb);
void swordEnemy_stateA_hook(GB *gb);
void swordEnemy_gotoState8_hook(GB *gb);
void enemyCode48_runState_hook(GB *gb);
void swordDarknut_state_uninitialized_hook(GB *gb);
void swordDarknut_state8_hook(GB *gb);
void swordDarknut_animate_hook(GB *gb);
void swordDarknut_state9_hook(GB *gb);
void swordDarknut_stateA_hook(GB *gb);
void swordEnemy_beginChasingLink_hook(GB *gb);
void swordEnemy_chooseRandomAngleAndCounter1_hook(GB *gb);
void swordEnemy_chooseRandomAngleAndCounter1_chooseAngle_hook(GB *gb);
void swordEnemy_checkLinkIsClose_hook(GB *gb);
void swordDarknut_checkLinkIsClose_hook(GB *gb);
void swordEnemy_setChaseCooldown_hook(GB *gb);
void swordEnemy_updateEnemyCollisionMode_hook(GB *gb);
void swordDarknut_updateEnemyCollisionMode_hook(GB *gb);
void swordEnemy_checkIgnoreCollision_hook(GB *gb);
void swordDarknut_delete_hook(GB *gb);

static uint16_t swordEnemy_jump_table(GB *gb) {
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

static void swordEnemy_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SWORD_MOBLIN
// ENEMY_SWORD_SHROUDED_STALFOS
// ENEMY_SWORD_MASKED_MOBLIN
//
// Shares some code with ENEMY_SWORD_DARKNUT (further down).
//
// Variables:
//   var30: Nonzero if enemyCollisionMode was changed to ignore sword damage (due to the
//          enemy's sword blocking it)
// ==================================================================================================
void enemyCode3d_hook(GB *gb) {
  BASE(enemyCode3d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  CALL_C(b_+3, enemyCode3d_runState_hook, b_+9, b_+6);
  CYC(b_+6, b_+9); swordEnemy_updateEnemyCollisionMode_hook(gb); return; // jp
}

// 0e:5569, local sub-label of enemyCode3d/enemyCode49/enemyCode4a (@runState); called with
// a real `call` (its @dead branch pops that return address to skip the caller's tail jp).
void enemyCode3d_runState_hook(GB *gb) {
  BASE(enemyCode3d);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+9, b_+11); goto normalStatus; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+13); return; } // ret c
  CYC(b_+13, b_+14);
  if (F & FZ) { CYCT(b_+14, b_+16); goto dead; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+17, b_+20);
  RET(b_+20); return; // ret

dead:
  POP(b_+21); // pop hl -- discards this call's return address
  CYC(b_+22, b_+25); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+25, ecom_checkScentSeedActive_b0e_hook, SYM(ecom_checkScentSeedActive_b0e), b_+28);
  if (F & FZ) { CYCT(b_+28, b_+35); goto dispatch; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+32, b_+34); A = 0x19; // SPEED_a0
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);

dispatch:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  {
    CYC(b_+38, b_+39); push_effect(gb, b_+39);
    uint16_t target = swordEnemy_jump_table(gb);
    if (target == SYM(swordEnemy_state_uninitialized)) { swordEnemy_state_uninitialized_hook(gb); return; }
    if (target == SYM(swordEnemy_state_stub)) { swordEnemy_state_stub_hook(gb); return; }
    if (target == SYM(swordEnemy_state_switchHook)) { swordEnemy_state_switchHook_hook(gb); return; }
    if (target == SYM(swordEnemy_state_scentSeed)) { swordEnemy_state_scentSeed_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(swordEnemy_state8)) { swordEnemy_state8_hook(gb); return; }
    if (target == SYM(swordEnemy_state9)) { swordEnemy_state9_hook(gb); return; }
    if (target == SYM(swordEnemy_stateA)) { swordEnemy_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:559d, bare global; jump-table target from enemyCode3d_runState, also reached by
// genuine jp from swordDarknut_state_uninitialized.
void swordEnemy_state_uninitialized_hook(GB *gb) {
  BASE(swordEnemy_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x1d; // PART_ENEMY_SWORD
  CALL_C(b_+2, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ecom_setRandomCardinalAngle_b0e_hook, SYM(ecom_setRandomCardinalAngle_b0e), b_+9);
  CALL_C(b_+9, ecom_updateAnimationFromAngle_b0e_hook, SYM(ecom_updateAnimationFromAngle_b0e), b_+12);
  CYC(b_+12, b_+14); A = 0x14; // SPEED_80
  CALL_C(b_+14, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+17);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  // Enable scent seeds
  CYC(b_+20, b_+22); L = ENEMY_BASE + 0x3f; // Enemy.var3f
  CYC(b_+22, b_+24); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+24, SYM(swordEnemy_state_switchHook)); swordEnemy_setChaseCooldown_hook(gb); return; // jp
}

// 0e:55b8, bare global; jump-table target from enemyCode3d_runState and enemyCode48_runState.
void swordEnemy_state_switchHook_hook(GB *gb) {
  BASE(swordEnemy_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = swordEnemy_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; } // ret (substate1/substate2)
    if (target == b_+12) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(b_+12, b_+14); B = 0x09;
  CALL_C(b_+14, ecom_fallToGroundAndSetState_b0e_hook, SYM(ecom_fallToGroundAndSetState_b0e), b_+17);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x10);
  RET(b_+21); return; // ret
}

// 0e:55ce, bare global; jump-table target from enemyCode3d_runState.
void swordEnemy_state_scentSeed_hook(GB *gb) {
  BASE(swordEnemy_state_scentSeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+5); H = D;
  if (F & FZ) { CYCT(b_+5, b_+8); swordEnemy_gotoState8_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CALL_C(b_+8, ecom_updateAngleToScentSeed_b0e_hook, SYM(ecom_updateAngleToScentSeed_b0e), b_+11);
  CALL_C(b_+11, ecom_updateAnimationFromAngle_b0e_hook, SYM(ecom_updateAnimationFromAngle_b0e), b_+14);
  CALL_C(b_+14, ecom_applyVelocityForSideviewEnemy_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0e), b_+17);
  CALL_C(b_+17, enemyAnimate_hook, SYM(enemyAnimate), b_+20);
  CYC(b_+20, SYM(swordEnemy_state_stub)); swordEnemy_animate_hook(gb); return; // jr
}

// 0e:55e4, bare global; jump-table target from enemyCode3d_runState and enemyCode48_runState.
void swordEnemy_state_stub_hook(GB *gb) {
  BASE(swordEnemy_state_stub);
  RET(b_+0); return; // ret
}

// 0e:55e5, bare global; jump-table target from enemyCode3d_runState. Moving slowly in
// cardinal directions until Link get close.
void swordEnemy_state8_hook(GB *gb) {
  BASE(swordEnemy_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, swordEnemy_checkLinkIsClose_hook, SYM(swordEnemy_checkLinkIsClose), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); swordEnemy_beginChasingLink_hook(gb); return; } // jp c
  CYC(b_+3, b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+12); swordEnemy_chooseRandomAngleAndCounter1_hook(gb); return; } // jp z
  CYC(b_+9, b_+12);
  CALL_C(b_+12, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); swordEnemy_animate_hook(gb); return; } // jr nz
  CYC(b_+15, b_+17);

  // Hit a wall
  CALL_C(b_+17, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), b_+20);
  if (!(F & FZ)) { CYCT(b_+20, SYM(swordEnemy_animate)); ecom_updateAnimationFromAngle_b0e_hook(gb); return; } // jp nz
  CYC(b_+20, SYM(swordEnemy_animate));
  swordEnemy_animate_hook(gb); return; // fallthrough
}

// 0e:55fc, bare global; falls into from swordEnemy_state8, also reached by genuine jr/jp
// from swordEnemy_gotoState8, swordEnemy_stateA and swordEnemy_state_scentSeed.
void swordEnemy_animate_hook(GB *gb) {
  BASE(swordEnemy_animate);
  CYC(b_+0, SYM(swordEnemy_state9)); enemyAnimate_hook(gb); return; // jp
}

// 0e:55ff, bare global; jump-table target from enemyCode3d_runState. Started chasing Link
// (don't adjust angle until next state).
void swordEnemy_state9_hook(GB *gb) {
  BASE(swordEnemy_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x19); // SPEED_a0
  RET(b_+12); return; // ret
}

// 0e:560c, bare global; jump-table target from enemyCode3d_runState. Chasing Link for
// [counter1] frames (adjusts angle appropriately).
void swordEnemy_stateA_hook(GB *gb) {
  BASE(swordEnemy_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); swordEnemy_gotoState8_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto applyVelocity; } // jr nz
  CYC(b_+9, b_+11);
  CALL_C(b_+11, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+14);
  CALL_C(b_+14, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+17);
  CALL_C(b_+17, ecom_updateAnimationFromAngle_b0e_hook, SYM(ecom_updateAnimationFromAngle_b0e), b_+20);

applyVelocity:
  CALL_C(b_+20, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+23);

  // Animate at double speed
  CALL_C(b_+23, enemyAnimate_hook, SYM(enemyAnimate), b_+26);
  CYC(b_+26, SYM(swordEnemy_gotoState8)); swordEnemy_animate_hook(gb); return; // jr
}

// 0e:5628, bare global; called from swordEnemy_stateA, swordEnemy_state_scentSeed and
// swordDarknut_stateA. Reverts to state 8; wandering around in cardinal directions.
void swordEnemy_gotoState8_hook(GB *gb) {
  BASE(swordEnemy_gotoState8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); L = E;
  CYC(b_+1, b_+3); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_add(gb, 0x04);
  CYC(b_+12, b_+14); alu_and(gb, 0x18);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CALL_C(b_+15, ecom_updateAnimationFromAngle_b0e_hook, SYM(ecom_updateAnimationFromAngle_b0e), b_+18);
  CALL_C(b_+18, swordEnemy_setChaseCooldown_hook, SYM(swordEnemy_setChaseCooldown), b_+21);
  CYC(b_+21, SYM(enemyCode48)); swordEnemy_animate_hook(gb); return; // jr
}

// ==================================================================================================
// ENEMY_SWORD_DARKNUT
// ==================================================================================================
void enemyCode48_hook(GB *gb) {
  BASE(enemyCode48);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  CALL_C(b_+3, enemyCode48_runState_hook, b_+9, b_+6);
  CYC(b_+6, b_+9); swordDarknut_updateEnemyCollisionMode_hook(gb); return; // jp
}

// 0e:5648, local sub-label of enemyCode48 (@runState); called with a real `call` (its
// @dead branch pops that return address to skip the caller's tail jp).
void enemyCode48_runState_hook(GB *gb) {
  BASE(enemyCode48);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+9, b_+11); goto normalStatus; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+13); return; } // ret c
  CYC(b_+13, b_+14);
  if (F & FZ) { CYCT(b_+14, b_+16); goto dead; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = alu_dec8(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+17, ecom_updateKnockbackAndCheckHazards_b0e_hook, SYM(ecom_updateKnockbackAndCheckHazards_b0e), b_+20); else CYC(b_+17, b_+20); // call nz
  CYC(b_+20, b_+23); swordDarknut_updateEnemyCollisionMode_hook(gb); return; // jp

dead:
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+28, b_+35); goto popDead; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+33); SET_HL(wKilledGoldenEnemies);
  CYC(b_+33, b_+35); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 2))); // set 2,(hl)

popDead:
  POP(b_+35); // pop hl -- discards this call's return address
  CYC(b_+36, b_+39); enemyDie_hook(gb); return; // jp

normalStatus:
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  {
    CYC(b_+42, b_+43); push_effect(gb, b_+43);
    uint16_t target = swordEnemy_jump_table(gb);
    if (target == SYM(swordDarknut_state_uninitialized)) { swordDarknut_state_uninitialized_hook(gb); return; }
    if (target == SYM(swordEnemy_state_stub)) { swordEnemy_state_stub_hook(gb); return; }
    if (target == SYM(swordEnemy_state_switchHook)) { swordEnemy_state_switchHook_hook(gb); return; }
    if (target == SYM(swordDarknut_state8)) { swordDarknut_state8_hook(gb); return; }
    if (target == SYM(swordDarknut_state9)) { swordDarknut_state9_hook(gb); return; }
    if (target == SYM(swordDarknut_stateA)) { swordDarknut_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5680, bare global; jump-table target from enemyCode48_runState.
void swordDarknut_state_uninitialized_hook(GB *gb) {
  BASE(swordDarknut_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto notGolden; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wKilledGoldenEnemies);
  CYC(b_+10, b_+12); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); swordDarknut_delete_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);

notGolden:
  CYC(b_+15, SYM(swordDarknut_state8)); swordEnemy_state_uninitialized_hook(gb); return; // jp
}

// 0e:5692, bare global; jump-table target from enemyCode48_runState. Moving slowly in
// cardinal directions until Link get close. Identical to swordEnemy_state8.
void swordDarknut_state8_hook(GB *gb) {
  BASE(swordDarknut_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, swordDarknut_checkLinkIsClose_hook, SYM(swordDarknut_checkLinkIsClose), b_+3);
  if (F & FC) { CYCT(b_+3, b_+5); swordEnemy_beginChasingLink_hook(gb); return; } // jr c
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); swordEnemy_chooseRandomAngleAndCounter1_hook(gb); return; } // jr z
  CYC(b_+8, b_+10);
  CALL_C(b_+10, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); swordDarknut_animate_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);

  // Hit a wall
  CALL_C(b_+15, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, SYM(swordDarknut_animate)); ecom_updateAnimationFromAngle_b0e_hook(gb); return; } // jp nz
  CYC(b_+18, SYM(swordDarknut_animate));
  swordDarknut_animate_hook(gb); return; // fallthrough
}

// 0e:56a7, bare global; falls into from swordDarknut_state8, also reached by genuine jr
// from swordDarknut_stateA.
void swordDarknut_animate_hook(GB *gb) {
  BASE(swordDarknut_animate);
  CYC(b_+0, SYM(swordDarknut_state9)); enemyAnimate_hook(gb); return; // jp
}

// 0e:56aa, bare global; jump-table target from enemyCode48_runState. Started chasing Link
// (don't adjust angle until next state). Identical to swordEnemy_state9 except for the
// speed.
void swordDarknut_state9_hook(GB *gb) {
  BASE(swordDarknut_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x1e); // SPEED_c0
  RET(b_+12); return; // ret
}

// 0e:56b7, bare global; jump-table target from enemyCode48_runState. Chasing Link for
// [counter1] frames (adjusts angle appropriately). Identical to swordEnemy_stateA except
// for how quickly it turns toward Link.
void swordDarknut_stateA_hook(GB *gb) {
  BASE(swordDarknut_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); swordEnemy_gotoState8_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto applyVelocity; } // jr nz
  CYC(b_+9, b_+11);
  CALL_C(b_+11, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+14);
  CALL_C(b_+14, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+17);
  CALL_C(b_+17, ecom_updateAnimationFromAngle_b0e_hook, SYM(ecom_updateAnimationFromAngle_b0e), b_+20);

applyVelocity:
  CALL_C(b_+20, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+23);

  // Animate at double speed
  CALL_C(b_+23, enemyAnimate_hook, SYM(enemyAnimate), b_+26);
  CYC(b_+26, SYM(swordEnemy_beginChasingLink)); swordDarknut_animate_hook(gb); return; // jr
}

// 0e:56d3, bare global; called from swordEnemy_state8 and swordDarknut_state8.
void swordEnemy_beginChasingLink_hook(GB *gb) {
  BASE(swordEnemy_beginChasingLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10);
  CALL_C(b_+7, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+10);
  CYC(b_+10, SYM(swordEnemy_chooseRandomAngleAndCounter1)); ecom_updateAnimationFromAngle_b0e_hook(gb); return; // jp
}

// 0e:56e0, bare global; called from swordEnemy_state8 and swordDarknut_state8.
void swordEnemy_chooseRandomAngleAndCounter1_hook(GB *gb) {
  BASE(swordEnemy_chooseRandomAngleAndCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC((SYM(func_3ee4) + 35));
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); A = 0x50;
  CYC(b_+10, b_+11); alu_add(gb, B);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, swordEnemy_chooseRandomAngleAndCounter1_chooseAngle_hook, b_+18, b_+15);
  CYC(b_+15, b_+18); ecom_updateAnimationFromAngle_b0e_hook(gb); return; // jp
}

// 0e:56f2, local sub-label of swordEnemy_chooseRandomAngleAndCounter1 (@chooseAngle),
// called with a real `call`.
// 1 in 8 chance of moving toward Link.
void swordEnemy_chooseRandomAngleAndCounter1_chooseAngle_hook(GB *gb) {
  BASE(swordEnemy_chooseRandomAngleAndCounter1);
  CYC(b_+18, b_+19); A = C;
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+20, SYM(ecom_updateCardinalAngleTowardTarget_b0e)); ecom_updateCardinalAngleTowardTarget_b0e_hook(gb); return; } // jp z
  CYC(b_+20, b_+23);
  CYC(b_+23, SYM(ecom_setRandomCardinalAngle_b0e)); ecom_setRandomCardinalAngle_b0e_hook(gb); return; // jp
}

// 0e:56fa, bare global; called from swordEnemy_state8.
// @param[out] cflag c if Link is within 40 pixels of enemy in both directions (and
// counter2, the timeout, has reached 0)
void swordEnemy_checkLinkIsClose_hook(GB *gb) {
  BASE(swordEnemy_checkLinkIsClose);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);

  // NOTE: Why does this use hFFB2, then hEnemyTargetX? It's mixing two position
  // variables.
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+8); A = hram_rd(gb, 0xb2); // hFFB2
  CYC(b_+8, b_+9); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+11); alu_add(gb, 0x28);
  CYC(b_+11, b_+13); alu_cp(gb, 0x51);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } // ret nc
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+16, b_+18); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+18, b_+19); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); alu_add(gb, 0x28);
  CYC(b_+21, b_+23); alu_cp(gb, 0x51);
  RET(b_+23); return; // ret
}

// 0e:5712, bare global; called from swordDarknut_state8. This is identical to
// swordEnemy_checkLinkIsClose.
void swordDarknut_checkLinkIsClose_hook(GB *gb) {
  BASE(swordDarknut_checkLinkIsClose);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);

  // NOTE: Why does this use hFFB2, then hEnemyTargetX? It's mixing two position
  // variables.
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+8); A = hram_rd(gb, 0xb2); // hFFB2
  CYC(b_+8, b_+9); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+11); alu_add(gb, 0x28);
  CYC(b_+11, b_+13); alu_cp(gb, 0x51);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } // ret nc
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+16, b_+18); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+18, b_+19); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); alu_add(gb, 0x28);
  CYC(b_+21, b_+23); alu_cp(gb, 0x51);
  RET(b_+23); return; // ret
}

// 0e:572a, bare global; called from swordEnemy_gotoState8 and
// swordEnemy_state_uninitialized. Sets counter2 to the number of frames to wait before
// chasing Link again. Higher subids have lower cooldowns.
void swordEnemy_setChaseCooldown_hook(GB *gb) {
  BASE(swordEnemy_setChaseCooldown);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_BC(b_+14); // @counter2Vals
  CALL_C(b_+6, addAToBc_hook, 0x006d, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return; // ret
}

// 0e:573b, bare global; called from enemyCode3d. Updates enemyCollisionMode based on
// Link's angle relative to the enemy. In this way, Link's sword doesn't damage the enemy
// if positioned in such a way that their sword should block it.
void swordEnemy_updateEnemyCollisionMode_hook(GB *gb) {
  BASE(swordEnemy_updateEnemyCollisionMode);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto stunned; } // jr nz
  CYC(b_+6, b_+8);

  CALL_C(b_+8, swordEnemy_checkIgnoreCollision_hook, SYM(swordEnemy_checkIgnoreCollision), b_+11);
  CYC(b_+11, b_+13); A = 0x55; // ENEMYCOLLISION_STALFOS_BLOCKED_WITH_SWORD
  CYC(b_+13, b_+15); B = 0x00;
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto setVars; } // jr nz
  CYC(b_+15, b_+17);

stunned:
  CYC(b_+17, b_+18); B = alu_inc8(gb, B);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ID;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_cp(gb, 0x49); // ENEMY_SWORD_SHROUDED_STALFOS
  CYC(b_+23, b_+25); A = 0x11; // ENEMYCOLLISION_BURNABLE_ENEMY
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto setVars; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = 0x11; // ENEMYCOLLISION_BURNABLE_ENEMY

setVars:
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);

  CYC(b_+32, b_+34); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  RET(b_+36); return; // ret
}

// 0e:5760, bare global; called from enemyCode48. Same as
// swordEnemy_updateEnemyCollisionMode, but with a different enemyCollisionMode for the
// darknut.
void swordDarknut_updateEnemyCollisionMode_hook(GB *gb) {
  BASE(swordDarknut_updateEnemyCollisionMode);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto stunned; } // jr nz
  CYC(b_+6, b_+8);

  CALL_C(b_+8, swordEnemy_checkIgnoreCollision_hook, SYM(swordEnemy_checkIgnoreCollision), b_+11);
  CYC(b_+11, b_+13); A = 0x56; // ENEMYCOLLISION_DARKNUT_BLOCKED_WITH_SWORD
  CYC(b_+13, b_+15); B = 0x00;
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto setVars; } // jr nz
  CYC(b_+15, b_+17);

stunned:
  CYC(b_+17, b_+19); A = 0x20; // ENEMYCOLLISION_DARKNUT
  CYC(b_+19, b_+20); B = alu_inc8(gb, B);

setVars:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);

  CYC(b_+23, b_+25); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+25, b_+26); A = B;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  RET(b_+27); return; // ret
}

// 0e:577c, bare global; called from swordEnemy_updateEnemyCollisionMode and
// swordDarknut_updateEnemyCollisionMode. Check whether the angle between Link and the
// enemy is such that the collision should be ignored (due to the sword blocking it).
// Knockback is handled by PART_ENEMY_SWORD.
// @param[out] zflag z if sword hits should be ignored
void swordEnemy_checkIgnoreCollision_hook(GB *gb) {
  BASE(swordEnemy_checkIgnoreCollision);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);

  CALL_C(b_+5, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+8);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+16); SET_HL(b_+21); // @angleBits
  swordEnemy_addDoubleIndexToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+21); checkFlag_hook(gb); return; // jp
}

// 0e:57a1, bare global; called from swordDarknut_state_uninitialized.
void swordDarknut_delete_hook(GB *gb) {
  BASE(swordDarknut_delete);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, SYM(enemyCode3e)); enemyDelete_hook(gb); return; // jp
}
