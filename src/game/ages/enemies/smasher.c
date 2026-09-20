#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void ecom_setSpeedAndState8AndVisible_b0f_hook(GB *gb);
void ecom_bounceOffWallsAndHoles_b0f_hook(GB *gb);
void ecom_incState_b0f_hook(GB *gb);
void ecom_decCounter1_b0f_hook(GB *gb);
void ecom_decCounter2_b0f_hook(GB *gb);
void ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook(GB *gb);
void ecom_updateCardinalAngleAwayFromTarget_b0f_hook(GB *gb);
void ecom_moveTowardPosition_b0f_hook(GB *gb);
void ecom_readPositionVars_b0f_hook(GB *gb);
void ecom_spawnUncountedEnemyWithSubid01_b0f_hook(GB *gb);
void ecom_updateKnockback_b0f_hook(GB *gb);
void ecom_killRelatedObj1_b0f_hook(GB *gb);
void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb);

void enemyBoss_initializeRoom_b0f_hook(GB *gb);
void enemyBoss_beginMiniboss_b0f_hook(GB *gb);
void enemyBoss_dead_b0f_hook(GB *gb);
void enemyDelete_hook(GB *gb);

void smasher_state_uninitialized_hook(GB *gb);
void smasher_state_grabbed_hook(GB *gb);
void smasher_state_stub_hook(GB *gb);
void smasher_ball_hook(GB *gb);
void smasher_ball_state8_hook(GB *gb);
void smasher_ball_state9_hook(GB *gb);
void smasher_ball_stateA_hook(GB *gb);
void smasher_ball_stateB_hook(GB *gb);
void smasher_ball_stateC_hook(GB *gb);
void smasher_ball_playLandSound_hook(GB *gb);
void smasher_ball_stateD_hook(GB *gb);
void smasher_ball_stateE_hook(GB *gb);
void smasher_ball_stateF_hook(GB *gb);
void smasher_parent_hook(GB *gb);
void smasher_parent_state8_hook(GB *gb);
void smasher_parent_state9_hook(GB *gb);
void smasher_parent_stateA_hook(GB *gb);
void smasher_parent_linkPickedUpBall_hook(GB *gb);
void smasher_parent_stateB_hook(GB *gb);
void smasher_hop_hook(GB *gb);
void smasher_parent_stateC_hook(GB *gb);
void smasher_parent_stateD_hook(GB *gb);
void smasher_updateAngleTowardPosition_hook(GB *gb);
void smasher_updateAngleTowardLink_hook(GB *gb);
void smasher_updateDirectionFromAngle_hook(GB *gb);
void smasher_setOamFlags_hook(GB *gb);
void smasher_ball_loadPositions_hook(GB *gb);
void smasher_ball_updateRespawnTimer_hook(GB *gb);
void smasher_ball_makeLinkDrop_hook(GB *gb);

static uint16_t smasher_jump_table(GB *gb) {
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

static void smasher_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SMASHER
// ==================================================================================================
void smasher_state_uninitialized_hook(GB *gb) {
  BASE(smasher_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto alreadySpawnedParent; } // jr nz
  CYC(b_+2, b_+4);

  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); A = 0xff;
  CALL_C(b_+7, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+10);
  CYC(b_+10, b_+12); B = 0x74; // ENEMY_SMASHER
  CALL_C(b_+12, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+15);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; } // ret nz
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+18, b_+19); E = L;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+23, b_+24); E = L;
  CYC(b_+24, b_+26); A = ENEMY_BASE; // Enemy.start
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); A = H;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); mem_wr(gb, HL, D);
  CALL_C(b_+32, objectCopyPosition_hook, SYM(objectCopyPosition), b_+35);
  CYC(b_+35, b_+36); A = H;
  CYC(b_+36, b_+37); alu_cp(gb, D);
  if (!(F & FC)) { CYCT(b_+37, b_+39); goto initialize; } // jr nc
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x80); // Change former "parent" to "ball"
  CYC(b_+43, b_+44); E = L;
  CYC(b_+44, b_+46); A = 0x01;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A); // Change former "ball" (this) to "parent"

alreadySpawnedParent:
  CYC(b_+47, b_+48); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+48, b_+50); goto gotoState8; } // jr z
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+52, b_+53); alu_xor(gb, A);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);

initialize:
  CYC(b_+54, b_+56); A = 0x01;
  CALL_C(b_+56, smasher_setOamFlags_hook, SYM(smasher_setOamFlags), b_+59);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_sub(gb, 0x20);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  CYC(b_+65, b_+67); A = 0x04;
  CALL_C(b_+67, enemySetAnimation_hook, SYM(enemySetAnimation), b_+70);

gotoState8:
  CYC(b_+70, b_+73); ecom_setSpeedAndState8AndVisible_b0f_hook(gb); return; // jp
}

void smasher_state_grabbed_hook(GB *gb) {
  BASE(smasher_state_grabbed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE); // [substate]
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = smasher_jump_table(gb);
    if (target == b_+11) goto justGrabbed;
    if (target == b_+22) goto beingHeld;
    if (target == b_+23) goto released;
    if (target == b_+90) goto atRest;
    HANDOFF(target);
  }

justGrabbed:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+14, b_+16); A = 0x20;
  CYC(b_+16, b_+19); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+19, b_+22); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  RET(b_+22); return;

released:
  CALL_C(b_+23, ecom_bounceOffWallsAndHoles_b0f_hook, SYM(ecom_bounceOffWallsAndHoles_b0f), b_+26);
  if (F & FZ) { CYCT(b_+26, b_+28); goto L_5225; } // jr z
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+34); SET_HL(w1ReservedItemC_angle);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);

L_5225:
  CYC(b_+35, b_+37); A = 0x2b;
  CALL_C(b_+37, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+40);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; } // ret nz
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+45, b_+46); E = L;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+50); alu_add(gb, 0x08);
  CYC(b_+50, b_+52); alu_cp(gb, 0x11);
  if (!(F & FC)) { RET_TAKEN(b_+52); return; } // ret nc
  CYC(b_+52, b_+53);
  CALL_C(b_+53, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+56);
  if (!(F & FC)) { RET_TAKEN(b_+56); return; } // ret nc
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x20);
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x10);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+68, smasher_ball_loadPositions_hook, SYM(smasher_ball_loadPositions), b_+71);
  CYC(b_+71, b_+72); push_effect(gb, HL);
  CALL_C(b_+72, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+75);
  CYC(b_+75, b_+76); SET_HL(pop_effect(gb));
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+81); alu_xor(gb, 0x10);
  CYC(b_+81, b_+84); SET_HL(w1ReservedItemC_angle);
  CYC(b_+84, b_+85); mem_wr(gb, HL, A);
  CYC(b_+85, b_+87); A = 0x63; // SND_BOSS_DAMAGE
  CYC(b_+87, b_+90); playSound_b00_hook(gb); return; // jp

atRest:
  CYC(b_+90, b_+91); E = alu_dec8(gb, E);
  CYC(b_+91, b_+93); A = 0x08;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A); // [state]
  CYC(b_+94, b_+97); objectSetVisiblec2_hook(gb); return; // jp
}

void smasher_state_stub_hook(GB *gb) {
  BASE(smasher_state_stub);
  RET(b_+0); return;
}

void smasher_ball_hook(GB *gb) {
  BASE(smasher_ball);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = smasher_jump_table(gb);
    if (target == SYM(smasher_ball_state8)) { smasher_ball_state8_hook(gb); return; }
    if (target == SYM(smasher_ball_state9)) { smasher_ball_state9_hook(gb); return; }
    if (target == SYM(smasher_ball_stateA)) { smasher_ball_stateA_hook(gb); return; }
    if (target == SYM(smasher_ball_stateB)) { smasher_ball_stateB_hook(gb); return; }
    if (target == SYM(smasher_ball_stateC)) { smasher_ball_stateC_hook(gb); return; }
    if (target == SYM(smasher_ball_stateD)) { smasher_ball_stateD_hook(gb); return; }
    if (target == SYM(smasher_ball_stateE)) { smasher_ball_stateE_hook(gb); return; }
    if (target == SYM(smasher_ball_stateF)) { smasher_ball_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization (or just reappeared after disappearing)
void smasher_ball_state8_hook(GB *gb) {
  BASE(smasher_ball_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x63); // ENEMYCOLLISION_SMASHER_BALL
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x19); // SPEED_a0
  smasher_ball_state9_hook(gb); return; // fallthrough
}

// Lying on ground, waiting for parent or Link to pick it up
void smasher_ball_state9_hook(GB *gb) {
  BASE(smasher_ball_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+3);
  CYC(b_+3, b_+6); objectPushLinkAwayOnCollision_hook(gb); return; // jp
}

// Parent is picking up the ball
void smasher_ball_stateA_hook(GB *gb) {
  BASE(smasher_ball_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+4, b_+6); alu_cp(gb, 0xf4);
  if (F & FZ) { CYCT(b_+6, b_+8); goto L_5299; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_sub(gb, 0x80);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_sbc(gb, 0x00);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);

L_5299:
  CYC(b_+16, b_+18); A = 0x0b;
  CALL_C(b_+18, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+21);
  CALL_C(b_+21, smasher_ball_loadPositions_hook, SYM(smasher_ball_loadPositions), b_+24);
  CYC(b_+24, b_+25); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nz
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); A = mem_rd(gb, hFF8F);
  CYC(b_+30, b_+31); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+31, b_+34); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nz
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); alu_cp(gb, 0xf4);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; } // ret nz
  CYC(b_+39, b_+40);
  CALL_C(b_+40, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+43);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x78);
  CYC(b_+51, b_+54); objectSetVisiblec1_hook(gb); return; // jp
}

// This state is a signal for the parent, which will update the ball's state when it gets released.
void smasher_ball_stateB_hook(GB *gb) {
  BASE(smasher_ball_stateB);
  RET(b_+0); return;
}

// Being thrown
void smasher_ball_stateC_hook(GB *gb) {
  BASE(smasher_ball_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto doneBouncing; } // jr c
  CYC(b_+5, b_+7);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_52d2; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); A = alu_srl(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, smasher_ball_playLandSound_hook, SYM(smasher_ball_playLandSound), b_+18);

L_52d2:
  CALL_C(b_+18, ecom_bounceOffWallsAndHoles_b0f_hook, SYM(ecom_bounceOffWallsAndHoles_b0f), b_+21);
  CYC(b_+21, b_+24); objectApplySpeed_hook(gb); return; // jp

doneBouncing:
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x08);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+33, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), SYM(smasher_ball_playLandSound));
  smasher_ball_playLandSound_hook(gb); return; // fallthrough
}

void smasher_ball_playLandSound_hook(GB *gb) {
  BASE(smasher_ball_playLandSound);
  CYC(b_+0, b_+2); A = 0x52; // SND_BOMB_LAND
  CYC(b_+2, b_+5); playSound_b00_hook(gb); return; // jp
}

// Disappearing (either after being thrown, or after a time limit)
void smasher_ball_stateD_hook(GB *gb) {
  BASE(smasher_ball_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCreatePuff_hook, SYM(objectCreatePuff), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x04;
  CALL_C(b_+6, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(b_+12, b_+14); goto L_52ff; } // jr c
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0d); // [parent.state]
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+18, b_+20); A = 0x03;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);

L_52ff:
  CALL_C(b_+22, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x3c);
  CYC(b_+33, b_+36); objectSetInvisible_hook(gb); return; // jp
}

// Ball is gone, will reappear after [counter1] frames
void smasher_ball_stateE_hook(GB *gb) {
  BASE(smasher_ball_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xe0); // -$20
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CALL_C(b_+15, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x0e);
  CYC(b_+20, b_+23); SET_HL(b_+38);
  CYC(b_+23, b_+24); smasher_addAToHl_from_rst(gb, b_+24);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CALL_C(b_+32, objectCreatePuff_hook, SYM(objectCreatePuff), b_+35);
  CYC(b_+35, b_+38); objectSetVisiblec1_hook(gb); return; // jp
}

// Ball is falling to ground after reappearing
void smasher_ball_stateF_hook(GB *gb) {
  BASE(smasher_ball_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto doneBouncing; } // jr c
  CYC(b_+5, b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); smasher_ball_playLandSound_hook(gb); return; // jp

doneBouncing:
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+15); A = 0x08;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+19); objectSetVisiblec2_hook(gb); return; // jp
}

void smasher_parent_hook(GB *gb) {
  BASE(smasher_parent);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = smasher_jump_table(gb);
    if (target == SYM(smasher_parent_state8)) { smasher_parent_state8_hook(gb); return; }
    if (target == SYM(smasher_parent_state9)) { smasher_parent_state9_hook(gb); return; }
    if (target == SYM(smasher_parent_stateA)) { smasher_parent_stateA_hook(gb); return; }
    if (target == SYM(smasher_parent_stateB)) { smasher_parent_stateB_hook(gb); return; }
    if (target == SYM(smasher_parent_stateC)) { smasher_parent_stateC_hook(gb); return; }
    if (target == SYM(smasher_parent_stateD)) { smasher_parent_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void smasher_parent_state8_hook(GB *gb) {
  BASE(smasher_parent_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x01);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x1e);
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+13, b_+15); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+15, b_+17); smasher_parent_state9_hook(gb); return; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [var32]
  CALL_C(b_+18, enemyBoss_beginMiniboss_b0f_hook, SYM(enemyBoss_beginMiniboss_b0f), SYM(smasher_parent_state9));
  smasher_parent_state9_hook(gb); return; // fallthrough
}

void smasher_parent_state9_hook(GB *gb) {
  BASE(smasher_parent_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+8, b_+10); goto moveTowardBall; } // jr z
  CYC(b_+8, b_+10);
  CALL_C(b_+10, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto updateMovement; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x3c); // [counter1]
  CALL_C(b_+17, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+20);
  CYC(b_+20, b_+22); alu_and(gb, 0x03);
  CYC(b_+22, b_+25); SET_HL(b_+115);
  CYC(b_+25, b_+26); smasher_addAToHl_from_rst(gb, b_+26);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CALL_C(b_+30, smasher_updateDirectionFromAngle_hook, SYM(smasher_updateDirectionFromAngle), b_+33);

updateMovement:
  CALL_C(b_+33, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0f), b_+36);
  CYC(b_+36, b_+38); C = 0x20;
  CALL_C(b_+38, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+41);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto updateAnim; } // jr nz
  CYC(b_+41, b_+43);
  CALL_C(b_+43, smasher_hop_hook, SYM(smasher_hop), b_+46);
  CYC(b_+46, b_+48); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); A = alu_inc8(gb, A);
  CYC(b_+50, b_+53); enemySetAnimation_hook(gb); return; // jp

updateAnim:
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl) [speedZ+1]
  CYC(b_+54, b_+55); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; } // ret nz
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+62); enemySetAnimation_hook(gb); return; // jp

moveTowardBall:
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+64, b_+66); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+66, b_+67); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  CYC(b_+68, b_+69); L = alu_inc8(gb, L);
  CYC(b_+69, b_+70); E = L;
  CYC(b_+70, b_+71); A = mem_rd(gb, DE); // [parent.xh]
  CYC(b_+71, b_+72); alu_cp(gb, mem_rd(gb, HL)); // [ball.xh]
  CYC(b_+72, b_+74); A = 0x0e;
  if (!(F & FC)) { CYCT(b_+74, b_+76); goto L_53c9; } // jr nc
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); A = 0xf2; // -$0e

L_53c9:
  CYC(b_+78, b_+79); C = A;
  CYC(b_+79, b_+80); alu_add(gb, mem_rd(gb, HL)); // [ball.xh]
  CYC(b_+80, b_+81); B = A;
  CYC(b_+81, b_+83); alu_sub(gb, 0x18);
  CYC(b_+83, b_+85); alu_cp(gb, 0xc0);
  if (F & FC) { CYCT(b_+85, b_+87); goto L_53d8; } // jr c
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); A = C;
  CYC(b_+88, b_+89); alu_cpl(gb);
  CYC(b_+89, b_+90); A = alu_inc8(gb, A);
  CYC(b_+90, b_+91); alu_add(gb, A);
  CYC(b_+91, b_+92); alu_add(gb, B);
  CYC(b_+92, b_+93); B = A;

L_53d8:
  CYC(b_+93, b_+94); H = D;
  CYC(b_+94, b_+96); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+96, b_+97); mem_wr(gb, HL, B);
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+99, b_+100); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+100, b_+102); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x28);
  CYC(b_+104, b_+106); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(b_+106, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+109);
  CALL_C(b_+109, smasher_updateAngleTowardPosition_hook, SYM(smasher_updateAngleTowardPosition), b_+112);
  CYC(b_+112, b_+119); enemySetAnimation_hook(gb); return; // jp
}

// Moving toward ball on the ground
void smasher_parent_stateA_hook(GB *gb) {
  BASE(smasher_parent_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); smasher_parent_linkPickedUpBall_hook(gb); return; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(b_+13, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+16);
  CYC(b_+16, b_+17); alu_sub(gb, C);
  CYC(b_+17, b_+19); alu_add(gb, 0x02);
  CYC(b_+19, b_+21); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto movingTowardBall; } // jr nc
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = mem_rd(gb, hFF8F);
  CYC(b_+25, b_+26); alu_sub(gb, B);
  CYC(b_+26, b_+28); alu_add(gb, 0x02);
  CYC(b_+28, b_+30); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+30, b_+32); goto movingTowardBall; } // jr nc
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [parent.state]
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);
  CYC(b_+39, b_+41); A = 0x02;
  CALL_C(b_+41, smasher_setOamFlags_hook, SYM(smasher_setOamFlags), b_+44);
  CYC(b_+44, b_+46); A = 0x04;
  CALL_C(b_+46, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+49);
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [ball.state]
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+52, b_+53); C = mem_rd(gb, HL);
  CALL_C(b_+53, smasher_updateAngleTowardPosition_hook, SYM(smasher_updateAngleTowardPosition), b_+56);
  CYC(b_+56, b_+57); A = alu_inc8(gb, A);
  CYC(b_+57, b_+60); enemySetAnimation_hook(gb); return; // jp

movingTowardBall:
  CALL_C(b_+60, ecom_moveTowardPosition_b0f_hook, SYM(ecom_moveTowardPosition_b0f), b_+63);
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CALL_C(b_+66, smasher_updateDirectionFromAngle_hook, SYM(smasher_updateDirectionFromAngle), b_+69);
  CALL_C(b_+69, enemySetAnimation_hook, SYM(enemySetAnimation), b_+72);
  CYC(b_+72, b_+74); C = 0x20;
  CALL_C(b_+74, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+77);
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; } // ret nz
  CYC(b_+77, b_+78);
  CYCT(b_+78, b_+80); smasher_hop_hook(gb); return; // jr
}

void smasher_parent_linkPickedUpBall_hook(GB *gb) {
  BASE(smasher_parent_linkPickedUpBall);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x1e);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x3c);
  CYC(b_+13, b_+15); A = 0x03;
  CALL_C(b_+15, smasher_setOamFlags_hook, SYM(smasher_setOamFlags), b_+18);
  CALL_C(b_+18, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0f), b_+21);
  CYC(b_+21, b_+24); smasher_updateDirectionFromAngle_hook(gb); return; // jp
}

// About to pick up ball
void smasher_parent_stateB_hook(GB *gb) {
  BASE(smasher_parent_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x02); // ENEMYSTATE_GRABBED
  if (F & FZ) { CYCT(b_+8, b_+10); smasher_parent_linkPickedUpBall_hook(gb); return; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x0b);
  if (F & FC) { RET_TAKEN(b_+12); return; } // ret c
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); A = 0x03;
  CALL_C(b_+15, smasher_setOamFlags_hook, SYM(smasher_setOamFlags), b_+18);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x1e);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x0a);
  smasher_hop_hook(gb); return; // fallthrough
}

void smasher_hop_hook(GB *gb) {
  BASE(smasher_hop);
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+2, b_+4); A = 0x40;
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0xff);
  RET(b_+7); return;
}

// Just picked up ball; hopping while moving slowly toward Link
void smasher_parent_stateC_hook(GB *gb) {
  BASE(smasher_parent_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, smasher_updateAngleTowardLink_hook, SYM(smasher_updateAngleTowardLink), b_+3);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CALL_C(b_+4, enemySetAnimation_hook, SYM(enemySetAnimation), b_+7);
  CALL_C(b_+7, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+10);
  CYC(b_+10, b_+12); C = 0x20;
  CALL_C(b_+12, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+17); goto hitGround; } // jr z
  CYC(b_+15, b_+17);
  CALL_C(b_+17, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0f), b_+20);
  CYC(b_+20, b_+22); A = 0x00;
  CALL_C(b_+22, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+25);
  CALL_C(b_+25, objectCopyPosition_hook, SYM(objectCopyPosition), b_+28);
  CYC(b_+28, b_+29); L = alu_dec8(gb, L);
  CYC(b_+29, b_+30); E = L;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE); // [parent.zh]
  CYC(b_+31, b_+33); alu_add(gb, 0xf4);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); // [ball.zh]
  RET(b_+34); return;

hitGround:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+42); smasher_hop_hook(gb); return; } // jp nz
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); A = 0xfe;
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x20);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+50, b_+52); A = 0x02;
  CYC(b_+52, b_+55); smasher_setOamFlags_hook(gb); return; // jp
}

// In midair just before throwing ball
void smasher_parent_stateD_hook(GB *gb) {
  BASE(smasher_parent_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto inMidair; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x08);
  RET(b_+11); return;

inMidair:
  CYC(b_+12, b_+14); A = 0x0f;
  CALL_C(b_+14, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+17);
  CYC(b_+17, b_+18); E = L;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_add(gb, 0xf4);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + 0x15; // Enemy.speedZ+1
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto movingDown; } // jr nc
  CYC(b_+26, b_+28);
  CALL_C(b_+28, smasher_updateAngleTowardLink_hook, SYM(smasher_updateAngleTowardLink), b_+31);
  CYC(b_+31, b_+32); A = alu_inc8(gb, A);
  CYC(b_+32, b_+35); enemySetAnimation_hook(gb); return; // jp

movingDown:
  CYC(b_+35, b_+36); B = A;
  CYC(b_+36, b_+37); E = alu_dec8(gb, E);
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_or(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; } // ret nz
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); A = 0x04;
  CALL_C(b_+42, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+45);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; } // ret nz
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [ball.state]
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+52, b_+53); E = L;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A);
  CYC(b_+55, b_+57); A = 0x03;
  CALL_C(b_+57, smasher_setOamFlags_hook, SYM(smasher_setOamFlags), b_+60);
  CYC(b_+60, b_+62); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+66); enemySetAnimation_hook(gb); return; // jp
}

// @param[out] a direction value
void smasher_updateAngleTowardPosition_hook(GB *gb) {
  BASE(smasher_updateAngleTowardPosition);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYCT(b_+6, b_+8); smasher_updateDirectionFromAngle_hook(gb); return; // jr
}

void smasher_updateAngleTowardLink_hook(GB *gb) {
  BASE(smasher_updateAngleTowardLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  smasher_updateDirectionFromAngle_hook(gb); return; // fallthrough
}

// @param a angle
// @param[out] a direction value
void smasher_updateDirectionFromAngle_hook(GB *gb) {
  BASE(smasher_updateDirectionFromAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = B;
  CYC(b_+5, b_+7); alu_and(gb, 0x10);
  CYC(b_+7, b_+9); alu_xor(gb, 0x10);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rlca(gb);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}

// @param a Value for oamFlags
void smasher_setOamFlags_hook(GB *gb) {
  BASE(smasher_setOamFlags);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  RET(b_+5); return;
}

// Loads positions into bc and hFF8E/hFF8F for subsequent call to
// objectGetRelativeAngleWithTempVars.
// @param h Parent object
void smasher_ball_loadPositions_hook(GB *gb) {
  BASE(smasher_ball_loadPositions);
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); E = L;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8F, A);
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); E = L;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); mem_wr(gb, hFF8E, A);
  CYC(b_+13, b_+14); C = mem_rd(gb, HL);
  RET(b_+14); return;
}

// Updates the ball's "respawn timer" and makes it disappear (goes to state $0d) when it
// hits zero.
void smasher_ball_updateRespawnTimer_hook(GB *gb) {
  BASE(smasher_ball_updateRespawnTimer);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x0d);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wFrameCounter);
  CYC(b_+14, b_+15); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+15); return; } // ret c
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0xb4); // 180
  if (F & FC) { RET_TAKEN(b_+23); return; } // ret c
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x00);
  CALL_C(b_+26, smasher_ball_makeLinkDrop_hook, SYM(smasher_ball_makeLinkDrop), b_+29);
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+31, b_+33); A = 0x0d;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

void smasher_ball_makeLinkDrop_hook(GB *gb) {
  BASE(smasher_ball_makeLinkDrop);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); dropLinkHeldItem_hook(gb); return; // jp
}

void enemyCode74_hook(GB *gb) {
  BASE(enemyCode74);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto normalStatus; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); ecom_updateKnockback_b0f_hook(gb); return; // jp

dead:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto mainDead; } // jr nz
  CYC(b_+17, b_+19);
  CALL_C(b_+19, smasher_ball_makeLinkDrop_hook, SYM(smasher_ball_makeLinkDrop), b_+22);
  CALL_C(b_+22, objectCreatePuff_hook, SYM(objectCreatePuff), b_+25);
  CYC(b_+25, b_+28); enemyDelete_hook(gb); return; // jp

mainDead:
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+32, ecom_killRelatedObj1_b0f_hook, SYM(ecom_killRelatedObj1_b0f), b_+35); else CYC(b_+32, b_+35); // call nz
  CYC(b_+35, b_+38); enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+38, smasher_ball_updateRespawnTimer_hook, SYM(smasher_ball_updateRespawnTimer), b_+41);
  CALL_C(b_+41, ecom_getSubidAndCpStateTo08_b0f_hook, SYM(ecom_getSubidAndCpStateTo08_b0f), b_+44);
  if (F & FC) { CYCT(b_+44, b_+46); goto commonState; } // jr c
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); A = B;
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+48, b_+51); smasher_ball_hook(gb); return; } // jp z
  CYC(b_+48, b_+51);
  CYC(b_+51, b_+54); smasher_parent_hook(gb); return; // jp

commonState:
  CYC(b_+54, b_+55); push_effect(gb, b_+55);
  {
    uint16_t target = smasher_jump_table(gb);
    if (target == SYM(smasher_state_grabbed)) { smasher_state_uninitialized_hook(gb); return; }
    if (target == SYM(smasher_state_grabbed__justGrabbed) || target == b_+43 /*unused*/) { smasher_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}
