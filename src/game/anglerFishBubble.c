#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode26_jump_table(GB *gb) {
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

// enemyCode26@popBubble: an @-local reached both by a genuine `call` from enemyCode26 itself
// (which pushes 0x6cbf -- @normalStatus's own address -- as the return address, so state
// dispatch runs normally right after popping the bubble) and by two genuine tail `jr`s from
// @state1 (which never push, relying on this routine's own final tail-jump into
// objectSetVisible83 to pop whatever @state1's real caller left on the stack). Declared as a
// bare _hook (not static) only because it needs CALL_C/sp0_ internally; never registered on its
// own since registering enemyCode26 collapses this whole routine's @-locals (rule 6).
void anglerFishBubble_popBubble_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d06, 0x6d07); H = D;
  CYC(0x6d07, 0x6d09); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d09, 0x6d0b); mem_wr(gb, HL, 0x02);
  CYC(0x6d0b, 0x6d0d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d0d, 0x6d0f); mem_wr(gb, HL, 0x08);
  CYC(0x6d0f, 0x6d11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6d11, 0x6d13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x6d13, 0x6d15); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x6d15, 0x6d17); mem_wr(gb, HL, 0x00);
  // 1 in 4 chance of item drop
  CALL_C(0x6d17, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6d1a);
  CYC(0x6d1a, 0x6d1c); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYCT(0x6d1c, 0x6d1e); goto popAnimation; } // jr nc
  CYC(0x6d1c, 0x6d1e);
  CALL_C(0x6d1e, getFreePartSlot_hook, 0x3e8e, 0x6d21);
  if (!(F & FZ)) { CYCT(0x6d21, 0x6d23); goto popAnimation; } // jr nz
  CYC(0x6d21, 0x6d23);
  CYC(0x6d23, 0x6d25); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(0x6d25, 0x6d26); L = alu_inc8(gb, L);
  CYC(0x6d26, 0x6d28); mem_wr(gb, HL, 0x06); // ITEM_DROP_SCENT_SEEDS
  CYC(0x6d28, 0x6d2a); L = PART_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x6d2a, 0x6d2c); mem_wr(gb, HL, 0xf0);
  CALL_C(0x6d2c, objectCopyPosition_hook, 0x2242, 0x6d2f);

popAnimation:
  // Bubble pop animation
  CYC(0x6d2f, 0x6d31); A = 0x01;
  CALL_C(0x6d31, enemySetAnimation_hook, 0x282b, 0x6d34);
  CYC(0x6d34, 0x6d37); objectSetVisible83_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_ANGLER_FISH_BUBBLE
//
// All of this enemy's states are @-local to enemyCode26 in the disassembly (no separate
// top-level symbols aside from @popBubble above), so the state machine lives in this one hook.
// ==================================================================================================
void enemyCode26_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6cb7, 0x6cb9); goto normalStatus; } // jr z
  CYC(0x6cb7, 0x6cb9);
  CYC(0x6cb9, 0x6cbb); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x6cbb, 0x6cbc); ret_effect(gb); return; } // ret c
  CYC(0x6cbb, 0x6cbc);
  CYC(0x6cbc, 0x6cbf); push_effect(gb, 0x6cbf); anglerFishBubble_popBubble_hook(gb);

normalStatus:
  CYC(0x6cbf, 0x6cc1); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6cc1, 0x6cc2); A = mem_rd(gb, DE);
  CYC(0x6cc2, 0x6cc3); push_effect(gb, 0x6cc3);
  {
    uint16_t target = enemyCode26_jump_table(gb);
    if (target == 0x6cec) goto state1;
    if (target == 0x6d37) goto state2;
    goto state0; // target == 0x6cc9
  }

  // Initialization
state0:
  CYC(0x6cc9, 0x6cca); H = D;
  CYC(0x6cca, 0x6ccb); L = E;
  CYC(0x6ccb, 0x6ccc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  // Can bounce off walls 5 times before popping
  CYC(0x6ccc, 0x6cce); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6cce, 0x6cd0); mem_wr(gb, HL, 0x05);
  CYC(0x6cd0, 0x6cd2); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6cd2, 0x6cd4); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x6cd4, 0x6cd6); A = OBJ_DIRECTION; // Object.direction
  CALL_C(0x6cd6, objectGetRelatedObject1Var_hook, 0x2160, 0x6cd9);
  CYC(0x6cd9, 0x6cdb); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x6cdb, 0x6cdd); C = 0xf4;
  if (F & FZ) { CYCT(0x6cdd, 0x6cdf); goto haveOffset; } // jr z
  CYC(0x6cdd, 0x6cdf);
  CYC(0x6cdf, 0x6ce1); C = 0x0c;

haveOffset:
  CYC(0x6ce1, 0x6ce3); B = 0x00;
  CALL_C(0x6ce3, objectTakePositionWithOffset_hook, 0x2277, 0x6ce6);
  CALL_C(0x6ce6, ecom_updateAngleTowardTarget_b0d_hook, 0x43bf, 0x6ce9);
  CYC(0x6ce9, 0x6cec); objectSetVisible81_hook(gb); return; // jp

  // Bubble moving around
state1:
  CYC(0x6cec, 0x6cee); A = OBJ_ID; // Object.id
  CALL_C(0x6cee, objectGetRelatedObject1Var_hook, 0x2160, 0x6cf1);
  CYC(0x6cf1, 0x6cf2); A = mem_rd(gb, HL);
  CYC(0x6cf2, 0x6cf4); alu_cp(gb, 0x76); // ENEMY_ANGLER_FISH
  if (!(F & FZ)) { CYCT(0x6cf4, 0x6cf6); anglerFishBubble_popBubble_hook(gb); return; } // jr nz
  CYC(0x6cf4, 0x6cf6);
  CALL_C(0x6cf6, objectApplySpeed_hook, 0x201d, 0x6cf9);
  CALL_C(0x6cf9, ecom_bounceOffWallsAndHoles_b0d_hook, 0x42de, 0x6cfc);
  if (F & FZ) { CYCT(0x6cfc, 0x6cfe); goto animate; } // jr z
  CYC(0x6cfc, 0x6cfe);
  // Each time it bounces off a wall, decrement counter1
  CALL_C(0x6cfe, ecom_decCounter1_b0d_hook, 0x439a, 0x6d01);
  if (F & FZ) { CYCT(0x6d01, 0x6d03); anglerFishBubble_popBubble_hook(gb); return; } // jr z
  CYC(0x6d01, 0x6d03);

animate:
  CYC(0x6d03, 0x6d06); enemyAnimate_hook(gb); return; // jp

  // Bubble in the process of popping
state2:
  CALL_C(0x6d37, ecom_decCounter1_b0d_hook, 0x439a, 0x6d3a);
  if (!(F & FZ)) { CYCT(0x6d3a, 0x6d3c); goto animate; } // jr nz
  CYC(0x6d3a, 0x6d3c);
  CYC(0x6d3c, 0x6d3f); enemyDelete_hook(gb); return; // jp
}
