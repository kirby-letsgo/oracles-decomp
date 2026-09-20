#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void beetle_state_uninitialized_hook(GB *gb);
void beetle_state_spawner_hook(GB *gb);
void beetle_state_galeSeed_hook(GB *gb);
void beetle_state_switchHook_hook(GB *gb);
void beetle_state_stub_hook(GB *gb);
void beetle_subid1_hook(GB *gb);
void beetle_stateA_hook(GB *gb);
void beetle_animate_hook(GB *gb);
void beetle_subid2_hook(GB *gb);
void beetle_subid3_hook(GB *gb);
void beetle_chooseRandomAngleAndCounter1_hook(GB *gb);
void beetle_checkHazards_hook(GB *gb);

static uint16_t beetle_jump_table(GB *gb) {
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

static void beetle_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_BEETLE
//
// Variables for spawner (subid 0):
//   var30: Number of beetles spawned in? It's never actually used, and it doesn't seem to
//          update correctly, so this was probably for some abandoned idea.
//
// Variables for actual beetles (subid 1+):
//   relatedObj1: Reference to spawner object (optional)
// ==================================================================================================
void enemyCode51_hook(GB *gb) {
  BASE(enemyCode51);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, beetle_checkHazards_hook, SYM(beetle_checkHazards), b_+3);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto normalStatus; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+8); return; } // ret c
  CYC(b_+8, b_+9);
  if (F & FZ) { CYCT(b_+9, b_+11); goto dead; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { RET_TAKEN(b_+26); return; } // ret z
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x0a);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x01);
  RET(b_+36); return; // ret

dead:
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto die; } // jr nz
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+47, b_+49); goto die; } // jr z
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); A = 0x30; // Object.var30
  CALL_C(b_+51, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+54);
  CYC(b_+54, b_+55); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

die:
  CYC(b_+55, b_+58); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+58, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+61);
  if (!(F & FC)) { CYCT(b_+61, b_+63); goto normalState; } // jr nc
  CYC(b_+61, b_+63);
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = beetle_jump_table(gb);
    if (target == SYM(beetle_state_uninitialized)) { beetle_state_uninitialized_hook(gb); return; }
    if (target == SYM(beetle_state_spawner)) { beetle_state_spawner_hook(gb); return; }
    if (target == SYM(beetle_state_stub)) { beetle_state_stub_hook(gb); return; }
    if (target == SYM(beetle_state_switchHook)) { beetle_state_switchHook_hook(gb); return; }
    if (target == SYM(beetle_state_galeSeed)) { beetle_state_galeSeed_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+80, b_+81); B = alu_dec8(gb, B);
  CYC(b_+81, b_+82); A = B;
  {
    CYC(b_+82, b_+83); push_effect(gb, b_+83);
    uint16_t target = beetle_jump_table(gb);
    if (target == SYM(beetle_subid1)) { beetle_subid1_hook(gb); return; }
    if (target == SYM(beetle_subid2)) { beetle_subid2_hook(gb); return; }
    if (target == SYM(beetle_subid3)) { beetle_subid3_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:643b, bare global; jump-table target from enemyCode51. Falls through into
// beetle_state_spawner.
void beetle_state_uninitialized_hook(GB *gb) {
  BASE(beetle_state_uninitialized);
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  CYC(b_+2, b_+4); A = 0x14; // SPEED_80
  if (!(F & FZ)) { CYCT(b_+4, b_+7); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp nz
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); A = 0x01;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A); // [state] = 1
  beetle_state_spawner_hook(gb); return; // fallthrough
}

// 0e:6445, bare global; jump-table target from enemyCode51, also falls into from
// beetle_state_uninitialized.
void beetle_state_spawner_hook(GB *gb) {
  BASE(beetle_state_spawner);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); C = 0x20;
  CALL_C(b_+6, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+9);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; } // ret nc
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+12, b_+14); A = 90;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); B = 0x51; // ENEMY_BEETLE
  CALL_C(b_+17, ecom_spawnEnemyWithSubid01_b0e_hook, SYM(ecom_spawnEnemyWithSubid01_b0e), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 2
  CYC(b_+22, b_+25); objectCopyPosition_hook(gb); return; // jp
}

// 0e:645e, bare global; jump-table target from enemyCode51.
void beetle_state_galeSeed_hook(GB *gb) {
  BASE(beetle_state_galeSeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_galeSeedEffect_b0e_hook, SYM(ecom_galeSeedEffect_b0e), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto skip; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = A;
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

skip:
  CALL_C(b_+14, decNumEnemies_hook, SYM(decNumEnemies), b_+17);
  CYC(b_+17, b_+20); enemyDelete_hook(gb); return; // jp
}

// 0e:6472, bare global; jump-table target from enemyCode51.
void beetle_state_switchHook_hook(GB *gb) {
  BASE(beetle_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = beetle_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; } // ret (substate1/substate2)
    if (target == b_+12) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(b_+12, b_+14); B = 0x0a;
  CYC(b_+14, b_+17); ecom_fallToGroundAndSetState_b0e_hook(gb); return; // jp
}

// 0e:6483, bare global; jump-table target from enemyCode51.
void beetle_state_stub_hook(GB *gb) {
  BASE(beetle_state_stub);
  RET(b_+0); return; // ret
}

// 0e:6484, bare global; jump-table target from enemyCode51@normalState. Falls from the sky.
void beetle_subid1_hook(GB *gb) {
  BASE(beetle_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = beetle_jump_table(gb);
    if (target == b_+10) goto state8;
    if (target == b_+30) goto state9;
    if (target == SYM(beetle_stateA)) { beetle_stateA_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+17, b_+19); C = 0x08;
  CALL_C(b_+19, ecom_setZAboveScreen_b0e_hook, SYM(ecom_setZAboveScreen_b0e), b_+22);
  CALL_C(b_+22, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+25);
  CYC(b_+25, b_+27); A = 0x59; // SND_FALLINHOLE
  CYC(b_+27, b_+30); playSound_b00_hook(gb); return; // jp

state9:
  CYC(b_+30, b_+32); C = 0x0e;
  CALL_C(b_+32, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; } // ret nz
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+43, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+46);
  CYC(b_+46, b_+48); A = 0x52; // SND_BOMB_LAND
  CALL_C(b_+48, playSound_b00_hook, SYM(playSound_b00), b_+51);
  CALL_C(b_+51, beetle_chooseRandomAngleAndCounter1_hook, SYM(beetle_chooseRandomAngleAndCounter1), b_+54);
  CYC(b_+54, b_+56); beetle_animate_hook(gb); return; // jr
}

// 0e:64bc, bare global; jump-table target from beetle_subid1/beetle_subid2/beetle_subid3.
// Common beetle state. Falls through into beetle_animate.
void beetle_stateA_hook(GB *gb) {
  BASE(beetle_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) CALL_C_CC(b_+3, beetle_chooseRandomAngleAndCounter1_hook, SYM(beetle_chooseRandomAngleAndCounter1), b_+6); else CYC(b_+3, b_+6); // call z
  CALL_C(b_+6, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), SYM(beetle_animate));
  beetle_animate_hook(gb); return; // fallthrough
}

// 0e:64c5, bare global; falls into from beetle_stateA, also reached by genuine jr from
// beetle_subid1/beetle_subid2.
void beetle_animate_hook(GB *gb) {
  BASE(beetle_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// 0e:64c8, bare global; jump-table target from enemyCode51@normalState. Spawns in instantly.
void beetle_subid2_hook(GB *gb) {
  BASE(beetle_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = beetle_jump_table(gb);
    if (target == b_+10) goto state8;
    if (target == b_+23) goto state9;
    if (target == SYM(beetle_stateA)) { beetle_stateA_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+17, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+20);
  CYC(b_+20, b_+23); objectSetVisiblec2_hook(gb); return; // jp

state9:
  CALL_C(b_+23, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto keepMovingTowardLink; } // jr nz
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(b_+29, b_+30); L = E;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+31, b_+33); beetle_stateA_hook(gb); return; // jr

keepMovingTowardLink:
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_cp(gb, 22);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto applyVelocity; } // jr nz
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+40, b_+42); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity:
  CALL_C(b_+42, ecom_applyVelocityForSideviewEnemy_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0e), b_+45);
  CYC(b_+45, b_+47); beetle_animate_hook(gb); return; // jr
}

// 0e:64f7, bare global; jump-table target from enemyCode51@normalState. "Bounces in" when it
// spawns (dug up from the ground).
void beetle_subid3_hook(GB *gb) {
  BASE(beetle_subid3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = beetle_jump_table(gb);
    if (target == b_+10) goto state8;
    if (target == b_+36) goto state9;
    if (target == SYM(beetle_stateA)) { beetle_stateA_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+15, b_+17); A = 0xfe; // <(-$102)
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0xfe); // >(-$102)
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+29); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+32); alu_rrca(gb);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+36); objectSetVisiblec2_hook(gb); return; // jp

state9:
  CYC(b_+36, b_+38); C = 0x0e;
  CALL_C(b_+38, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+41);
  if (F & FC) { CYCT(b_+41, b_+43); goto doneBouncing; } // jr c
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x52; // SND_BOMB_LAND
  if (F & FZ) CALL_C_CC(b_+45, playSound_b00_hook, SYM(playSound_b00), b_+48); else CYC(b_+45, b_+48); // call z
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_SPEED_Z + 1; // Enemy.speedZ+1
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+52, b_+54); goto applyVelocity; } // jr nz
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+57, b_+59); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity:
  CYC(b_+59, b_+62); ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(gb); return; // jp

doneBouncing:
  CALL_C(b_+62, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+65);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x14); // SPEED_80
  beetle_chooseRandomAngleAndCounter1_hook(gb); return; // fallthrough
}

// 0e:653c, bare global; called from beetle_stateA and beetle_subid1, also falls into from
// beetle_subid3.
void beetle_chooseRandomAngleAndCounter1_hook(GB *gb) {
  BASE(beetle_chooseRandomAngleAndCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x071c);
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+14); SET_HL(b_+20); // @counter1Vals
  CYC(b_+14, b_+15); beetle_addAToHl_from_rst(gb, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  RET(b_+19); return; // ret
}

// 0e:6558, bare global; called from enemyCode51. Beetle has custom checkHazards function so it
// can decrease the spawner's var30 (number of spawned).
void beetle_checkHazards_hook(GB *gb) {
  BASE(beetle_checkHazards);
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); alu_cp(gb, 0x0a);
  CYC(b_+6, b_+7); A = B;
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+11, b_+13); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+13, b_+15); goto checkHazards; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_cp(gb, 0x3b); // 59
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto checkHazards; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+26, b_+28); goto checkHazards; } // jr z
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); H = A;
  CYC(b_+29, b_+31); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

checkHazards:
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+36); ecom_checkHazards_b0e_hook(gb); return; // jp
}
