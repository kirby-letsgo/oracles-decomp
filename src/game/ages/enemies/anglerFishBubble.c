#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode26);
  uint16_t sp0_ = gb->sp;
  CYC(b_+79, b_+80); H = D;
  CYC(b_+80, b_+82); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x02);
  CYC(b_+84, b_+86); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x08);
  CYC(b_+88, b_+90); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+90, b_+92); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+92, b_+94); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x00);
  // 1 in 4 chance of item drop
  CALL_C(b_+96, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+99);
  CYC(b_+99, b_+101); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYCT(b_+101, b_+103); goto popAnimation; } // jr nc
  CYC(b_+101, b_+103);
  CALL_C(b_+103, getFreePartSlot_hook, SYM(getFreePartSlot), b_+106);
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto popAnimation; } // jr nz
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(b_+110, b_+111); L = alu_inc8(gb, L);
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x06); // ITEM_DROP_SCENT_SEEDS
  CYC(b_+113, b_+115); L = PART_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+117, objectCopyPosition_hook, SYM(objectCopyPosition), b_+120);

popAnimation:
  // Bubble pop animation
  CYC(b_+120, b_+122); A = 0x01;
  CALL_C(b_+122, enemySetAnimation_hook, SYM(enemySetAnimation), b_+125);
  CYC(b_+125, b_+128); objectSetVisible83_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_ANGLER_FISH_BUBBLE
//
// All of this enemy's states are @-local to enemyCode26 in the disassembly (no separate
// top-level symbols aside from @popBubble above), so the state machine lives in this one hook.
// ==================================================================================================
void enemyCode26_hook(GB *gb) {
  BASE(enemyCode26);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); push_effect(gb, b_+8); anglerFishBubble_popBubble_hook(gb);

normalStatus:
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  {
    uint16_t target = enemyCode26_jump_table(gb);
    if (target == b_+53) goto state1;
    if (target == b_+128) goto state2;
    goto state0; // target == 0x6cc9
  }

  // Initialization
state0:
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+20); L = E;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  // Can bounce off walls 5 times before popping
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x05);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+29, b_+31); A = OBJ_DIRECTION; // Object.direction
  CALL_C(b_+31, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+34);
  CYC(b_+34, b_+36); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+36, b_+38); C = 0xf4;
  if (F & FZ) { CYCT(b_+38, b_+40); goto haveOffset; } // jr z
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); C = 0x0c;

haveOffset:
  CYC(b_+42, b_+44); B = 0x00;
  CALL_C(b_+44, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+47);
  CALL_C(b_+47, ecom_updateAngleTowardTarget_b0d_hook, SYM(ecom_updateAngleTowardTarget_b0d), b_+50);
  CYC(b_+50, b_+53); objectSetVisible81_hook(gb); return; // jp

  // Bubble moving around
state1:
  CYC(b_+53, b_+55); A = OBJ_ID; // Object.id
  CALL_C(b_+55, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+61); alu_cp(gb, 0x76); // ENEMY_ANGLER_FISH
  if (!(F & FZ)) { CYCT(b_+61, b_+63); anglerFishBubble_popBubble_hook(gb); return; } // jr nz
  CYC(b_+61, b_+63);
  CALL_C(b_+63, objectApplySpeed_hook, SYM(objectApplySpeed), b_+66);
  CALL_C(b_+66, ecom_bounceOffWallsAndHoles_b0d_hook, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+69);
  if (F & FZ) { CYCT(b_+69, b_+71); goto animate; } // jr z
  CYC(b_+69, b_+71);
  // Each time it bounces off a wall, decrement counter1
  CALL_C(b_+71, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+74);
  if (F & FZ) { CYCT(b_+74, b_+76); anglerFishBubble_popBubble_hook(gb); return; } // jr z
  CYC(b_+74, b_+76);

animate:
  CYC(b_+76, b_+79); enemyAnimate_hook(gb); return; // jp

  // Bubble in the process of popping
state2:
  CALL_C(b_+128, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+131);
  if (!(F & FZ)) { CYCT(b_+131, b_+133); goto animate; } // jr nz
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+136); enemyDelete_hook(gb); return; // jp
}
