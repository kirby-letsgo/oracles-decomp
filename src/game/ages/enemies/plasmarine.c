#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode7e_hook(GB *gb);
void plasmarine_state_uninitialized_hook(GB *gb);
void plasmarine_state_switchHook_hook(GB *gb);
void plasmarine_state_stub_hook(GB *gb);
void plasmarine_state8_hook(GB *gb);
void plasmarine_state9_hook(GB *gb);
void plasmarine_stateA_hook(GB *gb);
void plasmarine_animate_hook(GB *gb);
void plasmarine_stateB_hook(GB *gb);
void plasmarine_stateC_hook(GB *gb);
void plasmarine_stateD_hook(GB *gb);
void plasmarine_stateE_hook(GB *gb);
void plasmarine_stateF_hook(GB *gb);
void plasmarine_decideNumberOfShockAttacks_hook(GB *gb);
void plasmarine_checkCloseToTargetPosition_hook(GB *gb);

static uint16_t plasmarine_jump_table(GB *gb) {
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

static void plasmarine_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// plasmarine_state_switchHook@swapColor: reached by fallthrough from @afterSwitch
// (within plasmarine_state_switchHook_hook), and also by a static `call nc` from
// enemyCode7e itself (NOT HOOKED). Ends at plasmarine_state_switchHook@beforeSwitch's
// `ret`, which is also a jump-table target of plasmarine_state_switchHook.
static void plasmarine_swapColor(GB *gb, uint16_t sp0_) {
  BASE(plasmarine_state_switchHook);
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = ENEMY_BASE + 0x32; // var32
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+39); alu_xor(gb, 0x01);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+41); A = alu_inc8(gb, A);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  RET(b_+45); return; // ret
}

// ==================================================================================================
// ENEMY_PLASMARINE
//
// Variables:
//   counter2: Number of times to do shock attack before firing projectiles
//   var30/var31: Target position?
//   var32: Color (0 for blue, 1 for red)
//   var33: ?
//   var34: Number of projectiles to fire in one attack
// ==================================================================================================
void enemyCode7e_hook(GB *gb) {
  BASE(enemyCode7e);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x68); // ENEMYCOLLISION_PLASMARINE_SHOCK
  if (F & FZ) { CYCT(b_+13, b_+15); goto normalStatus; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+20, b_+22); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (!(F & FC)) { CYCT(b_+22, b_+25); push_effect(gb, b_+25); plasmarine_swapColor(gb, sp0_); } // call nc
  else { CYC(b_+22, b_+25); }

normalStatus:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); push_effect(gb, b_+29);
  {
    uint16_t target = plasmarine_jump_table(gb);
    if (target == SYM(plasmarine_state_uninitialized)) { plasmarine_state_uninitialized_hook(gb); return; }
    if (target == SYM(plasmarine_state_stub)) { plasmarine_state_stub_hook(gb); return; }
    if (target == SYM(plasmarine_state_switchHook)) { plasmarine_state_switchHook_hook(gb); return; }
    if (target == SYM(plasmarine_state8)) { plasmarine_state8_hook(gb); return; }
    if (target == SYM(plasmarine_state9)) { plasmarine_state9_hook(gb); return; }
    if (target == SYM(plasmarine_stateA)) { plasmarine_stateA_hook(gb); return; }
    if (target == SYM(plasmarine_stateB)) { plasmarine_stateB_hook(gb); return; }
    if (target == SYM(plasmarine_stateC)) { plasmarine_stateC_hook(gb); return; }
    if (target == SYM(plasmarine_stateD)) { plasmarine_stateD_hook(gb); return; }
    if (target == SYM(plasmarine_stateE)) { plasmarine_stateE_hook(gb); return; }
    if (target == SYM(plasmarine_stateF)) { plasmarine_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

void plasmarine_state_uninitialized_hook(GB *gb) {
  BASE(plasmarine_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x64; // SPEED_280
  CALL_C(b_+2, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x04);
  CYC(b_+13, b_+15); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x58);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x78);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+25, b_+28); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+28, b_+30); A = 0x7e; // ENEMY_PLASMARINE
  CYC(b_+30, b_+32); B = 0x00;
  CALL_C(b_+32, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+35);
  CYC(b_+35, b_+38); objectSetVisible83_hook(gb); return; // jp
}

void plasmarine_state_switchHook_hook(GB *gb) {
  BASE(plasmarine_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = plasmarine_jump_table(gb);
    if (target == b_+11) goto justLatched;
    if (target == b_+45) { RET(b_+45); return; } // beforeSwitch
    if (target == b_+21) goto afterSwitch;
    if (target == b_+46) goto released;
    HANDOFF(target);
  }

justLatched:
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), b_+18);
  CYC(b_+18, b_+21); ecom_incSubstate_b0f_hook(gb); return; // jp

afterSwitch:
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; } // ret nz
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); A = 0x7b; // SND_MYSTERY_SEED
  CALL_C(b_+30, playSound_b00_hook, SYM(playSound_b00), b_+33);
  plasmarine_swapColor(gb, sp0_); return; // fallthrough

released:
  CYC(b_+46, b_+48); B = 0x0a;
  CALL_C(b_+48, ecom_fallToGroundAndSetState_b0f_hook, SYM(ecom_fallToGroundAndSetState_b0f), b_+51);
  if (!(F & FZ)) { RET_TAKEN(b_+51); return; } // ret nz
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+54, b_+56); mem_wr(gb, HL, 60);
  CYC(b_+56, b_+59); plasmarine_decideNumberOfShockAttacks_hook(gb); return; // jp
}

void plasmarine_state_stub_hook(GB *gb) {
  BASE(plasmarine_state_stub);
  RET(b_+0); return; // ret
}

// Moving toward centre of room before starting fight
void plasmarine_state8_hook(GB *gb) {
  BASE(plasmarine_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, plasmarine_checkCloseToTargetPosition_hook, SYM(plasmarine_checkCloseToTargetPosition), b_+3);
  if (F & FC) { CYCT(b_+3, b_+5); goto reachedTarget; } // jr c
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_7b0d; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x04);
  CALL_C(b_+12, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+15);
  CALL_C(b_+15, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+18);

L_7b0d:
  CALL_C(b_+18, objectApplySpeed_hook, SYM(objectApplySpeed), b_+21);
  CYCT(b_+21, b_+23); plasmarine_animate_hook(gb); return; // jr

reachedTarget:
  CYC(b_+23, b_+24); L = E;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 60);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+31, b_+32); mem_wr(gb, HL, B);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+34, b_+35); mem_wr(gb, HL, C);
  CYCT(b_+35, b_+37); plasmarine_animate_hook(gb); return; // jr
}

// 60 frame delay before starting fight
void plasmarine_state9_hook(GB *gb) {
  BASE(plasmarine_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); plasmarine_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 60); // [counter1]
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(b_+13, plasmarine_decideNumberOfShockAttacks_hook, SYM(plasmarine_decideNumberOfShockAttacks), b_+16);
  CALL_C(b_+16, enemyBoss_beginBoss_b0f_hook, SYM(enemyBoss_beginBoss_b0f), b_+19);
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+23); enemySetAnimation_hook(gb); return; // jp
}

// Standing in place before charging
void plasmarine_stateA_hook(GB *gb) {
  BASE(plasmarine_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); plasmarine_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+8, b_+10); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+10, b_+12); plasmarine_animate_hook(gb); return; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0c);
  CYC(b_+16, b_+17); L = E;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(b_+22, b_+24); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+24, b_+26); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+27, b_+29); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  plasmarine_animate_hook(gb); return; // fallthrough
}

void plasmarine_animate_hook(GB *gb) {
  BASE(plasmarine_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Charging toward Link
void plasmarine_stateB_hook(GB *gb) {
  BASE(plasmarine_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_7b5f; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c

L_7b5f:
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_sub(gb, 0x05); // SPEED_20
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  plasmarine_stateC_hook(gb); return; // fallthrough
}

void plasmarine_stateC_hook(GB *gb) {
  BASE(plasmarine_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, plasmarine_checkCloseToTargetPosition_hook, SYM(plasmarine_checkCloseToTargetPosition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+6); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nc
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+10); L = E;
  if (F & FZ) { CYCT(b_+10, b_+12); goto fireProjectiles; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x0d); // [state]
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 65);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+20, b_+22); mem_wr(gb, HL, (uint8_t)-8);
  CYC(b_+22, b_+24); L = ENEMY_BASE + 0x32; // var32
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_add(gb, 0x04);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x68); // ENEMYCOLLISION_PLASMARINE_SHOCK
  CYC(b_+35, b_+37); A = 0x02;
  CYC(b_+37, b_+40); enemySetAnimation_hook(gb); return; // jp

fireProjectiles:
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x0e); // [state]
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); A = alu_dec8(gb, A);
  CYC(b_+46, b_+49); SET_HL(b_+59); // @numProjectilesToFire
  CYC(b_+49, b_+50); plasmarine_addAToHl_from_rst(gb, b_+50);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+56); A = 0x01;
  CYC(b_+56, b_+59); enemySetAnimation_hook(gb); return; // jp
}

// Shock attack
void plasmarine_stateD_hook(GB *gb) {
  BASE(plasmarine_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto doneAttack; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x0f);
  CYC(b_+8, b_+10); A = 0xac; // SND_SHOCK
  if (F & FZ) { CALL_C_CC(b_+10, playSound_b00_hook, SYM(playSound_b00), b_+13); } else { CYC(b_+10, b_+13); } // call z
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  CYC(b_+17, b_+19); B = 0x04;
  if (F & FZ) { CYCT(b_+19, b_+21); goto L_7bbe; } // jr z
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); B = 0x01;

L_7bbe:
  CYC(b_+23, b_+25); E = ENEMY_BASE + 0x32; // var32
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYCT(b_+32, b_+34); plasmarine_animate_hook(gb); return; // jr

doneAttack:
  CYC(b_+34, b_+36); mem_wr(gb, HL, 60); // [counter1]
  CYC(b_+36, b_+37); L = E;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x0a); // [state]
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)-4);
  CYC(b_+47, b_+49); L = ENEMY_BASE + 0x32; // var32
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+51); A = alu_inc8(gb, A);
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+54, b_+55); mem_wr(gb, HL, A);
  CYC(b_+55, b_+57); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x4f); // ENEMYCOLLISION_PLASMARINE
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+63); enemySetAnimation_hook(gb); return; // jp
}

// Firing projectiles
void plasmarine_stateE_hook(GB *gb) {
  BASE(plasmarine_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto fire; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 60);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); enemySetAnimation_hook(gb); return; // jp

fire:
  CYC(b_+22, b_+23); mem_wr(gb, DE, A); // [animParameter] = 0
  CALL_C(b_+23, getFreePartSlot_hook, SYM(getFreePartSlot), b_+26);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x43); // PART_PLASMARINE_PROJECTILE
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); A = alu_dec8(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); // [projectile.var03]
  CYC(b_+35, b_+37); L = PART_BASE + OBJ_RELATED1 + 1;
  CYC(b_+37, b_+38); mem_wr(gb, HL, D);
  CYC(b_+38, b_+39); L = alu_dec8(gb, L);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x80); // Enemy.start
  CYC(b_+41, b_+44); SET_BC(0xec00);
  CALL_C(b_+44, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+47);
  CYC(b_+47, b_+49); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CYC(b_+49, b_+52); playSound_b00_hook(gb); return; // jp
}

// Decides whether to return to state $0e (fire another projectile) or charge at Link again
void plasmarine_stateF_hook(GB *gb) {
  BASE(plasmarine_stateF);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+10); L = E;
  if (F & FZ) { CYCT(b_+10, b_+12); goto chargeAtLink; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state] = $0e
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+18); enemySetAnimation_hook(gb); return; // jp

chargeAtLink:
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0a);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 30);
  plasmarine_decideNumberOfShockAttacks_hook(gb); return; // fallthrough
}

void plasmarine_decideNumberOfShockAttacks_hook(GB *gb) {
  BASE(plasmarine_decideNumberOfShockAttacks);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  RET(b_+9); return; // ret
}

// @param[out] cflag c if close enough to target position
void plasmarine_checkCloseToTargetPosition_hook(GB *gb) {
  BASE(plasmarine_checkCloseToTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // var30
  CALL_C(b_+3, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x04);
  CYC(b_+9, b_+11); alu_cp(gb, 0x09);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; } // ret nc
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8F); // hFF8F
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  RET(b_+19); return; // ret
}
