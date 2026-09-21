#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void peahat_state_uninitialized_hook(GB *gb);
void peahet_state_stub_hook(GB *gb);
void peahat_state8_hook(GB *gb);
void peahat_state9_hook(GB *gb);
void peahat_stateA_hook(GB *gb);
void peahat_animate_hook(GB *gb);
void peahat_stateB_hook(GB *gb);
void peahat_updateEnemyCollisionMode_hook(GB *gb);
void peahat_updatePosition_hook(GB *gb);

static uint16_t peahat_jump_table(GB *gb) {
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

static void peahat_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_PEAHAT
// ==================================================================================================
void enemyCode3e_hook(GB *gb) {
  BASE(enemyCode3e);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);

  // ENEMYSTATUS_KNOCKBACK
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, GV(0x58, 0x55)); // ENEMYCOLLISION_PEAHAT
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);

normalStatus:
  CALL_C(b_+14, peahat_updateEnemyCollisionMode_hook, SYM(peahat_updateEnemyCollisionMode), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  {
    CYC(b_+20, b_+21); push_effect(gb, b_+21);
    uint16_t target = peahat_jump_table(gb);
    if (target == SYM(peahat_state_uninitialized)) { peahat_state_uninitialized_hook(gb); return; }
    if (target == SYM(peahet_state_stub)) { peahet_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(peahat_state8)) { peahat_state8_hook(gb); return; }
    if (target == SYM(peahat_state9)) { peahat_state9_hook(gb); return; }
    if (target == SYM(peahat_stateA)) { peahat_stateA_hook(gb); return; }
    if (target == SYM(peahat_stateB)) { peahat_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:57d4, bare global; jump-table target from enemyCode3e.
void peahat_state_uninitialized_hook(GB *gb) {
  BASE(peahat_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+6); return; // ret
}

// 0e:57db, bare global; jump-table target from enemyCode3e.
void peahet_state_stub_hook(GB *gb) {
  BASE(peahet_state_stub);
  RET(b_+0); return; // ret
}

// 0e:57dc, bare global; jump-table target from enemyCode3e. Stationary for [counter1]
// frames.
void peahat_state8_hook(GB *gb) {
  BASE(peahat_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x7f);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x0f);
  CALL_C(b_+19, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+22);
  CYC(b_+22, b_+24); TAIL(peahat_animate); // jr
}

// 0e:57f4, bare global; jump-table target from enemyCode3e. Accelerating.
void peahat_state9_hook(GB *gb) {
  BASE(peahat_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); peahat_updatePosition_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+9, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x07);
  CYC(b_+14, b_+17); SET_HL(SYM(peahat_counter1Vals)); // @counter1Vals
  peahat_addAToHl_from_rst(gb, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, ecom_setRandomAngle_b0e_hook, SYM(ecom_setRandomAngle_b0e), b_+25);
  CYC(b_+25, b_+27); TAIL(peahat_animate); // jr
}

// 0e:580f, bare global; jump-table target from enemyCode3e. Flying around at top speed.
void peahat_stateA_hook(GB *gb) {
  BASE(peahat_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto beginSlowingDown; } // jr z
  CYC(b_+3, b_+5);

  // Change angle every 32 frames
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x1f);
  if (F & FZ) CALL_C_CC(b_+8, ecom_setRandomAngle_b0e_hook, SYM(ecom_setRandomAngle_b0e), b_+11); else CYC(b_+8, b_+11); // call z
  CALL_C(b_+11, objectApplySpeed_hook, SYM(objectApplySpeed), b_+14);
  CALL_C(b_+14, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+17);
  CYC(b_+17, b_+19); TAIL(peahat_animate); // jr

beginSlowingDown:
  CYC(b_+19, b_+20); L = E;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x00);
  TAIL(peahat_animate); // fallthrough
}

// 0e:5828, bare global; falls into from peahat_stateA, also reached by genuine jr from
// peahat_state8, peahat_state9 and peahat_stateB.
void peahat_animate_hook(GB *gb) {
  BASE(peahat_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0e:582b, bare global; jump-table target from enemyCode3e. Slowing down.
void peahat_stateB_hook(GB *gb) {
  BASE(peahat_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); A = 0x80;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+7, b_+10); peahat_updatePosition_hook(gb); return; } // jp nz
  CYC(b_+7, b_+10);

  // Go to state 8 for $80 frames (if tile is non-solid) or 1 frame (if tile is
  // solid).
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x80); // [counter1]
  PUSH(b_+12, HL);
  CALL_C(b_+13, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+16);
  SET_HL(POP(b_+16));
  if (F & FZ) { CYCT(b_+17, b_+19); goto keepCounter; } // jr z
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x01); // [counter1]

keepCounter:
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x08);
  CALL_C(b_+25, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+28);
  CYC(b_+28, b_+30); TAIL(peahat_animate); // jr
}

// 0e:5849, bare global; called from enemyCode3e.
void peahat_updateEnemyCollisionMode_hook(GB *gb) {
  BASE(peahat_updateEnemyCollisionMode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); A = 0x2e; // ENEMYCOLLISION_PEAHAT_VULNERABLE
  if (F & FZ) { CYCT(b_+6, b_+8); goto setVars; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = GV(0x58, 0x55); // ENEMYCOLLISION_PEAHAT

setVars:
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return; // ret
}

// 0e:5857, bare global; called from peahat_state9 and peahat_stateB. Adjusts speed based
// on counter1, updates position, animates.
void peahat_updatePosition_hook(GB *gb) {
  BASE(peahat_updatePosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+6); alu_cp(gb, 0x41);
  if (!(F & FC)) { CYCT(b_+6, b_+8); goto animate; } // jr nc
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x78);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+13); alu_rlca(gb);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); alu_sub(gb, 0x06);
  if (F & FC) { CYCT(b_+16, b_+18); goto setZh; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);

setZh:
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);

  // Determine speed
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+26); SET_HL(b_+74); // @speedVals
  peahat_addAToHl_from_rst(gb, b_+27);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, objectApplySpeed_hook, SYM(objectApplySpeed), b_+34);
  CALL_C(b_+34, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+37);

animate:
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_and(gb, 0xf0);
  CYC(b_+42, b_+44); A = alu_swap(gb, A);
  CYC(b_+44, b_+47); SET_HL(b_+66); // @animFrequencies
  peahat_addAToHl_from_rst(gb, b_+48);
  CYC(b_+48, b_+49); B = mem_rd(gb, HL);
  CYC(b_+49, b_+50); A = B;
  CYC(b_+50, b_+51); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto checkFrequency; } // jr nz
  CYC(b_+51, b_+53);
  CALL_C(b_+53, enemyAnimate_hook, SYM(enemyAnimate), b_+56);
  CYC(b_+56, b_+58); B = 0x00;

checkFrequency:
  CYC(b_+58, b_+61); A = mem_rd(gb, wFrameCounter);
  CYC(b_+61, b_+62); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+62, b_+65); enemyAnimate_hook(gb); return; } // jp z
  RET(b_+65); return; // ret
}
