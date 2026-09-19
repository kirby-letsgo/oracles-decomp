#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void ecom_setSpeedAndState8_b0f_hook(GB *gb);
void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb);
void ecom_decCounter1_b0f_hook(GB *gb);
void ecom_decCounter2_b0f_hook(GB *gb);
void ecom_updateAngleTowardTarget_b0f_hook(GB *gb);
void ecom_updateCardinalAngleAwayFromTarget_b0f_hook(GB *gb);
void ecom_spawnProjectile_b0f_hook(GB *gb);
void ecom_incState_b0f_hook(GB *gb);
void ecom_flickerVisibility_b0f_hook(GB *gb);
void ecom_readPositionVars_b0f_hook(GB *gb);
void ecom_moveTowardPosition_b0f_hook(GB *gb);
void ecom_getSideviewAdjacentWallsBitset_b0f_hook(GB *gb);
void ecom_applyGivenVelocity_b0f_hook(GB *gb);
void ecom_spawnUncountedEnemyWithSubid01_b0f_hook(GB *gb);

void enemyBoss_initializeRoom_b0f_hook(GB *gb);
void enemyBoss_dead_b0f_hook(GB *gb);

void enemyCode75_hook(GB *gb);
void vire_state_uninitialized_hook(GB *gb);
void vire_state_stub_hook(GB *gb);
void vire_mainForm_hook(GB *gb);
void vire_mainForm_state8_hook(GB *gb);
void vire_mainForm_state9_hook(GB *gb);
void vire_mainForm_stateA_hook(GB *gb);
void vire_mainForm_stateB_hook(GB *gb);
void vire_mainForm_stateB_substate0_hook(GB *gb);
void vire_mainForm_stateC_hook(GB *gb);
void vire_mainForm_stateD_hook(GB *gb);
void vire_state_moveOffScreen_hook(GB *gb);
void vire_mainForm_stateE_hook(GB *gb);
void vire_mainForm_stateF_hook(GB *gb);
void vire_mainForm_stateF_initBat_hook(GB *gb);
void vire_batForm_hook(GB *gb);
void vire_batForm_state8_hook(GB *gb);
void vire_batForm_state9_hook(GB *gb);
void vire_batForm_gotoStateA_hook(GB *gb);
void vire_batForm_stateA_hook(GB *gb);
void vire_batForm_animate_hook(GB *gb);
void vire_batForm_stateB_hook(GB *gb);
void vire_batForm_stateC_hook(GB *gb);
void vire_batForm_stateD_hook(GB *gb);
void vire_spawnOutsideCamera_hook(GB *gb);
void vire_mainForm_leftScreen_hook(GB *gb);
void vire_checkOffScreen_hook(GB *gb);
void vire_mainForm_circleAroundScreen_hook(GB *gb);
void vire_mainForm_applySpeedAndAnimate_hook(GB *gb);
void vire_mainForm_checkLinkTooClose_hook(GB *gb);
void vire_mainForm_fireProjectile_hook(GB *gb);
void vire_mainForm_fireProjectileWithSubid_hook(GB *gb);
void vire_batForm_moveAwayFromLinkIfTooClose_hook(GB *gb);
void vire_batForm_updateZPos_hook(GB *gb);

static uint16_t vireEnemy_jump_table(GB *gb) {
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

static void vireEnemy_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_VIRE
// ==================================================================================================
void enemyCode75_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x555c, 0x555e); goto normalStatus; } // jr z
  CYC(0x555c, 0x555e);
  CYC(0x555e, 0x5560); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5560); return; } // ret c
  CYC(0x5560, 0x5561);
  if (F & FZ) { CYCT(0x5561, 0x5563); goto dead; } // jr z
  CYC(0x5561, 0x5563);
  CYC(0x5563, 0x5565); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5565, 0x5566); A = mem_rd(gb, DE);
  CYC(0x5566, 0x5567); alu_or(gb, A);
  CYC(0x5567, 0x5569); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5569, 0x556a); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(0x556a, 0x556c); goto L_5570; } // jr z
  CYC(0x556a, 0x556c);
  CYC(0x556c, 0x556d); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x556d); return; } // ret z
  CYC(0x556d, 0x556e);
  CYCT(0x556e, 0x5570); goto normalStatus; // jr

L_5570:
  CYC(0x5570, 0x5571); H = D;
  CYC(0x5571, 0x5573); L = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x5573, 0x5574); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5574, 0x5576); goto normalStatus; } // jr z
  CYC(0x5574, 0x5576);
  CYC(0x5576, 0x5577); mem_wr(gb, HL, A);
  CYC(0x5577, 0x5578); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5578); return; } // ret z
  CYC(0x5578, 0x5579);
  CYC(0x5579, 0x557b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x557b, 0x557d); mem_wr(gb, HL, 0x0e);
  CYC(0x557d, 0x557e); L = alu_inc8(gb, L);
  CYC(0x557e, 0x5580); mem_wr(gb, HL, 0x00);
  RET(0x5580); return;

dead:
  CYC(0x5581, 0x5583); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5583, 0x5584); A = mem_rd(gb, DE);
  CYC(0x5584, 0x5585); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5585, 0x5587); goto subid0Dead; } // jr z
  CYC(0x5585, 0x5587);
  CALL_C(0x5587, objectCreatePuff_hook, 0x24c1, 0x558a);
  CYC(0x558a, 0x558c); A = ENEMY_BASE + 0x34; // Object.var34 offset
  CALL_C(0x558c, objectGetRelatedObject1Var_hook, 0x2160, 0x558f);
  CYC(0x558f, 0x5590); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CALL_C_CC(0x5590, objectCopyPosition_hook, 0x2242, 0x5593); } else { CYC(0x5590, 0x5593); } // call z
  CYC(0x5593, 0x5596); enemyDelete_hook(gb); return; // jp

subid0Dead:
  CYC(0x5596, 0x5597); H = D;
  CYC(0x5597, 0x5599); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5599, 0x559a); A = mem_rd(gb, HL);
  CYC(0x559a, 0x559c); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(0x559c, 0x559f); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(0x559c, 0x559f);
  CYC(0x559f, 0x55a1); mem_wr(gb, HL, 0x0f); // [state]
  CYC(0x55a1, 0x55a2); L = alu_inc8(gb, L);
  CYC(0x55a2, 0x55a4); mem_wr(gb, HL, 0x00); // [substate]
  CYC(0x55a4, 0x55a5); L = alu_inc8(gb, L);
  CYC(0x55a5, 0x55a7); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(0x55a7, 0x55a9); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x55a9, 0x55ab); mem_wr(gb, HL, 0x01);
  CYC(0x55ab, 0x55ad); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x55ad, 0x55ae); alu_xor(gb, A);
  CYC(0x55ae, 0x55af); mem_wr(gb, HL, A);
  CALL_C(0x55af, enemySetAnimation_hook, 0x282b, 0x55b2);

normalStatus:
  CALL_C(0x55b2, ecom_getSubidAndCpStateTo08_b0f_hook, 0x4426, 0x55b5);
  if (F & FC) { CYCT(0x55b5, 0x55b7); goto commonState; } // jr c
  CYC(0x55b5, 0x55b7);
  CYC(0x55b7, 0x55b8); A = B;
  CYC(0x55b8, 0x55b9); alu_or(gb, A);
  if (F & FZ) { CYCT(0x55b9, 0x55bc); vire_mainForm_hook(gb); return; } // jp z
  CYC(0x55b9, 0x55bc);
  CYC(0x55bc, 0x55bf); vire_batForm_hook(gb); return; // jp

commonState:
  CYC(0x55bf, 0x55c1); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x55c1, 0x55c2); A = mem_rd(gb, DE);
  CYC(0x55c2, 0x55c3); push_effect(gb, 0x55c3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x55d3) { vire_state_uninitialized_hook(gb); return; }
    if (target == 0x55e5) { vire_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

void vire_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55d3, 0x55d5); A = 0x1e; // SPEED_c0
  CALL_C(0x55d5, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x55d8);
  CYC(0x55d8, 0x55d9); A = B;
  CYC(0x55d9, 0x55da); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x55da); return; } // ret nz
  CYC(0x55da, 0x55db);
  CYC(0x55db, 0x55dd); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x55dd, 0x55df); mem_wr(gb, HL, 0xfc);
  CYC(0x55df, 0x55e0); A = alu_dec8(gb, A); // a = $ff
  CYC(0x55e0, 0x55e2); B = 0x00;
  CYC(0x55e2, 0x55e5); enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

void vire_state_stub_hook(GB *gb) {
  RET(0x55e5); return;
}

void vire_mainForm_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55e6, 0x55e8); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x55e8, 0x55e9); A = mem_rd(gb, DE);
  CYC(0x55e9, 0x55ea); alu_or(gb, A);
  if (F & FZ) { CYCT(0x55ea, 0x55ec); goto runState; } // jr z
  CYC(0x55ea, 0x55ec);
  CYC(0x55ec, 0x55ed); H = D;
  CYC(0x55ed, 0x55ef); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x55ef, 0x55f0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x55f0, 0x55f1); A = mem_rd(gb, HL);
  CYC(0x55f1, 0x55f3); alu_cp(gb, 0x18);
  if (F & FC) { CYCT(0x55f3, 0x55f5); goto runState; } // jr c
  CYC(0x55f3, 0x55f5);
  CYC(0x55f5, 0x55f6); alu_xor(gb, A);
  CYC(0x55f6, 0x55f7); mem_wr(gb, HL, A); // [var32] = 0
  CYC(0x55f7, 0x55f8); L = E;
  CYC(0x55f8, 0x55f9); mem_wr(gb, HL, A); // [direction] = 0
  CALL_C(0x55f9, enemySetAnimation_hook, 0x282b, 0x55fc);

runState:
  CYC(0x55fc, 0x55fe); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x55fe, 0x55ff); A = mem_rd(gb, DE);
  CYC(0x55ff, 0x5601); alu_sub(gb, 0x08);
  CYC(0x5601, 0x5602); push_effect(gb, 0x5602);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x5612) { vire_mainForm_state8_hook(gb); return; }
    if (target == 0x5693) { vire_mainForm_state9_hook(gb); return; }
    if (target == 0x56c7) { vire_mainForm_stateA_hook(gb); return; }
    if (target == 0x570b) { vire_mainForm_stateB_hook(gb); return; }
    if (target == 0x577f) { vire_mainForm_stateC_hook(gb); return; }
    if (target == 0x57c3) { vire_mainForm_stateD_hook(gb); return; }
    if (target == 0x5846) { vire_mainForm_stateE_hook(gb); return; }
    if (target == 0x5889) { vire_mainForm_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

// Mini-cutscene before starting fight
void vire_mainForm_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5612, 0x5613); E = alu_inc8(gb, E);
  CYC(0x5613, 0x5614); A = mem_rd(gb, DE); // [substate]
  CYC(0x5614, 0x5615); push_effect(gb, 0x5615);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x561d) goto substate0;
    if (target == 0x564b) goto substate1;
    if (target == 0x565d) goto substate2;
    if (target == 0x5671) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x561d, 0x561f); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x561f, 0x5621); alu_sub(gb, 0x38);
  CYC(0x5621, 0x5623); alu_cp(gb, 0x41);
  if (!(F & FC)) { RET_TAKEN(0x5623); return; } // ret nc
  CYC(0x5623, 0x5624);
  CYC(0x5624, 0x5626); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5626, 0x5628); alu_sub(gb, 0x50);
  CYC(0x5628, 0x562a); alu_cp(gb, 0x51);
  if (!(F & FC)) { RET_TAKEN(0x562a); return; } // ret nc
  CYC(0x562a, 0x562b);
  CYC(0x562b, 0x562e); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(0x562e, 0x562f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x562f); return; } // ret nz
  CYC(0x562f, 0x5630);
  CYC(0x5630, 0x5633); SET_BC(0x0502); // INTERAC_PUFF, subid 2
  CALL_C(0x5633, objectCreateInteraction_hook, 0x24c5, 0x5636);
  if (!(F & FZ)) { RET_TAKEN(0x5636); return; } // ret nz
  CYC(0x5636, 0x5637);
  CYC(0x5637, 0x5639); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x5639, 0x563a); A = H;
  CYC(0x563a, 0x563b); mem_wr(gb, DE, A);
  CYC(0x563b, 0x563c); E = alu_dec8(gb, E);
  CYC(0x563c, 0x563e); A = 0x40; // Interaction.start
  CYC(0x563e, 0x563f); mem_wr(gb, DE, A);
  CYC(0x563f, 0x5641); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x5641, 0x5643); A = 0x01;
  CYC(0x5643, 0x5644); mem_wr(gb, DE, A);
  CYC(0x5644, 0x5647); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5647, 0x564a); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  RET(0x564a); return;

substate1:
  CYC(0x564b, 0x564d); A = OBJ_ANIM_PARAMETER;
  CALL_C(0x564d, objectGetRelatedObject2Var_hook, 0x2164, 0x5650);
  CYC(0x5650, 0x5652); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5652); return; } // ret z
  CYC(0x5652, 0x5653);
  CYC(0x5653, 0x5654); H = D;
  CYC(0x5654, 0x5656); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x5656, 0x5657); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5657, 0x5658); L = alu_inc8(gb, L);
  CYC(0x5658, 0x565a); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(0x565a, 0x565d); objectSetVisiblec1_hook(gb); return; // jp

substate2:
  CALL_C(0x565d, ecom_decCounter1_b0f_hook, 0x439a, 0x5660);
  if (!(F & FZ)) { CYCT(0x5660, 0x5663); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x5660, 0x5663);
  CYC(0x5663, 0x5664); L = E;
  CYC(0x5664, 0x5665); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x5665, 0x5668); SET_BC(0x2f12); // TX_2f12
  CALL_C(0x5668, checkIsLinkedGame_hook, 0x1992, 0x566b);
  if (F & FZ) { CYCT(0x566b, 0x566d); goto L_566e; } // jr z
  CYC(0x566b, 0x566d);
  CYC(0x566d, 0x566e); C = alu_inc8(gb, C); // TX_2f13

L_566e:
  CYC(0x566e, 0x5671); showText_hook(gb); return; // jp

substate3:
  CALL_C(0x5671, objectCreatePuff_hook, 0x24c1, 0x5674);
  if (!(F & FZ)) { RET_TAKEN(0x5674); return; } // ret nz
  CYC(0x5674, 0x5675);
  CYC(0x5675, 0x5678); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5678, 0x567b); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(0x567b, ecom_incState_b0f_hook, 0x4000, 0x567e);
  CYC(0x567e, 0x567f); L = alu_inc8(gb, L);
  CYC(0x567f, 0x5680); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [substate]=0
  CYC(0x5680, 0x5682); mem_wr(gb, HL, 0x5a); // [counter1] = 90
  CYC(0x5682, 0x5684); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5684, 0x5685); A = mem_rd(gb, HL);
  CYC(0x5685, 0x5687); L = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x5687, 0x5688); mem_wr(gb, HL, A);
  CALL_C(0x5688, objectSetInvisible_hook, 0x1e7b, 0x568b);
  CYC(0x568b, 0x568d); A = 0x2d; // MUS_MINIBOSS
  CYC(0x568d, 0x5690); mem_wr(gb, wActiveMusic, A);
  CYC(0x5690, 0x5693); playSound_b00_hook(gb); return; // jp
}

// Off-screen for [counter1] frames
void vire_mainForm_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5693, ecom_decCounter1_b0f_hook, 0x439a, 0x5696);
  if (!(F & FZ)) { RET_TAKEN(0x5696); return; } // ret nz
  CYC(0x5696, 0x5697);
  CYC(0x5697, 0x5699); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5699, 0x569a); A = mem_rd(gb, DE);
  CYC(0x569a, 0x569c); C = 0x08;
  CYC(0x569c, 0x569e); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(0x569e, 0x56a0); goto L_56a8; } // jr c
  CYC(0x569e, 0x56a0);
  CYC(0x56a0, 0x56a2); C = 0x04;
  CYC(0x56a2, 0x56a4); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x56a4, 0x56a6); goto L_56a8; } // jr c
  CYC(0x56a4, 0x56a6);
  CYC(0x56a6, 0x56a8); C = 0x00;

L_56a8:
  CALL_C(0x56a8, getRandomNumber_hook, 0x043e, 0x56ab);
  CYC(0x56ab, 0x56ad); alu_and(gb, 0x07);
  CYC(0x56ad, 0x56ae); alu_add(gb, C);
  CYC(0x56ae, 0x56b1); SET_HL(0x56b7);
  CYC(0x56b1, 0x56b2); vireEnemy_addAToHl_from_rst(gb, 0x56b2);
  CYC(0x56b2, 0x56b3); A = mem_rd(gb, HL);
  CYC(0x56b3, 0x56b5); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x56b5, 0x56b6); mem_wr(gb, DE, A);
  RET(0x56b6); return;
}

// Charges across screen
void vire_mainForm_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56c7, 0x56c8); E = alu_inc8(gb, E);
  CYC(0x56c8, 0x56c9); A = mem_rd(gb, DE);
  CYC(0x56c9, 0x56ca); push_effect(gb, 0x56ca);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x56d0) goto substate0;
    if (target == 0x56db) goto substate1;
    if (target == 0x56f9) goto substate2;
    HANDOFF(target);
  }

substate0:
  CALL_C(0x56d0, vire_spawnOutsideCamera_hook, 0x5a45, 0x56d3);
  CYC(0x56d3, 0x56d4); L = alu_inc8(gb, L);
  CYC(0x56d4, 0x56d6); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(0x56d6, 0x56d8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x56d8, 0x56da); mem_wr(gb, HL, 0x28); // SPEED_100
  RET(0x56da); return;

substate1:
  CALL_C(0x56db, ecom_decCounter1_b0f_hook, 0x439a, 0x56de);
  if (!(F & FZ)) { CYCT(0x56de, 0x56e1); vire_mainForm_applySpeedAndAnimate_hook(gb); return; } // jp nz
  CYC(0x56de, 0x56e1);
  CYC(0x56e1, 0x56e2); L = E;
  CYC(0x56e2, 0x56e3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x56e3, 0x56e5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x56e5, 0x56e7); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x56e7, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x56ea);
  CALL_C(0x56ea, getRandomNumber_noPreserveVars_hook, 0x0453, 0x56ed);
  CYC(0x56ed, 0x56ef); alu_and(gb, 0x03);
  CYC(0x56ef, 0x56f1); alu_sub(gb, 0x02);
  CYC(0x56f1, 0x56f2); B = A;
  CYC(0x56f2, 0x56f4); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x56f4, 0x56f5); A = mem_rd(gb, DE);
  CYC(0x56f5, 0x56f6); alu_add(gb, B);
  CYC(0x56f6, 0x56f8); alu_and(gb, 0x1f);
  CYC(0x56f8, 0x56f9); mem_wr(gb, DE, A);

substate2:
  CALL_C(0x56f9, vire_checkOffScreen_hook, 0x5a98, 0x56fc);
  if (!(F & FC)) { CYCT(0x56fc, 0x56ff); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(0x56fc, 0x56ff);
  CALL_C(0x56ff, ecom_decCounter1_b0f_hook, 0x439a, 0x5702);
  CYC(0x5702, 0x5703); A = mem_rd(gb, HL);
  CYC(0x5703, 0x5705); alu_and(gb, 0x1f);
  if (F & FZ) { CALL_C_CC(0x5705, vire_mainForm_fireProjectile_hook, 0x5b14, 0x5708); } else { CYC(0x5705, 0x5708); } // call z
  CYC(0x5708, 0x570b); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Shared block: spawn outside camera, set counter1=120, pick var30 offset (used by state B's
// substate0 and, via the same ROM bytes, state D's substate0)
void vire_mainForm_stateB_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5716, vire_spawnOutsideCamera_hook, 0x5a45, 0x5719);
  CYC(0x5719, 0x571a); L = alu_inc8(gb, L);
  CYC(0x571a, 0x571c); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CALL_C(0x571c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x571f);
  CYC(0x571f, 0x5721); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x5721, 0x5723); goto L_5725; } // jr nz
  CYC(0x5721, 0x5723);
  CYC(0x5723, 0x5725); A = 0xf8;

L_5725:
  CYC(0x5725, 0x5727); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5727, 0x5728); mem_wr(gb, DE, A);
  RET(0x5728); return;
}

// Circling Link, runs away if Link gets too close (similar to state D)
void vire_mainForm_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x570b, 0x570c); E = alu_inc8(gb, E);
  CYC(0x570c, 0x570d); A = mem_rd(gb, DE);
  CYC(0x570d, 0x570e); push_effect(gb, 0x570e);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x5716) { vire_mainForm_stateB_substate0_hook(gb); return; }
    if (target == 0x5729) goto substate1;
    if (target == 0x574f) goto substate2;
    if (target == 0x576b) goto substate3;
    HANDOFF(target);
  }

substate1:
  CYC(0x5729, 0x572c); A = mem_rd(gb, wFrameCounter);
  CYC(0x572c, 0x572e); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x572e, 0x5730); goto L_573d; } // jr nz
  CYC(0x572e, 0x5730);
  CALL_C(0x5730, ecom_decCounter1_b0f_hook, 0x439a, 0x5733);
  if (F & FZ) { CYCT(0x5733, 0x5735); goto beginCharge; } // jr z
  CYC(0x5733, 0x5735);
  CYC(0x5735, 0x5736); A = mem_rd(gb, HL);
  CYC(0x5736, 0x5738); alu_and(gb, 0x1f);
  CYC(0x5738, 0x573a); B = 0x01;
  if (F & FZ) { CALL_C_CC(0x573a, vire_mainForm_fireProjectileWithSubid_hook, 0x5b1b, 0x573d); } else { CYC(0x573a, 0x573d); } // call z

L_573d:
  CALL_C(0x573d, vire_mainForm_checkLinkTooClose_hook, 0x5aff, 0x5740);
  if (!(F & FC)) { CYCT(0x5740, 0x5743); vire_mainForm_circleAroundScreen_hook(gb); return; } // jp nc
  CYC(0x5740, 0x5743);

beginCharge:
  CYC(0x5743, 0x5745); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x5745, 0x5746); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5746, 0x5748); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5748, 0x574a); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x574a, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x574d);
  CYCT(0x574d, 0x574f); goto animate; // jr

substate2:
  CALL_C(0x574f, vire_mainForm_checkLinkTooClose_hook, 0x5aff, 0x5752);
  if (F & FC) { CYCT(0x5752, 0x5754); goto updateAngleAway; } // jr c
  CYC(0x5752, 0x5754);
  CYC(0x5754, 0x5757); A = mem_rd(gb, wLinkUsingItem1);
  CYC(0x5757, 0x5758); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5758, 0x575a); goto updateAngleAway; } // jr nz
  CYC(0x5758, 0x575a);
  CALL_C(0x575a, vire_checkOffScreen_hook, 0x5a98, 0x575d);
  if (!(F & FC)) { CYCT(0x575d, 0x5760); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(0x575d, 0x5760);
  CYC(0x5760, 0x5763); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp

updateAngleAway:
  CYC(0x5763, 0x5764); L = E;
  CYC(0x5764, 0x5765); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CALL_C(0x5765, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, 0x43ab, 0x5768);

animate:
  CYC(0x5768, 0x576b); enemyAnimate_hook(gb); return; // jp

substate3:
  CALL_C(0x576b, vire_checkOffScreen_hook, 0x5a98, 0x576e);
  if (!(F & FC)) { CYCT(0x576e, 0x5771); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(0x576e, 0x5771);
  CALL_C(0x5771, ecom_decCounter1_b0f_hook, 0x439a, 0x5774);
  CYC(0x5774, 0x5775); A = mem_rd(gb, HL);
  CYC(0x5775, 0x5777); alu_and(gb, 0x1f);
  CYC(0x5777, 0x5779); B = 0x01;
  if (F & FZ) { CALL_C_CC(0x5779, vire_mainForm_fireProjectileWithSubid_hook, 0x5b1b, 0x577c); } else { CYC(0x5779, 0x577c); } // call z
  CYC(0x577c, 0x577f); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Vire creeps in from the screen edge to fire one projectile, then runs away
void vire_mainForm_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x577f, 0x5780); E = alu_inc8(gb, E);
  CYC(0x5780, 0x5781); A = mem_rd(gb, DE);
  CYC(0x5781, 0x5782); push_effect(gb, 0x5782);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x578a) goto substate0;
    if (target == 0x5795) goto substate1;
    if (target == 0x57a6) goto substate2;
    if (target == 0x57ba) goto substate3;
    HANDOFF(target);
  }

substate0:
  CALL_C(0x578a, vire_spawnOutsideCamera_hook, 0x5a45, 0x578d);
  CYC(0x578d, 0x578e); L = alu_inc8(gb, L);
  CYC(0x578e, 0x5790); mem_wr(gb, HL, 0x1c); // [counter1]
  CYC(0x5790, 0x5792); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5792, 0x5794); mem_wr(gb, HL, 0x28); // SPEED_100
  RET(0x5794); return;

substate1:
  CALL_C(0x5795, ecom_decCounter1_b0f_hook, 0x439a, 0x5798);
  if (!(F & FZ)) { CYCT(0x5798, 0x579b); vire_mainForm_applySpeedAndAnimate_hook(gb); return; } // jp nz
  CYC(0x5798, 0x579b);
  CYC(0x579b, 0x579d); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(0x579d, 0x579e); L = E;
  CYC(0x579e, 0x579f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x579f, 0x57a1); B = 0x03;
  CALL_C(0x57a1, vire_mainForm_fireProjectileWithSubid_hook, 0x5b1b, 0x57a4);
  CYCT(0x57a4, 0x57a6); goto animate; // jr

substate2:
  CALL_C(0x57a6, ecom_decCounter1_b0f_hook, 0x439a, 0x57a9);
  if (!(F & FZ)) { CYCT(0x57a9, 0x57ab); goto animate; } // jr nz
  CYC(0x57a9, 0x57ab);
  CYC(0x57ab, 0x57ac); L = E;
  CYC(0x57ac, 0x57ad); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x57ad, 0x57af); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x57af, 0x57b0); A = mem_rd(gb, HL);
  CYC(0x57b0, 0x57b2); alu_xor(gb, 0x10);
  CYC(0x57b2, 0x57b3); mem_wr(gb, HL, A);
  CYC(0x57b3, 0x57b5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x57b5, 0x57b7); mem_wr(gb, HL, 0x50); // SPEED_200

animate:
  CYC(0x57b7, 0x57ba); enemyAnimate_hook(gb); return; // jp

substate3:
  CALL_C(0x57ba, vire_checkOffScreen_hook, 0x5a98, 0x57bd);
  if (!(F & FC)) { CYCT(0x57bd, 0x57c0); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(0x57bd, 0x57c0);
  CYC(0x57c0, 0x57c3); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Circling Link, runs away if Link attempts to attack (similar to state B)
void vire_mainForm_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57c3, 0x57c4); E = alu_inc8(gb, E);
  CYC(0x57c4, 0x57c5); A = mem_rd(gb, DE);
  CYC(0x57c5, 0x57c6); push_effect(gb, 0x57c6);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x5716) { vire_mainForm_stateB_substate0_hook(gb); return; }
    if (target == 0x57d2) goto substate1;
    if (target == 0x57f9) goto substate2;
    if (target == 0x5817) goto substate3;
    if (target == 0x582d) goto substate4;
    if (target == 0x583d) { vire_state_moveOffScreen_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  CYC(0x57d2, 0x57d5); A = mem_rd(gb, wFrameCounter);
  CYC(0x57d5, 0x57d7); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x57d7, 0x57d9); goto L_57e6; } // jr nz
  CYC(0x57d7, 0x57d9);
  CALL_C(0x57d9, ecom_decCounter1_b0f_hook, 0x439a, 0x57dc);
  if (F & FZ) { CYCT(0x57dc, 0x57de); goto beginCharge; } // jr z
  CYC(0x57dc, 0x57de);
  CYC(0x57de, 0x57df); A = mem_rd(gb, HL);
  CYC(0x57df, 0x57e1); alu_and(gb, 0x1f);
  CYC(0x57e1, 0x57e3); B = 0x01;
  if (F & FZ) { CALL_C_CC(0x57e3, vire_mainForm_fireProjectileWithSubid_hook, 0x5b1b, 0x57e6); } else { CYC(0x57e3, 0x57e6); } // call z

L_57e6:
  CALL_C(0x57e6, vire_mainForm_checkLinkTooClose_hook, 0x5aff, 0x57e9);
  if (!(F & FC)) { CYCT(0x57e9, 0x57ec); vire_mainForm_circleAroundScreen_hook(gb); return; } // jp nc
  CYC(0x57e9, 0x57ec);

beginCharge:
  CYC(0x57ec, 0x57ee); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x57ee, 0x57ef); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x57ef, 0x57f1); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x57f1, 0x57f3); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x57f3, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x57f6);

animate:
  CYC(0x57f6, 0x57f9); enemyAnimate_hook(gb); return; // jp

substate2:
  CYC(0x57f9, 0x57fc); A = mem_rd(gb, wLinkUsingItem1);
  CYC(0x57fc, 0x57fd); alu_or(gb, A);
  if (F & FZ) { CYCT(0x57fd, 0x57ff); goto moveOffScreen; } // jr z
  CYC(0x57fd, 0x57ff);
  CYC(0x57ff, 0x5800); H = D;
  CYC(0x5800, 0x5801); L = E;
  CYC(0x5801, 0x5802); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x5802, 0x5803); L = alu_inc8(gb, L);
  CYC(0x5803, 0x5805); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(0x5805, 0x5807); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5807, 0x5809); mem_wr(gb, HL, 0x78); // SPEED_300
  CALL_C(0x5809, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, 0x43ab, 0x580c);
  CYCT(0x580c, 0x580e); goto animate; // jr

moveOffScreen:
  CALL_C(0x580e, vire_checkOffScreen_hook, 0x5a98, 0x5811);
  if (!(F & FC)) { CYCT(0x5811, 0x5814); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(0x5811, 0x5814);
  CYC(0x5814, 0x5817); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp

substate3:
  CALL_C(0x5817, ecom_decCounter1_b0f_hook, 0x439a, 0x581a);
  if (!(F & FZ)) { CYCT(0x581a, 0x581d); vire_mainForm_applySpeedAndAnimate_hook(gb); return; } // jp nz
  CYC(0x581a, 0x581d);
  CYC(0x581d, 0x581f); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(0x581f, 0x5820); L = E;
  CYC(0x5820, 0x5821); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x5821, 0x5823); B = 0x3a; // PART_VIRE_PROJECTILE
  CALL_C(0x5823, ecom_spawnProjectile_b0f_hook, 0x437c, 0x5826);
  CYC(0x5826, 0x5828); A = 0x87; // SND_SPLASH
  CALL_C(0x5828, playSound_b00_hook, 0x0c98, 0x582b);
  CYCT(0x582b, 0x582d); goto animate; // jr

substate4:
  CALL_C(0x582d, ecom_decCounter1_b0f_hook, 0x439a, 0x5830);
  if (!(F & FZ)) { CYCT(0x5830, 0x5832); goto animate; } // jr nz
  CYC(0x5830, 0x5832);
  CYC(0x5832, 0x5833); L = E;
  CYC(0x5833, 0x5834); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x5834, 0x5836); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5836, 0x5838); mem_wr(gb, HL, 0x46); // SPEED_1c0
  CALL_C(0x5838, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, 0x43ab, 0x583b);
  CYCT(0x583b, 0x583d); goto animate; // jr
}

// Used by states D and E
void vire_state_moveOffScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x583d, vire_checkOffScreen_hook, 0x5a98, 0x5840);
  if (!(F & FC)) { CYCT(0x5840, 0x5843); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(0x5840, 0x5843);
  CYC(0x5843, 0x5846); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Just took damage
void vire_mainForm_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5846, 0x5847); E = alu_inc8(gb, E);
  CYC(0x5847, 0x5848); A = mem_rd(gb, DE);
  CYC(0x5848, 0x5849); push_effect(gb, 0x5849);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x584f) goto substate0;
    if (target == 0x5866) goto substate1;
    if (target == 0x583d) { vire_state_moveOffScreen_hook(gb); return; }
    HANDOFF(target);
  }

substate0:
  CYC(0x584f, 0x5850); H = D;
  CYC(0x5850, 0x5851); L = E;
  CYC(0x5851, 0x5852); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x5852, 0x5853); L = alu_inc8(gb, L);
  CYC(0x5853, 0x5855); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(0x5855, 0x5857); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5857, 0x5859); mem_wr(gb, HL, 0x78); // SPEED_300
  CALL_C(0x5859, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, 0x43ab, 0x585c);
  CYC(0x585c, 0x585e); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x585e, 0x585f); alu_xor(gb, A);
  CYC(0x585f, 0x5860); mem_wr(gb, DE, A);
  CYC(0x5860, 0x5862); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x5862, 0x5863); mem_wr(gb, DE, A);
  CYC(0x5863, 0x5866); enemySetAnimation_hook(gb); return; // jp

substate1:
  CALL_C(0x5866, ecom_decCounter1_b0f_hook, 0x439a, 0x5869);
  if (!(F & FZ)) { CYCT(0x5869, 0x586c); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x5869, 0x586c);
  CYC(0x586c, 0x586d); L = E;
  CYC(0x586d, 0x586e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x586e, 0x5870); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5870, 0x5871); A = mem_rd(gb, HL);
  CYC(0x5871, 0x5873); alu_cp(gb, 0x10);
  if (!(F & FC)) { RET_TAKEN(0x5873); return; } // ret nc
  CYC(0x5873, 0x5874);
  CYC(0x5874, 0x5876); B = 0x01;
  CYC(0x5876, 0x5878); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(0x5878, 0x587a); goto L_587b; } // jr nc
  CYC(0x5878, 0x587a);
  CYC(0x587a, 0x587b); B = alu_inc8(gb, B);

L_587b:
  CYC(0x587b, 0x587c); A = B;
  CYC(0x587c, 0x587e); L = ENEMY_BASE + 0x37; // Enemy.var37
  CYC(0x587e, 0x587f); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x587f); return; } // ret z
  CYC(0x587f, 0x5880);
  CYC(0x5880, 0x5881); mem_wr(gb, HL, A);
  CYC(0x5881, 0x5883); alu_add(gb, 0x13);
  CYC(0x5883, 0x5884); C = A;
  CYC(0x5884, 0x5886); B = 0x2f; // TX_2f00 hi
  CYC(0x5886, 0x5889); showText_hook(gb); return; // jp
}

// "Main form" died, about to split into bats
void vire_mainForm_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5889, 0x588a); E = alu_inc8(gb, E);
  CYC(0x588a, 0x588b); A = mem_rd(gb, DE);
  CYC(0x588b, 0x588c); push_effect(gb, 0x588c);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x5898) goto substate0;
    if (target == 0x58a4) goto substate1;
    if (target == 0x58db) goto substate2;
    if (target == 0x58ee) goto substate3;
    if (target == 0x58fb) goto substate4;
    if (target == 0x5917) goto substate5;
    HANDOFF(target);
  }

substate0:
  CALL_C(0x5898, ecom_decCounter1_b0f_hook, 0x439a, 0x589b);
  if (!(F & FZ)) { RET_TAKEN(0x589b); return; } // ret nz
  CYC(0x589b, 0x589c);
  CYC(0x589c, 0x589d); L = E;
  CYC(0x589d, 0x589e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x589e, 0x58a1); SET_BC(0x2f16); // TX_2f16
  CYC(0x58a1, 0x58a4); showText_hook(gb); return; // jp

substate1:
  CYC(0x58a4, 0x58a6); B = 0x02;
  CALL_C(0x58a6, checkBEnemySlotsAvailable_hook, 0x20f7, 0x58a9);
  if (!(F & FZ)) { CYCT(0x58a9, 0x58ac); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x58a9, 0x58ac);
  CYC(0x58ac, 0x58ad); H = D;
  CYC(0x58ad, 0x58af); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x58af, 0x58b0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x58b0, 0x58b2); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x58b2, 0x58b4); mem_wr(gb, HL, 0x02);
  CALL_C(0x58b4, objectSetInvisible_hook, 0x1e7b, 0x58b7);
  CALL_C(0x58b7, objectCreatePuff_hook, 0x24c1, 0x58ba);
  CYC(0x58ba, 0x58bc); B = 0x75; // ENEMY_VIRE
  CALL_C(0x58bc, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x58bf);
  CALL_C(0x58bf, vire_mainForm_stateF_initBat_hook, 0x58c6, 0x58c2);
  CALL_C(0x58c2, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x58c5);
  CYC(0x58c5, 0x58c6); A = alu_inc8(gb, A);
  vire_mainForm_stateF_initBat_hook(gb); return; // fallthrough

substate2:
  CYC(0x58db, 0x58dd); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x58dd, 0x58de); A = mem_rd(gb, DE);
  CYC(0x58de, 0x58df); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x58df, 0x58e2); ecom_decCounter2_b0f_hook(gb); return; } // jp nz
  CYC(0x58df, 0x58e2);
  CYC(0x58e2, 0x58e3); H = D;
  CYC(0x58e3, 0x58e5); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x58e5, 0x58e6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x58e6, 0x58e7); L = alu_inc8(gb, L);
  CYC(0x58e7, 0x58e9); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(0x58e9, 0x58eb); A = 0xf0; // SNDCTRL_STOPMUSIC
  CYC(0x58eb, 0x58ee); playSound_b00_hook(gb); return; // jp

substate3:
  CALL_C(0x58ee, ecom_decCounter1_b0f_hook, 0x439a, 0x58f1);
  if (!(F & FZ)) { CYCT(0x58f1, 0x58f4); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(0x58f1, 0x58f4);
  CYC(0x58f4, 0x58f6); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(0x58f6, 0x58f7); L = E;
  CYC(0x58f7, 0x58f8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x58f8, 0x58fb); objectSetVisiblec1_hook(gb); return; // jp

substate4:
  CALL_C(0x58fb, ecom_decCounter1_b0f_hook, 0x439a, 0x58fe);
  if (!(F & FZ)) { CYCT(0x58fe, 0x5901); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x58fe, 0x5901);
  CYC(0x5901, 0x5902); L = E;
  CYC(0x5902, 0x5903); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x5903, 0x5905); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5905, 0x5907); mem_wr(gb, HL, 0x06);
  CYC(0x5907, 0x5909); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5909, 0x590b); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(0x590b, 0x590e); SET_BC(0x2f17); // TX_2f17
  CALL_C(0x590e, checkIsLinkedGame_hook, 0x1992, 0x5911);
  if (F & FZ) { CYCT(0x5911, 0x5913); goto L_5914; } // jr z
  CYC(0x5911, 0x5913);
  CYC(0x5913, 0x5914); C = alu_inc8(gb, C); // TX_2f18

L_5914:
  CYC(0x5914, 0x5917); showText_hook(gb); return; // jp

substate5:
  CALL_C(0x5917, checkIsLinkedGame_hook, 0x1992, 0x591a);
  if (F & FZ) { CYCT(0x591a, 0x591c); goto unlinked; } // jr z
  CYC(0x591a, 0x591c);

linked:
  CYC(0x591c, 0x591e); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x591e, 0x591f); alu_xor(gb, A);
  CYC(0x591f, 0x5920); mem_wr(gb, DE, A);
  RET(0x5920); return;

unlinked:
  CYC(0x5921, 0x5923); E = ENEMY_BASE + 0x38; // Enemy.var38
  CYC(0x5923, 0x5924); A = mem_rd(gb, DE);
  CYC(0x5924, 0x5925); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5925, 0x5927); goto L_592e; } // jr nz
  CYC(0x5925, 0x5927);
  CYC(0x5927, 0x5928); A = alu_inc8(gb, A);
  CYC(0x5928, 0x5929); mem_wr(gb, DE, A);
  CYC(0x5929, 0x592b); B = 0x01; // PART_ITEM_DROP
  CALL_C(0x592b, ecom_spawnProjectile_b0f_hook, 0x437c, 0x592e);

L_592e:
  CALL_C(0x592e, enemyAnimate_hook, 0x2818, 0x5931);
  CYC(0x5931, 0x5932); H = D;
  CYC(0x5932, 0x5934); L = ENEMY_BASE + OBJ_Z;
  CYC(0x5934, 0x5935); A = mem_rd(gb, HL);
  CYC(0x5935, 0x5937); alu_sub(gb, 0x80);
  CYC(0x5937, 0x5938); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5938, 0x5939); A = mem_rd(gb, HL);
  CYC(0x5939, 0x593b); alu_sbc(gb, 0x00);
  CYC(0x593b, 0x593c); mem_wr(gb, HL, A);
  CALL_C(0x593c, vire_checkOffScreen_hook, 0x5a98, 0x593f);
  if (F & FC) { CYCT(0x593f, 0x5942); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x593f, 0x5942);
  CALL_C(0x5942, markEnemyAsKilledInRoom_b02_hook, 0x320d, 0x5945);
  CALL_C(0x5945, decNumEnemies_hook, 0x24b3, 0x5948);
  CYC(0x5948, 0x594b); A = mem_rd(gb, wActiveMusic2);
  CYC(0x594b, 0x594e); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x594e, playSound_b00_hook, 0x0c98, 0x5951);
  CYC(0x5951, 0x5954); enemyDelete_hook(gb); return; // jp
}

void vire_mainForm_stateF_initBat_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x58c6, 0x58c7); L = alu_inc8(gb, L);
  CYC(0x58c7, 0x58c8); mem_wr(gb, HL, A); // [var03] = a
  CYC(0x58c8, 0x58c9); alu_rrca(gb);
  CYC(0x58c9, 0x58cb); A = alu_swap(gb, A);
  if (!(F & FZ)) { CYCT(0x58cb, 0x58cd); goto L_58cf; } // jr nz
  CYC(0x58cb, 0x58cd);
  CYC(0x58cd, 0x58cf); A = 0xf8;

L_58cf:
  CYC(0x58cf, 0x58d1); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x58d1, 0x58d2); mem_wr(gb, HL, A);
  CYC(0x58d2, 0x58d4); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x58d4, 0x58d6); A = ENEMY_BASE; // Enemy.start
  CYC(0x58d6, 0x58d7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x58d7, 0x58d8); mem_wr(gb, HL, D);
  CYC(0x58d8, 0x58db); objectCopyPosition_hook(gb); return; // jp
}

void vire_batForm_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5954, 0x5955); A = mem_rd(gb, DE);
  CYC(0x5955, 0x5957); alu_sub(gb, 0x08);
  CYC(0x5957, 0x5958); push_effect(gb, 0x5958);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == 0x5964) { vire_batForm_state8_hook(gb); return; }
    if (target == 0x597f) { vire_batForm_state9_hook(gb); return; }
    if (target == 0x59b2) { vire_batForm_stateA_hook(gb); return; }
    if (target == 0x59e1) { vire_batForm_stateB_hook(gb); return; }
    if (target == 0x59f4) { vire_batForm_stateC_hook(gb); return; }
    if (target == 0x5a26) { vire_batForm_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

void vire_batForm_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5964, 0x5965); H = D;
  CYC(0x5965, 0x5966); L = E;
  CYC(0x5966, 0x5967); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5967, 0x5969); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5969, 0x596a); A = mem_rd(gb, HL);
  CYC(0x596a, 0x596c); alu_and(gb, 0x1f);
  CYC(0x596c, 0x596e); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x596e, 0x596f); mem_wr(gb, HL, A);
  CYC(0x596f, 0x5971); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5971, 0x5973); mem_wr(gb, HL, 0x10);
  CYC(0x5973, 0x5975); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5975, 0x5977); mem_wr(gb, HL, 0x01);
  CYC(0x5977, 0x5979); A = 0x02;
  CALL_C(0x5979, enemySetAnimation_hook, 0x282b, 0x597c);
  CYC(0x597c, 0x597f); objectSetVisiblec1_hook(gb); return; // jp
}

// Moving upward after charging (or after spawning)
void vire_batForm_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x597f, ecom_decCounter1_b0f_hook, 0x439a, 0x5982);
  if (F & FZ) { CYCT(0x5982, 0x5984); vire_batForm_gotoStateA_hook(gb); return; } // jr z
  CYC(0x5982, 0x5984);
  CYC(0x5984, 0x5986); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5986, 0x5987); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5987, 0x5989); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(0x5989, 0x598b); goto L_5993; } // jr c
  CYC(0x5989, 0x598b);
  CYC(0x598b, 0x598c); A = mem_rd(gb, HL);
  CYC(0x598c, 0x598e); alu_sub(gb, 0xc0);
  CYC(0x598e, 0x598f); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x598f, 0x5990); A = mem_rd(gb, HL);
  CYC(0x5990, 0x5992); alu_sbc(gb, 0x00);
  CYC(0x5992, 0x5993); mem_wr(gb, HL, A);

L_5993:
  CALL_C(0x5993, objectApplySpeed_hook, 0x201d, 0x5996);
  CYCT(0x5996, 0x5998); vire_batForm_animate_hook(gb); return; // jr
}

void vire_batForm_gotoStateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5998, 0x5999); L = E;
  CYC(0x5999, 0x599b); mem_wr(gb, HL, 0x0a); // [state]
  CYC(0x599b, 0x599d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x599d, 0x599f); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x599f, 0x59a1); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x59a1, 0x59a3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(0x59a3, getRandomNumber_noPreserveVars_hook, 0x0453, 0x59a6);
  CYC(0x59a6, 0x59a8); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x59a8, 0x59a9); mem_wr(gb, DE, A);
  CYC(0x59a9, 0x59ab); A = OBJ_COUNTER2;
  CALL_C(0x59ab, objectGetRelatedObject1Var_hook, 0x2160, 0x59ae);
  CYC(0x59ae, 0x59b0); mem_wr(gb, HL, 0xb4); // [mainForm.counter2] = 180
  CYCT(0x59b0, 0x59b2); vire_batForm_animate_hook(gb); return; // jr
}

void vire_batForm_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59b2, vire_batForm_updateZPos_hook, 0x5b59, 0x59b5);
  CYC(0x59b5, 0x59b7); A = OBJ_COUNTER2;
  CALL_C(0x59b7, objectGetRelatedObject1Var_hook, 0x2160, 0x59ba);
  CYC(0x59ba, 0x59bb); A = mem_rd(gb, HL);
  CYC(0x59bb, 0x59bc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x59bc, 0x59be); goto L_59c6; } // jr nz
  CYC(0x59bc, 0x59be);
  CALL_C(0x59be, ecom_incState_b0f_hook, 0x4000, 0x59c1);
  CYC(0x59c1, 0x59c3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x59c3, 0x59c5); mem_wr(gb, HL, 0x08);
  RET(0x59c5); return;

L_59c6:
  CALL_C(0x59c6, vire_batForm_moveAwayFromLinkIfTooClose_hook, 0x5b39, 0x59c9);
  CALL_C(0x59c9, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x59cc);
  CYC(0x59cc, 0x59cd); B = A;
  CYC(0x59cd, 0x59cf); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x59cf, 0x59d0); A = mem_rd(gb, DE);
  CYC(0x59d0, 0x59d1); alu_add(gb, B);
  CYC(0x59d1, 0x59d3); alu_and(gb, 0x1f);
  CYC(0x59d3, 0x59d5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x59d5, 0x59d6); mem_wr(gb, DE, A);
  CYC(0x59d6, 0x59d8); A = 0x02;
  CALL_C(0x59d8, ecom_getSideviewAdjacentWallsBitset_b0f_hook, 0x420b, 0x59db);
  if (F & FZ) { CALL_C_CC(0x59db, objectApplySpeed_hook, 0x201d, 0x59de); } else { CYC(0x59db, 0x59de); } // call z
  vire_batForm_animate_hook(gb); return; // fallthrough
}

void vire_batForm_animate_hook(GB *gb) {
  CYC(0x59de, 0x59e1); enemyAnimate_hook(gb); return; // jp
}

// About to charge toward Link in [counter1] frames
void vire_batForm_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59e1, ecom_decCounter1_b0f_hook, 0x439a, 0x59e4);
  if (!(F & FZ)) { RET_TAKEN(0x59e4); return; } // ret nz
  CYC(0x59e4, 0x59e5);
  CYC(0x59e5, 0x59e6); L = E;
  CYC(0x59e6, 0x59e7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x59e7, 0x59e9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x59e9, 0x59eb); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x59eb, 0x59ed); L = ENEMY_BASE + 0x35; // Enemy.var35
  CYC(0x59ed, 0x59ef); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x59ef, 0x59f0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x59f0, 0x59f2); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x59f2, 0x59f3); mem_wr(gb, HL, A);
  RET(0x59f3); return;
}

// Charging toward target position in var35/var36
void vire_batForm_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59f4, 0x59f5); H = D;
  CYC(0x59f5, 0x59f7); L = ENEMY_BASE + 0x35; // Enemy.var35
  CALL_C(0x59f7, ecom_readPositionVars_b0f_hook, 0x4439, 0x59fa);
  CYC(0x59fa, 0x59fb); alu_sub(gb, C);
  CYC(0x59fb, 0x59fd); alu_add(gb, 0x08);
  CYC(0x59fd, 0x59ff); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(0x59ff, 0x5a01); goto notReachedPosition; } // jr nc
  CYC(0x59ff, 0x5a01);
  CYC(0x5a01, 0x5a03); A = hram_rd(gb, 0x8f);
  CYC(0x5a03, 0x5a04); alu_sub(gb, B);
  CYC(0x5a04, 0x5a06); alu_add(gb, 0x08);
  CYC(0x5a06, 0x5a08); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(0x5a08, 0x5a0a); goto notReachedPosition; } // jr nc
  CYC(0x5a08, 0x5a0a);
  CYC(0x5a0a, 0x5a0c); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5a0c, 0x5a0d); A = mem_rd(gb, HL);
  CYC(0x5a0d, 0x5a0f); alu_cp(gb, 0xfa);
  if (F & FC) { CYCT(0x5a0f, 0x5a11); goto notReachedPosition; } // jr c
  CYC(0x5a0f, 0x5a11);
  CYC(0x5a11, 0x5a12); L = E;
  CYC(0x5a12, 0x5a13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5a13, 0x5a15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5a15, 0x5a17); mem_wr(gb, HL, 0x14);
  CYCT(0x5a17, 0x5a19); vire_batForm_animate_hook(gb); return; // jr

notReachedPosition:
  CYC(0x5a19, 0x5a1b); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5a1b, 0x5a1c); A = mem_rd(gb, HL);
  CYC(0x5a1c, 0x5a1e); alu_cp(gb, 0xfe);
  if (!(F & FC)) { CYCT(0x5a1e, 0x5a20); goto L_5a21; } // jr nc
  CYC(0x5a1e, 0x5a20);
  CYC(0x5a20, 0x5a21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

L_5a21:
  CALL_C(0x5a21, ecom_moveTowardPosition_b0f_hook, 0x4430, 0x5a24);
  CYCT(0x5a24, 0x5a26); vire_batForm_animate_hook(gb); return; // jr
}

// Moving back up after charging
void vire_batForm_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a26, ecom_decCounter1_b0f_hook, 0x439a, 0x5a29);
  if (F & FZ) { CYCT(0x5a29, 0x5a2c); vire_batForm_gotoStateA_hook(gb); return; } // jp z
  CYC(0x5a29, 0x5a2c);
  CYC(0x5a2c, 0x5a2e); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5a2e, 0x5a2f); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5a2f, 0x5a31); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(0x5a31, 0x5a33); goto L_5a3b; } // jr c
  CYC(0x5a31, 0x5a33);
  CYC(0x5a33, 0x5a34); A = mem_rd(gb, HL);
  CYC(0x5a34, 0x5a36); alu_sub(gb, 0xc0);
  CYC(0x5a36, 0x5a37); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5a37, 0x5a38); A = mem_rd(gb, HL);
  CYC(0x5a38, 0x5a3a); alu_sbc(gb, 0x00);
  CYC(0x5a3a, 0x5a3b); mem_wr(gb, HL, A);

L_5a3b:
  CYC(0x5a3b, 0x5a3d); A = 0x02;
  CALL_C(0x5a3d, ecom_getSideviewAdjacentWallsBitset_b0f_hook, 0x420b, 0x5a40);
  if (F & FZ) { CALL_C_CC(0x5a40, objectApplySpeed_hook, 0x201d, 0x5a43); } else { CYC(0x5a40, 0x5a43); } // call z
  CYCT(0x5a43, 0x5a45); vire_batForm_animate_hook(gb); return; // jr
}

// Sets Vire's position to just outside the camera, increments substate.
void vire_spawnOutsideCamera_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a45, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5a48);
  CYC(0x5a48, 0x5a4a); alu_and(gb, 0x07);
  CYC(0x5a4a, 0x5a4b); B = A;
  CYC(0x5a4b, 0x5a4c); alu_add(gb, A);
  CYC(0x5a4c, 0x5a4d); alu_add(gb, B);
  CYC(0x5a4d, 0x5a50); SET_HL(0x5a6e);
  CYC(0x5a50, 0x5a51); vireEnemy_addAToHl_from_rst(gb, 0x5a51);
  CYC(0x5a51, 0x5a53); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5a53, 0x5a55); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(0x5a55, 0x5a56); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5a56, 0x5a57); mem_wr(gb, DE, A);
  CYC(0x5a57, 0x5a58); SET_HL(HL + 1);
  CYC(0x5a58, 0x5a5a); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5a5a, 0x5a5c); A = hram_rd(gb, 0xac); // hCameraX
  CYC(0x5a5c, 0x5a5d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5a5d, 0x5a5e); mem_wr(gb, DE, A);
  CYC(0x5a5e, 0x5a5f); SET_HL(HL + 1);
  CYC(0x5a5f, 0x5a61); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5a61, 0x5a62); A = mem_rd(gb, HL);
  CYC(0x5a62, 0x5a63); mem_wr(gb, DE, A);
  CYC(0x5a63, 0x5a64); H = D;
  CYC(0x5a64, 0x5a66); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5a66, 0x5a68); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5a68, 0x5a6a); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x5a6a, 0x5a6b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5a6b, 0x5a6e); objectSetVisiblec1_hook(gb); return; // jp
}

// Vire has left the screen; set state to 9, where he'll wait for 90 frames before attacking again.
void vire_mainForm_leftScreen_hook(GB *gb) {
  CYC(0x5a86, 0x5a87); H = D;
  CYC(0x5a87, 0x5a89); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5a89, 0x5a8b); mem_wr(gb, HL, 0x09);
  CYC(0x5a8b, 0x5a8c); L = alu_inc8(gb, L);
  CYC(0x5a8c, 0x5a8e); mem_wr(gb, HL, 0x00); // [substate]
  CYC(0x5a8e, 0x5a8f); L = alu_inc8(gb, L);
  CYC(0x5a8f, 0x5a91); mem_wr(gb, HL, 0x5a); // [counter1] = 90
  CYC(0x5a91, 0x5a93); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5a93, 0x5a95); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x5a95, 0x5a98); objectSetInvisible_hook(gb); return; // jp
}

// @param[out] cflag c if left screen
void vire_checkOffScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5a98, 0x5a9a); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5a9a, 0x5a9b); A = mem_rd(gb, DE);
  CYC(0x5a9b, 0x5a9d); alu_cp(gb, 0xb8);
  if (!(F & FC)) { RET_TAKEN(0x5a9d); return; } // ret nc
  CYC(0x5a9d, 0x5a9e);
  CYC(0x5a9e, 0x5aa0); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5aa0, 0x5aa1); A = mem_rd(gb, DE);
  CYC(0x5aa1, 0x5aa3); alu_cp(gb, 0xf0);
  RET(0x5aa3); return;
}

void vire_mainForm_circleAroundScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5aa4, 0x5aa6); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(0x5aa6, 0x5aa8); alu_add(gb, 0x44);
  CYC(0x5aa8, 0x5aa9); B = A;
  CYC(0x5aa9, 0x5aab); A = hram_rd(gb, 0xac); // hCameraX
  CYC(0x5aab, 0x5aad); alu_add(gb, 0x50);
  CYC(0x5aad, 0x5aae); C = A;
  CYC(0x5aae, 0x5aaf); push_effect(gb, BC);
  CALL_C(0x5aaf, objectGetRelativeAngle_hook, 0x1ea4, 0x5ab2);
  CYC(0x5ab2, 0x5ab3); SET_BC(pop_effect(gb));
  CYC(0x5ab3, 0x5ab4); H = A;
  CYC(0x5ab4, 0x5ab6); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5ab6, 0x5ab7); A = mem_rd(gb, DE);
  CYC(0x5ab7, 0x5ab8); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(0x5ab8, 0x5aba); goto L_5abc; } // jr nc
  CYC(0x5ab8, 0x5aba);
  CYC(0x5aba, 0x5abb); alu_cpl(gb);
  CYC(0x5abb, 0x5abc); A = alu_inc8(gb, A);

L_5abc:
  CYC(0x5abc, 0x5abd); B = A;
  CYC(0x5abd, 0x5abf); alu_cp(gb, 0x3e);
  CYC(0x5abf, 0x5ac0); A = H;
  if (!(F & FC)) { CYCT(0x5ac0, 0x5ac2); goto setAngleAndSpeed; } // jr nc
  CYC(0x5ac0, 0x5ac2);
  CYC(0x5ac2, 0x5ac4); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5ac4, 0x5ac5); A = mem_rd(gb, DE);
  CYC(0x5ac5, 0x5ac6); alu_sub(gb, C);
  if (!(F & FC)) { CYCT(0x5ac6, 0x5ac8); goto L_5aca; } // jr nc
  CYC(0x5ac6, 0x5ac8);
  CYC(0x5ac8, 0x5ac9); alu_cpl(gb);
  CYC(0x5ac9, 0x5aca); A = alu_inc8(gb, A);

L_5aca:
  CYC(0x5aca, 0x5acb); C = A;
  CYC(0x5acb, 0x5acd); alu_cp(gb, 0x3e);
  CYC(0x5acd, 0x5ace); A = H;
  if (!(F & FC)) { CYCT(0x5ace, 0x5ad0); goto setAngleAndSpeed; } // jr nc
  CYC(0x5ace, 0x5ad0);
  CYC(0x5ad0, 0x5ad1); A = B;
  CYC(0x5ad1, 0x5ad2); alu_add(gb, C);
  CYC(0x5ad2, 0x5ad4); alu_sub(gb, 0x42);
  CYC(0x5ad4, 0x5ad6); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(0x5ad6, 0x5ad8); goto offsetAngle; } // jr c
  CYC(0x5ad6, 0x5ad8);
  CYC(0x5ad8, 0x5ad9); alu_rlca(gb);
  CYC(0x5ad9, 0x5ada); A = H;
  if (!(F & FC)) { CYCT(0x5ada, 0x5adc); goto setAngleAndSpeed; } // jr nc
  CYC(0x5ada, 0x5adc);
  CYC(0x5adc, 0x5ade); alu_xor(gb, 0x10);

setAngleAndSpeed:
  CYC(0x5ade, 0x5adf); push_effect(gb, HL);
  CYC(0x5adf, 0x5ae1); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5ae1, 0x5ae2); mem_wr(gb, DE, A);
  CYC(0x5ae2, 0x5ae4); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5ae4, 0x5ae6); A = 0x0a; // SPEED_40
  CYC(0x5ae6, 0x5ae7); mem_wr(gb, DE, A);
  CALL_C(0x5ae7, objectApplySpeed_hook, 0x201d, 0x5aea);
  CYC(0x5aea, 0x5aeb); SET_HL(pop_effect(gb));

offsetAngle:
  CYC(0x5aeb, 0x5aed); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5aed, 0x5aee); A = mem_rd(gb, DE);
  CYC(0x5aee, 0x5aef); alu_add(gb, H);
  CYC(0x5aef, 0x5af1); alu_and(gb, 0x1f);
  CYC(0x5af1, 0x5af3); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5af3, 0x5af4); mem_wr(gb, DE, A);
  CYC(0x5af4, 0x5af6); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5af6, 0x5af8); A = 0x23; // SPEED_e0
  CYC(0x5af8, 0x5af9); mem_wr(gb, DE, A);
  vire_mainForm_applySpeedAndAnimate_hook(gb); return; // fallthrough
}

void vire_mainForm_applySpeedAndAnimate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5af9, objectApplySpeed_hook, 0x201d, 0x5afc);
  CYC(0x5afc, 0x5aff); enemyAnimate_hook(gb); return; // jp
}

// @param[out] cflag c if Link is too close (Vire will flee)
void vire_mainForm_checkLinkTooClose_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5aff, 0x5b00); H = D;
  CYC(0x5b00, 0x5b02); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5b02, 0x5b04); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5b04, 0x5b05); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b05, 0x5b07); alu_add(gb, 30);
  CYC(0x5b07, 0x5b09); alu_cp(gb, 61);
  if (!(F & FC)) { RET_TAKEN(0x5b09); return; } // ret nc
  CYC(0x5b09, 0x5b0a);
  CYC(0x5b0a, 0x5b0c); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5b0c, 0x5b0e); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5b0e, 0x5b0f); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b0f, 0x5b11); alu_add(gb, 30);
  CYC(0x5b11, 0x5b13); alu_cp(gb, 61);
  RET(0x5b13); return;
}

void vire_mainForm_fireProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b14, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5b17);
  CYC(0x5b17, 0x5b19); alu_and(gb, 0x01);
  CYC(0x5b19, 0x5b1a); A = alu_inc8(gb, A);
  CYC(0x5b1a, 0x5b1b); B = A;
  vire_mainForm_fireProjectileWithSubid_hook(gb); return; // fallthrough
}

// @param b Subid
void vire_mainForm_fireProjectileWithSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b1b, getFreePartSlot_hook, 0x3e8e, 0x5b1e);
  if (!(F & FZ)) { RET_TAKEN(0x5b1e); return; } // ret nz
  CYC(0x5b1e, 0x5b1f);
  CYC(0x5b1f, 0x5b21); mem_wr(gb, HL, 0x3a); // PART_VIRE_PROJECTILE
  CYC(0x5b21, 0x5b22); L = alu_inc8(gb, L);
  CYC(0x5b22, 0x5b23); mem_wr(gb, HL, B); // [subid]
  CYC(0x5b23, 0x5b25); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(0x5b25, 0x5b26); mem_wr(gb, HL, D);
  CYC(0x5b26, 0x5b27); L = alu_dec8(gb, L);
  CYC(0x5b27, 0x5b29); mem_wr(gb, HL, ENEMY_BASE); // Enemy.start
  CALL_C(0x5b29, objectCopyPosition_hook, 0x2242, 0x5b2c);
  CYC(0x5b2c, 0x5b2e); A = 0x87; // SND_SPLASH
  CALL_C(0x5b2e, playSound_b00_hook, 0x0c98, 0x5b31);
  CYC(0x5b31, 0x5b33); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5b33, 0x5b35); A = 0x01;
  CYC(0x5b35, 0x5b36); mem_wr(gb, DE, A);
  CYC(0x5b36, 0x5b39); enemySetAnimation_hook(gb); return; // jp
}

void vire_batForm_moveAwayFromLinkIfTooClose_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b39, 0x5b3a); H = D;
  CYC(0x5b3a, 0x5b3c); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5b3c, 0x5b3e); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5b3e, 0x5b3f); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b3f, 0x5b41); alu_add(gb, 0x0c);
  CYC(0x5b41, 0x5b43); alu_cp(gb, 0x19);
  if (!(F & FC)) { RET_TAKEN(0x5b43); return; } // ret nc
  CYC(0x5b43, 0x5b44);
  CYC(0x5b44, 0x5b46); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5b46, 0x5b48); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x5b48, 0x5b49); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5b49, 0x5b4b); alu_add(gb, 0x0c);
  CYC(0x5b4b, 0x5b4d); alu_cp(gb, 0x19);
  if (!(F & FC)) { RET_TAKEN(0x5b4d); return; } // ret nc
  CYC(0x5b4d, 0x5b4e);
  CALL_C(0x5b4e, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5b51);
  CYC(0x5b51, 0x5b53); alu_xor(gb, 0x10);
  CYC(0x5b53, 0x5b54); C = A;
  CYC(0x5b54, 0x5b56); B = 0x50; // SPEED_200
  CYC(0x5b56, 0x5b59); ecom_applyGivenVelocity_b0f_hook(gb); return; // jp
}

void vire_batForm_updateZPos_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5b59, ecom_decCounter1_b0f_hook, 0x439a, 0x5b5c);
  CYC(0x5b5c, 0x5b5d); A = mem_rd(gb, HL);
  CYC(0x5b5d, 0x5b5f); alu_and(gb, 0x1c);
  CYC(0x5b5f, 0x5b60); alu_rrca(gb);
  CYC(0x5b60, 0x5b61); alu_rrca(gb);
  CYC(0x5b61, 0x5b64); SET_HL(0x5b6a);
  CYC(0x5b64, 0x5b65); vireEnemy_addAToHl_from_rst(gb, 0x5b65);
  CYC(0x5b65, 0x5b67); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x5b67, 0x5b68); A = mem_rd(gb, HL);
  CYC(0x5b68, 0x5b69); mem_wr(gb, DE, A);
  RET(0x5b69); return;
}
