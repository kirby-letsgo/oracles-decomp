#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode36);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (F & FZ) { CYCT(b_+8, b_+11); cucco_hitWithMysterySeed_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x9e); // $80|ITEMCOLLISION_GALE_SEED
  if (!(F & FZ)) { CYCT(b_+13, b_+16); cucco_attacked_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);

normalStatus:
  CALL_C(b_+16, cucco_checkSpawnCuccoAttacker_hook, SYM(cucco_checkSpawnCuccoAttacker), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  {
    CYC(b_+22, b_+23); push_effect(gb, b_+23);
    uint16_t target = cucco_jump_table(gb);
    if (target == SYM(cucco_state_uninitialized)) { cucco_state_uninitialized_hook(gb); return; }
    if (target == SYM(cucco_state_stub)) { cucco_state_stub_hook(gb); return; }
    if (target == SYM(cucco_state_grabbed)) { cucco_state_grabbed_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(cucco_state8)) { cucco_state8_hook(gb); return; }
    if (target == SYM(cucco_state9)) { cucco_state9_hook(gb); return; }
    if (target == SYM(cucco_stateA)) { cucco_stateA_hook(gb); return; }
    if (target == SYM(cucco_stateB)) { cucco_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

void cucco_state_uninitialized_hook(GB *gb) {
  BASE(cucco_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x14; // SPEED_80
  CALL_C(b_+2, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  RET(b_+9); return; // ret
}

// Also used by ENEMY_GIANT_CUCCO
void cucco_state_grabbed_hook(GB *gb) {
  BASE(cucco_state_grabbed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = cucco_jump_table(gb);
    if (target == b_+11) goto justGrabbed;
    if (target == b_+46) goto holding;
    if (target == b_+66) goto checkOutOfScreenBounds;
    if (target == b_+84) goto landed;
    HANDOFF(target);
  }

justGrabbed:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+18, b_+20); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+25); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+25, b_+28); A = mem_rd(gb, w1Link_direction);
  CYC(b_+28, b_+30); A = alu_srl(gb, A);
  CYC(b_+30, b_+32); alu_xor(gb, 0x01);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CALL_C(b_+35, enemySetAnimation_hook, SYM(enemySetAnimation), b_+38);
  CYC(b_+38, b_+40); A = 0xa0; // SND_CHICKEN
  CALL_C(b_+40, playSound_b00_hook, SYM(playSound_b00), b_+43);
  CYC(b_+43, b_+46); objectSetVisiblec1_hook(gb); return; // jp

holding:
  CALL_C(b_+46, cucco_playChickenSoundEvery32Frames_hook, SYM(cucco_playChickenSoundEvery32Frames), b_+49);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+52, b_+55); A = mem_rd(gb, w1Link_direction);
  CYC(b_+55, b_+57); A = alu_srl(gb, A);
  CYC(b_+57, b_+59); alu_xor(gb, 0x01);
  CYC(b_+59, b_+60); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+60, b_+62); goto checkOutOfScreenBounds; } // jr z
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+66); enemySetAnimation_hook(gb); return; // jp

checkOutOfScreenBounds:
  CYC(b_+66, b_+68); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+71); alu_cp(gb, 0x80); // SMALL_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(b_+71, b_+73); goto L_4e2e; } // jr nc
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+75); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  CYC(b_+76, b_+78); alu_cp(gb, 0xa0); // SMALL_ROOM_WIDTH<<4
  if (F & FC) { CYCT(b_+78, b_+81); enemyAnimate_hook(gb); return; } // jp c
  CYC(b_+78, b_+81);

L_4e2e:
  CYC(b_+81, b_+84); enemyDelete_hook(gb); return; // jp

landed:
  CYC(b_+84, b_+85); H = D;
  CYC(b_+85, b_+87); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x0a);
  CYC(b_+89, b_+91); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+91, b_+93); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x01);
  CYC(b_+101, b_+104); objectSetVisiblec2_hook(gb); return; // jp
}

void cucco_state_stub_hook(GB *gb) {
  BASE(cucco_state_stub);
  RET(b_+0); return; // ret
}

// Standing still.
// Also used by ENEMY_GIANT_CUCCO.
void cucco_state8_hook(GB *gb) {
  BASE(cucco_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CYC(b_+3, b_+5); E = 0x3f;
  CYC(b_+5, b_+8); SET_BC(0x031f);
  CALL_C(b_+8, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+11);
  CYC(b_+11, b_+12); alu_or(gb, E);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz (63 in 64 chance of returning)
  CYC(b_+12, b_+13);
  CALL_C(b_+13, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [counter1] = 0
  CYC(b_+19, b_+21); A = 0x02;
  CYC(b_+21, b_+22); alu_add(gb, B);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); // [counter2]
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+25, b_+26); A = C;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+30); cucco_setAnimationFromAngle_hook(gb); return; // jp
}

// Moving in some direction until [counter2] == 0.
// Also used by ENEMY_GIANT_CUCCO.
void cucco_state9_hook(GB *gb) {
  BASE(cucco_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_and(gb, 0x0f);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+10, b_+13); SET_HL(SYM(cucco_zVals)); // cucco_zVals (data)
  CYC(b_+13, b_+14); cucco_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_4e81; } // jr nz
  CYC(b_+19, b_+21);
  CALL_C(b_+21, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+24);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto L_4e81; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)

L_4e81:
  CALL_C(b_+29, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), b_+32);
  if (!(F & FZ)) CALL_C_CC(b_+32, cucco_setAnimationFromAngle_hook, SYM(cucco_setAnimationFromAngle), b_+35); else CYC(b_+32, b_+35); // call nz
  CALL_C(b_+35, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(cucco_animate));
  cucco_animate_hook(gb); return; // fallthrough
}

void cucco_animate_hook(GB *gb) {
  BASE(cucco_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Just landed after being thrown. Run away from Link indefinitely.
void cucco_stateA_hook(GB *gb) {
  BASE(cucco_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CALL_C(b_+3, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0e), b_+6);
  CALL_C(b_+6, cucco_setAnimationFromAngle_hook, SYM(cucco_setAnimationFromAngle), b_+9);
  CALL_C(b_+9, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+12);
  CYCT(b_+12, b_+14); cucco_animate_hook(gb); return; // jr
}

// In the process of transforming (into ENEMY_BABY_CUCCO or ENEMY_GIANT_CUCCO, based on
// var31)
void cucco_stateB_hook(GB *gb) {
  BASE(cucco_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x21; // Object.animParameter
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+14); C = 0x00;
  CYC(b_+14, b_+17); objectReplaceWithID_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_GIANT_CUCCO
//
// Variables are the same as ENEMY_CUCCO.
// ==================================================================================================
void enemyCode3b_hook(GB *gb) {
  BASE(enemyCode3b);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+10, b_+12); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { CYCT(b_+12, b_+14); goto normalStatus; } // jr c
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x40);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto normalStatus; } // jr nc
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x0a);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x00);

normalStatus:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  {
    CYC(b_+38, b_+39); push_effect(gb, b_+39);
    uint16_t target = cucco_jump_table(gb);
    if (target == SYM(giantCucco_state_uninitialized)) { giantCucco_state_uninitialized_hook(gb); return; }
    if (target == SYM(cucco_state_stub)) { cucco_state_stub_hook(gb); return; }
    if (target == SYM(cucco_state_grabbed)) { cucco_state_grabbed_hook(gb); return; }
    if (target == SYM(cucco_state8)) { cucco_state8_hook(gb); return; }
    if (target == SYM(cucco_state9)) { cucco_state9_hook(gb); return; }
    if (target == SYM(giantCucco_stateA)) { giantCucco_stateA_hook(gb); return; }
    if (target == SYM(giantCucco_stateB)) { giantCucco_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

void giantCucco_state_uninitialized_hook(GB *gb) {
  BASE(giantCucco_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x1e; // SPEED_c0
  CALL_C(b_+2, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+5);
  CYC(b_+5, b_+7); A = 0x30;
  CALL_C(b_+7, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+10);
  CYC(b_+10, b_+13); objectSetVisiblec1_hook(gb); return; // jp
}

// Hit with anything other than Link or shield
void giantCucco_stateA_hook(GB *gb) {
  BASE(giantCucco_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+5, b_+7); goto runAway; } // jr c
  CYC(b_+5, b_+7);
  CALL_C(b_+7, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x00);
  CYC(b_+14, b_+16); A = 0x8d; // SND_TELEPORT
  CYC(b_+16, b_+19); playSound_b00_hook(gb); return; // jp

runAway:
  CALL_C(b_+19, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0e), b_+22);
  CALL_C(b_+22, cucco_setAnimationFromAngle_hook, SYM(cucco_setAnimationFromAngle), b_+25);
  CALL_C(b_+25, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+28);
  CYCT(b_+28, b_+30); giantCucco_animate_hook(gb); return; // jr
}

// Charging toward Link after being hit 8 times
void giantCucco_stateB_hook(GB *gb) {
  BASE(giantCucco_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CALL_C(b_+3, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+6);
  CALL_C(b_+6, cucco_setAnimationFromAngle_hook, SYM(cucco_setAnimationFromAngle), b_+9);
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(giantCucco_animate));
  giantCucco_animate_hook(gb); return; // fallthrough
}

void giantCucco_animate_hook(GB *gb) {
  BASE(giantCucco_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

void cucco_setAnimationFromAngle_hook(GB *gb) {
  BASE(cucco_setAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+6); return; } // ret z
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+8, b_+10); alu_and(gb, 0x10);
  CYC(b_+10, b_+12); alu_swap_a(gb);
  CYC(b_+12, b_+14); alu_xor(gb, 0x01);
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL)); // hl == direction
  if (F & FZ) { RET_TAKEN(b_+15); return; } // ret z
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+20); enemySetAnimation_hook(gb); return; // jp
}

void cucco_checkSpawnCuccoAttacker_hook(GB *gb) {
  BASE(cucco_checkSpawnCuccoAttacker);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_4f52; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);

L_4f52:
  CYC(b_+9, b_+11); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(b_+14); return; } // ret c
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); B = 0x22; // PART_CUCCO_ATTACKER
  CALL_C(b_+17, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_sub(gb, 0x10);
  CYC(b_+25, b_+27); alu_and(gb, 0x1e);
  CYC(b_+27, b_+28); alu_rrca(gb);
  CYC(b_+28, b_+31); SET_HL(b_+37); // @var33Vals (data)
  CYC(b_+31, b_+32); cucco_addAToHl_from_rst(gb, b_+32);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  RET(b_+36); return; // ret
}

void cucco_attacked_hook(GB *gb) {
  BASE(cucco_attacked);
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x40);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+8); A = 0x0a;
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_4f8b; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x00);

L_4f8b:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+24); return; } // ret nc
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+27, b_+29); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto L_4f97; } // jr nz
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

L_4f97:
  CYC(b_+32, b_+34); A = 0xa0; // SND_CHICKEN
  CYC(b_+34, b_+37); playSound_b00_hook(gb); return; // jp
}

// Cucco will transform into ENEMY_BABY_CUCCO (if not aggressive) or ENEMY_GIANT_CUCCO
// (if aggressive).
void cucco_hitWithMysterySeed_hook(GB *gb) {
  BASE(cucco_hitWithMysterySeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+4, b_+6); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+9, b_+11); goto L_4fab; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x3b; // ENEMY_GIANT_CUCCO
  CYCT(b_+13, b_+15); goto L_4fad; // jr

L_4fab:
  CYC(b_+15, b_+17); A = 0x33; // ENEMY_BABY_CUCCO

L_4fad:
  CYC(b_+17, b_+19); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); SET_BC(0x0502); // INTERAC_PUFF,$02
  CALL_C(b_+23, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+26);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+29, b_+31); A = 0x40; // Interaction.start
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); E = alu_inc8(gb, E);
  CYC(b_+33, b_+34); A = H;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); A = 0x0b;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+43); objectSetInvisible_hook(gb); return; // jp
}

void cucco_playChickenSoundEvery32Frames_hook(GB *gb) {
  BASE(cucco_playChickenSoundEvery32Frames);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); A = 0xa0; // SND_CHICKEN
  CYC(b_+15, b_+18); playSound_b00_hook(gb); return; // jp
}
