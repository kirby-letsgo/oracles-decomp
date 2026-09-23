#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/magunesu.s.
// ENEMY_MAGUNESU

static uint16_t magunesu_jump_table(GB *gb) {
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

static void magunesu_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_MAGUNESU
void s_enemyCode3c_hook(GB *gb) {
  BASE(enemyCode3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_6bca; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(enemyDie); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); TAIL(ecom_updateKnockback_b0d); }
  CYC(b_+9, b_+12);
  RET(b_+12); return;
L_6bca:
  CALL_C(b_+13, s_magunesuFunc_0d_6ccd_hook, SYM(magunesuFunc_0d_6ccd), b_+16);
  CALL_C(b_+16, s_magunesuFunc_0d_6ce6_hook, SYM(magunesuFunc_0d_6ce6), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); push_effect(gb, b_+23);
  do { uint16_t jt_ = (magunesu_jump_table(gb));
    if (jt_ == b_+53) goto state0;
    if (jt_ == b_+63) goto state_stub;
    if (jt_ == b_+64) goto state8;
    if (jt_ == b_+81) goto state9;
    if (jt_ == b_+107) goto stateA;
    if (jt_ == b_+121) goto stateB;
    if (jt_ == b_+138) goto stateC;
    if (jt_ == b_+163) goto stateD;
    if (jt_ == b_+177) goto stateE;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+53, s_magunesuFunc_0d_6d06_hook, SYM(magunesuFunc_0d_6d06), b_+56);
  CYC(b_+56, b_+58); A = 0x14;
  CALL_C(b_+58, s_ecom_setSpeedAndState8AndVisible, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+61);
  CYC(b_+61, b_+63);
  goto magunesuFunc_0d_6c54;
state_stub:
  RET(b_+63); return;
state8:
  CALL_C(b_+64, s_magunesuFunc_0d_6d18_hook, SYM(magunesuFunc_0d_6d18), b_+67);
  CALL_C(b_+67, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+73); TAIL(ecom_applyVelocityForTopDownEnemy_b0d); }
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+75); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+76, b_+78); A = 0x01;
  CYC(b_+78, b_+81);
  TAIL(enemySetAnimation);
state9:
  CALL_C(b_+81, s_enemyAnimate, SYM(enemyAnimate), b_+84);
  CYC(b_+84, b_+86); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CYC(b_+87, b_+88); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+88); return; }
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+90); A = alu_dec8(gb, A);
  CYC(b_+90, b_+92); A = 0x05;
  if (!(F & FZ)) { CYCT(b_+92, b_+95); TAIL_S(magunesuFunc_0d_6ca9); }
  CYC(b_+92, b_+95);
  CYC(b_+95, b_+96); H = D;
  CYC(b_+96, b_+98); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x02);
  CYC(b_+100, b_+102); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+102, b_+104); A = 0x02;
  CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+105, b_+106); mem_wr(gb, HL, A);
  RET(b_+106); return;
stateA:
  CALL_C(b_+107, s_magunesuFunc_0d_6cb7_hook, SYM(magunesuFunc_0d_6cb7), b_+110);
  if (!(F & FZ)) { RET_TAKEN(b_+110); return; }
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+112); L = E;
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+113, s_magunesuFunc_0d_6d06_hook, SYM(magunesuFunc_0d_6d06), b_+116);
  CYC(b_+116, b_+118); A = 0x03;
  CYC(b_+118, b_+121);
  TAIL(enemySetAnimation);
stateB:
  CALL_C(b_+121, s_magunesuFunc_0d_6d18_hook, SYM(magunesuFunc_0d_6d18), b_+124);
  CALL_C(b_+124, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+127);
  if (!(F & FZ)) { CYCT(b_+127, b_+130); TAIL(ecom_applyVelocityForTopDownEnemy_b0d); }
  CYC(b_+127, b_+130);
  CYC(b_+130, b_+132); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+133, b_+135); A = 0x04;
  CYC(b_+135, b_+138);
  TAIL(enemySetAnimation);
stateC:
  CALL_C(b_+138, s_enemyAnimate, SYM(enemyAnimate), b_+141);
  CYC(b_+141, b_+143); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+143, b_+144); A = mem_rd(gb, DE);
  CYC(b_+144, b_+145); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+145); return; }
  CYC(b_+145, b_+146);
  CYC(b_+146, b_+147); A = alu_dec8(gb, A);
  CYC(b_+147, b_+149); A = 0x02;
  if (!(F & FZ)) { CYCT(b_+149, b_+151); TAIL_S(magunesuFunc_0d_6ca9); }
  CYC(b_+149, b_+151);
magunesuFunc_0d_6c54:
  CYC(b_+151, b_+152); H = D;
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x00);
  CYC(b_+156, b_+158); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+158, b_+160); A = 0x01;
  CYC(b_+160, b_+161); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+161, b_+162); mem_wr(gb, HL, A);
  RET(b_+162); return;
stateD:
  CALL_C(b_+163, s_magunesuFunc_0d_6cb7_hook, SYM(magunesuFunc_0d_6cb7), b_+166);
  if (!(F & FZ)) { RET_TAKEN(b_+166); return; }
  CYC(b_+166, b_+167);
  CYC(b_+167, b_+168); L = E;
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x08);
  CALL_C(b_+170, s_magunesuFunc_0d_6d06_hook, SYM(magunesuFunc_0d_6d06), b_+173);
  CYC(b_+173, b_+174); alu_xor(gb, A);
  CYC(b_+174, b_+177);
  TAIL(enemySetAnimation);
stateE:
  CALL_C(b_+177, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+180);
  if (!(F & FZ)) { CYCT(b_+180, b_+182); goto L_6c82; }
  CYC(b_+180, b_+182);
  CYC(b_+182, b_+184); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+184, b_+186); mem_wr(gb, HL, 0x14);
  CYC(b_+186, b_+187); L = E;
  CYC(b_+187, b_+189); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+189, b_+190); A = mem_rd(gb, DE);
  CYC(b_+190, b_+191); alu_or(gb, A);
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x08);
  if (F & FZ) { RET_TAKEN(b_+193); return; }
  CYC(b_+193, b_+194);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0x0b);
  RET(b_+196); return;
L_6c82:
  CALL_C(b_+197, s_ecom_applyVelocityForTopDownEnemy, SYM(ecom_applyVelocityForTopDownEnemy_b0d), b_+200);
  if (!(F & FZ)) { RET_TAKEN(b_+200); return; }
  CYC(b_+200, b_+201);
  CALL_C(b_+201, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+204);
  CYC(b_+204, b_+206); alu_xor(gb, 0x10);
  CYC(b_+206, b_+207); H = D;
  CYC(b_+207, b_+209); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+209, b_+210); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+210, b_+212); alu_and(gb, 0x1f);
  CYC(b_+212, b_+214); alu_bit(gb, 4, A);
  CYC(b_+214, b_+216); A = 0x08;
  if (F & FZ) { CYCT(b_+216, b_+218); goto L_6c99; }
  CYC(b_+216, b_+218);
  CYC(b_+218, b_+220); A = 0xf8;
L_6c99:
  CYC(b_+220, b_+221); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+221, b_+223); alu_and(gb, 0x18);
  CYC(b_+223, b_+224); mem_wr(gb, HL, A);
  CYC(b_+224, b_+225); alu_xor(gb, A);
  CALL_C(b_+225, s_ecom_getTopDownAdjacentWallsBitset, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+228);
  if (F & FZ) { RET_TAKEN(b_+228); return; }
  CYC(b_+228, b_+229);
  CYC(b_+229, b_+231); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+231, b_+232); A = mem_rd(gb, DE);
  CYC(b_+232, b_+234); alu_xor(gb, 0x10);
  CYC(b_+234, b_+235); mem_wr(gb, DE, A);
  RET(b_+235); return;
}

void s_magunesuFunc_0d_6ca9_hook(GB *gb) {
  BASE(magunesuFunc_0d_6ca9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+8, s_enemySetAnimation, SYM(enemySetAnimation), b_+11);
  CYC(b_+11, b_+14);
  TAIL(ecom_setRandomCardinalAngle_b0d);
}

void s_magunesuFunc_0d_6cb7_hook(GB *gb) {
  BASE(magunesuFunc_0d_6cb7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CALL_C(b_+8, s_getFreePartSlot, SYM(getFreePartSlot), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x31);
  CYC(b_+14, b_+17); SET_BC(0x0400);
  CALL_C(b_+17, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+20);
  CYC(b_+20, b_+21); alu_or(gb, D);
  RET(b_+21); return;
}

void s_magunesuFunc_0d_6ccd_hook(GB *gb) {
  BASE(magunesuFunc_0d_6ccd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x38);
  CYC(b_+5, b_+7); A = alu_swap(gb, A);
  CYC(b_+7, b_+8); alu_rlca(gb);
  CYC(b_+8, b_+11); SET_HL(SYM(magunesuTable_0d_6cde));
  CYC(b_+11, b_+12); magunesu_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return;
}

void s_magunesuFunc_0d_6ce6_hook(GB *gb) {
  BASE(magunesuFunc_0d_6ce6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, s_magunesuFunc_0d_6cf3_hook, SYM(magunesuFunc_0d_6cf3), b_+8);
  CYC(b_+8, b_+10); B = 0x46;
  CYC(b_+10, b_+13);
  TAIL(ecom_applyGivenVelocity_b0d);
}

void s_magunesuFunc_0d_6cf3_hook(GB *gb) {
  BASE(magunesuFunc_0d_6cf3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+7, b_+10); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+13); alu_bit(gb, 1, A);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_xor(gb, 0x10);
  CYC(b_+17, b_+18); C = A;
  RET(b_+18); return;
}

void s_magunesuFunc_0d_6d06_hook(GB *gb) {
  BASE(magunesuFunc_0d_6d06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(SYM(magunesuTable_0d_6d14));
  CYC(b_+8, b_+9); magunesu_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

void s_magunesuFunc_0d_6d18_hook(GB *gb) {
  BASE(magunesuFunc_0d_6d18);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb));
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0e);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x2d);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+20, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+23);
  CYC(b_+23, b_+25); alu_sub(gb, 0x0c);
  CYC(b_+25, b_+27); alu_and(gb, 0x18);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  RET(b_+30); return;
}

