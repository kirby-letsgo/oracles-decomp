#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a02, ecom_checkHazards_b0d_hook, 0x4051, 0x4a05);
  if (F & FZ) { CYCT(0x4a05, 0x4a07); goto normalStatus; } // jr z
  CYC(0x4a05, 0x4a07);
  CYC(0x4a07, 0x4a09); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4a09); return; } // ret c
  CYC(0x4a09, 0x4a0a);
  if (F & FZ) { CYCT(0x4a0a, 0x4a0d); enemyDie_hook(gb); return; } // jp z
  CYC(0x4a0a, 0x4a0d);
  CYC(0x4a0d, 0x4a0e); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4a0e, 0x4a11); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x4a0e, 0x4a11);
  RET(0x4a11); return; // ret

normalStatus:
  CYC(0x4a12, 0x4a14); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a14, 0x4a15); A = mem_rd(gb, DE);
  {
    CYC(0x4a15, 0x4a16); push_effect(gb, 0x4a16);
    uint16_t target = enemyCode21_jump_table(gb);
    if (target == 0x4a2a) { arrowDarknut_state_uninitialized_hook(gb); return; }
    if (target == 0x49e1) { moblin_state_stub_hook(gb); return; }
    if (target == 0x49d5) { moblin_state_switchHook_hook(gb); return; }
    if (target == 0x49e2) { moblin_state_8_hook(gb); return; }
    if (target == 0x4a38) { arrowDarknut_state_9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:4a2a, bare global (also a jump-table target from moblin's dispatcher).
void arrowDarknut_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a2a, 0x4a2c); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4a2c, 0x4a2e); A = 0x14; // SPEED_80
  CYC(0x4a2e, 0x4a2f); mem_wr(gb, DE, A);
  CALL_C(0x4a2f, ecom_setRandomCardinalAngle_b0d_hook, 0x43c6, 0x4a32);
  CALL_C(0x4a32, arrowDarknut_setState8WithRandomAngleAndCounter_hook, 0x4a5a, 0x4a35);
  CYC(0x4a35, 0x4a38); objectSetVisiblec2_hook(gb); return; // jp
}

// 0d:4a38, bare global (also a jump-table target from moblin's dispatcher).
void arrowDarknut_state_9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a38, ecom_decCounter1_b0d_hook, 0x439a, 0x4a3b);
  if (!(F & FZ)) { RET_TAKEN(0x4a3b); return; } // ret nz
  CYC(0x4a3b, 0x4a3c);
  CALL_C(0x4a3c, arrowDarknut_chooseAngle_hook, 0x4a6c, 0x4a3f);
  CALL_C(0x4a3f, arrowDarknut_setState8WithRandomAngleAndCounter_hook, 0x4a5a, 0x4a42);
  arrowDarknut_fireArrowEveryOtherTime_hook(gb); return; // fallthrough
}

// 0d:4a42, bare global; also used by moblin's state 9 (see moblinsAndShroudedStalfos.c).
void arrowDarknut_fireArrowEveryOtherTime_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a42, 0x4a43); H = D;
  CYC(0x4a43, 0x4a45); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4a45, 0x4a46); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4a46, 0x4a48); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x4a48); return; } // ret z
  CYC(0x4a48, 0x4a49);
  CALL_C(0x4a49, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4a4c);
  CYC(0x4a4c, 0x4a4e); alu_add(gb, 0x04);
  CYC(0x4a4e, 0x4a50); alu_and(gb, 0x18);
  CYC(0x4a50, 0x4a51); H = D;
  CYC(0x4a51, 0x4a53); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4a53, 0x4a54); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x4a54); return; } // ret nz
  CYC(0x4a54, 0x4a55);
  CYC(0x4a55, 0x4a57); B = 0x1a; // PART_ENEMY_ARROW
  CYC(0x4a57, 0x4a5a); ecom_spawnProjectile_b0d_hook(gb); return; // jp
}

// 0d:4a5a, bare global; sets random angle and counter, and goes to state 8.
void arrowDarknut_setState8WithRandomAngleAndCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a5a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4a5d);
  CYC(0x4a5d, 0x4a5f); alu_and(gb, 0x3f);
  CYC(0x4a5f, 0x4a61); alu_add(gb, 0x30);
  CYC(0x4a61, 0x4a62); H = D;
  CYC(0x4a62, 0x4a64); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4a64, 0x4a65); mem_wr(gb, HL, A);
  CYC(0x4a65, 0x4a67); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a67, 0x4a69); mem_wr(gb, HL, 0x08);
  CYC(0x4a69, 0x4a6c); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp
}

// 0d:4a6c, bare global.
void arrowDarknut_chooseAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a6c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4a6f);
  CYC(0x4a6f, 0x4a71); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(0x4a71, 0x4a74); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb); return; } // jp z
  CYC(0x4a71, 0x4a74);
  CYC(0x4a74, 0x4a77); ecom_setRandomCardinalAngle_b0d_hook(gb); return; // jp
}
