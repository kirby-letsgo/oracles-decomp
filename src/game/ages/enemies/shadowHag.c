#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode7a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode7a), (from), (to), true)

void enemyCode7a_hook(GB *gb);
void shadowHag_state_uninitialized_hook(GB *gb);
void shadowHag_state_stub_hook(GB *gb);
void shadowHag_state8_hook(GB *gb);
void shadowHag_state9_hook(GB *gb);
void shadowHag_stateA_hook(GB *gb);
void shadowHag_stateB_hook(GB *gb);
void shadowHag_initStateC_hook(GB *gb);
void shadowHag_stateC_hook(GB *gb);
void shadowHag_stateD_hook(GB *gb);
void shadowHag_stateE_hook(GB *gb);
void shadowHag_stateF_hook(GB *gb);
void shadowHag_state10_hook(GB *gb);
void shadowHag_state11_hook(GB *gb);
void shadowHag_animate_hook(GB *gb);
void shadowHag_state12_hook(GB *gb);
void shadowHag_doneCharging_hook(GB *gb);
void shadowHag_state13_hook(GB *gb);
void shadowHag_beginEmergingFromShadow_hook(GB *gb);
void shadowHag_updateEmergingFromShadow_hook(GB *gb);
void shadowHag_updateReturningToGround_hook(GB *gb);
void shadowHag_beginReturningToGround_hook(GB *gb);
void shadowHag_chooseSpawnPosition_hook(GB *gb);
void shadowHag_checkLinkLookedAtHag_hook(GB *gb);

static uint16_t shadowHag_jump_table(GB *gb) {
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

static void shadowHag_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void shadowHag_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// ENEMY_SHADOW_HAG
//
// Variables:
//   counter2: Number of times to spawn bugs before shadows separate
//   var30: Number of bugs on-screen
//   var31: Set if the hag couldn't spawn because Link was in a bad position
// ==================================================================================================

void enemyCode7a_hook(GB *gb) {
  BASE(enemyCode7a);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto dead; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_HL(ENEMY_SLOTS); // FIRST_ENEMY_INDEX, Enemy.start

killNext:
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0x42); // ENEMY_SHADOW_HAG_BUG
  if (F & FZ) CALL_C_CC(b_+21, ecom_killObjectH_b0f_hook, SYM(ecom_killObjectH_b0f), b_+24); else CYC(b_+21, b_+24); // call z
  CYC(b_+24, b_+25); H = alu_inc8(gb, H);
  CYC(b_+25, b_+26); A = H;
  CYC(b_+26, b_+28); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+28, b_+30); goto killNext; } // jr c
  CYC(b_+28, b_+30);

dead:
  CYC(b_+30, b_+33); enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); push_effect(gb, b_+37);
  {
    uint16_t target = shadowHag_jump_table(gb);
    if (target == SYM(shadowHag_state_uninitialized)) { shadowHag_state_uninitialized_hook(gb); return; }
    if (target == SYM(shadowHag_state_stub)) { shadowHag_state_stub_hook(gb); return; }
    if (target == SYM(shadowHag_state8)) { shadowHag_state8_hook(gb); return; }
    if (target == SYM(shadowHag_state9)) { shadowHag_state9_hook(gb); return; }
    if (target == SYM(shadowHag_stateA)) { shadowHag_stateA_hook(gb); return; }
    if (target == SYM(shadowHag_stateB)) { shadowHag_stateB_hook(gb); return; }
    if (target == SYM(shadowHag_stateC)) { shadowHag_stateC_hook(gb); return; }
    if (target == SYM(shadowHag_stateD)) { shadowHag_stateD_hook(gb); return; }
    if (target == SYM(shadowHag_stateE)) { shadowHag_stateE_hook(gb); return; }
    if (target == SYM(shadowHag_stateF)) { shadowHag_stateF_hook(gb); return; }
    if (target == SYM(shadowHag_state10)) { shadowHag_state10_hook(gb); return; }
    if (target == SYM(shadowHag_state11)) { shadowHag_state11_hook(gb); return; }
    if (target == SYM(shadowHag_state12)) { shadowHag_state12_hook(gb); return; }
    if (target == SYM(shadowHag_state13)) { shadowHag_state13_hook(gb); return; }
    HANDOFF(target);
  }
}

void shadowHag_state_uninitialized_hook(GB *gb) {
  BASE(shadowHag_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x7a; // ENEMY_SHADOW_HAG
  CYC(b_+2, b_+4); B = 0x00;
  CALL_C(b_+4, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CYC(b_+7, b_+9); A = 0x14; // SPEED_80
  CYC(b_+9, SYM(shadowHag_state_stub)); ecom_setSpeedAndState8_b0f_hook(gb); return; // jp
}

void shadowHag_state_stub_hook(GB *gb) {
  BASE(shadowHag_state_stub);
  RET(b_+0); return; // ret
}

void shadowHag_state8_hook(GB *gb) {
  BASE(shadowHag_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = shadowHag_jump_table(gb);
    if (target == b_+13) goto substate0;
    if (target == b_+53) goto substate1;
    if (target == b_+78) goto substate2;
    if (target == b_+91) goto substate3;
    if (target == b_+106) goto substate4;
    HANDOFF(target);
  }

substate0:
  CYC(b_+13, b_+16); A = mem_rd(gb, wcc93);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } // ret nz
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+22, b_+25); SET_BC(0x0104);
  CALL_C(b_+25, enemyBoss_spawnShadow_b0f_hook, SYM(enemyBoss_spawnShadow_b0f), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; } // ret nz
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x18);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0xff);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+43, b_+45); A = H8(hEnemyTargetY);
  CYC(b_+45, b_+47); alu_add(gb, 0x04);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+48, b_+49); L = alu_inc8(gb, L);
  CYC(b_+49, b_+51); A = H8(hEnemyTargetX);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  RET(b_+53); return; // ret

substate1:
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+58); alu_cp(gb, 0x78); // (LARGE_ROOM_WIDTH/2)<<4 + 8
  if (!(F & FC)) { CYCT(b_+58, b_+61); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+58, b_+61);
  CALL_C(b_+61, shadowHag_beginEmergingFromShadow_hook, SYM(shadowHag_beginEmergingFromShadow), b_+64);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+68, b_+69); L = alu_inc8(gb, L);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x00);
  CYC(b_+75, b_+78); ecom_killRelatedObj2_b0f_hook(gb); return; // jp

substate2:
  CALL_C(b_+78, shadowHag_updateEmergingFromShadow_hook, SYM(shadowHag_updateEmergingFromShadow), b_+81);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; } // ret nz
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+84); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+84, b_+86); A = 0x03;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+88); A = alu_dec8(gb, A);
  CYC(b_+88, b_+91); enemySetAnimation_hook(gb); return; // jp

substate3:
  CALL_C(b_+91, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+94);
  if (!(F & FZ)) { CYCT(b_+94, b_+96); goto animate; } // jr nz
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(b_+98, b_+99); L = E;
  CYC(b_+99, b_+100); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+100, b_+103); SET_BC((SYM(updateEnemy) + 36)); // TX_2f2b
  CYC(b_+103, b_+106); showText_hook(gb); return; // jp

substate4:
  CALL_C(b_+106, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+109);
  if (!(F & FZ)) { CYCT(b_+109, b_+111); goto animate; } // jr nz
  CYC(b_+109, b_+111);
  CALL_C(b_+111, shadowHag_beginReturningToGround_hook, SYM(shadowHag_beginReturningToGround), b_+114);
  CALL_C(b_+114, enemyBoss_beginBoss_b0f_hook, SYM(enemyBoss_beginBoss_b0f), b_+117);

animate:
  CYC(b_+117, SYM(shadowHag_state9)); enemyAnimate_hook(gb); return; // jp
}

// Currently in the ground, showing eyes
void shadowHag_state9_hook(GB *gb) {
  BASE(shadowHag_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); shadowHag_updateReturningToGround_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = alu_dec8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto spawnShadows; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+12, b_+15); ecom_flickerVisibility_b0f_hook(gb); return; // jp

spawnShadows:
  CYC(b_+15, b_+17); B = 0x04;
  CALL_C(b_+17, checkBPartSlotsAvailable_hook, SYM(checkBPartSlotsAvailable), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+24); SET_BC((SYM(ecom_fallingInHole_b0f) + 39)); // PART_SHADOW_HAG_SHADOW,4

L_6c55:
  CALL_C(b_+24, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+27);
  CYC(b_+27, b_+28); C = alu_dec8(gb, C);
  CYC(b_+28, b_+30); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+31); mem_wr(gb, HL, C);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto L_6c55; } // jr nz
  CYC(b_+31, b_+33);
  CALL_C(b_+33, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x96); // 150
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x04); // [counter2]
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+47, SYM(shadowHag_stateA)); objectSetInvisible_hook(gb); return; // jp
}

// Shadows chasing Link
void shadowHag_stateA_hook(GB *gb) {
  BASE(shadowHag_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter1]=$ff
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]=$0b
  CALL_C(b_+12, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x06);
  CYC(b_+17, b_+20); SET_HL(b_+30); // @targetPositions
  CYC(b_+20, b_+21); shadowHag_addAToHl_from_rst(gb, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return; // ret
}

// Shadows reconverging to target position
void shadowHag_stateB_hook(GB *gb) {
  BASE(shadowHag_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CALL_C(b_+5, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x01);
  CYC(b_+10, b_+12); alu_add(gb, 0x02);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+14, SYM(shadowHag_initStateC)); mem_wr(gb, DE, A);
  shadowHag_initStateC_hook(gb); return; // fallthrough
}

void shadowHag_initStateC_hook(GB *gb) {
  BASE(shadowHag_initStateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0c);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x03);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x05);
  CALL_C(b_+20, objectSetVisible83_hook, SYM(objectSetVisible83), b_+23);
  CYC(b_+23, b_+25); A = 0x04;
  CYC(b_+25, SYM(shadowHag_stateC)); enemySetAnimation_hook(gb); return; // jp
}

// Delay before spawning bugs
void shadowHag_stateC_hook(GB *gb) {
  BASE(shadowHag_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_6cc9; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x41); // [state]
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

L_6cc9:
  CYC(b_+9, SYM(shadowHag_stateD)); enemyAnimate_hook(gb); return; // jp
}

// Spawning bugs
void shadowHag_stateD_hook(GB *gb) {
  BASE(shadowHag_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto doneSpawningBugs; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x07);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; } // ret nc
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); B = 0x42; // ENEMY_SHADOW_HAG_BUG
  CALL_C(b_+20, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+26, b_+28); A = ENEMY_BASE; // Enemy.start
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+29, b_+30); mem_wr(gb, HL, D);
  CALL_C(b_+30, objectCopyPosition_hook, SYM(objectCopyPosition), b_+33);
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+37); return; // ret

doneSpawningBugs:
  CALL_C(b_+38, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+41);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x1e); // 30
  RET(b_+45); return; // ret
}

// Done spawning bugs; delay before the hag herself spawns in
void shadowHag_stateE_hook(GB *gb) {
  BASE(shadowHag_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  CYC(b_+10, b_+12); A = 0x5a; // 90
  if (F & FZ) { CYCT(b_+12, b_+14); goto L_6d0c; } // jr z
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+18); A = 0x96; // 150

L_6d0c:
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); // [counter1]
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+24, b_+26); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+26, SYM(shadowHag_stateF)); objectSetInvisible_hook(gb); return; // jp
}

// Waiting for Link to be in a position where the hag can spawn behind him
void shadowHag_stateF_hook(GB *gb) {
  BASE(shadowHag_stateF);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto couldntSpawn; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, shadowHag_chooseSpawnPosition_hook, SYM(shadowHag_chooseSpawnPosition), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = C;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, shadowHag_beginEmergingFromShadow_hook, SYM(shadowHag_beginEmergingFromShadow), b_+20);
  CYC(b_+20, b_+23); ecom_incState_b0f_hook(gb); return; // jp

couldntSpawn:
  CYC(b_+23, b_+25); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+25, b_+27); A = 0x01;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); L = alu_inc8(gb, L);
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]--
  if (!(F & FZ)) { CYCT(b_+30, b_+33); shadowHag_initStateC_hook(gb); return; } // jp nz
  CYC(b_+30, b_+33);
  CALL_C(b_+33, shadowHag_beginReturningToGround_hook, SYM(shadowHag_beginReturningToGround), b_+36);
  CYC(b_+36, b_+38); A = 0x04;
  CYC(b_+38, SYM(shadowHag_state10)); enemySetAnimation_hook(gb); return; // jp
}

// Spawning out of ground to attack Link
void shadowHag_state10_hook(GB *gb) {
  BASE(shadowHag_state10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, shadowHag_updateEmergingFromShadow_hook, SYM(shadowHag_updateEmergingFromShadow), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x80 | 0x7a); // ENEMY_SHADOW_HAG
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xff);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x0c);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x08);
  CALL_C(b_+30, ecom_updateCardinalAngleTowardTarget_b0f_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0f), b_+33);
  CYC(b_+33, SYM(shadowHag_state11)); ecom_updateAnimationFromAngle_b0f_hook(gb); return; // jp
}

// Delay before charging at Link
void shadowHag_state11_hook(GB *gb) {
  BASE(shadowHag_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, shadowHag_checkLinkLookedAtHag_hook, SYM(shadowHag_checkLinkLookedAtHag), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); shadowHag_doneCharging_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, SYM(shadowHag_animate)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  shadowHag_animate_hook(gb); return; // fallthrough
}

void shadowHag_animate_hook(GB *gb) {
  BASE(shadowHag_animate);
  CYC(b_+0, SYM(shadowHag_state12)); enemyAnimate_hook(gb); return; // jp
}

// Charging at Link
void shadowHag_state12_hook(GB *gb) {
  BASE(shadowHag_state12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, shadowHag_checkLinkLookedAtHag_hook, SYM(shadowHag_checkLinkLookedAtHag), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); shadowHag_doneCharging_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); shadowHag_doneCharging_hook(gb); return; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_sub(gb, 0x12);
  CYC(b_+15, b_+17); alu_cp(gb, 0x7e); // (LARGE_ROOM_HEIGHT<<4)-$32
  if (!(F & FC)) { CYCT(b_+17, b_+19); shadowHag_doneCharging_hook(gb); return; } // jr nc
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_sub(gb, 0x18);
  CYC(b_+24, b_+26); alu_cp(gb, 0xc0); // (LARGE_ROOM_WIDTH<<4)-$30
  if (!(F & FC)) { CYCT(b_+26, b_+28); shadowHag_doneCharging_hook(gb); return; } // jr nc
  CYC(b_+26, b_+28);
  CALL_C(b_+28, objectApplySpeed_hook, SYM(objectApplySpeed), b_+31);
  CYCT(b_+31, SYM(shadowHag_doneCharging)); shadowHag_animate_hook(gb); return; // jr
}

void shadowHag_doneCharging_hook(GB *gb) {
  BASE(shadowHag_doneCharging);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); shadowHag_beginReturningToGround_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  CYC(b_+17, b_+19); A = 0x06;
  CYC(b_+19, SYM(shadowHag_state13)); enemySetAnimation_hook(gb); return; // jp
}

// Delay before spawning bugs again
void shadowHag_state13_hook(GB *gb) {
  BASE(shadowHag_state13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); shadowHag_updateReturningToGround_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, SYM(shadowHag_beginEmergingFromShadow)); shadowHag_initStateC_hook(gb); return; // jp
}

void shadowHag_beginEmergingFromShadow_hook(GB *gb) {
  BASE(shadowHag_beginEmergingFromShadow);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x05;
  CALL_C(b_+2, enemySetAnimation_hook, SYM(enemySetAnimation), b_+5);
  CALL_C(b_+5, objectSetVisible82_hook, SYM(objectSetVisible82), b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_sub(gb, 0x04);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

// @param[out] zflag z if done emerging? (animParameter was $ff)
void shadowHag_updateEmergingFromShadow_hook(GB *gb) {
  BASE(shadowHag_updateEmergingFromShadow);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); alu_sub(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_sub(gb, 0x08);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); alu_or(gb, D);
  RET(b_+19); return; // ret
}

void shadowHag_updateReturningToGround_hook(GB *gb) {
  BASE(shadowHag_updateReturningToGround);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  CYC(b_+12, b_+15); SET_HL(b_+26); // @yOffsets
  CYC(b_+15, b_+16); shadowHag_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  RET(b_+25); return; // ret
}

// Sets state to 9 & initializes stuff
void shadowHag_beginReturningToGround_hook(GB *gb) {
  BASE(shadowHag_beginReturningToGround);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x5a); // 90
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x1e); // [counter2] = 30
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x03);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x05);
  CYC(b_+23, b_+25); A = 0x06;
  CYC(b_+25, SYM(shadowHag_chooseSpawnPosition)); enemySetAnimation_hook(gb); return; // jp
}

// Chooses position to spawn at for charge attack based on Link's facing direction.
// @param[out] bc Spawn position
// @param[out] zflag nz if Link is too close to the wall to spawn in
void shadowHag_chooseSpawnPosition_hook(GB *gb) {
  BASE(shadowHag_chooseSpawnPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_direction);
  CYC(b_+3, b_+6); SET_HL(b_+40); // @spawnOffsets
  CYC(b_+6, b_+7); shadowHag_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_yh);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+14); alu_sub(gb, 0x1c);
  CYC(b_+14, b_+16); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto invalid; } // jr nc
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); SET_HL(HL + 1); // inc hl
  CYC(b_+19, b_+22); A = mem_rd(gb, w1Link_xh);
  CYC(b_+22, b_+23); E = A;
  CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+27); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto invalid; } // jr nc
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(b_+30, b_+32); goto L_6e31; } // jr nc
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); alu_cpl(gb);
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);

L_6e31:
  CYC(b_+34, b_+35); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+35, b_+38); getTileCollisionsAtPosition_hook(gb); return; } // jp nc
  CYC(b_+35, b_+38);

invalid:
  CYC(b_+38, b_+39); alu_or(gb, D);
  RET(b_+39); return; // ret
}

// @param[out] zflag z if Link looked at the hag
void shadowHag_checkLinkLookedAtHag_hook(GB *gb) {
  BASE(shadowHag_checkLinkLookedAtHag);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x14);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rlca(gb);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_direction);
  CYC(b_+14, b_+15); alu_cp(gb, B);
  RET(b_+15); return; // ret
}
