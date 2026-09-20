#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void zol_state_uninitialized_hook(GB *gb);
void zol_state_stub_hook(GB *gb);
void zol_subid00_hook(GB *gb);
void zol_subid00_state8_hook(GB *gb);
void zol_subid00_state9_hook(GB *gb);
void zol_subid00_stateA_hook(GB *gb);
void zol_animate_hook(GB *gb);
void zol_subid00_stateB_hook(GB *gb);
void zol_subid00_stateC_hook(GB *gb);
void zol_subid00_stateD_hook(GB *gb);
void zol_subid01_hook(GB *gb);
void zol_subid01_state8_hook(GB *gb);
void zol_subid01_state9_hook(GB *gb);
void zol_animate2_hook(GB *gb);
void zol_subid01_stateA_hook(GB *gb);
void zol_subid01_stateB_hook(GB *gb);
void zol_subid01_stateC_hook(GB *gb);
void zol_subid01_stateD_hook(GB *gb);
void zol_spawnGel_hook(GB *gb);

static uint16_t zol_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_ZOL
//
// Variables:
//   var30: 1 when the zol is out of the ground, 0 otherwise. (only for subid 0, and only
//          used to prevent the "jump" sound effect from playing more than once.)
// ==================================================================================================
void enemyCode34_hook(GB *gb) {
  BASE(enemyCode34);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazardsNoAnimationForHoles_b0e_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+19); return; } // ret z
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { CYCT(b_+25, b_+27); goto normalStatus; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+29, b_+31); alu_sub(gb, 0x01); // ITEMCOLLISION_L1_SHIELD
  CYC(b_+31, b_+33); alu_cp(gb, 0x03); // ITEMCOLLISION_L3_SHIELD - ITEMCOLLISION_L1_SHIELD + 1
  if (F & FC) { RET_TAKEN(b_+33); return; } // ret c
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+36, b_+38); A = 0x0c;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  RET(b_+39); return; // ret

normalStatus:
  CALL_C(b_+40, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+43);
  if (!(F & FC)) { CYCT(b_+43, b_+45); goto normalState; } // jr nc
  CYC(b_+43, b_+45);
  {
    CYC(b_+45, b_+46); push_effect(gb, b_+46);
    uint16_t target = zol_jump_table(gb);
    if (target == SYM(zol_state_uninitialized)) { zol_state_uninitialized_hook(gb); return; }
    if (target == SYM(zol_state_stub)) { zol_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+62, b_+63); A = B;
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = zol_jump_table(gb);
    if (target == SYM(zol_subid00)) { zol_subid00_hook(gb); return; }
    if (target == SYM(zol_subid01)) { zol_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

void zol_state_uninitialized_hook(GB *gb) {
  BASE(zol_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  CYC(b_+2, b_+4); A = 0x1e; // SPEED_c0
  if (F & FZ) { CYCT(b_+4, b_+7); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp z
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x18);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+16, b_+18); A = 0x04;
  CALL_C(b_+18, enemySetAnimation_hook, SYM(enemySetAnimation), b_+21);
  CYC(b_+21, b_+24); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

void zol_state_stub_hook(GB *gb) {
  BASE(zol_state_stub);
  RET(b_+0); return; // ret
}

void zol_subid00_hook(GB *gb) {
  BASE(zol_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = zol_jump_table(gb);
    if (target == SYM(zol_subid00_state8)) { zol_subid00_state8_hook(gb); return; }
    if (target == SYM(zol_subid00_state9)) { zol_subid00_state9_hook(gb); return; }
    if (target == SYM(zol_subid00_stateA)) { zol_subid00_stateA_hook(gb); return; }
    if (target == SYM(zol_subid00_stateB)) { zol_subid00_stateB_hook(gb); return; }
    if (target == SYM(zol_subid00_stateC)) { zol_subid00_stateC_hook(gb); return; }
    if (target == SYM(zol_subid00_stateD)) { zol_subid00_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Hiding in ground, waiting for Link to approach
void zol_subid00_state8_hook(GB *gb) {
  BASE(zol_subid00_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x28;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_BC(0xfe00);
  CALL_C(b_+9, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x04);
  CYC(b_+19, b_+22); objectSetVisiblec2_hook(gb); return; // jp
}

// Jumping out of ground
void zol_subid00_state9_hook(GB *gb) {
  BASE(zol_subid00_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); zol_animate_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+9, b_+10); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_49c3; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x01);
  CYC(b_+14, b_+16); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+16, playSound_b00_hook, SYM(playSound_b00), b_+19);

L_49c3:
  CYC(b_+19, b_+21); C = 0x28;
  CALL_C(b_+21, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+24);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);
  CALL_C(b_+25, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+28);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x30);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+34, b_+36); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+36, b_+37); A = alu_inc8(gb, A);
  CYC(b_+37, b_+40); enemySetAnimation_hook(gb); return; // jp
}

// Holding still for [counter1] frames, preparing to hop toward Link
void zol_subid00_stateA_hook(GB *gb) {
  BASE(zol_subid00_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+6, b_+9); SET_BC(0xfe00);
  CALL_C(b_+9, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+12);
  CALL_C(b_+12, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+15);
  CYC(b_+15, b_+17); A = 0x02;
  CALL_C(b_+17, enemySetAnimation_hook, SYM(enemySetAnimation), b_+20);
  CYC(b_+20, b_+22); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), SYM(zol_animate));
  zol_animate_hook(gb); return; // fallthrough
}

void zol_animate_hook(GB *gb) {
  BASE(zol_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Hopping toward Link
void zol_subid00_stateB_hook(GB *gb) {
  BASE(zol_subid00_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemy_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0e), b_+3);
  CYC(b_+3, b_+5); C = 0x28;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x30);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  CYC(b_+16, b_+18); A = 0x0a;
  CYC(b_+18, b_+20); B = 0x01;
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_4a12; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+24, b_+26); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+26, b_+28); A = 0x0c;
  CYC(b_+28, b_+30); B = 0x03;

L_4a12:
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+37); enemySetAnimation_hook(gb); return; // jp
}

// Disappearing into the ground
void zol_subid00_stateC_hook(GB *gb) {
  BASE(zol_subid00_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); zol_animate_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x28);
  CYC(b_+13, b_+15); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CALL_C(b_+17, enemySetAnimation_hook, SYM(enemySetAnimation), b_+20);
  CYC(b_+20, b_+23); objectSetInvisible_hook(gb); return; // jp
}

// Fully disappeared into ground. Wait [counter1] frames before we can emerge again
void zol_subid00_stateD_hook(GB *gb) {
  BASE(zol_subid00_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); enemySetAnimation_hook(gb); return; // jp
}

void zol_subid01_hook(GB *gb) {
  BASE(zol_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = zol_jump_table(gb);
    if (target == SYM(zol_subid01_state8)) { zol_subid01_state8_hook(gb); return; }
    if (target == SYM(zol_subid01_state9)) { zol_subid01_state9_hook(gb); return; }
    if (target == SYM(zol_subid01_stateA)) { zol_subid01_stateA_hook(gb); return; }
    if (target == SYM(zol_subid01_stateB)) { zol_subid01_stateB_hook(gb); return; }
    if (target == SYM(zol_subid01_stateC)) { zol_subid01_stateC_hook(gb); return; }
    if (target == SYM(zol_subid01_stateD)) { zol_subid01_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Holding still for [counter1] frames before deciding whether to hop or move forward
void zol_subid01_state8_hook(GB *gb) {
  BASE(zol_subid01_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); zol_animate2_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x07);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  if (F & FZ) { CYCT(b_+13, b_+15); goto hopTowardLink; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(b_+24, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+27);
  CYCT(b_+27, b_+29); zol_animate2_hook(gb); return; // jr

hopTowardLink:
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x20); // [counter1]
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x0a);
  CYC(b_+35, b_+37); A = 0x05;
  CYC(b_+37, b_+40); enemySetAnimation_hook(gb); return; // jp
}

// Sliding toward Link
void zol_subid01_state9_hook(GB *gb) {
  BASE(zol_subid01_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+3);
  CALL_C(b_+3, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); zol_animate2_hook(gb); return; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x18); // [counter1]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  zol_animate2_hook(gb); return; // fallthrough
}

void zol_animate2_hook(GB *gb) {
  BASE(zol_animate2);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Shaking before hopping toward Link
void zol_subid01_stateA_hook(GB *gb) {
  BASE(zol_subid01_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); zol_animate2_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xfe);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+19, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+22);
  CYC(b_+22, b_+24); A = 0x02;
  CALL_C(b_+24, enemySetAnimation_hook, SYM(enemySetAnimation), b_+27);
  CYC(b_+27, b_+29); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+29, playSound_b00_hook, SYM(playSound_b00), b_+32);
  CYC(b_+32, b_+35); objectSetVisiblec1_hook(gb); return; // jp
}

// Hopping toward Link
void zol_subid01_stateB_hook(GB *gb) {
  BASE(zol_subid01_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemy_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0e), b_+3);
  CYC(b_+3, b_+5); C = 0x28;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x18);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x08);
  CYC(b_+18, b_+20); A = 0x04;
  CALL_C(b_+20, enemySetAnimation_hook, SYM(enemySetAnimation), b_+23);
  CYC(b_+23, b_+26); objectSetVisiblec2_hook(gb); return; // jp
}

// Zol has been attacked, create puff, disable collisions, prepare to spawn two gels in the
// zol's place.
void zol_subid01_stateC_hook(GB *gb) {
  BASE(zol_subid01_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x08; // INTERAC_KILLENEMYPUFF
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 18);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+17, b_+19); A = 0x73; // SND_KILLENEMY
  CALL_C(b_+19, playSound_b00_hook, SYM(playSound_b00), b_+22);
  CYC(b_+22, b_+25); objectSetInvisible_hook(gb); return; // jp
}

// Zol has been attacked, spawn gels after [counter2] frames
void zol_subid01_stateD_hook(GB *gb) {
  BASE(zol_subid01_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); C = 0x04;
  CALL_C(b_+6, zol_spawnGel_hook, SYM(zol_spawnGel), b_+9);
  CYC(b_+9, b_+11); C = 0xfc;
  CALL_C(b_+11, zol_spawnGel_hook, SYM(zol_spawnGel), b_+14);
  CALL_C(b_+14, decNumEnemies_hook, SYM(decNumEnemies), b_+17);
  CYC(b_+17, b_+20); enemyDelete_hook(gb); return; // jp
}

// @param  c  X offset
void zol_spawnGel_hook(GB *gb) {
  BASE(zol_spawnGel);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x43; // ENEMY_GEL
  CALL_C(b_+2, ecom_spawnEnemyWithSubid01_b0e_hook, SYM(ecom_spawnEnemyWithSubid01_b0e), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); // [child.subid] = 0
  CYC(b_+7, b_+9); B = 0x00;
  CALL_C(b_+9, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+19, b_+20); E = L;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  RET(b_+22); return; // ret
}
