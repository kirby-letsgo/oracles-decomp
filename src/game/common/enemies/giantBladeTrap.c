#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode2a_jump_table(GB *gb) {
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

static void giantBladeTrap_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// giantBladeTrap_checkCanMoveInDirection@checkTileAtOffsetSolid: an @-local reached only by two
// genuine `call` instructions from giantBladeTrap_checkCanMoveInDirection below; not in the
// requested symbol list and never separately hooked.
static void giantBladeTrap_checkTileAtOffsetSolid(GB *gb, uint16_t return_address) {
  BASE(giantBladeTrap_checkCanMoveInDirection);
  push_effect(gb, return_address);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); alu_add(gb, B);
  CYC(b_+31, b_+33); alu_and(gb, 0xf0);
  CYC(b_+33, b_+34); E = A;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+35, b_+36); alu_add(gb, C);
  CYC(b_+36, b_+38); A = alu_swap(gb, A);
  CYC(b_+38, b_+40); alu_and(gb, 0x0f);
  CYC(b_+40, b_+41); alu_or(gb, E);
  CYC(b_+41, b_+42); E = A;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_or(gb, A);
  CYC(b_+44, b_+45); ret_effect(gb);
}

void giantBladeTrap_state_uninitialized_hook(GB *gb);
void giantBladeTrap_state_stub_hook(GB *gb);
void giantBladeTrap_subid00_hook(GB *gb);
void giantBladeTrap_subid01_hook(GB *gb);
void giantBladeTrap_subid01_state8_hook(GB *gb);
void giantBladeTrap_subid01_state9_hook(GB *gb);
void giantBladeTrap_subid01_stateA_hook(GB *gb);
void giantBladeTrap_subid02_hook(GB *gb);
void giantBladeTrap_subid02_state8_hook(GB *gb);
void giantBladeTrap_commonState9_hook(GB *gb);
void giantBladeTrap_subid02_stateA_hook(GB *gb);
void giantBladeTrap_subid03_hook(GB *gb);
void giantBladeTrap_subid03_state8_hook(GB *gb);
void giantBladeTrap_subid03_stateA_hook(GB *gb);
void giantBladeTrap_chooseInitialAngle_hook(GB *gb);
void giantBladeTrap_checkCanMoveInDirection_hook(GB *gb);
void giantBladeTrap_updateSpeed_hook(GB *gb);

// ==================================================================================================
// ENEMY_GIANT_BLADE_TRAP
// ==================================================================================================
void enemyCode2a_hook(GB *gb) {
  BASE(enemyCode2a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+1, b_+2); ret_effect(gb); return; } // ret z
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret z
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+7);
  if (F & FC) { CYCT(b_+7, b_+9); goto commonState; } // jr c
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == SYM(giantBladeTrap_subid01)) { giantBladeTrap_subid01_hook(gb); return; }
    if (target == SYM(giantBladeTrap_subid02)) { giantBladeTrap_subid02_hook(gb); return; }
    if (target == SYM(giantBladeTrap_subid03)) { giantBladeTrap_subid03_hook(gb); return; }
    giantBladeTrap_subid00_hook(gb); return; // target == 0x63aa
  }

commonState:
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == SYM(giantBladeTrap_state_uninitialized)) { giantBladeTrap_state_uninitialized_hook(gb); return; }
    giantBladeTrap_state_stub_hook(gb); return; // states 1-7 all target 0x63a9
  }
}

void giantBladeTrap_state_uninitialized_hook(GB *gb) {
  BASE(giantBladeTrap_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CYC(b_+3, b_+6); objectSetVisible82_hook(gb); return; // jp
}

void giantBladeTrap_state_stub_hook(GB *gb) {
  BASE(giantBladeTrap_state_stub);
  RET(b_+0); return;
}

void giantBladeTrap_subid00_hook(GB *gb) {
  BASE(giantBladeTrap_subid00);
  RET(b_+0); return;
}

void giantBladeTrap_subid01_hook(GB *gb) {
  BASE(giantBladeTrap_subid01);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == SYM(giantBladeTrap_subid01_state9)) { giantBladeTrap_subid01_state9_hook(gb); return; }
    if (target == SYM(giantBladeTrap_subid01_stateA)) { giantBladeTrap_subid01_stateA_hook(gb); return; }
    giantBladeTrap_subid01_state8_hook(gb); return; // target == 0x63b5
  }
}

// Choosing initial direction to move.
void giantBladeTrap_subid01_state8_hook(GB *gb) {
  BASE(giantBladeTrap_subid01_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x09;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state] = 9
  CALL_C(b_+3, giantBladeTrap_chooseInitialAngle_hook, SYM(giantBladeTrap_chooseInitialAngle), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+8, b_+10); A = 0x14; // SPEED_80
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11); return;
}

// Move until hitting a wall.
void giantBladeTrap_subid01_state9_hook(GB *gb) {
  BASE(giantBladeTrap_subid01_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); objectApplySpeed_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CALL_C(b_+6, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10);
  RET(b_+13); return;
}

// Wait 16 frames, then change directions and start moving again.
void giantBladeTrap_subid01_stateA_hook(GB *gb) {
  BASE(giantBladeTrap_subid01_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]--
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_add(gb, 0x08);
  CYC(b_+11, b_+13); alu_and(gb, 0x18);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  RET(b_+14); return;
}

void giantBladeTrap_subid02_hook(GB *gb) {
  BASE(giantBladeTrap_subid02);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == SYM(giantBladeTrap_commonState9)) { giantBladeTrap_commonState9_hook(gb); return; }
    if (target == SYM(giantBladeTrap_subid02_stateA)) { giantBladeTrap_subid02_stateA_hook(gb); return; }
    giantBladeTrap_subid02_state8_hook(gb); return; // target == 0x63e8
  }
}

// Initialization
void giantBladeTrap_subid02_state8_hook(GB *gb) {
  BASE(giantBladeTrap_subid02_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 60);
  RET(b_+7); return;
}

// Accelerate until hitting a wall. Shared by subid02 and subid03.
void giantBladeTrap_commonState9_hook(GB *gb) {
  BASE(giantBladeTrap_commonState9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, giantBladeTrap_updateSpeed_hook, SYM(giantBladeTrap_updateSpeed), b_+3);
  CALL_C(b_+3, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+9); objectApplySpeed_hook(gb); return; } // jp z
  CYC(b_+6, b_+9);
  CALL_C(b_+9, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_add(gb, 0x02);
  CYC(b_+17, b_+19); alu_and(gb, 0xf8);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_add(gb, 0x02);
  CYC(b_+24, b_+26); alu_and(gb, 0xf8);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x10);
  RET(b_+31); return;
}

// Hit a wall, waiting for a bit then changing direction.
void giantBladeTrap_subid02_stateA_hook(GB *gb) {
  BASE(giantBladeTrap_subid02_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_add(gb, 0x08);
  CYC(b_+9, b_+11); alu_and(gb, 0x1f);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+17); goto canMove; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_xor(gb, 0x10);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+26);
  if (F & FZ) { CYCT(b_+26, b_+28); goto canMove; } // jr z
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_sub(gb, 0x08);
  CYC(b_+33, b_+35); alu_and(gb, 0x1f);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);

canMove:
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 90);
  RET(b_+44); return;
}

void giantBladeTrap_subid03_hook(GB *gb) {
  BASE(giantBladeTrap_subid03);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == SYM(giantBladeTrap_commonState9)) { giantBladeTrap_commonState9_hook(gb); return; }
    if (target == SYM(giantBladeTrap_subid03_stateA)) { giantBladeTrap_subid03_stateA_hook(gb); return; }
    giantBladeTrap_subid03_state8_hook(gb); return; // target == 0x6447
  }
}

// Initialization
void giantBladeTrap_subid03_state8_hook(GB *gb) {
  BASE(giantBladeTrap_subid03_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 90);
  RET(b_+11); return;
}

// Hit a wall, waiting for a bit then changing direction.
void giantBladeTrap_subid03_stateA_hook(GB *gb) {
  BASE(giantBladeTrap_subid03_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_sub(gb, 0x08);
  CYC(b_+9, b_+11); alu_and(gb, 0x1f);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+17); goto canMove; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_xor(gb, 0x10);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+26);
  if (F & FZ) { CYCT(b_+26, b_+28); goto canMove; } // jr z
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_add(gb, 0x08);
  CYC(b_+33, b_+35); alu_and(gb, 0x1f);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);

canMove:
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 90);
  RET(b_+44); return;
}

// Subid 1 only; check all directions, choose which way to go.
void giantBladeTrap_chooseInitialAngle_hook(GB *gb) {
  BASE(giantBladeTrap_chooseInitialAngle);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+3);
  CYC(b_+3, b_+5); A = 0x08; // ANGLE_RIGHT
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto setAngle; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+13);
  CYC(b_+13, b_+15); A = 0x10; // ANGLE_DOWN
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto setAngle; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, giantBladeTrap_checkCanMoveInDirection_hook, SYM(giantBladeTrap_checkCanMoveInDirection), b_+23);
  CYC(b_+23, b_+25); A = 0x18; // ANGLE_LEFT
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto setAngle; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);

setAngle:
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return;
}

// Based on current angle value, this checks if it can move in that direction (it is not
// blocked by solid tiles directly ahead).
//
// @param[out] zflag  z if it can move in this direction.
void giantBladeTrap_checkCanMoveInDirection_hook(GB *gb) {
  BASE(giantBladeTrap_checkCanMoveInDirection);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+15); SET_HL(b_+45); // @positionOffsets
  CYC(b_+15, b_+16); giantBladeTrap_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); push_effect(gb, DE);
  CYC(b_+17, b_+19); D = 0xce; // >wRoomCollisions
  CYC(b_+19, b_+22); giantBladeTrap_checkTileAtOffsetSolid(gb, b_+22);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto skip; } // jr nz
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); giantBladeTrap_checkTileAtOffsetSolid(gb, b_+27);

skip:
  CYC(b_+27, b_+28); SET_DE(pop_effect(gb));
  RET(b_+28); return;
}

// Decrements counter1 and uses its value to determine speed. Lower values = higher speed.
void giantBladeTrap_updateSpeed_hook(GB *gb) {
  BASE(giantBladeTrap_updateSpeed);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); alu_and(gb, 0xf0);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+15); SET_HL(b_+21); // @speeds
  CYC(b_+15, b_+16); giantBladeTrap_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}
