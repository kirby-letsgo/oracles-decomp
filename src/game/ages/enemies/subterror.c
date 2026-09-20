#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode72), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode72), (from), (to), true)

void subterror_state_uninitialized_hook(GB *gb);
void subterror_state_stub_hook(GB *gb);
void subterror_state8_hook(GB *gb);
void subterror_digIntoGround_hook(GB *gb);
void subterror_state9_hook(GB *gb);
void subterror_beginUndergroundMovement_hook(GB *gb);
void subterror_stateA_hook(GB *gb);
void subterror_stateB_hook(GB *gb);
void subterror_stateC_hook(GB *gb);
void subterror_spawnDirtEvery8Frames_hook(GB *gb);
void subterror_spawnDirt_hook(GB *gb);
void subterror_retFromCallerIfAnimationUnfinished_hook(GB *gb);
void subterror_getAngerLevel_hook(GB *gb);
void subterror_setAnimationFromAngle_hook(GB *gb);
void subterror_stateA_resetUndergroundMovement_hook(GB *gb);

static uint16_t subterror_jump_table(GB *gb) {
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

static void subterror_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SUBTERROR
//
// Variables:
//   var30: If nonzero, dirt is created at subterror's position every 8 frames.
//   var31: Counter until a new dirt object (PART_SUBTERROR_DIRT) is created.
// ==================================================================================================
void enemyCode72_hook(GB *gb) {
  BASE(enemyCode72);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10);
  enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(b_+10, b_+12); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CALL_C_CC(b_+14, subterror_spawnDirtEvery8Frames_hook, SYM(subterror_spawnDirtEvery8Frames), b_+17); } else { CYC(b_+14, b_+17); } // call nz
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  {
    CYC(b_+20, b_+21); push_effect(gb, b_+21);
    uint16_t target = subterror_jump_table(gb);
    if (target == SYM(subterror_state_uninitialized)) { subterror_state_uninitialized_hook(gb); return; }
    if (target == SYM(subterror_state_stub)) { subterror_state_stub_hook(gb); return; }
    if (target == SYM(subterror_state8)) { subterror_state8_hook(gb); return; }
    if (target == SYM(subterror_state9)) { subterror_state9_hook(gb); return; }
    if (target == SYM(subterror_stateA)) { subterror_stateA_hook(gb); return; }
    if (target == SYM(subterror_stateB)) { subterror_stateB_hook(gb); return; }
    if (target == SYM(subterror_stateC)) { subterror_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

void subterror_state_uninitialized_hook(GB *gb) {
  BASE(subterror_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x72; // ENEMY_SUBTERROR
  CYC(b_+2, b_+4); B = 0xbe; // PALH_be
  CALL_C(b_+4, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CALL_C(b_+7, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+10);
  CYC(b_+10, b_+12); A = 0x07;
  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); // [var30]
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x1e);
  RET(b_+28); return; // ret
}

void subterror_state_stub_hook(GB *gb) {
  BASE(subterror_state_stub);
  RET(b_+0); return; // ret
}

// Cutscene before fight
void subterror_state8_hook(GB *gb) {
  BASE(subterror_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = subterror_jump_table(gb);
    if (target == b_+12) goto substate0;
    if (target == b_+67) goto substate1;
    if (target == b_+93) goto substate2;
    if (target == b_+117) goto substate3;
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
  CALL_C(b_+25, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; } // ret nz
  CYC(b_+28, b_+29);

  // Move further down
  CALL_C(b_+29, objectApplySpeed_hook, SYM(objectApplySpeed), b_+32);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+37); alu_cp(gb, 0x58);
  if (F & FC) { RET_TAKEN(b_+37); return; } // ret c
  CYC(b_+37, b_+38);

  // Reached middle of screen, about to pop out
  CYC(b_+38, b_+40); A = 0xa9; // SND_DIG
  CALL_C(b_+40, playSound_b00_hook, SYM(playSound_b00), b_+43);
  CYC(b_+43, b_+45); A = 0x06;
  CALL_C(b_+45, enemySetAnimation_hook, SYM(enemySetAnimation), b_+48);
  CALL_C(b_+48, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+51);
  CALL_C(b_+51, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+54);

  // Disable dirt animation
  CYC(b_+54, b_+56); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x00);
  CALL_C(b_+58, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+61);
  CYC(b_+61, b_+62); C = L;
  CYC(b_+62, b_+64); A = 0x4c; // TILEINDEX_DUNGEON_DUG_DIRT
  CYC(b_+64, b_+67);
  setTile_hook(gb); return; // jp

substate1:
  CALL_C(b_+67, subterror_retFromCallerIfAnimationUnfinished_hook, SYM(subterror_retFromCallerIfAnimationUnfinished), b_+70);
  CYC(b_+70, b_+72); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(b_+72, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+75);
  CALL_C(b_+75, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+78);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x3c);
  CYC(b_+82, b_+85); SET_BC(0xfe00);
  CALL_C(b_+85, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+88);
  CYC(b_+88, b_+90); A = 0x05;
  CYC(b_+90, b_+93);
  enemySetAnimation_hook(gb); return; // jp

substate2:
  CYC(b_+93, b_+95); C = 0x10;
  CALL_C(b_+95, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+98);
  if (!(F & FZ)) { RET_TAKEN(b_+98); return; } // ret nz
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+101); A = 0x02;
  CALL_C(b_+101, enemySetAnimation_hook, SYM(enemySetAnimation), b_+104);
  CALL_C(b_+104, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+107);
  if (!(F & FZ)) { RET_TAKEN(b_+107); return; } // ret nz
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+111); SET_BC((SYM(updateEnemies__next) + 3)); // TX_2f03
  CALL_C(b_+111, showText_hook, SYM(showText), b_+114);
  CYC(b_+114, b_+117);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate3:
  CALL_C(b_+117, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+120);
  CALL_C(b_+120, enemyBoss_beginMiniboss_b0f_hook, SYM(enemyBoss_beginMiniboss_b0f), b_+123);
  CYC(b_+123, b_+124); alu_xor(gb, A);
  CYC(b_+124, b_+127); W8(wDisabledObjects) = A;
  CYC(b_+127, SYM(subterror_digIntoGround)); W8(wMenuDisabled) = A;
  subterror_digIntoGround_hook(gb); return; // fallthrough
}

void subterror_digIntoGround_hook(GB *gb) {
  BASE(subterror_digIntoGround);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x09;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x04;
  CYC(b_+7, SYM(subterror_state9));
  enemySetAnimation_hook(gb); return; // jp
}

// Digging into ground
void subterror_state9_hook(GB *gb) {
  BASE(subterror_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, subterror_retFromCallerIfAnimationUnfinished_hook, SYM(subterror_retFromCallerIfAnimationUnfinished), SYM(subterror_beginUndergroundMovement));
  subterror_beginUndergroundMovement_hook(gb); return; // fallthrough
}

// Done digging, about to start moving around
void subterror_beginUndergroundMovement_hook(GB *gb) {
  BASE(subterror_beginUndergroundMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0a);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); // [substate]
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); // [angle] = $ff
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x69); // ENEMYCOLLISION_SUBTERROR_UNDERGROUND
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+24, subterror_getAngerLevel_hook, SYM(subterror_getAngerLevel), b_+27);
  CYC(b_+27, b_+30); SET_HL(SYM(subterror_timeUntilDrillAttack)); // subterror_timeUntilDrillAttack
  CYC(b_+30, b_+31); subterror_addAToHl_from_rst(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); A = 0xa9; // SND_DIG
  CALL_C(b_+37, playSound_b00_hook, SYM(playSound_b00), b_+40);
  CYC(b_+40, SYM(subterror_stateA));
  subterror_spawnDirt_hook(gb); return; // jp
}

// Currently in the ground, moving around
void subterror_stateA_hook(GB *gb) {
  BASE(subterror_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = subterror_jump_table(gb);
    if (target == b_+10) goto substate0;
    if (target == b_+59) goto substate1;
    if (target == b_+207) goto substate2;
    HANDOFF(target);
  }

// Staying underground for [counter1] frames before moving
substate0:
  CALL_C(b_+10, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);
  CALL_C(b_+14, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+17);
  subterror_stateA_resetUndergroundMovement_hook(gb); return; // fallthrough

// Moving around until shovel is used or he starts drilling
substate1:
  CYC(b_+59, b_+61); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+64); A = alu_sla(gb, A);
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto noShovel; } // jr nc
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); alu_cp(gb, 0x18); // ITEMCOLLISION_SHOVEL<<1
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto noShovel; } // jr nz
  CYC(b_+68, b_+70);

  // Shovel was used; will now pop out of ground
  CYC(b_+70, b_+73); SET_BC(0xff00);
  CALL_C(b_+73, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+76);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+80, b_+82); A = 0x0c;
  CYC(b_+82, b_+84); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+84, b_+85); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+85, b_+86); alu_xor(gb, A);
  CYC(b_+86, b_+87); mem_wr(gb, HL, A); // [substate] = 0
  CYC(b_+87, b_+89); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+89, b_+90); mem_wr(gb, HL, A); // [var30] = 0
  CYC(b_+90, b_+91); A = alu_inc8(gb, A);
  CYC(b_+91, b_+93); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+93, b_+94); mem_wr(gb, HL, A); // [counter1] = 1
  CYC(b_+94, b_+96); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+96, b_+98); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  // Bounces away from Link
  CALL_C(b_+98, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+101);
  CYC(b_+101, b_+103); alu_xor(gb, 0x10);
  CYC(b_+103, b_+105); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  CYC(b_+106, b_+108); A = 0x06;
  CALL_C(b_+108, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+111);
  CYC(b_+111, b_+113); A = 0x05;
  CYC(b_+113, b_+116);
  enemySetAnimation_hook(gb); return; // jp

noShovel:
  CALL_C(b_+116, objectApplySpeed_hook, SYM(objectApplySpeed), b_+119);
  CYC(b_+119, b_+121); A = 0x01;
  CALL_C(b_+121, ecom_getSideviewAdjacentWallsBitset_b0f_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0f), b_+124);
  if (F & FZ) { CYCT(b_+124, b_+126); goto stillUnderground; } // jr z

  // Hit wall
  CYC(b_+124, b_+126);
  CALL_C(b_+126, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+129);
  CYC(b_+129, b_+131); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x5a);
  CYC(b_+133, b_+135); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+135, b_+137); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+137, b_+139); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x00);
  RET(b_+141); return; // ret

stillUnderground:
  CALL_C(b_+142, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+145);
  if (F & FZ) {
    CYCT(b_+145, b_+148); // call z
    push_effect(gb, b_+148);
    subterror_stateA_resetUndergroundMovement_hook(gb);
  } else {
    CYC(b_+145, b_+148);
  }
  CALL_C(b_+148, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+151);
  if (!(F & FZ)) { RET_TAKEN(b_+151); return; } // ret nz
  CYC(b_+151, b_+152);

  // If Link is close enough, drill him
  CYC(b_+152, b_+154); C = 0x18;
  CALL_C(b_+154, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+157);
  if (!(F & FC)) { RET_TAKEN(b_+157); return; } // ret nc
  CYC(b_+157, b_+158);

  // "Transport" to the tile at Link's position
  CYC(b_+158, b_+161); SET_HL(w1Link_yh);
  CYC(b_+161, b_+162); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+162, b_+163); L = alu_inc8(gb, L);
  CYC(b_+163, b_+164); C = mem_rd(gb, HL);
  CYC(b_+164, b_+165); B = A;
  CALL_C(b_+165, getTileAtPosition_hook, SYM(getTileAtPosition), b_+168);
  CYC(b_+168, b_+169); C = L;
  CALL_C(b_+169, convertShortToLongPosition_paramC_hook, SYM(convertShortToLongPosition_paramC), b_+172);
  CYC(b_+172, b_+174); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+174, b_+175); A = B;
  CYC(b_+175, b_+176); mem_wr(gb, DE, A);
  CYC(b_+176, b_+178); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+178, b_+179); A = C;
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);
  CALL_C(b_+180, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+183); // [state] = $0b
  CYC(b_+183, b_+184); L = alu_inc8(gb, L);
  CYC(b_+184, b_+185); alu_xor(gb, A);
  CYC(b_+185, b_+186); mem_wr(gb, HL, A); // [substate] = 0
  CYC(b_+186, b_+188); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+188, b_+189); mem_wr(gb, HL, A); // [var30] = 0
  CYC(b_+189, b_+191); A = 0x3c;
  CYC(b_+191, b_+193); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+193, b_+194); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+194, b_+196); A = alu_sra(gb, A);
  CYC(b_+196, b_+197); mem_wr(gb, HL, A); // [counter2] = 30
  CYC(b_+197, b_+199); A = 0x06;
  CALL_C(b_+199, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+202);
  CYC(b_+202, b_+204); A = 0x06;
  CYC(b_+204, b_+207);
  enemySetAnimation_hook(gb); return; // jp

// Hit a wall; pause before resuming
substate2:
  CALL_C(b_+207, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+210);
  CALL_C(b_+210, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+213);
  if (!(F & FZ)) { RET_TAKEN(b_+213); return; } // ret nz
  CYC(b_+213, b_+214);
  CYC(b_+214, b_+216); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+216, b_+217); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+217, SYM(subterror_stateB));
  subterror_stateA_resetUndergroundMovement_hook(gb); return; // jp
}

void subterror_stateA_resetUndergroundMovement_hook(GB *gb) {
  BASE(subterror_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Adjust angle toward Link?
  CALL_C(b_+17, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+20);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+27); alu_cp(gb, C);
  CYC(b_+27, b_+28); A = C;
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto haveAngle; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); alu_add(gb, 0x08);
  CYC(b_+32, b_+34); alu_and(gb, 0x1f);

haveAngle:
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+37, b_+39); A = 0x1e;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CALL_C(b_+40, subterror_getAngerLevel_hook, SYM(subterror_getAngerLevel), b_+43);
  CYC(b_+43, b_+46); SET_HL(SYM(subterror_speedVals)); // subterror_speedVals
  CYC(b_+46, b_+47); subterror_addAToHl_from_rst(gb, b_+47);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+53); A = 0x0a;
  CALL_C(b_+53, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+56);
  CYC(b_+56, b_+59);
  subterror_spawnDirt_hook(gb); return; // jp
}

// Drilling
void subterror_stateB_hook(GB *gb) {
  BASE(subterror_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = subterror_jump_table(gb);
    if (target == b_+8) goto substate0;
    if (target == b_+49) goto substate1;
    HANDOFF(target);
  }

substate0:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+13, b_+15); goto drilling; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } // ret nz
  CYC(b_+16, b_+17);

  // Just started drilling
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x5f); // ENEMYCOLLISION_SUBTERROR_DRILLING
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+25, b_+27); A = 0xac; // SND_SHOCK
  CALL_C(b_+27, playSound_b00_hook, SYM(playSound_b00), b_+30);

drilling:
  CALL_C(b_+30, enemyAnimate_hook, SYM(enemyAnimate), b_+33);
  CALL_C(b_+33, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x3c);
  CYC(b_+41, b_+43); A = 0x07;
  CALL_C(b_+43, enemySetAnimation_hook, SYM(enemySetAnimation), b_+46);
  CYC(b_+46, b_+49);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate1:
  CALL_C(b_+49, subterror_retFromCallerIfAnimationUnfinished_hook, SYM(subterror_retFromCallerIfAnimationUnfinished), b_+52);
  CALL_C(b_+52, subterror_beginUndergroundMovement_hook, SYM(subterror_beginUndergroundMovement), b_+55);
  CYC(b_+55, b_+57); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+57, b_+58); alu_xor(gb, A);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  RET(b_+59); return; // ret
}

// Popping out of ground after shovel was used
void subterror_stateC_hook(GB *gb) {
  BASE(subterror_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = subterror_jump_table(gb);
    if (target == b_+10) goto substate0;
    if (target == b_+45) goto substate1;
    if (target == b_+89) goto substate2;
    HANDOFF(target);
  }

substate0:
  CALL_C(b_+10, ecom_applyVelocityForSideviewEnemy_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0f), b_+13);
  CYC(b_+13, b_+15); C = 0x10;
  CALL_C(b_+15, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+18);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A); // [var2a] = 0
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+24, b_+26); A = 0x44; // ENEMYCOLLISION_STANDARD_MINIBOSS
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+30);
  if (F & FZ) { CYCT(b_+30, b_+32); goto popSlowly; } // jr z
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0xb4);
  CYC(b_+36, b_+39);
  ecom_incSubstate_b0f_hook(gb); return; // jp

popSlowly:
  CYC(b_+39, b_+42); SET_BC(hOamFunc);
  CYC(b_+42, b_+45);
  objectSetSpeedZ_hook(gb); return; // jp

substate1:
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+49, b_+51); goto pickNewDirection; } // jr nz
  CYC(b_+49, b_+51);
  CALL_C(b_+51, enemyAnimate_hook, SYM(enemyAnimate), b_+54);
  CALL_C(b_+54, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+57);
  if (!(F & FZ)) { RET_TAKEN(b_+57); return; } // ret nz
  CYC(b_+57, b_+58);

pickNewDirection:
  CALL_C(b_+58, ecom_incSubstate_b0f_hook, SYM(ecom_incSubstate_b0f), b_+61);
  CALL_C(b_+61, getRandomNumber_hook, SYM(getRandomNumber), b_+64);
  CYC(b_+64, b_+66); alu_and(gb, 0x1c);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(b_+73, getRandomNumber_hook, SYM(getRandomNumber), b_+76);
  CYC(b_+76, b_+78); alu_and(gb, 0x03);
  CYC(b_+78, b_+81); SET_HL(SYM(subterror_durationAboveGround)); // subterror_durationAboveGround
  CYC(b_+81, b_+82); subterror_addAToHl_from_rst(gb, b_+82);
  CYC(b_+82, b_+83); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+83, b_+85); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+89);
  subterror_setAnimationFromAngle_hook(gb); return; // jp

substate2:
  CALL_C(b_+89, enemyAnimate_hook, SYM(enemyAnimate), b_+92);
  CYC(b_+92, b_+94); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CYC(b_+95, b_+96); alu_or(gb, A);
  CYC(b_+96, b_+98); A = 0xa3; // SND_LAND
  if (!(F & FZ)) { CALL_C_CC(b_+98, playSound_b00_hook, SYM(playSound_b00), b_+101); } else { CYC(b_+98, b_+101); } // call nz
  CALL_C(b_+101, objectApplySpeed_hook, SYM(objectApplySpeed), b_+104);
  CALL_C(b_+104, ecom_bounceOffWallsAndHoles_b0f_hook, SYM(ecom_bounceOffWallsAndHoles_b0f), b_+107);
  if (!(F & FZ)) { CALL_C_CC(b_+107, subterror_setAnimationFromAngle_hook, SYM(subterror_setAnimationFromAngle), b_+110); } else { CYC(b_+107, b_+110); } // call nz

  // Dig back into ground when [counter1] reaches 0
  CALL_C(b_+110, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+113);
  if (!(F & FZ)) { RET_TAKEN(b_+113); return; } // ret nz
  CYC(b_+113, b_+114);
  CYC(b_+114, SYM(subterror_spawnDirtEvery8Frames));
  subterror_digIntoGround_hook(gb); return; // jp
}

void subterror_spawnDirtEvery8Frames_hook(GB *gb) {
  BASE(subterror_spawnDirtEvery8Frames);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE); // [var31]
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, SYM(subterror_spawnDirt));
  subterror_spawnDirt_hook(gb); return; // fallthrough
}

void subterror_spawnDirt_hook(GB *gb) {
  BASE(subterror_spawnDirt);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+2, b_+4); A = 0x07;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A); // [var31] = 7
  CYC(b_+5, b_+6); E = alu_dec8(gb, E);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A); // [var30] = 7
  CYC(b_+7, b_+9); B = 0x32; // PART_SUBTERROR_DIRT
  CALL_C(b_+9, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+12);
  CALL_C(b_+12, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+15);
  CYC(b_+15, b_+16); C = L;
  CYC(b_+16, b_+18); A = 0xef;
  CYC(b_+18, SYM(subterror_retFromCallerIfAnimationUnfinished));
  setTile_hook(gb); return; // jp
}

void subterror_retFromCallerIfAnimationUnfinished_hook(GB *gb) {
  BASE(subterror_retFromCallerIfAnimationUnfinished);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  SET_AF(POP(b_+9)); // pop af -- discard caller's own return address
  RET(b_+10); return; // ret
}

// @param[out] a Anger level (0-2)
void subterror_getAngerLevel_hook(GB *gb) {
  BASE(subterror_getAngerLevel);
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

void subterror_setAnimationFromAngle_hook(GB *gb) {
  BASE(subterror_setAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+4, b_+5); alu_add(gb, A); // add a
  CYC(b_+5, b_+7); A = alu_swap(gb, A);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); // [direction]
  CYC(b_+10, b_+12); alu_add(gb, 0x00);
  CYC(b_+12, SYM(subterror_speedVals));
  enemySetAnimation_hook(gb); return; // jp
}
