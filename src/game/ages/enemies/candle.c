#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode55_hook(GB *gb);
void candle_state_uninitialized_hook(GB *gb);
void candle_state_stub_hook(GB *gb);
void candle_state8_hook(GB *gb);
void candle_state9_hook(GB *gb);
void candle_stateA_hook(GB *gb);
void candle_stateB_hook(GB *gb);
void candle_applySpeed_hook(GB *gb);
void candle_animate_hook(GB *gb);
void candle_stateC_hook(GB *gb);
void candle_stateD_hook(GB *gb);
void candle_stateE_hook(GB *gb);

static uint16_t candle_jump_table(GB *gb) {
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
// ENEMY_CANDLE
//
// Variables:
//   relatedObj1: reference to INTERAC_EXPLOSION while exploding
// ==================================================================================================
void enemyCode55_hook(GB *gb) {
  BASE(enemyCode55);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x9b); // $80|ITEMCOLLISION_EMBER_SEED
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto normalStatus; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto normalStatus; } // jr nc
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x0a;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  {
    CYC(b_+28, b_+29); push_effect(gb, b_+29);
    uint16_t target = candle_jump_table(gb);
    if (target == SYM(candle_state_uninitialized)) { candle_state_uninitialized_hook(gb); return; }
    if (target == SYM(candle_state_stub)) { candle_state_stub_hook(gb); return; }
    if (target == SYM(candle_state8)) { candle_state8_hook(gb); return; }
    if (target == SYM(candle_state9)) { candle_state9_hook(gb); return; }
    if (target == SYM(candle_stateA)) { candle_stateA_hook(gb); return; }
    if (target == SYM(candle_stateB)) { candle_stateB_hook(gb); return; }
    if (target == SYM(candle_stateC)) { candle_stateC_hook(gb); return; }
    if (target == SYM(candle_stateD)) { candle_stateD_hook(gb); return; }
    if (target == SYM(candle_stateE)) { candle_stateE_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7645, bare global; jump-table target from enemyCode55.
void candle_state_uninitialized_hook(GB *gb) {
  BASE(candle_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+4); A = 0x1e; // 30
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x0a; // SPEED_40
  CYC(b_+7, b_+10); TAIL(ecom_setSpeedAndState8AndVisible_b0e); // jp
}

// 0e:764f, bare global; jump-table target from enemyCode55.
void candle_state_stub_hook(GB *gb) {
  BASE(candle_state_stub);
  RET(b_+0); return; // ret
}

// 0e:7650, bare global; jump-table target from enemyCode55. Standing still for
// [counter1] frames.
void candle_state8_hook(GB *gb) {
  BASE(candle_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x5a); // 90
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x18);
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); TAIL(enemySetAnimation); // jp $282b
}

// 0e:7669, bare global; jump-table target from enemyCode55. Walking for [counter1]
// frames.
void candle_state9_hook(GB *gb) {
  BASE(candle_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto applySpeed; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x1e); // [counter1] = 30
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);

applySpeed:
  CALL_C(b_+13, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+16);
  CYC(b_+16, b_+18); TAIL(candle_animate); // jr
}

// 0e:767b, bare global; jump-table target from enemyCode55. Just lit on fire.
void candle_stateA_hook(GB *gb) {
  BASE(candle_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x36; // PART_CANDLE_FLAME
  CALL_C(b_+2, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x78); // 120
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+17, b_+19); A = 0x02;
  CYC(b_+19, b_+22); TAIL(enemySetAnimation); // jp $282b
}

// 0e:7691, bare global; jump-table target from enemyCode55. Moving slowly at first.
void candle_stateB_hook(GB *gb) {
  BASE(candle_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); candle_applySpeed_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+13, b_+15); A = 0x03;
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(candle_applySpeed));
  TAIL(candle_applySpeed); // fallthrough
}

// 0e:76a3, bare global; falls into from candle_stateB, also reached via tail-jump
// from candle_stateC/candle_stateD.
void candle_applySpeed_hook(GB *gb) {
  BASE(candle_applySpeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), SYM(candle_animate));
  TAIL(candle_animate); // fallthrough
}

// 0e:76a9, bare global; falls into from candle_applySpeed, also reached via tail-jump
// from candle_state9.
void candle_animate_hook(GB *gb) {
  BASE(candle_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0e:76ac, bare global; jump-table target from enemyCode55. Moving faster.
void candle_stateC_hook(GB *gb) {
  BASE(candle_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); candle_applySpeed_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  TAIL(candle_stateD); // fallthrough
}

// 0e:76b5, bare global; falls into from candle_stateC. Flickering visibility, about
// to explode.
void candle_stateD_hook(GB *gb) {
  BASE(candle_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_flickerVisibility_b0e_hook, SYM(ecom_flickerVisibility_b0e), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); candle_applySpeed_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(b_+9, b_+11); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(b_+11, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); A = H;
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x40); // Interaction.start == INTERACTION_BASE
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(b_+29, b_+32); TAIL(objectSetInvisible); // jp
}

// 0e:76d5, bare global; jump-table target from enemyCode55. Waiting for the
// explosion to end.
void candle_stateE_hook(GB *gb) {
  BASE(candle_stateE);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x21; // Object.animParameter
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); alu_rlca(gb);
  if (F & FC) { CYCT(b_+9, b_+11); goto done; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00); // [child.animParameter] = 0
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+15, b_+17); A = 0x0c;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  RET(b_+19); return; // ret

done:
  CALL_C(b_+20, markEnemyAsKilledInRoom_b00_hook, SYM(markEnemyAsKilledInRoom_b00), b_+23);
  CALL_C(b_+23, decNumEnemies_hook, SYM(decNumEnemies), b_+26);
  CYC(b_+26, b_+29); TAIL(enemyDelete); // jp
}
