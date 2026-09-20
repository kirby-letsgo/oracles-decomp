#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void armos_uninitialized_hook(GB *gb);
void armos_state1_hook(GB *gb);
void armos_state_switchHook_hook(GB *gb);
void armos_state_stub_hook(GB *gb);
void armos_subid00_hook(GB *gb);
void armos_subid00_state8_hook(GB *gb);
void armos_state9_hook(GB *gb);
void armos_subid00_stateA_hook(GB *gb);
void armos_beginMoving_hook(GB *gb);
void armos_subid00_stateB_hook(GB *gb);
void armos_subid00_stateC_hook(GB *gb);
void armos_subid01_hook(GB *gb);
void armos_subid01_state8_hook(GB *gb);
void armos_subid01_stateA_hook(GB *gb);
void armos_subid02_stateB_hook(GB *gb);
void armos_subid03_stateC_hook(GB *gb);
void armos_spawnArmosAtPosition_hook(GB *gb);
void armos_dead_hook(GB *gb);
void armos_clearKilledArmosBuffer_hook(GB *gb);
void armos_replaceTileUnderSelf_hook(GB *gb);

static uint16_t armos_jump_table(GB *gb) {
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

static void armos_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_ARMOS
//
// Variables:
//   subid: If bit 7 is set, it's a real armos; otherwise it's an armos spawner.
//   var31: The initial position of the armos (subid 1 only)
// ==================================================================================================
void enemyCode1d_hook(GB *gb) {
  BASE(enemyCode1d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); armos_dead_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto normalStatus; } // jr nz
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+34, b_+36); goto normalStatus; } // jr nc
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x09);
  RET(b_+38); return; // ret

normalStatus:
  CALL_C(b_+39, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+42);
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto normalState; } // jr nc
  CYC(b_+42, b_+44);
  {
    CYC(b_+44, b_+45); push_effect(gb, b_+45);
    uint16_t target = armos_jump_table(gb);
    if (target == SYM(armos_uninitialized)) { armos_uninitialized_hook(gb); return; }
    if (target == SYM(armos_state1)) { armos_state1_hook(gb); return; }
    if (target == SYM(armos_state_stub)) { armos_state_stub_hook(gb); return; }
    if (target == SYM(armos_state_switchHook)) { armos_state_switchHook_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+61, b_+63); B = (uint8_t)(B & ~(1 << 7)); // res 7,b
  CYC(b_+63, b_+64); A = B;
  {
    CYC(b_+64, b_+65); push_effect(gb, b_+65);
    uint16_t target = armos_jump_table(gb);
    if (target == SYM(armos_subid00)) { armos_subid00_hook(gb); return; }
    if (target == SYM(armos_subid01)) { armos_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:58f1, bare global; jump-table target from enemyCode1d.
void armos_uninitialized_hook(GB *gb) {
  BASE(armos_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+3, b_+5); goto gotoState1; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); alu_add(gb, A); // add a
  CYC(b_+6, b_+9); SET_HL(b_+38); // @oamFlagsAndSpeeds
  CYC(b_+9, b_+10); armos_addAToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); E = alu_dec8(gb, E);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CALL_C(b_+17, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+20);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+24); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+24, b_+26); goto noExtraHealth; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

noExtraHealth:
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  RET(b_+33); return; // ret

gotoState1:
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  RET(b_+37); return; // ret
}

// 0d:591b, bare global; jump-table target from enemyCode1d. For subid where bit 7 isn't
// set: spawn armos at all positions where their tiles are. (Enemy.yh currently contains
// the tile to replace, Enemy.xh is the new tile it becomes.)
void armos_state1_hook(GB *gb) {
  BASE(armos_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); SET_HL(wRoomLayout);
  CYC(b_+7, b_+9); C = 0xb0; // LARGE_ROOM_HEIGHT<<4

scanTiles:
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_cp(gb, B);
  if (F & FZ) CALL_C_CC(b_+11, armos_spawnArmosAtPosition_hook, SYM(armos_spawnArmosAtPosition), b_+14); else CYC(b_+11, b_+14); // call z
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto scanTiles; } // jr nz
  CYC(b_+16, b_+18);
  CALL_C(b_+18, armos_clearKilledArmosBuffer_hook, SYM(armos_clearKilledArmosBuffer), b_+21);
  CALL_C(b_+21, decNumEnemies_hook, SYM(decNumEnemies), b_+24);
  CYC(b_+24, b_+27); enemyDelete_hook(gb); return; // jp
}

// 0d:5936, bare global; jump-table target from enemyCode1d.
void armos_state_switchHook_hook(GB *gb) {
  BASE(armos_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = armos_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; } // ret (substate1/substate2)
    if (target == b_+12) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(b_+12, b_+14); B = 0x0b;
  CYC(b_+14, b_+17); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp
}

// 0d:5947, bare global; jump-table target from enemyCode1d.
void armos_state_stub_hook(GB *gb) {
  BASE(armos_state_stub);
  RET(b_+0); return; // ret
}

// 0d:5948, bare global; jump-table target from enemyCode1d@normalState.
void armos_subid00_hook(GB *gb) {
  BASE(armos_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = armos_jump_table(gb);
    if (target == SYM(armos_subid00_state8)) { armos_subid00_state8_hook(gb); return; }
    if (target == SYM(armos_state9)) { armos_state9_hook(gb); return; }
    if (target == SYM(armos_subid00_stateA)) { armos_subid00_stateA_hook(gb); return; }
    if (target == SYM(armos_subid00_stateB)) { armos_subid00_stateB_hook(gb); return; }
    if (target == SYM(armos_subid00_stateC)) { armos_subid00_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5956, bare global; jump-table target from armos_subid00, also reached by genuine call
// from armos_subid01_state8. Waiting for Link to touch the statue (or for the wcca2 trigger).
void armos_subid00_state8_hook(GB *gb) {
  BASE(armos_subid00_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcca2);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x09;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return; // ret
}

// 0d:595f, bare global; jump-table target shared by armos_subid00 and armos_subid01.
// The statue was just activated.
void armos_state9_hook(GB *gb) {
  BASE(armos_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 60);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+14); objectSetVisible82_hook(gb); return; // jp
}

// 0d:596d, bare global; jump-table target from armos_subid00. Flickering until it starts
// moving; falls through into armos_beginMoving.
void armos_subid00_stateA_hook(GB *gb) {
  BASE(armos_subid00_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); A = GV(0x1e, 0x1d); // ENEMYCOLLISION_ACTIVE_RED_ARMOS
  armos_beginMoving_hook(gb); return; // fallthrough
}

// 0d:5975, bare global; falls into from armos_subid00_stateA, also reached by genuine jp
// from armos_subid01_stateA.
// @param a EnemyCollisionMode
void armos_beginMoving_hook(GB *gb) {
  BASE(armos_beginMoving);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); L = E;
  CYC(b_+1, b_+2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  CYC(b_+2, b_+4); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x80 | 0x1d); // ENEMY_ARMOS
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- set enemyCollisionMode
  CYC(b_+8, b_+10); A = 0x06;
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CALL_C(b_+12, armos_replaceTileUnderSelf_hook, SYM(armos_replaceTileUnderSelf), b_+15);
  CYC(b_+15, b_+18); objectSetVisiblec2_hook(gb); return; // jp
}

// 0d:5987, bare global; jump-table target from armos_subid00. Choose a direction to move;
// falls through into armos_subid00_stateC.
void armos_subid00_stateB_hook(GB *gb) {
  BASE(armos_subid00_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 61);
  CALL_C(b_+7, ecom_setRandomCardinalAngle_b0d_hook, SYM(ecom_setRandomCardinalAngle_b0d), SYM(armos_subid00_stateC));
  armos_subid00_stateC_hook(gb); return; // fallthrough
}

// 0d:5991, bare global; jump-table target from armos_subid00, also falls into from
// armos_subid00_stateB. Moving in some direction for [counter1] frames.
void armos_subid00_stateC_hook(GB *gb) {
  BASE(armos_subid00_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) CALL_C_CC(b_+3, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+6); else CYC(b_+3, b_+6); // call nz
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto stillMoving; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+12); A = 0x0b;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);

stillMoving:
  CYC(b_+13, b_+16); enemyAnimate_hook(gb); return; // jp
}

// 0d:59a1, bare global; jump-table target from armos_subid00_hook... actually from
// enemyCode1d@normalState.
void armos_subid01_hook(GB *gb) {
  BASE(armos_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = armos_jump_table(gb);
    if (target == SYM(armos_subid01_state8)) { armos_subid01_state8_hook(gb); return; }
    if (target == SYM(armos_state9)) { armos_state9_hook(gb); return; }
    if (target == SYM(armos_subid01_stateA)) { armos_subid01_stateA_hook(gb); return; }
    if (target == SYM(armos_subid02_stateB)) { armos_subid02_stateB_hook(gb); return; }
    if (target == SYM(armos_subid03_stateC)) { armos_subid03_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:59af, bare global; jump-table target from armos_subid01. Waiting for Link to approach
// the statue.
void armos_subid01_state8_hook(GB *gb) {
  BASE(armos_subid01_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, armos_subid00_state8_hook, SYM(armos_subid00_state8), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+7, b_+9); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+9, b_+10); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+12); alu_add(gb, 0x18);
  CYC(b_+12, b_+14); alu_cp(gb, 0x31);
  if (!(F & FC)) { RET_TAKEN(b_+14); return; } // ret nc
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); B = mem_rd(gb, HL);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+18, b_+20); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+20, b_+21); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+23); alu_add(gb, 0x18);
  CYC(b_+23, b_+25); alu_cp(gb, 0x31);
  if (!(F & FC)) { RET_TAKEN(b_+25); return; } // ret nc
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_and(gb, 0xf0);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+32); C = A;
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+35); alu_and(gb, 0xf0);
  CYC(b_+35, b_+36); alu_or(gb, C);
  CYC(b_+36, b_+38); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+40); L = E;
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  RET(b_+41); return; // ret
}

// 0d:59d9, bare global; jump-table target from armos_subid01. Flickering until it starts
// moving.
void armos_subid01_stateA_hook(GB *gb) {
  BASE(armos_subid01_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); A = GV(0x54, 0x51); // ENEMYCOLLISION_ACTIVE_BLUE_ARMOS
  CYC(b_+8, b_+11); armos_beginMoving_hook(gb); return; // jp
}

// 0d:59e4, bare global; jump-table target from armos_subid01. Choose random new direction
// & amount of time to move in that direction.
void armos_subid02_stateB_hook(GB *gb) {
  BASE(armos_subid02_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0c;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state] = $0c
  CYC(b_+3, b_+6); SET_BC(0x0303);
  CALL_C(b_+6, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+9);
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13); SET_HL(b_+26); // @counter1Vals
  CYC(b_+13, b_+14); armos_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); A = C;
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+20, b_+23); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb); return; } // jp z
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+26); ecom_setRandomCardinalAngle_b0d_hook(gb); return; // jp
}

// 0d:5a02, bare global; jump-table target from armos_subid01. Moving in some direction for
// [counter1] frames.
void armos_subid03_stateC_hook(GB *gb) {
  BASE(armos_subid03_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto stoppedMoving; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto stoppedMoving; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); enemyAnimate_hook(gb); return; // jp

stoppedMoving:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+17); A = 0x0b;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return; // ret
}

// 0d:5a15, bare global; called from armos_state1 for each matching tile position.
// @param l Position to spawn at
void armos_spawnArmosAtPosition_hook(GB *gb) {
  BASE(armos_spawnArmosAtPosition);
  uint16_t sp0_ = gb->sp;
  PUSH(b_+0, BC);
  PUSH(b_+1, HL);
  CYC(b_+2, b_+3); C = L;
  CYC(b_+3, b_+5); B = 0x1d; // ENEMY_ARMOS
  CALL_C(b_+5, ecom_spawnEnemyWithSubid01_b0d_hook, SYM(ecom_spawnEnemyWithSubid01_b0d), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto spawnRet; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); E = L;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); A = (uint8_t)(A | (1 << 7)); // set 7,a
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); // [child.subid] = [this.subid]|$80
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+22); L = E;
  CYC(b_+22, b_+23); A = C;
  CYC(b_+23, b_+25); alu_and(gb, 0x0f);
  CYC(b_+25, b_+27); A = alu_swap(gb, A);
  CYC(b_+27, b_+29); alu_add(gb, 0x08);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+30, b_+31); L = alu_dec8(gb, L);
  CYC(b_+31, b_+32); A = C;
  CYC(b_+32, b_+34); alu_and(gb, 0xf0);
  CYC(b_+34, b_+36); alu_add(gb, 0x06);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);

spawnRet:
  SET_HL(POP(b_+37));
  SET_BC(POP(b_+38));
  RET(b_+39); return; // ret
}

// 0d:5a3d, bare global; called from enemyCode1d.
void armos_dead_hook(GB *gb) {
  BASE(armos_dead);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+7); enemyDie_hook(gb); return; } // jp nc
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions - 1);

findFreeSlot:
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto findFreeSlot; } // jr nz
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, B);
  CYC(b_+20, b_+23); enemyDie_hook(gb); return; // jp
}

// 0d:5a54, bare global; called from armos_state1.
void armos_clearKilledArmosBuffer_hook(GB *gb) {
  BASE(armos_clearKilledArmosBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+6); B = 0x04;

clearLoop:
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+10, b_+11); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto clearLoop; } // jr nz
  CYC(b_+11, b_+13);
  RET(b_+13); return; // ret
}

// 0d:5a62, bare global; called from armos_beginMoving. Replace the tile underneath the
// armos with [var30].
void armos_replaceTileUnderSelf_hook(GB *gb) {
  BASE(armos_replaceTileUnderSelf);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+4); C = L;
  CYC(b_+4, b_+6); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+10); setTile_hook(gb); return; // jp
}
