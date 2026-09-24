#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/ironMask.s.
// ENEMY_IRON_MASK

static uint16_t ironMask_jump_table(GB *gb) {
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

static void ironMask_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_IRON_MASK
void s_enemyCode1c_hook(GB *gb) {
  BASE(enemyCode1c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_checkHazards, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); TAIL(enemyDie); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); TAIL(ecom_updateKnockbackAndCheckHazards_b0d); }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_cp(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29);
  TAIL(enemyDelete);
normalStatus:
  CALL_C(b_+29, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+32);
  if (!(F & FC)) { CYCT(b_+32, b_+34); goto L_6c94; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (ironMask_jump_table(gb));
    if (jt_ == SYM(ironMask_state_uninitialized) && hook_is(gb, SYM(ironMask_state_uninitialized), s_ironMask_state_uninitialized_hook)) { s_ironMask_state_uninitialized_hook(gb); return; }
    if (jt_ == SYM(ironMask_state_stub) && hook_is(gb, SYM(ironMask_state_stub), s_ironMask_state_stub)) { s_ironMask_state_stub(gb); return; }
    if (jt_ == SYM(ecom_blownByGaleSeedState_b0d) && hook_is(gb, SYM(ecom_blownByGaleSeedState_b0d), s_ecom_blownByGaleSeedState)) { s_ecom_blownByGaleSeedState(gb); return; }
    HANDOFF(HL);
  } while (0);
L_6c94:
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+53); push_effect(gb, b_+53);
  do { uint16_t jt_ = (ironMask_jump_table(gb));
    if (jt_ == SYM(ironMask_subid00) && hook_is(gb, SYM(ironMask_subid00), s_ironMask_subid00_hook)) { s_ironMask_subid00_hook(gb); return; }
    if (jt_ == SYM(ironMask_subid01) && hook_is(gb, SYM(ironMask_subid01), s_ironMask_subid01_hook)) { s_ironMask_subid01_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_ironMask_state_uninitialized_hook(GB *gb) {
  BASE(ironMask_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_bit(gb, 0, B);
  if (!(F & FZ)) { CYCT(b_+2, b_+5); TAIL(ecom_setSpeedAndState8_b0d); }
  CYC(b_+2, b_+5);
  CYC(b_+5, b_+7); A = 0x14;
  CALL_C(b_+7, s_ecom_setSpeedAndState8AndVisible, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+13); return;
}

// Iron mask with mask on
void s_ironMask_subid00_hook(GB *gb) {
  BASE(ironMask_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ironMask_jump_table(gb));
    if (jt_ == b_+10) goto state8;
    if (jt_ == b_+25) goto state9;
    if (jt_ == b_+48) goto stateA;
    HANDOFF(HL);
  } while (0);
state8:
  CALL_C(b_+10, s_ironMask_magnetGloveCheck_hook, SYM(ironMask_magnetGloveCheck), b_+13);
  CALL_C(b_+13, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); TAIL(ironMask_updateCollisionsFromLinkRelativeAngle); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+22, s_ironMask_chooseRandomAngleAndCounter1_hook, SYM(ironMask_chooseRandomAngleAndCounter1), b_+25);
state9:
  CALL_C(b_+25, s_ironMask_magnetGloveCheck_hook, SYM(ironMask_magnetGloveCheck), b_+28);
  CALL_C(b_+28, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto L_6cd0; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+36, s_ironMask_chooseAmountOfTimeToStand, SYM(ironMask_chooseAmountOfTimeToStand), b_+39);
L_6cd0:
  CALL_C(b_+39, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+42);
  CALL_C(b_+42, s_ironMask_updateCollisionsFromLinkRelativeAngle, SYM(ironMask_updateCollisionsFromLinkRelativeAngle), b_+45);
  CYC(b_+45, b_+48);
  TAIL(enemyAnimate);
stateA:
  CALL_C(b_+48, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+51);
  if (F & FZ) CALL_C_CC(b_+51, s_ironMask_chooseRandomAngleAndCounter1_hook, SYM(ironMask_chooseRandomAngleAndCounter1), b_+54);
  else CYC(b_+51, b_+54);
  CALL_C(b_+54, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+57);
  CYC(b_+57, b_+60);
  TAIL(enemyAnimate);
}

// Detached "mask"
void s_ironMask_subid01_hook(GB *gb) {
  BASE(ironMask_subid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ironMask_jump_table(gb));
    if (jt_ == b_+10) goto state8;
    if (jt_ == b_+29) goto state9;
    if (jt_ == b_+48) goto stateA;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x50);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x05);
  CYC(b_+21, b_+23); A = 0x05;
  CALL_C(b_+23, s_enemySetAnimation, SYM(enemySetAnimation), b_+26);
  CALL_C(b_+26, s_objectSetVisible82, SYM(objectSetVisible82), b_+29);
state9:
  CYC(b_+29, b_+32); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+33, b_+35); goto L_6d0e; }
  CYC(b_+33, b_+35);
  CALL_C(b_+35, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+38);
  CYC(b_+38, b_+41);
  TAIL(objectApplySpeed);
L_6d0e:
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+43); L = E;
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x1e);
stateA:
  CALL_C(b_+48, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+51);
  if (!(F & FZ)) { CYCT(b_+51, b_+54); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+51, b_+54);
  CYC(b_+54, b_+57);
  TAIL(enemyDelete);
}

// ironMask_chooseRandomAngleAndCounter1@chooseAngle
static void ironMask_chooseAngle(GB *gb) {
  BASE(ironMask_chooseRandomAngleAndCounter1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+38, b_+39); A = C;
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+40, b_+43); TAIL(ecom_updateCardinalAngleTowardTarget_b0d); }
  CYC(b_+40, b_+43);
  CYC(b_+43, b_+46);
  TAIL(ecom_setRandomCardinalAngle_b0d);
}

void s_ironMask_chooseRandomAngleAndCounter1_hook(GB *gb) {
  BASE(ironMask_chooseRandomAngleAndCounter1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0703);
  CALL_C(b_+3, s_ecom_randomBitwiseAndBCE, SYM(ecom_randomBitwiseAndBCE_b0d), b_+6);
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+10); SET_HL(b_+46 /* @counter1Vals */);
  CYC(b_+10, b_+11); ironMask_add_a_to_hl(gb, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+20, b_+23); TAIL(ecom_setRandomCardinalAngle_b0d); }
  CYC(b_+20, b_+23);
  CALL_L(b_+23, ironMask_chooseAngle, b_+26);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+29); alu_rlca(gb);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+32, b_+33); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CYC(b_+35, b_+38);
  TAIL(enemySetAnimation);
}

void s_ironMask_magnetGloveCheck_hook(GB *gb) {
  BASE(ironMask_magnetGloveCheck);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_6d94; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); C = 0x40;
  CALL_C(b_+8, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+11);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto L_6d94; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_rrca(gb);
  CYC(b_+14, b_+16); alu_xor(gb, 0x02);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+20); A = mem_rd(gb, w1Link_direction);
  CYC(b_+20, b_+21); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+21, b_+23); goto L_6d9a; }
  CYC(b_+21, b_+23);
L_6d94:
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+25, b_+27); A = 0x3c;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return;
L_6d9a:
  CYC(b_+29, b_+30); SET_HL(pop_effect(gb));
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+34, b_+36); goto L_6db9; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+39); alu_and(gb, 0x03);
  CYC(b_+39, b_+41); alu_sub(gb, 0x01);
  if (!(F & FC)) { CYCT(b_+41, b_+43); goto L_6daa; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); alu_cpl(gb);
  CYC(b_+44, b_+45); A = alu_inc8(gb, A);
L_6daa:
  CYC(b_+45, b_+46); A = alu_dec8(gb, A);
  CYC(b_+46, b_+48); alu_bit(gb, 0, B);
  if (F & FZ) { CYCT(b_+48, b_+50); goto L_6db4; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+52, b_+53); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  RET(b_+54); return;
L_6db4:
  CYC(b_+55, b_+57); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+57, b_+58); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+58, b_+59); mem_wr(gb, HL, A);
  RET(b_+59); return;
L_6db9:
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x0a);
  CYC(b_+64, b_+66); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x50);
  CYC(b_+68, b_+70); A = 0x04;
  CALL_C(b_+70, s_enemySetAnimation, SYM(enemySetAnimation), b_+73);
  CYC(b_+73, b_+75); B = 0x1c;
  CALL_C(b_+75, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+78);
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+82);
  TAIL(objectCopyPosition);
}

