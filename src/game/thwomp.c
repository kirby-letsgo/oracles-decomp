#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  CYC(0x6712, 0x6714); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6714, 0x6715); A = mem_rd(gb, DE);
  CYC(0x6715, 0x6716); push_effect(gb, 0x6716);
  uint16_t target = enemyCode2f_jump_table(gb);
  if (target == 0x6744) { thwomp_state8_hook(gb); return; }
  if (target == 0x6770) { thwomp_state9_hook(gb); return; }
  if (target == 0x6791) { thwomp_stateA_hook(gb); return; }
  if (target == 0x67b0) { thwomp_stateB_hook(gb); return; }
  if (target == 0x672e) { thwomp_uninitialized_hook(gb); return; }
  thwomp_state_stub_hook(gb); return; // states 1-7 all target 0x6743
}

// ==================================================================================================
// ENEMY_THWOMP
//
// Variables:
//   var30: Original y-position (where it returns to after stomping)
// ==================================================================================================
void enemyCode2f_hook(GB *gb) {
  if (F & FZ) { CYCT(0x6707, 0x6709); goto normalStatus; } // jr z
  CYC(0x6707, 0x6709);
  CYC(0x6709, 0x670b); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x670b, 0x670c); ret_effect(gb); return; } // ret c
  CYC(0x670b, 0x670c);

normalStatus:
  CYC(0x670c, 0x670f); push_effect(gb, 0x670f); enemyCode2f_runState(gb);
  CYC(0x670f, 0x6712); thwomp_updateLinkRidingSelf_hook(gb); return; // jp
}

void thwomp_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x672e, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x6731);
  CYC(0x6731, 0x6733); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6733, 0x6735); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6735, 0x6736); A = mem_rd(gb, DE);
  CYC(0x6736, 0x6737); mem_wr(gb, HL, A);
  CYC(0x6737, 0x6739); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6739, 0x673b); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x673b, 0x673d); A = 0x04;
  CALL_C(0x673d, enemySetAnimation_hook, 0x282b, 0x6740);
  CYC(0x6740, 0x6743); objectSetVisible82_hook(gb); return; // jp
}

void thwomp_state_stub_hook(GB *gb) {
  RET(0x6743); return;
}

// Waiting for Link to approach
void thwomp_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6744, 0x6745); H = D;
  CYC(0x6745, 0x6747); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6747, 0x674a); A = W8(w1Link_xh);
  CYC(0x674a, 0x674b); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x674b, 0x674d); alu_add(gb, 0x14);
  CYC(0x674d, 0x674f); alu_cp(gb, 0x29);
  if (F & FC) { CYCT(0x674f, 0x6751); goto linkApproached; } // jr c
  CYC(0x674f, 0x6751);
  // Update eye looking at Link
  CALL_C(0x6751, objectGetAngleTowardLink_hook, 0x1e9c, 0x6754);
  CYC(0x6754, 0x6756); alu_add(gb, 0x02);
  CYC(0x6756, 0x6758); alu_and(gb, 0x1c);
  CYC(0x6758, 0x6759); H = D;
  CYC(0x6759, 0x675b); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x675b, 0x675c); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x675c, 0x675d); ret_effect(gb); return; } // ret z
  CYC(0x675c, 0x675d);
  CYC(0x675d, 0x675e); mem_wr(gb, HL, A);
  CYC(0x675e, 0x675f); alu_rrca(gb);
  CYC(0x675f, 0x6760); alu_rrca(gb);
  CYC(0x6760, 0x6763); enemySetAnimation_hook(gb); return; // jp

linkApproached:
  CALL_C(0x6763, ecom_incState_b0d_hook, 0x4000, 0x6766);
  CYC(0x6766, 0x6768); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6768, 0x6769); alu_xor(gb, A);
  CYC(0x6769, 0x676a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x676a, 0x676b); mem_wr(gb, HL, A);
  CYC(0x676b, 0x676d); A = 0x08;
  CYC(0x676d, 0x6770); enemySetAnimation_hook(gb); return; // jp
}

// Falling to ground
void thwomp_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6770, 0x6772); B = 0x10;
  CYC(0x6772, 0x6774); A = 0x30;
  CALL_C(0x6774, objectUpdateSpeedZ_sidescroll_givenYOffset_hook, 0x1f68, 0x6777);
  if (F & FC) { CYCT(0x6777, 0x6779); goto hitGround; } // jr c
  CYC(0x6777, 0x6779);
  // Cap speedZ to $0200 (ish... doesn't fix the low byte)
  CYC(0x6779, 0x677a); A = mem_rd(gb, HL);
  CYC(0x677a, 0x677c); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x677c, 0x677d); ret_effect(gb); return; } // ret c
  CYC(0x677c, 0x677d);
  CYC(0x677d, 0x677f); mem_wr(gb, HL, 0x02);
  RET(0x677f); return;

hitGround:
  CALL_C(0x6780, ecom_incState_b0d_hook, 0x4000, 0x6783);
  CYC(0x6783, 0x6785); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6785, 0x6787); mem_wr(gb, HL, 60);
  CYC(0x6787, 0x6789); A = 45;
  CYC(0x6789, 0x678c); W8(wScreenShakeCounterY) = A;
  CYC(0x678c, 0x678e); A = 0x70; // SND_DOORCLOSE
  CYC(0x678e, 0x6791); playSound_b00_hook(gb); return; // jp
}

// Resting on ground for 50 frames after hitting it, then moving back to starting position
void thwomp_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6791, ecom_decCounter2_b0d_hook, 0x43a3, 0x6794);
  if (!(F & FZ)) { CYCT(0x6794, 0x6795); ret_effect(gb); return; } // ret nz
  CYC(0x6794, 0x6795);
  CYC(0x6795, 0x6797); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6797, 0x6799); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6799, 0x679a); A = mem_rd(gb, DE);
  CYC(0x679a, 0x679b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x679b, 0x679d); goto doneMovingUp; } // jr z
  CYC(0x679b, 0x679d);
  CYC(0x679d, 0x679f); L = ENEMY_BASE + OBJ_Y;
  CYC(0x679f, 0x67a0); A = mem_rd(gb, HL);
  CYC(0x67a0, 0x67a2); alu_sub(gb, 0x80);
  CYC(0x67a2, 0x67a3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x67a3, 0x67a4); A = mem_rd(gb, HL);
  CYC(0x67a4, 0x67a6); alu_sbc(gb, 0x00);
  CYC(0x67a6, 0x67a7); mem_wr(gb, HL, A);
  RET(0x67a7); return;

doneMovingUp:
  CYC(0x67a8, 0x67aa); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x67aa, 0x67ab); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x67ab, 0x67ad); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x67ad, 0x67af); mem_wr(gb, HL, 0x20);
  RET(0x67af); return;
}

// Cooldown before stomping again
void thwomp_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x67b0, ecom_decCounter1_b0d_hook, 0x439a, 0x67b3);
  if (!(F & FZ)) { CYCT(0x67b3, 0x67b4); ret_effect(gb); return; } // ret nz
  CYC(0x67b3, 0x67b4);
  CYC(0x67b4, 0x67b5); L = E;
  CYC(0x67b5, 0x67b7); mem_wr(gb, HL, 0x08); // [state] = 8
  CYC(0x67b7, 0x67ba); thwomp_updateLinkRidingSelf_hook(gb); return; // jp
}

// Unused function.
//
// @param bc  Position offset
// @param[out] a  Tile collisions at thwomp's position + offset bc
void thwomp_func67ba_hook(GB *gb) {
  CYC(0x67ba, 0x67bc); E = ENEMY_BASE + OBJ_YH;
  CYC(0x67bc, 0x67bd); A = mem_rd(gb, DE);
  CYC(0x67bd, 0x67be); alu_add(gb, B);
  CYC(0x67be, 0x67bf); B = A;
  CYC(0x67bf, 0x67c1); E = ENEMY_BASE + OBJ_XH;
  CYC(0x67c1, 0x67c2); A = mem_rd(gb, DE);
  CYC(0x67c2, 0x67c3); C = A;
  CYC(0x67c3, 0x67c6); getTileCollisionsAtPosition_hook(gb); return; // jp
}

// Checks if Link is riding the thwomp, updates appropriate variables if so.
void thwomp_updateLinkRidingSelf_hook(GB *gb) {
  CYC(0x67c6, 0x67c7); H = D;
  CYC(0x67c7, 0x67c9); L = ENEMY_BASE + OBJ_XH;
  CYC(0x67c9, 0x67cc); A = W8(w1Link_xh);
  CYC(0x67cc, 0x67cd); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x67cd, 0x67cf); alu_add(gb, 0x13);
  CYC(0x67cf, 0x67d1); alu_cp(gb, 0x27);
  if (!(F & FC)) { CYCT(0x67d1, 0x67d3); goto notRiding; } // jr nc
  CYC(0x67d1, 0x67d3);
  CYC(0x67d3, 0x67d6); A = W8(w1Link_collisionRadiusY);
  CYC(0x67d6, 0x67d7); B = A;
  CYC(0x67d7, 0x67d9); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x67d9, 0x67db); E = ENEMY_BASE + OBJ_YH;
  CYC(0x67db, 0x67dc); A = mem_rd(gb, DE);
  CYC(0x67dc, 0x67dd); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x67dd, 0x67de); alu_sub(gb, B);
  CYC(0x67de, 0x67df); C = A;
  CYC(0x67df, 0x67e2); A = W8(w1Link_yh);
  CYC(0x67e2, 0x67e3); alu_sub(gb, C);
  CYC(0x67e3, 0x67e5); alu_add(gb, 0x03);
  CYC(0x67e5, 0x67e7); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(0x67e7, 0x67e9); goto notRiding; } // jr nc
  CYC(0x67e7, 0x67e9);
  CYC(0x67e9, 0x67ea); A = C;
  CYC(0x67ea, 0x67ec); alu_sub(gb, 0x03);
  CYC(0x67ec, 0x67ef); W8(w1Link_yh) = A;
  CYC(0x67ef, 0x67f0); A = D;
  CYC(0x67f0, 0x67f3); W8(wLinkRidingObject) = A;
  RET(0x67f3); return;

notRiding:
  // Only clear wLinkRidingObject if it's already equal to this object's index.
  CYC(0x67f4, 0x67f7); A = W8(wLinkRidingObject);
  CYC(0x67f7, 0x67f8); alu_sub(gb, D);
  if (!(F & FZ)) { CYCT(0x67f8, 0x67f9); ret_effect(gb); return; } // ret nz
  CYC(0x67f8, 0x67f9);
  CYC(0x67f9, 0x67fc); W8(wLinkRidingObject) = A;
  RET(0x67fc); return;
}
