#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6925, 0x6927); goto normalStatus; } // jr z
  CYC(0x6925, 0x6927);
  CYC(0x6927, 0x6929); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x6929, 0x692a); ret_effect(gb); return; } // ret c
  CYC(0x6929, 0x692a);
  if (!(F & FZ)) { CYCT(0x692a, 0x692c); goto normalStatus; } // jr nz
  CYC(0x692a, 0x692c);
  CYC(0x692c, 0x692e); A = OBJ_HEALTH; // Object.health
  CALL_C(0x692e, objectGetRelatedObject1Var_hook, 0x2160, 0x6931);
  CYC(0x6931, 0x6932); A = mem_rd(gb, HL);
  CYC(0x6932, 0x6933); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6933, 0x6936); enemyDie_uncounted_hook(gb); return; } // jp z
  CYC(0x6933, 0x6936);
  CALL_C(0x6936, objectCreatePuff_hook, 0x24c1, 0x6939);
  CYC(0x6939, 0x693a); H = D;
  CYC(0x693a, 0x693c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x693c, 0x693e); mem_wr(gb, HL, 0x0c);
  CYC(0x693e, 0x6940); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6940, 0x6942); mem_wr(gb, HL, 30);
  CYC(0x6942, 0x6944); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6944, 0x6946); mem_wr(gb, HL, 0x00);
  CYC(0x6946, 0x6948); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6948, 0x694a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x694a, 0x694c); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x694c, 0x694e); mem_wr(gb, HL, 0x04);
  CALL_C(0x694e, objectSetInvisible_hook, 0x1e7b, 0x6951);

normalStatus:
  CYC(0x6951, 0x6953); A = OBJ_VAR39; // Object.var39
  CALL_C(0x6953, objectGetRelatedObject1Var_hook, 0x2160, 0x6956);
  CYC(0x6956, 0x6958); alu_bit(gb, 1, mem_rd(gb, HL));
  CYC(0x6958, 0x6959); B = H;
  CYC(0x6959, 0x695b); E = ENEMY_BASE + OBJ_STATE;
  if (F & FZ) { CYCT(0x695b, 0x695d); goto runState; } // jr z
  CYC(0x695b, 0x695d);
  CYC(0x695d, 0x695e); A = mem_rd(gb, DE);
  CYC(0x695e, 0x6960); alu_cp(gb, 0x0f);
  if (!(F & FC)) { CYCT(0x6960, 0x6962); goto runState; } // jr nc
  CYC(0x6960, 0x6962);
  CYC(0x6962, 0x6964); alu_cp(gb, 0x0c);
  CYC(0x6964, 0x6965); H = D;
  if (F & FZ) { CYCT(0x6965, 0x6967); goto afterState15Setup; } // jr z
  CYC(0x6965, 0x6967);
  CYC(0x6967, 0x6968); L = E;
  CYC(0x6968, 0x696a); mem_wr(gb, HL, 0x0f); // [state]
  CYC(0x696a, 0x696c); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x696c, 0x696e); mem_wr(gb, HL, 240);

afterState15Setup:
  CYC(0x696e, 0x6970); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6970, 0x6972); mem_wr(gb, HL, 24);

runState:
  // Note: b == parent (ENEMY_EYESOAR), used in some of the states below.
  CYC(0x6972, 0x6973); A = mem_rd(gb, DE);
  CYC(0x6973, 0x6974); push_effect(gb, 0x6974);
  {
    uint16_t target = enemyCode11_jump_table(gb);
    if (target == 0x69c1) { eyesoarChild_state8_hook(gb); return; }
    if (target == 0x69d7) { eyesoarChild_state9_hook(gb); return; }
    if (target == 0x69f1) { eyesoarChild_stateA_hook(gb); return; }
    if (target == 0x6a20) { eyesoarChild_stateB_hook(gb); return; }
    if (target == 0x6a3f) { eyesoarChild_stateC_hook(gb); return; }
    if (target == 0x6a6b) { eyesoarChild_stateD_hook(gb); return; }
    if (target == 0x6a7f) { eyesoarChild_stateE_hook(gb); return; }
    if (target == 0x6a8d) { eyesoarChild_stateF_hook(gb); return; }
    if (target == 0x6abe) { eyesoarChild_state10_hook(gb); return; }
    if (target == 0x6996) { eyesoarChild_state_uninitialized_hook(gb); return; }
    eyesoarChild_state_stub_hook(gb); return; // states 1-7 all target 0x69c0
  }
}

void eyesoarChild_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6996, 0x6998); A = OBJ_YH; // Object.yh
  CALL_C(0x6998, objectGetRelatedObject1Var_hook, 0x2160, 0x699b);
  CYC(0x699b, 0x699c); B = mem_rd(gb, HL);
  CYC(0x699c, 0x699e); L = ENEMY_BASE + OBJ_XH;
  CYC(0x699e, 0x699f); C = mem_rd(gb, HL);
  CYC(0x699f, 0x69a1); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x69a1, 0x69a2); A = mem_rd(gb, DE);
  CYC(0x69a2, 0x69a5); SET_HL(0x69bc); // @initialAnglesForSubids
  CYC(0x69a5, 0x69a6); eyesoarChild_addAToHl_from_rst(gb, 0x69a6);
  CYC(0x69a6, 0x69a8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x69a8, 0x69a9); A = mem_rd(gb, HL);
  CYC(0x69a9, 0x69aa); mem_wr(gb, DE, A);
  CYC(0x69aa, 0x69ac); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x69ac, 0x69ad); mem_wr(gb, DE, A);
  CYC(0x69ad, 0x69af); A = 0x18;
  CALL_C(0x69af, objectSetPositionInCircleArc_hook, 0x210e, 0x69b2);
  CYC(0x69b2, 0x69b4); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x69b4, 0x69b6); A = 90;
  CYC(0x69b6, 0x69b7); mem_wr(gb, DE, A);
  CYC(0x69b7, 0x69b9); A = 0x28; // SPEED_100
  CYC(0x69b9, 0x69bc); ecom_setSpeedAndState8_b0d_hook(gb); return; // jp
}

// eyesoarChild_state_uninitialized@initialAnglesForSubids (0d:69bc-69bf): pure data
// (ANGLE_UP/RIGHT/DOWN/LEFT), read only through the addAToHl indexing above.

void eyesoarChild_state_stub_hook(GB *gb) {
  RET(0x69c0); return;
}

// Wait for [counter1] frames before becoming visible
void eyesoarChild_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x69c1, ecom_decCounter1_b0d_hook, 0x439a, 0x69c4);
  if (!(F & FZ)) { CYCT(0x69c4, 0x69c5); ret_effect(gb); return; } // ret nz
  CYC(0x69c4, 0x69c5);
  CYC(0x69c5, 0x69c8); SET_BC(0x0b02); // INTERAC_0b, $02
  CALL_C(0x69c8, objectCreateInteraction_hook, 0x24c5, 0x69cb);
  if (!(F & FZ)) { CYCT(0x69cb, 0x69cc); ret_effect(gb); return; } // ret nz
  CYC(0x69cb, 0x69cc);
  CYC(0x69cc, 0x69ce); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x69ce, 0x69d0); A = INTERACTION_BASE; // Interaction.start
  CYC(0x69d0, 0x69d1); mem_wr(gb, DE, A);
  CYC(0x69d1, 0x69d2); E = alu_inc8(gb, E);
  CYC(0x69d2, 0x69d3); A = H;
  CYC(0x69d3, 0x69d4); mem_wr(gb, DE, A);
  CYC(0x69d4, 0x69d7); ecom_incState_b0d_hook(gb); return; // jp
}

void eyesoarChild_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x69d7, 0x69d9); A = OBJ_ANIM_PARAMETER; // Object.animParameter
  CALL_C(0x69d9, objectGetRelatedObject2Var_hook, 0x2164, 0x69dc);
  CYC(0x69dc, 0x69de); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x69de, 0x69df); ret_effect(gb); return; } // ret z
  CYC(0x69de, 0x69df);
  CALL_C(0x69df, ecom_incState_b0d_hook, 0x4000, 0x69e2);
  CYC(0x69e2, 0x69e4); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x69e4, 0x69e6); mem_wr(gb, HL, 240);
  CYC(0x69e6, 0x69e8); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x69e8, 0x69ea); mem_wr(gb, HL, 0xfe);
  CYC(0x69ea, 0x69ec); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x69ec, 0x69ee); mem_wr(gb, HL, 0x18);
  CYC(0x69ee, 0x69f1); objectSetVisiblec2_hook(gb); return; // jp
}

// Moving around Eyesoar in a circle
void eyesoarChild_stateA_hook(GB *gb) {
  CYC(0x69f1, 0x69f2); H = B;
  CYC(0x69f2, 0x69f4); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(0x69f4, 0x69f6); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x69f6, 0x69f8); eyesoarChild_updatePosition_hook(gb); return; } // jr z
  CYC(0x69f6, 0x69f8);
  CYC(0x69f8, 0x69fa); L = ENEMY_BASE + 0x38; // Enemy.var38 (parent)
  CYC(0x69fa, 0x69fb); A = mem_rd(gb, HL);
  CYC(0x69fb, 0x69fd); alu_and(gb, 0xf8);
  CYC(0x69fd, 0x69ff); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x69ff, 0x6a00); mem_wr(gb, DE, A);
  CYC(0x6a00, 0x6a02); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6a02, 0x6a04); A = 0x0b;
  CYC(0x6a04, 0x6a05); mem_wr(gb, DE, A);
  eyesoarChild_updatePosition_hook(gb); return; // falls through
}

void eyesoarChild_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a05, 0x6a07); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6a07, 0x6a08); B = mem_rd(gb, HL);
  CYC(0x6a08, 0x6a0a); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6a0a, 0x6a0b); C = mem_rd(gb, HL);
  // [this.var32] += [parent.var3b] (update angle by rotation speed)
  CYC(0x6a0b, 0x6a0d); L = ENEMY_BASE + 0x3b; // Enemy.var3b (parent, h==b)
  CYC(0x6a0d, 0x6a0f); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6a0f, 0x6a10); A = mem_rd(gb, DE);
  CYC(0x6a10, 0x6a11); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6a11, 0x6a13); alu_and(gb, 0x1f);
  CYC(0x6a13, 0x6a15); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6a15, 0x6a16); mem_wr(gb, DE, A);
  CYC(0x6a16, 0x6a17); H = D;
  CYC(0x6a17, 0x6a19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6a19, 0x6a1a); A = mem_rd(gb, HL);
  CALL_C(0x6a1a, objectSetPositionInCircleArc_hook, 0x210e, 0x6a1d);
  CYC(0x6a1d, 0x6a20); enemyAnimate_hook(gb); return; // jp
}

void eyesoarChild_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Check if we're the correct distance away
  CYC(0x6a20, 0x6a21); H = D;
  CYC(0x6a21, 0x6a23); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6a23, 0x6a24); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6a24, 0x6a25); alu_cp(gb, mem_rd(gb, HL)); // [var30]
  if (!(F & FZ)) { CYCT(0x6a25, 0x6a27); eyesoarChild_incOrDecHL_hook(gb); return; } // jr nz
  CYC(0x6a25, 0x6a27);
  CYC(0x6a27, 0x6a28); L = E;
  CYC(0x6a28, 0x6a29); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  // Mark flag in parent indicating we're in position
  CYC(0x6a29, 0x6a2a); H = B;
  CYC(0x6a2a, 0x6a2c); L = ENEMY_BASE + 0x3a; // Enemy.var3a
  CYC(0x6a2c, 0x6a2e); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a2e, 0x6a2f); A = mem_rd(gb, DE);
  CALL_C(0x6a2f, setFlag_hook, 0x020e, 0x6a32);
  CYC(0x6a32, 0x6a34); eyesoarChild_updatePosition_hook(gb); return; // jr
}

void eyesoarChild_incOrDecHL_hook(GB *gb) {
  CYC(0x6a34, 0x6a36); A = 0x01;
  if (!(F & FC)) { CYCT(0x6a36, 0x6a38); goto addAndStore; } // jr nc
  CYC(0x6a36, 0x6a38);
  CYC(0x6a38, 0x6a3a); A = 0xff;

addAndStore:
  CYC(0x6a3a, 0x6a3b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6a3b, 0x6a3c); mem_wr(gb, HL, A);
  CYC(0x6a3c, 0x6a3d); H = B;
  CYC(0x6a3d, 0x6a3f); eyesoarChild_updatePosition_hook(gb); return; // jr
}

// Was just "killed"; waiting a bit before reappearing
void eyesoarChild_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a3f, 0x6a40); H = B;
  CYC(0x6a40, 0x6a42); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(0x6a42, 0x6a44); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6a44, 0x6a46); goto stillInvisible; } // jr nz
  CYC(0x6a44, 0x6a46);
  CALL_C(0x6a46, ecom_decCounter1_b0d_hook, 0x439a, 0x6a49);
  if (!(F & FZ)) { CYCT(0x6a49, 0x6a4b); goto stillInvisible; } // jr nz
  CYC(0x6a49, 0x6a4b);
  CYC(0x6a4b, 0x6a4c); L = E;
  CYC(0x6a4c, 0x6a4d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6a4d, 0x6a4f); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6a4f, 0x6a51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(0x6a51, objectSetVisiblec2_hook, 0x1e45, 0x6a54);
  CYC(0x6a54, 0x6a55); H = B;
  CYC(0x6a55, 0x6a57); eyesoarChild_updatePosition_hook(gb); return; // jr

stillInvisible:
  CYC(0x6a57, 0x6a58); H = B;
  CYC(0x6a58, 0x6a5a); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a5a, 0x6a5b); A = mem_rd(gb, DE);
  CYC(0x6a5b, 0x6a5e); SET_BC(0x6a67); // @data
  CALL_C(0x6a5e, addAToBc_hook, 0x006d, 0x6a61);
  CYC(0x6a61, 0x6a62); A = mem_rd(gb, BC);
  CYC(0x6a62, 0x6a64); L = ENEMY_BASE + 0x3a; // Enemy.var3a (parent, h==b)
  CYC(0x6a64, 0x6a65); alu_or(gb, mem_rd(gb, HL));
  CYC(0x6a65, 0x6a66); mem_wr(gb, HL, A);
  RET(0x6a66); return;
}

// eyesoarChild_stateC@data (0d:6a67-6a6a): pure data ($11 $22 $44 $88), read only through
// addAToBc above.

// Just reappeared
void eyesoarChild_stateD_hook(GB *gb) {
  // Update position relative to eyesoar
  CYC(0x6a6b, 0x6a6c); H = B;
  CYC(0x6a6c, 0x6a6e); L = ENEMY_BASE + 0x38; // Enemy.var38 (parent, h==b)
  CYC(0x6a6e, 0x6a6f); A = mem_rd(gb, HL);
  CYC(0x6a6f, 0x6a71); alu_and(gb, 0xf8);
  CYC(0x6a71, 0x6a72); H = D;
  CYC(0x6a72, 0x6a74); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6a74, 0x6a75); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6a75, 0x6a77); eyesoarChild_incOrDecHL_hook(gb); return; } // jr nz
  CYC(0x6a75, 0x6a77);
  // Reached desired position, go back to state $0a
  CYC(0x6a77, 0x6a79); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6a79, 0x6a7b); mem_wr(gb, HL, 0x0a);
  CYC(0x6a7b, 0x6a7c); H = B;
  CYC(0x6a7c, 0x6a7f); eyesoarChild_updatePosition_hook(gb); return; // jp
}

void eyesoarChild_stateE_hook(GB *gb) {
  CYC(0x6a7f, 0x6a80); H = B;
  CYC(0x6a80, 0x6a82); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(0x6a82, 0x6a84); alu_bit(gb, 4, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6a84, 0x6a87); eyesoarChild_updatePosition_hook(gb); return; } // jp nz
  CYC(0x6a84, 0x6a87);
  CYC(0x6a87, 0x6a89); A = 0x0b;
  CYC(0x6a89, 0x6a8a); mem_wr(gb, DE, A); // [state]
  CYC(0x6a8a, 0x6a8d); eyesoarChild_updatePosition_hook(gb); return; // jp
}

// Moving around randomly
void eyesoarChild_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a8d, 0x6a8e); H = B;
  CYC(0x6a8e, 0x6a90); L = ENEMY_BASE + 0x39; // Enemy.var39 (parent, h==b)
  CYC(0x6a90, 0x6a92); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6a92, 0x6a94); goto stillMovingRandomly; } // jr nz
  CYC(0x6a92, 0x6a94);
  // Calculate the angle relative to Eyesoar it should move to
  CYC(0x6a94, 0x6a96); L = ENEMY_BASE + 0x3b; // Enemy.var3b (parent, h==b)
  CYC(0x6a96, 0x6a98); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6a98, 0x6a99); A = mem_rd(gb, DE);
  CYC(0x6a99, 0x6a9a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6a9a, 0x6a9c); alu_and(gb, 0x1f);
  CYC(0x6a9c, 0x6a9e); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6a9e, 0x6a9f); mem_wr(gb, DE, A);
  CALL_C(0x6a9f, ecom_incState_b0d_hook, 0x4000, 0x6aa2);
  // $18 units away from Eyesoar
  CYC(0x6aa2, 0x6aa4); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6aa4, 0x6aa6); mem_wr(gb, HL, 0x18);
  CYC(0x6aa6, 0x6aa8); eyesoarChild_animate_hook(gb); return; // jr

stillMovingRandomly:
  CYC(0x6aa8, 0x6aab); A = W8(wFrameCounter);
  CYC(0x6aab, 0x6aad); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x6aad, 0x6aaf); goto applySpeed; } // jr nz
  CYC(0x6aad, 0x6aaf);
  CALL_C(0x6aaf, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6ab2);
  CALL_C(0x6ab2, objectNudgeAngleTowards_hook, 0x1fd4, 0x6ab5);

applySpeed:
  CALL_C(0x6ab5, objectApplySpeed_hook, 0x201d, 0x6ab8);
  CALL_C(0x6ab8, ecom_bounceOffScreenBoundary_b0d_hook, 0x42e5, 0x6abb);
  eyesoarChild_animate_hook(gb); return; // falls through
}

void eyesoarChild_animate_hook(GB *gb) {
  CYC(0x6abb, 0x6abe); enemyAnimate_hook(gb); return; // jp
}

// Moving back toward Eyesoar
void eyesoarChild_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Load into wTmpcec0 the position offset relative to Eyesoar where we should be moving to
  CYC(0x6abe, 0x6abf); H = B;
  CYC(0x6abf, 0x6ac1); L = ENEMY_BASE + 0x3b; // Enemy.var3b (parent, h==b)
  CYC(0x6ac1, 0x6ac2); A = mem_rd(gb, HL);
  CYC(0x6ac2, 0x6ac4); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6ac4, 0x6ac5); A = mem_rd(gb, DE);
  CYC(0x6ac5, 0x6ac6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6ac6, 0x6ac8); alu_and(gb, 0x1f);
  CYC(0x6ac8, 0x6ac9); C = A;
  CYC(0x6ac9, 0x6acb); A = 0x18;
  CYC(0x6acb, 0x6acd); B = 0x28; // SPEED_100
  CALL_C(0x6acd, getScaledPositionOffsetForVelocity_hook, 0x212a, 0x6ad0);
  // Get parent.position + offset in bc
  CYC(0x6ad0, 0x6ad2); A = OBJ_YH; // Object.yh
  CALL_C(0x6ad2, objectGetRelatedObject1Var_hook, 0x2160, 0x6ad5);
  CYC(0x6ad5, 0x6ad8); A = mem_rd(gb, wTmpcec0 + 1);
  CYC(0x6ad8, 0x6ad9); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6ad9, 0x6ada); B = A;
  CYC(0x6ada, 0x6adc); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6adc, 0x6adf); A = mem_rd(gb, wTmpcec0 + 3);
  CYC(0x6adf, 0x6ae0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6ae0, 0x6ae1); C = A;
  // Store current position
  CYC(0x6ae1, 0x6ae2); E = L;
  CYC(0x6ae2, 0x6ae3); A = mem_rd(gb, DE);
  CYC(0x6ae3, 0x6ae5); hram_wr(gb, 0x8e, A);
  CYC(0x6ae5, 0x6ae7); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6ae7, 0x6ae8); A = mem_rd(gb, DE);
  CYC(0x6ae8, 0x6aea); hram_wr(gb, 0x8f, A);
  // Check if we've reached the target position
  CYC(0x6aea, 0x6aeb); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x6aeb, 0x6aed); goto moveToward; } // jr nz
  CYC(0x6aeb, 0x6aed);
  CYC(0x6aed, 0x6aef); A = hram_rd(gb, 0x8e);
  CYC(0x6aef, 0x6af0); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x6af0, 0x6af2); goto reachedTargetPosition; } // jr z
  CYC(0x6af0, 0x6af2);

moveToward:
  CALL_C(0x6af2, ecom_moveTowardPosition_b0d_hook, 0x4430, 0x6af5);
  CYC(0x6af5, 0x6af7); eyesoarChild_animate_hook(gb); return; // jr

reachedTargetPosition:
  // Wait for signal to change state
  CYC(0x6af7, 0x6af9); L = ENEMY_BASE + 0x39; // Enemy.var39
  CYC(0x6af9, 0x6afb); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6afb, 0x6afc); ret_effect(gb); return; } // ret nz
  CYC(0x6afb, 0x6afc);
  CYC(0x6afc, 0x6afe); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6afe, 0x6b00); A = 0x0e;
  CYC(0x6b00, 0x6b01); mem_wr(gb, DE, A);
  // Set flag in parent's var3a indicating we're good to go?
  CYC(0x6b01, 0x6b03); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6b03, 0x6b04); A = mem_rd(gb, DE);
  CYC(0x6b04, 0x6b06); alu_add(gb, 0x04);
  CYC(0x6b06, 0x6b08); L = ENEMY_BASE + 0x3a; // Enemy.var3a (parent, h==b)
  CYC(0x6b08, 0x6b0b); setFlag_hook(gb); return; // jp
}
