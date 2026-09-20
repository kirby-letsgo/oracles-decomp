#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode09);
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
  CYC(b_+18, b_+20); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto die; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(wKilledGoldenEnemies);
  CYC(b_+25, b_+27); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)

die:
  CYC(b_+27, b_+30); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+30, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+33);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  {
    CYC(b_+36, b_+37); push_effect(gb, b_+37);
    uint16_t target = octorok_jump_table(gb);
    if (target == SYM(octorok_state_uninitialized)) { octorok_state_uninitialized_hook(gb); return; }
    if (target == SYM(octorok_state_stub)) { octorok_state_stub_hook(gb); return; }
    if (target == SYM(octorok_state_latchedBySwitchHook)) { octorok_state_latchedBySwitchHook_hook(gb); return; }
    if (target == SYM(octorok_state_followingScentSeed)) { octorok_state_followingScentSeed_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(octorok_state_08)) { octorok_state_08_hook(gb); return; }
    if (target == SYM(octorok_state_09)) { octorok_state_09_hook(gb); return; }
    if (target == SYM(octorok_state_0a)) { octorok_state_0a_hook(gb); return; }
    if (target == SYM(octorok_state_0b)) { octorok_state_0b_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:45c9, bare global (also entered as a jump-table target from enemyCode09).
void octorok_state_uninitialized_hook(GB *gb) {
  BASE(octorok_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto notGoldenOctorok; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wKilledGoldenEnemies);
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+12, b_+15); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);

notGoldenOctorok:
  CYC(b_+15, b_+16); alu_rrca(gb);
  CYC(b_+16, b_+18); A = 0x14; // SPEED_80
  if (!(F & FC)) { CYCT(b_+18, b_+20); goto setSpeed; } // jr nc
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); A = 0x1e; // SPEED_c0

setSpeed:
  CALL_C(b_+22, ecom_setSpeedAndState8AndVisible_b0d_hook, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+25);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x0a); // [state] = $0a
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+29, b_+31); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+37); SET_HL(b_+74); // @counter1Ranges (data)
  CYC(b_+37, b_+38); octorok_addAToHl_from_rst(gb, b_+38);
  CYC(b_+38, b_+40); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); E = A;
  CYC(b_+43, b_+46); SET_BC(0x1803);
  CALL_C(b_+46, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+49);
  CYC(b_+49, b_+50); A = E;
  CYC(b_+50, b_+53); SET_HL(SYM(octorok_counter1Values)); // octorok_counter1Values (data)
  CYC(b_+53, b_+54); octorok_addAToHl_from_rst(gb, b_+54);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+60); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+61); A = B;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+63); A = C;
  CYC(b_+63, b_+66); SET_HL(SYM(octorok_walkCounterValues)); // octorok_walkCounterValues (data)
  CYC(b_+66, b_+67); octorok_addAToHl_from_rst(gb, b_+67);
  CYC(b_+67, b_+69); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+69, b_+70); A = mem_rd(gb, HL);
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+74); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp
}

// 0d:4618, bare global.
void octorok_state_followingScentSeed_hook(GB *gb) {
  BASE(octorok_state_followingScentSeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto seek; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x08;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A); // [state] = 8
  RET(b_+9); return; // ret

seek:
  CALL_C(b_+10, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_add(gb, 0x04);
  CYC(b_+18, b_+20); alu_and(gb, 0x18);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CALL_C(b_+21, ecom_updateAnimationFromAngle_b0d_hook, SYM(ecom_updateAnimationFromAngle_b0d), b_+24);
  CALL_C(b_+24, ecom_applyVelocityForTopDownEnemy_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemy_b0d), b_+27);
  CYC(b_+27, b_+30); enemyAnimate_hook(gb); return; // jp
}

// 0d:4636, bare global.
void octorok_state_latchedBySwitchHook_hook(GB *gb) {
  BASE(octorok_state_latchedBySwitchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = octorok_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+11) goto substate1;
    if (target == SYM(ecom_fallToGroundAndSetState8_b0d)) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(b_+11); return; // ret
}

// 0d:4642, bare global.
void octorok_state_stub_hook(GB *gb) {
  BASE(octorok_state_stub);
  RET(b_+0); return; // ret
}

// 0d:4643, bare global.
void octorok_state_08_hook(GB *gb) {
  BASE(octorok_state_08);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+6, b_+7); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto standStill; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x0b); // [state] = $0b
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x10);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+22); return; } // ret c
  CYC(b_+22, b_+23);
  CALL_C(b_+23, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+26);
  CYC(b_+26, b_+29); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp

standStill:
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $09
  CYC(b_+30, b_+33); SET_BC(SYM(octorok_counter1Values)); // octorok_counter1Values (data)
  CALL_C(b_+33, addAToBc_hook, 0x006d, b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+39); A = mem_rd(gb, BC);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  RET(b_+40); return; // ret
}

// 0d:4674, bare global.
void octorok_state_09_hook(GB *gb) {
  BASE(octorok_state_09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+6, b_+8); E = 0x03;
  CYC(b_+8, b_+11); SET_BC(0x0318);
  CALL_C(b_+11, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+14);
  CYC(b_+14, b_+15); A = E;
  CYC(b_+15, b_+18); SET_HL(SYM(octorok_walkCounterValues)); // octorok_walkCounterValues (data)
  CYC(b_+18, b_+19); octorok_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+25, b_+26); A = C;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CALL_C(b_+29, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+32); } else { CYC(b_+29, b_+32); } // call z
  CYC(b_+32, b_+35); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp
}

// 0d:469b, bare global.
void octorok_state_0a_hook(GB *gb) {
  BASE(octorok_state_0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto keepWalking; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08); // [state] = $08
  RET(b_+9); return; // ret

keepWalking:
  CALL_C(b_+10, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto animate; } // jr nz
  CYC(b_+13, b_+15);
  CALL_C(b_+15, ecom_setRandomCardinalAngle_b0d_hook, SYM(ecom_setRandomCardinalAngle_b0d), b_+18);
  CALL_C(b_+18, ecom_updateAnimationFromAngle_b0d_hook, SYM(ecom_updateAnimationFromAngle_b0d), b_+21);

animate:
  CYC(b_+21, b_+24); enemyAnimate_hook(gb); return; // jp
}

// 0d:46b3, bare global.
void octorok_state_0b_hook(GB *gb) {
  BASE(octorok_state_0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x20); // [counter1] = $20
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x09); // [state] = $09
  CYC(b_+9, b_+11); B = 0x18; // PART_OCTOROK_PROJECTILE
  CALL_C(b_+11, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x51; // SND_THROW
  CYC(b_+17, b_+20); playSound_b00_hook(gb); return; // jp
}
