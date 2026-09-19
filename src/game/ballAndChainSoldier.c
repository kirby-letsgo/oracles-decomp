#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5ff4, 0x5ff6); goto normalStatus; } // jr z
  CYC(0x5ff4, 0x5ff6);
  CYC(0x5ff6, 0x5ff8); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5ff8); return; } // ret c
  CYC(0x5ff8, 0x5ff9);
  if (!(F & FZ)) { CYCT(0x5ff9, 0x5ffb); goto normalStatus; } // jr nz
  CYC(0x5ff9, 0x5ffb);
  CYC(0x5ffb, 0x5ffe); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5ffe, ecom_checkHazards_b0e_hook, 0x4051, 0x6001);
  CYC(0x6001, 0x6003); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6003, 0x6004); A = mem_rd(gb, DE);
  {
    CYC(0x6004, 0x6005); push_effect(gb, 0x6005);
    uint16_t target = ballAndChain_jump_table(gb);
    if (target == 0x601b) { ballAndChain_state_uninitialized_hook(gb); return; }
    if (target == 0x603c) { ballAndChain_state_stub_hook(gb); return; }
    if (target == 0x6029) { ballAndChain_state_switchHook_hook(gb); return; }
    if (target == 0x603d) { ballAndChain_state8_hook(gb); return; }
    if (target == 0x605f) { ballAndChain_state9_hook(gb); return; }
    if (target == 0x606e) { ballAndChain_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:601b, bare global; jump-table target from enemyCode4b.
void ballAndChain_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x601b, ballAndChain_spawnSpikedBall_hook, 0x6095, 0x601e);
  if (!(F & FZ)) { RET_TAKEN(0x601e); return; } // ret nz
  CYC(0x601e, 0x601f);
  CYC(0x601f, 0x6021); A = 0x0f; // SPEED_60
  CALL_C(0x6021, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x6024);
  CYC(0x6024, 0x6026); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6026, 0x6028); mem_wr(gb, HL, 0x08);
  RET(0x6028); return; // ret
}

// 0e:6029, bare global; jump-table target from enemyCode4b.
void ballAndChain_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6029, 0x602a); E = alu_inc8(gb, E);
  CYC(0x602a, 0x602b); A = mem_rd(gb, DE);
  {
    CYC(0x602b, 0x602c); push_effect(gb, 0x602c);
    uint16_t target = ballAndChain_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x6034) { RET(0x6034); return; } // ret (substate1/substate2)
    if (target == 0x6035) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(0x6035, 0x6037); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6037, 0x6038); A = mem_rd(gb, DE);
  CYC(0x6038, 0x6039); B = A;
  CYC(0x6039, 0x603c); ecom_fallToGroundAndSetState_b0e_hook(gb); return; // jp
}

// 0e:603c, bare global; jump-table target from enemyCode4b.
void ballAndChain_state_stub_hook(GB *gb) {
  RET(0x603c); return; // ret
}

// 0e:603d, bare global; jump-table target from enemyCode4b. Waiting for Link to be close
// enough to attack.
void ballAndChain_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x603d, 0x603f); C = 0x38;
  CALL_C(0x603f, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6042);
  if (!(F & FC)) { CYCT(0x6042, 0x6044); goto moveTowardLink; } // jr nc
  CYC(0x6042, 0x6044);
  CALL_C(0x6044, ecom_incState_b0e_hook, 0x4000, 0x6047);
  CALL_C(0x6047, ballAndChain_setDefaultState_hook, 0x60b6, 0x604a);
  CYC(0x604a, 0x604c); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x604c, 0x604e); mem_wr(gb, HL, 90);
  CYC(0x604e, 0x6050); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6050, 0x6051); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6051, 0x6053); A = 0x01;
  CYC(0x6053, 0x6056); enemySetAnimation_hook(gb); return; // jp

moveTowardLink:
  CALL_C(0x6056, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x6059);
  CALL_C(0x6059, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x605c);
  ballAndChain_animate_hook(gb); return; // fallthrough
}

// 0e:605c, bare global; falls into from ballAndChain_state8, also reached by genuine jr from
// ballAndChain_state9.
void ballAndChain_animate_hook(GB *gb) {
  CYC(0x605c, 0x605f); enemyAnimate_hook(gb); return; // jp
}

// 0e:605f, bare global; jump-table target from enemyCode4b. Spinning up ball for [counter1]
// frames before attacking.
void ballAndChain_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x605f, ecom_decCounter1_b0e_hook, 0x439a, 0x6062);
  if (!(F & FZ)) { CYCT(0x6062, 0x6064); ballAndChain_animate_hook(gb); return; } // jr nz
  CYC(0x6062, 0x6064);
  CYC(0x6064, 0x6065); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1]++
  CYC(0x6065, 0x6066); L = E;
  CYC(0x6066, 0x6067); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CALL_C(0x6067, ballAndChain_setDefaultState_hook, 0x60b6, 0x606a);
  CYC(0x606a, 0x606c); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x606c, 0x606d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x606d); return; // ret
}

// 0e:606e, bare global; jump-table target from enemyCode4b. Waiting for PART_SPIKED_BALL to
// set this object's counter1 to 0 (signalling the throw is done).
void ballAndChain_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x606e, 0x6070); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6070, 0x6071); A = mem_rd(gb, DE);
  CYC(0x6071, 0x6072); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6072); return; } // ret nz
  CYC(0x6072, 0x6073);
  CYC(0x6073, 0x6075); C = 0x38;
  CALL_C(0x6075, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6078);
  CYC(0x6078, 0x6079); H = D;
  CYC(0x6079, 0x607b); L = ENEMY_BASE + OBJ_STATE;
  if (!(F & FC)) { CYCT(0x607b, 0x607d); goto gotoState8; } // jr nc
  CYC(0x607b, 0x607d);
  CYC(0x607d, 0x607e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]--
  CALL_C(0x607e, ballAndChain_setDefaultState_hook, 0x60b6, 0x6081);
  CYC(0x6081, 0x6083); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6083, 0x6085); mem_wr(gb, HL, 90);
  CYC(0x6085, 0x6087); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6087, 0x6088); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x6088); return; // ret

gotoState8:
  CYC(0x6089, 0x608b); mem_wr(gb, HL, 0x08); // [state]
  CALL_C(0x608b, ballAndChain_setDefaultState_hook, 0x60b6, 0x608e);
  CYC(0x608e, 0x6090); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6090, 0x6091); alu_xor(gb, A);
  CYC(0x6091, 0x6092); mem_wr(gb, HL, A);
  CYC(0x6092, 0x6095); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6095, bare global; called from ballAndChain_state_uninitialized.
// @return zflag z if spawned successfully
void ballAndChain_spawnSpikedBall_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6095, 0x6097); B = 0x04;
  CALL_C(0x6097, checkBEnemySlotsAvailable_hook, 0x20f7, 0x609a);
  if (!(F & FZ)) { RET_TAKEN(0x609a); return; } // ret nz
  CYC(0x609a, 0x609b);
  CYC(0x609b, 0x609d); B = 0x2a; // PART_SPIKED_BALL
  CALL_C(0x609d, ecom_spawnProjectile_b0e_hook, 0x437c, 0x60a0);
  CYC(0x60a0, 0x60a1); C = H;
  CYC(0x60a1, 0x60a3); E = 0x01;

nextChain:
  CALL_C(0x60a3, getFreePartSlot_hook, 0x3e8e, 0x60a6);
  CYC(0x60a6, 0x60a7); mem_wr(gb, HL, B);
  CYC(0x60a7, 0x60a8); L = alu_inc8(gb, L);
  CYC(0x60a8, 0x60a9); mem_wr(gb, HL, E);
  CYC(0x60a9, 0x60ab); L = PART_BASE + OBJ_RELATED1;
  CYC(0x60ab, 0x60ad); A = PART_BASE; // Part.start
  CYC(0x60ad, 0x60ae); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x60ae, 0x60af); mem_wr(gb, HL, C);
  CYC(0x60af, 0x60b0); E = alu_inc8(gb, E);
  CYC(0x60b0, 0x60b1); A = E;
  CYC(0x60b1, 0x60b3); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x60b3, 0x60b5); goto nextChain; } // jr nz
  CYC(0x60b3, 0x60b5);
  RET(0x60b5); return; // ret
}

// 0e:60b6, bare global; called from ballAndChain_state8, ballAndChain_state9,
// ballAndChain_stateA. Sets state the enemy will return to after switch hook is used on it.
// @param hl Pointer to state
void ballAndChain_setDefaultState_hook(GB *gb) {
  CYC(0x60b6, 0x60b7); A = mem_rd(gb, HL);
  CYC(0x60b7, 0x60b9); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x60b9, 0x60ba); mem_wr(gb, HL, A);
  RET(0x60ba); return; // ret
}
