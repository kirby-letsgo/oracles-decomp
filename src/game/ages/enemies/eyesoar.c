#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void enemyCode7b_hook(GB *gb);
void eyesoar_state_uninitialized_hook(GB *gb);
void eyesoar_state1_hook(GB *gb);
void eyesoar_state_switchHook_hook(GB *gb);
void eyesoar_state_stub_hook(GB *gb);
void eyesoar_state8_hook(GB *gb);
void eyesoar_state9_hook(GB *gb);
void eyesoar_stateA_hook(GB *gb);
void eyesoar_stateB_hook(GB *gb);
void eyesoar_animate_hook(GB *gb);
void eyesoar_stateC_hook(GB *gb);
void eyesoar_stateD_hook(GB *gb);
void eyesoar_stateE_hook(GB *gb);
void eyesoar_updateFormation_hook(GB *gb);
void eyesoar_dead_hook(GB *gb);
void eyesoar_chooseNewAngle_hook(GB *gb);

static uint16_t eyesoar_jump_table(GB *gb) {
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

static void eyesoar_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_EYESOAR
//
// Variables:
//   var30-var35: Object indices of children
//   var36/var37: Target Y/X position for state $0b
//   var38: The distance each child should be from Eyesoar (the value they're moving
//          toward)
//   var39: Bit 4: Set when children should return?
//          Bit 3: Unset to make the children start moving back to Eyesoar (after using
//                 switch hook on him)
//          Bit 2: Set while eyesoar is in his "dazed" state (Signals children to start
//                 moving around randomly)
//          Bit 1: Set to indicate the children should start moving again as normal after
//                 returning to Eyesoar
//          Bit 0: While set, children don't respawn?
//   var3a: Bits 0-3: set when corresponding children have reached their target distance
//                    away from eyesoar?
//          Bits 4-7: set when corresponding children have reached their target position
//                    relative to eyesoar after using the switch hook on him?
//   var3b: Current "angle" (rotation offset for children)
//   var3c: Counter until bit 0 of var39 gets reset
// ==================================================================================================

void enemyCode7b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x6e4f, 0x6e51); goto normalStatus; } // jr z
  CYC(0x6e4f, 0x6e51);
  CYC(0x6e51, 0x6e53); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6e53); return; } // ret c
  CYC(0x6e53, 0x6e54);
  if (F & FZ) { CYCT(0x6e54, 0x6e57); eyesoar_dead_hook(gb); return; } // jp z
  CYC(0x6e54, 0x6e57);
  CYC(0x6e57, 0x6e58); H = D;
  CYC(0x6e58, 0x6e5a); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6e5a, 0x6e5b); A = mem_rd(gb, HL);
  CYC(0x6e5b, 0x6e5d); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (!(F & FZ)) { CYCT(0x6e5d, 0x6e5f); goto normalStatus; } // jr nz
  CYC(0x6e5d, 0x6e5f);
  CYC(0x6e5f, 0x6e61); L = ENEMY_BASE + 0x3c; // Enemy.var3c
  CYC(0x6e61, 0x6e63); mem_wr(gb, HL, 0x78);
  CYC(0x6e63, 0x6e65); L = ENEMY_BASE + OBJ_VAR39;
  CYC(0x6e65, 0x6e67); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)

normalStatus:
  CYC(0x6e67, 0x6e68); H = D;
  CYC(0x6e68, 0x6e6a); L = ENEMY_BASE + 0x3c; // Enemy.var3c
  CYC(0x6e6a, 0x6e6b); A = mem_rd(gb, HL);
  CYC(0x6e6b, 0x6e6c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6e6c, 0x6e6e); goto L_6e75; } // jr z
  CYC(0x6e6c, 0x6e6e);
  CYC(0x6e6e, 0x6e6f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x6e6f, 0x6e71); goto L_6e75; } // jr nz
  CYC(0x6e6f, 0x6e71);
  CYC(0x6e71, 0x6e73); L = ENEMY_BASE + OBJ_VAR39;
  CYC(0x6e73, 0x6e75); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0))); // res 0,(hl)

L_6e75:
  CYC(0x6e75, 0x6e77); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6e77, 0x6e78); A = mem_rd(gb, DE);
  CYC(0x6e78, 0x6e79); push_effect(gb, 0x6e79);
  {
    uint16_t target = eyesoar_jump_table(gb);
    if (target == 0x6e97) { eyesoar_state_uninitialized_hook(gb); return; }
    if (target == 0x6eb2) { eyesoar_state1_hook(gb); return; }
    if (target == 0x6f18) { eyesoar_state_stub_hook(gb); return; }
    if (target == 0x6ee1) { eyesoar_state_switchHook_hook(gb); return; }
    if (target == 0x6f19) { eyesoar_state8_hook(gb); return; }
    if (target == 0x6f35) { eyesoar_state9_hook(gb); return; }
    if (target == 0x6f44) { eyesoar_stateA_hook(gb); return; }
    if (target == 0x6f78) { eyesoar_stateB_hook(gb); return; }
    if (target == 0x6fa0) { eyesoar_stateC_hook(gb); return; }
    if (target == 0x6faf) { eyesoar_stateD_hook(gb); return; }
    if (target == 0x6fcf) { eyesoar_stateE_hook(gb); return; }
    HANDOFF(target);
  }
}

void eyesoar_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e97, 0x6e98); H = D;
  CYC(0x6e98, 0x6e9a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6e9a, 0x6e9c); mem_wr(gb, HL, 0x3c); // 60
  CYC(0x6e9c, 0x6e9e); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6e9e, 0x6ea0); mem_wr(gb, HL, 0xfe);
  CYC(0x6ea0, 0x6ea2); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6ea2, 0x6ea3); A = mem_rd(gb, HL);
  CYC(0x6ea3, 0x6ea4); alu_or(gb, A);
  CYC(0x6ea4, 0x6ea6); A = 0x14; // SPEED_80
  if (!(F & FZ)) { CYCT(0x6ea6, 0x6ea9); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(0x6ea6, 0x6ea9);
  CYC(0x6ea9, 0x6eaa); A = alu_inc8(gb, A);
  CYC(0x6eaa, 0x6eab); mem_wr(gb, DE, A); // [state] = $15 (bug: invalid state)
  CYC(0x6eab, 0x6ead); A = 0xff;
  CYC(0x6ead, 0x6eaf); B = 0x00;
  CALL_C(0x6eaf, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x6eb2);
  eyesoar_state1_hook(gb); return; // fallthrough
}

// Spawning "real" eyesoar and children.
void eyesoar_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6eb2, 0x6eb4); B = 0x05;
  CALL_C(0x6eb4, checkBEnemySlotsAvailable_hook, 0x20f7, 0x6eb7);
  if (!(F & FZ)) { RET_TAKEN(0x6eb7); return; } // ret nz
  CYC(0x6eb7, 0x6eb8);
  CYC(0x6eb8, 0x6eba); B = 0x7b; // ENEMY_EYESOAR
  CALL_C(0x6eba, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x6ebd);
  CYC(0x6ebd, 0x6ebf); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(0x6ebf, 0x6ec0); E = L;
  CYC(0x6ec0, 0x6ec1); A = mem_rd(gb, DE);
  CYC(0x6ec1, 0x6ec2); mem_wr(gb, HL, A);
  CALL_C(0x6ec2, objectCopyPosition_hook, 0x2242, 0x6ec5);
  CYC(0x6ec5, 0x6ec7); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6ec7, 0x6ec8); B = H;
  CYC(0x6ec8, 0x6eca); C = 0x04;

spawnChildLoop:
  CYC(0x6eca, 0x6ecb); push_effect(gb, HL);
  CALL_C(0x6ecb, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x6ece);
  CYC(0x6ece, 0x6ed0); mem_wr(gb, HL, 0x11); // ENEMY_EYESOAR_CHILD
  CYC(0x6ed0, 0x6ed1); L = alu_inc8(gb, L);
  CYC(0x6ed1, 0x6ed2); C = alu_dec8(gb, C);
  CYC(0x6ed2, 0x6ed3); mem_wr(gb, HL, C); // [child.subid]
  CYC(0x6ed3, 0x6ed5); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x6ed5, 0x6ed6); A = B;
  CYC(0x6ed6, 0x6ed7); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x6ed7, 0x6ed9); mem_wr(gb, HL, ENEMY_BASE); // Enemy.start
  CYC(0x6ed9, 0x6eda); A = H;
  SET_HL(pop_effect(gb));
  CYC(0x6eda, 0x6edb);
  CYC(0x6edb, 0x6edc); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a [var30+i]
  if (!(F & FZ)) { CYCT(0x6edc, 0x6ede); goto spawnChildLoop; } // jr nz
  CYC(0x6edc, 0x6ede);
  CYC(0x6ede, 0x6ee1); enemyDelete_hook(gb); return; // jp
}

void eyesoar_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ee1, 0x6ee2); E = alu_inc8(gb, E);
  CYC(0x6ee2, 0x6ee3); A = mem_rd(gb, DE);
  CYC(0x6ee3, 0x6ee4); push_effect(gb, 0x6ee4);
  {
    uint16_t target = eyesoar_jump_table(gb);
    if (target == 0x6eec) goto substate0;
    if (target == 0x6f08) goto substate1;
    if (target == 0x6f09) goto substate2;
    if (target == 0x6f13) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x6eec, 0x6eed); H = D;
  CYC(0x6eed, 0x6eef); L = ENEMY_BASE + OBJ_VAR39;
  CYC(0x6eef, 0x6ef0); A = mem_rd(gb, HL);
  CYC(0x6ef0, 0x6ef2); alu_or(gb, 0x0a);
  CYC(0x6ef2, 0x6ef3); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x6ef3, 0x6ef4); A = mem_rd(gb, HL);
  CYC(0x6ef4, 0x6ef6); alu_and(gb, 0x07);
  CYC(0x6ef6, 0x6ef8); alu_or(gb, 0x18);
  CYC(0x6ef8, 0x6ef9); mem_wr(gb, HL, A);
  CYC(0x6ef9, 0x6efb); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6efb, 0x6efd); mem_wr(gb, HL, 0x4c); // ENEMYCOLLISION_EYESOAR_VULNERABLE
  CYC(0x6efd, 0x6eff); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6eff, 0x6f01); mem_wr(gb, HL, 0x96); // 150
  CYC(0x6f01, 0x6f03); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6f03, 0x6f05); mem_wr(gb, HL, 0x00);
  CYC(0x6f05, 0x6f08); ecom_incSubstate_b0f_hook(gb); return; // jp

substate1:
  RET(0x6f08); return; // ret

substate2:
  CYC(0x6f09, 0x6f0b); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6f0b, 0x6f0c); A = mem_rd(gb, DE);
  CYC(0x6f0c, 0x6f0d); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6f0d); return; } // ret nz
  CYC(0x6f0d, 0x6f0e);
  CYC(0x6f0e, 0x6f0f); A = alu_inc8(gb, A);
  CYC(0x6f0f, 0x6f10); mem_wr(gb, DE, A);
  CYC(0x6f10, 0x6f13); enemySetAnimation_hook(gb); return; // jp

substate3:
  CYC(0x6f13, 0x6f15); B = 0x0c;
  CYC(0x6f15, 0x6f18); ecom_fallToGroundAndSetState_b0f_hook(gb); return; // jp
}

void eyesoar_state_stub_hook(GB *gb) {
  RET(0x6f18); return; // ret
}

// Flickering into existence
void eyesoar_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f19, 0x6f1c); A = mem_rd(gb, wcc93);
  CYC(0x6f1c, 0x6f1d); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6f1d); return; } // ret nz
  CYC(0x6f1d, 0x6f1e);
  CYC(0x6f1e, 0x6f1f); A = alu_inc8(gb, A);
  CYC(0x6f1f, 0x6f22); mem_wr(gb, wDisabledObjects, A);
  CALL_C(0x6f22, ecom_decCounter1_b0f_hook, 0x439a, 0x6f25);
  if (!(F & FZ)) { CYCT(0x6f25, 0x6f28); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(0x6f25, 0x6f28);
  CYC(0x6f28, 0x6f2a); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(0x6f2a, 0x6f2b); L = alu_inc8(gb, L);
  CYC(0x6f2b, 0x6f2d); mem_wr(gb, HL, 0xb4); // [counter2] = 180
  CYC(0x6f2d, 0x6f2f); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(0x6f2f, 0x6f30); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x6f30, 0x6f31); L = E;
  CYC(0x6f31, 0x6f32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6f32, 0x6f35); objectSetVisiblec2_hook(gb); return; // jp
}

// Waiting [counter1] frames until fight begins
void eyesoar_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f35, ecom_decCounter1_b0f_hook, 0x439a, 0x6f38);
  if (!(F & FZ)) { CYCT(0x6f38, 0x6f3a); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(0x6f38, 0x6f3a);
  CYC(0x6f3a, 0x6f3b); L = E;
  CYC(0x6f3b, 0x6f3c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6f3c, 0x6f3e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6f3e, 0x6f3f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(0x6f3f, enemyBoss_beginBoss_b0f_hook, 0x4584, 0x6f42);
  CYCT(0x6f42, 0x6f44); eyesoar_animate_hook(gb); return; // jr
}

// Standing still for [counter1] frames?
void eyesoar_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f44, eyesoar_updateFormation_hook, 0x6ffa, 0x6f47);
  CALL_C(0x6f47, ecom_decCounter1_b0f_hook, 0x439a, 0x6f4a);
  if (!(F & FZ)) { CYCT(0x6f4a, 0x6f4c); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(0x6f4a, 0x6f4c);
  CYC(0x6f4c, 0x6f4e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6f4e, 0x6f4f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6f4f, 0x6f51); L = ENEMY_BASE + 0x36; // Enemy.var36
  CYC(0x6f51, 0x6f53); A = H8(hEnemyTargetY);
  CYC(0x6f53, 0x6f54); B = A;
  CYC(0x6f54, 0x6f56); alu_sub(gb, 0x40);
  CYC(0x6f56, 0x6f58); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(0x6f58, 0x6f5a); goto L_6f62; } // jr c
  CYC(0x6f58, 0x6f5a);
  CYC(0x6f5a, 0x6f5c); alu_cp(gb, 0xc0);
  CYC(0x6f5c, 0x6f5e); B = 0x40;
  if (!(F & FC)) { CYCT(0x6f5e, 0x6f60); goto L_6f62; } // jr nc
  CYC(0x6f5e, 0x6f60);
  CYC(0x6f60, 0x6f62); B = 0x70;

L_6f62:
  CYC(0x6f62, 0x6f63); A = B;
  CYC(0x6f63, 0x6f64); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a [var36]
  CYC(0x6f64, 0x6f66); A = H8(hEnemyTargetX);
  CYC(0x6f66, 0x6f67); B = A;
  CYC(0x6f67, 0x6f69); alu_sub(gb, 0x40);
  CYC(0x6f69, 0x6f6b); alu_cp(gb, 0x70);
  if (F & FC) { CYCT(0x6f6b, 0x6f6d); goto L_6f75; } // jr c
  CYC(0x6f6b, 0x6f6d);
  CYC(0x6f6d, 0x6f6f); alu_cp(gb, 0xc0);
  CYC(0x6f6f, 0x6f71); B = 0x40;
  if (!(F & FC)) { CYCT(0x6f71, 0x6f73); goto L_6f75; } // jr nc
  CYC(0x6f71, 0x6f73);
  CYC(0x6f73, 0x6f75); B = 0xb0;

L_6f75:
  CYC(0x6f75, 0x6f76); mem_wr(gb, HL, B); // [var37]
  CYCT(0x6f76, 0x6f78); eyesoar_animate_hook(gb); return; // jr
}

// Moving until it reaches its target position
void eyesoar_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f78, eyesoar_updateFormation_hook, 0x6ffa, 0x6f7b);
  CYC(0x6f7b, 0x6f7c); H = D;
  CYC(0x6f7c, 0x6f7e); L = ENEMY_BASE + 0x36; // Enemy.var36
  CALL_C(0x6f7e, ecom_readPositionVars_b0f_hook, 0x4439, 0x6f81);
  CYC(0x6f81, 0x6f82); alu_sub(gb, C);
  CYC(0x6f82, 0x6f84); alu_add(gb, 0x02);
  CYC(0x6f84, 0x6f86); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x6f86, 0x6f88); goto L_6f9a; } // jr nc
  CYC(0x6f86, 0x6f88);
  CYC(0x6f88, 0x6f8a); A = H8(hFF8F);
  CYC(0x6f8a, 0x6f8b); alu_sub(gb, B);
  CYC(0x6f8b, 0x6f8d); alu_add(gb, 0x02);
  CYC(0x6f8d, 0x6f8f); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x6f8f, 0x6f91); goto L_6f9a; } // jr nc
  CYC(0x6f8f, 0x6f91);
  CYC(0x6f91, 0x6f93); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6f93, 0x6f94); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x6f94, 0x6f96); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6f96, 0x6f98); mem_wr(gb, HL, 0x3c); // 60
  CYCT(0x6f98, 0x6f9a); eyesoar_animate_hook(gb); return; // jr

L_6f9a:
  CALL_C(0x6f9a, ecom_moveTowardPosition_b0f_hook, 0x4430, 0x6f9d);
  eyesoar_animate_hook(gb); return; // fallthrough
}

void eyesoar_animate_hook(GB *gb) {
  CYC(0x6f9d, 0x6fa0); enemyAnimate_hook(gb); return; // jp
}

// Spinning in place after being switch hook'd
void eyesoar_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6fa0, ecom_decCounter1_b0f_hook, 0x439a, 0x6fa3);
  if (!(F & FZ)) { CYCT(0x6fa3, 0x6fa5); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(0x6fa3, 0x6fa5);
  CYC(0x6fa5, 0x6fa6); L = E;
  CYC(0x6fa6, 0x6fa7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6fa7, 0x6fa9); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x6fa9, 0x6fab); mem_wr(gb, HL, 0x6d); // ENEMYCOLLISION_EYESOAR
  CYC(0x6fab, 0x6fac); alu_xor(gb, A);
  CALL_C(0x6fac, enemySetAnimation_hook, 0x282b, 0x6faf);
  eyesoar_stateD_hook(gb); return; // fallthrough
}

// Moving back up into the air
void eyesoar_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6faf, 0x6fb0); H = D;
  CYC(0x6fb0, 0x6fb2); L = ENEMY_BASE + OBJ_Z;
  CYC(0x6fb2, 0x6fb3); A = mem_rd(gb, HL);
  CYC(0x6fb3, 0x6fb5); alu_sub(gb, 0x80);
  CYC(0x6fb5, 0x6fb6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6fb6, 0x6fb7); A = mem_rd(gb, HL);
  CYC(0x6fb7, 0x6fb9); alu_sbc(gb, 0x00);
  CYC(0x6fb9, 0x6fba); mem_wr(gb, HL, A);
  CYC(0x6fba, 0x6fbc); alu_cp(gb, 0xfe);
  if (!(F & FZ)) { CYCT(0x6fbc, 0x6fbe); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(0x6fbc, 0x6fbe);
  CYC(0x6fbe, 0x6fbf); L = E;
  CYC(0x6fbf, 0x6fc0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6fc0, 0x6fc2); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6fc2, 0x6fc4); mem_wr(gb, HL, 0xf0); // 240
  CYC(0x6fc4, 0x6fc6); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(0x6fc6, 0x6fc8); mem_wr(gb, HL, 0x0f);
  CYC(0x6fc8, 0x6fca); L = ENEMY_BASE + OBJ_VAR39;
  CYC(0x6fca, 0x6fcc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CALL_C(0x6fcc, eyesoar_chooseNewAngle_hook, 0x7057, 0x6fcf);
  eyesoar_stateE_hook(gb); return; // fallthrough
}

// Flying around kinda randomly
void eyesoar_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6fcf, ecom_decCounter1_b0f_hook, 0x439a, 0x6fd2);
  if (!(F & FZ)) { CYCT(0x6fd2, 0x6fd4); goto L_6fda; } // jr nz
  CYC(0x6fd2, 0x6fd4);
  CYC(0x6fd4, 0x6fd6); L = ENEMY_BASE + OBJ_VAR39;
  CYC(0x6fd6, 0x6fd8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 3))); // res 3,(hl)
  CYC(0x6fd8, 0x6fda); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)

L_6fda:
  CYC(0x6fda, 0x6fdc); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(0x6fdc, 0x6fdd); A = mem_rd(gb, HL);
  CYC(0x6fdd, 0x6fde); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x6fde, 0x6fe0); goto L_6fed; } // jr nz
  CYC(0x6fde, 0x6fe0);
  CYC(0x6fe0, 0x6fe1); L = alu_dec8(gb, L);
  CYC(0x6fe1, 0x6fe3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4))); // res 4,(hl) [var39]
  CYC(0x6fe3, 0x6fe4); L = E;
  CYC(0x6fe4, 0x6fe6); mem_wr(gb, HL, 0x0a); // [state]
  CYC(0x6fe6, 0x6fe8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6fe8, 0x6fea); mem_wr(gb, HL, 0x01);
  CYC(0x6fea, 0x6feb); L = alu_inc8(gb, L);
  CYC(0x6feb, 0x6fed); mem_wr(gb, HL, 0x08); // [counter2]

L_6fed:
  CYC(0x6fed, 0x6fef); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6fef, 0x6ff0); A = mem_rd(gb, HL);
  CYC(0x6ff0, 0x6ff2); alu_and(gb, 0x3f);
  if (F & FZ) CALL_C_CC(0x6ff2, eyesoar_chooseNewAngle_hook, 0x7057, 0x6ff5); else CYC(0x6ff2, 0x6ff5); // call z
  CALL_C(0x6ff5, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, 0x4156, 0x6ff8);
  CYCT(0x6ff8, 0x6ffa); eyesoar_animate_hook(gb); return; // jr
}

// Checks to update the "formation", that is, the distances away from Eyesoar for the
// children.
void eyesoar_updateFormation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ffa, 0x6ffc); E = ENEMY_BASE + OBJ_VAR3A;
  CYC(0x6ffc, 0x6ffd); A = mem_rd(gb, DE);
  CYC(0x6ffd, 0x6ffe); C = A;
  CYC(0x6ffe, 0x7000); alu_and(gb, 0xf0);
  CYC(0x7000, 0x7002); alu_cp(gb, 0xf0);
  if (!(F & FZ)) { RET_TAKEN(0x7002); return; } // ret nz
  CYC(0x7002, 0x7003);
  CYC(0x7003, 0x7006); A = mem_rd(gb, wFrameCounter);
  CYC(0x7006, 0x7008); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x7008, 0x700a); goto L_7011; } // jr nz
  CYC(0x7008, 0x700a);
  CYC(0x700a, 0x700c); E = ENEMY_BASE + 0x3b; // Enemy.var3b
  CYC(0x700c, 0x700d); A = mem_rd(gb, DE);
  CYC(0x700d, 0x700e); A = alu_inc8(gb, A);
  CYC(0x700e, 0x7010); alu_and(gb, 0x1f);
  CYC(0x7010, 0x7011); mem_wr(gb, DE, A);

L_7011:
  CYC(0x7011, 0x7012); C = alu_inc8(gb, C);
  if (!(F & FZ)) { CYCT(0x7012, 0x7014); goto notInFormation; } // jr nz
  CYC(0x7012, 0x7014);
  CALL_C(0x7014, ecom_decCounter2_b0f_hook, 0x43a3, 0x7017);
  if (!(F & FZ)) { RET_TAKEN(0x7017); return; } // ret nz
  CYC(0x7017, 0x7018);
  CYC(0x7018, 0x701a); mem_wr(gb, HL, 0xb4); // [counter2] = 180
  CYC(0x701a, 0x701c); L = ENEMY_BASE + OBJ_VAR39;
  CYC(0x701c, 0x701e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 2))); // set 2,(hl)
  CYC(0x701e, 0x7020); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(0x7020, 0x7022); mem_wr(gb, HL, 0xf0);
  CYC(0x7022, 0x7024); E = ENEMY_BASE + 0x38; // Enemy.var38
  CYC(0x7024, 0x7025); A = mem_rd(gb, DE);
  CYC(0x7025, 0x7026); A = alu_inc8(gb, A);
  CYC(0x7026, 0x7028); alu_and(gb, 0x07);
  CYC(0x7028, 0x7029); B = A;
  CYC(0x7029, 0x702c); SET_HL(0x7038); // distancesFromEyesoar
  CYC(0x702c, 0x702d); eyesoar_addAToHl_from_rst(gb, 0x702d);
  CYC(0x702d, 0x702e); A = mem_rd(gb, HL);
  CYC(0x702e, 0x702f); alu_or(gb, B);
  CYC(0x702f, 0x7030); mem_wr(gb, DE, A);
  RET(0x7030); return; // ret

notInFormation:
  CYC(0x7031, 0x7033); E = ENEMY_BASE + OBJ_VAR39;
  CYC(0x7033, 0x7034); A = mem_rd(gb, DE);
  CYC(0x7034, 0x7036); A = (uint8_t)(A & ~(1 << 2)); // res 2,a
  CYC(0x7036, 0x7037); mem_wr(gb, DE, A);
  RET(0x7037); return; // ret
}

void eyesoar_dead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7040, 0x7042); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7042, 0x7043); A = mem_rd(gb, DE);
  CYC(0x7043, 0x7044); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7044, 0x7046); goto doneKillingChildren; } // jr z
  CYC(0x7044, 0x7046);
  CYC(0x7046, 0x7048); E = ENEMY_BASE + 0x30; // Enemy.var30

killNextChild:
  CYC(0x7048, 0x7049); A = mem_rd(gb, DE);
  CYC(0x7049, 0x704a); H = A;
  CYC(0x704a, 0x704b); L = E;
  CALL_C(0x704b, ecom_killObjectH_b0f_hook, 0x445e, 0x704e);
  CYC(0x704e, 0x704f); E = alu_inc8(gb, E);
  CYC(0x704f, 0x7050); A = E;
  CYC(0x7050, 0x7052); alu_cp(gb, 0xb6); // Enemy.var36
  if (F & FC) { CYCT(0x7052, 0x7054); goto killNextChild; } // jr c
  CYC(0x7052, 0x7054);

doneKillingChildren:
  CYC(0x7054, 0x7057); enemyBoss_dead_b0f_hook(gb); return; // jp
}

// Chooses an angle which roughly goes toward the center of the room, plus a small, random
// angle offset.
void eyesoar_chooseNewAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

retry:
  CALL_C(0x7057, getRandomNumber_noPreserveVars_hook, 0x0453, 0x705a);
  CYC(0x705a, 0x705c); alu_and(gb, 0x0f);
  CYC(0x705c, 0x705e); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x705e, 0x7060); goto retry; } // jr nc
  CYC(0x705e, 0x7060);
  CYC(0x7060, 0x7061); C = A;
  CYC(0x7061, 0x7063); B = 0x00;
  CYC(0x7063, 0x7065); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7065, 0x7066); A = mem_rd(gb, DE);
  CYC(0x7066, 0x7068); alu_cp(gb, 0x58); // (LARGE_ROOM_HEIGHT/2)<<4 + 8
  if (F & FC) { CYCT(0x7068, 0x706a); goto L_706b; } // jr c
  CYC(0x7068, 0x706a);
  CYC(0x706a, 0x706b); B = alu_inc8(gb, B);

L_706b:
  CYC(0x706b, 0x706d); E = ENEMY_BASE + OBJ_XH;
  CYC(0x706d, 0x706e); A = mem_rd(gb, DE);
  CYC(0x706e, 0x7070); alu_cp(gb, 0x78); // (LARGE_ROOM_WIDTH/2)<<4 + 8
  if (F & FC) { CYCT(0x7070, 0x7072); goto L_7074; } // jr c
  CYC(0x7070, 0x7072);
  CYC(0x7072, 0x7074); B = (uint8_t)(B | (1 << 1)); // set 1,b

L_7074:
  CYC(0x7074, 0x7075); A = B;
  CYC(0x7075, 0x7078); SET_HL(0x7081); // @angleVals
  CYC(0x7078, 0x7079); eyesoar_addAToHl_from_rst(gb, 0x7079);
  CYC(0x7079, 0x707a); A = mem_rd(gb, HL);
  CYC(0x707a, 0x707b); alu_add(gb, C);
  CYC(0x707b, 0x707d); alu_and(gb, 0x1f);
  CYC(0x707d, 0x707f); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x707f, 0x7080); mem_wr(gb, DE, A);
  RET(0x7080); return; // ret
}
