#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/common/enemies/commonCode.s, bank $10 instance (Twinrova/Ganon/Veran's Final
// Form/Ramrock/King Moblin's minion). Included at the start of every enemy code bank; the same
// source produces separate hooks per bank (_b0d.._b10) because each bank has its own ROM bytes.

void ecom_splashOrLavaTail_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x40cf, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x40d2);
  ecom_decNumEnemiesAndDelete_b10_hook(gb);
}

void ecom_incState_b10_hook(GB *gb) {
  CYC(0x4000, 0x4001); H = D;
  CYC(0x4001, 0x4003); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4003, 0x4004); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x4004); return;
}

void ecom_incSubstate_b10_hook(GB *gb) {
  CYC(0x4005, 0x4006); H = D;
  CYC(0x4006, 0x4008); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4008, 0x4009); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x4009); return;
}

// Update knockback where solid tiles are defined "normally".
void ecom_updateKnockback_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x400a, 0x400b); alu_xor(gb, A);
  CYC(0x400b, 0x400d); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(0x400d, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10, 0x420d, 0x4010);
  ecom_updateKnockback_common_b10_hook(gb);
}

void ecom_updateKnockback_common_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4010, 0x4012); A = mem_rd(gb, DE); C = A;
  CYC(0x4012, 0x4014); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x4014, 0x4015); A = mem_rd(gb, DE);
  CYC(0x4015, 0x4016); alu_rlca(gb);
  CYC(0x4016, 0x4018); B = 0x50; // SPEED_200
  if (F & FC) {
    CYC(0x4018, 0x401a);
    CYC(0x401a, 0x401c); B = 0x78; // SPEED_300
    CYC(0x401c, 0x401e); alu_and(gb, 0x06);
    if (F & FZ) {
      CYC(0x401e, 0x4020);
      PUSH(0x4020, BC);
      CYC(0x4021, 0x4024); SET_BC(0x0f01);
      CALL_C(0x4024, objectCreateInteraction_hook, 0x24c5, 0x4027);
      SET_BC(POP(0x4027));
    } else {
      CYCT(0x401e, 0x4020);
    }
  } else {
    CYCT(0x4018, 0x401a);
  }
  CALL_C(0x4028, ecom_applyGivenVelocityGivenAdjacentWalls_b10, 0x4161, 0x402b);
  if (!(F & FZ)) { RET_TAKEN(0x402b); return; }
  CYC(0x402b, 0x402c);
  CYC(0x402c, 0x402e); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x402e, 0x402f); A = mem_rd(gb, DE);
  CYC(0x402f, 0x4031); alu_and(gb, 0x80);
  CYC(0x4031, 0x4032); mem_wr(gb, DE, A);
  RET(0x4032); return;
}

// Update knockback where the enemy can pass through anything except the screen boundary.
void ecom_updateKnockbackNoSolidity_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4033, 0x4035); A = 0x02;
  CYC(0x4035, 0x4037); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(0x4037, ecom_getSideviewAdjacentWallsBitsetGivenAngle_b10, 0x420d, 0x403a);
  CYC(0x403a, 0x403c);
  ecom_updateKnockback_common_b10_hook(gb);
}

void ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x403c, ecom_updateKnockback_b10_hook, 0x400a, 0x403f);
  CALL_C(0x403f, ecom_checkHazardsNoAnimationForHoles_b10_hook, 0x4043, 0x4042);
  RET(0x4042); return;
}

// Like "ecom_checkHazards", but the enemy doesn't animate when they fall into a hole. That is,
// they just get stuck on the last frame of their animation as they get sucked in.
void ecom_checkHazardsNoAnimationForHoles_b10_hook(GB *gb) {
  CYC(0x4043, 0x4045); hram_wr(gb, 0x8f, A);
  CYC(0x4045, 0x4046); alu_xor(gb, A);
  CYC(0x4046, 0x4048); hram_wr(gb, 0x8d, A);
  CYC(0x4048, 0x404a);
  ecom_checkHazardsCommon_b10_hook(gb);
}

// Standard implementation of "enemy experiencing knockback" state. Also, doesn't "return from
// caller" if it fell in a hazard since it calls "ecom_checkHazards" instead of jumping to it.
void ecom_updateKnockbackAndCheckHazards_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x404a, ecom_updateKnockback_b10_hook, 0x400a, 0x404d);
  CALL_C(0x404d, ecom_checkHazards_b10_hook, 0x4051, 0x4050);
  RET(0x4050); return;
}

// Checks whether the enemy falls into any hazards, and does the appropriate reaction if so.
// If the enemy falls in a hazard, this discards its return address to skip whatever remains in
// the caller.
void ecom_checkHazards_b10_hook(GB *gb) {
  CYC(0x4051, 0x4053); hram_wr(gb, 0x8f, A);
  CYC(0x4053, 0x4055); A = 0x01;
  CYC(0x4055, 0x4057); hram_wr(gb, 0x8d, A);
  ecom_checkHazardsCommon_b10_hook(gb);
}

void ecom_checkHazardsCommon_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4057, 0x4059); E = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x4059, 0x405a); A = mem_rd(gb, DE);
  CYC(0x405a, 0x405c); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x405c, 0x405e);
    goto applyHazardEffect;
  }
  CYC(0x405c, 0x405e);
  CYC(0x405e, 0x4060); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x4060, 0x4061); A = mem_rd(gb, DE);
  CYC(0x4061, 0x4062); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x4062, 0x4064);
    goto ret_;
  }
  CYC(0x4062, 0x4064);
  CYC(0x4064, 0x4067); SET_BC(0x05ff);
  CALL_C(0x4067, objectGetRelativeTile_hook, 0x1435, 0x406a);
  CYC(0x406a, 0x406d); SET_HL(hazardCollisionTable);
  CALL_C(0x406d, lookupCollisionTable_hook, 0x1e1f, 0x4070);
  CYC(0x4070, 0x4072); B = 0xff;
  if (F & FC) {
    CYCT(0x4072, 0x4074);
    goto touchedHazard;
  }
  CYC(0x4072, 0x4074);
  CYC(0x4074, 0x4077); SET_BC(0x0501);
  CALL_C(0x4077, objectGetRelativeTile_hook, 0x1435, 0x407a);
  CYC(0x407a, 0x407d); SET_HL(hazardCollisionTable);
  CALL_C(0x407d, lookupCollisionTable_hook, 0x1e1f, 0x4080);
  CYC(0x4080, 0x4082); B = 0x01;
  if (F & FC) {
    CYCT(0x4082, 0x4084);
    goto touchedHazard;
  }
  CYC(0x4082, 0x4084);
  CALL_C(0x4084, ecom_updateMovingPlatform_b10, 0x4123, 0x4087);
ret_:
  CYC(0x4087, 0x4089); A = hram_rd(gb, 0x8f);
  CYC(0x4089, 0x408a); alu_or(gb, A);
  RET(0x408a); return;
touchedHazard:
  CYC(0x408b, 0x408c); H = D;
  CYC(0x408c, 0x408e); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x408e, 0x408f); E = L;
  CYC(0x408f, 0x4090); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4090, 0x4091); mem_wr(gb, HL, A);
  CYC(0x4091, 0x4093); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x4093, 0x4095); mem_wr(gb, HL, 0x00);
  CYC(0x4095, 0x4097); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x4097, 0x4099); mem_wr(gb, HL, 0x00);
  CYC(0x4099, 0x409b); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x409b, 0x409d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x409d, 0x409f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x409f, 0x40a1); mem_wr(gb, HL, 60);
  CYC(0x40a1, 0x40a2); L = alu_inc8(gb, L);
  CYC(0x40a2, 0x40a4); A = hram_rd(gb, 0x8d);
  CYC(0x40a4, 0x40a5); mem_wr(gb, HL, A);
  CYC(0x40a5, 0x40a7); L = ENEMY_BASE + OBJ_XH;
  CYC(0x40a7, 0x40a8); A = mem_rd(gb, HL);
  CYC(0x40a8, 0x40a9); alu_add(gb, B);
  CYC(0x40a9, 0x40aa); mem_wr(gb, HL, A);
applyHazardEffect:
  // Discard return address: this enemy is about to be deleted.
  SET_HL(POP(0x40aa));
  CYC(0x40ab, 0x40ac); A = mem_rd(gb, DE);
  CYC(0x40ac, 0x40ad); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x40ad, 0x40af);
    ecom_makeSplashAndDelete_b10_hook(gb);
    return;
  }
  CYC(0x40ad, 0x40af);
  CYC(0x40af, 0x40b0); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x40b0, 0x40b2);
    ecom_fallingInHole_b10_hook(gb);
    return;
  }
  CYC(0x40b0, 0x40b2);
  CYC(0x40b2, 0x40b4);
  ecom_makeLavaSplashAndDelete_b10_hook(gb);
}

void ecom_makeSplashAndDelete_b10_hook(GB *gb) {
  CYC(0x40c9, 0x40cb); B = 0x03; // INTERAC_SPLASH
  CYC(0x40cb, 0x40cd);
  ecom_splashOrLavaTail_b10_hook(gb);
}

void ecom_makeLavaSplashAndDelete_b10_hook(GB *gb) {
  CYC(0x40cd, 0x40cf); B = 0x04; // INTERAC_LAVASPLASH
  ecom_splashOrLavaTail_b10_hook(gb);
}

void ecom_decNumEnemiesAndDelete_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x40d2, decNumEnemies_hook, 0x24b3, 0x40d5);
  CYC(0x40d5, 0x40d8);
  enemyDelete_hook(gb);
}

void ecom_fallDownHoleAndDelete_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x40d8, objectCreateFallingDownHoleInteraction_hook, 0x24d1, 0x40db);
  CYC(0x40db, 0x40dd);
  ecom_decNumEnemiesAndDelete_b10_hook(gb);
}

// @param[out] zflag z if enemy is in the center of the hole
static void ecom_fallingInHole_checkInCenterOfHole_b10(GB *gb) {
  CYC(0x4108, 0x410a); L = ENEMY_BASE + OBJ_YH;
  CYC(0x410a, 0x410b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x410b, 0x410d); hram_wr(gb, 0x8f, A);
  CYC(0x410d, 0x410f); alu_add(gb, 0x05);
  CYC(0x410f, 0x4111); alu_and(gb, 0xf0);
  CYC(0x4111, 0x4113); alu_add(gb, 0x08);
  CYC(0x4113, 0x4114); B = A;
  CYC(0x4114, 0x4115); L = alu_inc8(gb, L);
  CYC(0x4115, 0x4116); A = mem_rd(gb, HL);
  CYC(0x4116, 0x4118); hram_wr(gb, 0x8e, A);
  CYC(0x4118, 0x411a); alu_and(gb, 0xf0);
  CYC(0x411a, 0x411c); alu_add(gb, 0x08);
  CYC(0x411c, 0x411d); C = A;
  CYC(0x411d, 0x411e); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x411e); return; }
  CYC(0x411e, 0x411f);
  CYC(0x411f, 0x4121); A = hram_rd(gb, 0x8f);
  CYC(0x4121, 0x4122); alu_cp(gb, B);
  RET(0x4122); return;
}

// Enemy is currently falling down a hole.
void ecom_fallingInHole_b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x40dd, ecom_decCounter1_b10, 0x439a, 0x40e0);
  if (F & FZ) {
    CYCT(0x40e0, 0x40e2);
    ecom_fallDownHoleAndDelete_b10_hook(gb);
    return;
  }
  CYC(0x40e0, 0x40e2);
  CYC(0x40e2, 0x40e3); A = mem_rd(gb, HL);
  CYC(0x40e3, 0x40e5); alu_and(gb, 0x07);
  if (F & FZ) {
    CYC(0x40e5, 0x40e7); CYC(0x40e7, 0x40ea); push_effect(gb, 0x40ea);
    ecom_fallingInHole_checkInCenterOfHole_b10(gb);
    if (F & FZ) {
      CYCT(0x40ea, 0x40ec);
      ecom_fallDownHoleAndDelete_b10_hook(gb);
      return;
    }
    CYC(0x40ea, 0x40ec);
    CALL_C(0x40ec, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x40ef);
    CYC(0x40ef, 0x40f0); C = A;
    CYC(0x40f0, 0x40f2); B = 0x14; // SPEED_80
    CALL_C(0x40f2, ecom_applyGivenVelocity_b10, 0x4138, 0x40f5);
  } else {
    CYCT(0x40e5, 0x40e7);
  }
  // If bit 0 of counter2 is set, animate the enemy as it's being sucked toward the hole.
  CYC(0x40f5, 0x40f6); H = D;
  CYC(0x40f6, 0x40f8); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x40f8, 0x40fa); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x40fa); return; }
  CYC(0x40fa, 0x40fb);
  CYC(0x40fb, 0x40fd); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(0x40fd, 0x40fe); A = mem_rd(gb, HL);
  CYC(0x40fe, 0x4100); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x4100, 0x4102);
  } else {
    CYC(0x4100, 0x4102);
    alu_xor(gb, A);
  }
  CYC(0x4102, 0x4103); A = alu_inc8(gb, A);
  CYC(0x4103, 0x4105); mem_wr(gb, HL, A);
  CYC(0x4105, 0x4108);
  enemyAnimate_hook(gb);
}
