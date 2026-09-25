#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode40);
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
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); goto justHit; // jr

justHit:
  // For red wizzrobes only...
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+32); alu_cp(gb, 0x80); // ITEMCOLLISION_LINK|$80
  if (F & FZ) { RET_TAKEN(b_+32); return; } // ret z
  CYC(b_+32, b_+33);

  // The wizzrobe is knocked out of its normal position; allow other wizzrobes to
  // spawn there
  CYC(b_+33, b_+36); TAIL(wizzrobe_removePositionReservation); // jp

normalStatus:
  CALL_C(b_+36, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+39);
  if (!(F & FC)) { CYCT(b_+39, b_+41); goto normalState; } // jr nc
  CYC(b_+39, b_+41);
  {
    CYC(b_+41, b_+42); push_effect(gb, b_+42);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == SYM(wizzrobe_state_uninitialized)) { wizzrobe_state_uninitialized_hook(gb); return; }
    if (target == SYM(wizzrobe_state_stub)) { wizzrobe_state_stub_hook(gb); return; }
    if (target == SYM(wizzrobe_state_switchHook)) { wizzrobe_state_switchHook_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+58, b_+59); A = B;
  {
    CYC(b_+59, b_+60); push_effect(gb, b_+60);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == SYM(wizzrobe_subid0)) { wizzrobe_subid0_hook(gb); return; }
    if (target == SYM(wizzrobe_subid1)) { wizzrobe_subid1_hook(gb); return; }
    if (target == SYM(wizzrobe_subid2)) { wizzrobe_subid2_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:58f4, bare global; jump-table target from enemyCode40.
void wizzrobe_state_uninitialized_hook(GB *gb) {
  BASE(wizzrobe_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_or(gb, 0x42);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto subid1Or2; } // jr nz
  CYC(b_+12, b_+14);

subid0:
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x50);
  RET(b_+20); return; // ret

subid1Or2:
  CYC(b_+21, b_+22); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto subid2; } // jr nz
  CYC(b_+22, b_+24);

subid1:
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+26, b_+29); SET_HL(wWizzrobePositionReservations);
  CYC(b_+29, b_+31); B = 0x10;
  CYC(b_+31, b_+34); TAIL(clearMemory); // jp

subid2:
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x0b); // [state]
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x08);
  CALL_C(b_+44, ecom_setRandomCardinalAngle_b0e_hook, SYM(ecom_setRandomCardinalAngle_b0e), b_+47);
  CYC(b_+47, b_+50); TAIL(wizzrobe_setAnimationFromAngle); // jp
}

// 0e:5926, bare global; jump-table target from enemyCode40.
void wizzrobe_state_switchHook_hook(GB *gb) {
  BASE(wizzrobe_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; } // ret (substate1/substate2)
    if (target == b_+12) goto substate3;
    HANDOFF(target);
  }

substate3:
  CALL_C(b_+12, ecom_fallToGroundAndSetState_b0e_hook, SYM(ecom_fallToGroundAndSetState_b0e), b_+15);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; } // ret nz
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+26); SET_HL(b_+36); // @stateAndCounter1
  CYC(b_+26, b_+27); wizzrobe_addDoubleIndexToHl_from_rst(gb, b_+27);

  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  RET(b_+35); return; // ret
}

// 0e:5950, bare global; jump-table target from enemyCode40.
void wizzrobe_state_stub_hook(GB *gb) {
  BASE(wizzrobe_state_stub);
  RET(b_+0); return; // ret
}

// 0e:5951, bare global; jump-table target from enemyCode40. Green wizzrobe.
void wizzrobe_subid0_hook(GB *gb) {
  BASE(wizzrobe_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == SYM(wizzrobe_subid0_state8)) { wizzrobe_subid0_state8_hook(gb); return; }
    if (target == SYM(wizzrobe_subid0_state9)) { wizzrobe_subid0_state9_hook(gb); return; }
    if (target == SYM(wizzrobe_subid0_stateA)) { wizzrobe_subid0_stateA_hook(gb); return; }
    if (target == SYM(wizzrobe_subid0_stateB)) { wizzrobe_subid0_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:595d, bare global; jump-table target from wizzrobe_subid0. Waiting [counter1] frames
// before spawning in.
void wizzrobe_subid0_state8_hook(GB *gb) {
  BASE(wizzrobe_subid0_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 75);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+8, b_+11); TAIL(objectSetVisiblec2); // jp
}

// 0e:5968, bare global; jump-table target from wizzrobe_subid0. Phasing in for [counter1]
// frames.
void wizzrobe_subid0_state9_hook(GB *gb) {
  BASE(wizzrobe_subid0_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); wizzrobe_checkFlickerVisibility_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 72); // [counter1]
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  CALL_C(b_+14, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+17);
  CYC(b_+17, b_+20); TAIL(wizzrobe_setAnimationFromAngle); // jp
}

// 0e:597c, bare global; jump-table target from wizzrobe_subid0. Fully phased in; standing
// there for [counter1] frames, and firing a projectile at some point.
void wizzrobe_subid0_stateA_hook(GB *gb) {
  BASE(wizzrobe_subid0_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto phaseOut; } // jr z
  CYC(b_+3, b_+5);

  // Fire a projectile when [counter1] == 52
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 52);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); B = 0x1f; // PART_WIZZROBE_PROJECTILE
  CYC(b_+11, b_+14); TAIL(ecom_spawnProjectile_b0e); // jp

phaseOut:
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+24); TAIL(enemySetAnimation); // jp
}

// 0e:5994, bare global; jump-table target from wizzrobe_subid0. Phasing out.
void wizzrobe_subid0_stateB_hook(GB *gb) {
  BASE(wizzrobe_subid0_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_cp(gb, 75);
  if (F & FC) { CYCT(b_+7, b_+10); wizzrobe_checkFlickerVisibility_hook(gb); return; } // jp c
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 72); // [counter1]
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+15, b_+18); TAIL(objectSetInvisible); // jp
}

// 0e:59a6, bare global; jump-table target from enemyCode40. Red wizzrobe.
void wizzrobe_subid1_hook(GB *gb) {
  BASE(wizzrobe_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == SYM(wizzrobe_subid1_state8)) { wizzrobe_subid1_state8_hook(gb); return; }
    if (target == SYM(wizzrobe_subid1_state9)) { wizzrobe_subid1_state9_hook(gb); return; }
    if (target == SYM(wizzrobe_subid1_stateA)) { wizzrobe_subid1_stateA_hook(gb); return; }
    if (target == SYM(wizzrobe_subid1_stateB)) { wizzrobe_subid1_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:59b2, bare global; jump-table target from wizzrobe_subid1. Choosing a new position to
// spawn at.
void wizzrobe_subid1_state8_hook(GB *gb) {
  BASE(wizzrobe_subid1_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, wizzrobe_chooseSpawnPosition_hook, SYM(wizzrobe_chooseSpawnPosition), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, wizzrobe_markSpotAsTaken_hook, SYM(wizzrobe_markSpotAsTaken), b_+7);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);

  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); mem_wr(gb, HL, B);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); mem_wr(gb, HL, C);

  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 60);

  CALL_C(b_+22, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+25);
  CYC(b_+25, b_+28); TAIL(wizzrobe_setAnimationFromAngle); // jp
}

// 0e:59ce, bare global; jump-table target from wizzrobe_subid1. Phasing in for [counter1]
// frames.
void wizzrobe_subid1_state9_hook(GB *gb) {
  BASE(wizzrobe_subid1_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 72); // [counter1]
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+14, b_+17); TAIL(objectSetVisiblec2); // jp
}

// 0e:59df, bare global; jump-table target from wizzrobe_subid1. Fully phased in; standing
// there for [counter1] frames, and firing a projectile at some point.
void wizzrobe_subid1_stateA_hook(GB *gb) {
  BASE(wizzrobe_subid1_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto phaseOut; } // jr z
  CYC(b_+3, b_+5);

  // Fire a projectile when [counter1] == 52
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 52);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); B = 0x1f; // PART_WIZZROBE_PROJECTILE
  CYC(b_+11, b_+14); TAIL(ecom_spawnProjectile_b0e); // jp

phaseOut:
  CYC(b_+14, b_+16); mem_wr(gb, HL, 180); // [counter1]
  CYC(b_+16, b_+17); L = E;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+20, b_+22); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(b_+22); return; // ret
}

// 0e:59f6, bare global; jump-table target from wizzrobe_subid1. Phasing out.
void wizzrobe_subid1_stateB_hook(GB *gb) {
  BASE(wizzrobe_subid1_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto gotoState8; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 120);
  if (F & FC) { RET_TAKEN(b_+8); return; } // ret c
  CYC(b_+8, b_+9);
  if (F & FZ) { CYCT(b_+9, b_+12); objectSetInvisible_hook(gb); return; } // jp z
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); TAIL(ecom_flickerVisibility_b0e); // jp

gotoState8:
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x08); // [state]
  TAIL(wizzrobe_removePositionReservation); // fallthrough
}

// 0e:5a08, bare global; falls into from wizzrobe_subid1_stateB, also reached by genuine jp
// from enemyCode40. Removes position reservation in "wWizzrobePositionReservations"
// allowing other wizzrobes to spawn here.
void wizzrobe_removePositionReservation_hook(GB *gb) {
  BASE(wizzrobe_removePositionReservation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+3, b_+4); L = mem_rd(gb, HL);
  CYC(b_+4, b_+6); H = 0xce; // >wWizzrobePositionReservations
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_sub(gb, D);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return; // ret
}

// 0e:5a14, bare global; jump-table target from enemyCode40. Blue wizzrobe.
void wizzrobe_subid2_hook(GB *gb) {
  BASE(wizzrobe_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = wizzrobe_jump_table(gb);
    if (target == SYM(wizzrobe_subid2_state8)) { wizzrobe_subid2_state8_hook(gb); return; }
    if (target == SYM(wizzrobe_subid2_state9)) { wizzrobe_subid2_state9_hook(gb); return; }
    if (target == SYM(wizzrobe_subid2_stateA)) { wizzrobe_subid2_stateA_hook(gb); return; }
    if (target == SYM(wizzrobe_subid2_stateB)) { wizzrobe_subid2_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5a20, bare global; jump-table target from wizzrobe_subid2. Currently phased in,
// attacking until [counter1] reaches 0 or it hits a wall.
void wizzrobe_subid2_state8_hook(GB *gb) {
  BASE(wizzrobe_subid2_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto phaseOut; } // jr z
  CYC(b_+3, b_+5);

  // Reorient toward Link in [counter2] frames
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter2]
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto updatePosition; } // jr nz
  CYC(b_+7, b_+9);

  CALL_C(b_+9, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+12);
  CALL_C(b_+12, wizzrobe_setAnimationFromAngle_hook, SYM(wizzrobe_setAnimationFromAngle), b_+15);

  // Set random counter2 from $20-$5f
  CALL_C(b_+15, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x3f);
  CYC(b_+20, b_+22); alu_add(gb, 0x20);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);

updatePosition:
  CALL_C(b_+25, wizzrobe_fireEvery32Frames_hook, SYM(wizzrobe_fireEvery32Frames), b_+28);
  CALL_C(b_+28, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+31);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; } // ret nz
  CYC(b_+31, b_+32);

phaseOut:
  CALL_C(b_+32, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+35);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+37, b_+39); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(b_+39); return; // ret
}

// 0e:5a48, bare global; jump-table target from wizzrobe_subid2. Currently phased out,
// choosing a target position.
void wizzrobe_subid2_state9_hook(GB *gb) {
  BASE(wizzrobe_subid2_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, wizzrobe_chooseSpawnPosition_hook, SYM(wizzrobe_chooseSpawnPosition), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);

  // Store target position
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+9, b_+10); mem_wr(gb, HL, B);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);

  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

  CALL_C(b_+18, wizzrobe_setAngleTowardTargetPosition_hook, SYM(wizzrobe_setAngleTowardTargetPosition), b_+21);
  CYC(b_+21, b_+23); TAIL(wizzrobe_setAnimationFromAngle); // jr
}

// 0e:5a5f, bare global; jump-table target from wizzrobe_subid2. Currently phased out,
// moving toward target position.
void wizzrobe_subid2_stateA_hook(GB *gb) {
  BASE(wizzrobe_subid2_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, wizzrobe_setAngleTowardTargetPosition_hook, SYM(wizzrobe_setAngleTowardTargetPosition), b_+3);
  CALL_C(b_+3, ecom_flickerVisibility_b0e_hook, SYM(ecom_flickerVisibility_b0e), b_+6);
  CALL_C(b_+6, wizzrobe_checkReachedTargetPosition_hook, SYM(wizzrobe_checkReachedTargetPosition), b_+9);
  if (!(F & FC)) { CYCT(b_+9, b_+12); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+9, b_+12);

  // Reached target position
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x08);

  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x00);

  CALL_C(b_+23, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+26);
  CALL_C(b_+26, wizzrobe_setAnimationFromAngle_hook, SYM(wizzrobe_setAnimationFromAngle), b_+29);
  CYC(b_+29, b_+32); TAIL(objectSetVisiblec2); // jp
}

// 0e:5a7f, bare global; jump-table target from wizzrobe_subid2. Standing still for
// [counter1] frames (8 frames) before phasing in and attacking again.
void wizzrobe_subid2_stateB_hook(GB *gb) {
  BASE(wizzrobe_subid2_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);

  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x08); // [state]

  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  // Choose random counter1 between $80-$ff (how long to stay in state 8)
  CYC(b_+14, b_+17); SET_BC(0x7f3f);
  CALL_C(b_+17, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+25); alu_add(gb, 0x80);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);

  // Choose random counter2 between $10-$4f (when to reorient toward Link)
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); A = C;
  CYC(b_+28, b_+30); alu_add(gb, 0x10);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);

  CALL_C(b_+31, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+34);
  CALL_C(b_+34, wizzrobe_setAnimationFromAngle_hook, SYM(wizzrobe_setAnimationFromAngle), b_+37);
  CYC(b_+37, b_+40); TAIL(objectSetVisiblec2); // jp
}

// 0e:5aa7, bare global; called from wizzrobe_state_uninitialized, wizzrobe_subid0_state9,
// wizzrobe_subid1_state8 and various wizzrobe_subid2 states.
void wizzrobe_setAnimationFromAngle_hook(GB *gb) {
  BASE(wizzrobe_setAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rlca(gb);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+14); TAIL(enemySetAnimation); // jp
}

// 0e:5ab5, bare global; called from wizzrobe_subid0_state9 and wizzrobe_subid0_stateB.
// Flicker visibility when [counter1] < 45.
void wizzrobe_checkFlickerVisibility_hook(GB *gb) {
  BASE(wizzrobe_checkFlickerVisibility);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 45);
  if (F & FC) { RET_TAKEN(b_+5); return; } // ret c
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); TAIL(ecom_flickerVisibility_b0e); // jp
}

// 0e:5abe, bare global; called from wizzrobe_subid2_stateA.
// @param[out] cflag c if within 1 pixel of target position in both directions
void wizzrobe_checkReachedTargetPosition_hook(GB *gb) {
  BASE(wizzrobe_checkReachedTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  CYC(b_+8, b_+10); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CYC(b_+17, b_+19); alu_cp(gb, 0x03);
  RET(b_+19); return; // ret
}

// 0e:5ad2, bare global; called from wizzrobe_subid2_state9 and wizzrobe_subid2_stateA.
void wizzrobe_setAngleTowardTargetPosition_hook(GB *gb) {
  BASE(wizzrobe_setAngleTowardTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x31; // Enemy.var31
  CALL_C(b_+3, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+6);
  CALL_C(b_+6, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return; // ret
}

// 0e:5adf, bare global; called from wizzrobe_subid1_state8 and wizzrobe_subid2_state9.
// Chooses a random position somewhere within the screen boundaries (accounting for camera
// position). It may choose a solid position (in which case this need to be called again).
// @param[out] bc Chosen position (long form)
// @param[out] l Chosen position (short form)
// @param[out] zflag nz if this tile has solidity
void wizzrobe_chooseSpawnPosition_hook(GB *gb) {
  BASE(wizzrobe_chooseSpawnPosition);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x70); // strictly under SCREEN_HEIGHT<<4
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); A = mem_rd(gb, hCameraY); // hCameraY
  CYC(b_+8, b_+9); alu_add(gb, B);
  CYC(b_+9, b_+11); alu_and(gb, 0xf0);
  CYC(b_+11, b_+13); alu_add(gb, 0x08);
  CYC(b_+13, b_+14); B = A;

pickX:
  CALL_C(b_+14, getRandomNumber_hook, SYM(getRandomNumber), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0xf0);
  CYC(b_+19, b_+21); alu_cp(gb, 0xa0); // SCREEN_WIDTH<<4
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto pickX; } // jr nc
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+26); A = mem_rd(gb, hCameraX); // hCameraX
  CYC(b_+26, b_+27); alu_add(gb, C);
  CYC(b_+27, b_+29); alu_and(gb, 0xf0);
  CYC(b_+29, b_+31); alu_add(gb, 0x08);
  CYC(b_+31, b_+32); C = A;
  CYC(b_+32, b_+35); TAIL(getTileCollisionsAtPosition); // jp
}

// 0e:5b02, bare global; called from wizzrobe_subid2_state8.
void wizzrobe_fireEvery32Frames_hook(GB *gb) {
  BASE(wizzrobe_fireEvery32Frames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x1f; // PART_WIZZROBE_PROJECTILE
  CYC(b_+8, b_+11); TAIL(ecom_spawnProjectile_b0e); // jp
}

// 0e:5b0d, bare global; called from wizzrobe_subid1_state8. Marks a spot as taken in
// wWizzrobePositionReservations; the position is reserved so no other red wizzrobe can
// spawn there. If this position is already reserved, this returns with the zflag set.
// @param l Position
// @param[out] zflag z if position already reserved, or wWizzrobePositionReservations is
// full
void wizzrobe_markSpotAsTaken_hook(GB *gb) {
  BASE(wizzrobe_markSpotAsTaken);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, BC);
  CYC(b_+1, b_+2); E = L;
  CYC(b_+2, b_+4); B = 0x08;
  CYC(b_+4, b_+5); C = B;
  CYC(b_+5, b_+8); SET_HL(wWizzrobePositionReservations);

findSlot:
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+9, b_+10); alu_cp(gb, E);
  if (F & FZ) { CYCT(b_+10, b_+12); goto ret; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto findSlot; } // jr nz
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = 0xe0; // <wWizzrobePositionReservations

findBlank:
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+20, b_+22); goto fillBlankSpot; } // jr z
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+25); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto findBlank; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); goto ret; // jr

fillBlankSpot:
  CYC(b_+29, b_+30); mem_wr(gb, HL, E);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); mem_wr(gb, HL, D);
  CYC(b_+32, b_+34); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+34, b_+35); A = L;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+37); alu_or(gb, D);

ret:
  SET_BC(POP(b_+37));
  RET(b_+38); return; // ret
}
