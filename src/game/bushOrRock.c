#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook(GB *gb);
void enemyCode58_copyParentPosition_hook(GB *gb);
void enemyCode58_checkDisableDestruction_hook(GB *gb);

static uint16_t bushOrRock_jump_table(GB *gb) {
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

static void bushOrRock_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void bushOrRock_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0e:6825, bare local (no exported symbol); called via genuine call/ret from several states
// of enemyCode58. Make parent visible, remove self from Parent.relatedObj2.
void enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6825, 0x6827); A = OBJ_VISIBLE; // Object.visible
  CALL_C(0x6827, objectGetRelatedObject1Var_hook, 0x2160, 0x682a);
  CYC(0x682a, 0x682c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x682c, 0x682e); L = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x682e, 0x682f); alu_xor(gb, A);
  CYC(0x682f, 0x6830); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6830, 0x6831); mem_wr(gb, HL, A);
  RET(0x6831); return; // ret
}

// 0e:6832, bare local (no exported symbol); called via genuine call/ret from two states of
// enemyCode58. Copies parent position, with a Z offset determined by parent.var03.
void enemyCode58_copyParentPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6832, 0x6834); A = OBJ_YH; // Object.yh
  CALL_C(0x6834, objectGetRelatedObject1Var_hook, 0x2160, 0x6837);
  CALL_C(0x6837, objectTakePosition_hook, 0x2274, 0x683a);
  CYC(0x683a, 0x683c); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x683c, 0x683d); A = mem_rd(gb, HL);
  CYC(0x683d, 0x683f); alu_and(gb, 0x03);
  CYC(0x683f, 0x6842); SET_HL(0x6849); // @zVals
  CYC(0x6842, 0x6843); bushOrRock_addAToHl_from_rst(gb, 0x6843);
  CYC(0x6843, 0x6845); E = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x6845, 0x6846); A = mem_rd(gb, DE);
  CYC(0x6846, 0x6847); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6847, 0x6848); mem_wr(gb, DE, A);
  RET(0x6848); return; // ret
}

// 0e:684d, bare local (no exported symbol); called via genuine call/ret from
// enemyCode58@state_uninitialized. Disable bush destruction for deku scrubs only.
void enemyCode58_checkDisableDestruction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x684d, 0x684f); A = OBJ_ID; // Object.id
  CALL_C(0x684f, objectGetRelatedObject1Var_hook, 0x2160, 0x6852);
  CYC(0x6852, 0x6854); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6854, 0x6855); A = mem_rd(gb, HL);
  CYC(0x6855, 0x6856); mem_wr(gb, DE, A);
  CYC(0x6856, 0x6858); alu_cp(gb, 0x27); // ENEMY_DEKU_SCRUB
  if (!(F & FZ)) { RET_TAKEN(0x6858); return; } // ret nz
  CYC(0x6858, 0x6859);
  CYC(0x6859, 0x685b); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x685b, 0x685d); A = 0x52; // ENEMYCOLLISION_ROCK
  CYC(0x685d, 0x685e); mem_wr(gb, DE, A);
  RET(0x685e); return; // ret
}

// ==================================================================================================
// ENEMY_BUSH_OR_ROCK
//
// Variables:
//   var30: Enemy ID of parent object
// ==================================================================================================
void enemyCode58_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6770, 0x6772); goto normalStatus; } // jr z
  CYC(0x6770, 0x6772);
  CYC(0x6772, 0x6774); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6774); return; } // ret c
  CYC(0x6774, 0x6775);
  if (F & FZ) { CYCT(0x6775, 0x6778); goto destroyed; } // jp z
  CYC(0x6775, 0x6778);

normalStatus:
  CYC(0x6778, 0x677a); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x677a, 0x677b); A = mem_rd(gb, DE);
  {
    CYC(0x677b, 0x677c); push_effect(gb, 0x677c);
    uint16_t target = bushOrRock_jump_table(gb);
    if (target == 0x678e) goto state_uninitialized;
    if (target == 0x67f6) goto state_stub;
    if (target == 0x67b0) goto state_grabbed;
    if (target == 0x67dc) goto state_switchHook;
    if (target == 0x67f7) goto state8;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x678e, 0x6790); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6790, 0x6791); A = mem_rd(gb, DE);
  CYC(0x6791, 0x6794); SET_HL(0x67a8); // @collisionAndTileData
  CYC(0x6794, 0x6795); bushOrRock_addDoubleIndexToHl_from_rst(gb, 0x6795);
  CYC(0x6795, 0x6797); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6797, 0x6798); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6798, 0x6799); mem_wr(gb, DE, A);
  CYC(0x6799, 0x679a); A = mem_rd(gb, HL);
  CALL_C(0x679a, objectMimicBgTile_hook, 0x233b, 0x679d);
  CALL_C(0x679d, enemyCode58_checkDisableDestruction_hook, 0x684d, 0x67a0);
  CALL_C(0x67a0, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x67a3);
  CALL_C(0x67a3, enemyCode58_copyParentPosition_hook, 0x6832, 0x67a6);
  CYC(0x67a6, 0x67a8); goto setPriorityRelativeToLink; // jr

state_grabbed:
  CYC(0x67b0, 0x67b1); E = alu_inc8(gb, E);
  CYC(0x67b1, 0x67b2); A = mem_rd(gb, DE);
  {
    CYC(0x67b2, 0x67b3); push_effect(gb, 0x67b3);
    uint16_t target = bushOrRock_jump_table(gb);
    if (target == 0x67bb) goto grabbedSubstate0;
    if (target == 0x67cc) { RET(0x67cc); return; } // ret (grabbed substate1)
    if (target == 0x67cd) goto grabbedSubstate2;
    if (target == 0x67d7) goto grabbedSubstate3;
    HANDOFF(target);
  }

grabbedSubstate0:
  CYC(0x67bb, 0x67bc); H = D;
  CYC(0x67bc, 0x67bd); L = E;
  CYC(0x67bd, 0x67be); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(0x67be, 0x67c0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x67c0, 0x67c2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x67c2, 0x67c3); alu_xor(gb, A);
  CYC(0x67c3, 0x67c6); mem_wr(gb, wLinkGrabState2, A);
  CALL_C(0x67c6, enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook, 0x6825, 0x67c9);
  CYC(0x67c9, 0x67cc); objectSetVisible81_hook(gb); return; // jp

grabbedSubstate2:
  CYC(0x67cd, 0x67ce); H = D;
  CYC(0x67ce, 0x67d0); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x67d0, 0x67d2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CYC(0x67d2, 0x67d4); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x67d4, 0x67d6); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x67d6); return; } // ret nz
  CYC(0x67d6, 0x67d7);

grabbedSubstate3:
  CALL_C(0x67d7, objectSetPriorityRelativeToLink_hook, 0x22dc, 0x67da);
  CYC(0x67da, 0x67dc); goto makeDebrisAndDelete; // jr

state_switchHook:
  CYC(0x67dc, 0x67dd); E = alu_inc8(gb, E);
  CYC(0x67dd, 0x67de); A = mem_rd(gb, DE);
  {
    CYC(0x67de, 0x67df); push_effect(gb, 0x67df);
    uint16_t target = bushOrRock_jump_table(gb);
    if (target == 0x67e7) goto switchHookSubstate0;
    if (target == 0x67ed) { RET(0x67ed); return; } // ret (switchHook substate1/substate2)
    if (target == 0x67ee) goto switchHookSubstate3;
    HANDOFF(target);
  }

switchHookSubstate0:
  CALL_C(0x67e7, enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook, 0x6825, 0x67ea);
  CYC(0x67ea, 0x67ed); ecom_incSubstate_b0e_hook(gb); return; // jp

switchHookSubstate3:
  CYC(0x67ee, 0x67f0); C = 0x20;
  CALL_C(0x67f0, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x67f3);
  if (!(F & FZ)) { RET_TAKEN(0x67f3); return; } // ret nz
  CYC(0x67f3, 0x67f4);
  CYC(0x67f4, 0x67f6); goto makeDebrisAndDelete; // jr

state_stub:
  RET(0x67f6); return; // ret

state8:
  CYC(0x67f7, 0x67f9); A = OBJ_ID; // Object.id
  CALL_C(0x67f9, objectGetRelatedObject1Var_hook, 0x2160, 0x67fc);
  CYC(0x67fc, 0x67fe); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x67fe, 0x67ff); A = mem_rd(gb, DE);
  CYC(0x67ff, 0x6800); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6800, 0x6803); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x6800, 0x6803);
  CYC(0x6803, 0x6805); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x6805, 0x6806); A = mem_rd(gb, HL);
  CYC(0x6806, 0x6807); alu_rlca(gb);
  if (F & FC) CALL_C_CC(0x6807, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x680a); else CYC(0x6807, 0x680a); // call c
  CALL_C(0x680a, enemyCode58_copyParentPosition_hook, 0x6832, 0x680d);

setPriorityRelativeToLink:
  CYC(0x680d, 0x6810); objectSetPriorityRelativeToLink_hook(gb); return; // jp

destroyed:
  CALL_C(0x6810, enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook, 0x6825, 0x6813);

makeDebrisAndDelete:
  CYC(0x6813, 0x6815); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6815, 0x6816); A = mem_rd(gb, DE);
  CYC(0x6816, 0x6819); SET_HL(0x6821); // @debrisTypes
  CYC(0x6819, 0x681a); bushOrRock_addAToHl_from_rst(gb, 0x681a);
  CYC(0x681a, 0x681b); B = mem_rd(gb, HL);
  CALL_C(0x681b, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x681e);
  CYC(0x681e, 0x6821); enemyDelete_hook(gb); return; // jp
}
