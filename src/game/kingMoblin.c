#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void enemyCode7f_hook(GB *gb);
void kingMoblin_state_uninitialized_hook(GB *gb);
void kingMoblin_state_stub_hook(GB *gb);
void kingMoblin_state8_hook(GB *gb);
void kingMoblin_animate_hook(GB *gb);
void kingMoblin_state9_hook(GB *gb);
void kingMoblin_stateA_hook(GB *gb);
void kingMoblin_stateB_hook(GB *gb);
void kingMoblin_initBombPickupAnimation_hook(GB *gb);
void kingMoblin_stateC_hook(GB *gb);
void kingMoblin_stateD_hook(GB *gb);
void kingMoblin_stateE_hook(GB *gb);
void kingMoblin_stateF_hook(GB *gb);
void kingMoblin_grabBomb_hook(GB *gb);
void kingMoblin_state10_hook(GB *gb);
void kingMoblin_animate2_hook(GB *gb);
void kingMoblin_state11_hook(GB *gb);
void kingMoblin_state12_hook(GB *gb);
void kingMoblin_state13_hook(GB *gb);
void kingMoblin_state14_hook(GB *gb);
void kingMoblin_state15_hook(GB *gb);
void kingMoblin_checkMoveToCentre_hook(GB *gb);
void kingMoblin_moveToCentre_hook(GB *gb);
void kingMoblin_setAngleStateAndAnimation_hook(GB *gb);
void kingMoblin_setStateAndAnimation_hook(GB *gb);

static uint16_t kingMoblin_jump_table(GB *gb) {
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

static void kingMoblin_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// kingMoblin_state_uninitialized@spawnMinion: reached only by a static `call` from
// kingMoblin_state_uninitialized itself (NOT HOOKED).
// @param[out] a,h Object index
static void kingMoblin_spawnMinion(GB *gb, uint16_t sp0_) {
  CALL_C(0x7d1e, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x7d21);
  CYC(0x7d21, 0x7d23); mem_wr(gb, HL, 0x56); // ENEMY_KING_MOBLIN_MINION
  CYC(0x7d23, 0x7d25); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x7d25, 0x7d27); A = 0x80; // Enemy.start
  CYC(0x7d27, 0x7d28); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7d28, 0x7d29); mem_wr(gb, HL, D);
  CYC(0x7d29, 0x7d2a); A = H;
  RET(0x7d2a); return; // ret
}

// ==================================================================================================
// ENEMY_KING_MOBLIN
//
// Variables:
//   counter2: ?
//   relatedObj2: Instance of PART_KING_MOBLIN_BOMB
//   var30/var31: Object indices for two ENEMY_KING_MOBLIN_MINION instances
//   var32: Target x-position to walk toward to grab bomb
//   var33: Signal from ENEMY_KING_MOBLIN_MINION to trigger warp to the outside
// ==================================================================================================
void enemyCode7f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x7c60, 0x7c62); goto normalStatus; } // jr z
  CYC(0x7c60, 0x7c62);
  CYC(0x7c62, 0x7c64); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x7c64); return; } // ret c
  CYC(0x7c64, 0x7c65);
  if (F & FZ) { CYCT(0x7c65, 0x7c67); goto dead; } // jr z
  CYC(0x7c65, 0x7c67);
  CYC(0x7c67, 0x7c69); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x7c69, 0x7c6a); A = mem_rd(gb, DE);
  CYC(0x7c6a, 0x7c6c); alu_cp(gb, 0x98); // $80|ITEMCOLLISION_BOMB
  if (!(F & FZ)) { CYCT(0x7c6c, 0x7c6e); goto normalStatus; } // jr nz
  CYC(0x7c6c, 0x7c6e);
  CYC(0x7c6e, 0x7c70); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(0x7c70, playSound_b00_hook, 0x0c98, 0x7c73);
  CYC(0x7c73, 0x7c75); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7c75, 0x7c76); A = mem_rd(gb, DE);
  CYC(0x7c76, 0x7c77); A = alu_dec8(gb, A);
  CYC(0x7c77, 0x7c7a); SET_HL(0x7c81); // @speeds
  CYC(0x7c7a, 0x7c7b); kingMoblin_addAToHl_from_rst(gb, 0x7c7b);
  CYC(0x7c7b, 0x7c7d); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7c7d, 0x7c7e); A = mem_rd(gb, HL);
  CYC(0x7c7e, 0x7c7f); mem_wr(gb, DE, A);
  CYCT(0x7c7f, 0x7c81); goto normalStatus; // jr

dead:
  CALL_C(0x7c87, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7c8a);
  if (!(F & FC)) { RET_TAKEN(0x7c8a); return; } // ret nc
  CYC(0x7c8a, 0x7c8b);
  CYC(0x7c8b, 0x7c8d); A = 0x01;
  CYC(0x7c8d, 0x7c90); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7c90, 0x7c93); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7c93, 0x7c94); H = D;
  CYC(0x7c94, 0x7c96); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7c96, 0x7c98); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7c98, 0x7c9a); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7c9a, 0x7c9b); mem_wr(gb, HL, A); // [health] = $01
  CYC(0x7c9b, 0x7c9d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7c9d, 0x7c9f); mem_wr(gb, HL, 0x12);
  CYC(0x7c9f, 0x7ca1); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7ca1, 0x7ca3); mem_wr(gb, HL, 0x00);
  CYC(0x7ca3, 0x7ca5); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7ca5, 0x7ca7); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(0x7ca7, 0x7ca9); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x7ca9, 0x7cab); mem_wr(gb, HL, 0x00);
  CYC(0x7cab, 0x7cad); A = 0x06;
  CALL_C(0x7cad, enemySetAnimation_hook, 0x282b, 0x7cb0);

normalStatus:
  CYC(0x7cb0, 0x7cb2); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x7cb2, 0x7cb3); A = mem_rd(gb, DE);
  CYC(0x7cb3, 0x7cb4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7cb4); return; } // ret nz
  CYC(0x7cb4, 0x7cb5);
  CYC(0x7cb5, 0x7cb7); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7cb7, 0x7cb8); A = mem_rd(gb, DE);
  CYC(0x7cb8, 0x7cb9); push_effect(gb, 0x7cb9);
  {
    uint16_t target = kingMoblin_jump_table(gb);
    if (target == 0x7ce5) { kingMoblin_state_uninitialized_hook(gb); return; }
    if (target == 0x7d2b) { kingMoblin_state_stub_hook(gb); return; }
    if (target == 0x7d2c) { kingMoblin_state8_hook(gb); return; }
    if (target == 0x7d6f) { kingMoblin_state9_hook(gb); return; }
    if (target == 0x7d84) { kingMoblin_stateA_hook(gb); return; }
    if (target == 0x7d9c) { kingMoblin_stateB_hook(gb); return; }
    if (target == 0x7dc1) { kingMoblin_stateC_hook(gb); return; }
    if (target == 0x7df3) { kingMoblin_stateD_hook(gb); return; }
    if (target == 0x7e2c) { kingMoblin_stateE_hook(gb); return; }
    if (target == 0x7e37) { kingMoblin_stateF_hook(gb); return; }
    if (target == 0x7e88) { kingMoblin_state10_hook(gb); return; }
    if (target == 0x7ea4) { kingMoblin_state11_hook(gb); return; }
    if (target == 0x7ebb) { kingMoblin_state12_hook(gb); return; }
    if (target == 0x7edb) { kingMoblin_state13_hook(gb); return; }
    if (target == 0x7eef) { kingMoblin_state14_hook(gb); return; }
    if (target == 0x7f12) { kingMoblin_state15_hook(gb); return; }
    HANDOFF(target);
  }
}

void kingMoblin_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ce5, 0x7ce7); A = 0x7f; // ENEMY_KING_MOBLIN
  CYC(0x7ce7, 0x7cea); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CYC(0x7cea, 0x7cec); A = 0x8c; // PALH_8c
  CALL_C(0x7cec, loadPaletteHeader_hook, 0x050b, 0x7cef);
  CYC(0x7cef, 0x7cf1); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(0x7cf1, playSound_b00_hook, 0x0c98, 0x7cf4);
  CYC(0x7cf4, 0x7cf5); alu_xor(gb, A);
  CYC(0x7cf5, 0x7cf8); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(0x7cf8, 0x7cf9); A = alu_dec8(gb, A);
  CYC(0x7cf9, 0x7cfc); mem_wr(gb, wActiveMusic, A);
  CYC(0x7cfc, 0x7cfe); B = 0x02;
  CALL_C(0x7cfe, checkBEnemySlotsAvailable_hook, 0x20f7, 0x7d01);
  if (!(F & FZ)) { RET_TAKEN(0x7d01); return; } // ret nz
  CYC(0x7d01, 0x7d02);
  CYC(0x7d02, 0x7d05); push_effect(gb, 0x7d05); kingMoblin_spawnMinion(gb, sp0_);
  CYC(0x7d05, 0x7d07); E = ENEMY_BASE + 0x30; // var30
  CYC(0x7d07, 0x7d08); mem_wr(gb, DE, A);
  CYC(0x7d08, 0x7d0b); push_effect(gb, 0x7d0b); kingMoblin_spawnMinion(gb, sp0_);
  CYC(0x7d0b, 0x7d0d); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7d0d, 0x7d0e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7d0e, 0x7d10); E = ENEMY_BASE + 0x31; // var31
  CYC(0x7d10, 0x7d11); mem_wr(gb, DE, A);
  CYC(0x7d11, 0x7d13); A = 0x1e; // SPEED_c0
  CALL_C(0x7d13, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x7d16);
  CALL_C(0x7d16, objectSetVisible83_hook, 0x1e72, 0x7d19);
  CYC(0x7d19, 0x7d1b); A = 0x02;
  CYC(0x7d1b, 0x7d1e); enemySetAnimation_hook(gb); return; // jp
}

void kingMoblin_state_stub_hook(GB *gb) {
  RET(0x7d2b); return; // ret
}

// Waiting for Link to move in to start the fight
void kingMoblin_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d2c, 0x7d2f); SET_HL(w1Link_xh);
  CYC(0x7d2f, 0x7d30); A = mem_rd(gb, HL);
  CYC(0x7d30, 0x7d32); alu_sub(gb, 0x40);
  CYC(0x7d32, 0x7d34); alu_cp(gb, 0x20);
  if (!(F & FC)) { CYCT(0x7d34, 0x7d36); kingMoblin_animate_hook(gb); return; } // jr nc
  CYC(0x7d34, 0x7d36);
  CYC(0x7d36, 0x7d38); L = 0x0f; // w1Link.zh's low byte
  CYC(0x7d38, 0x7d39); A = mem_rd(gb, HL);
  CYC(0x7d39, 0x7d3a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7d3a, 0x7d3c); kingMoblin_animate_hook(gb); return; } // jr nz
  CYC(0x7d3a, 0x7d3c);
  CYC(0x7d3c, 0x7d3e); L = 0x08; // w1Link.direction's low byte
  CYC(0x7d3e, 0x7d40); mem_wr(gb, HL, 0x00); // DIR_UP
  CALL_C(0x7d40, checkLinkVulnerable_hook, 0x1d28, 0x7d43);
  if (!(F & FC)) { RET_TAKEN(0x7d43); return; } // ret nc
  CYC(0x7d43, 0x7d44);
  CALL_C(0x7d44, clearAllParentItems_hook, 0x2c10, 0x7d47);
  CYC(0x7d47, 0x7d49); A = 0x01;
  CYC(0x7d49, 0x7d4c); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7d4c, 0x7d4f); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7d4f, 0x7d51); C = 0x61;
  CYC(0x7d51, 0x7d53); A = 0xa0; // TILEINDEX_STANDARD_FLOOR
  CALL_C(0x7d53, setTile_hook, 0x3a9c, 0x7d56);
  CALL_C(0x7d56, getFreeInteractionSlot_hook, 0x3aef, 0x7d59);
  if (!(F & FZ)) { CYCT(0x7d59, 0x7d5b); goto L_7d65; } // jr nz
  CYC(0x7d59, 0x7d5b);
  CYC(0x7d5b, 0x7d5d); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x7d5d, 0x7d5f); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7d5f, 0x7d61); mem_wr(gb, HL, 0x68);
  CYC(0x7d61, 0x7d63); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7d63, 0x7d65); mem_wr(gb, HL, 0x18);

L_7d65:
  CALL_C(0x7d65, ecom_incState_b0f_hook, 0x4000, 0x7d68);
  CYC(0x7d68, 0x7d6a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7d6a, 0x7d6c); mem_wr(gb, HL, 0x18);
  kingMoblin_animate_hook(gb); return; // fallthrough
}

void kingMoblin_animate_hook(GB *gb) {
  CYC(0x7d6c, 0x7d6f); enemyAnimate_hook(gb); return; // jp
}

// Delay before showing text
void kingMoblin_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d6f, ecom_decCounter1_b0f_hook, 0x439a, 0x7d72);
  if (!(F & FZ)) { CYCT(0x7d72, 0x7d74); kingMoblin_animate_hook(gb); return; } // jr nz
  CYC(0x7d72, 0x7d74);
  CYC(0x7d74, 0x7d75); L = E;
  CYC(0x7d75, 0x7d76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CALL_C(0x7d76, checkIsLinkedGame_hook, 0x1992, 0x7d79);
  CYC(0x7d79, 0x7d7c); SET_BC(0x2f19); // TX_2f19
  if (F & FZ) { CYCT(0x7d7c, 0x7d7e); goto L_7d81; } // jr z
  CYC(0x7d7c, 0x7d7e);
  CYC(0x7d7e, 0x7d81); SET_BC(0x2f1a); // TX_2f1a

L_7d81:
  CYC(0x7d81, 0x7d84); showText_hook(gb); return; // jp
}

// Starting fight
void kingMoblin_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d84, 0x7d85); H = D;
  CYC(0x7d85, 0x7d86); L = E;
  CYC(0x7d86, 0x7d87); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  CYC(0x7d87, 0x7d89); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7d89, 0x7d8b); mem_wr(gb, HL, 30);
  CYC(0x7d8b, 0x7d8d); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7d8d, 0x7d8e); B = mem_rd(gb, HL);
  CYC(0x7d8e, 0x7d8f); C = E;
  CYC(0x7d8f, 0x7d91); A = 0x02;
  CYC(0x7d91, 0x7d92); mem_wr(gb, BC, A); // [minion1.state] = $02
  CYC(0x7d92, 0x7d93); L = alu_inc8(gb, L);
  CYC(0x7d93, 0x7d94); B = mem_rd(gb, HL);
  CYC(0x7d94, 0x7d95); mem_wr(gb, BC, A); // [minion2.state] = $02
  CALL_C(0x7d95, enemyBoss_beginBoss_b0f_hook, 0x4584, 0x7d98);
  CYC(0x7d98, 0x7d99); alu_xor(gb, A);
  CYC(0x7d99, 0x7d9c); enemySetAnimation_hook(gb); return; // jp
}

// Facing backwards while picking up a bomb
void kingMoblin_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d9c, ecom_decCounter2_b0f_hook, 0x43a3, 0x7d9f);
  if (!(F & FZ)) { CYCT(0x7d9f, 0x7da1); kingMoblin_animate_hook(gb); return; } // jr nz
  CYC(0x7d9f, 0x7da1);
  CYC(0x7da1, 0x7da3); B = 0x3f; // PART_KING_MOBLIN_BOMB
  CALL_C(0x7da3, ecom_spawnProjectile_b0f_hook, 0x437c, 0x7da6);
  if (!(F & FZ)) { RET_TAKEN(0x7da6); return; } // ret nz
  CYC(0x7da6, 0x7da7);
  CALL_C(0x7da7, ecom_incState_b0f_hook, 0x4000, 0x7daa);
  kingMoblin_initBombPickupAnimation_hook(gb); return; // fallthrough
}

void kingMoblin_initBombPickupAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7daa, 0x7dac); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7dac, 0x7dad); A = mem_rd(gb, DE);
  CYC(0x7dad, 0x7dae); A = alu_dec8(gb, A);
  CYC(0x7dae, 0x7db1); SET_HL(0x7dbb); // @counter2Vals
  CYC(0x7db1, 0x7db2); kingMoblin_addAToHl_from_rst(gb, 0x7db2);
  CYC(0x7db2, 0x7db4); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7db4, 0x7db5); A = mem_rd(gb, HL);
  CYC(0x7db5, 0x7db6); mem_wr(gb, DE, A);
  CYC(0x7db6, 0x7db8); A = 0x04;
  CYC(0x7db8, 0x7dbb); enemySetAnimation_hook(gb); return; // jp
}

// Will raise bomb over head in [counter2] frames?
void kingMoblin_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7dc1, kingMoblin_checkMoveToCentre_hook, 0x7f5c, 0x7dc4);
  if (!(F & FZ)) { RET_TAKEN(0x7dc4); return; } // ret nz
  CYC(0x7dc4, 0x7dc5);
  CALL_C(0x7dc5, ecom_decCounter2_b0f_hook, 0x43a3, 0x7dc8);
  if (!(F & FZ)) { RET_TAKEN(0x7dc8); return; } // ret nz
  CYC(0x7dc8, 0x7dc9);
  CYC(0x7dc9, 0x7dcb); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7dcb, 0x7dcc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0d
  CYC(0x7dcc, 0x7dce); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7dce, 0x7dcf); A = mem_rd(gb, HL);
  CYC(0x7dcf, 0x7dd0); A = alu_dec8(gb, A);
  CYC(0x7dd0, 0x7dd3); SET_HL(0x7ded); // @counter2Vals
  CYC(0x7dd3, 0x7dd4); kingMoblin_addAToHl_from_rst(gb, 0x7dd4);
  CYC(0x7dd4, 0x7dd6); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7dd6, 0x7dd7); A = mem_rd(gb, HL);
  CYC(0x7dd7, 0x7dd8); mem_wr(gb, DE, A);
  CYC(0x7dd8, 0x7dda); A = OBJ_STATE; // Object.state
  CALL_C(0x7dda, objectGetRelatedObject2Var_hook, 0x2164, 0x7ddd);
  CYC(0x7ddd, 0x7dde); A = mem_rd(gb, HL);
  CYC(0x7dde, 0x7de0); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x7de0, 0x7de2); goto L_7de8; } // jr z
  CYC(0x7de0, 0x7de2);
  CYC(0x7de2, 0x7de5); SET_BC(0xf0f2);
  CALL_C(0x7de5, objectCopyPositionWithOffset_hook, 0x225a, 0x7de8);

L_7de8:
  CYC(0x7de8, 0x7dea); A = 0x02;
  CYC(0x7dea, 0x7ded); enemySetAnimation_hook(gb); return; // jp
}

// Delay before throwing bomb
void kingMoblin_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7df3, kingMoblin_checkMoveToCentre_hook, 0x7f5c, 0x7df6);
  if (!(F & FZ)) { RET_TAKEN(0x7df6); return; } // ret nz
  CYC(0x7df6, 0x7df7);
  CALL_C(0x7df7, ecom_decCounter2_b0f_hook, 0x43a3, 0x7dfa);
  if (!(F & FZ)) { RET_TAKEN(0x7dfa); return; } // ret nz
  CYC(0x7dfa, 0x7dfb);
  CYC(0x7dfb, 0x7dfd); mem_wr(gb, HL, 30);
  CYC(0x7dfd, 0x7dff); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7dff, 0x7e00); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0e
  CALL_C(0x7e00, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x7e03);
  CYC(0x7e03, 0x7e04); B = A;
  CYC(0x7e04, 0x7e06); alu_sub(gb, 0x0c);
  CYC(0x7e06, 0x7e08); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(0x7e08, 0x7e0a); goto L_7e11; } // jr c
  CYC(0x7e08, 0x7e0a);
  CYC(0x7e0a, 0x7e0c); B = 0x0c;
  CYC(0x7e0c, 0x7e0d); alu_rlca(gb);
  if (F & FC) { CYCT(0x7e0d, 0x7e0f); goto L_7e11; } // jr c
  CYC(0x7e0d, 0x7e0f);
  CYC(0x7e0f, 0x7e11); B = 0x13;

L_7e11:
  CYC(0x7e11, 0x7e13); A = OBJ_STATE; // Object.state
  CALL_C(0x7e13, objectGetRelatedObject2Var_hook, 0x2164, 0x7e16);
  CYC(0x7e16, 0x7e17); A = mem_rd(gb, HL);
  CYC(0x7e17, 0x7e19); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x7e19, 0x7e1b); goto L_7e27; } // jr z
  CYC(0x7e19, 0x7e1b);
  CYC(0x7e1b, 0x7e1d); mem_wr(gb, HL, 0x03); // [bomb.state] = $03
  CYC(0x7e1d, 0x7e1f); L = PART_BASE + OBJ_ANGLE;
  CYC(0x7e1f, 0x7e20); mem_wr(gb, HL, B);
  CYC(0x7e20, 0x7e22); L = PART_BASE + OBJ_SPEED_Z;
  CYC(0x7e22, 0x7e24); mem_wr(gb, HL, 0x80); // <(-$180)
  CYC(0x7e24, 0x7e25); L = alu_inc8(gb, L);
  CYC(0x7e25, 0x7e27); mem_wr(gb, HL, 0xfe); // >(-$180)

L_7e27:
  CYC(0x7e27, 0x7e29); A = 0x05;
  CYC(0x7e29, 0x7e2c); enemySetAnimation_hook(gb); return; // jp
}

// Delay after throwing bomb
void kingMoblin_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7e2c, ecom_decCounter2_b0f_hook, 0x43a3, 0x7e2f);
  if (!(F & FZ)) { RET_TAKEN(0x7e2f); return; } // ret nz
  CYC(0x7e2f, 0x7e30);
  CYC(0x7e30, 0x7e31); L = E;
  CYC(0x7e31, 0x7e32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0f
  CYC(0x7e32, 0x7e34); A = 0x02;
  CYC(0x7e34, 0x7e37); enemySetAnimation_hook(gb); return; // jp
}

// Waiting for something to do
void kingMoblin_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7e37, 0x7e39); A = OBJ_ID; // Object.id
  CALL_C(0x7e39, objectGetRelatedObject2Var_hook, 0x2164, 0x7e3c);
  CYC(0x7e3c, 0x7e3d); A = mem_rd(gb, HL);
  CYC(0x7e3d, 0x7e3f); alu_cp(gb, 0x3f); // PART_KING_MOBLIN_BOMB
  if (!(F & FZ)) { CYCT(0x7e3f, 0x7e42); kingMoblin_moveToCentre_hook(gb); return; } // jp nz
  CYC(0x7e3f, 0x7e42);
  CYC(0x7e42, 0x7e44); L = PART_BASE + OBJ_STATE;
  CYC(0x7e44, 0x7e45); A = mem_rd(gb, HL);
  CYC(0x7e45, 0x7e47); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x7e47, 0x7e49); kingMoblin_animate2_hook(gb); return; } // jr nz
  CYC(0x7e47, 0x7e49);
  CYC(0x7e49, 0x7e4b); L = PART_BASE + OBJ_YH;
  CYC(0x7e4b, 0x7e4c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7e4c, 0x7e4e); alu_cp(gb, 0x36);
  if (F & FC) { CYCT(0x7e4e, 0x7e50); kingMoblin_animate2_hook(gb); return; } // jr nc
  CYC(0x7e4e, 0x7e50);
  CYC(0x7e50, 0x7e51); L = alu_inc8(gb, L);
  CYC(0x7e51, 0x7e52); A = mem_rd(gb, HL); // [bomb.xh]
  CYC(0x7e52, 0x7e54); alu_sub(gb, 0x30);
  CYC(0x7e54, 0x7e56); alu_cp(gb, 0x41);
  if (F & FC) { CYCT(0x7e56, 0x7e58); kingMoblin_animate2_hook(gb); return; } // jr nc
  CYC(0x7e56, 0x7e58);
  CYC(0x7e58, 0x7e5a); E = ENEMY_BASE + OBJ_XH;
  CYC(0x7e5a, 0x7e5b); A = mem_rd(gb, DE);
  CYC(0x7e5b, 0x7e5c); B = A;
  CYC(0x7e5c, 0x7e5d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7e5d, 0x7e5f); alu_add(gb, 0x08);
  CYC(0x7e5f, 0x7e61); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(0x7e61, 0x7e63); kingMoblin_grabBomb_hook(gb); return; } // jr c
  CYC(0x7e61, 0x7e63);
  CYC(0x7e63, 0x7e64); A = mem_rd(gb, HL);
  CYC(0x7e64, 0x7e65); alu_cp(gb, B);
  CYC(0x7e65, 0x7e66); H = D;
  CYC(0x7e66, 0x7e68); L = ENEMY_BASE + 0x32; // var32
  CYC(0x7e68, 0x7e69); mem_wr(gb, HL, A);
  CYC(0x7e69, 0x7e6b); B = 0x11; // state $11
  CYC(0x7e6b, 0x7e6e); kingMoblin_setAngleStateAndAnimation_hook(gb); return; // jp
}

void kingMoblin_grabBomb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7e6e, 0x7e70); A = OBJ_STATE; // Object.state
  CALL_C(0x7e70, objectGetRelatedObject2Var_hook, 0x2164, 0x7e73);
  CYC(0x7e73, 0x7e74); A = mem_rd(gb, HL);
  CYC(0x7e74, 0x7e76); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(0x7e76, 0x7e78); goto L_7e80; } // jr z
  CYC(0x7e76, 0x7e78);
  CYC(0x7e78, 0x7e7a); mem_wr(gb, HL, 0x01);
  CYC(0x7e7a, 0x7e7d); SET_BC(0x0800);
  CALL_C(0x7e7d, objectCopyPositionWithOffset_hook, 0x225a, 0x7e80);

L_7e80:
  CYC(0x7e80, 0x7e81); H = D;
  CYC(0x7e81, 0x7e83); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7e83, 0x7e85); mem_wr(gb, HL, 0x0c);
  CYC(0x7e85, 0x7e88); kingMoblin_initBombPickupAnimation_hook(gb); return; // jp
}

// Moving to centre of screen
void kingMoblin_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7e88, 0x7e8a); E = ENEMY_BASE + OBJ_XH;
  CYC(0x7e8a, 0x7e8b); A = mem_rd(gb, DE);
  CYC(0x7e8b, 0x7e8d); alu_sub(gb, 0x4e);
  CYC(0x7e8d, 0x7e8f); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7e8f, 0x7e91); goto L_7e9e; } // jr nc
  CYC(0x7e8f, 0x7e91);
  CYC(0x7e91, 0x7e92); H = D;
  CYC(0x7e92, 0x7e94); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7e94, 0x7e96); mem_wr(gb, HL, 0x0b);
  CYC(0x7e96, 0x7e98); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7e98, 0x7e9a); mem_wr(gb, HL, 30);
  CYC(0x7e9a, 0x7e9b); alu_xor(gb, A);
  CYC(0x7e9b, 0x7e9e); enemySetAnimation_hook(gb); return; // jp

L_7e9e:
  CALL_C(0x7e9e, objectApplySpeed_hook, 0x201d, 0x7ea1);
  kingMoblin_animate2_hook(gb); return; // fallthrough
}

void kingMoblin_animate2_hook(GB *gb) {
  CYC(0x7ea1, 0x7ea4); enemyAnimate_hook(gb); return; // jp
}

// Moving toward bomb
void kingMoblin_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7ea4, kingMoblin_checkMoveToCentre_hook, 0x7f5c, 0x7ea7);
  if (!(F & FZ)) { RET_TAKEN(0x7ea7); return; } // ret nz
  CYC(0x7ea7, 0x7ea8);
  CALL_C(0x7ea8, objectApplySpeed_hook, 0x201d, 0x7eab);
  CYC(0x7eab, 0x7eac); H = D;
  CYC(0x7eac, 0x7eae); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7eae, 0x7eb0); E = ENEMY_BASE + 0x32; // var32
  CYC(0x7eb0, 0x7eb1); A = mem_rd(gb, DE);
  CYC(0x7eb1, 0x7eb2); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7eb2, 0x7eb4); alu_add(gb, 0x08);
  CYC(0x7eb4, 0x7eb6); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(0x7eb6, 0x7eb8); kingMoblin_grabBomb_hook(gb); return; } // jr c
  CYC(0x7eb6, 0x7eb8);
  CYC(0x7eb8, 0x7ebb); enemyAnimate_hook(gb); return; // jp
}

// Just died
void kingMoblin_state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7ebb, objectApplySpeed_hook, 0x201d, 0x7ebe);
  CYC(0x7ebe, 0x7ec0); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7ec0, 0x7ec1); A = mem_rd(gb, DE);
  CYC(0x7ec1, 0x7ec3); alu_cp(gb, 0x0c);
  if (!(F & FC)) { RET_TAKEN(0x7ec3); return; } // ret nc
  CYC(0x7ec3, 0x7ec4);
  CALL_C(0x7ec4, ecom_incState_b0f_hook, 0x4000, 0x7ec7);
  CYC(0x7ec7, 0x7ec9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7ec9, 0x7ecb); mem_wr(gb, HL, 0x10);
  CYC(0x7ecb, 0x7ecd); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7ecd, 0x7ecf); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x7ecf, 0x7ed1); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x7ed1, 0x7ed3); A = 0xa0; // <(-$160)
  CYC(0x7ed3, 0x7ed4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7ed4, 0x7ed6); mem_wr(gb, HL, 0xfe); // >(-$160)
  CYC(0x7ed6, 0x7ed8); A = 60;
  CYC(0x7ed8, 0x7edb); setScreenShakeCounter_hook(gb); return; // jp
}

// Falling to ground
void kingMoblin_state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7edb, 0x7edd); C = 0x20;
  CALL_C(0x7edd, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x7ee0);
  if (!(F & FC)) { CYCT(0x7ee0, 0x7ee3); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x7ee0, 0x7ee3);
  CALL_C(0x7ee3, ecom_incState_b0f_hook, 0x4000, 0x7ee6);
  CYC(0x7ee6, 0x7ee8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7ee8, 0x7eea); mem_wr(gb, HL, 150);
  CYC(0x7eea, 0x7eec); L = ENEMY_BASE + OBJ_YH;
  CYC(0x7eec, 0x7eee); mem_wr(gb, HL, 0x20);
  RET(0x7eee); return; // ret
}

// Wait for signal from ENEMY_KING_MOBLIN_MINION to go to state $15?
void kingMoblin_state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7eef, 0x7ef1); E = ENEMY_BASE + 0x33; // var33
  CYC(0x7ef1, 0x7ef2); A = mem_rd(gb, DE);
  CYC(0x7ef2, 0x7ef3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7ef3, 0x7ef5); goto gotoState15; } // jr nz
  CYC(0x7ef3, 0x7ef5);
  CYC(0x7ef5, 0x7ef8); A = mem_rd(gb, wFrameCounter);
  CYC(0x7ef8, 0x7ef9); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x7ef9); return; } // ret c
  CYC(0x7ef9, 0x7efa);
  CALL_C(0x7efa, ecom_decCounter1_b0f_hook, 0x439a, 0x7efd);
  if (!(F & FZ)) { RET_TAKEN(0x7efd); return; } // ret nz
  CYC(0x7efd, 0x7efe);
  CYC(0x7efe, 0x7f00); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7f00, 0x7f02); mem_wr(gb, HL, 0x0b);
  CYC(0x7f02, 0x7f04); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7f04, 0x7f06); mem_wr(gb, HL, 30);
  CYC(0x7f06, 0x7f07); alu_xor(gb, A);
  CYC(0x7f07, 0x7f0a); enemySetAnimation_hook(gb); return; // jp

gotoState15:
  CALL_C(0x7f0a, ecom_incState_b0f_hook, 0x4000, 0x7f0d);
  CYC(0x7f0d, 0x7f0f); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7f0f, 0x7f11); mem_wr(gb, HL, 98);
  RET(0x7f11); return; // ret
}

// All bombs at top of screen explode, then initiates warp outside
void kingMoblin_state15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7f12, ecom_decCounter2_b0f_hook, 0x43a3, 0x7f15);
  if (F & FZ) { CYCT(0x7f15, 0x7f17); goto warpOutside; } // jr z
  CYC(0x7f15, 0x7f17);
  CYC(0x7f17, 0x7f18); A = mem_rd(gb, HL);
  CYC(0x7f18, 0x7f19); A = alu_dec8(gb, A);
  CYC(0x7f19, 0x7f1b); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x7f1b); return; } // ret nz
  CYC(0x7f1b, 0x7f1c);
  CYC(0x7f1c, 0x7f1d); A = mem_rd(gb, HL);
  CYC(0x7f1d, 0x7f1f); alu_and(gb, 0x60);
  CYC(0x7f1f, 0x7f20); alu_rrca(gb);
  CYC(0x7f20, 0x7f22); A = alu_swap(gb, A);
  CYC(0x7f22, 0x7f25); SET_HL(0x7f53); // @explosionPositions
  CYC(0x7f25, 0x7f26); kingMoblin_addAToHl_from_rst(gb, 0x7f26);
  CYC(0x7f26, 0x7f27); C = mem_rd(gb, HL);
  CYC(0x7f27, 0x7f29); B = 0x08;
  CALL_C(0x7f29, getFreeInteractionSlot_hook, 0x3aef, 0x7f2c);
  if (!(F & FZ)) { RET_TAKEN(0x7f2c); return; } // ret nz
  CYC(0x7f2c, 0x7f2d);
  CYC(0x7f2d, 0x7f2f); mem_wr(gb, HL, 0x56); // INTERAC_EXPLOSION
  CYC(0x7f2f, 0x7f31); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7f31, 0x7f32); mem_wr(gb, HL, B);
  CYC(0x7f32, 0x7f34); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7f34, 0x7f35); mem_wr(gb, HL, C);
  CALL_C(0x7f35, getTileAtPosition_hook, 0x1447, 0x7f38);
  CYC(0x7f38, 0x7f39); C = L;
  CYC(0x7f39, 0x7f3b); A = 0xa1;
  CYC(0x7f3b, 0x7f3e); setTile_hook(gb); return; // jp

warpOutside:
  CYC(0x7f3e, 0x7f41); SET_HL(0xc709); // wPresentRoomFlags+9
  CYC(0x7f41, 0x7f43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(0x7f43, 0x7f45); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(0x7f45, setGlobalFlag_hook, 0x31f9, 0x7f48);
  CYC(0x7f48, 0x7f4a); A = 0x16; // GLOBALFLAG_16
  CALL_C(0x7f4a, setGlobalFlag_hook, 0x31f9, 0x7f4d);
  CYC(0x7f4d, 0x7f50); SET_HL(0x7f57); // @warpDest
  CYC(0x7f50, 0x7f53); setWarpDestVariables_hook(gb); return; // jp
}

// Updates state and angle values to move king moblin to centre of screen, if there is no
// bomb on screen. Sets state to $10 while moving, $0b when reached centre.
// @param[out] zflag nz if state changed
void kingMoblin_checkMoveToCentre_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7f5c, 0x7f5e); A = OBJ_ID; // Object.id
  CALL_C(0x7f5e, objectGetRelatedObject2Var_hook, 0x2164, 0x7f61);
  CYC(0x7f61, 0x7f62); A = mem_rd(gb, HL);
  CYC(0x7f62, 0x7f64); alu_cp(gb, 0x3f); // PART_KING_MOBLIN_BOMB
  if (F & FZ) { RET_TAKEN(0x7f64); return; } // ret z
  CYC(0x7f64, 0x7f65);
  kingMoblin_moveToCentre_hook(gb); return; // fallthrough
}

void kingMoblin_moveToCentre_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7f65, 0x7f66); H = D;
  CYC(0x7f66, 0x7f68); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7f68, 0x7f69); A = mem_rd(gb, HL);
  CYC(0x7f69, 0x7f6b); alu_sub(gb, 0x4e);
  CYC(0x7f6b, 0x7f6d); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7f6d, 0x7f6f); goto moveTowardCentre; } // jr nc
  CYC(0x7f6d, 0x7f6f);
  CYC(0x7f6f, 0x7f71); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7f71, 0x7f73); mem_wr(gb, HL, 30);
  CYC(0x7f73, 0x7f75); B = 0x0b;
  CYC(0x7f75, 0x7f76); alu_xor(gb, A);
  CYCT(0x7f76, 0x7f78); kingMoblin_setStateAndAnimation_hook(gb); return; // jr

moveTowardCentre:
  CYC(0x7f78, 0x7f7a); alu_cp(gb, 0xb0);
  CYC(0x7f7a, 0x7f7c); B = 0x10;
  kingMoblin_setAngleStateAndAnimation_hook(gb); return; // fallthrough
}

void kingMoblin_setAngleStateAndAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7f7c, 0x7f7e); A = 0x08; // ANGLE_RIGHT
  if (!(F & FC)) { CYCT(0x7f7e, 0x7f80); goto L_7f82; } // jr nc
  CYC(0x7f7e, 0x7f80);
  CYC(0x7f80, 0x7f82); A = 0x18; // ANGLE_LEFT

L_7f82:
  CYC(0x7f82, 0x7f84); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7f84, 0x7f85); mem_wr(gb, DE, A);
  CYC(0x7f85, 0x7f87); A = alu_swap(gb, A);
  CYC(0x7f87, 0x7f88); alu_rlca(gb);
  kingMoblin_setStateAndAnimation_hook(gb); return; // fallthrough
}

void kingMoblin_setStateAndAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7f88, 0x7f8a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7f8a, 0x7f8b); mem_wr(gb, HL, B);
  CALL_C(0x7f8b, enemySetAnimation_hook, 0x282b, 0x7f8e);
  CYC(0x7f8e, 0x7f8f); alu_or(gb, D);
  RET(0x7f8f); return; // ret
}
