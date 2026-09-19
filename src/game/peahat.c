#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void peahat_state_uninitialized_hook(GB *gb);
void peahet_state_stub_hook(GB *gb);
void peahat_state8_hook(GB *gb);
void peahat_state9_hook(GB *gb);
void peahat_stateA_hook(GB *gb);
void peahat_animate_hook(GB *gb);
void peahat_stateB_hook(GB *gb);
void peahat_updateEnemyCollisionMode_hook(GB *gb);
void peahat_updatePosition_hook(GB *gb);

static uint16_t peahat_jump_table(GB *gb) {
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

static void peahat_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_PEAHAT
// ==================================================================================================
void enemyCode3e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x57a7, 0x57a9); goto normalStatus; } // jr z
  CYC(0x57a7, 0x57a9);
  CYC(0x57a9, 0x57ab); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x57ab); return; } // ret c
  CYC(0x57ab, 0x57ac);
  if (F & FZ) { CYCT(0x57ac, 0x57af); enemyDie_hook(gb); return; } // jp z
  CYC(0x57ac, 0x57af);

  // ENEMYSTATUS_KNOCKBACK
  CYC(0x57af, 0x57b1); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x57b1, 0x57b2); A = mem_rd(gb, DE);
  CYC(0x57b2, 0x57b4); alu_cp(gb, 0x58); // ENEMYCOLLISION_PEAHAT
  if (!(F & FZ)) { RET_TAKEN(0x57b4); return; } // ret nz
  CYC(0x57b4, 0x57b5);

normalStatus:
  CALL_C(0x57b5, peahat_updateEnemyCollisionMode_hook, 0x5849, 0x57b8);
  CYC(0x57b8, 0x57ba); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x57ba, 0x57bb); A = mem_rd(gb, DE);
  {
    CYC(0x57bb, 0x57bc); push_effect(gb, 0x57bc);
    uint16_t target = peahat_jump_table(gb);
    if (target == 0x57d4) { peahat_state_uninitialized_hook(gb); return; }
    if (target == 0x57db) { peahet_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x57dc) { peahat_state8_hook(gb); return; }
    if (target == 0x57f4) { peahat_state9_hook(gb); return; }
    if (target == 0x580f) { peahat_stateA_hook(gb); return; }
    if (target == 0x582b) { peahat_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:57d4, bare global; jump-table target from enemyCode3e.
void peahat_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57d4, ecom_setSpeedAndState8AndVisible_b0e_hook, 0x435e, 0x57d7);
  CYC(0x57d7, 0x57d9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x57d9, 0x57da); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x57da); return; // ret
}

// 0e:57db, bare global; jump-table target from enemyCode3e.
void peahet_state_stub_hook(GB *gb) {
  RET(0x57db); return; // ret
}

// 0e:57dc, bare global; jump-table target from enemyCode3e. Stationary for [counter1]
// frames.
void peahat_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57dc, ecom_decCounter1_b0e_hook, 0x439a, 0x57df);
  if (!(F & FZ)) { RET_TAKEN(0x57df); return; } // ret nz
  CYC(0x57df, 0x57e0);
  CYC(0x57e0, 0x57e2); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x57e2, 0x57e3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x57e3, 0x57e5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x57e5, 0x57e7); mem_wr(gb, HL, 0x7f);
  CYC(0x57e7, 0x57e9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x57e9, 0x57eb); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x57eb, 0x57ed); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x57ed, 0x57ef); mem_wr(gb, HL, 0x0f);
  CALL_C(0x57ef, objectSetVisiblec1_hook, 0x1e3c, 0x57f2);
  CYC(0x57f2, 0x57f4); peahat_animate_hook(gb); return; // jr
}

// 0e:57f4, bare global; jump-table target from enemyCode3e. Accelerating.
void peahat_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57f4, ecom_decCounter1_b0e_hook, 0x439a, 0x57f7);
  if (!(F & FZ)) { CYCT(0x57f7, 0x57fa); peahat_updatePosition_hook(gb); return; } // jp nz
  CYC(0x57f7, 0x57fa);
  CYC(0x57fa, 0x57fc); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x57fc, 0x57fd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x57fd, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5800);
  CYC(0x5800, 0x5802); alu_and(gb, 0x07);
  CYC(0x5802, 0x5805); SET_HL(0x58aa); // @counter1Vals
  peahat_addAToHl_from_rst(gb, 0x5806);
  CYC(0x5806, 0x5808); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5808, 0x5809); A = mem_rd(gb, HL);
  CYC(0x5809, 0x580a); mem_wr(gb, DE, A);
  CALL_C(0x580a, ecom_setRandomAngle_b0e_hook, 0x43cf, 0x580d);
  CYC(0x580d, 0x580f); peahat_animate_hook(gb); return; // jr
}

// 0e:580f, bare global; jump-table target from enemyCode3e. Flying around at top speed.
void peahat_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x580f, ecom_decCounter1_b0e_hook, 0x439a, 0x5812);
  if (F & FZ) { CYCT(0x5812, 0x5814); goto beginSlowingDown; } // jr z
  CYC(0x5812, 0x5814);

  // Change angle every 32 frames
  CYC(0x5814, 0x5815); A = mem_rd(gb, HL);
  CYC(0x5815, 0x5817); alu_and(gb, 0x1f);
  if (F & FZ) CALL_C_CC(0x5817, ecom_setRandomAngle_b0e_hook, 0x43cf, 0x581a); else CYC(0x5817, 0x581a); // call z
  CALL_C(0x581a, objectApplySpeed_hook, 0x201d, 0x581d);
  CALL_C(0x581d, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x5820);
  CYC(0x5820, 0x5822); peahat_animate_hook(gb); return; // jr

beginSlowingDown:
  CYC(0x5822, 0x5823); L = E;
  CYC(0x5823, 0x5824); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x5824, 0x5826); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5826, 0x5828); mem_wr(gb, HL, 0x00);
  peahat_animate_hook(gb); return; // fallthrough
}

// 0e:5828, bare global; falls into from peahat_stateA, also reached by genuine jr from
// peahat_state8, peahat_state9 and peahat_stateB.
void peahat_animate_hook(GB *gb) {
  CYC(0x5828, 0x582b); enemyAnimate_hook(gb); return; // jp
}

// 0e:582b, bare global; jump-table target from enemyCode3e. Slowing down.
void peahat_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x582b, 0x582c); H = D;
  CYC(0x582c, 0x582e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x582e, 0x582f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x582f, 0x5831); A = 0x80;
  CYC(0x5831, 0x5832); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5832, 0x5835); peahat_updatePosition_hook(gb); return; } // jp nz
  CYC(0x5832, 0x5835);

  // Go to state 8 for $80 frames (if tile is non-solid) or 1 frame (if tile is
  // solid).
  CYC(0x5835, 0x5837); mem_wr(gb, HL, 0x80); // [counter1]
  PUSH(0x5837, HL);
  CALL_C(0x5838, objectGetTileCollisions_hook, 0x14ad, 0x583b);
  SET_HL(POP(0x583b));
  if (F & FZ) { CYCT(0x583c, 0x583e); goto keepCounter; } // jr z
  CYC(0x583c, 0x583e);
  CYC(0x583e, 0x5840); mem_wr(gb, HL, 0x01); // [counter1]

keepCounter:
  CYC(0x5840, 0x5842); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5842, 0x5844); mem_wr(gb, HL, 0x08);
  CALL_C(0x5844, objectSetVisiblec2_hook, 0x1e45, 0x5847);
  CYC(0x5847, 0x5849); peahat_animate_hook(gb); return; // jr
}

// 0e:5849, bare global; called from enemyCode3e.
void peahat_updateEnemyCollisionMode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5849, 0x584b); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x584b, 0x584c); A = mem_rd(gb, DE);
  CYC(0x584c, 0x584d); alu_or(gb, A);
  CYC(0x584d, 0x584f); A = 0x2e; // ENEMYCOLLISION_PEAHAT_VULNERABLE
  if (F & FZ) { CYCT(0x584f, 0x5851); goto setVars; } // jr z
  CYC(0x584f, 0x5851);
  CYC(0x5851, 0x5853); A = 0x58; // ENEMYCOLLISION_PEAHAT

setVars:
  CYC(0x5853, 0x5855); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5855, 0x5856); mem_wr(gb, DE, A);
  RET(0x5856); return; // ret
}

// 0e:5857, bare global; called from peahat_state9 and peahat_stateB. Adjusts speed based
// on counter1, updates position, animates.
void peahat_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5857, 0x5859); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5859, 0x585a); A = mem_rd(gb, DE);
  CYC(0x585a, 0x585b); A = alu_dec8(gb, A);
  CYC(0x585b, 0x585d); alu_cp(gb, 0x41);
  if (!(F & FC)) { CYCT(0x585d, 0x585f); goto animate; } // jr nc
  CYC(0x585d, 0x585f);
  CYC(0x585f, 0x5861); alu_and(gb, 0x78);
  CYC(0x5861, 0x5863); A = alu_swap(gb, A);
  CYC(0x5863, 0x5864); alu_rlca(gb);
  CYC(0x5864, 0x5865); B = A;
  CYC(0x5865, 0x5867); alu_sub(gb, 0x06);
  if (F & FC) { CYCT(0x5867, 0x5869); goto setZh; } // jr c
  CYC(0x5867, 0x5869);
  CYC(0x5869, 0x586a); alu_xor(gb, A);

setZh:
  CYC(0x586a, 0x586c); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x586c, 0x586d); mem_wr(gb, DE, A);

  // Determine speed
  CYC(0x586d, 0x586e); A = B;
  CYC(0x586e, 0x5871); SET_HL(0x58a1); // @speedVals
  peahat_addAToHl_from_rst(gb, 0x5872);
  CYC(0x5872, 0x5874); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5874, 0x5875); A = mem_rd(gb, HL);
  CYC(0x5875, 0x5876); mem_wr(gb, DE, A);
  CALL_C(0x5876, objectApplySpeed_hook, 0x201d, 0x5879);
  CALL_C(0x5879, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x587c);

animate:
  CYC(0x587c, 0x587e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x587e, 0x587f); A = mem_rd(gb, DE);
  CYC(0x587f, 0x5881); alu_and(gb, 0xf0);
  CYC(0x5881, 0x5883); A = alu_swap(gb, A);
  CYC(0x5883, 0x5886); SET_HL(0x5899); // @animFrequencies
  peahat_addAToHl_from_rst(gb, 0x5887);
  CYC(0x5887, 0x5888); B = mem_rd(gb, HL);
  CYC(0x5888, 0x5889); A = B;
  CYC(0x5889, 0x588a); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x588a, 0x588c); goto checkFrequency; } // jr nz
  CYC(0x588a, 0x588c);
  CALL_C(0x588c, enemyAnimate_hook, 0x2818, 0x588f);
  CYC(0x588f, 0x5891); B = 0x00;

checkFrequency:
  CYC(0x5891, 0x5894); A = mem_rd(gb, wFrameCounter);
  CYC(0x5894, 0x5895); alu_and(gb, B);
  if (F & FZ) { CYCT(0x5895, 0x5898); enemyAnimate_hook(gb); return; } // jp z
  RET(0x5898); return; // ret
}
