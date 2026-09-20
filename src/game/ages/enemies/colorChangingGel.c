#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(colorChangingGel_updateColor), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(colorChangingGel_updateColor), (from), (to), true)

void enemyCode47_hook(GB *gb);
void colorChangingGel_state_uninitialized_hook(GB *gb);
void colorChangingGel_state_stub_hook(GB *gb);
void colorChangingGel_state8_hook(GB *gb);
void colorChangingGel_state9_hook(GB *gb);
void colorChangingGel_stateA_hook(GB *gb);
void colorChangingGel_updateColor_hook(GB *gb);
void colorChangingGel_chooseRandomColor_hook(GB *gb);

static uint16_t colorChangingGel_jump_table(GB *gb) {
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

static void colorChangingGel_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// 0e:7066, local to colorChangingGel_updateColor; called from two sites within it.
// @param a Tile index
// @param[out] a Color (defaults to red if floor tile not listed)
// @param[out] hl &Enemy.oamFlagsBackup
static void colorChangingGel_lookupFloorColor_hook(GB *gb, uint16_t sp0_) {
  BASE(colorChangingGel_updateColor);
  (void)sp0_;
  CYC(b_+58, b_+59); E = A;
  CYC(b_+59, b_+62); SET_HL(b_+72); // @floorColors
  CALL_C(b_+62, lookupKey_hook, SYM(lookupKey), b_+65);
  CYC(b_+65, b_+66); H = D;
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  if (F & FC) { RET_TAKEN(b_+68); return; } // ret c
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); A = 0x02;
  RET(b_+71); return; // ret
}

// ==================================================================================================
// ENEMY_COLOR_CHANGING_GEL
//
// Variables:
//   var30/var31: Target position while hopping
//   var32: Tile index at current position (purposely outdated so there's lag in updating
//          the color)
// ==================================================================================================
void enemyCode47_hook(GB *gb) {
  BASE(enemyCode47);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto attacked; } // jr nz
  CYC(b_+17, b_+19);
  CALL_C(b_+19, colorChangingGel_chooseRandomColor_hook, SYM(colorChangingGel_chooseRandomColor), b_+22);
  CYC(b_+22, b_+24); goto normalStatus; // jr

attacked:
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); alu_cp(gb, 0x6e); // ENEMYCOLLISION_COLOR_CHANGING_GEL
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto normalStatus; } // jr nz
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+32, b_+34); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+34, b_+36); alu_cp(gb, 0x0d); // ITEMCOLLISION_SWITCH_HOOK
  if (F & FZ) { CYCT(b_+36, b_+38); goto wasDamagingAttack; } // jr z
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); alu_sub(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  CYC(b_+40, b_+42); alu_cp(gb, 0x06); // ITEMCOLLISION_SWORD_HELD-ITEMCOLLISION_L1_SWORD+1
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto normalStatus; } // jr nc
  CYC(b_+42, b_+44);

wasDamagingAttack:
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0xf4); // [invincibilityCounter] = $f4
  CYC(b_+46, b_+48); A = 0x4e; // SND_DAMAGE_ENEMY
  CALL_C(b_+48, playSound_b00_hook, SYM(playSound_b00), b_+51);

normalStatus:
  CALL_C(b_+51, colorChangingGel_updateColor_hook, SYM(colorChangingGel_updateColor), b_+54);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  {
    CYC(b_+57, b_+58); push_effect(gb, b_+58);
    uint16_t target = colorChangingGel_jump_table(gb);
    if (target == SYM(colorChangingGel_state_uninitialized)) { colorChangingGel_state_uninitialized_hook(gb); return; }
    if (target == SYM(colorChangingGel_state_stub)) { colorChangingGel_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(colorChangingGel_state8)) { colorChangingGel_state8_hook(gb); return; }
    if (target == SYM(colorChangingGel_state9)) { colorChangingGel_state9_hook(gb); return; }
    if (target == SYM(colorChangingGel_stateA)) { colorChangingGel_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6f7d, bare global; jump-table target from enemyCode47.
void colorChangingGel_state_uninitialized_hook(GB *gb) {
  BASE(colorChangingGel_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x32; // SPEED_140
  CALL_C(b_+2, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x96);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00); // [counter2] = 0
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x6e); // ENEMYCOLLISION_COLOR_CHANGING_GEL
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CALL_C(b_+20, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28); A = 0xbf; // PALH_bf
  CALL_C(b_+28, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+31);
  CYC(b_+31, b_+33); A = 0x03;
  CYC(b_+33, SYM(colorChangingGel_state_stub)); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:6fa1, bare global; jump-table target from enemyCode47.
void colorChangingGel_state_stub_hook(GB *gb) {
  BASE(colorChangingGel_state_stub);
  RET(b_+0); return; // ret
}

// 0e:6fa2, bare global; jump-table target from enemyCode47. Standing still for
// [counter1] frames, then choosing a nearby tile to hop to.
void colorChangingGel_state8_hook(GB *gb) {
  BASE(colorChangingGel_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CALL_C(b_+5, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x0e);
  CYC(b_+10, b_+13); SET_HL(b_+54); // @directionsToJump
  CYC(b_+13, b_+14); colorChangingGel_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); SET_HL(HL + 1); // inc hl
  CYC(b_+26, b_+27); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); C = A;
  CALL_C(b_+31, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+34);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; } // ret nz
  CYC(b_+34, b_+35);
  CALL_C(b_+35, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+38);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x3c);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+44, b_+46); A = 0x80;
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0xfe); // speedZ = -$180
  CYC(b_+49, b_+51); A = 0x02;
  CYC(b_+51, b_+54); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:6fe8, bare global; jump-table target from enemyCode47. Waiting [counter1]
// frames before hopping to the target position.
void colorChangingGel_state9_hook(GB *gb) {
  BASE(colorChangingGel_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CALL_C(b_+11, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+14);
  CALL_C(b_+14, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x10);
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, SYM(colorChangingGel_stateA)); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:7000, bare global; jump-table target from enemyCode47. Hopping to the target
// position.
void colorChangingGel_stateA_hook(GB *gb) {
  BASE(colorChangingGel_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto stillInAir; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x08);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x96);
  CALL_C(b_+15, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+18);
  CYC(b_+18, b_+20); A = 0x03;
  CYC(b_+20, b_+23); enemySetAnimation_hook(gb); return; // jp $282b

stillInAir:
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CALL_C(b_+25, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+28);
  CYC(b_+28, b_+29); alu_sub(gb, C);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+32); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+32, b_+34); goto notThereYetX; } // jr nc
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = mem_rd(gb, hFF8F); // hEnemyStepsToZeroPositionY or similar
  CYC(b_+36, b_+37); alu_sub(gb, B);
  CYC(b_+37, b_+38); A = alu_inc8(gb, A);
  CYC(b_+38, b_+40); alu_cp(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+40); return; } // ret c
  CYC(b_+40, b_+41);

notThereYetX:
  CYC(b_+41, SYM(colorChangingGel_updateColor)); ecom_moveTowardPosition_b0e_hook(gb); return; // jp
}

// 0e:702c, bare global; called from enemyCode47. Updates the gel's color with
// intentional lag. Every 90 frames, this uses the value of var32 (tile index) to
// set the gel's color, then it updates the value of var32. Due to the order this
// is done in, it takes 180 frames for the color to update fully.
void colorChangingGel_updateColor_hook(GB *gb) {
  BASE(colorChangingGel_updateColor);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CALL_C(b_+5, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto updateStoredColor; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+14); goto updateColor; } // jr z
  CYC(b_+12, b_+14);
  SET_BC(POP(b_+14));
  CYC(b_+15, b_+17); goto updateImmunity; // jr

updateColor:
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+23); push_effect(gb, b_+23); colorChangingGel_lookupFloorColor_hook(gb, sp0_);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- [oamFlagsBackup]
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); // [oamFlags]

updateStoredColor:
  CYC(b_+25, b_+28); push_effect(gb, b_+28); goto updateImmunity;

updateImmunity:
  CALL_C(b_+38, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+41);
  CYC(b_+41, b_+43); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (F & FZ) { RET_TAKEN(b_+43); return; } // ret z
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+47); push_effect(gb, b_+47); colorChangingGel_lookupFloorColor_hook(gb, sp0_);
  CYC(b_+47, b_+48); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+50); B = 0x6e; // ENEMYCOLLISION_COLOR_CHANGING_GEL
  if (F & FZ) { CYCT(b_+50, b_+52); goto storeCollisionMode; } // jr z
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); B = 0x35; // ENEMYCOLLISION_GOHMA_GEL

storeCollisionMode:
  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+56, b_+57); mem_wr(gb, HL, B);
  RET(b_+57); return; // ret
}

// 0e:7081, bare global; called from enemyCode47. Sets the gel's color to something
// random that isn't its current color.
void colorChangingGel_chooseRandomColor_hook(GB *gb) {
  BASE(colorChangingGel_chooseRandomColor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); A = (uint8_t)(A & ~(1 << 0)); // res 0,a
  CYC(b_+11, b_+12); alu_add(gb, B);
  CYC(b_+12, b_+15); SET_HL(b_+21); // @oamFlagMap
  CYC(b_+15, b_+16); colorChangingGel_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+17, b_+18); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(b_+18, b_+19); E = alu_inc8(gb, E);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A); // [oamFlags]
  RET(b_+20); return; // ret
}
