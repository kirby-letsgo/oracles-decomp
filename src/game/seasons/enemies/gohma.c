#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/gohma.s, bank $0e.

static uint16_t gohma_jump_table(GB *gb) {
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

static void gohma_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void gohma_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_GOHMA. Subid 0 spawns the parts; 1 is the body (relatedObj2 the claw, var31 its
// animation, var32 the gel children spawned), 2 the leg hitbox and 3 the claw (var30 set
// once Link is caught); 2 and 3 point at the body through relatedObj1.
void s_enemyCode7b_hook(GB *gb) {
  BASE(enemyCode7b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto collisionOccurred; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+14); TAIL_S(gohma_subid1_dead); }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); TAIL_S(gohma_subid3_dead); }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); A = 0x29;
  CALL_C(b_+24, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+29, b_+31); goto L_6d0a; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x0a);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);
L_6d0a:
  CYC(b_+39, b_+42);
  TAIL(enemyDie_uncounted);
collisionOccurred:
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+47, b_+49); goto L_6d25; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); A = 0x01;
  CALL_C(b_+51, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+54);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+57); alu_cp(gb, 0x7b);
  if (!(F & FZ)) { CYCT(b_+57, b_+59); goto normalStatus; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+61, b_+62); E = L;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+64); mem_wr(gb, HL, A);
  CYC(b_+64, b_+66);
  goto normalStatus;
L_6d25:
  CYC(b_+66, b_+68); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto normalStatus; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+75); A = A & 0x7f;
  CYC(b_+75, b_+77); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+77, b_+79); goto normalStatus; }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+81); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+84); alu_cp(gb, 0x60);
  if (!(F & FZ)) { CYCT(b_+84, b_+86); goto normalStatus; }
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+88); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+88, b_+90); A = 0x01;
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
normalStatus:
  CALL_C(b_+91, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+94);
  if (!(F & FC)) { CYCT(b_+94, b_+96); goto state8OrHigher; }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+97); push_effect(gb, b_+97);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == SYM(gohma_state_uninitialized) && hook_is(gb, SYM(gohma_state_uninitialized), s_gohma_state_uninitialized_hook)) { s_gohma_state_uninitialized_hook(gb); return; }
    if (jt_ == SYM(gohma_state_spawner) && hook_is(gb, SYM(gohma_state_spawner), s_gohma_state_spawner_hook)) { s_gohma_state_spawner_hook(gb); return; }
    if (jt_ == SYM(gohma_state_stub) && hook_is(gb, SYM(gohma_state_stub), s_gohma_state_stub_hook)) { s_gohma_state_stub_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state8OrHigher:
  CYC(b_+113, b_+114); B = alu_dec8(gb, B);
  CYC(b_+114, b_+115); A = B;
  CYC(b_+115, b_+116); push_effect(gb, b_+116);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == SYM(gohma_subid1) && hook_is(gb, SYM(gohma_subid1), s_gohma_subid1_hook)) { s_gohma_subid1_hook(gb); return; }
    if (jt_ == SYM(gohma_subid2) && hook_is(gb, SYM(gohma_subid2), s_gohma_subid2_hook)) { s_gohma_subid2_hook(gb); return; }
    if (jt_ == SYM(gohma_subid3) && hook_is(gb, SYM(gohma_subid3), s_gohma_subid3_hook)) { s_gohma_subid3_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// Boss room setup.
void s_gohma_state_uninitialized_hook(GB *gb) {
  BASE(gohma_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+5); TAIL(ecom_setSpeedAndState8_b0d); }
  CYC(b_+2, b_+5);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); A = 0x7b;
  CALL_C(b_+9, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+12);
  s_gohma_state_spawner_hook(gb); return; // falls through
}

// gohma_state_spawner@spawnChild: another gohma part with subid e, its relatedObj1 pointing
// at the body in c.
static void gohma_spawn_child(GB *gb) {
  BASE(gohma_state_spawner);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+40, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+43);
  CYC(b_+43, b_+44); mem_wr(gb, HL, E);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+46, b_+48); A = 0x80;
  CYC(b_+48, b_+49); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+49, b_+50); mem_wr(gb, HL, C);
  RET(b_+50); return;
}

// Subid 0: the body, then the leg hitbox and the claw, then this spawner is deleted.
void s_gohma_state_spawner_hook(GB *gb) {
  BASE(gohma_state_spawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x03;
  CALL_C(b_+2, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x7b;
  CALL_C(b_+8, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+11);
  CALL_C(b_+11, s_objectCopyPosition, SYM(objectCopyPosition), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+16, b_+17); E = L;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+20); C = H;
  CYC(b_+20, b_+22); E = 0x02;
  CALL_L(b_+22, gohma_spawn_child, b_+25);
  CYC(b_+25, b_+27); E = 0x03;
  CALL_L(b_+27, gohma_spawn_child, b_+30);
  CYC(b_+30, b_+31); A = H;
  CYC(b_+31, b_+32); H = C;
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x80);
  CYC(b_+37, b_+40);
  TAIL(enemyDelete);
}

void s_gohma_state_stub_hook(GB *gb) {
  BASE(gohma_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

// Body.
void s_gohma_subid1_hook(GB *gb) {
  BASE(gohma_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == SYM(gohma_subid1_state8) && hook_is(gb, SYM(gohma_subid1_state8), s_gohma_subid1_state8_hook)) { s_gohma_subid1_state8_hook(gb); return; }
    if (jt_ == SYM(gohma_subid1_state9) && hook_is(gb, SYM(gohma_subid1_state9), s_gohma_subid1_state9_hook)) { s_gohma_subid1_state9_hook(gb); return; }
    if (jt_ == SYM(gohma_subid1_stateA) && hook_is(gb, SYM(gohma_subid1_stateA), s_gohma_subid1_stateA_hook)) { s_gohma_subid1_stateA_hook(gb); return; }
    if (jt_ == SYM(gohma_subid1_stateB) && hook_is(gb, SYM(gohma_subid1_stateB), s_gohma_subid1_stateB_hook)) { s_gohma_subid1_stateB_hook(gb); return; }
    if (jt_ == SYM(gohma_subid1_stateC) && hook_is(gb, SYM(gohma_subid1_stateC), s_gohma_subid1_stateC_hook)) { s_gohma_subid1_stateC_hook(gb); return; }
    if (jt_ == SYM(gohma_subid1_stateD) && hook_is(gb, SYM(gohma_subid1_stateD), s_gohma_subid1_stateD_hook)) { s_gohma_subid1_stateD_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// Body initialization.
void s_gohma_subid1_state8_hook(GB *gb) {
  BASE(gohma_subid1_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0208);
  CALL_C(b_+3, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x5e);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x02);
  CYC(b_+23, b_+25); C = 0x08;
  CYC(b_+25, b_+28);
  TAIL(ecom_setZAboveScreen_b0d);
}

// Following Link along the ceiling.
void s_gohma_subid1_state9_hook(GB *gb) {
  BASE(gohma_subid1_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto updatePosition; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x1e);
  CYC(b_+7, b_+9); C = 0x28;
  CALL_C(b_+9, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+12);
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto updatePosition; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+19); A = 0x1a;
  CALL_C(b_+19, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+24, s_objectSetVisible81, SYM(objectSetVisible81), b_+27);
  CYC(b_+27, b_+29); C = 0x08;
  CALL_C(b_+29, s_ecom_setZAboveScreen, SYM(ecom_setZAboveScreen_b0d), b_+32);
updatePosition:
  CALL_C(b_+32, s_ecom_updateCardinalAngleTowardTarget, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+35);
  CALL_C(b_+35, s_gohma_updateSpeedWhileFalling_hook, SYM(gohma_updateSpeedWhileFalling), b_+38);
  CALL_C(b_+38, s_objectApplySpeed, SYM(objectApplySpeed), b_+41);
  CYC(b_+41, b_+43); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); alu_cp(gb, 0x20);
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto L_6dfd; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); A = 0x20;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  RET(b_+51); return;
L_6dfd:
  CYC(b_+52, b_+54); alu_cp(gb, 0xd0);
  if (F & FC) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); A = 0xd0;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  RET(b_+58); return;
}

// Falling down.
void s_gohma_subid1_stateA_hook(GB *gb) {
  BASE(gohma_subid1_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto hitGround; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0xf9);
  if (F & FC) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x02;
  CYC(b_+13, b_+16);
  TAIL_S(gohma_setAnimation);
hitGround:
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x3c);
  CYC(b_+23, b_+25); A = 0x2e;
  CYC(b_+25, b_+28); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+28, s_playSound, SYM(playSound_b00), b_+31);
  CYC(b_+31, b_+33); A = 0x81;
  CALL_C(b_+33, s_playSound, SYM(playSound_b00), b_+36);
  CYC(b_+36, b_+38); A = 0x28;
  CALL_C(b_+38, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+41);
  CYC(b_+41, b_+44);
  TAIL(objectSetVisible83);
}

// Standing in place.
void s_gohma_subid1_stateB_hook(GB *gb) {
  BASE(gohma_subid1_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x2d);
  RET(b_+11); return;
}

// Phase 1 of the fight, claw intact: stand, walk, lunge.
void s_gohma_subid1_stateC_hook(GB *gb) {
  BASE(gohma_subid1_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_gohma_subid1_updateAnimationsAndCollisions_hook, SYM(gohma_subid1_updateAnimationsAndCollisions), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == b_+17) goto substate0;
    if (jt_ == b_+35) goto substate1;
    if (jt_ == SYM(gohma_stateC_substate2) && hook_is(gb, SYM(gohma_stateC_substate2), s_gohma_stateC_substate2_hook)) { s_gohma_stateC_substate2_hook(gb); return; }
    if (jt_ == SYM(gohma_stateC_substate3) && hook_is(gb, SYM(gohma_stateC_substate3), s_gohma_stateC_substate3_hook)) { s_gohma_stateC_substate3_hook(gb); return; }
    if (jt_ == SYM(gohma_stateC_substate4) && hook_is(gb, SYM(gohma_stateC_substate4), s_gohma_stateC_substate4_hook)) { s_gohma_stateC_substate4_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+17, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); L = E;
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+23, s_gohma_phase1_decideAngle_hook, SYM(gohma_phase1_decideAngle), b_+26);
  CALL_C(b_+26, s_gohma_decideMovementDuration_hook, SYM(gohma_decideMovementDuration), b_+29);
  CALL_C(b_+29, s_gohma_decideAnimation_hook, SYM(gohma_decideAnimation), b_+32);
  CYC(b_+32, b_+35);
  TAIL_S(gohma_updateSpeedWhileFalling);
substate1:
  CALL_C(b_+35, s_enemyAnimate, SYM(enemyAnimate), b_+38);
  CALL_C(b_+38, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+41);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+44, b_+47); A = mem_rd(gb, w1Link_yh);
  CYC(b_+47, b_+48); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+50); alu_cp(gb, 0x28);
  if (!(F & FC)) { CYCT(b_+50, b_+52); TAIL_S(gohma_movingNormally); }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+54, b_+57); A = mem_rd(gb, w1Link_xh);
  CYC(b_+57, b_+58); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+58, b_+60); alu_add(gb, 0x18);
  CYC(b_+60, b_+62); alu_cp(gb, 0x25);
  if (!(F & FC)) { CYCT(b_+62, b_+64); TAIL_S(gohma_movingNormally); }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+66, b_+67); A = mem_rd(gb, HL);
  CYC(b_+67, b_+68); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+68, b_+70); TAIL_S(gohma_beginLungeTowardLink); }
  CYC(b_+68, b_+70);
  s_gohma_movingNormally_hook(gb); return; // falls through
}

void s_gohma_movingNormally_hook(GB *gb) {
  BASE(gohma_movingNormally);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_gohma_checkWallsAndPlayWalkingSound_hook, SYM(gohma_checkWallsAndPlayWalkingSound), b_+3);
  CALL_C(b_+3, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+10, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x07);
  CYC(b_+15, b_+18); SET_HL(SYM(gohma_counter1Vals));
  CYC(b_+18, b_+19); gohma_add_a_to_hl(gb, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  RET(b_+23); return;
}

void s_gohma_beginLungeTowardLink_hook(GB *gb) {
  BASE(gohma_beginLungeTowardLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x1f);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x78);
  CYC(b_+10, b_+12); A = 0x29;
  CALL_C(b_+12, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+17, b_+19); goto L_6eb5; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0c);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+25, b_+27); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
L_6eb5:
  CYC(b_+27, b_+29); A = 0x09;
  s_gohma_setAnimation_hook(gb); return; // falls through
}

void s_gohma_setAnimation_hook(GB *gb) {
  BASE(gohma_setAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6);
  TAIL(enemySetAnimation);
}

// Lunging toward Link, or moving back, with the claw.
void s_gohma_stateC_substate2_hook(GB *gb) {
  BASE(gohma_stateC_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto doneLunge; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = A & 0x7f;
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16);
  TAIL_S(gohma_updateLunge);
doneLunge:
  CYC(b_+16, b_+19); A = mem_rd(gb, w1Link_state);
  CYC(b_+19, b_+21); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+21, b_+23); TAIL_S(gohma_stateC_setSubstate4); }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x14);
  RET(b_+30); return;
}

// Link grabbed.
void s_gohma_stateC_setSubstate4_hook(GB *gb) {
  BASE(gohma_stateC_setSubstate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+4); A = 0x04;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x0a;
  CYC(b_+7, b_+9);
  TAIL_S(gohma_setAnimation);
}

// Standing in place after a lunge.
void s_gohma_stateC_substate3_hook(GB *gb) {
  BASE(gohma_stateC_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_state);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+5, b_+7); TAIL_S(gohma_stateC_setSubstate4); }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28);
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x02);
  RET(b_+22); return;
}

// Holding Link.
void s_gohma_stateC_substate4_hook(GB *gb) {
  BASE(gohma_stateC_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+6, b_+8); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x3c);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x02);
  RET(b_+20); return;
}

// Phase 2, claw destroyed: moves around spawning gel children.
void s_gohma_subid1_stateD_hook(GB *gb) {
  BASE(gohma_subid1_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_gohma_subid1_updateAnimationsAndCollisions_hook, SYM(gohma_subid1_updateAnimationsAndCollisions), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == b_+13) goto substate0;
    if (jt_ == b_+57) goto substate1;
    if (jt_ == b_+63) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+13, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto chooseNextMovement; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto chooseNextMovement; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x02);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x01);
  CYC(b_+35, b_+37); A = 0x0b;
  CYC(b_+37, b_+40);
  TAIL_S(gohma_setAnimation);
chooseNextMovement:
  CALL_C(b_+40, s_ecom_setRandomCardinalAngle, SYM(ecom_setRandomCardinalAngle_b0d), b_+43);
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+45, b_+47); A = 0x01;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CALL_C(b_+48, s_gohma_decideMovementDuration_hook, SYM(gohma_decideMovementDuration), b_+51);
  CALL_C(b_+51, s_gohma_decideAnimation_hook, SYM(gohma_decideAnimation), b_+54);
  CYC(b_+54, b_+57);
  TAIL_S(gohma_updateSpeedWhileFalling);
substate1:
  CALL_C(b_+57, s_enemyAnimate, SYM(enemyAnimate), b_+60);
  CYC(b_+60, b_+63);
  TAIL_S(gohma_movingNormally);
substate2:
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+67, b_+69); goto chooseNextMovement; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); A = A & 0x7f;
  CYC(b_+71, b_+72); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+72, b_+74); goto animate; }
  CYC(b_+72, b_+74);
  CALL_C(b_+74, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+77);
  if (F & FZ) CALL_C_CC(b_+77, s_gohma_phase2_spawnGelChild_hook, SYM(gohma_phase2_spawnGelChild), b_+80);
  else CYC(b_+77, b_+80);
animate:
  CYC(b_+80, b_+83);
  TAIL(enemyAnimate);
}

// The leg hitbox, which follows the body.
void s_gohma_subid2_hook(GB *gb) {
  BASE(gohma_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == b_+8) goto state8;
    if (jt_ == b_+24) goto state9;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+10); L = E;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+15, b_+17); A = 0x01;
  CALL_C(b_+17, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
state9:
  CYC(b_+24, b_+26); A = 0x29;
  CALL_C(b_+26, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+31, b_+34); TAIL(enemyDelete); }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+37); SET_BC(IO_P1);
  CYC(b_+37, b_+39); A = 0x00;
  CALL_C(b_+39, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+42);
  CYC(b_+42, b_+45);
  TAIL(objectTakePositionWithOffset);
}

// gohma_subid3@updateNormalPosition: the claw held at (+8,-6) from the body.
static void gohma_claw_follow_body(GB *gb) {
  BASE(gohma_subid3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+112, b_+115); SET_BC(0x08fa);
  CYC(b_+115, b_+117); A = 0x00;
  CALL_C(b_+117, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+120);
  CYC(b_+120, b_+123);
  TAIL(objectTakePositionWithOffset);
}

// The claw: falls with the body, blocks the eye, lunges, grabs Link and slams him.
void s_gohma_subid3_hook(GB *gb) {
  BASE(gohma_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (gohma_jump_table(gb));
    if (jt_ == b_+20) goto state8;
    if (jt_ == b_+34) goto state9;
    if (jt_ == b_+80) goto stateA;
    if (jt_ == b_+92) goto stateB;
    if (jt_ == b_+123) goto stateC;
    if (jt_ == b_+143) goto stateD;
    if (jt_ == b_+212) goto stateE;
    if (jt_ == b_+235) goto stateF;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+20, b_+22); A = 0x09;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, s_objectSetVisible81, SYM(objectSetVisible81), b_+26);
  CALL_C(b_+26, s_objectSetInvisible, SYM(objectSetInvisible), b_+29);
  CYC(b_+29, b_+31); A = 0x0c;
  CYC(b_+31, b_+34);
  TAIL(enemySetAnimation);
state9:
  CYC(b_+34, b_+36); A = 0x04;
  CALL_C(b_+36, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+42, b_+44); goto falling; }
  CYC(b_+42, b_+44);
  if (F & FC) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+46); H = D;
  CYC(b_+46, b_+48); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x5f);
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x00);
  CYC(b_+57, b_+60);
  TAIL(objectSetVisible82);
falling:
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+65); alu_cp(gb, 0xf9);
  if (!(F & FC)) { CYCT(b_+65, b_+67); goto closeToGround; }
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+70); SET_BC(0xf806);
  CYC(b_+70, b_+73);
  TAIL(objectTakePositionWithOffset);
closeToGround:
  CYC(b_+73, b_+75); A = 0x0d;
  CALL_C(b_+75, s_enemySetAnimation, SYM(enemySetAnimation), b_+78);
  CYC(b_+78, b_+80);
  goto updateNormalPosition;
stateA:
  CALL_C(b_+80, s_gohma_checkShouldBlock_hook, SYM(gohma_checkShouldBlock), b_+83);
  CALL_C(b_+83, s_gohma_updateCollisionsEnabled_hook, SYM(gohma_updateCollisionsEnabled), b_+86);
  CALL_L(b_+86, gohma_claw_follow_body, b_+89);
  CYC(b_+89, b_+92);
  TAIL(enemyAnimate);
stateB:
  CALL_C(b_+92, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+95);
  if (!(F & FZ)) { CYCT(b_+95, b_+98); TAIL_S(gohma_claw_updateBlockingPosition); }
  CYC(b_+95, b_+98);
gotoStateA:
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+101); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x0a);
  CYC(b_+103, b_+105); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x5f);
  CYC(b_+107, b_+109); A = 0x0d;
  CALL_C(b_+109, s_enemySetAnimation, SYM(enemySetAnimation), b_+112);
updateNormalPosition:
  CYC(b_+112, b_+115); SET_BC(0x08fa);
  CYC(b_+115, b_+117); A = 0x00;
  CALL_C(b_+117, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+120);
  CYC(b_+120, b_+123);
  TAIL(objectTakePositionWithOffset);
stateC:
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+125); L = E;
  CYC(b_+125, b_+126); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+126, b_+128); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x00);
  CYC(b_+130, b_+132); A = 0x0f;
  CALL_C(b_+132, s_enemySetAnimation, SYM(enemySetAnimation), b_+135);
  CYC(b_+135, b_+137); A = 0x74;
  CALL_C(b_+137, s_playSound, SYM(playSound_b00), b_+140);
  CYC(b_+140, b_+143);
  TAIL_S(gohma_claw_updatePositionInLunge);
stateD:
  CALL_C(b_+143, s_enemyAnimate, SYM(enemyAnimate), b_+146);
  CYC(b_+146, b_+147); H = D;
  CYC(b_+147, b_+149); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+149, b_+151); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+151, b_+153); goto label_0e_308; }
  CYC(b_+151, b_+153);
  CYC(b_+153, b_+155); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+155, b_+157); goto L_7034; }
  CYC(b_+155, b_+157);
  CYC(b_+157, b_+159); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(b_+159, b_+161); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0x60);
L_7034:
  CALL_C(b_+163, s_gohma_claw_updatePositionInLunge_hook, SYM(gohma_claw_updatePositionInLunge), b_+166);
  CYC(b_+166, b_+168); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+168, b_+169); A = mem_rd(gb, DE);
  CYC(b_+169, b_+170); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+170); return; }
  CYC(b_+170, b_+171);
  CYC(b_+171, b_+173);
  goto linkCaught;
label_0e_308:
  CYC(b_+173, b_+175); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+177); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+177, b_+180); goto gotoStateA; }
  CYC(b_+177, b_+180);
  CYC(b_+180, b_+181); alu_xor(gb, A);
  CALL_C(b_+181, s_gohma_claw_setPositionInLunge_hook, SYM(gohma_claw_setPositionInLunge), b_+184);
linkCaught:
  CYC(b_+184, b_+186); A = 0x10;
  CALL_C(b_+186, s_enemySetAnimation, SYM(enemySetAnimation), b_+189);
  CYC(b_+189, b_+192); SET_HL(w1Link_var31);
  CYC(b_+192, b_+194); mem_wr(gb, HL, 0x0d);
  CYC(b_+194, b_+196); L = 0x30;
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0x00);
  CYC(b_+198, b_+200); L = 0x24;
  CYC(b_+200, b_+202); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+202, b_+203); H = D;
  CYC(b_+203, b_+205); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+205, b_+206); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+206, b_+208); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+208, b_+210); mem_wr(gb, HL, 0x08);
  CYC(b_+210, b_+212);
  goto updateLinkPosition;
stateE:
  CYC(b_+212, b_+214); A = 0x05;
  CALL_C(b_+214, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+217);
  CYC(b_+217, b_+218); A = mem_rd(gb, HL);
  CYC(b_+218, b_+220); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+220, b_+222); goto donePullingBack; }
  CYC(b_+220, b_+222);
  CALL_C(b_+222, s_gohma_claw_updatePositionInLunge_hook, SYM(gohma_claw_updatePositionInLunge), b_+225);
  CYC(b_+225, b_+227);
  goto updateLinkPosition;
donePullingBack:
  CYC(b_+227, b_+228); H = D;
  CYC(b_+228, b_+229); L = alu_dec8(gb, L);
  CYC(b_+229, b_+230); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+230, b_+232); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+232, b_+234); mem_wr(gb, HL, 0x00);
  RET(b_+234); return;
stateF:
  CALL_C(b_+235, s_enemyAnimate, SYM(enemyAnimate), b_+238);
  CYC(b_+238, b_+239); H = D;
  CYC(b_+239, b_+241); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+241, b_+243); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+243, b_+246); goto gotoStateA; }
  CYC(b_+243, b_+246);
  CYC(b_+246, b_+248); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+248, b_+250); goto label_0e_311; }
  CYC(b_+248, b_+250);
  CYC(b_+250, b_+252); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+252, b_+255); TAIL_S(gohma_updateClawPositionDuringSlamAttack); }
  CYC(b_+252, b_+255);
  CYC(b_+255, b_+257); mem_wr(gb, HL, mem_rd(gb, HL) & 0xef);
  CYC(b_+257, b_+260); SET_HL(w1Link_substate);
  CYC(b_+260, b_+262); mem_wr(gb, HL, 0x02);
  CYC(b_+262, b_+264); L = 0x24;
  CYC(b_+264, b_+266); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+266, b_+269);
  TAIL_S(gohma_updateClawPositionDuringSlamAttack);
label_0e_311:
  CYC(b_+269, b_+271); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+271, b_+273); goto L_70bb; }
  CYC(b_+271, b_+273);
  CYC(b_+273, b_+275); mem_wr(gb, HL, mem_rd(gb, HL) & 0xef);
  CYC(b_+275, b_+277); A = 0x6f;
  CALL_C(b_+277, s_playSound, SYM(playSound_b00), b_+280);
  CYC(b_+280, b_+282); A = 0x14;
  CALL_C(b_+282, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+285);
  CYC(b_+285, b_+287); A = 0x0a;
  CYC(b_+287, b_+290); mem_wr(gb, w1Link_invincibilityCounter, A);
  CYC(b_+290, b_+292); A = 0xfa;
  CYC(b_+292, b_+295); mem_wr(gb, w1Link_damageToApply, A);
  CYC(b_+295, b_+296); H = D;
  CYC(b_+296, b_+298); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
L_70bb:
  CALL_C(b_+298, s_gohma_updateLinkAnimAndClawPositionDuringSlamAttack_hook, SYM(gohma_updateLinkAnimAndClawPositionDuringSlamAttack), b_+301);
updateLinkPosition:
  CYC(b_+301, b_+304); SET_BC(0x0002);
  CYC(b_+304, b_+307); SET_HL(w4TileMap);
  CYC(b_+307, b_+310);
  TAIL(objectCopyPositionWithOffset);
}

// Fast vertically, slow horizontally while falling.
void s_gohma_updateSpeedWhileFalling_hook(GB *gb) {
  BASE(gohma_updateSpeedWhileFalling);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+5); alu_bit(gb, 3, mem_rd(gb, HL));
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x14);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x3c);
  RET(b_+12); return;
}

// Turns around at walls; the walking sound.
void s_gohma_checkWallsAndPlayWalkingSound_hook(GB *gb) {
  BASE(gohma_checkWallsAndPlayWalkingSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rrca(gb);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+15); SET_HL(SYM(gohma_positionOffsets));
  CYC(b_+15, b_+16); gohma_add_a_to_hl(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); alu_add(gb, B);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+22); C = A;
  CALL_C(b_+22, s_getTileCollisionsAtPosition, SYM(getTileCollisionsAtPosition), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+27); TAIL_S(gohma_updateMovement); }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+32); alu_xor(gb, 0x10);
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  s_gohma_updateMovement_hook(gb); return; // falls through
}

void s_gohma_updateMovement_hook(GB *gb) {
  BASE(gohma_updateMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x09);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0f);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto L_7105; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0x98);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto updateWalkingSound; }
  CYC(b_+14, b_+16);
L_7105:
  CALL_C(b_+16, s_objectApplySpeed, SYM(objectApplySpeed), b_+19);
updateWalkingSound:
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_bit(gb, 3, A);
  CYC(b_+24, b_+26); B = 0x07;
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto L_7113; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); B = 0x0f;
L_7113:
  CYC(b_+30, b_+33); A = mem_rd(gb, wFrameCounter);
  CYC(b_+33, b_+34); alu_and(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); A = 0xa3;
  CYC(b_+37, b_+40);
  TAIL(playSound_b00);
}

// Body and claw collisions on or off with the animation.
void s_gohma_updateCollisionsEnabled_hook(GB *gb) {
  BASE(gohma_updateCollisionsEnabled);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+8);
  CYC(b_+8, b_+10); alu_add(gb, 0x06);
  CYC(b_+10, b_+12); alu_and(gb, 0x1f);
  CYC(b_+12, b_+14); alu_cp(gb, 0x0d);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  if (F & FC) { CYCT(b_+17, b_+19); goto L_713b; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+21); return;
L_713b:
  CYC(b_+22, b_+24); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  RET(b_+24); return;
}

// The body has no health left.
void s_gohma_subid1_dead_hook(GB *gb) {
  BASE(gohma_subid1_dead);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto dead; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); H = 0xd0;
nextEnemy:
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x46);
  if (F & FZ) CALL_C_CC(b_+13, s_ecom_killObjectH, SYM(ecom_killObjectH_b0d), b_+16);
  else CYC(b_+13, b_+16);
  CYC(b_+16, b_+17); H = alu_inc8(gb, H);
  CYC(b_+17, b_+18); A = H;
  CYC(b_+18, b_+20); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+20, b_+22); goto nextEnemy; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x01;
  CALL_C(b_+24, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_cp(gb, 0x7b);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto dead; }
  CYC(b_+30, b_+32);
  CALL_C(b_+32, s_ecom_killObjectH, SYM(ecom_killObjectH_b0d), b_+35);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x80);
dead:
  CYC(b_+39, b_+42);
  TAIL(enemyBoss_dead_b0f);
}

// The claw has no health left.
void s_gohma_subid3_dead_hook(GB *gb) {
  BASE(gohma_subid3_dead);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_7184; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x1e);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x18);
  CYC(b_+17, b_+20); SET_BC(0xff20);
  CALL_C(b_+20, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+23);
  CYC(b_+23, b_+25); A = 0x11;
  CALL_C(b_+25, s_enemySetAnimation, SYM(enemySetAnimation), b_+28);
L_7184:
  CYC(b_+28, b_+30); C = 0x0a;
  CALL_C(b_+30, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+33);
  CALL_C(b_+33, s_objectApplySpeed, SYM(objectApplySpeed), b_+36);
  CYC(b_+36, b_+39);
  TAIL(enemyAnimate);
}

void s_gohma_subid1_updateAnimationsAndCollisions_hook(GB *gb) {
  BASE(gohma_subid1_updateAnimationsAndCollisions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto updateCollision; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto updateCollision; }
  CYC(b_+10, b_+12);
  CALL_C(b_+12, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x30);
  CYC(b_+17, b_+19); alu_add(gb, 0xa0);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_sub(gb, 0x02);
  CYC(b_+25, b_+27); A = alu_swap(gb, A);
  CYC(b_+27, b_+28); alu_cpl(gb);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
  CYC(b_+29, b_+30); alu_rrca(gb);
  CYC(b_+30, b_+31); alu_add(gb, B);
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+40); alu_add(gb, 0x18);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+46); alu_xor(gb, 0x04);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+52); A = alu_swap(gb, A);
  CYC(b_+52, b_+53); alu_rlca(gb);
  CYC(b_+53, b_+55); alu_and(gb, 0x02);
  CYC(b_+55, b_+56); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+56, b_+57); E = alu_dec8(gb, E);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+59); A = alu_dec8(gb, A);
  CALL_C(b_+59, s_enemySetAnimation, SYM(enemySetAnimation), b_+62);
updateCollision:
  CYC(b_+62, b_+64); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); alu_rlca(gb);
  if (F & FC) { CYCT(b_+66, b_+69); TAIL_S(gohma_updateCollisionsEnabled); }
  CYC(b_+66, b_+69);
  CYC(b_+69, b_+71); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+74); A = A & 0x7f;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  RET(b_+75); return;
}

void s_gohma_phase1_decideAngle_hook(GB *gb) {
  BASE(gohma_phase1_decideAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x60);
  if (!(F & FC)) { CYCT(b_+8, b_+10); goto setAngle; }
  CYC(b_+8, b_+10);
  CALL_C(b_+10, s_getRandomNumber, SYM(getRandomNumber), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x07);
  if (F & FZ) { CYCT(b_+15, b_+18); TAIL(ecom_setRandomCardinalAngle_b0d); }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, w1Link_yh);
  CYC(b_+21, b_+22); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+24); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(b_+24, b_+26); goto checkHorizontal; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); B = 0x10;
  CYC(b_+28, b_+30); alu_cp(gb, 0x80);
  if (F & FC) { CYCT(b_+30, b_+32); goto setAngle; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); B = 0x00;
  CYC(b_+34, b_+36); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+36, b_+38); goto setAngle; }
  CYC(b_+36, b_+38);
checkHorizontal:
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+40, b_+43); A = mem_rd(gb, w1Link_xh);
  CYC(b_+43, b_+44); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+44, b_+46); B = 0x18;
  if (F & FC) { CYCT(b_+46, b_+48); goto setAngle; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); B = 0x08;
setAngle:
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+52, b_+53); mem_wr(gb, HL, B);
  RET(b_+53); return;
}

void s_gohma_decideMovementDuration_hook(GB *gb) {
  BASE(gohma_decideMovementDuration);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+23);
  CYC(b_+8, b_+9); gohma_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_and(gb, 0x08);
  CYC(b_+14, b_+15); alu_add(gb, A);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_cpl(gb);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

void s_gohma_decideAnimation_hook(GB *gb) {
  BASE(gohma_decideAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); A = alu_swap(gb, A);
  CYC(b_+8, b_+9); alu_rlca(gb);
  CYC(b_+9, b_+11); alu_and(gb, 0x02);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+20);
  TAIL(enemySetAnimation);
}

void s_gohma_updateLunge_hook(GB *gb) {
  BASE(gohma_updateLunge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_cp(gb, 0x1e);
  CYC(b_+7, b_+8); push_effect(gb, AF);
  if (F & FZ) CALL_C_CC(b_+8, s_gohma_initAngleForLungeAtLink_hook, SYM(gohma_initAngleForLungeAtLink), b_+11);
  else CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); SET_AF(pop_effect(gb));
  CYC(b_+12, b_+14); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); TAIL_S(gohma_updateMovement); }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_xor(gb, 0x10);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+26);
  TAIL_S(gohma_updateMovement);
}

void s_gohma_initAngleForLungeAtLink_hook(GB *gb) {
  BASE(gohma_initAngleForLungeAtLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+4); A = 0x0d;
  CALL_C(b_+4, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_xh);
  CYC(b_+10, b_+11); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+13); alu_add(gb, 0x06);
  CYC(b_+13, b_+15); alu_cp(gb, 0x0d);
  if (F & FC) { CYCT(b_+15, b_+17); goto setAngle; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); B = 0xfe;
  CYC(b_+19, b_+21); alu_cp(gb, 0x86);
  if (F & FC) { CYCT(b_+21, b_+23); goto setAngle; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); B = 0x02;
setAngle:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+29); A = 0x10;
  CYC(b_+29, b_+30); alu_add(gb, B);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); A = 0xbb;
  CYC(b_+33, b_+36);
  TAIL(playSound_b00);
}

void s_gohma_phase2_spawnGelChild_hook(GB *gb) {
  BASE(gohma_phase2_spawnGelChild);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x07);
  CYC(b_+2, b_+4); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CALL_C(b_+8, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x03);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+16); B = 0x46;
  CALL_C(b_+16, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, C);
  CALL_C(b_+21, s_objectCopyPosition, SYM(objectCopyPosition), b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+26, b_+28); A = 0x80;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, D);
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+34, b_+36); A = 0x62;
  CYC(b_+36, b_+39);
  TAIL(playSound_b00);
}

void s_gohma_checkShouldBlock_hook(GB *gb) {
  BASE(gohma_checkShouldBlock);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkUsingItem1);
  CYC(b_+3, b_+5); alu_and(gb, 0xf0);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x0b;
  CALL_C(b_+8, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_yh);
  CYC(b_+14, b_+15); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+17); alu_cp(gb, 0x2c);
  if (F & FC) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); H = 0xd6;
nextItem:
  CYC(b_+20, b_+22); L = 0x01;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 0x1a);
  if (!(F & FC)) { CYCT(b_+25, b_+27); goto beginBlock; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); H = alu_inc8(gb, H);
  CYC(b_+28, b_+29); A = H;
  CYC(b_+29, b_+31); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+31, b_+33); goto nextItem; }
  CYC(b_+31, b_+33);
  RET(b_+33); return;
beginBlock:
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x3c);
  CYC(b_+42, b_+44); A = 0x0e;
  CALL_C(b_+44, s_enemySetAnimation, SYM(enemySetAnimation), b_+47);
  s_gohma_claw_updateBlockingPosition_hook(gb); return; // falls through
}

void s_gohma_claw_updateBlockingPosition_hook(GB *gb) {
  BASE(gohma_claw_updateBlockingPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x07ff);
  CYC(b_+3, b_+5); A = 0x00;
  CALL_C(b_+5, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+8);
  CYC(b_+8, b_+11);
  TAIL(objectTakePositionWithOffset);
}

void s_gohma_claw_updatePositionInLunge_hook(GB *gb) {
  BASE(gohma_claw_updatePositionInLunge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  s_gohma_claw_setPositionInLunge_hook(gb); return; // falls through
}

void s_gohma_claw_setPositionInLunge_hook(GB *gb) {
  BASE(gohma_claw_setPositionInLunge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+15);
  CYC(b_+3, b_+4); gohma_add_a_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+9); A = 0x00;
  CALL_C(b_+9, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+15);
  TAIL(objectTakePositionWithOffset);
}

void s_gohma_updateLinkAnimAndClawPositionDuringSlamAttack_hook(GB *gb) {
  BASE(gohma_updateLinkAnimAndClawPositionDuringSlamAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+4); SET_HL(SYM(gohma_linkVar31Stuff));
  CYC(b_+4, b_+5); gohma_add_a_to_hl(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+9); mem_wr(gb, w1Link_var31, A);
  s_gohma_updateClawPositionDuringSlamAttack_hook(gb); return; // falls through
}

void s_gohma_updateClawPositionDuringSlamAttack_hook(GB *gb) {
  BASE(gohma_updateClawPositionDuringSlamAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(SYM(gohma_clawSlamPositionOffsets));
  CYC(b_+8, b_+9); gohma_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); C = mem_rd(gb, HL);
  CYC(b_+12, b_+14); A = 0x00;
  CALL_C(b_+14, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+17);
  CYC(b_+17, b_+20);
  TAIL(objectTakePositionWithOffset);
}
