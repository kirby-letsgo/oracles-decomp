#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode7b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode7b), (from), (to), true)

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
  BASE(enemyCode7b);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); eyesoar_dead_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto normalStatus; } // jr nz
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + 0x3c; // Enemy.var3c
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x78);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+22, b_+24); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)

normalStatus:
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + 0x3c; // Enemy.var3c
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+29, b_+31); goto L_6e75; } // jr z
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto L_6e75; } // jr nz
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+36, b_+38); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0))); // res 0,(hl)

L_6e75:
  CYC(b_+38, b_+40); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); push_effect(gb, b_+42);
  {
    uint16_t target = eyesoar_jump_table(gb);
    if (target == SYM(eyesoar_state_uninitialized)) { eyesoar_state_uninitialized_hook(gb); return; }
    if (target == SYM(eyesoar_state1)) { eyesoar_state1_hook(gb); return; }
    if (target == SYM(eyesoar_state_stub)) { eyesoar_state_stub_hook(gb); return; }
    if (target == SYM(eyesoar_state_switchHook)) { eyesoar_state_switchHook_hook(gb); return; }
    if (target == SYM(eyesoar_state8)) { eyesoar_state8_hook(gb); return; }
    if (target == SYM(eyesoar_state9)) { eyesoar_state9_hook(gb); return; }
    if (target == SYM(eyesoar_stateA)) { eyesoar_stateA_hook(gb); return; }
    if (target == SYM(eyesoar_stateB)) { eyesoar_stateB_hook(gb); return; }
    if (target == SYM(eyesoar_stateC)) { eyesoar_stateC_hook(gb); return; }
    if (target == SYM(eyesoar_stateD)) { eyesoar_stateD_hook(gb); return; }
    if (target == SYM(eyesoar_stateE)) { eyesoar_stateE_hook(gb); return; }
    HANDOFF(target);
  }
}

void eyesoar_state_uninitialized_hook(GB *gb) {
  BASE(eyesoar_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x3c); // 60
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xfe);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_or(gb, A);
  CYC(b_+13, b_+15); A = 0x14; // SPEED_80
  if (!(F & FZ)) { CYCT(b_+15, b_+18); ecom_setSpeedAndState8_b0f_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A); // [state] = $15 (bug: invalid state)
  CYC(b_+20, b_+22); A = 0xff;
  CYC(b_+22, b_+24); B = 0x00;
  CALL_C(b_+24, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), SYM(eyesoar_state1));
  eyesoar_state1_hook(gb); return; // fallthrough
}

// Spawning "real" eyesoar and children.
void eyesoar_state1_hook(GB *gb) {
  BASE(eyesoar_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x05;
  CALL_C(b_+2, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x7b; // ENEMY_EYESOAR
  CALL_C(b_+8, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+13, b_+14); E = L;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CALL_C(b_+16, objectCopyPosition_hook, SYM(objectCopyPosition), b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+21, b_+22); B = H;
  CYC(b_+22, b_+24); C = 0x04;

spawnChildLoop:
  CYC(b_+24, b_+25); push_effect(gb, HL);
  CALL_C(b_+25, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x11); // ENEMY_EYESOAR_CHILD
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); C = alu_dec8(gb, C);
  CYC(b_+32, b_+33); mem_wr(gb, HL, C); // [child.subid]
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+37, b_+39); mem_wr(gb, HL, ENEMY_BASE); // Enemy.start
  CYC(b_+39, b_+40); A = H;
  SET_HL(pop_effect(gb));
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a [var30+i]
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto spawnChildLoop; } // jr nz
  CYC(b_+42, b_+44);
  CYC(b_+44, SYM(eyesoar_state_switchHook)); enemyDelete_hook(gb); return; // jp
}

void eyesoar_state_switchHook_hook(GB *gb) {
  BASE(eyesoar_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = eyesoar_jump_table(gb);
    if (target == b_+11) goto substate0;
    if (target == b_+39) goto substate1;
    if (target == b_+40) goto substate2;
    if (target == b_+50) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_or(gb, 0x0a);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_and(gb, 0x07);
  CYC(b_+21, b_+23); alu_or(gb, 0x18);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x4c); // ENEMYCOLLISION_EYESOAR_VULNERABLE
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x96); // 150
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  CYC(b_+36, b_+39); ecom_incSubstate_b0f_hook(gb); return; // jp

substate1:
  RET(b_+39); return; // ret

substate2:
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+44); return; } // ret nz
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+46); A = alu_inc8(gb, A);
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+50); enemySetAnimation_hook(gb); return; // jp

substate3:
  CYC(b_+50, b_+52); B = 0x0c;
  CYC(b_+52, SYM(eyesoar_state_stub)); ecom_fallToGroundAndSetState_b0f_hook(gb); return; // jp
}

void eyesoar_state_stub_hook(GB *gb) {
  BASE(eyesoar_state_stub);
  RET(b_+0); return; // ret
}

// Flickering into existence
void eyesoar_state8_hook(GB *gb) {
  BASE(eyesoar_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+9, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+12);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0xb4); // [counter2] = 180
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+23, b_+24); L = E;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+25, SYM(eyesoar_state9)); objectSetVisiblec2_hook(gb); return; // jp
}

// Waiting [counter1] frames until fight begins
void eyesoar_state9_hook(GB *gb) {
  BASE(eyesoar_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(b_+10, enemyBoss_beginBoss_b0f_hook, SYM(enemyBoss_beginBoss_b0f), b_+13);
  CYCT(b_+13, SYM(eyesoar_stateA)); eyesoar_animate_hook(gb); return; // jr
}

// Standing still for [counter1] frames?
void eyesoar_stateA_hook(GB *gb) {
  BASE(eyesoar_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, eyesoar_updateFormation_hook, SYM(eyesoar_updateFormation), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x36; // Enemy.var36
  CYC(b_+13, b_+15); A = H8(hEnemyTargetY);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); alu_sub(gb, 0x40);
  CYC(b_+18, b_+20); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(b_+20, b_+22); goto L_6f62; } // jr c
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); alu_cp(gb, 0xc0);
  CYC(b_+24, b_+26); B = 0x40;
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto L_6f62; } // jr nc
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); B = 0x70;

L_6f62:
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a [var36]
  CYC(b_+32, b_+34); A = H8(hEnemyTargetX);
  CYC(b_+34, b_+35); B = A;
  CYC(b_+35, b_+37); alu_sub(gb, 0x40);
  CYC(b_+37, b_+39); alu_cp(gb, 0x70);
  if (F & FC) { CYCT(b_+39, b_+41); goto L_6f75; } // jr c
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); alu_cp(gb, 0xc0);
  CYC(b_+43, b_+45); B = 0x40;
  if (!(F & FC)) { CYCT(b_+45, b_+47); goto L_6f75; } // jr nc
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); B = 0xb0;

L_6f75:
  CYC(b_+49, b_+50); mem_wr(gb, HL, B); // [var37]
  CYCT(b_+50, SYM(eyesoar_stateB)); eyesoar_animate_hook(gb); return; // jr
}

// Moving until it reaches its target position
void eyesoar_stateB_hook(GB *gb) {
  BASE(eyesoar_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, eyesoar_updateFormation_hook, SYM(eyesoar_updateFormation), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + 0x36; // Enemy.var36
  CALL_C(b_+6, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+9);
  CYC(b_+9, b_+10); alu_sub(gb, C);
  CYC(b_+10, b_+12); alu_add(gb, 0x02);
  CYC(b_+12, b_+14); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto L_6f9a; } // jr nc
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = H8(hFF8F);
  CYC(b_+18, b_+19); alu_sub(gb, B);
  CYC(b_+19, b_+21); alu_add(gb, 0x02);
  CYC(b_+21, b_+23); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto L_6f9a; } // jr nc
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x3c); // 60
  CYCT(b_+32, b_+34); eyesoar_animate_hook(gb); return; // jr

L_6f9a:
  CALL_C(b_+34, ecom_moveTowardPosition_b0f_hook, SYM(ecom_moveTowardPosition_b0f), SYM(eyesoar_animate));
  eyesoar_animate_hook(gb); return; // fallthrough
}

void eyesoar_animate_hook(GB *gb) {
  BASE(eyesoar_animate);
  CYC(b_+0, SYM(eyesoar_stateC)); enemyAnimate_hook(gb); return; // jp
}

// Spinning in place after being switch hook'd
void eyesoar_stateC_hook(GB *gb) {
  BASE(eyesoar_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x6d); // ENEMYCOLLISION_EYESOAR
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CALL_C(b_+12, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(eyesoar_stateD));
  eyesoar_stateD_hook(gb); return; // fallthrough
}

// Moving back up into the air
void eyesoar_stateD_hook(GB *gb) {
  BASE(eyesoar_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x80);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_sbc(gb, 0x00);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); alu_cp(gb, 0xfe);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); eyesoar_animate_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0xf0); // 240
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x0f);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+27, b_+29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CALL_C(b_+29, eyesoar_chooseNewAngle_hook, SYM(eyesoar_chooseNewAngle), SYM(eyesoar_stateE));
  eyesoar_stateE_hook(gb); return; // fallthrough
}

// Flying around kinda randomly
void eyesoar_stateE_hook(GB *gb) {
  BASE(eyesoar_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_6fda; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 3))); // res 3,(hl)
  CYC(b_+9, b_+11); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)

L_6fda:
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_6fed; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); L = alu_dec8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4))); // res 4,(hl) [var39]
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0a); // [state]
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x01);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x08); // [counter2]

L_6fed:
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+35); alu_and(gb, 0x3f);
  if (F & FZ) CALL_C_CC(b_+35, eyesoar_chooseNewAngle_hook, SYM(eyesoar_chooseNewAngle), b_+38); else CYC(b_+35, b_+38); // call z
  CALL_C(b_+38, ecom_applyVelocityForSideviewEnemyNoHoles_b0f_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0f), b_+41);
  CYCT(b_+41, SYM(eyesoar_updateFormation)); eyesoar_animate_hook(gb); return; // jr
}

// Checks to update the "formation", that is, the distances away from Eyesoar for the
// children.
void eyesoar_updateFormation_hook(GB *gb) {
  BASE(eyesoar_updateFormation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); alu_and(gb, 0xf0);
  CYC(b_+6, b_+8); alu_cp(gb, 0xf0);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wFrameCounter);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto L_7011; } // jr nz
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + 0x3b; // Enemy.var3b
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);
  CYC(b_+20, b_+22); alu_and(gb, 0x1f);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);

L_7011:
  CYC(b_+23, b_+24); C = alu_inc8(gb, C);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto notInFormation; } // jr nz
  CYC(b_+24, b_+26);
  CALL_C(b_+26, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+29);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } // ret nz
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xb4); // [counter2] = 180
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+34, b_+36); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 2))); // set 2,(hl)
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0xf0);
  CYC(b_+40, b_+42); E = ENEMY_BASE + 0x38; // Enemy.var38
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); A = alu_inc8(gb, A);
  CYC(b_+44, b_+46); alu_and(gb, 0x07);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+50); SET_HL(SYM(distancesFromEyesoar)); // distancesFromEyesoar
  CYC(b_+50, b_+51); eyesoar_addAToHl_from_rst(gb, b_+51);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+53); alu_or(gb, B);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  RET(b_+54); return; // ret

notInFormation:
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+60); A = (uint8_t)(A & ~(1 << 2)); // res 2,a
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  RET(b_+61); return; // ret
}

void eyesoar_dead_hook(GB *gb) {
  BASE(eyesoar_dead);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto doneKillingChildren; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + 0x30; // Enemy.var30

killNextChild:
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); H = A;
  CYC(b_+10, b_+11); L = E;
  CALL_C(b_+11, ecom_killObjectH_b0f_hook, SYM(ecom_killObjectH_b0f), b_+14);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
  CYC(b_+15, b_+16); A = E;
  CYC(b_+16, b_+18); alu_cp(gb, 0xb6); // Enemy.var36
  if (F & FC) { CYCT(b_+18, b_+20); goto killNextChild; } // jr c
  CYC(b_+18, b_+20);

doneKillingChildren:
  CYC(b_+20, SYM(eyesoar_chooseNewAngle)); enemyBoss_dead_b0f_hook(gb); return; // jp
}

// Chooses an angle which roughly goes toward the center of the room, plus a small, random
// angle offset.
void eyesoar_chooseNewAngle_hook(GB *gb) {
  BASE(eyesoar_chooseNewAngle);
  uint16_t sp0_ = gb->sp;

retry:
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto retry; } // jr nc
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+12); B = 0x00;
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x58); // (LARGE_ROOM_HEIGHT/2)<<4 + 8
  if (F & FC) { CYCT(b_+17, b_+19); goto L_706b; } // jr c
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); B = alu_inc8(gb, B);

L_706b:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_cp(gb, 0x78); // (LARGE_ROOM_WIDTH/2)<<4 + 8
  if (F & FC) { CYCT(b_+25, b_+27); goto L_7074; } // jr c
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); B = (uint8_t)(B | (1 << 1)); // set 1,b

L_7074:
  CYC(b_+29, b_+30); A = B;
  CYC(b_+30, b_+33); SET_HL(b_+42); // @angleVals
  CYC(b_+33, b_+34); eyesoar_addAToHl_from_rst(gb, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_add(gb, C);
  CYC(b_+36, b_+38); alu_and(gb, 0x1f);
  CYC(b_+38, b_+40); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  RET(b_+41); return; // ret
}
