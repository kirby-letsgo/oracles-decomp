#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode18), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode18), (from), (to), true)

void buzzblob_state_uninitialized_hook(GB *gb);
void buzzblob_state_scentSeed_hook(GB *gb);
void buzzblob_state_stub_hook(GB *gb);
void buzzblob_state8_hook(GB *gb);
void buzzblob_state9_hook(GB *gb);
void buzzblob_animate_hook(GB *gb);
void buzzblob_stateA_hook(GB *gb);
void buzzblob_chooseNewDirection_hook(GB *gb);
void buzzblob_checkShowText_hook(GB *gb);

static uint16_t enemyCode18_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_BUZZBLOB
// ==================================================================================================
void enemyCode18_hook(GB *gb) {
  BASE(enemyCode18);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+7, b_+10); buzzblob_checkShowText_hook(gb); return; } // jp c
  CYC(b_+7, b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); enemyDie_hook(gb); return; } // jp z
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+14); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (F & FZ) { CYCT(b_+23, b_+25); goto becomeCukeman; } // jr z
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0xa0); // $80|ITEMCOLLISION_ELECTRIC_SHOCK
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; } // ret nz
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x0a);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+34, b_+36); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4))); // res 4,(hl)
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x00);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x3c);
  CYC(b_+44, b_+46); A = 0x01;
  CYC(b_+46, b_+49); enemySetAnimation_hook(gb); return; // jp

becomeCukeman:
  CYC(b_+49, b_+51); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+51, b_+53); A = 0x02;
  CYC(b_+53, b_+54); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+54); return; } // ret z
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  CALL_C(b_+56, enemySetAnimation_hook, SYM(enemySetAnimation), b_+59);
  CYC(b_+59, b_+61); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+61, b_+64); objectAddToAButtonSensitiveObjectList_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+64, buzzblob_checkShowText_hook, SYM(buzzblob_checkShowText), b_+67);
  CALL_C(b_+67, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+70);
  CYC(b_+70, b_+72); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  {
    CYC(b_+73, b_+74); push_effect(gb, b_+74);
    uint16_t target = enemyCode18_jump_table(gb);
    if (target == SYM(buzzblob_state_uninitialized)) { buzzblob_state_uninitialized_hook(gb); return; }
    if (target == SYM(buzzblob_state_stub)) { buzzblob_state_stub_hook(gb); return; }
    if (target == SYM(buzzblob_state_scentSeed)) { buzzblob_state_scentSeed_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == SYM(buzzblob_state8)) { buzzblob_state8_hook(gb); return; }
    if (target == SYM(buzzblob_state9)) { buzzblob_state9_hook(gb); return; }
    if (target == SYM(buzzblob_stateA)) { buzzblob_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:569b, bare global.
void buzzblob_state_uninitialized_hook(GB *gb) {
  BASE(buzzblob_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0a; // SPEED_40
  CALL_C(b_+2, ecom_setSpeedAndState8AndVisible_b0d_hook, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_or(gb, 0x30);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return; // ret
}

// 0d:56a7, bare global.
void buzzblob_state_scentSeed_hook(GB *gb) {
  BASE(buzzblob_state_scentSeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto seek; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x08;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A); // [state] = 8
  CYC(b_+9, b_+11); buzzblob_animate_hook(gb); return; // jr

seek:
  CALL_C(b_+11, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_add(gb, 0x04);
  CYC(b_+19, b_+21); alu_and(gb, 0x18);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, ecom_applyVelocityForSideviewEnemy_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0d), b_+25);
  CYC(b_+25, SYM(buzzblob_state_stub)); enemyAnimate_hook(gb); return; // jp
}

// 0d:56c3, bare global.
void buzzblob_state_stub_hook(GB *gb) {
  BASE(buzzblob_state_stub);
  RET(b_+0); return; // ret
}

// 0d:56c4, bare global; choosing a direction and duration to move.
void buzzblob_state8_hook(GB *gb) {
  BASE(buzzblob_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x09;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state] = 9
  CYC(b_+3, b_+6); SET_BC((SYM(objectCheckCollidedWithLink_notDead) + 2));
  CALL_C(b_+6, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); A = 0x30;
  CYC(b_+13, b_+14); alu_add(gb, C);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, SYM(buzzblob_state9)); buzzblob_animate_hook(gb); return; // jr
}

// 0d:56d9, bare global; moving in some direction for a certain amount of time.
void buzzblob_state9_hook(GB *gb) {
  BASE(buzzblob_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); buzzblob_chooseNewDirection_hook(gb); return; } // jr z
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_bounceOffWallsAndHoles_b0d_hook, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+8);
  CALL_C(b_+8, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(buzzblob_animate));
  buzzblob_animate_hook(gb); return; // fallthrough
}

// 0d:56e4, bare global.
void buzzblob_animate_hook(GB *gb) {
  BASE(buzzblob_animate);
  CYC(b_+0, SYM(buzzblob_stateA)); enemyAnimate_hook(gb); return; // jp
}

// 0d:56e7, bare global; "shocking Link" state.
void buzzblob_stateA_hook(GB *gb) {
  BASE(buzzblob_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); buzzblob_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CALL_C(b_+8, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(buzzblob_chooseNewDirection));
  buzzblob_chooseNewDirection_hook(gb); return; // fallthrough
}

// 0d:56f2, bare global.
void buzzblob_chooseNewDirection_hook(GB *gb) {
  BASE(buzzblob_chooseNewDirection);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x08); // will choose new direction in state 8
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+13, SYM(buzzblob_checkShowText)); buzzblob_animate_hook(gb); return; // jr
}

// 0d:5701, bare global; boundary item shared with the buzzblob source file.
void buzzblob_checkShowText_hook(GB *gb) {
  BASE(buzzblob_checkShowText);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CALL_C(b_+7, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x07);
  CYC(b_+12, b_+14); alu_add(gb, 0x1e); // <TX_2f1e
  CYC(b_+14, b_+15); C = A;
  CYC(b_+15, b_+17); B = 0x2f; // >TX_2f00
  CYC(b_+17, SYM(enemyCode1a)); showText_hook(gb); return; // jp
}
