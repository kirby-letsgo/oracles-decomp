#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x563b, ecom_checkHazards_b0d_hook, 0x4051, 0x563e);
  if (F & FZ) { CYCT(0x563e, 0x5640); goto normalStatus; } // jr z
  CYC(0x563e, 0x5640);
  CYC(0x5640, 0x5642); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x5642, 0x5645); buzzblob_checkShowText_hook(gb); return; } // jp c
  CYC(0x5642, 0x5645);
  if (F & FZ) { CYCT(0x5645, 0x5648); enemyDie_hook(gb); return; } // jp z
  CYC(0x5645, 0x5648);
  CYC(0x5648, 0x5649); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5649, 0x564c); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x5649, 0x564c);
  CYC(0x564c, 0x564d); H = D;
  CYC(0x564d, 0x564f); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x564f, 0x5650); A = mem_rd(gb, HL);
  CYC(0x5650, 0x5652); alu_cp(gb, 0x9a); // $80|ITEMCOLLISION_MYSTERY_SEED
  if (F & FZ) { CYCT(0x5652, 0x5654); goto becomeCukeman; } // jr z
  CYC(0x5652, 0x5654);
  CYC(0x5654, 0x5656); alu_cp(gb, 0xa0); // $80|ITEMCOLLISION_ELECTRIC_SHOCK
  if (!(F & FZ)) { RET_TAKEN(0x5656); return; } // ret nz
  CYC(0x5656, 0x5657);
  CYC(0x5657, 0x5659); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5659, 0x565b); mem_wr(gb, HL, 0x0a);
  CYC(0x565b, 0x565d); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x565d, 0x565f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4))); // res 4,(hl)
  CYC(0x565f, 0x5661); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x5661, 0x5663); mem_wr(gb, HL, 0x00);
  CYC(0x5663, 0x5665); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5665, 0x5667); mem_wr(gb, HL, 0x3c);
  CYC(0x5667, 0x5669); A = 0x01;
  CYC(0x5669, 0x566c); enemySetAnimation_hook(gb); return; // jp

becomeCukeman:
  CYC(0x566c, 0x566e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x566e, 0x5670); A = 0x02;
  CYC(0x5670, 0x5671); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5671); return; } // ret z
  CYC(0x5671, 0x5672);
  CYC(0x5672, 0x5673); mem_wr(gb, HL, A);
  CALL_C(0x5673, enemySetAnimation_hook, 0x282b, 0x5676);
  CYC(0x5676, 0x5678); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x5678, 0x567b); objectAddToAButtonSensitiveObjectList_hook(gb); return; // jp

normalStatus:
  CALL_C(0x567b, buzzblob_checkShowText_hook, 0x5701, 0x567e);
  CALL_C(0x567e, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x5681);
  CYC(0x5681, 0x5683); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5683, 0x5684); A = mem_rd(gb, DE);
  {
    CYC(0x5684, 0x5685); push_effect(gb, 0x5685);
    uint16_t target = enemyCode18_jump_table(gb);
    if (target == 0x569b) { buzzblob_state_uninitialized_hook(gb); return; }
    if (target == 0x56c3) { buzzblob_state_stub_hook(gb); return; }
    if (target == 0x56a7) { buzzblob_state_scentSeed_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x56c4) { buzzblob_state8_hook(gb); return; }
    if (target == 0x56d9) { buzzblob_state9_hook(gb); return; }
    if (target == 0x56e7) { buzzblob_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:569b, bare global.
void buzzblob_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x569b, 0x569d); A = 0x0a; // SPEED_40
  CALL_C(0x569d, ecom_setSpeedAndState8AndVisible_b0d_hook, 0x435e, 0x56a0);
  CYC(0x56a0, 0x56a2); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x56a2, 0x56a3); A = mem_rd(gb, HL);
  CYC(0x56a3, 0x56a5); alu_or(gb, 0x30);
  CYC(0x56a5, 0x56a6); mem_wr(gb, HL, A);
  RET(0x56a6); return; // ret
}

// 0d:56a7, bare global.
void buzzblob_state_scentSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56a7, 0x56aa); A = mem_rd(gb, wScentSeedActive);
  CYC(0x56aa, 0x56ab); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x56ab, 0x56ad); goto seek; } // jr nz
  CYC(0x56ab, 0x56ad);
  CYC(0x56ad, 0x56af); A = 0x08;
  CYC(0x56af, 0x56b0); mem_wr(gb, DE, A); // [state] = 8
  CYC(0x56b0, 0x56b2); buzzblob_animate_hook(gb); return; // jr

seek:
  CALL_C(0x56b2, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x56b5);
  CYC(0x56b5, 0x56b7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x56b7, 0x56b8); A = mem_rd(gb, DE);
  CYC(0x56b8, 0x56ba); alu_add(gb, 0x04);
  CYC(0x56ba, 0x56bc); alu_and(gb, 0x18);
  CYC(0x56bc, 0x56bd); mem_wr(gb, DE, A);
  CALL_C(0x56bd, ecom_applyVelocityForSideviewEnemy_b0d_hook, 0x4153, 0x56c0);
  CYC(0x56c0, 0x56c3); enemyAnimate_hook(gb); return; // jp
}

// 0d:56c3, bare global.
void buzzblob_state_stub_hook(GB *gb) {
  RET(0x56c3); return; // ret
}

// 0d:56c4, bare global; choosing a direction and duration to move.
void buzzblob_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56c4, 0x56c6); A = 0x09;
  CYC(0x56c6, 0x56c7); mem_wr(gb, DE, A); // [state] = 9
  CYC(0x56c7, 0x56ca); SET_BC(0x1c30);
  CALL_C(0x56ca, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x56cd);
  CYC(0x56cd, 0x56cf); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x56cf, 0x56d1); A = 0x30;
  CYC(0x56d1, 0x56d2); alu_add(gb, C);
  CYC(0x56d2, 0x56d3); mem_wr(gb, DE, A);
  CYC(0x56d3, 0x56d5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x56d5, 0x56d6); A = B;
  CYC(0x56d6, 0x56d7); mem_wr(gb, DE, A);
  CYC(0x56d7, 0x56d9); buzzblob_animate_hook(gb); return; // jr
}

// 0d:56d9, bare global; moving in some direction for a certain amount of time.
void buzzblob_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56d9, ecom_decCounter1_b0d_hook, 0x439a, 0x56dc);
  if (F & FZ) { CYCT(0x56dc, 0x56de); buzzblob_chooseNewDirection_hook(gb); return; } // jr z
  CYC(0x56dc, 0x56de);
  CALL_C(0x56de, ecom_bounceOffWallsAndHoles_b0d_hook, 0x42de, 0x56e1);
  CALL_C(0x56e1, objectApplySpeed_hook, 0x201d, 0x56e4);
  buzzblob_animate_hook(gb); return; // fallthrough
}

// 0d:56e4, bare global.
void buzzblob_animate_hook(GB *gb) {
  CYC(0x56e4, 0x56e7); enemyAnimate_hook(gb); return; // jp
}

// 0d:56e7, bare global; "shocking Link" state.
void buzzblob_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56e7, ecom_decCounter1_b0d_hook, 0x439a, 0x56ea);
  if (!(F & FZ)) { CYCT(0x56ea, 0x56ec); buzzblob_animate_hook(gb); return; } // jr nz
  CYC(0x56ea, 0x56ec);
  CYC(0x56ec, 0x56ee); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x56ee, 0x56ef); A = mem_rd(gb, DE);
  CALL_C(0x56ef, enemySetAnimation_hook, 0x282b, 0x56f2);
  buzzblob_chooseNewDirection_hook(gb); return; // fallthrough
}

// 0d:56f2, bare global.
void buzzblob_chooseNewDirection_hook(GB *gb) {
  CYC(0x56f2, 0x56f3); H = D;
  CYC(0x56f3, 0x56f5); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x56f5, 0x56f7); mem_wr(gb, HL, 0x08); // will choose new direction in state 8
  CYC(0x56f7, 0x56f9); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x56f9, 0x56fb); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4))); // set 4,(hl)
  CYC(0x56fb, 0x56fd); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x56fd, 0x56ff); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x56ff, 0x5701); buzzblob_animate_hook(gb); return; // jr
}

// 0d:5701, bare global; boundary item shared with the buzzblob source file.
void buzzblob_checkShowText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5701, 0x5703); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x5703, 0x5704); A = mem_rd(gb, DE);
  CYC(0x5704, 0x5705); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5705); return; } // ret z
  CYC(0x5705, 0x5706);
  CYC(0x5706, 0x5707); alu_xor(gb, A);
  CYC(0x5707, 0x5708); mem_wr(gb, DE, A);
  CALL_C(0x5708, getRandomNumber_noPreserveVars_hook, 0x0453, 0x570b);
  CYC(0x570b, 0x570d); alu_and(gb, 0x07);
  CYC(0x570d, 0x570f); alu_add(gb, 0x1e); // <TX_2f1e
  CYC(0x570f, 0x5710); C = A;
  CYC(0x5710, 0x5712); B = 0x2f; // >TX_2f00
  CYC(0x5712, 0x5715); showText_hook(gb); return; // jp
}
