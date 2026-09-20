#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode42);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto commonState; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  RET(b_+11); return; // ret

dead:
  CYC(b_+12, b_+14); A = 0x30; // Object.var30
  CALL_C(b_+14, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+18, b_+21); enemyDie_uncounted_hook(gb); return; // jp

commonState:
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  {
    CYC(b_+24, b_+25); push_effect(gb, b_+25);
    uint16_t target = shadowHagBug_jump_table(gb);
    if (target == SYM(shadowHagBug_state_uninitialized)) { shadowHagBug_state_uninitialized_hook(gb); return; }
    if (target == SYM(shadowHagBug_state_stub)) { shadowHagBug_state_stub_hook(gb); return; }
    if (target == SYM(shadowHagBug_state_galeSeed)) { shadowHagBug_state_galeSeed_hook(gb); return; }
    if (target == SYM(shadowHagBug_state8)) { shadowHagBug_state8_hook(gb); return; }
    if (target == SYM(shadowHagBug_state9)) { shadowHagBug_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6eb9, bare global; jump-table target from enemyCode42.
void shadowHagBug_state_uninitialized_hook(GB *gb) {
  BASE(shadowHagBug_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0f; // SPEED_60
  CALL_C(b_+2, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+7, b_+9); A = 0x20; // <(-$e0)
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xff); // >(-$e0)
  CALL_C(b_+12, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x1f);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); objectSetVisible82_hook(gb); return; // jp
}

// 0e:6ed0, bare global; jump-table target from enemyCode42.
void shadowHagBug_state_galeSeed_hook(GB *gb) {
  BASE(shadowHagBug_state_galeSeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_galeSeedEffect_b0e_hook, SYM(ecom_galeSeedEffect_b0e), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); enemyDelete_hook(gb); return; // jp
}

// 0e:6ed7, bare global; jump-table target from enemyCode42.
void shadowHagBug_state_stub_hook(GB *gb) {
  BASE(shadowHagBug_state_stub);
  RET(b_+0); return; // ret
}

// 0e:6ed8, bare global; jump-table target from enemyCode42. Falls through into
// shadowHagBug_state9.
void shadowHagBug_state8_hook(GB *gb) {
  BASE(shadowHagBug_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x12;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); shadowHagBug_applySpeedAndAnimate_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+10, getRandomNumber_hook, SYM(getRandomNumber), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xb4); // [counter2] = 180
  shadowHagBug_state9_hook(gb); return; // fallthrough
}

// 0e:6eea, bare global; jump-table target from enemyCode42, also falls into from
// shadowHagBug_state8.
void shadowHagBug_state9_hook(GB *gb) {
  BASE(shadowHagBug_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); shadowHagBug_delete_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+6, b_+8); alu_cp(gb, 0x1e); // 30
  if (F & FC) CALL_C_CC(b_+8, ecom_flickerVisibility_b0e_hook, SYM(ecom_flickerVisibility_b0e), b_+11); else CYC(b_+8, b_+11); // call c
  CYC(b_+11, b_+12); L = alu_dec8(gb, L);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter1]
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); shadowHagBug_applySpeedAndAnimate_hook(gb); return; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); SET_BC(0x0f0f);
  CALL_C(b_+21, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+24);
  CYC(b_+24, b_+26); A = hram_rd(gb, (uint8_t)hEnemyTargetY);
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+29); alu_sub(gb, 0x08);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+32); A = hram_rd(gb, (uint8_t)hEnemyTargetX);
  CYC(b_+32, b_+33); alu_add(gb, C);
  CYC(b_+33, b_+35); alu_sub(gb, 0x08);
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); hram_wr(gb, (uint8_t)hFF8F, A);
  CYC(b_+41, b_+43); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); hram_wr(gb, (uint8_t)hFF8E, A);
  CALL_C(b_+46, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+49);
  CALL_C(b_+49, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), SYM(shadowHagBug_applySpeedAndAnimate));
  shadowHagBug_applySpeedAndAnimate_hook(gb); return; // fallthrough
}

// 0e:6f1e, bare global; falls into from shadowHagBug_state9, also reached by genuine jr from
// shadowHagBug_state8 and shadowHagBug_state9.
void shadowHagBug_applySpeedAndAnimate_hook(GB *gb) {
  BASE(shadowHagBug_applySpeedAndAnimate);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+6); enemyAnimate_hook(gb); return; // jp
}

// 0e:6f24, bare global; called from shadowHagBug_state9.
void shadowHagBug_delete_hook(GB *gb) {
  BASE(shadowHagBug_delete);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x30; // Object.var30
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+6, b_+9); enemyDelete_hook(gb); return; // jp
}
