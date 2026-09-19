#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode2d_jump_table(GB *gb) {
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

static void enemyCode2d_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// enemyCode2d@decCounter2Every4Frames: an @-local reached only by one genuine `call`, from
// @stateA below. Tail-jumps into the already-hooked ecom_decCounter2, so no pc/sp resume check
// is needed (same trampoline shape as interactionCode8a_checkEssenceObtained).
static void enemyCode2d_decCounter2Every4Frames(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x6671, 0x6674); A = W8(wFrameCounter);
  CYC(0x6674, 0x6676); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6676, 0x6677); ret_effect(gb); return; } // ret nz
  CYC(0x6676, 0x6677);
  CYC(0x6677, 0x667a); ecom_decCounter2_b0d_hook(gb); // jp
}

// enemyCode2d@updateCollisionRadiiAndYPosition: an @-local reached both by a genuine `call`
// (from @state9, which pushes its own return address before invoking this) and by a genuine
// tail `jr z` (from @stateB, which never pushes and relies on this routine's own `ret` to pop
// whatever @stateB's real caller left on the stack). This body always ends with the real `ret`
// and never diverges elsewhere, so neither call site needs a pc/sp resume check.
static void enemyCode2d_updateCollisionRadiiAndYPosition(GB *gb) {
  CYC(0x6649, 0x664b); alu_sub(gb, 0x03);
  CYC(0x664b, 0x664e); SET_HL(0x6662); // @data
  CYC(0x664e, 0x664f); enemyCode2d_addAToHl_from_rst(gb, 0x664f);
  CYC(0x664f, 0x6651); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6651, 0x6652); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6652, 0x6653); mem_wr(gb, DE, A);
  CYC(0x6653, 0x6654); E = alu_inc8(gb, E);
  CYC(0x6654, 0x6655); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6655, 0x6656); mem_wr(gb, DE, A);
  CYC(0x6656, 0x6658); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6658, 0x6659); A = mem_rd(gb, DE);
  CYC(0x6659, 0x665a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x665a, 0x665c); E = ENEMY_BASE + OBJ_YH;
  CYC(0x665c, 0x665d); mem_wr(gb, DE, A);
  CYC(0x665d, 0x665f); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x665f, 0x6660); alu_xor(gb, A);
  CYC(0x6660, 0x6661); mem_wr(gb, DE, A);
  CYC(0x6661, 0x6662); ret_effect(gb);
}

// enemyCode2d@data (0d:6662-6670): pure data (5 rows of {collisionRadiusY, collisionRadiusX,
// y-position offset}), read only through the addAToHl indexing above; never given a hook.

// ==================================================================================================
// ENEMY_PODOBOO_TOWER
//
// Variables:
//   var30: Base y-position. (Actual y-position changes as it emerges from the ground.)
//
// All of this enemy's states are @-local to enemyCode2d in the disassembly (no separate
// top-level symbols), so the entire state machine lives in this one hook.
// ==================================================================================================
void enemyCode2d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6585, 0x6587); goto normalStatus; } // jr z
  CYC(0x6585, 0x6587);
  CYC(0x6587, 0x6589); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x6589, 0x658a); ret_effect(gb); return; } // ret c
  CYC(0x6589, 0x658a);
  if (F & FZ) { CYCT(0x658a, 0x658d); enemyDie_withoutItemDrop_hook(gb); return; } // jp z
  CYC(0x658a, 0x658d);
  // ENEMYSTATUS_JUST_HIT or ENEMYSTATUS_KNOCKBACK
  CYC(0x658d, 0x658f); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x658f, 0x6590); A = mem_rd(gb, DE);
  CYC(0x6590, 0x6592); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (F & FZ) { CYCT(0x6592, 0x6595); enemyDie_uncounted_withoutItemDrop_hook(gb); return; } // jp z
  CYC(0x6592, 0x6595);

normalStatus:
  CYC(0x6595, 0x6597); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6597, 0x6598); A = mem_rd(gb, DE);
  CYC(0x6598, 0x6599); push_effect(gb, 0x6599);
  {
    uint16_t target = enemyCode2d_jump_table(gb);
    if (target == 0x65c8) goto state8;
    if (target == 0x65d7) goto state9;
    if (target == 0x65f2) goto stateA;
    if (target == 0x6614) goto stateB;
    if (target == 0x662c) goto stateC;
    if (target == 0x663c) goto stateD;
    if (target == 0x65b5) goto state_uninitialized;
    goto state_stub; // states 1-7 all target 0x65c7
  }

state_uninitialized:
  CALL_C(0x65b5, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x65b8);
  CYC(0x65b8, 0x65ba); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x65ba, 0x65bc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CYC(0x65bc, 0x65be); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x65be, 0x65c0); mem_wr(gb, HL, 60);
  CYC(0x65c0, 0x65c2); L = ENEMY_BASE + OBJ_YH;
  CYC(0x65c2, 0x65c3); A = mem_rd(gb, HL);
  CYC(0x65c3, 0x65c5); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x65c5, 0x65c6); mem_wr(gb, HL, A);
  RET(0x65c6); return;

state_stub:
  RET(0x65c7); return;

  // Head is in the ground, flickering, for 60 frames
state8:
  CALL_C(0x65c8, ecom_decCounter1_b0d_hook, 0x439a, 0x65cb);
  if (!(F & FZ)) { CYCT(0x65cb, 0x65ce); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(0x65cb, 0x65ce);
  CYC(0x65ce, 0x65cf); L = E;
  CYC(0x65cf, 0x65d0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x65d0, 0x65d2); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x65d2, 0x65d4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x65d4, 0x65d7); objectSetVisible82_hook(gb); return; // jp

  // Rising up out of the ground
state9:
  CALL_C(0x65d7, enemyAnimate_hook, 0x2818, 0x65da);
  CYC(0x65da, 0x65dc); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x65dc, 0x65dd); A = mem_rd(gb, DE);
  CYC(0x65dd, 0x65de); alu_or(gb, A);
  if (F & FZ) { CYCT(0x65de, 0x65df); ret_effect(gb); return; } // ret z
  CYC(0x65de, 0x65df);
  CYC(0x65df, 0x65e0); B = A;
  CYC(0x65e0, 0x65e3); push_effect(gb, 0x65e3); enemyCode2d_updateCollisionRadiiAndYPosition(gb);
  CYC(0x65e3, 0x65e4); A = B;
  CYC(0x65e4, 0x65e6); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x65e6, 0x65e7); ret_effect(gb); return; } // ret nz
  CYC(0x65e6, 0x65e7);
  // Fully emerged
  CYC(0x65e7, 0x65e8); H = D;
  CYC(0x65e8, 0x65ea); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x65ea, 0x65eb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x65eb, 0x65ed); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x65ed, 0x65ef); mem_wr(gb, HL, 150);
  CYC(0x65ef, 0x65f0); L = alu_inc8(gb, L);
  CYC(0x65f0, 0x65f2); mem_wr(gb, HL, 180); // [counter2]
  // falls through to stateA

  // Fully emerged from ground, firing at Link until counter2 reaches 0
stateA:
  CYC(0x65f2, 0x65f5); enemyCode2d_decCounter2Every4Frames(gb, 0x65f5);
  if (!(F & FZ)) { CYCT(0x65f5, 0x65f7); goto stateAFireCheck; } // jr nz
  CYC(0x65f5, 0x65f7);
  CYC(0x65f7, 0x65f8); L = E;
  CYC(0x65f8, 0x65f9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x65f9, 0x65fb); A = 0x01;
  CYC(0x65fb, 0x65fe); enemySetAnimation_hook(gb); return; // jp

stateAFireCheck:
  // Randomly fire projectile when [counter1] reaches 0
  CALL_C(0x65fe, ecom_decCounter1_b0d_hook, 0x439a, 0x6601);
  if (!(F & FZ)) { CYCT(0x6601, 0x6603); goto animate; } // jr nz
  CYC(0x6601, 0x6603);
  CYC(0x6603, 0x6605); mem_wr(gb, HL, 150); // $96
  CALL_C(0x6605, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6608);
  CYC(0x6608, 0x660a); alu_cp(gb, 0xb4);
  if (!(F & FC)) { CYCT(0x660a, 0x660c); goto animate; } // jr nc
  CYC(0x660a, 0x660c);
  CYC(0x660c, 0x660e); B = 0x31; // PART_GOPONGA_PROJECTILE
  CALL_C(0x660e, ecom_spawnProjectile_b0d_hook, 0x437c, 0x6611);
  // falls through to animate

animate:
  CYC(0x6611, 0x6614); enemyAnimate_hook(gb); return; // jp

  // Moving back into the ground
stateB:
  CALL_C(0x6614, enemyAnimate_hook, 0x2818, 0x6617);
  CYC(0x6617, 0x6619); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6619, 0x661a); A = mem_rd(gb, DE);
  CYC(0x661a, 0x661b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x661b, 0x661c); ret_effect(gb); return; } // ret z
  CYC(0x661b, 0x661c);
  CYC(0x661c, 0x661e); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x661e, 0x6620); enemyCode2d_updateCollisionRadiiAndYPosition(gb); return; } // jr z
  CYC(0x661e, 0x6620);
  // Head reached the ground
  CALL_C(0x6620, ecom_incState_b0d_hook, 0x4000, 0x6623);
  CYC(0x6623, 0x6625); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6625, 0x6627); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x6627, 0x6629); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6629, 0x662b); mem_wr(gb, HL, 60);
  RET(0x662b); return;

  // Head is in the ground, flickering, for 60 frames.
stateC:
  CALL_C(0x662c, ecom_decCounter1_b0d_hook, 0x439a, 0x662f);
  if (!(F & FZ)) { CYCT(0x662f, 0x6632); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(0x662f, 0x6632);
  CYC(0x6632, 0x6634); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6634, 0x6635); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6635, 0x6637); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6637, 0x6639); mem_wr(gb, HL, 180);
  CYC(0x6639, 0x663c); objectSetInvisible_hook(gb); return; // jp

  // Waiting underground for [counter1] frames.
stateD:
  CALL_C(0x663c, ecom_decCounter1_b0d_hook, 0x439a, 0x663f);
  if (!(F & FZ)) { CYCT(0x663f, 0x6640); ret_effect(gb); return; } // ret nz
  CYC(0x663f, 0x6640);
  CYC(0x6640, 0x6642); mem_wr(gb, HL, 60);
  CYC(0x6642, 0x6643); L = E;
  CYC(0x6643, 0x6645); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x6645, 0x6646); alu_xor(gb, A);
  CYC(0x6646, 0x6649); enemySetAnimation_hook(gb); return; // jp
}
