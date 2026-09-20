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
void moblin_state_uninitialized_hook(GB *gb);
void moblin_state_scentSeed_hook(GB *gb);
void moblin_state_switchHook_hook(GB *gb);
void moblin_state_stub_hook(GB *gb);
void moblin_state_8_hook(GB *gb);
void moblin_state_9_hook(GB *gb);

static uint16_t enemyCode0c_jump_table(GB *gb) {
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
// ENEMY_ARROW_MOBLIN
// ENEMY_MASKED_MOBLIN
// ENEMY_ARROW_SHROUDED_STALFOS
//
// enemyCode0c, enemyCode20 and enemyCode22 all alias the same address (identical code shared by
// these three enemies); only enemyCode0c is the first-declared bare global.
// ==================================================================================================
void enemyCode0c_hook(GB *gb) {
  BASE(enemyCode0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto dead; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);
  RET(b_+14); return; // ret

dead:
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto die; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(wKilledGoldenEnemies);
  CYC(b_+25, b_+27); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)

die:
  CYC(b_+27, b_+30); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+30, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+33);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  {
    CYC(b_+36, b_+37); push_effect(gb, b_+37);
    uint16_t target = enemyCode0c_jump_table(gb);
    if (target == SYM(moblin_state_uninitialized)) { moblin_state_uninitialized_hook(gb); return; }
    if (target == SYM(moblin_state_stub)) { moblin_state_stub_hook(gb); return; }
    if (target == SYM(moblin_state_switchHook)) { moblin_state_switchHook_hook(gb); return; }
    if (target == SYM(moblin_state_scentSeed)) { moblin_state_scentSeed_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(moblin_state_8)) { moblin_state_8_hook(gb); return; }
    if (target == SYM(moblin_state_9)) { moblin_state_9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:49a4, bare global.
void moblin_state_uninitialized_hook(GB *gb) {
  BASE(moblin_state_uninitialized);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+7, b_+9); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+9, b_+11); goto callArrowDarknutUninit; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wKilledGoldenEnemies);
  CYC(b_+14, b_+16); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+16, b_+19);

callArrowDarknutUninit:
  CYC(b_+19, b_+22); arrowDarknut_state_uninitialized_hook(gb); return; // jp
}

// 0d:49ba, bare global.
void moblin_state_scentSeed_hook(GB *gb) {
  BASE(moblin_state_scentSeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); arrowDarknut_setState8WithRandomAngleAndCounter_hook(gb); return; } // jp z
  CYC(b_+4, b_+7);
  CALL_C(b_+7, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_add(gb, 0x04);
  CYC(b_+15, b_+17); alu_and(gb, 0x18);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CALL_C(b_+18, ecom_updateAnimationFromAngle_b0d_hook, SYM(ecom_updateAnimationFromAngle_b0d), b_+21);
  CALL_C(b_+21, ecom_applyVelocityForSideviewEnemy_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0d), b_+24);
  CYC(b_+24, b_+27); enemyAnimate_hook(gb); return; // jp
}

// 0d:49d5, bare global (also used by darknuts).
void moblin_state_switchHook_hook(GB *gb) {
  BASE(moblin_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = enemyCode0c_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+11) goto substate1;
    if (target == SYM(ecom_fallToGroundAndSetState8_b0d)) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(b_+11); return; // ret
}

// 0d:49e1, bare global.
void moblin_state_stub_hook(GB *gb) {
  BASE(moblin_state_stub);
  RET(b_+0); return; // ret
}

// 0d:49e2, bare global (also darknut state 8).
void moblin_state_8_hook(GB *gb) {
  BASE(moblin_state_8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto incState; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto animate; } // jr nz
  CYC(b_+8, b_+10);

incState:
  CALL_C(b_+10, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x08);

animate:
  CYC(b_+17, b_+20); enemyAnimate_hook(gb); return; // jp
}

// 0d:49f6, bare global; standing until counter1 reaches 0 and a new direction is decided on.
void moblin_state_9_hook(GB *gb) {
  BASE(moblin_state_9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_setRandomCardinalAngle_b0d_hook, SYM(ecom_setRandomCardinalAngle_b0d), b_+7);
  CALL_C(b_+7, arrowDarknut_setState8WithRandomAngleAndCounter_hook, SYM(arrowDarknut_setState8WithRandomAngleAndCounter), b_+10);
  CYC(b_+10, b_+12); arrowDarknut_fireArrowEveryOtherTime_hook(gb); return; // jr
}
