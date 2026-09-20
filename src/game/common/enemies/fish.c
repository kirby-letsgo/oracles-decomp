#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void fish_state_uninitialized_hook(GB *gb);
void fish_state_stub_hook(GB *gb);
void fish_subid00_hook(GB *gb);
void fish_enterWater_hook(GB *gb);
void fish_subid01_hook(GB *gb);
void fish_checkReverseAngle_hook(GB *gb);
void fish_updateAnimationFromAngle_hook(GB *gb);
void fish_setAnimation_hook(GB *gb);
void fish_updatePosition_hook(GB *gb);
void fish_setRandomCounter1_hook(GB *gb);
void fish_getAdjacentWallsBitsetForKnockback_hook(GB *gb);

static uint16_t fish_jump_table(GB *gb) {
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

static void fish_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_PIRANHA
//
// Variables:
//   zh: Equals 2 when underwater
//   var30: Current animation index
// ==================================================================================================
void enemyCode1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5a6c, 0x5a6e); goto normalStatus; } // jr z
  CYC(0x5a6c, 0x5a6e);
  CYC(0x5a6e, 0x5a70); alu_sub(gb, 0x03);
  if (F & FC) { CYCT(0x5a70, 0x5a72); goto stunned; } // jr c
  CYC(0x5a70, 0x5a72);
  if (F & FZ) { CYCT(0x5a72, 0x5a75); enemyDie_hook(gb); return; } // jp z
  CYC(0x5a72, 0x5a75);
  CYC(0x5a75, 0x5a76); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x5a76); return; } // ret z
  CYC(0x5a76, 0x5a77);

  // ENEMYSTATUS_KNOCKBACK
  CYC(0x5a77, 0x5a79); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5a79, 0x5a7b); A = 0x50; // SPEED_200
  CYC(0x5a7b, 0x5a7c); mem_wr(gb, DE, A);
  CALL_C(0x5a7c, fish_getAdjacentWallsBitsetForKnockback_hook, 0x5bbb, 0x5a7f);
  CYC(0x5a7f, 0x5a81); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(0x5a81, ecom_applyVelocityGivenAdjacentWalls_b0d_hook, 0x415b, 0x5a84);
  CYC(0x5a84, 0x5a86); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5a86, 0x5a88); A = 0x1e; // SPEED_c0
  CYC(0x5a88, 0x5a89); mem_wr(gb, DE, A);
  RET(0x5a89); return; // ret

stunned:
  CYC(0x5a8a, 0x5a8c); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x5a8c, 0x5a8d); A = mem_rd(gb, DE);
  CYC(0x5a8d, 0x5a8f); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(0x5a8f); return; } // ret z
  CYC(0x5a8f, 0x5a90);
  CYC(0x5a90, 0x5a91); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5a91); return; } // ret nz
  CYC(0x5a91, 0x5a92);
  CYC(0x5a92, 0x5a95); fish_enterWater_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5a95, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x5a98);
  if (!(F & FC)) { CYCT(0x5a98, 0x5a9a); goto normalState; } // jr nc
  CYC(0x5a98, 0x5a9a);
  {
    CYC(0x5a9a, 0x5a9b); push_effect(gb, 0x5a9b);
    uint16_t target = fish_jump_table(gb);
    if (target == 0x5ab1) { fish_state_uninitialized_hook(gb); return; }
    if (target == 0x5ac7) { fish_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x5aab, 0x5aac); A = B;
  {
    CYC(0x5aac, 0x5aad); push_effect(gb, 0x5aad);
    uint16_t target = fish_jump_table(gb);
    if (target == 0x5ac8) { fish_subid00_hook(gb); return; }
    if (target == 0x5b3c) { fish_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5ab1, bare global; jump-table target from enemyCode1e.
void fish_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ab1, 0x5ab3); A = 0x14; // SPEED_80
  CALL_C(0x5ab3, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x5ab6);
  CALL_C(0x5ab6, objectSetVisible83_hook, 0x1e72, 0x5ab9);
  CYC(0x5ab9, 0x5abb); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5abb, 0x5abd); mem_wr(gb, HL, 0x02);
  CYC(0x5abd, 0x5abf); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5abf, 0x5ac1); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CALL_C(0x5ac1, fish_setRandomCounter1_hook, 0x5ba9, 0x5ac4);
  CYC(0x5ac4, 0x5ac7); fish_updateAnimationFromAngle_hook(gb); return; // jp
}

// 0d:5ac7, bare global; jump-table target from enemyCode1e.
void fish_state_stub_hook(GB *gb) {
  RET(0x5ac7); return; // ret
}

// 0d:5ac8, bare global; jump-table target from enemyCode1e@normalState. Internal @state8/
// @leapOutOfWater/@state9 are @-local (no separate registration), handled via goto.
void fish_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ac8, 0x5ac9); A = mem_rd(gb, DE);
  CYC(0x5ac9, 0x5acb); alu_sub(gb, 0x08);
  {
    CYC(0x5acb, 0x5acc); push_effect(gb, 0x5acc);
    uint16_t target = fish_jump_table(gb);
    if (target == 0x5ad0) goto state8;
    if (target == 0x5b03) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(0x5ad0, 0x5ad3); A = mem_rd(gb, wScentSeedActive);
  CYC(0x5ad3, 0x5ad4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5ad4, 0x5ad6); goto afterScentCheck; } // jr nz
  CYC(0x5ad4, 0x5ad6);
  CALL_C(0x5ad6, ecom_decCounter1_b0d_hook, 0x439a, 0x5ad9);
  if (F & FZ) { CYCT(0x5ad9, 0x5adb); goto leapOutOfWater; } // jr z
  CYC(0x5ad9, 0x5adb);

afterScentCheck:
  CALL_C(0x5adb, fish_updatePosition_hook, 0x5b79, 0x5ade);
  CYC(0x5ade, 0x5ae1); fish_checkReverseAngle_hook(gb); return; // jp

leapOutOfWater:
  CYC(0x5ae1, 0x5ae2); L = E;
  CYC(0x5ae2, 0x5ae3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5ae3, 0x5ae5); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5ae5, 0x5ae7); mem_wr(gb, HL, 0x14); // ENEMYCOLLISION_SWITCHHOOK_DAMAGE_ENEMY
  CYC(0x5ae7, 0x5ae9); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5ae9, 0x5aeb); mem_wr(gb, HL, 0x00);
  CYC(0x5aeb, 0x5aed); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5aed, 0x5aef); A = 0x80; // <(-$180)
  CYC(0x5aef, 0x5af0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5af0, 0x5af2); mem_wr(gb, HL, 0xfe); // >(-$180)
  CYC(0x5af2, 0x5af4); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5af4, 0x5af6); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x5af6, 0x5af8); B = 0x03; // INTERAC_SPLASH
  CALL_C(0x5af8, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5afb);
  CALL_C(0x5afb, objectSetVisiblec1_hook, 0x1e3c, 0x5afe);
  CYC(0x5afe, 0x5b00); B = 0x00;
  CYC(0x5b00, 0x5b03); fish_setAnimation_hook(gb); return; // jp

state9:
  CYC(0x5b03, 0x5b05); C = 0x10;
  CALL_C(0x5b05, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5b08);
  if (F & FZ) { CYCT(0x5b08, 0x5b0a); fish_enterWater_hook(gb); return; } // jr z
  CYC(0x5b08, 0x5b0a);
  CYC(0x5b0a, 0x5b0c); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5b0c, 0x5b0d); A = mem_rd(gb, HL);
  CYC(0x5b0d, 0x5b0e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5b0e, 0x5b10); goto state9UpdatePosition; } // jr nz
  CYC(0x5b0e, 0x5b10);
  CYC(0x5b10, 0x5b11); L = alu_inc8(gb, L);
  CYC(0x5b11, 0x5b12); A = mem_rd(gb, HL);
  CYC(0x5b12, 0x5b13); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5b13, 0x5b15); goto state9UpdatePosition; } // jr nz
  CYC(0x5b13, 0x5b15);
  CYC(0x5b15, 0x5b17); B = 0x01;
  CALL_C(0x5b17, fish_setAnimation_hook, 0x5b62, 0x5b1a);

state9UpdatePosition:
  CYC(0x5b1a, 0x5b1d); fish_updatePosition_hook(gb); return; // jp
}

// 0d:5b1d, bare global; called from enemyCode1e and fish_subid00.
void fish_enterWater_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b1d, 0x5b1e); H = D;
  CYC(0x5b1e, 0x5b20); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5b20, 0x5b22); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(0x5b22, 0x5b24); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5b24, 0x5b26); mem_wr(gb, HL, 0x02);
  CYC(0x5b26, 0x5b28); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5b28, 0x5b2a); mem_wr(gb, HL, 0x08);
  CYC(0x5b2a, 0x5b2c); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5b2c, 0x5b2e); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(0x5b2e, fish_setRandomCounter1_hook, 0x5ba9, 0x5b31);
  CYC(0x5b31, 0x5b33); B = 0x03; // INTERAC_SPLASH
  CALL_C(0x5b33, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5b36);
  CALL_C(0x5b36, objectSetVisible83_hook, 0x1e72, 0x5b39);
  CYC(0x5b39, 0x5b3c); fish_updateAnimationFromAngle_hook(gb); return; // jp
}

// 0d:5b3c, bare global; jump-table target from enemyCode1e@normalState. Internal @state8
// is @-local (no separate registration).
void fish_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b3c, 0x5b3d); A = mem_rd(gb, DE);
  CYC(0x5b3d, 0x5b3f); alu_sub(gb, 0x08);
  {
    CYC(0x5b3f, 0x5b40); push_effect(gb, 0x5b40);
    uint16_t target = fish_jump_table(gb);
    if (target == 0x5b42) { RET(0x5b42); return; }
    HANDOFF(target);
  }
}

// 0d:5b43, bare global; called from fish_subid00, falls through into
// fish_updateAnimationFromAngle.
// @param cflag c if we were able to move
void fish_checkReverseAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FC) { RET_TAKEN(0x5b43); return; } // ret c
  CYC(0x5b43, 0x5b44);
  CYC(0x5b44, 0x5b46); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5b46, 0x5b47); A = mem_rd(gb, DE);
  CYC(0x5b47, 0x5b49); alu_xor(gb, 0x10);
  CYC(0x5b49, 0x5b4a); mem_wr(gb, DE, A);
  fish_updateAnimationFromAngle_hook(gb); return; // fallthrough
}

// 0d:5b4a, bare global; called from fish_enterWater and fish_state_uninitialized, also
// falls into from fish_checkReverseAngle.
void fish_updateAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b4a, 0x5b4c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5b4c, 0x5b4d); A = mem_rd(gb, DE);
  CYC(0x5b4d, 0x5b4f); A = alu_swap(gb, A);
  CYC(0x5b4f, 0x5b50); alu_rlca(gb);
  CYC(0x5b50, 0x5b53); SET_HL(0x5b5e); // @animations
  CYC(0x5b53, 0x5b54); fish_addAToHl_from_rst(gb, 0x5b54);
  CYC(0x5b54, 0x5b55); A = mem_rd(gb, HL);
  CYC(0x5b55, 0x5b56); H = D;
  CYC(0x5b56, 0x5b58); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5b58, 0x5b59); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5b59); return; } // ret z
  CYC(0x5b59, 0x5b5a);
  CYC(0x5b5a, 0x5b5b); mem_wr(gb, HL, A);
  CYC(0x5b5b, 0x5b5e); enemySetAnimation_hook(gb); return; // jp
}

// 0d:5b62, bare global; called from fish_subid00. Sets animation (3 or 5 is added to value
// passed if we're moving right or left).
// @param b Value to add to animation index
void fish_setAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b62, 0x5b64); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5b64, 0x5b65); A = mem_rd(gb, DE);
  CYC(0x5b65, 0x5b67); A = alu_swap(gb, A);
  CYC(0x5b67, 0x5b69); alu_and(gb, 0x01);
  CYC(0x5b69, 0x5b6b); A = 0x03;
  if (!(F & FZ)) { CYCT(0x5b6b, 0x5b6d); goto addOffset; } // jr nz
  CYC(0x5b6b, 0x5b6d);
  CYC(0x5b6d, 0x5b6f); A = 0x05;

addOffset:
  CYC(0x5b6f, 0x5b70); alu_add(gb, B);
  CYC(0x5b70, 0x5b71); H = D;
  CYC(0x5b71, 0x5b73); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5b73, 0x5b74); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5b74); return; } // ret z
  CYC(0x5b74, 0x5b75);
  CYC(0x5b75, 0x5b76); mem_wr(gb, HL, A);
  CYC(0x5b76, 0x5b79); enemySetAnimation_hook(gb); return; // jp
}

// 0d:5b79, bare global; called from fish_subid00.
// @param[out] cflag c if we were able to move (tile in front of us is traversable)
void fish_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b79, 0x5b7b); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5b7b, 0x5b7c); A = mem_rd(gb, DE);
  CYC(0x5b7c, 0x5b7d); alu_rrca(gb);
  CYC(0x5b7d, 0x5b7e); alu_rrca(gb);
  CYC(0x5b7e, 0x5b81); SET_HL(0x5ba1); // @directionOffsets
  CYC(0x5b81, 0x5b82); fish_addAToHl_from_rst(gb, 0x5b82);
  CYC(0x5b82, 0x5b84); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5b84, 0x5b85); A = mem_rd(gb, DE);
  CYC(0x5b85, 0x5b86); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5b86, 0x5b88); alu_and(gb, 0xf0);
  CYC(0x5b88, 0x5b89); C = A;
  CYC(0x5b89, 0x5b8a); SET_HL(HL + 1); // inc hl
  CYC(0x5b8a, 0x5b8c); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5b8c, 0x5b8d); A = mem_rd(gb, DE);
  CYC(0x5b8d, 0x5b8e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5b8e, 0x5b90); alu_and(gb, 0xf0);
  CYC(0x5b90, 0x5b92); A = alu_swap(gb, A);
  CYC(0x5b92, 0x5b93); alu_or(gb, C);
  CYC(0x5b93, 0x5b94); C = A;
  CYC(0x5b94, 0x5b96); B = 0xcf; // >wRoomLayout
  CYC(0x5b96, 0x5b97); A = mem_rd(gb, BC);
  CYC(0x5b97, 0x5b99); alu_sub(gb, 0xf9); // TILEINDEX_PUDDLE
  CYC(0x5b99, 0x5b9b); alu_cp(gb, 0x05); // TILEINDEX_FD-TILEINDEX_PUDDLE+1
  if (!(F & FC)) { RET_TAKEN(0x5b9b); return; } // ret nc
  CYC(0x5b9b, 0x5b9c);
  CALL_C(0x5b9c, objectApplySpeed_hook, 0x201d, 0x5b9f);
  CYC(0x5b9f, 0x5ba0); alu_scf(gb);
  RET(0x5ba0); return; // ret
}

// 0d:5ba9, bare global; called from fish_enterWater and fish_state_uninitialized.
void fish_setRandomCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ba9, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5bac);
  CYC(0x5bac, 0x5bae); alu_and(gb, 0x03);
  CYC(0x5bae, 0x5bb1); SET_HL(0x5bb7); // @counter1Vals
  CYC(0x5bb1, 0x5bb2); fish_addAToHl_from_rst(gb, 0x5bb2);
  CYC(0x5bb2, 0x5bb4); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5bb4, 0x5bb5); A = mem_rd(gb, HL);
  CYC(0x5bb5, 0x5bb6); mem_wr(gb, DE, A);
  RET(0x5bb6); return; // ret
}

// 0d:5bbb, bare global; called from enemyCode1e. Gets the "adjacent walls bitset" for the
// fish; since this swims, water is traversable, everything else is not. Identical to
// waterTektite_getAdjacentWallsBitsetGivenAngle.
// @param[out] hFF8B Bitset of adjacent walls
void fish_getAdjacentWallsBitsetForKnockback_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5bbb, 0x5bbd); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x5bbd, 0x5bbe); A = mem_rd(gb, DE);
  CALL_C(0x5bbe, ecom_getAdjacentWallTableOffset_b0d_hook, 0x4253, 0x5bc1);
  CYC(0x5bc1, 0x5bc2); H = D;
  CYC(0x5bc2, 0x5bc4); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5bc4, 0x5bc5); B = mem_rd(gb, HL);
  CYC(0x5bc5, 0x5bc7); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5bc7, 0x5bc8); C = mem_rd(gb, HL);
  CYC(0x5bc8, 0x5bcb); SET_HL(0x425e); // ecom_sideviewAdjacentWallOffsetTable
  CYC(0x5bcb, 0x5bcc); fish_addAToHl_from_rst(gb, 0x5bcc);
  CYC(0x5bcc, 0x5bce); A = 0x10;
  CYC(0x5bce, 0x5bd0); H8(hFF8B) = A;
  CYC(0x5bd0, 0x5bd2); D = 0xcf; // >wRoomLayout

scanWall:
  CYC(0x5bd2, 0x5bd3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x5bd3, 0x5bd4); alu_add(gb, B);
  CYC(0x5bd4, 0x5bd5); B = A;
  CYC(0x5bd5, 0x5bd7); alu_and(gb, 0xf0);
  CYC(0x5bd7, 0x5bd8); E = A;
  CYC(0x5bd8, 0x5bd9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x5bd9, 0x5bda); alu_add(gb, C);
  CYC(0x5bda, 0x5bdb); C = A;
  CYC(0x5bdb, 0x5bdd); alu_and(gb, 0xf0);
  CYC(0x5bdd, 0x5bdf); A = alu_swap(gb, A);
  CYC(0x5bdf, 0x5be0); alu_or(gb, E);
  CYC(0x5be0, 0x5be1); E = A;
  CYC(0x5be1, 0x5be2); A = mem_rd(gb, DE);
  CYC(0x5be2, 0x5be4); alu_sub(gb, 0xf9); // TILEINDEX_PUDDLE
  CYC(0x5be4, 0x5be6); alu_cp(gb, 0x05); // TILEINDEX_FD-TILEINDEX_PUDDLE+1
  CYC(0x5be6, 0x5be8); A = H8(hFF8B);
  CYC(0x5be8, 0x5be9); alu_rla(gb);
  CYC(0x5be9, 0x5beb); H8(hFF8B) = A;
  if (!(F & FC)) { CYCT(0x5beb, 0x5bed); goto scanWall; } // jr nc
  CYC(0x5beb, 0x5bed);
  CYC(0x5bed, 0x5bef); alu_xor(gb, 0x0f);
  CYC(0x5bef, 0x5bf1); H8(hFF8B) = A;
  CYC(0x5bf1, 0x5bf3); A = H8(hActiveObject);
  CYC(0x5bf3, 0x5bf4); D = A;
  RET(0x5bf4); return; // ret
}
