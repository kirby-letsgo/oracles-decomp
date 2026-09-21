#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void polsVoice_state_uninitialized_hook(GB *gb);
void polsVoice_state_stub_hook(GB *gb);
void polsVoice_state8_hook(GB *gb);
void polsVoice_state9_hook(GB *gb);
void polsVoice_setLandedAnimation_hook(GB *gb);
void polsVoice_checkLinkPlayingInstrument_hook(GB *gb);

static uint16_t polsVoice_jump_table(GB *gb) {
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
// ENEMY_POLS_VOICE
//
// Variables:
//   var30: gravity
// ==================================================================================================
void enemyCode23_hook(GB *gb) {
  BASE(enemyCode23);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazardsNoAnimationForHoles_b0d_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b0d), b_+3);
  CALL_C(b_+3, polsVoice_checkLinkPlayingInstrument_hook, SYM(polsVoice_checkLinkPlayingInstrument), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto normalStatus; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+10); return; } // ret c
  CYC(b_+10, b_+11);
  if (F & FZ) { CYCT(b_+11, b_+14); enemyDie_hook(gb); return; } // jp z
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  RET(b_+18); return; // ret

normalStatus:
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  {
    CYC(b_+22, b_+23); push_effect(gb, b_+23);
    uint16_t target = polsVoice_jump_table(gb);
    if (target == SYM(polsVoice_state_uninitialized)) { polsVoice_state_uninitialized_hook(gb); return; }
    if (target == SYM(polsVoice_state_stub)) { polsVoice_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(polsVoice_state8)) { polsVoice_state8_hook(gb); return; }
    if (target == SYM(polsVoice_state9)) { polsVoice_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5c20, bare global; jump-table target from enemyCode23.
void polsVoice_state_uninitialized_hook(GB *gb) {
  BASE(polsVoice_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  // Note: a is uninitialized entering this call; arbitrary speed.
  CALL_C(b_+0, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CALL_C(b_+3, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); alu_and(gb, 0x3f);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); TAIL(polsVoice_setLandedAnimation); // jr
}

// 0d:5c2e, bare global; jump-table target from enemyCode23.
void polsVoice_state_stub_hook(GB *gb) {
  BASE(polsVoice_state_stub);
  RET(b_+0); return; // ret
}

// 0d:5c2f, bare global; jump-table target from enemyCode23.
void polsVoice_state8_hook(GB *gb) {
  BASE(polsVoice_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+6, b_+9); SET_BC(0x0f1c);
  CALL_C(b_+9, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+12);
  CYC(b_+12, b_+13); alu_or(gb, B);
  CYC(b_+13, b_+16); SET_HL(b_+59); // @jumpSpeeds1
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto readSpeeds; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); SET_HL(b_+63); // @jumpSpeeds2

readSpeeds:
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = ENEMY_BASE + 0x30; // Enemy.var30 (gravity)
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); alu_cp(gb, 0x14); // SPEED_80
  if (F & FZ) { CYCT(b_+38, b_+40); goto haveAngle; } // jr z
  CYC(b_+38, b_+40);
  CALL_C(b_+40, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+43);
  CYC(b_+43, b_+45); alu_add(gb, 0x02);
  CYC(b_+45, b_+47); alu_and(gb, 0x1c);
  CYC(b_+47, b_+48); C = A;

haveAngle:
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+50, b_+51); A = C;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+53); alu_xor(gb, A);
  CALL_C(b_+53, enemySetAnimation_hook, SYM(enemySetAnimation), b_+56);
  CYC(b_+56, b_+59); TAIL(objectSetVisiblec1); // jp
}

// 0d:5c72, bare global; jump-table target from enemyCode23, falls through into
// polsVoice_setLandedAnimation.
void polsVoice_state9_hook(GB *gb) {
  BASE(polsVoice_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + 0x30; // Enemy.var30 (gravity)
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); C = A;
  CALL_C(b_+7, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);

  // Landed
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = 8
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x20);
  TAIL(polsVoice_setLandedAnimation); // fallthrough
}

// 0d:5c85, bare global; called from polsVoice_state_uninitialized, falls into from
// polsVoice_state9.
void polsVoice_setLandedAnimation_hook(GB *gb) {
  BASE(polsVoice_setLandedAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, enemySetAnimation_hook, SYM(enemySetAnimation), b_+5);
  CYC(b_+5, b_+8); TAIL(objectSetVisiblec2); // jp
}

// 0d:5c8d, bare global; called from enemyCode23.
// @param a Enemy status
// @param[out] a Updated enemy status
void polsVoice_checkLinkPlayingInstrument_hook(GB *gb) {
  BASE(polsVoice_checkLinkPlayingInstrument);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto checkStatus; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); B = 0x03; // ENEMYSTATUS_NO_HEALTH

checkStatus:
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+11); alu_or(gb, A);
  RET(b_+11); return; // ret
}
