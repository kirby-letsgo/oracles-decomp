#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// NOTE: dekuScrub_targetAngles (0d:60db) is pure ROM data (a 32-byte lookup table used only
// via rst_addAToHl below), not code, despite lacking a "Table" suffix in its name -- verified
// with --report, which decodes it as garbage/illegal instructions:
//   60db  nop / nop / nop / nop / nop / nop / nop
//   60e2  ld ($0808),sp
//   60e5  inc c / inc c / inc c / inc c / inc c
//   60ea  stop  !! unsupported
// It is used below purely as a table base address (SET_HL(0x60db)); no _hook is written for it.

void dekuScrub_state_uninitialized_hook(GB *gb);
void dekuScrub_state_stub_hook(GB *gb);
void dekuScrub_state8_hook(GB *gb);
void dekuScrub_state9_hook(GB *gb);
void dekuScrub_stateA_hook(GB *gb);
void dekuScrub_animate_hook(GB *gb);
void dekuScrub_stateB_hook(GB *gb);
void dekuScrub_stateC_hook(GB *gb);
void dekuScrub_stateD_hook(GB *gb);
void dekuScrub_hideInBush_hook(GB *gb);
void dekuScrub_spawnBush_hook(GB *gb);

static uint16_t dekuScrub_jump_table(GB *gb) {
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

static void dekuScrub_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_DEKU_SCRUB
//
// Variables:
//   var03: Read by ENEMY_BUSH_OR_ROCK to control Z-offset
//   var30: Starts at 2, gets decremented each time one of the scrub's bullets hits itself.
//   var31: Index of ENEMY_BUSH_OR_ROCK
//   var32: "pressedAButton" variable (nonzero when player presses A)
//   var33: Former var03 value (low byte of text index, TX_45XX)
// ==================================================================================================
void enemyCode27_hook(GB *gb) {
  BASE(enemyCode27);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto normalStatus; } // jr nz
  CYC(b_+8, b_+10);

  // ENEMYSTATUS_JUST_HIT
  CYC(b_+10, b_+12); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0c);
  CYC(b_+20, b_+22); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  CYC(b_+23, b_+26); TAIL(ecom_killObjectH_b0d); // jp

dead:
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+33); enemyDie_hook(gb); return; } // jp nz
  CYC(b_+30, b_+33);

normalStatus:
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  {
    CYC(b_+36, b_+37); push_effect(gb, b_+37);
    uint16_t target = dekuScrub_jump_table(gb);
    if (target == SYM(dekuScrub_state_uninitialized)) { dekuScrub_state_uninitialized_hook(gb); return; }
    if (target == SYM(dekuScrub_state_stub)) { dekuScrub_state_stub_hook(gb); return; }
    if (target == SYM(dekuScrub_state8)) { dekuScrub_state8_hook(gb); return; }
    if (target == SYM(dekuScrub_state9)) { dekuScrub_state9_hook(gb); return; }
    if (target == SYM(dekuScrub_stateA)) { dekuScrub_stateA_hook(gb); return; }
    if (target == SYM(dekuScrub_stateB)) { dekuScrub_stateB_hook(gb); return; }
    if (target == SYM(dekuScrub_stateC)) { dekuScrub_stateC_hook(gb); return; }
    if (target == SYM(dekuScrub_stateD)) { dekuScrub_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:6008, bare global; jump-table target from enemyCode27.
void dekuScrub_state_uninitialized_hook(GB *gb) {
  BASE(dekuScrub_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dekuScrub_spawnBush_hook, SYM(dekuScrub_spawnBush), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+7);
  CYC(b_+7, b_+9); H = 0xcf; // >wRoomLayout
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  // The value of 'a' here depends on objectMakeTileSolid; it should be 0 if the enemy
  // spawned on an empty space. This enemy doesn't move, so it shouldn't matter either way.
  CALL_C(b_+11, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x02);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x00);
  CYC(b_+26, b_+28); L = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  RET(b_+29); return; // ret
}

// 0d:6026, bare global; jump-table target from enemyCode27.
void dekuScrub_state_stub_hook(GB *gb) {
  BASE(dekuScrub_state_stub);
  RET(b_+0); return; // ret
}

// 0d:6027, bare global; jump-table target from enemyCode27. Waiting for Link to be a
// certain distance away.
void dekuScrub_state8_hook(GB *gb) {
  BASE(dekuScrub_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x2c;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (F & FC) { RET_TAKEN(b_+5); return; } // ret c
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 90);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x02);
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CALL_C(b_+20, enemySetAnimation_hook, SYM(enemySetAnimation), b_+23);
  CYC(b_+23, b_+26); TAIL(objectSetVisiblec3); // jp
}

// 0d:6041, bare global; jump-table target from enemyCode27. Link is at a good distance,
// wait a bit longer before emerging from bush.
void dekuScrub_state9_hook(GB *gb) {
  BASE(dekuScrub_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x2c;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (F & FC) { CYCT(b_+5, b_+8); dekuScrub_hideInBush_hook(gb); return; } // jp c
  CYC(b_+5, b_+8);
  CALL_C(b_+8, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); dekuScrub_animate_hook(gb); return; } // jr nz
  CYC(b_+11, b_+13);

  // Emerge from under the bush
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  // Calculate angle to shoot
  CALL_C(b_+23, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+26);
  CYC(b_+26, b_+29); SET_HL(SYM(dekuScrub_targetAngles)); // dekuScrub_targetAngles (data)
  CYC(b_+29, b_+30); dekuScrub_addAToHl_from_rst(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+32, b_+34); dekuScrub_hideInBush_hook(gb); return; } // jr z
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+38); alu_rrca(gb);
  CYC(b_+38, b_+39); alu_rrca(gb);
  CYC(b_+39, b_+41); alu_sub(gb, 0x02);
  CYC(b_+41, b_+44); SET_HL(SYM(dekuScrub_fireAnimations)); // dekuScrub_fireAnimations (data)
  CYC(b_+44, b_+45); dekuScrub_addAToHl_from_rst(gb, b_+45);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+49); TAIL(enemySetAnimation); // jp
}

// 0d:6072, bare global; jump-table target from enemyCode27. Firing sequence; falls through
// into dekuScrub_animate.
void dekuScrub_stateA_hook(GB *gb) {
  BASE(dekuScrub_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x2c;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); dekuScrub_hideInBush_hook(gb); return; } // jr c
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); dekuScrub_hideInBush_hook(gb); return; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); dekuScrub_animate_hook(gb); return; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+20); B = 0x1e; // PART_DEKU_SCRUB_PROJECTILE
  CALL_C(b_+20, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), SYM(dekuScrub_animate));
  TAIL(dekuScrub_animate); // fallthrough
}

// 0d:6089, bare global; called from dekuScrub_state9/stateA/stateB/stateD.
void dekuScrub_animate_hook(GB *gb) {
  BASE(dekuScrub_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0d:608c, bare global; jump-table target from enemyCode27. Go hide in the bush again.
void dekuScrub_stateB_hook(GB *gb) {
  BASE(dekuScrub_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); dekuScrub_animate_hook(gb); return; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x08);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+18); TAIL(objectSetInvisible); // jp
}

// 0d:609e, bare global; jump-table target from enemyCode27. He's just been defeated; falls
// through into dekuScrub_stateD.
void dekuScrub_stateC_hook(GB *gb) {
  BASE(dekuScrub_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0d
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+7, b_+9); E = ENEMY_BASE + 0x32; // Enemy.var32
  CALL_C(b_+9, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+12);
  CYC(b_+12, b_+14); A = 0x07;
  CALL_C(b_+14, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(dekuScrub_stateD));
  TAIL(dekuScrub_stateD); // fallthrough
}

// 0d:60af, bare global; jump-table target from enemyCode27, also falls into from
// dekuScrub_stateC. Waiting for Link to talk to him.
void dekuScrub_stateD_hook(GB *gb) {
  BASE(dekuScrub_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); dekuScrub_animate_hook(gb); return; } // jr z
  CYC(b_+7, b_+9);

  // Pressed A in front of deku scrub
  CYC(b_+9, b_+11); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);

  // Show text
  CYC(b_+13, b_+15); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+19); B = 0x45; // >TX_4500
  CYC(b_+19, b_+22); TAIL(showText); // jp
}

// 0d:60c5, bare global; called from dekuScrub_state9 and dekuScrub_stateA.
void dekuScrub_hideInBush_hook(GB *gb) {
  BASE(dekuScrub_hideInBush);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0b);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 120);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x02);
  CYC(b_+17, b_+19); A = 0x06;
  CYC(b_+19, b_+22); TAIL(enemySetAnimation); // jp
}

// 0d:6100, bare global; called from dekuScrub_state_uninitialized.
// @param[out] zflag z if spawned bush successfully
void dekuScrub_spawnBush_hook(GB *gb) {
  BASE(dekuScrub_spawnBush);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x58; // ENEMY_BUSH_OR_ROCK
  CALL_C(b_+2, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, objectCopyPosition_hook, SYM(objectCopyPosition), b_+9);

  // [child.relatedObj1] = this
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+11, b_+13); A = ENEMY_BASE; // Enemy.start
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+14, b_+15); mem_wr(gb, HL, D);

  // Save projectile's index to var31
  CYC(b_+15, b_+17); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+17, b_+18); A = H;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);

  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); E = L;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  RET(b_+25); return; // ret
}
