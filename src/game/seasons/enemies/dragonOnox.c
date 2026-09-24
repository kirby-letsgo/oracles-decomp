#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/dragonOnox.s.
// ENEMY_DRAGON_ONOX
// Variables:
// var2a:
// var2f:
// var30:
// var31:
// var32:
// var33:
// var34:
// var35:
// var36:
// var37:
// var38:
// $cfc8 - near end
// $cfc9
// $cfca
// $cfcb
// $cfcc
// $cfcd
// $cfd7 - Pointer to main body (subid $01)
// $cfd8 - Pointer to left shoulder (subid $02)
// $cfd9 - Pointer to right shoulder (subid $03)
// $cfda - Pointer to left claw (subid $04)
// $cfdb - Pointer to right claw (subid $05)
// $cfdc - Pointer to left claw sphere (subid $06)
// $cfdd - Pointer to right claw sphere (subid $07)
// $cfde - Pointer to left shoulder sphere (subid $08)
// $cfdf - Pointer to right shoulder sphere (subid $09)

static uint16_t dragonOnox_jump_table(GB *gb) {
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

static void dragonOnox_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void dragonOnox_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_DRAGON_ONOX
// Variables:
// var2a:
// var2f:
// var30:
// var31:
// var32:
// var33:
// var34:
// var35:
// var36:
// var37:
// var38:
// $cfc8 - near end
// $cfc9
// $cfca
// $cfcb
// $cfcc
// $cfcd
// $cfd7 - Pointer to main body (subid $01)
// $cfd8 - Pointer to left shoulder (subid $02)
// $cfd9 - Pointer to right shoulder (subid $03)
// $cfda - Pointer to left claw (subid $04)
// $cfdb - Pointer to right claw (subid $05)
// $cfdc - Pointer to left claw sphere (subid $06)
// $cfdd - Pointer to right claw sphere (subid $07)
// $cfde - Pointer to left shoulder sphere (subid $08)
// $cfdf - Pointer to right shoulder sphere (subid $09)
void s_enemyCode05_hook(GB *gb) {
  BASE(enemyCode05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto justHit; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0e);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+20, b_+21); A = alu_inc8(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+25); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+25, b_+27);
  goto normalStatus;
justHit:
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto normalStatus; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); A = A & 0x7f;
  CYC(b_+38, b_+40); alu_sub(gb, 0x04);
  CYC(b_+40, b_+42); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto normalStatus; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x3c);
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x01);
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x06);
  CYC(b_+57, b_+58); L = alu_inc8(gb, L);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x04);
  CYC(b_+60, b_+62); A = 0x06;
  CALL_C(b_+62, s_dragonOnoxLoadaIntoVar30Andcfc9_hook, SYM(dragonOnoxLoadaIntoVar30Andcfc9), b_+65);
normalStatus:
  CYC(b_+65, b_+67); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); B = A;
  CYC(b_+69, b_+71); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+71, b_+72); A = B;
  CYC(b_+72, b_+73); push_effect(gb, b_+73);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == SYM(dragonOnox_bodyPartSpawner) && hook_is(gb, SYM(dragonOnox_bodyPartSpawner), s_dragonOnox_bodyPartSpawner_hook)) { s_dragonOnox_bodyPartSpawner_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody) && hook_is(gb, SYM(dragonOnox_mainBody), s_dragonOnox_mainBody_hook)) { s_dragonOnox_mainBody_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_leftShoulder) && hook_is(gb, SYM(dragonOnox_leftShoulder), s_dragonOnox_leftShoulder_hook)) { s_dragonOnox_leftShoulder_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_rightShoulder) && hook_is(gb, SYM(dragonOnox_rightShoulder), s_dragonOnox_rightShoulder_hook)) { s_dragonOnox_rightShoulder_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_leftClaw) && hook_is(gb, SYM(dragonOnox_leftClaw), s_dragonOnox_leftClaw_hook)) { s_dragonOnox_leftClaw_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_rightClaw) && hook_is(gb, SYM(dragonOnox_rightClaw), s_dragonOnox_rightClaw_hook)) { s_dragonOnox_rightClaw_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_leftClawSphere) && hook_is(gb, SYM(dragonOnox_leftClawSphere), s_dragonOnox_leftClawSphere_hook)) { s_dragonOnox_leftClawSphere_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_rightClawSphere) && hook_is(gb, SYM(dragonOnox_rightClawSphere), s_dragonOnox_rightClawSphere_hook)) { s_dragonOnox_rightClawSphere_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_leftShoulderSphere) && hook_is(gb, SYM(dragonOnox_leftShoulderSphere), s_dragonOnox_leftShoulderSphere_hook)) { s_dragonOnox_leftShoulderSphere_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_rightShoulderSphere) && hook_is(gb, SYM(dragonOnox_rightShoulderSphere), s_dragonOnox_rightShoulderSphere_hook)) { s_dragonOnox_rightShoulderSphere_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_dragonOnox_bodyPartSpawner_hook(GB *gb) {
  BASE(dragonOnox_bodyPartSpawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;
  CYC(b_+2, b_+4); B = 0x8a;
  CALL_C(b_+4, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wLinkForceState, A);
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+15, b_+18); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+18, b_+20); B = 0x09;
  CALL_C(b_+20, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); B = 0x05;
  CALL_C(b_+26, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+29);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+31, b_+32); E = L;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); A = H;
  CYC(b_+35, b_+38); SET_HL(wTmpcfc0 + 0x17);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+39, b_+41); C = 0x08;
L_5d21:
  CYC(b_+41, b_+42); push_effect(gb, HL);
  CALL_C(b_+42, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+45);
  CYC(b_+45, b_+47); A = 0x0a;
  CYC(b_+47, b_+48); alu_sub(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);
  CYC(b_+49, b_+50); A = H;
  CYC(b_+50, b_+51); SET_HL(pop_effect(gb));
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+52, b_+53); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto L_5d21; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+58);
  TAIL(enemyDelete);
}

void s_dragonOnox_mainBody_hook(GB *gb) {
  BASE(dragonOnox_mainBody);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x02);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0c);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto L_5d45; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wFrameCounter);
  CYC(b_+12, b_+14); alu_and(gb, 0x3f);
  CYC(b_+14, b_+16); A = 0x7c;
  if (F & FZ) CALL_C_CC(b_+16, s_playSound, SYM(playSound_b00), b_+19);
  else CYC(b_+16, b_+19);
L_5d45:
  CALL_C(b_+19, s_dragonOnox_checkTransitionState_hook, SYM(dragonOnox_checkTransitionState), b_+22);
  if (F & FZ) CALL_C_CC(b_+22, s_dragonOnox_mainBodyStateHandler_hook, SYM(dragonOnox_mainBodyStateHandler), b_+25);
  else CYC(b_+22, b_+25);
  CALL_C(b_+25, s_seasonsFunc_0f_65c7_hook, SYM(seasonsFunc_0f_65c7), b_+28);
  CYC(b_+28, b_+31);
  TAIL_S(seasonsFunc_0f_65fc);
}

void s_dragonOnox_mainBodyStateHandler_hook(GB *gb) {
  BASE(dragonOnox_mainBodyStateHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == SYM(dragonOnox_mainBody_state0) && hook_is(gb, SYM(dragonOnox_mainBody_state0), s_dragonOnox_mainBody_state0_hook)) { s_dragonOnox_mainBody_state0_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state1) && hook_is(gb, SYM(dragonOnox_mainBody_state1), s_dragonOnox_mainBody_state1_hook)) { s_dragonOnox_mainBody_state1_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state2) && hook_is(gb, SYM(dragonOnox_mainBody_state2), s_dragonOnox_mainBody_state2_hook)) { s_dragonOnox_mainBody_state2_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state3) && hook_is(gb, SYM(dragonOnox_mainBody_state3), s_dragonOnox_mainBody_state3_hook)) { s_dragonOnox_mainBody_state3_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state4) && hook_is(gb, SYM(dragonOnox_mainBody_state4), s_dragonOnox_mainBody_state4_hook)) { s_dragonOnox_mainBody_state4_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state5) && hook_is(gb, SYM(dragonOnox_mainBody_state5), s_dragonOnox_mainBody_state5_hook)) { s_dragonOnox_mainBody_state5_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state7) && hook_is(gb, SYM(dragonOnox_mainBody_state7), s_dragonOnox_mainBody_state7_hook)) { s_dragonOnox_mainBody_state7_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_state8) && hook_is(gb, SYM(dragonOnox_mainBody_state8), s_dragonOnox_mainBody_state8_hook)) { s_dragonOnox_mainBody_state8_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_stateA) && hook_is(gb, SYM(dragonOnox_mainBody_stateA), s_dragonOnox_mainBody_stateA_hook)) { s_dragonOnox_mainBody_stateA_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_stateB) && hook_is(gb, SYM(dragonOnox_mainBody_stateB), s_dragonOnox_mainBody_stateB_hook)) { s_dragonOnox_mainBody_stateB_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_stateD) && hook_is(gb, SYM(dragonOnox_mainBody_stateD), s_dragonOnox_mainBody_stateD_hook)) { s_dragonOnox_mainBody_stateD_hook(gb); return; }
    if (jt_ == SYM(dragonOnox_mainBody_stateE) && hook_is(gb, SYM(dragonOnox_mainBody_stateE), s_dragonOnox_mainBody_stateE_hook)) { s_dragonOnox_mainBody_stateE_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_dragonOnox_checkTransitionState_hook(GB *gb) {
  BASE(dragonOnox_checkTransitionState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0e);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+17); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_5d9c; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_sub(gb, 0x02);
  CYC(b_+24, b_+26); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto L_5d9c; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_bit(gb, 4, A);
  CYC(b_+33, b_+35); A = 0x08;
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_5da3; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); A = 0x09;
  CYC(b_+39, b_+41);
  goto L_5da3;
L_5d9c:
  CYC(b_+41, b_+43); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); alu_and(gb, 0x01);
  CYC(b_+46, b_+48); alu_add(gb, 0x00);
L_5da3:
  CYC(b_+48, b_+51);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
}

void s_dragonOnox_mainBody_state0_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x5a);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x14);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+17, b_+19); A = 0x05;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x14);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x0c);
  CALL_C(b_+28, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto L_5dcb; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x22);
L_5dcb:
  CALL_C(b_+37, s_objectSetVisible83, SYM(objectSetVisible83), b_+40);
  CYC(b_+40, b_+42); A = 0x04;
  CYC(b_+42, b_+45);
  TAIL(fadeinFromWhiteWithDelay);
}

void s_dragonOnox_mainBody_state1_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_5df8; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto L_5de6; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x3c);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); A = 0x7c;
  CYC(b_+16, b_+19);
  TAIL(playSound_b00);
L_5de6:
  CYC(b_+19, b_+20); L = E;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+21, b_+23); A = 0x08;
  CYC(b_+23, b_+26); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+26, b_+28); A = 0x06;
  CYC(b_+28, b_+31); mem_wr(gb, wTextboxPosition, A);
  CYC(b_+31, b_+34); SET_BC(0x501e);
  CYC(b_+34, b_+37);
  TAIL(showText);
L_5df8:
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+39); L = E;
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+42, b_+43); alu_xor(gb, A);
  CYC(b_+43, b_+46); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+46, b_+49); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+49, b_+52); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+52, b_+54); A = 0x32;
  CYC(b_+54, b_+57); mem_wr(gb, wActiveMusic, A);
  CYC(b_+57, b_+60);
  TAIL(playSound_b00);
}

void s_dragonOnox_mainBody_state2_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_5e1c; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
L_5e1c:
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+26) goto substate0;
    if (jt_ == b_+63) goto substate1;
    if (jt_ == b_+75) goto substate2;
    if (jt_ == b_+144) goto substate3;
    if (jt_ == b_+168) goto substate4;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+26, b_+27); H = D;
  CYC(b_+27, b_+28); L = E;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x1e);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x10);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x1e);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x00);
  CALL_C(b_+43, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+46);
  CYC(b_+46, b_+48); alu_and(gb, 0x01);
  CYC(b_+48, b_+50); B = 0x00;
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto L_5e46; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); A = alu_dec8(gb, A);
  CYC(b_+53, b_+55); B = 0x10;
L_5e46:
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+60); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+61); A = B;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  RET(b_+62); return;
substate1:
  CALL_C(b_+63, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+66);
  if (F & FZ) { CYCT(b_+66, b_+69); TAIL_S(seasonsFunc_0f_665c); }
  CYC(b_+66, b_+69);
  CYC(b_+69, b_+70); L = E;
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x02);
  CALL_C(b_+72, s_seasonsFunc_0f_6637_hook, SYM(seasonsFunc_0f_6637), b_+75);
substate2:
  CALL_C(b_+75, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+80); goto L_5e74; }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x1e);
  CYC(b_+82, b_+85); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+85, b_+87); alu_sub(gb, 0x10);
  CYC(b_+87, b_+89); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+89, b_+91); goto L_5e74; }
  CYC(b_+89, b_+91);
  CALL_C(b_+91, s_getRandomNumber, SYM(getRandomNumber), b_+94);
  CYC(b_+94, b_+96); alu_cp(gb, 0xa0);
  if (!(F & FC)) { CYCT(b_+96, b_+98); goto L_5e74; }
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+99); L = E;
  CYC(b_+99, b_+100); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+100); return;
L_5e74:
  CYC(b_+101, b_+103); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+103, b_+104); B = mem_rd(gb, HL);
  CYC(b_+104, b_+105); L = alu_inc8(gb, L);
  CYC(b_+105, b_+106); C = mem_rd(gb, HL);
  CYC(b_+106, b_+109); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+109, b_+110); H = A;
  CYC(b_+110, b_+113); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+113, b_+114); L = A;
  CYC(b_+114, b_+115); alu_sub(gb, C);
  CYC(b_+115, b_+117); alu_add(gb, 0x06);
  CYC(b_+117, b_+119); alu_cp(gb, 0x0d);
  if (!(F & FC)) { CYCT(b_+119, b_+121); goto L_5e90; }
  CYC(b_+119, b_+121);
  CYC(b_+121, b_+122); A = H;
  CYC(b_+122, b_+123); alu_sub(gb, B);
  CYC(b_+123, b_+125); alu_add(gb, 0x06);
  CYC(b_+125, b_+127); alu_cp(gb, 0x0d);
  if (F & FC) { CYCT(b_+127, b_+129); goto substate1; }
  CYC(b_+127, b_+129);
L_5e90:
  CYC(b_+129, b_+131); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+131, b_+132); A = mem_rd(gb, DE);
  CYC(b_+132, b_+133); alu_rrca(gb);
  if (F & FC) { CYCT(b_+133, b_+135); goto L_5e9c; }
  CYC(b_+133, b_+135);
  CALL_C(b_+135, s_seasonsFunc_0f_6529_hook, SYM(seasonsFunc_0f_6529), b_+138);
  CALL_C(b_+138, s_objectNudgeAngleTowards, SYM(objectNudgeAngleTowards), b_+141);
L_5e9c:
  CYC(b_+141, b_+144);
  TAIL_S(seasonsFunc_0f_650d);
substate3:
  CALL_C(b_+144, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+147);
  if (!(F & FZ)) { RET_TAKEN(b_+147); return; }
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+149); L = E;
  CYC(b_+149, b_+150); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+150, b_+152); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+152, b_+155); A = mem_rd(gb, w1Link_xh);
  CYC(b_+155, b_+156); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+156, b_+159); SET_HL(wTmpcfc0 + 0x1a);
  if (F & FC) { CYCT(b_+159, b_+161); goto L_5eb1; }
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+162); L = alu_inc8(gb, L);
L_5eb1:
  CYC(b_+162, b_+163); H = mem_rd(gb, HL);
  CYC(b_+163, b_+165); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x01);
  RET(b_+167); return;
substate4:
  CYC(b_+168, b_+169); H = D;
  CYC(b_+169, b_+171); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+171, b_+173); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+173); return; }
  CYC(b_+173, b_+174);
  CYC(b_+174, b_+176); mem_wr(gb, HL, 0x96);
  CYC(b_+176, b_+177); L = E;
  CYC(b_+177, b_+179); mem_wr(gb, HL, 0x02);
  RET(b_+179); return;
}

void s_dragonOnox_mainBody_state3_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto seasonsFunc_0f_5ecb; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
seasonsFunc_0f_5ecb:
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); C = mem_rd(gb, HL);
  CYC(b_+12, b_+13); B = A;
  CALL_C(b_+13, s_seasonsFunc_0f_66aa_hook, SYM(seasonsFunc_0f_66aa), b_+16);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20);
  TAIL_S(seasonsFunc_0f_6680);
}

void s_dragonOnox_mainBody_state4_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, s_dragonOnoxLowHealthThresholdIntoC_hook, SYM(dragonOnoxLowHealthThresholdIntoC), b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_cp(gb, C);
  CYC(b_+11, b_+12); A = B;
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto seasonsFunc_0f_5ee8; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+17);
  goto L_5eea;
seasonsFunc_0f_5ee8:
  CYC(b_+17, b_+19); alu_cp(gb, 0xa0);
L_5eea:
  CYC(b_+19, b_+21); A = 0x05;
  if (F & FC) { CYCT(b_+21, b_+23); goto L_5ef0; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x08;
L_5ef0:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); alu_xor(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

void s_dragonOnox_mainBody_state5_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_5f07; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
L_5f07:
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+24) goto substate0;
    if (jt_ == b_+38) goto substate1;
    if (jt_ == b_+68) goto substate2;
    if (jt_ == b_+88) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+26); L = E;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x2d);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x04);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x19);
  RET(b_+37); return;
substate1:
  CALL_C(b_+38, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+41);
  if (F & FZ) { CYCT(b_+41, b_+43); goto L_5f30; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); A = mem_rd(gb, w1Link_xh);
  CYC(b_+46, b_+48); alu_sub(gb, 0x50);
  CYC(b_+48, b_+49); C = A;
  CYC(b_+49, b_+51); B = 0x00;
  CYC(b_+51, b_+54);
  TAIL_S(seasonsFunc_0f_66aa);
L_5f30:
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x1e);
  CYC(b_+56, b_+57); L = E;
  CYC(b_+57, b_+58); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+63); alu_and(gb, 0x01);
  CYC(b_+63, b_+65); alu_add(gb, 0x02);
  CYC(b_+65, b_+68);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
substate2:
  CALL_C(b_+68, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+71);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+73); L = E;
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+74, b_+76); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x1e);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+83); alu_and(gb, 0x01);
  CYC(b_+83, b_+85); alu_add(gb, 0x04);
  CYC(b_+85, b_+88);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
substate3:
  CALL_C(b_+88, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+91);
  if (F & FZ) { CYCT(b_+91, b_+93); goto seasonsFunc_0f_5f6c; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+94); A = mem_rd(gb, HL);
  CYC(b_+94, b_+96); alu_cp(gb, 0x14);
  if (!(F & FZ)) { RET_TAKEN(b_+96); return; }
  CYC(b_+96, b_+97);
  CALL_C(b_+97, s_getFreePartSlot, SYM(getFreePartSlot), b_+100);
  if (!(F & FZ)) { RET_TAKEN(b_+100); return; }
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x33);
  CYC(b_+103, b_+106); SET_BC(0x1800);
  CALL_C(b_+106, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+109);
  CYC(b_+109, b_+111); A = 0x7f;
  CYC(b_+111, b_+114);
  TAIL(playSound_b00);
seasonsFunc_0f_5f6c:
  CYC(b_+114, b_+116); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+116, b_+117); A = mem_rd(gb, HL);
  CYC(b_+117, b_+119); alu_and(gb, 0x01);
  CYC(b_+119, b_+121); alu_add(gb, 0x00);
  CALL_C(b_+121, s_dragonOnoxLoadaIntoVar30Andcfc9_hook, SYM(dragonOnoxLoadaIntoVar30Andcfc9), b_+124);
  CALL_C(b_+124, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+127);
  if (F & FZ) { CYCT(b_+127, b_+130); TAIL_S(seasonsFunc_0f_665c); }
  CYC(b_+127, b_+130);
  CYC(b_+130, b_+131); L = alu_dec8(gb, L);
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x2d);
  CYC(b_+133, b_+134); L = alu_dec8(gb, L);
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x01);
  RET(b_+136); return;
}

void s_dragonOnox_mainBody_state7_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_dragonOnoxLowHealthThresholdIntoC_hook, SYM(dragonOnoxLowHealthThresholdIntoC), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_cp(gb, C);
  CYC(b_+7, b_+9); A = 0x08;
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto L_5f90; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x0b;
L_5f90:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

void s_dragonOnox_mainBody_state8_hook(GB *gb) {
  BASE(dragonOnox_mainBody_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+17) goto substate0;
    if (jt_ == b_+42) goto substate1;
    if (jt_ == b_+82) goto substate2;
    if (jt_ == b_+112) goto substate3;
    if (jt_ == b_+137) goto substate4;
    if (jt_ == b_+166) goto substate5;
    if (jt_ == b_+202) goto substate6;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+19); L = E;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x1e);
  CYC(b_+24, b_+27); SET_BC(0x20c0);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(b_+32, b_+34); goto L_5fbe; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); C = 0x40;
L_5fbe:
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+38, b_+39); mem_wr(gb, HL, B);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+41); mem_wr(gb, HL, C);
  RET(b_+41); return;
substate1:
  CYC(b_+42, b_+43); H = D;
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+45, b_+46); B = mem_rd(gb, HL);
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+48); C = mem_rd(gb, HL);
  CALL_C(b_+48, s_seasonsFunc_0f_66aa_hook, SYM(seasonsFunc_0f_66aa), b_+51);
  if (!(F & FZ)) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x1e);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+61, b_+63); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+63, b_+65); A = 0x09;
  CYC(b_+65, b_+68); SET_BC(0x0848);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto L_5fe5; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); A = 0x08;
  CYC(b_+72, b_+75); SET_BC(0x18b8);
L_5fe5:
  CYC(b_+75, b_+76); mem_wr(gb, HL, C);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+78, b_+79); mem_wr(gb, HL, B);
  CYC(b_+79, b_+82);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
substate2:
  CALL_C(b_+82, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+85);
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+87); L = E;
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+88, b_+90); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x5a);
  CYC(b_+92, b_+94); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+94, b_+96); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+96, b_+99); SET_HL(wTmpcfc0 + 0x1a);
  if (F & FZ) { CYCT(b_+99, b_+101); goto L_6000; }
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+102); L = alu_inc8(gb, L);
L_6000:
  CYC(b_+102, b_+103); H = mem_rd(gb, HL);
  CYC(b_+103, b_+105); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x02);
  CYC(b_+107, b_+109); A = 0x7f;
  CYC(b_+109, b_+112);
  TAIL(playSound_b00);
substate3:
  CYC(b_+112, b_+113); H = D;
  CYC(b_+113, b_+115); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+115, b_+118); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+118, b_+119); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+119, b_+121); alu_add(gb, 0x02);
  CYC(b_+121, b_+123); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+123, b_+126); TAIL_S(seasonsFunc_0f_650d); }
  CYC(b_+123, b_+126);
  CYC(b_+126, b_+127); L = E;
  CYC(b_+127, b_+128); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+128, b_+130); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+130, b_+132); mem_wr(gb, HL, 0x1e);
  CYC(b_+132, b_+134); A = 0x00;
  CYC(b_+134, b_+137);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
substate4:
  CALL_C(b_+137, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+140);
  CYC(b_+140, b_+141); A = mem_rd(gb, HL);
  CYC(b_+141, b_+143); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+143, b_+145); goto L_602f; }
  CYC(b_+143, b_+145);
  CYC(b_+145, b_+146); alu_xor(gb, A);
  CALL_C(b_+146, s_objectNudgeAngleTowards, SYM(objectNudgeAngleTowards), b_+149);
L_602f:
  CALL_C(b_+149, s_seasonsFunc_0f_650d_hook, SYM(seasonsFunc_0f_650d), b_+152);
  CYC(b_+152, b_+155); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+155, b_+157); alu_cp(gb, 0xd0);
  if (!(F & FZ)) { RET_TAKEN(b_+157); return; }
  CYC(b_+157, b_+158);
  CYC(b_+158, b_+159); H = D;
  CYC(b_+159, b_+161); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+161, b_+162); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+162, b_+163); L = alu_inc8(gb, L);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x00);
  RET(b_+165); return;
substate5:
  CALL_C(b_+166, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+169);
  CYC(b_+169, b_+172); SET_BC(0xb000);
  CYC(b_+172, b_+175); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+175, b_+176); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+176, b_+178); goto L_604f; }
  CYC(b_+176, b_+178);
  CYC(b_+178, b_+179); L = E;
  CYC(b_+179, b_+180); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+180); return;
L_604f:
  CYC(b_+181, b_+182); L = A;
  CYC(b_+182, b_+185); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+185, b_+186); H = A;
  CYC(b_+186, b_+188); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+188, b_+189); A = mem_rd(gb, DE);
  CYC(b_+189, b_+191); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+191, b_+193); goto L_6061; }
  CYC(b_+191, b_+193);
  CALL_C(b_+193, s_seasonsFunc_0f_6529_hook, SYM(seasonsFunc_0f_6529), b_+196);
  CALL_C(b_+196, s_objectNudgeAngleTowards, SYM(objectNudgeAngleTowards), b_+199);
L_6061:
  CYC(b_+199, b_+202);
  TAIL_S(seasonsFunc_0f_650d);
substate6:
  CYC(b_+202, b_+205); SET_HL(wTmpcfc0 + 0x0c);
  CYC(b_+205, b_+206); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+206); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+208); H = D;
  CYC(b_+208, b_+211);
  TAIL_S(seasonsFunc_0f_665c);
}

void s_dragonOnox_mainBody_stateA_hook(GB *gb) {
  BASE(dragonOnox_mainBody_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_dragonOnoxLowHealthThresholdIntoC_hook, SYM(dragonOnoxLowHealthThresholdIntoC), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_cp(gb, C);
  CYC(b_+7, b_+9); A = 0x02;
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto L_607a; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x0b;
L_607a:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

void s_dragonOnox_mainBody_stateB_hook(GB *gb) {
  BASE(dragonOnox_mainBody_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_608f; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+10); return;
L_608f:
  CYC(b_+11, b_+12); E = alu_inc8(gb, E);
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+22) goto substate0;
    if (jt_ == b_+30) goto substate1;
    if (jt_ == b_+54) goto substate2;
    if (jt_ == b_+88) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+24); L = E;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x1e);
  RET(b_+29); return;
substate1:
  CYC(b_+30, b_+33); SET_BC(0xf800);
  CALL_C(b_+33, s_seasonsFunc_0f_66aa_hook, SYM(seasonsFunc_0f_66aa), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x3c);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+49); alu_and(gb, 0x01);
  CYC(b_+49, b_+51); alu_add(gb, 0x02);
  CYC(b_+51, b_+54);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
substate2:
  CALL_C(b_+54, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+57);
  if (F & FZ) { CYCT(b_+57, b_+59); goto L_60cd; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+62); alu_cp(gb, 0x1e);
  if (!(F & FZ)) { RET_TAKEN(b_+62); return; }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL);
  CYC(b_+66, b_+68); alu_and(gb, 0x01);
  CYC(b_+68, b_+70); alu_add(gb, 0x04);
  CYC(b_+70, b_+73);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
L_60cd:
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x18);
  CYC(b_+77, b_+78); L = E;
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+79, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+82);
  CYC(b_+82, b_+84); alu_and(gb, 0x18);
  CYC(b_+84, b_+86); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  RET(b_+87); return;
substate3:
  CALL_C(b_+88, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+91);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x14);
  CYC(b_+94, b_+96); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+100); SET_HL(b_+146 /* @seasonsTable_0f_6116 */);
  CYC(b_+100, b_+101); dragonOnox_add_a_to_hl(gb, b_+101);
  CYC(b_+101, b_+102); C = mem_rd(gb, HL);
  CALL_C(b_+102, s_getFreePartSlot, SYM(getFreePartSlot), b_+105);
  if (!(F & FZ)) { RET_TAKEN(b_+105); return; }
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x4a);
  CYC(b_+108, b_+110); L = PART_BASE + OBJ_VAR31;
  CYC(b_+110, b_+111); mem_wr(gb, HL, C);
  CYC(b_+111, b_+114); SET_BC(0x1800);
  CALL_C(b_+114, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+117);
  CYC(b_+117, b_+119); A = 0xa4;
  CALL_C(b_+119, s_playSound, SYM(playSound_b00), b_+122);
  CYC(b_+122, b_+124); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+126); A = alu_inc8(gb, A);
  CYC(b_+126, b_+128); alu_and(gb, 0x1f);
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CALL_C(b_+129, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+132);
  if (!(F & FZ)) { RET_TAKEN(b_+132); return; }
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+138); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+138, b_+139); A = mem_rd(gb, HL);
  CYC(b_+139, b_+141); alu_and(gb, 0x01);
  CYC(b_+141, b_+143); alu_add(gb, 0x00);
  CYC(b_+143, b_+146);
  TAIL_S(dragonOnoxLoadaIntoVar30Andcfc9);
}

void s_dragonOnox_mainBody_stateD_hook(GB *gb) {
  BASE(dragonOnox_mainBody_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x02;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); E = alu_inc8(gb, E);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11); return;
}

void s_dragonOnox_mainBody_stateE_hook(GB *gb) {
  BASE(dragonOnox_mainBody_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+9) goto substate0;
    if (jt_ == b_+81) goto substate1;
    if (jt_ == b_+89) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x10);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0a);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+25, b_+27); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+27, b_+30); A = mem_rd(gb, wTmpcfc0 + 0x1a);
  CYC(b_+30, b_+31); H = A;
  CYC(b_+31, b_+33); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+33, b_+36); A = mem_rd(gb, wTmpcfc0 + 0x1b);
  CYC(b_+36, b_+37); H = A;
  CYC(b_+37, b_+39); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+39, b_+41); A = 0x04;
  CYC(b_+41, b_+44); mem_wr(gb, wTmpcfc0 + 0x08, A);
  CYC(b_+44, b_+46); A = 0xf0;
  CALL_C(b_+46, s_playSound, SYM(playSound_b00), b_+49);
  CYC(b_+49, b_+52); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+52, b_+53); alu_cpl(gb);
  CYC(b_+53, b_+54); A = alu_inc8(gb, A);
  CYC(b_+54, b_+57); mem_wr(gb, wScreenOffsetX, A);
  CYC(b_+57, b_+60); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+60, b_+61); alu_cpl(gb);
  CYC(b_+61, b_+62); A = alu_inc8(gb, A);
  CYC(b_+62, b_+65); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+65, b_+67); A = 0x08;
  CYC(b_+67, b_+70); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+70, b_+72); A = 0x04;
  CYC(b_+72, b_+75); mem_wr(gb, wTextboxPosition, A);
  CYC(b_+75, b_+78); SET_BC(0x501f);
  CYC(b_+78, b_+81);
  TAIL(showTextNonExitable);
substate1:
  CYC(b_+81, b_+83); A = 0x02;
  CYC(b_+83, b_+84); mem_wr(gb, DE, A);
  CYC(b_+84, b_+86); A = 0xbc;
  CYC(b_+86, b_+89);
  TAIL(playSound_b00);
substate2:
  CYC(b_+89, b_+92); A = mem_rd(gb, wFrameCounter);
  CYC(b_+92, b_+94); alu_and(gb, 0x0f);
  CYC(b_+94, b_+96); A = 0xb3;
  if (F & FZ) CALL_C_CC(b_+96, s_playSound, SYM(playSound_b00), b_+99);
  else CYC(b_+96, b_+99);
  CALL_C(b_+99, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_and(gb, 0x03);
  CYC(b_+105, b_+108); SET_HL(b_+119 /* @seasonsTable_0f_61b9 */);
  CYC(b_+108, b_+109); dragonOnox_add_a_to_hl(gb, b_+109);
  CYC(b_+109, b_+112); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+112, b_+113); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+113, b_+116); mem_wr(gb, wTmpcfc0 + 0x0d, A);
  CYC(b_+116, b_+119);
  TAIL_S(seasonsFunc_0f_650d);
}

void s_dragonOnox_leftShoulder_hook(GB *gb) {
  BASE(dragonOnox_leftShoulder);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+6) goto animate;
    if (jt_ == b_+17) goto offsetBasedOncfca;
    HANDOFF(HL);
  } while (0);
animate:
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); A = 0x09;
  CALL_C(b_+11, s_enemySetAnimation, SYM(enemySetAnimation), b_+14);
  CALL_C(b_+14, s_objectSetVisible83, SYM(objectSetVisible83), b_+17);
offsetBasedOncfca:
  CYC(b_+17, b_+20); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+20, b_+22); alu_cp(gb, 0x08);
  CYC(b_+22, b_+25); SET_BC(0x603a);
  if (F & FC) { CYCT(b_+25, b_+27); goto L_61e0; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+30); SET_BC(0x5238);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_61e0; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+35); SET_BC(0x6640);
L_61e0:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+40); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+40, b_+41); alu_add(gb, B);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+44, b_+47); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  RET(b_+49); return;
}

void s_dragonOnox_rightShoulder_hook(GB *gb) {
  BASE(dragonOnox_rightShoulder);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+6) goto animate;
    if (jt_ == b_+17) goto offsetBasedOncfca;
    HANDOFF(HL);
  } while (0);
animate:
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); A = 0x0a;
  CALL_C(b_+11, s_enemySetAnimation, SYM(enemySetAnimation), b_+14);
  CALL_C(b_+14, s_objectSetVisible83, SYM(objectSetVisible83), b_+17);
offsetBasedOncfca:
  CYC(b_+17, b_+20); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+20, b_+22); alu_cp(gb, 0x08);
  CYC(b_+22, b_+25); SET_BC(0x6066);
  if (F & FC) { CYCT(b_+25, b_+27); goto L_6212; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+30); SET_BC(0x6660);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_6212; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+35); SET_BC(0x5268);
L_6212:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+40); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+40, b_+41); alu_add(gb, B);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+44, b_+47); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  RET(b_+49); return;
}

// dragonOnox_leftClaw@state3@seasonsFunc_0f_63e1
static void dragonOnox_state3_seasonsFunc_0f_63e1(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+448, b_+449); H = D;
  CYC(b_+449, b_+451); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+451, b_+453); mem_wr(gb, HL, 0x01);
  CYC(b_+453, b_+455); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+455, b_+457); mem_wr(gb, HL, 0x05);
  CYC(b_+457, b_+458); L = alu_inc8(gb, L);
  CYC(b_+458, b_+460); mem_wr(gb, HL, 0x09);
  CYC(b_+460, b_+462); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+462, b_+464); mem_wr(gb, HL, 0xfc);
  CYC(b_+464, b_+466); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+466, b_+467); A = mem_rd(gb, DE);
  CYC(b_+467, b_+468); A = alu_dec8(gb, A);
  CYC(b_+468, b_+471);
  TAIL(enemySetAnimation);
}

// dragonOnox_leftClaw@seasonsFunc_0f_62a5@seasonsFunc_0f_6386
static void dragonOnox_seasonsFunc_0f_62a5_seasonsFunc_0f_6386(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+357, b_+359); A = 0x0b;
  CALL_C(b_+359, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+362);
  CYC(b_+362, b_+363); E = L;
  CYC(b_+363, b_+364); A = mem_rd(gb, DE);
  CYC(b_+364, b_+365); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+365, b_+367); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+367, b_+368); mem_wr(gb, DE, A);
  CYC(b_+368, b_+370); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+370, b_+371); E = L;
  CYC(b_+371, b_+372); A = mem_rd(gb, DE);
  CYC(b_+372, b_+373); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+373, b_+375); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+375, b_+376); mem_wr(gb, DE, A);
  RET(b_+376); return;
}

// dragonOnox_leftClaw@seasonsFunc_0f_62a5
static void dragonOnox_seasonsFunc_0f_62a5(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+132, b_+133); E = alu_inc8(gb, E);
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+135); push_effect(gb, b_+135);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+164) goto seasonsFunc_0f_62a5_substate0;
    if (jt_ == b_+189) goto seasonsFunc_0f_62a5_substate1;
    if (jt_ == b_+205) goto seasonsFunc_0f_62a5_substate2;
    if (jt_ == b_+240) goto seasonsFunc_0f_62a5_substate3;
    if (jt_ == b_+275) goto seasonsFunc_0f_62a5_substate4;
    if (jt_ == b_+302) goto seasonsFunc_0f_62a5_substate5;
    if (jt_ == b_+377) goto seasonsFunc_0f_62a5_substate6;
    HANDOFF(HL);
  } while (0);
seasonsFunc_0f_62a5_substate0:
  CALL_C(b_+164, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+167);
  if (!(F & FZ)) { RET_TAKEN(b_+167); return; }
  CYC(b_+167, b_+168);
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x14);
  CYC(b_+170, b_+171); L = E;
  CYC(b_+171, b_+172); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+172, b_+174); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+174, b_+176); mem_wr(gb, HL, 0x28);
  CYC(b_+176, b_+178); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+178, b_+180); mem_wr(gb, HL, 0x00);
  CYC(b_+180, b_+182); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0xf8);
  CYC(b_+184, b_+186); A = 0x74;
  CYC(b_+186, b_+189);
  TAIL(playSound_b00);
seasonsFunc_0f_62a5_substate1:
  CALL_C(b_+189, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+192);
  if (!(F & FZ)) { CYCT(b_+192, b_+194); goto L_62e8; }
  CYC(b_+192, b_+194);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0x06);
  CYC(b_+196, b_+197); L = E;
  CYC(b_+197, b_+198); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+198); return;
L_62e8:
  CALL_C(b_+199, s_objectApplySpeed, SYM(objectApplySpeed), b_+202);
  CYC(b_+202, b_+205);
  goto seasonsFunc_0f_62a5_seasonsFunc_0f_6386;
seasonsFunc_0f_62a5_substate2:
  CALL_C(b_+205, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+208);
  if (F & FZ) { CYCT(b_+208, b_+210); goto L_6300; }
  CYC(b_+208, b_+210);
  CYC(b_+210, b_+211); A = mem_rd(gb, HL);
  CYC(b_+211, b_+213); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+213); return; }
  CYC(b_+213, b_+214);
  CYC(b_+214, b_+216); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+216, b_+219); A = mem_rd(gb, w1Link_xh);
  CYC(b_+219, b_+220); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+220, b_+222); mem_wr(gb, HL, 0xa5);
  RET(b_+222); return;
L_6300:
  CYC(b_+223, b_+224); L = E;
  CYC(b_+224, b_+225); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+225, b_+227); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+227, b_+229); mem_wr(gb, HL, 0x78);
  CYC(b_+229, b_+231); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+231, b_+233); mem_wr(gb, HL, 0x0e);
  CYC(b_+233, b_+235); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+235, b_+236); A = mem_rd(gb, DE);
  CYC(b_+236, b_+237); A = alu_inc8(gb, A);
  CYC(b_+237, b_+240);
  TAIL(enemySetAnimation);
seasonsFunc_0f_62a5_substate3:
  CYC(b_+240, b_+241); H = D;
  CYC(b_+241, b_+243); L = ENEMY_BASE + OBJ_VAR35;
  CALL_C(b_+243, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+246);
  CALL_C(b_+246, s_ecom_moveTowardPosition, SYM(ecom_moveTowardPosition_b0d), b_+249);
  CALL_L(b_+249, dragonOnox_seasonsFunc_0f_62a5_seasonsFunc_0f_6386, b_+252);
  CYC(b_+252, b_+254); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+254, b_+255); A = mem_rd(gb, DE);
  CYC(b_+255, b_+257); alu_cp(gb, 0xa0);
  if (F & FC) { RET_TAKEN(b_+257); return; }
  CYC(b_+257, b_+258);
  CYC(b_+258, b_+260); A = 0x1e;
  CYC(b_+260, b_+263); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+263, b_+264); H = D;
  CYC(b_+264, b_+266); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+266, b_+267); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+267, b_+268); L = alu_inc8(gb, L);
  CYC(b_+268, b_+270); mem_wr(gb, HL, 0x3c);
  CYC(b_+270, b_+272); A = 0x6f;
  CYC(b_+272, b_+275);
  TAIL(playSound_b00);
seasonsFunc_0f_62a5_substate4:
  CALL_C(b_+275, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+278);
  if (F & FZ) { CYCT(b_+278, b_+280); goto L_6348; }
  CYC(b_+278, b_+280);
  CYC(b_+280, b_+281); A = mem_rd(gb, HL);
  CYC(b_+281, b_+283); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { RET_TAKEN(b_+283); return; }
  CYC(b_+283, b_+284);
  CYC(b_+284, b_+286); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+286, b_+288); mem_wr(gb, HL, 0x09);
  CYC(b_+288, b_+290); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+290, b_+291); A = mem_rd(gb, DE);
  CYC(b_+291, b_+292); A = alu_dec8(gb, A);
  CYC(b_+292, b_+295);
  TAIL(enemySetAnimation);
L_6348:
  CYC(b_+295, b_+296); L = E;
  CYC(b_+296, b_+297); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+297, b_+299); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+299, b_+301); mem_wr(gb, HL, 0x1e);
  RET(b_+301); return;
seasonsFunc_0f_62a5_substate5:
  CYC(b_+302, b_+304); A = 0x0b;
  CALL_C(b_+304, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+307);
  CYC(b_+307, b_+308); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+308, b_+310); alu_add(gb, 0x30);
  CYC(b_+310, b_+311); B = A;
  CYC(b_+311, b_+312); L = alu_inc8(gb, L);
  CYC(b_+312, b_+314); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+314, b_+315); A = mem_rd(gb, DE);
  CYC(b_+315, b_+317); alu_cp(gb, 0x04);
  CYC(b_+317, b_+319); C = 0x28;
  if (F & FZ) { CYCT(b_+319, b_+321); goto L_6364; }
  CYC(b_+319, b_+321);
  CYC(b_+321, b_+323); C = 0x78;
L_6364:
  CYC(b_+323, b_+324); A = mem_rd(gb, HL);
  CYC(b_+324, b_+325); alu_add(gb, C);
  CYC(b_+325, b_+326); C = A;
  CYC(b_+326, b_+327); H = D;
  CYC(b_+327, b_+328); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+328, b_+330); alu_add(gb, 0x50);
  CYC(b_+330, b_+332); mem_wr(gb, hFF8E, A);
  CYC(b_+332, b_+333); L = alu_dec8(gb, L);
  CYC(b_+333, b_+334); A = mem_rd(gb, HL);
  CYC(b_+334, b_+336); mem_wr(gb, hFF8F, A);
  CYC(b_+336, b_+337); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+337, b_+339); goto L_637d; }
  CYC(b_+337, b_+339);
  CYC(b_+339, b_+341); A = mem_rd(gb, hFF8E);
  CYC(b_+341, b_+342); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+342, b_+344); goto L_637d; }
  CYC(b_+342, b_+344);
  CYC(b_+344, b_+346); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+346, b_+347); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+347); return;
L_637d:
  CALL_C(b_+348, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+351);
  CYC(b_+351, b_+353); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+353, b_+354); mem_wr(gb, DE, A);
  CALL_C(b_+354, s_objectApplySpeed, SYM(objectApplySpeed), b_+357);
seasonsFunc_0f_62a5_seasonsFunc_0f_6386:
  CYC(b_+357, b_+359); A = 0x0b;
  CALL_C(b_+359, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+362);
  CYC(b_+362, b_+363); E = L;
  CYC(b_+363, b_+364); A = mem_rd(gb, DE);
  CYC(b_+364, b_+365); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+365, b_+367); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+367, b_+368); mem_wr(gb, DE, A);
  CYC(b_+368, b_+370); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+370, b_+371); E = L;
  CYC(b_+371, b_+372); A = mem_rd(gb, DE);
  CYC(b_+372, b_+373); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+373, b_+375); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+375, b_+376); mem_wr(gb, DE, A);
  RET(b_+376); return;
seasonsFunc_0f_62a5_substate6:
  CYC(b_+377, b_+378); H = D;
  CYC(b_+378, b_+380); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+380, b_+381); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+381, b_+383); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+383, b_+385); mem_wr(gb, HL, 0xfc);
  CYC(b_+385, b_+387); A = 0x06;
  CALL_C(b_+387, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+390);
  CYC(b_+390, b_+391); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+391); return;
}

// dragonOnox_leftClaw@seasonsFunc_0f_6277
static void dragonOnox_seasonsFunc_0f_6277(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+86, b_+87); H = D;
  CYC(b_+87, b_+88); L = E;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x00);
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+92, b_+93); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+93, b_+94); mem_wr(gb, HL, A);
  CYC(b_+94, b_+95); L = alu_inc8(gb, L);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x00);
  CYC(b_+97, b_+98); L = alu_inc8(gb, L);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x1e);
  CYC(b_+100, b_+102); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_cp(gb, 0x04);
  CYC(b_+105, b_+107); A = 0xd8;
  if (F & FZ) { CYCT(b_+107, b_+109); goto L_6290; }
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+111); A = 0x28;
L_6290:
  CYC(b_+111, b_+113); L = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x30);
  RET(b_+116); return;
}

void s_dragonOnox_leftClaw_hook(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+45) goto state1;
    if (jt_ == b_+117) goto state2;
    if (jt_ == b_+392) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x65);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x05);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x09);
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0xfc);
  CYC(b_+29, b_+31); L = 0x97;
  CYC(b_+31, b_+34); A = mem_rd(gb, wTmpcfc0 + 0x17);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x80);
  CYC(b_+37, b_+39); A = 0x03;
  CALL_C(b_+39, s_enemySetAnimation, SYM(enemySetAnimation), b_+42);
  CALL_C(b_+42, s_objectSetVisible82, SYM(objectSetVisible82), b_+45);
state1:
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_or(gb, A);
  if (!(F & FZ)) CALL_L_CC(b_+49, dragonOnox_seasonsFunc_0f_6277, b_+52);
  else CYC(b_+49, b_+52);
  CYC(b_+52, b_+54); A = 0x00;
  CALL_C(b_+54, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+57);
  CYC(b_+57, b_+60); SET_BC(0x30d8);
  CYC(b_+60, b_+63); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+63, b_+65); alu_cp(gb, 0x06);
  if (F & FC) { CYCT(b_+65, b_+67); goto L_6271; }
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); alu_sub(gb, 0x09);
  if (F & FZ) { CYCT(b_+69, b_+71); goto L_6271; }
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+74); SET_BC(0x18d0);
  CYC(b_+74, b_+75); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+75, b_+77); goto L_6271; }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+80); SET_BC(0x30e1);
L_6271:
  CALL_C(b_+80, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+83);
  CYC(b_+83, b_+86);
  TAIL_S(seasonsFunc_0f_6557);

state2:
  CYC(b_+117, b_+120); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+120, b_+122); alu_sub(gb, 0x06);
  CYC(b_+122, b_+124); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+124, b_+126); goto L_62b6; }
  CYC(b_+124, b_+126);
  CALL_L(b_+126, dragonOnox_seasonsFunc_0f_62a5, b_+129);
  CYC(b_+129, b_+132);
  TAIL_S(seasonsFunc_0f_6557);

L_62b6:
  CYC(b_+149, b_+151); A = 0x00;
  CALL_C(b_+151, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+154);
  CYC(b_+154, b_+156); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+156, b_+157); A = mem_rd(gb, DE);
  CYC(b_+157, b_+158); B = A;
  CYC(b_+158, b_+159); E = alu_inc8(gb, E);
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); C = A;
  CYC(b_+161, b_+164);
  TAIL(objectTakePositionWithOffset);

state3:
  CYC(b_+392, b_+393); E = alu_inc8(gb, E);
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+395); push_effect(gb, b_+395);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+399) goto state3_substate0;
    if (jt_ == b_+421) goto state3_substate1;
    HANDOFF(HL);
  } while (0);
state3_substate0:
  CYC(b_+399, b_+400); H = D;
  CYC(b_+400, b_+401); L = E;
  CYC(b_+401, b_+402); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+402, b_+404); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+404, b_+406); mem_wr(gb, HL, 0x0e);
  CYC(b_+406, b_+407); L = alu_inc8(gb, L);
  CYC(b_+407, b_+409); mem_wr(gb, HL, 0x0a);
  CYC(b_+409, b_+411); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+411, b_+413); mem_wr(gb, HL, 0xf8);
  CYC(b_+413, b_+415); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+415, b_+416); A = mem_rd(gb, DE);
  CYC(b_+416, b_+418); alu_add(gb, 0x03);
  CALL_C(b_+418, s_enemySetAnimation, SYM(enemySetAnimation), b_+421);
state3_substate1:
  CYC(b_+421, b_+424); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+424, b_+425); alu_or(gb, A);
  if (F & FZ) CALL_L_CC(b_+425, dragonOnox_state3_seasonsFunc_0f_63e1, b_+428);
  else CYC(b_+425, b_+428);
  CYC(b_+428, b_+430); A = 0x00;
  CALL_C(b_+430, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+433);
  CYC(b_+433, b_+436); SET_BC(0x30d8);
  CYC(b_+436, b_+438); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+438, b_+439); A = mem_rd(gb, DE);
  CYC(b_+439, b_+441); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+441, b_+443); goto L_63de; }
  CYC(b_+441, b_+443);
  CYC(b_+443, b_+445); C = 0x28;
L_63de:
  CYC(b_+445, b_+448);
  TAIL(objectTakePositionWithOffset);
}

// dragonOnox_leftClaw@state3
static void dragonOnox_state3(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+392, b_+393); E = alu_inc8(gb, E);
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+395); push_effect(gb, b_+395);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+399) goto state3_substate0;
    if (jt_ == b_+421) goto state3_substate1;
    HANDOFF(HL);
  } while (0);
state3_substate0:
  CYC(b_+399, b_+400); H = D;
  CYC(b_+400, b_+401); L = E;
  CYC(b_+401, b_+402); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+402, b_+404); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+404, b_+406); mem_wr(gb, HL, 0x0e);
  CYC(b_+406, b_+407); L = alu_inc8(gb, L);
  CYC(b_+407, b_+409); mem_wr(gb, HL, 0x0a);
  CYC(b_+409, b_+411); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+411, b_+413); mem_wr(gb, HL, 0xf8);
  CYC(b_+413, b_+415); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+415, b_+416); A = mem_rd(gb, DE);
  CYC(b_+416, b_+418); alu_add(gb, 0x03);
  CALL_C(b_+418, s_enemySetAnimation, SYM(enemySetAnimation), b_+421);
state3_substate1:
  CYC(b_+421, b_+424); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+424, b_+425); alu_or(gb, A);
  if (F & FZ) CALL_L_CC(b_+425, dragonOnox_state3_seasonsFunc_0f_63e1, b_+428);
  else CYC(b_+425, b_+428);
  CYC(b_+428, b_+430); A = 0x00;
  CALL_C(b_+430, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+433);
  CYC(b_+433, b_+436); SET_BC(0x30d8);
  CYC(b_+436, b_+438); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+438, b_+439); A = mem_rd(gb, DE);
  CYC(b_+439, b_+441); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+441, b_+443); goto L_63de; }
  CYC(b_+441, b_+443);
  CYC(b_+443, b_+445); C = 0x28;
L_63de:
  CYC(b_+445, b_+448);
  TAIL(objectTakePositionWithOffset);
}

// dragonOnox_leftClaw@state2
static void dragonOnox_state2(GB *gb) {
  BASE(dragonOnox_leftClaw);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+117, b_+120); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+120, b_+122); alu_sub(gb, 0x06);
  CYC(b_+122, b_+124); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+124, b_+126); goto L_62b6; }
  CYC(b_+124, b_+126);
  CALL_L(b_+126, dragonOnox_seasonsFunc_0f_62a5, b_+129);
  CYC(b_+129, b_+132);
  TAIL_S(seasonsFunc_0f_6557);

L_62b6:
  CYC(b_+149, b_+151); A = 0x00;
  CALL_C(b_+151, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+154);
  CYC(b_+154, b_+156); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+156, b_+157); A = mem_rd(gb, DE);
  CYC(b_+157, b_+158); B = A;
  CYC(b_+158, b_+159); E = alu_inc8(gb, E);
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); C = A;
  CYC(b_+161, b_+164);
  TAIL(objectTakePositionWithOffset);
}

void s_dragonOnox_rightClaw_hook(GB *gb) {
  BASE(dragonOnox_rightClaw);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+48) goto state1;
    if (jt_ == (b_ - 354)) { dragonOnox_state2(gb); return; }
    if (jt_ == (b_ - 79)) { dragonOnox_state3(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x65);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x05);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x09);
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0xfc);
  CYC(b_+29, b_+31); L = 0x97;
  CYC(b_+31, b_+34); A = mem_rd(gb, wTmpcfc0 + 0x17);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x80);
  CYC(b_+37, b_+39); A = 0x04;
  CALL_C(b_+39, s_enemySetAnimation, SYM(enemySetAnimation), b_+42);
  CALL_C(b_+42, s_objectSetVisible82, SYM(objectSetVisible82), b_+45);
  CYC(b_+45, b_+48);
  goto state1;
state1:
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (!(F & FZ)) CALL_L_CC(b_+52, dragonOnox_seasonsFunc_0f_6277, b_+55);
  else CYC(b_+52, b_+55);
  CYC(b_+55, b_+57); A = 0x00;
  CALL_C(b_+57, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+60);
  CYC(b_+60, b_+63); SET_BC(0x3028);
  CYC(b_+63, b_+66); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+66, b_+68); alu_cp(gb, 0x06);
  if (F & FC) { CYCT(b_+68, b_+70); goto L_644b; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(b_+72, b_+74); goto L_644b; }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+77); SET_BC(0x1830);
  CYC(b_+77, b_+78); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+78, b_+80); goto L_644b; }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+83); SET_BC(0x3031);
L_644b:
  CALL_C(b_+83, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+86);
  CYC(b_+86, b_+89);
  TAIL_S(seasonsFunc_0f_6557);
}

void s_dragonOnox_leftClawSphere_hook(GB *gb) {
  BASE(dragonOnox_leftClawSphere);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+6) goto linkPartsAndAnimate;
    if (jt_ == b_+33) goto connectParts;
    HANDOFF(HL);
  } while (0);
linkPartsAndAnimate:
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0x97;
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x1a);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x80);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+19, b_+22); A = mem_rd(gb, wTmpcfc0 + 0x18);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x80);
  CYC(b_+25, b_+27); A = 0x0d;
  CALL_C(b_+27, s_enemySetAnimation, SYM(enemySetAnimation), b_+30);
  CALL_C(b_+30, s_objectSetVisible82, SYM(objectSetVisible82), b_+33);
connectParts:
  CALL_C(b_+33, s_dragonOnoxDistanceToRelatedObjects_hook, SYM(dragonOnoxDistanceToRelatedObjects), b_+36);
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+39); B = alu_sra(gb, B);
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); B = alu_sra(gb, B);
  CYC(b_+42, b_+43); alu_add(gb, B);
  CYC(b_+43, b_+44); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+47, b_+48); E = L;
  CYC(b_+48, b_+50); C = alu_sra(gb, C);
  CYC(b_+50, b_+51); A = C;
  CYC(b_+51, b_+53); C = alu_sra(gb, C);
  CYC(b_+53, b_+54); alu_add(gb, C);
  CYC(b_+54, b_+55); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  RET(b_+56); return;
}

// dragonOnox_leftClawSphere@connectParts
static void dragonOnox_connectParts(GB *gb) {
  BASE(dragonOnox_leftClawSphere);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+33, s_dragonOnoxDistanceToRelatedObjects_hook, SYM(dragonOnoxDistanceToRelatedObjects), b_+36);
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+39); B = alu_sra(gb, B);
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); B = alu_sra(gb, B);
  CYC(b_+42, b_+43); alu_add(gb, B);
  CYC(b_+43, b_+44); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+47, b_+48); E = L;
  CYC(b_+48, b_+50); C = alu_sra(gb, C);
  CYC(b_+50, b_+51); A = C;
  CYC(b_+51, b_+53); C = alu_sra(gb, C);
  CYC(b_+53, b_+54); alu_add(gb, C);
  CYC(b_+54, b_+55); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  RET(b_+56); return;
}

void s_dragonOnox_rightClawSphere_hook(GB *gb) {
  BASE(dragonOnox_rightClawSphere);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+6) goto linkPartsAndAnimate;
    if (jt_ == (b_ - 24)) { dragonOnox_connectParts(gb); return; }
    HANDOFF(HL);
  } while (0);
linkPartsAndAnimate:
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0x97;
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x1b);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x80);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+19, b_+22); A = mem_rd(gb, wTmpcfc0 + 0x19);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x80);
  CYC(b_+25, b_+27); A = 0x0e;
  CALL_C(b_+27, s_enemySetAnimation, SYM(enemySetAnimation), b_+30);
  CALL_C(b_+30, s_objectSetVisible82, SYM(objectSetVisible82), b_+33);
  CYC(b_+33, b_+35);
  dragonOnox_connectParts(gb); return;
}

void s_dragonOnox_leftShoulderSphere_hook(GB *gb) {
  BASE(dragonOnox_leftShoulderSphere);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+6) goto linkPartsAndAnimate;
    if (jt_ == b_+33) goto connectParts;
    HANDOFF(HL);
  } while (0);
linkPartsAndAnimate:
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0x97;
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x1a);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x80);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+19, b_+22); A = mem_rd(gb, wTmpcfc0 + 0x18);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x80);
  CYC(b_+25, b_+27); A = 0x0b;
  CALL_C(b_+27, s_enemySetAnimation, SYM(enemySetAnimation), b_+30);
  CALL_C(b_+30, s_objectSetVisible82, SYM(objectSetVisible82), b_+33);
connectParts:
  CALL_C(b_+33, s_dragonOnoxDistanceToRelatedObjects_hook, SYM(dragonOnoxDistanceToRelatedObjects), b_+36);
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+39); B = alu_sra(gb, B);
  CYC(b_+39, b_+41); B = alu_sra(gb, B);
  CYC(b_+41, b_+42); A = B;
  CYC(b_+42, b_+44); B = alu_sra(gb, B);
  CYC(b_+44, b_+45); alu_add(gb, B);
  CYC(b_+45, b_+46); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+49, b_+50); E = L;
  CYC(b_+50, b_+52); C = alu_sra(gb, C);
  CYC(b_+52, b_+54); C = alu_sra(gb, C);
  CYC(b_+54, b_+55); A = C;
  CYC(b_+55, b_+57); C = alu_sra(gb, C);
  CYC(b_+57, b_+58); alu_add(gb, C);
  CYC(b_+58, b_+59); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  RET(b_+60); return;
}

// dragonOnox_leftShoulderSphere@connectParts
static void dragonOnox_dragonOnox_leftShoulderSphere_connectParts(GB *gb) {
  BASE(dragonOnox_leftShoulderSphere);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+33, s_dragonOnoxDistanceToRelatedObjects_hook, SYM(dragonOnoxDistanceToRelatedObjects), b_+36);
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+39); B = alu_sra(gb, B);
  CYC(b_+39, b_+41); B = alu_sra(gb, B);
  CYC(b_+41, b_+42); A = B;
  CYC(b_+42, b_+44); B = alu_sra(gb, B);
  CYC(b_+44, b_+45); alu_add(gb, B);
  CYC(b_+45, b_+46); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+49, b_+50); E = L;
  CYC(b_+50, b_+52); C = alu_sra(gb, C);
  CYC(b_+52, b_+54); C = alu_sra(gb, C);
  CYC(b_+54, b_+55); A = C;
  CYC(b_+55, b_+57); C = alu_sra(gb, C);
  CYC(b_+57, b_+58); alu_add(gb, C);
  CYC(b_+58, b_+59); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  RET(b_+60); return;
}

void s_dragonOnox_rightShoulderSphere_hook(GB *gb) {
  BASE(dragonOnox_rightShoulderSphere);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (dragonOnox_jump_table(gb));
    if (jt_ == b_+6) goto linkPartsAndAnimate;
    if (jt_ == (b_ - 28)) { dragonOnox_dragonOnox_leftShoulderSphere_connectParts(gb); return; }
    HANDOFF(HL);
  } while (0);
linkPartsAndAnimate:
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0x97;
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x1b);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x80);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+19, b_+22); A = mem_rd(gb, wTmpcfc0 + 0x19);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x80);
  CYC(b_+25, b_+27); A = 0x0c;
  CALL_C(b_+27, s_enemySetAnimation, SYM(enemySetAnimation), b_+30);
  CALL_C(b_+30, s_objectSetVisible82, SYM(objectSetVisible82), b_+33);
  CYC(b_+33, b_+35);
  dragonOnox_dragonOnox_leftShoulderSphere_connectParts(gb); return;
}

void s_seasonsFunc_0f_650d_hook(GB *gb) {
  BASE(seasonsFunc_0f_650d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+5); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+11); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, s_objectApplySpeed, SYM(objectApplySpeed), b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcfc0 + 0x0c, A);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+27); mem_wr(gb, wTmpcfc0 + 0x0d, A);
  RET(b_+27); return;
}

void s_seasonsFunc_0f_6529_hook(GB *gb) {
  BASE(seasonsFunc_0f_6529);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = H;
  CYC(b_+1, b_+3); alu_add(gb, 0x60);
  CYC(b_+3, b_+5); mem_wr(gb, hFF8F, A);
  CYC(b_+5, b_+6); A = L;
  CYC(b_+6, b_+8); alu_add(gb, 0x50);
  CYC(b_+8, b_+10); mem_wr(gb, hFF8E, A);
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+13); alu_add(gb, 0x60);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_add(gb, 0x50);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+21);
  TAIL(objectGetRelativeAngleWithTempVars);
}

// @param[out]	b	relatedObj1.yh - relatedObj2.yh
// @param[out]	c	relatedObj1.xh - relatedObj2.xh
// @param[out]	hl	relatedObj2.yh
void s_dragonOnoxDistanceToRelatedObjects_hook(GB *gb) {
  BASE(dragonOnoxDistanceToRelatedObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CYC(b_+10, b_+12); A = 0x0b;
  CALL_C(b_+12, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_sub(gb, B);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); alu_sub(gb, C);
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+24); SET_HL(pop_effect(gb));
  RET(b_+24); return;
}

void s_seasonsFunc_0f_6557_hook(GB *gb) {
  BASE(seasonsFunc_0f_6557);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, w1Link_speedZ + 0x01);
  CYC(b_+9, b_+10); alu_rlca(gb);
  if (F & FC) { CYCT(b_+10, b_+12); TAIL_S(seasonsFunc_0f_65b7); }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_add(gb, 0x06);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+19); alu_add(gb, A);
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+23, b_+26); A = mem_rd(gb, w1Link_xh);
  CYC(b_+26, b_+27); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+27, b_+28); alu_add(gb, B);
  CYC(b_+28, b_+29); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+29, b_+31); TAIL_S(seasonsFunc_0f_65b7); }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+33, b_+36); A = mem_rd(gb, w1Link_collisionRadiusY);
  CYC(b_+36, b_+37); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+37, b_+39); alu_add(gb, 0x03);
  CYC(b_+39, b_+40); B = A;
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_sub(gb, B);
  CYC(b_+44, b_+45); C = A;
  CYC(b_+45, b_+48); A = mem_rd(gb, w1Link_yh);
  CYC(b_+48, b_+49); alu_sub(gb, C);
  CYC(b_+49, b_+50); A = alu_inc8(gb, A);
  CYC(b_+50, b_+52); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+52, b_+54); TAIL_S(seasonsFunc_0f_65b7); }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); A = D;
  CYC(b_+55, b_+58); mem_wr(gb, wLinkRidingObject, A);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+60, b_+62); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+62, b_+64); goto L_659b; }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+65); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+65, s_seasonsFunc_0f_65bb_hook, SYM(seasonsFunc_0f_65bb), b_+68);
L_659b:
  CYC(b_+68, b_+69); A = C;
  CYC(b_+69, b_+72); mem_wr(gb, w1Link_yh, A);
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+77, b_+78); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+78, b_+79); E = A;
  CYC(b_+79, b_+82); A = mem_rd(gb, w1Link_xh);
  CYC(b_+82, b_+83); alu_add(gb, E);
  CYC(b_+83, b_+85); alu_sub(gb, 0x05);
  CYC(b_+85, b_+87); alu_cp(gb, 0x97);
  if (!(F & FC)) { CYCT(b_+87, b_+89); TAIL_S(seasonsFunc_0f_65bb); }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); alu_add(gb, 0x05);
  CYC(b_+91, b_+94); mem_wr(gb, w1Link_xh, A);
  CYC(b_+94, b_+96);
  TAIL_S(seasonsFunc_0f_65bb);
}

void s_seasonsFunc_0f_65b7_hook(GB *gb) {
  BASE(seasonsFunc_0f_65b7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x00);
  s_seasonsFunc_0f_65bb_hook(gb); return; // falls through
}

void s_seasonsFunc_0f_65bb_hook(GB *gb) {
  BASE(seasonsFunc_0f_65bb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return;
}

void s_seasonsFunc_0f_65c7_hook(GB *gb) {
  BASE(seasonsFunc_0f_65c7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0a);
  CYC(b_+3, b_+5); alu_and(gb, 0x0e);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+8); alu_add(gb, B);
  CYC(b_+8, b_+11); SET_HL(SYM(seasonsTable_0f_65ed));
  CYC(b_+11, b_+12); dragonOnox_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+17); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+17, b_+18); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+21, b_+22); SET_HL(HL + 1);
  CYC(b_+22, b_+25); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+25, b_+26); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); SET_HL(HL + 1);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+38);
  TAIL(enemySetAnimation);
}

void s_seasonsFunc_0f_65fc_hook(GB *gb) {
  BASE(seasonsFunc_0f_65fc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_661c; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  CYC(b_+10, b_+12); B = 0x14;
  if (F & FZ) { CYCT(b_+12, b_+14); goto L_660c; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); B = 0x06;
L_660c:
  CYC(b_+16, b_+17); mem_wr(gb, HL, B);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+22, b_+24); goto L_6617; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_xor(gb, 0x01);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
L_6617:
  CYC(b_+27, b_+29); alu_or(gb, 0x80);
  CYC(b_+29, b_+32); mem_wr(gb, wTmpcfc0 + 0x09, A);
L_661c:
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); alu_or(gb, A);
  CYC(b_+37, b_+39); B = 0x0c;
  if (F & FZ) { CYCT(b_+39, b_+41); goto L_6627; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); B = 0x04;
L_6627:
  CYC(b_+43, b_+44); mem_wr(gb, HL, B);
  CYC(b_+44, b_+47); A = mem_rd(gb, wTmpcfc0 + 0x0b);
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);
  CYC(b_+48, b_+50); alu_cp(gb, 0x06);
  if (F & FC) { CYCT(b_+50, b_+52); goto L_6631; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); alu_xor(gb, A);
L_6631:
  CYC(b_+53, b_+55); alu_or(gb, 0x80);
  CYC(b_+55, b_+58); mem_wr(gb, wTmpcfc0 + 0x0b, A);
  RET(b_+58); return;
}

void s_seasonsFunc_0f_6637_hook(GB *gb) {
  BASE(seasonsFunc_0f_6637);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+12); SET_HL(SYM(seasonsTable_0f_664c));
  CYC(b_+12, b_+13); dragonOnox_add_double_index(gb, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); E = alu_inc8(gb, E);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void s_seasonsFunc_0f_665c_hook(GB *gb) {
  BASE(seasonsFunc_0f_665c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0xff);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x14);
  CALL_C(b_+11, s_seasonsFunc_0f_6680_hook, SYM(seasonsFunc_0f_6680), b_+14);
  CALL_C(b_+14, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x07);
  CYC(b_+19, b_+22); SET_HL(SYM(seasonsTable_0f_6678));
  CYC(b_+22, b_+23); dragonOnox_add_a_to_hl(gb, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  RET(b_+27); return;
}

void s_seasonsFunc_0f_6680_hook(GB *gb) {
  BASE(seasonsFunc_0f_6680);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6680:
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+10, b_+12); goto L_6680; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); A = B;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+17); SET_HL(SYM(seasonsTable_0f_669a));
  CYC(b_+17, b_+18); dragonOnox_add_double_index(gb, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  RET(b_+25); return;
}

void s_seasonsFunc_0f_66aa_hook(GB *gb) {
  BASE(seasonsFunc_0f_66aa);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x0c);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wTmpcfc0 + 0x0d);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+9); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_66b8; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = H;
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
L_66b8:
  CALL_C(b_+14, s_seasonsFunc_0f_6529_hook, SYM(seasonsFunc_0f_6529), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, s_seasonsFunc_0f_650d_hook, SYM(seasonsFunc_0f_650d), b_+23);
  CYC(b_+23, b_+24); alu_or(gb, D);
  RET(b_+24); return;
}

void s_dragonOnoxLoadaIntoVar30Andcfc9_hook(GB *gb) {
  BASE(dragonOnoxLoadaIntoVar30Andcfc9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+5); alu_or(gb, 0x80);
  CYC(b_+5, b_+8); mem_wr(gb, wTmpcfc0 + 0x09, A);
  RET(b_+8); return;
}

void s_dragonOnoxLowHealthThresholdIntoC_hook(GB *gb) {
  BASE(dragonOnoxLowHealthThresholdIntoC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+3);
  CYC(b_+3, b_+5); C = 0x11;
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); C = 0x18;
  RET(b_+8); return;
}

