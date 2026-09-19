#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode62_hook(GB *gb);
void vineSprout_state0_hook(GB *gb);
void vineSprout_state1_hook(GB *gb);
void vineSprout_linkJumpingDownCliff_hook(GB *gb);
void vineSprout_destroy_hook(GB *gb);
void vineSprout_state_grabbed_hook(GB *gb);
void vineSprout_state_switchHook_hook(GB *gb);
void vineSprout_state4_hook(GB *gb);
void vineSprout_updateTileAtPosition_hook(GB *gb);
void vineSprout_restoreTileAtPosition_hook(GB *gb);
void vineSprout_checkLinkInSprout_hook(GB *gb);
void vineSprout_getDefaultPosition_hook(GB *gb);
void vineSprout_getPosition_hook(GB *gb);

static uint16_t vineSprout_jump_table(GB *gb) {
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

static void vineSprout_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_VINE_SPROUT
//
// Variables:
//   var31: Tile index underneath the sprout?
//   var32: Short-form position of vine sprout
//   var33: Nonzero if the "tile properties" underneath this sprout have been modified
// ==================================================================================================
void enemyCode62_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b89, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x7b8c);
  if (F & FC) { RET_TAKEN(0x7b8c); return; } // ret c
  CYC(0x7b8c, 0x7b8d);
  CYC(0x7b8d, 0x7b8f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7b8f, 0x7b90); A = mem_rd(gb, DE);
  {
    CYC(0x7b90, 0x7b91); push_effect(gb, 0x7b91);
    uint16_t target = vineSprout_jump_table(gb);
    if (target == 0x7b9b) { vineSprout_state0_hook(gb); return; }
    if (target == 0x7bc2) { vineSprout_state1_hook(gb); return; }
    if (target == 0x7c7c) { vineSprout_state_grabbed_hook(gb); return; }
    if (target == 0x7ca0) { vineSprout_state_switchHook_hook(gb); return; }
    if (target == 0x7cbe) { vineSprout_state4_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7b9b, bare global; jump-table target from enemyCode62. Initialization: delete self
// if there is any other vine sprout on-screen already.
void vineSprout_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b9b, 0x7b9e); SET_HL(0xd081); // FIRST_ENEMY_INDEX,Enemy.id

nextEnemy:
  CYC(0x7b9e, 0x7b9f); A = mem_rd(gb, HL);
  CYC(0x7b9f, 0x7ba1); alu_cp(gb, 0x62); // ENEMY_VINE_SPROUT
  if (!(F & FZ)) { CYCT(0x7ba1, 0x7ba3); goto nextEnemyCheck; } // jr nz
  CYC(0x7ba1, 0x7ba3);
  CYC(0x7ba3, 0x7ba4); A = D;
  CYC(0x7ba4, 0x7ba5); alu_cp(gb, H);
  if (!(F & FZ)) { CYCT(0x7ba5, 0x7ba8); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x7ba5, 0x7ba8);

nextEnemyCheck:
  CYC(0x7ba8, 0x7ba9); H = alu_inc8(gb, H);
  CYC(0x7ba9, 0x7baa); A = H;
  CYC(0x7baa, 0x7bac); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x7bac, 0x7bae); goto nextEnemy; } // jr c
  CYC(0x7bac, 0x7bae);
  CYC(0x7bae, 0x7baf); H = D;
  CYC(0x7baf, 0x7bb0); L = E;
  CYC(0x7bb0, 0x7bb1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7bb1, 0x7bb3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7bb3, 0x7bb5); mem_wr(gb, HL, 0x14); // 20
  CYC(0x7bb5, 0x7bb7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7bb7, 0x7bb9); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CALL_C(0x7bb9, vineSprout_getPosition_hook, 0x7d53, 0x7bbc);
  CALL_C(0x7bbc, objectSetShortPosition_hook, 0x20c3, 0x7bbf);
  CYC(0x7bbf, 0x7bc2); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:7bc2, bare global; jump-table target from enemyCode62.
void vineSprout_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7bc2, 0x7bc5); A = mem_rd(gb, wLinkInAir);
  CYC(0x7bc5, 0x7bc6); alu_rlca(gb);
  if (F & FC) { CYCT(0x7bc6, 0x7bc9); vineSprout_linkJumpingDownCliff_hook(gb); return; } // jp c
  CYC(0x7bc6, 0x7bc9);
  CALL_C(0x7bc9, vineSprout_checkLinkInSprout_hook, 0x7d28, 0x7bcc);
  CYC(0x7bcc, 0x7bce); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7bce, 0x7bcf); A = mem_rd(gb, DE);
  if (F & FC) { CYCT(0x7bcf, 0x7bd2); vineSprout_restoreTileAtPosition_hook(gb); return; } // jp c
  CYC(0x7bcf, 0x7bd2);
  CALL_C(0x7bd2, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x7bd5);
  CALL_C(0x7bd5, vineSprout_updateTileAtPosition_hook, 0x7cd3, 0x7bd8);
  CYC(0x7bd8, 0x7bdb); SET_HL(0xd001); // w1Link.id
  CYC(0x7bdb, 0x7bdc); A = mem_rd(gb, HL);
  CYC(0x7bdc, 0x7bdd); alu_or(gb, A); // cpa SPECIALOBJECT_LINK
  if (!(F & FZ)) { CYCT(0x7bdd, 0x7bdf); goto notPushingSprout; } // jr nz
  CYC(0x7bdd, 0x7bdf);
  CYC(0x7bdf, 0x7be1); L = 0x04; // w1Link.state
  CYC(0x7be1, 0x7be2); A = mem_rd(gb, HL);
  CYC(0x7be2, 0x7be4); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) { CYCT(0x7be4, 0x7be6); goto notPushingSprout; } // jr nz
  CYC(0x7be4, 0x7be6);
  CYC(0x7be6, 0x7be8); L = 0x0f; // w1Link.zh
  CYC(0x7be8, 0x7bea); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7bea, 0x7bec); goto notPushingSprout; } // jr nz
  CYC(0x7bea, 0x7bec);
  CYC(0x7bec, 0x7bef); A = mem_rd(gb, wLinkSwimmingState);
  CYC(0x7bef, 0x7bf0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7bf0, 0x7bf2); goto notPushingSprout; } // jr nz
  CYC(0x7bf0, 0x7bf2);
  CYC(0x7bf2, 0x7bf5); A = mem_rd(gb, wLinkAngle);
  CYC(0x7bf5, 0x7bf6); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x7bf6, 0x7bf8); goto notPushingSprout; } // jr z
  CYC(0x7bf6, 0x7bf8);
  CYC(0x7bf8, 0x7bfb); A = mem_rd(gb, wGameKeysPressed);
  CYC(0x7bfb, 0x7bfd); alu_and(gb, 0x03); // BTN_A|BTN_B
  if (!(F & FZ)) { CYCT(0x7bfd, 0x7bff); goto notPushingSprout; } // jr nz
  CYC(0x7bfd, 0x7bff);
  CYC(0x7bff, 0x7c02); A = mem_rd(gb, wLinkGrabState);
  CYC(0x7c02, 0x7c03); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7c03, 0x7c05); goto notPushingSprout; } // jr nz
  CYC(0x7c03, 0x7c05);
  CYC(0x7c05, 0x7c07); C = 0x12;
  CALL_C(0x7c07, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x7c0a);
  if (!(F & FC)) { CYCT(0x7c0a, 0x7c0c); goto notPushingSprout; } // jr nc
  CYC(0x7c0a, 0x7c0c);
  CYC(0x7c0c, 0x7c0e); B = 0x04;
  CALL_C(0x7c0e, objectCheckCenteredWithLink_hook, 0x1fee, 0x7c11);
  if (!(F & FC)) { CYCT(0x7c11, 0x7c13); goto notPushingSprout; } // jr nc
  CYC(0x7c11, 0x7c13);
  CALL_C(0x7c13, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, 0x43ab, 0x7c16);
  CYC(0x7c16, 0x7c18); alu_add(gb, 0x04);
  CYC(0x7c18, 0x7c1a); alu_and(gb, 0x18);
  CYC(0x7c1a, 0x7c1b); mem_wr(gb, DE, A); // [angle]
  CYC(0x7c1b, 0x7c1d); A = alu_swap(gb, A);
  CYC(0x7c1d, 0x7c1e); alu_rlca(gb);
  CYC(0x7c1e, 0x7c1f); B = A;
  CYC(0x7c1f, 0x7c22); A = mem_rd(gb, 0xd008); // w1Link.direction
  CYC(0x7c22, 0x7c23); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x7c23, 0x7c25); goto notPushingSprout; } // jr nz
  CYC(0x7c23, 0x7c25);
  CALL_C(0x7c25, ecom_decCounter1_b0e_hook, 0x439a, 0x7c28);
  if (!(F & FZ)) { RET_TAKEN(0x7c28); return; } // ret nz
  CYC(0x7c28, 0x7c29);
  CYC(0x7c29, 0x7c2a); A = mem_rd(gb, DE); // [angle]
  CYC(0x7c2a, 0x7c2b); alu_rrca(gb);
  CYC(0x7c2b, 0x7c2c); alu_rrca(gb);
  CYC(0x7c2c, 0x7c2f); SET_HL(0x7c55); // @pushOffsets
  CYC(0x7c2f, 0x7c30); vineSprout_addAToHl_from_rst(gb, 0x7c30);
  CALL_C(0x7c30, objectGetPosition_hook, 0x208a, 0x7c33);
  CYC(0x7c33, 0x7c34); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7c34, 0x7c35); alu_add(gb, B);
  CYC(0x7c35, 0x7c36); B = A;
  CYC(0x7c36, 0x7c37); A = mem_rd(gb, HL);
  CYC(0x7c37, 0x7c38); alu_add(gb, C);
  CYC(0x7c38, 0x7c39); C = A;
  CALL_C(0x7c39, getTileCollisionsAtPosition_hook, 0x14b7, 0x7c3c);
  if (!(F & FZ)) { CYCT(0x7c3c, 0x7c3e); goto notPushingSprout; } // jr nz
  CYC(0x7c3c, 0x7c3e);
  CYC(0x7c3e, 0x7c3f); H = D;
  CYC(0x7c3f, 0x7c41); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7c41, 0x7c43); mem_wr(gb, HL, 0x04);
  CYC(0x7c43, 0x7c45); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7c45, 0x7c47); mem_wr(gb, HL, 0x16);
  CYC(0x7c47, 0x7c49); A = 0x71; // SND_MOVEBLOCK
  CALL_C(0x7c49, playSound_b00_hook, 0x0c98, 0x7c4c);
  CYC(0x7c4c, 0x7c4f); vineSprout_restoreTileAtPosition_hook(gb); return; // jp

notPushingSprout:
  CYC(0x7c4f, 0x7c51); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7c51, 0x7c53); A = 0x14; // 20
  CYC(0x7c53, 0x7c54); mem_wr(gb, DE, A);
  RET(0x7c54); return; // ret
}

// 0e:7c5d, bare global; jump-table target from vineSprout_state1.
void vineSprout_linkJumpingDownCliff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c5d, vineSprout_restoreTileAtPosition_hook, 0x7d14, 0x7c60);
  CALL_C(0x7c60, vineSprout_checkLinkInSprout_hook, 0x7d28, 0x7c63);
  if (!(F & FC)) { RET_TAKEN(0x7c63); return; } // ret nc
  CYC(0x7c63, 0x7c64);
  CYC(0x7c64, 0x7c66); L = 0x0f; // SpecialObject.zh
  CYC(0x7c66, 0x7c67); A = mem_rd(gb, HL);
  CYC(0x7c67, 0x7c69); alu_add(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(0x7c69); return; } // ret nc
  CYC(0x7c69, 0x7c6a);
  vineSprout_destroy_hook(gb); return; // fallthrough
}

// 0e:7c6a, bare global; falls into from vineSprout_linkJumpingDownCliff, also reached via
// tail-jump from vineSprout_state_grabbed.
void vineSprout_destroy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c6a, 0x7c6c); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(0x7c6c, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x7c6f);
  CALL_C(0x7c6f, vineSprout_getDefaultPosition_hook, 0x7d42, 0x7c72);
  CYC(0x7c72, 0x7c73); B = A;
  CYC(0x7c73, 0x7c74); A = mem_rd(gb, DE); // [subid]
  CYC(0x7c74, 0x7c77); SET_HL(0xc8f0); // wVinePositions
  CYC(0x7c77, 0x7c78); vineSprout_addAToHl_from_rst(gb, 0x7c78);
  CYC(0x7c78, 0x7c79); mem_wr(gb, HL, B);
  CYC(0x7c79, 0x7c7c); enemyDelete_hook(gb); return; // jp
}

// 0e:7c7c, bare global; jump-table target from enemyCode62.
void vineSprout_state_grabbed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c7c, 0x7c7d); E = alu_inc8(gb, E);
  CYC(0x7c7d, 0x7c7e); A = mem_rd(gb, DE);
  {
    CYC(0x7c7e, 0x7c7f); push_effect(gb, 0x7c7f);
    uint16_t target = vineSprout_jump_table(gb);
    if (target == 0x7c87) goto justGrabbed;
    if (target == 0x7c93) goto beingHeld;
    if (target == 0x7c94) goto justReleased;
    if (target == 0x7c9e) goto hitGround;
    HANDOFF(target);
  }

justGrabbed:
  CYC(0x7c87, 0x7c88); alu_xor(gb, A);
  CYC(0x7c88, 0x7c8b); mem_wr(gb, wLinkGrabState2, A);
  CYC(0x7c8b, 0x7c8c); A = alu_inc8(gb, A);
  CYC(0x7c8c, 0x7c8d); mem_wr(gb, DE, A);
  CALL_C(0x7c8d, vineSprout_restoreTileAtPosition_hook, 0x7d14, 0x7c90);
  CYC(0x7c90, 0x7c93); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  RET(0x7c93); return; // ret

justReleased:
  CYC(0x7c94, 0x7c95); H = D;
  CYC(0x7c95, 0x7c97); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x7c97, 0x7c99); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CYC(0x7c99, 0x7c9b); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x7c9b, 0x7c9d); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x7c9d); return; } // ret nz
  CYC(0x7c9d, 0x7c9e);

hitGround:
  CYC(0x7c9e, 0x7ca0); vineSprout_destroy_hook(gb); return; // jr
}

// 0e:7ca0, bare global; jump-table target from enemyCode62.
void vineSprout_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ca0, 0x7ca1); E = alu_inc8(gb, E);
  CYC(0x7ca1, 0x7ca2); A = mem_rd(gb, DE);
  {
    CYC(0x7ca2, 0x7ca3); push_effect(gb, 0x7ca3);
    uint16_t target = vineSprout_jump_table(gb);
    if (target == 0x7cab) goto justLatched;
    if (target == 0x7cb1) goto beforeSwitch;
    if (target == 0x20db) { objectCenterOnTile_hook(gb); return; }
    if (target == 0x7cb2) goto released;
    HANDOFF(target);
  }

justLatched:
  CALL_C(0x7cab, vineSprout_restoreTileAtPosition_hook, 0x7d14, 0x7cae);
  CYC(0x7cae, 0x7cb1); ecom_incSubstate_b0e_hook(gb); return; // jp

beforeSwitch:
  RET(0x7cb1); return; // ret

released:
  CYC(0x7cb2, 0x7cb4); B = 0x01;
  CALL_C(0x7cb4, ecom_fallToGroundAndSetState_b0e_hook, 0x44e2, 0x7cb7);
  if (!(F & FZ)) { RET_TAKEN(0x7cb7); return; } // ret nz
  CYC(0x7cb7, 0x7cb8);
  CALL_C(0x7cb8, objectCenterOnTile_hook, 0x20db, 0x7cbb);
  CYC(0x7cbb, 0x7cbe); vineSprout_updateTileAtPosition_hook(gb); return; // jp
}

// 0e:7cbe, bare global; jump-table target from enemyCode62. Being pushed.
void vineSprout_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7cbe, 0x7cc1); SET_HL(0xd000); // w1Link
  CALL_C(0x7cc1, preventObjectHFromPassingObjectD_hook, 0x1d7f, 0x7cc4);
  CALL_C(0x7cc4, ecom_decCounter1_b0e_hook, 0x439a, 0x7cc7);
  if (!(F & FZ)) { CYCT(0x7cc7, 0x7cca); ecom_applyVelocityForTopDownEnemyNoHoles_b0e_hook(gb); return; } // jp nz
  CYC(0x7cc7, 0x7cca);
  CYC(0x7cca, 0x7ccc); mem_wr(gb, HL, 0x14); // [counter1] = 20
  CYC(0x7ccc, 0x7cce); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7cce, 0x7cd0); mem_wr(gb, HL, 0x01);
  CALL_C(0x7cd0, objectCenterOnTile_hook, 0x20db, 0x7cd3);
  vineSprout_updateTileAtPosition_hook(gb); return; // fallthrough
}

// 0e:7cd3, bare global; falls into from vineSprout_state4, also called from
// vineSprout_state1 and vineSprout_state_switchHook. Updates tile properties at the
// current position, updates wVinePositions, if var33 is nonzero.
void vineSprout_updateTileAtPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7cd3, 0x7cd5); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7cd5, 0x7cd6); A = mem_rd(gb, DE);
  CYC(0x7cd6, 0x7cd7); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7cd7); return; } // ret nz
  CALL_C(0x7cd8, objectGetTileCollisions_hook, 0x14ad, 0x7cdb);
  CYC(0x7cdb, 0x7cdd); mem_wr(gb, HL, 0x0f);
  CYC(0x7cdd, 0x7cdf); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(0x7cdf, 0x7ce1); H = 0xcf; // >wRoomLayout
  CYC(0x7ce1, 0x7ce2); A = mem_rd(gb, HL);
  CYC(0x7ce2, 0x7ce3); mem_wr(gb, DE, A); // [var31] = tile index
  CYC(0x7ce3, 0x7ce4); E = alu_inc8(gb, E);
  CYC(0x7ce4, 0x7ce5); A = L;
  CYC(0x7ce5, 0x7ce6); mem_wr(gb, DE, A); // [var32] = tile position
  CYC(0x7ce6, 0x7ce8); mem_wr(gb, HL, 0x00); // TILEINDEX_00
  CYC(0x7ce8, 0x7ce9); E = alu_inc8(gb, E);
  CYC(0x7ce9, 0x7ceb); A = 0x01;
  CYC(0x7ceb, 0x7cec); mem_wr(gb, DE, A); // [var33] = 1

fixVerticalBoundary:
  CYC(0x7cec, 0x7ced); A = L;
  CYC(0x7ced, 0x7cef); alu_and(gb, 0xf0);
  if (!(F & FZ)) { CYCT(0x7cef, 0x7cf1); goto fixHorizontalBoundary; } // jr nz
  CYC(0x7cef, 0x7cf1);
  CYC(0x7cf1, 0x7cf3); L = (uint8_t)(L | (1 << 4)); // set 4,l
  CYC(0x7cf3, 0x7cf5); goto fixHorizontalBoundaryEntry; // jr

fixHorizontalBoundary:
  CYC(0x7cf5, 0x7cf7); alu_cp(gb, 0x70); // (SMALL_ROOM_HEIGHT-1)<<4
  if (!(F & FZ)) { CYCT(0x7cf7, 0x7cf9); goto fixHorizontalBoundaryEntry; } // jr nz
  CYC(0x7cf7, 0x7cf9);
  CYC(0x7cf9, 0x7cfb); L = (uint8_t)(L & ~(1 << 4)); // res 4,l

fixHorizontalBoundaryEntry:
  CYC(0x7cfb, 0x7cfc); A = L;
  CYC(0x7cfc, 0x7cfe); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x7cfe, 0x7d00); goto checkRightBoundary; } // jr nz
  CYC(0x7cfe, 0x7d00);
  CYC(0x7d00, 0x7d01); L = alu_inc8(gb, L);
  CYC(0x7d01, 0x7d03); goto setPosition; // jr

checkRightBoundary:
  CYC(0x7d03, 0x7d05); alu_cp(gb, 0x09); // SMALL_ROOM_WIDTH-1
  if (!(F & FZ)) { CYCT(0x7d05, 0x7d07); goto setPosition; } // jr nz
  CYC(0x7d05, 0x7d07);
  CYC(0x7d07, 0x7d08); L = alu_dec8(gb, L);

setPosition:
  CYC(0x7d08, 0x7d0a); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7d0a, 0x7d0b); A = mem_rd(gb, DE);
  CYC(0x7d0b, 0x7d0e); SET_BC(0xc8f0); // wVinePositions
  CALL_C(0x7d0e, addAToBc_hook, 0x006d, 0x7d11);
  CYC(0x7d11, 0x7d12); A = L;
  CYC(0x7d12, 0x7d13); mem_wr(gb, BC, A);
  RET(0x7d13); return; // ret
}

// 0e:7d14, bare global; called from several other states. Undoes the changes done
// previously to the tile at the sprout's current position.
void vineSprout_restoreTileAtPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d14, 0x7d16); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7d16, 0x7d17); A = mem_rd(gb, DE);
  CYC(0x7d17, 0x7d18); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7d18); return; } // ret z
  CYC(0x7d18, 0x7d19);
  CYC(0x7d19, 0x7d1a); alu_xor(gb, A);
  CYC(0x7d1a, 0x7d1b); mem_wr(gb, DE, A); // [var33]
  CYC(0x7d1b, 0x7d1c); E = alu_dec8(gb, E);
  CYC(0x7d1c, 0x7d1d); A = mem_rd(gb, DE); // [var32]
  CYC(0x7d1d, 0x7d1e); L = A;
  CYC(0x7d1e, 0x7d1f); E = alu_dec8(gb, E);
  CYC(0x7d1f, 0x7d20); A = mem_rd(gb, DE); // [var31]
  CYC(0x7d20, 0x7d22); H = 0xcf; // >wRoomLayout
  CYC(0x7d22, 0x7d23); mem_wr(gb, HL, A);
  CYC(0x7d23, 0x7d25); H = 0xce; // >wRoomCollisions
  CYC(0x7d25, 0x7d27); mem_wr(gb, HL, 0x00);
  RET(0x7d27); return; // ret
}

// 0e:7d28, bare global; called from vineSprout_state1 and vineSprout_linkJumpingDownCliff.
// @param[out] cflag c if Link is in the sprout
void vineSprout_checkLinkInSprout_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d28, 0x7d2b); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x7d2b, 0x7d2c); H = A;
  CYC(0x7d2c, 0x7d2e); L = 0x0b; // SpecialObject.yh
  CYC(0x7d2e, 0x7d30); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7d30, 0x7d31); A = mem_rd(gb, DE);
  CYC(0x7d31, 0x7d32); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7d32, 0x7d34); alu_add(gb, 0x06);
  CYC(0x7d34, 0x7d36); alu_cp(gb, 0x0d);
  if (!(F & FC)) { RET_TAKEN(0x7d36); return; } // ret nc
  CYC(0x7d36, 0x7d37);
  CYC(0x7d37, 0x7d39); L = 0x0d; // SpecialObject.xh
  CYC(0x7d39, 0x7d3b); E = ENEMY_BASE + OBJ_XH;
  CYC(0x7d3b, 0x7d3c); A = mem_rd(gb, DE);
  CYC(0x7d3c, 0x7d3d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7d3d, 0x7d3f); alu_add(gb, 0x06);
  CYC(0x7d3f, 0x7d41); alu_cp(gb, 0x0d);
  RET(0x7d41); return; // ret
}

// 0e:7d42, bare global; called from vineSprout_destroy, vineSprout_getPosition and, when
// enabled, from vineSprout_state0.
// @param[out] a Sprout's default position
// @param[out] de Enemy.subid
void vineSprout_getDefaultPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d42, 0x7d44); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7d44, 0x7d45); A = mem_rd(gb, DE);
  CYC(0x7d45, 0x7d48); SET_BC(0x7d4d); // @defaultVinePositions
  CALL_C(0x7d48, addAToBc_hook, 0x006d, 0x7d4b);
  CYC(0x7d4b, 0x7d4c); A = mem_rd(gb, BC);
  RET(0x7d4c); return; // ret
}

// 0e:7d53, bare global; called from vineSprout_state0.
// @param[out] c Sprout's position
void vineSprout_getPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d53, 0x7d55); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7d55, 0x7d56); A = mem_rd(gb, DE);
  CYC(0x7d56, 0x7d59); SET_HL(0xc8f0); // wVinePositions
  CYC(0x7d59, 0x7d5a); vineSprout_addAToHl_from_rst(gb, 0x7d5a);
  CYC(0x7d5a, 0x7d5b); C = mem_rd(gb, HL);
  CYC(0x7d5b, 0x7d5d); B = 0xcf; // >wRoomLayout
  CYC(0x7d5d, 0x7d5e); A = mem_rd(gb, BC);
  CYC(0x7d5e, 0x7d5f); E = A;
  CYC(0x7d5f, 0x7d62); SET_HL(0x7d6d); // @respawnableTiles

checkTile:
  CYC(0x7d62, 0x7d63); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7d63, 0x7d64); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7d64); return; } // ret z
  CYC(0x7d64, 0x7d65);
  CYC(0x7d65, 0x7d66); alu_cp(gb, E);
  if (!(F & FZ)) { CYCT(0x7d66, 0x7d68); goto checkTile; } // jr nz
  CYC(0x7d66, 0x7d68);
  CALL_C(0x7d68, vineSprout_getDefaultPosition_hook, 0x7d42, 0x7d6b);
  CYC(0x7d6b, 0x7d6c); C = A;
  RET(0x7d6c); return; // ret
}
