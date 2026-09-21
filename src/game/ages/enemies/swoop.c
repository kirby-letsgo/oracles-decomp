#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void swoop_state_uninitialized_hook(GB *gb);
void swoop_state_stub_hook(GB *gb);
void swoop_state8_hook(GB *gb);
void swoop_state9_hook(GB *gb);
void swoop_stateA_hook(GB *gb);
void swoop_stateB_hook(GB *gb);
void swoop_setSpeedZForBounce_hook(GB *gb);
void swoop_setVisible_hook(GB *gb);
void swoop_stomp_substate2_hook(GB *gb);
void swoop_beginFlyingUp_hook(GB *gb);
void swoop_flyFurtherUp_hook(GB *gb);
void swoop_stomp_substate3_hook(GB *gb);
void swoop_getAngerLevel_hook(GB *gb);
void swoop_hitGround_hook(GB *gb);
void swoop_animate_hook(GB *gb);

static uint16_t swoop_jump_table(GB *gb) {
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

static void swoop_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SWOOP
//
// Variables:
//   var30: Number of frames before swoop begins to stomp
//   var31: Target stomp position (short-form)
//   var32/var33: Target stomp position (long-form)
// ==================================================================================================
void enemyCode71_hook(GB *gb) {
  BASE(enemyCode71);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); goto normalStatus; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11);
  TAIL(enemyBoss_dead_b0f); // jp

normalStatus:
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  {
    CYC(b_+14, b_+15); push_effect(gb, b_+15);
    uint16_t target = swoop_jump_table(gb);
    if (target == SYM(swoop_state_uninitialized)) { swoop_state_uninitialized_hook(gb); return; }
    if (target == SYM(swoop_state_stub)) { swoop_state_stub_hook(gb); return; }
    if (target == SYM(swoop_state8)) { swoop_state8_hook(gb); return; }
    if (target == SYM(swoop_state9)) { swoop_state9_hook(gb); return; }
    if (target == SYM(swoop_stateA)) { swoop_stateA_hook(gb); return; }
    if (target == SYM(swoop_stateB)) { swoop_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

void swoop_state_uninitialized_hook(GB *gb) {
  BASE(swoop_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x71; // ENEMY_SWOOP
  CYC(b_+2, b_+4); B = 0x00;
  CALL_C(b_+4, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CALL_C(b_+7, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+10);
  CYC(b_+10, b_+12); B = 0x01;
  CYC(b_+12, b_+14); C = 0x08;
  CYC(b_+14, b_+17);
  TAIL(enemyBoss_spawnShadow_b0f); // jp
}

void swoop_state_stub_hook(GB *gb) {
  BASE(swoop_state_stub);
  RET(b_+0); return; // ret
}

// Spawning in before the fight starts
void swoop_state8_hook(GB *gb) {
  BASE(swoop_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = swoop_jump_table(gb);
    if (target == b_+12) goto substate0;
    if (target == b_+50) goto substate1;
    if (target == b_+102) goto substate2;
    if (target == b_+114) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+12, b_+14); A = 0x01; // DISABLE_LINK
  CYC(b_+14, b_+17); W8(wDisabledObjects) = A;
  CYC(b_+17, b_+20); W8(wMenuDisabled) = A;

  // Wait for door to close
  CYC(b_+20, b_+23); A = W8(wcc93);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);
  CALL_C(b_+25, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+28);
  CYC(b_+28, b_+30); C = 0x08;
  CALL_C(b_+30, ecom_setZAboveScreen_b0f_hook, SYM(ecom_setZAboveScreen_b0f), b_+33);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+37); A = 0x3c;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+39); E = alu_inc8(gb, E);
  CYC(b_+39, b_+41); A = 0x02;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A); // [counter2]
  CALL_C(b_+42, objectSetVisible82_hook, SYM(objectSetVisible82), b_+45);
  CYC(b_+45, b_+47); A = 0x02;
  CYC(b_+47, b_+50);
  TAIL(enemySetAnimation); // jp

// Falling to ground
substate1:
  CYC(b_+50, b_+52); C = 0x10;
  CALL_C(b_+52, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+55);
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; } // ret nz
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+60, b_+62); goto doneBouncing; } // jr z
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); A = alu_dec8(gb, A);
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); goto bounceAgain; } // jr nz
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0x00;
  CALL_C(b_+68, enemySetAnimation_hook, SYM(enemySetAnimation), b_+71);
  CYC(b_+71, b_+73);
  goto doneBouncing; // jr

bounceAgain:
  CYC(b_+73, b_+76); SET_BC(0xfe80);
  CALL_C(b_+76, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+79);
  CYC(b_+79, b_+81); A = 0x0a;
  CALL_C(b_+81, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+84);
  CYC(b_+84, b_+86); A = 0x70; // SND_DOORCLOSE
  CYC(b_+86, b_+89);
  TAIL(playSound_b00); // jp

doneBouncing:
  CALL_C(b_+89, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+92);
  if (!(F & FZ)) { RET_TAKEN(b_+92); return; } // ret nz
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+96); SET_BC(SYM(updateEnemies__next)); // TX_2f00
  CALL_C(b_+96, showText_hook, SYM(showText), b_+99);
  CYC(b_+99, b_+102);
  TAIL(ecom_incSubstate_b0f); // jp

substate2:
  CALL_C(b_+102, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+105);
  CALL_C(b_+105, enemyBoss_beginMiniboss_b0f_hook, SYM(enemyBoss_beginMiniboss_b0f), b_+108);
  CALL_C(b_+108, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+111);
  CYC(b_+111, b_+114);
  TAIL(swoop_beginFlyingUp); // jp

substate3:
  CALL_C(b_+114, swoop_state9_hook, SYM(swoop_state9), b_+117);
  CYC(b_+117, b_+119); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  CYC(b_+120, b_+122); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { RET_TAKEN(b_+122); return; } // ret nz
  CYC(b_+122, b_+123);
  CYC(b_+123, b_+124); alu_xor(gb, A);
  CYC(b_+124, b_+127); W8(wDisabledObjects) = A;
  CYC(b_+127, b_+130); W8(wMenuDisabled) = A;
  RET(b_+130); return; // ret
}

// Flying upward
void swoop_state9_hook(GB *gb) {
  BASE(swoop_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, swoop_animate_hook, SYM(swoop_animate), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); alu_or(gb, A);
  CYC(b_+5, b_+8); SET_BC(0xff00);
  if (!(F & FZ)) { CALL_C_CC(b_+8, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+11); } else { CYC(b_+8, b_+11); } // call nz
  CYC(b_+11, b_+13); C = 0x08;
  CALL_C(b_+13, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+16);
  CALL_C(b_+16, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CALL_C(b_+20, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); swoop_flyFurtherUp_hook(gb); return; } // jp nz
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(b_+28, b_+30); A = 0x0a;
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x00); // [substate]
  CALL_C(b_+35, swoop_getAngerLevel_hook, SYM(swoop_getAngerLevel), b_+38);
  CYC(b_+38, b_+41); SET_HL(SYM(swoop_framesBeforeAttacking)); // swoop_framesBeforeAttacking
  CYC(b_+41, b_+42); swoop_addAToHl_from_rst(gb, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CALL_C(b_+46, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+49);
  CYC(b_+49, b_+51); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+54); A = 0x00;
  CYC(b_+54, b_+57);
  TAIL(enemySetAnimation); // jp
}

// Flying around, getting closer to Link before stomping
void swoop_stateA_hook(GB *gb) {
  BASE(swoop_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, swoop_animate_hook, SYM(swoop_animate), b_+3);
  CALL_C(b_+3, swoop_getAngerLevel_hook, SYM(swoop_getAngerLevel), b_+6);
  CYC(b_+6, b_+9); SET_HL(SYM(swoop_speedVals)); // swoop_speedVals
  CYC(b_+9, b_+10); swoop_addAToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+16); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+20); goto checkDistance; } // jr z
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); A = alu_dec8(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto updatePosition; } // jr nz
  CYC(b_+22, b_+24);

checkDistance:
  CYC(b_+24, b_+26); C = 0x30;
  CALL_C(b_+26, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+29);
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto updatePosition; } // jr nc
  CYC(b_+29, b_+31);
  CALL_C(b_+31, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+34);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x00); // [substate]
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x1e);
  RET(b_+41); return; // ret

updatePosition:
  CALL_C(b_+42, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto applyVelocity; } // jr nz
  CALL_C(b_+47, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+50);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+55, b_+57); A = 0x3c;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);

applyVelocity:
  CYC(b_+58, b_+61);
  TAIL(ecom_applyVelocityForSideviewEnemy_b0f); // jp
}

// Stomping
void swoop_stateB_hook(GB *gb) {
  BASE(swoop_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = swoop_jump_table(gb);
    if (target == b_+12) goto substate0;
    if (target == b_+84) goto substate1;
    if (target == SYM(swoop_stomp_substate2)) { swoop_stomp_substate2_hook(gb); return; }
    if (target == SYM(swoop_stomp_substate3)) { swoop_stomp_substate3_hook(gb); return; }
    HANDOFF(target);
  }

// Flapping wings quickly, telegraphing stomp is about to begin
substate0:
  CALL_C(b_+12, swoop_animate_hook, SYM(swoop_animate), b_+15);
  CALL_C(b_+15, swoop_animate_hook, SYM(swoop_animate), b_+18);
  CALL_C(b_+18, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+21);
  if (F & FZ) { CYCT(b_+21, b_+23); goto beginStomp; } // jr z
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+24, b_+26); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);

  // Decide on target position to stomp at, store in var31
  CYC(b_+27, b_+30); SET_HL(w1Link_yh);
  CYC(b_+30, b_+31); B = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+33); C = mem_rd(gb, HL); // [w1Link.xh]
  CALL_C(b_+33, getTileAtPosition_hook, SYM(getTileAtPosition), b_+37);
  CYC(b_+37, b_+38); A = L;
  CYC(b_+38, b_+40); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);

  // Convert to long-form, store in var32/var33
  CALL_C(b_+41, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+46, b_+47); A = B;
  CYC(b_+47, b_+49); alu_and(gb, 0xf0);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+51); E = alu_inc8(gb, E);
  CYC(b_+51, b_+52); A = C;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);

  // Get angle toward stomp position
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+55, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+58);
  CYC(b_+58, b_+60); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  RET(b_+61); return; // ret

beginStomp:
  CALL_C(b_+62, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+65);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+71, b_+73); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+73, b_+76); SET_BC(0x0000);
  CALL_C(b_+76, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+79);
  CYC(b_+79, b_+81); A = 0x02;
  CYC(b_+81, b_+84);
  TAIL(enemySetAnimation); // jp

// Moving toward stomp position while falling to ground
substate1:
  // Get target stomp position
  CYC(b_+84, b_+85); H = D;
  CYC(b_+85, b_+87); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+87, b_+88); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+88, b_+89); C = mem_rd(gb, HL);
  CYC(b_+89, b_+90); B = A;

  // Compare with current position
  CYC(b_+90, b_+92); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+92, b_+93); L = E;
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+94, b_+96); alu_and(gb, 0xfe);
  CYC(b_+96, b_+97); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto mustMove; } // jr nz
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+100); L = alu_inc8(gb, L);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+103); alu_and(gb, 0xfe);
  CYC(b_+103, b_+104); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+104, b_+106); goto reachedStompTarget; } // jr z
  CYC(b_+104, b_+106);

mustMove:
  CALL_C(b_+106, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+109);
  CYC(b_+109, b_+111); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+111, b_+112); mem_wr(gb, DE, A);
  CALL_C(b_+112, ecom_applyVelocityForSideviewEnemy_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0f), b_+115);

reachedStompTarget:
  CYC(b_+115, b_+117); C = 0x10;
  CALL_C(b_+117, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+120);
  if (!(F & FZ)) { RET_TAKEN(b_+120); return; } // ret nz
  CYC(b_+120, b_+121);

  // Hit the ground.
  CALL_C(b_+121, swoop_hitGround_hook, SYM(swoop_hitGround), b_+124);
  CALL_C(b_+124, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+127);
  CYC(b_+127, b_+129); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+132); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+132, b_+134); swoop_setVisible_hook(gb); return; } // jr nc
  CYC(b_+132, b_+134);

  // Health is low; will bounce either 2 or 3 times.
  CYC(b_+134, b_+135); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate] = 3

  // [counter1] = number of bounces
  CALL_C(b_+135, getRandomNumber_hook, SYM(getRandomNumber), b_+138);
  CYC(b_+138, b_+140); alu_and(gb, 0x01);
  CYC(b_+140, b_+141); A = alu_inc8(gb, A);
  CYC(b_+141, b_+143); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+143, b_+144); mem_wr(gb, HL, A);
  CYC(b_+144, b_+146); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+148, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+151);
  CYC(b_+151, b_+153); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+153, b_+154); mem_wr(gb, DE, A);
  TAIL(swoop_setSpeedZForBounce); // fallthrough
}

void swoop_setSpeedZForBounce_hook(GB *gb) {
  BASE(swoop_setSpeedZForBounce);
  CYC(b_+0, b_+3); SET_BC(0xff00);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ); // jp
}

void swoop_setVisible_hook(GB *gb) {
  BASE(swoop_setVisible);
  CYC(b_+0, b_+3);
  TAIL(objectSetVisible82); // jp
}

// Completed stomp, about to fly back up.
void swoop_stomp_substate2_hook(GB *gb) {
  BASE(swoop_stomp_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, swoop_animate_hook, SYM(swoop_animate), b_+3);

  // Wait until animation signals to fly up again, or Link attacks
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto flyBackUp; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+14); return; } // ret z
  CYC(b_+14, b_+15);

flyBackUp:
  CYC(b_+15, b_+18); SET_BC(0x0000);
  CALL_C(b_+18, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+21);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+25); A = 0x09;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00); // [substate]
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  TAIL(swoop_beginFlyingUp); // fallthrough
}

void swoop_beginFlyingUp_hook(GB *gb) {
  BASE(swoop_beginFlyingUp);
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x03); // 3 flaps before he goes to next state
  TAIL(swoop_flyFurtherUp); // fallthrough
}

void swoop_flyFurtherUp_hook(GB *gb) {
  BASE(swoop_flyFurtherUp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x30); // $30 frames per wing flap
  CALL_C(b_+4, objectSetVisible80_hook, SYM(objectSetVisible80), b_+7);
  CYC(b_+7, b_+9); A = 0x03;
  CYC(b_+9, b_+12);
  TAIL(enemySetAnimation); // jp
}

// Bouncing
void swoop_stomp_substate3_hook(GB *gb) {
  BASE(swoop_stomp_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemy_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0f), b_+3);
  CYC(b_+3, b_+5); C = 0x10;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CALL_C(b_+9, swoop_hitGround_hook, SYM(swoop_hitGround), b_+12);
  CALL_C(b_+12, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); swoop_setSpeedZForBounce_hook(gb); return; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+23);
  TAIL(objectSetVisible82); // jp
}

// @param[out] a Value from 0-2
void swoop_getAngerLevel_hook(GB *gb) {
  BASE(swoop_getAngerLevel);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto haveAnger; } // jr nc
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CYC(b_+10, b_+12); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto haveAnger; } // jr nc
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); B = alu_inc8(gb, B);

haveAnger:
  CYC(b_+15, b_+16); A = B;
  RET(b_+16); return; // ret
}

void swoop_hitGround_hook(GB *gb) {
  BASE(swoop_hitGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30;
  CALL_C(b_+2, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+5);
  CYC(b_+5, b_+7); A = 0x70; // SND_DOORCLOSE
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);

  // Replace tile at this position if it's of the appropriate type, and not solid.
  CYC(b_+10, b_+13); SET_BC(0x0500);
  CALL_C(b_+13, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+16);
  CYC(b_+16, b_+17); C = L;
  CYC(b_+17, b_+19); H = 0xce; // >wRoomCollisions
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_cp(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+22); return; } // ret z
  CYC(b_+22, b_+23);

  CYC(b_+23, b_+25); H = 0xcf; // >wRoomLayout
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+28); alu_cp(gb, 0xa2);
  if (F & FZ) { RET_TAKEN(b_+28); return; } // ret z
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); alu_cp(gb, 0x48);
  if (F & FZ) { RET_TAKEN(b_+31); return; } // ret z
  CYC(b_+31, b_+32);

  CYC(b_+32, b_+34); A = 0x48;
  CALL_C(b_+34, setTile_hook, SYM(setTile), b_+37);
  CYC(b_+37, b_+39); B = 0x06; // INTERAC_ROCKDEBRIS
  CYC(b_+39, b_+42);
  TAIL(objectCreateInteractionWithSubid00); // jp
}

// @param[out] de animParameter (if nonzero, just flapped wings)
void swoop_animate_hook(GB *gb) {
  BASE(swoop_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = 0x53; // SND_JUMP
  CYC(b_+10, b_+13);
  TAIL(playSound_b00); // jp
}
