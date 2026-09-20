#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode11), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode11), (from), (to), true)

static uint16_t enemyCode11_jump_table(GB *gb) {
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

static void eyesoarChild_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void eyesoarChild_state_uninitialized_hook(GB *gb);
void eyesoarChild_state_stub_hook(GB *gb);
void eyesoarChild_state8_hook(GB *gb);
void eyesoarChild_state9_hook(GB *gb);
void eyesoarChild_stateA_hook(GB *gb);
void eyesoarChild_updatePosition_hook(GB *gb);
void eyesoarChild_stateB_hook(GB *gb);
void eyesoarChild_incOrDecHL_hook(GB *gb);
void eyesoarChild_stateC_hook(GB *gb);
void eyesoarChild_stateD_hook(GB *gb);
void eyesoarChild_stateE_hook(GB *gb);
void eyesoarChild_stateF_hook(GB *gb);
void eyesoarChild_animate_hook(GB *gb);
void eyesoarChild_state10_hook(GB *gb);

// ==================================================================================================
// ENEMY_EYESOAR_CHILD
//
// Variables:
//   relatedObj1: Pointer to ENEMY_EYESOAR
//   relatedObj2: Pointer to INTERAC_0b?
//   var30: Distance away from Eyesoar (position in "circle arc")
//   var31: "Target" distance away from Eyesoar (var30 is moving toward this value)
//   var32: Angle offset for this child (each subid is a quarter circle apart)
//
// See also ENEMY_EYESOAR variables.
// ==================================================================================================
void enemyCode11_hook(GB *gb) {
  BASE(enemyCode11);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = OBJ_HEALTH; // Object.health
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+17); enemyDie_uncounted_hook(gb); return; } // jp z
  CYC(b_+14, b_+17);
  CALL_C(b_+17, objectCreatePuff_hook, SYM(objectCreatePuff), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x0c);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 30);
  CYC(b_+29, b_+31); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x00);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+35, b_+37); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x04);
  CALL_C(b_+41, objectSetInvisible_hook, SYM(objectSetInvisible), b_+44);

normalStatus:
  CYC(b_+44, b_+46); A = OBJ_VAR39; // Object.var39
  CALL_C(b_+46, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+49);
  CYC(b_+49, b_+51); alu_bit(gb, 1, mem_rd(gb, HL));
  CYC(b_+51, b_+52); B = H;
  CYC(b_+52, b_+54); E = ENEMY_BASE + OBJ_STATE;
  if (F & FZ) { CYCT(b_+54, b_+56); goto runState; } // jr z
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_cp(gb, 0x0f);
  if (!(F & FC)) { CYCT(b_+59, b_+61); goto runState; } // jr nc
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); alu_cp(gb, 0x0c);
  CYC(b_+63, b_+64); H = D;
  if (F & FZ) { CYCT(b_+64, b_+66); goto afterState15Setup; } // jr z
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+67); L = E;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x0f); // [state]
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 240);

afterState15Setup:
  CYC(b_+73, b_+75); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+75, b_+77); mem_wr(gb, HL, 24);

runState:
  // Note: b == parent (ENEMY_EYESOAR), used in some of the states below.
  CYC(b_+77, b_+78); A = mem_rd(gb, DE);
  CYC(b_+78, b_+79); push_effect(gb, b_+79);
  {
    uint16_t target = enemyCode11_jump_table(gb);
    if (target == SYM(eyesoarChild_state8)) { eyesoarChild_state8_hook(gb); return; }
    if (target == SYM(eyesoarChild_state9)) { eyesoarChild_state9_hook(gb); return; }
    if (target == SYM(eyesoarChild_stateA)) { eyesoarChild_stateA_hook(gb); return; }
    if (target == SYM(eyesoarChild_stateB)) { eyesoarChild_stateB_hook(gb); return; }
    if (target == SYM(eyesoarChild_stateC)) { eyesoarChild_stateC_hook(gb); return; }
    if (target == SYM(eyesoarChild_stateD)) { eyesoarChild_stateD_hook(gb); return; }
    if (target == SYM(eyesoarChild_stateE)) { eyesoarChild_stateE_hook(gb); return; }
    if (target == SYM(eyesoarChild_stateF)) { eyesoarChild_stateF_hook(gb); return; }
    if (target == SYM(eyesoarChild_state10)) { eyesoarChild_state10_hook(gb); return; }
    if (target == SYM(eyesoarChild_state_uninitialized)) { eyesoarChild_state_uninitialized_hook(gb); return; }
    eyesoarChild_state_stub_hook(gb); return; // states 1-7 all target 0x69c0
  }
}

void eyesoarChild_state_uninitialized_hook(GB *gb) {
  BASE(eyesoarChild_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_YH; // Object.yh
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+15); SET_HL(b_+38); // @initialAnglesForSubids
  CYC(b_+15, b_+16); eyesoarChild_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); A = 0x18;
  CALL_C(b_+25, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); A = 90;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+35); A = 0x28; // SPEED_100
  CYC(b_+35, b_+38); ecom_setSpeedAndState8_b0d_hook(gb); return; // jp
}

// eyesoarChild_state_uninitialized@initialAnglesForSubids (0d:69bc-69bf): pure data
// (ANGLE_UP/RIGHT/DOWN/LEFT), read only through the addAToHl indexing above.

void eyesoarChild_state_stub_hook(GB *gb) {
  BASE(eyesoarChild_state_stub);
  RET(b_+0); return;
}

// Wait for [counter1] frames before becoming visible
void eyesoarChild_state8_hook(GB *gb) {
  BASE(eyesoarChild_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); SET_BC((SYM(vblankDmaFunction) + 23)); // INTERAC_0b, $02
  CALL_C(b_+7, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+13, b_+15); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); A = H;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, SYM(eyesoarChild_state9)); ecom_incState_b0d_hook(gb); return; // jp
}

void eyesoarChild_state9_hook(GB *gb) {
  BASE(eyesoarChild_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ANIM_PARAMETER; // Object.animParameter
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+7, b_+8); ret_effect(gb); return; } // ret z
  CYC(b_+7, b_+8);
  CALL_C(b_+8, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 240);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xfe);
  CYC(b_+19, b_+21); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x18);
  CYC(b_+23, SYM(eyesoarChild_stateA)); objectSetVisiblec2_hook(gb); return; // jp
}

// Moving around Eyesoar in a circle
void eyesoarChild_stateA_hook(GB *gb) {
  BASE(eyesoarChild_stateA);
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(b_+3, b_+5); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); eyesoarChild_updatePosition_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x38; // Enemy.var38 (parent)
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_and(gb, 0xf8);
  CYC(b_+12, b_+14); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+19); A = 0x0b;
  CYC(b_+19, SYM(eyesoarChild_updatePosition)); mem_wr(gb, DE, A);
  eyesoarChild_updatePosition_hook(gb); return; // falls through
}

void eyesoarChild_updatePosition_hook(GB *gb) {
  BASE(eyesoarChild_updatePosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); B = mem_rd(gb, HL);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+5, b_+6); C = mem_rd(gb, HL);
  // [this.var32] += [parent.var3b] (update angle by rotation speed)
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x3b; // Enemy.var3b (parent, h==b)
  CYC(b_+8, b_+10); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); alu_and(gb, 0x1f);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CALL_C(b_+21, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+24);
  CYC(b_+24, SYM(eyesoarChild_stateB)); enemyAnimate_hook(gb); return; // jp
}

void eyesoarChild_stateB_hook(GB *gb) {
  BASE(eyesoarChild_stateB);
  uint16_t sp0_ = gb->sp;
  // Check if we're the correct distance away
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); alu_cp(gb, mem_rd(gb, HL)); // [var30]
  if (!(F & FZ)) { CYCT(b_+5, b_+7); eyesoarChild_incOrDecHL_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  // Mark flag in parent indicating we're in position
  CYC(b_+9, b_+10); H = B;
  CYC(b_+10, b_+12); L = ENEMY_BASE + 0x3a; // Enemy.var3a
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CALL_C(b_+15, setFlag_hook, SYM(setFlag), b_+18);
  CYC(b_+18, SYM(eyesoarChild_incOrDecHL)); eyesoarChild_updatePosition_hook(gb); return; // jr
}

void eyesoarChild_incOrDecHL_hook(GB *gb) {
  BASE(eyesoarChild_incOrDecHL);
  CYC(b_+0, b_+2); A = 0x01;
  if (!(F & FC)) { CYCT(b_+2, b_+4); goto addAndStore; } // jr nc
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); A = 0xff;

addAndStore:
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+9); H = B;
  CYC(b_+9, SYM(eyesoarChild_stateC)); eyesoarChild_updatePosition_hook(gb); return; // jr
}

// Was just "killed"; waiting a bit before reappearing
void eyesoarChild_stateC_hook(GB *gb) {
  BASE(eyesoarChild_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto stillInvisible; } // jr nz
  CYC(b_+5, b_+7);
  CALL_C(b_+7, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto stillInvisible; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(b_+18, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+21);
  CYC(b_+21, b_+22); H = B;
  CYC(b_+22, b_+24); eyesoarChild_updatePosition_hook(gb); return; // jr

stillInvisible:
  CYC(b_+24, b_+25); H = B;
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+31); SET_BC(b_+40); // @data
  CALL_C(b_+31, addAToBc_hook, 0x006d, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, BC);
  CYC(b_+35, b_+37); L = ENEMY_BASE + 0x3a; // Enemy.var3a (parent, h==b)
  CYC(b_+37, b_+38); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  RET(b_+39); return;
}

// eyesoarChild_stateC@data (0d:6a67-6a6a): pure data ($11 $22 $44 $88), read only through
// addAToBc above.

// Just reappeared
void eyesoarChild_stateD_hook(GB *gb) {
  BASE(eyesoarChild_stateD);
  // Update position relative to eyesoar
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x38; // Enemy.var38 (parent, h==b)
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0xf8);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+10, b_+12); eyesoarChild_incOrDecHL_hook(gb); return; } // jr nz
  CYC(b_+10, b_+12);
  // Reached desired position, go back to state $0a
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0a);
  CYC(b_+16, b_+17); H = B;
  CYC(b_+17, SYM(eyesoarChild_stateE)); eyesoarChild_updatePosition_hook(gb); return; // jp
}

void eyesoarChild_stateE_hook(GB *gb) {
  BASE(eyesoarChild_stateE);
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(b_+3, b_+5); alu_bit(gb, 4, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+8); eyesoarChild_updatePosition_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); A = 0x0b;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state]
  CYC(b_+11, SYM(eyesoarChild_stateF)); eyesoarChild_updatePosition_hook(gb); return; // jp
}

// Moving around randomly
void eyesoarChild_stateF_hook(GB *gb) {
  BASE(eyesoarChild_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(b_+3, b_+5); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto stillMovingRandomly; } // jr nz
  CYC(b_+5, b_+7);
  // Calculate the angle relative to Eyesoar it should move to
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x3b; // Enemy.var3b (parent, h==b)
  CYC(b_+9, b_+11); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+15); alu_and(gb, 0x1f);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CALL_C(b_+18, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+21);
  // $18 units away from Eyesoar
  CYC(b_+21, b_+23); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x18);
  CYC(b_+25, b_+27); eyesoarChild_animate_hook(gb); return; // jr

stillMovingRandomly:
  CYC(b_+27, b_+30); A = W8(wFrameCounter);
  CYC(b_+30, b_+32); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto applySpeed; } // jr nz
  CYC(b_+32, b_+34);
  CALL_C(b_+34, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+37);
  CALL_C(b_+37, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+40);

applySpeed:
  CALL_C(b_+40, objectApplySpeed_hook, SYM(objectApplySpeed), b_+43);
  CALL_C(b_+43, ecom_bounceOffScreenBoundary_b0d_hook, SYM(ecom_bounceOffScreenBoundary_b0d), SYM(eyesoarChild_animate));
  eyesoarChild_animate_hook(gb); return; // falls through
}

void eyesoarChild_animate_hook(GB *gb) {
  BASE(eyesoarChild_animate);
  CYC(b_+0, SYM(eyesoarChild_state10)); enemyAnimate_hook(gb); return; // jp
}

// Moving back toward Eyesoar
void eyesoarChild_state10_hook(GB *gb) {
  BASE(eyesoarChild_state10);
  uint16_t sp0_ = gb->sp;
  // Load into wTmpcec0 the position offset relative to Eyesoar where we should be moving to
  CYC(b_+0, b_+1); H = B;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x3b; // Enemy.var3b (parent, h==b)
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+10); alu_and(gb, 0x1f);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+13); A = 0x18;
  CYC(b_+13, b_+15); B = 0x28; // SPEED_100
  CALL_C(b_+15, getScaledPositionOffsetForVelocity_hook, SYM(getScaledPositionOffsetForVelocity), b_+18);
  // Get parent.position + offset in bc
  CYC(b_+18, b_+20); A = OBJ_YH; // Object.yh
  CALL_C(b_+20, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wTmpcec0 + 1);
  CYC(b_+26, b_+27); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+30, b_+33); A = mem_rd(gb, wTmpcec0 + 3);
  CYC(b_+33, b_+34); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+34, b_+35); C = A;
  // Store current position
  CYC(b_+35, b_+36); E = L;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); hram_wr(gb, 0x8e, A);
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+44); hram_wr(gb, 0x8f, A);
  // Check if we've reached the target position
  CYC(b_+44, b_+45); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto moveToward; } // jr nz
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); A = hram_rd(gb, 0x8e);
  CYC(b_+49, b_+50); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+50, b_+52); goto reachedTargetPosition; } // jr z
  CYC(b_+50, b_+52);

moveToward:
  CALL_C(b_+52, ecom_moveTowardPosition_b0d_hook, SYM(ecom_moveTowardPosition_b0d), b_+55);
  CYC(b_+55, b_+57); eyesoarChild_animate_hook(gb); return; // jr

reachedTargetPosition:
  // Wait for signal to change state
  CYC(b_+57, b_+59); L = ENEMY_BASE + 0x39; // Enemy.var39
  CYC(b_+59, b_+61); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+61, b_+62); ret_effect(gb); return; } // ret nz
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+64, b_+66); A = 0x0e;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  // Set flag in parent's var3a indicating we're good to go?
  CYC(b_+67, b_+69); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+72); alu_add(gb, 0x04);
  CYC(b_+72, b_+74); L = ENEMY_BASE + 0x3a; // Enemy.var3a (parent, h==b)
  CYC(b_+74, SYM(enemyCode1c)); setFlag_hook(gb); return; // jp
}
