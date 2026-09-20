#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x611a, 0x611c); goto normalStatus; } // jr z
  CYC(0x611a, 0x611c);
  CYC(0x611c, 0x611e); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(0x611e); return; } // ret c
  CYC(0x611e, 0x611f);
  if (F & FZ) { CYCT(0x611f, 0x6121); goto dead; } // jr z
  CYC(0x611f, 0x6121);
  CYC(0x6121, 0x6122); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6122, 0x6125); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(0x6122, 0x6125);

  // ENEMYSTATUS_JUST_HIT
  CYC(0x6125, 0x6127); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6127, 0x6128); A = mem_rd(gb, DE);
  CYC(0x6128, 0x612a); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(0x612a); return; } // ret nz
  CYC(0x612a, 0x612b);

  // Link just touched the hand. If not experiencing knockback, begin warping Link out.
  CYC(0x612b, 0x612d); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x612d, 0x612e); A = mem_rd(gb, DE);
  CYC(0x612e, 0x612f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x612f); return; } // ret nz
  CYC(0x612f, 0x6130);
  CYC(0x6130, 0x6131); H = D;
  CYC(0x6131, 0x6133); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6133, 0x6135); mem_wr(gb, HL, 0x01);
  CYC(0x6135, 0x6137); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6137, 0x6139); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6139, 0x613b); L = ENEMY_BASE + OBJ_YH;
  CYC(0x613b, 0x613c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(0x613c, 0x613f); mem_wr(gb, w1Link + OBJ_YH, A);
  CYC(0x613f, 0x6140); L = alu_inc8(gb, L);
  CYC(0x6140, 0x6141); A = mem_rd(gb, HL);
  CYC(0x6141, 0x6144); mem_wr(gb, w1Link + OBJ_XH, A);
  RET(0x6144); return; // ret

dead:
  CYC(0x6145, 0x6147); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x6147, 0x6148); A = mem_rd(gb, DE);
  CYC(0x6148, 0x6149); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6149, 0x614b); goto uncountedDie; } // jr z
  CYC(0x6149, 0x614b);
  CYC(0x614b, 0x614c); H = A;
  CYC(0x614c, 0x614e); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x614e, 0x6150); mem_wr(gb, HL, 0x00);
  CYC(0x6150, 0x6152); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6152, 0x6153); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

uncountedDie:
  CYC(0x6153, 0x6156); enemyDie_uncounted_hook(gb); return; // jp

normalStatus:
  CYC(0x6156, 0x6158); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6158, 0x6159); A = mem_rd(gb, DE);
  {
    CYC(0x6159, 0x615a); push_effect(gb, 0x615a);
    uint16_t target = wallmaster_jump_table(gb);
    if (target == 0x6176) { wallmaster_state_uninitialized_hook(gb); return; }
    if (target == 0x618a) { wallmaster_state1_hook(gb); return; }
    if (target == 0x61d5) { wallmaster_state_stub_hook(gb); return; }
    if (target == 0x61c3) { wallmaster_state_galeSeed_hook(gb); return; }
    if (target == 0x61d6) { wallmaster_state8_hook(gb); return; }
    if (target == 0x61f4) { wallmaster_state9_hook(gb); return; }
    if (target == 0x6212) { wallmaster_stateA_hook(gb); return; }
    if (target == 0x623a) { wallmaster_stateB_hook(gb); return; }
    if (target == 0x6260) { wallmaster_stateC_hook(gb); return; }
    if (target == 0x626f) { wallmaster_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:6176, bare global; jump-table target from enemyCode28.
void wallmaster_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6176, 0x6178); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6178, 0x6179); A = mem_rd(gb, DE);
  CYC(0x6179, 0x617a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x617a, 0x617d); ecom_setSpeedAndState8_b0d_hook(gb); return; } // jp nz
  CYC(0x617a, 0x617d);
  CYC(0x617d, 0x617e); H = D;
  CYC(0x617e, 0x6180); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6180, 0x6181); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6181, 0x6183); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6183, 0x6185); mem_wr(gb, HL, 180);
  CYC(0x6185, 0x6187); L = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x6187, 0x6189); mem_wr(gb, HL, ENEMY_BASE); // Enemy.start
  RET(0x6189); return; // ret
}

// 0d:618a, bare global; jump-table target from enemyCode28. Subid 0 (wallmaster spawner)
// stays in this state indefinitely; spawns a wallmaster every 2 seconds.
void wallmaster_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // "yh" acts as the number of wallmasters remaining to spawn, for the spawner.
  CYC(0x618a, 0x618c); E = ENEMY_BASE + OBJ_YH;
  CYC(0x618c, 0x618d); A = mem_rd(gb, DE);
  CYC(0x618d, 0x618e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x618e, 0x6190); goto deleteSpawner; } // jr z
  CYC(0x618e, 0x6190);
  CYC(0x6190, 0x6192); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x6192, 0x6193); A = mem_rd(gb, DE);
  CYC(0x6193, 0x6194); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6194); return; } // ret nz
  CYC(0x6194, 0x6195);
  CALL_C(0x6195, ecom_decCounter1_b0d_hook, 0x439a, 0x6198);
  if (!(F & FZ)) { RET_TAKEN(0x6198); return; } // ret nz
  CYC(0x6198, 0x6199);
  CYC(0x6199, 0x619b); mem_wr(gb, HL, 120);
  CYC(0x619b, 0x619e); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(0x619e, 0x619f); B = A;
  CYC(0x619f, 0x61a2); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(0x61a2, 0x61a3); C = A;
  CALL_C(0x61a3, getTileCollisionsAtPosition_hook, 0x14b7, 0x61a6);
  if (!(F & FZ)) { RET_TAKEN(0x61a6); return; } // ret nz
  CYC(0x61a6, 0x61a7);
  PUSH(0x61a7, BC);
  CYC(0x61a8, 0x61aa); B = 0x28; // ENEMY_WALLMASTER
  CALL_C(0x61aa, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, 0x436d, 0x61ad);
  SET_BC(POP(0x61ad));
  if (!(F & FZ)) { RET_TAKEN(0x61ae); return; } // ret nz
  CYC(0x61ae, 0x61af);
  CYC(0x61af, 0x61b1); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x61b1, 0x61b3); A = ENEMY_BASE; // Enemy.start
  CYC(0x61b3, 0x61b4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x61b4, 0x61b5); mem_wr(gb, HL, D);
  CYC(0x61b5, 0x61b7); E = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x61b7, 0x61b8); A = H;
  CYC(0x61b8, 0x61b9); mem_wr(gb, DE, A);
  RET(0x61b9); return; // ret

deleteSpawner:
  CALL_C(0x61ba, decNumEnemies_hook, 0x24b3, 0x61bd);
  CALL_C(0x61bd, markEnemyAsKilledInRoom_b00_hook, 0x320d, 0x61c0);
  CYC(0x61c0, 0x61c3); enemyDelete_hook(gb); return; // jp
}

// 0d:61c3, bare global; jump-table target from enemyCode28.
void wallmaster_state_galeSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x61c3, ecom_galeSeedEffect_b0d_hook, 0x447b, 0x61c6);
  if (F & FC) { RET_TAKEN(0x61c6); return; } // ret c
  CYC(0x61c6, 0x61c7);

  // Tell spawner that this wallmaster is dead
  CYC(0x61c7, 0x61c9); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x61c9, 0x61ca); A = mem_rd(gb, DE);
  CYC(0x61ca, 0x61cb); alu_or(gb, A);
  if (F & FZ) { CYCT(0x61cb, 0x61cd); goto deleteMe; } // jr z
  CYC(0x61cb, 0x61cd);
  CYC(0x61cd, 0x61ce); H = A;
  CYC(0x61ce, 0x61d0); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x61d0, 0x61d2); mem_wr(gb, HL, 0x00);

deleteMe:
  CYC(0x61d2, 0x61d5); enemyDelete_hook(gb); return; // jp
}

// 0d:61d5, bare global; jump-table target from enemyCode28.
void wallmaster_state_stub_hook(GB *gb) {
  RET(0x61d5); return; // ret
}

// 0d:61d6, bare global; jump-table target from enemyCode28. Spawning at Link's position,
// above the screen.
void wallmaster_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x61d6, 0x61d7); H = D;
  CYC(0x61d7, 0x61d8); L = E;
  CYC(0x61d8, 0x61d9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x61d9, 0x61db); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x61db, 0x61dd); mem_wr(gb, HL, 0x80 | 0x35); // ENEMY_FLOORMASTER

  // Copy Link's position, set high Z position
  CYC(0x61dd, 0x61df); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x61df, 0x61e1); mem_wr(gb, HL, 0xa0);
  CYC(0x61e1, 0x61e3); L = ENEMY_BASE + OBJ_YH;
  CYC(0x61e3, 0x61e6); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(0x61e6, 0x61e7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x61e7, 0x61e8); L = alu_inc8(gb, L);
  CYC(0x61e8, 0x61eb); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(0x61eb, 0x61ec); mem_wr(gb, HL, A);
  CYC(0x61ec, 0x61ee); A = 0x59; // SND_FALLINHOLE
  CALL_C(0x61ee, playSound_b00_hook, 0x0c98, 0x61f1);
  CYC(0x61f1, 0x61f4); objectSetVisiblec1_hook(gb); return; // jp
}

// 0d:61f4, bare global; jump-table target from enemyCode28. Falling to ground.
void wallmaster_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x61f4, 0x61f6); C = 0x0e;
  CALL_C(0x61f6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x61f9);
  if (F & FZ) { CYCT(0x61f9, 0x61fb); goto hitGround; } // jr z
  CYC(0x61f9, 0x61fb);
  CALL_C(0x61fb, wallmaster_flickerVisibilityIfHighUp_hook, 0x6275, 0x61fe);

  // Check for collision with Link
  CYC(0x61fe, 0x6200); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6200, 0x6201); A = mem_rd(gb, DE);
  CYC(0x6201, 0x6202); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6202); return; } // ret z
  CYC(0x6202, 0x6203);
  CYC(0x6203, 0x6205); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x6205, 0x6206); A = mem_rd(gb, DE);
  CYC(0x6206, 0x6209); mem_wr(gb, w1Link + OBJ_ZH, A);
  RET(0x6209); return; // ret

hitGround:
  CYC(0x620a, 0x620c); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x620c, 0x620e); mem_wr(gb, HL, 30);
  CYC(0x620e, 0x6210); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6210, 0x6211); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x6211); return; // ret
}

// 0d:6212, bare global; jump-table target from enemyCode28. Waiting on ground for
// [counter1] frames before moving back up.
void wallmaster_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6212, ecom_decCounter1_b0d_hook, 0x439a, 0x6215);
  if (!(F & FZ)) { CYCT(0x6215, 0x6217); goto checkCounter; } // jr nz
  CYC(0x6215, 0x6217);
  CYC(0x6217, 0x6218); L = E;
  CYC(0x6218, 0x6219); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  RET(0x6219); return; // ret

checkCounter:
  CYC(0x621a, 0x621b); A = mem_rd(gb, HL);
  CYC(0x621b, 0x621d); alu_cp(gb, 20); // [counter1] == 20?
  if (F & FC) { CYCT(0x621d, 0x621f); goto checkAlmostDone; } // jr c
  CYC(0x621d, 0x621f);
  if (!(F & FZ)) { RET_TAKEN(0x621f); return; } // ret nz
  CYC(0x621f, 0x6220);

  // Close hand when [counter1] == 20
  CYC(0x6220, 0x6222); A = 0x01;
  CYC(0x6222, 0x6225); enemySetAnimation_hook(gb); return; // jp

checkAlmostDone:
  CYC(0x6225, 0x6226); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6226, 0x6228); goto checkPulledLink; } // jr nz
  CYC(0x6226, 0x6228);

  // Set collisionType when [counter1] == 1
  CYC(0x6228, 0x622a); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x622a, 0x622b); A = mem_rd(gb, HL);
  CYC(0x622b, 0x622d); alu_and(gb, 0x80);
  CYC(0x622d, 0x622f); alu_or(gb, 0x28); // ENEMY_WALLMASTER
  CYC(0x622f, 0x6230); mem_wr(gb, HL, A);

checkPulledLink:
  CYC(0x6230, 0x6232); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6232, 0x6234); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6234); return; } // ret z
  CYC(0x6234, 0x6235);
  CYC(0x6235, 0x6236); alu_xor(gb, A);
  CYC(0x6236, 0x6239); mem_wr(gb, w1Link + OBJ_VISIBLE, A);
  RET(0x6239); return; // ret
}

// 0d:623a, bare global; jump-table target from enemyCode28. Moving back up.
void wallmaster_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x623a, wallmaster_flickerVisibilityIfHighUp_hook, 0x6275, 0x623d);
  CYC(0x623d, 0x623e); H = D;
  CYC(0x623e, 0x6240); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6240, 0x6241); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6241, 0x6242); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6242, 0x6243); A = mem_rd(gb, HL);
  CYC(0x6243, 0x6245); alu_cp(gb, 0xa0);
  if (!(F & FC)) { RET_TAKEN(0x6245); return; } // ret nc
  CYC(0x6245, 0x6246);

  // Moved high enough
  CALL_C(0x6246, objectSetInvisible_hook, 0x1e7b, 0x6249);
  CYC(0x6249, 0x624b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x624b, 0x624d); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x624d, 0x624f); goto notPulledLink; } // jr z
  CYC(0x624d, 0x624f);

  // We just pulled Link out, go to state $0d
  CYC(0x624f, 0x6251); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6251, 0x6253); mem_wr(gb, HL, 0x0d);
  RET(0x6253); return; // ret

notPulledLink:
  CYC(0x6254, 0x6256); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6256, 0x6257); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(0x6257, 0x6259); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6259, 0x625b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x625b, 0x625d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x625d, 0x625f); mem_wr(gb, HL, 120);
  RET(0x625f); return; // ret
}

// 0d:6260, bare global; jump-table target from enemyCode28. Waiting off-screen until time
// to attack again.
void wallmaster_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6260, ecom_decCounter1_b0d_hook, 0x439a, 0x6263);
  if (!(F & FZ)) { RET_TAKEN(0x6263); return; } // ret nz
  CYC(0x6263, 0x6264);
  CYC(0x6264, 0x6265); L = E;
  CYC(0x6265, 0x6267); mem_wr(gb, HL, 0x08); // [state] = 8
  CYC(0x6267, 0x6269); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6269, 0x626a); alu_xor(gb, A);
  CYC(0x626a, 0x626b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x626b, 0x626c); mem_wr(gb, HL, A);
  CYC(0x626c, 0x626f); enemySetAnimation_hook(gb); return; // jp
}

// 0d:626f, bare global; jump-table target from enemyCode28. Just dragged Link off-screen.
void wallmaster_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x626f, 0x6271); A = 0x02;
  CYC(0x6271, 0x6274); mem_wr(gb, w1Link + OBJ_SUBSTATE, A);
  RET(0x6274); return; // ret
}

// 0d:6275, bare global; called from wallmaster_state9 and wallmaster_stateB. Flickers
// visibility if very high up (zh < $b8).
void wallmaster_flickerVisibilityIfHighUp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6275, 0x6277); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x6277, 0x6278); A = mem_rd(gb, DE);
  CYC(0x6278, 0x6279); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6279); return; } // ret z
  CYC(0x6279, 0x627a);
  CYC(0x627a, 0x627c); alu_cp(gb, 0xb8);
  if (F & FC) { CYCT(0x627c, 0x627f); ecom_flickerVisibility_b0d_hook(gb); return; } // jp c
  CYC(0x627c, 0x627f);
  CYC(0x627f, 0x6281); alu_cp(gb, 0xbc);
  if (!(F & FC)) { RET_TAKEN(0x6281); return; } // ret nc
  CYC(0x6281, 0x6282);
  CYC(0x6282, 0x6285); objectSetVisiblec1_hook(gb); return; // jp
}
