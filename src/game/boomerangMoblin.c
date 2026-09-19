#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode0a_jump_table(GB *gb) {
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

static void enemyCode0a_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_BOOMERANG_MOBLIN
// ==================================================================================================
void enemyCode0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x46c7, ecom_checkHazards_b0d_hook, 0x4051, 0x46ca);
  if (F & FZ) { CYCT(0x46ca, 0x46cc); goto normalStatus; } // jr z
  CYC(0x46ca, 0x46cc);
  CYC(0x46cc, 0x46ce); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x46ce); return; } // ret c
  CYC(0x46ce, 0x46cf);
  if (F & FZ) { CYCT(0x46cf, 0x46d1); goto dead; } // jr z
  CYC(0x46cf, 0x46d1);
  CYC(0x46d1, 0x46d2); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x46d2, 0x46d5); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x46d2, 0x46d5);
  RET(0x46d5); return; // ret

dead:
  CYC(0x46d6, 0x46d8); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x46d8, 0x46d9); A = mem_rd(gb, DE);
  CYC(0x46d9, 0x46da); alu_or(gb, A);
  if (F & FZ) { CYCT(0x46da, 0x46dc); goto skipClearBoomerang; } // jr z
  CYC(0x46da, 0x46dc);
  CYC(0x46dc, 0x46dd); H = A;
  CYC(0x46dd, 0x46df); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(0x46df, 0x46e1); mem_wr(gb, HL, 0xff);

skipClearBoomerang:
  CYC(0x46e1, 0x46e4); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x46e4, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x46e7);
  CYC(0x46e7, 0x46e9); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x46e9, 0x46ea); A = mem_rd(gb, DE);
  {
    CYC(0x46ea, 0x46eb); push_effect(gb, 0x46eb);
    uint16_t target = enemyCode0a_jump_table(gb);
    if (target == 0x4701) goto state_uninitialized;
    if (target == 0x4739) goto state_stub;
    if (target == 0x4728) goto state_switchHook;
    if (target == 0x470d) goto state_scentSeed;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x473a) goto state_8;
    if (target == 0x474c) goto state_9;
    if (target == 0x476a) goto state_a;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x4701, 0x4703); A = 0x14; // SPEED_80
  CALL_C(0x4703, ecom_setSpeedAndState8AndVisible_b0d_hook, 0x435e, 0x4706);
  CYC(0x4706, 0x4708); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x4708, 0x470a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x470a, 0x470d); goto gotoState8WithRandomAngleAndCounter; // jp

state_scentSeed:
  CYC(0x470d, 0x4710); A = mem_rd(gb, wScentSeedActive);
  CYC(0x4710, 0x4711); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4711, 0x4714); goto gotoState8WithRandomAngleAndCounter; } // jp z
  CYC(0x4711, 0x4714);
  CALL_C(0x4714, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x4717);
  CYC(0x4717, 0x4719); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4719, 0x471a); A = mem_rd(gb, DE);
  CYC(0x471a, 0x471c); alu_add(gb, 0x04);
  CYC(0x471c, 0x471e); alu_and(gb, 0x18);
  CYC(0x471e, 0x471f); mem_wr(gb, DE, A);
  CALL_C(0x471f, ecom_updateAnimationFromAngle_b0d_hook, 0x43d8, 0x4722);
  CALL_C(0x4722, ecom_applyVelocityForSideviewEnemy_b0d_hook, 0x4153, 0x4725);
  CYC(0x4725, 0x4728); enemyAnimate_hook(gb); return; // jp

state_switchHook:
  CYC(0x4728, 0x4729); E = alu_inc8(gb, E);
  CYC(0x4729, 0x472a); A = mem_rd(gb, DE);
  {
    CYC(0x472a, 0x472b); push_effect(gb, 0x472b);
    uint16_t target = enemyCode0a_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x4733) goto switchHook_substate1;
    if (target == 0x4734) goto switchHook_substate3;
    HANDOFF(target);
  }

switchHook_substate1:
  RET(0x4733); return; // ret

switchHook_substate3:
  CYC(0x4734, 0x4736); B = 0x0a;
  CYC(0x4736, 0x4739); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp

state_stub:
  RET(0x4739); return; // ret

state_8:
  CALL_C(0x473a, ecom_decCounter1_b0d_hook, 0x439a, 0x473d);
  if (F & FZ) { CYCT(0x473d, 0x473f); goto state_8_setState9; } // jr z
  CYC(0x473d, 0x473f);
  CALL_C(0x473f, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x4742);
  if (!(F & FZ)) { CYCT(0x4742, 0x4744); goto state_8_animate; } // jr nz
  CYC(0x4742, 0x4744);

state_8_setState9:
  CYC(0x4744, 0x4746); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4746, 0x4748); A = 0x09;
  CYC(0x4748, 0x4749); mem_wr(gb, DE, A);

state_8_animate:
  CYC(0x4749, 0x474c); enemyAnimate_hook(gb); return; // jp

state_9:
  CYC(0x474c, 0x474f); push_effect(gb, 0x474f); goto gotoState8WithRandomAngleAndCounter;
state_9_resume: // resumes at 0x474f after the manual call above returns
  CALL_C(0x474f, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4752);
  CYC(0x4752, 0x4754); alu_add(gb, 0x04);
  CYC(0x4754, 0x4756); alu_and(gb, 0x18);
  CYC(0x4756, 0x4758); A = alu_swap(gb, A);
  CYC(0x4758, 0x4759); alu_rlca(gb);
  CYC(0x4759, 0x475a); H = D;
  CYC(0x475a, 0x475c); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x475c, 0x475d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x475d); return; } // ret nz
  CYC(0x475d, 0x475e);
  CYC(0x475e, 0x4760); B = 0x21; // PART_MOBLIN_BOOMERANG
  CALL_C(0x4760, ecom_spawnProjectile_b0d_hook, 0x437c, 0x4763);
  if (!(F & FZ)) { RET_TAKEN(0x4763); return; } // ret nz
  CYC(0x4763, 0x4764);
  CYC(0x4764, 0x4765); H = D;
  CYC(0x4765, 0x4767); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4767, 0x4769); mem_wr(gb, HL, 0x0a);
  RET(0x4769); return; // ret

state_a:
  CYC(0x476a, 0x476c); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x476c, 0x476d); A = mem_rd(gb, DE);
  CYC(0x476d, 0x476e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x476e, 0x4770); goto state_8_animate; } // jr nz
  CYC(0x476e, 0x4770);

gotoState8WithRandomAngleAndCounter:
  CALL_C(0x4770, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4773);
  CYC(0x4773, 0x4775); alu_and(gb, 0x03);
  CYC(0x4775, 0x4778); SET_HL(0x4788); // @counterVals
  CYC(0x4778, 0x4779); enemyCode0a_addAToHl_from_rst(gb, 0x4779);
  CYC(0x4779, 0x477b); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x477b, 0x477c); A = mem_rd(gb, HL);
  CYC(0x477c, 0x477d); mem_wr(gb, DE, A);
  CYC(0x477d, 0x477f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x477f, 0x4781); A = 0x08;
  CYC(0x4781, 0x4782); mem_wr(gb, DE, A);
  CALL_C(0x4782, ecom_setRandomCardinalAngle_b0d_hook, 0x43c6, 0x4785);
  CYC(0x4785, 0x4788); ecom_updateAnimationFromAngle_b0d_hook(gb);
  if (gb->pc == 0x474f && gb->sp == sp0_) goto state_9_resume;
  return; // jp
}
