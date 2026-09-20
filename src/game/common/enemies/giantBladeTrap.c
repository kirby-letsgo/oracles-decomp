#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode2a_jump_table(GB *gb) {
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

static void giantBladeTrap_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// giantBladeTrap_checkCanMoveInDirection@checkTileAtOffsetSolid: an @-local reached only by two
// genuine `call` instructions from giantBladeTrap_checkCanMoveInDirection below; not in the
// requested symbol list and never separately hooked.
static void giantBladeTrap_checkTileAtOffsetSolid(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x64bd, 0x64be); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x64be, 0x64bf); alu_add(gb, B);
  CYC(0x64bf, 0x64c1); alu_and(gb, 0xf0);
  CYC(0x64c1, 0x64c2); E = A;
  CYC(0x64c2, 0x64c3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x64c3, 0x64c4); alu_add(gb, C);
  CYC(0x64c4, 0x64c6); A = alu_swap(gb, A);
  CYC(0x64c6, 0x64c8); alu_and(gb, 0x0f);
  CYC(0x64c8, 0x64c9); alu_or(gb, E);
  CYC(0x64c9, 0x64ca); E = A;
  CYC(0x64ca, 0x64cb); A = mem_rd(gb, DE);
  CYC(0x64cb, 0x64cc); alu_or(gb, A);
  CYC(0x64cc, 0x64cd); ret_effect(gb);
}

void giantBladeTrap_state_uninitialized_hook(GB *gb);
void giantBladeTrap_state_stub_hook(GB *gb);
void giantBladeTrap_subid00_hook(GB *gb);
void giantBladeTrap_subid01_hook(GB *gb);
void giantBladeTrap_subid01_state8_hook(GB *gb);
void giantBladeTrap_subid01_state9_hook(GB *gb);
void giantBladeTrap_subid01_stateA_hook(GB *gb);
void giantBladeTrap_subid02_hook(GB *gb);
void giantBladeTrap_subid02_state8_hook(GB *gb);
void giantBladeTrap_commonState9_hook(GB *gb);
void giantBladeTrap_subid02_stateA_hook(GB *gb);
void giantBladeTrap_subid03_hook(GB *gb);
void giantBladeTrap_subid03_state8_hook(GB *gb);
void giantBladeTrap_subid03_stateA_hook(GB *gb);
void giantBladeTrap_chooseInitialAngle_hook(GB *gb);
void giantBladeTrap_checkCanMoveInDirection_hook(GB *gb);
void giantBladeTrap_updateSpeed_hook(GB *gb);

// ==================================================================================================
// ENEMY_GIANT_BLADE_TRAP
// ==================================================================================================
void enemyCode2a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x637f, 0x6380); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6380, 0x6381); ret_effect(gb); return; } // ret z
  CYC(0x6380, 0x6381);
  CYC(0x6381, 0x6382); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6382, 0x6383); ret_effect(gb); return; } // ret z
  CYC(0x6382, 0x6383);
  CALL_C(0x6383, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x6386);
  if (F & FC) { CYCT(0x6386, 0x6388); goto commonState; } // jr c
  CYC(0x6386, 0x6388);
  CYC(0x6388, 0x6389); A = B;
  CYC(0x6389, 0x638a); push_effect(gb, 0x638a);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == 0x63ab) { giantBladeTrap_subid01_hook(gb); return; }
    if (target == 0x63de) { giantBladeTrap_subid02_hook(gb); return; }
    if (target == 0x643d) { giantBladeTrap_subid03_hook(gb); return; }
    giantBladeTrap_subid00_hook(gb); return; // target == 0x63aa
  }

commonState:
  CYC(0x6392, 0x6393); push_effect(gb, 0x6393);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == 0x63a3) { giantBladeTrap_state_uninitialized_hook(gb); return; }
    giantBladeTrap_state_stub_hook(gb); return; // states 1-7 all target 0x63a9
  }
}

void giantBladeTrap_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63a3, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x63a6);
  CYC(0x63a6, 0x63a9); objectSetVisible82_hook(gb); return; // jp
}

void giantBladeTrap_state_stub_hook(GB *gb) {
  RET(0x63a9); return;
}

void giantBladeTrap_subid00_hook(GB *gb) {
  RET(0x63aa); return;
}

void giantBladeTrap_subid01_hook(GB *gb) {
  CYC(0x63ab, 0x63ac); A = mem_rd(gb, DE);
  CYC(0x63ac, 0x63ae); alu_sub(gb, 0x08);
  CYC(0x63ae, 0x63af); push_effect(gb, 0x63af);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == 0x63c1) { giantBladeTrap_subid01_state9_hook(gb); return; }
    if (target == 0x63cf) { giantBladeTrap_subid01_stateA_hook(gb); return; }
    giantBladeTrap_subid01_state8_hook(gb); return; // target == 0x63b5
  }
}

// Choosing initial direction to move.
void giantBladeTrap_subid01_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x63b5, 0x63b7); A = 0x09;
  CYC(0x63b7, 0x63b8); mem_wr(gb, DE, A); // [state] = 9
  CALL_C(0x63b8, giantBladeTrap_chooseInitialAngle_hook, 0x6480, 0x63bb);
  CYC(0x63bb, 0x63bd); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x63bd, 0x63bf); A = 0x14; // SPEED_80
  CYC(0x63bf, 0x63c0); mem_wr(gb, DE, A);
  RET(0x63c0); return;
}

// Move until hitting a wall.
void giantBladeTrap_subid01_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63c1, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x63c4);
  if (F & FZ) { CYCT(0x63c4, 0x63c7); objectApplySpeed_hook(gb); return; } // jp z
  CYC(0x63c4, 0x63c7);
  CALL_C(0x63c7, ecom_incState_b0d_hook, 0x4000, 0x63ca);
  CYC(0x63ca, 0x63cc); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x63cc, 0x63ce); mem_wr(gb, HL, 0x10);
  RET(0x63ce); return;
}

// Wait 16 frames, then change directions and start moving again.
void giantBladeTrap_subid01_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63cf, ecom_decCounter1_b0d_hook, 0x439a, 0x63d2);
  if (!(F & FZ)) { CYCT(0x63d2, 0x63d3); ret_effect(gb); return; } // ret nz
  CYC(0x63d2, 0x63d3);
  CYC(0x63d3, 0x63d4); L = E;
  CYC(0x63d4, 0x63d5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]--
  CYC(0x63d5, 0x63d7); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x63d7, 0x63d8); A = mem_rd(gb, HL);
  CYC(0x63d8, 0x63da); alu_add(gb, 0x08);
  CYC(0x63da, 0x63dc); alu_and(gb, 0x18);
  CYC(0x63dc, 0x63dd); mem_wr(gb, HL, A);
  RET(0x63dd); return;
}

void giantBladeTrap_subid02_hook(GB *gb) {
  CYC(0x63de, 0x63df); A = mem_rd(gb, DE);
  CYC(0x63df, 0x63e1); alu_sub(gb, 0x08);
  CYC(0x63e1, 0x63e2); push_effect(gb, 0x63e2);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == 0x63f0) { giantBladeTrap_commonState9_hook(gb); return; }
    if (target == 0x6410) { giantBladeTrap_subid02_stateA_hook(gb); return; }
    giantBladeTrap_subid02_state8_hook(gb); return; // target == 0x63e8
  }
}

// Initialization
void giantBladeTrap_subid02_state8_hook(GB *gb) {
  CYC(0x63e8, 0x63e9); H = D;
  CYC(0x63e9, 0x63ea); L = E;
  CYC(0x63ea, 0x63eb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x63eb, 0x63ed); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x63ed, 0x63ef); mem_wr(gb, HL, 60);
  RET(0x63ef); return;
}

// Accelerate until hitting a wall. Shared by subid02 and subid03.
void giantBladeTrap_commonState9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63f0, giantBladeTrap_updateSpeed_hook, 0x64dd, 0x63f3);
  CALL_C(0x63f3, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x63f6);
  if (F & FZ) { CYCT(0x63f6, 0x63f9); objectApplySpeed_hook(gb); return; } // jp z
  CYC(0x63f6, 0x63f9);
  CALL_C(0x63f9, ecom_incState_b0d_hook, 0x4000, 0x63fc);
  CYC(0x63fc, 0x63fe); L = ENEMY_BASE + OBJ_YH;
  CYC(0x63fe, 0x63ff); A = mem_rd(gb, HL);
  CYC(0x63ff, 0x6401); alu_add(gb, 0x02);
  CYC(0x6401, 0x6403); alu_and(gb, 0xf8);
  CYC(0x6403, 0x6404); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6404, 0x6405); L = alu_inc8(gb, L);
  CYC(0x6405, 0x6406); A = mem_rd(gb, HL);
  CYC(0x6406, 0x6408); alu_add(gb, 0x02);
  CYC(0x6408, 0x640a); alu_and(gb, 0xf8);
  CYC(0x640a, 0x640b); mem_wr(gb, HL, A);
  CYC(0x640b, 0x640d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x640d, 0x640f); mem_wr(gb, HL, 0x10);
  RET(0x640f); return;
}

// Hit a wall, waiting for a bit then changing direction.
void giantBladeTrap_subid02_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6410, ecom_decCounter1_b0d_hook, 0x439a, 0x6413);
  if (!(F & FZ)) { CYCT(0x6413, 0x6414); ret_effect(gb); return; } // ret nz
  CYC(0x6413, 0x6414);
  CYC(0x6414, 0x6416); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6416, 0x6417); A = mem_rd(gb, DE);
  CYC(0x6417, 0x6419); alu_add(gb, 0x08);
  CYC(0x6419, 0x641b); alu_and(gb, 0x1f);
  CYC(0x641b, 0x641c); mem_wr(gb, DE, A);
  CALL_C(0x641c, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x641f);
  if (F & FZ) { CYCT(0x641f, 0x6421); goto canMove; } // jr z
  CYC(0x641f, 0x6421);
  CYC(0x6421, 0x6423); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6423, 0x6424); A = mem_rd(gb, DE);
  CYC(0x6424, 0x6426); alu_xor(gb, 0x10);
  CYC(0x6426, 0x6427); mem_wr(gb, DE, A);
  CALL_C(0x6427, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x642a);
  if (F & FZ) { CYCT(0x642a, 0x642c); goto canMove; } // jr z
  CYC(0x642a, 0x642c);
  CYC(0x642c, 0x642e); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x642e, 0x642f); A = mem_rd(gb, DE);
  CYC(0x642f, 0x6431); alu_sub(gb, 0x08);
  CYC(0x6431, 0x6433); alu_and(gb, 0x1f);
  CYC(0x6433, 0x6434); mem_wr(gb, DE, A);

canMove:
  CYC(0x6434, 0x6435); H = D;
  CYC(0x6435, 0x6437); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6437, 0x6438); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6438, 0x643a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x643a, 0x643c); mem_wr(gb, HL, 90);
  RET(0x643c); return;
}

void giantBladeTrap_subid03_hook(GB *gb) {
  CYC(0x643d, 0x643e); A = mem_rd(gb, DE);
  CYC(0x643e, 0x6440); alu_sub(gb, 0x08);
  CYC(0x6440, 0x6441); push_effect(gb, 0x6441);
  {
    uint16_t target = enemyCode2a_jump_table(gb);
    if (target == 0x63f0) { giantBladeTrap_commonState9_hook(gb); return; }
    if (target == 0x6453) { giantBladeTrap_subid03_stateA_hook(gb); return; }
    giantBladeTrap_subid03_state8_hook(gb); return; // target == 0x6447
  }
}

// Initialization
void giantBladeTrap_subid03_state8_hook(GB *gb) {
  CYC(0x6447, 0x6448); H = D;
  CYC(0x6448, 0x6449); L = E;
  CYC(0x6449, 0x644a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x644a, 0x644c); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x644c, 0x644e); mem_wr(gb, HL, 0x10);
  CYC(0x644e, 0x6450); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6450, 0x6452); mem_wr(gb, HL, 90);
  RET(0x6452); return;
}

// Hit a wall, waiting for a bit then changing direction.
void giantBladeTrap_subid03_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6453, ecom_decCounter1_b0d_hook, 0x439a, 0x6456);
  if (!(F & FZ)) { CYCT(0x6456, 0x6457); ret_effect(gb); return; } // ret nz
  CYC(0x6456, 0x6457);
  CYC(0x6457, 0x6459); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6459, 0x645a); A = mem_rd(gb, DE);
  CYC(0x645a, 0x645c); alu_sub(gb, 0x08);
  CYC(0x645c, 0x645e); alu_and(gb, 0x1f);
  CYC(0x645e, 0x645f); mem_wr(gb, DE, A);
  CALL_C(0x645f, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x6462);
  if (F & FZ) { CYCT(0x6462, 0x6464); goto canMove; } // jr z
  CYC(0x6462, 0x6464);
  CYC(0x6464, 0x6466); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6466, 0x6467); A = mem_rd(gb, DE);
  CYC(0x6467, 0x6469); alu_xor(gb, 0x10);
  CYC(0x6469, 0x646a); mem_wr(gb, DE, A);
  CALL_C(0x646a, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x646d);
  if (F & FZ) { CYCT(0x646d, 0x646f); goto canMove; } // jr z
  CYC(0x646d, 0x646f);
  CYC(0x646f, 0x6471); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6471, 0x6472); A = mem_rd(gb, DE);
  CYC(0x6472, 0x6474); alu_add(gb, 0x08);
  CYC(0x6474, 0x6476); alu_and(gb, 0x1f);
  CYC(0x6476, 0x6477); mem_wr(gb, DE, A);

canMove:
  CYC(0x6477, 0x6478); H = D;
  CYC(0x6478, 0x647a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x647a, 0x647b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x647b, 0x647d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x647d, 0x647f); mem_wr(gb, HL, 90);
  RET(0x647f); return;
}

// Subid 1 only; check all directions, choose which way to go.
void giantBladeTrap_chooseInitialAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6480, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x6483);
  CYC(0x6483, 0x6485); A = 0x08; // ANGLE_RIGHT
  if (!(F & FZ)) { CYCT(0x6485, 0x6487); goto setAngle; } // jr nz
  CYC(0x6485, 0x6487);
  CYC(0x6487, 0x6489); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6489, 0x648a); mem_wr(gb, DE, A);
  CALL_C(0x648a, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x648d);
  CYC(0x648d, 0x648f); A = 0x10; // ANGLE_DOWN
  if (!(F & FZ)) { CYCT(0x648f, 0x6491); goto setAngle; } // jr nz
  CYC(0x648f, 0x6491);
  CYC(0x6491, 0x6493); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6493, 0x6494); mem_wr(gb, DE, A);
  CALL_C(0x6494, giantBladeTrap_checkCanMoveInDirection_hook, 0x64a0, 0x6497);
  CYC(0x6497, 0x6499); A = 0x18; // ANGLE_LEFT
  if (!(F & FZ)) { CYCT(0x6499, 0x649b); goto setAngle; } // jr nz
  CYC(0x6499, 0x649b);
  CYC(0x649b, 0x649c); alu_xor(gb, A);

setAngle:
  CYC(0x649c, 0x649e); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x649e, 0x649f); mem_wr(gb, DE, A);
  RET(0x649f); return;
}

// Based on current angle value, this checks if it can move in that direction (it is not
// blocked by solid tiles directly ahead).
//
// @param[out] zflag  z if it can move in this direction.
void giantBladeTrap_checkCanMoveInDirection_hook(GB *gb) {
  CYC(0x64a0, 0x64a2); E = ENEMY_BASE + OBJ_YH;
  CYC(0x64a2, 0x64a3); A = mem_rd(gb, DE);
  CYC(0x64a3, 0x64a4); B = A;
  CYC(0x64a4, 0x64a6); E = ENEMY_BASE + OBJ_XH;
  CYC(0x64a6, 0x64a7); A = mem_rd(gb, DE);
  CYC(0x64a7, 0x64a8); C = A;
  CYC(0x64a8, 0x64aa); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x64aa, 0x64ab); A = mem_rd(gb, DE);
  CYC(0x64ab, 0x64ac); alu_rrca(gb);
  CYC(0x64ac, 0x64af); SET_HL(0x64cd); // @positionOffsets
  CYC(0x64af, 0x64b0); giantBladeTrap_addAToHl_from_rst(gb, 0x64b0);
  CYC(0x64b0, 0x64b1); push_effect(gb, DE);
  CYC(0x64b1, 0x64b3); D = 0xce; // >wRoomCollisions
  CYC(0x64b3, 0x64b6); giantBladeTrap_checkTileAtOffsetSolid(gb, 0x64b6);
  if (!(F & FZ)) { CYCT(0x64b6, 0x64b8); goto skip; } // jr nz
  CYC(0x64b6, 0x64b8);
  CYC(0x64b8, 0x64bb); giantBladeTrap_checkTileAtOffsetSolid(gb, 0x64bb);

skip:
  CYC(0x64bb, 0x64bc); SET_DE(pop_effect(gb));
  RET(0x64bc); return;
}

// Decrements counter1 and uses its value to determine speed. Lower values = higher speed.
void giantBladeTrap_updateSpeed_hook(GB *gb) {
  CYC(0x64dd, 0x64df); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x64df, 0x64e0); A = mem_rd(gb, DE);
  CYC(0x64e0, 0x64e1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x64e1, 0x64e2); ret_effect(gb); return; } // ret z
  CYC(0x64e1, 0x64e2);
  CYC(0x64e2, 0x64e3); A = mem_rd(gb, DE);
  CYC(0x64e3, 0x64e4); A = alu_dec8(gb, A);
  CYC(0x64e4, 0x64e5); mem_wr(gb, DE, A);
  CYC(0x64e5, 0x64e7); alu_and(gb, 0xf0);
  CYC(0x64e7, 0x64e9); A = alu_swap(gb, A);
  CYC(0x64e9, 0x64ec); SET_HL(0x64f2); // @speeds
  CYC(0x64ec, 0x64ed); giantBladeTrap_addAToHl_from_rst(gb, 0x64ed);
  CYC(0x64ed, 0x64ef); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x64ef, 0x64f0); A = mem_rd(gb, HL);
  CYC(0x64f0, 0x64f1); mem_wr(gb, DE, A);
  RET(0x64f1); return;
}
