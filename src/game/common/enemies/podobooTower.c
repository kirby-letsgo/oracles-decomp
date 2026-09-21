#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode2d);
  push_effect(gb, return_address);
  CYC(b_+236, b_+239); A = W8(wFrameCounter);
  CYC(b_+239, b_+241); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+241, b_+242); ret_effect(gb); return; } // ret nz
  CYC(b_+241, b_+242);
  CYC(b_+242, b_+245); ecom_decCounter2_b0d_hook(gb); // jp
}

// enemyCode2d@updateCollisionRadiiAndYPosition: an @-local reached both by a genuine `call`
// (from @state9, which pushes its own return address before invoking this) and by a genuine
// tail `jr z` (from @stateB, which never pushes and relies on this routine's own `ret` to pop
// whatever @stateB's real caller left on the stack). This body always ends with the real `ret`
// and never diverges elsewhere, so neither call site needs a pc/sp resume check.
static void enemyCode2d_updateCollisionRadiiAndYPosition(GB *gb) {
  BASE(enemyCode2d);
  CYC(b_+196, b_+198); alu_sub(gb, 0x03);
  CYC(b_+198, b_+201); SET_HL(b_+221); // @data
  CYC(b_+201, b_+202); enemyCode2d_addAToHl_from_rst(gb, b_+202);
  CYC(b_+202, b_+204); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+204, b_+205); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+205, b_+206); mem_wr(gb, DE, A);
  CYC(b_+206, b_+207); E = alu_inc8(gb, E);
  CYC(b_+207, b_+208); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+208, b_+209); mem_wr(gb, DE, A);
  CYC(b_+209, b_+211); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+211, b_+212); A = mem_rd(gb, DE);
  CYC(b_+212, b_+213); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+213, b_+215); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+215, b_+216); mem_wr(gb, DE, A);
  CYC(b_+216, b_+218); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+218, b_+219); alu_xor(gb, A);
  CYC(b_+219, b_+220); mem_wr(gb, DE, A);
  CYC(b_+220, b_+221); ret_effect(gb);
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
  BASE(enemyCode2d);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_withoutItemDrop_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  // ENEMYSTATUS_JUST_HIT or ENEMYSTATUS_KNOCKBACK
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (F & FZ) { CYCT(b_+13, b_+16); enemyDie_uncounted_withoutItemDrop_hook(gb); return; } // jp z
  CYC(b_+13, b_+16);

normalStatus:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  {
    uint16_t target = enemyCode2d_jump_table(gb);
    if (target == b_+67) goto state8;
    if (target == b_+82) goto state9;
    if (target == b_+109) goto stateA;
    if (target == b_+143) goto stateB;
    if (target == b_+167) goto stateC;
    if (target == b_+183) goto stateD;
    if (target == b_+48) goto state_uninitialized;
    goto state_stub; // states 1-7 all target 0x65c7
  }

state_uninitialized:
  CALL_C(b_+48, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+51);
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+53, b_+55); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CYC(b_+55, b_+57); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 60);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  RET(b_+65); return;

state_stub:
  RET(b_+66); return;

  // Head is in the ground, flickering, for 60 frames
state8:
  CALL_C(b_+67, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+73); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+77, b_+79); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+79, b_+82); TAIL(objectSetVisible82); // jp

  // Rising up out of the ground
state9:
  CALL_C(b_+82, enemyAnimate_hook, SYM(enemyAnimate), b_+85);
  CYC(b_+85, b_+87); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+87, b_+88); A = mem_rd(gb, DE);
  CYC(b_+88, b_+89); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+89, b_+90); ret_effect(gb); return; } // ret z
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+91); B = A;
  CYC(b_+91, b_+94); push_effect(gb, b_+94); enemyCode2d_updateCollisionRadiiAndYPosition(gb);
  CYC(b_+94, b_+95); A = B;
  CYC(b_+95, b_+97); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+97, b_+98); ret_effect(gb); return; } // ret nz
  CYC(b_+97, b_+98);
  // Fully emerged
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+101); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+101, b_+102); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+102, b_+104); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 150);
  CYC(b_+106, b_+107); L = alu_inc8(gb, L);
  CYC(b_+107, b_+109); mem_wr(gb, HL, 180); // [counter2]
  // falls through to stateA

  // Fully emerged from ground, firing at Link until counter2 reaches 0
stateA:
  CYC(b_+109, b_+112); enemyCode2d_decCounter2Every4Frames(gb, b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+114); goto stateAFireCheck; } // jr nz
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+115); L = E;
  CYC(b_+115, b_+116); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+116, b_+118); A = 0x01;
  CYC(b_+118, b_+121); TAIL(enemySetAnimation); // jp

stateAFireCheck:
  // Randomly fire projectile when [counter1] reaches 0
  CALL_C(b_+121, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+124);
  if (!(F & FZ)) { CYCT(b_+124, b_+126); goto animate; } // jr nz
  CYC(b_+124, b_+126);
  CYC(b_+126, b_+128); mem_wr(gb, HL, 150); // $96
  CALL_C(b_+128, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+131);
  CYC(b_+131, b_+133); alu_cp(gb, 0xb4);
  if (!(F & FC)) { CYCT(b_+133, b_+135); goto animate; } // jr nc
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+137); B = 0x31; // PART_GOPONGA_PROJECTILE
  CALL_C(b_+137, ecom_spawnProjectile_b0d_hook, SYM(ecom_spawnProjectile_b0d), b_+140);
  // falls through to animate

animate:
  CYC(b_+140, b_+143); TAIL(enemyAnimate); // jp

  // Moving back into the ground
stateB:
  CALL_C(b_+143, enemyAnimate_hook, SYM(enemyAnimate), b_+146);
  CYC(b_+146, b_+148); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+148, b_+149); A = mem_rd(gb, DE);
  CYC(b_+149, b_+150); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+150, b_+151); ret_effect(gb); return; } // ret z
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+153, b_+155); enemyCode2d_updateCollisionRadiiAndYPosition(gb); return; } // jr z
  CYC(b_+153, b_+155);
  // Head reached the ground
  CALL_C(b_+155, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+158);
  CYC(b_+158, b_+160); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+160, b_+162); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+162, b_+164); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+164, b_+166); mem_wr(gb, HL, 60);
  RET(b_+166); return;

  // Head is in the ground, flickering, for 60 frames.
stateC:
  CALL_C(b_+167, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+170);
  if (!(F & FZ)) { CYCT(b_+170, b_+173); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(b_+170, b_+173);
  CYC(b_+173, b_+175); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+175, b_+176); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+176, b_+178); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+178, b_+180); mem_wr(gb, HL, 180);
  CYC(b_+180, b_+183); TAIL(objectSetInvisible); // jp

  // Waiting underground for [counter1] frames.
stateD:
  CALL_C(b_+183, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+186);
  if (!(F & FZ)) { CYCT(b_+186, b_+187); ret_effect(gb); return; } // ret nz
  CYC(b_+186, b_+187);
  CYC(b_+187, b_+189); mem_wr(gb, HL, 60);
  CYC(b_+189, b_+190); L = E;
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+192, b_+193); alu_xor(gb, A);
  CYC(b_+193, b_+196); TAIL(enemySetAnimation); // jp
}
