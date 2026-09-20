#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode0d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode0d), (from), (to), true)

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
  BASE(enemyCode0d);
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
  CYC(b_+25, b_+27); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3))); // set 3,(hl)

die:
  CYC(b_+27, b_+30); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+30, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+33);
  if (F & FZ) { CYCT(b_+33, b_+35); goto afterSpeedBump; } // jr z
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+37, b_+39); A = 0x28; // SPEED_100
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);

afterSpeedBump:
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  {
    CYC(b_+43, b_+44); push_effect(gb, b_+44);
    uint16_t target = enemyCode0d_jump_table(gb);
    if (target == b_+66) goto state_uninitialized;
    if (target == b_+143) goto state_stub;
    if (target == b_+114) goto state_scentSeed;
    if (target == b_+144) goto state_08;
    if (target == b_+181) goto state_09;
    if (target == b_+194) goto state_0a;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+66, b_+68); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+71); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto notGolden; } // jr nz
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+76); SET_HL(wKilledGoldenEnemies);
  CYC(b_+76, b_+78); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+78, b_+81); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+78, b_+81);

notGolden:
  CYC(b_+81, b_+83); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+87); SET_HL(b_+111); // @var30Vals (data)
  CYC(b_+87, b_+88); enemyCode0d_addAToHl_from_rst(gb, b_+88);
  CYC(b_+88, b_+90); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  CALL_C(b_+92, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+95);
  CALL_C(b_+95, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+98);
  CYC(b_+98, b_+100); alu_and(gb, 0x30);
  CYC(b_+100, b_+101); C = A;
  CYC(b_+101, b_+102); H = D;
  CYC(b_+102, b_+104); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+104, b_+106); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+106, b_+108); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+108, b_+111); goto changeDirection; // jp

state_scentSeed:
  CYC(b_+114, b_+117); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+117, b_+118); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+118, b_+121); goto gotoState8; } // jp z
  CYC(b_+118, b_+121);
  CALL_C(b_+121, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+124);
  CYC(b_+124, b_+126); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+129); alu_add(gb, 0x04);
  CYC(b_+129, b_+131); alu_and(gb, 0x18);
  CYC(b_+131, b_+132); mem_wr(gb, DE, A);
  CYC(b_+132, b_+134); B = 0x04;
  CYC(b_+134, b_+137); push_effect(gb, b_+137); goto updateAnimationFromAngle;
state_scentSeed_resume: // resumes at 0x4b00
  CALL_C(b_+137, ecom_applyVelocityForSideviewEnemy_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0d), b_+140);
  CYC(b_+140, b_+143); enemyAnimate_hook(gb); return; // jp

state_stub:
  RET(b_+143); return; // ret

state_08:
  CYC(b_+144, b_+146); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+146, b_+147); A = mem_rd(gb, DE);
  CYC(b_+147, b_+148); B = A;
  CYC(b_+148, b_+150); C = 0x30;
  CALL_C(b_+150, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+153);
  CYC(b_+153, b_+154); alu_or(gb, B);
  CYC(b_+154, b_+155); H = D;
  CYC(b_+155, b_+157); L = ENEMY_BASE + OBJ_STATE;
  if (F & FZ) { CYCT(b_+157, b_+159); goto prepareProjectile; } // jr z
  CYC(b_+157, b_+159);

changeDirection:
  CYC(b_+159, b_+161); mem_wr(gb, HL, 0x09); // [state] = $09
  CYC(b_+161, b_+163); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+163, b_+165); A = 0x30;
  CYC(b_+165, b_+166); alu_add(gb, C);
  CYC(b_+166, b_+167); mem_wr(gb, HL, A);
  CYC(b_+167, b_+169); goto updateAngleAndSpeed; // jr

prepareProjectile:
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x0a); // [state] = $0a
  CYC(b_+171, b_+173); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+173, b_+175); mem_wr(gb, HL, 0x08);
  CALL_C(b_+175, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+178);
  CYC(b_+178, b_+181); ecom_updateAnimationFromAngle_b0d_hook(gb); return; // jp

state_09:
  CALL_C(b_+181, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+184);
  if (F & FZ) { CYCT(b_+184, b_+186); goto gotoState8; } // jr z
  CYC(b_+184, b_+186);
  CALL_C(b_+186, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+189);
  if (F & FZ) { CYCT(b_+189, b_+191); goto updateAngleAndSpeed; } // jr z
  CYC(b_+189, b_+191);

animate:
  CYC(b_+191, b_+194); enemyAnimate_hook(gb); return; // jp

state_0a:
  CALL_C(b_+194, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+197);
  if (!(F & FZ)) { CYCT(b_+197, b_+199); goto animate; } // jr nz
  CYC(b_+197, b_+199);
  CYC(b_+199, b_+201); B = 0x1b; // PART_LYNEL_BEAM
  CALL_C(b_+201, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+204);
  if (!(F & FZ)) { CYCT(b_+204, b_+206); goto gotoState8; } // jr nz
  CYC(b_+204, b_+206);
  CALL_C(b_+206, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+209);
  CYC(b_+209, b_+211); alu_and(gb, 0x30);
  CYC(b_+211, b_+213); alu_add(gb, 0x30);
  CYC(b_+213, b_+215); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+215, b_+216); mem_wr(gb, DE, A);
  CYC(b_+216, b_+217); H = D;
  CYC(b_+217, b_+219); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+219, b_+221); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+221, b_+223); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+223, b_+225); mem_wr(gb, HL, 0x09);
  CYC(b_+225, b_+227); goto animate; // jr

gotoState8:
  CYC(b_+227, b_+229); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+229, b_+231); A = 0x08;
  CYC(b_+231, b_+232); mem_wr(gb, DE, A);
  CYC(b_+232, b_+234); goto animate; // jr

updateAngleAndSpeed:
  CYC(b_+234, b_+237); push_effect(gb, b_+237); goto chooseNewAngle;
updateAngleAndSpeed_resume: // resumes at 0x4b64
  CYC(b_+237, b_+239); B = 0x0e;
  CALL_C(b_+239, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+242);
  if (!(F & FC)) { CYCT(b_+242, b_+244); goto notCentered; } // jr nc
  CYC(b_+242, b_+244);
  CALL_C(b_+244, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+247);
  CYC(b_+247, b_+249); alu_add(gb, 0x04);
  CYC(b_+249, b_+251); alu_and(gb, 0x18);
  CYC(b_+251, b_+252); H = D;
  CYC(b_+252, b_+254); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+254, b_+255); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+255, b_+257); A = 0x28; // SPEED_100
  CYC(b_+257, b_+259); B = 0x04;
  if (F & FZ) { CYCT(b_+259, b_+261); goto setSpeed; } // jr z
  CYC(b_+259, b_+261);

notCentered:
  CYC(b_+261, b_+263); A = 0x14; // SPEED_80
  CYC(b_+263, b_+265); B = 0x00;

setSpeed:
  CYC(b_+265, b_+267); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+267, b_+268); mem_wr(gb, HL, A);

updateAnimationFromAngle:
  CYC(b_+268, b_+269); H = D;
  CYC(b_+269, b_+271); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+271, b_+272); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+272, b_+274); A = alu_swap(gb, A);
  CYC(b_+274, b_+275); alu_rlca(gb);
  CYC(b_+275, b_+276); alu_add(gb, B);
  CYC(b_+276, b_+277); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    RET_TAKEN(b_+277);
    if (gb->pc == b_+137 && gb->sp == sp0_) goto state_scentSeed_resume;
    return;
  } // ret z
  CYC(b_+277, b_+278);
  CYC(b_+278, b_+279); mem_wr(gb, HL, A);
  CYC(b_+279, b_+282); enemySetAnimation_hook(gb);
  if (gb->pc == b_+137 && gb->sp == sp0_) goto state_scentSeed_resume;
  return; // jp

chooseNewAngle:
  CALL_C(b_+282, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+285);
  CYC(b_+285, b_+286); H = D;
  CYC(b_+286, b_+288); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+288, b_+289); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+289, b_+292);
    ecom_setRandomCardinalAngle_b0d_hook(gb);
    if (gb->pc == b_+237 && gb->sp == sp0_) goto updateAngleAndSpeed_resume;
    return;
  } // jp nz
  CYC(b_+289, b_+292);
  CYC(b_+292, SYM(enemyCode0e)); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb);
  if (gb->pc == b_+237 && gb->sp == sp0_) goto updateAngleAndSpeed_resume;
  return; // jp
}
