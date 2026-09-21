#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void stalfos_state_uninitialized_hook(GB *gb);
void stalfos_state_switchHook_hook(GB *gb);
void stalfos_state_stub_hook(GB *gb);
void stalfos_state08_hook(GB *gb);
void stalfos_state09_hook(GB *gb);
void stalfos_state0a_hook(GB *gb);
void stalfos_state0b_hook(GB *gb);
void stalfos_state0c_hook(GB *gb);
void stalfos_state0d_hook(GB *gb);
void stalfos_state0e_hook(GB *gb);
void stalfos_state0f_hook(GB *gb);
void stalfos_state10_hook(GB *gb);
void stalfos_moveInRandomAngle_hook(GB *gb);
void stalfos_checkSubid3StompsLink_hook(GB *gb);
void stalfos_beginJumpAnimation_hook(GB *gb);
void stalfos_checkJumpAwayFromLink_hook(GB *gb);

static uint16_t stalfos_jump_table(GB *gb) {
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
// ENEMY_STALFOS
// ==================================================================================================
void enemyCode31_hook(GB *gb) {
  BASE(enemyCode31);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  RET(b_+15); return; // ret

normalStatus:
  CALL_C(b_+16, stalfos_checkJumpAwayFromLink_hook, SYM(stalfos_checkJumpAwayFromLink), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  {
    CYC(b_+22, b_+23); push_effect(gb, b_+23);
    uint16_t target = stalfos_jump_table(gb);
    if (target == SYM(stalfos_state_uninitialized)) { stalfos_state_uninitialized_hook(gb); return; }
    if (target == SYM(stalfos_state_stub)) { stalfos_state_stub_hook(gb); return; }
    if (target == SYM(stalfos_state_switchHook)) { stalfos_state_switchHook_hook(gb); return; }
    if (target == SYM(stalfos_state08)) { stalfos_state08_hook(gb); return; }
    if (target == SYM(stalfos_state09)) { stalfos_state09_hook(gb); return; }
    if (target == SYM(stalfos_state0a)) { stalfos_state0a_hook(gb); return; }
    if (target == SYM(stalfos_state0b)) { stalfos_state0b_hook(gb); return; }
    if (target == SYM(stalfos_state0c)) { stalfos_state0c_hook(gb); return; }
    if (target == SYM(stalfos_state0d)) { stalfos_state0d_hook(gb); return; }
    if (target == SYM(stalfos_state0e)) { stalfos_state0e_hook(gb); return; }
    if (target == SYM(stalfos_state0f)) { stalfos_state0f_hook(gb); return; }
    if (target == SYM(stalfos_state10)) { stalfos_state10_hook(gb); return; }
    HANDOFF(target);
  }
}

void stalfos_state_uninitialized_hook(GB *gb) {
  BASE(stalfos_state_uninitialized);
  CYC(b_+0, b_+2); A = 0x14; // SPEED_80
  CYC(b_+2, b_+5); TAIL(ecom_setSpeedAndState8AndVisible_b0e); // jp
}

void stalfos_state_switchHook_hook(GB *gb) {
  BASE(stalfos_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = stalfos_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+11) goto substate1;
    if (target == SYM(ecom_fallToGroundAndSetState8_b0e)) { ecom_fallToGroundAndSetState8_b0e_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(b_+11); return; // ret
}

void stalfos_state_stub_hook(GB *gb) {
  BASE(stalfos_state_stub);
  RET(b_+0); return; // ret
}

// Choosing what to do next (move in a random direction, or shoot a bone at Link)
void stalfos_state08_hook(GB *gb) {
  BASE(stalfos_state08);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, stalfos_checkSubid3StompsLink_hook, SYM(stalfos_checkSubid3StompsLink), b_+3);
  CALL_C(b_+3, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); stalfos_moveInRandomAngle_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); stalfos_moveInRandomAngle_hook(gb); return; } // jp nz
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+23); A = 0x0c;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return; // ret
}

// Moving in some direction for [counter1] frames
void stalfos_state09_hook(GB *gb) {
  BASE(stalfos_state09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, stalfos_checkSubid3StompsLink_hook, SYM(stalfos_checkSubid3StompsLink), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_4635; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x08);

L_4635:
  CALL_C(b_+12, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), b_+15);
  CALL_C(b_+15, objectApplySpeed_hook, SYM(objectApplySpeed), b_+18);
  CYC(b_+18, b_+21); TAIL(enemyAnimate); // jp
}

// Just starting a jump away from Link
void stalfos_state0a_hook(GB *gb) {
  BASE(stalfos_state0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xfe00);
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x32); // SPEED_140
  CALL_C(b_+16, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0e), b_+19);
  CYC(b_+19, b_+22); TAIL(stalfos_beginJumpAnimation); // jp
}

// Jumping until hitting the ground
void stalfos_state0b_hook(GB *gb) {
  BASE(stalfos_state0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto hitGround; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_4663; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

L_4663:
  CYC(b_+15, b_+18); TAIL(ecom_applyVelocityForSideviewEnemy_b0e); // jp

hitGround:
  CYC(b_+18, b_+20); A = 0x14; // SPEED_80
  CALL_C(b_+20, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CALL_C(b_+24, enemySetAnimation_hook, SYM(enemySetAnimation), b_+27);
  CYC(b_+27, b_+30); TAIL(objectSetVisiblec2); // jp
}

// Firing a projectile, then immediately going to state 9 to keep moving
void stalfos_state0c_hook(GB *gb) {
  BASE(stalfos_state0c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x1c; // PART_STALFOS_BONE
  CALL_C(b_+2, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+5);
  CYCT(b_+5, b_+7); TAIL(stalfos_moveInRandomAngle); // jr
}

// Stomping on Link
void stalfos_state0d_hook(GB *gb) {
  BASE(stalfos_state0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(gb); return; } // jp nz
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x08);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+17); return; // ret
}

// Wait for 8 frames while hanging in the air mid-stomp
void stalfos_state0e_hook(GB *gb) {
  BASE(stalfos_state0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  RET(b_+6); return; // ret
}

// Fall down for the stomp
void stalfos_state0f_hook(GB *gb) {
  BASE(stalfos_state0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_add(gb, 0x03);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+9); alu_cp(gb, 0x80);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; } // ret nc
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); alu_xor(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); // [zh] = 0
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 30);
  CYC(b_+18, b_+21); TAIL(objectSetVisiblec2); // jp
}

// Laying on the ground for [counter1] frames until he starts moving again
void stalfos_state10_hook(GB *gb) {
  BASE(stalfos_state10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  TAIL(stalfos_moveInRandomAngle); // fallthrough
}

// Go to state 9 with a freshly chosen angle
void stalfos_moveInRandomAngle_hook(GB *gb) {
  BASE(stalfos_moveInRandomAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x30; // Enemy.var30
  CYC(b_+2, b_+5); SET_BC(0x1f0f);
  CALL_C(b_+5, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x09);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+17, b_+19); A = 0x20;
  CYC(b_+19, b_+20); alu_add(gb, E);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+24); C = alu_dec8(gb, C);
  CYC(b_+24, b_+25); A = B;
  if (F & FZ) CALL_C_CC(b_+25, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+28); else CYC(b_+25, b_+28); // call z
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+35); TAIL(enemySetAnimation); // jp
}

// For subid 3 only, if Link approaches close enough, it will jump toward Link to stomp on
// him (goes to state $0d).
void stalfos_checkSubid3StompsLink_hook(GB *gb) {
  BASE(stalfos_checkSubid3StompsLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); C = 0x1c;
  CALL_C(b_+8, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+11);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; } // ret nc
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_BC(0xfdc0);
  CALL_C(b_+15, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+18);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x0d);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+26, b_+27); pop_effect(gb); // pop hl (discard the caller's return address; tail-falls into stalfos_beginJumpAnimation)
  CALL_C(b_+27, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), SYM(stalfos_beginJumpAnimation));
  TAIL(stalfos_beginJumpAnimation); // fallthrough
}

void stalfos_beginJumpAnimation_hook(GB *gb) {
  BASE(stalfos_beginJumpAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, enemySetAnimation_hook, SYM(enemySetAnimation), b_+5);
  CYC(b_+5, b_+7); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  CYC(b_+10, b_+13); TAIL(objectSetVisiblec1); // jp
}

// If Link is swinging something near this object, it will set its state to $0a if not
// already jumping.
void stalfos_checkJumpAwayFromLink_hook(GB *gb) {
  BASE(stalfos_checkJumpAwayFromLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wLinkUsingItem1);
  CYC(b_+8, b_+10); alu_and(gb, 0xf0);
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0a);
  if (!(F & FC)) { RET_TAKEN(b_+16); return; } // ret nc
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); C = 0x2c;
  CALL_C(b_+19, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+22);
  if (!(F & FC)) { RET_TAKEN(b_+22); return; } // ret nc
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+27); A = 0x0a;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret
}

// Unused
void stalfos_setState8_hook(GB *gb) {
  BASE(stalfos_setState8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x08;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  RET(b_+5); return; // ret
}
