#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void giantGhini_state_uninitialized_hook(GB *gb);
void giantGhini_state_stub_hook(GB *gb);
void giantGhini_state8_hook(GB *gb);
void giantGhini_gotoState9_hook(GB *gb);
void giantGhini_setChildRespawnTimer_hook(GB *gb);
void giantGhini_state9_hook(GB *gb);
void giantGhini_updateChargeTargetPosition_hook(GB *gb);
void giantGhini_stateA_hook(GB *gb);
void giantGhini_updateZPos_hook(GB *gb);
void giantGhini_spawnChildren_hook(GB *gb);
void giantGhini_getTargetAngle_hook(GB *gb);

static uint16_t giantGhini_jump_table(GB *gb) {
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
// ENEMY_GIANT_GHINI
//
// Variables:
//   var30: Number of children alive
//   var32: Nonzero to begin charging at Link (written to by ENEMY_GIANT_GHINI_CHILD)
//   var33: Counter for Z-axis movement (reverses direction every 16 frames)
//   var34: The current "vertical half" of the screen it's moving toward
//   var35: Position the ghini is currently charging toward
// ==================================================================================================
void enemyCode70_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4594, 0x4596); goto normalStatus; } // jr z
  CYC(0x4594, 0x4596);
  CYC(0x4596, 0x4598); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x4598); return; } // ret c
  CYC(0x4598, 0x4599);
  if (!(F & FZ)) { CYCT(0x4599, 0x459b); goto normalStatus; } // jr nz
  CYC(0x4599, 0x459b);
  CYC(0x459b, 0x459e);
  enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CALL_C(0x459e, giantGhini_updateZPos_hook, 0x46da, 0x45a1);
  CYC(0x45a1, 0x45a3); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x45a3, 0x45a4); A = mem_rd(gb, DE);
  {
    CYC(0x45a4, 0x45a5); push_effect(gb, 0x45a5);
    uint16_t target = giantGhini_jump_table(gb);
    if (target == 0x45bb) { giantGhini_state_uninitialized_hook(gb); return; }
    if (target == 0x45de) { giantGhini_state_stub_hook(gb); return; }
    if (target == 0x45df) { giantGhini_state8_hook(gb); return; }
    if (target == 0x464f) { giantGhini_state9_hook(gb); return; }
    if (target == 0x46a2) { giantGhini_stateA_hook(gb); return; }
    HANDOFF(target);
  }
}

void giantGhini_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x45bb, 0x45bd); A = 0x70; // ENEMY_GIANT_GHINI
  CYC(0x45bd, 0x45bf); B = 0x00;
  CALL_C(0x45bf, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x45c2);
  CALL_C(0x45c2, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x45c5);
  CYC(0x45c5, 0x45c8); SET_BC(0x0040);
  CALL_C(0x45c8, objectSetSpeedZ_hook, 0x239d, 0x45cb);
  CYC(0x45cb, 0x45cd); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x45cd, 0x45cf); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x45cf, 0x45d1); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x45d1, 0x45d3); mem_wr(gb, HL, 0x78);
  CYC(0x45d3, 0x45d5); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x45d5, 0x45d7); mem_wr(gb, HL, 0xf8);
  CYC(0x45d7, 0x45d9); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x45d9, 0x45db); mem_wr(gb, HL, 0x10);
  CYC(0x45db, 0x45de);
  giantGhini_spawnChildren_hook(gb); return; // jp
}

void giantGhini_state_stub_hook(GB *gb) {
  RET(0x45de); return; // ret
}

// The ghini is spawning in before the fight starts
void giantGhini_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x45df, 0x45e0); E = alu_inc8(gb, E);
  CYC(0x45e0, 0x45e1); A = mem_rd(gb, DE);
  {
    CYC(0x45e1, 0x45e2); push_effect(gb, 0x45e2);
    uint16_t target = giantGhini_jump_table(gb);
    if (target == 0x45e8) goto substate0;
    if (target == 0x4601) goto substate1;
    if (target == 0x4615) goto substate2;
    HANDOFF(target);
  }

substate0:
  CYC(0x45e8, 0x45ea); A = 0x01; // DISABLE_LINK
  CYC(0x45ea, 0x45ed); W8(wDisabledObjects) = A;
  CYC(0x45ed, 0x45f0); W8(wMenuDisabled) = A;

  // Wait for door to close
  CYC(0x45f0, 0x45f3); A = W8(wcc93);
  CYC(0x45f3, 0x45f4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x45f4); return; } // ret nz
  CYC(0x45f4, 0x45f5);
  CYC(0x45f5, 0x45f7); A = 0x78;
  CYC(0x45f7, 0x45f9); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x45f9, 0x45fa); mem_wr(gb, DE, A);
  CYC(0x45fa, 0x45fb); E = alu_inc8(gb, E);
  CYC(0x45fb, 0x45fd); A = 0x1e;
  CYC(0x45fd, 0x45fe); mem_wr(gb, DE, A); // [counter2]
  CYC(0x45fe, 0x4601);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate1:
  CALL_C(0x4601, ecom_decCounter1_b0f_hook, 0x439a, 0x4604);
  if (!(F & FZ)) { RET_TAKEN(0x4604); return; } // ret nz
  CYC(0x4604, 0x4605);
  CYC(0x4605, 0x4607); mem_wr(gb, HL, 0x3c);
  CYC(0x4607, 0x4609); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4609, 0x460b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x460b, 0x460d); B = 0x01;
  CYC(0x460d, 0x460f); C = 0x0c;
  CALL_C(0x460f, enemyBoss_spawnShadow_b0f_hook, 0x4534, 0x4612);
  CYC(0x4612, 0x4615);
  ecom_incSubstate_b0f_hook(gb); return; // jp

substate2:
  // Flicker visibility
  CYC(0x4615, 0x4617); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x4617, 0x4618); A = mem_rd(gb, DE);
  CYC(0x4618, 0x461a); alu_xor(gb, 0x80);
  CYC(0x461a, 0x461b); mem_wr(gb, DE, A);
  CALL_C(0x461b, ecom_decCounter1_b0f_hook, 0x439a, 0x461e);
  if (!(F & FZ)) { RET_TAKEN(0x461e); return; } // ret nz
  CYC(0x461e, 0x461f);

  // Finally begin the fight
  CALL_C(0x461f, enemyBoss_beginMiniboss_b0f_hook, 0x4580, 0x4622);
  CALL_C(0x4622, objectSetVisible80_hook, 0x1e57, 0x4625);
  giantGhini_gotoState9_hook(gb); return; // fallthrough
}

void giantGhini_gotoState9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4625, 0x4626); alu_xor(gb, A);
  CALL_C(0x4626, enemySetAnimation_hook, 0x282b, 0x4629);
  CALL_C(0x4629, giantGhini_getTargetAngle_hook, 0x4711, 0x462c);
  CYC(0x462c, 0x462d); H = D;
  CYC(0x462d, 0x462f); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x462f, 0x4630); mem_wr(gb, DE, A);
  CYC(0x4630, 0x4632); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4632, 0x4634); mem_wr(gb, HL, 0x09);
  CYC(0x4634, 0x4636); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4636, 0x4638); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x4638, 0x463a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x463a, 0x463c); mem_wr(gb, HL, 0x02);
  CYC(0x463c, 0x463e); L = ENEMY_BASE + OBJ_VAR32;
  CYC(0x463e, 0x4640); mem_wr(gb, HL, 0x00);
  giantGhini_setChildRespawnTimer_hook(gb); return; // fallthrough
}

void giantGhini_setChildRespawnTimer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4640, getRandomNumber_hook, 0x043e, 0x4643);
  CYC(0x4643, 0x4645); alu_and(gb, 0x03);
  CYC(0x4645, 0x4647); C = 0x3c;
  CALL_C(0x4647, multiplyAByC_hook, 0x019d, 0x464a);
  CYC(0x464a, 0x464c); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x464c, 0x464d); A = L;
  CYC(0x464d, 0x464e); mem_wr(gb, DE, A);
  RET(0x464e); return; // ret
}

// "Normal" state during battle
void giantGhini_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x464f, 0x4651); E = ENEMY_BASE + OBJ_VAR32;
  CYC(0x4651, 0x4652); A = mem_rd(gb, DE);
  CYC(0x4652, 0x4653); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4653, 0x4655); goto beginCharge; } // jr nz
  CYC(0x4653, 0x4655);
  CALL_C(0x4655, enemyAnimate_hook, 0x2818, 0x4658);
  CALL_C(0x4658, objectApplySpeed_hook, 0x201d, 0x465b);
  CALL_C(0x465b, ecom_decCounter1_b0f_hook, 0x439a, 0x465e);
  if (!(F & FZ)) { CYCT(0x465e, 0x4660); goto skipNudge; } // jr nz
  CYC(0x465e, 0x4660);
  CYC(0x4660, 0x4662); mem_wr(gb, HL, 0x02);
  CALL_C(0x4662, giantGhini_getTargetAngle_hook, 0x4711, 0x4665);
  CALL_C(0x4665, objectNudgeAngleTowards_hook, 0x1fd4, 0x4668);

skipNudge:
  CALL_C(0x4668, ecom_decCounter2_b0f_hook, 0x43a3, 0x466b);
  if (!(F & FZ)) { RET_TAKEN(0x466b); return; } // ret nz
  CYC(0x466b, 0x466c);
  CALL_C(0x466c, giantGhini_setChildRespawnTimer_hook, 0x4640, 0x466f);
  CYC(0x466f, 0x4671); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4671, 0x4672); A = mem_rd(gb, DE);
  CYC(0x4672, 0x4673); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4673); return; } // ret nz
  CYC(0x4673, 0x4674);
  CALL_C(0x4674, getRandomNumber_hook, 0x043e, 0x4677);
  CYC(0x4677, 0x4679); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x4679, 0x467c);
    giantGhini_spawnChildren_hook(gb); return; // jp nz
  }
  CYC(0x4679, 0x467c);

beginCharge:
  CYC(0x467c, 0x467e); A = 0x01;
  CALL_C(0x467e, enemySetAnimation_hook, 0x282b, 0x4681);
  CALL_C(0x4681, ecom_incState_b0f_hook, 0x4000, 0x4684);
  CYC(0x4684, 0x4686); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x4686, 0x4688); mem_wr(gb, HL, 0x96);
  CYC(0x4688, 0x468a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x468a, 0x468c); mem_wr(gb, HL, 0x05); // SPEED_20
  giantGhini_updateChargeTargetPosition_hook(gb); return; // fallthrough
}

void giantGhini_updateChargeTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  // Get Link's position, save that as the position we're charging toward
  CYC(0x468c, 0x468f); SET_HL(w1Link_yh);
  CYC(0x468f, 0x4690); B = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4690, 0x4691); L = alu_inc8(gb, L);
  CYC(0x4691, 0x4692); A = mem_rd(gb, HL);
  CYC(0x4692, 0x4693); C = A;
  CALL_C(0x4693, getTileAtPosition_hook, 0x1447, 0x4696);
  CYC(0x4696, 0x4697); A = L;
  CYC(0x4697, 0x4698); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x4698, 0x469a); mem_wr(gb, DE, A);
  CALL_C(0x469a, objectGetAngleTowardLink_hook, 0x1e9c, 0x469d);
  CYC(0x469d, 0x469e); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x469e, 0x46a0); mem_wr(gb, DE, A);
  RET(0x46a0); return; // ret
}

// Charging toward Link
void giantGhini_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x46a2, enemyAnimate_hook, 0x2818, 0x46a5);
  CALL_C(0x46a5, ecom_decCounter2_b0f_hook, 0x43a3, 0x46a8);
  CYC(0x46a8, 0x46a9); A = mem_rd(gb, HL);
  CYC(0x46a9, 0x46ab); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x46ab, 0x46ad); goto applySpeed; } // jr nz
  CYC(0x46ab, 0x46ad);
  CYC(0x46ad, 0x46af); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x46af, 0x46b0); A = mem_rd(gb, HL);
  CYC(0x46b0, 0x46b2); alu_cp(gb, 0x78); // SPEED_300
  if (F & FZ) { CYCT(0x46b2, 0x46b4); goto applySpeed; } // jr z
  CYC(0x46b2, 0x46b4);
  CYC(0x46b4, 0x46b6); alu_add(gb, 0x05); // SPEED_20
  CYC(0x46b6, 0x46b7); mem_wr(gb, HL, A);

applySpeed:
  CALL_C(0x46b7, objectApplySpeed_hook, 0x201d, 0x46ba);
  CYC(0x46ba, 0x46bc); E = ENEMY_BASE + OBJ_VAR32;
  CYC(0x46bc, 0x46bd); A = mem_rd(gb, DE);
  CYC(0x46bd, 0x46be); alu_or(gb, A);
  if (F & FZ) { CYC(0x46be, 0x46c1); } else { CALL_C_CC(0x46be, giantGhini_updateChargeTargetPosition_hook, 0x468c, 0x46c1); } // call nz
  CYC(0x46c1, 0x46c3); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x46c3, 0x46c4); A = mem_rd(gb, DE);
  CALL_C(0x46c4, convertShortToLongPosition_hook, 0x20cb, 0x46c7);
  CYC(0x46c7, 0x46c9); E = ENEMY_BASE + OBJ_YH;
  CALL_C(0x46c9, objectGetRelativeAngle_hook, 0x1ea4, 0x46cc);
  CYC(0x46cc, 0x46ce); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x46ce, 0x46cf); mem_wr(gb, DE, A);
  CALL_C(0x46cf, objectGetTileAtPosition_hook, 0x1444, 0x46d2);
  CYC(0x46d2, 0x46d4); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x46d4, 0x46d5); A = mem_rd(gb, DE);
  CYC(0x46d5, 0x46d6); alu_cp(gb, L);
  if (F & FZ) { CYCT(0x46d6, 0x46d9); giantGhini_gotoState9_hook(gb); return; } // jp z
  CYC(0x46d6, 0x46d9);
  RET(0x46d9); return; // ret
}

void giantGhini_updateZPos_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46da, 0x46dc); C = 0x00;
  CALL_C(0x46dc, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x46df);
  CYC(0x46df, 0x46e1); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x46e1, 0x46e2); A = mem_rd(gb, HL);
  CYC(0x46e2, 0x46e3); A = alu_dec8(gb, A);
  CYC(0x46e3, 0x46e4); mem_wr(gb, HL, A);
  if (!(F & FZ)) { RET_TAKEN(0x46e4); return; } // ret nz
  CYC(0x46e4, 0x46e5);
  CYC(0x46e5, 0x46e7); A = 0x10;
  CYC(0x46e7, 0x46e8); mem_wr(gb, HL, A);
  CYC(0x46e8, 0x46ea); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x46ea, 0x46eb); A = mem_rd(gb, HL);
  CYC(0x46eb, 0x46ec); alu_cpl(gb);
  CYC(0x46ec, 0x46ed); A = alu_inc8(gb, A);
  CYC(0x46ed, 0x46ee); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x46ee, 0x46ef); A = mem_rd(gb, HL);
  CYC(0x46ef, 0x46f0); alu_cpl(gb);
  CYC(0x46f0, 0x46f1); mem_wr(gb, HL, A);
  RET(0x46f1); return; // ret
}

void giantGhini_spawnChildren_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x46f2, 0x46f4); C = 0x03;

nextChild:
  CYC(0x46f4, 0x46f6); B = 0x3f; // ENEMY_GIANT_GHINI_CHILD
  CALL_C(0x46f6, ecom_spawnEnemyWithSubid01_b0f_hook, 0x4373, 0x46f9);
  if (!(F & FZ)) { RET_TAKEN(0x46f9); return; } // ret nz
  CYC(0x46f9, 0x46fa);
  CYC(0x46fa, 0x46fc); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x46fc, 0x46fd); A = mem_rd(gb, DE);
  CYC(0x46fd, 0x46fe); A = alu_inc8(gb, A);
  CYC(0x46fe, 0x46ff); mem_wr(gb, DE, A);

  // [child.subid] = [this.subid] | index
  CYC(0x46ff, 0x4701); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4701, 0x4702); A = mem_rd(gb, DE);
  CYC(0x4702, 0x4703); alu_or(gb, C);
  CYC(0x4703, 0x4704); mem_wr(gb, HL, A);
  CYC(0x4704, 0x4706); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x4706, 0x4708); A = ENEMY_BASE; // Enemy.start
  CYC(0x4708, 0x4709); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4709, 0x470a); mem_wr(gb, HL, D);
  CALL_C(0x470a, objectCopyPosition_hook, 0x2242, 0x470d);
  CYC(0x470d, 0x470e); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x470e, 0x4710); goto nextChild; } // jr nz
  CYC(0x470e, 0x4710);
  RET(0x4710); return; // ret
}

// Decides on a position to move towards, for state 9 ("normal" state). It will target
// the horizontal center of the screen, with the Y-position one quarter away from the
// screen boundary (depends which side Link is on). The camera affects the target position.
//
// When Link moves beyond the half-screen boundary, the ghini recalculates its angle to
// face directly away from Link before it slowly moves toward him again.
//
// @param[out] a angle
void giantGhini_getTargetAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
retry:
  CYC(0x4711, 0x4713); A = hram_rd(gb, hCameraY & 0xff);
  CYC(0x4713, 0x4714); C = A;
  CYC(0x4714, 0x4717); A = W8(w1Link_yh);
  CYC(0x4717, 0x4718); alu_sub(gb, C);
  CYC(0x4718, 0x471a); B = 0x28; // ((SCREEN_HEIGHT/4)<<4) + 8
  CYC(0x471a, 0x471c); alu_cp(gb, 0x48); // ((SCREEN_HEIGHT/2)<<4) + 8
  if (!(F & FC)) { CYCT(0x471c, 0x471e); goto haveHalf; } // jr nc
  CYC(0x471c, 0x471e);
  CYC(0x471e, 0x4720); B = 0x68; // ((SCREEN_HEIGHT*3/4)<<4) + 8

haveHalf:
  CYC(0x4720, 0x4722); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x4722, 0x4723); A = mem_rd(gb, DE);
  CYC(0x4723, 0x4724); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x4724, 0x4726); goto haveAngle; } // jr z
  CYC(0x4724, 0x4726);

  // Link changed sides on the screen boundary
  CYC(0x4726, 0x4727); A = B;
  CYC(0x4727, 0x4728); mem_wr(gb, DE, A); // [var34]
  CALL_C(0x4728, objectGetAngleTowardLink_hook, 0x1e9c, 0x472b);
  CYC(0x472b, 0x472d); alu_xor(gb, 0x10);
  CYC(0x472d, 0x472f); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x472f, 0x4730); mem_wr(gb, DE, A);
  CYC(0x4730, 0x4732); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4732, 0x4734); A = 0x0a;
  CYC(0x4734, 0x4735); mem_wr(gb, DE, A);
  CYC(0x4735, 0x4737);
  goto retry; // jr

haveAngle:
  CYC(0x4737, 0x4738); A = C;
  CYC(0x4738, 0x4739); alu_add(gb, B);
  CYC(0x4739, 0x473a); B = A;
  CYC(0x473a, 0x473c); A = hram_rd(gb, hCameraX & 0xff);
  CYC(0x473c, 0x473e); alu_add(gb, 0x50); // (SCREEN_WIDTH/2)<<4
  CYC(0x473e, 0x473f); C = A;
  CYC(0x473f, 0x4742);
  objectGetRelativeAngle_hook(gb); return; // jp
}
