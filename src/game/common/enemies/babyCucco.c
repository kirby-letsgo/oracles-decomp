#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode33), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode33), (from), (to), true)

void babyCucco_state_uninitialized_hook(GB *gb);
void babyCucco_state_grabbed_hook(GB *gb);
void babyCucco_state_stub_hook(GB *gb);
void babyCucco_state8_hook(GB *gb);
void babyCucco_animate_hook(GB *gb);
void babyCucco_state9_hook(GB *gb);
void babyCucco_updateAnimationFromAngle_hook(GB *gb);

static uint16_t babyCucco_jump_table(GB *gb) {
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
// ENEMY_BABY_CUCCO
// ==================================================================================================
void enemyCode33_hook(GB *gb) {
  BASE(enemyCode33);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = babyCucco_jump_table(gb);
    if (target == SYM(babyCucco_state_uninitialized)) { babyCucco_state_uninitialized_hook(gb); return; }
    if (target == SYM(babyCucco_state_stub)) { babyCucco_state_stub_hook(gb); return; }
    if (target == SYM(babyCucco_state_grabbed)) { babyCucco_state_grabbed_hook(gb); return; }
    if (target == SYM(babyCucco_state8)) { babyCucco_state8_hook(gb); return; }
    if (target == SYM(babyCucco_state9)) { babyCucco_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

void babyCucco_state_uninitialized_hook(GB *gb) {
  BASE(babyCucco_state_uninitialized);
  CYC(b_+0, b_+2); A = 0x0a; // SPEED_40
  CYC(b_+2, SYM(babyCucco_state_grabbed)); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

void babyCucco_state_grabbed_hook(GB *gb) {
  BASE(babyCucco_state_grabbed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = babyCucco_jump_table(gb);
    if (target == b_+11) goto justGrabbed;
    if (target == b_+38) goto beingHeld;
    if (target == b_+55) goto released;
    if (target == b_+73) goto landed;
    HANDOFF(target);
  }

justGrabbed:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+22, b_+25); A = mem_rd(gb, w1Link_direction);
  CYC(b_+25, b_+27); A = alu_srl(gb, A);
  CYC(b_+27, b_+29); alu_xor(gb, 0x01);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CALL_C(b_+32, enemySetAnimation_hook, SYM(enemySetAnimation), b_+35);
  CYC(b_+35, b_+38); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+41, b_+44); A = mem_rd(gb, w1Link_direction);
  CYC(b_+44, b_+46); A = alu_srl(gb, A);
  CYC(b_+46, b_+48); alu_xor(gb, 0x01);
  CYC(b_+48, b_+49); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+49, b_+51); goto released; } // jr z
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CYC(b_+52, b_+55); enemySetAnimation_hook(gb); return; // jp

released:
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+60); alu_cp(gb, 0x80); // SMALL_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(b_+60, b_+62); goto delete_; } // jr nc
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+67); alu_cp(gb, 0xa0); // SMALL_ROOM_WIDTH<<4
  if (F & FC) { CYCT(b_+67, b_+70); enemyAnimate_hook(gb); return; } // jp c
  CYC(b_+67, b_+70);

delete_:
  CYC(b_+70, b_+73); enemyDelete_hook(gb); return; // jp

landed:
  CYC(b_+73, b_+74); H = D;
  CYC(b_+74, b_+76); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x08);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+80, b_+82); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+82, b_+84); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0xff);
  CYC(b_+86, SYM(babyCucco_state_stub)); objectSetVisiblec2_hook(gb); return; // jp
}

void babyCucco_state_stub_hook(GB *gb) {
  BASE(babyCucco_state_stub);
  RET(b_+0); return; // ret
}

void babyCucco_state8_hook(GB *gb) {
  BASE(babyCucco_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CALL_C(b_+3, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+6);
  CALL_C(b_+6, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+9);
  CALL_C(b_+9, babyCucco_updateAnimationFromAngle_hook, SYM(babyCucco_updateAnimationFromAngle), b_+12);
  CYC(b_+12, b_+14); C = 0x10;
  CALL_C(b_+14, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+17);
  if (!(F & FC)) { CYCT(b_+17, b_+19); goto moveCloserToLink; } // jr nc
  CYC(b_+17, b_+19);
  CALL_C(b_+19, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+22);
  CYC(b_+22, b_+24); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);
  CALL_C(b_+25, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+28);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+30, b_+32); A = 0x40;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xff);
  RET(b_+35); return; // ret

moveCloserToLink:
  CALL_C(b_+36, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), SYM(babyCucco_animate));
  babyCucco_animate_hook(gb); return; // fallthrough
}

void babyCucco_animate_hook(GB *gb) {
  BASE(babyCucco_animate);
  CYC(b_+0, SYM(babyCucco_state9)); enemyAnimate_hook(gb); return; // jp
}

// Hopping
void babyCucco_state9_hook(GB *gb) {
  BASE(babyCucco_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x12;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); babyCucco_animate_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  RET(b_+10); return; // ret
}

void babyCucco_updateAnimationFromAngle_hook(GB *gb) {
  BASE(babyCucco_updateAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x10);
  CYC(b_+5, b_+7); A = 0x01;
  if (F & FC) { CYCT(b_+7, b_+9); goto L_4924; } // jr c
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);

L_4924:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+14); return; } // ret z
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, SYM(enemyCode34)); enemySetAnimation_hook(gb); return; // jp
}
