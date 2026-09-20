#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode1c_jump_table(GB *gb) {
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

static void ironMask_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ironMask_chooseRandomAngleAndCounter1@chooseAngle: an @-local reached only by one genuine
// `call`, from ironMask_chooseRandomAngleAndCounter1 below. It never executes its own `ret` --
// it tail-jumps into one of two already-hooked leaves -- so no pc/sp resume check is needed
// (same trampoline shape as interactionCode8a_checkEssenceObtained).
static void ironMask_chooseAngle(GB *gb, uint16_t return_address) {
  BASE(ironMask_chooseRandomAngleAndCounter1);
  push_effect(gb, return_address);
  CYC(b_+37, b_+38); A = C;
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+39, b_+42); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb); return; } // jp z
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+45); ecom_setRandomCardinalAngle_b0d_hook(gb); return; // jp
}

void ironMask_state_uninitialized_hook(GB *gb);
void ironMask_state_switchHook_hook(GB *gb);
void ironMask_state_stub_hook(GB *gb);
void ironMask_subid00_hook(GB *gb);
void ironMask_subid01_hook(GB *gb);
void ironMask_updateCollisionsFromLinkRelativeAngle_hook(GB *gb);
void ironMask_chooseRandomAngleAndCounter1_hook(GB *gb);
void ironMask_chooseAmountOfTimeToStand_hook(GB *gb);

// ==================================================================================================
// ENEMY_IRON_MASK
// ==================================================================================================
void enemyCode1c_hook(GB *gb) {
  BASE(enemyCode1c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+7, b_+8); ret_effect(gb); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);

normalStatus:
  CALL_C(b_+15, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+18);
  if (F & FC) { CYCT(b_+18, b_+20); goto commonState; } // jr c
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_bit(gb, 0, B);
  if (F & FZ) { CYCT(b_+22, b_+25); ironMask_subid00_hook(gb); return; } // jp z
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+28); ironMask_subid01_hook(gb); return; // jp

commonState:
  CYC(b_+28, b_+29); push_effect(gb, b_+29);
  {
    uint16_t target = enemyCode1c_jump_table(gb);
    if (target == SYM(ironMask_state_switchHook)) { ironMask_state_switchHook_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(ironMask_state_uninitialized)) { ironMask_state_uninitialized_hook(gb); return; }
    ironMask_state_stub_hook(gb); return; // states 1, 2, 4, 6, 7 all target 0x6bb0
  }
}

void ironMask_state_uninitialized_hook(GB *gb) {
  BASE(ironMask_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x14; // SPEED_80
  CALL_C(b_+2, ecom_setSpeedAndState8AndVisible_b0d_hook, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); alu_bit(gb, 0, B);
  if (F & FZ) { CYCT(b_+10, b_+11); ret_effect(gb); return; } // ret z
  CYC(b_+10, b_+11);
  // Subid 1 only
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x53); // ENEMYCOLLISION_UNMASKED_IRON_MASK
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x10);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xe8);
  CYC(b_+23, b_+25); A = 0x04;
  CYC(b_+25, b_+28); enemySetAnimation_hook(gb); return; // jp
}

void ironMask_state_switchHook_hook(GB *gb) {
  BASE(ironMask_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = enemyCode1c_jump_table(gb);
    if (target == b_+67) goto substate3;
    if (target == b_+66) { RET(b_+66); return; } // @substate1/@substate2, a bare `ret`
    goto substate0; // target == 0x6b5f
  }

substate0:
  // Using switch hook may cause this enemy's mask to be removed.
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto dontRemoveMask; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_cp(gb, 0x53); // ENEMYCOLLISION_UNMASKED_IRON_MASK
  if (F & FZ) { CYCT(b_+22, b_+24); goto dontRemoveMask; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); B = 0x1c; // ENEMY_IRON_MASK
  CALL_C(b_+26, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+29);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto dontRemoveMask; } // jr nz
  CYC(b_+29, b_+31);
  // Transfer "index" from enabled byte to new enemy
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+33, b_+34); E = L;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+38, b_+39); E = L;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CALL_C(b_+41, objectCopyPosition_hook, SYM(objectCopyPosition), b_+44);
  CYC(b_+44, b_+46); A = 0x05;
  CALL_C(b_+46, enemySetAnimation_hook, SYM(enemySetAnimation), b_+49);
  CYC(b_+49, b_+51); A = 0x52; // SND_BOMB_LAND
  CALL_C(b_+51, playSound_b00_hook, SYM(playSound_b00), b_+54);
  CYC(b_+54, b_+56); A = 60;
  CYC(b_+56, b_+58); goto haveCounterValue; // jr

dontRemoveMask:
  CYC(b_+58, b_+60); A = 16;

haveCounterValue:
  CYC(b_+60, b_+62); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+66); ecom_incSubstate_b0d_hook(gb); return; // jp

substate3:
  CYC(b_+67, b_+69); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+71, b_+74); ecom_fallToGroundAndSetState8_b0d_hook(gb); return; } // jp nz
  CYC(b_+71, b_+74);
  CYC(b_+74, b_+76); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+79); alu_cp(gb, 0x1d); // ENEMYCOLLISION_IRON_MASK
  if (!(F & FZ)) { CYCT(b_+79, b_+82); ecom_fallToGroundAndSetState8_b0d_hook(gb); return; } // jp nz
  CYC(b_+79, b_+82);
  CYC(b_+82, b_+84); B = 0x0a;
  CALL_C(b_+84, ecom_fallToGroundAndSetState_b0d_hook, SYM(ecom_fallToGroundAndSetState_b0d), b_+87);
  CYC(b_+87, b_+89); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+89, b_+91); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(b_+91); return;
}

void ironMask_state_stub_hook(GB *gb) {
  BASE(ironMask_state_stub);
  RET(b_+0); return;
}

// Iron mask with mask on
void ironMask_subid00_hook(GB *gb) {
  BASE(ironMask_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = enemyCode1c_jump_table(gb);
    if (target == b_+21) goto state9;
    if (target == b_+40) goto stateA;
    goto state8; // target == 0x6bbb
  }

  // Standing in place
state8:
  CALL_C(b_+10, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); ironMask_updateCollisionsFromLinkRelativeAngle_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+17); L = E;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(b_+18, ironMask_chooseRandomAngleAndCounter1_hook, SYM(ironMask_chooseRandomAngleAndCounter1), b_+21);
  // falls through to state9

  // Moving in some direction for [counter1] frames
state9:
  CALL_C(b_+21, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+24);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto afterCounter1Check; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); L = E;
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(b_+28, ironMask_chooseAmountOfTimeToStand_hook, SYM(ironMask_chooseAmountOfTimeToStand), b_+31);

afterCounter1Check:
  CALL_C(b_+31, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+34);
  CALL_C(b_+34, ironMask_updateCollisionsFromLinkRelativeAngle_hook, SYM(ironMask_updateCollisionsFromLinkRelativeAngle), b_+37);
  CYC(b_+37, b_+40); enemyAnimate_hook(gb); return; // jp

  // This enemy has turned into the mask that was removed; will delete self after [counter1]
  // frames.
stateA:
  CALL_C(b_+40, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+43);
  if (!(F & FZ)) { CYCT(b_+43, b_+46); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+49); enemyDelete_hook(gb); return; // jp
}

// Iron mask without mask on
void ironMask_subid01_hook(GB *gb) {
  BASE(ironMask_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) CALL_C_CC(b_+3, ironMask_chooseRandomAngleAndCounter1_hook, SYM(ironMask_chooseRandomAngleAndCounter1), b_+6); else CYC(b_+3, b_+6); // call z
  CALL_C(b_+6, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+9);
  CYC(b_+9, b_+12); enemyAnimate_hook(gb); return; // jp
}

// Modifies this object's enemyCollisionMode based on if Link is directly behind the iron mask
// or not.
void ironMask_updateCollisionsFromLinkRelativeAngle_hook(GB *gb) {
  BASE(ironMask_updateCollisionsFromLinkRelativeAngle);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); alu_and(gb, 0x1f);
  CYC(b_+9, b_+11); alu_sub(gb, 0x0c);
  CYC(b_+11, b_+13); alu_cp(gb, 0x09);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  if (F & FC) { CYCT(b_+15, b_+17); goto unmasked; } // jr c
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, GV(0x1d, 0x1c)); // ENEMYCOLLISION_IRON_MASK
  RET(b_+19); return;

unmasked:
  CYC(b_+20, b_+22); mem_wr(gb, HL, GV(0x53, 0x50)); // ENEMYCOLLISION_UNMASKED_IRON_MASK
  RET(b_+22); return;
}

void ironMask_chooseRandomAngleAndCounter1_hook(GB *gb) {
  BASE(ironMask_chooseRandomAngleAndCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(SYM(_label_00_062));
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+6);
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+10); SET_HL(b_+45); // @counter1Vals
  CYC(b_+10, b_+11); ironMask_addAToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); ecom_setRandomCardinalAngle_b0d_hook(gb); return; } // jp nz
  CYC(b_+19, b_+22);
  // Subid 0 only: 1 in 4 chance of turning directly toward Link, otherwise just
  // choose a random angle
  CYC(b_+22, b_+25); ironMask_chooseAngle(gb, b_+25);
  CYC(b_+25, b_+27); A = alu_swap(gb, A);
  CYC(b_+27, b_+28); alu_rlca(gb);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+31, b_+32); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+32, b_+33); ret_effect(gb); return; } // ret z
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+37); enemySetAnimation_hook(gb); return; // jp
}

// ironMask_chooseRandomAngleAndCounter1@counter1Vals (0d:6c32-6c39): pure data
// (25, 30, 35, 40, 45, 50, 55, 60), read only through the addAToHl indexing above.

void ironMask_chooseAmountOfTimeToStand_hook(GB *gb) {
  BASE(ironMask_chooseAmountOfTimeToStand);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+14); // @counter1Vals
  CYC(b_+8, b_+9); ironMask_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

// ironMask_chooseAmountOfTimeToStand@counter1Vals (0d:6c48-6c4b): pure data (15, 30, 45, 60),
// read only through the addAToHl indexing above.
