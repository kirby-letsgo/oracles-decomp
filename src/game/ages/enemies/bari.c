#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void bari_state_uninitialized_hook(GB *gb);
void bari_state_stub_hook(GB *gb);
void bari_subid0_hook(GB *gb);
void bari_subid0_state8_hook(GB *gb);
void bari_applySpeed_hook(GB *gb);
void bari_animate_hook(GB *gb);
void bari_state9_hook(GB *gb);
void bari_setRandomAngleAndCounter2_hook(GB *gb);
void bari_subid0_stateA_hook(GB *gb);
void bari_subid0_stateA_spawnSmallBari_hook(GB *gb);
void bari_subid1_hook(GB *gb);
void bari_updateZPosition_hook(GB *gb);

static uint16_t bari_jump_table(GB *gb) {
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

static void bari_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_BARI
//
// Variables:
//   var30/var31: Initial Y/X position (aka target position; they always hover around this
//                area. For subid 0 (large baris) only.)
//   var32: Counter for "bobbing" of Z position
// ==================================================================================================
void enemyCode3c_hook(GB *gb) {
  BASE(enemyCode3c);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockback_b0e_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x9e); // $80|ITEMCOLLISION_GALE_SEED
  if (F & FZ) { CYCT(b_+17, b_+19); goto normalStatus; } // jr z
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+23); return; } // ret z
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto normalStatus; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_cp(gb, 0x59); // ENEMYCOLLISION_BARI_ELECTRIC_SHOCK
  if (F & FZ) { CYCT(b_+35, b_+37); goto normalStatus; } // jr z
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_cp(gb, 0x83); // $80|ITEMCOLLISION_L3_SHIELD
  if (F & FC) { CYCT(b_+42, b_+44); goto normalStatus; } // jr c
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x0a);

normalStatus:
  CALL_C(b_+49, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+52);
  if (F & FC) { CYCT(b_+52, b_+54); goto commonState; } // jr c
  CYC(b_+52, b_+54);
  CALL_C(b_+54, bari_updateZPosition_hook, SYM(bari_updateZPosition), b_+57);
  CYC(b_+57, b_+59); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+59, b_+60); A = B;
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+61, b_+64); bari_subid0_hook(gb); return; } // jp z
  CYC(b_+61, b_+64);
  CYC(b_+64, b_+67); bari_subid1_hook(gb); return; // jp

commonState:
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  {
    CYC(b_+68, b_+69); push_effect(gb, b_+69);
    uint16_t target = bari_jump_table(gb);
    if (target == SYM(bari_state_uninitialized)) { bari_state_uninitialized_hook(gb); return; }
    if (target == SYM(bari_state_stub)) { bari_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6c3e, bare global; jump-table target from enemyCode3c@commonState.
void bari_state_uninitialized_hook(GB *gb) {
  BASE(bari_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0f; // SPEED_60
  CALL_C(b_+2, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x04);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xfc);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CALL_C(b_+23, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+26);
  CYC(b_+26, b_+28); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+33, b_+36); bari_setRandomAngleAndCounter2_hook(gb); return; } // jp z
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); A = 0x0a; // SPEED_40
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+43); A = 0x02;
  CYC(b_+43, b_+46); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6c6c, bare global; jump-table target from enemyCode3c@commonState.
void bari_state_stub_hook(GB *gb) {
  BASE(bari_state_stub);
  RET(b_+0); return; // ret
}

// 0e:6c6d, bare global; called from enemyCode3c.
void bari_subid0_hook(GB *gb) {
  BASE(bari_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bari_jump_table(gb);
    if (target == SYM(bari_subid0_state8)) { bari_subid0_state8_hook(gb); return; }
    if (target == SYM(bari_state9)) { bari_state9_hook(gb); return; }
    if (target == SYM(bari_subid0_stateA)) { bari_subid0_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6c77, bare global; jump-table target from bari_subid0. "Non-electric-shock" state.
void bari_subid0_state8_hook(GB *gb) {
  BASE(bari_subid0_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto dontShockYet; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3c); // [counter2] = 60
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x59); // ENEMYCOLLISION_BARI_ELECTRIC_SHOCK
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+18); enemySetAnimation_hook(gb); return; // jp

dontShockYet:
  CALL_C(b_+18, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+21);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); bari_applySpeed_hook(gb); return; } // jr nz
  CYC(b_+21, b_+23);
  CALL_C(b_+23, getRandomNumber_hook, SYM(getRandomNumber), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 0x0e);
  CYC(b_+28, b_+30); alu_add(gb, 0x02);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); // [counter1]
  CYC(b_+31, b_+33); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(b_+33, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+36);
  CALL_C(b_+36, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+39);
  CALL_C(b_+39, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), SYM(bari_applySpeed));
  bari_applySpeed_hook(gb); return; // fallthrough
}

// 0e:6ca1, bare global; falls into from bari_subid0_state8, also reached by genuine jr/jp from
// bari_subid1. Falls through into bari_animate.
void bari_applySpeed_hook(GB *gb) {
  BASE(bari_applySpeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), SYM(bari_animate));
  bari_animate_hook(gb); return; // fallthrough
}

// 0e:6ca7, bare global; falls into from bari_applySpeed, also reached by genuine jr from
// bari_state9.
void bari_animate_hook(GB *gb) {
  BASE(bari_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// 0e:6caa, bare global; jump-table target from bari_subid0, also called from bari_subid1. In
// its "electric shock" state. This is shared between subids 0 and 1 (large and small). Falls
// through into bari_setRandomAngleAndCounter2.
void bari_state9_hook(GB *gb) {
  BASE(bari_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); bari_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = 8
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x2d); // ENEMYCOLLISION_BARI
  CYC(b_+11, b_+12); L = alu_dec8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(bari_setRandomAngleAndCounter2));
  bari_setRandomAngleAndCounter2_hook(gb); return; // fallthrough
}

// 0e:6cbc, bare global; called from bari_state_uninitialized, also falls into from
// bari_state9.
void bari_setRandomAngleAndCounter2_hook(GB *gb) {
  BASE(bari_setRandomAngleAndCounter2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+16); // @counter2Vals
  CYC(b_+8, b_+9); bari_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); ecom_setRandomAngle_b0e_hook(gb); return; // jp
}

// 0e:6cd0, bare global; jump-table target from bari_subid0. Bari has just been attacked; now
// it's splitting in two.
void bari_subid0_stateA_hook(GB *gb) {
  BASE(bari_subid0_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE); // [substate]
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+5); goto substate0; } // jr z
  CYC(b_+3, b_+5);

  CALL_C(b_+5, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CALL_C(b_+9, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+12);
  CYC(b_+12, b_+14); C = 0x04;
  CALL_L(b_+14, bari_subid0_stateA_spawnSmallBari_hook, b_+17);
  CYC(b_+17, b_+19); C = 0xfc;
  CALL_L(b_+19, bari_subid0_stateA_spawnSmallBari_hook, b_+22);
  CALL_C(b_+22, decNumEnemies_hook, SYM(decNumEnemies), b_+25);
  CYC(b_+25, b_+28); enemyDelete_hook(gb); return; // jp

substate0:
  CYC(b_+52, b_+54); B = 0x08; // INTERAC_KILLENEMYPUFF
  CALL_C(b_+54, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+57);
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+60, b_+62); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x04);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+69, b_+71); A = 0x73; // SND_KILLENEMY
  CALL_C(b_+71, playSound_b00_hook, SYM(playSound_b00), b_+74);
  CYC(b_+74, b_+77); objectSetInvisible_hook(gb); return; // jp
}

// 0e:6cec, bare local (no exported symbol); called via genuine call/ret twice from
// bari_subid0_stateA.
// @param c X-offset (and value to add to angle)
void bari_subid0_stateA_spawnSmallBari_hook(GB *gb) {
  BASE(bari_subid0_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+28, b_+30); B = 0x3c; // ENEMY_BARI
  CALL_C(b_+30, ecom_spawnEnemyWithSubid01_b0e_hook, SYM(ecom_spawnEnemyWithSubid01_b0e), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+41, b_+42); E = L;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_add(gb, C);
  CYC(b_+44, b_+46); alu_and(gb, 0x1f);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  CYC(b_+47, b_+49); B = 0x00;
  CYC(b_+49, b_+52); objectCopyPositionWithOffset_hook(gb); return; // jp
}

// 0e:6d1d, bare global; called from enemyCode3c. A small bari.
void bari_subid1_hook(GB *gb) {
  BASE(bari_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); bari_state9_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);

  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); bari_applySpeed_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  CALL_C(b_+12, getRandomNumber_hook, SYM(getRandomNumber), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x1c);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); // [counter1]
  CALL_C(b_+19, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+22);
  CALL_C(b_+22, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+25);
  CYC(b_+25, b_+28); bari_applySpeed_hook(gb); return; // jp
}

// 0e:6d39, bare global; called from enemyCode3c. Bobs up and down.
void bari_updateZPosition_hook(GB *gb) {
  BASE(bari_updateZPosition);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x30);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+12); SET_HL(b_+18); // @zVals
  CYC(b_+12, b_+13); bari_addAToHl_from_rst(gb, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return; // ret
}
