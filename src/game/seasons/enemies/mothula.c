#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/mothula.s.
// ENEMY_MOTHULA
// Variables:
// var30: Angular speed (amount to add to angle; clockwise / counterclockwise)
// var31: Index used to decide turning speed while circling around room
// var32/var33: Target position
// var34: Counter until mothula stops circling around room
// var35: Counter to delay updating angle toward target position
// var36: If nonzero, spawns baby moths instead of ring of fire
// var37: Counter until mothula will shoot a fireball (while circling around room)

static uint16_t mothula_jump_table(GB *gb) {
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

static void mothula_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_MOTHULA
// Variables:
// var30: Angular speed (amount to add to angle; clockwise / counterclockwise)
// var31: Index used to decide turning speed while circling around room
// var32/var33: Target position
// var34: Counter until mothula stops circling around room
// var35: Counter to delay updating angle toward target position
// var36: If nonzero, spawns baby moths instead of ring of fire
// var37: Counter until mothula will shoot a fireball (while circling around room)
void s_enemyCode7a_hook(GB *gb) {
  BASE(enemyCode7a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; }
  CYC(b_+5, b_+7);
normalStatus:
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (mothula_jump_table(gb));
    if (jt_ == SYM(mothula_state_uninitialized) && hook_is(gb, SYM(mothula_state_uninitialized), s_mothula_state_uninitialized_hook)) { s_mothula_state_uninitialized_hook(gb); return; }
    if (jt_ == SYM(mothula_state_stub) && hook_is(gb, SYM(mothula_state_stub), s_mothula_state_stub_hook)) { s_mothula_state_stub_hook(gb); return; }
    if (jt_ == SYM(mothula_state8) && hook_is(gb, SYM(mothula_state8), s_mothula_state8_hook)) { s_mothula_state8_hook(gb); return; }
    if (jt_ == SYM(mothula_state9) && hook_is(gb, SYM(mothula_state9), s_mothula_state9_hook)) { s_mothula_state9_hook(gb); return; }
    if (jt_ == SYM(mothula_stateA) && hook_is(gb, SYM(mothula_stateA), s_mothula_stateA_hook)) { s_mothula_stateA_hook(gb); return; }
    if (jt_ == SYM(mothula_stateB) && hook_is(gb, SYM(mothula_stateB), s_mothula_stateB_hook)) { s_mothula_stateB_hook(gb); return; }
    if (jt_ == SYM(mothula_stateC) && hook_is(gb, SYM(mothula_stateC), s_mothula_stateC_hook)) { s_mothula_stateC_hook(gb); return; }
    if (jt_ == SYM(mothula_stateD) && hook_is(gb, SYM(mothula_stateD), s_mothula_stateD_hook)) { s_mothula_stateD_hook(gb); return; }
    if (jt_ == SYM(mothula_stateE) && hook_is(gb, SYM(mothula_stateE), s_mothula_stateE_hook)) { s_mothula_stateE_hook(gb); return; }
    if (jt_ == SYM(mothula_stateF) && hook_is(gb, SYM(mothula_stateF), s_mothula_stateF_hook)) { s_mothula_stateF_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
dead:
  CALL_C(b_+43, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+46);
  CYC(b_+46, b_+48); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+48, b_+51); A = mem_rd(gb, wDungeonMapData);
  CYC(b_+51, b_+52); H = A;
  CYC(b_+52, b_+54); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+54, b_+56); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+56, b_+59);
  TAIL(enemyBoss_dead_b0f);
}

void s_mothula_state_uninitialized_hook(GB *gb) {
  BASE(mothula_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x7a;
  CYC(b_+2, b_+4); B = 0x82;
  CALL_C(b_+4, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CYC(b_+7, b_+10); SET_BC(0x0108);
  CALL_C(b_+10, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CALL_C(b_+14, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+17);
  CYC(b_+17, b_+19); C = 0x10;
  CYC(b_+19, b_+22);
  TAIL(ecom_setZAboveScreen_b0d);
}

void s_mothula_state_stub_hook(GB *gb) {
  BASE(mothula_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

void s_mothula_state8_hook(GB *gb) {
  BASE(mothula_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectSetVisible81, SYM(objectSetVisible81), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x1f);
  CYC(b_+8, b_+10); A = 0x7c;
  if (F & FZ) CALL_C_CC(b_+10, s_playSound, SYM(playSound_b00), b_+13);
  else CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); C = 0x04;
  CALL_C(b_+15, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+18);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0xfe);
  if (F & FC) { CYCT(b_+23, b_+25); TAIL_S(mothula_animate); }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+32, s_mothula_setTargetPositionToLeftOrRightSide_hook, SYM(mothula_setTargetPositionToLeftOrRightSide), b_+35);
  CYC(b_+35, b_+37); A = 0x2e;
  CYC(b_+37, b_+40); mem_wr(gb, wActiveMusic, A);
  CYC(b_+40, b_+43);
  TAIL(playSound_b00);
}

// Delay before moving
void s_mothula_state9_hook(GB *gb) {
  BASE(mothula_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); TAIL_S(mothula_animate); }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x0a);
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+10, s_mothula_spawnChild_hook, SYM(mothula_spawnChild), b_+13);
  s_mothula_stateA_hook(gb); return; // falls through
}

// Just beginning to move
void s_mothula_stateA_hook(GB *gb) {
  BASE(mothula_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_mothula_checkReachedTargetPosition_hook, SYM(mothula_checkReachedTargetPosition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); TAIL_S(mothula_moveTowardTargetPosition); }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0e);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x50);
  CALL_C(b_+16, s_mothula_initializeStateB_hook, SYM(mothula_initializeStateB), b_+19);
  CYC(b_+19, b_+21);
  TAIL_S(mothula_stateB);
}

// Circling around normally
void s_mothula_stateB_hook(GB *gb) {
  BASE(mothula_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_mothula_decVar34Every4Frames_hook, SYM(mothula_decVar34Every4Frames), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto circlingAround; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  CALL_C(b_+11, s_mothula_chooseTargetPositionWithinHoles_hook, SYM(mothula_chooseTargetPositionWithinHoles), b_+14);
  CYC(b_+14, b_+16);
  TAIL_S(mothula_animate);
circlingAround:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) CALL_C_CC(b_+20, s_mothula_spawnFireball_hook, SYM(mothula_spawnFireball), b_+23);
  else CYC(b_+20, b_+23);
  CALL_C(b_+23, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto L_6b3a; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0a);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_add(gb, 0x0a);
  CYC(b_+35, b_+37); alu_cp(gb, 0x79);
  if (!(F & FC)) { CYCT(b_+37, b_+39); goto L_6b3a; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
L_6b3a:
  CALL_C(b_+40, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+43);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); TAIL_S(mothula_applySpeedAndAnimate); }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+51, b_+53); alu_and(gb, 0x1f);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CALL_C(b_+54, s_mothula_updateAnimation_hook, SYM(mothula_updateAnimation), b_+57);
  CALL_C(b_+57, s_mothula_updateCounter1ForCirclingRoom_hook, SYM(mothula_updateCounter1ForCirclingRoom), b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); TAIL_S(mothula_applySpeedAndAnimate); }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+64, b_+66); A = 0x0e;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CALL_C(b_+67, s_mothula_updateCounter1ForCirclingRoom_hook, SYM(mothula_updateCounter1ForCirclingRoom), b_+70);
  s_mothula_applySpeedAndAnimate_hook(gb); return; // falls through
}

void s_mothula_applySpeedAndAnimate_hook(GB *gb) {
  BASE(mothula_applySpeedAndAnimate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectApplySpeed, SYM(objectApplySpeed), b_+3);
  s_mothula_animate_hook(gb); return; // falls through
}

void s_mothula_animate_hook(GB *gb) {
  BASE(mothula_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(enemyAnimate);
}

// Returning to one of the two center spots
void s_mothula_stateC_hook(GB *gb) {
  BASE(mothula_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_mothula_checkReachedTargetPosition_hook, SYM(mothula_checkReachedTargetPosition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); TAIL_S(mothula_moveTowardTargetPosition); }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12);
  TAIL(enemySetAnimation);
}

void s_mothula_moveTowardTargetPosition_hook(GB *gb) {
  BASE(mothula_moveTowardTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_mothula_updateAngleTowardTargetPosition_hook, SYM(mothula_updateAngleTowardTargetPosition), b_+3);
  CYC(b_+3, b_+5);
  TAIL_S(mothula_applySpeedAndAnimate);
}

// Deciding how long to stand in place?
void s_mothula_stateD_hook(GB *gb) {
  BASE(mothula_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+6); SET_BC(0x0840);
  CALL_C(b_+6, s_ecom_randomBitwiseAndBCE, SYM(ecom_randomBitwiseAndBCE_b0d), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); A = 0x78;
  CYC(b_+17, b_+18); alu_add(gb, C);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21);
  TAIL_S(mothula_animate);
}

// Standing in place
void s_mothula_stateE_hook(GB *gb) {
  BASE(mothula_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto L_6b8e; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_mothula_updateZPosAndOamFlagsForStateE_hook, SYM(mothula_updateZPosAndOamFlagsForStateE), b_+8);
  CYC(b_+8, b_+10);
  TAIL_S(mothula_animate);
L_6b8e:
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x1e);
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xfe);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+21, b_+23); A = 0x06;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CALL_C(b_+25, s_mothula_spawnSomethingAfterStandingStill_hook, SYM(mothula_spawnSomethingAfterStandingStill), b_+28);
  CYC(b_+28, b_+30); A = 0x08;
  CYC(b_+30, b_+33);
  TAIL(enemySetAnimation);
}

// Delay before circling around room again
void s_mothula_stateF_hook(GB *gb) {
  BASE(mothula_stateF);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); TAIL_S(mothula_animate); }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x0a);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0a);
  CYC(b_+12, b_+13); L = alu_dec8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+18);
  TAIL_S(mothula_setTargetPositionToLeftOrRightSide);
}

// @param	hl	var37 (counter to spawn projectile)
void s_mothula_spawnFireball_hook(GB *gb) {
  BASE(mothula_spawnFireball);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x50);
  CYC(b_+2, b_+4); B = 0x31;
  CYC(b_+4, b_+7);
  TAIL(ecom_spawnProjectile_b0d);
}

// Decides what to spawn after state $0e (small moth or ring of fireballs).
void s_mothula_spawnSomethingAfterStandingStill_hook(GB *gb) {
  BASE(mothula_spawnSomethingAfterStandingStill);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); TAIL_S(mothula_spawnChild); }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); B = 0x42;
  CALL_C(b_+8, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); L = PART_BASE + OBJ_SUBID;
  s_mothula_initChild_hook(gb); return; // falls through
}

// Sets child object's subid to $80 normally, or $81 if mothula's health is $10 or less
// @param	hl	Pointer to child object's subid
void s_mothula_initChild_hook(GB *gb) {
  BASE(mothula_initChild);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x80;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto L_6bd6; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
L_6bd6:
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  RET(b_+11); return;
}

void s_mothula_spawnChild_hook(GB *gb) {
  BASE(mothula_spawnChild);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x47;
  CALL_C(b_+2, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_mothula_initChild_hook, SYM(mothula_initChild), b_+9);
  CYC(b_+9, b_+12);
  TAIL(objectCopyPosition);
}

// Update mothula "bouncing" in place for state $0e
// @param	hl	counter1
void s_mothula_updateZPosAndOamFlagsForStateE_hook(GB *gb) {
  BASE(mothula_updateZPosAndOamFlagsForStateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+3); alu_cp(gb, 0x5a);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); alu_and(gb, 0x0e);
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+10); SET_BC(b_+42 /* @zPositions */);
  CALL_C(b_+10, s_addAToBc, SYM(addAToBc), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+15, b_+16); A = mem_rd(gb, BC);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+19, b_+20); B = mem_rd(gb, HL);
  CYC(b_+20, b_+22); B = alu_srl(gb, B);
  CYC(b_+22, b_+24); B = alu_srl(gb, B);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_and(gb, 0x01);
  CYC(b_+29, b_+30); alu_add(gb, B);
  CYC(b_+30, b_+33); SET_BC(b_+50 /* @oamFlags */);
  CALL_C(b_+33, s_addAToBc, SYM(addAToBc), b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+38, b_+39); A = mem_rd(gb, BC);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  RET(b_+41); return;
}

// @param[out]	cflag	c if reached target position
void s_mothula_checkReachedTargetPosition_hook(GB *gb) {
  BASE(mothula_checkReachedTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR32;
  CALL_C(b_+3, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x04);
  CYC(b_+9, b_+11); alu_cp(gb, 0x09);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8F);
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  RET(b_+19); return;
}

void s_mothula_setTargetPositionToLeftOrRightSide_hook(GB *gb) {
  BASE(mothula_setTargetPositionToLeftOrRightSide);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_6c36; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
L_6c36:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); A = 0x32;
  CYC(b_+14, b_+16); B = 0xba;
  CYC(b_+16, b_+18); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+18, b_+20); goto L_6c44; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); B = 0x36;
L_6c44:
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, B);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x00);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x28);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x00);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x06);
  CYC(b_+42, b_+44);
  TAIL_S(mothula_updateAnimation);
}

// Chooses a position in one of the two center areas
void s_mothula_chooseTargetPositionWithinHoles_hook(GB *gb) {
  BASE(mothula_chooseTargetPositionWithinHoles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+4); alu_rrca(gb);
  CYC(b_+4, b_+6); A = 0x50;
  CYC(b_+6, b_+8); B = 0x68;
  if (F & FC) { CYCT(b_+8, b_+10); goto L_6c66; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); B = 0x88;
L_6c66:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, B);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x28);
  RET(b_+21); return;
}

void s_mothula_updateAngleTowardTargetPosition_hook(GB *gb) {
  BASE(mothula_updateAngleTowardTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x06);
  CALL_C(b_+7, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+10);
  CYC(b_+10, b_+11); C = A;
  CALL_C(b_+11, s_objectNudgeAngleTowards, SYM(objectNudgeAngleTowards), b_+14);
  s_mothula_updateAnimation_hook(gb); return; // falls through
}

void s_mothula_updateAnimation_hook(GB *gb) {
  BASE(mothula_updateAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+6); alu_add(gb, 0x02);
  CYC(b_+6, b_+8); alu_and(gb, 0x1c);
  CYC(b_+8, b_+9); alu_rrca(gb);
  CYC(b_+9, b_+10); alu_rrca(gb);
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16);
  TAIL(enemySetAnimation);
}

// Updates counter1 to decide how long until the angle will next be updated. This allows
// mothula to move in an oval pattern.
// @param[out]	zflag	z if completed a full circle (var31 should be reset)
void s_mothula_updateCounter1ForCirclingRoom_hook(GB *gb) {
  BASE(mothula_updateCounter1ForCirclingRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); A = alu_srl(gb, A);
  CYC(b_+8, b_+11); SET_HL(b_+26 /* @counterVals */);
  CYC(b_+11, b_+12); mothula_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); B = alu_rrc(gb, B);
  if (F & FC) { CYCT(b_+15, b_+17); goto L_6ca1; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
L_6ca1:
  CYC(b_+19, b_+21); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  RET(b_+25); return;
}

// @param[out]	zflag	z if var34 reached 0
void s_mothula_decVar34Every4Frames_hook(GB *gb) {
  BASE(mothula_decVar34Every4Frames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+10); return;
}

// Calculates appropriate angle, and decides how long to remain in state $0b (circling
// around room).
void s_mothula_initializeStateB_hook(GB *gb) {
  BASE(mothula_initializeStateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+28 /* @var34Vals */);
  CYC(b_+8, b_+9); mothula_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); A = alu_dec8(gb, A);
  CYC(b_+17, b_+19); A = 0x0c;
  if (F & FZ) { CYCT(b_+19, b_+21); goto L_6cda; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x14;
L_6cda:
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28);
  TAIL_S(mothula_updateAnimation);
}
