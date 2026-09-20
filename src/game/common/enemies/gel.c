#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode43), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode43), (from), (to), true)

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
  BASE(enemyCode43);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazardsNoAnimationForHoles_b0e_hook, SYM(ecom_checkHazardsNoAnimationForHoles_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);

  // ENEMYSTATUS_JUST_HIT or ENEMYSTATUS_KNOCKBACK
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto normalStatus; } // jr nz
  CYC(b_+16, b_+18);

  // Touched Link; attach self to him.
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+20, b_+22); A = 0x0c;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  {
    CYC(b_+26, b_+27); push_effect(gb, b_+27);
    uint16_t target = gel_jump_table(gb);
    if (target == SYM(gel_state_uninitialized)) { gel_state_uninitialized_hook(gb); return; }
    if (target == SYM(gel_state_stub)) { gel_state_stub_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(gel_state8)) { gel_state8_hook(gb); return; }
    if (target == SYM(gel_state9)) { gel_state9_hook(gb); return; }
    if (target == SYM(gel_stateA)) { gel_stateA_hook(gb); return; }
    if (target == SYM(gel_stateB)) { gel_stateB_hook(gb); return; }
    if (target == SYM(gel_stateC)) { gel_stateC_hook(gb); return; }
    if (target == SYM(gel_stateD)) { gel_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5d7b, bare global; jump-table target from enemyCode43.
void gel_state_uninitialized_hook(GB *gb) {
  BASE(gel_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+4); A = 0x10;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, SYM(gel_state_stub)); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

// 0e:5d83, bare global; jump-table target from enemyCode43.
void gel_state_stub_hook(GB *gb) {
  BASE(gel_state_stub);
  RET(b_+0); return; // ret
}

// 0e:5d84, bare global; jump-table target from enemyCode43. Standing in place for
// [counter1] frames.
void gel_state8_hook(GB *gb) {
  BASE(gel_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); gel_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);

  // 1 in 8 chance of switching to "hopping" state
  CALL_C(b_+5, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x07);
  CYC(b_+10, b_+11); H = D;
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto inchForward; } // jr nz
  CYC(b_+11, b_+13);

  // Prepare to hop
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x30);

  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0a);

  CYC(b_+21, b_+23); A = 0x02;
  CYC(b_+23, b_+26); enemySetAnimation_hook(gb); return; // jp

inchForward:
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x08);

  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x0a); // SPEED_40

  CALL_C(b_+37, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+40);
  CYC(b_+40, SYM(gel_state9)); gel_animate_hook(gb); return; // jr
}

// 0e:5dae, bare global; jump-table target from enemyCode43. Inching toward Link for
// [counter1] frames.
void gel_state9_hook(GB *gb) {
  BASE(gel_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); gel_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);

  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x08);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, SYM(gel_animate)); mem_wr(gb, HL, 0x10);
  gel_animate_hook(gb); return; // fallthrough
}

// 0e:5dbe, bare global; falls into from gel_state9, also reached by genuine jr/jp from
// gel_state8, gel_stateA and gel_stateD.
void gel_animate_hook(GB *gb) {
  BASE(gel_animate);
  CYC(b_+0, SYM(gel_stateA)); enemyAnimate_hook(gb); return; // jp
}

// 0e:5dc1, bare global; jump-table target from enemyCode43. Preparing to hop toward Link.
void gel_stateA_hook(GB *gb) {
  BASE(gel_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); gel_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, gel_beginHop_hook, SYM(gel_beginHop), b_+8);
  CYC(b_+8, SYM(gel_stateB)); ecom_updateAngleTowardTarget_b0e_hook(gb); return; // jp
}

// 0e:5dcc, bare global; jump-table target from enemyCode43. Hopping toward Link.
void gel_stateB_hook(GB *gb) {
  BASE(gel_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_applyVelocityForSideviewEnemy_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0e), b_+3);
  CYC(b_+3, b_+5); C = 0x28;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);

  // Just landed
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x08);

  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x10);

  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+20, b_+22); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+22, SYM(gel_stateC)); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:5de5, bare global; jump-table target from enemyCode43. Just latched onto Link.
void gel_stateC_hook(GB *gb) {
  BASE(gel_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 120);

  CYC(b_+7, b_+9); A = 0x01;
  CYC(b_+9, SYM(gel_stateD)); enemySetAnimation_hook(gb); return; // jp
}

// 0e:5df1, bare global; jump-table target from enemyCode43. Attached to Link, slowing him
// down.
void gel_stateD_hook(GB *gb) {
  BASE(gel_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+9); A = mem_rd(gb, w1Link_xh);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);

  CALL_C(b_+12, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+17); goto hopOff; } // jr z
  CYC(b_+15, b_+17);

  // If any button is pressed, counter2 goes down more quickly
  CYC(b_+17, b_+20); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+21, b_+23); goto checkAnimPriority; } // jr z
  CYC(b_+21, b_+23);

  CYC(b_+23, b_+24); A = mem_rd(gb, HL); // [counter2]
  CYC(b_+24, b_+26); alu_sub(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto storeCounter2; } // jr nc
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); A = 0x01;

storeCounter2:
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);

checkAnimPriority:
  // Invert draw priority every 4 frames
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto disableSword; } // jr nz
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_xor(gb, 0x07);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);

disableSword:
  // Disable use of sword
  CYC(b_+42, b_+45); SET_HL(wccd8);
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)

  // Disable movement every other frame
  CYC(b_+47, b_+50); A = mem_rd(gb, wFrameCounter);
  CYC(b_+50, b_+51); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+51, b_+53); gel_animate_hook(gb); return; } // jr nc
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+56); SET_HL(wLinkImmobilized);
  CYC(b_+56, b_+58); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(b_+58, b_+60); gel_animate_hook(gb); return; // jr

hopOff:
  CALL_C(b_+60, gel_setAngleAwayFromLink_hook, SYM(gel_setAngleAwayFromLink), b_+63);
  CYC(b_+63, SYM(gel_beginHop)); gel_beginHop_hook(gb); return; // jr
}

// 0e:5e32, bare global; called from gel_stateA and gel_stateD.
void gel_beginHop_hook(GB *gb) {
  BASE(gel_beginHop);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xfe00); // -$200
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);

  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x0b);

  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x28); // SPEED_100

  CYC(b_+14, b_+15); alu_xor(gb, A);
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), b_+18);

  CYC(b_+18, b_+20); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+20, playSound_b00_hook, SYM(playSound_b00), b_+23);
  CYC(b_+23, SYM(gel_setAngleAwayFromLink)); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:5e4c, bare global; called from gel_stateD.
void gel_setAngleAwayFromLink_hook(GB *gb) {
  BASE(gel_setAngleAwayFromLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_angle);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); ecom_setRandomAngle_b0e_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); alu_xor(gb, 0x10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return; // ret
}
