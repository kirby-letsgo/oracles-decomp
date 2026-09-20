#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode4b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode4b), (from), (to), true)

void ballAndChain_state_uninitialized_hook(GB *gb);
void ballAndChain_state_switchHook_hook(GB *gb);
void ballAndChain_state_stub_hook(GB *gb);
void ballAndChain_state8_hook(GB *gb);
void ballAndChain_animate_hook(GB *gb);
void ballAndChain_state9_hook(GB *gb);
void ballAndChain_stateA_hook(GB *gb);
void ballAndChain_spawnSpikedBall_hook(GB *gb);
void ballAndChain_setDefaultState_hook(GB *gb);

static uint16_t ballAndChain_jump_table(GB *gb) {
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
// ENEMY_BALL_AND_CHAIN_SOLDIER
//
// Variables:
//   relatedObj2: reference to PART_SPIKED_BALL
//   counter1: Written to by PART_SPIKED_BALL?
//   var30: Signal for PART_SPIKED_BALL.
//          0: Ball should rotate at normal speed.
//          1: Ball should rotate at double speed.
//          2: Ball should be thrown at Link.
//   var31: State to return to after switch hook is used on enemy
// ==================================================================================================
void enemyCode4b_hook(GB *gb) {
  BASE(enemyCode4b);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+10, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = ballAndChain_jump_table(gb);
    if (target == SYM(ballAndChain_state_uninitialized)) { ballAndChain_state_uninitialized_hook(gb); return; }
    if (target == SYM(ballAndChain_state_stub)) { ballAndChain_state_stub_hook(gb); return; }
    if (target == SYM(ballAndChain_state_switchHook)) { ballAndChain_state_switchHook_hook(gb); return; }
    if (target == SYM(ballAndChain_state8)) { ballAndChain_state8_hook(gb); return; }
    if (target == SYM(ballAndChain_state9)) { ballAndChain_state9_hook(gb); return; }
    if (target == SYM(ballAndChain_stateA)) { ballAndChain_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:601b, bare global; jump-table target from enemyCode4b.
void ballAndChain_state_uninitialized_hook(GB *gb) {
  BASE(ballAndChain_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ballAndChain_spawnSpikedBall_hook, SYM(ballAndChain_spawnSpikedBall), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x0f; // SPEED_60
  CALL_C(b_+6, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x08);
  RET(b_+13); return; // ret
}

// 0e:6029, bare global; jump-table target from enemyCode4b.
void ballAndChain_state_switchHook_hook(GB *gb) {
  BASE(ballAndChain_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = ballAndChain_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; } // ret (substate1/substate2)
    if (target == b_+12) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(b_+12, b_+14); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, SYM(ballAndChain_state_stub)); ecom_fallToGroundAndSetState_b0e_hook(gb); return; // jp
}

// 0e:603c, bare global; jump-table target from enemyCode4b.
void ballAndChain_state_stub_hook(GB *gb) {
  BASE(ballAndChain_state_stub);
  RET(b_+0); return; // ret
}

// 0e:603d, bare global; jump-table target from enemyCode4b. Waiting for Link to be close
// enough to attack.
void ballAndChain_state8_hook(GB *gb) {
  BASE(ballAndChain_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x38;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto moveTowardLink; } // jr nc
  CYC(b_+5, b_+7);
  CALL_C(b_+7, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+10);
  CALL_C(b_+10, ballAndChain_setDefaultState_hook, SYM(ballAndChain_setDefaultState), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 90);
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); enemySetAnimation_hook(gb); return; // jp

moveTowardLink:
  CALL_C(b_+25, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+28);
  CALL_C(b_+28, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), SYM(ballAndChain_animate));
  ballAndChain_animate_hook(gb); return; // fallthrough
}

// 0e:605c, bare global; falls into from ballAndChain_state8, also reached by genuine jr from
// ballAndChain_state9.
void ballAndChain_animate_hook(GB *gb) {
  BASE(ballAndChain_animate);
  CYC(b_+0, SYM(ballAndChain_state9)); enemyAnimate_hook(gb); return; // jp
}

// 0e:605f, bare global; jump-table target from enemyCode4b. Spinning up ball for [counter1]
// frames before attacking.
void ballAndChain_state9_hook(GB *gb) {
  BASE(ballAndChain_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ballAndChain_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1]++
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CALL_C(b_+8, ballAndChain_setDefaultState_hook, SYM(ballAndChain_setDefaultState), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+14); return; // ret
}

// 0e:606e, bare global; jump-table target from enemyCode4b. Waiting for PART_SPIKED_BALL to
// set this object's counter1 to 0 (signalling the throw is done).
void ballAndChain_stateA_hook(GB *gb) {
  BASE(ballAndChain_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); C = 0x38;
  CALL_C(b_+7, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_STATE;
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto gotoState8; } // jr nc
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]--
  CALL_C(b_+16, ballAndChain_setDefaultState_hook, SYM(ballAndChain_setDefaultState), b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 90);
  CYC(b_+23, b_+25); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+26); return; // ret

gotoState8:
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x08); // [state]
  CALL_C(b_+29, ballAndChain_setDefaultState_hook, SYM(ballAndChain_setDefaultState), b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, SYM(ballAndChain_spawnSpikedBall)); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6095, bare global; called from ballAndChain_state_uninitialized.
// @return zflag z if spawned successfully
void ballAndChain_spawnSpikedBall_hook(GB *gb) {
  BASE(ballAndChain_spawnSpikedBall);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x04;
  CALL_C(b_+2, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x2a; // PART_SPIKED_BALL
  CALL_C(b_+8, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+11);
  CYC(b_+11, b_+12); C = H;
  CYC(b_+12, b_+14); E = 0x01;

nextChain:
  CALL_C(b_+14, getFreePartSlot_hook, SYM(getFreePartSlot), b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, HL, B);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+20); mem_wr(gb, HL, E);
  CYC(b_+20, b_+22); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+22, b_+24); A = PART_BASE; // Part.start
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+25, b_+26); mem_wr(gb, HL, C);
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); A = E;
  CYC(b_+28, b_+30); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto nextChain; } // jr nz
  CYC(b_+30, b_+32);
  RET(b_+32); return; // ret
}

// 0e:60b6, bare global; called from ballAndChain_state8, ballAndChain_state9,
// ballAndChain_stateA. Sets state the enemy will return to after switch hook is used on it.
// @param hl Pointer to state
void ballAndChain_setDefaultState_hook(GB *gb) {
  BASE(ballAndChain_setDefaultState);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  RET(b_+4); return; // ret
}
