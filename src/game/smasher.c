#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x51b9, 0x51ba); A = B;
  CYC(0x51ba, 0x51bb); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x51bb, 0x51bd); goto alreadySpawnedParent; } // jr nz
  CYC(0x51bb, 0x51bd);

  CYC(0x51bd, 0x51be); B = A;
  CYC(0x51be, 0x51c0); A = 0xff;
  CALL_C(0x51c0, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x51c3);
  CYC(0x51c3, 0x51c5); B = 0x74; // ENEMY_SMASHER
  CALL_C(0x51c5, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x51c8);
  if (!(F & FZ)) { RET_TAKEN(0x51c8); return; } // ret nz
  CYC(0x51c8, 0x51c9);
  CYC(0x51c9, 0x51cb); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x51cb, 0x51cc); E = L;
  CYC(0x51cc, 0x51cd); A = mem_rd(gb, DE);
  CYC(0x51cd, 0x51ce); mem_wr(gb, HL, A);
  CYC(0x51ce, 0x51d0); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x51d0, 0x51d1); E = L;
  CYC(0x51d1, 0x51d3); A = ENEMY_BASE; // Enemy.start
  CYC(0x51d3, 0x51d4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x51d4, 0x51d5); mem_wr(gb, DE, A);
  CYC(0x51d5, 0x51d6); E = alu_inc8(gb, E);
  CYC(0x51d6, 0x51d7); A = H;
  CYC(0x51d7, 0x51d8); mem_wr(gb, DE, A);
  CYC(0x51d8, 0x51d9); mem_wr(gb, HL, D);
  CALL_C(0x51d9, objectCopyPosition_hook, 0x2242, 0x51dc);
  CYC(0x51dc, 0x51dd); A = H;
  CYC(0x51dd, 0x51de); alu_cp(gb, D);
  if (!(F & FC)) { CYCT(0x51de, 0x51e0); goto initialize; } // jr nc
  CYC(0x51de, 0x51e0);
  CYC(0x51e0, 0x51e2); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x51e2, 0x51e4); mem_wr(gb, HL, 0x80); // Change former "parent" to "ball"
  CYC(0x51e4, 0x51e5); E = L;
  CYC(0x51e5, 0x51e7); A = 0x01;
  CYC(0x51e7, 0x51e8); mem_wr(gb, DE, A); // Change former "ball" (this) to "parent"

alreadySpawnedParent:
  CYC(0x51e8, 0x51e9); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x51e9, 0x51eb); goto gotoState8; } // jr z
  CYC(0x51e9, 0x51eb);
  CYC(0x51eb, 0x51ed); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x51ed, 0x51ee); alu_xor(gb, A);
  CYC(0x51ee, 0x51ef); mem_wr(gb, DE, A);

initialize:
  CYC(0x51ef, 0x51f1); A = 0x01;
  CALL_C(0x51f1, smasher_setOamFlags_hook, 0x5516, 0x51f4);
  CYC(0x51f4, 0x51f6); L = ENEMY_BASE + OBJ_XH;
  CYC(0x51f6, 0x51f7); A = mem_rd(gb, HL);
  CYC(0x51f7, 0x51f9); alu_sub(gb, 0x20);
  CYC(0x51f9, 0x51fa); mem_wr(gb, HL, A);
  CYC(0x51fa, 0x51fc); A = 0x04;
  CALL_C(0x51fc, enemySetAnimation_hook, 0x282b, 0x51ff);

gotoState8:
  CYC(0x51ff, 0x5202); ecom_setSpeedAndState8AndVisible_b0f_hook(gb); return; // jp
}

void smasher_state_grabbed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5202, 0x5203); E = alu_inc8(gb, E);
  CYC(0x5203, 0x5204); A = mem_rd(gb, DE); // [substate]
  {
    CYC(0x5204, 0x5205); push_effect(gb, 0x5205);
    uint16_t target = smasher_jump_table(gb);
    if (target == 0x520d) goto justGrabbed;
    if (target == 0x5218) goto beingHeld;
    if (target == 0x5219) goto released;
    if (target == 0x525c) goto atRest;
    HANDOFF(target);
  }

justGrabbed:
  CYC(0x520d, 0x520e); H = D;
  CYC(0x520e, 0x520f); L = E;
  CYC(0x520f, 0x5210); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x5210, 0x5212); A = 0x20;
  CYC(0x5212, 0x5215); mem_wr(gb, wLinkGrabState2, A);
  CYC(0x5215, 0x5218); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  RET(0x5218); return;

released:
  CALL_C(0x5219, ecom_bounceOffWallsAndHoles_b0f_hook, 0x42de, 0x521c);
  if (F & FZ) { CYCT(0x521c, 0x521e); goto L_5225; } // jr z
  CYC(0x521c, 0x521e);
  CYC(0x521e, 0x5220); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5220, 0x5221); A = mem_rd(gb, DE);
  CYC(0x5221, 0x5224); SET_HL(w1ReservedItemC_angle);
  CYC(0x5224, 0x5225); mem_wr(gb, HL, A);

L_5225:
  CYC(0x5225, 0x5227); A = 0x2b;
  CALL_C(0x5227, objectGetRelatedObject1Var_hook, 0x2160, 0x522a);
  CYC(0x522a, 0x522b); A = mem_rd(gb, HL);
  CYC(0x522b, 0x522c); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x522c); return; } // ret nz
  CYC(0x522c, 0x522d);
  CYC(0x522d, 0x522f); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x522f, 0x5230); E = L;
  CYC(0x5230, 0x5231); A = mem_rd(gb, DE);
  CYC(0x5231, 0x5232); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5232, 0x5234); alu_add(gb, 0x08);
  CYC(0x5234, 0x5236); alu_cp(gb, 0x11);
  if (!(F & FC)) { RET_TAKEN(0x5236); return; } // ret nc
  CYC(0x5236, 0x5237);
  CALL_C(0x5237, checkObjectsCollided_hook, 0x1d5a, 0x523a);
  if (!(F & FC)) { RET_TAKEN(0x523a); return; } // ret nc
  CYC(0x523a, 0x523b);
  CYC(0x523b, 0x523d); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x523d, 0x523f); mem_wr(gb, HL, 0x20);
  CYC(0x523f, 0x5241); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x5241, 0x5243); mem_wr(gb, HL, 0x10);
  CYC(0x5243, 0x5245); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5245, 0x5246); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x5246, smasher_ball_loadPositions_hook, 0x551c, 0x5249);
  CYC(0x5249, 0x524a); push_effect(gb, HL);
  CALL_C(0x524a, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x524d);
  CYC(0x524d, 0x524e); SET_HL(pop_effect(gb));
  CYC(0x524e, 0x5250); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x5250, 0x5251); mem_wr(gb, HL, A);
  CYC(0x5251, 0x5253); alu_xor(gb, 0x10);
  CYC(0x5253, 0x5256); SET_HL(w1ReservedItemC_angle);
  CYC(0x5256, 0x5257); mem_wr(gb, HL, A);
  CYC(0x5257, 0x5259); A = 0x63; // SND_BOSS_DAMAGE
  CYC(0x5259, 0x525c); playSound_b00_hook(gb); return; // jp

atRest:
  CYC(0x525c, 0x525d); E = alu_dec8(gb, E);
  CYC(0x525d, 0x525f); A = 0x08;
  CYC(0x525f, 0x5260); mem_wr(gb, DE, A); // [state]
  CYC(0x5260, 0x5263); objectSetVisiblec2_hook(gb); return; // jp
}

void smasher_state_stub_hook(GB *gb) {
  RET(0x5263); return;
}

void smasher_ball_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5264, 0x5265); A = mem_rd(gb, DE);
  CYC(0x5265, 0x5267); alu_sub(gb, 0x08);
  CYC(0x5267, 0x5268); push_effect(gb, 0x5268);
  {
    uint16_t target = smasher_jump_table(gb);
    if (target == 0x5278) { smasher_ball_state8_hook(gb); return; }
    if (target == 0x5283) { smasher_ball_state9_hook(gb); return; }
    if (target == 0x5289) { smasher_ball_stateA_hook(gb); return; }
    if (target == 0x52bf) { smasher_ball_stateB_hook(gb); return; }
    if (target == 0x52c0) { smasher_ball_stateC_hook(gb); return; }
    if (target == 0x52e9) { smasher_ball_stateD_hook(gb); return; }
    if (target == 0x530d) { smasher_ball_stateE_hook(gb); return; }
    if (target == 0x5343) { smasher_ball_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization (or just reappeared after disappearing)
void smasher_ball_state8_hook(GB *gb) {
  CYC(0x5278, 0x5279); H = D;
  CYC(0x5279, 0x527a); L = E;
  CYC(0x527a, 0x527b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x527b, 0x527d); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x527d, 0x527f); mem_wr(gb, HL, 0x63); // ENEMYCOLLISION_SMASHER_BALL
  CYC(0x527f, 0x5281); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5281, 0x5283); mem_wr(gb, HL, 0x19); // SPEED_a0
  smasher_ball_state9_hook(gb); return; // fallthrough
}

// Lying on ground, waiting for parent or Link to pick it up
void smasher_ball_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5283, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x5286);
  CYC(0x5286, 0x5289); objectPushLinkAwayOnCollision_hook(gb); return; // jp
}

// Parent is picking up the ball
void smasher_ball_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5289, 0x528a); H = D;
  CYC(0x528a, 0x528c); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x528c, 0x528d); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x528d, 0x528f); alu_cp(gb, 0xf4);
  if (F & FZ) { CYCT(0x528f, 0x5291); goto L_5299; } // jr z
  CYC(0x528f, 0x5291);
  CYC(0x5291, 0x5292); A = mem_rd(gb, HL);
  CYC(0x5292, 0x5294); alu_sub(gb, 0x80);
  CYC(0x5294, 0x5295); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5295, 0x5296); A = mem_rd(gb, HL);
  CYC(0x5296, 0x5298); alu_sbc(gb, 0x00);
  CYC(0x5298, 0x5299); mem_wr(gb, HL, A);

L_5299:
  CYC(0x5299, 0x529b); A = 0x0b;
  CALL_C(0x529b, objectGetRelatedObject1Var_hook, 0x2160, 0x529e);
  CALL_C(0x529e, smasher_ball_loadPositions_hook, 0x551c, 0x52a1);
  CYC(0x52a1, 0x52a2); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x52a2, 0x52a5); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nz
  CYC(0x52a2, 0x52a5);
  CYC(0x52a5, 0x52a7); A = hram_rd(gb, 0x8f);
  CYC(0x52a7, 0x52a8); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x52a8, 0x52ab); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nz
  CYC(0x52a8, 0x52ab);
  CYC(0x52ab, 0x52ad); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x52ad, 0x52ae); A = mem_rd(gb, DE);
  CYC(0x52ae, 0x52b0); alu_cp(gb, 0xf4);
  if (!(F & FZ)) { RET_TAKEN(0x52b0); return; } // ret nz
  CYC(0x52b0, 0x52b1);
  CALL_C(0x52b1, ecom_incState_b0f_hook, 0x4000, 0x52b4);
  CYC(0x52b4, 0x52b6); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x52b6, 0x52b8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x52b8, 0x52ba); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x52ba, 0x52bc); mem_wr(gb, HL, 0x78);
  CYC(0x52bc, 0x52bf); objectSetVisiblec1_hook(gb); return; // jp
}

// This state is a signal for the parent, which will update the ball's state when it gets released.
void smasher_ball_stateB_hook(GB *gb) {
  RET(0x52bf); return;
}

// Being thrown
void smasher_ball_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52c0, 0x52c2); C = 0x20;
  CALL_C(0x52c2, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x52c5);
  if (F & FC) { CYCT(0x52c5, 0x52c7); goto doneBouncing; } // jr c
  CYC(0x52c5, 0x52c7);
  if (!(F & FZ)) { CYCT(0x52c7, 0x52c9); goto L_52d2; } // jr nz
  CYC(0x52c7, 0x52c9);
  CYC(0x52c9, 0x52cb); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x52cb, 0x52cc); A = mem_rd(gb, DE);
  CYC(0x52cc, 0x52ce); A = alu_srl(gb, A);
  CYC(0x52ce, 0x52cf); mem_wr(gb, DE, A);
  CALL_C(0x52cf, smasher_ball_playLandSound_hook, 0x52e4, 0x52d2);

L_52d2:
  CALL_C(0x52d2, ecom_bounceOffWallsAndHoles_b0f_hook, 0x42de, 0x52d5);
  CYC(0x52d5, 0x52d8); objectApplySpeed_hook(gb); return; // jp

doneBouncing:
  CYC(0x52d8, 0x52d9); H = D;
  CYC(0x52d9, 0x52db); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x52db, 0x52dd); mem_wr(gb, HL, 0x08);
  CYC(0x52dd, 0x52df); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x52df, 0x52e1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(0x52e1, objectSetVisiblec2_hook, 0x1e45, 0x52e4);
  smasher_ball_playLandSound_hook(gb); return; // fallthrough
}

void smasher_ball_playLandSound_hook(GB *gb) {
  CYC(0x52e4, 0x52e6); A = 0x52; // SND_BOMB_LAND
  CYC(0x52e6, 0x52e9); playSound_b00_hook(gb); return; // jp
}

// Disappearing (either after being thrown, or after a time limit)
void smasher_ball_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x52e9, objectCreatePuff_hook, 0x24c1, 0x52ec);
  if (!(F & FZ)) { RET_TAKEN(0x52ec); return; } // ret nz
  CYC(0x52ec, 0x52ed);
  CYC(0x52ed, 0x52ef); A = 0x04;
  CALL_C(0x52ef, objectGetRelatedObject1Var_hook, 0x2160, 0x52f2);
  CYC(0x52f2, 0x52f3); A = mem_rd(gb, HL);
  CYC(0x52f3, 0x52f5); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(0x52f5, 0x52f7); goto L_52ff; } // jr c
  CYC(0x52f5, 0x52f7);
  CYC(0x52f7, 0x52f9); mem_wr(gb, HL, 0x0d); // [parent.state]
  CYC(0x52f9, 0x52fb); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x52fb, 0x52fd); A = 0x03;
  CYC(0x52fd, 0x52fe); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x52fe, 0x52ff); mem_wr(gb, HL, A);

L_52ff:
  CALL_C(0x52ff, ecom_incState_b0f_hook, 0x4000, 0x5302);
  CYC(0x5302, 0x5304); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5304, 0x5306); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x5306, 0x5308); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5308, 0x530a); mem_wr(gb, HL, 0x3c);
  CYC(0x530a, 0x530d); objectSetInvisible_hook(gb); return; // jp
}

// Ball is gone, will reappear after [counter1] frames
void smasher_ball_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x530d, ecom_decCounter1_b0f_hook, 0x439a, 0x5310);
  if (!(F & FZ)) { RET_TAKEN(0x5310); return; } // ret nz
  CYC(0x5310, 0x5311);
  CYC(0x5311, 0x5312); L = E;
  CYC(0x5312, 0x5313); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5313, 0x5315); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5315, 0x5317); mem_wr(gb, HL, 0xe0); // -$20
  CYC(0x5317, 0x5319); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5319, 0x531a); alu_xor(gb, A);
  CYC(0x531a, 0x531b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x531b, 0x531c); mem_wr(gb, HL, A);
  CALL_C(0x531c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x531f);
  CYC(0x531f, 0x5321); alu_and(gb, 0x0e);
  CYC(0x5321, 0x5324); SET_HL(0x5333);
  CYC(0x5324, 0x5325); smasher_addAToHl_from_rst(gb, 0x5325);
  CYC(0x5325, 0x5327); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5327, 0x5328); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5328, 0x5329); mem_wr(gb, DE, A);
  CYC(0x5329, 0x532b); E = ENEMY_BASE + OBJ_XH;
  CYC(0x532b, 0x532c); A = mem_rd(gb, HL);
  CYC(0x532c, 0x532d); mem_wr(gb, DE, A);
  CALL_C(0x532d, objectCreatePuff_hook, 0x24c1, 0x5330);
  CYC(0x5330, 0x5333); objectSetVisiblec1_hook(gb); return; // jp
}

// Ball is falling to ground after reappearing
void smasher_ball_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5343, 0x5345); C = 0x20;
  CALL_C(0x5345, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x5348);
  if (F & FC) { CYCT(0x5348, 0x534a); goto doneBouncing; } // jr c
  CYC(0x5348, 0x534a);
  if (!(F & FZ)) { RET_TAKEN(0x534a); return; } // ret nz
  CYC(0x534a, 0x534b);
  CYC(0x534b, 0x534e); smasher_ball_playLandSound_hook(gb); return; // jp

doneBouncing:
  CYC(0x534e, 0x5350); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5350, 0x5352); A = 0x08;
  CYC(0x5352, 0x5353); mem_wr(gb, DE, A);
  CYC(0x5353, 0x5356); objectSetVisiblec2_hook(gb); return; // jp
}

void smasher_parent_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5356, 0x5357); A = mem_rd(gb, DE);
  CYC(0x5357, 0x5359); alu_sub(gb, 0x08);
  CYC(0x5359, 0x535a); push_effect(gb, 0x535a);
  {
    uint16_t target = smasher_jump_table(gb);
    if (target == 0x5366) { smasher_parent_state8_hook(gb); return; }
    if (target == 0x537b) { smasher_parent_state9_hook(gb); return; }
    if (target == 0x53f2) { smasher_parent_stateA_hook(gb); return; }
    if (target == 0x545a) { smasher_parent_stateB_hook(gb); return; }
    if (target == 0x547f) { smasher_parent_stateC_hook(gb); return; }
    if (target == 0x54b6) { smasher_parent_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Initialization
void smasher_parent_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5366, 0x5367); H = D;
  CYC(0x5367, 0x5368); L = E;
  CYC(0x5368, 0x5369); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5369, 0x536b); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x536b, 0x536d); mem_wr(gb, HL, 0x01);
  CYC(0x536d, 0x536f); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x536f, 0x5371); mem_wr(gb, HL, 0x1e);
  CYC(0x5371, 0x5373); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5373, 0x5375); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5375, 0x5377); smasher_parent_state9_hook(gb); return; } // jr nz
  CYC(0x5375, 0x5377);
  CYC(0x5377, 0x5378); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [var32]
  CALL_C(0x5378, enemyBoss_beginMiniboss_b0f_hook, 0x4580, 0x537b);
  smasher_parent_state9_hook(gb); return; // fallthrough
}

void smasher_parent_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x537b, 0x537d); A = 0x04;
  CALL_C(0x537d, objectGetRelatedObject1Var_hook, 0x2160, 0x5380);
  CYC(0x5380, 0x5381); A = mem_rd(gb, HL);
  CYC(0x5381, 0x5383); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(0x5383, 0x5385); goto moveTowardBall; } // jr z
  CYC(0x5383, 0x5385);
  CALL_C(0x5385, ecom_decCounter1_b0f_hook, 0x439a, 0x5388);
  if (!(F & FZ)) { CYCT(0x5388, 0x538a); goto updateMovement; } // jr nz
  CYC(0x5388, 0x538a);
  CYC(0x538a, 0x538c); mem_wr(gb, HL, 0x3c); // [counter1]
  CALL_C(0x538c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x538f);
  CYC(0x538f, 0x5391); alu_and(gb, 0x03);
  CYC(0x5391, 0x5394); SET_HL(0x53ee);
  CYC(0x5394, 0x5395); smasher_addAToHl_from_rst(gb, 0x5395);
  CYC(0x5395, 0x5397); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5397, 0x5398); A = mem_rd(gb, HL);
  CYC(0x5398, 0x5399); mem_wr(gb, DE, A);
  CALL_C(0x5399, smasher_updateDirectionFromAngle_hook, 0x5506, 0x539c);

updateMovement:
  CALL_C(0x539c, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, 0x4156, 0x539f);
  CYC(0x539f, 0x53a1); C = 0x20;
  CALL_C(0x53a1, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x53a4);
  if (!(F & FZ)) { CYCT(0x53a4, 0x53a6); goto updateAnim; } // jr nz
  CYC(0x53a4, 0x53a6);
  CALL_C(0x53a6, smasher_hop_hook, 0x5477, 0x53a9);
  CYC(0x53a9, 0x53ab); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x53ab, 0x53ac); A = mem_rd(gb, DE);
  CYC(0x53ac, 0x53ad); A = alu_inc8(gb, A);
  CYC(0x53ad, 0x53b0); enemySetAnimation_hook(gb); return; // jp

updateAnim:
  CYC(0x53b0, 0x53b1); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl) [speedZ+1]
  CYC(0x53b1, 0x53b2); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x53b2); return; } // ret nz
  CYC(0x53b2, 0x53b3);
  CYC(0x53b3, 0x53b5); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x53b5, 0x53b6); A = mem_rd(gb, DE);
  CYC(0x53b6, 0x53b9); enemySetAnimation_hook(gb); return; // jp

moveTowardBall:
  CYC(0x53b9, 0x53bb); L = ENEMY_BASE + OBJ_YH;
  CYC(0x53bb, 0x53bd); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x53bd, 0x53be); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x53be, 0x53bf); mem_wr(gb, DE, A);
  CYC(0x53bf, 0x53c0); L = alu_inc8(gb, L);
  CYC(0x53c0, 0x53c1); E = L;
  CYC(0x53c1, 0x53c2); A = mem_rd(gb, DE); // [parent.xh]
  CYC(0x53c2, 0x53c3); alu_cp(gb, mem_rd(gb, HL)); // [ball.xh]
  CYC(0x53c3, 0x53c5); A = 0x0e;
  if (!(F & FC)) { CYCT(0x53c5, 0x53c7); goto L_53c9; } // jr nc
  CYC(0x53c5, 0x53c7);
  CYC(0x53c7, 0x53c9); A = 0xf2; // -$0e

L_53c9:
  CYC(0x53c9, 0x53ca); C = A;
  CYC(0x53ca, 0x53cb); alu_add(gb, mem_rd(gb, HL)); // [ball.xh]
  CYC(0x53cb, 0x53cc); B = A;
  CYC(0x53cc, 0x53ce); alu_sub(gb, 0x18);
  CYC(0x53ce, 0x53d0); alu_cp(gb, 0xc0);
  if (F & FC) { CYCT(0x53d0, 0x53d2); goto L_53d8; } // jr c
  CYC(0x53d0, 0x53d2);
  CYC(0x53d2, 0x53d3); A = C;
  CYC(0x53d3, 0x53d4); alu_cpl(gb);
  CYC(0x53d4, 0x53d5); A = alu_inc8(gb, A);
  CYC(0x53d5, 0x53d6); alu_add(gb, A);
  CYC(0x53d6, 0x53d7); alu_add(gb, B);
  CYC(0x53d7, 0x53d8); B = A;

L_53d8:
  CYC(0x53d8, 0x53d9); H = D;
  CYC(0x53d9, 0x53db); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x53db, 0x53dc); mem_wr(gb, HL, B);
  CYC(0x53dc, 0x53de); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x53de, 0x53df); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x53df, 0x53e1); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x53e1, 0x53e3); mem_wr(gb, HL, 0x28);
  CYC(0x53e3, 0x53e5); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(0x53e5, ecom_readPositionVars_b0f_hook, 0x4439, 0x53e8);
  CALL_C(0x53e8, smasher_updateAngleTowardPosition_hook, 0x54f8, 0x53eb);
  CYC(0x53eb, 0x53f2); enemySetAnimation_hook(gb); return; // jp
}

// Moving toward ball on the ground
void smasher_parent_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53f2, 0x53f4); A = 0x04;
  CALL_C(0x53f4, objectGetRelatedObject1Var_hook, 0x2160, 0x53f7);
  CYC(0x53f7, 0x53f8); A = mem_rd(gb, HL);
  CYC(0x53f8, 0x53fa); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(0x53fa, 0x53fc); smasher_parent_linkPickedUpBall_hook(gb); return; } // jr nz
  CYC(0x53fa, 0x53fc);
  CYC(0x53fc, 0x53fd); H = D;
  CYC(0x53fd, 0x53ff); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(0x53ff, ecom_readPositionVars_b0f_hook, 0x4439, 0x5402);
  CYC(0x5402, 0x5403); alu_sub(gb, C);
  CYC(0x5403, 0x5405); alu_add(gb, 0x02);
  CYC(0x5405, 0x5407); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x5407, 0x5409); goto movingTowardBall; } // jr nc
  CYC(0x5407, 0x5409);
  CYC(0x5409, 0x540b); A = hram_rd(gb, 0x8f);
  CYC(0x540b, 0x540c); alu_sub(gb, B);
  CYC(0x540c, 0x540e); alu_add(gb, 0x02);
  CYC(0x540e, 0x5410); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x5410, 0x5412); goto movingTowardBall; } // jr nc
  CYC(0x5410, 0x5412);
  CYC(0x5412, 0x5414); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5414, 0x5415); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [parent.state]
  CYC(0x5415, 0x5417); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5417, 0x5419); mem_wr(gb, HL, 0x00);
  CYC(0x5419, 0x541b); A = 0x02;
  CALL_C(0x541b, smasher_setOamFlags_hook, 0x5516, 0x541e);
  CYC(0x541e, 0x5420); A = 0x04;
  CALL_C(0x5420, objectGetRelatedObject1Var_hook, 0x2160, 0x5423);
  CYC(0x5423, 0x5424); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [ball.state]
  CYC(0x5424, 0x5426); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5426, 0x5427); C = mem_rd(gb, HL);
  CALL_C(0x5427, smasher_updateAngleTowardPosition_hook, 0x54f8, 0x542a);
  CYC(0x542a, 0x542b); A = alu_inc8(gb, A);
  CYC(0x542b, 0x542e); enemySetAnimation_hook(gb); return; // jp

movingTowardBall:
  CALL_C(0x542e, ecom_moveTowardPosition_b0f_hook, 0x4430, 0x5431);
  CYC(0x5431, 0x5433); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5433, 0x5434); A = mem_rd(gb, DE);
  CALL_C(0x5434, smasher_updateDirectionFromAngle_hook, 0x5506, 0x5437);
  CALL_C(0x5437, enemySetAnimation_hook, 0x282b, 0x543a);
  CYC(0x543a, 0x543c); C = 0x20;
  CALL_C(0x543c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x543f);
  if (!(F & FZ)) { RET_TAKEN(0x543f); return; } // ret nz
  CYC(0x543f, 0x5440);
  CYCT(0x5440, 0x5442); smasher_hop_hook(gb); return; // jr
}

void smasher_parent_linkPickedUpBall_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5442, 0x5443); H = D;
  CYC(0x5443, 0x5445); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5445, 0x5447); mem_wr(gb, HL, 0x09);
  CYC(0x5447, 0x5449); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5449, 0x544b); mem_wr(gb, HL, 0x1e);
  CYC(0x544b, 0x544d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x544d, 0x544f); mem_wr(gb, HL, 0x3c);
  CYC(0x544f, 0x5451); A = 0x03;
  CALL_C(0x5451, smasher_setOamFlags_hook, 0x5516, 0x5454);
  CALL_C(0x5454, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, 0x43ab, 0x5457);
  CYC(0x5457, 0x545a); smasher_updateDirectionFromAngle_hook(gb); return; // jp
}

// About to pick up ball
void smasher_parent_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x545a, 0x545c); A = 0x04;
  CALL_C(0x545c, objectGetRelatedObject1Var_hook, 0x2160, 0x545f);
  CYC(0x545f, 0x5460); A = mem_rd(gb, HL);
  CYC(0x5460, 0x5462); alu_cp(gb, 0x02); // ENEMYSTATE_GRABBED
  if (F & FZ) { CYCT(0x5462, 0x5464); smasher_parent_linkPickedUpBall_hook(gb); return; } // jr z
  CYC(0x5462, 0x5464);
  CYC(0x5464, 0x5466); alu_cp(gb, 0x0b);
  if (F & FC) { RET_TAKEN(0x5466); return; } // ret c
  CYC(0x5466, 0x5467);
  CYC(0x5467, 0x5469); A = 0x03;
  CALL_C(0x5469, smasher_setOamFlags_hook, 0x5516, 0x546c);
  CYC(0x546c, 0x546e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x546e, 0x546f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x546f, 0x5471); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5471, 0x5473); mem_wr(gb, HL, 0x1e);
  CYC(0x5473, 0x5475); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5475, 0x5477); mem_wr(gb, HL, 0x0a);
  smasher_hop_hook(gb); return; // fallthrough
}

void smasher_hop_hook(GB *gb) {
  CYC(0x5477, 0x5479); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5479, 0x547b); A = 0x40;
  CYC(0x547b, 0x547c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x547c, 0x547e); mem_wr(gb, HL, 0xff);
  RET(0x547e); return;
}

// Just picked up ball; hopping while moving slowly toward Link
void smasher_parent_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x547f, smasher_updateAngleTowardLink_hook, 0x5500, 0x5482);
  CYC(0x5482, 0x5483); A = alu_inc8(gb, A);
  CALL_C(0x5483, enemySetAnimation_hook, 0x282b, 0x5486);
  CALL_C(0x5486, ecom_decCounter2_b0f_hook, 0x43a3, 0x5489);
  CYC(0x5489, 0x548b); C = 0x20;
  CALL_C(0x548b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x548e);
  if (F & FZ) { CYCT(0x548e, 0x5490); goto hitGround; } // jr z
  CYC(0x548e, 0x5490);
  CALL_C(0x5490, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, 0x4156, 0x5493);
  CYC(0x5493, 0x5495); A = 0x00;
  CALL_C(0x5495, objectGetRelatedObject1Var_hook, 0x2160, 0x5498);
  CALL_C(0x5498, objectCopyPosition_hook, 0x2242, 0x549b);
  CYC(0x549b, 0x549c); L = alu_dec8(gb, L);
  CYC(0x549c, 0x549d); E = L;
  CYC(0x549d, 0x549e); A = mem_rd(gb, DE); // [parent.zh]
  CYC(0x549e, 0x54a0); alu_add(gb, 0xf4);
  CYC(0x54a0, 0x54a1); mem_wr(gb, HL, A); // [ball.zh]
  RET(0x54a1); return;

hitGround:
  CYC(0x54a2, 0x54a4); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x54a4, 0x54a5); A = mem_rd(gb, DE);
  CYC(0x54a5, 0x54a6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x54a6, 0x54a9); smasher_hop_hook(gb); return; } // jp nz
  CYC(0x54a6, 0x54a9);
  CYC(0x54a9, 0x54ab); A = 0xfe;
  CYC(0x54ab, 0x54ac); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x54ac, 0x54ae); mem_wr(gb, HL, 0x20);
  CYC(0x54ae, 0x54b0); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x54b0, 0x54b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x54b1, 0x54b3); A = 0x02;
  CYC(0x54b3, 0x54b6); smasher_setOamFlags_hook(gb); return; // jp
}

// In midair just before throwing ball
void smasher_parent_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54b6, 0x54b8); C = 0x20;
  CALL_C(0x54b8, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x54bb);
  if (!(F & FZ)) { CYCT(0x54bb, 0x54bd); goto inMidair; } // jr nz
  CYC(0x54bb, 0x54bd);
  CYC(0x54bd, 0x54bf); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x54bf, 0x54c1); mem_wr(gb, HL, 0x08);
  RET(0x54c1); return;

inMidair:
  CYC(0x54c2, 0x54c4); A = 0x0f;
  CALL_C(0x54c4, objectGetRelatedObject1Var_hook, 0x2160, 0x54c7);
  CYC(0x54c7, 0x54c8); E = L;
  CYC(0x54c8, 0x54c9); A = mem_rd(gb, DE);
  CYC(0x54c9, 0x54cb); alu_add(gb, 0xf4);
  CYC(0x54cb, 0x54cc); mem_wr(gb, HL, A);
  CYC(0x54cc, 0x54ce); E = ENEMY_BASE + 0x15; // Enemy.speedZ+1
  CYC(0x54ce, 0x54cf); A = mem_rd(gb, DE);
  CYC(0x54cf, 0x54d0); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x54d0, 0x54d2); goto movingDown; } // jr nc
  CYC(0x54d0, 0x54d2);
  CALL_C(0x54d2, smasher_updateAngleTowardLink_hook, 0x5500, 0x54d5);
  CYC(0x54d5, 0x54d6); A = alu_inc8(gb, A);
  CYC(0x54d6, 0x54d9); enemySetAnimation_hook(gb); return; // jp

movingDown:
  CYC(0x54d9, 0x54da); B = A;
  CYC(0x54da, 0x54db); E = alu_dec8(gb, E);
  CYC(0x54db, 0x54dc); A = mem_rd(gb, DE);
  CYC(0x54dc, 0x54dd); alu_or(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x54dd); return; } // ret nz
  CYC(0x54dd, 0x54de);
  CYC(0x54de, 0x54e0); A = 0x04;
  CALL_C(0x54e0, objectGetRelatedObject1Var_hook, 0x2160, 0x54e3);
  CYC(0x54e3, 0x54e4); A = mem_rd(gb, HL);
  CYC(0x54e4, 0x54e6); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { RET_TAKEN(0x54e6); return; } // ret nz
  CYC(0x54e6, 0x54e7);
  CYC(0x54e7, 0x54e8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [ball.state]
  CYC(0x54e8, 0x54ea); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x54ea, 0x54eb); E = L;
  CYC(0x54eb, 0x54ec); A = mem_rd(gb, DE);
  CYC(0x54ec, 0x54ed); mem_wr(gb, HL, A);
  CYC(0x54ed, 0x54ef); A = 0x03;
  CALL_C(0x54ef, smasher_setOamFlags_hook, 0x5516, 0x54f2);
  CYC(0x54f2, 0x54f4); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x54f4, 0x54f5); A = mem_rd(gb, DE);
  CYC(0x54f5, 0x54f8); enemySetAnimation_hook(gb); return; // jp
}

// @param[out] a direction value
void smasher_updateAngleTowardPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x54f8, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x54fb);
  CYC(0x54fb, 0x54fd); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x54fd, 0x54fe); mem_wr(gb, DE, A);
  CYCT(0x54fe, 0x5500); smasher_updateDirectionFromAngle_hook(gb); return; // jr
}

void smasher_updateAngleTowardLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5500, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5503);
  CYC(0x5503, 0x5505); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5505, 0x5506); mem_wr(gb, DE, A);
  smasher_updateDirectionFromAngle_hook(gb); return; // fallthrough
}

// @param a angle
// @param[out] a direction value
void smasher_updateDirectionFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5506, 0x5507); B = A;
  CYC(0x5507, 0x5509); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x5509); return; } // ret z
  CYC(0x5509, 0x550a);
  CYC(0x550a, 0x550b); A = B;
  CYC(0x550b, 0x550d); alu_and(gb, 0x10);
  CYC(0x550d, 0x550f); alu_xor(gb, 0x10);
  CYC(0x550f, 0x5511); A = alu_swap(gb, A);
  CYC(0x5511, 0x5512); alu_rlca(gb);
  CYC(0x5512, 0x5514); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5514, 0x5515); mem_wr(gb, DE, A);
  RET(0x5515); return;
}

// @param a Value for oamFlags
void smasher_setOamFlags_hook(GB *gb) {
  CYC(0x5516, 0x5517); H = D;
  CYC(0x5517, 0x5519); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x5519, 0x551a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x551a, 0x551b); mem_wr(gb, HL, A);
  RET(0x551b); return;
}

// Loads positions into bc and hFF8E/hFF8F for subsequent call to
// objectGetRelativeAngleWithTempVars.
// @param h Parent object
void smasher_ball_loadPositions_hook(GB *gb) {
  CYC(0x551c, 0x551e); L = ENEMY_BASE + OBJ_YH;
  CYC(0x551e, 0x551f); E = L;
  CYC(0x551f, 0x5520); A = mem_rd(gb, DE);
  CYC(0x5520, 0x5522); hram_wr(gb, 0x8f, A);
  CYC(0x5522, 0x5523); B = mem_rd(gb, HL);
  CYC(0x5523, 0x5525); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5525, 0x5526); E = L;
  CYC(0x5526, 0x5527); A = mem_rd(gb, DE);
  CYC(0x5527, 0x5529); hram_wr(gb, 0x8e, A);
  CYC(0x5529, 0x552a); C = mem_rd(gb, HL);
  RET(0x552a); return;
}

// Updates the ball's "respawn timer" and makes it disappear (goes to state $0d) when it
// hits zero.
void smasher_ball_updateRespawnTimer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x552b, 0x552d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x552d, 0x552e); A = mem_rd(gb, DE);
  CYC(0x552e, 0x552f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x552f); return; } // ret nz
  CYC(0x552f, 0x5530);
  CYC(0x5530, 0x5532); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5532, 0x5533); A = mem_rd(gb, DE);
  CYC(0x5533, 0x5535); alu_cp(gb, 0x0d);
  if (!(F & FC)) { RET_TAKEN(0x5535); return; } // ret nc
  CYC(0x5535, 0x5536);
  CYC(0x5536, 0x5539); A = mem_rd(gb, wFrameCounter);
  CYC(0x5539, 0x553a); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x553a); return; } // ret c
  CYC(0x553a, 0x553b);
  CYC(0x553b, 0x553c); H = D;
  CYC(0x553c, 0x553e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x553e, 0x553f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x553f, 0x5540); A = mem_rd(gb, HL);
  CYC(0x5540, 0x5542); alu_cp(gb, 0xb4); // 180
  if (F & FC) { RET_TAKEN(0x5542); return; } // ret c
  CYC(0x5542, 0x5543);
  CYC(0x5543, 0x5545); mem_wr(gb, HL, 0x00);
  CALL_C(0x5545, smasher_ball_makeLinkDrop_hook, 0x554e, 0x5548);
  CYC(0x5548, 0x554a); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x554a, 0x554c); A = 0x0d;
  CYC(0x554c, 0x554d); mem_wr(gb, DE, A);
  RET(0x554d); return;
}

void smasher_ball_makeLinkDrop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x554e, 0x5550); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5550, 0x5551); A = mem_rd(gb, DE);
  CYC(0x5551, 0x5553); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x5553); return; } // ret nz
  CYC(0x5553, 0x5554);
  CYC(0x5554, 0x5555); E = alu_inc8(gb, E);
  CYC(0x5555, 0x5556); A = mem_rd(gb, DE);
  CYC(0x5556, 0x5558); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(0x5558); return; } // ret nc
  CYC(0x5558, 0x5559);
  CYC(0x5559, 0x555c); dropLinkHeldItem_hook(gb); return; // jp
}

void enemyCode74_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5172, 0x5174); goto normalStatus; } // jr z
  CYC(0x5172, 0x5174);
  CYC(0x5174, 0x5176); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5176); return; } // ret c
  CYC(0x5176, 0x5177);
  if (F & FZ) { CYCT(0x5177, 0x5179); goto dead; } // jr z
  CYC(0x5177, 0x5179);
  CYC(0x5179, 0x517a); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x517a, 0x517c); goto normalStatus; } // jr z
  CYC(0x517a, 0x517c);
  CYC(0x517c, 0x517f); ecom_updateKnockback_b0f_hook(gb); return; // jp

dead:
  CYC(0x517f, 0x5181); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5181, 0x5182); A = mem_rd(gb, DE);
  CYC(0x5182, 0x5183); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5183, 0x5185); goto mainDead; } // jr nz
  CYC(0x5183, 0x5185);
  CALL_C(0x5185, smasher_ball_makeLinkDrop_hook, 0x554e, 0x5188);
  CALL_C(0x5188, objectCreatePuff_hook, 0x24c1, 0x518b);
  CYC(0x518b, 0x518e); enemyDelete_hook(gb); return; // jp

mainDead:
  CYC(0x518e, 0x5190); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5190, 0x5191); A = mem_rd(gb, DE);
  CYC(0x5191, 0x5192); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x5192, ecom_killRelatedObj1_b0f_hook, 0x446d, 0x5195); else CYC(0x5192, 0x5195); // call nz
  CYC(0x5195, 0x5198); enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5198, smasher_ball_updateRespawnTimer_hook, 0x552b, 0x519b);
  CALL_C(0x519b, ecom_getSubidAndCpStateTo08_b0f_hook, 0x4426, 0x519e);
  if (F & FC) { CYCT(0x519e, 0x51a0); goto commonState; } // jr c
  CYC(0x519e, 0x51a0);
  CYC(0x51a0, 0x51a1); A = B;
  CYC(0x51a1, 0x51a2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x51a2, 0x51a5); smasher_ball_hook(gb); return; } // jp z
  CYC(0x51a2, 0x51a5);
  CYC(0x51a5, 0x51a8); smasher_parent_hook(gb); return; // jp

commonState:
  CYC(0x51a8, 0x51a9); push_effect(gb, 0x51a9);
  {
    uint16_t target = smasher_jump_table(gb);
    if (target == 0x5202) { smasher_state_uninitialized_hook(gb); return; }
    if (target == 0x520d || target == 0x519d /*unused*/) { smasher_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}
