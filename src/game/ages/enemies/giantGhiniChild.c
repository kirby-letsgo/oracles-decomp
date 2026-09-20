#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void giantGhiniChild_state_stub_hook(GB *gb);
void giantGhiniChild_state_uninitialized_hook(GB *gb);
void giantGhiniChild_state8_hook(GB *gb);
void giantGhiniChild_state9_hook(GB *gb);
void giantGhiniChild_gotoStateA_hook(GB *gb);
void giantGhiniChild_stateA_hook(GB *gb);
void giantGhiniChild_stateB_hook(GB *gb);
void giantGhiniChild_stateC_hook(GB *gb);

static uint16_t giantGhiniChild_jump_table(GB *gb) {
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

static void giantGhiniChild_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_GIANT_GHINI_CHILD
// ==================================================================================================
void enemyCode3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6d4f, 0x6d51); goto normalStatus; } // jr z
  CYC(0x6d4f, 0x6d51);
  CYC(0x6d51, 0x6d53); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6d53); return; } // ret c
  CYC(0x6d53, 0x6d54);
  if (F & FZ) { CYCT(0x6d54, 0x6d56); goto dead; } // jr z
  CYC(0x6d54, 0x6d56);
  CYC(0x6d56, 0x6d57); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6d57, 0x6d5a); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(0x6d57, 0x6d5a);
  CYC(0x6d5a, 0x6d5c); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6d5c, 0x6d5d); A = mem_rd(gb, DE);
  CYC(0x6d5d, 0x6d5f); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(0x6d5f, 0x6d61); goto normalStatus; } // jr nz
  CYC(0x6d5f, 0x6d61);
  CYC(0x6d61, 0x6d62); H = D;
  CYC(0x6d62, 0x6d64); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d64, 0x6d66); mem_wr(gb, HL, 0x0b);
  CYC(0x6d66, 0x6d68); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d68, 0x6d6a); mem_wr(gb, HL, 120);
  CYC(0x6d6a, 0x6d6c); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6d6c, 0x6d6e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x6d6e, 0x6d70); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x6d70, 0x6d72); mem_wr(gb, HL, 0x00);
  CYC(0x6d72, 0x6d74); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6d74, 0x6d75); H = mem_rd(gb, HL);
  CYC(0x6d75, 0x6d77); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6d77, 0x6d79); mem_wr(gb, HL, 0x01);
  CYC(0x6d79, 0x6d7b); goto normalStatus; // jr

dead:
  CYC(0x6d7b, 0x6d7d); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6d7d, 0x6d7e); A = mem_rd(gb, DE);
  CYC(0x6d7e, 0x6d7f); H = A;
  CYC(0x6d7f, 0x6d81); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6d81, 0x6d82); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6d82, 0x6d85); enemyDie_hook(gb); return; // jp

normalStatus:
  CYC(0x6d85, 0x6d87); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6d87, 0x6d88); A = mem_rd(gb, DE);
  CYC(0x6d88, 0x6d89); H = A;
  CYC(0x6d89, 0x6d8b); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6d8b, 0x6d8c); A = mem_rd(gb, HL);
  CYC(0x6d8c, 0x6d8d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6d8d, 0x6d8f); goto dead; } // jr z
  CYC(0x6d8d, 0x6d8f);
  CYC(0x6d8f, 0x6d91); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d91, 0x6d92); A = mem_rd(gb, DE);
  {
    CYC(0x6d92, 0x6d93); push_effect(gb, 0x6d93);
    uint16_t target = giantGhiniChild_jump_table(gb);
    if (target == 0x6dae) { giantGhiniChild_state_uninitialized_hook(gb); return; }
    if (target == 0x6dad) { giantGhiniChild_state_stub_hook(gb); return; }
    if (target == 0x6ddf) { giantGhiniChild_state8_hook(gb); return; }
    if (target == 0x6df8) { giantGhiniChild_state9_hook(gb); return; }
    if (target == 0x6e0d) { giantGhiniChild_stateA_hook(gb); return; }
    if (target == 0x6e1f) { giantGhiniChild_stateB_hook(gb); return; }
    if (target == 0x6e6c) { giantGhiniChild_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6dad, bare global; jump-table target from enemyCode3f.
void giantGhiniChild_state_stub_hook(GB *gb) {
  RET(0x6dad); return; // ret
}

// 0e:6dae, bare global; jump-table target from enemyCode3f.
void giantGhiniChild_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dae, 0x6db0); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6db0, 0x6db1); A = mem_rd(gb, DE);
  CYC(0x6db1, 0x6db3); alu_and(gb, 0x7f);
  CYC(0x6db3, 0x6db4); A = alu_dec8(gb, A);
  CYC(0x6db4, 0x6db7); SET_HL(0x6e86); // giantGhiniChild_spawnOffsets
  CYC(0x6db7, 0x6db8); giantGhiniChild_addDoubleIndexToHl_from_rst(gb, 0x6db8);
  CYC(0x6db8, 0x6dba); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6dba, 0x6dbb); A = mem_rd(gb, DE);
  CYC(0x6dbb, 0x6dbc); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6dbc, 0x6dbd); mem_wr(gb, DE, A);
  CYC(0x6dbd, 0x6dbe); SET_HL(HL + 1); // inc hl
  CYC(0x6dbe, 0x6dc0); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6dc0, 0x6dc1); A = mem_rd(gb, DE);
  CYC(0x6dc1, 0x6dc2); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6dc2, 0x6dc3); mem_wr(gb, DE, A);
  CYC(0x6dc3, 0x6dc5); A = 0x1e; // SPEED_c0
  CALL_C(0x6dc5, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x6dc8);
  CYC(0x6dc8, 0x6dca); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x6dca, 0x6dcc); mem_wr(gb, HL, 0xfc);
  CYC(0x6dcc, 0x6dce); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6dce, 0x6dcf); A = mem_rd(gb, HL);
  CYC(0x6dcf, 0x6dd0); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x6dd0); return; } // ret c
  CYC(0x6dd0, 0x6dd1);
  CYC(0x6dd1, 0x6dd3); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6dd3, 0x6dd5); mem_wr(gb, HL, 0x09);
  CYC(0x6dd5, 0x6dd7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6dd7, 0x6dd9); mem_wr(gb, HL, 30);
  CALL_C(0x6dd9, objectSetVisiblec1_hook, 0x1e3c, 0x6ddc);
  CYC(0x6ddc, 0x6ddf); objectCreatePuff_hook(gb); return; // jp
}

// 0e:6ddf, bare global; jump-table target from enemyCode3f. Waiting for battle to start.
void giantGhiniChild_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ddf, 0x6de1); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6de1, 0x6de2); A = mem_rd(gb, DE);
  CYC(0x6de2, 0x6de3); H = A;
  CYC(0x6de3, 0x6de5); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6de5, 0x6de6); A = mem_rd(gb, HL);
  CYC(0x6de6, 0x6de8); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(0x6de8, 0x6dea); goto battleNotStartedYet; } // jr c
  CYC(0x6de8, 0x6dea);
  CALL_C(0x6dea, giantGhiniChild_gotoStateA_hook, 0x6dfc, 0x6ded);
  CYC(0x6ded, 0x6df0); objectSetVisiblec1_hook(gb); return; // jp

battleNotStartedYet:
  CYC(0x6df0, 0x6df2); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x6df2, 0x6df3); E = L;
  CYC(0x6df3, 0x6df4); A = mem_rd(gb, HL);
  CYC(0x6df4, 0x6df6); alu_or(gb, 0x40);
  CYC(0x6df6, 0x6df7); mem_wr(gb, DE, A);
  RET(0x6df7); return; // ret
}

// 0e:6df8, bare global; jump-table target from enemyCode3f. Just spawned in, will charge
// after [counter1] frames. Falls through into giantGhiniChild_gotoStateA.
void giantGhiniChild_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6df8, ecom_decCounter1_b0e_hook, 0x439a, 0x6dfb);
  if (!(F & FZ)) { RET_TAKEN(0x6dfb); return; } // ret nz
  CYC(0x6dfb, 0x6dfc);
  giantGhiniChild_gotoStateA_hook(gb); return; // fallthrough
}

// 0e:6dfc, bare global; called from giantGhiniChild_state8, also falls into from
// giantGhiniChild_state9.
void giantGhiniChild_gotoStateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dfc, 0x6dfe); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6dfe, 0x6e00); A = 0x0a;
  CYC(0x6e00, 0x6e01); mem_wr(gb, DE, A);
  CYC(0x6e01, 0x6e03); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6e03, 0x6e05); A = 0x05;
  CYC(0x6e05, 0x6e06); mem_wr(gb, DE, A);
  CALL_C(0x6e06, objectGetAngleTowardLink_hook, 0x1e9c, 0x6e09);
  CYC(0x6e09, 0x6e0b); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6e0b, 0x6e0c); mem_wr(gb, DE, A);
  RET(0x6e0c); return; // ret
}

// 0e:6e0d, bare global; jump-table target from enemyCode3f. Charging at Link.
void giantGhiniChild_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6e0d, enemyAnimate_hook, 0x2818, 0x6e10);
  CALL_C(0x6e10, objectApplySpeed_hook, 0x201d, 0x6e13);
  CALL_C(0x6e13, ecom_decCounter1_b0e_hook, 0x439a, 0x6e16);
  if (!(F & FZ)) { RET_TAKEN(0x6e16); return; } // ret nz
  CYC(0x6e16, 0x6e17);
  CYC(0x6e17, 0x6e19); mem_wr(gb, HL, 0x05); // [counter1]
  CALL_C(0x6e19, objectGetAngleTowardLink_hook, 0x1e9c, 0x6e1c);
  CYC(0x6e1c, 0x6e1f); objectNudgeAngleTowards_hook(gb); return; // jp
}

// 0e:6e1f, bare global; jump-table target from enemyCode3f. Attached to Link.
void giantGhiniChild_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6e1f, enemyAnimate_hook, 0x2818, 0x6e22);
  CYC(0x6e22, 0x6e25); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(0x6e25, 0x6e27); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6e27, 0x6e28); mem_wr(gb, DE, A);
  CYC(0x6e28, 0x6e2b); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(0x6e2b, 0x6e2d); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6e2d, 0x6e2e); mem_wr(gb, DE, A);
  CALL_C(0x6e2e, ecom_decCounter1_b0e_hook, 0x439a, 0x6e31);
  if (F & FZ) { CYCT(0x6e31, 0x6e33); goto detach; } // jr z
  CYC(0x6e31, 0x6e33);
  CYC(0x6e33, 0x6e36); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(0x6e36, 0x6e37); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6e37, 0x6e39); goto adjustVisibility; } // jr z
  CYC(0x6e37, 0x6e39);
  CYC(0x6e39, 0x6e3a); A = mem_rd(gb, HL);
  CYC(0x6e3a, 0x6e3c); alu_sub(gb, 0x03); // BTN_A|BTN_B
  if (F & FC) { CYCT(0x6e3c, 0x6e3e); goto storeCounter1; } // jr nc
  CYC(0x6e3c, 0x6e3e);
  CYC(0x6e3e, 0x6e40); A = 0x01;

storeCounter1:
  CYC(0x6e40, 0x6e41); mem_wr(gb, HL, A);

adjustVisibility:
  CYC(0x6e41, 0x6e42); A = mem_rd(gb, HL);
  CYC(0x6e42, 0x6e44); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6e44, 0x6e46); goto slowLink; } // jr nz
  CYC(0x6e44, 0x6e46);
  CYC(0x6e46, 0x6e48); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x6e48, 0x6e49); A = mem_rd(gb, HL);
  CYC(0x6e49, 0x6e4b); alu_xor(gb, 0x80);
  CYC(0x6e4b, 0x6e4c); mem_wr(gb, HL, A);

slowLink:
  CYC(0x6e4c, 0x6e4f); SET_HL(wccd8);
  CYC(0x6e4f, 0x6e51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(0x6e51, 0x6e54); A = mem_rd(gb, wFrameCounter);
  CYC(0x6e54, 0x6e55); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x6e55); return; } // ret nc
  CYC(0x6e55, 0x6e56);
  CYC(0x6e56, 0x6e59); SET_HL(wLinkImmobilized);
  CYC(0x6e59, 0x6e5b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  RET(0x6e5b); return; // ret

detach:
  CYC(0x6e5c, 0x6e5e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6e5e, 0x6e60); mem_wr(gb, HL, 0x0c);
  CYC(0x6e60, 0x6e62); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6e62, 0x6e64); mem_wr(gb, HL, 0x3c); // 60
  CYC(0x6e64, 0x6e66); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6e66, 0x6e67); H = mem_rd(gb, HL);
  CYC(0x6e67, 0x6e69); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6e69, 0x6e6b); mem_wr(gb, HL, 0x00);
  RET(0x6e6b); return; // ret
}

// 0e:6e6c, bare global; jump-table target from enemyCode3f. Just detached from Link, fading
// away.
void giantGhiniChild_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6e6c, enemyAnimate_hook, 0x2818, 0x6e6f);
  CYC(0x6e6f, 0x6e71); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x6e71, 0x6e72); A = mem_rd(gb, DE);
  CYC(0x6e72, 0x6e74); alu_xor(gb, 0x80);
  CYC(0x6e74, 0x6e75); mem_wr(gb, DE, A);
  CALL_C(0x6e75, ecom_decCounter1_b0e_hook, 0x439a, 0x6e78);
  if (!(F & FZ)) { RET_TAKEN(0x6e78); return; } // ret nz
  CYC(0x6e78, 0x6e79);
  CYC(0x6e79, 0x6e7b); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6e7b, 0x6e7c); A = mem_rd(gb, DE);
  CYC(0x6e7c, 0x6e7d); H = A;
  CYC(0x6e7d, 0x6e7f); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6e7f, 0x6e80); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x6e80, decNumEnemies_hook, 0x24b3, 0x6e83);
  CYC(0x6e83, 0x6e86); enemyDelete_hook(gb); return; // jp
}
