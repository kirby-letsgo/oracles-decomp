#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void bari_state_uninitialized_hook(GB *gb);
void bari_state_stub_hook(GB *gb);
void bari_subid0_hook(GB *gb);
void bari_subid0_state8_hook(GB *gb);
void bari_applySpeed_hook(GB *gb);
void bari_animate_hook(GB *gb);
void bari_state9_hook(GB *gb);
void bari_setRandomAngleAndCounter2_hook(GB *gb);
void bari_subid0_stateA_hook(GB *gb);
void bari_subid0_stateA_spawnSmallBari_hook(GB *gb);
void bari_subid1_hook(GB *gb);
void bari_updateZPosition_hook(GB *gb);

static uint16_t bari_jump_table(GB *gb) {
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

static void bari_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_BARI
//
// Variables:
//   var30/var31: Initial Y/X position (aka target position; they always hover around this
//                area. For subid 0 (large baris) only.)
//   var32: Counter for "bobbing" of Z position
// ==================================================================================================
void enemyCode3c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6be9, 0x6beb); goto normalStatus; } // jr z
  CYC(0x6be9, 0x6beb);
  CYC(0x6beb, 0x6bed); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6bed); return; } // ret c
  CYC(0x6bed, 0x6bee);
  if (F & FZ) { CYCT(0x6bee, 0x6bf1); enemyDie_hook(gb); return; } // jp z
  CYC(0x6bee, 0x6bf1);
  CYC(0x6bf1, 0x6bf2); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6bf2, 0x6bf5); ecom_updateKnockback_b0e_hook(gb); return; } // jp nz
  CYC(0x6bf2, 0x6bf5);
  CYC(0x6bf5, 0x6bf7); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6bf7, 0x6bf8); A = mem_rd(gb, DE);
  CYC(0x6bf8, 0x6bfa); alu_cp(gb, 0x9e); // $80|ITEMCOLLISION_GALE_SEED
  if (F & FZ) { CYCT(0x6bfa, 0x6bfc); goto normalStatus; } // jr z
  CYC(0x6bfa, 0x6bfc);
  CYC(0x6bfc, 0x6bfe); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6bfe, 0x6bff); A = mem_rd(gb, DE);
  CYC(0x6bff, 0x6c00); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6c00); return; } // ret z
  CYC(0x6c00, 0x6c01);
  CYC(0x6c01, 0x6c03); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6c03, 0x6c04); A = mem_rd(gb, DE);
  CYC(0x6c04, 0x6c05); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6c05, 0x6c07); goto normalStatus; } // jr nz
  CYC(0x6c05, 0x6c07);
  CYC(0x6c07, 0x6c09); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6c09, 0x6c0a); A = mem_rd(gb, DE);
  CYC(0x6c0a, 0x6c0c); alu_cp(gb, 0x59); // ENEMYCOLLISION_BARI_ELECTRIC_SHOCK
  if (F & FZ) { CYCT(0x6c0c, 0x6c0e); goto normalStatus; } // jr z
  CYC(0x6c0c, 0x6c0e);
  CYC(0x6c0e, 0x6c10); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6c10, 0x6c11); A = mem_rd(gb, DE);
  CYC(0x6c11, 0x6c13); alu_cp(gb, 0x83); // $80|ITEMCOLLISION_L3_SHIELD
  if (F & FC) { CYCT(0x6c13, 0x6c15); goto normalStatus; } // jr c
  CYC(0x6c13, 0x6c15);
  CYC(0x6c15, 0x6c16); H = D;
  CYC(0x6c16, 0x6c18); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6c18, 0x6c1a); mem_wr(gb, HL, 0x0a);

normalStatus:
  CALL_C(0x6c1a, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x6c1d);
  if (F & FC) { CYCT(0x6c1d, 0x6c1f); goto commonState; } // jr c
  CYC(0x6c1d, 0x6c1f);
  CALL_C(0x6c1f, bari_updateZPosition_hook, 0x6d39, 0x6c22);
  CYC(0x6c22, 0x6c24); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6c24, 0x6c25); A = B;
  CYC(0x6c25, 0x6c26); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6c26, 0x6c29); bari_subid0_hook(gb); return; } // jp z
  CYC(0x6c26, 0x6c29);
  CYC(0x6c29, 0x6c2c); bari_subid1_hook(gb); return; // jp

commonState:
  CYC(0x6c2c, 0x6c2d); A = mem_rd(gb, DE);
  {
    CYC(0x6c2d, 0x6c2e); push_effect(gb, 0x6c2e);
    uint16_t target = bari_jump_table(gb);
    if (target == 0x6c3e) { bari_state_uninitialized_hook(gb); return; }
    if (target == 0x6c6c) { bari_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6c3e, bare global; jump-table target from enemyCode3c@commonState.
void bari_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c3e, 0x6c40); A = 0x0f; // SPEED_60
  CALL_C(0x6c40, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x6c43);
  CYC(0x6c43, 0x6c45); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6c45, 0x6c47); mem_wr(gb, HL, 0x04);
  CYC(0x6c47, 0x6c49); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x6c49, 0x6c4b); mem_wr(gb, HL, 0xfc);
  CYC(0x6c4b, 0x6c4d); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6c4d, 0x6c4f); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6c4f, 0x6c50); A = mem_rd(gb, DE);
  CYC(0x6c50, 0x6c51); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c51, 0x6c53); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6c53, 0x6c54); A = mem_rd(gb, DE);
  CYC(0x6c54, 0x6c55); mem_wr(gb, HL, A);
  CALL_C(0x6c55, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c58);
  CYC(0x6c58, 0x6c5a); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6c5a, 0x6c5b); mem_wr(gb, DE, A);
  CYC(0x6c5b, 0x6c5d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6c5d, 0x6c5e); A = mem_rd(gb, DE);
  CYC(0x6c5e, 0x6c5f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6c5f, 0x6c62); bari_setRandomAngleAndCounter2_hook(gb); return; } // jp z
  CYC(0x6c5f, 0x6c62);
  CYC(0x6c62, 0x6c64); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6c64, 0x6c66); A = 0x0a; // SPEED_40
  CYC(0x6c66, 0x6c67); mem_wr(gb, DE, A);
  CYC(0x6c67, 0x6c69); A = 0x02;
  CYC(0x6c69, 0x6c6c); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6c6c, bare global; jump-table target from enemyCode3c@commonState.
void bari_state_stub_hook(GB *gb) {
  RET(0x6c6c); return; // ret
}

// 0e:6c6d, bare global; called from enemyCode3c.
void bari_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c6d, 0x6c6e); A = mem_rd(gb, DE);
  CYC(0x6c6e, 0x6c70); alu_sub(gb, 0x08);
  {
    CYC(0x6c70, 0x6c71); push_effect(gb, 0x6c71);
    uint16_t target = bari_jump_table(gb);
    if (target == 0x6c77) { bari_subid0_state8_hook(gb); return; }
    if (target == 0x6caa) { bari_state9_hook(gb); return; }
    if (target == 0x6cd0) { bari_subid0_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6c77, bare global; jump-table target from bari_subid0. "Non-electric-shock" state.
void bari_subid0_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c77, ecom_decCounter2_b0e_hook, 0x43a3, 0x6c7a);
  if (!(F & FZ)) { CYCT(0x6c7a, 0x6c7c); goto dontShockYet; } // jr nz
  CYC(0x6c7a, 0x6c7c);
  CYC(0x6c7c, 0x6c7e); mem_wr(gb, HL, 0x3c); // [counter2] = 60
  CYC(0x6c7e, 0x6c7f); L = E;
  CYC(0x6c7f, 0x6c80); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6c80, 0x6c82); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6c82, 0x6c84); mem_wr(gb, HL, 0x59); // ENEMYCOLLISION_BARI_ELECTRIC_SHOCK
  CYC(0x6c84, 0x6c86); A = 0x01;
  CYC(0x6c86, 0x6c89); enemySetAnimation_hook(gb); return; // jp

dontShockYet:
  CALL_C(0x6c89, ecom_decCounter1_b0e_hook, 0x439a, 0x6c8c);
  if (!(F & FZ)) { CYCT(0x6c8c, 0x6c8e); bari_applySpeed_hook(gb); return; } // jr nz
  CYC(0x6c8c, 0x6c8e);
  CALL_C(0x6c8e, getRandomNumber_hook, 0x043e, 0x6c91);
  CYC(0x6c91, 0x6c93); alu_and(gb, 0x0e);
  CYC(0x6c93, 0x6c95); alu_add(gb, 0x02);
  CYC(0x6c95, 0x6c96); mem_wr(gb, HL, A); // [counter1]
  CYC(0x6c96, 0x6c98); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(0x6c98, ecom_readPositionVars_b0e_hook, 0x4439, 0x6c9b);
  CALL_C(0x6c9b, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x6c9e);
  CALL_C(0x6c9e, objectNudgeAngleTowards_hook, 0x1fd4, 0x6ca1);
  bari_applySpeed_hook(gb); return; // fallthrough
}

// 0e:6ca1, bare global; falls into from bari_subid0_state8, also reached by genuine jr/jp from
// bari_subid1. Falls through into bari_animate.
void bari_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6ca1, objectApplySpeed_hook, 0x201d, 0x6ca4);
  CALL_C(0x6ca4, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x6ca7);
  bari_animate_hook(gb); return; // fallthrough
}

// 0e:6ca7, bare global; falls into from bari_applySpeed, also reached by genuine jr from
// bari_state9.
void bari_animate_hook(GB *gb) {
  CYC(0x6ca7, 0x6caa); enemyAnimate_hook(gb); return; // jp
}

// 0e:6caa, bare global; jump-table target from bari_subid0, also called from bari_subid1. In
// its "electric shock" state. This is shared between subids 0 and 1 (large and small). Falls
// through into bari_setRandomAngleAndCounter2.
void bari_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6caa, ecom_decCounter2_b0e_hook, 0x43a3, 0x6cad);
  if (!(F & FZ)) { CYCT(0x6cad, 0x6caf); bari_animate_hook(gb); return; } // jr nz
  CYC(0x6cad, 0x6caf);
  CYC(0x6caf, 0x6cb0); L = E;
  CYC(0x6cb0, 0x6cb1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = 8
  CYC(0x6cb1, 0x6cb3); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6cb3, 0x6cb5); mem_wr(gb, HL, 0x2d); // ENEMYCOLLISION_BARI
  CYC(0x6cb5, 0x6cb6); L = alu_dec8(gb, L);
  CYC(0x6cb6, 0x6cb8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x6cb8, 0x6cb9); alu_xor(gb, A);
  CALL_C(0x6cb9, enemySetAnimation_hook, 0x282b, 0x6cbc);
  bari_setRandomAngleAndCounter2_hook(gb); return; // fallthrough
}

// 0e:6cbc, bare global; called from bari_state_uninitialized, also falls into from
// bari_state9.
void bari_setRandomAngleAndCounter2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6cbc, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6cbf);
  CYC(0x6cbf, 0x6cc1); alu_and(gb, 0x03);
  CYC(0x6cc1, 0x6cc4); SET_HL(0x6ccc); // @counter2Vals
  CYC(0x6cc4, 0x6cc5); bari_addAToHl_from_rst(gb, 0x6cc5);
  CYC(0x6cc5, 0x6cc7); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6cc7, 0x6cc8); A = mem_rd(gb, HL);
  CYC(0x6cc8, 0x6cc9); mem_wr(gb, DE, A);
  CYC(0x6cc9, 0x6ccc); ecom_setRandomAngle_b0e_hook(gb); return; // jp
}

// 0e:6cd0, bare global; jump-table target from bari_subid0. Bari has just been attacked; now
// it's splitting in two.
void bari_subid0_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6cd0, 0x6cd1); E = alu_inc8(gb, E);
  CYC(0x6cd1, 0x6cd2); A = mem_rd(gb, DE); // [substate]
  CYC(0x6cd2, 0x6cd3); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6cd3, 0x6cd5); goto substate0; } // jr z
  CYC(0x6cd3, 0x6cd5);

  CALL_C(0x6cd5, ecom_decCounter2_b0e_hook, 0x43a3, 0x6cd8);
  if (!(F & FZ)) { RET_TAKEN(0x6cd8); return; } // ret nz
  CYC(0x6cd8, 0x6cd9);
  CALL_C(0x6cd9, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x6cdc);
  CYC(0x6cdc, 0x6cde); C = 0x04;
  CALL_C(0x6cde, bari_subid0_stateA_spawnSmallBari_hook, 0x6cec, 0x6ce1);
  CYC(0x6ce1, 0x6ce3); C = 0xfc;
  CALL_C(0x6ce3, bari_subid0_stateA_spawnSmallBari_hook, 0x6cec, 0x6ce6);
  CALL_C(0x6ce6, decNumEnemies_hook, 0x24b3, 0x6ce9);
  CYC(0x6ce9, 0x6cec); enemyDelete_hook(gb); return; // jp

substate0:
  CYC(0x6d04, 0x6d06); B = 0x08; // INTERAC_KILLENEMYPUFF
  CALL_C(0x6d06, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x6d09);
  CYC(0x6d09, 0x6d0a); H = D;
  CYC(0x6d0a, 0x6d0c); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6d0c, 0x6d0e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6d0e, 0x6d10); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6d10, 0x6d12); mem_wr(gb, HL, 0x04);
  CYC(0x6d12, 0x6d14); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6d14, 0x6d15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d15, 0x6d17); A = 0x73; // SND_KILLENEMY
  CALL_C(0x6d17, playSound_b00_hook, 0x0c98, 0x6d1a);
  CYC(0x6d1a, 0x6d1d); objectSetInvisible_hook(gb); return; // jp
}

// 0e:6cec, bare local (no exported symbol); called via genuine call/ret twice from
// bari_subid0_stateA.
// @param c X-offset (and value to add to angle)
void bari_subid0_stateA_spawnSmallBari_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6cec, 0x6cee); B = 0x3c; // ENEMY_BARI
  CALL_C(0x6cee, ecom_spawnEnemyWithSubid01_b0e_hook, 0x4373, 0x6cf1);
  if (!(F & FZ)) { RET_TAKEN(0x6cf1); return; } // ret nz
  CYC(0x6cf1, 0x6cf2);
  CYC(0x6cf2, 0x6cf4); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x6cf4, 0x6cf5); E = L;
  CYC(0x6cf5, 0x6cf6); A = mem_rd(gb, DE);
  CYC(0x6cf6, 0x6cf7); mem_wr(gb, HL, A);
  CYC(0x6cf7, 0x6cf9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6cf9, 0x6cfa); E = L;
  CYC(0x6cfa, 0x6cfb); A = mem_rd(gb, DE);
  CYC(0x6cfb, 0x6cfc); alu_add(gb, C);
  CYC(0x6cfc, 0x6cfe); alu_and(gb, 0x1f);
  CYC(0x6cfe, 0x6cff); mem_wr(gb, HL, A);
  CYC(0x6cff, 0x6d01); B = 0x00;
  CYC(0x6d01, 0x6d04); objectCopyPositionWithOffset_hook(gb); return; // jp
}

// 0e:6d1d, bare global; called from enemyCode3c. A small bari.
void bari_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d1d, 0x6d1e); A = mem_rd(gb, DE);
  CYC(0x6d1e, 0x6d20); alu_sub(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x6d20, 0x6d23); bari_state9_hook(gb); return; } // jp nz
  CYC(0x6d20, 0x6d23);

  CALL_C(0x6d23, ecom_decCounter1_b0e_hook, 0x439a, 0x6d26);
  if (!(F & FZ)) { CYCT(0x6d26, 0x6d29); bari_applySpeed_hook(gb); return; } // jp nz
  CYC(0x6d26, 0x6d29);
  CALL_C(0x6d29, getRandomNumber_hook, 0x043e, 0x6d2c);
  CYC(0x6d2c, 0x6d2e); alu_and(gb, 0x1c);
  CYC(0x6d2e, 0x6d2f); A = alu_inc8(gb, A);
  CYC(0x6d2f, 0x6d30); mem_wr(gb, HL, A); // [counter1]
  CALL_C(0x6d30, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6d33);
  CALL_C(0x6d33, objectNudgeAngleTowards_hook, 0x1fd4, 0x6d36);
  CYC(0x6d36, 0x6d39); bari_applySpeed_hook(gb); return; // jp
}

// 0e:6d39, bare global; called from enemyCode3c. Bobs up and down.
void bari_updateZPosition_hook(GB *gb) {
  CYC(0x6d39, 0x6d3a); H = D;
  CYC(0x6d3a, 0x6d3c); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6d3c, 0x6d3d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6d3d, 0x6d3e); A = mem_rd(gb, HL);
  CYC(0x6d3e, 0x6d40); alu_and(gb, 0x30);
  CYC(0x6d40, 0x6d42); A = alu_swap(gb, A);
  CYC(0x6d42, 0x6d45); SET_HL(0x6d4b); // @zVals
  CYC(0x6d45, 0x6d46); bari_addAToHl_from_rst(gb, 0x6d46);
  CYC(0x6d46, 0x6d48); E = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x6d48, 0x6d49); A = mem_rd(gb, HL);
  CYC(0x6d49, 0x6d4a); mem_wr(gb, DE, A);
  RET(0x6d4a); return; // ret
}
