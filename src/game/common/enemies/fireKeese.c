#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void fireKeese_stateBOrHigher_hook(GB *gb);
void fireKeese_state_uninitialized_hook(GB *gb);
void fireKeese_state_stub_hook(GB *gb);
void fireKeese_state8_hook(GB *gb);
void fireKeese_state9_hook(GB *gb);
void fireKeese_stateA_hook(GB *gb);
void fireKeese_subid0_hook(GB *gb);
void fireKeese_subid0_stateB_hook(GB *gb);
void fireKeese_subid0_stateC_hook(GB *gb);
void fireKeese_updatePosition_hook(GB *gb);
void fireKeese_animate_hook(GB *gb);
void fireKeese_subid0_stateD_hook(GB *gb);
void fireKeese_subid1_hook(GB *gb);
void fireKeese_subid1_stateB_hook(GB *gb);
void fireKeese_subid1_stateC_hook(GB *gb);
void fireKeese_subid1_stateD_hook(GB *gb);
void fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1_hook(GB *gb);
void fireKeese_checkCloseToLink_hook(GB *gb);
void fireKeese_addCandidateTorch_hook(GB *gb);
void fireKeese_checkForNewlyLitTorch_hook(GB *gb);
void fireKeese_moveToGround_hook(GB *gb);
void fireKeese_moveTowardCenterIfOutOfBounds_hook(GB *gb);

static uint16_t fireKeese_jump_table(GB *gb) {
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

static void fireKeese_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_FIRE_KEESE
//
// Variables:
//   var30: Distance away (in tiles) closest lit torch is
//   var31/var32: Position of lit torch it's moving towards to re-light itself
//   var33: Nonzero if fire has been shed (set to 2). Doubles as animation index?
//   var34: Position at which to search for a lit torch ($16 tiles are checked each frame,
//          so this gets incremented by $16 each frame)
//   var35: Angular rotation for subid 0. (set to -1 or 1 randomly on initialization, for
//          counterclockwise or clockwise movement)
// ==================================================================================================
void enemyCode39_hook(GB *gb) {
  BASE(enemyCode39);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); E = OBJ_VAR2A;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } // ret nz
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } // ret nz
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); B = 0x20; // PART_FIRE
  CALL_C(b_+25, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+28);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+31, b_+33); A = 0x01;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x08);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0xfc); // -$04
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x02);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+51, b_+53); A = 0x03;
  CYC(b_+53, b_+56); TAIL(enemySetAnimation); // jp

normalStatus:
  CALL_C(b_+56, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+59);
  CYC(b_+59, b_+61); alu_cp(gb, 0x0b);
  if (!(F & FC)) { CYCT(b_+61, b_+63); fireKeese_stateBOrHigher_hook(gb); return; } // jr nc
  CYC(b_+61, b_+63);
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == SYM(fireKeese_state_uninitialized)) { fireKeese_state_uninitialized_hook(gb); return; }
    if (target == SYM(fireKeese_state_stub)) { fireKeese_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(fireKeese_state8)) { fireKeese_state8_hook(gb); return; }
    if (target == SYM(fireKeese_state9)) { fireKeese_state9_hook(gb); return; }
    if (target == SYM(fireKeese_stateA)) { fireKeese_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:51a5, bare global; jump-table target from enemyCode39.
void fireKeese_stateBOrHigher_hook(GB *gb) {
  BASE(fireKeese_stateBOrHigher);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == SYM(fireKeese_subid0)) { fireKeese_subid0_hook(gb); return; }
    if (target == SYM(fireKeese_subid1)) { fireKeese_subid1_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:51ab, bare global; jump-table target from enemyCode39.
void fireKeese_state_uninitialized_hook(GB *gb) {
  BASE(fireKeese_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x08);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xf8); // -$08
  CYC(b_+9, b_+11); alu_bit(gb, 0, B);
  CYC(b_+11, b_+12); L = E;
  if (F & FZ) { CYCT(b_+12, b_+14); goto subid0; } // jr z
  CYC(b_+12, b_+14);

  // subid1
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0b); // [state]
  CYC(b_+16, b_+19); TAIL(objectSetVisible82); // jp

subid0:
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0b); // [state]
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0xe4); // -$1c
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x14); // SPEED_80

  // Random angle
  CYC(b_+29, b_+32); SET_BC(0x1f01);
  CALL_C(b_+32, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+35);
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+37, b_+38); A = B;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);

  // Set var35 to 1 or -1 for clockwise or counterclockwise movement.
  CYC(b_+39, b_+40); A = C;
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto setVar35; } // jr nz
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); A = alu_dec8(gb, A);

setVar35:
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+49); A = 0x01;
  CALL_C(b_+49, enemySetAnimation_hook, SYM(enemySetAnimation), b_+52);
  CYC(b_+52, b_+55); TAIL(objectSetVisiblec1); // jp
}

// 0e:51e2, bare global; jump-table target from enemyCode39.
void fireKeese_state_stub_hook(GB *gb) {
  BASE(fireKeese_state_stub);
  RET(b_+0); return; // ret
}

// 0e:51e3, bare global; jump-table target from enemyCode39. Just lost fire; looks for a
// torch if one exists, otherwise it will keep flying around like normal.
void fireKeese_state8_hook(GB *gb) {
  BASE(fireKeese_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+2, b_+4); A = 0xff;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+9); C = L;
  CYC(b_+9, b_+11); L = 0x00;

nextTile:
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (F & FZ) CALL_C_CC(b_+14, fireKeese_addCandidateTorch_hook, SYM(fireKeese_addCandidateTorch), b_+17); else CYC(b_+14, b_+17); // call z
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); A = L;
  CYC(b_+19, b_+21); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (F & FC) { CYCT(b_+21, b_+23); goto nextTile; } // jr c
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);
  CYC(b_+27, b_+28); H = D;
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto torchFound; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+32, b_+34); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+34, b_+36); A = 0x0d;
  if (F & FZ) { CYCT(b_+36, b_+38); goto setState; } // jr z
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 120);
  CYC(b_+42, b_+44); A = 0x0c;

setState:
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  RET(b_+47); return; // ret

torchFound:
  CYC(b_+48, b_+50); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+55, b_+57); A = 0x03;
  CYC(b_+57, b_+60); TAIL(enemySetAnimation); // jp
}

// 0e:521f, bare global; jump-table target from enemyCode39. Moving towards a torch's
// position, marked in var31/var32.
void fireKeese_state9_hook(GB *gb) {
  BASE(fireKeese_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x31; // Enemy.var31
  CALL_C(b_+3, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+6);
  CYC(b_+6, b_+7); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto notAtTargetPosition; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = mem_rd(gb, hFF8F); // hFF8F
  CYC(b_+11, b_+12); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+12, b_+14); goto atTargetPosition; } // jr z
  CYC(b_+12, b_+14);

notAtTargetPosition:
  CALL_C(b_+14, fireKeese_moveToGround_hook, SYM(fireKeese_moveToGround), b_+17);
  CALL_C(b_+17, ecom_moveTowardPosition_b0e_hook, SYM(ecom_moveTowardPosition_b0e), b_+20);
  CYC(b_+20, b_+23); TAIL(enemyAnimate); // jp

atTargetPosition:
  CALL_C(b_+23, fireKeese_moveToGround_hook, SYM(fireKeese_moveToGround), b_+26);
  if (F & FC) { RET_TAKEN(b_+26); return; } // ret c
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 60);
  CYC(b_+34, b_+36); A = 0x02;
  CYC(b_+36, b_+39); TAIL(enemySetAnimation); // jp
}

// 0e:5246, bare global; jump-table target from enemyCode39. Touched down on the torch; in
// the process of being lit back on fire.
void fireKeese_stateA_hook(GB *gb) {
  BASE(fireKeese_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto gotoNextState; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+6, b_+8); alu_sub(gb, 30);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);

  // [counter1] == 30
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+11, b_+13); A = 0x05;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xf8); // -$08
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+26); TAIL(enemySetAnimation); // jp

gotoNextState:
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_add(gb, 0x10);
  CYC(b_+31, b_+33); alu_and(gb, 0x1f);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+36, b_+38); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+38, b_+40); A = 0x0d;
  if (F & FZ) { CYCT(b_+40, b_+42); goto setStateFromA; } // jr z
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 120);
  CYC(b_+46, b_+48); A = 0x0c;

setStateFromA:
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+51); A = 0x01;
  CYC(b_+51, b_+54); TAIL(enemySetAnimation); // jp
}

// 0e:527c, bare global; jump-table target from fireKeese_stateBOrHigher. Keese which move
// up and down on Z axis.
void fireKeese_subid0_hook(GB *gb) {
  BASE(fireKeese_subid0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fireKeese_checkForNewlyLitTorch_hook, SYM(fireKeese_checkForNewlyLitTorch), b_+3);
  // Above call may pop its return address; ignore everything below if so.
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_sub(gb, 0x0b);
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == SYM(fireKeese_subid0_stateB)) { fireKeese_subid0_stateB_hook(gb); return; }
    if (target == SYM(fireKeese_subid0_stateC)) { fireKeese_subid0_stateC_hook(gb); return; }
    if (target == SYM(fireKeese_subid0_stateD)) { fireKeese_subid0_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:528b, bare global; jump-table target from fireKeese_subid0. Flying around on fire.
void fireKeese_subid0_stateB_hook(GB *gb) {
  BASE(fireKeese_subid0_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fireKeese_checkCloseToLink_hook, SYM(fireKeese_checkCloseToLink), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); goto linkNotClose; } // jr nc
  CYC(b_+3, b_+5);

  // Link is close
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 91);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x19); // SPEED_a0

linkNotClose:
  CALL_C(b_+16, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+19);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto applySpeed; } // jr nz
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x08); // [counter1]

  // Move clockwise or counterclockwise (var35 is randomly set to 1 or -1 on
  // initialization)
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+31); alu_and(gb, 0x1f);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);

applySpeed:
  CALL_C(b_+32, objectApplySpeed_hook, SYM(objectApplySpeed), b_+35);
  CALL_C(b_+35, fireKeese_moveTowardCenterIfOutOfBounds_hook, SYM(fireKeese_moveTowardCenterIfOutOfBounds), b_+38);
  CYC(b_+38, b_+40); TAIL(fireKeese_animate); // jr
}

// 0e:52b3, bare global; jump-table target from fireKeese_subid0. Divebombing because Link
// got close enough.
void fireKeese_subid0_stateC_hook(GB *gb) {
  BASE(fireKeese_subid0_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto stillDiving; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); TAIL(fireKeese_animate); // jr

stillDiving:
  // Add some amount to Z-position
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+11, b_+13); alu_and(gb, 0xf0);
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+18); SET_HL(SYM(fireKeese_subid0_zOffsets)); // @zOffsets
  fireKeese_addAToHl_from_rst(gb, b_+19);

  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_Z;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); E = alu_inc8(gb, E);
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); alu_adc(gb, 0x00);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);

  // Adjust angle toward Link
  CALL_C(b_+29, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+32);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_and(gb, 0x03);
  CYC(b_+38, b_+39); A = B;
  if (F & FZ) CALL_C_CC(b_+39, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), SYM(fireKeese_updatePosition)); else CYC(b_+39, b_+42); // call z
  TAIL(fireKeese_updatePosition); // fallthrough
}

// 0e:52dd, bare global; falls into from fireKeese_subid0_stateC, also reached by genuine
// call from fireKeese_subid1_stateC.
void fireKeese_updatePosition_hook(GB *gb) {
  BASE(fireKeese_updatePosition);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(fireKeese_animate));
  TAIL(fireKeese_animate); // fallthrough
}

// 0e:52e3, bare global; falls into from fireKeese_updatePosition, also reached by genuine
// jr from fireKeese_subid0_stateB, fireKeese_subid0_stateC and fireKeese_subid0_stateD.
void fireKeese_animate_hook(GB *gb) {
  BASE(fireKeese_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0e:52e6, bare global; jump-table target from fireKeese_subid0. Moving back up after
// divebombing.
void fireKeese_subid0_stateD_hook(GB *gb) {
  BASE(fireKeese_subid0_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x40);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_sbc(gb, 0x00);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); alu_cp(gb, 0xe4);
  if (!(F & FC)) { CYCT(b_+13, b_+15); fireKeese_updatePosition_hook(gb); return; } // jr nc
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0b);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x08);
  CYC(b_+26, b_+28); TAIL(fireKeese_animate); // jr
}

// 0e:5302, bare global; jump-table target from fireKeese_stateBOrHigher. Keese which has no
// Z-axis movement.
void fireKeese_subid1_hook(GB *gb) {
  BASE(fireKeese_subid1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fireKeese_checkForNewlyLitTorch_hook, SYM(fireKeese_checkForNewlyLitTorch), b_+3);
  // Above call may pop its return address; ignore everything below if so.
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_sub(gb, 0x0b);
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == SYM(fireKeese_subid1_stateB)) { fireKeese_subid1_stateB_hook(gb); return; }
    if (target == SYM(fireKeese_subid1_stateC)) { fireKeese_subid1_stateC_hook(gb); return; }
    if (target == SYM(fireKeese_subid1_stateD)) { fireKeese_subid1_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5311, bare global; jump-table target from fireKeese_subid1. Waiting [counter1] frames
// (8 frames) before moving.
void fireKeese_subid1_stateB_hook(GB *gb) {
  BASE(fireKeese_subid1_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x1e); // SPEED_c0

  // Random angle
  CYC(b_+10, b_+13); SET_BC(0x1f3f);
  CALL_C(b_+13, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);

  // Random counter1 between $c0-$ff
  CYC(b_+20, b_+22); A = 0xc0;
  CYC(b_+22, b_+23); alu_add(gb, C);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);

  // Set animation based on if on fire
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CALL_C(b_+30, enemySetAnimation_hook, SYM(enemySetAnimation), b_+33);

  // Create fire when initially spawning
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); alu_or(gb, A);
  CYC(b_+37, b_+39); B = 0x20; // PART_FIRE
  if (F & FZ) CALL_C_CC(b_+39, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+42); else CYC(b_+39, b_+42); // call z
  CYC(b_+42, b_+45); TAIL(enemyAnimate); // jp
}

// 0e:533e, bare global; jump-table target from fireKeese_subid1. Moving around randomly
// for [counter1]*2 frames.
void fireKeese_subid1_stateC_hook(GB *gb) {
  BASE(fireKeese_subid1_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fireKeese_updatePosition_hook, SYM(fireKeese_updatePosition), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CALL_C(b_+9, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+12);
  if (F & FZ) { CYCT(b_+12, b_+14); goto gotoNextState; } // jr z
  CYC(b_+12, b_+14);

  // 1 in 16 chance of changing angle (every 2 frames)
  CYC(b_+14, b_+17); SET_BC(0x0f1f);
  CALL_C(b_+17, ecom_randomBitwiseAndBCE_b0e_hook, SYM(ecom_randomBitwiseAndBCE_b0e), b_+20);
  CYC(b_+20, b_+21); alu_or(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+25); A = C;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return; // ret

gotoNextState:
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+30); return; // ret
}

// 0e:535d, bare global; jump-table target from fireKeese_subid1. Slowing down, then
// stopping for a brief period.
void fireKeese_subid1_stateD_hook(GB *gb) {
  BASE(fireKeese_subid1_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x68);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto slowSpeed; } // jr nc
  CYC(b_+5, b_+7);
  CALL_C(b_+7, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+10);
  CALL_C(b_+10, objectApplySpeed_hook, SYM(objectApplySpeed), b_+13);

slowSpeed:
  CALL_C(b_+13, fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1_hook, SYM(fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1), b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); A = 0x7f;
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);

  // Time to start moving again; go back to state $0b where we'll abruptly go fast.
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x0b);

  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CALL_C(b_+31, enemySetAnimation_hook, SYM(enemySetAnimation), b_+34);
  CALL_C(b_+34, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+37);
  CYC(b_+37, b_+39); alu_and(gb, 0x7f);
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+41, b_+43); alu_add(gb, 0x20);
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  RET(b_+44); return; // ret
}

// 0e:538a, bare global; called from fireKeese_subid1_stateD. Subid 1 slows down gradually
// in state $0d.
void fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1_hook(GB *gb) {
  BASE(fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto animate; } // jr nz
  CYC(b_+5, b_+7);

  // Set speed based on value of counter1
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); A = alu_swap(gb, A);
  CYC(b_+10, b_+13); SET_HL(SYM(fireKeese_subid1_speeds)); // @speeds
  fireKeese_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);

animate:
  // Animate at some rate based on value of counter1
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_and(gb, 0xf0);
  CYC(b_+23, b_+25); A = alu_swap(gb, A);
  CYC(b_+25, b_+28); SET_HL(SYM(fireKeese_subid1_animFrequencies)); // @animFrequencies
  fireKeese_addAToHl_from_rst(gb, b_+29);
  CYC(b_+29, b_+32); A = mem_rd(gb, wFrameCounter);
  CYC(b_+32, b_+33); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+33, b_+36); enemyAnimate_hook(gb); return; } // jp z
  RET(b_+36); return; // ret
}

// 0e:53af, bare global; called from fireKeese_subid0_stateB.
// @param[out] cflag c if Link is within 32 pixels of keese in each direction
void fireKeese_checkCloseToLink_hook(GB *gb) {
  BASE(fireKeese_checkCloseToLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_add(gb, 0x20);
  CYC(b_+8, b_+10); alu_cp(gb, 0x41);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+15); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+18); alu_add(gb, 0x20);
  CYC(b_+18, b_+20); alu_cp(gb, 0x41);
  RET(b_+20); return; // ret
}

// 0e:53c4, bare global; called from fireKeese_state8. Given the position of a torch,
// checks whether to update "position of closest known torch" (var31/var32).
// @param c Position of lit torch
void fireKeese_addCandidateTorch_hook(GB *gb) {
  BASE(fireKeese_addCandidateTorch);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Get Y distance
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); alu_and(gb, 0xf0);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = L;
  CYC(b_+7, b_+9); alu_and(gb, 0xf0);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto yDistDone; } // jr nc
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_cpl(gb);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);

yDistDone:
  CYC(b_+16, b_+17); B = A;

  // Get X distance
  CYC(b_+17, b_+18); A = C;
  CYC(b_+18, b_+20); alu_and(gb, 0x0f);
  CYC(b_+20, b_+21); E = A;
  CYC(b_+21, b_+22); A = L;
  CYC(b_+22, b_+24); alu_and(gb, 0x0f);
  CYC(b_+24, b_+25); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(b_+25, b_+27); goto xDistDone; } // jr nc
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); alu_cpl(gb);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);

xDistDone:
  // Compare with closest candidate, return if farther away
  CYC(b_+29, b_+30); alu_add(gb, B);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+33); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); alu_cp(gb, B);
  if (F & FC) { RET_TAKEN(b_+35); return; } // ret c
  CYC(b_+35, b_+36);

  // This is the closest torch found so far.
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);

  // Mark its position in var31/var32
  CYC(b_+38, b_+40); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+40, b_+41); A = L;
  CYC(b_+41, b_+43); alu_and(gb, 0xf0);
  CYC(b_+43, b_+45); alu_add(gb, 0x08);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); E = alu_inc8(gb, E);
  CYC(b_+47, b_+48); A = L;
  CYC(b_+48, b_+50); alu_and(gb, 0x0f);
  CYC(b_+50, b_+52); A = alu_swap(gb, A);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  RET(b_+55); return; // ret
}

// 0e:53fc, bare global; called from fireKeese_subid0 and fireKeese_subid1. While the keese
// is not lit on fire, this function checks if any new lit torches suddenly appear in the
// room. If so, it sets the state to 8 and returns from the caller (discards return
// address).
void fireKeese_checkForNewlyLitTorch_hook(GB *gb) {
  BASE(fireKeese_checkForNewlyLitTorch);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Return if on fire already
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);

  // Check $16 tiles per frame, searching for a torch. (Searching all of them could
  // cause lag, especially with a lot of bats on-screen.)
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); L = A;
  CYC(b_+9, b_+11); H = 0xcf; // >wRoomLayout
  CYC(b_+11, b_+13); B = 0x16;

loop:
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+14, b_+16); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (F & FZ) { CYCT(b_+16, b_+18); goto foundTorch; } // jr z
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto loop; } // jr nz
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = L;
  CYC(b_+22, b_+24); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto notWrapped; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A);

notWrapped:
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret

foundTorch:
  POP(b_+29); // pop hl -- return from caller, discarding its return address
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+32); L = E;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x00); // [var34]

  // State 8 will cause the bat to move toward the torch.
  // (var31/var32 are not set here because the search will be done again in state 8.)
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x08);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x1e); // SPEED_c0
  RET(b_+42); return; // ret
}

// 0e:5427, bare global; called from fireKeese_state9.
// @param[out] cflag nc if reached ground (or at most 6 units away)
void fireKeese_moveToGround_hook(GB *gb) {
  BASE(fireKeese_moveToGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); alu_cp(gb, 0xfa);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; } // ret nc
  CYC(b_+7, b_+8);

  // [Enemy.z] += $0080
  CYC(b_+8, b_+9); E = alu_dec8(gb, E);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_add(gb, 0x80);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_adc(gb, 0x00);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); alu_scf(gb);
  RET(b_+19); return; // ret
}

// 0e:543b, bare global; called from fireKeese_subid0_stateB.
void fireKeese_moveTowardCenterIfOutOfBounds_hook(GB *gb) {
  BASE(fireKeese_moveTowardCenterIfOutOfBounds);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto outOfBounds; } // jr nc
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_cp(gb, 0xf0);
  if (F & FC) { RET_TAKEN(b_+12); return; } // ret c
  CYC(b_+12, b_+13);

outOfBounds:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); mem_wr(gb, hFF8F, A); // hFF8F
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); mem_wr(gb, hFF8E, A); // hFF8E

  CYC(b_+23, b_+26); SET_BC(0x5878); // ((LARGE_ROOM_HEIGHT/2)<<4)+8, ((LARGE_ROOM_WIDTH/2)<<4)+8
  CALL_C(b_+26, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+29);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+32); B = 0x28; // SPEED_100
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+34, b_+37); TAIL(objectApplyGivenSpeed); // jp
}
