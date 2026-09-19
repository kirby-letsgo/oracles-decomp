#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void beetle_state_uninitialized_hook(GB *gb);
void beetle_state_spawner_hook(GB *gb);
void beetle_state_galeSeed_hook(GB *gb);
void beetle_state_switchHook_hook(GB *gb);
void beetle_state_stub_hook(GB *gb);
void beetle_subid1_hook(GB *gb);
void beetle_stateA_hook(GB *gb);
void beetle_animate_hook(GB *gb);
void beetle_subid2_hook(GB *gb);
void beetle_subid3_hook(GB *gb);
void beetle_chooseRandomAngleAndCounter1_hook(GB *gb);
void beetle_checkHazards_hook(GB *gb);

static uint16_t beetle_jump_table(GB *gb) {
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

static void beetle_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_BEETLE
//
// Variables for spawner (subid 0):
//   var30: Number of beetles spawned in? It's never actually used, and it doesn't seem to
//          update correctly, so this was probably for some abandoned idea.
//
// Variables for actual beetles (subid 1+):
//   relatedObj1: Reference to spawner object (optional)
// ==================================================================================================
void enemyCode51_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63e2, beetle_checkHazards_hook, 0x6558, 0x63e5);
  CYC(0x63e5, 0x63e6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x63e6, 0x63e8); goto normalStatus; } // jr z
  CYC(0x63e6, 0x63e8);
  CYC(0x63e8, 0x63ea); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x63ea); return; } // ret c
  CYC(0x63ea, 0x63eb);
  if (F & FZ) { CYCT(0x63eb, 0x63ed); goto dead; } // jr z
  CYC(0x63eb, 0x63ed);
  CYC(0x63ed, 0x63ee); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x63ee, 0x63f1); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x63ee, 0x63f1);
  CYC(0x63f1, 0x63f3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x63f3, 0x63f4); A = mem_rd(gb, DE);
  CYC(0x63f4, 0x63f6); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x63f6); return; } // ret nz
  CYC(0x63f6, 0x63f7);
  CYC(0x63f7, 0x63f9); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x63f9, 0x63fa); A = mem_rd(gb, DE);
  CYC(0x63fa, 0x63fc); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { RET_TAKEN(0x63fc); return; } // ret z
  CYC(0x63fc, 0x63fd);
  CYC(0x63fd, 0x63fe); H = D;
  CYC(0x63fe, 0x6400); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6400, 0x6402); mem_wr(gb, HL, 0x0a);
  CYC(0x6402, 0x6404); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6404, 0x6406); mem_wr(gb, HL, 0x01);
  RET(0x6406); return; // ret

dead:
  CYC(0x6407, 0x6409); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6409, 0x640a); A = mem_rd(gb, DE);
  CYC(0x640a, 0x640b); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x640b, 0x640d); goto die; } // jr nz
  CYC(0x640b, 0x640d);
  CYC(0x640d, 0x640f); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x640f, 0x6410); A = mem_rd(gb, DE);
  CYC(0x6410, 0x6411); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6411, 0x6413); goto die; } // jr z
  CYC(0x6411, 0x6413);
  CYC(0x6413, 0x6415); A = 0x30; // Object.var30
  CALL_C(0x6415, objectGetRelatedObject1Var_hook, 0x2160, 0x6418);
  CYC(0x6418, 0x6419); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

die:
  CYC(0x6419, 0x641c); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x641c, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x641f);
  if (!(F & FC)) { CYCT(0x641f, 0x6421); goto normalState; } // jr nc
  CYC(0x641f, 0x6421);
  {
    CYC(0x6421, 0x6422); push_effect(gb, 0x6422);
    uint16_t target = beetle_jump_table(gb);
    if (target == 0x643b) { beetle_state_uninitialized_hook(gb); return; }
    if (target == 0x6445) { beetle_state_spawner_hook(gb); return; }
    if (target == 0x6483) { beetle_state_stub_hook(gb); return; }
    if (target == 0x6472) { beetle_state_switchHook_hook(gb); return; }
    if (target == 0x645e) { beetle_state_galeSeed_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x6432, 0x6433); B = alu_dec8(gb, B);
  CYC(0x6433, 0x6434); A = B;
  {
    CYC(0x6434, 0x6435); push_effect(gb, 0x6435);
    uint16_t target = beetle_jump_table(gb);
    if (target == 0x6484) { beetle_subid1_hook(gb); return; }
    if (target == 0x64c8) { beetle_subid2_hook(gb); return; }
    if (target == 0x64f7) { beetle_subid3_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:643b, bare global; jump-table target from enemyCode51. Falls through into
// beetle_state_spawner.
void beetle_state_uninitialized_hook(GB *gb) {
  CYC(0x643b, 0x643c); A = B;
  CYC(0x643c, 0x643d); alu_or(gb, A);
  CYC(0x643d, 0x643f); A = 0x14; // SPEED_80
  if (!(F & FZ)) { CYCT(0x643f, 0x6442); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp nz
  CYC(0x643f, 0x6442);
  CYC(0x6442, 0x6444); A = 0x01;
  CYC(0x6444, 0x6445); mem_wr(gb, DE, A); // [state] = 1
  beetle_state_spawner_hook(gb); return; // fallthrough
}

// 0e:6445, bare global; jump-table target from enemyCode51, also falls into from
// beetle_state_uninitialized.
void beetle_state_spawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6445, ecom_decCounter2_b0e_hook, 0x43a3, 0x6448);
  if (!(F & FZ)) { RET_TAKEN(0x6448); return; } // ret nz
  CYC(0x6448, 0x6449);
  CYC(0x6449, 0x644b); C = 0x20;
  CALL_C(0x644b, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x644e);
  if (!(F & FC)) { RET_TAKEN(0x644e); return; } // ret nc
  CYC(0x644e, 0x644f);
  CYC(0x644f, 0x6451); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6451, 0x6453); A = 90;
  CYC(0x6453, 0x6454); mem_wr(gb, DE, A);
  CYC(0x6454, 0x6456); B = 0x51; // ENEMY_BEETLE
  CALL_C(0x6456, ecom_spawnEnemyWithSubid01_b0e_hook, 0x4373, 0x6459);
  if (!(F & FZ)) { RET_TAKEN(0x6459); return; } // ret nz
  CYC(0x6459, 0x645a);
  CYC(0x645a, 0x645b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 2
  CYC(0x645b, 0x645e); objectCopyPosition_hook(gb); return; // jp
}

// 0e:645e, bare global; jump-table target from enemyCode51.
void beetle_state_galeSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x645e, ecom_galeSeedEffect_b0e_hook, 0x447b, 0x6461);
  if (F & FC) { RET_TAKEN(0x6461); return; } // ret c
  CYC(0x6461, 0x6462);
  CYC(0x6462, 0x6464); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6464, 0x6465); A = mem_rd(gb, DE);
  CYC(0x6465, 0x6466); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6466, 0x6468); goto skip; } // jr z
  CYC(0x6466, 0x6468);
  CYC(0x6468, 0x6469); H = A;
  CYC(0x6469, 0x646b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x646b, 0x646c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

skip:
  CALL_C(0x646c, decNumEnemies_hook, 0x24b3, 0x646f);
  CYC(0x646f, 0x6472); enemyDelete_hook(gb); return; // jp
}

// 0e:6472, bare global; jump-table target from enemyCode51.
void beetle_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6472, 0x6473); E = alu_inc8(gb, E);
  CYC(0x6473, 0x6474); A = mem_rd(gb, DE);
  {
    CYC(0x6474, 0x6475); push_effect(gb, 0x6475);
    uint16_t target = beetle_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x647d) { RET(0x647d); return; } // ret (substate1/substate2)
    if (target == 0x647e) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(0x647e, 0x6480); B = 0x0a;
  CYC(0x6480, 0x6483); ecom_fallToGroundAndSetState_b0e_hook(gb); return; // jp
}

// 0e:6483, bare global; jump-table target from enemyCode51.
void beetle_state_stub_hook(GB *gb) {
  RET(0x6483); return; // ret
}

// 0e:6484, bare global; jump-table target from enemyCode51@normalState. Falls from the sky.
void beetle_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6484, 0x6485); A = mem_rd(gb, DE);
  CYC(0x6485, 0x6487); alu_sub(gb, 0x08);
  {
    CYC(0x6487, 0x6488); push_effect(gb, 0x6488);
    uint16_t target = beetle_jump_table(gb);
    if (target == 0x648e) goto state8;
    if (target == 0x64a2) goto state9;
    if (target == 0x64bc) { beetle_stateA_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x648e, 0x648f); H = D;
  CYC(0x648f, 0x6490); L = E;
  CYC(0x6490, 0x6491); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6491, 0x6493); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6493, 0x6495); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x6495, 0x6497); C = 0x08;
  CALL_C(0x6497, ecom_setZAboveScreen_b0e_hook, 0x4446, 0x649a);
  CALL_C(0x649a, objectSetVisiblec1_hook, 0x1e3c, 0x649d);
  CYC(0x649d, 0x649f); A = 0x59; // SND_FALLINHOLE
  CYC(0x649f, 0x64a2); playSound_b00_hook(gb); return; // jp

state9:
  CYC(0x64a2, 0x64a4); C = 0x0e;
  CALL_C(0x64a4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x64a7);
  if (!(F & FZ)) { RET_TAKEN(0x64a7); return; } // ret nz
  CYC(0x64a7, 0x64a8);
  CYC(0x64a8, 0x64aa); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x64aa, 0x64ab); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x64ab, 0x64ac); mem_wr(gb, HL, A);
  CYC(0x64ac, 0x64ae); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x64ae, 0x64af); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x64af, objectSetVisiblec2_hook, 0x1e45, 0x64b2);
  CYC(0x64b2, 0x64b4); A = 0x52; // SND_BOMB_LAND
  CALL_C(0x64b4, playSound_b00_hook, 0x0c98, 0x64b7);
  CALL_C(0x64b7, beetle_chooseRandomAngleAndCounter1_hook, 0x653c, 0x64ba);
  CYC(0x64ba, 0x64bc); beetle_animate_hook(gb); return; // jr
}

// 0e:64bc, bare global; jump-table target from beetle_subid1/beetle_subid2/beetle_subid3.
// Common beetle state. Falls through into beetle_animate.
void beetle_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x64bc, ecom_decCounter1_b0e_hook, 0x439a, 0x64bf);
  if (F & FZ) CALL_C_CC(0x64bf, beetle_chooseRandomAngleAndCounter1_hook, 0x653c, 0x64c2); else CYC(0x64bf, 0x64c2); // call z
  CALL_C(0x64c2, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x64c5);
  beetle_animate_hook(gb); return; // fallthrough
}

// 0e:64c5, bare global; falls into from beetle_stateA, also reached by genuine jr from
// beetle_subid1/beetle_subid2.
void beetle_animate_hook(GB *gb) {
  CYC(0x64c5, 0x64c8); enemyAnimate_hook(gb); return; // jp
}

// 0e:64c8, bare global; jump-table target from enemyCode51@normalState. Spawns in instantly.
void beetle_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x64c8, 0x64c9); A = mem_rd(gb, DE);
  CYC(0x64c9, 0x64cb); alu_sub(gb, 0x08);
  {
    CYC(0x64cb, 0x64cc); push_effect(gb, 0x64cc);
    uint16_t target = beetle_jump_table(gb);
    if (target == 0x64d2) goto state8;
    if (target == 0x64df) goto state9;
    if (target == 0x64bc) { beetle_stateA_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x64d2, 0x64d3); H = D;
  CYC(0x64d3, 0x64d4); L = E;
  CYC(0x64d4, 0x64d5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x64d5, 0x64d7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x64d7, 0x64d9); mem_wr(gb, HL, 0x1e);
  CALL_C(0x64d9, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x64dc);
  CYC(0x64dc, 0x64df); objectSetVisiblec2_hook(gb); return; // jp

state9:
  CALL_C(0x64df, ecom_decCounter1_b0e_hook, 0x439a, 0x64e2);
  if (!(F & FZ)) { CYCT(0x64e2, 0x64e4); goto keepMovingTowardLink; } // jr nz
  CYC(0x64e2, 0x64e4);
  CYC(0x64e4, 0x64e5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(0x64e5, 0x64e6); L = E;
  CYC(0x64e6, 0x64e7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x64e7, 0x64e9); beetle_stateA_hook(gb); return; // jr

keepMovingTowardLink:
  CYC(0x64e9, 0x64ea); A = mem_rd(gb, HL);
  CYC(0x64ea, 0x64ec); alu_cp(gb, 22);
  if (!(F & FZ)) { CYCT(0x64ec, 0x64ee); goto applyVelocity; } // jr nz
  CYC(0x64ec, 0x64ee);
  CYC(0x64ee, 0x64f0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x64f0, 0x64f2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity:
  CALL_C(0x64f2, ecom_applyVelocityForSideviewEnemy_b0e_hook, 0x4153, 0x64f5);
  CYC(0x64f5, 0x64f7); beetle_animate_hook(gb); return; // jr
}

// 0e:64f7, bare global; jump-table target from enemyCode51@normalState. "Bounces in" when it
// spawns (dug up from the ground).
void beetle_subid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x64f7, 0x64f8); A = mem_rd(gb, DE);
  CYC(0x64f8, 0x64fa); alu_sub(gb, 0x08);
  {
    CYC(0x64fa, 0x64fb); push_effect(gb, 0x64fb);
    uint16_t target = beetle_jump_table(gb);
    if (target == 0x6501) goto state8;
    if (target == 0x651b) goto state9;
    if (target == 0x64bc) { beetle_stateA_hook(gb); return; }
    HANDOFF(target);
  }

state8:
  CYC(0x6501, 0x6502); H = D;
  CYC(0x6502, 0x6503); L = E;
  CYC(0x6503, 0x6504); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6504, 0x6506); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6506, 0x6508); A = 0xfe; // <(-$102)
  CYC(0x6508, 0x6509); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6509, 0x650b); mem_wr(gb, HL, 0xfe); // >(-$102)
  CYC(0x650b, 0x650d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x650d, 0x650f); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x650f, 0x6511); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6511, 0x6514); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(0x6514, 0x6516); A = alu_swap(gb, A);
  CYC(0x6516, 0x6517); alu_rrca(gb);
  CYC(0x6517, 0x6518); mem_wr(gb, HL, A);
  CYC(0x6518, 0x651b); objectSetVisiblec2_hook(gb); return; // jp

state9:
  CYC(0x651b, 0x651d); C = 0x0e;
  CALL_C(0x651d, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x6520);
  if (F & FC) { CYCT(0x6520, 0x6522); goto doneBouncing; } // jr c
  CYC(0x6520, 0x6522);
  CYC(0x6522, 0x6524); A = 0x52; // SND_BOMB_LAND
  if (F & FZ) CALL_C_CC(0x6524, playSound_b00_hook, 0x0c98, 0x6527); else CYC(0x6524, 0x6527); // call z
  CYC(0x6527, 0x6529); E = ENEMY_BASE + OBJ_SPEED_Z + 1; // Enemy.speedZ+1
  CYC(0x6529, 0x652a); A = mem_rd(gb, DE);
  CYC(0x652a, 0x652b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x652b, 0x652d); goto applyVelocity; } // jr nz
  CYC(0x652b, 0x652d);
  CYC(0x652d, 0x652e); H = D;
  CYC(0x652e, 0x6530); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6530, 0x6532); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

applyVelocity:
  CYC(0x6532, 0x6535); ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(gb); return; // jp

doneBouncing:
  CALL_C(0x6535, ecom_incState_b0e_hook, 0x4000, 0x6538);
  CYC(0x6538, 0x653a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x653a, 0x653c); mem_wr(gb, HL, 0x14); // SPEED_80
  beetle_chooseRandomAngleAndCounter1_hook(gb); return; // fallthrough
}

// 0e:653c, bare global; called from beetle_stateA and beetle_subid1, also falls into from
// beetle_subid3.
void beetle_chooseRandomAngleAndCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x653c, 0x653f); SET_BC(0x071c);
  CALL_C(0x653f, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x6542);
  CYC(0x6542, 0x6544); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6544, 0x6545); A = C;
  CYC(0x6545, 0x6546); mem_wr(gb, DE, A);
  CYC(0x6546, 0x6547); A = B;
  CYC(0x6547, 0x654a); SET_HL(0x6550); // @counter1Vals
  CYC(0x654a, 0x654b); beetle_addAToHl_from_rst(gb, 0x654b);
  CYC(0x654b, 0x654d); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x654d, 0x654e); A = mem_rd(gb, HL);
  CYC(0x654e, 0x654f); mem_wr(gb, DE, A);
  RET(0x654f); return; // ret
}

// 0e:6558, bare global; called from enemyCode51. Beetle has custom checkHazards function so it
// can decrease the spawner's var30 (number of spawned).
void beetle_checkHazards_hook(GB *gb) {
  CYC(0x6558, 0x6559); B = A;
  CYC(0x6559, 0x655b); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x655b, 0x655c); A = mem_rd(gb, DE);
  CYC(0x655c, 0x655e); alu_cp(gb, 0x0a);
  CYC(0x655e, 0x655f); A = B;
  if (F & FC) { RET_TAKEN(0x655f); return; } // ret c
  CYC(0x655f, 0x6560);
  CYC(0x6560, 0x6561); H = D;
  CYC(0x6561, 0x6563); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x6563, 0x6565); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6565, 0x6567); goto checkHazards; } // jr z
  CYC(0x6565, 0x6567);
  CYC(0x6567, 0x6569); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6569, 0x656a); A = mem_rd(gb, HL);
  CYC(0x656a, 0x656c); alu_cp(gb, 0x3b); // 59
  if (!(F & FZ)) { CYCT(0x656c, 0x656e); goto checkHazards; } // jr nz
  CYC(0x656c, 0x656e);
  CYC(0x656e, 0x6570); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6570, 0x6571); A = mem_rd(gb, HL);
  CYC(0x6571, 0x6572); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6572, 0x6574); goto checkHazards; } // jr z
  CYC(0x6572, 0x6574);
  CYC(0x6574, 0x6575); H = A;
  CYC(0x6575, 0x6577); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6577, 0x6578); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

checkHazards:
  CYC(0x6578, 0x6579); A = B;
  CYC(0x6579, 0x657c); ecom_checkHazards_b0e_hook(gb); return; // jp
}
