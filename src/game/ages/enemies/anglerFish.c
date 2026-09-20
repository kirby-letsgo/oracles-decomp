#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void ecom_killRelatedObj1_b0f_hook(GB *gb);
void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb);
void ecom_incState_b0f_hook(GB *gb);
void ecom_decCounter1_b0f_hook(GB *gb);
void ecom_decCounter2_b0f_hook(GB *gb);
void ecom_bounceOffWallsAndHoles_b0f_hook(GB *gb);
void ecom_flickerVisibility_b0f_hook(GB *gb);
void ecom_setSpeedAndState8_b0f_hook(GB *gb);
void ecom_spawnUncountedEnemyWithSubid01_b0f_hook(GB *gb);

void enemyBoss_dead_b0f_hook(GB *gb);
void enemyBoss_initializeRoom_b0f_hook(GB *gb);
void enemyBoss_beginMiniboss_b0f_hook(GB *gb);

void enemyCode76_hook(GB *gb);
void anglerFish_state_uninitialized_hook(GB *gb);
void anglerFish_state_stub_hook(GB *gb);
void anglerFish_main_hook(GB *gb);
void anglerFish_main_state8_hook(GB *gb);
void anglerFish_main_state9_hook(GB *gb);
void anglerFish_main_stateA_hook(GB *gb);
void anglerFish_bounceOffGround_hook(GB *gb);
void anglerFish_main_stateB_hook(GB *gb);
void anglerFish_updatePosition_hook(GB *gb);
void anglerFish_applySpeed_hook(GB *gb);
void anglerFish_updateAnimation_hook(GB *gb);
void anglerFish_main_stateC_hook(GB *gb);
void anglerFish_main_stateD_hook(GB *gb);
void anglerFish_main_stateE_hook(GB *gb);
void anglerFish_main_stateF_hook(GB *gb);
void anglerFish_antenna_hook(GB *gb);
void anglerFish_main_checkFireProjectile_hook(GB *gb);

static uint16_t anglerFish_jump_table(GB *gb) {
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

static void anglerFish_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_ANGLER_FISH
// ==================================================================================================
void enemyCode76_hook(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto justHit; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+14); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(b_+11, b_+14);
  CALL_C(b_+14, ecom_killRelatedObj1_b0f_hook, SYM(ecom_killRelatedObj1_b0f), b_+17);
  CYC(b_+17, b_+20); enemyDelete_hook(gb); return; // jp

justHit:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto fishHit; } // jr z
  CYC(b_+24, b_+26);

antennaHit:
  CYC(b_+26, b_+28); A = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CALL_C(b_+28, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+31);
  CYC(b_+31, b_+32); E = L;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYCT(b_+34, b_+36); goto normalStatus; // jr

fishHit:
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); alu_cp(gb, 0x9c); // $80|ITEMCOLLISION_SCENT_SEED
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto normalStatus; } // jr nz
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); H = D;
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x0d);
  CYC(b_+48, b_+50); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+50, b_+52); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+52, b_+54); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+57); alu_and(gb, 0x01);
  CYC(b_+57, b_+59); alu_add(gb, 0x04);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CALL_C(b_+60, enemySetAnimation_hook, SYM(enemySetAnimation), b_+63);
  CYC(b_+63, b_+65); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(b_+65, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+68);

normalStatus:
  CALL_C(b_+68, ecom_getSubidAndCpStateTo08_b0f_hook, SYM(ecom_getSubidAndCpStateTo08_b0f), b_+71);
  if (F & FC) { CYCT(b_+71, b_+73); goto commonState; } // jr c
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+74); A = B;
  CYC(b_+74, b_+75); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+75, b_+78); anglerFish_main_hook(gb); return; } // jp z
  CYC(b_+75, b_+78);
  CYC(b_+78, b_+81); anglerFish_antenna_hook(gb); return; // jp

commonState:
  CYC(b_+81, b_+82); push_effect(gb, b_+82);
  {
    uint16_t target = anglerFish_jump_table(gb);
    if (target == SYM(anglerFish_state_uninitialized)) { anglerFish_state_uninitialized_hook(gb); return; }
    if (target == SYM(anglerFish_state_stub)) { anglerFish_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

void anglerFish_state_uninitialized_hook(GB *gb) {
  BASE(anglerFish_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+4); B = 0x00;
  CALL_C(b_+4, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_bit(gb, 7, A);
  CYC(b_+12, b_+14); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto doneInit; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+20); goto doneInit; } // jr z
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); B = 0x76; // ENEMY_ANGLER_FISH
  CALL_C(b_+22, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0f), b_+25);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; } // ret nz
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+28, b_+29); L = E;
  CYC(b_+29, b_+31); A = ENEMY_BASE; // Enemy.start
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+33, b_+34); E = alu_inc8(gb, E);
  CYC(b_+34, b_+35); mem_wr(gb, HL, D);
  CYC(b_+35, b_+36); A = H;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+38); A = H;
  CYC(b_+38, b_+39); alu_cp(gb, D);
  if (!(F & FC)) { CYCT(b_+39, b_+41); goto doneInit; } // jr nc
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x80);
  CYC(b_+45, b_+46); E = L;
  CYC(b_+46, b_+48); A = 0x01;
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);

doneInit:
  CYC(b_+49, b_+52); ecom_setSpeedAndState8_b0f_hook(gb); return; // jp
}

void anglerFish_state_stub_hook(GB *gb) {
  BASE(anglerFish_state_stub);
  RET(b_+0); return;
}

void anglerFish_main_hook(GB *gb) {
  BASE(anglerFish_main);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = anglerFish_jump_table(gb);
    if (target == SYM(anglerFish_main_state8)) { anglerFish_main_state8_hook(gb); return; }
    if (target == SYM(anglerFish_main_state9)) { anglerFish_main_state9_hook(gb); return; }
    if (target == SYM(anglerFish_main_stateA)) { anglerFish_main_stateA_hook(gb); return; }
    if (target == SYM(anglerFish_main_stateB)) { anglerFish_main_stateB_hook(gb); return; }
    if (target == SYM(anglerFish_main_stateC)) { anglerFish_main_stateC_hook(gb); return; }
    if (target == SYM(anglerFish_main_stateD)) { anglerFish_main_stateD_hook(gb); return; }
    if (target == SYM(anglerFish_main_stateE)) { anglerFish_main_stateE_hook(gb); return; }
    if (target == SYM(anglerFish_main_stateF)) { anglerFish_main_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for Link to enter
void anglerFish_main_state8_hook(GB *gb) {
  BASE(anglerFish_main_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_state); // w1Link.state
  CYC(b_+3, b_+5); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CALL_C(b_+6, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+9);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; } // ret nc
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x01; // DISABLE_LINK
  CYC(b_+12, b_+15); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+15, b_+18); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+18, b_+20); A = 0x42;
  CYC(b_+20, b_+22); C = 0x80;
  CALL_C(b_+22, setTile_hook, SYM(setTile), b_+25);
  CYC(b_+25, b_+27); A = 0x52;
  CYC(b_+27, b_+29); C = 0x90;
  CALL_C(b_+29, setTile_hook, SYM(setTile), b_+32);
  CALL_C(b_+32, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+35);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x1e);
  CYC(b_+39, b_+41); A = 0x70; // SND_DOORCLOSE
  CYC(b_+41, b_+44); playSound_b00_hook(gb); return; // jp
}

// Delay before starting fight
void anglerFish_main_state9_hook(GB *gb) {
  BASE(anglerFish_main_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+14, b_+17); objectSetVisible82_hook(gb); return; // jp
}

// Falling to the ground, then the fight will begin
void anglerFish_main_stateA_hook(GB *gb) {
  BASE(anglerFish_main_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x0c;
  CYC(b_+2, b_+4); A = 0x10;
  CALL_C(b_+4, objectUpdateSpeedZ_sidescroll_givenYOffset_hook, SYM(objectUpdateSpeedZ_sidescroll_givenYOffset), b_+7);
  if (F & FC) { CYCT(b_+7, b_+9); goto hitGround; } // jr c
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + 0x15; // Enemy.speedZ+1
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+14); return; } // ret c
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x02);
  RET(b_+17); return;

hitGround:
  CALL_C(b_+18, enemyBoss_beginMiniboss_b0f_hook, SYM(enemyBoss_beginMiniboss_b0f), b_+21);
  CALL_C(b_+21, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xb4); // 180
  anglerFish_bounceOffGround_hook(gb); return; // fallthrough
}

void anglerFish_bounceOffGround_hook(GB *gb) {
  BASE(anglerFish_bounceOffGround);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+3, b_+5); A = 0xe0; // -$320 low byte
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xfc); // -$320 high byte
  CYC(b_+8, b_+10); A = 0x98; // SND_POOF
  CYC(b_+10, b_+13); playSound_b00_hook(gb); return; // jp
}

// Bouncing around normally
void anglerFish_main_stateB_hook(GB *gb) {
  BASE(anglerFish_main_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (F & FZ) { CALL_C_CC(b_+3, anglerFish_main_checkFireProjectile_hook, SYM(anglerFish_main_checkFireProjectile), SYM(anglerFish_updatePosition)); } else { CYC(b_+3, b_+6); } // call z
  anglerFish_updatePosition_hook(gb); return; // fallthrough
}

void anglerFish_updatePosition_hook(GB *gb) {
  BASE(anglerFish_updatePosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x0c;
  CYC(b_+2, b_+4); A = 0x10;
  CALL_C(b_+4, objectUpdateSpeedZ_sidescroll_givenYOffset_hook, SYM(objectUpdateSpeedZ_sidescroll_givenYOffset), b_+7);
  if (!(F & FC)) { CYCT(b_+7, b_+9); anglerFish_applySpeed_hook(gb); return; } // jr nc
  CYC(b_+7, b_+9);
  CALL_C(b_+9, anglerFish_bounceOffGround_hook, SYM(anglerFish_bounceOffGround), b_+12);
  CALL_C(b_+12, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x10);
  CYC(b_+17, b_+19); alu_add(gb, 0x08);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); alu_and(gb, 0x10);
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_and(gb, 0x01);
  CYC(b_+34, b_+35); alu_cp(gb, B);
  if (!(F & FZ)) { CALL_C_CC(b_+35, anglerFish_updateAnimation_hook, SYM(anglerFish_updateAnimation), SYM(anglerFish_applySpeed)); } else { CYC(b_+35, b_+38); } // call nz
  anglerFish_applySpeed_hook(gb); return; // fallthrough
}

void anglerFish_applySpeed_hook(GB *gb) {
  BASE(anglerFish_applySpeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_bounceOffWallsAndHoles_b0f_hook, SYM(ecom_bounceOffWallsAndHoles_b0f), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+9); enemyAnimate_hook(gb); return; } // jp z
  CYC(b_+6, b_+9);
  anglerFish_updateAnimation_hook(gb); return; // fallthrough
}

void anglerFish_updateAnimation_hook(GB *gb) {
  BASE(anglerFish_updateAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_xor(gb, 0x01);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9); enemySetAnimation_hook(gb); return; // jp
}

// Firing a projectile
void anglerFish_main_stateC_hook(GB *gb) {
  BASE(anglerFish_main_stateC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto doneFiring; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); anglerFish_updatePosition_hook(gb); return; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+14);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); anglerFish_updatePosition_hook(gb); return; } // jr nz
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x26); // ENEMY_ANGLER_FISH_BUBBLE
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+20, b_+22); A = ENEMY_BASE; // Enemy.start
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+23, b_+24); mem_wr(gb, HL, D);
  CYC(b_+24, b_+26); A = 0x59; // SND_FALLINHOLE
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CYCT(b_+29, b_+31); anglerFish_updatePosition_hook(gb); return; // jr

doneFiring:
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+40); alu_sub(gb, 0x02);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CALL_C(b_+41, enemySetAnimation_hook, SYM(enemySetAnimation), b_+44);
  CYCT(b_+44, b_+46); anglerFish_updatePosition_hook(gb); return; // jr
}

// Just hit with a scent seed, falling to ground
void anglerFish_main_stateD_hook(GB *gb) {
  BASE(anglerFish_main_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x96); // 150
  RET(b_+13); return;
}

// Vulnerable for [counter1] frames
void anglerFish_main_stateE_hook(GB *gb) {
  BASE(anglerFish_main_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+10, b_+12); A = 0x00;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xfc);
  RET(b_+15); return;
}

// Bouncing back up after being deflated
void anglerFish_main_stateF_hook(GB *gb) {
  BASE(anglerFish_main_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + 0x15; // Enemy.speedZ+1
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); anglerFish_applySpeed_hook(gb); return; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x0b);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xb4); // 180
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+28); alu_sub(gb, 0x04);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CALL_C(b_+29, enemySetAnimation_hook, SYM(enemySetAnimation), b_+32);
  CYC(b_+32, b_+34); A = 0x98; // SND_POOF
  CYC(b_+34, b_+37); playSound_b00_hook(gb); return; // jp
}

void anglerFish_antenna_hook(GB *gb) {
  BASE(anglerFish_antenna);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state8; } // jr z
  CYC(b_+3, b_+5);

state9:
  CYC(b_+5, b_+7); A = OBJ_DIRECTION;
  CALL_C(b_+7, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); push_effect(gb, HL);
  CYC(b_+12, b_+15); SET_HL(b_+38);
  CYC(b_+15, b_+16); anglerFish_addDoubleIndexToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CYC(b_+19, b_+20); SET_HL(pop_effect(gb));
  CALL_C(b_+20, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+23);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+27, b_+30); objectSetInvisible_hook(gb); return; } // jp nz
  CYC(b_+27, b_+30);
  CYC(b_+30, b_+33); A = mem_rd(gb, wFrameCounter);
  CYC(b_+33, b_+34); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+34); return; } // ret c
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+38); ecom_flickerVisibility_b0f_hook(gb); return; // jp

state8:
  CYC(b_+50, b_+51); H = D;
  CYC(b_+51, b_+52); L = E;
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x47); // ENEMYCOLLISION_ANGLER_FISH_ANTENNA
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+59, b_+61); A = 0x03;
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x1e);
  CYC(b_+67, b_+69); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+69, b_+71); A = 0x0d;
  CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+72, b_+73); mem_wr(gb, HL, A);
  CYC(b_+73, b_+75); A = 0x06;
  CYC(b_+75, b_+78); enemySetAnimation_hook(gb); return; // jp
}

// Changes state to $0c if conditions are appropriate to fire a projectile.
void anglerFish_main_checkFireProjectile_hook(GB *gb) {
  BASE(anglerFish_main_checkFireProjectile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x5c);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_sub(gb, 0x38);
  CYC(b_+11, b_+13); alu_cp(gb, 0x70);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } // ret nc
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0xb4); // [counter2] = 180 (hl left by caller's ecom_decCounter2)
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_add(gb, 0x02);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+28); enemySetAnimation_hook(gb); return; // jp
}
