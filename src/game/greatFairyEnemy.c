#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x5004, 0x5006); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5006, 0x5007); A = mem_rd(gb, DE);
  {
    CYC(0x5007, 0x5008); push_effect(gb, 0x5008);
    uint16_t target = greatFairy_jump_table(gb);
    if (target == 0x501c) { greatFairy_state_uninitialized_hook(gb); return; }
    if (target == 0x5024) { greatFairy_state1_hook(gb); return; }
    if (target == 0x5035) { greatFairy_state2_hook(gb); return; }
    if (target == 0x5040) { greatFairy_state3_hook(gb); return; }
    if (target == 0x507e) { greatFairy_state4_hook(gb); return; }
    if (target == 0x5088) { greatFairy_state5_hook(gb); return; }
    if (target == 0x50a3) { greatFairy_state6_hook(gb); return; }
    if (target == 0x50b5) { greatFairy_state7_hook(gb); return; }
    if (target == 0x50c5) { greatFairy_state8_hook(gb); return; }
    if (target == 0x50da) { greatFairy_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

void greatFairy_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x501c, 0x501d); H = D;
  CYC(0x501d, 0x501e); L = E;
  CYC(0x501e, 0x501f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x501f, 0x5021); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5021, 0x5023); mem_wr(gb, HL, 0xf0);
  RET(0x5023); return; // ret
}

// Create puff
void greatFairy_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5024, greatFairy_createPuff_hook, 0x5134, 0x5027);
  if (!(F & FZ)) { RET_TAKEN(0x5027); return; } // ret nz
  CYC(0x5027, 0x5028);
  CYC(0x5028, 0x502a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x502a, 0x502b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x502b, 0x502d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x502d, 0x502f); mem_wr(gb, HL, 0x11);
  CYC(0x502f, 0x5031); A = 0x0f; // MUS_FAIRY_FOUNTAIN
  CYC(0x5031, 0x5034); mem_wr(gb, wActiveMusic, A);
  RET(0x5034); return; // ret
}

// Waiting for puff to disappear
void greatFairy_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5035, 0x5037); A = 0x21; // Object.animParameter
  CALL_C(0x5037, objectGetRelatedObject2Var_hook, 0x2164, 0x503a);
  CYC(0x503a, 0x503c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x503c); return; } // ret z
  CYC(0x503c, 0x503d);
  CALL_C(0x503d, ecom_incState_b0e_hook, 0x4000, 0x5040);
  greatFairy_state3_hook(gb); return; // fallthrough
}

// Waiting for Link to approach
void greatFairy_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5040, greatFairy_checkLinkApproached_hook, 0x510a, 0x5043);
  if (!(F & FC)) { CYCT(0x5043, 0x5045); greatFairy_animate_hook(gb); return; } // jr nc
  CYC(0x5043, 0x5045);
  CYC(0x5045, 0x5047); A = 0x80;
  CYC(0x5047, 0x504a); mem_wr(gb, wMenuDisabled, A);
  CYC(0x504a, 0x504c); A = 0x21; // DISABLE_COMPANION|DISABLE_LINK
  CYC(0x504c, 0x504f); mem_wr(gb, wDisabledObjects, A);
  CYC(0x504f, 0x5052); SET_HL(wLinkHealth);
  CYC(0x5052, 0x5053); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5053, 0x5054); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x5054, 0x5056); A = 0x04;
  CYC(0x5056, 0x5059); SET_BC(0x4100); // TX_4100
  if (!(F & FZ)) { CYCT(0x5059, 0x505b); goto L_5065; } // jr nz
  CYC(0x5059, 0x505b);
  CYC(0x505b, 0x505d); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x505d, 0x505f); A = 30;
  CYC(0x505f, 0x5060); mem_wr(gb, DE, A);
  CYC(0x5060, 0x5062); A = 0x08;
  CYC(0x5062, 0x5065); SET_BC(0x4105); // TX_4105

L_5065:
  CYC(0x5065, 0x5067); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5067, 0x5068); mem_wr(gb, DE, A);
  CALL_C(0x5068, showText_hook, 0x1872, 0x506b);
  greatFairy_animate_hook(gb); return; // fallthrough
}

void greatFairy_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x506b, greatFairy_updateZPosition_hook, 0x50ee, 0x506e);
  CALL_C(0x506e, enemyAnimate_hook, 0x2818, 0x5071);
  CYC(0x5071, 0x5073); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5073, 0x5074); A = mem_rd(gb, DE);
  CYC(0x5074, 0x5075); B = A;
  CYC(0x5075, 0x5077); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5077, 0x5078); alu_cp(gb, B);
  if (F & FC) { CYCT(0x5078, 0x507b); objectSetVisiblec1_hook(gb); return; } // jp c
  CYC(0x5078, 0x507b);
  CYC(0x507b, 0x507e); objectSetVisiblec2_hook(gb); return; // jp
}

// Begin healing Link
void greatFairy_state4_hook(GB *gb) {
  CYC(0x507e, 0x507f); H = D;
  CYC(0x507f, 0x5080); L = E;
  CYC(0x5080, 0x5081); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5081, 0x5083); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5083, 0x5085); mem_wr(gb, HL, 0x0c);
  CYC(0x5085, 0x5086); L = alu_inc8(gb, L);
  CYC(0x5086, 0x5088); mem_wr(gb, HL, 0x09); // [counter2]
  greatFairy_state5_hook(gb); return; // fallthrough
}

// Spawning hearts
void greatFairy_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5088, greatFairy_playSoundEvery8Frames_hook, 0x5144, 0x508b);
  CALL_C(0x508b, ecom_decCounter1_b0e_hook, 0x439a, 0x508e);
  if (!(F & FZ)) { CYCT(0x508e, 0x5090); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(0x508e, 0x5090);
  CYC(0x5090, 0x5092); mem_wr(gb, HL, 0x0c); // [counter1]
  CYC(0x5092, 0x5093); L = alu_inc8(gb, L);
  CYC(0x5093, 0x5094); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  if (F & FZ) { CYCT(0x5094, 0x5096); goto spawnedAllHearts; } // jr z
  CYC(0x5094, 0x5096);
  CALL_C(0x5096, greatFairy_spawnCirclingHeart_hook, 0x5123, 0x5099);
  CYCT(0x5099, 0x509b); greatFairy_animate_hook(gb); return; // jr

spawnedAllHearts:
  CYC(0x509b, 0x509c); L = alu_dec8(gb, L);
  CYC(0x509c, 0x509e); mem_wr(gb, HL, 30); // [counter1]
  CYC(0x509e, 0x50a0); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x50a0, 0x50a1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYCT(0x50a1, 0x50a3); greatFairy_animate_hook(gb); return; // jr
}

// Hearts have all spawned, are now circling around Link
void greatFairy_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50a3, greatFairy_playSoundEvery8Frames_hook, 0x5144, 0x50a6);
  CALL_C(0x50a6, ecom_decCounter1_b0e_hook, 0x439a, 0x50a9);
  if (!(F & FZ)) { CYCT(0x50a9, 0x50ab); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(0x50a9, 0x50ab);
  CYC(0x50ab, 0x50ad); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x50ad, 0x50ae); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x50ae, 0x50b0); A = 0x29; // TREASURE_HEART_REFILL
  CYC(0x50b0, 0x50b2); C = 0x40; // MAX_LINK_HEALTH
  CALL_C(0x50b2, giveTreasure_hook, 0x171c, 0x50b5);
  greatFairy_state7_hook(gb); return; // fallthrough
}

// Waiting for all hearts to disappear
void greatFairy_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50b5, greatFairy_playSoundEvery8Frames_hook, 0x5144, 0x50b8);
  CYC(0x50b8, 0x50ba); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x50ba, 0x50bb); A = mem_rd(gb, DE);
  CYC(0x50bb, 0x50bc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x50bc, 0x50be); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(0x50bc, 0x50be);
  CALL_C(0x50be, ecom_incState_b0e_hook, 0x4000, 0x50c1);
  CYC(0x50c1, 0x50c3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x50c3, 0x50c5); mem_wr(gb, HL, 0x1e);
  greatFairy_state8_hook(gb); return; // fallthrough
}

// About to disappear; staying in place for 30 frames
void greatFairy_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50c5, ecom_decCounter1_b0e_hook, 0x439a, 0x50c8);
  if (!(F & FZ)) { CYCT(0x50c8, 0x50ca); greatFairy_animate_hook(gb); return; } // jr nz
  CYC(0x50c8, 0x50ca);
  CYC(0x50ca, 0x50cc); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(0x50cc, 0x50cd); L = E;
  CYC(0x50cd, 0x50ce); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x50ce, 0x50cf); alu_xor(gb, A);
  CYC(0x50cf, 0x50d2); mem_wr(gb, wDisabledObjects, A);
  CYC(0x50d2, 0x50d5); mem_wr(gb, wMenuDisabled, A);
  CYC(0x50d5, 0x50d7); A = 0x91; // SND_FAIRYCUTSCENE
  CALL_C(0x50d7, playSound_b00_hook, 0x0c98, 0x50da);
  greatFairy_state9_hook(gb); return; // fallthrough
}

// Disappearing
void greatFairy_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50da, ecom_decCounter1_b0e_hook, 0x439a, 0x50dd);
  if (F & FZ) { CYCT(0x50dd, 0x50e0); enemyDelete_hook(gb); return; } // jp z
  CYC(0x50dd, 0x50e0);
  CALL_C(0x50e0, greatFairy_animate_hook, 0x506b, 0x50e3);
  CYC(0x50e3, 0x50e4); H = D;
  CYC(0x50e4, 0x50e6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x50e6, 0x50e8); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x50e8); return; } // ret nz
  CYC(0x50e8, 0x50e9);
  CYC(0x50e9, 0x50eb); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x50eb, 0x50ed); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(0x50ed); return; // ret
}

void greatFairy_updateZPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50ee, 0x50ef); H = D;
  CYC(0x50ef, 0x50f1); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x50f1, 0x50f2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x50f2, 0x50f3); A = mem_rd(gb, HL);
  CYC(0x50f3, 0x50f5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x50f5); return; } // ret nz
  CYC(0x50f5, 0x50f6);
  CYC(0x50f6, 0x50f7); A = mem_rd(gb, HL);
  CYC(0x50f7, 0x50f9); alu_and(gb, 0x18);
  CYC(0x50f9, 0x50fb); alu_swap_a(gb);
  CYC(0x50fb, 0x50fc); alu_rlca(gb);
  CYC(0x50fc, 0x50fe); alu_sub(gb, 0x02);
  CYC(0x50fe, 0x5100); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5100, 0x5102); goto L_5104; } // jr nz
  CYC(0x5100, 0x5102);
  CYC(0x5102, 0x5103); alu_cpl(gb);
  CYC(0x5103, 0x5104); A = alu_inc8(gb, A);

L_5104:
  CYC(0x5104, 0x5106); alu_sub(gb, 0x10);
  CYC(0x5106, 0x5108); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x5108, 0x5109); mem_wr(gb, HL, A);
  RET(0x5109); return; // ret
}

// @param[out]  cflag  c if Link approached
void greatFairy_checkLinkApproached_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x510a, checkLinkVulnerable_hook, 0x1d28, 0x510d);
  if (!(F & FC)) { RET_TAKEN(0x510d); return; } // ret nc
  CYC(0x510d, 0x510e);
  CYC(0x510e, 0x510f); H = D;
  CYC(0x510f, 0x5111); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5111, 0x5113); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x5113, 0x5114); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5114, 0x5116); alu_sub(gb, 0x10);
  CYC(0x5116, 0x5118); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(0x5118); return; } // ret nc
  CYC(0x5118, 0x5119);
  CYC(0x5119, 0x511b); L = ENEMY_BASE + OBJ_XH;
  CYC(0x511b, 0x511d); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x511d, 0x511e); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x511e, 0x5120); alu_add(gb, 0x18);
  CYC(0x5120, 0x5122); alu_cp(gb, 0x31);
  RET(0x5122); return; // ret
}

void greatFairy_spawnCirclingHeart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5123, getFreePartSlot_hook, 0x3e8e, 0x5126);
  if (!(F & FZ)) { RET_TAKEN(0x5126); return; } // ret nz
  CYC(0x5126, 0x5127);
  CYC(0x5127, 0x5129); mem_wr(gb, HL, 0x30); // PART_GREAT_FAIRY_HEART
  CYC(0x5129, 0x512b); L = PART_BASE + OBJ_RELATED1;
  CYC(0x512b, 0x512d); A = 0x80; // Enemy.start
  CYC(0x512d, 0x512e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x512e, 0x512f); mem_wr(gb, HL, D);
  CYC(0x512f, 0x5130); H = D;
  CYC(0x5130, 0x5132); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5132, 0x5133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x5133); return; // ret
}

void greatFairy_createPuff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5134, 0x5137); SET_BC(0x0502); // INTERAC_PUFF,$02
  CALL_C(0x5137, objectCreateInteraction_hook, 0x24c5, 0x513a);
  if (!(F & FZ)) { RET_TAKEN(0x513a); return; } // ret nz
  CYC(0x513a, 0x513b);
  CYC(0x513b, 0x513c); A = H;
  CYC(0x513c, 0x513d); H = D;
  CYC(0x513d, 0x513f); L = ENEMY_BASE + OBJ_RELATED2 + 1;
  CYC(0x513f, 0x5140); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x5140, 0x5142); mem_wr(gb, HL, 0x40); // Interaction.start
  CYC(0x5142, 0x5143); alu_xor(gb, A);
  RET(0x5143); return; // ret
}

void greatFairy_playSoundEvery8Frames_hook(GB *gb) {
  CYC(0x5144, 0x5147); A = mem_rd(gb, wFrameCounter);
  CYC(0x5147, 0x5149); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x5149); return; } // ret nz
  CYC(0x5149, 0x514a);
  CYC(0x514a, 0x514c); A = 0x8c; // SND_FAIRY_HEAL
  CYC(0x514c, 0x514f); playSound_b00_hook(gb); return; // jp
}
