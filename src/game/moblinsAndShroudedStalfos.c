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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x496b, ecom_checkHazards_b0d_hook, 0x4051, 0x496e);
  if (F & FZ) { CYCT(0x496e, 0x4970); goto normalStatus; } // jr z
  CYC(0x496e, 0x4970);
  CYC(0x4970, 0x4972); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4972); return; } // ret c
  CYC(0x4972, 0x4973);
  if (F & FZ) { CYCT(0x4973, 0x4975); goto dead; } // jr z
  CYC(0x4973, 0x4975);
  CYC(0x4975, 0x4976); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4976, 0x4979); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x4976, 0x4979);
  RET(0x4979); return; // ret

dead:
  CYC(0x497a, 0x497c); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x497c, 0x497d); A = mem_rd(gb, DE);
  CYC(0x497d, 0x497f); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x497f, 0x4981); goto die; } // jr nz
  CYC(0x497f, 0x4981);
  CYC(0x4981, 0x4984); SET_HL(wKilledGoldenEnemies);
  CYC(0x4984, 0x4986); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)

die:
  CYC(0x4986, 0x4989); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x4989, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x498c);
  CYC(0x498c, 0x498e); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x498e, 0x498f); A = mem_rd(gb, DE);
  {
    CYC(0x498f, 0x4990); push_effect(gb, 0x4990);
    uint16_t target = enemyCode0c_jump_table(gb);
    if (target == 0x49a4) { moblin_state_uninitialized_hook(gb); return; }
    if (target == 0x49e1) { moblin_state_stub_hook(gb); return; }
    if (target == 0x49d5) { moblin_state_switchHook_hook(gb); return; }
    if (target == 0x49ba) { moblin_state_scentSeed_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x49e2) { moblin_state_8_hook(gb); return; }
    if (target == 0x49f6) { moblin_state_9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:49a4, bare global.
void moblin_state_uninitialized_hook(GB *gb) {
  CYC(0x49a4, 0x49a5); H = D;
  CYC(0x49a5, 0x49a7); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x49a7, 0x49a9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x49a9, 0x49ab); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x49ab, 0x49ad); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x49ad, 0x49af); goto callArrowDarknutUninit; } // jr z
  CYC(0x49ad, 0x49af);
  CYC(0x49af, 0x49b2); A = mem_rd(gb, wKilledGoldenEnemies);
  CYC(0x49b2, 0x49b4); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x49b4, 0x49b7); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x49b4, 0x49b7);

callArrowDarknutUninit:
  CYC(0x49b7, 0x49ba); arrowDarknut_state_uninitialized_hook(gb); return; // jp
}

// 0d:49ba, bare global.
void moblin_state_scentSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49ba, 0x49bd); A = mem_rd(gb, wScentSeedActive);
  CYC(0x49bd, 0x49be); alu_or(gb, A);
  if (F & FZ) { CYCT(0x49be, 0x49c1); arrowDarknut_setState8WithRandomAngleAndCounter_hook(gb); return; } // jp z
  CYC(0x49be, 0x49c1);
  CALL_C(0x49c1, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x49c4);
  CYC(0x49c4, 0x49c6); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x49c6, 0x49c7); A = mem_rd(gb, DE);
  CYC(0x49c7, 0x49c9); alu_add(gb, 0x04);
  CYC(0x49c9, 0x49cb); alu_and(gb, 0x18);
  CYC(0x49cb, 0x49cc); mem_wr(gb, DE, A);
  CALL_C(0x49cc, ecom_updateAnimationFromAngle_b0d_hook, 0x43d8, 0x49cf);
  CALL_C(0x49cf, ecom_applyVelocityForSideviewEnemy_b0d_hook, 0x4153, 0x49d2);
  CYC(0x49d2, 0x49d5); enemyAnimate_hook(gb); return; // jp
}

// 0d:49d5, bare global (also used by darknuts).
void moblin_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49d5, 0x49d6); E = alu_inc8(gb, E);
  CYC(0x49d6, 0x49d7); A = mem_rd(gb, DE);
  {
    CYC(0x49d7, 0x49d8); push_effect(gb, 0x49d8);
    uint16_t target = enemyCode0c_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x49e0) goto substate1;
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(0x49e0); return; // ret
}

// 0d:49e1, bare global.
void moblin_state_stub_hook(GB *gb) {
  RET(0x49e1); return; // ret
}

// 0d:49e2, bare global (also darknut state 8).
void moblin_state_8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49e2, ecom_decCounter1_b0d_hook, 0x439a, 0x49e5);
  if (F & FZ) { CYCT(0x49e5, 0x49e7); goto incState; } // jr z
  CYC(0x49e5, 0x49e7);
  CALL_C(0x49e7, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x49ea);
  if (!(F & FZ)) { CYCT(0x49ea, 0x49ec); goto animate; } // jr nz
  CYC(0x49ea, 0x49ec);

incState:
  CALL_C(0x49ec, ecom_incState_b0d_hook, 0x4000, 0x49ef);
  CYC(0x49ef, 0x49f1); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x49f1, 0x49f3); mem_wr(gb, HL, 0x08);

animate:
  CYC(0x49f3, 0x49f6); enemyAnimate_hook(gb); return; // jp
}

// 0d:49f6, bare global; standing until counter1 reaches 0 and a new direction is decided on.
void moblin_state_9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49f6, ecom_decCounter1_b0d_hook, 0x439a, 0x49f9);
  if (!(F & FZ)) { RET_TAKEN(0x49f9); return; } // ret nz
  CYC(0x49f9, 0x49fa);
  CALL_C(0x49fa, ecom_setRandomCardinalAngle_b0d_hook, 0x43c6, 0x49fd);
  CALL_C(0x49fd, arrowDarknut_setState8WithRandomAngleAndCounter_hook, 0x4a5a, 0x4a00);
  CYC(0x4a00, 0x4a02); arrowDarknut_fireArrowEveryOtherTime_hook(gb); return; // jr
}
