#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/facade.s.
// ENEMY_FACADE
// Variables:
// var03: The attack that has been randomly chosen.
// var30: The number of Beetles that have been spawned by the Beetle attack.

static uint16_t facade_jump_table(GB *gb) {
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

static void facade_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_FACADE
// Variables:
// var03: The attack that has been randomly chosen.
// var30: The number of Beetles that have been spawned by the Beetle attack.
void s_enemyCode71_hook(GB *gb) {
  BASE(enemyCode71);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+10, s_facade_killSpawnedBeetles_hook, SYM(facade_killSpawnedBeetles), b_+13);
  else CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); TAIL(enemyDie); }
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+23);
  TAIL(enemyBoss_dead_b0f);
normalStatus:
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); push_effect(gb, b_+27);
  do { uint16_t jt_ = (facade_jump_table(gb));
    if (jt_ == SYM(facade_state_uninitialized) && hook_is(gb, SYM(facade_state_uninitialized), s_facade_state_uninitialized_hook)) { s_facade_state_uninitialized_hook(gb); return; }
    if (jt_ == SYM(facade_state_stub) && hook_is(gb, SYM(facade_state_stub), s_facade_state_stub_hook)) { s_facade_state_stub_hook(gb); return; }
    if (jt_ == SYM(facade_state_waiting) && hook_is(gb, SYM(facade_state_waiting), s_facade_state_waiting_hook)) { s_facade_state_waiting_hook(gb); return; }
    if (jt_ == SYM(facade_state_chooseAttackAndBecomeVisible) && hook_is(gb, SYM(facade_state_chooseAttackAndBecomeVisible), s_facade_state_chooseAttackAndBecomeVisible_hook)) { s_facade_state_chooseAttackAndBecomeVisible_hook(gb); return; }
    if (jt_ == SYM(facade_state_waitAndRumble) && hook_is(gb, SYM(facade_state_waitAndRumble), s_facade_state_waitAndRumble_hook)) { s_facade_state_waitAndRumble_hook(gb); return; }
    if (jt_ == SYM(facade_state_attack) && hook_is(gb, SYM(facade_state_attack), s_facade_state_attack_hook)) { s_facade_state_attack_hook(gb); return; }
    if (jt_ == SYM(facade_state_resetAndInvisible) && hook_is(gb, SYM(facade_state_resetAndInvisible), s_facade_state_resetAndInvisible_hook)) { s_facade_state_resetAndInvisible_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// Do different initialization depending on [subid]:
// zero:     Just wait.
// non-zero: Start the fight.
void s_facade_state_uninitialized_hook(GB *gb) {
  BASE(facade_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x58);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x78);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  CYC(b_+15, b_+17); A = 0xff;
  CYC(b_+17, b_+19); B = 0x00;
  if (F & FZ) { CYCT(b_+19, b_+22); TAIL(enemyBoss_initializeRoom_b0f); }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x3c);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+29); return;
}

void s_facade_state_stub_hook(GB *gb) {
  BASE(facade_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

// Waiting for Link to enter the fight. Only entered when subid is 0.
void s_facade_state_waiting_hook(GB *gb) {
  BASE(facade_state_waiting);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+2, b_+4); alu_cp(gb, 0x58);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); A = 0x2d;
  CYC(b_+13, b_+16); mem_wr(gb, wActiveMusic, A);
  CYC(b_+16, b_+19);
  TAIL(playSound_b00);
}

void s_facade_state_chooseAttackAndBecomeVisible_hook(GB *gb) {
  BASE(facade_state_chooseAttackAndBecomeVisible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x78;
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+13, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+16);
  CALL_C(b_+16, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+19);
  CYC(b_+19, b_+21); alu_and(gb, 0x03);
  CYC(b_+21, b_+24); SET_HL(SYM(facade_attack_table));
  CYC(b_+24, b_+25); facade_add_a_to_hl(gb, b_+25);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); A = 0xb8;
  CALL_C(b_+31, s_playSound, SYM(playSound_b00), b_+34);
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CALL_C(b_+35, s_enemySetAnimation, SYM(enemySetAnimation), b_+38);
  CYC(b_+38, b_+41);
  TAIL(objectSetVisible83);
}

void s_facade_state_waitAndRumble_hook(GB *gb) {
  BASE(facade_state_waitAndRumble);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto nextState; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x1f);
  CYC(b_+8, b_+10); A = 0xb8;
  if (F & FZ) CALL_C_CC(b_+10, s_playSound, SYM(playSound_b00), b_+13);
  else CYC(b_+10, b_+13);
  CYC(b_+13, b_+15);
  goto onlyAnimate;
nextState:
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x00);
onlyAnimate:
  CYC(b_+20, b_+23);
  TAIL(enemyAnimate);
}

// Attack, based on the previously chosen value in var03.
// These attacks make use of substate.
void s_facade_state_attack_hook(GB *gb) {
  BASE(facade_state_attack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (facade_jump_table(gb));
    if (jt_ == b_+13) goto attack_beetle;
    if (jt_ == b_+58) goto attack_holeMaker;
    if (jt_ == b_+109) goto attack_volcanoRock;
    HANDOFF(HL);
  } while (0);
attack_beetle:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (facade_jump_table(gb));
    if (jt_ == b_+23) goto attack_beetle_attack_beetle_setup;
    if (jt_ == b_+30) goto attack_beetle_attack_beetle_wait;
    if (jt_ == b_+39) goto attack_beetle_attack_beetle_spawn;
    HANDOFF(HL);
  } while (0);
attack_beetle_attack_beetle_setup:
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+25); L = E;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x14);
  RET(b_+29); return;
attack_beetle_attack_beetle_wait:
  CALL_C(b_+30, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x46);
  CYC(b_+36, b_+37); L = E;
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+38); return;
attack_beetle_attack_beetle_spawn:
  CALL_C(b_+39, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+42);
  if (F & FZ) { CYCT(b_+42, b_+45); TAIL_S(facade_incrementStateAndFadeOut); }
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+58);
  TAIL_S(facade_spawnBeetle);
attack_holeMaker:
  CYC(b_+58, b_+60); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); push_effect(gb, b_+62);
  do { uint16_t jt_ = (facade_jump_table(gb));
    if (jt_ == b_+68) goto attack_holeMaker_attack_holeMaker_setup;
    if (jt_ == b_+75) goto attack_holeMaker_attack_holeMaker_animateAndInvisible;
    if (jt_ == b_+96) goto attack_holeMaker_attack_holeMaker_spawn;
    HANDOFF(HL);
  } while (0);
attack_holeMaker_attack_holeMaker_setup:
  CYC(b_+68, b_+70); A = 0x01;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+72); A = alu_inc8(gb, A);
  CYC(b_+72, b_+75);
  TAIL(enemySetAnimation);
attack_holeMaker_attack_holeMaker_animateAndInvisible:
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+78, b_+80); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+80, b_+83); TAIL(enemyAnimate); }
  CYC(b_+80, b_+83);
  CYC(b_+83, b_+84); L = E;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+85, b_+87); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0xb4);
  CYC(b_+89, b_+91); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+91, b_+93); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+93, b_+96);
  TAIL(objectSetInvisible);
attack_holeMaker_attack_holeMaker_spawn:
  CALL_C(b_+96, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+99);
  if (F & FZ) { CYCT(b_+99, b_+102); TAIL_S(facade_incrementStateAndFadeOut); }
  CYC(b_+99, b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+105); return; }
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+109);
  TAIL_S(facade_spawnHoleMaker);
attack_volcanoRock:
  CYC(b_+109, b_+111); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+111, b_+112); A = mem_rd(gb, DE);
  CYC(b_+112, b_+113); push_effect(gb, b_+113);
  do { uint16_t jt_ = (facade_jump_table(gb));
    if (jt_ == b_+117) goto attack_volcanoRock_attack_volcanoRock_setup;
    if (jt_ == b_+128) goto attack_volcanoRock_attack_volcanoRock_spawn;
    HANDOFF(HL);
  } while (0);
attack_volcanoRock_attack_volcanoRock_setup:
  CYC(b_+117, b_+118); H = D;
  CYC(b_+118, b_+119); L = E;
  CYC(b_+119, b_+120); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+120, b_+121); L = alu_inc8(gb, L);
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0xf0);
  CYC(b_+123, b_+125); A = 0x01;
  CYC(b_+125, b_+128);
  TAIL(enemySetAnimation);
attack_volcanoRock_attack_volcanoRock_spawn:
  CALL_C(b_+128, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+131);
  if (F & FZ) { CYCT(b_+131, b_+134); TAIL_S(facade_incrementStateAndFadeOut); }
  CYC(b_+131, b_+134);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL);
  CYC(b_+135, b_+137); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+137); return; }
  CYC(b_+137, b_+138);
  CYC(b_+138, b_+140); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+142); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+142); return; }
  CYC(b_+142, b_+143);
  CYC(b_+143, b_+145); A = 0x51;
  CALL_C(b_+145, s_playSound, SYM(playSound_b00), b_+148);
  CYC(b_+148, b_+151);
  TAIL_S(facade_spawnVolcanoRock);
}

// After the attack, Facade closes its eyes and goes invisible, before choosing another attack.
void s_facade_state_resetAndInvisible_hook(GB *gb) {
  BASE(facade_state_resetAndInvisible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(enemyAnimate); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x09);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x78);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+19, b_+22);
  TAIL(objectSetInvisible);
}

// Spawn a Beetle with subid $01 (ie, Beetle falls in from sky).
void s_facade_spawnBeetle_hook(GB *gb) {
  BASE(facade_spawnBeetle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x51;
  CALL_C(b_+2, s_ecom_spawnEnemyWithSubid01, SYM(ecom_spawnEnemyWithSubid01_b0d), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+8, b_+10); A = 0x80;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, D);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, s_getRandomNumber, SYM(getRandomNumber), b_+20);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+23); alu_and(gb, 0x70);
  CYC(b_+23, b_+25); alu_add(gb, 0x20);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);
  CYC(b_+29, b_+30); A = C;
  CYC(b_+30, b_+32); alu_and(gb, 0x07);
  CYC(b_+32, b_+34); A = alu_swap(gb, A);
  CYC(b_+34, b_+36); alu_add(gb, 0x40);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  RET(b_+37); return;
}

// Spawn the FACADE_HOLE_MAKER on a random tile on the 3x3 grid of tiles that
// has Link at its centre.
void s_facade_spawnHoleMaker_hook(GB *gb) {
  BASE(facade_spawnHoleMaker);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x2e;
  CALL_C(b_+2, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+10); SET_BC(0x1f1f);
  CALL_C(b_+10, s_ecom_randomBitwiseAndBCE, SYM(ecom_randomBitwiseAndBCE_b0d), b_+13);
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+16); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+19); alu_sub(gb, 0x10);
  CYC(b_+19, b_+21); alu_and(gb, 0xf0);
  CYC(b_+21, b_+23); alu_add(gb, 0x08);
  CYC(b_+23, b_+25); L = PART_BASE + OBJ_YH;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+28); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+31); alu_sub(gb, 0x10);
  CYC(b_+31, b_+33); alu_and(gb, 0xf0);
  CYC(b_+33, b_+35); alu_add(gb, 0x08);
  CYC(b_+35, b_+37); L = PART_BASE + OBJ_XH;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  RET(b_+38); return;
}

void s_facade_spawnVolcanoRock_hook(GB *gb) {
  BASE(facade_spawnVolcanoRock);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x11;
  CALL_C(b_+2, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = PART_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+9); return;
}

// This is used by the attack states to move to the next state and set the animation that closes its eyes.
void s_facade_incrementStateAndFadeOut_hook(GB *gb) {
  BASE(facade_incrementStateAndFadeOut);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+8);
  TAIL(enemySetAnimation);
}

// Kill all the Beetles that were spawned.
void s_facade_killSpawnedBeetles_hook(GB *gb) {
  BASE(facade_killSpawnedBeetles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1ReservedInteraction0_var3f + 0x01);
loop:
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x51);
  if (F & FZ) CALL_C_CC(b_+8, s_ecom_killObjectH, SYM(ecom_killObjectH_b0d), b_+11);
  else CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); H = alu_inc8(gb, H);
  CYC(b_+12, b_+13); A = H;
  CYC(b_+13, b_+15); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+15, b_+17); goto loop; }
  CYC(b_+15, b_+17);
  RET(b_+17); return;
}
