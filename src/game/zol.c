#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void zol_state_uninitialized_hook(GB *gb);
void zol_state_stub_hook(GB *gb);
void zol_subid00_hook(GB *gb);
void zol_subid00_state8_hook(GB *gb);
void zol_subid00_state9_hook(GB *gb);
void zol_subid00_stateA_hook(GB *gb);
void zol_animate_hook(GB *gb);
void zol_subid00_stateB_hook(GB *gb);
void zol_subid00_stateC_hook(GB *gb);
void zol_subid00_stateD_hook(GB *gb);
void zol_subid01_hook(GB *gb);
void zol_subid01_state8_hook(GB *gb);
void zol_subid01_state9_hook(GB *gb);
void zol_animate2_hook(GB *gb);
void zol_subid01_stateA_hook(GB *gb);
void zol_subid01_stateB_hook(GB *gb);
void zol_subid01_stateC_hook(GB *gb);
void zol_subid01_stateD_hook(GB *gb);
void zol_spawnGel_hook(GB *gb);

static uint16_t zol_jump_table(GB *gb) {
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
// ENEMY_ZOL
//
// Variables:
//   var30: 1 when the zol is out of the ground, 0 otherwise. (only for subid 0, and only
//          used to prevent the "jump" sound effect from playing more than once.)
// ==================================================================================================
void enemyCode34_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x492d, ecom_checkHazardsNoAnimationForHoles_b0e_hook, 0x4043, 0x4930);
  if (F & FZ) { CYCT(0x4930, 0x4932); goto normalStatus; } // jr z
  CYC(0x4930, 0x4932);
  CYC(0x4932, 0x4934); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4934); return; } // ret c
  CYC(0x4934, 0x4935);
  if (F & FZ) { CYCT(0x4935, 0x4938); enemyDie_hook(gb); return; } // jp z
  CYC(0x4935, 0x4938);
  CYC(0x4938, 0x4939); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x4939, 0x493c); ecom_updateKnockbackAndCheckHazardsNoAnimationsForHoles_b0e_hook(gb); return; } // jp nz
  CYC(0x4939, 0x493c);
  CYC(0x493c, 0x493e); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x493e, 0x493f); A = mem_rd(gb, DE);
  CYC(0x493f, 0x4940); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x4940); return; } // ret z
  CYC(0x4940, 0x4941);
  CYC(0x4941, 0x4943); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x4943, 0x4944); A = mem_rd(gb, DE);
  CYC(0x4944, 0x4946); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { CYCT(0x4946, 0x4948); goto normalStatus; } // jr z
  CYC(0x4946, 0x4948);
  CYC(0x4948, 0x494a); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x494a, 0x494c); alu_sub(gb, 0x01); // ITEMCOLLISION_L1_SHIELD
  CYC(0x494c, 0x494e); alu_cp(gb, 0x03); // ITEMCOLLISION_L3_SHIELD - ITEMCOLLISION_L1_SHIELD + 1
  if (F & FC) { RET_TAKEN(0x494e); return; } // ret c
  CYC(0x494e, 0x494f);
  CYC(0x494f, 0x4951); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4951, 0x4953); A = 0x0c;
  CYC(0x4953, 0x4954); mem_wr(gb, DE, A);
  RET(0x4954); return; // ret

normalStatus:
  CALL_C(0x4955, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x4958);
  if (!(F & FC)) { CYCT(0x4958, 0x495a); goto normalState; } // jr nc
  CYC(0x4958, 0x495a);
  {
    CYC(0x495a, 0x495b); push_effect(gb, 0x495b);
    uint16_t target = zol_jump_table(gb);
    if (target == 0x4971) { zol_state_uninitialized_hook(gb); return; }
    if (target == 0x4989) { zol_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x496b, 0x496c); A = B;
  {
    CYC(0x496c, 0x496d); push_effect(gb, 0x496d);
    uint16_t target = zol_jump_table(gb);
    if (target == 0x498a) { zol_subid00_hook(gb); return; }
    if (target == 0x4a3b) { zol_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

void zol_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4971, 0x4972); A = B;
  CYC(0x4972, 0x4973); alu_or(gb, A);
  CYC(0x4973, 0x4975); A = 0x1e; // SPEED_c0
  if (F & FZ) { CYCT(0x4975, 0x4978); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp z
  CYC(0x4975, 0x4978);
  CYC(0x4978, 0x4979); H = D;
  CYC(0x4979, 0x497b); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x497b, 0x497d); mem_wr(gb, HL, 0x18);
  CYC(0x497d, 0x497f); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x497f, 0x4981); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4981, 0x4983); A = 0x04;
  CALL_C(0x4983, enemySetAnimation_hook, 0x282b, 0x4986);
  CYC(0x4986, 0x4989); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

void zol_state_stub_hook(GB *gb) {
  RET(0x4989); return; // ret
}

void zol_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x498a, 0x498b); A = mem_rd(gb, DE);
  CYC(0x498b, 0x498d); alu_sub(gb, 0x08);
  {
    CYC(0x498d, 0x498e); push_effect(gb, 0x498e);
    uint16_t target = zol_jump_table(gb);
    if (target == 0x499a) { zol_subid00_state8_hook(gb); return; }
    if (target == 0x49b0) { zol_subid00_state9_hook(gb); return; }
    if (target == 0x49d8) { zol_subid00_stateA_hook(gb); return; }
    if (target == 0x49f4) { zol_subid00_stateB_hook(gb); return; }
    if (target == 0x4a19) { zol_subid00_stateC_hook(gb); return; }
    if (target == 0x4a30) { zol_subid00_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Hiding in ground, waiting for Link to approach
void zol_subid00_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x499a, 0x499c); C = 0x28;
  CALL_C(0x499c, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x499f);
  if (!(F & FC)) { RET_TAKEN(0x499f); return; } // ret nc
  CYC(0x499f, 0x49a0);
  CYC(0x49a0, 0x49a3); SET_BC(0xfe00);
  CALL_C(0x49a3, objectSetSpeedZ_hook, 0x239d, 0x49a6);
  CYC(0x49a6, 0x49a8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x49a8, 0x49a9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x49a9, 0x49ab); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x49ab, 0x49ad); mem_wr(gb, HL, 0x04);
  CYC(0x49ad, 0x49b0); objectSetVisiblec2_hook(gb); return; // jp
}

// Jumping out of ground
void zol_subid00_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49b0, 0x49b1); H = D;
  CYC(0x49b1, 0x49b3); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x49b3, 0x49b4); A = mem_rd(gb, HL);
  CYC(0x49b4, 0x49b5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x49b5, 0x49b7); zol_animate_hook(gb); return; } // jr z
  CYC(0x49b5, 0x49b7);
  CYC(0x49b7, 0x49b9); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x49b9, 0x49ba); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x49ba, 0x49bc); goto L_49c3; } // jr nz
  CYC(0x49ba, 0x49bc);
  CYC(0x49bc, 0x49be); mem_wr(gb, HL, 0x01);
  CYC(0x49be, 0x49c0); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x49c0, playSound_b00_hook, 0x0c98, 0x49c3);

L_49c3:
  CYC(0x49c3, 0x49c5); C = 0x28;
  CALL_C(0x49c5, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x49c8);
  if (!(F & FZ)) { RET_TAKEN(0x49c8); return; } // ret nz
  CYC(0x49c8, 0x49c9);
  CALL_C(0x49c9, ecom_incState_b0e_hook, 0x4000, 0x49cc);
  CYC(0x49cc, 0x49ce); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x49ce, 0x49d0); mem_wr(gb, HL, 0x30);
  CYC(0x49d0, 0x49d2); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x49d2, 0x49d4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x49d4, 0x49d5); A = alu_inc8(gb, A);
  CYC(0x49d5, 0x49d8); enemySetAnimation_hook(gb); return; // jp
}

// Holding still for [counter1] frames, preparing to hop toward Link
void zol_subid00_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49d8, ecom_decCounter1_b0e_hook, 0x439a, 0x49db);
  if (!(F & FZ)) { RET_TAKEN(0x49db); return; } // ret nz
  CYC(0x49db, 0x49dc);
  CYC(0x49dc, 0x49dd); L = E;
  CYC(0x49dd, 0x49de); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x49de, 0x49e1); SET_BC(0xfe00);
  CALL_C(0x49e1, objectSetSpeedZ_hook, 0x239d, 0x49e4);
  CALL_C(0x49e4, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x49e7);
  CYC(0x49e7, 0x49e9); A = 0x02;
  CALL_C(0x49e9, enemySetAnimation_hook, 0x282b, 0x49ec);
  CYC(0x49ec, 0x49ee); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x49ee, playSound_b00_hook, 0x0c98, 0x49f1);
  zol_animate_hook(gb); return; // fallthrough
}

void zol_animate_hook(GB *gb) {
  CYC(0x49f1, 0x49f4); enemyAnimate_hook(gb); return; // jp
}

// Hopping toward Link
void zol_subid00_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49f4, ecom_applyVelocityForSideviewEnemy_b0e_hook, 0x4153, 0x49f7);
  CYC(0x49f7, 0x49f9); C = 0x28;
  CALL_C(0x49f9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x49fc);
  if (!(F & FZ)) { RET_TAKEN(0x49fc); return; } // ret nz
  CYC(0x49fc, 0x49fd);
  CYC(0x49fd, 0x49fe); H = D;
  CYC(0x49fe, 0x4a00); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4a00, 0x4a02); mem_wr(gb, HL, 0x30);
  CYC(0x4a02, 0x4a03); L = alu_inc8(gb, L);
  CYC(0x4a03, 0x4a04); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  CYC(0x4a04, 0x4a06); A = 0x0a;
  CYC(0x4a06, 0x4a08); B = 0x01;
  if (!(F & FZ)) { CYCT(0x4a08, 0x4a0a); goto L_4a12; } // jr nz
  CYC(0x4a08, 0x4a0a);
  CYC(0x4a0a, 0x4a0c); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4a0c, 0x4a0e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4a0e, 0x4a10); A = 0x0c;
  CYC(0x4a10, 0x4a12); B = 0x03;

L_4a12:
  CYC(0x4a12, 0x4a14); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a14, 0x4a15); mem_wr(gb, HL, A);
  CYC(0x4a15, 0x4a16); A = B;
  CYC(0x4a16, 0x4a19); enemySetAnimation_hook(gb); return; // jp
}

// Disappearing into the ground
void zol_subid00_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a19, 0x4a1a); H = D;
  CYC(0x4a1a, 0x4a1c); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4a1c, 0x4a1d); A = mem_rd(gb, HL);
  CYC(0x4a1d, 0x4a1e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4a1e, 0x4a20); zol_animate_hook(gb); return; } // jr z
  CYC(0x4a1e, 0x4a20);
  CYC(0x4a20, 0x4a21); L = E;
  CYC(0x4a21, 0x4a22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x4a22, 0x4a24); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4a24, 0x4a26); mem_wr(gb, HL, 0x28);
  CYC(0x4a26, 0x4a28); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4a28, 0x4a29); alu_xor(gb, A);
  CYC(0x4a29, 0x4a2a); mem_wr(gb, HL, A);
  CALL_C(0x4a2a, enemySetAnimation_hook, 0x282b, 0x4a2d);
  CYC(0x4a2d, 0x4a30); objectSetInvisible_hook(gb); return; // jp
}

// Fully disappeared into ground. Wait [counter1] frames before we can emerge again
void zol_subid00_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a30, ecom_decCounter1_b0e_hook, 0x439a, 0x4a33);
  if (!(F & FZ)) { RET_TAKEN(0x4a33); return; } // ret nz
  CYC(0x4a33, 0x4a34);
  CYC(0x4a34, 0x4a35); L = E;
  CYC(0x4a35, 0x4a37); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x4a37, 0x4a38); alu_xor(gb, A);
  CYC(0x4a38, 0x4a3b); enemySetAnimation_hook(gb); return; // jp
}

void zol_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a3b, 0x4a3c); A = mem_rd(gb, DE);
  CYC(0x4a3c, 0x4a3e); alu_sub(gb, 0x08);
  {
    CYC(0x4a3e, 0x4a3f); push_effect(gb, 0x4a3f);
    uint16_t target = zol_jump_table(gb);
    if (target == 0x4a4b) { zol_subid01_state8_hook(gb); return; }
    if (target == 0x4a73) { zol_subid01_state9_hook(gb); return; }
    if (target == 0x4a86) { zol_subid01_stateA_hook(gb); return; }
    if (target == 0x4aa9) { zol_subid01_stateB_hook(gb); return; }
    if (target == 0x4ac3) { zol_subid01_stateC_hook(gb); return; }
    if (target == 0x4adc) { zol_subid01_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Holding still for [counter1] frames before deciding whether to hop or move forward
void zol_subid01_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a4b, ecom_decCounter1_b0e_hook, 0x439a, 0x4a4e);
  if (!(F & FZ)) { CYCT(0x4a4e, 0x4a50); zol_animate2_hook(gb); return; } // jr nz
  CYC(0x4a4e, 0x4a50);
  CALL_C(0x4a50, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4a53);
  CYC(0x4a53, 0x4a55); alu_and(gb, 0x07);
  CYC(0x4a55, 0x4a56); H = D;
  CYC(0x4a56, 0x4a58); L = ENEMY_BASE + OBJ_COUNTER1;
  if (F & FZ) { CYCT(0x4a58, 0x4a5a); goto hopTowardLink; } // jr z
  CYC(0x4a58, 0x4a5a);
  CYC(0x4a5a, 0x4a5c); mem_wr(gb, HL, 0x10); // [counter1]
  CYC(0x4a5c, 0x4a5e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a5e, 0x4a5f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x4a5f, 0x4a61); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4a61, 0x4a63); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(0x4a63, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x4a66);
  CYCT(0x4a66, 0x4a68); zol_animate2_hook(gb); return; // jr

hopTowardLink:
  CYC(0x4a68, 0x4a6a); mem_wr(gb, HL, 0x20); // [counter1]
  CYC(0x4a6a, 0x4a6c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a6c, 0x4a6e); mem_wr(gb, HL, 0x0a);
  CYC(0x4a6e, 0x4a70); A = 0x05;
  CYC(0x4a70, 0x4a73); enemySetAnimation_hook(gb); return; // jp
}

// Sliding toward Link
void zol_subid01_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a73, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x4a76);
  CALL_C(0x4a76, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x4a79);
  CALL_C(0x4a79, ecom_decCounter1_b0e_hook, 0x439a, 0x4a7c);
  if (!(F & FZ)) { CYCT(0x4a7c, 0x4a7e); zol_animate2_hook(gb); return; } // jr nz
  CYC(0x4a7c, 0x4a7e);
  CYC(0x4a7e, 0x4a80); mem_wr(gb, HL, 0x18); // [counter1]
  CYC(0x4a80, 0x4a82); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4a82, 0x4a83); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  zol_animate2_hook(gb); return; // fallthrough
}

void zol_animate2_hook(GB *gb) {
  CYC(0x4a83, 0x4a86); enemyAnimate_hook(gb); return; // jp
}

// Shaking before hopping toward Link
void zol_subid01_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a86, ecom_decCounter1_b0e_hook, 0x439a, 0x4a89);
  if (!(F & FZ)) { CYCT(0x4a89, 0x4a8b); zol_animate2_hook(gb); return; } // jr nz
  CYC(0x4a89, 0x4a8b);
  CALL_C(0x4a8b, ecom_incState_b0e_hook, 0x4000, 0x4a8e);
  CYC(0x4a8e, 0x4a90); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x4a90, 0x4a92); mem_wr(gb, HL, 0x00);
  CYC(0x4a92, 0x4a93); L = alu_inc8(gb, L);
  CYC(0x4a93, 0x4a95); mem_wr(gb, HL, 0xfe);
  CYC(0x4a95, 0x4a97); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4a97, 0x4a99); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x4a99, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x4a9c);
  CYC(0x4a9c, 0x4a9e); A = 0x02;
  CALL_C(0x4a9e, enemySetAnimation_hook, 0x282b, 0x4aa1);
  CYC(0x4aa1, 0x4aa3); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x4aa3, playSound_b00_hook, 0x0c98, 0x4aa6);
  CYC(0x4aa6, 0x4aa9); objectSetVisiblec1_hook(gb); return; // jp
}

// Hopping toward Link
void zol_subid01_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4aa9, ecom_applyVelocityForSideviewEnemy_b0e_hook, 0x4153, 0x4aac);
  CYC(0x4aac, 0x4aae); C = 0x28;
  CALL_C(0x4aae, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4ab1);
  if (!(F & FZ)) { RET_TAKEN(0x4ab1); return; } // ret nz
  CYC(0x4ab1, 0x4ab2);
  CYC(0x4ab2, 0x4ab3); H = D;
  CYC(0x4ab3, 0x4ab5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4ab5, 0x4ab7); mem_wr(gb, HL, 0x18);
  CYC(0x4ab7, 0x4ab9); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ab9, 0x4abb); mem_wr(gb, HL, 0x08);
  CYC(0x4abb, 0x4abd); A = 0x04;
  CALL_C(0x4abd, enemySetAnimation_hook, 0x282b, 0x4ac0);
  CYC(0x4ac0, 0x4ac3); objectSetVisiblec2_hook(gb); return; // jp
}

// Zol has been attacked, create puff, disable collisions, prepare to spawn two gels in the
// zol's place.
void zol_subid01_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ac3, 0x4ac5); B = 0x08; // INTERAC_KILLENEMYPUFF
  CALL_C(0x4ac5, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x4ac8);
  CYC(0x4ac8, 0x4ac9); H = D;
  CYC(0x4ac9, 0x4acb); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x4acb, 0x4acd); mem_wr(gb, HL, 18);
  CYC(0x4acd, 0x4acf); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4acf, 0x4ad1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x4ad1, 0x4ad3); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ad3, 0x4ad4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x4ad4, 0x4ad6); A = 0x73; // SND_KILLENEMY
  CALL_C(0x4ad6, playSound_b00_hook, 0x0c98, 0x4ad9);
  CYC(0x4ad9, 0x4adc); objectSetInvisible_hook(gb); return; // jp
}

// Zol has been attacked, spawn gels after [counter2] frames
void zol_subid01_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4adc, ecom_decCounter2_b0e_hook, 0x43a3, 0x4adf);
  if (!(F & FZ)) { RET_TAKEN(0x4adf); return; } // ret nz
  CYC(0x4adf, 0x4ae0);
  CYC(0x4ae0, 0x4ae2); C = 0x04;
  CALL_C(0x4ae2, zol_spawnGel_hook, 0x4af0, 0x4ae5);
  CYC(0x4ae5, 0x4ae7); C = 0xfc;
  CALL_C(0x4ae7, zol_spawnGel_hook, 0x4af0, 0x4aea);
  CALL_C(0x4aea, decNumEnemies_hook, 0x24b3, 0x4aed);
  CYC(0x4aed, 0x4af0); enemyDelete_hook(gb); return; // jp
}

// @param  c  X offset
void zol_spawnGel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4af0, 0x4af2); B = 0x43; // ENEMY_GEL
  CALL_C(0x4af2, ecom_spawnEnemyWithSubid01_b0e_hook, 0x4373, 0x4af5);
  if (!(F & FZ)) { RET_TAKEN(0x4af5); return; } // ret nz
  CYC(0x4af5, 0x4af6);
  CYC(0x4af6, 0x4af7); mem_wr(gb, HL, A); // [child.subid] = 0
  CYC(0x4af7, 0x4af9); B = 0x00;
  CALL_C(0x4af9, objectCopyPositionWithOffset_hook, 0x225a, 0x4afc);
  CYC(0x4afc, 0x4afd); alu_xor(gb, A);
  CYC(0x4afd, 0x4aff); L = ENEMY_BASE + OBJ_Z;
  CYC(0x4aff, 0x4b00); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4b00, 0x4b01); mem_wr(gb, HL, A);
  CYC(0x4b01, 0x4b03); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x4b03, 0x4b04); E = L;
  CYC(0x4b04, 0x4b05); A = mem_rd(gb, DE);
  CYC(0x4b05, 0x4b06); mem_wr(gb, HL, A);
  RET(0x4b06); return; // ret
}
