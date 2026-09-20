#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5560, ecom_checkHazards_b0e_hook, 0x4051, 0x5563);
  CALL_C(0x5563, enemyCode3d_runState_hook, 0x5569, 0x5566);
  CYC(0x5566, 0x5569); swordEnemy_updateEnemyCollisionMode_hook(gb); return; // jp
}

// 0e:5569, local sub-label of enemyCode3d/enemyCode49/enemyCode4a (@runState); called with
// a real `call` (its @dead branch pops that return address to skip the caller's tail jp).
void enemyCode3d_runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5569, 0x556b); goto normalStatus; } // jr z
  CYC(0x5569, 0x556b);
  CYC(0x556b, 0x556d); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x556d); return; } // ret c
  CYC(0x556d, 0x556e);
  if (F & FZ) { CYCT(0x556e, 0x5570); goto dead; } // jr z
  CYC(0x556e, 0x5570);
  CYC(0x5570, 0x5571); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5571, 0x5574); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x5571, 0x5574);
  RET(0x5574); return; // ret

dead:
  POP(0x5575); // pop hl -- discards this call's return address
  CYC(0x5576, 0x5579); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5579, ecom_checkScentSeedActive_b0e_hook, 0x44b6, 0x557c);
  if (F & FZ) { CYCT(0x557c, 0x5583); goto dispatch; } // jr z
  CYC(0x557c, 0x557e);
  CYC(0x557e, 0x5580); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5580, 0x5582); A = 0x19; // SPEED_a0
  CYC(0x5582, 0x5583); mem_wr(gb, DE, A);

dispatch:
  CYC(0x5583, 0x5585); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5585, 0x5586); A = mem_rd(gb, DE);
  {
    CYC(0x5586, 0x5587); push_effect(gb, 0x5587);
    uint16_t target = swordEnemy_jump_table(gb);
    if (target == 0x559d) { swordEnemy_state_uninitialized_hook(gb); return; }
    if (target == 0x55e4) { swordEnemy_state_stub_hook(gb); return; }
    if (target == 0x55b8) { swordEnemy_state_switchHook_hook(gb); return; }
    if (target == 0x55ce) { swordEnemy_state_scentSeed_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x55e5) { swordEnemy_state8_hook(gb); return; }
    if (target == 0x55ff) { swordEnemy_state9_hook(gb); return; }
    if (target == 0x560c) { swordEnemy_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:559d, bare global; jump-table target from enemyCode3d_runState, also reached by
// genuine jp from swordDarknut_state_uninitialized.
void swordEnemy_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x559d, 0x559f); B = 0x1d; // PART_ENEMY_SWORD
  CALL_C(0x559f, ecom_spawnProjectile_b0e_hook, 0x437c, 0x55a2);
  if (!(F & FZ)) { RET_TAKEN(0x55a2); return; } // ret nz
  CYC(0x55a2, 0x55a3);
  CALL_C(0x55a3, ecom_setRandomCardinalAngle_b0e_hook, 0x43c6, 0x55a6);
  CALL_C(0x55a6, ecom_updateAnimationFromAngle_b0e_hook, 0x43d8, 0x55a9);
  CYC(0x55a9, 0x55ab); A = 0x14; // SPEED_80
  CALL_C(0x55ab, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x55ae);
  CYC(0x55ae, 0x55b0); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x55b0, 0x55b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  // Enable scent seeds
  CYC(0x55b1, 0x55b3); L = ENEMY_BASE + 0x3f; // Enemy.var3f
  CYC(0x55b3, 0x55b5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x55b5, 0x55b8); swordEnemy_setChaseCooldown_hook(gb); return; // jp
}

// 0e:55b8, bare global; jump-table target from enemyCode3d_runState and enemyCode48_runState.
void swordEnemy_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55b8, 0x55b9); E = alu_inc8(gb, E);
  CYC(0x55b9, 0x55ba); A = mem_rd(gb, DE);
  {
    CYC(0x55ba, 0x55bb); push_effect(gb, 0x55bb);
    uint16_t target = swordEnemy_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x55c3) { RET(0x55c3); return; } // ret (substate1/substate2)
    if (target == 0x55c4) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(0x55c4, 0x55c6); B = 0x09;
  CALL_C(0x55c6, ecom_fallToGroundAndSetState_b0e_hook, 0x44e2, 0x55c9);
  CYC(0x55c9, 0x55cb); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x55cb, 0x55cd); mem_wr(gb, HL, 0x10);
  RET(0x55cd); return; // ret
}

// 0e:55ce, bare global; jump-table target from enemyCode3d_runState.
void swordEnemy_state_scentSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55ce, 0x55d1); A = mem_rd(gb, wScentSeedActive);
  CYC(0x55d1, 0x55d2); alu_or(gb, A);
  CYC(0x55d2, 0x55d3); H = D;
  if (F & FZ) { CYCT(0x55d3, 0x55d6); swordEnemy_gotoState8_hook(gb); return; } // jp z
  CYC(0x55d3, 0x55d6);
  CALL_C(0x55d6, ecom_updateAngleToScentSeed_b0e_hook, 0x44cb, 0x55d9);
  CALL_C(0x55d9, ecom_updateAnimationFromAngle_b0e_hook, 0x43d8, 0x55dc);
  CALL_C(0x55dc, ecom_applyVelocityForSideviewEnemy_b0e_hook, 0x4153, 0x55df);
  CALL_C(0x55df, enemyAnimate_hook, 0x2818, 0x55e2);
  CYC(0x55e2, 0x55e4); swordEnemy_animate_hook(gb); return; // jr
}

// 0e:55e4, bare global; jump-table target from enemyCode3d_runState and enemyCode48_runState.
void swordEnemy_state_stub_hook(GB *gb) {
  RET(0x55e4); return; // ret
}

// 0e:55e5, bare global; jump-table target from enemyCode3d_runState. Moving slowly in
// cardinal directions until Link get close.
void swordEnemy_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x55e5, swordEnemy_checkLinkIsClose_hook, 0x56fa, 0x55e8);
  if (F & FC) { CYCT(0x55e8, 0x55eb); swordEnemy_beginChasingLink_hook(gb); return; } // jp c
  CYC(0x55e8, 0x55eb);
  CALL_C(0x55eb, ecom_decCounter1_b0e_hook, 0x439a, 0x55ee);
  if (F & FZ) { CYCT(0x55ee, 0x55f1); swordEnemy_chooseRandomAngleAndCounter1_hook(gb); return; } // jp z
  CYC(0x55ee, 0x55f1);
  CALL_C(0x55f1, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x55f4);
  if (!(F & FZ)) { CYCT(0x55f4, 0x55f6); swordEnemy_animate_hook(gb); return; } // jr nz
  CYC(0x55f4, 0x55f6);

  // Hit a wall
  CALL_C(0x55f6, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x55f9);
  if (!(F & FZ)) { CYCT(0x55f9, 0x55fc); ecom_updateAnimationFromAngle_b0e_hook(gb); return; } // jp nz
  CYC(0x55f9, 0x55fc);
  swordEnemy_animate_hook(gb); return; // fallthrough
}

// 0e:55fc, bare global; falls into from swordEnemy_state8, also reached by genuine jr/jp
// from swordEnemy_gotoState8, swordEnemy_stateA and swordEnemy_state_scentSeed.
void swordEnemy_animate_hook(GB *gb) {
  CYC(0x55fc, 0x55ff); enemyAnimate_hook(gb); return; // jp
}

// 0e:55ff, bare global; jump-table target from enemyCode3d_runState. Started chasing Link
// (don't adjust angle until next state).
void swordEnemy_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x55ff, ecom_decCounter1_b0e_hook, 0x439a, 0x5602);
  if (!(F & FZ)) { RET_TAKEN(0x5602); return; } // ret nz
  CYC(0x5602, 0x5603);
  CYC(0x5603, 0x5605); mem_wr(gb, HL, 0x60);
  CYC(0x5605, 0x5606); L = E;
  CYC(0x5606, 0x5607); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x5607, 0x5609); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5609, 0x560b); mem_wr(gb, HL, 0x19); // SPEED_a0
  RET(0x560b); return; // ret
}

// 0e:560c, bare global; jump-table target from enemyCode3d_runState. Chasing Link for
// [counter1] frames (adjusts angle appropriately).
void swordEnemy_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x560c, ecom_decCounter1_b0e_hook, 0x439a, 0x560f);
  if (F & FZ) { CYCT(0x560f, 0x5612); swordEnemy_gotoState8_hook(gb); return; } // jp z
  CYC(0x560f, 0x5612);
  CYC(0x5612, 0x5613); A = mem_rd(gb, HL);
  CYC(0x5613, 0x5615); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x5615, 0x5617); goto applyVelocity; } // jr nz
  CYC(0x5615, 0x5617);
  CALL_C(0x5617, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x561a);
  CALL_C(0x561a, objectNudgeAngleTowards_hook, 0x1fd4, 0x561d);
  CALL_C(0x561d, ecom_updateAnimationFromAngle_b0e_hook, 0x43d8, 0x5620);

applyVelocity:
  CALL_C(0x5620, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x5623);

  // Animate at double speed
  CALL_C(0x5623, enemyAnimate_hook, 0x2818, 0x5626);
  CYC(0x5626, 0x5628); swordEnemy_animate_hook(gb); return; // jr
}

// 0e:5628, bare global; called from swordEnemy_stateA, swordEnemy_state_scentSeed and
// swordDarknut_stateA. Reverts to state 8; wandering around in cardinal directions.
void swordEnemy_gotoState8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5628, 0x5629); L = E;
  CYC(0x5629, 0x562b); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x562b, 0x562d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x562d, 0x562f); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x562f, 0x5631); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5631, 0x5632); A = mem_rd(gb, HL);
  CYC(0x5632, 0x5634); alu_add(gb, 0x04);
  CYC(0x5634, 0x5636); alu_and(gb, 0x18);
  CYC(0x5636, 0x5637); mem_wr(gb, HL, A);
  CALL_C(0x5637, ecom_updateAnimationFromAngle_b0e_hook, 0x43d8, 0x563a);
  CALL_C(0x563a, swordEnemy_setChaseCooldown_hook, 0x572a, 0x563d);
  CYC(0x563d, 0x563f); swordEnemy_animate_hook(gb); return; // jr
}

// ==================================================================================================
// ENEMY_SWORD_DARKNUT
// ==================================================================================================
void enemyCode48_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x563f, ecom_checkHazards_b0e_hook, 0x4051, 0x5642);
  CALL_C(0x5642, enemyCode48_runState_hook, 0x5648, 0x5645);
  CYC(0x5645, 0x5648); swordDarknut_updateEnemyCollisionMode_hook(gb); return; // jp
}

// 0e:5648, local sub-label of enemyCode48 (@runState); called with a real `call` (its
// @dead branch pops that return address to skip the caller's tail jp).
void enemyCode48_runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5648, 0x564a); goto normalStatus; } // jr z
  CYC(0x5648, 0x564a);
  CYC(0x564a, 0x564c); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x564c); return; } // ret c
  CYC(0x564c, 0x564d);
  if (F & FZ) { CYCT(0x564d, 0x564f); goto dead; } // jr z
  CYC(0x564d, 0x564f);
  CYC(0x564f, 0x5650); A = alu_dec8(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x5650, ecom_updateKnockbackAndCheckHazards_b0e_hook, 0x404a, 0x5653); else CYC(0x5650, 0x5653); // call nz
  CYC(0x5653, 0x5656); swordDarknut_updateEnemyCollisionMode_hook(gb); return; // jp

dead:
  CYC(0x5656, 0x5658); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5658, 0x5659); A = mem_rd(gb, DE);
  CYC(0x5659, 0x565b); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x565b, 0x5662); goto popDead; } // jr nz
  CYC(0x565b, 0x565d);
  CYC(0x565d, 0x5660); SET_HL(wKilledGoldenEnemies);
  CYC(0x5660, 0x5662); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 2))); // set 2,(hl)

popDead:
  POP(0x5662); // pop hl -- discards this call's return address
  CYC(0x5663, 0x5666); enemyDie_hook(gb); return; // jp

normalStatus:
  CYC(0x5666, 0x5668); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5668, 0x5669); A = mem_rd(gb, DE);
  {
    CYC(0x5669, 0x566a); push_effect(gb, 0x566a);
    uint16_t target = swordEnemy_jump_table(gb);
    if (target == 0x5680) { swordDarknut_state_uninitialized_hook(gb); return; }
    if (target == 0x55e4) { swordEnemy_state_stub_hook(gb); return; }
    if (target == 0x55b8) { swordEnemy_state_switchHook_hook(gb); return; }
    if (target == 0x5692) { swordDarknut_state8_hook(gb); return; }
    if (target == 0x56aa) { swordDarknut_state9_hook(gb); return; }
    if (target == 0x56b7) { swordDarknut_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5680, bare global; jump-table target from enemyCode48_runState.
void swordDarknut_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5680, 0x5682); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5682, 0x5683); A = mem_rd(gb, DE);
  CYC(0x5683, 0x5685); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x5685, 0x5687); goto notGolden; } // jr nz
  CYC(0x5685, 0x5687);
  CYC(0x5687, 0x568a); A = mem_rd(gb, wKilledGoldenEnemies);
  CYC(0x568a, 0x568c); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x568c, 0x568f); swordDarknut_delete_hook(gb); return; } // jp nz
  CYC(0x568c, 0x568f);

notGolden:
  CYC(0x568f, 0x5692); swordEnemy_state_uninitialized_hook(gb); return; // jp
}

// 0e:5692, bare global; jump-table target from enemyCode48_runState. Moving slowly in
// cardinal directions until Link get close. Identical to swordEnemy_state8.
void swordDarknut_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5692, swordDarknut_checkLinkIsClose_hook, 0x5712, 0x5695);
  if (F & FC) { CYCT(0x5695, 0x5697); swordEnemy_beginChasingLink_hook(gb); return; } // jr c
  CYC(0x5695, 0x5697);
  CALL_C(0x5697, ecom_decCounter1_b0e_hook, 0x439a, 0x569a);
  if (F & FZ) { CYCT(0x569a, 0x569c); swordEnemy_chooseRandomAngleAndCounter1_hook(gb); return; } // jr z
  CYC(0x569a, 0x569c);
  CALL_C(0x569c, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x569f);
  if (!(F & FZ)) { CYCT(0x569f, 0x56a1); swordDarknut_animate_hook(gb); return; } // jr nz
  CYC(0x569f, 0x56a1);

  // Hit a wall
  CALL_C(0x56a1, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x56a4);
  if (!(F & FZ)) { CYCT(0x56a4, 0x56a7); ecom_updateAnimationFromAngle_b0e_hook(gb); return; } // jp nz
  CYC(0x56a4, 0x56a7);
  swordDarknut_animate_hook(gb); return; // fallthrough
}

// 0e:56a7, bare global; falls into from swordDarknut_state8, also reached by genuine jr
// from swordDarknut_stateA.
void swordDarknut_animate_hook(GB *gb) {
  CYC(0x56a7, 0x56aa); enemyAnimate_hook(gb); return; // jp
}

// 0e:56aa, bare global; jump-table target from enemyCode48_runState. Started chasing Link
// (don't adjust angle until next state). Identical to swordEnemy_state9 except for the
// speed.
void swordDarknut_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56aa, ecom_decCounter1_b0e_hook, 0x439a, 0x56ad);
  if (!(F & FZ)) { RET_TAKEN(0x56ad); return; } // ret nz
  CYC(0x56ad, 0x56ae);
  CYC(0x56ae, 0x56b0); mem_wr(gb, HL, 0x60);
  CYC(0x56b0, 0x56b1); L = E;
  CYC(0x56b1, 0x56b2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x56b2, 0x56b4); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x56b4, 0x56b6); mem_wr(gb, HL, 0x1e); // SPEED_c0
  RET(0x56b6); return; // ret
}

// 0e:56b7, bare global; jump-table target from enemyCode48_runState. Chasing Link for
// [counter1] frames (adjusts angle appropriately). Identical to swordEnemy_stateA except
// for how quickly it turns toward Link.
void swordDarknut_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56b7, ecom_decCounter1_b0e_hook, 0x439a, 0x56ba);
  if (F & FZ) { CYCT(0x56ba, 0x56bd); swordEnemy_gotoState8_hook(gb); return; } // jp z
  CYC(0x56ba, 0x56bd);
  CYC(0x56bd, 0x56be); A = mem_rd(gb, HL);
  CYC(0x56be, 0x56c0); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x56c0, 0x56c2); goto applyVelocity; } // jr nz
  CYC(0x56c0, 0x56c2);
  CALL_C(0x56c2, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x56c5);
  CALL_C(0x56c5, objectNudgeAngleTowards_hook, 0x1fd4, 0x56c8);
  CALL_C(0x56c8, ecom_updateAnimationFromAngle_b0e_hook, 0x43d8, 0x56cb);

applyVelocity:
  CALL_C(0x56cb, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x56ce);

  // Animate at double speed
  CALL_C(0x56ce, enemyAnimate_hook, 0x2818, 0x56d1);
  CYC(0x56d1, 0x56d3); swordDarknut_animate_hook(gb); return; // jr
}

// 0e:56d3, bare global; called from swordEnemy_state8 and swordDarknut_state8.
void swordEnemy_beginChasingLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56d3, 0x56d5); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x56d5, 0x56d6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x56d6, 0x56d8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x56d8, 0x56da); mem_wr(gb, HL, 0x10);
  CALL_C(0x56da, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x56dd);
  CYC(0x56dd, 0x56e0); ecom_updateAnimationFromAngle_b0e_hook(gb); return; // jp
}

// 0e:56e0, bare global; called from swordEnemy_state8 and swordDarknut_state8.
void swordEnemy_chooseRandomAngleAndCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56e0, 0x56e3); SET_BC(0x3f07);
  CALL_C(0x56e3, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x56e6);
  CYC(0x56e6, 0x56e8); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x56e8, 0x56ea); A = 0x50;
  CYC(0x56ea, 0x56eb); alu_add(gb, B);
  CYC(0x56eb, 0x56ec); mem_wr(gb, DE, A);
  CALL_C(0x56ec, swordEnemy_chooseRandomAngleAndCounter1_chooseAngle_hook, 0x56f2, 0x56ef);
  CYC(0x56ef, 0x56f2); ecom_updateAnimationFromAngle_b0e_hook(gb); return; // jp
}

// 0e:56f2, local sub-label of swordEnemy_chooseRandomAngleAndCounter1 (@chooseAngle),
// called with a real `call`.
// 1 in 8 chance of moving toward Link.
void swordEnemy_chooseRandomAngleAndCounter1_chooseAngle_hook(GB *gb) {
  CYC(0x56f2, 0x56f3); A = C;
  CYC(0x56f3, 0x56f4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x56f4, 0x43b4); ecom_updateCardinalAngleTowardTarget_b0e_hook(gb); return; } // jp z
  CYC(0x56f4, 0x56f7);
  CYC(0x56f7, 0x43c6); ecom_setRandomCardinalAngle_b0e_hook(gb); return; // jp
}

// 0e:56fa, bare global; called from swordEnemy_state8.
// @param[out] cflag c if Link is within 40 pixels of enemy in both directions (and
// counter2, the timeout, has reached 0)
void swordEnemy_checkLinkIsClose_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56fa, ecom_decCounter2_b0e_hook, 0x43a3, 0x56fd);
  if (!(F & FZ)) { RET_TAKEN(0x56fd); return; } // ret nz
  CYC(0x56fd, 0x56fe);

  // NOTE: Why does this use hFFB2, then hEnemyTargetX? It's mixing two position
  // variables.
  CYC(0x56fe, 0x5700); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5700, 0x5702); A = hram_rd(gb, 0xb2); // hFFB2
  CYC(0x5702, 0x5703); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5703, 0x5705); alu_add(gb, 0x28);
  CYC(0x5705, 0x5707); alu_cp(gb, 0x51);
  if (!(F & FC)) { RET_TAKEN(0x5707); return; } // ret nc
  CYC(0x5707, 0x5708);
  CYC(0x5708, 0x570a); L = ENEMY_BASE + OBJ_XH;
  CYC(0x570a, 0x570c); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x570c, 0x570d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x570d, 0x570f); alu_add(gb, 0x28);
  CYC(0x570f, 0x5711); alu_cp(gb, 0x51);
  RET(0x5711); return; // ret
}

// 0e:5712, bare global; called from swordDarknut_state8. This is identical to
// swordEnemy_checkLinkIsClose.
void swordDarknut_checkLinkIsClose_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5712, ecom_decCounter2_b0e_hook, 0x43a3, 0x5715);
  if (!(F & FZ)) { RET_TAKEN(0x5715); return; } // ret nz
  CYC(0x5715, 0x5716);

  // NOTE: Why does this use hFFB2, then hEnemyTargetX? It's mixing two position
  // variables.
  CYC(0x5716, 0x5718); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5718, 0x571a); A = hram_rd(gb, 0xb2); // hFFB2
  CYC(0x571a, 0x571b); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x571b, 0x571d); alu_add(gb, 0x28);
  CYC(0x571d, 0x571f); alu_cp(gb, 0x51);
  if (!(F & FC)) { RET_TAKEN(0x571f); return; } // ret nc
  CYC(0x571f, 0x5720);
  CYC(0x5720, 0x5722); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5722, 0x5724); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5724, 0x5725); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5725, 0x5727); alu_add(gb, 0x28);
  CYC(0x5727, 0x5729); alu_cp(gb, 0x51);
  RET(0x5729); return; // ret
}

// 0e:572a, bare global; called from swordEnemy_gotoState8 and
// swordEnemy_state_uninitialized. Sets counter2 to the number of frames to wait before
// chasing Link again. Higher subids have lower cooldowns.
void swordEnemy_setChaseCooldown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x572a, 0x572c); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x572c, 0x572d); A = mem_rd(gb, DE);
  CYC(0x572d, 0x5730); SET_BC(0x5738); // @counter2Vals
  CALL_C(0x5730, addAToBc_hook, 0x006d, 0x5733);
  CYC(0x5733, 0x5735); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5735, 0x5736); A = mem_rd(gb, BC);
  CYC(0x5736, 0x5737); mem_wr(gb, DE, A);
  RET(0x5737); return; // ret
}

// 0e:573b, bare global; called from enemyCode3d. Updates enemyCollisionMode based on
// Link's angle relative to the enemy. In this way, Link's sword doesn't damage the enemy
// if positioned in such a way that their sword should block it.
void swordEnemy_updateEnemyCollisionMode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x573b, 0x573d); B = 0x00;
  CYC(0x573d, 0x573f); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x573f, 0x5740); A = mem_rd(gb, DE);
  CYC(0x5740, 0x5741); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5741, 0x5743); goto stunned; } // jr nz
  CYC(0x5741, 0x5743);

  CALL_C(0x5743, swordEnemy_checkIgnoreCollision_hook, 0x577c, 0x5746);
  CYC(0x5746, 0x5748); A = 0x55; // ENEMYCOLLISION_STALFOS_BLOCKED_WITH_SWORD
  CYC(0x5748, 0x574a); B = 0x00;
  if (!(F & FZ)) { CYCT(0x574a, 0x574c); goto setVars; } // jr nz
  CYC(0x574a, 0x574c);

stunned:
  CYC(0x574c, 0x574d); B = alu_inc8(gb, B);
  CYC(0x574d, 0x574f); E = ENEMY_BASE + OBJ_ID;
  CYC(0x574f, 0x5750); A = mem_rd(gb, DE);
  CYC(0x5750, 0x5752); alu_cp(gb, 0x49); // ENEMY_SWORD_SHROUDED_STALFOS
  CYC(0x5752, 0x5754); A = 0x11; // ENEMYCOLLISION_BURNABLE_ENEMY
  if (!(F & FZ)) { CYCT(0x5754, 0x5756); goto setVars; } // jr nz
  CYC(0x5754, 0x5756);
  CYC(0x5756, 0x5758); A = 0x11; // ENEMYCOLLISION_BURNABLE_ENEMY

setVars:
  CYC(0x5758, 0x575a); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x575a, 0x575b); mem_wr(gb, DE, A);

  CYC(0x575b, 0x575d); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x575d, 0x575e); A = B;
  CYC(0x575e, 0x575f); mem_wr(gb, DE, A);
  RET(0x575f); return; // ret
}

// 0e:5760, bare global; called from enemyCode48. Same as
// swordEnemy_updateEnemyCollisionMode, but with a different enemyCollisionMode for the
// darknut.
void swordDarknut_updateEnemyCollisionMode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5760, 0x5762); B = 0x00;
  CYC(0x5762, 0x5764); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x5764, 0x5765); A = mem_rd(gb, DE);
  CYC(0x5765, 0x5766); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5766, 0x5768); goto stunned; } // jr nz
  CYC(0x5766, 0x5768);

  CALL_C(0x5768, swordEnemy_checkIgnoreCollision_hook, 0x577c, 0x576b);
  CYC(0x576b, 0x576d); A = 0x56; // ENEMYCOLLISION_DARKNUT_BLOCKED_WITH_SWORD
  CYC(0x576d, 0x576f); B = 0x00;
  if (!(F & FZ)) { CYCT(0x576f, 0x5771); goto setVars; } // jr nz
  CYC(0x576f, 0x5771);

stunned:
  CYC(0x5771, 0x5773); A = 0x20; // ENEMYCOLLISION_DARKNUT
  CYC(0x5773, 0x5774); B = alu_inc8(gb, B);

setVars:
  CYC(0x5774, 0x5776); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5776, 0x5777); mem_wr(gb, DE, A);

  CYC(0x5777, 0x5779); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5779, 0x577a); A = B;
  CYC(0x577a, 0x577b); mem_wr(gb, DE, A);
  RET(0x577b); return; // ret
}

// 0e:577c, bare global; called from swordEnemy_updateEnemyCollisionMode and
// swordDarknut_updateEnemyCollisionMode. Check whether the angle between Link and the
// enemy is such that the collision should be ignored (due to the sword blocking it).
// Knockback is handled by PART_ENEMY_SWORD.
// @param[out] zflag z if sword hits should be ignored
void swordEnemy_checkIgnoreCollision_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x577c, 0x577e); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x577e, 0x577f); A = mem_rd(gb, DE);
  CYC(0x577f, 0x5780); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5780); return; } // ret nz
  CYC(0x5780, 0x5781);

  CALL_C(0x5781, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5784);
  CYC(0x5784, 0x5785); B = A;
  CYC(0x5785, 0x5787); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5787, 0x5788); A = mem_rd(gb, DE);
  CYC(0x5788, 0x5789); alu_add(gb, A);
  CYC(0x5789, 0x578c); SET_HL(0x5791); // @angleBits
  swordEnemy_addDoubleIndexToHl_from_rst(gb, 0x578d);
  CYC(0x578d, 0x578e); A = B;
  CYC(0x578e, 0x5791); checkFlag_hook(gb); return; // jp
}

// 0e:57a1, bare global; called from swordDarknut_state_uninitialized.
void swordDarknut_delete_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57a1, decNumEnemies_hook, 0x24b3, 0x57a4);
  CYC(0x57a4, 0x57a7); enemyDelete_hook(gb); return; // jp
}
