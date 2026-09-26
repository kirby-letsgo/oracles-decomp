#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void flyingTile_state_uninitialized_hook(GB *gb);
void flyingTile_state_spawner_hook(GB *gb);
void flyingTile_state_stub_hook(GB *gb);
void flyingTile_state8_hook(GB *gb);
void flyingTile_state9_hook(GB *gb);
void flyingTile_animate_hook(GB *gb);
void flyingTile_stateA_hook(GB *gb);
void flyingTile_stateB_hook(GB *gb);
void flyingTile_dead_hook(GB *gb);
void flyingTile_delete_hook(GB *gb);
void flyingTile_overwriteTileHere_hook(GB *gb);

static uint16_t flyingTile_jump_table(GB *gb) {
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

static void flyingTile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void flyingTile_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// 0e:65ca, local sub-label of flyingTile_state_spawner: falls into from substate0, also
// reached by genuine call/ret from substate1.
// @param hl Address to save to var30/var31
static void flyingTile_saveTileDataAddress(GB *gb) {
  BASE(flyingTile_state_spawner);
  CYC(b_+27, b_+29); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+29, b_+30); A = L;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); E = alu_inc8(gb, E);
  CYC(b_+32, b_+33); A = H;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35);
}

// ==================================================================================================
// ENEMY_FLYING_TILE
//
// Variables:
//   var30/var31: Pointer to current address in flyingTile_layoutData
// ==================================================================================================
void enemyCode52_hook(GB *gb) {
  BASE(enemyCode52);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); TAIL(flyingTile_dead); // jp

normalStatus:
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  {
    CYC(b_+11, b_+12); push_effect(gb, b_+12);
    uint16_t target = flyingTile_jump_table(gb);
    if (target == SYM(flyingTile_state_uninitialized)) { flyingTile_state_uninitialized_hook(gb); return; }
    if (target == SYM(flyingTile_state_spawner)) { flyingTile_state_spawner_hook(gb); return; }
    if (target == SYM(flyingTile_state_stub)) { flyingTile_state_stub_hook(gb); return; }
    if (target == SYM(flyingTile_state8)) { flyingTile_state8_hook(gb); return; }
    if (target == SYM(flyingTile_state9)) { flyingTile_state9_hook(gb); return; }
    if (target == SYM(flyingTile_stateA)) { flyingTile_stateA_hook(gb); return; }
    if (target == SYM(flyingTile_stateB)) { flyingTile_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:65a0, bare global; jump-table target from enemyCode52.
void flyingTile_state_uninitialized_hook(GB *gb) {
  BASE(flyingTile_state_uninitialized);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  CYC(b_+4, b_+6); A = 0x46; // SPEED_1c0
  if (F & FC) { CYCT(b_+6, b_+9); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp c
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

// 0e:65af, bare global; jump-table target from enemyCode52.
void flyingTile_state_spawner_hook(GB *gb) {
  BASE(flyingTile_state_spawner);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = flyingTile_jump_table(gb);
    if (target == b_+7) goto substate0;
    if (target == b_+35) goto substate1;
    HANDOFF(target);
  }

substate0:
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+19); SET_HL(SYM(flyingTile_layoutData)); // flyingTile_layoutData
  CYC(b_+19, b_+20); flyingTile_addDoubleIndexToHl_from_rst(gb, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+21, b_+22); H = mem_rd(gb, HL);
  CYC(b_+22, b_+23); L = A;
  CYC(b_+23, b_+25); E = ENEMY_BASE + 0x03; // Enemy.var03
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  flyingTile_saveTileDataAddress(gb); ret_effect(gb); return; // fallthrough

substate1:
  CALL_C(b_+35, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+38);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; } // ret nz
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(b_+41, b_+43); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+44, b_+45); H = mem_rd(gb, HL);
  CYC(b_+45, b_+46); L = A;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+47, b_+48); C = A;
  PUSH(b_+48, HL);
  CYC(b_+49, b_+52); push_effect(gb, b_+52); flyingTile_saveTileDataAddress(gb); ret_effect(gb);
  CYC(b_+52, b_+54); B = 0x52; // ENEMY_FLYING_TILE
  CALL_C(b_+54, ecom_spawnEnemyWithSubid01_b0e_hook, SYM(ecom_spawnEnemyWithSubid01_b0e), b_+57);
  if (!(F & FZ)) { CYCT(b_+57, b_+59); goto childDone; } // jr nz
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+61, b_+63); E = ENEMY_BASE + 0x03; // Enemy.var03
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+67, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+70);

childDone:
  SET_HL(POP(b_+70));
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+73); return; } // ret nz
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+77); TAIL(flyingTile_delete); // jp
}

// 0e:65fc, bare global; jump-table target from enemyCode52.
void flyingTile_state_stub_hook(GB *gb) {
  BASE(flyingTile_state_stub);
  RET(b_+0); return; // ret
}

// 0e:65fd, bare global; jump-table target from enemyCode52. Initialization of actual flying
// tile (not spawner).
void flyingTile_state8_hook(GB *gb) {
  BASE(flyingTile_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(b_+7, flyingTile_overwriteTileHere_hook, SYM(flyingTile_overwriteTileHere), b_+10);
  CYC(b_+10, b_+13); TAIL(objectSetVisiblec2); // jp
}

// 0e:660a, bare global; jump-table target from enemyCode52. Moving up before charging at
// Link. Falls through into flyingTile_animate.
void flyingTile_state9_hook(GB *gb) {
  BASE(flyingTile_state9);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x80); // <($0080)
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_sbc(gb, 0x00); // >($0080)
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); alu_cp(gb, 0xfd);
  if (!(F & FC)) { CYCT(b_+13, b_+15); flyingTile_animate_hook(gb); return; } // jr nc
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0f);
  TAIL(flyingTile_animate); // fallthrough
}

// 0e:661f, bare global; falls into from flyingTile_state9, also reached by genuine jp/jr from
// flyingTile_stateA/flyingTile_stateB.
void flyingTile_animate_hook(GB *gb) {
  BASE(flyingTile_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0e:6622, bare global; jump-table target from enemyCode52. Staying in place for [counter1]
// frames before charging Link.
void flyingTile_stateA_hook(GB *gb) {
  BASE(flyingTile_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); flyingTile_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(b_+7, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+10);
  CYC(b_+10, b_+12); TAIL(flyingTile_animate); // jr
}

// 0e:662e, bare global; jump-table target from enemyCode52. Charging at Link. Falls through
// into flyingTile_dead.
void flyingTile_stateB_hook(GB *gb) {
  BASE(flyingTile_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+8); flyingTile_animate_hook(gb); return; } // jr nc
  CYC(b_+6, b_+8);
  TAIL(flyingTile_dead); // fallthrough
}

// 0e:6636, bare global; called from enemyCode52, also falls into from flyingTile_stateB.
void flyingTile_dead_hook(GB *gb) {
  BASE(flyingTile_dead);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(b_+2, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), SYM(flyingTile_delete));
  TAIL(flyingTile_delete); // fallthrough
}

// 0e:663b, bare global; called from flyingTile_state_spawner, also falls into from
// flyingTile_dead.
void flyingTile_delete_hook(GB *gb) {
  BASE(flyingTile_delete);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6); TAIL(enemyDelete); // jp
}

// 0e:6641, bare global; called from flyingTile_state8. Overwrites the tile at this position
// with whatever it should become after a flying tile is created there (depends on subid).
void flyingTile_overwriteTileHere_hook(GB *gb) {
  BASE(flyingTile_overwriteTileHere);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_and(gb, 0x0f);
  CYC(b_+9, b_+12); SET_HL(b_+17); // @tileReplacements
  CYC(b_+12, b_+13); flyingTile_addAToHl_from_rst(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+17); TAIL(setTile); // jp
}
