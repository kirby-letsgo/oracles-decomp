#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/enemies/commonCode.s, bank $10 instance (Twinrova/Ganon/Veran's Final
// Form/Ramrock/King Moblin's minion). Included at the start of every enemy code bank; the same
// source produces separate hooks per bank (_b0d.._b10) because each bank has its own ROM bytes.

void ecom_splashOrLavaTail_b10_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), SYM(ecom_decNumEnemiesAndDelete_b10));
  TAIL(ecom_decNumEnemiesAndDelete_b10);
}

void ecom_incState_b10_hook(GB *gb) {
  BASE(ecom_incState_b10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_incSubstate_b10_hook(GB *gb) {
  BASE(ecom_incSubstate_b10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// Update knockback where solid tiles are defined "normally".
void ecom_updateKnockback_b10_hook(GB *gb) {
  BASE(ecom_updateKnockback_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+3, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10), SYM(ecom_updateKnockback_common_b10));
  TAIL(ecom_updateKnockback_common_b10);
}

void ecom_updateKnockback_common_b10_hook(GB *gb) {
  BASE(ecom_updateKnockback_common_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, DE); C = A;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+8); B = 0x50; // SPEED_200
  if (F & FC) {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); B = 0x78; // SPEED_300
    CYC(b_+12, b_+14); alu_and(gb, 0x06);
    if (F & FZ) {
      CYC(b_+14, b_+16);
      PUSH(b_+16, BC);
      CYC(b_+17, b_+20); SET_BC(0x0f01);
      CALL_C(b_+20, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+23);
      SET_BC(POP(b_+23));
    } else {
      CYCT(b_+14, b_+16);
    }
  } else {
    CYCT(b_+8, b_+10);
  }
  CALL_C(b_+24, ecom_applyGivenVelocityGivenAdjacentWalls_b10_hook, SYM(ecom_applyGivenVelocityGivenAdjacentWalls_b10), b_+27);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_and(gb, 0x80);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

// Update knockback where the enemy can pass through anything except the screen boundary.
void ecom_updateKnockbackNoSolidity_b10_hook(GB *gb) {
  BASE(ecom_updateKnockbackNoSolidity_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+4, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10), b_+7);
  CYC(b_+7, b_+9);
  TAIL(ecom_updateKnockback_common_b10);
}

void ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b10_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b10_hook, SYM(ecom_updateKnockback_b10), b_+3);
  CALL_C(b_+3, ecom_checkHazardsNoAnimationForHoles_b10_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b10), b_+6);
  RET(b_+6); return;
}

// Like "ecom_checkHazards", but the enemy doesn't animate when they fall into a hole. That is,
// they just get stuck on the last frame of their animation as they get sucked in.
void ecom_checkHazardsNoAnimationForHoles_b10_hook(GB *gb) {
  BASE(ecom_checkHazardsNoAnimationForHoles_b10);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+5); mem_wr(gb, hFF8D, A);
  CYC(b_+5, b_+7);
  TAIL(ecom_checkHazardsCommon_b10);
}

// Standard implementation of "enemy experiencing knockback" state. Also, doesn't "return from
// caller" if it fell in a hazard since it calls "ecom_checkHazards" instead of jumping to it.
void ecom_updateKnockbackAndCheckHazards_b10_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazards_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b10_hook, SYM(ecom_updateKnockback_b10), b_+3);
  CALL_C(b_+3, ecom_checkHazards_b10_hook, SYM(ecom_checkHazards_b10), b_+6);
  RET(b_+6); return;
}

// Checks whether the enemy falls into any hazards, and does the appropriate reaction if so.
// If the enemy falls in a hazard, this discards its return address to skip whatever remains in
// the caller.
void ecom_checkHazards_b10_hook(GB *gb) {
  BASE(ecom_checkHazards_b10);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+6); mem_wr(gb, hFF8D, A);
  TAIL(ecom_checkHazardsCommon_b10);
}

void ecom_checkHazardsCommon_b10_hook(GB *gb) {
  BASE(ecom_checkHazardsCommon_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto applyHazardEffect;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+11, b_+13);
    goto ret_;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x05ff);
  CALL_C(b_+16, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+19);
  CYC(b_+19, b_+22); SET_HL(hazardCollisionTable);
  CALL_C(b_+22, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+25);
  CYC(b_+25, b_+27); B = 0xff;
  if (F & FC) {
    CYCT(b_+27, b_+29);
    goto touchedHazard;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC(0x0501);
  CALL_C(b_+32, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+35);
  CYC(b_+35, b_+38); SET_HL(hazardCollisionTable);
  CALL_C(b_+38, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+41);
  CYC(b_+41, b_+43); B = 0x01;
  if (F & FC) {
    CYCT(b_+43, b_+45);
    goto touchedHazard;
  }
  CYC(b_+43, b_+45);
  CALL_C(b_+45, ecom_updateMovingPlatform_b10_hook, SYM(ecom_updateMovingPlatform_b10), b_+48);
ret_:
  CYC(b_+48, b_+50); A = mem_rd(gb, hFF8F);
  CYC(b_+50, b_+51); alu_or(gb, A);
  RET(b_+51); return;
touchedHazard:
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+55, b_+56); E = L;
  CYC(b_+56, b_+57); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x00);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+68, b_+70); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 60);
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+77); A = mem_rd(gb, hFF8D);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_add(gb, B);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
applyHazardEffect:
  // Discard return address: this enemy is about to be deleted.
  SET_HL(POP(b_+83));
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+86, b_+88);
    TAIL(ecom_makeSplashAndDelete_b10);
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+89, b_+91);
    TAIL(ecom_fallingInHole_b10);
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93);
  TAIL(ecom_makeLavaSplashAndDelete_b10);
}

void ecom_makeSplashAndDelete_b10_hook(GB *gb) {
  BASE(ecom_makeSplashAndDelete_b10);
  CYC(b_+0, b_+2); B = 0x03; // INTERAC_SPLASH
  CYC(b_+2, b_+4);
  ecom_splashOrLavaTail_b10_hook(gb);
}

void ecom_makeLavaSplashAndDelete_b10_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b10);
  CYC(b_+0, b_+2); B = 0x04; // INTERAC_LAVASPLASH
  ecom_splashOrLavaTail_b10_hook(gb);
}

void ecom_decNumEnemiesAndDelete_b10_hook(GB *gb) {
  BASE(ecom_decNumEnemiesAndDelete_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6);
  TAIL(enemyDelete);
}

void ecom_fallDownHoleAndDelete_b10_hook(GB *gb) {
  BASE(ecom_fallDownHoleAndDelete_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+3);
  CYC(b_+3, b_+5);
  TAIL(ecom_decNumEnemiesAndDelete_b10);
}

// @param[out] zflag z if enemy is in the center of the hole
static void ecom_fallingInHole_checkInCenterOfHole_b10(GB *gb) {
  BASE(ecom_fallingInHole_b10);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+48); mem_wr(gb, hFF8F, A);
  CYC(b_+48, b_+50); alu_add(gb, 0x05);
  CYC(b_+50, b_+52); alu_and(gb, 0xf0);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); mem_wr(gb, hFF8E, A);
  CYC(b_+59, b_+61); alu_and(gb, 0xf0);
  CYC(b_+61, b_+63); alu_add(gb, 0x08);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+65); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); A = mem_rd(gb, hFF8F);
  CYC(b_+68, b_+69); alu_cp(gb, B);
  RET(b_+69); return;
}

// Enemy is currently falling down a hole.
void ecom_fallingInHole_b10_hook(GB *gb) {
  BASE(ecom_fallingInHole_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    TAIL(ecom_fallDownHoleAndDelete_b10);
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (F & FZ) {
    CYC(b_+8, b_+10); CYC(b_+10, b_+13); push_effect(gb, b_+13);
    ecom_fallingInHole_checkInCenterOfHole_b10(gb);
    if (F & FZ) {
      CYCT(b_+13, b_+15);
      TAIL(ecom_fallDownHoleAndDelete_b10);
    }
    CYC(b_+13, b_+15);
    CALL_C(b_+15, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+18);
    CYC(b_+18, b_+19); C = A;
    CYC(b_+19, b_+21); B = 0x14; // SPEED_80
    CALL_C(b_+21, ecom_applyGivenVelocity_b10_hook, SYM(ecom_applyGivenVelocity_b10), b_+24);
  } else {
    CYCT(b_+8, b_+10);
  }
  // If bit 0 of counter2 is set, animate the enemy as it's being sucked toward the hole.
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+27, b_+29); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+35, b_+37);
  } else {
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+38); alu_xor(gb, A);
  }
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43);
  TAIL(enemyAnimate);
}

// Updates enemy's position if he's on a moving platform.
void ecom_updateMovingPlatform_b10_hook(GB *gb) {
  BASE(ecom_updateMovingPlatform_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0500);
  CALL_C(b_+8, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+11);
  CYC(b_+11, b_+14); SET_HL(SYM(enemyConveyorTilesTable_b10));
  CALL_C(b_+14, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+17);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); B = 0x14; // SPEED_80
  TAIL(ecom_applyGivenVelocity_b10);
}

void ecom_applyGivenVelocity_b10_hook(GB *gb) {
  BASE(ecom_applyGivenVelocity_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b10));
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8A, A);
  PUSH(b_+6, BC);
  CYC(b_+7, b_+8); A = C;
  CALL_C(b_+8, ecom_getAdjacentWallsBitset_b10_hook, SYM(ecom_getAdjacentWallsBitset_b10), b_+11);
  SET_BC(POP(b_+11));
  CYC(b_+12, b_+14);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b10);
}

void ecom_applyVelocityForTopDownEnemy_b10_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemy_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, ecom_getTopDownAdjacentWallsBitset_b10_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b10), b_+4);
  CYC(b_+4, b_+6);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b10);
}

void ecom_applyVelocityForTopDownEnemyNoHoles_b10_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemyNoHoles_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getTopDownAdjacentWallsBitset_b10_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b10), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b10);
}

void ecom_applyVelocityForSideviewEnemy_b10_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemy_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  CALL_C((SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b10) + 2), ecom_getSideviewAdjacentWallsBitset_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b10), SYM(ecom_applyVelocityGivenAdjacentWalls_b10));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b10);
}

void ecom_applyVelocityForSideviewEnemyNoHoles_b10_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemyNoHoles_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b10), SYM(ecom_applyVelocityGivenAdjacentWalls_b10));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b10);
}

void ecom_applyVelocityGivenAdjacentWalls_b10_hook(GB *gb) {
  BASE(ecom_applyVelocityGivenAdjacentWalls_b10);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE); C = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE); B = A;
  CYC(b_+5, b_+6);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b10);
}

static void ecom_addAToHl_from_rst_b10(GB *gb, uint16_t return_address) {
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

static void ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b10(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b10);
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+132); B = mem_rd(gb, HL);
  CYC(b_+132, b_+133); L = alu_inc8(gb, L);
  CYC(b_+133, b_+134); E = alu_inc8(gb, E);
  CYC(b_+134, b_+135); A = mem_rd(gb, DE);
  CYC(b_+135, b_+136); C = A;
  CYC(b_+136, b_+137); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+139); alu_sub(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+139, b_+141);
    goto capSpeed;
  }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); C = 0x20;
  CYC(b_+143, b_+145); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+148); alu_cp(gb, 0x32);
  if (F & FC) {
    CYCT(b_+148, b_+150);
    goto sharedF9;
  }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+152); C = 0x60;
sharedF9:
  CYC(b_+152, b_+153); A = B;
  CYC(b_+153, b_+154); alu_cp(gb, C);
  if (F & FC) { RET_TAKEN(b_+154); return; }
  CYC(b_+154, b_+155);
capSpeed:
  CYC(b_+155, b_+157); mem_wr(gb, hFF8D, A);
  RET(b_+157); return;
}

void ecom_applyGivenVelocityGivenAdjacentWalls_b10_hook(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); mem_wr(gb, hFF8C, A);
  CALL_C(b_+3, getPositionOffsetForVelocity_hook, SYM(getPositionOffsetForVelocity), b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hFF8D, A);
component1:
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFF8B);
  CYC(b_+13, b_+15); alu_and(gb, 0x0c);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto checkSlide1;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b10(gb);
  CYC(b_+20, b_+22);
  goto component2;
checkSlide1:
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+24, b_+26);
    goto component2;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_bit(gb, 3, A);
  CYC(b_+28, b_+30); A = mem_rd(gb, hFF8C);
  CYC(b_+30, b_+33); SET_BC(0x0060);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto checkSlideCap1;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); alu_xor(gb, 0x10);
  CYC(b_+37, b_+40); SET_BC(0xffa0);
checkSlideCap1:
  CYC(b_+40, b_+42); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+42, b_+44);
    goto component2;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_X;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_adc(gb, B);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+58); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+58, b_+60);
    goto component2;
  }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+64); mem_wr(gb, hFF8D, A);
component2:
  CYC(b_+64, b_+66); E = ENEMY_BASE + OBJ_X;
  CYC(b_+66, b_+68); L = 0xc2;
  CYC(b_+68, b_+70); A = mem_rd(gb, hFF8B);
  CYC(b_+70, b_+72); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+74);
    goto checkSlide2;
  }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+77); push_effect(gb, b_+77);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b10(gb);
  CYC(b_+77, b_+79);
  goto ret_;
checkSlide2:
  CYC(b_+79, b_+81); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+81, b_+83);
    goto ret_;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); alu_rrca(gb);
  CYC(b_+84, b_+86); A = mem_rd(gb, hFF8C);
  CYC(b_+86, b_+89); SET_BC(0x0060);
  if (!(F & FC)) {
    CYCT(b_+89, b_+91);
    goto checkSlideCap2;
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); alu_sub(gb, 0x10);
  CYC(b_+93, b_+96); SET_BC(0xffa0);
checkSlideCap2:
  CYC(b_+96, b_+98); alu_add(gb, 0x08);
  CYC(b_+98, b_+100); alu_and(gb, 0x1f);
  CYC(b_+100, b_+102); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+102, b_+104);
    goto ret_;
  }
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_add(gb, C);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_adc(gb, B);
  CYC(b_+112, b_+113); mem_wr(gb, DE, A);
  CYC(b_+113, b_+115); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+118, b_+120);
    goto ret_;
  }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+124); mem_wr(gb, hFF8D, A);
ret_:
  CYC(b_+124, b_+126); A = mem_rd(gb, hFF8D);
  CYC(b_+126, b_+127); alu_or(gb, A);
  RET(b_+127); return;
}

void ecom_getTopDownAdjacentWallsBitsetGivenAngle_b10_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitsetGivenAngle_b10);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b10));
  CYC(b_+3, b_+5);
  TAIL(ecom_getAdjacentWallsBitset_b10);
}

void ecom_getTopDownAdjacentWallsBitset_b10_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitset_b10);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+5); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b10));
  CYC(b_+5, b_+7);
  TAIL(label_025_b10);
}

void ecom_getSideviewAdjacentWallsBitset_b10_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitset_b10);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  TAIL(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10);
}

void ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b10));
  TAIL(label_025_b10);
}

void label_025_b10_hook(GB *gb) {
  BASE(label_025_b10);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8A, A);
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  TAIL(ecom_getAdjacentWallsBitset_b10);
}

void ecom_getAdjacentWallsBitset_checkCollisionAt_b10_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); SET_DE(DE + 1);
  CYC(b_+34, b_+35); alu_add(gb, B); B = A;
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); SET_DE(DE + 1);
  CYC(b_+38, b_+39); alu_add(gb, C); C = A;
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); A = mem_rd(gb, hFF8A);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+43, b_+46);
    TAIL(checkTileCollisionAt_disallowHoles);
  }
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
    goto secondCheck;
  }
  CYC(b_+47, b_+49);
  CALL_C(b_+49, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+52);
  CYC(b_+52, b_+54); alu_add(gb, 0x01);
  RET(b_+54); return;
secondCheck:
  CALL_C(b_+55, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+58);
  CYC(b_+58, b_+60); alu_add(gb, 0x01);
  if (!(F & FC)) {
    CYCT(b_+60, b_+63);
    TAIL(checkTileCollisionAt_allowHoles);
  }
  CYC(b_+60, b_+63);
  RET(b_+63); return;
}

void ecom_getAdjacentWallsBitset_b10_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, DE);
  CALL_C(b_+1, ecom_getAdjacentWallTableOffset_b10_hook, SYM(ecom_getAdjacentWallTableOffset_b10), b_+4);
  CYC(b_+4, b_+5); B = D;
  CYC(b_+5, b_+6);
  ecom_addAToHl_from_rst_b10(gb, b_+6);
  CYC(b_+6, b_+7); D = H;
  CYC(b_+7, b_+8); E = L;
  CYC(b_+8, b_+9); H = B;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); C = mem_rd(gb, HL);
  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+19); mem_wr(gb, hFF8B, A);
loop:
  CYC(b_+19, b_+22); push_effect(gb, b_+22);
  ecom_getAdjacentWallsBitset_checkCollisionAt_b10_hook(gb);
  CYC(b_+22, b_+24); A = mem_rd(gb, hFF8B);
  CYC(b_+24, b_+25); alu_rla(gb);
  CYC(b_+25, b_+27); mem_wr(gb, hFF8B, A);
  if (!(F & FC)) {
    CYCT(b_+27, b_+29);
    goto loop;
  }
  CYC(b_+27, b_+29);
  SET_DE(POP(b_+29));
  CYC(b_+30, b_+31); alu_or(gb, A);
  RET(b_+31); return;
}

void ecom_getAdjacentWallTableOffset_b10_hook(GB *gb) {
  BASE(ecom_getAdjacentWallTableOffset_b10);
  CYC(b_+0, b_+1); alu_rlca(gb); B = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); alu_and(gb, 0x0f);
  CYC(b_+4, b_+5); A = B;
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+10); alu_add(gb, 0x08);
  RET(b_+10); return;
}

void ecom_bounceOffScreenBoundary__getDirectionsHit_b10_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b10);
  CYC(b_+43, b_+45); C = 0x00;
  CYC(b_+45, b_+46); B = A;
  CYC(b_+46, b_+48); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+48, b_+50);
    goto afterInc;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); C = alu_inc8(gb, C);
afterInc:
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+54); alu_and(gb, 0x0c);
  if (F & FZ) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); C = (uint8_t)(C | (1 << 2));
  RET(b_+57); return;
}

// A local with an independent external caller (veranFinal_spiderForm_updateMovement); kept
// callable on its own rather than inlined into the bounce common body.
void ecom_bounceOffScreenBoundary__reverseDirection_b10_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b10);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_add(gb, 0x10);
  CYC(b_+38, b_+40); alu_and(gb, 0x1f);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); alu_or(gb, D);
  RET(b_+42); return;
}

void ecom_bounceOffScreenBoundary_common_b10_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b10), b_+5);
  CYC(b_+5, b_+8); push_effect(gb, b_+8);
  ecom_bounceOffScreenBoundary__getDirectionsHit_b10_hook(gb);
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    ecom_bounceOffScreenBoundary__reverseDirection_b10_hook(gb); return;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_HL(b_+74);
  CYC(b_+18, b_+20); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto rotate;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(b_+58);
rotate:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29);
  ecom_addAToHl_from_rst_b10(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_or(gb, D);
  RET(b_+32); return;
}

void ecom_bounceOffWallsAndHoles_b10_hook(GB *gb) {
  BASE(ecom_bounceOffWallsAndHoles_b10);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4);
  ecom_bounceOffScreenBoundary_common_b10_hook(gb);
}

void ecom_bounceOffWalls_b10_hook(GB *gb) {
  BASE(ecom_bounceOffWalls_b10);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  ecom_bounceOffScreenBoundary_common_b10_hook(gb);
}

void ecom_bounceOffScreenBoundary_b10_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b10);
  CYC(b_+0, b_+2); A = 0x02;
  ecom_bounceOffScreenBoundary_common_b10_hook(gb);
}

void ecom_randomBitwiseAndBCE_b10_hook(GB *gb) {
  BASE(ecom_randomBitwiseAndBCE_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, BC);
  CALL_C(b_+1, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+4);
  SET_BC(POP(b_+4));
  CYC(b_+5, b_+6); alu_and(gb, E); E = A;
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); alu_and(gb, B); B = A;
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+12); alu_and(gb, C); C = A;
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  RET(b_+14); return;
}

void ecom_setSpeedAndState8AndVisible_b10_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8AndVisible_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_setSpeedAndState8_b10_hook, SYM(ecom_setSpeedAndState8_b10), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetVisiblec2);
}

void ecom_setSpeedAndState8_b10_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8_b10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);
  RET(b_+8); return;
}

static void ecom_spawnEnemyCommon_b10_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b10);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); alu_xor(gb, A);
  RET(b_+8); return;
}

void ecom_spawnUncountedEnemyWithSubid01_b10_hook(GB *gb) {
  BASE(ecom_spawnUncountedEnemyWithSubid01_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6);
  ecom_spawnEnemyCommon_b10_hook(gb);
}

void ecom_spawnEnemyWithSubid01_b10_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  ecom_spawnEnemyCommon_b10_hook(gb);
}

void ecom_spawnProjectile_b10_hook(GB *gb) {
  BASE(ecom_spawnProjectile_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CALL_C(b_+5, objectCopyPosition_hook, SYM(objectCopyPosition), b_+8);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+10, b_+12); A = 0x80;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, D);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR18;
  CYC(b_+16, b_+18); A = 0xc0;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+26); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  RET(b_+29); return;
}

void ecom_decCounter1_b10_hook(GB *gb) {
  BASE(ecom_decCounter1_b10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_dec16BitCounter_b10_hook(GB *gb) {
  BASE(ecom_dec16BitCounter_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  TAIL(ecom_decCounter2_b10);
}

void ecom_decCounter2_b10_hook(GB *gb) {
  BASE(ecom_decCounter2_b10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
}

void ecom_updateCardinalAngleAwayFromTarget_b10_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleAwayFromTarget_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_xor(gb, 0x10);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_updateCardinalAngleTowardTarget_b10_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleTowardTarget_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

void ecom_updateAngleTowardTarget_b10_hook(GB *gb) {
  BASE(ecom_updateAngleTowardTarget_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

void ecom_setRandomCardinalAngle_b10_hook(GB *gb) {
  BASE(ecom_setRandomCardinalAngle_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_setRandomAngle_b10_hook(GB *gb) {
  BASE(ecom_setRandomAngle_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

// @angleToAnimIndex ($43ff, 32 bytes) is a private lookup table, not code; it is only reached by
// address arithmetic, so it needs no hook entry of its own.
void ecom_updateAnimationFromAngle_b10_hook(GB *gb) {
  BASE(ecom_updateAnimationFromAngle_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+8); SET_BC(b_+39);
  CALL_C(b_+8, addAToBc_hook, 0x006d, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+14); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+14, b_+16);
    goto setAnimation;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); alu_cp(gb, 0x07);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); alu_sub(gb, 0x03);
  CYC(b_+22, b_+24); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); A = E;
  CYC(b_+26, b_+28); alu_add(gb, 0x04);
  CYC(b_+28, b_+30); alu_and(gb, 0x18);
  CYC(b_+30, b_+32); A = alu_swap(gb, A);
  CYC(b_+32, b_+33); alu_rlca(gb);
setAnimation:
  CYC(b_+33, b_+34); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+39);
  TAIL(enemySetAnimation);
}

void ecom_flickerVisibility_b10_hook(GB *gb) {
  BASE(ecom_flickerVisibility_b10);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x80);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

// @param[out] a State
// @param[out] b Subid
// @param[out] cflag c if state < 8
void ecom_getSubidAndCpStateTo08_b10_hook(GB *gb) {
  BASE(ecom_getSubidAndCpStateTo08_b10);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE); B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x08);
  RET(b_+9); return;
}

void ecom_moveTowardPosition_b10_hook(GB *gb) {
  BASE(ecom_moveTowardPosition_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9);
  TAIL(objectApplySpeed);
}

void ecom_readPositionVars_b10_hook(GB *gb) {
  BASE(ecom_readPositionVars_b10);
  CYC(b_+0, b_+1); B = mem_rd(gb, HL);
  CYC(b_+1, b_+2); L = alu_inc8(gb, L);
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, hFF8F, A);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); mem_wr(gb, hFF8E, A);
  RET(b_+12); return;
}

void ecom_setZAboveScreen_b10_hook(GB *gb) {
  BASE(ecom_setZAboveScreen_b10);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, C);
  CYC(b_+5, b_+6); alu_cpl(gb);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = mem_rd(gb, hCameraY);
  CYC(b_+10, b_+11); alu_add(gb, C);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
    goto checkTop;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = C;
checkTop:
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto storeZ;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); A = 0x80;
storeZ:
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

void ecom_killObjectH_b10_hook(GB *gb) {
  BASE(ecom_killObjectH_b10);
  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+3); alu_and(gb, 0xc0);
  CYC(b_+3, b_+5); alu_or(gb, 0x29);
  CYC(b_+5, b_+6); L = A;
  TAIL(ecom_killRelatedObj_b10);
}

void ecom_killRelatedObj_b10_hook(GB *gb) {
  BASE(ecom_killRelatedObj_b10);
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x00);
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+5); alu_add(gb, 0xfb);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(b_+8); return;
}

void ecom_killRelatedObj1_b10_hook(GB *gb) {
  BASE(ecom_killRelatedObj1_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b10);
}

void ecom_killRelatedObj2_b10_hook(GB *gb) {
  BASE(ecom_killRelatedObj2_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b10);
}

// @oscillationX ($44a8) is a private position-offset lookup table, not code; it needs no hook
// entry of its own.
void ecom_galeSeedEffect_b10_hook(GB *gb) {
  BASE(ecom_galeSeedEffect_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto zero;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+11); SET_HL(b_+45);
  CYC(b_+11, b_+12);
  ecom_addAToHl_from_rst_b10(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_scf(gb);
  RET(b_+18); return;
zero:
  CALL_C(b_+19, objectApplySpeed_hook, SYM(objectApplySpeed), b_+22);
  CYC(b_+22, b_+24); C = 0x10;
  CALL_C(b_+24, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+27);
  CYC(b_+27, b_+29); A = mem_rd(gb, hCameraY);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_cp(gb, 0x80);
  CYC(b_+35, b_+36); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+42); alu_sub(gb, B);
  CYC(b_+42, b_+44); alu_cp(gb, 0xb0);
  RET(b_+44); return;
}

void ecom_blownByGaleSeedState_b10_hook(GB *gb) {
  BASE(ecom_blownByGaleSeedState_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_galeSeedEffect_b10_hook, SYM(ecom_galeSeedEffect_b10), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, decNumEnemies_hook, SYM(decNumEnemies), b_+7);
  CYC(b_+7, b_+10);
  TAIL(enemyDelete);
}

void ecom_checkScentSeedActive_b10_hook(GB *gb) {
  BASE(ecom_checkScentSeedActive_b10);
  CYC(b_+0, b_+3); A = W8(wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_bit(gb, 4, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_and(gb, 0xf8);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); A = 0x04;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_updateAngleToScentSeed_b10_hook(GB *gb) {
  BASE(ecom_updateAngleToScentSeed_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR3D;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = mem_rd(gb, hFFB2);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFFB3);
  CYC(b_+13, b_+14); C = A;
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_fallToGroundAndSetState8_b10_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState8_b10);
  CYC(b_+0, b_+2); B = 0x08;
  TAIL(ecom_fallToGroundAndSetState_b10);
}

void ecom_fallToGroundAndSetState_b10_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState_b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  RET(b_+13); return;
}

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

// object_code/common/enemies/commonCode.s, bank $0d instance. Byte-identical addresses to
// the bank $10 copy above (same source, same offset from bank start in every enemy bank),
// confirmed via --report on both banks; this section is a mechanical rename of that one.


void ecom_splashOrLavaTail_b0d_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), SYM(ecom_decNumEnemiesAndDelete_b0d));
  TAIL(ecom_decNumEnemiesAndDelete_b0d);
}

void ecom_incState_b0d_hook(GB *gb) {
  BASE(ecom_incState_b0d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_incSubstate_b0d_hook(GB *gb) {
  BASE(ecom_incSubstate_b0d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// Update knockback where solid tiles are defined "normally".
void ecom_updateKnockback_b0d_hook(GB *gb) {
  BASE(ecom_updateKnockback_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+3, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d), SYM(ecom_updateKnockback_common_b0d));
  TAIL(ecom_updateKnockback_common_b0d);
}

void ecom_updateKnockback_common_b0d_hook(GB *gb) {
  BASE(ecom_updateKnockback_common_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, DE); C = A;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+8); B = 0x50; // SPEED_200
  if (F & FC) {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); B = 0x78; // SPEED_300
    CYC(b_+12, b_+14); alu_and(gb, 0x06);
    if (F & FZ) {
      CYC(b_+14, b_+16);
      PUSH(b_+16, BC);
      CYC(b_+17, b_+20); SET_BC(0x0f01);
      CALL_C(b_+20, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+23);
      SET_BC(POP(b_+23));
    } else {
      CYCT(b_+14, b_+16);
    }
  } else {
    CYCT(b_+8, b_+10);
  }
  CALL_C(b_+24, ecom_applyGivenVelocityGivenAdjacentWalls_b0d_hook, SYM(ecom_applyGivenVelocityGivenAdjacentWalls_b0d), b_+27);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_and(gb, 0x80);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

// Update knockback where the enemy can pass through anything except the screen boundary.
void ecom_updateKnockbackNoSolidity_b0d_hook(GB *gb) {
  BASE(ecom_updateKnockbackNoSolidity_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+4, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d), b_+7);
  CYC(b_+7, b_+9);
  TAIL(ecom_updateKnockback_common_b0d);
}

void ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0d_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b0d_hook, SYM(ecom_updateKnockback_b0d), b_+3);
  CALL_C(b_+3, ecom_checkHazardsNoAnimationForHoles_b0d_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b0d), b_+6);
  RET(b_+6); return;
}

// Like "ecom_checkHazards", but the enemy doesn't animate when they fall into a hole. That is,
// they just get stuck on the last frame of their animation as they get sucked in.
void ecom_checkHazardsNoAnimationForHoles_b0d_hook(GB *gb) {
  BASE(ecom_checkHazardsNoAnimationForHoles_b0d);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+5); mem_wr(gb, hFF8D, A);
  CYC(b_+5, b_+7);
  TAIL(ecom_checkHazardsCommon_b0d);
}

// Standard implementation of "enemy experiencing knockback" state. Also, doesn't "return from
// caller" if it fell in a hazard since it calls "ecom_checkHazards" instead of jumping to it.
void ecom_updateKnockbackAndCheckHazards_b0d_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazards_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b0d_hook, SYM(ecom_updateKnockback_b0d), b_+3);
  CALL_C(b_+3, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+6);
  RET(b_+6); return;
}

// Checks whether the enemy falls into any hazards, and does the appropriate reaction if so.
// If the enemy falls in a hazard, this discards its return address to skip whatever remains in
// the caller.
void ecom_checkHazards_b0d_hook(GB *gb) {
  BASE(ecom_checkHazards_b0d);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+6); mem_wr(gb, hFF8D, A);
  TAIL(ecom_checkHazardsCommon_b0d);
}

void ecom_checkHazardsCommon_b0d_hook(GB *gb) {
  BASE(ecom_checkHazardsCommon_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto applyHazardEffect;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+11, b_+13);
    goto ret_;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x05ff);
  CALL_C(b_+16, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+19);
  CYC(b_+19, b_+22); SET_HL(hazardCollisionTable);
  CALL_C(b_+22, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+25);
  CYC(b_+25, b_+27); B = 0xff;
  if (F & FC) {
    CYCT(b_+27, b_+29);
    goto touchedHazard;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC(0x0501);
  CALL_C(b_+32, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+35);
  CYC(b_+35, b_+38); SET_HL(hazardCollisionTable);
  CALL_C(b_+38, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+41);
  CYC(b_+41, b_+43); B = 0x01;
  if (F & FC) {
    CYCT(b_+43, b_+45);
    goto touchedHazard;
  }
  CYC(b_+43, b_+45);
  CALL_C(b_+45, ecom_updateMovingPlatform_b0d_hook, SYM(ecom_updateMovingPlatform_b0d), b_+48);
ret_:
  CYC(b_+48, b_+50); A = mem_rd(gb, hFF8F);
  CYC(b_+50, b_+51); alu_or(gb, A);
  RET(b_+51); return;
touchedHazard:
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+55, b_+56); E = L;
  CYC(b_+56, b_+57); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x00);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+68, b_+70); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 60);
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+77); A = mem_rd(gb, hFF8D);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_add(gb, B);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
applyHazardEffect:
  // Discard return address: this enemy is about to be deleted.
  SET_HL(POP(b_+83));
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+86, b_+88);
    TAIL(ecom_makeSplashAndDelete_b0d);
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+89, b_+91);
    TAIL(ecom_fallingInHole_b0d);
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93);
  TAIL(ecom_makeLavaSplashAndDelete_b0d);
}

void ecom_makeSplashAndDelete_b0d_hook(GB *gb) {
  BASE(ecom_makeSplashAndDelete_b0d);
  CYC(b_+0, b_+2); B = 0x03; // INTERAC_SPLASH
  CYC(b_+2, b_+4);
  ecom_splashOrLavaTail_b0d_hook(gb);
}

void ecom_makeLavaSplashAndDelete_b0d_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b0d);
  CYC(b_+0, b_+2); B = 0x04; // INTERAC_LAVASPLASH
  ecom_splashOrLavaTail_b0d_hook(gb);
}

void ecom_decNumEnemiesAndDelete_b0d_hook(GB *gb) {
  BASE(ecom_decNumEnemiesAndDelete_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6);
  TAIL(enemyDelete);
}

void ecom_fallDownHoleAndDelete_b0d_hook(GB *gb) {
  BASE(ecom_fallDownHoleAndDelete_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+3);
  CYC(b_+3, b_+5);
  TAIL(ecom_decNumEnemiesAndDelete_b0d);
}

// @param[out] zflag z if enemy is in the center of the hole
static void ecom_fallingInHole_checkInCenterOfHole_b0d(GB *gb) {
  BASE(ecom_fallingInHole_b0d);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+48); mem_wr(gb, hFF8F, A);
  CYC(b_+48, b_+50); alu_add(gb, 0x05);
  CYC(b_+50, b_+52); alu_and(gb, 0xf0);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); mem_wr(gb, hFF8E, A);
  CYC(b_+59, b_+61); alu_and(gb, 0xf0);
  CYC(b_+61, b_+63); alu_add(gb, 0x08);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+65); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); A = mem_rd(gb, hFF8F);
  CYC(b_+68, b_+69); alu_cp(gb, B);
  RET(b_+69); return;
}

// Enemy is currently falling down a hole.
void ecom_fallingInHole_b0d_hook(GB *gb) {
  BASE(ecom_fallingInHole_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    TAIL(ecom_fallDownHoleAndDelete_b0d);
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (F & FZ) {
    CYC(b_+8, b_+10); CYC(b_+10, b_+13); push_effect(gb, b_+13);
    ecom_fallingInHole_checkInCenterOfHole_b0d(gb);
    if (F & FZ) {
      CYCT(b_+13, b_+15);
      TAIL(ecom_fallDownHoleAndDelete_b0d);
    }
    CYC(b_+13, b_+15);
    CALL_C(b_+15, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+18);
    CYC(b_+18, b_+19); C = A;
    CYC(b_+19, b_+21); B = 0x14; // SPEED_80
    CALL_C(b_+21, ecom_applyGivenVelocity_b0d_hook, SYM(ecom_applyGivenVelocity_b0d), b_+24);
  } else {
    CYCT(b_+8, b_+10);
  }
  // If bit 0 of counter2 is set, animate the enemy as it's being sucked toward the hole.
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+27, b_+29); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+35, b_+37);
  } else {
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+38); alu_xor(gb, A);
  }
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43);
  TAIL(enemyAnimate);
}

// Updates enemy's position if he's on a moving platform.
void ecom_updateMovingPlatform_b0d_hook(GB *gb) {
  BASE(ecom_updateMovingPlatform_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0500);
  CALL_C(b_+8, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+11);
  CYC(b_+11, b_+14); SET_HL(SYM(enemyConveyorTilesTable_b0d));
  CALL_C(b_+14, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+17);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); B = 0x14; // SPEED_80
  TAIL(ecom_applyGivenVelocity_b0d);
}

void ecom_applyGivenVelocity_b0d_hook(GB *gb) {
  BASE(ecom_applyGivenVelocity_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0d));
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8A, A);
  PUSH(b_+6, BC);
  CYC(b_+7, b_+8); A = C;
  CALL_C(b_+8, ecom_getAdjacentWallsBitset_b0d_hook, SYM(ecom_getAdjacentWallsBitset_b0d), b_+11);
  SET_BC(POP(b_+11));
  CYC(b_+12, b_+14);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b0d);
}

void ecom_applyVelocityForTopDownEnemy_b0d_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemy_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, ecom_getTopDownAdjacentWallsBitset_b0d_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+4);
  CYC(b_+4, b_+6);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0d);
}

void ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemyNoHoles_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getTopDownAdjacentWallsBitset_b0d_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0d);
}

void ecom_applyVelocityForSideviewEnemy_b0d_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemy_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  CALL_C((SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d) + 2), ecom_getSideviewAdjacentWallsBitset_b0d_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0d), SYM(ecom_applyVelocityGivenAdjacentWalls_b0d));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0d);
}

void ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemyNoHoles_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0d_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0d), SYM(ecom_applyVelocityGivenAdjacentWalls_b0d));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0d);
}

void ecom_applyVelocityGivenAdjacentWalls_b0d_hook(GB *gb) {
  BASE(ecom_applyVelocityGivenAdjacentWalls_b0d);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE); C = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE); B = A;
  CYC(b_+5, b_+6);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b0d);
}

static void ecom_addAToHl_from_rst_b0d(GB *gb, uint16_t return_address) {
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

static void ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0d(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b0d);
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+132); B = mem_rd(gb, HL);
  CYC(b_+132, b_+133); L = alu_inc8(gb, L);
  CYC(b_+133, b_+134); E = alu_inc8(gb, E);
  CYC(b_+134, b_+135); A = mem_rd(gb, DE);
  CYC(b_+135, b_+136); C = A;
  CYC(b_+136, b_+137); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+139); alu_sub(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+139, b_+141);
    goto capSpeed;
  }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); C = 0x20;
  CYC(b_+143, b_+145); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+148); alu_cp(gb, 0x32);
  if (F & FC) {
    CYCT(b_+148, b_+150);
    goto sharedF9;
  }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+152); C = 0x60;
sharedF9:
  CYC(b_+152, b_+153); A = B;
  CYC(b_+153, b_+154); alu_cp(gb, C);
  if (F & FC) { RET_TAKEN(b_+154); return; }
  CYC(b_+154, b_+155);
capSpeed:
  CYC(b_+155, b_+157); mem_wr(gb, hFF8D, A);
  RET(b_+157); return;
}

void ecom_applyGivenVelocityGivenAdjacentWalls_b0d_hook(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); mem_wr(gb, hFF8C, A);
  CALL_C(b_+3, getPositionOffsetForVelocity_hook, SYM(getPositionOffsetForVelocity), b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hFF8D, A);
component1:
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFF8B);
  CYC(b_+13, b_+15); alu_and(gb, 0x0c);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto checkSlide1;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0d(gb);
  CYC(b_+20, b_+22);
  goto component2;
checkSlide1:
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+24, b_+26);
    goto component2;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_bit(gb, 3, A);
  CYC(b_+28, b_+30); A = mem_rd(gb, hFF8C);
  CYC(b_+30, b_+33); SET_BC(0x0060);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto checkSlideCap1;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); alu_xor(gb, 0x10);
  CYC(b_+37, b_+40); SET_BC(0xffa0);
checkSlideCap1:
  CYC(b_+40, b_+42); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+42, b_+44);
    goto component2;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_X;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_adc(gb, B);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+58); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+58, b_+60);
    goto component2;
  }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+64); mem_wr(gb, hFF8D, A);
component2:
  CYC(b_+64, b_+66); E = ENEMY_BASE + OBJ_X;
  CYC(b_+66, b_+68); L = 0xc2;
  CYC(b_+68, b_+70); A = mem_rd(gb, hFF8B);
  CYC(b_+70, b_+72); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+74);
    goto checkSlide2;
  }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+77); push_effect(gb, b_+77);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0d(gb);
  CYC(b_+77, b_+79);
  goto ret_;
checkSlide2:
  CYC(b_+79, b_+81); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+81, b_+83);
    goto ret_;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); alu_rrca(gb);
  CYC(b_+84, b_+86); A = mem_rd(gb, hFF8C);
  CYC(b_+86, b_+89); SET_BC(0x0060);
  if (!(F & FC)) {
    CYCT(b_+89, b_+91);
    goto checkSlideCap2;
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); alu_sub(gb, 0x10);
  CYC(b_+93, b_+96); SET_BC(0xffa0);
checkSlideCap2:
  CYC(b_+96, b_+98); alu_add(gb, 0x08);
  CYC(b_+98, b_+100); alu_and(gb, 0x1f);
  CYC(b_+100, b_+102); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+102, b_+104);
    goto ret_;
  }
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_add(gb, C);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_adc(gb, B);
  CYC(b_+112, b_+113); mem_wr(gb, DE, A);
  CYC(b_+113, b_+115); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+118, b_+120);
    goto ret_;
  }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+124); mem_wr(gb, hFF8D, A);
ret_:
  CYC(b_+124, b_+126); A = mem_rd(gb, hFF8D);
  CYC(b_+126, b_+127); alu_or(gb, A);
  RET(b_+127); return;
}

void ecom_getTopDownAdjacentWallsBitsetGivenAngle_b0d_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitsetGivenAngle_b0d);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b0d));
  CYC(b_+3, b_+5);
  TAIL(ecom_getAdjacentWallsBitset_b0d);
}

void ecom_getTopDownAdjacentWallsBitset_b0d_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitset_b0d);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+5); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b0d));
  CYC(b_+5, b_+7);
  TAIL(label_025_b0d);
}

void ecom_getSideviewAdjacentWallsBitset_b0d_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitset_b0d);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  TAIL(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d);
}

void ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0d);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0d));
  TAIL(label_025_b0d);
}

void label_025_b0d_hook(GB *gb) {
  BASE(label_025_b0d);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8A, A);
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  TAIL(ecom_getAdjacentWallsBitset_b0d);
}

void ecom_getAdjacentWallsBitset_checkCollisionAt_b0d_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); SET_DE(DE + 1);
  CYC(b_+34, b_+35); alu_add(gb, B); B = A;
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); SET_DE(DE + 1);
  CYC(b_+38, b_+39); alu_add(gb, C); C = A;
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); A = mem_rd(gb, hFF8A);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+43, b_+46);
    TAIL(checkTileCollisionAt_disallowHoles);
  }
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
    goto secondCheck;
  }
  CYC(b_+47, b_+49);
  CALL_C(b_+49, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+52);
  CYC(b_+52, b_+54); alu_add(gb, 0x01);
  RET(b_+54); return;
secondCheck:
  CALL_C(b_+55, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+58);
  CYC(b_+58, b_+60); alu_add(gb, 0x01);
  if (!(F & FC)) {
    CYCT(b_+60, b_+63);
    TAIL(checkTileCollisionAt_allowHoles);
  }
  CYC(b_+60, b_+63);
  RET(b_+63); return;
}

void ecom_getAdjacentWallsBitset_b0d_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, DE);
  CALL_C(b_+1, ecom_getAdjacentWallTableOffset_b0d_hook, SYM(ecom_getAdjacentWallTableOffset_b0d), b_+4);
  CYC(b_+4, b_+5); B = D;
  CYC(b_+5, b_+6);
  ecom_addAToHl_from_rst_b0d(gb, b_+6);
  CYC(b_+6, b_+7); D = H;
  CYC(b_+7, b_+8); E = L;
  CYC(b_+8, b_+9); H = B;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); C = mem_rd(gb, HL);
  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+19); mem_wr(gb, hFF8B, A);
loop:
  CYC(b_+19, b_+22); push_effect(gb, b_+22);
  ecom_getAdjacentWallsBitset_checkCollisionAt_b0d_hook(gb);
  CYC(b_+22, b_+24); A = mem_rd(gb, hFF8B);
  CYC(b_+24, b_+25); alu_rla(gb);
  CYC(b_+25, b_+27); mem_wr(gb, hFF8B, A);
  if (!(F & FC)) {
    CYCT(b_+27, b_+29);
    goto loop;
  }
  CYC(b_+27, b_+29);
  SET_DE(POP(b_+29));
  CYC(b_+30, b_+31); alu_or(gb, A);
  RET(b_+31); return;
}

void ecom_getAdjacentWallTableOffset_b0d_hook(GB *gb) {
  BASE(ecom_getAdjacentWallTableOffset_b0d);
  CYC(b_+0, b_+1); alu_rlca(gb); B = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); alu_and(gb, 0x0f);
  CYC(b_+4, b_+5); A = B;
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+10); alu_add(gb, 0x08);
  RET(b_+10); return;
}

void ecom_bounceOffScreenBoundary__getDirectionsHit_b0d_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0d);
  CYC(b_+43, b_+45); C = 0x00;
  CYC(b_+45, b_+46); B = A;
  CYC(b_+46, b_+48); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+48, b_+50);
    goto afterInc;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); C = alu_inc8(gb, C);
afterInc:
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+54); alu_and(gb, 0x0c);
  if (F & FZ) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); C = (uint8_t)(C | (1 << 2));
  RET(b_+57); return;
}

// A local with an independent external caller (veranFinal_spiderForm_updateMovement); kept
// callable on its own rather than inlined into the bounce common body.
void ecom_bounceOffScreenBoundary__reverseDirection_b0d_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0d);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_add(gb, 0x10);
  CYC(b_+38, b_+40); alu_and(gb, 0x1f);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); alu_or(gb, D);
  RET(b_+42); return;
}

void ecom_bounceOffScreenBoundary_common_b0d_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0d_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0d), b_+5);
  CYC(b_+5, b_+8); push_effect(gb, b_+8);
  ecom_bounceOffScreenBoundary__getDirectionsHit_b0d_hook(gb);
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    ecom_bounceOffScreenBoundary__reverseDirection_b0d_hook(gb); return;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_HL(b_+74);
  CYC(b_+18, b_+20); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto rotate;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(b_+58);
rotate:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29);
  ecom_addAToHl_from_rst_b0d(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_or(gb, D);
  RET(b_+32); return;
}

void ecom_bounceOffWallsAndHoles_b0d_hook(GB *gb) {
  BASE(ecom_bounceOffWallsAndHoles_b0d);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4);
  ecom_bounceOffScreenBoundary_common_b0d_hook(gb);
}

void ecom_bounceOffWalls_b0d_hook(GB *gb) {
  BASE(ecom_bounceOffWalls_b0d);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  ecom_bounceOffScreenBoundary_common_b0d_hook(gb);
}

void ecom_bounceOffScreenBoundary_b0d_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0d);
  CYC(b_+0, b_+2); A = 0x02;
  ecom_bounceOffScreenBoundary_common_b0d_hook(gb);
}

void ecom_randomBitwiseAndBCE_b0d_hook(GB *gb) {
  BASE(ecom_randomBitwiseAndBCE_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, BC);
  CALL_C(b_+1, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+4);
  SET_BC(POP(b_+4));
  CYC(b_+5, b_+6); alu_and(gb, E); E = A;
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); alu_and(gb, B); B = A;
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+12); alu_and(gb, C); C = A;
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  RET(b_+14); return;
}

void ecom_setSpeedAndState8AndVisible_b0d_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8AndVisible_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetVisiblec2);
}

void ecom_setSpeedAndState8_b0d_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8_b0d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);
  RET(b_+8); return;
}

static void ecom_spawnEnemyCommon_b0d_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b0d);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); alu_xor(gb, A);
  RET(b_+8); return;
}

void ecom_spawnUncountedEnemyWithSubid01_b0d_hook(GB *gb) {
  BASE(ecom_spawnUncountedEnemyWithSubid01_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6);
  ecom_spawnEnemyCommon_b0d_hook(gb);
}

void ecom_spawnEnemyWithSubid01_b0d_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  ecom_spawnEnemyCommon_b0d_hook(gb);
}

void ecom_spawnProjectile_b0d_hook(GB *gb) {
  BASE(ecom_spawnProjectile_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CALL_C(b_+5, objectCopyPosition_hook, SYM(objectCopyPosition), b_+8);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+10, b_+12); A = 0x80;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, D);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR18;
  CYC(b_+16, b_+18); A = 0xc0;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+26); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  RET(b_+29); return;
}

void ecom_decCounter1_b0d_hook(GB *gb) {
  BASE(ecom_decCounter1_b0d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_dec16BitCounter_b0d_hook(GB *gb) {
  BASE(ecom_dec16BitCounter_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  TAIL(ecom_decCounter2_b0d);
}

void ecom_decCounter2_b0d_hook(GB *gb) {
  BASE(ecom_decCounter2_b0d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
}

void ecom_updateCardinalAngleAwayFromTarget_b0d_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleAwayFromTarget_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_xor(gb, 0x10);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_updateCardinalAngleTowardTarget_b0d_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleTowardTarget_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

void ecom_updateAngleTowardTarget_b0d_hook(GB *gb) {
  BASE(ecom_updateAngleTowardTarget_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

void ecom_setRandomCardinalAngle_b0d_hook(GB *gb) {
  BASE(ecom_setRandomCardinalAngle_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_setRandomAngle_b0d_hook(GB *gb) {
  BASE(ecom_setRandomAngle_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

// @angleToAnimIndex ($43ff, 32 bytes) is a private lookup table, not code; it is only reached by
// address arithmetic, so it needs no hook entry of its own.
void ecom_updateAnimationFromAngle_b0d_hook(GB *gb) {
  BASE(ecom_updateAnimationFromAngle_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+8); SET_BC(b_+39);
  CALL_C(b_+8, addAToBc_hook, 0x006d, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+14); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+14, b_+16);
    goto setAnimation;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); alu_cp(gb, 0x07);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); alu_sub(gb, 0x03);
  CYC(b_+22, b_+24); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); A = E;
  CYC(b_+26, b_+28); alu_add(gb, 0x04);
  CYC(b_+28, b_+30); alu_and(gb, 0x18);
  CYC(b_+30, b_+32); A = alu_swap(gb, A);
  CYC(b_+32, b_+33); alu_rlca(gb);
setAnimation:
  CYC(b_+33, b_+34); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+39);
  TAIL(enemySetAnimation);
}

void ecom_flickerVisibility_b0d_hook(GB *gb) {
  BASE(ecom_flickerVisibility_b0d);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x80);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

// @param[out] a State
// @param[out] b Subid
// @param[out] cflag c if state < 8
void ecom_getSubidAndCpStateTo08_b0d_hook(GB *gb) {
  BASE(ecom_getSubidAndCpStateTo08_b0d);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE); B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x08);
  RET(b_+9); return;
}

void ecom_moveTowardPosition_b0d_hook(GB *gb) {
  BASE(ecom_moveTowardPosition_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9);
  TAIL(objectApplySpeed);
}

void ecom_readPositionVars_b0d_hook(GB *gb) {
  BASE(ecom_readPositionVars_b0d);
  CYC(b_+0, b_+1); B = mem_rd(gb, HL);
  CYC(b_+1, b_+2); L = alu_inc8(gb, L);
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, hFF8F, A);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); mem_wr(gb, hFF8E, A);
  RET(b_+12); return;
}

void ecom_setZAboveScreen_b0d_hook(GB *gb) {
  BASE(ecom_setZAboveScreen_b0d);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, C);
  CYC(b_+5, b_+6); alu_cpl(gb);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = mem_rd(gb, hCameraY);
  CYC(b_+10, b_+11); alu_add(gb, C);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
    goto checkTop;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = C;
checkTop:
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto storeZ;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); A = 0x80;
storeZ:
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

void ecom_killObjectH_b0d_hook(GB *gb) {
  BASE(ecom_killObjectH_b0d);
  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+3); alu_and(gb, 0xc0);
  CYC(b_+3, b_+5); alu_or(gb, 0x29);
  CYC(b_+5, b_+6); L = A;
  TAIL(ecom_killRelatedObj_b0d);
}

void ecom_killRelatedObj_b0d_hook(GB *gb) {
  BASE(ecom_killRelatedObj_b0d);
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x00);
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+5); alu_add(gb, 0xfb);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(b_+8); return;
}

void ecom_killRelatedObj1_b0d_hook(GB *gb) {
  BASE(ecom_killRelatedObj1_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b0d);
}

void ecom_killRelatedObj2_b0d_hook(GB *gb) {
  BASE(ecom_killRelatedObj2_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b0d);
}

// @oscillationX ($44a8) is a private position-offset lookup table, not code; it needs no hook
// entry of its own.
void ecom_galeSeedEffect_b0d_hook(GB *gb) {
  BASE(ecom_galeSeedEffect_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto zero;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+11); SET_HL(b_+45);
  CYC(b_+11, b_+12);
  ecom_addAToHl_from_rst_b0d(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_scf(gb);
  RET(b_+18); return;
zero:
  CALL_C(b_+19, objectApplySpeed_hook, SYM(objectApplySpeed), b_+22);
  CYC(b_+22, b_+24); C = 0x10;
  CALL_C(b_+24, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+27);
  CYC(b_+27, b_+29); A = mem_rd(gb, hCameraY);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_cp(gb, 0x80);
  CYC(b_+35, b_+36); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+42); alu_sub(gb, B);
  CYC(b_+42, b_+44); alu_cp(gb, 0xb0);
  RET(b_+44); return;
}

void ecom_blownByGaleSeedState_b0d_hook(GB *gb) {
  BASE(ecom_blownByGaleSeedState_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_galeSeedEffect_b0d_hook, SYM(ecom_galeSeedEffect_b0d), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, decNumEnemies_hook, SYM(decNumEnemies), b_+7);
  CYC(b_+7, b_+10);
  TAIL(enemyDelete);
}

void ecom_checkScentSeedActive_b0d_hook(GB *gb) {
  BASE(ecom_checkScentSeedActive_b0d);
  CYC(b_+0, b_+3); A = W8(wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_bit(gb, 4, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_and(gb, 0xf8);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); A = 0x04;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_updateAngleToScentSeed_b0d_hook(GB *gb) {
  BASE(ecom_updateAngleToScentSeed_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR3D;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = mem_rd(gb, hFFB2);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFFB3);
  CYC(b_+13, b_+14); C = A;
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_fallToGroundAndSetState8_b0d_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState8_b0d);
  CYC(b_+0, b_+2); B = 0x08;
  TAIL(ecom_fallToGroundAndSetState_b0d);
}

void ecom_fallToGroundAndSetState_b0d_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState_b0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  RET(b_+13); return;
}


#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

// object_code/common/enemies/commonCode.s, bank $0e instance. Byte-identical addresses to
// the bank $0d/$10 copies above (same source, same offset from bank start in every enemy
// bank), confirmed via --report; this section is a mechanical rename of the bank-$0d copy
// (itself already fixed for the missing-push and OBJ_X bugs found there).

void ecom_splashOrLavaTail_b0e_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), SYM(ecom_decNumEnemiesAndDelete_b0e));
  TAIL(ecom_decNumEnemiesAndDelete_b0e);
}

void ecom_incState_b0e_hook(GB *gb) {
  BASE(ecom_incState_b0e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_incSubstate_b0e_hook(GB *gb) {
  BASE(ecom_incSubstate_b0e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// Update knockback where solid tiles are defined "normally".
void ecom_updateKnockback_b0e_hook(GB *gb) {
  BASE(ecom_updateKnockback_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+3, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e), SYM(ecom_updateKnockback_common_b0e));
  TAIL(ecom_updateKnockback_common_b0e);
}

void ecom_updateKnockback_common_b0e_hook(GB *gb) {
  BASE(ecom_updateKnockback_common_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, DE); C = A;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+8); B = 0x50; // SPEED_200
  if (F & FC) {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); B = 0x78; // SPEED_300
    CYC(b_+12, b_+14); alu_and(gb, 0x06);
    if (F & FZ) {
      CYC(b_+14, b_+16);
      PUSH(b_+16, BC);
      CYC(b_+17, b_+20); SET_BC(0x0f01);
      CALL_C(b_+20, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+23);
      SET_BC(POP(b_+23));
    } else {
      CYCT(b_+14, b_+16);
    }
  } else {
    CYCT(b_+8, b_+10);
  }
  CALL_C(b_+24, ecom_applyGivenVelocityGivenAdjacentWalls_b0e_hook, SYM(ecom_applyGivenVelocityGivenAdjacentWalls_b0e), b_+27);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_and(gb, 0x80);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

// Update knockback where the enemy can pass through anything except the screen boundary.
void ecom_updateKnockbackNoSolidity_b0e_hook(GB *gb) {
  BASE(ecom_updateKnockbackNoSolidity_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+4, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e), b_+7);
  CYC(b_+7, b_+9);
  TAIL(ecom_updateKnockback_common_b0e);
}

void ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0e_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b0e_hook, SYM(ecom_updateKnockback_b0e), b_+3);
  CALL_C(b_+3, ecom_checkHazardsNoAnimationForHoles_b0e_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b0e), b_+6);
  RET(b_+6); return;
}

// Like "ecom_checkHazards", but the enemy doesn't animate when they fall into a hole. That is,
// they just get stuck on the last frame of their animation as they get sucked in.
void ecom_checkHazardsNoAnimationForHoles_b0e_hook(GB *gb) {
  BASE(ecom_checkHazardsNoAnimationForHoles_b0e);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+5); mem_wr(gb, hFF8D, A);
  CYC(b_+5, b_+7);
  TAIL(ecom_checkHazardsCommon_b0e);
}

// Standard implementation of "enemy experiencing knockback" state. Also, doesn't "return from
// caller" if it fell in a hazard since it calls "ecom_checkHazards" instead of jumping to it.
void ecom_updateKnockbackAndCheckHazards_b0e_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazards_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b0e_hook, SYM(ecom_updateKnockback_b0e), b_+3);
  CALL_C(b_+3, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+6);
  RET(b_+6); return;
}

// Checks whether the enemy falls into any hazards, and does the appropriate reaction if so.
// If the enemy falls in a hazard, this discards its return address to skip whatever remains in
// the caller.
void ecom_checkHazards_b0e_hook(GB *gb) {
  BASE(ecom_checkHazards_b0e);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+6); mem_wr(gb, hFF8D, A);
  TAIL(ecom_checkHazardsCommon_b0e);
}

void ecom_checkHazardsCommon_b0e_hook(GB *gb) {
  BASE(ecom_checkHazardsCommon_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto applyHazardEffect;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+11, b_+13);
    goto ret_;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x05ff);
  CALL_C(b_+16, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+19);
  CYC(b_+19, b_+22); SET_HL(hazardCollisionTable);
  CALL_C(b_+22, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+25);
  CYC(b_+25, b_+27); B = 0xff;
  if (F & FC) {
    CYCT(b_+27, b_+29);
    goto touchedHazard;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC(0x0501);
  CALL_C(b_+32, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+35);
  CYC(b_+35, b_+38); SET_HL(hazardCollisionTable);
  CALL_C(b_+38, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+41);
  CYC(b_+41, b_+43); B = 0x01;
  if (F & FC) {
    CYCT(b_+43, b_+45);
    goto touchedHazard;
  }
  CYC(b_+43, b_+45);
  CALL_C(b_+45, ecom_updateMovingPlatform_b0e_hook, SYM(ecom_updateMovingPlatform_b0e), b_+48);
ret_:
  CYC(b_+48, b_+50); A = mem_rd(gb, hFF8F);
  CYC(b_+50, b_+51); alu_or(gb, A);
  RET(b_+51); return;
touchedHazard:
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+55, b_+56); E = L;
  CYC(b_+56, b_+57); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x00);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+68, b_+70); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 60);
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+77); A = mem_rd(gb, hFF8D);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_add(gb, B);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
applyHazardEffect:
  // Discard return address: this enemy is about to be deleted.
  SET_HL(POP(b_+83));
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+86, b_+88);
    TAIL(ecom_makeSplashAndDelete_b0e);
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+89, b_+91);
    TAIL(ecom_fallingInHole_b0e);
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93);
  TAIL(ecom_makeLavaSplashAndDelete_b0e);
}

void ecom_makeSplashAndDelete_b0e_hook(GB *gb) {
  BASE(ecom_makeSplashAndDelete_b0e);
  CYC(b_+0, b_+2); B = 0x03; // INTERAC_SPLASH
  CYC(b_+2, b_+4);
  ecom_splashOrLavaTail_b0e_hook(gb);
}

void ecom_makeLavaSplashAndDelete_b0e_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b0e);
  CYC(b_+0, b_+2); B = 0x04; // INTERAC_LAVASPLASH
  ecom_splashOrLavaTail_b0e_hook(gb);
}

void ecom_decNumEnemiesAndDelete_b0e_hook(GB *gb) {
  BASE(ecom_decNumEnemiesAndDelete_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6);
  TAIL(enemyDelete);
}

void ecom_fallDownHoleAndDelete_b0e_hook(GB *gb) {
  BASE(ecom_fallDownHoleAndDelete_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+3);
  CYC(b_+3, b_+5);
  TAIL(ecom_decNumEnemiesAndDelete_b0e);
}

// @param[out] zflag z if enemy is in the center of the hole
static void ecom_fallingInHole_checkInCenterOfHole_b0e(GB *gb) {
  BASE(ecom_fallingInHole_b0e);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+48); mem_wr(gb, hFF8F, A);
  CYC(b_+48, b_+50); alu_add(gb, 0x05);
  CYC(b_+50, b_+52); alu_and(gb, 0xf0);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); mem_wr(gb, hFF8E, A);
  CYC(b_+59, b_+61); alu_and(gb, 0xf0);
  CYC(b_+61, b_+63); alu_add(gb, 0x08);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+65); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); A = mem_rd(gb, hFF8F);
  CYC(b_+68, b_+69); alu_cp(gb, B);
  RET(b_+69); return;
}

// Enemy is currently falling down a hole.
void ecom_fallingInHole_b0e_hook(GB *gb) {
  BASE(ecom_fallingInHole_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    TAIL(ecom_fallDownHoleAndDelete_b0e);
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (F & FZ) {
    CYC(b_+8, b_+10); CYC(b_+10, b_+13); push_effect(gb, b_+13);
    ecom_fallingInHole_checkInCenterOfHole_b0e(gb);
    if (F & FZ) {
      CYCT(b_+13, b_+15);
      TAIL(ecom_fallDownHoleAndDelete_b0e);
    }
    CYC(b_+13, b_+15);
    CALL_C(b_+15, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+18);
    CYC(b_+18, b_+19); C = A;
    CYC(b_+19, b_+21); B = 0x14; // SPEED_80
    CALL_C(b_+21, ecom_applyGivenVelocity_b0e_hook, SYM(ecom_applyGivenVelocity_b0e), b_+24);
  } else {
    CYCT(b_+8, b_+10);
  }
  // If bit 0 of counter2 is set, animate the enemy as it's being sucked toward the hole.
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+27, b_+29); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+35, b_+37);
  } else {
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+38); alu_xor(gb, A);
  }
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43);
  TAIL(enemyAnimate);
}

// Updates enemy's position if he's on a moving platform.
void ecom_updateMovingPlatform_b0e_hook(GB *gb) {
  BASE(ecom_updateMovingPlatform_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0500);
  CALL_C(b_+8, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+11);
  CYC(b_+11, b_+14); SET_HL(SYM(enemyConveyorTilesTable_b0e));
  CALL_C(b_+14, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+17);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); B = 0x14; // SPEED_80
  TAIL(ecom_applyGivenVelocity_b0e);
}

void ecom_applyGivenVelocity_b0e_hook(GB *gb) {
  BASE(ecom_applyGivenVelocity_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0e));
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8A, A);
  PUSH(b_+6, BC);
  CYC(b_+7, b_+8); A = C;
  CALL_C(b_+8, ecom_getAdjacentWallsBitset_b0e_hook, SYM(ecom_getAdjacentWallsBitset_b0e), b_+11);
  SET_BC(POP(b_+11));
  CYC(b_+12, b_+14);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b0e);
}

void ecom_applyVelocityForTopDownEnemy_b0e_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemy_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, ecom_getTopDownAdjacentWallsBitset_b0e_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0e), b_+4);
  CYC(b_+4, b_+6);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0e);
}

void ecom_applyVelocityForTopDownEnemyNoHoles_b0e_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemyNoHoles_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getTopDownAdjacentWallsBitset_b0e_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0e), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0e);
}

void ecom_applyVelocityForSideviewEnemy_b0e_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemy_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  CALL_C((SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e) + 2), ecom_getSideviewAdjacentWallsBitset_b0e_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0e), SYM(ecom_applyVelocityGivenAdjacentWalls_b0e));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0e);
}

void ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemyNoHoles_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0e_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0e), SYM(ecom_applyVelocityGivenAdjacentWalls_b0e));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0e);
}

void ecom_applyVelocityGivenAdjacentWalls_b0e_hook(GB *gb) {
  BASE(ecom_applyVelocityGivenAdjacentWalls_b0e);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE); C = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE); B = A;
  CYC(b_+5, b_+6);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b0e);
}

static void ecom_addAToHl_from_rst_b0e(GB *gb, uint16_t return_address) {
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

static void ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0e(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b0e);
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+132); B = mem_rd(gb, HL);
  CYC(b_+132, b_+133); L = alu_inc8(gb, L);
  CYC(b_+133, b_+134); E = alu_inc8(gb, E);
  CYC(b_+134, b_+135); A = mem_rd(gb, DE);
  CYC(b_+135, b_+136); C = A;
  CYC(b_+136, b_+137); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+139); alu_sub(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+139, b_+141);
    goto capSpeed;
  }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); C = 0x20;
  CYC(b_+143, b_+145); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+148); alu_cp(gb, 0x32);
  if (F & FC) {
    CYCT(b_+148, b_+150);
    goto sharedF9;
  }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+152); C = 0x60;
sharedF9:
  CYC(b_+152, b_+153); A = B;
  CYC(b_+153, b_+154); alu_cp(gb, C);
  if (F & FC) { RET_TAKEN(b_+154); return; }
  CYC(b_+154, b_+155);
capSpeed:
  CYC(b_+155, b_+157); mem_wr(gb, hFF8D, A);
  RET(b_+157); return;
}

void ecom_applyGivenVelocityGivenAdjacentWalls_b0e_hook(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); mem_wr(gb, hFF8C, A);
  CALL_C(b_+3, getPositionOffsetForVelocity_hook, SYM(getPositionOffsetForVelocity), b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hFF8D, A);
component1:
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFF8B);
  CYC(b_+13, b_+15); alu_and(gb, 0x0c);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto checkSlide1;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0e(gb);
  CYC(b_+20, b_+22);
  goto component2;
checkSlide1:
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+24, b_+26);
    goto component2;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_bit(gb, 3, A);
  CYC(b_+28, b_+30); A = mem_rd(gb, hFF8C);
  CYC(b_+30, b_+33); SET_BC(0x0060);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto checkSlideCap1;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); alu_xor(gb, 0x10);
  CYC(b_+37, b_+40); SET_BC(0xffa0);
checkSlideCap1:
  CYC(b_+40, b_+42); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+42, b_+44);
    goto component2;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_X;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_adc(gb, B);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+58); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+58, b_+60);
    goto component2;
  }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+64); mem_wr(gb, hFF8D, A);
component2:
  CYC(b_+64, b_+66); E = ENEMY_BASE + OBJ_X;
  CYC(b_+66, b_+68); L = 0xc2;
  CYC(b_+68, b_+70); A = mem_rd(gb, hFF8B);
  CYC(b_+70, b_+72); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+74);
    goto checkSlide2;
  }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+77); push_effect(gb, b_+77);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0e(gb);
  CYC(b_+77, b_+79);
  goto ret_;
checkSlide2:
  CYC(b_+79, b_+81); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+81, b_+83);
    goto ret_;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); alu_rrca(gb);
  CYC(b_+84, b_+86); A = mem_rd(gb, hFF8C);
  CYC(b_+86, b_+89); SET_BC(0x0060);
  if (!(F & FC)) {
    CYCT(b_+89, b_+91);
    goto checkSlideCap2;
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); alu_sub(gb, 0x10);
  CYC(b_+93, b_+96); SET_BC(0xffa0);
checkSlideCap2:
  CYC(b_+96, b_+98); alu_add(gb, 0x08);
  CYC(b_+98, b_+100); alu_and(gb, 0x1f);
  CYC(b_+100, b_+102); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+102, b_+104);
    goto ret_;
  }
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_add(gb, C);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_adc(gb, B);
  CYC(b_+112, b_+113); mem_wr(gb, DE, A);
  CYC(b_+113, b_+115); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+118, b_+120);
    goto ret_;
  }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+124); mem_wr(gb, hFF8D, A);
ret_:
  CYC(b_+124, b_+126); A = mem_rd(gb, hFF8D);
  CYC(b_+126, b_+127); alu_or(gb, A);
  RET(b_+127); return;
}

void ecom_getTopDownAdjacentWallsBitsetGivenAngle_b0e_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitsetGivenAngle_b0e);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b0e));
  CYC(b_+3, b_+5);
  TAIL(ecom_getAdjacentWallsBitset_b0e);
}

void ecom_getTopDownAdjacentWallsBitset_b0e_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitset_b0e);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+5); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b0e));
  CYC(b_+5, b_+7);
  TAIL(label_025_b0e);
}

void ecom_getSideviewAdjacentWallsBitset_b0e_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitset_b0e);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  TAIL(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e);
}

void ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0e);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0e));
  TAIL(label_025_b0e);
}

void label_025_b0e_hook(GB *gb) {
  BASE(label_025_b0e);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8A, A);
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  TAIL(ecom_getAdjacentWallsBitset_b0e);
}

void ecom_getAdjacentWallsBitset_checkCollisionAt_b0e_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); SET_DE(DE + 1);
  CYC(b_+34, b_+35); alu_add(gb, B); B = A;
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); SET_DE(DE + 1);
  CYC(b_+38, b_+39); alu_add(gb, C); C = A;
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); A = mem_rd(gb, hFF8A);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+43, b_+46);
    TAIL(checkTileCollisionAt_disallowHoles);
  }
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
    goto secondCheck;
  }
  CYC(b_+47, b_+49);
  CALL_C(b_+49, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+52);
  CYC(b_+52, b_+54); alu_add(gb, 0x01);
  RET(b_+54); return;
secondCheck:
  CALL_C(b_+55, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+58);
  CYC(b_+58, b_+60); alu_add(gb, 0x01);
  if (!(F & FC)) {
    CYCT(b_+60, b_+63);
    TAIL(checkTileCollisionAt_allowHoles);
  }
  CYC(b_+60, b_+63);
  RET(b_+63); return;
}

void ecom_getAdjacentWallsBitset_b0e_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, DE);
  CALL_C(b_+1, ecom_getAdjacentWallTableOffset_b0e_hook, SYM(ecom_getAdjacentWallTableOffset_b0e), b_+4);
  CYC(b_+4, b_+5); B = D;
  CYC(b_+5, b_+6);
  ecom_addAToHl_from_rst_b0e(gb, b_+6);
  CYC(b_+6, b_+7); D = H;
  CYC(b_+7, b_+8); E = L;
  CYC(b_+8, b_+9); H = B;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); C = mem_rd(gb, HL);
  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+19); mem_wr(gb, hFF8B, A);
loop:
  CYC(b_+19, b_+22); push_effect(gb, b_+22);
  ecom_getAdjacentWallsBitset_checkCollisionAt_b0e_hook(gb);
  CYC(b_+22, b_+24); A = mem_rd(gb, hFF8B);
  CYC(b_+24, b_+25); alu_rla(gb);
  CYC(b_+25, b_+27); mem_wr(gb, hFF8B, A);
  if (!(F & FC)) {
    CYCT(b_+27, b_+29);
    goto loop;
  }
  CYC(b_+27, b_+29);
  SET_DE(POP(b_+29));
  CYC(b_+30, b_+31); alu_or(gb, A);
  RET(b_+31); return;
}

void ecom_getAdjacentWallTableOffset_b0e_hook(GB *gb) {
  BASE(ecom_getAdjacentWallTableOffset_b0e);
  CYC(b_+0, b_+1); alu_rlca(gb); B = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); alu_and(gb, 0x0f);
  CYC(b_+4, b_+5); A = B;
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+10); alu_add(gb, 0x08);
  RET(b_+10); return;
}

void ecom_bounceOffScreenBoundary__getDirectionsHit_b0e_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0e);
  CYC(b_+43, b_+45); C = 0x00;
  CYC(b_+45, b_+46); B = A;
  CYC(b_+46, b_+48); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+48, b_+50);
    goto afterInc;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); C = alu_inc8(gb, C);
afterInc:
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+54); alu_and(gb, 0x0c);
  if (F & FZ) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); C = (uint8_t)(C | (1 << 2));
  RET(b_+57); return;
}

// A local with an independent external caller (veranFinal_spiderForm_updateMovement); kept
// callable on its own rather than inlined into the bounce common body.
void ecom_bounceOffScreenBoundary__reverseDirection_b0e_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0e);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_add(gb, 0x10);
  CYC(b_+38, b_+40); alu_and(gb, 0x1f);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); alu_or(gb, D);
  RET(b_+42); return;
}

void ecom_bounceOffScreenBoundary_common_b0e_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0e_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0e), b_+5);
  CYC(b_+5, b_+8); push_effect(gb, b_+8);
  ecom_bounceOffScreenBoundary__getDirectionsHit_b0e_hook(gb);
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    ecom_bounceOffScreenBoundary__reverseDirection_b0e_hook(gb); return;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_HL(b_+74);
  CYC(b_+18, b_+20); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto rotate;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(b_+58);
rotate:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29);
  ecom_addAToHl_from_rst_b0e(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_or(gb, D);
  RET(b_+32); return;
}

void ecom_bounceOffWallsAndHoles_b0e_hook(GB *gb) {
  BASE(ecom_bounceOffWallsAndHoles_b0e);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4);
  ecom_bounceOffScreenBoundary_common_b0e_hook(gb);
}

void ecom_bounceOffWalls_b0e_hook(GB *gb) {
  BASE(ecom_bounceOffWalls_b0e);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  ecom_bounceOffScreenBoundary_common_b0e_hook(gb);
}

void ecom_bounceOffScreenBoundary_b0e_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0e);
  CYC(b_+0, b_+2); A = 0x02;
  ecom_bounceOffScreenBoundary_common_b0e_hook(gb);
}

void ecom_randomBitwiseAndBCE_b0e_hook(GB *gb) {
  BASE(ecom_randomBitwiseAndBCE_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, BC);
  CALL_C(b_+1, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+4);
  SET_BC(POP(b_+4));
  CYC(b_+5, b_+6); alu_and(gb, E); E = A;
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); alu_and(gb, B); B = A;
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+12); alu_and(gb, C); C = A;
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  RET(b_+14); return;
}

void ecom_setSpeedAndState8AndVisible_b0e_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8AndVisible_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetVisiblec2);
}

void ecom_setSpeedAndState8_b0e_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8_b0e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);
  RET(b_+8); return;
}

static void ecom_spawnEnemyCommon_b0e_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b0e);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); alu_xor(gb, A);
  RET(b_+8); return;
}

void ecom_spawnUncountedEnemyWithSubid01_b0e_hook(GB *gb) {
  BASE(ecom_spawnUncountedEnemyWithSubid01_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6);
  ecom_spawnEnemyCommon_b0e_hook(gb);
}

void ecom_spawnEnemyWithSubid01_b0e_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  ecom_spawnEnemyCommon_b0e_hook(gb);
}

void ecom_spawnProjectile_b0e_hook(GB *gb) {
  BASE(ecom_spawnProjectile_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CALL_C(b_+5, objectCopyPosition_hook, SYM(objectCopyPosition), b_+8);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+10, b_+12); A = 0x80;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, D);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR18;
  CYC(b_+16, b_+18); A = 0xc0;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+26); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  RET(b_+29); return;
}

void ecom_decCounter1_b0e_hook(GB *gb) {
  BASE(ecom_decCounter1_b0e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_dec16BitCounter_b0e_hook(GB *gb) {
  BASE(ecom_dec16BitCounter_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  TAIL(ecom_decCounter2_b0e);
}

void ecom_decCounter2_b0e_hook(GB *gb) {
  BASE(ecom_decCounter2_b0e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
}

void ecom_updateCardinalAngleAwayFromTarget_b0e_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleAwayFromTarget_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_xor(gb, 0x10);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_updateCardinalAngleTowardTarget_b0e_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleTowardTarget_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

void ecom_updateAngleTowardTarget_b0e_hook(GB *gb) {
  BASE(ecom_updateAngleTowardTarget_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

void ecom_setRandomCardinalAngle_b0e_hook(GB *gb) {
  BASE(ecom_setRandomCardinalAngle_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_setRandomAngle_b0e_hook(GB *gb) {
  BASE(ecom_setRandomAngle_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

// @angleToAnimIndex ($43ff, 32 bytes) is a private lookup table, not code; it is only reached by
// address arithmetic, so it needs no hook entry of its own.
void ecom_updateAnimationFromAngle_b0e_hook(GB *gb) {
  BASE(ecom_updateAnimationFromAngle_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+8); SET_BC(b_+39);
  CALL_C(b_+8, addAToBc_hook, 0x006d, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+14); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+14, b_+16);
    goto setAnimation;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); alu_cp(gb, 0x07);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); alu_sub(gb, 0x03);
  CYC(b_+22, b_+24); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); A = E;
  CYC(b_+26, b_+28); alu_add(gb, 0x04);
  CYC(b_+28, b_+30); alu_and(gb, 0x18);
  CYC(b_+30, b_+32); A = alu_swap(gb, A);
  CYC(b_+32, b_+33); alu_rlca(gb);
setAnimation:
  CYC(b_+33, b_+34); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+39);
  TAIL(enemySetAnimation);
}

void ecom_flickerVisibility_b0e_hook(GB *gb) {
  BASE(ecom_flickerVisibility_b0e);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x80);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

// @param[out] a State
// @param[out] b Subid
// @param[out] cflag c if state < 8
void ecom_getSubidAndCpStateTo08_b0e_hook(GB *gb) {
  BASE(ecom_getSubidAndCpStateTo08_b0e);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE); B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x08);
  RET(b_+9); return;
}

void ecom_moveTowardPosition_b0e_hook(GB *gb) {
  BASE(ecom_moveTowardPosition_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9);
  TAIL(objectApplySpeed);
}

void ecom_readPositionVars_b0e_hook(GB *gb) {
  BASE(ecom_readPositionVars_b0e);
  CYC(b_+0, b_+1); B = mem_rd(gb, HL);
  CYC(b_+1, b_+2); L = alu_inc8(gb, L);
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, hFF8F, A);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); mem_wr(gb, hFF8E, A);
  RET(b_+12); return;
}

void ecom_setZAboveScreen_b0e_hook(GB *gb) {
  BASE(ecom_setZAboveScreen_b0e);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, C);
  CYC(b_+5, b_+6); alu_cpl(gb);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = mem_rd(gb, hCameraY);
  CYC(b_+10, b_+11); alu_add(gb, C);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
    goto checkTop;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = C;
checkTop:
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto storeZ;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); A = 0x80;
storeZ:
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

void ecom_killObjectH_b0e_hook(GB *gb) {
  BASE(ecom_killObjectH_b0e);
  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+3); alu_and(gb, 0xc0);
  CYC(b_+3, b_+5); alu_or(gb, 0x29);
  CYC(b_+5, b_+6); L = A;
  TAIL(ecom_killRelatedObj_b0e);
}

void ecom_killRelatedObj_b0e_hook(GB *gb) {
  BASE(ecom_killRelatedObj_b0e);
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x00);
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+5); alu_add(gb, 0xfb);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(b_+8); return;
}

void ecom_killRelatedObj1_b0e_hook(GB *gb) {
  BASE(ecom_killRelatedObj1_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b0e);
}

void ecom_killRelatedObj2_b0e_hook(GB *gb) {
  BASE(ecom_killRelatedObj2_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b0e);
}

// @oscillationX ($44a8) is a private position-offset lookup table, not code; it needs no hook
// entry of its own.
void ecom_galeSeedEffect_b0e_hook(GB *gb) {
  BASE(ecom_galeSeedEffect_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto zero;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+11); SET_HL(b_+45);
  CYC(b_+11, b_+12);
  ecom_addAToHl_from_rst_b0e(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_scf(gb);
  RET(b_+18); return;
zero:
  CALL_C(b_+19, objectApplySpeed_hook, SYM(objectApplySpeed), b_+22);
  CYC(b_+22, b_+24); C = 0x10;
  CALL_C(b_+24, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+27);
  CYC(b_+27, b_+29); A = mem_rd(gb, hCameraY);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_cp(gb, 0x80);
  CYC(b_+35, b_+36); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+42); alu_sub(gb, B);
  CYC(b_+42, b_+44); alu_cp(gb, 0xb0);
  RET(b_+44); return;
}

void ecom_blownByGaleSeedState_b0e_hook(GB *gb) {
  BASE(ecom_blownByGaleSeedState_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_galeSeedEffect_b0e_hook, SYM(ecom_galeSeedEffect_b0e), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, decNumEnemies_hook, SYM(decNumEnemies), b_+7);
  CYC(b_+7, b_+10);
  TAIL(enemyDelete);
}

void ecom_checkScentSeedActive_b0e_hook(GB *gb) {
  BASE(ecom_checkScentSeedActive_b0e);
  CYC(b_+0, b_+3); A = W8(wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_bit(gb, 4, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_and(gb, 0xf8);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); A = 0x04;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_updateAngleToScentSeed_b0e_hook(GB *gb) {
  BASE(ecom_updateAngleToScentSeed_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR3D;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = mem_rd(gb, hFFB2);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFFB3);
  CYC(b_+13, b_+14); C = A;
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_fallToGroundAndSetState8_b0e_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState8_b0e);
  CYC(b_+0, b_+2); B = 0x08;
  TAIL(ecom_fallToGroundAndSetState_b0e);
}

void ecom_fallToGroundAndSetState_b0e_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState_b0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  RET(b_+13); return;
}


#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

// object_code/common/enemies/commonCode.s, bank $0f instance. Byte-identical addresses to
// the bank $0d/$0e/$10 copies above (same source, same offset from bank start in every
// enemy bank), confirmed via --report; this section is a mechanical rename of the bank-$0d
// copy (itself already fixed for the missing-push and OBJ_X bugs found there).

void ecom_splashOrLavaTail_b0f_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), SYM(ecom_decNumEnemiesAndDelete_b0f));
  TAIL(ecom_decNumEnemiesAndDelete_b0f);
}

void ecom_incState_b0f_hook(GB *gb) {
  BASE(ecom_incState_b0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_incSubstate_b0f_hook(GB *gb) {
  BASE(ecom_incSubstate_b0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// Update knockback where solid tiles are defined "normally".
void ecom_updateKnockback_b0f_hook(GB *gb) {
  BASE(ecom_updateKnockback_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+3, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f), SYM(ecom_updateKnockback_common_b0f));
  TAIL(ecom_updateKnockback_common_b0f);
}

void ecom_updateKnockback_common_b0f_hook(GB *gb) {
  BASE(ecom_updateKnockback_common_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, DE); C = A;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+8); B = 0x50; // SPEED_200
  if (F & FC) {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); B = 0x78; // SPEED_300
    CYC(b_+12, b_+14); alu_and(gb, 0x06);
    if (F & FZ) {
      CYC(b_+14, b_+16);
      PUSH(b_+16, BC);
      CYC(b_+17, b_+20); SET_BC(0x0f01);
      CALL_C(b_+20, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+23);
      SET_BC(POP(b_+23));
    } else {
      CYCT(b_+14, b_+16);
    }
  } else {
    CYCT(b_+8, b_+10);
  }
  CALL_C(b_+24, ecom_applyGivenVelocityGivenAdjacentWalls_b0f_hook, SYM(ecom_applyGivenVelocityGivenAdjacentWalls_b0f), b_+27);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_and(gb, 0x80);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

// Update knockback where the enemy can pass through anything except the screen boundary.
void ecom_updateKnockbackNoSolidity_b0f_hook(GB *gb) {
  BASE(ecom_updateKnockbackNoSolidity_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+4, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f), b_+7);
  CYC(b_+7, b_+9);
  TAIL(ecom_updateKnockback_common_b0f);
}

void ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0f_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b0f_hook, SYM(ecom_updateKnockback_b0f), b_+3);
  CALL_C(b_+3, ecom_checkHazardsNoAnimationForHoles_b0f_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b0f), b_+6);
  RET(b_+6); return;
}

// Like "ecom_checkHazards", but the enemy doesn't animate when they fall into a hole. That is,
// they just get stuck on the last frame of their animation as they get sucked in.
void ecom_checkHazardsNoAnimationForHoles_b0f_hook(GB *gb) {
  BASE(ecom_checkHazardsNoAnimationForHoles_b0f);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+5); mem_wr(gb, hFF8D, A);
  CYC(b_+5, b_+7);
  TAIL(ecom_checkHazardsCommon_b0f);
}

// Standard implementation of "enemy experiencing knockback" state. Also, doesn't "return from
// caller" if it fell in a hazard since it calls "ecom_checkHazards" instead of jumping to it.
void ecom_updateKnockbackAndCheckHazards_b0f_hook(GB *gb) {
  BASE(ecom_updateKnockbackAndCheckHazards_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_updateKnockback_b0f_hook, SYM(ecom_updateKnockback_b0f), b_+3);
  CALL_C(b_+3, ecom_checkHazards_b0f_hook, SYM(ecom_checkHazards_b0f), b_+6);
  RET(b_+6); return;
}

// Checks whether the enemy falls into any hazards, and does the appropriate reaction if so.
// If the enemy falls in a hazard, this discards its return address to skip whatever remains in
// the caller.
void ecom_checkHazards_b0f_hook(GB *gb) {
  BASE(ecom_checkHazards_b0f);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8F, A);
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+6); mem_wr(gb, hFF8D, A);
  TAIL(ecom_checkHazardsCommon_b0f);
}

void ecom_checkHazardsCommon_b0f_hook(GB *gb) {
  BASE(ecom_checkHazardsCommon_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto applyHazardEffect;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+11, b_+13);
    goto ret_;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x05ff);
  CALL_C(b_+16, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+19);
  CYC(b_+19, b_+22); SET_HL(hazardCollisionTable);
  CALL_C(b_+22, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+25);
  CYC(b_+25, b_+27); B = 0xff;
  if (F & FC) {
    CYCT(b_+27, b_+29);
    goto touchedHazard;
  }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC(0x0501);
  CALL_C(b_+32, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+35);
  CYC(b_+35, b_+38); SET_HL(hazardCollisionTable);
  CALL_C(b_+38, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+41);
  CYC(b_+41, b_+43); B = 0x01;
  if (F & FC) {
    CYCT(b_+43, b_+45);
    goto touchedHazard;
  }
  CYC(b_+43, b_+45);
  CALL_C(b_+45, ecom_updateMovingPlatform_b0f_hook, SYM(ecom_updateMovingPlatform_b0f), b_+48);
ret_:
  CYC(b_+48, b_+50); A = mem_rd(gb, hFF8F);
  CYC(b_+50, b_+51); alu_or(gb, A);
  RET(b_+51); return;
touchedHazard:
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+55, b_+56); E = L;
  CYC(b_+56, b_+57); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x00);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+68, b_+70); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 60);
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+77); A = mem_rd(gb, hFF8D);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_add(gb, B);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
applyHazardEffect:
  // Discard return address: this enemy is about to be deleted.
  SET_HL(POP(b_+83));
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+86, b_+88);
    TAIL(ecom_makeSplashAndDelete_b0f);
  }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+89, b_+91);
    TAIL(ecom_fallingInHole_b0f);
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93);
  TAIL(ecom_makeLavaSplashAndDelete_b0f);
}

void ecom_makeSplashAndDelete_b0f_hook(GB *gb) {
  BASE(ecom_makeSplashAndDelete_b0f);
  CYC(b_+0, b_+2); B = 0x03; // INTERAC_SPLASH
  CYC(b_+2, b_+4);
  ecom_splashOrLavaTail_b0f_hook(gb);
}

void ecom_makeLavaSplashAndDelete_b0f_hook(GB *gb) {
  BASE(ecom_makeLavaSplashAndDelete_b0f);
  CYC(b_+0, b_+2); B = 0x04; // INTERAC_LAVASPLASH
  ecom_splashOrLavaTail_b0f_hook(gb);
}

void ecom_decNumEnemiesAndDelete_b0f_hook(GB *gb) {
  BASE(ecom_decNumEnemiesAndDelete_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6);
  TAIL(enemyDelete);
}

void ecom_fallDownHoleAndDelete_b0f_hook(GB *gb) {
  BASE(ecom_fallDownHoleAndDelete_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+3);
  CYC(b_+3, b_+5);
  TAIL(ecom_decNumEnemiesAndDelete_b0f);
}

// @param[out] zflag z if enemy is in the center of the hole
static void ecom_fallingInHole_checkInCenterOfHole_b0f(GB *gb) {
  BASE(ecom_fallingInHole_b0f);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+48); mem_wr(gb, hFF8F, A);
  CYC(b_+48, b_+50); alu_add(gb, 0x05);
  CYC(b_+50, b_+52); alu_and(gb, 0xf0);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+59); mem_wr(gb, hFF8E, A);
  CYC(b_+59, b_+61); alu_and(gb, 0xf0);
  CYC(b_+61, b_+63); alu_add(gb, 0x08);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+65); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); A = mem_rd(gb, hFF8F);
  CYC(b_+68, b_+69); alu_cp(gb, B);
  RET(b_+69); return;
}

// Enemy is currently falling down a hole.
void ecom_fallingInHole_b0f_hook(GB *gb) {
  BASE(ecom_fallingInHole_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    TAIL(ecom_fallDownHoleAndDelete_b0f);
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (F & FZ) {
    CYC(b_+8, b_+10); CYC(b_+10, b_+13); push_effect(gb, b_+13);
    ecom_fallingInHole_checkInCenterOfHole_b0f(gb);
    if (F & FZ) {
      CYCT(b_+13, b_+15);
      TAIL(ecom_fallDownHoleAndDelete_b0f);
    }
    CYC(b_+13, b_+15);
    CALL_C(b_+15, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+18);
    CYC(b_+18, b_+19); C = A;
    CYC(b_+19, b_+21); B = 0x14; // SPEED_80
    CALL_C(b_+21, ecom_applyGivenVelocity_b0f_hook, SYM(ecom_applyGivenVelocity_b0f), b_+24);
  } else {
    CYCT(b_+8, b_+10);
  }
  // If bit 0 of counter2 is set, animate the enemy as it's being sucked toward the hole.
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+27, b_+29); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+35, b_+37);
  } else {
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+38); alu_xor(gb, A);
  }
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43);
  TAIL(enemyAnimate);
}

// Updates enemy's position if he's on a moving platform.
void ecom_updateMovingPlatform_b0f_hook(GB *gb) {
  BASE(ecom_updateMovingPlatform_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0500);
  CALL_C(b_+8, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+11);
  CYC(b_+11, b_+14); SET_HL(SYM(enemyConveyorTilesTable_b0f));
  CALL_C(b_+14, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+17);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); B = 0x14; // SPEED_80
  TAIL(ecom_applyGivenVelocity_b0f);
}

void ecom_applyGivenVelocity_b0f_hook(GB *gb) {
  BASE(ecom_applyGivenVelocity_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0f));
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8A, A);
  PUSH(b_+6, BC);
  CYC(b_+7, b_+8); A = C;
  CALL_C(b_+8, ecom_getAdjacentWallsBitset_b0f_hook, SYM(ecom_getAdjacentWallsBitset_b0f), b_+11);
  SET_BC(POP(b_+11));
  CYC(b_+12, b_+14);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b0f);
}

void ecom_applyVelocityForTopDownEnemy_b0f_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemy_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CALL_C(b_+1, ecom_getTopDownAdjacentWallsBitset_b0f_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0f), b_+4);
  CYC(b_+4, b_+6);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0f);
}

void ecom_applyVelocityForTopDownEnemyNoHoles_b0f_hook(GB *gb) {
  BASE(ecom_applyVelocityForTopDownEnemyNoHoles_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getTopDownAdjacentWallsBitset_b0f_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0f), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0f);
}

void ecom_applyVelocityForSideviewEnemy_b0f_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemy_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  CALL_C((SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0f) + 2), ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), SYM(ecom_applyVelocityGivenAdjacentWalls_b0f));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0f);
}

void ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook(GB *gb) {
  BASE(ecom_applyVelocityForSideviewEnemyNoHoles_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), SYM(ecom_applyVelocityGivenAdjacentWalls_b0f));
  TAIL(ecom_applyVelocityGivenAdjacentWalls_b0f);
}

void ecom_applyVelocityGivenAdjacentWalls_b0f_hook(GB *gb) {
  BASE(ecom_applyVelocityGivenAdjacentWalls_b0f);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE); C = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE); B = A;
  CYC(b_+5, b_+6);
  TAIL(ecom_applyGivenVelocityGivenAdjacentWalls_b0f);
}

static void ecom_addAToHl_from_rst_b0f(GB *gb, uint16_t return_address) {
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

static void ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0f(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b0f);
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+130, b_+131); mem_wr(gb, DE, A);
  CYC(b_+131, b_+132); B = mem_rd(gb, HL);
  CYC(b_+132, b_+133); L = alu_inc8(gb, L);
  CYC(b_+133, b_+134); E = alu_inc8(gb, E);
  CYC(b_+134, b_+135); A = mem_rd(gb, DE);
  CYC(b_+135, b_+136); C = A;
  CYC(b_+136, b_+137); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+139); alu_sub(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+139, b_+141);
    goto capSpeed;
  }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); C = 0x20;
  CYC(b_+143, b_+145); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+148); alu_cp(gb, 0x32);
  if (F & FC) {
    CYCT(b_+148, b_+150);
    goto sharedF9;
  }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+152); C = 0x60;
sharedF9:
  CYC(b_+152, b_+153); A = B;
  CYC(b_+153, b_+154); alu_cp(gb, C);
  if (F & FC) { RET_TAKEN(b_+154); return; }
  CYC(b_+154, b_+155);
capSpeed:
  CYC(b_+155, b_+157); mem_wr(gb, hFF8D, A);
  RET(b_+157); return;
}

void ecom_applyGivenVelocityGivenAdjacentWalls_b0f_hook(GB *gb) {
  BASE(ecom_applyGivenVelocityGivenAdjacentWalls_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); mem_wr(gb, hFF8C, A);
  CALL_C(b_+3, getPositionOffsetForVelocity_hook, SYM(getPositionOffsetForVelocity), b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, hFF8D, A);
component1:
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFF8B);
  CYC(b_+13, b_+15); alu_and(gb, 0x0c);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    goto checkSlide1;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0f(gb);
  CYC(b_+20, b_+22);
  goto component2;
checkSlide1:
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(b_+24, b_+26);
    goto component2;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); alu_bit(gb, 3, A);
  CYC(b_+28, b_+30); A = mem_rd(gb, hFF8C);
  CYC(b_+30, b_+33); SET_BC(0x0060);
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35);
    goto checkSlideCap1;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); alu_xor(gb, 0x10);
  CYC(b_+37, b_+40); SET_BC(0xffa0);
checkSlideCap1:
  CYC(b_+40, b_+42); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+42, b_+44);
    goto component2;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_X;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); E = alu_inc8(gb, E);
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_adc(gb, B);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+58); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+58, b_+60);
    goto component2;
  }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+64); mem_wr(gb, hFF8D, A);
component2:
  CYC(b_+64, b_+66); E = ENEMY_BASE + OBJ_X;
  CYC(b_+66, b_+68); L = 0xc2;
  CYC(b_+68, b_+70); A = mem_rd(gb, hFF8B);
  CYC(b_+70, b_+72); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+74);
    goto checkSlide2;
  }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+77); push_effect(gb, b_+77);
  ecom_applyGivenVelocityGivenAdjacentWalls_applySpeedComponent_b0f(gb);
  CYC(b_+77, b_+79);
  goto ret_;
checkSlide2:
  CYC(b_+79, b_+81); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+81, b_+83);
    goto ret_;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); alu_rrca(gb);
  CYC(b_+84, b_+86); A = mem_rd(gb, hFF8C);
  CYC(b_+86, b_+89); SET_BC(0x0060);
  if (!(F & FC)) {
    CYCT(b_+89, b_+91);
    goto checkSlideCap2;
  }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); alu_sub(gb, 0x10);
  CYC(b_+93, b_+96); SET_BC(0xffa0);
checkSlideCap2:
  CYC(b_+96, b_+98); alu_add(gb, 0x08);
  CYC(b_+98, b_+100); alu_and(gb, 0x1f);
  CYC(b_+100, b_+102); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+102, b_+104);
    goto ret_;
  }
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_Y;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_add(gb, C);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_adc(gb, B);
  CYC(b_+112, b_+113); mem_wr(gb, DE, A);
  CYC(b_+113, b_+115); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); alu_cp(gb, 0x32);
  if (!(F & FC)) {
    CYCT(b_+118, b_+120);
    goto ret_;
  }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+124); mem_wr(gb, hFF8D, A);
ret_:
  CYC(b_+124, b_+126); A = mem_rd(gb, hFF8D);
  CYC(b_+126, b_+127); alu_or(gb, A);
  RET(b_+127); return;
}

void ecom_getTopDownAdjacentWallsBitsetGivenAngle_b0f_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitsetGivenAngle_b0f);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b0f));
  CYC(b_+3, b_+5);
  TAIL(ecom_getAdjacentWallsBitset_b0f);
}

void ecom_getTopDownAdjacentWallsBitset_b0f_hook(GB *gb) {
  BASE(ecom_getTopDownAdjacentWallsBitset_b0f);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+5); SET_HL(SYM(ecom_topDownAdjacentWallOffsetTable_b0f));
  CYC(b_+5, b_+7);
  TAIL(label_025_b0f);
}

void ecom_getSideviewAdjacentWallsBitset_b0f_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitset_b0f);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  TAIL(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f);
}

void ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f_hook(GB *gb) {
  BASE(ecom_getSideviewAdjacentWallsBitsetGivenAngle_b0f);
  CYC(b_+0, b_+3); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0f));
  TAIL(label_025_b0f);
}

void label_025_b0f_hook(GB *gb) {
  BASE(label_025_b0f);
  CYC(b_+0, b_+2); mem_wr(gb, hFF8A, A);
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  TAIL(ecom_getAdjacentWallsBitset_b0f);
}

void ecom_getAdjacentWallsBitset_checkCollisionAt_b0f_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); SET_DE(DE + 1);
  CYC(b_+34, b_+35); alu_add(gb, B); B = A;
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); SET_DE(DE + 1);
  CYC(b_+38, b_+39); alu_add(gb, C); C = A;
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); A = mem_rd(gb, hFF8A);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+43, b_+46);
    TAIL(checkTileCollisionAt_disallowHoles);
  }
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+47, b_+49);
    goto secondCheck;
  }
  CYC(b_+47, b_+49);
  CALL_C(b_+49, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+52);
  CYC(b_+52, b_+54); alu_add(gb, 0x01);
  RET(b_+54); return;
secondCheck:
  CALL_C(b_+55, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+58);
  CYC(b_+58, b_+60); alu_add(gb, 0x01);
  if (!(F & FC)) {
    CYCT(b_+60, b_+63);
    TAIL(checkTileCollisionAt_allowHoles);
  }
  CYC(b_+60, b_+63);
  RET(b_+63); return;
}

void ecom_getAdjacentWallsBitset_b0f_hook(GB *gb) {
  BASE(ecom_getAdjacentWallsBitset_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, DE);
  CALL_C(b_+1, ecom_getAdjacentWallTableOffset_b0f_hook, SYM(ecom_getAdjacentWallTableOffset_b0f), b_+4);
  CYC(b_+4, b_+5); B = D;
  CYC(b_+5, b_+6);
  ecom_addAToHl_from_rst_b0f(gb, b_+6);
  CYC(b_+6, b_+7); D = H;
  CYC(b_+7, b_+8); E = L;
  CYC(b_+8, b_+9); H = B;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); C = mem_rd(gb, HL);
  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+19); mem_wr(gb, hFF8B, A);
loop:
  CYC(b_+19, b_+22); push_effect(gb, b_+22);
  ecom_getAdjacentWallsBitset_checkCollisionAt_b0f_hook(gb);
  CYC(b_+22, b_+24); A = mem_rd(gb, hFF8B);
  CYC(b_+24, b_+25); alu_rla(gb);
  CYC(b_+25, b_+27); mem_wr(gb, hFF8B, A);
  if (!(F & FC)) {
    CYCT(b_+27, b_+29);
    goto loop;
  }
  CYC(b_+27, b_+29);
  SET_DE(POP(b_+29));
  CYC(b_+30, b_+31); alu_or(gb, A);
  RET(b_+31); return;
}

void ecom_getAdjacentWallTableOffset_b0f_hook(GB *gb) {
  BASE(ecom_getAdjacentWallTableOffset_b0f);
  CYC(b_+0, b_+1); alu_rlca(gb); B = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+4); alu_and(gb, 0x0f);
  CYC(b_+4, b_+5); A = B;
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+10); alu_add(gb, 0x08);
  RET(b_+10); return;
}

void ecom_bounceOffScreenBoundary__getDirectionsHit_b0f_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0f);
  CYC(b_+43, b_+45); C = 0x00;
  CYC(b_+45, b_+46); B = A;
  CYC(b_+46, b_+48); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+48, b_+50);
    goto afterInc;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); C = alu_inc8(gb, C);
afterInc:
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+54); alu_and(gb, 0x0c);
  if (F & FZ) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); C = (uint8_t)(C | (1 << 2));
  RET(b_+57); return;
}

// A local with an independent external caller (veranFinal_spiderForm_updateMovement); kept
// callable on its own rather than inlined into the bounce common body.
void ecom_bounceOffScreenBoundary__reverseDirection_b0f_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0f);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_add(gb, 0x10);
  CYC(b_+38, b_+40); alu_and(gb, 0x1f);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); alu_or(gb, D);
  RET(b_+42); return;
}

void ecom_bounceOffScreenBoundary_common_b0f_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), b_+5);
  CYC(b_+5, b_+8); push_effect(gb, b_+8);
  ecom_bounceOffScreenBoundary__getDirectionsHit_b0f_hook(gb);
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    ecom_bounceOffScreenBoundary__reverseDirection_b0f_hook(gb); return;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_HL(b_+74);
  CYC(b_+18, b_+20); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto rotate;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_HL(b_+58);
rotate:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29);
  ecom_addAToHl_from_rst_b0f(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_or(gb, D);
  RET(b_+32); return;
}

void ecom_bounceOffWallsAndHoles_b0f_hook(GB *gb) {
  BASE(ecom_bounceOffWallsAndHoles_b0f);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4);
  ecom_bounceOffScreenBoundary_common_b0f_hook(gb);
}

void ecom_bounceOffWalls_b0f_hook(GB *gb) {
  BASE(ecom_bounceOffWalls_b0f);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3);
  ecom_bounceOffScreenBoundary_common_b0f_hook(gb);
}

void ecom_bounceOffScreenBoundary_b0f_hook(GB *gb) {
  BASE(ecom_bounceOffScreenBoundary_b0f);
  CYC(b_+0, b_+2); A = 0x02;
  ecom_bounceOffScreenBoundary_common_b0f_hook(gb);
}

void ecom_randomBitwiseAndBCE_b0f_hook(GB *gb) {
  BASE(ecom_randomBitwiseAndBCE_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, BC);
  CALL_C(b_+1, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+4);
  SET_BC(POP(b_+4));
  CYC(b_+5, b_+6); alu_and(gb, E); E = A;
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); alu_and(gb, B); B = A;
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+12); alu_and(gb, C); C = A;
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  RET(b_+14); return;
}

void ecom_setSpeedAndState8AndVisible_b0f_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8AndVisible_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetVisiblec2);
}

void ecom_setSpeedAndState8_b0f_hook(GB *gb) {
  BASE(ecom_setSpeedAndState8_b0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);
  RET(b_+8); return;
}

static void ecom_spawnEnemyCommon_b0f_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b0f);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); alu_xor(gb, A);
  RET(b_+8); return;
}

void ecom_spawnUncountedEnemyWithSubid01_b0f_hook(GB *gb) {
  BASE(ecom_spawnUncountedEnemyWithSubid01_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6);
  ecom_spawnEnemyCommon_b0f_hook(gb);
}

void ecom_spawnEnemyWithSubid01_b0f_hook(GB *gb) {
  BASE(ecom_spawnEnemyWithSubid01_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  ecom_spawnEnemyCommon_b0f_hook(gb);
}

void ecom_spawnProjectile_b0f_hook(GB *gb) {
  BASE(ecom_spawnProjectile_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CALL_C(b_+5, objectCopyPosition_hook, SYM(objectCopyPosition), b_+8);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+10, b_+12); A = 0x80;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, D);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR18;
  CYC(b_+16, b_+18); A = 0xc0;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+26); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  RET(b_+29); return;
}

void ecom_decCounter1_b0f_hook(GB *gb) {
  BASE(ecom_decCounter1_b0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void ecom_dec16BitCounter_b0f_hook(GB *gb) {
  BASE(ecom_dec16BitCounter_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  TAIL(ecom_decCounter2_b0f);
}

void ecom_decCounter2_b0f_hook(GB *gb) {
  BASE(ecom_decCounter2_b0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return;
}

void ecom_updateCardinalAngleAwayFromTarget_b0f_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleAwayFromTarget_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_xor(gb, 0x10);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_updateCardinalAngleTowardTarget_b0f_hook(GB *gb) {
  BASE(ecom_updateCardinalAngleTowardTarget_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

void ecom_updateAngleTowardTarget_b0f_hook(GB *gb) {
  BASE(ecom_updateAngleTowardTarget_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

void ecom_setRandomCardinalAngle_b0f_hook(GB *gb) {
  BASE(ecom_setRandomCardinalAngle_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void ecom_setRandomAngle_b0f_hook(GB *gb) {
  BASE(ecom_setRandomAngle_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

// @angleToAnimIndex ($43ff, 32 bytes) is a private lookup table, not code; it is only reached by
// address arithmetic, so it needs no hook entry of its own.
void ecom_updateAnimationFromAngle_b0f_hook(GB *gb) {
  BASE(ecom_updateAnimationFromAngle_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+8); SET_BC(b_+39);
  CALL_C(b_+8, addAToBc_hook, 0x006d, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+14); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+14, b_+16);
    goto setAnimation;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); alu_cp(gb, 0x07);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); alu_sub(gb, 0x03);
  CYC(b_+22, b_+24); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); A = E;
  CYC(b_+26, b_+28); alu_add(gb, 0x04);
  CYC(b_+28, b_+30); alu_and(gb, 0x18);
  CYC(b_+30, b_+32); A = alu_swap(gb, A);
  CYC(b_+32, b_+33); alu_rlca(gb);
setAnimation:
  CYC(b_+33, b_+34); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);
  CYC(b_+36, b_+39);
  TAIL(enemySetAnimation);
}

void ecom_flickerVisibility_b0f_hook(GB *gb) {
  BASE(ecom_flickerVisibility_b0f);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x80);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

// @param[out] a State
// @param[out] b Subid
// @param[out] cflag c if state < 8
void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb) {
  BASE(ecom_getSubidAndCpStateTo08_b0f);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE); B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x08);
  RET(b_+9); return;
}

void ecom_moveTowardPosition_b0f_hook(GB *gb) {
  BASE(ecom_moveTowardPosition_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9);
  TAIL(objectApplySpeed);
}

void ecom_readPositionVars_b0f_hook(GB *gb) {
  BASE(ecom_readPositionVars_b0f);
  CYC(b_+0, b_+1); B = mem_rd(gb, HL);
  CYC(b_+1, b_+2); L = alu_inc8(gb, L);
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, hFF8F, A);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); mem_wr(gb, hFF8E, A);
  RET(b_+12); return;
}

void ecom_setZAboveScreen_b0f_hook(GB *gb) {
  BASE(ecom_setZAboveScreen_b0f);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, C);
  CYC(b_+5, b_+6); alu_cpl(gb);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = mem_rd(gb, hCameraY);
  CYC(b_+10, b_+11); alu_add(gb, C);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
    goto checkTop;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = C;
checkTop:
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto storeZ;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); A = 0x80;
storeZ:
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23); return;
}

void ecom_killObjectH_b0f_hook(GB *gb) {
  BASE(ecom_killObjectH_b0f);
  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+3); alu_and(gb, 0xc0);
  CYC(b_+3, b_+5); alu_or(gb, 0x29);
  CYC(b_+5, b_+6); L = A;
  TAIL(ecom_killRelatedObj_b0f);
}

void ecom_killRelatedObj_b0f_hook(GB *gb) {
  BASE(ecom_killRelatedObj_b0f);
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x00);
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+5); alu_add(gb, 0xfb);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  RET(b_+8); return;
}

void ecom_killRelatedObj1_b0f_hook(GB *gb) {
  BASE(ecom_killRelatedObj1_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b0f);
}

void ecom_killRelatedObj2_b0f_hook(GB *gb) {
  BASE(ecom_killRelatedObj2_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7);
  TAIL(ecom_killRelatedObj_b0f);
}

// @oscillationX ($44a8) is a private position-offset lookup table, not code; it needs no hook
// entry of its own.
void ecom_galeSeedEffect_b0f_hook(GB *gb) {
  BASE(ecom_galeSeedEffect_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto zero;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  CYC(b_+8, b_+11); SET_HL(b_+45);
  CYC(b_+11, b_+12);
  ecom_addAToHl_from_rst_b0f(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_scf(gb);
  RET(b_+18); return;
zero:
  CALL_C(b_+19, objectApplySpeed_hook, SYM(objectApplySpeed), b_+22);
  CYC(b_+22, b_+24); C = 0x10;
  CALL_C(b_+24, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+27);
  CYC(b_+27, b_+29); A = mem_rd(gb, hCameraY);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_cp(gb, 0x80);
  CYC(b_+35, b_+36); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+42); alu_sub(gb, B);
  CYC(b_+42, b_+44); alu_cp(gb, 0xb0);
  RET(b_+44); return;
}

void ecom_blownByGaleSeedState_b0f_hook(GB *gb) {
  BASE(ecom_blownByGaleSeedState_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_galeSeedEffect_b0f_hook, SYM(ecom_galeSeedEffect_b0f), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, decNumEnemies_hook, SYM(decNumEnemies), b_+7);
  CYC(b_+7, b_+10);
  TAIL(enemyDelete);
}

void ecom_checkScentSeedActive_b0f_hook(GB *gb) {
  BASE(ecom_checkScentSeedActive_b0f);
  CYC(b_+0, b_+3); A = W8(wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_bit(gb, 4, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_and(gb, 0xf8);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); A = 0x04;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_updateAngleToScentSeed_b0f_hook(GB *gb) {
  BASE(ecom_updateAngleToScentSeed_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR3D;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = mem_rd(gb, hFFB2);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFFB3);
  CYC(b_+13, b_+14); C = A;
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void ecom_fallToGroundAndSetState8_b0f_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState8_b0f);
  CYC(b_+0, b_+2); B = 0x08;
  TAIL(ecom_fallToGroundAndSetState_b0f);
}

void ecom_fallToGroundAndSetState_b0f_hook(GB *gb) {
  BASE(ecom_fallToGroundAndSetState_b0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  RET(b_+13); return;
}

