#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  CYC(0x65ca, 0x65cc); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x65cc, 0x65cd); A = L;
  CYC(0x65cd, 0x65ce); mem_wr(gb, DE, A);
  CYC(0x65ce, 0x65cf); E = alu_inc8(gb, E);
  CYC(0x65cf, 0x65d0); A = H;
  CYC(0x65d0, 0x65d1); mem_wr(gb, DE, A);
  CYC(0x65d1, 0x65d2);
}

// ==================================================================================================
// ENEMY_FLYING_TILE
//
// Variables:
//   var30/var31: Pointer to current address in flyingTile_layoutData
// ==================================================================================================
void enemyCode52_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x657c, 0x657e); goto normalStatus; } // jr z
  CYC(0x657c, 0x657e);
  CYC(0x657e, 0x6580); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6580); return; } // ret c
  CYC(0x6580, 0x6581);
  CYC(0x6581, 0x6584); flyingTile_dead_hook(gb); return; // jp

normalStatus:
  CYC(0x6584, 0x6586); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6586, 0x6587); A = mem_rd(gb, DE);
  {
    CYC(0x6587, 0x6588); push_effect(gb, 0x6588);
    uint16_t target = flyingTile_jump_table(gb);
    if (target == 0x65a0) { flyingTile_state_uninitialized_hook(gb); return; }
    if (target == 0x65af) { flyingTile_state_spawner_hook(gb); return; }
    if (target == 0x65fc) { flyingTile_state_stub_hook(gb); return; }
    if (target == 0x65fd) { flyingTile_state8_hook(gb); return; }
    if (target == 0x660a) { flyingTile_state9_hook(gb); return; }
    if (target == 0x6622) { flyingTile_stateA_hook(gb); return; }
    if (target == 0x662e) { flyingTile_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:65a0, bare global; jump-table target from enemyCode52.
void flyingTile_state_uninitialized_hook(GB *gb) {
  CYC(0x65a0, 0x65a2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x65a2, 0x65a3); A = mem_rd(gb, DE);
  CYC(0x65a3, 0x65a4); alu_rlca(gb);
  CYC(0x65a4, 0x65a6); A = 0x46; // SPEED_1c0
  if (F & FC) { CYCT(0x65a6, 0x65a9); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp c
  CYC(0x65a6, 0x65a9);
  CYC(0x65a9, 0x65ab); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x65ab, 0x65ad); A = 0x01;
  CYC(0x65ad, 0x65ae); mem_wr(gb, DE, A);
  RET(0x65ae); return; // ret
}

// 0e:65af, bare global; jump-table target from enemyCode52.
void flyingTile_state_spawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x65af, 0x65b0); E = alu_inc8(gb, E);
  CYC(0x65b0, 0x65b1); A = mem_rd(gb, DE);
  {
    CYC(0x65b1, 0x65b2); push_effect(gb, 0x65b2);
    uint16_t target = flyingTile_jump_table(gb);
    if (target == 0x65b6) goto substate0;
    if (target == 0x65d2) goto substate1;
    HANDOFF(target);
  }

substate0:
  CYC(0x65b6, 0x65b7); H = D;
  CYC(0x65b7, 0x65b8); L = E;
  CYC(0x65b8, 0x65b9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(0x65b9, 0x65ba); L = alu_inc8(gb, L);
  CYC(0x65ba, 0x65bc); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(0x65bc, 0x65be); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x65be, 0x65bf); A = mem_rd(gb, DE);
  CYC(0x65bf, 0x65c2); SET_HL(0x6657); // flyingTile_layoutData
  CYC(0x65c2, 0x65c3); flyingTile_addDoubleIndexToHl_from_rst(gb, 0x65c3);
  CYC(0x65c3, 0x65c4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x65c4, 0x65c5); H = mem_rd(gb, HL);
  CYC(0x65c5, 0x65c6); L = A;
  CYC(0x65c6, 0x65c8); E = ENEMY_BASE + 0x03; // Enemy.var03
  CYC(0x65c8, 0x65c9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x65c9, 0x65ca); mem_wr(gb, DE, A);
  flyingTile_saveTileDataAddress(gb); return; // fallthrough

substate1:
  CALL_C(0x65d2, ecom_decCounter1_b0e_hook, 0x439a, 0x65d5);
  if (!(F & FZ)) { RET_TAKEN(0x65d5); return; } // ret nz
  CYC(0x65d5, 0x65d6);
  CYC(0x65d6, 0x65d8); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(0x65d8, 0x65da); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x65da, 0x65db); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x65db, 0x65dc); H = mem_rd(gb, HL);
  CYC(0x65dc, 0x65dd); L = A;
  CYC(0x65dd, 0x65de); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x65de, 0x65df); C = A;
  PUSH(0x65df, HL);
  CYC(0x65e0, 0x65e3); flyingTile_saveTileDataAddress(gb);
  CYC(0x65e3, 0x65e5); B = 0x52; // ENEMY_FLYING_TILE
  CALL_C(0x65e5, ecom_spawnEnemyWithSubid01_b0e_hook, 0x4373, 0x65e8);
  if (!(F & FZ)) { CYCT(0x65e8, 0x65ea); goto childDone; } // jr nz
  CYC(0x65e8, 0x65ea);
  CYC(0x65ea, 0x65ec); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x65ec, 0x65ee); E = ENEMY_BASE + 0x03; // Enemy.var03
  CYC(0x65ee, 0x65ef); A = mem_rd(gb, DE);
  CYC(0x65ef, 0x65f0); mem_wr(gb, HL, A);
  CYC(0x65f0, 0x65f2); L = ENEMY_BASE + OBJ_YH;
  CALL_C(0x65f2, setShortPosition_paramC_hook, 0x20b9, 0x65f5);

childDone:
  SET_HL(POP(0x65f5));
  CYC(0x65f6, 0x65f7); A = mem_rd(gb, HL);
  CYC(0x65f7, 0x65f8); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x65f8); return; } // ret nz
  CYC(0x65f8, 0x65f9);
  CYC(0x65f9, 0x65fc); flyingTile_delete_hook(gb); return; // jp
}

// 0e:65fc, bare global; jump-table target from enemyCode52.
void flyingTile_state_stub_hook(GB *gb) {
  RET(0x65fc); return; // ret
}

// 0e:65fd, bare global; jump-table target from enemyCode52. Initialization of actual flying
// tile (not spawner).
void flyingTile_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x65fd, 0x65fe); H = D;
  CYC(0x65fe, 0x65ff); L = E;
  CYC(0x65ff, 0x6600); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6600, 0x6602); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6602, 0x6604); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(0x6604, flyingTile_overwriteTileHere_hook, 0x6641, 0x6607);
  CYC(0x6607, 0x660a); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:660a, bare global; jump-table target from enemyCode52. Moving up before charging at
// Link. Falls through into flyingTile_animate.
void flyingTile_state9_hook(GB *gb) {
  CYC(0x660a, 0x660b); H = D;
  CYC(0x660b, 0x660d); L = ENEMY_BASE + OBJ_Z;
  CYC(0x660d, 0x660e); A = mem_rd(gb, HL);
  CYC(0x660e, 0x6610); alu_sub(gb, 0x80); // <($0080)
  CYC(0x6610, 0x6611); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6611, 0x6612); A = mem_rd(gb, HL);
  CYC(0x6612, 0x6614); alu_sbc(gb, 0x00); // >($0080)
  CYC(0x6614, 0x6615); mem_wr(gb, HL, A);
  CYC(0x6615, 0x6617); alu_cp(gb, 0xfd);
  if (F & FC) { CYCT(0x6617, 0x6619); flyingTile_animate_hook(gb); return; } // jr nc
  CYC(0x6617, 0x6619);
  CYC(0x6619, 0x661a); L = E;
  CYC(0x661a, 0x661b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x661b, 0x661d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x661d, 0x661f); mem_wr(gb, HL, 0x0f);
  flyingTile_animate_hook(gb); return; // fallthrough
}

// 0e:661f, bare global; falls into from flyingTile_state9, also reached by genuine jp/jr from
// flyingTile_stateA/flyingTile_stateB.
void flyingTile_animate_hook(GB *gb) {
  CYC(0x661f, 0x6622); enemyAnimate_hook(gb); return; // jp
}

// 0e:6622, bare global; jump-table target from enemyCode52. Staying in place for [counter1]
// frames before charging Link.
void flyingTile_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6622, ecom_decCounter1_b0e_hook, 0x439a, 0x6625);
  if (!(F & FZ)) { CYCT(0x6625, 0x6627); flyingTile_animate_hook(gb); return; } // jr nz
  CYC(0x6625, 0x6627);
  CYC(0x6627, 0x6628); L = E;
  CYC(0x6628, 0x6629); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(0x6629, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x662c);
  CYC(0x662c, 0x662e); flyingTile_animate_hook(gb); return; // jr
}

// 0e:662e, bare global; jump-table target from enemyCode52. Charging at Link. Falls through
// into flyingTile_dead.
void flyingTile_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x662e, objectApplySpeed_hook, 0x201d, 0x6631);
  CALL_C(0x6631, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x6634);
  if (!(F & FC)) { CYCT(0x6634, 0x6636); flyingTile_animate_hook(gb); return; } // jr nc
  CYC(0x6634, 0x6636);
  flyingTile_dead_hook(gb); return; // fallthrough
}

// 0e:6636, bare global; called from enemyCode52, also falls into from flyingTile_stateB.
void flyingTile_dead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6636, 0x6638); B = 0x06; // INTERAC_ROCKDEBRIS
  CALL_C(0x6638, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x663b);
  flyingTile_delete_hook(gb); return; // fallthrough
}

// 0e:663b, bare global; called from flyingTile_state_spawner, also falls into from
// flyingTile_dead.
void flyingTile_delete_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x663b, decNumEnemies_hook, 0x24b3, 0x663e);
  CYC(0x663e, 0x6641); enemyDelete_hook(gb); return; // jp
}

// 0e:6641, bare global; called from flyingTile_state8. Overwrites the tile at this position
// with whatever it should become after a flying tile is created there (depends on subid).
void flyingTile_overwriteTileHere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6641, objectGetShortPosition_hook, 0x2096, 0x6644);
  CYC(0x6644, 0x6645); C = A;
  CYC(0x6645, 0x6647); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6647, 0x6648); A = mem_rd(gb, DE);
  CYC(0x6648, 0x664a); alu_and(gb, 0x0f);
  CYC(0x664a, 0x664d); SET_HL(0x6652); // @tileReplacements
  CYC(0x664d, 0x664e); flyingTile_addAToHl_from_rst(gb, 0x664e);
  CYC(0x664e, 0x664f); A = mem_rd(gb, HL);
  CYC(0x664f, 0x6652); setTile_hook(gb); return; // jp
}
