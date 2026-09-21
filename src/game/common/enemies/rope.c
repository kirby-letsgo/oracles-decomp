#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void rope_subid00_hook(GB *gb);
void rope_subid01_hook(GB *gb);
void rope_subid02_hook(GB *gb);
void rope_subid03_hook(GB *gb);
void rope_state_moveAround_hook(GB *gb);
void rope_callEnemyAnimate_hook(GB *gb);
void rope_state_chargeLink_hook(GB *gb);
void rope_changeDirection_hook(GB *gb);
void rope_updateAnimationFromAngle_hook(GB *gb);
void rope_animate_hook(GB *gb);
void rope_checkHazardsIfApplicable_hook(GB *gb);

static uint16_t rope_jump_table(GB *gb) {
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

static void rope_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_ROPE
// ==================================================================================================
void enemyCode10_hook(GB *gb) {
  BASE(enemyCode10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, rope_checkHazardsIfApplicable_hook, SYM(rope_checkHazardsIfApplicable), b_+3);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto normalStatus; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+8); return; } // ret c
  CYC(b_+8, b_+9);
  if (F & FZ) { CYCT(b_+9, b_+12); enemyDie_hook(gb); return; } // jp z
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);
  RET(b_+16); return; // ret

normalStatus:
  CALL_C(b_+17, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+22); goto afterSpeedBump; } // jr z
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); A = 0x32; // SPEED_140
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);

afterSpeedBump:
  CALL_C(b_+27, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+30);
  if (!(F & FC)) { CYCT(b_+30, b_+32); goto normalState; } // jr nc
  CYC(b_+30, b_+32);
  {
    CYC(b_+32, b_+33); push_effect(gb, b_+33);
    uint16_t target = rope_jump_table(gb);
    if (target == b_+59) goto state_uninitialized;
    if (target == b_+148) goto state_stub;
    if (target == b_+78) goto state_switchHook;
    if (target == b_+101) goto state_scentSeed;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+49, b_+50); A = B;
  {
    CYC(b_+50, b_+51); push_effect(gb, b_+51);
    uint16_t target = rope_jump_table(gb);
    if (target == SYM(rope_subid00)) { rope_subid00_hook(gb); return; }
    if (target == SYM(rope_subid01)) { rope_subid01_hook(gb); return; }
    if (target == SYM(rope_subid02)) { rope_subid02_hook(gb); return; }
    if (target == SYM(rope_subid03)) { rope_subid03_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+59, b_+61); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+61, b_+63); A = 0xff;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+65); B = alu_dec8(gb, B);
  CYC(b_+65, b_+67); A = 0x0f; // SPEED_60
  if (F & FZ) { CYCT(b_+67, b_+70); ecom_setSpeedAndState8_b0d_hook(gb); return; } // jp z
  CYC(b_+67, b_+70);
  CYC(b_+70, b_+71); H = D;
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+73, b_+75); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+75, b_+78); TAIL(ecom_setSpeedAndState8AndVisible_b0d); // jp

state_switchHook:
  CYC(b_+78, b_+79); E = alu_inc8(gb, E);
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  {
    CYC(b_+80, b_+81); push_effect(gb, b_+81);
    uint16_t target = rope_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+89) goto substate1;
    if (target == b_+90) goto substate3;
    HANDOFF(target);
  }

substate1:
  RET(b_+89); return; // ret

substate3:
  CYC(b_+90, b_+92); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+96); SET_HL(b_+144); // @defaultStates (data)
  CYC(b_+96, b_+97); rope_addAToHl_from_rst(gb, b_+97);
  CYC(b_+97, b_+98); B = mem_rd(gb, HL);
  CYC(b_+98, b_+101); TAIL(ecom_fallToGroundAndSetState_b0d); // jp

state_scentSeed:
  CYC(b_+101, b_+104); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+104, b_+105); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+105, b_+107); goto seek; } // jr nz
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+113); SET_HL(b_+144); // @defaultStates (data)
  CYC(b_+113, b_+114); rope_addAToHl_from_rst(gb, b_+114);
  CYC(b_+114, b_+116); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+116, b_+117); A = mem_rd(gb, HL);
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
  CYC(b_+118, b_+120); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+120, b_+122); A = 0x0f; // SPEED_60
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  RET(b_+123); return; // ret

seek:
  CALL_C(b_+124, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+127);
  CYC(b_+127, b_+129); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+132); alu_add(gb, 0x04);
  CYC(b_+132, b_+134); alu_and(gb, 0x18);
  CYC(b_+134, b_+135); mem_wr(gb, DE, A);
  CALL_C(b_+135, rope_updateAnimationFromAngle_hook, SYM(rope_updateAnimationFromAngle), b_+138);
  CALL_C(b_+138, ecom_applyVelocityForSideviewEnemy_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0d), b_+141);
  CYC(b_+141, b_+148); TAIL(rope_animate); // jp

state_stub:
  RET(b_+148); return; // ret
}

// 0d:4ea0, bare global; normal rope.
void rope_subid00_hook(GB *gb) {
  BASE(rope_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = rope_jump_table(gb);
    if (target == b_+10) goto state8;
    if (target == SYM(rope_state_moveAround)) { rope_state_moveAround_hook(gb); return; }
    if (target == SYM(rope_state_chargeLink)) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  TAIL(rope_state_moveAround); // fallthrough
}

// 0d:4eb5, bare global; moving around, checking whether to charge Link.
void rope_state_moveAround_hook(GB *gb) {
  BASE(rope_state_moveAround);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x0a;
  CALL_C(b_+2, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto notCentered; } // jr nc
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto notCentered; } // jr nz
  CYC(b_+11, b_+13);
  CALL_C(b_+13, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+16);
  CALL_C(b_+16, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+23, b_+26); TAIL(rope_updateAnimationFromAngle); // jp

notCentered:
  CALL_C(b_+26, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+29);
  CYC(b_+29, b_+30); L = alu_dec8(gb, L);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter1]--
  if (!(F & FZ)) { CALL_C_CC(b_+31, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+34); } else { CYC(b_+31, b_+34); } // call nz
  if (F & FZ) { CYCT(b_+34, b_+37); rope_changeDirection_hook(gb); return; } // jp z
  CYC(b_+34, b_+37);
  TAIL(rope_callEnemyAnimate); // fallthrough
}

// 0d:4eda, bare global.
void rope_callEnemyAnimate_hook(GB *gb) {
  BASE(rope_callEnemyAnimate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0d:4edd, bare global; charging Link.
void rope_state_chargeLink_hook(GB *gb) {
  BASE(rope_state_chargeLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); rope_animate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x0f); // SPEED_60
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x40);
  CYC(b_+18, b_+21); TAIL(rope_changeDirection); // jp
}

// 0d:4ef2, bare global; rope that falls from the sky.
void rope_subid01_hook(GB *gb) {
  BASE(rope_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = rope_jump_table(gb);
    if (target == b_+14) goto state8;
    if (target == b_+27) goto state9;
    if (target == b_+57) goto stateA;
    if (target == SYM(rope_state_moveAround)) { rope_state_moveAround_hook(gb); return; }
    if (target == SYM(rope_state_chargeLink)) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+14, b_+16); A = 0x09;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); alu_and(gb, 0x38);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return; // ret

state9:
  CALL_C(b_+27, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); L = E;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+35, b_+37); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+37, b_+39); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+39, b_+41); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+41, b_+43); L = 0x15; // Enemy.speedZ+1
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+44, b_+46); A = 0x59; // SND_FALLINHOLE
  CALL_C(b_+46, playSound_b00_hook, SYM(playSound_b00), b_+49);
  CALL_C(b_+49, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+52);
  CYC(b_+52, b_+54); C = 0x08;
  CYC(b_+54, b_+57); TAIL(ecom_setZAboveScreen_b0d); // jp

stateA:
  CYC(b_+57, b_+59); C = 0x0e;
  CALL_C(b_+59, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+62);
  if (!(F & FZ)) { RET_TAKEN(b_+62); return; } // ret nz
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+65); L = 0x14; // Enemy.speedZ
  CYC(b_+65, b_+66); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  CYC(b_+67, b_+69); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+72, b_+74); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CALL_C(b_+74, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+77);
  CYC(b_+77, b_+79); A = 0x52; // SND_BOMB_LAND
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);
  CALL_C(b_+82, rope_changeDirection_hook, SYM(rope_changeDirection), b_+85);
  CYC(b_+85, b_+87); TAIL(rope_callEnemyAnimate); // jr
}

// 0d:4f49, bare global; immediately charges Link upon spawning.
void rope_subid02_hook(GB *gb) {
  BASE(rope_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = rope_jump_table(gb);
    if (target == b_+12) goto state8;
    if (target == b_+29) goto state9;
    if (target == SYM(rope_state_moveAround)) { rope_state_moveAround_hook(gb); return; }
    if (target == SYM(rope_state_chargeLink)) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x08);
  CALL_C(b_+23, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+26);
  CYC(b_+26, b_+29); TAIL(rope_updateAnimationFromAngle); // jp

state9:
  CALL_C(b_+29, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto applyVelocity; } // jr nz
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); L = E;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x0b); // [state] = "charge at Link" state
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+39, b_+41); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+41, b_+43); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+43, b_+45); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity:
  CALL_C(b_+45, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+48);
  CYC(b_+48, b_+51); TAIL(enemyAnimate); // jp
}

// 0d:4f7c, bare global; falls and bounces toward Link when it spawns.
void rope_subid03_hook(GB *gb) {
  BASE(rope_subid03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = rope_jump_table(gb);
    if (target == b_+12) goto state8;
    if (target == b_+38) goto state9;
    if (target == SYM(rope_state_moveAround)) { rope_state_moveAround_hook(gb); return; }
    if (target == SYM(rope_state_chargeLink)) { rope_state_chargeLink_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+15, b_+17); L = 0x14; // Enemy.speedZ
  CYC(b_+17, b_+19); A = 0xfe;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xfe);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+31); A = mem_rd(gb, w1Link + OBJ_DIRECTION); // ld a,(w1Link.direction)
  CYC(b_+31, b_+33); A = alu_swap(gb, A);
  CYC(b_+33, b_+34); alu_rrca(gb);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CYC(b_+35, b_+38); TAIL(rope_updateAnimationFromAngle); // jp

state9:
  CYC(b_+38, b_+40); C = 0x0e;
  CALL_C(b_+40, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+43);
  if (F & FC) { CYCT(b_+43, b_+45); goto doneBouncing; } // jr c
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); A = 0x52; // SND_BOMB_LAND
  if (F & FZ) { CALL_C_CC(b_+47, playSound_b00_hook, SYM(playSound_b00), b_+50); } else { CYC(b_+47, b_+50); } // call z
  CYC(b_+50, b_+52); E = 0x15; // Enemy.speedZ+1
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto applyVelocity2; } // jr nz
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+59, b_+61); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+61, b_+63); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+63, b_+65); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity2:
  CYC(b_+65, b_+68); TAIL(ecom_applyVelocityForSideviewEnemyNoHoles_b0d); // jp

doneBouncing:
  CALL_C(b_+68, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+71);
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x0f); // SPEED_60
  TAIL(rope_changeDirection); // fallthrough
}

// 0d:4fc7, bare global; chooses random new angle, random value for counter1.
void rope_changeDirection_hook(GB *gb) {
  BASE(rope_changeDirection);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x1870);
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+13); A = C;
  CYC(b_+13, b_+15); alu_add(gb, 0x70);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  TAIL(rope_updateAnimationFromAngle); // fallthrough
}

// 0d:4fd7, bare global.
void rope_updateAnimationFromAngle_hook(GB *gb) {
  BASE(rope_updateAnimationFromAngle);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+6); return; } // ret z
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+8, b_+10); alu_and(gb, 0x10);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+14); alu_xor(gb, 0x01);
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+15); return; } // ret z
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+20); TAIL(enemySetAnimation); // jp
}

// 0d:4feb, bare global.
void rope_animate_hook(GB *gb) {
  BASE(rope_animate);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+6, b_+8); goto incAndStore; } // jr nc
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);

incAndStore:
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+14); TAIL(enemyAnimate); // jp
}

// 0d:4ff9, bare global.
void rope_checkHazardsIfApplicable_hook(GB *gb) {
  BASE(rope_checkHazardsIfApplicable);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); TAIL(ecom_checkHazards_b0d); // jp
}
