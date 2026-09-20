#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void keese_state_uninitialized_hook(GB *gb);
void keese_state_stub_hook(GB *gb);
void keese_subid00_hook(GB *gb);
void keese_subid00_state8_hook(GB *gb);
void keese_subid00_state9_hook(GB *gb);
void keese_animate_hook(GB *gb);
void keese_subid00_stateA_hook(GB *gb);
void keese_subid01_hook(GB *gb);
void keese_subid01_state8_hook(GB *gb);
void keese_subid02_state9_hook(GB *gb);
void keese_updateDeceleration_hook(GB *gb);
void keese_initializeSubid_hook(GB *gb);
void keese_chooseWhetherToReverseTurningAngle_hook(GB *gb);

static uint16_t keese_jump_table(GB *gb) {
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

static void keese_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_KEESE
//
// Variables (for subid 1 only, the one that moves as Link approaches):
//   var30: Amount to add to angle each frame. (Clockwise or counterclockwise turning)
// ==================================================================================================
void enemyCode32_hook(GB *gb) {
  BASE(enemyCode32);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  RET(b_+12); return; // ret

normalStatus:
  CALL_C(b_+13, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto normalState; } // jr nc
  CYC(b_+16, b_+18);
  {
    CYC(b_+18, b_+19); push_effect(gb, b_+19);
    uint16_t target = keese_jump_table(gb);
    if (target == SYM(keese_state_uninitialized)) { keese_state_uninitialized_hook(gb); return; }
    if (target == SYM(keese_state_stub)) { keese_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+35, b_+36); A = B;
  {
    CYC(b_+36, b_+37); push_effect(gb, b_+37);
    uint16_t target = keese_jump_table(gb);
    if (target == SYM(keese_subid00)) { keese_subid00_hook(gb); return; }
    if (target == SYM(keese_subid01)) { keese_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

void keese_state_uninitialized_hook(GB *gb) {
  BASE(keese_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+3);
  CALL_C(b_+3, keese_initializeSubid_hook, SYM(keese_initializeSubid), b_+6);
  CYC(b_+6, b_+9); objectSetVisible82_hook(gb); return; // jp
}

void keese_state_stub_hook(GB *gb) {
  BASE(keese_state_stub);
  RET(b_+0); return; // ret
}

void keese_subid00_hook(GB *gb) {
  BASE(keese_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = keese_jump_table(gb);
    if (target == SYM(keese_subid00_state8)) { keese_subid00_state8_hook(gb); return; }
    if (target == SYM(keese_subid00_state9)) { keese_subid00_state9_hook(gb); return; }
    if (target == SYM(keese_subid00_stateA)) { keese_subid00_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// Resting for [counter1] frames
void keese_subid00_state8_hook(GB *gb) {
  BASE(keese_subid00_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_BC(0x1f3f);
  CALL_C(b_+7, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+10);
  CALL_C(b_+10, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, B);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+20, b_+22); A = 0xc0;
  CYC(b_+22, b_+23); alu_add(gb, C);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+28); A = 0x01;
  CALL_C(b_+28, enemySetAnimation_hook, SYM(enemySetAnimation), b_+31);
  CYCT(b_+31, b_+33); keese_animate_hook(gb); return; // jr
}

// Moving in some direction for [counter1] frames
void keese_subid00_state9_hook(GB *gb) {
  BASE(keese_subid00_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+10); alu_rrca(gb);
  if (F & FC) { CYCT(b_+10, b_+12); keese_animate_hook(gb); return; } // jr c
  CYC(b_+10, b_+12);
  CALL_C(b_+12, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+17); goto timeToStop; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); SET_BC(0x0f1f);
  CALL_C(b_+20, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+23);
  CYC(b_+23, b_+24); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); keese_animate_hook(gb); return; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+29); A = C;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYCT(b_+30, b_+32); keese_animate_hook(gb); return; // jr

timeToStop:
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  keese_animate_hook(gb); return; // fallthrough
}

void keese_animate_hook(GB *gb) {
  BASE(keese_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Decelerating until [counter1] counts up to $7f, when it stops completely.
void keese_subid00_stateA_hook(GB *gb) {
  BASE(keese_subid00_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x68);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto L_47ad; } // jr nc
  CYC(b_+5, b_+7);
  CALL_C(b_+7, objectApplySpeed_hook, SYM(objectApplySpeed), b_+10);
  CALL_C(b_+10, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+13);

L_47ad:
  CALL_C(b_+13, keese_updateDeceleration_hook, SYM(keese_updateDeceleration), b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+20, b_+22); A = 0x7f;
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x08);
  CALL_C(b_+28, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+31);
  CYC(b_+31, b_+33); alu_and(gb, 0x7f);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+37); alu_add(gb, 0x20);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+39); alu_xor(gb, A);
  CYC(b_+39, b_+42); enemySetAnimation_hook(gb); return; // jp
}

void keese_subid01_hook(GB *gb) {
  BASE(keese_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = keese_jump_table(gb);
    if (target == SYM(keese_subid01_state8)) { keese_subid01_state8_hook(gb); return; }
    if (target == SYM(keese_subid02_state9)) { keese_subid02_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for Link to approach
void keese_subid01_state8_hook(GB *gb) {
  BASE(keese_subid01_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x31;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+9);
  CALL_C(b_+9, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+20); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+24); alu_and(gb, 0x1f);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 12);
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 12); // [counter2]
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+37); enemySetAnimation_hook(gb); return; // jp
}

void keese_subid02_state9_hook(GB *gb) {
  BASE(keese_subid02_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); keese_animate_hook(gb); return; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 12); // [counter1]
  CYC(b_+13, b_+15); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); alu_and(gb, 0x1f);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+25, b_+27); keese_animate_hook(gb); return; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CALL_C(b_+30, keese_chooseWhetherToReverseTurningAngle_hook, SYM(keese_chooseWhetherToReverseTurningAngle), b_+33);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37); enemySetAnimation_hook(gb); return; // jp
}

// Every 16 frames (based on counter1) this updates the keese's speed as it's decelerating.
// Also handles the animation (which slows down).
void keese_updateDeceleration_hook(GB *gb) {
  BASE(keese_updateDeceleration);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_482e; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_swap_a(gb);
  CYC(b_+10, b_+13); SET_HL(b_+37); // @speeds (data)
  CYC(b_+13, b_+14); keese_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);

L_482e:
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_and(gb, 0xf0);
  CYC(b_+23, b_+25); alu_swap_a(gb);
  CYC(b_+25, b_+28); SET_HL(b_+45); // @bits (data)
  CYC(b_+28, b_+29); keese_addAToHl_from_rst(gb, b_+29);
  CYC(b_+29, b_+32); A = mem_rd(gb, wFrameCounter);
  CYC(b_+32, b_+33); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+33, b_+36); enemyAnimate_hook(gb); return; } // jp z
  CYC(b_+33, b_+36);
  RET(b_+36); return; // ret
}

void keese_initializeSubid_hook(GB *gb) {
  BASE(keese_initializeSubid);
  CYC(b_+0, b_+1); B = alu_dec8(gb, B);
  if (F & FZ) { CYCT(b_+1, b_+3); goto subid1; } // jr z
  CYC(b_+1, b_+3);

  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x20);
  RET(b_+7); return; // ret

subid1:
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xff);
  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x02);
  keese_chooseWhetherToReverseTurningAngle_hook(gb); return; // fallthrough
}

// For subid 1 only, this has a 1 in 4 chance of deciding to reverse the turning angle
// (clockwise or counterclockwise).
void keese_chooseWhetherToReverseTurningAngle_hook(GB *gb) {
  BASE(keese_chooseWhetherToReverseTurningAngle);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_cpl(gb);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return; // ret
}
