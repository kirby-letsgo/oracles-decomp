#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode58), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode58), (from), (to), true)

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
  BASE(enemyCode58);
  uint16_t sp0_ = gb->sp;
  CYC(b_+181, b_+183); A = OBJ_VISIBLE; // Object.visible
  CALL_C(b_+183, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+186);
  CYC(b_+186, b_+188); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+188, b_+190); L = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+190, b_+191); alu_xor(gb, A);
  CYC(b_+191, b_+192); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+192, b_+193); mem_wr(gb, HL, A);
  RET(b_+193); return; // ret
}

// 0e:6832, bare local (no exported symbol); called via genuine call/ret from two states of
// enemyCode58. Copies parent position, with a Z offset determined by parent.var03.
void enemyCode58_copyParentPosition_hook(GB *gb) {
  BASE(enemyCode58);
  uint16_t sp0_ = gb->sp;
  CYC(b_+194, b_+196); A = OBJ_YH; // Object.yh
  CALL_C(b_+196, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+199);
  CALL_C(b_+199, objectTakePosition_hook, SYM(objectTakePosition), b_+202);
  CYC(b_+202, b_+204); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+204, b_+205); A = mem_rd(gb, HL);
  CYC(b_+205, b_+207); alu_and(gb, 0x03);
  CYC(b_+207, b_+210); SET_HL(b_+217); // @zVals
  CYC(b_+210, b_+211); bushOrRock_addAToHl_from_rst(gb, b_+211);
  CYC(b_+211, b_+213); E = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+213, b_+214); A = mem_rd(gb, DE);
  CYC(b_+214, b_+215); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+215, b_+216); mem_wr(gb, DE, A);
  RET(b_+216); return; // ret
}

// 0e:684d, bare local (no exported symbol); called via genuine call/ret from
// enemyCode58@state_uninitialized. Disable bush destruction for deku scrubs only.
void enemyCode58_checkDisableDestruction_hook(GB *gb) {
  BASE(enemyCode58);
  uint16_t sp0_ = gb->sp;
  CYC(b_+221, b_+223); A = OBJ_ID; // Object.id
  CALL_C(b_+223, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+226);
  CYC(b_+226, b_+228); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+228, b_+229); A = mem_rd(gb, HL);
  CYC(b_+229, b_+230); mem_wr(gb, DE, A);
  CYC(b_+230, b_+232); alu_cp(gb, 0x27); // ENEMY_DEKU_SCRUB
  if (!(F & FZ)) { RET_TAKEN(b_+232); return; } // ret nz
  CYC(b_+232, b_+233);
  CYC(b_+233, b_+235); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+235, b_+237); A = 0x52; // ENEMYCOLLISION_ROCK
  CYC(b_+237, b_+238); mem_wr(gb, DE, A);
  RET(b_+238); return; // ret
}

// ==================================================================================================
// ENEMY_BUSH_OR_ROCK
//
// Variables:
//   var30: Enemy ID of parent object
// ==================================================================================================
void enemyCode58_hook(GB *gb) {
  BASE(enemyCode58);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); goto destroyed; } // jp z
  CYC(b_+5, b_+8);

normalStatus:
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  {
    CYC(b_+11, b_+12); push_effect(gb, b_+12);
    uint16_t target = bushOrRock_jump_table(gb);
    if (target == b_+30) goto state_uninitialized;
    if (target == b_+134) goto state_stub;
    if (target == b_+64) goto state_grabbed;
    if (target == b_+108) goto state_switchHook;
    if (target == b_+135) goto state8;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+36); SET_HL(b_+56); // @collisionAndTileData
  CYC(b_+36, b_+37); bushOrRock_addDoubleIndexToHl_from_rst(gb, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CALL_C(b_+42, objectMimicBgTile_hook, SYM(objectMimicBgTile), b_+45);
  CALL_C(b_+45, enemyCode58_checkDisableDestruction_hook, b_+221, b_+48);
  CALL_C(b_+48, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+51);
  CALL_C(b_+51, enemyCode58_copyParentPosition_hook, b_+194, b_+54);
  CYC(b_+54, b_+56); goto setPriorityRelativeToLink; // jr

state_grabbed:
  CYC(b_+64, b_+65); E = alu_inc8(gb, E);
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  {
    CYC(b_+66, b_+67); push_effect(gb, b_+67);
    uint16_t target = bushOrRock_jump_table(gb);
    if (target == b_+75) goto grabbedSubstate0;
    if (target == b_+92) { RET(b_+92); return; } // ret (grabbed substate1)
    if (target == b_+93) goto grabbedSubstate2;
    if (target == b_+103) goto grabbedSubstate3;
    HANDOFF(target);
  }

grabbedSubstate0:
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+77); L = E;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+80, b_+82); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+82, b_+83); alu_xor(gb, A);
  CYC(b_+83, b_+86); mem_wr(gb, wLinkGrabState2, A);
  CALL_C(b_+86, enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook, b_+181, b_+89);
  CYC(b_+89, b_+92); objectSetVisible81_hook(gb); return; // jp

grabbedSubstate2:
  CYC(b_+93, b_+94); H = D;
  CYC(b_+94, b_+96); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+96, b_+98); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CYC(b_+98, b_+100); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+100, b_+102); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; } // ret nz
  CYC(b_+102, b_+103);

grabbedSubstate3:
  CALL_C(b_+103, objectSetPriorityRelativeToLink_hook, SYM(objectSetPriorityRelativeToLink), b_+106);
  CYC(b_+106, b_+108); goto makeDebrisAndDelete; // jr

state_switchHook:
  CYC(b_+108, b_+109); E = alu_inc8(gb, E);
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  {
    CYC(b_+110, b_+111); push_effect(gb, b_+111);
    uint16_t target = bushOrRock_jump_table(gb);
    if (target == b_+119) goto switchHookSubstate0;
    if (target == b_+125) { RET(b_+125); return; } // ret (switchHook substate1/substate2)
    if (target == b_+126) goto switchHookSubstate3;
    HANDOFF(target);
  }

switchHookSubstate0:
  CALL_C(b_+119, enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook, b_+181, b_+122);
  CYC(b_+122, b_+125); ecom_incSubstate_b0e_hook(gb); return; // jp

switchHookSubstate3:
  CYC(b_+126, b_+128); C = 0x20;
  CALL_C(b_+128, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+131);
  if (!(F & FZ)) { RET_TAKEN(b_+131); return; } // ret nz
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+134); goto makeDebrisAndDelete; // jr

state_stub:
  RET(b_+134); return; // ret

state8:
  CYC(b_+135, b_+137); A = OBJ_ID; // Object.id
  CALL_C(b_+137, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+140);
  CYC(b_+140, b_+142); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+144); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+144, b_+147); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+144, b_+147);
  CYC(b_+147, b_+149); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+149, b_+150); A = mem_rd(gb, HL);
  CYC(b_+150, b_+151); alu_rlca(gb);
  if (F & FC) CALL_C_CC(b_+151, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+154); else CYC(b_+151, b_+154); // call c
  CALL_C(b_+154, enemyCode58_copyParentPosition_hook, b_+194, b_+157);

setPriorityRelativeToLink:
  CYC(b_+157, b_+160); objectSetPriorityRelativeToLink_hook(gb); return; // jp

destroyed:
  CALL_C(b_+160, enemyCode58_makeParentEnemyVisibleAndRemoveReference_hook, b_+181, b_+163);

makeDebrisAndDelete:
  CYC(b_+163, b_+165); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+165, b_+166); A = mem_rd(gb, DE);
  CYC(b_+166, b_+169); SET_HL(b_+177); // @debrisTypes
  CYC(b_+169, b_+170); bushOrRock_addAToHl_from_rst(gb, b_+170);
  CYC(b_+170, b_+171); B = mem_rd(gb, HL);
  CALL_C(b_+171, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+174);
  CYC(b_+174, b_+177); enemyDelete_hook(gb); return; // jp
}
