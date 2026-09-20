#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void arrowDarknut_state_uninitialized_hook(GB *gb);
void arrowDarknut_state_9_hook(GB *gb);
void arrowDarknut_fireArrowEveryOtherTime_hook(GB *gb);
void arrowDarknut_setState8WithRandomAngleAndCounter_hook(GB *gb);
void arrowDarknut_chooseAngle_hook(GB *gb);
void moblin_state_stub_hook(GB *gb);
void moblin_state_switchHook_hook(GB *gb);
void moblin_state_8_hook(GB *gb);

static uint16_t enemyCode21_jump_table(GB *gb) {
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
// ENEMY_ARROW_DARKNUT
// ==================================================================================================
void enemyCode21_hook(GB *gb) {
  BASE(enemyCode21);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  RET(b_+15); return; // ret

normalStatus:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = enemyCode21_jump_table(gb);
    if (target == SYM(arrowDarknut_state_uninitialized)) { arrowDarknut_state_uninitialized_hook(gb); return; }
    if (target == SYM(moblin_state_stub)) { moblin_state_stub_hook(gb); return; }
    if (target == SYM(moblin_state_switchHook)) { moblin_state_switchHook_hook(gb); return; }
    if (target == SYM(moblin_state_8)) { moblin_state_8_hook(gb); return; }
    if (target == SYM(arrowDarknut_state_9)) { arrowDarknut_state_9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:4a2a, bare global (also a jump-table target from moblin's dispatcher).
void arrowDarknut_state_uninitialized_hook(GB *gb) {
  BASE(arrowDarknut_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+2, b_+4); A = 0x14; // SPEED_80
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CALL_C(b_+5, ecom_setRandomCardinalAngle_b0d_hook, SYM(ecom_setRandomCardinalAngle_b0d), b_+8);
  CALL_C(b_+8, arrowDarknut_setState8WithRandomAngleAndCounter_hook, SYM(arrowDarknut_setState8WithRandomAngleAndCounter), b_+11);
  CYC(b_+11, b_+14); objectSetVisiblec2_hook(gb); return; // jp
}

// 0d:4a38, bare global (also a jump-table target from moblin's dispatcher).
void arrowDarknut_state_9_hook(GB *gb) {
  BASE(arrowDarknut_state_9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, arrowDarknut_chooseAngle_hook, SYM(arrowDarknut_chooseAngle), b_+7);
  CALL_C(b_+7, arrowDarknut_setState8WithRandomAngleAndCounter_hook, SYM(arrowDarknut_setState8WithRandomAngleAndCounter), SYM(arrowDarknut_fireArrowEveryOtherTime));
  arrowDarknut_fireArrowEveryOtherTime_hook(gb); return; // fallthrough
}

// 0d:4a42, bare global; also used by moblin's state 9 (see moblinsAndShroudedStalfos.c).
void arrowDarknut_fireArrowEveryOtherTime_hook(GB *gb) {
  BASE(arrowDarknut_fireArrowEveryOtherTime);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+6); return; } // ret z
  CYC(b_+6, b_+7);
  CALL_C(b_+7, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+10);
  CYC(b_+10, b_+12); alu_add(gb, 0x04);
  CYC(b_+12, b_+14); alu_and(gb, 0x18);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); B = 0x1a; // PART_ENEMY_ARROW
  CYC(b_+21, b_+24); ecom_spawnProjectile_b0d_hook(gb); return; // jp
}

// 0d:4a5a, bare global; sets random angle and counter, and goes to state 8.
void arrowDarknut_setState8WithRandomAngleAndCounter_hook(GB *gb) {
  BASE(arrowDarknut_setState8WithRandomAngleAndCounter);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x3f);
  CYC(b_+5, b_+7); alu_add(gb, 0x30);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x08);
  CYC(b_+15, b_+18); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp
}

// 0d:4a6c, bare global.
void arrowDarknut_chooseAngle_hook(GB *gb) {
  BASE(arrowDarknut_chooseAngle);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+5, b_+8); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); ecom_setRandomCardinalAngle_b0d_hook(gb); return; // jp
}
