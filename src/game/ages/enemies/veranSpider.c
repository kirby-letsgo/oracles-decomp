#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode0f_jump_table(GB *gb) {
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

void veranSpider_state_uninitialized_hook(GB *gb);
void veranSpider_state_switchHook_hook(GB *gb);
void veranSpider_state_scentSeed_hook(GB *gb);
void veranSpider_updateAnimation_hook(GB *gb);
void veranSpider_gotoState9_hook(GB *gb);
void veranSpider_state_stub_hook(GB *gb);
void veranSpider_state8_hook(GB *gb);
void veranSpider_state9_hook(GB *gb);
void veranSpider_animate_hook(GB *gb);
void veranSpider_stateA_hook(GB *gb);
void veranSpider_setRandomAngleAndCounter1_hook(GB *gb);

// ==================================================================================================
// ENEMY_VERAN_SPIDER
// ==================================================================================================
void enemyCode0f_hook(GB *gb) {
  BASE(enemyCode0f);
  uint16_t sp0_ = gb->sp;
  // Kill spiders when a cutscene trigger occurs
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+4, b_+5); alu_or(gb, A);
  CYC(b_+5, b_+6); A = B;
  if (F & FZ) { CYCT(b_+6, b_+8); goto afterStatusOverride; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = 0x03; // ENEMYSTATUS_NO_HEALTH

afterStatusOverride:
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto normalStatus; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+15, b_+16); ret_effect(gb); return; } // ret c
  CYC(b_+15, b_+16);
  if (F & FZ) { CYCT(b_+16, b_+19); enemyDie_hook(gb); return; } // jp z
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(b_+20, b_+23);
  RET(b_+23); return;

normalStatus:
  CALL_C(b_+24, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+27);
  if (F & FZ) { CYCT(b_+27, b_+29); goto afterSpeedOverride; } // jr z
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+31, b_+33); A = 0x32; // SPEED_140
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);

afterSpeedOverride:
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); push_effect(gb, b_+38);
  {
    uint16_t target = enemyCode0f_jump_table(gb);
    if (target == SYM(veranSpider_state_switchHook)) { veranSpider_state_switchHook_hook(gb); return; }
    if (target == SYM(veranSpider_state_scentSeed)) { veranSpider_state_scentSeed_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(veranSpider_state8)) { veranSpider_state8_hook(gb); return; }
    if (target == SYM(veranSpider_state9)) { veranSpider_state9_hook(gb); return; }
    if (target == SYM(veranSpider_stateA)) { veranSpider_stateA_hook(gb); return; }
    if (target == SYM(veranSpider_state_uninitialized)) { veranSpider_state_uninitialized_hook(gb); return; }
    veranSpider_state_stub_hook(gb); return; // states 1, 2, 6, 7 all target 0x68ad
  }
}

void veranSpider_state_uninitialized_hook(GB *gb) {
  BASE(veranSpider_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x8a; // PALH_8a
  CALL_C(b_+2, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+5);

  // Choose a random position roughly within the current screen bounds to spawn the spider at.
retryPosition:
  CALL_C(b_+5, getRandomNumber_hook, SYM(getRandomNumber), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x7f);
  CYC(b_+10, b_+12); alu_cp(gb, 0x7a); // $70 + SCREEN_WIDTH
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto retryPosition; } // jr nc
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); C = A;
  CALL_C(b_+15, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+18);
  // Adjust position to be relative to screen bounds
  CYC(b_+18, b_+20); A = mem_rd(gb, hCameraX); // hCameraX
  CYC(b_+20, b_+21); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+24); L = alu_dec8(gb, L);
  CYC(b_+24, b_+26); A = mem_rd(gb, hCameraY); // hCameraY
  CYC(b_+26, b_+27); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+29); B = A;
  // If solid at this position, try again next frame.
  CALL_C(b_+29, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+33); ret_effect(gb); return; } // ret nz
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); C = 0x08;
  CALL_C(b_+35, ecom_setZAboveScreen_b0d_hook, SYM(ecom_setZAboveScreen_b0d), b_+38);
  CYC(b_+38, b_+40); A = 0x0f; // SPEED_60
  CALL_C(b_+40, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+43);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+47, b_+49); A = 0x59; // SND_FALLINHOLE
  CALL_C(b_+49, playSound_b00_hook, SYM(playSound_b00), b_+52);
  CYC(b_+52, b_+55); objectSetVisiblec1_hook(gb); return; // jp
}

void veranSpider_state_switchHook_hook(GB *gb) {
  BASE(veranSpider_state_switchHook);
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = enemyCode0f_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+12) {
      CYC(b_+12, b_+14); B = 0x09;
      CYC(b_+14, b_+17); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp
    }
    RET(b_+11); return; // @substate1/@substate2 both target 0x687b, a bare `ret`
  }
}

void veranSpider_state_scentSeed_hook(GB *gb) {
  BASE(veranSpider_state_scentSeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); veranSpider_gotoState9_hook(gb); return; } // jr z
  CYC(b_+4, b_+6);
  CALL_C(b_+6, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_and(gb, 0x18);
  CYC(b_+14, b_+16); alu_add(gb, 0x04);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), SYM(veranSpider_updateAnimation));
  veranSpider_updateAnimation_hook(gb); return; // falls through
}

void veranSpider_updateAnimation_hook(GB *gb) {
  BASE(veranSpider_updateAnimation);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+6, b_+8); goto incAndSet; } // jr nc
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);

incAndSet:
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+14); enemyAnimate_hook(gb); return; // jp
}

void veranSpider_gotoState9_hook(GB *gb) {
  BASE(veranSpider_gotoState9);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x0f); // SPEED_60
  RET(b_+9); return;
}

void veranSpider_state_stub_hook(GB *gb) {
  BASE(veranSpider_state_stub);
  RET(b_+0); return;
}

// Falling from sky
void veranSpider_state8_hook(GB *gb) {
  BASE(veranSpider_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x0e;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret nz
  CYC(b_+5, b_+6);
  // Landed on ground
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  // Enable scent seeds
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4)));
  CALL_C(b_+17, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+20);
  CYC(b_+20, b_+22); A = 0x52; // SND_BOMB_LAND
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CALL_C(b_+25, veranSpider_setRandomAngleAndCounter1_hook, SYM(veranSpider_setRandomAngleAndCounter1), b_+28);
  CYC(b_+28, b_+30); veranSpider_animate_hook(gb); return; // jr
}

// Moving in some direction for [counter1] frames
void veranSpider_state9_hook(GB *gb) {
  BASE(veranSpider_state9);
  uint16_t sp0_ = gb->sp;
  // Check if Link is along a diagonal relative to self?
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+7); alu_sub(gb, 0x04);
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  CYC(b_+8, b_+10); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto moveNormally; } // jr nc
  CYC(b_+10, b_+12);
  // He is on a diagonal; if counter2 is zero, go to state $0a (charge at Link).
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto moveNormally; } // jr nz
  CYC(b_+16, b_+18);
  CALL_C(b_+18, ecom_updateAngleTowardTarget_b0d_hook, SYM(ecom_updateAngleTowardTarget_b0d), b_+21);
  CYC(b_+21, b_+23); alu_and(gb, 0x18);
  CYC(b_+23, b_+25); alu_add(gb, 0x04);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CALL_C(b_+26, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+29);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 120);
  RET(b_+37); return;

moveNormally:
  CALL_C(b_+38, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+41);
  CYC(b_+41, b_+42); L = alu_dec8(gb, L);
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter1]--
  if (!(F & FZ)) CALL_C_CC(b_+43, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+46); else CYC(b_+43, b_+46); // call nz
  if (F & FZ) { CYCT(b_+46, b_+49); veranSpider_setRandomAngleAndCounter1_hook(gb); return; } // jp z
  CYC(b_+46, b_+49);
  veranSpider_animate_hook(gb); return; // falls through
}

void veranSpider_animate_hook(GB *gb) {
  BASE(veranSpider_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Charging in some direction for [counter1] frames
void veranSpider_stateA_hook(GB *gb) {
  BASE(veranSpider_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto afterCharge; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); veranSpider_updateAnimation_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);

afterCharge:
  CALL_C(b_+11, veranSpider_gotoState9_hook, SYM(veranSpider_gotoState9), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x40);
  veranSpider_setRandomAngleAndCounter1_hook(gb); return; // falls through
}

void veranSpider_setRandomAngleAndCounter1_hook(GB *gb) {
  BASE(veranSpider_setRandomAngleAndCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x1870);
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+11); alu_add(gb, 0x04);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+15); A = C;
  CYC(b_+15, b_+17); alu_add(gb, 0x70);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return;
}
