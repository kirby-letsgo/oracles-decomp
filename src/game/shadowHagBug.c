#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void shadowHagBug_state_uninitialized_hook(GB *gb);
void shadowHagBug_state_galeSeed_hook(GB *gb);
void shadowHagBug_state_stub_hook(GB *gb);
void shadowHagBug_state8_hook(GB *gb);
void shadowHagBug_state9_hook(GB *gb);
void shadowHagBug_applySpeedAndAnimate_hook(GB *gb);
void shadowHagBug_delete_hook(GB *gb);

static uint16_t shadowHagBug_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_SHADOW_HAG_BUG
//
// Variables:
//   counter2: Lifetime counter
// ==================================================================================================
void enemyCode42_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6e8c, 0x6e8e); goto commonState; } // jr z
  CYC(0x6e8c, 0x6e8e);
  CYC(0x6e8e, 0x6e90); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6e90); return; } // ret c
  CYC(0x6e90, 0x6e91);
  if (F & FZ) { CYCT(0x6e91, 0x6e93); goto dead; } // jr z
  CYC(0x6e91, 0x6e93);
  CYC(0x6e93, 0x6e94); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6e94, 0x6e97); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(0x6e94, 0x6e97);
  RET(0x6e97); return; // ret

dead:
  CYC(0x6e98, 0x6e9a); A = 0x30; // Object.var30
  CALL_C(0x6e9a, objectGetRelatedObject1Var_hook, 0x2160, 0x6e9d);
  CYC(0x6e9d, 0x6e9e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6e9e, 0x6ea1); enemyDie_uncounted_hook(gb); return; // jp

commonState:
  CYC(0x6ea1, 0x6ea3); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6ea3, 0x6ea4); A = mem_rd(gb, DE);
  {
    CYC(0x6ea4, 0x6ea5); push_effect(gb, 0x6ea5);
    uint16_t target = shadowHagBug_jump_table(gb);
    if (target == 0x6eb9) { shadowHagBug_state_uninitialized_hook(gb); return; }
    if (target == 0x6ed7) { shadowHagBug_state_stub_hook(gb); return; }
    if (target == 0x6ed0) { shadowHagBug_state_galeSeed_hook(gb); return; }
    if (target == 0x6ed8) { shadowHagBug_state8_hook(gb); return; }
    if (target == 0x6eea) { shadowHagBug_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6eb9, bare global; jump-table target from enemyCode42.
void shadowHagBug_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6eb9, 0x6ebb); A = 0x0f; // SPEED_60
  CALL_C(0x6ebb, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x6ebe);
  CYC(0x6ebe, 0x6ec0); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6ec0, 0x6ec2); A = 0x20; // <(-$e0)
  CYC(0x6ec2, 0x6ec3); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6ec3, 0x6ec5); mem_wr(gb, HL, 0xff); // >(-$e0)
  CALL_C(0x6ec5, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6ec8);
  CYC(0x6ec8, 0x6eca); alu_and(gb, 0x1f);
  CYC(0x6eca, 0x6ecc); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6ecc, 0x6ecd); mem_wr(gb, DE, A);
  CYC(0x6ecd, 0x6ed0); objectSetVisible82_hook(gb); return; // jp
}

// 0e:6ed0, bare global; jump-table target from enemyCode42.
void shadowHagBug_state_galeSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6ed0, ecom_galeSeedEffect_b0e_hook, 0x447b, 0x6ed3);
  if (F & FC) { RET_TAKEN(0x6ed3); return; } // ret c
  CYC(0x6ed3, 0x6ed4);
  CYC(0x6ed4, 0x6ed7); enemyDelete_hook(gb); return; // jp
}

// 0e:6ed7, bare global; jump-table target from enemyCode42.
void shadowHagBug_state_stub_hook(GB *gb) {
  RET(0x6ed7); return; // ret
}

// 0e:6ed8, bare global; jump-table target from enemyCode42. Falls through into
// shadowHagBug_state9.
void shadowHagBug_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ed8, 0x6eda); C = 0x12;
  CALL_C(0x6eda, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6edd);
  if (!(F & FZ)) { CYCT(0x6edd, 0x6edf); shadowHagBug_applySpeedAndAnimate_hook(gb); return; } // jr nz
  CYC(0x6edd, 0x6edf);
  CYC(0x6edf, 0x6ee1); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6ee1, 0x6ee2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x6ee2, getRandomNumber_hook, 0x043e, 0x6ee5);
  CYC(0x6ee5, 0x6ee7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6ee7, 0x6ee8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6ee8, 0x6eea); mem_wr(gb, HL, 0xb4); // [counter2] = 180
  shadowHagBug_state9_hook(gb); return; // fallthrough
}

// 0e:6eea, bare global; jump-table target from enemyCode42, also falls into from
// shadowHagBug_state8.
void shadowHagBug_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6eea, ecom_decCounter2_b0e_hook, 0x43a3, 0x6eed);
  if (F & FZ) { CYCT(0x6eed, 0x6eef); shadowHagBug_delete_hook(gb); return; } // jr z
  CYC(0x6eed, 0x6eef);
  CYC(0x6eef, 0x6ef0); A = mem_rd(gb, HL); // [counter1]
  CYC(0x6ef0, 0x6ef2); alu_cp(gb, 0x1e); // 30
  if (F & FC) CALL_C_CC(0x6ef2, ecom_flickerVisibility_b0e_hook, 0x441f, 0x6ef5); else CYC(0x6ef2, 0x6ef5); // call c
  CYC(0x6ef5, 0x6ef6); L = alu_dec8(gb, L);
  CYC(0x6ef6, 0x6ef7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter1]
  CYC(0x6ef7, 0x6ef8); A = mem_rd(gb, HL);
  CYC(0x6ef8, 0x6efa); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6efa, 0x6efc); shadowHagBug_applySpeedAndAnimate_hook(gb); return; } // jr nz
  CYC(0x6efa, 0x6efc);
  CYC(0x6efc, 0x6eff); SET_BC(0x0f0f);
  CALL_C(0x6eff, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x6f02);
  CYC(0x6f02, 0x6f04); A = hram_rd(gb, (uint8_t)hEnemyTargetY);
  CYC(0x6f04, 0x6f05); alu_add(gb, B);
  CYC(0x6f05, 0x6f07); alu_sub(gb, 0x08);
  CYC(0x6f07, 0x6f08); B = A;
  CYC(0x6f08, 0x6f0a); A = hram_rd(gb, (uint8_t)hEnemyTargetX);
  CYC(0x6f0a, 0x6f0b); alu_add(gb, C);
  CYC(0x6f0b, 0x6f0d); alu_sub(gb, 0x08);
  CYC(0x6f0d, 0x6f0e); C = A;
  CYC(0x6f0e, 0x6f10); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6f10, 0x6f11); A = mem_rd(gb, DE);
  CYC(0x6f11, 0x6f13); hram_wr(gb, (uint8_t)hFF8F, A);
  CYC(0x6f13, 0x6f15); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6f15, 0x6f16); A = mem_rd(gb, DE);
  CYC(0x6f16, 0x6f18); hram_wr(gb, (uint8_t)hFF8E, A);
  CALL_C(0x6f18, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x6f1b);
  CALL_C(0x6f1b, objectNudgeAngleTowards_hook, 0x1fd4, 0x6f1e);
  shadowHagBug_applySpeedAndAnimate_hook(gb); return; // fallthrough
}

// 0e:6f1e, bare global; falls into from shadowHagBug_state9, also reached by genuine jr from
// shadowHagBug_state8 and shadowHagBug_state9.
void shadowHagBug_applySpeedAndAnimate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f1e, objectApplySpeed_hook, 0x201d, 0x6f21);
  CYC(0x6f21, 0x6f24); enemyAnimate_hook(gb); return; // jp
}

// 0e:6f24, bare global; called from shadowHagBug_state9.
void shadowHagBug_delete_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f24, 0x6f26); A = 0x30; // Object.var30
  CALL_C(0x6f26, objectGetRelatedObject1Var_hook, 0x2160, 0x6f29);
  CYC(0x6f29, 0x6f2a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6f2a, 0x6f2d); enemyDelete_hook(gb); return; // jp
}
