#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(ramrock_updateHorizontalMovement), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(ramrock_updateHorizontalMovement), (from), (to), true)

// object_code/ages/enemies/ramrock.s (ENEMY_RAMROCK), bank $10.

static uint16_t ramrock_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void ramrock_updateHorizontalMovement_hook(GB *gb) {
  BASE(ramrock_updateHorizontalMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemy_b10_hook, SYM(ecom_applyVelocityForSideviewEnemy_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_xor(gb, 0x10);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); alu_xor(gb, A);
  RET(b_+11); return;
}

void ramrock_glovePhase_reverseDirection_hook(GB *gb) {
  BASE(ramrock_glovePhase_reverseDirection);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+3, b_+5); A = 0xc0;
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+6, b_+8);
    goto applySpeed;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = 0x28;
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+11, b_+13);
    goto writeXh;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
writeXh:
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_xor(gb, 0x10);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); alu_xor(gb, A);
applySpeed:
  CYC(b_+22, SYM(ramrock_glovePhase_updateMovement));
  objectApplySpeed_hook(gb);
}

void ramrock_glovePhase_updateMovement_hook(GB *gb) {
  BASE(ramrock_glovePhase_updateMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+7, b_+9);
    goto label_10_262;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); alu_sub(gb, C);
  CYC(b_+12, b_+14); alu_cp(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+14, b_+16);
    goto ret;
  }
  CYC(b_+14, b_+16);
  if (F & FC) {
    CYCT(b_+16, b_+18);
    goto label_10_262;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_cp(gb, 0x50);
  CYC(b_+21, b_+23); C = 0x10;
  if (!(F & FC)) {
    CYCT(b_+23, b_+25);
    goto ret;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27);
  goto moveInDirection;
label_10_262:
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_cp(gb, 0x41);
  CYC(b_+30, b_+32); C = 0x00;
  if (F & FC) {
    CYCT(b_+32, b_+34);
    goto ret;
  }
  CYC(b_+32, b_+34);
moveInDirection:
  CYC(b_+34, b_+36); B = 0x14;
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_ANGLE;
  CALL_C(b_+38, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+41);
ret:
  RET(b_+41); return;
}

void ramrock_glovePhase_gotoSubstate3_hook(GB *gb) {
  BASE(ramrock_glovePhase_gotoSubstate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(hOamFunc);
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x11);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x03);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x08);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x78);
  RET(b_+22); return;
}

void label_10_248_hook(GB *gb) {
  BASE(label_10_248);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);
  CYC(b_+8, b_+10); A = 0x83;
  CALL_C(b_+10, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+13);
  CYC(b_+13, b_+15); A = 0xab;
  CYC(b_+15, SYM(ramrock_seedPhase_substate2));
  playSound_b00_hook(gb);
}

void label_10_236_hook(GB *gb) {
  BASE(label_10_236);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+3);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, SYM(label_10_237));
  enemySetAnimation_hook(gb);
}

void label_10_237_hook(GB *gb) {
  BASE(label_10_237);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+3);
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x0a);
  CYC(b_+13, b_+15); A = 0x04;
  CYC(b_+15, SYM(ramrock_bombPhase_substate4));
  enemySetAnimation_hook(gb);
}

void ramrock_seedPhase_resumeNormalMovement_hook(GB *gb) {
  BASE(ramrock_seedPhase_resumeNormalMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x02);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x78);
  CYC(b_+9, b_+11); A = 0x00;
  CYC(b_+11, SYM(label_10_248));
  enemySetAnimation_hook(gb);
}

void ramrock_seedPhase_6a94_hook(GB *gb) {
  BASE(ramrock_seedPhase_6a94);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+4); A = 0x0c;
  CYC(b_+4, SYM(ramrock_seedPhase_resumeNormalMovement)); mem_wr(gb, DE, A);
  ramrock_seedPhase_resumeNormalMovement_hook(gb);
}

void ramrock_bombPhase_gotoSubstate3_hook(GB *gb) {
  BASE(ramrock_bombPhase_gotoSubstate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+5); A = 0x04;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x32);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x03);
  RET(b_+12); return;
}

void ramrock_state8_substate0_hook(GB *gb) {
  BASE(ramrock_state8_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+5, b_+8); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+8, b_+11); A = mem_rd(gb, wcc93);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+15, b_+17); A = 0x3c;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, SYM(ramrock_state8_substate1));
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_state8_substate1_hook(GB *gb) {
  BASE(ramrock_state8_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(hOamFunc);
  CALL_C(b_+8, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+11);
  CYC(b_+11, b_+13); C = 0x00;
  CALL_C(b_+13, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_cp(gb, 0xf9);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); C = 0x01;
spawnArm:
  CYC(b_+24, b_+26); B = 0x05;
  CALL_C(b_+26, ecom_spawnEnemyWithSubid01_b10_hook, SYM(ecom_spawnEnemyWithSubid01_b10), b_+29);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); mem_wr(gb, HL, C);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x80);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+38); mem_wr(gb, HL, D);
  CYC(b_+38, b_+39); C = alu_dec8(gb, C);
  if (F & FZ) {
    CYCT(b_+39, b_+41);
    goto spawnArm;
  }
  CYC(b_+39, b_+41);
  CYC(b_+41, SYM(ramrock_state8_substate2));
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_state8_substate2_hook(GB *gb) {
  BASE(ramrock_state8_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+14);
  CYC(b_+14, b_+16); A = 0x84;
  CYC(b_+16, SYM(ramrock_state8_substate3));
  loadPaletteHeader_hook(gb);
}

void ramrock_state8_substate3_hook(GB *gb) {
  BASE(ramrock_state8_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+7);
  CYC(b_+7, b_+9); A = 0xab;
  CALL_C(b_+9, playSound_b00_hook, SYM(playSound_b00), b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); A = 0x83;
  CYC(b_+17, SYM(ramrock_state8_substate4));
  loadPaletteHeader_hook(gb);
}

void ramrock_state8_substate5_hook(GB *gb) {
  BASE(ramrock_state8_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0x41);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+16, b_+19); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+19, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x08);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x03);
  CYC(b_+30, b_+32); A = 0x2e;
  CYC(b_+32, SYM(ramrock_swordPhase));
  playSound_b00_hook(gb);
}

void ramrock_state8_substate4_hook(GB *gb) {
  BASE(ramrock_state8_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CALL_C(b_+8, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+17); A = 0x00;
  CALL_C(b_+17, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(ramrock_state8_substate5));
  ramrock_state8_substate5_hook(gb);
}

void ramrock_state8_hook(GB *gb) {
  BASE(ramrock_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ramrock_jump_table(gb));
    if (jt_ == SYM(ramrock_state8_substate0)) { ramrock_state8_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state8_substate1)) { ramrock_state8_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state8_substate2)) { ramrock_state8_substate2_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state8_substate3)) { ramrock_state8_substate3_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state8_substate4)) { ramrock_state8_substate4_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state8_substate5)) { ramrock_state8_substate5_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrock_state0_hook(GB *gb) {
  BASE(ramrock_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x07;
  CYC(b_+2, b_+4); B = 0x83;
  CALL_C(b_+4, enemyBoss_initializeRoom_b10_hook, SYM(enemyBoss_initializeRoom_b10), b_+7);
  CYC(b_+7, b_+9); A = 0x28;
  CALL_C(b_+9, ecom_setSpeedAndState8_b10_hook, SYM(ecom_setSpeedAndState8_b10), b_+12);
  CYC(b_+12, b_+14); A = 0x04;
  CALL_C(b_+14, enemySetAnimation_hook, SYM(enemySetAnimation), b_+17);
  CYC(b_+17, b_+19); B = 0x00;
  CYC(b_+19, b_+21); C = 0x0c;
  CALL_C(b_+21, enemyBoss_spawnShadow_b10_hook, SYM(enemyBoss_spawnShadow_b10), b_+24);
  CYC(b_+24, SYM(ramrock_state_stub));
  objectSetVisible81_hook(gb);
}

void ramrock_state_stub_hook(GB *gb) {
  BASE(ramrock_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

void ramrock_swordPhase_hook(GB *gb) {
  BASE(ramrock_swordPhase);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+8, b_+10);
    ramrock_updateHorizontalMovement_hook(gb);
    return;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18); SET_BC(0x0000);
  CALL_C(b_+18, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+21);
  CALL_C(b_+21, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+24);
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x00);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x1e);
  CYC(b_+34, b_+36); A = 0x04;
  CYC(b_+36, SYM(ramrock_bombPhase));
  enemySetAnimation_hook(gb);
}

void ramrock_bombPhase_substate0_hook(GB *gb) {
  BASE(ramrock_bombPhase_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); B = 0x05;
  CALL_C(b_+17, ecom_spawnEnemyWithSubid01_b10_hook, SYM(ecom_spawnEnemyWithSubid01_b10), b_+20);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x02);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x80);
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);
  CYC(b_+29, b_+30); mem_wr(gb, HL, D);
  CYC(b_+30, b_+32); B = 0x05;
  CALL_C(b_+32, ecom_spawnEnemyWithSubid01_b10_hook, SYM(ecom_spawnEnemyWithSubid01_b10), b_+35);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x03);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x80);
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); mem_wr(gb, HL, D);
  CYC(b_+45, SYM(ramrock_bombPhase_substate1));
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_bombPhase_substate1_hook(GB *gb) {
  BASE(ramrock_bombPhase_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, enemyAnimate_hook, SYM(enemyAnimate), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); SET_BC(hOamFunc);
  CALL_C(b_+17, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+20);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x08);
  CYC(b_+24, b_+26); A = 0x01;
  CALL_C(b_+26, enemySetAnimation_hook, SYM(enemySetAnimation), b_+29);
  CYC(b_+29, SYM(ramrock_bombPhase_substate2));
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_bombPhase_substate2_hook(GB *gb) {
  BASE(ramrock_bombPhase_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x00;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0xf9);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0xab;
  CALL_C(b_+13, playSound_b00_hook, SYM(playSound_b00), SYM(ramrock_bombPhase_gotoSubstate3));
  ramrock_bombPhase_gotoSubstate3_hook(gb);
}

void ramrock_bombPhase_substate3_hook(GB *gb) {
  BASE(ramrock_bombPhase_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x01);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
    goto checkVar35;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = 0x1e;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0xba;
  CALL_C(b_+13, playSound_b00_hook, SYM(playSound_b00), b_+16);
checkVar35:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+21, b_+23);
    label_10_237_hook(gb);
    return;
  }
  CYC(b_+21, b_+23);
  CALL_C(b_+23, enemyAnimate_hook, SYM(enemyAnimate), b_+26);
  CALL_C(b_+26, ecom_applyVelocityForSideviewEnemy_b10_hook, SYM(ecom_applyVelocityForSideviewEnemy_b10), b_+29);
  CALL_C(b_+29, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+32);
  if (F & FZ) {
    CYCT(b_+32, b_+34);
    label_10_236_hook(gb);
    return;
  }
  CYC(b_+32, b_+34);
  CALL_C(b_+34, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+37);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x04);
  CALL_C(b_+40, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+43);
  CYC(b_+43, SYM(label_10_236));
  objectNudgeAngleTowards_hook(gb);
}

void ramrock_bombPhase_substate4_hook(GB *gb) {
  BASE(ramrock_bombPhase_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
    goto rla;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x09);
  CYC(b_+19, b_+21); A = 0x81;
  CYC(b_+21, b_+24);
  playSound_b00_hook(gb);
  return;
rla:
  CYC(b_+24, b_+25); alu_rla(gb);
  if (!(F & FC)) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); A = 0x01;
  CALL_C(b_+28, enemySetAnimation_hook, SYM(enemySetAnimation), b_+31);
  CYC(b_+31, SYM(ramrock_seedPhase));
  ramrock_bombPhase_gotoSubstate3_hook(gb);
}

void ramrock_bombPhase__func_68fe_hook(GB *gb) {
  BASE(ramrock_bombPhase);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+33, b_+34); E = alu_inc8(gb, E);
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); push_effect(gb, b_+36);
  do { uint16_t jt_ = (ramrock_jump_table(gb));
    if (jt_ == SYM(ramrock_bombPhase_substate0)) { ramrock_bombPhase_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrock_bombPhase_substate1)) { ramrock_bombPhase_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrock_bombPhase_substate2)) { ramrock_bombPhase_substate2_hook(gb); return; }
    else if (jt_ == SYM(ramrock_bombPhase_substate3)) { ramrock_bombPhase_substate3_hook(gb); return; }
    else if (jt_ == SYM(ramrock_bombPhase_substate4)) { ramrock_bombPhase_substate4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrock_bombPhase_hook(GB *gb) {
  BASE(ramrock_bombPhase);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  ramrock_bombPhase__func_68fe_hook(gb);
  CYC(b_+3, b_+5); C = 0x03;
  CALL_C(b_+5, findItemWithID_hook, SYM(findItemWithID), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+12);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
    goto findSecondBomb;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); L = 0x09;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xff);
findSecondBomb:
  CYC(b_+18, b_+20); C = 0x03;
  CALL_C(b_+20, findItemWithID_startingAfterH_hook, SYM(findItemWithID_startingAfterH), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CALL_C(b_+24, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+27);
  if (!(F & FC)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); L = 0x09;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xff);
  RET(b_+32); return;
}

void ramrock_seedPhase_substate0_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+4); SET_BC((SYM(mergedTwinrova_iceRoom) + 7));
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto updateMovement;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
    goto updateMovement;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x02);
  CYC(b_+22, b_+24); C = 0x04;
spawnArm:
  CYC(b_+24, b_+26); B = 0x05;
  CALL_C(b_+26, ecom_spawnEnemyWithSubid01_b10_hook, SYM(ecom_spawnEnemyWithSubid01_b10), b_+29);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); mem_wr(gb, HL, C);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x80);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+38); mem_wr(gb, HL, D);
  CYC(b_+38, b_+39); C = alu_inc8(gb, C);
  CYC(b_+39, b_+40); A = C;
  CYC(b_+40, b_+42); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    goto spawnArm;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+47);
  ecom_incSubstate_b10_hook(gb);
  return;
updateMovement:
  CALL_C(b_+47, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+50);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, SYM(ramrock_seedPhase_substate1));
  objectApplySpeed_hook(gb);
}

void ramrock_seedPhase_substate1_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
    label_10_248_hook(gb);
    return;
  }
  CYC(b_+10, b_+12);
  CALL_C(b_+12, enemyAnimate_hook, SYM(enemyAnimate), b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, SYM(ramrock_seedPhase_6a94));
  ramrock_seedPhase_6a94_hook(gb);
}

void ramrock_seedPhase_substate2_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ramrock_updateHorizontalMovement_hook, SYM(ramrock_updateHorizontalMovement), b_+6);
  CALL_C(b_+6, getRandomNumber_hook, SYM(getRandomNumber), b_+9);
  CYC(b_+9, b_+10); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CALL_C(b_+21, getRandomNumber_hook, SYM(getRandomNumber), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x03);
  CYC(b_+26, b_+27); L = E;
  if (F & FZ) {
    CYCT(b_+27, b_+29);
    goto gotoNextSubstate;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x0f);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x06);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x3c);
  CYC(b_+39, b_+41); B = 0x4f;
  CALL_C(b_+41, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+44);
  CYC(b_+44, b_+47); SET_BC((SYM(_getObjectPositionOnScreen_duringScreenTransition) + 63));
  CALL_C(b_+47, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+50);
  CYC(b_+50, b_+52);
  goto setAnimation0;
gotoNextSubstate:
  CYC(b_+52, b_+55); mem_wr(gb, HL, 0x0e);
  CYC(b_+55, b_+56); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x18);
  CALL_C(b_+58, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+61);
setAnimation0:
  CYC(b_+61, b_+63); A = 0x00;
  CYC(b_+63, SYM(ramrock_seedPhase_substate3));
  enemySetAnimation_hook(gb);
}

void ramrock_seedPhase_substate3_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    ramrock_seedPhase_resumeNormalMovement_hook(gb);
    return;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, ramrock_updateHorizontalMovement_hook, SYM(ramrock_updateHorizontalMovement), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xb4);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x1e);
  CYC(b_+21, b_+23); B = 0x34;
  CALL_C(b_+23, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+26);
  CYC(b_+26, b_+28); L = PART_BASE + OBJ_SUBID;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0e);
  CYC(b_+30, b_+33); SET_BC((SYM(gfxRegisterStates) + 250));
  CYC(b_+33, SYM(ramrock_seedPhase_substate4));
  objectCopyPositionWithOffset_hook(gb);
}

void ramrock_seedPhase_substate5_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, SYM(ramrock_seedPhase_substate6));
  ramrock_seedPhase_resumeNormalMovement_hook(gb);
}

void ramrock_seedPhase_substate4_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    ramrock_seedPhase_resumeNormalMovement_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CALL_C(b_+8, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+15);
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    goto gotoNextSubstate;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_and(gb, 0x07);
  CYC(b_+20, b_+22); A = 0x5d;
  if (F & FZ) {
    CALL_C_CC(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  } else {
    CYC(b_+22, b_+25);
  }
  CYC(b_+25, b_+28);
  ramrock_updateHorizontalMovement_hook(gb);
  return;
gotoNextSubstate:
  CALL_C(b_+28, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+31);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x5a);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+37, SYM(ramrock_seedPhase_substate5)); mem_wr(gb, HL, 0x0c);
  ramrock_seedPhase_substate5_hook(gb);
}

void ramrock_seedPhase_substate6_hook(GB *gb) {
  BASE(ramrock_seedPhase_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto callSeedPhase6a94;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
callSeedPhase6a94:
  CYC(b_+11, SYM(ramrock_glovePhase));
  ramrock_seedPhase_6a94_hook(gb);
}

void ramrock_seedPhase_hook(GB *gb) {
  BASE(ramrock_seedPhase);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto runSubstate;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    goto runSubstate;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_cp(gb, 0x9a);
  if (F & FC) {
    CYCT(b_+15, b_+17);
    goto noSeedCollision;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x9f);
  if (F & FC) {
    CYCT(b_+19, b_+21);
    goto seedCollision;
  }
  CYC(b_+19, b_+21);
noSeedCollision:
  CYC(b_+21, b_+22); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+22, b_+24);
    goto noCollision;
  }
  CYC(b_+22, b_+24);
otherCollision:
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x0d);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x10);
  CYC(b_+32, b_+34);
  goto runSubstate;
seedCollision:
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+40);
    goto otherCollision;
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x3c);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+47); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
    goto seedPhaseEnd;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+50, b_+52); A = 0x63;
  CALL_C(b_+52, playSound_b00_hook, SYM(playSound_b00), b_+55);
  CYC(b_+55, b_+57);
  goto runSubstate;
noCollision:
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+61, b_+63);
    goto runSubstate;
  }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+64, b_+66);
    goto runSubstate;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x0c);
runSubstate:
  CYC(b_+70, b_+72); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+74); push_effect(gb, b_+74);
  do { uint16_t jt_ = (ramrock_jump_table(gb));
    if (jt_ == SYM(ramrock_seedPhase_substate0)) { ramrock_seedPhase_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase_substate1)) { ramrock_seedPhase_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase_substate2)) { ramrock_seedPhase_substate2_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase_substate3)) { ramrock_seedPhase_substate3_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase_substate4)) { ramrock_seedPhase_substate4_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase_substate5)) { ramrock_seedPhase_substate5_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase_substate6)) { ramrock_seedPhase_substate6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
seedPhaseEnd:
  CYC(b_+88, b_+90); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x10);
  CALL_C(b_+92, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+95);
  CYC(b_+95, b_+96); L = alu_inc8(gb, L);
  CYC(b_+96, b_+97); alu_xor(gb, A);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A);
  CYC(b_+98, b_+100); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  RET(b_+101); return;
}

void ramrock_glovePhase_substate0_hook(GB *gb) {
  BASE(ramrock_glovePhase_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+4); SET_BC((SYM(mergedTwinrova_iceRoom) + 7));
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto updateMovement;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
    goto updateMovement;
  }
  CYC(b_+13, b_+15);
  CALL_C(b_+15, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+18);
  CYC(b_+18, b_+21); SET_BC((w2FadingSprPalettes + 65));
spawnArm:
  CYC(b_+21, b_+22); push_effect(gb, BC);
  CYC(b_+22, b_+24); B = 0x35;
  CALL_C(b_+24, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+27);
  CYC(b_+27, b_+29); L = PART_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); mem_wr(gb, HL, C);
  CYC(b_+30, b_+31); SET_BC(pop_effect(gb));
  CYC(b_+31, b_+32); push_effect(gb, BC);
  CYC(b_+32, b_+33); C = B;
  CYC(b_+33, b_+35); B = 0x18;
  CALL_C(b_+35, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+38);
  CYC(b_+38, b_+39); SET_BC(pop_effect(gb));
  CYC(b_+39, b_+40); C = alu_dec8(gb, C);
  CYC(b_+40, b_+42); A = 0x04;
  if (!(F & FZ)) {
    CYCT(b_+42, b_+45);
    enemySetAnimation_hook(gb);
    return;
  }
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+46); A = B;
  CYC(b_+46, b_+47); alu_cpl(gb);
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);
  CYC(b_+48, b_+49); B = A;
  CYC(b_+49, b_+51);
  goto spawnArm;
updateMovement:
  CALL_C(b_+51, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+54);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, SYM(ramrock_glovePhase_substate1));
  objectApplySpeed_hook(gb);
}

void ramrock_glovePhase_substate1_hook(GB *gb) {
  BASE(ramrock_glovePhase_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x02);
  CYC(b_+18, b_+20); A = 0x84;
  CYC(b_+20, SYM(ramrock_glovePhase_substate2));
  loadPaletteHeader_hook(gb);
}

void ramrock_glovePhase_substate2_hook(GB *gb) {
  BASE(ramrock_glovePhase_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto afterPalette;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0xab;
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  CYC(b_+10, b_+12); A = 0x83;
  CALL_C(b_+12, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+15);
afterPalette:
  CALL_C(b_+15, enemyAnimate_hook, SYM(enemyAnimate), b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); A = 0x03;
  CALL_C(b_+25, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(ramrock_glovePhase_gotoSubstate3));
  ramrock_glovePhase_gotoSubstate3_hook(gb);
}

void ramrock_glovePhase_substate3_hook(GB *gb) {
  BASE(ramrock_glovePhase_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0xf9);
  CYC(b_+8, b_+10); C = 0x00;
  if (!(F & FZ)) {
    CALL_C_CC(b_+10, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+13);
  } else {
    CYC(b_+10, b_+13);
  }
  CALL_C(b_+13, ramrock_glovePhase_updateMovement_hook, SYM(ramrock_glovePhase_updateMovement), b_+16);
  CALL_C(b_+16, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+19);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    ramrock_glovePhase_reverseDirection_hook(gb);
    return;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); C = 0x50;
  CALL_C(b_+23, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+26);
  if (!(F & FC)) {
    CYCT(b_+26, b_+28);
    ramrock_glovePhase_reverseDirection_hook(gb);
    return;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+31, b_+33); A = 0x12;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(b_+34, getRandomNumber_hook, SYM(getRandomNumber), b_+37);
  CYC(b_+37, b_+39); alu_and(gb, 0x01);
  CYC(b_+39, b_+41); A = alu_swap(gb, A);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);
  CALL_C(b_+42, getRandomNumber_hook, SYM(getRandomNumber), b_+45);
  CYC(b_+45, b_+47); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
    goto setSubstate4;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
setSubstate4:
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x04);
  RET(b_+55); return;
}

void ramrock_glovePhase_substate4_hook(GB *gb) {
  BASE(ramrock_glovePhase_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
    goto dead;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, enemyAnimate_hook, SYM(enemyAnimate), b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18);
  ramrock_glovePhase_gotoSubstate3_hook(gb);
  return;
dead:
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, SYM(ramrock_updateHorizontalMovement));
  enemyBoss_dead_b10_hook(gb);
}

void ramrock_glovePhase_hook(GB *gb) {
  BASE(ramrock_glovePhase);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (ramrock_jump_table(gb));
    if (jt_ == SYM(ramrock_glovePhase_substate0)) { ramrock_glovePhase_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrock_glovePhase_substate1)) { ramrock_glovePhase_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrock_glovePhase_substate2)) { ramrock_glovePhase_substate2_hook(gb); return; }
    else if (jt_ == SYM(ramrock_glovePhase_substate3)) { ramrock_glovePhase_substate3_hook(gb); return; }
    else if (jt_ == SYM(ramrock_glovePhase_substate4)) { ramrock_glovePhase_substate4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void enemyCode07_hook(GB *gb) {
  BASE(enemyCode07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ramrock_jump_table(gb));
    if (jt_ == SYM(ramrock_state0)) { ramrock_state0_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state_stub)) { ramrock_state_stub_hook(gb); return; }
    else if (jt_ == SYM(ramrock_state8)) { ramrock_state8_hook(gb); return; }
    else if (jt_ == SYM(ramrock_swordPhase)) { ramrock_swordPhase_hook(gb); return; }
    else if (jt_ == SYM(ramrock_bombPhase)) { ramrock_bombPhase_hook(gb); return; }
    else if (jt_ == SYM(ramrock_seedPhase)) { ramrock_seedPhase_hook(gb); return; }
    else if (jt_ == SYM(ramrock_glovePhase)) { ramrock_glovePhase_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
