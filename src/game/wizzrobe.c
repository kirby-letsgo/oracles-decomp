#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void wizzrobe_state_uninitialized_hook(GB *gb);
void wizzrobe_state_switchHook_hook(GB *gb);
void wizzrobe_state_stub_hook(GB *gb);
void wizzrobe_subid0_hook(GB *gb);
void wizzrobe_subid0_state8_hook(GB *gb);
void wizzrobe_subid0_state9_hook(GB *gb);
void wizzrobe_subid0_stateA_hook(GB *gb);
void wizzrobe_subid0_stateB_hook(GB *gb);
void wizzrobe_subid1_hook(GB *gb);
void wizzrobe_subid1_state8_hook(GB *gb);
void wizzrobe_subid1_state9_hook(GB *gb);
void wizzrobe_subid1_stateA_hook(GB *gb);
void wizzrobe_subid1_stateB_hook(GB *gb);
void wizzrobe_removePositionReservation_hook(GB *gb);
void wizzrobe_subid2_hook(GB *gb);
void wizzrobe_subid2_state8_hook(GB *gb);
void wizzrobe_subid2_state9_hook(GB *gb);
void wizzrobe_subid2_stateA_hook(GB *gb);
void wizzrobe_subid2_stateB_hook(GB *gb);
void wizzrobe_setAnimationFromAngle_hook(GB *gb);
void wizzrobe_checkFlickerVisibility_hook(GB *gb);
void wizzrobe_checkReachedTargetPosition_hook(GB *gb);
void wizzrobe_setAngleTowardTargetPosition_hook(GB *gb);
void wizzrobe_chooseSpawnPosition_hook(GB *gb);
void wizzrobe_fireEvery32Frames_hook(GB *gb);
void wizzrobe_markSpotAsTaken_hook(GB *gb);

static uint16_t wizzrobe_jump_table(GB *gb) {
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

static void wizzrobe_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_WIZZROBE
//
// Variables:
//   var30: The low byte of wWizzrobePositionReservations that this wizzrobe is using
//          (red wizzrobes only)
//   var31/var32: Target position (blue wizzrobes only)
// ==================================================================================================
void enemyCode40_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x58b2, ecom_checkHazardsNoAnimationForHoles_b0e_hook, 0x4043, 0x58b5);
  if (F & FZ) { CYCT(0x58b5, 0x58b7); goto normalStatus; } // jr z
  CYC(0x58b5, 0x58b7);
  CYC(0x58b7, 0x58b9); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x58b9); return; } // ret c
  CYC(0x58b9, 0x58ba);
  if (F & FZ) { CYCT(0x58ba, 0x58bd); enemyDie_hook(gb); return; } // jp z
  CYC(0x58ba, 0x58bd);
  CYC(0x58bd, 0x58be); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x58be, 0x58c1); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x58be, 0x58c1);
  CYC(0x58c1, 0x58c3); goto justHit; // jr

justHit:
  // For red wizzrobes only...
  CYC(0x58c3, 0x58c5); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x58c5, 0x58c6); A = mem_rd(gb, DE);
  CYC(0x58c6, 0x58c7); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x58c7); return; } // ret nz
  CYC(0x58c7, 0x58c8);
  CYC(0x58c8, 0x58ca); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x58ca, 0x58cb); A = mem_rd(gb, DE);
  CYC(0x58cb, 0x58cc); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x58cc); return; } // ret nz
  CYC(0x58cc, 0x58cd);
  CYC(0x58cd, 0x58cf); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x58cf, 0x58d0); A = mem_rd(gb, DE);
  CYC(0x58d0, 0x58d2); alu_cp(gb, 0x80); // ITEMCOLLISION_LINK|$80
  if (F & FZ) { RET_TAKEN(0x58d2); return; } // ret z
  CYC(0x58d2, 0x58d3);

  // The wizzrobe is knocked out of its normal position; allow other wizzrobes to
  // spawn there
  CYC(0x58d3, 0x58d6); wizzrobe_removePositionReservation_hook(gb); return; // jp

normalStatus:
  CALL_C(0x58d6, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x58d9);
  if (!(F & FC)) { CYCT(0x58d9, 0x58db); goto normalState; } // jr nc
  CYC(0x58d9, 0x58db);
  {
    CYC(0x58db, 0x58dc); push_effect(gb, 0x58dc);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == 0x58f4) { wizzrobe_state_uninitialized_hook(gb); return; }
    if (target == 0x5950) { wizzrobe_state_stub_hook(gb); return; }
    if (target == 0x5926) { wizzrobe_state_switchHook_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x58ec, 0x58ed); A = B;
  {
    CYC(0x58ed, 0x58ee); push_effect(gb, 0x58ee);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == 0x5951) { wizzrobe_subid0_hook(gb); return; }
    if (target == 0x59a6) { wizzrobe_subid1_hook(gb); return; }
    if (target == 0x5a14) { wizzrobe_subid2_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:58f4, bare global; jump-table target from enemyCode40.
void wizzrobe_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x58f4, 0x58f5); H = D;
  CYC(0x58f5, 0x58f7); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x58f7, 0x58f8); A = mem_rd(gb, HL);
  CYC(0x58f8, 0x58fa); alu_or(gb, 0x42);
  CYC(0x58fa, 0x58fb); mem_wr(gb, HL, A);
  CYC(0x58fb, 0x58fc); L = E;
  CYC(0x58fc, 0x58fe); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x58fe, 0x58ff); A = mem_rd(gb, DE);
  CYC(0x58ff, 0x5900); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5900, 0x5902); goto subid1Or2; } // jr nz
  CYC(0x5900, 0x5902);

subid0:
  CYC(0x5902, 0x5904); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x5904, 0x5906); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5906, 0x5908); mem_wr(gb, HL, 0x50);
  RET(0x5908); return; // ret

subid1Or2:
  CYC(0x5909, 0x590a); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x590a, 0x590c); goto subid2; } // jr nz
  CYC(0x590a, 0x590c);

subid1:
  CYC(0x590c, 0x590e); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x590e, 0x5911); SET_HL(wWizzrobePositionReservations);
  CYC(0x5911, 0x5913); B = 0x10;
  CYC(0x5913, 0x5916); clearMemory_hook(gb); return; // jp

subid2:
  CYC(0x5916, 0x5918); mem_wr(gb, HL, 0x0b); // [state]
  CYC(0x5918, 0x591a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x591a, 0x591c); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x591c, 0x591e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x591e, 0x5920); mem_wr(gb, HL, 0x08);
  CALL_C(0x5920, ecom_setRandomCardinalAngle_b0e_hook, 0x43c6, 0x5923);
  CYC(0x5923, 0x5926); wizzrobe_setAnimationFromAngle_hook(gb); return; // jp
}

// 0e:5926, bare global; jump-table target from enemyCode40.
void wizzrobe_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5926, 0x5927); E = alu_inc8(gb, E);
  CYC(0x5927, 0x5928); A = mem_rd(gb, DE);
  {
    CYC(0x5928, 0x5929); push_effect(gb, 0x5929);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x5931) { RET(0x5931); return; } // ret (substate1/substate2)
    if (target == 0x5932) goto substate3;
    HANDOFF(target);
  }

substate3:
  CALL_C(0x5932, ecom_fallToGroundAndSetState_b0e_hook, 0x44e2, 0x5935);
  if (!(F & FZ)) { RET_TAKEN(0x5935); return; } // ret nz
  CYC(0x5935, 0x5936);
  CYC(0x5936, 0x5938); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5938, 0x593a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

  CYC(0x593a, 0x593c); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x593c, 0x593d); A = mem_rd(gb, DE);
  CYC(0x593d, 0x5940); SET_HL(0x594a); // @stateAndCounter1
  wizzrobe_addDoubleIndexToHl_from_rst(gb, 0x5941);

  CYC(0x5941, 0x5943); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5943, 0x5944); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5944, 0x5945); mem_wr(gb, DE, A);
  CYC(0x5945, 0x5947); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5947, 0x5948); A = mem_rd(gb, HL);
  CYC(0x5948, 0x5949); mem_wr(gb, DE, A);
  RET(0x5949); return; // ret
}

// 0e:5950, bare global; jump-table target from enemyCode40.
void wizzrobe_state_stub_hook(GB *gb) {
  RET(0x5950); return; // ret
}

// 0e:5951, bare global; jump-table target from enemyCode40. Green wizzrobe.
void wizzrobe_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5951, 0x5952); A = mem_rd(gb, DE);
  CYC(0x5952, 0x5954); alu_sub(gb, 0x08);
  {
    CYC(0x5954, 0x5955); push_effect(gb, 0x5955);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == 0x595d) { wizzrobe_subid0_state8_hook(gb); return; }
    if (target == 0x5968) { wizzrobe_subid0_state9_hook(gb); return; }
    if (target == 0x597c) { wizzrobe_subid0_stateA_hook(gb); return; }
    if (target == 0x5994) { wizzrobe_subid0_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:595d, bare global; jump-table target from wizzrobe_subid0. Waiting [counter1] frames
// before spawning in.
void wizzrobe_subid0_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x595d, ecom_decCounter1_b0e_hook, 0x439a, 0x5960);
  if (!(F & FZ)) { RET_TAKEN(0x5960); return; } // ret nz
  CYC(0x5960, 0x5961);
  CYC(0x5961, 0x5963); mem_wr(gb, HL, 75);
  CYC(0x5963, 0x5964); L = E;
  CYC(0x5964, 0x5965); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x5965, 0x5968); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:5968, bare global; jump-table target from wizzrobe_subid0. Phasing in for [counter1]
// frames.
void wizzrobe_subid0_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5968, ecom_decCounter1_b0e_hook, 0x439a, 0x596b);
  if (!(F & FZ)) { CYCT(0x596b, 0x596e); wizzrobe_checkFlickerVisibility_hook(gb); return; } // jp nz
  CYC(0x596b, 0x596e);
  CYC(0x596e, 0x5970); mem_wr(gb, HL, 72); // [counter1]
  CYC(0x5970, 0x5971); L = E;
  CYC(0x5971, 0x5972); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x5972, 0x5974); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5974, 0x5976); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  CALL_C(0x5976, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x5979);
  CYC(0x5979, 0x597c); wizzrobe_setAnimationFromAngle_hook(gb); return; // jp
}

// 0e:597c, bare global; jump-table target from wizzrobe_subid0. Fully phased in; standing
// there for [counter1] frames, and firing a projectile at some point.
void wizzrobe_subid0_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x597c, ecom_decCounter1_b0e_hook, 0x439a, 0x597f);
  if (F & FZ) { CYCT(0x597f, 0x5981); goto phaseOut; } // jr z
  CYC(0x597f, 0x5981);

  // Fire a projectile when [counter1] == 52
  CYC(0x5981, 0x5982); A = mem_rd(gb, HL);
  CYC(0x5982, 0x5984); alu_cp(gb, 52);
  if (!(F & FZ)) { RET_TAKEN(0x5984); return; } // ret nz
  CYC(0x5984, 0x5985);
  CYC(0x5985, 0x5987); B = 0x1f; // PART_WIZZROBE_PROJECTILE
  CYC(0x5987, 0x598a); ecom_spawnProjectile_b0e_hook(gb); return; // jp

phaseOut:
  CYC(0x598a, 0x598b); L = E;
  CYC(0x598b, 0x598c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x598c, 0x598e); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x598e, 0x5990); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

  CYC(0x5990, 0x5991); alu_xor(gb, A);
  CYC(0x5991, 0x5994); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5994, bare global; jump-table target from wizzrobe_subid0. Phasing out.
void wizzrobe_subid0_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5994, 0x5995); H = D;
  CYC(0x5995, 0x5997); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5997, 0x5998); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5998, 0x5999); A = mem_rd(gb, HL);
  CYC(0x5999, 0x599b); alu_cp(gb, 75);
  if (F & FC) { CYCT(0x599b, 0x599e); wizzrobe_checkFlickerVisibility_hook(gb); return; } // jp c
  CYC(0x599b, 0x599e);
  CYC(0x599e, 0x59a0); mem_wr(gb, HL, 72); // [counter1]
  CYC(0x59a0, 0x59a1); L = E;
  CYC(0x59a1, 0x59a3); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x59a3, 0x59a6); objectSetInvisible_hook(gb); return; // jp
}

// 0e:59a6, bare global; jump-table target from enemyCode40. Red wizzrobe.
void wizzrobe_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59a6, 0x59a7); A = mem_rd(gb, DE);
  CYC(0x59a7, 0x59a9); alu_sub(gb, 0x08);
  {
    CYC(0x59a9, 0x59aa); push_effect(gb, 0x59aa);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == 0x59b2) { wizzrobe_subid1_state8_hook(gb); return; }
    if (target == 0x59ce) { wizzrobe_subid1_state9_hook(gb); return; }
    if (target == 0x59df) { wizzrobe_subid1_stateA_hook(gb); return; }
    if (target == 0x59f6) { wizzrobe_subid1_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:59b2, bare global; jump-table target from wizzrobe_subid1. Choosing a new position to
// spawn at.
void wizzrobe_subid1_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59b2, wizzrobe_chooseSpawnPosition_hook, 0x5adf, 0x59b5);
  if (!(F & FZ)) { RET_TAKEN(0x59b5); return; } // ret nz
  CYC(0x59b5, 0x59b6);
  CALL_C(0x59b6, wizzrobe_markSpotAsTaken_hook, 0x5b0d, 0x59b9);
  if (F & FZ) { RET_TAKEN(0x59b9); return; } // ret z
  CYC(0x59b9, 0x59ba);

  CYC(0x59ba, 0x59bb); H = D;
  CYC(0x59bb, 0x59bd); L = ENEMY_BASE + OBJ_YH;
  CYC(0x59bd, 0x59be); mem_wr(gb, HL, B);
  CYC(0x59be, 0x59c0); L = ENEMY_BASE + OBJ_XH;
  CYC(0x59c0, 0x59c1); mem_wr(gb, HL, C);

  CYC(0x59c1, 0x59c3); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x59c3, 0x59c4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(0x59c4, 0x59c6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x59c6, 0x59c8); mem_wr(gb, HL, 60);

  CALL_C(0x59c8, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x59cb);
  CYC(0x59cb, 0x59ce); wizzrobe_setAnimationFromAngle_hook(gb); return; // jp
}

// 0e:59ce, bare global; jump-table target from wizzrobe_subid1. Phasing in for [counter1]
// frames.
void wizzrobe_subid1_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59ce, ecom_decCounter1_b0e_hook, 0x439a, 0x59d1);
  if (!(F & FZ)) { CYCT(0x59d1, 0x59d4); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(0x59d1, 0x59d4);
  CYC(0x59d4, 0x59d6); mem_wr(gb, HL, 72); // [counter1]
  CYC(0x59d6, 0x59d7); L = E;
  CYC(0x59d7, 0x59d8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x59d8, 0x59da); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x59da, 0x59dc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x59dc, 0x59df); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:59df, bare global; jump-table target from wizzrobe_subid1. Fully phased in; standing
// there for [counter1] frames, and firing a projectile at some point.
void wizzrobe_subid1_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59df, ecom_decCounter1_b0e_hook, 0x439a, 0x59e2);
  if (F & FZ) { CYCT(0x59e2, 0x59e4); goto phaseOut; } // jr z
  CYC(0x59e2, 0x59e4);

  // Fire a projectile when [counter1] == 52
  CYC(0x59e4, 0x59e5); A = mem_rd(gb, HL);
  CYC(0x59e5, 0x59e7); alu_cp(gb, 52);
  if (!(F & FZ)) { RET_TAKEN(0x59e7); return; } // ret nz
  CYC(0x59e7, 0x59e8);
  CYC(0x59e8, 0x59ea); B = 0x1f; // PART_WIZZROBE_PROJECTILE
  CYC(0x59ea, 0x59ed); ecom_spawnProjectile_b0e_hook(gb); return; // jp

phaseOut:
  CYC(0x59ed, 0x59ef); mem_wr(gb, HL, 180); // [counter1]
  CYC(0x59ef, 0x59f0); L = E;
  CYC(0x59f0, 0x59f1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x59f1, 0x59f3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x59f3, 0x59f5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(0x59f5); return; // ret
}

// 0e:59f6, bare global; jump-table target from wizzrobe_subid1. Phasing out.
void wizzrobe_subid1_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59f6, ecom_decCounter1_b0e_hook, 0x439a, 0x59f9);
  if (F & FZ) { CYCT(0x59f9, 0x59fb); goto gotoState8; } // jr z
  CYC(0x59f9, 0x59fb);
  CYC(0x59fb, 0x59fc); A = mem_rd(gb, HL);
  CYC(0x59fc, 0x59fe); alu_cp(gb, 120);
  if (F & FC) { RET_TAKEN(0x59fe); return; } // ret c
  CYC(0x59fe, 0x59ff);
  if (F & FZ) { CYCT(0x59ff, 0x5a02); objectSetInvisible_hook(gb); return; } // jp z
  CYC(0x59ff, 0x5a02);
  CYC(0x5a02, 0x5a05); ecom_flickerVisibility_b0e_hook(gb); return; // jp

gotoState8:
  CYC(0x5a05, 0x5a06); L = E;
  CYC(0x5a06, 0x5a08); mem_wr(gb, HL, 0x08); // [state]
  wizzrobe_removePositionReservation_hook(gb); return; // fallthrough
}

// 0e:5a08, bare global; falls into from wizzrobe_subid1_stateB, also reached by genuine jp
// from enemyCode40. Removes position reservation in "wWizzrobePositionReservations"
// allowing other wizzrobes to spawn here.
void wizzrobe_removePositionReservation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a08, 0x5a09); H = D;
  CYC(0x5a09, 0x5a0b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5a0b, 0x5a0c); L = mem_rd(gb, HL);
  CYC(0x5a0c, 0x5a0e); H = 0xce; // >wWizzrobePositionReservations
  CYC(0x5a0e, 0x5a0f); A = mem_rd(gb, HL);
  CYC(0x5a0f, 0x5a10); alu_sub(gb, D);
  if (!(F & FZ)) { RET_TAKEN(0x5a10); return; } // ret nz
  CYC(0x5a10, 0x5a11);
  CYC(0x5a11, 0x5a12); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x5a12, 0x5a13); mem_wr(gb, HL, A);
  RET(0x5a13); return; // ret
}

// 0e:5a14, bare global; jump-table target from enemyCode40. Blue wizzrobe.
void wizzrobe_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5a14, 0x5a15); A = mem_rd(gb, DE);
  CYC(0x5a15, 0x5a17); alu_sub(gb, 0x08);
  {
    CYC(0x5a17, 0x5a18); push_effect(gb, 0x5a18);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == 0x5a20) { wizzrobe_subid2_state8_hook(gb); return; }
    if (target == 0x5a48) { wizzrobe_subid2_state9_hook(gb); return; }
    if (target == 0x5a5f) { wizzrobe_subid2_stateA_hook(gb); return; }
    if (target == 0x5a7f) { wizzrobe_subid2_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5a20, bare global; jump-table target from wizzrobe_subid2. Currently phased in,
// attacking until [counter1] reaches 0 or it hits a wall.
void wizzrobe_subid2_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a20, ecom_decCounter1_b0e_hook, 0x439a, 0x5a23);
  if (F & FZ) { CYCT(0x5a23, 0x5a25); goto phaseOut; } // jr z
  CYC(0x5a23, 0x5a25);

  // Reorient toward Link in [counter2] frames
  CYC(0x5a25, 0x5a26); L = alu_inc8(gb, L);
  CYC(0x5a26, 0x5a27); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter2]
  if (!(F & FZ)) { CYCT(0x5a27, 0x5a29); goto updatePosition; } // jr nz
  CYC(0x5a27, 0x5a29);

  CALL_C(0x5a29, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x5a2c);
  CALL_C(0x5a2c, wizzrobe_setAnimationFromAngle_hook, 0x5aa7, 0x5a2f);

  // Set random counter2 from $20-$5f
  CALL_C(0x5a2f, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5a32);
  CYC(0x5a32, 0x5a34); alu_and(gb, 0x3f);
  CYC(0x5a34, 0x5a36); alu_add(gb, 0x20);
  CYC(0x5a36, 0x5a38); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5a38, 0x5a39); mem_wr(gb, DE, A);

updatePosition:
  CALL_C(0x5a39, wizzrobe_fireEvery32Frames_hook, 0x5b02, 0x5a3c);
  CALL_C(0x5a3c, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x5a3f);
  if (!(F & FZ)) { RET_TAKEN(0x5a3f); return; } // ret nz
  CYC(0x5a3f, 0x5a40);

phaseOut:
  CALL_C(0x5a40, ecom_incState_b0e_hook, 0x4000, 0x5a43);
  CYC(0x5a43, 0x5a45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5a45, 0x5a47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(0x5a47); return; // ret
}

// 0e:5a48, bare global; jump-table target from wizzrobe_subid2. Currently phased out,
// choosing a target position.
void wizzrobe_subid2_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a48, wizzrobe_chooseSpawnPosition_hook, 0x5adf, 0x5a4b);
  if (!(F & FZ)) { CYCT(0x5a4b, 0x5a4e); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(0x5a4b, 0x5a4e);

  // Store target position
  CYC(0x5a4e, 0x5a4f); H = D;
  CYC(0x5a4f, 0x5a51); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5a51, 0x5a52); mem_wr(gb, HL, B);
  CYC(0x5a52, 0x5a53); L = alu_inc8(gb, L);
  CYC(0x5a53, 0x5a54); mem_wr(gb, HL, C);

  CYC(0x5a54, 0x5a56); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5a56, 0x5a57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(0x5a57, 0x5a59); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5a59, 0x5a5a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

  CALL_C(0x5a5a, wizzrobe_setAngleTowardTargetPosition_hook, 0x5ad2, 0x5a5d);
  CYC(0x5a5d, 0x5a5f); wizzrobe_setAnimationFromAngle_hook(gb); return; // jr
}

// 0e:5a5f, bare global; jump-table target from wizzrobe_subid2. Currently phased out,
// moving toward target position.
void wizzrobe_subid2_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a5f, wizzrobe_setAngleTowardTargetPosition_hook, 0x5ad2, 0x5a62);
  CALL_C(0x5a62, ecom_flickerVisibility_b0e_hook, 0x441f, 0x5a65);
  CALL_C(0x5a65, wizzrobe_checkReachedTargetPosition_hook, 0x5abe, 0x5a68);
  if (!(F & FC)) { CYCT(0x5a68, 0x5a6b); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x5a68, 0x5a6b);

  // Reached target position
  CYC(0x5a6b, 0x5a6d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5a6d, 0x5a6e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(0x5a6e, 0x5a70); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5a70, 0x5a72); mem_wr(gb, HL, 0x08);

  CYC(0x5a72, 0x5a74); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5a74, 0x5a76); mem_wr(gb, HL, 0x00);

  CALL_C(0x5a76, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x5a79);
  CALL_C(0x5a79, wizzrobe_setAnimationFromAngle_hook, 0x5aa7, 0x5a7c);
  CYC(0x5a7c, 0x5a7f); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:5a7f, bare global; jump-table target from wizzrobe_subid2. Standing still for
// [counter1] frames (8 frames) before phasing in and attacking again.
void wizzrobe_subid2_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a7f, ecom_decCounter1_b0e_hook, 0x439a, 0x5a82);
  if (!(F & FZ)) { CYCT(0x5a82, 0x5a85); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(0x5a82, 0x5a85);

  CYC(0x5a85, 0x5a86); H = D;
  CYC(0x5a86, 0x5a87); L = E;
  CYC(0x5a87, 0x5a89); mem_wr(gb, HL, 0x08); // [state]

  CYC(0x5a89, 0x5a8b); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5a8b, 0x5a8d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  // Choose random counter1 between $80-$ff (how long to stay in state 8)
  CYC(0x5a8d, 0x5a90); SET_BC(0x7f3f);
  CALL_C(0x5a90, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x5a93);
  CYC(0x5a93, 0x5a95); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5a95, 0x5a96); A = B;
  CYC(0x5a96, 0x5a98); alu_add(gb, 0x80);
  CYC(0x5a98, 0x5a99); mem_wr(gb, DE, A);

  // Choose random counter2 between $10-$4f (when to reorient toward Link)
  CYC(0x5a99, 0x5a9a); E = alu_inc8(gb, E);
  CYC(0x5a9a, 0x5a9b); A = C;
  CYC(0x5a9b, 0x5a9d); alu_add(gb, 0x10);
  CYC(0x5a9d, 0x5a9e); mem_wr(gb, DE, A);

  CALL_C(0x5a9e, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x5aa1);
  CALL_C(0x5aa1, wizzrobe_setAnimationFromAngle_hook, 0x5aa7, 0x5aa4);
  CYC(0x5aa4, 0x5aa7); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:5aa7, bare global; called from wizzrobe_state_uninitialized, wizzrobe_subid0_state9,
// wizzrobe_subid1_state8 and various wizzrobe_subid2 states.
void wizzrobe_setAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5aa7, 0x5aa9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5aa9, 0x5aaa); A = mem_rd(gb, DE);
  CYC(0x5aaa, 0x5aac); alu_add(gb, 0x04);
  CYC(0x5aac, 0x5aae); alu_and(gb, 0x18);
  CYC(0x5aae, 0x5ab0); A = alu_swap(gb, A);
  CYC(0x5ab0, 0x5ab1); alu_rlca(gb);
  CYC(0x5ab1, 0x5ab2); A = alu_inc8(gb, A);
  CYC(0x5ab2, 0x5ab5); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5ab5, bare global; called from wizzrobe_subid0_state9 and wizzrobe_subid0_stateB.
// Flicker visibility when [counter1] < 45.
void wizzrobe_checkFlickerVisibility_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ab5, 0x5ab7); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5ab7, 0x5ab8); A = mem_rd(gb, DE);
  CYC(0x5ab8, 0x5aba); alu_cp(gb, 45);
  if (F & FC) { RET_TAKEN(0x5aba); return; } // ret c
  CYC(0x5aba, 0x5abb);
  CYC(0x5abb, 0x5abe); ecom_flickerVisibility_b0e_hook(gb); return; // jp
}

// 0e:5abe, bare global; called from wizzrobe_subid2_stateA.
// @param[out] cflag c if within 1 pixel of target position in both directions
void wizzrobe_checkReachedTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5abe, 0x5abf); H = D;
  CYC(0x5abf, 0x5ac1); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5ac1, 0x5ac3); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5ac3, 0x5ac4); A = mem_rd(gb, DE);
  CYC(0x5ac4, 0x5ac5); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5ac5, 0x5ac6); A = alu_inc8(gb, A);
  CYC(0x5ac6, 0x5ac8); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(0x5ac8); return; } // ret nc
  CYC(0x5ac8, 0x5ac9);
  CYC(0x5ac9, 0x5acb); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5acb, 0x5acc); E = alu_inc8(gb, E);
  CYC(0x5acc, 0x5acd); A = mem_rd(gb, DE);
  CYC(0x5acd, 0x5ace); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5ace, 0x5acf); A = alu_inc8(gb, A);
  CYC(0x5acf, 0x5ad1); alu_cp(gb, 0x03);
  RET(0x5ad1); return; // ret
}

// 0e:5ad2, bare global; called from wizzrobe_subid2_state9 and wizzrobe_subid2_stateA.
void wizzrobe_setAngleTowardTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ad2, 0x5ad3); H = D;
  CYC(0x5ad3, 0x5ad5); L = ENEMY_BASE + 0x31; // Enemy.var31
  CALL_C(0x5ad5, ecom_readPositionVars_b0e_hook, 0x4439, 0x5ad8);
  CALL_C(0x5ad8, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x5adb);
  CYC(0x5adb, 0x5add); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5add, 0x5ade); mem_wr(gb, DE, A);
  RET(0x5ade); return; // ret
}

// 0e:5adf, bare global; called from wizzrobe_subid1_state8 and wizzrobe_subid2_state9.
// Chooses a random position somewhere within the screen boundaries (accounting for camera
// position). It may choose a solid position (in which case this need to be called again).
// @param[out] bc Chosen position (long form)
// @param[out] l Chosen position (short form)
// @param[out] zflag nz if this tile has solidity
void wizzrobe_chooseSpawnPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5adf, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5ae2);
  CYC(0x5ae2, 0x5ae4); alu_and(gb, 0x70); // strictly under SCREEN_HEIGHT<<4
  CYC(0x5ae4, 0x5ae5); B = A;
  CYC(0x5ae5, 0x5ae7); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(0x5ae7, 0x5ae8); alu_add(gb, B);
  CYC(0x5ae8, 0x5aea); alu_and(gb, 0xf0);
  CYC(0x5aea, 0x5aec); alu_add(gb, 0x08);
  CYC(0x5aec, 0x5aed); B = A;

pickX:
  CALL_C(0x5aed, getRandomNumber_hook, 0x043e, 0x5af0);
  CYC(0x5af0, 0x5af2); alu_and(gb, 0xf0);
  CYC(0x5af2, 0x5af4); alu_cp(gb, 0xa0); // SCREEN_WIDTH<<4
  if (!(F & FC)) { CYCT(0x5af4, 0x5af6); goto pickX; } // jr nc
  CYC(0x5af4, 0x5af6);
  CYC(0x5af6, 0x5af7); C = A;
  CYC(0x5af7, 0x5af9); A = hram_rd(gb, 0xac); // hCameraX
  CYC(0x5af9, 0x5afa); alu_add(gb, C);
  CYC(0x5afa, 0x5afc); alu_and(gb, 0xf0);
  CYC(0x5afc, 0x5afe); alu_add(gb, 0x08);
  CYC(0x5afe, 0x5aff); C = A;
  CYC(0x5aff, 0x5b02); getTileCollisionsAtPosition_hook(gb); return; // jp
}

// 0e:5b02, bare global; called from wizzrobe_subid2_state8.
void wizzrobe_fireEvery32Frames_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b02, 0x5b04); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5b04, 0x5b05); A = mem_rd(gb, DE);
  CYC(0x5b05, 0x5b07); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x5b07); return; } // ret nz
  CYC(0x5b07, 0x5b08);
  CYC(0x5b08, 0x5b0a); B = 0x1f; // PART_WIZZROBE_PROJECTILE
  CYC(0x5b0a, 0x5b0d); ecom_spawnProjectile_b0e_hook(gb); return; // jp
}

// 0e:5b0d, bare global; called from wizzrobe_subid1_state8. Marks a spot as taken in
// wWizzrobePositionReservations; the position is reserved so no other red wizzrobe can
// spawn there. If this position is already reserved, this returns with the zflag set.
// @param l Position
// @param[out] zflag z if position already reserved, or wWizzrobePositionReservations is
// full
void wizzrobe_markSpotAsTaken_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(0x5b0d, BC);
  CYC(0x5b0e, 0x5b0f); E = L;
  CYC(0x5b0f, 0x5b11); B = 0x08;
  CYC(0x5b11, 0x5b12); C = B;
  CYC(0x5b12, 0x5b15); SET_HL(wWizzrobePositionReservations);

findSlot:
  CYC(0x5b15, 0x5b16); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5b16, 0x5b17); alu_cp(gb, E);
  if (F & FZ) { CYCT(0x5b17, 0x5b19); goto ret; } // jr z
  CYC(0x5b17, 0x5b19);
  CYC(0x5b19, 0x5b1a); L = alu_inc8(gb, L);
  CYC(0x5b1a, 0x5b1b); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x5b1b, 0x5b1d); goto findSlot; } // jr nz
  CYC(0x5b1b, 0x5b1d);
  CYC(0x5b1d, 0x5b1f); L = 0xe0; // <wWizzrobePositionReservations

findBlank:
  CYC(0x5b1f, 0x5b20); A = mem_rd(gb, HL);
  CYC(0x5b20, 0x5b21); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b21, 0x5b23); goto fillBlankSpot; } // jr z
  CYC(0x5b21, 0x5b23);
  CYC(0x5b23, 0x5b24); L = alu_inc8(gb, L);
  CYC(0x5b24, 0x5b25); L = alu_inc8(gb, L);
  CYC(0x5b25, 0x5b26); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x5b26, 0x5b28); goto findBlank; } // jr nz
  CYC(0x5b26, 0x5b28);
  CYC(0x5b28, 0x5b2a); goto ret; // jr

fillBlankSpot:
  CYC(0x5b2a, 0x5b2b); mem_wr(gb, HL, E);
  CYC(0x5b2b, 0x5b2c); L = alu_inc8(gb, L);
  CYC(0x5b2c, 0x5b2d); mem_wr(gb, HL, D);
  CYC(0x5b2d, 0x5b2f); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5b2f, 0x5b30); A = L;
  CYC(0x5b30, 0x5b31); mem_wr(gb, DE, A);
  CYC(0x5b31, 0x5b32); alu_or(gb, D);

ret:
  SET_BC(POP(0x5b32));
  RET(0x5b33); return; // ret
}
