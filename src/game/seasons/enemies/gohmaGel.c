#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/gohmaGel.s.
// ENEMY_GOHMA_GEL

static uint16_t gohmaGel_jump_table(GB *gb) {
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

static void gohmaGel_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_GOHMA_GEL
void s_enemyCode46_hook(GB *gb) {
  BASE(enemyCode46);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_6db4; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_6da1; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x32;
  CALL_C(b_+9, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); TAIL(enemyDie_uncounted); }
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+23);
  TAIL(enemyDie_uncounted_withoutItemDrop);
L_6da1:
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto L_6db4; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_6db4; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+39, b_+41); A = 0x0d;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
L_6db4:
  CALL_C(b_+42, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+45);
  CYC(b_+45, b_+47); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+47, b_+49); goto L_6dd0; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); push_effect(gb, b_+50);
  do { uint16_t jt_ = (gohmaGel_jump_table(gb));
    if (jt_ == SYM(gohma_gel_state0) && hook_is(gb, SYM(gohma_gel_state0), s_gohma_gel_state0_hook)) { s_gohma_gel_state0_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_state_stub) && hook_is(gb, SYM(gohma_gel_state_stub), s_gohma_gel_state_stub_hook)) { s_gohma_gel_state_stub_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_state5) && hook_is(gb, SYM(gohma_gel_state5), s_gohma_gel_state5_hook)) { s_gohma_gel_state5_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_state8) && hook_is(gb, SYM(gohma_gel_state8), s_gohma_gel_state8_hook)) { s_gohma_gel_state8_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_state9) && hook_is(gb, SYM(gohma_gel_state9), s_gohma_gel_state9_hook)) { s_gohma_gel_state9_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
L_6dd0:
  CYC(b_+70, b_+71); A = B;
  CYC(b_+71, b_+72); push_effect(gb, b_+72);
  do { uint16_t jt_ = (gohmaGel_jump_table(gb));
    if (jt_ == SYM(gohma_gel_subid0) && hook_is(gb, SYM(gohma_gel_subid0), s_gohma_gel_subid0_hook)) { s_gohma_gel_subid0_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_subid1) && hook_is(gb, SYM(gohma_gel_subid1), s_gohma_gel_subid1_hook)) { s_gohma_gel_subid1_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_subid2) && hook_is(gb, SYM(gohma_gel_subid2), s_gohma_gel_subid2_hook)) { s_gohma_gel_subid2_hook(gb); return; }
    if (jt_ == SYM(gohma_gel_subid3) && hook_is(gb, SYM(gohma_gel_subid3), s_gohma_gel_subid3_hook)) { s_gohma_gel_subid3_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_gohma_gel_state0_hook(GB *gb) {
  BASE(gohma_gel_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_6de8; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x31);
L_6de8:
  CYC(b_+14, b_+17);
  TAIL(objectSetVisiblec1);
}

void s_gohma_gel_state5_hook(GB *gb) {
  BASE(gohma_gel_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_galeSeedEffect, SYM(ecom_galeSeedEffect_b0d), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x32;
  CALL_C(b_+6, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+9);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+13);
  TAIL(enemyDelete);
}

void s_gohma_gel_state_stub_hook(GB *gb) {
  BASE(gohma_gel_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

void s_gohma_gel_state8_hook(GB *gb) {
  BASE(gohma_gel_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(IO_P1);
  CALL_C(b_+3, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+12, s_ecom_setRandomAngle, SYM(ecom_setRandomAngle_b0d), b_+15);
  s_gohma_gel_state9_hook(gb); return; // falls through
}

void s_gohma_gel_state9_hook(GB *gb) {
  BASE(gohma_gel_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x0e;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); TAIL(ecom_applyVelocityForSideviewEnemyNoHoles_b0d); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+11); return;
}

void s_gohma_gel_subid0_hook(GB *gb) {
  BASE(gohma_gel_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x0a);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohmaGel_jump_table(gb));
    if (jt_ == b_+10) goto stateA;
    if (jt_ == b_+17) goto stateB;
    if (jt_ == b_+62) goto stateC;
    HANDOFF(HL);
  } while (0);
stateA:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28);
  CYC(b_+15, b_+17);
  goto L_6e5c;
stateB:
  CALL_C(b_+17, s_enemyAnimate, SYM(enemyAnimate), b_+20);
  CYC(b_+20, b_+22); C = 0x0c;
  CALL_C(b_+22, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+27); goto L_6e35; }
  CYC(b_+25, b_+27);
  CALL_C(b_+27, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+30);
  CYC(b_+30, b_+33);
  TAIL(objectApplySpeed);
L_6e35:
  CALL_C(b_+33, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+36);
  CYC(b_+36, b_+38); alu_and(gb, 0x07);
  CYC(b_+38, b_+41); SET_HL(b_+54 /* @gohma_gel_seasonsTable_0d_6e4a */);
  CYC(b_+41, b_+42); gohmaGel_add_a_to_hl(gb, b_+42);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+48, b_+50); A = 0x0c;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+54);
  TAIL(objectSetVisible82);

stateC:
  CALL_C(b_+62, s_enemyAnimate, SYM(enemyAnimate), b_+65);
  CALL_C(b_+65, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+68);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CALL_C(b_+69, s_objectSetVisiblec1, SYM(objectSetVisiblec1), b_+72);
L_6e5c:
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x0b);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+78, b_+80); A = 0x80;
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0xfe);
  CYC(b_+83, b_+86);
  TAIL(ecom_updateAngleTowardTarget_b0d);
}

void s_gohma_gel_subid1_hook(GB *gb) {
  BASE(gohma_gel_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x0a);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohmaGel_jump_table(gb));
    if (jt_ == b_+12) goto stateA;
    if (jt_ == b_+34) goto stateB;
    if (jt_ == b_+44) goto stateC;
    if (jt_ == b_+85) goto stateD;
    HANDOFF(HL);
  } while (0);
stateA:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+19, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+22);
  CYC(b_+22, b_+24); alu_and(gb, 0x1f);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); A = 0x3c;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34);
  goto L_6ed4;
stateB:
  CALL_C(b_+34, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+37);
  if (F & FZ) { CYCT(b_+37, b_+39); goto L_6ec4; }
  CYC(b_+37, b_+39);
  CALL_C(b_+39, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+42);
  CYC(b_+42, b_+44);
  goto L_6ed4;
stateC:
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_VAR30;
  CALL_C(b_+47, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+50);
  CYC(b_+50, b_+51); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_6eab; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); A = mem_rd(gb, hFF8F);
  CYC(b_+55, b_+56); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto L_6eab; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); L = E;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+60, s_seasonsFunc_0d_6f87_hook, SYM(seasonsFunc_0d_6f87), b_+63);
  CYC(b_+63, b_+65);
  goto L_6ed4;
L_6eab:
  CALL_C(b_+65, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+68);
  CYC(b_+68, b_+70); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CALL_C(b_+71, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+74);
  if (F & FZ) { CYCT(b_+74, b_+76); goto L_6eb8; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78);
  goto L_6ed4;
L_6eb8:
  CALL_C(b_+78, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); goto L_6ed4; }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85);
  goto L_6ec4;
stateD:
  CALL_C(b_+85, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+88);
  if (!(F & FZ)) { CYCT(b_+88, b_+90); goto L_6ed4; }
  CYC(b_+88, b_+90);
L_6ec4:
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x0c);
  CYC(b_+94, b_+96); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x08);
  CYC(b_+98, b_+100); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+100, b_+102); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+102, b_+103); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+103, b_+105); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+105, b_+106); mem_wr(gb, HL, A);
L_6ed4:
  CYC(b_+106, b_+109);
  TAIL(enemyAnimate);
}

// gohma_gel_subid2@seasonsFunc_0d_6f25
static void gohmaGel_seasonsFunc_0d_6f25(GB *gb) {
  BASE(gohma_gel_subid2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+78, b_+81); A = mem_rd(gb, w1Link_angle);
  CYC(b_+81, b_+83); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+83, b_+86); TAIL(ecom_setRandomAngle_b0d); }
  CYC(b_+83, b_+86);
  CYC(b_+86, b_+88); alu_xor(gb, 0x10);
  CYC(b_+88, b_+90); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  RET(b_+91); return;
}

// gohma_gel_subid0@stateC
static void gohmaGel_stateC(GB *gb) {
  BASE(gohma_gel_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+62, s_enemyAnimate, SYM(enemyAnimate), b_+65);
  CALL_C(b_+65, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+68);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CALL_C(b_+69, s_objectSetVisiblec1, SYM(objectSetVisiblec1), b_+72);
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x0b);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+78, b_+80); A = 0x80;
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0xfe);
  CYC(b_+83, b_+86);
  TAIL(ecom_updateAngleTowardTarget_b0d);
}

// gohma_gel_subid0@stateB
static void gohmaGel_stateB(GB *gb) {
  BASE(gohma_gel_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+17, s_enemyAnimate, SYM(enemyAnimate), b_+20);
  CYC(b_+20, b_+22); C = 0x0c;
  CALL_C(b_+22, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+27); goto L_6e35; }
  CYC(b_+25, b_+27);
  CALL_C(b_+27, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+30);
  CYC(b_+30, b_+33);
  TAIL(objectApplySpeed);
L_6e35:
  CALL_C(b_+33, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+36);
  CYC(b_+36, b_+38); alu_and(gb, 0x07);
  CYC(b_+38, b_+41); SET_HL(b_+54 /* @gohma_gel_seasonsTable_0d_6e4a */);
  CYC(b_+41, b_+42); gohmaGel_add_a_to_hl(gb, b_+42);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+48, b_+50); A = 0x0c;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+54);
  TAIL(objectSetVisible82);
}

// gohma_gel_subid0@stateA
static void gohmaGel_stateA(GB *gb) {
  BASE(gohma_gel_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28);
  CYC(b_+15, b_+17);
  goto L_6e5c;

L_6e5c:
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x0b);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+78, b_+80); A = 0x80;
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0xfe);
  CYC(b_+83, b_+86);
  TAIL(ecom_updateAngleTowardTarget_b0d);
}

void s_gohma_gel_subid2_hook(GB *gb) {
  BASE(gohma_gel_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x0a);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohmaGel_jump_table(gb));
    if (jt_ == SYM(gohma_gel_subid0__stateA)) { gohmaGel_stateA(gb); return; }
    if (jt_ == SYM(gohma_gel_subid0__stateB)) { gohmaGel_stateB(gb); return; }
    if (jt_ == SYM(gohma_gel_subid0__stateC)) { gohmaGel_stateC(gb); return; }
    if (jt_ == b_+14) goto stateD;
    if (jt_ == b_+92) goto seasonsFunc_0d_6f33;
    HANDOFF(HL);
  } while (0);
stateD:
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_yh);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_xh);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CALL_C(b_+26, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+29);
  if (F & FZ) { CYCT(b_+29, b_+31); goto L_6f0f; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+35, b_+37); goto L_6f02; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+38, b_+40); goto L_6f0f; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+41, b_+43); goto L_6f0f; }
  CYC(b_+41, b_+43);
L_6f02:
  CYC(b_+43, b_+46); A = mem_rd(gb, wFrameCounter);
  CYC(b_+46, b_+47); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+47, b_+49); goto L_6f43; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+52); SET_HL(wLinkImmobilized);
  CYC(b_+52, b_+54); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+54, b_+56);
  goto L_6f43;
L_6f0f:
  CALL_L(b_+56, gohmaGel_seasonsFunc_0d_6f25, b_+59);
  CYC(b_+59, b_+62); SET_BC(0xff20);
  CALL_C(b_+62, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+65);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+68, b_+70); A = 0x8f;
  CALL_C(b_+70, s_playSound, SYM(playSound_b00), b_+73);
  CALL_C(b_+73, s_objectSetVisiblec1, SYM(objectSetVisiblec1), b_+76);
  CYC(b_+76, b_+78);
  goto L_6f43;

seasonsFunc_0d_6f33:
  CYC(b_+92, b_+94); C = 0x0e;
  CALL_C(b_+94, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+97);
  if (!(F & FZ)) { CYCT(b_+97, b_+100); TAIL(ecom_applyVelocityForSideviewEnemyNoHoles_b0d); }
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+102); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x0b);
  CYC(b_+104, b_+106); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+106, b_+108); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
L_6f43:
  CYC(b_+108, b_+111);
  TAIL(enemyAnimate);
}

void s_gohma_gel_subid3_hook(GB *gb) {
  BASE(gohma_gel_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x0a);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohmaGel_jump_table(gb));
    if (jt_ == b_+10) goto stateA;
    if (jt_ == b_+17) goto stateB;
    if (jt_ == b_+47) goto stateC;
    HANDOFF(HL);
  } while (0);
stateA:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x1e);
stateB:
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+19); L = E;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+24, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+27);
  CALL_C(b_+27, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+30);
  CYC(b_+30, b_+32); alu_and(gb, 0x01);
  CYC(b_+32, b_+35); SET_HL(b_+45 /* @seasonsTable_0d_6f73 */);
  CYC(b_+35, b_+36); gohmaGel_add_a_to_hl(gb, b_+36);
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+40, b_+42); alu_and(gb, 0x1f);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+45);
  goto L_6f84;

stateC:
  CALL_C(b_+47, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+50);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto L_6f7e; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); L = E;
  CYC(b_+53, b_+54); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+54, b_+56);
  goto L_6f84;
L_6f7e:
  CALL_C(b_+56, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+59);
  CALL_C(b_+59, s_objectApplySpeed, SYM(objectApplySpeed), b_+62);
L_6f84:
  CYC(b_+62, b_+65);
  TAIL(enemyAnimate);
}

void s_seasonsFunc_0d_6f87_hook(GB *gb) {
  BASE(seasonsFunc_0d_6f87);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+8); SET_HL(SYM(seasonsTable_0d_6f95));
  CYC(b_+8, b_+9); gohmaGel_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

