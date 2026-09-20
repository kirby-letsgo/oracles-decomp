#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  push_effect(gb, return_address);
  CYC(0x6c2a, 0x6c2b); A = C;
  CYC(0x6c2b, 0x6c2c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6c2c, 0x6c2f); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb); return; } // jp z
  CYC(0x6c2c, 0x6c2f);
  CYC(0x6c2f, 0x6c32); ecom_setRandomCardinalAngle_b0d_hook(gb); return; // jp
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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b0b, ecom_checkHazards_b0d_hook, 0x4051, 0x6b0e);
  if (F & FZ) { CYCT(0x6b0e, 0x6b10); goto normalStatus; } // jr z
  CYC(0x6b0e, 0x6b10);
  CYC(0x6b10, 0x6b12); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x6b12, 0x6b13); ret_effect(gb); return; } // ret c
  CYC(0x6b12, 0x6b13);
  if (F & FZ) { CYCT(0x6b13, 0x6b16); enemyDie_hook(gb); return; } // jp z
  CYC(0x6b13, 0x6b16);
  CYC(0x6b16, 0x6b17); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6b17, 0x6b1a); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x6b17, 0x6b1a);

normalStatus:
  CALL_C(0x6b1a, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x6b1d);
  if (F & FC) { CYCT(0x6b1d, 0x6b1f); goto commonState; } // jr c
  CYC(0x6b1d, 0x6b1f);
  CYC(0x6b1f, 0x6b21); alu_bit(gb, 0, B);
  if (F & FZ) { CYCT(0x6b21, 0x6b24); ironMask_subid00_hook(gb); return; } // jp z
  CYC(0x6b21, 0x6b24);
  CYC(0x6b24, 0x6b27); ironMask_subid01_hook(gb); return; // jp

commonState:
  CYC(0x6b27, 0x6b28); push_effect(gb, 0x6b28);
  {
    uint16_t target = enemyCode1c_jump_table(gb);
    if (target == 0x6b54) { ironMask_state_switchHook_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x6b38) { ironMask_state_uninitialized_hook(gb); return; }
    ironMask_state_stub_hook(gb); return; // states 1, 2, 4, 6, 7 all target 0x6bb0
  }
}

void ironMask_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b38, 0x6b3a); A = 0x14; // SPEED_80
  CALL_C(0x6b3a, ecom_setSpeedAndState8AndVisible_b0d_hook, 0x435e, 0x6b3d);
  CYC(0x6b3d, 0x6b3f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6b3f, 0x6b40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6b40, 0x6b42); alu_bit(gb, 0, B);
  if (F & FZ) { CYCT(0x6b42, 0x6b43); ret_effect(gb); return; } // ret z
  CYC(0x6b42, 0x6b43);
  // Subid 1 only
  CYC(0x6b43, 0x6b45); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6b45, 0x6b47); mem_wr(gb, HL, 0x53); // ENEMYCOLLISION_UNMASKED_IRON_MASK
  CYC(0x6b47, 0x6b49); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x6b49, 0x6b4b); mem_wr(gb, HL, 0x10);
  CYC(0x6b4b, 0x6b4d); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x6b4d, 0x6b4f); mem_wr(gb, HL, 0xe8);
  CYC(0x6b4f, 0x6b51); A = 0x04;
  CYC(0x6b51, 0x6b54); enemySetAnimation_hook(gb); return; // jp
}

void ironMask_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b54, 0x6b55); E = alu_inc8(gb, E);
  CYC(0x6b55, 0x6b56); A = mem_rd(gb, DE);
  CYC(0x6b56, 0x6b57); push_effect(gb, 0x6b57);
  {
    uint16_t target = enemyCode1c_jump_table(gb);
    if (target == 0x6b97) goto substate3;
    if (target == 0x6b96) { RET(0x6b96); return; } // @substate1/@substate2, a bare `ret`
    goto substate0; // target == 0x6b5f
  }

substate0:
  // Using switch hook may cause this enemy's mask to be removed.
  CYC(0x6b5f, 0x6b61); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6b61, 0x6b62); A = mem_rd(gb, DE);
  CYC(0x6b62, 0x6b63); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6b63, 0x6b65); goto dontRemoveMask; } // jr nz
  CYC(0x6b63, 0x6b65);
  CYC(0x6b65, 0x6b67); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6b67, 0x6b68); A = mem_rd(gb, DE);
  CYC(0x6b68, 0x6b6a); alu_cp(gb, 0x53); // ENEMYCOLLISION_UNMASKED_IRON_MASK
  if (F & FZ) { CYCT(0x6b6a, 0x6b6c); goto dontRemoveMask; } // jr z
  CYC(0x6b6a, 0x6b6c);
  CYC(0x6b6c, 0x6b6e); B = 0x1c; // ENEMY_IRON_MASK
  CALL_C(0x6b6e, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, 0x436d, 0x6b71);
  if (!(F & FZ)) { CYCT(0x6b71, 0x6b73); goto dontRemoveMask; } // jr nz
  CYC(0x6b71, 0x6b73);
  // Transfer "index" from enabled byte to new enemy
  CYC(0x6b73, 0x6b75); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x6b75, 0x6b76); E = L;
  CYC(0x6b76, 0x6b77); A = mem_rd(gb, DE);
  CYC(0x6b77, 0x6b78); mem_wr(gb, HL, A);
  CYC(0x6b78, 0x6b7a); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x6b7a, 0x6b7b); E = L;
  CYC(0x6b7b, 0x6b7c); A = mem_rd(gb, DE);
  CYC(0x6b7c, 0x6b7d); mem_wr(gb, HL, A);
  CALL_C(0x6b7d, objectCopyPosition_hook, 0x2242, 0x6b80);
  CYC(0x6b80, 0x6b82); A = 0x05;
  CALL_C(0x6b82, enemySetAnimation_hook, 0x282b, 0x6b85);
  CYC(0x6b85, 0x6b87); A = 0x52; // SND_BOMB_LAND
  CALL_C(0x6b87, playSound_b00_hook, 0x0c98, 0x6b8a);
  CYC(0x6b8a, 0x6b8c); A = 60;
  CYC(0x6b8c, 0x6b8e); goto haveCounterValue; // jr

dontRemoveMask:
  CYC(0x6b8e, 0x6b90); A = 16;

haveCounterValue:
  CYC(0x6b90, 0x6b92); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6b92, 0x6b93); mem_wr(gb, DE, A);
  CYC(0x6b93, 0x6b96); ecom_incSubstate_b0d_hook(gb); return; // jp

substate3:
  CYC(0x6b97, 0x6b99); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6b99, 0x6b9a); A = mem_rd(gb, DE);
  CYC(0x6b9a, 0x6b9b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6b9b, 0x6b9e); ecom_fallToGroundAndSetState8_b0d_hook(gb); return; } // jp nz
  CYC(0x6b9b, 0x6b9e);
  CYC(0x6b9e, 0x6ba0); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6ba0, 0x6ba1); A = mem_rd(gb, DE);
  CYC(0x6ba1, 0x6ba3); alu_cp(gb, 0x1d); // ENEMYCOLLISION_IRON_MASK
  if (!(F & FZ)) { CYCT(0x6ba3, 0x6ba6); ecom_fallToGroundAndSetState8_b0d_hook(gb); return; } // jp nz
  CYC(0x6ba3, 0x6ba6);
  CYC(0x6ba6, 0x6ba8); B = 0x0a;
  CALL_C(0x6ba8, ecom_fallToGroundAndSetState_b0d_hook, 0x44e2, 0x6bab);
  CYC(0x6bab, 0x6bad); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6bad, 0x6baf); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(0x6baf); return;
}

void ironMask_state_stub_hook(GB *gb) {
  RET(0x6bb0); return;
}

// Iron mask with mask on
void ironMask_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bb1, 0x6bb2); A = mem_rd(gb, DE);
  CYC(0x6bb2, 0x6bb4); alu_sub(gb, 0x08);
  CYC(0x6bb4, 0x6bb5); push_effect(gb, 0x6bb5);
  {
    uint16_t target = enemyCode1c_jump_table(gb);
    if (target == 0x6bc6) goto state9;
    if (target == 0x6bd9) goto stateA;
    goto state8; // target == 0x6bbb
  }

  // Standing in place
state8:
  CALL_C(0x6bbb, ecom_decCounter1_b0d_hook, 0x439a, 0x6bbe);
  if (!(F & FZ)) { CYCT(0x6bbe, 0x6bc1); ironMask_updateCollisionsFromLinkRelativeAngle_hook(gb); return; } // jp nz
  CYC(0x6bbe, 0x6bc1);
  CYC(0x6bc1, 0x6bc2); L = E;
  CYC(0x6bc2, 0x6bc3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(0x6bc3, ironMask_chooseRandomAngleAndCounter1_hook, 0x6c05, 0x6bc6);
  // falls through to state9

  // Moving in some direction for [counter1] frames
state9:
  CALL_C(0x6bc6, ecom_decCounter1_b0d_hook, 0x439a, 0x6bc9);
  if (!(F & FZ)) { CYCT(0x6bc9, 0x6bcb); goto afterCounter1Check; } // jr nz
  CYC(0x6bc9, 0x6bcb);
  CYC(0x6bcb, 0x6bcc); L = E;
  CYC(0x6bcc, 0x6bcd); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(0x6bcd, ironMask_chooseAmountOfTimeToStand_hook, 0x6c3a, 0x6bd0);

afterCounter1Check:
  CALL_C(0x6bd0, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x6bd3);
  CALL_C(0x6bd3, ironMask_updateCollisionsFromLinkRelativeAngle_hook, 0x6bee, 0x6bd6);
  CYC(0x6bd6, 0x6bd9); enemyAnimate_hook(gb); return; // jp

  // This enemy has turned into the mask that was removed; will delete self after [counter1]
  // frames.
stateA:
  CALL_C(0x6bd9, ecom_decCounter1_b0d_hook, 0x439a, 0x6bdc);
  if (!(F & FZ)) { CYCT(0x6bdc, 0x6bdf); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(0x6bdc, 0x6bdf);
  CYC(0x6bdf, 0x6be2); enemyDelete_hook(gb); return; // jp
}

// Iron mask without mask on
void ironMask_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6be2, ecom_decCounter1_b0d_hook, 0x439a, 0x6be5);
  if (F & FZ) CALL_C_CC(0x6be5, ironMask_chooseRandomAngleAndCounter1_hook, 0x6c05, 0x6be8); else CYC(0x6be5, 0x6be8); // call z
  CALL_C(0x6be8, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x6beb);
  CYC(0x6beb, 0x6bee); enemyAnimate_hook(gb); return; // jp
}

// Modifies this object's enemyCollisionMode based on if Link is directly behind the iron mask
// or not.
void ironMask_updateCollisionsFromLinkRelativeAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6bee, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6bf1);
  CYC(0x6bf1, 0x6bf2); H = D;
  CYC(0x6bf2, 0x6bf4); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6bf4, 0x6bf5); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6bf5, 0x6bf7); alu_and(gb, 0x1f);
  CYC(0x6bf7, 0x6bf9); alu_sub(gb, 0x0c);
  CYC(0x6bf9, 0x6bfb); alu_cp(gb, 0x09);
  CYC(0x6bfb, 0x6bfd); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  if (F & FC) { CYCT(0x6bfd, 0x6bff); goto unmasked; } // jr c
  CYC(0x6bfd, 0x6bff);
  CYC(0x6bff, 0x6c01); mem_wr(gb, HL, 0x1d); // ENEMYCOLLISION_IRON_MASK
  RET(0x6c01); return;

unmasked:
  CYC(0x6c02, 0x6c04); mem_wr(gb, HL, 0x53); // ENEMYCOLLISION_UNMASKED_IRON_MASK
  RET(0x6c04); return;
}

void ironMask_chooseRandomAngleAndCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c05, 0x6c08); SET_BC(0x0703);
  CALL_C(0x6c08, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x6c0b);
  CYC(0x6c0b, 0x6c0c); A = B;
  CYC(0x6c0c, 0x6c0f); SET_HL(0x6c32); // @counter1Vals
  CYC(0x6c0f, 0x6c10); ironMask_addAToHl_from_rst(gb, 0x6c10);
  CYC(0x6c10, 0x6c12); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6c12, 0x6c13); A = mem_rd(gb, HL);
  CYC(0x6c13, 0x6c14); mem_wr(gb, DE, A);
  CYC(0x6c14, 0x6c16); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6c16, 0x6c17); A = mem_rd(gb, DE);
  CYC(0x6c17, 0x6c18); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6c18, 0x6c1b); ecom_setRandomCardinalAngle_b0d_hook(gb); return; } // jp nz
  CYC(0x6c18, 0x6c1b);
  // Subid 0 only: 1 in 4 chance of turning directly toward Link, otherwise just
  // choose a random angle
  CYC(0x6c1b, 0x6c1e); ironMask_chooseAngle(gb, 0x6c1e);
  CYC(0x6c1e, 0x6c20); A = alu_swap(gb, A);
  CYC(0x6c20, 0x6c21); alu_rlca(gb);
  CYC(0x6c21, 0x6c22); H = D;
  CYC(0x6c22, 0x6c24); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6c24, 0x6c25); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6c25, 0x6c26); ret_effect(gb); return; } // ret z
  CYC(0x6c25, 0x6c26);
  CYC(0x6c26, 0x6c27); mem_wr(gb, HL, A);
  CYC(0x6c27, 0x6c2a); enemySetAnimation_hook(gb); return; // jp
}

// ironMask_chooseRandomAngleAndCounter1@counter1Vals (0d:6c32-6c39): pure data
// (25, 30, 35, 40, 45, 50, 55, 60), read only through the addAToHl indexing above.

void ironMask_chooseAmountOfTimeToStand_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c3a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c3d);
  CYC(0x6c3d, 0x6c3f); alu_and(gb, 0x03);
  CYC(0x6c3f, 0x6c42); SET_HL(0x6c48); // @counter1Vals
  CYC(0x6c42, 0x6c43); ironMask_addAToHl_from_rst(gb, 0x6c43);
  CYC(0x6c43, 0x6c45); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6c45, 0x6c46); A = mem_rd(gb, HL);
  CYC(0x6c46, 0x6c47); mem_wr(gb, DE, A);
  RET(0x6c47); return;
}

// ironMask_chooseAmountOfTimeToStand@counter1Vals (0d:6c48-6c4b): pure data (15, 30, 45, 60),
// read only through the addAToHl indexing above.
