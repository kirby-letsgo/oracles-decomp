#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode2c_jump_table(GB *gb) {
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

void cheepCheep_state_uninitialized_hook(GB *gb);
void cheepCheep_state_stub_hook(GB *gb);
void cheepCheep_subid00_hook(GB *gb);
void cheepCheep_subid00_state8_hook(GB *gb);
void cheepCheep_state9_hook(GB *gb);
void cheepCheep_animate_hook(GB *gb);
void cheepCheep_stateA_hook(GB *gb);
void cheepCheep_subid01_hook(GB *gb);
void cheepCheep_subid01_state8_hook(GB *gb);

// ==================================================================================================
// ENEMY_CHEEP_CHEEP
//
// Variables:
//   var03: How far to travel (copied to counter1)
// ==================================================================================================
void enemyCode2c_hook(GB *gb) {
  BASE(enemyCode2c);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);

normalStatus:
  CALL_C(b_+12, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+15);
  if (!(F & FC)) { CYCT(b_+15, b_+17); goto normalState; } // jr nc
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == SYM(cheepCheep_state_uninitialized)) { cheepCheep_state_uninitialized_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    cheepCheep_state_stub_hook(gb); return; // states 1-4, 6, 7 all target 0x6528
  }

normalState:
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+36); push_effect(gb, b_+36);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == SYM(cheepCheep_subid01)) { cheepCheep_subid01_hook(gb); return; }
    cheepCheep_subid00_hook(gb); return; // target == 0x6529
  }
}

void cheepCheep_state_uninitialized_hook(GB *gb) {
  BASE(cheepCheep_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x14; // SPEED_80
  CALL_C(b_+2, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+5);
  CYC(b_+5, b_+8); objectSetVisible82_hook(gb); return; // jp
}

void cheepCheep_state_stub_hook(GB *gb) {
  BASE(cheepCheep_state_stub);
  RET(b_+0); return;
}

void cheepCheep_subid00_hook(GB *gb) {
  BASE(cheepCheep_subid00);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == SYM(cheepCheep_state9)) { cheepCheep_state9_hook(gb); return; }
    if (target == SYM(cheepCheep_stateA)) { cheepCheep_stateA_hook(gb); return; }
    cheepCheep_subid00_state8_hook(gb); return; // target == 0x6533
  }
}

// Initialize angle (left), counter1.
void cheepCheep_subid00_state8_hook(GB *gb) {
  BASE(cheepCheep_subid00_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  RET(b_+15); return;
}

// Moving until counter1 expires
void cheepCheep_state9_hook(GB *gb) {
  BASE(cheepCheep_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto applySpeed; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 60);
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++

applySpeed:
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(cheepCheep_animate));
  cheepCheep_animate_hook(gb); return; // falls through
}

void cheepCheep_animate_hook(GB *gb) {
  BASE(cheepCheep_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Waiting for 60 frames, then reverse direction
void cheepCheep_stateA_hook(GB *gb) {
  BASE(cheepCheep_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); cheepCheep_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); // [counter1] = [var03]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_xor(gb, 0x10);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_xor(gb, 0x01);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+25); enemySetAnimation_hook(gb); return; // jp
}

void cheepCheep_subid01_hook(GB *gb) {
  BASE(cheepCheep_subid01);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == SYM(cheepCheep_state9)) { cheepCheep_state9_hook(gb); return; }
    if (target == SYM(cheepCheep_stateA)) { cheepCheep_stateA_hook(gb); return; }
    cheepCheep_subid01_state8_hook(gb); return; // target == 0x6575
  }
}

// Initialize angle (down), counter1.
void cheepCheep_subid01_state8_hook(GB *gb) {
  BASE(cheepCheep_subid01_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  RET(b_+15); return;
}
