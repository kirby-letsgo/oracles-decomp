#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void octorok_state_uninitialized_hook(GB *gb);
void octorok_state_followingScentSeed_hook(GB *gb);
void octorok_state_latchedBySwitchHook_hook(GB *gb);
void octorok_state_stub_hook(GB *gb);
void octorok_state_08_hook(GB *gb);
void octorok_state_09_hook(GB *gb);
void octorok_state_0a_hook(GB *gb);
void octorok_state_0b_hook(GB *gb);

static uint16_t octorok_jump_table(GB *gb) {
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

static void octorok_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_OCTOROK
// ==================================================================================================
void enemyCode09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x458c, ecom_checkHazards_b0d_hook, 0x4051, 0x458f);
  if (F & FZ) { CYCT(0x458f, 0x4591); goto normalStatus; } // jr z
  CYC(0x458f, 0x4591);
  CYC(0x4591, 0x4593); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4593); return; } // ret c
  CYC(0x4593, 0x4594);
  if (F & FZ) { CYCT(0x4594, 0x4596); goto dead; } // jr z
  CYC(0x4594, 0x4596);
  CYC(0x4596, 0x4597); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4597, 0x459a); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x4597, 0x459a);
  RET(0x459a); return; // ret

dead:
  CYC(0x459b, 0x459d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x459d, 0x459e); A = mem_rd(gb, DE);
  CYC(0x459e, 0x45a0); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x45a0, 0x45a2); goto die; } // jr nz
  CYC(0x45a0, 0x45a2);
  CYC(0x45a2, 0x45a5); SET_HL(wKilledGoldenEnemies);
  CYC(0x45a5, 0x45a7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)

die:
  CYC(0x45a7, 0x45aa); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x45aa, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x45ad);
  CYC(0x45ad, 0x45af); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x45af, 0x45b0); A = mem_rd(gb, DE);
  {
    CYC(0x45b0, 0x45b1); push_effect(gb, 0x45b1);
    uint16_t target = octorok_jump_table(gb);
    if (target == 0x45c9) { octorok_state_uninitialized_hook(gb); return; }
    if (target == 0x4642) { octorok_state_stub_hook(gb); return; }
    if (target == 0x4636) { octorok_state_latchedBySwitchHook_hook(gb); return; }
    if (target == 0x4618) { octorok_state_followingScentSeed_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x4643) { octorok_state_08_hook(gb); return; }
    if (target == 0x4674) { octorok_state_09_hook(gb); return; }
    if (target == 0x469b) { octorok_state_0a_hook(gb); return; }
    if (target == 0x46b3) { octorok_state_0b_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:45c9, bare global (also entered as a jump-table target from enemyCode09).
void octorok_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x45c9, 0x45cb); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x45cb, 0x45cc); A = mem_rd(gb, DE);
  CYC(0x45cc, 0x45ce); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x45ce, 0x45d0); goto notGoldenOctorok; } // jr nz
  CYC(0x45ce, 0x45d0);
  CYC(0x45d0, 0x45d3); SET_HL(wKilledGoldenEnemies);
  CYC(0x45d3, 0x45d5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x45d5, 0x45d8); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x45d5, 0x45d8);

notGoldenOctorok:
  CYC(0x45d8, 0x45d9); alu_rrca(gb);
  CYC(0x45d9, 0x45db); A = 0x14; // SPEED_80
  if (!(F & FC)) { CYCT(0x45db, 0x45dd); goto setSpeed; } // jr nc
  CYC(0x45db, 0x45dd);
  CYC(0x45dd, 0x45df); A = 0x1e; // SPEED_c0

setSpeed:
  CALL_C(0x45df, ecom_setSpeedAndState8AndVisible_b0d_hook, 0x435e, 0x45e2);
  CYC(0x45e2, 0x45e4); mem_wr(gb, HL, 0x0a); // [state] = $0a
  CYC(0x45e4, 0x45e6); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x45e6, 0x45e8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x45e8, 0x45ea); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x45ea, 0x45eb); A = mem_rd(gb, DE);
  CYC(0x45eb, 0x45ee); SET_HL(0x4613); // @counter1Ranges (data)
  CYC(0x45ee, 0x45ef); octorok_addAToHl_from_rst(gb, 0x45ef);
  CYC(0x45ef, 0x45f1); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x45f1, 0x45f2); A = mem_rd(gb, HL);
  CYC(0x45f2, 0x45f3); mem_wr(gb, DE, A);
  CYC(0x45f3, 0x45f4); E = A;
  CYC(0x45f4, 0x45f7); SET_BC(0x1803);
  CALL_C(0x45f7, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x45fa);
  CYC(0x45fa, 0x45fb); A = E;
  CYC(0x45fb, 0x45fe); SET_HL(0x466c); // octorok_counter1Values (data)
  CYC(0x45fe, 0x45ff); octorok_addAToHl_from_rst(gb, 0x45ff);
  CYC(0x45ff, 0x4601); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4601, 0x4602); A = mem_rd(gb, HL);
  CYC(0x4602, 0x4603); mem_wr(gb, DE, A);
  CYC(0x4603, 0x4605); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4605, 0x4606); A = B;
  CYC(0x4606, 0x4607); mem_wr(gb, DE, A);
  CYC(0x4607, 0x4608); A = C;
  CYC(0x4608, 0x460b); SET_HL(0x4697); // octorok_walkCounterValues (data)
  CYC(0x460b, 0x460c); octorok_addAToHl_from_rst(gb, 0x460c);
  CYC(0x460c, 0x460e); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x460e, 0x460f); A = mem_rd(gb, HL);
  CYC(0x460f, 0x4610); mem_wr(gb, DE, A);
  CYC(0x4610, 0x4613); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp
}

// 0d:4618, bare global.
void octorok_state_followingScentSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4618, 0x461b); A = mem_rd(gb, wScentSeedActive);
  CYC(0x461b, 0x461c); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x461c, 0x461e); goto seek; } // jr nz
  CYC(0x461c, 0x461e);
  CYC(0x461e, 0x4620); A = 0x08;
  CYC(0x4620, 0x4621); mem_wr(gb, DE, A); // [state] = 8
  RET(0x4621); return; // ret

seek:
  CALL_C(0x4622, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x4625);
  CYC(0x4625, 0x4627); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4627, 0x4628); A = mem_rd(gb, DE);
  CYC(0x4628, 0x462a); alu_add(gb, 0x04);
  CYC(0x462a, 0x462c); alu_and(gb, 0x18);
  CYC(0x462c, 0x462d); mem_wr(gb, DE, A);
  CALL_C(0x462d, ecom_updateAnimationFromAngle_b0d_hook, 0x43d8, 0x4630);
  CALL_C(0x4630, ecom_applyVelocityForTopDownEnemy_b0d_hook, 0x4146, 0x4633);
  CYC(0x4633, 0x4636); enemyAnimate_hook(gb); return; // jp
}

// 0d:4636, bare global.
void octorok_state_latchedBySwitchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4636, 0x4637); E = alu_inc8(gb, E);
  CYC(0x4637, 0x4638); A = mem_rd(gb, DE);
  {
    CYC(0x4638, 0x4639); push_effect(gb, 0x4639);
    uint16_t target = octorok_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x4641) goto substate1;
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(0x4641); return; // ret
}

// 0d:4642, bare global.
void octorok_state_stub_hook(GB *gb) {
  RET(0x4642); return; // ret
}

// 0d:4643, bare global.
void octorok_state_08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4643, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4646);
  CYC(0x4646, 0x4647); H = D;
  CYC(0x4647, 0x4649); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4649, 0x464a); alu_and(gb, mem_rd(gb, HL));
  CYC(0x464a, 0x464c); L = ENEMY_BASE + OBJ_STATE;
  if (!(F & FZ)) { CYCT(0x464c, 0x464e); goto standStill; } // jr nz
  CYC(0x464c, 0x464e);
  CYC(0x464e, 0x4650); mem_wr(gb, HL, 0x0b); // [state] = $0b
  CYC(0x4650, 0x4652); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4652, 0x4654); mem_wr(gb, HL, 0x10);
  CYC(0x4654, 0x4656); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4656, 0x4657); A = mem_rd(gb, HL);
  CYC(0x4657, 0x4659); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(0x4659); return; } // ret c
  CYC(0x4659, 0x465a);
  CALL_C(0x465a, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x465d);
  CYC(0x465d, 0x4660); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp

standStill:
  CYC(0x4660, 0x4661); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $09
  CYC(0x4661, 0x4664); SET_BC(0x466c); // octorok_counter1Values (data)
  CALL_C(0x4664, addAToBc_hook, 0x006d, 0x4667);
  CYC(0x4667, 0x4669); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4669, 0x466a); A = mem_rd(gb, BC);
  CYC(0x466a, 0x466b); mem_wr(gb, HL, A);
  RET(0x466b); return; // ret
}

// 0d:4674, bare global.
void octorok_state_09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4674, ecom_decCounter1_b0d_hook, 0x439a, 0x4677);
  if (!(F & FZ)) { RET_TAKEN(0x4677); return; } // ret nz
  CYC(0x4677, 0x4678);
  CYC(0x4678, 0x4679); L = E;
  CYC(0x4679, 0x467a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x467a, 0x467c); E = 0x03;
  CYC(0x467c, 0x467f); SET_BC(0x0318);
  CALL_C(0x467f, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x4682);
  CYC(0x4682, 0x4683); A = E;
  CYC(0x4683, 0x4686); SET_HL(0x4697); // octorok_walkCounterValues (data)
  CYC(0x4686, 0x4687); octorok_addAToHl_from_rst(gb, 0x4687);
  CYC(0x4687, 0x4688); A = mem_rd(gb, HL);
  CYC(0x4688, 0x468a); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x468a, 0x468b); mem_wr(gb, DE, A);
  CYC(0x468b, 0x468d); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x468d, 0x468e); A = C;
  CYC(0x468e, 0x468f); mem_wr(gb, DE, A);
  CYC(0x468f, 0x4690); A = B;
  CYC(0x4690, 0x4691); alu_or(gb, A);
  if (F & FZ) { CALL_C(0x4691, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x4694); } else { CYC(0x4691, 0x4694); } // call z
  CYC(0x4694, 0x4697); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp
}

// 0d:469b, bare global.
void octorok_state_0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x469b, 0x469c); H = D;
  CYC(0x469c, 0x469e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x469e, 0x469f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x469f, 0x46a1); goto keepWalking; } // jr nz
  CYC(0x469f, 0x46a1);
  CYC(0x46a1, 0x46a2); L = E;
  CYC(0x46a2, 0x46a4); mem_wr(gb, HL, 0x08); // [state] = $08
  RET(0x46a4); return; // ret

keepWalking:
  CALL_C(0x46a5, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x46a8);
  if (!(F & FZ)) { CYCT(0x46a8, 0x46aa); goto animate; } // jr nz
  CYC(0x46a8, 0x46aa);
  CALL_C(0x46aa, ecom_setRandomCardinalAngle_b0d_hook, 0x43c6, 0x46ad);
  CALL_C(0x46ad, ecom_updateAnimationFromAngle_b0d_hook, 0x43d8, 0x46b0);

animate:
  CYC(0x46b0, 0x46b3); enemyAnimate_hook(gb); return; // jp
}

// 0d:46b3, bare global.
void octorok_state_0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x46b3, ecom_decCounter1_b0d_hook, 0x439a, 0x46b6);
  if (!(F & FZ)) { RET_TAKEN(0x46b6); return; } // ret nz
  CYC(0x46b6, 0x46b7);
  CYC(0x46b7, 0x46b9); mem_wr(gb, HL, 0x20); // [counter1] = $20
  CYC(0x46b9, 0x46ba); L = E;
  CYC(0x46ba, 0x46bc); mem_wr(gb, HL, 0x09); // [state] = $09
  CYC(0x46bc, 0x46be); B = 0x18; // PART_OCTOROK_PROJECTILE
  CALL_C(0x46be, ecom_spawnProjectile_b0d_hook, 0x437c, 0x46c1);
  if (!(F & FZ)) { RET_TAKEN(0x46c1); return; } // ret nz
  CYC(0x46c1, 0x46c2);
  CYC(0x46c2, 0x46c4); A = 0x51; // SND_THROW
  CYC(0x46c4, 0x46c7); playSound_b00_hook(gb); return; // jp
}
