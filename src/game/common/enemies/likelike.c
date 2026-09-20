#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void likelike_state_uninitialized_hook(GB *gb);
void likelike_state_switchHook_hook(GB *gb);
void likelike_state_galeSeed_hook(GB *gb);
void likelike_state_stub_hook(GB *gb);
void likelike_subid00_hook(GB *gb);
void likelike_subid00_state8_hook(GB *gb);
void likelike_state9_hook(GB *gb);
void likelike_stateA_hook(GB *gb);
void likelike_animate_hook(GB *gb);
void likelike_stateB_hook(GB *gb);
void likelike_releaseLink_hook(GB *gb);
void likelike_stateC_hook(GB *gb);
void likelike_subid01_hook(GB *gb);
void likelike_subid02_hook(GB *gb);
void likelike_animate2_hook(GB *gb);
void likelike_subid03_hook(GB *gb);
void likelike_spawn_hook(GB *gb);
void likelike_setChildSpawnPosition_hook(GB *gb);
void likelike_findAllLikelikesWithSubid0_hook(GB *gb);
void likelike_chooseRandomPosition_hook(GB *gb);
void likelike_checkHazards_hook(GB *gb);

static uint16_t likelike_jump_table(GB *gb) {
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

static void likelike_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_LIKE_LIKE
//
// Variables:
//   relatedObj1: Pointer to the like-like spawner (subid 1), if one exists.
//   var30: Number of like-likes on-screen (for subid 1)
// ==================================================================================================
void enemyCode24_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c99, likelike_checkHazards_hook, 0x5f2a, 0x5c9c);
  if (F & FZ) { CYCT(0x5c9c, 0x5c9e); goto normalStatus; } // jr z
  CYC(0x5c9c, 0x5c9e);
  CYC(0x5c9e, 0x5ca0); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5ca0); return; } // ret c
  CYC(0x5ca0, 0x5ca1);
  if (F & FZ) { CYCT(0x5ca1, 0x5ca3); goto dead; } // jr z
  CYC(0x5ca1, 0x5ca3);
  CYC(0x5ca3, 0x5ca4); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5ca4, 0x5ca7); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x5ca4, 0x5ca7);

  // ENEMYSTATUS_JUST_HIT
  CYC(0x5ca7, 0x5ca9); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x5ca9, 0x5caa); A = mem_rd(gb, DE);
  CYC(0x5caa, 0x5cac); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(0x5cac); return; } // ret nz
  CYC(0x5cac, 0x5cad);

  // Just collided with Link. omnomnom
  CYC(0x5cad, 0x5cae); H = D;
  CYC(0x5cae, 0x5cb0); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5cb0, 0x5cb1); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5cb1, 0x5cb2); B = A;
  CYC(0x5cb2, 0x5cb3); L = alu_inc8(gb, L);
  CYC(0x5cb3, 0x5cb4); C = mem_rd(gb, HL);
  CYC(0x5cb4, 0x5cb7); SET_HL(0x5e92); // bank5.checkPositionSurroundedByWalls
  CYC(0x5cb7, 0x5cb9); E = 0x05;
  CALL_C(0x5cb9, interBankCall_hook, 0x008a, 0x5cbc);
  CYC(0x5cbc, 0x5cbe); B = alu_rl(gb, B);
  if (F & FC) { CYCT(0x5cbe, 0x5cc1); likelike_releaseLink_hook(gb); return; } // jp c
  CYC(0x5cbe, 0x5cc1);
  CYC(0x5cc1, 0x5cc3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5cc3, 0x5cc4); A = mem_rd(gb, DE);
  CYC(0x5cc4, 0x5cc5); alu_or(gb, A);
  CYC(0x5cc5, 0x5cc7); A = 0x0b;
  if (F & FZ) { CYCT(0x5cc7, 0x5cc9); goto setState; } // jr z
  CYC(0x5cc7, 0x5cc9);
  CYC(0x5cc9, 0x5cca); A = alu_inc8(gb, A);

setState:
  CYC(0x5cca, 0x5ccb); H = D;
  CYC(0x5ccb, 0x5ccd); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5ccd, 0x5cce); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5cce, 0x5ccf); L = alu_inc8(gb, L);
  CYC(0x5ccf, 0x5cd1); mem_wr(gb, HL, 0x00); // [counter1] = 0
  CYC(0x5cd1, 0x5cd2); L = alu_inc8(gb, L);
  CYC(0x5cd2, 0x5cd4); mem_wr(gb, HL, 90); // [counter2] = 90
  CYC(0x5cd4, 0x5cd6); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5cd6, 0x5cd8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x5cd8, 0x5cdb); SET_HL(w1Link);
  CALL_C(0x5cdb, objectCopyPosition_hook, 0x2242, 0x5cde);
  CYC(0x5cde, 0x5ce0); L = OBJ_COLLISION_TYPE; // <w1Link.collisionType
  CYC(0x5ce0, 0x5ce2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x5ce2, 0x5ce4); A = 0x01;
  CALL_C(0x5ce4, enemySetAnimation_hook, 0x282b, 0x5ce7);
  CYC(0x5ce7, 0x5cea); objectSetVisiblec1_hook(gb); return; // jp

dead:
  CYC(0x5cea, 0x5cec); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x5cec, 0x5ced); A = mem_rd(gb, DE);
  CYC(0x5ced, 0x5cee); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5cee, 0x5cf1); enemyDie_hook(gb); return; } // jp z
  CYC(0x5cee, 0x5cf1);
  CYC(0x5cf1, 0x5cf2); H = A;
  CYC(0x5cf2, 0x5cf4); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5cf4, 0x5cf5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5cf5, 0x5cf8); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5cf8, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x5cfb);
  if (!(F & FC)) { CYCT(0x5cfb, 0x5cfd); goto normalState; } // jr nc
  CYC(0x5cfb, 0x5cfd);
  {
    CYC(0x5cfd, 0x5cfe); push_effect(gb, 0x5cfe);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x5d18) { likelike_state_uninitialized_hook(gb); return; }
    if (target == 0x5d51) { likelike_state_stub_hook(gb); return; }
    if (target == 0x5d22) { likelike_state_switchHook_hook(gb); return; }
    if (target == 0x5d3d) { likelike_state_galeSeed_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x5d0e, 0x5d0f); A = B;
  {
    CYC(0x5d0f, 0x5d10); push_effect(gb, 0x5d10);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x5d52) { likelike_subid00_hook(gb); return; }
    if (target == 0x5df3) { likelike_subid01_hook(gb); return; }
    if (target == 0x5e54) { likelike_subid02_hook(gb); return; }
    if (target == 0x5e8a) { likelike_subid03_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5d18, bare global; jump-table target from enemyCode24.
void likelike_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d18, 0x5d1a); alu_bit(gb, 0, B);
  if (F & FZ) CALL_C_CC(0x5d1a, objectSetVisiblec2_hook, 0x1e45, 0x5d1d); else CYC(0x5d1a, 0x5d1d); // call z
  CYC(0x5d1d, 0x5d1f); A = 0x0a; // SPEED_40
  CYC(0x5d1f, 0x5d22); ecom_setSpeedAndState8_b0d_hook(gb); return; // jp
}

// 0d:5d22, bare global; jump-table target from enemyCode24. Internal @substate1/@substate2/
// @substate3 are @-local (no separate registration).
void likelike_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d22, 0x5d23); E = alu_inc8(gb, E);
  CYC(0x5d23, 0x5d24); A = mem_rd(gb, DE);
  {
    CYC(0x5d24, 0x5d25); push_effect(gb, 0x5d25);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x5d2d) { RET(0x5d2d); return; }
    if (target == 0x5d2e) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(0x5d2e, 0x5d30); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5d30, 0x5d31); A = mem_rd(gb, DE);
  CYC(0x5d31, 0x5d34); SET_HL(0x5d39); // @defaultStates
  CYC(0x5d34, 0x5d35); likelike_addAToHl_from_rst(gb, 0x5d35);
  CYC(0x5d35, 0x5d36); B = mem_rd(gb, HL);
  CYC(0x5d36, 0x5d39); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp
}

// 0d:5d3d, bare global; jump-table target from enemyCode24.
void likelike_state_galeSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d3d, ecom_galeSeedEffect_b0d_hook, 0x447b, 0x5d40);
  if (F & FC) { RET_TAKEN(0x5d40); return; } // ret c
  CYC(0x5d40, 0x5d41);
  CYC(0x5d41, 0x5d43); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x5d43, 0x5d44); A = mem_rd(gb, DE);
  CYC(0x5d44, 0x5d45); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5d45, 0x5d47); goto decNum; } // jr z
  CYC(0x5d45, 0x5d47);
  CYC(0x5d47, 0x5d48); H = A;
  CYC(0x5d48, 0x5d4a); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5d4a, 0x5d4b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

decNum:
  CALL_C(0x5d4b, decNumEnemies_hook, 0x24b3, 0x5d4e);
  CYC(0x5d4e, 0x5d51); enemyDelete_hook(gb); return; // jp
}

// 0d:5d51, bare global; jump-table target from enemyCode24.
void likelike_state_stub_hook(GB *gb) {
  RET(0x5d51); return; // ret
}

// 0d:5d52, bare global; jump-table target from enemyCode24@normalState.
void likelike_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d52, 0x5d53); A = mem_rd(gb, DE);
  CYC(0x5d53, 0x5d55); alu_sub(gb, 0x08);
  {
    CYC(0x5d55, 0x5d56); push_effect(gb, 0x5d56);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x5d60) { likelike_subid00_state8_hook(gb); return; }
    if (target == 0x5d67) { likelike_state9_hook(gb); return; }
    if (target == 0x5d7c) { likelike_stateA_hook(gb); return; }
    if (target == 0x5d8f) { likelike_stateB_hook(gb); return; }
    if (target == 0x5dd4) { likelike_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5d60, bare global; jump-table target from likelike_subid00. Initialization; falls
// through into likelike_state9.
void likelike_subid00_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d60, 0x5d61); H = D;
  CYC(0x5d61, 0x5d62); L = E;
  CYC(0x5d62, 0x5d63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5d63, 0x5d65); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5d65, 0x5d67); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  likelike_state9_hook(gb); return; // fallthrough
}

// 0d:5d67, bare global; jump-table target shared by likelike_subid00/02/03, also falls
// into from likelike_subid00_state8. Choosing a new direction & duration.
void likelike_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d67, 0x5d68); H = D;
  CYC(0x5d68, 0x5d69); L = E;
  CYC(0x5d69, 0x5d6a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5d6a, 0x5d6d); SET_BC(0x1830);
  CALL_C(0x5d6d, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5d70);
  CYC(0x5d70, 0x5d72); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5d72, 0x5d73); A = B;
  CYC(0x5d73, 0x5d74); mem_wr(gb, DE, A);
  CYC(0x5d74, 0x5d76); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5d76, 0x5d78); A = 0x38;
  CYC(0x5d78, 0x5d79); alu_add(gb, C);
  CYC(0x5d79, 0x5d7a); mem_wr(gb, DE, A);
  CYC(0x5d7a, 0x5d7c); likelike_animate_hook(gb); return; // jr
}

// 0d:5d7c, bare global; jump-table target shared by likelike_subid00/02/03. Moving in some
// direction for [counter1] frames. Internal @newDirection/@move are @-local.
void likelike_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d7c, ecom_decCounter1_b0d_hook, 0x439a, 0x5d7f);
  if (!(F & FZ)) { CYCT(0x5d7f, 0x5d81); goto move; } // jr nz
  CYC(0x5d7f, 0x5d81);

newDirection:
  CYC(0x5d81, 0x5d82); H = D;
  CYC(0x5d82, 0x5d84); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5d84, 0x5d85); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5d85, 0x5d87); likelike_animate_hook(gb); return; // jr

move:
  CALL_C(0x5d87, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x5d8a);
  if (F & FZ) { CYCT(0x5d8a, 0x5d8c); goto newDirection; } // jr z
  CYC(0x5d8a, 0x5d8c);
  likelike_animate_hook(gb); return; // fallthrough
}

// 0d:5d8c, bare global; called from likelike_state9/stateA/stateB/stateC and their
// subid02/03 aliases.
void likelike_animate_hook(GB *gb) {
  CYC(0x5d8c, 0x5d8f); enemyAnimate_hook(gb); return; // jp
}

// 0d:5d8f, bare global; jump-table target shared by likelike_subid00/03. Eating Link.
// Internal @releaseLink is @-local; falls through into likelike_releaseLink.
void likelike_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d8f, ecom_decCounter2_b0d_hook, 0x43a3, 0x5d92);
  if (F & FZ) { CYCT(0x5d92, 0x5d94); goto releaseLink; } // jr z
  CYC(0x5d92, 0x5d94);
  CYC(0x5d94, 0x5d97); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(0x5d97, 0x5d98); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5d98, 0x5d9a); likelike_animate_hook(gb); return; } // jr z
  CYC(0x5d98, 0x5d9a);
  CYC(0x5d9a, 0x5d9b); L = alu_dec8(gb, L);
  CYC(0x5d9b, 0x5d9c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1]++
  CYC(0x5d9c, 0x5d9e); likelike_animate_hook(gb); return; // jr

releaseLink:
  CYC(0x5d9e, 0x5da0); mem_wr(gb, HL, 60);
  CYC(0x5da0, 0x5da2); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5da2, 0x5da3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5da3, 0x5da5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5da5, 0x5da6); A = mem_rd(gb, HL);
  CYC(0x5da6, 0x5da8); alu_cp(gb, 19);
  if (!(F & FC)) { CYCT(0x5da8, 0x5daa); goto afterShieldCheck; } // jr nc
  CYC(0x5da8, 0x5daa);
  CYC(0x5daa, 0x5dac); A = 0x01; // TREASURE_SHIELD
  CALL_C(0x5dac, checkTreasureObtained_hook, 0x1748, 0x5daf);
  if (!(F & FC)) { CYCT(0x5daf, 0x5db1); goto afterShieldCheck; } // jr nc
  CYC(0x5daf, 0x5db1);
  CYC(0x5db1, 0x5db3); A = 0x01; // TREASURE_SHIELD
  CALL_C(0x5db3, loseTreasure_hook, 0x1733, 0x5db6);
  CYC(0x5db6, 0x5db9); SET_BC(0x510b); // TX_510b
  CALL_C(0x5db9, showText_hook, 0x1872, 0x5dbc);

afterShieldCheck:
  CALL_C(0x5dbc, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5dbf);
  CYC(0x5dbf, 0x5dc1); alu_and(gb, 0x18);
  CYC(0x5dc1, 0x5dc3); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5dc3, 0x5dc4); mem_wr(gb, DE, A);
  CALL_C(0x5dc4, objectSetVisiblec2_hook, 0x1e45, 0x5dc7);
  likelike_releaseLink_hook(gb); return; // fallthrough
}

// 0d:5dc7, bare global; called from enemyCode24 and likelike_checkHazards, also falls into
// from likelike_stateB.
void likelike_releaseLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5dc7, 0x5dca); SET_HL(w1Link + OBJ_SUBSTATE);
  CYC(0x5dca, 0x5dcc); mem_wr(gb, HL, 0x04);
  CYC(0x5dcc, 0x5dce); L = OBJ_COLLISION_TYPE; // <w1Link.collisionType
  CYC(0x5dce, 0x5dd0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5dd0, 0x5dd1); alu_xor(gb, A);
  CYC(0x5dd1, 0x5dd4); enemySetAnimation_hook(gb); return; // jp
}

// 0d:5dd4, bare global; jump-table target shared by likelike_subid00/02/03. Cooldown after
// eating Link.
void likelike_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5dd4, ecom_decCounter2_b0d_hook, 0x43a3, 0x5dd7);
  if (!(F & FZ)) { CYCT(0x5dd7, 0x5dd9); goto ranIntoWall; } // jr nz
  CYC(0x5dd7, 0x5dd9);
  CYC(0x5dd9, 0x5dda); L = E;
  CYC(0x5dda, 0x5ddb); A = mem_rd(gb, HL);
  CYC(0x5ddb, 0x5ddd); alu_sub(gb, 0x03);
  CYC(0x5ddd, 0x5dde); mem_wr(gb, HL, A); // [state] -= 3
  CYC(0x5dde, 0x5de0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5de0, 0x5de2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5de2, 0x5de4); likelike_animate_hook(gb); return; // jr

ranIntoWall:
  CALL_C(0x5de4, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x5de7);
  if (!(F & FZ)) { CYCT(0x5de7, 0x5de9); likelike_animate_hook(gb); return; } // jr nz
  CYC(0x5de7, 0x5de9);
  CALL_C(0x5de9, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5dec);
  CYC(0x5dec, 0x5dee); alu_and(gb, 0x18);
  CYC(0x5dee, 0x5df0); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5df0, 0x5df1); mem_wr(gb, DE, A);
  CYC(0x5df1, 0x5df3); likelike_animate_hook(gb); return; // jr
}

// 0d:5df3, bare global; jump-table target from enemyCode24@normalState. Like-like spawner.
// Internal @state8/@state9/@stateA/@fromBottom/@fromTop are @-local.
void likelike_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5df3, 0x5df4); A = mem_rd(gb, DE);
  CYC(0x5df4, 0x5df6); alu_sub(gb, 0x08);
  {
    CYC(0x5df6, 0x5df7); push_effect(gb, 0x5df7);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x5dfd) goto state8;
    if (target == 0x5e06) goto state9;
    if (target == 0x5e19) goto stateA;
    HANDOFF(target);
  }

state8:
  CYC(0x5dfd, 0x5dfe); H = D;
  CYC(0x5dfe, 0x5dff); L = E;
  CYC(0x5dff, 0x5e00); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5e00, 0x5e02); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5e02, 0x5e03); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5e03, 0x5e06); likelike_findAllLikelikesWithSubid0_hook(gb); return; // jp

state9:
  CYC(0x5e06, 0x5e09); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(0x5e09, 0x5e0b); alu_sub(gb, 0x10);
  CYC(0x5e0b, 0x5e0d); alu_cp(gb, 0x60); // (SMALL_ROOM_HEIGHT<<4)-$20
  if (!(F & FC)) { RET_TAKEN(0x5e0d); return; } // ret nc
  CYC(0x5e0d, 0x5e0e);
  CYC(0x5e0e, 0x5e11); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(0x5e11, 0x5e13); alu_sub(gb, 0x10);
  CYC(0x5e13, 0x5e15); alu_cp(gb, 0x80); // (SMALL_ROOM_WIDTH<<4)-$20
  if (!(F & FC)) { RET_TAKEN(0x5e15); return; } // ret nc
  CYC(0x5e15, 0x5e16);
  CYC(0x5e16, 0x5e18); A = 0x0a;
  CYC(0x5e18, 0x5e19); mem_wr(gb, DE, A); // [state] = $0a

stateA:
  CALL_C(0x5e19, ecom_decCounter1_b0d_hook, 0x439a, 0x5e1c);
  if (!(F & FZ)) { RET_TAKEN(0x5e1c); return; } // ret nz
  CYC(0x5e1c, 0x5e1d);
  CYC(0x5e1d, 0x5e1e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(0x5e1e, 0x5e20); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5e20, 0x5e21); A = mem_rd(gb, HL);
  CYC(0x5e21, 0x5e23); alu_cp(gb, 0x06);
  if (!(F & FC)) { RET_TAKEN(0x5e23); return; } // ret nc
  CYC(0x5e23, 0x5e24);
  CALL_C(0x5e24, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5e27);
  CYC(0x5e27, 0x5e29); alu_and(gb, 0x02);
  CYC(0x5e29, 0x5e2a); C = A;
  CYC(0x5e2a, 0x5e2d); A = mem_rd(gb, wActiveRoom);
  CYC(0x5e2d, 0x5e2f); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(0x5e2f, 0x5e31); goto fromTop; } // jr z
  CYC(0x5e2f, 0x5e31);
  CYC(0x5e31, 0x5e33); alu_cp(gb, 0x40);
  if (F & FZ) { CYCT(0x5e33, 0x5e35); goto fromBottom; } // jr z
  CYC(0x5e33, 0x5e35);
  CYC(0x5e35, 0x5e37); C = (uint8_t)(C | (1 << 2)); // set 2,c
  CYC(0x5e37, 0x5e39); alu_cp(gb, 0x51);
  if (!(F & FZ)) { RET_TAKEN(0x5e39); return; } // ret nz
  CYC(0x5e39, 0x5e3a);

fromBottom:
  CYC(0x5e3a, 0x5e3c); E = 0x02;
  CALL_C(0x5e3c, likelike_spawn_hook, 0x5ec9, 0x5e3f);
  if (!(F & FZ)) { RET_TAKEN(0x5e3f); return; } // ret nz
  CYC(0x5e3f, 0x5e40);
  CALL_C(0x5e40, likelike_setChildSpawnPosition_hook, 0x5ed8, 0x5e43);
  CYC(0x5e43, 0x5e45); goto spawnedOk; // jr

fromTop:
  CYC(0x5e45, 0x5e47); E = 0x03;
  CALL_C(0x5e47, likelike_spawn_hook, 0x5ec9, 0x5e4a);
  if (!(F & FZ)) { RET_TAKEN(0x5e4a); return; } // ret nz

spawnedOk:
  CYC(0x5e4b, 0x5e4c); H = D;
  CYC(0x5e4c, 0x5e4e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5e4e, 0x5e4f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5e4f, 0x5e51); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5e51, 0x5e53); mem_wr(gb, HL, 120);
  RET(0x5e53); return; // ret
}

// 0d:5e54, bare global; jump-table target from enemyCode24@normalState. Internal @state8/
// @state9 are @-local; other table entries alias likelike_state9/stateA/stateB/stateC.
void likelike_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e54, 0x5e55); A = mem_rd(gb, DE);
  CYC(0x5e55, 0x5e57); alu_sub(gb, 0x08);
  {
    CYC(0x5e57, 0x5e58); push_effect(gb, 0x5e58);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x5e64) goto state8;
    if (target == 0x5e77) goto state9;
    if (target == 0x5d67) { likelike_state9_hook(gb); return; }
    if (target == 0x5d7c) { likelike_stateA_hook(gb); return; }
    if (target == 0x5d8f) { likelike_stateB_hook(gb); return; }
    if (target == 0x5dd4) { likelike_stateC_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x5e64, 0x5e65); H = D;
  CYC(0x5e65, 0x5e66); L = E;
  CYC(0x5e66, 0x5e67); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5e67, 0x5e69); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5e69, 0x5e6a); A = mem_rd(gb, HL);
  CYC(0x5e6a, 0x5e6c); alu_cp(gb, 0x88); // (SMALL_ROOM_HEIGHT<<4)+8
  if (F & FZ) { CYCT(0x5e6c, 0x5e6e); goto state8SetCounter; } // jr z
  CYC(0x5e6c, 0x5e6e);
  CYC(0x5e6e, 0x5e70); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5e70, 0x5e72); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT

state8SetCounter:
  CYC(0x5e72, 0x5e74); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5e74, 0x5e76); mem_wr(gb, HL, 45);
  RET(0x5e76); return; // ret

state9:
  CALL_C(0x5e77, ecom_decCounter1_b0d_hook, 0x439a, 0x5e7a);
  if (F & FZ) { CYCT(0x5e7a, 0x5e7c); goto state9SetCollision; } // jr z
  CYC(0x5e7a, 0x5e7c);
  CALL_C(0x5e7c, objectApplySpeed_hook, 0x201d, 0x5e7f);
  CYC(0x5e7f, 0x5e81); likelike_animate2_hook(gb); return; // jr

state9SetCollision:
  CYC(0x5e81, 0x5e82); L = E;
  CYC(0x5e82, 0x5e83); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5e83, 0x5e85); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5e85, 0x5e87); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  likelike_animate2_hook(gb); return; // fallthrough
}

// 0d:5e87, bare global; called from likelike_subid02/03.
void likelike_animate2_hook(GB *gb) {
  CYC(0x5e87, 0x5e8a); enemyAnimate_hook(gb); return; // jp
}

// 0d:5e8a, bare global; jump-table target from enemyCode24@normalState. Internal @state8/
// @state9/@stateB are @-local; other table entries alias likelike_state9/stateA/stateC.
void likelike_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e8a, 0x5e8b); A = mem_rd(gb, DE);
  CYC(0x5e8b, 0x5e8d); alu_sub(gb, 0x08);
  {
    CYC(0x5e8d, 0x5e8e); push_effect(gb, 0x5e8e);
    uint16_t target = likelike_jump_table(gb);
    if (target == 0x5e9a) goto state8;
    if (target == 0x5eac) goto state9;
    if (target == 0x5d67) { likelike_state9_hook(gb); return; }
    if (target == 0x5d7c) { likelike_stateA_hook(gb); return; }
    if (target == 0x5ebb) goto stateB;
    if (target == 0x5dd4) { likelike_stateC_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CALL_C(0x5e9a, likelike_chooseRandomPosition_hook, 0x5f14, 0x5e9d);
  if (!(F & FZ)) { RET_TAKEN(0x5e9d); return; } // ret nz
  CYC(0x5e9d, 0x5e9e);
  CYC(0x5e9e, 0x5ea0); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5ea0, 0x5ea1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5ea1, 0x5ea3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5ea3, 0x5ea5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5ea5, 0x5ea7); L = ENEMY_BASE + OBJ_SPEED_Z + 1;
  CYC(0x5ea7, 0x5ea9); mem_wr(gb, HL, 0x02);
  CYC(0x5ea9, 0x5eac); objectSetVisiblec1_hook(gb); return; // jp

state9:
  CYC(0x5eac, 0x5eae); C = 0x08;
  CALL_C(0x5eae, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5eb1);
  if (!(F & FZ)) { CYCT(0x5eb1, 0x5eb3); likelike_animate2_hook(gb); return; } // jr nz
  CYC(0x5eb1, 0x5eb3);
  CYC(0x5eb3, 0x5eb5); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5eb5, 0x5eb6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x5eb6, objectSetVisiblec2_hook, 0x1e45, 0x5eb9);
  CYC(0x5eb9, 0x5ebb); likelike_animate2_hook(gb); return; // jr

stateB:
  CYC(0x5ebb, 0x5ebd); C = 0x08;
  CALL_C(0x5ebd, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5ec0);
  CYC(0x5ec0, 0x5ec2); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5ec2, 0x5ec3); A = mem_rd(gb, HL);
  CYC(0x5ec3, 0x5ec6); mem_wr(gb, w1Link + OBJ_ZH, A);
  CYC(0x5ec6, 0x5ec9); likelike_stateB_hook(gb); return; // jp
}

// 0d:5ec9, bare global; called from likelike_subid01. Spawner (subid 1) calls this to make
// new like-likes where their relatedObj1 references the spawner.
// @param e Subid of like-like to spawn
void likelike_spawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ec9, 0x5ecb); B = 0x24; // ENEMY_LIKE_LIKE
  CALL_C(0x5ecb, ecom_spawnEnemyWithSubid01_b0d_hook, 0x4373, 0x5ece);
  if (!(F & FZ)) { RET_TAKEN(0x5ece); return; } // ret nz
  CYC(0x5ece, 0x5ecf);
  CYC(0x5ecf, 0x5ed0); mem_wr(gb, HL, E);
  CYC(0x5ed0, 0x5ed2); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5ed2, 0x5ed4); A = ENEMY_BASE; // Enemy.start
  CYC(0x5ed4, 0x5ed5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5ed5, 0x5ed6); mem_wr(gb, HL, D);
  CYC(0x5ed6, 0x5ed7); alu_xor(gb, A);
  RET(0x5ed7); return; // ret
}

// 0d:5ed8, bare global; called from likelike_subid01.
// @param c Index of spawn position to use
void likelike_setChildSpawnPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(0x5ed8, HL);
  CYC(0x5ed9, 0x5eda); A = C;
  CYC(0x5eda, 0x5edd); SET_HL(0x5ee9); // @spawnPositions
  CYC(0x5edd, 0x5ede); likelike_addAToHl_from_rst(gb, 0x5ede);
  CYC(0x5ede, 0x5edf); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x5edf, 0x5ee0); B = A;
  CYC(0x5ee0, 0x5ee1); C = mem_rd(gb, HL);
  SET_HL(POP(0x5ee1));
  CYC(0x5ee2, 0x5ee4); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5ee4, 0x5ee5); mem_wr(gb, HL, B);
  CYC(0x5ee5, 0x5ee7); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5ee7, 0x5ee8); mem_wr(gb, HL, C);
  RET(0x5ee8); return; // ret
}

// 0d:5ef1, bare global; called from likelike_subid01. Searches for all existing like-likes
// with subid 0, sets their relatedObj1 to point to this object (the spawner), and stores
// the current like-like count in var30.
void likelike_findAllLikelikesWithSubid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ef1, 0x5ef4); SET_HL(ENEMY_SLOTS + OBJ_ID); // FIRST_ENEMY_INDEX, Enemy.id
  CYC(0x5ef4, 0x5ef6); C = 0x00;

nextEnemy:
  CYC(0x5ef6, 0x5ef7); A = mem_rd(gb, HL);
  CYC(0x5ef7, 0x5ef9); alu_cp(gb, 0x24); // ENEMY_LIKE_LIKE
  if (!(F & FZ)) { CYCT(0x5ef9, 0x5efb); goto loopIncrement; } // jr nz
  CYC(0x5ef9, 0x5efb);
  CYC(0x5efb, 0x5efc); L = alu_inc8(gb, L);
  CYC(0x5efc, 0x5efd); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd
  CYC(0x5efd, 0x5efe); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5efe, 0x5f00); goto loopIncrement; } // jr nz
  CYC(0x5efe, 0x5f00);
  CYC(0x5f00, 0x5f02); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5f02, 0x5f04); A = ENEMY_BASE; // Enemy.start
  CYC(0x5f04, 0x5f05); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5f05, 0x5f06); mem_wr(gb, HL, D);
  CYC(0x5f06, 0x5f08); L = ENEMY_BASE + OBJ_ID;
  CYC(0x5f08, 0x5f09); C = alu_inc8(gb, C);

loopIncrement:
  CYC(0x5f09, 0x5f0a); H = alu_inc8(gb, H);
  CYC(0x5f0a, 0x5f0b); A = H;
  CYC(0x5f0b, 0x5f0d); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x5f0d, 0x5f0f); goto nextEnemy; } // jr c
  CYC(0x5f0d, 0x5f0f);
  CYC(0x5f0f, 0x5f11); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5f11, 0x5f12); A = C;
  CYC(0x5f12, 0x5f13); mem_wr(gb, DE, A);
  RET(0x5f13); return; // ret
}

// 0d:5f14, bare global; called from likelike_subid03. Choose a random position to fall
// from the sky. If a good position is chosen, the Z position is also set to be above the
// screen.
// @param[out] zflag z if chose valid position
void likelike_chooseRandomPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f14, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5f17);
  CYC(0x5f17, 0x5f19); alu_and(gb, 0x77);
  CYC(0x5f19, 0x5f1a); A = alu_inc8(gb, A);
  CYC(0x5f1a, 0x5f1b); C = A;
  CYC(0x5f1b, 0x5f1d); B = 0xce; // >wRoomCollisions
  CYC(0x5f1d, 0x5f1e); A = mem_rd(gb, BC);
  CYC(0x5f1e, 0x5f1f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5f1f); return; } // ret nz
  CYC(0x5f1f, 0x5f20);
  CALL_C(0x5f20, objectSetShortPosition_hook, 0x20c3, 0x5f23);
  CYC(0x5f23, 0x5f25); C = 0x08;
  CALL_C(0x5f25, ecom_setZAboveScreen_b0d_hook, 0x4446, 0x5f28);
  CYC(0x5f28, 0x5f29); alu_xor(gb, A);
  RET(0x5f29); return; // ret
}

// 0d:5f2a, bare global; called from enemyCode24.
void likelike_checkHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(0x5f2a, AF);
  CYC(0x5f2b, 0x5f2e); A = mem_rd(gb, w1Link + OBJ_STATE);
  CYC(0x5f2e, 0x5f30); alu_cp(gb, 0x0d); // LINK_STATE_GRABBED
  if (!(F & FZ)) { CYCT(0x5f30, 0x5f32); goto restoreAf; } // jr nz
  CYC(0x5f30, 0x5f32);
  CYC(0x5f32, 0x5f34); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x5f34, 0x5f35); A = mem_rd(gb, DE);
  CYC(0x5f35, 0x5f36); alu_rlca(gb);
  if (F & FC) { CYCT(0x5f36, 0x5f38); goto restoreAf; } // jr c
  CYC(0x5f36, 0x5f38);
  CYC(0x5f38, 0x5f3b); SET_BC(0x0500);
  CALL_C(0x5f3b, objectGetRelativeTile_hook, 0x1435, 0x5f3e);
  CYC(0x5f3e, 0x5f41); SET_HL(hazardCollisionTable);
  CALL_C(0x5f41, lookupCollisionTable_hook, 0x1e1f, 0x5f44);
  if (F & FC) CALL_C_CC(0x5f44, likelike_releaseLink_hook, 0x5dc7, 0x5f47); else CYC(0x5f44, 0x5f47); // call c

restoreAf:
  SET_AF(POP(0x5f47));
  CYC(0x5f48, 0x5f4b); ecom_checkHazards_b0d_hook(gb); return; // jp
}
