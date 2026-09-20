#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void likelike_state_uninitialized_hook(GB *gb);
void likelike_state_switchHook_hook(GB *gb);
void likelike_state_galeSeed_hook(GB *gb);
void likelike_state_stub_hook(GB *gb);
void likelike_subid00_hook(GB *gb);
void likelike_subid00_state8_hook(GB *gb);
void likelike_state9_hook(GB *gb);
void likelike_stateA_hook(GB *gb);
void likelike_animate_hook(GB *gb);
void likelike_stateB_hook(GB *gb);
void likelike_releaseLink_hook(GB *gb);
void likelike_stateC_hook(GB *gb);
void likelike_subid01_hook(GB *gb);
void likelike_subid02_hook(GB *gb);
void likelike_animate2_hook(GB *gb);
void likelike_subid03_hook(GB *gb);
void likelike_spawn_hook(GB *gb);
void likelike_setChildSpawnPosition_hook(GB *gb);
void likelike_findAllLikelikesWithSubid0_hook(GB *gb);
void likelike_chooseRandomPosition_hook(GB *gb);
void likelike_checkHazards_hook(GB *gb);

static uint16_t likelike_jump_table(GB *gb) {
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

static void likelike_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_LIKE_LIKE
//
// Variables:
//   relatedObj1: Pointer to the like-like spawner (subid 1), if one exists.
//   var30: Number of like-likes on-screen (for subid 1)
// ==================================================================================================
void enemyCode24_hook(GB *gb) {
  BASE(enemyCode24);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, likelike_checkHazards_hook, SYM(likelike_checkHazards), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto dead; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);

  // ENEMYSTATUS_JUST_HIT
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);

  // Just collided with Link. omnomnom
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+27); C = mem_rd(gb, HL);
  CYC(b_+27, b_+30); SET_HL((SYM(likelike_subid03) + 8)); // bank5.checkPositionSurroundedByWalls
  CYC(b_+30, b_+32); E = 0x05;
  CALL_C(b_+32, interBankCall_hook, 0x008a, b_+35);
  CYC(b_+35, b_+37); B = alu_rl(gb, B);
  if (F & FC) { CYCT(b_+37, b_+40); likelike_releaseLink_hook(gb); return; } // jp c
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_or(gb, A);
  CYC(b_+44, b_+46); A = 0x0b;
  if (F & FZ) { CYCT(b_+46, b_+48); goto setState; } // jr z
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+49); A = alu_inc8(gb, A);

setState:
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+53, b_+54); L = alu_inc8(gb, L);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x00); // [counter1] = 0
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 90); // [counter2] = 90
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+61, b_+63); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+63, b_+66); SET_HL(w1Link);
  CALL_C(b_+66, objectCopyPosition_hook, SYM(objectCopyPosition), b_+69);
  CYC(b_+69, b_+71); L = OBJ_COLLISION_TYPE; // <w1Link.collisionType
  CYC(b_+71, b_+73); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+73, b_+75); A = 0x01;
  CALL_C(b_+75, enemySetAnimation_hook, SYM(enemySetAnimation), b_+78);
  CYC(b_+78, b_+81); objectSetVisiblec1_hook(gb); return; // jp

dead:
  CYC(b_+81, b_+83); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+85, b_+88); enemyDie_hook(gb); return; } // jp z
  CYC(b_+85, b_+88);
  CYC(b_+88, b_+89); H = A;
  CYC(b_+89, b_+91); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+92, b_+95); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+95, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+98);
  if (!(F & FC)) { CYCT(b_+98, b_+100); goto normalState; } // jr nc
  CYC(b_+98, b_+100);
  {
    CYC(b_+100, b_+101); push_effect(gb, b_+101);
    uint16_t target = likelike_jump_table(gb);
    if (target == SYM(likelike_state_uninitialized)) { likelike_state_uninitialized_hook(gb); return; }
    if (target == SYM(likelike_state_stub)) { likelike_state_stub_hook(gb); return; }
    if (target == SYM(likelike_state_switchHook)) { likelike_state_switchHook_hook(gb); return; }
    if (target == SYM(likelike_state_galeSeed)) { likelike_state_galeSeed_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+117, b_+118); A = B;
  {
    CYC(b_+118, b_+119); push_effect(gb, b_+119);
    uint16_t target = likelike_jump_table(gb);
    if (target == SYM(likelike_subid00)) { likelike_subid00_hook(gb); return; }
    if (target == SYM(likelike_subid01)) { likelike_subid01_hook(gb); return; }
    if (target == SYM(likelike_subid02)) { likelike_subid02_hook(gb); return; }
    if (target == SYM(likelike_subid03)) { likelike_subid03_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5d18, bare global; jump-table target from enemyCode24.
void likelike_state_uninitialized_hook(GB *gb) {
  BASE(likelike_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_bit(gb, 0, B);
  if (F & FZ) CALL_C_CC(b_+2, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+5); else CYC(b_+2, b_+5); // call z
  CYC(b_+5, b_+7); A = 0x0a; // SPEED_40
  CYC(b_+7, b_+10); ecom_setSpeedAndState8_b0d_hook(gb); return; // jp
}

// 0d:5d22, bare global; jump-table target from enemyCode24. Internal @substate1/@substate2/
// @substate3 are @-local (no separate registration).
void likelike_state_switchHook_hook(GB *gb) {
  BASE(likelike_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = likelike_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; }
    if (target == b_+12) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+18); SET_HL(b_+23); // @defaultStates
  CYC(b_+18, b_+19); likelike_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+20); B = mem_rd(gb, HL);
  CYC(b_+20, b_+23); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp
}

// 0d:5d3d, bare global; jump-table target from enemyCode24.
void likelike_state_galeSeed_hook(GB *gb) {
  BASE(likelike_state_galeSeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_galeSeedEffect_b0d_hook, SYM(ecom_galeSeedEffect_b0d), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto decNum; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = A;
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

decNum:
  CALL_C(b_+14, decNumEnemies_hook, SYM(decNumEnemies), b_+17);
  CYC(b_+17, b_+20); enemyDelete_hook(gb); return; // jp
}

// 0d:5d51, bare global; jump-table target from enemyCode24.
void likelike_state_stub_hook(GB *gb) {
  BASE(likelike_state_stub);
  RET(b_+0); return; // ret
}

// 0d:5d52, bare global; jump-table target from enemyCode24@normalState.
void likelike_subid00_hook(GB *gb) {
  BASE(likelike_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = likelike_jump_table(gb);
    if (target == SYM(likelike_subid00_state8)) { likelike_subid00_state8_hook(gb); return; }
    if (target == SYM(likelike_state9)) { likelike_state9_hook(gb); return; }
    if (target == SYM(likelike_stateA)) { likelike_stateA_hook(gb); return; }
    if (target == SYM(likelike_stateB)) { likelike_stateB_hook(gb); return; }
    if (target == SYM(likelike_stateC)) { likelike_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5d60, bare global; jump-table target from likelike_subid00. Initialization; falls
// through into likelike_state9.
void likelike_subid00_state8_hook(GB *gb) {
  BASE(likelike_subid00_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  likelike_state9_hook(gb); return; // fallthrough
}

// 0d:5d67, bare global; jump-table target shared by likelike_subid00/02/03, also falls
// into from likelike_subid00_state8. Choosing a new direction & duration.
void likelike_state9_hook(GB *gb) {
  BASE(likelike_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+3, b_+6); SET_BC(0x1830);
  CALL_C(b_+6, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); A = 0x38;
  CYC(b_+17, b_+18); alu_add(gb, C);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); likelike_animate_hook(gb); return; // jr
}

// 0d:5d7c, bare global; jump-table target shared by likelike_subid00/02/03. Moving in some
// direction for [counter1] frames. Internal @newDirection/@move are @-local.
void likelike_stateA_hook(GB *gb) {
  BASE(likelike_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto move; } // jr nz
  CYC(b_+3, b_+5);

newDirection:
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); likelike_animate_hook(gb); return; // jr

move:
  CALL_C(b_+11, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+14);
  if (F & FZ) { CYCT(b_+14, b_+16); goto newDirection; } // jr z
  CYC(b_+14, b_+16);
  likelike_animate_hook(gb); return; // fallthrough
}

// 0d:5d8c, bare global; called from likelike_state9/stateA/stateB/stateC and their
// subid02/03 aliases.
void likelike_animate_hook(GB *gb) {
  BASE(likelike_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// 0d:5d8f, bare global; jump-table target shared by likelike_subid00/03. Eating Link.
// Internal @releaseLink is @-local; falls through into likelike_releaseLink.
void likelike_stateB_hook(GB *gb) {
  BASE(likelike_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto releaseLink; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); likelike_animate_hook(gb); return; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); L = alu_dec8(gb, L);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1]++
  CYC(b_+13, b_+15); likelike_animate_hook(gb); return; // jr

releaseLink:
  CYC(b_+15, b_+17); mem_wr(gb, HL, 60);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 19);
  if (!(F & FC)) { CYCT(b_+25, b_+27); goto afterShieldCheck; } // jr nc
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = 0x01; // TREASURE_SHIELD
  CALL_C(b_+29, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+32);
  if (!(F & FC)) { CYCT(b_+32, b_+34); goto afterShieldCheck; } // jr nc
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = 0x01; // TREASURE_SHIELD
  CALL_C(b_+36, loseTreasure_hook, SYM(loseTreasure), b_+39);
  CYC(b_+39, b_+42); SET_BC((SYM(enemyCode19) + 15)); // TX_510b
  CALL_C(b_+42, showText_hook, SYM(showText), b_+45);

afterShieldCheck:
  CALL_C(b_+45, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+48);
  CYC(b_+48, b_+50); alu_and(gb, 0x18);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CALL_C(b_+53, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), SYM(likelike_releaseLink));
  likelike_releaseLink_hook(gb); return; // fallthrough
}

// 0d:5dc7, bare global; called from enemyCode24 and likelike_checkHazards, also falls into
// from likelike_stateB.
void likelike_releaseLink_hook(GB *gb) {
  BASE(likelike_releaseLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link + OBJ_SUBSTATE);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x04);
  CYC(b_+5, b_+7); L = OBJ_COLLISION_TYPE; // <w1Link.collisionType
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+13); enemySetAnimation_hook(gb); return; // jp
}

// 0d:5dd4, bare global; jump-table target shared by likelike_subid00/02/03. Cooldown after
// eating Link.
void likelike_stateC_hook(GB *gb) {
  BASE(likelike_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto ranIntoWall; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_sub(gb, 0x03);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); // [state] -= 3
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+14, b_+16); likelike_animate_hook(gb); return; // jr

ranIntoWall:
  CALL_C(b_+16, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+19);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); likelike_animate_hook(gb); return; } // jr nz
  CYC(b_+19, b_+21);
  CALL_C(b_+21, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x18);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); likelike_animate_hook(gb); return; // jr
}

// 0d:5df3, bare global; jump-table target from enemyCode24@normalState. Like-like spawner.
// Internal @state8/@state9/@stateA/@fromBottom/@fromTop are @-local.
void likelike_subid01_hook(GB *gb) {
  BASE(likelike_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = likelike_jump_table(gb);
    if (target == b_+10) goto state8;
    if (target == b_+19) goto state9;
    if (target == b_+38) goto stateA;
    HANDOFF(target);
  }

state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+19); likelike_findAllLikelikesWithSubid0_hook(gb); return; // jp

state9:
  CYC(b_+19, b_+22); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(b_+22, b_+24); alu_sub(gb, 0x10);
  CYC(b_+24, b_+26); alu_cp(gb, 0x60); // (SMALL_ROOM_HEIGHT<<4)-$20
  if (!(F & FC)) { RET_TAKEN(b_+26); return; } // ret nc
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+30); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(b_+30, b_+32); alu_sub(gb, 0x10);
  CYC(b_+32, b_+34); alu_cp(gb, 0x80); // (SMALL_ROOM_WIDTH<<4)-$20
  if (!(F & FC)) { RET_TAKEN(b_+34); return; } // ret nc
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); A = 0x0a;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A); // [state] = $0a

stateA:
  CALL_C(b_+38, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+41);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; } // ret nz
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(b_+43, b_+45); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_cp(gb, 0x06);
  if (!(F & FC)) { RET_TAKEN(b_+48); return; } // ret nc
  CYC(b_+48, b_+49);
  CALL_C(b_+49, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+52);
  CYC(b_+52, b_+54); alu_and(gb, 0x02);
  CYC(b_+54, b_+55); C = A;
  CYC(b_+55, b_+58); A = mem_rd(gb, wActiveRoom);
  CYC(b_+58, b_+60); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+60, b_+62); goto fromTop; } // jr z
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); alu_cp(gb, 0x40);
  if (F & FZ) { CYCT(b_+64, b_+66); goto fromBottom; } // jr z
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); C = (uint8_t)(C | (1 << 2)); // set 2,c
  CYC(b_+68, b_+70); alu_cp(gb, 0x51);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; } // ret nz
  CYC(b_+70, b_+71);

fromBottom:
  CYC(b_+71, b_+73); E = 0x02;
  CALL_C(b_+73, likelike_spawn_hook, SYM(likelike_spawn), b_+76);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; } // ret nz
  CYC(b_+76, b_+77);
  CALL_C(b_+77, likelike_setChildSpawnPosition_hook, SYM(likelike_setChildSpawnPosition), b_+80);
  CYC(b_+80, b_+82); goto spawnedOk; // jr

fromTop:
  CYC(b_+82, b_+84); E = 0x03;
  CALL_C(b_+84, likelike_spawn_hook, SYM(likelike_spawn), b_+87);
  if (!(F & FZ)) { RET_TAKEN(b_+87); return; } // ret nz

spawnedOk:
  CYC(b_+88, b_+89); H = D;
  CYC(b_+89, b_+91); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+92, b_+94); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+94, b_+96); mem_wr(gb, HL, 120);
  RET(b_+96); return; // ret
}

// 0d:5e54, bare global; jump-table target from enemyCode24@normalState. Internal @state8/
// @state9 are @-local; other table entries alias likelike_state9/stateA/stateB/stateC.
void likelike_subid02_hook(GB *gb) {
  BASE(likelike_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = likelike_jump_table(gb);
    if (target == b_+16) goto state8;
    if (target == b_+35) goto state9;
    if (target == SYM(likelike_state9)) { likelike_state9_hook(gb); return; }
    if (target == SYM(likelike_stateA)) { likelike_stateA_hook(gb); return; }
    if (target == SYM(likelike_stateB)) { likelike_stateB_hook(gb); return; }
    if (target == SYM(likelike_stateC)) { likelike_stateC_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+18); L = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_cp(gb, 0x88); // (SMALL_ROOM_HEIGHT<<4)+8
  if (F & FZ) { CYCT(b_+24, b_+26); goto state8SetCounter; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT

state8SetCounter:
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 45);
  RET(b_+34); return; // ret

state9:
  CALL_C(b_+35, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+38);
  if (F & FZ) { CYCT(b_+38, b_+40); goto state9SetCollision; } // jr z
  CYC(b_+38, b_+40);
  CALL_C(b_+40, objectApplySpeed_hook, SYM(objectApplySpeed), b_+43);
  CYC(b_+43, b_+45); likelike_animate2_hook(gb); return; // jr

state9SetCollision:
  CYC(b_+45, b_+46); L = E;
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+49, b_+51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  likelike_animate2_hook(gb); return; // fallthrough
}

// 0d:5e87, bare global; called from likelike_subid02/03.
void likelike_animate2_hook(GB *gb) {
  BASE(likelike_animate2);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// 0d:5e8a, bare global; jump-table target from enemyCode24@normalState. Internal @state8/
// @state9/@stateB are @-local; other table entries alias likelike_state9/stateA/stateC.
void likelike_subid03_hook(GB *gb) {
  BASE(likelike_subid03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = likelike_jump_table(gb);
    if (target == b_+16) goto state8;
    if (target == b_+34) goto state9;
    if (target == SYM(likelike_state9)) { likelike_state9_hook(gb); return; }
    if (target == SYM(likelike_stateA)) { likelike_stateA_hook(gb); return; }
    if (target == b_+49) goto stateB;
    if (target == SYM(likelike_stateC)) { likelike_stateC_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CALL_C(b_+16, likelike_chooseRandomPosition_hook, SYM(likelike_chooseRandomPosition), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+25, b_+27); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_SPEED_Z + 1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x02);
  CYC(b_+31, b_+34); objectSetVisiblec1_hook(gb); return; // jp

state9:
  CYC(b_+34, b_+36); C = 0x08;
  CALL_C(b_+36, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+39);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); likelike_animate2_hook(gb); return; } // jr nz
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+44, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+47);
  CYC(b_+47, b_+49); likelike_animate2_hook(gb); return; // jr

stateB:
  CYC(b_+49, b_+51); C = 0x08;
  CALL_C(b_+51, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+54);
  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+60); mem_wr(gb, w1Link + OBJ_ZH, A);
  CYC(b_+60, b_+63); likelike_stateB_hook(gb); return; // jp
}

// 0d:5ec9, bare global; called from likelike_subid01. Spawner (subid 1) calls this to make
// new like-likes where their relatedObj1 references the spawner.
// @param e Subid of like-like to spawn
void likelike_spawn_hook(GB *gb) {
  BASE(likelike_spawn);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x24; // ENEMY_LIKE_LIKE
  CALL_C(b_+2, ecom_spawnEnemyWithSubid01_b0d_hook, SYM(ecom_spawnEnemyWithSubid01_b0d), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, E);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+9, b_+11); A = ENEMY_BASE; // Enemy.start
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+12, b_+13); mem_wr(gb, HL, D);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  RET(b_+14); return; // ret
}

// 0d:5ed8, bare global; called from likelike_subid01.
// @param c Index of spawn position to use
void likelike_setChildSpawnPosition_hook(GB *gb) {
  BASE(likelike_setChildSpawnPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, HL);
  CYC(b_+1, b_+2); A = C;
  CYC(b_+2, b_+5); SET_HL(b_+17); // @spawnPositions
  CYC(b_+5, b_+6); likelike_addAToHl_from_rst(gb, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  SET_HL(POP(b_+9));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+15, b_+16); mem_wr(gb, HL, C);
  RET(b_+16); return; // ret
}

// 0d:5ef1, bare global; called from likelike_subid01. Searches for all existing like-likes
// with subid 0, sets their relatedObj1 to point to this object (the spawner), and stores
// the current like-like count in var30.
void likelike_findAllLikelikesWithSubid0_hook(GB *gb) {
  BASE(likelike_findAllLikelikesWithSubid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(ENEMY_SLOTS + OBJ_ID); // FIRST_ENEMY_INDEX, Enemy.id
  CYC(b_+3, b_+5); C = 0x00;

nextEnemy:
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x24); // ENEMY_LIKE_LIKE
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto loopIncrement; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto loopIncrement; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+17, b_+19); A = ENEMY_BASE; // Enemy.start
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+20, b_+21); mem_wr(gb, HL, D);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+23, b_+24); C = alu_inc8(gb, C);

loopIncrement:
  CYC(b_+24, b_+25); H = alu_inc8(gb, H);
  CYC(b_+25, b_+26); A = H;
  CYC(b_+26, b_+28); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+28, b_+30); goto nextEnemy; } // jr c
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+32, b_+33); A = C;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return; // ret
}

// 0d:5f14, bare global; called from likelike_subid03. Choose a random position to fall
// from the sky. If a good position is chosen, the Z position is also set to be above the
// screen.
// @param[out] zflag z if chose valid position
void likelike_chooseRandomPosition_hook(GB *gb) {
  BASE(likelike_chooseRandomPosition);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x77);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); B = 0xce; // >wRoomCollisions
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CALL_C(b_+12, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+15);
  CYC(b_+15, b_+17); C = 0x08;
  CALL_C(b_+17, ecom_setZAboveScreen_b0d_hook, SYM(ecom_setZAboveScreen_b0d), b_+20);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  RET(b_+21); return; // ret
}

// 0d:5f2a, bare global; called from enemyCode24.
void likelike_checkHazards_hook(GB *gb) {
  BASE(likelike_checkHazards);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  PUSH(b_+0, AF);
  CYC(b_+1, b_+4); A = mem_rd(gb, w1Link + OBJ_STATE);
  CYC(b_+4, b_+6); alu_cp(gb, 0x0d); // LINK_STATE_GRABBED
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto restoreAf; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_rlca(gb);
  if (F & FC) { CYCT(b_+12, b_+14); goto restoreAf; } // jr c
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); SET_BC(0x0500);
  CALL_C(b_+17, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+20);
  CYC(b_+20, b_+23); SET_HL(hazardCollisionTable);
  CALL_C(b_+23, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+26);
  if (F & FC) CALL_C_CC(b_+26, likelike_releaseLink_hook, SYM(likelike_releaseLink), b_+29); else CYC(b_+26, b_+29); // call c

restoreAf:
  SET_AF(POP(b_+29));
  CYC(b_+30, b_+33); ecom_checkHazards_b0d_hook(gb); return; // jp
}
