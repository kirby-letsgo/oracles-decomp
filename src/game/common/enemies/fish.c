#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void fish_state_uninitialized_hook(GB *gb);
void fish_state_stub_hook(GB *gb);
void fish_subid00_hook(GB *gb);
void fish_enterWater_hook(GB *gb);
void fish_subid01_hook(GB *gb);
void fish_checkReverseAngle_hook(GB *gb);
void fish_updateAnimationFromAngle_hook(GB *gb);
void fish_setAnimation_hook(GB *gb);
void fish_updatePosition_hook(GB *gb);
void fish_setRandomCounter1_hook(GB *gb);
void fish_getAdjacentWallsBitsetForKnockback_hook(GB *gb);

static uint16_t fish_jump_table(GB *gb) {
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

static void fish_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_PIRANHA
//
// Variables:
//   zh: Equals 2 when underwater
//   var30: Current animation index
// ==================================================================================================
void enemyCode1e_hook(GB *gb) {
  BASE(enemyCode1e);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { CYCT(b_+4, b_+6); goto stunned; } // jr c
  CYC(b_+4, b_+6);
  if (F & FZ) { CYCT(b_+6, b_+9); enemyDie_hook(gb); return; } // jp z
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);

  // ENEMYSTATUS_KNOCKBACK
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); A = 0x50; // SPEED_200
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, fish_getAdjacentWallsBitsetForKnockback_hook, SYM(fish_getAdjacentWallsBitsetForKnockback), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+21, ecom_applyVelocityGivenAdjacentWalls_b0d_hook, SYM(ecom_applyVelocityGivenAdjacentWalls_b0d), b_+24);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+26, b_+28); A = 0x1e; // SPEED_c0
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return; // ret

stunned:
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+35); return; } // ret z
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); TAIL(fish_enterWater); // jp

normalStatus:
  CALL_C(b_+41, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+44);
  if (!(F & FC)) { CYCT(b_+44, b_+46); goto normalState; } // jr nc
  CYC(b_+44, b_+46);
  {
    CYC(b_+46, b_+47); push_effect(gb, b_+47);
    uint16_t target = fish_jump_table(gb);
    if (target == SYM(fish_state_uninitialized)) { fish_state_uninitialized_hook(gb); return; }
    if (target == SYM(fish_state_stub)) { fish_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+63, b_+64); A = B;
  {
    CYC(b_+64, b_+65); push_effect(gb, b_+65);
    uint16_t target = fish_jump_table(gb);
    if (target == SYM(fish_subid00)) { fish_subid00_hook(gb); return; }
    if (target == SYM(fish_subid01)) { fish_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5ab1, bare global; jump-table target from enemyCode1e.
void fish_state_uninitialized_hook(GB *gb) {
  BASE(fish_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x14; // SPEED_80
  CALL_C(b_+2, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+5);
  CALL_C(b_+5, objectSetVisible83_hook, SYM(objectSetVisible83), b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x02);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CALL_C(b_+16, fish_setRandomCounter1_hook, SYM(fish_setRandomCounter1), b_+19);
  CYC(b_+19, b_+22); TAIL(fish_updateAnimationFromAngle); // jp
}

// 0d:5ac7, bare global; jump-table target from enemyCode1e.
void fish_state_stub_hook(GB *gb) {
  BASE(fish_state_stub);
  RET(b_+0); return; // ret
}

// 0d:5ac8, bare global; jump-table target from enemyCode1e@normalState. Internal @state8/
// @leapOutOfWater/@state9 are @-local (no separate registration), handled via goto.
void fish_subid00_hook(GB *gb) {
  BASE(fish_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = fish_jump_table(gb);
    if (target == b_+8) goto state8;
    if (target == b_+59) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(b_+8, b_+11); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto afterScentCheck; } // jr nz
  CYC(b_+12, b_+14);
  CALL_C(b_+14, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+19); goto leapOutOfWater; } // jr z
  CYC(b_+17, b_+19);

afterScentCheck:
  CALL_C(b_+19, fish_updatePosition_hook, SYM(fish_updatePosition), b_+22);
  CYC(b_+22, b_+25); TAIL(fish_checkReverseAngle); // jp

leapOutOfWater:
  CYC(b_+25, b_+26); L = E;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, GV(0x14, 0x10)); // ENEMYCOLLISION_SWITCHHOOK_DAMAGE_ENEMY
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x00);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+37, b_+39); A = 0x80; // <(-$180)
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0xfe); // >(-$180)
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+46, b_+48); B = 0x03; // INTERAC_SPLASH
  CALL_C(b_+48, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+51);
  CALL_C(b_+51, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+54);
  CYC(b_+54, b_+56); B = 0x00;
  CYC(b_+56, b_+59); TAIL(fish_setAnimation); // jp

state9:
  CYC(b_+59, b_+61); C = 0x10;
  CALL_C(b_+61, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+64);
  if (F & FZ) { CYCT(b_+64, b_+66); fish_enterWater_hook(gb); return; } // jr z
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+70); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+70, b_+72); goto state9UpdatePosition; } // jr nz
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+73); L = alu_inc8(gb, L);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+75); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+75, b_+77); goto state9UpdatePosition; } // jr nz
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+79); B = 0x01;
  CALL_C(b_+79, fish_setAnimation_hook, SYM(fish_setAnimation), b_+82);

state9UpdatePosition:
  CYC(b_+82, b_+85); TAIL(fish_updatePosition); // jp
}

// 0d:5b1d, bare global; called from enemyCode1e and fish_subid00.
void fish_enterWater_hook(GB *gb) {
  BASE(fish_enterWater);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x08);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(b_+17, fish_setRandomCounter1_hook, SYM(fish_setRandomCounter1), b_+20);
  CYC(b_+20, b_+22); B = 0x03; // INTERAC_SPLASH
  CALL_C(b_+22, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+25);
  CALL_C(b_+25, objectSetVisible83_hook, SYM(objectSetVisible83), b_+28);
  CYC(b_+28, b_+31); TAIL(fish_updateAnimationFromAngle); // jp
}

// 0d:5b3c, bare global; jump-table target from enemyCode1e@normalState. Internal @state8
// is @-local (no separate registration).
void fish_subid01_hook(GB *gb) {
  BASE(fish_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = fish_jump_table(gb);
    if (target == b_+6) { RET(b_+6); return; }
    HANDOFF(target);
  }
}

// 0d:5b43, bare global; called from fish_subid00, falls through into
// fish_updateAnimationFromAngle.
// @param cflag c if we were able to move
void fish_checkReverseAngle_hook(GB *gb) {
  BASE(fish_checkReverseAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FC) { RET_TAKEN(b_+0); return; } // ret c
  CYC(b_+0, b_+1);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); alu_xor(gb, 0x10);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  TAIL(fish_updateAnimationFromAngle); // fallthrough
}

// 0d:5b4a, bare global; called from fish_enterWater and fish_state_uninitialized, also
// falls into from fish_checkReverseAngle.
void fish_updateAnimationFromAngle_hook(GB *gb) {
  BASE(fish_updateAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+9); SET_HL(b_+20); // @animations
  CYC(b_+9, b_+10); fish_addAToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+15); return; } // ret z
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+20); TAIL(enemySetAnimation); // jp
}

// 0d:5b62, bare global; called from fish_subid00. Sets animation (3 or 5 is added to value
// passed if we're moving right or left).
// @param b Value to add to animation index
void fish_setAnimation_hook(GB *gb) {
  BASE(fish_setAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+7); alu_and(gb, 0x01);
  CYC(b_+7, b_+9); A = 0x03;
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto addOffset; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x05;

addOffset:
  CYC(b_+13, b_+14); alu_add(gb, B);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+18); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+18); return; } // ret z
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+23); TAIL(enemySetAnimation); // jp
}

// 0d:5b79, bare global; called from fish_subid00.
// @param[out] cflag c if we were able to move (tile in front of us is traversable)
void fish_updatePosition_hook(GB *gb) {
  BASE(fish_updatePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rrca(gb);
  CYC(b_+4, b_+5); alu_rrca(gb);
  CYC(b_+5, b_+8); SET_HL(b_+40); // @directionOffsets
  CYC(b_+8, b_+9); fish_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+15); alu_and(gb, 0xf0);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+17); SET_HL(HL + 1); // inc hl
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+23); alu_and(gb, 0xf0);
  CYC(b_+23, b_+25); A = alu_swap(gb, A);
  CYC(b_+25, b_+26); alu_or(gb, C);
  CYC(b_+26, b_+27); C = A;
  CYC(b_+27, b_+29); B = 0xcf; // >wRoomLayout
  CYC(b_+29, b_+30); A = mem_rd(gb, BC);
  CYC(b_+30, b_+32); alu_sub(gb, GV(0xf9, 0xfa)); // TILEINDEX_PUDDLE
  CYC(b_+32, b_+34); alu_cp(gb, GV(0x05, 0x04)); // TILEINDEX_FD-TILEINDEX_PUDDLE+1
  if (!(F & FC)) { RET_TAKEN(b_+34); return; } // ret nc
  CYC(b_+34, b_+35);
  CALL_C(b_+35, objectApplySpeed_hook, SYM(objectApplySpeed), b_+38);
  CYC(b_+38, b_+39); alu_scf(gb);
  RET(b_+39); return; // ret
}

// 0d:5ba9, bare global; called from fish_enterWater and fish_state_uninitialized.
void fish_setRandomCounter1_hook(GB *gb) {
  BASE(fish_setRandomCounter1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+14); // @counter1Vals
  CYC(b_+8, b_+9); fish_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return; // ret
}

// 0d:5bbb, bare global; called from enemyCode1e. Gets the "adjacent walls bitset" for the
// fish; since this swims, water is traversable, everything else is not. Identical to
// waterTektite_getAdjacentWallsBitsetGivenAngle.
// @param[out] hFF8B Bitset of adjacent walls
void fish_getAdjacentWallsBitsetForKnockback_hook(GB *gb) {
  BASE(fish_getAdjacentWallsBitsetForKnockback);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CALL_C(b_+3, ecom_getAdjacentWallTableOffset_b0d_hook, SYM(ecom_getAdjacentWallTableOffset_b0d), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+12, b_+13); C = mem_rd(gb, HL);
  CYC(b_+13, b_+16); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0d)); // ecom_sideviewAdjacentWallOffsetTable
  CYC(b_+16, b_+17); fish_addAToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+19); A = 0x10;
  CYC(b_+19, b_+21); H8(hFF8B) = A;
  CYC(b_+21, b_+23); D = 0xcf; // >wRoomLayout

scanWall:
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+24, b_+25); alu_add(gb, B);
  CYC(b_+25, b_+26); B = A;
  CYC(b_+26, b_+28); alu_and(gb, 0xf0);
  CYC(b_+28, b_+29); E = A;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi
  CYC(b_+30, b_+31); alu_add(gb, C);
  CYC(b_+31, b_+32); C = A;
  CYC(b_+32, b_+34); alu_and(gb, 0xf0);
  CYC(b_+34, b_+36); A = alu_swap(gb, A);
  CYC(b_+36, b_+37); alu_or(gb, E);
  CYC(b_+37, b_+38); E = A;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); alu_sub(gb, GV(0xf9, 0xfa)); // TILEINDEX_PUDDLE
  CYC(b_+41, b_+43); alu_cp(gb, GV(0x05, 0x04)); // TILEINDEX_FD-TILEINDEX_PUDDLE+1
  CYC(b_+43, b_+45); A = H8(hFF8B);
  CYC(b_+45, b_+46); alu_rla(gb);
  CYC(b_+46, b_+48); H8(hFF8B) = A;
  if (!(F & FC)) { CYCT(b_+48, b_+50); goto scanWall; } // jr nc
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); alu_xor(gb, 0x0f);
  CYC(b_+52, b_+54); H8(hFF8B) = A;
  CYC(b_+54, b_+56); A = H8(hActiveObject);
  CYC(b_+56, b_+57); D = A;
  RET(b_+57); return; // ret
}
