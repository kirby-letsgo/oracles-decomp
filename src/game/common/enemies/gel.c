#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void gel_state_uninitialized_hook(GB *gb);
void gel_state_stub_hook(GB *gb);
void gel_state8_hook(GB *gb);
void gel_state9_hook(GB *gb);
void gel_animate_hook(GB *gb);
void gel_stateA_hook(GB *gb);
void gel_stateB_hook(GB *gb);
void gel_stateC_hook(GB *gb);
void gel_stateD_hook(GB *gb);
void gel_beginHop_hook(GB *gb);
void gel_setAngleAwayFromLink_hook(GB *gb);

static uint16_t gel_jump_table(GB *gb) {
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
// ENEMY_GEL
// ==================================================================================================
void enemyCode43_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d44, ecom_checkHazardsNoAnimationForHoles_b0e_hook, 0x4043, 0x5d47);
  if (F & FZ) { CYCT(0x5d47, 0x5d49); goto normalStatus; } // jr z
  CYC(0x5d47, 0x5d49);
  CYC(0x5d49, 0x5d4b); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5d4b); return; } // ret c
  CYC(0x5d4b, 0x5d4c);
  if (F & FZ) { CYCT(0x5d4c, 0x5d4f); enemyDie_hook(gb); return; } // jp z
  CYC(0x5d4c, 0x5d4f);

  // ENEMYSTATUS_JUST_HIT or ENEMYSTATUS_KNOCKBACK
  CYC(0x5d4f, 0x5d51); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x5d51, 0x5d52); A = mem_rd(gb, DE);
  CYC(0x5d52, 0x5d54); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(0x5d54, 0x5d56); goto normalStatus; } // jr nz
  CYC(0x5d54, 0x5d56);

  // Touched Link; attach self to him.
  CYC(0x5d56, 0x5d58); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5d58, 0x5d5a); A = 0x0c;
  CYC(0x5d5a, 0x5d5b); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x5d5b, 0x5d5d); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5d5d, 0x5d5e); A = mem_rd(gb, DE);
  {
    CYC(0x5d5e, 0x5d5f); push_effect(gb, 0x5d5f);
    uint16_t target = gel_jump_table(gb);
    if (target == 0x5d7b) { gel_state_uninitialized_hook(gb); return; }
    if (target == 0x5d83) { gel_state_stub_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x5d84) { gel_state8_hook(gb); return; }
    if (target == 0x5dae) { gel_state9_hook(gb); return; }
    if (target == 0x5dc1) { gel_stateA_hook(gb); return; }
    if (target == 0x5dcc) { gel_stateB_hook(gb); return; }
    if (target == 0x5de5) { gel_stateC_hook(gb); return; }
    if (target == 0x5df1) { gel_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5d7b, bare global; jump-table target from enemyCode43.
void gel_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d7b, 0x5d7d); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5d7d, 0x5d7f); A = 0x10;
  CYC(0x5d7f, 0x5d80); mem_wr(gb, DE, A);
  CYC(0x5d80, 0x5d83); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

// 0e:5d83, bare global; jump-table target from enemyCode43.
void gel_state_stub_hook(GB *gb) {
  RET(0x5d83); return; // ret
}

// 0e:5d84, bare global; jump-table target from enemyCode43. Standing in place for
// [counter1] frames.
void gel_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d84, ecom_decCounter1_b0e_hook, 0x439a, 0x5d87);
  if (!(F & FZ)) { CYCT(0x5d87, 0x5d89); gel_animate_hook(gb); return; } // jr nz
  CYC(0x5d87, 0x5d89);

  // 1 in 8 chance of switching to "hopping" state
  CALL_C(0x5d89, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5d8c);
  CYC(0x5d8c, 0x5d8e); alu_and(gb, 0x07);
  CYC(0x5d8e, 0x5d8f); H = D;
  if (!(F & FZ)) { CYCT(0x5d8f, 0x5d91); goto inchForward; } // jr nz
  CYC(0x5d8f, 0x5d91);

  // Prepare to hop
  CYC(0x5d91, 0x5d93); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5d93, 0x5d95); mem_wr(gb, HL, 0x30);

  CYC(0x5d95, 0x5d97); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5d97, 0x5d99); mem_wr(gb, HL, 0x0a);

  CYC(0x5d99, 0x5d9b); A = 0x02;
  CYC(0x5d9b, 0x5d9e); enemySetAnimation_hook(gb); return; // jp

inchForward:
  CYC(0x5d9e, 0x5da0); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5da0, 0x5da2); mem_wr(gb, HL, 0x08);

  CYC(0x5da2, 0x5da4); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5da4, 0x5da5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(0x5da5, 0x5da7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5da7, 0x5da9); mem_wr(gb, HL, 0x0a); // SPEED_40

  CALL_C(0x5da9, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x5dac);
  CYC(0x5dac, 0x5dae); gel_animate_hook(gb); return; // jr
}

// 0e:5dae, bare global; jump-table target from enemyCode43. Inching toward Link for
// [counter1] frames.
void gel_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5dae, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x5db1);
  CALL_C(0x5db1, ecom_decCounter1_b0e_hook, 0x439a, 0x5db4);
  if (!(F & FZ)) { CYCT(0x5db4, 0x5db6); gel_animate_hook(gb); return; } // jr nz
  CYC(0x5db4, 0x5db6);

  CYC(0x5db6, 0x5db8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5db8, 0x5dba); mem_wr(gb, HL, 0x08);
  CYC(0x5dba, 0x5dbc); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5dbc, 0x5dbe); mem_wr(gb, HL, 0x10);
  gel_animate_hook(gb); return; // fallthrough
}

// 0e:5dbe, bare global; falls into from gel_state9, also reached by genuine jr/jp from
// gel_state8, gel_stateA and gel_stateD.
void gel_animate_hook(GB *gb) {
  CYC(0x5dbe, 0x5dc1); enemyAnimate_hook(gb); return; // jp
}

// 0e:5dc1, bare global; jump-table target from enemyCode43. Preparing to hop toward Link.
void gel_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5dc1, ecom_decCounter1_b0e_hook, 0x439a, 0x5dc4);
  if (!(F & FZ)) { CYCT(0x5dc4, 0x5dc6); gel_animate_hook(gb); return; } // jr nz
  CYC(0x5dc4, 0x5dc6);
  CALL_C(0x5dc6, gel_beginHop_hook, 0x5e32, 0x5dc9);
  CYC(0x5dc9, 0x5dcc); ecom_updateAngleTowardTarget_b0e_hook(gb); return; // jp
}

// 0e:5dcc, bare global; jump-table target from enemyCode43. Hopping toward Link.
void gel_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5dcc, ecom_applyVelocityForSideviewEnemy_b0e_hook, 0x4153, 0x5dcf);
  CYC(0x5dcf, 0x5dd1); C = 0x28;
  CALL_C(0x5dd1, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5dd4);
  if (!(F & FZ)) { RET_TAKEN(0x5dd4); return; } // ret nz
  CYC(0x5dd4, 0x5dd5);

  // Just landed
  CYC(0x5dd5, 0x5dd6); H = D;
  CYC(0x5dd6, 0x5dd8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5dd8, 0x5dda); mem_wr(gb, HL, 0x08);

  CYC(0x5dda, 0x5ddc); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5ddc, 0x5dde); mem_wr(gb, HL, 0x10);

  CYC(0x5dde, 0x5de0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5de0, 0x5de2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5de2, 0x5de5); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:5de5, bare global; jump-table target from enemyCode43. Just latched onto Link.
void gel_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5de5, 0x5de6); H = D;
  CYC(0x5de6, 0x5de7); L = E;
  CYC(0x5de7, 0x5de8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(0x5de8, 0x5dea); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5dea, 0x5dec); mem_wr(gb, HL, 120);

  CYC(0x5dec, 0x5dee); A = 0x01;
  CYC(0x5dee, 0x5df1); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5df1, bare global; jump-table target from enemyCode43. Attached to Link, slowing him
// down.
void gel_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5df1, 0x5df4); A = mem_rd(gb, w1Link_yh);
  CYC(0x5df4, 0x5df6); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5df6, 0x5df7); mem_wr(gb, DE, A);
  CYC(0x5df7, 0x5dfa); A = mem_rd(gb, w1Link_xh);
  CYC(0x5dfa, 0x5dfc); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5dfc, 0x5dfd); mem_wr(gb, DE, A);

  CALL_C(0x5dfd, ecom_decCounter2_b0e_hook, 0x43a3, 0x5e00);
  if (F & FZ) { CYCT(0x5e00, 0x5e02); goto hopOff; } // jr z
  CYC(0x5e00, 0x5e02);

  // If any button is pressed, counter2 goes down more quickly
  CYC(0x5e02, 0x5e05); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(0x5e05, 0x5e06); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5e06, 0x5e08); goto checkAnimPriority; } // jr z
  CYC(0x5e06, 0x5e08);

  CYC(0x5e08, 0x5e09); A = mem_rd(gb, HL); // [counter2]
  CYC(0x5e09, 0x5e0b); alu_sub(gb, 0x03);
  if (!(F & FC)) { CYCT(0x5e0b, 0x5e0d); goto storeCounter2; } // jr nc
  CYC(0x5e0b, 0x5e0d);
  CYC(0x5e0d, 0x5e0f); A = 0x01;

storeCounter2:
  CYC(0x5e0f, 0x5e10); mem_wr(gb, HL, A);

checkAnimPriority:
  // Invert draw priority every 4 frames
  CYC(0x5e10, 0x5e11); A = mem_rd(gb, HL);
  CYC(0x5e11, 0x5e13); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x5e13, 0x5e15); goto disableSword; } // jr nz
  CYC(0x5e13, 0x5e15);
  CYC(0x5e15, 0x5e17); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x5e17, 0x5e18); A = mem_rd(gb, HL);
  CYC(0x5e18, 0x5e1a); alu_xor(gb, 0x07);
  CYC(0x5e1a, 0x5e1b); mem_wr(gb, HL, A);

disableSword:
  // Disable use of sword
  CYC(0x5e1b, 0x5e1e); SET_HL(wccd8);
  CYC(0x5e1e, 0x5e20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)

  // Disable movement every other frame
  CYC(0x5e20, 0x5e23); A = mem_rd(gb, wFrameCounter);
  CYC(0x5e23, 0x5e24); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x5e24, 0x5e26); gel_animate_hook(gb); return; } // jr nc
  CYC(0x5e24, 0x5e26);
  CYC(0x5e26, 0x5e29); SET_HL(wLinkImmobilized);
  CYC(0x5e29, 0x5e2b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(0x5e2b, 0x5e2d); gel_animate_hook(gb); return; // jr

hopOff:
  CALL_C(0x5e2d, gel_setAngleAwayFromLink_hook, 0x5e4c, 0x5e30);
  CYC(0x5e30, 0x5e32); gel_beginHop_hook(gb); return; // jr
}

// 0e:5e32, bare global; called from gel_stateA and gel_stateD.
void gel_beginHop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e32, 0x5e35); SET_BC(0xfe00); // -$200
  CALL_C(0x5e35, objectSetSpeedZ_hook, 0x239d, 0x5e38);

  CYC(0x5e38, 0x5e3a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5e3a, 0x5e3c); mem_wr(gb, HL, 0x0b);

  CYC(0x5e3c, 0x5e3e); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5e3e, 0x5e40); mem_wr(gb, HL, 0x28); // SPEED_100

  CYC(0x5e40, 0x5e41); alu_xor(gb, A);
  CALL_C(0x5e41, enemySetAnimation_hook, 0x282b, 0x5e44);

  CYC(0x5e44, 0x5e46); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x5e46, playSound_b00_hook, 0x0c98, 0x5e49);
  CYC(0x5e49, 0x5e4c); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:5e4c, bare global; called from gel_stateD.
void gel_setAngleAwayFromLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e4c, 0x5e4f); A = mem_rd(gb, w1Link_angle);
  CYC(0x5e4f, 0x5e51); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x5e51, 0x5e54); ecom_setRandomAngle_b0e_hook(gb); return; } // jp nz
  CYC(0x5e51, 0x5e54);
  CYC(0x5e54, 0x5e56); alu_xor(gb, 0x10);
  CYC(0x5e56, 0x5e58); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5e58, 0x5e59); mem_wr(gb, DE, A);
  RET(0x5e59); return; // ret
}
