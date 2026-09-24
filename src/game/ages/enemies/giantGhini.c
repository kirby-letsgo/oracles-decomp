#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void giantGhini_state_uninitialized_hook(GB *gb);
void giantGhini_state_stub_hook(GB *gb);
void giantGhini_state8_hook(GB *gb);
void giantGhini_gotoState9_hook(GB *gb);
void giantGhini_setChildRespawnTimer_hook(GB *gb);
void giantGhini_state9_hook(GB *gb);
void giantGhini_updateChargeTargetPosition_hook(GB *gb);
void giantGhini_stateA_hook(GB *gb);
void giantGhini_updateZPos_hook(GB *gb);
void giantGhini_spawnChildren_hook(GB *gb);
void giantGhini_getTargetAngle_hook(GB *gb);

static uint16_t giantGhini_jump_table(GB *gb) {
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
// ENEMY_GIANT_GHINI
//
// Variables:
//   var30: Number of children alive
//   var32: Nonzero to begin charging at Link (written to by ENEMY_GIANT_GHINI_CHILD)
//   var33: Counter for Z-axis movement (reverses direction every 16 frames)
//   var34: The current "vertical half" of the screen it's moving toward
//   var35: Position the ghini is currently charging toward
// ==================================================================================================
void enemyCode70_hook(GB *gb) {
  BASE(enemyCode70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10);
  TAIL(enemyBoss_dead_b0f); // jp

normalStatus:
  CALL_C(b_+10, giantGhini_updateZPos_hook, SYM(giantGhini_updateZPos), b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = giantGhini_jump_table(gb);
    if (target == SYM(giantGhini_state_uninitialized)) { giantGhini_state_uninitialized_hook(gb); return; }
    if (target == SYM(giantGhini_state_stub)) { giantGhini_state_stub_hook(gb); return; }
    if (target == SYM(giantGhini_state8)) { giantGhini_state8_hook(gb); return; }
    if (target == SYM(giantGhini_state9)) { giantGhini_state9_hook(gb); return; }
    if (target == SYM(giantGhini_stateA)) { giantGhini_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

void giantGhini_state_uninitialized_hook(GB *gb) {
  BASE(giantGhini_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x70; // ENEMY_GIANT_GHINI
  CYC(b_+2, b_+4); B = 0x00;
  CALL_C(b_+4, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CALL_C(b_+7, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+10);
  CYC(b_+10, b_+13); SET_BC(0x0040);
  CALL_C(b_+13, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x78);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xf8);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x10);
  CYC(b_+32, b_+35);
  TAIL(giantGhini_spawnChildren); // jp
}

void giantGhini_state_stub_hook(GB *gb) {
  BASE(giantGhini_state_stub);
  RET(b_+0); return; // ret
}

// The ghini is spawning in before the fight starts
void giantGhini_state8_hook(GB *gb) {
  BASE(giantGhini_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = giantGhini_jump_table(gb);
    if (target == b_+9) goto substate0;
    if (target == b_+34) goto substate1;
    if (target == b_+54) goto substate2;
    HANDOFF(target);
  }

substate0:
  CYC(b_+9, b_+11); A = 0x01; // DISABLE_LINK
  CYC(b_+11, b_+14); W8(wDisabledObjects) = A;
  CYC(b_+14, b_+17); W8(wMenuDisabled) = A;

  // Wait for door to close
  CYC(b_+17, b_+20); A = W8(wcc93);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); A = 0x78;
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);
  CYC(b_+28, b_+30); A = 0x1e;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A); // [counter2]
  CYC(b_+31, b_+34);
  TAIL(ecom_incSubstate_b0f); // jp

substate1:
  CALL_C(b_+34, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+37);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x3c);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+42, b_+44); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+44, b_+46); B = 0x01;
  CYC(b_+46, b_+48); C = 0x0c;
  CALL_C(b_+48, enemyBoss_spawnShadow_b0f_hook, SYM(enemyBoss_spawnShadow_b0f), b_+51);
  CYC(b_+51, b_+54);
  TAIL(ecom_incSubstate_b0f); // jp

substate2:
  // Flicker visibility
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_xor(gb, 0x80);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CALL_C(b_+60, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+63);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; } // ret nz
  CYC(b_+63, b_+64);

  // Finally begin the fight
  CALL_C(b_+64, enemyBoss_beginMiniboss_b0f_hook, SYM(enemyBoss_beginMiniboss_b0f), b_+67);
  CALL_C(b_+67, objectSetVisible80_hook, SYM(objectSetVisible80), SYM(giantGhini_gotoState9));
  TAIL(giantGhini_gotoState9); // fallthrough
}

void giantGhini_gotoState9_hook(GB *gb) {
  BASE(giantGhini_gotoState9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, enemySetAnimation_hook, SYM(enemySetAnimation), b_+4);
  CALL_C(b_+4, giantGhini_getTargetAngle_hook, SYM(giantGhini_getTargetAngle), b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x09);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x02);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x00);
  TAIL(giantGhini_setChildRespawnTimer); // fallthrough
}

void giantGhini_setChildRespawnTimer_hook(GB *gb) {
  BASE(giantGhini_setChildRespawnTimer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+7); C = 0x3c;
  CALL_C(b_+7, multiplyAByC_hook, SYM(multiplyAByC), b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+12, b_+13); A = L;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

// "Normal" state during battle
void giantGhini_state9_hook(GB *gb) {
  BASE(giantGhini_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto beginCharge; } // jr nz
  CYC(b_+4, b_+6);
  CALL_C(b_+6, enemyAnimate_hook, SYM(enemyAnimate), b_+9);
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), b_+12);
  CALL_C(b_+12, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto skipNudge; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x02);
  CALL_C(b_+19, giantGhini_getTargetAngle_hook, SYM(giantGhini_getTargetAngle), b_+22);
  CALL_C(b_+22, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+25);

skipNudge:
  CALL_C(b_+25, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; } // ret nz
  CYC(b_+28, b_+29);
  CALL_C(b_+29, giantGhini_setChildRespawnTimer_hook, SYM(giantGhini_setChildRespawnTimer), b_+32);
  CYC(b_+32, b_+34); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CALL_C(b_+37, getRandomNumber_hook, SYM(getRandomNumber), b_+40);
  CYC(b_+40, b_+42); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+45);
    TAIL(giantGhini_spawnChildren); // jp nz
  }
  CYC(b_+42, b_+45);

beginCharge:
  CYC(b_+45, b_+47); A = 0x01;
  CALL_C(b_+47, enemySetAnimation_hook, SYM(enemySetAnimation), b_+50);
  CALL_C(b_+50, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+53);
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x96);
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x05); // SPEED_20
  TAIL(giantGhini_updateChargeTargetPosition); // fallthrough
}

void giantGhini_updateChargeTargetPosition_hook(GB *gb) {
  BASE(giantGhini_updateChargeTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Get Link's position, save that as the position we're charging toward
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); C = A;
  CALL_C(b_+8, getTileAtPosition_hook, SYM(getTileAtPosition), b_+11);
  CYC(b_+11, b_+12); A = L;
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret
}

// Charging toward Link
void giantGhini_stateA_hook(GB *gb) {
  BASE(giantGhini_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto applySpeed; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x78); // SPEED_300
  if (F & FZ) { CYCT(b_+16, b_+18); goto applySpeed; } // jr z
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); alu_add(gb, 0x05); // SPEED_20
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);

applySpeed:
  CALL_C(b_+21, objectApplySpeed_hook, SYM(objectApplySpeed), b_+24);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYC(b_+28, b_+31); } else { CALL_C_CC(b_+28, giantGhini_updateChargeTargetPosition_hook, SYM(giantGhini_updateChargeTargetPosition), b_+31); } // call nz
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CALL_C(b_+34, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+39, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+42);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CALL_C(b_+45, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+48);
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_cp(gb, L);
  if (F & FZ) { CYCT(b_+52, b_+55); giantGhini_gotoState9_hook(gb); return; } // jp z
  CYC(b_+52, b_+55);
  RET(b_+55); return; // ret
}

void giantGhini_updateZPos_hook(GB *gb) {
  BASE(giantGhini_updateZPos);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x00;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x10;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_cpl(gb);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); alu_cpl(gb);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return; // ret
}

void giantGhini_spawnChildren_hook(GB *gb) {
  BASE(giantGhini_spawnChildren);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x03;

nextChild:
  CYC(b_+2, b_+4); B = 0x3f; // ENEMY_GIANT_GHINI_CHILD
  CALL_C(b_+4, ecom_spawnEnemyWithSubid01_b0f_hook, SYM(ecom_spawnEnemyWithSubid01_b0f), b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);

  // [child.subid] = [this.subid] | index
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, C);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+20, b_+22); A = ENEMY_BASE; // Enemy.start
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+23, b_+24); mem_wr(gb, HL, D);
  CALL_C(b_+24, objectCopyPosition_hook, SYM(objectCopyPosition), b_+27);
  CYC(b_+27, b_+28); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto nextChild; } // jr nz
  CYC(b_+28, b_+30);
  RET(b_+30); return; // ret
}

// Decides on a position to move towards, for state 9 ("normal" state). It will target
// the horizontal center of the screen, with the Y-position one quarter away from the
// screen boundary (depends which side Link is on). The camera affects the target position.
//
// When Link moves beyond the half-screen boundary, the ghini recalculates its angle to
// face directly away from Link before it slowly moves toward him again.
//
// @param[out] a angle
void giantGhini_getTargetAngle_hook(GB *gb) {
  BASE(giantGhini_getTargetAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
retry:
  CYC(b_+0, b_+2); A = hram_rd(gb, hCameraY & 0xff);
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+6); A = W8(w1Link_yh);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); B = 0x28; // ((SCREEN_HEIGHT/4)<<4) + 8
  CYC(b_+9, b_+11); alu_cp(gb, 0x48); // ((SCREEN_HEIGHT/2)<<4) + 8
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto haveHalf; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); B = 0x68; // ((SCREEN_HEIGHT*3/4)<<4) + 8

haveHalf:
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+19, b_+21); goto haveAngle; } // jr z
  CYC(b_+19, b_+21);

  // Link changed sides on the screen boundary
  CYC(b_+21, b_+22); A = B;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A); // [var34]
  CALL_C(b_+23, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+26);
  CYC(b_+26, b_+28); alu_xor(gb, 0x10);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); A = 0x0a;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38);
  goto retry; // jr

haveAngle:
  CYC(b_+38, b_+39); A = C;
  CYC(b_+39, b_+40); alu_add(gb, B);
  CYC(b_+40, b_+41); B = A;
  CYC(b_+41, b_+43); A = hram_rd(gb, hCameraX & 0xff);
  CYC(b_+43, b_+45); alu_add(gb, 0x50); // (SCREEN_WIDTH/2)<<4
  CYC(b_+45, b_+46); C = A;
  CYC(b_+46, b_+49);
  TAIL(objectGetRelativeAngle); // jp
}
