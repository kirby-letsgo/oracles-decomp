#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  (void)sp0_;
  CYC(0x7066, 0x7067); E = A;
  CYC(0x7067, 0x706a); SET_HL(0x7074); // @floorColors
  CALL_C(0x706a, lookupKey_hook, 0x1e06, 0x706d);
  CYC(0x706d, 0x706e); H = D;
  CYC(0x706e, 0x7070); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  if (F & FC) { RET_TAKEN(0x7070); return; } // ret c
  CYC(0x7070, 0x7071);
  CYC(0x7071, 0x7073); A = 0x02;
  RET(0x7073); return; // ret
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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f2d, ecom_checkHazards_b0e_hook, 0x4051, 0x6f30);
  if (F & FZ) { CYCT(0x6f30, 0x6f32); goto normalStatus; } // jr z
  CYC(0x6f30, 0x6f32);
  CYC(0x6f32, 0x6f34); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6f34); return; } // ret c
  CYC(0x6f34, 0x6f35);
  if (F & FZ) { CYCT(0x6f35, 0x6f38); enemyDie_hook(gb); return; } // jp z
  CYC(0x6f35, 0x6f38);
  CYC(0x6f38, 0x6f39); H = D;
  CYC(0x6f39, 0x6f3b); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6f3b, 0x6f3c); A = mem_rd(gb, HL);
  CYC(0x6f3c, 0x6f3e); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (!(F & FZ)) { CYCT(0x6f3e, 0x6f40); goto attacked; } // jr nz
  CYC(0x6f3e, 0x6f40);
  CALL_C(0x6f40, colorChangingGel_chooseRandomColor_hook, 0x7081, 0x6f43);
  CYC(0x6f43, 0x6f45); goto normalStatus; // jr

attacked:
  CYC(0x6f45, 0x6f47); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6f47, 0x6f48); A = mem_rd(gb, DE);
  CYC(0x6f48, 0x6f4a); alu_cp(gb, 0x6e); // ENEMYCOLLISION_COLOR_CHANGING_GEL
  if (!(F & FZ)) { CYCT(0x6f4a, 0x6f4c); goto normalStatus; } // jr nz
  CYC(0x6f4a, 0x6f4c);
  CYC(0x6f4c, 0x6f4d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6f4d, 0x6f4f); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x6f4f, 0x6f51); alu_cp(gb, 0x0d); // ITEMCOLLISION_SWITCH_HOOK
  if (F & FZ) { CYCT(0x6f51, 0x6f53); goto wasDamagingAttack; } // jr z
  CYC(0x6f51, 0x6f53);
  CYC(0x6f53, 0x6f55); alu_sub(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  CYC(0x6f55, 0x6f57); alu_cp(gb, 0x06); // ITEMCOLLISION_SWORD_HELD-ITEMCOLLISION_L1_SWORD+1
  if (!(F & FC)) { CYCT(0x6f57, 0x6f59); goto normalStatus; } // jr nc
  CYC(0x6f57, 0x6f59);

wasDamagingAttack:
  CYC(0x6f59, 0x6f5b); mem_wr(gb, HL, 0xf4); // [invincibilityCounter] = $f4
  CYC(0x6f5b, 0x6f5d); A = 0x4e; // SND_DAMAGE_ENEMY
  CALL_C(0x6f5d, playSound_b00_hook, 0x0c98, 0x6f60);

normalStatus:
  CALL_C(0x6f60, colorChangingGel_updateColor_hook, 0x702c, 0x6f63);
  CYC(0x6f63, 0x6f65); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6f65, 0x6f66); A = mem_rd(gb, DE);
  {
    CYC(0x6f66, 0x6f67); push_effect(gb, 0x6f67);
    uint16_t target = colorChangingGel_jump_table(gb);
    if (target == 0x6f7d) { colorChangingGel_state_uninitialized_hook(gb); return; }
    if (target == 0x6fa1) { colorChangingGel_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x6fa2) { colorChangingGel_state8_hook(gb); return; }
    if (target == 0x6fe8) { colorChangingGel_state9_hook(gb); return; }
    if (target == 0x7000) { colorChangingGel_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6f7d, bare global; jump-table target from enemyCode47.
void colorChangingGel_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f7d, 0x6f7f); A = 0x32; // SPEED_140
  CALL_C(0x6f7f, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x6f82);
  CYC(0x6f82, 0x6f84); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6f84, 0x6f86); mem_wr(gb, HL, 0x96);
  CYC(0x6f86, 0x6f87); L = alu_inc8(gb, L);
  CYC(0x6f87, 0x6f89); mem_wr(gb, HL, 0x00); // [counter2] = 0
  CYC(0x6f89, 0x6f8b); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6f8b, 0x6f8d); mem_wr(gb, HL, 0x6e); // ENEMYCOLLISION_COLOR_CHANGING_GEL
  CYC(0x6f8d, 0x6f8f); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x6f8f, 0x6f91); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CALL_C(0x6f91, objectGetTileAtPosition_hook, 0x1444, 0x6f94);
  CYC(0x6f94, 0x6f96); E = ENEMY_BASE + OBJ_VAR32;
  CYC(0x6f96, 0x6f97); mem_wr(gb, DE, A);
  CYC(0x6f97, 0x6f99); A = 0xbf; // PALH_bf
  CALL_C(0x6f99, loadPaletteHeader_hook, 0x050b, 0x6f9c);
  CYC(0x6f9c, 0x6f9e); A = 0x03;
  CYC(0x6f9e, 0x6fa1); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:6fa1, bare global; jump-table target from enemyCode47.
void colorChangingGel_state_stub_hook(GB *gb) {
  RET(0x6fa1); return; // ret
}

// 0e:6fa2, bare global; jump-table target from enemyCode47. Standing still for
// [counter1] frames, then choosing a nearby tile to hop to.
void colorChangingGel_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6fa2, ecom_decCounter1_b0e_hook, 0x439a, 0x6fa5);
  if (!(F & FZ)) { RET_TAKEN(0x6fa5); return; } // ret nz
  CYC(0x6fa5, 0x6fa6);
  CYC(0x6fa6, 0x6fa7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CALL_C(0x6fa7, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6faa);
  CYC(0x6faa, 0x6fac); alu_and(gb, 0x0e);
  CYC(0x6fac, 0x6faf); SET_HL(0x6fd8); // @directionsToJump
  CYC(0x6faf, 0x6fb0); colorChangingGel_addAToHl_from_rst(gb, 0x6fb0);
  CYC(0x6fb0, 0x6fb2); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6fb2, 0x6fb3); A = mem_rd(gb, DE);
  CYC(0x6fb3, 0x6fb4); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6fb4, 0x6fb6); E = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(0x6fb6, 0x6fb7); mem_wr(gb, DE, A);
  CYC(0x6fb7, 0x6fb8); B = A;
  CYC(0x6fb8, 0x6fba); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6fba, 0x6fbb); A = mem_rd(gb, DE);
  CYC(0x6fbb, 0x6fbc); SET_HL(HL + 1); // inc hl
  CYC(0x6fbc, 0x6fbd); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6fbd, 0x6fbf); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(0x6fbf, 0x6fc0); mem_wr(gb, DE, A);
  CYC(0x6fc0, 0x6fc1); C = A;
  CALL_C(0x6fc1, getTileCollisionsAtPosition_hook, 0x14b7, 0x6fc4);
  if (!(F & FZ)) { RET_TAKEN(0x6fc4); return; } // ret nz
  CYC(0x6fc4, 0x6fc5);
  CALL_C(0x6fc5, ecom_incState_b0e_hook, 0x4000, 0x6fc8);
  CYC(0x6fc8, 0x6fca); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6fca, 0x6fcc); mem_wr(gb, HL, 0x3c);
  CYC(0x6fcc, 0x6fce); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6fce, 0x6fd0); A = 0x80;
  CYC(0x6fd0, 0x6fd1); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6fd1, 0x6fd3); mem_wr(gb, HL, 0xfe); // speedZ = -$180
  CYC(0x6fd3, 0x6fd5); A = 0x02;
  CYC(0x6fd5, 0x6fd8); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:6fe8, bare global; jump-table target from enemyCode47. Waiting [counter1]
// frames before hopping to the target position.
void colorChangingGel_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6fe8, ecom_decCounter1_b0e_hook, 0x439a, 0x6feb);
  if (!(F & FZ)) { CYCT(0x6feb, 0x6fee); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x6feb, 0x6fee);
  CYC(0x6fee, 0x6fef); L = E;
  CYC(0x6fef, 0x6ff0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x6ff0, 0x6ff1); H = D;
  CYC(0x6ff1, 0x6ff3); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CALL_C(0x6ff3, ecom_readPositionVars_b0e_hook, 0x4439, 0x6ff6);
  CALL_C(0x6ff6, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x6ff9);
  CYC(0x6ff9, 0x6ffb); alu_and(gb, 0x10);
  CYC(0x6ffb, 0x6ffd); A = alu_swap(gb, A);
  CYC(0x6ffd, 0x7000); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:7000, bare global; jump-table target from enemyCode47. Hopping to the target
// position.
void colorChangingGel_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7000, 0x7002); C = 0x30;
  CALL_C(0x7002, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7005);
  if (!(F & FZ)) { CYCT(0x7005, 0x7007); goto stillInAir; } // jr nz
  CYC(0x7005, 0x7007);
  CYC(0x7007, 0x7009); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7009, 0x700b); mem_wr(gb, HL, 0x08);
  CYC(0x700b, 0x700d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x700d, 0x700f); mem_wr(gb, HL, 0x96);
  CALL_C(0x700f, objectCenterOnTile_hook, 0x20db, 0x7012);
  CYC(0x7012, 0x7014); A = 0x03;
  CYC(0x7014, 0x7017); enemySetAnimation_hook(gb); return; // jp $282b

stillInAir:
  CYC(0x7017, 0x7019); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CALL_C(0x7019, ecom_readPositionVars_b0e_hook, 0x4439, 0x701c);
  CYC(0x701c, 0x701d); alu_sub(gb, C);
  CYC(0x701d, 0x701e); A = alu_inc8(gb, A);
  CYC(0x701e, 0x7020); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x7020, 0x7022); goto notThereYetX; } // jr nc
  CYC(0x7020, 0x7022);
  CYC(0x7022, 0x7024); A = mem_rd(gb, 0xff8f); // hEnemyStepsToZeroPositionY or similar
  CYC(0x7024, 0x7025); alu_sub(gb, B);
  CYC(0x7025, 0x7026); A = alu_inc8(gb, A);
  CYC(0x7026, 0x7028); alu_cp(gb, 0x03);
  if (F & FC) { RET_TAKEN(0x7028); return; } // ret c
  CYC(0x7028, 0x7029);

notThereYetX:
  CYC(0x7029, 0x702c); ecom_moveTowardPosition_b0e_hook(gb); return; // jp
}

// 0e:702c, bare global; called from enemyCode47. Updates the gel's color with
// intentional lag. Every 90 frames, this uses the value of var32 (tile index) to
// set the gel's color, then it updates the value of var32. Due to the order this
// is done in, it takes 180 frames for the color to update fully.
void colorChangingGel_updateColor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x702c, 0x702e); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x702e, 0x702f); A = mem_rd(gb, DE);
  CYC(0x702f, 0x7030); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x7030); return; } // ret c
  CYC(0x7030, 0x7031);
  CALL_C(0x7031, ecom_decCounter2_b0e_hook, 0x43a3, 0x7034);
  if (F & FZ) { CYCT(0x7034, 0x7036); goto updateStoredColor; } // jr z
  CYC(0x7034, 0x7036);
  CYC(0x7036, 0x7037); A = mem_rd(gb, HL);
  CYC(0x7037, 0x7038); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7038, 0x703a); goto updateColor; } // jr z
  CYC(0x7038, 0x703a);
  SET_BC(POP(0x703a));
  CYC(0x703b, 0x703d); goto updateImmunity; // jr

updateColor:
  CYC(0x703d, 0x703f); E = ENEMY_BASE + OBJ_VAR32;
  CYC(0x703f, 0x7040); A = mem_rd(gb, DE);
  CYC(0x7040, 0x7043); push_effect(gb, 0x7043); colorChangingGel_lookupFloorColor_hook(gb, sp0_);
  CYC(0x7043, 0x7044); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- [oamFlagsBackup]
  CYC(0x7044, 0x7045); mem_wr(gb, HL, A); // [oamFlags]

updateStoredColor:
  CYC(0x7045, 0x7048); push_effect(gb, 0x7048); goto updateImmunity;

updateImmunity:
  CALL_C(0x7052, objectGetTileAtPosition_hook, 0x1444, 0x7055);
  CYC(0x7055, 0x7057); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (F & FZ) { RET_TAKEN(0x7057); return; } // ret z
  CYC(0x7057, 0x7058);
  CYC(0x7058, 0x705b); push_effect(gb, 0x705b); colorChangingGel_lookupFloorColor_hook(gb, sp0_);
  CYC(0x705b, 0x705c); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x705c, 0x705e); B = 0x6e; // ENEMYCOLLISION_COLOR_CHANGING_GEL
  if (F & FZ) { CYCT(0x705e, 0x7060); goto storeCollisionMode; } // jr z
  CYC(0x705e, 0x7060);
  CYC(0x7060, 0x7062); B = 0x35; // ENEMYCOLLISION_GOHMA_GEL

storeCollisionMode:
  CYC(0x7062, 0x7064); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x7064, 0x7065); mem_wr(gb, HL, B);
  RET(0x7065); return; // ret
}

// 0e:7081, bare global; called from enemyCode47. Sets the gel's color to something
// random that isn't its current color.
void colorChangingGel_chooseRandomColor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7081, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7084);
  CYC(0x7084, 0x7086); alu_and(gb, 0x01);
  CYC(0x7086, 0x7087); B = A;
  CYC(0x7087, 0x7089); E = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7089, 0x708a); A = mem_rd(gb, DE);
  CYC(0x708a, 0x708c); A = (uint8_t)(A & ~(1 << 0)); // res 0,a
  CYC(0x708c, 0x708d); alu_add(gb, B);
  CYC(0x708d, 0x7090); SET_HL(0x7096); // @oamFlagMap
  CYC(0x7090, 0x7091); colorChangingGel_addAToHl_from_rst(gb, 0x7091);
  CYC(0x7091, 0x7092); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7092, 0x7093); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(0x7093, 0x7094); E = alu_inc8(gb, E);
  CYC(0x7094, 0x7095); mem_wr(gb, DE, A); // [oamFlags]
  RET(0x7095); return; // ret
}
