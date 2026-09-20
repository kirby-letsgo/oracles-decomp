#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void stalfos_state_uninitialized_hook(GB *gb);
void stalfos_state_switchHook_hook(GB *gb);
void stalfos_state_stub_hook(GB *gb);
void stalfos_state08_hook(GB *gb);
void stalfos_state09_hook(GB *gb);
void stalfos_state0a_hook(GB *gb);
void stalfos_state0b_hook(GB *gb);
void stalfos_state0c_hook(GB *gb);
void stalfos_state0d_hook(GB *gb);
void stalfos_state0e_hook(GB *gb);
void stalfos_state0f_hook(GB *gb);
void stalfos_state10_hook(GB *gb);
void stalfos_moveInRandomAngle_hook(GB *gb);
void stalfos_checkSubid3StompsLink_hook(GB *gb);
void stalfos_beginJumpAnimation_hook(GB *gb);
void stalfos_checkJumpAwayFromLink_hook(GB *gb);

static uint16_t stalfos_jump_table(GB *gb) {
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
// ENEMY_STALFOS
// ==================================================================================================
void enemyCode31_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x45c5, ecom_checkHazards_b0e_hook, 0x4051, 0x45c8);
  if (F & FZ) { CYCT(0x45c8, 0x45ca); goto normalStatus; } // jr z
  CYC(0x45c8, 0x45ca);
  CYC(0x45ca, 0x45cc); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x45cc); return; } // ret c
  CYC(0x45cc, 0x45cd);
  if (F & FZ) { CYCT(0x45cd, 0x45d0); enemyDie_hook(gb); return; } // jp z
  CYC(0x45cd, 0x45d0);
  CYC(0x45d0, 0x45d1); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x45d1, 0x45d4); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x45d1, 0x45d4);
  RET(0x45d4); return; // ret

normalStatus:
  CALL_C(0x45d5, stalfos_checkJumpAwayFromLink_hook, 0x46f9, 0x45d8);
  CYC(0x45d8, 0x45da); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x45da, 0x45db); A = mem_rd(gb, DE);
  {
    CYC(0x45db, 0x45dc); push_effect(gb, 0x45dc);
    uint16_t target = stalfos_jump_table(gb);
    if (target == 0x45fe) { stalfos_state_uninitialized_hook(gb); return; }
    if (target == 0x460f) { stalfos_state_stub_hook(gb); return; }
    if (target == 0x4603) { stalfos_state_switchHook_hook(gb); return; }
    if (target == 0x4610) { stalfos_state08_hook(gb); return; }
    if (target == 0x4629) { stalfos_state09_hook(gb); return; }
    if (target == 0x463e) { stalfos_state0a_hook(gb); return; }
    if (target == 0x4654) { stalfos_state0b_hook(gb); return; }
    if (target == 0x4672) { stalfos_state0c_hook(gb); return; }
    if (target == 0x4679) { stalfos_state0d_hook(gb); return; }
    if (target == 0x468b) { stalfos_state0e_hook(gb); return; }
    if (target == 0x4692) { stalfos_state0f_hook(gb); return; }
    if (target == 0x46a7) { stalfos_state10_hook(gb); return; }
    HANDOFF(target);
  }
}

void stalfos_state_uninitialized_hook(GB *gb) {
  CYC(0x45fe, 0x4600); A = 0x14; // SPEED_80
  CYC(0x4600, 0x4603); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

void stalfos_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4603, 0x4604); E = alu_inc8(gb, E);
  CYC(0x4604, 0x4605); A = mem_rd(gb, DE);
  {
    CYC(0x4605, 0x4606); push_effect(gb, 0x4606);
    uint16_t target = stalfos_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x460e) goto substate1;
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0e_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(0x460e); return; // ret
}

void stalfos_state_stub_hook(GB *gb) {
  RET(0x460f); return; // ret
}

// Choosing what to do next (move in a random direction, or shoot a bone at Link)
void stalfos_state08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4610, stalfos_checkSubid3StompsLink_hook, 0x46ce, 0x4613);
  CALL_C(0x4613, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4616);
  CYC(0x4616, 0x4618); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x4618, 0x461b); stalfos_moveInRandomAngle_hook(gb); return; } // jp nz
  CYC(0x4618, 0x461b);
  CYC(0x461b, 0x461d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x461d, 0x461e); A = mem_rd(gb, DE);
  CYC(0x461e, 0x4620); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4620, 0x4623); stalfos_moveInRandomAngle_hook(gb); return; } // jp nz
  CYC(0x4620, 0x4623);
  CYC(0x4623, 0x4625); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4625, 0x4627); A = 0x0c;
  CYC(0x4627, 0x4628); mem_wr(gb, DE, A);
  RET(0x4628); return; // ret
}

// Moving in some direction for [counter1] frames
void stalfos_state09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4629, stalfos_checkSubid3StompsLink_hook, 0x46ce, 0x462c);
  CALL_C(0x462c, ecom_decCounter1_b0e_hook, 0x439a, 0x462f);
  if (!(F & FZ)) { CYCT(0x462f, 0x4631); goto L_4635; } // jr nz
  CYC(0x462f, 0x4631);
  CYC(0x4631, 0x4633); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4633, 0x4635); mem_wr(gb, HL, 0x08);

L_4635:
  CALL_C(0x4635, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x4638);
  CALL_C(0x4638, objectApplySpeed_hook, 0x201d, 0x463b);
  CYC(0x463b, 0x463e); enemyAnimate_hook(gb); return; // jp
}

// Just starting a jump away from Link
void stalfos_state0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x463e, 0x4641); SET_BC(0xfe00);
  CALL_C(0x4641, objectSetSpeedZ_hook, 0x239d, 0x4644);
  CYC(0x4644, 0x4645); L = E;
  CYC(0x4645, 0x4646); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x4646, 0x4648); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4648, 0x464a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x464a, 0x464c); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x464c, 0x464e); mem_wr(gb, HL, 0x32); // SPEED_140
  CALL_C(0x464e, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, 0x43ab, 0x4651);
  CYC(0x4651, 0x4654); stalfos_beginJumpAnimation_hook(gb); return; // jp
}

// Jumping until hitting the ground
void stalfos_state0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4654, 0x4656); C = 0x20;
  CALL_C(0x4656, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4659);
  if (F & FZ) { CYCT(0x4659, 0x465b); goto hitGround; } // jr z
  CYC(0x4659, 0x465b);
  CYC(0x465b, 0x465c); A = mem_rd(gb, HL);
  CYC(0x465c, 0x465d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x465d, 0x465f); goto L_4663; } // jr nz
  CYC(0x465d, 0x465f);
  CYC(0x465f, 0x4661); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4661, 0x4663); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

L_4663:
  CYC(0x4663, 0x4666); ecom_applyVelocityForSideviewEnemy_b0e_hook(gb); return; // jp

hitGround:
  CYC(0x4666, 0x4668); A = 0x14; // SPEED_80
  CALL_C(0x4668, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x466b);
  CYC(0x466b, 0x466c); alu_xor(gb, A);
  CALL_C(0x466c, enemySetAnimation_hook, 0x282b, 0x466f);
  CYC(0x466f, 0x4672); objectSetVisiblec2_hook(gb); return; // jp
}

// Firing a projectile, then immediately going to state 9 to keep moving
void stalfos_state0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4672, 0x4674); B = 0x1c; // PART_STALFOS_BONE
  CALL_C(0x4674, ecom_spawnProjectile_b0e_hook, 0x437c, 0x4677);
  CYCT(0x4677, 0x4679); stalfos_moveInRandomAngle_hook(gb); return; // jr
}

// Stomping on Link
void stalfos_state0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4679, 0x467b); C = 0x20;
  CALL_C(0x467b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x467e);
  CYC(0x467e, 0x467f); A = mem_rd(gb, HL);
  CYC(0x467f, 0x4680); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4680, 0x4683); ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(gb); return; } // jp nz
  CYC(0x4680, 0x4683);
  CYC(0x4683, 0x4685); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4685, 0x4687); mem_wr(gb, HL, 0x08);
  CYC(0x4687, 0x4689); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4689, 0x468a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x468a); return; // ret
}

// Wait for 8 frames while hanging in the air mid-stomp
void stalfos_state0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x468b, ecom_decCounter1_b0e_hook, 0x439a, 0x468e);
  if (!(F & FZ)) { RET_TAKEN(0x468e); return; } // ret nz
  CYC(0x468e, 0x468f);
  CYC(0x468f, 0x4690); L = E;
  CYC(0x4690, 0x4691); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  RET(0x4691); return; // ret
}

// Fall down for the stomp
void stalfos_state0f_hook(GB *gb) {
  CYC(0x4692, 0x4693); H = D;
  CYC(0x4693, 0x4695); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x4695, 0x4696); A = mem_rd(gb, HL);
  CYC(0x4696, 0x4698); alu_add(gb, 0x03);
  CYC(0x4698, 0x4699); mem_wr(gb, HL, A);
  CYC(0x4699, 0x469b); alu_cp(gb, 0x80);
  if (!(F & FC)) { RET_TAKEN(0x469b); return; } // ret nc
  CYC(0x469b, 0x469c);
  CYC(0x469c, 0x469d); alu_xor(gb, A);
  CYC(0x469d, 0x469e); mem_wr(gb, HL, A); // [zh] = 0
  CYC(0x469e, 0x469f); L = E;
  CYC(0x469f, 0x46a0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x46a0, 0x46a2); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x46a2, 0x46a4); mem_wr(gb, HL, 30);
  CYC(0x46a4, 0x46a7); objectSetVisiblec2_hook(gb); return; // jp
}

// Laying on the ground for [counter1] frames until he starts moving again
void stalfos_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x46a7, ecom_decCounter1_b0e_hook, 0x439a, 0x46aa);
  if (!(F & FZ)) { RET_TAKEN(0x46aa); return; } // ret nz
  CYC(0x46aa, 0x46ab);
  stalfos_moveInRandomAngle_hook(gb); return; // fallthrough
}

// Go to state 9 with a freshly chosen angle
void stalfos_moveInRandomAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46ab, 0x46ad); E = 0x30; // Enemy.var30
  CYC(0x46ad, 0x46b0); SET_BC(0x1f0f);
  CALL_C(0x46b0, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x46b3);
  CYC(0x46b3, 0x46b4); H = D;
  CYC(0x46b4, 0x46b6); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x46b6, 0x46b8); mem_wr(gb, HL, 0x09);
  CYC(0x46b8, 0x46ba); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x46ba, 0x46bc); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x46bc, 0x46be); A = 0x20;
  CYC(0x46be, 0x46bf); alu_add(gb, E);
  CYC(0x46bf, 0x46c1); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x46c1, 0x46c2); mem_wr(gb, HL, A);
  CYC(0x46c2, 0x46c3); C = alu_dec8(gb, C);
  CYC(0x46c3, 0x46c4); A = B;
  if (F & FZ) CALL_C_CC(0x46c4, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x46c7); else CYC(0x46c4, 0x46c7); // call z
  CYC(0x46c7, 0x46c9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x46c9, 0x46ca); mem_wr(gb, DE, A);
  CYC(0x46ca, 0x46cb); alu_xor(gb, A);
  CYC(0x46cb, 0x46ce); enemySetAnimation_hook(gb); return; // jp
}

// For subid 3 only, if Link approaches close enough, it will jump toward Link to stomp on
// him (goes to state $0d).
void stalfos_checkSubid3StompsLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46ce, 0x46d0); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x46d0, 0x46d1); A = mem_rd(gb, DE);
  CYC(0x46d1, 0x46d3); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x46d3); return; } // ret nz
  CYC(0x46d3, 0x46d4);
  CYC(0x46d4, 0x46d6); C = 0x1c;
  CALL_C(0x46d6, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x46d9);
  if (!(F & FC)) { RET_TAKEN(0x46d9); return; } // ret nc
  CYC(0x46d9, 0x46da);
  CYC(0x46da, 0x46dd); SET_BC(0xfdc0);
  CALL_C(0x46dd, objectSetSpeedZ_hook, 0x239d, 0x46e0);
  CYC(0x46e0, 0x46e2); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x46e2, 0x46e4); mem_wr(gb, HL, 0x0d);
  CYC(0x46e4, 0x46e6); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x46e6, 0x46e8); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x46e8, 0x46e9); pop_effect(gb); // pop hl (discard the caller's return address; tail-falls into stalfos_beginJumpAnimation)
  CALL_C(0x46e9, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x46ec);
  stalfos_beginJumpAnimation_hook(gb); return; // fallthrough
}

void stalfos_beginJumpAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46ec, 0x46ee); A = 0x01;
  CALL_C(0x46ee, enemySetAnimation_hook, 0x282b, 0x46f1);
  CYC(0x46f1, 0x46f3); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x46f3, playSound_b00_hook, 0x0c98, 0x46f6);
  CYC(0x46f6, 0x46f9); objectSetVisiblec1_hook(gb); return; // jp
}

// If Link is swinging something near this object, it will set its state to $0a if not
// already jumping.
void stalfos_checkJumpAwayFromLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46f9, 0x46fb); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x46fb, 0x46fc); A = mem_rd(gb, DE);
  CYC(0x46fc, 0x46fd); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x46fd); return; } // ret z
  CYC(0x46fd, 0x46fe);
  CYC(0x46fe, 0x4701); A = mem_rd(gb, wLinkUsingItem1);
  CYC(0x4701, 0x4703); alu_and(gb, 0xf0);
  if (F & FZ) { RET_TAKEN(0x4703); return; } // ret z
  CYC(0x4703, 0x4704);
  CYC(0x4704, 0x4706); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4706, 0x4707); A = mem_rd(gb, DE);
  CYC(0x4707, 0x4709); alu_cp(gb, 0x0a);
  if (!(F & FC)) { RET_TAKEN(0x4709); return; } // ret nc
  CYC(0x4709, 0x470a);
  CYC(0x470a, 0x470c); C = 0x2c;
  CALL_C(0x470c, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x470f);
  if (!(F & FC)) { RET_TAKEN(0x470f); return; } // ret nc
  CYC(0x470f, 0x4710);
  CYC(0x4710, 0x4712); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4712, 0x4714); A = 0x0a;
  CYC(0x4714, 0x4715); mem_wr(gb, DE, A);
  RET(0x4715); return; // ret
}

// Unused
void stalfos_setState8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4716, 0x4718); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4718, 0x471a); A = 0x08;
  CYC(0x471a, 0x471b); mem_wr(gb, DE, A);
  RET(0x471b); return; // ret
}
