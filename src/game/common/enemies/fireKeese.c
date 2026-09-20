#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x514f, 0x5151); goto normalStatus; } // jr z
  CYC(0x514f, 0x5151);
  CYC(0x5151, 0x5153); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5153); return; } // ret c
  CYC(0x5153, 0x5154);
  if (F & FZ) { CYCT(0x5154, 0x5157); enemyDie_hook(gb); return; } // jp z
  CYC(0x5154, 0x5157);
  CYC(0x5157, 0x5158); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5158, 0x515b); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(0x5158, 0x515b);
  CYC(0x515b, 0x515d); E = OBJ_VAR2A;
  CYC(0x515d, 0x515e); A = mem_rd(gb, DE);
  CYC(0x515e, 0x5160); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(0x5160); return; } // ret nz
  CYC(0x5160, 0x5161);
  CYC(0x5161, 0x5163); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x5163, 0x5164); A = mem_rd(gb, DE);
  CYC(0x5164, 0x5165); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5165); return; } // ret nz
  CYC(0x5165, 0x5166);
  CYC(0x5166, 0x5168); B = 0x20; // PART_FIRE
  CALL_C(0x5168, ecom_spawnProjectile_b0e_hook, 0x437c, 0x516b);
  CYC(0x516b, 0x516c); H = D;
  CYC(0x516c, 0x516e); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(0x516e, 0x5170); A = 0x01;
  CYC(0x5170, 0x5171); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x5171, 0x5172); mem_wr(gb, HL, A);
  CYC(0x5172, 0x5174); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5174, 0x5176); mem_wr(gb, HL, 0x08);
  CYC(0x5176, 0x5178); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(0x5178, 0x517a); mem_wr(gb, HL, 0xfc); // -$04
  CYC(0x517a, 0x517c); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x517c, 0x517e); mem_wr(gb, HL, 0x02);
  CYC(0x517e, 0x5180); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5180, 0x5182); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x5182, 0x5184); A = 0x03;
  CYC(0x5184, 0x5187); enemySetAnimation_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5187, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x518a);
  CYC(0x518a, 0x518c); alu_cp(gb, 0x0b);
  if (!(F & FC)) { CYCT(0x518c, 0x518e); fireKeese_stateBOrHigher_hook(gb); return; } // jr nc
  CYC(0x518c, 0x518e);
  {
    CYC(0x518e, 0x518f); push_effect(gb, 0x518f);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == 0x51ab) { fireKeese_state_uninitialized_hook(gb); return; }
    if (target == 0x51e2) { fireKeese_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x51e3) { fireKeese_state8_hook(gb); return; }
    if (target == 0x521f) { fireKeese_state9_hook(gb); return; }
    if (target == 0x5246) { fireKeese_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:51a5, bare global; jump-table target from enemyCode39.
void fireKeese_stateBOrHigher_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51a5, 0x51a6); A = B;
  {
    CYC(0x51a6, 0x51a7); push_effect(gb, 0x51a7);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == 0x527c) { fireKeese_subid0_hook(gb); return; }
    if (target == 0x5302) { fireKeese_subid1_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:51ab, bare global; jump-table target from enemyCode39.
void fireKeese_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51ab, 0x51ac); H = D;
  CYC(0x51ac, 0x51ae); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x51ae, 0x51b0); mem_wr(gb, HL, 0x08);
  CYC(0x51b0, 0x51b2); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(0x51b2, 0x51b4); mem_wr(gb, HL, 0xf8); // -$08
  CYC(0x51b4, 0x51b6); alu_bit(gb, 0, B);
  CYC(0x51b6, 0x51b7); L = E;
  if (F & FZ) { CYCT(0x51b7, 0x51b9); goto subid0; } // jr z
  CYC(0x51b7, 0x51b9);

  // subid1
  CYC(0x51b9, 0x51bb); mem_wr(gb, HL, 0x0b); // [state]
  CYC(0x51bb, 0x51be); objectSetVisible82_hook(gb); return; // jp

subid0:
  CYC(0x51be, 0x51c0); mem_wr(gb, HL, 0x0b); // [state]
  CYC(0x51c0, 0x51c2); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x51c2, 0x51c4); mem_wr(gb, HL, 0xe4); // -$1c
  CYC(0x51c4, 0x51c6); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x51c6, 0x51c8); mem_wr(gb, HL, 0x14); // SPEED_80

  // Random angle
  CYC(0x51c8, 0x51cb); SET_BC(0x1f01);
  CALL_C(0x51cb, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x51ce);
  CYC(0x51ce, 0x51d0); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x51d0, 0x51d1); A = B;
  CYC(0x51d1, 0x51d2); mem_wr(gb, DE, A);

  // Set var35 to 1 or -1 for clockwise or counterclockwise movement.
  CYC(0x51d2, 0x51d3); A = C;
  CYC(0x51d3, 0x51d4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x51d4, 0x51d6); goto setVar35; } // jr nz
  CYC(0x51d4, 0x51d6);
  CYC(0x51d6, 0x51d7); A = alu_dec8(gb, A);

setVar35:
  CYC(0x51d7, 0x51d9); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x51d9, 0x51da); mem_wr(gb, DE, A);
  CYC(0x51da, 0x51dc); A = 0x01;
  CALL_C(0x51dc, enemySetAnimation_hook, 0x282b, 0x51df);
  CYC(0x51df, 0x51e2); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:51e2, bare global; jump-table target from enemyCode39.
void fireKeese_state_stub_hook(GB *gb) {
  RET(0x51e2); return; // ret
}

// 0e:51e3, bare global; jump-table target from enemyCode39. Just lost fire; looks for a
// torch if one exists, otherwise it will keep flying around like normal.
void fireKeese_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51e3, 0x51e5); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x51e5, 0x51e7); A = 0xff;
  CYC(0x51e7, 0x51e8); mem_wr(gb, DE, A);
  CALL_C(0x51e8, objectGetTileAtPosition_hook, 0x1444, 0x51eb);
  CYC(0x51eb, 0x51ec); C = L;
  CYC(0x51ec, 0x51ee); L = 0x00;

nextTile:
  CYC(0x51ee, 0x51ef); A = mem_rd(gb, HL);
  CYC(0x51ef, 0x51f1); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (F & FZ) CALL_C_CC(0x51f1, fireKeese_addCandidateTorch_hook, 0x53c4, 0x51f4); else CYC(0x51f1, 0x51f4); // call z
  CYC(0x51f4, 0x51f5); L = alu_inc8(gb, L);
  CYC(0x51f5, 0x51f6); A = L;
  CYC(0x51f6, 0x51f8); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (F & FC) { CYCT(0x51f8, 0x51fa); goto nextTile; } // jr c
  CYC(0x51f8, 0x51fa);
  CYC(0x51fa, 0x51fc); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x51fc, 0x51fd); A = mem_rd(gb, DE);
  CYC(0x51fd, 0x51fe); A = alu_inc8(gb, A);
  CYC(0x51fe, 0x51ff); H = D;
  if (!(F & FZ)) { CYCT(0x51ff, 0x5201); goto torchFound; } // jr nz
  CYC(0x51ff, 0x5201);
  CYC(0x5201, 0x5203); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5203, 0x5205); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x5205, 0x5207); A = 0x0d;
  if (F & FZ) { CYCT(0x5207, 0x5209); goto setState; } // jr z
  CYC(0x5207, 0x5209);
  CYC(0x5209, 0x520b); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x520b, 0x520d); mem_wr(gb, HL, 120);
  CYC(0x520d, 0x520f); A = 0x0c;

setState:
  CYC(0x520f, 0x5211); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5211, 0x5212); mem_wr(gb, HL, A);
  RET(0x5212); return; // ret

torchFound:
  CYC(0x5213, 0x5215); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5215, 0x5216); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5216, 0x5218); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5218, 0x521a); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x521a, 0x521c); A = 0x03;
  CYC(0x521c, 0x521f); enemySetAnimation_hook(gb); return; // jp
}

// 0e:521f, bare global; jump-table target from enemyCode39. Moving towards a torch's
// position, marked in var31/var32.
void fireKeese_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x521f, 0x5220); H = D;
  CYC(0x5220, 0x5222); L = ENEMY_BASE + 0x31; // Enemy.var31
  CALL_C(0x5222, ecom_readPositionVars_b0e_hook, 0x4439, 0x5225);
  CYC(0x5225, 0x5226); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x5226, 0x5228); goto notAtTargetPosition; } // jr nz
  CYC(0x5226, 0x5228);
  CYC(0x5228, 0x522a); A = hram_rd(gb, 0x8f); // hFF8F
  CYC(0x522a, 0x522b); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x522b, 0x522d); goto atTargetPosition; } // jr z
  CYC(0x522b, 0x522d);

notAtTargetPosition:
  CALL_C(0x522d, fireKeese_moveToGround_hook, 0x5427, 0x5230);
  CALL_C(0x5230, ecom_moveTowardPosition_b0e_hook, 0x4430, 0x5233);
  CYC(0x5233, 0x5236); enemyAnimate_hook(gb); return; // jp

atTargetPosition:
  CALL_C(0x5236, fireKeese_moveToGround_hook, 0x5427, 0x5239);
  if (F & FC) { RET_TAKEN(0x5239); return; } // ret c
  CYC(0x5239, 0x523a);
  CYC(0x523a, 0x523c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x523c, 0x523d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x523d, 0x523f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x523f, 0x5241); mem_wr(gb, HL, 60);
  CYC(0x5241, 0x5243); A = 0x02;
  CYC(0x5243, 0x5246); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5246, bare global; jump-table target from enemyCode39. Touched down on the torch; in
// the process of being lit back on fire.
void fireKeese_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5246, ecom_decCounter1_b0e_hook, 0x439a, 0x5249);
  if (F & FZ) { CYCT(0x5249, 0x524b); goto gotoNextState; } // jr z
  CYC(0x5249, 0x524b);
  CYC(0x524b, 0x524c); A = mem_rd(gb, HL); // [counter1]
  CYC(0x524c, 0x524e); alu_sub(gb, 30);
  if (!(F & FZ)) { RET_TAKEN(0x524e); return; } // ret nz
  CYC(0x524e, 0x524f);

  // [counter1] == 30
  CYC(0x524f, 0x5251); L = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(0x5251, 0x5253); A = 0x05;
  CYC(0x5253, 0x5254); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x5254, 0x5255); mem_wr(gb, HL, A);
  CYC(0x5255, 0x5257); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(0x5257, 0x5259); mem_wr(gb, HL, 0xf8); // -$08
  CYC(0x5259, 0x525b); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x525b, 0x525c); alu_xor(gb, A);
  CYC(0x525c, 0x525d); mem_wr(gb, HL, A);
  CYC(0x525d, 0x5260); enemySetAnimation_hook(gb); return; // jp

gotoNextState:
  CYC(0x5260, 0x5262); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5262, 0x5263); A = mem_rd(gb, HL);
  CYC(0x5263, 0x5265); alu_add(gb, 0x10);
  CYC(0x5265, 0x5267); alu_and(gb, 0x1f);
  CYC(0x5267, 0x5268); mem_wr(gb, HL, A);
  CYC(0x5268, 0x526a); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x526a, 0x526c); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x526c, 0x526e); A = 0x0d;
  if (F & FZ) { CYCT(0x526e, 0x5270); goto setStateFromA; } // jr z
  CYC(0x526e, 0x5270);
  CYC(0x5270, 0x5272); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5272, 0x5274); mem_wr(gb, HL, 120);
  CYC(0x5274, 0x5276); A = 0x0c;

setStateFromA:
  CYC(0x5276, 0x5277); mem_wr(gb, DE, A);
  CYC(0x5277, 0x5279); A = 0x01;
  CYC(0x5279, 0x527c); enemySetAnimation_hook(gb); return; // jp
}

// 0e:527c, bare global; jump-table target from fireKeese_stateBOrHigher. Keese which move
// up and down on Z axis.
void fireKeese_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x527c, fireKeese_checkForNewlyLitTorch_hook, 0x53fc, 0x527f);
  // Above call may pop its return address; ignore everything below if so.
  CYC(0x527f, 0x5281); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5281, 0x5282); A = mem_rd(gb, DE);
  CYC(0x5282, 0x5284); alu_sub(gb, 0x0b);
  {
    CYC(0x5284, 0x5285); push_effect(gb, 0x5285);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == 0x528b) { fireKeese_subid0_stateB_hook(gb); return; }
    if (target == 0x52b3) { fireKeese_subid0_stateC_hook(gb); return; }
    if (target == 0x52e6) { fireKeese_subid0_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:528b, bare global; jump-table target from fireKeese_subid0. Flying around on fire.
void fireKeese_subid0_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x528b, fireKeese_checkCloseToLink_hook, 0x53af, 0x528e);
  if (!(F & FC)) { CYCT(0x528e, 0x5290); goto linkNotClose; } // jr nc
  CYC(0x528e, 0x5290);

  // Link is close
  CYC(0x5290, 0x5292); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5292, 0x5293); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5293, 0x5295); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5295, 0x5297); mem_wr(gb, HL, 91);
  CYC(0x5297, 0x5299); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5299, 0x529b); mem_wr(gb, HL, 0x19); // SPEED_a0

linkNotClose:
  CALL_C(0x529b, ecom_decCounter1_b0e_hook, 0x439a, 0x529e);
  if (!(F & FZ)) { CYCT(0x529e, 0x52a0); goto applySpeed; } // jr nz
  CYC(0x529e, 0x52a0);
  CYC(0x52a0, 0x52a2); mem_wr(gb, HL, 0x08); // [counter1]

  // Move clockwise or counterclockwise (var35 is randomly set to 1 or -1 on
  // initialization)
  CYC(0x52a2, 0x52a4); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x52a4, 0x52a5); A = mem_rd(gb, DE);
  CYC(0x52a5, 0x52a7); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x52a7, 0x52a8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x52a8, 0x52aa); alu_and(gb, 0x1f);
  CYC(0x52aa, 0x52ab); mem_wr(gb, HL, A);

applySpeed:
  CALL_C(0x52ab, objectApplySpeed_hook, 0x201d, 0x52ae);
  CALL_C(0x52ae, fireKeese_moveTowardCenterIfOutOfBounds_hook, 0x543b, 0x52b1);
  CYC(0x52b1, 0x52b3); fireKeese_animate_hook(gb); return; // jr
}

// 0e:52b3, bare global; jump-table target from fireKeese_subid0. Divebombing because Link
// got close enough.
void fireKeese_subid0_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x52b3, ecom_decCounter1_b0e_hook, 0x439a, 0x52b6);
  if (!(F & FZ)) { CYCT(0x52b6, 0x52b8); goto stillDiving; } // jr nz
  CYC(0x52b6, 0x52b8);
  CYC(0x52b8, 0x52ba); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x52ba, 0x52bb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x52bb, 0x52bd); fireKeese_animate_hook(gb); return; // jr

stillDiving:
  // Add some amount to Z-position
  CYC(0x52bd, 0x52be); A = mem_rd(gb, HL); // [counter1]
  CYC(0x52be, 0x52c0); alu_and(gb, 0xf0);
  CYC(0x52c0, 0x52c2); A = alu_swap(gb, A);
  CYC(0x52c2, 0x52c5); SET_HL(0x5460); // @zOffsets
  fireKeese_addAToHl_from_rst(gb, 0x52c6);

  CYC(0x52c6, 0x52c8); E = ENEMY_BASE + OBJ_Z;
  CYC(0x52c8, 0x52c9); A = mem_rd(gb, DE);
  CYC(0x52c9, 0x52ca); alu_add(gb, mem_rd(gb, HL));
  CYC(0x52ca, 0x52cb); mem_wr(gb, DE, A);
  CYC(0x52cb, 0x52cc); E = alu_inc8(gb, E);
  CYC(0x52cc, 0x52cd); A = mem_rd(gb, DE);
  CYC(0x52cd, 0x52cf); alu_adc(gb, 0x00);
  CYC(0x52cf, 0x52d0); mem_wr(gb, DE, A);

  // Adjust angle toward Link
  CALL_C(0x52d0, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x52d3);
  CYC(0x52d3, 0x52d4); B = A;
  CYC(0x52d4, 0x52d6); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x52d6, 0x52d7); A = mem_rd(gb, DE);
  CYC(0x52d7, 0x52d9); alu_and(gb, 0x03);
  CYC(0x52d9, 0x52da); A = B;
  if (F & FZ) CALL_C_CC(0x52da, objectNudgeAngleTowards_hook, 0x1fd4, 0x52dd); else CYC(0x52da, 0x52dd); // call z
  fireKeese_updatePosition_hook(gb); return; // fallthrough
}

// 0e:52dd, bare global; falls into from fireKeese_subid0_stateC, also reached by genuine
// call from fireKeese_subid1_stateC.
void fireKeese_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x52dd, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x52e0);
  CALL_C(0x52e0, objectApplySpeed_hook, 0x201d, 0x52e3);
  fireKeese_animate_hook(gb); return; // fallthrough
}

// 0e:52e3, bare global; falls into from fireKeese_updatePosition, also reached by genuine
// jr from fireKeese_subid0_stateB, fireKeese_subid0_stateC and fireKeese_subid0_stateD.
void fireKeese_animate_hook(GB *gb) {
  CYC(0x52e3, 0x52e6); enemyAnimate_hook(gb); return; // jp
}

// 0e:52e6, bare global; jump-table target from fireKeese_subid0. Moving back up after
// divebombing.
void fireKeese_subid0_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52e6, 0x52e7); H = D;
  CYC(0x52e7, 0x52e9); L = ENEMY_BASE + OBJ_Z;
  CYC(0x52e9, 0x52ea); A = mem_rd(gb, HL);
  CYC(0x52ea, 0x52ec); alu_sub(gb, 0x40);
  CYC(0x52ec, 0x52ed); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x52ed, 0x52ee); A = mem_rd(gb, HL);
  CYC(0x52ee, 0x52f0); alu_sbc(gb, 0x00);
  CYC(0x52f0, 0x52f1); mem_wr(gb, HL, A);
  CYC(0x52f1, 0x52f3); alu_cp(gb, 0xe4);
  if (!(F & FC)) { CYCT(0x52f3, 0x52f5); fireKeese_updatePosition_hook(gb); return; } // jr nc
  CYC(0x52f3, 0x52f5);
  CYC(0x52f5, 0x52f6); L = E;
  CYC(0x52f6, 0x52f8); mem_wr(gb, HL, 0x0b);
  CYC(0x52f8, 0x52fa); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x52fa, 0x52fc); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x52fc, 0x52fe); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x52fe, 0x5300); mem_wr(gb, HL, 0x08);
  CYC(0x5300, 0x5302); fireKeese_animate_hook(gb); return; // jr
}

// 0e:5302, bare global; jump-table target from fireKeese_stateBOrHigher. Keese which has no
// Z-axis movement.
void fireKeese_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5302, fireKeese_checkForNewlyLitTorch_hook, 0x53fc, 0x5305);
  // Above call may pop its return address; ignore everything below if so.
  CYC(0x5305, 0x5307); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5307, 0x5308); A = mem_rd(gb, DE);
  CYC(0x5308, 0x530a); alu_sub(gb, 0x0b);
  {
    CYC(0x530a, 0x530b); push_effect(gb, 0x530b);
    uint16_t target = fireKeese_jump_table(gb);
    if (target == 0x5311) { fireKeese_subid1_stateB_hook(gb); return; }
    if (target == 0x533e) { fireKeese_subid1_stateC_hook(gb); return; }
    if (target == 0x535d) { fireKeese_subid1_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5311, bare global; jump-table target from fireKeese_subid1. Waiting [counter1] frames
// (8 frames) before moving.
void fireKeese_subid1_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5311, ecom_decCounter1_b0e_hook, 0x439a, 0x5314);
  if (!(F & FZ)) { RET_TAKEN(0x5314); return; } // ret nz
  CYC(0x5314, 0x5315);
  CYC(0x5315, 0x5316); L = E;
  CYC(0x5316, 0x5317); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x5317, 0x5319); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5319, 0x531b); mem_wr(gb, HL, 0x1e); // SPEED_c0

  // Random angle
  CYC(0x531b, 0x531e); SET_BC(0x1f3f);
  CALL_C(0x531e, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x5321);
  CYC(0x5321, 0x5323); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5323, 0x5324); A = B;
  CYC(0x5324, 0x5325); mem_wr(gb, DE, A);

  // Random counter1 between $c0-$ff
  CYC(0x5325, 0x5327); A = 0xc0;
  CYC(0x5327, 0x5328); alu_add(gb, C);
  CYC(0x5328, 0x532a); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x532a, 0x532b); mem_wr(gb, DE, A);

  // Set animation based on if on fire
  CYC(0x532b, 0x532d); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x532d, 0x532e); A = mem_rd(gb, DE);
  CYC(0x532e, 0x532f); A = alu_inc8(gb, A);
  CALL_C(0x532f, enemySetAnimation_hook, 0x282b, 0x5332);

  // Create fire when initially spawning
  CYC(0x5332, 0x5334); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x5334, 0x5335); A = mem_rd(gb, DE);
  CYC(0x5335, 0x5336); alu_or(gb, A);
  CYC(0x5336, 0x5338); B = 0x20; // PART_FIRE
  if (F & FZ) CALL_C_CC(0x5338, ecom_spawnProjectile_b0e_hook, 0x437c, 0x533b); else CYC(0x5338, 0x533b); // call z
  CYC(0x533b, 0x533e); enemyAnimate_hook(gb); return; // jp
}

// 0e:533e, bare global; jump-table target from fireKeese_subid1. Moving around randomly
// for [counter1]*2 frames.
void fireKeese_subid1_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x533e, fireKeese_updatePosition_hook, 0x52dd, 0x5341);
  CYC(0x5341, 0x5344); A = mem_rd(gb, wFrameCounter);
  CYC(0x5344, 0x5346); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x5346); return; } // ret nz
  CYC(0x5346, 0x5347);
  CALL_C(0x5347, ecom_decCounter1_b0e_hook, 0x439a, 0x534a);
  if (F & FZ) { CYCT(0x534a, 0x534c); goto gotoNextState; } // jr z
  CYC(0x534a, 0x534c);

  // 1 in 16 chance of changing angle (every 2 frames)
  CYC(0x534c, 0x534f); SET_BC(0x0f1f);
  CALL_C(0x534f, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x5352);
  CYC(0x5352, 0x5353); alu_or(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x5353); return; } // ret nz
  CYC(0x5353, 0x5354);
  CYC(0x5354, 0x5356); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5356, 0x5357); A = C;
  CYC(0x5357, 0x5358); mem_wr(gb, DE, A);
  RET(0x5358); return; // ret

gotoNextState:
  CYC(0x5359, 0x535b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x535b, 0x535c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x535c); return; // ret
}

// 0e:535d, bare global; jump-table target from fireKeese_subid1. Slowing down, then
// stopping for a brief period.
void fireKeese_subid1_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x535d, 0x535f); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x535f, 0x5360); A = mem_rd(gb, DE);
  CYC(0x5360, 0x5362); alu_cp(gb, 0x68);
  if (!(F & FC)) { CYCT(0x5362, 0x5364); goto slowSpeed; } // jr nc
  CYC(0x5362, 0x5364);
  CALL_C(0x5364, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x5367);
  CALL_C(0x5367, objectApplySpeed_hook, 0x201d, 0x536a);

slowSpeed:
  CALL_C(0x536a, fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1_hook, 0x538a, 0x536d);
  CYC(0x536d, 0x536e); H = D;
  CYC(0x536e, 0x5370); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5370, 0x5371); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5371, 0x5373); A = 0x7f;
  CYC(0x5373, 0x5374); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x5374); return; } // ret nz
  CYC(0x5374, 0x5375);

  // Time to start moving again; go back to state $0b where we'll abruptly go fast.
  CYC(0x5375, 0x5377); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5377, 0x5379); mem_wr(gb, HL, 0x0b);

  CYC(0x5379, 0x537b); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x537b, 0x537c); A = mem_rd(gb, DE);
  CALL_C(0x537c, enemySetAnimation_hook, 0x282b, 0x537f);
  CALL_C(0x537f, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5382);
  CYC(0x5382, 0x5384); alu_and(gb, 0x7f);
  CYC(0x5384, 0x5386); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5386, 0x5388); alu_add(gb, 0x20);
  CYC(0x5388, 0x5389); mem_wr(gb, DE, A);
  RET(0x5389); return; // ret
}

// 0e:538a, bare global; called from fireKeese_subid1_stateD. Subid 1 slows down gradually
// in state $0d.
void fireKeese_subid1_setSpeedAndAnimateBasedOnCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x538a, 0x538c); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x538c, 0x538d); A = mem_rd(gb, DE);
  CYC(0x538d, 0x538f); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x538f, 0x5391); goto animate; } // jr nz
  CYC(0x538f, 0x5391);

  // Set speed based on value of counter1
  CYC(0x5391, 0x5392); A = mem_rd(gb, DE);
  CYC(0x5392, 0x5394); A = alu_swap(gb, A);
  CYC(0x5394, 0x5397); SET_HL(0x5466); // @speeds
  fireKeese_addAToHl_from_rst(gb, 0x5398);
  CYC(0x5398, 0x539a); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x539a, 0x539b); A = mem_rd(gb, HL);
  CYC(0x539b, 0x539c); mem_wr(gb, DE, A);

animate:
  // Animate at some rate based on value of counter1
  CYC(0x539c, 0x539e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x539e, 0x539f); A = mem_rd(gb, DE);
  CYC(0x539f, 0x53a1); alu_and(gb, 0xf0);
  CYC(0x53a1, 0x53a3); A = alu_swap(gb, A);
  CYC(0x53a3, 0x53a6); SET_HL(0x546e); // @animFrequencies
  fireKeese_addAToHl_from_rst(gb, 0x53a7);
  CYC(0x53a7, 0x53aa); A = mem_rd(gb, wFrameCounter);
  CYC(0x53aa, 0x53ab); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x53ab, 0x53ae); enemyAnimate_hook(gb); return; } // jp z
  RET(0x53ae); return; // ret
}

// 0e:53af, bare global; called from fireKeese_subid0_stateB.
// @param[out] cflag c if Link is within 32 pixels of keese in each direction
void fireKeese_checkCloseToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x53af, 0x53b0); H = D;
  CYC(0x53b0, 0x53b2); L = ENEMY_BASE + OBJ_YH;
  CYC(0x53b2, 0x53b4); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x53b4, 0x53b5); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x53b5, 0x53b7); alu_add(gb, 0x20);
  CYC(0x53b7, 0x53b9); alu_cp(gb, 0x41);
  if (!(F & FC)) { RET_TAKEN(0x53b9); return; } // ret nc
  CYC(0x53b9, 0x53ba);
  CYC(0x53ba, 0x53bc); L = ENEMY_BASE + OBJ_XH;
  CYC(0x53bc, 0x53be); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x53be, 0x53bf); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x53bf, 0x53c1); alu_add(gb, 0x20);
  CYC(0x53c1, 0x53c3); alu_cp(gb, 0x41);
  RET(0x53c3); return; // ret
}

// 0e:53c4, bare global; called from fireKeese_state8. Given the position of a torch,
// checks whether to update "position of closest known torch" (var31/var32).
// @param c Position of lit torch
void fireKeese_addCandidateTorch_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Get Y distance
  CYC(0x53c4, 0x53c5); A = C;
  CYC(0x53c5, 0x53c7); alu_and(gb, 0xf0);
  CYC(0x53c7, 0x53c9); A = alu_swap(gb, A);
  CYC(0x53c9, 0x53ca); B = A;
  CYC(0x53ca, 0x53cb); A = L;
  CYC(0x53cb, 0x53cd); alu_and(gb, 0xf0);
  CYC(0x53cd, 0x53cf); A = alu_swap(gb, A);
  CYC(0x53cf, 0x53d0); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(0x53d0, 0x53d2); goto yDistDone; } // jr nc
  CYC(0x53d0, 0x53d2);
  CYC(0x53d2, 0x53d3); alu_cpl(gb);
  CYC(0x53d3, 0x53d4); A = alu_inc8(gb, A);

yDistDone:
  CYC(0x53d4, 0x53d5); B = A;

  // Get X distance
  CYC(0x53d5, 0x53d6); A = C;
  CYC(0x53d6, 0x53d8); alu_and(gb, 0x0f);
  CYC(0x53d8, 0x53d9); E = A;
  CYC(0x53d9, 0x53da); A = L;
  CYC(0x53da, 0x53dc); alu_and(gb, 0x0f);
  CYC(0x53dc, 0x53dd); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(0x53dd, 0x53df); goto xDistDone; } // jr nc
  CYC(0x53dd, 0x53df);
  CYC(0x53df, 0x53e0); alu_cpl(gb);
  CYC(0x53e0, 0x53e1); A = alu_inc8(gb, A);

xDistDone:
  // Compare with closest candidate, return if farther away
  CYC(0x53e1, 0x53e2); alu_add(gb, B);
  CYC(0x53e2, 0x53e3); B = A;
  CYC(0x53e3, 0x53e5); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x53e5, 0x53e6); A = mem_rd(gb, DE);
  CYC(0x53e6, 0x53e7); alu_cp(gb, B);
  if (F & FC) { RET_TAKEN(0x53e7); return; } // ret c
  CYC(0x53e7, 0x53e8);

  // This is the closest torch found so far.
  CYC(0x53e8, 0x53e9); A = B;
  CYC(0x53e9, 0x53ea); mem_wr(gb, DE, A);

  // Mark its position in var31/var32
  CYC(0x53ea, 0x53ec); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x53ec, 0x53ed); A = L;
  CYC(0x53ed, 0x53ef); alu_and(gb, 0xf0);
  CYC(0x53ef, 0x53f1); alu_add(gb, 0x08);
  CYC(0x53f1, 0x53f2); mem_wr(gb, DE, A);
  CYC(0x53f2, 0x53f3); E = alu_inc8(gb, E);
  CYC(0x53f3, 0x53f4); A = L;
  CYC(0x53f4, 0x53f6); alu_and(gb, 0x0f);
  CYC(0x53f6, 0x53f8); A = alu_swap(gb, A);
  CYC(0x53f8, 0x53fa); alu_add(gb, 0x08);
  CYC(0x53fa, 0x53fb); mem_wr(gb, DE, A);
  RET(0x53fb); return; // ret
}

// 0e:53fc, bare global; called from fireKeese_subid0 and fireKeese_subid1. While the keese
// is not lit on fire, this function checks if any new lit torches suddenly appear in the
// room. If so, it sets the state to 8 and returns from the caller (discards return
// address).
void fireKeese_checkForNewlyLitTorch_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Return if on fire already
  CYC(0x53fc, 0x53fe); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x53fe, 0x53ff); A = mem_rd(gb, DE);
  CYC(0x53ff, 0x5400); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5400); return; } // ret z
  CYC(0x5400, 0x5401);

  // Check $16 tiles per frame, searching for a torch. (Searching all of them could
  // cause lag, especially with a lot of bats on-screen.)
  CYC(0x5401, 0x5403); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x5403, 0x5404); A = mem_rd(gb, DE);
  CYC(0x5404, 0x5405); L = A;
  CYC(0x5405, 0x5407); H = 0xcf; // >wRoomLayout
  CYC(0x5407, 0x5409); B = 0x16;

loop:
  CYC(0x5409, 0x540a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x540a, 0x540c); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (F & FZ) { CYCT(0x540c, 0x540e); goto foundTorch; } // jr z
  CYC(0x540c, 0x540e);
  CYC(0x540e, 0x540f); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x540f, 0x5411); goto loop; } // jr nz
  CYC(0x540f, 0x5411);
  CYC(0x5411, 0x5412); A = L;
  CYC(0x5412, 0x5414); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FZ)) { CYCT(0x5414, 0x5416); goto notWrapped; } // jr nz
  CYC(0x5414, 0x5416);
  CYC(0x5416, 0x5417); alu_xor(gb, A);

notWrapped:
  CYC(0x5417, 0x5418); mem_wr(gb, DE, A);
  RET(0x5418); return; // ret

foundTorch:
  POP(0x5419); // pop hl -- return from caller, discarding its return address
  CYC(0x541a, 0x541b); H = D;
  CYC(0x541b, 0x541c); L = E;
  CYC(0x541c, 0x541e); mem_wr(gb, HL, 0x00); // [var34]

  // State 8 will cause the bat to move toward the torch.
  // (var31/var32 are not set here because the search will be done again in state 8.)
  CYC(0x541e, 0x5420); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5420, 0x5422); mem_wr(gb, HL, 0x08);
  CYC(0x5422, 0x5424); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5424, 0x5426); mem_wr(gb, HL, 0x1e); // SPEED_c0
  RET(0x5426); return; // ret
}

// 0e:5427, bare global; called from fireKeese_state9.
// @param[out] cflag nc if reached ground (or at most 6 units away)
void fireKeese_moveToGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5427, 0x5429); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x5429, 0x542a); A = mem_rd(gb, DE);
  CYC(0x542a, 0x542b); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x542b); return; } // ret z
  CYC(0x542b, 0x542c);
  CYC(0x542c, 0x542e); alu_cp(gb, 0xfa);
  if (!(F & FC)) { RET_TAKEN(0x542e); return; } // ret nc
  CYC(0x542e, 0x542f);

  // [Enemy.z] += $0080
  CYC(0x542f, 0x5430); E = alu_dec8(gb, E);
  CYC(0x5430, 0x5431); A = mem_rd(gb, DE);
  CYC(0x5431, 0x5433); alu_add(gb, 0x80);
  CYC(0x5433, 0x5434); mem_wr(gb, DE, A);
  CYC(0x5434, 0x5435); E = alu_inc8(gb, E);
  CYC(0x5435, 0x5436); A = mem_rd(gb, DE);
  CYC(0x5436, 0x5438); alu_adc(gb, 0x00);
  CYC(0x5438, 0x5439); mem_wr(gb, DE, A);
  CYC(0x5439, 0x543a); alu_scf(gb);
  RET(0x543a); return; // ret
}

// 0e:543b, bare global; called from fireKeese_subid0_stateB.
void fireKeese_moveTowardCenterIfOutOfBounds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x543b, 0x543d); E = ENEMY_BASE + OBJ_YH;
  CYC(0x543d, 0x543e); A = mem_rd(gb, DE);
  CYC(0x543e, 0x5440); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(0x5440, 0x5442); goto outOfBounds; } // jr nc
  CYC(0x5440, 0x5442);
  CYC(0x5442, 0x5444); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5444, 0x5445); A = mem_rd(gb, DE);
  CYC(0x5445, 0x5447); alu_cp(gb, 0xf0);
  if (F & FC) { RET_TAKEN(0x5447); return; } // ret c
  CYC(0x5447, 0x5448);

outOfBounds:
  CYC(0x5448, 0x544a); E = ENEMY_BASE + OBJ_YH;
  CYC(0x544a, 0x544b); A = mem_rd(gb, DE);
  CYC(0x544b, 0x544d); hram_wr(gb, 0x8f, A); // hFF8F
  CYC(0x544d, 0x544f); E = ENEMY_BASE + OBJ_XH;
  CYC(0x544f, 0x5450); A = mem_rd(gb, DE);
  CYC(0x5450, 0x5452); hram_wr(gb, 0x8e, A); // hFF8E

  CYC(0x5452, 0x5455); SET_BC(0x5878); // ((LARGE_ROOM_HEIGHT/2)<<4)+8, ((LARGE_ROOM_WIDTH/2)<<4)+8
  CALL_C(0x5455, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x5458);
  CYC(0x5458, 0x5459); C = A;
  CYC(0x5459, 0x545b); B = 0x28; // SPEED_100
  CYC(0x545b, 0x545d); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x545d, 0x5460); objectApplyGivenSpeed_hook(gb); return; // jp
}
