#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void crow_state_uninitialized_hook(GB *gb);
void crow_state_stub_hook(GB *gb);
void crow_subid0_hook(GB *gb);
void crow_subid0_state8_hook(GB *gb);
void crow_subid0_state9_hook(GB *gb);
void crow_subid0_stateA_hook(GB *gb);
void crow_subid0_animate_hook(GB *gb);
void crow_subid1_hook(GB *gb);
void crow_subid1_state8_hook(GB *gb);
void crow_subid1_state9_hook(GB *gb);
void crow_subid1_stateA_hook(GB *gb);
void crow_subid1_animate_hook(GB *gb);
void crow_subid1_stateB_hook(GB *gb);
void crow_subid1_stateC_hook(GB *gb);
void crow_subid1_stateD_hook(GB *gb);
void crow_updateAngleTowardLinkIfCounter1Zero_hook(GB *gb);
void crow_setAnimationFromAngle_hook(GB *gb);
void crow_subid0_checkWithinScreenBounds_hook(GB *gb);
void crow_moveTowardTargetPosition_hook(GB *gb);
void crow_updateSpeed_hook(GB *gb);
void crow_subid1_checkWithinScreenBounds_hook(GB *gb);

static uint16_t crow_jump_table(GB *gb) {
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

static void crow_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_CROW
// ENEMY_BLUE_CROW
//
// Variables:
//   var30: "Base" animation index (direction gets added to this)
//   var31: Actual animation index
//   var32/var33: Target position (subid 1 only)
// ==================================================================================================
void enemyCode41_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5b34, 0x5b36); goto normalStatus; } // jr z
  CYC(0x5b34, 0x5b36);
  CYC(0x5b36, 0x5b38); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5b38); return; } // ret c
  CYC(0x5b38, 0x5b39);
  if (F & FZ) { CYCT(0x5b39, 0x5b3c); enemyDie_hook(gb); return; } // jp z
  CYC(0x5b39, 0x5b3c);
  CYC(0x5b3c, 0x5b3d); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x5b3d); return; } // ret z
  CYC(0x5b3d, 0x5b3e);
  CYC(0x5b3e, 0x5b41); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5b41, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x5b44);
  if (!(F & FC)) { CYCT(0x5b44, 0x5b46); goto normalState; } // jr nc
  CYC(0x5b44, 0x5b46);
  {
    CYC(0x5b46, 0x5b47); push_effect(gb, 0x5b47);
    uint16_t target = crow_jump_table(gb);
    if (target == 0x5b5d) { crow_state_uninitialized_hook(gb); return; }
    if (target == 0x5b6c) { crow_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x5b57, 0x5b58); A = B;
  {
    CYC(0x5b58, 0x5b59); push_effect(gb, 0x5b59);
    uint16_t target = crow_jump_table(gb);
    if (target == 0x5b6d) { crow_subid0_hook(gb); return; }
    if (target == 0x5bf0) { crow_subid1_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5b5d, bare global; jump-table target from enemyCode41.
void crow_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b5d, 0x5b5f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5b5f, 0x5b60); A = mem_rd(gb, DE);
  CYC(0x5b60, 0x5b61); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5b61, 0x5b64); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp nz
  CYC(0x5b61, 0x5b64);

  // Subid 0
  CYC(0x5b64, 0x5b66); A = 0x32; // SPEED_140
  CALL_C(0x5b66, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x5b69);
  CYC(0x5b69, 0x5b6c); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:5b6c, bare global; jump-table target from enemyCode41.
void crow_state_stub_hook(GB *gb) {
  RET(0x5b6c); return; // ret
}

// 0e:5b6d, bare global; jump-table target from enemyCode41.
void crow_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b6d, 0x5b6e); A = mem_rd(gb, DE);
  CYC(0x5b6e, 0x5b70); alu_sub(gb, 0x08);
  {
    CYC(0x5b70, 0x5b71); push_effect(gb, 0x5b71);
    uint16_t target = crow_jump_table(gb);
    if (target == 0x5b77) { crow_subid0_state8_hook(gb); return; }
    if (target == 0x5b9e) { crow_subid0_state9_hook(gb); return; }
    if (target == 0x5bd1) { crow_subid0_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5b77, bare global; jump-table target from crow_subid0. Perched, waiting for Link to
// approach.
void crow_subid0_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b77, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5b7a);
  CALL_C(0x5b7a, crow_setAnimationFromAngle_hook, 0x5ccc, 0x5b7d);

  // Check if Link has approached
  CYC(0x5b7d, 0x5b7e); H = D;
  CYC(0x5b7e, 0x5b80); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5b80, 0x5b82); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5b82, 0x5b83); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b83, 0x5b85); alu_add(gb, 0x30);
  CYC(0x5b85, 0x5b87); alu_cp(gb, 0x61);
  if (!(F & FC)) { RET_TAKEN(0x5b87); return; } // ret nc
  CYC(0x5b87, 0x5b88);

  CYC(0x5b88, 0x5b8a); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5b8a, 0x5b8c); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5b8c, 0x5b8d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b8d, 0x5b8f); alu_add(gb, 0x18);
  CYC(0x5b8f, 0x5b91); alu_cp(gb, 0x31);
  if (!(F & FC)) { RET_TAKEN(0x5b91); return; } // ret nc
  CYC(0x5b91, 0x5b92);

  // Link has approached.
  CALL_C(0x5b92, ecom_incState_b0e_hook, 0x4000, 0x5b95);
  CYC(0x5b95, 0x5b97); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5b97, 0x5b99); mem_wr(gb, HL, 25);

  CYC(0x5b99, 0x5b9b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5b9b, 0x5b9d); mem_wr(gb, HL, 0x02);
  RET(0x5b9d); return; // ret
}

// 0e:5b9e, bare global; jump-table target from crow_subid0. Moving up and preparing to
// charge at Link after [counter1] frames (25 frames).
void crow_subid0_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b9e, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5ba1);
  CALL_C(0x5ba1, crow_setAnimationFromAngle_hook, 0x5ccc, 0x5ba4);
  CALL_C(0x5ba4, ecom_decCounter1_b0e_hook, 0x439a, 0x5ba7);
  if (F & FZ) { CYCT(0x5ba7, 0x5ba9); goto beginCharge; } // jr z
  CYC(0x5ba7, 0x5ba9);
  CYC(0x5ba9, 0x5baa); A = mem_rd(gb, HL); // [counter1]
  CYC(0x5baa, 0x5bac); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x5bac, 0x5bae); crow_subid0_animate_hook(gb); return; } // jr nz
  CYC(0x5bac, 0x5bae);
  CYC(0x5bae, 0x5bb0); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5bb0, 0x5bb1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5bb1, 0x5bb3); crow_subid0_animate_hook(gb); return; // jr

beginCharge:
  CYC(0x5bb3, 0x5bb4); L = alu_inc8(gb, L);
  CYC(0x5bb4, 0x5bb6); mem_wr(gb, HL, 0x5a); // [counter2]

  CYC(0x5bb6, 0x5bb8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5bb8, 0x5bb9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(0x5bb9, 0x5bbb); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5bbb, 0x5bbd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  CALL_C(0x5bbd, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5bc0);

  // Randomly add or subtract 4 from angle (will either overshoot or undershoot Link)
  CALL_C(0x5bc0, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5bc3);
  CYC(0x5bc3, 0x5bc5); alu_and(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x5bc5, 0x5bc7); goto addAngle; } // jr nz
  CYC(0x5bc5, 0x5bc7);
  CYC(0x5bc7, 0x5bc9); A = 0xfc; // -$04

addAngle:
  CYC(0x5bc9, 0x5bca); B = A;
  CYC(0x5bca, 0x5bcc); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5bcc, 0x5bcd); A = mem_rd(gb, DE);
  CYC(0x5bcd, 0x5bce); alu_add(gb, B);
  CYC(0x5bce, 0x5bcf); mem_wr(gb, DE, A);
  CYC(0x5bcf, 0x5bd1); crow_subid0_animate_hook(gb); return; // jr
}

// 0e:5bd1, bare global; jump-table target from crow_subid0. Charging toward Link.
void crow_subid0_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5bd1, crow_subid0_checkWithinScreenBounds_hook, 0x5ce3, 0x5bd4);
  if (!(F & FC)) { CYCT(0x5bd4, 0x5bd7); enemyDelete_hook(gb); return; } // jp nc
  CYC(0x5bd4, 0x5bd7);
  CALL_C(0x5bd7, ecom_decCounter2_b0e_hook, 0x43a3, 0x5bda);
  if (F & FZ) { CYCT(0x5bda, 0x5bdc); goto applySpeed; } // jr z
  CYC(0x5bda, 0x5bdc);

  // Adjust angle toward Link every 8 frames
  CYC(0x5bdc, 0x5bdd); A = mem_rd(gb, HL);
  CYC(0x5bdd, 0x5bdf); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x5bdf, 0x5be1); goto applySpeed; } // jr nz
  CYC(0x5bdf, 0x5be1);

  CALL_C(0x5be1, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5be4);
  CALL_C(0x5be4, objectNudgeAngleTowards_hook, 0x1fd4, 0x5be7);
  CALL_C(0x5be7, crow_setAnimationFromAngle_hook, 0x5ccc, 0x5bea);

applySpeed:
  CALL_C(0x5bea, objectApplySpeed_hook, 0x201d, 0x5bed);
  crow_subid0_animate_hook(gb); return; // fallthrough
}

// 0e:5bed, bare global; falls into from crow_subid0_stateA, also reached by genuine jr
// from crow_subid0_state9.
void crow_subid0_animate_hook(GB *gb) {
  CYC(0x5bed, 0x5bf0); enemyAnimate_hook(gb); return; // jp
}

// 0e:5bf0, bare global; jump-table target from enemyCode41.
void crow_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5bf0, 0x5bf1); A = mem_rd(gb, DE);
  CYC(0x5bf1, 0x5bf3); alu_sub(gb, 0x08);
  {
    CYC(0x5bf3, 0x5bf4); push_effect(gb, 0x5bf4);
    uint16_t target = crow_jump_table(gb);
    if (target == 0x5c00) { crow_subid1_state8_hook(gb); return; }
    if (target == 0x5c2f) { crow_subid1_state9_hook(gb); return; }
    if (target == 0x5c77) { crow_subid1_stateA_hook(gb); return; }
    if (target == 0x5c8b) { crow_subid1_stateB_hook(gb); return; }
    if (target == 0x5ca5) { crow_subid1_stateC_hook(gb); return; }
    if (target == 0x5cba) { crow_subid1_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5c00, bare global; jump-table target from crow_subid1. Checking whether it's ok to
// charge in right now.
void crow_subid1_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Count the number of crows that are in state 9 or higher (number of crows that
  // are either about to or are already charging across the screen)
  CYC(0x5c00, 0x5c03); SET_HL(0xd081); // FIRST_ENEMY_INDEX, Enemy.id
  CYC(0x5c03, 0x5c05); B = 0x00;

nextEnemy:
  CYC(0x5c05, 0x5c06); A = mem_rd(gb, HL);
  CYC(0x5c06, 0x5c08); alu_cp(gb, 0x41); // ENEMY_CROW
  if (!(F & FZ)) { CYCT(0x5c08, 0x5c0a); goto notCrow; } // jr nz
  CYC(0x5c08, 0x5c0a);

  CYC(0x5c0a, 0x5c0b); L = E; // l = state
  CYC(0x5c0b, 0x5c0c); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5c0c, 0x5c0d); L = alu_dec8(gb, L);
  CYC(0x5c0d, 0x5c0e); L = alu_dec8(gb, L);
  CYC(0x5c0e, 0x5c10); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(0x5c10, 0x5c12); goto notCrow; } // jr c
  CYC(0x5c10, 0x5c12);
  CYC(0x5c12, 0x5c13); B = alu_inc8(gb, B);

notCrow:
  CYC(0x5c13, 0x5c14); H = alu_inc8(gb, H);
  CYC(0x5c14, 0x5c15); A = H;
  CYC(0x5c15, 0x5c17); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x5c17, 0x5c19); goto nextEnemy; } // jr c
  CYC(0x5c17, 0x5c19);

  // Only allow 2 such crows at a time (this one needs to wait)
  CYC(0x5c19, 0x5c1a); A = B;
  CYC(0x5c1a, 0x5c1c); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(0x5c1c); return; } // ret nc
  CYC(0x5c1c, 0x5c1d);

  CYC(0x5c1d, 0x5c1e); H = D;
  CYC(0x5c1e, 0x5c1f); L = E;
  CYC(0x5c1f, 0x5c20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x5c20, 0x5c22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5c22, 0x5c23); alu_or(gb, A);
  CYC(0x5c23, 0x5c25); A = 60; // 1st crow on-screen
  if (F & FZ) { CYCT(0x5c25, 0x5c27); goto setCounter1; } // jr z
  CYC(0x5c25, 0x5c27);
  CYC(0x5c27, 0x5c29); A = 240; // 2nd crow on-screen

setCounter1:
  CYC(0x5c29, 0x5c2a); mem_wr(gb, HL, A);
  CYC(0x5c2a, 0x5c2c); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5c2c, 0x5c2e); mem_wr(gb, HL, 0x02);
  RET(0x5c2e); return; // ret
}

// 0e:5c2f, bare global; jump-table target from crow_subid1. Spawn in after [counter1]
// frames.
void crow_subid1_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c2f, ecom_decCounter1_b0e_hook, 0x439a, 0x5c32);
  if (!(F & FZ)) { RET_TAKEN(0x5c32); return; } // ret nz
  CYC(0x5c32, 0x5c33);

  // Determine spawn/target position data to read based on which screen quadrant Link
  // is in
  CYC(0x5c33, 0x5c35); B = 0x00;
  CYC(0x5c35, 0x5c37); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5c37, 0x5c39); alu_cp(gb, 0x40); // (SMALL_ROOM_HEIGHT/2)<<4
  if (F & FC) { CYCT(0x5c39, 0x5c3b); goto checkX; } // jr c
  CYC(0x5c39, 0x5c3b);
  CYC(0x5c3b, 0x5c3d); B = 0x08;

checkX:
  CYC(0x5c3d, 0x5c3f); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5c3f, 0x5c41); alu_cp(gb, 0x50); // (SMALL_ROOM_WIDTH/2)<<4
  if (F & FC) { CYCT(0x5c41, 0x5c43); goto lookup; } // jr c
  CYC(0x5c41, 0x5c43);
  CYC(0x5c43, 0x5c45); B = (uint8_t)(B | (1 << 2)); // set 2,b

lookup:
  CYC(0x5c45, 0x5c46); A = B;
  CYC(0x5c46, 0x5c49); SET_HL(0x5d34); // @crow_offScreenSpawnData
  crow_addAToHl_from_rst(gb, 0x5c4a);

  // Read in spawn position
  CYC(0x5c4a, 0x5c4c); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5c4c, 0x5c4d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5c4d, 0x5c4e); mem_wr(gb, DE, A);
  CYC(0x5c4e, 0x5c50); hram_wr(gb, 0x8f, A); // hFF8F

  CYC(0x5c50, 0x5c52); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5c52, 0x5c53); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5c53, 0x5c54); mem_wr(gb, DE, A);
  CYC(0x5c54, 0x5c56); hram_wr(gb, 0x8e, A); // hFF8E

  // Read in target position
  CYC(0x5c56, 0x5c58); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5c58, 0x5c59); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5c59, 0x5c5a); mem_wr(gb, DE, A);
  CYC(0x5c5a, 0x5c5b); B = A;

  CYC(0x5c5b, 0x5c5c); E = alu_inc8(gb, E);
  CYC(0x5c5c, 0x5c5d); A = mem_rd(gb, HL);
  CYC(0x5c5d, 0x5c5e); mem_wr(gb, DE, A);
  CYC(0x5c5e, 0x5c5f); C = A;

  // Set angle to target position
  CALL_C(0x5c5f, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5c62);
  CALL_C(0x5c62, ecom_incState_b0e_hook, 0x4000, 0x5c65);

  CYC(0x5c65, 0x5c67); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5c67, 0x5c69); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  CYC(0x5c69, 0x5c6b); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5c6b, 0x5c6d); mem_wr(gb, HL, 0x14); // SPEED_80

  CYC(0x5c6d, 0x5c6f); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5c6f, 0x5c71); mem_wr(gb, HL, 0xfa); // -$06

  CALL_C(0x5c71, crow_setAnimationFromAngle_hook, 0x5ccc, 0x5c74);
  CYC(0x5c74, 0x5c77); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:5c77, bare global; jump-table target from crow_subid1. Moving into screen.
void crow_subid1_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c77, crow_moveTowardTargetPosition_hook, 0x5cef, 0x5c7a);
  if (!(F & FC)) { CYCT(0x5c7a, 0x5c7c); crow_subid1_animate_hook(gb); return; } // jr nc
  CYC(0x5c7a, 0x5c7c);

  CYC(0x5c7c, 0x5c7d); L = E;
  CYC(0x5c7d, 0x5c7e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x5c7e, 0x5c80); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5c80, 0x5c82); mem_wr(gb, HL, 0x3c);

  CALL_C(0x5c82, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5c85);
  CALL_C(0x5c85, crow_setAnimationFromAngle_hook, 0x5ccc, 0x5c88);
  crow_subid1_animate_hook(gb); return; // fallthrough
}

// 0e:5c88, bare global; falls into from crow_subid1_stateA, also reached by genuine jr/jp
// from crow_subid1_stateB and crow_subid1_stateC.
void crow_subid1_animate_hook(GB *gb) {
  CYC(0x5c88, 0x5c8b); enemyAnimate_hook(gb); return; // jp
}

// 0e:5c8b, bare global; jump-table target from crow_subid1. Hovering in position for
// [counter1] frames before charging.
void crow_subid1_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c8b, ecom_decCounter1_b0e_hook, 0x439a, 0x5c8e);
  if (!(F & FZ)) { CYCT(0x5c8e, 0x5c90); crow_subid1_animate_hook(gb); return; } // jr nz
  CYC(0x5c8e, 0x5c90);

  CYC(0x5c90, 0x5c92); mem_wr(gb, HL, 24); // [counter1]
  CYC(0x5c92, 0x5c93); L = alu_inc8(gb, L);
  CYC(0x5c93, 0x5c95); mem_wr(gb, HL, 0x00); // [counter2]

  CYC(0x5c95, 0x5c96); L = E;
  CYC(0x5c96, 0x5c97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x5c97, 0x5c99); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5c99, 0x5c9b); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5c9b, 0x5c9c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5c9c, 0x5c9e); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5c9e, 0x5c9f); mem_wr(gb, HL, A);

  CYC(0x5c9f, 0x5ca1); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5ca1, 0x5ca3); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x5ca3, 0x5ca5); crow_subid1_animate_hook(gb); return; // jr
}

// 0e:5ca5, bare global; jump-table target from crow_subid1. Moving, accelerating toward
// Link.
void crow_subid1_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5ca5, crow_subid1_checkWithinScreenBounds_hook, 0x5d20, 0x5ca8);
  if (!(F & FC)) { CYCT(0x5ca8, 0x5caa); goto outOfBounds; } // jr nc
  CYC(0x5ca8, 0x5caa);

  CALL_C(0x5caa, crow_updateAngleTowardLinkIfCounter1Zero_hook, 0x5cc5, 0x5cad);
  CALL_C(0x5cad, crow_updateSpeed_hook, 0x5d0a, 0x5cb0);
  CALL_C(0x5cb0, objectApplySpeed_hook, 0x201d, 0x5cb3);
  CYC(0x5cb3, 0x5cb5); crow_subid1_animate_hook(gb); return; // jr

outOfBounds:
  CALL_C(0x5cb5, ecom_incState_b0e_hook, 0x4000, 0x5cb8);
  CYC(0x5cb8, 0x5cba); crow_subid1_animate_hook(gb); return; // jr
}

// 0e:5cba, bare global; jump-table target from crow_subid1. Moved out of bounds; go back
// to state 8 to eventually charge again.
void crow_subid1_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5cba, 0x5cbb); H = D;
  CYC(0x5cbb, 0x5cbc); L = E;
  CYC(0x5cbc, 0x5cbe); mem_wr(gb, HL, 0x08); // [state]

  CYC(0x5cbe, 0x5cc0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5cc0, 0x5cc2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

  CYC(0x5cc2, 0x5cc5); objectSetInvisible_hook(gb); return; // jp
}

// 0e:5cc5, bare global; called from crow_subid1_stateC. Adjusts angle to move directly
// toward Link when [counter1] reaches 0. After this it underflows to 255, so the angle
// correction only happens once.
void crow_updateAngleTowardLinkIfCounter1Zero_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5cc5, ecom_decCounter1_b0e_hook, 0x439a, 0x5cc8);
  if (!(F & FZ)) { RET_TAKEN(0x5cc8); return; } // ret nz
  CYC(0x5cc8, 0x5cc9);
  CALL_C(0x5cc9, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5ccc);
  crow_setAnimationFromAngle_hook(gb); return; // fallthrough
}

// 0e:5ccc, bare global; falls into from crow_updateAngleTowardLinkIfCounter1Zero, also
// reached by genuine call from crow_moveTowardTargetPosition and various crow_subid0/1
// states.
void crow_setAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ccc, 0x5ccd); H = D;
  CYC(0x5ccd, 0x5ccf); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5ccf, 0x5cd0); A = mem_rd(gb, HL);
  CYC(0x5cd0, 0x5cd2); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x5cd2); return; } // ret z
  CYC(0x5cd2, 0x5cd3);

  CYC(0x5cd3, 0x5cd5); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(0x5cd5, 0x5cd7); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5cd7, 0x5cd8); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(0x5cd8, 0x5cda); goto compareAnim; } // jr nz
  CYC(0x5cd8, 0x5cda);
  CYC(0x5cda, 0x5cdb); A = alu_inc8(gb, A);

compareAnim:
  CYC(0x5cdb, 0x5cdd); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5cdd, 0x5cde); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5cde); return; } // ret z
  CYC(0x5cde, 0x5cdf);
  CYC(0x5cdf, 0x5ce0); mem_wr(gb, HL, A);
  CYC(0x5ce0, 0x5ce3); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5ce3, bare global; called from crow_subid0_stateA. Identical to
// crow_subid1_checkWithinScreenBounds.
// @param[out] cflag c if within screen bounds
void crow_subid0_checkWithinScreenBounds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ce3, 0x5ce5); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5ce5, 0x5ce6); A = mem_rd(gb, DE);
  CYC(0x5ce6, 0x5ce8); alu_cp(gb, 0x88); // (SMALL_ROOM_HEIGHT<<4) + 8
  if (!(F & FC)) { RET_TAKEN(0x5ce8); return; } // ret nc
  CYC(0x5ce8, 0x5ce9);
  CYC(0x5ce9, 0x5ceb); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5ceb, 0x5cec); A = mem_rd(gb, DE);
  CYC(0x5cec, 0x5cee); alu_cp(gb, 0xa8); // (SMALL_ROOM_WIDTH<<4) + 8
  RET(0x5cee); return; // ret
}

// 0e:5cef, bare global; called from crow_subid1_stateA.
// @param[out] cflag c if within 1 pixel of target position
void crow_moveTowardTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cef, 0x5cf0); H = D;
  CYC(0x5cf0, 0x5cf2); L = ENEMY_BASE + 0x32; // Enemy.var32
  CALL_C(0x5cf2, ecom_readPositionVars_b0e_hook, 0x4439, 0x5cf5);
  CYC(0x5cf5, 0x5cf6); alu_sub(gb, C);
  CYC(0x5cf6, 0x5cf7); A = alu_inc8(gb, A);
  CYC(0x5cf7, 0x5cf9); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x5cf9, 0x5cfb); goto moveToward; } // jr nc
  CYC(0x5cf9, 0x5cfb);

  CYC(0x5cfb, 0x5cfd); A = hram_rd(gb, 0x8f); // hFF8F
  CYC(0x5cfd, 0x5cfe); alu_sub(gb, B);
  CYC(0x5cfe, 0x5cff); A = alu_inc8(gb, A);
  CYC(0x5cff, 0x5d01); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(0x5d01); return; } // ret c
  CYC(0x5d01, 0x5d02);

moveToward:
  CALL_C(0x5d02, ecom_moveTowardPosition_b0e_hook, 0x4430, 0x5d05);
  CALL_C(0x5d05, crow_setAnimationFromAngle_hook, 0x5ccc, 0x5d08);
  CYC(0x5d08, 0x5d09); alu_or(gb, D);
  RET(0x5d09); return; // ret
}

// 0e:5d0a, bare global; called from crow_subid1_stateC. Updates speed based on counter2.
// For subid 1.
void crow_updateSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d0a, 0x5d0c); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5d0c, 0x5d0d); A = mem_rd(gb, DE);
  CYC(0x5d0d, 0x5d0f); alu_cp(gb, 0x7f);
  if (F & FZ) { CYCT(0x5d0f, 0x5d11); goto setSpeed; } // jr z
  CYC(0x5d0f, 0x5d11);
  CYC(0x5d11, 0x5d12); A = alu_inc8(gb, A);
  CYC(0x5d12, 0x5d13); mem_wr(gb, DE, A);

setSpeed:
  CYC(0x5d13, 0x5d15); alu_and(gb, 0xf0);
  CYC(0x5d15, 0x5d17); A = alu_swap(gb, A);
  CYC(0x5d17, 0x5d1a); SET_HL(0x5d2c); // @crow_speeds
  crow_addAToHl_from_rst(gb, 0x5d1b);
  CYC(0x5d1b, 0x5d1d); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5d1d, 0x5d1e); A = mem_rd(gb, HL);
  CYC(0x5d1e, 0x5d1f); mem_wr(gb, DE, A);
  RET(0x5d1f); return; // ret
}

// 0e:5d20, bare global; called from crow_subid1_stateC. Identical to
// crow_subid0_checkWithinScreenBounds.
// @param[out] cflag c if within screen bounds
void crow_subid1_checkWithinScreenBounds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d20, 0x5d22); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5d22, 0x5d23); A = mem_rd(gb, DE);
  CYC(0x5d23, 0x5d25); alu_cp(gb, 0x88); // (SCREEN_HEIGHT<<4) + 8
  if (!(F & FC)) { RET_TAKEN(0x5d25); return; } // ret nc
  CYC(0x5d25, 0x5d26);
  CYC(0x5d26, 0x5d28); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5d28, 0x5d29); A = mem_rd(gb, DE);
  CYC(0x5d29, 0x5d2b); alu_cp(gb, 0xa8); // (SCREEN_WIDTH<<4) + 8
  RET(0x5d2b); return; // ret
}
