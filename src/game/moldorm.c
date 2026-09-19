#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void moldorm_state_uninitialized_hook(GB *gb);
void moldorm_state1_hook(GB *gb);
void moldorm_state_stub_hook(GB *gb);
void moldorm_head_hook(GB *gb);
void moldorm_tail_hook(GB *gb);
void moldorm_tail_delete_hook(GB *gb);
void moldorm_tail_setRelatedObj1AndCopyPosition_hook(GB *gb);
void moldorm_head_updateAnimationFromAngle_hook(GB *gb);
void moldorm_tail_clearOffsetBuffer_hook(GB *gb);
void moldorm_checkHazards_hook(GB *gb);

static uint16_t moldorm_jump_table(GB *gb) {
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
// ENEMY_MOLDORM
//
// Variables for head (subid 1):
//   var30: Tail 1 object index
//   var31: Tail 2 object index
//   var32: Animation index
//   var33: Angular speed (added to angle)
//
// Variables for tail (subids 2-3):
//   relatedObj1: Object to follow (either the head or the tail in front)
//   var30: Index for offset buffer
//   var31/var32: Parent object's position last frame
//   var33-var3b: Offset buffer. Stores the parent's movement offsets for up to 8 frames.
// ==================================================================================================
void enemyCode4f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6197, moldorm_checkHazards_hook, 0x6322, 0x619a);
  if (F & FZ) { CYCT(0x619a, 0x619c); goto normalStatus; } // jr z
  CYC(0x619a, 0x619c);
  CYC(0x619c, 0x619e); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x619e); return; } // ret c
  CYC(0x619e, 0x619f);
  if (F & FZ) { CYCT(0x619f, 0x61a1); goto dead; } // jr z
  CYC(0x619f, 0x61a1);
  CYC(0x61a1, 0x61a2); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x61a2, 0x61a4); goto knockback; } // jr nz
  CYC(0x61a2, 0x61a4);
  CYC(0x61a4, 0x61a6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x61a6, 0x61a7); A = mem_rd(gb, DE);
  CYC(0x61a7, 0x61a8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x61a8, 0x61aa); goto normalStatus; } // jr nz
  CYC(0x61a8, 0x61aa);
  CYC(0x61aa, 0x61ac); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x61ac, 0x61ad); L = E;
  CYC(0x61ad, 0x61ae); A = mem_rd(gb, DE);
  CYC(0x61ae, 0x61af); B = A;
  CYC(0x61af, 0x61b1); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x61b1, 0x61b2); A = mem_rd(gb, DE);
  CYC(0x61b2, 0x61b3); H = A;
  CYC(0x61b3, 0x61b4); mem_wr(gb, HL, B);
  CYC(0x61b4, 0x61b5); E = alu_inc8(gb, E);
  CYC(0x61b5, 0x61b6); A = mem_rd(gb, DE);
  CYC(0x61b6, 0x61b7); H = A;
  CYC(0x61b7, 0x61b8); mem_wr(gb, HL, B);
  RET(0x61b8); return; // ret

dead:
  CYC(0x61b9, 0x61bb); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x61bb, 0x61bc); A = mem_rd(gb, DE);
  CYC(0x61bc, 0x61bd); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x61bd, 0x61c0); moldorm_tail_delete_hook(gb); return; } // jp nz
  CYC(0x61bd, 0x61c0);
  CYC(0x61c0, 0x61c2); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x61c2, 0x61c3); A = mem_rd(gb, DE);
  CYC(0x61c3, 0x61c4); H = A;
  CALL_C(0x61c4, ecom_killObjectH_b0e_hook, 0x445e, 0x61c7);
  CYC(0x61c7, 0x61c8); E = alu_inc8(gb, E);
  CYC(0x61c8, 0x61c9); A = mem_rd(gb, DE);
  CYC(0x61c9, 0x61ca); H = A;
  CALL_C(0x61ca, ecom_killObjectH_b0e_hook, 0x445e, 0x61cd);
  CYC(0x61cd, 0x61d0); enemyDie_hook(gb); return; // jp

knockback:
  CYC(0x61d0, 0x61d2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x61d2, 0x61d3); A = mem_rd(gb, DE);
  CYC(0x61d3, 0x61d4); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x61d4, 0x61d6); goto normalStatus; } // jr nz
  CYC(0x61d4, 0x61d6);
  CYC(0x61d6, 0x61d9); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; // jp

normalStatus:
  CALL_C(0x61d9, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x61dc);
  if (!(F & FC)) { CYCT(0x61dc, 0x61de); goto normalState; } // jr nc
  CYC(0x61dc, 0x61de);
  {
    CYC(0x61de, 0x61df); push_effect(gb, 0x61df);
    uint16_t target = moldorm_jump_table(gb);
    if (target == 0x61f8) { moldorm_state_uninitialized_hook(gb); return; }
    if (target == 0x620b) { moldorm_state1_hook(gb); return; }
    if (target == 0x623c) { moldorm_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x61ef, 0x61f0); B = alu_dec8(gb, B);
  CYC(0x61f0, 0x61f1); A = B;
  {
    CYC(0x61f1, 0x61f2); push_effect(gb, 0x61f2);
    uint16_t target = moldorm_jump_table(gb);
    if (target == 0x623d) { moldorm_head_hook(gb); return; }
    if (target == 0x6283) { moldorm_tail_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:61f8, bare global; jump-table target from enemyCode4f.
void moldorm_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x61f8, 0x61f9); A = B;
  CYC(0x61f9, 0x61fa); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x61fa, 0x61fc); goto notSpawner; } // jr nz
  CYC(0x61fa, 0x61fc);
  CYC(0x61fc, 0x61fd); A = alu_inc8(gb, A);
  CYC(0x61fd, 0x61fe); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x61fe, 0x6200); moldorm_state1_hook(gb); return; // jr

notSpawner:
  CALL_C(0x6200, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x6203);
  CYC(0x6203, 0x6204); A = B;
  CYC(0x6204, 0x6205); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x6205); return; } // ret z
  CYC(0x6205, 0x6206);
  CYC(0x6206, 0x6208); alu_add(gb, 0x07);
  CYC(0x6208, 0x620b); enemySetAnimation_hook(gb); return; // jp
}

// 0e:620b, bare global; jump-table target from enemyCode4f, also falls into from
// moldorm_state_uninitialized. Spawner; spawn the head and tails, then delete self.
void moldorm_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x620b, 0x620d); B = 0x03;
  CALL_C(0x620d, checkBEnemySlotsAvailable_hook, 0x20f7, 0x6210);
  if (!(F & FZ)) { CYCT(0x6210, 0x6213); objectSetVisible82_hook(gb); return; } // jp nz
  CYC(0x6210, 0x6213);
  CYC(0x6213, 0x6215); B = 0x4f; // ENEMY_MOLDORM
  CALL_C(0x6215, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x6218);
  CYC(0x6218, 0x6219); C = H;
  PUSH(0x6219, HL);
  CALL_C(0x621a, ecom_spawnEnemyWithSubid01_b0e_hook, 0x4373, 0x621d);
  CYC(0x621d, 0x621e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 2
  CALL_C(0x621e, moldorm_tail_setRelatedObj1AndCopyPosition_hook, 0x62f8, 0x6221);
  CYC(0x6221, 0x6222); C = H;
  CALL_C(0x6222, ecom_spawnEnemyWithSubid01_b0e_hook, 0x4373, 0x6225);
  CYC(0x6225, 0x6226); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6226, 0x6227); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 3
  CALL_C(0x6227, moldorm_tail_setRelatedObj1AndCopyPosition_hook, 0x62f8, 0x622a);
  CYC(0x622a, 0x622b); B = H;
  SET_HL(POP(0x622b));
  CYC(0x622c, 0x622e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x622e, 0x622f); mem_wr(gb, HL, C);
  CYC(0x622f, 0x6230); L = alu_inc8(gb, L);
  CYC(0x6230, 0x6231); mem_wr(gb, HL, B);
  CYC(0x6231, 0x6233); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x6233, 0x6234); E = L;
  CYC(0x6234, 0x6235); A = mem_rd(gb, DE);
  CYC(0x6235, 0x6236); mem_wr(gb, HL, A);
  CALL_C(0x6236, objectCopyPosition_hook, 0x2242, 0x6239);
  CYC(0x6239, 0x623c); enemyDelete_hook(gb); return; // jp
}

// 0e:623c, bare global; jump-table target from enemyCode4f.
void moldorm_state_stub_hook(GB *gb) {
  RET(0x623c); return; // ret
}

// 0e:623d, bare global; jump-table target from enemyCode4f@normalState. Subid 1.
void moldorm_head_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x623d, 0x623e); A = mem_rd(gb, DE);
  CYC(0x623e, 0x6240); alu_sub(gb, 0x08);
  {
    CYC(0x6240, 0x6241); push_effect(gb, 0x6241);
    uint16_t target = moldorm_jump_table(gb);
    if (target == 0x6245) goto state8;
    if (target == 0x625a) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(0x6245, 0x6246); H = D;
  CYC(0x6246, 0x6247); L = E;
  CYC(0x6247, 0x6248); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6248, 0x624a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x624a, 0x624c); mem_wr(gb, HL, 0x08);
  CYC(0x624c, 0x624e); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x624e, 0x6250); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x6250, 0x6252); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x6252, 0x6254); mem_wr(gb, HL, 0x02);
  CALL_C(0x6254, ecom_setRandomAngle_b0e_hook, 0x43cf, 0x6257);
  CYC(0x6257, 0x625a); moldorm_head_updateAnimationFromAngle_hook(gb); return; // jp

state9:
  CALL_C(0x625a, ecom_decCounter1_b0e_hook, 0x439a, 0x625d);
  if (!(F & FZ)) { CYCT(0x625d, 0x625f); goto applySpeed; } // jr nz
  CYC(0x625d, 0x625f);
  CYC(0x625f, 0x6261); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(0x6261, 0x6263); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x6263, 0x6265); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6265, 0x6266); A = mem_rd(gb, DE);
  CYC(0x6266, 0x6267); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6267, 0x6269); alu_and(gb, 0x1f);
  CYC(0x6269, 0x626a); mem_wr(gb, DE, A);
  CALL_C(0x626a, moldorm_head_updateAnimationFromAngle_hook, 0x6301, 0x626d);
  CALL_C(0x626d, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6270);
  CYC(0x6270, 0x6272); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x6272, 0x6274); goto applySpeed; } // jr nz
  CYC(0x6272, 0x6274);
  CYC(0x6274, 0x6276); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x6276, 0x6277); A = mem_rd(gb, DE);
  CYC(0x6277, 0x6278); alu_cpl(gb);
  CYC(0x6278, 0x6279); A = alu_inc8(gb, A);
  CYC(0x6279, 0x627a); mem_wr(gb, DE, A);

applySpeed:
  CALL_C(0x627a, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x627d);
  if (!(F & FZ)) CALL_C_CC(0x627d, moldorm_head_updateAnimationFromAngle_hook, 0x6301, 0x6280); else CYC(0x627d, 0x6280); // call nz
  CYC(0x6280, 0x6283); objectApplySpeed_hook(gb); return; // jp
}

// 0e:6283, bare global; jump-table target from enemyCode4f@normalState.
void moldorm_tail_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6283, 0x6285); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6285, 0x6286); A = mem_rd(gb, DE);
  CYC(0x6286, 0x6288); alu_sub(gb, 0x08);
  {
    CYC(0x6288, 0x6289); push_effect(gb, 0x6289);
    uint16_t target = moldorm_jump_table(gb);
    if (target == 0x628d) goto state8;
    if (target == 0x62a4) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(0x628d, 0x628e); H = D;
  CYC(0x628e, 0x628f); L = E;
  CYC(0x628f, 0x6290); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6290, 0x6292); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6292, 0x6294); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6294, 0x6296); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6296, 0x6297); H = mem_rd(gb, HL);
  CYC(0x6297, 0x6299); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6299, 0x629b); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x629b, 0x629c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x629c, 0x629d); mem_wr(gb, DE, A);
  CYC(0x629d, 0x629e); E = alu_inc8(gb, E);
  CYC(0x629e, 0x629f); L = alu_inc8(gb, L);
  CYC(0x629f, 0x62a0); A = mem_rd(gb, HL);
  CYC(0x62a0, 0x62a1); mem_wr(gb, DE, A);
  CYC(0x62a1, 0x62a4); moldorm_tail_clearOffsetBuffer_hook(gb); return; // jp

state9:
  CYC(0x62a4, 0x62a6); A = OBJ_ENABLED; // Object.enabled
  CALL_C(0x62a6, objectGetRelatedObject1Var_hook, 0x2160, 0x62a9);
  CYC(0x62a9, 0x62aa); A = mem_rd(gb, HL);
  CYC(0x62aa, 0x62ab); alu_or(gb, A);
  if (F & FZ) { CYCT(0x62ab, 0x62ad); moldorm_tail_delete_hook(gb); return; } // jr z
  CYC(0x62ab, 0x62ad);
  CYC(0x62ad, 0x62af); L = ENEMY_BASE + OBJ_YH;
  CYC(0x62af, 0x62b1); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x62b1, 0x62b2); A = mem_rd(gb, DE);
  CYC(0x62b2, 0x62b3); B = A;
  CYC(0x62b3, 0x62b4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x62b4, 0x62b5); alu_sub(gb, B);
  CYC(0x62b5, 0x62b7); alu_add(gb, 0x08);
  CYC(0x62b7, 0x62b9); A = alu_swap(gb, A);
  CYC(0x62b9, 0x62ba); B = A;
  CYC(0x62ba, 0x62bb); E = alu_inc8(gb, E);
  CYC(0x62bb, 0x62bc); L = alu_inc8(gb, L);
  CYC(0x62bc, 0x62bd); A = mem_rd(gb, DE);
  CYC(0x62bd, 0x62be); C = A;
  CYC(0x62be, 0x62bf); A = mem_rd(gb, HL);
  CYC(0x62bf, 0x62c0); alu_sub(gb, C);
  CYC(0x62c0, 0x62c2); alu_add(gb, 0x08);
  CYC(0x62c2, 0x62c3); alu_or(gb, B);
  CYC(0x62c3, 0x62c4); B = A;
  CYC(0x62c4, 0x62c5); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x62c5, 0x62c6); mem_wr(gb, DE, A);
  CYC(0x62c6, 0x62c7); E = alu_dec8(gb, E);
  CYC(0x62c7, 0x62c8); L = alu_dec8(gb, L);
  CYC(0x62c8, 0x62c9); A = mem_rd(gb, HL);
  CYC(0x62c9, 0x62ca); mem_wr(gb, DE, A);
  CYC(0x62ca, 0x62cc); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x62cc, 0x62cd); A = mem_rd(gb, DE);
  CYC(0x62cd, 0x62cf); alu_add(gb, ENEMY_BASE + OBJ_VAR33); // add Enemy.var33
  CYC(0x62cf, 0x62d0); E = A;
  CYC(0x62d0, 0x62d1); A = B;
  CYC(0x62d1, 0x62d2); mem_wr(gb, DE, A);
  CYC(0x62d2, 0x62d3); H = D;
  CYC(0x62d3, 0x62d5); L = ENEMY_BASE + OBJ_YH;
  CYC(0x62d5, 0x62d7); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x62d7, 0x62d8); A = mem_rd(gb, DE);
  CYC(0x62d8, 0x62d9); A = alu_inc8(gb, A);
  CYC(0x62d9, 0x62db); alu_and(gb, 0x07);
  CYC(0x62db, 0x62dc); mem_wr(gb, DE, A);
  CYC(0x62dc, 0x62de); alu_add(gb, ENEMY_BASE + OBJ_VAR33); // add Enemy.var33
  CYC(0x62de, 0x62df); E = A;
  CYC(0x62df, 0x62e0); A = mem_rd(gb, DE);
  CYC(0x62e0, 0x62e1); B = A;
  CYC(0x62e1, 0x62e3); alu_and(gb, 0xf0);
  CYC(0x62e3, 0x62e5); A = alu_swap(gb, A);
  CYC(0x62e5, 0x62e7); alu_sub(gb, 0x08);
  CYC(0x62e7, 0x62e8); alu_add(gb, mem_rd(gb, HL)); // [yh]
  CYC(0x62e8, 0x62e9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x62e9, 0x62ea); L = alu_inc8(gb, L);
  CYC(0x62ea, 0x62eb); A = B;
  CYC(0x62eb, 0x62ed); alu_and(gb, 0x0f);
  CYC(0x62ed, 0x62ef); alu_sub(gb, 0x08);
  CYC(0x62ef, 0x62f0); alu_add(gb, mem_rd(gb, HL)); // [xh]
  CYC(0x62f0, 0x62f1); mem_wr(gb, HL, A);
  RET(0x62f1); return; // ret
}

// 0e:62f2, bare global; called from enemyCode4f and moldorm_tail.
void moldorm_tail_delete_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x62f2, decNumEnemies_hook, 0x24b3, 0x62f5);
  CYC(0x62f5, 0x62f8); enemyDelete_hook(gb); return; // jp
}

// 0e:62f8, bare global; called from moldorm_state1 for each spawned tail.
// @param h Object to follow (either the head or the tail in front)
void moldorm_tail_setRelatedObj1AndCopyPosition_hook(GB *gb) {
  CYC(0x62f8, 0x62fa); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x62fa, 0x62fc); A = ENEMY_BASE; // Enemy.start
  CYC(0x62fc, 0x62fd); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x62fd, 0x62fe); mem_wr(gb, HL, C);
  CYC(0x62fe, 0x6301); objectCopyPosition_hook(gb); return; // jp
}

// 0e:6301, bare global; called from moldorm_head.
void moldorm_head_updateAnimationFromAngle_hook(GB *gb) {
  CYC(0x6301, 0x6303); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6303, 0x6304); A = mem_rd(gb, DE);
  CYC(0x6304, 0x6306); alu_add(gb, 0x02);
  CYC(0x6306, 0x6308); alu_and(gb, 0x1c);
  CYC(0x6308, 0x6309); alu_rrca(gb);
  CYC(0x6309, 0x630a); alu_rrca(gb);
  CYC(0x630a, 0x630b); H = D;
  CYC(0x630b, 0x630d); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x630d, 0x630e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x630e); return; } // ret z
  CYC(0x630e, 0x630f);
  CYC(0x630f, 0x6310); mem_wr(gb, HL, A);
  CYC(0x6310, 0x6313); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6313, bare global; called from moldorm_tail.
void moldorm_tail_clearOffsetBuffer_hook(GB *gb) {
  CYC(0x6313, 0x6314); H = D;
  CYC(0x6314, 0x6316); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x6316, 0x6318); B = 0x02;
  CYC(0x6318, 0x631a); A = 0x88;

L_631a:
  CYC(0x631a, 0x631b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x631b, 0x631c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x631c, 0x631d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x631d, 0x631e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x631e, 0x631f); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x631f, 0x6321); goto L_631a; } // jr nz
  CYC(0x631f, 0x6321);
  RET(0x6321); return; // ret
}

// 0e:6322, bare global; called from enemyCode4f.
void moldorm_checkHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6322, 0x6323); B = A;
  CYC(0x6323, 0x6325); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6325, 0x6326); A = mem_rd(gb, DE);
  CYC(0x6326, 0x6327); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6327, 0x6329); goto checkHazards; } // jr z
  CYC(0x6327, 0x6329);
  CYC(0x6329, 0x632b); A = OBJ_VAR3F; // Object.var3f
  CALL_C(0x632b, objectGetRelatedObject1Var_hook, 0x2160, 0x632e);
  CYC(0x632e, 0x632f); A = mem_rd(gb, HL);
  CYC(0x632f, 0x6331); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6331, 0x6333); goto checkHazards; } // jr nz
  CYC(0x6331, 0x6333);
  CYC(0x6333, 0x6334); A = B;
  CYC(0x6334, 0x6335); alu_or(gb, A);
  RET(0x6335); return; // ret

checkHazards:
  CYC(0x6336, 0x6337); A = B;
  CYC(0x6337, 0x633a); ecom_checkHazardsNoAnimationForHoles_b0e_hook(gb); return; // jp
}
