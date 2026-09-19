#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void keese_state_uninitialized_hook(GB *gb);
void keese_state_stub_hook(GB *gb);
void keese_subid00_hook(GB *gb);
void keese_subid00_state8_hook(GB *gb);
void keese_subid00_state9_hook(GB *gb);
void keese_animate_hook(GB *gb);
void keese_subid00_stateA_hook(GB *gb);
void keese_subid01_hook(GB *gb);
void keese_subid01_state8_hook(GB *gb);
void keese_subid02_state9_hook(GB *gb);
void keese_updateDeceleration_hook(GB *gb);
void keese_initializeSubid_hook(GB *gb);
void keese_chooseWhetherToReverseTurningAngle_hook(GB *gb);

static uint16_t keese_jump_table(GB *gb) {
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

static void keese_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_KEESE
//
// Variables (for subid 1 only, the one that moves as Link approaches):
//   var30: Amount to add to angle each frame. (Clockwise or counterclockwise turning)
// ==================================================================================================
void enemyCode32_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x471c, 0x471e); goto normalStatus; } // jr z
  CYC(0x471c, 0x471e);
  CYC(0x471e, 0x4720); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4720); return; } // ret c
  CYC(0x4720, 0x4721);
  if (F & FZ) { CYCT(0x4721, 0x4724); enemyDie_hook(gb); return; } // jp z
  CYC(0x4721, 0x4724);
  CYC(0x4724, 0x4725); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4725, 0x4728); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(0x4725, 0x4728);
  RET(0x4728); return; // ret

normalStatus:
  CALL_C(0x4729, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x472c);
  if (!(F & FC)) { CYCT(0x472c, 0x472e); goto normalState; } // jr nc
  CYC(0x472c, 0x472e);
  {
    CYC(0x472e, 0x472f); push_effect(gb, 0x472f);
    uint16_t target = keese_jump_table(gb);
    if (target == 0x4745) { keese_state_uninitialized_hook(gb); return; }
    if (target == 0x474e) { keese_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x473f, 0x4740); A = B;
  {
    CYC(0x4740, 0x4741); push_effect(gb, 0x4741);
    uint16_t target = keese_jump_table(gb);
    if (target == 0x474f) { keese_subid00_hook(gb); return; }
    if (target == 0x47ca) { keese_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

void keese_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4745, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x4748);
  CALL_C(0x4748, keese_initializeSubid_hook, 0x4851, 0x474b);
  CYC(0x474b, 0x474e); objectSetVisible82_hook(gb); return; // jp
}

void keese_state_stub_hook(GB *gb) {
  RET(0x474e); return; // ret
}

void keese_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x474f, 0x4750); A = mem_rd(gb, DE);
  CYC(0x4750, 0x4752); alu_sub(gb, 0x08);
  {
    CYC(0x4752, 0x4753); push_effect(gb, 0x4753);
    uint16_t target = keese_jump_table(gb);
    if (target == 0x4759) { keese_subid00_state8_hook(gb); return; }
    if (target == 0x477a) { keese_subid00_state9_hook(gb); return; }
    if (target == 0x47a0) { keese_subid00_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// Resting for [counter1] frames
void keese_subid00_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4759, ecom_decCounter1_b0e_hook, 0x439a, 0x475c);
  if (!(F & FZ)) { RET_TAKEN(0x475c); return; } // ret nz
  CYC(0x475c, 0x475d);
  CYC(0x475d, 0x4760); SET_BC(0x1f3f);
  CALL_C(0x4760, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x4763);
  CALL_C(0x4763, ecom_incState_b0e_hook, 0x4000, 0x4766);
  CYC(0x4766, 0x4768); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4768, 0x4769); mem_wr(gb, HL, B);
  CYC(0x4769, 0x476b); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x476b, 0x476d); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x476d, 0x476f); A = 0xc0;
  CYC(0x476f, 0x4770); alu_add(gb, C);
  CYC(0x4770, 0x4772); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4772, 0x4773); mem_wr(gb, HL, A);
  CYC(0x4773, 0x4775); A = 0x01;
  CALL_C(0x4775, enemySetAnimation_hook, 0x282b, 0x4778);
  CYCT(0x4778, 0x477a); keese_animate_hook(gb); return; // jr
}

// Moving in some direction for [counter1] frames
void keese_subid00_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x477a, objectApplySpeed_hook, 0x201d, 0x477d);
  CALL_C(0x477d, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x4780);
  CYC(0x4780, 0x4783); A = mem_rd(gb, wFrameCounter);
  CYC(0x4783, 0x4784); alu_rrca(gb);
  if (F & FC) { CYCT(0x4784, 0x4786); keese_animate_hook(gb); return; } // jr c
  CYC(0x4784, 0x4786);
  CALL_C(0x4786, ecom_decCounter1_b0e_hook, 0x439a, 0x4789);
  if (F & FZ) { CYCT(0x4789, 0x478b); goto timeToStop; } // jr z
  CYC(0x4789, 0x478b);
  CYC(0x478b, 0x478e); SET_BC(0x0f1f);
  CALL_C(0x478e, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x4791);
  CYC(0x4791, 0x4792); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x4792, 0x4794); keese_animate_hook(gb); return; } // jr nz
  CYC(0x4792, 0x4794);
  CYC(0x4794, 0x4796); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4796, 0x4797); A = C;
  CYC(0x4797, 0x4798); mem_wr(gb, DE, A);
  CYCT(0x4798, 0x479a); keese_animate_hook(gb); return; // jr

timeToStop:
  CYC(0x479a, 0x479c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x479c, 0x479d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  keese_animate_hook(gb); return; // fallthrough
}

void keese_animate_hook(GB *gb) {
  CYC(0x479d, 0x47a0); enemyAnimate_hook(gb); return; // jp
}

// Decelerating until [counter1] counts up to $7f, when it stops completely.
void keese_subid00_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x47a0, 0x47a2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x47a2, 0x47a3); A = mem_rd(gb, DE);
  CYC(0x47a3, 0x47a5); alu_cp(gb, 0x68);
  if (!(F & FC)) { CYCT(0x47a5, 0x47a7); goto L_47ad; } // jr nc
  CYC(0x47a5, 0x47a7);
  CALL_C(0x47a7, objectApplySpeed_hook, 0x201d, 0x47aa);
  CALL_C(0x47aa, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x47ad);

L_47ad:
  CALL_C(0x47ad, keese_updateDeceleration_hook, 0x481c, 0x47b0);
  CYC(0x47b0, 0x47b1); H = D;
  CYC(0x47b1, 0x47b3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x47b3, 0x47b4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x47b4, 0x47b6); A = 0x7f;
  CYC(0x47b6, 0x47b7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x47b7); return; } // ret nz
  CYC(0x47b7, 0x47b8);
  CYC(0x47b8, 0x47ba); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x47ba, 0x47bc); mem_wr(gb, HL, 0x08);
  CALL_C(0x47bc, getRandomNumber_noPreserveVars_hook, 0x0453, 0x47bf);
  CYC(0x47bf, 0x47c1); alu_and(gb, 0x7f);
  CYC(0x47c1, 0x47c3); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x47c3, 0x47c5); alu_add(gb, 0x20);
  CYC(0x47c5, 0x47c6); mem_wr(gb, DE, A);
  CYC(0x47c6, 0x47c7); alu_xor(gb, A);
  CYC(0x47c7, 0x47ca); enemySetAnimation_hook(gb); return; // jp
}

void keese_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x47ca, 0x47cb); A = mem_rd(gb, DE);
  CYC(0x47cb, 0x47cd); alu_sub(gb, 0x08);
  {
    CYC(0x47cd, 0x47ce); push_effect(gb, 0x47ce);
    uint16_t target = keese_jump_table(gb);
    if (target == 0x47d2) { keese_subid01_state8_hook(gb); return; }
    if (target == 0x47f7) { keese_subid02_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for Link to approach
void keese_subid01_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x47d2, 0x47d4); C = 0x31;
  CALL_C(0x47d4, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x47d7);
  if (!(F & FC)) { RET_TAKEN(0x47d7); return; } // ret nc
  CYC(0x47d7, 0x47d8);
  CALL_C(0x47d8, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x47db);
  CALL_C(0x47db, ecom_incState_b0e_hook, 0x4000, 0x47de);
  CYC(0x47de, 0x47e0); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x47e0, 0x47e2); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x47e2, 0x47e4); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x47e4, 0x47e6); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x47e6, 0x47e7); A = mem_rd(gb, DE);
  CYC(0x47e7, 0x47e8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x47e8, 0x47ea); alu_and(gb, 0x1f);
  CYC(0x47ea, 0x47eb); mem_wr(gb, DE, A);
  CYC(0x47eb, 0x47ed); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x47ed, 0x47ef); mem_wr(gb, HL, 12);
  CYC(0x47ef, 0x47f0); L = alu_inc8(gb, L);
  CYC(0x47f0, 0x47f2); mem_wr(gb, HL, 12); // [counter2]
  CYC(0x47f2, 0x47f4); A = 0x01;
  CYC(0x47f4, 0x47f7); enemySetAnimation_hook(gb); return; // jp
}

void keese_subid02_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x47f7, objectApplySpeed_hook, 0x201d, 0x47fa);
  CALL_C(0x47fa, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x47fd);
  CALL_C(0x47fd, ecom_decCounter1_b0e_hook, 0x439a, 0x4800);
  if (!(F & FZ)) { CYCT(0x4800, 0x4802); keese_animate_hook(gb); return; } // jr nz
  CYC(0x4800, 0x4802);
  CYC(0x4802, 0x4804); mem_wr(gb, HL, 12); // [counter1]
  CYC(0x4804, 0x4806); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4806, 0x4808); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4808, 0x4809); A = mem_rd(gb, DE);
  CYC(0x4809, 0x480a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x480a, 0x480c); alu_and(gb, 0x1f);
  CYC(0x480c, 0x480d); mem_wr(gb, DE, A);
  CYC(0x480d, 0x480f); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x480f, 0x4810); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x4810, 0x4812); keese_animate_hook(gb); return; } // jr nz
  CYC(0x4810, 0x4812);
  CYC(0x4812, 0x4814); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4814, 0x4815); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CALL_C(0x4815, keese_chooseWhetherToReverseTurningAngle_hook, 0x4861, 0x4818);
  CYC(0x4818, 0x4819); alu_xor(gb, A);
  CYC(0x4819, 0x481c); enemySetAnimation_hook(gb); return; // jp
}

// Every 16 frames (based on counter1) this updates the keese's speed as it's decelerating.
// Also handles the animation (which slows down).
void keese_updateDeceleration_hook(GB *gb) {
  CYC(0x481c, 0x481e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x481e, 0x481f); A = mem_rd(gb, DE);
  CYC(0x481f, 0x4821); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x4821, 0x4823); goto L_482e; } // jr nz
  CYC(0x4821, 0x4823);
  CYC(0x4823, 0x4824); A = mem_rd(gb, DE);
  CYC(0x4824, 0x4826); alu_swap_a(gb);
  CYC(0x4826, 0x4829); SET_HL(0x4841); // @speeds (data)
  CYC(0x4829, 0x482a); keese_addAToHl_from_rst(gb, 0x482a);
  CYC(0x482a, 0x482b); A = mem_rd(gb, HL);
  CYC(0x482b, 0x482d); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x482d, 0x482e); mem_wr(gb, DE, A);

L_482e:
  CYC(0x482e, 0x4830); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4830, 0x4831); A = mem_rd(gb, DE);
  CYC(0x4831, 0x4833); alu_and(gb, 0xf0);
  CYC(0x4833, 0x4835); alu_swap_a(gb);
  CYC(0x4835, 0x4838); SET_HL(0x4849); // @bits (data)
  CYC(0x4838, 0x4839); keese_addAToHl_from_rst(gb, 0x4839);
  CYC(0x4839, 0x483c); A = mem_rd(gb, wFrameCounter);
  CYC(0x483c, 0x483d); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x483d, 0x4840); enemyAnimate_hook(gb); return; } // jp z
  CYC(0x483d, 0x4840);
  RET(0x4840); return; // ret
}

void keese_initializeSubid_hook(GB *gb) {
  CYC(0x4851, 0x4852); B = alu_dec8(gb, B);
  if (F & FZ) { CYCT(0x4852, 0x4854); goto subid1; } // jr z
  CYC(0x4852, 0x4854);

  CYC(0x4854, 0x4856); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4856, 0x4858); mem_wr(gb, HL, 0x20);
  RET(0x4858); return; // ret

subid1:
  CYC(0x4859, 0x485b); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x485b, 0x485d); mem_wr(gb, HL, 0xff);
  CYC(0x485d, 0x485f); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x485f, 0x4861); mem_wr(gb, HL, 0x02);
  keese_chooseWhetherToReverseTurningAngle_hook(gb); return; // fallthrough
}

// For subid 1 only, this has a 1 in 4 chance of deciding to reverse the turning angle
// (clockwise or counterclockwise).
void keese_chooseWhetherToReverseTurningAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4861, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4864);
  CYC(0x4864, 0x4866); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x4866); return; } // ret nz
  CYC(0x4866, 0x4867);
  CYC(0x4867, 0x4869); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4869, 0x486a); A = mem_rd(gb, DE);
  CYC(0x486a, 0x486b); alu_cpl(gb);
  CYC(0x486b, 0x486c); A = alu_inc8(gb, A);
  CYC(0x486c, 0x486d); mem_wr(gb, DE, A);
  RET(0x486d); return; // ret
}
