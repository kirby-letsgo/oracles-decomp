#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/aquamentus.s.

static uint16_t aquamentus_jump_table(GB *gb) {
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

static void aquamentus_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void aquamentus_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_AQUAMENTUS, the Gnarled Root boss. Subid 0 spawns the parts; 1 is the body, 2 a
// hitbox and 3 the horn.
void s_enemyCode78_hook(GB *gb) {
  BASE(enemyCode78);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto justHit; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto normalStatus; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_sub(gb, 0x02);
  if (F & FZ) { CYCT(b_+16, b_+19); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); TAIL(enemyDelete); }
  CYC(b_+20, b_+23);
  CALL_C(b_+23, s_ecom_killRelatedObj1, SYM(ecom_killRelatedObj1_b0d), b_+26);
  CALL_C(b_+26, s_ecom_killRelatedObj2, SYM(ecom_killRelatedObj2_b0d), b_+29);
  CYC(b_+29, b_+32);
  TAIL(enemyDie_uncounted_withoutItemDrop);
justHit:
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+37); alu_sub(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto normalStatus; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0x2b;
  CALL_C(b_+41, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+44);
  CYC(b_+44, b_+45); E = L;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
normalStatus:
  CALL_C(b_+47, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+50);
  if (!(F & FC)) { CYCT(b_+50, b_+52); goto state8OrHigher; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); push_effect(gb, b_+53);
  do { uint16_t jt_ = (aquamentus_jump_table(gb));
    if (jt_ == SYM(aquamentus_state_uninitialized) && hook_is(gb, SYM(aquamentus_state_uninitialized), s_aquamentus_state_uninitialized_hook)) { s_aquamentus_state_uninitialized_hook(gb); return; }
    if (jt_ == SYM(aquamentus_state_spawner) && hook_is(gb, SYM(aquamentus_state_spawner), s_aquamentus_state_spawner_hook)) { s_aquamentus_state_spawner_hook(gb); return; }
    if (jt_ == SYM(aquamentus_state_stub) && hook_is(gb, SYM(aquamentus_state_stub), s_aquamentus_state_stub_hook)) { s_aquamentus_state_stub_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state8OrHigher:
  CYC(b_+69, b_+70); B = alu_dec8(gb, B);
  CYC(b_+70, b_+71); A = B;
  CYC(b_+71, b_+72); push_effect(gb, b_+72);
  do { uint16_t jt_ = (aquamentus_jump_table(gb));
    if (jt_ == SYM(aquamentus_subid1) && hook_is(gb, SYM(aquamentus_subid1), s_aquamentus_subid1_hook)) { s_aquamentus_subid1_hook(gb); return; }
    if (jt_ == SYM(aquamentus_subid2) && hook_is(gb, SYM(aquamentus_subid2), s_aquamentus_subid2_hook)) { s_aquamentus_subid2_hook(gb); return; }
    if (jt_ == SYM(aquamentus_subid3) && hook_is(gb, SYM(aquamentus_subid3), s_aquamentus_subid3_hook)) { s_aquamentus_subid3_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_aquamentus_state_uninitialized_hook(GB *gb) {
  BASE(aquamentus_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_ecom_setZAboveScreen, SYM(ecom_setZAboveScreen_b0d), b_+5);
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); TAIL(ecom_setSpeedAndState8_b0d); }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+14); A = 0x78;
  CYC(b_+14, b_+16); B = 0x80;
  CYC(b_+16, b_+19);
  TAIL(enemyBoss_initializeRoom_b0f);
}

void s_aquamentus_state_spawner_hook(GB *gb) {
  BASE(aquamentus_state_spawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); B = 0x03;
  CALL_C(b_+7, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); B = 0x78;
  CALL_C(b_+13, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+16);
  CYC(b_+16, b_+17); C = H;
  CALL_C(b_+17, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+21, s_aquamentus_initializeChildObject_hook, SYM(aquamentus_initializeChildObject), b_+24);
  CYC(b_+24, b_+25); push_effect(gb, HL);
  CALL_C(b_+25, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x03);
  CALL_C(b_+30, s_aquamentus_initializeChildObject_hook, SYM(aquamentus_initializeChildObject), b_+33);
  CYC(b_+33, b_+34); E = H;
  CYC(b_+34, b_+35); SET_HL(pop_effect(gb));
  CYC(b_+35, b_+36); A = H;
  CYC(b_+36, b_+37); H = C;
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+41); mem_wr(gb, HL, E);
  CALL_C(b_+41, s_objectCopyPosition, SYM(objectCopyPosition), b_+44);
  CYC(b_+44, b_+47);
  TAIL(enemyDelete);
}

void s_aquamentus_state_stub_hook(GB *gb) {
  BASE(aquamentus_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

// Body.
void s_aquamentus_subid1_hook(GB *gb) {
  BASE(aquamentus_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (aquamentus_jump_table(gb));
    if (jt_ == SYM(aquamentus_body_state8) && hook_is(gb, SYM(aquamentus_body_state8), s_aquamentus_body_state8_hook)) { s_aquamentus_body_state8_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_state9) && hook_is(gb, SYM(aquamentus_body_state9), s_aquamentus_body_state9_hook)) { s_aquamentus_body_state9_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_stateA) && hook_is(gb, SYM(aquamentus_body_stateA), s_aquamentus_body_stateA_hook)) { s_aquamentus_body_stateA_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_stateB) && hook_is(gb, SYM(aquamentus_body_stateB), s_aquamentus_body_stateB_hook)) { s_aquamentus_body_stateB_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_stateC) && hook_is(gb, SYM(aquamentus_body_stateC), s_aquamentus_body_stateC_hook)) { s_aquamentus_body_stateC_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_stateD) && hook_is(gb, SYM(aquamentus_body_stateD), s_aquamentus_body_stateD_hook)) { s_aquamentus_body_stateD_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_stateE) && hook_is(gb, SYM(aquamentus_body_stateE), s_aquamentus_body_stateE_hook)) { s_aquamentus_body_stateE_hook(gb); return; }
    if (jt_ == SYM(aquamentus_body_stateF) && hook_is(gb, SYM(aquamentus_body_stateF), s_aquamentus_body_stateF_hook)) { s_aquamentus_body_stateF_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_aquamentus_body_state8_hook(GB *gb) {
  BASE(aquamentus_body_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x020c);
  CALL_C(b_+3, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x5d);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+17, b_+19); A = 0x50;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xc0);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x01);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x5a);
  RET(b_+30); return;
}

void s_aquamentus_body_state9_hook(GB *gb) {
  BASE(aquamentus_body_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xf4);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto doneLowering; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0xf0);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+14); SET_HL(SYM(aquamentus_fallingSpeeds));
  CYC(b_+14, b_+15); aquamentus_add_a_to_hl(gb, b_+15);
  CYC(b_+15, b_+16); E = alu_dec8(gb, E);
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_adc(gb, 0x00);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+27);
  TAIL_S(aquamentus_playHoverSoundEvery32Frames);
doneLowering:
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x02);
  RET(b_+35); return;
}

void s_aquamentus_body_stateA_hook(GB *gb) {
  BASE(aquamentus_body_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); TAIL_S(aquamentus_playHoverSoundEvery32Frames); }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x3c);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x04);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+20, b_+22); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto L_6457; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+27); A = 0x2e;
  CYC(b_+27, b_+30); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+30, s_playSound, SYM(playSound_b00), b_+33);
L_6457:
  CYC(b_+33, b_+35); A = 0x20;
  s_aquamentus_body_pound_hook(gb); return; // falls through
}

void s_aquamentus_body_pound_hook(GB *gb) {
  BASE(aquamentus_body_pound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+3);
  CYC(b_+3, b_+5); A = 0x81;
  CYC(b_+5, b_+8);
  TAIL(playSound_b00);
}

void s_aquamentus_body_stateB_hook(GB *gb) {
  BASE(aquamentus_body_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x96);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x04);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x18);
  CYC(b_+13, b_+16);
  TAIL_S(aquamentus_decideNextAttack);
}

void s_aquamentus_body_stateC_hook(GB *gb) {
  BASE(aquamentus_body_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_aquamentus_body_playFootstepSoundEvery24Frames_hook, SYM(aquamentus_body_playFootstepSoundEvery24Frames), b_+3);
  CALL_C(b_+3, s_aquamentus_body_6694_hook, SYM(aquamentus_body_6694), b_+6);
  CALL_C(b_+6, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto applySpeed; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+13, b_+16); SET_BC(SYM(aquamentus_projectileFireDelayCounters));
  CALL_C(b_+16, s_addAToBc, SYM(addAToBc), b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto fireProjectiles; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x08);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x14);
  RET(b_+35); return;
fireProjectiles:
  CALL_C(b_+36, s_aquamentus_body_chooseRandomLeftwardAngle_hook, SYM(aquamentus_body_chooseRandomLeftwardAngle), b_+39);
  CALL_C(b_+39, s_aquamentus_fireProjectiles_hook, SYM(aquamentus_fireProjectiles), b_+42);
applySpeed:
  CYC(b_+42, b_+45);
  TAIL(objectApplySpeed);
}

void s_aquamentus_body_stateD_hook(GB *gb) {
  BASE(aquamentus_body_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_aquamentus_body_playFootstepSoundEvery18Frames_hook, SYM(aquamentus_body_playFootstepSoundEvery18Frames), b_+3);
  CALL_C(b_+3, s_aquamentus_body_6694_hook, SYM(aquamentus_body_6694), b_+6);
  CALL_C(b_+6, s_aquamentus_body_checkReachedTargetPosition_hook, SYM(aquamentus_body_checkReachedTargetPosition), b_+9);
  if (F & FC) { CYCT(b_+9, b_+11); goto gotoStateB; }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+14);
  if (F & FZ) CALL_C_CC(b_+14, s_aquamentus_fireProjectiles_hook, SYM(aquamentus_fireProjectiles), b_+17);
  else CYC(b_+14, b_+17);
  CYC(b_+17, b_+20);
  TAIL(ecom_moveTowardPosition_b0d);
gotoStateB:
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x1e);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x0b);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x04);
  RET(b_+32); return;
}

void s_aquamentus_body_stateE_hook(GB *gb) {
  BASE(aquamentus_body_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x1c);
  if (F & FC) { CYCT(b_+9, b_+11); goto onLeftSide; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wFrameCounter);
  CYC(b_+14, b_+16); alu_and(gb, 0x1f);
  CYC(b_+16, b_+18); A = 0x6b;
  if (F & FZ) CALL_C_CC(b_+18, s_playSound, SYM(playSound_b00), b_+21);
  else CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, wFrameCounter);
  CYC(b_+24, b_+26); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto applySpeed; }
  CYC(b_+26, b_+28);
  CALL_C(b_+28, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto applySpeed; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x0f);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+40); SET_BC(0x1010);
  CALL_C(b_+40, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+43);
applySpeed:
  CYC(b_+43, b_+46);
  TAIL(objectApplySpeed);
onLeftSide:
  CALL_C(b_+46, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+49);
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+50, b_+52); goto gotoStateF; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_cp(gb, 0x94);
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+58); A = 0x46;
  CYC(b_+58, b_+61);
  TAIL_S(aquamentus_body_pound);
gotoStateF:
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0xf0);
  CYC(b_+63, b_+64); L = alu_inc8(gb, L);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x3c);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0xf8);
  CYC(b_+73, b_+75); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x01);
  CYC(b_+77, b_+79); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x08);
  CYC(b_+81, b_+83); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x1e);
  RET(b_+85); return;
}

void s_aquamentus_body_stateF_hook(GB *gb) {
  BASE(aquamentus_body_stateF);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_aquamentus_playHoverSoundEvery32Frames_hook, SYM(aquamentus_playHoverSoundEvery32Frames), b_+3);
  CALL_C(b_+3, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto moveBack; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0xe8);
  if (F & FC) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); B = 0x80;
  CYC(b_+16, b_+19);
  TAIL_S(aquamentus_body_subZ);
moveBack:
  CALL_C(b_+19, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+22);
  if (F & FZ) { CYCT(b_+22, b_+24); goto lowerDown; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0xd2);
  CYC(b_+27, b_+29); B = 0xc0;
  if (!(F & FC)) CALL_C_CC(b_+29, s_aquamentus_body_subZ_hook, SYM(aquamentus_body_subZ), b_+32);
  else CYC(b_+29, b_+32);
  CALL_C(b_+32, s_aquamentus_body_checkReachedTargetPosition_hook, SYM(aquamentus_body_checkReachedTargetPosition), b_+35);
  if (F & FC) { RET_TAKEN(b_+35); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+39);
  TAIL(ecom_moveTowardPosition_b0d);
lowerDown:
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x09);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x1e);
  RET(b_+47); return;
}

// The extra collision box that follows the body.
void s_aquamentus_subid2_hook(GB *gb) {
  BASE(aquamentus_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state8; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x31;
  CALL_C(b_+7, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); B = mem_rd(gb, HL);
  CYC(b_+11, b_+12); A = D;
  CYC(b_+12, b_+13); D = H;
  CYC(b_+13, b_+14); H = A;
  CALL_C(b_+14, s_objectCopyPosition, SYM(objectCopyPosition), b_+17);
  CYC(b_+17, b_+18); D = H;
  CYC(b_+18, b_+19); A = B;
  CALL_C(b_+19, s_getHighestSetBit, SYM(getHighestSetBit), b_+22);
  if (!(F & FC)) { CYCT(b_+22, b_+24); TAIL_S(aquamentus_animate); }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); SET_HL(SYM(aquamentus_animations));
  CYC(b_+27, b_+28); aquamentus_add_a_to_hl(gb, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+32, b_+34); TAIL_S(aquamentus_animate); }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+39);
  TAIL(enemySetAnimation);
state8:
  CYC(b_+39, b_+40); H = D;
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+47, b_+49); L = 0x97;
  CYC(b_+49, b_+50); H = mem_rd(gb, HL);
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+52, b_+54); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+59);
  TAIL(objectSetVisible81);
}

// The horn.
void s_aquamentus_subid3_hook(GB *gb) {
  BASE(aquamentus_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+5); TAIL_S(aquamentus_subid3_state8); }
  CYC(b_+3, b_+5);
  s_aquamentus_subid3_state9_hook(gb); return; // falls through
}

void s_aquamentus_subid3_state9_hook(GB *gb) {
  BASE(aquamentus_subid3_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x1a;
  CALL_C(b_+2, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); E = L;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_and(gb, 0x80);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_and(gb, 0x7f);
  CYC(b_+13, b_+14); alu_or(gb, B);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, s_aquamentus_horn_updateAnimation_hook, SYM(aquamentus_horn_updateAnimation), b_+18);
  CYC(b_+18, b_+20); A = 0x0b;
  CALL_C(b_+20, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+23);
  CYC(b_+23, b_+24); B = mem_rd(gb, HL);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+26, b_+27); C = mem_rd(gb, HL);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+29, b_+30); E = L;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_sub(gb, 0x07);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+36, b_+37); H = mem_rd(gb, HL);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(b_+42, b_+44); goto L_65b3; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); A = 0x05;
L_65b3:
  CYC(b_+46, b_+49); SET_HL(SYM(aquamentus_hornXYOffsets));
  CYC(b_+49, b_+50); aquamentus_add_double_index(gb, b_+50);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); alu_add(gb, B);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+59); alu_add(gb, C);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  s_aquamentus_animate_hook(gb); return; // falls through
}

void s_aquamentus_animate_hook(GB *gb) {
  BASE(aquamentus_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(enemyAnimate);
}

void s_aquamentus_subid3_state8_hook(GB *gb) {
  BASE(aquamentus_subid3_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x06);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x03);
  CYC(b_+10, b_+12); L = 0x97;
  CYC(b_+12, b_+13); H = mem_rd(gb, HL);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+22);
  TAIL(objectSetVisible81);
}

void s_aquamentus_initializeChildObject_hook(GB *gb) {
  BASE(aquamentus_initializeChildObject);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+2, b_+4); A = 0x80;
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, C);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+11);
  TAIL(objectCopyPosition);
}

// Charge, fire or hover, by aquamentus_chargeProbabilities.
void s_aquamentus_decideNextAttack_hook(GB *gb) {
  BASE(aquamentus_decideNextAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+6, b_+7); alu_cp(gb, B);
  CYC(b_+7, b_+9); A = 0x0c;
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto setState; }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x07);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+19); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+19, b_+20); alu_rlca(gb);
  CYC(b_+20, b_+21); alu_rlca(gb);
  CYC(b_+21, b_+23); alu_and(gb, 0x03);
  CYC(b_+23, b_+26); SET_HL(SYM(aquamentus_chargeProbabilities));
  CYC(b_+26, b_+27); aquamentus_add_a_to_hl(gb, b_+27);
  CYC(b_+27, b_+28); A = C;
  CALL_C(b_+28, s_checkFlag, SYM(checkFlag), b_+31);
  CYC(b_+31, b_+33); A = 0x0c;
  if (F & FZ) { CYCT(b_+33, b_+35); goto setState; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = 0x0e;
setState:
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+42, b_+44); goto initializeMovement; }
  CYC(b_+42, b_+44);
  CALL_C(b_+44, s_aquamentus_body_calculateAngleForCharge_hook, SYM(aquamentus_body_calculateAngleForCharge), b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+49, b_+51); A = 0x1e;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+54); A = 0x20;
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_COUNTER1;
setVar31AndSpeed:
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+59, b_+60); mem_wr(gb, HL, A);
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+62, b_+63); mem_wr(gb, HL, E);
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CALL_C(b_+66, s_getHighestSetBit, SYM(getHighestSetBit), b_+69);
  if (!(F & FC)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73); SET_HL(SYM(aquamentus_collisionBoxSizes));
  CYC(b_+73, b_+74); aquamentus_add_double_index(gb, b_+74);
  CYC(b_+74, b_+76); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  CYC(b_+78, b_+79); E = alu_inc8(gb, E);
  CYC(b_+79, b_+80); A = mem_rd(gb, HL);
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  RET(b_+81); return;
initializeMovement:
  CALL_C(b_+82, s_aquamentus_body_chooseRandomLeftwardAngle_hook, SYM(aquamentus_body_chooseRandomLeftwardAngle), b_+85);
  CYC(b_+85, b_+87); A = 0x04;
  CYC(b_+87, b_+89); E = 0x0a;
  CYC(b_+89, b_+91);
  goto setVar31AndSpeed;
}

void s_aquamentus_body_chooseRandomLeftwardAngle_hook(GB *gb) {
  BASE(aquamentus_body_chooseRandomLeftwardAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+7); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_664b; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x03;
L_664b:
  CYC(b_+11, b_+13); alu_add(gb, 0x15);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return;
}

void s_aquamentus_body_calculateAngleForCharge_hook(GB *gb) {
  BASE(aquamentus_body_calculateAngleForCharge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x02;
  CYC(b_+2, b_+4); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+4, b_+6); alu_cp(gb, 0x48);
  if (F & FC) { CYCT(b_+6, b_+8); goto setAngle; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); B = alu_dec8(gb, B);
  CYC(b_+9, b_+11); alu_cp(gb, 0x68);
  if (F & FC) { CYCT(b_+11, b_+13); goto setAngle; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); B = alu_dec8(gb, B);
setAngle:
  CYC(b_+14, b_+16); A = 0x17;
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void s_aquamentus_body_checkReachedTargetPosition_hook(GB *gb) {
  BASE(aquamentus_body_checkReachedTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR32;
  CALL_C(b_+3, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x02);
  CYC(b_+9, b_+11); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8F);
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+17); alu_add(gb, 0x02);
  CYC(b_+17, b_+19); alu_cp(gb, 0x05);
  RET(b_+19); return;
}

void s_aquamentus_body_subZ_hook(GB *gb) {
  BASE(aquamentus_body_subZ);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_Z;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_sub(gb, B);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_sbc(gb, 0x00);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

void s_aquamentus_fireProjectiles_hook(GB *gb) {
  BASE(aquamentus_fireProjectiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+2, b_+4); A = 0x10;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x7f;
  CALL_C(b_+7, s_playSound, SYM(playSound_b00), b_+10);
  CYC(b_+10, b_+12); B = 0x40;
  CYC(b_+12, b_+15);
  TAIL(ecom_spawnProjectile_b0d);
}

void s_aquamentus_body_6694_hook(GB *gb) {
  BASE(aquamentus_body_6694);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0x0c);
  CYC(b_+14, b_+16); A = 0x04;
  if (F & FZ) { CYCT(b_+16, b_+18); goto L_66a7; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_add(gb, A);
L_66a7:
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

void s_aquamentus_playHoverSoundEvery32Frames_hook(GB *gb) {
  BASE(aquamentus_playHoverSoundEvery32Frames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x7c;
  CYC(b_+8, b_+10);
  TAIL_S(aquamentus_playSound);
}

// aquamentus_body_playFootstepSoundEvery24Frames from +2: counts var36 down; at zero it reloads
// it with a and plays the footstep.
static void aquamentus_footstep_countdown(GB *gb) {
  BASE(aquamentus_body_playFootstepSoundEvery24Frames);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+10); A = 0x82;
  s_aquamentus_playSound_hook(gb); return; // falls through
}

void s_aquamentus_body_playFootstepSoundEvery18Frames_hook(GB *gb) {
  BASE(aquamentus_body_playFootstepSoundEvery18Frames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x12;
  CYC(b_+2, b_+4);
  aquamentus_footstep_countdown(gb); return;
}

void s_aquamentus_body_playFootstepSoundEvery24Frames_hook(GB *gb) {
  BASE(aquamentus_body_playFootstepSoundEvery24Frames);
  CYC(b_+0, b_+2); A = 0x18;
  aquamentus_footstep_countdown(gb); return;
}

void s_aquamentus_playSound_hook(GB *gb) {
  BASE(aquamentus_playSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(playSound_b00);
}

void s_aquamentus_horn_updateAnimation_hook(GB *gb) {
  BASE(aquamentus_horn_updateAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x34;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+13); SET_HL(b_+24);
  CYC(b_+13, b_+14); aquamentus_add_a_to_hl(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+18, b_+19); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+24);
  TAIL(enemySetAnimation);
}
