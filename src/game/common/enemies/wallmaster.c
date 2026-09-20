#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void wallmaster_state_uninitialized_hook(GB *gb);
void wallmaster_state1_hook(GB *gb);
void wallmaster_state_galeSeed_hook(GB *gb);
void wallmaster_state_stub_hook(GB *gb);
void wallmaster_state8_hook(GB *gb);
void wallmaster_state9_hook(GB *gb);
void wallmaster_stateA_hook(GB *gb);
void wallmaster_stateB_hook(GB *gb);
void wallmaster_stateC_hook(GB *gb);
void wallmaster_stateD_hook(GB *gb);
void wallmaster_flickerVisibilityIfHighUp_hook(GB *gb);

static uint16_t wallmaster_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_WALLMASTER
//
// Variables:
//   relatedObj1: For actual wallmaster (subid 1): reference to spawner.
//   relatedObj2: For spawner (subid 0): reference to actual wallmaster.
//   var30: Nonzero if collided with Link (currently warping him out)
// ==================================================================================================
void enemyCode28_hook(GB *gb) {
  BASE(enemyCode28);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);

  // ENEMYSTATUS_JUST_HIT
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } // ret nz
  CYC(b_+16, b_+17);

  // Link just touched the hand. If not experiencing knockback, begin warping Link out.
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x01);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+34, b_+37); mem_wr(gb, w1Link + OBJ_YH, A);
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+42); mem_wr(gb, w1Link + OBJ_XH, A);
  RET(b_+42); return; // ret

dead:
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+47, b_+49); goto uncountedDie; } // jr z
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); H = A;
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x00);
  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

uncountedDie:
  CYC(b_+57, b_+60); enemyDie_uncounted_hook(gb); return; // jp

normalStatus:
  CYC(b_+60, b_+62); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = wallmaster_jump_table(gb);
    if (target == SYM(wallmaster_state_uninitialized)) { wallmaster_state_uninitialized_hook(gb); return; }
    if (target == SYM(wallmaster_state1)) { wallmaster_state1_hook(gb); return; }
    if (target == SYM(wallmaster_state_stub)) { wallmaster_state_stub_hook(gb); return; }
    if (target == SYM(wallmaster_state_galeSeed)) { wallmaster_state_galeSeed_hook(gb); return; }
    if (target == SYM(wallmaster_state8)) { wallmaster_state8_hook(gb); return; }
    if (target == SYM(wallmaster_state9)) { wallmaster_state9_hook(gb); return; }
    if (target == SYM(wallmaster_stateA)) { wallmaster_stateA_hook(gb); return; }
    if (target == SYM(wallmaster_stateB)) { wallmaster_stateB_hook(gb); return; }
    if (target == SYM(wallmaster_stateC)) { wallmaster_stateC_hook(gb); return; }
    if (target == SYM(wallmaster_stateD)) { wallmaster_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:6176, bare global; jump-table target from enemyCode28.
void wallmaster_state_uninitialized_hook(GB *gb) {
  BASE(wallmaster_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); ecom_setSpeedAndState8_b0d_hook(gb); return; } // jp nz
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 180);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+17, b_+19); mem_wr(gb, HL, ENEMY_BASE); // Enemy.start
  RET(b_+19); return; // ret
}

// 0d:618a, bare global; jump-table target from enemyCode28. Subid 0 (wallmaster spawner)
// stays in this state indefinitely; spawns a wallmaster every 2 seconds.
void wallmaster_state1_hook(GB *gb) {
  BASE(wallmaster_state1);
  uint16_t sp0_ = gb->sp;
  // "yh" acts as the number of wallmasters remaining to spawn, for the spawner.
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto deleteSpawner; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 120);
  CYC(b_+17, b_+20); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+24); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(b_+24, b_+25); C = A;
  CALL_C(b_+25, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; } // ret nz
  CYC(b_+28, b_+29);
  PUSH(b_+29, BC);
  CYC(b_+30, b_+32); B = 0x28; // ENEMY_WALLMASTER
  CALL_C(b_+32, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+35);
  SET_BC(POP(b_+35));
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+39, b_+41); A = ENEMY_BASE; // Enemy.start
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+42, b_+43); mem_wr(gb, HL, D);
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+45, b_+46); A = H;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  RET(b_+47); return; // ret

deleteSpawner:
  CALL_C(b_+48, decNumEnemies_hook, SYM(decNumEnemies), b_+51);
  CALL_C(b_+51, markEnemyAsKilledInRoom_b00_hook, SYM(markEnemyAsKilledInRoom_b00), b_+54);
  CYC(b_+54, b_+57); enemyDelete_hook(gb); return; // jp
}

// 0d:61c3, bare global; jump-table target from enemyCode28.
void wallmaster_state_galeSeed_hook(GB *gb) {
  BASE(wallmaster_state_galeSeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_galeSeedEffect_b0d_hook, SYM(ecom_galeSeedEffect_b0d), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);

  // Tell spawner that this wallmaster is dead
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto deleteMe; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = A;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);

deleteMe:
  CYC(b_+15, b_+18); enemyDelete_hook(gb); return; // jp
}

// 0d:61d5, bare global; jump-table target from enemyCode28.
void wallmaster_state_stub_hook(GB *gb) {
  BASE(wallmaster_state_stub);
  RET(b_+0); return; // ret
}

// 0d:61d6, bare global; jump-table target from enemyCode28. Spawning at Link's position,
// above the screen.
void wallmaster_state8_hook(GB *gb) {
  BASE(wallmaster_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x80 | 0x35); // ENEMY_FLOORMASTER

  // Copy Link's position, set high Z position
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xa0);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+13, b_+16); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+21); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); A = 0x59; // SND_FALLINHOLE
  CALL_C(b_+24, playSound_b00_hook, SYM(playSound_b00), b_+27);
  CYC(b_+27, b_+30); objectSetVisiblec1_hook(gb); return; // jp
}

// 0d:61f4, bare global; jump-table target from enemyCode28. Falling to ground.
void wallmaster_state9_hook(GB *gb) {
  BASE(wallmaster_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x0e;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto hitGround; } // jr z
  CYC(b_+5, b_+7);
  CALL_C(b_+7, wallmaster_flickerVisibilityIfHighUp_hook, SYM(wallmaster_flickerVisibilityIfHighUp), b_+10);

  // Check for collision with Link
  CYC(b_+10, b_+12); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+14); return; } // ret z
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+21); mem_wr(gb, w1Link + OBJ_ZH, A);
  RET(b_+21); return; // ret

hitGround:
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 30);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+29); return; // ret
}

// 0d:6212, bare global; jump-table target from enemyCode28. Waiting on ground for
// [counter1] frames before moving back up.
void wallmaster_stateA_hook(GB *gb) {
  BASE(wallmaster_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto checkCounter; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  RET(b_+7); return; // ret

checkCounter:
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 20); // [counter1] == 20?
  if (F & FC) { CYCT(b_+11, b_+13); goto checkAlmostDone; } // jr c
  CYC(b_+11, b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);

  // Close hand when [counter1] == 20
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+19); enemySetAnimation_hook(gb); return; // jp

checkAlmostDone:
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto checkPulledLink; } // jr nz
  CYC(b_+20, b_+22);

  // Set collisionType when [counter1] == 1
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_and(gb, 0x80);
  CYC(b_+27, b_+29); alu_or(gb, 0x28); // ENEMY_WALLMASTER
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);

checkPulledLink:
  CYC(b_+30, b_+32); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+32, b_+34); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+34); return; } // ret z
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CYC(b_+36, b_+39); mem_wr(gb, w1Link + OBJ_VISIBLE, A);
  RET(b_+39); return; // ret
}

// 0d:623a, bare global; jump-table target from enemyCode28. Moving back up.
void wallmaster_stateB_hook(GB *gb) {
  BASE(wallmaster_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, wallmaster_flickerVisibilityIfHighUp_hook, SYM(wallmaster_flickerVisibilityIfHighUp), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0xa0);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; } // ret nc
  CYC(b_+11, b_+12);

  // Moved high enough
  CALL_C(b_+12, objectSetInvisible_hook, SYM(objectSetInvisible), b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+19); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+19, b_+21); goto notPulledLink; } // jr z
  CYC(b_+19, b_+21);

  // We just pulled Link out, go to state $0d
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x0d);
  RET(b_+25); return; // ret

notPulledLink:
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 120);
  RET(b_+37); return; // ret
}

// 0d:6260, bare global; jump-table target from enemyCode28. Waiting off-screen until time
// to attack again.
void wallmaster_stateC_hook(GB *gb) {
  BASE(wallmaster_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08); // [state] = 8
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+15); enemySetAnimation_hook(gb); return; // jp
}

// 0d:626f, bare global; jump-table target from enemyCode28. Just dragged Link off-screen.
void wallmaster_stateD_hook(GB *gb) {
  BASE(wallmaster_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); mem_wr(gb, w1Link + OBJ_SUBSTATE, A);
  RET(b_+5); return; // ret
}

// 0d:6275, bare global; called from wallmaster_state9 and wallmaster_stateB. Flickers
// visibility if very high up (zh < $b8).
void wallmaster_flickerVisibilityIfHighUp_hook(GB *gb) {
  BASE(wallmaster_flickerVisibilityIfHighUp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); alu_cp(gb, 0xb8);
  if (F & FC) { CYCT(b_+7, b_+10); ecom_flickerVisibility_b0d_hook(gb); return; } // jp c
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0xbc);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; } // ret nc
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); objectSetVisiblec1_hook(gb); return; // jp
}
