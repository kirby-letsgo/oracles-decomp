#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode0a);
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
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+19, b_+21); goto skipClearBoomerang; } // jr z
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); H = A;
  CYC(b_+22, b_+24); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xff);

skipClearBoomerang:
  CYC(b_+26, b_+29); TAIL(enemyDie); // jp

normalStatus:
  CALL_C(b_+29, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+32);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  {
    CYC(b_+35, b_+36); push_effect(gb, b_+36);
    uint16_t target = enemyCode0a_jump_table(gb);
    if (target == b_+58) goto state_uninitialized;
    if (target == b_+114) goto state_stub;
    if (target == b_+97) goto state_switchHook;
    if (target == b_+70) goto state_scentSeed;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == b_+115) goto state_8;
    if (target == b_+133) goto state_9;
    if (target == b_+163) goto state_a;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+58, b_+60); A = 0x14; // SPEED_80
  CALL_C(b_+60, ecom_setSpeedAndState8AndVisible_b0d_hook, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+63);
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+65, b_+67); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+67, b_+70); goto gotoState8WithRandomAngleAndCounter; // jp

state_scentSeed:
  CYC(b_+70, b_+73); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+73, b_+74); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+74, b_+77); goto gotoState8WithRandomAngleAndCounter; } // jp z
  CYC(b_+74, b_+77);
  CALL_C(b_+77, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+80);
  CYC(b_+80, b_+82); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+85); alu_add(gb, 0x04);
  CYC(b_+85, b_+87); alu_and(gb, 0x18);
  CYC(b_+87, b_+88); mem_wr(gb, DE, A);
  CALL_C(b_+88, ecom_updateAnimationFromAngle_b0d_hook, SYM(ecom_updateAnimationFromAngle_b0d), b_+91);
  CALL_C(b_+91, ecom_applyVelocityForSideviewEnemy_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0d), b_+94);
  CYC(b_+94, b_+97); TAIL(enemyAnimate); // jp

state_switchHook:
  CYC(b_+97, b_+98); E = alu_inc8(gb, E);
  CYC(b_+98, b_+99); A = mem_rd(gb, DE);
  {
    CYC(b_+99, b_+100); push_effect(gb, b_+100);
    uint16_t target = enemyCode0a_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+108) goto switchHook_substate1;
    if (target == b_+109) goto switchHook_substate3;
    HANDOFF(target);
  }

switchHook_substate1:
  RET(b_+108); return; // ret

switchHook_substate3:
  CYC(b_+109, b_+111); B = 0x0a;
  CYC(b_+111, b_+114); TAIL(ecom_fallToGroundAndSetState_b0d); // jp

state_stub:
  RET(b_+114); return; // ret

state_8:
  CALL_C(b_+115, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+118);
  if (F & FZ) { CYCT(b_+118, b_+120); goto state_8_setState9; } // jr z
  CYC(b_+118, b_+120);
  CALL_C(b_+120, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+123);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); goto state_8_animate; } // jr nz
  CYC(b_+123, b_+125);

state_8_setState9:
  CYC(b_+125, b_+127); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+127, b_+129); A = 0x09;
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);

state_8_animate:
  CYC(b_+130, b_+133); TAIL(enemyAnimate); // jp

state_9:
  CYC(b_+133, b_+136); push_effect(gb, b_+136); goto gotoState8WithRandomAngleAndCounter;
state_9_resume: // resumes at 0x474f after the manual call above returns
  CALL_C(b_+136, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+139);
  CYC(b_+139, b_+141); alu_add(gb, 0x04);
  CYC(b_+141, b_+143); alu_and(gb, 0x18);
  CYC(b_+143, b_+145); A = alu_swap(gb, A);
  CYC(b_+145, b_+146); alu_rlca(gb);
  CYC(b_+146, b_+147); H = D;
  CYC(b_+147, b_+149); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+149, b_+150); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+150); return; } // ret nz
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); B = 0x21; // PART_MOBLIN_BOOMERANG
  CALL_C(b_+153, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+156);
  if (!(F & FZ)) { RET_TAKEN(b_+156); return; } // ret nz
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+158); H = D;
  CYC(b_+158, b_+160); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+160, b_+162); mem_wr(gb, HL, 0x0a);
  RET(b_+162); return; // ret

state_a:
  CYC(b_+163, b_+165); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+165, b_+166); A = mem_rd(gb, DE);
  CYC(b_+166, b_+167); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+167, b_+169); goto state_8_animate; } // jr nz
  CYC(b_+167, b_+169);

gotoState8WithRandomAngleAndCounter:
  CALL_C(b_+169, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+172);
  CYC(b_+172, b_+174); alu_and(gb, 0x03);
  CYC(b_+174, b_+177); SET_HL(b_+193); // @counterVals
  CYC(b_+177, b_+178); enemyCode0a_addAToHl_from_rst(gb, b_+178);
  CYC(b_+178, b_+180); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+180, b_+181); A = mem_rd(gb, HL);
  CYC(b_+181, b_+182); mem_wr(gb, DE, A);
  CYC(b_+182, b_+184); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+184, b_+186); A = 0x08;
  CYC(b_+186, b_+187); mem_wr(gb, DE, A);
  CALL_C(b_+187, ecom_setRandomCardinalAngle_b0d_hook, SYM(ecom_setRandomCardinalAngle_b0d), b_+190);
  CYC(b_+190, b_+193); ecom_updateAnimationFromAngle_b0d_hook(gb);
  if (gb->pc == b_+136 && gb->sp == sp0_) goto state_9_resume;
  return; // jp
}
