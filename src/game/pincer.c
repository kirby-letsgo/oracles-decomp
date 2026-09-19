#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void pincer_state_uninitialized_hook(GB *gb);
void pincer_state1_hook(GB *gb);
void pincer_state_stub_hook(GB *gb);
void pincer_head_hook(GB *gb);
void pincer_head_state8_hook(GB *gb);
void pincer_head_state9_hook(GB *gb);
void pincer_head_stateA_hook(GB *gb);
void pincer_head_stateB_hook(GB *gb);
void pincer_head_stateC_hook(GB *gb);
void pincer_head_stateD_hook(GB *gb);
void pincer_head_stateE_hook(GB *gb);
void pincer_body_hook(GB *gb);
void pincer_setChildRelatedObj1_hook(GB *gb);
void pincer_updatePosition_hook(GB *gb);
void pincer_body_updateExtendedAmount_hook(GB *gb);

static uint16_t pincer_jump_table(GB *gb) {
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

// 0e:5feb, local sub-label of pincer_body_updateExtendedAmount reached by genuine call/ret.
static uint8_t pincer_getExtendedAmountDividedByFour(GB *gb) {
  CYC(0x5feb, 0x5fed); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x5fed, 0x5fee); E = L;
  CYC(0x5fee, 0x5fef); A = mem_rd(gb, HL);
  CYC(0x5fef, 0x5ff1); A = alu_srl(gb, A);
  CYC(0x5ff1, 0x5ff3); A = alu_srl(gb, A);
  CYC(0x5ff3, 0x5ff4);
  return A;
}

// ==================================================================================================
// ENEMY_PINCER
//
// Variables:
//   relatedObj1: Pointer to "head", aka subid 1 (only for body parts, subids 2+)
//   var31/var32: Base Y/X position (where it originates from)
//   var33: Amount extended (0 means still in hole)
//   var34: Copy of parent's "id" value. For body parts only.
// ==================================================================================================
void enemyCode45_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5e5a, 0x5e5c); goto normalStatus; } // jr z
  CYC(0x5e5a, 0x5e5c);
  CYC(0x5e5c, 0x5e5e); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5e5e); return; } // ret c
  CYC(0x5e5e, 0x5e5f);
  if (F & FZ) { CYCT(0x5e5f, 0x5e62); enemyDie_hook(gb); return; } // jp z
  CYC(0x5e5f, 0x5e62);

normalStatus:
  CALL_C(0x5e62, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x5e65);
  if (!(F & FC)) { CYCT(0x5e65, 0x5e67); goto normalState; } // jr nc
  CYC(0x5e65, 0x5e67);
  {
    CYC(0x5e67, 0x5e68); push_effect(gb, 0x5e68);
    uint16_t target = pincer_jump_table(gb);
    if (target == 0x5e83) { pincer_state_uninitialized_hook(gb); return; }
    if (target == 0x5e8a) { pincer_state1_hook(gb); return; }
    if (target == 0x5eb6) { pincer_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x5e78, 0x5e79); B = alu_dec8(gb, B); // dec b
  CYC(0x5e79, 0x5e7a); A = B; // ld a,b
  {
    CYC(0x5e7a, 0x5e7b); push_effect(gb, 0x5e7b);
    uint16_t target = pincer_jump_table(gb);
    if (target == 0x5eb7) { pincer_head_hook(gb); return; }
    if (target == 0x5f69) { pincer_body_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5e83, bare global; jump-table target from enemyCode45. Falls through into pincer_state1
// for the spawner (subid 0).
void pincer_state_uninitialized_hook(GB *gb) {
  CYC(0x5e83, 0x5e84); A = B; // ld a,b
  CYC(0x5e84, 0x5e85); alu_or(gb, A); // or a
  if (!(F & FZ)) { CYCT(0x5e85, 0x5e88); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp nz
  CYC(0x5e85, 0x5e88);
  CYC(0x5e88, 0x5e89); A = alu_inc8(gb, A); // inc a
  CYC(0x5e89, 0x5e8a); mem_wr(gb, DE, A); // [state] = 1
  pincer_state1_hook(gb); return; // fallthrough
}

// 0e:5e8a, bare global; jump-table target from enemyCode45, also falls into from
// pincer_state_uninitialized. Spawner only (subid 0): spawn head and body parts, then delete self.
void pincer_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e8a, 0x5e8c); B = 0x04;
  CALL_C(0x5e8c, checkBEnemySlotsAvailable_hook, 0x20f7, 0x5e8f);
  if (F & FZ) { RET_TAKEN(0x5e8f); return; } // ret nz -- not-z path continues
  CYC(0x5e8f, 0x5e90);
  CYC(0x5e90, 0x5e92); B = 0x45; // ENEMY_PINCER
  CALL_C(0x5e92, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x5e95);
  CYC(0x5e95, 0x5e97); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x5e97, 0x5e98); E = L;
  CYC(0x5e98, 0x5e99); A = mem_rd(gb, DE);
  CYC(0x5e99, 0x5e9a); mem_wr(gb, HL, A);
  CALL_C(0x5e9a, objectCopyPosition_hook, 0x2242, 0x5e9d);
  CYC(0x5e9d, 0x5e9e); C = H;
  CALL_C(0x5e9e, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x5ea1);
  CALL_C(0x5ea1, pincer_setChildRelatedObj1_hook, 0x5fb3, 0x5ea4);
  CALL_C(0x5ea4, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x5ea7);
  CYC(0x5ea7, 0x5ea8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl), [child.subid]
  CALL_C(0x5ea8, pincer_setChildRelatedObj1_hook, 0x5fb3, 0x5eab);
  CALL_C(0x5eab, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x5eae);
  CYC(0x5eae, 0x5eaf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl), [child.subid]
  CYC(0x5eaf, 0x5eb0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl), [child.subid]
  CALL_C(0x5eb0, pincer_setChildRelatedObj1_hook, 0x5fb3, 0x5eb3);
  CYC(0x5eb3, 0x5eb6); enemyDelete_hook(gb); return; // jp
}

// 0e:5eb6, bare global; jump-table target from enemyCode45.
void pincer_state_stub_hook(GB *gb) {
  RET(0x5eb6); return; // ret
}

// 0e:5eb7, bare global; jump-table target from enemyCode45@normalState. Subid 1: head of
// pincer (the "main" part, which is attackable).
void pincer_head_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5eb7, 0x5eb8); A = mem_rd(gb, DE);
  CYC(0x5eb8, 0x5eba); alu_sub(gb, 0x08);
  {
    CYC(0x5eba, 0x5ebb); push_effect(gb, 0x5ebb);
    uint16_t target = pincer_jump_table(gb);
    if (target == 0x5ec9) { pincer_head_state8_hook(gb); return; }
    if (target == 0x5ed7) { pincer_head_state9_hook(gb); return; }
    if (target == 0x5ee5) { pincer_head_stateA_hook(gb); return; }
    if (target == 0x5f21) { pincer_head_stateB_hook(gb); return; }
    if (target == 0x5f37) { pincer_head_stateC_hook(gb); return; }
    if (target == 0x5f3e) { pincer_head_stateD_hook(gb); return; }
    if (target == 0x5f54) { pincer_head_stateE_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5ec9, bare global; jump-table target from pincer_head. Initialization.
void pincer_head_state8_hook(GB *gb) {
  CYC(0x5ec9, 0x5eca); H = D;
  CYC(0x5eca, 0x5ecb); L = E;
  CYC(0x5ecb, 0x5ecc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x5ecc, 0x5ece); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5ece, 0x5ed0); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5ed0, 0x5ed1); A = mem_rd(gb, DE);
  CYC(0x5ed1, 0x5ed2); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5ed2, 0x5ed4); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5ed4, 0x5ed5); A = mem_rd(gb, DE);
  CYC(0x5ed5, 0x5ed6); mem_wr(gb, HL, A);
  RET(0x5ed6); return; // ret
}

// 0e:5ed7, bare global; jump-table target from pincer_head. Waiting for Link to approach.
void pincer_head_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ed7, 0x5ed9); C = 0x28;
  CALL_C(0x5ed9, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x5edc);
  if (!(F & FC)) { RET_TAKEN(0x5edc); return; } // ret nc
  CYC(0x5edc, 0x5edd);
  CYC(0x5edd, 0x5edf); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5edf, 0x5ee1); A = 0x0a;
  CYC(0x5ee1, 0x5ee2); mem_wr(gb, DE, A);
  CYC(0x5ee2, 0x5ee5); objectSetVisible82_hook(gb); return; // jp
}

// 0e:5ee5, bare global; jump-table target from pincer_head. Showing eyes as a "warning" that
// it's about to attack.
void pincer_head_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ee5, 0x5ee7); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5ee7, 0x5ee8); A = mem_rd(gb, DE);
  CYC(0x5ee8, 0x5ee9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5ee9, 0x5eec); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x5ee9, 0x5eec);
  CALL_C(0x5eec, ecom_incState_b0e_hook, 0x4000, 0x5eef);
  CYC(0x5eef, 0x5ef1); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5ef1, 0x5ef3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5ef3, 0x5ef5); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x5ef5, 0x5ef7); mem_wr(gb, HL, 0x00);
  CYC(0x5ef7, 0x5ef9); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5ef9, 0x5efa); B = mem_rd(gb, HL);
  CYC(0x5efa, 0x5efc); L = ENEMY_BASE + OBJ_XH;
  CYC(0x5efc, 0x5efd); C = mem_rd(gb, HL);
  CYC(0x5efd, 0x5eff); A = 0x06; // BREAKABLETILESOURCE_SHOVEL
  CALL_C(0x5eff, tryToBreakTile_hook, 0x2bf6, 0x5f02);
  CALL_C(0x5f02, objectCheckTileAtPositionIsWater_hook, 0x22a9, 0x5f05);
  if (!(F & FC)) { CYCT(0x5f05, 0x5f07); goto attack; } // jr nc
  CYC(0x5f05, 0x5f07);
  CALL_C(0x5f07, getFreeInteractionSlot_hook, 0x3aef, 0x5f0a);
  if (!(F & FZ)) { CYCT(0x5f0a, 0x5f0c); goto attack; } // jr nz
  CYC(0x5f0a, 0x5f0c);
  CYC(0x5f0c, 0x5f0e); mem_wr(gb, HL, 0x03); // INTERAC_SPLASH
  CYC(0x5f0e, 0x5f11); SET_BC(0xfa00);
  CALL_C(0x5f11, objectCopyPositionWithOffset_hook, 0x225a, 0x5f14);

attack:
  CALL_C(0x5f14, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5f17);
  CYC(0x5f17, 0x5f19); alu_add(gb, 0x02);
  CYC(0x5f19, 0x5f1b); alu_and(gb, 0x1c);
  CYC(0x5f1b, 0x5f1c); alu_rrca(gb);
  CYC(0x5f1c, 0x5f1d); alu_rrca(gb);
  CYC(0x5f1d, 0x5f1e); A = alu_inc8(gb, A);
  CYC(0x5f1e, 0x5f21); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5f21, bare global; jump-table target from pincer_head. Extending toward target.
void pincer_head_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f21, pincer_updatePosition_hook, 0x5fbb, 0x5f24);
  CYC(0x5f24, 0x5f26); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x5f26, 0x5f27); A = mem_rd(gb, DE);
  CYC(0x5f27, 0x5f29); alu_add(gb, 0x02);
  CYC(0x5f29, 0x5f2b); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(0x5f2b, 0x5f2d); goto fullyExtended; } // jr nc
  CYC(0x5f2b, 0x5f2d);
  CYC(0x5f2d, 0x5f2e); mem_wr(gb, DE, A);
  RET(0x5f2e); return; // ret

fullyExtended:
  CALL_C(0x5f2f, ecom_incState_b0e_hook, 0x4000, 0x5f32);
  CYC(0x5f32, 0x5f34); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5f34, 0x5f36); mem_wr(gb, HL, 0x08);
  RET(0x5f36); return; // ret
}

// 0e:5f37, bare global; jump-table target from pincer_head. Staying fully extended for
// several frames.
void pincer_head_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f37, ecom_decCounter1_b0e_hook, 0x439a, 0x5f3a);
  if (!(F & FZ)) { RET_TAKEN(0x5f3a); return; } // ret nz
  CYC(0x5f3a, 0x5f3b);
  CYC(0x5f3b, 0x5f3c); L = E;
  CYC(0x5f3c, 0x5f3d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  RET(0x5f3d); return; // ret
}

// 0e:5f3e, bare global; jump-table target from pincer_head. Retracting.
void pincer_head_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f3e, pincer_updatePosition_hook, 0x5fbb, 0x5f41);
  CYC(0x5f41, 0x5f42); H = D;
  CYC(0x5f42, 0x5f44); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x5f44, 0x5f45); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x5f45); return; } // ret nz
  CYC(0x5f45, 0x5f46);
  CYC(0x5f46, 0x5f48); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5f48, 0x5f4a); mem_wr(gb, HL, 30);
  CYC(0x5f4a, 0x5f4c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5f4c, 0x5f4d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5f4d, 0x5f4f); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5f4f, 0x5f51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x5f51, 0x5f54); objectSetInvisible_hook(gb); return; // jp
}

// 0e:5f54, bare global; jump-table target from pincer_head. Fully retracted; on cooldown.
void pincer_head_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f54, ecom_decCounter1_b0e_hook, 0x439a, 0x5f57);
  if (!(F & FZ)) { RET_TAKEN(0x5f57); return; } // ret nz
  CYC(0x5f57, 0x5f58);
  CYC(0x5f58, 0x5f59); L = E;
  CYC(0x5f59, 0x5f5b); mem_wr(gb, HL, 0x09); // [state]
  CYC(0x5f5b, 0x5f5d); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5f5d, 0x5f5f); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5f5f, 0x5f60); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5f60, 0x5f61); mem_wr(gb, DE, A);
  CYC(0x5f61, 0x5f63); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5f63, 0x5f64); A = mem_rd(gb, HL);
  CYC(0x5f64, 0x5f65); mem_wr(gb, DE, A);
  CYC(0x5f65, 0x5f66); alu_xor(gb, A);
  CYC(0x5f66, 0x5f69); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5f69, bare global; jump-table target from enemyCode45@normalState. Subid 2-4: body of
// pincer (just decoration).
void pincer_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f69, 0x5f6a); A = mem_rd(gb, DE);
  CYC(0x5f6a, 0x5f6c); alu_sub(gb, 0x08);
  {
    CYC(0x5f6c, 0x5f6d); push_effect(gb, 0x5f6d);
    uint16_t target = pincer_jump_table(gb);
    if (target == 0x5f71) goto state8;
    if (target == 0x5f8c) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(0x5f71, 0x5f73); A = 0x09;
  CYC(0x5f73, 0x5f74); mem_wr(gb, DE, A); // [state] = 9
  CYC(0x5f74, 0x5f76); A = OBJ_YH; // Object.yh
  CALL_C(0x5f76, objectGetRelatedObject1Var_hook, 0x2160, 0x5f79);
  CYC(0x5f79, 0x5f7b); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5f7b, 0x5f7c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5f7c, 0x5f7d); mem_wr(gb, DE, A);
  CYC(0x5f7d, 0x5f7e); L = alu_inc8(gb, L);
  CYC(0x5f7e, 0x5f7f); E = alu_inc8(gb, E);
  CYC(0x5f7f, 0x5f80); A = mem_rd(gb, HL);
  CYC(0x5f80, 0x5f81); mem_wr(gb, DE, A);
  CYC(0x5f81, 0x5f83); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x5f83, 0x5f85); L = ENEMY_BASE + OBJ_ID;
  CYC(0x5f85, 0x5f86); A = mem_rd(gb, HL);
  CYC(0x5f86, 0x5f87); mem_wr(gb, DE, A);
  CYC(0x5f87, 0x5f89); A = 0x09;
  CYC(0x5f89, 0x5f8c); enemySetAnimation_hook(gb); return; // jp

state9:
  CYC(0x5f8c, 0x5f8e); A = OBJ_ID; // Object.id
  CALL_C(0x5f8e, objectGetRelatedObject1Var_hook, 0x2160, 0x5f91);
  CYC(0x5f91, 0x5f93); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x5f93, 0x5f94); A = mem_rd(gb, DE);
  CYC(0x5f94, 0x5f95); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5f95, 0x5f98); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x5f95, 0x5f98);
  CYC(0x5f98, 0x5f9a); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5f9a, 0x5f9b); E = L;
  CYC(0x5f9b, 0x5f9c); A = mem_rd(gb, HL);
  CYC(0x5f9c, 0x5f9d); mem_wr(gb, DE, A);
  CYC(0x5f9d, 0x5f9f); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x5f9f, 0x5fa0); E = L;
  CYC(0x5fa0, 0x5fa1); A = mem_rd(gb, HL);
  CYC(0x5fa1, 0x5fa2); mem_wr(gb, DE, A);
  CYC(0x5fa2, 0x5fa4); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5fa4, 0x5fa5); A = mem_rd(gb, HL);
  CYC(0x5fa5, 0x5fa7); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(0x5fa7, 0x5fa9); goto updateExtendedAmount; } // jr c
  CYC(0x5fa7, 0x5fa9);
  CYC(0x5fa9, 0x5fab); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x5fab, 0x5fac); E = L;
  CYC(0x5fac, 0x5fad); A = mem_rd(gb, HL);
  CYC(0x5fad, 0x5fae); mem_wr(gb, DE, A);

updateExtendedAmount:
  CALL_C(0x5fae, pincer_body_updateExtendedAmount_hook, 0x5fc8, 0x5fb1);
  CYC(0x5fb1, 0x5fb3); pincer_updatePosition_hook(gb); return; // jr
}

// 0e:5fb3, bare global; called from pincer_state1 for each of the 3 body parts. Sets
// relatedObj1 of object 'h' to object 'c'. 'h' is part of the pincer's body, 'c' is the
// pincer's head. Also increments the body part's subid since that does need to be done.
void pincer_setChildRelatedObj1_hook(GB *gb) {
  CYC(0x5fb3, 0x5fb4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid]++
  CYC(0x5fb4, 0x5fb6); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5fb6, 0x5fb8); A = ENEMY_BASE; // Enemy.start
  CYC(0x5fb8, 0x5fb9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5fb9, 0x5fba); mem_wr(gb, HL, C);
  RET(0x5fba); return; // ret
}

// 0e:5fbb, bare global; called from pincer_body and both pincer_head extend/retract states.
// Updates position based on "base position" (var31), angle, and distance extended (var33).
void pincer_updatePosition_hook(GB *gb) {
  CYC(0x5fbb, 0x5fbc); H = D;
  CYC(0x5fbc, 0x5fbe); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5fbe, 0x5fbf); B = mem_rd(gb, HL);
  CYC(0x5fbf, 0x5fc0); L = alu_inc8(gb, L);
  CYC(0x5fc0, 0x5fc1); C = mem_rd(gb, HL);
  CYC(0x5fc1, 0x5fc2); L = alu_inc8(gb, L);
  CYC(0x5fc2, 0x5fc3); A = mem_rd(gb, HL);
  CYC(0x5fc3, 0x5fc5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5fc5, 0x5fc8); objectSetPositionInCircleArc_hook(gb); return; // jp
}

// 0e:5fc8, bare global; called from pincer_body. Calculates value for var33 (amount extended)
// for a body part.
void pincer_body_updateExtendedAmount_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  PUSH(0x5fc8, HL);
  CYC(0x5fc9, 0x5fcb); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5fcb, 0x5fcc); A = mem_rd(gb, DE);
  CYC(0x5fcc, 0x5fce); alu_sub(gb, 0x02);
  {
    CYC(0x5fce, 0x5fcf); push_effect(gb, 0x5fcf);
    uint16_t target = pincer_jump_table(gb);
    if (target == 0x5fd5) goto subid2;
    if (target == 0x5fde) goto subid3;
    if (target == 0x5fe5) goto subid4;
    HANDOFF(target);
  }

subid2:
  SET_HL(POP(0x5fd5));
  CYC(0x5fd6, 0x5fd9); A = pincer_getExtendedAmountDividedByFour(gb);
  CYC(0x5fd9, 0x5fda); B = A;
  CYC(0x5fda, 0x5fdb); alu_add(gb, A); // add a,a
  CYC(0x5fdb, 0x5fdc); alu_add(gb, B); // add a,b
  CYC(0x5fdc, 0x5fdd); mem_wr(gb, DE, A);
  RET(0x5fdd); return; // ret

subid3:
  SET_HL(POP(0x5fde));
  CYC(0x5fdf, 0x5fe2); A = pincer_getExtendedAmountDividedByFour(gb);
  CYC(0x5fe2, 0x5fe3); alu_add(gb, A); // add a,a
  CYC(0x5fe3, 0x5fe4); mem_wr(gb, DE, A);
  RET(0x5fe4); return; // ret

subid4:
  SET_HL(POP(0x5fe5));
  CYC(0x5fe6, 0x5fe9); A = pincer_getExtendedAmountDividedByFour(gb);
  CYC(0x5fe9, 0x5fea); mem_wr(gb, DE, A);
  RET(0x5fea); return; // ret
}
