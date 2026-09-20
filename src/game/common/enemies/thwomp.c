#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode2f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode2f), (from), (to), true)

static uint16_t enemyCode2f_jump_table(GB *gb) {
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

void thwomp_uninitialized_hook(GB *gb);
void thwomp_state_stub_hook(GB *gb);
void thwomp_state8_hook(GB *gb);
void thwomp_state9_hook(GB *gb);
void thwomp_stateA_hook(GB *gb);
void thwomp_stateB_hook(GB *gb);
void thwomp_updateLinkRidingSelf_hook(GB *gb);

// enemyCode2f@runState: an @-local reached only by one genuine `call`, from enemyCode2f below,
// which pushes its own return address first. Its jump table's targets are the bare-global
// thwomp_state*_hook functions, each of which ends in a real `ret` (or a tail-jump into one),
// so plain nested C calls here unwind naturally back through this function and into
// enemyCode2f_hook once that `ret` pops the address enemyCode2f_hook pushed.
static void enemyCode2f_runState(GB *gb) {
  BASE(enemyCode2f);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); push_effect(gb, b_+15);
  uint16_t target = enemyCode2f_jump_table(gb);
  if (target == SYM(thwomp_state8)) { thwomp_state8_hook(gb); return; }
  if (target == SYM(thwomp_state9)) { thwomp_state9_hook(gb); return; }
  if (target == SYM(thwomp_stateA)) { thwomp_stateA_hook(gb); return; }
  if (target == SYM(thwomp_stateB)) { thwomp_stateB_hook(gb); return; }
  if (target == SYM(thwomp_uninitialized)) { thwomp_uninitialized_hook(gb); return; }
  thwomp_state_stub_hook(gb); return; // states 1-7 all target 0x6743
}

// ==================================================================================================
// ENEMY_THWOMP
//
// Variables:
//   var30: Original y-position (where it returns to after stomping)
// ==================================================================================================
void enemyCode2f_hook(GB *gb) {
  BASE(enemyCode2f);
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);

normalStatus:
  CYC(b_+5, b_+8); push_effect(gb, b_+8); enemyCode2f_runState(gb);
  CYC(b_+8, b_+11); thwomp_updateLinkRidingSelf_hook(gb); return; // jp
}

void thwomp_uninitialized_hook(GB *gb) {
  BASE(thwomp_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+13, b_+15); A = 0x04;
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), b_+18);
  CYC(b_+18, SYM(thwomp_state_stub)); objectSetVisible82_hook(gb); return; // jp
}

void thwomp_state_stub_hook(GB *gb) {
  BASE(thwomp_state_stub);
  RET(b_+0); return;
}

// Waiting for Link to approach
void thwomp_state8_hook(GB *gb) {
  BASE(thwomp_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+3, b_+6); A = W8(w1Link_xh);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); alu_add(gb, 0x14);
  CYC(b_+9, b_+11); alu_cp(gb, 0x29);
  if (F & FC) { CYCT(b_+11, b_+13); goto linkApproached; } // jr c
  CYC(b_+11, b_+13);
  // Update eye looking at Link
  CALL_C(b_+13, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+16);
  CYC(b_+16, b_+18); alu_add(gb, 0x02);
  CYC(b_+18, b_+20); alu_and(gb, 0x1c);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+24, b_+25); ret_effect(gb); return; } // ret z
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+27); alu_rrca(gb);
  CYC(b_+27, b_+28); alu_rrca(gb);
  CYC(b_+28, b_+31); enemySetAnimation_hook(gb); return; // jp

linkApproached:
  CALL_C(b_+31, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+34);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+36, b_+37); alu_xor(gb, A);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); A = 0x08;
  CYC(b_+41, SYM(thwomp_state9)); enemySetAnimation_hook(gb); return; // jp
}

// Falling to ground
void thwomp_state9_hook(GB *gb) {
  BASE(thwomp_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x10;
  CYC(b_+2, b_+4); A = 0x30;
  CALL_C(b_+4, objectUpdateSpeedZ_sidescroll_givenYOffset_hook, SYM(objectUpdateSpeedZ_sidescroll_givenYOffset), b_+7);
  if (F & FC) { CYCT(b_+7, b_+9); goto hitGround; } // jr c
  CYC(b_+7, b_+9);
  // Cap speedZ to $0200 (ish... doesn't fix the low byte)
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+12, b_+13); ret_effect(gb); return; } // ret c
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x02);
  RET(b_+15); return;

hitGround:
  CALL_C(b_+16, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 60);
  CYC(b_+23, b_+25); A = 45;
  CYC(b_+25, b_+28); W8(wScreenShakeCounterY) = A;
  CYC(b_+28, b_+30); A = 0x70; // SND_DOORCLOSE
  CYC(b_+30, SYM(thwomp_stateA)); playSound_b00_hook(gb); return; // jp
}

// Resting on ground for 50 frames after hitting it, then moving back to starting position
void thwomp_stateA_hook(GB *gb) {
  BASE(thwomp_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+10, b_+12); goto doneMovingUp; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_Y;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_sub(gb, 0x80);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_sbc(gb, 0x00);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  RET(b_+22); return;

doneMovingUp:
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x20);
  RET(b_+30); return;
}

// Cooldown before stomping again
void thwomp_stateB_hook(GB *gb) {
  BASE(thwomp_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08); // [state] = 8
  CYC(b_+7, SYM(thwomp_func67ba)); thwomp_updateLinkRidingSelf_hook(gb); return; // jp
}

// Unused function.
//
// @param bc  Position offset
// @param[out] a  Tile collisions at thwomp's position + offset bc
void thwomp_func67ba_hook(GB *gb) {
  BASE(thwomp_func67ba);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, B);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, SYM(thwomp_updateLinkRidingSelf)); getTileCollisionsAtPosition_hook(gb); return; // jp
}

// Checks if Link is riding the thwomp, updates appropriate variables if so.
void thwomp_updateLinkRidingSelf_hook(GB *gb) {
  BASE(thwomp_updateLinkRidingSelf);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+3, b_+6); A = W8(w1Link_xh);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); alu_add(gb, 0x13);
  CYC(b_+9, b_+11); alu_cp(gb, 0x27);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto notRiding; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = W8(w1Link_collisionRadiusY);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); alu_sub(gb, B);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+28); A = W8(w1Link_yh);
  CYC(b_+28, b_+29); alu_sub(gb, C);
  CYC(b_+29, b_+31); alu_add(gb, 0x03);
  CYC(b_+31, b_+33); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+33, b_+35); goto notRiding; } // jr nc
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); A = C;
  CYC(b_+36, b_+38); alu_sub(gb, 0x03);
  CYC(b_+38, b_+41); W8(w1Link_yh) = A;
  CYC(b_+41, b_+42); A = D;
  CYC(b_+42, b_+45); W8(wLinkRidingObject) = A;
  RET(b_+45); return;

notRiding:
  // Only clear wLinkRidingObject if it's already equal to this object's index.
  CYC(b_+46, b_+49); A = W8(wLinkRidingObject);
  CYC(b_+49, b_+50); alu_sub(gb, D);
  if (!(F & FZ)) { CYCT(b_+50, b_+51); ret_effect(gb); return; } // ret nz
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+54); W8(wLinkRidingObject) = A;
  RET(b_+54); return;
}
