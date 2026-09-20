#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void swoop_state_uninitialized_hook(GB *gb);
void swoop_state_stub_hook(GB *gb);
void swoop_state8_hook(GB *gb);
void swoop_state9_hook(GB *gb);
void swoop_stateA_hook(GB *gb);
void swoop_stateB_hook(GB *gb);
void swoop_setSpeedZForBounce_hook(GB *gb);
void swoop_setVisible_hook(GB *gb);
void swoop_stomp_substate2_hook(GB *gb);
void swoop_beginFlyingUp_hook(GB *gb);
void swoop_flyFurtherUp_hook(GB *gb);
void swoop_stomp_substate3_hook(GB *gb);
void swoop_getAngerLevel_hook(GB *gb);
void swoop_hitGround_hook(GB *gb);
void swoop_animate_hook(GB *gb);

static uint16_t swoop_jump_table(GB *gb) {
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

static void swoop_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_SWOOP
//
// Variables:
//   var30: Number of frames before swoop begins to stomp
//   var31: Target stomp position (short-form)
//   var32/var33: Target stomp position (long-form)
// ==================================================================================================
void enemyCode71_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4742, 0x4744); goto normalStatus; } // jr z
  CYC(0x4742, 0x4744);
  CYC(0x4744, 0x4746); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4746); return; } // ret c
  CYC(0x4746, 0x4747);
  if (!(F & FZ)) { CYCT(0x4747, 0x474a); goto normalStatus; } // jp nz
  CYC(0x4747, 0x474a);
  CYC(0x474a, 0x474d);
  enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(0x474d, 0x474f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x474f, 0x4750); A = mem_rd(gb, DE);
  {
    CYC(0x4750, 0x4751); push_effect(gb, 0x4751);
    uint16_t target = swoop_jump_table(gb);
    if (target == 0x4769) { swoop_state_uninitialized_hook(gb); return; }
    if (target == 0x477a) { swoop_state_stub_hook(gb); return; }
    if (target == 0x477b) { swoop_state8_hook(gb); return; }
    if (target == 0x47fe) { swoop_state9_hook(gb); return; }
    if (target == 0x4837) { swoop_stateA_hook(gb); return; }
    if (target == 0x4874) { swoop_stateB_hook(gb); return; }
    HANDOFF(target);
  }
}

void swoop_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4769, 0x476b); A = 0x71; // ENEMY_SWOOP
  CYC(0x476b, 0x476d); B = 0x00;
  CALL_C(0x476d, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x4770);
  CALL_C(0x4770, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x4773);
  CYC(0x4773, 0x4775); B = 0x01;
  CYC(0x4775, 0x4777); C = 0x08;
  CYC(0x4777, 0x477a);
  enemyBoss_spawnShadow_b0f_hook(gb); return; // jp
}

void swoop_state_stub_hook(GB *gb) {
  RET(0x477a); return; // ret
}

// Spawning in before the fight starts
void swoop_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x477b, 0x477d); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x477d, 0x477e); A = mem_rd(gb, DE);
  {
    CYC(0x477e, 0x477f); push_effect(gb, 0x477f);
    uint16_t target = swoop_jump_table(gb);
    if (target == 0x4787) goto substate0;
    if (target == 0x47ad) goto substate1;
    if (target == 0x47e1) goto substate2;
    if (target == 0x47ed) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x4787, 0x4789); A = 0x01; // DISABLE_LINK
  CYC(0x4789, 0x478c); W8(wDisabledObjects) = A;
  CYC(0x478c, 0x478f); W8(wMenuDisabled) = A;

  // Wait for door to close
  CYC(0x478f, 0x4792); A = W8(wcc93);
  CYC(0x4792, 0x4793); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4793); return; } // ret nz
  CYC(0x4793, 0x4794);
  CALL_C(0x4794, ecom_incSubstate_b0f_hook, 0x4005, 0x4797);
  CYC(0x4797, 0x4799); C = 0x08;
  CALL_C(0x4799, ecom_setZAboveScreen_b0f_hook, 0x4446, 0x479c);
  CYC(0x479c, 0x479e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x479e, 0x47a0); A = 0x3c;
  CYC(0x47a0, 0x47a1); mem_wr(gb, DE, A);
  CYC(0x47a1, 0x47a2); E = alu_inc8(gb, E);
  CYC(0x47a2, 0x47a4); A = 0x02;
  CYC(0x47a4, 0x47a5); mem_wr(gb, DE, A); // [counter2]
  CALL_C(0x47a5, objectSetVisible82_hook, 0x1e69, 0x47a8);
  CYC(0x47a8, 0x47aa); A = 0x02;
  CYC(0x47aa, 0x47ad);
  enemySetAnimation_hook(gb); return; // jp

// Falling to ground
substate1:
  CYC(0x47ad, 0x47af); C = 0x10;
  CALL_C(0x47af, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x47b2);
  if (!(F & FZ)) { RET_TAKEN(0x47b2); return; } // ret nz
  CYC(0x47b2, 0x47b3);
  CYC(0x47b3, 0x47b5); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x47b5, 0x47b6); A = mem_rd(gb, DE);
  CYC(0x47b6, 0x47b7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x47b7, 0x47b9); goto doneBouncing; } // jr z
  CYC(0x47b7, 0x47b9);
  CYC(0x47b9, 0x47ba); A = alu_dec8(gb, A);
  CYC(0x47ba, 0x47bb); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(0x47bb, 0x47bd); goto bounceAgain; } // jr nz
  CYC(0x47bb, 0x47bd);
  CYC(0x47bd, 0x47bf); A = 0x00;
  CALL_C(0x47bf, enemySetAnimation_hook, 0x282b, 0x47c2);
  CYC(0x47c2, 0x47c4);
  goto doneBouncing; // jr

bounceAgain:
  CYC(0x47c4, 0x47c7); SET_BC(0xfe80);
  CALL_C(0x47c7, objectSetSpeedZ_hook, 0x239d, 0x47ca);
  CYC(0x47ca, 0x47cc); A = 0x0a;
  CALL_C(0x47cc, setScreenShakeCounter_hook, 0x24bb, 0x47cf);
  CYC(0x47cf, 0x47d1); A = 0x70; // SND_DOORCLOSE
  CYC(0x47d1, 0x47d4);
  playSound_b00_hook(gb); return; // jp

doneBouncing:
  CALL_C(0x47d4, ecom_decCounter1_b0f_hook, 0x439a, 0x47d7);
  if (!(F & FZ)) { RET_TAKEN(0x47d7); return; } // ret nz
  CYC(0x47d7, 0x47d8);
  CYC(0x47d8, 0x47db); SET_BC(0x2f00); // TX_2f00
  CALL_C(0x47db, showText_hook, 0x1872, 0x47de);
  CYC(0x47de, 0x47e1);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate2:
  CALL_C(0x47e1, retIfTextIsActive_hook, 0x1859, 0x47e4);
  CALL_C(0x47e4, enemyBoss_beginMiniboss_b0f_hook, 0x4580, 0x47e7);
  CALL_C(0x47e7, ecom_incSubstate_b0f_hook, 0x4005, 0x47ea);
  CYC(0x47ea, 0x47ed);
  swoop_beginFlyingUp_hook(gb); return; // jp

substate3:
  CALL_C(0x47ed, swoop_state9_hook, 0x47fe, 0x47f0);
  CYC(0x47f0, 0x47f2); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x47f2, 0x47f3); A = mem_rd(gb, DE);
  CYC(0x47f3, 0x47f5); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { RET_TAKEN(0x47f5); return; } // ret nz
  CYC(0x47f5, 0x47f6);
  CYC(0x47f6, 0x47f7); alu_xor(gb, A);
  CYC(0x47f7, 0x47fa); W8(wDisabledObjects) = A;
  CYC(0x47fa, 0x47fd); W8(wMenuDisabled) = A;
  RET(0x47fd); return; // ret
}

// Flying upward
void swoop_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x47fe, swoop_animate_hook, 0x4999, 0x4801);
  CYC(0x4801, 0x4802); A = mem_rd(gb, DE);
  CYC(0x4802, 0x4803); alu_or(gb, A);
  CYC(0x4803, 0x4806); SET_BC(0xff00);
  if (!(F & FZ)) { CALL_C_CC(0x4806, objectSetSpeedZ_hook, 0x239d, 0x4809); } else { CYC(0x4806, 0x4809); } // call nz
  CYC(0x4809, 0x480b); C = 0x08;
  CALL_C(0x480b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x480e);
  CALL_C(0x480e, ecom_decCounter2_b0f_hook, 0x43a3, 0x4811);
  if (!(F & FZ)) { RET_TAKEN(0x4811); return; } // ret nz
  CYC(0x4811, 0x4812);
  CALL_C(0x4812, ecom_decCounter1_b0f_hook, 0x439a, 0x4815);
  if (!(F & FZ)) { CYCT(0x4815, 0x4818); swoop_flyFurtherUp_hook(gb); return; } // jp nz
  CYC(0x4815, 0x4818);
  CYC(0x4818, 0x481a); mem_wr(gb, HL, 0x3c); // [counter1]
  CYC(0x481a, 0x481c); A = 0x0a;
  CYC(0x481c, 0x481e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x481e, 0x481f); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x481f, 0x4821); mem_wr(gb, HL, 0x00); // [substate]
  CALL_C(0x4821, swoop_getAngerLevel_hook, 0x495e, 0x4824);
  CYC(0x4824, 0x4827); SET_HL(0x49a9); // swoop_framesBeforeAttacking
  CYC(0x4827, 0x4828); swoop_addAToHl_from_rst(gb, 0x4828);
  CYC(0x4828, 0x4829); A = mem_rd(gb, HL);
  CYC(0x4829, 0x482b); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x482b, 0x482c); mem_wr(gb, DE, A);
  CALL_C(0x482c, objectGetAngleTowardLink_hook, 0x1e9c, 0x482f);
  CYC(0x482f, 0x4831); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4831, 0x4832); mem_wr(gb, DE, A);
  CYC(0x4832, 0x4834); A = 0x00;
  CYC(0x4834, 0x4837);
  enemySetAnimation_hook(gb); return; // jp
}

// Flying around, getting closer to Link before stomping
void swoop_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4837, swoop_animate_hook, 0x4999, 0x483a);
  CALL_C(0x483a, swoop_getAngerLevel_hook, 0x495e, 0x483d);
  CYC(0x483d, 0x4840); SET_HL(0x49a6); // swoop_speedVals
  CYC(0x4840, 0x4841); swoop_addAToHl_from_rst(gb, 0x4841);
  CYC(0x4841, 0x4842); A = mem_rd(gb, HL);
  CYC(0x4842, 0x4844); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4844, 0x4845); mem_wr(gb, DE, A);
  CYC(0x4845, 0x4847); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4847, 0x4848); A = mem_rd(gb, DE);
  CYC(0x4848, 0x4849); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4849, 0x484b); goto checkDistance; } // jr z
  CYC(0x4849, 0x484b);
  CYC(0x484b, 0x484c); A = alu_dec8(gb, A);
  CYC(0x484c, 0x484d); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(0x484d, 0x484f); goto updatePosition; } // jr nz
  CYC(0x484d, 0x484f);

checkDistance:
  CYC(0x484f, 0x4851); C = 0x30;
  CALL_C(0x4851, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x4854);
  if (!(F & FC)) { CYCT(0x4854, 0x4856); goto updatePosition; } // jr nc
  CYC(0x4854, 0x4856);
  CALL_C(0x4856, ecom_incState_b0f_hook, 0x4000, 0x4859);
  CYC(0x4859, 0x485a); L = alu_inc8(gb, L);
  CYC(0x485a, 0x485c); mem_wr(gb, HL, 0x00); // [substate]
  CYC(0x485c, 0x485e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x485e, 0x4860); mem_wr(gb, HL, 0x1e);
  RET(0x4860); return; // ret

updatePosition:
  CALL_C(0x4861, ecom_decCounter1_b0f_hook, 0x439a, 0x4864);
  if (!(F & FZ)) { CYCT(0x4864, 0x4866); goto applyVelocity; } // jr nz
  CALL_C(0x4866, objectGetAngleTowardLink_hook, 0x1e9c, 0x4869);
  CYC(0x4869, 0x486b); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x486b, 0x486c); mem_wr(gb, DE, A);
  CYC(0x486c, 0x486e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x486e, 0x4870); A = 0x3c;
  CYC(0x4870, 0x4871); mem_wr(gb, DE, A);

applyVelocity:
  CYC(0x4871, 0x4874);
  ecom_applyVelocityForSideviewEnemy_b0f_hook(gb); return; // jp
}

// Stomping
void swoop_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4874, 0x4876); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x4876, 0x4877); A = mem_rd(gb, DE);
  {
    CYC(0x4877, 0x4878); push_effect(gb, 0x4878);
    uint16_t target = swoop_jump_table(gb);
    if (target == 0x4880) goto substate0;
    if (target == 0x48c8) goto substate1;
    if (target == 0x4917) { swoop_stomp_substate2_hook(gb); return; }
    if (target == 0x4947) { swoop_stomp_substate3_hook(gb); return; }
    HANDOFF(target);
  }

// Flapping wings quickly, telegraphing stomp is about to begin
substate0:
  CALL_C(0x4880, swoop_animate_hook, 0x4999, 0x4883);
  CALL_C(0x4883, swoop_animate_hook, 0x4999, 0x4886);
  CALL_C(0x4886, ecom_decCounter1_b0f_hook, 0x439a, 0x4889);
  if (F & FZ) { CYCT(0x4889, 0x488b); goto beginStomp; } // jr z
  CYC(0x4889, 0x488b);
  CYC(0x488b, 0x488c); A = mem_rd(gb, HL); // [counter1]
  CYC(0x488c, 0x488e); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { RET_TAKEN(0x488e); return; } // ret nz
  CYC(0x488e, 0x488f);

  // Decide on target position to stomp at, store in var31
  CYC(0x488f, 0x4892); SET_HL(w1Link_yh);
  CYC(0x4892, 0x4893); B = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4893, 0x4894); L = alu_inc8(gb, L);
  CYC(0x4894, 0x4895); C = mem_rd(gb, HL); // [w1Link.xh]
  CALL_C(0x4895, getTileAtPosition_hook, 0x1447, 0x4899);
  CYC(0x4899, 0x489a); A = L;
  CYC(0x489a, 0x489c); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x489c, 0x489d); mem_wr(gb, DE, A);

  // Convert to long-form, store in var32/var33
  CALL_C(0x489d, convertShortToLongPosition_hook, 0x20cb, 0x48a0);
  CYC(0x48a0, 0x48a2); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x48a2, 0x48a3); A = B;
  CYC(0x48a3, 0x48a5); alu_and(gb, 0xf0);
  CYC(0x48a5, 0x48a6); mem_wr(gb, DE, A);
  CYC(0x48a6, 0x48a7); E = alu_inc8(gb, E);
  CYC(0x48a7, 0x48a8); A = C;
  CYC(0x48a8, 0x48a9); mem_wr(gb, DE, A);

  // Get angle toward stomp position
  CYC(0x48a9, 0x48ab); E = ENEMY_BASE + OBJ_YH;
  CALL_C(0x48ab, objectGetRelativeAngle_hook, 0x1ea4, 0x48ae);
  CYC(0x48ae, 0x48b0); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x48b0, 0x48b1); mem_wr(gb, DE, A);
  RET(0x48b1); return; // ret

beginStomp:
  CALL_C(0x48b2, ecom_incSubstate_b0f_hook, 0x4005, 0x48b5);
  CYC(0x48b5, 0x48b7); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x48b7, 0x48b9); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x48b9, 0x48bb); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x48bb, 0x48bd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x48bd, 0x48c0); SET_BC(0x0000);
  CALL_C(0x48c0, objectSetSpeedZ_hook, 0x239d, 0x48c3);
  CYC(0x48c3, 0x48c5); A = 0x02;
  CYC(0x48c5, 0x48c8);
  enemySetAnimation_hook(gb); return; // jp

// Moving toward stomp position while falling to ground
substate1:
  // Get target stomp position
  CYC(0x48c8, 0x48c9); H = D;
  CYC(0x48c9, 0x48cb); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x48cb, 0x48cc); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x48cc, 0x48cd); C = mem_rd(gb, HL);
  CYC(0x48cd, 0x48ce); B = A;

  // Compare with current position
  CYC(0x48ce, 0x48d0); E = ENEMY_BASE + OBJ_YH;
  CYC(0x48d0, 0x48d1); L = E;
  CYC(0x48d1, 0x48d2); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x48d2, 0x48d4); alu_and(gb, 0xfe);
  CYC(0x48d4, 0x48d5); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x48d5, 0x48d7); goto mustMove; } // jr nz
  CYC(0x48d5, 0x48d7);
  CYC(0x48d7, 0x48d8); L = alu_inc8(gb, L);
  CYC(0x48d8, 0x48d9); A = mem_rd(gb, HL);
  CYC(0x48d9, 0x48db); alu_and(gb, 0xfe);
  CYC(0x48db, 0x48dc); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x48dc, 0x48de); goto reachedStompTarget; } // jr z
  CYC(0x48dc, 0x48de);

mustMove:
  CALL_C(0x48de, objectGetRelativeAngle_hook, 0x1ea4, 0x48e1);
  CYC(0x48e1, 0x48e3); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x48e3, 0x48e4); mem_wr(gb, DE, A);
  CALL_C(0x48e4, ecom_applyVelocityForSideviewEnemy_b0f_hook, 0x4153, 0x48e7);

reachedStompTarget:
  CYC(0x48e7, 0x48e9); C = 0x10;
  CALL_C(0x48e9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x48ec);
  if (!(F & FZ)) { RET_TAKEN(0x48ec); return; } // ret nz
  CYC(0x48ec, 0x48ed);

  // Hit the ground.
  CALL_C(0x48ed, swoop_hitGround_hook, 0x496f, 0x48f0);
  CALL_C(0x48f0, ecom_incSubstate_b0f_hook, 0x4005, 0x48f3);
  CYC(0x48f3, 0x48f5); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x48f5, 0x48f6); A = mem_rd(gb, DE);
  CYC(0x48f6, 0x48f8); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x48f8, 0x48fa); swoop_setVisible_hook(gb); return; } // jr nc
  CYC(0x48f8, 0x48fa);

  // Health is low; will bounce either 2 or 3 times.
  CYC(0x48fa, 0x48fb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate] = 3

  // [counter1] = number of bounces
  CALL_C(0x48fb, getRandomNumber_hook, 0x043e, 0x48fe);
  CYC(0x48fe, 0x4900); alu_and(gb, 0x01);
  CYC(0x4900, 0x4901); A = alu_inc8(gb, A);
  CYC(0x4901, 0x4903); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4903, 0x4904); mem_wr(gb, HL, A);
  CYC(0x4904, 0x4906); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4906, 0x4908); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x4908, objectGetAngleTowardLink_hook, 0x1e9c, 0x490b);
  CYC(0x490b, 0x490d); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x490d, 0x490e); mem_wr(gb, DE, A);
  swoop_setSpeedZForBounce_hook(gb); return; // fallthrough
}

void swoop_setSpeedZForBounce_hook(GB *gb) {
  CYC(0x490e, 0x4911); SET_BC(0xff00);
  CYC(0x4911, 0x4914);
  objectSetSpeedZ_hook(gb); return; // jp
}

void swoop_setVisible_hook(GB *gb) {
  CYC(0x4914, 0x4917);
  objectSetVisible82_hook(gb); return; // jp
}

// Completed stomp, about to fly back up.
void swoop_stomp_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4917, swoop_animate_hook, 0x4999, 0x491a);

  // Wait until animation signals to fly up again, or Link attacks
  CYC(0x491a, 0x491c); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x491c, 0x491d); A = mem_rd(gb, DE);
  CYC(0x491d, 0x491f); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(0x491f, 0x4921); goto flyBackUp; } // jr nz
  CYC(0x491f, 0x4921);
  CYC(0x4921, 0x4923); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4923, 0x4924); A = mem_rd(gb, DE);
  CYC(0x4924, 0x4925); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x4925); return; } // ret z
  CYC(0x4925, 0x4926);

flyBackUp:
  CYC(0x4926, 0x4929); SET_BC(0x0000);
  CALL_C(0x4929, objectSetSpeedZ_hook, 0x239d, 0x492c);
  CYC(0x492c, 0x492e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x492e, 0x4930); A = 0x09;
  CYC(0x4930, 0x4931); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4931, 0x4933); mem_wr(gb, HL, 0x00); // [substate]
  CYC(0x4933, 0x4935); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4935, 0x4937); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  swoop_beginFlyingUp_hook(gb); return; // fallthrough
}

void swoop_beginFlyingUp_hook(GB *gb) {
  CYC(0x4937, 0x4939); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4939, 0x493b); mem_wr(gb, HL, 0x03); // 3 flaps before he goes to next state
  swoop_flyFurtherUp_hook(gb); return; // fallthrough
}

void swoop_flyFurtherUp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x493b, 0x493d); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x493d, 0x493f); mem_wr(gb, HL, 0x30); // $30 frames per wing flap
  CALL_C(0x493f, objectSetVisible80_hook, 0x1e57, 0x4942);
  CYC(0x4942, 0x4944); A = 0x03;
  CYC(0x4944, 0x4947);
  enemySetAnimation_hook(gb); return; // jp
}

// Bouncing
void swoop_stomp_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4947, ecom_applyVelocityForSideviewEnemy_b0f_hook, 0x4153, 0x494a);
  CYC(0x494a, 0x494c); C = 0x10;
  CALL_C(0x494c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x494f);
  if (!(F & FZ)) { RET_TAKEN(0x494f); return; } // ret nz
  CYC(0x494f, 0x4950);
  CALL_C(0x4950, swoop_hitGround_hook, 0x496f, 0x4953);
  CALL_C(0x4953, ecom_decCounter1_b0f_hook, 0x439a, 0x4956);
  if (!(F & FZ)) { CYCT(0x4956, 0x4958); swoop_setSpeedZForBounce_hook(gb); return; } // jr nz
  CYC(0x4956, 0x4958);
  CYC(0x4958, 0x495a); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x495a, 0x495b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x495b, 0x495e);
  objectSetVisible82_hook(gb); return; // jp
}

// @param[out] a Value from 0-2
void swoop_getAngerLevel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x495e, 0x4960); B = 0x00;
  CYC(0x4960, 0x4962); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x4962, 0x4963); A = mem_rd(gb, DE);
  CYC(0x4963, 0x4965); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x4965, 0x4967); goto haveAnger; } // jr nc
  CYC(0x4965, 0x4967);
  CYC(0x4967, 0x4968); B = alu_inc8(gb, B);
  CYC(0x4968, 0x496a); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x496a, 0x496c); goto haveAnger; } // jr nc
  CYC(0x496a, 0x496c);
  CYC(0x496c, 0x496d); B = alu_inc8(gb, B);

haveAnger:
  CYC(0x496d, 0x496e); A = B;
  RET(0x496e); return; // ret
}

void swoop_hitGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x496f, 0x4971); A = 0x30;
  CALL_C(0x4971, setScreenShakeCounter_hook, 0x24bb, 0x4974);
  CYC(0x4974, 0x4976); A = 0x70; // SND_DOORCLOSE
  CALL_C(0x4976, playSound_b00_hook, 0x0c98, 0x4979);

  // Replace tile at this position if it's of the appropriate type, and not solid.
  CYC(0x4979, 0x497c); SET_BC(0x0500);
  CALL_C(0x497c, objectGetRelativeTile_hook, 0x1435, 0x497f);
  CYC(0x497f, 0x4980); C = L;
  CYC(0x4980, 0x4982); H = 0xce; // >wRoomCollisions
  CYC(0x4982, 0x4983); A = mem_rd(gb, HL);
  CYC(0x4983, 0x4985); alu_cp(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x4985); return; } // ret z
  CYC(0x4985, 0x4986);

  CYC(0x4986, 0x4988); H = 0xcf; // >wRoomLayout
  CYC(0x4988, 0x4989); A = mem_rd(gb, HL);
  CYC(0x4989, 0x498b); alu_cp(gb, 0xa2);
  if (F & FZ) { RET_TAKEN(0x498b); return; } // ret z
  CYC(0x498b, 0x498c);
  CYC(0x498c, 0x498e); alu_cp(gb, 0x48);
  if (F & FZ) { RET_TAKEN(0x498e); return; } // ret z
  CYC(0x498e, 0x498f);

  CYC(0x498f, 0x4991); A = 0x48;
  CALL_C(0x4991, setTile_hook, 0x3a9c, 0x4994);
  CYC(0x4994, 0x4996); B = 0x06; // INTERAC_ROCKDEBRIS
  CYC(0x4996, 0x4999);
  objectCreateInteractionWithSubid00_hook(gb); return; // jp
}

// @param[out] de animParameter (if nonzero, just flapped wings)
void swoop_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4999, enemyAnimate_hook, 0x2818, 0x499c);
  CYC(0x499c, 0x499e); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x499e, 0x499f); A = mem_rd(gb, DE);
  CYC(0x499f, 0x49a0); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x49a0); return; } // ret z
  CYC(0x49a0, 0x49a1);
  CYC(0x49a1, 0x49a3); A = 0x53; // SND_JUMP
  CYC(0x49a3, 0x49a6);
  playSound_b00_hook(gb); return; // jp
}
