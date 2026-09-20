#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x64f8, 0x64fa); goto normalStatus; } // jr z
  CYC(0x64f8, 0x64fa);
  CYC(0x64fa, 0x64fc); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x64fc, 0x64fd); ret_effect(gb); return; } // ret c
  CYC(0x64fc, 0x64fd);
  if (F & FZ) { CYCT(0x64fd, 0x6500); enemyDie_hook(gb); return; } // jp z
  CYC(0x64fd, 0x6500);
  CYC(0x6500, 0x6501); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6501, 0x6504); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(0x6501, 0x6504);

normalStatus:
  CALL_C(0x6504, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x6507);
  if (!(F & FC)) { CYCT(0x6507, 0x6509); goto normalState; } // jr nc
  CYC(0x6507, 0x6509);
  CYC(0x6509, 0x650a); push_effect(gb, 0x650a);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == 0x6520) { cheepCheep_state_uninitialized_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    cheepCheep_state_stub_hook(gb); return; // states 1-4, 6, 7 all target 0x6528
  }

normalState:
  CYC(0x651a, 0x651b); A = B;
  CYC(0x651b, 0x651c); push_effect(gb, 0x651c);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == 0x656b) { cheepCheep_subid01_hook(gb); return; }
    cheepCheep_subid00_hook(gb); return; // target == 0x6529
  }
}

void cheepCheep_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6520, 0x6522); A = 0x14; // SPEED_80
  CALL_C(0x6522, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x6525);
  CYC(0x6525, 0x6528); objectSetVisible82_hook(gb); return; // jp
}

void cheepCheep_state_stub_hook(GB *gb) {
  RET(0x6528); return;
}

void cheepCheep_subid00_hook(GB *gb) {
  CYC(0x6529, 0x652a); A = mem_rd(gb, DE);
  CYC(0x652a, 0x652c); alu_sub(gb, 0x08);
  CYC(0x652c, 0x652d); push_effect(gb, 0x652d);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == 0x6543) { cheepCheep_state9_hook(gb); return; }
    if (target == 0x6552) { cheepCheep_stateA_hook(gb); return; }
    cheepCheep_subid00_state8_hook(gb); return; // target == 0x6533
  }
}

// Initialize angle (left), counter1.
void cheepCheep_subid00_state8_hook(GB *gb) {
  CYC(0x6533, 0x6534); H = D;
  CYC(0x6534, 0x6535); L = E;
  CYC(0x6535, 0x6536); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x6536, 0x6538); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6538, 0x653a); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(0x653a, 0x653c); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x653c, 0x653d); A = mem_rd(gb, HL);
  CYC(0x653d, 0x653e); alu_add(gb, A);
  CYC(0x653e, 0x653f); mem_wr(gb, HL, A);
  CYC(0x653f, 0x6541); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6541, 0x6542); mem_wr(gb, HL, A);
  RET(0x6542); return;
}

// Moving until counter1 expires
void cheepCheep_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6543, ecom_decCounter1_b0d_hook, 0x439a, 0x6546);
  if (!(F & FZ)) { CYCT(0x6546, 0x6548); goto applySpeed; } // jr nz
  CYC(0x6546, 0x6548);
  CYC(0x6548, 0x654a); mem_wr(gb, HL, 60);
  CYC(0x654a, 0x654b); L = E;
  CYC(0x654b, 0x654c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++

applySpeed:
  CALL_C(0x654c, objectApplySpeed_hook, 0x201d, 0x654f);
  cheepCheep_animate_hook(gb); return; // falls through
}

void cheepCheep_animate_hook(GB *gb) {
  CYC(0x654f, 0x6552); enemyAnimate_hook(gb); return; // jp
}

// Waiting for 60 frames, then reverse direction
void cheepCheep_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6552, ecom_decCounter1_b0d_hook, 0x439a, 0x6555);
  if (!(F & FZ)) { CYCT(0x6555, 0x6557); cheepCheep_animate_hook(gb); return; } // jr nz
  CYC(0x6555, 0x6557);
  CYC(0x6557, 0x6559); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x6559, 0x655a); A = mem_rd(gb, DE);
  CYC(0x655a, 0x655b); mem_wr(gb, HL, A); // [counter1] = [var03]
  CYC(0x655b, 0x655d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x655d, 0x655e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x655e, 0x6560); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6560, 0x6561); A = mem_rd(gb, HL);
  CYC(0x6561, 0x6563); alu_xor(gb, 0x10);
  CYC(0x6563, 0x6564); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6564, 0x6565); A = mem_rd(gb, HL);
  CYC(0x6565, 0x6567); alu_xor(gb, 0x01);
  CYC(0x6567, 0x6568); mem_wr(gb, HL, A);
  CYC(0x6568, 0x656b); enemySetAnimation_hook(gb); return; // jp
}

void cheepCheep_subid01_hook(GB *gb) {
  CYC(0x656b, 0x656c); A = mem_rd(gb, DE);
  CYC(0x656c, 0x656e); alu_sub(gb, 0x08);
  CYC(0x656e, 0x656f); push_effect(gb, 0x656f);
  {
    uint16_t target = enemyCode2c_jump_table(gb);
    if (target == 0x6543) { cheepCheep_state9_hook(gb); return; }
    if (target == 0x6552) { cheepCheep_stateA_hook(gb); return; }
    cheepCheep_subid01_state8_hook(gb); return; // target == 0x6575
  }
}

// Initialize angle (down), counter1.
void cheepCheep_subid01_state8_hook(GB *gb) {
  CYC(0x6575, 0x6576); H = D;
  CYC(0x6576, 0x6577); L = E;
  CYC(0x6577, 0x6578); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x6578, 0x657a); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x657a, 0x657c); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x657c, 0x657e); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x657e, 0x657f); A = mem_rd(gb, HL);
  CYC(0x657f, 0x6580); alu_add(gb, A);
  CYC(0x6580, 0x6581); mem_wr(gb, HL, A);
  CYC(0x6581, 0x6583); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6583, 0x6584); mem_wr(gb, HL, A);
  RET(0x6584); return;
}
