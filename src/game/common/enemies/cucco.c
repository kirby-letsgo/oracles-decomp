#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void cucco_state_uninitialized_hook(GB *gb);
void cucco_state_grabbed_hook(GB *gb);
void cucco_state_stub_hook(GB *gb);
void cucco_state8_hook(GB *gb);
void cucco_state9_hook(GB *gb);
void cucco_animate_hook(GB *gb);
void cucco_stateA_hook(GB *gb);
void cucco_stateB_hook(GB *gb);
void giantCucco_state_uninitialized_hook(GB *gb);
void giantCucco_stateA_hook(GB *gb);
void giantCucco_stateB_hook(GB *gb);
void giantCucco_animate_hook(GB *gb);
void cucco_setAnimationFromAngle_hook(GB *gb);
void cucco_checkSpawnCuccoAttacker_hook(GB *gb);
void cucco_attacked_hook(GB *gb);
void cucco_hitWithMysterySeed_hook(GB *gb);
void cucco_playChickenSoundEvery32Frames_hook(GB *gb);

static uint16_t cucco_jump_table(GB *gb) {
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

static void cucco_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_CUCCO
//
// Shares some code with ENEMY_GIANT_CUCCO.
//
// Variables:
//   relatedObj1: INTERAC_PUFF object when transforming
//   var30: Number of times it's been hit (also read by PART_CUCCO_ATTACKER to decide
//          speed)
//   var31: Enemy ID to transform into, when a mystery seed is used on it
//   var32: Counter used while being held
//   var33: Counter until next PART_CUCCO_ATTACKER is spawned
// ==================================================================================================
void enemyCode36_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x4da4, 0x4da6); goto normalStatus; } // jr z
  CYC(0x4da4, 0x4da6);
  CYC(0x4da6, 0x4da7); H = D;
  CYC(0x4da7, 0x4da9); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4da9, 0x4daa); A = mem_rd(gb, HL);
  CYC(0x4daa, 0x4dac); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (F & FZ) { CYCT(0x4dac, 0x4daf); cucco_hitWithMysterySeed_hook(gb); return; } // jp z
  CYC(0x4dac, 0x4daf);
  CYC(0x4daf, 0x4db1); alu_cp(gb, 0x9e); // $80|ITEMCOLLISION_GALE_SEED
  if (!(F & FZ)) { CYCT(0x4db1, 0x4db4); cucco_attacked_hook(gb); return; } // jp nz
  CYC(0x4db1, 0x4db4);

normalStatus:
  CALL_C(0x4db4, cucco_checkSpawnCuccoAttacker_hook, 0x4f49, 0x4db7);
  CYC(0x4db7, 0x4db9); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4db9, 0x4dba); A = mem_rd(gb, DE);
  {
    CYC(0x4dba, 0x4dbb); push_effect(gb, 0x4dbb);
    uint16_t target = cucco_jump_table(gb);
    if (target == 0x4dd3) { cucco_state_uninitialized_hook(gb); return; }
    if (target == 0x4e45) { cucco_state_stub_hook(gb); return; }
    if (target == 0x4ddd) { cucco_state_grabbed_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x4e46) { cucco_state8_hook(gb); return; }
    if (target == 0x4e64) { cucco_state9_hook(gb); return; }
    if (target == 0x4e8d) { cucco_stateA_hook(gb); return; }
    if (target == 0x4e9b) { cucco_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

void cucco_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4dd3, 0x4dd5); A = 0x14; // SPEED_80
  CALL_C(0x4dd5, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x4dd8);
  CYC(0x4dd8, 0x4dda); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x4dda, 0x4ddc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  RET(0x4ddc); return; // ret
}

// Also used by ENEMY_GIANT_CUCCO
void cucco_state_grabbed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ddd, 0x4dde); E = alu_inc8(gb, E);
  CYC(0x4dde, 0x4ddf); A = mem_rd(gb, DE);
  {
    CYC(0x4ddf, 0x4de0); push_effect(gb, 0x4de0);
    uint16_t target = cucco_jump_table(gb);
    if (target == 0x4de8) goto justGrabbed;
    if (target == 0x4e0b) goto holding;
    if (target == 0x4e1f) goto checkOutOfScreenBounds;
    if (target == 0x4e31) goto landed;
    HANDOFF(target);
  }

justGrabbed:
  CYC(0x4de8, 0x4de9); H = D;
  CYC(0x4de9, 0x4dea); L = E;
  CYC(0x4dea, 0x4deb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x4deb, 0x4ded); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4ded, 0x4def); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4def, 0x4df1); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4df1, 0x4df2); alu_xor(gb, A);
  CYC(0x4df2, 0x4df3); mem_wr(gb, HL, A);
  CYC(0x4df3, 0x4df6); mem_wr(gb, wLinkGrabState2, A);
  CYC(0x4df6, 0x4df9); A = mem_rd(gb, w1Link_direction);
  CYC(0x4df9, 0x4dfb); A = alu_srl(gb, A);
  CYC(0x4dfb, 0x4dfd); alu_xor(gb, 0x01);
  CYC(0x4dfd, 0x4dff); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x4dff, 0x4e00); mem_wr(gb, HL, A);
  CALL_C(0x4e00, enemySetAnimation_hook, 0x282b, 0x4e03);
  CYC(0x4e03, 0x4e05); A = 0xa0; // SND_CHICKEN
  CALL_C(0x4e05, playSound_b00_hook, 0x0c98, 0x4e08);
  CYC(0x4e08, 0x4e0b); objectSetVisiblec1_hook(gb); return; // jp

holding:
  CALL_C(0x4e0b, cucco_playChickenSoundEvery32Frames_hook, 0x4fc7, 0x4e0e);
  CYC(0x4e0e, 0x4e0f); H = D;
  CYC(0x4e0f, 0x4e11); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x4e11, 0x4e14); A = mem_rd(gb, w1Link_direction);
  CYC(0x4e14, 0x4e16); A = alu_srl(gb, A);
  CYC(0x4e16, 0x4e18); alu_xor(gb, 0x01);
  CYC(0x4e18, 0x4e19); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4e19, 0x4e1b); goto checkOutOfScreenBounds; } // jr z
  CYC(0x4e19, 0x4e1b);
  CYC(0x4e1b, 0x4e1c); mem_wr(gb, HL, A);
  CYC(0x4e1c, 0x4e1f); enemySetAnimation_hook(gb); return; // jp

checkOutOfScreenBounds:
  CYC(0x4e1f, 0x4e21); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4e21, 0x4e22); A = mem_rd(gb, DE);
  CYC(0x4e22, 0x4e24); alu_cp(gb, 0x80); // SMALL_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(0x4e24, 0x4e26); goto L_4e2e; } // jr nc
  CYC(0x4e24, 0x4e26);
  CYC(0x4e26, 0x4e28); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4e28, 0x4e29); A = mem_rd(gb, DE);
  CYC(0x4e29, 0x4e2b); alu_cp(gb, 0xa0); // SMALL_ROOM_WIDTH<<4
  if (F & FC) { CYCT(0x4e2b, 0x4e2e); enemyAnimate_hook(gb); return; } // jp c
  CYC(0x4e2b, 0x4e2e);

L_4e2e:
  CYC(0x4e2e, 0x4e31); enemyDelete_hook(gb); return; // jp

landed:
  CYC(0x4e31, 0x4e32); H = D;
  CYC(0x4e32, 0x4e34); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4e34, 0x4e36); mem_wr(gb, HL, 0x0a);
  CYC(0x4e36, 0x4e38); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4e38, 0x4e3a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4e3a, 0x4e3c); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4e3c, 0x4e3e); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x4e3e, 0x4e40); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4e40, 0x4e42); mem_wr(gb, HL, 0x01);
  CYC(0x4e42, 0x4e45); objectSetVisiblec2_hook(gb); return; // jp
}

void cucco_state_stub_hook(GB *gb) {
  RET(0x4e45); return; // ret
}

// Standing still.
// Also used by ENEMY_GIANT_CUCCO.
void cucco_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4e46, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x4e49);
  CYC(0x4e49, 0x4e4b); E = 0x3f;
  CYC(0x4e4b, 0x4e4e); SET_BC(0x031f);
  CALL_C(0x4e4e, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x4e51);
  CYC(0x4e51, 0x4e52); alu_or(gb, E);
  if (!(F & FZ)) { RET_TAKEN(0x4e52); return; } // ret nz (63 in 64 chance of returning)
  CYC(0x4e52, 0x4e53);
  CALL_C(0x4e53, ecom_incState_b0e_hook, 0x4000, 0x4e56);
  CYC(0x4e56, 0x4e58); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4e58, 0x4e59); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [counter1] = 0
  CYC(0x4e59, 0x4e5b); A = 0x02;
  CYC(0x4e5b, 0x4e5c); alu_add(gb, B);
  CYC(0x4e5c, 0x4e5d); mem_wr(gb, HL, A); // [counter2]
  CYC(0x4e5d, 0x4e5f); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4e5f, 0x4e60); A = C;
  CYC(0x4e60, 0x4e61); mem_wr(gb, HL, A);
  CYC(0x4e61, 0x4e64); cucco_setAnimationFromAngle_hook(gb); return; // jp
}

// Moving in some direction until [counter2] == 0.
// Also used by ENEMY_GIANT_CUCCO.
void cucco_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4e64, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x4e67);
  CYC(0x4e67, 0x4e68); H = D;
  CYC(0x4e68, 0x4e6a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4e6a, 0x4e6b); A = mem_rd(gb, HL);
  CYC(0x4e6b, 0x4e6d); alu_and(gb, 0x0f);
  CYC(0x4e6d, 0x4e6e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x4e6e, 0x4e71); SET_HL(0x4f39); // cucco_zVals (data)
  CYC(0x4e71, 0x4e72); cucco_addAToHl_from_rst(gb, 0x4e72);
  CYC(0x4e72, 0x4e74); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x4e74, 0x4e75); A = mem_rd(gb, HL);
  CYC(0x4e75, 0x4e76); mem_wr(gb, DE, A);
  CYC(0x4e76, 0x4e77); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4e77, 0x4e79); goto L_4e81; } // jr nz
  CYC(0x4e77, 0x4e79);
  CALL_C(0x4e79, ecom_decCounter2_b0e_hook, 0x43a3, 0x4e7c);
  if (!(F & FZ)) { CYCT(0x4e7c, 0x4e7e); goto L_4e81; } // jr nz
  CYC(0x4e7c, 0x4e7e);
  CYC(0x4e7e, 0x4e80); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4e80, 0x4e81); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)

L_4e81:
  CALL_C(0x4e81, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x4e84);
  if (!(F & FZ)) CALL_C_CC(0x4e84, cucco_setAnimationFromAngle_hook, 0x4f25, 0x4e87); else CYC(0x4e84, 0x4e87); // call nz
  CALL_C(0x4e87, objectApplySpeed_hook, 0x201d, 0x4e8a);
  cucco_animate_hook(gb); return; // fallthrough
}

void cucco_animate_hook(GB *gb) {
  CYC(0x4e8a, 0x4e8d); enemyAnimate_hook(gb); return; // jp
}

// Just landed after being thrown. Run away from Link indefinitely.
void cucco_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4e8d, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x4e90);
  CALL_C(0x4e90, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, 0x43ab, 0x4e93);
  CALL_C(0x4e93, cucco_setAnimationFromAngle_hook, 0x4f25, 0x4e96);
  CALL_C(0x4e96, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x4e99);
  CYCT(0x4e99, 0x4e9b); cucco_animate_hook(gb); return; // jr
}

// In the process of transforming (into ENEMY_BABY_CUCCO or ENEMY_GIANT_CUCCO, based on
// var31)
void cucco_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e9b, 0x4e9d); A = 0x21; // Object.animParameter
  CALL_C(0x4e9d, objectGetRelatedObject1Var_hook, 0x2160, 0x4ea0);
  CYC(0x4ea0, 0x4ea2); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x4ea2); return; } // ret z
  CYC(0x4ea2, 0x4ea3);
  CYC(0x4ea3, 0x4ea5); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4ea5, 0x4ea6); A = mem_rd(gb, DE);
  CYC(0x4ea6, 0x4ea7); B = A;
  CYC(0x4ea7, 0x4ea9); C = 0x00;
  CYC(0x4ea9, 0x4eac); objectReplaceWithID_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_GIANT_CUCCO
//
// Variables are the same as ENEMY_CUCCO.
// ==================================================================================================
void enemyCode3b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x4eac, 0x4eae); goto normalStatus; } // jr z
  CYC(0x4eac, 0x4eae);
  CYC(0x4eae, 0x4eb0); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4eb0); return; } // ret c
  CYC(0x4eb0, 0x4eb1);
  CYC(0x4eb1, 0x4eb3); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4eb3, 0x4eb4); A = mem_rd(gb, DE);
  CYC(0x4eb4, 0x4eb6); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x4eb6, 0x4eb8); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { CYCT(0x4eb8, 0x4eba); goto normalStatus; } // jr c
  CYC(0x4eb8, 0x4eba);
  CYC(0x4eba, 0x4ebb); H = D;
  CYC(0x4ebb, 0x4ebd); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4ebd, 0x4ebe); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x4ebe, 0x4ec0); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x4ec0, 0x4ec2); mem_wr(gb, HL, 0x40);
  CYC(0x4ec2, 0x4ec4); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ec4, 0x4ec5); A = mem_rd(gb, HL);
  CYC(0x4ec5, 0x4ec7); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x4ec7, 0x4ec9); goto normalStatus; } // jr nc
  CYC(0x4ec7, 0x4ec9);
  CYC(0x4ec9, 0x4ecb); mem_wr(gb, HL, 0x0a);
  CYC(0x4ecb, 0x4ecd); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x4ecd, 0x4ecf); mem_wr(gb, HL, 0x00);

normalStatus:
  CYC(0x4ecf, 0x4ed1); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ed1, 0x4ed2); A = mem_rd(gb, DE);
  {
    CYC(0x4ed2, 0x4ed3); push_effect(gb, 0x4ed3);
    uint16_t target = cucco_jump_table(gb);
    if (target == 0x4eeb) { giantCucco_state_uninitialized_hook(gb); return; }
    if (target == 0x4e45) { cucco_state_stub_hook(gb); return; }
    if (target == 0x4ddd) { cucco_state_grabbed_hook(gb); return; }
    if (target == 0x4e46) { cucco_state8_hook(gb); return; }
    if (target == 0x4e64) { cucco_state9_hook(gb); return; }
    if (target == 0x4ef8) { giantCucco_stateA_hook(gb); return; }
    if (target == 0x4f16) { giantCucco_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

void giantCucco_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4eeb, 0x4eed); A = 0x1e; // SPEED_c0
  CALL_C(0x4eed, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x4ef0);
  CYC(0x4ef0, 0x4ef2); A = 0x30;
  CALL_C(0x4ef2, setScreenShakeCounter_hook, 0x24bb, 0x4ef5);
  CYC(0x4ef5, 0x4ef8); objectSetVisiblec1_hook(gb); return; // jp
}

// Hit with anything other than Link or shield
void giantCucco_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ef8, 0x4efa); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4efa, 0x4efb); A = mem_rd(gb, DE);
  CYC(0x4efb, 0x4efd); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(0x4efd, 0x4eff); goto runAway; } // jr c
  CYC(0x4efd, 0x4eff);
  CALL_C(0x4eff, ecom_incState_b0e_hook, 0x4000, 0x4f02);
  CYC(0x4f02, 0x4f04); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4f04, 0x4f06); mem_wr(gb, HL, 0x00);
  CYC(0x4f06, 0x4f08); A = 0x8d; // SND_TELEPORT
  CYC(0x4f08, 0x4f0b); playSound_b00_hook(gb); return; // jp

runAway:
  CALL_C(0x4f0b, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, 0x43ab, 0x4f0e);
  CALL_C(0x4f0e, cucco_setAnimationFromAngle_hook, 0x4f25, 0x4f11);
  CALL_C(0x4f11, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x4f14);
  CYCT(0x4f14, 0x4f16); giantCucco_animate_hook(gb); return; // jr
}

// Charging toward Link after being hit 8 times
void giantCucco_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f16, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4f19);
  CALL_C(0x4f19, objectNudgeAngleTowards_hook, 0x1fd4, 0x4f1c);
  CALL_C(0x4f1c, cucco_setAnimationFromAngle_hook, 0x4f25, 0x4f1f);
  CALL_C(0x4f1f, objectApplySpeed_hook, 0x201d, 0x4f22);
  giantCucco_animate_hook(gb); return; // fallthrough
}

void giantCucco_animate_hook(GB *gb) {
  CYC(0x4f22, 0x4f25); enemyAnimate_hook(gb); return; // jp
}

void cucco_setAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f25, 0x4f26); H = D;
  CYC(0x4f26, 0x4f28); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4f28, 0x4f29); A = mem_rd(gb, HL);
  CYC(0x4f29, 0x4f2b); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x4f2b); return; } // ret z
  CYC(0x4f2b, 0x4f2c);
  CYC(0x4f2c, 0x4f2d); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4f2d, 0x4f2f); alu_and(gb, 0x10);
  CYC(0x4f2f, 0x4f31); alu_swap_a(gb);
  CYC(0x4f31, 0x4f33); alu_xor(gb, 0x01);
  CYC(0x4f33, 0x4f34); alu_cp(gb, mem_rd(gb, HL)); // hl == direction
  if (F & FZ) { RET_TAKEN(0x4f34); return; } // ret z
  CYC(0x4f34, 0x4f35);
  CYC(0x4f35, 0x4f36); mem_wr(gb, HL, A);
  CYC(0x4f36, 0x4f39); enemySetAnimation_hook(gb); return; // jp
}

void cucco_checkSpawnCuccoAttacker_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f49, 0x4f4a); H = D;
  CYC(0x4f4a, 0x4f4c); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x4f4c, 0x4f4d); A = mem_rd(gb, HL);
  CYC(0x4f4d, 0x4f4e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4f4e, 0x4f50); goto L_4f52; } // jr z
  CYC(0x4f4e, 0x4f50);
  CYC(0x4f50, 0x4f51); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(0x4f51); return; } // ret nz
  CYC(0x4f51, 0x4f52);

L_4f52:
  CYC(0x4f52, 0x4f54); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4f54, 0x4f55); A = mem_rd(gb, HL);
  CYC(0x4f55, 0x4f57); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(0x4f57); return; } // ret c
  CYC(0x4f57, 0x4f58);
  CYC(0x4f58, 0x4f5a); B = 0x22; // PART_CUCCO_ATTACKER
  CALL_C(0x4f5a, ecom_spawnProjectile_b0e_hook, 0x437c, 0x4f5d);
  CYC(0x4f5d, 0x4f5f); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4f5f, 0x4f60); A = mem_rd(gb, DE);
  CYC(0x4f60, 0x4f62); alu_sub(gb, 0x10);
  CYC(0x4f62, 0x4f64); alu_and(gb, 0x1e);
  CYC(0x4f64, 0x4f65); alu_rrca(gb);
  CYC(0x4f65, 0x4f68); SET_HL(0x4f6e); // @var33Vals (data)
  CYC(0x4f68, 0x4f69); cucco_addAToHl_from_rst(gb, 0x4f69);
  CYC(0x4f69, 0x4f6b); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x4f6b, 0x4f6c); A = mem_rd(gb, HL);
  CYC(0x4f6c, 0x4f6d); mem_wr(gb, DE, A);
  RET(0x4f6d); return; // ret
}

void cucco_attacked_hook(GB *gb) {
  CYC(0x4f77, 0x4f79); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x4f79, 0x4f7b); mem_wr(gb, HL, 0x40);
  CYC(0x4f7b, 0x4f7d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4f7d, 0x4f7f); A = 0x0a;
  CYC(0x4f7f, 0x4f80); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4f80, 0x4f82); goto L_4f8b; } // jr z
  CYC(0x4f80, 0x4f82);
  CYC(0x4f82, 0x4f83); mem_wr(gb, HL, A);
  CYC(0x4f83, 0x4f85); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4f85, 0x4f87); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x4f87, 0x4f89); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x4f89, 0x4f8b); mem_wr(gb, HL, 0x00);

L_4f8b:
  CYC(0x4f8b, 0x4f8d); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4f8d, 0x4f8e); A = mem_rd(gb, DE);
  CYC(0x4f8e, 0x4f8f); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(0x4f8f); return; } // ret nc
  CYC(0x4f8f, 0x4f90);
  CYC(0x4f90, 0x4f92); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4f92, 0x4f94); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4f94, 0x4f96); goto L_4f97; } // jr nz
  CYC(0x4f94, 0x4f96);
  CYC(0x4f96, 0x4f97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

L_4f97:
  CYC(0x4f97, 0x4f99); A = 0xa0; // SND_CHICKEN
  CYC(0x4f99, 0x4f9c); playSound_b00_hook(gb); return; // jp
}

// Cucco will transform into ENEMY_BABY_CUCCO (if not aggressive) or ENEMY_GIANT_CUCCO
// (if aggressive).
void cucco_hitWithMysterySeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f9c, 0x4f9e); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4f9e, 0x4fa0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4fa0, 0x4fa2); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4fa2, 0x4fa3); A = mem_rd(gb, HL);
  CYC(0x4fa3, 0x4fa5); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x4fa5, 0x4fa7); goto L_4fab; } // jr c
  CYC(0x4fa5, 0x4fa7);
  CYC(0x4fa7, 0x4fa9); A = 0x3b; // ENEMY_GIANT_CUCCO
  CYCT(0x4fa9, 0x4fab); goto L_4fad; // jr

L_4fab:
  CYC(0x4fab, 0x4fad); A = 0x33; // ENEMY_BABY_CUCCO

L_4fad:
  CYC(0x4fad, 0x4faf); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4faf, 0x4fb0); mem_wr(gb, DE, A);
  CYC(0x4fb0, 0x4fb3); SET_BC(0x0502); // INTERAC_PUFF,$02
  CALL_C(0x4fb3, objectCreateInteraction_hook, 0x24c5, 0x4fb6);
  if (!(F & FZ)) { RET_TAKEN(0x4fb6); return; } // ret nz
  CYC(0x4fb6, 0x4fb7);
  CYC(0x4fb7, 0x4fb9); E = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x4fb9, 0x4fbb); A = 0x40; // Interaction.start
  CYC(0x4fbb, 0x4fbc); mem_wr(gb, DE, A);
  CYC(0x4fbc, 0x4fbd); E = alu_inc8(gb, E);
  CYC(0x4fbd, 0x4fbe); A = H;
  CYC(0x4fbe, 0x4fbf); mem_wr(gb, DE, A);
  CYC(0x4fbf, 0x4fc1); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4fc1, 0x4fc3); A = 0x0b;
  CYC(0x4fc3, 0x4fc4); mem_wr(gb, DE, A);
  CYC(0x4fc4, 0x4fc7); objectSetInvisible_hook(gb); return; // jp
}

void cucco_playChickenSoundEvery32Frames_hook(GB *gb) {
  CYC(0x4fc7, 0x4fc8); H = D;
  CYC(0x4fc8, 0x4fca); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x4fca, 0x4fcb); A = mem_rd(gb, HL);
  CYC(0x4fcb, 0x4fcc); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4fcc); return; } // ret nz
  CYC(0x4fcc, 0x4fcd);
  CYC(0x4fcd, 0x4fcf); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4fcf, 0x4fd0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x4fd0, 0x4fd1); A = mem_rd(gb, HL);
  CYC(0x4fd1, 0x4fd3); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x4fd3); return; } // ret nz
  CYC(0x4fd3, 0x4fd4);
  CYC(0x4fd4, 0x4fd6); A = 0xa0; // SND_CHICKEN
  CYC(0x4fd6, 0x4fd9); playSound_b00_hook(gb); return; // jp
}
