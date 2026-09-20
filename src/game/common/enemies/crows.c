#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode41), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode41), (from), (to), true)

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
  BASE(enemyCode41);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+9); return; } // ret z
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+13, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto normalState; } // jr nc
  CYC(b_+16, b_+18);
  {
    CYC(b_+18, b_+19); push_effect(gb, b_+19);
    uint16_t target = crow_jump_table(gb);
    if (target == SYM(crow_state_uninitialized)) { crow_state_uninitialized_hook(gb); return; }
    if (target == SYM(crow_state_stub)) { crow_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+35, b_+36); A = B;
  {
    CYC(b_+36, b_+37); push_effect(gb, b_+37);
    uint16_t target = crow_jump_table(gb);
    if (target == SYM(crow_subid0)) { crow_subid0_hook(gb); return; }
    if (target == SYM(crow_subid1)) { crow_subid1_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5b5d, bare global; jump-table target from enemyCode41.
void crow_state_uninitialized_hook(GB *gb) {
  BASE(crow_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp nz
  CYC(b_+4, b_+7);

  // Subid 0
  CYC(b_+7, b_+9); A = 0x32; // SPEED_140
  CALL_C(b_+9, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+12);
  CYC(b_+12, SYM(crow_state_stub)); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:5b6c, bare global; jump-table target from enemyCode41.
void crow_state_stub_hook(GB *gb) {
  BASE(crow_state_stub);
  RET(b_+0); return; // ret
}

// 0e:5b6d, bare global; jump-table target from enemyCode41.
void crow_subid0_hook(GB *gb) {
  BASE(crow_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = crow_jump_table(gb);
    if (target == SYM(crow_subid0_state8)) { crow_subid0_state8_hook(gb); return; }
    if (target == SYM(crow_subid0_state9)) { crow_subid0_state9_hook(gb); return; }
    if (target == SYM(crow_subid0_stateA)) { crow_subid0_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5b77, bare global; jump-table target from crow_subid0. Perched, waiting for Link to
// approach.
void crow_subid0_state8_hook(GB *gb) {
  BASE(crow_subid0_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+3);
  CALL_C(b_+3, crow_setAnimationFromAngle_hook, SYM(crow_setAnimationFromAngle), b_+6);

  // Check if Link has approached
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+11); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+11, b_+12); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); alu_add(gb, 0x30);
  CYC(b_+14, b_+16); alu_cp(gb, 0x61);
  if (!(F & FC)) { RET_TAKEN(b_+16); return; } // ret nc
  CYC(b_+16, b_+17);

  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+19, b_+21); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+21, b_+22); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+24); alu_add(gb, 0x18);
  CYC(b_+24, b_+26); alu_cp(gb, 0x31);
  if (!(F & FC)) { RET_TAKEN(b_+26); return; } // ret nc
  CYC(b_+26, b_+27);

  // Link has approached.
  CALL_C(b_+27, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 25);

  CYC(b_+34, b_+36); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x02);
  RET(b_+38); return; // ret
}

// 0e:5b9e, bare global; jump-table target from crow_subid0. Moving up and preparing to
// charge at Link after [counter1] frames (25 frames).
void crow_subid0_state9_hook(GB *gb) {
  BASE(crow_subid0_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+3);
  CALL_C(b_+3, crow_setAnimationFromAngle_hook, SYM(crow_setAnimationFromAngle), b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+11); goto beginCharge; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); crow_subid0_animate_hook(gb); return; } // jr nz
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); crow_subid0_animate_hook(gb); return; // jr

beginCharge:
  CYC(b_+21, b_+22); L = alu_inc8(gb, L);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x5a); // [counter2]

  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  CALL_C(b_+31, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+34);

  // Randomly add or subtract 4 from angle (will either overshoot or undershoot Link)
  CALL_C(b_+34, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+37);
  CYC(b_+37, b_+39); alu_and(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto addAngle; } // jr nz
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); A = 0xfc; // -$04

addAngle:
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_add(gb, B);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, SYM(crow_subid0_stateA)); crow_subid0_animate_hook(gb); return; // jr
}

// 0e:5bd1, bare global; jump-table target from crow_subid0. Charging toward Link.
void crow_subid0_stateA_hook(GB *gb) {
  BASE(crow_subid0_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, crow_subid0_checkWithinScreenBounds_hook, SYM(crow_subid0_checkWithinScreenBounds), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+6); enemyDelete_hook(gb); return; } // jp nc
  CYC(b_+3, b_+6);
  CALL_C(b_+6, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+11); goto applySpeed; } // jr z
  CYC(b_+9, b_+11);

  // Adjust angle toward Link every 8 frames
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto applySpeed; } // jr nz
  CYC(b_+14, b_+16);

  CALL_C(b_+16, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+19);
  CALL_C(b_+19, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+22);
  CALL_C(b_+22, crow_setAnimationFromAngle_hook, SYM(crow_setAnimationFromAngle), b_+25);

applySpeed:
  CALL_C(b_+25, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(crow_subid0_animate));
  crow_subid0_animate_hook(gb); return; // fallthrough
}

// 0e:5bed, bare global; falls into from crow_subid0_stateA, also reached by genuine jr
// from crow_subid0_state9.
void crow_subid0_animate_hook(GB *gb) {
  BASE(crow_subid0_animate);
  CYC(b_+0, SYM(crow_subid1)); enemyAnimate_hook(gb); return; // jp
}

// 0e:5bf0, bare global; jump-table target from enemyCode41.
void crow_subid1_hook(GB *gb) {
  BASE(crow_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = crow_jump_table(gb);
    if (target == SYM(crow_subid1_state8)) { crow_subid1_state8_hook(gb); return; }
    if (target == SYM(crow_subid1_state9)) { crow_subid1_state9_hook(gb); return; }
    if (target == SYM(crow_subid1_stateA)) { crow_subid1_stateA_hook(gb); return; }
    if (target == SYM(crow_subid1_stateB)) { crow_subid1_stateB_hook(gb); return; }
    if (target == SYM(crow_subid1_stateC)) { crow_subid1_stateC_hook(gb); return; }
    if (target == SYM(crow_subid1_stateD)) { crow_subid1_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5c00, bare global; jump-table target from crow_subid1. Checking whether it's ok to
// charge in right now.
void crow_subid1_state8_hook(GB *gb) {
  BASE(crow_subid1_state8);
  uint16_t sp0_ = gb->sp;
  // Count the number of crows that are in state 9 or higher (number of crows that
  // are either about to or are already charging across the screen)
  CYC(b_+0, b_+3); SET_HL((w1ReservedInteraction0_var3f + 2)); // FIRST_ENEMY_INDEX, Enemy.id
  CYC(b_+3, b_+5); B = 0x00;

nextEnemy:
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x41); // ENEMY_CROW
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto notCrow; } // jr nz
  CYC(b_+8, b_+10);

  CYC(b_+10, b_+11); L = E; // l = state
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+12, b_+13); L = alu_dec8(gb, L);
  CYC(b_+13, b_+14); L = alu_dec8(gb, L);
  CYC(b_+14, b_+16); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(b_+16, b_+18); goto notCrow; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); B = alu_inc8(gb, B);

notCrow:
  CYC(b_+19, b_+20); H = alu_inc8(gb, H);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+23); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+23, b_+25); goto nextEnemy; } // jr c
  CYC(b_+23, b_+25);

  // Only allow 2 such crows at a time (this one needs to wait)
  CYC(b_+25, b_+26); A = B;
  CYC(b_+26, b_+28); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(b_+28); return; } // ret nc
  CYC(b_+28, b_+29);

  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+35); alu_or(gb, A);
  CYC(b_+35, b_+37); A = 60; // 1st crow on-screen
  if (F & FZ) { CYCT(b_+37, b_+39); goto setCounter1; } // jr z
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 240; // 2nd crow on-screen

setCounter1:
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);
  CYC(b_+42, b_+44); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x02);
  RET(b_+46); return; // ret
}

// 0e:5c2f, bare global; jump-table target from crow_subid1. Spawn in after [counter1]
// frames.
void crow_subid1_state9_hook(GB *gb) {
  BASE(crow_subid1_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);

  // Determine spawn/target position data to read based on which screen quadrant Link
  // is in
  CYC(b_+4, b_+6); B = 0x00;
  CYC(b_+6, b_+8); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+8, b_+10); alu_cp(gb, 0x40); // (SMALL_ROOM_HEIGHT/2)<<4
  if (F & FC) { CYCT(b_+10, b_+12); goto checkX; } // jr c
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); B = 0x08;

checkX:
  CYC(b_+14, b_+16); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+16, b_+18); alu_cp(gb, 0x50); // (SMALL_ROOM_WIDTH/2)<<4
  if (F & FC) { CYCT(b_+18, b_+20); goto lookup; } // jr c
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); B = (uint8_t)(B | (1 << 2)); // set 2,b

lookup:
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+26); SET_HL(SYM(crow_offScreenSpawnData)); // @crow_offScreenSpawnData
  crow_addAToHl_from_rst(gb, b_+27);

  // Read in spawn position
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); hram_wr(gb, 0x8f, A); // hFF8F

  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+39); hram_wr(gb, 0x8e, A); // hFF8E

  // Read in target position
  CYC(b_+39, b_+41); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+44); B = A;

  CYC(b_+44, b_+45); E = alu_inc8(gb, E);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+48); C = A;

  // Set angle to target position
  CALL_C(b_+48, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+51);
  CALL_C(b_+51, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+54);

  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+56, b_+58); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x14); // SPEED_80

  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0xfa); // -$06

  CALL_C(b_+66, crow_setAnimationFromAngle_hook, SYM(crow_setAnimationFromAngle), b_+69);
  CYC(b_+69, SYM(crow_subid1_stateA)); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:5c77, bare global; jump-table target from crow_subid1. Moving into screen.
void crow_subid1_stateA_hook(GB *gb) {
  BASE(crow_subid1_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, crow_moveTowardTargetPosition_hook, SYM(crow_moveTowardTargetPosition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); crow_subid1_animate_hook(gb); return; } // jr nc
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x3c);

  CALL_C(b_+11, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+14);
  CALL_C(b_+14, crow_setAnimationFromAngle_hook, SYM(crow_setAnimationFromAngle), SYM(crow_subid1_animate));
  crow_subid1_animate_hook(gb); return; // fallthrough
}

// 0e:5c88, bare global; falls into from crow_subid1_stateA, also reached by genuine jr/jp
// from crow_subid1_stateB and crow_subid1_stateC.
void crow_subid1_animate_hook(GB *gb) {
  BASE(crow_subid1_animate);
  CYC(b_+0, SYM(crow_subid1_stateB)); enemyAnimate_hook(gb); return; // jp
}

// 0e:5c8b, bare global; jump-table target from crow_subid1. Hovering in position for
// [counter1] frames before charging.
void crow_subid1_stateB_hook(GB *gb) {
  BASE(crow_subid1_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); crow_subid1_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+7); mem_wr(gb, HL, 24); // [counter1]
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00); // [counter2]

  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+14, b_+16); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+17, b_+19); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);

  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+24, SYM(crow_subid1_stateC)); crow_subid1_animate_hook(gb); return; // jr
}

// 0e:5ca5, bare global; jump-table target from crow_subid1. Moving, accelerating toward
// Link.
void crow_subid1_stateC_hook(GB *gb) {
  BASE(crow_subid1_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, crow_subid1_checkWithinScreenBounds_hook, SYM(crow_subid1_checkWithinScreenBounds), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); goto outOfBounds; } // jr nc
  CYC(b_+3, b_+5);

  CALL_C(b_+5, crow_updateAngleTowardLinkIfCounter1Zero_hook, SYM(crow_updateAngleTowardLinkIfCounter1Zero), b_+8);
  CALL_C(b_+8, crow_updateSpeed_hook, SYM(crow_updateSpeed), b_+11);
  CALL_C(b_+11, objectApplySpeed_hook, SYM(objectApplySpeed), b_+14);
  CYC(b_+14, b_+16); crow_subid1_animate_hook(gb); return; // jr

outOfBounds:
  CALL_C(b_+16, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+19);
  CYC(b_+19, SYM(crow_subid1_stateD)); crow_subid1_animate_hook(gb); return; // jr
}

// 0e:5cba, bare global; jump-table target from crow_subid1. Moved out of bounds; go back
// to state 8 to eventually charge again.
void crow_subid1_stateD_hook(GB *gb) {
  BASE(crow_subid1_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x08); // [state]

  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

  CYC(b_+8, SYM(crow_updateAngleTowardLinkIfCounter1Zero)); objectSetInvisible_hook(gb); return; // jp
}

// 0e:5cc5, bare global; called from crow_subid1_stateC. Adjusts angle to move directly
// toward Link when [counter1] reaches 0. After this it underflows to 255, so the angle
// correction only happens once.
void crow_updateAngleTowardLinkIfCounter1Zero_hook(GB *gb) {
  BASE(crow_updateAngleTowardLinkIfCounter1Zero);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), SYM(crow_setAnimationFromAngle));
  crow_setAnimationFromAngle_hook(gb); return; // fallthrough
}

// 0e:5ccc, bare global; falls into from crow_updateAngleTowardLinkIfCounter1Zero, also
// reached by genuine call from crow_moveTowardTargetPosition and various crow_subid0/1
// states.
void crow_setAnimationFromAngle_hook(GB *gb) {
  BASE(crow_setAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+6); return; } // ret z
  CYC(b_+6, b_+7);

  CYC(b_+7, b_+9); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+9, b_+11); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto compareAnim; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);

compareAnim:
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+17, b_+18); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+18); return; } // ret z
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, SYM(crow_subid0_checkWithinScreenBounds)); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5ce3, bare global; called from crow_subid0_stateA. Identical to
// crow_subid1_checkWithinScreenBounds.
// @param[out] cflag c if within screen bounds
void crow_subid0_checkWithinScreenBounds_hook(GB *gb) {
  BASE(crow_subid0_checkWithinScreenBounds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x88); // (SMALL_ROOM_HEIGHT<<4) + 8
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0xa8); // (SMALL_ROOM_WIDTH<<4) + 8
  RET(b_+11); return; // ret
}

// 0e:5cef, bare global; called from crow_subid1_stateA.
// @param[out] cflag c if within 1 pixel of target position
void crow_moveTowardTargetPosition_hook(GB *gb) {
  BASE(crow_moveTowardTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x32; // Enemy.var32
  CALL_C(b_+3, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  CYC(b_+8, b_+10); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto moveToward; } // jr nc
  CYC(b_+10, b_+12);

  CYC(b_+12, b_+14); A = hram_rd(gb, 0x8f); // hFF8F
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+18); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+18); return; } // ret c
  CYC(b_+18, b_+19);

moveToward:
  CALL_C(b_+19, ecom_moveTowardPosition_b0e_hook, SYM(ecom_moveTowardPosition_b0e), b_+22);
  CALL_C(b_+22, crow_setAnimationFromAngle_hook, SYM(crow_setAnimationFromAngle), b_+25);
  CYC(b_+25, b_+26); alu_or(gb, D);
  RET(b_+26); return; // ret
}

// 0e:5d0a, bare global; called from crow_subid1_stateC. Updates speed based on counter2.
// For subid 1.
void crow_updateSpeed_hook(GB *gb) {
  BASE(crow_updateSpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x7f);
  if (F & FZ) { CYCT(b_+5, b_+7); goto setSpeed; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);

setSpeed:
  CYC(b_+9, b_+11); alu_and(gb, 0xf0);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+16); SET_HL(SYM(crow_speeds)); // @crow_speeds
  crow_addAToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret
}

// 0e:5d20, bare global; called from crow_subid1_stateC. Identical to
// crow_subid0_checkWithinScreenBounds.
// @param[out] cflag c if within screen bounds
void crow_subid1_checkWithinScreenBounds_hook(GB *gb) {
  BASE(crow_subid1_checkWithinScreenBounds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x88); // (SCREEN_HEIGHT<<4) + 8
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0xa8); // (SCREEN_WIDTH<<4) + 8
  RET(b_+11); return; // ret
}
