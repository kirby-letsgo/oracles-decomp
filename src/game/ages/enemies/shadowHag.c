#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void enemyCode7a_hook(GB *gb);
void shadowHag_state_uninitialized_hook(GB *gb);
void shadowHag_state_stub_hook(GB *gb);
void shadowHag_state8_hook(GB *gb);
void shadowHag_state9_hook(GB *gb);
void shadowHag_stateA_hook(GB *gb);
void shadowHag_stateB_hook(GB *gb);
void shadowHag_initStateC_hook(GB *gb);
void shadowHag_stateC_hook(GB *gb);
void shadowHag_stateD_hook(GB *gb);
void shadowHag_stateE_hook(GB *gb);
void shadowHag_stateF_hook(GB *gb);
void shadowHag_state10_hook(GB *gb);
void shadowHag_state11_hook(GB *gb);
void shadowHag_animate_hook(GB *gb);
void shadowHag_state12_hook(GB *gb);
void shadowHag_doneCharging_hook(GB *gb);
void shadowHag_state13_hook(GB *gb);
void shadowHag_beginEmergingFromShadow_hook(GB *gb);
void shadowHag_updateEmergingFromShadow_hook(GB *gb);
void shadowHag_updateReturningToGround_hook(GB *gb);
void shadowHag_beginReturningToGround_hook(GB *gb);
void shadowHag_chooseSpawnPosition_hook(GB *gb);
void shadowHag_checkLinkLookedAtHag_hook(GB *gb);

static uint16_t shadowHag_jump_table(GB *gb) {
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

static void shadowHag_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void shadowHag_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SHADOW_HAG
//
// Variables:
//   counter2: Number of times to spawn bugs before shadows separate
//   var30: Number of bugs on-screen
//   var31: Set if the hag couldn't spawn because Link was in a bad position
// ==================================================================================================

void enemyCode7a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6b6b, 0x6b6d); goto normalStatus; } // jr z
  CYC(0x6b6b, 0x6b6d);
  CYC(0x6b6d, 0x6b6f); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6b6f); return; } // ret c
  CYC(0x6b6f, 0x6b70);
  if (!(F & FZ)) { CYCT(0x6b70, 0x6b72); goto normalStatus; } // jr nz
  CYC(0x6b70, 0x6b72);
  CYC(0x6b72, 0x6b74); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6b74, 0x6b75); A = mem_rd(gb, DE);
  CYC(0x6b75, 0x6b76); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6b76, 0x6b78); goto dead; } // jr z
  CYC(0x6b76, 0x6b78);
  CYC(0x6b78, 0x6b7b); SET_HL(ENEMY_SLOTS); // FIRST_ENEMY_INDEX, Enemy.start

killNext:
  CYC(0x6b7b, 0x6b7d); L = ENEMY_BASE + OBJ_ID;
  CYC(0x6b7d, 0x6b7e); A = mem_rd(gb, HL);
  CYC(0x6b7e, 0x6b80); alu_cp(gb, 0x42); // ENEMY_SHADOW_HAG_BUG
  if (F & FZ) CALL_C_CC(0x6b80, ecom_killObjectH_b0f_hook, 0x445e, 0x6b83); else CYC(0x6b80, 0x6b83); // call z
  CYC(0x6b83, 0x6b84); H = alu_inc8(gb, H);
  CYC(0x6b84, 0x6b85); A = H;
  CYC(0x6b85, 0x6b87); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x6b87, 0x6b89); goto killNext; } // jr c
  CYC(0x6b87, 0x6b89);

dead:
  CYC(0x6b89, 0x6b8c); enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(0x6b8c, 0x6b8e); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6b8e, 0x6b8f); A = mem_rd(gb, DE);
  CYC(0x6b8f, 0x6b90); push_effect(gb, 0x6b90);
  {
    uint16_t target = shadowHag_jump_table(gb);
    if (target == 0x6bb8) { shadowHag_state_uninitialized_hook(gb); return; }
    if (target == 0x6bc4) { shadowHag_state_stub_hook(gb); return; }
    if (target == 0x6bc5) { shadowHag_state8_hook(gb); return; }
    if (target == 0x6c3d) { shadowHag_state9_hook(gb); return; }
    if (target == 0x6c6f) { shadowHag_stateA_hook(gb); return; }
    if (target == 0x6c95) { shadowHag_stateB_hook(gb); return; }
    if (target == 0x6cc0) { shadowHag_stateC_hook(gb); return; }
    if (target == 0x6ccc) { shadowHag_stateD_hook(gb); return; }
    if (target == 0x6cfa) { shadowHag_stateE_hook(gb); return; }
    if (target == 0x6d17) { shadowHag_stateF_hook(gb); return; }
    if (target == 0x6d40) { shadowHag_state10_hook(gb); return; }
    if (target == 0x6d64) { shadowHag_state11_hook(gb); return; }
    if (target == 0x6d75) { shadowHag_state12_hook(gb); return; }
    if (target == 0x6dac) { shadowHag_state13_hook(gb); return; }
    HANDOFF(target);
  }
}

void shadowHag_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bb8, 0x6bba); A = 0x7a; // ENEMY_SHADOW_HAG
  CYC(0x6bba, 0x6bbc); B = 0x00;
  CALL_C(0x6bbc, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x6bbf);
  CYC(0x6bbf, 0x6bc1); A = 0x14; // SPEED_80
  CYC(0x6bc1, 0x6bc4); ecom_setSpeedAndState8_b0f_hook(gb); return; // jp
}

void shadowHag_state_stub_hook(GB *gb) {
  RET(0x6bc4); return; // ret
}

void shadowHag_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bc5, 0x6bc6); E = alu_inc8(gb, E);
  CYC(0x6bc6, 0x6bc7); A = mem_rd(gb, DE);
  CYC(0x6bc7, 0x6bc8); push_effect(gb, 0x6bc8);
  {
    uint16_t target = shadowHag_jump_table(gb);
    if (target == 0x6bd2) goto substate0;
    if (target == 0x6bfa) goto substate1;
    if (target == 0x6c13) goto substate2;
    if (target == 0x6c20) goto substate3;
    if (target == 0x6c2f) goto substate4;
    HANDOFF(target);
  }

substate0:
  CYC(0x6bd2, 0x6bd5); A = mem_rd(gb, wcc93);
  CYC(0x6bd5, 0x6bd6); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6bd6); return; } // ret nz
  CYC(0x6bd6, 0x6bd7);
  CYC(0x6bd7, 0x6bd8); A = alu_inc8(gb, A);
  CYC(0x6bd8, 0x6bdb); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6bdb, 0x6bde); SET_BC(0x0104);
  CALL_C(0x6bde, enemyBoss_spawnShadow_b0f_hook, 0x4534, 0x6be1);
  if (!(F & FZ)) { RET_TAKEN(0x6be1); return; } // ret nz
  CYC(0x6be1, 0x6be2);
  CYC(0x6be2, 0x6be3); H = D;
  CYC(0x6be3, 0x6be5); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6be5, 0x6be6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6be6, 0x6be8); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6be8, 0x6bea); mem_wr(gb, HL, 0x18);
  CYC(0x6bea, 0x6bec); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6bec, 0x6bee); mem_wr(gb, HL, 0xff);
  CYC(0x6bee, 0x6bf0); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6bf0, 0x6bf2); A = H8(hEnemyTargetY);
  CYC(0x6bf2, 0x6bf4); alu_add(gb, 0x04);
  CYC(0x6bf4, 0x6bf5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6bf5, 0x6bf6); L = alu_inc8(gb, L);
  CYC(0x6bf6, 0x6bf8); A = H8(hEnemyTargetX);
  CYC(0x6bf8, 0x6bf9); mem_wr(gb, HL, A);
  RET(0x6bfa); return; // ret

substate1:
  CYC(0x6bfa, 0x6bfc); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6bfc, 0x6bfd); A = mem_rd(gb, DE);
  CYC(0x6bfd, 0x6bff); alu_cp(gb, 0x78); // (LARGE_ROOM_WIDTH/2)<<4 + 8
  if (!(F & FC)) { CYCT(0x6bff, 0x6c02); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x6bff, 0x6c02);
  CALL_C(0x6c02, shadowHag_beginEmergingFromShadow_hook, 0x6db4, 0x6c05);
  CYC(0x6c05, 0x6c06); H = D;
  CYC(0x6c06, 0x6c08); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6c08, 0x6c09); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6c09, 0x6c0a); L = alu_inc8(gb, L);
  CYC(0x6c0a, 0x6c0c); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(0x6c0c, 0x6c0e); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6c0e, 0x6c10); mem_wr(gb, HL, 0x00);
  CYC(0x6c10, 0x6c13); ecom_killRelatedObj2_b0f_hook(gb); return; // jp

substate2:
  CALL_C(0x6c13, shadowHag_updateEmergingFromShadow_hook, 0x6dc3, 0x6c16);
  if (!(F & FZ)) { RET_TAKEN(0x6c16); return; } // ret nz
  CYC(0x6c16, 0x6c17);
  CYC(0x6c17, 0x6c19); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6c19, 0x6c1b); A = 0x03;
  CYC(0x6c1b, 0x6c1c); mem_wr(gb, DE, A);
  CYC(0x6c1c, 0x6c1d); A = alu_dec8(gb, A);
  CYC(0x6c1d, 0x6c20); enemySetAnimation_hook(gb); return; // jp

substate3:
  CALL_C(0x6c20, ecom_decCounter1_b0f_hook, 0x439a, 0x6c23);
  if (!(F & FZ)) { CYCT(0x6c23, 0x6c25); goto animate; } // jr nz
  CYC(0x6c23, 0x6c25);
  CYC(0x6c25, 0x6c27); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(0x6c27, 0x6c28); L = E;
  CYC(0x6c28, 0x6c29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6c29, 0x6c2c); SET_BC(0x2f2b); // TX_2f2b
  CYC(0x6c2c, 0x6c2f); showText_hook(gb); return; // jp

substate4:
  CALL_C(0x6c2f, ecom_decCounter1_b0f_hook, 0x439a, 0x6c32);
  if (!(F & FZ)) { CYCT(0x6c32, 0x6c34); goto animate; } // jr nz
  CYC(0x6c32, 0x6c34);
  CALL_C(0x6c34, shadowHag_beginReturningToGround_hook, 0x6df3, 0x6c37);
  CALL_C(0x6c37, enemyBoss_beginBoss_b0f_hook, 0x4584, 0x6c3a);

animate:
  CYC(0x6c3a, 0x6c3d); enemyAnimate_hook(gb); return; // jp
}

// Currently in the ground, showing eyes
void shadowHag_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c3d, ecom_decCounter2_b0f_hook, 0x43a3, 0x6c40);
  if (!(F & FZ)) { CYCT(0x6c40, 0x6c43); shadowHag_updateReturningToGround_hook(gb); return; } // jp nz
  CYC(0x6c40, 0x6c43);
  CYC(0x6c43, 0x6c44); L = alu_dec8(gb, L);
  CYC(0x6c44, 0x6c45); A = mem_rd(gb, HL);
  CYC(0x6c45, 0x6c46); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6c46, 0x6c48); goto spawnShadows; } // jr z
  CYC(0x6c46, 0x6c48);
  CYC(0x6c48, 0x6c49); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x6c49, 0x6c4c); ecom_flickerVisibility_b0f_hook(gb); return; // jp

spawnShadows:
  CYC(0x6c4c, 0x6c4e); B = 0x04;
  CALL_C(0x6c4e, checkBPartSlotsAvailable_hook, 0x20f2, 0x6c51);
  if (!(F & FZ)) { RET_TAKEN(0x6c51); return; } // ret nz
  CYC(0x6c51, 0x6c52);
  CYC(0x6c52, 0x6c55); SET_BC(0x4104); // PART_SHADOW_HAG_SHADOW,4

L_6c55:
  CALL_C(0x6c55, ecom_spawnProjectile_b0f_hook, 0x437c, 0x6c58);
  CYC(0x6c58, 0x6c59); C = alu_dec8(gb, C);
  CYC(0x6c59, 0x6c5b); L = PART_BASE + OBJ_ANGLE;
  CYC(0x6c5b, 0x6c5c); mem_wr(gb, HL, C);
  if (!(F & FZ)) { CYCT(0x6c5c, 0x6c5e); goto L_6c55; } // jr nz
  CYC(0x6c5c, 0x6c5e);
  CALL_C(0x6c5e, ecom_incState_b0f_hook, 0x4000, 0x6c61);
  CYC(0x6c61, 0x6c63); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6c63, 0x6c65); mem_wr(gb, HL, 0x96); // 150
  CYC(0x6c65, 0x6c66); L = alu_inc8(gb, L);
  CYC(0x6c66, 0x6c68); mem_wr(gb, HL, 0x04); // [counter2]
  CYC(0x6c68, 0x6c6a); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6c6a, 0x6c6c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6c6c, 0x6c6f); objectSetInvisible_hook(gb); return; // jp
}

// Shadows chasing Link
void shadowHag_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c6f, 0x6c72); A = mem_rd(gb, wFrameCounter);
  CYC(0x6c72, 0x6c73); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x6c73); return; } // ret c
  CYC(0x6c73, 0x6c74);
  CALL_C(0x6c74, ecom_decCounter1_b0f_hook, 0x439a, 0x6c77);
  if (!(F & FZ)) { RET_TAKEN(0x6c77); return; } // ret nz
  CYC(0x6c77, 0x6c78);
  CYC(0x6c78, 0x6c79); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter1]=$ff
  CYC(0x6c79, 0x6c7a); L = E;
  CYC(0x6c7a, 0x6c7b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]=$0b
  CALL_C(0x6c7b, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c7e);
  CYC(0x6c7e, 0x6c80); alu_and(gb, 0x06);
  CYC(0x6c80, 0x6c83); SET_HL(0x6c8d); // @targetPositions
  CYC(0x6c83, 0x6c84); shadowHag_addAToHl_from_rst(gb, 0x6c84);
  CYC(0x6c84, 0x6c86); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6c86, 0x6c87); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6c87, 0x6c88); mem_wr(gb, DE, A);
  CYC(0x6c88, 0x6c8a); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6c8a, 0x6c8b); A = mem_rd(gb, HL);
  CYC(0x6c8b, 0x6c8c); mem_wr(gb, DE, A);
  RET(0x6c8c); return; // ret
}

// Shadows reconverging to target position
void shadowHag_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c95, 0x6c97); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6c97, 0x6c98); A = mem_rd(gb, DE);
  CYC(0x6c98, 0x6c99); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6c99); return; } // ret nz
  CYC(0x6c99, 0x6c9a);
  CALL_C(0x6c9a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c9d);
  CYC(0x6c9d, 0x6c9f); alu_and(gb, 0x01);
  CYC(0x6c9f, 0x6ca1); alu_add(gb, 0x02);
  CYC(0x6ca1, 0x6ca3); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6ca3, 0x6ca4); mem_wr(gb, DE, A);
  shadowHag_initStateC_hook(gb); return; // fallthrough
}

void shadowHag_initStateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ca4, 0x6ca5); H = D;
  CYC(0x6ca5, 0x6ca7); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6ca7, 0x6ca9); mem_wr(gb, HL, 0x0c);
  CYC(0x6ca9, 0x6cab); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6cab, 0x6cad); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x6cad, 0x6caf); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6caf, 0x6cb1); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  CYC(0x6cb1, 0x6cb3); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6cb3, 0x6cb5); mem_wr(gb, HL, 0x03);
  CYC(0x6cb5, 0x6cb6); L = alu_inc8(gb, L);
  CYC(0x6cb6, 0x6cb8); mem_wr(gb, HL, 0x05);
  CALL_C(0x6cb8, objectSetVisible83_hook, 0x1e72, 0x6cbb);
  CYC(0x6cbb, 0x6cbd); A = 0x04;
  CYC(0x6cbd, 0x6cc0); enemySetAnimation_hook(gb); return; // jp
}

// Delay before spawning bugs
void shadowHag_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6cc0, ecom_decCounter1_b0f_hook, 0x439a, 0x6cc3);
  if (!(F & FZ)) { CYCT(0x6cc3, 0x6cc5); goto L_6cc9; } // jr nz
  CYC(0x6cc3, 0x6cc5);
  CYC(0x6cc5, 0x6cc7); mem_wr(gb, HL, 0x41); // [state]
  CYC(0x6cc7, 0x6cc8); L = E;
  CYC(0x6cc8, 0x6cc9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

L_6cc9:
  CYC(0x6cc9, 0x6ccc); enemyAnimate_hook(gb); return; // jp
}

// Spawning bugs
void shadowHag_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6ccc, enemyAnimate_hook, 0x2818, 0x6ccf);
  CALL_C(0x6ccf, ecom_decCounter1_b0f_hook, 0x439a, 0x6cd2);
  if (F & FZ) { CYCT(0x6cd2, 0x6cd4); goto doneSpawningBugs; } // jr z
  CYC(0x6cd2, 0x6cd4);
  CYC(0x6cd4, 0x6cd5); A = mem_rd(gb, HL);
  CYC(0x6cd5, 0x6cd7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x6cd7); return; } // ret nz
  CYC(0x6cd7, 0x6cd8);
  CYC(0x6cd8, 0x6cda); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6cda, 0x6cdb); A = mem_rd(gb, DE);
  CYC(0x6cdb, 0x6cdd); alu_cp(gb, 0x07);
  if (!(F & FC)) { RET_TAKEN(0x6cdd); return; } // ret nc
  CYC(0x6cdd, 0x6cde);
  CYC(0x6cde, 0x6ce0); B = 0x42; // ENEMY_SHADOW_HAG_BUG
  CALL_C(0x6ce0, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x6ce3);
  if (!(F & FZ)) { RET_TAKEN(0x6ce3); return; } // ret nz
  CYC(0x6ce3, 0x6ce4);
  CYC(0x6ce4, 0x6ce6); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x6ce6, 0x6ce8); A = ENEMY_BASE; // Enemy.start
  CYC(0x6ce8, 0x6ce9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6ce9, 0x6cea); mem_wr(gb, HL, D);
  CALL_C(0x6cea, objectCopyPosition_hook, 0x2242, 0x6ced);
  CYC(0x6ced, 0x6cee); H = D;
  CYC(0x6cee, 0x6cf0); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6cf0, 0x6cf1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x6cf1); return; // ret

doneSpawningBugs:
  CALL_C(0x6cf2, ecom_incState_b0f_hook, 0x4000, 0x6cf5);
  CYC(0x6cf5, 0x6cf7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6cf7, 0x6cf9); mem_wr(gb, HL, 0x1e); // 30
  RET(0x6cf9); return; // ret
}

// Done spawning bugs; delay before the hag herself spawns in
void shadowHag_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6cfa, ecom_decCounter1_b0f_hook, 0x439a, 0x6cfd);
  if (!(F & FZ)) { CYCT(0x6cfd, 0x6d00); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(0x6cfd, 0x6d00);
  CYC(0x6d00, 0x6d02); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6d02, 0x6d03); A = mem_rd(gb, DE);
  CYC(0x6d03, 0x6d04); alu_or(gb, A);
  CYC(0x6d04, 0x6d06); A = 0x5a; // 90
  if (F & FZ) { CYCT(0x6d06, 0x6d08); goto L_6d0c; } // jr z
  CYC(0x6d06, 0x6d08);
  CYC(0x6d08, 0x6d09); alu_xor(gb, A);
  CYC(0x6d09, 0x6d0a); mem_wr(gb, DE, A);
  CYC(0x6d0a, 0x6d0c); A = 0x96; // 150

L_6d0c:
  CYC(0x6d0c, 0x6d0d); mem_wr(gb, HL, A); // [counter1]
  CYC(0x6d0d, 0x6d0f); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d0f, 0x6d10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6d10, 0x6d12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6d12, 0x6d14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6d14, 0x6d17); objectSetInvisible_hook(gb); return; // jp
}

// Waiting for Link to be in a position where the hag can spawn behind him
void shadowHag_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d17, ecom_decCounter1_b0f_hook, 0x439a, 0x6d1a);
  if (F & FZ) { CYCT(0x6d1a, 0x6d1c); goto couldntSpawn; } // jr z
  CYC(0x6d1a, 0x6d1c);
  CALL_C(0x6d1c, shadowHag_chooseSpawnPosition_hook, 0x6e0f, 0x6d1f);
  if (!(F & FZ)) { RET_TAKEN(0x6d1f); return; } // ret nz
  CYC(0x6d1f, 0x6d20);
  CYC(0x6d20, 0x6d22); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6d22, 0x6d23); A = B;
  CYC(0x6d23, 0x6d24); mem_wr(gb, DE, A);
  CYC(0x6d24, 0x6d26); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6d26, 0x6d27); A = C;
  CYC(0x6d27, 0x6d28); mem_wr(gb, DE, A);
  CALL_C(0x6d28, shadowHag_beginEmergingFromShadow_hook, 0x6db4, 0x6d2b);
  CYC(0x6d2b, 0x6d2e); ecom_incState_b0f_hook(gb); return; // jp

couldntSpawn:
  CYC(0x6d2e, 0x6d30); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6d30, 0x6d32); A = 0x01;
  CYC(0x6d32, 0x6d33); mem_wr(gb, DE, A);
  CYC(0x6d33, 0x6d34); L = alu_inc8(gb, L);
  CYC(0x6d34, 0x6d35); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]--
  if (!(F & FZ)) { CYCT(0x6d35, 0x6d38); shadowHag_initStateC_hook(gb); return; } // jp nz
  CYC(0x6d35, 0x6d38);
  CALL_C(0x6d38, shadowHag_beginReturningToGround_hook, 0x6df3, 0x6d3b);
  CYC(0x6d3b, 0x6d3d); A = 0x04;
  CYC(0x6d3d, 0x6d40); enemySetAnimation_hook(gb); return; // jp
}

// Spawning out of ground to attack Link
void shadowHag_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d40, shadowHag_updateEmergingFromShadow_hook, 0x6dc3, 0x6d43);
  if (!(F & FZ)) { RET_TAKEN(0x6d43); return; } // ret nz
  CYC(0x6d43, 0x6d44);
  CALL_C(0x6d44, ecom_incState_b0f_hook, 0x4000, 0x6d47);
  CYC(0x6d47, 0x6d49); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6d49, 0x6d4b); mem_wr(gb, HL, 0x80 | 0x7a); // ENEMY_SHADOW_HAG
  CYC(0x6d4b, 0x6d4d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6d4d, 0x6d4f); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x6d4f, 0x6d51); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d51, 0x6d53); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x6d53, 0x6d55); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6d55, 0x6d57); mem_wr(gb, HL, 0xff);
  CYC(0x6d57, 0x6d59); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6d59, 0x6d5b); mem_wr(gb, HL, 0x0c);
  CYC(0x6d5b, 0x6d5c); L = alu_inc8(gb, L);
  CYC(0x6d5c, 0x6d5e); mem_wr(gb, HL, 0x08);
  CALL_C(0x6d5e, ecom_updateCardinalAngleTowardTarget_b0f_hook, 0x43b4, 0x6d61);
  CYC(0x6d61, 0x6d64); ecom_updateAnimationFromAngle_b0f_hook(gb); return; // jp
}

// Delay before charging at Link
void shadowHag_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d64, shadowHag_checkLinkLookedAtHag_hook, 0x6e3f, 0x6d67);
  if (F & FZ) { CYCT(0x6d67, 0x6d69); shadowHag_doneCharging_hook(gb); return; } // jr z
  CYC(0x6d67, 0x6d69);
  CALL_C(0x6d69, ecom_decCounter1_b0f_hook, 0x439a, 0x6d6c);
  if (!(F & FZ)) { RET_TAKEN(0x6d6c); return; } // ret nz
  CYC(0x6d6c, 0x6d6d);
  CYC(0x6d6d, 0x6d6f); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(0x6d6f, 0x6d71); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d71, 0x6d72); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  shadowHag_animate_hook(gb); return; // fallthrough
}

void shadowHag_animate_hook(GB *gb) {
  CYC(0x6d72, 0x6d75); enemyAnimate_hook(gb); return; // jp
}

// Charging at Link
void shadowHag_state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d75, shadowHag_checkLinkLookedAtHag_hook, 0x6e3f, 0x6d78);
  if (F & FZ) { CYCT(0x6d78, 0x6d7a); shadowHag_doneCharging_hook(gb); return; } // jr z
  CYC(0x6d78, 0x6d7a);
  CALL_C(0x6d7a, ecom_decCounter1_b0f_hook, 0x439a, 0x6d7d);
  if (F & FZ) { CYCT(0x6d7d, 0x6d7f); shadowHag_doneCharging_hook(gb); return; } // jr z
  CYC(0x6d7d, 0x6d7f);
  CYC(0x6d7f, 0x6d81); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6d81, 0x6d82); A = mem_rd(gb, DE);
  CYC(0x6d82, 0x6d84); alu_sub(gb, 0x12);
  CYC(0x6d84, 0x6d86); alu_cp(gb, 0x7e); // (LARGE_ROOM_HEIGHT<<4)-$32
  if (!(F & FC)) { CYCT(0x6d86, 0x6d88); shadowHag_doneCharging_hook(gb); return; } // jr nc
  CYC(0x6d86, 0x6d88);
  CYC(0x6d88, 0x6d8a); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6d8a, 0x6d8b); A = mem_rd(gb, DE);
  CYC(0x6d8b, 0x6d8d); alu_sub(gb, 0x18);
  CYC(0x6d8d, 0x6d8f); alu_cp(gb, 0xc0); // (LARGE_ROOM_WIDTH<<4)-$30
  if (!(F & FC)) { CYCT(0x6d8f, 0x6d91); shadowHag_doneCharging_hook(gb); return; } // jr nc
  CYC(0x6d8f, 0x6d91);
  CALL_C(0x6d91, objectApplySpeed_hook, 0x201d, 0x6d94);
  CYCT(0x6d94, 0x6d96); shadowHag_animate_hook(gb); return; // jr
}

void shadowHag_doneCharging_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d96, ecom_decCounter2_b0f_hook, 0x43a3, 0x6d99);
  if (F & FZ) { CYCT(0x6d99, 0x6d9c); shadowHag_beginReturningToGround_hook(gb); return; } // jp z
  CYC(0x6d99, 0x6d9c);
  CYC(0x6d9c, 0x6d9e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d9e, 0x6da0); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x6da0, 0x6da2); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6da2, 0x6da3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6da3, 0x6da5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6da5, 0x6da7); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  CYC(0x6da7, 0x6da9); A = 0x06;
  CYC(0x6da9, 0x6dac); enemySetAnimation_hook(gb); return; // jp
}

// Delay before spawning bugs again
void shadowHag_state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6dac, ecom_decCounter1_b0f_hook, 0x439a, 0x6daf);
  if (!(F & FZ)) { CYCT(0x6daf, 0x6db1); shadowHag_updateReturningToGround_hook(gb); return; } // jr nz
  CYC(0x6daf, 0x6db1);
  CYC(0x6db1, 0x6db4); shadowHag_initStateC_hook(gb); return; // jp
}

void shadowHag_beginEmergingFromShadow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6db4, 0x6db6); A = 0x05;
  CALL_C(0x6db6, enemySetAnimation_hook, 0x282b, 0x6db9);
  CALL_C(0x6db9, objectSetVisible82_hook, 0x1e69, 0x6dbc);
  CYC(0x6dbc, 0x6dbe); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6dbe, 0x6dbf); A = mem_rd(gb, DE);
  CYC(0x6dbf, 0x6dc1); alu_sub(gb, 0x04);
  CYC(0x6dc1, 0x6dc2); mem_wr(gb, DE, A);
  RET(0x6dc2); return; // ret
}

// @param[out] zflag z if done emerging? (animParameter was $ff)
void shadowHag_updateEmergingFromShadow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6dc3, enemyAnimate_hook, 0x2818, 0x6dc6);
  CYC(0x6dc6, 0x6dc8); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6dc8, 0x6dc9); A = mem_rd(gb, DE);
  CYC(0x6dc9, 0x6dca); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(0x6dca); return; } // ret z
  CYC(0x6dca, 0x6dcb);
  CYC(0x6dcb, 0x6dcd); alu_sub(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x6dcd); return; } // ret nz
  CYC(0x6dcd, 0x6dce);
  CYC(0x6dce, 0x6dcf); mem_wr(gb, DE, A);
  CYC(0x6dcf, 0x6dd1); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6dd1, 0x6dd2); A = mem_rd(gb, DE);
  CYC(0x6dd2, 0x6dd4); alu_sub(gb, 0x08);
  CYC(0x6dd4, 0x6dd5); mem_wr(gb, DE, A);
  CYC(0x6dd5, 0x6dd6); alu_or(gb, D);
  RET(0x6dd6); return; // ret
}

void shadowHag_updateReturningToGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6dd7, enemyAnimate_hook, 0x2818, 0x6dda);
  CYC(0x6dda, 0x6ddc); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6ddc, 0x6ddd); A = mem_rd(gb, DE);
  CYC(0x6ddd, 0x6dde); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6dde); return; } // ret z
  CYC(0x6dde, 0x6ddf);
  CYC(0x6ddf, 0x6de1); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(0x6de1); return; } // ret nz
  CYC(0x6de1, 0x6de2);
  CYC(0x6de2, 0x6de3); A = alu_dec8(gb, A);
  CYC(0x6de3, 0x6de6); SET_HL(0x6df1); // @yOffsets
  CYC(0x6de6, 0x6de7); shadowHag_addAToHl_from_rst(gb, 0x6de7);
  CYC(0x6de7, 0x6de9); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6de9, 0x6dea); A = mem_rd(gb, DE);
  CYC(0x6dea, 0x6deb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6deb, 0x6dec); mem_wr(gb, DE, A);
  CYC(0x6dec, 0x6dee); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6dee, 0x6def); alu_xor(gb, A);
  CYC(0x6def, 0x6df0); mem_wr(gb, DE, A);
  RET(0x6df0); return; // ret
}

// Sets state to 9 & initializes stuff
void shadowHag_beginReturningToGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6df3, 0x6df4); H = D;
  CYC(0x6df4, 0x6df6); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6df6, 0x6df8); mem_wr(gb, HL, 0x09);
  CYC(0x6df8, 0x6dfa); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6dfa, 0x6dfc); mem_wr(gb, HL, 0x5a); // 90
  CYC(0x6dfc, 0x6dfd); L = alu_inc8(gb, L);
  CYC(0x6dfd, 0x6dff); mem_wr(gb, HL, 0x1e); // [counter2] = 30
  CYC(0x6dff, 0x6e01); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6e01, 0x6e03); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  CYC(0x6e03, 0x6e05); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6e05, 0x6e07); mem_wr(gb, HL, 0x03);
  CYC(0x6e07, 0x6e08); L = alu_inc8(gb, L);
  CYC(0x6e08, 0x6e0a); mem_wr(gb, HL, 0x05);
  CYC(0x6e0a, 0x6e0c); A = 0x06;
  CYC(0x6e0c, 0x6e0f); enemySetAnimation_hook(gb); return; // jp
}

// Chooses position to spawn at for charge attack based on Link's facing direction.
// @param[out] bc Spawn position
// @param[out] zflag nz if Link is too close to the wall to spawn in
void shadowHag_chooseSpawnPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e0f, 0x6e12); A = mem_rd(gb, w1Link_direction);
  CYC(0x6e12, 0x6e15); SET_HL(0x6e37); // @spawnOffsets
  CYC(0x6e15, 0x6e16); shadowHag_addDoubleIndexToHl_from_rst(gb, 0x6e16);
  CYC(0x6e16, 0x6e19); A = mem_rd(gb, w1Link_yh);
  CYC(0x6e19, 0x6e1a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6e1a, 0x6e1b); B = A;
  CYC(0x6e1b, 0x6e1d); alu_sub(gb, 0x1c);
  CYC(0x6e1d, 0x6e1f); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(0x6e1f, 0x6e21); goto invalid; } // jr nc
  CYC(0x6e1f, 0x6e21);
  CYC(0x6e21, 0x6e22); SET_HL(HL + 1); // inc hl
  CYC(0x6e22, 0x6e25); A = mem_rd(gb, w1Link_xh);
  CYC(0x6e25, 0x6e26); E = A;
  CYC(0x6e26, 0x6e27); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6e27, 0x6e28); C = A;
  CYC(0x6e28, 0x6e2a); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(0x6e2a, 0x6e2c); goto invalid; } // jr nc
  CYC(0x6e2a, 0x6e2c);
  CYC(0x6e2c, 0x6e2d); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(0x6e2d, 0x6e2f); goto L_6e31; } // jr nc
  CYC(0x6e2d, 0x6e2f);
  CYC(0x6e2f, 0x6e30); alu_cpl(gb);
  CYC(0x6e30, 0x6e31); A = alu_inc8(gb, A);

L_6e31:
  CYC(0x6e31, 0x6e32); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x6e32, 0x6e35); getTileCollisionsAtPosition_hook(gb); return; } // jp nc
  CYC(0x6e32, 0x6e35);

invalid:
  CYC(0x6e35, 0x6e36); alu_or(gb, D);
  RET(0x6e36); return; // ret
}

// @param[out] zflag z if Link looked at the hag
void shadowHag_checkLinkLookedAtHag_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6e3f, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6e42);
  CYC(0x6e42, 0x6e44); alu_add(gb, 0x14);
  CYC(0x6e44, 0x6e46); alu_and(gb, 0x18);
  CYC(0x6e46, 0x6e48); A = alu_swap(gb, A);
  CYC(0x6e48, 0x6e49); alu_rlca(gb);
  CYC(0x6e49, 0x6e4a); B = A;
  CYC(0x6e4a, 0x6e4d); A = mem_rd(gb, w1Link_direction);
  CYC(0x6e4d, 0x6e4e); alu_cp(gb, B);
  RET(0x6e4e); return; // ret
}
