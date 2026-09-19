#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void floormaster_state_uninitialized_hook(GB *gb);
void floormaster_state1_hook(GB *gb);
void floormaster_state_galeSeed_hook(GB *gb);
void floormaster_state_stub_hook(GB *gb);
void floormaster_state8_hook(GB *gb);
void floormaster_state9_hook(GB *gb);
void floormaster_stateA_hook(GB *gb);
void floormaster_stateB_hook(GB *gb);
void floormaster_animate_hook(GB *gb);
void floormaster_stateC_hook(GB *gb);
void floormaster_stateD_hook(GB *gb);
void floormaster_updateAngleTowardLink_hook(GB *gb);
void floormaster_updateZPosition_hook(GB *gb);
void floormaster_checkLinkMoved8PixelsAway_hook(GB *gb);
void floormaster_initSpawner_hook(GB *gb);
void floormaster_getAdjacentWallsBitset_hook(GB *gb);

static uint16_t floormaster_jump_table(GB *gb) {
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

static void floormaster_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_FLOORMASTER
//
// Variables for subids other than 0:
//   relatedObj1: Reference to spawner object (subid 0)
//   var30: Animation index
//   var31: Index for z-position to use while chasing Link (0-7)
//   var32: Angle relative to Link where floormaster should spawn
//
// Variables for spawner (subid 0):
//   var30: Number of floormaster currently spawned (they delete themselves after
//          disappearing into the ground)
//   var31/var32: Link's position last time a floormaster was spawned. If Link hasn't moved
//                far from here, the floormaster will spawn at a random angle relative to
//                him; otherwise it will spawn in the direction Link is moving.
//   var33: # floormasters to spawn. Children decrement this when they're killed.
//          (High nibble of original Y value.)
//   var34: Subid for child objects (high nibble of original X value, plus one)
// ==================================================================================================
void enemyCode35_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x4b07, 0x4b09); goto normalStatus; } // jr z
  CYC(0x4b07, 0x4b09);
  CYC(0x4b09, 0x4b0b); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4b0b); return; } // ret c
  CYC(0x4b0b, 0x4b0c);
  if (F & FZ) { CYCT(0x4b0c, 0x4b0e); goto dead; } // jr z
  CYC(0x4b0c, 0x4b0e);
  CYC(0x4b0e, 0x4b0f); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4b0f, 0x4b12); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(0x4b0f, 0x4b12);
  CYC(0x4b12, 0x4b14); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4b14, 0x4b15); A = mem_rd(gb, DE);
  CYC(0x4b15, 0x4b17); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(0x4b17); return; } // ret nz
  CYC(0x4b17, 0x4b18);
  CYC(0x4b18, 0x4b19); H = D;
  CYC(0x4b19, 0x4b1b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4b1b, 0x4b1d); mem_wr(gb, HL, 0x0c);
  CYC(0x4b1d, 0x4b1f); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x4b1f, 0x4b21); mem_wr(gb, HL, 0xfb);
  CALL_C(0x4b21, floormaster_updateAngleTowardLink_hook, 0x4d0f, 0x4b24);
  CYC(0x4b24, 0x4b26); alu_add(gb, 0x04);
  CALL_C(0x4b26, enemySetAnimation_hook, 0x282b, 0x4b29);
  CYC(0x4b29, 0x4b2a); H = D;
  CYC(0x4b2a, 0x4b2c); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4b2c, 0x4b2f); A = mem_rd(gb, w1Link_yh);
  CYC(0x4b2f, 0x4b30); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4b30, 0x4b32); A = alu_sra(gb, A);
  CYC(0x4b32, 0x4b33); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4b33, 0x4b34); mem_wr(gb, HL, A);
  CYC(0x4b34, 0x4b36); L = ENEMY_BASE + OBJ_XH;
  CYC(0x4b36, 0x4b39); A = mem_rd(gb, w1Link_xh);
  CYC(0x4b39, 0x4b3a); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4b3a, 0x4b3c); A = alu_sra(gb, A);
  CYC(0x4b3c, 0x4b3d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4b3d, 0x4b3e); mem_wr(gb, HL, A);
  RET(0x4b3e); return; // ret

dead:
  CYC(0x4b3f, 0x4b41); A = 0x30; // Object.var30
  CALL_C(0x4b41, objectGetRelatedObject1Var_hook, 0x2160, 0x4b44);
  CYC(0x4b44, 0x4b45); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4b45, 0x4b47); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x4b47, 0x4b48); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4b48, 0x4b4b); enemyDie_uncounted_hook(gb); return; // jp

normalStatus:
  CYC(0x4b4b, 0x4b4d); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4b4d, 0x4b4e); A = mem_rd(gb, DE);
  {
    CYC(0x4b4e, 0x4b4f); push_effect(gb, 0x4b4f);
    uint16_t target = floormaster_jump_table(gb);
    if (target == 0x4b6b) { floormaster_state_uninitialized_hook(gb); return; }
    if (target == 0x4b7c) { floormaster_state1_hook(gb); return; }
    if (target == 0x4bc6) { floormaster_state_stub_hook(gb); return; }
    if (target == 0x4bb6) { floormaster_state_galeSeed_hook(gb); return; }
    if (target == 0x4bc7) { floormaster_state8_hook(gb); return; }
    if (target == 0x4c44) { floormaster_state9_hook(gb); return; }
    if (target == 0x4c58) { floormaster_stateA_hook(gb); return; }
    if (target == 0x4c9a) { floormaster_stateB_hook(gb); return; }
    if (target == 0x4cd2) { floormaster_stateC_hook(gb); return; }
    if (target == 0x4cfe) { floormaster_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

void floormaster_state_uninitialized_hook(GB *gb) {
  CYC(0x4b6b, 0x4b6c); H = D;
  CYC(0x4b6c, 0x4b6e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b6e, 0x4b70); mem_wr(gb, HL, 60);
  CYC(0x4b70, 0x4b71); L = E;
  CYC(0x4b71, 0x4b72); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state] = 1
  CYC(0x4b72, 0x4b74); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4b74, 0x4b75); A = mem_rd(gb, DE);
  CYC(0x4b75, 0x4b76); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4b76, 0x4b79); floormaster_initSpawner_hook(gb); return; } // jp z
  CYC(0x4b76, 0x4b79);
  CYC(0x4b79, 0x4b7b); mem_wr(gb, HL, 0x08); // [state] = 8
  RET(0x4b7b); return; // ret
}

// State 1: only for subid 0 (spawner).
void floormaster_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b7c, 0x4b7d); H = D;
  CYC(0x4b7d, 0x4b7f); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x4b7f, 0x4b80); A = mem_rd(gb, HL);
  CYC(0x4b80, 0x4b81); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4b81, 0x4b83); goto delete_; } // jr z
  CYC(0x4b81, 0x4b83);
  CYC(0x4b83, 0x4b85); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b85, 0x4b86); A = mem_rd(gb, DE);
  CYC(0x4b86, 0x4b87); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(0x4b87); return; } // ret nc
  CYC(0x4b87, 0x4b88);
  CYC(0x4b88, 0x4b8a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b8a, 0x4b8b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(0x4b8b); return; } // ret nz
  CYC(0x4b8b, 0x4b8c);
  CYC(0x4b8c, 0x4b8e); mem_wr(gb, HL, 0x01);
  CYC(0x4b8e, 0x4b90); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b90, 0x4b91); A = mem_rd(gb, HL);
  CYC(0x4b91, 0x4b93); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(0x4b93); return; } // ret nc
  CYC(0x4b93, 0x4b94);
  CYC(0x4b94, 0x4b96); B = 0x35; // ENEMY_FLOORMASTER
  CALL_C(0x4b96, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x4b99);
  if (!(F & FZ)) { RET_TAKEN(0x4b99); return; } // ret nz
  CYC(0x4b99, 0x4b9a);
  CYC(0x4b9a, 0x4b9c); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x4b9c, 0x4b9d); A = mem_rd(gb, DE);
  CYC(0x4b9d, 0x4b9e); mem_wr(gb, HL, A); // [child.subid] = [this.var34]
  CYC(0x4b9e, 0x4ba0); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x4ba0, 0x4ba2); A = 0x80; // Enemy.start
  CYC(0x4ba2, 0x4ba3); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4ba3, 0x4ba4); mem_wr(gb, HL, D);
  CYC(0x4ba4, 0x4ba5); H = D;
  CYC(0x4ba5, 0x4ba7); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4ba7, 0x4ba8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x4ba8, 0x4baa); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4baa, 0x4bac); mem_wr(gb, HL, 0x80);
  RET(0x4bac); return; // ret

delete_:
  CALL_C(0x4bad, decNumEnemies_hook, 0x24b3, 0x4bb0);
  CALL_C(0x4bb0, markEnemyAsKilledInRoom_b00_hook, 0x320d, 0x4bb3);
  CYC(0x4bb3, 0x4bb6); enemyDelete_hook(gb); return; // jp
}

void floormaster_state_galeSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4bb6, ecom_galeSeedEffect_b0e_hook, 0x447b, 0x4bb9);
  if (F & FC) { RET_TAKEN(0x4bb9); return; } // ret c
  CYC(0x4bb9, 0x4bba);
  CYC(0x4bba, 0x4bbc); A = 0x30; // Object.var30
  CALL_C(0x4bbc, objectGetRelatedObject1Var_hook, 0x2160, 0x4bbf);
  CYC(0x4bbf, 0x4bc0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4bc0, 0x4bc2); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x4bc2, 0x4bc3); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4bc3, 0x4bc6); enemyDelete_hook(gb); return; // jp
}

void floormaster_state_stub_hook(GB *gb) {
  RET(0x4bc6); return; // ret
}

// Choosing a position to spawn at.
void floormaster_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4bc7, floormaster_checkLinkMoved8PixelsAway_hook, 0x4d6f, 0x4bca);
  CYC(0x4bca, 0x4bcc); A = 0x00;
  PUSH(0x4bcc, BC);
  if (F & FC) CALL_C_CC(0x4bcd, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4bd0); else CYC(0x4bcd, 0x4bd0); // call c
  SET_BC(POP(0x4bd0));
  CYC(0x4bd1, 0x4bd2); E = A;
  CYC(0x4bd2, 0x4bd5); A = mem_rd(gb, w1Link_angle);
  CYC(0x4bd5, 0x4bd6); alu_add(gb, E);
  CYC(0x4bd6, 0x4bd8); alu_and(gb, 0x1f);
  CYC(0x4bd8, 0x4bda); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4bda, 0x4bdb); mem_wr(gb, DE, A);
  CYC(0x4bdb, 0x4bdd); A = 0x50;
  CYC(0x4bdd, 0x4bdf); hram_wr(gb, 0x8a, A);

tryDistance:
  CYC(0x4bdf, 0x4be1); A = hram_rd(gb, 0x8a);
  CYC(0x4be1, 0x4be3); alu_sub(gb, 0x10);
  if (F & FZ) { CYCT(0x4be3, 0x4be5); goto doneLoop; } // jr z
  CYC(0x4be3, 0x4be5);
  CYC(0x4be5, 0x4be7); hram_wr(gb, 0x8a, A);
  PUSH(0x4be7, BC);
  CYC(0x4be8, 0x4bea); E = ENEMY_BASE + 0x32; // Enemy.var32
  CALL_C(0x4bea, objectSetPositionInCircleArc_hook, 0x210e, 0x4bed);
  SET_BC(POP(0x4bed));
  CYC(0x4bee, 0x4bef); A = mem_rd(gb, DE);
  CYC(0x4bef, 0x4bf0); E = A;
  CYC(0x4bf0, 0x4bf3); A = mem_rd(gb, w1Link_xh);
  CYC(0x4bf3, 0x4bf4); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(0x4bf4, 0x4bf6); goto L_4bf8; } // jr nc
  CYC(0x4bf4, 0x4bf6);
  CYC(0x4bf6, 0x4bf7); alu_cpl(gb);
  CYC(0x4bf7, 0x4bf8); A = alu_inc8(gb, A);

L_4bf8:
  CYC(0x4bf8, 0x4bfa); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(0x4bfa, 0x4bfc); goto tryDistance; } // jr nc
  CYC(0x4bfa, 0x4bfc);
  CYC(0x4bfc, 0x4bfe); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4bfe, 0x4bff); A = mem_rd(gb, DE);
  CYC(0x4bff, 0x4c01); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(0x4c01, 0x4c03); goto tryDistance; } // jr nc
  CYC(0x4c01, 0x4c03);
  PUSH(0x4c03, BC);
  CALL_C(0x4c04, objectGetTileCollisions_hook, 0x14ad, 0x4c07);
  SET_BC(POP(0x4c07));
  if (!(F & FZ)) { CYCT(0x4c08, 0x4c0a); goto tryDistance; } // jr nz
  CYC(0x4c08, 0x4c0a);
  CYC(0x4c0a, 0x4c0b); H = D;
  CYC(0x4c0b, 0x4c0d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4c0d, 0x4c0f); mem_wr(gb, HL, 0x09);
  CYC(0x4c0f, 0x4c11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4c11, 0x4c13); mem_wr(gb, HL, 0x20);
  CALL_C(0x4c13, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4c16);
  CYC(0x4c16, 0x4c17); B = A;
  CYC(0x4c17, 0x4c19); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4c19, 0x4c1a); A = mem_rd(gb, DE);
  CYC(0x4c1a, 0x4c1b); A = alu_dec8(gb, A);
  CYC(0x4c1b, 0x4c1c); A = B;
  if (!(F & FZ)) { CYCT(0x4c1c, 0x4c1e); goto L_4c22; } // jr nz
  CYC(0x4c1c, 0x4c1e);
  CYC(0x4c1e, 0x4c20); alu_add(gb, 0x04);
  CYC(0x4c20, 0x4c22); alu_and(gb, 0x18);

L_4c22:
  CYC(0x4c22, 0x4c24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4c24, 0x4c25); mem_wr(gb, DE, A);
  CYC(0x4c25, 0x4c27); alu_cp(gb, 0x10);
  CYC(0x4c27, 0x4c29); A = 0x00;
  if (!(F & FC)) { CYCT(0x4c29, 0x4c2b); goto L_4c2c; } // jr nc
  CYC(0x4c29, 0x4c2b);
  CYC(0x4c2b, 0x4c2c); A = alu_inc8(gb, A);

L_4c2c:
  CYC(0x4c2c, 0x4c2e); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4c2e, 0x4c2f); mem_wr(gb, DE, A);
  CALL_C(0x4c2f, enemySetAnimation_hook, 0x282b, 0x4c32);
  CALL_C(0x4c32, objectSetVisiblec1_hook, 0x1e3c, 0x4c35);

doneLoop:
  CYC(0x4c35, 0x4c37); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x4c37, 0x4c38); A = mem_rd(gb, DE);
  CYC(0x4c38, 0x4c39); H = A;
  CYC(0x4c39, 0x4c3b); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4c3b, 0x4c3e); A = mem_rd(gb, w1Link_yh);
  CYC(0x4c3e, 0x4c3f); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4c3f, 0x4c42); A = mem_rd(gb, w1Link_xh);
  CYC(0x4c42, 0x4c43); mem_wr(gb, HL, A);
  RET(0x4c43); return; // ret
}

// Emerging from ground
void floormaster_state9_hook(GB *gb) {
  CYC(0x4c44, 0x4c46); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4c46, 0x4c47); A = mem_rd(gb, DE);
  CYC(0x4c47, 0x4c48); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4c48, 0x4c4b); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x4c48, 0x4c4b);
  CYC(0x4c4b, 0x4c4d); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4c4d, 0x4c4f); A = 0x0a;
  CYC(0x4c4f, 0x4c50); mem_wr(gb, DE, A);
  CYC(0x4c50, 0x4c52); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4c52, 0x4c53); A = mem_rd(gb, DE);
  CYC(0x4c53, 0x4c55); alu_add(gb, 0x02);
  CYC(0x4c55, 0x4c58); enemySetAnimation_hook(gb); return; // jp
}

// Floating in place for [counter1] frames before chasing Link
void floormaster_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4c58, ecom_decCounter1_b0e_hook, 0x439a, 0x4c5b);
  if (F & FZ) { CYCT(0x4c5b, 0x4c5d); goto beginChasing; } // jr z
  CYC(0x4c5b, 0x4c5d);
  CYC(0x4c5d, 0x4c5e); A = mem_rd(gb, HL);
  CYC(0x4c5e, 0x4c60); A = alu_srl(gb, A);
  CYC(0x4c60, 0x4c62); A = alu_srl(gb, A);
  CYC(0x4c62, 0x4c65); SET_HL(0x4c92); // @zVals (data)
  CYC(0x4c65, 0x4c66); floormaster_addAToHl_from_rst(gb, 0x4c66);
  CYC(0x4c66, 0x4c67); A = mem_rd(gb, HL);
  CYC(0x4c67, 0x4c69); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x4c69, 0x4c6a); mem_wr(gb, DE, A);
  RET(0x4c6a); return; // ret

beginChasing:
  CYC(0x4c6b, 0x4c6d); mem_wr(gb, HL, 0xf0); // [counter1]
  CYC(0x4c6d, 0x4c6f); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4c6f, 0x4c71); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4c71, 0x4c73); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4c73, 0x4c75); mem_wr(gb, HL, 0x0b);
  CALL_C(0x4c75, floormaster_updateAngleTowardLink_hook, 0x4d0f, 0x4c78);
  CYC(0x4c78, 0x4c79); B = A;
  CYC(0x4c79, 0x4c7b); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x4c7b, 0x4c7c); A = mem_rd(gb, DE);
  CYC(0x4c7c, 0x4c7d); H = A;
  CYC(0x4c7d, 0x4c7f); L = ENEMY_BASE + OBJ_XH;
  CYC(0x4c7f, 0x4c81); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(0x4c81, 0x4c82); H = D;
  CYC(0x4c82, 0x4c84); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4c84, 0x4c86); mem_wr(gb, HL, 0x0f); // SPEED_60
  if (F & FZ) { CYCT(0x4c86, 0x4c88); goto L_4c8a; } // jr z
  CYC(0x4c86, 0x4c88);
  CYC(0x4c88, 0x4c8a); mem_wr(gb, HL, 0x19); // SPEED_a0

L_4c8a:
  CYC(0x4c8a, 0x4c8b); A = B;
  CYC(0x4c8b, 0x4c8d); alu_add(gb, 0x02);
  CALL_C(0x4c8d, enemySetAnimation_hook, 0x282b, 0x4c90);
  CYCT(0x4c90, 0x4c92); floormaster_animate_hook(gb); return; // jr
}

// Chasing Link
void floormaster_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4c9a, ecom_decCounter1_b0e_hook, 0x439a, 0x4c9d);
  if (!(F & FZ)) { CYCT(0x4c9d, 0x4c9f); goto stillChasing; } // jr nz
  CYC(0x4c9d, 0x4c9f);
  CYC(0x4c9f, 0x4ca1); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x4ca1, 0x4ca3); mem_wr(gb, HL, 0x00);
  CYC(0x4ca3, 0x4ca5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4ca5, 0x4ca7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4ca7, 0x4ca8); L = E;
  CYC(0x4ca8, 0x4caa); mem_wr(gb, HL, 0x0d); // [state]
  CYC(0x4caa, 0x4cac); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4cac, 0x4cae); A = 0x06;
  CYC(0x4cae, 0x4caf); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4caf, 0x4cb2); enemySetAnimation_hook(gb); return; // jp

stillChasing:
  CYC(0x4cb2, 0x4cb4); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4cb4, 0x4cb5); A = mem_rd(gb, DE);
  CYC(0x4cb5, 0x4cb7); hram_wr(gb, 0x8d, A);
  CALL_C(0x4cb7, floormaster_updateAngleTowardLink_hook, 0x4d0f, 0x4cba);
  CYC(0x4cba, 0x4cbb); B = A;
  CYC(0x4cbb, 0x4cbd); A = hram_rd(gb, 0x8d);
  CYC(0x4cbd, 0x4cbe); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x4cbe, 0x4cc0); goto L_4cc6; } // jr z
  CYC(0x4cbe, 0x4cc0);
  CYC(0x4cc0, 0x4cc2); A = 0x02;
  CYC(0x4cc2, 0x4cc3); alu_add(gb, B);
  CALL_C(0x4cc3, enemySetAnimation_hook, 0x282b, 0x4cc6);

L_4cc6:
  CALL_C(0x4cc6, floormaster_updateZPosition_hook, 0x4d51, 0x4cc9);
  CALL_C(0x4cc9, floormaster_getAdjacentWallsBitset_hook, 0x4d9f, 0x4ccc);
  CALL_C(0x4ccc, ecom_applyVelocityGivenAdjacentWalls_b0e_hook, 0x415b, 0x4ccf);
  floormaster_animate_hook(gb); return; // fallthrough
}

void floormaster_animate_hook(GB *gb) {
  CYC(0x4ccf, 0x4cd2); enemyAnimate_hook(gb); return; // jp
}

// Grabbing Link
void floormaster_stateC_hook(GB *gb) {
  CYC(0x4cd2, 0x4cd4); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4cd4, 0x4cd5); A = mem_rd(gb, DE);
  CYC(0x4cd5, 0x4cd6); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4cd6, 0x4cd8); goto makeLinkInvisible; } // jr z
  CYC(0x4cd6, 0x4cd8);
  CYC(0x4cd8, 0x4cd9); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4cd9, 0x4cdb); goto setZToZero; } // jr z
  CYC(0x4cd9, 0x4cdb);
  CYC(0x4cdb, 0x4cdc); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4cdc, 0x4cde); floormaster_animate_hook(gb); return; } // jr nz
  CYC(0x4cdc, 0x4cde);
  CYC(0x4cde, 0x4ce0); A = 0x02;
  CYC(0x4ce0, 0x4ce3); mem_wr(gb, w1Link_substate, A);
  CYC(0x4ce3, 0x4ce6); objectSetInvisible_hook(gb); return; // jp

makeLinkInvisible: // [animParameter] == 1
  CYC(0x4ce6, 0x4ce7); mem_wr(gb, DE, A);
  CYC(0x4ce7, 0x4cea); mem_wr(gb, w1Link_visible, A);
  CYC(0x4cea, 0x4cec); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4cec, 0x4cef); A = mem_rd(gb, w1Link_yh);
  CYC(0x4cef, 0x4cf0); mem_wr(gb, DE, A);
  CYC(0x4cf0, 0x4cf2); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4cf2, 0x4cf5); A = mem_rd(gb, w1Link_xh);
  CYC(0x4cf5, 0x4cf6); mem_wr(gb, DE, A);
  RET(0x4cf6); return; // ret

setZToZero: // [animParameter] == 2
  CYC(0x4cf7, 0x4cf8); alu_xor(gb, A);
  CYC(0x4cf8, 0x4cf9); mem_wr(gb, DE, A);
  CYC(0x4cf9, 0x4cfb); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x4cfb, 0x4cfc); mem_wr(gb, DE, A);
  CYCT(0x4cfc, 0x4cfe); floormaster_animate_hook(gb); return; // jr
}

// Sinking into ground
void floormaster_stateD_hook(GB *gb) {
  CYC(0x4cfe, 0x4d00); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4d00, 0x4d01); A = mem_rd(gb, DE);
  CYC(0x4d01, 0x4d03); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x4d03, 0x4d05); floormaster_animate_hook(gb); return; } // jr nz
  CYC(0x4d03, 0x4d05);
  CYC(0x4d05, 0x4d07); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x4d07, 0x4d08); A = mem_rd(gb, DE);
  CYC(0x4d08, 0x4d09); H = A;
  CYC(0x4d09, 0x4d0b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4d0b, 0x4d0c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4d0c, 0x4d0f); enemyDelete_hook(gb); return; // jp
}

// @param[out]  a  Value written to var30 (0 if Link is to the left, 1 if right)
void floormaster_updateAngleTowardLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d0f, 0x4d12); push_effect(gb, 0x4d12); // call @checkLinkCollisionsEnabled (inlined local sub-label; not independently hookable)
  goto checkLinkCollisionsEnabled;

afterCheck:
  if (!(F & FC)) { RET_TAKEN(0x4d12); return; } // ret nc
  CYC(0x4d12, 0x4d13);
  CALL_C(0x4d13, objectGetAngleTowardLink_hook, 0x1e9c, 0x4d16);
  CYC(0x4d16, 0x4d17); B = A;
  CYC(0x4d17, 0x4d19); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x4d19, 0x4d1b); goto L_4d23; } // jr nz
  CYC(0x4d19, 0x4d1b);
  CYC(0x4d1b, 0x4d1d); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4d1d, 0x4d1e); A = B;
  CYC(0x4d1e, 0x4d1f); mem_wr(gb, DE, A);
  CYC(0x4d1f, 0x4d21); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4d21, 0x4d22); A = mem_rd(gb, DE);
  RET(0x4d22); return; // ret

L_4d23:
  CYC(0x4d23, 0x4d25); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4d25, 0x4d26); A = mem_rd(gb, DE);
  CYC(0x4d26, 0x4d27); A = alu_dec8(gb, A);
  CYC(0x4d27, 0x4d28); A = B;
  if (!(F & FZ)) { CYCT(0x4d28, 0x4d2a); goto subid0; } // jr nz
  CYC(0x4d28, 0x4d2a);

  // subid1: only move in cardinal directions
  CYC(0x4d2a, 0x4d2c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4d2c, 0x4d2e); alu_and(gb, 0xf8);
  CYC(0x4d2e, 0x4d2f); mem_wr(gb, DE, A);
  CYC(0x4d2f, 0x4d31); alu_cp(gb, 0x10);
  CYC(0x4d31, 0x4d33); A = 0x00;
  if (!(F & FC)) { CYCT(0x4d33, 0x4d35); goto L_4d36; } // jr nc
  CYC(0x4d33, 0x4d35);
  CYC(0x4d35, 0x4d36); A = alu_inc8(gb, A);

L_4d36:
  CYC(0x4d36, 0x4d38); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4d38, 0x4d39); mem_wr(gb, DE, A);
  RET(0x4d39); return; // ret

subid0:
  CYC(0x4d3a, 0x4d3c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4d3c, 0x4d3d); mem_wr(gb, DE, A);
  CYC(0x4d3d, 0x4d3f); alu_cp(gb, 0x10);
  CYC(0x4d3f, 0x4d41); A = 0x00;
  if (!(F & FC)) { CYCT(0x4d41, 0x4d43); goto L_4d44; } // jr nc
  CYC(0x4d41, 0x4d43);
  CYC(0x4d43, 0x4d44); A = alu_inc8(gb, A);

L_4d44:
  CYC(0x4d44, 0x4d46); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4d46, 0x4d47); mem_wr(gb, DE, A);
  RET(0x4d47); return; // ret

checkLinkCollisionsEnabled:
  CYC(0x4d48, 0x4d4b); A = mem_rd(gb, w1Link_collisionType);
  CYC(0x4d4b, 0x4d4c); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x4d4c); goto afterCheck; } // ret c
  CYC(0x4d4c, 0x4d4d);
  CYC(0x4d4d, 0x4d4f); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4d4f, 0x4d50); A = mem_rd(gb, DE);
  RET(0x4d50); goto afterCheck; // ret
}

void floormaster_updateZPosition_hook(GB *gb) {
  CYC(0x4d51, 0x4d53); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4d53, 0x4d54); A = mem_rd(gb, DE);
  CYC(0x4d54, 0x4d56); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x4d56); return; } // ret nz
  CYC(0x4d56, 0x4d57);
  CYC(0x4d57, 0x4d59); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4d59, 0x4d5a); A = mem_rd(gb, DE);
  CYC(0x4d5a, 0x4d5b); A = alu_inc8(gb, A);
  CYC(0x4d5b, 0x4d5d); alu_and(gb, 0x07);
  CYC(0x4d5d, 0x4d5e); mem_wr(gb, DE, A);
  CYC(0x4d5e, 0x4d61); SET_HL(0x4d67); // @zVals (data)
  CYC(0x4d61, 0x4d62); floormaster_addAToHl_from_rst(gb, 0x4d62);
  CYC(0x4d62, 0x4d64); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x4d64, 0x4d65); A = mem_rd(gb, HL);
  CYC(0x4d65, 0x4d66); mem_wr(gb, DE, A);
  RET(0x4d66); return; // ret
}

// Checks whether Link has moved 8 pixels away from his position last time a floormaster
// was spawned.
//
// @param[out]  bc  Link's position
// @param[out]  cflag  c if he's within 8 pixels
void floormaster_checkLinkMoved8PixelsAway_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d6f, 0x4d71); A = 0x31; // Object.var31
  CALL_C(0x4d71, objectGetRelatedObject1Var_hook, 0x2160, 0x4d74);
  CYC(0x4d74, 0x4d77); A = mem_rd(gb, w1Link_yh);
  CYC(0x4d77, 0x4d78); B = A;
  CYC(0x4d78, 0x4d79); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4d79, 0x4d7b); alu_add(gb, 0x08);
  CYC(0x4d7b, 0x4d7d); alu_cp(gb, 0x10);
  CYC(0x4d7d, 0x4d80); A = mem_rd(gb, w1Link_xh);
  CYC(0x4d80, 0x4d81); C = A;
  if (!(F & FC)) { RET_TAKEN(0x4d81); return; } // ret nc
  CYC(0x4d81, 0x4d82);
  CYC(0x4d82, 0x4d83); L = alu_inc8(gb, L);
  CYC(0x4d83, 0x4d84); alu_sub(gb, mem_rd(gb, HL)); // [var32]
  CYC(0x4d84, 0x4d86); alu_add(gb, 0x08);
  CYC(0x4d86, 0x4d88); alu_cp(gb, 0x10);
  RET(0x4d88); return; // ret
}

void floormaster_initSpawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4d89, 0x4d8b); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4d8b, 0x4d8c); A = mem_rd(gb, DE);
  CYC(0x4d8c, 0x4d8e); alu_and(gb, 0xf0);
  CYC(0x4d8e, 0x4d90); alu_swap_a(gb);
  CYC(0x4d90, 0x4d92); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x4d92, 0x4d93); mem_wr(gb, DE, A);
  CYC(0x4d93, 0x4d95); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4d95, 0x4d96); A = mem_rd(gb, DE);
  CYC(0x4d96, 0x4d98); alu_and(gb, 0xf0);
  CYC(0x4d98, 0x4d9a); alu_swap_a(gb);
  CYC(0x4d9a, 0x4d9b); A = alu_inc8(gb, A);
  CYC(0x4d9b, 0x4d9d); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x4d9d, 0x4d9e); mem_wr(gb, DE, A);
  RET(0x4d9e); return; // ret
}

// Only screen boundaries count as "walls" for floormaster.
void floormaster_getAdjacentWallsBitset_hook(GB *gb) {
  CYC(0x4d9f, 0x4da1); A = 0x02;
  CYC(0x4da1, 0x4da4); ecom_getTopDownAdjacentWallsBitset_b0e_hook(gb); return; // jp
}
