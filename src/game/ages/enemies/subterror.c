#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x49ac, 0x49ae); goto normalStatus; } // jr z
  CYC(0x49ac, 0x49ae);
  CYC(0x49ae, 0x49b0); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x49b0); return; } // ret c
  CYC(0x49b0, 0x49b1);
  if (!(F & FZ)) { CYCT(0x49b1, 0x49b3); goto normalStatus; } // jr nz
  CYC(0x49b1, 0x49b3);
  CYC(0x49b3, 0x49b6);
  enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(0x49b6, 0x49b8); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x49b8, 0x49b9); A = mem_rd(gb, DE);
  CYC(0x49b9, 0x49ba); alu_or(gb, A);
  if (!(F & FZ)) { CALL_C_CC(0x49ba, subterror_spawnDirtEvery8Frames_hook, 0x4c40, 0x49bd); } else { CYC(0x49ba, 0x49bd); } // call nz
  CYC(0x49bd, 0x49bf); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x49bf, 0x49c0); A = mem_rd(gb, DE);
  {
    CYC(0x49c0, 0x49c1); push_effect(gb, 0x49c1);
    uint16_t target = subterror_jump_table(gb);
    if (target == 0x49db) { subterror_state_uninitialized_hook(gb); return; }
    if (target == 0x49f8) { subterror_state_stub_hook(gb); return; }
    if (target == 0x49f9) { subterror_state8_hook(gb); return; }
    if (target == 0x4a85) { subterror_state9_hook(gb); return; }
    if (target == 0x4ab3) { subterror_stateA_hook(gb); return; }
    if (target == 0x4b8f) { subterror_stateB_hook(gb); return; }
    if (target == 0x4bcb) { subterror_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

void subterror_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x49db, 0x49dd); A = 0x72; // ENEMY_SUBTERROR
  CYC(0x49dd, 0x49df); B = 0xbe; // PALH_be
  CALL_C(0x49df, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x49e2);
  CALL_C(0x49e2, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x49e5);
  CYC(0x49e5, 0x49e7); A = 0x07;
  CYC(0x49e7, 0x49e9); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x49e9, 0x49ea); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x49ea, 0x49eb); mem_wr(gb, HL, A); // [var30]
  CYC(0x49eb, 0x49ed); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x49ed, 0x49ef); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x49ef, 0x49f1); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x49f1, 0x49f3); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x49f3, 0x49f5); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x49f5, 0x49f7); mem_wr(gb, HL, 0x1e);
  RET(0x49f7); return; // ret
}

void subterror_state_stub_hook(GB *gb) {
  RET(0x49f8); return; // ret
}

// Cutscene before fight
void subterror_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x49f9, 0x49fb); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x49fb, 0x49fc); A = mem_rd(gb, DE);
  {
    CYC(0x49fc, 0x49fd); push_effect(gb, 0x49fd);
    uint16_t target = subterror_jump_table(gb);
    if (target == 0x4a05) goto substate0;
    if (target == 0x4a3c) goto substate1;
    if (target == 0x4a56) goto substate2;
    if (target == 0x4a6e) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x4a05, 0x4a07); A = 0x01; // DISABLE_LINK
  CYC(0x4a07, 0x4a0a); W8(wDisabledObjects) = A;
  CYC(0x4a0a, 0x4a0d); W8(wMenuDisabled) = A;

  // Wait for door to close
  CYC(0x4a0d, 0x4a10); A = W8(wcc93);
  CYC(0x4a10, 0x4a11); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4a11); return; } // ret nz
  CYC(0x4a11, 0x4a12);
  CALL_C(0x4a12, ecom_decCounter2_b0f_hook, 0x43a3, 0x4a15);
  if (!(F & FZ)) { RET_TAKEN(0x4a15); return; } // ret nz
  CYC(0x4a15, 0x4a16);

  // Move further down
  CALL_C(0x4a16, objectApplySpeed_hook, 0x201d, 0x4a19);
  CYC(0x4a19, 0x4a1b); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4a1b, 0x4a1c); A = mem_rd(gb, DE);
  CYC(0x4a1c, 0x4a1e); alu_cp(gb, 0x58);
  if (F & FC) { RET_TAKEN(0x4a1e); return; } // ret c
  CYC(0x4a1e, 0x4a1f);

  // Reached middle of screen, about to pop out
  CYC(0x4a1f, 0x4a21); A = 0xa9; // SND_DIG
  CALL_C(0x4a21, playSound_b00_hook, 0x0c98, 0x4a24);
  CYC(0x4a24, 0x4a26); A = 0x06;
  CALL_C(0x4a26, enemySetAnimation_hook, 0x282b, 0x4a29);
  CALL_C(0x4a29, objectSetVisiblec2_hook, 0x1e45, 0x4a2c);
  CALL_C(0x4a2c, ecom_incSubstate_b0f_hook, 0x4005, 0x4a2f);

  // Disable dirt animation
  CYC(0x4a2f, 0x4a31); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4a31, 0x4a33); mem_wr(gb, HL, 0x00);
  CALL_C(0x4a33, objectGetTileAtPosition_hook, 0x1444, 0x4a36);
  CYC(0x4a36, 0x4a37); C = L;
  CYC(0x4a37, 0x4a39); A = 0x4c; // TILEINDEX_DUNGEON_DUG_DIRT
  CYC(0x4a39, 0x4a3c);
  setTile_hook(gb); return; // jp

substate1:
  CALL_C(0x4a3c, subterror_retFromCallerIfAnimationUnfinished_hook, 0x4c5a, 0x4a3f);
  CYC(0x4a3f, 0x4a41); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(0x4a41, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x4a44);
  CALL_C(0x4a44, ecom_incSubstate_b0f_hook, 0x4005, 0x4a47);
  CYC(0x4a47, 0x4a49); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4a49, 0x4a4b); mem_wr(gb, HL, 0x3c);
  CYC(0x4a4b, 0x4a4e); SET_BC(0xfe00);
  CALL_C(0x4a4e, objectSetSpeedZ_hook, 0x239d, 0x4a51);
  CYC(0x4a51, 0x4a53); A = 0x05;
  CYC(0x4a53, 0x4a56);
  enemySetAnimation_hook(gb); return; // jp

substate2:
  CYC(0x4a56, 0x4a58); C = 0x10;
  CALL_C(0x4a58, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4a5b);
  if (!(F & FZ)) { RET_TAKEN(0x4a5b); return; } // ret nz
  CYC(0x4a5b, 0x4a5c);
  CYC(0x4a5c, 0x4a5e); A = 0x02;
  CALL_C(0x4a5e, enemySetAnimation_hook, 0x282b, 0x4a61);
  CALL_C(0x4a61, ecom_decCounter1_b0f_hook, 0x439a, 0x4a64);
  if (!(F & FZ)) { RET_TAKEN(0x4a64); return; } // ret nz
  CYC(0x4a64, 0x4a65);
  CYC(0x4a65, 0x4a68); SET_BC(0x2f03); // TX_2f03
  CALL_C(0x4a68, showText_hook, 0x1872, 0x4a6b);
  CYC(0x4a6b, 0x4a6e);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate3:
  CALL_C(0x4a6e, retIfTextIsActive_hook, 0x1859, 0x4a71);
  CALL_C(0x4a71, enemyBoss_beginMiniboss_b0f_hook, 0x4580, 0x4a74);
  CYC(0x4a74, 0x4a75); alu_xor(gb, A);
  CYC(0x4a75, 0x4a78); W8(wDisabledObjects) = A;
  CYC(0x4a78, 0x4a7b); W8(wMenuDisabled) = A;
  subterror_digIntoGround_hook(gb); return; // fallthrough
}

void subterror_digIntoGround_hook(GB *gb) {
  CYC(0x4a7b, 0x4a7d); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a7d, 0x4a7f); A = 0x09;
  CYC(0x4a7f, 0x4a80); mem_wr(gb, DE, A);
  CYC(0x4a80, 0x4a82); A = 0x04;
  CYC(0x4a82, 0x4a85);
  enemySetAnimation_hook(gb); return; // jp
}

// Digging into ground
void subterror_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4a85, subterror_retFromCallerIfAnimationUnfinished_hook, 0x4c5a, 0x4a88);
  subterror_beginUndergroundMovement_hook(gb); return; // fallthrough
}

// Done digging, about to start moving around
void subterror_beginUndergroundMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4a88, 0x4a89); H = D;
  CYC(0x4a89, 0x4a8b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a8b, 0x4a8d); mem_wr(gb, HL, 0x0a);
  CYC(0x4a8d, 0x4a8e); L = alu_inc8(gb, L);
  CYC(0x4a8e, 0x4a8f); alu_xor(gb, A);
  CYC(0x4a8f, 0x4a90); mem_wr(gb, HL, A); // [substate]
  CYC(0x4a90, 0x4a91); A = alu_dec8(gb, A);
  CYC(0x4a91, 0x4a93); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4a93, 0x4a94); mem_wr(gb, HL, A); // [angle] = $ff
  CYC(0x4a94, 0x4a96); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x4a96, 0x4a98); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4a98, 0x4a9a); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4a9a, 0x4a9c); mem_wr(gb, HL, 0x69); // ENEMYCOLLISION_SUBTERROR_UNDERGROUND
  CYC(0x4a9c, 0x4a9e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4a9e, 0x4aa0); mem_wr(gb, HL, 0x3c);
  CALL_C(0x4aa0, subterror_getAngerLevel_hook, 0x4c65, 0x4aa3);
  CYC(0x4aa3, 0x4aa6); SET_HL(0x4c88); // subterror_timeUntilDrillAttack
  CYC(0x4aa6, 0x4aa7); subterror_addAToHl_from_rst(gb, 0x4aa7);
  CYC(0x4aa7, 0x4aa8); A = mem_rd(gb, HL);
  CYC(0x4aa8, 0x4aaa); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x4aaa, 0x4aab); mem_wr(gb, DE, A);
  CYC(0x4aab, 0x4aad); A = 0xa9; // SND_DIG
  CALL_C(0x4aad, playSound_b00_hook, 0x0c98, 0x4ab0);
  CYC(0x4ab0, 0x4ab3);
  subterror_spawnDirt_hook(gb); return; // jp
}

// Currently in the ground, moving around
void subterror_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ab3, 0x4ab5); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4ab5, 0x4ab6); A = mem_rd(gb, DE);
  {
    CYC(0x4ab6, 0x4ab7); push_effect(gb, 0x4ab7);
    uint16_t target = subterror_jump_table(gb);
    if (target == 0x4abd) goto substate0;
    if (target == 0x4aee) goto substate1;
    if (target == 0x4b82) goto substate2;
    HANDOFF(target);
  }

// Staying underground for [counter1] frames before moving
substate0:
  CALL_C(0x4abd, ecom_decCounter1_b0f_hook, 0x439a, 0x4ac0);
  if (!(F & FZ)) { RET_TAKEN(0x4ac0); return; } // ret nz
  CYC(0x4ac0, 0x4ac1);
  CALL_C(0x4ac1, ecom_incSubstate_b0f_hook, 0x4005, 0x4ac4);
  subterror_stateA_resetUndergroundMovement_hook(gb); return; // fallthrough

// Moving around until shovel is used or he starts drilling
substate1:
  CYC(0x4aee, 0x4af0); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4af0, 0x4af1); A = mem_rd(gb, DE);
  CYC(0x4af1, 0x4af3); A = alu_sla(gb, A);
  if (!(F & FC)) { CYCT(0x4af3, 0x4af5); goto noShovel; } // jr nc
  CYC(0x4af3, 0x4af5);
  CYC(0x4af5, 0x4af7); alu_cp(gb, 0x18); // ITEMCOLLISION_SHOVEL<<1
  if (!(F & FZ)) { CYCT(0x4af7, 0x4af9); goto noShovel; } // jr nz
  CYC(0x4af7, 0x4af9);

  // Shovel was used; will now pop out of ground
  CYC(0x4af9, 0x4afc); SET_BC(0xff00);
  CALL_C(0x4afc, objectSetSpeedZ_hook, 0x239d, 0x4aff);
  CYC(0x4aff, 0x4b01); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4b01, 0x4b03); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x4b03, 0x4b05); A = 0x0c;
  CYC(0x4b05, 0x4b07); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4b07, 0x4b08); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4b08, 0x4b09); alu_xor(gb, A);
  CYC(0x4b09, 0x4b0a); mem_wr(gb, HL, A); // [substate] = 0
  CYC(0x4b0a, 0x4b0c); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b0c, 0x4b0d); mem_wr(gb, HL, A); // [var30] = 0
  CYC(0x4b0d, 0x4b0e); A = alu_inc8(gb, A);
  CYC(0x4b0e, 0x4b10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b10, 0x4b11); mem_wr(gb, HL, A); // [counter1] = 1
  CYC(0x4b11, 0x4b13); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x4b13, 0x4b15); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

  // Bounces away from Link
  CALL_C(0x4b15, objectGetAngleTowardLink_hook, 0x1e9c, 0x4b18);
  CYC(0x4b18, 0x4b1a); alu_xor(gb, 0x10);
  CYC(0x4b1a, 0x4b1c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4b1c, 0x4b1d); mem_wr(gb, DE, A);
  CYC(0x4b1d, 0x4b1f); A = 0x06;
  CALL_C(0x4b1f, objectSetCollideRadius_hook, 0x24a1, 0x4b22);
  CYC(0x4b22, 0x4b24); A = 0x05;
  CYC(0x4b24, 0x4b27);
  enemySetAnimation_hook(gb); return; // jp

noShovel:
  CALL_C(0x4b27, objectApplySpeed_hook, 0x201d, 0x4b2a);
  CYC(0x4b2a, 0x4b2c); A = 0x01;
  CALL_C(0x4b2c, ecom_getSideviewAdjacentWallsBitset_b0f_hook, 0x420b, 0x4b2f);
  if (F & FZ) { CYCT(0x4b2f, 0x4b31); goto stillUnderground; } // jr z

  // Hit wall
  CYC(0x4b2f, 0x4b31);
  CALL_C(0x4b31, ecom_incSubstate_b0f_hook, 0x4005, 0x4b34);
  CYC(0x4b34, 0x4b36); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b36, 0x4b38); mem_wr(gb, HL, 0x5a);
  CYC(0x4b38, 0x4b3a); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x4b3a, 0x4b3c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4b3c, 0x4b3e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b3e, 0x4b40); mem_wr(gb, HL, 0x00);
  RET(0x4b40); return; // ret

stillUnderground:
  CALL_C(0x4b41, ecom_decCounter1_b0f_hook, 0x439a, 0x4b44);
  if (F & FZ) {
    CYCT(0x4b44, 0x4b47); // call z
    push_effect(gb, 0x4b47);
    subterror_stateA_resetUndergroundMovement_hook(gb);
  } else {
    CYC(0x4b44, 0x4b47);
  }
  CALL_C(0x4b47, ecom_decCounter2_b0f_hook, 0x43a3, 0x4b4a);
  if (!(F & FZ)) { RET_TAKEN(0x4b4a); return; } // ret nz
  CYC(0x4b4a, 0x4b4b);

  // If Link is close enough, drill him
  CYC(0x4b4b, 0x4b4d); C = 0x18;
  CALL_C(0x4b4d, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x4b50);
  if (!(F & FC)) { RET_TAKEN(0x4b50); return; } // ret nc
  CYC(0x4b50, 0x4b51);

  // "Transport" to the tile at Link's position
  CYC(0x4b51, 0x4b54); SET_HL(w1Link_yh);
  CYC(0x4b54, 0x4b55); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4b55, 0x4b56); L = alu_inc8(gb, L);
  CYC(0x4b56, 0x4b57); C = mem_rd(gb, HL);
  CYC(0x4b57, 0x4b58); B = A;
  CALL_C(0x4b58, getTileAtPosition_hook, 0x1447, 0x4b5b);
  CYC(0x4b5b, 0x4b5c); C = L;
  CALL_C(0x4b5c, convertShortToLongPosition_paramC_hook, 0x20cc, 0x4b5f);
  CYC(0x4b5f, 0x4b61); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4b61, 0x4b62); A = B;
  CYC(0x4b62, 0x4b63); mem_wr(gb, DE, A);
  CYC(0x4b63, 0x4b65); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4b65, 0x4b66); A = C;
  CYC(0x4b66, 0x4b67); mem_wr(gb, DE, A);
  CALL_C(0x4b67, ecom_incState_b0f_hook, 0x4000, 0x4b6a); // [state] = $0b
  CYC(0x4b6a, 0x4b6b); L = alu_inc8(gb, L);
  CYC(0x4b6b, 0x4b6c); alu_xor(gb, A);
  CYC(0x4b6c, 0x4b6d); mem_wr(gb, HL, A); // [substate] = 0
  CYC(0x4b6d, 0x4b6f); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4b6f, 0x4b70); mem_wr(gb, HL, A); // [var30] = 0
  CYC(0x4b70, 0x4b72); A = 0x3c;
  CYC(0x4b72, 0x4b74); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4b74, 0x4b75); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4b75, 0x4b77); A = alu_sra(gb, A);
  CYC(0x4b77, 0x4b78); mem_wr(gb, HL, A); // [counter2] = 30
  CYC(0x4b78, 0x4b7a); A = 0x06;
  CALL_C(0x4b7a, objectSetCollideRadius_hook, 0x24a1, 0x4b7d);
  CYC(0x4b7d, 0x4b7f); A = 0x06;
  CYC(0x4b7f, 0x4b82);
  enemySetAnimation_hook(gb); return; // jp

// Hit a wall; pause before resuming
substate2:
  CALL_C(0x4b82, ecom_decCounter2_b0f_hook, 0x43a3, 0x4b85);
  CALL_C(0x4b85, ecom_decCounter1_b0f_hook, 0x439a, 0x4b88);
  if (!(F & FZ)) { RET_TAKEN(0x4b88); return; } // ret nz
  CYC(0x4b88, 0x4b89);
  CYC(0x4b89, 0x4b8b); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4b8b, 0x4b8c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4b8c, 0x4b8f);
  subterror_stateA_resetUndergroundMovement_hook(gb); return; // jp
}

void subterror_stateA_resetUndergroundMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Adjust angle toward Link?
  CALL_C(0x4ac4, objectGetAngleTowardLink_hook, 0x1e9c, 0x4ac7);
  CYC(0x4ac7, 0x4ac8); C = A;
  CYC(0x4ac8, 0x4aca); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4aca, 0x4acb); A = mem_rd(gb, DE);
  CYC(0x4acb, 0x4acd); alu_xor(gb, 0x10);
  CYC(0x4acd, 0x4ace); alu_cp(gb, C);
  CYC(0x4ace, 0x4acf); A = C;
  if (!(F & FZ)) { CYCT(0x4acf, 0x4ad1); goto haveAngle; } // jr nz
  CYC(0x4acf, 0x4ad1);
  CYC(0x4ad1, 0x4ad3); alu_add(gb, 0x08);
  CYC(0x4ad3, 0x4ad5); alu_and(gb, 0x1f);

haveAngle:
  CYC(0x4ad5, 0x4ad6); mem_wr(gb, DE, A);
  CYC(0x4ad6, 0x4ad8); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4ad8, 0x4ada); A = 0x1e;
  CYC(0x4ada, 0x4adb); mem_wr(gb, DE, A);
  CALL_C(0x4adb, subterror_getAngerLevel_hook, 0x4c65, 0x4ade);
  CYC(0x4ade, 0x4ae1); SET_HL(0x4c85); // subterror_speedVals
  CYC(0x4ae1, 0x4ae2); subterror_addAToHl_from_rst(gb, 0x4ae2);
  CYC(0x4ae2, 0x4ae3); A = mem_rd(gb, HL);
  CYC(0x4ae3, 0x4ae5); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4ae5, 0x4ae6); mem_wr(gb, DE, A);
  CYC(0x4ae6, 0x4ae8); A = 0x0a;
  CALL_C(0x4ae8, objectSetCollideRadius_hook, 0x24a1, 0x4aeb);
  CYC(0x4aeb, 0x4aee);
  subterror_spawnDirt_hook(gb); return; // jp
}

// Drilling
void subterror_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b8f, 0x4b91); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4b91, 0x4b92); A = mem_rd(gb, DE);
  {
    CYC(0x4b92, 0x4b93); push_effect(gb, 0x4b93);
    uint16_t target = subterror_jump_table(gb);
    if (target == 0x4b97) goto substate0;
    if (target == 0x4bc0) goto substate1;
    HANDOFF(target);
  }

substate0:
  CYC(0x4b97, 0x4b98); H = D;
  CYC(0x4b98, 0x4b9a); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x4b9a, 0x4b9b); A = mem_rd(gb, HL);
  CYC(0x4b9b, 0x4b9c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4b9c, 0x4b9e); goto drilling; } // jr z
  CYC(0x4b9c, 0x4b9e);
  CYC(0x4b9e, 0x4b9f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x4b9f); return; } // ret nz
  CYC(0x4b9f, 0x4ba0);

  // Just started drilling
  CYC(0x4ba0, 0x4ba2); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4ba2, 0x4ba4); mem_wr(gb, HL, 0x5f); // ENEMYCOLLISION_SUBTERROR_DRILLING
  CYC(0x4ba4, 0x4ba6); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x4ba6, 0x4ba8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4ba8, 0x4baa); A = 0xac; // SND_SHOCK
  CALL_C(0x4baa, playSound_b00_hook, 0x0c98, 0x4bad);

drilling:
  CALL_C(0x4bad, enemyAnimate_hook, 0x2818, 0x4bb0);
  CALL_C(0x4bb0, ecom_decCounter1_b0f_hook, 0x439a, 0x4bb3);
  if (!(F & FZ)) { RET_TAKEN(0x4bb3); return; } // ret nz
  CYC(0x4bb3, 0x4bb4);
  CYC(0x4bb4, 0x4bb6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4bb6, 0x4bb8); mem_wr(gb, HL, 0x3c);
  CYC(0x4bb8, 0x4bba); A = 0x07;
  CALL_C(0x4bba, enemySetAnimation_hook, 0x282b, 0x4bbd);
  CYC(0x4bbd, 0x4bc0);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate1:
  CALL_C(0x4bc0, subterror_retFromCallerIfAnimationUnfinished_hook, 0x4c5a, 0x4bc3);
  CALL_C(0x4bc3, subterror_beginUndergroundMovement_hook, 0x4a88, 0x4bc6);
  CYC(0x4bc6, 0x4bc8); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4bc8, 0x4bc9); alu_xor(gb, A);
  CYC(0x4bc9, 0x4bca); mem_wr(gb, DE, A);
  RET(0x4bca); return; // ret
}

// Popping out of ground after shovel was used
void subterror_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4bcb, 0x4bcd); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4bcd, 0x4bce); A = mem_rd(gb, DE);
  {
    CYC(0x4bce, 0x4bcf); push_effect(gb, 0x4bcf);
    uint16_t target = subterror_jump_table(gb);
    if (target == 0x4bd5) goto substate0;
    if (target == 0x4bf8) goto substate1;
    if (target == 0x4c24) goto substate2;
    HANDOFF(target);
  }

substate0:
  CALL_C(0x4bd5, ecom_applyVelocityForSideviewEnemy_b0f_hook, 0x4153, 0x4bd8);
  CYC(0x4bd8, 0x4bda); C = 0x10;
  CALL_C(0x4bda, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4bdd);
  if (!(F & FZ)) { RET_TAKEN(0x4bdd); return; } // ret nz
  CYC(0x4bdd, 0x4bde);
  CYC(0x4bde, 0x4be0); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4be0, 0x4be1); mem_wr(gb, DE, A); // [var2a] = 0
  CYC(0x4be1, 0x4be3); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x4be3, 0x4be5); A = 0x44; // ENEMYCOLLISION_STANDARD_MINIBOSS
  CYC(0x4be5, 0x4be6); mem_wr(gb, DE, A);
  CALL_C(0x4be6, ecom_decCounter1_b0f_hook, 0x439a, 0x4be9);
  if (F & FZ) { CYCT(0x4be9, 0x4beb); goto popSlowly; } // jr z
  CYC(0x4be9, 0x4beb);
  CYC(0x4beb, 0x4bed); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4bed, 0x4bef); mem_wr(gb, HL, 0xb4);
  CYC(0x4bef, 0x4bf2);
  ecom_incSubstate_b0f_hook(gb); return; // jp

popSlowly:
  CYC(0x4bf2, 0x4bf5); SET_BC(0xff80);
  CYC(0x4bf5, 0x4bf8);
  objectSetSpeedZ_hook(gb); return; // jp

substate1:
  CYC(0x4bf8, 0x4bfa); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4bfa, 0x4bfb); A = mem_rd(gb, DE);
  CYC(0x4bfb, 0x4bfc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4bfc, 0x4bfe); goto pickNewDirection; } // jr nz
  CYC(0x4bfc, 0x4bfe);
  CALL_C(0x4bfe, enemyAnimate_hook, 0x2818, 0x4c01);
  CALL_C(0x4c01, ecom_decCounter1_b0f_hook, 0x439a, 0x4c04);
  if (!(F & FZ)) { RET_TAKEN(0x4c04); return; } // ret nz
  CYC(0x4c04, 0x4c05);

pickNewDirection:
  CALL_C(0x4c05, ecom_incSubstate_b0f_hook, 0x4005, 0x4c08);
  CALL_C(0x4c08, getRandomNumber_hook, 0x043e, 0x4c0b);
  CYC(0x4c0b, 0x4c0d); alu_and(gb, 0x1c);
  CYC(0x4c0d, 0x4c0f); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4c0f, 0x4c10); mem_wr(gb, HL, A);
  CYC(0x4c10, 0x4c12); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4c12, 0x4c14); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(0x4c14, getRandomNumber_hook, 0x043e, 0x4c17);
  CYC(0x4c17, 0x4c19); alu_and(gb, 0x03);
  CYC(0x4c19, 0x4c1c); SET_HL(0x4c8b); // subterror_durationAboveGround
  CYC(0x4c1c, 0x4c1d); subterror_addAToHl_from_rst(gb, 0x4c1d);
  CYC(0x4c1d, 0x4c1e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4c1e, 0x4c20); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4c20, 0x4c21); mem_wr(gb, DE, A);
  CYC(0x4c21, 0x4c24);
  subterror_setAnimationFromAngle_hook(gb); return; // jp

substate2:
  CALL_C(0x4c24, enemyAnimate_hook, 0x2818, 0x4c27);
  CYC(0x4c27, 0x4c29); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4c29, 0x4c2a); A = mem_rd(gb, DE);
  CYC(0x4c2a, 0x4c2b); alu_or(gb, A);
  CYC(0x4c2b, 0x4c2d); A = 0xa3; // SND_LAND
  if (!(F & FZ)) { CALL_C_CC(0x4c2d, playSound_b00_hook, 0x0c98, 0x4c30); } else { CYC(0x4c2d, 0x4c30); } // call nz
  CALL_C(0x4c30, objectApplySpeed_hook, 0x201d, 0x4c33);
  CALL_C(0x4c33, ecom_bounceOffWallsAndHoles_b0f_hook, 0x42de, 0x4c36);
  if (!(F & FZ)) { CALL_C_CC(0x4c36, subterror_setAnimationFromAngle_hook, 0x4c76, 0x4c39); } else { CYC(0x4c36, 0x4c39); } // call nz

  // Dig back into ground when [counter1] reaches 0
  CALL_C(0x4c39, ecom_decCounter1_b0f_hook, 0x439a, 0x4c3c);
  if (!(F & FZ)) { RET_TAKEN(0x4c3c); return; } // ret nz
  CYC(0x4c3c, 0x4c3d);
  CYC(0x4c3d, 0x4c40);
  subterror_digIntoGround_hook(gb); return; // jp
}

void subterror_spawnDirtEvery8Frames_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c40, 0x4c41); E = alu_inc8(gb, E);
  CYC(0x4c41, 0x4c42); A = mem_rd(gb, DE); // [var31]
  CYC(0x4c42, 0x4c43); A = alu_dec8(gb, A);
  CYC(0x4c43, 0x4c44); mem_wr(gb, DE, A);
  if (!(F & FZ)) { RET_TAKEN(0x4c44); return; } // ret nz
  CYC(0x4c44, 0x4c45);
  subterror_spawnDirt_hook(gb); return; // fallthrough
}

void subterror_spawnDirt_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c45, 0x4c47); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4c47, 0x4c49); A = 0x07;
  CYC(0x4c49, 0x4c4a); mem_wr(gb, DE, A); // [var31] = 7
  CYC(0x4c4a, 0x4c4b); E = alu_dec8(gb, E);
  CYC(0x4c4b, 0x4c4c); mem_wr(gb, DE, A); // [var30] = 7
  CYC(0x4c4c, 0x4c4e); B = 0x32; // PART_SUBTERROR_DIRT
  CALL_C(0x4c4e, ecom_spawnProjectile_b0f_hook, 0x437c, 0x4c51);
  CALL_C(0x4c51, objectGetTileAtPosition_hook, 0x1444, 0x4c54);
  CYC(0x4c54, 0x4c55); C = L;
  CYC(0x4c55, 0x4c57); A = 0xef;
  CYC(0x4c57, 0x4c5a);
  setTile_hook(gb); return; // jp
}

void subterror_retFromCallerIfAnimationUnfinished_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c5a, enemyAnimate_hook, 0x2818, 0x4c5d);
  CYC(0x4c5d, 0x4c5e); H = D;
  CYC(0x4c5e, 0x4c60); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4c60, 0x4c61); A = mem_rd(gb, HL);
  CYC(0x4c61, 0x4c62); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4c62); return; } // ret nz
  CYC(0x4c62, 0x4c63);
  SET_AF(POP(0x4c63)); // pop af -- discard caller's own return address
  RET(0x4c64); return; // ret
}

// @param[out] a Anger level (0-2)
void subterror_getAngerLevel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c65, 0x4c67); B = 0x00;
  CYC(0x4c67, 0x4c69); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x4c69, 0x4c6a); A = mem_rd(gb, DE);
  CYC(0x4c6a, 0x4c6c); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x4c6c, 0x4c6e); goto haveAnger; } // jr nc
  CYC(0x4c6c, 0x4c6e);
  CYC(0x4c6e, 0x4c6f); B = alu_inc8(gb, B);
  CYC(0x4c6f, 0x4c71); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x4c71, 0x4c73); goto haveAnger; } // jr nc
  CYC(0x4c71, 0x4c73);
  CYC(0x4c73, 0x4c74); B = alu_inc8(gb, B);

haveAnger:
  CYC(0x4c74, 0x4c75); A = B;
  RET(0x4c75); return; // ret
}

void subterror_setAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c76, 0x4c77); H = D;
  CYC(0x4c77, 0x4c79); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4c79, 0x4c7a); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4c7a, 0x4c7b); alu_add(gb, A); // add a
  CYC(0x4c7b, 0x4c7d); A = alu_swap(gb, A);
  CYC(0x4c7d, 0x4c7f); alu_and(gb, 0x03);
  CYC(0x4c7f, 0x4c80); mem_wr(gb, HL, A); // [direction]
  CYC(0x4c80, 0x4c82); alu_add(gb, 0x00);
  CYC(0x4c82, 0x4c85);
  enemySetAnimation_hook(gb); return; // jp
}
