#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void greatFairy_state_uninitialized_hook(GB *gb);
void greatFairy_state1_hook(GB *gb);
void greatFairy_state2_hook(GB *gb);
void greatFairy_state3_hook(GB *gb);
void greatFairy_animate_hook(GB *gb);
void greatFairy_state4_hook(GB *gb);
void greatFairy_state5_hook(GB *gb);
void greatFairy_state6_hook(GB *gb);
void greatFairy_state7_hook(GB *gb);
void greatFairy_state8_hook(GB *gb);
void greatFairy_state9_hook(GB *gb);
void greatFairy_updateZPosition_hook(GB *gb);
void greatFairy_checkLinkApproached_hook(GB *gb);
void greatFairy_spawnCirclingHeart_hook(GB *gb);
void greatFairy_createPuff_hook(GB *gb);
void greatFairy_playSoundEvery8Frames_hook(GB *gb);

static uint16_t greatFairy_jump_table(GB *gb) {
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
// ENEMY_GREAT_FAIRY
//
// Variables:
//   relatedObj2: Reference to INTERAC_PUFF
//   var30: Counter used to update Z-position as she floats up and down
//   var31: Number of hearts spawned (the ones that circle around Link)
// ==================================================================================================
void enemyCode38_hook(GB *gb) {
  BASE(enemyCode38);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = greatFairy_jump_table(gb);
    if (target == SYM(greatFairy_state_uninitialized)) { greatFairy_state_uninitialized_hook(gb); return; }
    if (target == SYM(greatFairy_state1)) { greatFairy_state1_hook(gb); return; }
    if (target == SYM(greatFairy_state2)) { greatFairy_state2_hook(gb); return; }
    if (target == SYM(greatFairy_state3)) { greatFairy_state3_hook(gb); return; }
    if (target == SYM(greatFairy_state4)) { greatFairy_state4_hook(gb); return; }
    if (target == SYM(greatFairy_state5)) { greatFairy_state5_hook(gb); return; }
    if (target == SYM(greatFairy_state6)) { greatFairy_state6_hook(gb); return; }
    if (target == SYM(greatFairy_state7)) { greatFairy_state7_hook(gb); return; }
    if (target == SYM(greatFairy_state8)) { greatFairy_state8_hook(gb); return; }
    if (target == SYM(greatFairy_state9)) { greatFairy_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

void greatFairy_state_uninitialized_hook(GB *gb) {
  BASE(greatFairy_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0xf0);
  RET(b_+7); return; // ret
}

// Create puff
void greatFairy_state1_hook(GB *gb) {
  BASE(greatFairy_state1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, greatFairy_createPuff_hook, SYM(greatFairy_createPuff), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x11);
  CYC(b_+11, b_+13); A = 0x0f; // MUS_FAIRY_FOUNTAIN
  CYC(b_+13, b_+16); mem_wr(gb, wActiveMusic, A);
  RET(b_+16); return; // ret
}

// Waiting for puff to disappear
void greatFairy_state2_hook(GB *gb) {
  BASE(greatFairy_state2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x21; // Object.animParameter
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CALL_C(b_+8, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), SYM(greatFairy_state3));
  greatFairy_state3_hook(gb); return; // fallthrough
}

// Waiting for Link to approach
void greatFairy_state3_hook(GB *gb) {
  BASE(greatFairy_state3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, greatFairy_checkLinkApproached_hook, SYM(greatFairy_checkLinkApproached), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); greatFairy_animate_hook(gb); return; } // jr nc
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x80;
  CYC(b_+7, b_+10); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+10, b_+12); A = 0x21; // DISABLE_COMPANION|DISABLE_LINK
  CYC(b_+12, b_+15); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+15, b_+18); SET_HL(wLinkHealth);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+19, b_+20); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+22); A = 0x04;
  CYC(b_+22, b_+25); SET_BC((SYM(ecom_fallingInHole_b0e) + 35)); // TX_4100
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto L_5065; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); A = 30;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); A = 0x08;
  CYC(b_+34, b_+37); SET_BC((SYM(ecom_fallingInHole_b0e) + 40)); // TX_4105

L_5065:
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CALL_C(b_+40, showText_hook, SYM(showText), SYM(greatFairy_animate));
  greatFairy_animate_hook(gb); return; // fallthrough
}

void greatFairy_animate_hook(GB *gb) {
  BASE(greatFairy_animate);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, greatFairy_updateZPosition_hook, SYM(greatFairy_updateZPosition), b_+3);
  CALL_C(b_+3, enemyAnimate_hook, SYM(enemyAnimate), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+13, b_+16); objectSetVisiblec1_hook(gb); return; } // jp c
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+19); objectSetVisiblec2_hook(gb); return; // jp
}

// Begin healing Link
void greatFairy_state4_hook(GB *gb) {
  BASE(greatFairy_state4);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0c);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x09); // [counter2]
  greatFairy_state5_hook(gb); return; // fallthrough
}

// Spawning hearts
void greatFairy_state5_hook(GB *gb) {
  BASE(greatFairy_state5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, greatFairy_playSoundEvery8Frames_hook, SYM(greatFairy_playSoundEvery8Frames), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  if (F & FZ) { CYCT(b_+12, b_+14); goto spawnedAllHearts; } // jr z
  CYC(b_+12, b_+14);
  CALL_C(b_+14, greatFairy_spawnCirclingHeart_hook, SYM(greatFairy_spawnCirclingHeart), b_+17);
  CYCT(b_+17, b_+19); greatFairy_animate_hook(gb); return; // jr

spawnedAllHearts:
  CYC(b_+19, b_+20); L = alu_dec8(gb, L);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 30); // [counter1]
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYCT(b_+25, b_+27); greatFairy_animate_hook(gb); return; // jr
}

// Hearts have all spawned, are now circling around Link
void greatFairy_state6_hook(GB *gb) {
  BASE(greatFairy_state6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, greatFairy_playSoundEvery8Frames_hook, SYM(greatFairy_playSoundEvery8Frames), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+11, b_+13); A = 0x29; // TREASURE_HEART_REFILL
  CYC(b_+13, b_+15); C = 0x40; // MAX_LINK_HEALTH
  CALL_C(b_+15, giveTreasure_hook, SYM(giveTreasure), SYM(greatFairy_state7));
  greatFairy_state7_hook(gb); return; // fallthrough
}

// Waiting for all hearts to disappear
void greatFairy_state7_hook(GB *gb) {
  BASE(greatFairy_state7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, greatFairy_playSoundEvery8Frames_hook, SYM(greatFairy_playSoundEvery8Frames), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(b_+7, b_+9);
  CALL_C(b_+9, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x1e);
  greatFairy_state8_hook(gb); return; // fallthrough
}

// About to disappear; staying in place for 30 frames
void greatFairy_state8_hook(GB *gb) {
  BASE(greatFairy_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+13); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+13, b_+16); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+16, b_+18); A = 0x91; // SND_FAIRYCUTSCENE
  CALL_C(b_+18, playSound_b00_hook, SYM(playSound_b00), SYM(greatFairy_state9));
  greatFairy_state9_hook(gb); return; // fallthrough
}

// Disappearing
void greatFairy_state9_hook(GB *gb) {
  BASE(greatFairy_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); enemyDelete_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CALL_C(b_+6, greatFairy_animate_hook, SYM(greatFairy_animate), b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(b_+19); return; // ret
}

void greatFairy_updateZPosition_hook(GB *gb) {
  BASE(greatFairy_updateZPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0x18);
  CYC(b_+11, b_+13); alu_swap_a(gb);
  CYC(b_+13, b_+14); alu_rlca(gb);
  CYC(b_+14, b_+16); alu_sub(gb, 0x02);
  CYC(b_+16, b_+18); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto L_5104; } // jr nz
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); alu_cpl(gb);
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);

L_5104:
  CYC(b_+22, b_+24); alu_sub(gb, 0x10);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  RET(b_+27); return; // ret
}

// @param[out]  cflag  c if Link approached
void greatFairy_checkLinkApproached_hook(GB *gb) {
  BASE(greatFairy_checkLinkApproached);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } // ret nc
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+7, b_+9); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+9, b_+10); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+12); alu_sub(gb, 0x10);
  CYC(b_+12, b_+14); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(b_+14); return; } // ret nc
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+17, b_+19); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+19, b_+20); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+22); alu_add(gb, 0x18);
  CYC(b_+22, b_+24); alu_cp(gb, 0x31);
  RET(b_+24); return; // ret
}

void greatFairy_spawnCirclingHeart_hook(GB *gb) {
  BASE(greatFairy_spawnCirclingHeart);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x30); // PART_GREAT_FAIRY_HEART
  CYC(b_+6, b_+8); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+8, b_+10); A = 0x80; // Enemy.start
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+11, b_+12); mem_wr(gb, HL, D);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+16); return; // ret
}

void greatFairy_createPuff_hook(GB *gb) {
  BASE(greatFairy_createPuff);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x0502); // INTERAC_PUFF,$02
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = H;
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x40); // Interaction.start
  CYC(b_+14, b_+15); alu_xor(gb, A);
  RET(b_+15); return; // ret
}

void greatFairy_playSoundEvery8Frames_hook(GB *gb) {
  BASE(greatFairy_playSoundEvery8Frames);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x8c; // SND_FAIRY_HEAL
  CYC(b_+8, b_+11); playSound_b00_hook(gb); return; // jp
}
