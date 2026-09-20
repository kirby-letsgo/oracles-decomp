#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void rope_subid00_hook(GB *gb);
void rope_subid01_hook(GB *gb);
void rope_subid02_hook(GB *gb);
void rope_subid03_hook(GB *gb);
void rope_state_moveAround_hook(GB *gb);
void rope_callEnemyAnimate_hook(GB *gb);
void rope_state_chargeLink_hook(GB *gb);
void rope_changeDirection_hook(GB *gb);
void rope_updateAnimationFromAngle_hook(GB *gb);
void rope_animate_hook(GB *gb);
void rope_checkHazardsIfApplicable_hook(GB *gb);

static uint16_t rope_jump_table(GB *gb) {
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

static void rope_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_ROPE
// ==================================================================================================
void enemyCode10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4e0b, rope_checkHazardsIfApplicable_hook, 0x4ff9, 0x4e0e);
  CYC(0x4e0e, 0x4e0f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4e0f, 0x4e11); goto normalStatus; } // jr z
  CYC(0x4e0f, 0x4e11);
  CYC(0x4e11, 0x4e13); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(0x4e13); return; } // ret c
  CYC(0x4e13, 0x4e14);
  if (F & FZ) { CYCT(0x4e14, 0x4e17); enemyDie_hook(gb); return; } // jp z
  CYC(0x4e14, 0x4e17);
  CYC(0x4e17, 0x4e18); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4e18, 0x4e1b); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x4e18, 0x4e1b);
  RET(0x4e1b); return; // ret

normalStatus:
  CALL_C(0x4e1c, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x4e1f);
  if (F & FZ) { CYCT(0x4e1f, 0x4e21); goto afterSpeedBump; } // jr z
  CYC(0x4e1f, 0x4e21);
  CYC(0x4e21, 0x4e23); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4e23, 0x4e25); A = 0x32; // SPEED_140
  CYC(0x4e25, 0x4e26); mem_wr(gb, DE, A);

afterSpeedBump:
  CALL_C(0x4e26, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x4e29);
  if (!(F & FC)) { CYCT(0x4e29, 0x4e2b); goto normalState; } // jr nc
  CYC(0x4e29, 0x4e2b);
  {
    CYC(0x4e2b, 0x4e2c); push_effect(gb, 0x4e2c);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4e46) goto state_uninitialized;
    if (target == 0x4e9f) goto state_stub;
    if (target == 0x4e59) goto state_switchHook;
    if (target == 0x4e70) goto state_scentSeed;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x4e3c, 0x4e3d); A = B;
  {
    CYC(0x4e3d, 0x4e3e); push_effect(gb, 0x4e3e);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4ea0) { rope_subid00_hook(gb); return; }
    if (target == 0x4ef2) { rope_subid01_hook(gb); return; }
    if (target == 0x4f49) { rope_subid02_hook(gb); return; }
    if (target == 0x4f7c) { rope_subid03_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x4e46, 0x4e48); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x4e48, 0x4e4a); A = 0xff;
  CYC(0x4e4a, 0x4e4b); mem_wr(gb, DE, A);
  CYC(0x4e4b, 0x4e4c); B = alu_dec8(gb, B);
  CYC(0x4e4c, 0x4e4e); A = 0x0f; // SPEED_60
  if (F & FZ) { CYCT(0x4e4e, 0x4e51); ecom_setSpeedAndState8_b0d_hook(gb); return; } // jp z
  CYC(0x4e4e, 0x4e51);
  CYC(0x4e51, 0x4e52); H = D;
  CYC(0x4e52, 0x4e54); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x4e54, 0x4e56); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x4e56, 0x4e59); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_switchHook:
  CYC(0x4e59, 0x4e5a); E = alu_inc8(gb, E);
  CYC(0x4e5a, 0x4e5b); A = mem_rd(gb, DE);
  {
    CYC(0x4e5b, 0x4e5c); push_effect(gb, 0x4e5c);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x4e64) goto substate1;
    if (target == 0x4e65) goto substate3;
    HANDOFF(target);
  }

substate1:
  RET(0x4e64); return; // ret

substate3:
  CYC(0x4e65, 0x4e67); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4e67, 0x4e68); A = mem_rd(gb, DE);
  CYC(0x4e68, 0x4e6b); SET_HL(0x4e9b); // @defaultStates (data)
  CYC(0x4e6b, 0x4e6c); rope_addAToHl_from_rst(gb, 0x4e6c);
  CYC(0x4e6c, 0x4e6d); B = mem_rd(gb, HL);
  CYC(0x4e6d, 0x4e70); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp

state_scentSeed:
  CYC(0x4e70, 0x4e73); A = mem_rd(gb, wScentSeedActive);
  CYC(0x4e73, 0x4e74); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4e74, 0x4e76); goto seek; } // jr nz
  CYC(0x4e74, 0x4e76);
  CYC(0x4e76, 0x4e78); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4e78, 0x4e79); A = mem_rd(gb, DE);
  CYC(0x4e79, 0x4e7c); SET_HL(0x4e9b); // @defaultStates (data)
  CYC(0x4e7c, 0x4e7d); rope_addAToHl_from_rst(gb, 0x4e7d);
  CYC(0x4e7d, 0x4e7f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4e7f, 0x4e80); A = mem_rd(gb, HL);
  CYC(0x4e80, 0x4e81); mem_wr(gb, DE, A);
  CYC(0x4e81, 0x4e83); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4e83, 0x4e85); A = 0x0f; // SPEED_60
  CYC(0x4e85, 0x4e86); mem_wr(gb, DE, A);
  RET(0x4e86); return; // ret

seek:
  CALL_C(0x4e87, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x4e8a);
  CYC(0x4e8a, 0x4e8c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4e8c, 0x4e8d); A = mem_rd(gb, DE);
  CYC(0x4e8d, 0x4e8f); alu_add(gb, 0x04);
  CYC(0x4e8f, 0x4e91); alu_and(gb, 0x18);
  CYC(0x4e91, 0x4e92); mem_wr(gb, DE, A);
  CALL_C(0x4e92, rope_updateAnimationFromAngle_hook, 0x4fd7, 0x4e95);
  CALL_C(0x4e95, ecom_applyVelocityForSideviewEnemy_b0d_hook, 0x4153, 0x4e98);
  CYC(0x4e98, 0x4e9f); rope_animate_hook(gb); return; // jp

state_stub:
  RET(0x4e9f); return; // ret
}

// 0d:4ea0, bare global; normal rope.
void rope_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ea0, 0x4ea1); A = mem_rd(gb, DE);
  CYC(0x4ea1, 0x4ea3); alu_sub(gb, 0x08);
  {
    CYC(0x4ea3, 0x4ea4); push_effect(gb, 0x4ea4);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4eaa) goto state8;
    if (target == 0x4eb5) { rope_state_moveAround_hook(gb); return; }
    if (target == 0x4edd) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x4eaa, 0x4eab); H = D;
  CYC(0x4eab, 0x4eac); L = E;
  CYC(0x4eac, 0x4ead); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x4ead, 0x4eaf); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4eaf, 0x4eb1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4eb1, 0x4eb3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4eb3, 0x4eb5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  rope_state_moveAround_hook(gb); return; // fallthrough
}

// 0d:4eb5, bare global; moving around, checking whether to charge Link.
void rope_state_moveAround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4eb5, 0x4eb7); B = 0x0a;
  CALL_C(0x4eb7, objectCheckCenteredWithLink_hook, 0x1fee, 0x4eba);
  if (!(F & FC)) { CYCT(0x4eba, 0x4ebc); goto notCentered; } // jr nc
  CYC(0x4eba, 0x4ebc);
  CYC(0x4ebc, 0x4ebe); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x4ebe, 0x4ebf); A = mem_rd(gb, DE);
  CYC(0x4ebf, 0x4ec0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4ec0, 0x4ec2); goto notCentered; } // jr nz
  CYC(0x4ec0, 0x4ec2);
  CALL_C(0x4ec2, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x4ec5);
  CALL_C(0x4ec5, ecom_incState_b0d_hook, 0x4000, 0x4ec8);
  CYC(0x4ec8, 0x4eca); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4eca, 0x4ecc); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(0x4ecc, 0x4ecf); rope_updateAnimationFromAngle_hook(gb); return; // jp

notCentered:
  CALL_C(0x4ecf, ecom_decCounter2_b0d_hook, 0x43a3, 0x4ed2);
  CYC(0x4ed2, 0x4ed3); L = alu_dec8(gb, L);
  CYC(0x4ed3, 0x4ed4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter1]--
  if (!(F & FZ)) { CALL_C(0x4ed4, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x4ed7); } else { CYC(0x4ed4, 0x4ed7); } // call nz
  if (F & FZ) { CYCT(0x4ed7, 0x4eda); rope_changeDirection_hook(gb); return; } // jp z
  CYC(0x4ed7, 0x4eda);
  rope_callEnemyAnimate_hook(gb); return; // fallthrough
}

// 0d:4eda, bare global.
void rope_callEnemyAnimate_hook(GB *gb) {
  CYC(0x4eda, 0x4edd); enemyAnimate_hook(gb); return; // jp
}

// 0d:4edd, bare global; charging Link.
void rope_state_chargeLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4edd, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x4ee0);
  if (!(F & FZ)) { CYCT(0x4ee0, 0x4ee3); rope_animate_hook(gb); return; } // jp nz
  CYC(0x4ee0, 0x4ee3);
  CYC(0x4ee3, 0x4ee4); H = D;
  CYC(0x4ee4, 0x4ee6); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ee6, 0x4ee7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4ee7, 0x4ee9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4ee9, 0x4eeb); mem_wr(gb, HL, 0x0f); // SPEED_60
  CYC(0x4eeb, 0x4eed); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x4eed, 0x4eef); mem_wr(gb, HL, 0x40);
  CYC(0x4eef, 0x4ef2); rope_changeDirection_hook(gb); return; // jp
}

// 0d:4ef2, bare global; rope that falls from the sky.
void rope_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ef2, 0x4ef3); A = mem_rd(gb, DE);
  CYC(0x4ef3, 0x4ef5); alu_sub(gb, 0x08);
  {
    CYC(0x4ef5, 0x4ef6); push_effect(gb, 0x4ef6);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4f00) goto state8;
    if (target == 0x4f0d) goto state9;
    if (target == 0x4f2b) goto stateA;
    if (target == 0x4eb5) { rope_state_moveAround_hook(gb); return; }
    if (target == 0x4edd) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x4f00, 0x4f02); A = 0x09;
  CYC(0x4f02, 0x4f03); mem_wr(gb, DE, A);
  CALL_C(0x4f03, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4f06);
  CYC(0x4f06, 0x4f08); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4f08, 0x4f0a); alu_and(gb, 0x38);
  CYC(0x4f0a, 0x4f0b); A = alu_inc8(gb, A);
  CYC(0x4f0b, 0x4f0c); mem_wr(gb, DE, A);
  RET(0x4f0c); return; // ret

state9:
  CALL_C(0x4f0d, ecom_decCounter1_b0d_hook, 0x439a, 0x4f10);
  if (!(F & FZ)) { RET_TAKEN(0x4f10); return; } // ret nz
  CYC(0x4f10, 0x4f11);
  CYC(0x4f11, 0x4f12); L = E;
  CYC(0x4f12, 0x4f13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x4f13, 0x4f15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4f15, 0x4f17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4f17, 0x4f19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4f19, 0x4f1b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4f1b, 0x4f1d); L = 0x15; // Enemy.speedZ+1
  CYC(0x4f1d, 0x4f1e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f1e, 0x4f20); A = 0x59; // SND_FALLINHOLE
  CALL_C(0x4f20, playSound_b00_hook, 0x0c98, 0x4f23);
  CALL_C(0x4f23, objectSetVisiblec1_hook, 0x1e3c, 0x4f26);
  CYC(0x4f26, 0x4f28); C = 0x08;
  CYC(0x4f28, 0x4f2b); ecom_setZAboveScreen_b0d_hook(gb); return; // jp

stateA:
  CYC(0x4f2b, 0x4f2d); C = 0x0e;
  CALL_C(0x4f2d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4f30);
  if (!(F & FZ)) { RET_TAKEN(0x4f30); return; } // ret nz
  CYC(0x4f30, 0x4f31);
  CYC(0x4f31, 0x4f33); L = 0x14; // Enemy.speedZ
  CYC(0x4f33, 0x4f34); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4f34, 0x4f35); mem_wr(gb, HL, A);
  CYC(0x4f35, 0x4f37); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4f37, 0x4f38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f38, 0x4f3a); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x4f3a, 0x4f3c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CALL_C(0x4f3c, objectSetVisiblec2_hook, 0x1e45, 0x4f3f);
  CYC(0x4f3f, 0x4f41); A = 0x52; // SND_BOMB_LAND
  CALL_C(0x4f41, playSound_b00_hook, 0x0c98, 0x4f44);
  CALL_C(0x4f44, rope_changeDirection_hook, 0x4fc7, 0x4f47);
  CYC(0x4f47, 0x4f49); rope_callEnemyAnimate_hook(gb); return; // jr
}

// 0d:4f49, bare global; immediately charges Link upon spawning.
void rope_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f49, 0x4f4a); A = mem_rd(gb, DE);
  CYC(0x4f4a, 0x4f4c); alu_sub(gb, 0x08);
  {
    CYC(0x4f4c, 0x4f4d); push_effect(gb, 0x4f4d);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4f55) goto state8;
    if (target == 0x4f66) goto state9;
    if (target == 0x4eb5) { rope_state_moveAround_hook(gb); return; }
    if (target == 0x4edd) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x4f55, 0x4f56); H = D;
  CYC(0x4f56, 0x4f57); L = E;
  CYC(0x4f57, 0x4f58); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x4f58, 0x4f5a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4f5a, 0x4f5c); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(0x4f5c, 0x4f5e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4f5e, 0x4f60); mem_wr(gb, HL, 0x08);
  CALL_C(0x4f60, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x4f63);
  CYC(0x4f63, 0x4f66); rope_updateAnimationFromAngle_hook(gb); return; // jp

state9:
  CALL_C(0x4f66, ecom_decCounter1_b0d_hook, 0x439a, 0x4f69);
  if (!(F & FZ)) { CYCT(0x4f69, 0x4f6b); goto applyVelocity; } // jr nz
  CYC(0x4f69, 0x4f6b);
  CYC(0x4f6b, 0x4f6c); L = E;
  CYC(0x4f6c, 0x4f6e); mem_wr(gb, HL, 0x0b); // [state] = "charge at Link" state
  CYC(0x4f6e, 0x4f70); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4f70, 0x4f72); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4f72, 0x4f74); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4f74, 0x4f76); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity:
  CALL_C(0x4f76, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x4f79);
  CYC(0x4f79, 0x4f7c); enemyAnimate_hook(gb); return; // jp
}

// 0d:4f7c, bare global; falls and bounces toward Link when it spawns.
void rope_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f7c, 0x4f7d); A = mem_rd(gb, DE);
  CYC(0x4f7d, 0x4f7f); alu_sub(gb, 0x08);
  {
    CYC(0x4f7f, 0x4f80); push_effect(gb, 0x4f80);
    uint16_t target = rope_jump_table(gb);
    if (target == 0x4f88) goto state8;
    if (target == 0x4fa2) goto state9;
    if (target == 0x4eb5) { rope_state_moveAround_hook(gb); return; }
    if (target == 0x4edd) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x4f88, 0x4f89); H = D;
  CYC(0x4f89, 0x4f8a); L = E;
  CYC(0x4f8a, 0x4f8b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x4f8b, 0x4f8d); L = 0x14; // Enemy.speedZ
  CYC(0x4f8d, 0x4f8f); A = 0xfe;
  CYC(0x4f8f, 0x4f90); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4f90, 0x4f92); mem_wr(gb, HL, 0xfe);
  CYC(0x4f92, 0x4f94); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4f94, 0x4f96); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x4f96, 0x4f98); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4f98, 0x4f9b); A = mem_rd(gb, w1Link + OBJ_DIRECTION); // ld a,(w1Link.direction)
  CYC(0x4f9b, 0x4f9d); A = alu_swap(gb, A);
  CYC(0x4f9d, 0x4f9e); alu_rrca(gb);
  CYC(0x4f9e, 0x4f9f); mem_wr(gb, HL, A);
  CYC(0x4f9f, 0x4fa2); rope_updateAnimationFromAngle_hook(gb); return; // jp

state9:
  CYC(0x4fa2, 0x4fa4); C = 0x0e;
  CALL_C(0x4fa4, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4fa7);
  if (F & FC) { CYCT(0x4fa7, 0x4fa9); goto doneBouncing; } // jr c
  CYC(0x4fa7, 0x4fa9);
  CYC(0x4fa9, 0x4fab); A = 0x52; // SND_BOMB_LAND
  if (F & FZ) { CALL_C(0x4fab, playSound_b00_hook, 0x0c98, 0x4fae); } else { CYC(0x4fab, 0x4fae); } // call z
  CYC(0x4fae, 0x4fb0); E = 0x15; // Enemy.speedZ+1
  CYC(0x4fb0, 0x4fb1); A = mem_rd(gb, DE);
  CYC(0x4fb1, 0x4fb2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4fb2, 0x4fb4); goto applyVelocity2; } // jr nz
  CYC(0x4fb2, 0x4fb4);
  CYC(0x4fb4, 0x4fb5); H = D;
  CYC(0x4fb5, 0x4fb7); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4fb7, 0x4fb9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4fb9, 0x4fbb); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4fbb, 0x4fbd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity2:
  CYC(0x4fbd, 0x4fc0); ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook(gb); return; // jp

doneBouncing:
  CALL_C(0x4fc0, ecom_incState_b0d_hook, 0x4000, 0x4fc3);
  CYC(0x4fc3, 0x4fc5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4fc5, 0x4fc7); mem_wr(gb, HL, 0x0f); // SPEED_60
  rope_changeDirection_hook(gb); return; // fallthrough
}

// 0d:4fc7, bare global; chooses random new angle, random value for counter1.
void rope_changeDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fc7, 0x4fca); SET_BC(0x1870);
  CALL_C(0x4fca, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x4fcd);
  CYC(0x4fcd, 0x4fcf); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4fcf, 0x4fd0); A = B;
  CYC(0x4fd0, 0x4fd1); mem_wr(gb, DE, A);
  CYC(0x4fd1, 0x4fd3); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4fd3, 0x4fd4); A = C;
  CYC(0x4fd4, 0x4fd6); alu_add(gb, 0x70);
  CYC(0x4fd6, 0x4fd7); mem_wr(gb, DE, A);
  rope_updateAnimationFromAngle_hook(gb); return; // fallthrough
}

// 0d:4fd7, bare global.
void rope_updateAnimationFromAngle_hook(GB *gb) {
  CYC(0x4fd7, 0x4fd8); H = D;
  CYC(0x4fd8, 0x4fda); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4fda, 0x4fdb); A = mem_rd(gb, HL);
  CYC(0x4fdb, 0x4fdd); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x4fdd); return; } // ret z
  CYC(0x4fdd, 0x4fde);
  CYC(0x4fde, 0x4fdf); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4fdf, 0x4fe1); alu_and(gb, 0x10);
  CYC(0x4fe1, 0x4fe3); A = alu_swap(gb, A);
  CYC(0x4fe3, 0x4fe5); alu_xor(gb, 0x01);
  CYC(0x4fe5, 0x4fe6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x4fe6); return; } // ret z
  CYC(0x4fe6, 0x4fe7);
  CYC(0x4fe7, 0x4fe8); mem_wr(gb, HL, A);
  CYC(0x4fe8, 0x4feb); enemySetAnimation_hook(gb); return; // jp
}

// 0d:4feb, bare global.
void rope_animate_hook(GB *gb) {
  CYC(0x4feb, 0x4fec); H = D;
  CYC(0x4fec, 0x4fee); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(0x4fee, 0x4fef); A = mem_rd(gb, HL);
  CYC(0x4fef, 0x4ff1); alu_sub(gb, 0x03);
  if (!(F & FC)) { CYCT(0x4ff1, 0x4ff3); goto incAndStore; } // jr nc
  CYC(0x4ff1, 0x4ff3);
  CYC(0x4ff3, 0x4ff4); alu_xor(gb, A);

incAndStore:
  CYC(0x4ff4, 0x4ff5); A = alu_inc8(gb, A);
  CYC(0x4ff5, 0x4ff6); mem_wr(gb, HL, A);
  CYC(0x4ff6, 0x4ff9); enemyAnimate_hook(gb); return; // jp
}

// 0d:4ff9, bare global.
void rope_checkHazardsIfApplicable_hook(GB *gb) {
  CYC(0x4ff9, 0x4ffa); H = D;
  CYC(0x4ffa, 0x4ffc); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4ffc, 0x4ffe); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x4ffe); return; } // ret z
  CYC(0x4ffe, 0x4fff);
  CYC(0x4fff, 0x5002); ecom_checkHazards_b0d_hook(gb); return; // jp
}
