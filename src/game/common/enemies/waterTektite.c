#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode3a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode3a), (from), (to), true)

void waterTektite_state_uninitialized_hook(GB *gb);
void waterTektike_decideNewAngle_hook(GB *gb);
void waterTektike_state_stub_hook(GB *gb);
void waterTektike_state8_hook(GB *gb);
void waterTektike_animate_hook(GB *gb);
void waterTektike_state9_hook(GB *gb);
void waterTektite_getAdjacentWallsBitset_hook(GB *gb);
void waterTektite_getAdjacentWallsBitsetGivenAngle_hook(GB *gb);
void waterTektike_setSpeedFromCounter1_hook(GB *gb);

static uint16_t waterTektite_jump_table(GB *gb) {
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

static void waterTektite_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_WATER_TEKTITE
// ==================================================================================================
void enemyCode3a_hook(GB *gb) {
  BASE(enemyCode3a);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+9); return; } // ret z
  CYC(b_+9, b_+10);

  // ENEMYSTATUS_KNOCKBACK
  // Need special knockback code for special "solidity" properties (water is
  // traversible, everything else is solid)
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, AF); // push af
  CYC(b_+14, b_+16); A = 0x50; // SPEED_200
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+19, waterTektite_getAdjacentWallsBitsetGivenAngle_hook, SYM(waterTektite_getAdjacentWallsBitsetGivenAngle), b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CALL_C(b_+24, ecom_applyVelocityGivenAdjacentWalls_b0e_hook, SYM(ecom_applyVelocityGivenAdjacentWalls_b0e), b_+27);

  CYC(b_+27, b_+28); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return; // ret

normalStatus:
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  {
    CYC(b_+35, b_+36); push_effect(gb, b_+36);
    uint16_t target = waterTektite_jump_table(gb);
    if (target == SYM(waterTektite_state_uninitialized)) { waterTektite_state_uninitialized_hook(gb); return; }
    if (target == SYM(waterTektike_state_stub)) { waterTektike_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(waterTektike_state8)) { waterTektike_state8_hook(gb); return; }
    if (target == SYM(waterTektike_state9)) { waterTektike_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:54ae, bare global; jump-table target from enemyCode3a. Falls into
// waterTektike_decideNewAngle.
void waterTektite_state_uninitialized_hook(GB *gb) {
  BASE(waterTektite_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectSetVisible82_hook, SYM(objectSetVisible82), SYM(waterTektike_decideNewAngle));
  waterTektike_decideNewAngle_hook(gb); return; // fallthrough
}

// 0e:54b1, bare global; falls into from waterTektite_state_uninitialized, also reached by
// genuine jr from waterTektike_state9.
void waterTektike_decideNewAngle_hook(GB *gb) {
  BASE(waterTektike_decideNewAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x08);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x40);
  CYC(b_+9, b_+12); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto scentSeedActive; } // jr nz
  CYC(b_+13, b_+15);

  // Random diagonal angle
  CALL_C(b_+15, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x18);
  CYC(b_+20, b_+22); alu_add(gb, 0x04);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); waterTektike_animate_hook(gb); return; // jr

scentSeedActive:
  CYC(b_+27, b_+29); A = hram_rd(gb, 0xb2); // hFFB2
  CYC(b_+29, b_+31); hram_wr(gb, 0x8f, A); // hFF8F
  CYC(b_+31, b_+33); A = hram_rd(gb, 0xb3); // hFFB3
  CYC(b_+33, b_+35); hram_wr(gb, 0x8e, A); // hFF8E
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+38, b_+39); B = A;
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+41); C = mem_rd(gb, HL);
  CALL_C(b_+41, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, SYM(waterTektike_state_stub)); waterTektike_animate_hook(gb); return; // jr
}

// 0e:54e2, bare global; jump-table target from enemyCode3a.
void waterTektike_state_stub_hook(GB *gb) {
  BASE(waterTektike_state_stub);
  RET(b_+0); return; // ret
}

// 0e:54e3, bare global; jump-table target from enemyCode3a. Moving in some direction for
// [counter1] frames, at varying speeds.
void waterTektike_state8_hook(GB *gb) {
  BASE(waterTektike_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto keepMoving; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x08);
  CYC(b_+11, b_+13); waterTektike_animate_hook(gb); return; // jr

keepMoving:
  CALL_C(b_+13, waterTektike_setSpeedFromCounter1_hook, SYM(waterTektike_setSpeedFromCounter1), b_+16);
  CALL_C(b_+16, waterTektite_getAdjacentWallsBitset_hook, SYM(waterTektite_getAdjacentWallsBitset), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_ANGLE;
  CALL_C(b_+21, ecom_applyVelocityGivenAdjacentWalls_b0e_hook, SYM(ecom_applyVelocityGivenAdjacentWalls_b0e), b_+24);
  CALL_C(b_+24, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), SYM(waterTektike_animate));
  waterTektike_animate_hook(gb); return; // fallthrough
}

// 0e:54fe, bare global; falls into from waterTektike_state8, also reached by genuine jr
// from waterTektite_state_uninitialized (via waterTektike_decideNewAngle) and
// waterTektike_state9.
void waterTektike_animate_hook(GB *gb) {
  BASE(waterTektike_animate);
  CYC(b_+0, SYM(waterTektike_state9)); enemyAnimate_hook(gb); return; // jp
}

// 0e:5501, bare global; jump-table target from enemyCode3a. Not moving for [counter1]
// frames; then choosing new angle.
void waterTektike_state9_hook(GB *gb) {
  BASE(waterTektike_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); waterTektike_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, SYM(waterTektite_getAdjacentWallsBitset)); waterTektike_decideNewAngle_hook(gb); return; // jr
}

// 0e:5508, bare global; called from waterTektike_state8. Gets the "adjacent walls bitset"
// for the tektike; since this swims, water is traversable, everything else is not. This is
// identical to "fish_getAdjacentWallsBitsetForKnockback".
void waterTektite_getAdjacentWallsBitset_hook(GB *gb) {
  BASE(waterTektite_getAdjacentWallsBitset);
  CYC(b_+0, SYM(waterTektite_getAdjacentWallsBitsetGivenAngle)); E = ENEMY_BASE + OBJ_ANGLE;
  waterTektite_getAdjacentWallsBitsetGivenAngle_hook(gb); return; // fallthrough
}

// 0e:550a, bare global; falls into from waterTektite_getAdjacentWallsBitset, also reached
// by genuine call from enemyCode3a.
// @param de Angle variable
void waterTektite_getAdjacentWallsBitsetGivenAngle_hook(GB *gb) {
  BASE(waterTektite_getAdjacentWallsBitsetGivenAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CALL_C(b_+1, ecom_getAdjacentWallTableOffset_b0e_hook, SYM(ecom_getAdjacentWallTableOffset_b0e), b_+4);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);
  CYC(b_+11, b_+14); SET_HL(SYM(ecom_sideviewAdjacentWallOffsetTable_b0e)); // ecom_sideviewAdjacentWallOffsetTable (bank 0e)
  waterTektite_addAToHl_from_rst(gb, b_+15);

  CYC(b_+15, b_+17); A = 0x10;
  CYC(b_+17, b_+19); hram_wr(gb, 0x8b, A); // hFF8B
  CYC(b_+19, b_+21); D = 0xcf; // >wRoomLayout

nextOffset:
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+22, b_+23); alu_add(gb, B);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+26); alu_and(gb, 0xf0);
  CYC(b_+26, b_+27); E = A;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+32); alu_and(gb, 0xf0);
  CYC(b_+32, b_+34); A = alu_swap(gb, A);
  CYC(b_+34, b_+35); alu_or(gb, E);
  CYC(b_+35, b_+36); E = A;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); alu_sub(gb, 0xf9); // TILEINDEX_PUDDLE
  CYC(b_+39, b_+41); alu_cp(gb, 0x05); // TILEINDEX_FD-TILEINDEX_PUDDLE+1
  CYC(b_+41, b_+43); A = hram_rd(gb, 0x8b); // hFF8B
  CYC(b_+43, b_+44); alu_rla(gb);
  CYC(b_+44, b_+46); hram_wr(gb, 0x8b, A); // hFF8B
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto nextOffset; } // jr nc
  CYC(b_+46, b_+48);

  CYC(b_+48, b_+50); alu_xor(gb, 0x0f);
  CYC(b_+50, b_+52); hram_wr(gb, 0x8b, A); // hFF8B
  CYC(b_+52, b_+54); A = hram_rd(gb, 0xaf); // hActiveObject
  CYC(b_+54, b_+55); D = A;
  RET(b_+55); return; // ret
}

// 0e:5542, bare global; called from waterTektike_state8.
// @param hl Pointer to counter1
void waterTektike_setSpeedFromCounter1_hook(GB *gb) {
  BASE(waterTektike_setSpeedFromCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+3); A = alu_srl(gb, A);
  CYC(b_+3, b_+5); A = alu_srl(gb, A);
  CYC(b_+5, b_+8); SET_HL(b_+14); // @speedVals
  waterTektite_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return; // ret
}
