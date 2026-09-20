#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode75), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode75), (from), (to), true)

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
  BASE(enemyCode75);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+14, b_+16); goto L_5570; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+17); return; } // ret z
  CYC(b_+17, b_+18);
  CYCT(b_+18, b_+20); goto normalStatus; // jr

L_5570:
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+24, b_+26); goto normalStatus; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+28); return; } // ret z
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x0e);
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  RET(b_+36); return;

dead:
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+41, b_+43); goto subid0Dead; } // jr z
  CYC(b_+41, b_+43);
  CALL_C(b_+43, objectCreatePuff_hook, SYM(objectCreatePuff), b_+46);
  CYC(b_+46, b_+48); A = ENEMY_BASE + 0x34; // Object.var34 offset
  CALL_C(b_+48, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+51);
  CYC(b_+51, b_+52); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CALL_C_CC(b_+52, objectCopyPosition_hook, SYM(objectCopyPosition), b_+55); } else { CYC(b_+52, b_+55); } // call z
  CYC(b_+55, b_+58); enemyDelete_hook(gb); return; // jp

subid0Dead:
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(b_+64, b_+67); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x0f); // [state]
  CYC(b_+69, b_+70); L = alu_inc8(gb, L);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x00); // [substate]
  CYC(b_+72, b_+73); L = alu_inc8(gb, L);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x01);
  CYC(b_+79, b_+81); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+81, b_+82); alu_xor(gb, A);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
  CALL_C(b_+83, enemySetAnimation_hook, SYM(enemySetAnimation), b_+86);

normalStatus:
  CALL_C(b_+86, ecom_getSubidAndCpStateTo08_b0f_hook, SYM(ecom_getSubidAndCpStateTo08_b0f), b_+89);
  if (F & FC) { CYCT(b_+89, b_+91); goto commonState; } // jr c
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+92); A = B;
  CYC(b_+92, b_+93); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+93, b_+96); vire_mainForm_hook(gb); return; } // jp z
  CYC(b_+93, b_+96);
  CYC(b_+96, b_+99); vire_batForm_hook(gb); return; // jp

commonState:
  CYC(b_+99, b_+101); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  CYC(b_+102, b_+103); push_effect(gb, b_+103);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == SYM(vire_state_uninitialized)) { vire_state_uninitialized_hook(gb); return; }
    if (target == SYM(vire_state_stub)) { vire_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

void vire_state_uninitialized_hook(GB *gb) {
  BASE(vire_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x1e; // SPEED_c0
  CALL_C(b_+2, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+5);
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xfc);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A); // a = $ff
  CYC(b_+13, b_+15); B = 0x00;
  CYC(b_+15, SYM(vire_state_stub)); enemyBoss_initializeRoom_b0f_hook(gb); return; // jp
}

void vire_state_stub_hook(GB *gb) {
  BASE(vire_state_stub);
  RET(b_+0); return;
}

void vire_mainForm_hook(GB *gb) {
  BASE(vire_mainForm);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto runState; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x18);
  if (F & FC) { CYCT(b_+13, b_+15); goto runState; } // jr c
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); // [var32] = 0
  CYC(b_+17, b_+18); L = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); // [direction] = 0
  CALL_C(b_+19, enemySetAnimation_hook, SYM(enemySetAnimation), b_+22);

runState:
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_sub(gb, 0x08);
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == SYM(vire_mainForm_state8)) { vire_mainForm_state8_hook(gb); return; }
    if (target == SYM(vire_mainForm_state9)) { vire_mainForm_state9_hook(gb); return; }
    if (target == SYM(vire_mainForm_stateA)) { vire_mainForm_stateA_hook(gb); return; }
    if (target == SYM(vire_mainForm_stateB)) { vire_mainForm_stateB_hook(gb); return; }
    if (target == SYM(vire_mainForm_stateC)) { vire_mainForm_stateC_hook(gb); return; }
    if (target == SYM(vire_mainForm_stateD)) { vire_mainForm_stateD_hook(gb); return; }
    if (target == SYM(vire_mainForm_stateE)) { vire_mainForm_stateE_hook(gb); return; }
    if (target == SYM(vire_mainForm_stateF)) { vire_mainForm_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

// Mini-cutscene before starting fight
void vire_mainForm_state8_hook(GB *gb) {
  BASE(vire_mainForm_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE); // [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == b_+11) goto substate0;
    if (target == b_+57) goto substate1;
    if (target == b_+75) goto substate2;
    if (target == b_+95) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+11, b_+13); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+13, b_+15); alu_sub(gb, 0x38);
  CYC(b_+15, b_+17); alu_cp(gb, 0x41);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; } // ret nc
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+20, b_+22); alu_sub(gb, 0x50);
  CYC(b_+22, b_+24); alu_cp(gb, 0x51);
  if (!(F & FC)) { RET_TAKEN(b_+24); return; } // ret nc
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } // ret nz
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+33); SET_BC((SYM(initializeVramMap1) + 21)); // INTERAC_PUFF, subid 2
  CALL_C(b_+33, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+39, b_+40); A = H;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); E = alu_dec8(gb, E);
  CYC(b_+42, b_+44); A = 0x40; // Interaction.start
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+53, b_+56); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  RET(b_+56); return;

substate1:
  CYC(b_+57, b_+59); A = OBJ_ANIM_PARAMETER;
  CALL_C(b_+59, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+62);
  CYC(b_+62, b_+64); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+64); return; } // ret z
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+66); H = D;
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+69, b_+70); L = alu_inc8(gb, L);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(b_+72, b_+75); objectSetVisiblec1_hook(gb); return; // jp

substate2:
  CALL_C(b_+75, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+81); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+78, b_+81);
  CYC(b_+81, b_+82); L = E;
  CYC(b_+82, b_+83); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+83, b_+86); SET_BC((SYM(updateEnemy) + 11)); // TX_2f12
  CALL_C(b_+86, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+89);
  if (F & FZ) { CYCT(b_+89, b_+91); goto L_566e; } // jr z
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+92); C = alu_inc8(gb, C); // TX_2f13

L_566e:
  CYC(b_+92, b_+95); showText_hook(gb); return; // jp

substate3:
  CALL_C(b_+95, objectCreatePuff_hook, SYM(objectCreatePuff), b_+98);
  if (!(F & FZ)) { RET_TAKEN(b_+98); return; } // ret nz
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+102); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+102, b_+105); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+105, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+108);
  CYC(b_+108, b_+109); L = alu_inc8(gb, L);
  CYC(b_+109, b_+110); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [substate]=0
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x5a); // [counter1] = 90
  CYC(b_+112, b_+114); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+114, b_+115); A = mem_rd(gb, HL);
  CYC(b_+115, b_+117); L = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+117, b_+118); mem_wr(gb, HL, A);
  CALL_C(b_+118, objectSetInvisible_hook, SYM(objectSetInvisible), b_+121);
  CYC(b_+121, b_+123); A = 0x2d; // MUS_MINIBOSS
  CYC(b_+123, b_+126); mem_wr(gb, wActiveMusic, A);
  CYC(b_+126, SYM(vire_mainForm_state9)); playSound_b00_hook(gb); return; // jp
}

// Off-screen for [counter1] frames
void vire_mainForm_state9_hook(GB *gb) {
  BASE(vire_mainForm_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); C = 0x08;
  CYC(b_+9, b_+11); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(b_+11, b_+13); goto L_56a8; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); C = 0x04;
  CYC(b_+15, b_+17); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+17, b_+19); goto L_56a8; } // jr c
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); C = 0x00;

L_56a8:
  CALL_C(b_+21, getRandomNumber_hook, SYM(getRandomNumber), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x07);
  CYC(b_+26, b_+27); alu_add(gb, C);
  CYC(b_+27, b_+30); SET_HL(b_+36);
  CYC(b_+30, b_+31); vireEnemy_addAToHl_from_rst(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  RET(b_+35); return;
}

// Charges across screen
void vire_mainForm_stateA_hook(GB *gb) {
  BASE(vire_mainForm_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == b_+9) goto substate0;
    if (target == b_+20) goto substate1;
    if (target == b_+50) goto substate2;
    HANDOFF(target);
  }

substate0:
  CALL_C(b_+9, vire_spawnOutsideCamera_hook, SYM(vire_spawnOutsideCamera), b_+12);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x28); // SPEED_100
  RET(b_+19); return;

substate1:
  CALL_C(b_+20, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); vire_mainForm_applySpeedAndAnimate_hook(gb); return; } // jp nz
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+27); L = E;
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+32, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+35);
  CALL_C(b_+35, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+38);
  CYC(b_+38, b_+40); alu_and(gb, 0x03);
  CYC(b_+40, b_+42); alu_sub(gb, 0x02);
  CYC(b_+42, b_+43); B = A;
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_add(gb, B);
  CYC(b_+47, b_+49); alu_and(gb, 0x1f);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);

substate2:
  CALL_C(b_+50, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+53);
  if (!(F & FC)) { CYCT(b_+53, b_+56); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(b_+53, b_+56);
  CALL_C(b_+56, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+59);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+62); alu_and(gb, 0x1f);
  if (F & FZ) { CALL_C_CC(b_+62, vire_mainForm_fireProjectile_hook, SYM(vire_mainForm_fireProjectile), b_+65); } else { CYC(b_+62, b_+65); } // call z
  CYC(b_+65, SYM(vire_mainForm_stateB)); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Shared block: spawn outside camera, set counter1=120, pick var30 offset (used by state B's
// substate0 and, via the same ROM bytes, state D's substate0)
void vire_mainForm_stateB_substate0_hook(GB *gb) {
  BASE(vire_mainForm_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+11, vire_spawnOutsideCamera_hook, SYM(vire_spawnOutsideCamera), b_+14);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CALL_C(b_+17, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+20);
  CYC(b_+20, b_+22); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto L_5725; } // jr nz
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); A = 0xf8;

L_5725:
  CYC(b_+26, b_+28); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return;
}

// Circling Link, runs away if Link gets too close (similar to state D)
void vire_mainForm_stateB_hook(GB *gb) {
  BASE(vire_mainForm_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == b_+11) { vire_mainForm_stateB_substate0_hook(gb); return; }
    if (target == b_+30) goto substate1;
    if (target == b_+68) goto substate2;
    if (target == b_+96) goto substate3;
    HANDOFF(target);
  }

substate1:
  CYC(b_+30, b_+33); A = mem_rd(gb, wFrameCounter);
  CYC(b_+33, b_+35); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_573d; } // jr nz
  CYC(b_+35, b_+37);
  CALL_C(b_+37, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+40);
  if (F & FZ) { CYCT(b_+40, b_+42); goto beginCharge; } // jr z
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_and(gb, 0x1f);
  CYC(b_+45, b_+47); B = 0x01;
  if (F & FZ) { CALL_C_CC(b_+47, vire_mainForm_fireProjectileWithSubid_hook, SYM(vire_mainForm_fireProjectileWithSubid), b_+50); } else { CYC(b_+47, b_+50); } // call z

L_573d:
  CALL_C(b_+50, vire_mainForm_checkLinkTooClose_hook, SYM(vire_mainForm_checkLinkTooClose), b_+53);
  if (!(F & FC)) { CYCT(b_+53, b_+56); vire_mainForm_circleAroundScreen_hook(gb); return; } // jp nc
  CYC(b_+53, b_+56);

beginCharge:
  CYC(b_+56, b_+58); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+58, b_+59); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+63, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+66);
  CYCT(b_+66, b_+68); goto animate; // jr

substate2:
  CALL_C(b_+68, vire_mainForm_checkLinkTooClose_hook, SYM(vire_mainForm_checkLinkTooClose), b_+71);
  if (F & FC) { CYCT(b_+71, b_+73); goto updateAngleAway; } // jr c
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+76); A = mem_rd(gb, wLinkUsingItem1);
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto updateAngleAway; } // jr nz
  CYC(b_+77, b_+79);
  CALL_C(b_+79, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+82);
  if (!(F & FC)) { CYCT(b_+82, b_+85); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(b_+82, b_+85);
  CYC(b_+85, b_+88); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp

updateAngleAway:
  CYC(b_+88, b_+89); L = E;
  CYC(b_+89, b_+90); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CALL_C(b_+90, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0f), b_+93);

animate:
  CYC(b_+93, b_+96); enemyAnimate_hook(gb); return; // jp

substate3:
  CALL_C(b_+96, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+99);
  if (!(F & FC)) { CYCT(b_+99, b_+102); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(b_+99, b_+102);
  CALL_C(b_+102, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+105);
  CYC(b_+105, b_+106); A = mem_rd(gb, HL);
  CYC(b_+106, b_+108); alu_and(gb, 0x1f);
  CYC(b_+108, b_+110); B = 0x01;
  if (F & FZ) { CALL_C_CC(b_+110, vire_mainForm_fireProjectileWithSubid_hook, SYM(vire_mainForm_fireProjectileWithSubid), b_+113); } else { CYC(b_+110, b_+113); } // call z
  CYC(b_+113, SYM(vire_mainForm_stateC)); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Vire creeps in from the screen edge to fire one projectile, then runs away
void vire_mainForm_stateC_hook(GB *gb) {
  BASE(vire_mainForm_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == b_+11) goto substate0;
    if (target == b_+22) goto substate1;
    if (target == b_+39) goto substate2;
    if (target == b_+59) goto substate3;
    HANDOFF(target);
  }

substate0:
  CALL_C(b_+11, vire_spawnOutsideCamera_hook, SYM(vire_spawnOutsideCamera), b_+14);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x1c); // [counter1]
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x28); // SPEED_100
  RET(b_+21); return;

substate1:
  CALL_C(b_+22, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); vire_mainForm_applySpeedAndAnimate_hook(gb); return; } // jp nz
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+32, b_+34); B = 0x03;
  CALL_C(b_+34, vire_mainForm_fireProjectileWithSubid_hook, SYM(vire_mainForm_fireProjectileWithSubid), b_+37);
  CYCT(b_+37, b_+39); goto animate; // jr

substate2:
  CALL_C(b_+39, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto animate; } // jr nz
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); L = E;
  CYC(b_+45, b_+46); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+46, b_+48); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); alu_xor(gb, 0x10);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x50); // SPEED_200

animate:
  CYC(b_+56, b_+59); enemyAnimate_hook(gb); return; // jp

substate3:
  CALL_C(b_+59, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+62);
  if (!(F & FC)) { CYCT(b_+62, b_+65); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(b_+62, b_+65);
  CYC(b_+65, SYM(vire_mainForm_stateD)); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Circling Link, runs away if Link attempts to attack (similar to state B)
void vire_mainForm_stateD_hook(GB *gb) {
  BASE(vire_mainForm_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == SYM(vire_mainForm_stateB__substate0)) { vire_mainForm_stateB_substate0_hook(gb); return; }
    if (target == b_+15) goto substate1;
    if (target == b_+54) goto substate2;
    if (target == b_+84) goto substate3;
    if (target == b_+106) goto substate4;
    if (target == SYM(vire_state_moveOffScreen)) { vire_state_moveOffScreen_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  CYC(b_+15, b_+18); A = mem_rd(gb, wFrameCounter);
  CYC(b_+18, b_+20); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_57e6; } // jr nz
  CYC(b_+20, b_+22);
  CALL_C(b_+22, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+27); goto beginCharge; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_and(gb, 0x1f);
  CYC(b_+30, b_+32); B = 0x01;
  if (F & FZ) { CALL_C_CC(b_+32, vire_mainForm_fireProjectileWithSubid_hook, SYM(vire_mainForm_fireProjectileWithSubid), b_+35); } else { CYC(b_+32, b_+35); } // call z

L_57e6:
  CALL_C(b_+35, vire_mainForm_checkLinkTooClose_hook, SYM(vire_mainForm_checkLinkTooClose), b_+38);
  if (!(F & FC)) { CYCT(b_+38, b_+41); vire_mainForm_circleAroundScreen_hook(gb); return; } // jp nc
  CYC(b_+38, b_+41);

beginCharge:
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+48, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+51);

animate:
  CYC(b_+51, b_+54); enemyAnimate_hook(gb); return; // jp

substate2:
  CYC(b_+54, b_+57); A = mem_rd(gb, wLinkUsingItem1);
  CYC(b_+57, b_+58); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+58, b_+60); goto moveOffScreen; } // jr z
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+62); L = E;
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+63, b_+64); L = alu_inc8(gb, L);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x78); // SPEED_300
  CALL_C(b_+70, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0f), b_+73);
  CYCT(b_+73, b_+75); goto animate; // jr

moveOffScreen:
  CALL_C(b_+75, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+78);
  if (!(F & FC)) { CYCT(b_+78, b_+81); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(b_+78, b_+81);
  CYC(b_+81, b_+84); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp

substate3:
  CALL_C(b_+84, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+87);
  if (!(F & FZ)) { CYCT(b_+87, b_+90); vire_mainForm_applySpeedAndAnimate_hook(gb); return; } // jp nz
  CYC(b_+87, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(b_+92, b_+93); L = E;
  CYC(b_+93, b_+94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+94, b_+96); B = 0x3a; // PART_VIRE_PROJECTILE
  CALL_C(b_+96, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+99);
  CYC(b_+99, b_+101); A = 0x87; // SND_SPLASH
  CALL_C(b_+101, playSound_b00_hook, SYM(playSound_b00), b_+104);
  CYCT(b_+104, b_+106); goto animate; // jr

substate4:
  CALL_C(b_+106, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+109);
  if (!(F & FZ)) { CYCT(b_+109, b_+111); goto animate; } // jr nz
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+112); L = E;
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+113, b_+115); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x46); // SPEED_1c0
  CALL_C(b_+117, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0f), b_+120);
  CYCT(b_+120, SYM(vire_state_moveOffScreen)); goto animate; // jr
}

// Used by states D and E
void vire_state_moveOffScreen_hook(GB *gb) {
  BASE(vire_state_moveOffScreen);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+6); vire_mainForm_leftScreen_hook(gb); return; } // jp nc
  CYC(b_+3, b_+6);
  CYC(b_+6, SYM(vire_mainForm_stateE)); vire_mainForm_applySpeedAndAnimate_hook(gb); return; // jp
}

// Just took damage
void vire_mainForm_stateE_hook(GB *gb) {
  BASE(vire_mainForm_stateE);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == b_+9) goto substate0;
    if (target == b_+32) goto substate1;
    if (target == SYM(vire_state_moveOffScreen)) { vire_state_moveOffScreen_hook(gb); return; }
    HANDOFF(target);
  }

substate0:
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x14); // [counter1]
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x78); // SPEED_300
  CALL_C(b_+19, ecom_updateCardinalAngleAwayFromTarget_b0f_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0f), b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+32); enemySetAnimation_hook(gb); return; // jp

substate1:
  CALL_C(b_+32, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+35);
  if (!(F & FZ)) { CYCT(b_+35, b_+38); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+35, b_+38);
  CYC(b_+38, b_+39); L = E;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_cp(gb, 0x10);
  if (!(F & FC)) { RET_TAKEN(b_+45); return; } // ret nc
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); B = 0x01;
  CYC(b_+48, b_+50); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(b_+50, b_+52); goto L_587b; } // jr nc
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); B = alu_inc8(gb, B);

L_587b:
  CYC(b_+53, b_+54); A = B;
  CYC(b_+54, b_+56); L = ENEMY_BASE + 0x37; // Enemy.var37
  CYC(b_+56, b_+57); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+57); return; } // ret z
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+59); mem_wr(gb, HL, A);
  CYC(b_+59, b_+61); alu_add(gb, 0x13);
  CYC(b_+61, b_+62); C = A;
  CYC(b_+62, b_+64); B = 0x2f; // TX_2f00 hi
  CYC(b_+64, SYM(vire_mainForm_stateF)); showText_hook(gb); return; // jp
}

// "Main form" died, about to split into bats
void vire_mainForm_stateF_hook(GB *gb) {
  BASE(vire_mainForm_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == b_+15) goto substate0;
    if (target == b_+27) goto substate1;
    if (target == b_+82) goto substate2;
    if (target == b_+101) goto substate3;
    if (target == b_+114) goto substate4;
    if (target == b_+142) goto substate5;
    HANDOFF(target);
  }

substate0:
  CALL_C(b_+15, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+18);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); L = E;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+21, b_+24); SET_BC((SYM(updateEnemy) + 15)); // TX_2f16
  CYC(b_+24, b_+27); showText_hook(gb); return; // jp

substate1:
  CYC(b_+27, b_+29); B = 0x02;
  CALL_C(b_+29, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+35); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+39, b_+41); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x02);
  CALL_C(b_+43, objectSetInvisible_hook, SYM(objectSetInvisible), b_+46);
  CALL_C(b_+46, objectCreatePuff_hook, SYM(objectCreatePuff), b_+49);
  CYC(b_+49, b_+51); B = 0x75; // ENEMY_VIRE
  CALL_C(b_+51, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+54);
  CALL_C(b_+54, vire_mainForm_stateF_initBat_hook, b_+61, b_+57);
  CALL_C(b_+57, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+60);
  CYC(b_+60, b_+61); A = alu_inc8(gb, A);
  vire_mainForm_stateF_initBat_hook(gb); return; // fallthrough

substate2:
  CYC(b_+82, b_+84); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+86, b_+89); ecom_decCounter2_b0f_hook(gb); return; } // jp nz
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+92, b_+93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(b_+96, b_+98); A = 0xf0; // SNDCTRL_STOPMUSIC
  CYC(b_+98, b_+101); playSound_b00_hook(gb); return; // jp

substate3:
  CALL_C(b_+101, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+107); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(b_+104, b_+107);
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(b_+109, b_+110); L = E;
  CYC(b_+110, b_+111); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+111, b_+114); objectSetVisiblec1_hook(gb); return; // jp

substate4:
  CALL_C(b_+114, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+117);
  if (!(F & FZ)) { CYCT(b_+117, b_+120); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+117, b_+120);
  CYC(b_+120, b_+121); L = E;
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+122, b_+124); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x06);
  CYC(b_+126, b_+128); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(b_+130, b_+133); SET_BC((SYM(updateEnemy) + 16)); // TX_2f17
  CALL_C(b_+133, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+136);
  if (F & FZ) { CYCT(b_+136, b_+138); goto L_5914; } // jr z
  CYC(b_+136, b_+138);
  CYC(b_+138, b_+139); C = alu_inc8(gb, C); // TX_2f18

L_5914:
  CYC(b_+139, b_+142); showText_hook(gb); return; // jp

substate5:
  CALL_C(b_+142, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+145);
  if (F & FZ) { CYCT(b_+145, b_+147); goto unlinked; } // jr z
  CYC(b_+145, b_+147);

linked:
  CYC(b_+147, b_+149); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+149, b_+150); alu_xor(gb, A);
  CYC(b_+150, b_+151); mem_wr(gb, DE, A);
  RET(b_+151); return;

unlinked:
  CYC(b_+152, b_+154); E = ENEMY_BASE + 0x38; // Enemy.var38
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+156); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+156, b_+158); goto L_592e; } // jr nz
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+159); A = alu_inc8(gb, A);
  CYC(b_+159, b_+160); mem_wr(gb, DE, A);
  CYC(b_+160, b_+162); B = 0x01; // PART_ITEM_DROP
  CALL_C(b_+162, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+165);

L_592e:
  CALL_C(b_+165, enemyAnimate_hook, SYM(enemyAnimate), b_+168);
  CYC(b_+168, b_+169); H = D;
  CYC(b_+169, b_+171); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+171, b_+172); A = mem_rd(gb, HL);
  CYC(b_+172, b_+174); alu_sub(gb, 0x80);
  CYC(b_+174, b_+175); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+175, b_+176); A = mem_rd(gb, HL);
  CYC(b_+176, b_+178); alu_sbc(gb, 0x00);
  CYC(b_+178, b_+179); mem_wr(gb, HL, A);
  CALL_C(b_+179, vire_checkOffScreen_hook, SYM(vire_checkOffScreen), b_+182);
  if (F & FC) { CYCT(b_+182, b_+185); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+182, b_+185);
  CALL_C(b_+185, markEnemyAsKilledInRoom_b02_hook, SYM(markEnemyAsKilledInRoom_b00), b_+188);
  CALL_C(b_+188, decNumEnemies_hook, SYM(decNumEnemies), b_+191);
  CYC(b_+191, b_+194); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+194, b_+197); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+197, playSound_b00_hook, SYM(playSound_b00), b_+200);
  CYC(b_+200, SYM(vire_batForm)); enemyDelete_hook(gb); return; // jp
}

void vire_mainForm_stateF_initBat_hook(GB *gb) {
  BASE(vire_mainForm_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+61, b_+62); L = alu_inc8(gb, L);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A); // [var03] = a
  CYC(b_+63, b_+64); alu_rrca(gb);
  CYC(b_+64, b_+66); A = alu_swap(gb, A);
  if (!(F & FZ)) { CYCT(b_+66, b_+68); goto L_58cf; } // jr nz
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); A = 0xf8;

L_58cf:
  CYC(b_+70, b_+72); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+72, b_+73); mem_wr(gb, HL, A);
  CYC(b_+73, b_+75); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+75, b_+77); A = ENEMY_BASE; // Enemy.start
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+78, b_+79); mem_wr(gb, HL, D);
  CYC(b_+79, b_+82); objectCopyPosition_hook(gb); return; // jp
}

void vire_batForm_hook(GB *gb) {
  BASE(vire_batForm);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = vireEnemy_jump_table(gb);
    if (target == SYM(vire_batForm_state8)) { vire_batForm_state8_hook(gb); return; }
    if (target == SYM(vire_batForm_state9)) { vire_batForm_state9_hook(gb); return; }
    if (target == SYM(vire_batForm_stateA)) { vire_batForm_stateA_hook(gb); return; }
    if (target == SYM(vire_batForm_stateB)) { vire_batForm_stateB_hook(gb); return; }
    if (target == SYM(vire_batForm_stateC)) { vire_batForm_stateC_hook(gb); return; }
    if (target == SYM(vire_batForm_stateD)) { vire_batForm_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

void vire_batForm_state8_hook(GB *gb) {
  BASE(vire_batForm_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x1f);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x10);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x01);
  CYC(b_+19, b_+21); A = 0x02;
  CALL_C(b_+21, enemySetAnimation_hook, SYM(enemySetAnimation), b_+24);
  CYC(b_+24, SYM(vire_batForm_state9)); objectSetVisiblec1_hook(gb); return; // jp
}

// Moving upward after charging (or after spawning)
void vire_batForm_state9_hook(GB *gb) {
  BASE(vire_batForm_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); vire_batForm_gotoStateA_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+8, b_+10); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(b_+10, b_+12); goto L_5993; } // jr c
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_sub(gb, 0xc0);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_sbc(gb, 0x00);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);

L_5993:
  CALL_C(b_+20, objectApplySpeed_hook, SYM(objectApplySpeed), b_+23);
  CYCT(b_+23, SYM(vire_batForm_gotoStateA)); vire_batForm_animate_hook(gb); return; // jr
}

void vire_batForm_gotoStateA_hook(GB *gb) {
  BASE(vire_batForm_gotoStateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); L = E;
  CYC(b_+1, b_+3); mem_wr(gb, HL, 0x0a); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(b_+11, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); A = OBJ_COUNTER2;
  CALL_C(b_+19, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0xb4); // [mainForm.counter2] = 180
  CYCT(b_+24, SYM(vire_batForm_stateA)); vire_batForm_animate_hook(gb); return; // jr
}

void vire_batForm_stateA_hook(GB *gb) {
  BASE(vire_batForm_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, vire_batForm_updateZPos_hook, SYM(vire_batForm_updateZPos), b_+3);
  CYC(b_+3, b_+5); A = OBJ_COUNTER2;
  CALL_C(b_+5, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_59c6; } // jr nz
  CYC(b_+10, b_+12);
  CALL_C(b_+12, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x08);
  RET(b_+19); return;

L_59c6:
  CALL_C(b_+20, vire_batForm_moveAwayFromLinkIfTooClose_hook, SYM(vire_batForm_moveAwayFromLinkIfTooClose), b_+23);
  CALL_C(b_+23, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+26);
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+29); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_add(gb, B);
  CYC(b_+31, b_+33); alu_and(gb, 0x1f);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); A = 0x02;
  CALL_C(b_+38, ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), b_+41);
  if (F & FZ) { CALL_C_CC(b_+41, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(vire_batForm_animate)); } else { CYC(b_+41, SYM(vire_batForm_animate)); } // call z
  vire_batForm_animate_hook(gb); return; // fallthrough
}

void vire_batForm_animate_hook(GB *gb) {
  BASE(vire_batForm_animate);
  CYC(b_+0, SYM(vire_batForm_stateB)); enemyAnimate_hook(gb); return; // jp
}

// About to charge toward Link in [counter1] frames
void vire_batForm_stateB_hook(GB *gb) {
  BASE(vire_batForm_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+10, b_+12); L = ENEMY_BASE + 0x35; // Enemy.var35
  CYC(b_+12, b_+14); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+15, b_+17); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  RET(b_+18); return;
}

// Charging toward target position in var35/var36
void vire_batForm_stateC_hook(GB *gb) {
  BASE(vire_batForm_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x35; // Enemy.var35
  CALL_C(b_+3, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x08);
  CYC(b_+9, b_+11); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto notReachedPosition; } // jr nc
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = hram_rd(gb, 0x8f);
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CYC(b_+16, b_+18); alu_add(gb, 0x08);
  CYC(b_+18, b_+20); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto notReachedPosition; } // jr nc
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0xfa);
  if (F & FC) { CYCT(b_+27, b_+29); goto notReachedPosition; } // jr c
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); L = E;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x14);
  CYCT(b_+35, b_+37); vire_batForm_animate_hook(gb); return; // jr

notReachedPosition:
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0xfe);
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto L_5a21; } // jr nc
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

L_5a21:
  CALL_C(b_+45, ecom_moveTowardPosition_b0f_hook, SYM(ecom_moveTowardPosition_b0f), b_+48);
  CYCT(b_+48, SYM(vire_batForm_stateD)); vire_batForm_animate_hook(gb); return; // jr
}

// Moving back up after charging
void vire_batForm_stateD_hook(GB *gb) {
  BASE(vire_batForm_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); vire_batForm_gotoStateA_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+9, b_+11); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(b_+11, b_+13); goto L_5a3b; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_sub(gb, 0xc0);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_sbc(gb, 0x00);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);

L_5a3b:
  CYC(b_+21, b_+23); A = 0x02;
  CALL_C(b_+23, ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), b_+26);
  if (F & FZ) { CALL_C_CC(b_+26, objectApplySpeed_hook, SYM(objectApplySpeed), b_+29); } else { CYC(b_+26, b_+29); } // call z
  CYCT(b_+29, SYM(vire_spawnOutsideCamera)); vire_batForm_animate_hook(gb); return; // jr
}

// Sets Vire's position to just outside the camera, increments substate.
void vire_spawnOutsideCamera_hook(GB *gb) {
  BASE(vire_spawnOutsideCamera);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_add(gb, A);
  CYC(b_+7, b_+8); alu_add(gb, B);
  CYC(b_+8, b_+11); SET_HL(b_+41);
  CYC(b_+11, b_+12); vireEnemy_addAToHl_from_rst(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+16); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+21, b_+23); A = hram_rd(gb, 0xac); // hCameraX
  CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); SET_HL(HL + 1);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+33, b_+35); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+38, b_+41); objectSetVisiblec1_hook(gb); return; // jp
}

// Vire has left the screen; set state to 9, where he'll wait for 90 frames before attacking again.
void vire_mainForm_leftScreen_hook(GB *gb) {
  BASE(vire_mainForm_leftScreen);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00); // [substate]
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x5a); // [counter1] = 90
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+15, SYM(vire_checkOffScreen)); objectSetInvisible_hook(gb); return; // jp
}

// @param[out] cflag c if left screen
void vire_checkOffScreen_hook(GB *gb) {
  BASE(vire_checkOffScreen);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xb8);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0xf0);
  RET(b_+11); return;
}

void vire_mainForm_circleAroundScreen_hook(GB *gb) {
  BASE(vire_mainForm_circleAroundScreen);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(b_+2, b_+4); alu_add(gb, 0x44);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); A = hram_rd(gb, 0xac); // hCameraX
  CYC(b_+7, b_+9); alu_add(gb, 0x50);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); push_effect(gb, BC);
  CALL_C(b_+11, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+14);
  CYC(b_+14, b_+15); SET_BC(pop_effect(gb));
  CYC(b_+15, b_+16); H = A;
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto L_5abc; } // jr nc
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); alu_cpl(gb);
  CYC(b_+23, b_+24); A = alu_inc8(gb, A);

L_5abc:
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+27); alu_cp(gb, 0x3e);
  CYC(b_+27, b_+28); A = H;
  if (!(F & FC)) { CYCT(b_+28, b_+30); goto setAngleAndSpeed; } // jr nc
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); alu_sub(gb, C);
  if (!(F & FC)) { CYCT(b_+34, b_+36); goto L_5aca; } // jr nc
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); alu_cpl(gb);
  CYC(b_+37, b_+38); A = alu_inc8(gb, A);

L_5aca:
  CYC(b_+38, b_+39); C = A;
  CYC(b_+39, b_+41); alu_cp(gb, 0x3e);
  CYC(b_+41, b_+42); A = H;
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto setAngleAndSpeed; } // jr nc
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); A = B;
  CYC(b_+45, b_+46); alu_add(gb, C);
  CYC(b_+46, b_+48); alu_sub(gb, 0x42);
  CYC(b_+48, b_+50); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+50, b_+52); goto offsetAngle; } // jr c
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); alu_rlca(gb);
  CYC(b_+53, b_+54); A = H;
  if (!(F & FC)) { CYCT(b_+54, b_+56); goto setAngleAndSpeed; } // jr nc
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); alu_xor(gb, 0x10);

setAngleAndSpeed:
  CYC(b_+58, b_+59); push_effect(gb, HL);
  CYC(b_+59, b_+61); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+64); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+64, b_+66); A = 0x0a; // SPEED_40
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CALL_C(b_+67, objectApplySpeed_hook, SYM(objectApplySpeed), b_+70);
  CYC(b_+70, b_+71); SET_HL(pop_effect(gb));

offsetAngle:
  CYC(b_+71, b_+73); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); alu_add(gb, H);
  CYC(b_+75, b_+77); alu_and(gb, 0x1f);
  CYC(b_+77, b_+79); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+82); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+82, b_+84); A = 0x23; // SPEED_e0
  CYC(b_+84, SYM(vire_mainForm_applySpeedAndAnimate)); mem_wr(gb, DE, A);
  vire_mainForm_applySpeedAndAnimate_hook(gb); return; // fallthrough
}

void vire_mainForm_applySpeedAndAnimate_hook(GB *gb) {
  BASE(vire_mainForm_applySpeedAndAnimate);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, SYM(vire_mainForm_checkLinkTooClose)); enemyAnimate_hook(gb); return; // jp
}

// @param[out] cflag c if Link is too close (Vire will flee)
void vire_mainForm_checkLinkTooClose_hook(GB *gb) {
  BASE(vire_mainForm_checkLinkTooClose);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_add(gb, 30);
  CYC(b_+8, b_+10); alu_cp(gb, 61);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+15); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+18); alu_add(gb, 30);
  CYC(b_+18, b_+20); alu_cp(gb, 61);
  RET(b_+20); return;
}

void vire_mainForm_fireProjectile_hook(GB *gb) {
  BASE(vire_mainForm_fireProjectile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, SYM(vire_mainForm_fireProjectileWithSubid)); B = A;
  vire_mainForm_fireProjectileWithSubid_hook(gb); return; // fallthrough
}

// @param b Subid
void vire_mainForm_fireProjectileWithSubid_hook(GB *gb) {
  BASE(vire_mainForm_fireProjectileWithSubid);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x3a); // PART_VIRE_PROJECTILE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, B); // [subid]
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(b_+10, b_+11); mem_wr(gb, HL, D);
  CYC(b_+11, b_+12); L = alu_dec8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, ENEMY_BASE); // Enemy.start
  CALL_C(b_+14, objectCopyPosition_hook, SYM(objectCopyPosition), b_+17);
  CYC(b_+17, b_+19); A = 0x87; // SND_SPLASH
  CALL_C(b_+19, playSound_b00_hook, SYM(playSound_b00), b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, SYM(vire_batForm_moveAwayFromLinkIfTooClose)); enemySetAnimation_hook(gb); return; // jp
}

void vire_batForm_moveAwayFromLinkIfTooClose_hook(GB *gb) {
  BASE(vire_batForm_moveAwayFromLinkIfTooClose);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_add(gb, 0x0c);
  CYC(b_+8, b_+10); alu_cp(gb, 0x19);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+15); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+18); alu_add(gb, 0x0c);
  CYC(b_+18, b_+20); alu_cp(gb, 0x19);
  if (!(F & FC)) { RET_TAKEN(b_+20); return; } // ret nc
  CYC(b_+20, b_+21);
  CALL_C(b_+21, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+24);
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+27); C = A;
  CYC(b_+27, b_+29); B = 0x50; // SPEED_200
  CYC(b_+29, SYM(vire_batForm_updateZPos)); ecom_applyGivenVelocity_b0f_hook(gb); return; // jp
}

void vire_batForm_updateZPos_hook(GB *gb) {
  BASE(vire_batForm_updateZPos);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x1c);
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+11); SET_HL(b_+17);
  CYC(b_+11, b_+12); vireEnemy_addAToHl_from_rst(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return;
}
