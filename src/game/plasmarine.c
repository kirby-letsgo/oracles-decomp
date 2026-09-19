#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void enemyCode7e_hook(GB *gb);
void plasmarine_state_uninitialized_hook(GB *gb);
void plasmarine_state_switchHook_hook(GB *gb);
void plasmarine_state_stub_hook(GB *gb);
void plasmarine_state8_hook(GB *gb);
void plasmarine_state9_hook(GB *gb);
void plasmarine_stateA_hook(GB *gb);
void plasmarine_animate_hook(GB *gb);
void plasmarine_stateB_hook(GB *gb);
void plasmarine_stateC_hook(GB *gb);
void plasmarine_stateD_hook(GB *gb);
void plasmarine_stateE_hook(GB *gb);
void plasmarine_stateF_hook(GB *gb);
void plasmarine_decideNumberOfShockAttacks_hook(GB *gb);
void plasmarine_checkCloseToTargetPosition_hook(GB *gb);

static uint16_t plasmarine_jump_table(GB *gb) {
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

static void plasmarine_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// plasmarine_state_switchHook@swapColor: reached by fallthrough from @afterSwitch
// (within plasmarine_state_switchHook_hook), and also by a static `call nc` from
// enemyCode7e itself (NOT HOOKED). Ends at plasmarine_state_switchHook@beforeSwitch's
// `ret`, which is also a jump-table target of plasmarine_state_switchHook.
static void plasmarine_swapColor(GB *gb, uint16_t sp0_) {
  CYC(0x7ae0, 0x7ae1); H = D;
  CYC(0x7ae1, 0x7ae3); L = ENEMY_BASE + 0x32; // var32
  CYC(0x7ae3, 0x7ae4); A = mem_rd(gb, HL);
  CYC(0x7ae4, 0x7ae6); alu_xor(gb, 0x01);
  CYC(0x7ae6, 0x7ae7); mem_wr(gb, HL, A);
  CYC(0x7ae7, 0x7ae8); A = alu_inc8(gb, A);
  CYC(0x7ae8, 0x7aea); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7aea, 0x7aeb); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7aeb, 0x7aec); mem_wr(gb, HL, A);
  RET(0x7aec); return; // ret
}

// ==================================================================================================
// ENEMY_PLASMARINE
//
// Variables:
//   counter2: Number of times to do shock attack before firing projectiles
//   var30/var31: Target position?
//   var32: Color (0 for blue, 1 for red)
//   var33: ?
//   var34: Number of projectiles to fire in one attack
// ==================================================================================================
void enemyCode7e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x7a5c, 0x7a5e); goto normalStatus; } // jr z
  CYC(0x7a5c, 0x7a5e);
  CYC(0x7a5e, 0x7a60); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x7a60); return; } // ret c
  CYC(0x7a60, 0x7a61);
  if (F & FZ) { CYCT(0x7a61, 0x7a64); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(0x7a61, 0x7a64);
  CYC(0x7a64, 0x7a66); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x7a66, 0x7a67); A = mem_rd(gb, DE);
  CYC(0x7a67, 0x7a69); alu_cp(gb, 0x68); // ENEMYCOLLISION_PLASMARINE_SHOCK
  if (F & FZ) { CYCT(0x7a69, 0x7a6b); goto normalStatus; } // jr z
  CYC(0x7a69, 0x7a6b);
  CYC(0x7a6b, 0x7a6d); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x7a6d, 0x7a6e); A = mem_rd(gb, DE);
  CYC(0x7a6e, 0x7a70); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x7a70, 0x7a72); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (!(F & FC)) { CYCT(0x7a72, 0x7a75); push_effect(gb, 0x7a75); plasmarine_swapColor(gb, sp0_); } // call nc
  else { CYC(0x7a72, 0x7a75); }

normalStatus:
  CYC(0x7a75, 0x7a77); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7a77, 0x7a78); A = mem_rd(gb, DE);
  CYC(0x7a78, 0x7a79); push_effect(gb, 0x7a79);
  {
    uint16_t target = plasmarine_jump_table(gb);
    if (target == 0x7a99) { plasmarine_state_uninitialized_hook(gb); return; }
    if (target == 0x7afa) { plasmarine_state_stub_hook(gb); return; }
    if (target == 0x7abf) { plasmarine_state_switchHook_hook(gb); return; }
    if (target == 0x7afb) { plasmarine_state8_hook(gb); return; }
    if (target == 0x7b20) { plasmarine_state9_hook(gb); return; }
    if (target == 0x7b37) { plasmarine_stateA_hook(gb); return; }
    if (target == 0x7b58) { plasmarine_stateB_hook(gb); return; }
    if (target == 0x7b65) { plasmarine_stateC_hook(gb); return; }
    if (target == 0x7ba7) { plasmarine_stateD_hook(gb); return; }
    if (target == 0x7be6) { plasmarine_stateE_hook(gb); return; }
    if (target == 0x7c1a) { plasmarine_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

void plasmarine_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a99, 0x7a9b); A = 0x64; // SPEED_280
  CALL_C(0x7a9b, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x7a9e);
  CYC(0x7a9e, 0x7aa0); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7aa0, 0x7aa2); mem_wr(gb, HL, 0x08);
  CYC(0x7aa2, 0x7aa4); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7aa4, 0x7aa6); mem_wr(gb, HL, 0x04);
  CYC(0x7aa6, 0x7aa8); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7aa8, 0x7aaa); mem_wr(gb, HL, 0x58);
  CYC(0x7aaa, 0x7aab); L = alu_inc8(gb, L);
  CYC(0x7aab, 0x7aad); mem_wr(gb, HL, 0x78);
  CYC(0x7aad, 0x7aaf); A = 0x01;
  CYC(0x7aaf, 0x7ab2); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7ab2, 0x7ab5); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7ab5, 0x7ab7); A = 0x7e; // ENEMY_PLASMARINE
  CYC(0x7ab7, 0x7ab9); B = 0x00;
  CALL_C(0x7ab9, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x7abc);
  CYC(0x7abc, 0x7abf); objectSetVisible83_hook(gb); return; // jp
}

void plasmarine_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7abf, 0x7ac0); E = alu_inc8(gb, E);
  CYC(0x7ac0, 0x7ac1); A = mem_rd(gb, DE);
  CYC(0x7ac1, 0x7ac2); push_effect(gb, 0x7ac2);
  {
    uint16_t target = plasmarine_jump_table(gb);
    if (target == 0x7aca) goto justLatched;
    if (target == 0x7aec) { RET(0x7aec); return; } // beforeSwitch
    if (target == 0x7ad4) goto afterSwitch;
    if (target == 0x7aed) goto released;
    HANDOFF(target);
  }

justLatched:
  CYC(0x7aca, 0x7acb); alu_xor(gb, A);
  CYC(0x7acb, 0x7acd); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7acd, 0x7ace); mem_wr(gb, DE, A);
  CALL_C(0x7ace, enemySetAnimation_hook, 0x282b, 0x7ad1);
  CYC(0x7ad1, 0x7ad4); ecom_incSubstate_b0f_hook(gb); return; // jp

afterSwitch:
  CYC(0x7ad4, 0x7ad6); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7ad6, 0x7ad7); A = mem_rd(gb, DE);
  CYC(0x7ad7, 0x7ad8); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7ad8); return; } // ret nz
  CYC(0x7ad8, 0x7ad9);
  CYC(0x7ad9, 0x7ada); A = alu_inc8(gb, A);
  CYC(0x7ada, 0x7adb); mem_wr(gb, DE, A);
  CYC(0x7adb, 0x7add); A = 0x7b; // SND_MYSTERY_SEED
  CALL_C(0x7add, playSound_b00_hook, 0x0c98, 0x7ae0);
  plasmarine_swapColor(gb, sp0_); return; // fallthrough

released:
  CYC(0x7aed, 0x7aef); B = 0x0a;
  CALL_C(0x7aef, ecom_fallToGroundAndSetState_b0f_hook, 0x44e2, 0x7af2);
  if (!(F & FZ)) { RET_TAKEN(0x7af2); return; } // ret nz
  CYC(0x7af2, 0x7af3);
  CYC(0x7af3, 0x7af5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7af5, 0x7af7); mem_wr(gb, HL, 60);
  CYC(0x7af7, 0x7afa); plasmarine_decideNumberOfShockAttacks_hook(gb); return; // jp
}

void plasmarine_state_stub_hook(GB *gb) {
  RET(0x7afa); return; // ret
}

// Moving toward centre of room before starting fight
void plasmarine_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7afb, plasmarine_checkCloseToTargetPosition_hook, 0x7c3c, 0x7afe);
  if (F & FC) { CYCT(0x7afe, 0x7b00); goto reachedTarget; } // jr c
  CYC(0x7afe, 0x7b00);
  CALL_C(0x7b00, ecom_decCounter1_b0f_hook, 0x439a, 0x7b03);
  if (!(F & FZ)) { CYCT(0x7b03, 0x7b05); goto L_7b0d; } // jr nz
  CYC(0x7b03, 0x7b05);
  CYC(0x7b05, 0x7b07); mem_wr(gb, HL, 0x04);
  CALL_C(0x7b07, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x7b0a);
  CALL_C(0x7b0a, objectNudgeAngleTowards_hook, 0x1fd4, 0x7b0d);

L_7b0d:
  CALL_C(0x7b0d, objectApplySpeed_hook, 0x201d, 0x7b10);
  CYCT(0x7b10, 0x7b12); plasmarine_animate_hook(gb); return; // jr

reachedTarget:
  CYC(0x7b12, 0x7b13); L = E;
  CYC(0x7b13, 0x7b14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x7b14, 0x7b16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7b16, 0x7b18); mem_wr(gb, HL, 60);
  CYC(0x7b18, 0x7b1a); L = ENEMY_BASE + OBJ_YH;
  CYC(0x7b1a, 0x7b1b); mem_wr(gb, HL, B);
  CYC(0x7b1b, 0x7b1d); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7b1d, 0x7b1e); mem_wr(gb, HL, C);
  CYCT(0x7b1e, 0x7b20); plasmarine_animate_hook(gb); return; // jr
}

// 60 frame delay before starting fight
void plasmarine_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b20, ecom_decCounter1_b0f_hook, 0x439a, 0x7b23);
  if (!(F & FZ)) { CYCT(0x7b23, 0x7b25); plasmarine_animate_hook(gb); return; } // jr nz
  CYC(0x7b23, 0x7b25);
  CYC(0x7b25, 0x7b27); mem_wr(gb, HL, 60); // [counter1]
  CYC(0x7b27, 0x7b28); L = E;
  CYC(0x7b28, 0x7b29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x7b29, 0x7b2b); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7b2b, 0x7b2d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(0x7b2d, plasmarine_decideNumberOfShockAttacks_hook, 0x7c32, 0x7b30);
  CALL_C(0x7b30, enemyBoss_beginBoss_b0f_hook, 0x4584, 0x7b33);
  CYC(0x7b33, 0x7b34); alu_xor(gb, A);
  CYC(0x7b34, 0x7b37); enemySetAnimation_hook(gb); return; // jp
}

// Standing in place before charging
void plasmarine_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b37, ecom_decCounter1_b0f_hook, 0x439a, 0x7b3a);
  if (!(F & FZ)) { CYCT(0x7b3a, 0x7b3c); plasmarine_animate_hook(gb); return; } // jr nz
  CYC(0x7b3a, 0x7b3c);
  CYC(0x7b3c, 0x7b3d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(0x7b3d, 0x7b3f); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7b3f, 0x7b41); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7b41, 0x7b43); plasmarine_animate_hook(gb); return; } // jr z
  CYC(0x7b41, 0x7b43);
  CYC(0x7b43, 0x7b45); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7b45, 0x7b47); mem_wr(gb, HL, 0x0c);
  CYC(0x7b47, 0x7b48); L = E;
  CYC(0x7b48, 0x7b49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  CYC(0x7b49, 0x7b4b); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7b4b, 0x7b4d); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(0x7b4d, 0x7b4f); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7b4f, 0x7b51); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x7b51, 0x7b52); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7b52, 0x7b54); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x7b54, 0x7b55); mem_wr(gb, HL, A);
  plasmarine_animate_hook(gb); return; // fallthrough
}

void plasmarine_animate_hook(GB *gb) {
  CYC(0x7b55, 0x7b58); enemyAnimate_hook(gb); return; // jp
}

// Charging toward Link
void plasmarine_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b58, ecom_decCounter1_b0f_hook, 0x439a, 0x7b5b);
  if (!(F & FZ)) { CYCT(0x7b5b, 0x7b5d); goto L_7b5f; } // jr nz
  CYC(0x7b5b, 0x7b5d);
  CYC(0x7b5d, 0x7b5e); L = E;
  CYC(0x7b5e, 0x7b5f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c

L_7b5f:
  CYC(0x7b5f, 0x7b61); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7b61, 0x7b62); A = mem_rd(gb, HL);
  CYC(0x7b62, 0x7b64); alu_sub(gb, 0x05); // SPEED_20
  CYC(0x7b64, 0x7b65); mem_wr(gb, HL, A);
  plasmarine_stateC_hook(gb); return; // fallthrough
}

void plasmarine_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b65, plasmarine_checkCloseToTargetPosition_hook, 0x7c3c, 0x7b68);
  if (!(F & FC)) { CYCT(0x7b68, 0x7b6b); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nc
  CYC(0x7b68, 0x7b6b);
  CYC(0x7b6b, 0x7b6d); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7b6d, 0x7b6e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7b6e, 0x7b6f); L = E;
  if (F & FZ) { CYCT(0x7b6f, 0x7b71); goto fireProjectiles; } // jr z
  CYC(0x7b6f, 0x7b71);
  CYC(0x7b71, 0x7b73); mem_wr(gb, HL, 0x0d); // [state]
  CYC(0x7b73, 0x7b75); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7b75, 0x7b77); mem_wr(gb, HL, 65);
  CYC(0x7b77, 0x7b79); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(0x7b79, 0x7b7b); mem_wr(gb, HL, (uint8_t)-8);
  CYC(0x7b7b, 0x7b7d); L = ENEMY_BASE + 0x32; // var32
  CYC(0x7b7d, 0x7b7e); A = mem_rd(gb, HL);
  CYC(0x7b7e, 0x7b80); alu_add(gb, 0x04);
  CYC(0x7b80, 0x7b82); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7b82, 0x7b83); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7b83, 0x7b84); mem_wr(gb, HL, A);
  CYC(0x7b84, 0x7b86); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x7b86, 0x7b88); mem_wr(gb, HL, 0x68); // ENEMYCOLLISION_PLASMARINE_SHOCK
  CYC(0x7b88, 0x7b8a); A = 0x02;
  CYC(0x7b8a, 0x7b8d); enemySetAnimation_hook(gb); return; // jp

fireProjectiles:
  CYC(0x7b8d, 0x7b8f); mem_wr(gb, HL, 0x0e); // [state]
  CYC(0x7b8f, 0x7b91); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7b91, 0x7b92); A = mem_rd(gb, HL);
  CYC(0x7b92, 0x7b93); A = alu_dec8(gb, A);
  CYC(0x7b93, 0x7b96); SET_HL(0x7ba0); // @numProjectilesToFire
  CYC(0x7b96, 0x7b97); plasmarine_addAToHl_from_rst(gb, 0x7b97);
  CYC(0x7b97, 0x7b99); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7b99, 0x7b9a); A = mem_rd(gb, HL);
  CYC(0x7b9a, 0x7b9b); mem_wr(gb, DE, A);
  CYC(0x7b9b, 0x7b9d); A = 0x01;
  CYC(0x7b9d, 0x7ba0); enemySetAnimation_hook(gb); return; // jp
}

// Shock attack
void plasmarine_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7ba7, ecom_decCounter1_b0f_hook, 0x439a, 0x7baa);
  if (F & FZ) { CYCT(0x7baa, 0x7bac); goto doneAttack; } // jr z
  CYC(0x7baa, 0x7bac);
  CYC(0x7bac, 0x7bad); A = mem_rd(gb, HL);
  CYC(0x7bad, 0x7baf); alu_and(gb, 0x0f);
  CYC(0x7baf, 0x7bb1); A = 0xac; // SND_SHOCK
  if (F & FZ) { CALL_C_CC(0x7bb1, playSound_b00_hook, 0x0c98, 0x7bb4); } else { CYC(0x7bb1, 0x7bb4); } // call z
  CYC(0x7bb4, 0x7bb6); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7bb6, 0x7bb7); A = mem_rd(gb, DE);
  CYC(0x7bb7, 0x7bb8); alu_or(gb, A);
  CYC(0x7bb8, 0x7bba); B = 0x04;
  if (F & FZ) { CYCT(0x7bba, 0x7bbc); goto L_7bbe; } // jr z
  CYC(0x7bba, 0x7bbc);
  CYC(0x7bbc, 0x7bbe); B = 0x01;

L_7bbe:
  CYC(0x7bbe, 0x7bc0); E = ENEMY_BASE + 0x32; // var32
  CYC(0x7bc0, 0x7bc1); A = mem_rd(gb, DE);
  CYC(0x7bc1, 0x7bc2); alu_add(gb, B);
  CYC(0x7bc2, 0x7bc3); H = D;
  CYC(0x7bc3, 0x7bc5); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7bc5, 0x7bc6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7bc6, 0x7bc7); mem_wr(gb, HL, A);
  CYCT(0x7bc7, 0x7bc9); plasmarine_animate_hook(gb); return; // jr

doneAttack:
  CYC(0x7bc9, 0x7bcb); mem_wr(gb, HL, 60); // [counter1]
  CYC(0x7bcb, 0x7bcc); L = E;
  CYC(0x7bcc, 0x7bce); mem_wr(gb, HL, 0x0a); // [state]
  CYC(0x7bce, 0x7bd0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7bd0, 0x7bd2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x7bd2, 0x7bd4); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(0x7bd4, 0x7bd6); mem_wr(gb, HL, (uint8_t)-4);
  CYC(0x7bd6, 0x7bd8); L = ENEMY_BASE + 0x32; // var32
  CYC(0x7bd8, 0x7bd9); A = mem_rd(gb, HL);
  CYC(0x7bd9, 0x7bda); A = alu_inc8(gb, A);
  CYC(0x7bda, 0x7bdc); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7bdc, 0x7bdd); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7bdd, 0x7bde); mem_wr(gb, HL, A);
  CYC(0x7bde, 0x7be0); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x7be0, 0x7be2); mem_wr(gb, HL, 0x4f); // ENEMYCOLLISION_PLASMARINE
  CYC(0x7be2, 0x7be3); alu_xor(gb, A);
  CYC(0x7be3, 0x7be6); enemySetAnimation_hook(gb); return; // jp
}

// Firing projectiles
void plasmarine_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7be6, enemyAnimate_hook, 0x2818, 0x7be9);
  CYC(0x7be9, 0x7beb); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7beb, 0x7bec); A = mem_rd(gb, DE);
  CYC(0x7bec, 0x7bed); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7bed, 0x7bef); goto fire; } // jr z
  CYC(0x7bed, 0x7bef);
  CYC(0x7bef, 0x7bf0); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(0x7bf0); return; } // ret z
  CYC(0x7bf0, 0x7bf1);
  CALL_C(0x7bf1, ecom_incState_b0f_hook, 0x4000, 0x7bf4);
  CYC(0x7bf4, 0x7bf6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7bf6, 0x7bf8); mem_wr(gb, HL, 60);
  CYC(0x7bf8, 0x7bf9); alu_xor(gb, A);
  CYC(0x7bf9, 0x7bfc); enemySetAnimation_hook(gb); return; // jp

fire:
  CYC(0x7bfc, 0x7bfd); mem_wr(gb, DE, A); // [animParameter] = 0
  CALL_C(0x7bfd, getFreePartSlot_hook, 0x3e8e, 0x7c00);
  if (!(F & FZ)) { RET_TAKEN(0x7c00); return; } // ret nz
  CYC(0x7c00, 0x7c01);
  CYC(0x7c01, 0x7c03); mem_wr(gb, HL, 0x43); // PART_PLASMARINE_PROJECTILE
  CYC(0x7c03, 0x7c04); L = alu_inc8(gb, L);
  CYC(0x7c04, 0x7c06); E = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(0x7c06, 0x7c07); A = mem_rd(gb, DE);
  CYC(0x7c07, 0x7c08); A = alu_dec8(gb, A);
  CYC(0x7c08, 0x7c09); mem_wr(gb, HL, A); // [projectile.var03]
  CYC(0x7c09, 0x7c0b); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(0x7c0b, 0x7c0c); mem_wr(gb, HL, D);
  CYC(0x7c0c, 0x7c0d); L = alu_dec8(gb, L);
  CYC(0x7c0d, 0x7c0f); mem_wr(gb, HL, 0x80); // Enemy.start
  CYC(0x7c0f, 0x7c12); SET_BC(0xec00);
  CALL_C(0x7c12, objectCopyPositionWithOffset_hook, 0x225a, 0x7c15);
  CYC(0x7c15, 0x7c17); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CYC(0x7c17, 0x7c1a); playSound_b00_hook(gb); return; // jp
}

// Decides whether to return to state $0e (fire another projectile) or charge at Link again
void plasmarine_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c1a, ecom_decCounter1_b0f_hook, 0x439a, 0x7c1d);
  if (!(F & FZ)) { CYCT(0x7c1d, 0x7c20); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x7c1d, 0x7c20);
  CYC(0x7c20, 0x7c22); L = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7c22, 0x7c23); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7c23, 0x7c24); L = E;
  if (F & FZ) { CYCT(0x7c24, 0x7c26); goto chargeAtLink; } // jr z
  CYC(0x7c24, 0x7c26);
  CYC(0x7c26, 0x7c27); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = $0e
  CYC(0x7c27, 0x7c29); A = 0x01;
  CYC(0x7c29, 0x7c2c); enemySetAnimation_hook(gb); return; // jp

chargeAtLink:
  CYC(0x7c2c, 0x7c2e); mem_wr(gb, HL, 0x0a);
  CYC(0x7c2e, 0x7c30); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7c30, 0x7c32); mem_wr(gb, HL, 30);
  plasmarine_decideNumberOfShockAttacks_hook(gb); return; // fallthrough
}

void plasmarine_decideNumberOfShockAttacks_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c32, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7c35);
  CYC(0x7c35, 0x7c37); alu_and(gb, 0x01);
  CYC(0x7c37, 0x7c38); A = alu_inc8(gb, A);
  CYC(0x7c38, 0x7c3a); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7c3a, 0x7c3b); mem_wr(gb, DE, A);
  RET(0x7c3b); return; // ret
}

// @param[out] cflag c if close enough to target position
void plasmarine_checkCloseToTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c3c, 0x7c3d); H = D;
  CYC(0x7c3d, 0x7c3f); L = ENEMY_BASE + 0x30; // var30
  CALL_C(0x7c3f, ecom_readPositionVars_b0f_hook, 0x4439, 0x7c42);
  CYC(0x7c42, 0x7c43); alu_sub(gb, C);
  CYC(0x7c43, 0x7c45); alu_add(gb, 0x04);
  CYC(0x7c45, 0x7c47); alu_cp(gb, 0x09);
  if (!(F & FC)) { RET_TAKEN(0x7c47); return; } // ret nc
  CYC(0x7c47, 0x7c48);
  CYC(0x7c48, 0x7c4a); A = hram_rd(gb, 0x8f); // hFF8F
  CYC(0x7c4a, 0x7c4b); alu_sub(gb, B);
  CYC(0x7c4b, 0x7c4d); alu_add(gb, 0x04);
  CYC(0x7c4d, 0x7c4f); alu_cp(gb, 0x09);
  RET(0x7c4f); return; // ret
}
