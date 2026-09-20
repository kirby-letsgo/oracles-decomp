#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode14), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode14), (from), (to), true)

static uint16_t enemyCode14_jump_table(GB *gb) {
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

static void enemyCode14_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SPIKED_BEETLE
// ==================================================================================================
void enemyCode14_hook(GB *gb) {
  BASE(enemyCode14);
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
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto knockback; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+19); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+19, b_+21); goto checkFlipCollision; } // jr z
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_rlca(gb);
  if (F & FC) { CYCT(b_+25, b_+27); goto checkFlipCollision; } // jr c
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x00);

checkFlipCollision:
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_cp(gb, 0x8c); // $80|ITEMCOLLISION_SHOVEL
  if (F & FZ) { CYCT(b_+34, b_+36); goto alreadyFlipped; } // jr z
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+38, b_+40); alu_sub(gb, 0x01); // ITEMCOLLISION_L1_SHIELD
  CYC(b_+40, b_+42); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto normalStatus; } // jr nc
  CYC(b_+42, b_+44);

alreadyFlipped:
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+49); alu_cp(gb, 0x0b);
  if (F & FZ) { RET_TAKEN(b_+49); return; } // ret z
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x01); // [var30] = $01
  CYC(b_+52, b_+55); SET_BC(0xfe80); // -$180
  CALL_C(b_+55, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+58);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x0b);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x51); // ENEMYCOLLISION_SPIKED_BEETLE_FLIPPED
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 180);
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+75); alu_xor(gb, 0x10);
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); A = 0x52; // SND_BOMB_LAND
  CALL_C(b_+80, playSound_b00_hook, SYM(playSound_b00), b_+83);
  CYC(b_+83, b_+85); A = 0x01;
  CYC(b_+85, b_+88); enemySetAnimation_hook(gb); return; // jp

knockback:
  CYC(b_+88, b_+90); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+92, b_+95); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp z
  CYC(b_+92, b_+95);
  CYC(b_+95, b_+97); C = 0x18;
  CALL_C(b_+97, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+100);
  CYC(b_+100, b_+102); A = 0x01;
  if (!(F & FC)) { CYCT(b_+102, b_+104); goto storeKnockback; } // jr nc
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+105); alu_xor(gb, A);

storeKnockback:
  CYC(b_+105, b_+107); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+110); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); C = A;
  CYC(b_+112, b_+114); B = 0x23; // SPEED_e0
  CYC(b_+114, b_+117); ecom_applyGivenVelocity_b0d_hook(gb); return; // jp

normalStatus:
  CYC(b_+117, b_+119); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  {
    CYC(b_+120, b_+121); push_effect(gb, b_+121);
    uint16_t target = enemyCode14_jump_table(gb);
    if (target == b_+147) goto uninitialized;
    if (target == b_+155) goto state_stub;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == b_+156) goto state8;
    if (target == b_+179) goto state9;
    if (target == b_+198) goto stateA;
    if (target == b_+222) goto stateB;
    if (target == b_+269) goto stateC;
    HANDOFF(target);
  }

uninitialized:
  CYC(b_+147, b_+150); push_effect(gb, b_+150); goto setRandomAngleAndCounter1;
uninitialized_resume: // resumes at 0x526a
  CYC(b_+150, b_+152); A = 0x0a; // SPEED_40
  CYC(b_+152, b_+155); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_stub:
  RET(b_+155); return; // ret

state8:
  CYC(b_+156, b_+158); B = 0x08;
  CALL_C(b_+158, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+161);
  if (F & FC) { CYCT(b_+161, b_+164); goto chargeLink; } // jp c
  CYC(b_+161, b_+164);
  CALL_C(b_+164, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+167);
  if (F & FZ) { CYCT(b_+167, b_+170); goto setRandomAngleAndCounter1; } // jp z
  CYC(b_+167, b_+170);
  CALL_C(b_+170, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+173);
  if (F & FZ) { CYCT(b_+173, b_+176); goto setRandomAngleAndCounter1; } // jp z
  CYC(b_+173, b_+176);

animate:
  CYC(b_+176, b_+179); enemyAnimate_hook(gb); return; // jp

state9:
  CALL_C(b_+179, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+182);
  CYC(b_+182, b_+185); push_effect(gb, b_+185); goto incSpeed;
state9_resume: // resumes at 0x528d
  CALL_C(b_+185, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+188);
  if (!(F & FZ)) { CYCT(b_+188, b_+190); goto animate; } // jr nz
  CYC(b_+188, b_+190);
  CALL_C(b_+190, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+193);
  CYC(b_+193, b_+195); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+195, b_+197); mem_wr(gb, HL, 30);
  RET(b_+197); return; // ret

stateA:
  CYC(b_+198, b_+200); B = 0x08;
  CALL_C(b_+200, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+203);
  if (F & FC) { CYCT(b_+203, b_+206); goto chargeLink; } // jp c
  CYC(b_+203, b_+206);
  CALL_C(b_+206, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+209);
  if (!(F & FZ)) { CYCT(b_+209, b_+211); goto animate; } // jr nz
  CYC(b_+209, b_+211);
  CYC(b_+211, b_+213); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0x08);
  CYC(b_+215, b_+217); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+217, b_+219); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(b_+219, b_+222); goto setRandomAngleAndCounter1; // jp

stateB:
  CALL_C(b_+222, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+225);
  if (!(F & FZ)) { CYCT(b_+225, b_+227); goto waitingToFlipBack; } // jr nz
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+228); L = E;
  CYC(b_+228, b_+229); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(b_+229, b_+231); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+231, b_+233); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+233, b_+235); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+235, b_+237); mem_wr(gb, HL, 0x18); // ENEMYCOLLISION_SPIKED_BEETLE
  CYC(b_+237, b_+239); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+239, b_+240); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+240, b_+243); SET_BC(0xfe80); // -$180
  CALL_C(b_+243, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+246);
  CYC(b_+246, b_+247); alu_xor(gb, A);
  CYC(b_+247, b_+250); enemySetAnimation_hook(gb); return; // jp

waitingToFlipBack:
  CYC(b_+250, b_+251); A = mem_rd(gb, HL);
  CYC(b_+251, b_+253); alu_cp(gb, 0x3c);
  if (!(F & FC)) { CYCT(b_+253, b_+255); goto animate; } // jr nc
  CYC(b_+253, b_+255);
  CYC(b_+255, b_+257); alu_and(gb, 0x06);
  CYC(b_+257, b_+258); alu_rrca(gb);
  CYC(b_+258, b_+261); SET_HL(b_+349); // @xOscillationOffsets (data)
  CYC(b_+261, b_+262); enemyCode14_addAToHl_from_rst(gb, b_+262);
  CYC(b_+262, b_+264); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+264, b_+265); A = mem_rd(gb, DE);
  CYC(b_+265, b_+266); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+266, b_+267); mem_wr(gb, DE, A);
  CYC(b_+267, b_+269); goto animate; // jr

stateC:
  CALL_C(b_+269, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+272);
  CALL_C(b_+272, enemyAnimate_hook, SYM(enemyAnimate), b_+275);
  CYC(b_+275, b_+277); C = 0x18;
  CALL_C(b_+277, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+280);
  if (!(F & FZ)) { RET_TAKEN(b_+280); return; } // ret nz
  CYC(b_+280, b_+281);
  CYC(b_+281, b_+283); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+283, b_+285); A = 0x08;
  CYC(b_+285, b_+286); mem_wr(gb, DE, A);
  CYC(b_+286, b_+288); B = 0x10;
  CALL_C(b_+288, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+291);
  if (F & FC) { CYCT(b_+291, b_+293); goto chargeLink; } // jr c
  CYC(b_+291, b_+293);
  CYC(b_+293, b_+295); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+295, b_+297); A = 0x0a; // SPEED_40
  CYC(b_+297, b_+298); mem_wr(gb, DE, A);
  RET(b_+298); return; // ret

setRandomAngleAndCounter1:
  CYC(b_+299, b_+302); SET_BC((SYM(addToGashaMaturity) + 15));
  CALL_C(b_+302, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+305);
  CYC(b_+305, b_+307); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+307, b_+308); A = B;
  CYC(b_+308, b_+309); mem_wr(gb, DE, A);
  CYC(b_+309, b_+311); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+311, b_+313); A = 0x30;
  CYC(b_+313, b_+314); alu_add(gb, C);
  CYC(b_+314, b_+315); mem_wr(gb, DE, A);
  RET(b_+315);
  if (gb->pc == b_+150 && gb->sp == sp0_) goto uninitialized_resume;
  return; // ret

chargeLink:
  CALL_C(b_+316, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+319);
  CYC(b_+319, b_+320); H = D;
  CYC(b_+320, b_+322); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+322, b_+324); mem_wr(gb, HL, 0x09);
  CYC(b_+324, b_+326); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+326, b_+328); mem_wr(gb, HL, 150);
  CYC(b_+328, b_+330); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+330, b_+332); mem_wr(gb, HL, 0x0a); // SPEED_40
  RET(b_+332); return; // ret

incSpeed:
  CYC(b_+333, b_+335); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+335, b_+336); A = mem_rd(gb, DE);
  CYC(b_+336, b_+338); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    RET_TAKEN(b_+338);
    if (gb->pc == b_+185 && gb->sp == sp0_) goto state9_resume;
    return;
  } // ret nz
  CYC(b_+338, b_+339);
  CYC(b_+339, b_+341); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+341, b_+342); A = mem_rd(gb, DE);
  CYC(b_+342, b_+344); alu_cp(gb, 0x3c); // SPEED_180
  if (!(F & FC)) {
    RET_TAKEN(b_+344);
    if (gb->pc == b_+185 && gb->sp == sp0_) goto state9_resume;
    return;
  } // ret nc
  CYC(b_+344, b_+345);
  CYC(b_+345, b_+347); alu_add(gb, 0x05); // SPEED_20
  CYC(b_+347, b_+348); mem_wr(gb, DE, A);
  RET(b_+348);
  if (gb->pc == b_+185 && gb->sp == sp0_) goto state9_resume;
  return; // ret
}
