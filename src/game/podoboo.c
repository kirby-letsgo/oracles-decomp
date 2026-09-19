#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  CYC(0x6285, 0x6286); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6286, 0x6287); ret_effect(gb); return; } // ret z
  CYC(0x6286, 0x6287);
  CYC(0x6287, 0x6288); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6288, 0x6289); ret_effect(gb); return; } // ret z
  CYC(0x6288, 0x6289);
  CYC(0x6289, 0x628b); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x628b, 0x628c); A = mem_rd(gb, DE);
  CYC(0x628c, 0x628d); push_effect(gb, 0x628d);
  {
    uint16_t target = enemyCode29_jump_table(gb);
    if (target == 0x62a7) { podoboo_state_uninitialized_hook(gb); return; }
    if (target == 0x62bf) { podoboo_state8_hook(gb); return; }
    if (target == 0x62d2) { podoboo_state9_hook(gb); return; }
    if (target == 0x62ef) { podoboo_stateA_hook(gb); return; }
    if (target == 0x6306) { podoboo_stateB_hook(gb); return; }
    if (target == 0x630d) { podoboo_stateC_hook(gb); return; }
    podoboo_state_stub_hook(gb); return; // states 1-7 all target 0x62be
  }
}

void podoboo_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62a7, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x62aa);
  CYC(0x62aa, 0x62ac); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x62ac, 0x62ad); A = mem_rd(gb, DE);
  CYC(0x62ad, 0x62ae); alu_or(gb, A);
  if (F & FZ) { CYCT(0x62ae, 0x62af); ret_effect(gb); return; } // ret z
  CYC(0x62ae, 0x62af);
  CYC(0x62af, 0x62b1); mem_wr(gb, HL, 0x0c); // [state] = $0c
  CYC(0x62b1, 0x62b3); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x62b3, 0x62b4); H = mem_rd(gb, HL);
  CYC(0x62b4, 0x62b6); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x62b6, 0x62b7); A = mem_rd(gb, HL);
  CYC(0x62b7, 0x62b8); A = alu_inc8(gb, A);
  CALL_C(0x62b8, enemySetAnimation_hook, 0x282b, 0x62bb);
  CYC(0x62bb, 0x62be); objectSetVisible83_hook(gb); return; // jp
}

void podoboo_state_stub_hook(GB *gb) {
  RET(0x62be); return;
}

// Subid 0: Waiting for Link to approach horizontally
void podoboo_state8_hook(GB *gb) {
  CYC(0x62bf, 0x62c0); H = D;
  CYC(0x62c0, 0x62c2); L = ENEMY_BASE + OBJ_XH;
  CYC(0x62c2, 0x62c4); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x62c4, 0x62c5); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x62c5, 0x62c7); alu_add(gb, 0x30);
  CYC(0x62c7, 0x62c9); alu_cp(gb, 0x61);
  if (!(F & FC)) { CYCT(0x62c9, 0x62ca); ret_effect(gb); return; } // ret nc
  CYC(0x62c9, 0x62ca);
  CYC(0x62ca, 0x62cc); L = ENEMY_BASE + OBJ_YH;
  CYC(0x62cc, 0x62cd); A = mem_rd(gb, HL);
  CYC(0x62cd, 0x62cf); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x62cf, 0x62d0); mem_wr(gb, HL, A);
  CYC(0x62d0, 0x62d2); podoboo_beginMovingUp_hook(gb); return; // jr
}

// Leaping out of lava
void podoboo_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62d2, enemyAnimate_hook, 0x2818, 0x62d5);
  CALL_C(0x62d5, podoboo_updatePosition_hook, 0x635e, 0x62d8);
  if (F & FZ) { CYCT(0x62d8, 0x62da); goto doneLeaping; } // jr z
  CYC(0x62d8, 0x62da);
  CYC(0x62da, 0x62db); A = mem_rd(gb, HL); // hl == Enemy.speedZ+1
  CYC(0x62db, 0x62dc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x62dc, 0x62de); podoboo_spawnLavaParticleEvery16Frames_hook(gb); return; } // jr nz
  CYC(0x62dc, 0x62de);
  CYC(0x62de, 0x62e0); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x62e0, 0x62e1); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x62e1, 0x62e2); ret_effect(gb); return; } // ret z
  CYC(0x62e1, 0x62e2);
  CYC(0x62e2, 0x62e3); mem_wr(gb, HL, A);
  CALL_C(0x62e3, enemySetAnimation_hook, 0x282b, 0x62e6);
  CYC(0x62e6, 0x62e8); podoboo_spawnLavaParticle_hook(gb); return; // jr

doneLeaping:
  CYC(0x62e8, 0x62ea); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x62ea, 0x62eb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x62eb, 0x62ed); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x62ed, 0x62ef); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  podoboo_stateA_hook(gb); return; // falls through
}

// Just re-entered the lava
void podoboo_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62ef, podoboo_makeLavaSplash_hook, 0x6354, 0x62f2);
  if (!(F & FZ)) { CYCT(0x62f2, 0x62f3); ret_effect(gb); return; } // ret nz
  CYC(0x62f2, 0x62f3);
  CALL_C(0x62f3, getRandomNumber_noPreserveVars_hook, 0x0453, 0x62f6);
  CYC(0x62f6, 0x62f8); alu_and(gb, 0x03);
  CYC(0x62f8, 0x62fb); SET_HL(0x635a); // podoboo_counter1Vals
  CYC(0x62fb, 0x62fc); podoboo_addAToHl_from_rst(gb, 0x62fc);
  CYC(0x62fc, 0x62fe); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x62fe, 0x62ff); A = mem_rd(gb, HL);
  CYC(0x62ff, 0x6300); mem_wr(gb, DE, A);
  CALL_C(0x6300, ecom_incState_b0d_hook, 0x4000, 0x6303);
  CYC(0x6303, 0x6306); objectSetInvisible_hook(gb); return; // jp
}

// Waiting for [counter1] frames before jumping out again.
void podoboo_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6306, ecom_decCounter1_b0d_hook, 0x439a, 0x6309);
  if (!(F & FZ)) { CYCT(0x6309, 0x630a); ret_effect(gb); return; } // ret nz
  CYC(0x6309, 0x630a);
  CYC(0x630a, 0x630b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x630b, 0x630d); podoboo_beginMovingUp_hook(gb); return; // jr
}

// State for "lava particle" (subid 1); just animate until time to delete self.
void podoboo_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x630d, enemyAnimate_hook, 0x2818, 0x6310);
  CYC(0x6310, 0x6312); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6312, 0x6313); A = mem_rd(gb, DE);
  CYC(0x6313, 0x6314); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x6314, 0x6317); enemyDelete_hook(gb); return; } // jp z
  CYC(0x6314, 0x6317);
  CYC(0x6317, 0x6318); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6318, 0x631b); objectSetInvisible_hook(gb); return; } // jp nz
  CYC(0x6318, 0x631b);
  CYC(0x631b, 0x631e); ecom_flickerVisibility_b0d_hook(gb); return; // jp
}

void podoboo_spawnLavaParticleEvery16Frames_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x631e, ecom_decCounter1_b0d_hook, 0x439a, 0x6321);
  CYC(0x6321, 0x6322); A = mem_rd(gb, HL);
  CYC(0x6322, 0x6324); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x6324, 0x6325); ret_effect(gb); return; } // ret nz
  CYC(0x6324, 0x6325);
  podoboo_spawnLavaParticle_hook(gb); return; // falls through
}

void podoboo_spawnLavaParticle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6325, 0x6327); B = 0x29; // ENEMY_PODOBOO
  CALL_C(0x6327, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, 0x436d, 0x632a);
  if (!(F & FZ)) { CYCT(0x632a, 0x632b); ret_effect(gb); return; } // ret nz
  CYC(0x632a, 0x632b);
  CALL_C(0x632b, objectCopyPosition_hook, 0x2242, 0x632e);
  CYC(0x632e, 0x6330); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x6330, 0x6332); A = ENEMY_BASE; // Enemy.start
  CYC(0x6332, 0x6333); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6333, 0x6334); mem_wr(gb, HL, D);
  RET(0x6334); return;
}

// Makes a splash, sets animation and speed, enables collisions for when the splash has
// just spawned, sets state to 9.
void podoboo_beginMovingUp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6335, podoboo_makeLavaSplash_hook, 0x6354, 0x6338);
  if (!(F & FZ)) { CYCT(0x6338, 0x6339); ret_effect(gb); return; } // ret nz
  CYC(0x6338, 0x6339);
  CALL_C(0x6339, objectSetVisible82_hook, 0x1e69, 0x633c);
  CYC(0x633c, 0x633e); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x633e, 0x6340); A = 0x02;
  CYC(0x6340, 0x6341); mem_wr(gb, DE, A);
  CALL_C(0x6341, enemySetAnimation_hook, 0x282b, 0x6344);
  CYC(0x6344, 0x6347); SET_BC(0xfbc0); // -$440
  CALL_C(0x6347, objectSetSpeedZ_hook, 0x239d, 0x634a);
  CYC(0x634a, 0x634c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x634c, 0x634e); mem_wr(gb, HL, 0x09);
  CYC(0x634e, 0x6350); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6350, 0x6352); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6352, 0x6353); alu_xor(gb, A);
  RET(0x6353); return;
}

// @param[out] zflag  z if created successfully
void podoboo_makeLavaSplash_hook(GB *gb) {
  CYC(0x6354, 0x6357); SET_BC(0x0401); // INTERAC_LAVASPLASH, $01
  CYC(0x6357, 0x635a); objectCreateInteraction_hook(gb); return; // jp
}

// podoboo_counter1Vals (0d:635a) is pure data (.db $10 $50 $50 $50), not code -- confirmed via
// --report, which decodes it as `stop` followed by garbage. Referenced only as a data pointer
// from podoboo_stateA above via podoboo_addAToHl_from_rst; never given its own hook.

// @param[out] zflag  z if returned to original position.
void podoboo_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x635e, 0x635f); H = D;
  CYC(0x635f, 0x6361); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6361, 0x6363); E = ENEMY_BASE + OBJ_Y;
  CALL_C(0x6363, add16BitRefs_hook, 0x23a7, 0x6366);
  CYC(0x6366, 0x6367); B = A;
  CYC(0x6367, 0x6369); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6369, 0x636a); A = mem_rd(gb, DE);
  CYC(0x636a, 0x636b); alu_cp(gb, B);
  if (F & FC) { CYCT(0x636b, 0x636d); goto reachedOriginalPosition; } // jr c
  CYC(0x636b, 0x636d);
  CYC(0x636d, 0x636e); L = alu_dec8(gb, L);
  CYC(0x636e, 0x6370); A = 0x1c;
  CYC(0x6370, 0x6371); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6371, 0x6372); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6372, 0x6374); A = 0x00;
  CYC(0x6374, 0x6375); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x6375, 0x6376); mem_wr(gb, HL, A);
  CYC(0x6376, 0x6377); alu_or(gb, D);
  RET(0x6377); return;

reachedOriginalPosition:
  CYC(0x6378, 0x637a); L = ENEMY_BASE + OBJ_YH;
  CYC(0x637a, 0x637b); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x637b, 0x637d); mem_wr(gb, HL, 0x00);
  CYC(0x637d, 0x637e); alu_xor(gb, A);
  RET(0x637e); return;
}
