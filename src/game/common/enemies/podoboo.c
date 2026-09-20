#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode29_jump_table(GB *gb) {
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

static void podoboo_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void podoboo_state_uninitialized_hook(GB *gb);
void podoboo_state_stub_hook(GB *gb);
void podoboo_state8_hook(GB *gb);
void podoboo_state9_hook(GB *gb);
void podoboo_stateA_hook(GB *gb);
void podoboo_stateB_hook(GB *gb);
void podoboo_stateC_hook(GB *gb);
void podoboo_spawnLavaParticleEvery16Frames_hook(GB *gb);
void podoboo_spawnLavaParticle_hook(GB *gb);
void podoboo_beginMovingUp_hook(GB *gb);
void podoboo_makeLavaSplash_hook(GB *gb);
void podoboo_updatePosition_hook(GB *gb);

// ==================================================================================================
// ENEMY_PODOBOO
//
// Variables:
//   relatedObj1: "Parent" (for subid 1, the lava particle)
//   var30: Animation index
//   var31: Initial Y position; the point at which the podoboo returns back to the lava
// ==================================================================================================
void enemyCode29_hook(GB *gb) {
  BASE(enemyCode29);
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+1, b_+2); ret_effect(gb); return; } // ret z
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret z
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  {
    uint16_t target = enemyCode29_jump_table(gb);
    if (target == SYM(podoboo_state_uninitialized)) { podoboo_state_uninitialized_hook(gb); return; }
    if (target == SYM(podoboo_state8)) { podoboo_state8_hook(gb); return; }
    if (target == SYM(podoboo_state9)) { podoboo_state9_hook(gb); return; }
    if (target == SYM(podoboo_stateA)) { podoboo_stateA_hook(gb); return; }
    if (target == SYM(podoboo_stateB)) { podoboo_stateB_hook(gb); return; }
    if (target == SYM(podoboo_stateC)) { podoboo_stateC_hook(gb); return; }
    podoboo_state_stub_hook(gb); return; // states 1-7 all target 0x62be
  }
}

void podoboo_state_uninitialized_hook(GB *gb) {
  BASE(podoboo_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+8); ret_effect(gb); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x0c); // [state] = $0c
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+12, b_+13); H = mem_rd(gb, HL);
  CYC(b_+13, b_+15); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CALL_C(b_+17, enemySetAnimation_hook, SYM(enemySetAnimation), b_+20);
  CYC(b_+20, b_+23); objectSetVisible83_hook(gb); return; // jp
}

void podoboo_state_stub_hook(GB *gb) {
  BASE(podoboo_state_stub);
  RET(b_+0); return;
}

// Subid 0: Waiting for Link to approach horizontally
void podoboo_state8_hook(GB *gb) {
  BASE(podoboo_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+3, b_+5); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+8); alu_add(gb, 0x30);
  CYC(b_+8, b_+10); alu_cp(gb, 0x61);
  if (!(F & FC)) { CYCT(b_+10, b_+11); ret_effect(gb); return; } // ret nc
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); podoboo_beginMovingUp_hook(gb); return; // jr
}

// Leaping out of lava
void podoboo_state9_hook(GB *gb) {
  BASE(podoboo_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, podoboo_updatePosition_hook, SYM(podoboo_updatePosition), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto doneLeaping; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); // hl == Enemy.speedZ+1
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); podoboo_spawnLavaParticleEvery16Frames_hook(gb); return; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+15, b_+16); ret_effect(gb); return; } // ret z
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CALL_C(b_+17, enemySetAnimation_hook, SYM(enemySetAnimation), b_+20);
  CYC(b_+20, b_+22); podoboo_spawnLavaParticle_hook(gb); return; // jr

doneLeaping:
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  podoboo_stateA_hook(gb); return; // falls through
}

// Just re-entered the lava
void podoboo_stateA_hook(GB *gb) {
  BASE(podoboo_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, podoboo_makeLavaSplash_hook, SYM(podoboo_makeLavaSplash), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  CYC(b_+9, b_+12); SET_HL(SYM(podoboo_counter1Vals)); // podoboo_counter1Vals
  CYC(b_+12, b_+13); podoboo_addAToHl_from_rst(gb, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+20);
  CYC(b_+20, b_+23); objectSetInvisible_hook(gb); return; // jp
}

// Waiting for [counter1] frames before jumping out again.
void podoboo_stateB_hook(GB *gb) {
  BASE(podoboo_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+5, b_+7); podoboo_beginMovingUp_hook(gb); return; // jr
}

// State for "lava particle" (subid 1); just animate until time to delete self.
void podoboo_stateC_hook(GB *gb) {
  BASE(podoboo_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+10); enemyDelete_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); objectSetInvisible_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); ecom_flickerVisibility_b0d_hook(gb); return; // jp
}

void podoboo_spawnLavaParticleEvery16Frames_hook(GB *gb) {
  BASE(podoboo_spawnLavaParticleEvery16Frames);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; } // ret nz
  CYC(b_+6, b_+7);
  podoboo_spawnLavaParticle_hook(gb); return; // falls through
}

void podoboo_spawnLavaParticle_hook(GB *gb) {
  BASE(podoboo_spawnLavaParticle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x29; // ENEMY_PODOBOO
  CALL_C(b_+2, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, objectCopyPosition_hook, SYM(objectCopyPosition), b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+11, b_+13); A = ENEMY_BASE; // Enemy.start
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, D);
  RET(b_+15); return;
}

// Makes a splash, sets animation and speed, enables collisions for when the splash has
// just spawned, sets state to 9.
void podoboo_beginMovingUp_hook(GB *gb) {
  BASE(podoboo_beginMovingUp);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, podoboo_makeLavaSplash_hook, SYM(podoboo_makeLavaSplash), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, objectSetVisible82_hook, SYM(objectSetVisible82), b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, enemySetAnimation_hook, SYM(enemySetAnimation), b_+15);
  CYC(b_+15, b_+18); SET_BC(0xfbc0); // -$440
  CALL_C(b_+18, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+21);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x09);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+29, b_+30); alu_xor(gb, A);
  RET(b_+30); return;
}

// @param[out] zflag  z if created successfully
void podoboo_makeLavaSplash_hook(GB *gb) {
  BASE(podoboo_makeLavaSplash);
  CYC(b_+0, b_+3); SET_BC(0x0401); // INTERAC_LAVASPLASH, $01
  CYC(b_+3, b_+6); objectCreateInteraction_hook(gb); return; // jp
}

// podoboo_counter1Vals (0d:635a) is pure data (.db $10 $50 $50 $50), not code -- confirmed via
// --report, which decodes it as `stop` followed by garbage. Referenced only as a data pointer
// from podoboo_stateA above via podoboo_addAToHl_from_rst; never given its own hook.

// @param[out] zflag  z if returned to original position.
void podoboo_updatePosition_hook(GB *gb) {
  BASE(podoboo_updatePosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_Y;
  CALL_C(b_+5, add16BitRefs_hook, SYM(add16BitRefs), b_+8);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+13, b_+15); goto reachedOriginalPosition; } // jr c
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); L = alu_dec8(gb, L);
  CYC(b_+16, b_+18); A = 0x1c;
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+22); A = 0x00;
  CYC(b_+22, b_+23); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+25); alu_or(gb, D);
  RET(b_+25); return;

reachedOriginalPosition:
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x00);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  RET(b_+32); return;
}
