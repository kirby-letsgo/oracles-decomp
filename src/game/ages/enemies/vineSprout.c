#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode62), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode62), (from), (to), true)

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
  BASE(enemyCode62);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  {
    CYC(b_+7, b_+8); push_effect(gb, b_+8);
    uint16_t target = vineSprout_jump_table(gb);
    if (target == SYM(vineSprout_state0)) { vineSprout_state0_hook(gb); return; }
    if (target == SYM(vineSprout_state1)) { vineSprout_state1_hook(gb); return; }
    if (target == SYM(vineSprout_state_grabbed)) { vineSprout_state_grabbed_hook(gb); return; }
    if (target == SYM(vineSprout_state_switchHook)) { vineSprout_state_switchHook_hook(gb); return; }
    if (target == SYM(vineSprout_state4)) { vineSprout_state4_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7b9b, bare global; jump-table target from enemyCode62. Initialization: delete self
// if there is any other vine sprout on-screen already.
void vineSprout_state0_hook(GB *gb) {
  BASE(vineSprout_state0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL((w1ReservedInteraction0_var3f + 2)); // FIRST_ENEMY_INDEX,Enemy.id

nextEnemy:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x62); // ENEMY_VINE_SPROUT
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto nextEnemyCheck; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = D;
  CYC(b_+9, b_+10); alu_cp(gb, H);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);

nextEnemyCheck:
  CYC(b_+13, b_+14); H = alu_inc8(gb, H);
  CYC(b_+14, b_+15); A = H;
  CYC(b_+15, b_+17); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+17, b_+19); goto nextEnemy; } // jr c
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x14); // 20
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CALL_C(b_+30, vineSprout_getPosition_hook, SYM(vineSprout_getPosition), b_+33);
  CALL_C(b_+33, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+36);
  CYC(b_+36, SYM(vineSprout_state1)); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:7bc2, bare global; jump-table target from enemyCode62.
void vineSprout_state1_hook(GB *gb) {
  BASE(vineSprout_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkInAir);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) { CYCT(b_+4, b_+7); vineSprout_linkJumpingDownCliff_hook(gb); return; } // jp c
  CYC(b_+4, b_+7);
  CALL_C(b_+7, vineSprout_checkLinkInSprout_hook, SYM(vineSprout_checkLinkInSprout), b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  if (F & FC) { CYCT(b_+13, b_+16); vineSprout_restoreTileAtPosition_hook(gb); return; } // jp c
  CYC(b_+13, b_+16);
  CALL_C(b_+16, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+19);
  CALL_C(b_+19, vineSprout_updateTileAtPosition_hook, SYM(vineSprout_updateTileAtPosition), b_+22);
  CYC(b_+22, b_+25); SET_HL(w1Link_id); // w1Link.id
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); alu_or(gb, A); // cpa SPECIALOBJECT_LINK
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto notPushingSprout; } // jr nz
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); L = 0x04; // w1Link.state
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto notPushingSprout; } // jr nz
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); L = 0x0f; // w1Link.zh
  CYC(b_+38, b_+40); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto notPushingSprout; } // jr nz
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto notPushingSprout; } // jr nz
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wLinkAngle);
  CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+52, b_+54); goto notPushingSprout; } // jr z
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+57); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+57, b_+59); alu_and(gb, 0x03); // BTN_A|BTN_B
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto notPushingSprout; } // jr nz
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+64); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto notPushingSprout; } // jr nz
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); C = 0x12;
  CALL_C(b_+69, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+72);
  if (!(F & FC)) { CYCT(b_+72, b_+74); goto notPushingSprout; } // jr nc
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+76); B = 0x04;
  CALL_C(b_+76, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+79);
  if (!(F & FC)) { CYCT(b_+79, b_+81); goto notPushingSprout; } // jr nc
  CYC(b_+79, b_+81);
  CALL_C(b_+81, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0e), b_+84);
  CYC(b_+84, b_+86); alu_add(gb, 0x04);
  CYC(b_+86, b_+88); alu_and(gb, 0x18);
  CYC(b_+88, b_+89); mem_wr(gb, DE, A); // [angle]
  CYC(b_+89, b_+91); A = alu_swap(gb, A);
  CYC(b_+91, b_+92); alu_rlca(gb);
  CYC(b_+92, b_+93); B = A;
  CYC(b_+93, b_+96); A = mem_rd(gb, w1Link_direction); // w1Link.direction
  CYC(b_+96, b_+97); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto notPushingSprout; } // jr nz
  CYC(b_+97, b_+99);
  CALL_C(b_+99, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+102);
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; } // ret nz
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+104); A = mem_rd(gb, DE); // [angle]
  CYC(b_+104, b_+105); alu_rrca(gb);
  CYC(b_+105, b_+106); alu_rrca(gb);
  CYC(b_+106, b_+109); SET_HL(b_+147); // @pushOffsets
  CYC(b_+109, b_+110); vineSprout_addAToHl_from_rst(gb, b_+110);
  CALL_C(b_+110, objectGetPosition_hook, SYM(objectGetPosition), b_+113);
  CYC(b_+113, b_+114); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+114, b_+115); alu_add(gb, B);
  CYC(b_+115, b_+116); B = A;
  CYC(b_+116, b_+117); A = mem_rd(gb, HL);
  CYC(b_+117, b_+118); alu_add(gb, C);
  CYC(b_+118, b_+119); C = A;
  CALL_C(b_+119, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+122);
  if (!(F & FZ)) { CYCT(b_+122, b_+124); goto notPushingSprout; } // jr nz
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+125); H = D;
  CYC(b_+125, b_+127); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x04);
  CYC(b_+129, b_+131); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x16);
  CYC(b_+133, b_+135); A = 0x71; // SND_MOVEBLOCK
  CALL_C(b_+135, playSound_b00_hook, SYM(playSound_b00), b_+138);
  CYC(b_+138, b_+141); vineSprout_restoreTileAtPosition_hook(gb); return; // jp

notPushingSprout:
  CYC(b_+141, b_+143); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+143, b_+145); A = 0x14; // 20
  CYC(b_+145, b_+146); mem_wr(gb, DE, A);
  RET(b_+146); return; // ret
}

// 0e:7c5d, bare global; jump-table target from vineSprout_state1.
void vineSprout_linkJumpingDownCliff_hook(GB *gb) {
  BASE(vineSprout_linkJumpingDownCliff);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, vineSprout_restoreTileAtPosition_hook, SYM(vineSprout_restoreTileAtPosition), b_+3);
  CALL_C(b_+3, vineSprout_checkLinkInSprout_hook, SYM(vineSprout_checkLinkInSprout), b_+6);
  if (!(F & FC)) { RET_TAKEN(b_+6); return; } // ret nc
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x0f; // SpecialObject.zh
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_add(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; } // ret nc
  CYC(b_+12, SYM(vineSprout_destroy));
  vineSprout_destroy_hook(gb); return; // fallthrough
}

// 0e:7c6a, bare global; falls into from vineSprout_linkJumpingDownCliff, also reached via
// tail-jump from vineSprout_state_grabbed.
void vineSprout_destroy_hook(GB *gb) {
  BASE(vineSprout_destroy);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+5);
  CALL_C(b_+5, vineSprout_getDefaultPosition_hook, SYM(vineSprout_getDefaultPosition), b_+8);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE); // [subid]
  CYC(b_+10, b_+13); SET_HL((wGroup1RoomFlags + 240)); // wVinePositions
  CYC(b_+13, b_+14); vineSprout_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, B);
  CYC(b_+15, SYM(vineSprout_state_grabbed)); enemyDelete_hook(gb); return; // jp
}

// 0e:7c7c, bare global; jump-table target from enemyCode62.
void vineSprout_state_grabbed_hook(GB *gb) {
  BASE(vineSprout_state_grabbed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = vineSprout_jump_table(gb);
    if (target == b_+11) goto justGrabbed;
    if (target == b_+23) goto beingHeld;
    if (target == b_+24) goto justReleased;
    if (target == b_+34) goto hitGround;
    HANDOFF(target);
  }

justGrabbed:
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, vineSprout_restoreTileAtPosition_hook, SYM(vineSprout_restoreTileAtPosition), b_+20);
  CYC(b_+20, b_+23); objectSetVisiblec1_hook(gb); return; // jp

beingHeld:
  RET(b_+23); return; // ret

justReleased:
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+27, b_+29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+31, b_+33); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);

hitGround:
  CYC(b_+34, SYM(vineSprout_state_switchHook)); vineSprout_destroy_hook(gb); return; // jr
}

// 0e:7ca0, bare global; jump-table target from enemyCode62.
void vineSprout_state_switchHook_hook(GB *gb) {
  BASE(vineSprout_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = vineSprout_jump_table(gb);
    if (target == b_+11) goto justLatched;
    if (target == b_+17) goto beforeSwitch;
    if (target == SYM(objectCenterOnTile)) { objectCenterOnTile_hook(gb); return; }
    if (target == b_+18) goto released;
    HANDOFF(target);
  }

justLatched:
  CALL_C(b_+11, vineSprout_restoreTileAtPosition_hook, SYM(vineSprout_restoreTileAtPosition), b_+14);
  CYC(b_+14, b_+17); ecom_incSubstate_b0e_hook(gb); return; // jp

beforeSwitch:
  RET(b_+17); return; // ret

released:
  CYC(b_+18, b_+20); B = 0x01;
  CALL_C(b_+20, ecom_fallToGroundAndSetState_b0e_hook, SYM(ecom_fallToGroundAndSetState_b0e), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);
  CALL_C(b_+24, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+27);
  CYC(b_+27, SYM(vineSprout_state4)); vineSprout_updateTileAtPosition_hook(gb); return; // jp
}

// 0e:7cbe, bare global; jump-table target from enemyCode62. Being pushed.
void vineSprout_state4_hook(GB *gb) {
  BASE(vineSprout_state4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Link); // w1Link
  CALL_C(b_+3, preventObjectHFromPassingObjectD_hook, SYM(preventObjectHFromPassingObjectD), b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_applyVelocityForTopDownEnemyNoHoles_b0e_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x14); // [counter1] = 20
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x01);
  CALL_C(b_+18, objectCenterOnTile_hook, SYM(objectCenterOnTile), SYM(vineSprout_updateTileAtPosition));
  vineSprout_updateTileAtPosition_hook(gb); return; // fallthrough
}

// 0e:7cd3, bare global; falls into from vineSprout_state4, also called from
// vineSprout_state1 and vineSprout_state_switchHook. Updates tile properties at the
// current position, updates wVinePositions, if var33 is nonzero.
void vineSprout_updateTileAtPosition_hook(GB *gb) {
  BASE(vineSprout_updateTileAtPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CALL_C(b_+5, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x0f);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(b_+12, b_+14); H = 0xcf; // >wRoomLayout
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A); // [var31] = tile index
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); A = L;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A); // [var32] = tile position
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x00); // TILEINDEX_00
  CYC(b_+21, b_+22); E = alu_inc8(gb, E);
  CYC(b_+22, b_+24); A = 0x01;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A); // [var33] = 1

fixVerticalBoundary:
  CYC(b_+25, b_+26); A = L;
  CYC(b_+26, b_+28); alu_and(gb, 0xf0);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto fixHorizontalBoundary; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); L = (uint8_t)(L | (1 << 4)); // set 4,l
  CYC(b_+32, b_+34); goto fixHorizontalBoundaryEntry; // jr

fixHorizontalBoundary:
  CYC(b_+34, b_+36); alu_cp(gb, 0x70); // (SMALL_ROOM_HEIGHT-1)<<4
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto fixHorizontalBoundaryEntry; } // jr nz
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); L = (uint8_t)(L & ~(1 << 4)); // res 4,l

fixHorizontalBoundaryEntry:
  CYC(b_+40, b_+41); A = L;
  CYC(b_+41, b_+43); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto checkRightBoundary; } // jr nz
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+46); L = alu_inc8(gb, L);
  CYC(b_+46, b_+48); goto setPosition; // jr

checkRightBoundary:
  CYC(b_+48, b_+50); alu_cp(gb, 0x09); // SMALL_ROOM_WIDTH-1
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto setPosition; } // jr nz
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); L = alu_dec8(gb, L);

setPosition:
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+59); SET_BC((wGroup1RoomFlags + 240)); // wVinePositions
  CALL_C(b_+59, addAToBc_hook, 0x006d, b_+62);
  CYC(b_+62, b_+63); A = L;
  CYC(b_+63, b_+64); mem_wr(gb, BC, A);
  RET(b_+64); return; // ret
}

// 0e:7d14, bare global; called from several other states. Undoes the changes done
// previously to the tile at the sprout's current position.
void vineSprout_restoreTileAtPosition_hook(GB *gb) {
  BASE(vineSprout_restoreTileAtPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A); // [var33]
  CYC(b_+7, b_+8); E = alu_dec8(gb, E);
  CYC(b_+8, b_+9); A = mem_rd(gb, DE); // [var32]
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+11); E = alu_dec8(gb, E);
  CYC(b_+11, b_+12); A = mem_rd(gb, DE); // [var31]
  CYC(b_+12, b_+14); H = 0xcf; // >wRoomLayout
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); H = 0xce; // >wRoomCollisions
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  RET(b_+19); return; // ret
}

// 0e:7d28, bare global; called from vineSprout_state1 and vineSprout_linkJumpingDownCliff.
// @param[out] cflag c if Link is in the sprout
void vineSprout_checkLinkInSprout_hook(GB *gb) {
  BASE(vineSprout_checkLinkInSprout);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = 0x0b; // SpecialObject.yh
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+12); alu_add(gb, 0x06);
  CYC(b_+12, b_+14); alu_cp(gb, 0x0d);
  if (!(F & FC)) { RET_TAKEN(b_+14); return; } // ret nc
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); L = 0x0d; // SpecialObject.xh
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+23); alu_add(gb, 0x06);
  CYC(b_+23, b_+25); alu_cp(gb, 0x0d);
  RET(b_+25); return; // ret
}

// 0e:7d42, bare global; called from vineSprout_destroy, vineSprout_getPosition and, when
// enabled, from vineSprout_state0.
// @param[out] a Sprout's default position
// @param[out] de Enemy.subid
void vineSprout_getDefaultPosition_hook(GB *gb) {
  BASE(vineSprout_getDefaultPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_BC(b_+11); // @defaultVinePositions
  CALL_C(b_+6, addAToBc_hook, 0x006d, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  RET(b_+10); return; // ret
}

// 0e:7d53, bare global; called from vineSprout_state0.
// @param[out] c Sprout's position
void vineSprout_getPosition_hook(GB *gb) {
  BASE(vineSprout_getPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL((wGroup1RoomFlags + 240)); // wVinePositions
  CYC(b_+6, b_+7); vineSprout_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); C = mem_rd(gb, HL);
  CYC(b_+8, b_+10); B = 0xcf; // >wRoomLayout
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+15); SET_HL(b_+26); // @respawnableTiles

checkTile:
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+17); return; } // ret z
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); alu_cp(gb, E);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto checkTile; } // jr nz
  CYC(b_+19, b_+21);
  CALL_C(b_+21, vineSprout_getDefaultPosition_hook, SYM(vineSprout_getDefaultPosition), b_+24);
  CYC(b_+24, b_+25); C = A;
  RET(b_+25); return; // ret
}
