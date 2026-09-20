#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode0d_jump_table(GB *gb) {
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

static void enemyCode0d_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_LYNEL
// ==================================================================================================
void enemyCode0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a77, ecom_checkHazards_b0d_hook, 0x4051, 0x4a7a);
  if (F & FZ) { CYCT(0x4a7a, 0x4a7c); goto normalStatus; } // jr z
  CYC(0x4a7a, 0x4a7c);
  CYC(0x4a7c, 0x4a7e); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4a7e); return; } // ret c
  CYC(0x4a7e, 0x4a7f);
  if (F & FZ) { CYCT(0x4a7f, 0x4a81); goto dead; } // jr z
  CYC(0x4a7f, 0x4a81);
  CYC(0x4a81, 0x4a82); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4a82, 0x4a85); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x4a82, 0x4a85);
  RET(0x4a85); return; // ret

dead:
  CYC(0x4a86, 0x4a88); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4a88, 0x4a89); A = mem_rd(gb, DE);
  CYC(0x4a89, 0x4a8b); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4a8b, 0x4a8d); goto die; } // jr nz
  CYC(0x4a8b, 0x4a8d);
  CYC(0x4a8d, 0x4a90); SET_HL(wKilledGoldenEnemies);
  CYC(0x4a90, 0x4a92); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3))); // set 3,(hl)

die:
  CYC(0x4a92, 0x4a95); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x4a95, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x4a98);
  if (F & FZ) { CYCT(0x4a98, 0x4a9a); goto afterSpeedBump; } // jr z
  CYC(0x4a98, 0x4a9a);
  CYC(0x4a9a, 0x4a9c); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4a9c, 0x4a9e); A = 0x28; // SPEED_100
  CYC(0x4a9e, 0x4a9f); mem_wr(gb, DE, A);

afterSpeedBump:
  CYC(0x4a9f, 0x4aa1); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4aa1, 0x4aa2); A = mem_rd(gb, DE);
  {
    CYC(0x4aa2, 0x4aa3); push_effect(gb, 0x4aa3);
    uint16_t target = enemyCode0d_jump_table(gb);
    if (target == 0x4ab9) goto state_uninitialized;
    if (target == 0x4b06) goto state_stub;
    if (target == 0x4ae9) goto state_scentSeed;
    if (target == 0x4b07) goto state_08;
    if (target == 0x4b2c) goto state_09;
    if (target == 0x4b39) goto state_0a;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x4ab9, 0x4abb); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4abb, 0x4abc); A = mem_rd(gb, DE);
  CYC(0x4abc, 0x4abe); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4abe, 0x4ac0); goto notGolden; } // jr nz
  CYC(0x4abe, 0x4ac0);
  CYC(0x4ac0, 0x4ac3); SET_HL(wKilledGoldenEnemies);
  CYC(0x4ac3, 0x4ac5); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4ac5, 0x4ac8); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x4ac5, 0x4ac8);

notGolden:
  CYC(0x4ac8, 0x4aca); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4aca, 0x4acb); A = mem_rd(gb, DE);
  CYC(0x4acb, 0x4ace); SET_HL(0x4ae6); // @var30Vals (data)
  CYC(0x4ace, 0x4acf); enemyCode0d_addAToHl_from_rst(gb, 0x4acf);
  CYC(0x4acf, 0x4ad1); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4ad1, 0x4ad2); A = mem_rd(gb, HL);
  CYC(0x4ad2, 0x4ad3); mem_wr(gb, DE, A);
  CALL_C(0x4ad3, objectSetVisiblec2_hook, 0x1e45, 0x4ad6);
  CALL_C(0x4ad6, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4ad9);
  CYC(0x4ad9, 0x4adb); alu_and(gb, 0x30);
  CYC(0x4adb, 0x4adc); C = A;
  CYC(0x4adc, 0x4add); H = D;
  CYC(0x4add, 0x4adf); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x4adf, 0x4ae1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x4ae1, 0x4ae3); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ae3, 0x4ae6); goto changeDirection; // jp

state_scentSeed:
  CYC(0x4ae9, 0x4aec); A = mem_rd(gb, wScentSeedActive);
  CYC(0x4aec, 0x4aed); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4aed, 0x4af0); goto gotoState8; } // jp z
  CYC(0x4aed, 0x4af0);
  CALL_C(0x4af0, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x4af3);
  CYC(0x4af3, 0x4af5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4af5, 0x4af6); A = mem_rd(gb, DE);
  CYC(0x4af6, 0x4af8); alu_add(gb, 0x04);
  CYC(0x4af8, 0x4afa); alu_and(gb, 0x18);
  CYC(0x4afa, 0x4afb); mem_wr(gb, DE, A);
  CYC(0x4afb, 0x4afd); B = 0x04;
  CYC(0x4afd, 0x4b00); push_effect(gb, 0x4b00); goto updateAnimationFromAngle;
state_scentSeed_resume: // resumes at 0x4b00
  CALL_C(0x4b00, ecom_applyVelocityForSideviewEnemy_b0d_hook, 0x4153, 0x4b03);
  CYC(0x4b03, 0x4b06); enemyAnimate_hook(gb); return; // jp

state_stub:
  RET(0x4b06); return; // ret

state_08:
  CYC(0x4b07, 0x4b09); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b09, 0x4b0a); A = mem_rd(gb, DE);
  CYC(0x4b0a, 0x4b0b); B = A;
  CYC(0x4b0b, 0x4b0d); C = 0x30;
  CALL_C(0x4b0d, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x4b10);
  CYC(0x4b10, 0x4b11); alu_or(gb, B);
  CYC(0x4b11, 0x4b12); H = D;
  CYC(0x4b12, 0x4b14); L = ENEMY_BASE + OBJ_STATE;
  if (F & FZ) { CYCT(0x4b14, 0x4b16); goto prepareProjectile; } // jr z
  CYC(0x4b14, 0x4b16);

changeDirection:
  CYC(0x4b16, 0x4b18); mem_wr(gb, HL, 0x09); // [state] = $09
  CYC(0x4b18, 0x4b1a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b1a, 0x4b1c); A = 0x30;
  CYC(0x4b1c, 0x4b1d); alu_add(gb, C);
  CYC(0x4b1d, 0x4b1e); mem_wr(gb, HL, A);
  CYC(0x4b1e, 0x4b20); goto updateAngleAndSpeed; // jr

prepareProjectile:
  CYC(0x4b20, 0x4b22); mem_wr(gb, HL, 0x0a); // [state] = $0a
  CYC(0x4b22, 0x4b24); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b24, 0x4b26); mem_wr(gb, HL, 0x08);
  CALL_C(0x4b26, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x4b29);
  CYC(0x4b29, 0x4b2c); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp

state_09:
  CALL_C(0x4b2c, ecom_decCounter1_b0d_hook, 0x439a, 0x4b2f);
  if (F & FZ) { CYCT(0x4b2f, 0x4b31); goto gotoState8; } // jr z
  CYC(0x4b2f, 0x4b31);
  CALL_C(0x4b31, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x4b34);
  if (F & FZ) { CYCT(0x4b34, 0x4b36); goto updateAngleAndSpeed; } // jr z
  CYC(0x4b34, 0x4b36);

animate:
  CYC(0x4b36, 0x4b39); enemyAnimate_hook(gb); return; // jp

state_0a:
  CALL_C(0x4b39, ecom_decCounter1_b0d_hook, 0x439a, 0x4b3c);
  if (!(F & FZ)) { CYCT(0x4b3c, 0x4b3e); goto animate; } // jr nz
  CYC(0x4b3c, 0x4b3e);
  CYC(0x4b3e, 0x4b40); B = 0x1b; // PART_LYNEL_BEAM
  CALL_C(0x4b40, ecom_spawnProjectile_b0d_hook, 0x437c, 0x4b43);
  if (!(F & FZ)) { CYCT(0x4b43, 0x4b45); goto gotoState8; } // jr nz
  CYC(0x4b43, 0x4b45);
  CALL_C(0x4b45, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4b48);
  CYC(0x4b48, 0x4b4a); alu_and(gb, 0x30);
  CYC(0x4b4a, 0x4b4c); alu_add(gb, 0x30);
  CYC(0x4b4c, 0x4b4e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b4e, 0x4b4f); mem_wr(gb, DE, A);
  CYC(0x4b4f, 0x4b50); H = D;
  CYC(0x4b50, 0x4b52); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4b52, 0x4b54); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x4b54, 0x4b56); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4b56, 0x4b58); mem_wr(gb, HL, 0x09);
  CYC(0x4b58, 0x4b5a); goto animate; // jr

gotoState8:
  CYC(0x4b5a, 0x4b5c); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4b5c, 0x4b5e); A = 0x08;
  CYC(0x4b5e, 0x4b5f); mem_wr(gb, DE, A);
  CYC(0x4b5f, 0x4b61); goto animate; // jr

updateAngleAndSpeed:
  CYC(0x4b61, 0x4b64); push_effect(gb, 0x4b64); goto chooseNewAngle;
updateAngleAndSpeed_resume: // resumes at 0x4b64
  CYC(0x4b64, 0x4b66); B = 0x0e;
  CALL_C(0x4b66, objectCheckCenteredWithLink_hook, 0x1fee, 0x4b69);
  if (!(F & FC)) { CYCT(0x4b69, 0x4b6b); goto notCentered; } // jr nc
  CYC(0x4b69, 0x4b6b);
  CALL_C(0x4b6b, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4b6e);
  CYC(0x4b6e, 0x4b70); alu_add(gb, 0x04);
  CYC(0x4b70, 0x4b72); alu_and(gb, 0x18);
  CYC(0x4b72, 0x4b73); H = D;
  CYC(0x4b73, 0x4b75); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4b75, 0x4b76); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x4b76, 0x4b78); A = 0x28; // SPEED_100
  CYC(0x4b78, 0x4b7a); B = 0x04;
  if (F & FZ) { CYCT(0x4b7a, 0x4b7c); goto setSpeed; } // jr z
  CYC(0x4b7a, 0x4b7c);

notCentered:
  CYC(0x4b7c, 0x4b7e); A = 0x14; // SPEED_80
  CYC(0x4b7e, 0x4b80); B = 0x00;

setSpeed:
  CYC(0x4b80, 0x4b82); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4b82, 0x4b83); mem_wr(gb, HL, A);

updateAnimationFromAngle:
  CYC(0x4b83, 0x4b84); H = D;
  CYC(0x4b84, 0x4b86); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4b86, 0x4b87); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4b87, 0x4b89); A = alu_swap(gb, A);
  CYC(0x4b89, 0x4b8a); alu_rlca(gb);
  CYC(0x4b8a, 0x4b8b); alu_add(gb, B);
  CYC(0x4b8b, 0x4b8c); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    RET_TAKEN(0x4b8c);
    if (gb->pc == 0x4b00 && gb->sp == sp0_) goto state_scentSeed_resume;
    return;
  } // ret z
  CYC(0x4b8c, 0x4b8d);
  CYC(0x4b8d, 0x4b8e); mem_wr(gb, HL, A);
  CYC(0x4b8e, 0x4b91); enemySetAnimation_hook(gb);
  if (gb->pc == 0x4b00 && gb->sp == sp0_) goto state_scentSeed_resume;
  return; // jp

chooseNewAngle:
  CALL_C(0x4b91, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4b94);
  CYC(0x4b94, 0x4b95); H = D;
  CYC(0x4b95, 0x4b97); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b97, 0x4b98); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4b98, 0x4b9b);
    ecom_setRandomCardinalAngle_b0d_hook(gb);
    if (gb->pc == 0x4b64 && gb->sp == sp0_) goto updateAngleAndSpeed_resume;
    return;
  } // jp nz
  CYC(0x4b98, 0x4b9b);
  CYC(0x4b9b, 0x4b9e); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb);
  if (gb->pc == 0x4b64 && gb->sp == sp0_) goto updateAngleAndSpeed_resume;
  return; // jp
}
