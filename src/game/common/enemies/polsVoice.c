#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5bf5, ecom_checkHazardsNoAnimationForHoles_b0d_hook, 0x4043, 0x5bf8);
  CALL_C(0x5bf8, polsVoice_checkLinkPlayingInstrument_hook, 0x5c8d, 0x5bfb);
  if (F & FZ) { CYCT(0x5bfb, 0x5bfd); goto normalStatus; } // jr z
  CYC(0x5bfb, 0x5bfd);
  CYC(0x5bfd, 0x5bff); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5bff); return; } // ret c
  CYC(0x5bff, 0x5c00);
  if (F & FZ) { CYCT(0x5c00, 0x5c03); enemyDie_hook(gb); return; } // jp z
  CYC(0x5c00, 0x5c03);
  CYC(0x5c03, 0x5c04); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5c04, 0x5c07); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x5c04, 0x5c07);
  RET(0x5c07); return; // ret

normalStatus:
  CYC(0x5c08, 0x5c0a); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5c0a, 0x5c0b); A = mem_rd(gb, DE);
  {
    CYC(0x5c0b, 0x5c0c); push_effect(gb, 0x5c0c);
    uint16_t target = polsVoice_jump_table(gb);
    if (target == 0x5c20) { polsVoice_state_uninitialized_hook(gb); return; }
    if (target == 0x5c2e) { polsVoice_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x5c2f) { polsVoice_state8_hook(gb); return; }
    if (target == 0x5c72) { polsVoice_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5c20, bare global; jump-table target from enemyCode23.
void polsVoice_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Note: a is uninitialized entering this call; arbitrary speed.
  CALL_C(0x5c20, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x5c23);
  CALL_C(0x5c23, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5c26);
  CYC(0x5c26, 0x5c28); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5c28, 0x5c2a); alu_and(gb, 0x3f);
  CYC(0x5c2a, 0x5c2b); A = alu_inc8(gb, A);
  CYC(0x5c2b, 0x5c2c); mem_wr(gb, DE, A);
  CYC(0x5c2c, 0x5c2e); polsVoice_setLandedAnimation_hook(gb); return; // jr
}

// 0d:5c2e, bare global; jump-table target from enemyCode23.
void polsVoice_state_stub_hook(GB *gb) {
  RET(0x5c2e); return; // ret
}

// 0d:5c2f, bare global; jump-table target from enemyCode23.
void polsVoice_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c2f, ecom_decCounter1_b0d_hook, 0x439a, 0x5c32);
  if (!(F & FZ)) { RET_TAKEN(0x5c32); return; } // ret nz
  CYC(0x5c32, 0x5c33);
  CYC(0x5c33, 0x5c34); L = E;
  CYC(0x5c34, 0x5c35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x5c35, 0x5c38); SET_BC(0x0f1c);
  CALL_C(0x5c38, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5c3b);
  CYC(0x5c3b, 0x5c3c); alu_or(gb, B);
  CYC(0x5c3c, 0x5c3f); SET_HL(0x5c6a); // @jumpSpeeds1
  if (!(F & FZ)) { CYCT(0x5c3f, 0x5c41); goto readSpeeds; } // jr nz
  CYC(0x5c3f, 0x5c41);
  CYC(0x5c41, 0x5c44); SET_HL(0x5c6e); // @jumpSpeeds2

readSpeeds:
  CYC(0x5c44, 0x5c46); E = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5c46, 0x5c47); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x5c47, 0x5c48); mem_wr(gb, DE, A);
  CYC(0x5c48, 0x5c49); E = alu_inc8(gb, E);
  CYC(0x5c49, 0x5c4a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x5c4a, 0x5c4b); mem_wr(gb, DE, A);
  CYC(0x5c4b, 0x5c4d); E = ENEMY_BASE + 0x30; // Enemy.var30 (gravity)
  CYC(0x5c4d, 0x5c4e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x5c4e, 0x5c4f); mem_wr(gb, DE, A);
  CYC(0x5c4f, 0x5c51); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5c51, 0x5c52); A = mem_rd(gb, HL);
  CYC(0x5c52, 0x5c53); mem_wr(gb, DE, A);
  CYC(0x5c53, 0x5c55); alu_cp(gb, 0x14); // SPEED_80
  if (F & FZ) { CYCT(0x5c55, 0x5c57); goto haveAngle; } // jr z
  CYC(0x5c55, 0x5c57);
  CALL_C(0x5c57, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5c5a);
  CYC(0x5c5a, 0x5c5c); alu_add(gb, 0x02);
  CYC(0x5c5c, 0x5c5e); alu_and(gb, 0x1c);
  CYC(0x5c5e, 0x5c5f); C = A;

haveAngle:
  CYC(0x5c5f, 0x5c61); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5c61, 0x5c62); A = C;
  CYC(0x5c62, 0x5c63); mem_wr(gb, DE, A);
  CYC(0x5c63, 0x5c64); alu_xor(gb, A);
  CALL_C(0x5c64, enemySetAnimation_hook, 0x282b, 0x5c67);
  CYC(0x5c67, 0x5c6a); objectSetVisiblec1_hook(gb); return; // jp
}

// 0d:5c72, bare global; jump-table target from enemyCode23, falls through into
// polsVoice_setLandedAnimation.
void polsVoice_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c72, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x5c75);
  CYC(0x5c75, 0x5c77); E = ENEMY_BASE + 0x30; // Enemy.var30 (gravity)
  CYC(0x5c77, 0x5c78); A = mem_rd(gb, DE);
  CYC(0x5c78, 0x5c79); C = A;
  CALL_C(0x5c79, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5c7c);
  if (!(F & FZ)) { RET_TAKEN(0x5c7c); return; } // ret nz
  CYC(0x5c7c, 0x5c7d);

  // Landed
  CYC(0x5c7d, 0x5c7e); H = D;
  CYC(0x5c7e, 0x5c80); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5c80, 0x5c81); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = 8
  CYC(0x5c81, 0x5c83); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5c83, 0x5c85); mem_wr(gb, HL, 0x20);
  polsVoice_setLandedAnimation_hook(gb); return; // fallthrough
}

// 0d:5c85, bare global; called from polsVoice_state_uninitialized, falls into from
// polsVoice_state9.
void polsVoice_setLandedAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c85, 0x5c87); A = 0x01;
  CALL_C(0x5c87, enemySetAnimation_hook, 0x282b, 0x5c8a);
  CYC(0x5c8a, 0x5c8d); objectSetVisiblec2_hook(gb); return; // jp
}

// 0d:5c8d, bare global; called from enemyCode23.
// @param a Enemy status
// @param[out] a Updated enemy status
void polsVoice_checkLinkPlayingInstrument_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c8d, 0x5c8e); B = A;
  CYC(0x5c8e, 0x5c91); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(0x5c91, 0x5c92); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5c92, 0x5c94); goto checkStatus; } // jr z
  CYC(0x5c92, 0x5c94);
  CYC(0x5c94, 0x5c96); B = 0x03; // ENEMYSTATUS_NO_HEALTH

checkStatus:
  CYC(0x5c96, 0x5c97); A = B;
  CYC(0x5c97, 0x5c98); alu_or(gb, A);
  RET(0x5c98); return; // ret
}
